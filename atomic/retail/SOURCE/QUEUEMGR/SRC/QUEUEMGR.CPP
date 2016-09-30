/////////////////////////////////////////////////////////////////////
// QUEUEMGR.CPP - Created - 01/15/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/QUEUEMGR/VCS/queuemgr.cpv   1.0   18 Apr 1997 14:05:12   DSACKIN  $
//
// Description:
//    Here is the implementation file for the Queue Manager class
//    that is used to hold the Alert information between the time
//    it enters the Alert Service and the time that the Alerts are
//    processed.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/QUEUEMGR/VCS/queuemgr.cpv  $
// 
//    Rev 1.0   18 Apr 1997 14:05:12   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   24 Mar 1997 16:45:58   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
//
//    Rev 1.1   21 Jan 1997 13:52:22   TSmith
// Modified 'QueueManagerWorkerThread' so that instantion of the
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

#include "QueueMgr.h"


/////////////////////////////////////////////////////////////////////
// Local function prototypes

DWORD ObjectQueueManagerWorkerThreadProc(
   LPVOID lpInfo);                      // [in] - this pointer to our object


/////////////////////////////////////////////////////////////////////
// CAlertWorkOrder member functions


// /////////////////////////////////////////////////////////////////////
// // CAlertWorkOrder::CAlertWorkOrder()
// //
// // Purpose:
// //    This is the constructor for our workorder object.
// //
// // Parameters:
// //   CAlertNotification& cAlert  - Information about the Alert
// //   CAlertTarget&       cTarget - Where to send the alert
// //
// //
// // Returns:
// //   none
// //
// // See Also:
// //
// //
// /////////////////////////////////////////////////////////////////////
// // 01/15/97 - DSACKIN - Function Created.
// /////////////////////////////////////////////////////////////////////
// CAlertWorkOrder::CAlertWorkOrder (
//    CAlertNotification&  cAlert,
//    CAlertTarget        *lpcTarget)
// {
//                                         // Pretty straight forward.  Just
//                                         // assign the member variables.
//    SetAlert  (cAlert);
//    SetTarget (lpcTarget);
// }
//
//
// /////////////////////////////////////////////////////////////////////
// // void CAlertWorkOrder::SetAlert (
// //
// // Purpose:
// //    Sets the internal Alert information
// //
// // Parameters:
// //    CAlertNotification& cAlert - [in] Alert info to set
// //
// // Returns:
// //    none
// //
// // See Also:
// //
// //
// /////////////////////////////////////////////////////////////////////
// // 01/15/97 - DSACKIN - Function Created.
// /////////////////////////////////////////////////////////////////////
// void CAlertWorkOrder::SetAlert (
//    CAlertNotification& cAlert)
// {
//    m_cAlert = cAlert;
// }
//
//
//
// /////////////////////////////////////////////////////////////////////
// // void CAlertWorkOrder::GetAlert (
// //
// // Purpose:
// //    Gets the Alert information out of the object
// //
// // Parameters:
// //    CAlertNotification& cAlert - [out] - Alert information
// //
// // Returns:
// //    none
// //
// // See Also:
// //
// //
// /////////////////////////////////////////////////////////////////////
// // 01/15/97 - DSACKIN - Function Created.
// /////////////////////////////////////////////////////////////////////
// void CAlertWorkOrder::GetAlert (
//    CAlertNotification& cAlert)
// {
//    cAlert = m_cAlert;
// }
//
//
//
// /////////////////////////////////////////////////////////////////////
// // void CAlertWorkOrder::SetTarget (
// //
// // Purpose:
// //    Sets the target information in the current record
// //
// // Parameters:
// //    CAlertTarget& cTarget - [in] - Target information to be assigned
// //                                   to this record
// //
// // Returns:
// //    none
// //
// // See Also:
// //
// //
// /////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
// /////////////////////////////////////////////////////////////////////
// void CAlertWorkOrder::SetTarget (
//    CAlertTarget *lpcTarget)
// {
//    m_lpcTarget = lpcTarget;
// }
//
//
//
// /////////////////////////////////////////////////////////////////////
// // void CAlertWorkOrder::GetTarget (
// //
// // Purpose:
// //    Gets the target information from the current record
// //
// // Parameters:
// //    CAlertTarget& cTarget - [out] - Target information associated
// //                                    with this record
// //
// // Returns:
// //    none.
// //
// // See Also:
// //
// //
// /////////////////////////////////////////////////////////////////////
// // 01/15/97 - DSACKIN - Function Created.
// /////////////////////////////////////////////////////////////////////
// void CAlertWorkOrder::GetTarget (
//    CAlertTarget **lpcTarget)
// {
//    *lpcTarget = m_lpcTarget;
// }
//



/////////////////////////////////////////////////////////////////////
// CQueueManager member functions go here


/////////////////////////////////////////////////////////////////////
// CQueueManager::CQueueManager()
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
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
CQueueManager::CQueueManager()
{

                                        // Initialize the member variables
   m_hEvent              = NULL;
   m_hWorkThread         = NULL;
                                        // This flag is for signaling to the
                                        // worker thread that we are leaving
                                        // the management object
   m_bExiting = FALSE;

                                        // Start the Critical Task flag at
                                        // FALSE.  This is going to be used by
                                        // the worker thread to request that it
                                        // not be killed abruptly.  (It will be
                                        // given a little time to complete its
                                        // task before being kicked out)
   m_bCriticalTask = FALSE;

                                        // Set by StopManager when the thread
                                        // terminates.
   m_bThreadTerminated = FALSE;

                                        // Make sure there is nothing in this
                                        // string.
   m_szQueueManagerName[0] = EOS;
}


/////////////////////////////////////////////////////////////////////
// CQueueManager::~CQueueManager()
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
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
CQueueManager::~CQueueManager()
{

                                        // If the Worker Thread is still
                                        // active, we need to get rid of him.
   if (!m_bThreadTerminated)
   {
                                        // This will take him out one way or
                                        // another
      StopManager();
   }

                                        // Clean up our event handle
   if (NULL != m_hEvent)
   {
      CloseHandle(
         m_hEvent);

      m_hEvent = NULL;
   }

                                        // Clean up the handle to our thread
   if (NULL != m_hWorkThread)
   {
      CloseHandle(
         m_hWorkThread);

      m_hWorkThread = NULL;
   }
}




/////////////////////////////////////////////////////////////////////
// BOOL CQueueManager::Initialize()
//
// Purpose:
//    Starts this queue functioning
//
// Parameters:
//    LPCWSTR lpszQueueManagerName - [in] - Name of our QueueManager
//
// Returns:
//    TRUE  - Startup was successful
//    FALSE - Something failed to initialize.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
#define WORK_THREAD_STACK_SIZE  (4096)  // Work Thread Stack Size
BOOL CQueueManager::Initialize(
   LPCWSTR lpszQueueManagerName)   // [in] - Name of our QueueManager
{
BOOL bReturn = TRUE;


                                        // First off, lets get the profiler up
                                        // and running so we can see what is
                                        // going on.
   InitializeProfiler(
      lpszQueueManagerName);

                                        // Give the derived class a chance to
                                        // initialize before we start the
                                        // WorkThread going.
   bReturn = StartUp();

   if (FALSE == bReturn)
   {
      LogMessage ( _T("Call to StartUp() failed") );
      goto Cleanup;
   }
                                        // Start by creating our event
                                        // that will be used to signal
                                        // when something comes in

   m_hEvent = CreateEvent (
                 NULL,                  // Pointer to security attributes
                 FALSE,                 // flag for manual-reset of event
                 FALSE,                 // flag for initial state
                 NULL );                // Don't name the event

                                        // Check to make sure the event was
                                        // created successfully
   if (NULL == m_hEvent)
   {
      LogMessage (_T("CreateEvent failed."));

      bReturn = FALSE;
      goto Cleanup;
   }

   LogMessage (_T("Created Event successfully."));

                                        // Use CreateThread so that we will
                                        // have a valid ID returned
                                        // (_beginthread closes the handle for
                                        // us)
   m_hWorkThread = CreateThread(
                      NULL,             // Security ID
                      WORK_THREAD_STACK_SIZE, // Stack Size
                      (LPTHREAD_START_ROUTINE) ObjectQueueManagerWorkerThreadProc,  // Procedure to call
                      (LPVOID) this,    // Send in our this pointer
                      0,                // No creation flags (Start working immediately)
                      &m_dwWorkThreadID); // Get the thread ID that is returned

   if (NULL == m_hWorkThread)
   {
      LogMessage (_T("Failed to create the WorkThread."));
      bReturn = FALSE;
   }

   LogMessage (_T("WorkThread is up and running."));

Cleanup:

   if (!bReturn)
   {
      if (NULL != m_hEvent)
      {
         CloseHandle(
            m_hEvent);

         m_hEvent = NULL;
      }
   }

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CQueueManager::Initialize(
//
// Purpose:
//    This is the NON-UNICODE version of the Initialize function
//    that converts the name to UNICODE and calls the real Initialize
//    function.
//
// Parameters:
//   LPCSTR lpszQueueManagerName - [in] - Name of our QueueManager
//
//
// Returns:
//    TRUE  - Initialization was successful
//    FALSE - Something went wrong.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
BOOL CQueueManager::Initialize(
   LPCSTR lpszQueueManagerName)    // [in] - Name of our QueueManager
{
DWORD  dwReturn;
WCHAR  szQueueManagerNameW[SYM_MAX_PATH + 1];
BOOL   bReturn = FALSE;

   dwReturn = MultiByteToWideChar (
                 CP_ACP,
                 MB_PRECOMPOSED,
                 lpszQueueManagerName,
                 -1,
                 szQueueManagerNameW,
                 sizeof(szQueueManagerNameW) / sizeof(WCHAR) );

                                        // If we dwReturn is zero, just return
                                        // it to the caller.
   if (0 != dwReturn)
   {
                                        // Call the UNICODE version.
      bReturn = Initialize(
                   szQueueManagerNameW);
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// VOID CQueueManager::StopManager(VOID)
//
// Purpose:
//    Shuts down the CQueueManager.
//
// Parameters:
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
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////

#define MS_TO_WAIT_FOR_THREAD_TERMINATION     250
#define MAX_MS_TO_WAIT_FOR_THREAD_TERMINATION 1000

VOID CQueueManager::StopManager(VOID)
{
DWORD     dwReturn;
DWORD     dwElapsedTime = 0;
CObject  *lpcWorkOrder;

   LogMessage ( _T("Setting m_bExiting Flag") );
                                        // Start the shutdown process by
                                        // setting the exiting flag.
   m_bExiting = TRUE;

   LogMessage ( _T("Signaling Event for StopManager") );

                                        // Now signal to the queue manager to
                                        // wake up if it is asleep.
   SetEvent (m_hEvent);

                                        // Now give the thread the opportunity
                                        // to get out on its own.
   do
   {
      dwReturn = WaitForSingleObject(
                     m_hWorkThread,     // Thread Handle
                     MS_TO_WAIT_FOR_THREAD_TERMINATION);  // Time to wait

      dwElapsedTime += MS_TO_WAIT_FOR_THREAD_TERMINATION;

   }
   while ( (WAIT_OBJECT_0 != dwReturn) &&
           (m_bCriticalTask) &&
           (dwElapsedTime < MAX_MS_TO_WAIT_FOR_THREAD_TERMINATION) );

                                        // Did the thread actually leave?
   if (WAIT_OBJECT_0 != dwReturn)
   {
      LogMessage ( _T("Worker Thread did not exit - Calling TerminateThread()") );

                                        // If not, it is time to be rude and
                                        // kick him out.
      TerminateThread(
         m_hWorkThread,                 // Handle of the thread to terminate
         0);                            // Exit code

                                        // We aren't going to set the
                                        // m_hWorkThread to NULL yet becuase we
                                        // need to close our handle to it, and
                                        // this should happen in the destructor
                                        // of our class.

   }

                                        // Set the flag that our thread has
                                        // terminated because if StopManager has
                                        // been called explicitly, we don't
                                        // want to try to StopManager again in the
                                        // object's destructor
   m_bThreadTerminated = TRUE;

                                        // Now that we KNOW that the thread has
                                        // left the system, it is time to close
                                        // our handle to it.
   CloseHandle(
      m_hWorkThread);

                                        // Tidy up
   m_hWorkThread = NULL;

                                        // Now that the thread has been
                                        // terminated, it is time to remove all
                                        // of the unsent items from our queue.
   while (!m_oblWorkOrders.IsEmpty())
   {
                                        // Get the next order
      lpcWorkOrder = (CObject *) m_oblWorkOrders.RemoveTail();

                                        // And delete it
      delete lpcWorkOrder;
   }

                                        // Now give the derived class a chance
                                        // to clean up any information that
                                        // they need to before we delete them.
   ShutDown();

}


/////////////////////////////////////////////////////////////////////
// BOOL CQueueManager::AddObject(
//
// Purpose:
//    Adds an object to our queue
//
// Parameters:
//   CObject *lpcObject - [in] - Object to add to our queue.
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
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
BOOL CQueueManager::AddObject(
   CObject  *lpcObject)                 // [in] - Object to add.
{
BOOL             bReturn = TRUE;

   LogMessage ( _T("Receiving a new WorkOrder") );

                                        // Add it to the list so that our
                                        // processing function can handle it.
   m_oblWorkOrders.AddHead(
      lpcObject);

   LogMessage ( _T("Signaling WorkThread that there is a new WorkOrder") );

                                        // Now signal to the worker thread that
                                        // it is time to come look in the queue
   if (!SetEvent(
           m_hEvent))
   {
      LogMessage ( _T("Failed SetEvent to signal the WorkThread that there is a new WorkOrder") );

      bReturn = FALSE;
      goto Cleanup;
   }

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// VOID ObjectQueueManagerWorkerThreadProc(
//
// Purpose:
//    This is the pass through function for our worker thread procedure
//
// Parameters:
//    LPVOID - lpInfo - [in] - this pointer for our Manager procedure
//
// Returns:
//    doesn't return.  It calls _endthread() to terminate
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
DWORD ObjectQueueManagerWorkerThreadProc(
   LPVOID lpInfo)                       // [in] - this pointer to our object
{
CQueueManager *pThis;

                                        // Get our this pointer
   pThis = (CQueueManager *) lpInfo;

                                        // Call the worker thread
   pThis->ObjectQueueManagerWorkerThread();

                                        // Terminate our thread when done
   _endthread();

                                        // Dumb, but the compiler wants this to
                                        // keep us from having an error
   return 0;
}


/////////////////////////////////////////////////////////////////////
// VOID CQueueManager::ObjectQueueManagerWorkerThread(VOID)
//
// Purpose:
//    This is our worker thread that takes WorkOrder objects out of
//    our queue and processes them.
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
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
VOID CQueueManager::ObjectQueueManagerWorkerThread(VOID)
{
CObject *lpcObject;

                                        // Only process events until the
                                        // exiting manager flag is set
   while (!m_bExiting)
   {

      LogMessage( _T("Worker Thread going to sleep.") );

                                        // Wait until we have something to do
      WaitForSingleObject (
         m_hEvent,
         INFINITE);

      LogMessage( _T("Worker Thread waking up.") );

                                        // It is possible that the event has
                                        // been set to tell us to leave the
                                        // thread
      if (!m_bExiting)
      {

                                        // If we aren't exiting, it is time to
                                        // check our queue for events

         while ( !m_bExiting &&
                 !m_oblWorkOrders.IsEmpty() )
         {
            LogMessage( _T("Processing Next Work Order.") );

                                        // Get next work order from the queue
            lpcObject = (CObject *) m_oblWorkOrders.RemoveTail();

                                        // Now process!
            ProcessWorkOrder(
               lpcObject);

         }

      }
   }

   LogMessage( _T("Worker Thread is Terminating Normally.") );

}


/////////////////////////////////////////////////////////////////////
// VOID CQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This is a virtual function that will be overriden by the
//    individual Queues.  This function does nothing with
//    the information.
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
//    This function is responsible for cleaning up the lpcObject
//    variable.  If it passes it on to another queue, that queue then
//    becomes responsible for deleting the object when it is done.
//    There is only one copy of the given CObject at any given time, and
//    it lasts from the creation to the time the object is processed.
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - DSACKIN - Function Created.
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////

VOID CQueueManager::ProcessWorkOrder(
   CObject *lpcObject)                  // [in] - Object to process.
{

                                        // Only delete it if it is not NULL
   if (lpcObject)
   {
      delete lpcObject;
      lpcObject = NULL;
   }
}



/////////////////////////////////////////////////////////////////////
// BOOL CQueueManager::Startup(void)
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
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
BOOL CQueueManager::StartUp(void)
{
   return TRUE;
}



/////////////////////////////////////////////////////////////////////
// BOOL CQueueManager::ShutDown(void)
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
// 03/17/97 - DSACKIN - Queue CObjects now instead of CAlertObjects
/////////////////////////////////////////////////////////////////////
BOOL CQueueManager::ShutDown(void)
{
   return TRUE;
}

