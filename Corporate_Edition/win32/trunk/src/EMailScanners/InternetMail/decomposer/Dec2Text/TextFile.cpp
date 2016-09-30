// TextFile.cpp : Main driver module for decomposing text-based data formats.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// Headers

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "TextFile.h"
#include "Support.h"

#include "MIME/MIME.h"
#include "UUE/UUE.h"
#include "HQX/HQX.h"
#include "mbox/mbox.h"
#include "RTF/RTF.h"
#include "HTTP/HTTP.h"

#include "asc_char.h"
#include "asc_ctype.h"
#include "bstricmp.h"

#define MY_BUFFER_SIZE		 (8 * 1024)

// global variables
volatile DWORD g_dwTextMaxExtractSize = 0;
volatile bool g_bTextAbort = false;
volatile bool g_bLogWarnings = false;


DWORD g_dwEnabled[MAX_TEXT_ENGINES] =
{
	DEC_TYPE_UNKNOWN,
	DEC_TYPE_UNKNOWN,
	DEC_TYPE_UNKNOWN,
	DEC_TYPE_UNKNOWN
};

/////////////////////////////////////////////////////////////////////////
// CTextFile methods

/////////////////////////////////////////////////////////////////////////
// CTextFile::CTextFile()

CTextFile::CTextFile()
{
	m_pFile = NULL;
	m_pMarkerFile = NULL;
	m_bChanged = false;
	m_bExtractOK = false;
	m_bIdentified = false;
	m_szMarkerFile[0] = 0;
	m_szFilename[0] = 0;
	m_pBuffer = NULL;
	m_nBufferBytes = 0;
	m_iBufferIndex = 0;
	m_dwType = DEC_TYPE_UNKNOWN;
	m_dwChildType = DEC_TYPE_UNKNOWN;
	m_dwFilenameCharset = DEC_CHARSET_US_ASCII;
	m_dwItemType = DEC_ITEMTYPE_NORMAL;
	m_iEngineIndex = MAX_TEXT_ENGINES;
	m_nLastIDOffset = TEXTFILE_NOT_IDENTIFIED_OFFSET;
	for (int index = 0; index < MAX_TEXT_ENGINES; index++)
		m_pLexer[index] = NULL;

	m_pDecomposer = NULL;
	m_pEngine = NULL;

	//
	// Option defaults
	//
	m_dwEnhancedID = TEXT_OPTION_NONE;
}

/////////////////////////////////////////////////////////////////////////
// CTextFile::~CTextFile()

CTextFile::~CTextFile()
{
	// The marker file should always be closed by now.
	dec_assert(m_pMarkerFile == NULL);

	if (m_pFile)
		Close(NULL, NULL);
	if (m_pBuffer)
		free(m_pBuffer);

	for (int index = 0; index < MAX_TEXT_ENGINES; index++)
	{
		if (m_pLexer[index] != NULL)
			delete m_pLexer[index];
	}
}

/////////////////////////////////////////////////////////////////////////
// CTextFile::Open()

DECRESULT CTextFile::Open(FILE *fp, IDecomposerEx *pDecomposer, CTextEngine *pEngine, IDecContainerObjectEx *pObject, DWORD *pdwType)
{
	DECRESULT hr = DECERR_CONTAINER_OPEN;

	m_pDecomposer = pDecomposer;
	m_pEngine = pEngine;

	if (m_pFile)
		Close(NULL, NULL);

	m_pFile = fp;
	if (m_pFile)
	{
		*pdwType = DEC_TYPE_UNKNOWN;

		hr = Identify(pObject, pdwType);

		// Give all the lexer/parser engines a chance to clean up.
		IdentifyComplete();

		if (hr != DEC_OK ||
			*pdwType == DEC_TYPE_UNKNOWN)
		{
			// Either an error occurred or the data was not identified.
			// In either case, close up shop and move on...
			Close(NULL, NULL);

			if (hr == DEC_OK)
				hr = S_FALSE;  // Change return code to indicate data was not identified.
		}
	}

	// Reset the last ID offset.  This resets the find-first/find-next
	// enumeration logic.
	m_nLastIDOffset = TEXTFILE_NOT_IDENTIFIED_OFFSET;

	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::Close()

DECRESULT CTextFile::Close(WORD *pwResult, char *pszNewDataFile)
{
	DECRESULT	hr = DECERR_CONTAINER_CLOSE;

	// We need to give each enabled engine a chance to do some
	// clean-up here.  Some engines may need to finalize changes
	// that are going to get fed into the Reconstruct() call below.
	for (int index = 0; index < MAX_TEXT_ENGINES; index++)
	{
		switch (g_dwEnabled[index])
		{
			case DEC_TYPE_MIME:
			case DEC_TYPE_UUE:
			case DEC_TYPE_HQX:
			case DEC_TYPE_MBOX:
				if (m_pLexer[index])
					m_EngineReturnCode[index] = m_pLexer[index]->ProcessComplete();
				break;

			default:
				// If the engine type is DEC_TYPE_UNKNOWN or
				// some other type not listed above, we do nothing.
				break;
		}
	}

	// See if any changes have been made to the data stream.
	if (IsModified())
	{
		// There is at least one modification that needs to be made to the
		// original file.
		if (Reconstruct())
		{
			if (pwResult && pszNewDataFile)
			{
				strcpy(pszNewDataFile, m_szOutputFile);
				*pwResult = DECRESULT_TO_BE_REPLACED;
			}

			hr = DEC_OK;
		}
	}
	else
	{
		hr = DEC_OK;
	}

	// If a marker file is still open at this point, it means that
	// an error occurred at some point between the time we received
	// the first change, replace, or delete request and now.
	// Since we are finished with the marker file at this point,
	// just close it and delete it.
	if (m_pMarkerFile)
	{
		dec_fclose(m_pMarkerFile);
		dec_remove(m_szMarkerFile);
		m_pMarkerFile = NULL;
	}

	// NOTE: Do NOT actually close the input file here.  The caller is
	// responsible for closing it.
	m_pFile = NULL;

	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::Reconstruct()

bool CTextFile::Reconstruct(void)
{
	bool		rc = false;	// Assume failure.
	size_t		nStartOffset;
	size_t		nUnchangedOffset = 0;
	size_t		nFileSize;
	size_t		nBytesLeft;
	FILE		*fpin = NULL;
	FILE		*fpout = NULL;
	unsigned char *pmarks = NULL;
	unsigned char *ptr;
	TEXT_MARKER tm;
	TEXT_MARKER *ptm;
	TEXT_MARKER *pNextBlock;

	// Open the output file.
	fpout = dec_fopen(m_szOutputFile, "wb");
	if (!fpout)
		goto done_reconstruct;

	if (dec_fseek(m_pMarkerFile, 0, SEEK_END) != 0)
		goto done_reconstruct;

	// Get the size of the marker file.  If it is 0 or if it is
	// bigger than 1MB (an arbitrary limit), don't do the
	// reconstruction.
	nFileSize = dec_ftell(m_pMarkerFile);
	if (nFileSize == 0 || nFileSize > (1024 * 1024))
		goto done_reconstruct;

	// Seek back to the start of the marker file.
	if (dec_fseek(m_pMarkerFile, 0, SEEK_SET) != 0)
		goto done_reconstruct;

	// Allocate a buffer to read the entire marker file into.
	pmarks = (unsigned char *)malloc(nFileSize);
	if (!pmarks)
		goto done_reconstruct;

	if (dec_fread(pmarks, 1, nFileSize, m_pMarkerFile) != nFileSize)
		goto done_reconstruct;

reconstruct_loop:
	// Walk through the entire set of marker data looking for the next
	// lowest offset in a marked block.
	ptr = pmarks;
	nBytesLeft = nFileSize;
	nStartOffset = 0xffffffff;
	pNextBlock = NULL;
	while (nBytesLeft)
	{
		// To avoid bus-fault problems due to this structure being
		// unaligned, make a local copy that is guaranteed to be aligned.
		memcpy(&tm, ptr, sizeof(TEXT_MARKER));
		ptm = (TEXT_MARKER *)ptr;
		dec_assert(nBytesLeft >= sizeof(TEXT_MARKER));
		nBytesLeft -= sizeof(TEXT_MARKER);
		ptr += sizeof(TEXT_MARKER);

		// Check for special start offset (-1) that indicates we have already
		// processed this marked block.
		if (tm.nStartOffset == (size_t)-1)
		{
			if (tm.nDataSize)
			{
				dec_assert(nBytesLeft >= tm.nDataSize);
				nBytesLeft -= tm.nDataSize;
				ptr += tm.nDataSize;
			}
			continue;
		}

		// Execute the command.
		switch (tm.dwCommand)
		{
			case MARKER_COMMAND_COPY:
			case MARKER_COMMAND_DELETE:
			case MARKER_COMMAND_REPLACE_FILE:
			case MARKER_COMMAND_REPLACE_DATA:
			case MARKER_COMMAND_ADD:
			{
				dec_assert(nBytesLeft >= tm.nDataSize);
				nBytesLeft -= tm.nDataSize;
				ptr += tm.nDataSize;

				if (tm.nStartOffset <= nStartOffset)
				{
					pNextBlock = ptm;
					nStartOffset = tm.nStartOffset;
				}
				break;
			}

			default:
				dec_assert(0);
				pNextBlock = NULL;	// Prevent us from continuing with bogus data
				break;
		}
	}

	rc = true;	// Assume the logic below will succeed.
	ptm = pNextBlock;
	if (!ptm)
		goto done_reconstruct;

	// To avoid bus-fault problems due to this structure being
	// unaligned, make a local copy that is guaranteed to be aligned.
	memcpy(&tm, ptm, sizeof(TEXT_MARKER));
	ptr = (unsigned char *)ptm;
	ptr += sizeof(TEXT_MARKER);

	if (tm.nStartOffset > nUnchangedOffset)
	{
		// There is an unchanged block of data below this marked section
		// that needs to be copied.  Do so now.
		if (!CopyBlock(m_pFile, fpout, nUnchangedOffset, tm.nStartOffset - 1))
		{
			rc = false;
			goto done_reconstruct;
		}

		nUnchangedOffset = tm.nEndOffset + 1;
	}

	// Execute the command.
	switch (tm.dwCommand)
	{
		case MARKER_COMMAND_COPY:
		{
			// Copy the specified block from m_pFile.
#ifdef _DEBUG
			printf("Copying block: 0x%08x to 0x%08x\n", tm.nStartOffset, tm.nEndOffset);
#endif
			if (!CopyBlock(m_pFile, fpout, tm.nStartOffset, tm.nEndOffset))
			{
				rc = false;
				break;
			}
			break;
		}

		case MARKER_COMMAND_DELETE:
		{
			// To delete this block we simply don't do anything.
#ifdef _DEBUG
			printf("Deleting block: 0x%08x to 0x%08x\n", tm.nStartOffset, tm.nEndOffset);
#endif
			break;
		}

		case MARKER_COMMAND_REPLACE_FILE:
		{
			char	*pszFilename;
			size_t	nReplacementFileSize;

			// Get a pointer to the filename.
			dec_assert((tm.nDataSize > 0) && (tm.nDataSize < MAX_PATH));
			pszFilename = (char *)ptr;

#ifdef _DEBUG
			printf("Replacing block: 0x%08x to 0x%08x\n with %s\n", tm.nStartOffset, tm.nEndOffset, pszFilename);
#endif

			// Now that we have the filename, open the file and copy the data.
			fpin = dec_fopen(pszFilename, "rb");
			if (fpin == NULL)
			{
				dec_remove(pszFilename);  // Delete the replacment file.
				rc = false;
				break;
			}

			if (dec_fseek(fpin, 0, SEEK_END) != 0)
			{
				dec_fclose(fpin);
				fpin = NULL;
				dec_remove(pszFilename);  // Delete the replacment file.
				rc = false;
				break;
			}

			nReplacementFileSize = dec_ftell(fpin);
			if (nReplacementFileSize)
				nReplacementFileSize--;	// Turn the size into an offset.

			//////////////////////////////////////////////////////////////
			// **DEBUG FOR PAUL** - Suggested fix for STS#377288
			// When trying to replace a child in a MIME message and
			// that child is zero bytes (e.g. you delete the only child
			// inside of a LHA container that is a child of a MIME message)
			// the CopyBlock function will fail (due to there being 0 bytes
			// to copy.  Instead, detect that case and don't try and do the
			// copy.
			//
			// Paul - if this fix is ok with you, please remove this comment.

			// Write the replacement data.
			if (nReplacementFileSize > 0)
			{
				rc = CopyBlock(fpin, fpout, 0, nReplacementFileSize);
			}

			// Close the replacement file.
			dec_fclose(fpin);
			fpin = NULL;

			// Delete the replacement file now that we are done with it.
			dec_remove(pszFilename);
			break;
		}

		case MARKER_COMMAND_REPLACE_DATA:
		{
#ifdef _DEBUG
			printf("Replacing block: 0x%08x to 0x%08x\n with %s\n", tm.nStartOffset, tm.nEndOffset, ptr);
#endif

			// Write the replacement data.
			if (dec_fwrite(ptr, 1, tm.nDataSize, fpout) != tm.nDataSize)
				rc = false;
			break;
		}

		case MARKER_COMMAND_ADD:
		{
			char	*pszFilename;
			size_t	nReplacementFileSize;

			// Get a pointer to the filename.
			dec_assert((tm.nDataSize > 0) && (tm.nDataSize < MAX_PATH));
			pszFilename = (char *)ptr;

#ifdef _DEBUG
			printf("Adding block from file: %s\n", pszFilename);
#endif

			// Now that we have the filename, open the file and copy the data.
			fpin = dec_fopen(pszFilename, "rb");
			if (fpin == NULL)
			{
				dec_remove(pszFilename);  // Delete the add file.
				rc = false;
				break;
			}

			if (dec_fseek(fpin, 0, SEEK_END) != 0)
			{
				dec_fclose(fpin);
				fpin = NULL;
				dec_remove(pszFilename);  // Delete the add file.
				rc = false;
				break;
			}

			nReplacementFileSize = dec_ftell(fpin);
			if (nReplacementFileSize)
				nReplacementFileSize--;	// Turn the size into an offset.

			// Write the new data.
			rc = CopyBlock(fpin, fpout, 0, nReplacementFileSize);

			// Close the add file.
			dec_fclose(fpin);
			fpin = NULL;

			// Delete the add file now that we are done with it.
			dec_remove(pszFilename);
			break;
		}

		default:
		{
			dec_assert(0);
			rc = false;
			break;
		}
	}

	if (!rc)
		goto done_reconstruct;	// Something went wrong.

	// Set nUnchangedOffset to the offset of the next unchanged byte.
	if (tm.nEndOffset > nUnchangedOffset)
	{
		nUnchangedOffset = tm.nEndOffset + 1;
	}

	// Disable this block from being processed again.
	tm.nStartOffset = (size_t)-1;
	memcpy(ptm, &tm, sizeof(TEXT_MARKER));
	goto reconstruct_loop;

done_reconstruct:
	if (rc)
	{
		// Seek to the EOF on the original data to get the size of the file.
		if (dec_fseek(m_pFile, 0, SEEK_END) != 0)
		{
			rc = false;
			goto done_reconstruct;
		}

		nFileSize = dec_ftell(m_pFile);
		if (nFileSize)
			nFileSize--;  // Turn size into offset.

		// Are there any unchanged bytes beyond the last modification that
		// need to be copied?
		if (nFileSize >= nUnchangedOffset)
		{
			// Yes, there are.  So, copy them.
			rc = CopyBlock(m_pFile, fpout, nUnchangedOffset, nFileSize);
		}
	}

	if (m_pMarkerFile)
	{
		dec_fclose(m_pMarkerFile);
		dec_remove(m_szMarkerFile);
		m_pMarkerFile = NULL;
	}

	if (pmarks)
		free(pmarks);

	if (fpout)
	{
		if (EOF == dec_fclose(fpout))
			rc = false;

		if (!rc)
			dec_remove(m_szOutputFile);

		fpout = NULL;
	}

	return rc;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::CopyBlock()

bool CTextFile::CopyBlock(FILE *fpin, FILE *fpout, size_t nStartOffset, size_t nEndOffset)
{
	bool	rc = false;	// Assume failure.
	size_t	nBytesTotal;
	size_t	nBytesToRead;
	size_t	nBytesRead;
	unsigned char *pbuffer = NULL;

	// First, seek to the start of the block in the original data.
	if (dec_fseek(fpin, nStartOffset, SEEK_SET) != 0)
		goto done_copy_block;

	// Do the copy.
	pbuffer = (unsigned char *)malloc(MY_BUFFER_SIZE);
	if (!pbuffer)
		goto done_copy_block;

	nBytesTotal = nEndOffset - nStartOffset + 1;
	while (nBytesTotal)
	{
		nBytesToRead = MY_BUFFER_SIZE;
		if (nBytesToRead > nBytesTotal)
			nBytesToRead = nBytesTotal;

		nBytesRead = dec_fread(pbuffer, 1, nBytesToRead, fpin);
		if (nBytesRead == 0)
			goto done_copy_block;
		if (dec_fwrite(pbuffer, 1, nBytesRead, fpout) != nBytesRead)
			goto done_copy_block;

		nBytesTotal -= nBytesRead;
	}

	rc = true;

done_copy_block:
	if (pbuffer)
		free(pbuffer);
	return (rc);
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::Identify()

DECRESULT CTextFile::Identify(IDecContainerObjectEx *pObject, DWORD *pdwType)
{
	unsigned char	uchByte;
	int				index = 0;
	size_t			nFileSize = 0;
	size_t			nOffset = 0;
	bool			bAllDisabled = true;  // Assume all engines are disabled
	DWORD			dwValue;
	DWORD			dwMaxScanBytes;
	int				rc;

	// Set any global Text engine options here...
	dwValue = 0;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_ENABLE_MIME_ENGINE, &dwValue)))
	{
		if (dwValue)
			g_dwEnabled[index++] = DEC_TYPE_MIME;
	}

	dwValue = 0;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_ENABLE_UUE_ENGINE, &dwValue)))
	{
		if (dwValue)
			g_dwEnabled[index++] = DEC_TYPE_UUE;
	}

	dwValue = 0;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_ENABLE_BINHEX_ENGINE, &dwValue)))
	{
		if (dwValue)
			g_dwEnabled[index++] = DEC_TYPE_HQX;
	}

	dwValue = 0;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_ENABLE_MBOX_ENGINE, &dwValue)))
	{
		if (dwValue)
			g_dwEnabled[index++] = DEC_TYPE_MBOX;
	}

/* Add RTF and HTTP when the engines are ready...
	dwValue = 0;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_ENABLE_RTF_ENGINE, &dwValue)))
	{
		if (dwValue)
			g_dwEnabled[index++] = DEC_TYPE_RTF;
	}

	dwValue = 0;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_ENABLE_HTTP_ENGINE, &dwValue)))
	{
		if (dwValue)
			g_dwEnabled[index++] = DEC_TYPE_HTTP;
	}
*/

	// Set any global Text engine options here...
	dwValue = 0;
	dwMaxScanBytes = 0;
	if (SUCCEEDED(m_pDecomposer->GetOption(DEC_OPTION_MAX_TEXT_SCAN_BYTES, &dwValue)))
		dwMaxScanBytes = dwValue;

	// First, check to see if we have any engines that are enabled.
	for (index = 0; index < MAX_TEXT_ENGINES; index++)
	{
		// Reset the identification offsets of all engines.
		m_nIDOffset[index] = TEXTFILE_NOT_IDENTIFIED_OFFSET;
		m_EngineReturnCode[index] = LEXER_DONE;

		if (g_dwEnabled[index] != DEC_TYPE_UNKNOWN)
		{
			bAllDisabled = false;
			m_EngineReturnCode[index] = LEXER_OK;

			// Based on the type of enabled engine, create the
			// lexer object that we will use to analyze the data
			// specifically for this format.
			switch (g_dwEnabled[index])
			{
				case DEC_TYPE_MIME:
					m_pLexer[index] = new CMIMELexer(dwMaxScanBytes);
					break;

				case DEC_TYPE_UUE:
					m_pLexer[index] = new CUUELexer(dwMaxScanBytes);
					break;

				case DEC_TYPE_HQX:
					m_pLexer[index] = new CHQXLexer(dwMaxScanBytes);
					break;

				case DEC_TYPE_MBOX:
					m_pLexer[index] = new CMBOXLexer(dwMaxScanBytes);
					break;

/*
				case DEC_TYPE_RTF:
					m_pLexer[index] = new CRTFLexer;
					break;

				case DEC_TYPE_HTTP:
					m_pLexer[index] = new CHTTPLexer;
					break;
*/
			}

			if (m_pLexer[index] == NULL)
			{
				// We failed to construct the ILexer interface for the
				// engine.  Return an out-of-memory error.
				return DECERR_OUT_OF_MEMORY;
			}

			if (m_pLexer[index]->Init(this, m_pEngine, m_pDecomposer, pObject) == false)
			{
				return DECERR_OUT_OF_MEMORY;
			}
		}
	}

	if (bAllDisabled)
		return DEC_OK;	// All of the engines are disabled, so just leave.

	// Initialize the main processing loop.
	m_nBufferBytes = 0;
	m_iBufferIndex = 0;
	m_dwType = DEC_TYPE_UNKNOWN;
	m_dwChildType = DEC_TYPE_UNKNOWN;
	m_bIdentified = false;
	m_iEngineIndex = MAX_TEXT_ENGINES;
	m_nLastIDOffset = TEXTFILE_NOT_IDENTIFIED_OFFSET;

	if (!m_pBuffer)
	{
		m_pBuffer = (unsigned char *)malloc(MY_BUFFER_SIZE);
		if (!m_pBuffer)
			return DECERR_OUT_OF_MEMORY;
	}

	// Make sure we start at the beginning of the stream.
	if (dec_fseek(m_pFile, 0, SEEK_SET) != 0)
		return DECERR_CONTAINER_OPEN;

	while (true)
	{
		if (m_nBufferBytes == 0)
		{
			// We need more input bytes.

			// For responsiveness reasons, check the abort flag here.
			// If we don't do this here, it could be a long time before the abort
			// flag gets checked (for very large files).
			if (g_bTextAbort)
				return DECERR_USER_CANCEL;

			// Read the next MY_BUFFER_SIZE bytes.
			m_iBufferIndex = 0;
			m_nBufferBytes = dec_fread(m_pBuffer, 1, MY_BUFFER_SIZE, m_pFile);
			nFileSize += m_nBufferBytes;
			if (m_nBufferBytes == 0)
			{
				// Failed to read any more data.  Send an EOF token to each
				// of the enabled text engines.  It does not matter
				// what order these calls are made in.
				for (int index = 0; index < MAX_TEXT_ENGINES; index++)
				{
					rc = m_EngineReturnCode[index]; 
					if (rc == LEXER_OK ||
						rc == LEXER_DONE ||
						rc == LEXER_IDENTIFIED_DONE)
					{
						switch (g_dwEnabled[index])
						{
							case DEC_TYPE_MIME:
							case DEC_TYPE_UUE:
							case DEC_TYPE_HQX:
							case DEC_TYPE_MBOX:
								rc = m_pLexer[index]->ConsumeEOF(nFileSize);
								m_EngineReturnCode[index] = rc;
								if (rc == LEXER_IDENTIFIED ||
									rc == LEXER_IDENTIFIED_DONE)
								{
									m_nIDOffset[index] = nOffset;
									if (m_nLastIDOffset == TEXTFILE_NOT_IDENTIFIED_OFFSET)
									{
										m_nLastIDOffset = nOffset;
										m_dwType = g_dwEnabled[index];
									}
									else if (nOffset < m_nLastIDOffset)
									{
										m_nLastIDOffset = nOffset;
										m_dwType = g_dwEnabled[index];
									}
								}
								break;

							default:
								// If the engine type is DEC_TYPE_UNKNOWN or
								// some other type not listed above, we do nothing.
								break;
						}
					}
				}

				// See if this was a read error or a regular EOF condition.
				if (!dec_feof(m_pFile))
					return DECERR_CONTAINER_OPEN;

				*pdwType = m_dwType;
				break;
			}
		}

		// Send the next byte to each enabled text engine.
		uchByte = m_pBuffer[m_iBufferIndex];

		// If the byte is <= 0x08 and we have not identified the file yet and
		// we are not configured to aggressively identify text files, then this
		// file is not in a text format that we recognize.
		if (uchByte < 0x08 && !m_bIdentified && (m_dwEnhancedID & TEXT_OPTION_ABORT_ON_BINARY))
		{
			*pdwType = DEC_TYPE_UNKNOWN;
			return (DEC_OK);
		}

		// Call each of the enabled text engines here.  It does not matter
		// what order these calls are made in.
		bAllDisabled = true;  // Assume all engines are done.
		for (int index = 0; index < MAX_TEXT_ENGINES; index++)
		{
			if (m_EngineReturnCode[index] == LEXER_OK)
			{
				// At least one engine is not finished lexing/parsing.
				bAllDisabled = false;

				// One of the engines has identified the data.
				// If we are not configured to do enhanced identification,
				// stop calling all of the other engines - only continue
				// to call the engine that identified the data.  This
				// is a performance enhancement that trades off detection
				// for better speed.
				if (m_bIdentified &&
					!(m_dwEnhancedID & TEXT_OPTION_PROCESS_MULTIPLE_ID) &&
					g_dwEnabled[index] != m_dwType)
				{
					// Set this engine's return code to LEXER_DONE so that
					// we don't have to keep checking for this condition.
					m_EngineReturnCode[index] = LEXER_DONE;
					continue;
				}

				switch (g_dwEnabled[index])
				{
					case DEC_TYPE_MIME:
					case DEC_TYPE_UUE:
					case DEC_TYPE_HQX:
					case DEC_TYPE_MBOX:
						if (m_pLexer[index]->m_bWantBytes)
							rc = m_pLexer[index]->ConsumeByte(uchByte);
						else
							rc = m_pLexer[index]->ConsumeBuffer(m_pBuffer, &m_nBufferBytes, &m_iBufferIndex, &nOffset);

						m_EngineReturnCode[index] = rc;
						if (rc == LEXER_IDENTIFIED ||
							rc == LEXER_IDENTIFIED_DONE)
						{
							m_bIdentified = true;

							m_nIDOffset[index] = nOffset;
							if (m_nLastIDOffset == TEXTFILE_NOT_IDENTIFIED_OFFSET)
							{
								m_nLastIDOffset = nOffset;
								m_dwType = g_dwEnabled[index];
							}
							else if (nOffset < m_nLastIDOffset)
							{
								m_nLastIDOffset = nOffset;
								m_dwType = g_dwEnabled[index];
							}

							if (rc == LEXER_IDENTIFIED)
								m_EngineReturnCode[index] = LEXER_OK;
							else
								m_EngineReturnCode[index] = LEXER_DONE;
						}
						break;

					default:
						// If the engine type is DEC_TYPE_UNKNOWN or
						// some other type not listed above, we do nothing.
						m_EngineReturnCode[index] = LEXER_DONE;
						break;
				}
			}
		}

		// If all engines are done, fall out.
		if (bAllDisabled)
		{
			// Since all of the engines have finished parsing, the last thing
			// we need to always do is emit an EOF token.  This is done because
			// some of the parsers may not care about seeing any more data, but
			// they may care about seeing where the EOF is.
			if (dec_fseek(m_pFile, 0, SEEK_END) != 0)
				return (DECERR_CONTAINER_OPEN);

			nFileSize = dec_ftell(m_pFile);
			if (nFileSize == (size_t)-1)
				return (DECERR_CONTAINER_OPEN);

			// Send an EOF token to each of the enabled text engines.  It does not matter
			// what order these calls are made in.
			for (int index = 0; index < MAX_TEXT_ENGINES; index++)
			{
				rc = m_EngineReturnCode[index]; 
				if (rc == LEXER_OK ||
					rc == LEXER_DONE ||
					rc == LEXER_IDENTIFIED_DONE)
				{
					switch (g_dwEnabled[index])
					{
						case DEC_TYPE_MIME:
						case DEC_TYPE_UUE:
						case DEC_TYPE_HQX:
						case DEC_TYPE_MBOX:
							rc = m_pLexer[index]->ConsumeEOF(nFileSize);
							m_EngineReturnCode[index] = rc;
							if (rc == LEXER_IDENTIFIED ||
								rc == LEXER_IDENTIFIED_DONE)
							{
								m_nIDOffset[index] = nOffset;
								if (m_nLastIDOffset == TEXTFILE_NOT_IDENTIFIED_OFFSET)
								{
									m_nLastIDOffset = nOffset;
									m_dwType = g_dwEnabled[index];
								}
								else if (nOffset < m_nLastIDOffset)
								{
									m_nLastIDOffset = nOffset;
									m_dwType = g_dwEnabled[index];
								}
							}
							break;

						default:
							// If the engine type is DEC_TYPE_UNKNOWN or
							// some other type not listed above, we do nothing.
							break;
					}
				}
			}

			*pdwType = m_dwType;
			break;
		}

		// Decrement the number of bytes left in the buffer for us to consume.
		m_nBufferBytes--;

		// Increment the buffer index.
		m_iBufferIndex++;

		nOffset++;	// Increment the byte offset of the next byte to process.
	}

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::IdentifyComplete()

void CTextFile::IdentifyComplete(void)
{
	// Call each of the enabled engines and give them a chance to
	// do some clean-up.
	for (int index = 0; index < MAX_TEXT_ENGINES; index++)
	{
		switch (g_dwEnabled[index])
		{
			case DEC_TYPE_MIME:
			case DEC_TYPE_UUE:
			case DEC_TYPE_HQX:
			case DEC_TYPE_MBOX:
				if (m_pLexer[index])
					m_pLexer[index]->LexComplete();
				break;

			default:
				// If the engine type is DEC_TYPE_UNKNOWN or
				// some other type not listed above, we do nothing.
				break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::GetInterface()

DECRESULT CTextFile::GetInterface(void **ppInterface)
{
	*ppInterface = NULL;

	// Get the public interface for the engine type specified by m_dwType.
	for (int index = 0; index < MAX_TEXT_ENGINES; index++)
	{
		if (g_dwEnabled[index] == m_dwType &&
			m_pLexer[index])
		{
			// Call the engine that we need an interface for.
			m_pLexer[index]->GetInterface(ppInterface);
			break;
		}
	}

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::GetFirstEntry()

DECRESULT CTextFile::GetFirstEntry(TEXT_FIND_DATA *pData)
{
	// Here we need to decide which engine is going to handle this
	// series of find-first/find-next calls.
	if (!GetEnumerationEngine())
	{
		// There are no more engines that have identified the data.
		// We return DECERR_CHILD_ACCESS to signify this fact.
		return (DECERR_CHILD_ACCESS);
	}

	// Call the engine that needs to do the enumeration.
	if (m_iEngineIndex < MAX_TEXT_ENGINES)
	{
		m_EngineReturnCode[m_iEngineIndex] = m_pLexer[m_iEngineIndex]->FindFirstEntry(m_pFile, pData);
		if (m_EngineReturnCode[m_iEngineIndex] == LEXER_OK)
		{
//#ifdef _DEBUG
//			DumpFindData(pData);
//#endif
			return DEC_OK;
		}
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::GetNextEntry()

DECRESULT CTextFile::GetNextEntry(TEXT_FIND_DATA *pData)
{
	// Call the engine that needs to do the enumeration.
	if (m_iEngineIndex < MAX_TEXT_ENGINES)
	{
		m_EngineReturnCode[m_iEngineIndex] = m_pLexer[m_iEngineIndex]->FindNextEntry(pData);
		if (m_EngineReturnCode[m_iEngineIndex] == LEXER_OK)
		{
//#ifdef _DEBUG
//			DumpFindData(pData);
//#endif
			return DEC_OK;
		}
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::SkipExtractEntry()

DECRESULT CTextFile::SkipExtractEntry(TEXT_FIND_DATA *pData)
{
	// Call the engine that needs to do the enumeration.
	if (m_iEngineIndex < MAX_TEXT_ENGINES)
	{
		m_EngineReturnCode[m_iEngineIndex] = m_pLexer[m_iEngineIndex]->SkipExtractEntry(pData);
		if (m_EngineReturnCode[m_iEngineIndex] == LEXER_OK)
		{
			return DEC_OK;
		}

		if (m_EngineReturnCode[m_iEngineIndex] == LEXER_USER_ABORT)
			return DECERR_USER_CANCEL;

		if (m_EngineReturnCode[m_iEngineIndex] == LEXER_MAX_EXTRACT_SIZE)
			return DECERR_CHILD_MAX_SIZE;

		if (m_EngineReturnCode[m_iEngineIndex] == LEXER_ENCRYPTED)
			return DECERR_CHILD_DECRYPT;
	}

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////
// CTextFile::GetEnumerationEngine()

bool CTextFile::GetEnumerationEngine(void)
{
	bool	rc = false;	// Assume data was not identified.
	int		index;

	// Reset the last ID offset.
	m_nLastIDOffset = TEXTFILE_NOT_IDENTIFIED_OFFSET;

	for (index = 0; index < MAX_TEXT_ENGINES; index++)
	{
		if (g_dwEnabled[index] != DEC_TYPE_UNKNOWN)
		{
			// This engine is enabled.  Now see if it identified the data.
			if (m_nIDOffset[index] != TEXTFILE_NOT_IDENTIFIED_OFFSET)
			{
				// Yes, the engine *did* identify the data.
				// If the offset at which the identification occurred is
				// less than the offset we have a prior identification, then
				// this engine needs to enumerate the data first.
				if ((m_nLastIDOffset == TEXTFILE_NOT_IDENTIFIED_OFFSET) ||
					(m_nLastIDOffset >= m_nIDOffset[index]))
				{
					m_iEngineIndex = index;
					m_nLastIDOffset = m_nIDOffset[index];
					rc = true;
				}
			}
		}
	}

	if (rc)
	{
		// Reset this engine's identified offset so that we
		// won't try the same engine more than once.
		m_nIDOffset[m_iEngineIndex] = TEXTFILE_NOT_IDENTIFIED_OFFSET;
	}
	else
	{
		// This signifies that there are no more engines that have identified
		// the data.
		m_iEngineIndex = MAX_TEXT_ENGINES;
	}

	return (rc);
}


#ifdef _DEBUG
void CTextFile::DumpFindData(TEXT_FIND_DATA *pData)
{
	printf("Found entry:\n");
	printf("  pszName:          %s\n", (pData->pszName == NULL) ? "" : pData->pszName);
	printf("  pszSecondaryName: %s\n", (pData->pszSecondaryName == NULL) ? "" : pData->pszSecondaryName);
	printf("  dwNameCharset:    %d\n", pData->dwNameCharset);
	printf("  nCompressedSize:  %d\n", pData->nCompressedSize);
	printf("  nUncompressedSize:%d\n", pData->nUncompressedSize);
	printf("  dwType:           %d\n", pData->dwType);
	printf("  dwAttributes:     0x%08x\n", pData->dwAttributes);
	printf("  dwAttrType:       %d\n", pData->dwAttrType);
	printf("  YY/MM/DD:         %d %d %d\n", pData->dwYear, pData->dwMonth, pData->dwDay);
	printf("  HH/MM/SS/ss:      %d %d %d %d\n", pData->dwHour, pData->dwMinute, pData->dwSecond, pData->dwMillisecond);
}
#endif


/////////////////////////////////////////////////////////////////////////////
// CTextFile::GetEntryName()

DECRESULT CTextFile::GetEntryName(char *pszName, WORD wNameSize)
{
	DECRESULT hr = E_FAIL;

	if (pszName)
	{
		if (strlen(m_szFilename) + 1 <= wNameSize) 
		{
			strcpy (pszName, m_szFilename);
			hr = S_OK;
		}
		else
		{
			pszName[0] = 0;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTextFile::GetEntryAttributes()

DECRESULT CTextFile::GetEntryAttributes(DWORD *pdwAttrs)
{
	if (!pdwAttrs)
		return E_FAIL;

//    *pdwAttrs = m_dwMode;
	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTextFile::ExtractEntry()

DECRESULT CTextFile::ExtractEntry(const char *pszFilename)
{
	DECRESULT hr = DEC_OK;

	// Call the engine that needs to do the extraction.
	m_EngineReturnCode[m_iEngineIndex] =
		m_pLexer[m_iEngineIndex]->ExtractEntry((char *)pszFilename, m_pFile);
	if (m_EngineReturnCode[m_iEngineIndex] == LEXER_OK)
	{
		SetExtractOK(true);
		return hr;
	}

	// Do not change the ExtractOK flag here - it is a one-way
	// flag.  It should only change from false to true - never
	// from true to false.
	//
	// Map lexer errors to an appropriate DECRESULT
	switch(m_EngineReturnCode[m_iEngineIndex])
	{
		case PARSER_USER_ABORT:
			hr = DECERR_USER_CANCEL;
			break;

		case PARSER_MAX_EXTRACT_SIZE:
			hr = DECERR_CHILD_MAX_SIZE;
			break;

		case PARSER_ENCRYPTED:
			hr = DECERR_CHILD_DECRYPT;
			break;

		default:
			hr = DECERR_CHILD_EXTRACT;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTextFile::DeleteEntry()

DECRESULT CTextFile::DeleteEntry(TEXT_CHANGE_DATA *pData)
{
	// Call the engine that needs to do the deletion.
	m_EngineReturnCode[m_iEngineIndex] =
		m_pLexer[m_iEngineIndex]->DeleteEntry(pData);
	if (m_EngineReturnCode[m_iEngineIndex] == LEXER_OK)
		return DEC_OK;

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////////
// CTextFile::ReplaceEntry()

DECRESULT CTextFile::ReplaceEntry(TEXT_CHANGE_DATA *pData, bool *pbKeepChildDataFile, bool *pbKeepChildNewDataFile)
{
	// Call the engine that needs to do the replacement.
	m_EngineReturnCode[m_iEngineIndex] = m_pLexer[m_iEngineIndex]->ReplaceEntry(pData, 
																				pbKeepChildDataFile,
																				pbKeepChildNewDataFile);
	if (m_EngineReturnCode[m_iEngineIndex] == LEXER_OK)
		return DEC_OK;

	return DECERR_GENERIC;
}


/////////////////////////////////////////////////////////////////////////////
// CTextFile::BeginChanges()
//
// Set the name of the output file.
// Must be called prior to DeleteEntry or ReplaceEntry.

DECRESULT CTextFile::BeginChanges(const char *szOutFile)
{
	DECRESULT	hr;

	dec_assert(szOutFile != NULL);
	dec_assert(strlen(szOutFile) < MAX_PATH);
	strncpy(m_szOutputFile, szOutFile, MAX_PATH);
	m_szOutputFile[MAX_PATH - 1] = 0;

	// Create an intermediate marker file to track the changes.
	hr = m_pEngine->TextCreateTempFile(".mrk", m_szMarkerFile, m_pDecomposer);
	if (FAILED(hr))
		return DECERR_GENERIC;

	// Create and truncate to 0 bytes the marker file.
	// Note we open the marker file for reading AND writing because
	// once the marker file has been written, we will normally turn
	// around and read it as we re-construct the original file with
	// the modifications specified in the marker file.
	m_pMarkerFile = dec_fopen(m_szMarkerFile, "w+b");
	if (m_pMarkerFile == NULL)
	{
		dec_remove(m_szMarkerFile);
		return DECERR_GENERIC;
	}

	return DEC_OK;
}


bool CTextFile::WriteMarker(TEXT_MARKER *pMarker)
{
	bool	rc = false;	// Assume failure

	// The output file must already be open.
	dec_assert(m_pMarkerFile);
	if (m_pMarkerFile == NULL)
		goto done_write_marker;

	if (dec_fwrite(pMarker, 1, sizeof(TEXT_MARKER), m_pMarkerFile) != sizeof(TEXT_MARKER))
		goto done_write_marker;

	rc = true;

done_write_marker:
	return (rc);
}


DECRESULT CTextFile::MarkForDelete(TEXT_CHANGE_DATA *pData)
{
	TEXT_MARKER		tm;

	tm.dwCommand = MARKER_COMMAND_DELETE;
	tm.nStartOffset = pData->nStartOffset;
	tm.nEndOffset = pData->nEndOffset;
	tm.nDataSize = 0;
	if (!WriteMarker(&tm))
		return DECERR_GENERIC;

	// Set the modified flag to tell the Close routine that it needs to
	// re-construct the original file based on the text markers.
	SetModified(true);
	return (DEC_OK);
}


DECRESULT CTextFile::MarkForReplace(TEXT_CHANGE_DATA *pData)
{
	TEXT_MARKER		tm;

	if (pData->pszData)
	{
		tm.dwCommand = MARKER_COMMAND_REPLACE_DATA;
		tm.nDataSize = strlen(pData->pszData);
	}
	else
	{
		tm.dwCommand = MARKER_COMMAND_REPLACE_FILE;
		tm.nDataSize = strlen(pData->pszFilename) + 1;
	}

	tm.nStartOffset = pData->nStartOffset;
	tm.nEndOffset = pData->nEndOffset;
	if (!WriteMarker(&tm))
		return DECERR_GENERIC;

	if (pData->pszData)
	{
		if (dec_fwrite(pData->pszData, 1, tm.nDataSize, m_pMarkerFile) != tm.nDataSize)
			return DECERR_GENERIC;
	}
	else
	{
		if (dec_fwrite(pData->pszFilename, 1, tm.nDataSize, m_pMarkerFile) != tm.nDataSize)
			return DECERR_GENERIC;
	}

	// Set the modified flag to tell the Close routine that it needs to
	// re-construct the original file based on the text markers.
	SetModified(true);
	return (DEC_OK);
}


DECRESULT CTextFile::MarkForAdd(TEXT_CHANGE_DATA *pData)
{
	TEXT_MARKER		tm;

	tm.dwCommand = MARKER_COMMAND_ADD;
	tm.nStartOffset = pData->nStartOffset;
	tm.nEndOffset = pData->nEndOffset;
	tm.nDataSize = strlen(pData->pszFilename) + 1;
	if (!WriteMarker(&tm))
		return DECERR_GENERIC;

	if (dec_fwrite(pData->pszFilename, 1, tm.nDataSize, m_pMarkerFile) != tm.nDataSize)
		return DECERR_GENERIC;

	// Set the modified flag to tell the Close routine that it needs to
	// re-construct the original file based on the text markers.
	SetModified(true);
	return (DEC_OK);
}


void TextSetMaxExtractSize(DWORD dwMaxSize)
{
	g_dwTextMaxExtractSize = dwMaxSize;
}

void TextAbortProcess(bool bAbort)
{
	g_bTextAbort = bAbort;
}

void TextSetLogWarnings(bool bLogWarnings)
{
	g_bLogWarnings = bLogWarnings;
}
