// UUE.h : Header for primary UUE engine APIs
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

//
// Note: The AS400 build system performs its builds one directory
// level higher than the other platforms.  It is easier to change
// just the specific AS400 cases rather than change every workspace's
// include search path.
//
#if defined(OS400)
	#include "TextFile.h"
#else
	#include "../TextFile.h"
#endif

#include "asc_char.h"

#define MAX_UUE_TOKEN_SIZE				512
#define UU_MAX_OCTETS_PER_LINE			45
#define RFC822_MAX_LINE_LENGTH			1000

#define TERMINATOR_NONE					0
#define TERMINATOR_GRAVE				1
#define TERMINATOR_END					2

typedef struct tagUUEEmitToken
{
	int					iTokenID;
	size_t				nTokenOffset;
	unsigned long		dwFlags;
} UUEEMITTOKEN, *PUUEEMITTOKEN;


class CUUEParser;  // Forward-reference

class CUUELexer : public ILexer
{
public:
	CUUELexer(DWORD dwMaxScanBytes);
	~CUUELexer();

	// ILexer methods:
	bool	Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject);
	int		GetInterface(void **ppInterface);
	int		ConsumeByte(unsigned char uchByte);
	int     ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset);
	int		ConsumeEOF(size_t nOffset);
	int		LexComplete(void);
	int		ProcessComplete(void);
	int		FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData);
	int		FindNextEntry(TEXT_FIND_DATA *pData);
	int		SkipExtractEntry(TEXT_FIND_DATA *pData);
	int		ExtractEntry(char *pszFilename, FILE *fpin);
	int		ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile);
	int		AddEntry(TEXT_CHANGE_DATA *pData);
	int		DeleteEntry(TEXT_CHANGE_DATA *pData);
	

private:
	inline int		EmitToken(int iTokenID);

	CUUEParser		*m_pParser;
	unsigned char	m_byBuffer[MAX_UUE_TOKEN_SIZE];
	size_t			m_nLexemeOffset;
	size_t			m_nStartOffset;
	size_t			m_nForwardOffset;
	size_t			m_nLineNumber;	// current line number
	size_t			m_nOffset;
	int				m_State;
	int				m_iNextStartStateIndex;
	bool			m_bIdentified;

	//
	// Variables controlled by the SetOption API
	//
	DWORD m_dwMaxScanBytes;
	DWORD m_dwTextNonUUEThreshold;
};


class CUUEParser
{
public:
	CUUEParser();
	~CUUEParser();

	bool	Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer);
	int		ConsumeToken(int iTokenID, char *pString, size_t nStringLength, size_t nOffset);
	int		Close(void);
	int		FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData);
	int		FindNextEntry(TEXT_FIND_DATA *pData);
	int		SkipExtractEntry(TEXT_FIND_DATA *pData);
	int		ExtractEntry(char *pszFilename, FILE *fpin);
	int		ReplaceEntry(TEXT_CHANGE_DATA *pData);
	int		AddEntry(TEXT_CHANGE_DATA *pData);
	int		DeleteEntry(TEXT_CHANGE_DATA *pData);
	inline bool IsModified() { return m_bChanged; };
	inline bool SetModified(bool bNew = true)
	{ bool bTemp = m_bChanged; m_bChanged = bNew; return bTemp; };

private:
	// From UUEncode.cpp:
	bool IsUUECandidate(char *lpszLine, DWORD *pdwMode, char *lpszAttachFileName, DWORD dwNameSize);
	bool IsValidUUEContainer(bool* bIdentified, bool fStrongHeader, const unsigned char* szLine, unsigned int nLineLength, int* nReturnCode );
	int  DecodeUU(FILE *lpstInput, size_t nStartOffset, FILE *lpstOutput);
	int  SkipDecodeUU(FILE *lpstInput, size_t nStartOffset);
	int  EncodeUU(FILE *lpstInput, FILE *lpstOutput, const char *lpszFileName);
	int  WriteUU(FILE *lpstFile, size_t nStartOffset, const char *lpszFileName);
	int  ReadLine(FILE *lpstFile, char *lpszData, int nBufferSize,
					LPINT lpnLineLength, bool *lpbGotEOL);
	bool IsUULineValid( const unsigned char* szBuffer, const size_t nBytesInMultiLineBuffer, FILE* lpstInput );
	bool GetFirstLine( unsigned char* szBuffer, long nLengthIn, long* nLineLenOut );
	void ReadAhead( FILE* lpstInput, unsigned char* szLine, const size_t nBytesInMultiLineBuffer, long* nLineLen, unsigned long nBytesToAlloc, bool* pfSuccess );

	// UUE state machine functions
	inline int State_HEADERLINE1(void);
	inline int State_HEADERLINE2(void);
	inline int State_BEGIN1(void);
	inline int State_BEGIN2(void);
	inline int State_BEGIN3(void);
	inline int State_BEGIN4(void);
	inline int State_BODY1(void);
	inline int State_END1(void);
	inline int State_END2(void);
	inline int State_TABLE1(void);
	inline int State_TABLE2(void);
	inline int State_TABLE3(void);
	inline int State_SECTION1(void);
	inline int State_SECTION2(void);
	inline int State_SUM1(void);
	inline int State_SUM2(void);
	inline int State_SINGLESPACE1(void);
	inline int State_DOUBLESPACE(void);
	inline int State_OCTALNUMBER(void);
	inline int State_SINGLESPACE2(void);
	inline int State_FILENAME(void);
	inline int State_FORMATERROR(void);

	IDecomposerEx *m_pDecomposer;
	CTextEngine *m_pEngine;
	CTextFile	*m_pArchive;

	bool	m_bIdentified;
	int		m_iTokenID;
	int		m_ParseState;
	char	*m_pString;
	size_t	m_nStringLength;
	size_t	m_nLineNumber;
	size_t	m_nSectionStart;	// Used for extraction
	size_t	m_nStartOffset;		// Used for extraction
	size_t	m_nTerminator;

	// Variables used for decoding
	bool	m_bNewLine;
	size_t	m_nEncodedLineLength;
	unsigned char m_byQuad[4];
	unsigned char m_byTri[3];
	unsigned char m_uulookup[256];
	size_t	m_nTableIndex;
	size_t	m_nQuadIndex;

	FILE	*m_pFile;
	char	m_szOutFile[MAX_PATH];
	char	m_szFilename[MAX_PATH];
	bool	m_bChanged;
	size_t	m_nOffset;
	size_t	m_nHeaderOffset;
	size_t	m_nEndOffset;
	size_t	m_nTopUnchangedOffset;
	size_t	m_nBottomUnchangedOffset;
	DWORD	m_dwMode;
	bool	m_fStrongHeader;
	bool    m_fValidateUUEBody;
	int     m_fNumValidUUELines;

	//
	// Variables controlled by the SetOption API
	//
	DWORD m_dwTextNonUUEThreshold;
};


enum UUELexerTokens
{
// Start of lexer tokens
	UUE_TOKEN_LWSP = 0,						// 0
	UUE_TOKEN_NEWLINE,						// 1
	UUE_TOKEN_BEGIN,						// 2
	UUE_TOKEN_END,							// 3
	UUE_TOKEN_SECTION,						// 4
	UUE_TOKEN_SUM,							// 5
	UUE_TOKEN_TABLE,						// 6
	UUE_TOKEN_ATOM,							// 7
	UUE_TOKEN_EOF,							// 8
// End of lexer tokens
// Start of parser tokens
	UUE_TOKEN_END_MARKER,					// 9
	UUE_TOKEN_MODE,							// 10
	UUE_TOKEN_FILENAME,						// 11
	UUE_TOKEN_BODY,							// 12

// End of parser tokens
	UUE_TOKEN_LAST							// 12
};


enum UUELexerStates
{
	UUE_LEXER_STATE_CRLF1 = 0,
	UUE_LEXER_STATE_CRLF2,
	UUE_LEXER_STATE_CRLF3,
	UUE_LEXER_STATE_CRLF4,
	UUE_LEXER_STATE_LF1,
	UUE_LEXER_STATE_LF2,
	UUE_LEXER_STATE_LWSP1,
	UUE_LEXER_STATE_LWSP2,
	UUE_LEXER_STATE_ATOM1,
	UUE_LEXER_STATE_ATOM2,
	UUE_LEXER_STATE_COUNT
};


enum UUEParserStates
{
	UUE_PARSER_STATE_HEADERLINE1 = 0,
    UUE_PARSER_STATE_HEADERLINE2,
	UUE_PARSER_STATE_BEGIN1,
	UUE_PARSER_STATE_BEGIN2,
	UUE_PARSER_STATE_BEGIN3,
	UUE_PARSER_STATE_BEGIN4,
	UUE_PARSER_STATE_BODY1,
	UUE_PARSER_STATE_END1,
	UUE_PARSER_STATE_END2,
	UUE_PARSER_STATE_TABLE1,
	UUE_PARSER_STATE_TABLE2,
	UUE_PARSER_STATE_TABLE3,
	UUE_PARSER_STATE_SECTION1,
	UUE_PARSER_STATE_SECTION2,
	UUE_PARSER_STATE_SUM1,
	UUE_PARSER_STATE_SUM2,
	UUE_PARSER_STATE_SINGLESPACE1,
	UUE_PARSER_STATE_DOUBLESPACE,
	UUE_PARSER_STATE_OCTALNUMBER,
	UUE_PARSER_STATE_SINGLESPACE2,
	UUE_PARSER_STATE_FILENAME,
	UUE_PARSER_STATE_FORMATERROR,
	UUE_PARSER_STATE_LAST
};


// ASCII constants used below. This code must be able to run on machines whose
// native character set is not ASCII. Even on such machines, though, the data
// that we are parsing is always ASCII. In order to make such a thing work, we
// must treat the message data as binary rather than text. All comparisons are
// against binary constants that we know to be in ASCII, defined here. For case
// insensitive comparisons we use both upper and lowercase versions of the
// constant along with a special function called binary_stricmp.

#define STR_begin	"\x62\x65\x67\x69\x6E"
#define STR_BEGIN	"\x42\x45\x47\x49\x4E"
#define STR_end		"\x65\x6E\x64"
#define STR_END		"\x45\x4E\x44"
#define STR_table	"\x74\x61\x62\x6C\x65"
#define STR_TABLE	"\x54\x41\x42\x4C\x45"
#define STR_section "\x73\x65\x63\x74\x69\x6F\x6E"
#define STR_SECTION "\x53\x45\x43\x54\x49\x4F\x4E"
#define STR_sum		"\x73\x75\x6D"
#define STR_SUM		"\x53\x55\x4D"

inline bool IsValidOctalChar(char ch)
{
	if ((ch < ASC_CHR_0) || (ch > ASC_CHR_7))
		return false;

	return true;
}
