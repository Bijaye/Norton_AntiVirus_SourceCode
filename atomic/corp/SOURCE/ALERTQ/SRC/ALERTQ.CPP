/////////////////////////////////////////////////////////////////////
// QUEUEMGR.CPP - Created - 01/15/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTQ/VCS/alertq.cpv   1.2   09 Jun 1997 01:45:52   DSACKIN  $
//
// Description:
//    Here is the implementation file for the Queue Manager class
//    that is used to hold the Alert information between the time
//    it enters the Alert Service and the time that the Alerts are
//    processed.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTQ/VCS/alertq.cpv  $
// 
//    Rev 1.2   09 Jun 1997 01:45:52   DSACKIN
// We were deleting objects we weren't responsible for.
//
//    Rev 1.1   18 May 1997 04:23:10   DSACKIN
// Changed from CAlertNotification to CVirusAlert.
//
//    Rev 1.0   24 Mar 1997 16:37:02   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
//
//    Rev 1.1   21 Jan 1997 13:52:22   TSmith
// Modified 'AlertQueueManagerWorkerThread' so that instantion of the
// 'CAlertNotification object' is at top of queue processing loop so a fresh
// object will be instantiated for each event.
//
//    Rev 1.0   18 Jan 1997 06:58:52   DSACKIN
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
// #include "alertsvc.h"
// #include "alertstr.h"

// #include "CMail.h"
// #include "CSMTP.h"

#include "QueueMgr.h"
#include "AlertQ.h"


/////////////////////////////////////////////////////////////////////
// Local function prototypes


/////////////////////////////////////////////////////////////////////
// CAlertWorkOrder member functions


/////////////////////////////////////////////////////////////////////
// CAlertWorkOrder::CAlertWorkOrder()
//
// Purpose:
//    This is the constructor for our workorder object.
//
// Parameters:
//   CVirusAlert   *lpcVirusAlert - Information about the Alert
//   CAlertTarget& *lpcTarget     - Where to send the alert
//
//
// Returns:
//   none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertWorkOrder::CAlertWorkOrder (
   CVirusAlert  *lpcVirusAlert,
   CAlertTarget *lpcTarget)
{
                                        // Pretty straight forward.  Just
                                        // assign the member variables.
   SetAlert  (lpcVirusAlert);
   SetTarget (lpcTarget);
}


/////////////////////////////////////////////////////////////////////
// void CAlertWorkOrder::SetAlert (
//
// Purpose:
//    Sets the internal Alert information
//
// Parameters:
//    CVirusAlert & cAlert - [in] Alert info to set
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertWorkOrder::SetAlert (
   CVirusAlert *lpcVirusAlert)
{
   m_lpcVirusAlert = lpcVirusAlert;
}



/////////////////////////////////////////////////////////////////////
// CVirusAlert * CAlertWorkOrder::GetAlert (
//
// Purpose:
//    Gets the Alert information out of the object
//
// Parameters:
//    none
//
// Returns:
//    CVirusAlert * - reference to the requested work order.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CVirusAlert * CAlertWorkOrder::GetAlert (void)
{
   return m_lpcVirusAlert;
}



/////////////////////////////////////////////////////////////////////
// void CAlertWorkOrder::SetTarget (
//
// Purpose:
//    Sets the target information in the current record
//
// Parameters:
//    CAlertTarget *lpcTarget - [in] - Target information to be assigned
//                                     to this record
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertWorkOrder::SetTarget (
   CAlertTarget *lpcTarget)
{
   m_lpcTarget = lpcTarget;
}



/////////////////////////////////////////////////////////////////////
// CAlertTarget * CAlertWorkOrder::GetTarget (
//
// Purpose:
//    Gets the target information from the current record
//
// Parameters:
//    none.
//
// Returns:
//    CAlertTarget * - reference to the requested target.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTarget * CAlertWorkOrder::GetTarget (void)
{
   return m_lpcTarget;
}




/////////////////////////////////////////////////////////////////////
// CAlertQueueManager member functions go here


/////////////////////////////////////////////////////////////////////
// CAlertQueueManager::CAlertQueueManager()
//
// Purpose:
//    Default constructor
//
// Parameters:
//    none
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertQueueManager::CAlertQueueManager()
{

                                        // What type of alerts are we going to
                                        // take?  This must be set using the
                                        // SetAlertType function
   m_dwAlertType = 0xffffffff;

                                        // In MOST cases, we are going to have
                                        // to require a target in order to let
                                        // the alert go through.
   m_bRequireTarget = TRUE;
}


/////////////////////////////////////////////////////////////////////
// CAlertQueueManager::~CAlertQueueManager()
//
// Purpose:
//    Default destructor
//
// Parameters:
//    none
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertQueueManager::~CAlertQueueManager()
{
}




/////////////////////////////////////////////////////////////////////
// BOOL CAlertQueueManager::AddAlert(
//
// Purpose:
//    Adds an alert to our queue
//
// Parameters:
//   CVirusAlert  *lpcVirusAlert - [in] - Information about the alert
//   CAlertTarget *lpcTarget     - [in] - Where to send it.
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertQueueManager::AddAlert(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
CAlertWorkOrder *lpNewWorkOrder;
BOOL             bReturn = TRUE;

   LogMessage ( _T("Receiving a new WorkOrder") );

   lpNewWorkOrder = new CAlertWorkOrder (
                           lpcVirusAlert,  // Alert information
                           lpcTarget);  // Where to send it

   if (NULL == lpNewWorkOrder)
   {
      LogMessage ( _T("Failed to allocate memory for the new WorkOrder") );

                                        // Set our failure state and abort.
      bReturn = FALSE;
      goto Cleanup;
   }

   bReturn = AddObject (lpNewWorkOrder);

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// VOID CAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This is a virtual function that will be overriden by the
//    individual Alert Queues.  This function does nothing with
//    the information.
//
// Parameters:
//   CVirusAlert  *lpcVirusAlert - [in] - Information about the alert
//   CAlertTarget *lpcTarget     - [in] - Where to send it.
//
// Returns:
//    none
//
// See Also:
//
// Note:
//    This function is responsible for cleaning up the lpcTarget
//    variable.  If it passes it on to another queue, that queue then
//    becomes responsible for deleting the object when it is done.
//    There is only one copy of any given CAlertTarget, and it lasts
//    from the creation to the time the alert is sent.
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////

VOID CAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{

                                        // Only delete it if it is not NULL
   if (lpcVirusAlert)
   {
      delete lpcVirusAlert;
      lpcVirusAlert = NULL;
   }

                                        // Only delete it if it is not NULL
   if (lpcTarget)
   {
      delete lpcTarget;
      lpcTarget = NULL;
   }

}



/////////////////////////////////////////////////////////////////////
// VOID CAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This is the function derived form the base class that splits
//    out the object given to us and sends it into the derived object
//
// Parameters:
//   CObject *lpcObject - [in] - Object to process.
//
// Returns:
//    none
//
// See Also:
//
// Note:
//    This function is responsible for cleaning up the object when
//    we are done with it.  because this is not a virtual function,
//    we will delete the object when we are done.  The virtual version
//    of this function that we are going to call is going to have to
//    be responsible for deleting the information in the object itself.
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CAlertQueueManager::ProcessWorkOrder(
   CObject *lpcObject)                  // [in] - Object to process.
{
CAlertWorkOrder  *lpcWorkOrder;
CVirusAlert      *lpcVirusAlert = NULL;
CAlertTarget     *lpcTarget = NULL;


   if (NULL == lpcObject)
   {
      LogMessage ( _T("Tried to process a NULL object.") );
      goto Cleanup;
   }

                                        // Start by type casting it to what we
                                        // need so we can call the member
                                        // functions
   lpcWorkOrder = (CAlertWorkOrder *) lpcObject;

                                        // Now that we have the work order,
                                        // time to send it to our other
                                        // processing function.

   lpcVirusAlert = lpcWorkOrder->GetAlert();
   lpcTarget     = lpcWorkOrder->GetTarget();

   if (NULL == lpcVirusAlert)
   {
      LogMessage ("*** Received a bogus alert!");
      goto Cleanup;
   }

   if (m_bRequireTarget &&
       (NULL == lpcTarget))
   {
      LogMessage ("*** Received a bogus target!");
      goto Cleanup;
   }


   ProcessWorkOrder(
      lpcVirusAlert,
      lpcTarget);

Cleanup:

                                        // Only clean up the Object we created
                                        // in this case.
   if (NULL != lpcObject)
   {
      delete lpcObject;
      lpcObject = NULL;
   }

   return;
}



/////////////////////////////////////////////////////////////////////
// BOOL CAlertQueueManager::Startup(void)
//
// Purpose:
//    This is a virtual function place holder that derived classes
//    are going to want to override if they have some special things
//    that need to be done on Startup.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - StartUp happened succesfully.
//    FALSE - Something didn't function correctly.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/17/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertQueueManager::StartUp(void)
{
   return TRUE;
}



/////////////////////////////////////////////////////////////////////
// BOOL CAlertQueueManager::ShutDown(void)
//
// Purpose:
//    This is a virtual function place holder that derived classes
//    are going to want to override if they have some special things
//    that need to be done on ShutDown
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - ShutDown was successful
//    FALSE - ShutDown was unsuccessful
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/17/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertQueueManager::ShutDown(void)
{
   return TRUE;
}

