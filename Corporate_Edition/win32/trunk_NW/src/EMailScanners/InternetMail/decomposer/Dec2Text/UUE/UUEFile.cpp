// UUEFile.cpp : UUE support functions
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
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

#include "Support.h"
#include "UUE.h"
#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"


int CUUEParser::Close(void)
{
	int rc = LEXER_OK;

	// See if any changes have been made to the data stream.
	if (IsModified())
	{
		// To close out the changes, we need to copy everything from the
		// top unchanged offset to the EOF.
		// First seek to the end of the original data stream.
		if (dec_fseek(m_pFile, 0, SEEK_END) == 0)
		{
			m_nBottomUnchangedOffset = (size_t)dec_ftell (m_pFile);
			if (m_nBottomUnchangedOffset > m_nTopUnchangedOffset)
			{
				FILE	*stream;
				size_t	nCount;
				size_t	bytestoread;
				unsigned char buffer[64];

				// Compute the number of bytes that we need to copy.
				nCount = m_nBottomUnchangedOffset - m_nTopUnchangedOffset;

				// Open the output stream for appending...
				stream = dec_fopen(m_szOutFile, "ab");
				if (stream)
				{
					// Seek on the original stream to the top of the unchanged data.
					if (dec_fseek (m_pFile, m_nTopUnchangedOffset, SEEK_SET) == 0)
					{
						// Copy the data from the original stream to the new stream.
						while (nCount)
						{
							bytestoread = nCount;
							if (bytestoread > sizeof (buffer))
								bytestoread = sizeof (buffer);
							if (dec_fread (buffer, 1, bytestoread, m_pFile) != bytestoread)
								break;
							if (dec_fwrite (buffer, 1, bytestoread, stream) != bytestoread)
								break;
							nCount -= bytestoread;
						}
					}

					if (EOF == dec_fclose(stream))
						rc = LEXER_ERROR;

					stream = NULL;
				}
			}
		}
	}

	return rc;
}


int CUUEParser::FindFirstEntry(FILE *fpin, TEXT_FIND_DATA *pData)
{
	dec_assert(fpin);

	m_pFile = fpin;
	m_nOffset = 0;
	m_nHeaderOffset = 0;
	m_nStartOffset = 0;
	m_nEndOffset = 0;
	return (FindNextEntry(pData));
}


static void fabricate_file_name(DWORD offset, char *pszName)
{
	int		idx;
	char	hex_buf[8];
	int		hex_idx;
	int		hex_len;
	DWORD	dwTemp;
	int		this_digit;

	// Build a name string that is (in ASCII) the word "Unknown" followed
	// by the 8-digit hex value of the section offset with a .data extension.
	strcpy(pszName, "\x55\x6E\x6B\x6E\x6F\x77\x6E"); // "Unknown"
	idx = 7; // strlen("Unknown");

	// Build the hex part in a separate buffer because we have to build it
	// from right to left.
	hex_idx = sizeof(hex_buf);	// Pre-decremented, so start one past the end of the buffer.
	hex_len = 0;

	dwTemp = offset;

	do 
	{
		this_digit = dwTemp & 0x0F;
		dwTemp = (dwTemp >> 4);
		--hex_idx;

		if (this_digit > 9) 
		{
			hex_buf[hex_idx] = (this_digit - 10) + 0x41; /* A */
		}
		else 
		{
			hex_buf[hex_idx] = this_digit + 0x30; /* 0 */
		}

		++hex_len;
	} while (dwTemp != 0);

	// Pad the hex string to the left with zeros to 8 digits..
	while (hex_len < 8)
	{
		--hex_idx;
		hex_buf[hex_idx] = '\x30'; // "0"
		++hex_len;
	}

	// Append the hex string to the name.
	while (hex_len > 0) 
	{
		pszName[idx] = hex_buf[hex_idx];
		--hex_len;
		++idx;
		++hex_idx;
	}

	// Append the extension.
	pszName[idx] = '\x2E'; // "."
	++idx;
	pszName[idx] = '\x64'; // "d"
	++idx;
	pszName[idx] = '\x61'; // "a"
	++idx;
	pszName[idx] = '\x74'; // "t"
	++idx;
	pszName[idx] = '\x61'; // "a"
	++idx;
	pszName[idx] = 0;
}


int CUUEParser::FindNextEntry(TEXT_FIND_DATA *pData)
{
	bool	bGotEOL;
	size_t	nLength;
	char	szLine[RFC822_MAX_LINE_LENGTH+10];
	char	szTemp[RFC822_MAX_LINE_LENGTH+1];
	int		rc = LEXER_ERROR;
	size_t	nScanThreshold;

	// All UUE filenames are considered to be in the CP850 character set.
	m_pArchive->m_dwFilenameCharset = DEC_CHARSET_CP850;

	// Set the scan threshold to whatever the client specified, or the
	// default of 1024 bytes if the client doesn't say...
	nScanThreshold = (m_dwTextNonUUEThreshold > 0 ? m_dwTextNonUUEThreshold : 1024);

	// Compute the threshold above the current offset.
	nScanThreshold += m_nOffset;

	// Seek to the current offset of the uuencoded data stream.
	if (dec_fseek(m_pFile, m_nOffset, SEEK_SET) == 0)
	{
		while (m_nOffset < nScanThreshold)
		{
			// Read in the "begin UNIX-permission-mode filename.ext" line.
			if (ReadLine(m_pFile,
						szLine,
						RFC822_MAX_LINE_LENGTH,
						(LPINT)&nLength,
						&bGotEOL) != PARSER_ERROR)
			{
				if (bGotEOL)
				{
					// See if we can recognize this line as a UU-encoded header
					// of the form "begin UNIX-permission-mode filename.ext".
					if (IsUUECandidate (szLine, &m_dwMode, &szTemp[0], sizeof(szTemp)))
					{
						m_nStartOffset = m_nOffset;

						// UUE header found!  Save the filename from the header.
						// Also note here that m_nOffset is currently still set
						// to point to the position of the "begin ..." line.
						// This is necessary so that the ExtractEntry function
						// knows where the UUE header line starts in the data stream.
						strncpy(m_szFilename, szTemp, MAX_PATH);
						m_szFilename[MAX_PATH - 1] = 0;

						if(*m_szFilename == 0)
						{
							// An attachment with an empty file name. Fabricate one.
							fabricate_file_name(m_nOffset, m_szFilename);
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
						pData->pszSecondaryName = NULL;
						pData->pszName = (char *)malloc(strlen(m_szFilename) + 1);
						if (pData->pszName)
						{
							strcpy(pData->pszName, m_szFilename);
							rc = LEXER_OK;
						}

						break;
					}
					else
					{
						// No UUE header found.
						// Update the m_nOffset stream position to point to the
						// beginning of the next line that we are going to look at.
						m_nOffset = (size_t)dec_ftell (m_pFile);

						// Return LEXER_ERROR to allow other engines to try to identify
						// the data stream.
						rc = LEXER_ERROR;
					}
				}
				else
				{
					// No end-of-line found.  This is not a UU-encoded header line.
					// We will go ahead and look at the first couple of lines since
					// some UUENCODE programs put additional header information
					// on lines in front of the "begin" line.
					if (dec_feof(m_pFile))
					{
						// Reached end-of-file so leave and indicate that no more
						// UUE headers have been found (return LEXER_ERROR).
						rc = LEXER_ERROR;
						break;	// Exit the while() loop
					}

					// Update the m_nOffset stream position to point to the
					// beginning of the next line that we are going to look at.
					m_nOffset = (size_t)dec_ftell (m_pFile);
					// Return LEXER_ERROR to allow other engines to try to identify
					// the data stream.
					rc = LEXER_ERROR;
				}
			}
			else
			{
				break;
			}
		}
	}

	return (rc);
}


int CUUEParser::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	int			rc = PARSER_OK;

	// Preserve the offset of this item's UUE header.
	// This information is used by the DeleteEntry and
	// ReplaceEntry functions.
	m_nHeaderOffset = m_nOffset;

	rc = SkipDecodeUU(m_pFile, m_nOffset);
	
	// Update the m_nOffset stream position.
	m_nOffset = (size_t)dec_ftell (m_pFile);
	m_nBottomUnchangedOffset = m_nOffset;
	m_nEndOffset = m_nOffset;
	return rc;
}


#define UUE_EXTRACT_BUFFER_SIZE	  (8 * 1024)

int CUUEParser::ExtractEntry(char *pszFilename, FILE *fpin)
{
	int			rc = LEXER_ERROR;

	if (pszFilename)
	{
		// Preserve the offset of this item's UUE header.
		// This information is used by the DeleteEntry and
		// ReplaceEntry functions.
		m_nHeaderOffset = m_nOffset;

		rc = WriteUU(fpin, m_nOffset, pszFilename);

		// Update the m_nOffset stream position.
		m_nOffset = (size_t)dec_ftell (m_pFile);
		m_nBottomUnchangedOffset = m_nOffset;
		m_nEndOffset = m_nOffset;
	}

	return rc;
}


int CUUEParser::ReplaceEntry(TEXT_CHANGE_DATA *pData)
{
	int				rc = LEXER_ERROR;  // Assume failure
	DECRESULT		hr;
	FILE			*fpIn;
	FILE			*fpOut;
	char			*pszOutputFilename = NULL;

	// We need to encode the data here.
	pszOutputFilename = (char *)malloc(MAX_PATH);
	if (pszOutputFilename)
	{
		hr = m_pEngine->TextCreateTempFile(".dat", pszOutputFilename, m_pDecomposer);
		if (!FAILED(hr))
		{
			// Here we need to encode the data before putting it back into the UUE message.
			fpIn = dec_fopen(pData->pszFilename, "rb");
			if (fpIn)
			{
				fpOut = dec_fopen(pszOutputFilename, "wb");
				if (fpOut)
				{
					// UU-Encode the input file.
					if (EncodeUU(fpIn, fpOut, pData->pszName) == PARSER_OK)
					{
						pData->pszFilename = pszOutputFilename;
						pData->nStartOffset = m_nStartOffset;
						pData->nEndOffset = m_nEndOffset;
						hr = m_pArchive->MarkForReplace(pData);
						if (!FAILED(hr))
							rc = LEXER_OK;
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


int CUUEParser::AddEntry(TEXT_CHANGE_DATA *pData)
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


#define SCAN_BUFFER_SIZE		(8 * 1024)

int CUUEParser::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	int				rc = LEXER_ERROR;  // Assume failure
	DECRESULT		hr;
	size_t			nIndex;
	size_t			nBytesRead;
	char			*pbuffer = NULL;

	// Here we need to adjust m_nEndOffset.  It currently points to
	// whatever token we identified to represent the end of the
	// encoded section (m_nTerminator).  Scan forward from there
	// to see if we can determine the byte-offset of the end of
	// the section.
	// Allocate a buffer for the scan.
	pbuffer = (char *)malloc(SCAN_BUFFER_SIZE);
	if (!pbuffer)
		goto deleteentry_done;

	// Seek to where the termination sequence should start.
	m_nOffset = m_nEndOffset;
	if (dec_fseek(m_pFile, m_nOffset, SEEK_SET) != 0)
		goto deleteentry_done;

	// Read data in a loop until we locate the real end of the section.
	while (!dec_feof(m_pFile))
	{
		// Read as much data as we can into the buffer.
		nBytesRead = dec_fread(pbuffer, 1, SCAN_BUFFER_SIZE, m_pFile);
		if (nBytesRead < 3)
		{
			// There is no significant data past the end of the
			// encoded UUE section.  So, delete everything up to
			// the end-of-file.
			m_nEndOffset = m_nOffset;
			goto do_markfordelete;
		}

		// Scan the buffer for a termination sequence.
		// *** NOTE ***
		// If we see any data that might be non-UUE-section
		// data, we must NOT pass over it.  If we do, we might end up
		// deleting information that is NOT part of the UUE section.
		// ************
		for (nIndex = 0; nIndex <= nBytesRead - 3; nIndex++)
		{
			char	ch;

			ch = pbuffer[nIndex];

			// We can pass over spaces, tabs, carriage-returns, line-feeds,
			// and accent-graves.
			if (ch == ASC_CHR_SPACE ||
				ch == ASC_CHR_TAB ||
				ch == ASC_CHR_CR ||
				ch == ASC_CHR_LF ||
				ch == ASC_CHR_GRAVE)
				continue;

			if (ch == ASC_CHR_E ||
				ch == ASC_CHR_e)
			{
				// This *must* be the word "end".  Otherwise we have
				// to assume that this data is outside the UUE section.
				ch = pbuffer[nIndex + 1];
				if (ch == ASC_CHR_N ||
					ch == ASC_CHR_n)
				{
					ch = pbuffer[nIndex + 2];
					if (ch == ASC_CHR_D ||
						ch == ASC_CHR_d)
					{
						m_nEndOffset = m_nOffset + nIndex + 2;
						goto do_markfordelete;
					}
				}
			}

			// This character might be outside the UUE section.
			// Since we have not located an end marker at this
			// point, we are forced to assume that we need to use
			// this offset as the end of the UUE section.
			m_nEndOffset = m_nOffset + nIndex - 1;
			break;
		}

		m_nOffset += nBytesRead;
	}

	// There is no significant data past the end of the
	// encoded UUE section.  So, delete everything up to
	// the end-of-file.
	m_nEndOffset = m_nOffset;

do_markfordelete:
	pData->nStartOffset = m_nStartOffset;
	pData->nEndOffset = m_nEndOffset;
	hr = m_pArchive->MarkForDelete(pData);
	if (!FAILED(hr))
		rc = LEXER_OK;

deleteentry_done:
	if (pbuffer)
		free(pbuffer);

	return (rc);
}

