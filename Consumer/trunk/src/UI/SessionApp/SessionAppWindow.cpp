////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SessionAppWindow.h"
#include "SessionAppServerImpl.h"

#include <ccEventManagerHelper.h>
#include <AvProdSvcCommIds.h>

#include "SessionAppClientImpl.h"

using namespace SessionApp;

const LPCTSTR SessionApp::CAppWindow::SZ_AppWND_APPWINDOWNAME = _T("_SessionApp_AppWnd_Class_");

const UINT SessionApp::RM_TRAY                       = ::RegisterWindowMessage(_T("{15918834-B104-4c50-AFCE-9C73E8E6D8EB}"));
const UINT SessionApp::RM_TASKBARCREATED             = ::RegisterWindowMessage(_T("TaskbarCreated"));
const UINT SessionApp::RM_SHOWTRAYICON               = ::RegisterWindowMessage(_T("_ShowTrayIcon_"));
const UINT SessionApp::RM_EVTMGRISDOWN               = ::RegisterWindowMessage(_T("_RM_EVTMGRISDOWN_"));
const UINT SessionApp::RM_DOSTATECHANGE              = ::RegisterWindowMessage(_T("_RM_DOSTATECHANGE_"));

const UINT SessionApp::RM_SHOWTRAYAPP                = ::RegisterWindowMessage(_T("_RM_SHOWTRAYAPP_"));
const UINT SessionApp::RM_HIDETRAYAPP                = ::RegisterWindowMessage(_T("_RM_HIDETRAYAPP_"));
const UINT SessionApp::RM_SETTRAYICON                = ::RegisterWindowMessage(_T("_RM_SETTRAYICON_"));
const UINT SessionApp::RM_SETTOOLTIPTEXT             = ::RegisterWindowMessage(_T("_RM_SETTOOLTIPTEXT_"));
const UINT SessionApp::RM_CHANGESTATE                = ::RegisterWindowMessage(_T("_RM_CHANGESTATE_"));
const UINT SessionApp::RM_GETTRAYICONSTATE           = ::RegisterWindowMessage(_T("_RM_GETTRAYICONSTATE_"));

const UINT SessionApp::TIMERID_CCEVTCHK  = 0x03;
const UINT SessionApp::TIMERID_FLASHICON = 0x04;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAppWindow::CAppWindow(void) : 
m_pSessionAppServer(NULL),
m_hrExitCode(E_FAIL), 
m_dwCurrentAppState(NotifyState::eInvalid),
m_dwCurrentNotifyState(AppState::eUnknown), 
m_dwFlashIconTimeout(NULL),
m_iDefaultMenuItem(NULL)
{
    HINSTANCE hRes = NULL;
    UINT nIconId = NULL;
    
}

CAppWindow::~CAppWindow(void)
{
    KillTimer(SessionApp::TIMERID_CCEVTCHK);
    RemoveIcon();
    
    MAP_DWORD_TO_HICON::iterator itIcon;
    for(itIcon = m_mapIconMap.begin(); itIcon != m_mapIconMap.end(); itIcon++)
    {
        HICON hIcon = (*itIcon).second;
        DeleteObject(hIcon);
    }

    m_mapIconMap.clear();
}

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::ShowTrayApp(DWORD dwState)
{
    //  Call using the message pump to 
    //    ensure synchronous access to members
    return 0 == SendMessage(RM_SHOWTRAYAPP, (WPARAM)dwState);
};

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnShowTrayApp( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    DWORD dwState = (DWORD)wParam;
    
    if(dwState > SessionApp::AppState::eFirst && dwState < SessionApp::AppState::eLast)
        m_dwCurrentAppState = dwState;

    // Set up the Tray Icon
    if(!m_bInstalled)
        InstallIcon(m_mapToolTipText[m_dwCurrentAppState], m_mapIconMap[m_dwCurrentAppState]);
    else
        ChangeState(m_dwCurrentAppState, NULL, m_mapToolTipText[m_dwCurrentAppState]);

    return 0;
};


//****************************************************************************
//****************************************************************************
BOOL CAppWindow::HideTrayApp()
{
    //  Call using the message pump to 
    //    ensure synchronous access to members
    return 0 == SendMessage(RM_HIDETRAYAPP);
};

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnHideTrayApp( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    KillTimer(SessionApp::TIMERID_CCEVTCHK);
    RemoveIcon();
    return 0;
};

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::SetIcon(DWORD dwIconId, HMODULE hResModule, UINT iResIconId)
{
    // Put data into struct and SendMessage() to
    //  ensure synchronous access to members

    SETICON_DATA SetIconData;
    SetIconData.dwIconId = dwIconId;
    SetIconData.hResModule = hResModule;
    SetIconData.iResIconId = iResIconId;

    return 0 == SendMessage(RM_SETTRAYICON, (WPARAM)&SetIconData);
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnSetIcon( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    CCTRCTXI0(_T("Changing state."));
    if(NULL == wParam)
    {
        CCTRACEE( CCTRCTX _T("Invalid data parameter."));
        return -1;
    }
    
    LPSETICON_DATA pData = (LPSETICON_DATA)wParam;

    HICON hIconOld = m_mapIconMap[pData->dwIconId];
    HICON hIconNew = (HICON)LoadImage(pData->hResModule, MAKEINTRESOURCE(pData->iResIconId), IMAGE_ICON, 16, 16, 0);

    if(NULL == hIconNew)
        return FALSE;

    m_mapIconMap[pData->dwIconId] = hIconNew;

    if(NULL == hIconOld && m_dwCurrentNotifyState != pData->dwIconId)
        return TRUE;

    ChangeIcon(hIconNew);
    DeleteObject(hIconOld);

    return TRUE;

}

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::SetTooltipText(DWORD dwState, HMODULE hModule, UINT iToolTipTextId)
{
    CString cszToolTipText;
    ::LoadString(hModule, iToolTipTextId, cszToolTipText.GetBuffer(512), 511);
    cszToolTipText.ReleaseBuffer();

    if(cszToolTipText.IsEmpty())
        return FALSE;

    return SetTooltipText(dwState, cszToolTipText);
}

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::SetTooltipText(DWORD dwState, LPCWSTR wszToolTipText)
{
    // Put data into struct and SendMessage() to
    //  ensure synchronous access to members

    STATESTRING_DATA StateStringData;
    StateStringData.dwState = dwState;
    StateStringData.wszText = wszToolTipText;

    return 0 == SendMessage(RM_SETTOOLTIPTEXT, (WPARAM)&StateStringData);
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnSetTooltipText( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    CCTRCTXI0(_T("Changing state."));
    if(NULL == wParam)
    {
        CCTRACEE( CCTRCTX _T("Invalid data parameter."));
        return -1;
    }

    LPSTATESTRING_DATA pData = (LPSTATESTRING_DATA)wParam;
    
    BOOL bRet = FALSE;
    DWORD dwState = pData->dwState;
    
    if( (dwState > AppState::eFirst && dwState < AppState::eLast) ||
        (dwState > NotifyState::eFirst && dwState < NotifyState::eLast) )
    {
        m_mapToolTipText[dwState] = CW2T(pData->wszText);
        bRet = TRUE;
    }

    return bRet ? 0 : -1;
}

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::PreTranslateMessage(MSG* pMsg)
{
    return FALSE;//IsDialogMessage(pMsg);
}

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::OnIdle()
{
    return FALSE;
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnCreate( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    SetExitCode(S_OK);
    return 0;
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnQueryEndSession( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    PostMessage(WM_CLOSE);
    bHandled = FALSE;
    return TRUE;
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnDestroy( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    RemoveIcon();
    PostQuitMessage(NULL);
    return 0;
}

//****************************************************************************
//****************************************************************************
void CAppWindow::OnFinalMessage( HWND hWnd )
{

}

//****************************************************************************
//****************************************************************************
void CAppWindow::PrepareMenu(HMENU &hMenu)
{
    if(m_pSessionAppServer)
    {
        m_pSessionAppServer->OnPrepareMenu(hMenu);
    }
}

//****************************************************************************
//****************************************************************************
void CAppWindow::OnTrayItemClick()
{
}

//****************************************************************************
//****************************************************************************
void CAppWindow::OnTrayItemDblClick()
{
    if(m_pSessionAppServer)
    {
        m_pSessionAppServer->OnTrayItemDblClick();
    }
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnMenuCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    bHandled = FALSE;
    if(m_pSessionAppServer)
        m_pSessionAppServer->OnMenuCommand(wID);

    return 0;
};


//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnTaskbarCreated( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    CCTRACEI(_T("CAppWindow::OnTaskbarCreated - re-creating TrayIcon"));
    InstallIcon(m_mapToolTipText[m_dwCurrentAppState], m_mapIconMap[m_dwCurrentAppState]);

    return 0;
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnEvtMgrIsDown( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    CCTRACEI(_T("CAppWindow::OnEvtMgrIsDown - starting monitor for ccEventManager status."));
    SetTimer(SessionApp::TIMERID_CCEVTCHK, 5000);

    return 0;
}

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::GetCurrentIconState(DWORD& dwAppState, DWORD& dwNotifyState, DWORD& dwDuration, CString& cszToolTipText)
{
    // Put data into struct and SendMessage() to
    //  ensure synchronous access to members

    GETTRAYICONSTATE_DATA IconStateData;
    IconStateData.pdwAppState = &dwAppState;
    IconStateData.pdwNotifyState = &dwNotifyState;
    IconStateData.pdwDuration = &dwDuration;
    IconStateData.pcszText = &cszToolTipText;

    return 0 == SendMessage(RM_GETTRAYICONSTATE, (WPARAM)&IconStateData);
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnGetCurrentIconState( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    CCTRCTXI0(_T("Changing state."));
    if(NULL == wParam)
    {
        CCTRACEE( CCTRCTX _T("Invalid data parameter."));
        return -1;
    }
    
    LPGETTRAYICONSTATE_DATA pData = (LPGETTRAYICONSTATE_DATA)wParam;
    *(pData->pdwAppState) = m_dwCurrentAppState;
    *(pData->pdwNotifyState) = m_dwCurrentNotifyState;
    
    if(NotifyState::eInvalid != m_dwCurrentAppState)
    {
        *(pData->pdwDuration) = (-1 == m_dwFlashIconTimeout) ? -1 : m_dwFlashIconTimeout - GetTickCount();
        *(pData->pcszText) = m_cszTempToolTipText;

    }
    else
    {
        *(pData->pdwDuration) = NULL;
        *(pData->pcszText) = m_mapToolTipText[m_dwCurrentAppState];
    }
    
    return 0;
};

//****************************************************************************
//****************************************************************************
BOOL CAppWindow::ChangeState(DWORD dwState, DWORD dwDuration, LPCWSTR wszToolTipText)
{
    // Put data into struct and SendMessage() to
    //  ensure synchronous access to members

    STATECHANGE_DATA StateChangeData;
    StateChangeData.dwState = dwState;
    StateChangeData.dwDuration = dwDuration;
    StateChangeData.wszText = wszToolTipText;

    return 0 == SendMessage(RM_CHANGESTATE, (WPARAM)&StateChangeData);
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnChangeState( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    CCTRCTXI0(_T("Changing state."));
    if(NULL == wParam)
    {
        CCTRACEE( CCTRCTX _T("Invalid data parameter."));
        return -1;
    }
    
    STATECHANGE_DATA *pData = (LPSTATECHANGE_DATA)wParam;
    
    if(pData->dwState > AppState::eFirst && pData->dwState < AppState::eLast)
    {
        m_dwCurrentNotifyState = NotifyState::eInvalid;
        m_dwCurrentAppState = pData->dwState;
        SetFlashingState(m_dwCurrentAppState, 0, pData->wszText);
    }
    else if(pData->dwState > NotifyState::eFirst && pData->dwState < NotifyState::eLast)
    {
        m_dwCurrentNotifyState = pData->dwState;
        SetFlashingState(m_dwCurrentNotifyState, pData->dwDuration, pData->wszText);
    }
    else if(NotifyState::eInvalid == pData->dwState)
    {
        m_dwCurrentNotifyState = pData->dwState;
        SetFlashingState(m_dwCurrentNotifyState, NULL, NULL);
    }

    return 0;
}


//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnDoStateChange( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    SetTimer(SessionApp::TIMERID_FLASHICON, 500, NULL);
    return 0;
}

//****************************************************************************
//****************************************************************************
LRESULT CAppWindow::OnTimer( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    switch(wParam)
    {
        case SessionApp::TIMERID_CCEVTCHK:
            OnTimerCheckEventManagerStatus();
            break;

        case SessionApp::TIMERID_FLASHICON:
            OnTimerFlashIcon();
            break;

        default:
            break;
    }

    return 0;
}

//****************************************************************************
//****************************************************************************
void CAppWindow::OnTimerCheckEventManagerStatus()
{
    CCTRACEI(_T("CAppWindow::OnTimerCheckEventManagerStatus - enter."));

    if(ccEvtMgr::CEventManagerHelper::IsEventManagerActive())
    {
        CCTRACEI(_T("CAppWindow::OnTimerCheckEventManagerStatus - ccEvtMgr appears to be up."));

        KillTimer(SessionApp::TIMERID_CCEVTCHK);
        
#pragma message(_T("TODO: Add OnEvtMgrIsUp callback."))          
    }

    return;
}

//****************************************************************************
//****************************************************************************
void CAppWindow::OnTimerFlashIcon()
{
    if(GetTickCount() < m_dwFlashIconTimeout && m_dwCurrentNotifyState != NotifyState::eInvalid)
    {
        if(m_dwFlashState >= Flash::eEnd)
            m_dwFlashState = Flash::eBegin;
        
        m_dwFlashState++;
    }
    else
    {
        KillTimer(SessionApp::TIMERID_FLASHICON);
        m_dwFlashState = Flash::eAppState;
        m_dwCurrentNotifyState = NotifyState::eInvalid;
		m_cszTempToolTipText = m_mapToolTipText[m_dwCurrentAppState];
		CTrayIconImpl<CAppWindow>::SetTooltipText(m_cszTempToolTipText);
    }
    
    // Set up the Tray Icon
    if(Flash::eAppState == m_dwFlashState)
        ChangeIcon(m_mapIconMap[m_dwCurrentAppState]);
    else if(Flash::eAppNotify == m_dwFlashState)
        ChangeIcon(m_mapIconMap[m_dwCurrentNotifyState]);

}

void CAppWindow::SetFlashingState(DWORD dwState, DWORD dwTimeout, LPCWSTR wszToolTipText)
{
    m_dwFlashState = Flash::eBegin;
    if(-1 == dwTimeout)
        m_dwFlashIconTimeout = -1;
    else
        m_dwFlashIconTimeout = GetTickCount() + dwTimeout;

    if(wszToolTipText)
        m_cszTempToolTipText = wszToolTipText;
    else
        m_cszTempToolTipText = m_mapToolTipText[dwState];
    
    CTrayIconImpl<CAppWindow>::SetTooltipText(m_cszTempToolTipText);
    PostMessage(SessionApp::RM_DOSTATECHANGE, NULL, NULL);
}

