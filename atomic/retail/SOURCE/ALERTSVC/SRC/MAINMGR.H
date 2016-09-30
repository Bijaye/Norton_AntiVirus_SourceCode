/////////////////////////////////////////////////////////////////////
// MAINMGR.H - Created - 01/16/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/mainmgr.h_v   1.1   18 May 1997 05:05:50   DSACKIN  $
//
// Description:
//    Declaration file for the Main Queue manager object
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/mainmgr.h_v  $
// 
//    Rev 1.1   18 May 1997 05:05:50   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:24   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:28:20   DSACKIN
// Moved to a new AlertQueueManager.
//
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
//
//    Rev 1.0   18 Jan 1997 07:02:32   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__MAINMGR_H)
#define __MAINMGR_H

#if !defined(__CALERT_H)
#   include "CAlert.h"
#endif

#if !defined(__CVALERT_H)
#   include "CVAlert.h"
#endif

#if !defined(__ALERTQ_H)
#   include "AlertQ.h"
#endif // if !defined(__ALERTQ_H)


/////////////////////////////////////////////////////////////////////
// CManagerEntry - This will be used as a queue for our list of
// Managers that we are keeping track of

class CManagerEntry : public CObject
{
private:

   CAlertQueueManager *m_lpcAlertQueueManager;
   CString             m_szAlertQueueManagerName;

public:
                                        // Constructor will take the alert
   CManagerEntry (
      CAlertQueueManager *lpcAlertQueueManager,
      CString             lpszAlertQueueManagerName);

                                        // Default destructor
   ~CManagerEntry ( void ) { };

                                        // Utility functions for accessing
                                        // private member variables
   void SetAlertQueueManager (
      CAlertQueueManager *lpcAlertQueueManager);

   void GetAlertQueueManager (
      CAlertQueueManager **lpcAlertQueueManager);

   void SetAlertQueueManagerName (
      CString lpszAlertQueueManagerName);

   void GetAlertQueueManagerName (
      CString& lpszAlertQueueManagerName);

};

/////////////////////////////////////////////////////////////////////
// Declaration of the CMainAlertQueueManager object

class CMainAlertQueueManager : public CAlertQueueManager
{
private:
   CObList m_oblManagers;               // Here is our list of managers

public:

   CMainAlertQueueManager();
   ~CMainAlertQueueManager();


                                        // This function will be called by the
                                        // base class when we are starting up
   BOOL StartUp(void);
                                        // This function will be called by the
                                        // base class when we are shutting down
   BOOL ShutDown(void);

   VOID AddAlertQueueManager(
      CAlertQueueManager *lpcAlertQueueManager,  // [in] - Manager to add to our queue
      LPWSTR              lpszAlertQueueManagerName);  // [in] - Name of the manager to add

                                        // ProcessWorkOrder is where we are
                                        // going to distribute the incoming
                                        // alerts
   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcNotUsed);        // [in] - Where to send it.

                                        // This will be used to estimate the
                                        // time it will take us to shut down.
   DWORD GetNumberOfManagers(VOID)
   {
      return m_oblManagers.GetCount();
   };

};

#endif // if !defined(__MAINMGR_H)
