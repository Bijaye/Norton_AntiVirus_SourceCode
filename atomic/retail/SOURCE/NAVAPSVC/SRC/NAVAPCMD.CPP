// Copyright 1996 Symantec Corporation
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVAPSVC/VCS/navapcmd.cpv   1.9   24 Jun 1998 22:54:46   RStanev  $
//
// Description:
//    This is where we will field all calls from other applications.
//    This includes requests to scan floppy disks, requests to re-load
//    the auto-protect and the like.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVAPSVC/VCS/navapcmd.cpv  $
// 
//    Rev 1.9   24 Jun 1998 22:54:46   RStanev
// More quarantine fixes.
//
//    Rev 1.8   16 Mar 1998 14:43:04   tcashin
// Process the new control code to display Navw options.
//
//    Rev 1.7   17 Feb 1998 12:40:46   mdunn
// Ported NEC code from the ROSNEC codebase.
//
//    Rev 1.6   13 Aug 1997 22:37:04   DALLEE
// Only respond to SCANFLOPPYA from NAVAPW32 if we're configured to do
// floppy scans on shutdown.
//
//    Rev 1.5   11 Jul 1997 12:24:02   RStanev
// Added custom response support.
//
//    Rev 1.4   13 Jun 1997 10:23:14   JBRENNA
// Rename UpdateDefSvcImagePaths to UpdateDefServicePaths.
//
//    Rev 1.3   06 Jun 1997 11:17:50   JBRENNA
// Add call to UpdateDefSvcImagePaths() in ReLoadAutoProtect().
//
//    Rev 1.2   30 Apr 1997 12:26:54   RStanev
// Updated to match the new APCOMM functionality.
//
//    Rev 1.1   18 Apr 1997 13:59:20   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   06 Apr 1997 15:31:02   RStanev
// Updated to match the new DriverComm APIs and changes in NAVAP.
//
//    Rev 1.0   13 Mar 1997 15:12:16   RFULLER
// Initial revision
//
//    Rev 1.4   30 Jan 1997 15:00:52   DSACKIN
// We are now responsible for stopping the alert service's definition monitor
//
//    Rev 1.3   22 Jan 1997 15:30:28   TSmith
// Modified calls to 'ScanBoot' to accomodate that functions new argument (pvInBuffer).
//
//    Rev 1.2   13 Jan 1997 22:25:10   DSACKIN
// Added code to make the NAVAP Service wait until the Alert Service has
// completed its startup before sending across the first alert.
//
//    Rev 1.1   24 Dec 1996 18:53:42   DSACKIN
// Added include "afxtmpl.h"
//
//    Rev 1.0   08 Nov 1996 12:16:20   DKEEFER
// Initial revision
//
//    Rev 1.19   18 Oct 1996 18:23:24   TSmith
// Removed obsolete #ifdefs.
//
//    Rev 1.18   30 Aug 1996 03:14:30   DSACKIN
// Changed m_bUpdatingVirusList to m_bPauseWatchingDefs
// Changed NAVAPSVC_VIRUSLIST_ENTER to NAVAPSVC_PAUSE_WATCHING_DEFS
//
//    Rev 1.17   28 Aug 1996 17:22:08   DSACKIN
// Added support for new commands: NAVAPSVC_VIRUSLIST_ENTER and NAVAPSVC_VIRUSLIST_EXIT
//
//    Rev 1.16   08 Aug 1996 02:00:52   DSACKIN
// Fixed a problem with a misplaced parenthese that caused us to always
// allow the system to shut down even if an infected disk is in the A: drive.
//
//    Rev 1.15   06 Aug 1996 02:12:10   DSACKIN
// Added code to handle new method for re-loading options for NAVAP.
//
//    Rev 1.14   05 Aug 1996 15:55:28   DSACKIN
// Removed all calls to ErrorHandlerID because ErrorHandler will work now.
// LogEvent has been re-worked to not need a NULL string at the end.
//
//    Rev 1.13   04 Aug 1996 22:52:54   DSACKIN
// Added function WaitForStartup for NAVAPW to call when waiting for us
// to start.  This function waits on an event until something happens.
//
//    Rev 1.12   25 Jul 1996 21:28:32   DSACKIN
// Made the DrvComm handling more robust.
//
//    Rev 1.11   19 Jul 1996 00:57:52   DSACKIN
// Use our CLinkServer instead of relying on the ServiceControlManager for
// messages from the other applications.
//
//    Rev 1.10   02 Jul 1996 14:16:22   PVENKAT
// Replaced WinExec by CreateProcess on EXEC_PROGRAM to specify Desktop
// This solves #59907.
//
//    Rev 1.9   18 Jun 1996 15:40:10   DSACKIN
// Implemented a new packet receiving algorithm for our named pipe.
//
//    Rev 1.8   13 Jun 1996 17:23:50   DSACKIN
// Added DEBUG_PROFILE strings.
//
//    Rev 1.7   05 Jun 1996 22:26:28   DSACKIN
// Removed ReportStatus calls from our startup routine.
//
//    Rev 1.6   29 May 1996 13:20:10   DSACKIN
// Wrapped the re-loading of the options in a critical section.
//
//    Rev 1.5   28 May 1996 17:13:58   DSACKIN
// On the command to scan a floppy drive, check the repair flag.
//
//    Rev 1.4   21 May 1996 15:43:52   DSACKIN
// Added a command that allows us to scan the A: drive upon request.
//
//    Rev 1.3   16 May 1996 20:32:36   DSACKIN
// Added a new command to tell the Service to re-load the options file.
//
//    Rev 1.2   16 May 1996 15:30:48   DSACKIN
// Switched the order of drvcomm.h and apcomm.h
//
//    Rev 1.1   16 May 1996 13:21:44   DSACKIN
// SetStatus doesn't take a return value.
//
//    Rev 1.0   16 May 1996 11:09:30   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//      Includes

// Quake & System Include Files
#include "afxtempl.h"
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
#include "xapi.h"
#include "symstat.h"
#include "navstat.h"

#include "servenfy.h"

// NAV & AP Driver Include Files
#include "drvcomm.h"
#include "apcomm.h"
#include "apscomm.h"                // For apps to communicate with me.
#include "navrpc.h"
#include "msgreg.h"

// Project Include files.
#include "SERVICE.h"
#include "NAVAPSVC.h"
#include "APSVCSTR.H"

/////////////////////////////////////////////////////////////////////
//      Constants
#define         MAX_THREAD_STACK_SIZE           (4096)  // Max Stack size
#define         MAX_SERVICE_NAME                (256)


/////////////////////////////////////////////////////////////////////
// Externs
extern CNavAPService APService;

/////////////////////////////////////////////////////////////////////
// Globals

/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.
//

/////////////////////////////////////////////////////////////////////
// Functions

/////////////////////////////////////////////////////////////////////
// ReLoadAutoProtect
//
// This routine takes care of getting a new configuration into
// the NAVAP.
//
// Parameters:
//    none
//
// Return Values:
//
/////////////////////////////////////////////////////////////////////
// 04/24/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::ReLoadAutoProtect (VOID)
{
DriverComm   commNAVAP;
TCHAR        szNAVEX[MAX_SERVICE_NAME];
BOOL         bReturn = TRUE;
BOOL         bNAVAPStopped = FALSE;
DWORD        dwWaitResult;
DWORD        dwResult, dwResultSize;
int          nRet;
//MMENDON: 02-16-2000 NAVENG reload fix:  Need to reload NAVENG along with NAVEX
TCHAR        szNAVENG[MAX_SERVICE_NAME];
//MMENDON: 02-16-2000 NAVENG reload fix end

   LogMessage(_T("Entering CNavAPService::ReLoadAutoProtect"));

   LogMessage(_T("ReLoadAutoProtect - WaitForSingleObject"));

   dwWaitResult = WaitForSingleObject(
                     m_hMutex,          // Handle to our mutex
                     3000L);            // Wait 3 seconds

   EnterCriticalSection(&m_Critical);   // Make sure I'm the only one.

   if (WAIT_OBJECT_0 == dwWaitResult)   // Did we succeed?
   {

      LogMessage(_T("ReLoadAutoProtect - Starting Re-Load"));

//MMENDON: 02-16-2000 NAVENG reload fix:  Need to reload NAVENG along with NAVEX
      nRet = LoadString (m_hInst,
                IDS_NAVAP_NAVENG,
                szNAVENG,
                sizeof (szNAVENG) / sizeof (szNAVENG[0]));

                                     // Did the string load OK?
      if (0 == nRet)
      {
         bReturn = FALSE;
         goto AbortReLoad;
      }
//MMENDON: 02-16-2000 NAVENG reload fix end

      nRet = LoadString (m_hInst,
                IDS_NAVAP_NAVEX,
                szNAVEX,
                sizeof (szNAVEX) / sizeof (szNAVEX[0]));

                                     // Did the string load OK?
      if (0 == nRet)
      {
         bReturn = FALSE;
         goto AbortReLoad;
      }

      LogMessage(_T("ReLoadAutoProtect - Establishing Connection to Driver"));

                                        // Open Driver Communication
      if (!commNAVAP.Open( NAVAP_COMMUNICATION_NAME ) )
      {
         bReturn = FALSE;
         goto AbortReLoad;
      }

      LogMessage(_T("ReLoadAutoProtect - Sending: NAVAP_COMM_RELOADCONFIG"));

                                        // First tell the driver to reload the configuration
      dwResultSize = 0;
      if ( !(commNAVAP.Send( NAVAP_COMM_RELOADCONFIG, NULL, 0, &dwResult, sizeof(dwResult), &dwResultSize )) ||
           dwResultSize != sizeof(dwResult) ||
           dwResult != ERROR_SUCCESS )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed Sending NAVAP_COMM_RELOADCONFIG"));
         bReturn = FALSE;
      }

      LogMessage(_T("ReLoadAutoProtect - Sending: NAVAP_COMM_UNLOADAVAPI"));

                                        // First tell the driver to unload the defs
      dwResultSize = 0;
      if (!(commNAVAP.Send( NAVAP_COMM_UNLOADAVAPI, NULL, 0, &dwResult, sizeof(dwResult), &dwResultSize ) ) ||
           dwResultSize != sizeof(dwResult) ||
           dwResult != ERROR_SUCCESS )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed Sending NAVAP_COMM_UNLOADAVAPI"));
         bReturn = FALSE;
      }

//MMENDON: 02-16-2000 NAVENG reload fix:  Need to reload NAVENG along with NAVEX
      LogMessage(_T("ReLoadAutoProtect - Stopping NAVENG"));

                                        // Stopping NAVENG drops it out of memory
      if (!StopService( &m_NAVENG ) )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed Stopping NAVENG"));
         bReturn = FALSE;
      }
//MMENDON: 02-16-2000 end NAVENG reload fix


      LogMessage(_T("ReLoadAutoProtect - Stopping NAVEX"));

      // _JUNK notes:
      // 1. Should we be considering the NAVEX failures as errors? Notice that
      //    CNavAPService::StartServices() does not.
      // 2. Should we be attempting to stop & restart NAVENG? Notice that
      //    CNavAPService::StartServices() includes NAVENG processing.

#undef _JUNK
                                        // Stopping NAVEX drops it out of memory
      if (!StopService( &m_NAVEX ) )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed Stopping NAVEX"));
         bReturn = FALSE;
      }

      LogMessage(_T("ReLoadAutoProtect - Attempting to UpdateDefServicePaths"));
      if (!UpdateDefServicePaths() )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed UpdateDefServicePaths"));
         bReturn = FALSE;
      }

      LogMessage(_T("ReLoadAutoProtect - ReLoading NAVEX"));

                                        // Starting reloads it!
      if ( ERROR_SUCCESS != StartService(szNAVEX, &m_NAVEX) )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed ReLoading NAVEX"));
         bReturn = FALSE;
      }

//MMENDON: 02-16-2000 NAVENG reload fix:  Need to reload NAVENG along with NAVEX
      LogMessage(_T("ReLoadAutoProtect - ReLoading NAVENG"));

                                        // Starting reloads it!
      if ( ERROR_SUCCESS != StartService(szNAVENG, &m_NAVENG) )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed ReLoading NAVENG"));
         bReturn = FALSE;
      }
//MMENDON: 02-16-2000 end NAVENG reload fix

      LogMessage(_T("ReLoadAutoProtect - Sending: NAVAP_COMM_LOADAVAPI"));

                                        // Now load the defs again
      dwResultSize = 0;
      if (!commNAVAP.Send( NAVAP_COMM_LOADAVAPI, NULL, 0, &dwResult, sizeof(dwResult), &dwResultSize ) ||
           dwResultSize != sizeof(dwResult) ||
           dwResult != ERROR_SUCCESS )
      {
         LogMessage(_T("ReLoadAutoProtect - *** failed Sending NAVAP_COMM_LOADAVAPI"));
         bReturn = FALSE;
      }
                                        // This label is here because we
                                        // may want to abort our re-load,
                                        // but we will need to release our
                                        // mutex and do other cleanup.
AbortReLoad:

      LogMessage(_T("ReLoadAutoProtect - Releasing Mutex"));
                                        // At this point, we know we
                                        // own the mutex, so release it.
      if (!ReleaseMutex (m_hMutex))
      {
         bReturn = FALSE;
         SYM_ASSERT(0);
      }
   }
   else
   {
     ErrorHandler (ID_ERR_WAITFORMUTEXFAILED);
     bReturn = FALSE;
   }

   LeaveCriticalSection(&m_Critical);   // Let the next guy through if waiting

   LogMessage(_T("Exiting CNavAPService::ReLoadAutoProtect"));

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// ReLoadOptions
//
// This routine loads in the Options file again
//
// Parameters:
//    none
//
// Return Values:
//    none
//
/////////////////////////////////////////////////////////////////////
// 05/16/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::ReLoadOptions (VOID)
{
   LogMessage(_T("CNavAPService::ReLoadOptions()"));
                                        // Very simple operation here.
	ReadNavBin (&m_NavOptions);
	ReadNavStart (&m_NavOptions);
}


/////////////////////////////////////////////////////////////////////
// SetApStatus
//
// This function tries to enable/disable the device
//
// Parameters:
//    BOOL   - TRUE: Enable device, FALSE: disable the device
//
// Return Value:
//    DWORD  - Status of the communication
//
/////////////////////////////////////////////////////////////////////
// 05/16/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNavAPService::SetApStatus(
   BOOL     bEnable)                    // [in] - Command to process.
{
DWORD   dwSend;
DWORD   dwReturn;
BOOL    bReturn;

   LogMessage(_T("CNavAPService::SetApStatus(%d)"), (DWORD) bEnable);

   if (m_bDrvCommOpen)
   {
      dwSend = (bEnable) ? TRUE : FALSE;   // Set up the DWORD to send

      bReturn = m_pApDrvComm->Send (
                  NAVAP_COMM_SETENABLEDDISABLED, // Control Code
                  &dwSend,                       // Send enable/disable
                  sizeof (dwSend) );             // 4 bytes

      if (bReturn)
      {
                                        // If successfully set, return what
                                        // they were trying to do.
         dwReturn = (bEnable) ? NAVAPSVC_ENABLED : NAVAPSVC_DISABLED;
      }
      else                              // Didn't understand what we got back
         dwReturn = NAVAPSVC_ERROR;
   }
   else                                 // No connection yet!
      dwReturn = NAVAPSVC_ERROR;

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// GetApStatus
//
// This function gets the current status of the AP Device
//
// Parameters:
//    none.
//
// Return Value:
//    DWORD  - Status of the device
//
/////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNavAPService::GetApStatus(VOID)
{
DWORD   dwStatus;
DWORD   dwBytesReturned;
DWORD   dwReturn;

   if (m_bDrvCommOpen)
   {

      if (m_pApDrvComm->Send (
               NAVAP_COMM_GETENABLEDDISABLED, // Control code
               NULL,                          // Send nothing
               0,                             // 0 bytes of nothing
               &dwStatus,                     // Receiving a dword
               sizeof (dwStatus),             // 4 bytes
               &dwBytesReturned) )            // how many bytes were returned
      {
         if ( sizeof (dwStatus) == dwBytesReturned )
         {
            if (dwStatus)
               dwReturn = NAVAPSVC_ENABLED;
            else
               dwReturn = NAVAPSVC_DISABLED;
         }
         else
            dwReturn = NAVAPSVC_ERROR;
      }
      else
         dwReturn = NAVAPSVC_ERROR;
   }
   else                                 // No connection yet!
      dwReturn = NAVAPSVC_ERROR;

   LogMessage(_T("CNavAPService::GetApStatus - %d"), dwReturn);

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// WaitForStartup
//
// This function gets the current status of the AP Device
//
// Parameters:
//    none.
//
// Return Value:
//    DWORD  - Status of the device
//
/////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNavAPService::WaitForStartup(VOID)
{
DWORD   dwReturn = NAVAPSVC_SUCCESS;

   LogMessage(_T("Entering CNavAPService::WaitForStartup()"));


   if (NULL == m_hStartupEvent)
   {
      dwReturn = NAVAPSVC_ERROR;
      goto Cleanup;
   }

   WaitForSingleObject (
      m_hStartupEvent,
      INFINITE);

                                        // If we are exiting when we get here,
                                        // some failure happened in our service
                                        // initialization, and we must
                                        // report this failure to our caller
   if (m_bExitingService)
      dwReturn = NAVAPSVC_ERROR;

Cleanup:

   LogMessage(_T("Exiting CNavAPService::WaitForStartup()"));

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// CmdExecProgram
//
// This gets called when an application wants us to run a function.
// at this point however, we need to read from our named pipe the
// command line to execute.  The command line MUST be in single byte
// character set.
//
// Parameters:
//    none.
//
// Return Value:
//    DWORD  - Success or Failure
//
/////////////////////////////////////////////////////////////////////
// 06/17/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNavAPService::CmdExecProgram(
   PEXEDATA pExeData)
{
DWORD      dwReturn = NAVAPSVC_SUCCESS;

#if  0
UINT       uReturn;

   LogMessage(_T("Entering CNavAPService::MsgExecProgram()"));


#ifdef DEBUG_PROFILE
   {
   TCHAR szTempW[512];

      if ( 0 != MultiByteToWideChar (
                   CP_ACP,
                   MB_PRECOMPOSED,
                   pExeData->szCmdLine,
                   -1,
                   szTempW,
                   sizeof(szTempW) / sizeof(szTempW[0])) )
      {
         LogMessage(
            _T("CNavAPService::MsgExecProgram - Command line to execute: [%s]"),
            szTempW);
      }
      else
      {
         LogMessage(
            _T("CNavAPService::MsgExecProgram - Error converting string for log."));
      }
   }
#endif // DEBUG_PROFILE

                                        // Now execute the request
   uReturn = WinExec(
                pExeData->szCmdLine,
                pExeData->nCmdShow);

   if (uReturn < 32)
   {

      LogMessage(_T("CNavAPService::MsgExecProgram - WinExecError: %d"),
         uReturn);

                                          // WinExec Errored out.
      dwReturn = NAVAPSVC_ERROR;
      goto Cleanup;
   }
   else
   {

      LogMessage(_T("CNavAPService::MsgExecProgram - WinExec Success - Return: %d"),
         uReturn);

   }
#else
   BOOL                 bRet = TRUE;
   TCHAR                szTempW[512];
   SECURITY_ATTRIBUTES  saThread, saProcess;
   PROCESS_INFORMATION  rNav;
   SECURITY_DESCRIPTOR  sdSec;
   STARTUPINFO          si;
   TCHAR                szDesktop[256];

    // Get Unicode string for command line.
    if ( 0 != MultiByteToWideChar (
                   CP_ACP,
                   MB_PRECOMPOSED,
                   pExeData->szCmdLine,
                   -1,
                   szTempW,
                   sizeof(szTempW) / sizeof(szTempW[0])) )
    {
         LogMessage(
            _T("CNavAPService::MsgExecProgram - Command line to execute: [%s]"),
            szTempW);
    }
    else
    {
         LogMessage(
            _T("CNavAPService::MsgExecProgram - Error converting string for log."));
    }


    if (InitializeSecurityDescriptor (&sdSec, SECURITY_DESCRIPTOR_REVISION))
    {
        if (SetSecurityDescriptorDacl (&sdSec, TRUE, (PACL) NULL, FALSE))
        {

            dwReturn = LoadString (m_hInst,
                                    IDS_NAVAP_DESKTOP_NAME,
                                    szDesktop,
                                    sizeof (szDesktop));
            // Only if we get the default desktop name do the following
            if (dwReturn)
            {
                MEMSET (&si, 0, sizeof (si));
                MEMSET (&saProcess, 0, sizeof (saProcess));
                MEMSET (&saThread, 0, sizeof (saThread));

                // Set the startup Info.
                si.cb = sizeof (si);
                si.lpDesktop = szDesktop;
                si.dwFlags = 0;
                si.wShowWindow = pExeData->nCmdShow;

                saProcess.nLength = sizeof (saProcess);
                saProcess.lpSecurityDescriptor = &sdSec;
                saProcess.bInheritHandle = FALSE;

                saThread.nLength = sizeof (saThread);
                saThread.lpSecurityDescriptor = &sdSec;
                saThread.bInheritHandle = FALSE;


                bRet = CreateProcess (NULL,
                                    szTempW,
                                    &saProcess,
                                    &saThread,
                                    FALSE,
                                    HIGH_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
                                    NULL,
                                    NULL,
                                    &si,
                                    &rNav);
                if (!bRet)
                    dwReturn = GetLastError ();
            }
            else
                dwReturn = GetLastError ();
        }
        else
            dwReturn = GetLastError ();
    }
    else
        dwReturn = GetLastError ();

#endif

#if 0
Cleanup:

   LogMessage(_T("Exiting CNavAPService::MsgExecProgram()"));
#endif

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// ProcessRequest
//
// This function takes a command that was sent in through the pipe
// and performs the function being requested.  The return value
// for this function is meant to be sent back to the caller.
//
// Parameters:
//    DWORD  dwCommand  - What are we being asked to do?
//
// Return Value:
//    DWORD  - To be returned to the application requesting our action
//
/////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNavAPService::ProcessRequest(
   PCMDPACKET pPacket)                  // [in] - Command to process.
{
DWORD   dwReturn;
BOOL    bTryRepair = FALSE;

   LogMessage(_T("Entering CNavAPService::ProcessRequest()"));

   switch (pPacket->dwCommand)
   {
      case NAVAPSVC_GET_STATUS:
         LogMessage(_T("ProcessRequest - NAVAPSVC_GET_STATUS"));
         dwReturn = GetApStatus ();
         break;

      case NAVAPSVC_AP_ENABLE:
         LogMessage(_T("ProcessRequest - NAVAPSVC_AP_ENABLE"));
         dwReturn = SetApStatus (TRUE);
         break;

      case NAVAPSVC_AP_DISABLE:
         LogMessage(_T("ProcessRequest - NAVAPSVC_AP_DISABLE"));

         dwReturn = SetApStatus (FALSE);
         break;


      case NAVAPSVC_AP_SCANFLOPPYA:
         LogMessage(_T("ProcessRequest - NAVAPSVC_AP_SCANFLOPPYA"));
         if ( !m_NavOptions.tsr.bChkFlopOnBoot )
         {
            LogMessage(_T("Shutdown scan not selected"));
         }
         else
         {
            switch ( m_NavOptions.tsr.uActionKnown == TSR_CUSTOM_RESPONSE ?
                        m_NavOptions.customAP.uchBoot :
                        m_NavOptions.tsr.uActionKnown )
            {
               case TSR_REPAIR:
               case TSR_DELETE:
               case TSR_QUAR_CANT_REP:

                  bTryRepair = TRUE;
            }
                                           // ScanBoot ONLY returns true if we
                                           // know the boot sector is infected.
            if ( ScanBoot ((UINT)m_cFirstFloppyDrive, NULL, bTryRepair) )
               dwReturn = NAVAPSVC_FLOPPYINFECTED;
            else
               dwReturn = NAVAPSVC_FLOPPYCLEAN;
         }
         break;

      case NAVAPSVC_EXEC_PROGRAM:
         LogMessage(_T("ProcessRequest - NAVAPSVC_EXEC_PROGRAM"));
         dwReturn = CmdExecProgram( &(pPacket->rData.rExeData));
         break;

      case NAVAPSVC_RELOADCONFIG:
         LogMessage(_T("ProcessRequest - NAVAPSVC_RELOADCONFIG"));
         if ( ERROR_SUCCESS != ReLoadAutoProtect() )
         {
            ErrorHandler(IDS_ERR_RELOADCONFIG);
            dwReturn = NAVAPSVC_ERROR;
         }
         else
            dwReturn = NAVAPSVC_SUCCESS;
         break;

      case NAVAPSVC_RELOADOPTIONS:
         LogMessage(_T("ProcessRequest - NAVAPSVC_RELOADOPTIONS"));
         ReLoadOptions();
         dwReturn = NAVAPSVC_SUCCESS;
         break;

      case NAVAPSVC_WAIT_FOR_STARTUP:
         LogMessage(_T("ProcessRequest - NAVAPSVC_WAIT_FOR_STARTUP"));
         dwReturn = WaitForStartup();
         break;

      case NAVAPSVC_PAUSE_WATCHING_DEFS:
         LogMessage(_T("ProcessRequest - NAVAPSVC_PAUSE_WATCHING_DEFS"));
         m_bPauseWatchingDefs = TRUE;

                                        // Now that we have turned our monitor
                                        // off, we should go and turn off the
                                        // alert service also (If it is there)
         NavRpcSendCommand(
            ALERTSVC_PROTOCOL,
            ALERTSVC_ADDRESS,
            NULL,
            NAVAPSVC_PAUSE_WATCHING_DEFS);

         dwReturn = NAVAPSVC_SUCCESS;
         break;

      case NAVAPSVC_CONTINUE_WATCHING_DEFS:
         LogMessage(_T("ProcessRequest - NAVAPSVC_CONTINUE_WATCHING_DEFS"));
         m_bPauseWatchingDefs = FALSE;

                                        // We are also responsible for turning
                                        // the definition monitor back on when
                                        // we are turned back on.
         NavRpcSendCommand(
            ALERTSVC_PROTOCOL,
            ALERTSVC_ADDRESS,
            NULL,
            NAVAPSVC_CONTINUE_WATCHING_DEFS);

         dwReturn = NAVAPSVC_SUCCESS;
         break;

      case NAVAPSVC_ALERTSVC_READY:
         LogMessage(_T("ProcessRequest - NAVAPSVC_ALERTSVC_READY"));
         m_bAlertSvcReady = TRUE;
         dwReturn = NAVAPSVC_SUCCESS;
         break;

      case NAVAPSVC_DISPLAY_NAVW_OPTIONS:
         LogMessage(_T("ProcessRequest - NAVAPSVC_DISPLAY_NAVW_OPTIONS"));

         NavRpcSendCommand(
              NAVW_PROTOCOL,
              NAVW_ADDRESS,
              NAVW_ENDPOINT,
              UWM_DISPLAY_OPTIONS);

         dwReturn = NAVAPSVC_SUCCESS;
         break;

      default:
         dwReturn = NAVAPSVC_ERROR;
   }

   LogMessage(_T("Exiting CNavAPService::ProcessRequest()"));

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// SendCommand
//
// This function is our callback recepticle that takes commands
// from the other applications
//
// Parameters:
//    LPVOID - pPacket - Packet to receive from the client
//
// Return Value:
//    DWORD - return value from processing function
//
/////////////////////////////////////////////////////////////////////
// 07/17/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNavAPService::SendCommand(
   LPVOID pPacket)                      // [in] - Packet from the client
{
DWORD      dwReturn;

   dwReturn = ProcessRequest(
                 (PCMDPACKET) pPacket);

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// SendCommandProc
//
// This is just the passthrough function to get to the member function.
// This is called from the RPC CLinkClient guy.
//
// Parameters:
//    LPVOID - lpPacket - Packet from client guy
//
// Return Value:
//    Member function doesn't return.
//
/////////////////////////////////////////////////////////////////////
// 05/07/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD SendCommandProc(
   LPVOID pPacket)                      // [in] - Packet from Client
{
DWORD dwReturn;

   dwReturn = APService.SendCommand(pPacket);

   return dwReturn;
}


//***************************************************************************/
//      ServiceControlPause ()
//
//	Description:
//		To suspend the Service Thread (based on User request).
//
//      Parameters:
//		-None-
//
//      Returns:
//              VOID
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
VOID
CNavAPService::ServiceControlPause ()
{
}


//***************************************************************************/
//      ServiceControlContinue ()
//
//	Description:
//		To continue the Service Thread (based on User request).
//
//      Parameters:
//		-None-
//
//      Returns:
//              VOID
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
VOID
CNavAPService::ServiceControlContinue ()
{
}


/////////////////////////////////////////////////////////////////////
//      ServiceControlUser ()
//
//	Description:
//		This is where we can receive special messages from the
//              SCMgr or other applications
//
//      Parameters:
//          DWORD   dwCtrlCode - [in] Control code sent from outside our service
//
//      Returns:
//          none
//***************************************************************************/
//	04/26/96 DSACKING - Function Created
// 07/18/96 DSACKING - Function not used anymore due to RPC functionality
//***************************************************************************/
VOID
CNavAPService::ServiceControlUser (
   DWORD   dwCtrlCode)
{
   dwCtrlCode;
}

