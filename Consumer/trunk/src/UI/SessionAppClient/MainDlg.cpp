////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

#include "srx.h"

#include "ccInstanceFactory.h"
#include "SessionAppClientImpl.h"
#include "MainDlg.h"
#include ".\maindlg.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);
    
    GetTrayIconState();
    
    SetDlgItemText(IDC_EDT_NOTIFYTITLE, _T("Session App Notify Test"));
    SetDlgItemText(IDC_EDT_NOTIFYTEXT, _T("Consider yourself notified!!"));
    m_iNotifyDuration = 5000;

    DoDataExchange(FALSE);

	return TRUE;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoDataExchange(TRUE);
    
    INT iAppState = m_iAppState;

    // TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void CMainDlg::GetTrayIconState()
{
    StahlSoft::HRX hrx; SRX srx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");

        DWORD dwAppState = SessionApp::AppState::eInvalid;
        hrx << spClient->GetCurrentAppState(dwAppState);

        DWORD dwDuration = NULL;
        cc::IStringPtr spToolTipText;
        DWORD dwNotifyState = SessionApp::NotifyState::eInvalid;
        hrx << spClient->GetCurrentNotifyState(dwNotifyState, &dwDuration, &spToolTipText);
        

        m_iAppState = dwAppState - (SessionApp::AppState::eFirst + 1);
        if(SessionApp::NotifyState::eInvalid == dwNotifyState)
            m_iNotifyState = 0;
        else
            m_iNotifyState = dwNotifyState - (SessionApp::NotifyState::eFirst + 1);
        m_iDuration = dwDuration;

        SetDlgItemText(IDC_EDT_TOOLTIPTEXT, CW2T(spToolTipText->GetStringW()));
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

void CMainDlg::SetTrayIconState()
{
    StahlSoft::HRX hrx; SRX srx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        DoDataExchange(TRUE);
        DWORD dwAppState = m_iAppState + SessionApp::AppState::eFirst + 1;
        DWORD dwNotifyState = 0 == m_iNotifyState ? SessionApp::NotifyState::eInvalid : m_iNotifyState + SessionApp::NotifyState::eFirst + 1;
        DWORD dwDuration = m_iDuration;
        
        CString cszToolTipText;
        GetDlgItemText(IDC_EDT_TOOLTIPTEXT, cszToolTipText);

        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");
        hrx << spClient->SetCurrentAppState(dwAppState);
        hrx << spClient->SetCurrentNotifyState(dwNotifyState, dwDuration, cszToolTipText);
    
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

LRESULT CMainDlg::OnBnClickedApply(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    SetTrayIconState();

    return 0;
}

LRESULT CMainDlg::OnBnClickedApply2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    GetTrayIconState();
    DoDataExchange(FALSE);

    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnApplyNotify(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

    SendNotifyMessage();

    return 0;
}

void CMainDlg::SendNotifyMessage()
{
    StahlSoft::HRX hrx; SRX srx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CString cszNotifyTitle, cszNotifyText;
        GetDlgItemText(IDC_EDT_NOTIFYTITLE, cszNotifyTitle);
        GetDlgItemText(IDC_EDT_NOTIFYTEXT, cszNotifyText);

        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");
        
        cc::INotifyPtr spNotify;
        hrx << spClient->CreateNotification(spNotify);
        
        spNotify->SetTitle(cszNotifyTitle);
        spNotify->SetText(cszNotifyText);
        spNotify->SetTimeout(m_iNotifyDuration);

        spNotify->Display(NULL);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}
