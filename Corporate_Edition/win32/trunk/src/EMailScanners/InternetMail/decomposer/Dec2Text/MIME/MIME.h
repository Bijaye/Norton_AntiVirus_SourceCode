// MIME.h : Header for primary MIME engine APIs
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#include "asc_char.h"

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

#include "IMIME.h"
#include "bstricmp.h"
#include "FastArray.h"

#define DEFAULT_FIELD_NAME_LENGTH		32
#define MAX_FILENAME_LEN				(4 * 1024)
#define MAX_CHARSET_NAME_LEN			80
#define MAX_BOUNDARIES					32
#define MAX_MIME_TOKEN_SIZE				(8 * 1024)
#define RFC822_MAX_LINE_LENGTH			1000
#define QUOTED_PRINTABLE_LINE_LEN		100 // 76 max chars, 77 for Netscape &
											// IE which cheese the spec.
#define BASE64_LINE_LEN					72	// 72
#define MAX_EMIT_BUFFER_SIZE			(65 * 1024)
#define MAX_ATOM_CANDIDATE_SIZE			128

#define IsSPECIAL(x) \
 ((x == ASC_CHR_LPAREN) || \
  (x == ASC_CHR_RPAREN) || \
  (x == ASC_CHR_LT) || \
  (x == ASC_CHR_GT) || \
  (x == ASC_CHR_AT) || \
  (x == ASC_CHR_COMMA) || \
  (x == ASC_CHR_SEMI) || \
  (x == ASC_CHR_COLON) || \
  (x == ASC_CHR_BSLASH) || \
  (x == ASC_CHR_DQUOTE) || \
  (x == ASC_CHR_PERIOD) || \
  (x == ASC_CHR_LBRACK) || \
  (x == ASC_CHR_RBRACK))

#define IsTSPECIAL(x) \
 ((x == ASC_CHR_LPAREN) || \
  (x == ASC_CHR_RPAREN) || \
  (x == ASC_CHR_LT) || \
  (x == ASC_CHR_GT) || \
  (x == ASC_CHR_AT) || \
  (x == ASC_CHR_COMMA) || \
  (x == ASC_CHR_SEMI) || \
  (x == ASC_CHR_COLON) || \
  (x == ASC_CHR_BSLASH) || \
  (x == ASC_CHR_DQUOTE) || \
  (x == ASC_CHR_PERIOD) || \
  (x == ASC_CHR_LBRACK) || \
  (x == ASC_CHR_RBRACK) || \
  (x == ASC_CHR_QUESTION) || \
  (x == ASC_CHR_EQ) || \
  (x == ASC_CHR_FSLASH))

#define EMIT_OFFSET				m_emit.nTokenOffset
#define EMIT_DATA				m_emit.nTokenData
#define EMIT_ID					m_emit.iTokenID
#define EMIT_FLAGS				m_emit.dwFlags
#define DO_EMIT					if (!EmitToken()) return (PARSER_ERROR)

// Return codes for CheckForBoundary function.
#define MIME_NOT_BOUNDARY		0
#define MIME_BOUNDARY_START		1
#define MIME_BOUNDARY_END		2

// MIMEATOM groups (see MIMEParser.cpp token/string arrays).
enum
{
	MIME_ATOM_GROUP_HEADER_FIELDS,
	MIME_ATOM_GROUP_RECEIVED,
	MIME_ATOM_GROUP_ENCODING,
	MIME_ATOM_GROUP_DISPOSITION,
	MIME_ATOM_GROUP_TYPE,
	MIME_ATOM_GROUP_DAYOFWEEK,
	MIME_ATOM_GROUP_MONTH,
	MIME_ATOM_GROUP_TIMEZONE,
	MIME_ATOM_GROUP_SIGNED_DATA
};


// Extraction states
#define MIME_EXTRACT_STATE_HEADER_FIRST 0
#define MIME_EXTRACT_STATE_DATE			1
#define MIME_EXTRACT_STATE_ATTACHMENT	2
#define MIME_EXTRACT_STATE_HEADER_LAST	3
#define MIME_EXTRACT_STATE_DONE			4  // This must be the last extract state.


typedef struct tagMIMEAtoms
{
	int		iTokenID;
	char	*pszFieldNameL;
	char	*pszFieldNameU;
} MIMEATOM, *PMIMEATOM;


typedef struct tagFindNameData
{
	size_t	nIndex;
	char	*pszName;
	tagFindNameData *pNext;
} FINDNAMEDATA, *PFINDNAMEDATA;

struct BoundaryRecord
{
	char*	m_pszBoundary;
	size_t	m_nBoundaryStrLen;
	size_t	m_nNestedLevel;
	bool	m_fClosed;
};

class CMIMEParser;	

// Forward-reference - CMIME and CMIMELexer need this.
// MIMEIdentificationProfile is used when we are evaluating if a text file
// is a MIME message. The profile contains the required thresholds that
// must be met for a message to be considered MIME.

class MIMEIdentificationProfile {

public:

	MIMEIdentificationProfile();
	MIMEIdentificationProfile(	size_t minKnownKeywords,
								size_t minKeywordWeighting,
								size_t minIdentifiedHeaderLines,
								size_t maxPercentageMalformaties,
								size_t maxMalformaties);
	MIMEIdentificationProfile(const MIMEIdentificationProfile& rhs);
	MIMEIdentificationProfile& operator=(const MIMEIdentificationProfile& rhs);

	~MIMEIdentificationProfile();
	size_t GetMinNumberOfKnownKeywords() const;
	size_t GetMinKeywordWeighting() const;
	size_t GetMinPercentageIdentifiedHeaderLines() const;
	size_t GetMaxPercentageOfMalformaties() const;
	size_t GetMaxMalformaties() const;
	void SetMinNumberOfKnownKeywords(size_t value);
	void SetMinKeywordWeighting(size_t value);
	void SetMinPercentageIdentifiedHeaderLines(size_t value);
	void SetMaxPercentageOfMalformaties(size_t value);
	void SetMaxMalformaties(size_t value);

private:

	size_t	m_nMinNumberOfKnownKeywords;
	size_t	m_nMinKeywordWeighting;
	size_t	m_nMinIdentifiedHeaderLines;
	size_t	m_nMaxPercentageMalformaties;
	size_t	m_nMaxMalformaties;
};

// This is the implementation of the IMIME interface that clients access
// to directly manipulate MIME objects.  This class hides the private
// CMIMEParser class beneath the public IMIME interface.
class CMIME : public IMIME
{
public:
	CMIME();
	~CMIME();

	// IMIME methods:
	int		GetTokenFilename(char *pszFilename, size_t nBufferSize);
	int		GetWarningFilename(char *pszFilename, size_t nBufferSize);
	int		GetSectionInfo(MIMESECTIONINFO *pInfo, size_t nSectionID, size_t nSectionNumber);
	int		ExtractRawSection(char *pszFilename, size_t nSectionID, size_t nSectionNumber);
	int		ReplaceRawSection(char *pszFilename, size_t nSectionID, size_t nSectionNumber);
	int		ExtractBody(char *pszFilename, size_t nSectionNumber);

	// CMIME methods:
	void	Init(CMIMEParser *pParser);

private:
	CMIMEParser		*m_pParser;
	FILE			*m_fpTokenFile;
	MIMEEMITTOKEN	m_emit;
	size_t			m_nStartOffset;
	size_t			m_nEndOffset;
	int				m_iFieldID;
	int				m_iExtractState;	// MIME_EXTRACT_xxx state
	int				m_iEncoding;
	WORD			m_wResult;
};


class CMIMELexer : public ILexer
{
public:

	enum eTerminator {
			eUnknown,
			eCarriageReturn,
			eLineFeed,
			eCarriageReturnLineFeed
	};

	CMIMELexer(DWORD dwMaxScanBytes);
	~CMIMELexer();

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
	int		GetSizeOfLineTerminator() const;
	 
private:

	inline int		EmitToken(int iTokenID);

	CMIMEParser		*m_pParser;
	CMIME			*m_pIMIME;
	unsigned char	m_byBuffer[MAX_MIME_TOKEN_SIZE];
	size_t			m_nLexemeOffset;
	size_t			m_nContinuationOffset;
	size_t			m_nLWSPCount;
	size_t			m_nStartOffset;
	size_t			m_nForwardOffset;
	size_t			m_nLineNumber;	// current line number
	size_t			m_nOffset;
	size_t			m_nCommentLevel;
	int				m_State;
	int				m_iNextStartStateIndex;
	bool			m_bNewLine;
	bool			m_bIdentified;
	bool			m_bContinuationLine;
	CTextFile*		m_pArchive;
	bool			m_bMIMEDetectionDisabled;
	bool			m_bEndOfHeaderIdentificationEnabled;

	IDecContainerObjectEx *
					m_pObject;
	//
	// Variables controlled by the SetOption API
	//
	DWORD			m_dwMaxScanBytes;

	// CRLF Length Indicator.  This value is 1 or 2 depending
	// on the CRLF used
	unsigned int	m_nLineTerminatorLength;
	unsigned char	m_uchLastChar;

	eTerminator		m_ePrimaryLineTerminator;

};

enum MIMESignatureState { NONE = 0, FIRST_BYTE, SIGNED_DATA_CANDIDATE, SIGNED_DATA };

class CMIMEParser
{
public:
	CMIMEParser();
	~CMIMEParser();

	bool	Init(CMIMELexer* pLexer,CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject);
    int     ConsumeToken(int iTokenID, char *pString, size_t nStringLength, size_t nOffset, size_t nLineNumber);
	int		ParseComplete(void);
	int		FindFirstEntry(TEXT_FIND_DATA *pData);
	int		FindNextEntry(TEXT_FIND_DATA *pData);
	bool	DecodeName(char *pszName, bool bRFC2231, DWORD *pdwCharset, bool* pfFound);
	int		ExtractEntry(char *pszFilename, FILE *fpin);
	int		ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile);
	int		AddEntry(TEXT_CHANGE_DATA *pData);
	int		DeleteEntry(TEXT_CHANGE_DATA *pData);
	int		FindFirstSection(MIMESECTIONINFO *pInfo);
	int		FindNextSection(MIMESECTIONINFO *pInfo);
	void	EmitWarning(size_t nWarningID, size_t nContextTokenID);
	bool	IsProcessingSignature();
	bool	isMultipart() const;
	bool	isAttachment() const;
	bool	isFuzzy() const;
	int		GetContentType() const;
	bool	EndOfHeaderFound() const;
	void	ResetEndOfHeaderFlag();
	bool	isValidMIMEHeader() const;
	void	FileDeterminedToBeMIME();

	CMIMELexer		*m_pLexer;
	IDecomposerEx	*m_pDecomposer;
	CTextEngine		*m_pEngine;
	CTextFile		*m_pArchive;
	IDecContainerObjectEx *m_pObject;
	char			*m_pszTokenFile;
	char			*m_pszWarningFile;
	size_t			m_nStartOffset;		// Used for extraction
	size_t			m_nEndOffset;		// Used for extraction
	bool			m_bScanForPairs;
	bool			m_bInHeader;
	bool			m_bWantBytes;
	bool			m_bIdentified;

private:

	// Identification level is used if we are identifying
	// the file at the end of the header.

	enum IdentificationLevel {
								idDisabled	= 0,
								idLevel1	= 1,		// Weak
								idLevel2	= 2,
								idLevel3	= 3,		// Middle of the road
								idLevel4	= 4,
								idLevel5	= 5			// Strong
	};

	bool	GetIdentificationProfile(MIMEIdentificationProfile& fillin) const;
	size_t	Percentage(size_t thisVaraible,size_t relativeToThisVaraible) const;
	int		ProcessHeaderAtom();
	int		UpdateHeader(int iEncoding, const char *pszName, const char *pszContentType, DWORD dwType);
	int		FindName(FINDNAMEDATA *pData);
	bool	ConstructName(char **ppszName, FINDNAMEDATA *pData);
	inline void FreeFindNameData(FINDNAMEDATA *pfnd);
	inline bool EmitToken(void);
	int		EmitName(void);
	int		IdentifyAtom(int iGroup);
	inline bool IsRFC2231(char *pszString, size_t *pnOffset);

	void	SetEmitWarning(size_t nWarningID, size_t nContextTokenID);
	void	TriggerWarning();
	bool    IsAtomInSignature();
	bool	IsValidWarningDelay();

	// From QuotedPrintable.cpp:
	bool	IsValidQPChar(char cNextChar);
	size_t	GetQPLineLength(char *pBuffer, size_t nCount, size_t *pnWhitespaceLength);
	bool	DequoteChar(char *lpszHex, char *lpcChar);
	int		DecodeQuotedPrintable(FILE *lpstInput, DWORD dwStartOffset, DWORD dwEndOffset,
								 FILE *lpstOutput);
	size_t	DecodeQuotedPrintableBuffer(char *pIn, size_t nInBytes, char *pOut, size_t nOutBytes, size_t *pnIndex);
	int		WriteQP(DWORD dwCur, DWORD dwNext, FILE *lpstFile, const char *lpszOutputFileName);
	int		Decode8bit(FILE *lpstInput, DWORD dwStartOffset, DWORD dwEndOffset, FILE *lpstOutput);
	int		Write8bit(DWORD dwCur, DWORD dwNext, FILE *lpstFile, const char *lpszOutputFileName);
	bool	DecodeEncodedWords(char *pszDest, DWORD dwDestSize, const char *pszSource, DWORD *pdwCharset, bool* pfFound);
	size_t	GenerateUnknownName(char *pBuffer, size_t nBufferSize);
	DWORD	GetCharset(char *pszCharset);
	DWORD	DecodeQBuffer(char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize);
	DWORD	EncodeQBuffer(char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize);
	int		CopyBytes(FILE *lpstInputFile, FILE *lpstOutputFile,
					 DWORD dwStart, DWORD dwEnd,
					 char *lpszRenameName, char *lpszContentType);
	bool	WriteRenamedContentLine(BYTE *pBuffer, DWORD dwBufSize,
									char *lpszRenameName, char *lpszContentType,
									FILE *lpstOutputFile);

	// From Base64.cpp:
	int		DecodeBase64(FILE *lpstInput, DWORD dwStartOffset, DWORD dwEndOffset, FILE *lpstOutput);
	int		EncodeBase64(FILE *lpstInput, FILE *lpstOutput);
	int		WriteBase64(DWORD dwCur, DWORD dwNext, FILE *lpstFile, const char *lpszOutputFileName);
	DWORD	DecodeBase64Buffer(char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize);
	DWORD	EncodeBase64Buffer(char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize);
	void	EncodeTriToUUQuad(LPBYTE lpbyTri, char *lpszQuad);

	inline	bool HasResentPrefix(char*& fillinPtr) const {
		bool result = false;
	    // Check for the "Resent-" prefix and set the RESENT bit-flag
	    // if it is present and we find a matching keyword.
		if (	 m_pString != NULL  
			 &&	(m_pString[0] == '\x52' || m_pString[0] == '\x72')	// 'R' || 'r'
			 && (m_pString[1] == '\x45' || m_pString[1] == '\x65')	// 'E' || 'e'
			 && (m_pString[2] == '\x53' || m_pString[2] == '\x73'))	// 'S' || 's'
		{
			if ( binary_strnicmp(m_pString, STR_resent_, STR_RESENT_, 7) == 0)
			{
				result = true;
				fillinPtr = &m_pString[7];
			}
		}
		return result;
	}


	inline bool HasXDashPrefix(char*& fillinPtr) const 
	{
		bool result = false;
	    // Check for the "X-" prefix and set the EXTENSION bit-flag
		// if it is present and we find a matching keyword.
		if ((m_pString[0] == ASC_CHR_X || m_pString[0] == ASC_CHR_x) &&
			(m_pString[1] == ASC_CHR_MINUS))
		{
			result = true;
			fillinPtr = &m_pString[2];
		}
		return result;
	}


	inline bool HasRFC2231Suffix(size_t& nOffset) 
	{
		bool result = false;
        if (IsRFC2231(m_pString, &nOffset))
        {
			char*	ptr;
            char    ch;
            size_t  nValue = 0;

            // NULL-terminate the string at nOffset so that we can compare
            // only the keyword-portion of the atom.
            m_pString[nOffset] = 0;
            result = true;

            // Now get the index (if any) associated with this keyword.
            ptr = &m_pString[nOffset + 1];
            ch = *ptr++;
            while (ch)
            {
                if (ch < ASC_CHR_0 || ch > ASC_CHR_9)
                    break;

                nValue = nValue * 10;
                nValue += (ch - ASC_CHR_0);
                ch = *ptr++;
            }

            EMIT_DATA = nValue;
        }
		return result;
	}

	inline char ForceLowercase(char in)
	{
		return ( ( in >= '\x41' && in <= '\x5A' ) ? (in |= 0x20) : in );
	}

	// MIME state machine functions
	inline int State_HEADERFIELD1(void);
	inline int State_HEADERFIELD3(void);
	inline int State_HEADERFIELD4(void);
	inline int State_RETURNPATH1(void);
	inline int State_RETURNPATH2(void);
	inline int State_RETURNPATH3(void);
	inline int State_ROUTEADDR1(void);
	inline int State_ROUTEADDR2(void);
	inline int State_ROUTEADDR3(void);
	inline int State_RECEIVED1(void);
	inline int State_RECEIVED2(void);
	inline int State_RECEIVED3(void);
	inline int State_RECEIVED4(void);
	inline int State_RECEIVED5(void);
	inline int State_RECEIVED6(void);
	inline int State_RECEIVED7(void);
	inline int State_RECEIVED8(void);
	inline int State_REPLYTO1(void);
	inline int State_FROM1(void);
	inline int State_SENDER1(void);
	inline int State_DATE1(void);
	inline int State_DATE2(void);
	inline int State_DATE3(void);
	inline int State_DATE4(void);
	inline int State_DATE5(void);
	inline int State_DATE6(void);
	inline int State_DATE7(void);
	inline int State_DATE8(void);
	inline int State_DATE9(void);
	inline int State_DATE10(void);
	inline int State_DATE11(void);
	inline int State_TO1(void);
	inline int State_CC1(void);
	inline int State_BCC1(void);
	inline int State_MESSAGEID1(void);
	inline int State_INREPLYTO1(void);
	inline int State_REFERENCES1(void);
	inline int State_KEYWORDS1(void);
	inline int State_SUBJECT1(void);
	inline int State_COMMENTS1(void);
	inline int State_ENCRYPTED1(void);
	inline int State_CONTENTTYPE1(void);
	inline int State_CONTENTTYPE2(void);
	inline int State_CONTENTTYPE3(void);
	inline int State_CONTENTTYPE4(void);
	inline int State_CONTENTTYPE5(void);
	inline int State_CONTENTTYPE6(void);
	inline int State_CONTENTTYPE7(void);
	inline int State_CONTENTTYPE8(void);
	inline int State_CONTENTTYPE9(void);
	inline int State_CONTENTTYPE10(void);
	inline int State_CONTENTTYPE11(void);
	inline int State_CONTENTDISPOSITION1(void);
	inline int State_CONTENTDISPOSITION2(void);
	inline int State_CONTENTDISPOSITION3(void);
	inline int State_CONTENTDISPOSITION4(void);
	inline int State_CONTENTDESCRIPTION1(void);
	inline int State_CONTENTTRANSFERENCODING1(void);
	inline int State_CONTENTID1(void);
	inline int State_MIMEVERSION1(void);
	inline int State_IMPORTANCE1(void);
	inline int State_BODY1(void);
	inline int State_BODY2(void);
	inline int State_MULTIPART1(void);
	inline int State_MULTIPART2(void);
	inline int State_MULTIPART3(void);
	inline int State_MULTIPARTHEADER1(void);
	inline int State_MULTIPARTHEADER3(void);
	inline int State_MULTIPARTHEADER4(void);
	inline int State_MULTIPARTBODY1(void);
	inline int State_MULTIPARTBODY2(void);
	inline int State_MULTIPARTBODY3(void);
	inline int State_MULTIPARTBODY4(void);
	inline int State_ORGANIZATION1(void);
	inline int State_AUTHOR1(void);
	inline int State_PRIORITY1(void);
	inline int State_DELIVEREDTO1(void);
	inline int State_CONTENTLOCATION1(void);
	inline int State_THREADTOPIC1(void);
	inline int State_THREADINDEX1(void);
	inline int State_MAILER1(void);
	inline int State_CONTENTLENGTH1(void);

	bool	SetBoundaryTest(char *pszTest);
	bool	AppendBoundaryTest(char *pszTest);
	void	FreeBoundaryTest(void);
	inline int CheckForBoundary(void);

	bool	m_bMultipart;
	bool	m_bInPreamble;
	bool	m_bInMultipartHeader;
	bool	m_bHasXDashPrefix;
	int		m_iExtractState;	// MIME_EXTRACT_xxx state
	int		m_iTokenID;
	int		m_iPreviousTokenID;
	int		m_iFieldID;
	int		m_iEncoding;
	int		m_iEncryption;
	int		m_ParseState;
	int		m_ParseSubstate;
	FILE	*m_fpTokenFile;     // Intermediate/tokenized output file
	unsigned char *m_pEmitBuffer;
	size_t	m_nEmitBufferOffset;
	FILE	*m_fpWarningFile;   // Intermediate/tokenized output file
	char	*m_pString;
	FastArray<char,DEFAULT_FIELD_NAME_LENGTH> m_AtomCandidate;
	size_t	m_iAtomCandidateBufSize;
	size_t	m_nAtomCandidateOffset;
	int		m_nAtomCandidatePreviousToken;
	bool	m_bBoundarySeparatorFoundInHeader;
	FastArray<BoundaryRecord,MAX_BOUNDARIES> m_pszBoundary;
	char	*m_pszName;
	char	*m_pszBoundaryTest;
	char	m_szCharset[MAX_CHARSET_NAME_LEN];
	size_t	m_nHeaderLinesIDed;
	size_t	m_nHeaderWarnings;
	size_t	m_nWarnings;
	size_t	m_nStringLength;
	size_t	m_nOffset;
	size_t	m_nLineNumber;
	size_t	m_nBoundaryCount;
	size_t	m_nBoundaryNestLevel;
	size_t	m_nBoundaryStart;
	size_t	m_nSectionStart;
	size_t	m_nLineOffset;		// Offset of start of a header line
	size_t	m_nMultipartCount;
	size_t	m_nCharsetValueOffset;
	unsigned long	m_dwType;
	unsigned long	m_dwFlags;
	unsigned long	m_dwYear;
	unsigned long	m_dwMonth;
	unsigned long	m_dwDay;
	unsigned long	m_dwHour;
	unsigned long	m_dwMinute;
	unsigned long	m_dwSecond;
	int				m_iTimezoneDifferential;
	MIMEEMITTOKEN	m_emit;
	MIMEEMITTOKEN	m_lastEmit;
	int				m_nContentTypeID;
	bool			m_bAttachment;
	bool			m_bEndOfHeaderFound;
	bool			m_bBareTerminatorHasBeenCounted;

	unsigned long		m_nIdentifiedHeaderLines;
	unsigned long		m_nUnidentifiedHeaderLines;
	unsigned long		m_nMalformaties;
	unsigned long		m_nKeywordWeighting;
	IdentificationLevel	m_eIdentificationLevel;

	bool	m_fX_pkcs7_mime;
	bool	m_fEmitWarning;
	int	m_nDelayedWarningID;
	int	m_nDelayedContextTokenID;
	int	m_nSignedDataTokenCnt;
	bool	m_fIsProcessingSignature;

	enum MIMESignatureState m_eSigState; 

	//
	// Variables controlled by the SetOption API
	//
	char m_chTranslateNULL;
	bool m_bFuzzyHeader;
	DWORD m_dwMinScanBytes;
	bool m_bExtractMessageBodies;


#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	//
	// Variables used to support IDecAttributes
	//
	int		m_nStartOffsetOfHeader;			// Uninitialized == -1
	int		m_nEndingOffsetOfHeader;		// Uninitialized == -1
	CMIMEAttributeCollection*	m_pMIMEAttributes;
	int		m_nKeyID;
	int		m_nKeyOffset;
	int		m_nValueStartOffset;
	bool	m_addMultipartAttributes;

	// End Variables used to support IDecAttributes
#endif //*** End AS400 Attribute API Removal *** 

	// Known Word Counters
	size_t	m_nReturn_Path;
	size_t	m_nReceived;
	size_t	m_nReply_to;
	size_t	m_nFrom;
	size_t	m_nSender;
	size_t	m_nDate;
	size_t	m_nTo;
	size_t	m_nCc;
	size_t	m_nBcc;
	size_t	m_nMessage_id;
	size_t	m_nIn_reply_to;
	size_t	m_nReferences;
	size_t	m_nKeywords;
	size_t	m_nSubject;
	size_t	m_nComments;
	size_t	m_nEncrypted;
	size_t	m_nContent_type;
	size_t	m_nContent_disposition;
	size_t	m_nContent_description;
	size_t	m_nContent_transfer_encoding;
	size_t	m_nContent_id;
	size_t	m_nMime_version;
	size_t	m_nImportance;
	size_t	m_nOrganization;
	size_t	m_nAuthor;
	size_t	m_nPriority;
	size_t	m_nDelivered_to;
	size_t	m_nContent_location;
	size_t	m_nThread_topic;
	size_t	m_nThread_index;
	size_t	m_nMailer;
	size_t	m_nContent_length;
	size_t	m_nHelo;
	size_t	m_nData;
	size_t	m_nAccept_language;
	size_t	m_nMsmail_priority;
	size_t	m_nUser_agent;
	size_t	m_nApparently_to;
	size_t	m_nId;
	size_t	m_nEgroups_return;
	size_t	m_nList_unsubscribe;
	size_t	m_nXList_unsubscribe;
	size_t	m_nList_subscribe;
	size_t	m_nXList_subscribe;
	size_t	m_nAuto_submitted;
	size_t	m_nOriginating_ip;
	size_t	m_nMailer_version;
	size_t	m_nList_id;
	size_t	m_nList_post;
	size_t	m_nList_help;
	size_t	m_nErrors_to;
	size_t	m_nMailing_list;
	size_t	m_nMs_has_attach;
	size_t	m_nMs_tnef_correlator;
	size_t	m_nMime_autoconverted;
	size_t	m_nContent_class;
	size_t	m_nPrecedence;
	size_t	m_nOriginalarrivaltime;
	size_t	m_nMimeole;
	size_t	m_nList_archive;

};


enum MIMELexerStates
{
	MIME_LEXER_STATE_CRLF1 = 0,
	MIME_LEXER_STATE_CRLF2,
	MIME_LEXER_STATE_CRLF3,
	MIME_LEXER_STATE_LF1,
	MIME_LEXER_STATE_LWSP1,
	MIME_LEXER_STATE_LWSP2,
	MIME_LEXER_STATE_SPECIAL1,
	MIME_LEXER_STATE_QUOTEDSTRING1,
	MIME_LEXER_STATE_QUOTEDSTRING2,
	MIME_LEXER_STATE_QUOTEDSTRING3,
	MIME_LEXER_STATE_QUOTEDSTRING4,
	MIME_LEXER_STATE_QUOTEDSTRING5,
	MIME_LEXER_STATE_QUOTEDSTRING6,
	MIME_LEXER_STATE_DOMAINLITERAL1,
	MIME_LEXER_STATE_DOMAINLITERAL2,
	MIME_LEXER_STATE_DOMAINLITERAL3,
	MIME_LEXER_STATE_DOMAINLITERAL4,
	MIME_LEXER_STATE_DOMAINLITERAL5,
	MIME_LEXER_STATE_DOMAINLITERAL6,
	MIME_LEXER_STATE_COMMENT1,
	MIME_LEXER_STATE_COMMENT2,
	MIME_LEXER_STATE_COMMENT3,
	MIME_LEXER_STATE_COMMENT4,
	MIME_LEXER_STATE_COMMENT5,
	MIME_LEXER_STATE_COMMENT6,
	MIME_LEXER_STATE_ROUTEADDR1,
	MIME_LEXER_STATE_ROUTEADDR2,
	MIME_LEXER_STATE_ROUTEADDR3,
	MIME_LEXER_STATE_ROUTEADDR4,
	MIME_LEXER_STATE_ROUTEADDR5,
	MIME_LEXER_STATE_ROUTEADDR6,
	MIME_LEXER_STATE_ATOM1,
	MIME_LEXER_STATE_ATOM2,
	MIME_LEXER_STATE_COUNT
};


enum MIMEParserStates
{
	MIME_PARSER_STATE_HEADERFIELD1 = 0,
	MIME_PARSER_STATE_HEADERFIELD3,
	MIME_PARSER_STATE_HEADERFIELD4,
	MIME_PARSER_STATE_RETURNPATH1,
	MIME_PARSER_STATE_RETURNPATH2,
	MIME_PARSER_STATE_RETURNPATH3,
	MIME_PARSER_STATE_ROUTEADDR1,
	MIME_PARSER_STATE_ROUTEADDR2,
	MIME_PARSER_STATE_ROUTEADDR3,
	MIME_PARSER_STATE_RECEIVED1,
	MIME_PARSER_STATE_RECEIVED2,
	MIME_PARSER_STATE_RECEIVED3,
	MIME_PARSER_STATE_RECEIVED4,
	MIME_PARSER_STATE_RECEIVED5,
	MIME_PARSER_STATE_RECEIVED6,
	MIME_PARSER_STATE_RECEIVED7,
	MIME_PARSER_STATE_RECEIVED8,
	MIME_PARSER_STATE_REPLYTO1,
	MIME_PARSER_STATE_FROM1,
	MIME_PARSER_STATE_SENDER1,
	MIME_PARSER_STATE_DATE1,
	MIME_PARSER_STATE_DATE2,
	MIME_PARSER_STATE_DATE3,
	MIME_PARSER_STATE_DATE4,
	MIME_PARSER_STATE_DATE5,
	MIME_PARSER_STATE_DATE6,
	MIME_PARSER_STATE_DATE7,
	MIME_PARSER_STATE_DATE8,
	MIME_PARSER_STATE_DATE9,
	MIME_PARSER_STATE_DATE10,
	MIME_PARSER_STATE_DATE11,
	MIME_PARSER_STATE_TO1,
	MIME_PARSER_STATE_CC1,
	MIME_PARSER_STATE_BCC1,
	MIME_PARSER_STATE_MESSAGEID1,
	MIME_PARSER_STATE_INREPLYTO1,
	MIME_PARSER_STATE_REFERENCES1,
	MIME_PARSER_STATE_KEYWORDS1,
	MIME_PARSER_STATE_SUBJECT1,
	MIME_PARSER_STATE_COMMENTS1,
	MIME_PARSER_STATE_ENCRYPTED1,
	MIME_PARSER_STATE_CONTENTTYPE1,
	MIME_PARSER_STATE_CONTENTTYPE2,
	MIME_PARSER_STATE_CONTENTTYPE3,
	MIME_PARSER_STATE_CONTENTTYPE4,
	MIME_PARSER_STATE_CONTENTTYPE5,
	MIME_PARSER_STATE_CONTENTTYPE6,
	MIME_PARSER_STATE_CONTENTTYPE7,
	MIME_PARSER_STATE_CONTENTTYPE8,
	MIME_PARSER_STATE_CONTENTTYPE9,
	MIME_PARSER_STATE_CONTENTTYPE10,
	MIME_PARSER_STATE_CONTENTTYPE11,
	MIME_PARSER_STATE_CONTENTDISPOSITION1,
	MIME_PARSER_STATE_CONTENTDISPOSITION2,
	MIME_PARSER_STATE_CONTENTDISPOSITION3,
	MIME_PARSER_STATE_CONTENTDISPOSITION4,
	MIME_PARSER_STATE_CONTENTDESCRIPTION1,
	MIME_PARSER_STATE_CONTENTTRANSFERENCODING1,
	MIME_PARSER_STATE_CONTENTID1,
	MIME_PARSER_STATE_MIMEVERSION1,
	MIME_PARSER_STATE_IMPORTANCE1,
	MIME_PARSER_STATE_BODY1,
	MIME_PARSER_STATE_BODY2,
	MIME_PARSER_STATE_MULTIPART1,
	MIME_PARSER_STATE_MULTIPART2,
	MIME_PARSER_STATE_MULTIPART3,
	MIME_PARSER_STATE_MULTIPARTHEADER1,
	MIME_PARSER_STATE_MULTIPARTHEADER3,
	MIME_PARSER_STATE_MULTIPARTHEADER4,
	MIME_PARSER_STATE_MULTIPARTBODY1,
	MIME_PARSER_STATE_MULTIPARTBODY2,
	MIME_PARSER_STATE_MULTIPARTBODY3,
	MIME_PARSER_STATE_MULTIPARTBODY4,
	MIME_PARSER_STATE_ORGANIZATION1,
	MIME_PARSER_STATE_AUTHOR1,
	MIME_PARSER_STATE_PRIORITY1,
	MIME_PARSER_STATE_DELIVEREDTO1,
	MIME_PARSER_STATE_CONTENTLOCATION1,
	MIME_PARSER_STATE_THREADTOPIC1,
	MIME_PARSER_STATE_THREADINDEX1,
	MIME_PARSER_STATE_MAILER1,
	MIME_PARSER_STATE_CONTENTLENGTH1,
	MIME_PARSER_STATE_LAST
};

