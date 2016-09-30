////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SecurityRisksMenu.h"

#include "..\NAVLogVRes\resource.h"
#include <ScanUILoader.h>

#include "Message.h"
#include "Provider.h"

#include <AvInterfaceLoader.h>
#include <QBackupLoader.h>
#include <ccEraserInterface.h>

#include <SRX.h>
#include <HRX.h>
using namespace ccEvtMgr;

CSecurityRisksMenu::CSecurityRisksMenu(void)
{
    m_Menu.LoadMenu(IDM_SECURITYRISKSMENU);
}

CSecurityRisksMenu::~CSecurityRisksMenu(void)
{
	m_Menu.DestroyMenu();
}

//****************************************************************************
//****************************************************************************
HRESULT CSecurityRisksMenu::GetMenuHandle(HMENU &hMenu) throw()   
{
    hMenu = (HMENU)m_Menu.GetSubMenu(0);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSecurityRisksMenu::OnMenuCommand(UINT id) throw()
{
    if(ID_SECURITYRISKSMENU_CLEARSECURITYRISKS == id)
    {
		if(!CAntivirusMessageBase::m_AccessControl.AllowGlobalClearLogs())
		{
			::MessageBox(GetActiveWindow(), _S(IDS_ERR_INSUFFICIENT_RIGHTS), _S(IDS_TITLE_CLEAR_SECURITYRISKS), MB_OK);
			return S_OK;
		}

		UINT nRet = ::MessageBox(GetActiveWindow(), _S(IDS_CONFIRM_CLEAR_SECURITYRISKS), _S(IDS_TITLE_CLEAR_SECURITYRISKS), MB_YESNO|MB_ICONWARNING);
		if(IDYES == nRet)
		{
			ClearSecurityRisksLog();
			ClearAvModuleSecurityRisks();
		
			if(m_spAppServer)
				m_spAppServer->RefreshCurrentView();
		}
	}

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSecurityRisksMenu::OnCloseMenu() throw()
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSecurityRisksMenu::SetAppServer(ISymBase* pIApplication) throw()
{
    m_spAppServer = pIApplication;
    return NULL != m_spAppServer.m_p ? S_OK : E_UNEXPECTED;
}

//****************************************************************************
//****************************************************************************
HRESULT CSecurityRisksMenu::GetTooltipText(UINT nId, cc::IString*& pString) throw()
{
    CString cszToolTipText = _S(nId);
    
    if(cszToolTipText.IsEmpty())
        return E_FAIL;
    
    cc::IStringPtr spToolTipText;
    spToolTipText.Attach(ccSym::CStringImpl::CreateStringImpl());
    if(!spToolTipText)
        return E_FAIL;

    spToolTipText->SetStringW(cszToolTipText);
    
    SYMRESULT sr = spToolTipText->QueryInterface(cc::IID_String, (void**)&pString);
    return MCF::HRESULT_FROM_SYMRESULT(sr);
}

// ==============================================
//	CSecurityRisksMenu::OnStatus
//	-----------------------------------
// 
// Description:
//		Handle a OnStatus from the application. Only status at this time is the number of items in the list.
//		Change the menu item to be disabled if dwListCnt == 0. Otherwise make sure it is enabled.
//		Call IApplication::UpdateMenu() when ready.
// In:
//		DWORD dwListCnt	= Number of items in the current lis.
// Out:
//		HRESULT			= Errorvalue or S_OK if no error

HRESULT CSecurityRisksMenu::OnStatus(DWORD dwListCnt) throw()
{
    HRESULT hr = S_OK;
	CCTRCTXI1(_T("dwListCnt=%ld"), dwListCnt);

	if (m_spAppServer != NULL)
	{
		HMENU hMenu;
		if(m_Menu.m_hMenu == NULL)
		{
			CCTRCTXE0(_T("m_Menu == NULL"));
			return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
		}
		hMenu = (HMENU)m_Menu.GetSubMenu(0);
		if(hMenu == NULL)
		{
			CCTRCTXE0(_T("hMenu == NULL"));
			return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
		}

		UINT rc;
		if (dwListCnt == 0)
			rc = ::EnableMenuItem(hMenu, ID_SECURITYRISKSMENU_CLEARSECURITYRISKS, MF_BYCOMMAND | MF_GRAYED);
		else
			rc = ::EnableMenuItem(hMenu, ID_SECURITYRISKSMENU_CLEARSECURITYRISKS, MF_BYCOMMAND | MF_ENABLED);

		if (rc != -1)
			m_spAppServer->UpdateMenu(hMenu);
		else
		{
			hr = MAKE_HRESULT(SEVERITY_ERROR, 0, E_FAIL);
			CCTRCTXE0(_T("unable to enable menu item:%08x"));
		}
	}
	else
	{
		CCTRCTXE0(_T("m_spAppServer == NULL"));
		hr = E_NOINTERFACE;
	}

	return hr;
}


//****************************************************************************
//****************************************************************************
void CSecurityRisksMenu::ClearSecurityRisksLog()
{
    StahlSoft::HRX hrx; SRX srx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        // Pointer to event manager object
        ccEvtMgr::CEventFactoryEx2Ptr pEventFactory;
        ccEvtMgr::CProxyFactoryExPtr pProxyFactory;

        CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> EventManager;
        ccEvtMgr::CLogManagerHelper LogManagerHelper;

        // Load the factory and proxy DLLs
        AV::NAVEventFactory_CEventFactoryEx2 AvEventFactoryLoader;
        AV::NAVEventFactory_CProxyFactoryEx AvProxyFactoryLoader;
        srx << AvEventFactoryLoader.CreateObject(GETMODULEMGR(), &pEventFactory);
        srx << AvProxyFactoryLoader.CreateObject(GETMODULEMGR(), &pProxyFactory);
        if(!pEventFactory || !pProxyFactory)
            throw _com_error(E_UNEXPECTED);

        // Create the Event Manager Helper object
        if(FALSE == EventManager.Create(pEventFactory, pProxyFactory))
            throw _com_error(E_UNEXPECTED);

        // Create Log Helper object
        if (LogManagerHelper.Create(pEventFactory) == FALSE)
            throw _com_error(E_UNEXPECTED);

        CError::ErrorType evtmgrError = CError::eUnknownError;
        evtmgrError = LogManagerHelper.ClearLog(AV::Event_ID_Threat);

        LogManagerHelper.Destroy();

        // Delete the factories in reverse order before the loaders
        if ( pProxyFactory )
        {
            pProxyFactory.Release ();
        }

        if ( pEventFactory )
        {
            pEventFactory.Release ();
        }

        EventManager.Destroy ();
        
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

//****************************************************************************
//****************************************************************************
void CSecurityRisksMenu::ClearAvModuleSecurityRisks()
{
	StahlSoft::HRX hrx; SRX srx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
		AVModule::IAVThreatInfoPtr spThreatInfo;
		AvThreatInfoLoader.CreateObject(spThreatInfo);
		if(!spThreatInfo)
		{
			CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
			hrx << E_UNEXPECTED;
		}

		AVModule::AVLoader_IAVMapDwordData AVMapDwordDataLoader;
		AVModule::IAVMapDwordDataPtr spFilter;
		AVMapDwordDataLoader.CreateObject(spFilter);
		if(!spFilter)
		{
			CCTRACEW( CCTRCTX _T("AVLoader_IAVMapDwordData::CreateObject() failed."));
			hrx << E_UNEXPECTED;
		}

		// Get all threats
		AVModule::IAVArrayDataPtr spThreatData;
		hrx << spThreatInfo->GetThreatsWithFilter(NULL, spThreatData);

		DWORD dwThreatInfoCount = NULL;
		hrx << spThreatData->GetCount(dwThreatInfoCount);

		for(DWORD dwThreatInfoIndex = 0; dwThreatInfoIndex < dwThreatInfoCount; dwThreatInfoIndex++)
		{
			HRESULT hr;
			ISymBasePtr spTempData;
			hr = spThreatData->GetValue(dwThreatInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
			if(FAILED(hr) || !spTempData)
				continue;

			AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
			if(!spMapCurThreat)
				continue;

			GUID guidThreatTrackId;
			hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, guidThreatTrackId);
			if(FAILED(hr))
				continue;

			DWORD dwThreatTypeId = NULL;
			hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, dwThreatTypeId);
			if(FAILED(hr))
				continue;
			
			if(AVModule::ThreatTracking::ThreatType_OEH == dwThreatTypeId)
				continue; // Ignore OEH

			if(!ThreatHasBeenResolved(spMapCurThreat))
				continue; // only resolved threats

			hr = spThreatInfo->RemoveThreat(guidThreatTrackId);
			if(FAILED(hr))
			{
				CCTRACEW( CCTRCTX _T("Problem deleting threat from ThreatTrackInfo. hr=0x%08X, dwThreatInfoIndex=%d"), hr, dwThreatInfoIndex);
				continue;
			}
		}
	}
	CCCATCHMEM(exceptionInfo)
	CCCATCHCOM(exceptionInfo);

	return;
}

//****************************************************************************
//****************************************************************************
bool CSecurityRisksMenu::ThreatHasBeenResolved(AVModule::IAVMapDwordData* pThreatTrackInfo)
{
	if(!pThreatTrackInfo)
		return false; // should throw exception?

	DWORD dwThreatStateId = NULL, dwThreatTypeId = NULL;
	pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwThreatStateId);
	pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, dwThreatTypeId);

	// This logic is essentially mirrored in NAVLOGV/Message.cpp
	//  TODO: Should make a shared class so this doesn't get out of date...
	// 

	// Get threat resolved
	bool bThreatHasBeenResolved = false;
	if(  AVModule::ThreatTracking::ThreatRemoved(dwThreatStateId) 
		|| AVModule::ThreatTracking::ThreatState_BackupOnly == dwThreatStateId 
		|| AVModule::ThreatTracking::ThreatType_Email == dwThreatTypeId )
	{
		bThreatHasBeenResolved = true;
	}

	bool bManualRemovalRequired = false;
	bManualRemovalRequired = !bThreatHasBeenResolved && (AVModule::ThreatTracking::ThreatState_DoNotDelete == dwThreatStateId);

	HRESULT hr;
	ISymBasePtr spTempObj;

	bool bHasDependencyFlag = false;
	hr = pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVModule::AVDataTypes::eTypeMapDword, spTempObj);
	if(SUCCEEDED(hr) && spTempObj)
	{
		AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempObj;
		if(spTempObj)
		{
			DWORD dwDependencyFlag = (DWORD)-1;
			spMapCurThreat->GetValue(ccEraser::IAnomaly::Dependency, dwDependencyFlag);
			bHasDependencyFlag = dwDependencyFlag != 0 && dwDependencyFlag != (DWORD)-1;
		}
	}

    DWORD dwRequiresReboot = NULL, dwRequiresProcTerm = NULL;
    pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_REBOOTREQUIRED, dwRequiresReboot);
    pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_PROCTERMREQUIRED, dwRequiresProcTerm);

    bool bAttentionRequired =   0 != dwRequiresReboot 
        || 0 != dwRequiresProcTerm
        || bManualRemovalRequired
        || bHasDependencyFlag;

    bool bThreatHasBeenIgnored = false;
    if( !bThreatHasBeenResolved 
        && (AVModule::ThreatTracking::ThreatState_RemoveNotAttempted  == dwThreatStateId) 
        && !bAttentionRequired )
    {
        bThreatHasBeenIgnored = true;
    }

	return bThreatHasBeenResolved || bThreatHasBeenIgnored;
}

