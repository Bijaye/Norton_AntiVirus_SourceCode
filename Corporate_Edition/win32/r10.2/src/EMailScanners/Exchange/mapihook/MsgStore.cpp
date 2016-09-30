// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "ldvpmec.h"
#include "mec.h"
#include "smartptr.h"

HRESULT CMsgStore::StoreLogoff(ULONG *lpulFlags)
{
	static TCHAR s_verboseName[] = _T("CMsgStore::StoreLogoff");

	HRESULT
		hr = S_OK;

	ULONG
		flags;

	if (m_pIMsgStore)
	{
		if (!lpulFlags)
		{
			flags = LOGOFF_ORDERLY;
			lpulFlags = &flags;
		}

		DebugOut(SM_GENERAL, "%s: MsgStore: %s", s_verboseName, DisplayName());
		hr = m_pIMsgStore->StoreLogoff(lpulFlags);
		m_pIMsgStore.Release();
	}

	return hr;
}

void CMsgStore::GetMsgStoreProps(void)
{
	enum { MsgStorePropCount = 6 };
	static SizedSPropTagArray (MsgStorePropCount, sptMsgStoreProps) = { MsgStorePropCount, PR_ENTRYID, PR_DISPLAY_NAME, PR_STORE_SUPPORT_MASK, PR_IPM_SUBTREE_ENTRYID, PR_RECORD_KEY, PR_VALID_FOLDER_MASK };
		// Changes to sptMsgStoreProps need to be reflected in the enum MsgStorePropIndexes.

	ULONG
		cValues;

	LPSPropValue
		paProps;

	HRESULT
		hr;

	if (m_paProps)			// Only do this once per instance
		return;

	hr = m_pIMsgStore->GetProps((LPSPropTagArray) &sptMsgStoreProps, fMapiUnicode, &cValues, &paProps);
	if ((hr != S_OK && hr != MAPI_W_ERRORS_RETURNED) || cValues != MsgStorePropCount)
	{
		if (hr == MAPI_W_ERRORS_RETURNED || hr == S_OK)
			m_allocators.FreeBuffer(paProps);
		THROW_CMAPIException(m_allocators, hr, m_pIMsgStore);
	}

	m_paProps = paProps;
}

LPCTSTR CMsgStore::DisplayName(void)
{
	GetMsgStoreProps();

	return IsValidProp(m_paProps, INDEX_PR_DISPLAY_NAME, PR_DISPLAY_NAME) ?
		m_paProps[INDEX_PR_DISPLAY_NAME].Value.LPSZ :
		_T("");
}

ULONG CMsgStore::StoreSupportMask(void)
{
	GetMsgStoreProps();

	return IsValidProp(m_paProps, INDEX_PR_STORE_SUPPORT_MASK, PR_STORE_SUPPORT_MASK) ?
		m_paProps[INDEX_PR_STORE_SUPPORT_MASK].Value.ul :
		0;
}

ULONG CMsgStore::ValidFolderMask(void)
{
	GetMsgStoreProps();

	return IsValidProp(m_paProps, INDEX_PR_VALID_FOLDER_MASK, PR_VALID_FOLDER_MASK) ?
		m_paProps[INDEX_PR_VALID_FOLDER_MASK].Value.ul :
		0;
}


const LPSBinary CMsgStore::EntryID(void)
{
	GetMsgStoreProps();

	return IsValidProp(m_paProps, INDEX_PR_ENTRYID, PR_ENTRYID) ?
		&m_paProps[INDEX_PR_ENTRYID].Value.bin :
		&g_NullSBinary;
}

const LPSBinary CMsgStore::IPMSubtreeEntryID(void)
{
	ULONG
		vfMask = ValidFolderMask();

	if ((vfMask & FOLDER_IPM_SUBTREE_VALID) == 0)
		return &g_NullSBinary;

	return IsValidProp(m_paProps, INDEX_PR_IPM_SUBTREE_ENTRYID, PR_IPM_SUBTREE_ENTRYID) ?
		&m_paProps[INDEX_PR_IPM_SUBTREE_ENTRYID].Value.bin :
		&g_NullSBinary;
}

const LPSBinary CMsgStore::RecordKey(void)
{
	GetMsgStoreProps();

	return IsValidProp(m_paProps, INDEX_PR_RECORD_KEY, PR_RECORD_KEY) ?
		&m_paProps[INDEX_PR_RECORD_KEY].Value.bin :
		&g_NullSBinary;
}


HRESULT CMsgStore::OpenFolder(CMAPIFolder **ppFolder, LPSBinary psbEntryID, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMsgStore::OpenFolder");

	HRESULT
		hr;

	ULONG
		ulObjType;

	CComQIPtr<IMAPIFolder, &IID_IMAPIFolder>
		pMAPIFolder;

	SAVASSERT(*ppFolder == 0);

	DebugOut(SM_GENERAL, "%s: MsgStore: %s %s", s_verboseName, DisplayName(), psbEntryID ? _T("") : _T("(Root)"));

	hr = m_pIMsgStore->OpenEntry(psbEntryID ? psbEntryID->cb : 0,	// ULONG cbEntryID
		(LPENTRYID) (psbEntryID ? psbEntryID->lpb : 0),				// LPENTRYID lpEntryID
 		0,															// LPCIID lpInterface
		flags,														// ULONG ulFlags
		&ulObjType,													// ULONG FAR * lpulObjType
		(LPUNKNOWN *) &pMAPIFolder);								// LPUNKNOWN FAR * lppUnk

	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMsgStore);

	if (ulObjType != MAPI_FOLDER)
		THROW_CMAPIExceptionHR(MAPI_E_INVALID_OBJECT);

	*ppFolder = new CMAPIFolder(pMAPIFolder, m_allocators);

	return S_OK;
}


HRESULT CMsgStore::OpenOutboxFolder(CMAPIFolder **ppFolder, ULONG flags)
{
	static SizedSPropTagArray (1, sptOutboxProps) = { 1, PR_IPM_OUTBOX_ENTRYID };
		// Changes to sptMsgStoreProps need to be reflected in the enum MsgStorePropIndexes.

	ULONG
		cValues;

	SmartMAPIBufPtr<SPropValue>
		paProps(m_allocators);

	HRESULT
		hr;

	ULONG
		vfMask = ValidFolderMask();

	if ((vfMask & FOLDER_IPM_OUTBOX_VALID) == 0)
		return MAPI_E_NO_ACCESS;

	hr = m_pIMsgStore->GetProps((LPSPropTagArray) &sptOutboxProps, fMapiUnicode, &cValues, &paProps);
	if ((hr != S_OK && hr != MAPI_W_ERRORS_RETURNED) || cValues != 1)
	{
		if (hr != MAPI_W_ERRORS_RETURNED && hr != S_OK)
			paProps.Reset();
		THROW_CMAPIException(m_allocators, hr, m_pIMsgStore);
	}

	if (!IsValidProp(paProps, 0, PR_IPM_OUTBOX_ENTRYID))
		return MAPI_E_NO_ACCESS;

	return OpenFolder(ppFolder, &paProps[0].Value.bin, flags);
}


HRESULT CMsgStore::OpenInboxFolder(CMAPIFolder **ppFolder, ULONG ulFlags)
{
	HRESULT
		hr;

	SBinary
		sb;

	SmartMAPIBufPtr<ENTRYID>
		pEntryID(m_allocators);

	LPTSTR
		pszExplicitClass;

	hr = m_pIMsgStore->GetReceiveFolder(0, fMapiUnicode, &sb.cb, &pEntryID, &pszExplicitClass);
	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMsgStore);

	m_allocators.FreeBuffer(pszExplicitClass);

	sb.lpb = (LPBYTE) (LPENTRYID) pEntryID;
	return OpenFolder(ppFolder, &sb, ulFlags);
}

HRESULT CMsgStore::GetOutlookFolderEntryIDs(ULONG *pcValues, LPSPropValue *ppaProps)
{
	enum { OutlookFolderCount = 5 };
	static SizedSPropTagArray (OutlookFolderCount, sptOutlookFolderProps) = { OutlookFolderCount,
		PR_OUTLOOK_CALENDAR_ENTRYID, PR_OUTLOOK_CONTACTS_ENTRYID, PR_OUTLOOK_JOURNAL_ENTRYID, PR_OUTLOOK_NOTES_ENTRYID,
		PR_OUTLOOK_TASKS_ENTRYID };

	HRESULT
		hr;

	SmartPtr<CMAPIFolder>
		pCInboxFolder;

	hr = OpenInboxFolder(&pCInboxFolder);
	if (hr != S_OK)
		return hr;

	*pcValues = OutlookFolderCount;
	return pCInboxFolder->GetProps((LPSPropTagArray) &sptOutlookFolderProps, pcValues, ppaProps);
}

ULONG CMsgStore::CompareEntryIDs(ULONG cb1, LPENTRYID pEntryID1, ULONG cb2, LPENTRYID pEntryID2)
{
	HRESULT
		hr;

	ULONG
		rv;

	hr = m_pIMsgStore->CompareEntryIDs(cb1, pEntryID1, cb2, pEntryID2, 0, &rv);
	if (hr != S_OK)
		return false;

	return !!rv;
}

/* end source file */