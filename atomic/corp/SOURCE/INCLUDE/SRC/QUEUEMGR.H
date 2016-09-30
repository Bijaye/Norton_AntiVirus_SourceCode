/////////////////////////////////////////////////////////////////////
// QUEUEMGR.H - Created - 01/15/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/QUEUEMGR.H_v   1.0   24 Apr 1997 20:19:54   JBRENNA  $
//
// Description:
//    This is where we will create our Queue manager class that will
//    be used to handle the incoming alerts between the time they
//    arive and the time they are processed.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/QUEUEMGR.H_v  $
// 
//    Rev 1.0   24 Apr 1997 20:19:54   JBRENNA
// Initial revision.
// 
//    Rev 1.0   24 Mar 1997 16:49:44   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
//
//    Rev 1.0   18 Jan 1997 07:02:36   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__QUEUEMGR_H)
#define __QUEUEMGR_H

#if !defined(__TIMERCLI_H)
#   include "timercli.h"
#endif // if !defined(__TIMERCLI_H)

/////////////////////////////////////////////////////////////////////
// CQueueManager - This class will handle all of the routine
// queuing of work orders.

class CQueueManager : public CObject
{
private:
   HANDLE  m_hEvent;                    // Event handle for incoming notification
   CObList m_oblWorkOrders;             // Here is our WorkOrder queue

   BOOL    m_bExiting;                  // Signal to get out of our worker thread
   BOOL    m_bCriticalTask;             // Set by worker thread when doing something important
   BOOL    m_bThreadTerminated;         // Set when ShutDown has completed getting rid of the thread

   HANDLE  m_hWorkThread;               // Handle to our thread (Use to control it)
   DWORD   m_dwWorkThreadID;            // ID of our worker thread.

                                        // Now for our profiling variables
   WCHAR        m_szQueueManagerName[SYM_MAX_PATH + 1];
   CTimerClient m_cTimer;


public:

   CQueueManager();
   ~CQueueManager();

                                        // Call Initialize to start the queue
                                        // manager going.
   BOOL Initialize(
      LPCWSTR lpszQueueManagerName);    // [in] - Name of our QueueManager

                                        // Call Initialize to start the queue
                                        // manager going.
   BOOL Initialize(
      LPCSTR lpszQueueManagerName);     // [in] - Name of our QueueManager

                                        // Call ShutDown to get the worker
                                        // thread out of memory.  The Thread is
                                        // given some time, but will eventually
                                        // be terminated if it doesn't respond.
   VOID StopManager(VOID);

                                        // This can be overridden by derived
                                        // class for their initialization needs
   virtual BOOL StartUp(void);
                                        // This can be overridden by a derived
                                        // class to handle their special
                                        // shutdown tasks
   virtual BOOL ShutDown(void);

   BOOL AddObject(
      CObject            *lpcObject);   // [in] - Object to add to our queue.

                                        // This is our thread that takes care
                                        // of incoming objects
   VOID ObjectQueueManagerWorkerThread(VOID);

                                        // This function is virtual so that
                                        // derived classes can put in their own
                                        // processing functions.
   virtual VOID ProcessWorkOrder(
      CObject             *lpcObject);  // [in] - Object to projess

   VOID SetCriticalTask(
      BOOL bCriticalTask)               // [in] - Are we in a critical task?
   {
      m_bCriticalTask = bCriticalTask;
   };

                                        // Use this function to find out what
                                        // the queue is named.
   LPWSTR GetQueueName(void)
   {
      return m_szQueueManagerName;
   }


                                        // These are the profiling functions
   VOID InitializeProfiler(
      LPCTSTR lpszQueueManagerName);    // [in] - The name of the service
#ifdef UNICODE
   VOID InitializeProfiler(
      LPCSTR lpszQueueManagerName);     // [in] - The name of the service
#else // UNICODE
   VOID InitializeProfiler(
      LPCWSTR lpszQueueManagerName);    // [in] - The name of the service
#endif // UNICODE

   VOID LogMessage(
      LPTSTR lpszMessage);              // [in] - Message to send
#ifdef UNICODE
   VOID LogMessage(
      LPSTR lpszMessage);               // [in] - Message to send
#else // UNICODE
   VOID LogMessage(
      LPWSTR lpszMessage);              // [in] - Message to send
#endif // UNICODE

   VOID LogMessage(
      LPTSTR lpszMessage,               // [in] - Message to send
      LPTSTR lpszString);               // [in] - Support string for message
#ifdef UNICODE
   VOID LogMessage(
      LPSTR lpszMessage,                // [in] - Message to send
      LPSTR lpszString);                // [in] - Support string for message
#else // UNICODE
   VOID LogMessage(
      LPWSTR lpszMessage,               // [in] - Message to send
      LPWSTR lpszString);               // [in] - Support string for message
#endif // UNICODE

   VOID LogMessage(
      LPTSTR lpszMessage,               // [in] - Message to send
      DWORD  dwValue);                  // [in] - Support value for message
#ifdef UNICODE
   VOID LogMessage(
      LPSTR lpszMessage,                // [in] - Message to send
      DWORD  dwValue);                  // [in] - Support value for message
#else // UNICODE
   VOID LogMessage(
      LPWSTR lpszMessage,               // [in] - Message to send
      DWORD  dwValue);                  // [in] - Support value for message
#endif // UNICODE





};



#endif // if !defined(__QUEUEMGR_H)
