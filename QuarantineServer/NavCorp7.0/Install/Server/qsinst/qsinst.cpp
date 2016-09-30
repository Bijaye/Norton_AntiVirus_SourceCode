// qsinst.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "tchar.h"
#include "resource.h"
#include "winsvc.h"
#include "qsregval.h"


///////////////////////////////////////////////////////////////////////////////
// 
// Locals
// 
///////////////////////////////////////////////////////////////////////////////

HINSTANCE g_hInstance = NULL;
HMODULE g_hQsPakModule = NULL;

typedef ULONG (*PFNISQSERVERFILE)( const char * pszFileName );
PFNISQSERVERFILE g_pfnIsQserverFile;


static int FilesInQuarantine( HWND hWnd, LPSTR szFolder );
static BOOL IsQserverFile( const char * pszFileName );
void CleanupRegistry(void);


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
extern "C" LONG __declspec( dllexport ) UninstInitialize( HWND hWnd, HANDLE hInstance, LONG lReserved )
{

    char szUninstall[ MAX_PATH ];
    char szError[ MAX_PATH * 2 ];
    LoadString( g_hInstance, IDS_UNINSTALL, szUninstall, MAX_PATH );

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
    // Check to see if there are any files in the quarantine folder.
    // 
    HKEY hKey;
    int iRet = 0;
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, 0, KEY_READ, &hKey ) )
        {
        // 
        // Fetch the quarantine folder from the registry.
        // 
        char szQFolder[ MAX_PATH + 1 ];
        ZeroMemory( szQFolder, MAX_PATH +1 );
        ULONG lLength = MAX_PATH;
        if( ERROR_SUCCESS == RegQueryValueEx( hKey, 
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
extern "C" LONG __declspec( dllexport ) UninstUnInitialize( HWND hWnd, HANDLE hInstance, LONG lReserved )
{
    CleanupRegistry();

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
        // Remove directory if we can.
        // 
        if( iRet == IDYES )
            {
            RemoveDirectory( szFolder );
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
        if( ERROR_SUCCESS == RegQueryValueEx( hKey, 
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
		
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Quarantine"), 
									 0L, KEY_ALL_ACCESS, &hRegKey))
	{
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
