// nvlbDlg.cpp : implementation file
//
#include <afxdlgs.h>
#include "stdafx.h"
#include "nvlb.h"
#include "nvlbDlg.h"
#include "navengine.h"
#include "frecurse.h"
#include "dirdialog.h"
#include "heursetting.h"
#include "librarystatus.h"

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
// CNvlbDlg dialog

CNvlbDlg::CNvlbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNvlbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNvlbDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hAccel = ::LoadAccelerators( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1) );
}

void CNvlbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNvlbDlg)
	DDX_Control(pDX, IDC_HEUR_SETTING, m_Heur_Setting);
	DDX_Control(pDX, IDC_VIRLIB_DIR, m_Virlib_Dir);
	DDX_Control(pDX, IDC_INF_PATH, m_Inf_Path);
	DDX_Control(pDX, IDC_INCOMING_DIR, m_Incoming_Dir);
	DDX_Control(pDX, IDC_DEFS_DIR, m_Defs_Dir);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNvlbDlg, CDialog)
	//{{AFX_MSG_MAP(CNvlbDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_BN_CLICKED(IDC_BUILD_LIBRARY, OnBuildLibrary)
	ON_BN_CLICKED(IDC_DEFS_DIR, OnDefsDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNvlbDlg message handlers

BOOL CNvlbDlg::OnInitDialog()
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

	// Remove maximize item from menu
	pSysMenu->DeleteMenu( 4, MF_BYPOSITION );


	// Remove Size menu option
	pSysMenu->RemoveMenu( SC_SIZE, MF_BYCOMMAND );

	// Remove Maximize menu option
	pSysMenu->RemoveMenu( SC_MAXIMIZE, MF_BYCOMMAND );

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNvlbDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CNvlbDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNvlbDlg::OnPaint() 
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
HCURSOR CNvlbDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CNvlbDlg::OnButtonExit() 
{
	// Exit out
	SendMessage(WM_CLOSE, 0, 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////
void CNvlbDlg::OnBuildLibrary() 
{

	CHeurSetting		HeurSetting (this);
	CDirDialog			DirDialog;
	CLibraryStatus		LibraryStatus;
	CFileDialog			FileDir (TRUE, 
		                         "inf",
								 "*.inf",
								 OFN_FILEMUSTEXIST |OFN_HIDEREADONLY,
								 "*.inf",
								 this);


	// Get defs location
	if( DirDialog.DoBrowse("Select virus definitions",this, NULL) != IDOK )
	{
		MessageBox("You must select a directory where virus definitions can be found!","Error", MB_ICONSTOP );
		return;
	}
	else
	{
		LibraryStatus.m_Defs_Dir = DirDialog.GetPath();		
		m_Defs_Dir.SetWindowText(DirDialog.GetPath());
	}


	// Get NAVEX15.INF file path
	if( FileDir.DoModal() == IDOK )
	{
		LibraryStatus.m_Inf_Path = FileDir.GetPathName();
		m_Inf_Path.SetWindowText(FileDir.GetPathName());
	}
	else
	{
		LibraryStatus.m_Inf_Path = "";
		m_Inf_Path.SetWindowText("");
	}


	// Get and set heuristic level setting
	HeurSetting.DoModal();
	LibraryStatus.m_HeurLevel = HeurSetting.m_HeurValue;
	
	switch(HeurSetting.m_HeurValue)
	{
		case 0:
			m_Heur_Setting.SetWindowText("Heuristics is off");
			break;
	
		case 1:
			m_Heur_Setting.SetWindowText("Heuristics level is 1");
			break;

		case 2:
			m_Heur_Setting.SetWindowText("Heuristics level is 2");
			break;

		case 3:
			m_Heur_Setting.SetWindowText("Heuristics level is 3");
			break;
	}


	// Get incoming location
	if (DirDialog.DoBrowse ("Select the incoming directory",this, NULL) != IDOK)
	{
		MessageBox ("You must select a directory where virus samples can be found!","Error", MB_ICONSTOP);
		return;
	}
	else
	{
		LibraryStatus.m_Incoming_Dir = DirDialog.GetPath();
		m_Incoming_Dir.SetWindowText(DirDialog.GetPath());
	}



	// Get virus library location
	if (DirDialog.DoBrowse ("Select a directory where the virus library will reside",this, NULL) != IDOK)
	{
		MessageBox ("You must select a directory where to place virus samples!","Error", MB_ICONSTOP);
		return;
	}
	else
	{
		m_Virlib_Dir.SetWindowText (DirDialog.GetPath());
		LibraryStatus.m_Virlib_Dir = DirDialog.GetPath();
	}

	
	LibraryStatus.DoModal();

}


BOOL CNvlbDlg::PreTranslateMessage(MSG* pMsg) 
{

	if( m_hAccel != NULL )
		if(::TranslateAccelerator( m_hWnd, m_hAccel, pMsg ) )
			return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


void CNvlbDlg::OnDefsDir() 
{
	CDirDialog	DirDialog;
	
	DirDialog.DoBrowse("Select virus definitions location", NULL, "C:\\");

	m_Defs_Dir.SetWindowText(DirDialog.GetPath());	
	MessageBox("Error initializing AntiVirus Engine!", "Error", MB_OK);
}


LRESULT CNvlbDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CNvlbDlg::CreateVirLib(LPSTR lpszFile)
{

}
