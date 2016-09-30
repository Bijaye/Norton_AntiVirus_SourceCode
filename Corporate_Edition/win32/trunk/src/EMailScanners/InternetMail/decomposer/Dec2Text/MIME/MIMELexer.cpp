// MIMELexer.cpp : Lexer for the MIME engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "dec_assert.h"
#include "MIME.h"


static int g_iMIMELexerFirstState[256] =
{
	// Lexer state to transition to			// Byte
	MIME_LEXER_STATE_ATOM1,					// 0
	MIME_LEXER_STATE_ATOM1,					// 1
	MIME_LEXER_STATE_ATOM1,					// 2
	MIME_LEXER_STATE_ATOM1,					// 3
	MIME_LEXER_STATE_ATOM1,					// 4
	MIME_LEXER_STATE_ATOM1,					// 5
	MIME_LEXER_STATE_ATOM1,					// 6
	MIME_LEXER_STATE_ATOM1,					// 7
	MIME_LEXER_STATE_ATOM1,					// 8
	MIME_LEXER_STATE_LWSP1,					// 9 tab
	MIME_LEXER_STATE_LF1,					// 10 line-feed
	MIME_LEXER_STATE_ATOM1,					// 11
	MIME_LEXER_STATE_ATOM1,					// 12
	MIME_LEXER_STATE_CRLF1,					// 13 carriage-return
	MIME_LEXER_STATE_ATOM1,					// 14
	MIME_LEXER_STATE_ATOM1,					// 15
	MIME_LEXER_STATE_ATOM1,					// 16
	MIME_LEXER_STATE_ATOM1,					// 17
	MIME_LEXER_STATE_ATOM1,					// 18
	MIME_LEXER_STATE_ATOM1,					// 19
	MIME_LEXER_STATE_ATOM1,					// 20
	MIME_LEXER_STATE_ATOM1,					// 21
	MIME_LEXER_STATE_ATOM1,					// 22
	MIME_LEXER_STATE_ATOM1,					// 23
	MIME_LEXER_STATE_ATOM1,					// 24
	MIME_LEXER_STATE_ATOM1,					// 25
	MIME_LEXER_STATE_ATOM1,					// 26
	MIME_LEXER_STATE_ATOM1,					// 27
	MIME_LEXER_STATE_ATOM1,					// 28
	MIME_LEXER_STATE_ATOM1,					// 29
	MIME_LEXER_STATE_ATOM1,					// 30
	MIME_LEXER_STATE_ATOM1,					// 31
	MIME_LEXER_STATE_LWSP1,					// 32 space
	MIME_LEXER_STATE_ATOM1,					// 33 !
	MIME_LEXER_STATE_QUOTEDSTRING1,			// 34 "
	MIME_LEXER_STATE_ATOM1,					// 35 #
	MIME_LEXER_STATE_ATOM1,					// 36 $
	MIME_LEXER_STATE_ATOM1,					// 37 %
	MIME_LEXER_STATE_ATOM1,					// 38 &
	MIME_LEXER_STATE_ATOM1,					// 39 '
	MIME_LEXER_STATE_COMMENT1,				// 40 (
	MIME_LEXER_STATE_SPECIAL1,				// 41 )
	MIME_LEXER_STATE_ATOM1,					// 42 *
	MIME_LEXER_STATE_ATOM1,					// 43 +
	MIME_LEXER_STATE_SPECIAL1,				// 44 ,
	MIME_LEXER_STATE_ATOM1,					// 45 -
	MIME_LEXER_STATE_SPECIAL1,				// 46 .
	MIME_LEXER_STATE_SPECIAL1,				// 47 /
	MIME_LEXER_STATE_ATOM1,					// 48 0
	MIME_LEXER_STATE_ATOM1,					// 49 1
	MIME_LEXER_STATE_ATOM1,					// 50 2
	MIME_LEXER_STATE_ATOM1,					// 51 3
	MIME_LEXER_STATE_ATOM1,					// 52 4
	MIME_LEXER_STATE_ATOM1,					// 53 5
	MIME_LEXER_STATE_ATOM1,					// 54 6
	MIME_LEXER_STATE_ATOM1,					// 55 7
	MIME_LEXER_STATE_ATOM1,					// 56 8
	MIME_LEXER_STATE_ATOM1,					// 57 9
	MIME_LEXER_STATE_SPECIAL1,				// 58 :
	MIME_LEXER_STATE_SPECIAL1,				// 59 ;
	MIME_LEXER_STATE_ROUTEADDR1,			// 60 <
	MIME_LEXER_STATE_SPECIAL1,				// 61 =
	MIME_LEXER_STATE_SPECIAL1,				// 62 >
	MIME_LEXER_STATE_SPECIAL1,				// 63 ?
	MIME_LEXER_STATE_SPECIAL1,				// 64 @
	MIME_LEXER_STATE_ATOM1,					// 65 A
	MIME_LEXER_STATE_ATOM1,					// 66 B
	MIME_LEXER_STATE_ATOM1,					// 67 C
	MIME_LEXER_STATE_ATOM1,					// 68 D
	MIME_LEXER_STATE_ATOM1,					// 69 E
	MIME_LEXER_STATE_ATOM1,					// 70 F
	MIME_LEXER_STATE_ATOM1,					// 71 G
	MIME_LEXER_STATE_ATOM1,					// 72 H
	MIME_LEXER_STATE_ATOM1,					// 73 I
	MIME_LEXER_STATE_ATOM1,					// 74 J
	MIME_LEXER_STATE_ATOM1,					// 75 K
	MIME_LEXER_STATE_ATOM1,					// 76 L
	MIME_LEXER_STATE_ATOM1,					// 77 M
	MIME_LEXER_STATE_ATOM1,					// 78 N
	MIME_LEXER_STATE_ATOM1,					// 79 O
	MIME_LEXER_STATE_ATOM1,					// 80 P
	MIME_LEXER_STATE_ATOM1,					// 81 Q
	MIME_LEXER_STATE_ATOM1,					// 82 R
	MIME_LEXER_STATE_ATOM1,					// 83 S
	MIME_LEXER_STATE_ATOM1,					// 84 T
	MIME_LEXER_STATE_ATOM1,					// 85 U
	MIME_LEXER_STATE_ATOM1,					// 86 V
	MIME_LEXER_STATE_ATOM1,					// 87 W
	MIME_LEXER_STATE_ATOM1,					// 88 X
	MIME_LEXER_STATE_ATOM1,					// 89 Y
	MIME_LEXER_STATE_ATOM1,					// 90 Z
	MIME_LEXER_STATE_DOMAINLITERAL1,		// 91 [
	MIME_LEXER_STATE_SPECIAL1,				// 92 backslash
	MIME_LEXER_STATE_SPECIAL1,				// 93 ]
	MIME_LEXER_STATE_ATOM1,					// 94 ^
	MIME_LEXER_STATE_ATOM1,					// 95 _
	MIME_LEXER_STATE_ATOM1,					// 96 `
	MIME_LEXER_STATE_ATOM1,					// 97 a
	MIME_LEXER_STATE_ATOM1,					// 98 b
	MIME_LEXER_STATE_ATOM1,					// 99 c
	MIME_LEXER_STATE_ATOM1,					// 100 d
	MIME_LEXER_STATE_ATOM1,					// 101 e
	MIME_LEXER_STATE_ATOM1,					// 102 f
	MIME_LEXER_STATE_ATOM1,					// 103 g
	MIME_LEXER_STATE_ATOM1,					// 104 h
	MIME_LEXER_STATE_ATOM1,					// 105 i
	MIME_LEXER_STATE_ATOM1,					// 106 j
	MIME_LEXER_STATE_ATOM1,					// 107 k
	MIME_LEXER_STATE_ATOM1,					// 108 l
	MIME_LEXER_STATE_ATOM1,					// 109 m
	MIME_LEXER_STATE_ATOM1,					// 110 n
	MIME_LEXER_STATE_ATOM1,					// 111 o
	MIME_LEXER_STATE_ATOM1,					// 112 p
	MIME_LEXER_STATE_ATOM1,					// 113 q
	MIME_LEXER_STATE_ATOM1,					// 114 r
	MIME_LEXER_STATE_ATOM1,					// 115 s
	MIME_LEXER_STATE_ATOM1,					// 116 t
	MIME_LEXER_STATE_ATOM1,					// 117 u
	MIME_LEXER_STATE_ATOM1,					// 118 v
	MIME_LEXER_STATE_ATOM1,					// 119 w
	MIME_LEXER_STATE_ATOM1,					// 120 x
	MIME_LEXER_STATE_ATOM1,					// 121 y
	MIME_LEXER_STATE_ATOM1,					// 122 z
	MIME_LEXER_STATE_ATOM1,					// 123 {
	MIME_LEXER_STATE_ATOM1,					// 124 |
	MIME_LEXER_STATE_ATOM1,					// 125 }
	MIME_LEXER_STATE_ATOM1,					// 126 ~
	MIME_LEXER_STATE_ATOM1,					// 127
	MIME_LEXER_STATE_ATOM1,					// 128
	MIME_LEXER_STATE_ATOM1,					// 129
	MIME_LEXER_STATE_ATOM1,					// 130
	MIME_LEXER_STATE_ATOM1,					// 131
	MIME_LEXER_STATE_ATOM1,					// 132
	MIME_LEXER_STATE_ATOM1,					// 133
	MIME_LEXER_STATE_ATOM1,					// 134
	MIME_LEXER_STATE_ATOM1,					// 135
	MIME_LEXER_STATE_ATOM1,					// 136
	MIME_LEXER_STATE_ATOM1,					// 137
	MIME_LEXER_STATE_ATOM1,					// 138
	MIME_LEXER_STATE_ATOM1,					// 139
	MIME_LEXER_STATE_ATOM1,					// 140
	MIME_LEXER_STATE_ATOM1,					// 141
	MIME_LEXER_STATE_ATOM1,					// 142
	MIME_LEXER_STATE_ATOM1,					// 143
	MIME_LEXER_STATE_ATOM1,					// 144
	MIME_LEXER_STATE_ATOM1,					// 145
	MIME_LEXER_STATE_ATOM1,					// 146
	MIME_LEXER_STATE_ATOM1,					// 147
	MIME_LEXER_STATE_ATOM1,					// 148
	MIME_LEXER_STATE_ATOM1,					// 149
	MIME_LEXER_STATE_ATOM1,					// 150
	MIME_LEXER_STATE_ATOM1,					// 151
	MIME_LEXER_STATE_ATOM1,					// 152
	MIME_LEXER_STATE_ATOM1,					// 153
	MIME_LEXER_STATE_ATOM1,					// 154
	MIME_LEXER_STATE_ATOM1,					// 155
	MIME_LEXER_STATE_ATOM1,					// 156
	MIME_LEXER_STATE_ATOM1,					// 157
	MIME_LEXER_STATE_ATOM1,					// 158
	MIME_LEXER_STATE_ATOM1,					// 159
	MIME_LEXER_STATE_ATOM1,					// 160
	MIME_LEXER_STATE_ATOM1,					// 161
	MIME_LEXER_STATE_ATOM1,					// 162
	MIME_LEXER_STATE_ATOM1,					// 163
	MIME_LEXER_STATE_ATOM1,					// 164
	MIME_LEXER_STATE_ATOM1,					// 165
	MIME_LEXER_STATE_ATOM1,					// 166
	MIME_LEXER_STATE_ATOM1,					// 167
	MIME_LEXER_STATE_ATOM1,					// 168
	MIME_LEXER_STATE_ATOM1,					// 169
	MIME_LEXER_STATE_ATOM1,					// 170
	MIME_LEXER_STATE_ATOM1,					// 171
	MIME_LEXER_STATE_ATOM1,					// 172
	MIME_LEXER_STATE_ATOM1,					// 173
	MIME_LEXER_STATE_ATOM1,					// 174
	MIME_LEXER_STATE_ATOM1,					// 175
	MIME_LEXER_STATE_ATOM1,					// 176
	MIME_LEXER_STATE_ATOM1,					// 177
	MIME_LEXER_STATE_ATOM1,					// 178
	MIME_LEXER_STATE_ATOM1,					// 179
	MIME_LEXER_STATE_ATOM1,					// 180
	MIME_LEXER_STATE_ATOM1,					// 181
	MIME_LEXER_STATE_ATOM1,					// 182
	MIME_LEXER_STATE_ATOM1,					// 183
	MIME_LEXER_STATE_ATOM1,					// 184
	MIME_LEXER_STATE_ATOM1,					// 185
	MIME_LEXER_STATE_ATOM1,					// 186
	MIME_LEXER_STATE_ATOM1,					// 187
	MIME_LEXER_STATE_ATOM1,					// 188
	MIME_LEXER_STATE_ATOM1,					// 189
	MIME_LEXER_STATE_ATOM1,					// 190
	MIME_LEXER_STATE_ATOM1,					// 191
	MIME_LEXER_STATE_ATOM1,					// 192
	MIME_LEXER_STATE_ATOM1,					// 193
	MIME_LEXER_STATE_ATOM1,					// 194
	MIME_LEXER_STATE_ATOM1,					// 195
	MIME_LEXER_STATE_ATOM1,					// 196
	MIME_LEXER_STATE_ATOM1,					// 197
	MIME_LEXER_STATE_ATOM1,					// 198
	MIME_LEXER_STATE_ATOM1,					// 199
	MIME_LEXER_STATE_ATOM1,					// 200
	MIME_LEXER_STATE_ATOM1,					// 201
	MIME_LEXER_STATE_ATOM1,					// 202
	MIME_LEXER_STATE_ATOM1,					// 203
	MIME_LEXER_STATE_ATOM1,					// 204
	MIME_LEXER_STATE_ATOM1,					// 205
	MIME_LEXER_STATE_ATOM1,					// 206
	MIME_LEXER_STATE_ATOM1,					// 207
	MIME_LEXER_STATE_ATOM1,					// 208
	MIME_LEXER_STATE_ATOM1,					// 209
	MIME_LEXER_STATE_ATOM1,					// 210
	MIME_LEXER_STATE_ATOM1,					// 211
	MIME_LEXER_STATE_ATOM1,					// 212
	MIME_LEXER_STATE_ATOM1,					// 213
	MIME_LEXER_STATE_ATOM1,					// 214
	MIME_LEXER_STATE_ATOM1,					// 215
	MIME_LEXER_STATE_ATOM1,					// 216
	MIME_LEXER_STATE_ATOM1,					// 217
	MIME_LEXER_STATE_ATOM1,					// 218
	MIME_LEXER_STATE_ATOM1,					// 219
	MIME_LEXER_STATE_ATOM1,					// 220
	MIME_LEXER_STATE_ATOM1,					// 221
	MIME_LEXER_STATE_ATOM1,					// 222
	MIME_LEXER_STATE_ATOM1,					// 223
	MIME_LEXER_STATE_ATOM1,					// 224
	MIME_LEXER_STATE_ATOM1,					// 225
	MIME_LEXER_STATE_ATOM1,					// 226
	MIME_LEXER_STATE_ATOM1,					// 227
	MIME_LEXER_STATE_ATOM1,					// 228
	MIME_LEXER_STATE_ATOM1,					// 229
	MIME_LEXER_STATE_ATOM1,					// 230
	MIME_LEXER_STATE_ATOM1,					// 231
	MIME_LEXER_STATE_ATOM1,					// 232
	MIME_LEXER_STATE_ATOM1,					// 233
	MIME_LEXER_STATE_ATOM1,					// 234
	MIME_LEXER_STATE_ATOM1,					// 235
	MIME_LEXER_STATE_ATOM1,					// 236
	MIME_LEXER_STATE_ATOM1,					// 237
	MIME_LEXER_STATE_ATOM1,					// 238
	MIME_LEXER_STATE_ATOM1,					// 239
	MIME_LEXER_STATE_ATOM1,					// 240
	MIME_LEXER_STATE_ATOM1,					// 241
	MIME_LEXER_STATE_ATOM1,					// 242
	MIME_LEXER_STATE_ATOM1,					// 243
	MIME_LEXER_STATE_ATOM1,					// 244
	MIME_LEXER_STATE_ATOM1,					// 245
	MIME_LEXER_STATE_ATOM1,					// 246
	MIME_LEXER_STATE_ATOM1,					// 247
	MIME_LEXER_STATE_ATOM1,					// 248
	MIME_LEXER_STATE_ATOM1,					// 249
	MIME_LEXER_STATE_ATOM1,					// 250
	MIME_LEXER_STATE_ATOM1,					// 251
	MIME_LEXER_STATE_ATOM1,					// 252
	MIME_LEXER_STATE_ATOM1,					// 253
	MIME_LEXER_STATE_ATOM1,					// 254
	MIME_LEXER_STATE_ATOM1					// 255
};

#define MIME_LEXER_START_STATE_COUNT	9
#define MIME_DISABLE_TEXT_BODIES "MIME_Disable_Text_Bodies"

// Reset value (anything above the value of MIME_LEXER_START_STATE_COUNT will do)
#define MIME_LEXER_START_RESET			100

int g_iMIMELexerStartStates[MIME_LEXER_START_STATE_COUNT] =
{
	MIME_LEXER_STATE_CRLF1,
	MIME_LEXER_STATE_LF1,
	MIME_LEXER_STATE_LWSP1,
	MIME_LEXER_STATE_QUOTEDSTRING1,
	MIME_LEXER_STATE_DOMAINLITERAL1,
	MIME_LEXER_STATE_COMMENT1,
	MIME_LEXER_STATE_ROUTEADDR1,
	MIME_LEXER_STATE_SPECIAL1,
	MIME_LEXER_STATE_ATOM1
};


typedef struct tagToken
{
	int		iTokenID;
	int		iLexemeCount;
	char	**ppszLexemes;
} TOKEN, *PTOKEN;


CMIMELexer::CMIMELexer(DWORD dwMaxScanBytes)
:	m_nLineTerminatorLength(0),
	m_nOffset(0),
	m_nStartOffset(0),
	m_nLexemeOffset(0),
	m_nContinuationOffset(0),
	m_nLWSPCount(0),
	m_nForwardOffset(0),
	m_nLineNumber(1),
	m_State(g_iMIMELexerStartStates[0]),
	m_iNextStartStateIndex(1),
	m_pParser(NULL),
	m_pIMIME(NULL),
	m_nCommentLevel(0),
	m_bNewLine(true),
	m_bIdentified(false),
    m_bContinuationLine(false),
	m_bMIMEDetectionDisabled(false),
	m_dwMaxScanBytes(dwMaxScanBytes),
	m_uchLastChar(0),
	m_ePrimaryLineTerminator(eUnknown)
{
	m_bWantBytes = true;
}

int CMIMELexer::GetSizeOfLineTerminator() const
{
	return m_nLineTerminatorLength;
}


CMIMELexer::~CMIMELexer()
{
	// Destroy the CMIMEParser object if we created one.
	if (m_pParser)
		delete m_pParser;

	// Destroy the CMIME object if we created one.
	if (m_pIMIME)
		delete m_pIMIME;
}


bool CMIMELexer::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject)
{
	m_uchLastChar = 0;
	m_nLineTerminatorLength = 0;
	m_nOffset = 0;
	m_nStartOffset = 0;
	m_nLexemeOffset = 0;
	m_nContinuationOffset = 0;
	m_nLWSPCount = 0;
	m_nForwardOffset = 0;
	m_nLineNumber = 1;
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	m_nCommentLevel = 0;
	m_bNewLine = true;
	m_bIdentified = false;
    m_bContinuationLine = false;
	m_bWantBytes = true;
	m_ePrimaryLineTerminator = eUnknown;

	// Construct and attach the CMIMEParser object.
	m_pParser = new CMIMEParser;
	if (m_pParser == NULL)
		return false;

	// Construct the IMIME public (client) interface object.
	m_pIMIME = new CMIME;
	if (m_pIMIME == NULL)
	{
		delete m_pParser;
		return false;
	}

	// Hold on to the archive so we can get options later.
	m_pArchive = pArchive;

	m_pObject = pObject;

	// Attach the CMIMEParser implementation to the IMIME interface.
	m_pIMIME->Init(m_pParser);

	// See if our parent wants us to skip this file.  This allows
	// our parent to determine, based on more information, whether
	// this file can really be a MIME file.
	
	IDecContainerObject* pParent = NULL;
	DWORD propValue = 0;
	// See if we are suppose to skip MIME detection.  This is set by
	// our parent if we have one.

	if( m_pObject != NULL	&& m_pObject->GetParent(&pParent) == DEC_OK &&
		m_pArchive != NULL	&& (m_pArchive->m_dwEnhancedID & TEXT_OPTION_IGNORE_MIME_PLAIN_TEXT_BODIES) &&
		pParent != NULL		&& pParent->GetProperty(MIME_DISABLE_TEXT_BODIES,propValue) == DEC_OK &&
		propValue == 1 )
	{
		m_bMIMEDetectionDisabled = true;
	}
	else
	{
		m_bMIMEDetectionDisabled = false;
	}

	m_bEndOfHeaderIdentificationEnabled = false;

	// Get the MIMEDetectionStength
	if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_MIME_IDENTIFICATION_STRENGTH, &propValue)) &&
		propValue > 0 )
	{
		m_bEndOfHeaderIdentificationEnabled = true;
	}


	// Give the parser everything it needs to do its work.
	return (m_pParser->Init(this,pArchive, pEngine, pDecomposer, pObject));
}


int CMIMELexer::GetInterface(void **ppInterface)
{
	if (ppInterface)
	{
		*ppInterface = m_pIMIME;
		return (LEXER_OK);
	}

	return (LEXER_ERROR);
}


int CMIMELexer::ConsumeByte(unsigned char uchByte)
{
	int		rc = LEXER_OK;	// Assume we want to continue lexing.
	bool	bGotIdentified = false;

	// See if we are suppose to skip MIME detection.  This is set by
	// our parent if we have one.

	if( m_bMIMEDetectionDisabled )
	{
		rc = LEXER_DONE;
		return (rc);
	}
	if ( m_ePrimaryLineTerminator == eUnknown && m_pParser->IsProcessingSignature() == false )
	{
		if ( IsLF(uchByte) )
		{
			// If the previous character was a CR then
			// termination is CRLF.  Otherwise is just LF
			if ( IsCR(m_uchLastChar) )
			{
				m_ePrimaryLineTerminator = eCarriageReturnLineFeed;
			}
			else 
			{
				m_ePrimaryLineTerminator = eLineFeed;
			}
		}
		else if ( IsCR(m_uchLastChar) )
		{
			// Since the last character was CR and the 
			// current character is NOT a LF then we must
			// be using a single CR as a line termination
			m_ePrimaryLineTerminator = eCarriageReturn;
		}
	}

	// Determine the length of the line terminating
	// sequence.

	if ( IsLF(uchByte) )
	{
		if ( IsCR(m_uchLastChar) )
		{
			m_nLineTerminatorLength = 2;
		}
		else
		{
			m_nLineTerminatorLength = 1;
		}
	}
	else if ( IsCR(m_uchLastChar) )
	{
		m_nLineTerminatorLength = 1;
	}

	m_uchLastChar = uchByte;

	// Make sure we can add this byte to our current token buffer.
	// If not, then emit whatever we have so far as an atom and start over.
	if (m_nLexemeOffset >= sizeof(m_byBuffer))
	{
		rc = EmitToken(MIME_TOKEN_ATOM);
		if (rc == LEXER_ERROR)
			return (rc);

		m_nStartOffset = m_nOffset;	// Set the next token's start location.
	}
	// Handle a malformity here - convert embedded NUL's to spaces
	if (uchByte == ASC_CHR_NUL)
		uchByte = ASC_CHR_SPACE;

	// Store the byte in our internal buffer.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	if (m_iNextStartStateIndex >= MIME_LEXER_START_STATE_COUNT)
		m_iNextStartStateIndex = 1;

	// This is the MIME lexer's state machine.
	while (m_iNextStartStateIndex <= MIME_LEXER_START_STATE_COUNT)
	{
		if (rc == LEXER_IDENTIFIED)
			bGotIdentified = true;

		// Performance optimization.  We can save alot of CPU cycles by not switching to states
		// that we know aren't going to do any meaningful work.
		if (m_State == MIME_LEXER_STATE_CRLF1)
		{
			// We are starting at the top of the state machine.  As a
			// performance optimization, fast-forward to the first
			// meaningful state for this input byte.
			m_State = g_iMIMELexerFirstState[uchByte];
		}

		switch (m_State)
		{
			case MIME_LEXER_STATE_CRLF1:
			{
				if (IsCR(uchByte))
				{
					// The byte is a carriage-return character.  Since we need
					// to also recognize the longer sequence CRLF, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_CRLF2;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a carriage-return character.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_CRLF2:
			{
				if (IsLF(uchByte))
				{
					// The byte is a line-feed character.  Move to the
					// next state and look for LWSP (to un-fold folded lines).
					m_nLWSPCount = 0;
					m_State = MIME_LEXER_STATE_CRLF3;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states

					// We have found a CR/LF terminator.  If this is a mixed mode
					// header then emit a warning...

					if ( m_pParser->m_bIdentified && m_pParser->m_bInHeader )
					{
						if ( m_ePrimaryLineTerminator == eCarriageReturn )
						{
							m_pParser->EmitWarning(MIME_WARNING_BARE_CARRIAGE_RETURN, MIME_TOKEN_LF);
						}
						else if ( m_ePrimaryLineTerminator == eLineFeed )
						{
							m_pParser->EmitWarning(MIME_WARNING_BARE_LINE_FEED, MIME_TOKEN_LF);
						}
					}
					break;
				} 
				else if ( m_pParser->m_bIdentified && m_pParser->m_bInHeader && m_ePrimaryLineTerminator != eCarriageReturn )
				{
					//  Set the MALFORMED mime flag if we find a bare CR in the MIME header
					 
					m_pParser->EmitWarning(MIME_WARNING_BARE_CARRIAGE_RETURN, MIME_TOKEN_CR);
				}

				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// Only do line-folding if we are in a header.
					if (m_pParser->m_bInHeader)
					{
						// The byte is a line-feed character or whitespace.  Move to the
						// next state and look for LWSP (to un-fold folded lines).
						m_bContinuationLine = true;
						m_nLWSPCount = 1;
						m_State = MIME_LEXER_STATE_CRLF3;
						m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
						break;
					}
				}

				// The byte is not a line-feed or whitespace character.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_CRLF3:
			{
				// Here we are looking for LWSP following a CR, LF, or CRLF sequence.
				// We translate this into LWSP to un-fold folded lines.
				if (!m_bNewLine)
				{
					if (IsSPACE(uchByte) || IsHTAB(uchByte))
					{
						// Only do line-folding if we are in a header.
						if (m_pParser->m_bInHeader)
						{
							// The byte is a whitespace character.
							// Increment the current line number.
							m_nLineNumber++;

							// Move to the LWSP2 state to continue looking for more
							// whitespace characters.
							m_bContinuationLine = true;
							m_nLWSPCount++;
							m_State = MIME_LEXER_STATE_LWSP2; 
							m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
							break;
						}
					}

					if (m_bContinuationLine)
					{
						// The byte is not a whitespace character and we have already seen the
						// start of a continuation line.
						// Remove the last byte added to our buffer by backing up one byte.
						m_nLexemeOffset--;

						// Emit a LWSP token.
						rc = EmitToken(MIME_TOKEN_LWSP);
						if (rc == LEXER_ERROR)
							return (rc);

						m_bContinuationLine = false;
						m_nStartOffset = m_nOffset;	// Set the next token's start location.

						// Now that our buffer has been reset, put this
						// new byte back.
						m_byBuffer[m_nLexemeOffset++] = uchByte;

						// And, finally, go back and analyze the byte from scratch.
						m_iNextStartStateIndex = 1;
						break;
					}
				}

				// The byte is not a whitespace character.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_LF1:
			{
				if (IsLF(uchByte))
				{
					// We have encountered a bare LF character in the file.
					// This emits a warning if the primary line terminator
					// is not a linefeed.
					if ( m_pParser->m_bIdentified &&  m_pParser->m_bInHeader && m_ePrimaryLineTerminator != eLineFeed)
					{
						//  Set the MALFORMED mime flag if we find a bare LF in the MIME header
						 
						m_pParser->EmitWarning(MIME_WARNING_BARE_LINE_FEED, MIME_TOKEN_LF);
					}
					// The byte is a line-feed character.  Move to the
					// next state and look for LWSP (to un-fold folded lines).
					m_nLWSPCount = 0;
					m_State = MIME_LEXER_STATE_CRLF3;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a line-feed character.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_LWSP1:
			{
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_nLWSPCount = 1;
					m_State = MIME_LEXER_STATE_LWSP2;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_LWSP2:
			{
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  We stay in this
					// same state until we find a non-whitespace character.
					m_nLWSPCount++;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				if (!m_bNewLine && m_bContinuationLine)
				{
					if (IsCR(uchByte))
					{
						// We are on a continuation line and now we have a LWSP sequence followed
						// by a CR or LF.  Treat this as a blank line.
						// Remove the last byte added to our buffer by backing up one byte.
						m_nLexemeOffset--;

						// Emit a CRLF token.  Note that the CRLF token represents three
						// character combinations: CR, LF, and CRLF.
						rc = EmitToken(MIME_TOKEN_CRLF);
						if (rc == LEXER_ERROR)
							return (rc);

						m_nStartOffset = m_nOffset - m_nLWSPCount;	// Set the next token's start location.
						m_nLWSPCount = 0;

						// Now that our buffer has been reset, put this
						// new byte back.
						m_byBuffer[m_nLexemeOffset++] = uchByte;

						// Increment the current line number.
						m_nLineNumber++;

						// Make a mental note that we just emitted a new-line.
						m_bNewLine = true;

						// And, finally, go back and analyze the byte from scratch.
						m_iNextStartStateIndex = 1;
						break;
					}

					if (IsLF(uchByte))
					{
						// We are on a continuation line and now we have a LWSP sequence followed
						// by a CR or LF.  Treat this as a blank line.
						// Remove the last byte added to our buffer by backing up one byte.
						m_nLexemeOffset--;

						// Emit a CRLF token.  Note that the CRLF token represents three
						// character combinations: CR, LF, and CRLF.
						rc = EmitToken(MIME_TOKEN_CRLF);
						if (rc == LEXER_ERROR)
							return (rc);

						m_nStartOffset = m_nOffset - m_nLWSPCount;	// Set the next token's start location.
						m_nLWSPCount = 0;

						// Now that our buffer has been reset, put this
						// new byte back.
						m_byBuffer[m_nLexemeOffset++] = uchByte;

						// Increment the current line number.
						m_nLineNumber++;

						// Make a mental note that we just emitted a new-line.
						m_bNewLine = true;

						// And, finally, go back and analyze the byte from scratch.
						m_iNextStartStateIndex = 1;
						break;
					}

				}

				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit a LWSP token.
				rc = EmitToken(MIME_TOKEN_LWSP);
				if (rc == LEXER_ERROR)
					return (rc);

				m_bContinuationLine = false;
				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_QUOTEDSTRING1:
			{
				if (m_pParser->m_bScanForPairs && IsQUOTE(uchByte))
				{
					// The byte is a quote character.  Since we need to look
					// for the closing quote and everything else in-between,
					// we can't emit anything yet.  Move to the next state
					// and wait for the next input byte.
					m_State = MIME_LEXER_STATE_QUOTEDSTRING2;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a quote character.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_QUOTEDSTRING2:
			{
				if (IsQUOTE(uchByte))
				{
					// The byte is a quote character.
					// Emit a QUOTED_STRING token.
					rc = EmitToken(MIME_TOKEN_QUOTED_STRING);
					if (rc == LEXER_ERROR)
						return (rc);

					m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsCR(uchByte))
				{
					// The byte is a CR character.  Move to state
					// QUOTEDSTRING4 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_QUOTEDSTRING4;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  Move to state
					// QUOTEDSTRING5 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_QUOTEDSTRING5;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (uchByte == ASC_CHR_BSLASH)
				{
					// The byte is the special quoted-pair character.
					// Since we need to get the next character to finish
					// the pair, we can't emit anything yet.  Move to the
					// next state and wait for the next input byte.
					m_State = MIME_LEXER_STATE_QUOTEDSTRING3;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is a CHAR.  We still need to locate the
				// closing quote character so stay in this state.
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_QUOTEDSTRING3:
			{
				// The byte is a CHAR.
				// Copy the byte over the special quoted-pair character
				// in our buffer.
				m_nLexemeOffset--;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Go back to the previous state where we are looking for
				// the closing quote.
				m_State = MIME_LEXER_STATE_QUOTEDSTRING2;
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_QUOTEDSTRING4:
			{
				// Here we are looking for a continuation line.  So far all we
				// have is a CR character.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_QUOTEDSTRING6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  We now have a CRLF sequence.
					// Move to state QUOTEDSTRING5 to look for continuation lines.
					m_State = MIME_LEXER_STATE_QUOTEDSTRING5;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a line-feed or whitespace character.
				// This is not a continuation line sequence.  We have a single CR
				// character.  This should be treated exactly the same as the
				// CRLF2 state (which also handles the single CR case).
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit the suspected quoted string, since we know it was quoted to start
				//  and have abandon the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_QUOTED_STRING);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_QUOTEDSTRING5:
			{
				size_t	nCount;

				// Here we are looking for a continuation line.  So far all we
				// have is either a LF character or a CRLF sequence.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_QUOTEDSTRING6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				// This is not a continuation line sequence.  We either have a
				// single LF character or a CRLF sequence.  In either case we
				// need to emit a CRLF token.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Back up past any CR or LF characters.
				nCount = 0;
				while (m_nLexemeOffset != 0 &&
					  (IsCR(m_byBuffer[m_nLexemeOffset - 1]) || IsLF(m_byBuffer[m_nLexemeOffset - 1])))
				{
					nCount++;
					m_nLexemeOffset--;
				}

				// Emit the suspected quoted string, since we know it was quoted to start
				//  and have abandon the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_QUOTED_STRING);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset - nCount;  // Set the next token's start location.

				// Now we need to put back the LF or CRLF characters.
				if (nCount == 2)
					m_byBuffer[m_nLexemeOffset++] = ASC_CHR_CR;

				m_byBuffer[m_nLexemeOffset++] = ASC_CHR_LF;

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_QUOTEDSTRING6:
			{
				// Here we are looking for additional whitespace on
				// a continuation line.  So far we have one of the following
				// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
				// Now all we need to do is skip any additional whitespace.
				// Once we hit something that is not whitespace, that character
				// must be the beginning of the continuation line.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  This is still part
					// of the continuation-line sequence.  Stay in this state
					// to keep looking for more whitespace.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.  This must be the
				// beginning of the continued line.  So, we need to fold the
				// previous line here by removing the continuation characters
				// that have been added to the buffer up to this point.  Since
				// we saved the offset (m_nContinuationOffset) of the first
				// lexeme in the continuation sequence, we simply need to back
				// up m_nLexemeOffset to that point and re-add this byte there.
				m_nLexemeOffset = m_nContinuationOffset;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_State = MIME_LEXER_STATE_QUOTEDSTRING2;
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_DOMAINLITERAL1:
			{
				if (m_pParser->m_bScanForPairs && uchByte == ASC_CHR_LBRACK)
				{
					// The byte is a left-bracket.  Since we need to look
					// for the right-bracket and everything else in-between,
					// we can't emit anything yet.  Move to the next state
					// and wait for the next input byte.
					m_State = MIME_LEXER_STATE_DOMAINLITERAL2;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a left-bracket.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_DOMAINLITERAL2:
			{
				if (uchByte == ASC_CHR_RBRACK)
				{
					// The byte is a right-bracket character.
					// Emit a DOMAIN_LITERAL token.
					rc = EmitToken(MIME_TOKEN_DOMAIN_LITERAL);
					if (rc == LEXER_ERROR)
						return (rc);

					m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsCR(uchByte))
				{
					// The byte is a CR character.  Move to state
					// DOMAINLITERAL4 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_DOMAINLITERAL4;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  Move to state
					// DOMAINLITERAL5 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_DOMAINLITERAL5;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (uchByte == ASC_CHR_BSLASH)
				{
					// The byte is the special quoted-pair character.
					// Since we need to get the next character to finish
					// the pair, we can't emit anything yet.  Move to the
					// next state and wait for the next input byte.
					m_State = MIME_LEXER_STATE_DOMAINLITERAL3;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is a CHAR.  We still need to locate the
				// right-bracket character so stay in this state.
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_DOMAINLITERAL3:
			{
				// The byte is a CHAR.
				// Copy the byte over the special quoted-pair character
				// in our buffer.
				m_nLexemeOffset--;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Go back to the previous state where we are looking for
				// the right-bracket.
				m_State = MIME_LEXER_STATE_DOMAINLITERAL2;
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_DOMAINLITERAL4:
			{
				// Here we are looking for a continuation line.  So far all we
				// have is a CR character.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_DOMAINLITERAL6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  We now have a CRLF sequence.
					// Move to state DOMAINLITERAL5 to look for continuation lines.
					m_State = MIME_LEXER_STATE_DOMAINLITERAL5;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a line-feed or whitespace character.
				// This is not a continuation line sequence.  We have a single CR
				// character.  This should be treated exactly the same as the
				// CRLF2 state (which also handles the single CR case).
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit the suspected domain literal string, since we know it was
				// started and we have abandoned the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_DOMAIN_LITERAL);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_DOMAINLITERAL5:
			{
				// Here we are looking for a continuation line.  So far all we
				// have is either a LF character or a CRLF sequence.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_DOMAINLITERAL6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				// This is not a continuation line sequence.  We either have a
				// single LF character or a CRLF sequence.  In either case we
				// need to emit a CRLF token.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit the suspected domain literal string, since we know it was
				// started and we have abandoned the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_DOMAIN_LITERAL);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_DOMAINLITERAL6:
			{
				// Here we are looking for additional whitespace on
				// a continuation line.  So far we have one of the following
				// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
				// Now all we need to do is skip any additional whitespace.
				// Once we hit something that is not whitespace, that character
				// must be the beginning of the continuation line.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  This is still part
					// of the continuation-line sequence.  Stay in this state
					// to keep looking for more whitespace.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.  This must be the
				// beginning of the continued line.  So, we need to fold the
				// previous line here by removing the continuation characters
				// that have been added to the buffer up to this point.  Since
				// we saved the offset (m_nContinuationOffset) of the first
				// lexeme in the continuation sequence, we simply need to back
				// up m_nLexemeOffset to that point and re-add this byte there.
				m_nLexemeOffset = m_nContinuationOffset;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_State = MIME_LEXER_STATE_DOMAINLITERAL2;
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_COMMENT1:
			{
				if (m_pParser->m_bScanForPairs && uchByte == ASC_CHR_LPAREN)
				{
					// The byte is an open-parenthesis.  Since we need to look
					// for the close-parenthesis and everything else in-between,
					// we can't emit anything yet.  Move to the next state
					// and wait for the next input byte.
					m_State = MIME_LEXER_STATE_COMMENT2;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not an open-parenthesis.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_COMMENT2:
			{
				if (uchByte == ASC_CHR_RPAREN)
				{
					// The byte is a close-parenthesis character.
					if (m_nCommentLevel)
						m_nCommentLevel--;

					if (m_nCommentLevel == 0)
					{
						// Emit a COMMENT token.
						rc = EmitToken(MIME_TOKEN_COMMENT);
						if (rc == LEXER_ERROR)
							return (rc);

						m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
					}

					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsCR(uchByte))
				{
					// The byte is a CR character.  Move to state
					// COMMENT4 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_COMMENT4;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  Move to state
					// COMMENT5 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_COMMENT5;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (uchByte == ASC_CHR_BSLASH)
				{
					// The byte is the special quoted-pair character.
					// Since we need to get the next character to finish
					// the pair, we can't emit anything yet.  Move to the
					// next state and wait for the next input byte.
					m_State = MIME_LEXER_STATE_COMMENT3;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is a CHAR.  We still need to locate the
				// close-parenthesis character so stay in this state.
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_COMMENT3:
			{
				// The byte is a CHAR.
				// Copy the byte over the special quoted-pair character
				// in our buffer.
				m_nLexemeOffset--;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Go back to the previous state where we are looking for
				// the closing quote.
				m_State = MIME_LEXER_STATE_COMMENT2;
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_COMMENT4:
			{
				// Here we are looking for a continuation line.  So far all we
				// have is a CR character.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_COMMENT6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  We now have a CRLF sequence.
					// Move to state COMMENT5 to look for continuation lines.
					m_State = MIME_LEXER_STATE_COMMENT5;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a line-feed or whitespace character.
				// This is not a continuation line sequence.  We have a single CR
				// character.  This should be treated exactly the same as the
				// CRLF2 state (which also handles the single CR case).
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit the suspected comment string, since we know it was
				// started and we have abandoned the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_COMMENT);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_COMMENT5:
			{
				// Here we are looking for a continuation line.  So far all we
				// have is either a LF character or a CRLF sequence.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_COMMENT6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				// This is not a continuation line sequence.  We either have a
				// single LF character or a CRLF sequence.  In either case we
				// need to emit a CRLF token.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit the suspected comment string, since we know it was
				// started and we have abandoned the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_COMMENT);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_COMMENT6:
			{
				// Here we are looking for additional whitespace on
				// a continuation line.  So far we have one of the following
				// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
				// Now all we need to do is skip any additional whitespace.
				// Once we hit something that is not whitespace, that character
				// must be the beginning of the continuation line.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  This is still part
					// of the continuation-line sequence.  Stay in this state
					// to keep looking for more whitespace.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.  This must be the
				// beginning of the continued line.  So, we need to fold the
				// previous line here by removing the continuation characters
				// that have been added to the buffer up to this point.  Since
				// we saved the offset (m_nContinuationOffset) of the first
				// lexeme in the continuation sequence, we simply need to back
				// up m_nLexemeOffset to that point and re-add this byte there.
				m_nLexemeOffset = m_nContinuationOffset;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_State = MIME_LEXER_STATE_COMMENT2;
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_ROUTEADDR1:
			{
				if (m_pParser->m_bScanForPairs && uchByte == ASC_CHR_LT)
				{
					// The byte is an less-than symbol.  Since we need to look
					// for the greater-than symbol and everything else in-between,
					// we can't emit anything yet.  Move to the next state
					// and wait for the next input byte.
					m_State = MIME_LEXER_STATE_ROUTEADDR2;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a less-than symbol.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_ROUTEADDR2:
			{
				if (uchByte == ASC_CHR_GT)
				{
					// The byte is a greater-than symbol character.
					// Emit a ROUTE_ADDR token.
					rc = EmitToken(MIME_TOKEN_ROUTE_ADDR);
					if (rc == LEXER_ERROR)
						return (rc);

					m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsCR(uchByte))
				{
					// The byte is a CR character.  Move to state
					// ROUTEADDR4 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_ROUTEADDR4;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  Move to state
					// ROUTEADDR5 to look for continuation lines.
					// Also save the offset of this character in the lexeme buffer
					// so that we can remove the continuation characters later.
					m_State = MIME_LEXER_STATE_ROUTEADDR5;
					m_nContinuationOffset = m_nLexemeOffset - 1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (uchByte == ASC_CHR_BSLASH)
				{
					// The byte is the special quoted-pair character.
					// Since we need to get the next character to finish
					// the pair, we can't emit anything yet.  Move to the
					// next state and wait for the next input byte.
					m_State = MIME_LEXER_STATE_ROUTEADDR3;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is a CHAR.  We still need to locate the
				// greater-than symbol character so stay in this state.
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_ROUTEADDR3:
			{
				// The byte is a CHAR.
				// Copy the byte over the special quoted-pair character
				// in our buffer.
				m_nLexemeOffset--;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Go back to the previous state where we are looking for
				// the closing quote.
				m_State = MIME_LEXER_STATE_ROUTEADDR2;
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_ROUTEADDR4:
			{
				// Here we are looking for a continuation line.  So far all we
				// have is a CR character.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_ROUTEADDR6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					// The byte is a LF character.  We now have a CRLF sequence.
					// Move to state ROUTEADDR5 to look for continuation lines.
					m_State = MIME_LEXER_STATE_ROUTEADDR5;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a line-feed or whitespace character.
				// This is not a continuation line sequence.  We have a single CR
				// character.  This should be treated exactly the same as the
				// CRLF2 state (which also handles the single CR case).
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit the suspected route address string, since we know it was
				// started and we have abandoned the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_ROUTE_ADDR);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_ROUTEADDR5:
			{
				// Here we are looking for a continuation line.  So far all we
				// have is either a LF character or a CRLF sequence.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Now we have a valid
					// continuation-line sequence.  However, since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MIME_LEXER_STATE_ROUTEADDR6;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				// This is not a continuation line sequence.  We either have a
				// single LF character or a CRLF sequence.  In either case we
				// need to emit a CRLF token.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit the suspected route address string, since we know it was
				// started and we have abandoned the notion of a possible continuation line.
				rc = EmitToken(MIME_TOKEN_ROUTE_ADDR);
				if (rc == LEXER_ERROR)
				  return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Emit a CRLF token.  Note that the CRLF token represents three
				// character combinations: CR, LF, and CRLF.
				rc = EmitToken(MIME_TOKEN_CRLF);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// Increment the current line number.
				m_nLineNumber++;

				// Make a mental note that we just emitted a new-line.
				m_bNewLine = true;

				// And, finally, go back and analyze the byte from scratch.
				m_State = g_iMIMELexerStartStates[0];
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_ROUTEADDR6:
			{
				// Here we are looking for additional whitespace on
				// a continuation line.  So far we have one of the following
				// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
				// Now all we need to do is skip any additional whitespace.
				// Once we hit something that is not whitespace, that character
				// must be the beginning of the continuation line.
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  This is still part
					// of the continuation-line sequence.  Stay in this state
					// to keep looking for more whitespace.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.  This must be the
				// beginning of the continued line.  So, we need to fold the
				// previous line here by removing the continuation characters
				// that have been added to the buffer up to this point.  Since
				// we saved the offset (m_nContinuationOffset) of the first
				// lexeme in the continuation sequence, we simply need to back
				// up m_nLexemeOffset to that point and re-add this byte there.
				m_nLexemeOffset = m_nContinuationOffset;
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_State = MIME_LEXER_STATE_ROUTEADDR2;
				m_iNextStartStateIndex = 1;
				break;
			}

			case MIME_LEXER_STATE_SPECIAL1:
			{
				if (IsTSPECIAL(uchByte))
				{
					// Emit a SPECIAL token.
					rc = EmitToken(MIME_TOKEN_SPECIAL);
					if (rc == LEXER_ERROR)
						return (rc);

					m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a special character.
				// Move to the next start state.
				m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MIME_LEXER_STATE_ATOM1:
			{
				// If the character did not get recognized by any of the
				// previous start states, then we'll call it an atom.
				// Since we need to also recognize long sequences of atom
				// characters, we can't emit anything yet.  Move to the next
				// state and wait for the next input byte.
				m_State = MIME_LEXER_STATE_ATOM2;
				m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
				break;
			}

			case MIME_LEXER_STATE_ATOM2:
			{
				if (!IsTSPECIAL(uchByte) &&
					!IsSPACE(uchByte) &&
					!IsCTL(uchByte))
				{
					// The byte is a valid atom character.  We stay in this
					// same state until we find a non-atom character.
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a valid atom character.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				// Emit an ATOM token.
				rc = EmitToken(MIME_TOKEN_ATOM);
				if (rc == LEXER_ERROR)
					return (rc);

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				break;
			}

			default:
			{
				dec_assert(0);	// We should never fall outside the state machine!
				break;
			}
		} // end of switch (m_State)
	}

	m_nOffset++;

	// Determine if we are delaying identification until the end of header
	// has been found.  If we are then we replace the standard identification
	// with this code.
	if ( m_bEndOfHeaderIdentificationEnabled )
	{
		// User requested delayed identification. Until we reach the end of
		// header we will do nothing but continue.  Once the end of header
		// has been found we will look at what has been identified and make
		// our decision.
		if ( m_pParser->EndOfHeaderFound() )
		{
			m_pParser->ResetEndOfHeaderFlag();
			// This is a MIME mail message.
			if ( bGotIdentified )
			{
				if ( m_pParser->isValidMIMEHeader() ) 
				{
					m_pParser->FileDeterminedToBeMIME();
					rc = LEXER_IDENTIFIED;
				}
				else
				{
					rc = LEXER_DONE;
				}
			}
			
			// End Of Header Identification applies only to the
			// top level container.  Headers that are part of a multipart body
			// are not subject to the same tests as the main header.

			m_bEndOfHeaderIdentificationEnabled = false;
		}
	}
	else if (bGotIdentified)
	{
		// See if this is MIME.  If so set the attributes.
		if ( m_pParser->EndOfHeaderFound()  )
		{
			m_pParser->FileDeterminedToBeMIME();
		}

		if (rc == LEXER_DONE)
			rc = LEXER_IDENTIFIED_DONE;
		else
			rc = LEXER_IDENTIFIED;
	}

	if (!m_bIdentified && m_dwMaxScanBytes && m_nOffset > m_dwMaxScanBytes)
		rc = LEXER_DONE;
	return (rc);
}


int CMIMELexer::ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset)
{
	int				index;
	int				iMaxIndex;
	unsigned char	uchByte1;
	unsigned char	uchByte2;
	unsigned char	uchByte3;
	unsigned char	uchByte4;
	size_t			nBytesConsumed;
	size_t			nOffset;

	dec_assert(piBufferIndex);
	dec_assert(pnOffset);

	// The MIME engine uses this function only to skip over body parts while
	// it is looking for the next boundary marker.  To do this as quickly as
	// possible, we scan the buffer for a possible boundary-start sequence.
	// If we find a possible boundary-start sequence, we switch back to
	// single-byte lexing.  If we don't, then we just wait for the next buffer.
	// Note that we need to carefully handle the situation where the
	// boundary-start sequence just happens to span across the end of one buffer
	// and into the next buffer.
	if (*pnBufferSize < 4)
	{
		// Change back to lexing bytes.
		m_bWantBytes = true;
		return (ConsumeByte(pBuffer[*piBufferIndex]));
	}

	// Does this look like the start of a boundary? This may occur if we have a single
	// lf or cr line terminator.
	index = *piBufferIndex;

	uchByte1 = pBuffer[index];

	if (uchByte1 == ASC_CHR_MINUS)
	{
		uchByte2 = pBuffer[index + 1];

		if (uchByte2 == ASC_CHR_MINUS)
		{
			// Looks like we might be at a boundary. Reset and return from ConsumeBuffer().

			// Set the next token's start location.
			m_nStartOffset      = *pnOffset;
			m_nOffset           = *pnOffset;

			// Back up one
			*pnOffset           -= 1;

			// Reset the buffer index to 0 to start building up the next token.
			m_nLexemeOffset     = 0;

			// Reset the new-line flag.
			m_bNewLine          = false;
			m_bContinuationLine = false;

			// Back up one
			*piBufferIndex      -= 1;

			*pnBufferSize++;

			// Reset last Char..
			m_uchLastChar = 0;

			m_State = MIME_LEXER_STATE_CRLF1;
			// Skip remaining start states
			m_iNextStartStateIndex = MIME_LEXER_START_RESET;

			// Change back to lexing bytes.
			m_bWantBytes = true;

			return (LEXER_OK);
		}
	}
	
	iMaxIndex = *piBufferIndex + (int)(*pnBufferSize - 3);
	for (index = *piBufferIndex; index < iMaxIndex; index++)
	{
		uchByte1 = pBuffer[index];
		if (uchByte1 == ASC_CHR_CR)
		{
			m_nLineTerminatorLength = 1;

			uchByte2 = pBuffer[index + 1];
			if (uchByte2 == ASC_CHR_LF)
			{
				m_nLineTerminatorLength = 2;

				uchByte3 = pBuffer[index + 2];
				if (uchByte3 == ASC_CHR_MINUS)
				{
					uchByte4 = pBuffer[index + 3];
					if (uchByte4 == ASC_CHR_MINUS)
					{
						// Position ourselves over the last character before the first "-".
						// We do this so that the parser can receive the entire beginning
						// of this possible boundary marker.
						nBytesConsumed = (size_t)(index - *piBufferIndex);
						nOffset = *pnOffset;
						nOffset += (nBytesConsumed + 1);
						*pnOffset = nOffset++;

						// Reset the buffer index to 0 to start building up the next token.
						m_nLexemeOffset = 0;

						// Set the next token's start location.
						m_nStartOffset = nOffset;
						m_nOffset = nOffset;

						// Reset the new-line flag.
						m_bNewLine = false;
						m_bContinuationLine = false;

						*piBufferIndex = index + 1;
						nOffset = *pnBufferSize;
						nOffset -= (nBytesConsumed + 1);
						*pnBufferSize = nOffset;

						// Reset last Char..
						m_uchLastChar = 0;

						m_State = MIME_LEXER_STATE_CRLF1;
						m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states

						// Change back to lexing bytes.
						m_bWantBytes = true;
						return (LEXER_OK);
					}
				}
			}

			if (uchByte2 == ASC_CHR_MINUS)
			{
				uchByte3 = pBuffer[index + 2];
				if (uchByte3 == ASC_CHR_MINUS)
				{
					// Position ourselves over the last character before the first "-".
					// We do this so that the parser can receive the entire beginning
					// of this possible boundary marker.
					nBytesConsumed = (size_t)(index - *piBufferIndex);
					nOffset = *pnOffset;
					nOffset += nBytesConsumed;
					*pnOffset = nOffset++;

					// Reset the buffer index to 0 to start building up the next token.
					m_nLexemeOffset = 0;

					// Set the next token's start location.
					m_nStartOffset = nOffset;
					m_nOffset = nOffset;

					// Reset the new-line flag.
					m_bNewLine = false;
					m_bContinuationLine = false;

					*piBufferIndex = index;
					nOffset = *pnBufferSize;
					nOffset -= nBytesConsumed;
					*pnBufferSize = nOffset;

					// Reset last Char..
					m_uchLastChar = 0;

					m_State = MIME_LEXER_STATE_CRLF1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states

					// Change back to lexing bytes.
					m_bWantBytes = true;
					return (LEXER_OK);
				}
			}
		}

		if (uchByte1 == ASC_CHR_LF)
		{
			uchByte2 = pBuffer[index + 1];
	
			if ( IsCR(m_uchLastChar) )
			{
				m_nLineTerminatorLength = 2;
			}
			else
			{
				m_nLineTerminatorLength = 1;
			}

			if (uchByte2 == ASC_CHR_MINUS)
			{
				uchByte3 = pBuffer[index + 2];
				if (uchByte3 == ASC_CHR_MINUS)
				{
					// Position ourselves over the last character before the first "-".
					// We do this so that the parser can receive the entire beginning
					// of this possible boundary marker.
					nBytesConsumed = (size_t)(index - *piBufferIndex);
					nOffset = *pnOffset;
					nOffset += nBytesConsumed;
					*pnOffset = nOffset++;

					// Reset the buffer index to 0 to start building up the next token.
					m_nLexemeOffset = 0;

					// Set the next token's start location.
					m_nStartOffset = nOffset;
					m_nOffset = nOffset;

					// Reset the new-line flag.
					m_bNewLine = false;
					m_bContinuationLine = false;

					*piBufferIndex = index;
					nOffset = *pnBufferSize;
					nOffset -= nBytesConsumed;
					*pnBufferSize = nOffset;

					// Reset last Char..
					m_uchLastChar = 0;

					m_State = MIME_LEXER_STATE_CRLF1;
					m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states

					// Change back to lexing bytes.
					m_bWantBytes = true;
					return (LEXER_OK);
				}
			}
			m_uchLastChar = uchByte1;
		}
	}

	// Backup the index by 1 since we did not examine the byte
	// at this index, but instead exited the for loop above.

	index--;
	nBytesConsumed = (size_t)(index - *piBufferIndex);
	nOffset = *pnOffset;
	nOffset += nBytesConsumed;
	*pnOffset = nOffset++;

	// Reset the buffer index to 0 to start building up the next token.
	m_nLexemeOffset = 0;

	// Set the next token's start location.
	m_nStartOffset = nOffset;
	m_nOffset = nOffset;

	// Reset the new-line flag.
	m_bNewLine = false;

	*piBufferIndex = index;
	nOffset = *pnBufferSize;
	nOffset -= nBytesConsumed;
	*pnBufferSize = nOffset;

	m_State = MIME_LEXER_STATE_CRLF1;
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states

	// Change back to lexing bytes.
	m_bWantBytes = true;
	return (LEXER_OK);
}


int CMIMELexer::ConsumeEOF(size_t nOffset)
{
	int		rc;

	// Before we emit the EOF token, make sure we don't have a token
	// that still needs to be emitted first.
	switch (m_State)
	{
		case MIME_LEXER_STATE_CRLF2:
			// Emit a CRLF token.  Note that the CRLF token represents three
			// character combinations: CR, LF, and CRLF.
			rc = EmitToken(MIME_TOKEN_CRLF);
			if (rc == LEXER_ERROR)
				return (rc);

			m_nStartOffset = m_nOffset;	// Set the next token's start location.
			break;

		case MIME_LEXER_STATE_CRLF3:
			// Emit a CRLF token.  Note that the CRLF token represents three
			// character combinations: CR, LF, and CRLF.
			rc = EmitToken(MIME_TOKEN_CRLF);
			if (rc == LEXER_ERROR)
				return (rc);

			m_nStartOffset = m_nOffset;	// Set the next token's start location.
			break;

		case MIME_LEXER_STATE_LWSP2:
			// Emit a LWSP token.
			rc = EmitToken(MIME_TOKEN_LWSP);
			if (rc == LEXER_ERROR)
				return (rc);

			m_nStartOffset = m_nOffset;	// Set the next token's start location.
			break;

		case MIME_LEXER_STATE_QUOTEDSTRING2:
		case MIME_LEXER_STATE_QUOTEDSTRING3:
		case MIME_LEXER_STATE_QUOTEDSTRING4:
		case MIME_LEXER_STATE_QUOTEDSTRING5:
		case MIME_LEXER_STATE_QUOTEDSTRING6:
		case MIME_LEXER_STATE_DOMAINLITERAL2:
		case MIME_LEXER_STATE_DOMAINLITERAL3:
		case MIME_LEXER_STATE_DOMAINLITERAL4:
		case MIME_LEXER_STATE_DOMAINLITERAL5:
		case MIME_LEXER_STATE_DOMAINLITERAL6:
		case MIME_LEXER_STATE_COMMENT2:
		case MIME_LEXER_STATE_COMMENT3:
		case MIME_LEXER_STATE_COMMENT4:
		case MIME_LEXER_STATE_COMMENT5:
		case MIME_LEXER_STATE_COMMENT6:
		case MIME_LEXER_STATE_ROUTEADDR2:
		case MIME_LEXER_STATE_ROUTEADDR3:
		case MIME_LEXER_STATE_ROUTEADDR4:
		case MIME_LEXER_STATE_ROUTEADDR5:
		case MIME_LEXER_STATE_ROUTEADDR6:
		case MIME_LEXER_STATE_ATOM2:
			// Emit an ATOM token.
			rc = EmitToken(MIME_TOKEN_ATOM);
			if (rc == LEXER_ERROR)
				return (rc);

			m_nStartOffset = m_nOffset;	// Set the next token's start location.
			break;

		default:
			break;
	}
	
	// Emit an EOF token.
	m_nLexemeOffset = 0;
	m_nOffset = nOffset;
	m_nStartOffset = nOffset;
	rc = EmitToken(MIME_TOKEN_EOF);

	// Determine if we are delaying identification until the end of header
	// has been found.  If we are then we replace the standard identification
	// with this code.
	if ( m_bEndOfHeaderIdentificationEnabled )
	{
		// User requested delayed identification. Until we reach the end of
		// header we will do nothing but continue.  Once the end of header
		// has been found we will look at what has been identified and make
		// our decision.
		if ( m_pParser->EndOfHeaderFound() )
		{
			m_pParser->ResetEndOfHeaderFlag();
			// This is a MIME mail message.
			if ( m_pParser->isValidMIMEHeader() ) 
			{
				m_pParser->FileDeterminedToBeMIME();
				rc = LEXER_IDENTIFIED;
			}
			else
			{
				rc = LEXER_DONE;
			}

			// End Of Header Identification applies only to the
			// top level container.  Headers that are part of a multipart body
			// are not subject to the same tests as the main header.

			m_bEndOfHeaderIdentificationEnabled = false;
		}
		else
		{
			rc = LEXER_DONE;
		}
	}
	else
	{
		// See if this is MIME.  If so set the attributes.
		if ( m_bIdentified && m_pParser->EndOfHeaderFound()  )
		{
			m_pParser->FileDeterminedToBeMIME();
		}
	}

	// Return the result
	return rc;
}


int CMIMELexer::LexComplete(void)
{
	m_pParser->ParseComplete();
	return (LEXER_OK);
}


int CMIMELexer::ProcessComplete(void)
{
	return (LEXER_OK);
}


inline int CMIMELexer::EmitToken(int iTokenID)
{
	int		rc;

	// Make sure we don't try to emit an invalid token ID.
	dec_assert(iTokenID < MIME_TOKEN_LAST);

	// We need to emit whatever token we have in our buffer.
	// Send the emitted token on to the parsing engine.
	if (m_nLexemeOffset < sizeof(m_byBuffer))	m_byBuffer[m_nLexemeOffset] = 0;
	rc = m_pParser->ConsumeToken(iTokenID,
								(char *)m_byBuffer,
								m_nLexemeOffset,
								m_nStartOffset,
								m_nLineNumber);
	if (rc == PARSER_DONE)
	{
		rc = LEXER_DONE;
	}
	else if (rc == PARSER_IDENTIFIED)
	{
		rc = LEXER_IDENTIFIED;
		m_bIdentified = true;
	}
	else if (rc == PARSER_ERROR)
	{
		rc = LEXER_ERROR;
	}

	// Set which lexing routine the parser wants to use next.
	m_bWantBytes = m_pParser->m_bWantBytes;

	// Reset the buffer index to 0 to start building up the next token.
	m_nLexemeOffset = 0;

	// Reset the new-line flag.
	m_bNewLine = false;

	// Reset the state machine to the first start state.
	m_State = g_iMIMELexerStartStates[0];
	return (rc);
}


int CMIMELexer::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	int result = 0;
	int contentType = 0;

	if ( m_pParser ) 
		result = m_pParser->FindFirstEntry(pData);

	if ( m_pObject != NULL &&
		 m_pParser != NULL )
	{
		// If the entry we find is plain text and is not an attachment
		// we will not want to disable the text/MIME engine when processing
		// the child.  This will reduce false positives.

		if ((m_pParser->isAttachment() == false ) &&
			((contentType = m_pParser->GetContentType()) == MIME_TOKEN_TEXT || contentType == -1) )
		{
			m_pObject->SetProperty(MIME_DISABLE_TEXT_BODIES,1);
		}
		else
		{
			m_pObject->SetProperty(MIME_DISABLE_TEXT_BODIES,(DWORD)0);
		}
	}

	return result;
}

int CMIMELexer::FindNextEntry(TEXT_FIND_DATA *pData)
{
	int result = 0;
	int contentType = 0;

	if ( m_pParser ) 
		result = m_pParser->FindNextEntry(pData);

	if ( m_pObject != NULL &&
		 m_pParser != NULL )
	{
		if ((m_pParser->isAttachment() == false ) &&
			((contentType = m_pParser->GetContentType()) == MIME_TOKEN_TEXT || contentType == -1) )
		{
			m_pObject->SetProperty(MIME_DISABLE_TEXT_BODIES,1);
		}
		else
		{
			m_pObject->SetProperty(MIME_DISABLE_TEXT_BODIES,(DWORD)0);
		}
	}
	
	return result;
}

 
int CMIMELexer::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	// Not used by this engine.
	return LEXER_OK;
}


int CMIMELexer::ExtractEntry(char *pszFilename, FILE *fpin)
{
	return (m_pParser->ExtractEntry(pszFilename, fpin));
}


int CMIMELexer::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	return (m_pParser->ReplaceEntry(pData, pbKeepChildDataFile, pbKeepChildNewDataFile));
}


int CMIMELexer::AddEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->AddEntry(pData));
}


int CMIMELexer::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->DeleteEntry(pData));
}
