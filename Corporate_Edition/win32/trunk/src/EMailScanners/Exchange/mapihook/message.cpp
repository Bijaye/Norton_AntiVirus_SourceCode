// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "ldvpmec.h"
#include "mec.h"
#include "smartptr.h"
#include "SymSaferStrings.h"

void CMessage::GetMessageProps(void)
{
	enum { MessagePropCount = 14 };
	static SizedSPropTagArray (MessagePropCount, sptMessageProps) = { MessagePropCount,
		PR_ENTRYID, PR_NORMALIZED_SUBJECT, PR_HASATTACH, PR_SENDER_NAME, PR_SENDER_EMAIL_ADDRESS, PR_RECORD_KEY,
		PR_SENT_REPRESENTING_SEARCH_KEY, PR_SENT_REPRESENTING_ENTRYID, PR_SENT_REPRESENTING_EMAIL_ADDRESS, PR_SENT_REPRESENTING_NAME,
		PR_SENT_REPRESENTING_ADDRTYPE, PR_MESSAGE_FLAGS, PR_HASSIG, PR_RECEIVED_BY_NAME
	};	// Changes to sptMessageProps need to be reflected in the enum MessagePropIndexes.

	ULONG
		cValues;

	LPSPropValue
		paProps;

	HRESULT
		hr;

	if (m_paProps)			// Only do this once per instance
		return;

	hr = m_pIMessage->GetProps((LPSPropTagArray) &sptMessageProps, fMapiUnicode, &cValues, &paProps);
	if ((hr != S_OK && hr != MAPI_W_ERRORS_RETURNED) || cValues != MessagePropCount)
	{
		if (hr == MAPI_W_ERRORS_RETURNED || hr == S_OK)
			m_allocators.FreeBuffer(paProps);
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);
	}

	m_paProps = paProps;
}

LPCTSTR CMessage::SenderName(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_SENDER_NAME, PR_SENDER_NAME) ?
		m_paProps[INDEX_PR_SENDER_NAME].Value.LPSZ :
		_T("");
}

LPCTSTR CMessage::SenderEmail(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_SENDER_EMAIL_ADDRESS, PR_SENDER_EMAIL_ADDRESS) ?
		m_paProps[INDEX_PR_SENDER_EMAIL_ADDRESS].Value.LPSZ :
		_T("");
}

LPCTSTR CMessage::UserName(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_RECEIVED_BY_NAME, PR_RECEIVED_BY_NAME) ?
		m_paProps[INDEX_PR_RECEIVED_BY_NAME].Value.LPSZ :
		_T("");
}

LPCTSTR CMessage::NormalizedSubject(void)
{
	LPTSTR pSubject = _T("");
	
	GetMessageProps();

	if(IsValidProp(m_paProps, INDEX_PR_NORMALIZED_SUBJECT, PR_NORMALIZED_SUBJECT))
	{
		LPTSTR pTmp;

		pSubject = m_paProps[INDEX_PR_NORMALIZED_SUBJECT].Value.LPSZ;

		// Now strip all offending characters out 
		// to support the character mode protocol.
		pTmp = pSubject;		
		while(*pTmp)
		{
			if(_tcschr("/\\:,", (int)*pTmp))
				*pTmp = ' ';
			
			pTmp++;
		}
	}		
	
	return pSubject;
}

int CMessage::HasAttach(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_HASATTACH, PR_HASATTACH) ?
		m_paProps[INDEX_PR_HASATTACH].Value.b != 0 :
		0;
}

ULONG CMessage::MessageFlags(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_MESSAGE_FLAGS, PR_MESSAGE_FLAGS) ?
		m_paProps[INDEX_PR_MESSAGE_FLAGS].Value.ul :
		0;
}


const LPSBinary CMessage::EntryID(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_ENTRYID, PR_ENTRYID) ?
		&m_paProps[INDEX_PR_ENTRYID].Value.bin :
		&g_NullSBinary;
}

const LPSBinary CMessage::RecordKey(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_RECORD_KEY, PR_RECORD_KEY) ?
		&m_paProps[INDEX_PR_RECORD_KEY].Value.bin :
		&g_NullSBinary;
}


LPCTSTR CMessage::SentRepresentingAddrType(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_SENT_REPRESENTING_ADDRTYPE, PR_SENT_REPRESENTING_ADDRTYPE) ?
		m_paProps[INDEX_PR_SENT_REPRESENTING_ADDRTYPE].Value.LPSZ :
		_T("");
}

LPCTSTR CMessage::SentRepresentingName(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_SENT_REPRESENTING_NAME, PR_SENT_REPRESENTING_NAME) ?
		m_paProps[INDEX_PR_SENT_REPRESENTING_NAME].Value.LPSZ :
		_T("");
}

LPCTSTR CMessage::SentRepresentingEmailAddress(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_SENT_REPRESENTING_EMAIL_ADDRESS, PR_SENT_REPRESENTING_EMAIL_ADDRESS) ?
		m_paProps[INDEX_PR_SENT_REPRESENTING_EMAIL_ADDRESS].Value.LPSZ :
		_T("");
}

const LPSBinary CMessage::SentRepresentingEntryID(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_SENT_REPRESENTING_ENTRYID, PR_SENT_REPRESENTING_ENTRYID) ?
		&m_paProps[INDEX_PR_SENT_REPRESENTING_ENTRYID].Value.bin :
		&g_NullSBinary;
}

const LPSBinary CMessage::SentRepresentingSearchKey(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_SENT_REPRESENTING_SEARCH_KEY, PR_SENT_REPRESENTING_SEARCH_KEY) ?
		&m_paProps[INDEX_PR_SENT_REPRESENTING_SEARCH_KEY].Value.bin :
		&g_NullSBinary;
}

LPCTSTR CMessage::HasSig(void)
{
	GetMessageProps();

	return IsValidProp(m_paProps, INDEX_PR_HASSIG, PR_HASSIG) ?
		m_paProps[INDEX_PR_HASSIG].Value.LPSZ :
		_T("");
}

HRESULT CMessage::OpenAttach(CAttach **ppAttach, ULONG ulAttachNum, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMessage::OpenAttach");

	HRESULT
		hr;

	CComQIPtr<IAttach, &IID_IAttachment>
		pAttach;

	SAVASSERT(*ppAttach == 0);

	DebugOut(SM_GENERAL, "%s: [%u], sender: %s, subject: %s", s_verboseName, ulAttachNum, SenderName(), NormalizedSubject());

	hr = m_pIMessage->OpenAttach(ulAttachNum, 0, flags, &pAttach);
	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	*ppAttach = new CAttach(pAttach, m_allocators);

	return S_OK;
}


// GetAttacmentTable() obtains the messages attachment Table Interface.
//	Returns	S_OK if contents table available
//			MAPI_E_NO_SUPPORT if there is no contents table for the folder
//  Throws CMAPIException for any other error
//
HRESULT CMessage::GetAttachmentTable(LPMAPITABLE* ppAttachmentTable, ULONG flags)
{
	static TCHAR s_verboseName[] = _T("CMessage::GetAttachmentTable");

	SmartSRowSetPtr
		pAttachmentRows(m_allocators);

	HRESULT
		hr;

	DebugOut(SM_GENERAL, "%s: sender: %s, subject: %s", s_verboseName, SenderName(), NormalizedSubject());

	hr = m_pIMessage->GetAttachmentTable(flags, ppAttachmentTable);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	return hr;
}


// CMessage::EnumAttachments() walks the attachment table.
//
// Inputs:
//	pfnEnumAttach	Function to call for each attachment
//	pvData			Pointer to data to be passed on the pfnEnumAttach call.
//
// Outputs:
//	returns			S_OK	if all attachments enumerated.
//					S_FALSE	if enumeration ended early (by pfnEnumAttach), but still successful
//					error code

// Exceptions:
//	CMAPIException will be thrown if an unexpected MAPI error occurs.
//	CMyMemoryException is possible (from OpenAttach() or enum function)
//	Function pfnEnumAttach may throw an exception
//
// Function pfnEnumAttach(CAttach *, void *pvData)
//	Inputs:
//		pAttach		Pointer to the CAttach instance for the attachment
//		pvData		Context data passed into EnumAttachments()
//	Outputs:
//		Returns		S_OK if enumeration is to continue
//					S_FALSE if enumeration is to halt (but no error)
//					error code (enumeration halts)
//	
HRESULT CMessage::EnumAttachments(PFNEnumAttach pfnEnumAttach, void *pvData)
{
	static TCHAR s_verboseName[] = _T("CMessage::EnumAttachments");

	static SizedSPropTagArray(1, sptAttachCols) = { 1, PR_ATTACH_NUM };
	static SizedSSortOrderSet(1, ssoAttachCols) = { 0 };

	enum AttachIndexes { INDEX_PR_ATTACH_NUM = 0 };
	enum CMessageConstants { AttachMaxRows = 32 };

	CComQIPtr<IMAPITable, &IID_IMAPITable>
		pAttachmentTable;

	SmartPtr<CAttach>
		pAttach;

	SmartSRowSetPtr
		pAttachmentRows(m_allocators);

	HRESULT
		hr;

	ULONG
		cRows,
		i;

	SAVASSERT(pfnEnumAttach != 0);

	DebugOut(SM_GENERAL, "%s: sender: %s, subject: %s", s_verboseName, SenderName(), NormalizedSubject());

	if (!HasAttach())
		return S_OK;

	hr = GetAttachmentTable(&pAttachmentTable);
	if (hr != S_OK)
		return S_OK;

	hr = pAttachmentTable->SetColumns((LPSPropTagArray) &sptAttachCols, TBL_BATCH);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, pAttachmentTable);

	hr = pAttachmentTable->Restrict(0, TBL_BATCH);		// ABB: fails on mailbox stores
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, pAttachmentTable);

	hr = pAttachmentTable->SortTable((LPSSortOrderSet) &ssoAttachCols, TBL_BATCH);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, pAttachmentTable);

	hr = S_OK;
	while (hr == S_OK)
	{
		hr = pAttachmentTable->QueryRows(AttachMaxRows, 0, &pAttachmentRows);
		if (hr != S_OK)
			THROW_CMAPIException(m_allocators, hr, pAttachmentTable);

		cRows = pAttachmentRows->cRows;
		if (!cRows)
			break;

		for (i = 0; hr == S_OK && i < cRows; i++)
		{
			if (IsValidPropInRow(pAttachmentRows, i, INDEX_PR_ATTACH_NUM, PR_ATTACH_NUM))
			{ 
				hr = OpenAttach(&pAttach, pAttachmentRows->aRow[i].lpProps[INDEX_PR_ATTACH_NUM].Value.ul);
				// we need to reference the attachment via the number in the attachment table,
				// and not via the number in the attachment itself, so set the valid attachment number here
				pAttach->SetAttachNum(pAttachmentRows->aRow[i].lpProps[INDEX_PR_ATTACH_NUM].Value.ul);
				hr = pfnEnumAttach(pAttach, pvData);
				pAttach.Release();
			}
		}
	}

	return hr;
}

HRESULT CMessage::DeleteAttach(ULONG ulAttachmentNum, ULONG ulUIParam, LPMAPIPROGRESS lpProgress, ULONG ulFlags)
{
	static TCHAR s_verboseName[] = _T("CMessage::DeleteAttach");

	HRESULT
		hr;

	DebugOut(SM_GENERAL, "%s: sender: %s, subject: %s [%d]", s_verboseName, SenderName(), NormalizedSubject(), ulAttachmentNum);

	hr = m_pIMessage->DeleteAttach(ulAttachmentNum, ulUIParam, lpProgress, ulFlags);
	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	return S_OK;
}


HRESULT CMessage::OpenProperty(ULONG ulPropTag, LPCIID lpiid, ULONG ulInterfaceOptions, ULONG ulFlags, LPUNKNOWN *lppUnk, int bThrow)
{
	static TCHAR s_verboseName[] = _T("CMessage::OpenProperty");

	HRESULT
		hr;

	hr = m_pIMessage->OpenProperty(ulPropTag, lpiid, ulInterfaceOptions, ulFlags, lppUnk);
	if (hr != S_OK && bThrow)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	return hr;
}

HRESULT CMessage::SetProps(ULONG cValues, LPSPropValue lpPropArray, LPSPropProblemArray FAR * lppProblems, int bThrow)
{
	static TCHAR s_verboseName[] = _T("CMessage::SetProps");

	HRESULT
		hr;

	hr = m_pIMessage->SetProps(cValues, lpPropArray, lppProblems);
	if (hr != S_OK && bThrow)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	return hr;
}

HRESULT CMessage::ModifyRecipients(ULONG ulFlags, LPADRLIST lpMods)
{
	static TCHAR s_verboseName[] = _T("CMessage::ModifyRecipients");

	HRESULT
		hr;

	hr = m_pIMessage->ModifyRecipients(ulFlags, lpMods);
	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	return hr;
}

HRESULT CMessage::SubmitMessage(ULONG ulFlags)
{
	static TCHAR s_verboseName[] = _T("CMessage::SubmitMessage");

	HRESULT
		hr;

	hr = m_pIMessage->SubmitMessage(ulFlags);
	if (hr != S_OK)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	return hr;
}

// GetAttacmentTable() obtains the messages attachment Table Interface.
//	Returns	S_OK if contents table available
//			MAPI_E_NO_SUPPORT if there is no contents table for the folder
//  Throws CMAPIException for any other error
//
HRESULT CMessage::GetRecipientTable(LPMAPITABLE* ppRecipientTable, ULONG flags)
{
	static TCHAR s_verboseName[] = "CMessage::GetRecipientsTable";

	HRESULT
		hr;

	DebugOut(SM_GENERAL, "%s: sender: %s, subject: %s", s_verboseName, SenderName(), NormalizedSubject());

	hr = m_pIMessage->GetRecipientTable(flags, ppRecipientTable);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, m_pIMessage);

	return hr;
}


// CMessage::EnumRecipients() walks the recipients table.
//
//
// Outputs:
//	returns			S_OK	if all attachments enumerated.
//					S_FALSE	if enumeration ended early (by pfnEnumAttach), but still successful
//					error code

HRESULT CMessage::EnumRecipients()
{
	static TCHAR s_verboseName[] = _T("CMessage::EnumRecipients");

	static SizedSPropTagArray(1, sptRecipientCols) = { 1, PR_DISPLAY_NAME };
	static SizedSSortOrderSet(1, ssoRecipientCols) = { 0 };

	enum RecipientIndexes { INDEX_PR_DISPLAY_NAME = 0 };
	enum CMessageConstants { RecipientMaxRows = 32 };

	CComQIPtr<IMAPITable, &IID_IMAPITable>
		pRecipientTable;

	SmartSRowSetPtr
		pRecipientRows(m_allocators);

	HRESULT
		hr;

	ULONG
		cRows,
		i,
		count = 0;

	hr = GetRecipientTable(&pRecipientTable, 0);
	if (hr != S_OK)
		return S_OK;

	hr = pRecipientTable->SetColumns((LPSPropTagArray) &sptRecipientCols, TBL_BATCH);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, pRecipientTable);

	hr = pRecipientTable->Restrict(0, TBL_BATCH);		
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, pRecipientTable);

	hr = pRecipientTable->SortTable((LPSSortOrderSet) &ssoRecipientCols, TBL_BATCH);
	if (hr != S_OK && hr != MAPI_E_NO_SUPPORT)
		THROW_CMAPIException(m_allocators, hr, pRecipientTable);

	hr = S_OK;
	while (hr == S_OK)
	{
		hr = pRecipientTable->QueryRows(RecipientMaxRows, 0, &pRecipientRows);
		if (hr != S_OK)
			THROW_CMAPIException(m_allocators, hr, pRecipientTable);

		cRows = pRecipientRows->cRows;
		if (!cRows)
			break;

		for (i = 0; hr == S_OK && i < cRows; i++)
		{
			int nLen;
			if (IsValidPropInRow(pRecipientRows, i, INDEX_PR_DISPLAY_NAME, PR_DISPLAY_NAME))
			{
				nLen = _tcslen(pRecipientRows->aRow[i].lpProps[INDEX_PR_DISPLAY_NAME].Value.LPSZ);
				if(count + nLen	>= MAX_RECIPIENTS_LEN - 1)
					break;

				ssStrnCpy (m_szRecipients + count, 
                           pRecipientRows->aRow[i].lpProps[INDEX_PR_DISPLAY_NAME].Value.LPSZ,
                           (MAX_RECIPIENTS_LEN - count - 1) * sizeof (m_szRecipients[0]));
				count += nLen;
								
				if(i < cRows - 1)
				{
					ssStrnCpy (m_szRecipients + count, ";", (MAX_RECIPIENTS_LEN - count) * sizeof (m_szRecipients[0]));
					count++;
				}	
			}
		}
	}

	return hr;
}

/* end source file */