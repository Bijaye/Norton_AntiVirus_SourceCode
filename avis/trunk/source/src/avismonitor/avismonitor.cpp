// AVISMonitor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AVISMonitor.h"

#include "MainFrm.h"
#include <bldinfo.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorApp

BEGIN_MESSAGE_MAP(CAVISMonitorApp, CWinApp)
	//{{AFX_MSG_MAP(CAVISMonitorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CString titleStr;

/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorApp construction

CAVISMonitorApp::CAVISMonitorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAVISMonitorApp object

CAVISMonitorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorApp initialization

BOOL CAVISMonitorApp::InitInstance()
{
    // Run a single instance of avismonitor.exe
    if ( !m_singleInstance.Create( IDR_MAINFRAME) )
    {
        // Return indicating that this instance
        // of the app should be shut down
        return FALSE;
    }

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("AVISMonitor"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


	titleStr.LoadString (AFX_IDS_APP_TITLE);



	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorApp message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();


// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
    CString m_aboutVersionInfo;
    CString m_aboutProductName;
    CString m_aboutCopyrightInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_aboutCopyrightInfo = _T("");
	m_aboutProductName = _T("");
	m_aboutVersionInfo = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_ABOUT_COPYRIGHT_INFO, m_aboutCopyrightInfo);
	DDX_Text(pDX, IDC_ABOUT_PRODUCT_NAME, m_aboutProductName);
	DDX_Text(pDX, IDC_ABOUT_VERSION_INFO, m_aboutVersionInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAVISMonitorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorApp message handlers


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_aboutVersionInfo.GetBuffer(2048);
    // The version info is always displayed in english
	MyAfxFormatStrings ( m_aboutVersionInfo, _T("Version %1 (build %2)"), 2, // IDS_ABOUT_VERSION_INFO
		                 AVIS_VERSION,
					     AVIS_BUILD );
	m_aboutProductName = titleStr;
	m_aboutCopyrightInfo.GetBuffer(2048);
	MyAfxFormatStrings(m_aboutCopyrightInfo, IDS_ABOUT_COPYRIGHT_INFO,
		               1, AVIS_COPYRIGHTDATES);
	UpdateData (FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*-----------------------------------------------------------------------------
*   Function: MyAfxFormatStrings  
*   Description: 
*       Format the string taking upto 9 string parameters 
*   Parameters: 
*       CString& outString -  
*       UINT formatID -  
*       int count -  
*       LPCTSTR str1 -  
*       LPCTSTR str2 -  
*       LPCTSTR str3 -  
*       LPCTSTR str4 -  
*       LPCTSTR str5 -  
*       LPCTSTR str6 -  
*       LPCTSTR str7 -  
*       LPCTSTR str8 -  
*       LPCTSTR str9 -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void MyAfxFormatStrings (CString& outString, UINT formatID, 
						 int count,
						 LPCTSTR str1,
						 LPCTSTR str2,
						 LPCTSTR str3,
						 LPCTSTR str4,
						 LPCTSTR str5,
						 LPCTSTR str6,
						 LPCTSTR str7,
						 LPCTSTR str8,
						 LPCTSTR str9)
{
	CString formatStr;
	formatStr.LoadString(formatID);

	MyAfxFormatStrings (outString, (LPCTSTR) formatStr, count,
		str1, str2, str3, str4, str5, str6, str7, str8, str9);
}

/*-----------------------------------------------------------------------------
*   Function: MyAfxFormatStrings  
*   Description: 
*       Format the string taking upto 9 string parameters 
*   Parameters: 
*       CString& outString -  
*       LPCTSTR formatStr -  
*       int count -  
*       LPCTSTR str1 -  
*       LPCTSTR str2 -  
*       LPCTSTR str3 -  
*       LPCTSTR str4 -  
*       LPCTSTR str5 -  
*       LPCTSTR str6 -  
*       LPCTSTR str7 -  
*       LPCTSTR str8 -  
*       LPCTSTR str9 -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void MyAfxFormatStrings (CString& outString, LPCTSTR formatStr, 
						 int count,
						 LPCTSTR str1,
						 LPCTSTR str2,
						 LPCTSTR str3,
						 LPCTSTR str4,
						 LPCTSTR str5,
						 LPCTSTR str6,
						 LPCTSTR str7,
						 LPCTSTR str8,
						 LPCTSTR str9)
{
	LPCTSTR tmpBuf[9];
	tmpBuf[0] = str1;
	tmpBuf[1] = str2;
	tmpBuf[2] = str3;
	tmpBuf[3] = str4;
	tmpBuf[4] = str5;
	tmpBuf[5] = str6;
	tmpBuf[6] = str7;
	tmpBuf[7] = str8;
	tmpBuf[8] = str9;

	if (count > 9)
		count = 9;

	AfxFormatStrings(outString, formatStr, tmpBuf, count);
}
