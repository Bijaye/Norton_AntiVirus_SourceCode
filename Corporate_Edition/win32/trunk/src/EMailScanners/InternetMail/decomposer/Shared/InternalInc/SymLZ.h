// SymLZ.h - LZ compression/decompression routines (SZDD format)
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2002, 2005 Symantec, Corp.. All rights reserved.

#include "DecPlat.h"
#include "Dec2.h"
#include "asc_char.h"

#include "dec_assert.h"

// Declared in SymLZ.cpp
extern volatile DWORD g_dwLZMaxExtractSize;
extern volatile bool g_bLZAbort;

//
// Platform specific path separator defs
//
#if !defined(LZ_NATIVE_PATH_SEPARATOR)
	#if defined(_WINDOWS) || defined(SYM_NLM)
		#define LZ_NATIVE_PATH_SEPARATOR '\\'
	#elif defined(UNIX)
		#define LZ_NATIVE_PATH_SEPARATOR '/'
	#else
		#error Your platform is not supported!
	#endif
#endif

#if !defined(LZ_ASC_PATH_SEPARATOR)
	#if defined(_WINDOWS) || defined(SYM_NLM)
		#define LZ_ASC_PATH_SEPARATOR ASC_CHR_BSLASH
	#elif defined(UNIX)
		#define LZ_ASC_PATH_SEPARATOR ASC_CHR_FSLASH
	#else
		#error Your platform is not supported!
	#endif
#endif

// 
// The readahead length of the decompressor. Reading single bytes
// using _lread() would be SLOW.
//
#define LZ_BUFFER_SIZE	(1024 * 4)

//
// LZ table size
//
#define LZ_TABLE_SIZE	0x1000

//
// LZ magic header bytes
//
static BYTE LZMagic[8] = { ASC_CHR_S, ASC_CHR_Z, ASC_CHR_D, ASC_CHR_D, 0x88, 0xf0, 0x27, 0x33 };

//
// Pack on disk structure
//
#if defined(IBMCXX)
	#pragma option align=packed	 // AIX
#else
	#pragma pack(1)				 // WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif

//
// Format of first 14 byte of LZ compressed file
//
struct LZ_HEADER
{
	BYTE  bySig[8];			 // 53 5A 44 44 88 F0 27 33
	BYTE  byMethod;			 // 41 or above
	BYTE  byOrigChar;		 // 00 or letter removed from extension & replaced with '_'
	DWORD dwOrigsize;		 // Original file size
};

#if defined(IBMCXX)
	#pragma option align=reset	// AIX
#else
	#pragma pack()				// WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif


//
// State info for current LZ file
//
class CLZState 
{
public:
	CLZState(FILE *pFile, BYTE byOrigChar)
	{
		// Initialize variables
		m_pFile = pFile;
		m_byOrigChar = byOrigChar;

		m_dwRealCurrent = 0;
		m_dwRealWanted = 0;
		m_byStringLen = 0;
		m_dwStringPos = 0;
		m_wByteType = 0;
		m_pbyGet = NULL;
		m_dwGetCur = 0;
		m_dwGetLen = 0;
	 
		// Yes, preinitialize with spaces 
		memset(m_byTable, ASC_CHR_SPACE, LZ_TABLE_SIZE);
		
		// Yes, start 16 byte from the END of the table 
		m_uCurrentTableEntry  = 0xFF0; 
	}

	~CLZState()
	{
		if (m_pbyGet)
		{
			free(m_pbyGet);
			m_pbyGet = NULL;
		}
	}

	FILE	*m_pFile;                   // The file handle
	UINT	m_uCurrentTableEntry;		// Current table entry    
	WORD	m_wByteType;				// The bitmask within blocks    
	DWORD	m_dwRealCurrent;			// The position the decompressor currently is
	DWORD	m_dwRealWanted;				// The position the user wants to read from    
	DWORD	m_dwGetCur;					// The current read
	DWORD	m_dwStringPos;				// From stringtable        
	DWORD	m_dwGetLen;					// The length last got
	BYTE	m_byTable[LZ_TABLE_SIZE];	// The rotating LZ table
	BYTE	*m_pbyGet;                  // LZ_BUFFER_SIZE bytes
	BYTE	m_byStringLen;				// The length and position of current string 
	BYTE	m_byOrigChar;				// The last char of the filename    
};


class CSymLZ
{
public:
	CSymLZ()
	{
		m_dwUncompressedSize = 0;
		m_dwCompressedSize = 0;
		m_lzState = NULL;
	}

	~CSymLZ()
	{
		// Make sure to clean up
		Close();
	}

	// Public interface
	DECRESULT	OpenLZ(IDecContainerObjectEx *pObject, const char *szFile);
	DECRESULT	FormExpandedName(const char *szFile, char *pszExpandedFile);
	DECRESULT	ExpandToFile(const char *pszOutputFile);
	int			LZCompress(const char *pszInputName, const char *pszOutputName);

	DWORD m_dwUncompressedSize;
	DWORD m_dwCompressedSize;

private:
	// Internal interface to low level LZ functions
	DECRESULT	Init(FILE *pFile);
	size_t		LZDecompressBuffer(BYTE *pbyBuffer, size_t sizeOfBuffer);
	void		Close();

	// Buffering function.  Returns 1 for success, anything
	// else for failure.
	inline DECRESULT GetNextByte(BYTE &byByte) 
	{
		// Sanity check
		dec_assert(m_lzState);

		//
		// Since we're already using the CRT buffered I/O routines,
		// we may get no benefit from this second level of buffering,
		// especially on some platforms (AS400 or S/390 for example).
		// Define this preprocessor directive to turn off the second
		// level buffering.
		//
		// #define NO_SECOND_LEVEL_LZ_BUFFER
		//
	#if defined(NO_SECOND_LEVEL_LZ_BUFFER)
		int nNextByte = dec_fgetc(m_lzState->m_pFile);

		if (EOF == nNextByte)
			return DECERR_CHILD_ACCESS;

		byByte = (BYTE)nNextByte;
		return DEC_OK;
	#else
		// Is anything left in the buffer?
		if (m_lzState->m_dwGetCur < m_lzState->m_dwGetLen) 
		{
			byByte = m_lzState->m_pbyGet[m_lzState->m_dwGetCur++];
			return DEC_OK;
		} 
	
		// No, refill the buffer
		else
		{
			size_t sizeBytesRead = dec_fread(m_lzState->m_pbyGet, sizeof(BYTE), LZ_BUFFER_SIZE, m_lzState->m_pFile);
		
			if (0 == sizeBytesRead)
			{
				// It doesn't matter if it was EOF or an error, 
				// we never use these results except internally.
				return DECERR_CONTAINER_ACCESS;
			}
		
			m_lzState->m_dwGetLen = sizeBytesRead;
			m_lzState->m_dwGetCur = 1;
			byByte = *(m_lzState->m_pbyGet);

			return DEC_OK;
		}
	#endif
	}

	int		NodeInsert(int i, int run);
	void	NodeDelete(int z);
	int		ReadByte(FILE *fp);

	CLZState	*m_lzState;
	char		*m_buffer;
	int			*m_node;
	int			m_pos;
};


// Decomposer-specific control functions.
inline void LZSetMaxExtractSize(DWORD dwMaxSize)
{
	g_dwLZMaxExtractSize = dwMaxSize;
}


inline void LZAbortProcess(bool bAbort)
{
   g_bLZAbort = bAbort;
}
