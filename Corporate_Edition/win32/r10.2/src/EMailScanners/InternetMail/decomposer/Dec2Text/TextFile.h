// TextFile.h : Header for main driver module for decomposing text-based data formats.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(TEXTFILE_H)
#define TEXTFILE_H

#define MAX_TEXT_ENGINES	4
#define TEXTFILE_NOT_IDENTIFIED_OFFSET	(size_t)-1

// These are the possible return values from a ConsumeByte call.
#define LEXER_DONE				0
#define LEXER_OK				1
#define LEXER_IDENTIFIED		2
#define LEXER_IDENTIFIED_DONE	3
#define LEXER_ERROR				-1
#define LEXER_USER_ABORT		-2		// Maps to DECERR_USER_CANCEL
#define LEXER_MAX_EXTRACT_SIZE	-3		// Maps to DECERR_CHILD_MAX_SIZE
#define LEXER_ENCRYPTED			-4		// Maps to DECERR_CHILD_DECRYPT

// These are the possible return values from a ConsumeToken call.
#define PARSER_DONE				0
#define PARSER_OK				1
#define PARSER_IDENTIFIED		2
#define PARSER_CHANGE_STATE		3
#define PARSER_ERROR			-1
#define PARSER_USER_ABORT		-2		// Maps to DECERR_USER_CANCEL
#define PARSER_MAX_EXTRACT_SIZE -3		// Maps to DECERR_CHILD_MAX_SIZE
#define PARSER_ENCRYPTED		-4		// Maps to DECERR_CHILD_DECRYPT

//
// Common macros used by the text engines
//
#define IsCHAR(xx)	( (unsigned char)(xx) < 128 )
#define IsALPHA(xx) ( ( (unsigned char)(xx) >= 65 && (unsigned char)(xx) <= 90) || \
					 ( (unsigned char)(xx) >= 97 && (unsigned char)(xx) <= 122) )
#define IsDIGIT(xx) ( (unsigned char)(xx) >= 48 && x <= 57)
#define IsCTL(xx)	( (unsigned char)(xx) <= 31 || (unsigned char)(xx) == 127)
#define IsCR(xx)	( (unsigned char)(xx) == 13)
#define IsLF(xx)	( (unsigned char)(xx) == 10)
#define IsSPACE(xx) ( (unsigned char)(xx) == 32)
#define IsHTAB(xx)	( (unsigned char)(xx) == 9)
#define IsQUOTE(xx) ( (unsigned char)(xx) == 34)


#define MARKER_COMMAND_COPY				0
#define MARKER_COMMAND_DELETE			1
#define MARKER_COMMAND_REPLACE_FILE		2
#define MARKER_COMMAND_REPLACE_DATA		3
#define MARKER_COMMAND_ADD				4

typedef struct tagTEXT_FIND_DATA
{
	char			*pszName;
	char			*pszSecondaryName;
	unsigned long	dwNameCharset;
	size_t			nCompressedSize;
	size_t			nUncompressedSize;
	unsigned long	dwType;				// DEC_ITEMTYPE_xxx
	unsigned long	dwAttributes;
	unsigned long	dwAttrType;			// DEC_ATTRTYPE_xxx
	unsigned long	dwYear;
	unsigned long	dwMonth;
	unsigned long	dwDay;
	unsigned long	dwHour;
	unsigned long	dwMinute;
	unsigned long	dwSecond;
	unsigned long	dwMillisecond;
} TEXT_FIND_DATA, *PTEXT_FIND_DATA;


typedef struct tagTEXT_CHANGE_DATA
{
	bool			bRenameItem;
	bool			bRawChange;
	const char		*pszFilename;
	const char		*pszName;
	const char		*pszContentType;
	char			*pszData;
	unsigned long	dwType;				// DEC_ITEMTYPE_xxx
	unsigned long	dwYear;
	unsigned long	dwMonth;
	unsigned long	dwDay;
	unsigned long	dwHour;
	unsigned long	dwMinute;
	unsigned long	dwSecond;
	unsigned long	dwMillisecond;
	unsigned long	dwAttributes;
//    unsigned long   dwFlags;
	size_t			nStartOffset;
	size_t			nEndOffset;
} TEXT_CHANGE_DATA, *PTEXT_CHANGE_DATA;


//
// On-disk structure
//
#if defined(IBMCXX)
#pragma option align=packed	 // AIX
#else
#pragma pack(1)				 // WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif

typedef struct tagTEXT_MARKER
{
	unsigned long	dwCommand;		// Command ID (MARKER_COMMAND_xxx)
	size_t			nStartOffset;	// Start offset of original data block to be modified.
	size_t			nEndOffset;		// End offset of original data block to be modified.
	size_t			nDataSize;		// Number of bytes (if any) following this TEXT_MARKER.
} TEXT_MARKER, *PTEXT_MARKER;

#if defined(IBMCXX)
#pragma option align=reset	// AIX
#else
#pragma pack()				// WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif


class CTextEngine;	// forward-reference
class CTextFile;	// forward-reference

class ILexer
{
public:
	ILexer() {}
	virtual ~ILexer() {}

	// ILexer methods:
	virtual bool	Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject) = 0;
	virtual int		GetInterface(void **ppInterface) = 0;
	virtual int		ConsumeByte(unsigned char uchByte) = 0;
	virtual int     ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset) = 0;
	virtual int		ConsumeEOF(size_t nOffset) = 0;
	virtual int		LexComplete(void) = 0;
	virtual int		ProcessComplete(void) = 0;
	virtual int		FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData) = 0;
	virtual int		FindNextEntry(TEXT_FIND_DATA *pData) = 0;
	virtual int		SkipExtractEntry(TEXT_FIND_DATA *pData) = 0;
	virtual int		ExtractEntry(char *pszFilename, FILE *fpin) = 0;
	virtual int		ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile) = 0;
	virtual int		AddEntry(TEXT_CHANGE_DATA *pData) = 0;
	virtual int		DeleteEntry(TEXT_CHANGE_DATA *pData) = 0;

	bool	m_bWantBytes;
};


class CTextFile
{
public:
	CTextFile();
	~CTextFile();
	DECRESULT Open(FILE *fp, IDecomposerEx *pDecomposer, CTextEngine *pEngine, IDecContainerObjectEx *pObject, DWORD *pdwType);
	DECRESULT Close(WORD *pwResult, char *pszNewDataFile);
	DECRESULT Identify(IDecContainerObjectEx *pObject, DWORD *pdwType);
	void	 IdentifyComplete(void);
	DECRESULT GetInterface(void **ppInterface);
	DECRESULT GetFirstEntry(TEXT_FIND_DATA *pData);
	DECRESULT GetNextEntry(TEXT_FIND_DATA *pData);
	DECRESULT SkipExtractEntry(TEXT_FIND_DATA *pData);
	bool	 GetEnumerationEngine(void);
	DECRESULT BeginChanges(const char *szOutFile);
#ifdef _DEBUG
	void	 DumpFindData(TEXT_FIND_DATA *pData);
#endif
	DECRESULT GetEntryName(char *pszName, WORD wNameSize);
	DECRESULT GetEntryAttributes(DWORD *pdwAttrs);
	DECRESULT ExtractEntry(const char *szFile);
	DECRESULT ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile);
	DECRESULT AddEntry(TEXT_CHANGE_DATA *pData);
	DECRESULT DeleteEntry(TEXT_CHANGE_DATA *pData);
	DECRESULT MarkForDelete(TEXT_CHANGE_DATA *pData);
	DECRESULT MarkForReplace(TEXT_CHANGE_DATA *pData);
	DECRESULT MarkForAdd(TEXT_CHANGE_DATA *pData);
	bool IsModified() { return m_bChanged; };
	bool SetModified(bool bNew = true){ bool bTemp = m_bChanged; m_bChanged = bNew; return bTemp; };
	bool IsExtractOK() { return m_bExtractOK; };
	bool SetExtractOK(bool bNew = true){ bool bTemp = m_bExtractOK; m_bExtractOK = bNew; return bTemp; };
	bool Reconstruct(void);
	bool CopyBlock(FILE *fpin, FILE *fpout, size_t nStartOffset, size_t nEndOffset);

	FILE	*m_pFile;
	DWORD	m_dwFilenameCharset;
	DWORD	m_dwItemType;
	DWORD	m_dwChildType;

	//
	// Textfile specific variables controlled by the SetOption API
	//
	DWORD	m_dwEnhancedID;

private:
	bool	WriteMarker(TEXT_MARKER *pMarker);

	DWORD	m_dwType;
	FILE	*m_pMarkerFile;
	char	m_szMarkerFile[MAX_PATH];
	char	m_szOutputFile[MAX_PATH];
	char	m_szFilename[MAX_PATH];
	bool	m_bChanged;
	bool	m_bExtractOK;
	bool	m_bIdentified;
	unsigned char *m_pBuffer;
	int		m_iBufferIndex;
	size_t	m_nBufferBytes;
	int		m_iEngineIndex;
	int		m_EngineReturnCode[MAX_TEXT_ENGINES];
	size_t	m_nIDOffset[MAX_TEXT_ENGINES];
	size_t	m_nLastIDOffset;
	ILexer	*m_pLexer[MAX_TEXT_ENGINES];
	IDecomposerEx	*m_pDecomposer;
	CTextEngine		*m_pEngine;
};


/////////////////////////////////////////////////////////////////////////
// CTextEngine

class CTextEngine : public CDecEngine
{
public:
	virtual DECRESULT Process(
		IDecomposerEx *pDecomposer,
		IDecContainerObjectEx *pObject,
		IDecEventSink *pSink,
		WORD *pwResult,
		char *pszNewDataFile,
		bool *pbIdentified,
		bool *pbContinue);

	virtual DECRESULT SetMaxExtractSize(DWORD dwMaxSize);
	virtual DECRESULT AbortProcess(bool bAbort);
	virtual DECRESULT SetIOCallback(IDecIOCB *pIOCallback);

	DECRESULT TextCreateTempFile(const char *szExtension, char *pszFile, IDecomposerEx *pDecomposer);
	DECRESULT PrepareNewDataFile(
		IDecomposerEx *pDecomposer,
		IDecContainerObjectEx *pObject,
		CTextFile *pTextFile,
		WORD *pwResult,
		char *pszNewDataFile);

private:
	DECRESULT ProcessChildren(
		IDecomposerEx *pDecomposer,
		IDecContainerObjectEx *pObject,
		IDecEventSink *pSink,
		CTextFile *pTextFile,
		WORD *pwResult,
		char *pszNewDataFile);

	void FreeFindData(TEXT_FIND_DATA *ptfd);

	DECRESULT ProcessChild(
		IDecomposerEx *pDecomposer,
		IDecContainerObjectEx *pObject,
		IDecEventSink *pSink,
		CTextFile *pTextFile,
		TEXT_FIND_DATA *pfd,
		const char *szChildName,
		WORD *pwResult,
		char *pszNewDataFile,
		bool *pbContinueWithChildren,
		size_t& attributeIndex);

	DECRESULT ApplyChildResults(
		IDecomposerEx *pDecomposer,
		IDecContainerObjectEx *pObject,
		CTextFile *pTextFile,
		TEXT_FIND_DATA *pfd,
		const char *szChildDataFile,
		WORD wChildResult,
		const char *szChildNewDataFile,
		WORD *pwResult,
		char *pszNewDataFile,
		bool *pbKeepChildDataFile);

	DECRESULT OpenTextFile(
		IDecomposerEx *pDecomposer,
		IDecContainerObjectEx *pObject,
		CTextFile *pTextFile,
		FILE *fp,
		DWORD *pdwType);

	DECRESULT CloseTextFile(
		CTextFile *pTextFile,
		WORD *pwResult,
		char *pszNewDataFile);

	DWORD m_dwEventSinkVersion;
};


// Decomposer-specific control functions.
void TextSetMaxExtractSize(DWORD dwMaxSize);
void TextAbortProcess(bool bAbort);
void TextSetLogWarnings(bool bLogWarnings);

#endif	// TEXTFILE_H

