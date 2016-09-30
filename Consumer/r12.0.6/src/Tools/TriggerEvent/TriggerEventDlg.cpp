// TriggerEventDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TriggerEvent.h"
#include "TriggerEventDlg.h"
#include "GlobalEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CTriggerEventDlg dialog

CTriggerEventDlg::CTriggerEventDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTriggerEventDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTriggerEventDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTriggerEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTriggerEventDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTriggerEventDlg, CDialog)
	//{{AFX_MSG_MAP(CTriggerEventDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_FIRE, OnBtnFire)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriggerEventDlg message handlers

BOOL CTriggerEventDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
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
	
	// TODO: Add extra initialization here
	CheckRadioButton( IDC_RDO_SET, IDC_RDO_PULSE, IDC_RDO_SET );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTriggerEventDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTriggerEventDlg::OnPaint() 
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
HCURSOR CTriggerEventDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTriggerEventDlg::OnBtnFire() 
{

	// Grab pointer to Extended Combobox
	
	CComboBoxEx*  pCombo = (CComboBoxEx*) GetDlgItem(IDC_CMB_EVENTLIST);

	// Store Event to fire (Allow user to specify one if it's not in list)

	CString strSelected;
	pCombo->GetDlgItemText(IDC_CMB_EVENTLIST, strSelected);

	// Make sure it's not blank
	if (strSelected.IsEmpty())
	{
		MessageBox("Which event do you want to Fire?", "Pay Attention", MB_OK);
		return;
	}

	// Create the Event
	
	CGlobalEvent MyEvent;
	if (MyEvent.Create(strSelected) == FALSE)
	{
		// couldn't create the event, let user know.
		CString strMessage = "Unable to create the ";
		strMessage += strSelected;
		strMessage += " event.";

		MessageBox(strMessage, NULL, MB_OK);
		
		// bail
		return;
	}
	
	// Set Event or Pulse it

	if (IsDlgButtonChecked(IDC_RDO_PULSE))
	{
		PulseEvent(MyEvent);
		CString strMessage = "Event Fired (Pulse): ";
		strMessage += strSelected;
		MessageBox(strMessage, NULL, MB_OK);
	}
	else
	{
		SetEvent(MyEvent);
		CString strMessage = "Event Fired (Set): ";
		strMessage += strSelected;
		MessageBox(strMessage, NULL, MB_OK);
	}
}
