// HQXFile.cpp : HQX file manager
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "dec_assert.h"

#include "SymSaferStrings.h"

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

#include "HQXFile.h"
#include "asc_char.h"
#include "HQXFile.inc"

extern volatile DWORD g_dwTextMaxExtractSize;
extern volatile bool g_bTextAbort;


/////////////////////////////////////////////////////////////////////////
// CHQXFile methods

/////////////////////////////////////////////////////////////////////////
// CHQXFile::CHQXFile()

CHQXFile::CHQXFile(bool bEnforceCRC)
{
	m_nCount = 0;
	m_nEndOffset = 0;
	m_dwPendingChanges = 0;
	m_bChanged = false;
	m_bRealEOF = false;
	m_pInputFile = NULL;
	m_pOutputFile = NULL;
	m_bInRLESection = false;
	m_wCrc = 0;
	m_nLastCharacter = 0;
	m_nIndexIntoDecodedBuffer = -1;
	m_nNumberOfRepeatedCodes = 0;
	m_bEnforceCRC = bEnforceCRC;

	m_sReplacementDataFile[0] = 0;
	m_sReplacementResourceFile[0] = 0;
	m_sNewEntryName[0] = 0;

	m_pszDataForkFile = NULL;
	m_pszResourceForkFile = NULL;
	m_pszOutputFile = NULL;

	memset(&m_hqxHeader, 0, sizeof(HQXHEADER));
	memset(m_szQuad, 0, sizeof(m_szQuad));
	memset(m_uchTri, 0, sizeof(m_uchTri));
}


/////////////////////////////////////////////////////////////////////////
// CHQXFile::~CHQXFile()

CHQXFile::~CHQXFile()
{
}


/////////////////////////////////////////////////////////////////////////
// CHQXFile::Open()

DECRESULT CHQXFile::Open(FILE *fpin, size_t nOffset)
{
	DECRESULT	hr = DECERR_CONTAINER_OPEN;	// Assume failure.

	m_nCount = 0;
	m_nEndOffset = 0;
	m_dwPendingChanges = 0;
	m_bChanged = false;
	m_bRealEOF = false;
	m_pInputFile = NULL;
	m_pOutputFile = NULL;
	m_bInRLESection = false;
	m_wCrc = 0;
	m_nLastCharacter = 0;
	m_nIndexIntoDecodedBuffer = -1;
	m_nNumberOfRepeatedCodes = 0;

	memset(&m_hqxHeader, 0, sizeof(HQXHEADER));
	memset(m_szQuad, 0, sizeof(m_szQuad));
	memset(m_uchTri, 0, sizeof(m_uchTri));

	if (!fpin)
		goto done_open;

	m_pInputFile = fpin;

	// Seek to where the binhex body data starts (nOffset).
	if (dec_fseek(m_pInputFile, nOffset, SEEK_SET) != 0)
		goto done_open;

	if (!DecodeHeader())
		goto done_open;

	hr = DEC_OK;

done_open:
	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CHQXFile::Close()

DECRESULT CHQXFile::Close(char *pszOutputFile)
{
	DECRESULT hr = DEC_OK;

	// Perform any pending changes.  If any of these steps fail, log the
	// failure, but try to clean up as much as possible.
	if (!CommitChanges(pszOutputFile))
		hr = DECERR_CONTAINER_CLOSE;

	// We do NOT close the input file here because it is owned by the
	// text engine.

	if (m_pOutputFile)
	{
		if (EOF == dec_fclose(m_pOutputFile))
			hr = DECERR_CONTAINER_CLOSE;
	}

	// Delete the replacement files, if any...
	if (m_dwPendingChanges & MODIFY_DATA_REPLACE)
	{
		if (m_sReplacementDataFile != NULL)
			dec_remove(m_sReplacementDataFile);
	}

	if (m_dwPendingChanges & MODIFY_RESOURCE_REPLACE)
	{
		if (m_sReplacementResourceFile != NULL)
			dec_remove(m_sReplacementResourceFile);
	}

	// Delete the temp files
	if (m_pszDataForkFile)
	{
		dec_remove(m_pszDataForkFile);
		free(m_pszDataForkFile);
		m_pszDataForkFile = NULL;
	}

	if (m_pszResourceForkFile)
	{
		dec_remove(m_pszResourceForkFile);
		free(m_pszResourceForkFile);
		m_pszResourceForkFile = NULL;
	}

	m_nCount = 0;
	m_dwPendingChanges = 0;
	m_bChanged = false;
	m_bRealEOF = false;
	m_pInputFile = NULL;
	m_pOutputFile = NULL;
	m_bInRLESection = false;
	m_wCrc = 0;
	m_nLastCharacter = 0;
	m_nIndexIntoDecodedBuffer = -1;
	m_nNumberOfRepeatedCodes = 0;

	memset(&m_hqxHeader, 0, sizeof(HQXHEADER));
	memset(m_szQuad, 0, sizeof(m_szQuad));
	memset(m_uchTri, 0, sizeof(m_uchTri));

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CHQXFile::GetEntryName()

DECRESULT CHQXFile::GetEntryName(int iFork, char *pszName, WORD wNameSize)
{
	if (!pszName)
	{
		dec_assert(0);
		return DECERR_OUT_OF_MEMORY;
	}

	if (iFork > HQX_FORK_RESOURCE)
		return DECERR_CONTAINER_ACCESS;

	// For the resource fork, we need to give it a different name.
	// Otherwise, a change/replace/delete on one fork will cause both
	// to be deleted, or perhaps only the data fork when we really want
	// to delete the resource fork...
	switch (iFork)
	{
		case HQX_FORK_DATA:
			strncpy(pszName, m_hqxHeader.szFilename, wNameSize);
			pszName[wNameSize - 1] = 0;
			break;
		
		case HQX_FORK_RESOURCE:
			char pszRscName[MAX_NAME_LEN + 6];
			/* Prepend an ascii _rsc_ to the name */
			sssnprintf(pszRscName, sizeof(pszRscName), "%s%s", "\x5F\x72\x73\x63\x5F", m_hqxHeader.szFilename);
			strncpy(pszName, pszRscName, wNameSize);
			pszName[wNameSize - 1] = 0;
			break;
		
		default:
			return DECERR_CHILD_SIZE_UNKNOWN;
	}

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CHQXFile::GetEntryUncompressedSize()

DECRESULT CHQXFile::GetEntryUncompressedSize(int iFork, DWORDLONG *dwlSize)
{
	if (!dwlSize)
		return DECERR_INVALID_PARAMS;

	switch (iFork)
	{
		case HQX_FORK_DATA:
			*dwlSize = (DWORDLONG)m_hqxHeader.lDataForkLength;
			break;
		
		case HQX_FORK_RESOURCE:
			*dwlSize = (DWORDLONG)m_hqxHeader.lResourceForkLength;
			break;
		
		default:
			return DECERR_CHILD_SIZE_UNKNOWN;
	}

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CHQXFile::GetEntryCompressedSize()

DECRESULT CHQXFile::GetEntryCompressedSize(int iFork, DWORDLONG *dwlSize)
{
	if (!dwlSize)
		return DECERR_INVALID_PARAMS;

	if (iFork > HQX_FORK_RESOURCE)
		return DECERR_CHILD_SIZE_UNKNOWN;

	*dwlSize = DEC_CHILD_SIZE_UNKNOWN;
	return DEC_OK;
}

 
/////////////////////////////////////////////////////////////////////////////
// CHQXFile::ExtractBothForks()

DECRESULT CHQXFile::ExtractBothForks(
	FILE *fp,
	size_t	nOffset,
	const char *pszDataForkChildDataFile, 
	const char *pszResourceForkChildDataFile,
	DECRESULT &hrDataFork,
	DECRESULT &hrResourceFork,
	size_t *pnEndOffset)
{
	DECRESULT hr;
	hr = hrDataFork = hrResourceFork = DEC_OK;

	if (!pszDataForkChildDataFile || !pszResourceForkChildDataFile)
		return DECERR_OUT_OF_MEMORY;

	// Attempt to open the BinHex body.
	hr = Open(fp, nOffset);
	if (FAILED(hr))
		return (hr);

	// Save the names of the temp files...
	if (m_pszDataForkFile)
	{
		free(m_pszDataForkFile);
		m_pszDataForkFile = NULL;
	}

	m_pszDataForkFile = (char *)malloc(strlen(pszDataForkChildDataFile) + 1);
	dec_assert(m_pszDataForkFile != NULL);
	if (!m_pszDataForkFile)
		return DECERR_OUT_OF_MEMORY;

	strcpy(m_pszDataForkFile, pszDataForkChildDataFile);

	if (m_pszResourceForkFile)
	{
		free(m_pszResourceForkFile);
		m_pszResourceForkFile = NULL;
	}

	m_pszResourceForkFile = (char *)malloc(strlen(pszResourceForkChildDataFile) + 1);
	dec_assert(m_pszResourceForkFile != NULL);
	if (!m_pszResourceForkFile)
		return DECERR_OUT_OF_MEMORY;

	strcpy(m_pszResourceForkFile, pszResourceForkChildDataFile);

	// Try to extract the data fork.
	hrDataFork = ExtractFork(HQX_FORK_DATA, m_pszDataForkFile);

	// Try to extract the resource fork.
	hrResourceFork = ExtractFork(HQX_FORK_RESOURCE, m_pszResourceForkFile);

	if (pnEndOffset)
		*pnEndOffset = m_nEndOffset;
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CHQXFile::ExtractFork()

DECRESULT CHQXFile::ExtractFork(int iFork, const char *pszFile)
{
	DECRESULT hr = DEC_OK;
	int nReturnCode = HQX_SUCCESS;

	dec_assert(pszFile != NULL && *pszFile != 0);
	if (pszFile == NULL || *pszFile == 0)
		return DECERR_OUT_OF_MEMORY;

	if (iFork > HQX_FORK_RESOURCE)
		return DECERR_CHILD_ACCESS;

	// Do the extraction.
	nReturnCode = DecodeFork(iFork, pszFile);
	switch (nReturnCode)
	{
		case HQX_ERROR_MAX_EXTRACT:
			hr = DECERR_CHILD_MAX_SIZE;
			break;

		case HQX_ERROR_ABORT:
			hr = DECERR_USER_CANCEL;
			break;

		case HQX_SUCCESS:
			hr = DEC_OK;
			break;

		default:
			hr = DECERR_CHILD_EXTRACT;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CHQXFile::DeleteEntry()

DECRESULT CHQXFile::DeleteEntry(int iFork)
{
	switch (iFork)
	{
		case HQX_FORK_DATA:
			m_dwPendingChanges |= MODIFY_DATA_DELETE;
			break;

		case HQX_FORK_RESOURCE:
			m_dwPendingChanges |= MODIFY_RESOURCE_DELETE;
			break;
		
		default:
			return DECERR_INVALID_DELETE;
	}
	
	m_bChanged = true;
	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CHQXFile::ReplaceEntry()

DECRESULT CHQXFile::ReplaceEntry(int iFork, const char *pszFile, const char *pszNewEntryName)
{
	if (!pszFile)
		return DECERR_INVALID_PARAMS;

	switch (iFork)
	{
		case HQX_FORK_DATA:
			m_dwPendingChanges |= MODIFY_DATA_REPLACE;
			strcpy(m_sReplacementDataFile, pszFile);
			break;

		case HQX_FORK_RESOURCE:
			m_dwPendingChanges |= MODIFY_RESOURCE_REPLACE;
			strcpy(m_sReplacementResourceFile, pszFile);
			break;
		
		default:
			return DECERR_INVALID_REPLACE;
	}

	// Only one entry name in a BinHex file.  Both forks share the name (actually,
	// the resource fork has no name).
	if (pszNewEntryName)
	{
		int nEntryNameLen = strlen(pszNewEntryName);

		// Don't exceed max name length.
		if (nEntryNameLen > MAX_NAME_LEN)
			return DECERR_INVALID_PARAMS;
		
		// Only set this if there is actually something to put
		// in there.
		if (nEntryNameLen)
		{
			m_dwPendingChanges |= MODIFY_RENAME_ITEM;
			strcpy(m_sNewEntryName, pszNewEntryName);
		}
	}

	m_bChanged = true;
	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// Internal methods

// ------------------------------------------------------------------------------------
// This function will attempt to read the BinHex header.  The header looks like this:
// Byte:      Length of FileName (1->63)
// Bytes:     FileName ("Length" bytes plus terminating NULL)
// Long:      Type
// Long:      Creator
// Word:      Flags
// Long:      Length of Data Fork
// Long:      Length of Resource Fork
// Word:      CRC of this header (excluding HQX Identifier)
// ------------------------------------------------------------------------------------
bool CHQXFile::DecodeHeader()
{
	int		nNextChar = 0;
	int		nIndex = 0;
	WORD	wHeaderCrc = 0;
	char	*pszFilename = m_hqxHeader.szFilename;

	if (!m_pInputFile)
		return false;

	// Initialize the CRC to 0.  Each section of a BinHex file
	// (header, data fork, resource fork) has its own CRC.
	m_wCrc = 0;

	// The first decoded byte is supposed to be the length of the original filename.
	nNextChar = GetNextDecodedByte();
	if (HQX_ERROR_EOF == nNextChar)
		return false;

	m_hqxHeader.byFilenameLen = (BYTE)nNextChar;

	// Validate filename length (add one to nFilenameLength to account for trailing NULL)
	if ((m_hqxHeader.byFilenameLen == 0) || (m_hqxHeader.byFilenameLen + 1) > MAX_NAME_LEN)
	{
		// If the filename is == 0 or is larger than MAX_NAME_LEN, then this file is not
		// a valid HQX file.  The check for == 0 will help eliminate some false-positives.
		return false;
	}

	// Read in the filename...
	for (nIndex = 0; nIndex < (m_hqxHeader.byFilenameLen + 1); nIndex++)
	{
		*pszFilename = GetNextDecodedByte();

		// Check for read/decode errors...
		if (HQX_ERROR_EOF == *pszFilename++)
			return false;
	}

	// Next, read in the 'type' field
	if (HQX_SUCCESS == GetLong(&m_hqxHeader.lFileType))
	{
		// Get the 'Creator' field
		if (HQX_SUCCESS == GetLong(&m_hqxHeader.lCreator))
		{
			// Get the 'Flags'
			if (HQX_SUCCESS == GetWord(&m_hqxHeader.wFlags))
			{
				// Get data fork length
				if (HQX_SUCCESS == GetLong(&m_hqxHeader.lDataForkLength))
				{
					// Get resource fork length
					if (HQX_SUCCESS == GetLong(&m_hqxHeader.lResourceForkLength))
					{
						// We need to calculate the CRC of the header thus far.
						// According to spec, we should do two CRC calculations
						// with 0x00 in place of the real CRC, then compare that
						// to the CRC in the file...
						ComputeCRC(0);
						ComputeCRC(0);

						// Get the header CRC
						if (HQX_SUCCESS == GetWord(&wHeaderCrc, false))
						{
							// At this point, everything in the header looks ok.
							// The last check is to make sure the calculated header
							// CRC matches the header CRC in the file.
							// We always check this flag regardless of g_bEnforceCRC.
							// We must check this one just to make sure we have a valid
							// HQX file.
							if (m_wCrc == wHeaderCrc)
								return true;
						}
					}
				}
			}
		}
	}

	return false;
}


// ------------------------------------------------------------------------------------
// This function will attempt to decode the BinHex header.  The header looks like this:
// Byte:      Length of FileName (1->63)
// Bytes:     FileName ("Length" bytes plus terminating NULL)
// Long:      Type
// Long:      Creator
// Word:      Flags
// Long:      Length of Data Fork
// Long:      Length of Resource Fork
// Word:      CRC of this header (excluding HQX Identifier)
// ------------------------------------------------------------------------------------
bool CHQXFile::DecodeHeader(char **pphdr, size_t *pnBufferSize)
{
	int		nNextChar = 0;
	int		nIndex = 0;
	WORD	wHeaderCrc = 0;
	char	*pszFilename = m_hqxHeader.szFilename;

	if (!pphdr || !pnBufferSize)
		return false;

	if (*pnBufferSize < 20)
		return false;

	// Initialize the CRC to 0.  Each section of a BinHex file
	// (header, data fork, resource fork) has its own CRC.
	m_wCrc = 0;

	// The first decoded byte is supposed to be the length of the original filename.
	nNextChar = GetNextDecodedByte(pphdr, pnBufferSize);
	if (HQX_ERROR_EOF == nNextChar)
		return false;

	m_hqxHeader.byFilenameLen = (BYTE)nNextChar;

	// Validate filename length (add one to nFilenameLength to account for trailing NULL)
	if ((m_hqxHeader.byFilenameLen == 0) || (m_hqxHeader.byFilenameLen + 1) > MAX_NAME_LEN)
	{
		// If the filename is == 0 or is larger than MAX_NAME_LEN, then this file is not
		// a valid HQX file.  The check for == 0 will help eliminate some false-positives.
		return false;
	}

	// Read in the filename...
	for (nIndex = 0; nIndex < (m_hqxHeader.byFilenameLen + 1); nIndex++)
	{
		*pszFilename = GetNextDecodedByte(pphdr, pnBufferSize);

		// Check for read/decode errors...
		if (HQX_ERROR_EOF == *pszFilename++)
			return false;
	}

	// Next, read in the 'type' field
	if (HQX_SUCCESS == GetLong(pphdr, pnBufferSize, &m_hqxHeader.lFileType))
	{
		// Get the 'Creator' field
		if (HQX_SUCCESS == GetLong(pphdr, pnBufferSize, &m_hqxHeader.lCreator))
		{
			// Get the 'Flags'
			if (HQX_SUCCESS == GetWord(pphdr, pnBufferSize, &m_hqxHeader.wFlags))
			{
				// Get data fork length
				if (HQX_SUCCESS == GetLong(pphdr, pnBufferSize, &m_hqxHeader.lDataForkLength))
				{
					// Get resource fork length
					if (HQX_SUCCESS == GetLong(pphdr, pnBufferSize, &m_hqxHeader.lResourceForkLength))
					{
						// We need to calculate the CRC of the header thus far.
						// According to spec, we should do two CRC calculations
						// with 0x00 in place of the real CRC, then compare that
						// to the CRC in the file...
						ComputeCRC(0);
						ComputeCRC(0);

						// Get the header CRC
						if (HQX_SUCCESS == GetWord(pphdr, pnBufferSize, &wHeaderCrc, false))
						{
							// At this point, everything in the header looks ok.
							// The last check is to make sure the calculated header
							// CRC matches the header CRC in the file.
							// We always check this flag regardless of g_bEnforceCRC.
							// We must check this one just to make sure we have a valid
							// HQX file.
							if (m_wCrc == wHeaderCrc)
								return true;
						}
					}
				}
			}
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
// This function will decode one byte from the input stream and return it.
// Upon error or EOF, this function returns EOF.
//
// Because HQX files use RLE encoding, we need to account for repeated characters.
// We will do that here in this function.
//
// If bComputeCRC is false, then we do NOT add the next char into the CRC
// calculation.  This is used because some chars are not to be considered
// in computing the CRC (the embedded CRC for the header, data, and resource
// forks for example).
// -----------------------------------------------------------------------------
int CHQXFile::GetNextDecodedByte(bool bComputeCRC)
{
	// Are we in an RLE section?
	if (m_bInRLESection)
	{
		// Do we have any more repeated codes to return?
		if (--m_nNumberOfRepeatedCodes > 0)
		{
			// Keep a running tally of the CRC
			if (bComputeCRC)
				ComputeCRC((unsigned int)m_nLastCharacter);

			return m_nLastCharacter;
		}

		// Nope, we're done...
		m_bInRLESection = false;
	}

	// See if we need to refill the decoded character buffer...
	if ((m_nIndexIntoDecodedBuffer < 0) || (m_nIndexIntoDecodedBuffer >= LEN_DECODED_CHAR_BUFF))
	{
		if (HQX_SUCCESS != FillInputBuffer() && m_bRealEOF)
			return HQX_ERROR_EOF;
	}

	// See if we are starting an RLE section
	if (RLE_DELIMITER == m_uchTri[m_nIndexIntoDecodedBuffer])
	{
		// It looks like we are, so get the next byte so we know how many times
		// to repeat this code.  We might need to refill the buffer here...  
		m_nIndexIntoDecodedBuffer++;

		if ((m_nIndexIntoDecodedBuffer < 0) || (m_nIndexIntoDecodedBuffer >= LEN_DECODED_CHAR_BUFF))
		{
			if (HQX_SUCCESS != FillInputBuffer() && m_bRealEOF)
				return HQX_ERROR_EOF;
		}

		m_nNumberOfRepeatedCodes = m_uchTri[m_nIndexIntoDecodedBuffer++];	

		// SPECIAL CASE!
		// If m_nNumberOfRepeatedCodes == 0, then it is not truly an RLE
		// delimiter, but rather a literal 0x90, so just
		// return it...
		if (0 == m_nNumberOfRepeatedCodes)
		{
			m_nLastCharacter = RLE_DELIMITER;

			if (bComputeCRC)
				ComputeCRC((unsigned int)m_nLastCharacter);

			return m_nLastCharacter;
		}

		// Since the last character we returned prior to finding the delimiter
		// was the first in a RLE series, we need to decrement the number of 
		// repeated codes by 1.
		m_nNumberOfRepeatedCodes--;

		m_bInRLESection = true;
		
		if (bComputeCRC)
			ComputeCRC((unsigned int)m_nLastCharacter);
		
		return m_nLastCharacter;
	}
	
	// Keep a running tally of the CRC
	if (bComputeCRC)
		ComputeCRC((unsigned int)m_uchTri[m_nIndexIntoDecodedBuffer]);

	m_nLastCharacter = m_uchTri[m_nIndexIntoDecodedBuffer++];
	return m_nLastCharacter;
}


// -----------------------------------------------------------------------------
// This function will decode one byte from the input buffer and return it.
// Upon error or EOF, this function returns EOF.
//
// Because HQX files use RLE encoding, we need to account for repeated characters.
// We will do that here in this function.
//
// If bComputeCRC is false, then we do NOT add the next char into the CRC
// calculation.  This is used because some chars are not to be considered
// in computing the CRC (the embedded CRC for the header, data, and resource
// forks for example).
// -----------------------------------------------------------------------------
int CHQXFile::GetNextDecodedByte(char **pptr, size_t *pnBufferSize, bool bComputeCRC)
{
	// Are we in an RLE section?
	if (m_bInRLESection)
	{
		// Do we have any more repeated codes to return?
		if (--m_nNumberOfRepeatedCodes > 0)
		{
			// Keep a running tally of the CRC
			if (bComputeCRC)
				ComputeCRC((unsigned int)m_nLastCharacter);

			return m_nLastCharacter;
		}

		// Nope, we're done...
		m_bInRLESection = false;
	}

	// See if we need to refill the decoded character buffer...
	if ((m_nIndexIntoDecodedBuffer < 0) || (m_nIndexIntoDecodedBuffer >= LEN_DECODED_CHAR_BUFF))
	{
		if (HQX_SUCCESS != FillInputBuffer(pptr, pnBufferSize) && m_bRealEOF)
			return HQX_ERROR_EOF;
	}

	// See if we are starting an RLE section
	if (RLE_DELIMITER == m_uchTri[m_nIndexIntoDecodedBuffer])
	{
		// It looks like we are, so get the next byte so we know how many times
		// to repeat this code.  We might need to refill the buffer here...  
		m_nIndexIntoDecodedBuffer++;

		if ((m_nIndexIntoDecodedBuffer < 0) || (m_nIndexIntoDecodedBuffer >= LEN_DECODED_CHAR_BUFF))
		{
			if (HQX_SUCCESS != FillInputBuffer(pptr, pnBufferSize) && m_bRealEOF)
				return HQX_ERROR_EOF;
		}

		m_nNumberOfRepeatedCodes = m_uchTri[m_nIndexIntoDecodedBuffer++];	

		// SPECIAL CASE!
		// If m_nNumberOfRepeatedCodes == 0, then it is not truly an RLE
		// delimiter, but rather a literal 0x90, so just
		// return it...
		if (0 == m_nNumberOfRepeatedCodes)
		{
			m_nLastCharacter = RLE_DELIMITER;

			if (bComputeCRC)
				ComputeCRC((unsigned int)m_nLastCharacter);

			return m_nLastCharacter;
		}

		// Since the last character we returned prior to finding the delimiter
		// was the first in a RLE series, we need to decrement the number of 
		// repeated codes by 1.
		m_nNumberOfRepeatedCodes--;

		m_bInRLESection = true;
		
		if (bComputeCRC)
			ComputeCRC((unsigned int)m_nLastCharacter);
		
		return m_nLastCharacter;
	}
	
	// Keep a running tally of the CRC
	if (bComputeCRC)
		ComputeCRC((unsigned int)m_uchTri[m_nIndexIntoDecodedBuffer]);

	m_nLastCharacter = m_uchTri[m_nIndexIntoDecodedBuffer++];
	return m_nLastCharacter;
}


// -----------------------------------------------------------------------------
// This function fills the decoded character buffer with 3 decoded 8bit
// characters.  
//
// According to the spec, we should ignore any CR/LF we encounter so we eat all
// of these.
//
// If we find a ':' then that indicates the end of the HQX file so we bail out
// if we find one of those, or if we actually hit EOF.
//
// Any other character we find is potentially a 6bit encoded character so we'll
// add it to the 6bit encoded character buffer.  Then we'll attempt to decode
// this buffer, failing if any of the input characters are invalid.
// -----------------------------------------------------------------------------
int CHQXFile::FillInputBuffer()
{
	int nNextChar = 0;
	int nIndex = 0;

	if (!m_pInputFile)
	{
		return HQX_ERROR_GENERIC;
	}

	// We MUST zero out the input buffer every time we fill it.  This is because
	// we may reach the end of the file and not have a full buffer, in which case
	// there could be illegal garbage characters left in it...
	m_szQuad[0] = m_szQuad[1] = m_szQuad[2] = m_szQuad[3] = 0;

	while (nIndex < LEN_ENCODED_CHAR_BUFF)
	{
		nNextChar = dec_fgetc(m_pInputFile);
		switch(nNextChar)
		{
			// Eat these...
			case ASC_CHR_LF:
			case ASC_CHR_CR:
				break;

			// Found our ':', this mean EOF! Don't get any more chars, just decode
			// what we have...
			case ASC_CHR_COLON:
				goto decodeBuffer;

			// Hit the actual end of file...
			case EOF:
				m_bRealEOF = true;
				return HQX_ERROR_EOF;
			
			// This is a 'regular' character, add it to the 6bit buffer
			default:
				m_szQuad[nIndex++] = nNextChar;
				break;
		}
	}

decodeBuffer:
	// Decode this buffer
	if (!DecodeQuadToTri(m_szQuad, m_uchTri))
	{
		m_bRealEOF = true;
		return HQX_ERROR_GENERIC;
	}

	// Reset index into decoded buffer
	m_nIndexIntoDecodedBuffer = 0;
	
	return HQX_SUCCESS;
}


// -----------------------------------------------------------------------------
// This function fills the decoded character buffer with 3 decoded 8bit
// characters.  
//
// According to the spec, we should ignore any CR/LF we encounter so we eat all
// of these.
//
// If we find a ':' then that indicates the end of the HQX file so we bail out
// if we find one of those, or if we actually hit EOF.
//
// Any other character we find is potentially a 6bit encoded character so we'll
// add it to the 6bit encoded character buffer.  Then we'll attempt to decode
// this buffer, failing if any of the input characters are invalid.
// -----------------------------------------------------------------------------
int CHQXFile::FillInputBuffer(char **pptr, size_t *pnBufferSize)
{
	int		nNextChar = 0;
	int		nIndex = 0;
	char	*ptr;

	// We MUST zero out the input buffer every time we fill it.  This is because
	// we may reach the end of the file and not have a full buffer, in which case
	// there could be illegal garbage characters left in it...
	m_szQuad[0] = m_szQuad[1] = m_szQuad[2] = m_szQuad[3] = 0;

	while (nIndex < LEN_ENCODED_CHAR_BUFF)
	{
		if (*pnBufferSize == 0)
		{
			m_bRealEOF = true;
			return HQX_ERROR_EOF;
		}

		ptr = *pptr;
		nNextChar = (int)(*ptr & 0x00ff);
		ptr++;
		*pptr = ptr;
		(*pnBufferSize)--;

		switch(nNextChar)
		{
			// Eat these...
			case ASC_CHR_LF:
			case ASC_CHR_CR:
				break;

			// Found our ':', this mean EOF! Don't get any more chars, just decode
			// what we have...
			case ASC_CHR_COLON:
				goto decodeBuffer;

			// Hit the actual end of file...
			case EOF:
				m_bRealEOF = true;
				return HQX_ERROR_EOF;
			
			// This is a 'regular' character, add it to the 6bit buffer
			default:
				m_szQuad[nIndex++] = nNextChar;
				break;
		}
	}

decodeBuffer:
	// Decode this buffer
	if (!DecodeQuadToTri(m_szQuad, m_uchTri))
	{
		m_bRealEOF = true;
		return HQX_ERROR_GENERIC;
	}

	// Reset index into decoded buffer
	m_nIndexIntoDecodedBuffer = 0;
	
	return HQX_SUCCESS;
}


//
// This CRC algorithm is adapted from the psuedo-pascal algorithm described in
// BinHex 4.0 Definition by Peter N Lewis, Aug 1991:
//
// "BinHex 4.0 uses a 16-bit CRC with a 0x1021 seed. The general algorithm
// is to take data 1 bit at a time and process it through the following:
// 1) Take the old CRC (use 0x0000 if there is no previous CRC) and shift it to the left by 1.
// 2) Put the new data bit in the least significant position (right bit).
// 3) If the bit shifted out in (1) was a 1 then xor the CRC with 0x1021.
// 4) Loop back to (1) until all the data has been processed.

// Or in pseudo pascal:
// var crc:integer; { init to zero at the beginning of each of the three forks }
// procedure CalcCRC (v: integer); { 0 <= v <= 255 }
// 
// var
// temp: boolean;
// i: integer;
// 
// begin
//     for i := 1 to 8 do begin
//         temp := (crc AND 0x8000) <> 0;
//         crc := (crc << 1) OR (v >> 7);
//
//         if temp then
//             crc := crc XOR 0x1021;
//         v := (v << 1) AND 0xFF;
//     end;
//end;"
//

void CHQXFile::ComputeCRC(unsigned int uChar)
{
	unsigned long ulTemp = m_wCrc;

	for (int i = 0; i < 8; i++)
	{
		uChar <<= 1;

		if ((ulTemp <<= 1) & 0x10000)
		{
			ulTemp = (ulTemp & 0xffff) ^ CRC_SEED;
		}

		ulTemp ^= (uChar >> 8);
		uChar &= 0xff;
	}

	m_wCrc = (WORD)ulTemp;
}


//************************************************************************
// bool DecodeQuadToTri(pszQuad, puchTri)
//
// Purpose
//
//  This function takes a 4-byte character array that contains valid BinHex
//  characters and produces a 3-byte unsigned char array with the decoded contents.
//
// Parameters:
//  pszQuad - The 4-byte array to convert
//  puchTri - The 3-bytes of decoded data
//
// Returns:
//
//  true if the 4-byte string is comprised of valid BinHex characters
//  false otherwise
//
//************************************************************************
// 8/19/96 Carey created.
// 10/6/00 CBenson modified for use in BinHex engine
//************************************************************************

bool CHQXFile::DecodeQuadToTri(char *pszQuad,
								 unsigned char *puchTri)
{
	if (!pszQuad || !puchTri)
		return false;

	if (!ComputeBinHexValues(pszQuad))
		return false;

	puchTri[0] = (pszQuad[0] << 2) | (pszQuad[1] >> 4);
	puchTri[1] = (pszQuad[1] << 4) | (pszQuad[2] >> 2);
	puchTri[2] = (pszQuad[2] << 6) |  pszQuad[3];
	return true;
}


//************************************************************************
// void EncodeTriToQuad(puchTri, pszQuad)
//
// Purpose
//
//  This function encodes 3 8-bit bytes into 4 valid BinHex bytes.
//
// Parameters:
//
//  lpbyTri: IN=Array of 3, 8-bit bytes to be encoded
//  lpszQuad: OUT=Array of 4, valid BinHex characters to be output
//
// Returns:
//
//  nothing
//
//************************************************************************
// 8/19/96 Carey created.
// 10/6/00 CBenson modified for use in BinHex engine
//************************************************************************
bool CHQXFile::EncodeTriToQuad(unsigned char *puchTri,
								 char *pszQuad)
{
	if (!pszQuad || !puchTri)
		return false;

	pszQuad[0] =   puchTri[0] >> 2;
	pszQuad[1] = ((puchTri[0] & 0x3) << 4) | (puchTri[1] >> 4);
	pszQuad[2] = ((puchTri[1] & 0xF) << 2) | (puchTri[2] >> 6);
	pszQuad[3] =   puchTri[2] & 0x3F;

	// Take the 6bit ascii codes and convert them to valid BinHex codes.
	return ComputeAsciiValues(pszQuad);
}


//************************************************************************
// bool ComputeBinHexValues(lpszQuad)
//
// Purpose
//
//  This function takes a 4-byte character array that contains valid BinHex
//  characters and updates its contents so it contains values between 0 and 63.
//  These 4, 6-bit bytes can then be converted into 3 8-bit bytes. 
//
// Parameters:
//
//  lpszQuad: IN=The 4-byte array to convert
//
// Returns:
//
//  true if the 4-byte string is comprised of valid BASE64 characters
//  false otherwise
//
//************************************************************************
// 8/19/96 Carey created.
// 10/6/00 CBenson modified for use in BinHex engine
//************************************************************************
bool CHQXFile::ComputeBinHexValues(char *lpszQuad)
{
	if (!lpszQuad)
		return false;

	int nIndex = 0;

	for (int i = 0; i < LEN_ENCODED_CHAR_BUFF; i++)
	{
		nIndex = lpszQuad[i] - ASC_CHR_SPACE;

		if (nIndex < 0)
			lpszQuad[i] = (char)0x00;  // Index underrun
		else if (nIndex >= LEN_LOOKUP_TABLE)
			lpszQuad[i] = (char)0xff;  // Index overrun
		else
			lpszQuad[i] = gszDecodeTable[nIndex];  // Index is valid

		if (lpszQuad[i] == (char)0xff)
			return false;
	}

	return true;
}


//************************************************************************
// bool ComputeAsciiValues(lpszQuad)
//
// Purpose
//
//  This function takes a 4-byte character array that contains valid ascii
//  characters and updates its contents so it contains valid BinHex characters.
//
// Parameters:
//
//  lpszQuad: IN=The 4-byte array to convert
//
// Returns:
//
//  true if the 4-byte string is comprised of valid BASE64 characters
//  false otherwise
//
//************************************************************************
// 8/19/96 Carey created.
// 10/6/00 CBenson modified for use in BinHex engine
//************************************************************************
bool CHQXFile::ComputeAsciiValues(char *lpszQuad)
{
	if (!lpszQuad)
		return false;

	for (int i = 0; i < LEN_ENCODED_CHAR_BUFF; i++)
		lpszQuad[i] = gszEncodeTable[lpszQuad[i] + ASC_CHR_SPACE];

	return true;
}


//************************************************************************
// bool GetLong(*plValue)
//
// Purpose
//
//  This function retrieves a single decoded 4 byte long from the input
//  stream.  
//
// Parameters:
//
//  *plValue: OUT=The long that was retrieved
//
// Returns:
//
//  HQX_SUCCESS if the long was retrieved successfully
//  An error code otherwise
//
//************************************************************************
// 10/6/00 CBenson created
//************************************************************************
int CHQXFile::GetLong(long *plValue)
{
	int nNextChar = 0;

	if (!plValue)
		return HQX_ERROR_GENERIC;

	// Next, read in a long value
	for (int nIndex = 0; nIndex < 4; nIndex++)
	{
		nNextChar = GetNextDecodedByte();

		// Check for read/decode errors...
		if (HQX_ERROR_EOF == nNextChar)
			return HQX_ERROR_EOF;

		// Build the long value
		*plValue <<= 8;
		*plValue |= nNextChar;
	}

	return HQX_SUCCESS;
}


//************************************************************************
// bool GetWord(*pwValue, bComputeCRC)
//
// Purpose
//
//  This function retrieves a single decoded 2 byte WORD from the input
//  stream.  If bComputeCRC is false, the retrieved WORD is not included
//  in the running CRC calculation.
//
// Parameters:
//
//  *pwValue: OUT=The WORD that was retrieved
//  bComputeCRC: IN=Determines if we use wValue in the CRC computation
//                  (default is true)
//
// Returns:
//
//  HQX_SUCCESS if the WORD was retrieved successfully
//  An error code otherwise
//
//************************************************************************
// 10/6/00 CBenson created
//************************************************************************
int CHQXFile::GetWord(WORD *pwValue, bool bComputeCRC)
{
	int nNextChar = 0;

	if (!pwValue)
		return HQX_ERROR_GENERIC;

	// Read in a WORD value
	for (int nIndex = 0; nIndex < 2; nIndex++)
	{
		nNextChar = GetNextDecodedByte(bComputeCRC);

		// Check for read/decode errors...
		if (HQX_ERROR_EOF == nNextChar)
			return HQX_ERROR_EOF;

		// Build the WORD value
		*pwValue <<= 8;
		*pwValue |= nNextChar;
	}

	return HQX_SUCCESS;
}


int CHQXFile::GetLong(char **pptr, size_t *pnBufferSize, long *plValue)
{
	int		nNextChar = 0;

	if (!plValue)
		return HQX_ERROR_GENERIC;

	// Decode 4 more bytes.
	for (int nIndex = 0; nIndex < 4; nIndex++)
	{
		nNextChar = GetNextDecodedByte(pptr, pnBufferSize);

		// Check for read/decode errors...
		if (HQX_ERROR_EOF == nNextChar)
			return HQX_ERROR_EOF;

		// Build the long value
		*plValue <<= 8;
		*plValue |= nNextChar;
	}

	return HQX_SUCCESS;
}


int CHQXFile::GetWord(char **pptr, size_t *pnBufferSize, WORD *pwValue, bool bComputeCRC)
{
	int nNextChar = 0;

	if (!pwValue)
		return HQX_ERROR_GENERIC;

	// Read in a WORD value
	for (int nIndex = 0; nIndex < 2; nIndex++)
	{
		nNextChar = GetNextDecodedByte(pptr, pnBufferSize, bComputeCRC);

		// Check for read/decode errors...
		if (HQX_ERROR_EOF == nNextChar)
			return HQX_ERROR_EOF;

		// Build the WORD value
		*pwValue <<= 8;
		*pwValue |= nNextChar;
	}

	return HQX_SUCCESS;
}


bool CHQXFile::WriteQuad(char *pszQuad)
{
	size_t	nBytesToWrite;

	nBytesToWrite = LEN_MAX_LINE_LENGTH - m_nCount;
	if (nBytesToWrite > 4)
		nBytesToWrite = 4;

	if (nBytesToWrite)
	{
		if (dec_fwrite(pszQuad, 1, nBytesToWrite, m_pOutputFile) != nBytesToWrite)
			return false;
	}

	// Increment the number of bytes we have written on the current line.
	m_nCount += nBytesToWrite;
	if (m_nCount == LEN_MAX_LINE_LENGTH)
	{
		// We have written as many bytes as we are allowed on this line.
		// Write a CRLF and go to the next line.
		if (dec_fwrite("\x0D\x0A", 1, 2, m_pOutputFile) != 2)
			return false;

		m_nCount = 0;
	}

	// Did we write less than 4 bytes?
	if (nBytesToWrite < 4)
	{
		size_t	nBytesLeft;

		// Yes.  So, we now need to write the remaining bytes of the quad.
		nBytesLeft = 4 - nBytesToWrite;
		if (dec_fwrite(&pszQuad[nBytesToWrite], 1, nBytesLeft, m_pOutputFile) != nBytesLeft)
			return false;

		m_nCount += nBytesLeft;
	}

	return true;
}


//************************************************************************
// bool DecodeCurrentFork(*pszFile)
//
// Purpose
//
//  This function will decode the current BinHex fork (either data or
//  resource) to pszFile.
//
// Parameters: 
//  *pszFile - File to output the current fork into
//
// Returns:
//
//  HQX_SUCCESS if the current fork is decoded properly
//  An error code otherwise
//
//************************************************************************
// 10/6/00 CBenson created
//************************************************************************
int CHQXFile::DecodeFork(int iFork, const char *pszFile)
{
	int		nReturn = HQX_ERROR_GENERIC;
	int		nNextChar = 0;
	long	lLen = 0;
	long	lIndex = 0;
	DWORD	dwTotalBytesWritten = 0;
	size_t	sizeBytesToWrite = 0;
	FILE	*pOutputFile = NULL;
	unsigned char *pOutputBuffer = NULL;
	WORD	wCRC;

	if (!pszFile)
		goto bailOut;

	// Validate input file pointer and open output file
	pOutputFile = dec_fopen(pszFile, "wb");
	if (!m_pInputFile || !pOutputFile)
		goto bailOut;

	pOutputBuffer = (unsigned char *)malloc(HQX_OUTPUT_BUFFER_SIZE);
	if (!pOutputBuffer)
		goto bailOut;

	// Reset the crc before extraction of each fork...
	m_wCrc = 0;
	wCRC = 0;

	// Calculate how much data to extract, and also update our 
	// HQX structure with the starting offsets for each fork
	// (used later if we need to change/replace/delete a fork.
	switch (iFork)
	{
		case HQX_FORK_DATA:
			lLen = m_hqxHeader.lDataForkLength;
			break;

		case HQX_FORK_RESOURCE:
			lLen = m_hqxHeader.lResourceForkLength;
			break;
		
		default:
			return nReturn;
	}

	// Do the extraction
	for (lIndex = 0; lIndex < lLen; lIndex++)
	{
		nNextChar = GetNextDecodedByte(m_bEnforceCRC);
		if (HQX_ERROR_EOF == nNextChar)
		{
			nReturn = HQX_ERROR_EOF; 
			goto bailOut;
		}

		pOutputBuffer[sizeBytesToWrite++] = (unsigned char)nNextChar;

		if (sizeBytesToWrite == HQX_OUTPUT_BUFFER_SIZE)
		{
			// Flush the output buffer
			if (sizeBytesToWrite != dec_fwrite(pOutputBuffer, 1, sizeBytesToWrite, pOutputFile))
			{
				nReturn = HQX_ERROR_EOF;
				goto bailOut;
			}

			dwTotalBytesWritten += sizeBytesToWrite;
			sizeBytesToWrite = 0;

			// See if we've exceeded our max extraction size or been asked to abort...
			if (g_dwTextMaxExtractSize && dwTotalBytesWritten > g_dwTextMaxExtractSize)
			{
				nReturn = HQX_ERROR_MAX_EXTRACT;
				goto bailOut;
			}

			if (g_bTextAbort)
			{
				nReturn = HQX_ERROR_ABORT;
				goto bailOut;
			}
		}
	}

	// Flush any remaining bytes in the output buffer
	if (sizeBytesToWrite)
	{
		if (sizeBytesToWrite != dec_fwrite(pOutputBuffer, 1, sizeBytesToWrite, pOutputFile))
		{
			nReturn = HQX_ERROR_EOF;
			goto bailOut;
		}
		
		dwTotalBytesWritten += sizeBytesToWrite;
		
		// See if we've exceeded our max extraction size or been asked to abort...
		if (g_dwTextMaxExtractSize && dwTotalBytesWritten > g_dwTextMaxExtractSize)
		{
			nReturn = HQX_ERROR_MAX_EXTRACT;
			goto bailOut;
		}
	}

	// Finish computing the CRC for this fork and check it against
	// the CRC in the file...
	if (m_bEnforceCRC)
	{
		ComputeCRC(0);
		ComputeCRC(0);

		if (HQX_SUCCESS == GetWord(&wCRC, false))
		{
			if (m_wCrc == wCRC)
				nReturn = HQX_SUCCESS;
		}	
	}

	else
	{
		if (HQX_SUCCESS == GetWord(&wCRC, false))
			nReturn = HQX_SUCCESS;
	}

	if (nReturn == HQX_SUCCESS && iFork == HQX_FORK_RESOURCE)
	{
		int		iChar = 0;

		// Save the offset of the end of the BinHex body data so
		// that we know where to apply change, replace, and delete
		// operations.
		while (iChar != -1)
		{
			iChar = dec_fgetc(m_pInputFile);
			if (iChar == ASC_CHR_COLON ||
				iChar == ASC_CHR_CR ||
				iChar == ASC_CHR_LF)
			{
				m_nEndOffset = dec_ftell(m_pInputFile) - 1;
				break;
			}
		}

		if (iChar == -1)
		{
			// We hit EOF before locating a colon.
			m_nEndOffset = dec_ftell(m_pInputFile) - 1;
		}
	}

bailOut:
	// Close the output file and clean up
	if (pOutputFile)
	{
		if (EOF == dec_fclose(pOutputFile))
			nReturn = HQX_ERROR_GENERIC;

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
// bool ConvertHeaderToBuffer(hqxHeader, **ppszHeaderBuff, &lLengthOfHeader)
//
// Purpose
//
//  This function will prepare to create a new HQX file by linearizing
//  an HQX header into a stream of bytes that can be encoded.
//
// Parameters: 
//  hqxHeader - The header to convert
//  ppszHeaderBuff - The buffer to hold the converted header
//  lLengthOfHeader - The size of ppszHeaderBuff
//
// Returns:
//
//  true if the header is converted properly
//  An error code otherwise
//
//************************************************************************
// 10/6/00 CBenson created
//************************************************************************
bool CHQXFile::ConvertHeaderToBuffer(HQXHEADER hqxHeader,
										unsigned char **ppszHeaderBuff,
										long &lLengthOfHeader)
{
	unsigned int uSizeOfHeader = 0;
	unsigned int uSizeOfEncodedHeader = 0;
	unsigned char *pszHeader = NULL;
	unsigned char *pszTemp = NULL;
	unsigned int uLengthOfFilename = strlen(hqxHeader.szFilename) + 1;

	// Now we need to encode the header.  To do this, we need to make a member-by-member
	// copy of the existing header into a single buffer (we can't just copy it directly
	// because the filename can vary in length.
	uSizeOfHeader = (uLengthOfFilename + sizeof(BYTE) + (sizeof(long) * 4) + sizeof(WORD));

	// Allocate memory for the buffer.  The caller is responsible for freeing this.
	pszHeader = (unsigned char *)malloc(uSizeOfHeader);
	if (!pszHeader)
		return false;

	// Save the header's start...
	pszTemp = pszHeader;

	// Make a copy of the header buffer for encoding and copy the fields.
	memcpy(pszTemp, &hqxHeader.byFilenameLen, sizeof(BYTE));

	pszTemp += sizeof(BYTE);

	memcpy(pszTemp, &hqxHeader.szFilename, (hqxHeader.byFilenameLen + 1));

	pszTemp += uLengthOfFilename;

	// Now copy the remaining fields...
#if !defined(CPU_BIG_ENDIAN)
	DWORD dwVal;
	WORD wVal;
	
	dwVal = SWAPDWORD(hqxHeader.lFileType);
	memcpy(pszTemp, &dwVal, sizeof(long));
	pszTemp += sizeof(long);

	dwVal = SWAPDWORD(hqxHeader.lCreator);
	memcpy(pszTemp, &dwVal, sizeof(long));
	pszTemp += sizeof(long);

	wVal = SWAPWORD(hqxHeader.wFlags);
	memcpy(pszTemp, &wVal, sizeof(WORD));
	pszTemp += sizeof(WORD);
	
	dwVal = SWAPDWORD(hqxHeader.lDataForkLength);
	memcpy(pszTemp, &dwVal, sizeof(long));
	pszTemp += sizeof(long);
	
	dwVal = SWAPDWORD(hqxHeader.lResourceForkLength);
	memcpy(pszTemp, &dwVal, sizeof(long));
	pszTemp += sizeof(long);
#else
	memcpy(pszTemp, &hqxHeader.lFileType, sizeof(long));
	pszTemp += sizeof(long);

	memcpy(pszTemp, &hqxHeader.lCreator, sizeof(long));
	pszTemp += sizeof(long);

	memcpy(pszTemp, &hqxHeader.wFlags, sizeof(WORD));
	pszTemp += sizeof(WORD);

	memcpy(pszTemp, &hqxHeader.lDataForkLength, sizeof(long));
	pszTemp += sizeof(long);

	memcpy(pszTemp, &hqxHeader.lResourceForkLength, sizeof(long));
	pszTemp += sizeof(long);
#endif

	*ppszHeaderBuff = pszHeader;
	lLengthOfHeader = uSizeOfHeader;

	return true;
}


//************************************************************************
// int EncodeHeaderToBinHex(hqxHeader, 
//                          *pucTri, 
//                          &nCount)
//
// Purpose
//
//  This function will perform BinHex encoding on the header hqxHeader.  
//  We may not fill the output buffer completely with the header alone,
//  so we track the encoded bits in pucTri and pszLine, to be passed
//  down to EncodeFileToBinHex later.
//
//  A running 16bit CRC will be kept on each of the unencoded characters
//  and written immediately following the last encoded byte.
//
// Parameters: 
//  hqxHeader - Header to encode
//  *pucTri - Buffer of unencoded bytes
//  &nCount - Index into pucTri
//
// Returns:
//
//  HQX_SUCCESS if pszDataFile is encoded correctly
//  An error code otherwise
//
//************************************************************************
// 10/6/00 CBenson created
//************************************************************************
int CHQXFile::EncodeHeaderToBinHex(
	HQXHEADER hqxHeader,
	unsigned char *pucTri,
	int &nCount)
{
	// Buffer and encoding management variables
	unsigned char	*puchHeaderBuff = NULL;
	unsigned char	*pucIntermediateBuffer = NULL;
	BYTE			*pbyCRCByte = NULL;
	long			lLengthOfHeader = 0;
	int				nIndex;
	int				nIntermediateIndex;
	int				nLengthOfIntermediateBuffer;	 
	int				nRetval = HQX_ERROR_GENERIC;
	bool			bFirstLine = true;
	char			szQuad[4];
	
	if (!pucTri)
	{
		dec_assert(0);
		goto bailOut;
	}

	// Create a buffer to hold the intermediate data (RLE compressed data)
	pucIntermediateBuffer = (unsigned char *)malloc(HQX_ENCODE_BUFFER_SIZE * 2);
	if (!pucIntermediateBuffer)
		goto bailOut;

	// Make a linear buffer out of the header
	if (!ConvertHeaderToBuffer(hqxHeader, &puchHeaderBuff, lLengthOfHeader))
		goto bailOut;

	// Initialize vars...
	nIndex = nIntermediateIndex = nLengthOfIntermediateBuffer = 0;

	// Initialize CRC
	m_wCrc = 0;

	// Encode the header...
	if (!CompressBuffer(puchHeaderBuff, 
						lLengthOfHeader,
						pucIntermediateBuffer,
						nLengthOfIntermediateBuffer))
	{
		goto bailOut;
	}

	// Filling the input buffer will also calculate the CRC.  We need to tack that on the 
	// end of the buffer...before we do that, we need to compute two zero bytes of CRC.
	ComputeCRC(0);
	ComputeCRC(0);

#if !defined(CPU_BIG_ENDIAN)
	m_wCrc = SWAPWORD(m_wCrc);
#endif

	pbyCRCByte = (BYTE *)&m_wCrc;

	// Account for any 0x90's in the CRC
	pucIntermediateBuffer[nLengthOfIntermediateBuffer++] = *pbyCRCByte;
	if (RLE_DELIMITER == *pbyCRCByte)
	{
		pucIntermediateBuffer[nLengthOfIntermediateBuffer++] = 0x00;
	}

	*pbyCRCByte++;

	pucIntermediateBuffer[nLengthOfIntermediateBuffer++] = *pbyCRCByte;
	if (RLE_DELIMITER == *pbyCRCByte)
	{
		pucIntermediateBuffer[nLengthOfIntermediateBuffer++] = 0x00;
	}

	while (true)	
	{
		pucTri[nCount++] = pucIntermediateBuffer[nIntermediateIndex++];

		// Is the to-be-encoded buffer full?  Encode it...
		if (nCount == 3)
		{
			// Encode the 3 bytes into 4 valid BinHex characters and
			// write the quad.
			if (!EncodeTriToQuad(pucTri, szQuad))
				goto bailOut;

			if (!WriteQuad(szQuad))
				goto bailOut;

			nCount = 0;
			pucTri[0] = pucTri[1] = pucTri[2] = 0;
		}

		// Have we reached the end of the input?
		if (nIntermediateIndex == nLengthOfIntermediateBuffer)
			break;
	}

	nRetval = HQX_SUCCESS;

bailOut:
	// Clean up buffers
	if (puchHeaderBuff)
	{
		free(puchHeaderBuff);
		puchHeaderBuff = NULL;
	}

	if (pucIntermediateBuffer)
	{
		free(pucIntermediateBuffer);
		pucIntermediateBuffer = NULL;
	}

	return nRetval;
}


//************************************************************************
// int EncodeFileToBinHex(*pszDataFile, 
//                        *pucTri, 
//                        &nCount)
//
// Purpose
//
//  This function perform BinHex encoding on the file named 'pszDataFile'.
//  Since the different sections of a BinHex file will overlap, pucTri
//  and pszLine will probably contain some bits from the previous file
//  or header. 
//
//  A running 16bit CRC will be kept on each of the unencoded characters
//  and written immediately following the last encoded byte.
//
// Parameters: 
//  *pszDataFile - File to encode
//  *pucTri - Buffer of unencoded bytes
//  &nCount - Index into pucTri
//
// Returns:
//
//  HQX_SUCCESS if pszDataFile is encoded correctly
//  An error code otherwise
//
//************************************************************************
// 10/6/00 CBenson created
//************************************************************************
int CHQXFile::EncodeFileToBinHex(
	const char *pszFileToEncode,
	unsigned char *pucTri,
	int &nCount)
{
	// Buffer and encoding management variables
	unsigned char	pszZeroByteCRC[] = { 0x00, 0x00, 0x00 };
	FILE			*pForkFile = NULL;
	BYTE			*pbyCRCByte = NULL;
	int				nRetval = HQX_ERROR_GENERIC;
	int				nIndex;
	int				nIntermediateIndex;
	int				nLengthOfIntermediateBuffer;	 
	unsigned char	*pszEncodingBuffer = NULL;
	unsigned char	*pucIntermediateBuffer = NULL;
	size_t			sizeBytesToEncode = 0; 
	char			szQuad[4];
	
	if (!pucTri)
	{
		dec_assert(0);
		goto bailOut;
	}

	// Create a buffer to hold data to be encoded
	pszEncodingBuffer = (unsigned char *)malloc(HQX_ENCODE_BUFFER_SIZE);
	if (!pszEncodingBuffer)
		goto bailOut;

	// Create a buffer to hold the intermediate data (RLE compressed data)
	pucIntermediateBuffer = (unsigned char *)malloc(HQX_ENCODE_BUFFER_SIZE * 2);
	if (!pucIntermediateBuffer)
		goto bailOut;

	// Initialize vars...
	nIndex = nIntermediateIndex = nLengthOfIntermediateBuffer = 0;

	// Initialize CRC
	m_wCrc = 0;

	// Encode pszDataFile...
	if (pszFileToEncode)
	{
		// Open the data fork file
		pForkFile = dec_fopen(pszFileToEncode, "rb");
		if (!pForkFile)
			goto bailOut;

		// Get a buffer of data to encode
		sizeBytesToEncode = dec_fread(pszEncodingBuffer,
									 1,
									 HQX_ENCODE_BUFFER_SIZE,
									 pForkFile);
		if (sizeBytesToEncode != 0)
		{
			// Compress the buffer 
			if(!CompressBuffer(pszEncodingBuffer, 
							  sizeBytesToEncode, 
							  pucIntermediateBuffer,
							  nLengthOfIntermediateBuffer))
			{
				goto bailOut;
			}
		}
		else
		{
			// Check for error or EOF
			if (dec_feof(pForkFile))
				goto finishFork;
			else
				goto bailOut;
		}

		while (true)	
		{
			pucTri[nCount++] = pucIntermediateBuffer[nIntermediateIndex++];

			// Is the to-be-encoded buffer full?  Encode it...
			if (nCount == 3)
			{
				// Encode the 3 bytes into 4 valid BinHex characters and
				// write the quad.
				if (!EncodeTriToQuad(pucTri, szQuad))
					return HQX_ERROR_GENERIC;

				if (!WriteQuad(szQuad))
					return HQX_ERROR_GENERIC;

				nCount = 0;
				pucTri[0] = pucTri[1] = pucTri[2] = 0;
			}

			// Do we need to refill the lookahead buffer?
			if (nIntermediateIndex == nLengthOfIntermediateBuffer)
			{
				// Get a buffer of data to encode
				sizeBytesToEncode = dec_fread(pszEncodingBuffer,
											 1,
											 HQX_ENCODE_BUFFER_SIZE,
											 pForkFile);
				if (sizeBytesToEncode != 0)
				{
					// Compress the buffer 
					if (!CompressBuffer(pszEncodingBuffer, 
									  sizeBytesToEncode, 
									  pucIntermediateBuffer,
									  nLengthOfIntermediateBuffer))
					{
						goto bailOut;
					}

					nIntermediateIndex = 0;
				}
				else
				{
					// Check for error or EOF
					if (dec_feof(pForkFile))
						goto finishFork;
					else
						goto bailOut;
				}
			}
		}
	}

finishFork:
	// Now, we need to encode the CRC.  It was either calculated above
	// (if pszFileToEncode is not NULL) or is two zero bytes.
	if (pszFileToEncode)
	{
		// Need to calculate two zero bytes of CRC - per spec.
		ComputeCRC(0);
		ComputeCRC(0);

#if !defined(CPU_BIG_ENDIAN)
		m_wCrc = SWAPWORD(m_wCrc);
#endif
		pbyCRCByte = (BYTE *)&m_wCrc;
	}

	for (nIndex = 0; nIndex < 2; nIndex++)
	{
		if (!pszFileToEncode)
		{
			pucTri[nCount++] = 0x00;
		}
		else
		{			
			pucTri[nCount++] = *pbyCRCByte;

			// Is the to-be-encoded buffer full?  Encode it...
			if (nCount == 3)
			{
				// Encode the 3 bytes into 4 valid BinHex characters and
				// write the quad.
				if (!EncodeTriToQuad(pucTri, szQuad))
					goto bailOut;

				if (!WriteQuad(szQuad))
					goto bailOut;

				nCount = 0;
				pucTri[0] = pucTri[1] = pucTri[2] = 0;
			}

			// Account for any 0x90's in the CRC
			if (RLE_DELIMITER == *pbyCRCByte)
				pucTri[nCount++] = 0x00;

			*pbyCRCByte++;
		}

		// Is the to-be-encoded buffer full?  Encode it...
		if (nCount == 3)
		{
			// Encode the 3 bytes into 4 valid BinHex characters and
			// write the quad.
			if (!EncodeTriToQuad(pucTri, szQuad))
				goto bailOut;

			if (!WriteQuad(szQuad))
				goto bailOut;

			nCount = 0;
			pucTri[0] = pucTri[1] = pucTri[2] = 0;
		}
	}

	nRetval = HQX_SUCCESS;

bailOut:
	// Clean up buffers
	if (pszEncodingBuffer)
	{
		free(pszEncodingBuffer);
		pszEncodingBuffer = NULL;
	}

	if (pucIntermediateBuffer)
	{
		free(pucIntermediateBuffer);
		pucIntermediateBuffer = NULL;
	}

	// Close the data fork file
	if (pForkFile)
	{
		if (EOF == dec_fclose(pForkFile))
			nRetval = HQX_ERROR_GENERIC;

		pForkFile = NULL;
	}

	return nRetval;
}


//************************************************************************
// int CompressBuffer(*puchInput, nSizeInput, *pucOutputBuffer, &nSizeOfOutput)
//
// Purpose
//
//  This function will fill pucOutputBuffer with nSizeInput bytes from 
//  puchInput.  While doing so, it will also perform RLE (run length
//  encoding) on the input.  
//
//  Some notes about RLE:
//
//  When a set of repeated characters are encountered, we can gain
//  some compression on the input by using RLE.
//
//  A RLE section has the following form (where the numbers are
//  successive bytes:
//
//  [0] - Character to repeat
//  [1] - RLE delimiter (0x90)
//  [2] - Character denoting how many times to repeat [0]
//
//  There is a special case.  If the character to repeat is a 0x90, 
//  then we must emit a 0x00 right after.  This is true even if there 
//  is only one 0x90.
//  So, if we wanted to emit a single 0x90 we would emit 0x90 0x00.
//  If we had a run of 5 0x90's, we would emit:
//  0x90 0x00 0x90 0x05
//
//  [0] - Character to repeat (0x90)
//  [1] - 0x00 (Special case for 0x90)
//  [2] - RLE delimiter (0x90)
//  [3] - Character denoting how many times to repeat
//
//  Note the space requirements for RLE.  In the general case, it will
//  take 3 bytes, so we should only encode runs of 4 characters or more.
//  However, the spec says to encode runs even as small as 3 bytes, so
//  I am doing that.  Seems stupid to do and it would still be a valid
//  HQX file if we only did 3 bytes, but in the interest of consistency,
//  we'll follow spec.
//
//  In the case of a run of 0x90, it will require 4 bytes to encode, 
//
// Parameters: 
//  *puchInput - Buffer to get input from
//  nSizeInput - Size of pszLookAhead
//  *pucOutputBuffer - Buffer to fill with compressed data             
//  &nSizeOfOutput - Size of pucOutputBuffer on exit
//
// Returns:
//
//  true if pucOutputBuffer is filled successfully
//  An error code otherwise
//
//************************************************************************
// 11/15/00 CBenson created
//************************************************************************

bool CHQXFile::CompressBuffer(unsigned char *puchInput,
								int nSizeInput,
								unsigned char *pucOutputBuffer,
								int &nSizeOfOutput)
{
	bool bEndOfBuffer = false;
	bool bMaxRLEReached = false;
	int nLastChar = 0;
	int nChar = 0;
	int nNumRepeatedChars = 0;
	bool bFirstChar = true;
	int i = 0;
	int nTotalBytesRead = 0;

	int nIndexIntoInput = 0;
	nSizeOfOutput = 0;

	if (!puchInput || !pucOutputBuffer)
	{
		dec_assert(0);
		return false;
	}

	while (!bEndOfBuffer)
	{
		nChar = SafeGetChar(puchInput, nIndexIntoInput, nSizeInput, bEndOfBuffer);

		if (bEndOfBuffer)
		{
			break;
		}

		nTotalBytesRead++;

		// Keep tally of CRC while we read in...
		ComputeCRC((unsigned int)nChar);

		// Special case for first character, just output it...
		if (bFirstChar)
		{
			pucOutputBuffer[nSizeOfOutput++] = nChar;
			nLastChar = nChar;

			//
			// If nChar is a 0x90, we need to emit a 0x00 after it
			// to indicate this is a literal 0x90 and not the start
			// of a run...
			//
			if (RLE_DELIMITER == nChar)
				pucOutputBuffer[nSizeOfOutput++] = (unsigned char)0x00;

			bFirstChar = false;

			continue;
		}

		// Check for a run
		if (nChar == nLastChar)
		{
			int nRunChar, nCharThird;

			// We've now seen two of the same character...
			nNumRepeatedChars = 2;

			// Maybe...must have at least 3 repeating characters to make
			// it worthwhile to RLE...Get one more char and see...
			nCharThird = SafeGetChar(puchInput, nIndexIntoInput, nSizeInput, bEndOfBuffer);
			
			if (bEndOfBuffer)
			{
				// If we hit the end of the buffer, we still must emit the second char we
				// read in since we haven't done so yet...
				pucOutputBuffer[nSizeOfOutput++] = nChar;

				// If we read in a literal 0x90, then emit a 0x00 after it...
				if (RLE_DELIMITER == nChar)
					pucOutputBuffer[nSizeOfOutput++] = (unsigned char)0x00;

				continue;
			}

			nTotalBytesRead++;

			// Check for repeated chars
			if (nLastChar != nCharThird)
			{
				// No it's not a run, so we need to stick the third
				// character we just read into the output and update the CRC correctly.
				pucOutputBuffer[nSizeOfOutput++] = nChar;

				// If we read in a literal 0x90, then emit a 0x00 after it...
				if (RLE_DELIMITER == nChar)
					pucOutputBuffer[nSizeOfOutput++] = (unsigned char)0x00;

				pucOutputBuffer[nSizeOfOutput++] = nCharThird;
				ComputeCRC((unsigned int)nCharThird);

				// If we read in a literal 0x90, then emit a 0x00 after it...
				if (RLE_DELIMITER == nCharThird)
					pucOutputBuffer[nSizeOfOutput++] = (unsigned char)0x00;

				nLastChar = nCharThird;

				// Go on the next iteration of the loop...
				continue;
			}

			// Yes, it's a valid run...keep reading until we don't find a match
			nNumRepeatedChars++;

			pucOutputBuffer[nSizeOfOutput++] = (BYTE)RLE_DELIMITER;

			// Update CRC for third byte
			ComputeCRC((unsigned int)nChar);

			do
			{
				nRunChar = SafeGetChar(puchInput, nIndexIntoInput, nSizeInput, bEndOfBuffer);

				if (!bEndOfBuffer && nRunChar == nLastChar)
				{
					nTotalBytesRead++;
					nNumRepeatedChars++;
					ComputeCRC((unsigned int)nRunChar);
				}

				// End case - don't forget to count this byte...chars don't match, end of run
				else if (!bEndOfBuffer)
				{
					nTotalBytesRead++;
					break;
				}

				// We can only encode a run of 255 (0xFF) chars at once.
				if (nNumRepeatedChars == 255)
				{
					bFirstChar = bMaxRLEReached = true;
					break;
				}

			} while (!bEndOfBuffer);

			// Emit the count of repeated characters
			pucOutputBuffer[nSizeOfOutput++] = nNumRepeatedChars;

			// If we aren't done with the input yet, stick the last read character into pszLookAhead
			// and continue...unless we are still in a run and we have reached the limit of the number
			// of RLE codes we can store (255).  In that case, don't emit the char, just start over
			// at the top, just as if we are at the start of the buffer (set bFirstChar to true).
			if (!bEndOfBuffer && !bMaxRLEReached)
			{
				pucOutputBuffer[nSizeOfOutput++] = nRunChar;
				nLastChar = nRunChar;

				ComputeCRC((unsigned int)nRunChar);

				// If we read in a literal 0x90, then emit a 0x00 after it...
				if (RLE_DELIMITER == nRunChar)
					pucOutputBuffer[nSizeOfOutput++] = (unsigned char)0x00;
			}
			
			bMaxRLEReached = false;
			nNumRepeatedChars = 0;
		}

		// No run, just output the byte...
		else
		{
			pucOutputBuffer[nSizeOfOutput++] = nChar;
			nLastChar = nChar;

			// If the char to emit is actually a literal 0x90
			// then emit a 0x00 after it.
			if (RLE_DELIMITER == nChar)
				pucOutputBuffer[nSizeOfOutput++] = (unsigned char)0x00;
		}
	}

	return true;
}


//
// Perform boundary checking on array before getting something from it...
//

inline int CHQXFile::SafeGetChar(unsigned char *puchInput,
									int &nIndexInBuffer, 
									int nSizeOfBuffer, 
									bool &bEndOfBuffer) 
{
	if (!puchInput)
	{
		dec_assert(0);
		return EOF;
	}

	if (nIndexInBuffer >= nSizeOfBuffer) 
	{
		bEndOfBuffer = true;
		return EOF;	
	}
		
	return puchInput[nIndexInBuffer++]; 
}


//************************************************************************
// int CreateNewBinHexBody(HQXHEADER hqxHeader,
//                         const char *pszDataFile,
//                         const char *pszRscFile)
//
// Purpose
//
//  This function will create a new BinHex body from its component parts.
//  It does this be encoding and concatenating the 6 parts that make up
//  a BinHex body:
//
//  <HEADER><WORD-CRC><DATAFORK><WORD-CRC><RESOURCEFORK><WORD-CRC>
//
//  All these parts are essentially treated as a single stream of data
//  to be encoded.  The only thing we need to keep track of is when to
//  emit the CRC (after each fork).
//
//  Upon entry into this function, it is required that the new header
//  (complete with CRC) be fully constructed in puchHeaderBuff, and the
//  files that represent DATAFORK and RESOURCEFORK be identified in 
//  pszDataFile and pszRscFile respectively.  If either of these are NULL,
//  then that means that that particular fork is being deleted so we can
//  account for the CRC of that fork (two zero bytes) when encoding.
//
//  Note that either pszDataFile or pszRscFile may refer to a completely
//  new file (in the case of replace) or may refer to a changed copy of the
//  original (in the case of a change).
//
//  The basic idea here is to perform 3 loops.  First, to encode all the 
//  bytes of the header+crc in puchHeaderBuff, next to encode the bytes in 
//  pszDataFile+crc, finally to encode the bytes in pszRscFile+crc.  We keep
//  a lookahead buffer filled with the bytes from the appropriate input source.
//  The only thing we need to track is the lengths of the data and rsc forks
//  so we know where to emit the CRC's at (the header comes in here with its
//  CRC already calcuated and placed in the buffer).
//
// Parameters: 
//  hqxHeader - New header to write
//  pszDataFile - The file to encode as the data fork
//  pszRscFile - The file to encode as the resource fork
//
// Returns:
//
//  HQX_SUCCESS if successful
//  HQX_ERROR_GENERIC otherwise
//
//************************************************************************
// 11/14/00 CBenson created
//************************************************************************

int CHQXFile::CreateNewBinHexBody(
	HQXHEADER hqxHeader,
	const char *pszDataFile,
	const char *pszRscFile)
{
	unsigned char	ucTri[4];
	char			szQuad[4];
	int				nCount;
	int				nRetval = HQX_ERROR_GENERIC;  // Assume failure.

	dec_assert(m_pOutputFile != NULL);
	if (m_pOutputFile == NULL)
		return HQX_ERROR_GENERIC;

	// NOTE: The data and resource filenames can be null or empty.

	nCount = 0;
	ucTri[0] = ucTri[1] = ucTri[2] = ucTri[3] = 0;

	// Account for the colon that is on the first line.  This aligns
	// the output data with lines of length LEN_MAX_LINE_LENGTH.
	m_nCount = 1;

	// Encode the header first...
	if (HQX_SUCCESS != EncodeHeaderToBinHex(hqxHeader, ucTri, nCount))
		goto bailOut;

	// Encode the data fork next.
	if (HQX_SUCCESS != EncodeFileToBinHex(pszDataFile, ucTri, nCount))
		goto bailOut;

	// Encode the resource fork next.
	if (HQX_SUCCESS != EncodeFileToBinHex(pszRscFile, ucTri, nCount))
		goto bailOut;

	// FINAL STEP:
	// See if we have a partially full ucTri (< 3 bytes).  Add it to the
	// current line.
	if (nCount != 0)
	{
		if (!EncodeTriToQuad(ucTri, szQuad))
			goto bailOut;

		if (!WriteQuad(szQuad))
			goto bailOut;
	}

	// Write the ASCII colon character that terminates the BinHex data.
	if (dec_fwrite("\x3A", 1, 1, m_pOutputFile) != 1)
		goto bailOut;

	nRetval = HQX_SUCCESS;

bailOut:
	return nRetval;
}


/////////////////////////////////////////////////////////////////////////
// CHQXFile::CommitChanges()

bool CHQXFile::CommitChanges(char *pszOutputFile)
{
	bool		bRetVal = false;
	const char	*pszDataFile = NULL;
	const char	*pszResourceFile = NULL;
	long		lReplacementDataLength = 0;
	long		lReplacementResourceLength = 0;

	if (!m_bChanged)
		return true;

	// Open the new output file.
	m_pOutputFile = dec_fopen(pszOutputFile, "wb");
	if (!m_pOutputFile)
		return bRetVal;

	// Prepare a new header.  The szFilename field in this new header
	// will just point to the same pointer as in the existing header, 
	// but that's ok because we will never be changing this...
	HQXHEADER hqxNewHeader = m_hqxHeader;

	// Determine what we are doing now...we need to set up the new header appropriately.
	// Track delete operations first...
	if (m_dwPendingChanges & MODIFY_DATA_DELETE)
	{
		// Make sure no invalid flag combinations are used
		if (m_dwPendingChanges & MODIFY_DATA_REPLACE)
		{
			dec_assert(0);
			goto bailOut;
		}

		// Set length of data fork to zero...
		hqxNewHeader.lDataForkLength = 0;

		// Set data file to NULL since we're deleting it.
		pszDataFile = NULL;

		// If we're NOT modifying the resource fork, then
		// set it to the existing extracted file.  If it IS 
		// being modified, then the code below will handle 
		// setting its value.
		if (!(m_dwPendingChanges & MODIFY_RESOURCE_DELETE) &&
			!(m_dwPendingChanges & MODIFY_RESOURCE_REPLACE))
		{
			pszResourceFile = m_pszResourceForkFile;
		}
	}

	if (m_dwPendingChanges & MODIFY_RESOURCE_DELETE)
	{
		// Make sure no invalid flag combinations are used
		if (m_dwPendingChanges & MODIFY_RESOURCE_REPLACE)
		{
			dec_assert(0);
			goto bailOut;
		}

		// Set length of resource fork to zero...
		hqxNewHeader.lResourceForkLength = 0;
		
		// Set resource file to NULL since we're deleting it.
		pszResourceFile = NULL;

		// If we're NOT modifying the data fork, then
		// set it to the existing extracted file.  If it IS 
		// being modified, then the code below will handle 
		// setting its value.
		if (!(m_dwPendingChanges & MODIFY_DATA_DELETE) &&
			!(m_dwPendingChanges & MODIFY_DATA_REPLACE))
		{
			pszDataFile = m_pszDataForkFile;
		}
	}

	// Track replacements next...
	if (m_dwPendingChanges & MODIFY_DATA_REPLACE)
	{
		// Make sure no invalid flag combinations are used
		if (m_dwPendingChanges & MODIFY_DATA_DELETE)
		{
			dec_assert(0);
			goto bailOut;
		}

		// Get the length of the replacement file
		if (!GetFileLength(m_sReplacementDataFile, &lReplacementDataLength))
			goto bailOut;

		// Set length of data fork to the length of the replacement file...
		hqxNewHeader.lDataForkLength = lReplacementDataLength;
		
		// Set data file to point to the replacement data file
		// since we're replacing this entry.
		pszDataFile = m_sReplacementDataFile;

		// If we're NOT modifying the resource fork, then
		// set it to the existing extracted file.  If it IS 
		// being modified, then the code below will handle 
		// setting its value.
		if (!(m_dwPendingChanges & MODIFY_RESOURCE_DELETE) &&
			!(m_dwPendingChanges & MODIFY_RESOURCE_REPLACE))
		{
			pszResourceFile = m_pszResourceForkFile;
		}
	}

	if (m_dwPendingChanges & MODIFY_RESOURCE_REPLACE)
	{
		// Make sure no invalid flag combinations are used
		if (m_dwPendingChanges & MODIFY_RESOURCE_DELETE)
		{
			dec_assert(0);
			goto bailOut;
		}
		
		// Get the length of the replacement file
		if (!GetFileLength(m_sReplacementResourceFile, &lReplacementResourceLength))
			goto bailOut;

		// Set length of resource fork to the length of the replacement file...
		hqxNewHeader.lResourceForkLength = lReplacementResourceLength;
	
		// Set resource file to point to the replacement data file
		// since we're replacing this entry.
		pszResourceFile = m_sReplacementResourceFile;

		// If we're NOT modifying the data fork, then
		// set it to the existing extracted file.  If it IS 
		// being modified, then the code below will handle 
		// setting its value.
		if (!(m_dwPendingChanges & MODIFY_DATA_DELETE) &&
			!(m_dwPendingChanges & MODIFY_DATA_REPLACE))
		{
			pszDataFile = m_pszDataForkFile;
		}
	}

	// See if we have a new entry name...
	if (m_dwPendingChanges & MODIFY_RENAME_ITEM)
	{
		strcpy(hqxNewHeader.szFilename, m_sNewEntryName);
		hqxNewHeader.byFilenameLen = strlen(hqxNewHeader.szFilename);	 // Does NOT include trailing NULL!
	}

	// Create the new HQX file...
	if (HQX_SUCCESS != CreateNewBinHexBody(hqxNewHeader, pszDataFile, pszResourceFile))
		goto bailOut;

	bRetVal = true;

bailOut:
	// Close the new output file
	if (m_pOutputFile)
	{
		if (EOF == dec_fclose(m_pOutputFile))
			bRetVal = false;

		m_pOutputFile = NULL;
	}

	return bRetVal;
}


bool CHQXFile::GetFileLength(const char *pszFilename, long *plLength)
{
	FILE *pNewFile = NULL;

	if (!pszFilename || !plLength)
		return false;

	// We need to get the size of the replacement file.  I don't know if it is faster
	// to do an fopen/fseek/ftell/fclose or a single _stat() call, but we'll need to find
	// out.
	pNewFile = dec_fopen(pszFilename, "rb");
	if (!pNewFile)
		return false;

	if (0 == dec_fseek(pNewFile, 0, SEEK_END))
	{
		*plLength = dec_ftell(pNewFile);
		if (-1 != *plLength)
		{
			if (EOF == dec_fclose(pNewFile))
				return false;

			pNewFile = NULL;
			return true;
		}
	}

	// Don't need to check this dec_fclose since if we get here,
	// we're already returning an error.
	dec_fclose(pNewFile);
	pNewFile = NULL;
	return false;
}
