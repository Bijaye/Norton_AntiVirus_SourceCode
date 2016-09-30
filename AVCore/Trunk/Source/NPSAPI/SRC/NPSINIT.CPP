/////////////////////////////////////////////////////////////////////
// NPSInit.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NPSAPI/VCS/npsinit.cpv   1.5   01 Jul 1998 12:08:08   mdunn  $
//
// Description:
//   This file will contain all code for checking if the service or
//   the executable watching the data file is loaded.  It will also
//   contain a function to start them for you if they are not
//   already.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NPSAPI/VCS/npsinit.cpv  $
// 
//    Rev 1.5   01 Jul 1998 12:08:08   mdunn
// Changed KEY_ALL_ACCESS to KEY_QUERY_VALUE so things will work for non-admin
// users.
// 
//    Rev 1.4   04 May 1998 19:18:42   mdunn
// Added code to detect & start the Win 98 scheduler.
// 
//    Rev 1.3   17 Apr 1998 11:59:12   mdunn
// #undef'ed TRACE since it now collides with an MFC function.
// 
//    Rev 1.2   10 Sep 1997 15:48:22   KSACKIN
// Ported from QAKG.
//
//    Rev 1.1.1.1   13 Aug 1997 18:28:46   ksackin
// Added SERVICE_INTERACTIVE_PROCESS to all of my calls to create the Scheduler service, or to change the service config.
//
//    Rev 1.1.1.0   12 Aug 1997 22:47:48   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.1   06 Aug 1997 08:58:38   JBRENNA
// Change StartSchedulerW so that it can create a Shortcut in the StartUp
// Start Menu for this user on Win95 only. WinNT logic remains the same.
//
//    Rev 1.0   05 Jun 1997 18:07:48   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"
#include <malloc.h>
#include "stdhelp.h"
#include "sys\timeb.h"
#include "time.h"
#include "undoc.h"                      // for extra typedefs
#include "msgdisp.h"                    // error message stuff
#include "xapi.h"
#include "file.h"
#include "ngui.h"
#include "symintl.h"
#include "npsres.h"
#include "npsstb.h"
#include "npserr.h"

#define INITGUID
  #include "ole2.h"
  #include "ShlGuid.h"
  #include "ShlObj.h"
#undef  INITGUID

#include "nps.h"                        // Scheduler includes

#undef TRACE
#define TRACE   ERR_TRACE_ID (_NPSINIT_CPP)


/************************************************************************/
/* Function Prototypes                                                  */
/************************************************************************/

BOOL StartWin98TaskScheduler();

                                        // Function to query service status.
BOOL QueryMyServiceStatus(
    LPTSTR szServiceName,
    LPSERVICE_STATUS lpServiceStatus
    );

/************************************************************************/
/* Static Function Implementation.                                      */
/************************************************************************/

/////////////////////////////////////////////////////////////////////
// HRESULT GetStartMenuStartUpFolder (LPTSTR szDir, DWORD dwNumDirBytes)
//
// Purpose:
//   To determine the StartMenu - Program - StartUp folder for this
//   machine. Remember that on DBCS machines, the "StartUp" folder will
//   not be named "StartUp". We read the folder name from the registry.
//
// Parameters:
//   szDir         [out] A buffer that receives the folder name.
//   dwNumDirBytes [in]  The number of bytes in the szDir buffer.
//
// Returns:
//   TRUE  - success.
//   FALSE - failure.
//
/////////////////////////////////////////////////////////////////////
// 08/05/97 JBRENNA Function created.
/////////////////////////////////////////////////////////////////////
static HRESULT GetStartMenuStartUpFolder (LPTSTR szDir, DWORD dwNumDirBytes)
{
    auto  HRESULT    hResult     = E_FAIL;
    auto  HKEY       hBaseKey;
    auto  DWORD      dwKeyType;

    SYM_ASSERT (szDir);
    SYM_ASSERT (dwNumDirBytes > 0);

    //  Get the value from the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_CURRENT_USER,
                                       _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
                                       0, KEY_QUERY_VALUE, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, _T("StartUp"),
                                              0, &dwKeyType, (LPBYTE)szDir,
                                              &dwNumDirBytes))
        {
            if (dwKeyType == REG_SZ && dwNumDirBytes)
                hResult = S_OK;
        }

        RegCloseKey(hBaseKey);
    }

    // If this function failed, zero out the output parameter.
    if (FAILED (hResult))
        *szDir = '\0';

    return hResult;
}

/////////////////////////////////////////////////////////////////////
// BOOL StartSchedulerW95 (LPCWSTR sz95ShortcutName)
//
// Purpose:
//   This function will start the scheduler for Win95. This includes
//   creating a Shortcut in the User's StartUp Start Menu.
//
// Parameters:
//   sz95ShortcutName [in] - Specifies the name of the Start Menu - Program -
//                           StartUp folder item. Generally this is
//                           "Norton Program Scheduler". The string should be
//                           loaded from a resource file. This allows Dublin
//                           to translate the string.
//
/////////////////////////////////////////////////////////////////////
// 08/06/97 JBRENNA Create Function.
/////////////////////////////////////////////////////////////////////
static BOOL StartSchedulerW95 (LPCWSTR sz95ShortcutName)
{
    auto    IShellLink   *pShellLink;
    auto    IPersistFile *pPersistFile;
    auto    HRESULT      hResult;
    auto    TCHAR        szSchedulerBinary[SYM_MAX_PATH];
    auto    TCHAR        szDir[SYM_MAX_PATH];
    auto    WCHAR        szOleDir[SYM_MAX_PATH];
    extern  TCHAR        *szNPS95Binary;
    OSVERSIONINFO rOSInfo = { sizeof(OSVERSIONINFO) };

    // Ensure the caller specified the shortcut name when we are
    // running on 95.
    SYM_ASSERT (sz95ShortcutName);
    SYM_ASSERT (*sz95ShortcutName);

                                    // Get the path to our binary.
    NpsGetMyPath (szDir);
    STRCPY (szSchedulerBinary, szDir);
    STRCAT (szSchedulerBinary, "\\");

                                    // Append our binary file name.
    STRCAT (szSchedulerBinary, szNPS95Binary);

                                    // If we're running on Win 98 and the
                                    // NPS EXE isn't installed, then start
                                    // up the 98 scheduler.
    if ( 0xFFFFFFFF == GetFileAttributes ( szSchedulerBinary ) &&
         GetVersionEx ( &rOSInfo )    &&
         rOSInfo.dwMajorVersion == 4  &&
         rOSInfo.dwMinorVersion >= 10   )
        {
        return StartWin98TaskScheduler();
        }

    // Init the COM interfaces.
    CoInitialize (NULL);

    // attempt to create ShellLink object
    hResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                               IID_IShellLink, (LPVOID *)&pShellLink);

    if (SUCCEEDED (hResult))
    {
        // Set the information for the Shell Link.

        hResult = pShellLink->SetPath (szSchedulerBinary);

        if (SUCCEEDED (hResult))
            hResult = pShellLink->SetWorkingDirectory (szDir);

        if (SUCCEEDED (hResult))
            hResult = pShellLink->SetShowCmd (SW_SHOWMINNOACTIVE);

        // If no errors occured, write the ShellLink out to a file.
        if (SUCCEEDED (hResult))
        {
            hResult = pShellLink->QueryInterface (IID_IPersistFile,
                                                  (LPVOID*)&pPersistFile);

            if (SUCCEEDED (hResult))
            {
                if (SUCCEEDED (GetStartMenuStartUpFolder (szDir, sizeof(szDir))))
                {
#ifndef UNICODE
                    MultiByteToWideChar (CP_ACP, 0, szDir, -1,
                                         szOleDir, MAX_PATH);
#else
    			    wcscpy (szOleDir, szDir);
#endif

                    wcscat (szOleDir, OLESTR("\\"));
                    wcscat (szOleDir, sz95ShortcutName);
                    wcscat (szOleDir, OLESTR(".lnk"));

                    hResult = pPersistFile->Save (szOleDir, TRUE);
                }
                pPersistFile->Release();
            }
        }

        // release ShellLink object
        pShellLink->Release();
    }

    CoUninitialize ();

                                    // Start the Scheduler executable.
                                    // (32 for WinExec)...
    if (32 >  WinExec (szSchedulerBinary, SW_SHOWMINNOACTIVE))
        return FALSE;
    else
        return TRUE;             // Success.  Return TRUE.
}


// These #define's are in the MSVC 5 docs, so it should be safe to have
// the strings here.

#define SCHED_CLASS             TEXT("SAGEWINDOWCLASS")
#define SCHED_TITLE             TEXT("SYSTEM AGENT COM WINDOW")
#define SCHED_SERVICE_APP_NAME  TEXT("mstask.exe")
#define SCHED_SERVICE_NAME      TEXT("Schedule")

//////////////////////////////////////////////////////////////////////////
//
// Function:    StartWin98TaskScheduler
//
// Description:
//  Starts up Win 98's native scheduler.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the scheduler was started (or was already running), FALSE
//  if it couldn't be started.
//
//////////////////////////////////////////////////////////////////////////
// 5/1/98  Mike Dunn -- function created (taken from MSVC 5 docs)
//////////////////////////////////////////////////////////////////////////

BOOL StartWin98TaskScheduler()
{
HWND                hwnd = FindWindow(SCHED_CLASS, SCHED_TITLE);
STARTUPINFO         sui;
PROCESS_INFORMATION pi;
TCHAR               szApp[MAX_PATH];
LPTSTR              pszPath;
DWORD               dwRet;
BOOL                bLaunched;

    if (hwnd != NULL)
        {                               // It is already running.
        return TRUE;
        }

                                        // Execute the task scheduler process.
    ZeroMemory(&sui, sizeof(sui));
    sui.cb = sizeof (STARTUPINFO);

    dwRet = SearchPath ( NULL, SCHED_SERVICE_APP_NAME, NULL, MAX_PATH,
                         szApp, &pszPath);
                         
    if (dwRet == 0)
        {
        return FALSE;
        }

    bLaunched = CreateProcess ( szApp, NULL, NULL, NULL, FALSE,
                                CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
                                NULL, NULL, &sui, &pi );

    if (bLaunched == 0)
        {
        return FALSE;
        }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return TRUE;
}

/************************************************************************/
/* Function Implementation.                                             */
/************************************************************************/


/////////////////////////////////////////////////////////////////////
// BOOL bIsSchedulerRunning( void )
//
// Purpose:
//   To determine if the scheduler is running, and if not, start the
//   scheduler.
//
// Parameters:
//
// Returns:
//   TRUE  - Scheduler is Ready.
//   FALSE - Scheduler is not currently running
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-03-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL IsSchedulerRunning( void )
{
SERVICE_STATUS  rServiceStatus;
extern char     *szNPSUIWndClassName;   // NPSUI main windoes class name.
extern char     *szNPSServiceName;      // NPSSVC service name.
BOOL            bRetVal = TRUE;

                                        // If we are operating under Windows
                                        // 95...
    if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_95 )
        {
        OSVERSIONINFO rOSInfo = { sizeof(OSVERSIONINFO) };
        TCHAR         szNPSPath [ SYM_MAX_PATH ];
        extern TCHAR* szNPS95Binary;

        NpsGetMyPath ( szNPSPath );
        NameAppendFile ( szNPSPath, szNPS95Binary );

        // I'm checking for existence of the NPS EXE because NPS can still
        // be installed on 98.  If it exists, then we search for its
        // window.  Else, search for the 98 scheduler.

        if ( 0xFFFFFFFF == GetFileAttributes ( szNPSPath ) &&
             GetVersionEx ( &rOSInfo )    &&
             rOSInfo.dwMajorVersion == 4  &&
             rOSInfo.dwMinorVersion >= 10   )
            {
            if ( NULL == FindWindow ( SCHED_CLASS, SCHED_TITLE ) )
                {
                bRetVal = FALSE;
                }
            }
        else
            {
                                        // See if the executable is running.
            if ( FALSE == FindWindow (szNPSUIWndClassName, NULL) )
                {
                bRetVal = FALSE;
                }
            }
        }
    else
        {
        if ( QueryMyServiceStatus( szNPSServiceName, &rServiceStatus ) )
            {
                                        // If we are operating under Windows
                                        // NT, Query Service.
            switch( rServiceStatus.dwCurrentState )
                {
                case SERVICE_RUNNING:
                                        // Service is running, return TRUE.
                    bRetVal = TRUE;
                    break;

                default:
                                        // Service is stopped or not
                                        // functioning properly.
                    bRetVal = FALSE;
                    break;
                }
            }
        else
            {
                                        // Could not query the state of the
                                        // service.  The service is either not
                                        // responding, or not installed.
            bRetVal = FALSE;
            }
        }

                                        // Return the status of the scheduler.
    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// BOOL QueryMyServiceStatus( LPTSTR szServiceName, LPSERVICE_STATUS lpServiceStatus )
//
// Purpose:
//  This function will open a handle to the service control manager,
//  and query the state of the specified service.
//
// Parameters:
//
//
// Returns:
//   TRUE  - Query successfull.
//   FALSE -
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 06-04-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL QueryMyServiceStatus(
    LPTSTR szServiceName,
    LPSERVICE_STATUS lpServiceStatus
    )
{
SC_HANDLE hSCManager=NULL;
SC_HANDLE hRequestedService=NULL;
BOOL bRetVal = TRUE;

                                        // Open the Service Control Manager...
    if ( hSCManager = OpenSCManager( NULL, NULL, GENERIC_READ ) )
        {
                                        // Open the requested service.
        hRequestedService = OpenService(
            hSCManager,
            szServiceName,
            SERVICE_QUERY_STATUS
            );

                                        // Verify valid handle...
        if ( NULL != hRequestedService )
            {
                                        // Query the status.
            if ( QueryServiceStatus( hRequestedService, lpServiceStatus ) )
                {
                                        // Query successful.  Return
                                        // lpServiceStatus struct, and TRUE for
                                        // query result.
                bRetVal = TRUE;
                }
            else
                {
                                        // Query failed.  Return FALSE.
                bRetVal = FALSE;
                }
            }
        else
            {
                                        // Could not open the selected
                                        // service...
            bRetVal = FALSE;
            }
        }
    else
        {
                                        // Could not open the Service Control
                                        // Manager.  Return Fail.
        bRetVal = FALSE;
        }

                                        // If we have an open handle to the
                                        // Requested Service...
    if ( NULL != hRequestedService )
        {
                                        // Close it.
        CloseServiceHandle( hRequestedService );
        }
                                        // If we have an open handle to the
                                        // Service Control Manager...
    if ( NULL != hSCManager )
        {
                                        // Close it.
        CloseServiceHandle( hSCManager );
        }



                                        // Return Result.
    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// BOOL StartSchedulerA (LPCSTR sz95ShortcutName)
//
// Purpose:
//   See StartSchedulerW. This function merely translates
//   the LPSTR to a LPWSTR.
/////////////////////////////////////////////////////////////////////
// 08/05/97 JBRENNA Function Created.
/////////////////////////////////////////////////////////////////////
BOOL StartSchedulerA (LPCSTR sz95ShortcutName)
{
    if (!sz95ShortcutName)
        return StartSchedulerW (NULL);

    const int    nMaxBytes  = sizeof (WCHAR)*(STRLEN (sz95ShortcutName) +1);
    auto  LPWSTR szWideName = (LPWSTR)_alloca (nMaxBytes);
    auto  BOOL   bResult;

    MultiByteToWideChar (CP_ACP, 0, sz95ShortcutName, -1, szWideName, MAX_PATH);

    bResult = StartSchedulerW (szWideName);

    return bResult;
}

/////////////////////////////////////////////////////////////////////
// BOOL StartSchedulerW (LPCWSTR sz95ShortcutName)
//
// Purpose:
//   This function will start the scheduler for the respective
//   platform that we are running on.  Within NT, it will attempt
//   to create the service, if not created, and then
//   service
//
// Parameters:
//   sz95ShortcutName [in] - Only used for Win95. Can be NULL for WinNT.
//                           Specifies the name of the Start Menu - Program -
//                           StartUp folder item. Generally this is
//                           "Norton Program Scheduler". The string should be
//                           loaded from a resource file. This allows Dublin
//                           to translate the string.
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 06-04-97 - KSACKIN - Function Created.
// 08-05-97 - JBRENNA - Added Win95 IShellLink support for creating
//                      Startup folder shortcut.
/////////////////////////////////////////////////////////////////////
BOOL StartSchedulerW (LPCWSTR sz95ShortcutName)
{
    auto    BOOL      bRetVal = TRUE;

                                        // If we are operating under Windows
                                        // 95...
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
    {
        bRetVal = StartSchedulerW95 (sz95ShortcutName);
    }
    else
    {
                                        // Windows NT...

                                        // Create the scheduler.  If this
                                        // fails, no big deal.  It is probably
                                        // already there.
        CreateSchedulerService();

        if ( FALSE == StartSchedulerService() )
        {
            bRetVal = FALSE;
        }
    }

                                        // Return our result.
    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// BOOL CreateSchedulerService( void )
//
// Purpose:
//   This function will ensure that the Norton Program Scheduler
//   service has been created.  If it has not been created, it will
//   create the service.
//
// Parameters:
//   None.
//
// Returns:
//   TRUE  - Serivce exists.  Either exists, or has been created.
//   FALSE - Service could not be created.
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-05-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CreateSchedulerService( void )
{
    auto    SC_HANDLE   hSCManager=NULL;
    auto    SC_HANDLE   hSchedulerService=NULL;
    extern  TCHAR *     szNPSServiceName;
    extern  TCHAR *     szNPSNTBinary;
    auto    TCHAR       szNPSServiceBinPath[SYM_MAX_PATH];
    auto    BOOL        bRetVal=TRUE;

                                        // Get the path to the service.
    NpsGetMyPath( szNPSServiceBinPath );
    STRCAT( szNPSServiceBinPath, "\\" );
    STRCAT( szNPSServiceBinPath, szNPSNTBinary );


                                        // Open the Service Control Manager...
    if ( hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE ) )
        {
                                        // Attempt to open the service.
        if ( hSchedulerService = OpenService(
            hSCManager,
            szNPSServiceName,
            SERVICE_CHANGE_CONFIG
            ) )
            {
                                        // Make sure that the service has been
                                        // created correctly.
            if ( ChangeServiceConfig(
                hSchedulerService,
                SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                SERVICE_AUTO_START,
                SERVICE_NO_CHANGE,
                szNPSServiceBinPath,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                szNPSServiceName
                ) )
                {
                                        // Positive return value means success.
                bRetVal = TRUE;
                }
            else
                {
                                        // Error occurred in changing the
                                        // service configuration.
                bRetVal = FALSE;
                }
            }
        else
            {
                                        // Create the Scheduler service.
            if ( hSchedulerService = CreateService(
                hSCManager,
                szNPSServiceName,
                szNPSServiceName,
                SERVICE_ALL_ACCESS,
                SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                SERVICE_AUTO_START,
                SERVICE_ERROR_NORMAL,
                szNPSServiceBinPath,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL
                ) )
                {
                                        // Service created successfully.
                bRetVal = TRUE;
                }
            else
                {
                                        // Error creating service.
                bRetVal = FALSE;
                }
            }
        }
    else
        {
                                        // Service is already there.  Return
                                        // TRUE.
        bRetVal = FALSE;
        }

                                        // If we have an open handle to the
                                        // Requested Service...
    if ( NULL != hSchedulerService )
        {
                                        // Close it.
        CloseServiceHandle( hSchedulerService );
        }
                                        // If we have an open handle to the
                                        // Service Control Manager...
    if ( NULL != hSCManager )
        {
                                        // Close it.
        CloseServiceHandle( hSCManager );
        }

                                        // Return Result.
    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// BOOL StartSchedulerService( void )
//
// Purpose:
//    This function will start the scheduler service.  This should
//    be called through StartScheduler().  The StartScheduler()
//    function will determine the OS, and do the appropriate action.
//
// Parameters:
//    None.
//
// Returns:
//    TRUE  - Scheduler started successfully.
//    FALSE - Scheduler could not be started.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-05-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL StartSchedulerService( void )
{
    auto    SC_HANDLE   hSCManager=NULL;
    auto    SC_HANDLE   hSchedulerService=NULL;
    extern  TCHAR *     szNPSServiceName;
    auto    BOOL        bRetVal=TRUE;

                                        // Open the Service Control Manager...
    if ( hSCManager = OpenSCManager( NULL, NULL, GENERIC_READ ) )
        {
                                        // Open the requested service.
        hSchedulerService = OpenService(
            hSCManager,
            szNPSServiceName,
            SERVICE_START
            );

                                        // If we got an open handle.
        if ( NULL != hSchedulerService )
            {
                                        // Start the service.
            if ( StartService( hSchedulerService, NULL, NULL ) )
                {
                                        // Service started successfully.
                bRetVal = TRUE;
                }
            else
                {
                                        // Could not start the service.
                bRetVal = FALSE;
                }
            }
        else
            {
                                        // Failed to open a handle to the
                                        // service.
            bRetVal = FALSE;
            }
        }
    else
        {
                                        // Failed to open a handle to the
                                        // service control manager.
        bRetVal = FALSE;
        }

                                        // If we have an open handle to the
                                        // Requested Service...
    if ( NULL != hSchedulerService )
        {
                                        // Close it.
        CloseServiceHandle( hSchedulerService );
        }
                                        // If we have an open handle to the
                                        // Service Control Manager...
    if ( NULL != hSCManager )
        {
                                        // Close it.
        CloseServiceHandle( hSCManager );
        }

                                        // Return success of service start.
    return bRetVal;
}

