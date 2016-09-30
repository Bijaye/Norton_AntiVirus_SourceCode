// MBOXLexer.cpp : Lexer for the MBOX engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2002, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "mbox.h"

#define MBOX_LEXER_START_STATE_COUNT	  2

// Reset value (anything above the value of MBOX_LEXER_START_STATE_COUNT will do)
#define MBOX_LEXER_START_RESET		  MBOX_LEXER_START_STATE_COUNT + 1

int g_iMBOXLexerStartStates[MBOX_LEXER_START_STATE_COUNT] =
{
	MBOX_LEXER_STATE_CRLF1,
	MBOX_LEXER_STATE_LF1
};


typedef struct tagMBOXToken
{
	int		iTokenID;
	int		iLexemeCount;
	char	**ppszLexemes;
} MBOXTOKEN, *PMBOXTOKEN;


CMBOXLexer::CMBOXLexer(DWORD dwMaxScanBytes)
{
	m_nOffset = 0;
	m_nStartOffset = 0;
	m_nLexemeOffset = 0;
	m_State = MBOX_LEXER_STATE_FROM1;
	m_iNextStartStateIndex = 1;
	m_pParser = NULL;
	m_bIdentified = false;
	m_bWantBytes = true;

	//
	// Option defaults
	//
	m_dwTextNonMBOXThreshold = 4096;
	m_dwMaxScanBytes = dwMaxScanBytes;
}


CMBOXLexer::~CMBOXLexer()
{
	// Destroy the CMBOXParser object if we created one.
	if (m_pParser)
	{
		delete m_pParser;
		m_pParser = NULL;
	}
}


bool CMBOXLexer::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer, IDecContainerObjectEx *pObject)
{
	m_nOffset = 0;
	m_nStartOffset = 0;
	m_nLexemeOffset = 0;
	m_iNextStartStateIndex = 1;
	m_bIdentified = false;
	m_bWantBytes = true;

	// We always start in this state because an mbox file must start with "From ".
	m_State = MBOX_LEXER_STATE_FROM1;

	DWORD dwValue = 0;

	//
	// Set any applicable MBOX lexer options here
	//
	if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_NONMIMETHRESHOLD, &dwValue)))
		m_dwTextNonMBOXThreshold = dwValue;

	m_pParser = new CMBOXParser;
	if (!m_pParser)
		return false;

	return (m_pParser->Init(pArchive, pEngine, pDecomposer));
}


int CMBOXLexer::GetInterface(void **ppInterface)
{
	if (ppInterface)
	{
		*ppInterface = NULL;
		return (LEXER_OK);
	}

	return (LEXER_ERROR);
}


int CMBOXLexer::ConsumeByte(unsigned char uchByte)
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

	if (m_iNextStartStateIndex >= MBOX_LEXER_START_STATE_COUNT)
		m_iNextStartStateIndex = 0;

	// This is the MBOX lexer's state machine.
	while (m_iNextStartStateIndex <= MBOX_LEXER_START_STATE_COUNT)
	{
		switch (m_State)
		{
			case MBOX_LEXER_STATE_CRLF1:
			{
				if (IsCR(uchByte))
				{
					// The byte is a carriage-return character.  Since we need
					// to also recognize the longer sequence CRLF, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MBOX_LEXER_STATE_CRLF2;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a carriage-return character.
				// Move to the next start state.
				m_State = g_iMBOXLexerStartStates[m_iNextStartStateIndex++];
				break;
			}

			case MBOX_LEXER_STATE_CRLF2:
			{
				if (IsLF(uchByte))
				{
					// The byte is a line-feed character.
					// Move to the next state and start looking for the word "FROM".
					m_State = MBOX_LEXER_STATE_FROM1;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a line-feed character.
				// Remove the last byte added to our buffer by backing up one byte.
				m_nLexemeOffset--;

				SkipEmit();

				m_nStartOffset = m_nOffset;	// Set the next token's start location.

				// Now that our buffer has been reset, put this
				// new byte back.
				m_byBuffer[m_nLexemeOffset++] = uchByte;

				// And, finally, go back and analyze the byte from scratch.
				m_State = MBOX_LEXER_STATE_FROM1;
				m_iNextStartStateIndex = 1;
				break;
			}

			case MBOX_LEXER_STATE_LF1:
			{
				if (IsLF(uchByte))
				{
					// The byte is a line-feed character.
					// Move to the next state and start looking for the word "FROM".
					m_State = MBOX_LEXER_STATE_FROM1;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a line-feed character.
				SkipEmit();
				m_State = MBOX_LEXER_STATE_CRLF1;
				m_iNextStartStateIndex = MBOX_LEXER_START_RESET;	// Skip remaining start states
				break;
			}

			case MBOX_LEXER_STATE_FROM1:
			{
				if (uchByte == ASC_CHR_F ||
					uchByte == ASC_CHR_f)
				{
					m_State = MBOX_LEXER_STATE_FROM2;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				/*
				// Allow "From " to be prefixed by spaces
				if( uchByte == ASC_CHR_SPACE ) 
				{
					m_State = MBOX_LEXER_STATE_FROM1;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}
				*/

				if (!m_bIdentified)
				{
					// The first character of the file must be an "F" or "f" to be a valid MBOX file.
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					rc = LEXER_DONE;
					break;
				}

				// This line does not start with "From ".
				SkipEmit();

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				m_State = MBOX_LEXER_STATE_CRLF1;
				break;
			}

			case MBOX_LEXER_STATE_FROM2:
			{
				if (uchByte == ASC_CHR_R ||
					uchByte == ASC_CHR_r)
				{
					m_State = MBOX_LEXER_STATE_FROM3;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// This line does not start with "From ".
				SkipEmit();

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				m_State = MBOX_LEXER_STATE_CRLF1;
				break;
			}

			case MBOX_LEXER_STATE_FROM3:
			{
				if (uchByte == ASC_CHR_O ||
					uchByte == ASC_CHR_o)
				{
					m_State = MBOX_LEXER_STATE_FROM4;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// This line does not start with "From ".
				SkipEmit();

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				m_State = MBOX_LEXER_STATE_CRLF1;
				break;
			}

			case MBOX_LEXER_STATE_FROM4:
			{
				if (uchByte == ASC_CHR_M ||
					uchByte == ASC_CHR_m)
				{
					m_State = MBOX_LEXER_STATE_SPACE1;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// This line does not start with "From ".
				SkipEmit();

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				m_State = MBOX_LEXER_STATE_CRLF1;
				break;
			}

			case MBOX_LEXER_STATE_SPACE1:
			{
				if (IsSPACE(uchByte))
				{
					// The byte is a space character.
					// We now have a "From " sequence.
					// If this is not the first "From " we have seen, emit
					// an end of entry marker.
					if (m_bIdentified)
					{
						// Emit an ENTRY_END token.
						rc = EmitToken(MBOX_TOKEN_ENTRY_END);
						if (rc == LEXER_ERROR)
							return (rc);
					}

					// Emit an MBOX_TOKEN_FROM_ token.
					rc = EmitToken(MBOX_TOKEN_FROM_);
					if (rc == LEXER_ERROR)
						return (rc);

					m_State = MBOX_LEXER_STATE_HEADERCRLF1;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				// The byte is not a space character so the line does not start with "From ".
				SkipEmit();

				// And, finally, go back and analyze the byte from scratch.
				m_iNextStartStateIndex = 1;
				m_State = MBOX_LEXER_STATE_CRLF1;
				break;
			}

			case MBOX_LEXER_STATE_HEADERCRLF1:
			{
				if (IsCR(uchByte))
				{
					// The byte is a carriage-return character.  Since we need
					// to also recognize the longer sequence CRLF, we can't emit
					// anything yet.  Move to the next state and wait for the
					// next input byte.
					m_State = MBOX_LEXER_STATE_HEADERCRLF2;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if (IsLF(uchByte))
				{
					rc = EmitToken(MBOX_TOKEN_FROM_LINE_LF);
					if (rc == LEXER_ERROR)
						return (rc);

					// The byte is a line-feed character.
					// Emit an ENTRY_START token.
					rc = EmitToken(MBOX_TOKEN_ENTRY_START);

					if (rc == LEXER_ERROR)
					{
						return (rc);
					}

					if (rc == LEXER_IDENTIFIED || rc == LEXER_IDENTIFIED_DONE)
					{
						m_bIdentified = true;
					}

					// Move to the next state and start looking for the word "FROM".
					m_State = MBOX_LEXER_STATE_FROM1;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				if( IsSPACE(uchByte) || IsHTAB(uchByte) )
				{
					rc = EmitToken(MBOX_TOKEN_HEADER_ATOM);

					if (rc == LEXER_ERROR)
					{
						return (rc);
					}
				}

				m_State = MBOX_LEXER_STATE_HEADERCRLF1;  // Stay in this same state.
				m_iNextStartStateIndex = MBOX_LEXER_START_RESET;	// Skip remaining start states
				break;
			}

			case MBOX_LEXER_STATE_HEADERCRLF2:
			{
				if (IsLF(uchByte))
				{
					rc = EmitToken(MBOX_TOKEN_FROM_LINE_LF);

					if (rc == LEXER_ERROR)
					{
						return (rc);
					}

					// The byte is a line-feed character.
					// Emit an ENTRY_START token.
					rc = EmitToken(MBOX_TOKEN_ENTRY_START);

					if (rc == LEXER_ERROR)
					{
						return (rc);
					}

					if (rc == LEXER_IDENTIFIED || rc == LEXER_IDENTIFIED_DONE)
					{
						m_bIdentified = true;
					}

					// Move to the next state and start looking for the word "FROM".
					m_State = MBOX_LEXER_STATE_FROM1;
					m_iNextStartStateIndex = MBOX_LEXER_START_RESET;  // Skip remaining start states
					break;
				}

				SkipEmit();
				m_State = MBOX_LEXER_STATE_HEADERCRLF1;  // Stay in this same state.
				m_iNextStartStateIndex = MBOX_LEXER_START_RESET;	// Skip remaining start states
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

		if (m_dwTextNonMBOXThreshold && m_nOffset > m_dwTextNonMBOXThreshold)
			rc = LEXER_DONE;
	}

	return (rc);
}


int CMBOXLexer::ConsumeBuffer(unsigned char *pBuffer, size_t *pnBufferSize, int *piBufferIndex, size_t *pnOffset)
{
	return (LEXER_OK);
}


int CMBOXLexer::ConsumeEOF(size_t nOffset)
{
	// Emit an EOF token.
	m_nLexemeOffset = 0;
	m_nOffset = nOffset;
	m_nStartOffset = nOffset;
	return (EmitToken(MBOX_TOKEN_EOF));
}


int CMBOXLexer::LexComplete(void)
{
	return (LEXER_OK);
}


int CMBOXLexer::ProcessComplete(void)
{
	return (m_pParser->ProcessComplete());
}


inline int CMBOXLexer::SkipEmit(void)
{
	// Increment the next token's start offset by the size of the buffer we
	// just skipped over.
	m_nStartOffset += m_nLexemeOffset;

	// Reset the buffer index to 0 to start building up the next token.
	m_nLexemeOffset = 0;

	return (LEXER_OK);
}


inline int CMBOXLexer::EmitToken(int iTokenID)
{
	int		rc;

	// Make sure we don't try to emit an invalid token ID.
	dec_assert(iTokenID < MBOX_TOKEN_LAST);

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
	m_State = g_iMBOXLexerStartStates[0];
	return (rc);
}


int CMBOXLexer::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	return (m_pParser->FindFirstEntry(fpin, pData));
}


int CMBOXLexer::FindNextEntry(TEXT_FIND_DATA *pData)
{
	return (m_pParser->FindNextEntry(pData));
}


int CMBOXLexer::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	// Not used by this engine.
	return LEXER_OK;
}


int CMBOXLexer::ExtractEntry(char *pszFilename, FILE *fpin)
{
	return (m_pParser->ExtractEntry(pszFilename, fpin));
}


int CMBOXLexer::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	return (m_pParser->ReplaceEntry(pData, pbKeepChildDataFile, pbKeepChildNewDataFile));
}


int CMBOXLexer::AddEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->AddEntry(pData));
}


int CMBOXLexer::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	return (m_pParser->DeleteEntry(pData));
}
