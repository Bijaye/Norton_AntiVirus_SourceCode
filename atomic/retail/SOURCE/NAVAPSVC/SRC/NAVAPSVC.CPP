//      Copyright 1996 Symantec Corporation
//***************************************************************************/
//      $Header:   S:/NAVAPSVC/VCS/navapsvc.cpv   1.16   17 Aug 1998 11:27:50   mdunn  $
//
//      Description:
//              This source file contains routines for NAV Alert Service (for NT)
//
//
//***************************************************************************/
//      $Log:   S:/NAVAPSVC/VCS/navapsvc.cpv  $
// 
//    Rev 1.16   17 Aug 1998 11:27:50   mdunn
// Little tweak - moved ErrorHandler() call after the MessageBox() call.
// This preserves the old sequence of events.
// 
//    Rev 1.15   17 Aug 1998 10:58:00   mdunn
// The trial-expired message now tells you how long the trial period lasted.
//
//    Rev 1.14   24 Jun 1998 22:54:56   RStanev
// More quarantine fixes.
//
//    Rev 1.13   27 May 1998 16:52:32   mdunn
// Fixed IsTrialValid() to use the right product name & version numbers &
// stuff.
//
//    Rev 1.12   21 May 1998 12:01:30   tcashin
// Fixed the COM-related includes & defines.
//
//    Rev 1.11   13 May 1998 16:03:06   DALLEE
// Initialization of new m_bIncomingQuarantineItems member var.
//
//    Rev 1.10   17 Feb 1998 12:42:26   mdunn
// Ported NEC code from the ROSNEC codebase.
//
//    Rev 1.9   20 Nov 1997 10:07:40   mdunn
// Added initialization of the critical section used by the AP prompting code to
// the CNavAPService constructor.
//
//    Rev 1.8   13 Aug 1997 09:28:18   JBRENNA
// Add a LogMessage to StartService that appears when the StartService fails.
// the LogMessage logs the failure code.
//
//    Rev 1.7   12 Aug 1997 13:33:38   JBRENNA
// Call UpdateDefDateInRegistry() when starting the service. This ensures that
// the definitions date in the registry is up-to-date.
//
//    Rev 1.6   11 Jul 1997 12:23:24   RStanev
// Added custom response support.
//
//    Rev 1.5   13 Jun 1997 10:23:38   JBRENNA
// Rename UpdateDefSvcImagePaths to UpdateDefServicePaths.
//
//    Rev 1.4   06 Jun 1997 11:20:14   JBRENNA
// Add initialization for m_DefUtilsAppId1 and m_DefUtilsAppId2.
// 2. Change TEXT() macros to _T().
// 3. In StartServices(), before starting NAVEX or NAVENG, update their
//    ImagePath settings in the registry. This is done by calling
//    UpdateDefSvcImagePaths().
// 4. In StartAPService(), move the p_pApDrvComm->Send (NAVAP_COMM_UNPROTECTPROC
//    down. This way the p_mApDrvComm->Open has occured.
//
//    Rev 1.3   03 Jun 1997 14:48:42   DSACKIN
// Changed the StartService function to return a DWORD value stating what
// happened in the function.
//
//    Rev 1.2   02 Jun 1997 19:22:28   RStanev
// Added support for new NAVAP driver.
//
//    Rev 1.1   18 Apr 1997 13:59:04   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   06 Apr 1997 15:32:28   RStanev
// Updated to match the new DriverComm APIs and changes in NAVAP.
//
//    Rev 1.0   13 Mar 1997 15:12:14   RFULLER
// Initial revision
//
//    Rev 1.9   27 Feb 1997 17:28:30   TSmith
// Typecast string arguments to SymTrial calls to LPCWSTR. This was done because
// the SymTrial DLL is non Unicode, uses TCHARS and does not provide both Ansi
// and Unicode functions.
//
//    Rev 1.8   06 Feb 1997 03:23:44   DSACKIN
// We now will NEVER give up the mutex once we are stopping the service.  Before,
// we were giving back the mutex, and it was possible that another thread
// was re-loading the configuration while we were shutting down.  This could
// cause problems because the DrvComm object was being used.
//
//    Rev 1.7   03 Feb 1997 19:40:38   JBELDEN
// replaced IsPreviewOK with IsTrialValid, which uses symtrial
// COM object, for NAVWNT_PREVIEW
//
//    Rev 1.6   22 Jan 1997 15:50:42   TSmith
// Modified calls to ScanBoot to accomodate that function's new argument (pvInBuffer).
//
//    Rev 1.5   13 Jan 1997 22:24:16   DSACKIN
// Added code to make the NAVAP Service wait until the Alert Service has
// completed its startup before sending across the first alert.
//
//    Rev 1.4   24 Dec 1996 18:53:54   DSACKIN
// Added include "afxtmpl.h"
//
//    Rev 1.3   24 Dec 1996 16:45:02   DSACKIN
// Added critical section stuff for the RPC link.
//
//    Rev 1.2   09 Dec 1996 09:48:46   TIVANOV
// new define for linear preview date
//
//    Rev 1.1   08 Nov 1996 17:41:50   TSMITH
// Removed alerting, messaging and logging functionality from all NAVAPSVC
// modules. Removed string resources regarding virus found and virus action
// events. Added link to ALRTNTFY for NOTIFICATION_PACKET handling.
//
//    Rev 1.51   18 Oct 1996 18:23:18   TSmith
// Removed obsolete #ifdefs.
//
//    Rev 1.50   16 Sep 1996 09:26:18   TIVANOV
// fix the preview condition check
//
//    Rev 1.49   09 Sep 1996 15:17:34   SEDWARD
// Ported Tchavdar's fix on checking the expiration date for the preview version.
//
//    Rev 1.48   08 Sep 1996 15:40:04   DSACKIN
// Added preview stuff.
//
//    Rev 1.47   30 Aug 1996 03:46:48   DSACKIN
// Unprotect us from NAVAP.
//
//    Rev 1.46   30 Aug 1996 03:13:54   DSACKIN
// Changed m_bUpdatingVirusList to m_bPauseWatchingDefs
//
//    Rev 1.45   28 Aug 1996 17:25:06   DSACKIN
// Added m_bUpdatingVirusList variable initialization.
//
//    Rev 1.44   28 Aug 1996 16:24:20   DSACKIN
// Added a parameter to SetProtocol on our RPC server to disable security.
//
//    Rev 1.43   22 Aug 1996 00:40:46   DSACKIN
// Added a line to start our Definition monitor.
//
//    Rev 1.42   20 Aug 1996 02:39:30   DSACKIN
// Added an initialization call to InitNLMMessenger for NLM Server Notifications.
//
//    Rev 1.41   05 Aug 1996 15:55:32   DSACKIN
// Removed all calls to ErrorHandlerID because ErrorHandler will work now.
// LogEvent has been re-worked to not need a NULL string at the end.
//
//    Rev 1.40   04 Aug 1996 22:54:00   DSACKIN
// Start the Rpc Server before going into our StartAPServiceProc.  This gives
// NAVAPW the ability to call into us and wait for something to happen.
//
//    Rev 1.39   01 Aug 1996 14:17:08   DSACKIN
// Don't shut down other services in StartServices if we fail.  This will be
// taken care of in ServiceControlStop handling.
//
//    Rev 1.38   25 Jul 1996 21:26:56   DSACKIN
// Added a call to AllowStop() to make sure that NAVAPSVC can NOT be stopped
// until WE say it can be stopped.
//
//    Rev 1.37   23 Jul 1996 13:52:46   DSACKIN
// Reworked ToBroadcast to use our RPC function from NavXUtil instead of
// using the Named pipe.
//
//    Rev 1.36   22 Jul 1996 16:33:20   DSACKIN
// Call to NavRpcSendCommand instead of TellNAVW....
//
//    Rev 1.35   19 Jul 1996 14:21:54   DSACKIN
// Call Navxutil to tell NAVW that we are started.
//
//    Rev 1.34   19 Jul 1996 01:00:08   DSACKIN
// Convert to use our CLinkServer RPC server instead of relying on the
// ServiceControlManager for messages from our other applications.
//
//    Rev 1.33   16 Jul 1996 01:35:38   DSACKIN
// Tell NAVW we are starting so they can exclude themselves from the AP.
//
//    Rev 1.32   08 Jul 1996 19:45:08   DSACKIN
// Re-worked error-handling so that certain errors are now fatal.
//
//    Rev 1.31   28 Jun 1996 18:06:36   DSACKIN
// Removed a string that was incorrect, and replaced it with the right one.
//
//    Rev 1.30   28 Jun 1996 16:20:28   DSACKIN
// Fixed ToBroadcast to ONLY take the string we are going to send.
//
//    Rev 1.28   24 Jun 1996 23:08:12   DSACKIN
// Added a mutex around the NavStat init/de-init
//
//    Rev 1.27   20 Jun 1996 02:33:00   DSACKIN
// Fixed one of the DEBUG_PROFILE messages.
//
//    Rev 1.26   14 Jun 1996 15:00:34   DSACKIN
// Added DEBUG_PROFILE for StartService and StopService.
//
//    Rev 1.25   13 Jun 1996 17:23:12   DSACKIN
// Added DEBUG_PROFILE strings.
//
//    Rev 1.24   06 Jun 1996 14:33:10   PVENKAT
// Checking for availability of NAV Alert Service before calling.
//
//    Rev 1.23   05 Jun 1996 22:34:44   DSACKIN
// Changed order of startup to create a thread to start our devices/services.
//
//    Rev 1.22   29 May 1996 13:19:40   DSACKIN
// Don't unload Symevent anymore.
//
//    Rev 1.21   28 May 1996 14:06:44   DSACKIN
// Make sure to initialize SymStat before starting the ScanThread.
//
//    Rev 1.20   23 May 1996 13:06:40   DSACKIN
// Changed the shutdown process to take care of all shutdown issues in another
// thread.  This should make it so that the service immediately is responsive
// to the Service Control Manager.
//
//    Rev 1.19   17 May 1996 18:45:46   DSACKIN
// Added scanning of the 'A' drive when service is unloaded.
//
//    Rev 1.18   16 May 1996 20:34:00   DSACKIN
// Added code to load the options when the service is being initialized.
//
//    Rev 1.17   16 May 1996 15:30:32   DSACKIN
// Switched the order of drvcomm.h and apcomm.h
//
//    Rev 1.16   16 May 1996 11:08:36   DSACKIN
// Split this file into many smaller files.
//
//    Rev 1.15   08 May 1996 20:22:04   DSACKIN
// Fixed the Start function to support Service Locking.
//
//    Rev 1.14   08 May 1996 20:05:58   DSACKIN
// Added a lock for NAVAP Device.
//
//    Rev 1.13   08 May 1996 20:01:16   DSACKIN
// Now we create a pipe for NAVAPW to request device status.
//
//    Rev 1.12   30 Apr 1996 16:31:48   PVENKAT
// Commented NotifyServers as NAVSERVE is not yet available on W32.
//
//    Rev 1.11   30 Apr 1996 15:27:18   PVENKAT
// Added m_NavOptions and displaying Alert Msg (User specified) if available.
// Calling NotifyServers to notify NAV NLM if present.
//
//    Rev 1.10   29 Apr 1996 20:24:08   DSACKIN
// Added a MUTEX so that we can't unload NAVAPSVC while re-loading config.
//
//    Rev 1.9   26 Apr 1996 17:54:12   DSACKIN
// Cleaned up the Code.  Got rid of the WorkerThread that did nothing.
//
//    Rev 1.8   26 Apr 1996 13:33:32   DSACKIN
// Added a User control code so that NAVW can tell us to reload the AP config.
//
//    Rev 1.7   25 Apr 1996 14:53:36   PVENKAT
// Added functions to do NAV Log (Activity Log).
//
//    Rev 1.6   24 Apr 1996 20:58:08   DSACKIN
// Fixed some of the LoadString lines.
//
//    Rev 1.5   24 Apr 1996 20:51:22   DSACKIN
// Now NAVAPSVC will load and unload Symevent, NAVEX, NAVAP, and NAV Alert Service
// and unload them in the exact opposite direction.
//
//    Rev 1.4   22 Apr 1996 15:10:00   PVENKAT
// Using CService::LogEvent to Log events to NT Log. (with MC files.)
//
//    Rev 1.3   18 Apr 1996 12:58:18   PVENKAT
// CService Version.  ErrID = CatID + ResID with Message File for Logging.
//
//    Rev 1.2   08 Apr 1996 14:19:06   PVENKAT
// Modified Handler/ServiceMain to WINAPI to STOP on NT 4.0 gracefully.
//
//    Rev 1.1   01 Apr 1996 16:22:28   PVENKAT
//
//    Rev 1.0   29 Mar 1996 14:57:28   PVENKAT
// Initial revision.
//
//***************************************************************************/

//***************************************************************************/
//      Includes
//***************************************************************************/

// Ensure that we define and initialize the strings defined in NavAppIDs.h
#define NAVAPPIDS_BUILDING

// Define the Windows version we're targeting, in order to get the right
// MB_SERVICE_NOTIFICATION #define in winuser.h
#define _WIN32_WINNT  0x0400

// Quake & System Include Files
#include <afxtempl.h>
#include    "platform.h"
#include    "process.h"

#ifndef INITGUID
    #define INITGUID
#endif

#ifdef NAVWNT_PREVIEW
    #include "ole2.h"
    #include "tryint.h"
#endif

#include    "nonav.h"
#include    "inoc.h"
#include    "navalog.h"
#include    "navaplog.h"
#include    "navbin.h"
#include    "actions.h"
#include    "symalog.h"
#include    "navutil.h"
#include    "xapi.h"
#include    "symstat.h"
#include    "navstat.h"
#include    "disk.h"                // For DiskGetFloppyType()

#include    "navver.h"              // Version #s for trial stuff
#include    "servenfy.h"
#include    "navrpc.h"              // For protocol/address/endpoint definition
#include    "NavAppIds.h"           // For CDefUtils application identifiers

// NAV & AP Driver Include Files
#include    "drvcomm.h"
#include    "apcomm.h"
#include    "apscomm.h"             // For apps to communicate with me.
#include    "msgreg.h"

// RPC related include
#include    "linkcbk.h"
#include    "linksrv.h"
#include    "linkcli.h"

// Project Include files.
#include    "service.h"
#include    "navapsvc.h"
#include    "apsvcstr.h"

//***************************************************************************/
//      Constants
//***************************************************************************/
#define         MAX_THREAD_STACK_SIZE           (4096)  // Max Stack size
#define         MAX_SERVICE_NAME                (256)


//***************************************************************************/
// Globals
//***************************************************************************/
DECLARE_SERVICE(CNavAPService, APService, ID_ERR_REGISTERSERVICECTRLR_FAILED);

/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.
//

DWORD SendCommandProc(
   LPVOID pPacket);                     // [in] - Packet from Client

VOID NotificationQueueProc(
   LPVOID lpObject);                    // [in] - This pointer from caller

//***************************************************************************/
// Functions
//***************************************************************************/

//***************************************************************************/
//      CNavAPService
//
//      Constructor: Simply calls the base constructor and no extra
//	operations
//
//      Parameters:
//		pMain		LPSERVICE_MAIN_FUNCTION	ptr to ServiceMain
//		pHandler	LPHANDLER_FUNCTION	ptr to Handler
//
//	NOTE:
//		The parameters are generated by DECLARE_SERVICE macro.
//
//      Returns:
//              -None-
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
CNavAPService::CNavAPService (LPSERVICE_MAIN_FUNCTION pMain, LPHANDLER_FUNCTION pHandler)
	: CService (pMain, pHandler)
{
   m_pApDrvComm   = NULL;
   m_bDrvCommOpen = FALSE;
   m_hMutex       = NULL;

                                        // Set this to true to signal threads
                                        // to exit
   m_bExitingService = FALSE;
                                        // Set up our critical sections
   InitializeCriticalSection(&m_Critical);
   InitializeCriticalSection(&m_RpcCritical);
   InitializeCriticalSection(&m_PromptingCritical);

   m_bIncomingQuarantineItems = FALSE;  // No incoming quarantine items to process.

                                        // Set to true when updating virus list.
   m_bPauseWatchingDefs = FALSE;

                                        // Use this flag for when to allow the
                                        // first alerts to go through to the
                                        // Alert Service.
   m_bAlertSvcReady = FALSE;

   m_cFirstFloppyDrive   = 0;
   m_fdwFloppiesOnSystem = 0;

                                        // Initialize the CDefUtils applciation
                                        // identifier. This is what will be
                                        // used for the shared definition usage
                                        // counts.
   m_DefUtilsAppId1.InitWindowsApp (g_szNavAppId1ApNt);
   m_DefUtilsAppId2.InitWindowsApp (g_szNavAppId2ApNt);
}

//***************************************************************************/
//      ~CNavAPService
//
//      Destructor and no operation done here.
//
//      Parameters:
//		-None-
//
//      Returns:
//              -None-
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
CNavAPService::~CNavAPService ()
{
	// Nothing to cleanup.
}


/////////////////////////////////////////////////////////////////////
// CNavAPService::IsTrialValid(VOID)
//
// Purpose:
//    Check to see if the preview time has expired.
//    This code was taken from NAVW.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Trial period has not expired
//    FALSE - Trial period has expired
//
/////////////////////////////////////////////////////////////////////
// 02-03-97 - JBELDEN - Function Created.
/////////////////////////////////////////////////////////////////////
#ifdef NAVWNT_PREVIEW
BOOL CNavAPService::IsTrialValid( VOID )
    {
    LogMessage(_T("Entering CNavApService::IsTrialValid()"));

    CoInitialize(NULL);                 // Initalize COM.

	ITrialWare* pTrial;
    BOOL bValid = FALSE;

                                        // Attempt to create trial object.
    if ( SUCCEEDED( CoCreateInstance( CLSID_TrialWare, NULL,
        CLSCTX_INPROC_SERVER, IID_ITrialWare, ( LPVOID* )&pTrial ) ) )
		{
                                        // attempt to initialize trial object.
                                        //
                                        // NOTE:
                                        // The following strings are cast to
                                        // WIDE strings even though they aren't
                                        // to get around the fact that the
                                        // SYMTRIAL COM was not built with Unicode
                                        // and does not provide both Unicode and
										// ANSI interfaces.
        if ( SUCCEEDED( pTrial->Initialize( ( LPCWSTR )"Symantec",
            ( LPCWSTR )"Norton AntiVirus",
            NAV_VERSION_MAJOR, NAV_VERSION_MINOR ) ) )
			{
            if ( pTrial->IsValid( HWND_DESKTOP, TRUE, NULL ) == S_OK )
                bValid = TRUE;
			}

        pTrial->Release( );
		}

                                        // Balance call to CoInitalize.
    CoUninitialize();
    LogMessage( _T( "Exiting CNavApService::IsTrialValid( ) - Status %d" ), ( DWORD ) bValid );
    return( bValid );
    }
#endif // NAVWNT_PREVIEW


/////////////////////////////////////////////////////////////////////
// StartService
//
// This routine takes care of loading the specified service
// and reporting any problems found.
//
// Parameters:
//
// Returns:
//    ID_ERR_OPENSERVICE  - Couldn't open the service
//    ID_ERR_STARTSERVICE - Couldn't start the service.
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 - DSACKIN - Function Created
// 06-03-97 - DSACKIN - Return the ID_ERR if there is one instead of
//                    - calling ErrorHandler
/////////////////////////////////////////////////////////////////////

DWORD CNavAPService::StartService (
   LPTSTR           lpszServiceName,    // Name to assign to service
   CServiceControl *lpServiceControl,   // Object to talk to   BOOL             bLock)
   BOOL             bLock)
{
DWORD     dwReturn = ERROR_SUCCESS;

   LogMessage(_T("Starting Service/Device:%s"), lpszServiceName);

   if (ERROR_SUCCESS != lpServiceControl->Open(lpszServiceName))
   {
      dwReturn = ID_ERR_OPENSERVICE;
   }
   else
   {
      if (ERROR_SUCCESS != lpServiceControl->Start(bLock))
      {
         lpServiceControl->Close();     // On failure, clean up.

         dwReturn = ID_ERR_STARTSERVICE;

//         ErrorHandler(
//            ID_ERR_STARTSERVICE |
//            ID_ERR_FATAL,
//            lpszServiceName,
//            NULL);

      }
   }

   if (ERROR_SUCCESS != dwReturn)
      LogMessage(_T("Starting Service/Device failed: %d error"), dwReturn);

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// StopService
//
// This routine stops a given service and logs any errors found.
//
// Parameters:
//    none
/////////////////////////////////////////////////////////////////////
// 04/23/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

BOOL CNavAPService::StopService (
   CServiceControl *lpServiceControl)   // Object to talk to
{
BOOL      bReturn = TRUE;
TCHAR     szServiceName[MAX_SERVICE_NAME];

   lpServiceControl->GetName (szServiceName);
   LogMessage(_T("Stopping Service/Device:"), szServiceName);

   if (ERROR_SUCCESS == lpServiceControl->Stop())
   {
      lpServiceControl->Close();
   }
   else
   {
      ErrorHandler (
         ID_ERR_CLOSESERVICE,
         szServiceName,
         NULL);

      bReturn = FALSE;
   }

   return bReturn;
}




/////////////////////////////////////////////////////////////////////
// StartService
//
// This routine takes care of loading the devices/services
// That is set to us and reporting any errors found.
// If the services fail to start, all shutdown will take place
// when the ID_ERR_FATAL is processed.
//
// Parameters:
//    none
/////////////////////////////////////////////////////////////////////
// 04/23/96 - DSACKIN - Function Created
// 08/01/96 - DSACKIN - Don't shut down services when we fail.
//                    - ServiceControlStop will do this for us.
/////////////////////////////////////////////////////////////////////

BOOL CNavAPService::StartServices (VOID)
{
BOOL      bReturn = TRUE;
DWORD     dwReturn;
int       nRet;
TCHAR     szSymEvent[MAX_SERVICE_NAME];
TCHAR     szNAVENG[MAX_SERVICE_NAME];
TCHAR     szNAVEX[MAX_SERVICE_NAME];
TCHAR     szNAVAP[MAX_SERVICE_NAME];
TCHAR     szAlertService[MAX_SERVICE_NAME];

   LogMessage(_T("Entering CNavAPService::StartServices"));

                                        // Now load service Names
   nRet = LoadString (m_hInst,
             IDS_NAVAP_SYMEVENT,
             szSymEvent,
             sizeof (szSymEvent) / sizeof (szSymEvent[0]));

   if (nRet > 0)
   {
      nRet = LoadString (m_hInst,
                IDS_NAVAP_NAVENG,
                szNAVENG,
                sizeof (szNAVENG) / sizeof (szNAVENG[0]));
   }

   if (nRet > 0)
   {
      nRet = LoadString (m_hInst,
                IDS_NAVAP_NAVEX,
                szNAVEX,
                sizeof (szNAVEX) / sizeof (szNAVEX[0]));
   }

   if (nRet > 0)
   {
      nRet = LoadString (m_hInst,
                IDS_NAVAP_NAVAP,
                szNAVAP,
                sizeof (szNAVAP) / sizeof (szNAVAP[0]));
   }

   if (nRet > 0)
   {
      nRet = LoadString (m_hInst,
                IDS_NAVAP_ALERTSVC,
                szAlertService,
                sizeof (szAlertService) / sizeof (szAlertService[0]));
   }

   if (nRet == 0)
   {
      ErrorHandler(
         ID_ERR_LOADSTRING_FAILED |
         ID_ERR_FATAL);

      bReturn = FALSE;
   }
   else
   {
      LogMessage(_T("Attempting to start SymEvent"));

      dwReturn = StartService(szSymEvent, &m_SymEvent);

      if (ERROR_SUCCESS != dwReturn)
      {
         ErrorHandler(
            dwReturn |
            ID_ERR_FATAL,
            szSymEvent);

         LogMessage(_T("Failure to start SymEvent"));

         bReturn = FALSE;
      }
      else
      {
         LogMessage(_T("Updating ImagePaths to NAVENG, NAVEX, and NAVAP"));

                                        // Update the paths to NAVEX and
                                        // NAVENG. Their directories change
                                        // because of the shared definitions.
         UpdateDefServicePaths();

         LogMessage(_T("Attempting to start NAVENG"));

                                        // NO error checking on NAVENG because
                                        // it may not be there, and this is fine.
         StartService(szNAVENG, &m_NAVENG);

         LogMessage(_T("Attempting to start NAVEX"));
                                        // NO error checking on NAVEX because
                                        // it may not be there, and this is fine.
         StartService(szNAVEX, &m_NAVEX);

         LogMessage(_T("Attempting to start NAVAP"));

         dwReturn = StartService(szNAVAP, &m_NAVAP);

                                        // The additional TRUE locks the device
                                        // In memory until WE exit.
         if (ERROR_SUCCESS != dwReturn)
         {
            ErrorHandler(
               dwReturn |
               ID_ERR_FATAL,
               szNAVAP);

            LogMessage(_T("Failure to start NAVAP"));

            bReturn = FALSE;
         }
         else
         {

            LogMessage(_T("Attempting to start AlertService"));

            dwReturn = StartService(szAlertService, &m_AlertService);

            if (ERROR_SUCCESS != dwReturn)
            {
               ErrorHandler(
                  dwReturn |
                  ID_ERR_FATAL,
                  szAlertService);

               LogMessage(_T("Failure to start AlertService"));

               bReturn = FALSE;
            }
         }
      }
   }

   LogMessage(_T("Exiting CNavAPService::StartServices"));

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// StopServices
//
// This routine takes care of loading the devices/services
// That is set to us and reporting any errors found.
//
// Parameters:
//    none
//
// Note:
//
//   Do NOT Release the mutex from this point forward!!! If we
//   release the mutex, the option loading thread may have a chance
//   to go through and start re-loading the definitions.  Re-Loading
//   the definitions is done using the DrvComm object.  If we have
//   a DrvComm object open to the NAVAP device driver at the time
//   we are taken out of memory, the NAVAP device driver will not
//   only refuse to unload, but also will not be able to be
//   re-initialized.
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::StopServices (VOID)
{
BOOL      bReturn = TRUE;
DWORD     dwWaitResult = WAIT_TIMEOUT;  // Start with a failure
DWORD     dwWaitTries = 10;             // 10 tries to get the mutex

   LogMessage(_T("Entering CNavAPService::StopServices"));

                                        // Do a loop so that we can be updating
                                        // the Service Control manager with our
                                        // status.

   LogMessage(_T("(StopServices) Waiting for Mutex on Loading Options"));

   while ( (WAIT_OBJECT_0 != dwWaitResult) &&
           (dwWaitTries > 0) )
   {
      ReportStatusToSCMgr(
         SERVICE_STOP_PENDING,
         NO_ERROR,
         4000);

      dwWaitResult = WaitForSingleObject(
                        m_hMutex,       // Handle to our mutex
                        3000L);         // Wait 3 seconds

      dwWaitTries--;
   }

   if ( WAIT_OBJECT_0 == dwWaitResult )
   {

      LogMessage(_T("(StopServices) Received Mutex on Loading Options"));

                                        // This is going to take some time.
      ReportStatusToSCMgr (SERVICE_STOP_PENDING,
                           NO_ERROR,
                           4000);

      LogMessage(_T("Stopping Alert Service"));

      if (!StopService( &m_AlertService ))
         bReturn = FALSE;

                                        // Inform service manager that we
                                        // are still alive.
      ReportStatusToSCMgr (SERVICE_STOP_PENDING,
                           NO_ERROR,
                           4000);

      LogMessage(_T("Stopping NAVAP"));

      if (!StopService( &m_NAVAP ))
         bReturn = FALSE;

                                        // Inform service manager that we
                                        // are still alive.
      ReportStatusToSCMgr (SERVICE_STOP_PENDING,
                           NO_ERROR,
                           4000);

      LogMessage(_T("Stopping NAVEX"));

      if (!StopService( &m_NAVEX ))
         bReturn = FALSE;

      LogMessage(_T("Stopping NAVENG"));

      if (!StopService( &m_NAVENG ))
         bReturn = FALSE;

                                        // Inform service manager that we
                                        // are still alive.
      ReportStatusToSCMgr (SERVICE_STOP_PENDING,
                           NO_ERROR,
                           4000);

                                                 // For now, don't stop
                                                 // Symevent
/////////////////////////////////////////////////////////////////////
//               if (!StopService( &m_SymEvent ))
//                  bReturn = FALSE;

   }
   else
   {

      LogMessage(_T("CNavAPService::StopServices - Failed our wait for MUTEX."));

      ErrorHandler( ID_ERR_WAITFORMUTEXFAILED );
   }

   LogMessage(_T("Exiting CNavAPService::StopServices"));

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// StopAPService
//
// This is where we will take care of shutting down our service.
// all of this will happen in a separate thread from our shutdown
// call from the ServiceControlManager.
//
// Parameters:
//    none
/////////////////////////////////////////////////////////////////////
// 05/23/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::StopAPService (VOID)
{
BOOL      bReturn = TRUE;
BOOL      bTryRepair = FALSE;
DWORD     dwWaitResult, dwStatus, dwBytesReturned;
TCHAR     szDebugMsg[] = { m_cFirstFloppyDrive, ':', '\0' };

   LogMessage(_T("Entering CNavAPService::StopAPService"));

                                        // If we get here and the event hasn't
                                        // already been set, this should tell
                                        // them that we failed here.
   SetEvent (m_hStartupEvent);

   LogMessage(_T("Stopping RPC Server"));

   m_RequestServer.StopListen();
                                        // Scan the A drive on shutdown?

   if ( m_NavOptions.tsr.bChkFlopOnBoot )
   {
                                        // First, tell them a few seconds
      ReportStatusToSCMgr (SERVICE_STOP_PENDING,
                           NO_ERROR,
                           3000);

      switch ( m_NavOptions.tsr.uActionKnown == TSR_CUSTOM_RESPONSE ?
                  m_NavOptions.customAP.uchBoot :
                  m_NavOptions.tsr.uActionKnown )
      {
         case TSR_REPAIR:
         case TSR_DELETE:
         case TSR_QUAR_CANT_REP:

            bTryRepair = TRUE;
      }

      LogMessage( _T("Scanning drive %s on shutdown."), szDebugMsg );

                                        // Now scan the first floppy drive.
      ScanBoot ( (UINT) m_cFirstFloppyDrive, NULL, bTryRepair );
   }

                                        // Signal that we are closed!
   m_bDrvCommOpen = FALSE;

   if ( m_pApDrvComm )
   {
                                        // Signal to re-protect us.
      m_pApDrvComm->Send( NAVAP_COMM_PROTECTPROCESS );
      m_pApDrvComm->Send( NAVAP_COMM_CONTROLLEDDEINIT,
                          0,
                          NULL,
                          &dwStatus,
                          sizeof(dwStatus),
                          &dwBytesReturned );
      SYM_ASSERT ( dwBytesReturned == sizeof(dwStatus) );
      SYM_ASSERT ( dwStatus == ERROR_SUCCESS );
      delete m_pApDrvComm;
   }

                                        // This function call will stop all
                                        // of the services we have started.
   StopServices();

   LogMessage(_T("NavStatEnd()"));

                                        // As a Safety, we don't want to release
                                        // this mutex.  We can NEVER allow someone
                                        // to scan again after shutting down
                                        // NavStat !!!!!!!!!!!!!!!!!!!!!!!!!

   dwWaitResult = WaitForSingleObject(
                     m_hNavStatMutex,   // Handle to our mutex
                     3000L);            // Wait 3 seconds

   if (WAIT_OBJECT_0 == dwWaitResult)   // Did we succeed?
   {
                                        // Now that we are done locking drives,
                                        // Clean up the NavStat stuff.
      if ( NOERR != NavStatEnd() )
         ErrorHandler (ID_ERR_NAVSTATEND);

   }
   else
   {

      LogMessage(_T("NavStatEnd() - Couldn't get mutex!!! didn't stop NavStat"));

      SYM_ASSERT (FALSE);
   }

                                        // Now the the Service manager we
                                        // have stopped perhaps not successfully,
                                        // but at this point we will always be stopped.
   ReportStatusToSCMgr (SERVICE_STOPPED,
                        NO_ERROR,
                        0);

   LogMessage(_T("Exiting CNavAPService::StopAPService"));

   return bReturn;
}


//***************************************************************************/
//      GetAlertServer
//
//	Description:
//      This function should always return the name of the local system. The
//      Navap Service will always forward alert packets to the Alert Service
//      on the local system who will determine and perform all alerting and
//      logging functionality.
//
//  Parameters:
//		lpszServerName	LPTSTR	- Buffer to hold the server name
//
//  Returns:
//      VOID
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
VOID
CNavAPService::GetAlertServer (LPTSTR lpszServerName)
{
    TCHAR   	szDotString[MAX_COMPUTERNAME_LENGTH + 1];
    int		nRet;

    nRet = LoadString (m_hInst,
    		IDS_DOT_STRING,
		szDotString,
		sizeof (szDotString) / sizeof (szDotString[0]));

    if (nRet == 0)
    	SYM_ASSERT (0);

    STRCPY (lpszServerName, szDotString);
}


//***************************************************************************/
//      ServiceCallBack ()
//
//      This function is called directly NAVAP NT Driver to notify of virus
//	action
//
//      Parameters:
//
//      dwServiceCode    - non-0 service code from APCOMM.H
//      pvInBuffer       - buffer with data related to dwServiceCode
//      dwInBufferSize   - length of pvInBuffer
//      pvOutBuffer      - NULL
//      dwOutBufferSize  - 0
//      pdwBytesReturned - NULL
//
//      Returns:
//              VOID
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
VOID
ServiceCallback (
DWORD dwServiceCode,
PVOID pvInBuffer,
DWORD dwInBufferSize,
PVOID pvOutBuffer,
DWORD dwOutBufferSize,
PDWORD pdwBytesReturned )
{
    SYM_ASSERT ( dwServiceCode );
    SYM_ASSERT ( pvInBuffer );
    SYM_ASSERT ( dwInBufferSize );
    SYM_ASSERT ( !pvOutBuffer );
    SYM_ASSERT ( !dwOutBufferSize );
    SYM_ASSERT ( !pdwBytesReturned );

    APService.Callback (dwServiceCode, pvInBuffer, dwInBufferSize);
}


/////////////////////////////////////////////////////////////////////
// InformNAVW()
//
// This function posts a message to NAVW that we are starting and
// that they should tell the NAVAP device to exclude them from
// protection so they can scan files without US telling them
// they are infected and denying them access.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 07/15/96 - DSACKIN - function created.
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::InformNAVW (VOID)
{
                                        // Send the command using NavXUtil.
   NavRpcSendCommand(
      NAVW_PROTOCOL,
      NAVW_ADDRESS,
      NAVW_ENDPOINT,
      UWM_NAVAP_ACTIVATING);
}


//***************************************************************************/
//      StartAPService ()
//
//	Description:
//		To Initialize the Service related operations: register with
//		AP Driver, and start the thread (dummy).
//
//      Parameters:
//		-None-
//
//      Returns:
//              VOID
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
VOID CNavAPService::StartAPService (VOID)
{
TCHAR   szDriver[MAX_PATH + 1];
TCHAR   szVxDName[MAX_PATH + 1];
TCHAR   szEvent[MAX_PATH + 1];
int     nRet;
DWORD   dwStatus, dwBytesReturned;
   LogMessage(_T("Entering CNavAPService::StartAPService"));

   m_hMutex = CreateMutex(
                 NULL,                  // Default security
                 FALSE,                 // We don't want initial ownership
                 NULL);                 // No name on the mutex;

   if ( NULL == m_hMutex )
   {
      ErrorHandler (
         ID_ERR_CREATEMUTEX |
         ID_ERR_FATAL);

      goto Cleanup;
   }

                                        // This mutex will be used in junction
                                        // with the NavStat functions.
   m_hNavStatMutex = CreateMutex(
                        NULL,           // Default security
                        FALSE,          // We don't want initial ownership
                        NULL);          // No name on the mutex;

   if ( NULL == m_hNavStatMutex )
   {
      ErrorHandler (
         ID_ERR_CREATEMUTEX |
         ID_ERR_FATAL);

      goto Cleanup;
   }

   if (!StartServices())
      goto Cleanup;

                                        // OK.  Now look for a version of
                                        // NAV running and let them know we
                                        // are here.
   InformNAVW();

   LogMessage(_T("Loading NAV Options for the first time"));

                                        // Load the options!!!
   ReLoadOptions();

   m_pApDrvComm = new DriverComm;

   if (m_pApDrvComm == NULL)
   {
      ErrorHandler (
         ID_ERR_AP_UNINITIALIZED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

   nRet = LoadString (m_hInst,
               IDS_NAVAP_NAVAP,
               szVxDName,
               sizeof (szVxDName) / sizeof (szVxDName[0]));

   SYM_ASSERT (nRet > 0);

   nRet = LoadString (m_hInst,
                        IDS_NAVAP_EVENT_NAME,
                        szEvent,
                        sizeof (szEvent) / sizeof (szEvent[0]));

   SYM_ASSERT (nRet > 0);

   STRCPY (szDriver, szVxDName);

   LogMessage(_T("Establishing Connection to the Device Driver."));

   if (!m_pApDrvComm->Open ( szDriver ) )
   {
      LogMessage(_T("m_pApDrvComm->Open failed."));
      ErrorHandler (
         ID_ERR_APDRIVER_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

                                        // Tell NAVAP not to watch us
   m_pApDrvComm->Send( NAVAP_COMM_UNPROTECTPROCESS );

   if ( !m_pApDrvComm->RegisterServiceCallback ( ServiceCallback ) )
   {
      LogMessage(_T("m_pApDrvComm->RegisterServiceCallback failed."));
      ErrorHandler (
         ID_ERR_APDRIVER_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

   if ( !m_pApDrvComm->Send ( NAVAP_COMM_CONTROLLEDINIT,
                              NULL,
                              0,
                              &dwStatus,
                              sizeof(dwStatus),
                              &dwBytesReturned) ||
        dwBytesReturned != sizeof(dwStatus) ||
        dwStatus != ERROR_SUCCESS )
   {
      LogMessage(_T("m_pApDrvComm->Send failed."));
      ErrorHandler (
         ID_ERR_APDRIVER_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

                                        // Signal that we are OPEN!
   m_bDrvCommOpen = TRUE;

   LogMessage(_T("Calling to NavStatInit()"));

   if ( NOERR != NavStatInit() )
   {
      ErrorHandler (
         ID_ERR_NAVSTATINIT |
         ID_ERR_FATAL );

      goto Cleanup;
   }

                                        // Make sure the definition date in the
                                        // registry is up-to-date.
   UpdateDefDateInRegistry();

                                        // Start watching for definition and
                                        // option changes.
   if (!StartDefinitionMonitor())
   {

      ErrorHandler (
         ID_ERR_STARTDEFINITIONMONITOR);

      LogMessage(_T("Failed call to StartDefinitionMonitor()"));
   }

                                        // When we get to this point, we KNOW
                                        // the service has started OK.  It is
                                        // time to set the m_hStartupEvent.
   SetEvent (m_hStartupEvent);

                                        // Now that everything is initialized,
                                        // do the startup scan.
   StartupScan();

Cleanup:
                                        // We must ALWAYS re-enable stop.
   AllowStop(TRUE);

                                        // And if we aren't already exiting,
                                        // update the database that we can
                                        // now be stopped.
   if (!m_bExitingService)
   {
      ReportStatusToSCMgr (
         SERVICE_RUNNING,
         NO_ERROR,
         0);
   }

   LogMessage(_T("Exiting CNavAPService::StartAPService"));

}

/////////////////////////////////////////////////////////////////////
// StartAPServiceProc
//
// This is a thread start callthrough that will call our StartServices
// member function.
//
/////////////////////////////////////////////////////////////////////
// 05/31/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID StartAPServiceProc (VOID *notUsed)
{
   APService.StartAPService();

   _endthread();                        // Don't just return!!!
}



/////////////////////////////////////////////////////////////////////
// StopAPServiceProc
//
// This is a thread start callthrough that will call our StopServices
// member function.
//
/////////////////////////////////////////////////////////////////////
// 04/24/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID StopAPServiceProc (VOID *notUsed)
{
   APService.StopAPService();

   _endthread();                        // Don't just return!!!
}



////////////////////////////////////////////////////////////////////
// CNavAPService::InitService()
//
//	Description:
//    When starting our service, we want to make sure that our
//    devices and Alert Service are started correctly.  Unfortunately
//    when the system starts, it locks the SCM database so we can't
//    start our services/devices.  This function starts a thread that
//    will block on Opening a handle to the database, but will continue
//    as the system comes up.  In this function, we immediately report
//    the service as started.
//
// Parameters:
//    none
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 05/31/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::InitService (VOID)
{
HANDLE  hThread;

LINKCALLBACKS rCbk = { SendCommandProc };

   LogMessage(_T("Entering CNavAPService::InitService()"));

                                        // Don't allow stop until we are all
                                        // the way 100% started!
   AllowStop (FALSE);
                                        // Never allow pause/continue
   AllowPauseContinue(FALSE);

   ReportStatusToSCMgr (SERVICE_START_PENDING,
                        NO_ERROR,
                        3000);

                                        // Start by creating our event
                                        // that will be used to signal when
                                        // we are done with our startup
   m_hStartupEvent = CreateEvent (NULL, TRUE, FALSE, NULL);

                                        // Make sure we got our event
   if (NULL == m_hStartupEvent)
   {
      ErrorHandler (
         ID_ERR_CREATEEVENT_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

   ReportStatusToSCMgr (SERVICE_START_PENDING,
                        NO_ERROR,
                        3000);

                                        // Start the RPC server for NAVAPW
                                        // in case he is starting us.
   LogMessage(_T("Starting RPC server."));

                                        // Initialize our server.
   m_RequestServer.SetProtocol(
      NAVAPSVC_PROTOCOL,
      NAVAPSVC_ENDPOINT,
      FALSE);                           // Don't use security! Everyone needs access.

                                        // Set up the callback
   m_RequestServer.SetCallbacks(
      &rCbk);
                                        // Now Create our listen pipe
                                        // and thread to listen on it.
   if(!m_RequestServer.StartListen())
   {
      LogMessage(_T("*** Failed to Start our RPC Server ***"));
   }

   LogMessage(_T("Creating StartAPService Thread"));
                                        // Start a thread that will take
                                        // care of getting everything running.
   hThread = (HANDLE)_beginthread (
                  StartAPServiceProc,   // start Addr
                  MAX_THREAD_STACK_SIZE,// stack size
                  NULL);                // We don't use the user parameter.

   if ((HANDLE)INVALID_HANDLE_VALUE == hThread)
   {
      ErrorHandler (
         ID_ERR_BEGINTHREAD_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

   LogMessage(_T("Creating NotificationQueue Thread"));
                                        // Start a thread that will take
                                        // care of getting everything running.
   hThread = (HANDLE)_beginthread (
                  NotificationQueueProc,// start Addr
                  MAX_THREAD_STACK_SIZE,// stack size
                  (LPVOID) this);       // We don't use the user parameter.

   if ((HANDLE)INVALID_HANDLE_VALUE == hThread)
   {
      ErrorHandler (
         ID_ERR_BEGINTHREAD_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

                                         //check to see if preview has expired
#ifdef NAVWNT_PREVIEW

    if( !IsTrialValid( ) )
        {
        TCHAR   szMsg [1024], szFormat [1024], szServiceName [64];
        HMODULE hModule;
        HKEY    hkey;
        LONG    lRet;
        DWORD   dwTrialLength = 30, dwBufSize = sizeof(DWORD);


                                        // Read in the trial period length
                                        // for use in the trial-expired
                                        // message.
        lRet = RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                              _T("Software\\Symantec\\Norton AntiVirus"),
                              0, KEY_QUERY_VALUE, &hkey );

        if ( ERROR_SUCCESS == lRet )
            {
            lRet = RegQueryValueEx ( hkey, _T("TrialLength"), NULL,
                                     NULL, (LPBYTE) &dwTrialLength,
                                     &dwBufSize );

            RegCloseKey ( hkey );
            }

        hModule = GetModuleHandle ( NULL );

        LoadString ( (HINSTANCE) hModule, IDS_LICENSEEXPIRED, szFormat, 1024 );
        wsprintf ( szMsg, szFormat, dwTrialLength );

        LoadString ( (HINSTANCE) hModule, IDS_NAVAP_SERVICE_NAME,
                     szServiceName, 64 );

        MessageBox ( NULL, szMsg, szServiceName,
                     MB_ICONEXCLAMATION | MB_OK | MB_SERVICE_NOTIFICATION );


                                        // Log the trial-expired to the NT
                                        // app log.
        ErrorHandler( ID_LICENSEEXPIRED | ID_ERR_FATAL );

        goto Cleanup;
		}
#endif

                                        // Report running so startup will
                                        // continue.
   ReportStatusToSCMgr (SERVICE_RUNNING, NO_ERROR, 0);

Cleanup:

   LogMessage(_T("Exiting CNavAPService::InitService()"));

   return;
}

//***************************************************************************/
//      ServiceControlStop ()
//
//	Description:
//		To stop the Service Thread (based on User request).
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
CNavAPService::ServiceControlStop ()
{
HANDLE hThread;

   LogMessage(_T("Entering CNavAPService::ServiceControlStop()"));

                                        // First, tell them a few seconds
   ReportStatusToSCMgr (SERVICE_STOP_PENDING,
                        NO_ERROR,
                        5000);

   m_bExitingService = TRUE;

   LogMessage(_T("Starting StopAPService thread"));

                                        // Start a thread that will take
                                        // care of cleaning up the rest of
                                        // our services.
   hThread = (HANDLE)_beginthread (
                StopAPServiceProc,      // start Addr
                MAX_THREAD_STACK_SIZE,  // stack size
                NULL);                  // We don't use the user parameter.

                                        // Check for error
   if (hThread == (HANDLE)-1)
   {
      ErrorHandler (ID_ERR_BEGINTHREAD_FAILED);
   }

   LogMessage(_T("Exiting CNavAPService::ServiceControlStop()"));
}

//***************************************************************************/
//      main ()
//
//      This is the main entry point for the NAV Auto-Protect Service process.
//	This calls the CService Object's Create function which takes care of
//	registering with Service-Control-Manager and starting the Thread etc.
//
//      Parameters:
//              dwArgc                          # of the command line arguments
//              lpArgv                          Command line options array.
//
//      Returns:
//              VOID
//***************************************************************************/
//	04/18/96	PVENKAT
//***************************************************************************/
VOID
main (DWORD dwArgc, LPTSTR *lpArgv)
{
	TCHAR			szService[MAX_PATH + 1];
	int			nRet;
	HINSTANCE		hInst;

	hInst = GetModuleHandle (lpArgv[0]);
	nRet = LoadString (hInst,
			IDS_NAVAP_SERVICE_NAME,
			szService,
			sizeof (szService) / sizeof (szService[0]));

	if (nRet != 0)
   {
		APService.Create (szService);
   }
	else
   {
		SYM_ASSERT (0);
   }
}
