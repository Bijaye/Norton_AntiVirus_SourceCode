// mbox.h : Header for primary MBOX engine APIs
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2002, 2005 by Symantec Corporation.  All rights reserved.

//
// Note: The AS400 build system performs its builds one directory
// level higher than the other platforms.  It is easier to change
// just the specific AS400 cases rather than change every workspace's
// include search path.
//
#if defined(OS400)
	#include "TextFile.h"
	#include "mbox/mboxFile.h"
#else
	#include "../TextFile.h"
	#include "mboxFile.h"
#endif

#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"

#define MAX_MBOX_TOKEN_SIZE		512

#define EMIT_OFFSET				m_emit.nTokenOffset
#define EMIT_DATA				m_emit.nTokenData
#define EMIT_ID					m_emit.iTokenID
#define EMIT_FLAGS				m_emit.dwFlags
#define DO_EMIT					if (!EmitToken()) return (PARSER_ERROR)

typedef struct tagMBOXEmitToken
{
	size_t			iTokenID;
	size_t			nTokenOffset;
	size_t			nTokenData;
	unsigned long	dwFlags;

} MBOXEMITTOKEN, *PMBOXEMITTOKEN;

enum MBOXLexerTokens
{
	MBOX_TOKEN_FROM_ = 0,            // 0
	MBOX_TOKEN_HEADER_ATOM,          // 1
	MBOX_TOKEN_FROM_LINE_LF,         // 2
	MBOX_TOKEN_ENTRY_START,          // 3
	MBOX_TOKEN_ENTRY_END,            // 4
	MBOX_TOKEN_EOF,                  // 5
	MBOX_TOKEN_LAST                  // 6
};


enum MBOXLexerStates
{
	MBOX_LEXER_STATE_CRLF1 = 0,      // 0
	MBOX_LEXER_STATE_CRLF2,          // 1
	MBOX_LEXER_STATE_LF1,            // 2
	MBOX_LEXER_STATE_FROM1,          // 3
	MBOX_LEXER_STATE_FROM2,          // 4
	MBOX_LEXER_STATE_FROM3,          // 5
	MBOX_LEXER_STATE_FROM4,          // 6
	MBOX_LEXER_STATE_SPACE1,         // 7
	MBOX_LEXER_STATE_HEADERCRLF1,    // 8
	MBOX_LEXER_STATE_HEADERCRLF2     // 9
};

enum MBOXParserStates
{
	PS_START = 0,                    // 0
	PS_FROM_,                        // 1
	PS_SENDER,                       // 2
	PS_WEEKDAY,                      // 3
	PS_MONTH,                        // 4
	PS_DAY,                          // 5
	PS_TIME,                         // 6
	PS_TIME_ZONE1,                   // 7
	PS_TIME_ZONE2,                   // 8
	PS_YEAR,                         // 9
	PS_STARTENTRY                    // 10
};

//
// Forward defines
//
class CMBOXParser;

class CMBOXLexer : public ILexer
{
public:
	CMBOXLexer(DWORD dwMaxScanBytes);
	~CMBOXLexer();

	// ILexer methods:
	bool  Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject);
	int   GetInterface(void **ppInterface);
	int   ConsumeByte(unsigned char uchByte);
	int   ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset);
	int   ConsumeEOF(size_t nOffset);
	int   LexComplete(void);
	int   ProcessComplete(void);
	int   FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData);
	int   FindNextEntry(TEXT_FIND_DATA *pData);
	int   SkipExtractEntry(TEXT_FIND_DATA *pData);
	int   ExtractEntry(char *pszFilename, FILE *fpin);
	int   ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile);
	int   AddEntry(TEXT_CHANGE_DATA *pData);
	int   DeleteEntry(TEXT_CHANGE_DATA *pData);

private:
	inline int     EmitToken(int iTokenID);
	inline int     SkipEmit(void);

	CMBOXParser   *m_pParser;
	unsigned char  m_byBuffer[MAX_MBOX_TOKEN_SIZE];
	size_t         m_nLexemeOffset;
	size_t         m_nStartOffset;
	size_t         m_nOffset;
	int            m_State;
	int            m_iNextStartStateIndex;
	bool           m_bIdentified;

	//
	// Variables controlled by the SetOption API
	//
	DWORD          m_dwTextNonMBOXThreshold;
	DWORD          m_dwMaxScanBytes;
};


class CMBOXParser
{
public:
	CMBOXParser();
	~CMBOXParser();

	bool   Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer);
	int    ConsumeToken(int iTokenID, char *pString, size_t nStringLength, size_t nOffset);
	int    FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData);
	int    FindNextEntry(TEXT_FIND_DATA *pData);
	int    ExtractEntry(char *pszFilename, FILE *fpin);
	int    ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile);
	int    AddEntry(TEXT_CHANGE_DATA *pData);
	int    DeleteEntry(TEXT_CHANGE_DATA *pData);
	int    ProcessComplete(void);
	size_t GenerateUnknownName(char *pBuffer, size_t nBufferSize);

private:
	inline bool EmitToken(void);

	// mbox state machine functions
	int    StateMachine( bool fFirstLine );
	bool   RemoveWhitespace( char* szString );
	bool   IsSenderValid( char* szSender );
	bool   IsYearValid( char* szYear );
	bool   IsWeekDayValid( char* szWeekDay );
	bool   IsMonthValid( char* szMonth );
	bool   IsDayValid( char* szDay );
	bool   IsTimeValid( char* szTime );
	int    AddEntry( void );
	char   ForceLowercase(char in);
	void   SaveEntryOffset( void );
	int    StateStart( bool fFirstLine );
	int    StateFrom( void );
	int    StateSender( void );
	int    StateWeekDay( void );
	int    StateMonth( void );
	int    StateDay( void );
	int    StateTime( bool fFirstLine );
	int    StateTimezone1( bool fFirstLine );
	int    StateTimezone2( bool fFirstLine );
	int    StateYear( bool fFirstLine );
	int    StateStartEntry( bool fFirstLine );
	void   SetState( MBOXParserStates eState, bool fSyntaxError );

	inline void   DebugOut( char* szText, char* szCaption );
	inline void   DebugValueOut( char* szText, char* szCaption );
	inline void   DebugStateOut( char* szText, char* szCaption );
	inline void   DiscardTokenDebugOut( void );

	bool                m_fFirstLine;
	MBOXParserStates    m_eParseState;
	bool                m_fHaveEndToken;
	bool                m_fJustSimpleFrom;
	int                 m_iTokenIDSave;
	char*               m_pStringSave;
	size_t              m_nStringLengthSave;
	size_t              m_nOffsetSave;
	size_t              m_nStartOffsetSave;

	IDecomposerEx      *m_pDecomposer;
	CTextEngine        *m_pEngine;
	CTextFile          *m_pArchive;
	CMBOXFile          *m_pMBOXArchive;

	FILE               *m_fpArchive;
	int                 m_iTokenID;
	char               *m_pString;
	char               *m_pszName;
	char               *m_pszDataName;
	char               *m_pszReplacementName;
	size_t              m_nStringLength;
	size_t              m_nOffset;
	size_t              m_nStartOffset;
	size_t              m_nCurrentEntry;
	unsigned long       m_dwType;
	unsigned long       m_dwFlags;
	MBOXEMITTOKEN       m_emit;
	bool                m_fSyntaxError;
	int                 m_nSenderAtoms;
	int                 m_nTrailingAtoms;
};




