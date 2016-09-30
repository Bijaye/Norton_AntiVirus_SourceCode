// IMIME.h : Header for public MIME engine interface.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#ifndef _IMIME_H_
#define _IMIME_H_
// Return codes for all IMIME APIs.
#define MIME_ERROR_NONE					0
#define MIME_ERROR_INVALID_PARAMS		1
#define MIME_ERROR_NO_FILENAME			2
#define MIME_ERROR_NO_MORE_SECTIONS		3
#define MIME_ERROR_GENERIC				100

// Section ID values.
#define MIME_SECTION_MAIN_HEADER		1000
#define MIME_SECTION_PREAMBLE			1001
#define MIME_SECTION_MULTIPART_HEADER	1002
#define MIME_SECTION_BODY				1003

typedef struct tagMIMESectionInfo
{
	size_t			nSectionID;
	size_t			nStartOffset;
	size_t			nEndOffset;
	char			*pszName;
	char			*pszSecondaryName;
	unsigned long	dwNameCharset;
	unsigned long	dwYear;
	unsigned long	dwMonth;
	unsigned long	dwDay;
	unsigned long	dwHour;
	unsigned long	dwMinute;
	unsigned long	dwSecond;
	int				iTimezoneDifferential;
	bool			bMultipart;
	size_t			nMultipartCount;
} MIMESECTIONINFO, *PMIMESECTIONINFO;


enum MIME_WARNINGS
{
	MIME_WARNING_UNEXPECTED_TOKEN = 10000,
	MIME_WARNING_UNEXPECTED_SEPARATOR,
	MIME_WARNING_UNEXPECTED_BOUNDARY_CLOSURE,
	MIME_WARNING_MAX_BOUNDARY_NESTING,
	MIME_WARNING_INVALID_HEADER,
	MIME_WARNING_INVALID_DAYOFWEEK,
	MIME_WARNING_INVALID_DAYOFMONTH,
	MIME_WARNING_INVALID_MONTH,
	MIME_WARNING_INVALID_YEAR,
	MIME_WARNING_INVALID_HOUR,
	MIME_WARNING_INVALID_MINUTE,
	MIME_WARNING_INVALID_SECOND,
	MIME_WARNING_LONG_DATE,
	MIME_WARNING_INVALID_MULTIPART_HEADER,
	MIME_WARNING_MAX_FILENAME_LENGTH,
	MIME_WARNING_BARE_CARRIAGE_RETURN,
	MIME_WARNING_BARE_LINE_FEED,
	MIME_WARNING_EMPTY_BOUNDARY_SPECIFIED,
	MIME_WARNING_MULTIPLE_DIFFERENT_TRANSFER_ENCODINGS
};


typedef struct tagMIMEWarning
{
	size_t			nWarningID;			// MIME_WARNING_xxx
	size_t			nOffset;
	size_t			nLineNumber;
	size_t			nContextTokenID;	// MIME_TOKEN_xxx
	size_t			nLength;			// Length of string to follow.
//  char            szString[nLength];
} MIMEWARNING, *PMIMEWARNING;


class IMIME
{
public:
	IMIME() {}
	virtual ~IMIME() {}

	// IMIME methods:
	virtual int		GetTokenFilename(char *pszFilename, size_t nBufferSize) = 0;
	virtual int		GetWarningFilename(char *pszFilename, size_t nBufferSize) = 0;
	virtual int		GetSectionInfo(MIMESECTIONINFO *pInfo, size_t nSectionID, size_t nSectionNumber) = 0;
	virtual int		ExtractRawSection(char *pszFilename, size_t nSectionID, size_t nSectionNumber) = 0;
	virtual int		ReplaceRawSection(char *pszFilename, size_t nSectionID, size_t nSectionNumber) = 0;
	virtual int		ExtractBody(char *pszFilename, size_t nSectionNumber) = 0;
};


// Definitions for MIMEEMITTOKEN dwFlags field.
#define NO				0x00000000		// Transition is not allowed
#define YES				0x00000001		// Transition is allowed and RFC-compliant
#define MAL				0x00000002		// Transition is allowed, but not RFC-compliant
#define HMAIL			0x00100000		// HandyMail specific
#define MSOUT			0x00200000		// MS-OutLook specific
#define ENCODE_NONE		0x00000010		// No encoding
#define ENCODE_BASE64	0x00000020		// Base64 encoding
#define ENCODE_UUE		0x00000040		// UUE encoding
#define ENCODE_7BIT		0x00000080		// 7-bit encoding
#define ENCODE_8BIT		0x00000100		// 8-bit encoding
#define ENCODE_QP		0x00000200		// Quoted-printable encoding
#define ENCODE_BINARY	0x00000400		// Binary encoding
#define ENCRYPTED		0x00000800		// Content is encrypted
#define RFC2231			0x20000000		// Token ID is an RFC 2231 continuation token {token "*" digit}
#define EXTENSION		0x40000000		// Token ID is an extension token {"X-" token ":" field}
#define RESENT			0x80000000		// Token ID is a {"Resent-xxx" ":"} field

typedef struct tagMIMEEmitToken
{
	size_t			iTokenID;
	size_t			nTokenOffset;
	size_t			nTokenData;
	unsigned long	dwFlags;
} MIMEEMITTOKEN, *PMIMEEMITTOKEN;


enum MIMELexerTokens
{
// Start of lexer tokens
	MIME_TOKEN_LWSP = 0,					// 0
	MIME_TOKEN_CR,							// 1
	MIME_TOKEN_LF,							// 2
	MIME_TOKEN_CRLF,						// 3
	MIME_TOKEN_SPECIAL,						// 4
	MIME_TOKEN_ATOM,						// 5
	MIME_TOKEN_QUOTED_STRING,				// 6
	MIME_TOKEN_DOMAIN_LITERAL,				// 7
	MIME_TOKEN_COMMENT,						// 8
	MIME_TOKEN_ROUTE_ADDR,					// 9
	MIME_TOKEN_QUOTED_PAIR,					// 10
	MIME_TOKEN_EOF,							// 11
// End of lexer tokens
// Start of parser tokens
	MIME_TOKEN_END_MARKER,					// 12
	MIME_TOKEN_HEADER_SEPARATOR,			// 13
	MIME_TOKEN_RETURN_PATH,					// 14
	MIME_TOKEN_RECEIVED,					// 15
	MIME_TOKEN_REPLY_TO,					// 16
	MIME_TOKEN_FROM,						// 17
	MIME_TOKEN_SENDER,						// 18
	MIME_TOKEN_DATE,						// 19
	MIME_TOKEN_TO,							// 20
	MIME_TOKEN_CC,							// 21
	MIME_TOKEN_BCC,							// 22
	MIME_TOKEN_MESSAGE_ID,					// 23
	MIME_TOKEN_IN_REPLY_TO,					// 24
	MIME_TOKEN_REFERENCES,					// 25
	MIME_TOKEN_KEYWORDS,					// 26
	MIME_TOKEN_SUBJECT,						// 27
	MIME_TOKEN_COMMENTS,					// 28
	MIME_TOKEN_ENCRYPTED,					// 29
	MIME_TOKEN_BY,							// 30
	MIME_TOKEN_VIA,							// 31
	MIME_TOKEN_WITH,						// 32
	MIME_TOKEN_ID,							// 33
	MIME_TOKEN_FOR,							// 34
	MIME_TOKEN_UNKNOWN_FIELD,				// 35
	MIME_TOKEN_CONTENT_TYPE,				// 36
	MIME_TOKEN_CONTENT_DISPOSITION,			// 37
	MIME_TOKEN_CONTENT_DESCRIPTION,			// 38
	MIME_TOKEN_CONTENT_TRANSFER_ENCODING,	// 39
	MIME_TOKEN_CONTENT_ID,					// 40
	MIME_TOKEN_MIME_VERSION,				// 41
	MIME_TOKEN_IMPORTANCE,					// 42
	MIME_TOKEN_PREAMBLE,					// 43
	MIME_TOKEN_BODY,						// 44
	MIME_TOKEN_BASE64,						// 45
	MIME_TOKEN_UUENCODE,					// 46
	MIME_TOKEN_7BIT,						// 47
	MIME_TOKEN_8BIT,						// 48
	MIME_TOKEN_QUOTED_PRINTABLE,			// 49
	MIME_TOKEN_BINARY,						// 50
	MIME_TOKEN_NONE,						// 51
	MIME_TOKEN_ATTACHMENT,					// 52
	MIME_TOKEN_FILENAME,					// 53
	MIME_TOKEN_DOMAIN,						// 54
	MIME_TOKEN_BOUNDARY_START,				// 55
	MIME_TOKEN_MONDAY,						// 56
	MIME_TOKEN_TUESDAY,						// 57
	MIME_TOKEN_WEDNESDAY,					// 58
	MIME_TOKEN_THURSDAY,					// 59
	MIME_TOKEN_FRIDAY,						// 60
	MIME_TOKEN_SATURDAY,					// 61
	MIME_TOKEN_SUNDAY,						// 62
	MIME_TOKEN_JANUARY,						// 63
	MIME_TOKEN_FEBRUARY,					// 64
	MIME_TOKEN_MARCH,						// 65
	MIME_TOKEN_APRIL,						// 66
	MIME_TOKEN_MAY,							// 67
	MIME_TOKEN_JUNE,						// 68
	MIME_TOKEN_JULY,						// 69
	MIME_TOKEN_AUGUST,						// 70
	MIME_TOKEN_SEPTEMBER,					// 71
	MIME_TOKEN_OCTOBER,						// 72
	MIME_TOKEN_NOVEMBER,					// 73
	MIME_TOKEN_DECEMBER,					// 74
	MIME_TOKEN_MALICIOUS_DATE,				// 75 (Malicious Date functionality has been removed as of release 12g, July 2004
	MIME_TOKEN_TEXT,						// 76
	MIME_TOKEN_PLAIN,						// 77
	MIME_TOKEN_RICHTEXT,					// 78
	MIME_TOKEN_HTML,						// 79
	MIME_TOKEN_TAB_SEPARATED_VALUES,		// 80
	MIME_TOKEN_MULTIPART,					// 81
	MIME_TOKEN_MIXED,						// 82
	MIME_TOKEN_ALTERNATIVE,					// 83
	MIME_TOKEN_DIGEST,						// 84
	MIME_TOKEN_PARALLEL,					// 85
	MIME_TOKEN_APPLEDOUBLE,					// 86
	MIME_TOKEN_HEADER_SET,					// 87
	MIME_TOKEN_MESSAGE,						// 88
	MIME_TOKEN_RFC822,						// 89
	MIME_TOKEN_PARTIAL,						// 90
	MIME_TOKEN_EXTERNAL_BODY,				// 91
	MIME_TOKEN_NEWS,						// 92
	MIME_TOKEN_APPLICATION,					// 93
	MIME_TOKEN_OCTET_STREAM,				// 94
	MIME_TOKEN_POSTSCRIPT,					// 95
	MIME_TOKEN_ODA,							// 96
	MIME_TOKEN_ATOMICMAIL,					// 97
	MIME_TOKEN_ANDREW_INSET,				// 98
	MIME_TOKEN_SLATE,						// 99
	MIME_TOKEN_WITA,						// 100
	MIME_TOKEN_DEC_DX,						// 101
	MIME_TOKEN_DCA_RFT,						// 102
	MIME_TOKEN_ACTIVEMESSAGE,				// 103
	MIME_TOKEN_RTF,							// 104
	MIME_TOKEN_APPLEFILE,					// 105
	MIME_TOKEN_MAC_BINHEX40,				// 106
	MIME_TOKEN_NEWS_MESSAGE_ID,				// 107
	MIME_TOKEN_NEWS_TRANSMISSION,			// 108
	MIME_TOKEN_WORDPERFECT51,				// 109
	MIME_TOKEN_PDF,							// 110
	MIME_TOKEN_ZIP,							// 111
	MIME_TOKEN_MACWRITEII,					// 112
	MIME_TOKEN_MSWORD,						// 113
	MIME_TOKEN_REMOTE_PRINTING,				// 114
	MIME_TOKEN_IMAGE,						// 115
	MIME_TOKEN_JPEG,						// 116
	MIME_TOKEN_GIF,							// 117
	MIME_TOKEN_IEF,							// 118
	MIME_TOKEN_TIFF,						// 119
	MIME_TOKEN_AUDIO,						// 120
	MIME_TOKEN_BASIC,						// 121
	MIME_TOKEN_VIDEO,						// 122
	MIME_TOKEN_MPEG,						// 123
	MIME_TOKEN_QUICKTIME,					// 124
	MIME_TOKEN_BOUNDARY,					// 125
	MIME_TOKEN_NAME,						// 126
	MIME_TOKEN_CHARSET,						// 127
	MIME_TOKEN_ACCESS_TYPE,					// 128
	MIME_TOKEN_URL,							// 129
	MIME_TOKEN_ENRICHED,					// 130
	MIME_TOKEN_ORGANIZATION,				// 131
	MIME_TOKEN_AUTHOR,						// 132
	MIME_TOKEN_PRIORITY,					// 133
	MIME_TOKEN_DELIVERED_TO,				// 134
	MIME_TOKEN_CONTENT_LOCATION,			// 135
	MIME_TOKEN_THREAD_TOPIC,				// 136
	MIME_TOKEN_THREAD_INDEX,				// 137
	MIME_TOKEN_MAILER,						// 138
	MIME_TOKEN_CONTENT_LENGTH,				// 139
	MIME_TOKEN_END_OF_NAME_MARKER,			// 140
	MIME_TOKEN_REPORT_TYPE,					// 141
	MIME_TOKEN_ENCRYPTION,					// 142
	MIME_TOKEN_ENCRYPTION_FLAGS,			// 143
    MIME_TOKEN_HELO,                        // 144
    MIME_TOKEN_DATA,                        // 145
	MIME_TOKEN_UT,							// 146
	MIME_TOKEN_GMT,							// 147
	MIME_TOKEN_EST,							// 148
	MIME_TOKEN_EDT,							// 149
	MIME_TOKEN_CST,							// 150
	MIME_TOKEN_CDT,							// 151
	MIME_TOKEN_MST,							// 152
	MIME_TOKEN_MDT,							// 153
	MIME_TOKEN_PST,							// 154
	MIME_TOKEN_PDT,							// 155
	MIME_TOKEN_ACCEPT_LANGUAGE,				// 156
	MIME_TOKEN_MSMAIL_PRIORITY,				// 157
	MIME_TOKEN_USER_AGENT,					// 158
	MIME_TOKEN_APPARENTLY_TO,				// 159
	MIME_TOKEN_EGROUPS_RETURN,				// 160
	MIME_TOKEN_LIST_UNSUBSCRIBE,			// 161
	MIME_TOKEN_LIST_SUBSCRIBE,				// 162
	MIME_TOKEN_AUTO_SUBMITTED,				// 163
	MIME_TOKEN_ORIGINATING_IP,				// 164
	MIME_TOKEN_MAILER_VERSION,				// 165
	MIME_TOKEN_LIST_ID,						// 166
	MIME_TOKEN_LIST_POST,					// 167
	MIME_TOKEN_LIST_HELP,					// 168
	MIME_TOKEN_ERRORS_TO,					// 169
	MIME_TOKEN_MAILING_LIST,				// 170
	MIME_TOKEN_MS_HAS_ATTACH,				// 171
	MIME_TOKEN_MS_TNEF_CORRELATOR,			// 172
	MIME_TOKEN_MIME_AUTOCONVERTED,			// 173
	MIME_TOKEN_CONTENT_CLASS,				// 174
	MIME_TOKEN_PRECEDENCE,					// 175
	MIME_TOKEN_ORIGINALARRIVALTIME,			// 176
	MIME_TOKEN_MIMEOLE,						// 177
	MIME_TOKEN_LIST_ARCHIVE,				// 178
	MIME_TOKEN_SIGNED_DATA,					// 179
	MIME_TOKEN_X_PKSC7_MIME,				// 180
// End of parser tokens
	MIME_TOKEN_LAST							// 181
};


// ASCII constants. This code must be able to run on machines whose
// native character set is not ASCII. Even on such machines, though, the data
// that we are parsing is always ASCII. In order to make such a thing work, we
// must treat the message data as binary rather than text. All comparisons are
// against binary constants that we know to be in ASCII, defined here. For case
// insensitive comparisons we use both upper and lowercase versions of the
// constant along with a special function called binary_stricmp.

#define STR_resent_			"\x72\x65\x73\x65\x6E\x74\x2D"
#define STR_RESENT_			"\x52\x45\x53\x45\x4E\x54\x2D"
#define STR_content_type	"\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x74\x79\x70\x65"
#define STR_CONTENT_TYPE	"\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x54\x59\x50\x45"
#define STR_content_disposition "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x64\x69\x73\x70\x6F\x73\x69\x74\x69\x6F\x6E"
#define STR_CONTENT_DISPOSITION "\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x44\x49\x53\x50\x4F\x53\x49\x54\x49\x4F\x4E"
#define STR_content_transfer_encoding \
  "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x74\x72\x61\x6E\x73\x66\x65\x72\x2D\x65\x6E\x63\x6F\x64\x69\x6E\x67"
#define STR_CONTENT_TRANSFER_ENCODING \
  "\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x54\x52\x41\x4E\x53\x46\x45\x52\x2D\x45\x4E\x43\x4F\x44\x49\x4E\x47"
#define STR_mime_version	"\x6D\x69\x6D\x65\x2D\x76\x65\x72\x73\x69\x6F\x6E"
#define STR_MIME_VERSION	"\x4D\x49\x4D\x45\x2D\x56\x45\x52\x53\x49\x4F\x4E"
#define STR_importance		"\x69\x6D\x70\x6F\x72\x74\x61\x6E\x63\x65"
#define STR_IMPORTANCE		"\x49\x4D\x50\x4F\x52\x54\x41\x4E\x43\x45"
#define STR_date_HDR		"\x64\x61\x74\x65"
#define STR_DATE_HDR		"\x44\x41\x54\x45"
#define STR_filename		"\x66\x69\x6C\x65\x6E\x61\x6D\x65"
#define STR_FILENAME		"\x46\x49\x4C\x45\x4E\x41\x4D\x45"
#define STR_filename0		"\x66\x69\x6C\x65\x6E\x61\x6D\x65\x2A\x30\x2A"
#define STR_FILENAME0		"\x46\x49\x4C\x45\x4E\x41\x4D\x45\x2A\x30\x2A"
#define STR_quoted_printable "\x71\x75\x6F\x74\x65\x64\x2D\x70\x72\x69\x6E\x74\x61\x62\x6C\x65"
#define STR_QUOTED_PRINTABLE "\x51\x55\x4F\x54\x45\x44\x2D\x50\x52\x49\x4E\x54\x41\x42\x4C\x45"
#define STR_base64			"\x62\x61\x73\x65\x36\x34"
#define STR_BASE64			"\x42\x41\x53\x45\x36\x34"
#define STR_8bit			"\x38\x62\x69\x74"
#define STR_8BIT			"\x38\x42\x49\x54"
#define STR_7bit			"\x37\x62\x69\x74"
#define STR_7BIT			"\x37\x42\x49\x54"
#define STR_uuencode		"\x75\x75\x65\x6E\x63\x6F\x64\x65"
#define STR_UUENCODE		"\x55\x55\x45\x4E\x43\x4F\x44\x45"
#define STR_uue				"\x75\x75\x65"
#define STR_UUE				"\x55\x55\x45"
#define STR_binary			"\x62\x69\x6E\x61\x72\x79"
#define STR_BINARY			"\x42\x49\x4E\x41\x52\x59"
#define STR_APPLICATION		"\x41\x50\x50\x4C\x49\x43\x41\x54\x49\x4F\x4E"
#define STR_application		"\x61\x70\x70\x6C\x69\x63\x61\x74\x69\x6F\x6E"
#define STR_TEXT			"\x54\x45\x58\x54"
#define STR_text			"\x74\x65\x78\x74"
#define STR_IMAGE			"\x49\x4D\x41\x47\x45"
#define STR_image			"\x69\x6D\x61\x67\x65"
#define STR_name			"\x6E\x61\x6D\x65"
#define STR_NAME			"\x4E\x41\x4D\x45"
#define STR_MULTIPART		"\x4D\x55\x4C\x54\x49\x50\x41\x52\x54"
#define STR_multipart		"\x6D\x75\x6C\x74\x69\x70\x61\x72\x74"
#define STR_boundary		"\x62\x6F\x75\x6E\x64\x61\x72\x79"
#define STR_BOUNDARY		"\x42\x4F\x55\x4E\x44\x41\x52\x59"
#define STR_MESSAGE			"\x4D\x45\x53\x53\x41\x47\x45"
#define STR_message			"\x6D\x65\x73\x73\x61\x67\x65"
#define STR_RFC822			"\x52\x46\x43\x38\x32\x32"
#define STR_rfc822			"\x72\x66\x63\x38\x32\x32"
#define STR_TEXT			"\x54\x45\x58\x54"
#define STR_text			"\x74\x65\x78\x74"
#define STR_PLAIN			"\x50\x4C\x41\x49\x4E"
#define STR_plain			"\x70\x6C\x61\x69\x6E"
#define STR_HTML			"\x48\x54\x4D\x4C"
#define STR_html			"\x68\x74\x6D\x6C"
#define STR_mon				"\x6D\x6F\x6E"
#define STR_MON				"\x4D\x4F\x4E"
#define STR_tue				"\x74\x75\x65"
#define STR_TUE				"\x54\x55\x45"
#define STR_wed				"\x77\x65\x64"
#define STR_WED				"\x57\x45\x44"
#define STR_thu				"\x74\x68\x75"
#define STR_THU				"\x54\x48\x55"
#define STR_fri				"\x66\x72\x69"
#define STR_FRI				"\x46\x52\x49"
#define STR_sat				"\x73\x61\x74"
#define STR_SAT				"\x53\x41\x54"
#define STR_sun				"\x73\x75\x6E"
#define STR_SUN				"\x53\x55\x4E"
#define STR_jan				"\x6A\x61\x6E"
#define STR_JAN				"\x4A\x41\x4E"
#define STR_feb				"\x66\x65\x62"
#define STR_FEB				"\x46\x45\x42"
#define STR_mar				"\x6D\x61\x72"
#define STR_MAR				"\x4D\x41\x52"
#define STR_apr				"\x61\x70\x72"
#define STR_APR				"\x41\x50\x52"
#define STR_may				"\x6D\x61\x79"
#define STR_MAY				"\x4D\x41\x59"
#define STR_jun				"\x6A\x75\x6E"
#define STR_JUN				"\x4A\x55\x4E"
#define STR_jul				"\x6A\x75\x6C"
#define STR_JUL				"\x4A\x55\x4C"
#define STR_aug				"\x61\x75\x67"
#define STR_AUG				"\x41\x55\x47"
#define STR_sep				"\x73\x65\x70"
#define STR_SEP				"\x53\x45\x50"
#define STR_oct				"\x6F\x63\x74"
#define STR_OCT				"\x4F\x43\x54"
#define STR_nov				"\x6E\x6F\x76"
#define STR_NOV				"\x4E\x4F\x56"
#define STR_dec				"\x64\x65\x63"
#define STR_DEC				"\x44\x45\x43"
#define STR_return_path		"\x72\x65\x74\x75\x72\x6E\x2D\x70\x61\x74\x68"
#define STR_RETURN_PATH		"\x52\x45\x54\x55\x52\x4E\x2D\x50\x41\x54\x48"
#define STR_received		"\x72\x65\x63\x65\x69\x76\x65\x64"
#define STR_RECEIVED		"\x52\x45\x43\x45\x49\x56\x45\x44"
#define STR_reply_to		"\x72\x65\x70\x6C\x79\x2D\x74\x6F"
#define STR_REPLY_TO		"\x52\x45\x50\x4C\x59\x2D\x54\x4F"
#define STR_from			"\x66\x72\x6F\x6D"
#define STR_FROM			"\x46\x52\x4F\x4D"
#define STR_sender			"\x73\x65\x6E\x64\x65\x72"
#define STR_SENDER			"\x53\x45\x4E\x44\x45\x52"
#define STR_date			"\x64\x61\x74\x65"
#define STR_DATE			"\x44\x41\x54\x45"
#define STR_to				"\x74\x6F"
#define STR_TO				"\x54\x4F"
#define STR_cc				"\x63\x63"
#define STR_CC				"\x43\x43"
#define STR_bcc				"\x62\x63\x63"
#define STR_BCC				"\x42\x43\x43"
#define STR_message_id		"\x6D\x65\x73\x73\x61\x67\x65\x2D\x69\x64"
#define STR_MESSAGE_ID		"\x4D\x45\x53\x53\x41\x47\x45\x2D\x49\x44"
#define STR_in_reply_to		"\x69\x6E\x2D\x72\x65\x70\x6C\x79\x2D\x74\x6F"
#define STR_IN_REPLY_TO		"\x49\x4E\x2D\x52\x45\x50\x4C\x59\x2D\x54\x4F"
#define STR_references		"\x72\x65\x66\x65\x72\x65\x6E\x63\x65\x73"
#define STR_REFERENCES		"\x52\x45\x46\x45\x52\x45\x4E\x43\x45\x53"
#define STR_keywords		"\x6B\x65\x79\x77\x6F\x72\x64\x73"
#define STR_KEYWORDS		"\x4B\x45\x59\x57\x4F\x52\x44\x53"
#define STR_subject			"\x73\x75\x62\x6A\x65\x63\x74"
#define STR_SUBJECT			"\x53\x55\x42\x4A\x45\x43\x54"
#define STR_comments		"\x63\x6F\x6D\x6D\x65\x6E\x74\x73"
#define STR_COMMENTS		"\x43\x4F\x4D\x4D\x45\x4E\x54\x53"
#define STR_encrypted		"\x65\x6E\x63\x72\x79\x70\x74\x65\x64"
#define STR_ENCRYPTED		"\x45\x4E\x43\x52\x59\x50\x54\x45\x44"
#define STR_content_description "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x64\x65\x73\x63\x72\x69\x70\x74\x69\x6F\x6E"
#define STR_CONTENT_DESCRIPTION "\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x44\x45\x53\x43\x52\x49\x50\x54\x49\x4F\x4E"
#define STR_content_id		"\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x69\x64"
#define STR_CONTENT_ID		"\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x49\x44"
#define STR_by				"\x62\x79"
#define STR_BY				"\x42\x59"
#define STR_via				"\x76\x69\x61"
#define STR_VIA				"\x56\x49\x41"
#define STR_with			"\x77\x69\x74\x68"
#define STR_WITH			"\x57\x49\x54\x48"
#define STR_id				"\x69\x64"
#define STR_ID				"\x49\x44"
#define STR_for				"\x66\x6F\x72"
#define STR_FOR				"\x46\x4F\x52"
#define STR_attachment		"\x61\x74\x74\x61\x63\x68\x6D\x65\x6E\x74"
#define STR_ATTACHMENT		"\x41\x54\x54\x41\x43\x48\x4D\x45\x4E\x54"
#define STR_audio			"\x61\x75\x64\x69\x6F"
#define STR_AUDIO			"\x41\x55\x44\x49\x4F"
#define STR_video			"\x76\x69\x64\x65\x6F"
#define STR_VIDEO			"\x56\x49\x44\x45\x4F"
#define STR_charset			"\x63\x68\x61\x72\x73\x65\x74"
#define STR_CHARSET			"\x43\x48\x41\x52\x53\x45\x54"
#define STR_richtext		"\x72\x69\x63\x68\x74\x65\x78\x74"
#define STR_RICHTEXT		"\x52\x49\x43\x48\x54\x45\x58\x54"
#define STR_tab_separated_values "\x74\x61\x62\x2D\x73\x65\x70\x61\x72\x61\x74\x65\x64\x2D\x76\x61\x6C\x75\x65\x73"
#define STR_TAB_SEPARATED_VALUES "\x54\x41\x42\x2D\x53\x45\x50\x41\x52\x41\x54\x45\x44\x2D\x56\x41\x4C\x55\x45\x53"
#define STR_mixed			"\x6D\x69\x78\x65\x64"
#define STR_MIXED			"\x4D\x49\x58\x45\x44"
#define STR_alternative		"\x61\x6C\x74\x65\x72\x6E\x61\x74\x69\x76\x65"
#define STR_ALTERNATIVE		"\x41\x4C\x54\x45\x52\x4E\x41\x54\x49\x56\x45"
#define STR_digest			"\x64\x69\x67\x65\x73\x74"
#define STR_DIGEST			"\x44\x49\x47\x45\x53\x54"
#define STR_parallel		"\x70\x61\x72\x61\x6C\x6C\x65\x6C"
#define STR_PARALLEL		"\x50\x41\x52\x41\x4C\x4C\x45\x4C"
#define STR_appledouble		"\x61\x70\x70\x6C\x65\x64\x6F\x75\x62\x6C\x65"
#define STR_APPLEDOUBLE		"\x41\x50\x50\x4C\x45\x44\x4F\x55\x42\x4C\x45"
#define STR_header_set		"\x68\x65\x61\x64\x65\x72\x2D\x73\x65\x74"
#define STR_HEADER_SET		"\x48\x45\x41\x44\x45\x52\x2D\x53\x45\x54"
#define STR_partial			"\x70\x61\x72\x74\x69\x61\x6C"
#define STR_PARTIAL			"\x50\x41\x52\x54\x49\x41\x4C"
#define STR_external_body	"\x65\x78\x74\x65\x72\x6E\x61\x6C\x2D\x62\x6F\x64\x79"
#define STR_EXTERNAL_BODY	"\x45\x58\x54\x45\x52\x4E\x41\x4C\x2D\x42\x4F\x44\x59"
#define STR_news			"\x6E\x65\x77\x73"
#define STR_NEWS			"\x4E\x45\x57\x53"
#define STR_octet_stream	"\x6F\x63\x74\x65\x74\x2D\x73\x74\x72\x65\x61\x6D"
#define STR_OCTET_STREAM	"\x4F\x43\x54\x45\x54\x2D\x53\x54\x52\x45\x41\x4D"
#define STR_postscript		"\x70\x6F\x73\x74\x73\x63\x72\x69\x70\x74"
#define STR_POSTSCRIPT		"\x50\x4F\x53\x54\x53\x43\x52\x49\x50\x54"
#define STR_oda				"\x6F\x64\x61"
#define STR_ODA				"\x4F\x44\x41"
#define STR_atomicmail		"\x61\x74\x6F\x6D\x69\x63\x6D\x61\x69\x6C"
#define STR_ATOMICMAIL		"\x41\x54\x4F\x4D\x49\x43\x4D\x41\x49\x4C"
#define STR_andrew_inset	"\x61\x6E\x64\x72\x65\x77\x2D\x69\x6E\x73\x65\x74"
#define STR_ANDREW_INSET	"\x41\x4E\x44\x52\x45\x57\x2D\x49\x4E\x53\x45\x54"
#define STR_slate			"\x73\x6C\x61\x74\x65"
#define STR_SLATE			"\x53\x4C\x41\x54\x45"
#define STR_wita			"\x77\x69\x74\x61"
#define STR_WITA			"\x57\x49\x54\x41"
#define STR_dec_dx			"\x64\x65\x63\x2D\x64\x78"
#define STR_DEC_DX			"\x44\x45\x43\x2D\x44\x58"
#define STR_dca_rft			"\x64\x63\x61\x2D\x72\x66\x74"
#define STR_DCA_RFT			"\x44\x43\x41\x2D\x52\x46\x54"
#define STR_activemessage	"\x61\x63\x74\x69\x76\x65\x6D\x65\x73\x73\x61\x67\x65"
#define STR_ACTIVEMESSAGE	"\x41\x43\x54\x49\x56\x45\x4D\x45\x53\x53\x41\x47\x45"
#define STR_rtf				"\x72\x74\x66"
#define STR_RTF				"\x52\x54\x46"
#define STR_applefile		"\x61\x70\x70\x6C\x65\x66\x69\x6C\x65"
#define STR_APPLEFILE		"\x41\x50\x50\x4C\x45\x46\x49\x4C\x45"
#define STR_mac_binhex40	"\x6D\x61\x63\x2D\x62\x69\x6E\x68\x65\x78\x34\x30"
#define STR_MAC_BINHEX40	"\x4D\x41\x43\x2D\x42\x49\x4E\x48\x45\x58\x34\x30"
#define STR_news_message_id "\x6E\x65\x77\x73\x2D\x6D\x65\x73\x73\x61\x67\x65\x2D\x69\x64"
#define STR_NEWS_MESSAGE_ID "\x4E\x45\x57\x53\x2D\x4D\x45\x53\x53\x41\x47\x45\x2D\x49\x44"
#define STR_news_transmission "\x6E\x65\x77\x73\x2D\x74\x72\x61\x6E\x73\x6D\x69\x73\x73\x69\x6F\x6E"
#define STR_NEWS_TRANSMISSION "\x4E\x45\x57\x53\x2D\x54\x52\x41\x4E\x53\x4D\x49\x53\x53\x49\x4F\x4E"
#define STR_wordperfect5_1	"\x77\x6F\x72\x64\x70\x65\x72\x66\x65\x63\x74\x35\x2E\x31"
#define STR_WORDPERFECT5_1	"\x57\x4F\x52\x44\x50\x45\x52\x46\x45\x43\x54\x35\x2E\x31"
#define STR_pdf				"\x70\x64\x66"
#define STR_PDF				"\x50\x44\x46"
#define STR_zip				"\x7A\x69\x70"
#define STR_ZIP				"\x5A\x49\x50"
#define STR_macwriteii		"\x6D\x61\x63\x77\x72\x69\x74\x65\x69\x69"
#define STR_MACWRITEII		"\x4D\x41\x43\x57\x52\x49\x54\x45\x49\x49"
#define STR_msword			"\x6D\x73\x77\x6F\x72\x64"
#define STR_MSWORD			"\x4D\x53\x57\x4F\x52\x44"
#define STR_remote_printing "\x72\x65\x6D\x6F\x74\x65\x2D\x70\x72\x69\x6E\x74\x69\x6E\x67"
#define STR_REMOTE_PRINTING "\x52\x45\x4D\x4F\x54\x45\x2D\x50\x52\x49\x4E\x54\x49\x4E\x47"
#define STR_jpeg			"\x6A\x70\x65\x67"
#define STR_JPEG			"\x4A\x50\x45\x47"
#define STR_gif				"\x67\x69\x66"
#define STR_GIF				"\x47\x49\x46"
#define STR_ief				"\x69\x65\x66"
#define STR_IEF				"\x49\x45\x46"
#define STR_tiff			"\x74\x69\x66\x66"
#define STR_TIFF			"\x54\x49\x46\x46"
#define STR_basic			"\x62\x61\x73\x69\x63"
#define STR_BASIC			"\x42\x41\x53\x49\x43"
#define STR_mpeg			"\x6D\x70\x65\x67"
#define STR_MPEG			"\x4D\x50\x45\x47"
#define STR_quicktime		"\x71\x75\x69\x63\x6B\x74\x69\x6D\x65"
#define STR_QUICKTIME		"\x51\x55\x49\x43\x4B\x54\x49\x4D\x45"
#define STR_access_type		"\x61\x63\x63\x65\x73\x73\x2D\x74\x79\x70\x65"
#define STR_ACCESS_TYPE		"\x41\x43\x43\x45\x53\x53\x2D\x54\x59\x50\x45"
#define STR_url				"\x75\x72\x6C"
#define STR_URL				"\x55\x52\x4C"
#define STR_enriched		"\x65\x6E\x72\x69\x63\x68\x65\x64"
#define STR_ENRICHED		"\x45\x4E\x52\x49\x43\x48\x45\x44"
#define STR_report_type		"\x72\x65\x70\x6F\x72\x74\x2D\x74\x79\x70\x65"
#define STR_REPORT_TYPE		"\x52\x45\x50\x4F\x52\x54\x2D\x54\x59\x50\x45"
#define STR_organization	"\x6F\x72\x67\x61\x6E\x69\x7A\x61\x74\x69\x6F\x6E"
#define STR_ORGANIZATION	"\x4F\x52\x47\x41\x4E\x49\x5A\x41\x54\x49\x4F\x4E"
#define STR_author			"\x61\x75\x74\x68\x6F\x72"
#define STR_AUTHOR			"\x41\x55\x54\x48\x4F\x52"
#define STR_priority		"\x70\x72\x69\x6F\x72\x69\x74\x79"
#define STR_PRIORITY		"\x50\x52\x49\x4F\x52\x49\x54\x59"
#define STR_delivered_to	"\x64\x65\x6C\x69\x76\x65\x72\x65\x64\x2D\x74\x6F"
#define STR_DELIVERED_TO	"\x44\x45\x4C\x49\x56\x45\x52\x45\x44\x2D\x54\x4F"
#define STR_content_location "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x6C\x6F\x63\x61\x74\x69\x6F\x6E"
#define STR_CONTENT_LOCATION "\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x4C\x4F\x43\x41\x54\x49\x4F\x4E"
#define STR_thread_topic	"\x74\x68\x72\x65\x61\x64\x2D\x74\x6F\x70\x69\x63"
#define STR_THREAD_TOPIC	"\x54\x48\x52\x45\x41\x44\x2D\x54\x4F\x50\x49\x43"
#define STR_thread_index	"\x74\x68\x72\x65\x61\x64\x2D\x69\x6E\x64\x65\x78"
#define STR_THREAD_INDEX	"\x54\x48\x52\x45\x41\x44\x2D\x49\x4E\x44\x45\x58"
#define STR_mailer			"\x6D\x61\x69\x6C\x65\x72"
#define STR_MAILER			"\x4D\x41\x49\x4C\x45\x52"
#define STR_content_length	"\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x6C\x65\x6E\x67\x74\x68"
#define STR_CONTENT_LENGTH	"\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x4C\x45\x4E\x47\x54\x48"
#define STR_encryption		"\x65\x6E\x63\x72\x79\x70\x74\x69\x6F\x6E"
#define STR_ENCRYPTION		"\x45\x4E\x43\x52\x59\x50\x54\x49\x4F\x4E"
#define STR_encryption_flags "\x65\x6E\x63\x72\x79\x70\x74\x69\x6F\x6E\x2D\x66\x6C\x61\x67\x73"
#define STR_ENCRYPTION_FLAGS "\x45\x4E\x43\x52\x59\x50\x54\x49\x4F\x4E\x2D\x46\x4C\x41\x47\x53"
#define STR_accept_language "\x61\x63\x63\x65\x70\x74\x2D\x6C\x61\x6E\x67\x75\x61\x67\x65"
#define STR_ACCEPT_LANGUAGE "\x41\x43\x43\x45\x50\x54\x2D\x4C\x41\x4E\x47\x55\x41\x47\x45"
#define STR_msmail_priority "\x6D\x73\x6D\x61\x69\x6C\x2D\x70\x72\x69\x6F\x72\x69\x74\x79"
#define STR_MSMAIL_PRIORITY "\x4D\x53\x4D\x41\x49\x4C\x2D\x50\x52\x49\x4F\x52\x49\x54\x59"
#define STR_user_agent		"\x75\x73\x65\x72\x2D\x61\x67\x65\x6E\x74"
#define STR_USER_AGENT		"\x55\x53\x45\x52\x2D\x41\x47\x45\x4E\x54"
#define STR_apparently_to	"\x61\x70\x70\x61\x72\x65\x6E\x74\x6C\x79\x2D\x74\x6F"
#define STR_APPARENTLY_TO	"\x41\x50\x50\x41\x52\x45\x4E\x54\x4C\x59\x2D\x54\x4F"
#define STR_egroups_return	"\x65\x67\x72\x6F\x75\x70\x73\x2D\x72\x65\x74\x75\x72\x6E"
#define STR_EGROUPS_RETURN	"\x45\x47\x52\x4F\x55\x50\x53\x2D\x52\x45\x54\x55\x52\x4E"
#define STR_list_unsubscribe "\x6C\x69\x73\x74\x2D\x75\x6E\x73\x75\x62\x73\x63\x72\x69\x62\x65"
#define STR_LIST_UNSUBSCRIBE "\x4C\x49\x53\x54\x2D\x55\x4E\x53\x55\x42\x53\x43\x52\x49\x42\x45"
#define STR_list_subscribe	"\x6C\x69\x73\x74\x2D\x73\x75\x62\x73\x63\x72\x69\x62\x65"
#define STR_LIST_SUBSCRIBE	"\x4C\x49\x53\x54\x2D\x53\x55\x42\x53\x43\x52\x49\x42\x45"
#define STR_auto_submitted	"\x61\x75\x74\x6F\x2D\x73\x75\x62\x6D\x69\x74\x74\x65\x64"
#define STR_AUTO_SUBMITTED	"\x41\x55\x54\x4F\x2D\x53\x55\x42\x4D\x49\x54\x54\x45\x44"
#define STR_originating_ip	"\x6F\x72\x69\x67\x69\x6E\x61\x74\x69\x6E\x67\x2D\x69\x70"
#define STR_ORIGINATING_IP	"\x4F\x52\x49\x47\x49\x4E\x41\x54\x49\x4E\x47\x2D\x49\x50"
#define STR_mailer_version	"\x6D\x61\x69\x6C\x65\x72\x2D\x76\x65\x72\x73\x69\x6F\x6E"
#define STR_MAILER_VERSION	"\x4D\x41\x49\x4C\x45\x52\x2D\x56\x45\x52\x53\x49\x4F\x4E"
#define STR_list_id			"\x6C\x69\x73\x74\x2D\x69\x64"
#define STR_LIST_ID			"\x4C\x49\x53\x54\x2D\x49\x44"
#define STR_list_post		"\x6C\x69\x73\x74\x2D\x70\x6F\x73\x74"
#define STR_LIST_POST		"\x4C\x49\x53\x54\x2D\x50\x4F\x53\x54"
#define STR_list_help		"\x6C\x69\x73\x74\x2D\x68\x65\x6C\x70"
#define STR_LIST_HELP		"\x4C\x49\x53\x54\x2D\x48\x45\x4C\x50"
#define STR_errors_to		"\x65\x72\x72\x6F\x72\x73\x2D\x74\x6F"
#define STR_ERRORS_TO		"\x45\x52\x52\x4F\x52\x53\x2D\x54\x4F"
#define STR_mailing_list	"\x6D\x61\x69\x6C\x69\x6E\x67\x2D\x6C\x69\x73\x74"
#define STR_MAILING_LIST	"\x4D\x41\x49\x4C\x49\x4E\x47\x2D\x4C\x49\x53\x54"
#define STR_ms_has_attach	"\x6D\x73\x2D\x68\x61\x73\x2D\x61\x74\x74\x61\x63\x68"
#define STR_MS_HAS_ATTACH	"\x4D\x53\x2D\x48\x41\x53\x2D\x41\x54\x54\x41\x43\x48"
#define STR_ms_tnef_correlator "\x6D\x73\x2D\x74\x6E\x65\x66\x2D\x63\x6F\x72\x72\x65\x6C\x61\x74\x6F\x72"
#define STR_MS_TNEF_CORRELATOR "\x4D\x53\x2D\x54\x4E\x45\x46\x2D\x43\x4F\x52\x52\x45\x4C\x41\x54\x4F\x52"
#define STR_mime_autoconverted "\x6D\x69\x6D\x65\x2D\x61\x75\x74\x6F\x63\x6F\x6E\x76\x65\x72\x74\x65\x64"
#define STR_MIME_AUTOCONVERTED "\x4D\x49\x4D\x45\x2D\x41\x55\x54\x4F\x43\x4F\x4E\x56\x45\x52\x54\x45\x44"
#define STR_content_class	"\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x63\x6C\x61\x73\x73"
#define STR_CONTENT_CLASS	"\x43\x4F\x4E\x54\x45\x4E\x54\x2D\x43\x4C\x41\x53\x53"
#define STR_precedence		"\x70\x72\x65\x63\x65\x64\x65\x6E\x63\x65"
#define STR_PRECEDENCE		"\x50\x52\x45\x43\x45\x44\x45\x4E\x43\x45"
#define STR_originalarrivaltime "\x6F\x72\x69\x67\x69\x6E\x61\x6C\x61\x72\x72\x69\x76\x61\x6C\x74\x69\x6D\x65"
#define STR_ORIGINALARRIVALTIME "\x4F\x52\x49\x47\x49\x4E\x41\x4C\x41\x52\x52\x49\x56\x41\x4C\x54\x49\x4D\x45"
#define STR_mimeole			"\x6D\x69\x6D\x65\x6F\x6C\x65"
#define STR_MIMEOLE			"\x4D\x49\x4D\x45\x4F\x4C\x45"
#define STR_list_archive	"\x6C\x69\x73\x74\x2D\x61\x72\x63\x68\x69\x76\x65"
#define STR_LIST_ARCHIVE	"\x4C\x49\x53\x54\x2D\x41\x52\x43\x48\x49\x56\x45"// *** Fix for W32.Chir@mm virus ***
#define STR_helo            "\x68\x65\x6C\x6F"
#define STR_HELO            "\x48\x45\x4C\x4F"
#define STR_data            "\x64\x61\x74\x61"
#define STR_DATA            "\x44\x41\x54\x41"
#define STR_ASN1_PARTIAL_ID  "\x2A\x86\x48\x86\xF7"
#define STR_CONTENT_TYPE_signed_data "\x73\x69\x67\x6E\x65\x64\x2D\x64\x61\x74\x61"
#define STR_CONTENT_TYPE_SIGNED_DATA "\x53\x49\x47\x4E\x45\x44\x2D\x44\x41\x54\x41"
#define STR_x_pkcs7_mime	"\x78\x2D\x70\x6B\x63\x73\x37\x2D\x6D\x69\x6D\x65"
#define STR_X_PKCS7_MIME	"\x58\x2D\x50\x4B\x43\x53\x37\x2D\x4D\x49\x4D\x45"

// *********************************

#define STR_ut				"\x75\x74"
#define STR_UT				"\x55\x54"
#define STR_gmt				"\x67\x6D\x74"
#define STR_GMT				"\x47\x4D\x54"
#define STR_est				"\x65\x73\x74"
#define STR_EST				"\x45\x53\x54"
#define STR_edt				"\x65\x64\x74"
#define STR_EDT				"\x45\x44\x54"
#define STR_cst				"\x63\x73\x74"
#define STR_CST				"\x43\x53\x54"
#define STR_cdt				"\x63\x64\x74"
#define STR_CDT				"\x43\x44\x54"
#define STR_mst				"\x6D\x73\x74"
#define STR_MST				"\x4D\x53\x54"
#define STR_mdt				"\x6D\x64\x74"
#define STR_MDT				"\x4D\x44\x54"
#define STR_pst				"\x70\x73\x74"
#define STR_PST				"\x50\x53\x54"
#define STR_pdt				"\x70\x64\x74"
#define STR_PDT				"\x50\x44\x54"
#endif // _IMIME_H_
