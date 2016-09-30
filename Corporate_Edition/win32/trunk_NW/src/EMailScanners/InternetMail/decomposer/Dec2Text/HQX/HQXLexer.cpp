// HQXLexer.cpp : Lexer for the HQX engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "HQX.h"

#define HQX_LEXER_START_STATE_COUNT	  1

// Reset value (anything above the value of HQX_LEXER_START_STATE_COUNT will do)
#define HQX_LEXER_START_RESET		  HQX_LEXER_START_STATE_COUNT + 1

int g_iHQXLexerStartStates[HQX_LEXER_START_STATE_COUNT] =
{
	HQX_LEXER_STATE_COLON1
};

typedef struct tagHQXToken
{
	int		iTokenID;
	int		iLexemeCount;
	char	**ppszLexemes;
} HQXTOKEN, *PHQXTOKEN;


CHQXLexer::CHQXLexer(DWORD dwMaxScanBytes)
{
	m_nOffset = 0;
	m_nStartOffset = 0;
	m_nLexemeOffset = 0;
	m_State = g_iHQXLexerStartStates[0];
	m_iNextStartStateIndex = 1;
	m_pParser = NULL;
	m_bInBody = false;
	m_bIdentified = false;
	m_bWantBytes = true;

	//
	// Option defaults
	//
	m_dwTextNonHQXThreshold = 4096;
	m_dwMaxScanBytes = dwMaxScanBytes;
}


CHQXLexer::~CHQXLexer()
{
	// Destroy the CHQXParser object if we created one.
	if (m_pParser)
	{
		delete m_pParser;
		m_pParser = NULL;
	}
}


bool CHQXLexer::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject)
{
	m_nOffset = 0;
	m_nStartOffset = 0;
	m_nLexemeOffset = 0;
	m_State = g_iHQXLexerStartStates[0];
	m_iNextStartStateIndex = 1;
	m_bInBody = false;
	m_bIdentified = false;
	m_bWantBytes = true;

	DWORD dwValue = 0;

	//
	// Set any applicable HQX lexer options here
	//
	if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_NONHQXTHRESHOLD, &dwValue)))
		m_dwTextNonHQXThreshold = dwValue;

	m_pParser = new CHQXParser;
	if (!m_pParser)
		return false;

	return (m_pParser->Init(pArchive, pEngine, pDecomposer));
}


int CHQXLexer::GetInterface(void **ppInterface)
{
	if (ppInterface)
	{
		*ppInterface = NULL;
		return (LEXER_OK);
	}

	return (LEXER_ERROR);
}


int CHQXLexer::ConsumeByte(unsigned char uchByte)
{
	int		rc = LEXER_OK;	// Assume we want to continue lexing.

	// Make sure we can add this byte to our current token buffer.
	// If not, then reset.
	if (m_nLexemeOffset >= sizeof(m_byBuffer))
	{
		m_nStartOffset += m_nLexemeOffset;
		m_nLexemeOffset = 0;
	}

	// Store the byte in our internal buffer.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	if (m_iNextStartStateIndex >= HQX_LEXER_START_STATE_COUNT)
		m_iNextStartStateIndex = 0;

	// This is the HQX lexer's state machine.
	while (m_iNextStartStateIndex <= HQX_LEXER_START_STATE_COUNT)
	{
		switch (m_State)
		{
			case HQX_LEXER_STATE_COLON1:
			{
				if (uchByte == ASC_CHR_COLON)
				{
					m_bIdentified = false;

					// The byte is a colon character.
					if (!m_bInBody)
					{
						// We have not entered a body yet (i.e. this is the first colon).
						// Emit a BODY_START token (ignore anything else in the buffer).
						m_nStartOffset += m_nLexemeOffset - 1;
						m_nLexemeOffset = 0;
						m_byBuffer[m_nLexemeOffset++] = uchByte;
						rc = EmitToken(HQX_TOKEN_BODY_START);
						if (rc == LEXER_ERROR)
							return (rc);

						m_bInBody = true;
						m_State = HQX_LEXER_STATE_BODY1;
					}
					else
					{
						// We have already entered a body (i.e. this is the second colon).
						// Emit a BODY_END token.
						m_bInBody = false;
						m_nStartOffset += m_nLexemeOffset;
						m_nLexemeOffset = 0;
						m_byBuffer[m_nLexemeOffset++] = uchByte;
						rc = EmitToken(HQX_TOKEN_BODY_END);
						if (rc == LEXER_ERROR)
							return (rc);
					}
				}

				m_iNextStartStateIndex = HQX_LEXER_START_RESET;	// Skip remaining start states
				break;
			}

			case HQX_LEXER_STATE_BODY1:
			{
				if (!m_bIdentified && m_nLexemeOffset >= HQX_HEADER_BYTES_NEEDED)
				{
					// The buffer should contain the beginning of the BinHex
					// body.  We should have enough data to decode the BinHex
					// header.  Emit a BODY_DATA token and let the parser
					// try to decode and verify the header.
					rc = EmitToken(HQX_TOKEN_BODY_DATA);
					if (rc == LEXER_ERROR)
						return (rc);

					if (rc == LEXER_IDENTIFIED)
					{
						m_bIdentified = true;
						rc = LEXER_IDENTIFIED_DONE;
					}
					else
					{
						m_bInBody = false;
						m_State = g_iHQXLexerStartStates[0];
					}
				}

				if (uchByte == ASC_CHR_COLON)
				{
					// The buffer should contain the beginning of the BinHex
					// body.  We should have enough data to decode the BinHex
					// header.  Emit a BODY_DATA token and let the parser
					// try to decode and verify the header.
					rc = EmitToken(HQX_TOKEN_BODY_DATA);
					if (rc == LEXER_ERROR)
						return (rc);

					if (rc == LEXER_IDENTIFIED)
					{
						m_bIdentified = true;
						rc = LEXER_IDENTIFIED_DONE;
					}
					else
					{
						m_bInBody = false;
						m_State = g_iHQXLexerStartStates[0];
						break;
					}

					// Emit a BODY_END token.
					if (EmitToken(HQX_TOKEN_BODY_END) == LEXER_ERROR)
						return (LEXER_ERROR);

					m_bInBody = false;
					m_State = g_iHQXLexerStartStates[0];
				}

				// Stay in this state which will keep adding characters to 
				// the buffer until we hit a colon or EOF.
				m_iNextStartStateIndex = HQX_LEXER_START_RESET;	// Skip remaining start states
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
	if (!m_bIdentified)
	{
		if (m_dwMaxScanBytes && m_nOffset > m_dwMaxScanBytes)
			rc = LEXER_DONE;

		if (m_dwTextNonHQXThreshold && m_nOffset > m_dwTextNonHQXThreshold)
			rc = LEXER_DONE;
	}

	return (rc);
}


int CHQXLexer::ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset)
{
	return (LEXER_DONE);
}


int CHQXLexer::ConsumeEOF(size_t nOffset)
{
	// Emit an EOF token.
	m_nLexemeOffset = 0;
	m_nOffset = nOffset;
	m_nStartOffset = nOffset;
	return (EmitToken(HQX_TOKEN_EOF));
}


int CHQXLexer::LexComplete(void)
{
	return (LEXER_OK);
}


int CHQXLexer::ProcessComplete(void)
{
	return (m_pParser->ProcessComplete());
}


inline int CHQXLexer::EmitToken(int iTokenID)
{
	int		rc;

	// Make sure we don't try to emit an invalid token ID.
	dec_assert(iTokenID < HQX_TOKEN_LAST);

	// We need to emit whatever token we have in our buffer.
	// Send the emitted token on to the parsing engine.
	m_byBuffer[m_nLexemeOffset] = 0;
	rc = m_pParser->ConsumeToken(iTokenID, (char *)m_byBuffer, m_nLexemeOffset, m_nStartOffset);
	if (rc == PARSER_DONE)
		rc = LEXER_DONE;
	else if (rc == PARSER_IDENTIFIED)
		rc = LEXER_IDENTIFIED;
	else if (rc == PARSER_ERROR)
		rc = LEXER_ERROR;

	// Increment the next token's start offset by the size of the token we
	// just emitted.
	m_nStartOffset += m_nLexemeOffset;

	// Reset the buffer index to 0 to start building up the next token.
	m_nLexemeOffset = 0;

	// Reset the state machine to the first start state.
	m_State = g_iHQXLexerStartStates[0];
	return (rc);
}


int CHQXLexer::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	return (m_pParser->FindFirstEntry(fpin, pData));
}


int CHQXLexer::FindNextEntry(TEXT_FIND_DATA *pData)
{
	return (m_pParser->FindNextEntry(pData));
}


int CHQXLexer::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	// Not used by this engine.
	return LEXER_OK;
}


int CHQXLexer::ExtractEntry(char *pszFilename, FILE *fpin)
{
	return (m_pParser->ExtractEntry(pszFilename, fpin));
}


int CHQXLexer::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	return (m_pParser->ReplaceEntry(pData, pbKeepChildDataFile, pbKeepChildNewDataFile));
}


int CHQXLexer::AddEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->AddEntry(pData));
}


int CHQXLexer::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->DeleteEntry(pData));
}
