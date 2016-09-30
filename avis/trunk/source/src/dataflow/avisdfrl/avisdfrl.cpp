// DFLauncher.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AVISDFRL.h"

#include "MainFrm.h"
#include "AVISDFRLView.h"
#include <bldinfo.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CDFLauncherApp

BEGIN_MESSAGE_MAP(CDFLauncherApp, CWinApp)
	//{{AFX_MSG_MAP(CDFLauncherApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFLauncherApp construction

CDFLauncherApp::CDFLauncherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDFLauncherApp object

CDFLauncherApp theApp;
CString titleStr;

BOOL CDFLauncherApp::InitInstance() 
{
	// Standard initialization
	titleStr.LoadString (AFX_IDS_APP_TITLE);

    return CWinApp::InitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_aboutCopyrightInfo;
	CString	m_aboutProductName;
	CString	m_aboutVersionInfo;
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
void CDFLauncherApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// The first entry point for the DLL. The window handles are also exchanged
// so that the messages can be posted for communication between the Manager
// and the Resources window.

__declspec( dllexport ) HWND InitDFLauncher(HWND hALMWnd)
{
    // AFX_MANAGE_STATE is required as the first call for every DLL entry
    // point function which in turn calls other MFC functions.

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    theApp.m_hALMWnd = hALMWnd;
    theApp.InitDFLauncherWindow();
    return theApp.m_pMainWnd->m_hWnd;
}

// The last DLL entry point. The application has to wait till all the worker
// threads started by the resources DLL to run individual jobs are killed.

__declspec( dllexport ) void TermDFLauncher()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    theApp.TermDFLauncherWindow();
}

// The tooltip messages for the toolbar are always send by MFC to the main
// window of the application which happens to be the main window of the dataflow
// Analysis List Manager window and so the tooltips are not displayed for the
// Resources window. To overcome this limitation, the unprocessed messages from
// the main application window is passed to the resources window and is processed
// to display the tooltips properly.

__declspec( dllexport ) BOOL FilterDFLauncherMessage (MSG *pMsg)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (theApp.FilterDFLauncherMessage (pMsg));
}

void CDFLauncherApp::TermDFLauncherWindow()
{
    CDFLauncherView * pDFLauncherView =
        (CDFLauncherView *) ((CFrameWnd*)m_pMainWnd)->GetActiveView();

    if (pDFLauncherView)
    {
        pDFLauncherView->CleanUp();
    }
    m_pMainWnd->DestroyWindow();
}

void CDFLauncherApp::InitDFLauncherWindow()
{

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW, NULL,
		NULL);


	// The one and only window has been initialized, so show and update it.
    // The window is initially hidden and can be viewed by selecting proper
    // option from the application main window.
	m_pMainWnd->ShowWindow(SW_HIDE);
	m_pMainWnd->UpdateWindow();

    return;
}

BOOL CDFLauncherApp::FilterDFLauncherMessage(MSG * pMsg)
{
    return (PreTranslateMessage(pMsg));
}

int CDFLauncherApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

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
