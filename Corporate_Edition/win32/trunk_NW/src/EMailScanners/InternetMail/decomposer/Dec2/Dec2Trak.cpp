// Dec2Trak.cpp : Decomposer 2 Result Tracker
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// Headers

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

// C Library headers
#include "dec_assert.h"

// Project headers
#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2Trak.h"
#include "Decstdio.h"
#include "dectmpf.h"


/////////////////////////////////////////////////////////////////////////////
// CDecResultsTracker::CDecResultsTracker()

CDecResultTracker::CDecResultTracker(
	IDecContainerObjectEx *pObject, 
	WORD *pwResult, 
	char *pszNewDataFile)
{
	// Save pointers to object and final results.
	m_pObject = pObject;
	m_pwResult = pwResult;
	m_pszNewDataFile = pszNewDataFile;

	// Initialize final results.
	*m_pwResult = DECRESULT_UNCHANGED;
	*m_pszNewDataFile = '\0';

	// Save original object properties.
	pObject->GetCanModify(&m_bOrigCanModify);
	pObject->GetDataFile(m_szOrigDataFile, NULL);

	// Have not replace the original data file yet.
	m_bReplacedDataFile = false;
}


/////////////////////////////////////////////////////////////////////////////
// CDecResultsTracker::Apply()

DECRESULT CDecResultTracker::Apply(
	WORD wResult, 
	const char *szNewDataFile)
{
	// Once something is deleted (use Deleted() to check),
	// there should be no futher processing.
	if (*m_pwResult == DECRESULT_TO_BE_DELETED)
		return DECERR_INVALID_DELETE;

	switch (wResult)
	{
	case DECRESULT_UNCHANGED:
		break;

	case DECRESULT_CHANGED:
		*m_pwResult = DECRESULT_CHANGED;
		break;

	case DECRESULT_TO_BE_REPLACED:
		// If we have done case this before...
		if (m_bReplacedDataFile)
		{
			// Delete the previous replacement file.
			char szDataFile[MAX_PATH];
			m_pObject->GetDataFile(szDataFile, NULL);
			dec_remove(szDataFile);
		}

		// Remember we did this case.
		m_bReplacedDataFile = true;

		// The object now represents the replacement file
		// (until final cleanup in the destructor).
		m_pObject->SetDataFile(szNewDataFile);

		// Permission to replace the original data file 
		// now extends to modification of the new data file.
		m_pObject->SetCanModify(true);

		// We now have a changed data file, not a replacement.
		*m_pwResult = DECRESULT_CHANGED;
		break;

	case DECRESULT_TO_BE_DELETED:
		*m_pwResult = DECRESULT_TO_BE_DELETED;
		break;

	default:
		return DECERR_GENERIC;
	}

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDecResultsTracker::Deleted()

bool CDecResultTracker::Deleted()
{
	return (*m_pwResult == DECRESULT_TO_BE_DELETED);
}


/////////////////////////////////////////////////////////////////////////////
// CDecResultsTracker::~CDecResultsTracker()

CDecResultTracker::~CDecResultTracker()
{
	// Assume there will be no final replacement file.
	*m_pszNewDataFile = '\0';

	switch (*m_pwResult)
	{
	case DECRESULT_UNCHANGED:
		break;

	case DECRESULT_CHANGED:
		// If the change was to an applied replacement file...
		if (m_bReplacedDataFile)
		{
			// A changed replacement file is still a replacement file.
			*m_pwResult = DECRESULT_TO_BE_REPLACED;

			// Transfer the applied replacement file name to the final output.
			char szDataFile[MAX_PATH];
			m_pObject->GetDataFile(szDataFile, NULL);
			strcpy(m_pszNewDataFile, szDataFile);
		}
		break;

	case DECRESULT_TO_BE_REPLACED:
		// Transfer the applied replacement file name to the final output.
		char szDataFile[MAX_PATH];
		m_pObject->GetDataFile(szDataFile, NULL);
		strcpy(m_pszNewDataFile, szDataFile);
		break;

	case DECRESULT_TO_BE_DELETED:
		// If a replacement file was applied...
		if (m_bReplacedDataFile)
		{
			// Delete the applied replacement file.
			char szDataFile[MAX_PATH];
			m_pObject->GetDataFile(szDataFile, NULL);
			dec_remove(szDataFile);
		}
		break;
	}

	// Restore the object state.
	m_pObject->SetCanModify(m_bOrigCanModify);
	m_pObject->SetDataFile(m_szOrigDataFile);
}
