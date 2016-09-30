// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "ldvpmec.h"
#include "mec.h"
#include "hook.h"
#include "comm.h"
#include "smartptr.h"
#include "tempfile.h"
#include "SavrtModuleInterface.h"
#include "SymSaferStrings.h"

static TCHAR s_crlf[] = _T("\r\n");

static TCHAR VERBOSE_MemError[]	= _T("ERROR: %s: out of memory condition (%d)");
static TCHAR VERBOSE_UnkError[]	= _T("ERROR: %s: unknown exception");

/*===========================================================================*/
// Util functions
/*===========================================================================*/



static BOOL _isExtWanted(LPCTSTR list, LPCTSTR ext)
{
	int pLen = 0, rLen = 0;
	LPCTSTR p1, p2, q, r1, r2;
	TCHAR Ext[_MAX_EXT];

	if (ext[0] == _T('.'))
		ext = _tcsinc(ext);	// skip leading dot

    ssStrnCpy(Ext, ext, sizeof(Ext));
	_tcsupr(Ext);

#ifndef LINUX
    // For non-Linux platforms, making assumption that filesystem we're dealing
    // with is case-insensitive. Not quite true, but this is the historical
    // behavior for NLM and Windows flavors of PSCAN.
    char szExtList[EXT_LIST_SIZE];

    ssStrnCpy( szExtList, list, sizeof(szExtList) );
    _tcsupr( szExtList );
    list = szExtList;
#endif

	for (p1 = p2 = list; p1; p1 = (*q ? _tcsinc(q) : NULL))
	{
		q = _tcschr(p1, ',');
		if (!q)
			q = p1 + _tcslen(p1);

		for (r1 = r2 = Ext; ;
			p2 = _tcsinc(p1), pLen = p2 - p1, p1 = p2,
			r2 = _tcsinc(r1), rLen = r2 - r1, r1 = r2)
        {
			if (p1 >= q)
            {
				if (!(*r1))
					return TRUE;
				break;
            }

			if (!(*r1))
				break;		// End if Ext - no match here

			if (*p1 == '?')
				continue;	// i.e., '?' matches ANY char, even wchars

			if (pLen != rLen || *p1 != *r1)
				break;		// Dosen't match

			//	This assumes ONLY 1 or 2 byte characters!
			if (pLen > 1 && *(p1 - 1) != *(r1 - 1))
				break;		// PrevChar doesn't match
        }
    }

	return FALSE;
}


// UpdateDebugFlag()
//
// Read debug value from options in memory mapped file and update global debug flag
// for exchange hook.
//
// Will default to false if communications objects haven't been created yet.
//
// Sets global g_debug and returns new value.


static BOOL UpdateDebugFlag()
{
	// get the debug property that the storage ext set...

    if ( COMM_INITIALIZED == GetCommunicationState() )
    {
	    g_debug = g_pMemFile->bDebug != 0;
    }
    else
    {
        g_debug = FALSE;
    }

    return ( g_debug );
}

/*===========================================================================*/
// CHook class implementation follows:
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

DWORD CHook::m_cInstances = 0;

/*---------------------------------------------------------------------------*/

CHook::~CHook()
{
	m_cInstances--;
	if (!m_cInstances)
		CloseCommunicationObjects();
}

/*---------------------------------------------------------------------------*/

HRESULT CHook::Initialize(void)
{
	if (!m_cInstances)
		{
		if (CreateCommunicationObjects() != ERROR_SUCCESS)
			return E_FAIL;
		}

	m_cInstances++;

	// get the debug property that the storage ext set...
    UpdateDebugFlag();

	DebugOut(SM_GENERAL, "CHook initialization complete");

	return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::QueryInterface(REFIID pIID, void **ppvObject)
{
	return m_pParent->QueryInterface(pIID, ppvObject);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(ULONG) CHook::AddRef(void)
{
	return m_pParent->AddRef();
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(ULONG) CHook::Release(void)
{
	return m_pParent->Release();
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(ULONG) CHook::NonDelegatingRelease(void)
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::OnReadComplete(LPEXCHEXTCALLBACK /* lpeecb */, ULONG /* ulFlags */)
{
	// get the debug property that the storage ext set...
    UpdateDebugFlag();
	DebugOut(SM_GENERAL, "CHook:OnReadComplete");
	return m_hrOnReadComplete;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::OnWrite(LPEXCHEXTCALLBACK /* lpeecb */)
{
	// get the debug property that the storage ext set...
    UpdateDebugFlag();
	DebugOut(SM_GENERAL, "CHook:OnWrite");
	return S_FALSE;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::OnWriteComplete(LPEXCHEXTCALLBACK /* lpeecb */, ULONG /* ulFlags */)
{
	// get the debug property that the storage ext set...
    UpdateDebugFlag();
	DebugOut(SM_GENERAL, "CHook:OnWriteComplete");
	return S_FALSE;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::OnCheckNames(LPEXCHEXTCALLBACK /* lpeecb */)
{
	// get the debug property that the storage ext set...
    UpdateDebugFlag();
	DebugOut(SM_GENERAL, "CHook:OnCheckNames");
	return S_FALSE;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::OnCheckNamesComplete(LPEXCHEXTCALLBACK /* lpeecb */, ULONG /* ulFlags */)
{
	// get the debug property that the storage ext set...
    UpdateDebugFlag();
	DebugOut(SM_GENERAL, "CHook:OnCheckNamesComplete");
	return S_FALSE;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::OnSubmit(LPEXCHEXTCALLBACK /* lpeecb */)
{
	// get the debug property that the storage ext set...
    UpdateDebugFlag();
	DebugOut(SM_GENERAL, "CHook:OnSubmit");
	return S_FALSE;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(void) CHook::OnSubmitComplete(LPEXCHEXTCALLBACK /* lpeecb */, ULONG /* ulFlags */)
{
	// get the debug property that the storage ext set...
    UpdateDebugFlag();
	DebugOut(SM_GENERAL, "CHook:OnSubmitComplete");
	return;
}

/*---------------------------------------------------------------------------*/

// CHook::ScanThisAttachment() qualifies the attachment for scanning.
//
// Inputs
//	pAttach	Attachment to be examined
//	pData	Scan data block
//
// Outputs
//	returns	true if attachment should be scanned
//			false if attachment should be skipped

int CHook::ScanThisAttachment(CAttach *pAttach, RTScanData *pData)
{
	LPCTSTR
		pExt;

	ULONG
		method = pAttach->AttachMethod();

	if (method != ATTACH_BY_VALUE)
		return false;

	pExt = pAttach->AttachExtension();

	if (_isExtWanted(pData->extInfo.szSkipExts, pExt))
		return false;

	if (pData->extInfo.fileType != 1)
		return true;

	if (pData->extInfo.bScanZipFiles)
		if (_isExtWanted(pData->extInfo.szZIPExts, pExt))
			return true;

	return _isExtWanted(pData->extInfo.szExts, pExt);
}

/*---------------------------------------------------------------------------*/

// CHook::SendNotifications() sends notifications to both the sender and any administrators

HRESULT CHook::SendNotificationMessages(RTScanData *pData)
{
	static TCHAR s_verboseName[] = _T("CHook::SendNotificationMessages");

	HRESULT
		hr;

	SmartPtr<CMAPIFolder>
		pCMAPIFolder;

	SmartPtr<CMessage>
		pCMessage;

	SPropValue
		v[3];

	SmartADRLISTPtr
		pal(m_allocators);

	LPSPropValue
		paProps;

	LPCTSTR
		psz;

	LPTSTR
		pszAdmin,
		pszNext;

	LPSBinary
		psb;

	CComPtr<IStream>
		pBody;

	RTActionData
		*pNext;

	int
		len,
		crlfLen,
		i,
		recipients,
		cb;

	CComPtr<IAddrBook>
		pIAddrBook;

//	DebugOut(SM_GENERAL, "%s", s_verboseName);

	try
	{
		hr = pData->pCMsgStore->OpenOutboxFolder(&pCMAPIFolder, MAPI_MODIFY);
		if (hr != S_OK)
			{
			DebugOut(SM_ERROR, "ERROR: %s: unable to open Outbox", s_verboseName);
			return hr;
			}

		if (pData->pLast->action.bNotifySender)
		{
			hr = pCMAPIFolder->CreateMessage(&pCMessage);		// Create message to sender
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to create sender message", s_verboseName);
				return hr;
				}

			// To Sender properties

			v[0].ulPropTag = PR_SUBJECT;	// Subject
			v[0].Value.LPSZ = pData->pLast->action.szSenderSubject;

			v[1].ulPropTag = PR_DELETE_AFTER_SUBMIT;	// Delete messsage after send
			v[1].Value.b = true;

			v[2].ulPropTag = PR_RTF_IN_SYNC;	// Force RTF sync
			v[2].Value.b = false;

			hr = pCMessage->SetProps(3, v, 0);
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to set props", s_verboseName);
				return hr;
				}

			// Create message body

			hr = pCMessage->OpenProperty(PR_BODY, &IID_IStream, STGM_WRITE, MAPI_MODIFY | MAPI_CREATE, (IUnknown **) &pBody);
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to create body", s_verboseName);
				return hr;
				}

			crlfLen = _tcslen(s_crlf);
		
			len = _tcslen(pData->pLast->action.szSenderMessage);
			if (len)
			{
				hr = pBody->Write(pData->pLast->action.szSenderMessage, len, 0);
				if (hr != S_OK)
					{
					DebugOut(SM_ERROR, "ERROR: %s: unable to write warning data", s_verboseName);
					return hr;
					}

				// Always append a linefeed carriage return in case of truncation.
				hr = pBody->Write(s_crlf, crlfLen, 0);
				if (hr != S_OK)
					{
					DebugOut(SM_ERROR, "ERROR: %s: unable to write warning crlf", s_verboseName);
					return hr;
					}
			}

			// append in attachment specific text

			pNext = pData->pLast;
			while (pNext)
			{
				len = _tcslen(pNext->action.szSenderInfectionInformation);
				if (len)
				{
					hr = pBody->Write(pNext->action.szSenderInfectionInformation, len, 0);
					if (hr != S_OK)
						{
						DebugOut(SM_ERROR, "ERROR: %s: unable to write infection data", s_verboseName);
						return hr;
						}

					// Always append a linefeed carriage return in case of truncation.
					hr = pBody->Write(s_crlf, crlfLen, 0);
					if (hr != S_OK)
						{
						DebugOut(SM_ERROR, "ERROR: %s: unable to write infection crlf", s_verboseName);
						return hr;
						}

				}
				pNext = pNext->pNext;
			}

			pBody.Release();

			// Create recipient list

			recipients = 1;
			cb = CbNewADRLIST(recipients);
			m_allocators.AllocateBuffer(cb, (LPVOID *) &pal);
			memset(pal, 0, cb);
			pal->cEntries = recipients;

			cb = 7 * sizeof(SPropValue);
			m_allocators.AllocateBuffer(cb, (LPVOID *) &pal->aEntries[0].rgPropVals);
			paProps = pal->aEntries[0].rgPropVals;
			memset(paProps, 0, cb);
			pal->aEntries[0].cValues = 7;

			// PR_ENTRYID

			psb = pData->pCMessage->SentRepresentingEntryID();
			paProps[0].ulPropTag = PR_ENTRYID;
			m_allocators.AllocateMore(psb->cb, paProps, (LPVOID *) &paProps[0].Value.bin.lpb);
			memcpy(paProps[0].Value.bin.lpb, psb->lpb, psb->cb);

			// PR_DISPLAY_NAME

			psz = pData->pCMessage->SentRepresentingName();
			paProps[1].ulPropTag = PR_DISPLAY_NAME;
			m_allocators.AllocateMore((_tcslen(psz) + 1) * sizeof(TCHAR), paProps, (LPVOID *) &paProps[1].Value.LPSZ);
			_tcscpy(paProps[1].Value.LPSZ, psz);

			// PR_ADDRTYPE

			psz = pData->pCMessage->SentRepresentingAddrType();
			paProps[2].ulPropTag = PR_ADDRTYPE;
			m_allocators.AllocateMore((_tcslen(psz) + 1) * sizeof(TCHAR), paProps, (LPVOID *) &paProps[2].Value.LPSZ);
			_tcscpy(paProps[2].Value.LPSZ, psz);

			// PR_EMAIL_ADDRESS

			psz = pData->pCMessage->SentRepresentingEmailAddress();
			paProps[3].ulPropTag = PR_EMAIL_ADDRESS;
			m_allocators.AllocateMore((_tcslen(psz) + 1) * sizeof(TCHAR), paProps, (LPVOID *) &paProps[3].Value.LPSZ);
			_tcscpy(paProps[3].Value.LPSZ, psz);

			// PR_RECIPIENT_TYPE

			paProps[4].ulPropTag = PR_RECIPIENT_TYPE;
			paProps[4].Value.ul = MAPI_TO;

			// PR_OBJECTTYPE

			paProps[5].ulPropTag = PR_OBJECT_TYPE;
			paProps[5].Value.ul = MAPI_MAILUSER;

			// PR_SEARCH_KEY

			psb = pData->pCMessage->SentRepresentingSearchKey();
			paProps[6].ulPropTag = PR_SEARCH_KEY;
			m_allocators.AllocateMore(psb->cb, paProps, (LPVOID *) &paProps[6].Value.bin.lpb);
			memcpy(paProps[6].Value.bin.lpb, psb->lpb, psb->cb);

			// Add the recepient

			hr = pData->peecb->GetSession(0, &pIAddrBook);
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to get session", s_verboseName);
				return E_FAIL;
				}

			hr = pCMessage->ModifyRecipients(MODRECIP_ADD, pal);
			// note that the ModifyRecipients call will throw an exception,
			// so we don't really need to check for any error return!
//			if (hr != S_OK)
//				{
//				DebugOut(SM_ERROR, "ERROR: %s: unable to add recepient", s_verboseName);
//				return hr;
//				}

			// Send the message

			hr = pCMessage->SubmitMessage();
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to submit message", s_verboseName);
				return hr;
				}

			DebugOut(SM_GENERAL, "%s: notified sender", s_verboseName);
		}

		// ----------------------------------------------
		//
		// Send notification messages to administrators
		//
		// ----------------------------------------------

		if (pData->pLast->action.bNotifySelected)
		{
			hr = pCMAPIFolder->CreateMessage(&pCMessage);		// Create message to administrators
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to create admin message", s_verboseName);
				return hr;
				}

			// Set adminstrator message properties

			v[0].ulPropTag = PR_SUBJECT;		// Subject
			v[0].Value.LPSZ = pData->pLast->action.szSelectedSubject;

			v[1].ulPropTag = PR_DELETE_AFTER_SUBMIT;
			v[1].Value.b = true;				// Delete message after send

			v[2].ulPropTag = PR_RTF_IN_SYNC;	// force RTF sync.
			v[2].Value.b = false;

			hr = pCMessage->SetProps(3, v, 0);
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to set props", s_verboseName);
				return hr;
				}

			// Create message body

			hr = pCMessage->OpenProperty(PR_BODY, &IID_IStream, STGM_WRITE, MAPI_MODIFY | MAPI_CREATE, (IUnknown **) &pBody);
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to create body", s_verboseName);
				return hr;
				}

			crlfLen = _tcslen(s_crlf);
		
			len = _tcslen(pData->pLast->action.szSelectedMessage);
			if (len)
			{
				if (len + crlfLen < MAX_MESSAGE_STRING)
				{
					_tcscat(pData->pLast->action.szSelectedMessage, s_crlf);
					len += crlfLen;
				}
				hr = pBody->Write(pData->pLast->action.szSelectedMessage, len, 0);
				if (hr != S_OK)
					{
					DebugOut(SM_ERROR, "ERROR: %s: unable to write warning data", s_verboseName);
					return hr;
					}
			}

			// append in attachment specific text

			pNext = pData->pLast;
			while (pNext)
			{
				len = _tcslen(pNext->action.szSelectedInfectionInformation);
				if (len)
				{
					if (len + crlfLen < MAX_INFECT_INFO)
					{
						_tcscat(pNext->action.szSelectedInfectionInformation, s_crlf);
						len += crlfLen;
					}
					hr = pBody->Write(pNext->action.szSelectedInfectionInformation, len, 0);
					if (hr != S_OK)
						{
						DebugOut(SM_ERROR, "ERROR: %s: unable to write infection data", s_verboseName);
						return hr;
						}
				}
				pNext = pNext->pNext;
			}

			pBody.Release();

			// Count recipients

			recipients = 0;
			pszAdmin = pData->pLast->action.szSelectedRecips;
			while (pszAdmin && *pszAdmin)
			{
				recipients++;
				pszAdmin = _tcschr(pszAdmin, _T(';'));
				if (pszAdmin)
					pszAdmin = _tcsinc(pszAdmin);
			} 

			if (!recipients)
				goto NoRecipients;

			// Create recipient list with the max number of entries
			cb = CbNewADRLIST(recipients);
			m_allocators.AllocateBuffer(cb, (LPVOID *) &pal);
			memset(pal, 0, cb);

			// start our count with no entries
			pal->cEntries = 0;

			// get an address book object
			hr = pData->peecb->GetSession(0, &pIAddrBook);
			if (hr != S_OK)
				{
				DebugOut(SM_ERROR, "ERROR: %s: unable to get session", s_verboseName);
				return E_FAIL;
				}

			// Allocate unresolved properties for each recepient

			pszAdmin = pData->pLast->action.szSelectedRecips;
			for (i = 0; i < recipients && pszAdmin; i++)
			{
				pszNext = _tcschr(pszAdmin, _T(';'));
				if (pszNext)
				{
					LPTSTR p = pszNext;
					pszNext = _tcsinc(pszNext);
					*p = _T('\0');
				}

				// now we have one more entry
				// j = (number of entries - 1) (zero based index)
				int j = pal->cEntries++;

				cb = 2 * sizeof(SPropValue);
				m_allocators.AllocateBuffer(cb, (LPVOID *) &pal->aEntries[j].rgPropVals);
				paProps = pal->aEntries[j].rgPropVals;
				memset(paProps, 0, cb);
				pal->aEntries[j].cValues = 2;

				// PR_DISPLAY_NAME

				paProps[0].ulPropTag = PR_DISPLAY_NAME;
				// we add in extra space (6 chars: 5 in case we need to make a 'one-off' name, and 1 char for the NULL char)
				m_allocators.AllocateMore((_tcslen(pszAdmin) + 6) * sizeof(TCHAR), paProps, (LPVOID *) &paProps[0].Value.LPSZ);
				_tcscpy(paProps[0].Value.LPSZ, pszAdmin);

				// PR_RECIPIENT_TYPE

				paProps[1].ulPropTag = PR_RECIPIENT_TYPE;
				paProps[1].Value.ul = MAPI_TO;

				// attempt to resolve the recipient list (i.e. the new name we added to the list...)
				hr = pIAddrBook->ResolveName((ULONG) HWND_DESKTOP, 0 /* no dialog */, 0 /* no title */, pal);
				if (hr != S_OK)
					{
					// first attempt to create a 'one-off' address
					_tcscpy(paProps[0].Value.LPSZ, _T("[EX:"));
					_tcscat(paProps[0].Value.LPSZ, pszAdmin);
					_tcscat(paProps[0].Value.LPSZ, _T("]"));
					// and try to 'resolve' this
					hr = pIAddrBook->ResolveName((ULONG) HWND_DESKTOP, 0 /* no dialog */, 0 /* no title */, pal);
					// if this was successful, the user should get an "unsuccess delivery" email
					if (hr != S_OK)
						{
						// the name we tried to add to the list is invalid, so we need to re-use the slot in the pal array
						DebugOut(SM_WARNING, "%s: unable to resolve name(s) \"%s\", [%d]", s_verboseName, pszAdmin, hr);
						m_allocators.FreeBuffer(paProps);
						pal->cEntries--;
						}
					}

				pszAdmin = pszNext;
			}

			if (pal->cEntries)
				{
				// Add the recepient list

				hr = pCMessage->ModifyRecipients(MODRECIP_ADD, pal);


				// Send message to adminstrators

				hr = pCMessage->SubmitMessage();

				DebugOut(SM_GENERAL, "%s: notified selected recipients", s_verboseName);
				}
			else
				{
NoRecipients:
				DebugOut(SM_WARNING, "%s: NO selected recipients notified!", s_verboseName);
				}
		}

		DebugOut(SM_GENERAL, "%s: complete", s_verboseName);
		return hr;
	}

	catch (CMyMemoryException e)
		{
		DebugOut(SM_ERROR, VERBOSE_MemError, s_verboseName, e.size);
		return E_OUTOFMEMORY;
		}

	catch (CMAPIException e)
		{
		verboseMAPIException(s_verboseName, e);
		return e.hr;
		}

	catch (...)
		{
		DebugOut(SM_ERROR, VERBOSE_UnkError, s_verboseName);
		return E_UNEXPECTED;
		}
}

/*---------------------------------------------------------------------------*/

// Chook::MoveAttachmentRenderings() is called
//  by pCMessage->EnumAttachments() for each attachment in a message.
//	It changes the rendering position of the attachment by passed in value
//
// Inputs
//	pAttach	Attachment to be scanned
//	pvData	Type cast ULONG which is the rendering position adjustment
//
// Outputs
//	returns	S_OK if enumeration is to continue
//			S_FALSE or error if enumeration is to halt

HRESULT CHook::MoveAttachmentRendering(CAttach *pAttach, void *pvData)
{
	static TCHAR s_verboseName[] = _T("CHook::MoveAttachmentRendering");

	HRESULT
		hr;

	ULONG
		adjustment = (int) pvData,
		pos = pAttach->RenderingPosition();

	SPropValue
		v;

	pos += adjustment;
	if (pos < 0)
		pos = (ULONG) -1;

	v.ulPropTag = PR_RENDERING_POSITION;
	v.Value.ul = pos;
	hr = pAttach->SetProps(1, &v, 0, false);
	if (hr != S_OK)
		return S_FALSE;

	hr = pAttach->SaveChanges(KEEP_OPEN_READONLY | MAPI_DEFERRED_ERRORS);
	if (hr == MAPI_E_OBJECT_CHANGED)
		hr = pAttach->SaveChanges(FORCE_SAVE | MAPI_DEFERRED_ERRORS);

	return S_OK;
}

/*---------------------------------------------------------------------------*/

// CHook::ModifyMessageBody() is called to modify the message body when a virus is found
//
// Inputs:
//	pData		RTScanData for message
//
// Outputs:
//	returns							S_OK if successful
//	pData->messageCommitRequired	Set if successful

HRESULT CHook::ModifyMessageBody(RTScanData *pData)
{
	static TCHAR s_verboseName[] = _T("CHook::ModifyMessageBody");

	CComPtr<IStream>
		pBody,
		pCopy;

	ULARGE_INTEGER
		cb;

	LARGE_INTEGER
		sp;

	HRESULT
		hr;

	SPropValue
		v[2];

	int
		len,
		crlfLen,
		cv,
		messageLen;

	RTActionData
		*pNext;

	try
	{
		// Copy message body to temporary IStream pCopy.

		hr = CreateStreamOnHGlobal(0, true, &pCopy);
		if (hr != S_OK)
			return hr;

		messageLen = 0;
		crlfLen = _tcslen(s_crlf);

		// set main body

		len = _tcslen(pData->pLast->action.szWarningMessage);
		if (len)
		{
			if (len + crlfLen < MAX_MESSAGE_STRING)
			{
				_tcscat(pData->pLast->action.szWarningMessage, s_crlf);
				len += crlfLen;
			}
			hr = pCopy->Write(pData->pLast->action.szWarningMessage, len, 0);
			if (hr != S_OK)
				return hr;
			messageLen += len;
		}


		// append in attachment specific text

		pNext = pData->pLast;
		while (pNext)
		{
			len = _tcslen(pNext->action.szWarningInfectionInformation);
			if (len)
			{
				if (len + crlfLen < MAX_INFECT_INFO)
				{
					_tcscat(pNext->action.szWarningInfectionInformation, s_crlf);
					len += crlfLen;
				}
				hr = pCopy->Write(pNext->action.szWarningInfectionInformation, len, 0);
				if (hr != S_OK)
					return hr;
				messageLen += len;
			}
			pNext = pNext->pNext;
		}

		//copy in old body
		hr = pData->pCMessage->OpenProperty(PR_BODY, &IID_IStream, STGM_READ, 0, (IUnknown **) &pBody, false);
		if (hr != S_OK)
			return hr;

		// test for empty message and empty forward,
        // skip special bytes when copying original message
		unsigned char baCheckBuf[3];
		DWORD dwRead = 3;
		hr = pBody->Read(baCheckBuf, dwRead, &dwRead);
		if( (baCheckBuf[0] != 0xa0 && baCheckBuf[1] != 0x0D && baCheckBuf[2] != 0x0A) && 
			(baCheckBuf[0] != 0x0D && baCheckBuf[1] != 0x0A && baCheckBuf[2] != 0x0D)){
			sp.QuadPart = 0;
			hr = pBody->Seek( sp, STREAM_SEEK_SET,0 );
		}

		cb.QuadPart = (unsigned __int64) -1;
		hr = pBody->CopyTo(pCopy, cb, NULL, NULL);
		if (hr != S_OK)
			return hr;

		pBody.Release();

		// Update the rendering positions of all of the attachments by the
        // amount of additional warning text we're inserting

		hr = pData->pCMessage->EnumAttachments(MoveAttachmentRendering, (void *) messageLen);

		// Now replace body with our new message

		hr = pData->pCMessage->OpenProperty(PR_BODY, &IID_IStream, STGM_WRITE, MAPI_MODIFY | MAPI_CREATE, (IUnknown **) &pBody);
		if (hr != S_OK)
			return hr;

		sp.QuadPart = 0;
		hr = pCopy->Seek(sp, STREAM_SEEK_SET, 0);

		cb.QuadPart = (unsigned __int64) -1;
		hr = pCopy->CopyTo(pBody, cb, NULL, NULL);
		if (hr != S_OK)
			return hr;

		// Now insure that rich text gets re-syncronized.

		cv = 0;
		v[cv].ulPropTag = PR_RTF_IN_SYNC;
		v[cv++].Value.b = false;

		if (pData->pLast->action.bChangeSubject)
		{
			v[cv].ulPropTag = PR_SUBJECT;
			v[cv++].Value.LPSZ = pData->pLast->action.szWarningSubject;
		}

		hr = pData->pCMessage->SetProps(cv, v, 0);

		pData->messageCommitRequired = true;

		return hr;
	}

	catch (CMyMemoryException e)
		{
		DebugOut(SM_ERROR, VERBOSE_MemError, s_verboseName, e.size);
		return E_OUTOFMEMORY;
		}

	catch (CMAPIException e)
		{
		verboseMAPIException(s_verboseName, e);
		return e.hr;
		}

	catch (...)
		{
		DebugOut(SM_ERROR, VERBOSE_UnkError, s_verboseName);
		return E_UNEXPECTED;
		}
}

/*---------------------------------------------------------------------------*/

// CHook::RealTimeScanAttachment() is called (by pCMessage->EnumAttachments()) for each attachment in a message.
// It qualifies, copies, sends the attachment the SE, and performs the SE requests.
//
// Inputs
//	pAttach	Attachment to be scanned
//	pvData	Pointer to real time scan data (Initialized in CHook::InboundMessageHook())
//
// Outputs
//	returns	S_OK if enumeration is to continue
//			S_FALSE or error if enumeration is to halt
//	*pvData	updated with information about current attachment
//
// Exceptions
//	CMyMemoryExeception and CMAPIException possible.

HRESULT CHook::RealTimeScanAttachment(CAttach *pAttach, void *pvData)
{
	static TCHAR s_verboseName[] = _T("CHook::RealTimeScanAttachment");

	TCHAR szTempPath[MAX_PATH];

	RTScanData
		*pData = (RTScanData *) pvData;

	ULONG
		method;

	CTemporaryFileRemover
		tempFile;

	PPROCESSBLOCK
		pProcessBlock;

	RTActionData
		*pActionData;

	int
		index,
		SEComplete;

	DWORD
		instanceID;

	LPCTSTR
		pszWork;

	HRESULT
		rv = S_OK;

	LPTSTR 
		pTmp;

	if (pData->signature != RTScanData::SIGNATURE)
		{
		DebugOut(SM_ERROR, "ERROR: %s: MEC signature mismatch!", s_verboseName);
		return E_FAIL;
		}

	g_pMemFile->cInboundAttachments++;

	method = pAttach->AttachMethod();
	DebugOut(SM_GENERAL, "%s: [%u] \"%s\", method: %s, level = %x, ext \"%s\", file \"%s\", lfile \"%s\", path \"%s\", lpath \"%s\", size: %u",
		s_verboseName,
		pAttach->AttachNum(), 
		pAttach->DisplayName(),

		method == NO_ATTACHMENT			? _T("NO_ATTACHMENT")			:
		method == ATTACH_BY_VALUE		? _T("ATTACH_BY_VALUE")			:
		method == ATTACH_BY_REFERENCE	? _T("ATTACH_BY_REFERENCE")		:
		method == ATTACH_BY_REF_RESOLVE	? _T("ATTACH_BY_REF_RESOLVE")	:
		method == ATTACH_BY_REF_ONLY	? _T("ATTACH_BY_REF_ONLY")		:
		method == ATTACH_EMBEDDED_MSG	? _T("ATTACH_EMBEDDED_MSG")		:
		method == ATTACH_OLE			? _T("ATTACH_OLE")				:
										  _T("Unknown Method"),
		pAttach->AccessLevel(),
		pAttach->AttachExtension(),
		pAttach->AttachFilename(),
		pAttach->AttachLongFilename(),
		pAttach->AttachPathname(),
		pAttach->AttachLongPathname(),
		pAttach->AttachSize() );

	if (!pData->pCHook->ScanThisAttachment(pAttach, pData))
		{
		DebugOut(SM_GENERAL, "%s: attachment skipped", s_verboseName);
		return S_OK;
		}

    if (!GetTempPath(MAX_PATH, szTempPath))
		{
		DebugOut(SM_ERROR, "ERROR: %s: unable to get temp path", s_verboseName);
		return E_FAIL;
		}

    _tcscat(szTempPath, _T("VPMECTMP"));

    if (!CreateDirectory(szTempPath, NULL))
		{
		int err = GetLastError();
		if (!(err == ERROR_ALREADY_EXISTS))
			{
			DebugOut(SM_ERROR, "ERROR: %s: unable to create temp dir \"%s\" [%d]", s_verboseName, szTempPath, err);
			return E_FAIL;
			}
		}

	pProcessBlock = AcquireProcessBlock(&index);
	if (!pProcessBlock)
		{
		DebugOut(SM_ERROR, "ERROR: %s: unable to get process block", s_verboseName);
		return E_FAIL;
		}

	// Any exits after this point MUST free the process block!!!

	// create a temp file using the instanceID
	sssnprintf(tempFile.szPath, sizeof(tempFile.szPath), "%s\\%s%.u%s", szTempPath, _T("MEC"), pProcessBlock->instanceID, pAttach->AttachExtension());
	DeleteFile(tempFile.szPath);	// delete any lingering file...

	// now copy the attachment to the temp file
	if (pAttach->CopyToTemporaryFile(tempFile.szPath) != S_OK)
		{
		DebugOut(SM_ERROR, "ERROR: %s: unable to copy to temp file", s_verboseName);
		pProcessBlock->state = PROCESSBLOCK::Free;	// we must free process block before leaving this function
		return E_FAIL;
		}

	instanceID = pProcessBlock->instanceID;

	pszWork = pAttach->AttachLongFilename();
	if (_tcslen(pszWork) >= (MAX_PATH - 1))
		pszWork = pAttach->AttachFilename();

	ssStrnCpy(pProcessBlock->input.szAttachmentName, pszWork,                               sizeof (pProcessBlock->input.szAttachmentName));
	ssStrnCpy(pProcessBlock->input.szExtension,      pAttach->AttachExtension(),            sizeof (pProcessBlock->input.szExtension));
	ssStrnCpy(pProcessBlock->input.szSubject,        pData->pCMessage->NormalizedSubject(), sizeof (pProcessBlock->input.szSubject));

	{
	TCHAR _temp_str[MAX_SENDER_STRING];
	sssnprintf(_temp_str, sizeof (_temp_str), _T("%s <%s>"), pData->pCMessage->SenderName(), pData->pCMessage->SenderEmail());
	ssStrnCpy(pProcessBlock->input.szSender, _temp_str, sizeof (pProcessBlock->input.szSender));
	}

	ssStrnCpy(pProcessBlock->input.szTempPath, tempFile.szPath, sizeof (pProcessBlock->input.szTempPath));
	ssStrnCpy(pProcessBlock->input.szRecipients, pData->pCMessage->Recipients(), sizeof (pProcessBlock->input.szRecipients));

	// Parse out any ','s so we don't break the log
	pTmp = pProcessBlock->input.szRecipients;
	while(pTmp = _tcschr(pTmp, ','))
		*pTmp = ' ';
	
	// Get the messaging user name
	ssStrnCpy(pProcessBlock->input.szUserName, pData->pCMessage->UserName(), sizeof (pProcessBlock->input.szUserName));
	
	// Parse out any ','s so we don't break the log
	pTmp = pProcessBlock->input.szUserName;
	while(pTmp = _tcschr(pTmp, ','))
		*pTmp = ' ';

	pProcessBlock->state = PROCESSBLOCK::Scan;	// state must be LAST process block value changed before releasing SE
	ReleaseSemaphore(g_hRTSemaphore, 1, 0);		// release SE

	g_pMemFile->cInboundAttachmentsProcessed++;

	DebugOut(SM_GENERAL, "%s: waiting for scan...", s_verboseName);

	WaitForSingleObject(g_hPBEvent[index], WAIT_MemEvent);	// Wait on SE

	SEComplete = false;

    switch ( WaitForSingleObject(g_hMemMutex, WAIT_MemMutex) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
		pProcessBlock->instanceID = 0;			// Let SE know we're not waiting anymore
		if (pProcessBlock->state == PROCESSBLOCK::Complete)
			SEComplete = true;
		else
			tempFile.Clear();					// If SE not yet complete, it must delete temporary file

		ReleaseMutex(g_hMemMutex);
        break;

    default:
		pProcessBlock->instanceID = 0;			// Can't get mutex?  Clear instance ID anyway
        break;
    }

	// Now process any actions

	if (SEComplete)
		{
		int commit = false;
		int virusFound = false;

		switch (pProcessBlock->action.code)
			{
			default :
			case ACTIONDATA::None :
			DebugOut(SM_GENERAL, "%s: scan complete, action = None", s_verboseName);
			break;

			case ACTIONDATA::Remove :
			DebugOut(SM_GENERAL, "%s: scan complete, action = Remove", s_verboseName);
			pData->pCMessage->DeleteAttach(pAttach->AttachNum());
			virusFound = pData->messageCommitRequired = true;
			break;

			case ACTIONDATA::Replace :
			DebugOut(SM_GENERAL, "%s: scan complete, action = Replace", s_verboseName);
			pAttach->CopyFromFile(tempFile.szPath);
			virusFound = pData->messageCommitRequired = commit = true;
			break;

			case ACTIONDATA::LeaveAlone :
			DebugOut(SM_GENERAL, "%s: scan complete, action = LeaveAlone", s_verboseName);
			virusFound = true;
			break;
			}

		if (virusFound)
			{
			pActionData = new(false) RTActionData;
			if (pActionData)
				{
				pActionData->action = pProcessBlock->action;	// large copy
				pData->AddActionData(pActionData);
				pData->virusFound = true;
				}
			}

		if (commit)
			{
			rv = pAttach->SaveChanges(KEEP_OPEN_READWRITE /*| MAPI_DEFERRED_ERRORS*/);
			if (rv == MAPI_E_OBJECT_CHANGED)
				rv = pAttach->SaveChanges(FORCE_SAVE /*| MAPI_DEFERRED_ERRORS*/);
			}
		}
	else
		{
		rv = S_FALSE;
		g_pMemFile->cInboundAttachmentsTimeout++;
		DebugOut(SM_WARNING, "%s: timeout! [%u]", s_verboseName, pAttach->AccessLevel());
		}

	pProcessBlock->state = PROCESSBLOCK::Free;	// we must free process block before leaving this function

	return rv;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CHook::OnRead(LPEXCHEXTCALLBACK lpeecb)
{
	static TCHAR s_verboseName[] = _T("CHook::OnRead");

	HRESULT
		hr;

	CComPtr<IMessage>
		pIMessage;

	CComPtr<IMsgStore>
		pIMsgStore;

	SmartPtr<CMsgStore>
		pCMsgStore;

	SmartPtr<CMessage>
		pCMessage;

	RTScanData
		data;

	SPropValue sProps = {0};
	BOOL bCommit = FALSE;
	BOOL bAPdisabled = FALSE;
			
	// get the debug property that the storage ext set...
    UpdateDebugFlag();

//	DebugOut(SM_GENERAL, s_verboseName);

	// tell OnReadComplete to show its dialog
	m_hrOnReadComplete = S_FALSE;	

    // Skip message unless at least one SE Real Time Watch is active
	if ( ( GetCommunicationState() != COMM_INITIALIZED ) ||
         !g_pMemFile->cSERTWatch || 
         !g_pMemFile->bRTEnabled )
		{
		DebugOut(SM_WARNING, "%s: RTS not enabled", s_verboseName);
		return S_FALSE;
		}

	try
		{
		g_pMemFile->cInboundMessages++;

		hr = lpeecb->GetObject(&pIMsgStore, (LPMAPIPROP *)&pIMessage);
		if (hr != S_OK || !pIMsgStore || !pIMessage)
			{
			DebugOut(SM_ERROR, "ERROR: %s: unable to get interface objs", s_verboseName);
			return S_FALSE;
			}

		pCMsgStore = new CMsgStore(pIMsgStore, m_allocators, false);
		pCMessage = new CMessage(pIMessage, m_allocators);

		if (pCMessage->HasAttach() && (pCMessage->MessageFlags() & MSGFLAG_UNSENT) == 0)
			{
			if (g_pMemFile->bMarkMessages)
				{
				// Check if this message has our signature
				LPCTSTR szSigStr = pCMessage->HasSig();
				if (_tcscmp(szSigStr, "LanDeskVirusProtect"))
					{
					// No signature, hence add 
					sProps.ulPropTag = PR_HASSIG;
					sProps.Value.lpszA = "LanDeskVirusProtect";

					hr = pCMessage->SetProps(1, &sProps, 0);
					
					// Whether above call succeeded or failed just continue scanning
					// We need to commit the above changes
					bCommit = TRUE;
					}
				else 
					{
					// Signature found; Message already scanned hence return
					DebugOut(SM_WARNING, "%s: signature found; message skipped", s_verboseName);
					return S_FALSE;
					}
				}


			DebugOut(SM_GENERAL, "%s: attachments found", s_verboseName);
		
			// Setup message level data for scan

            switch ( WaitForSingleObject(g_hMemMutex, WAIT_MemMutex) )
            {
            case WAIT_OBJECT_0:
            case WAIT_ABANDONED:
                // Okay
                break;
            default:
				DebugOut(SM_WARNING, "%s: unable to get mutex; message skipped", s_verboseName);
				return S_FALSE;		// If can't safely access MemFile, don't scan the message
            }

			data.extInfo = g_pMemFile->extInfo;	// Make a copy extension information

			ReleaseMutex(g_hMemMutex);

			// disable AP!
			bAPdisabled = TRUE;
			SAVRT_PTR SavrtModulePtr(CSavrtModuleInterface::Init(false));
			if( SavrtModulePtr.Get() != NULL ){
				SavrtModulePtr->UnProtectProcess();
			}

			// Enumerate the message recipients
			pCMessage->EnumRecipients();

			data.pCMessage = pCMessage;
			data.pCMsgStore = pCMsgStore;
			data.pCHook = this;
			data.peecb = lpeecb;

			g_pMemFile->cInboundMessagesProcessed++;

			hr = pCMessage->EnumAttachments(RealTimeScanAttachment, &data);

			if (data.virusFound && data.pLast)
				{
				if (data.pLast->action.bNotifySender || data.pLast->action.bNotifySelected)
					hr = SendNotificationMessages(&data);

				if (data.pLast->action.bInsertWarning)
					hr = ModifyMessageBody(&data);
				}

			if ((data.messageCommitRequired) || (bCommit == TRUE))
				{
				hr = pCMessage->SaveChanges(KEEP_OPEN_READWRITE);
				if (hr == MAPI_E_OBJECT_CHANGED)
					hr = pCMessage->SaveChanges(FORCE_SAVE);
				}
			}
		else
			{
			DebugOut(SM_GENERAL, "%s: no attachments found", s_verboseName);
			return S_FALSE;
			}
		}

	catch (CMAPIException e)
		{
		verboseMAPIException(s_verboseName, e);
		}

	catch (CMyMemoryException e)
		{
		DebugOut(SM_ERROR, VERBOSE_MemError, s_verboseName, e.size);
		}

	catch (...)
		{
		DebugOut(SM_ERROR, VERBOSE_UnkError, s_verboseName);
		}

	// re-enable AP
	if (bAPdisabled){
		SAVRT_PTR SavrtModulePtr(CSavrtModuleInterface::Init(false));
		if( SavrtModulePtr.Get() != NULL ){
			SavrtModulePtr->ProtectProcess();
		}
	}

	DebugOut(SM_GENERAL, "%s: exit", s_verboseName);
    return S_FALSE;
}


/*===========================================================================*/
// CExchExt class implementation follows:
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

CExchExt::CExchExt() : 
        m_signature(SIGNATURE),
        m_context(0),
        m_bMAPILoaded(false),
        m_cRef(1)
{
    // Initialize and cache CSavrtModuleInterface (deletable == false) for disabling AP.
    CSavrtModuleInterface::Init(false);

    DebugOut(SM_GENERAL, "CExchExt object created [%x]", this);
}


CExchExt::~CExchExt()
{
    DebugOut(SM_GENERAL, "CExchExt object destroyed [%x]", this);

    if (m_pCHook)
        m_pCHook->NonDelegatingRelease();

    if (m_bMAPILoaded)
        UnloadMAPI();

    // Make sure CSavrtModuleInterface is deletable when all references gone (presumably now)
    SAVRT_PTR SavrtModulePtr( CSavrtModuleInterface::Init() );
    if ( SavrtModulePtr.Get() != NULL )
        SavrtModulePtr->SetDelete();
}


HRESULT CExchExt::Initialize(void)
{
	HRESULT
		hr;

	hr = LoadMAPI();
	if (hr != S_OK)
		return hr;

	m_bMAPILoaded = true;

	m_allocators.Set(g_pfnMAPIAllocateBuffer, g_pfnMAPIAllocateMore, g_pfnMAPIFreeBuffer, g_pfnFreeProws, g_pfnFreePaddrlist);

	m_pCHook = new(false) CHook(this, m_allocators);
	if (!m_pCHook)
		return E_OUTOFMEMORY;

	hr = m_pCHook->Initialize();
	if (hr != S_OK)
		return hr;

	DebugOut(SM_GENERAL, "CExchExt initialization complete");

	return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CExchExt::QueryInterface(REFIID pIID, void **ppvObject)
{
	if (IsEqualIID(pIID, IID_IUnknown))
	{
		*ppvObject = (IUnknown *) this;
		AddRef();
		return S_OK;
	}

	if (IsEqualIID(pIID, IID_IExchExt))
	{
		*ppvObject = (IExchExt *) this;
		AddRef();
		return S_OK;
	}

	if (IsEqualIID(pIID, IID_IExchExtMessageEvents))
	{
		*ppvObject = (IExchExtMessageEvents *) m_pCHook;
		AddRef();
		return S_OK;
	}

	return MAPI_E_INTERFACE_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(ULONG) CExchExt::AddRef(void)
{
    return ++m_cRef;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(ULONG) CExchExt::Release(void)
{
	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CExchExt::Install(LPEXCHEXTCALLBACK /* peecb */, ULONG eecontext, ULONG /* ulFlags */)
{
	HRESULT
		hr;

	m_context = eecontext;
	   
	switch (eecontext)
	{
	case EECONTEXT_READNOTEMESSAGE:
		DebugOut(SM_GENERAL, "CExchExt: context READNOTEMESSAGE installed");
		hr = S_OK;
		break;
	case EECONTEXT_SENDNOTEMESSAGE:
		DebugOut(SM_GENERAL, "CExchExt: context SENDNOTEMESSAGE installed");
		hr = S_OK;
		break;
	case EECONTEXT_READPOSTMESSAGE:
		DebugOut(SM_GENERAL, "CExchExt: context READPOSTMESSAGE installed");
		hr = S_OK;
		break;
	case EECONTEXT_SENDPOSTMESSAGE:
		DebugOut(SM_GENERAL, "CExchExt: context SENDPOSTMESSAGE installed");
		hr = S_OK;
		break;
	case EECONTEXT_SENDRESENDMESSAGE:
		DebugOut(SM_GENERAL, "CExchExt: context SENDRESENDMESSAGE installed");
		hr = S_OK;
		break;
	case EECONTEXT_READREPORTMESSAGE:
		DebugOut(SM_GENERAL, "CExchExt: context READREPORTMESSAGE installed");
		hr = S_OK;
		break;

	default:
		DebugOut(SM_WARNING, "CExchExt: context (%u) skipped", eecontext);
		hr = S_FALSE;
		break;
	}

	return hr;
}

/*---------------------------------------------------------------------------*/

// Main entry point for Client extension.  This is the "main" routine for the MEC portion of this .DLL.
// ExchEntryPoint() is called by exchange clients to get the top level interface pointer.

LPEXCHEXT CALLBACK ExchEntryPoint()
{
	CExchExt
		*pCExchExt;

	HRESULT
		hr;

	if (g_personality != MEC_Personality)
		{
		g_debug = FALSE;// assume we do not want debug output until we map
						// to the shared buffer, at which time we will
						// get the real value of the debug flag

		g_personality = MEC_Personality;
		}

	pCExchExt = new(false) CExchExt;
	if (!pCExchExt)
		{
		DebugOut(SM_ERROR, "ERROR creating new CExchExt object");
		return 0;
		}

	hr = pCExchExt->Initialize();
	if (hr != S_OK)
		{
		DebugOut(SM_ERROR, "ERROR initializing CExchExt object");
		pCExchExt->Release();
		return 0;
		}

	DebugOut(SM_GENERAL, "ExchEntryPoint exited; new hook obj created");

	return (LPEXCHEXT) pCExchExt;
}

/* end source file */