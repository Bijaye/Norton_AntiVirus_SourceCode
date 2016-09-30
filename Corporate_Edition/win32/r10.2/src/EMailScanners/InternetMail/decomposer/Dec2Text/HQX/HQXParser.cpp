// HQXParser.cpp : Parser for the HQX engine
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
#include "dec_assert.h"

#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"


CHQXParser::CHQXParser()
{
	m_hrLastDataForkErr = DEC_OK;
	m_hrLastResourceForkErr = DEC_OK;
	m_pDecomposer = NULL;
	m_pEngine = NULL;
	m_pArchive = NULL;
	m_pHQXArchive = NULL;
	m_fpArchive = NULL;
	m_ParseState = HQX_PARSER_STATE_BODY1;
	m_pszDataForkName = NULL;
	m_pszResourceForkName = NULL;
	m_pszReplacementName = NULL;
	m_nBodyStartOffset = 0;
	m_nBodyEndOffset = 0;
}


CHQXParser::~CHQXParser()
{
	if (m_pHQXArchive)
	{
		delete m_pHQXArchive;
		m_pHQXArchive = NULL;
	}

	if(m_pszDataForkName)
	{
		free(m_pszDataForkName);
		m_pszDataForkName = NULL;
	}

	if(m_pszResourceForkName)
	{
		free(m_pszResourceForkName);
		m_pszResourceForkName = NULL;
	}

	if(m_pszReplacementName)
	{
		free(m_pszReplacementName);
		m_pszReplacementName = NULL;
	}
}


bool CHQXParser::Init(CTextFile *pArchive, CTextEngine *pEngine, IDecomposerEx *pDecomposer)
{
	m_pArchive = pArchive;
	m_pEngine = pEngine;
	m_pDecomposer = pDecomposer;
	m_ParseState = HQX_PARSER_STATE_BODY1;
	m_pszDataForkName = NULL;
	m_pszResourceForkName = NULL;
	m_pszReplacementName = NULL;
	m_nBodyStartOffset = 0;
	m_nBodyEndOffset = 0;

	DWORD dwValue = 0;
	bool bEnforceCRC = false;

	//
	// Set any applicable HQX archive options here
	//
	if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_ENFORCE_CRC, &dwValue)))
		bEnforceCRC = (dwValue ? true : false);

	// Create new HQX archive object.  This object handles all the decoding/encoding/extraction.
	m_pHQXArchive = new CHQXFile(bEnforceCRC);
	if (!m_pHQXArchive)
		return false;

	return true;
}


int CHQXParser::ConsumeToken(
	int iTokenID,
	char *pString,
	size_t nStringLength,
	size_t nOffset)
{
	int		rc = PARSER_OK;	// Assume parsing needs to continue

	// Copy the input parameters into the parser object.  These parameters
	// now become the things that drive the state machine.
	m_iTokenID = iTokenID;
	m_pString = pString;
	m_nStringLength = nStringLength;
	m_nOffset = nOffset;

	// See if the input token is valid for the state that we are
	// currently in.  If not, then we need to stop parsing because the data
	// is not in a form that this engine is designed to handle.  We check
	// this by simply consulting a matrix of HQX_PARSER_STATE_xxx vs.
	// HQX_TOKEN_xxx.
	// For debug purposes, make sure the input token ID and current state
	// are within the range of the matrix.
	dec_assert(iTokenID < HQX_TOKEN_LAST);
	dec_assert(m_ParseState < HQX_PARSER_STATE_LAST);

	// Always reset the entire emit structure here.  These fields may
	// get modified as we progress through the state machine.
	EMIT_OFFSET = m_nOffset;
	EMIT_ID = m_iTokenID;
	EMIT_FLAGS = 0;
	EMIT_DATA = 0;

	// Where we go next depends on what state the parser is in right now.
	// The parsing always begins in state HQX_PARSER_STATE_HEADERFIELD1.
	// Call the function associated with the current parse state.
	switch (m_ParseState)
	{
		case HQX_PARSER_STATE_BODY1:
			rc = State_BODY1();
			break;

		case HQX_PARSER_STATE_BODY2:
			rc = State_BODY2();
			break;

		default:
			dec_assert(0);	// We should never fall outside the state machine!
			break;
	}

	return (rc);
}


int CHQXParser::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	dec_assert(pData);

	if (!fpin)
		return (LEXER_ERROR);

	// All BinHex filenames are considered to be in the CP850 character set.
	m_pArchive->m_dwFilenameCharset = DEC_CHARSET_CP850;

	m_fpArchive = fpin;
	m_iCurrentFork = HQX_FORK_DATA;

	// Reset the primary and secondary name strings.
	pData->pszName = NULL;
	pData->pszSecondaryName = NULL;

	// Extract both the data fork and the resource fork into two
	// separate files.  ExtractForks will return the TEXT_FIND_DATA
	// information for the data fork.
	return (ExtractForks(pData));
}


int CHQXParser::FindNextEntry(TEXT_FIND_DATA *pData)
{
	char		*pszPrimaryName = NULL;
	DWORD		dwCharset = DEC_CHARSET_ASCII;
	DWORDLONG	dwlSize;
	DECRESULT	hr;

	dec_assert(pData);

	if (m_iCurrentFork != HQX_FORK_DATA)
		return LEXER_ERROR;

	m_iCurrentFork = HQX_FORK_RESOURCE;

	// Reset the primary and secondary name strings.
	pData->pszName = NULL;
	pData->pszSecondaryName = NULL;

	pszPrimaryName = (char *)malloc(MAX_PATH);
	if (!pszPrimaryName)
		return LEXER_ERROR;

	hr = m_pHQXArchive->GetEntryName(HQX_FORK_RESOURCE, pszPrimaryName, MAX_PATH);
	if (FAILED(hr))
	{
		// Use ASCII "Unknown.data" if we could not get a name.
		strcat(pszPrimaryName, "\x55\x6E\x6B\x6E\x6F\x77\x6E\x2E\x64\x61\x74\x61");
	}

	// Fill in the caller's TEXT_FIND_DATA structure.
	pData->dwAttributes = 0;
	pData->dwAttrType = DEC_ATTRTYPE_DOS;
	pData->dwYear = 1980;
	pData->dwMonth = 1;
	pData->dwDay = 1;
	pData->dwHour = 0;
	pData->dwMinute = 0;
	pData->dwSecond = 0;
	pData->dwMillisecond = 0;
	pData->dwType = DEC_ITEMTYPE_NORMAL;
	pData->nCompressedSize = (size_t)-1;
	pData->nUncompressedSize = (size_t)-1;

	hr = m_pHQXArchive->GetEntryCompressedSize(HQX_FORK_RESOURCE, &dwlSize);
	if (!FAILED(hr))
		pData->nCompressedSize = (size_t)dwlSize;

	hr = m_pHQXArchive->GetEntryUncompressedSize(HQX_FORK_RESOURCE, &dwlSize);
	if (!FAILED(hr))
		pData->nUncompressedSize = (size_t)dwlSize;

	pData->pszName = pszPrimaryName;
	pData->dwNameCharset = DEC_CHARSET_CP850;

	// Return the enumerated resource fork to the caller.
	return LEXER_OK;
}


int CHQXParser::ExtractForks(TEXT_FIND_DATA *pData)
{
	int			rc = LEXER_ERROR;  // Assume error.
	DECRESULT	hr;
	DWORDLONG	dwlSize;
	char		*pszPrimaryName = NULL;

	// Extract both forks.
	m_pszDataForkName = (char *)malloc(MAX_PATH);
	if (!m_pszDataForkName)
		goto done_extractforks;

	m_pszResourceForkName = (char *)malloc(MAX_PATH);
	if (!m_pszResourceForkName)
		goto done_extractforks;

	// Create a temporary file for the data fork.
	hr = m_pEngine->TextCreateTempFile(".dat", m_pszDataForkName, m_pDecomposer);
	if (FAILED(hr))
		goto done_extractforks;

	// Create a temporary file for the resource fork.
	hr = m_pEngine->TextCreateTempFile(".dat", m_pszResourceForkName, m_pDecomposer);
	if (FAILED(hr))
		goto done_extractforks;

	//
	// Reset last error state
	//
	m_hrLastDataForkErr = DEC_OK;
	m_hrLastResourceForkErr = DEC_OK;

	hr = m_pHQXArchive->ExtractBothForks(m_fpArchive, m_nBodyStartOffset,
										m_pszDataForkName, m_pszResourceForkName,
										m_hrLastDataForkErr, m_hrLastResourceForkErr,
										&m_nBodyEndOffset);
	//
	// We want to cache any fork specific errors (m_hrLastDataForkErr or m_hrLastResourceForkErr).
	// For general errors (hr), we will report those immediately.  Fork specific
	// errors will be reported when an extraction attempt is made.
	//
	if (FAILED(hr))
	{
		// Real error - bail now
		goto done_extractforks;
	}

	pszPrimaryName = (char *)malloc(MAX_PATH);
	if (!pszPrimaryName)
		goto done_extractforks;

	hr = m_pHQXArchive->GetEntryName(HQX_FORK_DATA, pszPrimaryName, MAX_PATH);
	if (FAILED(hr))
	{
		// Use ASCII "Unknown.data" if we could not get a name.
		strcpy(pszPrimaryName, "\x55\x6E\x6B\x6E\x6F\x77\x6E\x2E\x64\x61\x74\x61");
	}

	// Fill in the caller's TEXT_FIND_DATA structure.
	pData->dwAttributes = 0;
	pData->dwAttrType = DEC_ATTRTYPE_DOS;
	pData->dwYear = 1980;
	pData->dwMonth = 1;
	pData->dwDay = 1;
	pData->dwHour = 0;
	pData->dwMinute = 0;
	pData->dwSecond = 0;
	pData->dwMillisecond = 0;
	pData->dwType = DEC_ITEMTYPE_NORMAL;
	pData->nCompressedSize = (size_t)-1;
	pData->nUncompressedSize = (size_t)-1;

	hr = m_pHQXArchive->GetEntryCompressedSize(HQX_FORK_DATA, &dwlSize);
	if (!FAILED(hr))
		pData->nCompressedSize = (size_t)dwlSize;

	hr = m_pHQXArchive->GetEntryUncompressedSize(HQX_FORK_DATA, &dwlSize);
	if (!FAILED(hr))
		pData->nUncompressedSize = (size_t)dwlSize;

	pData->pszName = pszPrimaryName;
	pData->dwNameCharset = DEC_CHARSET_CP850;
	rc = LEXER_OK;	// This is already set.
	// Return the enumerated block of data to the caller.

done_extractforks:
	if (rc != LEXER_OK)
	{
		// Delete the data fork file if extraction failed.
		if (m_pszDataForkName)
		{
			dec_remove(m_pszDataForkName);
			free(m_pszDataForkName);
			m_pszDataForkName = NULL;
		}

		// Delete the resource fork file if extraction failed.
		if (m_pszResourceForkName)
		{
			dec_remove(m_pszResourceForkName);
			free(m_pszResourceForkName);
			m_pszResourceForkName = NULL;
		}

		if (pszPrimaryName)
			free(pszPrimaryName);
	}

	return (rc);
}


#define HQX_EXTRACT_BUFFER_SIZE	  (8 * 1024)

int CHQXParser::ExtractEntry(char *pszFilename, FILE *fpin)
{
	char	*pszForkName;

	// Determine which fork is currently being enumerated.
	// If either fork has a cached error (stored during 
	// FindFirstEntry) then map it to its correct LEXER_ERR
	// and return the error now.
	if (m_iCurrentFork <= HQX_FORK_DATA)
	{
		if(FAILED(m_hrLastDataForkErr))
		{
			if(DECERR_CHILD_MAX_SIZE == m_hrLastDataForkErr)
			{
				return LEXER_MAX_EXTRACT_SIZE;
			}

			else if(DECERR_USER_CANCEL == m_hrLastDataForkErr)
			{
				return LEXER_USER_ABORT;
			}

			else
			{
				// Some other non-specific error
				return LEXER_ERROR;
			}
		}

		pszForkName = m_pszDataForkName;
	}

	else
	{
		if(FAILED(m_hrLastResourceForkErr))
		{
			if(DECERR_CHILD_MAX_SIZE == m_hrLastResourceForkErr)
			{
				return LEXER_MAX_EXTRACT_SIZE;
			}

			else if(DECERR_USER_CANCEL == m_hrLastResourceForkErr)
			{
				return LEXER_USER_ABORT;
			}

			else
			{
				// Some other non-specific error
				return LEXER_ERROR;
			}
		}

		pszForkName = m_pszResourceForkName;
	}

	// Copy the already extracted fork to pszFilename.
	if (!Support_CopyFile(pszForkName, pszFilename, false))
		return LEXER_ERROR;

	return LEXER_OK;
}


int CHQXParser::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	DECRESULT	hr;

	hr = m_pHQXArchive->ReplaceEntry(m_iCurrentFork, 
									pData->pszFilename, 
									pData->bRenameItem ? pData->pszName : NULL);
	if (FAILED(hr))
		return LEXER_ERROR;

	// To replace the entry, we need to mark the body of the BinHex data
	// for replacement.  Since we can not finish preparing the new
	// body data file until after we have finished processing both
	// forks, the replacement file is empty at this point.  It gets
	// written during the ProcessComplete() call.
	if (!m_pszReplacementName)
	{
		m_pszReplacementName = (char *)malloc(MAX_PATH);
		if (!m_pszReplacementName)
			return LEXER_ERROR;

		hr = m_pEngine->TextCreateTempFile(".rep", m_pszReplacementName, m_pDecomposer);
		if (FAILED(hr))
		{
			free(m_pszReplacementName);
			return LEXER_ERROR;
		}

		pData->pszFilename = m_pszReplacementName;
		pData->nStartOffset = m_nBodyStartOffset;
		pData->nEndOffset = m_nBodyEndOffset;
		hr = m_pArchive->MarkForReplace(pData);
		if (FAILED(hr))
			return LEXER_ERROR;
	}

	//
	// Decide which files we need to keep...
	//
	if (pbKeepChildDataFile)
		*pbKeepChildDataFile = true;
	if (pbKeepChildNewDataFile)
		*pbKeepChildNewDataFile = true;

	return LEXER_OK;
}


int CHQXParser::AddEntry(TEXT_CHANGE_DATA *pData)
{
	// This function is not supported for BinHex.
	return LEXER_ERROR;
}


int CHQXParser::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	DECRESULT	hr;
	char		*pszOutputFilename = NULL;

	hr = m_pHQXArchive->DeleteEntry(m_iCurrentFork);
	if (FAILED(hr))
		return LEXER_ERROR;

	// To delete the entry, we need to mark the body of the BinHex data
	// for replacement.  Since we can not finish preparing the new
	// body data file until after we have finished processing both
	// forks, the replacement file is empty at this point.  It gets
	// written during the ProcessComplete() call.
	if (!m_pszReplacementName)
	{
		m_pszReplacementName = (char *)malloc(MAX_PATH);
		if (!m_pszReplacementName)
			return LEXER_ERROR;

		hr = m_pEngine->TextCreateTempFile(".rep", m_pszReplacementName, m_pDecomposer);
		if (FAILED(hr))
		{
			free(m_pszReplacementName);
			return LEXER_ERROR;
		}

		pData->pszFilename = m_pszReplacementName;
		pData->nStartOffset = m_nBodyStartOffset;
		pData->nEndOffset = m_nBodyEndOffset;
		hr = m_pArchive->MarkForReplace(pData);
		if (FAILED(hr))
			return LEXER_ERROR;
	}

	return LEXER_OK;
}


int CHQXParser::ProcessComplete(void)
{
	DECRESULT	hr;

	hr = m_pHQXArchive->Close(m_pszReplacementName);

	return (FAILED(hr) ? LEXER_ERROR : LEXER_OK);
}


inline int CHQXParser::State_BODY1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	switch (m_iTokenID)
	{
		case HQX_TOKEN_EOF:
			break;

		case HQX_TOKEN_BODY_START:
			m_nBodyStartOffset = m_nOffset + 1;
			m_ParseState = HQX_PARSER_STATE_BODY2;
			break;

		default:
			dec_assert(0);	// No other tokens should be received here.
			break;
	}

	return (rc);
}


inline int CHQXParser::State_BODY2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	switch (m_iTokenID)
	{
		case HQX_TOKEN_EOF:
			break;

		case HQX_TOKEN_BODY_DATA:
		{
			char	*phdr;
			size_t	nBufferSize;

			phdr = m_pString;
			nBufferSize = m_nStringLength;
			if (m_pHQXArchive->DecodeHeader(&phdr, &nBufferSize))
				rc = PARSER_IDENTIFIED;
			break;
		}

		case HQX_TOKEN_BODY_END:
			m_nBodyEndOffset = m_nOffset - 2;
			m_ParseState = HQX_PARSER_STATE_BODY1;
			rc = PARSER_DONE;
			break;

		default:
			break;
	}

	return (rc);
}
