//      Copyright 1996 Symantec Corporation
/////////////////////////////////////////////////////////////////////
//      $Header:   S:/ALERTSVC/VCS/alertsvc.cpv   1.10   14 Aug 1997 18:22:34   DSACKIN  $
//
//      Description:
//              This source file contains routines for NAV Alert Service (for NT)
//
//
/////////////////////////////////////////////////////////////////////
//
//      $Log:   S:/ALERTSVC/VCS/alertsvc.cpv  $
// 
//    Rev 1.10   14 Aug 1997 18:22:34   DSACKIN
// All of our RPCServer managment functions are in alertrpc.cpp now.
//
//    Rev 1.9   30 Jun 1997 19:07:46   DSACKIN
// We should be doing NT Event logging even in the non-Server product.
//
//    Rev 1.8   23 Jun 1997 16:57:46   DSACKIN
// Oops.  I shouldn't have removed Local popups and activity log
// from NT workstation.
//
//    Rev 1.7   23 Jun 1997 12:26:02   DSACKIN
// Added Personality DLL Functionality.
//
//    Rev 1.6   09 Jun 1997 01:41:08   DSACKIN
// When adding a new CVirusAlert to our main manager, we CAN'T delete the
// alert if we have handed it off to him.
//
//    Rev 1.5   21 May 1997 13:43:26   DSACKIN
// Added processing for Marconi type alerts.
//
//    Rev 1.4   18 May 1997 05:06:12   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.3   08 May 1997 14:12:58   GDZIECI
// Moved StartAlertReceivers() before initializing queue managers.
//
//    Rev 1.2   07 May 1997 15:41:04   GDZIECI
// Added support to receive AppleTalk, IPX and IP alerts.
//
//    Rev 1.1   18 Apr 1997 13:40:38   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:23:00   DSACKIN
// Updated the queue mechanism.
//
//    Rev 1.0   13 Mar 1997 15:06:04   RFULLER
// Initial revision
//
//    Rev 1.15   28 Jan 1997 18:16:46   DSACKIN
// We now check for command packets.
//
//    Rev 1.14   28 Jan 1997 15:53:44   DSACKIN
// Removed Security off of the RPC link.
//
//    Rev 1.13   23 Jan 1997 17:13:42   DSACKIN
// Fixed a bug in the destructor where we were Getting the tail object of
// the UUID queue and deleting it, and then getting it again and doing it
// over and over.  It should have been RemoveTail instead of GetHead.
//
//    Rev 1.12   18 Jan 1997 06:59:40   DSACKIN
// Reworked the Alert Service to use the new CAlertQueueManager class for
// all alerting.
//
//    Rev 1.11   13 Jan 1997 22:26:56   DSACKIN
// We now send a command to NAVAP Service telling it that we are ready to
// start accepting Alerts when we are done starting up.
//
//    Rev 1.10   20 Dec 1996 13:59:18   DSACKIN
// Changed the UUID queue to use RemoveTail instead of GetTail.
//
//    Rev 1.9   19 Dec 1996 13:23:42   DSACKIN
// The messenger ServiceControl object was moved to be a member variable because
// when it went out of scope of the function it used to be in, it would shut
// down all communication from the current process to the Logging mechanism.
//
//    Rev 1.8   27 Nov 1996 05:33:12   DSACKIN
// Start the definition monitor.
//
//    Rev 1.7   26 Nov 1996 07:19:34   DSACKIN
// Start the LocalPopupManager thread.
//
//    Rev 1.6   26 Nov 1996 06:21:06   DSACKIN
// Start a new thread to handle the Forwarding events.
//
//    Rev 1.5   20 Nov 1996 22:12:46   DSACKIN
// Moved all utility functions to util.cpp
//
//    Rev 1.4   19 Nov 1996 14:56:24   DSACKIN
// Added code to take care of blocking circular packet routing.
//
//    Rev 1.3   14 Nov 1996 01:41:08   KSACKIN
// Removed comments in preparation for the CAlertOptions class implementation.
// This class will now only be used in the NTHandle.cpp file.
//
//    Rev 1.2   13 Nov 1996 22:12:34   DSACKIN
// Moved out Notification targets to separate files.
//
//    Rev 1.1   08 Nov 1996 17:13:26   TSMITH
// Ported messaging, alerting and logging functionality from NAVAPSVC.
// Added new alerting functions, utility functions and message packet
// handling functionality.
//
//    Rev 1.25   18 Oct 1996 18:25:18   TSmith
// Removed obsolete #ifdefs.
//
//    Rev 1.24   08 Sep 1996 15:31:46   DSACKIN
// Removed Definition of MAX_MESSAGE_LEN.  This define is no longer used in
// this project.  All references to this define are now in CService.
//
//    Rev 1.23   06 Sep 1996 09:56:40   PVENKAT
// Fixed rework on #54304.  Messenger service will be started if not running when
// we get a msg from AP Service of Virus detection.
//
//
//    Rev 1.22   26 Aug 1996 01:26:36   DSACKIN
// Don't allow a stop to complete until we are either initialized or
// fail to initialize.  This mainly is used for when we are waiting for the
// messenger service to start so we don't yank his variables out from
// under him.
//
//    Rev 1.21   13 Aug 1996 21:33:10   DSACKIN
// For every state that the messenger service is in, handle the startup
// of that service correctly.
//
//    Rev 1.20   05 Aug 1996 16:01:04   DSACKIN
// Changed all calls to ErrorHandlerID to ErrorHandler.
//
//    Rev 1.19   29 Jul 1996 00:52:14   DSACKIN
// We now start a thread to do our initialization instead of in InitService.
//
//    Rev 1.18   23 Jul 1996 17:32:26   DSACKIN
// Increased the timeouts and added DEBUG_PROFILE stuff.
//
//    Rev 1.17   23 Jul 1996 13:50:12   DSACKIN
// Numerous changes to use RPC.
//
//    Rev 1.16   08 Jul 1996 14:23:54   DSACKIN
// Added ID_ERR_FATAL to the error messages that should stop the service.
//
//    Rev 1.15   28 Jun 1996 16:06:52   DSACKIN
// Fixed the alerting service to return a TRUE/FALSE if we did the message correctly.
//
//    Rev 1.14   21 Jun 1996 16:18:28   PVENKAT
// Importing NetMessageBufferSend from NETAPI32 dynamically to avoid crashing
// on WIN95.
//
//
//    Rev 1.13   13 Jun 1996 17:51:40   DSACKIN
// Added DEBUG_PROFILE strings.
//
//    Rev 1.12   13 Jun 1996 16:19:34   PVENKAT
// Fixed #57530.  Alert Service now cannot Pause/Continue.
//
//
//    Rev 1.11   06 Jun 1996 14:02:00   PVENKAT
// Added CheckMessenger function to check and start Messenger Service.
//
//    Rev 1.10   28 May 1996 11:42:30   PVENKAT
// Removed debug breakpoint.
//
//    Rev 1.9   26 May 1996 13:12:54   PVENKAT
// Changed the buffer size (from 256 to 2048).  Small buf size caused problems
// while 'alerting' on Long-file-names.
//
//    Rev 1.8   23 May 1996 13:36:58   DSACKIN
// Report more often to the Service Control Manager.  This will keep us from
// timing out when being started.
//
//    Rev 1.7   24 Apr 1996 20:21:08   DSACKIN
// Give better estimations to Service Control Manager.
//
//    Rev 1.6   24 Apr 1996 16:38:12   DSACKIN
// Report back to the ServiceControl Manager when we are installed OK.
//
//    Rev 1.5   19 Apr 1996 18:57:18   DSACKIN
// Removed TBD section.
//
//    Rev 1.4   17 Apr 1996 14:52:26   DSACKIN
// GetAlertServer now only gets current machine name which is what we will use
// in Gryphon.  For future releases, we will leave this function for NEF integration.
//
//    Rev 1.3   16 Apr 1996 14:50:26   DSACKIN
// Use Error IDs that describe what to do with the error.
//
//    Rev 1.2   15 Apr 1996 15:20:30   DSACKING
// Cleaned it up and added much error checking.
//
//    Rev 1.1   08 Apr 1996 14:39:34   PVENKAT
// Changed Handler & ServiceMain to WINAPI to STOP on NT4.0 gracefully.
//
//    Rev 1.0   29 Mar 1996 14:46:46   PVENKAT
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////
//      Includes
#include "afxtempl.h"
#include "platform.h"
#include "process.h"
#include "winnetwk.h"
#include "lmcons.h"
#include "lmmsg.h"
#include "lmerr.h"
#include "servenfy.h"
#include "linksrv.h"
#include "service.h"
#include "svcctrl.h"
#include "navrpc.h"
#include "apscomm.h"
#include "xapi.h"

#include "QueueMgr.h"
#include "AlertQ.h"
#include "MainMgr.h"
#include "Managers.h"

#include "AlerType.h"

#include "alertsvc.h"
#include "alertstr.h"

#include "n32behav.h"

/////////////////////////////////////////////////////////////////////
//      Constants

#define MAX_THREAD_STACK_SIZE (10240)   // Max Stack size.
#define MAX_BUFLEN            (512)     // Used for i/o with pipe.
#define MAX_AP_BUFSIZE        (10240)

/////////////////////////////////////////////////////////////////////
// Globals

DECLARE_SERVICE(CAlertService, AlertService, IDS_ERR_REGISTERSERVICECTRLR_FAILED);

HINSTANCE ghInst;

/////////////////////////////////////////////////////////////////////
// Prototypes

DWORD SendPacketProc(
   LPVOID pInPacket);                   // [in] - Incoming data.


/////////////////////////////////////////////////////////////////////
// Functions


/////////////////////////////////////////////////////////////////////
// CAlertService : Constructor
//
// Our constructor will initialize any variables that we will need.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/10/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CAlertService::CAlertService(
   LPSERVICE_MAIN_FUNCTION pMain,       // [in] - Pointer to our Main callback
   LPHANDLER_FUNCTION      pHandler) :  // [in] - Pointer to our Handler callback
      CService(pMain, pHandler)         // Pass through initialization to
                                        // our base CService.
{
                                        // Set this to true after we are
                                        // done with our initialization and
                                        // we are all ready to send alerts.
   m_bInitialized = FALSE;

                                        // If we fail our startup and we have
                                        // messages in our Queue, we use this
                                        // signal to dump the messages.
   m_bStartupFailed = FALSE;

                                        // This option isn't currently used in
                                        // the Alert Service because the Alert
                                        // service doesn't try to split the
                                        // definition files.
   m_bPauseWatchingDefs = FALSE;


                                        // Initialize ptrs to objects that will
                                        // listen to ATP, IPX and IP alerts
   m_pATPReceiver = NULL;
   m_pIPXReceiver = NULL;
   m_pIPReceiver  = NULL;

                                        // This is our flag to tell us when we
                                        // have to re-start the RPC Server
                                        // instead of just start it.
   m_bRPCServerListening = FALSE;

                                        // Set this bool when the service is
                                        // shutting down.
   m_bStoppingService = FALSE;

                                        // Set the first character to NULL.
   m_szNOSAPDat[0] = NULL;
                                        // This flag is to remember if we are
                                        // using the protocols that SAP or not.
   m_bEnableSAP = TRUE;

                                        // This critical section is to make
                                        // sure that we don't try to start and
                                        // stop the RPC Server at the same time.
   InitializeCriticalSection(&m_csRPCServer);

}


/////////////////////////////////////////////////////////////////////
// CAlertService : Destructor
//
// This function does our cleanup
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/10/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CAlertService::~CAlertService()
{
CUUIDObject *poUUID;

                                        // Take care of removing our UUID
                                        // history
   while (!m_UUIDHistory.IsEmpty())
   {
      poUUID = (CUUIDObject *) (m_UUIDHistory.RemoveTail());

                                       // If we got one, delete it
      if (poUUID)
      {
         delete poUUID;
         poUUID = NULL;
      }
   }

}


/////////////////////////////////////////////////////////////////////
// CAlertService : ServiceControlStop
//
// This function does our cleanup
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/10/96 DSACKIN - Function Created
// 07/22/96 DSACKIN - Added RPC support
/////////////////////////////////////////////////////////////////////
VOID CAlertService::ServiceControlStop(VOID)
{
DWORD dwTimeEstimate;

   LogMessage(_T("Recieved a request to Stop the Alert Server"));


                                        // Now... we can't safely exit
                                        // until our initialization is
                                        // either completed or we fail.

   while (!m_bInitialized &&
          !m_bStartupFailed)
   {
      LogMessage(_T("Hanging out because the AlertService hasn't finished initializing yet."));
                                        // Give ourselves 3 seconds.
      ReportStatusToSCMgr(
         SERVICE_STOP_PENDING,
         NO_ERROR,
         3000 );

                                        // Sleep one second
      Sleep (1000);
   }

                                        // Set the flag that says the AlertSvc
                                        // is shutting down.
   m_bStoppingService = TRUE;
                                        // This call will shut down our RPC
                                        // Server.
   CheckRPCServer();



   StopAlertReceivers();

   LogMessage(_T("Stopping RPC Alert Server"));

   ReportStatusToSCMgr(
      SERVICE_STOP_PENDING,
      NO_ERROR,
      3000 );

                                        // Time to shut down the managers.  For
                                        // the estimate to the
                                        // ServiceControlManager, we want to
                                        // give each manager 1.1 seconds to get
                                        // out (After 1 second, they are killed
                                        // anyway)
   dwTimeEstimate =
      (1100) * cMainAlertQueueManager.GetNumberOfManagers();

                                        // For my cMainAlertQueueManger
   dwTimeEstimate += 1100;

                                        // Now tell the SCMgr how long we need.
   ReportStatusToSCMgr(
      SERVICE_STOP_PENDING,
      NO_ERROR,
      dwTimeEstimate );

                                        // Now shut him down!
   cMainAlertQueueManager.StopManager();

   ReportStatusToSCMgr(
      SERVICE_STOPPED,
      NO_ERROR,
      0 );

}


/////////////////////////////////////////////////////////////////////
// BOOL CAlertService::ValidateUUID
//
// Purpose:
//    Check the UUID to see if it is in the list.  If it is not,
//    add it.
//
// Parameters:
//    UUID uuidToCheck - [in] - UUID to check.
//
// Returns:
//    TRUE  - UUID is valid
//    FALSE - UUID was duplicate
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/18/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_UUIDS  128

BOOL CAlertService::ValidateUUID(
    UUID uuidToCheck)
{
BOOL         bFound = FALSE;
POSITION     pList;
CUUIDObject *poUUID;
UUID         uuidTemp;

                                        // First off make sure we don't have
                                        // more than the maximum number of UUIDs
   while (m_UUIDHistory.GetCount() > MAX_NUMBER_OF_UUIDS)
   {

                                        // Get the last one
      poUUID = (CUUIDObject *) (m_UUIDHistory.RemoveTail());

                                        // If we got one, delete it
      if (poUUID)
      {
         delete poUUID;
         poUUID = NULL;
      }
   }


                                        // Now iterate through looking for the
                                        // correct one
   for ( pList = m_UUIDHistory.GetHeadPosition();
         (pList != NULL) && !bFound;
         )
   {
      poUUID = (CUUIDObject *) (m_UUIDHistory.GetNext( pList ));

      uuidTemp = poUUID->GetValue();

      if (!memcmp( (LPVOID) &uuidToCheck, (LPVOID) &uuidTemp, sizeof (UUID)) )
         bFound = TRUE;
   }

   if (!bFound)
   {
      poUUID = new CUUIDObject( uuidToCheck );
      m_UUIDHistory.AddHead(poUUID);
   }

   return !bFound;
}


/////////////////////////////////////////////////////////////////////
// DWORD CAlertService::ProcessCommand(
//
// Purpose:
//    If the packet that came across is (sizeof(DWORD) * 2), then
//    we know we are getting a command.  We process the commands
//    here
//
// Parameters:
//   DWORD dwCommand - [in] - Formatted buffer from client.
//
//
// Returns:
//    DWORD  - To be returned to the application requesting our action
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/28/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CAlertService::ProcessCommand(
   DWORD dwCommand)                     // Command from packet
{
DWORD dwReturn;

   switch (dwCommand)
   {
      case NAVAPSVC_PAUSE_WATCHING_DEFS:
         LogMessage(_T("ProcessRequest - NAVAPSVC_PAUSE_WATCHING_DEFS"));
         m_bPauseWatchingDefs = TRUE;
         dwReturn = NAVAPSVC_SUCCESS;
         break;

      case NAVAPSVC_CONTINUE_WATCHING_DEFS:
         LogMessage(_T("ProcessRequest - NAVAPSVC_CONTINUE_WATCHING_DEFS"));
         m_bPauseWatchingDefs = FALSE;
         dwReturn = NAVAPSVC_SUCCESS;
         break;

      default:
         LogMessage(_T("*** Received an unknown command: %d"), dwCommand);
         dwReturn = NAVAPSVC_ERROR;
         break;
   }

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// DWORD CAlertService::ProcessCVirusAlert(
//
// Purpose:
//    This does function takes a CVirusAlert and processes it
//    using the methods of our new alert service.
//
// Parameters:
//    CAlert * lpcAlert - [in] - Packet from the client.
//
// Returns:
//    DWORD  - Return value to the client.
//
/////////////////////////////////////////////////////////////////////
// 05-19-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CAlertService::ProcessCVirusAlert(
   CAlert *lpcAlert)                    // [in] - Packet from the client.
{
CVirusAlert *lpcVirusAlert = NULL;
LPBYTE       lpBuffer = NULL;
DWORD        dwReturn = ERROR_SUCCESS;
BOOL         bAddedSuccessfully = FALSE;


   lpcVirusAlert = new CVirusAlert ( * ((CVirusAlert *) lpcAlert ));

                                        // Check for the validity of our packet.
   if (NULL == lpcVirusAlert)
   {
      LogMessage(_T("Failed to create the new CVirusAlert class."));
      goto Cleanup;
   }

   if (cMainAlertQueueManager.AddAlert(
         lpcVirusAlert,                 // Send in the packet
         (CAlertTarget *) NULL))        // The Main Manager will handle this
   {
      bAddedSuccessfully = TRUE;
   }
   else
   {
      LogMessage(_T("Failed call to cMainAlertQueueManager.AddAlert"));

                                        // If we aren't able to pass on the
                                        // packet we created, we are
                                        // responsible for deleting it.
      delete lpcVirusAlert;
   }

Cleanup:

   if (!bAddedSuccessfully && (NULL != lpcVirusAlert))
   {
      delete lpcVirusAlert;
      lpcVirusAlert = NULL;
   }

   return( dwReturn );
}

/////////////////////////////////////////////////////////////////////
// SendPacket
//
// Receives and queues a NAV_NOTIFICATION_PACKET. Packet received via
// RPC and queued for later processing so this function can return
// quickly.
//
// Parameters:
//      PBYTE pInPacket - Formatted buffer from client.
//
// Return Value:
//      DWORD Result - A Windows ERROR_XXXX value.
//
/////////////////////////////////////////////////////////////////////
// 10/25/96 TSMITH  Function created
/////////////////////////////////////////////////////////////////////
DWORD CAlertService::SendPacket(
   LPVOID pInPacket)                    // [in] - Formatted buffer from client.
{
CAlert      *lpcAlert = NULL;
DWORD       *lpdwPtr;
UUID         uuidUUID;
BOOL         bStatus;
DWORD        dwReturn = ERROR_SUCCESS;
DWORD        dwPacketType;

   LogMessage(_T("Received an alert packet."));

   if ( pInPacket == NULL )
   {

      LogMessage(_T("Packet was NULL."));

      dwReturn = ERROR_INVALID_DATA;
      goto Cleanup;
   }

                                        // First off, check to see if it is a
                                        // command packet
   lpdwPtr = (DWORD *) pInPacket;

   if ( (sizeof(DWORD) * 2) == *lpdwPtr)
   {
                                        // OK.. Its a command... Process the
                                        // command

                                        // Bump to the command DWORD
      lpdwPtr++;

                                        // Process the command and send back
                                        // the return value
      dwReturn = ProcessCommand(*lpdwPtr);

                                        // Bail out!
      goto Cleanup;
   }

   lpcAlert = new CAlert ((LPBYTE) pInPacket);

   if (NULL == lpcAlert)
   {
      LogMessage(_T("Failed to create a new CAlert object"));
      goto Cleanup;
   }

                                        // Check to make sure that we haven't
                                        // seen this packet before.
   bStatus = lpcAlert->GetUniqueID(
                uuidUUID);

   if (!bStatus)
   {
      LogMessage(_T("Failed to get UUID from class."));
      goto Cleanup;
   }
                                        // Check its UUID
   if (!ValidateUUID(uuidUUID))
   {
      LogMessage (_T("Received a duplicate packet UUID."));
      goto Cleanup;
   }

                                        // OK.  Now that we know it is a CAlert
                                        // type packet, we need to figure out
                                        // what type of packet it really is.
   if (!lpcAlert->GetPacketType(dwPacketType))
   {
      LogMessage(_T("Failed to get the packet type."));
      goto Cleanup;
   }

   switch (dwPacketType)
   {
      case ALERT_TYPE_MARCONI:
         ProcessCAlertNotification(
            lpcAlert);
         break;

      case ALERT_TYPE_CVIRUSALERT:
         ProcessCVirusAlert(
            lpcAlert);
         break;

      case ALERT_TYPE_CMESSAGEALERT:
         break;

      default:
         LogMessage(_T("Didn't understand what type of packet I got."));
         break;
   }


Cleanup:

   if (lpcAlert)
   {
      delete lpcAlert;
      lpcAlert = NULL;
   }

   return( dwReturn );
}


/////////////////////////////////////////////////////////////////////
// SendPacketProc
//
// This function is a pass through function for our class's packet
// recieving function.
//
// Parameters:
//      LPVOID pInPacket - Incomming data buffer.
//
// Return Value:
//      DWORD - Returned from our member function.
//
/////////////////////////////////////////////////////////////////////
// 10/28/96 TSMITH  Function created.
/////////////////////////////////////////////////////////////////////
DWORD SendPacketProc(
   LPVOID pInPacket)                    // [in] - Incoming data.
{
   return( AlertService.SendPacket( pInPacket ) );
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlertService::InitializeMainAlertQueueManager(VOID)
//
// Purpose:
//    This will set up the Main Alert Queue Manager.
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - Initialization went successfully
//    FALSE - Something didn't happen correctly.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertService::InitializeMainAlertQueueManager(VOID)
{
CAlertQueueManager *lpcAlertQueueManager;
BOOL                bReturn = TRUE;

                                        // Add a LocalPopup Manager
   lpcAlertQueueManager = new CLocalPopupAlertQueueManager;

   if (NULL == lpcAlertQueueManager)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   cMainAlertQueueManager.AddAlertQueueManager(
      lpcAlertQueueManager,
      _T("CLocalPopupAlertQueueManager"));

                                        // Add an ActivityLog Manager
   lpcAlertQueueManager = new CActivityLogAlertQueueManager;

   if (NULL == lpcAlertQueueManager)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   cMainAlertQueueManager.AddAlertQueueManager(
      lpcAlertQueueManager,
      _T("CActivityLogAlertQueueManager"));

                                        // Add an NT EventLog Manager
   lpcAlertQueueManager = new CEventLogAlertQueueManager;

                                        // The EventLog Manager needs to have a
                                        // pointer to the CService Class.
   ((CEventLogAlertQueueManager *)lpcAlertQueueManager)->SetService(this);

   if (NULL == lpcAlertQueueManager)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   cMainAlertQueueManager.AddAlertQueueManager(
      lpcAlertQueueManager,
      _T("CEventLogAlertQueueManager"));



   if (GetBehavior(BEHAVIOR_SERVER,0))
   {
                                        // Add a MessengerService Manager
      lpcAlertQueueManager = new CRemotePopupAlertQueueManager;

      if (NULL == lpcAlertQueueManager)
      {
         bReturn = FALSE;
         goto Cleanup;
      }

      cMainAlertQueueManager.AddAlertQueueManager(
         lpcAlertQueueManager,
         _T("CRemotePopupAlertQueueManager"));

   }

                                        // Add a NAV NLM AlertManager
   lpcAlertQueueManager = new CNavNlmAlertQueueManager;

   if (NULL == lpcAlertQueueManager)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   cMainAlertQueueManager.AddAlertQueueManager(
      lpcAlertQueueManager,
      _T("CNavNlmAlertQueueManager"));


                                        // Add a Forwarding AlertManager
   lpcAlertQueueManager = new CForwardAlertQueueManager;

   if (NULL == lpcAlertQueueManager)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   cMainAlertQueueManager.AddAlertQueueManager(
      lpcAlertQueueManager,
      _T("CForwardAlertQueueManager"));


   if (GetBehavior(BEHAVIOR_SERVER,0))
   {
                                        // Add a SMTP AlertManager
      lpcAlertQueueManager = new CSMTPAlertQueueManager;

      if (NULL == lpcAlertQueueManager)
      {
         bReturn = FALSE;
         goto Cleanup;
      }

      cMainAlertQueueManager.AddAlertQueueManager(
         lpcAlertQueueManager,
         _T("CSMTPAlertQueueManager"));


                                        // Add a SNMP AlertManager
      lpcAlertQueueManager = new CSNMPAlertQueueManager;

      if (NULL == lpcAlertQueueManager)
      {
         bReturn = FALSE;
         goto Cleanup;
      }

      cMainAlertQueueManager.AddAlertQueueManager(
         lpcAlertQueueManager,
         _T("CSNMPAlertQueueManager"));

   }

                                        // Once all of the AlertQueueManagers
                                        // have been added, it is time to
                                        // initialize the MainAlertQueueManager
   bReturn = cMainAlertQueueManager.Initialize(_T("MainAlertQueueManager"));

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// StartAlertService
//
// This function starts the alert service
//
// Parameters:
//    none.
//
// Return Value:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 07/28/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CAlertService::StartAlertService (VOID)
{
LINKCALLBACKS rCbk = {
                  (SENDCOMMANDPROC) SendPacketProc,
                  NULL,
                  NULL };

   LogMessage(_T("Entering CAlertService::StartAlertService()"));

                                        // Read NAVOPTS.DAT
   LoadOptions( );

                                        // Set up the RPC callbacks
   m_RpcAlertServer.SetCallbacks( &rCbk );

                                        // Now start the RPC server
   if (!CheckRPCServer())
   {
      ErrorHandler (
        ID_ERR_STARTING_RPC_SERVER);

      goto Cleanup;
   }

   LogMessage(_T("CAlertService::StartAlertService() - RPC Server Started."));

   StartAlertReceivers();


   LogMessage(_T("CAlertService::StartAlertService() - Now it is time to start our MainQueueManager"));

   if (!InitializeMainAlertQueueManager())
   {
      LogMessage( _T("CAlertService::StartAlertService() - Failed to initializeMainAlertQueueManager"));

      goto Cleanup;
   }

   LogMessage(TEXT("Starting Definition Monitor"));

   if (!StartDefinitionMonitor())
   {

      ErrorHandler (
         ID_ERR_STARTDEFINITIONMONITOR);

      LogMessage(TEXT("Failed call to StartDefinitionMonitor()"));
   }

                                        // If we get to this point, we
                                        // know everything is in place
   m_bInitialized = TRUE;

Cleanup:

   if (!m_bInitialized)
   {

      LogMessage(_T("CAlertService::StartAlertService() - Startup Failed"));

                                        // Signal to dump pending messages.
      m_bStartupFailed = TRUE;
   }
   else
   {
                                        // Here we have a successful startup.
                                        // Tell NAVAPSVC that it is OK to start
                                        // sending things over.

                                        // Don't check the return value because
                                        // we don't care if he heard.  It is
                                        // possible that we are running alone
                                        // (Without NAVAPSVC)
      NavRpcSendCommand(
         NAVAPSVC_PROTOCOL,
         NAVAPSVC_ADDRESS,
         NAVAPSVC_ENDPOINT,
         NAVAPSVC_ALERTSVC_READY );

   }

   LogMessage(_T("Exiting CAlertService::StartAlertService()"));
}


/////////////////////////////////////////////////////////////////////
// StartAlertServiceProc
//
// This function is a C callthrough to our C++ member function
//
// Parameters:
//    none.
//
// Return Value:
//    doesn't return.  We Terminate this thread when we are done.
//
/////////////////////////////////////////////////////////////////////
// 07/28/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID StartAlertServiceProc (LPVOID lpNotUsed)
{
   AlertService.StartAlertService();

   _endthread();                        // Don't just return!!!
}


/////////////////////////////////////////////////////////////////////
// InitService
//
// This function is where we crank up the service.  Here is where we
// start our RPC server.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/10/96 DSACKIN - Function Ported from Venkat's Service
/////////////////////////////////////////////////////////////////////
VOID CAlertService::InitService (VOID)
{
HANDLE        hThread;
WORD          wPlatform;
OSVERSIONINFO osInfo;


                                        // First Estimate at least 3 seconds
   ReportStatusToSCMgr (SERVICE_START_PENDING,
                        NO_ERROR,
                        6000);

   LogMessage(_T("Entering CAlertService::InitService()"));

   MEMSET (&osInfo, 0, sizeof (osInfo));
   osInfo.dwOSVersionInfoSize = sizeof (osInfo);

   GetVersionEx (&osInfo);
   wPlatform = (osInfo.dwPlatformId == VER_PLATFORM_WIN32s) ? PLAT_WIN31 :
               (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) ? PLAT_WINNT : PLAT_WIN95;

                                        // This application can not run on
                                        // WIN95, check this return blindly.
   if (wPlatform == PLAT_WIN95)
   {
       ErrorHandler (
          ID_ERR_WIN95 |
          ID_ERR_FATAL);
                                        // No need to report the status as
                                        // there is no SC MGR in 95!
       goto Cleanup;
   }

   LogMessage(_T("Entering CAlertService::InitService() - Platform OK"));

                                        // Keep feeding that Service Control Mgr
   ReportStatusToSCMgr (SERVICE_START_PENDING,
                        NO_ERROR,
                        3000);

   LogMessage(_T("Creating StartAlertService Thread"));

                                        // Start a thread that will take
                                        // care of getting everything running.
   hThread = (HANDLE)_beginthread (
                  StartAlertServiceProc,// start Addr
                  MAX_THREAD_STACK_SIZE,// stack size
                  NULL);                // We don't use the user parameter.

   if ((HANDLE)INVALID_HANDLE_VALUE == hThread)
   {
      ErrorHandler (
         ID_ERR_BEGINTHREAD_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

                                        // Don't allow Pause or Continue!
   AllowPauseContinue (FALSE);

                                        // OK.. We are done.
   ReportStatusToSCMgr (
      SERVICE_RUNNING,
      NO_ERROR,
      0);

Cleanup:

    LogMessage(_T("Exiting CAlertService::InitService()"));
}

/////////////////////////////////////////////////////////////////////
//  main ()
//
// This is the main entry point for the NAV Alert Service process.
// It simply starts with registering with Service Control Manager (SCM)
// and if it fails assumes that the executable is invoked to run as an
// application and not as a service and in that case starts the
// initialization.
//
//  Parameters:
//      dwArgc - # of the command line arguments
//      lpArgv - Command line options array.
//
//  Returns:
//      None.
//
/////////////////////////////////////////////////////////////////////
// 01/18/97 - DSACKIN - Started the function history
/////////////////////////////////////////////////////////////////////
VOID main(
   DWORD dwArgc,
   LPTSTR *lpArgv)
{
TCHAR       szAlert[MAX_PATH + 1];
HINSTANCE   hInst;
int         nReturn;

   hInst = GetModuleHandle (lpArgv[0]);

                                    // Make a copy for other functions that
                                    // are not a part of our CAlertService
                                    // class
   ghInst = hInst;

   nReturn = LoadString (
                hInst,
                IDS_NAV_ALERT_SERVICE_NAME,
                szAlert,
                sizeof (szAlert) / sizeof (szAlert[0]));

   if (0 != nReturn)
   {
                                        // Starts the service running.
      AlertService.Create(szAlert);
   }
   else
   {
      SYM_ASSERT( FALSE );
                                    // I'm not sure what to do with this
                                    // yet.  Our object hasn't been
                                    // initialized so we can't report??
   }
}
