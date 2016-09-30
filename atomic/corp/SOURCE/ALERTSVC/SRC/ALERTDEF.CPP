// Copyright 1996 Symantec Corporation
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/alertdef.cpv   1.2   14 Aug 1997 18:21:40   DSACKIN  $
//
// Description:
//    This source file contains routines for watching the Definition files
//    when we discover that the definition files have changed, we send
//    a re-load configuration message to the device driver.
//
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/ALERTSVC/VCS/alertdef.cpv  $
// 
//    Rev 1.2   14 Aug 1997 18:21:40   DSACKIN
// We are calling a function now to check the RPCServer when we have file activity
//
//    Rev 1.1   09 Jun 1997 17:10:48   DSACKIN
// Only watch NavOpts.Dat and NavAlert.Dat
//
//    Rev 1.0   18 Apr 1997 13:41:08   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:06:10   RFULLER
// Initial revision
//
//    Rev 1.1   19 Dec 1996 13:24:38   DSACKIN
// Needed to include SVCCTRL.H
//
//    Rev 1.0   27 Nov 1996 05:31:24   DSACKIN
// Initial revision.
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
#include    "afxtempl.h"
#include    "platform.h"
#include    "process.h"
#include    "winnetwk.h"
#include    "lmcons.h"
#include    "lmmsg.h"
#include    "lmerr.h"

#include    "nonav.h"
#include    "inoc.h"
#include    "navalog.h"
#include    "navaplog.h"
#include    "navbin.h"
#include    "actions.h"
#include    "symalog.h"
#include    "navutil.h"
#include    "xapi.h"
#include    "file.h"
#include    "symstat.h"
#include    "navstat.h"

#include    "linksrv.h"
#include    "servenfy.h"

// NAV & AP Driver Include Files
#include    "drvcomm.h"
#include    "apcomm.h"
#include    "apscomm.h"             // For apps to communicate with me.

// Project Include files.
#include    "service.h"
#include    "svcctrl.h"
#include    "ALERTSVC.h"
#include    "ALERTSTR.H"

//***************************************************************************/
//      Constants
//***************************************************************************/
#define         MAX_THREAD_STACK_SIZE           (10240)  // Max Stack size
#define         MAX_AP_BUFSIZE                  (10240)
#define         MAX_SERVICE_NAME                (256)


/////////////////////////////////////////////////////////////////////
// Globals

/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.

/////////////////////////////////////////////////////////////////////
// Functions

/////////////////////////////////////////////////////////////////////
// GetDefFileDates()
//
// This function gets the FILETIME information for NAVEX.SYS,
// VIRSCAN.DAT and VIRSCAN2.DAT
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
// 06-09-97 - DSACKIN - Only watch NavOpts and NavAlert
/////////////////////////////////////////////////////////////////////

#define NUMBER_OF_FILES_TO_WATCH 2

BOOL CAlertService::GetDefFileDates(
   LPFILETIME lprNavOptsDat,            // [out] - returns time of NAVOPTS.DAT
   LPFILETIME lprNavAlertDat)           // [out] - returns time of NAVALERT.DAT
{
BOOL    bReturn = TRUE;
                                        // Filename strings
TCHAR   szNavOptsDat[SYM_MAX_PATH];
TCHAR   szNavAlertDat[SYM_MAX_PATH];

LPTSTR     lpszFiles[NUMBER_OF_FILES_TO_WATCH] = {
              szNavOptsDat,
              szNavAlertDat
              };

LPFILETIME lprFiles[NUMBER_OF_FILES_TO_WATCH] =  {
              lprNavOptsDat,
              lprNavAlertDat
              };

HANDLE     hFile;
int        nCount;

   LogMessage(TEXT("Entering CAlertService::GetDefFileDates"));

                                        // First, build all of the names
   STRCPY (szNavOptsDat, m_szNAVPath);
   STRCAT (szNavOptsDat, TEXT("\\"));
   STRCAT (szNavOptsDat, m_szNavOptsDat);

   STRCPY (szNavAlertDat, m_szNAVPath);
   STRCAT (szNavAlertDat, TEXT("\\"));
   STRCAT (szNavAlertDat, m_szNavAlertDat);

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


      if ((HANDLE)INVALID_HANDLE_VALUE == hFile)
      {
         LogMessage(TEXT("CAlertService::GetDefFileDates - Failed file open of [%s]"), lpszFiles[nCount]);

         bReturn = FALSE;
         break;
      }

      if (!GetFileTime(
              hFile,                    // hFile
              NULL,                     // lpCreationTime
              NULL,                     // lpLastAccessTime
              lprFiles[nCount]))        // lpLastWriteTime
      {
         LogMessage(TEXT("CAlertService::GetDefFileDates - Failed FileGetTime"));
         bReturn = FALSE;
      }

      CloseHandle( hFile );
   }

   LogMessage(TEXT("Exiting CAlertService::GetDefFileDates with status of - %d"), (DWORD) bReturn);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// DefsNeedReLoading()
//
// This function checks for the definitions to see if we need to
// tell the Auto-Protect to re-load them.
//
// This function has been built on the assumption that the only
// files that we will watch are NAVEX.SYS, VIRSCAN.DAT and VIRSCAN2.DAT.
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
//    BOOL - TRUE  - Defs need updating
//         - FALSE - Defs haven't changed.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CAlertService::DefsNeedReLoading(VOID)
{
FILETIME rNavOptsDat;
FILETIME rNavAlertDat;

BOOL     bReturn = FALSE;

   LogMessage(TEXT("Entering CAlertService::DefsNeedReLoading"));

   if (!GetDefFileDates(
           &rNavOptsDat,
           &rNavAlertDat))
   {
                                        // If we can't get the new dates,
                                        // we definitely don't want to
                                        // re-load the defs
      goto Cleanup;
   }
                                        // Now check the dates
   if( (0 != MEMCMP( &m_rNavOptsDat,  &rNavOptsDat,  sizeof (FILETIME) )) ||
       (0 != MEMCMP( &m_rNavAlertDat, &rNavAlertDat, sizeof (FILETIME) )) )
   {
                                        // Something has changed

                                        // Take the new times.
      MEMCPY( &m_rNavOptsDat,  &rNavOptsDat,  sizeof (FILETIME) );
      MEMCPY( &m_rNavAlertDat, &rNavAlertDat, sizeof (FILETIME) );

      bReturn = TRUE;
   }

Cleanup:

   LogMessage(TEXT("Exiting CAlertService::DefsNeedReLoading with status: %d"), (DWORD) bReturn);

   return bReturn;
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
// 08/12/97 - DSACKIN - Added RPC Server Check.
/////////////////////////////////////////////////////////////////////
#define MONITOR_WAIT_MS 1000
VOID CAlertService::DefinitionMonitor(VOID)
{
HANDLE hEvent;
DWORD  dwReturn;
BOOL   bContinue = TRUE;
BOOL   bCheckDefs = FALSE;
BOOL   bLocalPauseWatchingDefs = FALSE;

   LogMessage(TEXT("Entering CAlertService::DefinitionMonitor"));

                                        // Now create our events

   hEvent = FindFirstChangeNotification(
               m_szNAVPath,
               FALSE,                   // Don't watch the subtree
               FILE_NOTIFY_CHANGE_LAST_WRITE);

   if ((HANDLE)INVALID_HANDLE_VALUE == hEvent )
   {
      goto Cleanup;
   }


                                        // Now that we have the events,
                                        // its time to start monitoring

   while (bContinue) // !Zoso! && !m_bExitingService)
   {
      dwReturn = WaitForSingleObject(
                    hEvent,
                    MONITOR_WAIT_MS);

      bCheckDefs = FALSE;

      switch (dwReturn)
      {
                                        // Did something change?
         case WAIT_OBJECT_0:
            bCheckDefs = TRUE;
            break;

                                        // Did we time out?
         case WAIT_TIMEOUT:
            bCheckDefs = FALSE;
            break;

                                        // Or did someone abort?
         default:
            LogMessage(TEXT("CAlertService::DefinitionMonitor - Unknown return from WaitForMultipleObjects"));

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
         if (DefsNeedReLoading())
         {
            LogMessage(TEXT("CAlertService::DefinitionMonitor - Attempting to Re-Load AutoProtect"));

            LoadOptions();
         }

                                        // If we were suppsed to check the
                                        // defs, something in this directory
                                        // has changed.  It is time to check
                                        // our RPC Server status in case the
                                        // NOSAP.DAT file has shown up.
         CheckRPCServer();
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

   LogMessage(TEXT("Exiting CAlertService::DefinitionMonitor"));

}


/////////////////////////////////////////////////////////////////////
// DefinitionMonitorProc()
//
// This function is a passthrough to our DefinitionMonitor function
//
// Parameters:
//    LPVOID - (CAlertService * lpObject) - Pointer to our object.
//
// Returns:
//    Doesn't return.  We call endthread to exit this function.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID DefinitionMonitorProc(LPVOID lpObject)
{
CAlertService *pThis;

   if (NULL != lpObject)
   {
                                        // Get our this pointer
      pThis = (CAlertService *) lpObject;

                                        // And then call our monitor
      pThis->DefinitionMonitor();
   }

   _endthread();                        // Don't just return!!!
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
BOOL CAlertService::StartDefinitionMonitor(VOID)
{
TCHAR  szTemp[SYM_MAX_PATH];

HANDLE hThread;
BOOL   bReturn = TRUE;
DWORD  dwReturn;
int    nRet;

#ifdef UNICODE
LPTSTR pPtr;
#endif // ifdef UNICODE

   LogMessage(TEXT("Entering CAlertService::StartDefinitionMonitor"));

   nRet = LoadString(
               m_hInst,
               IDS_ALERT_NAVOPTS_DAT,
               m_szNavOptsDat,
               sizeof (m_szNavOptsDat) / sizeof (TCHAR));

   if (nRet > 0)
   {
      nRet = LoadString(
                m_hInst,
                IDS_ALERT_NAVALERT_DAT,
                m_szNavAlertDat,
                sizeof (m_szNavAlertDat) / sizeof (TCHAR));
   }

   if (0 == nRet)
   {
      ErrorHandler(
         ID_ERR_LOADSTRING);

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
           &m_rNavAlertDat))
   {
      ErrorHandler (
         ID_ERR_GETDEFFILEDATES);
                                        // If we can't get the new dates,
                                        // we don't have anything to check for
                                        // and we need to abort.
      bReturn = FALSE;
      goto Cleanup;
   }
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

Cleanup:

   LogMessage(TEXT("Exiting CAlertService::StartDefinitionMonitor"));

   return bReturn;;

}

