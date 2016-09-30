////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuarantineMenu.h"

#include "..\NAVLogVRes\resource.h"
#include <ScanUILoader.h>

#include "Message.h"
#include "Provider.h"

CQuarantineMenu::CQuarantineMenu(void)
{
    m_Menu.LoadMenu(IDM_QUARANTINEMENU);
}

CQuarantineMenu::~CQuarantineMenu(void)
{
	m_Menu.DestroyMenu();
}

//****************************************************************************
//****************************************************************************
HRESULT CQuarantineMenu::GetMenuHandle(HMENU &hMenu) throw()   
{
    hMenu = (HMENU)m_Menu.GetSubMenu(0);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CQuarantineMenu::OnMenuCommand(UINT id) throw()
{
    if(ID_QUARANTINEMENU_ADDITEM == id)
    {
        HRESULT hrReturn = E_FAIL, hr;
        GUID guidThreatTrackId = GUID_NULL;

        for(;;)
        {
            avScanUI::sui_IScanUIMisc ScanUIMiscLoader;
            avScanUI::IScanUIMiscPtr spScanUIMisc;
            ScanUIMiscLoader.CreateObject(spScanUIMisc);
            if(!spScanUIMisc)
            {
                CCTRACEE( CCTRCTX _T("Unable to create AvScanUIMisc object."));
                break;
            }

            hr = spScanUIMisc->Initialize();
            if(FAILED(hr))
            {
                CCTRACEE( CCTRCTX _T("IScanUIMisc::Initialize() failed. hr=0x%08X"), hr);
                break;
            }

            hr = spScanUIMisc->QuarantineAdd(GetActiveWindow(), guidThreatTrackId);
            if(FAILED(hr))
            {
                CCTRACEE( CCTRCTX _T("IScanUIMisc::QuarantineAdd() failed. hr=0x%08X"), hr);
                break;
            }
            else if(S_FALSE == hr)
            {
                CCTRACEE( CCTRCTX _T("IScanUIMisc::QuarantineAdd() aborted."));
                break;
            }
            else if(0 == memcmp(&guidThreatTrackId, &GUID_NULL, sizeof(GUID)))
            {
                CCTRACEE( CCTRCTX _T("IScanUIMisc::QuarantineAdd() returned unexpected ThreatTrackId value, GUID_NULL."));
                break;
            }
            
            CreateQuarantineItem(guidThreatTrackId);
            hrReturn = S_OK;
            break;
        }
   }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CQuarantineMenu::OnCloseMenu() throw()
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CQuarantineMenu::SetAppServer(ISymBase* pIApplication) throw()
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
void CQuarantineMenu::SetMessageProviderSink(MCF::IMessageProviderSink* pSink)
{
    m_pMessageProviderSink = pSink;
}

//****************************************************************************
//****************************************************************************
HRESULT CQuarantineMenu::GetTooltipText(UINT nId, cc::IString*& pString) throw()
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
HRESULT CQuarantineMenu::OnStatus(DWORD dwListCnt) throw()
{
	// Nothing to do at this time.
	return S_OK;
}

//****************************************************************************
//****************************************************************************
void CQuarantineMenu::CreateQuarantineItem(REFGUID guidThreatTrackId)
{
    HRESULT hr;
    StahlSoft::HRX hrx;
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

        // Get the single threat
        AVModule::IAVMapDwordDataQIPtr spMapCurThreat;
        hr = spThreatInfo->GetSingleThreat(guidThreatTrackId, spMapCurThreat);
        if(FAILED(hr) || !spMapCurThreat)
        {
            CCTRACEW( CCTRCTX _T("IAVThreatInfo::GetSingleThreat() failed. hr=0x%08X"), hr);
            hrx << E_UNEXPECTED;
        }

        CAvModuleItemMessage* pQItemMessage = NULL;
        MCF::IMessagePtr spMessage;
        hrx << CAvModuleItemMessage::CreateInstance(pQItemMessage);
        hrx << CAvModuleItemMessage::CreateObjectFromInstance(pQItemMessage, spMessage);
        hrx << pQItemMessage->Initialize(MCPROVIDERID_MCAntivirusProviderId, spMapCurThreat);
        m_pMessageProviderSink->OnNewMessage(spMessage);

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

