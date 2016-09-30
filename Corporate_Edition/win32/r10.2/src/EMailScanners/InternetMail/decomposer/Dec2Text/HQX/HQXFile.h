// HQXFile.h : Decomposer 2 Archive class for HQX files.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************

#if !defined(HQXFILE_H)
#define HQXFILE_H

#define HQX_HEADER_BYTES_NEEDED 120

#define HQX_OUTPUT_BUFFER_SIZE	32 * 1024
#define HQX_ENCODE_BUFFER_SIZE	4  * 1024
#define HQX_ERROR_EOF			EOF
#define HQX_SUCCESS				0
#define HQX_ERROR_GENERIC		1 
#define HQX_ERROR_MAX_EXTRACT	2
#define HQX_ERROR_ABORT			3

#define HQX_FORK_UNKNOWN		0
#define HQX_FORK_DATA			1
#define HQX_FORK_RESOURCE		2

#define LEN_MAX_LINE_LENGTH		64
#define LEN_MAX_LOOKAHEAD_SIZE	(LEN_MAX_LINE_LENGTH * 4)
#define LEN_HQX_IDENTIFIER		20		// Length of "(This file must be converted with BinHex"
#define LEN_DECODED_CHAR_BUFF	3
#define LEN_ENCODED_CHAR_BUFF	4
#define LEN_LOOKUP_TABLE		83
#define LEN_ENCODE_TABLE		96
#define LEN_HQX_IDENTIFIER_TOT	49
#define RLE_DELIMITER			0x90

#define CRC_SEED				0x1021

#define MAX_NAME_LEN			63

//
// Since we only have to track two forks, we can do it easily with a simple
// bitmask rather than some elaborate tracking mechanism.
//
#define MODIFY_DATA_CHANGE		0x1
#define MODIFY_DATA_REPLACE		0x2
#define MODIFY_DATA_DELETE		0x4
#define MODIFY_RESOURCE_CHANGE	0x8
#define MODIFY_RESOURCE_REPLACE 0x10
#define MODIFY_RESOURCE_DELETE	0x20
#define MODIFY_RENAME_ITEM		0x40

//
// This structure describes the HQX header found in every valid HQX file.
//
typedef struct tagHQXHEADER
{
	BYTE byFilenameLen;						// Length of the filename (not including the final NULL)
	char szFilename[MAX_NAME_LEN + 1];		// Original filename
	long lFileType;							// File type
	long lCreator;							// Creator
	WORD wFlags;							// Flags
	long lDataForkLength;					// Length of data fork - this is the unencoded length
	long lResourceForkLength;				// Length of resource fork - this is the unencoded length
} HQXHEADER, *LPHQXHEADER;


// Decomposer-specific control functions.
void HQXSetMaxExtractSize(DWORD dwMaxSize);
void HQXAbortProcess(bool bAbort);


/////////////////////////////////////////////////////////////////////////
// CHQXFile

class CHQXFile
{
public:
	CHQXFile(bool bEnforceCRC);
	~CHQXFile();

	DECRESULT Open(FILE *fpin, size_t nOffset);
	DECRESULT Close(char *pszOutputFile);
	DECRESULT GetEntryName(int iFork, char *pszName, WORD wNameSize);
	DECRESULT GetEntryUncompressedSize(int iFork, DWORDLONG *dwlSize);
	DECRESULT GetEntryCompressedSize(int iFork, DWORDLONG *dwlSize);
	DECRESULT ExtractFork(int iFork, const char *pszFile);
	DECRESULT DeleteEntry(int iFork);
	DECRESULT ReplaceEntry(int iFork, const char *pszNewDataFile, const char *pszNewEntryName);
	DECRESULT ExtractBothForks(FILE *fp,
							  size_t nOffset,
							  const char *pszDataForkChildDataFile, 
							  const char *pszResourceForkChildDataFile,
							  DECRESULT &hrDataFork,
							  DECRESULT &hrResourceFork,
							  size_t *pnEndOffset);

	bool DecodeHeader(char **pphdr, size_t *pnBufferSize);

private:
	bool CommitChanges(char *pszOutputFile);        // Perform any change/replace/delete operations needed

	bool DecodeHeader();							// Decode the BinHex header

	bool ConvertHeaderToBuffer(HQXHEADER hqxHeader, // Create a linear byte buffer from the header
							  unsigned char **ppszHeaderBuff,
							  long &lLengthOfHeader);

	int GetNextDecodedByte(bool bComputeCRC = true);// Retrieve the next decoded byte
	int GetLong(long *plValue);                     // Retrieve a decoded 'long' from the input stream
	int GetWord(WORD *pwValue, 
				bool bComputeCRC = true);			// Retrieve a decoded 'WORD' from the input stream    

	int GetNextDecodedByte(char **pptr, size_t *pnBufferSize, bool bComputeCRC = true);
	int GetLong(char **pptr, size_t *pnBufferSize, long *plValue);
	int GetWord(char **pptr, size_t *pnBufferSize, WORD *pwValue, bool bComputeCRC = true);
	bool WriteQuad(char *pszQuad);
	
	int DecodeFork(int iFork, const char *pszFile); // Decode a fork to the given file    
	
	int EncodeHeaderToBinHex(HQXHEADER hqxHeader,	// Encode a header to BinHex
							unsigned char *pucTri,
							int &nCount);
	int EncodeFileToBinHex(const char *pszFileToEncode, // Encode a file to BinHex
						  unsigned char *pucTri,
						  int &nCount);

	void ComputeCRC(unsigned int uChar);			// Keep a running CRC
	bool ComputeBinHexValues(char *lpszQuad);       // Encode the given 8bit values into 6bit codes
	bool ComputeAsciiValues(char *lpszQuad);        // Encode the given 6bit values into 8bit codes
   
	bool DecodeQuadToTri(char *pszQuad,
						unsigned char *puchTri);   // Decode a single BinHex quad
	bool EncodeTriToQuad(unsigned char *puchTri, 
						char *pszQuad);            // Encode a single plaintext tri
	
	int FillInputBuffer();							// Fill the input buffer (6bit codes)
	int FillInputBuffer(char **pptr, size_t *pnBufferSize);
	bool CompressBuffer(unsigned char *puchInput,   // Fill a lookahead buffer with puchInput as the source and
						int nSizeInput,				// perform RLE on it.
						unsigned char *pucOutputBuffer,
						int &nSizeOfOutput);
	
	int CreateNewBinHexBody(HQXHEADER hqxHeader,	// Construct a new BinHex body from the given input
							const char *pszDataFile,
							const char *pszRscFile);

	inline int SafeGetChar(unsigned char *puchInput,
						  int &nIndexInBuffer, 
						  int nSizeOfBuffer, 
						  bool &bEndOfBuffer); 

	bool GetFileLength(const char *pszFilename, long *plLength);

	char m_szQuad[LEN_ENCODED_CHAR_BUFF + 1];					// Holds 4 6bit encoded characters
	unsigned char m_uchTri[LEN_DECODED_CHAR_BUFF + 1];			// Holds 3 8bit decoded characters
	int m_nIndexIntoDecodedBuffer;								// Index into m_uchTri

	size_t	m_nCount;
	size_t	m_nEndOffset;
	bool	m_bEnforceCRC;				  // Client-controlled option - enforce CRC checks?
	bool	m_bChanged;					  // Are there changes to make?
	FILE	*m_pInputFile;                 // Input file
	FILE	*m_pOutputFile;                // Output file
	WORD	m_wCrc;						  // Running CRC on input (one for the header, and one for each fork)
	bool	m_bRealEOF;					  // Have we actually hit the real EOF?
	int		m_nLastCharacter;			  // Keeps track of the last character read (used for RLE encoding)
	bool	m_bInRLESection;			  // Are we in a RLE encoded section?
	int		m_nNumberOfRepeatedCodes;	  // How many time to repeat m_nLastCharacter (used for RLE encoding)
	HQXHEADER m_hqxHeader;				  // Holds HQX header
	char	m_sReplacementDataFile[MAX_PATH];	// File to use when replacing the data fork
	char	m_sReplacementResourceFile[MAX_PATH];// File to use when replacing the resource fork
	char	m_sNewEntryName[MAX_PATH];			// New entry name if desired during replace operation
	char	*m_pszDataForkFile;            // File that contains the data fork.
	char	*m_pszResourceForkFile;        // File that contains the resources fork.
	char	*m_pszOutputFile;              // Output filename
	DWORD	m_dwPendingChanges;			  // Bitmask that identifies any pending changes
};

#endif	// HQXFILE_H

