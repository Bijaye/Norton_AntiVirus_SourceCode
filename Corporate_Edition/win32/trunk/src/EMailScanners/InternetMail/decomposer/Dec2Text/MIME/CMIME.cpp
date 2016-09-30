// CMIME.cpp : Implementation of IMIME interface.
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

#define MY_BUFFER_SIZE		(8 * 1024)


CMIME::CMIME()
{
	m_pParser = NULL;
	m_wResult = DECRESULT_UNCHANGED;
}


CMIME::~CMIME()
{
}


void CMIME::Init(CMIMEParser *pParser)
{
	m_pParser = pParser;
}


int CMIME::GetTokenFilename(char *pszFilename, size_t nBufferSize)
{
	dec_assert(m_pParser);
	if (m_pParser->m_pszTokenFile == NULL)
		return MIME_ERROR_NO_FILENAME;

	if (!nBufferSize)
		return MIME_ERROR_INVALID_PARAMS;

	strncpy(pszFilename, m_pParser->m_pszTokenFile, nBufferSize);
	pszFilename[nBufferSize - 1] = 0;
	return MIME_ERROR_NONE;
}


int CMIME::GetWarningFilename(char *pszFilename, size_t nBufferSize)
{
	dec_assert(m_pParser);
	if (m_pParser->m_pszWarningFile == NULL)
		return MIME_ERROR_NO_FILENAME;

	if (!nBufferSize)
		return MIME_ERROR_INVALID_PARAMS;

	strncpy(pszFilename, m_pParser->m_pszWarningFile, nBufferSize);
	pszFilename[nBufferSize - 1] = 0;
	return MIME_ERROR_NONE;
}


int CMIME::GetSectionInfo(MIMESECTIONINFO *pInfo, size_t nSectionID, size_t nSectionNumber)
{
	int				rc;
	size_t			nSectionCount;

	if (nSectionID == MIME_SECTION_MULTIPART_HEADER)
	{
		// Since the main header is also ID'd as a multipart header,
		// increment the section number by one to skip over it.
		nSectionNumber++;
	}

	dec_assert(m_pParser);
	rc = m_pParser->FindFirstSection(pInfo);
	if (rc != MIME_ERROR_NONE)
		return rc;

	nSectionCount = 0;
	while (rc == MIME_ERROR_NONE)
	{
		// There is always only one main header.
		if (nSectionID == MIME_SECTION_MAIN_HEADER)
		{
			if (nSectionCount != 0)
				break;

			if (pInfo->nSectionID == MIME_SECTION_MULTIPART_HEADER)
				pInfo->nSectionID = MIME_SECTION_MAIN_HEADER;
		}

		if (pInfo->nSectionID == nSectionID)
		{
			if (nSectionCount == nSectionNumber)
			{
				pInfo->nSectionID = nSectionID;
				return MIME_ERROR_NONE;
			}

			nSectionCount++;
		}

		if (pInfo->pszName)
		{
			free(pInfo->pszName);
			pInfo->pszName = NULL;
		}

		if (pInfo->pszSecondaryName)
		{
			free(pInfo->pszSecondaryName);
			pInfo->pszSecondaryName = NULL;
		}

		rc = m_pParser->FindNextSection(pInfo);
	}

	if (pInfo->pszName)
	{
		free(pInfo->pszName);
		pInfo->pszName = NULL;
	}

	if (pInfo->pszSecondaryName)
	{
		free(pInfo->pszSecondaryName);
		pInfo->pszSecondaryName = NULL;
	}

	return MIME_ERROR_NO_MORE_SECTIONS;
}


int CMIME::ExtractRawSection(char *pszFilename, size_t nSectionID, size_t nSectionNumber)
{
	int		rc = MIME_ERROR_NONE;  // Assume success.
	FILE	*fpin = NULL;
	FILE	*fpout = NULL;
	MIMESECTIONINFO msi;

	dec_assert(m_pParser);
	dec_assert(m_pParser->m_pArchive);

	// First, get some information about the section to be extracted.
	rc = GetSectionInfo(&msi, nSectionID, nSectionNumber);
	if (rc != MIME_ERROR_NONE)
		return (rc);

	// Make sure the start offset is less than or equal to the end offset.
	if (msi.nStartOffset > msi.nEndOffset)
	{
		rc = MIME_ERROR_GENERIC;
		goto done_extractraw;
	}

	// Note that the text engine already has the input file open
	// in m_pArchive->m_pFile.
	fpin = m_pParser->m_pArchive->m_pFile;
	if (!fpin)
	{
		rc = MIME_ERROR_GENERIC;
		goto done_extractraw;
	}

	// Open the file the caller is asking us to extract the raw data into.
	fpout = dec_fopen(pszFilename, "w+b");
	if (!fpout)
	{
		rc = MIME_ERROR_GENERIC;
		goto done_extractraw;
	}

	if (!m_pParser->m_pArchive->CopyBlock(fpin, fpout, msi.nStartOffset, msi.nEndOffset))
		rc = MIME_ERROR_GENERIC;

done_extractraw:
	// Free the names allocated by the GetSectionInfo call.
	if (msi.pszName)
		free(msi.pszName);
	if (msi.pszSecondaryName)
		free(msi.pszSecondaryName);

	if (fpout)
	{
		// Close the output file.
		if (EOF == dec_fclose(fpout) ||
			rc != MIME_ERROR_NONE)
		{
			dec_remove(pszFilename);
			rc = MIME_ERROR_GENERIC;
		}
	}

	fpout = NULL;
	return (rc);
}


int CMIME::ReplaceRawSection(char *pszFilename, size_t nSectionID, size_t nSectionNumber)
{
	int					rc = MIME_ERROR_NONE;  // Assume success.
	MIMESECTIONINFO		msi;
	TEXT_CHANGE_DATA	tcd;
	DECRESULT			hr;
	char				szNewDataFile[MAX_PATH];

	dec_assert(m_pParser);
	dec_assert(m_pParser->m_pArchive);

	// First, get some information about the section to be replaced.
	rc = GetSectionInfo(&msi, nSectionID, nSectionNumber);
	if (rc != MIME_ERROR_NONE)
		return (rc);

	// Make sure the start offset is less than or equal to the end offset.
	if (msi.nStartOffset > msi.nEndOffset)
	{
		rc = MIME_ERROR_GENERIC;
		goto done_replaceraw;
	}

	hr = m_pParser->m_pEngine->PrepareNewDataFile(
			m_pParser->m_pDecomposer,
			m_pParser->m_pObject,
			m_pParser->m_pArchive,
			&m_wResult, &szNewDataFile[0]);
	if (FAILED(hr))
	{
		rc = MIME_ERROR_GENERIC;
		goto done_replaceraw;
	}

	memset(&tcd, 0, sizeof(tcd));
	tcd.pszFilename = pszFilename;
	tcd.nStartOffset = msi.nStartOffset;
	tcd.nEndOffset = msi.nEndOffset;
	tcd.bRawChange = true;
	hr = m_pParser->m_pArchive->MarkForReplace(&tcd);
	if (FAILED(hr))
		rc = MIME_ERROR_GENERIC;

	m_wResult = DECRESULT_TO_BE_REPLACED;

done_replaceraw:
	// Free the names allocated by the GetSectionInfo call.
	if (msi.pszName)
		free(msi.pszName);
	if (msi.pszSecondaryName)
		free(msi.pszSecondaryName);

	return (rc);
}


int CMIME::ExtractBody(char *pszFilename, size_t nSectionNumber)
{
	int		rc = MIME_ERROR_NONE;  // Assume success.
	MIMESECTIONINFO msi;

	dec_assert(m_pParser);
	dec_assert(m_pParser->m_pArchive);

	// First, get some information about the section to be extracted.
	rc = GetSectionInfo(&msi, MIME_SECTION_BODY, nSectionNumber);
	if (rc != MIME_ERROR_NONE)
		return (rc);

	// Make sure the start offset is less than or equal to the end offset.
	if (msi.nStartOffset > msi.nEndOffset)
	{
		rc = MIME_ERROR_GENERIC;
		goto done_extractbody;
	}

	// Set the start and end offsets of the body.
	m_pParser->m_nStartOffset = msi.nStartOffset;
	m_pParser->m_nEndOffset = msi.nEndOffset;
	if (m_pParser->ExtractEntry(pszFilename, m_pParser->m_pArchive->m_pFile) != LEXER_OK)
		rc = MIME_ERROR_GENERIC;

done_extractbody:
	// Free the names allocated by the GetSectionInfo call.
	if (msi.pszName)
		free(msi.pszName);
	if (msi.pszSecondaryName)
		free(msi.pszSecondaryName);

	return (rc);
}

