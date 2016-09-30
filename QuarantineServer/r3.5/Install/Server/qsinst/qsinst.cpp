// qsinst.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "tchar.h"
#include "resource.h"
#include "winsvc.h"
#include "qsregval.h"
#include "qsinst.h"
#include "CServiceInstall.h"
#include "IntelSharedFiles.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// WINSOCK STUFF
#include  <winsock.h>
#include "SymSaferRegistry.h"

///////////////////////////////////////////////////////////////////////////////
//
// Locals
//
///////////////////////////////////////////////////////////////////////////////

HINSTANCE g_hInstance = NULL;
HMODULE g_hQsPakModule = NULL;
HWND g_hWnd = NULL;

typedef ULONG (*PFNISQSERVERFILE)( const char * pszFileName );
PFNISQSERVERFILE g_pfnIsQserverFile;

static WSADATA WinsockData;
static BOOL bWsockOpen = FALSE;

// PROTOTYPES
bool RunProcess(char * sCommandLine);
static int FilesInQuarantine( HWND hWnd, LPSTR szFolder );
static BOOL IsQserverFile( const char * pszFileName );
void CleanupRegistry(void);
extern "C" LONG __declspec( dllexport ) UninstallIcePackServices( LPSTR pszQFolder );
WORD IsIpAddress( const char * pszAddress );
void UnInstallAMSConsole();
extern "C" BOOL __declspec( dllexport )  RemoveDirectoryTree ( LPCTSTR lpPathName );
BOOL GetInstallFolder( LPTSTR lpBuffer, DWORD dwSize );
void CleanupInstallPath();

DWORD (*lpfnInstallAMS2)(LPCTSTR, LPTSTR, LPTSTR, DWORD, DWORD, BOOL );

// Possible return values for IsIpAddress()
enum
{
    IS_IPADDRESS,
    IS_NOT_IPADDRESS,
    IS_INVALID_IPADDRESS
};


///////////////////////////////////////////////////////////////////////////////
//
// Function name: DllMain
//
// Description  :
//
// Return type  : BOOL APIENTRY
//
// Argument     : HANDLE hModule
// Argument     : DWORD  ul_reason_for_call
// Argument     : LPVOID lpReserved
//
///////////////////////////////////////////////////////////////////////////////
// 4/27/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    switch( ul_reason_for_call )
        {
        case DLL_PROCESS_ATTACH:
            g_hInstance = (HINSTANCE) hModule;
            break;

        }

    return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: QServerInstStopService
//
// Description  : Stops the specified service.
//
// Return type  : LONG
//
// Argument     : LPSTR szService
//
///////////////////////////////////////////////////////////////////////////////
// 4/27/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) QServerInstStopService( LPSTR szService )
{
    //
    // Open the SCM for this machine.
    //
    SC_HANDLE hSCM = OpenSCManager( NULL, NULL, GENERIC_READ | GENERIC_WRITE );
    if( hSCM == NULL )
        return -1;

    //
    // Stop the service in question.
    //
    SC_HANDLE hService = OpenService( hSCM, szService, SERVICE_STOP | SERVICE_QUERY_STATUS );
    if( hService )
        {
        SERVICE_STATUS status;
        ZeroMemory(&status, sizeof( SERVICE_STATUS ) );
        if( ControlService( hService, SERVICE_CONTROL_STOP, &status ) )
            {
            //
            // Service is stopping.  Wait here until it stops.
            //
            while( status.dwCurrentState != SERVICE_STOPPED )
                {
                //
                // Sleep until we should check again.
                //
                Sleep( status.dwWaitHint );

                //
                // Check for status once again.
                //
                ZeroMemory( &status, sizeof( SERVICE_STATUS ) );
                QueryServiceStatus( hService, &status );
                }
            }
        }

    //
    // Cleanup
    //
    if( hSCM )
        CloseServiceHandle( hSCM );
    if( hService )
        CloseServiceHandle( hService );

    //
    // Were done.
    //
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QServerInstStartService
//
// Description  : Stops the specified service.
//
// Return type  : LONG
//
// Argument     : LPSTR szService
//
///////////////////////////////////////////////////////////////////////////////
// 4/27/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) QServerInstStartService( LPSTR szService )
{
    BOOL bStarted = FALSE;
    //
    // Open the SCM for this machine.
    //
    SC_HANDLE hSCM = OpenSCManager( NULL, NULL, GENERIC_READ | GENERIC_WRITE );
    if( hSCM == NULL )
        return -1;

    //
    // Start the service in question.
    //
    SC_HANDLE hService = OpenService( hSCM, szService, SERVICE_START );
    if( hService )
        {
        bStarted = StartService( hService, 0, NULL );
        }

    //
    // Cleanup
    //
    if( hSCM )
        CloseServiceHandle( hSCM );
    if( hService )
        CloseServiceHandle( hService );

    //
    // Were done.
    //
    return bStarted ? 0 : -1;
}


bool RunProcess(char * sCommandLine)
{

    SECURITY_DESCRIPTOR  sdSec;

    InitializeSecurityDescriptor (&sdSec, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl (&sdSec, TRUE, (PACL) NULL, FALSE);

    STARTUPINFO          si;
    // Only if we get the default desktop name do the following
    memset (&si, 0, sizeof (si));

    si.lpDesktop = TEXT("WinSta0\\Default");

    SECURITY_ATTRIBUTES  saThread, saProcess;

    memset (&saProcess, 0, sizeof (saProcess));
    memset (&saThread,  0, sizeof (saThread));

    saThread.lpSecurityDescriptor  = &sdSec;
    saProcess.lpSecurityDescriptor = &sdSec;

    // Set the startup Info.
    si.cb = sizeof (si);
    si.dwFlags = 0;
    si.wShowWindow = SW_SHOWNORMAL;

    saProcess.nLength = sizeof (saProcess);
    saProcess.bInheritHandle = TRUE;

    saThread.nLength = sizeof (saThread);
    saThread.bInheritHandle = TRUE;

    PROCESS_INFORMATION  ProcessInformation;

    DWORD dwExitCode = CreateProcess ( NULL,
                        sCommandLine,
                        &saProcess,
                        &saThread,
                        TRUE,
                        HIGH_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
                        NULL,
                        NULL,
                        &si,
                        &ProcessInformation);
    if (!dwExitCode)
    {

        switch (GetLastError())
        {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
            case ERROR_INVALID_NAME:
            break;

        }
		return false;
    }
    else
    {
        // wait for the process to finish
        WaitForSingleObject(ProcessInformation.hProcess, INFINITE );
        CloseHandle ( ProcessInformation.hThread );
        CloseHandle ( ProcessInformation.hProcess );
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: UninstInitialize
//
// Description  :
//
// Return type  : LONG
//
// Argument     : HWND hWnd
// Argument     : HANDLE hInstance
// Argument     : LONG lReserved
//
///////////////////////////////////////////////////////////////////////////////
// 4/27/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) UninstInitialize( HWND hWnd/*, HANDLE hInstance, LONG lReserved*/ )
{

    char  szUninstall[ MAX_PATH ];
    char  szError[ MAX_PATH * 2 ];
    TCHAR szQFolder[ MAX_PATH + 1 ];
    HKEY hKey;
    int iRet = 0;
    TCHAR szInstallPath[ MAX_PATH + 1 ];
    ULONG lLength;
    DWORD buffer = 0;

    if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, QSERVER_REGKEY, 0, KEY_READ, &hKey ) )
        return -1;

    ZeroMemory( szInstallPath, sizeof(szInstallPath) );

    lLength = MAX_PATH;
    SymSaferRegQueryValueEx( hKey, INSTALLPATH_REGVALUE, 0, NULL, (LPBYTE)szInstallPath, &lLength );

    if ( (!szInstallPath[0]) )
        return -1;

    RegCloseKey( hKey );


    ZeroMemory( szQFolder, MAX_PATH +1 );
    LoadString( g_hInstance, IDS_UNINSTALL, szUninstall, MAX_PATH );

    g_hWnd = hWnd;

    //
    // Stop the service.
    //
    if( QServerInstStopService( "IcePack" ) == -1 )
        {
        //
        // The user does not have rights to the SCM on this machine.
        //
        LoadString( g_hInstance, IDS_NO_RIGHTS, szError, MAX_PATH * 2);
        MessageBox( hWnd, szError, szUninstall, MB_ICONSTOP | MB_OK );
        return -1;
        }

    //
    // Stop the service.
    //
    if( QServerInstStopService( "ScanExplicit" ) == -1 )
        {
        //
        // The user does not have rights to the SCM on this machine.
        //
        LoadString( g_hInstance, IDS_NO_RIGHTS, szError, MAX_PATH * 2);
        MessageBox( hWnd, szError, szUninstall, MB_ICONSTOP | MB_OK );
        return -1;
        }

    //
    // Stop the service.
    //
    if( QServerInstStopService( "QServer" ) == -1 )
        {
        //
        // The user does not have rights to the SCM on this machine.
        //
        LoadString( g_hInstance, IDS_NO_RIGHTS, szError, MAX_PATH * 2);
        MessageBox( hWnd, szError, szUninstall, MB_ICONSTOP | MB_OK );
        return -1;
        }

    //
    // Unregister and remove the service.
    //
    strcat(szInstallPath, "\\qserver.exe /unregserver");
    if( RunProcess(szInstallPath) == false )
        {
        //
        // The user does not have rights to the SCM on this machine.
        //
        LoadString( g_hInstance, IDS_NO_RIGHTS, szError, MAX_PATH * 2);
        MessageBox( hWnd, szError, szUninstall, MB_ICONSTOP | MB_OK );
        return -1;
        }

    //
    // Check to see if there are any files in the quarantine folder.
    //

    iRet = 0;
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, 0, KEY_READ, &hKey ) )
        {
        //
        // Fetch the quarantine folder from the registry.
        //
        ULONG lLength = MAX_PATH;
        if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey,
                            REGVALUE_QUARANTINE_FOLDER,
                            0,
                            NULL,
                            (LPBYTE)szQFolder,
                            &lLength ) )
            {
            //
            // Prompt user if neccessary for removal of quarantine files.
            //
            if( FilesInQuarantine( hWnd, szQFolder ) == IDCANCEL )
                iRet = -1;
            }
        RegCloseKey( hKey );
        }

    //
    // Now that we are done, unload QsPak DLL
    //
    if( g_hQsPakModule )
        {
        FreeLibrary( g_hQsPakModule );
        g_hQsPakModule = NULL;
        }

    // Uninstall the ScanExplicit and IcePack services
    ZeroMemory( szQFolder, sizeof(szQFolder) );
    GetInstallFolder( szQFolder, sizeof(szQFolder) );

    UninstallIcePackServices( szQFolder );

    //
    // Uninstall AMS
    //
    UnInstallAMSConsole();

    return iRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: UninstUnInitialize
//
// Description  :
//
// Return type  : LONG
//
// Argument     : HWND hWnd
// Argument     : HANDLE hInstance
// Argument     : LONG lReserved
//
///////////////////////////////////////////////////////////////////////////////
// 4/27/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) UninstUnInitialize( HWND hWnd/*, HANDLE hInstance, LONG lReserved*/ )
{
    TCHAR  szIntelFile   [ MAX_PATH + 1 ];
    TCHAR  szSystemPath  [ MAX_PATH + 1 ];

    CleanupInstallPath();

    CleanupRegistry();

	if ( GetWindowsSystemFolder( szSystemPath, MAX_PATH) != NULL )
	{
		_tcscat ( szSystemPath, TEXT("\\"));

		_tcscpy (szIntelFile, szSystemPath);
		_tcscat ( szIntelFile, TEXT("CBA.DLL"));
		UpdateIntelSharedFileCount ( szIntelFile, ID_INTELFILES_QSERVER, FALSE);

		_tcscpy (szIntelFile, szSystemPath);
		_tcscat ( szIntelFile, TEXT("NTS.DLL"));
		UpdateIntelSharedFileCount ( szIntelFile, ID_INTELFILES_QSERVER, FALSE);

		_tcscpy (szIntelFile, szSystemPath);
		_tcscat ( szIntelFile, TEXT("PDS.DLL"));
		UpdateIntelSharedFileCount ( szIntelFile, ID_INTELFILES_QSERVER, FALSE);

		_tcscpy (szIntelFile, szSystemPath);
		_tcscat ( szIntelFile, TEXT("MSGSYS.DLL"));
		UpdateIntelSharedFileCount ( szIntelFile, ID_INTELFILES_QSERVER, FALSE);
	}

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: FilesInQuarantine
//
// Description  :
//
// Return type  : int
//
// Argument     : HWND hWnd
//
///////////////////////////////////////////////////////////////////////////////
// 4/27/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
int FilesInQuarantine( HWND hWnd, LPSTR szFolder )
{
    int iRet = IDOK;
    BOOL bAskedAndAnswered = FALSE;

    //
    // Q: Are there any files in the quarantine folder?
    //
    char szTemp[ MAX_PATH + 5];
    lstrcpy( szTemp, szFolder );
    lstrcat( szTemp, "\\*." ); // Quarantine files have no extension.

    WIN32_FIND_DATA findData;
    ZeroMemory( &findData, sizeof( WIN32_FIND_DATA ) );
    HANDLE hFind = FindFirstFile( szTemp, &findData );
    if( hFind != INVALID_HANDLE_VALUE )
        {
        do
            {
            //
            // Skip other folders.
            //
            if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                continue;

            //
            // Make sure this is a quarantine file.
            //
            wsprintf( szTemp, _T( "%s\\%s" ), szFolder, findData.cFileName );
            if( IsQserverFile( szTemp ) )
                {
                if( !bAskedAndAnswered )
                    {
                    //
                    // Tell user what is going on.
                    //
                    char szTitle[ MAX_PATH ];
                    char szText[ MAX_PATH *2 ];
                    LoadString( g_hInstance, IDS_UNINSTALL, szTitle, MAX_PATH );
                    LoadString( g_hInstance, IDS_REMOVE_QFILES, szText, MAX_PATH *2 );

                    iRet = MessageBox( hWnd, szText, szTitle, MB_YESNO | MB_ICONQUESTION );
                    if( iRet != IDYES )
                        break;

                    bAskedAndAnswered = TRUE;
                    }

                //
                // Nuke the file.
                //
                ::DeleteFile( szTemp );
                }
            }
        while( FindNextFile( hFind, &findData ) );

        //
        // Unless user decided to keep the quarantine folder mark it for deletion
        //
        if( (iRet == IDYES) || (iRet == IDOK) )
            {
            HKEY hKey;
            DWORD buffer = 1;
            if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, 0, KEY_WRITE, &hKey ) )
                {
                RegSetValueEx( hKey, QFOLDER_DELETE_REGVALUE, 0, REG_DWORD, (CONST BYTE *)&buffer, sizeof(buffer) );
                RegCloseKey( hKey );
                }
            }

        //
        // Cleanup.
        //
        FindClose( hFind );
        }


    return iRet;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsQserverFile
//
// Description  :
//
// Return type  : BOOL
//
// Argument     : const char * pszFileName
//
///////////////////////////////////////////////////////////////////////////////
// 4/27/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
BOOL IsQserverFile( const char * pszFileName )
{
    if( g_pfnIsQserverFile )
        return (BOOL)( 0 == g_pfnIsQserverFile( pszFileName ) );

    //
    // Figure out where we are installed.
    //
    BOOL bRet = FALSE;
    HKEY hKey;
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\qserver.exe", 0, KEY_READ, &hKey ) )
        {
        //
        // Fetch the quarantine folder from the registry.
        //
        char szInstallDir[ MAX_PATH + 1 ];
        ZeroMemory( szInstallDir, MAX_PATH +1 );
        ULONG lLength = MAX_PATH;
        if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey,
                            "Path",
                            0,
                            NULL,
                            (LPBYTE)szInstallDir,
                            &lLength ) )
            {
            //
            // Add name of packaging DLL
            //
            lstrcat( szInstallDir, "\\qspak32.dll" );
            g_hQsPakModule = LoadLibrary( szInstallDir );
            if( g_hQsPakModule )
                {
                //
                // Get function to call.
                //
                g_pfnIsQserverFile = (PFNISQSERVERFILE) GetProcAddress( g_hQsPakModule, "QsPakIsQserverFile" );
                if( g_pfnIsQserverFile )
                    bRet =(BOOL) ( 0 == g_pfnIsQserverFile( pszFileName ) );
                }
            }

        RegCloseKey( hKey );
        }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CleanupRegistry
//
// Description  : Cleans up our registry entries. We make sure that there are
//				  no SubKeys that could belong to the QConsole.  If there are
//				  no SubKeys, then we delete the Quarantine key.
//
// Return type  : None
//
// Argument     : None
///////////////////////////////////////////////////////////////////////////////
// 8/17/99 - CMCGOWEN: Function created / header added
///////////////////////////////////////////////////////////////////////////////
void CleanupRegistry()
{
	HKEY hRegKey = NULL;

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Quarantine\\Server"),
									 0L, KEY_ALL_ACCESS, &hRegKey))
	{
		RegDeleteKey(hRegKey, _T("Avis"));
		RegCloseKey(hRegKey);

		hRegKey = NULL;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Quarantine"),
									 0L, KEY_ALL_ACCESS, &hRegKey);
		RegDeleteKey(hRegKey, _T("Server"));

		DWORD dwNumSubKeys = 0;
		RegQueryInfoKey(hRegKey, NULL, NULL, NULL, &dwNumSubKeys, NULL, NULL, NULL, NULL, NULL,
						NULL, NULL);

		RegCloseKey(hRegKey);
		hRegKey = NULL;

		if(dwNumSubKeys == 0) // there were no sub keys, so delete the key.
		{
			if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), 0L,
											 KEY_ALL_ACCESS, &hRegKey))
			{
				RegDeleteKey(hRegKey, _T("Quarantine"));
				RegCloseKey(hRegKey);
			}
		}
	}

		
	return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: InstallIcePackServices
//
// Description  : Installs the IcePack and ScanExplicit services
//
// Return type  : LONG
//
// Argument     : pszTargetDir
///////////////////////////////////////////////////////////////////////////////
// 1/26/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) InstallIcePackServices( LPSTR pszTargetDir )
{
#if 0
    char szPath[ MAX_PATH + 1 ];
    char szCmdLine[ MAX_PATH + 1];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    BOOL bResult;
    DWORD dwResult;

    *szPath = 0;
    *szCmdLine = 0;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_FORCEMINIMIZE;

    lstrcpy(szPath, pszTargetDir);

    // Build command-line to ScanExplicit and execute
    lstrcpy(szCmdLine, szPath);
    lstrcat(szCmdLine, "\\");
    lstrcat(szCmdLine, "ScanExplicit.exe -install");
    bResult = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    if (!bResult)
        return -1;

    // Wait for the process to terminate
    dwResult = WaitForSingleObject(pi.hProcess, 5000);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    if (dwResult != WAIT_OBJECT_0)
        return -1;

    // Build command-line to IcePack and execute
    lstrcpy(szCmdLine, szPath);
    lstrcat(szCmdLine, "\\");
    lstrcat(szCmdLine, "IcePack.exe -install");
    bResult = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    if (!bResult)
        return -1;

    // Wait for the process to terminate
    dwResult = WaitForSingleObject(pi.hProcess, 5000);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    if (dwResult != WAIT_OBJECT_0)
        return -1;

    return 0;
#endif

    TCHAR szPath[ MAX_PATH + 1 ];

    szPath[0] = 0;
    _tcscpy( szPath, pszTargetDir );
    _tcscat( szPath, "\\ScanExplicit.exe");

    CServiceInstall scanExplicit("ScanExplicit", "Symantec Quarantine Scanner", szPath);

    scanExplicit.Install( SERVICE_WIN32_OWN_PROCESS,
                          SERVICE_AUTO_START,
                          "RPCSS\0\0");

    szPath[0] = 0;
    _tcscpy( szPath, pszTargetDir );
    _tcscat( szPath, "\\IcePack.exe");

    CServiceInstall	icePack("IcePack", "Symantec Quarantine Agent", szPath);

    icePack.Install( SERVICE_WIN32_OWN_PROCESS,
                     SERVICE_AUTO_START,
                     "qserver\0ScanExplicit\0\0");

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: UninstallIcePackServices
//
// Description  : Remove the IcePack and ScanExplicit from the SCM
//
// Return type  : LONG
//
// Argument     : pszQFolder
///////////////////////////////////////////////////////////////////////////////
// 1/26/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) UninstallIcePackServices( LPSTR pszQFolder )
{
#if 0
    char szPath[ MAX_PATH + 1 ];
    char szCmdLine[ MAX_PATH + 1];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    BOOL bResult;

    *szPath = 0;
    *szCmdLine = 0;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_FORCEMINIMIZE;

    lstrcpy(szPath, pszQFolder);

    // Build command-line to ScanExplicit and execute
    lstrcpy(szCmdLine, szPath);
    lstrcat(szCmdLine, "\\");
    lstrcat(szCmdLine, "ScanExplicit.exe -remove");
    bResult = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    if (!bResult)
        return -1;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Build command-line to IcePack and execute
    lstrcpy(szCmdLine, szPath);
    lstrcat(szCmdLine, "\\");
    lstrcat(szCmdLine, "IcePack.exe -remove");
    bResult = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    if (!bResult)
        return -1;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
#endif

    TCHAR szPath[ MAX_PATH + 1 ];

    szPath[0] = 0;
    _tcscpy( szPath, pszQFolder );
    _tcscat( szPath, "\\ScanExplicit.exe");

    CServiceInstall scanExplicit("ScanExplicit", "Symantec Quarantine Scanner", szPath);

    scanExplicit.Remove( FALSE );

    szPath[0] = 0;
    _tcscpy( szPath, pszQFolder );
    _tcscat( szPath, "\\IcePack.exe");

    CServiceInstall	icePack("IcePack", "Symantec Quarantine Agent", szPath);

    icePack.Remove( FALSE );

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: ConfirmEmailAddress
//
// Description  : Find out if the argument resembles an email address
//
// Return type  : LONG
//
// Argument     : pszEmail
///////////////////////////////////////////////////////////////////////////////
// 2/9/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) ConfirmEmailAddress( LPSTR pszEmail )
{
    // Length must not be less than 7 characters
    if ( lstrlen(pszEmail) < 7 )
        return (-1);

    // Must have a '@'
    if ( strchr(pszEmail, '@') == NULL )
        return (-1);

    // Must have atleast one '.'
    if ( strchr(pszEmail, '.') == NULL )
        return (-1);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: WSOCKInit
//
// Description  : Initialize Winsock for subsequent calls
//
// Return type  : LONG
//
// Argument     : none
///////////////////////////////////////////////////////////////////////////////
// 2/9/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) WSOCKInit()
{
    // INITIALIZE WINSOCK
    WORD wVersion = 0x0101; /* Version 1.1 */
    bWsockOpen    = FALSE;
    int iRet = WSAStartup(wVersion,&WinsockData);
    if( iRet == 0 )
        bWsockOpen = TRUE;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: WSOCKFini
//
// Description  : Shut down Winsock
//
// Return type  : LONG
//
// Argument     : none
///////////////////////////////////////////////////////////////////////////////
// 2/9/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) WSOCKFini()
{
    if( bWsockOpen )
        WSACleanup();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: ConfirmIpAddress
//
// Description  : Make sure the internet address is reachable
//
// Return type  : LONG
//
// Argument     : pszIpAddress
///////////////////////////////////////////////////////////////////////////////
// 2/9/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) ConfirmIpAddress( LPSTR pszIpAddress )
{
    char   szAddr[ MAX_PATH + 1 ];
    WORD   wResult;

    if( !bWsockOpen )
        return (-1);

    if( pszIpAddress == NULL )
        return (-1);

    lstrcpy( szAddr, pszIpAddress );

    DWORD dwWsockError = 0;
    sockaddr saDestAddr;
    sockaddr_in* pAddr = (sockaddr_in*) &saDestAddr;
    memset( &saDestAddr, 0, sizeof( sockaddr ) );
    pAddr->sin_family = PF_INET;

    wResult = IsIpAddress( szAddr );

    if( wResult == IS_INVALID_IPADDRESS )
        return (-1);

    if( wResult == IS_IPADDRESS )
    {
        // Convert x.x.x.x address to IP address
        pAddr->sin_addr.S_un.S_addr = inet_addr( szAddr );
        if( pAddr->sin_addr.S_un.S_addr == INADDR_NONE )
        {
            return (-1);
        }
    }
    else
    {
        // Try looking up server using DNS.
        struct hostent *hp = gethostbyname( szAddr );
        dwWsockError = WSAGetLastError();
        if( hp == NULL )
        {
            return (-1);
        }
    }

    return (0);
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : IsIpAddress
//
// Description   : Determines whether the argument is a valid address
//                 (xxx.xxx.xxx.xxx)
//
// Return type   : WORD
//
//                 IS_IPADDRESS
//                 IS_NOT_IPADDRESS
//                 IS_INVALID_IPADDRESS
//
// Argument      : const char * pszAddress
//
///////////////////////////////////////////////////////////////////////////////
// 12/28/98 - DBUCHES: Function created
// 02/09/00 - TNILI: Changed return type
///////////////////////////////////////////////////////////////////////////////
WORD IsIpAddress( const char * pszAddress )
{
    char szAddress[ 17 ];
    int iAddressComponents = 4;
    int iLength, i;

    //
    // Make sure no one is messing with us.
    //
    if( strstr( pszAddress, ".." ) != NULL ||
        strlen( pszAddress ) > 15 )
        return IS_NOT_IPADDRESS;

    //
    // Parse string
    //
    strncpy( szAddress, pszAddress, 17 );
    char *p = strtok( szAddress, "." );

    while( p != NULL && iAddressComponents )
        {
        //
        // Check length.
        //
        iLength = strlen( p );
        if( iLength > 3 )
            return IS_NOT_IPADDRESS;

        //
        // Make sure all characters are digits
        //
        for( i = 0; i < iLength; i++ )
            {
            if( !isdigit( p[i] ) )
                return IS_NOT_IPADDRESS;
            }

        //
        // Make sure ranges are correct.
        //
        if( atoi( p ) > 255 )
            return IS_INVALID_IPADDRESS;

        //
        // Get next token.
        //
        iAddressComponents --;
        p = strtok( NULL, "." );
        }

    //
    // If we have found 4 address components, and there are no more
    // we have a valid IP address.
    //
    if( iAddressComponents == 0 && p == NULL )
        return IS_IPADDRESS;

    //
    // Valid IP address not found.
    //
    return IS_NOT_IPADDRESS;

}

//***************************************************************************
// UnInstallAMSConsole()
//
// Description: This functions checks if AMS console had been installed
//              and uninstalls it
//
// Parameters:  None.
//
// Returns:     void
//
//***************************************************************************
//  06/11/99   Rajesh Pulinthanathu
//  02/16/00   TNILI: Modified to fit our needs
//***************************************************************************
void UnInstallAMSConsole()
{
    LONG        lRet;
    HKEY        hKeyINSTApps = 0;
    TCHAR       szAMSInstallPath [MAX_PATH+1];
    TCHAR       szAMSInstallFile [MAX_PATH+1];
    TCHAR       szOriginalCWD [MAX_PATH+1];
    HINSTANCE   hInstAMS = NULL;
    TCHAR       szAMSUnInstallStatus[MAX_STR_LEN+1];
    int         nAMSStatusLen = MAX_STR_LEN * sizeof(TCHAR);
    DWORD       nResult = 0 ;
    DWORD       dwAMSInstalled;
    HKEY        hKey = 0;
    DWORD		dwType;
    DWORD		dwDataLen;
    TCHAR       szMessage [MAX_PATH*2+1] = {0};
    TCHAR       szCaption [MAX_PATH+1];
    // PR

    // Check if AMS Client is installed
    lRet = RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                          QSERVER_REGKEY ,
                          0,
                          KEY_READ,
                          &hKey );
    if (ERROR_SUCCESS != lRet)
        goto ExitFunction;

    dwAMSInstalled = 0 ;
    dwDataLen = sizeof(DWORD);

    SymSaferRegQueryValueEx ( hKey, AMSINSTALLED_REGVALUE,
                      NULL, &dwType,
                      (LPBYTE) &dwAMSInstalled, &dwDataLen );

    ZeroMemory (szAMSInstallPath, sizeof(szAMSInstallPath));
    dwDataLen = sizeof(szAMSInstallPath);

    SymSaferRegQueryValueEx ( hKey, AMSINSTALLPATH_REGVALUE,
                      NULL, &dwType,
                      (LPBYTE) szAMSInstallPath, &dwDataLen );

    if (hKey)
        RegCloseKey (hKey);

    if (dwAMSInstalled != 1 )
        goto ExitFunction;

	 //	Comment out the "Uninstall AMS" question" code below, because AMS will always be uninstalled
/*
    LoadString(g_hInstance, IDS_UNINSTALL_AMSCLIENT, szMessage, sizeof(szMessage));
 	 LoadString(g_hInstance, IDS_UNINSTALL_CAPTION, szCaption, sizeof(szCaption));
    lRet = MessageBox(g_hWnd, szMessage, szCaption, MB_YESNO);
    if (IDYES != lRet)
        goto ExitFunction;
*/
	ZeroMemory (szAMSUnInstallStatus, nAMSStatusLen);

    // Save Current Working Directory and change to AMS2\WINNT folder
    GetCurrentDirectory( MAX_PATH, szOriginalCWD );

    _tcscpy ( szAMSInstallFile, szAMSInstallPath);
    _tcscat ( szAMSInstallFile, TEXT("\\"));
    _tcscat ( szAMSInstallFile, AMS_INSTALL_DLL	);

    //
    //  OK, uninstall AMS Client
    //

    SetCurrentDirectory( szAMSInstallPath );

	hInstAMS = LoadLibrary( szAMSInstallFile );

    if ( NULL == hInstAMS )
    {
        LoadString(g_hInstance, IDS_ERROR_AMSCLIENT, szMessage, sizeof(szMessage));
        LoadString(g_hInstance, IDS_CAPTION_AMSCLIENT, szCaption, sizeof(szCaption));
        MessageBox(g_hWnd, szMessage, szCaption, MB_OK);
        goto ExitFunction;
    }

    lpfnInstallAMS2 = (DWORD (*)(LPCTSTR, LPTSTR, LPTSTR, DWORD, DWORD, BOOL ))GetProcAddress( hInstAMS, "InstallAMS2" );

    if ( lpfnInstallAMS2 != NULL)
    {
        nResult = lpfnInstallAMS2(szAMSInstallPath,AMS_PRODUCT_NAME, szAMSUnInstallStatus, nAMSStatusLen, AMS_FLAGS_CLIENTLITE,FALSE );

        if ( 0 != nResult )
        {
            _tcsncpy (szMessage, szAMSUnInstallStatus, sizeof(szMessage));
            LoadString (g_hInstance, IDS_ERRORCAPTION_AMSCLIENT, szCaption, sizeof(szCaption));
            MessageBox (g_hWnd, szMessage, szCaption, MB_OK);
        }
    }
    else
    {
        LoadString (g_hInstance, IDS_ERR_AMSCLIENT_INSTALLAMS, szMessage, sizeof(szMessage));
        LoadString (g_hInstance, IDS_ERRORCAPTION_AMSCLIENT, szCaption, sizeof(szCaption));
        MessageBox(NULL, szMessage, szCaption, MB_OK);
    }


ExitFunction:
    if (hKeyINSTApps)
        RegCloseKey (hKeyINSTApps);

    if (hInstAMS)
        FreeLibrary(hInstAMS);

    // Restore the current working directory.
    SetCurrentDirectory( szOriginalCWD );

}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: RemoveDirectoryTree
//
// Description  : This function uses DeleteFile, RemoveDirectory and recursion
//                to delete an entire directory tree.
//
// Return type  : BOOL
//
// Argument     : LPCTSTR
///////////////////////////////////////////////////////////////////////////////
// 3/10/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" BOOL __declspec( dllexport )  RemoveDirectoryTree ( LPCTSTR lpPathName )
{
    BOOL bResult = TRUE;
    WIN32_FIND_DATA findData;
    TCHAR szTemp[ MAX_PATH*2 + 1 ];
    DWORD dwError = ERROR_SUCCESS;

    //
    // Do some sanity checks here to avoid wiping the whole harddrive
    //
    if ( !lpPathName || !lpPathName[0] )
        return bResult;

    wsprintf( szTemp, "%s\\%s", lpPathName, "*.*" );

    ZeroMemory( &findData, sizeof( WIN32_FIND_DATA ) );

    HANDLE hFind = FindFirstFile( szTemp, &findData );
    if( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            //
            // Handle special case here
            //
            if ( !(_tcscmp( findData.cFileName, "." )) || !(_tcscmp( findData.cFileName, ".." )) )
                continue;

            //
            // Found a directory
            //
            if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                wsprintf( szTemp, "%s\\%s", lpPathName, findData.cFileName );
                RemoveDirectoryTree( szTemp );
            }

            //
            // Found a file
            //
            else if ( findData.dwFileAttributes &
                ( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL |
                FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM |
                FILE_ATTRIBUTE_ARCHIVE  | FILE_ATTRIBUTE_TEMPORARY ) )
            {
                wsprintf( szTemp, "%s\\%s", lpPathName, findData.cFileName );
                bResult = DeleteFile( szTemp );

                if (!bResult)
                    dwError = GetLastError();
            }
        }
        while( FindNextFile( hFind, &findData ) );

        FindClose( hFind );
    }

    //
    // No more files or subdirectories so go ahead and delete the folder
    //
    bResult = RemoveDirectory( lpPathName );

    if (!bResult)
        dwError = GetLastError();

    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetInstallFolder
//
// Description  : Uses GetCommandLine() and parsing to find the install path
//
// Return type  : LPTSTR
//
// Argument     : none
///////////////////////////////////////////////////////////////////////////////
// 3/10/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
BOOL GetInstallFolder( LPTSTR lpBuffer, DWORD dwSize )
{
    TCHAR szTemp[ MAX_PATH + 1 ];
    LPTSTR lpPath = GetCommandLine();

    //
    // INPUT: "C:\WINNT\IsUninst.exe -f"C:\Program Files\Symantec\Quarantine\Server\Uninst.isu"
    //        -c"C:\Program Files\Symantec\Quarantine\Server\qsinst.dll""
    //
    // OUTPUT: "C:\Program Files\Symantec\Quarantine\Server"
    //

    if ((lpPath = _tcsstr(lpPath, "-c")) == NULL)
        return FALSE;

    for (int i=0; i<3; i++)
        lpPath = CharNext(lpPath);

    _tcscpy(szTemp, lpPath);

    if ((lpPath = _tcsrchr(szTemp, '\\')) == NULL)
        return FALSE;

    *lpPath = 0;

    _tcsncpy( lpBuffer, szTemp, dwSize-1 );
    lpBuffer[dwSize-1] = 0;

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: QServerInstDeleteService
//
// Description  : This function removes an NT service
//
// Return type  : LONG
//
// Argument     : LPCTSTR
///////////////////////////////////////////////////////////////////////////////
// 4/10/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
extern "C" LONG __declspec( dllexport ) QServerInstDeleteService ( LPSTR lpName, BOOL bForce )
{
    LONG lResult = 0;

    CServiceInstall service( lpName, "", "" );

    service.Remove( bForce );

    return lResult;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CleanupInstallPath
//
// Description  : Delete directories and misc. files not affected by uninstall
//
// Return type  : none
//
// Argument     : none
///////////////////////////////////////////////////////////////////////////////
// 5/19/00 - TNILI: Function created
///////////////////////////////////////////////////////////////////////////////
void CleanupInstallPath()
{
    HKEY hKey;
    int iRet = 0;
    BOOL bResult = TRUE;
    DWORD  dwError = ERROR_SUCCESS;
    WIN32_FIND_DATA findData;
    TCHAR szQFolder[ MAX_PATH + 1 ];
    TCHAR szInstallPath[ MAX_PATH + 1 ];
    TCHAR szTemp[ MAX_PATH*2 + 1 ];
    LPTSTR lpPath;
    BOOL bQFolderFound = 0;
    ULONG lLength;
    DWORD buffer = 0;

    if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, 0, KEY_READ, &hKey ) )
        return;

    ZeroMemory( szQFolder, sizeof(szQFolder) );
    ZeroMemory( szInstallPath, sizeof(szInstallPath) );
    ZeroMemory( szTemp, sizeof(szTemp) );

    //
    // Fetch the quarantine folder from the registry.
    //
    lLength = MAX_PATH;
    SymSaferRegQueryValueEx( hKey, REGVALUE_QUARANTINE_FOLDER, 0, NULL, (LPBYTE)szQFolder, &lLength );

    lLength = MAX_PATH;
    SymSaferRegQueryValueEx( hKey, INSTALLPATH_REGVALUE, 0, NULL, (LPBYTE)szInstallPath, &lLength );

    lLength = sizeof(buffer);
    SymSaferRegQueryValueEx( hKey, QFOLDER_DELETE_REGVALUE, 0, NULL, (LPBYTE)&buffer, &lLength );

    if ( (!szQFolder[0]) || (!szInstallPath[0]) )
        return;

    RegCloseKey( hKey );

    ZeroMemory( &findData, sizeof( WIN32_FIND_DATA ) );
    wsprintf( szTemp, _T("%s\\%s"), szInstallPath, "*.*" );

    HANDLE hFind = FindFirstFile( szTemp, &findData );
    if( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            //
            // Handle special case here
            //
            if ( !(_tcscmp( findData.cFileName, "." )) || !(_tcscmp( findData.cFileName, ".." )) )
                continue;

            //
            // Found a directory
            //
            if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                wsprintf( szTemp, _T("%s\\%s"), szInstallPath, findData.cFileName );

                // Do not delete the quarantine folder!
                if ( !(_tcscmp( szTemp, szQFolder )) && (!buffer) )
                {
                    bQFolderFound = TRUE;
                    continue;
                }

                RemoveDirectoryTree( szTemp );
            }

            //
            // Found a file
            //
            else if ( findData.dwFileAttributes &
                ( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL |
                FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM |
                FILE_ATTRIBUTE_ARCHIVE  | FILE_ATTRIBUTE_TEMPORARY ) )
            {
                wsprintf( szTemp, _T("%s\\%s"), szInstallPath, findData.cFileName );
                bResult = DeleteFile( szTemp );

                if (!bResult)
                    dwError = GetLastError();
            }
        }
        while( FindNextFile( hFind, &findData ) );

        FindClose( hFind );
    }

    // If there was a quarantine folder that means we are done
    if (bQFolderFound)
        return;

    //
    // Try wiping the remaining directories if possible.
    //
    do
    {
        bResult = RemoveDirectory( szInstallPath );

        if (!bResult)
        {
            dwError = GetLastError();
            break;
        }

        if ( !(lpPath = _tcsrchr( szInstallPath, '\\' )) )
            break;

        *lpPath = 0;
    } while (TRUE);

    return;
}

#ifndef VER_SUITE_TERMINAL
#define VER_SUITE_TERMINAL                  0x00000010
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsNT4TerminalServer
//
// Description  :
//
// Return type  : LONG
//
// Argument     : 
//
///////////////////////////////////////////////////////////////////////////////
// 5/25/02 - ASiddiqui: Function created
///////////////////////////////////////////////////////////////////////////////

extern "C" BOOL __declspec( dllexport ) IsNT4TerminalServer()
{
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;

    // Assume not terminal server install
    LONG lRetVal = 0;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure,
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    // Get the version information about the system
    bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);
    if( !bOsVersionInfoEx )
    {
        // Failed to get the version info. Exit the function
        return lRetVal;
    }

    // Test to see if this is Terminal Server edition
//    if( (osvi.wReserved[0] & VER_SUITE_TERMINAL) )
	if ((osvi.wSuiteMask & VER_SUITE_TERMINAL ))
    {
        // Yes it is terminal server
        lRetVal = 1;
    }

    return lRetVal;
}

