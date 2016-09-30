// MBOXFile.cpp : MBOX file manager
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2002, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "dec_assert.h"

// Project headers
#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Support.h"
#include "Decstdio.h"

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

#include "mboxFile.h"
#include "asc_char.h"

extern volatile DWORD g_dwTextMaxExtractSize;
extern volatile bool g_bTextAbort;


/////////////////////////////////////////////////////////////////////////
// CMBOXFile methods

/////////////////////////////////////////////////////////////////////////
// CMBOXFile::CMBOXFile()

CMBOXFile::CMBOXFile(bool bEnforceCRC)
{
	m_pInputFile = NULL;
	m_pMBOXHead = NULL;
}


/////////////////////////////////////////////////////////////////////////
// CMBOXFile::~CMBOXFile()

CMBOXFile::~CMBOXFile()
{
}


/////////////////////////////////////////////////////////////////////////
// CMBOXFile::Open()

DECRESULT CMBOXFile::Open(FILE *fpin)
{
	DECRESULT	hr = DECERR_CONTAINER_OPEN;	// Assume failure.

	m_pInputFile = NULL;

	if (!fpin)
		goto done_open;

	m_pInputFile = fpin;
	hr = DEC_OK;

done_open:
	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CMBOXFile::Close()

DECRESULT CMBOXFile::Close(char *pszOutputFile)
{
	DECRESULT hr = DEC_OK;
	MBOXENTRY	*pEntry;
	MBOXENTRY	*pNext;

	// We do NOT close the input file here because it is owned by the
	// text engine.
	m_pInputFile = NULL;

	// Free all of the nodes in the m_pMBOXHead linked-list.
	pEntry = m_pMBOXHead;
	while (pEntry != NULL)
	{
		pNext = pEntry->pNext;
		free(pEntry);
		pEntry = pNext;
	}

	return hr;
}


DECRESULT CMBOXFile::AddFoundEntry(size_t nStartOffset, size_t nEndOffset)
{
	MBOXENTRY	*pEntry;
	MBOXENTRY	*pNext;

	pEntry = m_pMBOXHead;
	if (pEntry)
	{
		while (pEntry->pNext != NULL)
			pEntry = pEntry->pNext;
	}

	pNext = (MBOXENTRY *)malloc(sizeof(MBOXENTRY));
	if (!pNext)
		return DECERR_OUT_OF_MEMORY;

	pNext->nStartOffset = nStartOffset;
	pNext->nEndOffset = nEndOffset;
	pNext->szFrom[0] = 0;
	pNext->pNext = NULL;

	if (pEntry)
		pEntry->pNext = pNext;
	else
		m_pMBOXHead = pNext;

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMBOXFile::GetEntryName()

DECRESULT CMBOXFile::GetEntryName(size_t nEntry, char *pszName, WORD wNameSize)
{
	size_t		nCount;
	MBOXENTRY	*pEntry;

	if (!pszName)
	{
		dec_assert(0);
		return DECERR_OUT_OF_MEMORY;
	}

	nCount = 0;
	pEntry = m_pMBOXHead;
	while (pEntry != NULL && nCount < nEntry)
	{
		pEntry = pEntry->pNext;
		nCount++;
	}

	if (nCount == nEntry && pEntry != NULL)
	{
		strncpy(pszName, pEntry->szFrom, wNameSize);
		pszName[wNameSize - 1] = 0;
		return DEC_OK;
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////////
// CMBOXFile::SetEntryName()

DECRESULT CMBOXFile::SetEntryName(size_t nEntry, char *pszName)
{
	size_t		nCount;
	MBOXENTRY	*pEntry;

	if (!pszName)
	{
		dec_assert(0);
		return DECERR_OUT_OF_MEMORY;
	}

	nCount = 0;
	pEntry = m_pMBOXHead;
	while (pEntry != NULL && nCount < nEntry)
	{
		pEntry = pEntry->pNext;
		nCount++;
	}

	if (nCount == nEntry && pEntry != NULL)
	{
		strncpy(pEntry->szFrom, pszName, MAX_FROM_LEN);
		pEntry->szFrom[MAX_FROM_LEN] = 0;
		return DEC_OK;
	}

	return DECERR_GENERIC;
}


DECRESULT CMBOXFile::GetEntryOffsets(size_t nEntry, size_t *pnStartOffset, size_t *pnEndOffset)
{
	size_t		nCount;
	MBOXENTRY	*pEntry;

	nCount = 0;
	pEntry = m_pMBOXHead;
	while (pEntry != NULL && nCount < nEntry)
	{
		pEntry = pEntry->pNext;
		nCount++;
	}

	if (nCount == nEntry && pEntry != NULL)
	{
		if (pnStartOffset)
			*pnStartOffset = pEntry->nStartOffset;
		if (pnEndOffset)
			*pnEndOffset = pEntry->nEndOffset;
		return DEC_OK;
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////////
// CMBOXFile::GetEntryUncompressedSize()

DECRESULT CMBOXFile::GetEntryUncompressedSize(size_t nEntry, DWORDLONG *dwlSize)
{
	size_t		nCount;
	MBOXENTRY	*pEntry;

	if (!dwlSize)
		return DECERR_INVALID_PARAMS;

	nCount = 0;
	pEntry = m_pMBOXHead;
	while (pEntry != NULL && nCount < nEntry)
	{
		pEntry = pEntry->pNext;
		nCount++;
	}

	if (nCount == nEntry && pEntry != NULL)
	{
		*dwlSize = (DWORDLONG)(pEntry->nEndOffset - pEntry->nStartOffset);
		return DEC_OK;
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////////
// CMBOXFile::ExtractEntry()

DECRESULT CMBOXFile::ExtractEntry(size_t nEntry, const char *pszFile)
{
	DECRESULT hr = DEC_OK;
	int nReturnCode = MBOX_SUCCESS;

	dec_assert(pszFile != NULL && *pszFile != 0);
	if (pszFile == NULL || *pszFile == 0)
		return DECERR_OUT_OF_MEMORY;

	// Do the extraction.
	nReturnCode = ExtractData(nEntry, pszFile);
	switch (nReturnCode)
	{
		case MBOX_ERROR_MAX_EXTRACT:
			hr = DECERR_CHILD_MAX_SIZE;
			break;

		case MBOX_ERROR_ABORT:
			hr = DECERR_USER_CANCEL;
			break;

		case MBOX_SUCCESS:
			hr = DEC_OK;
			break;

		default:
			hr = DECERR_CHILD_EXTRACT;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMBOXFile::DeleteEntry()

DECRESULT CMBOXFile::DeleteEntry(size_t nEntry)
{
	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMBOXFile::ReplaceEntry()

DECRESULT CMBOXFile::ReplaceEntry(size_t nEntry, const char *pszFile, const char *pszNewEntryName)
{
	if (!pszFile)
		return DECERR_INVALID_PARAMS;

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// Internal methods

//************************************************************************
// int ExtractData(size_t nEntry, const char *pszFile)
//
// Purpose
//
//  This function will extract the current mbox entry to pszFile.
//
// Parameters: 
//  nEntry - Entry number to extract (0-based)
//  *pszFile - File to output the section's data into
//
// Returns:
//
//  MBOX_SUCCESS if the section was successfully extracted
//  An error code otherwise
//
//************************************************************************
int CMBOXFile::ExtractData(size_t nEntry, const char *pszFile)
{
	int		nReturn = MBOX_ERROR_GENERIC;
	size_t	nLen;
	DWORD	dwTotalBytesWritten = 0;
	size_t	nBytesToRead;
	size_t	nBytesRead;
	FILE	*pOutputFile = NULL;
	unsigned char *pOutputBuffer = NULL;
	size_t		nCount;
	MBOXENTRY	*pEntry;

	if (!pszFile)
		return nReturn;

	// Validate input file pointer and open output file
	pOutputFile = dec_fopen(pszFile, "wb");
	if (!m_pInputFile || !pOutputFile)
		goto bailOut;

	// Allocate a buffer large enough to handle the original input with carriage returns added later
	pOutputBuffer = (unsigned char *)malloc(MBOX_OUTPUT_BUFFER_SIZE * 2);
	if (!pOutputBuffer)
		goto bailOut;

	nCount = 0;
	pEntry = m_pMBOXHead;
	while (pEntry != NULL && nCount < nEntry)
	{
		pEntry = pEntry->pNext;
		nCount++;
	}

	if (nCount != nEntry || pEntry == NULL)
		goto bailOut;

	if (dec_fseek(m_pInputFile, pEntry->nStartOffset, SEEK_SET) != 0)
		goto bailOut;

	// Get how much data to extract, and also update our 
	// MBOX structure with the starting offset of the section
	// (used later if we need to change/replace/delete a section.
	nLen = pEntry->nEndOffset - pEntry->nStartOffset;
	while (nLen != 0)
	{
		nBytesToRead = MBOX_OUTPUT_BUFFER_SIZE;
		if (nBytesToRead > nLen)
			nBytesToRead = nLen;

		// Read the data from the mbox file.
		nBytesRead = dec_fread(pOutputBuffer, 1, nBytesToRead, m_pInputFile);
		if (nBytesRead != nBytesToRead)
		{
			nReturn = MBOX_ERROR_EOF;
			goto bailOut;
		}

		bool fPrefixed = false;
		// Add carriage returns before line feeds
		size_t nNewBytes = PrefixCRtoLF( pOutputBuffer, nBytesRead, fPrefixed );

		// Write the data
		if (nNewBytes != dec_fwrite(pOutputBuffer, 1, nNewBytes, pOutputFile))
		{
			nReturn = MBOX_ERROR_EOF;
			goto bailOut;
		}

		nLen -= nBytesRead;
		dwTotalBytesWritten += nBytesRead;

		// See if we've exceeded our max extraction size or been asked to abort...
		if (g_dwTextMaxExtractSize && dwTotalBytesWritten > g_dwTextMaxExtractSize)
		{
			nReturn = MBOX_ERROR_MAX_EXTRACT;
			goto bailOut;
		}

		if (g_bTextAbort)
		{
			nReturn = MBOX_ERROR_ABORT;
			goto bailOut;
		}
	}

	nReturn = MBOX_SUCCESS;

bailOut:
	// Close the output file and clean up
	if (pOutputFile)
	{
		if (EOF == dec_fclose(pOutputFile))
			nReturn = MBOX_ERROR_GENERIC;

		pOutputFile = NULL;
	}

	if (pOutputBuffer)
	{
		free(pOutputBuffer);
		pOutputBuffer = NULL;
	}

	return nReturn;
}
//************************************************************************
//
// Purpose
//
// Adds carriage returns before line feeds in mbox files. This function
// allocates a new buffer, calls a worker function to add carriage returns,
// and then replaces the original buffer contents.
//
// Parameters
//
//		szInput		Input buffer
//		nBytesRead	Number of bytes in the input buffer
//		rfPrefixed	True if carriage returns were added
//
// Returns
//
//		Number of bytes in the buffer
//
//************************************************************************
size_t CMBOXFile::PrefixCRtoLF( unsigned char* szInput, size_t nBytesRead, bool& rfPrefixed )
{
	// The return value
	size_t nBytesOut = 0;

	// Validate input parameters
	if( szInput == NULL || nBytesRead == 0 )
	{
		// Input error
		return nBytesOut;
	}

	// Initialize
	rfPrefixed = false;

	// Allocate a new buffer for the stream containing carriage returns
	unsigned char* szOutput = ( unsigned char * )malloc( MBOX_OUTPUT_BUFFER_SIZE * 2 );

	if ( !szOutput )
	{
		return nBytesRead;
	}

	// Scan the stream adding carriage returns before line feeds
	nBytesOut = AddCarriageReturns( szInput, nBytesRead, szOutput, rfPrefixed );

	if( rfPrefixed )
	{
		// Copy over the modifications
		szInput = ( unsigned char* )memcpy( szInput, szOutput, nBytesOut );

		if( szInput )
		{
			rfPrefixed = true;
		}
	}

	free( szOutput );

	return nBytesOut;
}
//************************************************************************
//
// Purpose
//
// Adds carriage returns before line feeds in mbox files. This will
// suppress malformity warnings when mime children are processed.
//
// Parameters
// 
//		szInput		Input buffer
//		nBytesRead	Number of bytes in the input buffer
//		szOutput		Output buffer with carriage returns added
//		rfPrefixed	True if carriage returns were added
//
// Returns
//
//		Number of bytes in the output buffer
//
//************************************************************************
size_t CMBOXFile::AddCarriageReturns( unsigned char* szInput, const size_t nBytesRead, unsigned char* szOutput, bool& rfPrefixed )
{
	// The return value
	size_t nBytesOut = 0;

	// Validate input parameters
	if( szInput == NULL || szOutput == NULL || nBytesRead == 0 )
	{
		// Input error
		return nBytesOut;
	}

	// Initialize
	rfPrefixed = false;

	// Traverse the input array looking at the current byte
	for( size_t iCurr = 0; iCurr < nBytesRead; iCurr++ )
	{
		// Check our boundary. Note our buffer is twice the expected input size. 
		if( nBytesOut > MBOX_OUTPUT_BUFFER_SIZE * 2 )
		{
			// Error condition
			rfPrefixed = false;
			nBytesOut = 0;
			return nBytesOut;
		}

		// If we have a lf, add a cr, assuming that we don't already have one.
		if( szInput[iCurr] == ASC_CHR_LF )
		{
			// We will examine the previously copied byte, if one is available in the buffer.
			if( nBytesOut > 0 )
			{
				// Only if the previous byte is not a carriage return will we add a cr.
				if( szOutput[nBytesOut - 1] != ASC_CHR_CR )
				{
					// Inject a carriage return into the output buffer
					szOutput[nBytesOut] = ASC_CHR_CR;

					// Increment the output buffer index
					nBytesOut++;

					// Copy over the current input byte
					szOutput[nBytesOut] = szInput[iCurr];

					// Increment the output buffer index
					nBytesOut++;
				}
				else
				{
					// Copy over the current input byte
					szOutput[nBytesOut] = szInput[iCurr];

					// Increment the output buffer index
					nBytesOut++;
				}
			}
			// We do not have a carriage return for this line feed. We know this since there
			// is no previous byte.
			else
			{
					// Inject a carriage return into the output buffer
					szOutput[nBytesOut] = ASC_CHR_CR;

					// Increment the output buffer index
					nBytesOut++;

					// Copy over the current input byte
					szOutput[nBytesOut] = szInput[iCurr];

					// Increment the output buffer index
					nBytesOut++;
			}

			// Changed the stream
			rfPrefixed = true;
		}
		// This byte is not a line feed.
		else
		{
			// Copy the input byte to the output buffer
			szOutput[nBytesOut] = szInput[iCurr];

			// Increment the output buffer index
			nBytesOut++;
		}
	}

	return nBytesOut;
}
