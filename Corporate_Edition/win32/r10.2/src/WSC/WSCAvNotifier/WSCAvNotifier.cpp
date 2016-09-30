// WSCAvNotifier.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"
#include "WSCAvNotifier.h"
#include "vpcommon.h"
#include "ClientReg.h"
#include "RegUtils.h"
#include "SymSaferStrings.h"
#include "SrtValues.h"
#include "AdminInfo.h"
#include "vpexceptionhandling.h"
#include "SymSaferRegistry.h"
#include "ControlAP.h"
#include "shellapi.h"

#include <string>
#include <atlstr.h>

#define MAX_LOADSTRING 100
#define CONTROLAP_EXE_NAME  "ControlAP.exe"
#define MAX_WAIT_FOR_CONTROLAP_MSEC  1000

WINOLEAPI CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#define ENABLEFLAG "/enable"
#define UPDATEFLAG "/update"
#define ANTIVIRUSCOMPONENT "/antivirus"
#define ANTISPYWARECOMPONENT "/antispyware"
#define FIREWALLCOMPONENT "/firewall"

typedef enum ComponentType {
    SYM_ANTISPYWARE,
    SYM_ANTIVIRUS,
    SYM_FIREWALL
};

void ParseCommandLineForFlags( LPTSTR lpCmdLine, bool* enable, bool* update, ComponentType *type )
{
    // Convert the command line all to lower case
    _strlwr( lpCmdLine );

    *enable = false;
    *update = false;
    *type   = SYM_ANTISPYWARE;

    // Find /enable switch
    if ( _tcsstr(lpCmdLine, ENABLEFLAG) != NULL ) 
        *enable = true;
    
    // Find /update switch
    if ( _tcsstr(lpCmdLine, UPDATEFLAG) != NULL ) 
        *update = true;

    // Find /component switches
    if ( _tcsstr(lpCmdLine, ANTIVIRUSCOMPONENT) != NULL )
        *type = SYM_ANTIVIRUS;

    if ( _tcsstr(lpCmdLine, ANTISPYWARECOMPONENT) != NULL )
        *type = SYM_ANTISPYWARE;
 
    if ( _tcsstr(lpCmdLine, FIREWALLCOMPONENT) != NULL )
        *type = SYM_FIREWALL;
}

void ExecuteControlAP( bool tunOnAP, bool turnOnSecurityRiskScanning )
{
	// Run ControlAP to toggle AP.
	// Load the path to the SAV dir -- we don't want to run just any ControlAP.exe!
	std::string strControlAPPath;
	HKEY		hMainKey = NULL;
	DWORD		dwRet;

	dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						 szReg_Key_Main,
						 0,
						 KEY_READ,
						 &hMainKey);
	if (dwRet != ERROR_SUCCESS)
	{
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_APP_TITLE);
			strError.LoadString(IDS_ENABLE_ERROR);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	}
	TCHAR szSAVPath[MAX_PATH + 1] = {0};
	DWORD dwSize = sizeof(szSAVPath);

	dwRet = SymSaferRegQueryStringValue(hMainKey,
										szReg_Val_LocalAppDir,
										szSAVPath,
										&dwSize);
	RegCloseKey(hMainKey);
	hMainKey = NULL;
	if (dwRet != ERROR_SUCCESS)
	{
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_APP_TITLE);
			strError.LoadString(IDS_ENABLE_ERROR);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	}
	// Create the executable name.
	try
	{
		strControlAPPath = szSAVPath;
		if(*(strControlAPPath.rbegin()) != _T('\\'))	// if last char is not delim
			strControlAPPath.append(1, _T('\\'));		// append delim
		strControlAPPath += _T(CONTROLAP_EXE_NAME);
	}
	VP_CATCH_MEMORYEXCEPTIONS
	(
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_APP_TITLE);
			strError.LoadString(IDS_ENABLE_ERROR);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	)

    // Create the parameters to pass to ControlAP
    TCHAR params[256];
    memset( params, 0, sizeof(params));
    if ( tunOnAP ) {
        ssStrnAppend(params, CONTROLAP_PARAM_ENABLE_AP, sizeof(params));
        ssStrnAppend(params, " ", sizeof(params));
    }

    if ( turnOnSecurityRiskScanning ) {
        ssStrnAppend(params, CONTROLAP_PARAM_ENABLE_RISKSCAN, sizeof(params));
    }

	// Now create the ControlAP process.
	SHELLEXECUTEINFO sExecInfo;

	ZeroMemory(&sExecInfo, sizeof(sExecInfo));
	sExecInfo.cbSize = sizeof(sExecInfo);
	sExecInfo.fMask  = SEE_MASK_NOCLOSEPROCESS; // ask to have sExecInfo.hProcess filled in
	sExecInfo.lpVerb = _T("open"); // run the specified process
	sExecInfo.lpFile = _T(CONTROLAP_EXE_NAME);//strControlAPPath.c_str();
	sExecInfo.lpParameters = _T(params);
	sExecInfo.lpDirectory  = szSAVPath;
	sExecInfo.nShow		   = SW_HIDE;

	if (!ShellExecuteEx(&sExecInfo))
	{
		DWORD	dwError = GetLastError();
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_APP_TITLE);
			strError.LoadString(IDS_ENABLE_ERROR);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	}
	// Wait for a return value from ControlAP.
	DWORD dwExitCode	= 0;
	bool  bControlAPRan = true;

	SAVASSERT(sExecInfo.hProcess != NULL);
	if (WaitForSingleObject(sExecInfo.hProcess, MAX_WAIT_FOR_CONTROLAP_MSEC) != WAIT_OBJECT_0 ||
		!GetExitCodeProcess(sExecInfo.hProcess, &dwExitCode))
	{
		// We couldn't get ControlAP's return value.
		// It might have worked, so just exit silently.
		bControlAPRan = false;
	}
	CloseHandle(sExecInfo.hProcess);
	if (!bControlAPRan)
		return;

    try 
    {
        CString strTitle;
        CString strError;
	    if (dwExitCode == -1)
	    {
    	    strTitle.LoadString(IDS_APP_TITLE);
		    strError.LoadString(IDS_ENABLE_ERROR);
		    MessageBox(NULL, strError, strTitle, MB_OK);
	    }
        else if (dwExitCode == -2)
        {
            strTitle.LoadString(IDS_APP_TITLE);
            strError.LoadString(IDS_ENABLE_LOCKED);
            MessageBox(NULL, strError, strTitle, MB_OK);
        }
    }
    VP_CATCH_MEMORYEXCEPTIONS(;)
    return;
}

bool StartRtvscanService()
{
    DWORD ret = ERROR_ACCESS_DENIED;
    IVirusProtect *pVirusProtect;
	IServiceControl *pServiceControl;
    GUID clsid_cliproxy         = _CLSID_CliProxy;
    GUID iid_ivirusprotect      = _IID_IVirusProtect;
    GUID iid_iservicecontrol    = _IID_IServiceControl;
    CString str;

    long val = 0;
    bool status = true;

    HRESULT hr = CoCreateInstance( clsid_cliproxy, NULL, CLSCTX_INPROC_SERVER, iid_ivirusprotect, (void**)&pVirusProtect );
	if (SUCCEEDED(hr))
	{
        // Get reference to IServiceControl
        hr = pVirusProtect->CreateByIID( iid_iservicecontrol, reinterpret_cast<void**> (&pServiceControl) );

        if ( SUCCEEDED(hr) ) {
            ret = pServiceControl->Start();
		    pServiceControl->Release();

            if ( ret != ERROR_SUCCESS ) 
            {
                str.LoadString(IDS_ENABLE_ERROR);
                ::MessageBox(NULL, str, SERVICE_NAME, MB_OK);
                status = false;
            }
        }

        pVirusProtect->Release();
	}
    else {
        str.LoadString(IDS_ENABLE_ERROR);
        ::MessageBox(NULL, str, SERVICE_NAME, MB_OK);
        status = false;
    }

    return status;
}

bool IsAPLocked()
{
    bool status = false;    // Assume that it is not locked
    HKEY hRealTimeKey;
    TCHAR szSubKey[MAX_PATH];
    TCHAR szLocked[MAX_PATH];
    DWORD dwRet;
    CString str;

    memset(szSubKey, 0, MAX_PATH * sizeof(TCHAR));
    memset(szLocked, 0, MAX_PATH * sizeof(TCHAR));

	ssStrnAppend(szSubKey, szReg_Key_Main "\\" szReg_Key_Storages "\\" szReg_Key_Storage_File "\\" szReg_Key_Storage_RealTime, sizeof(szSubKey));
    ssStrnAppend(szLocked, szReg_Val_RTSScanOnOff, sizeof(szLocked));
    ssStrnAppend(szLocked, "-L", sizeof(szLocked));

    dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,szSubKey,NULL,KEY_READ,&hRealTimeKey);

    if( dwRet != ERROR_SUCCESS ) {
        str.LoadString(IDS_ENABLE_ERROR);
        ::MessageBox(NULL, str, SERVICE_NAME, MB_OK);
        return true;    // Unable to open key, hence the key might actually be locked
    }

    // Check if key is locked
    dwRet = GetVal( hRealTimeKey, szLocked, 0);

    // If registry is locked then display a message
    if ( dwRet == 1 ) {
        str.LoadString(IDS_ENABLE_LOCKED);
        ::MessageBox( NULL, str, SERVICE_NAME,  MB_OK );
        status = true;
    }
    
    RegCloseKey(hRealTimeKey);
    return status;
}

bool IsSecurityRisksScanLocked()
{
    bool status = false;
    HKEY hRealTimeKey;
    TCHAR szSubKey[MAX_PATH];
    TCHAR szLocked[MAX_PATH];
    CString str;

	memset(szSubKey, 0, MAX_PATH * sizeof(TCHAR));
    memset(szLocked, 0, MAX_PATH * sizeof(TCHAR));
	ssStrnAppend(szSubKey, szReg_Key_Main "\\" szReg_Key_Storages "\\" szReg_Key_Storage_File "\\" szReg_Key_Storage_RealTime, sizeof(szSubKey));
    ssStrnAppend(szLocked, szReg_Val_RespondToThreats, sizeof(szLocked));
    ssStrnAppend(szLocked, "-L", sizeof(szLocked));


    DWORD dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,szSubKey,NULL,KEY_READ, &hRealTimeKey);

    if( dwRet != ERROR_SUCCESS ) {
        str.LoadString(IDS_ENABLE_ERROR);
        ::MessageBox( NULL, str, SERVICE_NAME,  MB_OK );
        return true; // Unable to open key, hence the key might actually be locked
    }

    // First lets check if we can modify this value
    dwRet = GetVal( hRealTimeKey, szLocked, 0);
    
    // If registry is locked then display a message
    if ( dwRet == 1 ) {
        str.LoadString(IDS_ENABLE_LOCKED);
        ::MessageBox( NULL, str, SERVICE_NAME, MB_OK );
        status = true;
    }

    RegCloseKey(hRealTimeKey);
    return status;
}

void DoLiveUpdate()
{
    TCHAR szSubKey[MAX_PATH];
	HKEY	hKey = NULL;
    DWORD   dwVal;

	PROCESS_INFORMATION processInfo;
	STARTUPINFO			infoStartup;
	memset( &infoStartup, '\0', sizeof( STARTUPINFO ) );
	infoStartup.cb = sizeof( STARTUPINFO );

    CAdminInfo  AdminInfo;
    IUtil*      pUtil = NULL;
    BOOL        bEnableAllUsers = 0;
    BOOL        bNormalUser = 0;
    BOOL        bLockUpdate = FALSE;

    CString liveUpdate;
    liveUpdate.LoadString(IDS_LIVEUPDATE_STR);

    GUID clsid_cliproxy      = _CLSID_CliProxy;
    GUID iid_iutil           = _IID_IUtil;
    GUID iid_ivirusprotect   = _IID_IVirusProtect;

    // See if we're a normal user (i.e. without reg write access)
    bNormalUser = !AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );
    
	if (!bNormalUser)
		AdminInfo.IsProcessNormalUser( &bNormalUser );

    memset(szSubKey, 0, MAX_PATH * sizeof(TCHAR));
    ssStrnAppend(szSubKey, szReg_Key_Main "\\" szReg_Key_PatternManager, sizeof(szSubKey));

	if( ERROR_SUCCESS != RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szSubKey,
										0,
										KEY_READ,
										&hKey) )
	{

        // TODO: Pop up a message. Although earlier, this case was never handled
        return;
    }


    // Get the value of LockUpdateManager
    if ( ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey, szReg_Val_LockUpdatePattern, 
                                            &dwVal ) )
    {
        if ( dwVal == 1 ) {
            bLockUpdate = TRUE;
        }
    }

   
    // TC: STS Defect 366765
    // 
    // Get value of PatternManager/EnableAllUsers
    if(ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey,
						                    szReg_Val_EnableAllUsers,
						                    &dwVal ))
    {
        if ( dwVal == 1 )
        {
            bEnableAllUsers = TRUE;
        }
    }

    // Do LiveUpdate Only if the functionality is not locked
    if ( bLockUpdate != TRUE ) 
    {
        // If Restricted User and is allowed to update
        if ( bNormalUser && bEnableAllUsers )
        {
            // TC: STS Defect 366765
            // 
            // If we're running as a restricted user and EnableAllUsers is set, then
            // start LiveUpdate.

		    // Vista change: since LiveUpdate will be run through rtvscan service, it will be run silently.
		    // We need to ask for User verification here before we proceed.

            CString runSilentMsg;
            runSilentMsg.LoadString(IDS_LIVEUPDATE_SILENT);
    		
		    int ret = ::MessageBox(NULL, runSilentMsg, liveUpdate, MB_OKCANCEL );
            
		    if ( ret == IDOK ) 
		    {
			    // Create an instance of the IUtil interface from Cliproxy
                IVirusProtect   *pVirusProtect = NULL;

                HRESULT hr = CoCreateInstance( clsid_cliproxy, NULL, CLSCTX_INPROC_SERVER, iid_ivirusprotect, (void**)&pVirusProtect );

                if( SUCCEEDED( hr ) )
                {
                    hr = pVirusProtect->CreateByIID( iid_iutil, reinterpret_cast<void**> (&pUtil) );
                    pVirusProtect->Release();

                    if ( hr == S_OK )
			        {        
				        // Tell RTVScan to start LiveUpdate
				        pUtil->LaunchLU();

				        // Clean up
				        pUtil->Release();
				        pUtil = NULL;
                    }
			    }
			    else
			    {
				    // No Cliproxy/RTVScan communication
                    CString strError;
                    strError.LoadString(IDS_LIVEUPDATE_ERROR);
        			
                    ::MessageBox( NULL, strError, liveUpdate, MB_OK );
			    }
		    }
        }

        // For Admin User, we can launch LiveUpdate directly
        else
        {
            // set up filter format expected by vpdn_lu.exe, which will
            // process it into format expected by navlu.dll.
            try
            {
                // Let's determine the location of SAV Install Directory
                TCHAR           path[MAX_PATH] = {0};
                DWORD           dwSize = sizeof(path);
                LRESULT         lRet = ERROR_SUCCESS;
                HKEY            hKeyHome = NULL;
                BOOL            status = FALSE;
                
                lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szReg_Key_Main, 0, KEY_READ , &hKeyHome );

                if ( ERROR_SUCCESS == lRet )
                {
                    lRet = SymSaferRegQueryValueEx(hKeyHome, szReg_Val_HomeDir, NULL, NULL, (LPBYTE) path, &dwSize);
                    RegCloseKey( hKeyHome );

                    // Now append the name of the file
                    ssStrnAppendFile (path, "vpdn_lu.exe", MAX_PATH);
                }

                // should the user download product updates?
		        if( ERROR_SUCCESS != SymSaferRegQueryDWORDValue(hKey,
						                            szReg_Val_EnableProductUpdates,
						                            &dwVal ) ||
				    dwVal == 0 )
                {
				    // filter products
                    ssStrnAppend(path, " /fUpdate", MAX_PATH);
                }

			    // should the user download virusdefs?
			    dwVal = 0; // reset
		        if( ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey,
						                            szReg_Val_DisableVirusDefUpdates,
						                            &dwVal ) &&
				    dwVal != 0 )
                {
				    // filter content updates
				    ssStrnAppend(path, " /fVirusDef", MAX_PATH);
                }

                // should the user download SCF content (Trojans and IDSs)?
			    dwVal = 0; // reset
		        if( ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey,
						                            szReg_Val_DisableSCFContentUpdates,
						                            &dwVal ) &&
				    dwVal != 0 )
                {
				    // filter content updates
                    ssStrnAppend(path, " /fSCFContent", MAX_PATH);
                }

                // Note: double-quotes already placed around application name in command line.
		        // Invoke VPDN_LU.EXE with the cmd line that we just built.
	            if( !CreateProcess(
						            NULL,
						            path,
						            NULL,
						            NULL,
						            FALSE,
						            CREATE_SEPARATE_WOW_VDM,
						            NULL,
						            NULL,
						            &infoStartup,
						            &processInfo ) )
	            {
                    CString strError;
                    strError.LoadString(IDS_LIVEUPDATE_ERROR2);
                    ::MessageBox( NULL, strError, liveUpdate, MB_OK);
	            }
                else
                {
                    CloseHandle( processInfo.hThread );
                    CloseHandle( processInfo.hProcess );
                }
            }
            VP_CATCH_MEMORYEXCEPTIONS(;)
        }
    }

    // Display message notifying the user that the action is locked by Administrator
    else {
        CString error;
        error.LoadString(IDS_UPDATE_LOCKED);
        ::MessageBox( NULL, error, SERVICE_NAME,  MB_OK );
    }

    if ( NULL != hKey )
    {
		RegCloseKey( hKey );
        hKey = NULL;
    }
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

    // Parse the command line
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED );
    bool enable, update;
    ComponentType type = SYM_ANTISPYWARE;

    ParseCommandLineForFlags(lpCmdLine, &enable, &update, &type);

    if ( enable ) 
    {
        // Enable AntiVirus
        if ( type == SYM_ANTIVIRUS ) {
            if ( StartRtvscanService() ) {
                if ( !IsAPLocked() ) {
                    ExecuteControlAP( true, false );
                }
            }

        }

        // Enable AntiSpyware
        else if ( type == SYM_ANTISPYWARE ) 
        {
            if ( StartRtvscanService() ) {
                if ( !IsAPLocked() && !IsSecurityRisksScanLocked() ) {
                    ExecuteControlAP( true, true );
                }
            }
        }

        // Enable Firewall
        else if ( type == SYM_FIREWALL ) 
        {
            // TODO: Needs to be implemented later
        }
    }

    // LiveUpdate is the same for all components
    if ( update )
        DoLiveUpdate();

    CoUninitialize();
	return 0;

}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WSCAVNOTIFIER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_WSCAVNOTIFIER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
