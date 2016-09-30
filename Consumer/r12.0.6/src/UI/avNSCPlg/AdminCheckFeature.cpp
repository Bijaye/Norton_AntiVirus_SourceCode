#include "StdAfx.h"
#include "AdminCheckFeature.h"
#include "ccSymStringImpl.h"
#include "nscLibCHealth.h"
//#include "Sddl.h"
#include "CNAVActionsCollection.h"
#include "CClientCallback.h"
#include "util.h"


CAdminCheckFeature::CAdminCheckFeature(void)
{
}

CAdminCheckFeature::~CAdminCheckFeature(void)
{
}

nsc::NSCRESULT CAdminCheckFeature::GetHealth(const nsc::IContext* context_in, const nsc::IHealth*& health_out) const throw()
{
	if(m_pRealObject)
	{
		nsc::IFeatureQIPtr pFeature(m_pRealObject);
		if(pFeature)
		{
			// have the feature create the real health
			CCTRCTXI0(_T("called"));

			nsc::NSCRESULT result = pFeature->GetHealth(context_in, health_out);
			if((nsc::NSC_SUCCESS != result) || !health_out)
			{
				CCTRCTXE1(_T("pFeature->GetHealth failed: %d"), result);
				return result;
			}

			if(IsAdmin(context_in))
			{
				return result;
			}

			// create a new health object 
			nscLib::CHealth *pHealth = new nscLib::CHealth();
			if(NULL == pHealth)
			{
				CCTRCTXE0(_T("Could not create CHealth object, returning NSCERR_MEMORY"));
				return nsc::NSCERR_MEMORY;
			}
			pHealth->AddRef();

			// pass the health through
			nsc::e_health health = nsc::POOR;
			health_out->GetHealthLevel(health);
			pHealth->SetHealthLevel(health);

			// pass the notify state through
			bool bNotify = false;
			health_out->GetNotify(bNotify);
			pHealth->SetNotify(bNotify);

			// (for non-admin users, a fix action should only be passed through
			// if it is available for all users)
			nsc::IAction* pAction = NULL;
			result = health_out->GetFixAction(pAction);
			if(NSC_FAILED(result) || pAction == NULL)
			{
				CCTRCTXI0(_T("No Fix Action for health"));
			}
			else
			{
				GUID guidAction = GUID_NULL;
				pAction->GetGUID(guidAction);
				if(IsAllUserAccessAction(guidAction))
				{
					result = pHealth->SetFixAction(pAction);
					pAction->Release();
					if(NSC_FAILED(result))
					{
						CCTRCTXE1(_T("Failed to SetFixAction(), result=%d"), result);
					}
					else
					{
						CCTRCTXI1(_T("SetFixAction() for action=%d"), guidAction);
					}
				}
			}

			nsc::IHealthPtr spHealth;
			spHealth.Attach(const_cast<nsc::IHealth*>(health_out));

			// return the health object
			health_out = pHealth;

			return nsc::NSC_SUCCESS;
		}
		return nsc::NSCERR_NOTIMPLEMENTED;
	}
	return nsc::NSC_DOESNOTEXIST;
}

nsc::NSCRESULT CAdminCheckFeature::GetActions(const nsc::IContext* context_in, const nsc::IActionsCollection*& actions_out) const throw()
{
	nsc::NSCRESULT result = CFeatureContainerImpl::GetActions(context_in, actions_out);
	if(NSC_FAILED(result))
	{
		CCTRCTXE1(_T("Failed CFeatureContainerImpl::GetActions(): %d"), result);
		return result;
	}

	if(IsAdmin(context_in))
	{
		// return the modified actions_out
		return result;
	}

	// Non-admin
	// Check if the actions we fetched are permitted for non-admin users
	result = AllUserAccessActions(actions_out);

	unsigned int iActionCount = 0;
	result = actions_out->GetCount(iActionCount);
	if(NSC_FAILED(result))
	{
		CCTRCTXE1(_T("Failed actions_out->GetCount(): %d"), result);
		return result;
	}

	if(iActionCount > 0)
	{
		// There is at least one non-admin action left, 
		// return the modified actions_out
		CCTRCTXI1(_T("# all user actions = %d, return"), iActionCount);
		return result;
	}

	// There are no non-admin actions. Add our non-admin message action

	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection();
    if(spActions == NULL)
    {
        CCTRCTXE0(_T("Error in CNAVActionsCollection allocation."));
        return nsc::NSCERR_MEMORY;
    }

	result = spActions->AddClientAction(IDS_ACTION_CONFIGURE, IDS_ACTION_CONFIGURE_DESCRIPTION, CClientCallback::eCOOKIE_NON_ADMIN_MESSAGE, guidAction_Non_Admin_Message, CLSID_CAdminCheck);
	if(NSC_FAILED(result))
	{
		CCTRCTXE1(_T("Added non-admin message Action failed: %d"), result);
		return result;
	}
	else
	{
		CCTRCTXI0(_T("Added non-admin message Action"));
	}

	// return the actions collection object
	spActions->AddRef();
	actions_out = spActions;

	return result;
}

nsc::NSCRESULT CAdminCheckFeature::AllUserAccessActions(const nsc::IActionsCollection*& actions_out) const
{
	CNAVActionsCollectionPtr spActions = new CNAVActionsCollection();
    if(spActions == NULL)
    {
        CCTRCTXE0(_T("Error in CNAVActionsCollection allocation."));
        return nsc::NSCERR_MEMORY;
    }

	nsc::IActionPtr pAction;

	actions_out->Reset();
	nsc::NSCRESULT nscResult;
	while(nsc::NSC_SUCCESS == (nscResult = actions_out->GetNext(pAction)))
	{
		GUID guidAction = GUID_NULL;
		pAction->GetGUID(guidAction);
		if(IsAllUserAccessAction(guidAction))
		{
			nscResult = spActions->PushBack(pAction);
			if(NSC_FAILED(nscResult))
			{
				CCTRCTXE2(_T("Failed CNAVActionsCollection::PushBack(%d), nscResult=%d"), guidAction, nscResult);				
			}
			else
			{
				CCTRCTXI1(_T("CNAVActionsCollection::PushBack(%d)"), guidAction);
			}
		}
	}

	if(nsc::NSC_ENDOFCOLLECTION == nscResult)
		nscResult = nsc::NSC_SUCCESS;

	actions_out = spActions;
	actions_out->AddRef();
	return nscResult;
}

bool CAdminCheckFeature::IsAllUserAccessAction(GUID& guidAction) const
{
	return ((guidAction_Launch_FSS == guidAction) ? true : false);
}

nsc::NSCRESULT CAdminCheckFeature::Execute(HWND window_in, DWORD cookie_in) const throw()
{
	CString sTitle;
    CString sMessage;
    sTitle.LoadString(IDS_PRODUCT_NAME_NAV);
    sMessage.LoadString(IDS_ADMIN_RIGHTS);
    MessageBox(NULL, sMessage, sTitle, MB_OK);
	return nsc::NSC_SUCCESS;
}

//nsc::NSCRESULT CAdminCheckFeature::AddClientAction(nscLib::CActionsCollection*& pCollection, CString sActionName, CString sActionDesc, DWORD dwPolicyID, const GUID& guid_in) const throw()
//{
//	CSymPtr<nscLib::CAction> pActionTemp;
//	nsc::NSCRESULT nscres = CreateClientAction(&pActionTemp, sActionName, sActionDesc, dwPolicyID, guid_in);
//	if(NSC_FAILED(nscres))
//	{
//		CCTRCTXE1(_T("CreateClientAction failed (0x%08x)"), nscres);
//		return nscres;
//	}
//
//	// add to collection
//	nscres = pCollection->PushBack(pActionTemp);
//	if(NSC_FAILED(nscres))
//	{
//		CCTRCTXE1(_T("PushBack failed (0x%08x)"), nscres);
//		return nscres;
//	}
//
//	return nscres;
//}

//nsc::NSCRESULT CAdminCheckFeature::CreateAction(nscLib::CAction **ppAction, CString sActionName, CString sActionDesc, const GUID& guid_in) const throw()
//{
//	// check param
//	if((NULL == ppAction) || (NULL != *ppAction))
//	{
//		CCTRCTXE0(_T("invalid parameter"));
//		return nsc::NSCERR_ILLEGALPARAM;
//	}
//
//	// create a new CAction
//	*ppAction = new nscLib::CAction();
//	if(NULL == *ppAction)
//	{
//		CCTRCTXE0(_T("Could not create CAction object, returning NSCERR_MEMORY"));
//		return nsc::NSCERR_MEMORY;
//	}
//	(*ppAction)->AddRef();
//
//	// set the name
//	cc::IStringPtr psName;
//	psName.Attach(ccSym::CStringImpl::CreateStringImpl(sActionName));
//	(*ppAction)->SetName(psName);
//
//	// set the description
//	cc::IStringPtr psDesc;
//	psDesc.Attach(ccSym::CStringImpl::CreateStringImpl(sActionDesc));
//	(*ppAction)->SetDescription(psDesc);
//
//	// set the guid
//	(*ppAction)->SetGUID(guid_in);
//
//	return nsc::NSC_SUCCESS;
//}

//nsc::NSCRESULT CAdminCheckFeature::CreateClientAction(nscLib::CAction **ppAction, CString sActionName, CString sActionDesc, DWORD dwPolicyID, const GUID& guid_in) const throw()
//{
//	// check params
//	if((NULL == ppAction) || (NULL != *ppAction))
//	{
//		CCTRCTXE0(_T("invalid parameter"));
//		return nsc::NSCERR_ILLEGALPARAM;
//	}
//
//	nsc::NSCRESULT nscres = CreateAction(ppAction, sActionName, sActionDesc, guid_in);
//	if(NSC_FAILED(nscres))
//	{
//		CCTRCTXE1(_T("CreateAction failed (0x%08x)"), nscres);
//		return nscres;
//	}
//
//	// get the module path
//	ccLib::CString sPath = g_szModulePath;
//
//	// copy from SYMGUID to GUID
//	GUID guid;
//	memcpy((LPVOID)&guid, &CLSID_CAdminCheck, sizeof(GUID));
//
//	// set client callback
//	nscres = (*ppAction)->SetClientCallback(ccSym::CStringImpl::CreateStringImpl(sPath), guid, dwPolicyID);
//	if(NSC_FAILED(nscres))
//	{
//		CCTRCTXE1(_T("SetClientCallback failed (0x%08x)"), nscres);
//		(*ppAction)->Release();
//		*ppAction = NULL;
//		return nscres;
//	}
//
//	return nscres;
//}

//bool CAdminCheckFeature::IsAdmin(const nsc::IContext* context_in) const throw()
//{
//	cc::IStringPtr psString;
//	context_in->GetUserSID(psString);
//	
//	// convert SID from String to SID
//	PSID psidUser = NULL;
//#ifdef _UNICODE
//	CCTRCTXI1(_T("context_in->GetUserSID(): %s"), psString->GetStringW());
//	if(!ConvertStringSidToSid(psString->GetStringW(), &psidUser))
//#else // _UNICODE
//	CCTRCTXI1(_T("context_in->GetUserSID(): %s"), psString->GetStringA());
//	if(!ConvertStringSidToSid(psString->GetStringA(), &psidUser))
//#endif // _UNICODE
//	{
//		CCTRCTXE1(_T("ConvertStringSidToSid failed (0x%08x)"), GetLastError());
//		return true;
//	}
//
//	CString sName, sDomain;
//	DWORD dwCount = MAX_PATH, dwDomainCount = MAX_PATH;
//	SID_NAME_USE sidUse;
//
//	BOOL bRet = LookupAccountSid(NULL, psidUser, sName.GetBuffer(dwCount), &dwCount, sDomain.GetBuffer(dwDomainCount), &dwDomainCount, &sidUse);
//	DWORD dwLastError = ::GetLastError();
//	sName.ReleaseBuffer();
//	sDomain.ReleaseBuffer();
//	if(!bRet)
//		CCTRCTXE1(_T("LookupAccountSid() returned FALSE, GetLastError()=%d"), dwLastError);
//	else
//		CCTRCTXI2(_T("LookupAccountSid() Account name: %s, Domain: %s"), sName, sDomain);
//
//	bool bAdmin = false;
//	CNetAPI netApi;
//	if(!netApi.Create())
//	{
//		CCTRCTXE1(_T("failed to load netapi32.dll (allowing admin) (0x%08x)"), GetLastError());
//		return true;
//	}
//
//	// code taken from NISUMUtils.cpp (NISUMUtils::GetWindowsUserType)
//	LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
//	DWORD dwEntriesRead = 0;
//	DWORD dwTotalEntries = 0;
//	NET_API_STATUS nStatus;
//
//	WCHAR szwUserName[1024];
//	MultiByteToWideChar(CP_ACP, 0, sName.GetBuffer(), -1, szwUserName, sizeof(szwUserName) / sizeof(WCHAR));
//
//	do
//	{
//		nStatus = netApi.NetUserGetLocalGroups(NULL, szwUserName, 0, 0, (LPBYTE*)&pBuf, -1, &dwEntriesRead, &dwTotalEntries);
//		if(nStatus == NERR_Success || nStatus == ERROR_MORE_DATA)
//		{
//			for(DWORD i = 0; i < dwEntriesRead; ++i)
//			{
//				BYTE abySidBuffer[512];
//				PISID pSid = (PISID) abySidBuffer;
//				BOOL bRet;
//				WCHAR szDomain[512];
//				DWORD dwSidLength = sizeof(abySidBuffer);
//				DWORD dwDomainLength = sizeof(szDomain) / sizeof(*szDomain);
//				SID_NAME_USE tNameUse;
//				bRet = LookupAccountNameW(NULL, pBuf[i].lgrui0_name, pSid, &dwSidLength, szDomain, &dwDomainLength, &tNameUse);
//				if (bRet)
//				{
//					for (DWORD j = 0; j < pSid->SubAuthorityCount; ++j)
//					{
//						if(pSid->SubAuthority[j] == DOMAIN_ALIAS_RID_POWER_USERS)
//							bAdmin = true;
//
//						if(pSid->SubAuthority[j] == DOMAIN_ALIAS_RID_ADMINS)
//							bAdmin = true;
//					}
//				}
//			}
//
//			if(pBuf)
//				netApi.NetApiBufferFree(pBuf);
//		}
//
//	} while(nStatus == ERROR_MORE_DATA);
//
//	CCTRCTXI1(_T("bAdmin=%s"), (bAdmin ? _T("true") : _T("false")));
//	return bAdmin;
//}
//
//CAdminCheckFeature::CNetAPI::CNetAPI() throw()
//{
//	Init(false);
//}
//
//CAdminCheckFeature::CNetAPI::~CNetAPI() throw()
//{
//}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// DLL export map
// //////////////////////
//CCMODULE_BEGIN_PROC_MAP(CAdminCheckFeature::CNetAPI);
//	CCMODULE_PROC_MAP_ENTRY(NetUserGetLocalGroups);
//	CCMODULE_PROC_MAP_ENTRY(NetApiBufferFree);
//CCMODULE_END_PROC_MAP();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
//bool CAdminCheckFeature::CNetAPI::Create() throw()
//{
//	// Destroy
//	Destroy();
//
//	// Load DBGHELP DLL
//	if (Load(_T("NetApi32.DLL")) == false &&
//		IsModule() == false)
//	{
//		CCTRACEE(CCTRCTX _T("Load(_T(\"NetApi32.DLL\")) == false\n"));
//		return false;
//	}
//	return true;
//}
//
//void CAdminCheckFeature::CNetAPI::Destroy() throw()
//{
//	// Unload DLL
//	Free();
//}