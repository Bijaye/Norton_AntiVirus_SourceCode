// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "ldvpmec.h"
#include "mec.h"

void CMAPIFolder::GetMAPIFolderProps(void)
{
	enum { MAPIFolderPropCount = 3 };
	static SizedSPropTagArray (MAPIFolderPropCount, sptMAPIFolderProps) = { MAPIFolderPropCount, PR_ENTRYID, PR_DISPLAY_NAME, PR_RECORD_KEY };
		// Changes to sptMAPIFolderProps need to be reflected in the enum MAPIFolderPropIndexes.

	ULONG
		cValues;

	LPSPropValue
		paProps;

	HRESULT
		hr;

	if (m_paProps)			// Only do this once per instance
		return;

	hr = m_pIMAPIFolder->GetProps((LPSPropTagArray) &sptMAPIFolderProps, fMapiUnicode, &cValues, &paProps);
	if ((hr != S_OK && hr != MAPI_W_ERRORS_RETURNED) || cValues != MAPIFolderPropCount)
	{
		if (hr == MAPI_W_ERRORS_RETURNED || hr == S_OK)
			m_allocators.FreeBuffer(paProps);
		THROW_CMAPIException(m_allocators, hr, m_pIMAPIFolder);
	}

	m_paProps = paProps;
}

HRESULT CMAPIFolder::GetProps(LPSPropTagArray pPropTagArray, ULONG *pcValues, LPSPropValue *ppPropArray)
{
	HRESULT
		hr;

	ULONG
		expected = *pcValues;

	hr = m_pIMAPIFolder->GetProps(pPropTagArray, fMapiUnicode, pcValues, ppPropArray);
	if ((hr != S_OK && hr != MAPI_W_ERRORS_RETURNED) || *pcValues != expected)
	{
		if (hr == MAPI_W_ERRORS_RETURNED || hr == S_OK)
			m_allocators.FreeBuffer(*ppPropArray);
		THROW_CMAPIException(m_allocators, hr, m_pIMAPIFolder);
	}

	return S_OK;
}


LPCTSTR CMAPIFolder::DisplayName(void)
{
	GetMAPIFolderProps();

	return IsValidProp(m_paProps, INDEX_PR_DISPLAY_NAME, PR_DISPLAY_NAME) ?
		m_paProps[INDEX_PR_DISPLAY_NAME].Value.LPSZ :
		_T("");
}

const LPSBinary CMAPIFolder::EntryID(void)
{
	GetMAPIFolderProps();

	return IsValidProp(m_paProps, INDEX_PR_ENTRYID, PR_ENTRYID) ?
		&m_paProps[INDEX_PR_ENTRYID].Value.bin :
		&g_NullSBinary;
}

const LPSBinary CMAPIFolder::RecordKey(void)
{
	GetMAPIFolderProps();

	return IsValidProp(m_paProps, INDEX_PR_RECORD_KEY, PR_RECORD_KEY) ?
		&m_paProps[INDEX_PR_RECORD_KEY].Value.bin :
		&g_NullSBinary;
}

HRESULT CMAPIFolder::OpenFolder(CMAPIFolder **ppFolder, LPSBinary psbEntryID, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMAPIFolder::OpenFolder");

	HRESULT
		hr;

	ULONG
		ulObjType;

	CComQIPtr<IMAPIFolder, &IID_IMAPIFolder>
		pMAPIFolder;

	SAVASSERT(*ppFolder == 0);
	SAVASSERT(psbEntryID != 0);

	DebugOut(SM_GENERAL, "%s: folder: %s, new folder: %s", s_verboseName, DisplayName(), psbEntryID ? _T("") : _T("Root"));
	
	hr = m_pIMAPIFolder->OpenEntry(psbEntryID ? psbEntryID->cb : 0,	// ULONG cbEntryID
		(LPENTRYID) (psbEntryID ? psbEntryID->lpb : 0),				// LPENTRYID lpEntryID
		0,															// LPCIID lpInterface
		flags,														// ULONG ulFlags
		&ulObjType,													// ULONG FAR * lpulObjType
		(LPUNKNOWN *) &pMAPIFolder);								// LPUNKNOWN FAR * lppUnk

	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMAPIFolder);

	if (ulObjType != MAPI_FOLDER)
		THROW_CMAPIExceptionHR(MAPI_E_INVALID_OBJECT);


	*ppFolder = new CMAPIFolder(pMAPIFolder, m_allocators);

	return S_OK;
}


HRESULT CMAPIFolder::OpenMessage(CMessage **ppMessage, LPSBinary psbEntryID, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMAPIFolder::OpenMessage");

	HRESULT
		hr;

	ULONG
		ulObjType;

	CComQIPtr<IMessage, &IID_IMessage>
		pMessage;

	SAVASSERT(psbEntryID != 0);
	SAVASSERT(*ppMessage == 0);

	DebugOut(SM_GENERAL, "%s: folder: %s", s_verboseName, DisplayName());
	
	hr = m_pIMAPIFolder->OpenEntry(psbEntryID->cb,	// ULONG cbEntryID
		(LPENTRYID) psbEntryID->lpb,				// LPENTRYID lpEntryID
		0,											// LPCIID lpInterface
		flags,										// ULONG ulFlags
		&ulObjType,									// ULONG FAR * lpulObjType
		(LPUNKNOWN *) &pMessage);					// LPUNKNOWN FAR * lppUnk

	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMAPIFolder);

	if (ulObjType != MAPI_MESSAGE)
		THROW_CMAPIExceptionHR(MAPI_E_INVALID_OBJECT);

	*ppMessage = new CMessage(pMessage, m_allocators);

	return S_OK;
}


// GetHierarchyTable() obtains the folders Hierarchy Table Interface.
//	Returns S_OK if hierarchy table available
//			MAPI_E_NO_SUPPORT if there is no hierarchy table for the folder
//  Throws CMAPIException for any other error
//
HRESULT CMAPIFolder::GetHierarchyTable(LPMAPITABLE* ppHierarchyTable, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMAPIFolder::GetHierarchyTable");

	HRESULT
		hr;

	DebugOut(SM_GENERAL, "%s: %s", s_verboseName, DisplayName());

	hr = m_pIMAPIFolder->GetHierarchyTable(flags, ppHierarchyTable);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, m_pIMAPIFolder);

	return hr;
}

// GetContentsTable() obtains the folders Contents Table Interface.
//	Returns S_OK if contents table available
//			MAPI_E_NO_SUPPORT if there is no contents table for the folder
//  Throws CMAPIException for any other error
//
HRESULT CMAPIFolder::GetContentsTable(LPMAPITABLE* ppContentsTable, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMAPIFolder::GetContentsTable");

	HRESULT
		hr;

	DebugOut(SM_GENERAL, "%s: %s", s_verboseName, DisplayName());

	hr = m_pIMAPIFolder->GetContentsTable(flags, ppContentsTable);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, m_pIMAPIFolder);

	return hr;
}


HRESULT CMAPIFolder::CreateMessage(CMessage **ppMessage, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMAPIFolder::CreateMessage");

	HRESULT
		hr;

	CComQIPtr<IMessage, &IID_IMessage>
		pMessage;

	DebugOut(SM_GENERAL, "%s: folder: %s", s_verboseName, DisplayName());
	
	hr = m_pIMAPIFolder->CreateMessage(0, flags, &pMessage);
	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMAPIFolder);

	*ppMessage = new CMessage(pMessage, m_allocators);

	return S_OK;
}

/* end source file */