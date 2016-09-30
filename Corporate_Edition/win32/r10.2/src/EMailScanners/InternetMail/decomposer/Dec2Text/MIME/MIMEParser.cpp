// MIMEParser.cpp : Parser for the MIME engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2002, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "MIME.h"
#include "dec_assert.h"

#include "asc_ctype.h"
#include "bstricmp.h"

// "Content-Type: "
#define STR_CLIENT_PROVIDED_CONTENT_TYPE_REPLACEMENT_START "\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x54\x79\x70\x65\x3A\x20"
// ";CRLF"
#define STR_CLIENT_PROVIDED_CONTENT_TYPE_REPLACEMENT_END "\x3B\x0D\x0A"

//#define STR_CONTENT_TYPE_REPLACEMENT "\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x54\x79\x70\x65\x3A\x20\x61\x70\x70\x6C\x69\x63\x61\x74\x69\x6F\x6E\x2F\x6F\x63\x74\x65\x74\x2D\x73\x74\x72\x65\x61\x6D\x3B\x0D\x0A"
#define STR_CONTENT_TRANSFER_ENCODING_REPLACEMENT "\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x54\x72\x61\x6E\x73\x66\x65\x72\x2D\x45\x6E\x63\x6F\x64\x69\x6E\x67\x3A\x20\x62\x61\x73\x65\x36\x34"
//#define STR_CONTENT_DISPOSITION_REPLACEMENT "\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x44\x69\x73\x70\x6F\x73\x69\x74\x69\x6F\x6E\x3A\x20\x61\x74\x74\x61\x63\x68\x6D\x65\x6E\x74\x3B\x0D\x0A"
#define STR_FILENAME_REPLACEMENT "\x66\x69\x6C\x65\x6E\x61\x6D\x65\x3D\x22"
#define STR_NAME_REPLACEMENT "\x6E\x61\x6D\x65\x3D\x22"

#define MIN_DECODE_NAME_BUFFER_SIZE		32

extern volatile bool g_bLogWarnings;
extern volatile DWORD g_dwTextMaxExtractSize;
extern volatile bool g_bTextAbort;

/*
//
// Array of pointers to parser state table functions 
//
static int (*pfnMIMEParserStateTable[])(CMIMEParser *pThis) =
{
	CMIMEParser::Stub_HEADERFIELD1,
	CMIMEParser::Stub_HEADERFIELD2,
	CMIMEParser::Stub_HEADERFIELD3,
	CMIMEParser::Stub_HEADERFIELD4,
	CMIMEParser::Stub_RETURNPATH1,
	CMIMEParser::Stub_RETURNPATH2,
	CMIMEParser::Stub_RETURNPATH3,
	CMIMEParser::Stub_ROUTEADDR1,
	CMIMEParser::Stub_ROUTEADDR2,
	CMIMEParser::Stub_ROUTEADDR3,
	CMIMEParser::Stub_RECEIVED1,
	CMIMEParser::Stub_RECEIVED2,
	CMIMEParser::Stub_RECEIVED3,
	CMIMEParser::Stub_RECEIVED4,
	CMIMEParser::Stub_RECEIVED5,
	CMIMEParser::Stub_RECEIVED6,
	CMIMEParser::Stub_RECEIVED7,
	CMIMEParser::Stub_RECEIVED8,
	CMIMEParser::Stub_REPLYTO1,
	CMIMEParser::Stub_FROM1,
	CMIMEParser::Stub_SENDER1,
	CMIMEParser::Stub_DATE1,
	CMIMEParser::Stub_DATE2,
	CMIMEParser::Stub_DATE3,
	CMIMEParser::Stub_DATE4,
	CMIMEParser::Stub_DATE5,
	CMIMEParser::Stub_DATE6,
	CMIMEParser::Stub_DATE7,
	CMIMEParser::Stub_DATE8,
	CMIMEParser::Stub_DATE9,
	CMIMEParser::Stub_DATE10,
	CMIMEParser::Stub_TO1,
	CMIMEParser::Stub_CC1,
	CMIMEParser::Stub_BCC1,
	CMIMEParser::Stub_MESSAGEID1,
	CMIMEParser::Stub_INREPLYTO1,
	CMIMEParser::Stub_REFERENCES1,
	CMIMEParser::Stub_KEYWORDS1,
	CMIMEParser::Stub_SUBJECT1,
	CMIMEParser::Stub_COMMENTS1,
	CMIMEParser::Stub_ENCRYPTED1,
	CMIMEParser::Stub_CONTENTTYPE1,
	CMIMEParser::Stub_CONTENTTYPE2,
	CMIMEParser::Stub_CONTENTTYPE3,
	CMIMEParser::Stub_CONTENTTYPE4,
	CMIMEParser::Stub_CONTENTTYPE5,
	CMIMEParser::Stub_CONTENTTYPE6,
	CMIMEParser::Stub_CONTENTTYPE7,
	CMIMEParser::Stub_CONTENTTYPE8,
	CMIMEParser::Stub_CONTENTTYPE9,
	CMIMEParser::Stub_CONTENTTYPE10,
	CMIMEParser::Stub_CONTENTTYPE11,
	CMIMEParser::Stub_CONTENTDISPOSITION1,
	CMIMEParser::Stub_CONTENTDISPOSITION2,
	CMIMEParser::Stub_CONTENTDISPOSITION3,
	CMIMEParser::Stub_CONTENTDISPOSITION4,
	CMIMEParser::Stub_CONTENTDESCRIPTION1,
	CMIMEParser::Stub_CONTENTTRANSFERENCODING1,
	CMIMEParser::Stub_CONTENTID1,
	CMIMEParser::Stub_MIMEVERSION1,
	CMIMEParser::Stub_IMPORTANCE1,
	CMIMEParser::Stub_BODY1,
	CMIMEParser::Stub_BODY2,
	CMIMEParser::Stub_MULTIPART1,
	CMIMEParser::Stub_MULTIPART2,
	CMIMEParser::Stub_MULTIPART3,
	CMIMEParser::Stub_MULTIPARTHEADER1,
	CMIMEParser::Stub_MULTIPARTHEADER2,
	CMIMEParser::Stub_MULTIPARTHEADER3,
	CMIMEParser::Stub_MULTIPARTHEADER4,
	CMIMEParser::Stub_MULTIPARTBODY1,
	CMIMEParser::Stub_MULTIPARTBODY2,
	CMIMEParser::Stub_MULTIPARTBODY3,
	CMIMEParser::Stub_MULTIPARTBODY4,
	CMIMEParser::Stub_ORGANIZATION1,
	CMIMEParser::Stub_AUTHOR1,
	CMIMEParser::Stub_PRIORITY1,
	CMIMEParser::Stub_DELIVEREDTO1,
	CMIMEParser::Stub_CONTENTLOCATION1,
	CMIMEParser::Stub_THREADTOPIC1,
	CMIMEParser::Stub_THREADINDEX1,
	CMIMEParser::Stub_MAILER1,
	CMIMEParser::Stub_CONTENTLENGTH1
};
*/

unsigned long g_dwMIMEStateMatrix[MIME_PARSER_STATE_HEADERFIELD4 + 1][MIME_TOKEN_EOF + 1] =
{
//                          LWSP    CR  LF  CRLF    SPECIAL ATOM    QUOTED_STRING   DOMAIN_LITERAL  COMMENT     ROUTE_ADDR  QUOTED_PAIR EOF
/* HEADERFIELD1 */          YES,    NO, NO, YES,    YES,    YES,    NO,             NO,             NO,         NO,         NO,         YES,
/* HEADERFIELD3 */          YES,    NO, NO, YES,    YES,    YES,    YES,            YES,            YES,        YES,        YES,        YES,
/* HEADERFIELD4 */          YES,    NO, NO, YES,    YES,    YES,    YES,            YES,            YES,        YES,        YES,        YES
};

#ifdef _DEBUG
char *g_pszTokenID[MIME_TOKEN_LAST + 1] =
{
	"LWSP",
	"CR",
	"LF",
	"CRLF",
	"SPECIAL",
	"ATOM",
	"QUOTED_STRING",
	"DOMAIN_LITERAL",
	"COMMENT",
	"ROUTE_ADDR",
	"QUOTED_PAIR",
	"EOF",
	"END_MARKER",
	"HEADER_SEPARATOR",
	"RETURN_PATH",
	"RECEIVED",
	"REPLY_TO",
	"FROM",
	"SENDER",
	"DATE",
	"TO",
	"CC",
	"BCC",
	"MESSAGE_ID",
	"IN_REPLY_TO",
	"REFERENCES",
	"KEYWORDS",
	"SUBJECT",
	"COMMENTS",
	"ENCRYPTED",
	"BY",
	"VIA",
	"WITH",
	"ID",
	"FOR",
	"UNKNOWN_FIELD",
	"CONTENT_TYPE",
	"CONTENT_DISPOSITION",
	"CONTENT_DESCRIPTION",
	"CONTENT_TRANSFER_ENCODING",
	"CONTENT_ID",
	"MIME_VERSION",
	"IMPORTANCE",
	"PREAMBLE",
	"BODY",
	"BASE64",
	"UUENCODE",
	"7BIT",
	"8BIT",
	"QUOTED_PRINTABLE",
	"BINARY",
	"NONE",
	"ATTACHMENT",
	"FILENAME",
	"DOMAIN",
	"BOUNDARY_START",
	"MONDAY",
	"TUESDAY",
	"WEDNESDAY",
	"THURSDAY",
	"FRIDAY",
	"SATURDAY",
	"SUNDAY",
	"JANUARY",
	"FEBRUARY",
	"MARCH",
	"APRIL",
	"MAY",
	"JUNE",
	"JULY",
	"AUGUST",
	"SEPTEMBER",
	"OCTOBER",
	"NOVEMBER",
	"DECEMBER",
	"MALICIOUS_DATE",
	"TEXT",
	"PLAIN",
	"RICHTEXT",
	"HTML",
	"TAB_SEPARATED_VALUES",
	"MULTIPART",
	"MIXED",
	"ALTERNATIVE",
	"DIGEST",
	"PARALLEL",
	"APPLEDOUBLE",
	"HEADER_SET",
	"MESSAGE",
	"RFC822",
	"PARTIAL",
	"EXTERNAL_BODY",
	"NEWS",
	"APPLICATION",
	"OCTET_STREAM",
	"POSTSCRIPT",
	"ODA",
	"ATOMICMAIL",
	"ANDREW_INSET",
	"SLATE",
	"WITA",
	"DEC_DX",
	"DCA_RFT",
	"ACTIVEMESSAGE",
	"RTF",
	"APPLEFILE",
	"MAC_BINHEX40",
	"NEWS_MESSAGE_ID",
	"NEWS_TRANSMISSION",
	"WORDPERFECT51",
	"PDF",
	"ZIP",
	"MACWRITEII",
	"MSWORD",
	"REMOTE_PRINTING",
	"IMAGE",
	"JPEG",
	"GIF",
	"IEF",
	"TIFF",
	"AUDIO",
	"BASIC",
	"VIDEO",
	"MPEG",
	"QUICKTIME",
	"BOUNDARY",
	"NAME",
	"CHARSET",
	"ACCESS_TYPE",
	"URL",
	"ENRICHED",
	"ORGANIZATION",
	"AUTHOR",
	"PRIORITY",
	"DELIVERED_TO",
	"CONTENT_LOCATION",
	"THREAD_TOPIC",
	"THREAD_INDEX",
	"MAILER",
	"CONTENT_LENGTH",
	"LAST"
};

#define DUMP_EMIT(et) \
	printf("  iTokenID:     %d  %s\n", et.iTokenID, g_pszTokenID[et.iTokenID]); \
	printf("  nTokenOffset: %d\n", et.nTokenOffset); \
	printf("  nTokenData:   %d\n", et.nTokenData); \
	printf("  dwFlags:      0x%08x\n", et.dwFlags)
#endif


MIMEIdentificationProfile::MIMEIdentificationProfile() 
:	m_nMinKeywordWeighting(60),
	m_nMinIdentifiedHeaderLines(50),
	m_nMaxPercentageMalformaties(60),
	m_nMaxMalformaties(20),
	m_nMinNumberOfKnownKeywords(2)
{
}

MIMEIdentificationProfile::MIMEIdentificationProfile(size_t minKnownKeywords,
													 size_t minKeywordWeighting,
													 size_t minIdentifiedHeaderLines,
													 size_t maxPercentageMalformaties,
													 size_t maxMalformaties) 

{
	m_nMinNumberOfKnownKeywords = minKnownKeywords;
	m_nMinKeywordWeighting = minKeywordWeighting;
	m_nMinIdentifiedHeaderLines = minIdentifiedHeaderLines;
	m_nMaxPercentageMalformaties = maxPercentageMalformaties;
	m_nMaxMalformaties = maxMalformaties;
}

MIMEIdentificationProfile::MIMEIdentificationProfile(const MIMEIdentificationProfile& rhs)
{
	*this = rhs;
}

MIMEIdentificationProfile& MIMEIdentificationProfile::operator=(const MIMEIdentificationProfile& rhs)
{
	if ( this != &rhs)
	{
		m_nMinNumberOfKnownKeywords = rhs.m_nMinNumberOfKnownKeywords;
		m_nMinKeywordWeighting = rhs.m_nMinKeywordWeighting;
		m_nMinIdentifiedHeaderLines = rhs.m_nMinIdentifiedHeaderLines;
		m_nMaxPercentageMalformaties = rhs.m_nMaxPercentageMalformaties;
		m_nMaxMalformaties = rhs.m_nMaxMalformaties;
	}
	return *this;
}

MIMEIdentificationProfile::~MIMEIdentificationProfile()
{
}

size_t MIMEIdentificationProfile::GetMinNumberOfKnownKeywords() const
{
	return m_nMinNumberOfKnownKeywords;
}

size_t MIMEIdentificationProfile::GetMinKeywordWeighting() const
{
	return m_nMinKeywordWeighting;
}

size_t MIMEIdentificationProfile::GetMinPercentageIdentifiedHeaderLines() const
{
	return m_nMinIdentifiedHeaderLines;
}


size_t MIMEIdentificationProfile::GetMaxPercentageOfMalformaties() const
{
	return m_nMaxPercentageMalformaties;
}

size_t MIMEIdentificationProfile::GetMaxMalformaties() const
{
	return m_nMaxMalformaties;
}

void MIMEIdentificationProfile::SetMinNumberOfKnownKeywords(size_t value) 
{
	m_nMinNumberOfKnownKeywords = value;
}

void MIMEIdentificationProfile::SetMinKeywordWeighting(size_t value) 
{
	m_nMinKeywordWeighting = value;
}

void MIMEIdentificationProfile::SetMinPercentageIdentifiedHeaderLines(size_t value) 
{
	m_nMinIdentifiedHeaderLines = value;
}


void MIMEIdentificationProfile::SetMaxPercentageOfMalformaties(size_t value) 
{
	m_nMaxPercentageMalformaties = value;
}

void MIMEIdentificationProfile::SetMaxMalformaties(size_t value) 
{
	m_nMaxMalformaties = value;
}


CMIMEParser::CMIMEParser()
:	m_pLexer(NULL),
	m_nReturn_Path(0),
	m_nReceived(0),
	m_nReply_to(0),
	m_nFrom(0),
	m_nSender(0),
	m_nDate(0),
	m_nTo(0),
	m_nCc(0),
	m_nBcc(0),
	m_nMessage_id(0),
	m_nIn_reply_to(0),
	m_nReferences(0),
	m_nKeywords(0),
	m_nSubject(0),
	m_nComments(0),
	m_nEncrypted(0),
	m_nContent_type(0),
	m_nContent_disposition(0),
	m_nContent_description(0),
	m_nContent_transfer_encoding(0),
	m_nContent_id(0),
	m_nMime_version(0),
	m_nImportance(0),
	m_nOrganization(0),
	m_nAuthor(0),
	m_nPriority(0),
	m_nDelivered_to(0),
	m_nContent_location(0),
	m_nThread_topic(0),
	m_nThread_index(0),
	m_nMailer(0),
	m_nContent_length(0),
	m_nHelo(0),
	m_nData(0),
	m_nAccept_language(0),
	m_nMsmail_priority(0),
	m_nUser_agent(0),
	m_nApparently_to(0),
	m_nId(0),
	m_nEgroups_return(0),
	m_nList_unsubscribe(0),
	m_nList_subscribe(0),
	m_nXList_unsubscribe(0),
	m_nXList_subscribe(0),
	m_nAuto_submitted(0),
	m_nOriginating_ip(0),
	m_nMailer_version(0),
	m_nList_id(0),
	m_nList_post(0),
	m_nList_help(0),
	m_nErrors_to(0),
	m_nMailing_list(0),
	m_nMs_has_attach(0),
	m_nMs_tnef_correlator(0),
	m_nMime_autoconverted(0),
	m_nContent_class(0),
	m_nPrecedence(0),
	m_nOriginalarrivaltime(0),

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	m_nStartOffsetOfHeader(-1),
	m_nEndingOffsetOfHeader(-1),
	m_pMIMEAttributes(NULL),
	m_nKeyID(-1),
	m_nKeyOffset(-1),
	m_nValueStartOffset(-1),
#endif //*** End AS400 Attribute API Removal *** 

	m_bHasXDashPrefix(false),
	m_eIdentificationLevel(idDisabled),	// Middle of the road.
	m_nIdentifiedHeaderLines(0),
	m_nUnidentifiedHeaderLines(0),
	m_nMalformaties(0),
	m_pDecomposer(NULL),
	m_pEngine(NULL),
	m_pObject(NULL),
	m_pArchive(NULL),
	m_ParseState(MIME_PARSER_STATE_HEADERFIELD1),
	m_fpTokenFile(NULL),
	m_pEmitBuffer(NULL),
	m_nEmitBufferOffset(0),
	m_fpWarningFile(NULL),
	m_pszTokenFile(NULL),
	m_pszWarningFile(NULL),
	m_pszBoundaryTest(NULL),
	m_bIdentified(false),
	m_bMultipart(false),
	m_bInPreamble(false),
	m_bInMultipartHeader(false),
	m_bScanForPairs(false),
	m_bInHeader(true),  // We always start parsing header lines.
	m_bWantBytes(true),
	m_iEncoding(MIME_TOKEN_NONE),
	m_iEncryption(0),
	m_nLineNumber(1),
	m_nHeaderLinesIDed(0),
	m_nHeaderWarnings(0),
	m_nWarnings(0),
	m_nBoundaryStart(0),
	m_nBoundaryCount(0),
	m_nBoundaryNestLevel(0),
	m_nSectionStart(0),
	m_nMultipartCount(0),
	m_pszName(NULL),
	m_dwYear(1980),
	m_dwMonth(1),
	m_dwDay(1),
	m_dwHour(0),
	m_dwMinute(0),
	m_dwSecond(0),
	m_iTimezoneDifferential(0),
    m_iFieldID(MIME_TOKEN_UNKNOWN_FIELD),
	m_iAtomCandidateBufSize(0),
	m_nAtomCandidateOffset(0),
	m_pString(NULL),
	m_chTranslateNULL(ASC_CHR_PERIOD),
	m_bFuzzyHeader(false),
	m_dwMinScanBytes(0),

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	m_addMultipartAttributes(false),
#endif //*** End AS400 Attribute API Removal *** 

	m_bBareTerminatorHasBeenCounted(false),
	m_nCharsetValueOffset(0),
	m_eSigState(NONE),
	m_fX_pkcs7_mime(false),
	m_fEmitWarning(false),
	m_nSignedDataTokenCnt(0),
	m_bExtractMessageBodies(true),
	m_fIsProcessingSignature(false)
{
	m_szCharset[0] = 0;
	// Set default date/time to Jan 1, 1980 00:00:00
	m_AtomCandidate.reset();

	//
	// Option defaults
	//
	m_pszBoundary.initializeMemory(true);
	m_pszBoundary.reset();

}


CMIMEParser::~CMIMEParser()
{
	if (m_pEmitBuffer)
	{
		free(m_pEmitBuffer);
		m_pEmitBuffer = NULL;
	}

	if (m_fpTokenFile)
	{
		dec_fclose(m_fpTokenFile);
		m_fpTokenFile = NULL;
	}

	if (m_pszTokenFile)
	{
		dec_remove(m_pszTokenFile);
		free(m_pszTokenFile);
		m_pszTokenFile = NULL;
	}

	if (m_fpWarningFile)
	{
		dec_fclose(m_fpWarningFile);
		m_fpWarningFile = NULL;
	}

	if (m_pszWarningFile)
	{
		dec_remove(m_pszWarningFile);
		free(m_pszWarningFile);
		m_pszWarningFile = NULL;
	}

	FreeBoundaryTest();

	if (m_pszName)
	{
		free(m_pszName);
		m_pszName = NULL;
	}

	for (size_t index = 0; index < m_nBoundaryCount; index++)
	{
		if (m_pszBoundary[index].m_pszBoundary)
		{
			free(m_pszBoundary[index].m_pszBoundary);
			m_pszBoundary[index].m_pszBoundary = NULL;
		}
	}

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	if ( m_pMIMEAttributes != NULL )
	{
		delete m_pMIMEAttributes;
	}
#endif //*** End AS400 Attribute API Removal *** 
}

int CMIMEParser::ProcessHeaderAtom(void)
{
	int		rc		= PARSER_OK;
	m_bBoundarySeparatorFoundInHeader = false;

	if ( m_AtomCandidate.getPosition() != 0 )
	{
		char*	save_m_pszBoundaryTest = m_pszBoundaryTest;
		char*	save_m_pString	= m_pString;
		int		save_m_nStringLength = m_nStringLength;
		int		save_m_nOffset = m_nOffset;

		m_pString		= &m_AtomCandidate[0];
		m_nStringLength = strlen(m_pString);

		// This might be the beginning of a message header.  See if
		// we can identify the atom as a well-known message header field.
		m_iFieldID = IdentifyAtom(MIME_ATOM_GROUP_HEADER_FIELDS);


		// If we have not identified the token and this is multipart
		// mime then we need to see if this is a boundary separator
		// stuck in the header section.  If so, then force a header
		// separator token and start processing the body.  We only
		// look for the separator after we have encountered a 
		// Content-Type containing a boundary separator.

		if (m_iFieldID == MIME_TOKEN_LAST && m_bMultipart)
		{
			// See if we have a malformed case of no break before the 
			// bounadary marker...

			// According to RFC 1521, all multipart boundaries *must*
			// be preceeded by a CRLF sequence.  We can't really ignore
			// this rule because if we do, we will get lots of false-positive
			// boundary detections.
			m_pszBoundaryTest = &m_pString[2];
			if (m_nAtomCandidatePreviousToken == MIME_TOKEN_CRLF &&
				m_nStringLength >= 2 &&
				m_pString[0] == ASC_CHR_MINUS &&
				m_pString[1] == ASC_CHR_MINUS &&
				CheckForBoundary() != MIME_NOT_BOUNDARY
				)
			{
				if (m_nHeaderLinesIDed != 0)
					m_bIdentified = true;

				m_bBoundarySeparatorFoundInHeader = true;
				// Warn that we've found a problem with this file...
				EmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);

				m_nBoundaryStart = m_nOffset-m_nStringLength;

				EMIT_OFFSET = m_nBoundaryStart;

				EMIT_ID = MIME_TOKEN_HEADER_SEPARATOR;
				EMIT_DATA = 2;
				DO_EMIT;

				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;

				EMIT_ID = MIME_TOKEN_PREAMBLE;
				DO_EMIT;  // Emit PREAMBLE

				EMIT_ID = MIME_TOKEN_END_MARKER;
				EMIT_DATA = 0;
				DO_EMIT;

				m_bInPreamble = false;

				int save_m_iTokenID = m_iTokenID;
				m_iTokenID = MIME_TOKEN_CRLF;
				rc = State_MULTIPART2();
				m_iTokenID = save_m_iTokenID;

				m_pszBoundaryTest = save_m_pszBoundaryTest;
				m_AtomCandidate.reset();
				m_iAtomCandidateBufSize = 0;
				m_pString = save_m_pString;
				m_nStringLength = save_m_nStringLength;
				m_nOffset = save_m_nOffset;

				EMIT_OFFSET = save_m_nOffset;
				m_nLineOffset = EMIT_OFFSET;
				return PARSER_IDENTIFIED;
			}
			else
			{
				m_pszBoundaryTest = save_m_pszBoundaryTest;
				// We did not identify the field.
				m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;
			}
		}
		else if (m_iFieldID == MIME_TOKEN_LAST )
		{
			// We did not identify the field.
			m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;
		}
		m_AtomCandidate.reset();
		m_iAtomCandidateBufSize = 0;
		m_pString = save_m_pString;
		m_nStringLength = save_m_nStringLength;
		m_nOffset = save_m_nOffset;

		EMIT_OFFSET = m_nAtomCandidateOffset;
		EMIT_ID = m_iFieldID;
		DO_EMIT;  // Emit the identified field.
		m_nLineOffset = EMIT_OFFSET;
	}
	return rc;
}

int CMIMEParser::GetContentType() const
{
	return m_nContentTypeID;
}

bool	CMIMEParser::isAttachment() const
{
	return m_bAttachment;
}

bool CMIMEParser::isFuzzy() const
{
	return m_bFuzzyHeader;
}


size_t CMIMEParser::Percentage(size_t thisVaraible,size_t relativeToThisVaraible) const
{
	size_t result = 0;

	// Calculate the unit size.

	float divideBy = (float)thisVaraible+(float)relativeToThisVaraible;
	
	if (divideBy >  0 )
	{
		float unitSize = 100/divideBy;
		float fResult = unitSize*thisVaraible;
		result = (size_t)fResult;
	}

	return result;
}


bool CMIMEParser::GetIdentificationProfile(MIMEIdentificationProfile& fillin) const
{
	// Currently we support 5 levels of ID from weak to strong
	MIMEIdentificationProfile fillin_result;
	
	bool result = true;

	switch ( m_eIdentificationLevel )
	{
		case idDisabled: {
							result = false;
							break;
						 }
		case idLevel1: {
							fillin_result.SetMinNumberOfKnownKeywords(2);
							fillin_result.SetMinKeywordWeighting(0);
							fillin_result.SetMaxPercentageOfMalformaties(90);
							fillin_result.SetMaxMalformaties(100);
							fillin_result.SetMinPercentageIdentifiedHeaderLines(20);
							break;
						}
		case idLevel2: {
							fillin_result.SetMinNumberOfKnownKeywords(3);
							fillin_result.SetMinKeywordWeighting(40);
							fillin_result.SetMaxPercentageOfMalformaties(70);
							fillin_result.SetMaxMalformaties(40);
							fillin_result.SetMinPercentageIdentifiedHeaderLines(30);
							break;
						}
		case idLevel3: {
							fillin_result.SetMinNumberOfKnownKeywords(4);
							fillin_result.SetMinKeywordWeighting(60);
							fillin_result.SetMaxPercentageOfMalformaties(60);
							fillin_result.SetMaxMalformaties(20);
							fillin_result.SetMinPercentageIdentifiedHeaderLines(50);
							break;
						}
		case idLevel4: {
							fillin_result.SetMinNumberOfKnownKeywords(5);
							fillin_result.SetMinKeywordWeighting(165);
							fillin_result.SetMaxPercentageOfMalformaties(45);
							fillin_result.SetMaxMalformaties(10);
							fillin_result.SetMinPercentageIdentifiedHeaderLines(70);
							break;
						}
		case idLevel5: {
							fillin_result.SetMinNumberOfKnownKeywords(5);
							fillin_result.SetMinKeywordWeighting(300);
							fillin_result.SetMaxPercentageOfMalformaties(10);
							fillin_result.SetMaxMalformaties(3);
							fillin_result.SetMinPercentageIdentifiedHeaderLines(100);
							break;
						}
	}

	fillin = fillin_result;

	return result;
}


bool CMIMEParser::isValidMIMEHeader() const
{
	bool isMIMEMessage = false;	// assume the worst

	if ( m_nIdentifiedHeaderLines > 0 )
	{

		// See if we need to account for malformed header lines.  

		MIMEIdentificationProfile idProfile;
		if ( GetIdentificationProfile(idProfile) )
		{
			// Profiles are enabled since GetIdentificationProfile returned true;

			if ( isFuzzy() )
			{
				// Exclude malformed lines if user wants us
				// to ignore malformaties in the message.
				if (	m_nIdentifiedHeaderLines >= idProfile.GetMinNumberOfKnownKeywords()
					 && Percentage(m_nIdentifiedHeaderLines,m_nUnidentifiedHeaderLines ) >= idProfile.GetMinPercentageIdentifiedHeaderLines()
					 && m_nKeywordWeighting >= idProfile.GetMinKeywordWeighting() )
				{
					isMIMEMessage = true;
				}
			}
			else
			{

				if (	m_nIdentifiedHeaderLines >= idProfile.GetMinNumberOfKnownKeywords()
					 && m_nMalformaties <= idProfile.GetMaxMalformaties() 
					 &&	Percentage(m_nMalformaties,(m_nIdentifiedHeaderLines+m_nUnidentifiedHeaderLines)) <= idProfile.GetMaxPercentageOfMalformaties()
					 && Percentage(m_nIdentifiedHeaderLines,m_nUnidentifiedHeaderLines ) >= idProfile.GetMinPercentageIdentifiedHeaderLines()				 
					 && m_nKeywordWeighting >= idProfile.GetMinKeywordWeighting()  )
				{
					isMIMEMessage = true;
				}
			}
#if DUMP_MIME_PROFILE_EVAL


			printf("\nMIME Profile Evaluation for profile %d.  format ('required','actual'). 'isMIMEMessage'=%s\n\n",(int)m_eIdentificationLevel,isMIMEMessage ? "true": "false");
printf("--> Number identified lines: (%d,%d) \t\t%s\n",
						idProfile.GetMinNumberOfKnownKeywords(),
						m_nIdentifiedHeaderLines,
						(m_nIdentifiedHeaderLines >= idProfile.GetMinNumberOfKnownKeywords() ? "PASSED" : "FAILED") );
printf("--> Keyword Weighting:       (%d,%d) \t\t%s\n",
						idProfile.GetMinKeywordWeighting(),
						m_nKeywordWeighting,
						(m_nKeywordWeighting >= idProfile.GetMinKeywordWeighting() ? "PASSED" : "FAILED" ) );
if ( !isFuzzy() )
{
	printf("--> Max %% malformities:      (%d,%d) \t\t%s\n",
							idProfile.GetMaxPercentageOfMalformaties(),
							Percentage(m_nMalformaties,(m_nIdentifiedHeaderLines+m_nUnidentifiedHeaderLines)),
							( Percentage(m_nMalformaties,(m_nIdentifiedHeaderLines+m_nUnidentifiedHeaderLines)) <= idProfile.GetMaxPercentageOfMalformaties()
							? "PASSED" : "FAILED") );
	printf("--> Max abs. malformities:   (%d,%d) \t\t%s\n",
							idProfile.GetMaxMalformaties() ,
							m_nMalformaties,
							( m_nMalformaties <= idProfile.GetMaxMalformaties() ? "PASSED" : "FAILED") );
}
else
{
	printf("--> Max %% malformities:      (%d,%d) \t\t%s\n",
							idProfile.GetMaxPercentageOfMalformaties(),
							Percentage(m_nMalformaties,(m_nIdentifiedHeaderLines+m_nUnidentifiedHeaderLines)),
							( Percentage(m_nMalformaties,(m_nIdentifiedHeaderLines+m_nUnidentifiedHeaderLines)) <= idProfile.GetMaxPercentageOfMalformaties()
							? "IGNORED/FUZZY" : "IGNORED/FUZZY") );
	printf("--> Max abs. malformities:   (%d,%d) \t\t%s\n",
							idProfile.GetMaxMalformaties() ,
							m_nMalformaties,
							( m_nMalformaties <= idProfile.GetMaxMalformaties() ? "IGNORED/FUZZY" : "IGNORED/FUZZY") );
}
printf("--> Min %% known headers:     (%d,%d) \t\t%s\n\n",
						idProfile.GetMinPercentageIdentifiedHeaderLines() ,
						Percentage(m_nIdentifiedHeaderLines,m_nUnidentifiedHeaderLines ),
						( Percentage(m_nIdentifiedHeaderLines,m_nUnidentifiedHeaderLines ) >= idProfile.GetMinPercentageIdentifiedHeaderLines() 
						? "PASSED" : "FAILED" ) );

#endif
		}
		else
		{
			// Profiles are disabled so we are using IdentifiedHeaderLines
			// as our sole means of identification.
			isMIMEMessage = true;
		}
	}
	else
	{
		// No valid header lines found.
		isMIMEMessage = false;
	}

	// if the file was identified as MALFORMED mime we need to reset
	// the malformed flag to indicate that it is NOT malformed mime.

	if ( isMIMEMessage == false )
	{
		m_pObject->SetMalformed(DEC_TYPE_UNKNOWN);
	}

	return isMIMEMessage;
}



bool CMIMEParser::Init(CMIMELexer* pLexer,CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject)
{
	m_pLexer = pLexer;
	m_bHasXDashPrefix = false;
	m_eIdentificationLevel = idDisabled;	// Middle of the road.
	m_nKeywordWeighting = 0;
	m_bEndOfHeaderFound = false;
	m_nIdentifiedHeaderLines = 0;
	m_nUnidentifiedHeaderLines = 0;
	m_nMalformaties = 0;
	m_bAttachment = false;
	m_nContentTypeID = -1;
	m_AtomCandidate.reset();
	m_iAtomCandidateBufSize = 0;
	m_bIdentified = false;
	m_bMultipart = false;
	m_bInPreamble = false;
	m_bInMultipartHeader = false;
	m_bScanForPairs = false;
	m_bInHeader = true;  // We always start parsing header lines.
	m_bWantBytes = true;
	m_pArchive = pArchive;
	m_pEngine = pEngine;
	m_pDecomposer = pDecomposer;
	m_pObject = pObject;
	m_iPreviousTokenID = MIME_TOKEN_LAST;
	m_nHeaderLinesIDed = 0;
	m_nHeaderWarnings = 0;
	m_nWarnings = 0;
	m_nLineNumber = 1;
	m_nBoundaryStart = 0;
	m_nBoundaryCount = 0;
	m_nBoundaryNestLevel = 0;
	m_nSectionStart = 0;
	m_pszName = NULL;
	m_szCharset[0] = 0;
    m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;
	m_pString = NULL;
	m_nAtomCandidateOffset = 0;
	m_eSigState = NONE,
	m_fX_pkcs7_mime = false;
	m_fEmitWarning = false;
	m_nSignedDataTokenCnt = 0;
	m_fIsProcessingSignature = false;

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	m_nStartOffsetOfHeader = -1;
	m_nEndingOffsetOfHeader = -1;
	m_addMultipartAttributes = false;
#endif //*** End AS400 Attribute API Removal *** 

	m_bBareTerminatorHasBeenCounted = false;
	m_nCharsetValueOffset = 0;


#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	if ( m_pMIMEAttributes != NULL ) 
	{
		delete m_pMIMEAttributes;
	}

	m_pMIMEAttributes = new CMIMEAttributeCollection();
	m_nKeyID = -1;
	m_nKeyOffset = -1;
	m_nValueStartOffset = -1;
#endif //*** End AS400 Attribute API Removal *** 


	DWORD dwValue = 0;

	//
	// Set any applicable MIME options here
	//
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_TRANSLATE_NULL, &dwValue)))
		m_chTranslateNULL = (char)(dwValue & 0x000000ff);
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_MIME_FUZZY_MAIN_HEADER, &dwValue)))
		m_bFuzzyHeader = (dwValue ? true : false);
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_MIN_MIME_SCAN_BYTES, &dwValue)))
		m_dwMinScanBytes = dwValue;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_MIME_IDENTIFICATION_STRENGTH, &dwValue)))
	{
		IdentificationLevel level = (IdentificationLevel)dwValue;

		switch ( level ) 
		{
			case idDisabled:{	m_eIdentificationLevel  = idDisabled;
								break;
							}
			case idLevel1: {
								m_eIdentificationLevel = idLevel1;
								break;
							};
			case idLevel2: {
								m_eIdentificationLevel = idLevel2;
								break;
							};
			case idLevel3: {
								m_eIdentificationLevel = idLevel3;
								break;
							};
			case idLevel4: {
								m_eIdentificationLevel = idLevel4;
								break;
							};
			case idLevel5: {
								m_eIdentificationLevel = idLevel5;
								break;
							};
			default:		{
								m_eIdentificationLevel = idLevel3;
								
							};
		}
	}

	m_pszBoundary.initializeMemory(true);
	m_pszBoundary.reset();
	m_bExtractMessageBodies = true;

	// Initialize the counters...

	m_nReturn_Path = 0;
	m_nReceived = 0;
	m_nReply_to = 0;
	m_nFrom = 0;
	m_nSender = 0;
	m_nDate = 0;
	m_nTo = 0;
	m_nCc = 0;
	m_nBcc = 0;
	m_nMessage_id = 0;
	m_nIn_reply_to = 0;
	m_nReferences = 0;
	m_nKeywords = 0;
	m_nSubject = 0;
	m_nComments = 0;
	m_nEncrypted = 0;
	m_nContent_type = 0;
	m_nContent_disposition = 0;
	m_nContent_description = 0;
	m_nContent_transfer_encoding = 0;
	m_nContent_id = 0;
	m_nMime_version = 0;
	m_nImportance = 0;
	m_nOrganization = 0;
	m_nAuthor = 0;
	m_nPriority = 0;
	m_nDelivered_to = 0;
	m_nContent_location = 0;
	m_nThread_topic = 0;
	m_nThread_index = 0;
	m_nMailer = 0;
	m_nContent_length = 0;
	m_nHelo = 0;
	m_nData = 0;
	m_nAccept_language = 0;
	m_nMsmail_priority = 0;
	m_nUser_agent = 0;
	m_nApparently_to = 0;
	m_nId = 0;
	m_nEgroups_return = 0;
	m_nList_unsubscribe = 0;
	m_nList_subscribe = 0;
	m_nXList_unsubscribe = 0;
	m_nXList_subscribe = 0;
	m_nAuto_submitted = 0;
	m_nOriginating_ip = 0;
	m_nMailer_version = 0;
	m_nList_id = 0;
	m_nList_post = 0;
	m_nList_help = 0;
	m_nErrors_to = 0;
	m_nMailing_list = 0;
	m_nMs_has_attach = 0;
	m_nMs_tnef_correlator = 0;
	m_nMime_autoconverted = 0;
	m_nContent_class = 0;
	m_nPrecedence = 0;
	m_nOriginalarrivaltime = 0;
	m_nMimeole = 0;
	m_nList_archive = 0;

	return true;
}


int CMIMEParser::ConsumeToken(
	int iTokenID,
	char *pString,
	size_t nStringLength,
	size_t nOffset,
    size_t nLineNumber)
{
	int		rc = PARSER_OK;	// Assume parsing needs to continue

	// Copy the input parameters into the parser object.  These parameters
	// now become the things that drive the state machine.
	m_iTokenID = iTokenID;
	m_pString = pString;
	m_nStringLength = nStringLength;
	m_nOffset = nOffset;
	m_nLineNumber = nLineNumber;


change_state:
	// See if the input token is valid for the state that we are
	// currently in.  If not, then we need to stop parsing because the data
	// is not in a form that this engine is designed to handle.  We check
	// this by simply consulting a matrix of MIME_PARSER_STATE_xxx vs.
	// MIME_TOKEN_xxx.
	// For debug purposes, make sure the input token ID and current state
	// are within the range of the matrix.
	dec_assert(iTokenID < MIME_TOKEN_LAST);
	dec_assert(m_ParseState < MIME_PARSER_STATE_LAST);
	if (m_bFuzzyHeader || m_ParseState > MIME_PARSER_STATE_HEADERFIELD4)
		m_dwFlags = YES;
	else
		m_dwFlags = g_dwMIMEStateMatrix[m_ParseState][iTokenID];

	if (m_dwFlags == NO)
	{
		// See if we have processed the minimum amount of data to make a
		// MIME vs. non-MIME determination.
		if (m_nOffset < m_dwMinScanBytes)
		{
			m_dwFlags = YES;  // No, we haven't so keep looking.
		}
		else
		{
			// The input token is not valid for transitioning out of the current
			// parser state.
			// Are we at EOF?
			if (m_iTokenID == MIME_TOKEN_EOF)
			{
				// Yes, have we identified at least something that looks MIME-ish?
				if (m_bIdentified)
				{
					// Yes, we now have an EOF token with a possible MIME header.
					// Go ahead and call it malformed MIME then.
					EmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);
					return (PARSER_IDENTIFIED);
				}
			}

// *** Fix for W32.Chir@mm virus ***
            if ((m_iFieldID == MIME_TOKEN_HELO || m_iFieldID == MIME_TOKEN_DATA) &&
				(m_ParseState == MIME_PARSER_STATE_HEADERFIELD1 ))
            {
                m_dwFlags = YES;  // We will allow the transition for these two cases.
            }
            else
            {
			// At this point the input data is NOT considered to be
			// MIME (or even invalid MIME).
			// Tell the lexer to stop calling this engine.
			m_iPreviousTokenID = m_iTokenID;
			return (PARSER_DONE);
		}
// *********************************
	}
    }

	if (m_dwFlags == MAL)
	{
		// The input token is valid for transitioning out of the current
		// parser state, but the message should be considered malformed
		// because the transition is not RFC-compliant.
		EmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);
	}

	// Always reset the entire emit structure here.  These fields may
	// get modified as we progress through the state machine.
	EMIT_OFFSET = m_nOffset;
	EMIT_ID = m_iTokenID;
	EMIT_FLAGS = m_dwFlags;
	EMIT_DATA = 0;

	// Where we go next depends on what state the parser is in right now.
	// The parsing always begins in state MIME_PARSER_STATE_HEADERFIELD1.
	// Call the function associated with the current parse state.
/* *** Running through this state table has been shown to be slower than the switch.
	rc = pfnMIMEParserStateTable[m_ParseState](this);
*/

	switch (m_ParseState)
	{
		case MIME_PARSER_STATE_HEADERFIELD1:
			// We are looking for the beginning of a MIME header line.
			rc = State_HEADERFIELD1();
			break;

		case MIME_PARSER_STATE_HEADERFIELD3:
			rc = State_HEADERFIELD3();
			break;

		case MIME_PARSER_STATE_HEADERFIELD4:
			rc = State_HEADERFIELD4();
			break;

		case MIME_PARSER_STATE_RETURNPATH1:
			rc = State_RETURNPATH1();
			break;

		case MIME_PARSER_STATE_RETURNPATH2:
			rc = State_RETURNPATH2();
			break;

		case MIME_PARSER_STATE_RETURNPATH3:
			rc = State_RETURNPATH3();
			break;

		case MIME_PARSER_STATE_ROUTEADDR1:
			rc = State_ROUTEADDR1();
			break;

		case MIME_PARSER_STATE_ROUTEADDR2:
			rc = State_ROUTEADDR2();
			break;

		case MIME_PARSER_STATE_ROUTEADDR3:
			rc = State_ROUTEADDR3();
			break;

		case MIME_PARSER_STATE_RECEIVED1:
			rc = State_RECEIVED1();
			break;

		case MIME_PARSER_STATE_RECEIVED2:
			rc = State_RECEIVED2();
			break;

		case MIME_PARSER_STATE_RECEIVED3:
			rc = State_RECEIVED3();
			break;

		case MIME_PARSER_STATE_RECEIVED4:
			rc = State_RECEIVED4();
			break;

		case MIME_PARSER_STATE_RECEIVED5:
			rc = State_RECEIVED5();
			break;

		case MIME_PARSER_STATE_RECEIVED6:
			rc = State_RECEIVED6();
			break;

		case MIME_PARSER_STATE_RECEIVED7:
			rc = State_RECEIVED7();
			break;

		case MIME_PARSER_STATE_RECEIVED8:
			rc = State_RECEIVED8();
			break;

		case MIME_PARSER_STATE_REPLYTO1:
			rc = State_REPLYTO1();
			break;

		case MIME_PARSER_STATE_FROM1:
			rc = State_FROM1();
			break;

		case MIME_PARSER_STATE_SENDER1:
			rc = State_SENDER1();
			break;

		case MIME_PARSER_STATE_DATE1:
			rc = State_DATE1();
			break;

		case MIME_PARSER_STATE_DATE2:
			rc = State_DATE2();
			break;

		case MIME_PARSER_STATE_DATE3:
			rc = State_DATE3();
			break;

		case MIME_PARSER_STATE_DATE4:
			rc = State_DATE4();
			break;

		case MIME_PARSER_STATE_DATE5:
			rc = State_DATE5();
			break;

		case MIME_PARSER_STATE_DATE6:
			rc = State_DATE6();
			break;

		case MIME_PARSER_STATE_DATE7:
			rc = State_DATE7();
			break;

		case MIME_PARSER_STATE_DATE8:
			rc = State_DATE8();
			break;

		case MIME_PARSER_STATE_DATE9:
			rc = State_DATE9();
			break;

		case MIME_PARSER_STATE_DATE10:
			rc = State_DATE10();
			break;

		case MIME_PARSER_STATE_DATE11:
			rc = State_DATE11();
			break;

		case MIME_PARSER_STATE_TO1:
			rc = State_TO1();
			break;

		case MIME_PARSER_STATE_CC1:
			rc = State_CC1();
			break;

		case MIME_PARSER_STATE_BCC1:
			rc = State_BCC1();
			break;

		case MIME_PARSER_STATE_MESSAGEID1:
			rc = State_MESSAGEID1();
			break;

		case MIME_PARSER_STATE_INREPLYTO1:
			rc = State_INREPLYTO1();
			break;

		case MIME_PARSER_STATE_REFERENCES1:
			rc = State_REFERENCES1();
			break;

		case MIME_PARSER_STATE_KEYWORDS1:
			rc = State_KEYWORDS1();
			break;

		case MIME_PARSER_STATE_SUBJECT1:
			rc = State_SUBJECT1();
			break;

		case MIME_PARSER_STATE_COMMENTS1:
			rc = State_COMMENTS1();
			break;

		case MIME_PARSER_STATE_ENCRYPTED1:
			rc = State_ENCRYPTED1();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE1:
			rc = State_CONTENTTYPE1();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE2:
			rc = State_CONTENTTYPE2();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE3:
			rc = State_CONTENTTYPE3();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE4:
			rc = State_CONTENTTYPE4();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE5:
			rc = State_CONTENTTYPE5();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE6:
			rc = State_CONTENTTYPE6();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE7:
			rc = State_CONTENTTYPE7();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE8:
			rc = State_CONTENTTYPE8();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE9:
			rc = State_CONTENTTYPE9();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE10:
			rc = State_CONTENTTYPE10();
			break;

		case MIME_PARSER_STATE_CONTENTTYPE11:
			rc = State_CONTENTTYPE11();
			break;

		case MIME_PARSER_STATE_CONTENTDISPOSITION1:
			rc = State_CONTENTDISPOSITION1();
			break;

		case MIME_PARSER_STATE_CONTENTDISPOSITION2:
			rc = State_CONTENTDISPOSITION2();
			break;

		case MIME_PARSER_STATE_CONTENTDISPOSITION3:
			rc = State_CONTENTDISPOSITION3();
			break;

		case MIME_PARSER_STATE_CONTENTDISPOSITION4:
			rc = State_CONTENTDISPOSITION4();
			break;

		case MIME_PARSER_STATE_CONTENTDESCRIPTION1:
			rc = State_CONTENTDESCRIPTION1();
			break;

		case MIME_PARSER_STATE_CONTENTTRANSFERENCODING1:
			rc = State_CONTENTTRANSFERENCODING1();
			break;

		case MIME_PARSER_STATE_CONTENTID1:
			rc = State_CONTENTID1();
			break;

		case MIME_PARSER_STATE_MIMEVERSION1:
			rc = State_MIMEVERSION1();
			break;

		case MIME_PARSER_STATE_IMPORTANCE1:
			rc = State_IMPORTANCE1();
			break;

		case MIME_PARSER_STATE_ORGANIZATION1:
			rc = State_ORGANIZATION1();
			break;

		case MIME_PARSER_STATE_AUTHOR1:
			rc = State_AUTHOR1();
			break;

		case MIME_PARSER_STATE_PRIORITY1:
			rc = State_PRIORITY1();
			break;

		case MIME_PARSER_STATE_DELIVEREDTO1:
			rc = State_DELIVEREDTO1();
			break;

		case MIME_PARSER_STATE_CONTENTLOCATION1:
			rc = State_CONTENTLOCATION1();
			break;

		case MIME_PARSER_STATE_THREADTOPIC1:
			rc = State_THREADTOPIC1();
			break;

		case MIME_PARSER_STATE_THREADINDEX1:
			rc = State_THREADINDEX1();
			break;

		case MIME_PARSER_STATE_MAILER1:
			rc = State_MAILER1();
			break;

		case MIME_PARSER_STATE_CONTENTLENGTH1:
			rc = State_CONTENTLENGTH1();
			break;

		case MIME_PARSER_STATE_BODY1:
			rc = State_BODY1();
			break;

		case MIME_PARSER_STATE_BODY2:
			rc = State_BODY2();
			break;

		case MIME_PARSER_STATE_MULTIPART1:
			rc = State_MULTIPART1();
			break;

		case MIME_PARSER_STATE_MULTIPART2:
			rc = State_MULTIPART2();
			break;

		case MIME_PARSER_STATE_MULTIPART3:
			rc = State_MULTIPART3();
			break;

		case MIME_PARSER_STATE_MULTIPARTHEADER1:
			rc = State_MULTIPARTHEADER1();
			break;

		case MIME_PARSER_STATE_MULTIPARTHEADER3:
			rc = State_MULTIPARTHEADER3();
			break;

		case MIME_PARSER_STATE_MULTIPARTHEADER4:
			rc = State_MULTIPARTHEADER4();
			break;

		case MIME_PARSER_STATE_MULTIPARTBODY1:
			rc = State_MULTIPARTBODY1();
			break;

		case MIME_PARSER_STATE_MULTIPARTBODY2:
			rc = State_MULTIPARTBODY2();
			break;

		case MIME_PARSER_STATE_MULTIPARTBODY3:
			rc = State_MULTIPARTBODY3();
			break;

		case MIME_PARSER_STATE_MULTIPARTBODY4:
			rc = State_MULTIPARTBODY4();
			break;

		default:
			dec_assert(0);	// We should never fall outside the state machine!
			break;
	}

	// Save off the last token ID (used for look-behind).
	m_iPreviousTokenID = m_iTokenID;

	if (rc == PARSER_CHANGE_STATE)
		goto change_state;

	return (rc);
}


int CMIMEParser::ParseComplete(void)
{
	int		rc = PARSER_OK;

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	m_pObject->FinalizeAttributes();
#endif //*** End AS400 Attribute API Removal *** 

	// Close the token file when we are all through with the parse.
	// Clients need to call IMIME::GetTokenFilename to get the
	// token file's filename.
	if (m_fpTokenFile)
	{
		if (m_pEmitBuffer && m_nEmitBufferOffset != 0)
		{
			if (dec_fwrite(m_pEmitBuffer, 1, m_nEmitBufferOffset, m_fpTokenFile) != m_nEmitBufferOffset)
			{
				// Failed to write to the intermediate output file.
				// We can't possibly parse the input data so return an error.
				rc = PARSER_ERROR;
			}

			m_nEmitBufferOffset = 0;
		}

		// It's OK for this close call to fail.  The client will
		// encounter some kind of open or read error later on when
		// it goes to access the token file.
		dec_fclose(m_fpTokenFile);
		m_fpTokenFile = NULL;
	}

	// Close the warning file when we are all through with the parse.
	// Clients need to call IMIME::GetWarningFilename to get the
	// warning file's filename.
	if (m_fpWarningFile)
	{
		// It's OK for this close call to fail.  The client will
		// encounter some kind of open or read error later on when
		// it goes to access the warning file.
		dec_fclose(m_fpWarningFile);
		m_fpWarningFile = NULL;
	}

	return (rc);
}

bool CMIMEParser::IsProcessingSignature()
{
	return m_fIsProcessingSignature;
}
bool CMIMEParser::IsValidWarningDelay()
{
	bool IsValidDelay = false;

	// Look for an ASN.1 opaque digital signature at the top of the mime container.
	// From wincrypt.h RSA_signedData 06 09 2A 86 48 86 F7 0D 01 07 02

	char* szDebug = &m_AtomCandidate[0];

	// The Lexer sends us (...) (06) (09) (2A 86 48 86 F7) (0D) (01) (07) (02 ...)
	if( m_eSigState == SIGNED_DATA_CANDIDATE )
	{
		if( m_pString[0] == '\x01' && m_pString[1] == 0 )
		{
			m_eSigState = SIGNED_DATA;

			m_fEmitWarning	= false;
			IsValidDelay	= true;

			m_nSignedDataTokenCnt = 0;
		}
	}
	else if( m_eSigState == FIRST_BYTE )
	{
		if( m_pString[0] == '\x2A' )
		{
			if( binary_stricmp( m_pString, STR_ASN1_PARTIAL_ID, STR_ASN1_PARTIAL_ID ) == 0 )
			{
				// Bump our counter up by 2 to account for the trailing \x0D
				m_nSignedDataTokenCnt+=2;
				// At this point we are very likely to have signed data. Continue to delay any malformed warnings.
				m_eSigState = SIGNED_DATA_CANDIDATE;

				// The lexer can request the value of this variable
				m_fIsProcessingSignature = true;

				IsValidDelay = true;
			}
		}
	}
	else if( m_pString[0] == '\x06' && m_pString[1] == 0 )
	{
		if( m_eSigState != SIGNED_DATA )
		{
			// This may be signed data. Delay any malformed warnings for 1 iteration.
			m_nSignedDataTokenCnt++;

			m_eSigState = FIRST_BYTE;

			IsValidDelay = true;
		}
	}

	return IsValidDelay;
}
bool CMIMEParser::IsAtomInSignature()
{
	// Save our original base address
	char* pSave = m_pString;

	// Check each index in the array for the keyword.
	for( int j= 0; j < (signed int)m_nStringLength - 1; j++ )
	{
		// Set the starting point
		m_pString = &m_pString[j];
		// Attempt to id it.
		int iID = IdentifyAtom(MIME_ATOM_GROUP_HEADER_FIELDS);
		m_pString = pSave;

		if( iID != MIME_TOKEN_LAST )
		{
			// We have identified a keyword. Advance beyond the signature.
			m_nOffset += j;
			m_pString = &m_pString[j];
			m_nStringLength = m_nStringLength - j;

			// This is our first true atom candidate, so start at zero.
			m_iAtomCandidateBufSize = 0;
			// We are now parsing beyond the signature.
			m_eSigState = NONE;

			// The lexer can request the value of this variable
			m_fIsProcessingSignature = false;

			// We have a valid mime header
			m_eIdentificationLevel = idDisabled;

			return true;
		}
	}

	return false;

}
void CMIMEParser::SetEmitWarning(size_t nWarningID, size_t nContextTokenID)
{
	if( m_eSigState != SIGNED_DATA )
	{
		m_fEmitWarning = true;
		m_nDelayedWarningID = nWarningID;
		m_nDelayedContextTokenID = nContextTokenID;
	}
}
void CMIMEParser::TriggerWarning()
{
	if( m_fEmitWarning && m_nSignedDataTokenCnt == 0 && m_eSigState != SIGNED_DATA )
	{
		EmitWarning( m_nDelayedWarningID, m_nDelayedContextTokenID );

		m_fEmitWarning = false;

		m_eSigState = NONE;
	}
	else if( m_fEmitWarning )
	{
		m_nSignedDataTokenCnt--;
	}
}
void CMIMEParser::EmitWarning(size_t nWarningID, size_t nContextTokenID)
{
	DECRESULT	hr;
	MIMEWARNING mw;

	// Only set the malformed flag if we have been able to determine that the
	// file looks like MIME.
	if (m_bIdentified)
	{
		// The MIME engine has issued a warning.  Always set the malformed
		// flag for this object so that clients can know when some kind of
		// malformed MIME message has been detected.
		m_pObject->SetMalformed(DEC_TYPE_MIME);
	}

	if ( (nWarningID == MIME_WARNING_BARE_CARRIAGE_RETURN || nWarningID == MIME_WARNING_BARE_LINE_FEED)  )
	{
		if ( m_bBareTerminatorHasBeenCounted == false )
		{	
			m_nMalformaties++;
			m_bBareTerminatorHasBeenCounted = true;
		}
	}
	else
	{
		m_nMalformaties++;
	}

	if (!g_bLogWarnings)
		return;

	if (m_fpWarningFile == NULL)
	{
		// We have not created a warning file yet.
		// Do it now.
		m_pszWarningFile = (char *)malloc(MAX_PATH);
		if (!m_pszWarningFile)
		{
			// Failed to allocate memory for the warning file's name.
			return;
		}

		// Create a new temporary file here that we will dump the warning
		// information into.
		hr = m_pEngine->TextCreateTempFile(".wrn", m_pszWarningFile, m_pDecomposer);
		if (FAILED(hr))
		{
			// Failed to create the warning file.
			free(m_pszWarningFile);
			m_pszWarningFile = NULL;
			return;
		}

		m_fpWarningFile = dec_fopen(m_pszWarningFile, "w+b");
		if (!m_fpWarningFile)
		{
			// Failed to create the warning file.
			free(m_pszWarningFile);
			m_pszWarningFile = NULL;
			return;
		}
	}

	// Only write up to 5 invalid header warnings.
	if (nWarningID == MIME_WARNING_INVALID_HEADER)
	{
		if (m_nHeaderWarnings > 4)
			return;	// Do not emit more than 5 invalid header warnings.

		m_nHeaderWarnings++;
	}

	// Do not emit more than 100 total warnings.
	if (m_nWarnings > 100)
		return;

	m_nWarnings++;
	mw.nWarningID = nWarningID;
	mw.nContextTokenID = nContextTokenID;
	mw.nLineNumber = m_nLineNumber;
	mw.nOffset = m_nOffset;
	mw.nLength = m_nStringLength;
	dec_fwrite(&mw, 1, sizeof(mw), m_fpWarningFile);
	if (m_nStringLength)
		dec_fwrite(&m_pString[0], 1, m_nStringLength, m_fpWarningFile);
}


inline bool CMIMEParser::EmitToken(void)
{
	DECRESULT	hr;

	if (m_fpTokenFile == NULL)
	{
		// If the token file is closed at this point, there had better not be
		// a filename allocated already.  If so, then we have a logic problem.
		dec_assert(m_pszTokenFile == NULL);

		// We have not created our intermediate output file yet.
		// Do it now.
		m_pszTokenFile = (char *)malloc(MAX_PATH);
		if (!m_pszTokenFile)
		{
			// Failed to allocate memory for the intermediate output
			// file's name.  We can't possibly parse the input data
			// so return an error.
			return false;
		}

		// Create a new temporary file here that we will dump the token
		// information into.
		hr = m_pEngine->TextCreateTempFile(".tok", m_pszTokenFile, m_pDecomposer);
		if (FAILED(hr))
		{
			// Failed to create intermediate output file.  We can't possibly
			// parse the input data so return an error.
			free(m_pszTokenFile);
			m_pszTokenFile = NULL;
			return false;
		}

		m_fpTokenFile = dec_fopen(m_pszTokenFile, "w+b");
		if (!m_fpTokenFile)
		{
			// Failed to create the intermediate output file.
			// We can't possibly parse the input data so return an error.
			free(m_pszTokenFile);
			m_pszTokenFile = NULL;
			return false;
		}
	}

	// Optimization.  Buffer up the tokens to avoid calling dec_fwrite so much.
	// Allocate the emit buffer if it has not already been allocated.
	if (m_pEmitBuffer == NULL)
	{
		m_pEmitBuffer = (unsigned char *)malloc(MAX_EMIT_BUFFER_SIZE);
		if (!m_pEmitBuffer)
			return false;
	}

	// Is the emit buffer full?
	if (m_nEmitBufferOffset > (MAX_EMIT_BUFFER_SIZE - sizeof(MIMEEMITTOKEN) - 1))
	{
		// Yes, the emit buffer is full.  Flush it.
		if (dec_fwrite(m_pEmitBuffer, 1, m_nEmitBufferOffset, m_fpTokenFile) != m_nEmitBufferOffset)
		{
			// Failed to write to the intermediate output file.
			// We can't possibly parse the input data so return an error.
			return false;
		}

		m_nEmitBufferOffset = 0;
	}

	memcpy(&m_pEmitBuffer[m_nEmitBufferOffset], &m_emit, sizeof(MIMEEMITTOKEN));
	m_nEmitBufferOffset += sizeof(MIMEEMITTOKEN);

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	// We need to capture any attribute that may have been generated.
	if (	m_emit.iTokenID == MIME_TOKEN_END_MARKER 
		&&	m_pMIMEAttributes != NULL 
		&&	m_nKeyID != -1
		&&	m_nKeyOffset != -1
		&&	m_nValueStartOffset != -1
		&&	(int)m_nOffset >= m_nValueStartOffset)
	{
		m_pMIMEAttributes->AddKeyValue(m_nKeyID,m_nKeyOffset,m_nValueStartOffset,m_nOffset-m_nValueStartOffset);
		m_nKeyID = -1;
		m_nKeyOffset = -1;
		m_nValueStartOffset = -1;
	}

	if ( m_emit.iTokenID == MIME_TOKEN_HEADER_SEPARATOR )
	{
		m_bEndOfHeaderFound = true;
		m_nEndingOffsetOfHeader = m_emit.nTokenOffset;
		// Now we need to do something with the starting and ending tokens
		{

			if ( !(m_nEndingOffsetOfHeader < m_nStartOffsetOfHeader) )
			{
				bool atEOF = (dec_feof(m_pArchive->m_pFile) != 0);

				long position = dec_ftell(m_pArchive->m_pFile);
				if ( position != -1 && dec_fseek(m_pArchive->m_pFile,m_nStartOffsetOfHeader,SEEK_SET) == 0 )
				{
					char* headerBuffer = new char[m_nEndingOffsetOfHeader-m_nStartOffsetOfHeader+1];
					
					if ( headerBuffer != NULL )
					{
						headerBuffer[m_nEndingOffsetOfHeader-m_nStartOffsetOfHeader] = 0;
						if ( dec_fread(headerBuffer,1,m_nEndingOffsetOfHeader-m_nStartOffsetOfHeader,m_pArchive->m_pFile) == 
							 (unsigned int)(m_nEndingOffsetOfHeader-m_nStartOffsetOfHeader) )
						{
							if ( dec_fseek(m_pArchive->m_pFile,position,SEEK_SET) == 0 )
							{
								if ( atEOF ) 
								{	// Read one byte so that dec_feof will return the correct value at
									// our use of the file.
									char oneByte;
									dec_fread(&oneByte,1,1,m_pArchive->m_pFile);
								}
								size_t nbufferSizeFillin;
								if ( m_pMIMEAttributes->GetRawHeaderBuffer(nbufferSizeFillin) == NULL )
								{
									m_pMIMEAttributes->AdoptHeaderBuffer(headerBuffer, m_nEndingOffsetOfHeader-m_nStartOffsetOfHeader);
									// Note the attributes will be added by the Lexer for the first header.
									// Headers found in the multipart section need to be added by this routine.
									// We can't add the primary header attributes here since the file may later be rejected
									// as MIME by MIME profiles.
									if ( m_addMultipartAttributes )
									{
										m_pObject->SetAttributesSupported();
										m_pObject->AddAttributeCollection(m_pMIMEAttributes);
										m_pMIMEAttributes = new CMIMEAttributeCollection();
									}
								}
								else
								{
									// Something really bad just happened. We should not be using an active Attribute set.
									// Delete the storage we allocated above before we leave.
									delete [] headerBuffer;
								}
							}
						}
					}
				}
			}
		}

		m_nStartOffsetOfHeader = -1;
		m_nEndingOffsetOfHeader = -1;

	}
#else
	if ( m_emit.iTokenID == MIME_TOKEN_HEADER_SEPARATOR )
	{
		m_bEndOfHeaderFound = true;
	}
#endif //*** End AS400 Attribute API Removal *** 

	m_lastEmit = m_emit;

	return true;
}

void CMIMEParser::FileDeterminedToBeMIME()
{
	// If the file has been determined to be MIME then we
	// need to process any attributes that may be pending.

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 

	size_t nbufferSizeFillin;
					

	if (	m_pObject != NULL 
		&&	m_pMIMEAttributes != NULL 
		&&	m_pMIMEAttributes->GetRawHeaderBuffer(nbufferSizeFillin) != NULL )
	{
		m_addMultipartAttributes = true;
		m_pObject->SetAttributesSupported();
		m_pObject->AddAttributeCollection(m_pMIMEAttributes);
		m_pMIMEAttributes = new CMIMEAttributeCollection();
	}
#endif //*** End AS400 Attribute API Removal *** 

}

bool CMIMEParser::EndOfHeaderFound() const
{
	return m_bEndOfHeaderFound;
}

void CMIMEParser::ResetEndOfHeaderFlag()
{
	m_bEndOfHeaderFound = false;
}

int CMIMEParser::EmitName(void)
{
	int		rc = PARSER_ERROR;	// Assume an error.
	DWORD	dwLen = 0;
	bool	bTruncate = false;

	if (m_pszName)
	{
		// We have a name that we need to write to the token file.
		dwLen = strlen(m_pszName) + 1;
		if (dwLen >= MAX_EMIT_BUFFER_SIZE - 1024)
			bTruncate = true;
	}

	if (bTruncate || (m_nEmitBufferOffset > (MAX_EMIT_BUFFER_SIZE - dwLen - 5)))
	{
		// Not enough room in the token buffer.  Flush it.
		if (dec_fwrite(m_pEmitBuffer, 1, m_nEmitBufferOffset, m_fpTokenFile) != m_nEmitBufferOffset)
		{
			// Failed to write to the intermediate output file.
			// We can't possibly parse the input data so return an error.
			return (PARSER_ERROR);
		}

		m_nEmitBufferOffset = 0;
	}

	// The m_pszName buffer is transient.  It should only exist while we
	// are building up a filename from a series of tokens (see Content-Type
	// and/or Content-Disposition processing).  It gets destroyed here after
	// writing it to the token file.
	if (m_pszName)
	{
		if (bTruncate)
			dwLen = MAX_EMIT_BUFFER_SIZE - 1024;  // Arbitrarily truncating the name here (at 64K!).

		// We have a name that we need to write to the token file.
		// First write the size, then the name string.
		memcpy(&m_pEmitBuffer[m_nEmitBufferOffset], &dwLen, sizeof(dwLen));
		m_nEmitBufferOffset += sizeof(dwLen);
		memcpy(&m_pEmitBuffer[m_nEmitBufferOffset], m_pszName, dwLen);
		if (bTruncate)
			m_pEmitBuffer[m_nEmitBufferOffset + dwLen] = 0x00;
		m_nEmitBufferOffset += dwLen;
		free(m_pszName);
		m_pszName = NULL;
		rc = PARSER_OK;
	}
	else
	{
		// Write a 0 length.
		memcpy(&m_pEmitBuffer[m_nEmitBufferOffset], &dwLen, sizeof(dwLen));
		m_nEmitBufferOffset += sizeof(dwLen);
		rc = PARSER_OK;

		/*
		if (dec_fwrite(&dwLen, 1, sizeof(dwLen), m_fpTokenFile) == sizeof(dwLen))
			rc = PARSER_OK;
		*/
	}

	return (rc);
}


int CMIMEParser::IdentifyAtom(int iGroup)
{
    int     iID = -1;
    int     iIndex = 0;
    bool    bResent = false;
    bool    bRFC2231 = false;
    size_t  nOffset = 0;
    char    *ptr = m_pString;
			m_bHasXDashPrefix = false;

	// Ptr contains the string that we need to identify
	// We will use switch statements to quickly reduce
	// the set of strings that need to be examined.

    switch (iGroup)
    {
// --------------------------------------------------------------------------------------------------------------------
       case MIME_ATOM_GROUP_HEADER_FIELDS:
 			bResent		= HasResentPrefix(ptr);
			m_bHasXDashPrefix	= HasXDashPrefix(ptr);

			switch ( ForceLowercase(ptr[0]) ) {	
			// -----------------------------------------------------------------------------------
			// Handle string: author, accept-language, apparently-to, auto-submitted
			// -----------------------------------------------------------------------------------
			case '\x61':	// 'a'
					 if (binary_stricmp(ptr,STR_author,STR_AUTHOR) == 0) { iID = MIME_TOKEN_AUTHOR; }
				else if (binary_stricmp(ptr,STR_accept_language,STR_ACCEPT_LANGUAGE) == 0) { iID = MIME_TOKEN_ACCEPT_LANGUAGE; }
				else if (binary_stricmp(ptr,STR_apparently_to,STR_APPARENTLY_TO) == 0) { iID = MIME_TOKEN_APPARENTLY_TO; }
				else if (binary_stricmp(ptr,STR_auto_submitted,STR_AUTO_SUBMITTED) == 0) { iID = MIME_TOKEN_AUTO_SUBMITTED; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string: bcc
			// -----------------------------------------------------------------------------------
			case '\x62':	// 'b'
				if (binary_stricmp(ptr,STR_bcc,STR_BCC) == 0) { iID = MIME_TOKEN_BCC; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	cc, comments, content-description, content-disposition, content-id
			//					content-length, content-location, content-transfter-encoding,
			//					content-type
			// -----------------------------------------------------------------------------------
			case '\x63':	// 'c'
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x63':	// 'c'	Handle cc
					if (binary_stricmp(ptr,STR_cc,STR_CC) == 0) { iID = MIME_TOKEN_CC; }
					break;
				case '\x6F':	// 'o'	Handle all other here
					switch ( ForceLowercase(ptr[2]) ) {
					case '\x6D':	// 'm'	Handle comments
						if (binary_stricmp(ptr,STR_comments,STR_COMMENTS) == 0) { iID = MIME_TOKEN_COMMENTS; }
						break;
					case '\x6E':	// 'n'	Handle content-'xxx'
							 if (binary_stricmp(ptr,STR_content_type,STR_CONTENT_TYPE) == 0) { iID = MIME_TOKEN_CONTENT_TYPE; }
						else if (binary_stricmp(ptr,STR_content_transfer_encoding,STR_CONTENT_TRANSFER_ENCODING) == 0) { iID = MIME_TOKEN_CONTENT_TRANSFER_ENCODING; }
						else if (binary_stricmp(ptr,STR_content_length,STR_CONTENT_LENGTH) == 0) { iID = MIME_TOKEN_CONTENT_LENGTH; }
						else if (binary_stricmp(ptr,STR_content_id,STR_CONTENT_ID) == 0) { iID = MIME_TOKEN_CONTENT_ID; }
						else if (binary_stricmp(ptr,STR_content_disposition,STR_CONTENT_DISPOSITION) == 0) { iID = MIME_TOKEN_CONTENT_DISPOSITION; }
						else if (binary_stricmp(ptr,STR_content_description,STR_CONTENT_DESCRIPTION) == 0) { iID = MIME_TOKEN_CONTENT_DESCRIPTION; }
						else if (binary_stricmp(ptr,STR_content_location,STR_CONTENT_LOCATION) == 0) { iID = MIME_TOKEN_CONTENT_LOCATION; }
						else if (binary_stricmp(ptr,STR_content_class,STR_CONTENT_CLASS) == 0) { iID = MIME_TOKEN_CONTENT_CLASS; }
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	data,date, delivered
			// -----------------------------------------------------------------------------------
			case '\x64':	// 'd'
					 if (binary_stricmp(ptr,STR_date,STR_DATE) == 0) { iID = MIME_TOKEN_DATE; }
				else if (binary_stricmp(ptr,STR_delivered_to,STR_DELIVERED_TO) == 0) { iID = MIME_TOKEN_DELIVERED_TO; }
				else if (binary_stricmp(ptr,STR_data,STR_DATA) == 0) { iID = MIME_TOKEN_DATA; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	encrypted, egroups-return, errors-to
			// -----------------------------------------------------------------------------------
			case '\x65':	// 'e'	Handle encrypted
					 if (binary_stricmp(ptr,STR_encrypted,STR_ENCRYPTED) == 0) { iID = MIME_TOKEN_ENCRYPTED; }
				else if (binary_stricmp(ptr,STR_egroups_return,STR_EGROUPS_RETURN) == 0) { iID = MIME_TOKEN_EGROUPS_RETURN; }
				else if (binary_stricmp(ptr,STR_errors_to,STR_ERRORS_TO) == 0) { iID = MIME_TOKEN_ERRORS_TO; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	from
			// -----------------------------------------------------------------------------------
			case '\x66':	// 'f'	Handle from
				if (binary_stricmp(ptr,STR_from,STR_FROM) == 0) { iID = MIME_TOKEN_FROM; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	helo
			// -----------------------------------------------------------------------------------
			case '\x68':	// 'h'
				if (binary_stricmp(ptr,STR_helo,STR_HELO) == 0) { iID = MIME_TOKEN_HELO; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	importance, in-reply-to
			// -----------------------------------------------------------------------------------
			case '\x69':	// 'i'	 
					 if (binary_stricmp(ptr,STR_in_reply_to,STR_IN_REPLY_TO) == 0) { iID = MIME_TOKEN_IN_REPLY_TO; }
				else if (binary_stricmp(ptr,STR_importance,STR_IMPORTANCE) == 0) { iID = MIME_TOKEN_IMPORTANCE; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	keywords
			// -----------------------------------------------------------------------------------
			case '\x6B':	// 'k'	 
				if (binary_stricmp(ptr,STR_keywords,STR_KEYWORDS) == 0) { iID = MIME_TOKEN_KEYWORDS; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	list-subscribe, list-unsubscribe, list-id, list-post, list-help
			//					list-archive
			// -----------------------------------------------------------------------------------
			case '\x6C':	// 'l'	 
					 if (binary_stricmp(ptr,STR_list_unsubscribe,STR_LIST_UNSUBSCRIBE) == 0) { iID = MIME_TOKEN_LIST_UNSUBSCRIBE; }
				else if (binary_stricmp(ptr,STR_list_subscribe,STR_LIST_SUBSCRIBE) == 0) { iID = MIME_TOKEN_LIST_SUBSCRIBE; }
				else if (binary_stricmp(ptr,STR_list_id,STR_LIST_ID) == 0) { iID = MIME_TOKEN_LIST_ID; }
				else if (binary_stricmp(ptr,STR_list_post,STR_LIST_POST) == 0) { iID = MIME_TOKEN_LIST_POST; }
				else if (binary_stricmp(ptr,STR_list_help,STR_LIST_HELP) == 0) { iID = MIME_TOKEN_LIST_HELP; }
				else if (binary_stricmp(ptr,STR_list_archive,STR_LIST_ARCHIVE) == 0) { iID = MIME_TOKEN_LIST_ARCHIVE; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	mailer, message-id, mime-version, msmail-priority,
			//					mailer-version, mailing-list,ms-has-attach,ms-tnef-correlator
			//					mime-autoconverted,mimeole
			// -----------------------------------------------------------------------------------
			case '\x6D':	// 'm'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x61':	// 'a'	Handle mailer
						 if (binary_stricmp(ptr,STR_mailer,STR_MAILER) == 0) { iID = MIME_TOKEN_MAILER; }
					else if (binary_stricmp(ptr,STR_mailer_version,STR_MAILER_VERSION) == 0) { iID = MIME_TOKEN_MAILER_VERSION; }
					else if (binary_stricmp(ptr,STR_mailing_list,STR_MAILING_LIST) == 0) { iID = MIME_TOKEN_MAILING_LIST; }
					break;
				case '\x65':	// 'e'	Handle message-id
					if (binary_stricmp(ptr,STR_message_id,STR_MESSAGE_ID) == 0) { iID = MIME_TOKEN_MESSAGE_ID; }
					break;
				case '\x69':	// 'i'	Handle mime-version
					if (binary_stricmp(ptr,STR_mime_version,STR_MIME_VERSION) == 0) { iID = MIME_TOKEN_MIME_VERSION; }
					else if (binary_stricmp(ptr,STR_mime_autoconverted,STR_MIME_AUTOCONVERTED) == 0) { iID = MIME_TOKEN_MIME_AUTOCONVERTED; }
					else if (binary_stricmp(ptr,STR_mimeole,STR_MIMEOLE) == 0) { iID = MIME_TOKEN_MIMEOLE; }
					break;
				case '\x73':	// 's'  Handle msmail-priority
						 if (binary_stricmp(ptr,STR_msmail_priority,STR_MSMAIL_PRIORITY) == 0) { iID = MIME_TOKEN_MSMAIL_PRIORITY; }
					else if (binary_stricmp(ptr,STR_ms_has_attach,STR_MS_HAS_ATTACH) == 0) { iID = MIME_TOKEN_MS_HAS_ATTACH; }
					else if (binary_stricmp(ptr,STR_ms_tnef_correlator,STR_MS_TNEF_CORRELATOR) == 0) { iID = MIME_TOKEN_MS_TNEF_CORRELATOR; }
					break;
				default:
					break;
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	organization, originating-ip, originalarrivaltime
			// -----------------------------------------------------------------------------------
			case '\x6F':	// 'o'	 
					 if (binary_stricmp(ptr,STR_organization,STR_ORGANIZATION) == 0) { iID = MIME_TOKEN_ORGANIZATION; }
				else if (binary_stricmp(ptr,STR_originating_ip,STR_ORIGINATING_IP) == 0) { iID = MIME_TOKEN_ORIGINATING_IP; }
				else if (binary_stricmp(ptr,STR_originalarrivaltime,STR_ORIGINALARRIVALTIME) == 0) { iID = MIME_TOKEN_ORIGINALARRIVALTIME; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	priority, precedence
			// -----------------------------------------------------------------------------------
			case '\x70':	// 'p'	 
					 if (binary_stricmp(ptr,STR_priority,STR_PRIORITY) == 0) { iID = MIME_TOKEN_PRIORITY; }
				else if (binary_stricmp(ptr,STR_precedence,STR_PRECEDENCE) == 0) { iID = MIME_TOKEN_PRECEDENCE; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	received, reference, reply-to, return-path
			// -----------------------------------------------------------------------------------
			case '\x72':	// 'r'	 
				if ( ForceLowercase(ptr[1]) == '\x65' ) {	// 'e'
					switch ( ForceLowercase(ptr[2]) ) {
					case '\x63':	// 'c'	Handle received
						if (binary_stricmp(ptr,STR_received,STR_RECEIVED) == 0) { iID = MIME_TOKEN_RECEIVED; }
						break;
					case '\x66':	// 'f'	Handle reference
						if (binary_stricmp(ptr,STR_references,STR_REFERENCES) == 0) { iID = MIME_TOKEN_REFERENCES; }
						break;
					case '\x70':	// 'p'	Handle reply-to
						if (binary_stricmp(ptr,STR_reply_to,STR_REPLY_TO) == 0) { iID = MIME_TOKEN_REPLY_TO; }
						break;
					case '\x74':	// 't'	Handle return-path
						if (binary_stricmp(ptr,STR_return_path,STR_RETURN_PATH) == 0) { iID = MIME_TOKEN_RETURN_PATH; }
						break;
					default:
						break;
					}
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	subject, sender
			// -----------------------------------------------------------------------------------
			case '\x73':	// 's'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x65':	// 'e'	Handle sender
					if (binary_stricmp(ptr,STR_sender,STR_SENDER) == 0) { iID = MIME_TOKEN_SENDER; }
					break;
				case '\x75':	// 'u'	Handle subject
					if (binary_stricmp(ptr,STR_subject,STR_SUBJECT) == 0) { iID = MIME_TOKEN_SUBJECT; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	to,thread-index, thread-topic
			// -----------------------------------------------------------------------------------
			case '\x74':	// 't'  
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x68':	// 'h'	Handle thread-topic, thread-index
						 if (binary_stricmp(ptr,STR_thread_topic,STR_THREAD_TOPIC) == 0) { iID = MIME_TOKEN_THREAD_TOPIC; }
					else if (binary_stricmp(ptr,STR_thread_index,STR_THREAD_INDEX) == 0) { iID = MIME_TOKEN_THREAD_INDEX; }
					break;
				case '\x6F':	// 'o'	Handle to
					if (binary_stricmp(ptr,STR_to,STR_TO) == 0) { iID = MIME_TOKEN_TO; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	user-agent
			// -----------------------------------------------------------------------------------
			case '\x75':	// 'u'  
				if (binary_stricmp(ptr,STR_user_agent,STR_USER_AGENT) == 0) { iID = MIME_TOKEN_USER_AGENT; }
				break;
			default:
				break;
			}

            break;
 
// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
      case MIME_ATOM_GROUP_RECEIVED:
 			bResent		= HasResentPrefix(ptr);
			m_bHasXDashPrefix	= HasXDashPrefix(ptr);
			switch ( ForceLowercase(ptr[0]) ) {
			// -----------------------------------------------------------------------------------
			// Handle string:	by
			// -----------------------------------------------------------------------------------
			case '\x62':	// 'b'	 
				if (binary_stricmp(ptr,STR_by,STR_BY) == 0) { iID = MIME_TOKEN_BY; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	for, from
			// -----------------------------------------------------------------------------------
			case '\x66':	// 'f'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x6F':	// 'o'	Handle For
					if (binary_stricmp(ptr,STR_for,STR_FOR) == 0) { iID = MIME_TOKEN_FOR; }
					break;
				case '\x72':	// 'r'	Handle From
					if (binary_stricmp(ptr,STR_from,STR_FROM) == 0) { iID = MIME_TOKEN_FROM; }
					break;
				default:
					break;
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	id
			// -----------------------------------------------------------------------------------
			case '\x69':	// 'i'	 
				if (binary_stricmp(ptr,STR_7bit,STR_7BIT) == 0) { iID = MIME_TOKEN_ID; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	via
			// -----------------------------------------------------------------------------------
			case '\x76':	// 'v'	 
				if (binary_stricmp(ptr,STR_id,STR_ID) == 0) { iID = MIME_TOKEN_VIA; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	with
			// -----------------------------------------------------------------------------------
			case '\x77':	// 'w'	 
				if (binary_stricmp(ptr,STR_with,STR_WITH) == 0) { iID = MIME_TOKEN_WITH; }
				break;
			default:
				break;
			}
            break;

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
		case MIME_ATOM_GROUP_ENCODING:
 			bResent		= HasResentPrefix(ptr);
			m_bHasXDashPrefix	= HasXDashPrefix(ptr);

			switch ( ForceLowercase(ptr[0]) ) {
			// -----------------------------------------------------------------------------------
			// Handle strings:	7Bit
			// -----------------------------------------------------------------------------------
			case '\x37':	// '7'	 
				if (binary_stricmp(ptr,STR_7bit,STR_7BIT) == 0) { iID = MIME_TOKEN_7BIT; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	8Bit
			// -----------------------------------------------------------------------------------
			case '\x38':	// '8'	 
				if (binary_stricmp(ptr,STR_8bit,STR_8BIT) == 0) { iID = MIME_TOKEN_8BIT; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	base64, binary
			// -----------------------------------------------------------------------------------
			case '\x62':	// 'b'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x61':	// 'a'	Handle base64
					 if (binary_stricmp(ptr,STR_base64,STR_BASE64) == 0) { iID = MIME_TOKEN_BASE64; }
					break;
				case '\x69':	// 'i'	Handle binary
					 if (binary_stricmp(ptr,STR_binary,STR_BINARY) == 0) { iID = MIME_TOKEN_BINARY; }
					break;
				default:
					break;
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	quoted-printable
			// -----------------------------------------------------------------------------------
			case '\x71':	// 'q'	 
				if (binary_stricmp(ptr,STR_quoted_printable,STR_QUOTED_PRINTABLE) == 0) { iID = MIME_TOKEN_QUOTED_PRINTABLE; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	uuencode , uue
			// -----------------------------------------------------------------------------------
			case '\x75':	// 'u'	 
					 if (binary_stricmp(ptr,STR_uue,STR_UUE) == 0) { iID = MIME_TOKEN_UUENCODE; }
				else if (binary_stricmp(ptr,STR_uuencode,STR_UUENCODE) == 0) { iID = MIME_TOKEN_UUENCODE; }
				break;
			default:
				break;
			}
            break;

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
       case MIME_ATOM_GROUP_DISPOSITION:
 			bResent		= HasResentPrefix(ptr);
			m_bHasXDashPrefix	= HasXDashPrefix(ptr);
			bRFC2231	= HasRFC2231Suffix(nOffset);

			switch ( ForceLowercase(ptr[0]) ) {
			// -----------------------------------------------------------------------------------
			// Handle string:	attachment
			// -----------------------------------------------------------------------------------
			case '\x61':	// 'a'	 
				if (binary_stricmp(ptr,STR_attachment,STR_ATTACHMENT) == 0) { iID = MIME_TOKEN_ATTACHMENT; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	encryption , encryption-flags
			// -----------------------------------------------------------------------------------
			case '\x65':	// 'e' 
					 if (binary_stricmp(ptr,STR_encryption,STR_ENCRYPTION) == 0) { iID = MIME_TOKEN_ENCRYPTION; }
				else if (binary_stricmp(ptr,STR_encryption_flags,STR_ENCRYPTION_FLAGS) == 0) { iID = MIME_TOKEN_ENCRYPTION_FLAGS; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	filename
			// -----------------------------------------------------------------------------------
			case '\x66':	// 'f'	 
				if (binary_stricmp(ptr,STR_filename,STR_FILENAME) == 0) { iID = MIME_TOKEN_FILENAME; }
				break;
			default:
				break;
			}
            break;

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
       case MIME_ATOM_GROUP_TYPE:
 			bResent		= HasResentPrefix(ptr);
			m_bHasXDashPrefix	= HasXDashPrefix(ptr);
			bRFC2231	= HasRFC2231Suffix(nOffset);

			switch ( ForceLowercase(ptr[0]) ) {

			// -----------------------------------------------------------------------------------
			// Handle strings:	alternative,appledouble,application,atomicmail, andrew-inset,
			//					activemessage,applefile, audio, & access-type
			// -----------------------------------------------------------------------------------
			case '\x61':	// 'a'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x63':	// 'c'	Handle activemessage, access-type
					switch( ForceLowercase(ptr[2]) ) {
					case '\x63':	// 'c'	Handle access-type
						if (binary_stricmp(ptr,STR_access_type,STR_ACCESS_TYPE) == 0) { iID = MIME_TOKEN_ACCESS_TYPE; }
						break;
					case '\x74':	// 't'	Handle activemessage
						if (binary_stricmp(ptr,STR_activemessage,STR_ACTIVEMESSAGE) == 0) { iID = MIME_TOKEN_ACTIVEMESSAGE; }
						break;
					default:
						break;
					}
					break;
				case '\x6C':	// 'l'	Handle alternative
					if (binary_stricmp(ptr,STR_alternative,STR_ALTERNATIVE) == 0) { iID = MIME_TOKEN_ALTERNATIVE; }
					break;
				case '\x6E':	// 'n'	Handle andrew-inset
					if (binary_stricmp(ptr,STR_andrew_inset,STR_ANDREW_INSET) == 0) { iID = MIME_TOKEN_ANDREW_INSET; }
					break;
				case '\x70':	// 'p'	Handle appledouble, application, applefile
						 if (binary_stricmp(ptr,STR_application,	STR_APPLICATION) == 0)	{ iID = MIME_TOKEN_APPLICATION; }
					else if (binary_stricmp(ptr,STR_applefile,		STR_APPLEFILE) == 0)	{ iID = MIME_TOKEN_APPLEFILE; }
					else if (binary_stricmp(ptr,STR_appledouble,	STR_APPLEDOUBLE) == 0)	{ iID = MIME_TOKEN_APPLEDOUBLE; }
					break;
				case '\x74':	// 't'	Handle atomicmail
					if (binary_stricmp(ptr,STR_atomicmail,STR_ATOMICMAIL) == 0) { iID = MIME_TOKEN_ATOMICMAIL; }
					break;
				case '\x75':	// 'u'	Handle audio
					if (binary_stricmp(ptr,STR_audio,STR_AUDIO) == 0) { iID = MIME_TOKEN_AUDIO; }
					break;
				default:
					break;
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	basic & boundary
			// -----------------------------------------------------------------------------------
			case '\x62':	// 'b'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x61':	// 'a'	Handle basic
					if (binary_stricmp(ptr,STR_basic,STR_BASIC) == 0) { iID = MIME_TOKEN_BASIC; }
					break;
				case '\x6F':	// 'o'	Handle boundary
					if (binary_stricmp(ptr,STR_boundary,STR_BOUNDARY) == 0) { iID = MIME_TOKEN_BOUNDARY; }
					break;
				default:
					break;
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	charset
			// -----------------------------------------------------------------------------------
			case '\x63':	// 'c'	 
				if (binary_stricmp(ptr,STR_charset,STR_CHARSET) == 0) { iID = MIME_TOKEN_CHARSET; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	digest, dec-dx, & dca-rtf
			// -----------------------------------------------------------------------------------
			case '\x64':	// 'd'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x63':	// 'c'	Handle dca-rtf
					if (binary_stricmp(ptr,STR_dca_rft,STR_DCA_RFT) == 0) { iID = MIME_TOKEN_DCA_RFT; }
					break;
				case '\x65':	// 'e'	Handle dec-dx
					if (binary_stricmp(ptr,STR_dec_dx,STR_DEC_DX) == 0) { iID = MIME_TOKEN_DEC_DX; }
					break;
				case '\x69':	// 'i'	Handle digest
					if (binary_stricmp(ptr,STR_digest,STR_DIGEST) == 0) { iID = MIME_TOKEN_DIGEST; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	external-body & enriched
			// -----------------------------------------------------------------------------------
			case '\x65':	// 'e'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x6E':	// 'n'	Handle enriched
					if (binary_stricmp(ptr,STR_enriched,STR_ENRICHED) == 0) { iID = MIME_TOKEN_ENRICHED; }
					break;
				case '\x78':	// 'x'	Handle external-body
					if (binary_stricmp(ptr,STR_external_body,STR_EXTERNAL_BODY) == 0) { iID = MIME_TOKEN_EXTERNAL_BODY; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	gif
			// -----------------------------------------------------------------------------------
			case '\x67':	// 'g'	 
				if (binary_stricmp(ptr,STR_gif,STR_GIF) == 0) { iID = MIME_TOKEN_GIF; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	html & header-set
			// -----------------------------------------------------------------------------------
			case '\x68':	// 'h'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x65':	// 'e'	Handle header-set
					if (binary_stricmp(ptr,STR_header_set,STR_HEADER_SET) == 0) { iID = MIME_TOKEN_HEADER_SET; }
					break;
				case '\x74':	// 't'	Handle html
					if (binary_stricmp(ptr,STR_html,STR_HTML) == 0) { iID = MIME_TOKEN_HTML; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	ief & image
			// -----------------------------------------------------------------------------------
			case '\x69':	// 'i' 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x65':	// 'e'	Handle ief
					if (binary_stricmp(ptr,STR_ief,STR_IEF) == 0) { iID = MIME_TOKEN_IEF; }
					break;
				case '\x6D':	// 'm'	Handle image
					if (binary_stricmp(ptr,STR_image,STR_IMAGE) == 0) { iID = MIME_TOKEN_IMAGE; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	jpeg
			// -----------------------------------------------------------------------------------
			case '\x6A':	// 'j'	 
				if (binary_stricmp(ptr,STR_jpeg,STR_JPEG) == 0) { iID = MIME_TOKEN_JPEG; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	multipart, mixed, message,mac-binhex40, macwriteii, msword & mpeg
			// -----------------------------------------------------------------------------------
			case '\x6D':	// 'm'	Handle 	
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x61':	// 'a'	Handle mac-binhex40, macwriteii
						 if (binary_stricmp(ptr,STR_mac_binhex40,STR_MAC_BINHEX40) == 0) { iID = MIME_TOKEN_MAC_BINHEX40; }
					else if (binary_stricmp(ptr,STR_macwriteii,STR_MACWRITEII) == 0) { iID = MIME_TOKEN_MACWRITEII; }
					break;
				case '\x65':	// 'e'	Handle message
					if (binary_stricmp(ptr,STR_message,STR_MESSAGE) == 0) { iID = MIME_TOKEN_MESSAGE; }
					break;
				case '\x69':	// 'i'	Handle mixed
					if (binary_stricmp(ptr,STR_mixed,STR_MIXED) == 0) { iID = MIME_TOKEN_MIXED; }
					break;
				case '\x70':	// 'p'	Handle mpeg
					if (binary_stricmp(ptr,STR_mpeg,STR_MPEG) == 0) { iID = MIME_TOKEN_MPEG; }
					break;
				case '\x73':	// 's'	Handle msword
					if (binary_stricmp(ptr,STR_msword,STR_MSWORD) == 0) { iID = MIME_TOKEN_MSWORD; }
					break;
				case '\x75':	// 'u'	Handle multipart
					if (binary_stricmp(ptr,STR_multipart,STR_MULTIPART) == 0) { iID = MIME_TOKEN_MULTIPART; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	news, news-message-id, news-transmission, & name
			// -----------------------------------------------------------------------------------
			case '\x6E':	// 'n'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x61':	// 'a'	Handle name
					if (binary_stricmp(ptr,STR_name,STR_NAME) == 0) { iID = MIME_TOKEN_NAME; }
					break;
				case '\x65':	// 'e'	Handle news, news-message-id, news-transmission & name
						 if (binary_stricmp(ptr,STR_news,STR_NEWS) == 0) { iID = MIME_TOKEN_NEWS; }
					else if (binary_stricmp(ptr,STR_news_message_id,STR_NEWS_MESSAGE_ID) == 0) { iID = MIME_TOKEN_NEWS_MESSAGE_ID; }
					else if (binary_stricmp(ptr,STR_news_transmission,STR_NEWS_TRANSMISSION) == 0) { iID = MIME_TOKEN_NEWS_TRANSMISSION; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	oda, octet_stream
			// -----------------------------------------------------------------------------------
			case '\x6F':	// 'o'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x63':	// 'c'	Handle octet-stream
					if (binary_stricmp(ptr,STR_octet_stream,STR_OCTET_STREAM) == 0) { iID = MIME_TOKEN_OCTET_STREAM; }
					break;
				case '\x64':	// 'd'	Handle oda
					if (binary_stricmp(ptr,STR_oda,STR_ODA) == 0) { iID = MIME_TOKEN_ODA; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	pdf,plain, parallel, postscript, partial
			// -----------------------------------------------------------------------------------
			case '\x70':	// 'p' 
							//		
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x61':	// 'a'	Handle partial, parallel
						 if (binary_stricmp(ptr,STR_partial,STR_PARTIAL) == 0) { iID = MIME_TOKEN_PARTIAL; }
					else if (binary_stricmp(ptr,STR_parallel,STR_PARALLEL) == 0) { iID = MIME_TOKEN_PARALLEL; }
					break;
				case '\x64':	// 'd'	Handle pdf
					if (binary_stricmp(ptr,STR_pdf,STR_PDF) == 0) { iID = MIME_TOKEN_PDF; }
					break;
				case '\x6C':	// 'l'	Handle plain
					if (binary_stricmp(ptr,STR_plain,STR_PLAIN) == 0) { iID = MIME_TOKEN_PLAIN; }
					break;
				case '\x6F':	// 'o'	Handle postscript
					if (binary_stricmp(ptr,STR_postscript,STR_POSTSCRIPT) == 0) { iID = MIME_TOKEN_POSTSCRIPT; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	quicktime
			// -----------------------------------------------------------------------------------
			case '\x71':	// 'q'	 
				if (binary_stricmp(ptr,STR_quicktime,STR_QUICKTIME) == 0) { iID = MIME_TOKEN_QUICKTIME; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	richtext, rfc822, rtfremote-printing, report-type
			// -----------------------------------------------------------------------------------
			case '\x72':	// 'r' 
							//		
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x65':	// 'e'	Handle remote-printing, report-type
						 if (binary_stricmp(ptr,STR_remote_printing,STR_REMOTE_PRINTING) == 0) { iID = MIME_TOKEN_REMOTE_PRINTING; }
					else if (binary_stricmp(ptr,STR_report_type,STR_REPORT_TYPE) == 0) { iID = MIME_TOKEN_REPORT_TYPE; }
					break;
				case '\x66':	// 'f'	Handle rfc822
					if (binary_stricmp(ptr,STR_rfc822,STR_RFC822) == 0) { iID = MIME_TOKEN_RFC822; }
					break;
				case '\x69':	// 'i'	Handle richtext
					if (binary_stricmp(ptr,STR_richtext,STR_RICHTEXT) == 0) { iID = MIME_TOKEN_RICHTEXT; }
					break;
				case '\x74':	// 't'	Handle rtf
					if (binary_stricmp(ptr,STR_rtf,STR_RTF) == 0) { iID = MIME_TOKEN_RTF; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	slate
			// -----------------------------------------------------------------------------------
			case '\x73':	// 's'	 
				if (binary_stricmp(ptr,STR_slate,STR_SLATE) == 0) { iID = MIME_TOKEN_SLATE; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	text,tab-separated-values,tiff
			// -----------------------------------------------------------------------------------
			case '\x74':	// 't'	 
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x61':	// 'a'	Handle tab-separated-values
					if (binary_stricmp(ptr,STR_tab_separated_values,STR_TAB_SEPARATED_VALUES) == 0) { iID = MIME_TOKEN_TAB_SEPARATED_VALUES; }
					break;
				case '\x65':	// 'e'	Handle text
					if (binary_stricmp(ptr,STR_text,STR_TEXT) == 0) { iID = MIME_TOKEN_TEXT; }
					break;
				case '\x69':	// 'i'	Handle tiff
					if (binary_stricmp(ptr,STR_tiff,STR_TIFF) == 0) { iID = MIME_TOKEN_TIFF; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	url
			// -----------------------------------------------------------------------------------
			case '\x75':	// 'u'	 
				if (binary_stricmp(ptr,STR_url,STR_URL) == 0) { iID = MIME_TOKEN_URL; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	video
			// -----------------------------------------------------------------------------------
			case '\x76':	// 'v'	
				if (binary_stricmp(ptr,STR_video,STR_VIDEO) == 0) { iID = MIME_TOKEN_VIDEO; }
				break;
			// -----------------------------------------------------------------------------------
			// Handle strings:	wita & wordperfect5.1
			// -----------------------------------------------------------------------------------
			case '\x77':	// 'w'	
				switch ( ForceLowercase(ptr[1]) ) {
				case '\x69':	// 'i'	Handle wita
					if (binary_stricmp(ptr,STR_wita,STR_WITA) == 0) { iID = MIME_TOKEN_WITA; }
					break;
				case '\x6F':	// 'o'	Handle wordperfect5.1
					if (binary_stricmp(ptr,STR_wordperfect5_1,STR_WORDPERFECT5_1) == 0) { iID = MIME_TOKEN_WORDPERFECT51; }
					break;
				default:
					break;
				}				
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	zip
			// -----------------------------------------------------------------------------------
			case '\x7A':	// 'z'	 
				if (binary_stricmp(ptr,STR_zip,STR_ZIP) == 0) { iID = MIME_TOKEN_ZIP; }
				break;
			default:
				break;
			}

            
			break;

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
       case MIME_ATOM_GROUP_DAYOFWEEK:
 			bResent		= HasResentPrefix(ptr);
			m_bHasXDashPrefix	= HasXDashPrefix(ptr);

			switch ( ForceLowercase(ptr[0]) ) {

			// -----------------------------------------------------------------------------------
			// Handle strings:	fri
			// -----------------------------------------------------------------------------------
			case '\x66':	// 'f'	 
				if (binary_stricmp(ptr,STR_fri,STR_FRI) == 0) { iID = MIME_TOKEN_FRIDAY; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	mon
			// -----------------------------------------------------------------------------------
			case '\x6D':	// 'm'	 
				if (binary_stricmp(ptr,STR_mon,STR_MON) == 0) { iID = MIME_TOKEN_MONDAY; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	tue & thu
			// -----------------------------------------------------------------------------------
			case '\x74':	// 't'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x68':	// 'h'
					if (binary_stricmp(ptr,STR_thu,STR_THU) == 0) {	iID = MIME_TOKEN_THURSDAY;  }
					break;

				case '\x75':	// 'u'
					if (binary_stricmp(ptr,STR_tue,STR_TUE) == 0) { iID = MIME_TOKEN_TUESDAY; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	sat & sun
			// -----------------------------------------------------------------------------------
			case '\x73':	// 's'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x61':	// 'a'
					if (binary_stricmp(ptr,STR_sat,STR_SAT) == 0) {	iID = MIME_TOKEN_SATURDAY;  }
					break;

				case '\x75':	// 'u'
					if (binary_stricmp(ptr,STR_sun,STR_SUN) == 0) { iID = MIME_TOKEN_SUNDAY; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle string:	wed
			// -----------------------------------------------------------------------------------
			case '\x77':	// 'w'	 
				if (binary_stricmp(ptr,STR_wed,STR_WED) == 0) { iID = MIME_TOKEN_WEDNESDAY; }
				break;

			default:
				break;
			}            
			break;

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
       case MIME_ATOM_GROUP_MONTH:
 			bResent		= HasResentPrefix(ptr);
			m_bHasXDashPrefix	= HasXDashPrefix(ptr);

			switch ( ForceLowercase(ptr[0]) ) {
			// -----------------------------------------------------------------------------------
			// Handle strings:	apr & aug
			// -----------------------------------------------------------------------------------
			case '\x61':	// 'a'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x70':	// 'p'
					if (binary_stricmp(ptr,STR_apr,STR_APR) == 0) {	iID = MIME_TOKEN_APRIL;  }
					break;

				case '\x75':	// 'u'
					if (binary_stricmp(ptr,STR_aug,STR_AUG) == 0) { iID = MIME_TOKEN_AUGUST; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	dec
			// -----------------------------------------------------------------------------------
			case '\x64':	// 'd'	 
				if (binary_stricmp(ptr,STR_dec,STR_DEC) == 0) { iID = MIME_TOKEN_DECEMBER; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	feb
			// -----------------------------------------------------------------------------------
			case '\x66':	// 'f'	 
				if (binary_stricmp(ptr,STR_feb,STR_FEB) == 0) { iID = MIME_TOKEN_FEBRUARY; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	jan, jun, & jul
			// -----------------------------------------------------------------------------------
			case '\x6A':	// 'j'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x61':	// 'a'
					if (binary_stricmp(ptr,STR_jan,STR_JAN) == 0) {	iID = MIME_TOKEN_JANUARY;  }
					break;

				case '\x75':	// 'u'
					if (binary_stricmp(ptr,STR_jun,STR_JUN) == 0) { iID = MIME_TOKEN_JUNE; }
					else
					if (binary_stricmp(ptr,STR_jul,STR_JUL) == 0) { iID = MIME_TOKEN_JULY; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	mar & may
			// -----------------------------------------------------------------------------------
			case '\x6D':	// 'm'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x61':	// 'a'
					if (binary_stricmp(ptr,STR_mar,STR_MAR) == 0) { iID = MIME_TOKEN_MARCH; }
					else
					if (binary_stricmp(ptr,STR_may,STR_MAY) == 0) { iID = MIME_TOKEN_MAY; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle string:	nov
			// -----------------------------------------------------------------------------------
			case '\x6E':	// 'n'	 
				if (binary_stricmp(ptr,STR_nov,STR_NOV) == 0) { iID = MIME_TOKEN_NOVEMBER; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle string:	oct
			// -----------------------------------------------------------------------------------
			case '\x6F':	// 'o'	 
				if (binary_stricmp(ptr,STR_oct,STR_OCT) == 0) { iID = MIME_TOKEN_OCTOBER; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle string:	sep
			// -----------------------------------------------------------------------------------
			case '\x73':	// 's'	 
				if (binary_stricmp(ptr,STR_sep,STR_SEP) == 0) { iID = MIME_TOKEN_SEPTEMBER; }
				break;

			default:
				break;
			}
            break;

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
		case MIME_ATOM_GROUP_TIMEZONE:
 			bResent			= HasResentPrefix(ptr);
			m_bHasXDashPrefix		= HasXDashPrefix(ptr);

			switch ( ForceLowercase(ptr[0]) ) {

			// -----------------------------------------------------------------------------------
			// Handle strings:	est & edt
			// -----------------------------------------------------------------------------------
			case '\x65':	// 'e'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x73':	// 's'
					if (binary_stricmp(ptr,STR_edt,STR_EDT) == 0) {	iID = MIME_TOKEN_EST;  }
					break;

				case '\x64':	// 'd'
					if (binary_stricmp(ptr,STR_edt,STR_EDT) == 0) { iID = MIME_TOKEN_EDT; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	cst & cdt
			// -----------------------------------------------------------------------------------
			case '\x63':	// 'c'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x73':	// 's'
					if (binary_stricmp(ptr,STR_cst,STR_CST) == 0) { iID = MIME_TOKEN_CST; }
					break;

				case '\x64':	// 'd'
					if (binary_stricmp(ptr,STR_cdt,STR_CDT) == 0) { iID = MIME_TOKEN_CDT; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle string:	gmt
			// -----------------------------------------------------------------------------------
			case '\x67':	// 'g'	 
				if (binary_stricmp(ptr,STR_gmt,STR_GMT) == 0) {	iID = MIME_TOKEN_GMT; }
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	mst & mdt
			// -----------------------------------------------------------------------------------
			case '\x6D':	// 'm'	 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x73':	// 's'
					if (binary_stricmp(ptr,STR_mst,STR_MST) == 0) { iID = MIME_TOKEN_MST; }
					break;

				case '\x64':	// 'd'
					if (binary_stricmp(ptr,STR_mdt,STR_MDT) == 0) { iID = MIME_TOKEN_MDT; }
					break;

				default: 
					break;
				}
				break;

			// -----------------------------------------------------------------------------------
			// Handle strings:	pst & pdt
			// -----------------------------------------------------------------------------------
			case '\x70':	// 'p' 
				switch ( ForceLowercase(ptr[1]) ) {

				case '\x73':	// 's'
					if (binary_stricmp(ptr,STR_pst,STR_PST) == 0) { iID = MIME_TOKEN_PST; }
					break;

				case '\x64':	// 'd'
					if (binary_stricmp(ptr,STR_pdt,STR_PDT) == 0) { iID = MIME_TOKEN_PDT; }
					break;

				default: 
					break;
				}
				break;
			// -----------------------------------------------------------------------------------
			// Handle string:	ut
			// -----------------------------------------------------------------------------------
			case '\x75':	// 'u' Handle ut
				if (binary_stricmp(ptr,STR_ut,STR_UT) == 0) { iID = MIME_TOKEN_UT; }
				break;
			default:
				break;
			}
			break;
		case MIME_ATOM_GROUP_SIGNED_DATA:
		{
			switch ( ForceLowercase(ptr[0]) )
			{
			
			case '\x73':	// 's'	Handle signed-data
				if( binary_stricmp(ptr, STR_CONTENT_TYPE_signed_data, STR_CONTENT_TYPE_SIGNED_DATA ) == 0 ) { iID = MIME_TOKEN_SIGNED_DATA; }
				break;
			case '\x78':	// 'x'	Handle x-pkcs7-mime
				if (binary_stricmp(ptr,STR_X_PKCS7_MIME, STR_x_pkcs7_mime) == 0) { iID = MIME_TOKEN_X_PKSC7_MIME; }
				break;
			default:
				break;
			
			}
		}
        default:
            break;
    }

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
	if ( iID != -1 ) 
	{
        if (bResent)
            EMIT_FLAGS |= RESENT;
        if (m_bHasXDashPrefix)
            EMIT_FLAGS |= EXTENSION;
        if (bRFC2231)
        {
            EMIT_FLAGS |= RFC2231;

            // If we NULL-terminated the RFC 2231 atom where the continuation
            // character was, put it back.
            m_pString[nOffset] = '*';
        }

        return (iID);

	} 
	else
	{
		// If we NULL-terminated the RFC 2231 atom where the continuation
		// character was, put it back.
		if (bRFC2231)
			m_pString[nOffset] = '*';

		return (MIME_TOKEN_LAST);
	}
}


inline bool CMIMEParser::IsRFC2231(char *pszString, size_t *pnOffset)
{
	bool	brc = false;
	char	ch;
	size_t	nOffset = 0;

	ch = *pszString;
	while (ch)
	{
		if (ch == '*')
		{
			*pnOffset = nOffset;
			brc = true;
			break;
		}

		nOffset++;
		pszString++;
		ch = *pszString;
	}

	return (brc);
}

#include "BCC.cpp"
#include "Body.cpp"
#include "CC.cpp"
#include "ContentDescription.cpp"
#include "ContentDisposition.cpp"
#include "ContentTransferEncoding.cpp"
#include "ContentType.cpp"
#include "Date.cpp"
#include "From.cpp"
#include "HeaderField.cpp"
#include "Multipart.cpp"
#include "MultipartBody.cpp"
#include "MultipartHeader.cpp"
#include "Other.cpp"
#include "Received.cpp"
#include "ReplyTo.cpp"
#include "ReturnPath.cpp"
#include "RouteAddr.cpp"
#include "Sender.cpp"
#include "To.cpp"


int CMIMEParser::FindFirstEntry(TEXT_FIND_DATA *pData)
{
	// Initialize the extract state.
	m_iExtractState = MIME_EXTRACT_STATE_HEADER_FIRST;

	// Initialize the offset of the current section that we are enumerating in.
	m_nBoundaryStart = 0;

	// Make sure the intermediate file is available (i.e. open).
	if (!m_fpTokenFile)
	{
		if (!m_pszTokenFile)
			return (LEXER_ERROR);

		m_fpTokenFile = dec_fopen(m_pszTokenFile, "rb");
		if (!m_fpTokenFile)
			return (LEXER_ERROR);
	}

	// Re-using this variable to track when we are enumerating the main message body section.
	m_ParseState = DEC_ITEMTYPE_MIME_BODY;

	// Rewind back to the start of the token data.
	if (dec_fseek(m_fpTokenFile, 0, SEEK_SET) != 0)
		return (LEXER_ERROR);

	// Start looking through the tokenized information.
	return (FindNextEntry(pData));
}


int CMIMEParser::FindNextEntry(TEXT_FIND_DATA *pData)
{
	int			rc = LEXER_OK;
	bool		bReadFirst = false;
	bool		bExtract = false;
	bool		bPrimaryRFC2231 = false;
	bool		bSecondaryRFC2231 = false;
	size_t		nRead;
	char		*pszPrimaryName = NULL;
	char		*pszSecondaryName = NULL;
	DWORD		dwCharset;
	FINDNAMEDATA PrimaryNameData;
	FINDNAMEDATA SecondaryNameData;
	int			nLastTokenID = -1;
	m_nContentTypeID = -1;
	m_bAttachment = false;

	// Reset the multi-part boundary start offset here.
	m_nBoundaryStart = 0;

	// Reset the primary and secondary name strings.
	pData->pszName = NULL;
	pData->pszSecondaryName = NULL;

	// Clear the primary and secondary name construction structures.
	memset(&PrimaryNameData, 0, sizeof(PrimaryNameData));
	memset(&SecondaryNameData, 0, sizeof(SecondaryNameData));

	// Make sure the intermediate file is available (i.e. open).
	if (!m_fpTokenFile)
		return (LEXER_ERROR);

	// Keep reading tokens until we locate something of interest.
findnext_top:
	while (m_iExtractState < MIME_EXTRACT_STATE_DONE)
	{
		rc = LEXER_OK;	// Assume success

		// *** DEBUG ***
		// Disabling extraction of the header for now...
		if (m_iExtractState == MIME_EXTRACT_STATE_HEADER_FIRST ||
			m_iExtractState == MIME_EXTRACT_STATE_HEADER_LAST)
		{
			// Move to the next extraction state and start
			// walking through the token information again.
			m_iExtractState++;
			if (dec_fseek(m_fpTokenFile, 0, SEEK_SET) != 0)
			{
				rc = LEXER_ERROR;
				break;
			}

			goto findnext_top;
		}
		// *************

		// Read the next token.
		nRead = dec_fread(&m_emit, 1, sizeof(m_emit), m_fpTokenFile);
		if (nRead != sizeof(m_emit))
		{
			// Make sure we are stopping because of an EOF, not an error.
			rc = LEXER_ERROR;
			if (dec_feof(m_fpTokenFile))
			{
				if (m_iExtractState < MIME_EXTRACT_STATE_HEADER_LAST)
				{
					// Move to the next extraction state and start
					// walking through the token information again.
					m_iExtractState++;
					if (dec_fseek(m_fpTokenFile, 0, SEEK_SET) != 0)
					{
						rc = LEXER_ERROR;
						break;
					}

					goto findnext_top;
				}

				rc = LEXER_DONE;
			}
			break;
		}

#ifdef _DEBUG
//		DUMP_EMIT(m_emit);
#endif
		switch (EMIT_ID)
		{
			case MIME_TOKEN_BOUNDARY_START:
				m_bAttachment = false;
				m_nContentTypeID = -1;
				// Save offset of section start token for UpdateHeader routine.
				m_nSectionStart = dec_ftell(m_fpTokenFile) - sizeof(m_emit);
				m_nBoundaryStart = EMIT_OFFSET;
				break;

			case MIME_TOKEN_NAME:
				bPrimaryRFC2231 = (EMIT_FLAGS & RFC2231) ? true : false;
				rc = FindName(&PrimaryNameData);
				break;

			case MIME_TOKEN_FILENAME:
				bSecondaryRFC2231 = (EMIT_FLAGS & RFC2231) ? true : false;
				rc = FindName(&SecondaryNameData);
				break;

			default:
				if ( nLastTokenID == MIME_TOKEN_CONTENT_DISPOSITION &&
					 EMIT_ID == MIME_TOKEN_ATTACHMENT) {
					m_bAttachment = true;		
				}
				if ( nLastTokenID == MIME_TOKEN_CONTENT_TYPE ) {
					m_nContentTypeID = EMIT_ID;
				}
				break;
		}  // end of switch (EMIT_ID)

		// See if above logic wants us to bail out.
		if (rc != LEXER_OK)
			break;

		if (bReadFirst)
		{
			if (EMIT_ID == MIME_TOKEN_END_MARKER)
			{
				bReadFirst = false;

				if (bExtract)
				{
					bExtract = false;

					// END_MARKER tokens are always placed on the byte
					// following the encapsulated token sequence.  So, to
					// set the end offset to be the last byte of the token
					// sequence, we need to subtract one.
					m_nEndOffset = EMIT_OFFSET - 1;
					if (m_nEndOffset >= (m_nStartOffset - 1))
					{
						// If there is no primary name data, but there is secondary name
						// data, use the secondary name data as the primary as well.
						if (PrimaryNameData.pszName == NULL && SecondaryNameData.pszName != NULL)
						{
							// Construct the primary name from the secondary name's
							// FINDNAMEDATA structure.
							if (!ConstructName(&pszPrimaryName, &SecondaryNameData))
							{
								rc = LEXER_ERROR;
								break;
							}

							// Now that the primary name is actually using the secondary name,
							// change its RFC2231 flag to match.
							bPrimaryRFC2231 = bSecondaryRFC2231;
						}
						else
						{
							// Construct the primary name from its FINDNAMEDATA structure.
							if (!ConstructName(&pszPrimaryName, &PrimaryNameData))
							{
								rc = LEXER_ERROR;
								break;
							}
						}

						// Construct the secondary name from its FINDNAMEDATA structure.
						if (!ConstructName(&pszSecondaryName, &SecondaryNameData))
						{
							rc = LEXER_ERROR;
							break;
						}

						bool fFound;
						DecodeName(pszSecondaryName, bSecondaryRFC2231, &dwCharset, &fFound);
						DecodeName(pszPrimaryName, bPrimaryRFC2231, &dwCharset, &fFound);

						pData->nCompressedSize = m_nEndOffset - m_nStartOffset + 1;
						pData->pszName = pszPrimaryName;
						pData->pszSecondaryName = pszSecondaryName;
						pData->dwNameCharset = dwCharset;
						m_pArchive->m_dwFilenameCharset = dwCharset;
						//rc = LEXER_OK;  // This is already set.
						break;	// Return the enumerated block of data to the caller.
					}
				}
			}

			// We skip over all the other tokens in this section.
		}
		else
		{
			bReadFirst = true;
			m_iFieldID = EMIT_ID;  // Save off the ID.

			if (EMIT_ID == MIME_TOKEN_BODY &&
				m_iExtractState == MIME_EXTRACT_STATE_ATTACHMENT)
			{
				// Assume we want to extract this body section.
				bExtract = true;
				// If this is the message body section, see if we are configured to
				// extract message bodies.  The check for m_nBoundaryStart != 0 is
				// there so that we always extract the body of single-part messages.
				if (m_ParseState == DEC_ITEMTYPE_MIME_BODY && !m_bExtractMessageBodies && m_nBoundaryStart != 0)
				{
					m_ParseState = DEC_ITEMTYPE_NORMAL;
					bExtract = false;
				}
			}

			if (EMIT_ID == MIME_TOKEN_HEADER_SEPARATOR &&
				(m_iExtractState == MIME_EXTRACT_STATE_HEADER_FIRST ||
				m_iExtractState == MIME_EXTRACT_STATE_HEADER_LAST))
			{
				// *** DEBUG ***
				// Disabling extraction of the header for now...
				// Need a way to force the Decomposer NOT to recurse on
				// an extracted item like this.
				//bExtract = true;
				// *************
				m_iExtractState++;
			}

			if (bExtract)
			{
				// Get the object's encoding from EMIT_FLAGS
				m_iEncoding = MIME_TOKEN_NONE;
				if (EMIT_FLAGS & ENCODE_BASE64)
					m_iEncoding = MIME_TOKEN_BASE64;
				if (EMIT_FLAGS & ENCODE_UUE)
					m_iEncoding = MIME_TOKEN_UUENCODE;
				if (EMIT_FLAGS & ENCODE_7BIT)
					m_iEncoding = MIME_TOKEN_7BIT;
				if (EMIT_FLAGS & ENCODE_8BIT)
					m_iEncoding = MIME_TOKEN_8BIT;
				if (EMIT_FLAGS & ENCODE_QP)
					m_iEncoding = MIME_TOKEN_QUOTED_PRINTABLE;
				if (EMIT_FLAGS & ENCODE_BINARY)
					m_iEncoding = MIME_TOKEN_BINARY;
				if (EMIT_FLAGS & ENCRYPTED)
					m_iEncoding = MIME_TOKEN_ENCRYPTED;

				// Fill in the caller's TEXT_FIND_DATA structure.
				pData->dwAttributes = 0;
				pData->dwAttrType = DEC_ATTRTYPE_DOS;
				pData->dwYear = m_dwYear;
				pData->dwMonth = m_dwMonth;
				pData->dwDay = m_dwDay;
				pData->dwHour = m_dwHour;
				pData->dwMinute = m_dwMinute;
				pData->dwSecond = m_dwSecond;
				pData->dwMillisecond = 0;
				
				if (EMIT_ID == MIME_TOKEN_HEADER_SEPARATOR)
				{
					m_dwType = DEC_ITEMTYPE_MIME_HEADERS;
				}
				else
				{
					m_dwType = m_ParseState;
					m_ParseState = DEC_ITEMTYPE_NORMAL;
				}

				pData->dwType = m_dwType;
				pData->nCompressedSize = (size_t)-1;
				pData->nUncompressedSize = (size_t)-1;

				if (m_iExtractState == MIME_EXTRACT_STATE_HEADER_FIRST ||
					m_iExtractState == MIME_EXTRACT_STATE_HEADER_LAST)
				{
					// We always extract the header starting from offset 0.
					m_nStartOffset = 0;
				}
				else
				{
					// Everything else we extract starting from this token's offset.
					m_nStartOffset = EMIT_OFFSET;
				}
				// Now look for the end of the section before giving it
				// to the caller.
			}
		}
		nLastTokenID = EMIT_ID;
	}

	if (m_iExtractState >= MIME_EXTRACT_STATE_DONE)
		rc = LEXER_DONE;  // All done!  No more items to enumerate.

	if (rc != LEXER_OK)
	{
		if (pszPrimaryName)
			free(pszPrimaryName);
		if (pszSecondaryName)
			free(pszSecondaryName);
	}

	// Free any FINDNAMEDATA structures allocated due to RFC2231 handling.
	FreeFindNameData(&PrimaryNameData);
	FreeFindNameData(&SecondaryNameData);
	return (rc);
}


inline void CMIMEParser::FreeFindNameData(FINDNAMEDATA *pfnd)
{
	FINDNAMEDATA *pfnd1;
	FINDNAMEDATA *pfnd2;

	if (pfnd->pszName)
		free(pfnd->pszName);

	pfnd1 = pfnd->pNext;
	while (pfnd1)
	{
		pfnd2 = pfnd1->pNext;
		if (pfnd1->pszName)
			free(pfnd1->pszName);
		free(pfnd1);
		pfnd1 = pfnd2;
	}
}


bool CMIMEParser::ConstructName(char **ppszName, FINDNAMEDATA *pData)
{
	bool			bFound;
	FINDNAMEDATA	*pfnd;
	size_t			nLen = 0;
	size_t			nIndex = 0;
	size_t			nMaxIndex = 1;
	int				iCount = 0;

	// First, discover the total length of all strings that we need
	// to concatenate (order does not matter here).
	pfnd = pData;
	while (pfnd)
	{
		if (pfnd->pszName)
		{
			nLen += strlen(pfnd->pszName);
			iCount++;
		}

		if (pfnd->nIndex > nMaxIndex)
			nMaxIndex = pfnd->nIndex;

		pfnd = pfnd->pNext;
	}

	if (nLen == 0)
	{
		char	*pszName = NULL;

		// There is no name associated with this section so make one up.
		pszName = (char *)malloc(MIN_DECODE_NAME_BUFFER_SIZE);
		if (!pszName)
			return false;

		GenerateUnknownName(pszName, MIN_DECODE_NAME_BUFFER_SIZE);
		*ppszName = pszName;
		return true;
	}

	// Allocate space for the full name.
	// *** NOTE ***
	// The extra bytes allocated here are to guarantee that we give the
	// DecodeEncodedWords function some extra space in case it needs to
	// generate a name.
	// ************
	*ppszName = (char *)malloc(nLen + MIN_DECODE_NAME_BUFFER_SIZE);
	if (!*ppszName)
		return false;

	// NULL-terminate the buffer so that we can just use strcat's below.
	(*ppszName)[0] = 0;

	// Now repeatedly loop through the name strings and concatenate them
	// in order of their indices.
	do
	{
		bFound = false;
		pfnd = pData;
		while (pfnd)
		{
			if (pfnd->nIndex == nIndex)
			{
				strcat(*ppszName, pfnd->pszName);
				bFound = true;
				iCount--;
				break;
			}

			pfnd = pfnd->pNext;
		}

		if (nMaxIndex == 0)
			break;

		nMaxIndex--;
		nIndex++;  // Move to the next index.
	} while (bFound || iCount > 0);

	return true;
}


bool CMIMEParser::DecodeName(char *pszName, bool bRFC2231, DWORD *pdwCharset, bool* pfFound)

{
	size_t	nLen;
	char	*pszOutputName = NULL;

	if (!pszName)
		return true;

	// As per RFC 1522, the name may contain one or more encoded-words.
	// Translate any encoded-words in the name here before passing
	// the name out to the caller.  Note that the decoded string can not
	// possibly become larger than the original, so we only need to
	// allocate a buffer of at most the same size as the original.
	// *** NOTE ***
	// The extra bytes allocated here are to guarantee that we give the
	// DecodeEncodedWords function some extra space in case it needs to
	// generate a name.
	// ************
	nLen = strlen(pszName) + MIN_DECODE_NAME_BUFFER_SIZE;
	pszOutputName = (char *)malloc(nLen);
	if (!pszOutputName)
		return false;

	if (!DecodeEncodedWords(pszOutputName, (DWORD)nLen, pszName, pdwCharset, pfFound))
	{
		// Something wrong with the format of the encoded word string.
		free(pszOutputName);
		return false;
	}

	// Copy the output name over the input name.
	strcpy(pszName, pszOutputName);

	if (bRFC2231)
	{
		// This name was taken from a parameter that was specified in RFC2231
		// form.  So, we might need to decode it further.
#define RFC2231_STATE_CHARSET		0
#define RFC2231_STATE_LANGUAGE		1
#define RFC2231_STATE_NAME			2
		char	*ptr1;
		char	*ptr2;
		char	*pCharset = NULL;
		char	*pLanguage = NULL;
		char	ch;
		int		iState = RFC2231_STATE_CHARSET;

		ptr1 = pszName;
		ptr2 = pszOutputName;
		ch = *ptr1;
		while (ch != 0)
		{
			switch (iState)
			{
				case RFC2231_STATE_CHARSET:
					if (ch == 0x27)	// What is "'" - ASC_CHR_SINGLEQUOTE???
					{
						pCharset = pszName;
						*ptr1 = 0;
						pLanguage = ptr1;
						iState = RFC2231_STATE_LANGUAGE;
					}
					break;

				case RFC2231_STATE_LANGUAGE:
					if (ch == 0x27)	// What is "'" - ASC_CHR_SINGLEQUOTE???
					{
						*ptr1 = 0;
						iState = RFC2231_STATE_NAME;
					}
					break;

				case RFC2231_STATE_NAME:
					if (ch == 0x25)	// What is "%" - ASC_CHR_PERCENT???
					{
						char	chHex;

						// Decode the next two characters as hex.
						// Decode the 1st hex character.
						ptr1++;
						ch = *ptr1;
						if (ch == 0)
							break;

						if (ch >= ASC_CHR_0 && ch <= ASC_CHR_9)
							chHex = (ch - ASC_CHR_0) << 4;
						else if (ch >= ASC_CHR_A && ch <= ASC_CHR_F)
							chHex = (ch - ASC_CHR_A + 10) << 4;
						else if (ch >= ASC_CHR_a && ch <= ASC_CHR_f)
							chHex = (ch - ASC_CHR_a + 10) << 4;
						else
							break;

						// Decode the 2nd hex character.
						ptr1++;
						ch = *ptr1;
						if (ch == 0)
							break;

						if (ch >= ASC_CHR_0 && ch <= ASC_CHR_9)
							chHex += (ch - ASC_CHR_0);
						else if (ch >= ASC_CHR_A && ch <= ASC_CHR_F)
							chHex += (ch - ASC_CHR_A + 10);
						else if (ch >= ASC_CHR_a && ch <= ASC_CHR_f)
							chHex += (ch - ASC_CHR_a + 10);
						else
							break;

						ch = chHex;
					}

					*ptr2++ = ch;
					break;

				default:
					break;
			}

			ptr1++;
			ch = *ptr1;
		}

		// Only if we found both single-quote delimiters did we modify
		// the output name - in which case we need to NULL-terminate it.
		// Otherwise, the name is not in valid RFC2231 form, so just
		// return it as-is.
		if (iState == RFC2231_STATE_NAME)
		{
			*ptr2 = 0;

			if (pCharset)
				*pdwCharset = GetCharset(pCharset);

			// Copy the output name over the input name.
			strcpy(pszName, pszOutputName);
		}
	}

	free(pszOutputName);
	return true;
}


// This function builds up a filename from a series of one or more
// MIME_TOKEN_NAME or MIME_TOKEN_FILENAME tokens.
int CMIMEParser::FindName(FINDNAMEDATA *pData)
{
	size_t	nRead;
	DWORD	dwNameSize;
	FINDNAMEDATA *pfnd1;
	FINDNAMEDATA *pfnd2 = NULL;

	dec_assert(pData);	// Pointer must not be NULL.

	// Read the DWORD length of the string (length includes NULL terminator).
	nRead = dec_fread(&dwNameSize, 1, sizeof(dwNameSize), m_fpTokenFile);
	if (nRead != sizeof(dwNameSize))
		return LEXER_ERROR;

	if (dwNameSize == 0)
		return LEXER_OK;

	// We only need to read the name if we are in an extraction state that
	// actually uses it.
	if (m_iExtractState != MIME_EXTRACT_STATE_ATTACHMENT &&
		dwNameSize != 0)
	{
		// Seek past the name to set up read of next token.
		if (dec_fseek(m_fpTokenFile, dwNameSize, SEEK_CUR) != 0)
			return LEXER_ERROR;
		return LEXER_OK;
	}

	// We need to read the name into a buffer.  Locate the end of the
	// FINDNAMEDATA linked-list, allocate a buffer, and attach it to the list.
	pfnd1 = pData;
	while (pfnd1)
	{
		pfnd2 = pfnd1;
		if (pfnd1->pszName == NULL)
			break;

		pfnd1 = pfnd1->pNext;
	}

	if (pfnd1 == NULL)
	{
		// We did not find an empty FINDNAMEDATA structure, so allocate a
		// new one.
		pfnd1 = (FINDNAMEDATA *)malloc(sizeof(FINDNAMEDATA));
		if (!pfnd1)
			return LEXER_ERROR;

		// Clear out the new node.
		memset(pfnd1, 0, sizeof(FINDNAMEDATA));

		// Hook it up.
		pfnd2->pNext = pfnd1;
	}

	// Adding one to dwNameSize here to handle the case where
	// dwNameSize is zero.
	pfnd1->pszName = (char *)malloc(dwNameSize + 1);
	if (!pfnd1->pszName)
	{
		// Don't free the first node in the list - we didn't allocate it here!
		if (pfnd1 != pData)
			free(pfnd1);

		// Unhook the node.
		pfnd2->pNext = NULL;
		return LEXER_ERROR;	// Failed to allocate buffer for name.
	}

	pfnd1->pszName[0] = 0;
	if (dwNameSize != 0)
	{
		nRead = dec_fread(pfnd1->pszName, 1, dwNameSize, m_fpTokenFile);
		if (nRead != dwNameSize)
		{
			// Don't free the first node in the list - we didn't allocate it here!
			if (pfnd1 != pData)
				free(pfnd1);

			// Unhook the node.
			pfnd2->pNext = NULL;
			return LEXER_ERROR;
		}
	}

	// Save the index associated with this name section.
	pfnd1->nIndex = EMIT_DATA;
	return LEXER_OK;
}


#define MIME_EXTRACT_BUFFER_SIZE	(8 * 1024)

int CMIMEParser::ExtractEntry(char *pszFilename, FILE *fpin)
{
	int		rc = LEXER_ERROR;  // Assume an error
	FILE	*fpout = NULL;
	size_t	nBytesToRead;
	size_t	nTotalBytes;
	size_t	nRead;
	unsigned char *pbuffer = NULL;


	// First, make sure there is at least 1 byte to extract.
	// If not, extract a 0-byte file.
	if (m_nEndOffset <= m_nStartOffset)
	{
		fpout = dec_fopen(pszFilename, "w+b");
		rc = LEXER_OK;
		goto done_extract;
	}

	// Allocate a buffer for reading the original data.
	pbuffer = (unsigned char *)malloc(MIME_EXTRACT_BUFFER_SIZE);
	if (!pbuffer)
		goto done_extract;

	// Seek to the location of the data we want to extract.
	if (dec_fseek(fpin, m_nStartOffset, SEEK_SET) != 0)
		goto done_extract;

	switch (m_iEncoding)
	{
		case MIME_TOKEN_BASE64:
		{
			rc = WriteBase64(m_nStartOffset, m_nEndOffset, fpin, pszFilename);
			
			//
			// Map parser error codes to lexer error codes
			//
			if (rc == PARSER_ERROR)
				rc = LEXER_ERROR;
			else if (rc == PARSER_USER_ABORT)
				rc = LEXER_USER_ABORT;
			else if (rc == PARSER_MAX_EXTRACT_SIZE)
				rc = LEXER_MAX_EXTRACT_SIZE;
			else
				rc = LEXER_OK;
			break;
		}

		case MIME_TOKEN_QUOTED_PRINTABLE:
		{
			rc = WriteQP(m_nStartOffset, m_nEndOffset, fpin, pszFilename);
			
			//
			// Map parser error codes to lexer error codes
			//
			if (rc == PARSER_ERROR)
				rc = LEXER_ERROR;
			else if (rc == PARSER_USER_ABORT)
				rc = LEXER_USER_ABORT;
			else if (rc == PARSER_MAX_EXTRACT_SIZE)
				rc = LEXER_MAX_EXTRACT_SIZE;
			else
				rc = LEXER_OK;
			break;
		}

		case MIME_TOKEN_NONE:
		case MIME_TOKEN_UUENCODE:
		case MIME_TOKEN_7BIT:
		case MIME_TOKEN_8BIT:
		case MIME_TOKEN_BINARY:
		{
			fpout = dec_fopen(pszFilename, "w+b");
			if (!fpout)
				break;

			// Seek to the start of the data we want to extract.
			if (dec_fseek(fpin, m_nStartOffset, SEEK_SET) != 0)
				break;

            nTotalBytes = (m_nEndOffset - m_nStartOffset) + 1;

			// We have calculated the size of the stream, so see if
			// it will violate policy...
			if (g_dwTextMaxExtractSize &&
				nTotalBytes >= g_dwTextMaxExtractSize)
			{
				rc = LEXER_MAX_EXTRACT_SIZE;
				goto done_extract;
			}

			while (nTotalBytes)
			{
				nBytesToRead = MIME_EXTRACT_BUFFER_SIZE;
				if (nBytesToRead > nTotalBytes)
					nBytesToRead = nTotalBytes;
				nRead = dec_fread(pbuffer, 1, nBytesToRead, fpin);
				if (nRead == 0)
					break;

				if (dec_fwrite(pbuffer, 1, nRead, fpout) != nRead)
					break;

				nTotalBytes -= nRead;

				// Should we abort?
				if (g_bTextAbort)
				{
					rc = LEXER_USER_ABORT;
					goto done_extract;
				}
			}

			// Return success - the data has been extracted.
			rc = LEXER_OK;
			break;
		}

		case MIME_TOKEN_ENCRYPTED:
			rc = LEXER_ENCRYPTED;
			break;

		default:
		{
			// This should never happen.
			dec_assert(0);
			break;
		}
	}

done_extract:
	if (fpout && EOF == dec_fclose(fpout))
		rc = LEXER_ERROR;
	if (pbuffer)
		free(pbuffer);

	fpout = NULL;
	return (rc);
}


int CMIMEParser::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	int				rc = LEXER_ERROR;  // Assume failure
	DECRESULT		hr;
	FILE			*fpIn;
	FILE			*fpOut;
	char			*pszOutputFilename = NULL;

	// If the attachment was UU-Encoded, we must preserve this encoding.
	// Adding base64 encoding on top of UUE causes many e-mail readers to
	// display the UU-Encoded data instead of treating it as an attachment.
	// Also, if the file is MIME/Text we do not need to encode the result
	// as the original file is already acceptable.

	if (m_pArchive->m_dwChildType == DEC_TYPE_UUE ||
		m_pArchive->m_dwChildType == DEC_TYPE_HQX ||
		m_pArchive->m_dwChildType == DEC_TYPE_MIME)
	{
		switch (m_iEncoding)
		{
			case MIME_TOKEN_NONE:
			case MIME_TOKEN_7BIT:
			case MIME_TOKEN_UUENCODE:
				pData->bRawChange = true;
				if (pbKeepChildDataFile)
					*pbKeepChildDataFile = true;
				if (pbKeepChildNewDataFile)
					*pbKeepChildNewDataFile = true;
				break;

			case MIME_TOKEN_BASE64:
			case MIME_TOKEN_8BIT:
			case MIME_TOKEN_QUOTED_PRINTABLE:
			case MIME_TOKEN_BINARY:
				break;

			default:
				break;
		}
	}

	// Do not encode the data if the bRawChange flag is set.
	// We only need to encode the data if it is an attachment that we are replacing.
	// This is the case when pData->dwType is set to DEC_ITEMTYPE_NORMAL or DEC_ITEMTYPE_MIME_BODY.
	// Otherwise, just use the input file (from pData->pszFilename).
	if (pData->bRawChange == false &&
		(pData->dwType == DEC_ITEMTYPE_NORMAL ||
		pData->dwType == DEC_ITEMTYPE_MIME_BODY))
	{
		pszOutputFilename = (char *)malloc(MAX_PATH);
		if (pszOutputFilename)
		{
			hr = m_pEngine->TextCreateTempFile(".rep", pszOutputFilename, m_pDecomposer);
			if (!FAILED(hr))
			{
				// Here we need to encode the data before putting it back into the MIME message.
				// We *always* use base64 encoding because it is 100% guaranteed to be
				// correctly transported regardless of the data.
				fpIn = dec_fopen(pData->pszFilename, "rb");
				if (fpIn)
				{
					fpOut = dec_fopen(pszOutputFilename, "wb");
					if (fpOut)
					{
						// Encode the input file using base64.
						if (EncodeBase64(fpIn, fpOut) == PARSER_OK)
						{
							if (UpdateHeader(m_iEncoding,
											pData->bRenameItem ? pData->pszName : NULL,
											pData->pszContentType,
											pData->dwType) == LEXER_OK)
							{
								pData->pszFilename = pszOutputFilename;
								pData->nStartOffset = m_nStartOffset;
								pData->nEndOffset = m_nEndOffset;
								hr = m_pArchive->MarkForReplace(pData);
								if (!FAILED(hr))
									rc = LEXER_OK;
							}
						}

						if (EOF == dec_fclose(fpOut))
							rc = LEXER_ERROR;

						fpOut = NULL;
					}

					if (EOF == dec_fclose(fpIn))
						rc = LEXER_ERROR;

					fpIn = NULL;
				}
			}
		}
	}
	else
	{
		// We are replacing a non-DEC_ITEMTYPE_NORMAL block of text.
		// Use the input file just the way it is (from pData->pszFilename).
		pData->nStartOffset = m_nStartOffset;
		pData->nEndOffset = m_nEndOffset;
		hr = m_pArchive->MarkForReplace(pData);
		if (!FAILED(hr))
			rc = LEXER_OK;
	}

	// Delete the output file only if an error occurred.
	// Otherwise we need to keep the output file around
	// until we close the original file and re-write it
	// (see the Close() function).
	if (rc != LEXER_OK)
	{
		if (pszOutputFilename)
			dec_remove(pszOutputFilename);
	}

	// Free the output filename buffer.  Note that the name was written
	// to the marker file in the MarkForReplace call above if we are going
	// to be using the output file later.
	if (pszOutputFilename)
		free(pszOutputFilename);

	return (rc);
}


int CMIMEParser::AddEntry(TEXT_CHANGE_DATA *pData)
{
	int				rc = LEXER_OK;	// Assume success
	DECRESULT		hr;

	pData->nStartOffset = m_nStartOffset;
	pData->nEndOffset = m_nStartOffset;
	hr = m_pArchive->MarkForAdd(pData);
	if (FAILED(hr))
		rc = LEXER_ERROR;

	return (rc);
}


int CMIMEParser::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	int				rc = LEXER_OK;	// Assume success
	DECRESULT		hr;

	if (m_nBoundaryStart)
		pData->nStartOffset = m_nBoundaryStart;
	else
		pData->nStartOffset = m_nStartOffset;

	pData->nEndOffset = m_nEndOffset;
	hr = m_pArchive->MarkForDelete(pData);
	if (FAILED(hr))
		rc = LEXER_ERROR;

	return (rc);
}


int CMIMEParser::UpdateHeader(int iEncoding, const char *pszName, const char *pszContentType, DWORD dwType)
{
	DECRESULT	hr;
	int			rc = LEXER_ERROR;  // Assume an error.
	int			iLineType = MIME_TOKEN_NONE;
	size_t		nSaveOffset;
	size_t		nRead;
	size_t		nTruncateOffset = 0;
	size_t		nLen;
	size_t		nNewNameSize = 0;
	size_t		nNewTypeSize = 0;
	size_t		nNameStartOffset = 0;
	size_t		nNameEndOffset = 0;
	bool		bUpdatedEncoding = false;
	bool		bName = false;
	DWORD		dwOldNameSize = 0;
	char		*pszOldName = NULL;
	TEXT_CHANGE_DATA	tcd;

	// This function makes the necessary adjustments to the header
	// associated with the currently enumerated item.
	tcd.pszData = NULL;

	// Get the length of the new name.
	if (pszName)
		nNewNameSize = strlen(pszName);

	// Get the length of the new content type.
	if (pszContentType)
		nNewTypeSize = strlen(pszContentType);

	// Save the current find-first/find-next file pointer for the token file.
	nSaveOffset = dec_ftell(m_fpTokenFile);

	// Move the token file pointer back to where this section's header begins.
	if (dec_fseek(m_fpTokenFile, m_nSectionStart, SEEK_SET) != 0)
		goto done_update_header;

	// Loop through the tokens in the header and look for those that need
	// to be adjusted.
	while (true)
	{
		// Read the next token.
		nRead = dec_fread(&m_emit, 1, sizeof(m_emit), m_fpTokenFile);
		if (nRead != sizeof(m_emit))
		{
			// Make sure we are stopping because of an EOF, not an error.
			if (dec_feof(m_fpTokenFile))
			{
				rc = LEXER_OK;
				break;
			}

			break;
		}

		if (EMIT_ID == MIME_TOKEN_NAME ||
			EMIT_ID == MIME_TOKEN_FILENAME)
		{
			bName = true;

			// These tokens are followed by a DWORD size then a string (NULL-terminated).
			nRead = dec_fread(&dwOldNameSize, 1, sizeof(dwOldNameSize), m_fpTokenFile);
			if (nRead != sizeof(dwOldNameSize))
				break;	// We return LEXER_ERROR here - fread failed.

			if (dwOldNameSize)
			{
				// Allocate a buffer to hold the old name.
				if (pszOldName)
					free(pszOldName);

				pszOldName = (char *)malloc(dwOldNameSize);
				if (!pszOldName)
					break;	// We return LEXER_ERROR here - malloc failed.

				// Read the old name (read will include a terminal NULL).
				if (dec_fread(pszOldName, 1, dwOldNameSize, m_fpTokenFile) != dwOldNameSize)
					break;	// We return LEXER_ERROR here - fread failed.

				if (nNameStartOffset == 0)
					nNameStartOffset = EMIT_OFFSET;
			}
		}

		if (EMIT_ID == MIME_TOKEN_CONTENT_TYPE)
		{
			// Need to update a Content-Type line when we hit the end of the line.
			iLineType = MIME_TOKEN_CONTENT_TYPE;
			tcd.nStartOffset = EMIT_OFFSET;
		}

		if (EMIT_ID == MIME_TOKEN_CONTENT_DISPOSITION)
		{
			// Need to update a Content-Disposition line when we hit the end of the line.
			iLineType = MIME_TOKEN_CONTENT_DISPOSITION;
			tcd.nStartOffset = EMIT_OFFSET;
		}

		if (EMIT_ID == MIME_TOKEN_CONTENT_TRANSFER_ENCODING)
		{
			// Need to update a Content-Transfer-Encoding line when we hit the end of the line.
			iLineType = MIME_TOKEN_CONTENT_TRANSFER_ENCODING;
			tcd.nStartOffset = EMIT_OFFSET;
		}

		if (EMIT_ID == MIME_TOKEN_END_OF_NAME_MARKER)
		{
			// This terminates a filename= or name= token sequence.
			nNameEndOffset = EMIT_OFFSET;
		}

		if (EMIT_ID == MIME_TOKEN_END_MARKER)
		{
			bool	bFail = false;

			// We have reached the end of a line.  At this point we should have
			// all of the information we need to be able to re-construct the
			// line.  Now see if the line is of a type that needs to be updated.
			switch (iLineType)
			{
				case MIME_TOKEN_CONTENT_TYPE:
				{
					// If a new content type has been provided, construct a brand
					// new Content-Type line.  Otherwise just replace the name
					// portion of the line.
					if (nNewTypeSize != 0)
					{
						// Construct the new Content-Type line.
						// Allocate a buffer large enough to hold the whole new line.
						if (nNewNameSize)
							nLen = 100 + nNewTypeSize + nNewNameSize;
						else
							nLen = 100 + nNewTypeSize + dwOldNameSize;

						tcd.pszData = (char *)malloc(nLen);
						if (!tcd.pszData)
						{
							bFail = true;
							break;
						}

						// Build the string:
						// Content-Type: <user provided content type>;CRLF
						//     name="<user provided name>"
						strcpy(tcd.pszData, STR_CLIENT_PROVIDED_CONTENT_TYPE_REPLACEMENT_START);
						strcat(tcd.pszData, pszContentType);

						// Add the new name if one has been provided, but not if there was no
						// name present originally.
						if (nNewNameSize && nNameStartOffset != 0 && nNameEndOffset != 0)
						{
							strcat(tcd.pszData, STR_CLIENT_PROVIDED_CONTENT_TYPE_REPLACEMENT_END);
							strcat(tcd.pszData, "\x09");
							strcat(tcd.pszData, STR_NAME_REPLACEMENT);
							strcat(tcd.pszData, pszName);
							strcat(tcd.pszData, "\x22");
							tcd.nEndOffset = EMIT_OFFSET - 1;  // Replace the entire Content-Type line.
							nNameStartOffset = 0;
							nNameEndOffset = 0;
							break;
						}

						if (dwOldNameSize)
						{
							strcat(tcd.pszData, STR_CLIENT_PROVIDED_CONTENT_TYPE_REPLACEMENT_END);
							strcat(tcd.pszData, "\x09");
							strcat(tcd.pszData, STR_NAME_REPLACEMENT);
							strcat(tcd.pszData, pszOldName);
							strcat(tcd.pszData, "\x22");
							tcd.nEndOffset = EMIT_OFFSET - 1;  // Replace the entire Content-Type line.
							nNameStartOffset = 0;
							nNameEndOffset = 0;
							break;
						}

						tcd.nEndOffset = EMIT_OFFSET - 1;  // Replace the entire Content-Type line.
						nNameStartOffset = 0;
						nNameEndOffset = 0;
						break;
					}

					if (nNewNameSize && nNameStartOffset != 0 && nNameEndOffset != 0)
					{
						// We only need to replace the name portion of the Content-Type line.
						tcd.pszData = (char *)malloc(6 + nNewNameSize + 2);  // name="<new name>"\0
						if (!tcd.pszData)
						{
							bFail = true;
							break;
						}

						strcpy(tcd.pszData, STR_NAME_REPLACEMENT);
						strcat(tcd.pszData, pszName);
						strcat(tcd.pszData, "\x22");
						tcd.nStartOffset = nNameStartOffset;
						tcd.nEndOffset = nNameEndOffset;
						nNameStartOffset = 0;
						nNameEndOffset = 0;
						break;
					}

					break;
				}

				case MIME_TOKEN_CONTENT_DISPOSITION:
				{
					// We only need to replace the filename portion of a Content-Disposition line.
					if (nNewNameSize && nNameStartOffset != 0 && nNameEndOffset != 0)
					{
						tcd.pszData = (char *)malloc(10 + nNewNameSize + 2);  // filename="<new name>"\0
						if (!tcd.pszData)
						{
							bFail = true;
							break;
						}

						strcpy(tcd.pszData, STR_FILENAME_REPLACEMENT);
						strcat(tcd.pszData, pszName);
						strcat(tcd.pszData, "\x22");
						tcd.nStartOffset = nNameStartOffset;
						tcd.nEndOffset = nNameEndOffset;
						nNameStartOffset = 0;
						nNameEndOffset = 0;
						break;
					}

					break;
				}

				case MIME_TOKEN_CONTENT_TRANSFER_ENCODING:
				{
					// Construct the new Content-Transfer-Encoding line.
					tcd.pszData = (char *)malloc(256);
					if (!tcd.pszData)
					{
						bFail = true;
						break;
					}

					strcpy(tcd.pszData, STR_CONTENT_TRANSFER_ENCODING_REPLACEMENT);
					bUpdatedEncoding = true;
					tcd.nEndOffset = EMIT_OFFSET - 1;  // Replace the entire Content-Transfer-Encoding line.
					break;
				}

				default:
					break;
			}

			iLineType = MIME_TOKEN_NONE;

			// Did a malloc fail?
			if (bFail)
				break;	// Yes, bail out.

			// Did we create a replacement line?
			if (tcd.pszData)
			{
				// Yes, we created a replacement line.
				// Save off the replacement data block.
				hr = m_pArchive->MarkForReplace(&tcd);
				free(tcd.pszData);
				tcd.pszData = NULL;

				if (FAILED(hr))
					break;
			}
		}

		if (EMIT_ID == MIME_TOKEN_HEADER_SEPARATOR)
		{
			// Since we always re-encode the data using base64, it
			// is always necessary to have a Content-Transfer-Encoding
			// header line that specifies base64 as the encoding type.
			// If we have not updated a transfer-encoding line yet, we
			// need to add one.
			if (!bUpdatedEncoding)
			{
				// Construct the new Content-Transfer-Encoding line.
				tcd.pszData = (char *)malloc(1024);
				if (!tcd.pszData)
					break;

				strcpy(tcd.pszData, STR_CONTENT_TRANSFER_ENCODING_REPLACEMENT);
				strcat(tcd.pszData, "\x0D\x0A\x0D\x0A");
				tcd.nStartOffset = EMIT_OFFSET;
				tcd.nEndOffset = EMIT_OFFSET + EMIT_DATA - 1;
				hr = m_pArchive->MarkForReplace(&tcd);
				free(tcd.pszData);
				tcd.pszData = NULL;

				if (FAILED(hr))
					break;
			}

			rc = LEXER_OK;
			break;
		}
	}

done_update_header:
	// Restore the find-first/find-next file pointer for the token file.
	if (dec_fseek(m_fpTokenFile, nSaveOffset, SEEK_SET) != 0)
		rc = LEXER_ERROR;

	if (pszOldName)
		free(pszOldName);

	if (tcd.pszData)
		free(tcd.pszData);

	return (rc);
}


int CMIMEParser::FindFirstSection(MIMESECTIONINFO *pInfo)
{
	// Make sure the intermediate file is available (i.e. open).
	if (!m_fpTokenFile)
	{
		if (!m_pszTokenFile)
			return MIME_ERROR_GENERIC;

		m_fpTokenFile = dec_fopen(m_pszTokenFile, "rb");
		if (!m_fpTokenFile)
			return MIME_ERROR_GENERIC;
	}

	m_nStartOffset = 0;

	// Rewind back to the start of the token data.
	if (dec_fseek(m_fpTokenFile, 0, SEEK_SET) != 0)
		return MIME_ERROR_GENERIC;

	// Start looking through the tokenized information.
	return (FindNextSection(pInfo));
}


int CMIMEParser::FindNextSection(MIMESECTIONINFO *pInfo)
{
	int			rc;
	bool		bReadFirst = false;
	bool		bExtract = false;
	bool		bWaitForSeparator = false;
	bool		bPrimaryRFC2231 = false;
	bool		bSecondaryRFC2231 = false;
	size_t		nRead;
	char		*pszPrimaryName = NULL;
	char		*pszSecondaryName = NULL;
	DWORD		dwCharset;
	FINDNAMEDATA PrimaryNameData;
	FINDNAMEDATA SecondaryNameData;

	// Reset the primary and secondary name strings.
	pInfo->pszName = NULL;
	pInfo->pszSecondaryName = NULL;

	// Clear the primary and secondary name construction structures.
	memset(&PrimaryNameData, 0, sizeof(PrimaryNameData));
	memset(&SecondaryNameData, 0, sizeof(SecondaryNameData));

	// Make sure the intermediate file is available (i.e. open).
	if (!m_fpTokenFile)
		return MIME_ERROR_GENERIC;

	m_iExtractState = MIME_EXTRACT_STATE_ATTACHMENT;
	m_nBoundaryStart = 0;

	// Keep reading tokens until we locate something of interest.
	while (true)
	{
		rc = MIME_ERROR_NONE;  // Assume success

		// Read the next token.
		nRead = dec_fread(&m_emit, 1, sizeof(m_emit), m_fpTokenFile);
		if (nRead != sizeof(m_emit))
		{
			// Make sure we are stopping because of an EOF, not an error.
			rc = MIME_ERROR_GENERIC;
			if (dec_feof(m_fpTokenFile))
				rc = MIME_ERROR_NO_MORE_SECTIONS;
			break;
		}

		switch (EMIT_ID)
		{
			case MIME_TOKEN_NAME:
				bPrimaryRFC2231 = (EMIT_FLAGS & RFC2231) ? true : false;
				if (FindName(&PrimaryNameData) != LEXER_OK)
					rc = MIME_ERROR_GENERIC;
				break;

			case MIME_TOKEN_FILENAME:
				bSecondaryRFC2231 = (EMIT_FLAGS & RFC2231) ? true : false;
				if (FindName(&SecondaryNameData) != LEXER_OK)
					rc = MIME_ERROR_GENERIC;
				break;

			default:
				break;
		}  // end of switch (EMIT_ID)

		// See if above logic wants us to bail out.
		if (rc != MIME_ERROR_NONE)
			break;

		if (bReadFirst)
		{
			if (bWaitForSeparator)
			{
				if (EMIT_ID == MIME_TOKEN_HEADER_SEPARATOR)
					bWaitForSeparator = false;

				if (EMIT_ID == MIME_TOKEN_EOF)
				{
					bWaitForSeparator = false;
					goto found_end_marker;
				}
			}
			else if (EMIT_ID == MIME_TOKEN_END_MARKER)
			{
found_end_marker:
				bReadFirst = false;

				// Do not enumerate blocks unless they are at least 1 byte
				// in size.
				if (bExtract && m_nStartOffset < EMIT_OFFSET)
				{
					// If there is no primary name data, but there is secondary name
					// data, use the secondary name data as the primary as well.
					if (PrimaryNameData.pszName == NULL && SecondaryNameData.pszName != NULL)
					{
						// Construct the primary name from the secondary name's
						// FINDNAMEDATA structure.
						if (!ConstructName(&pszPrimaryName, &SecondaryNameData))
						{
							rc = LEXER_ERROR;
							break;
						}

						// Now that the primary name is actually using the secondary name,
						// change its RFC2231 flag to match.
						bPrimaryRFC2231 = bSecondaryRFC2231;
					}
					else
					{
						// Construct the primary name from its FINDNAMEDATA structure.
						if (!ConstructName(&pszPrimaryName, &PrimaryNameData))
						{
							rc = LEXER_ERROR;
							break;
						}
					}

					// Construct the secondary name from its FINDNAMEDATA structure.
					if (!ConstructName(&pszSecondaryName, &SecondaryNameData))
					{
						rc = MIME_ERROR_GENERIC;
						break;
					}

					bool fFound;
					DecodeName(pszSecondaryName, bSecondaryRFC2231, &dwCharset, &fFound);
					DecodeName(pszPrimaryName, bPrimaryRFC2231, &dwCharset, &fFound);

					// END_MARKER tokens are always placed on the byte
					// following the encapsulated token sequence.  So, to
					// set the end offset to be the last byte of the token
					// sequence, we need to subtract one.
					m_nEndOffset = EMIT_OFFSET - 1;
					pInfo->nStartOffset = m_nStartOffset;
					pInfo->nEndOffset = m_nEndOffset;
					pInfo->pszName = pszPrimaryName;
					pInfo->pszSecondaryName = pszSecondaryName;
					pInfo->dwNameCharset = dwCharset;
					//rc = MIME_ERROR_NONE;  // This is already set.
					break;	// Return the enumerated block of data to the caller.
				}

				bExtract = false;
			}

			// We skip over all the other tokens in this section.
		}
		else
		{
			bReadFirst = true;
			m_iFieldID = EMIT_ID;  // Save off the ID.
			if (EMIT_ID == MIME_TOKEN_HEADER_SEPARATOR)
			{
				// If this is the first header separator, then the
				// caller knows this is the main header.  Otherwise,
				// this must be a multi-part header.
				pInfo->nSectionID = MIME_SECTION_MULTIPART_HEADER;
				bExtract = true;
			}

			if (EMIT_ID == MIME_TOKEN_PREAMBLE)
			{
				pInfo->nSectionID = MIME_SECTION_PREAMBLE;
				m_nStartOffset = EMIT_OFFSET;
				bExtract = true;
			}

			if (EMIT_ID == MIME_TOKEN_BOUNDARY_START)
			{
				pInfo->nSectionID = MIME_SECTION_MULTIPART_HEADER;
				m_nStartOffset = EMIT_OFFSET;
				m_nBoundaryStart = EMIT_OFFSET;
				bExtract = true;
				bWaitForSeparator = true;
			}

			if (EMIT_ID == MIME_TOKEN_BODY)
			{
				pInfo->nSectionID = MIME_SECTION_BODY;
				m_nStartOffset = EMIT_OFFSET;
				bExtract = true;
			}

			if (bExtract)
			{
				// Get the object's encoding from EMIT_FLAGS
				m_iEncoding = MIME_TOKEN_NONE;
				if (EMIT_FLAGS & ENCODE_BASE64)
					m_iEncoding = MIME_TOKEN_BASE64;
				if (EMIT_FLAGS & ENCODE_UUE)
					m_iEncoding = MIME_TOKEN_UUENCODE;
				if (EMIT_FLAGS & ENCODE_7BIT)
					m_iEncoding = MIME_TOKEN_7BIT;
				if (EMIT_FLAGS & ENCODE_8BIT)
					m_iEncoding = MIME_TOKEN_8BIT;
				if (EMIT_FLAGS & ENCODE_QP)
					m_iEncoding = MIME_TOKEN_QUOTED_PRINTABLE;
				if (EMIT_FLAGS & ENCODE_BINARY)
					m_iEncoding = MIME_TOKEN_BINARY;
				if (EMIT_FLAGS & ENCRYPTED)
					m_iEncoding = MIME_TOKEN_ENCRYPTED;

				// Fill in the caller's MIMESECTIONINFO structure.
				pInfo->nStartOffset = 0;
				pInfo->nEndOffset = 0;
				pInfo->pszName = NULL;
				pInfo->pszSecondaryName = NULL;
				pInfo->dwNameCharset = DEC_CHARSET_US_ASCII;
				pInfo->dwYear = m_dwYear;
				pInfo->dwMonth = m_dwMonth;
				pInfo->dwDay = m_dwDay;
				pInfo->dwHour = m_dwHour;
				pInfo->dwMinute = m_dwMinute;
				pInfo->dwSecond = m_dwSecond;
				pInfo->iTimezoneDifferential = 0;
				pInfo->bMultipart = false;
				pInfo->nMultipartCount = 0;
				// Now look for the end of the section before giving it
				// to the caller.
			}
		}
	}

	if (rc != MIME_ERROR_NONE)
	{
		if (pszPrimaryName)
			free(pszPrimaryName);
		if (pszSecondaryName)
			free(pszSecondaryName);
	}

	// Free any FINDNAMEDATA structures allocated due to RFC2231 handling.
	FreeFindNameData(&PrimaryNameData);
	FreeFindNameData(&SecondaryNameData);
	return (rc);
}

bool CMIMEParser::isMultipart() const {
	return m_bMultipart;
}


/*
// MIME state machine functions
int CMIMEParser::Stub_HEADERFIELD1(CMIMEParser *pThis)
{
	return pThis->State_HEADERFIELD1();
}

int CMIMEParser::Stub_HEADERFIELD2(CMIMEParser *pThis)
{
	return pThis->State_HEADERFIELD2();
}

int CMIMEParser::Stub_HEADERFIELD3(CMIMEParser *pThis)
{
	return pThis->State_HEADERFIELD3();
}

int CMIMEParser::Stub_HEADERFIELD4(CMIMEParser *pThis)
{
	return pThis->State_HEADERFIELD4();
}

int CMIMEParser::Stub_RETURNPATH1(CMIMEParser *pThis)
{
	return pThis->State_RETURNPATH1();
}

int CMIMEParser::Stub_RETURNPATH2(CMIMEParser *pThis)
{
	return pThis->State_RETURNPATH2();
}

int CMIMEParser::Stub_RETURNPATH3(CMIMEParser *pThis)
{
	return pThis->State_RETURNPATH3();
}

int CMIMEParser::Stub_ROUTEADDR1(CMIMEParser *pThis)
{
	return pThis->State_ROUTEADDR1();
}

int CMIMEParser::Stub_ROUTEADDR2(CMIMEParser *pThis)
{
	return pThis->State_ROUTEADDR2();
}

int CMIMEParser::Stub_ROUTEADDR3(CMIMEParser *pThis)
{
	return pThis->State_ROUTEADDR3();
}

int CMIMEParser::Stub_RECEIVED1(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED1();
}

int CMIMEParser::Stub_RECEIVED2(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED2();
}

int CMIMEParser::Stub_RECEIVED3(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED3();
}

int CMIMEParser::Stub_RECEIVED4(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED4();
}

int CMIMEParser::Stub_RECEIVED5(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED5();
}

int CMIMEParser::Stub_RECEIVED6(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED6();
}

int CMIMEParser::Stub_RECEIVED7(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED7();
}

int CMIMEParser::Stub_RECEIVED8(CMIMEParser *pThis)
{
	return pThis->State_RECEIVED8();
}

int CMIMEParser::Stub_REPLYTO1(CMIMEParser *pThis)
{
	return pThis->State_REPLYTO1();
}

int CMIMEParser::Stub_FROM1(CMIMEParser *pThis)
{
	return pThis->State_FROM1();
}

int CMIMEParser::Stub_SENDER1(CMIMEParser *pThis)
{
	return pThis->State_SENDER1();
}

int CMIMEParser::Stub_DATE1(CMIMEParser *pThis)
{
	return pThis->State_DATE1();
}

int CMIMEParser::Stub_DATE2(CMIMEParser *pThis)
{
	return pThis->State_DATE2();
}

int CMIMEParser::Stub_DATE3(CMIMEParser *pThis)
{
	return pThis->State_DATE3();
}

int CMIMEParser::Stub_DATE4(CMIMEParser *pThis)
{
	return pThis->State_DATE4();
}

int CMIMEParser::Stub_DATE5(CMIMEParser *pThis)
{
	return pThis->State_DATE5();
}

int CMIMEParser::Stub_DATE6(CMIMEParser *pThis)
{
	return pThis->State_DATE6();
}

int CMIMEParser::Stub_DATE7(CMIMEParser *pThis)
{
	return pThis->State_DATE7();
}

int CMIMEParser::Stub_DATE8(CMIMEParser *pThis)
{
	return pThis->State_DATE8();
}

int CMIMEParser::Stub_DATE9(CMIMEParser *pThis)
{
	return pThis->State_DATE9();
}

int CMIMEParser::Stub_DATE10(CMIMEParser *pThis)
{
	return pThis->State_DATE10();
}

int CMIMEParser::Stub_TO1(CMIMEParser *pThis)
{
	return pThis->State_TO1();
}

int CMIMEParser::Stub_CC1(CMIMEParser *pThis)
{
	return pThis->State_CC1();
}

int CMIMEParser::Stub_BCC1(CMIMEParser *pThis)
{
	return pThis->State_BCC1();
}

int CMIMEParser::Stub_MESSAGEID1(CMIMEParser *pThis)
{
	return pThis->State_MESSAGEID1();
}

int CMIMEParser::Stub_INREPLYTO1(CMIMEParser *pThis)
{
	return pThis->State_INREPLYTO1();
}

int CMIMEParser::Stub_REFERENCES1(CMIMEParser *pThis)
{
	return pThis->State_REFERENCES1();
}

int CMIMEParser::Stub_KEYWORDS1(CMIMEParser *pThis)
{
	return pThis->State_KEYWORDS1();
}

int CMIMEParser::Stub_SUBJECT1(CMIMEParser *pThis)
{
	return pThis->State_SUBJECT1();
}

int CMIMEParser::Stub_COMMENTS1(CMIMEParser *pThis)
{
	return pThis->State_COMMENTS1();
}

int CMIMEParser::Stub_ENCRYPTED1(CMIMEParser *pThis)
{
	return pThis->State_ENCRYPTED1();
}

int CMIMEParser::Stub_CONTENTTYPE1(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE1();
}

int CMIMEParser::Stub_CONTENTTYPE2(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE2();
}

int CMIMEParser::Stub_CONTENTTYPE3(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE3();
}

int CMIMEParser::Stub_CONTENTTYPE4(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE4();
}

int CMIMEParser::Stub_CONTENTTYPE5(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE5();
}

int CMIMEParser::Stub_CONTENTTYPE6(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE6();
}

int CMIMEParser::Stub_CONTENTTYPE7(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE7();
}

int CMIMEParser::Stub_CONTENTTYPE8(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE8();
}

int CMIMEParser::Stub_CONTENTTYPE9(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE9();
}

int CMIMEParser::Stub_CONTENTTYPE10(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE10();
}

int CMIMEParser::Stub_CONTENTTYPE11(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTYPE11();
}

int CMIMEParser::Stub_CONTENTDISPOSITION1(CMIMEParser *pThis)
{
	return pThis->State_CONTENTDISPOSITION1();
}

int CMIMEParser::Stub_CONTENTDISPOSITION2(CMIMEParser *pThis)
{
	return pThis->State_CONTENTDISPOSITION2();
}

int CMIMEParser::Stub_CONTENTDISPOSITION3(CMIMEParser *pThis)
{
	return pThis->State_CONTENTDISPOSITION3();
}

int CMIMEParser::Stub_CONTENTDISPOSITION4(CMIMEParser *pThis)
{
	return pThis->State_CONTENTDISPOSITION4();
}

int CMIMEParser::Stub_CONTENTDESCRIPTION1(CMIMEParser *pThis)
{
	return pThis->State_CONTENTDESCRIPTION1();
}

int CMIMEParser::Stub_CONTENTTRANSFERENCODING1(CMIMEParser *pThis)
{
	return pThis->State_CONTENTTRANSFERENCODING1();
}

int CMIMEParser::Stub_CONTENTID1(CMIMEParser *pThis)
{
	return pThis->State_CONTENTID1();
}

int CMIMEParser::Stub_MIMEVERSION1(CMIMEParser *pThis)
{
	return pThis->State_MIMEVERSION1();
}

int CMIMEParser::Stub_IMPORTANCE1(CMIMEParser *pThis)
{
	return pThis->State_IMPORTANCE1();
}

int CMIMEParser::Stub_BODY1(CMIMEParser *pThis)
{
	return pThis->State_BODY1();
}

int CMIMEParser::Stub_BODY2(CMIMEParser *pThis)
{
	return pThis->State_BODY2();
}

int CMIMEParser::Stub_MULTIPART1(CMIMEParser *pThis)
{
	return pThis->State_MULTIPART1();
}

int CMIMEParser::Stub_MULTIPART2(CMIMEParser *pThis)
{
	return pThis->State_MULTIPART2();
}

int CMIMEParser::Stub_MULTIPART3(CMIMEParser *pThis)
{
	return pThis->State_MULTIPART3();
}

int CMIMEParser::Stub_MULTIPARTHEADER1(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTHEADER1();
}

int CMIMEParser::Stub_MULTIPARTHEADER2(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTHEADER2();
}

int CMIMEParser::Stub_MULTIPARTHEADER3(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTHEADER3();
}

int CMIMEParser::Stub_MULTIPARTHEADER4(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTHEADER4();
}

int CMIMEParser::Stub_MULTIPARTBODY1(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTBODY1();
}

int CMIMEParser::Stub_MULTIPARTBODY2(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTBODY2();
}

int CMIMEParser::Stub_MULTIPARTBODY3(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTBODY3();
}

int CMIMEParser::Stub_MULTIPARTBODY4(CMIMEParser *pThis)
{
	return pThis->State_MULTIPARTBODY4();
}

int CMIMEParser::Stub_ORGANIZATION1(CMIMEParser *pThis)
{
	return pThis->State_ORGANIZATION1();
}

int CMIMEParser::Stub_AUTHOR1(CMIMEParser *pThis)
{
	return pThis->State_AUTHOR1();
}

int CMIMEParser::Stub_PRIORITY1(CMIMEParser *pThis)
{
	return pThis->State_PRIORITY1();
}

int CMIMEParser::Stub_DELIVEREDTO1(CMIMEParser *pThis)
{
	return pThis->State_DELIVEREDTO1();
}

int CMIMEParser::Stub_CONTENTLOCATION1(CMIMEParser *pThis)
{
	return pThis->State_CONTENTLOCATION1();
}

int CMIMEParser::Stub_THREADTOPIC1(CMIMEParser *pThis)
{
	return pThis->State_THREADTOPIC1();
}

int CMIMEParser::Stub_THREADINDEX1(CMIMEParser *pThis)
{
	return pThis->State_THREADINDEX1();
}

int CMIMEParser::Stub_MAILER1(CMIMEParser *pThis)
{
	return pThis->State_MAILER1();
}

int CMIMEParser::Stub_CONTENTLENGTH1(CMIMEParser *pThis)
{
	return pThis->State_CONTENTLENGTH1();
}
*/
