// Copyright 1996 Symantec Corporation, Peter Norton Computing Group
//*****************************************************************************
//
//	$Header:   S:/ALERTSVC/VCS/alertsvc.h_v   1.6   14 Aug 1997 18:22:42   DSACKIN  $
//
//	Description:
//
//*****************************************************************************
// $Log:   S:/ALERTSVC/VCS/alertsvc.h_v  $
// 
//    Rev 1.6   14 Aug 1997 18:22:42   DSACKIN
// Added functions for managing the RPC Server and NOSAP.DAT
//
//    Rev 1.5   09 Jun 1997 17:12:48   DSACKIN
// Only watch NavOpts.Dat and NavAlert.Dat
//
//    Rev 1.4   21 May 1997 13:42:40   DSACKIN
// Added a function to process Marconi alerts.
//
//    Rev 1.3   18 May 1997 05:05:34   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.2   07 May 1997 15:40:14   GDZIECI
// Added support for receiving ATP, IPX and IP alerts.
//
//    Rev 1.1   18 Apr 1997 13:40:42   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:24:46   DSACKIN
// Changed the prototype for a function.
//
//    Rev 1.0   13 Mar 1997 15:06:06   RFULLER
// Initial revision
//
//    Rev 1.16   05 Feb 1997 15:14:00   DSACKIN
// Moved RPC protocol definitions into RPCProt.h
//
//    Rev 1.15   28 Jan 1997 18:18:48   DSACKIN
// Added A new member function: ProcessCommand.
//
//    Rev 1.14   28 Jan 1997 04:10:12   DSACKIN
// Changed prototype for BuildVirusMessage to take a max length for the string.
//
//    Rev 1.13   18 Jan 1997 07:01:44   DSACKIN
// Changed the AlertSvc to use the CAlertQueueManager object for all alerting.
//
//    Rev 1.12   26 Dec 1996 15:08:38   TSmith
// Corrected ambiguous overloaded function declarations which were preventing a
// successful make for build 9.
//
//    Rev 1.11   22 Dec 1996 15:45:12   TSmith
// Changed declaration of 'ComposeActionString' to accept a pointer to a
// CString rather than a reference to facilitate the optional use of that
// argument.
//
//    Rev 1.10   19 Dec 1996 13:24:52   DSACKIN
// Moved m_svcMessenger here (See AlertSvc.CPP comment for more info)
//
//    Rev 1.9   27 Nov 1996 05:32:36   DSACKIN
// Added the definition monitor code from NAVAPSVC.
//
//    Rev 1.8   26 Nov 1996 07:19:58   DSACKIN
// Added a LocalPopupManager procedure
//
//    Rev 1.7   26 Nov 1996 06:41:40   DSACKIN
// Added SMTP configuration.
//
//    Rev 1.6   26 Nov 1996 06:22:52   DSACKIN
// Added dynamic protocol selection.
//
//    Rev 1.5   21 Nov 1996 01:28:22   DSACKIN
// Changed the EMailNotification function to InitializeEmailNotification
//
//    Rev 1.4   19 Nov 1996 14:57:00   DSACKIN
// Added a CObList for blocking circular packet routing.
//
//    Rev 1.3   18 Nov 1996 18:02:08   DSACKIN
// Implemented basic forwarding
//
//    Rev 1.2   18 Nov 1996 12:30:40   DSACKIN
// Fixed the NLM function prototype to take an extra parameter.
//
//    Rev 1.1   08 Nov 1996 17:18:52   TSMITH
// Added definitions for several notification and utility functions.
//
//    Rev 1.6   29 Jul 1996 00:56:40   DSACKIN
// Added two state booleans and one StartupAlertService function prototype.
//
//    Rev 1.5   23 Jul 1996 13:50:34   DSACKIN
// Changed many function prototypes to use RPC.
//
//    Rev 1.4   08 Jul 1996 14:24:14   DSACKIN
// Removed the Pause and Continue functions because they are no longer used.
//
//    Rev 1.3   06 Jun 1996 13:58:26   PVENKAT
// Added CheckMessenger function to start Messenger Service if not running.
//
//
//    Rev 1.2   16 Apr 1996 14:50:50   DSACKIN
// Cleaned it up a bit.
//
//    Rev 1.1   15 Apr 1996 15:21:28   DSACKING
// Finished work in getting alert service to work through CService.
//
//    Rev 1.0   29 Mar 1996 14:47:44   PVENKAT
// Initial revision.
//*****************************************************************************
#if !defined(__ALERTSVC_H)
#define __ALERTSVC_H

#if !defined(__SERVICE_H)
#   include "service.h"
#endif

// #if !defined(__ALRTNTFY_H)
// #   include "alrtntfy.h"
// #endif

#if !defined(__CALERT_H)
#   include "CAlert.h"
#endif

#if !defined(__CVALERT_H)
#   include "CVAlert.h"
#endif

#if !defined(_NAVUTIL_H)
#   include "navutil.h"
#endif

#if !defined(_RPCPROT_H)
#   include "rpcprot.h"
#endif

#if !defined(__NAVAP_LOG)
#   include "navaplog.h"
#endif

#if !defined(__CMAIL_H)
#   include "CMail.h"
#endif

#if !defined(__CSMTP_H)
#   include "CSMTP.H"
#endif

#if !defined(__QUEUEMGR_H)
#   include "QueueMgr.h"
#endif

#if !defined(__MAINMGR_H)
#   include "MainMgr.h"
#endif

#if !defined(__MANAGERS_H)
#   include "Managers.h"
#endif

#if !defined(__ALRTRECV_H)
#   include "alrtrecv.h"
#endif

#define PLAT_WIN31 (0x0000)
#define PLAT_WIN95 (0x0001)
#define PLAT_WINNT (0x0002)


/////////////////////////////////////////////////////////////////////
//  CAlertTarget class   (derived from CObject)

class CUUIDObject : public CObject
{
private:
                                        // UUID for this record
   UUID  m_UUID;

public:
                                        // Constructor and Destructor.
    CUUIDObject( UUID uuidNewUUID ) { m_UUID = uuidNewUUID; };
    ~CUUIDObject( void ) { };

                                        // Functions for accessing UUID
    UUID GetValue( void ) { return m_UUID; };
    void SetValue( UUID uuidNewUUID ) { m_UUID = uuidNewUUID; };
};


/////////////////////////////////////////////////////////////////////
// CAlertService

class CAlertService : public CService
{

private:

   CMainAlertQueueManager    cMainAlertQueueManager;

                                        // We now have an RPC requestor
   CLinkServer     m_RpcAlertServer;

                                        // This flag is set when we can
                                        // actually process incoming messages
   BOOL            m_bInitialized;

                                        // Set this flag if something failed to
                                        // start.
   BOOL            m_bStartupFailed;

   NAVOPTIONS      m_NavOptions;        // Notify NLM, Custom alert message

   CObList         m_UUIDHistory;       // signatures of packets we received

                                        // OK.  Here is where we should add the
                                        // file watch variables
   FILETIME        m_rNavOptsDat;
   FILETIME        m_rNavAlertDat;

                                        // used for virscan.dat and virscan2.dat
   TCHAR           m_szNAVPath[SYM_MAX_PATH];
                                        // "navopts.dat"
   TCHAR           m_szNavOptsDat[SYM_MAX_PATH];
                                        // "navalert.dat"
   TCHAR           m_szNavAlertDat[SYM_MAX_PATH];
                                        // Keep the current file times here
   BOOL            m_bPauseWatchingDefs;

   ATPReceiver    *m_pATPReceiver;

   IPXReceiver    *m_pIPXReceiver;

   IPReceiver     *m_pIPReceiver;

                                        // Is our RPC Server Listening?
   BOOL            m_bRPCServerListening;

                                        // Set this flag when the service is
                                        // shutting down
   BOOL            m_bStoppingService;
                                        // "nosap.dat"
   TCHAR           m_szNOSAPDat[SYM_MAX_PATH];
                                        // This variable is to remember if we
                                        // are SAPping or not.
   BOOL            m_bEnableSAP;

   CRITICAL_SECTION m_csRPCServer;      // Variable for our semaphore stuff

public:
   CAlertService(
      LPSERVICE_MAIN_FUNCTION pMain,    // Pointer to our Main callback
      LPHANDLER_FUNCTION pHandler       // Pointer to our Handler callback
      );

   ~CAlertService( );                   // Destructor

   VOID InitService( );                 // Where we report starting.
                                        // Where startup actually takes place
   VOID StartAlertService( );

                                        // This gets our queue manager running
   BOOL InitializeMainAlertQueueManager(VOID);

   VOID ServiceControlStop( );

   DWORD ProcessCommand(
      DWORD dwCommand);                 // [in] - Command from packet

   DWORD SendPacket(
      LPVOID pInPacket);                // [in] - Data packet from client

   DWORD ProcessCVirusAlert(
      CAlert *lpcAlert);                // [in] - Packet from the client.

   DWORD ProcessCAlertNotification(
      CAlert *lpcAlert);                // [in] - our old marconi style alert.

   VOID StartAlertReceivers();          // starts listening for ATP, IPX and
                                        // alerts if enabled
   VOID StopAlertReceivers();           // stops listening for ATP, IPX and
                                        // alerts

                                        // Use the following two functions for
                                        // the RPC Server Manager.
   BOOL CheckRPCServer(VOID);

private:

   BOOL GetNOSAPDatFilename(VOID);

   VOID LoadOptions( );                 // To find out how to behave

                                        // Function that will be used to make
                                        // sure we don't get duplicate messages.
   BOOL ValidateUUID(
      UUID uuidToCheck);

/////////////////////////////////////////////////////////////////////
// Definition update funtions

private:

   BOOL GetDefFileDates(
      LPFILETIME lprNavOptsDat,         // [out] - returns time of NAVOPTS.DAT
      LPFILETIME lprNavAlertDat);       // [out] - returns time of NAVALERT.DAT

   BOOL DefsNeedReLoading(VOID);

   BOOL StartDefinitionMonitor(VOID);

public:
                                        // Needs to be called from outside
                                        // of our object by DefinitionMonitorProc
   VOID DefinitionMonitor(VOID);


};


/////////////////////////////////////////////////////////////////////
// Enums
enum AlertException
{
   ErrLoadString,
   ErrMemAlloc
};


/////////////////////////////////////////////////////////////////////
// Function Prototypes

VOID GetResourceString(
   HINSTANCE hInst,                     // [in]  - Our process instance handle
   UINT uResId,                         // [in]  - ID of string to retrieve
   CString& szReturn);                  // [out] - Reference to a CString to fill

UINT ComposeActionString(
   DWORD    dwAlertType,                // [in]  - File, boot record or MBR infection
   DWORD    dwAction,                   // [in]  - What action was attempted
   BOOL     fSuccess,                   // [in]  - Were we successful
   CString* pszReturn);                 // [out] - Pointer to a CString object

VOID AppendCustomMessage(
   CString& szReturn);                  // [in] - String to append custom message to

VOID BuildVirusMessage(
   CVirusAlert * lpcVirusAlert,         // [in]  - Alert to parse
   CString&      szMessage,             // [out] - Message to use
   DWORD         dwMaxLength = 0xFFFFFFFF);   // [in]  - Max size to put in szMessage

WORD LogWrite(
   LPNAVAP_LOG lpNavApLog );            // [in] - Info to write to the log


#endif  // __ALERTSVC_H
