
#include <tchar.h>
#include "resource.h"
#include "NuInst.h"
/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  Local Macros/Inlines
/////////////////////////////////////////////////////////////////////////////
#define ADVUNINST _T("UnInstAdvTools")
#define ADVINST   _T("AdvTools")
#define INSTALLEDAPPS_KEY _T("Software\\Symantec\\InstalledApps")
// ==== DllMain ===========================================================
//
//
// ========================================================================
// ========================================================================


BOOL WINAPI DllMain(HANDLE hInstDLL, ULONG fdwReason, LPVOID lpReserved)
{
    if(DLL_PROCESS_ATTACH == fdwReason)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hInstDLL);

        g_hInst = (HINSTANCE) hInstDLL;
	}
	
    return  (TRUE);
	
}  // end of "DllMain"


BOOL IsOSNT()
{
	OSVERSIONINFO OsVersion;
	ZeroMemory(&OsVersion, sizeof(OSVERSIONINFO));
	OsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if(FALSE == GetVersionEx(&OsVersion))
	{
		OsVersion.dwPlatformId = VER_PLATFORM_WIN32_NT;
	}

	return((OsVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) ? TRUE : FALSE);
}


/////////////////////////////////////////////////////////////////////////////
//
//  FileInUseHandler() - Function to handle a file-in-use properly under the
//                       appropriate OS for file/directory deletions
//
//  Params: LPSTR - File or directory to delete
//
//  Return value:   TRUE if successfully scheduled for deletion
//
/////////////////////////////////////////////////////////////////////////////
//  05/19/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////
//NOTE:  this function was copied directly from the working NU install
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI FileInUseHandler( LPTSTR lpName )
{
    BOOL                    bRet = FALSE;
    FILE                    *fpFile;
    LPTSTR                  lpValue = "NSI";
    LPTSTR                  lpSection = "Rename";
    TCHAR					szFile[MAX_PATH ] = {0};
    TCHAR					szLine[MAX_PATH * 2 ] = {0};
    TCHAR					szShort[MAX_PATH ] = {0};

    
	if( NULL == GetShortPathName( lpName, szShort, SI_MAX_PATH - 1 ) )
	{
		return FALSE;
	}

    if (IsOSNT())
	{	
		if(0 != MoveFileEx( szShort, NULL, MOVEFILE_DELAY_UNTIL_REBOOT ))
			bRet = TRUE;
	}
	else
    {
        GetWindowsDirectory( szFile, SI_MAX_PATH - 1 );
        _tcscat(szFile, _T("\\WININIT.INI"));

        WritePrivateProfileString( lpSection, lpValue, szShort, szFile );
        WritePrivateProfileString( NULL, NULL, NULL, szFile );


        fpFile = fopen( szFile, "rt" );

        if ( fpFile != NULL )
        {
            fpos_t          fpos;

            do
            {
                fgetpos( fpFile, &fpos );
                fgets( szLine, SI_MAX_PATH * 2, fpFile );

                if ( !_strnicmp( szLine, lpValue, 3 ) )
                {
                    fflush( fpFile );
                    fclose( fpFile );

                    fpFile = fopen( szFile, "r+t" );

                    if ( fpFile != NULL )
                    {
                        fsetpos( fpFile, &fpos );
                        fprintf( fpFile, "NUL=%s\n", (LPTSTR)szShort );
                        fflush( fpFile );
                        fclose( fpFile );
                        fpFile = NULL;
                    }
                    break;
                }
            }
            while( !feof( fpFile ) );

        if ( fpFile != NULL )
            fclose( fpFile );

        bRet = TRUE;
        }
    }

    return( bRet );
}



/////////////////////////////////////////////////////////////////////////////
//  UninstInitialize - Function that gets called by UNINST.EXE at the
//                     beginning of uninstall.
//
// Return values:
//  0 - Continue Uninstall
//  <0 - Abort Uninstall
//
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

extern "C" DllExport LONG UninstInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes)
{

	BOOL bWait = TRUE;

	TCHAR szFullCmdLine[MAX_PATH] = {0};
	HKEY tmpKey = NULL;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, INSTALLEDAPPS_KEY, 0, KEY_ALL_ACCESS, &tmpKey))
	{
		DWORD dwSize = MAX_PATH;
		DWORD dwType = REG_SZ;
		if( ERROR_SUCCESS == RegQueryValueEx(tmpKey, ADVINST, 0, &dwType, (BYTE*) szFullCmdLine, &dwSize) )
		{
			//Add this value so we can remove our directory at the very end of install.
			RegSetValueEx(tmpKey, ADVUNINST, 0, dwType, (BYTE*) szFullCmdLine, dwSize);
		}
		RegCloseKey(tmpKey);

	}
	
	

	_tcscat(szFullCmdLine, _T("\\ADVCHK.EXE /u"));
		
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    
    si.cb = sizeof(si);
    if(CreateProcess(NULL, szFullCmdLine,
        NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
        NULL,   //Starting dir
        &si, &pi))
    {
        
        //now start up the process
        ResumeThread(pi.hThread);
        
        if(bWait)
		{
			while (WAIT_TIMEOUT ==	WaitForSingleObject(pi.hProcess, 100))
			{
				MSG   msg;
            
				while (PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
				{
					DispatchMessage( &msg );
				}
			}
		}
        
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
	
	}



    return(0);
}

/////////////////////////////////////////////////////////////////////////////
//  UninstUninitialize - Function that gets called by UNINST.EXE at the
//                       end of uninstall.
//
// Return values:
//  NOT IMPORTANT
//
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

extern "C" DllExport LONG UninstUnInitialize(HWND hwndDlg, HANDLE hInstance, LONG lRes)
{

	//Get our saved directory and remove it
	TCHAR szFullCmdLine[MAX_PATH] = {0};
	HKEY tmpKey = NULL;

	//Get the directory from the backup version we put in the installedapps key
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, INSTALLEDAPPS_KEY , 0, KEY_ALL_ACCESS, &tmpKey))
	{
		DWORD dwSize = MAX_PATH;
		DWORD dwType = REG_SZ;
		if( ERROR_SUCCESS == RegQueryValueEx(tmpKey, ADVUNINST, 0, &dwType, (BYTE*) szFullCmdLine, &dwSize) )
		{
			//Now take that value and give it off to our file/directory in use removal function.
			FileInUseHandler(szFullCmdLine);
			RegDeleteValue(tmpKey, ADVUNINST);
		};

		RegCloseKey(tmpKey);
	}	

	// Remove all the left over registry keys
	// I'm not going to enumerate the directories (hard coding instead)
	RegDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec\\Norton Utilities\\WipeinfoSE\\Wipe info\\Settings"));
	RegDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec\\Norton Utilities\\WipeinfoSE\\Wipe info"));
	RegDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec\\Norton Utilities\\WipeinfoSE\\options\\SymantecExclusionApps"));
	RegDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec\\Norton Utilities\\WipeinfoSE\\options"));
	RegDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec\\Norton Utilities\\WipeinfoSE"));
	
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec\\Norton Utilities"), 0, KEY_ALL_ACCESS, &tmpKey))
		{
		DWORD dwKeys, dwValues;
		if (GetRegistryKeyInfo(&tmpKey, &dwKeys, &dwValues) == ERROR_SUCCESS
			&& (!dwKeys && !dwValues))
			{
			RegDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec\\Norton Utilities"));
			}
		RegCloseKey(tmpKey);
		}

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec"), 0, KEY_ALL_ACCESS, &tmpKey))
		{
		DWORD dwKeys, dwValues;
		if (GetRegistryKeyInfo(&tmpKey, &dwKeys, &dwValues) == ERROR_SUCCESS
			&& (!dwKeys && !dwValues))
			{
			RegDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Symantec"));	
			}
		RegCloseKey(tmpKey);
		}	
	
    return(0);
}


/////////////////////////////////////////////////////////////////////////////
//
//  LTrace() - Function to trace output to a log file put in the windows dir
//
//  Params: LPCSTR - Format string
//          Variable Args - To be substituted into format string
//
//  Return value:   VOID
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

VOID CDECL LTrace( LPCSTR lpszFmt, ... )
{
    FILE *fp = NULL;
    DWORD dwNumBytes = 0L;
    TCHAR szBuffer[SI_MAXSTR] = {0};
    TCHAR szLogFile[SI_MAX_PATH] = {0};

    LPSTR lpArgs = (LPSTR)&lpszFmt + sizeof( lpszFmt );
    wvsprintf( (LPSTR)szBuffer, lpszFmt, lpArgs );
    lstrcat( szBuffer, "\n" );

    if ( g_bLogging )
    {
        GetWindowsDirectory( szLogFile, SI_MAXSTR - 1 );
		_tcscat(szLogFile, _T("SYMINST.LOG"));

        fp = fopen( szLogFile, "at" );

        if ( fp != NULL )
        {
            fputs( szBuffer, fp );
            fflush( fp );
            fclose( fp );
        }
    }
    else
    {
#ifdef _DEBUG    
        OutputDebugString( szBuffer );
#endif
    }

    return;
}




LONG GetRegistryKeyInfo( PHKEY phKey, LPDWORD lpdwKeys, LPDWORD lpdwValues )
{
    BOOL        bClose = FALSE;
    LONG        lReturn = ERROR_INVALID_DATA;
    TCHAR       szClass[ MAX_PATH ];
    DWORD       dwClass = MAX_PATH;
    DWORD       dwMaxSubkey = 0;
    DWORD       dwMaxClass = 0;
    DWORD       dwMaxValueName = 0;
    DWORD       dwMaxValueData = 0;
    DWORD       dwSecurityDescriptor = 0;
    FILETIME    ftWriteTime;

    lReturn = RegQueryInfoKey( *phKey, szClass, &dwClass,
                               NULL, lpdwKeys, &dwMaxSubkey,
                               &dwMaxClass,
                               lpdwValues, &dwMaxValueName, &dwMaxValueData,
                               &dwSecurityDescriptor, &ftWriteTime );

    return( lReturn );
}