/////////////////////////////////////////////////////////////////////
// MANAGERS.H - Created - 01/16/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/managers.h_v   1.1   18 May 1997 05:05:52   DSACKIN  $
//
// Description:
//    This file will contain the declarations of all of the different
//    managers that are derived from the CAlertQueueManager base
//    object (Except the Main Manager which is in MainMgr.h/MainMgr.cpp)
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/managers.h_v  $
// 
//    Rev 1.1   18 May 1997 05:05:52   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:28   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:29:56   DSACKIN
// Moved to a new AlertQueueManager.
//
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
//
//    Rev 1.1   23 Jan 1997 19:00:38   DSACKIN
// Now the CForwardAlertQueueManager needs to process the ShutDown function.
//
//    Rev 1.0   18 Jan 1997 07:02:34   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__MANAGERS_H)
#define __MANAGERS_H

#if !defined(__CALERT_H)
#   include "CAlert.h"
#endif

#if !defined(__CVALERT_H)
#   include "CVAlert.h"
#endif

/////////////////////////////////////////////////////////////////////
// Typedefs

                                        // This typedef is going to be used in
                                        // the RemotePopupManager
typedef NET_API_STATUS (WINAPI * LPNETMESSAGEBUFFERSEND_FCN) (
    LPTSTR  lpszServer,
    LPTSTR  lpszMsgName,
    LPTSTR  lpszFrom,
    LPBYTE  lpMsg,
    DWORD   dwBufLen
    );





/////////////////////////////////////////////////////////////////////
// CActiviayLogAlertQueueManager Class
class CActivityLogAlertQueueManager : public CAlertQueueManager
{
public:

   CActivityLogAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_ACTIVITY_LOG);

   };

   ~CActivityLogAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

};


/////////////////////////////////////////////////////////////////////
// CEventLogAlertQueueManager Class
class CEventLogAlertQueueManager : public CAlertQueueManager
{
private:
CService *m_lpService;

public:

   CEventLogAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_EVENTLOG);

   };

   ~CEventLogAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

                                        // The EventLog Function is a member of
                                        // CService.  We need a CService object
                                        // in order to write to the event log
   VOID SetService(
      CService *lpService)
   {
      m_lpService = lpService;
   }

};


/////////////////////////////////////////////////////////////////////
// CLocalPopupAlertQueueManager class
class CLocalPopupAlertQueueManager : public CAlertQueueManager
{
public:

   CLocalPopupAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_MESSAGE_LOCAL);

   };

   ~CLocalPopupAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

};


/////////////////////////////////////////////////////////////////////
// CRemotePopupAlertQueueManager class
class CRemotePopupAlertQueueManager : public CAlertQueueManager
{
private:

                                        // TCHAR ? don't compile the Alert
                                        // Service non-unicode
TCHAR                       m_szComputer[MAX_COMPUTERNAME_LENGTH + 1];

HINSTANCE                   m_hNetApi32;
LPNETMESSAGEBUFFERSEND_FCN  m_lpfnNetMessageBufferSend;

public:

   CRemotePopupAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_MESSAGE_NETWORK);

                                        // Initialize this so that we don't use
                                        // it if it isn't ready
      m_lpfnNetMessageBufferSend = NULL;

   };

                                        // Default Destructor
   ~CRemotePopupAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

                                        // Here are the overridden functions
                                        // that will allow us to initialize and
                                        // shut down properly
   BOOL StartUp(void);
   BOOL ShutDown(void);

};

/////////////////////////////////////////////////////////////////////
// CNavNlmAlertQueueManager class
class CNavNlmAlertQueueManager : public CAlertQueueManager
{
private:

public:

   CNavNlmAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_NLM);
   };

                                        // Default Destructor
   ~CNavNlmAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

                                        // This startup function starts the NLM
                                        // Notification Manager that is in the
                                        // NAVSERVE.DLL
   BOOL StartUp(void);

};


/////////////////////////////////////////////////////////////////////
// CForwardAddressProtocol and CForwardAlertQueueManager class
// work together to keep track of the last dynamic protocols that
// we have used.

class CForwardAddressProtocol : public CObject
{
private:
   CString            m_cszAddress;
   int                m_nProtocol;

public:
                                        // Constructor will take all three
                                        // arguments and set the internal
                                        // variables
   CForwardAddressProtocol (
      CString            cszAddress);

                                        // Default destructor
   ~CForwardAddressProtocol ( void ) { };


                                        // Utility functions for accessing
                                        // private member variables
   void SetAddress (
      CString cszAddress);

   void GetAddress (
      CString& cszAddress);

   void SetProtocol (
      int nProtocol);

   int GetProtocol (void);

};


#define MAX_DYNAMIC_STRING_LENGTH 20

class CForwardAlertQueueManager : public CAlertQueueManager
{
private:
TCHAR   m_szDynamic[MAX_DYNAMIC_STRING_LENGTH];  // "dynamic"
CObList m_oblAddressProtocol;           // To keep track of address/protocol pairs

public:

   CForwardAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_FORWARD);
   };

                                        // Default Destructor
   ~CForwardAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

                                        // The following StartUp and ShutDown
                                        // functions take care of getting the
                                        // Forward Manager ready to go, and
                                        // shutting him down when we are done.
   BOOL StartUp(void);
   BOOL ShutDown(void);


                                        // Utility functions for deciding how
                                        // to get the packets from here to there
   DWORD TryProtocol(
      CVirusAlert *lpcVirusAlert,       // [in] - Alert to send
      CString&     cszAddress,          // [in] - Where to send it
      CString&     cszProtocol);        // [in] - What protocol to try

   DWORD TryDynamic(
      CVirusAlert *lpcVirusAlert,       // [in] - Alert to send
      CString&     cszAddress);         // [in] - Where to send it


};

/////////////////////////////////////////////////////////////////////
// CSMTPAlertQueueManager class
class CSMTPAlertQueueManager : public CAlertQueueManager
{
private:

public:

   CSMTPAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_EMAIL);
   };

                                        // Default Destructor
   ~CSMTPAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

};


/////////////////////////////////////////////////////////////////////
// CSNMPAlertQueueManager class
class CSNMPAlertQueueManager : public CAlertQueueManager
{
private:

public:

   CSNMPAlertQueueManager()
   {
                                        // Set up what type of alerts we are
                                        // going to be processing
      SetAlertType(
         TARGET_SNMP);
   };

                                        // Default Destructor
   ~CSNMPAlertQueueManager() { };

   VOID ProcessWorkOrder(
      CVirusAlert  *lpcVirusAlert,      // [in] - Information about the alert
      CAlertTarget *lpcTarget);         // [in] - Where to send it.

};



#endif // if !defined(__MANAGERS_H)
