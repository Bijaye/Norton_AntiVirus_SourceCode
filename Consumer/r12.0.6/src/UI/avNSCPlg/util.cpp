// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "stdafx.h"

// network api for admin determination
#include<lm.h>

#include "util.h"
#include "CConsoleTypeNotInstalled.h"


nsc::NSCRESULT CreateAction(nscLib::CAction **ppAction,
                             DWORD dwActionName,
                             DWORD dwActionDesc)
{
    nsc::NSCRESULT result = nsc::NSC_SUCCESS;

    // check params
    if((NULL == ppAction) || (NULL != *ppAction))
    {
        CCTRCTXE0(_T("invalid parameter(s)"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    // alloc CAction
	*ppAction = new nscLib::CAction();
    if(NULL == *ppAction)
    {
		CCTRCTXE0(_T("Could not create CAction object, returning NSCERR_MEMORY"));
        return nsc::NSCERR_MEMORY;
    }

	(*ppAction)->AddRef();

    // add name
	ccLib::CString sActionName;	
	if(!sActionName.LoadString(dwActionName))
    {
        CCTRCTXE2(_T("LoadString failed on %d with error %d"), dwActionName, ::GetLastError());
        sActionName = "";
    }

    cc::IStringPtr spStrActionName;
	spStrActionName.Attach(ccSym::CStringImpl::CreateStringImpl(sActionName));
    if(!spStrActionName)
    {
        CCTRCTXE0(_T("String implementation failed for action name."));
        (*ppAction)->Release();
        *ppAction = NULL;
        return nsc::NSC_FAIL;
    }

    result = (*ppAction)->SetName(spStrActionName);
    if(NSC_FAILED(result))
	{
        CCTRCTXE1(_T("Action::SetName failed nscerror=%d"),result);
        (*ppAction)->Release();
        *ppAction = NULL;
		return result;
	}

    // add description
	ccLib::CString sActionDesc;	
	if(!sActionDesc.LoadString(dwActionDesc))
    {
        CCTRCTXE2(_T("LoadString failed on %d with error %d"), dwActionDesc, ::GetLastError());
        sActionDesc = "";
    }

    cc::IStringPtr spStrActionDesc;
	spStrActionDesc.Attach(ccSym::CStringImpl::CreateStringImpl(sActionDesc));
    if(!spStrActionDesc)
    {
        CCTRCTXE0(_T("String implementation failed for action description."));
        (*ppAction)->Release();
        *ppAction = NULL;
        return nsc::NSC_FAIL;
    }

    result = (*ppAction)->SetDescription(spStrActionDesc);
    if(NSC_FAILED(result))
	{
        (*ppAction)->Release();
        *ppAction = NULL;
		return result;
	}

    return result;
}

nsc::NSCRESULT CreateClientAction(
    nsc::IAction **ppActionReturn,
    const DWORD dwNameID,
    const DWORD dwDescID,
    const CClientCallback::e_CallbackCookie eCallbackCookie,
    const GUID& guid_action,
	const GUID& guid_callback)
{
    // check params
    if((NULL == ppActionReturn) || (NULL != *ppActionReturn))
    {
        CCTRCTXE0(_T("invalid parameter(s)"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nscLib::CAction* pActionTemp = NULL;
    nsc::NSCRESULT result = CreateAction (&pActionTemp,dwNameID, dwDescID);
    if (NSC_FAILED(result))
    {
		CCTRCTXE1(_T("CreateAction failed: %d"), result);
        return result;
    }

    cc::IStringPtr spStrModPath;
	spStrModPath.Attach(ccSym::CStringImpl::CreateStringImpl(g_szModulePath));
    if(!spStrModPath)
    {
        CCTRCTXE0(_T("String implementation failed for client module path."));
        pActionTemp->Release();
        return nsc::NSC_FAIL;
    }

    // add client callback
    result = pActionTemp->SetClientCallback(spStrModPath, guid_callback, eCallbackCookie);
    if (NSC_FAILED(result))
	{
        CHECK_NSCRESULT(result);
        pActionTemp-> Release();
        return result;
	}

    // add action guid
    result = pActionTemp->SetGUID(guid_action);
    if (NSC_FAILED(result))
	{
        CHECK_NSCRESULT(result);
        pActionTemp-> Release();
        return result;
	}

    *ppActionReturn = pActionTemp;

    return result;
}


nsc::NSCRESULT CreateStatus (nsc::IConsoleType **ppStatusReturn, nsc::e_SymConsoleType ConsoleType, bool bOnOff)
{
    nsc::NSCRESULT result = nsc::NSC_SUCCESS;

    // check params
    if (ppStatusReturn == NULL || *ppStatusReturn != NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

	if(nsc::ONOFF_CONSOLETYPE == ConsoleType)
	{
		nscLib::COnOff *pStatus = new nscLib::COnOff();
		if (pStatus == NULL)
		{
			return nsc::NSCERR_MEMORY;
		}
		pStatus->AddRef();

		// set status
		result = pStatus->SetValue(bOnOff);
		if (NSC_FAILED(result))
		{
			TRACEE(_T("SetValue failed"));
			pStatus->Release();
			pStatus = NULL;
		}

		*ppStatusReturn = pStatus;
	}
	else if(nsc::NOTINSTALLED_CONSOLETYPE == ConsoleType)
	{
		CConsoleTypeNotInstalled *pStatus = new CConsoleTypeNotInstalled();
		if (pStatus == NULL)
		{
			return nsc::NSCERR_MEMORY;
		}
		pStatus->AddRef();

		*ppStatusReturn = pStatus;
	}
	else
	{
		return nsc::NSCERR_ILLEGALPARAM;
	}

	return result;
}

nsc::NSCRESULT CreateStatus (nsc::IConsoleType **ppStatusReturn, 
							 cc::IString*& status_string)
{
    nsc::NSCRESULT result = nsc::NSC_SUCCESS;

    // check params
    if (ppStatusReturn == NULL || *ppStatusReturn != NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nscLib::CStringType *pStatus = new nscLib::CStringType(status_string);
    if (pStatus == NULL)
    {
        return nsc::NSCERR_MEMORY;
    }
    pStatus->AddRef();

    *ppStatusReturn = pStatus;

    return result;
}


// create health with no action
//  used by other createhealth and for good health
nsc::NSCRESULT CreateHealth (    
        nsc::IHealth **ppHealthReturn,
        nsc::e_health e_health)
{
    nsc::NSCRESULT result = nsc::NSC_SUCCESS;

    // check params
    if (ppHealthReturn == NULL || *ppHealthReturn != NULL)
    {
        TRACEE(_T("Bad param"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    // create health
    nscLib::CHealth *pHealth = new nscLib::CHealth();
    if (pHealth == NULL)
    {
        return nsc::NSCERR_MEMORY;
    }
    pHealth->AddRef();

    // set value
    result = pHealth->SetHealthLevel(e_health);
    if (NSC_FAILED(result))
    {
        TRACEE(_T("SetHealthLevel failed"));
        pHealth->Release();
        return result;
    }

    // notify is now always set to false (default)
    // this will prevent the miniui from popping up 

    *ppHealthReturn = pHealth;

    return result;
}


nsc::NSCRESULT CreateHealth (nsc::IHealth **ppHealthReturn,
                            nsc::e_health e_health,
                            DWORD dwNameID,
                            DWORD dwDescID,
                            CClientCallback::e_CallbackCookie eCallbackCookie,
                            const GUID& guidAction,
							const GUID& guidCallback)
{
    nsc::NSCRESULT result = nsc::NSC_SUCCESS;

    // create health
    result = CreateHealth(ppHealthReturn, e_health);
    if (NSC_FAILED(result))
    {
        CHECK_NSCRESULT(result);
        return result;
    }

    // store in temp and set return to NULL in case of failure
    nscLib::CHealth *pHealth = (nscLib::CHealth *) *ppHealthReturn;
    *ppHealthReturn = NULL;

    // add action
    nsc::IAction *pAction = NULL;
    result = CreateClientAction (&pAction, dwNameID, dwDescID, eCallbackCookie, guidAction, guidCallback);
    if (NSC_FAILED(result))
    {
        TRACEE(_T("CreateServerAction failed"));
        CHECK_NSCRESULT(result);
        pHealth->Release();
        return result;
    }

    result = pHealth->SetFixAction(pAction);
	pAction->Release();
    if (NSC_FAILED(result))
    {
        TRACEE(_T("SetFixAction failed"));
        CHECK_NSCRESULT(result);
        pHealth->Release();
        return result;
    }

    *ppHealthReturn = pHealth;

    return result;
}

const UINT BUFFER_SIZE = 1024;

// takes username and gets list of SIDs for groups
//  checks each group to see if it is local admin group
bool IsAdminUser (LPCTSTR strName)
{
    USES_CONVERSION;

    NET_API_STATUS status = NERR_Success;
    do
    {
        LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
        DWORD dwEntriesRead = 0;
        DWORD dwTotalEntries = 0;
        status = NetUserGetLocalGroups(NULL,T2W(strName), 0, 0, 
            (LPBYTE*)&pBuf, MAX_PREFERRED_LENGTH, &dwEntriesRead, &dwTotalEntries);
		if(status == NERR_Success || status == ERROR_MORE_DATA)
		{
			for(DWORD i = 0; i < dwEntriesRead; ++i)
			{
				BYTE abySidBuffer[BUFFER_SIZE];
				PISID pSid = (PISID) abySidBuffer;
				DWORD dwSidLength = sizeof(abySidBuffer);

                WCHAR szDomain[BUFFER_SIZE];
				DWORD dwDomainLength = sizeof(szDomain) / sizeof(*szDomain);

				SID_NAME_USE tNameUse;
				BOOL bRet = LookupAccountNameW(NULL, pBuf[i].lgrui0_name, pSid, &dwSidLength, szDomain, &dwDomainLength, &tNameUse);
				if (bRet)
				{
					for (DWORD i=0; i<pSid->SubAuthorityCount; i++)
					{
//                        TRACEI("   checking subauthority %d\n", i);
//                        TRACEI("   subauth value = %x\n", pSid->SubAuthority[i]);

	                    if(pSid->SubAuthority[i] == DOMAIN_ALIAS_RID_ADMINS)
                        {
                            // user is an admin
                            NET_API_STATUS statusFree = NetApiBufferFree(pBuf);
                            if (statusFree != NERR_Success)
                            {
                                TRACEI(_T("NetApiBufferFree failed %d"), statusFree);
                            }
                            return true;
                        }
					}
				}
			}
		}
        else
        {
            TRACEI("NetUserGetLocalGroups failed with %d\n", status);
            return false;
        }

        NET_API_STATUS statusFree = NetApiBufferFree(pBuf);
        if (statusFree != NERR_Success)
        {
            TRACEI(_T("NetApiBufferFree failed %d"), statusFree);
        }
    }
    while(status == ERROR_MORE_DATA);

    return false;
}


// get name from sid
// allocates and returns buffer
//  caller must delete
bool GetNameFromSid (LPTSTR *pstrName, LPCTSTR strUserSID)
{
    USES_CONVERSION;

    if (*pstrName!= NULL)
    {
        TRACEE(_T("Invalid parameter!"));
        return false;
    }

    PISID pSid = NULL;

    // convert strSID to SID
    BOOL bRet = ConvertStringSidToSid(strUserSID, (PSID*)&pSid);
    if (!bRet)
    {
        CCTRCTXE1(_T("ConvertStringSidToSid failed (0x%08x)"), GetLastError());
        return false;
    }

    TCHAR strName[BUFFER_SIZE];
    DWORD dwNameSize = BUFFER_SIZE;

    TCHAR strDomain[BUFFER_SIZE];
    DWORD dwDomainSize = BUFFER_SIZE;

    SID_NAME_USE NameUse;

    bRet = LookupAccountSid(NULL, (PSID)pSid,
        strName, &dwNameSize,
        strDomain, &dwDomainSize,
        &NameUse);
	DWORD dwLastError = ::GetLastError();
    if (!bRet)
    {
        CCTRCTXE1(_T("LookupAccountSid() returned FALSE, GetLastError()=%d"), dwLastError);
    }
    else
    {
		CCTRCTXI2(_T("LookupAccountSid() Account name: %s, Domain: %s"), strName, strDomain);
        UINT uReturnBufferSize = dwNameSize + dwDomainSize + 2; // +2 for \ & \0
        *pstrName = new TCHAR[uReturnBufferSize];
        if (pstrName == NULL)
        {
            TRACEE("failed to allocate return buffer\n");
            bRet = false;
        }
        else
        {
            _tcscpy(*pstrName, strDomain);
            _tcscat(*pstrName, "\\");
            _tcscat(*pstrName, strName);
//            TRACEI("returned name = %s\n",*pstrName);
        }
    }

    if (bRet)
    {
        return true;
    }
    else
    {
        return false;
    }
}


// return true if admin, false if not admin or cannot determine
bool IsAdmin (const nsc::IContext* context_in)
{
    bool bAdmin = false; // assume nonadmin

    cc::IString *pstrSid = NULL;
    nsc::NSCRESULT result = context_in->GetUserSID(pstrSid);
	CHECK_NSCRESULT(result);
    if (NSC_FAILED(result))
    {
        TRACEE(_T("Failed to get requesting user sid"));
    }
    else
	{
		LPTSTR strName = NULL;

#ifdef _UNICODE
		CCTRCTXI1(_T("context_in->GetUserSID(): %s"), pstrSid->GetStringW());
		if (!GetNameFromSid (&strName, pstrSid->GetStringW()))
#else // _UNICODE
		CCTRCTXI1(_T("context_in->GetUserSID(): %s"), pstrSid->GetStringA());
		if (!GetNameFromSid (&strName, pstrSid->GetStringA()))
#endif // _UNICODE
        {
            TRACEE(_T("Failed to GetNameFromSid"));
        }
        else
        {
            bAdmin = IsAdminUser (strName);
            delete strName;
            strName = NULL;
        }

        pstrSid->Release();
    }

	CCTRCTXI1(_T("bAdmin=%s"), (bAdmin ? _T("true") : _T("false")));

    return bAdmin;
}
