////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "TestProvider.h"
#include "TestProviderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const TCHAR szTitle[] = "Test Event Provider (Log View) Ver 1.5";

CTestProviderDlg::CTestProviderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestProviderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestProviderDlg)
	m_nProviderId = 0;
	m_nTestValue = 0;
	m_nEventId = 0;
	m_bBroadcast = FALSE;
	m_nCancelledId = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestProviderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestProviderDlg)
	DDX_Text(pDX, IDC_PROVIDER_ID_EDIT, m_nProviderId);
	DDX_Text(pDX, IDC_TEST_VALUE_EDIT, m_nTestValue);
	DDX_Text(pDX, IDC_EVENT_ID_EDIT, m_nEventId);
	DDX_Check(pDX, IDC_BROADCAST_CHECK, m_bBroadcast);
	DDX_Text(pDX, IDC_CANCELLED_ID_EDIT, m_nCancelledId);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestProviderDlg, CDialog)
	//{{AFX_MSG_MAP(CTestProviderDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_EVENT_ERROR_BUTTON, OnEventErrorButton)
    ON_BN_CLICKED(IDC_VIRUS_ALERT_BUTTON, OnVirusAlertEventButton)
	ON_BN_CLICKED(IDC_REGISTER_BUTTON, OnRegisterButton)
	ON_BN_CLICKED(IDC_UNREGISTER_BUTTON, OnUnregisterButton)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, OnConnectButton)
	ON_BN_CLICKED(IDC_VIRUS_LIKE_BUTTON, OnVirusLikeButton)
	ON_BN_CLICKED(IDC_SB_BUTTON, OnSbButton)
	ON_BN_CLICKED(IDC_SCAN_BUTTON, OnScanButton)
	ON_BN_CLICKED(IDC_BUTTON_ALL, OnButtonAll)
	ON_BN_CLICKED(IDC_BROADCAST_CHECK, OnBroadcastCheck)
	ON_BN_CLICKED(IDC_BUTTON_TIMER, OnButtonTimer)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTestProviderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

    GetDlgItem(IDC_EVENT_ERROR_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_VIRUS_ALERT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_SB_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_SCAN_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_UNREGISTER_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_VIRUS_LIKE_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ALL)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_BUTTON_TIMER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);	

	GetDlgItem(IDC_EDIT_FILENAME)->SetWindowText("C:\\Hello this is a test of Norton AntiVirus Log Viewer");
	
	GetDlgItem(IDC_EDIT_VIRUSLIKE)->SetWindowText("1");
	GetDlgItem(IDC_EDIT_ERROREVENT)->SetWindowText("1");
	GetDlgItem(IDC_EDIT_VIRUSALERT)->SetWindowText("1");
	GetDlgItem(IDC_EDIT_SCRIPTBLOCKING)->SetWindowText("1");
	GetDlgItem(IDC_EDIT_APPACTIVITY)->SetWindowText("1");
    GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("NAV Log View Tester");

	m_bTimer = false;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestProviderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTestProviderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestProviderDlg::OnOk() 
{
	// Nothing
}



void CTestProviderDlg::OnRegisterButton() 
{
    UpdateData(TRUE);

    if (m_Provider.Register(m_hWnd, 
                            m_nProviderId) == FALSE)
    {
        MessageBox(_T("Failed to register provider"));
    }
    else
    {
        GetDlgItem(IDC_EVENT_ERROR_BUTTON)->EnableWindow(TRUE);
        GetDlgItem(IDC_VIRUS_ALERT_BUTTON)->EnableWindow(TRUE);
        GetDlgItem(IDC_SB_BUTTON)->EnableWindow(TRUE);
        GetDlgItem(IDC_SCAN_BUTTON)->EnableWindow(TRUE);
        GetDlgItem(IDC_UNREGISTER_BUTTON)->EnableWindow(TRUE);
        GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_VIRUS_LIKE_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ALL)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_TIMER)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEST_VALUE_EDIT)->EnableWindow(false);
	//GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);	
		
    }

    UpdateData(FALSE);
}

void CTestProviderDlg::OnUnregisterButton() 
{
    UpdateData(TRUE);

    m_Provider.Unregister();

    m_nProviderId = 0;
    m_nEventId = 0;

    GetDlgItem(IDC_EVENT_ERROR_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_VIRUS_ALERT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_SB_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_SCAN_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_UNREGISTER_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_VIRUS_LIKE_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ALL)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_TIMER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);	

    UpdateData(FALSE);
}

void CTestProviderDlg::OnCancel() 
{
	m_Provider.Disconnect();
	
    if (m_pEvent != NULL)
    {
        m_Provider.DeleteEvent(m_pEvent);
        m_pEvent = NULL;
    }
	
	CDialog::OnCancel();
}

void CTestProviderDlg::OnConnectButton() 
{
	if (m_Provider.Connect() == FALSE)
    {
        MessageBox(_T("Failed to connect to Event Manager"));
        return;
    }
    GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(TRUE);
}

LRESULT CTestProviderDlg::OnShutdown(WPARAM wParam, LPARAM lParam)
{
    OnUnregisterButton();

    return 0;
}

// Virus Like
void CTestProviderDlg::OnVirusLikeButton() 
{
    //////////////////////////////////////////////////////////////
    DWORD dwHowMany = GetDlgItemInt(IDC_EDIT_VIRUSLIKE);
	if(dwHowMany <= 0 || dwHowMany > 10000)
	{
		MessageBox("Enter digit from 1 to 10000.", "Virus Like...", MB_OK | MB_ICONERROR);
		SetDlgItemInt(IDC_EDIT_VIRUSLIKE, 1);
		return;
	}
	CString strFileName;
	GetDlgItemText((IDC_EDIT_FILENAME), strFileName);
	DWORD dwI;
	UpdateData(TRUE);
	GetDlgItem(IDC_VIRUS_LIKE_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Creating 'Virus Like' entries...");
	if(m_bTimer)CWaitCursor wait;
	for(dwI = 0; dwI < dwHowMany; dwI++)
	{
		if (m_Provider.CreateTestEvent(m_bBroadcast, 
									   m_nTestValue, 
									   m_nEventId,
									   AV_Event_ID_VirusLike, strFileName) == FALSE)
		{
			MessageBox(_T("Failed to create Test Event"));
			return;
		}
	}
	Sleep(1500);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Done.");
	GetDlgItem(IDC_VIRUS_LIKE_BUTTON)->EnableWindow(true);
	UpdateData(FALSE);
	
}

// Error Event
void CTestProviderDlg::OnEventErrorButton() 
{
    DWORD dwHowMany = GetDlgItemInt(IDC_EDIT_ERROREVENT);
	if(dwHowMany <= 0 || dwHowMany > 10000)
	{
		MessageBox("Enter digit from 1 to 10000.", "Error Event...", MB_OK | MB_ICONERROR);
		SetDlgItemInt(IDC_EDIT_ERROREVENT, 1);
		return;
	}
	CString strFileName;
	GetDlgItemText((IDC_EDIT_FILENAME), strFileName);
	DWORD dwI;
	UpdateData(TRUE);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Creating 'Error Events' entries...");
	GetDlgItem(IDC_EVENT_ERROR_BUTTON)->EnableWindow(false);
	if(m_bTimer)CWaitCursor wait;
	for(dwI = 0; dwI < dwHowMany; dwI++)
	{

		if (m_Provider.CreateTestEvent(m_bBroadcast, 
									   m_nTestValue, 
									   m_nEventId,
									   AV_Event_ID_Error, strFileName) == FALSE)
		{
			MessageBox(_T("Failed to create Test Event"));
			return;
		}
	}
	Sleep(1500);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Done.");	
	GetDlgItem(IDC_EVENT_ERROR_BUTTON)->EnableWindow(true);
	UpdateData(FALSE);
}


// Virus Alert Event
void CTestProviderDlg::OnVirusAlertEventButton() 
{
    UpdateData(TRUE);

    DWORD dwHowMany = GetDlgItemInt(IDC_EDIT_VIRUSALERT);
	if(dwHowMany <= 0 || dwHowMany > 10000)
	{
		MessageBox("Enter digit from 1 to 10000.", "Virus Alert Event...", MB_OK | MB_ICONERROR);
		SetDlgItemInt(IDC_EDIT_VIRUSALERT, 1);
		return;
	}
	CString strFileName;
	GetDlgItemText((IDC_EDIT_FILENAME), strFileName);
	GetDlgItem(IDC_VIRUS_ALERT_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Creating 'Virus Alert Events' entries...");	
	DWORD dwI;
	if(m_bTimer)CWaitCursor wait;	
	for(dwI = 0; dwI < dwHowMany; dwI++)
	{
		
		if (m_Provider.CreateTestEvent(m_bBroadcast, 
									   m_nTestValue, 
									   m_nEventId,
									   AV_Event_ID_VirusAlert, strFileName) == FALSE)
		{
			MessageBox(_T("Failed to create Test Event."));
			return;
		}
	}
	Sleep(1500);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Done.");	
	GetDlgItem(IDC_VIRUS_ALERT_BUTTON)->EnableWindow(true);
	UpdateData(FALSE);
}


// Script Blocking
void CTestProviderDlg::OnSbButton() 
{
	UpdateData(TRUE);
	DWORD dwHowMany = GetDlgItemInt(IDC_EDIT_SCRIPTBLOCKING);
	if(dwHowMany <= 0 || dwHowMany > 10000)
	{
		MessageBox("Enter digit from 1 to 10000.", "Script Blocking...", MB_OK | MB_ICONERROR);
		SetDlgItemInt(IDC_EDIT_SCRIPTBLOCKING, 1);
		return;
	}
	CString strFileName;
	GetDlgItemText((IDC_EDIT_FILENAME), strFileName);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Creating 'Script Blocking' entries...");		
	GetDlgItem(IDC_SB_BUTTON)->EnableWindow(false);
	if(m_bTimer)CWaitCursor wait;	
	DWORD dwI;
	for(dwI = 0; dwI < dwHowMany; dwI++)
	{
		
		if (m_Provider.CreateTestEvent(m_bBroadcast, 
									   m_nTestValue, 
									   m_nEventId,
									   AV_Event_ID_SBAction, strFileName) == FALSE)
		{
			MessageBox(_T("Failed to create Test Event"));
			return;
		}
	}
	Sleep(1500);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Done.");		
	GetDlgItem(IDC_SB_BUTTON)->EnableWindow(true);
	UpdateData(FALSE);	
}

// Application Acitiviy
void CTestProviderDlg::OnScanButton() 
{
    UpdateData(TRUE);

	DWORD dwHowMany = GetDlgItemInt(IDC_EDIT_APPACTIVITY);
	if(dwHowMany <= 0 || dwHowMany > 10000)
	{
		MessageBox("Enter digit from 1 to 10000.", "Application Activity...", MB_OK | MB_ICONERROR);
		SetDlgItemInt(IDC_EDIT_SCRIPTBLOCKING, 1);
		return;
	}
	CString strFileName;
	GetDlgItemText((IDC_EDIT_FILENAME), strFileName);
	DWORD dwI;
	if(m_bTimer)CWaitCursor wait;	
	UpdateData(TRUE);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Creating 'Application Activity' entries...");	
	GetDlgItem(IDC_SCAN_BUTTON)->EnableWindow(false);
	for(dwI = 0; dwI < dwHowMany; dwI++)
	{
		if (m_Provider.CreateTestEvent(m_bBroadcast, 
									   m_nTestValue, 
									   m_nEventId,
									   AV_Event_ID_ScanAction, 
									   m_strDBCSName) == FALSE)
		{
			MessageBox(_T("Failed to create Test Event"));
			return;
		}
	}
	Sleep(1500);
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Done.");	
	GetDlgItem(IDC_SCAN_BUTTON)->EnableWindow(true);
    UpdateData(FALSE);	
}

void CTestProviderDlg::OnButtonAll() 
{
	PeekIt();
	GetDlgItem(IDC_BUTTON_ALL)->EnableWindow(false);
	OnVirusLikeButton();
	OnEventErrorButton();
	OnVirusAlertEventButton();
	OnSbButton();
	OnScanButton();
	Sleep(500);
	Beep(90, 150);
	GetDlgItem(IDC_BUTTON_ALL)->EnableWindow(true);
}

void CTestProviderDlg::OnBroadcastCheck() 
{
	// TODO: Add your control notification handler code here
	MessageBox("Under Construction for future...");
	CButton * pBroadCast = (CButton*) GetDlgItem(IDC_BROADCAST_CHECK);
	pBroadCast->SetCheck(0);
	GetDlgItem(IDC_BROADCAST_CHECK)->EnableWindow(false);
}

void CTestProviderDlg::OnButtonTimer() 
{
	int iTimer = SetTimer(1, 5000, NULL);
	if(iTimer == 0)
	{
		MessageBox("Failed to install Timer");
		return;
	}
	PeekIt();
	GetDlgItem(IDC_BUTTON_TIMER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);		
	m_bTimer = true;
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("LOGGING EVERY 5 SECS");

}

void CTestProviderDlg::OnTimer(UINT nIDEvent) 
{
	PeekIt();
	OnButtonAll();
	SetWindowText("Starting Timer. Wait...");
	Sleep(600);
	
	CDialog::OnTimer(nIDEvent);
}

void CTestProviderDlg::OnButtonStop() 
{
	PeekIt();
	int iTimer = KillTimer(1);
	if(iTimer == 0)
	{
		MessageBox("Failed to kill timer, manually kill application!");
		return;
	}
	GetDlgItem(IDC_STATIC_TIMER)->SetWindowText("Timer Stopped");
	SetWindowText("Stopping Timer. Wait...");
	Sleep(600);
	GetDlgItem(IDC_BUTTON_TIMER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);		
	SetWindowText(szTitle);
}

void CTestProviderDlg::PeekIt()
{
	MSG msg;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

