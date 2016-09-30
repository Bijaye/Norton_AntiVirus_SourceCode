// AVISACSampleCommDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AVISACSampleCommDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_EVENT_ID	10
#define TIMER_PERIOD	1000

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAVISACSampleCommDlg dialog

CAVISACSampleCommDlg::CAVISACSampleCommDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAVISACSampleCommDlg::IDD, pParent),
	  m_pGwMgr(NULL),
	  m_boShutdown(FALSE)
{
	//{{AFX_DATA_INIT(CAVISACSampleCommDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAVISACSampleCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAVISACSampleCommDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_USER_MSG, m_ctlUserMsg);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAVISACSampleCommDlg, CDialog)
	//{{AFX_MSG_MAP(CAVISACSampleCommDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAVISACSampleCommDlg message handlers

BOOL CAVISACSampleCommDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
		pSysMenu->DeleteMenu(SC_SIZE, MF_BYCOMMAND);
		pSysMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);
		pSysMenu->EnableMenuItem(SC_RESTORE, MF_BYCOMMAND | MF_GRAYED);
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// launch the gateway clients
	m_pGwMgr = new CAVISGatewayManagerSampleComm;

	if (!m_pGwMgr->StartAllGatewayComm())
	{
		CAVISACSampleCommApp::CriticalError("Failed to launch all gateway clients; exiting...");
		EndDialog(0);
		return FALSE;
	}

	m_timeLast = CTime::GetCurrentTime();

	// create a timer that fires every second
	SetTimer(TIMER_EVENT_ID, TIMER_PERIOD, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAVISACSampleCommDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CMenu* pSysMenu;

	switch(nID & 0xFFF0)
	{
		case IDM_ABOUTBOX:
		{
			CAboutDlg dlgAbout;
			dlgAbout.DoModal();
			break;
		}

		case SC_MINIMIZE:
			pSysMenu = GetSystemMenu(FALSE);
			if (pSysMenu != NULL)
			{
				pSysMenu->EnableMenuItem(SC_RESTORE, MF_BYCOMMAND | MF_ENABLED);
				pSysMenu->EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);
			}
			CDialog::OnSysCommand(nID, lParam);
			break;

		case SC_RESTORE:
			pSysMenu = GetSystemMenu(FALSE);
			if (pSysMenu != NULL)
			{
				pSysMenu->EnableMenuItem(SC_RESTORE, MF_BYCOMMAND | MF_GRAYED);
				pSysMenu->EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | MF_ENABLED);
			}
			CDialog::OnSysCommand(nID, lParam);
			break;

		default:
			CDialog::OnSysCommand(nID, lParam);
			break;
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAVISACSampleCommDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAVISACSampleCommDlg::OnQueryDragIcon() 
{
	return (HCURSOR) m_hIcon;
}
/*****/
void CAVISACSampleCommDlg::OnOK() 
{
	if (m_boShutdown || (IDYES == ::AfxMessageBox("Are you sure?", MB_YESNO)))
	{
		// the destructor for CAVISGatewayManager asks each gateway to stop its clients.
		if (m_pGwMgr != NULL)
		{
			m_btnOK.EnableWindow(FALSE);
			m_ctlUserMsg.SetWindowText("Shutting Down Clients...");
			UpdateData(FALSE);
			Sleep(1000);
			delete m_pGwMgr;
			m_pGwMgr = NULL;
			m_ctlUserMsg.SetWindowText("Clients have shut themselves down.");
			UpdateData(FALSE);
			Sleep(2000);
		}

		CDialog::OnOK();
	}
}
/*****/
void CAVISACSampleCommDlg::ShutDown()
{
	m_boShutdown = TRUE;
	PostMessage(WM_COMMAND, IDOK);
}
/*****/
void CAVISACSampleCommDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);

	CTime timeNow = CTime::GetCurrentTime();

	if (timeNow.GetDay() != m_timeLast.GetDay())
	{
		KillTimer(TIMER_EVENT_ID);
		CAVISACSampleCommApp::s_tc.msg("The day has changed; shutting down for later restart...");
		ShutDown();
		return;
	}

	m_timeLast = timeNow;
}
