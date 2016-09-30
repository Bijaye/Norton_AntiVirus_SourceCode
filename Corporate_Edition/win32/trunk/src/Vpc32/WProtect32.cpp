// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: WProtect32.cpp
//  Purpose: CWProtect32 Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------



#include "stdafx.h"
#include "OSMigrat.h" //EA for Apllication migration of Win2K
#include "navver.h"

#include "WProtect32.h"
#include "SavAssert.h"
#include "MainFrm.h"
#include "WProtect32Doc.h"
#include "WProtect32View.h"
#include "clientreg.h"
#include "io.h"
#include "PasswordDlg.h"
#include "Language.c"
#include "comdef.h"
#include "ldvpaboutdlg.h"
#include "password.h"
#include "securecommsmisc.h"
#include "UserAccounts.h"
#include "RoleVector.h"
#include "SavrtModuleInterface.h"
#include "SecureZeroMemory.h"
#include <winsock.h>
#include "ScsCommsUtils.h"
#include "SymSaferRegistry.h"

#include "DarwinResCommon.h"

DARWIN_DECLARE_RESOURCE_LOADER(_T("VPC32Res.dll"), _T("VPC32"))

// OEM files
#include "oem_obj.h"

//Define NEEDKERNAL before including this file so I don't get undefined externals
#define NEEDKERNAL
#include "commisc.cpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Import my GUIDS from VpCommon
IMP_VPCOMMON_IIDS

// Sync changes to this constant with NCClient/NCServer's Setup.rul ATOM_REBOOTREQUIRED and WPRotect32's ATOM_INSTALL_REBOOT_REQUIRED
#define ATOM_INSTALL_REBOOT_REQUIRED    "NavCorpRebootRequired"
 
#ifdef WIN32
#define SYM_WIN32
#endif

#include "ClientReg.h"
#include "vphtmlhelp.h"


// BOOL WINAPI HandlerRouting(DWORD dwCtrlType)
//
// always returns true because we need to handle all
// Control values with with a shutdown of the app
//
// posts a message to the VPC main window to close
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType){

	HWND hWnd = FindWindow(NAV_WIN_CLASS_NAME, SERVICE_NAME);

	if( hWnd != NULL )
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}

//Globals
CString	m_gHelpFileName;
CString m_gEncyclopediaName;

BEGIN_MESSAGE_MAP(CWProtect32App, CWinApp)
	//{{AFX_MSG_MAP(CWProtect32App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

//----------------------------------------------------------------
//
// Global methods
//
//----------------------------------------------------------------

//I need a variable to hold the DllRegisterServer address
typedef HRESULT (*DllRegisterServerFP)(void) ;
DllRegisterServerFP DllRegServer;

DWORD GetClientType()
{
	HKEY	hKey;
	DWORD	dwType = 0,
			dwSize;

	//Open the reg key and get the client type out of it
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_READ,
										&hKey) )
	{
		dwSize = sizeof( DWORD );
		SymSaferRegQueryValueEx(	hKey,
							szReg_Val_Client_Type,
							NULL,
							NULL,
							(BYTE*)&dwType,
							&dwSize );

		RegCloseKey( hKey );

	}

	return dwType;
}

//----------------------------------------------------------------
// DoUserLogin
//----------------------------------------------------------------
BOOL DoUserLogin()
{
	HKEY			hAccountsKey;
	HKEY			hKey;
	CString			strSubKey;
	DWORD			dwSize;

	char			szRoles[SYM_MAX_ROLES_LEN];

	ScsSecureComms::CRoleVector roles;
	CPasswordDlg dlg;

	// If we are NOT in server mode, then simply return TRUE.

	if( GetClientType() != CLIENT_TYPE_SERVER )
    	return TRUE;

	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                       szReg_Key_Main _T("\\") szReg_Key_Accounts,
                                       0,
                                       KEY_READ,
                                       &hAccountsKey) )
	{
        // Inability to open the SAV root key means that this user doens't have rights.

        return FALSE;
    }

	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                       szReg_Key_Main,
                                       0,
                                       KEY_READ,
                                       &hKey) )
	{
        // Inability to open the SAV root key means that this user doens't have rights.

        return FALSE;
    }

    // Look up the Server Group name.

	dwSize = 0;
    SymSaferRegQueryValueEx( hKey,
					 szReg_Val_Domain,
					 NULL,
					 NULL,
					 NULL,
					 &dwSize );

	if( dwSize != 0 )
	{
		LPSTR szTemp = dlg.m_strDomainName.GetBuffer( dwSize );

		SymSaferRegQueryValueEx( hKey,
						 szReg_Val_Domain,
						 NULL,
						 NULL,
						 (BYTE*)szTemp,
						 &dwSize );

		dlg.m_strDomainName.ReleaseBuffer();
	}

    // Load ScsComs.

    if( SYM_FAILED( LoadScsComms()) )
    {
        return FALSE;
    }

    // Prompt the user for a user name and password.

	while( true )
	{
        // Make sure that the password is empty.

        dlg.m_strPassword.Empty();

		if( IDOK != dlg.DoModal() )
        {
            // User cancelled.

            return FALSE;
        }

        // Look up the password associated with the user name specified by the user.

        if( VerifyPassword( hAccountsKey, dlg.m_strUserName, dlg.m_strPassword ) != ERROR_SUCCESS )
        {
            // User name doesn't exist or password is incorrect.
            // Don't let the user distinguish between these two cases.

            AfxMessageBox( IDS_WRONG_PASSWORD, MB_OK );

            continue;
        }

        // Make sure that they have Administrative privs.

        if( GetRolesForAccount( hAccountsKey, dlg.m_strUserName, szRoles, sizeof(szRoles) ) == FALSE ||
            roles.ParseRoles(szRoles) == false ||
            roles.CheckForRole( ScsSecureComms::BUILTIN_ROLE_ADMIN ) == false )
        {

            AfxMessageBox( IDS_NO_ADMIN_PRIVS, MB_OK );

            continue;
        }

        break;
    }

    SecureZeroMemory( szRoles, sizeof(szRoles) );

	RegCloseKey( hKey );
	RegCloseKey( hAccountsKey );

	return( TRUE );
}

DWORD g_groupTask[ 50 ] = {0x00};

void InterpretSnapInCommand( WPARAM &wParam )
{
	//Turn the wParam sent in into the proper
	//	group/task DWORD
	wParam = (WPARAM)g_groupTask[wParam];
}

UINT StoreSnapInCommand( DWORD dwID )
{
	static UINT iCounter = 0;
	UINT iRet;

	//We should NEVER get to 50 - that would mean
	//	that we have 50 items on ONE menu. Just return if we
	//	get to that point.
	if( iCounter == 50 )
		return 0;

	//Store the dwID sent in into the proper
	//	group/task DWORD
	g_groupTask[iRet = iCounter] = dwID;

	iCounter++;

	return iRet;
}

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CWProtect32App::CWProtect32App()
{
	// add construction code here,
	// Place all significant initialization in InitInstance
}

//----------------------------------------------------------------
// theApp
//----------------------------------------------------------------
CWProtect32App theApp;

void (FAR PASCAL *procSwitch) (HWND, BOOLEAN);


//----------------------------------------------------------------
// InitInstance
//----------------------------------------------------------------
BOOL CWProtect32App::InitInstance()
{
	CString szMigrationMessage;
	CString szAppName;

	if (SUCCEEDED(g_ResLoader.Initialize()))
	{
		::AfxSetResourceHandle(g_ResLoader.GetResourceInstance());
	}
	else
	{
		return FALSE;
	}

	if (OEMObj.IsOEMBuild())
	{
		if (::GetDesktopWindow())
			OEMObj.LaunchConfigWiz(::GetDesktopWindow());

		// if EULA hasn't been accepted, disable UI
		if (!OEMObj.IsOEM_Initialized())
			return FALSE;
	}
    // Check to see if the user must reboot before install is complete
    if (GlobalFindAtom(ATOM_INSTALL_REBOOT_REQUIRED) != 0)
    {
        CString rebootMessage;
		szAppName.LoadString(AFX_IDS_APP_TITLE);
        rebootMessage.LoadString(IDS_INSTALL_REBOOT_REQUIRED);
        ::MessageBox(NULL, rebootMessage, szAppName, MB_OK | MB_ICONERROR);
        return FALSE;
    }

	//EA - END Application Migration for Win2K
	//Create a mutex
	m_hMutex = CreateMutex ( NULL, TRUE, "PreventSecondClientInstance" );
	//If it already exists, activate the previous instance
	DWORD dwError = GetLastError();
	if ( dwError == ERROR_ALREADY_EXISTS || dwError == ERROR_ACCESS_DENIED )
	{
		CMainFrame	*cParentWnd = NULL;

		cParentWnd = (CMainFrame *)CWnd::FindWindow( NAV_WIN_CLASS_NAME, NULL );

		if ( cParentWnd && cParentWnd->IsWindowEnabled())
		{
			cParentWnd->ShowWindow(SW_RESTORE);

			HMODULE hUser = GetModuleHandle("USER32.DLL");
			procSwitch = (void(FAR PASCAL *)(HWND, unsigned char))
				GetProcAddress(hUser, "SwitchToThisWindow");
			if (procSwitch)
				procSwitch(cParentWnd->m_hWnd, TRUE);
			cParentWnd->BringWindowToTop();
		}
		return FALSE;
	}

	//Register my OCX in case Setup didn't
//	RegisterOCX();

	//Check the language and load the proper DLL
    LCID locale = GetThreadLocale();
    TCHAR lang[4];
	CString strFile,
			strAppDir,
			strTemplate, 
			strTemp;
	char* szFilename;
	
	//Get the program full path
	szFilename = strAppDir.GetBuffer( _MAX_PATH );
	GetModuleFileName( AfxGetInstanceHandle(), szFilename, _MAX_PATH );
	strAppDir.ReleaseBuffer();
	// and chop off the filename
	
	strAppDir = strAppDir.Left( strAppDir.ReverseFind( '\\' ) );

    strTemplate = strAppDir + "\\%sRVPC.DLL";
	GetLanguage( lang, strFile.GetBuffer( strTemplate.GetLength() + 1 ), strTemplate.GetBuffer( strTemplate.GetLength() ) );
	//Release my buffers
	strTemplate.ReleaseBuffer();
	strFile.ReleaseBuffer();


	//Set my global help filename
	m_gHelpFileName = SAV_MAIN_HTML_HELP;

	//Now, make sure I could load the help file
	strTemp.Format( "%s\\%s", strAppDir, m_gHelpFileName );
    if (-1 == access( (LPCTSTR)strTemp, 0 ) )
	{
		m_gHelpFileName.Empty();
	}

	//Finally, set the help file name for all context-sensitive help calls
	free ((void*)m_pszHelpFilePath );

    if ( !m_gHelpFileName.IsEmpty() )
    {
    	m_pszHelpFilePath = m_gHelpFileName;
    }

	InitDLL( AfxGetInstanceHandle() );
	
	//Now get the encyclopedia
	//Open the registry

	HKEY	hKey;
	LPSTR	szTemp;
	DWORD	dwSize = 64;

	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_QUERY_VALUE,
										&hKey ) )
	{
		szTemp = m_gEncyclopediaName.GetBuffer( dwSize );
		SymSaferRegQueryValueEx(	hKey,
							szReg_Val_ScanEngineHelpFile,
							NULL,
							NULL,
							(BYTE*)szTemp,
							&dwSize );
		m_gEncyclopediaName.ReleaseBuffer();

		//If I have a name now, make sure it has the REF type so
		// the window appears correctly.
		// if( !m_gEncyclopediaName.IsEmpty() )
		//	m_gEncyclopediaName += ">ref";

		RegCloseKey( hKey );
	}

	//If I am running on a server, then I need to 
	// get the password first
	if( DoUserLogin() == FALSE )
		return FALSE;

	//Cache out the OCX's I will use
	m_comCache.CacheObjects( "" );

    // Removing the following code to prevent the processing of the command line
    // to fix STS #135863 -- RCHINTA
	// Parse command line for standard shell commands, DDE, file open
    //	CCommandLineInfo cmdInfo;
    //	ParseCommandLine(cmdInfo);

	//I need to initialize OLE first
	OleInitialize(NULL);
	AfxEnableControlContainer();

//	CSplashWnd::EnableSplashScreen( cmdInfo.m_bShowSplash );

	CSingleDocTemplate* pDocTemplate = NULL;

	try
	{
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWProtect32Doc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWProtect32View));
	AddDocTemplate(pDocTemplate);
	}
	catch(std::bad_alloc &) {return FALSE;}

	// Dispatch commands specified on the command line
    // Removing the following code to prevent the processing of the command line
    // to fix STS #135863 -- RCHINTA
    //	if (!ProcessShellCommand(cmdInfo))
    //		return FALSE;

    // Added the following function call to ensure OnNewDocument() gets called
    // since ProcessShellCommand() is being removed (above) -- RCHINTA
    pDocTemplate->OpenDocumentFile(NULL);

	//Finally, I need to get an IVirusProtect interface so
	//	I can tell CliScan when to unload
	m_pVirusProtect = NULL;
	
	// Vista change: We no longer use CliScan
	// CoCreateScanEngine( IID_IVirusProtect, (void**)&m_pVirusProtect );


    SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() == NULL ){
		AfxMessageBox(IDS_SAVRT32_DLL, MB_OK);
		return FALSE;
	}

	HRESULT hr = pSAVRT->UnProtectProcess();
	SAVASSERT( SUCCEEDED(hr) );

    ((CMainFrame*)AfxGetMainWnd())->SetStartupFlag(FALSE);


	//setup a handler that gets calls from a console window 
	//This allows us to get a graceful shutdown of the process
	//instead of allowing the system to rip the process out from under itself
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);

	return TRUE;
}


//----------------------------------------------------------------
// ExitInstance
//----------------------------------------------------------------
int CWProtect32App::ExitInstance() 
{	
	m_comCache.Flush();

	//Tell Cliscan to shut down
	//	This really should just be done on the Release of this interface,
	//	but instead I have to call this funky method with this unload param.
	if( m_pVirusProtect )
	{
		m_pVirusProtect->ControlDLL( CD_UNLOAD_ALL,0);
		m_pVirusProtect->Release();
		m_pVirusProtect = NULL;
	}

// NOTE: Win95 will IPF when returning from this method
//			if we unload the resource DLL, even if we
//			re-load the old resource handle. That being the case,
//			I am simply going to let the unload of the app remove
//			the DLL from memory.
//	if( m_hInstance )
//	{
//		FreeLibrary( m_hInstance );
//	}

	AfxGetApp()->m_pszHelpFilePath = NULL;

	DeinitDLL( AfxGetInstanceHandle() );

	ReleaseMutex(m_hMutex);

    SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() != NULL ){
		pSAVRT->SetDelete();
		HRESULT hr = pSAVRT->ProtectProcess();
		SAVASSERT( SUCCEEDED(hr) );
	}

	//unregister handler 
	//set in InitInstance()
	SetConsoleCtrlHandler(HandlerRoutine, FALSE);

	return CWinApp::ExitInstance();
}

//----------------------------------------------------------------
// RegisterOCX
//----------------------------------------------------------------
void CWProtect32App::RegisterOCX()
{
	STARTUPINFO
		si;
	PROCESS_INFORMATION
		pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	si.wShowWindow = SW_HIDE;

	if (CreateProcess("LDVPReg.exe", "", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, 10000);
		DeleteFile("LDVPReg.exe");
        CloseHandle ( pi.hThread );
        CloseHandle ( pi.hProcess );
	}
    

/*	CString			strFilename;
	WIN32_FIND_DATA	data;
	HANDLE			handle;
	LPSTR			szPathname;

	//Get the app dir
	szPathname = strFilename.GetBuffer( MAX_PATH );
	GetModuleFileName( AfxGetInstanceHandle(), szPathname, MAX_PATH );
	strFilename.ReleaseBuffer();
	// and chop off the filename		
	strFilename = strFilename.Left( strFilename.ReverseFind( '\\' ) );

	//Append a *.OCX to the app
	strFilename += "\\*.OCX";

	//Register all OCX's in my directory
	handle = FindFirstFile( (LPCTSTR)strFilename, &data );
	
	while( handle != INVALID_HANDLE_VALUE )
	{
		//and try to load and register it
		HINSTANCE hLibInst = LoadLibrary( data.cFileName );

		if( hLibInst )
		{
			if( DllRegServer = (DllRegisterServerFP)GetProcAddress( (HMODULE)hLibInst, "DllRegisterServer" ) )
			{
				if( S_OK != DllRegServer() )
				{
					CString strErr;
					AfxFormatString1( strErr, IDS_NO_LOAD_OCX, data.cFileName );
					AfxMessageBox( strErr );
				}
			}

			FreeLibrary( (HMODULE)hLibInst );
		}

		if( !FindNextFile( handle, &data ) )
		{
			handle = INVALID_HANDLE_VALUE;
		}
	}
*/
}		


//----------------------------------------------------------------
// OnAppAbout
//----------------------------------------------------------------
void CWProtect32App::OnAppAbout()
{
	CRect			rect( 0,0,0,0);
	CLDVPAboutDlg	AboutDlg;

	if( AboutDlg.Create( "", WS_CHILD | WS_VISIBLE,
					rect, AfxGetMainWnd(), ID_ABOUT_DLG,
					NULL, FALSE ) )
	{
		AboutDlg.SetType(0);
		AboutDlg.ShowAboutDialog();
	}
}



void CWProtect32App::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
    // Just hook this to disable F1 help in the main UI
}
