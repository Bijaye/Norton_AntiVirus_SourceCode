/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/qconsole.cpv   1.12   14 Aug 1998 12:59:12   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
// qconsole.cpp : Defines the class behaviors for the application.
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/qconsole.cpv  $
// 
//    Rev 1.12   14 Aug 1998 12:59:12   mdunn
// Changed trial-expired message to support varying-length trial periods.
// 
//    Rev 1.11   14 Jul 1998 16:05:24   mdunn
// D'oh... include tryint.h only in trial versions.
// 
//    Rev 1.10   14 Jul 1998 15:52:08   mdunn
// Added #include <initguid.h> to get trialware GUIDs declared properly.
// 
//    Rev 1.9   10 Jul 1998 15:10:10   DBuches
// Ignore files passed in on command line.
// 
//    Rev 1.8   01 Jul 1998 18:10:38   VGLOSTE
// Made the function IsTrialPeriodOverridden() a member function instead 
// of a static function.
// 
//    Rev 1.7   29 Jun 1998 16:08:12   VGLOSTE
// Added code to respect a regkey that overrides the trial length.
// 
//    Rev 1.6   17 Jun 1998 10:42:14   mdunn
// Added a very, very important note by the CreateMutex() call.
// 
//    Rev 1.5   11 Jun 1998 15:48:26   DBuches
// Added support for trial ware.
// 
//    Rev 1.4   07 May 1998 15:08:12   DBuches
// Added support for status text.
// 
//    Rev 1.3   04 May 1998 20:50:00   DBuches
// Only prompt for scan when there are items to scan.
// 
//    Rev 1.2   28 Apr 1998 15:24:58   DBuches
// Added single instance support.  Also hooked up NAV about box.
// 
//    Rev 1.1   07 Apr 1998 16:50:38   DBuches
// 1st pass at startup scanning.
// 
//    Rev 1.0   27 Feb 1998 15:08:58   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"

#include "MainFrm.h"
#include "qconsoleDoc.h"
#include "qconsoleView.h"
#include "const.h"
#include "qresinit.h"
#include "navdoabt.h"
#include "navver.h"

// For trial ware stuff        
#ifdef NAV_PREVIEW
#define INITGUID
#include <initguid.h>
#include "tryint.h"
#define DEFAULT_TRIAL_LENGTH 30
#define MAX_TRIAL_LENGTH 90
#endif

                                 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Local functions

BOOL CALLBACK EnumWindowProc( HWND hwnd, // handle to parent window 
                              LPARAM lParam // application-defined value 
                             );


/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp

BEGIN_MESSAGE_MAP(CQconsoleApp, CWinApp)
	//{{AFX_MSG_MAP(CQconsoleApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp construction

CQconsoleApp::CQconsoleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
    m_nShowCmd = SW_SHOWNORMAL;	
    m_hMutex = NULL;
    m_bTrialVaild = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CQconsoleApp object

CQconsoleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp initialization

BOOL CQconsoleApp::InitInstance()
{
	
    // Standard initialization
    //
    // ** WARNING **  If you change the name of the mutex, you MUST also
    // make the same change in the RemoveQConsole() function in NAVINST.CPP
    // of BOTH the NAVINST95 and NAVINSTNT projects.


    m_hMutex = CreateMutex(NULL, FALSE, _T("Quarantine Console Mutex"));

    // Q: Is there another instance of us hanging around?
    if( GetLastError() == ERROR_ALREADY_EXISTS )
        {
        // Locate other window and bring to front.
        HWND hFound = NULL;
        EnumWindows( EnumWindowProc, (LPARAM) &hFound );
        if( hFound != NULL )
            {
            if( IsIconic( hFound ) )
                ShowWindow( hFound, SW_RESTORE );
            ::SetForegroundWindow( hFound );
            return FALSE;
            }
        }

    // Load our resource DLL
    InitQConsoleResources();

    // 
    // Setup trial ware stuff.
    //
    SetupTrial();
    m_bTrialVaild = CheckTrialExpire();



#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    // Initialize OLE
    AfxOleInit();        

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey( SYMANTEC );

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CQconsoleDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CQconsoleView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

    // Ignore files passed in on command line.
    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
    if( ProcessShellCommand( cmdInfo ) == FALSE )
        return FALSE;

	// The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(m_nShowCmd);
	m_pMainWnd->UpdateWindow();

    
    CQconsoleDoc* pDoc = (CQconsoleDoc*) ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
    ASSERT( pDoc );


    if( m_bTrialVaild )
        {
    

        // Check to see if we need to do a scan at startup.
        BOOL bStartScan;
        pDoc->m_pOpts->GetScanAtStartup( &bStartScan );

        int iItemCount = pDoc->GetQuarantineItemCount() + 
                         pDoc->GetSubmittedItemCount();

        if( bStartScan && iItemCount && pDoc->GetStartupScanNeeded() )
            {
            CString sTitle((LPCTSTR) IDS_NEW_DEFS_AVAILIABLE );
            CString sText((LPCTSTR) IDS_NEW_DEFS_AVAILIABLE_TEXT );
        
            if( m_pMainWnd->MessageBox( sText, sTitle, MB_ICONQUESTION | MB_YESNO ) == IDYES )
                {
                pDoc->UpdateAllViews( NULL, LISTVIEW_NEW_DEFS, 0 );
                }
            }
        }
    else
        {
        CString cstrExpiredMsg;
        DWORD   dwTrialLength = 30, dwBufSize = sizeof(DWORD);
        HKEY    hkey;
        LONG    lRet;

        lRet = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, 
                              _T("software\\Symantec\\Norton AntiVirus"),
                              0, KEY_QUERY_VALUE, &hkey );

        if ( ERROR_SUCCESS == lRet )
            {
            lRet = RegQueryValueEx ( hkey, _T("TrialLength"), NULL,
                                     NULL, (LPBYTE) &dwTrialLength,
                                     &dwBufSize );

            RegCloseKey ( hkey );
            }

        cstrExpiredMsg.Format ( IDS_TRIALWARE_EXPIRED, dwTrialLength );

        AfxMessageBox( cstrExpiredMsg, MB_ICONINFORMATION | MB_OK );
        }

    // Process all messages in our queue.
    MSG msg;
    while( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        PumpMessage();

    // If there are quarantine items, set status text.
    if( pDoc->GetQuarantineItemCount() )
        ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( IDS_STATUS_TEXT_QUARANTINED );


    return TRUE;
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
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
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

// App command to run the dialog
void CQconsoleApp::OnAppAbout()
{
    // Fire off about box.
    DoStdAbout( AfxGetMainWnd()->GetSafeHwnd() );
}

/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp commands


BOOL CALLBACK EnumWindowProc( HWND hwnd, // handle to parent window 
                              LPARAM lParam // application-defined value 
                             )
{
    // Get title text to compare
    CString sTitleText( (LPCTSTR) IDS_MAINFRM_TITLE );    
    TCHAR szWindowText[MAX_PATH];

    // Get window text.
    ::GetWindowText( hwnd, szWindowText, MAX_PATH );

    // Shorten text for comaprision
    szWindowText[sTitleText.GetLength()] = _T('\0');

    // Check for match
    if( sTitleText == szWindowText )
        {
        // Save off this window handle.
        HWND *pHwnd = (HWND*) lParam;
        *pHwnd = hwnd;
        return FALSE;
        }

    // Continue enumeration.
    return TRUE;
}

 
////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleApp::ExitInstance
//
// Description	    : Called when app shuts down
//
// Return type		: int 
//
//
////////////////////////////////////////////////////////////////////////////
// 4/28/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CQconsoleApp::ExitInstance() 
{
	// Remove single instance mutex.
    if( m_hMutex != NULL )
        {
        CloseHandle( m_hMutex );
        m_hMutex = NULL;
        }
	
	return CWinApp::ExitInstance();
}

//************************************************************************
// CQconsoleApp::IsTrialPeriodOverridden(int *pTrialLength)
//
// Description:
//		Checks to see if the trial period is overridden 
//		by the key value
//		\\HKEY_LOCAL_MACHINE\SOFTWARE\Symantec\SymSE\NAVTrialLength
//
// Parameters:
//		int *pTrialLength	Points to an integer containing the current
//							trial length.  If the length is overridden,
//							the new value is written to this integer
//
// Returns:
//      TRUE    if trial length overridden.
//      FALSE   trial length not overridden.
//************************************************************************
// 6/30/98 VGLOSTE, ported from Roswell
//************************************************************************

BOOL CQconsoleApp::IsTrialPeriodOverridden(int *pTrialLength)
{
#ifdef NAV_PREVIEW
	auto    BOOL bResult = FALSE;
	auto    HKEY hKey;
	auto    DWORD dwLength, dwType, dwSize;

	if(RegOpenKey(	HKEY_LOCAL_MACHINE, 
					_T("SOFTWARE\\Symantec\\SymSE"), 
					&hKey) == ERROR_SUCCESS)
	{	dwSize = sizeof(dwLength);
		if(RegQueryValueEx(	hKey, 
							_T("NAVTrialLength"), 
							NULL, 
							&dwType, 
							(LPBYTE) &dwLength, 
							&dwSize) == ERROR_SUCCESS &&
							dwType == REG_DWORD)
		{	*pTrialLength = dwLength;
			bResult = TRUE;
		}
		RegCloseKey(hKey);
	}
	return bResult;
#else
	return FALSE;
#endif
}

//*************************************************************************
// CQconsoleApp::SetupTrial()
//
// CQconsoleApp::SetupTrial( )
//
// Description:     Sets up 30 day trial period for trialware version    
//
// Returns: void 
//
//*************************************************************************
// 6/11/98 DBUCHES, created - header added.
//*************************************************************************

void CQconsoleApp::SetupTrial()
{
#ifdef NAV_PREVIEW
    auto    ITrialWare  *pTrial;
    auto    BOOL        fSucceeded = FALSE;
	auto	int			iTrial = DEFAULT_TRIAL_LENGTH;

    // Initalize COM
    CoInitialize(NULL);

    // attempt to create trial object
    HRESULT hResult = CoCreateInstance( CLSID_TrialWare,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_ITrialWare,
					(LPVOID *)&pTrial );

    if ( SUCCEEDED( hResult ) )
	{
	// attempt to initialize trial object
	if ( SUCCEEDED( pTrial->Initialize( TEXT("Symantec"),
					    TEXT("Norton AntiVirus"),
					    NAV_VERSION_MAJOR,
					    NAV_VERSION_MINOR ) ) )
	    {
			// Change the trial length if the override key exists 
			// in the registry
			IsTrialPeriodOverridden(&iTrial);
			if(iTrial > MAX_TRIAL_LENGTH)
			   iTrial = MAX_TRIAL_LENGTH;

	        // display error message and cancel operation if call failed
	        if ( SUCCEEDED( pTrial->Setup( iTrial, 0L )))
		    {
		    fSucceeded = TRUE;
		    }
	    }

	// release TrialWare object
	pTrial->Release();
	}

    // Balance call to CoInitalize;
    CoUninitialize();
#endif
}

//*************************************************************************
// CQconsoleApp::CheckTrialExpire()
//
// CQconsoleApp::CheckTrialExpire( )
//
// Description:  Trialware validation code copied from NAVW.
//
// Returns: BOOL 
//
//*************************************************************************
// 6/11/98 DBUCHES, created - header added.
//*************************************************************************

BOOL CQconsoleApp::CheckTrialExpire()
{
#ifdef NAV_PREVIEW    
    auto    ITrialWare  *pTrial;
    auto    BOOL        bExpired = TRUE;

    // Initalize COM
    CoInitialize(NULL);

    // attempt to create trial object
    if ( SUCCEEDED( CoCreateInstance( CLSID_TrialWare,
				      NULL,
				      CLSCTX_INPROC_SERVER,
				      IID_ITrialWare,
				      (LPVOID *)&pTrial ) ) )
	{
	// attempt to initialize trial object

	if ( SUCCEEDED( pTrial->Initialize( TEXT( "Symantec" ),
					    TEXT( "Norton AntiVirus" ),
					    NAV_VERSION_MAJOR,
					    NAV_VERSION_MINOR ) ) )
	    {
	    if ( pTrial->IsValid( HWND_DESKTOP, TRUE, NULL ) == S_OK )
		    {
		    bExpired = FALSE;
		    }
	    }

	pTrial->Release();
	}

    // Balance call to CoInitalize;
    CoUninitialize();

    return ( !bExpired );
#else
    return TRUE;
#endif
}
