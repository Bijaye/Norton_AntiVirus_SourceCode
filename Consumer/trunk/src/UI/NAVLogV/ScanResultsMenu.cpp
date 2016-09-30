////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanResultsMenu.h"

#include "..\NAVLogVRes\resource.h"
#include <ScanUILoader.h>

#include "Message.h"
#include "Provider.h"
#include "ISVersion.h"

#include <SRX.h>
#include <HRX.h>
using namespace ccEvtMgr;

bool ValidateUser ();

CScanResultsMenu::CScanResultsMenu(void)
{
    m_Menu.LoadMenu(IDM_SCANRESULTSMENU);
}

CScanResultsMenu::~CScanResultsMenu(void)
{
    m_Menu.DestroyMenu();
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMenu::GetMenuHandle(HMENU &hMenu) throw()   
{
    hMenu = (HMENU)m_Menu.GetSubMenu(0);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMenu::OnMenuCommand(UINT id) throw()
{
    if(ID_SCANRESULTSMENU_CLEARSCANRESULTS == id)
    {
		if(!CAntivirusMessageBase::m_AccessControl.AllowGlobalClearLogs())
        {
			::MessageBox(GetActiveWindow(), _S(IDS_ERR_INSUFFICIENT_RIGHTS), _S(IDS_TITLE_CLEAR_SCANRESULTS), MB_OK);
			return S_OK;
        }
        
        UINT nRet = ::MessageBox(GetActiveWindow(), _S(IDS_CONFIRM_CLEAR_SCANRESULTS), _S(IDS_TITLE_CLEAR_SCANRESULTS), MB_YESNO|MB_ICONWARNING);
		if(IDYES == nRet)
		{
			ClearScanResultsLog();

			if(m_spAppServer)
				m_spAppServer->RefreshCurrentView();
		}
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMenu::OnCloseMenu() throw()
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMenu::SetAppServer(ISymBase* pIApplication) throw()
{
    m_spAppServer = pIApplication;
    return NULL != m_spAppServer.m_p ? S_OK : E_UNEXPECTED;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMenu::GetTooltipText(UINT nId, cc::IString*& pString) throw()
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
//	CScanResultsMenu::OnStatus
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

HRESULT CScanResultsMenu::OnStatus(DWORD dwListCnt) throw()
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
			rc = ::EnableMenuItem(hMenu, ID_SCANRESULTSMENU_CLEARSCANRESULTS, MF_BYCOMMAND | MF_GRAYED);
		else
			rc = ::EnableMenuItem(hMenu, ID_SCANRESULTSMENU_CLEARSCANRESULTS, MF_BYCOMMAND | MF_ENABLED);

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
void CScanResultsMenu::ClearScanResultsLog()
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
        evtmgrError = LogManagerHelper.ClearLog(AV::Event_ID_ScanAction);

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

