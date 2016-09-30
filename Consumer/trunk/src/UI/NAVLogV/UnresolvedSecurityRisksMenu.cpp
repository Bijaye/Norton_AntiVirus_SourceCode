////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UnresolvedSecurityRisksMenu.h"

#include "..\NAVLogVRes\resource.h"
#include <ScanUILoader.h>

#include "Message.h"
#include "Provider.h"

#include <isDataClientLoader.h>
#include <uiNISDataElementGuids.h>

#include <SRX.h>
#include <HRX.h>
using namespace ccEvtMgr;

CUnresolvedSecurityRisksMenu::CUnresolvedSecurityRisksMenu(void)
{
    m_Menu.LoadMenu(IDM_UNRESOLVEDSECURITYRISKSMENU);
}

CUnresolvedSecurityRisksMenu::~CUnresolvedSecurityRisksMenu(void)
{
	m_Menu.DestroyMenu();
}

//****************************************************************************
//****************************************************************************
HRESULT CUnresolvedSecurityRisksMenu::GetMenuHandle(HMENU &hMenu) throw()   
{
    hMenu = (HMENU)m_Menu.GetSubMenu(0);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CUnresolvedSecurityRisksMenu::OnMenuCommand(UINT id) throw()
{
    if(ID_UNRESOLVEDSECURITYRISKSMENU_RUNQUICKSCAN != id)
		return S_FALSE;

	::MessageBox(GetActiveWindow(), _S(IDS_TEXT_CONFIRM_QUICKSCAN), _S(IDS_TITLE_RUN_QUICKSCAN), MB_OK);
	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		SYMRESULT sr;
		ISShared::ISShared_IProvider ProviderLoader;
		ui::IProviderPtr spISElementProvider;
		sr = ProviderLoader.CreateObject(GETMODULEMGR(), &spISElementProvider);
		hrx << MCF::HRESULT_FROM_SYMRESULT(sr);

		ui::IElementPtr pElement;
		hrx << spISElementProvider->GetElement ( ISShared::CLSID_NIS_QuickScan, pElement);
		hrx << pElement->Configure (::GetActiveWindow(), NULL);

		CCTRACEI( CCTRCTX _T("QuickScan Element successfully launched"));
	}
	CCCATCHMEM(exceptionInfo)
	CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
	{
		CCTRACEE( CCTRCTX _T("Unable to launch element. Error: %s"), exceptionInfo.GetDescription());
		::MessageBox(GetActiveWindow(), _S(IDS_ERR_CANNOT_RUN_QUICKSCAN), _S(IDS_TITLE_RUN_QUICKSCAN), MB_OK|MB_ICONSTOP);
	}

	return S_FALSE;
}

//****************************************************************************
//****************************************************************************
HRESULT CUnresolvedSecurityRisksMenu::OnCloseMenu() throw()
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CUnresolvedSecurityRisksMenu::SetAppServer(ISymBase* pIApplication) throw()
{
    m_spAppServer = pIApplication;
    return NULL != m_spAppServer.m_p ? S_OK : E_UNEXPECTED;
}

//****************************************************************************
//****************************************************************************
HRESULT CUnresolvedSecurityRisksMenu::GetTooltipText(UINT nId, cc::IString*& pString) throw()
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


//****************************************************************************
//****************************************************************************
HRESULT CUnresolvedSecurityRisksMenu::OnStatus(DWORD dwListCnt) throw()
{
	// Nothing to do at this time.
	return S_OK;
}
