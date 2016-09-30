// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "HPPEventsInterface.h"
#include "HPPEventHelper.h"

#include "HPPUtilityLoader.h"

#include "aboutdlg.h"
#include "RegKeyMonitor.h"
#include "MainDlg.h"

#include "WindowsSecurityAccount.h"
#include ".\maindlg.h"

const UINT TIMERID_FINALINIT = 101;

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
    
    m_spEvtHelper = new CHPPEventHelper;
    m_spEvtHelper->AddRef();
    
//    m_regKeyMonitorHKLM.Initialize(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Internet Explorer\\main"), _T("Start Page"));
//    m_regKeyMonitorHKCU.Initialize(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\main"), _T("Start Page"));
    
    
    CString cszCurrentPath;
    size_t nSize = MAX_PATH;
    ccSym::CProcessPathProvider::GetPath(cszCurrentPath.GetBuffer(nSize), nSize);
    cszCurrentPath.ReleaseBuffer();
    m_EventLog.Initialize(cszCurrentPath, _T("PendingAlertsQueue.log"));
    
    // Set up hyperlinks ******************************************************
    m_wndHyperLink.SubclassWindow(GetDlgItem(IDC_TXT_LINK));
//    m_wndHyperLink.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);

//    m_wndHyperLink.SetToolTipText(_T("http://www.symantec.com"));
    ::EnableWindow(m_wndHyperLink, FALSE);
    
    // Set up status bar ******************************************************
    m_wndStatusBar.Create(m_hWnd, _T("Status Bar"));

    // set status bar panes. ID_DEFAULT_PANE is defined by WTL
    int arrPanes[] = { ID_DEFAULT_PANE };
    int arrWidths[] = { 0 };
    
    CRect rcClient;
    GetClientRect(&rcClient);

    arrWidths[0] = rcClient.Width();

    m_wndStatusBar.SetPanes(arrPanes, 
        sizeof(arrPanes) / sizeof(int), false);

    // set status bar pane widths using local workaround
    SetPaneWidths(arrWidths, sizeof(arrWidths) / sizeof(int));

    // set the status bar pane icon to unknown
    m_wndStatusBar.SetPaneIcon(ID_DEFAULT_PANE, 
        AtlLoadIconImage(IDI_BBICON_UKNOWN, LR_DEFAULTCOLOR));
    
    m_wndStatusBar.SetPaneText(ID_DEFAULT_PANE, _T("BBSysMon status: initializing..."));

    SetTimer(TIMERID_FINALINIT, 1000, NULL);

    return TRUE;
}

LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    switch(wParam)
    {
        case TIMERID_FINALINIT:
            KillTimer(TIMERID_FINALINIT);
            OnFinalInitDialog();
            break;
    }
    
    return 0;
}

void CMainDlg::OnFinalInitDialog()
{
    // Set up SysMon rules ****************************************************
    bool bEnabled = false, bError = false, bRet = false;

    bRet = m_spEvtHelper->GetStatus(bEnabled, bError);
    if(bRet && bEnabled && !bError)
    {
         // set the status bar pane icon to ok
        m_wndStatusBar.SetPaneIcon(ID_DEFAULT_PANE, 
            AtlLoadIconImage(IDI_BBICON_OK, LR_DEFAULTCOLOR));

       m_wndStatusBar.SetPaneText(ID_DEFAULT_PANE, _T("BBSysMon status: On"));

        m_spEvtHelper->RegisterHPPClient();
        m_spEvtHelper->CreateHPPRules(HPP::HPP_ACTION_BLOCK, HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS);

    }
    else if(bRet && !bError && !bEnabled)
    {
        // set the status bar pane icon to error
        m_wndStatusBar.SetPaneIcon(ID_DEFAULT_PANE, 
            AtlLoadIconImage(IDI_BBICON_ERROR, LR_DEFAULTCOLOR));

        m_wndStatusBar.SetPaneText(ID_DEFAULT_PANE, _T("BBSysMon status: Off"));
    }
    else
    {
        // set the status bar pane icon to error
        m_wndStatusBar.SetPaneIcon(ID_DEFAULT_PANE, 
            AtlLoadIconImage(IDI_BBICON_ERROR, LR_DEFAULTCOLOR));

        CString cszPaneText;
        cszPaneText.Format(_T("BBSysMon status: Unknown - GetStatus() return %s, bEnabled: %s, bError: %s"), 
                            bRet ? _T("true") : _T("false"), 
                            bEnabled ? _T("true") : _T("false"), 
                            bError ? _T("true") : _T("false"));

        m_wndStatusBar.SetPaneText(ID_DEFAULT_PANE, cszPaneText);
    }

}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    bool bEnabled = false, bError = false, bRet = false;
    bRet = m_spEvtHelper->GetStatus(bEnabled, bError);
    if(bRet && bEnabled && !bError)
    {
        m_spEvtHelper->UnRegisterHPPClient();
    }
    
    return 0;
}

void CMainDlg::SetPaneWidths(int* arrWidths, int nPanes)
{ 
    // find the size of the borders
    int arrBorders[3];
    m_wndStatusBar.GetBorders(arrBorders);

    // calculate right edge of default pane (0)
    arrWidths[0] += arrBorders[2];
    for (int i = 1; i < nPanes; i++)
        arrWidths[0] += arrWidths[i];

    // calculate right edge of remaining panes (1 thru nPanes-1)
    for (int j = 1; j < nPanes; j++)
        arrWidths[j] += arrBorders[2] + arrWidths[j - 1];

    // set the pane widths
    m_wndStatusBar.SetParts(m_wndStatusBar.m_nPanes, arrWidths); 
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
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

LRESULT CMainDlg::OnBnClickedBtnSendEvent1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    
    ccEvtMgr::CEventEx *pEvent;

    SYMRESULT sr = m_spEvtHelper->NewHPPEvent(HPP::Event_ID_HPPNotifyHomePage, &pEvent);
    if(SYM_FAILED(sr))
        return 0;
    
    CHPPEventCommonInterfaceQIPtr spCommonEvent = pEvent;
    if(!spCommonEvent)
        return 0;
    
    CWindowsSecurityAccount cSecurityInfo;

    CString cszValue;
    DWORD lValue;
    BOOL bRet;
    HRESULT hr;

    // Get current user name
    cszValue.Empty();
    hr = cSecurityInfo.GetUserName(cszValue);
    if(SUCCEEDED(hr))
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propUserName, cbszValue);
    }

    // Get current user SID
    cszValue.Empty();
    hr = cSecurityInfo.GetCurrentUserSID(cszValue);
    if(SUCCEEDED(hr))
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propUserSID, cbszValue);
    }

    // Get current session id
    lValue = 0;
    hr = cSecurityInfo.GetSessionId(lValue);
    if(SUCCEEDED(hr))
    {
        spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propSessionID, lValue);
    }

    // Set current process id
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propProcessID, GetCurrentProcessId());

    // Set current process path and name
    bRet = GetModuleFileName(NULL, cszValue.GetBuffer(MAX_PATH), MAX_PATH);
    cszValue.ReleaseBuffer();
    if(bRet)
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessPath, cbszValue);

        PathStripPath(cszValue.GetBuffer(MAX_PATH));
        cszValue.ReleaseBuffer();

        cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessName, cbszValue);
    }
    
    _bstr_t cbszHomePageStartValue = L"http://www.msn.com";
    _bstr_t cbszHomePageActionValue = L"http://www.coolwebsearch.com/ads.asp?username={33CC6709-C22D-4c0f-AD8A-0F43A23C2E7B}";

    spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propStartValue, cbszHomePageStartValue);
    spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propActionValue, cbszHomePageActionValue);
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, HPP::HPP_ACTION_BLOCK);

    long lEventId = 0;
    if(m_spEvtHelper->CreateEvent(*pEvent, true, lEventId) != ccEvtMgr::CError::eNoError)
    {
        CCTRACEI(_T("CreateEvent() failed."));
    }
    

    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnChgHomePage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CRegKey rkCurrent;
    LONG lRet;
    lRet = rkCurrent.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\main"), KEY_WRITE);
    if(lRet != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(GetLastError());

    for(int i=1; i <= 100; i++)
    {
        CString cszThePage;
        cszThePage.Format(_T("http://%04d.symantec.com"), i);
        lRet = rkCurrent.SetStringValue(_T("Start Page"), cszThePage);
        Sleep(50);
    }

    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnCreateLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ccEvtMgr::CEventEx *pEvent;

    SYMRESULT sr = m_spEvtHelper->NewHPPEvent(HPP::Event_ID_HPPNotifyHomePage, &pEvent);
    if(SYM_FAILED(sr))
        return 0;

    CHPPEventCommonInterfaceQIPtr spCommonEvent = pEvent;
    if(!spCommonEvent)
        return 0;

    ULONGLONG uIndex = NULL;
    DWORD dwIndexCheck = NULL;

    CError::ErrorType Error = m_EventLog.SaveEvent(*pEvent, uIndex, dwIndexCheck);

    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnReadLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    
    SYSTEMTIME stStart, stStop;
    ccEvtMgr::CLogFactoryEx::EventInfo* pEventInfoArray = NULL;
    long nContextId = NULL, nEventCount = NULL;
    CError::ErrorType eError;
    
    ZeroMemory(&stStart, sizeof(SYSTEMTIME));
    GetSystemTime(&stStop);

    eError = m_EventLog.SearchEvents(stStart, stStop, false, 1000, nContextId, pEventInfoArray, nEventCount);
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CMainDlg::OnBnClickedBtnReadLog() : m_EventLog.SearchEvents() != CError::eNoError"));
    }
    
    if(pEventInfoArray)
    {
        CCTRACEI(_T("CMainDlg::OnBnClickedBtnReadLog() : m_EventLog.SearchEvents() returned %d events"), nEventCount);
        long nCount = NULL;
        for(nCount = 0; nCount < nEventCount; nCount++)
        {
            CEventExPtr spEvent;

            eError = m_EventLog.LoadEvent(pEventInfoArray[nCount].m_uIndex, pEventInfoArray[nCount].m_dwIndexCheck, nContextId, spEvent.m_p);
            if (eError != CError::eNoError)
            {
                CCTRACEE(_T("CMainDlg::OnBnClickedBtnReadLog() : m_EventLog.LoadEvent() != CError::eNoError"));
            }
            else
            {
                CHPPEventCommonInterfaceQIPtr spCommonEvent = spEvent;
                if(!spCommonEvent)
                    continue;

                _bstr_t cbszValue;
                BOOL bRet = spCommonEvent->GetPropertyBSTR(HPP::Event_Base_propUserName, cbszValue.GetAddress());
                if(!bRet)
                {
                    CCTRACEE(_T("CMainDlg::OnBnClickedBtnReadLog() : spCommonEvent->GetPropertyBSTR() != TRUE"));
                }

            }
        }

        m_EventLog.DeleteSearchData(pEventInfoArray);
    }

    m_EventLog.ClearLog();
    

    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnIeOpened(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ccEvtMgr::CEventEx *pEvent;

    SYMRESULT sr = m_spEvtHelper->NewHPPEvent(HPP::Event_ID_IE_Started, &pEvent);
    if(SYM_FAILED(sr))
        return 0;

    long lEventId = 0;
    if(m_spEvtHelper->CreateEvent(*pEvent, true, lEventId) != ccEvtMgr::CError::eNoError)
    {
        CCTRACEI(_T("CreateEvent() failed."));
    }



    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnCnnctBb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    
    
    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnDiscnnctBb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnPrepForUninstall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    HPP::CHPPUtilityLoader cLoader;
    HPP::IHPPUtilityInterfacePtr spUtility;

    SYMRESULT sr;
    sr = cLoader.CreateObject(&spUtility);
    if(SYM_FAILED(sr) || !spUtility)
        return 0;

    HRESULT hr = E_FAIL;
//    hr = spUtility->PrepHPPForUninstall();

    hr = spUtility->ResetOptions();

    return 0;
}
