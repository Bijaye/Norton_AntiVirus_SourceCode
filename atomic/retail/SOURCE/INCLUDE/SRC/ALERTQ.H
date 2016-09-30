/////////////////////////////////////////////////////////////////////
// QUEUEMGR.H - Created - 01/15/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/alertq.h_v   1.1   18 May 1997 04:23:22   DSACKIN  $
//
// Description:
//    This is where we will create our Queue manager class that will
//    be used to handle the incoming alerts between the time they
//    arive and the time they are processed.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/alertq.h_v  $
// 
//    Rev 1.1   18 May 1997 04:23:22   DSACKIN
// Changed from CAlertNotification to CVirusAlert
//
//    Rev 1.0   24 Apr 1997 20:52:18   JBRENNA
// Initial revision.
//
//    Rev 1.0   24 Mar 1997 16:38:12   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
//
//    Rev 1.0   18 Jan 1997 07:02:36   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__ALERTQ_H)
#define __ALERTQ_H

#if !defined(__QUEUEMGR_H)
#   include "queuemgr.h"
#endif // if !defined(__QUEUEMGR_H)

#if !defined(__CALERT_H)
#   include "CAlert.h"
#endif

#if !defined(__CVALERT_H)
#   include "CVAlert.h"
#endif

// #if !defined(__ALRTNTFY_H)
// #   include "alrtntfy.h"
// #endif

#if !defined(__AT_OPTS_H)
#   include "at_opts.h"
#endif



/////////////////////////////////////////////////////////////////////
// CAlertWorkOrder declaration.  This work order contains the alert
// information and the place that it needs to be sent to.  The Queue
// Manager will take this information one at a time and send out the
// alerts.  This is being derived from the CObject class so that it
// can be used in a MFC object list.

class CAlertWorkOrder : public CObject
{
private:

   CVirusAlert  *m_lpcVirusAlert;
   CAlertTarget *m_lpcTarget;

public:
                                        // Constructor will take the alert
                                        // and the Target
   CAlertWorkOrder (
      CVirusAlert  *lpcAlert,
      CAlertTarget *lpcTarget);

                                        // Default destructor
   ~CAlertWorkOrder ( void ) { };


                                        // Utility functions for accessing
                                        // private member variables
   void SetAlert (
      CVirusAlert *lpcAlert);

   CVirusAlert * GetAlert (void);

   void SetTarget (
      CAlertTarget *lpcTarget);

   CAlertTarget * GetTarget (void);

};


class CAlertQueueManager : public CQueueManager
{
private:
   DWORD   m_dwAlertType;               // What alert type am I going to process?
   BOOL    m_bRequireTarget;            // Do we need the target in this queue?

public:

   CAlertQueueManager();
   ~CAlertQueueManager();

                                        // This can be overridden by derived
                                        // class for their initialization needs
   virtual BOOL StartUp(void);
                                        // This can be overridden by a derived
                                        // class to handle their special
                                        // shutdown tasks
   virtual BOOL ShutDown(void);

   BOOL AddAlert(
      CVirusAlert  *lpcAlert,           // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

                                        // This function is virtual so that
                                        // derived classes can put in their own
                                        // processing functions.
   virtual VOID ProcessWorkOrder(
      CVirusAlert  *lpcAlert,           // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

                                        // This function is our version of the
                                        // ProcessWorkOrder function that
                                        // takes the CObject and calls
                                        // The ProcessWorkOrder that takes
                                        // the Notification and Target objects
   VOID ProcessWorkOrder(
      CObject             *lpcObject);  // [in] - Object to projess

                                        // Use the next two functions if this
                                        // manager is going to be aware of any
                                        // specific type of alert ID
   VOID SetAlertType(
      DWORD dwAlertType)
   {
      m_dwAlertType = dwAlertType;
   };

   DWORD GetAlertType(VOID)
   {
      return m_dwAlertType;
   };

                                        // The main alert queue manager is not
                                        // going to want to require targets
                                        // because it is going to be matching
                                        // the alerts with the targets.
   VOID SetRequireTarget(
      BOOL bRequireTarget)
   {
      m_bRequireTarget = bRequireTarget;
   };

   BOOL GetRequireTarget(VOID)
   {
      return m_bRequireTarget;
   };

};



#endif // if !defined(__ALERTQ_H)
