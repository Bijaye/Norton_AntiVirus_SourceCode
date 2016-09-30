/////////////////////////////////////////////////////////////////////
// MAINMGR.CPP - Created - 01/16/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/mainmgr.cpv   1.2   02 Jul 1997 15:16:16   DSACKIN  $
//
// Description:
//    Implementation file for the main Queue processing object
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/mainmgr.cpv  $
// 
//    Rev 1.2   02 Jul 1997 15:16:16   DSACKIN
// Now we will process all alerts from the AP.  If we aren't supposed to
// send alerts from the AP, our allowed mask of Alerts gets set to
// Activity logging and NT Event Logging.
//
//    Rev 1.1   18 May 1997 05:06:28   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:14   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:27:24   DSACKIN
// Moved to a new AlertQueueManager
//
//    Rev 1.0   13 Mar 1997 15:06:10   RFULLER
// Initial revision
//
//    Rev 1.0   18 Jan 1997 06:58:46   DSACKIN
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
#include "linksrv.h"
#include "service.h"
#include "svcctrl.h"
#include "navrpc.h"
#include "xapi.h"
#include "at_opts.h"                    // For CAlertOptions class
#include "alertsvc.h"
#include "alertstr.h"

#include "CMail.h"
#include "CSMTP.h"

                                        // Queue headers.
#include "AlertQ.h"
#include "MainMgr.h"



/////////////////////////////////////////////////////////////////////
// Lets start this file by taking care of our CManagerEntry object


/////////////////////////////////////////////////////////////////////
// CManagerEntry::CManagerEntry()
//
// Purpose:
//    Constructor for our CManagerEntry object
//
// Parameters:
//    CAlertQueueManager *lpcAlertQueueManager     - [in] - The Manager
//    CString             lpszAlertQueueMangerName - [in] - A name for the manager
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CManagerEntry::CManagerEntry(
      CAlertQueueManager *lpcAlertQueueManager,     // [in] - The Manager
      CString             lpszAlertQueueManagerName)// [in] - A name for the manager
{

                                        // Pretty basic.  Just assign the
                                        // incoming data.
   m_lpcAlertQueueManager    = lpcAlertQueueManager;
   m_szAlertQueueManagerName = lpszAlertQueueManagerName;
}


/////////////////////////////////////////////////////////////////////
// void CManagerEntry::SetAlertQueueManager(
//
// Purpose:
//    This allows someone to set the AlertQueueManager for retrieval
//    at a later date.
//
// Parameters:
//   CAlertQueueManager *lpcAlertQueueManager - [in] - New AlertQueueManager
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CManagerEntry::SetAlertQueueManager(
   CAlertQueueManager *lpcAlertQueueManager)
{
   m_lpcAlertQueueManager = lpcAlertQueueManager;
}


/////////////////////////////////////////////////////////////////////
// void CManagerEntry::GetAlertQueueManager (
//
// Purpose:
//    This allows someone to get the AlertQueueManager from our object
//
// Parameters:
//   CAlertQueueManager **lpcAlertQueueManager - [out] - reference to the
//                                                       current QueueManager
//
// Returns:
//    see parameters
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CManagerEntry::GetAlertQueueManager (
   CAlertQueueManager **lpcAlertQueueManager)
{
   *lpcAlertQueueManager = m_lpcAlertQueueManager;
}


/////////////////////////////////////////////////////////////////////
// void CManagerEntry::SetAlertQueueManagerName (
//
// Purpose:
//    Allows someone to set the AlertQueueManagerName for later
//    retrieval
//
// Parameters:
//   CString lpszAlertQueueManagerName - [in] - New QueueManager name
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CManagerEntry::SetAlertQueueManagerName (
   CString lpszAlertQueueManagerName)
{
   m_szAlertQueueManagerName = lpszAlertQueueManagerName;
}

/////////////////////////////////////////////////////////////////////
// void CManagerEntry::GetAlertQueueManagerName (
//
// Purpose:
//   This function allows someone to get the name of the current
//   AlertQueueManager.
//
// Parameters:
//   CString& lpszAlertQueueManagerName - [out] - Name of the AlertQueueManager
//
// Returns:
//   See Parameters
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CManagerEntry::GetAlertQueueManagerName (
   CString& lpszAlertQueueManagerName)
{
   lpszAlertQueueManagerName = m_szAlertQueueManagerName;
}




/////////////////////////////////////////////////////////////////////
// CMainAlertQueueManager::CMainAlertQueueManager()
//
// Purpose:
//    Constructor for our Main Alert queue manager
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CMainAlertQueueManager::CMainAlertQueueManager()
{
                                        // We are going to be the one handing
                                        // out targets, so we will be receiving
                                        // alerts only and we need to let the
                                        // alertqueueing mechanism know that.
   SetRequireTarget(FALSE);
}

/////////////////////////////////////////////////////////////////////
// CMainAlertQueueManager::~CMainAlertQueueManager()
//
// Purpose:
//    Destructor for our Main Alert queue manager
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CMainAlertQueueManager::~CMainAlertQueueManager()
{
                                        // This call to ShutDown is OK because
                                        // if the list is empty, there is
                                        // nothing to clean up.
   StopManager();
}

/////////////////////////////////////////////////////////////////////
// void CMainAlertQueueManager::ShutDown(void)
//
// Purpose:
//    This function is to be called when the MainAlertQueueManager is
//    shut down.  It will in turn shut down all of the other AlertQueueManager
//    objects.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/17/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CMainAlertQueueManager::ShutDown(void)
{
CManagerEntry      *lpcManagerEntry;
CAlertQueueManager *lpcAlertQueueManager;
POSITION            pPos;

   LogMessage ( _T("Starting ShutDown Process") );

                                        // Lets start at the head and go
                                        // through all of the managers and
                                        // Stop them.  After that, we will go
                                        // through and delete them all
   for ( pPos = m_oblManagers.GetHeadPosition();
         pPos != NULL;
         )
   {
                                        // Get the next queue manager
      lpcManagerEntry = (CManagerEntry *)
                                m_oblManagers.GetNext(pPos);

      lpcManagerEntry->GetAlertQueueManager(
                          &lpcAlertQueueManager);

      lpcAlertQueueManager->StopManager();

   }

                                        // When we are done, we are going to
                                        // have to go through and delete all of
                                        // the managers that have been added to
                                        // our manager list.

                                        // The reason we Stop the managers and
                                        // then delete them is because when we
                                        // delete them it KILLS the RPC link
                                        // for all of the other guys

   while (!m_oblManagers.IsEmpty())
   {
      lpcManagerEntry = (CManagerEntry *)
                           m_oblManagers.RemoveTail();

      lpcManagerEntry->GetAlertQueueManager(
                          &lpcAlertQueueManager);

                                        // Get rid of the manager
      delete lpcAlertQueueManager;

                                        // And then the entry in our Queue.
      delete lpcManagerEntry;
   }

                                        // Nothing can really fail here.
   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// BOOL CMainAlertQueueManager::StartUp(VOID)
//
// Purpose:
//    This function gets the main queue manager up and running.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Initialization happened exactly as expected
//    FALSE - Something didn't initialize.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CMainAlertQueueManager::StartUp(VOID)
{
CManagerEntry      *lpcManagerEntry;
CAlertQueueManager *lpcAlertQueueManager;
CString             szAlertQueueManagerName;
BOOL                bReturn = TRUE;
POSITION            pPos;

                                        // Lets start at the head and go
                                        // through all of the managers
   for ( pPos = m_oblManagers.GetHeadPosition();
         bReturn && (pPos != NULL);
         )
   {
                                        // Get the next queue manager
      lpcManagerEntry = (CManagerEntry *)
                                m_oblManagers.GetNext(pPos);

      lpcManagerEntry->GetAlertQueueManager(
                          &lpcAlertQueueManager);

      lpcManagerEntry->GetAlertQueueManagerName(
                          szAlertQueueManagerName);

                                        // And initialize it
      bReturn = lpcAlertQueueManager->Initialize(
                   szAlertQueueManagerName);

   }

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// VOID CMainAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This is where we are going to distribute the alerts and targets
//    to the individual target managers.
//
// Parameters:
//   CVirusAlert &  cVirusAlert      - [in] - Information about the alert
//   CAlertTarget        *lpcNotUsed - [in] - Where to send it.
//                                          - Will always be NULL in this
//                                          - case
//
// Returns:
//   none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
// 07-02-97 - DSACKIN - Now we decide if we are to process alerts
//                    - from NAVW or NAVAPSVC.
/////////////////////////////////////////////////////////////////////
VOID CMainAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcNotUsed)            // [in] - Where to send it.
{
CAlertOptions       cAlertOptions;
CVirusAlert        *lpcNewVirusAlert;
CAlertTarget       *lpcTarget;
CManagerEntry      *lpcManagerEntry;
CAlertQueueManager *lpcAlertQueueManager;
DWORD               dwTargetType;
DWORD               dwAcceptedTypes = 0xFFFFFFFF;
DWORD               dwPacketSrc;
POSITION            pManagersPos;
BOOL                bAssigned;

                                        // We can't do our work if there is no
                                        // work to do.
   if (NULL == lpcVirusAlert)
   {
      goto Cleanup;
   }

                                        // Ok... Now find out what to do with
                                        // the alert.
   if (!cAlertOptions.LoadOptions())
   {
      goto Cleanup;
   }

                                        // First, lets figure out what types of
                                        // alerts we are going to allow through
                                        // based on where the alert came from.
   if (!lpcVirusAlert->GetPacketSource(
           dwPacketSrc))
   {
      goto Cleanup;
   }

                                        // This applies ONLY to local alerts.
   if (dwPacketSrc & PACKET_SOURCE_LOCAL)
   {
                                        // If it came from the Local NAVAP, we
                                        // need to check to see if alerts are
                                        // off.
      if ( (dwPacketSrc & PACKET_SOURCE_NAVAP) &&
           (!cAlertOptions.GetAPRemoteAlertsEnabled()) )
      {
                                        // We must LOG them anyway.
         dwAcceptedTypes = TARGET_ACTIVITY_LOG | TARGET_EVENTLOG;
      }

   }

   while (NULL != (lpcTarget = cAlertOptions.RemoveNextTarget()))
   {

                                        // Start off with this flag set to
                                        // FALSE until it is assigned to a
                                        // queue.
      bAssigned = FALSE;

                                        // Only process the target if it is
                                        // Active and it is an accepted target
                                        // type.
      if ( (lpcTarget->GetTargetType() & dwAcceptedTypes) &&
           (lpcTarget->GetActive()) )
      {

                                        // OK, which manager are we looking for?
         dwTargetType = lpcTarget->GetTargetType();

         LogMessage( _T("Processing Target Type: %d"), dwTargetType);

                                        // Lets start at the head and go
                                        // through all of the managers
         for ( pManagersPos = m_oblManagers.GetHeadPosition();
               !bAssigned && (pManagersPos != NULL);
               )
         {
            lpcManagerEntry = (CManagerEntry *)
                                 m_oblManagers.GetNext(pManagersPos);

                                        // Get the next queue manager
            lpcManagerEntry->GetAlertQueueManager(
                                 &lpcAlertQueueManager);

                                        // Is this the manager that we are
                                        // looking for?
            if (dwTargetType == lpcAlertQueueManager->GetAlertType())
            {
                                        // OK.  Now that we found it, we need
                                        // to make a copy of the alert and give
                                        // it to this manager.
               lpcNewVirusAlert = new CVirusAlert (*lpcVirusAlert);

               if (NULL != lpcNewVirusAlert)
               {
                  lpcAlertQueueManager->AddAlert(
                     lpcNewVirusAlert,
                     lpcTarget);

                  bAssigned = TRUE;
               }
               else
               {
                  LogMessage (_T("Failed to create a new Alert to give to a secondary queue."));
               }
            }
         }

      }

                                        // If the target has not been assigned,
                                        // we are responsible to free up the
                                        // memory being used by the object
      if (!bAssigned)
      {
         delete lpcTarget;
      }
   }

Cleanup:

                                        // We MUST clean up the alert when we
                                        // are done with it.
   if (NULL != lpcVirusAlert)
   {
      delete lpcVirusAlert;
      lpcVirusAlert = NULL;
   }

}


/////////////////////////////////////////////////////////////////////
// VOID CMainAlertQueueManager::AddAlertQueueManager(
//
// Purpose:
//    This will add a new manager to our internal list.
//
// Parameters:
//   CAlertQueueManager *lpcAlertQueueManager      - [in] - Manager to add to our queue
//   LPWSTR              lpszAlertQueueManagerName - [in] - Name of the manager to add
//
// Returns:
//   none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CMainAlertQueueManager::AddAlertQueueManager(
   CAlertQueueManager *lpcAlertQueueManager,  // [in] - Manager to add to our queue
   LPWSTR              lpszAlertQueueManagerName)  // [in] - Name of the manager to add
{
CManagerEntry *lpManagerEntry;

   lpManagerEntry = new CManagerEntry(
                           lpcAlertQueueManager,
                           lpszAlertQueueManagerName);

   if (NULL == lpManagerEntry)
   {
      goto Cleanup;
   }
                                        // Add it to the list so that our
                                        // processing function can handle it
   m_oblManagers.AddHead(
      lpManagerEntry);

Cleanup:

   return;
}

