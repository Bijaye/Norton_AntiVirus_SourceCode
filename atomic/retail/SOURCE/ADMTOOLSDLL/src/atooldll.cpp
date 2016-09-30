/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1998 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/ADMTOOLSDLL/VCS/atooldll.cpv   1.2   19 Jun 1998 17:28:22   mdunn  $
//
//  AToolDLL.CPP -  Module that holds callbacks that InstallShield calls
//                  during the uninstall process.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/ADMTOOLSDLL/VCS/atooldll.cpv  $
// 
//    Rev 1.2   19 Jun 1998 17:28:22   mdunn
// Added StringContainsDBCSChars()
// 
//    Rev 1.1   28 May 1998 12:23:50   mdunn
// Added prototypes and extern "C"'s and stuff like that.
// 
//    Rev 1.0   23 May 1998 17:20:56   mdunn
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#include    "platform.h"
#include    "svcctrl.h"


/////////////////////////////////////////////////////////////////////////////
//  Global Variables
/////////////////////////////////////////////////////////////////////////////

HANDLE  g_hMutex;


#define EXTERN_C                extern "C"
#define DllExport               __declspec(dllexport)
#define QUAR_SVC_INTERNAL_NAME  "QUARSVC"


//////////////////////////////////////////////////////////////////////////
// Exported function prototypes

EXTERN_C
{
DllExport LONG UninstInitialize ( HWND, HANDLE, LONG );
DllExport LONG UninstUnInitialize ( HWND, HANDLE, LONG );
DllExport BOOL IsServiceMarkedForDeletion(LPTSTR);
DllExport BOOL IsServiceRunning(LPTSTR);
DllExport BOOL StopSingleService ( LPTSTR, BOOL, BOOL );
DllExport BOOL CreateProcessAndWait (LPCSTR, LPINT);
DllExport BOOL CreateNAVMutex ( LPCTSTR szMutexName );
DllExport BOOL ReleaseNAVMutex();
DllExport BOOL StringContainsDBCSChars ( LPTSTR );
}


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// Function:    DllMain
//
// Description:
//  Entry point for the DLL.
//
//////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain ( HANDLE hModule, ULONG ulReason,
                                         LPVOID lpvReserved )
{
    switch ( ulReason )
        {
        case  DLL_PROCESS_ATTACH:
            break;

        case  DLL_THREAD_ATTACH:
            break;

        case  DLL_THREAD_DETACH:
            break;

        case  DLL_PROCESS_DETACH:
            break;

        default:
            break;
        }

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////


EXTERN_C DllExport LONG UninstInitialize (
                              HWND hwndDlg, HANDLE hInstance,
                              LONG lReserved )
{
    StopSingleService ( QUAR_SVC_INTERNAL_NAME, TRUE, TRUE );

    return 0;                           // Proceed with uninstall.
}


EXTERN_C DllExport LONG UninstUnInitialize (
                              HWND hwndDlg, HANDLE hInstance,
                              LONG lReserved )
{
    return 0;                           // Ignored by IS.
}


//////////////////////////////////////////////////////////////////////////


// ==== IsServiceMarkedForDeletion ======================================
//
//  This function tests to see if a target service is marked for deletion.
//
//  Input:
//      pszServiceName  -- the name of the service
//
//  Output: TRUE if the service is marked for deletion, FALSE if it is not
//
// ========================================================================
//  Function created: 06/97, SEDWARD
// ========================================================================

EXTERN_C DllExport    BOOL    IsServiceMarkedForDeletion(LPTSTR  pszServiceName)
{
    auto    BOOL            bRetValue;
    auto    CServiceControl cServiceControl;
    auto    DWORD           dwReturn;

    bRetValue = FALSE;
    cServiceControl.Open(pszServiceName);
    if (ERROR_SUCCESS != cServiceControl.ChangeServiceConfig())
        {
        dwReturn = GetLastError();

        // now test for the deletion flag
        if (ERROR_SERVICE_MARKED_FOR_DELETE == dwReturn)
            {
            bRetValue = TRUE;
            }
        }

    return (bRetValue);

}  // end of "IsServiceMarkedForDeletetion"


// ==== IsServiceRunning ==================================================
//
//  This function tests to see if a target service is currently running.
//
//  Input:
//      pszServiceName  -- the name of the service
//
//  Output: TRUE if the service is running, FALSE if it is not
//
// ========================================================================
//  Function created: 06/97, SEDWARD
// ========================================================================

EXTERN_C DllExport    BOOL    IsServiceRunning(LPTSTR  pszServiceName)
{
    auto    BOOL                bRetValue;
    auto    CServiceControl     cService;
    auto    DWORD               dwStatus;

    bRetValue = FALSE;
    if (ERROR_SUCCESS == cService.Open(pszServiceName))
        {
        if  ((cService.QueryServiceStatus(&dwStatus))
        &&  (SERVICE_RUNNING == dwStatus))
            {
            bRetValue = TRUE;
            }
        }

    return (bRetValue);

}  // end of "IsServiceRunning"


// ==== StopSingleService =================================================
//
//  This exported function stops a single service.  If the "delete" argument
//  is set to TRUE, then the service is actually deleted, otherwise it is
//  simply stopped.
//
//  Input:
//      lpszServiceName -- the display name of the service
//      bStopService    -- TRUE means stop the service, FALSE means not to
//      bDeleteService  -- if TRUE, the service is deleted
//
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 6/97, SEDWARD
// ========================================================================

EXTERN_C DllExport   BOOL    StopSingleService(LPTSTR  lpszServiceName
                                                            , BOOL  bStopService
                                                            , BOOL  bDeleteService)
{
    auto    BOOL                    bReturn = TRUE;
    auto    CServiceControl         cService;
    auto    DWORD                   dwStatus = TRUE;

    if (ERROR_SUCCESS == cService.Open(lpszServiceName))
        {
        if  ((bStopService)
        &&  (cService.QueryServiceStatus(&dwStatus))
        &&  (SERVICE_STOPPED != dwStatus))
            {
            // if it is not stopped, try to stop it
            if (ERROR_SUCCESS != cService.Stop())
                {
                bReturn = FALSE;
                }
            }

        if (bDeleteService)
            {
            // don't do error checking here because there are times when delete will
            // fail, but the services are set up correctly
            cService.Delete();
            }
        }
    else    // failed to open it?
        {
        bReturn = FALSE;
        }

    return  (bReturn);

}  // end of "StopSingleService"


//////////////////////////////////////////////////////////////////////////
//
// Function:    CreateProcessAndWait
//
// Description:
//  Launches an app and waits for it to terminate.  Similar to the
//  InstallShield LaunchAppAndWait(), but this fn reads the app's
//  return value.
//
// Input:
//  szCmd: [in]
//  szCommand: [in] The NAVDX command line to write to the file.
//
// Returns:
//  TRUE if the line was added OK, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 3/2/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

EXTERN_C DllExport BOOL CreateProcessAndWait (
                             LPCTSTR szCmd,
                             LPINT   pnExitCode )
{
PROCESS_INFORMATION	ProcessInfo;
STARTUPINFO			StartUpInfo;

	ZeroMemory(&StartUpInfo,sizeof(STARTUPINFO));
	StartUpInfo.cb = sizeof(STARTUPINFO); 			// size of structure

	BOOL bSuccess = CreateProcess (
                        NULL, (LPSTR)szCmd, NULL, NULL, FALSE,
                        HIGH_PRIORITY_CLASS, NULL, NULL, &StartUpInfo,
                        &ProcessInfo );

	if (bSuccess)
	{
		while (WaitForSingleObject(ProcessInfo.hProcess, 50) == WAIT_TIMEOUT)
		{
			MSG		msg;
		    while (PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
			{
				TranslateMessage(&msg);
			    DispatchMessage(&msg);
			}
		}
		DWORD dwExitCode;
		if (GetExitCodeProcess(ProcessInfo.hProcess, &dwExitCode))
			*pnExitCode = (int)dwExitCode;
		else
			bSuccess = FALSE;
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
	}
	return bSuccess;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CreateNAVMutex
//
// Description:
//  Creates a named mutex, used to keep more than one instance of
//  install running at once.
//
// Input:
//  szMutexName: [in] Name to use for the mutex.
//
// Returns:
//  TRUE if the installer may continue, or FALSE if the mutex couldn't
//  be created or if it already existed, in which case the installer should
//  exit.
//
//////////////////////////////////////////////////////////////////////////
// 3/26/98  Mike Dunn -- Function created
//////////////////////////////////////////////////////////////////////////

EXTERN_C DllExport BOOL CreateNAVMutex ( LPCTSTR szMutexName )
{
HANDLE hMutex;

    hMutex = CreateMutex ( NULL, TRUE, szMutexName );

                                        // Succeed only if CreateMutex()
                                        // returned a valid handle AND the
                                        // mutex didn't already exist.
    if ( NULL != hMutex  &&
         ERROR_ALREADY_EXISTS != GetLastError() )
        {
        g_hMutex = hMutex;
        return TRUE;
        }
    else
        {
        g_hMutex = NULL;
        return FALSE;
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    ReleaseNAVMutex()
//
// Description:
//  Releases a mutex created by CreateNAVMutex().
//
// Input:
//  Nothing.
//
// Returns:
//  Always 1.
//
//////////////////////////////////////////////////////////////////////////
// 3/26/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

EXTERN_C DllExport BOOL ReleaseNAVMutex()
{
    if ( NULL != g_hMutex )
        {
        ReleaseMutex ( g_hMutex );
        g_hMutex = NULL;
        }

    return 1;
}


/////////////////////////////////////////////////////////////////////////
// StringContainsDBCSChars
//
// Description: Check a string to see if has any DBCS characters
//
// Parameters: LPTSTR - The string to check
//
// Return Value: TRUE if any DBCS characters are in the string
//
// Author: GWESTER
// Date:   10/27/96
//
/////////////////////////////////////////////////////////////////////////

EXTERN_C DllExport BOOL StringContainsDBCSChars ( LPTSTR lpszString )
{
    int                 nLen;
    LPTSTR              lpszCurrent;

    nLen = STRLEN ( lpszString );

    if (nLen)
    {
        for (lpszCurrent = lpszString; *lpszCurrent != '\0';
             lpszCurrent = AnsiNext ( lpszCurrent ) )
        {
            if (IsDBCSLeadByte ( *lpszCurrent ) ) return TRUE;
        }
    }

    return FALSE;
}

