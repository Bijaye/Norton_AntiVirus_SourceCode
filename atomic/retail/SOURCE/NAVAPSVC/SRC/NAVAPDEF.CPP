// Copyright 1996 Symantec Corporation
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVAPSVC/VCS/NAVAPDEF.CPv   1.8   29 May 1998 23:52:46   mdunn  $
//
// Description:
//    This source file contains routines for watching the Definition files
//    when we discover that the definition files have changed, we send
//    a re-load configuration message to the device driver.
//
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVAPSVC/VCS/NAVAPDEF.CPv  $
//
//    Rev 1.8   29 May 1998 23:52:46   mdunn
// Fixed handle comparison in GetDefFileDates() from != to == so the handle
// is only used when it's valid.
//
//    Rev 1.7   28 May 1998 06:22:42   RStanev
// Keep on patching.
//
//    Rev 1.6   28 May 1998 06:07:36   RStanev
// Asking AP to reload options when longmsg.txt is changed.
//
//    Rev 1.5   12 Aug 1997 13:28:34   JBRENNA
// Change UpdateDefDateInRegistry to use CDefUtils for retrieving the
// definition date. This is far better than relying on the VirList.DLL for
// callbacks. Also, this works.
//
//    Rev 1.4   13 Jun 1997 10:24:14   JBRENNA
// 1. Rename UpdateDefSvcImagePaths to UpdateDefServicePaths.
// 2. Rather than updating the registry's ImagePath setting directly, use the
//    CServiceControl class. This is portable for WinNT 3.51 and 4.0.
//
//    Rev 1.3   06 Jun 1997 11:31:56   JBRENNA
// 1. Changed all TEXT() macros to _T().
// 2. Rename DefsNeedReLoading() to OptsNeedReLoading().
// 3. Change GetDefFileDates and OptsNeedReLoading to only process option
//    files. Removed watching of NAVEX, VirscanDat, and Virscan2Dat.
// 4. Add new DefsNeedReLoading, DefsSyncAppIds, and UpdateDefSvcImagePaths.
// 5. Change UpdateDefDateInRegistry to get the directory from the CDefUtils
//    library. ... specifically the m_DefUtilsAppId1 instance.
// 6. Change the way that DefinitionMonditor watches the options files and
//    definition files. The options are processed the same. The definitions
//    are checked every two minutes using CDefUtils member functions.
//
//    Rev 1.2   30 Apr 1997 14:01:56   JBRENNA
// Remove the prototypes for NameStripFile and NameAppendFile when including
// the XAPI.H header file. This ensures Dan Sackinger's hack continues to
// work.
//
//    Rev 1.1   18 Apr 1997 13:59:26   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:12:18   RFULLER
// Initial revision
//
//    Rev 1.3   27 Jan 1997 18:18:26   DSACKIN
// Added function UpdateDefDateInRegistry.
//
//    Rev 1.2   24 Dec 1996 18:53:50   DSACKIN
// Added include "afxtmpl.h"
//
//    Rev 1.1   27 Nov 1996 03:45:52   DSACKIN
// Added 5 files to the list of files to watch for re-loading the definitions
//
//    Rev 1.0   08 Nov 1996 12:16:26   DKEEFER
// Initial revision
//
//    Rev 1.4   18 Oct 1996 18:23:26   TSmith
// Removed obsolete #ifdefs.
//
//    Rev 1.3   03 Sep 1996 15:52:48   DSACKIN
// Added code to support NAVEX's move to the NAV directory.
//
//    Rev 1.1   28 Aug 1996 17:23:56   DSACKIN
// Added code that will prevent us from re-loading the AP config while
// we are in the Definition List of NAVW
//
//    Rev 1.0   22 Aug 1996 00:40:14   DSACKIN
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//      Includes

// Quake & System Include Files
#include <afxtempl.h>
#include "platform.h"
#include "process.h"

#include "nonav.h"
#include "inoc.h"
#include "navalog.h"
#include "navaplog.h"
#include "navbin.h"
#include "actions.h"
#include "symalog.h"
#include "navutil.h"
                                        // #include "xapi.h"
#include "file.h"
#include "symstat.h"
#include "navstat.h"

#include "servenfy.h"

// NAV & AP Driver Include Files
#include "drvcomm.h"
#include "apcomm.h"
#include "apscomm.h"                    // For apps to communicate with me.


                                        // The Reason for creating a dependency
                                        // on virlist.h is because there is a
                                        // function in VirList that initializes
                                        // AVAPI so we can get a virus context
                                        // without doing all of the bookwork.
#include "virlist.h"

// Project Include files.
#include "service.h"
#include "NAVAPSVC.h"
#include "APSVCSTR.H"

//***************************************************************************/
//      Constants
//***************************************************************************/
#define         MAX_THREAD_STACK_SIZE           (10240)  // Max Stack size
#define         MAX_SERVICE_NAME                (256)


/////////////////////////////////////////////////////////////////////
// Globals

static LPTSTR SZ_VIRDEF_KEY = _T("SOFTWARE\\Symantec\\Norton AntiVirus\\Virus Defs\\LastUpdate");
static LPTSTR SZ_VIRDEF_VALUENAME = _T("SystemTime");

/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.

/////////////////////////////////////////////////////////////////////
// Functions

/////////////////////////////////////////////////////////////////////
// GetDefFileDates()
//
// This function gets the FILETIME information for all the files
// that we monitor when they have changed.
//
// Parameters:
//
//
// Returns:
//    BOOL - TRUE  - Defs need updating
//         - FALSE - Defs haven't changed.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#define NUMBER_OF_FILES_TO_WATCH 6

BOOL CNavAPService::GetDefFileDates(
   LPFILETIME lprNavOptsDat,            // [out] - returns time of NAVOPTS.DAT
   LPFILETIME lprNavStartDat,           // [out] - returns time of NAVSTART.DAT
   LPFILETIME lprExcludeDat,            // [out] - returns time of EXCLUDE.DAT
   LPFILETIME lprExcludeLDat,           // [out] - returns time of EXCLUDEL.DAT
   LPFILETIME lprNavAlertDat,           // [out] - returns time of NAVALERT.DAT
   LPFILETIME lprLongAlertTxt)          // [out] - returns time of LONGMSG.TXT
{
BOOL    bReturn = TRUE;
                                        // Filename strings
TCHAR   szNavOptsDat[SYM_MAX_PATH];
TCHAR   szNavStartDat[SYM_MAX_PATH];
TCHAR   szExcludeDat[SYM_MAX_PATH];
TCHAR   szExcludeLDat[SYM_MAX_PATH];
TCHAR   szNavAlertDat[SYM_MAX_PATH];
TCHAR   szLongAlertTxt[SYM_MAX_PATH];

LPTSTR     lpszFiles[NUMBER_OF_FILES_TO_WATCH] = {
              szNavOptsDat,
              szNavStartDat,
              szExcludeDat,
              szExcludeLDat,
              szNavAlertDat,
              szLongAlertTxt
              };

LPFILETIME lprFiles[NUMBER_OF_FILES_TO_WATCH] =  {
              lprNavOptsDat,
              lprNavStartDat,
              lprExcludeDat,
              lprExcludeLDat,
              lprNavAlertDat,
              lprLongAlertTxt
              };

HANDLE     hFile;
int        nCount;

   LogMessage(_T("Entering CNavAPService::GetDefFileDates"));

   STRCPY (szNavOptsDat, m_szNAVPath);
   STRCAT (szNavOptsDat, _T("\\"));
   STRCAT (szNavOptsDat, m_szNavOptsDat);

   STRCPY (szNavStartDat, m_szNAVPath);
   STRCAT (szNavStartDat, _T("\\"));
   STRCAT (szNavStartDat, m_szNavStartDat);

   STRCPY (szExcludeDat, m_szNAVPath);
   STRCAT (szExcludeDat, _T("\\"));
   STRCAT (szExcludeDat, m_szExcludeDat);

   STRCPY (szExcludeLDat, m_szNAVPath);
   STRCAT (szExcludeLDat, _T("\\"));
   STRCAT (szExcludeLDat, m_szExcludeLDat);

   STRCPY (szNavAlertDat, m_szNAVPath);
   STRCAT (szNavAlertDat, _T("\\"));
   STRCAT (szNavAlertDat, m_szNavAlertDat);

   STRCPY (szLongAlertTxt, m_szNAVPath);
   STRCAT (szLongAlertTxt, _T("\\"));
   STRCAT (szLongAlertTxt, m_szLongAlertTxt);

                                        // Now get the dates
   for (nCount = 0; bReturn && (nCount < NUMBER_OF_FILES_TO_WATCH); nCount++)
   {
      hFile = CreateFile(
                 lpszFiles[nCount],     // lpFileName
                 GENERIC_READ,          // dwDesiredAccess
                 FILE_SHARE_READ |
                    FILE_SHARE_WRITE,   // dwShareMode
                 NULL,                  // lpSecurityAttributes
                 OPEN_EXISTING,         // dwCreation Distributeion
                 FILE_ATTRIBUTE_NORMAL, // dwFlagsAndAttributes
                 0);                    // hTemplateFile


      if ((HANDLE)INVALID_HANDLE_VALUE == hFile &&
          lpszFiles[nCount] != szLongAlertTxt)
      {
         LogMessage(_T("CNavAPService::GetDefFileDates - Failed file open of [%s]"), lpszFiles[nCount]);

         bReturn = FALSE;
         break;
      }

                                        // keep on patching...
      if ((HANDLE)INVALID_HANDLE_VALUE == hFile)
      {
         memset(lprFiles[nCount],0,sizeof(*lprFiles[nCount]));
      }
      else
      {
         if (!GetFileTime(
                 hFile,                 // hFile
                 NULL,                  // lpCreationTime
                 NULL,                  // lpLastAccessTime
                 lprFiles[nCount]))     // lpLastWriteTime
         {
            LogMessage(_T("CNavAPService::GetDefFileDates - Failed FileGetTime"));
            bReturn = FALSE;
         }

         CloseHandle( hFile );
      }
   }

   LogMessage(_T("Exiting CNavAPService::GetDefFileDates with status of - %d"), (DWORD) bReturn);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// DefsSyncAppIds()
//
// This function synchronizes the AppId2 such that it is using the
// same definitions as AppId1. By moving the AppId2's usage, potentially
// the AppId2's old definitions will be removed from the system.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - AppIds are sync-ed
//         - FALSE - AppIds are not sync-ed
//
/////////////////////////////////////////////////////////////////////
// 06/05/97 - JBRENNA - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::DefsSyncAppIds(void)
{
char     szAppId1Path[_MAX_PATH];
BOOL     bReturn = TRUE;


   LogMessage(_T("Entering CNavAPService::DefsSyncAppIds"));

   bReturn = m_DefUtilsAppId1.GetCurrentDefs (szAppId1Path, sizeof (szAppId1Path));

   if (bReturn)
       bReturn = m_DefUtilsAppId2.SetCurrentDefs (szAppId1Path);

   LogMessage(_T("Exiting CNavAPService::DefsSyncAppIds with status: %d"), (DWORD) bReturn);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// DefsNeedReLoading()
//
// This function determines if we need to tell the Auto-Protect to
// re-load the virus definitions.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Defs need updating
//         - FALSE - Defs haven't changed.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
// 06/05/97 - JBRENNA - Changed to use CDefUtils for def management
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::DefsNeedReLoading(void)
{
BOOL     bReturn = FALSE;

   LogMessage(_T("Entering CNavAPService::DefsNeedReLoading"));

   if (!m_DefUtilsAppId1.UseNewestDefs (&bReturn))
       bReturn = FALSE;

   LogMessage(_T("Exiting CNavAPService::DefsNeedReLoading with status: %d"), (DWORD) bReturn);

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// OptsNeedReLoading()
//
// This function checks for the options to see if we need to
// tell the Auto-Protect to re-load them.
//
// This function has been built on the assumption that the only
// files that we will watch are NAVOPTS.DAT, NAVSTART.DAT, EXCLUDE.DAT,
// EXCLUDEL.DAT, and NAVALERT.DAT.
// All of the files we are looking for will need to be in the
// NAV directory (Same directory as this service.)
//
// Currently, we will only be checking the date and time of the files.
// Future releases are expected to use a new function in AVAPI that
// will do an actually verification of the file to see if we need to
// re-load.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Opts need updating
//         - FALSE - Opts haven't changed.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
// 06/05/97 - JBRENNA - Removed NAVEX, VirscanDat, and Virscan2.Dat
//                      checks.
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::OptsNeedReLoading(void)
{
FILETIME rNavOptsDat;
FILETIME rNavStartDat;
FILETIME rExcludeDat;
FILETIME rExcludeLDat;
FILETIME rNavAlertDat;
FILETIME rLongAlertTxt;

BOOL     bReturn = FALSE;

   LogMessage(_T("Entering CNavAPService::OptsNeedReLoading"));

   if (!GetDefFileDates(
           &rNavOptsDat,
           &rNavStartDat,
           &rExcludeDat,
           &rExcludeLDat,
           &rNavAlertDat,
           &rLongAlertTxt))
   {
                                        // If we can't get the new dates,
                                        // we definitely don't want to
                                        // re-load the defs
      goto Cleanup;
   }
                                        // Now check the dates
   if( (0 != MEMCMP( &m_rNavOptsDat,   &rNavOptsDat,   sizeof (FILETIME) )) ||
       (0 != MEMCMP( &m_rNavStartDat,  &rNavStartDat,  sizeof (FILETIME) )) ||
       (0 != MEMCMP( &m_rExcludeDat,   &rExcludeDat,   sizeof (FILETIME) )) ||
       (0 != MEMCMP( &m_rExcludeLDat,  &rExcludeLDat,  sizeof (FILETIME) )) ||
       (0 != MEMCMP( &m_rExcludeLDat,  &rExcludeLDat,  sizeof (FILETIME) )) ||
       (0 != MEMCMP( &m_rLongAlertTxt, &rLongAlertTxt, sizeof (FILETIME) )) )
   {
                                        // Something has changed

                                        // Take the new times.
      MEMCPY( &m_rNavOptsDat,   &rNavOptsDat,   sizeof (FILETIME) );
      MEMCPY( &m_rNavStartDat,  &rNavStartDat,  sizeof (FILETIME) );
      MEMCPY( &m_rExcludeDat,   &rExcludeDat,   sizeof (FILETIME) );
      MEMCPY( &m_rExcludeLDat,  &rExcludeLDat,  sizeof (FILETIME) );
      MEMCPY( &m_rNavAlertDat,  &rNavAlertDat,  sizeof (FILETIME) );
      MEMCPY( &m_rLongAlertTxt, &rLongAlertTxt, sizeof (FILETIME) );

      bReturn = TRUE;
   }

Cleanup:

   LogMessage(_T("Exiting CNavAPService::OptsNeedReLoading with status: %d"), (DWORD) bReturn);

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// void CNavAPService::UpdateDefDateInRegistry(void)
//
// Purpose:
//    This uses Hawking (CDefUtils) to set the definition date in
//    the registry. This allows non-NAV applications to determine the
//    virus definitions date.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 01/24/97 - DSACKIN - Function Created.
// 08/12/97 - JBRENNA - Updated the logic to use CDefUtils when computing
//                      the definitions date.
/////////////////////////////////////////////////////////////////////

void CNavAPService::UpdateDefDateInRegistry(void)
{
    WORD        wYear;
    WORD        wMonth;
    WORD        wDay;
    DWORD       dwVersion;
    SYSTEMTIME  SysTime;
    LONG        lStatus;
    HKEY        hKey;
    DWORD       dwDisposition;

    memset (&SysTime, 0, sizeof (SysTime));

                                        // Get the definition date.
    m_DefUtilsAppId1.GetCurrentDefsDate (&wYear, &wMonth, &wDay, &dwVersion);



                                        // Create, if it doesn't already
                                        // exist, a registry entry for
                                        // keeping track of the date of the
                                        // virus defs in use by NAV.
    lStatus = RegCreateKeyEx (HKEY_LOCAL_MACHINE, SZ_VIRDEF_KEY, 0, _T(""),
                              REG_OPTION_NON_VOLATILE,
                              KEY_ALL_ACCESS, NULL, &hKey,
                              &dwDisposition);

    if (lStatus == ERROR_SUCCESS)
    {
        SysTime.wMonth = wMonth;
        SysTime.wDay   = wDay;
        SysTime.wYear  = wYear;

                                        // Store that date in the registry.
        lStatus = RegSetValueEx (hKey, SZ_VIRDEF_VALUENAME, 0, REG_BINARY,
                                 (BYTE *)&SysTime, sizeof (SysTime));

        RegCloseKey (hKey);
    }
}


/////////////////////////////////////////////////////////////////////
// UpdateDefServicePaths
//
// Remember that the full path to the shared definitions changes with
// each new definition set. Also remember that NAVEX and NAVENG are
// loaded as a service. As such, the caller must stop the NAVEX/NAVENG
// service, call this function, and start the NAVEX/NAVENG service.
// This function updates the path to NAVEX and NAVENG  to point to the
// current definition directory.
//
/////////////////////////////////////////////////////////////////////
// 06/05/97 JBRENNA - Function Created
// 06/12/97 JBRENNA - Changed to use CServiceControl class. Previously,
//                    was updating the registries ImagePath setting
//                    directly.
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::UpdateDefServicePaths(void)
{
    BOOL            bResult    = TRUE;
    TCHAR           tszFmt[]   = _T("%s\\%s");
    TCHAR           tszSvcName[MAX_SERVICE_NAME];
    char            szDefDir[SYM_MAX_PATH];
    TCHAR           tszDefDir[SYM_MAX_PATH];
    TCHAR           tszSysName[SYM_MAX_PATH];
    TCHAR           tszFullPath[SYM_MAX_PATH];
    CServiceControl ServiceControl;

    LogMessage(_T("Entering CNavAPService::UpdateDefServicePaths()"));

    if (!m_DefUtilsAppId1.GetCurrentDefs (szDefDir, sizeof (szDefDir)))
        bResult = FALSE;

    if (bResult)
    {
                                        // Convert the Multibyte string to
                                        // unicode when we are building Unicode.
#ifdef _UNICODE
        bResult = (0 != MultiByteToWideChar (CP_ACP, 0, szDefDir, -1, tszDefDir,
                                             sizeof (tszDefDir) / sizeof (TCHAR)));
#else
        STRCPY (tszDefDir, szDefDir);
#endif
    }

    //////////////////////////////////////////////////////
    // Update NAVEX's path.
    //////////////////////////////////////////////////////
    if (bResult)
        bResult = (0 != LoadString(m_hInst,
                                   IDS_NAVAP_NAVEX,
                                   tszSvcName,
                                   sizeof (tszSvcName) / sizeof (tszSvcName[0])));

    if (bResult)
        bResult = (0 != LoadString(m_hInst,
                                   IDS_NAVAP_NAVEX_SYS,
                                   tszSysName,
                                   sizeof (tszSysName) / sizeof (tszSysName[0])));

    if (bResult)
    {
        wsprintf (tszFullPath, tszFmt, tszDefDir, tszSysName);

                                        // Set the services path value.
                                        // Remember that NAVEX is optional.
                                        // Do not error.
        ServiceControl.Open (tszSvcName);
        ServiceControl.ChangeServiceConfig (SERVICE_NO_CHANGE,
                                            SERVICE_NO_CHANGE,
                                            SERVICE_NO_CHANGE,
                                            tszFullPath);
    }

    //////////////////////////////////////////////////////
    // Update NAVENG's path.
    //////////////////////////////////////////////////////
    if (bResult)
        bResult = (0 != LoadString(m_hInst,
                                   IDS_NAVAP_NAVENG,
                                   tszSvcName,
                                   sizeof (tszSvcName) / sizeof (tszSvcName[0])));

    if (bResult)
        bResult = (0 != LoadString(m_hInst,
                                   IDS_NAVAP_NAVENG_SYS,
                                   tszSysName,
                                   sizeof (tszSysName) / sizeof (tszSysName[0])));

    if (bResult)
    {
        wsprintf (tszFullPath, tszFmt, tszDefDir, tszSysName);

                                        // Set the services path value.
                                        // Remember that NAVENG is optional.
                                        // Do not error.
        ServiceControl.Open (tszSvcName);
        ServiceControl.ChangeServiceConfig (SERVICE_NO_CHANGE,
                                            SERVICE_NO_CHANGE,
                                            SERVICE_NO_CHANGE,
                                            tszFullPath);
    }

    LogMessage(_T("Exiting CNavAPService::UpdateDefServicePaths() with a %d result"), (DWORD)bResult);

    return bResult;
}


/////////////////////////////////////////////////////////////////////
// DefinitionMonitor()
//
// This function monitors the definitions to see when we need to
// update them.  We do so using the Win32 API for FindFirstChangeNotification
// and FindNextChangeNotification.  The functions changed when
// Microsoft came out with Service Pack 3 to where they will actually
// tell us which files changed.  For ease of implementation, we will not be
// requesting which files have changed, but we will be checking them
// manually.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Success
//         - FALSE - Faulire
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
#define MONITOR_WAIT_MS			 1000  // 1 second
#define VIRUSDEFS_CHANGE_TIMEOUT 60000 // 1 minute
VOID CNavAPService::DefinitionMonitor(VOID)
{
                                        // Check for new definitions every
                                        // 120 times the TIMEOUT occurs. As
                                        // MONITOR_WAIT_MS is set to 1 second,
                                        // This should check about every 2
                                        // minutes.
const DWORD dwDefsChkMod = 120;

HANDLE hEvent;
DWORD  dwReturn;
BOOL   bContinue = TRUE;
BOOL   bCheckDefs = FALSE;
BOOL   bLocalPauseWatchingDefs = FALSE;
BOOL   bReloadDefs = FALSE;
BOOL   bDefUpdateInProgress = FALSE;
DWORD  dwTimeOutCounter = 0;

   LogMessage(_T("Entering CNavAPService::DefinitionMonitor"));

                                        // Now create our events
   hEvent = FindFirstChangeNotification(
               m_szVirusDefFolder,
               TRUE,                   // Don't watch the subtree
               FILE_NOTIFY_CHANGE_LAST_WRITE );

   if ((HANDLE)INVALID_HANDLE_VALUE == hEvent )
   {
      goto Cleanup;
   }
                                        // Now that we have the events,
                                        // its time to start monitoring

   while (bContinue && !m_bExitingService)
   {
      dwReturn = WaitForSingleObject(
                    hEvent,
                    MONITOR_WAIT_MS);

      bCheckDefs = FALSE;

      switch (dwReturn)
      {
         case WAIT_OBJECT_0:

            bDefUpdateInProgress = TRUE;
            dwTimeOutCounter = 0;       // This means files are still changing
                                        // and we should still wait....

         break;
                                                // Did we time out?
												// Only preform defupdate if 
												// enough time has pass...
         case WAIT_TIMEOUT:

            if( bDefUpdateInProgress )
            {
                if( dwTimeOutCounter < VIRUSDEFS_CHANGE_TIMEOUT )
                {
                    dwTimeOutCounter += MONITOR_WAIT_MS;
                }
                else
                {
                                                // Enough time has pass, let's
                                                // check the defs...
                    bCheckDefs = TRUE;

                                                // Reset state...
                    bDefUpdateInProgress = FALSE;
                    dwTimeOutCounter     = 0;
                }
            }

         break;

         default:
            LogMessage(_T("CNavAPService::DefinitionMonitor - Unknown return from WaitForMultipleObjects"));

            bCheckDefs = FALSE;
            break;
      }

                                        // Now that we know if we want to
                                        // check the defs, make sure that
                                        // NAVW isn't updating the VirusList
                                        // Or if it is done, force a check anyway
      if (m_bPauseWatchingDefs)
      {
                                        // If NAVW is updating the virus list,
                                        // we need to make sure we don't
                                        // check the defs
         bLocalPauseWatchingDefs = TRUE;
         bCheckDefs = FALSE;
      }
      else // Not updating the list..
      {
                                        // Has this state recently changed?
         if (bLocalPauseWatchingDefs)
         {
                                        // Yes? Force a Def check.
            bCheckDefs = TRUE;
            bLocalPauseWatchingDefs = FALSE;
         }
      }

                                        // Now if we are still interested
                                        // in checking the defs, do so.
      if (bCheckDefs)
      {
         bReloadDefs = FALSE;

         LogMessage(_T("CNavAPService::DefinitionMonitor - Checking for new definitions (%d) ..."), (DWORD)bCheckDefs);

         if (bCheckDefs)
            bReloadDefs = DefsNeedReLoading();

                                        // Reload if something decided it needs
                                        // reloading.
         if (bReloadDefs)
         {
            LogMessage(_T("CNavAPService::DefinitionMonitor - Attempting to Re-Load AutoProtect"));

                                        // Something changed that mattered
            if ( !ReLoadAutoProtect() )
            {
               LogMessage(_T("CNavAPService::DefinitionMonitor - Failed to Re-Load AutoProtect"));
            }
            if (bReloadDefs)
            {
                UpdateDefDateInRegistry();

                if (!DefsSyncAppIds())
                {
                    LogMessage (_T("CNavAPService::DefinitionMonitor - Failed to synchronize AppId1 & AppId2"));
                }
            }
         }
      }

                                        // If something DID change, re-set
                                        // the change notification event
      if ( WAIT_OBJECT_0 == dwReturn )
      {
         bContinue = FindNextChangeNotification(
                        hEvent);
      }
   }

Cleanup:

   if ((HANDLE)INVALID_HANDLE_VALUE != hEvent)
   {
      FindCloseChangeNotification (hEvent);
      hEvent = (HANDLE)INVALID_HANDLE_VALUE;
   }

   LogMessage(_T("Exiting CNavAPService::DefinitionMonitor"));

}


/////////////////////////////////////////////////////////////////////
// DefinitionMonitorProc()
//
// This function is a passthrough to our DefinitionMonitor function
//
// Parameters:
//    LPVOID - (CNavAPService * lpObject) - Pointer to our object.
//
// Returns:
//    Doesn't return.  We call endthread to exit this function.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID DefinitionMonitorProc(LPVOID lpObject)
{
CNavAPService *pThis;

   if (NULL != lpObject)
   {
                                        // Get our this pointer
      pThis = (CNavAPService *) lpObject;

                                        // And then call our monitor
      pThis->DefinitionMonitor();
   }

   _endthread();                        // Don't just return!!!
}

/////////////////////////////////////////////////////////////////////
// This function is a passthrough to our OptionsMonitor function
//
//
//
/////////////////////////////////////////////////////////////////////
// 08/24/99 - CBROWN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID OptionsMonitorProc( LPVOID lpObject )
{
    CNavAPService *pThis;

    if (NULL != lpObject)
    {
                                        // Get our this pointer
        pThis = (CNavAPService *) lpObject;

                                        // And then call our monitor
        pThis->OptionsMonitor();
    }

    _endthread();                        // Don't just return!!!
}


/////////////////////////////////////////////////////////////////////
// OptionsMonitor()
//
// This function monitors the options to see when we need to
// update them.  We do so using the Win32 API for FindFirstChangeNotification
// and FindNextChangeNotification.  The functions changed when
// Microsoft came out with Service Pack 3 to where they will actually
// tell us which files changed.  For ease of implementation, we will not be
// requesting which files have changed, but we will be checking them
// manually.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Success
//         - FALSE - Faulire
//
/////////////////////////////////////////////////////////////////////
// 08/24/99 - CBROWN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::OptionsMonitor(VOID)
{
                                        // Check for new definitions every
                                        // 120 times the TIMEOUT occurs. As
                                        // MONITOR_WAIT_MS is set to 1 second,
                                        // This should check about every 2
                                        // minutes.
    HANDLE hEvent      (0);
    DWORD  dwReturn    (0);
    BOOL   bContinue   (TRUE);
    BOOL   bCheckOpts  (FALSE);
    BOOL   bReloadOpts (FALSE);

    LogMessage(_T("Entering CNavAPService::OptionsMonitor"));

                                        // Now create our events
    hEvent = FindFirstChangeNotification( m_szNAVPath,
                                          FALSE,                   // Don't watch the subtree - not
                                          FILE_NOTIFY_CHANGE_LAST_WRITE );


    if( INVALID_HANDLE_VALUE == hEvent )
    {
        goto Cleanup;
    }
                                        // Now that we have the events,
                                        // its time to start monitoring
    while( bContinue && !m_bExitingService )
    {
        dwReturn = WaitForSingleObject( hEvent,
                                        MONITOR_WAIT_MS );

        switch( dwReturn )
        {
                                        // Did something change?
            case WAIT_OBJECT_0:
                bCheckOpts = TRUE;

            break;

            case WAIT_TIMEOUT:         // Did we time out?
            default:                   // Or did someone abort?

                bCheckOpts = FALSE;

            break;
        }
                                        // Now if we are still interested
                                        // in checking the defs, do so.
        if( bCheckOpts )
        {
            LogMessage(_T("CNavAPService::DefinitionMonitor - Checking for new options..."));

            if( bCheckOpts )
            {
                bReloadOpts = OptsNeedReLoading();
            }
                                        // Reload if something decided it needs
                                        // reloading.
            if( bReloadOpts )
            {
                LogMessage(_T("CNavAPService::DefinitionMonitor - Attempting to Re-Load AutoProtect Options"));

                                        // Reload the options...
                ReLoadOptions();
                                        // Now reload AutoProtect...
                ReLoadAutoProtect();
            }

        }
                                        // If something DID change, reset
                                        // the change notification event
        if( WAIT_OBJECT_0 == dwReturn )
        {
             bContinue = FindNextChangeNotification( hEvent );
        }
    }

Cleanup:

   if ((HANDLE)INVALID_HANDLE_VALUE != hEvent)
   {
      FindCloseChangeNotification( hEvent );

      hEvent = INVALID_HANDLE_VALUE;
   }

   LogMessage(_T("Exiting CNavAPService::OptionsMonitor"));
}



/////////////////////////////////////////////////////////////////////
// StartDefinitionMonitor()
//
// This function does a beginthread on our DefinitionMonitor.
//
// Parameters:
//    none
//
// Returns:
//    BOOL - TRUE  - Success
//         - FALSE - Faulire
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::StartDefinitionMonitor(VOID)
{
TCHAR  szTemp[SYM_MAX_PATH];

HANDLE hThread;
BOOL   bReturn = TRUE;
DWORD  dwReturn;
int    nRet;

HKEY  hAvengeDefKey;
DWORD dwValueType  = 0;
DWORD dwBufferSize = 0;


#ifdef UNICODE
LPTSTR pPtr;
#endif // ifdef UNICODE

   LogMessage(_T("Entering CNavAPService::StartDefinitionMonitor"));

   nRet = LoadString(
             m_hInst,
             IDS_NAVAP_NAVOPTS_DAT,
             m_szNavOptsDat,
             sizeof (m_szNavOptsDat) / sizeof (TCHAR));

   if (nRet > 0)
   {
      nRet = LoadString(
                m_hInst,
                IDS_NAVAP_NAVSTART_DAT,
                m_szNavStartDat,
                sizeof (m_szNavStartDat) / sizeof (TCHAR));
   }

   if (nRet > 0)
   {
      nRet = LoadString(
                m_hInst,
                IDS_NAVAP_EXCLUDE_DAT,
                m_szExcludeDat,
                sizeof (m_szExcludeDat) / sizeof (TCHAR));
   }

   if (nRet > 0)
   {
      nRet = LoadString(
                m_hInst,
                IDS_NAVAP_EXCLUDEL_DAT,
                m_szExcludeLDat,
                sizeof (m_szExcludeLDat) / sizeof (TCHAR));
   }

   if (nRet > 0)
   {
      nRet = LoadString(
                m_hInst,
                IDS_NAVAP_NAVALERT_DAT,
                m_szNavAlertDat,
                sizeof (m_szNavAlertDat) / sizeof (TCHAR));
   }

   if (nRet > 0)
   {
      nRet = LoadString(
                m_hInst,
                IDS_NAVAP_LONGALERT_TXT,
                m_szLongAlertTxt,
                sizeof (m_szLongAlertTxt) / sizeof (TCHAR));
   }

   if (0 == nRet)
   {
      ErrorHandler(
         ID_ERR_LOADSTRING_FAILED);

      bReturn = FALSE;
   }

   dwReturn = GetModuleFileName(
                 m_hInst,
                 szTemp,
                 sizeof (szTemp) / sizeof (TCHAR));

   if (0 == dwReturn)
   {
      ErrorHandler(
         ID_ERR_GETMODULEFILENAME);

      bReturn = FALSE;
                                        // Did we get a module filename?
      goto Cleanup;
   }

#ifdef UNICODE
                                        // In UNICODE mode, we can't use
                                        // the nifty NameReturnPath function,
                                        // but we CAN traverse strings backwards
   pPtr = &szTemp[STRLEN(szTemp) - 1];

   while (*pPtr != '\\')
      pPtr--;

   *pPtr = EOS;

   STRCPY (m_szNAVPath, szTemp);

#else // ifdef UNICODE
                                        // Now chop off the filename
   NameReturnPath(szTemp, m_szNAVPath);

#endif // ifdef UNICODE

                                        // Now get the current dates
   if (!GetDefFileDates(
           &m_rNavOptsDat,
           &m_rNavStartDat,
           &m_rExcludeDat,
           &m_rExcludeLDat,
           &m_rNavAlertDat,
           &m_rLongAlertTxt))
   {
      ErrorHandler (
         ID_ERR_GETDEFFILEDATES);
                                        // If we can't get the new dates,
                                        // we don't have anything to check for
                                        // and we need to abort.
      bReturn = FALSE;
      goto Cleanup;
   }

                                        //
                                        // Load string containing location on Avenge defs...
                                        //
   dwBufferSize = sizeof( m_szVirusDefFolder );

   if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                      _T("SOFTWARE\\Symantec\\InstalledApps"),
                                      0,
                                      KEY_READ|KEY_QUERY_VALUE,
                                      &hAvengeDefKey) )
   {
      ErrorHandler( ID_ERR_LOADSTRING_FAILED );
      bReturn = FALSE;

      goto Cleanup;
   }

   if( ERROR_SUCCESS != RegQueryValueEx( hAvengeDefKey,
                                        _T("AVENGEDEFS"),
                                         0,
                                         &dwValueType,
                                         reinterpret_cast<LPBYTE>(m_szVirusDefFolder),
                                         &dwBufferSize) )
   {
      ErrorHandler( ID_ERR_LOADSTRING_FAILED );
      bReturn = FALSE;

      RegCloseKey( hAvengeDefKey );

      goto Cleanup;
   }

   RegCloseKey( hAvengeDefKey );

                                        // Now that we have the two directories,
                                        // lets start up the monitor thread

   hThread = (HANDLE)_beginthread (
                  DefinitionMonitorProc,// start Addr
                  MAX_THREAD_STACK_SIZE,// stack size
                  (LPVOID) this);       // Let it know what object!

   if ((HANDLE)INVALID_HANDLE_VALUE == hThread)
   {
      ErrorHandler (
         ID_ERR_BEGINTHREAD_FAILED);

      bReturn = FALSE;
      goto Cleanup;
   }
                                        //
                                        // Now start the options monitor thread.
                                        //
   hThread = (HANDLE)_beginthread ( OptionsMonitorProc,// start Addr
                                    MAX_THREAD_STACK_SIZE,// stack size
                                    (LPVOID) this);       // Let it know what object!

   if( (HANDLE)INVALID_HANDLE_VALUE == hThread )
   {
      ErrorHandler (ID_ERR_BEGINTHREAD_FAILED);

      bReturn = FALSE;
      goto Cleanup;
   }

Cleanup:

   LogMessage(_T("Exiting CNavAPService::StartDefinitionMonitor"));

   return bReturn;
}

