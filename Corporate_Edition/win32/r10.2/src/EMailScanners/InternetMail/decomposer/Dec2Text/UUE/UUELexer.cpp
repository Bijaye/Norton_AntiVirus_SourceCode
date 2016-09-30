// UUELexer.cpp : Lexer for the UUE engine
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
#include "UUE.h"

#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"

#define UUE_LEXER_START_STATE_COUNT	  9

// Reset value (anything above the value of UUE_LEXER_START_STATE_COUNT will do)
#define UUE_LEXER_START_RESET		  100

int g_iUUELexerStartStates[UUE_LEXER_START_STATE_COUNT] =
{
	UUE_LEXER_STATE_CRLF1,
	UUE_LEXER_STATE_LF1,
	UUE_LEXER_STATE_LWSP1,
	UUE_LEXER_STATE_ATOM1
};


typedef struct tagToken
{
	int		iTokenID;
	int		iLexemeCount;
	char	**ppszLexemes;
} TOKEN, *PTOKEN;


CUUELexer::CUUELexer(DWORD dwMaxScanBytes)
{
	m_nOffset = 0;
	m_nStartOffset = 0;
	m_nLexemeOffset = 0;
	m_nForwardOffset = 0;
	m_nLineNumber = 1;
	m_State = g_iUUELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	m_pParser = NULL;
	m_bIdentified = false;
	m_bWantBytes = true;

	//
	// Option defaults
	//
	m_dwMaxScanBytes = dwMaxScanBytes;
	m_dwTextNonUUEThreshold = 1024;
}


CUUELexer::~CUUELexer()
{
	// Destroy the CUUEParser object if we created one.
	if (m_pParser)
		delete m_pParser;
}


bool CUUELexer::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject)
{
	m_nOffset = 0;
	m_nStartOffset = 0;
	m_nLexemeOffset = 0;
	m_nForwardOffset = 0;
	m_nLineNumber = 1;
	m_State = g_iUUELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	m_bIdentified = false;
	m_bWantBytes = true;

	DWORD dwValue = 0;

	//
	// Set any applicable UUE options here
	//
	if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_NONUUETHRESHOLD_BYTES, &dwValue)))
		m_dwTextNonUUEThreshold = dwValue;

	// Construct and attach the CUUEParser object.
	m_pParser = new CUUEParser;
	if (m_pParser == NULL)
		return false;

	return (m_pParser->Init(pArchive, pEngine, pDecomposer));
}


int CUUELexer::GetInterface(void **ppInterface)
{
	if (ppInterface)
	{
		*ppInterface = NULL;
		return (LEXER_OK);
	}

	return (LEXER_ERROR);
}


int CUUELexer::ConsumeByte(unsigned char uchByte)
{
	int		rc = LEXER_OK;	// Assume we want to continue lexing.

	// Make sure we can add this byte to our current token buffer.
	// If not, then emit whatever we have so far as an atom and start over.
	if (m_nLexemeOffset >= sizeof(m_byBuffer))
	{
		rc = EmitToken(UUE_TOKEN_ATOM);
		if (rc != LEXER_OK)
			return (rc);
	}

	// Store the byte in our internal buffer.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	if (m_iNextStartStateIndex >= UUE_LEXER_START_STATE_COUNT)
		m_iNextStartStateIndex = 1;

	// This is the UUE lexer's state machine.
	while (m_iNextStartStateIndex <= UUE_LEXER_START_STATE_COUNT)
	{
		switch (m_State)
		{
			case UUE_LEXER_STATE_CRLF1:
			{
				if (IsCR(uchByte))
				{
					// The byte is a carriage-return character.  Since we need
					// to also recognize the longer sequence CRLF, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = UUE_LEXER_STATE_CRLF2;
					m_iNextStartStateIndex = UUE_LEXER_START_RESET;	// Skip remaining start states
					break;
				}

				// The byte is not a carriage-return character.
				// Move to the next start state.
				m_State = g_iUUELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case UUE_LEXER_STATE_CRLF2:
			{
				if (IsLF(uchByte))
				{
					// The byte is a line-feed character.
					// Emit a NEWLINE token.
					rc = EmitToken(UUE_TOKEN_NEWLINE);
					if (rc != LEXER_OK &&
						rc != LEXER_IDENTIFIED)
						return (rc);

					m_iNextStartStateIndex = UUE_LEXER_START_RESET;	// Skip remaining start states
					break;
				}

				// The byte is not a line-feed character.
				// Remove the last byte added to our buffer by
				// backing up one byte.
				m_nLexemeOffset--;

				// Emit a NEWLINE token.
				rc = EmitToken(UUE_TOKEN_NEWLINE);
				if (rc != LEXER_OK)
					return (rc);

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				break;
			}

			case UUE_LEXER_STATE_LF1:
			{
				if (IsLF(uchByte))
				{
					// The byte is a line-feed character.
					// Emit a NEWLINE token.
					rc = EmitToken(UUE_TOKEN_NEWLINE);
					if (rc != LEXER_OK)
						return (rc);

					m_iNextStartStateIndex = UUE_LEXER_START_RESET;	// Skip remaining start states
					break;
				}

				// The byte is not a line-feed character.
				// Move to the next start state.
				m_State = g_iUUELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case UUE_LEXER_STATE_LWSP1:
			{
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  Since we need
					// to also recognize long sequences of whitespace, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = UUE_LEXER_STATE_LWSP2;
					m_iNextStartStateIndex = UUE_LEXER_START_RESET;	// Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				// Move to the next start state.
				m_State = g_iUUELexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case UUE_LEXER_STATE_LWSP2:
			{
				if (IsSPACE(uchByte) || IsHTAB(uchByte))
				{
					// The byte is a whitespace character.  We stay in this
					// same state until we find a non-whitespace character.
					m_iNextStartStateIndex = UUE_LEXER_START_RESET;	// Skip remaining start states
					break;
				}

				// The byte is not a whitespace character.
				// Remove the last byte added to our buffer by
				// backing up one byte.
				m_nLexemeOffset--;

				// Emit a LWSP token.
				rc = EmitToken(UUE_TOKEN_LWSP);
				if (rc != LEXER_OK)
					return (rc);

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				break;
			}

			case UUE_LEXER_STATE_ATOM1:
			{
				// If the character did not get recognized by any of the
				// previous start states, then we'll call it an atom.
				// Since we need to also recognize long sequences of atom
				// characters, we can't emit anything yet.  Move to the next
				// state and wait for the next input byte.
				m_State = UUE_LEXER_STATE_ATOM2;
				m_iNextStartStateIndex = UUE_LEXER_START_RESET;	// Skip remaining start states
				break;
			}

			case UUE_LEXER_STATE_ATOM2:
			{
				int		iTokenID = UUE_TOKEN_ATOM;

				if (IsCHAR(uchByte) &&
					!IsSPACE(uchByte) &&
					!IsCTL(uchByte))
				{
					// The byte is a valid atom character.  We stay in this
					// same state until we find a non-atom character.
					m_iNextStartStateIndex = UUE_LEXER_START_RESET;	// Skip remaining start states
					break;
				}

				// The byte is not a valid atom character.
				// Remove the last byte added to our buffer by
				// backing up one byte.
				m_nLexemeOffset--;

				// See if we recognize the atom as a keyword.
				if (binary_strnicmp((const char *)&m_byBuffer[0], STR_begin, STR_BEGIN, 5) == 0)
				{
					// Make sure that if we recognize 'begin' that it is the word
					// 'begin' by itself (e.g. not begin_foo_blah or beginning etc...)
					// This means that the word 'begin' must be followed by a whitespace character.
					// Since MS-Outlook also handles case where begin is followed by a NULL byte,
					// we will handle that case as well.
					if (IsSPACE(m_byBuffer[5])	|| 
						IsHTAB(m_byBuffer[5])	|| 
						IsCR(m_byBuffer[5])		|| 
						IsLF(m_byBuffer[5])		||
						m_byBuffer[5] == 0x00)
					{
						iTokenID = UUE_TOKEN_BEGIN;
					}
				}
				else if (binary_strnicmp((const char *)&m_byBuffer[0], STR_end, STR_END, 3) == 0)
					iTokenID = UUE_TOKEN_END;
				else if (binary_strnicmp((const char *)&m_byBuffer[0], STR_table, STR_TABLE, 5) == 0)
					iTokenID = UUE_TOKEN_TABLE;
				else if (binary_strnicmp((const char *)&m_byBuffer[0], STR_section, STR_SECTION, 7) == 0)
					iTokenID = UUE_TOKEN_SECTION;
				else if (binary_strnicmp((const char *)&m_byBuffer[0], STR_sum, STR_SUM, 3) == 0)
					iTokenID = UUE_TOKEN_SUM;

				// Emit the token.
				rc = EmitToken(iTokenID);
				if (rc != LEXER_OK)
					return (rc);

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
	if (!m_bIdentified)
	{
		if (m_dwMaxScanBytes && m_nOffset > m_dwMaxScanBytes)
			rc = LEXER_DONE;

		if (m_dwTextNonUUEThreshold && m_nOffset > m_dwTextNonUUEThreshold)
			rc = LEXER_DONE;
	}

	return (rc);
}


int CUUELexer::ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset)
{
	return (LEXER_OK);
}


int CUUELexer::ConsumeEOF(size_t nOffset)
{
	// Emit an EOF token.
	m_nLexemeOffset = 0;
	m_nOffset = nOffset;
	m_nStartOffset = nOffset;
	return (EmitToken(UUE_TOKEN_EOF));
}


int CUUELexer::LexComplete(void)
{
	return (LEXER_OK);
}


int CUUELexer::ProcessComplete(void)
{
	return (LEXER_OK);
}


inline int CUUELexer::EmitToken(int iTokenID)
{
	int		rc;

	// Make sure we don't try to emit an invalid token ID.
	dec_assert(iTokenID < UUE_TOKEN_LAST);

	// We need to emit whatever token we have in our buffer.
	// Send the emitted token on to the parsing engine.
	m_byBuffer[m_nLexemeOffset] = 0;
	rc = m_pParser->ConsumeToken(iTokenID, (char *)m_byBuffer, m_nLexemeOffset, m_nStartOffset);
	if (rc == PARSER_DONE)
	{
		rc = LEXER_DONE;
	}
	else if (rc == PARSER_IDENTIFIED)
	{
		rc = LEXER_IDENTIFIED_DONE;
		m_bIdentified = true;
	}
	else if (rc == PARSER_ERROR)
	{
		rc = LEXER_ERROR;
	}

	// Increment the next token's start offset by the size of the token we
	// just emitted.
	m_nStartOffset += m_nLexemeOffset;

	// Reset the buffer index to 0 to start building up the next token.
	m_nLexemeOffset = 0;

	// Reset the state machine to the first start state.
	m_State = g_iUUELexerStartStates[0];
	return (rc);
}


int CUUELexer::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	return (m_pParser->FindFirstEntry(fpin, pData));
}


int CUUELexer::FindNextEntry(TEXT_FIND_DATA *pData)
{
	return (m_pParser->FindNextEntry(pData));
}


int CUUELexer::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	return (m_pParser->SkipExtractEntry(pData));
}


int CUUELexer::ExtractEntry(char *pszFilename, FILE *fpin)
{
	return (m_pParser->ExtractEntry(pszFilename, fpin));
}


int CUUELexer::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	return (m_pParser->ReplaceEntry(pData));
}


int CUUELexer::AddEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->AddEntry(pData));
}


int CUUELexer::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->DeleteEntry(pData));
}
