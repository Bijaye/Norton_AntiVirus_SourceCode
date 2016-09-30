// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVW/VCS/quarmon.cpv   1.6   15 Sep 1998 18:51:24   CEATON  $
//
// Description:
//      Routines to monitor Quarantine while NAVW is running.
//
// Contains:
//
//*************************************************************************
// $Log:   S:/NAVW/VCS/quarmon.cpv  $
// 
//    Rev 1.6   15 Sep 1998 18:51:24   CEATON
// Modified thread to process a second file change notification for incoming virus
// definitions, added functions to support this file notification.
// 
//    Rev 1.5   26 Jun 1998 09:09:48   tcashin
// Don't call UpdateQuarantineStatus(). Send a message to do it in WM_COMMAND
// processing.
// 
//    Rev 1.4   23 Jun 1998 19:59:44   tcashin
// Changed CreateThread() to beginThread/endThread.
// 
//    Rev 1.3   13 Jun 1998 16:20:22   tcashin
// Still left the hardcoded directory! Fixed a potential tread shutdown problem
// and some general cleanup.
// 
//    Rev 1.2   11 Jun 1998 12:51:54   tcashin
// Got rid of the named event threads. We're only using the dir change thread
// to track changes in quarantine.
// 
//    Rev 1.1   11 Jun 1998 02:08:28   tcashin
// Get the real quarantine directory from the registry and get rid of
// a stray debug string.
// 
//    Rev 1.0   10 Jun 1998 22:36:42   tcashin
// Initial revision.
// 
//*************************************************************************

#include <afxmt.h>
#include "platform.h"
#include "navw.h"
#include "navwstb.h"
#include "navwres.h"
#include "navcmdid.h"
#include "quaradd.h"    // Reg key values for the quarantine directory
#include "process.h"    // beginthread and endthread
#include "n32behav.h"

//*************************************************************************
// External Data
//*************************************************************************
extern  BOOL    g_bQuarMonitorShutdown;
extern  HWND    ghWndMain;

static HANDLE   s_hThreadHandle = NULL;
static HANDLE   s_hThreadSignalEvent = NULL;

const char AVENGEDEFS_VAL[] = "AVENGEDEFS";
const char SYMANTEC_INSTALLED_APPS_KEY[] = "SOFTWARE\\Symantec\\InstalledApps";


//*******************************************************************
//
// Function: char StringGetLastChar(LPCTSTR lpszString)
//
// Description: This function returns the last character in a string.
//              Note that it is a WCHAR.
//
//*******************************************************************

TCHAR LocalStringGetLastChar(LPCTSTR lpszString)
{
    LPCTSTR     lpszLast = lpszString;
    TCHAR       cLast;


    while (*lpszString)
        {
        lpszLast = lpszString;
        lpszString = AnsiNext(lpszString);
        }
        cLast = *lpszLast;
    return(cLast);
}

//*******************************************************************
//
// Function: LocalRemoveLastBackslash()
//
// Description: This function removes any trailing backslash from 
//              a full path.
//
//*******************************************************************

void LocalRemoveLastBackslash(LPTSTR pszDir)
{
    TCHAR   cLast;
    cLast = LocalStringGetLastChar(pszDir);
    if (cLast == '\\')
        *(pszDir + STRLEN(pszDir) - 1) = '\0';
}

//*******************************************************************
//
// Function: LocalDirectoryExists()
//
// Description: This function determines if given directory exists on
//              the hard drive.
//
//*******************************************************************

BOOL LocalDirectoryExists(LPCTSTR pszDir)
{
    TCHAR szDir[_MAX_PATH];
    STRCPY(szDir, pszDir);
    LocalRemoveLastBackslash(szDir);
    DWORD dwAttrib = GetFileAttributes(szDir);
    BOOL bRetVal = FALSE;
    if ((dwAttrib != 0xffffffff) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
        bRetVal = TRUE;

    return (bRetVal);
}



//*******************************************************************
//
// Function: GetVirusDefsDir()
//
// Description: This finds the location of the shared virus 
//              definitions.  It returns TRUE if the directory
//              was found, FALSE otherwise.
//
//*******************************************************************

BOOL GetVirusDefsDir(LPTSTR pszDefsDir, int nMaxCount)
{
    TCHAR szDir[_MAX_PATH];
    DWORD dwType = 0;
    DWORD dwSize = (DWORD) nMaxCount;
    HKEY  hKey = NULL;
    BOOL  bFoundDir = FALSE;
    
    // open "InstalledApps" key
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYMANTEC_INSTALLED_APPS_KEY, 0, KEY_READ, &hKey))
        {
        // check for "AVENGEDEFS" value
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, AVENGEDEFS_VAL, 0, &dwType, (LPBYTE)szDir, &dwSize))
            {
            if ((dwType == REG_SZ) && STRLEN(szDir))
                {
                // convert pathname to short name, if possible
                char szShort[_MAX_PATH];
                if (GetShortPathName(szDir, szShort, sizeof(szShort)))
                    STRCPY(szDir, szShort);
                    
                STRCPY(pszDefsDir, szDir);
                
                if (LocalDirectoryExists(pszDefsDir))
                    bFoundDir = TRUE;
                }
            }
        RegCloseKey(hKey);
        }

    return (bFoundDir);
}


//*************************************************************************
// MyStatusMonitorQuarAndDefDirChange()
//
// DWORD MyStatusMonitorQuarAndDefDirChange(LPVOID lpvParam)
//
// Returns:
//      None
//*************************************************************************
// 06/09/1998 TCashin - functions created
//
//*************************************************************************

void __cdecl MyStatusMonitorQuarAndDefDirChange(LPVOID lpvParam)
{
    auto    LPNAVMAIN   lpNavMain;
    auto    HWND        hNotify;
    auto    HANDLE      aEvents[3];
    auto    DWORD       dwEvent;
    auto    BOOL        bQuarantine = TRUE;

    bQuarantine = NAV_IS_QUARANTINE();

    lpNavMain = (LPNAVMAIN) lpvParam;

    aEvents[0] = s_hThreadSignalEvent;
    aEvents[1] = lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent;
    if ( bQuarantine )
        aEvents[2] = lpNavMain->rQuarantineStatus.hQuarantineDirChangeEvent;
    
    hNotify = lpNavMain->hScanMain;

    // Spin!
    do
    {
        // Wait for the dir change Event
        if ( bQuarantine )
            dwEvent = WaitForMultipleObjects( 3, aEvents, FALSE, INFINITE );
        else
            dwEvent = WaitForMultipleObjects( 2, aEvents, FALSE, INFINITE );
            
       
        if( dwEvent - WAIT_OBJECT_0 == 0 )
        {
            // Bail out of loop.  User wants to shutdown.
            break;
        }
       
        switch( dwEvent - WAIT_OBJECT_0 )
        {
        case 0:
            break;
       
        case 1:
            // Is a scan in progess?
            if (!lpNavMain->bScanInProgress)
            {
                auto	TCHAR		szIncomingDir[ _MAX_PATH ];

                FindCloseChangeNotification( lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent );

                ProcessNewDefinitions ( hNotify );

                if ( GetVirusDefsDir(szIncomingDir, _MAX_PATH) )
                {
                    lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent = 
                       FindFirstChangeNotification( 
                                     szIncomingDir, 
                                     TRUE, 
                                     FILE_NOTIFY_CHANGE_FILE_NAME );

                    aEvents[1] = lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent;
                }
            }

            break;

        case 2:
            // Is a scan in progess?
            if (!lpNavMain->bScanInProgress)
            {
                // If the quarantine status changed, the post a message to display 
                // the current stuff
                PostMessage(hNotify, WM_COMMAND, IDC_STATUS_QUAR_ENUM, 0L);
                FindNextChangeNotification( lpNavMain->rQuarantineStatus.hQuarantineDirChangeEvent );       
            }
            break;
        }
    } while ( TRUE );

    // Cleanup and close the handles
    if( bQuarantine )
        FindCloseChangeNotification( lpNavMain->rQuarantineStatus.hQuarantineDirChangeEvent );

    FindCloseChangeNotification( lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent );
    CloseHandle( s_hThreadSignalEvent );

    _endthread();
    
} // MyStatusMonitorQuarAndDefDirChange


//*************************************************************************
// StartStatusMonitor()
//
// VOID StartStatusMonitor()
//
// Returns:
//      None
//*************************************************************************
// 06/09/1998 TCashin - functions created
//*************************************************************************

VOID StartStatusMonitor(LPNAVMAIN lpNavMain)
{
    auto    TCHAR       szValueBuf[1024];
    auto    DWORD       dwValueBufLen;
    auto    HKEY        hBaseKey;
    auto    DWORD       dwValueType;
    auto    BOOL        bMonitorQuarDirChange = FALSE;
    auto    BOOL        bMonitorDefChange = FALSE;
	auto	TCHAR		szIncomingDir[ _MAX_PATH ];
    auto    BOOL        bQuarantine = TRUE;

    bQuarantine =  NAV_IS_QUARANTINE();

    if( bQuarantine )
        lpNavMain->rQuarantineStatus.pQuarMonCS = new CCriticalSection;
    
    // 
    // Create our thread termination signal event.
    //
    s_hThreadSignalEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    
    
    //
    // Get the Quarantine directory
    //
    if( bQuarantine )
    {
        if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, REGKEY_QUARANTINE_PATH, 0, KEY_READ, &hBaseKey))
        {
            dwValueBufLen = sizeof(szValueBuf);
    
            if (ERROR_SUCCESS == RegQueryValueEx ( hBaseKey, 
                                               REGVAL_QUARANTINE_PATH_STR,
                                               0, 
                                               &dwValueType, 
                                               (LPBYTE)szValueBuf, 
                                               &dwValueBufLen))
            {
                if (dwValueType == REG_SZ && dwValueBufLen)
                {
                    bMonitorQuarDirChange = TRUE;
                }                
            }
        }

        RegCloseKey(hBaseKey);
    }

	// Get the Definitions Directory
	bMonitorDefChange = GetVirusDefsDir(szIncomingDir, _MAX_PATH);


    // 
    // If we got the quarantine directory, then start the quarantine monitoring event
    //
    if ( bMonitorQuarDirChange )
    {
        lpNavMain->rQuarantineStatus.hQuarantineDirChangeEvent = 
                FindFirstChangeNotification( 
                                szValueBuf, 
                                FALSE, 
                                FILE_NOTIFY_CHANGE_FILE_NAME );
    }

    // if we got the Defs Directory, then start the Defs monitoring event
    if ( bMonitorDefChange )
    {
        lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent = 
                FindFirstChangeNotification( 
                                szIncomingDir, 
                                TRUE, 
                                FILE_NOTIFY_CHANGE_FILE_NAME );
    }


    if ( lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent || 
		 lpNavMain->rQuarantineStatus.hQuarantineDirChangeEvent  )
    {
        _beginthread( MyStatusMonitorQuarAndDefDirChange, 0, lpNavMain );
    }
	

}


//*************************************************************************
// StopStatusMonitor()
//
// VOID StopStatusMonitor()
//
// Returns:
//      None
//*************************************************************************
// 06/09/1998 TCashin - functions created
//*************************************************************************

VOID StopStatusMonitor(LPNAVMAIN lpNavMain)
{
    //
    // Tell monitor thread that we want out!
    //
    if ( lpNavMain->rQuarantineStatus.hQuarantineDirChangeEvent ||
         lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent)
    {
        SetEvent( s_hThreadSignalEvent );
    }

}

VOID ContinueDefinitionNotifications( LPNAVMAIN lpNavMain )

{
#ifdef TESTING
	auto	TCHAR		szIncomingDir[ _MAX_PATH ];
    auto    BOOL        bMonitorDefChange = FALSE;

    // This is necessary due to a problem noticed under Windows 98. If ProcessNewDefinitions()
    // is called
    FindCloseChangeNotification( lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent );

                // If the definitions status changed, then post a message to display 
                // the new definitions arrival
                ProcessNewDefinitions ( hNotify );
	bMonitorDefChange = GetVirusDefsDir(szIncomingDir, _MAX_PATH);
    if ( bMonitorDefChange )
    {
        lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent = 
                FindFirstChangeNotification( 
                                szIncomingDir, 
                                TRUE, 
                                FILE_NOTIFY_CHANGE_FILE_NAME );

    aEvents[1] = lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent;
    }
#endif
                FindNextChangeNotification( lpNavMain->rDefinitionsStatus.hDefsUpdatedEvent );            
}
