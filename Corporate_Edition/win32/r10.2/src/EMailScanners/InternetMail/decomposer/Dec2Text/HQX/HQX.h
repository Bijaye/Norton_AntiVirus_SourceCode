// HQX.h : Header for primary HQX engine APIs
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
	#include "HQX/HQXFile.h"
#else
	#include "../TextFile.h"
	#include "HQXFile.h"
#endif

#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"

#define MAX_HQX_TOKEN_SIZE		512

#define EMIT_OFFSET				m_emit.nTokenOffset
#define EMIT_DATA				m_emit.nTokenData
#define EMIT_ID					m_emit.iTokenID
#define EMIT_FLAGS				m_emit.dwFlags
#define DO_EMIT					if (!EmitToken()) return (PARSER_ERROR)


typedef struct tagHQXEmitToken
{
	size_t			iTokenID;
	size_t			nTokenOffset;
	size_t			nTokenData;
	unsigned long	dwFlags;
} HQXEMITTOKEN, *PHQXEMITTOKEN;

//
// Forward defines
//
class CHQXParser;

class CHQXLexer : public ILexer
{
public:
	CHQXLexer(DWORD dwMaxScanBytes);
	~CHQXLexer();

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

	CHQXParser		*m_pParser;
	unsigned char	m_byBuffer[MAX_HQX_TOKEN_SIZE];
	size_t			m_nLexemeOffset;
	size_t			m_nStartOffset;
	size_t			m_nOffset;
	int				m_State;
	int				m_iNextStartStateIndex;
	bool			m_bInBody;
	bool			m_bIdentified;

	//
	// Variables controlled by the SetOption API
	//
	DWORD m_dwTextNonHQXThreshold;
	DWORD m_dwMaxScanBytes;
};


class CHQXParser
{
public:
	CHQXParser();
	~CHQXParser();

	bool	Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer);
	int		ConsumeToken(int iTokenID, char *pString, size_t nStringLength, size_t nOffset);
	int		FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData);
	int		FindNextEntry(TEXT_FIND_DATA *pData);
	int		ExtractEntry(char *pszFilename, FILE *fpin);
	int		ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile);
	int		AddEntry(TEXT_CHANGE_DATA *pData);
	int		DeleteEntry(TEXT_CHANGE_DATA *pData);
	int		ProcessComplete(void);

private:
	inline bool EmitToken(void);
	int		ExtractForks(TEXT_FIND_DATA *pData);

	// BinHex state machine functions
	inline int State_BODY1(void);
	inline int State_BODY2(void);

	IDecomposerEx	*m_pDecomposer;
	CTextEngine		*m_pEngine;
	CTextFile		*m_pArchive;
	CHQXFile		*m_pHQXArchive;

	FILE			*m_fpArchive;
	int				m_iTokenID;
	int				m_ParseState;
	int				m_iCurrentFork;
	DECRESULT		m_hrLastDataForkErr;
	DECRESULT		m_hrLastResourceForkErr;
	char			*m_pString;
	char			*m_pszName;
	char			*m_pszDataForkName;
	char			*m_pszResourceForkName;
	char			*m_pszReplacementName;
	size_t			m_nStringLength;
	size_t			m_nOffset;
	size_t			m_nBodyStartOffset;
	size_t			m_nBodyEndOffset;
	unsigned long	m_dwType;
	unsigned long	m_dwFlags;
	unsigned long	m_dwYear;
	unsigned long	m_dwMonth;
	unsigned long	m_dwDay;
	unsigned long	m_dwHour;
	unsigned long	m_dwMinute;
	unsigned long	m_dwSecond;
	HQXEMITTOKEN	m_emit;
};


enum HQXLexerTokens
{
// Start of lexer tokens
	HQX_TOKEN_BODY_START = 0,				// 0
	HQX_TOKEN_BODY_DATA,					// 1
	HQX_TOKEN_BODY_END,						// 2
	HQX_TOKEN_EOF,							// 3
// End of lexer tokens
// Start of parser tokens
// End of parser tokens
	HQX_TOKEN_LAST							// 4
};


enum HQXLexerStates
{
	HQX_LEXER_STATE_COLON1 = 0,
	HQX_LEXER_STATE_BODY1,
	HQX_LEXER_STATE_COUNT
};


enum HQXParserStates
{
	HQX_PARSER_STATE_BODY1 = 0,
	HQX_PARSER_STATE_BODY2,
	HQX_PARSER_STATE_LAST
};


// ASCII constants used below. This code must be able to run on machines whose
// native character set is not ASCII. Even on such machines, though, the data
// that we are parsing is always ASCII. In order to make such a thing work, we
// must treat the message data as binary rather than text. All comparisons are
// against binary constants that we know to be in ASCII, defined here. For case
// insensitive comparisons we use both upper and lowercase versions of the
// constant along with a special function called binary_stricmp.

// (this file must be converted with binhex
static const char gszBinHexIdL[] =
{
	ASC_CHR_LPAREN,
	ASC_CHR_t,
	ASC_CHR_h,
	ASC_CHR_i,
	ASC_CHR_s,
	ASC_CHR_SPACE,
	ASC_CHR_f,
	ASC_CHR_i,
	ASC_CHR_l,
	ASC_CHR_e,
	ASC_CHR_SPACE,
	ASC_CHR_m,
	ASC_CHR_u,
	ASC_CHR_s,
	ASC_CHR_t,
	ASC_CHR_SPACE,
	ASC_CHR_b,
	ASC_CHR_e,
	ASC_CHR_SPACE,
	ASC_CHR_c,
	ASC_CHR_o,
	ASC_CHR_n,
	ASC_CHR_v,
	ASC_CHR_e,
	ASC_CHR_r,
	ASC_CHR_t,
	ASC_CHR_e,
	ASC_CHR_d,
	ASC_CHR_SPACE,
	ASC_CHR_w,
	ASC_CHR_i,
	ASC_CHR_t,
	ASC_CHR_h,
	ASC_CHR_SPACE,
	ASC_CHR_b,
	ASC_CHR_i,
	ASC_CHR_n,
	ASC_CHR_h,
	ASC_CHR_e,
	ASC_CHR_x,
	ASC_CHR_NUL
};

// (THIS FILE MUST BE CONVERTED WITH BINHEX
static const char gszBinHexIdU[] =
{
	ASC_CHR_LPAREN,
	ASC_CHR_T,
	ASC_CHR_H,
	ASC_CHR_I,
	ASC_CHR_S,
	ASC_CHR_SPACE,
	ASC_CHR_F,
	ASC_CHR_I,
	ASC_CHR_L,
	ASC_CHR_E,
	ASC_CHR_SPACE,
	ASC_CHR_M,
	ASC_CHR_U,
	ASC_CHR_S,
	ASC_CHR_T,
	ASC_CHR_SPACE,
	ASC_CHR_B,
	ASC_CHR_E,
	ASC_CHR_SPACE,
	ASC_CHR_C,
	ASC_CHR_O,
	ASC_CHR_N,
	ASC_CHR_V,
	ASC_CHR_E,
	ASC_CHR_R,
	ASC_CHR_T,
	ASC_CHR_E,
	ASC_CHR_D,
	ASC_CHR_SPACE,
	ASC_CHR_W,
	ASC_CHR_I,
	ASC_CHR_T,
	ASC_CHR_H,
	ASC_CHR_SPACE,
	ASC_CHR_B,
	ASC_CHR_I,
	ASC_CHR_N,
	ASC_CHR_H,
	ASC_CHR_E,
	ASC_CHR_X,
	ASC_CHR_NUL
};

static const int gnSizeOfBinHexID = sizeof(gszBinHexIdL);

