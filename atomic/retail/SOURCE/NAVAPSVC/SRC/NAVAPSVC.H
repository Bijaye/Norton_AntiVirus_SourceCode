// Copyright 1996 Symantec Corporation
//***************************************************************************/
//
// $Header:   S:/NAVAPSVC/VCS/navapsvc.h_v   1.12   28 May 1998 05:59:22   RStanev  $
//
// Description:
// 	This source file contains routines for NAV Alert Service (for NT)
//
//***************************************************************************/
//      $Log:   S:/NAVAPSVC/VCS/navapsvc.h_v  $
//
//    Rev 1.12   28 May 1998 05:59:22   RStanev
// Added support for monitoring changes to longmsg.txt.
//
//    Rev 1.11   13 May 1998 16:01:44   DALLEE
// Added flag for indicating AP driver put items in Quarantine INCOMING
// folder and that the service's notification queue should complete the move
// to the legitimate quarantine folder.
//
//    Rev 1.10   17 Feb 1998 12:42:28   mdunn
// Ported NEC code from the ROSNEC codebase.
//
//    Rev 1.9   20 Nov 1997 10:06:48   mdunn
// Added a critical section object used when the AP prompting code launches
// NAVW.
//
//    Rev 1.8   17 Nov 1997 15:28:20   mdunn
// Put in Vance's changes (with a couple of my own tweaks) for AP prompting.
//
//    Rev 1.7   13 Jun 1997 10:23:42   JBRENNA
// Rename UpdateDefSvcImagePaths to UpdateDefServicePaths.
//
//    Rev 1.6   06 Jun 1997 11:25:52   JBRENNA
// 1. Remove obsolete memory variables: m_szDriversPath, m_szNAVEX,
//    m_szVirscanDat, and m_szVirscan2Dat.
// 2. Add new CDefUtil member variables: m_DefUtilsAppId1 and m_DefUtilsAppId2.
// 3. Update GetDefFileDates() prototype.
// 4. Add prototypes for new member functions: DefsNeedReLoading,
//    OptsNeedReLoading, DefsSyncAppIds, and UpdateDefSvcImagePaths.
//
//    Rev 1.5   03 Jun 1997 14:51:26   DSACKIN
// Changed the prototype for StartService to return a DWORD.
//
//    Rev 1.4   02 Jun 1997 19:24:46   RStanev
// Added support for new NAVAP driver.
//
//    Rev 1.3   16 May 1997 16:26:56   DSACKIN
// Switched from CAlertNotification to CVirusAlert
//
//    Rev 1.2   30 Apr 1997 12:24:44   RStanev
// Updated to match the new APCOMM functionality.
//
//    Rev 1.1   18 Apr 1997 13:59:06   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:12:14   RFULLER
// Initial revision
//
//    Rev 1.9   03 Feb 1997 19:38:32   JBELDEN
// removed prototype for IsPreviewOK and replaced it with
// IsTrialValid for use with symtrial.
//
//    Rev 1.8   27 Jan 1997 18:23:44   DSACKIN
// Added prototype for: UpdateDefDateInRegistry
//
//    Rev 1.7   22 Jan 1997 15:00:52   TSmith
// Added 'LPVOID pvInBuffer' argument to member function declarations
// 'ReportBootVirusEvent' and 'ScanBoot', defined MAX_RES_STR_LEN.
//
//    Rev 1.6   13 Jan 1997 22:25:12   DSACKIN
// Added code to make the NAVAP Service wait until the Alert Service has
// completed its startup before sending across the first alert.
//
//    Rev 1.5   24 Dec 1996 18:54:14   DSACKIN
// Added a new class for our object list.
//
//    Rev 1.4   24 Dec 1996 16:46:18   DSACKIN
// New member variable (CriticalSection)
//
//    Rev 1.3   27 Nov 1996 04:06:04   DSACKIN
// Fixed an error in a function declaration.
//
//    Rev 1.2   27 Nov 1996 03:47:34   DSACKIN
// Added member variables to hold new file names.
//
//    Rev 1.1   08 Nov 1996 17:44:30   TSMITH
// Removed definitions for alerting & logging functionality.
//
//    Rev 1.32   08 Sep 1996 15:40:36   DSACKIN
// Added IsPreviewOK function prototype for NAVWNT_PREVIEW version.
//
//    Rev 1.31   30 Aug 1996 03:15:42   DSACKIN
// Changed m_bUpdatingVirusList to m_bPauseWatchingDefs
//
//    Rev 1.30   28 Aug 1996 17:26:30   DSACKIN
// Added a new member variable of m_bUpdatingVirusList.
//
//    Rev 1.29   22 Aug 1996 00:42:26   DSACKIN
// Added our Definition Monitor functions.
//
//    Rev 1.28   04 Aug 1996 22:55:26   DSACKIN
// Removed obsolete variables and added WaitForStartup prototype.
//
//    Rev 1.27   28 Jul 1996 10:37:50   PVENKAT
// Added new function to get user/domain name from registry.
//
//    Rev 1.26   25 Jul 1996 21:26:42   DSACKIN
// Removed GetAlertPipeName.
//
//    Rev 1.25   19 Jul 1996 01:00:40   DSACKIN
// Moved from NamedPipes to RPC for communications.
//
//    Rev 1.24   16 Jul 1996 01:35:58   DSACKIN
// Added a prototype for our alert NAVW we are starting function.
//
//    Rev 1.23   12 Jul 1996 19:48:02   pvenkat
// Changed NAVAP_LOGPACKET and NAVAP_ALERTPACKET to NAVAP_INFOPACKET.
//
//    Rev 1.22   28 Jun 1996 16:00:52   DSACKIN
// Changed the ToBroadcast to ONLY take a string and not size or pipe.
//
//    Rev 1.21   24 Jun 1996 23:07:10   DSACKIN
// Added ReportBootVirusEvent prototype.
//
//    Rev 1.20   18 Jun 1996 15:40:52   DSACKIN
// Reflected the changes for our new packet sending algorithm.
//
//    Rev 1.19   13 Jun 1996 15:17:18   DSACKIN
// Include for service.h should have "" instead of <> around it.
//
//    Rev 1.18   06 Jun 1996 14:32:02   PVENKAT
// NotifyNavNLM is called for Alert and not for Log. (and uses PNAVAP_ALERT*)
//
//    Rev 1.17   05 Jun 1996 22:38:02   DSACKIN
// Added StartupScan member function and m_bExitingService variable.
//
//    Rev 1.16   29 May 1996 13:20:30   DSACKIN
// Added a Critical Section variable.
//
//    Rev 1.15   23 May 1996 13:07:54   DSACKIN
// Added prototypes for new shutdown function and new startup scan function.
//
//    Rev 1.14   21 May 1996 15:44:48   DSACKIN
// Changed the ScanBoot and RepairBoot prototypes.
//
//    Rev 1.13   17 May 1996 16:32:54   DSACKIN
// Added prototype for the CBKMount function.
//
//    Rev 1.12   16 May 1996 20:35:16   DSACKIN
// Added a couple of new functions.
//
//    Rev 1.11   16 May 1996 11:07:12   DSACKIN
// Added prototypes for new functions.
//
//    Rev 1.10   08 May 1996 20:22:20   DSACKIN
// Changed the StartService Prototype.
//
//    Rev 1.9   08 May 1996 20:01:44   DSACKIN
// Added new functions for getting and setting NAVAP status
//
//    Rev 1.8   03 May 1996 10:03:46   PVENKAT
// Removed APSVCSTR.H.
//
//    Rev 1.7   30 Apr 1996 15:21:02   PVENKAT
// No change.
//
//    Rev 1.6   30 Apr 1996 15:13:14   PVENKAT
//
//    Rev 1.5   29 Apr 1996 20:25:30   DSACKIN
// Added a m_hMutex variable.
//
//    Rev 1.4   26 Apr 1996 13:33:58   DSACKIN
// Added a function to reload the AP configuration.
//
//    Rev 1.3   25 Apr 1996 14:53:06   PVENKAT
// Added functions to log into NAV Log (Activity Log)
//
//    Rev 1.2   24 Apr 1996 20:48:24   DSACKIN
// Added CServiceControl objects to load the other devices/service
//
//    Rev 1.1   18 Apr 1996 12:58:02   PVENKAT
// CService Version.
//
//    Rev 1.0   29 Mar 1996 14:58:16   PVENKAT
// Initial revision.
//
//***************************************************************************/

#if !defined( __NAVAPSVC_H__ )
#define __NAVAPSVC_H__

#include <apcomm.h>                     // Auto-Protect Communication Layer
#include <drvcomm.h>                    // Driver Communication Layer

#if !defined( __SERVICE_H )
#   include "service.h"                 // Common Service Oriented class
#endif

#if !defined( __SVCCTRL_H )
#   include "svcctrl.h"
#endif

#if !defined( __APSCOMM_H )
#   include "apscomm.h"
#endif

#if !defined( __LINKSRV_H )
#   include "linksrv.h"                 // RPC server
#endif

#if !defined( __LINKCLI_H )
#   include "linkcli.h"                 // RPC client
#endif

#if !defined( __CVALERT_H )
#   include "CVAlert.h"
#endif

// #if !defined( __ALRTNTFY_H )
// #   include "alrtntfy.h"                // CAlertNotification class
// #endif

#if !defined( __DEFUTILS_H__ )
#   include "DefUtils.h"                // CDefUtils class
#endif

#define PLAT_WIN31      (0x0000)
#define PLAT_WIN95      (0x0001)
#define PLAT_WINNT      (0x0002)

#define NS_AP_DRIVER    (0x0001)
#define NS_BG_SCANNER   (0x0002)
#define NS_REMOTE       (0x0003)
#define NS_FG_SCANNER   (0x0004)

#define MAX_RES_STR_LEN 1024

// Added for new code to do NT workstation prompting
#define WNDD_SHAREMEM_DATA_NAME	"SYMCP4_WNDD_DATA_%lx"
#define WNDD_START_VALUE 999
#define NAVNT_COMMAND "NAVWNT.exe /B- /HMEM=%lx"
#define NAVNT_MODULE  "NAVWNT.exe"

/////////////////////////////////////////////////////////////////////
// CPacketItem - This will be used as a queue for our AP notifications
// as they come up from AP.

class CPacketItem : public CObject
{
private:

   LPVOID             m_lpPacket;
   DWORD              m_dwSize;         // Size of the Packet

public:
                                        // Constructor will take the alert
   CPacketItem (
      LPVOID lpPacket,
      DWORD  dwSize);

                                        // Default destructor
   ~CPacketItem ( );


                                        // Utility functions for accessing
                                        // private member variables
   void SetPacket (
      LPVOID lpPacket,
      DWORD  dwSize);

   void GetPacket (
      LPVOID *lppPacket,
      DWORD  *lpdwSize);

};


//************************
//	CNavAPService

class CNavAPService	: public CService
    {
private:
    CRITICAL_SECTION m_Critical;        // Variable for our semaphore stuff
                                        // We must not allow more than one RPC
                                        // call at a time from the asynchronous
                                        // callback (AP)
    CRITICAL_SECTION m_RpcCritical;

                                        // This critical section is used to
                                        // prevent us from launching multiple
                                        // instances of NAVW one after another
                                        // whilst doing AP prompting.  See
                                        // LaunchNavw() in navapcbk.cpp.
    CRITICAL_SECTION m_PromptingCritical;


                                        // Next 3 items track things to do in notification queue.
    CObList          m_oblPacketList;   // To keep track of what packets to send
	CObList          m_oblPromptList;   // List to send to NAVW for prompting
    BOOL             m_bIncomingQuarantineItems;    // Need to process incoming quarantine items.

    CServiceControl  m_SymEvent;        // Use these control objects to
    CServiceControl  m_NAVENG;          // talk to device drivers and
    CServiceControl  m_NAVEX;           // services.
    CServiceControl  m_NAVAP;
    CServiceControl  m_AlertService;

    CLinkServer      m_RequestServer;   // Use RPC for requestor now.

    NAVOPTIONS       m_NavOptions;      // Options selected by User.
    DriverComm*      m_pApDrvComm;

    FILETIME         m_rNAVEX;
    FILETIME         m_rVirscanDat;
    FILETIME         m_rVirscan2Dat;
    FILETIME         m_rNavOptsDat;
    FILETIME         m_rNavStartDat;
    FILETIME         m_rExcludeDat;
    FILETIME         m_rExcludeLDat;
    FILETIME         m_rNavAlertDat;
    FILETIME         m_rLongAlertTxt;

    HANDLE           m_hMutex;          // Used in reloading Options
    HANDLE           m_hNavStatMutex;   // Make sure not to de-init while scanning
    HANDLE           m_hStartupEvent;   // Event handle for startup

                                        // used for NavOpt.dat, NavStart.Dat, etc.
    TCHAR            m_szNAVPath[SYM_MAX_PATH];

                                        // "navopts.dat"
    TCHAR            m_szNavOptsDat[SYM_MAX_PATH];

                                        // "navstart.dat"
    TCHAR            m_szNavStartDat[SYM_MAX_PATH];

                                        // "exclude.dat"
    TCHAR            m_szExcludeDat[SYM_MAX_PATH];

                                        // "excludel.dat"
    TCHAR            m_szExcludeLDat[SYM_MAX_PATH];

                                        // "navalert.dat"
    TCHAR            m_szNavAlertDat[SYM_MAX_PATH];

                                        // "longmsg.txt"
    TCHAR            m_szLongAlertTxt[SYM_MAX_PATH];

                                        // Used to monitor Avenge Def incoming folder...
    TCHAR            m_szVirusDefFolder[SYM_MAX_PATH];

                                        // Keep the current file times here
    BOOL             m_bPauseWatchingDefs;

                                        // Is the Alert Service Ready?
    BOOL             m_bAlertSvcReady;

    BOOL             m_bDrvCommOpen;    // Flag for initialized DrvComm.
    BOOL             m_bExitingService;

    CDefUtils        m_DefUtilsAppId1;  // DefUtils class using Auto-Protect's 1st AppId
    CDefUtils        m_DefUtilsAppId2;  // DefUtils class using Auto-Protect's 2nd AppId


                                        // Hold the first floppy drive letter
                                        // here - it ain't 'A' for NEC
                                        // machines.  This variable is
                                        // initialized in StartupScan().
   TCHAR           m_cFirstFloppyDrive;

                                        // This DWORD is used as flags -
                                        // bit x is set if drive 'A'+x
                                        // is a floppy.  This is used in
                                        // CNavAPService::CBKMount() to
                                        // decide whether to scan a newly-
                                        // mounted drive.  This
                                        // variable is initialized in
                                        // RecordFloppyDrives, which is
                                        // called in StartupScan().
   DWORD           m_fdwFloppiesOnSystem;

public:
    CNavAPService(
        LPSERVICE_MAIN_FUNCTION pMain,  // Pointer to Main
        LPHANDLER_FUNCTION pHandler);   // Handler

    ~CNavAPService( );

    VOID InitService( VOID );
    VOID WorkerThread( VOID );
    VOID StartAPService( VOID );

    DWORD SendCommand(
        LPVOID pPacket                  // [in] - Packet from the client
        );

    VOID ServiceControlPause( VOID );
    VOID ServiceControlContinue( VOID );
    VOID ServiceControlStop( VOID );

    VOID ServiceControlUser(
        DWORD dwCtrlCode                // Custom control message
        );

    VOID GetAlertServer(
        LPTSTR lpszServer
        );

                                        // Called by our callback function
    BOOL  StopServices (VOID);          // Use to UnLoad autoprotect services

    BOOL  StopAPService (VOID);         // This function does all cleanup
                                        // and should only be called from
                                        // a separate thread from SCMgr message

                                        // This function takes care of our
                                        // Notifications from the NAVAP device
                                        // driver
    VOID NotificationQueue (VOID);
private:

#ifdef NAVWNT_PREVIEW
                                        // For Preview target.
   BOOL IsTrialValid(VOID);
#endif

   BOOL  StartServices (VOID);          // Use to load autoprotect services

   DWORD StartService (
      LPTSTR           lpszServiceName,    // Name to assign to service
      CServiceControl *lpServiceControl,   // Object to talk to
      BOOL             bLock = FALSE);     // Lock the object?

   BOOL  StopService  (
      CServiceControl *lpServiceControl);  // Object to talk to

   VOID InformNAVW (VOID);              // Tell NAVW when we start

   DWORD ProcessRequest(
      PCMDPACKET pPacket);              // [in] - Packet to process.

   DWORD WaitForStartup(VOID);          // NAVAPW will call this to wait
                                        // for our startup.

   DWORD CmdExecProgram(                // Used by the scheduler.
      PEXEDATA pExeData);               // Packet received in our pipe

   BOOL  ReLoadAutoProtect (VOID);      // Called when a user requests it
                                        // using ServiceControlUser

   DWORD SetApStatus(
      BOOL     bEnable);                // [in] - Command to process.

   DWORD GetApStatus(VOID);

/////////////////////////////////////////////////////////////////////
// Scanning functions

   VOID ReportBootVirusEvent (
      LPTSTR     lpszVirusName,         // [in] - Virus name to report
      UINT       uDrive,                // [in] - Drive we are talking about
      BOOL       bIsInfected,           // [in] - IS it infected or WAS it?
      BOOL       bAttemptedRepair,      // [in] - Did we try to repair?
      LPVOID     pvInBuffer = NULL);    // [in] - Optional buffer from driver

   BOOL ScanBoot (
      UINT      uDrive,                 // [in] - Drive letter to scan
      LPVOID    pvInBuffer,             // [in] - Optional buffer from driver
      BOOL      bTryRepair = TRUE);     // [in] - Try to repair it?

   BOOL RepairBoot (
      UINT uDrive,                      // [in] - Drive letter to repair
      UINT uVirusSigIndex);             // [in] - What is the Virus ID?

                                        // Startup scan must be public so
                                        // it can be called from _beginthread
public:
   VOID StartupScan(VOID);

   VOID Callback (
      ULONG  ulIoControlCode,           // [in] - Control code from Device
      LPVOID pvInBuffer,                // [in] - Input buffer
      UINT   uInBufferSize);            // [in] - Bytes in the Input buffer.

private:
/////////////////////////////////////////////////////////////////////
// Callback functions

    DWORD ConvertLastAPAction(
        PNAVAP_EVENTPROGRESSREPORTPACKET lpLog
        );

    VOID CBKDecodePacket(
        LPVOID pvInBuffer,              // [in] - buffer sent from driver
        DWORD  dwInBufferSize,          // [in] - size of the buffer
        CVirusAlert & cVirusAlert);     // [out] - reference to notification packet

    VOID CBKMount(
        LPVOID pvInBuffer,              // [in] - buffer sent from driver
        UINT   uInBufferSize            // [in] - size of the buffer
        );

/////////////////////////////////////////////////////////////////////
// Utility functions

   BOOL DecodeNAVAPUserInfoSubPacket (
      PNAVAP_USERINFOSUBPACKET prPacket,   // [in]  - Packet to decode
      PTSTR                    pszUser,    // [out] - User name
      PDWORD                   pcchUser,   // [in]  - size of username buffer
      PTSTR                    pszDomain,  // [out] - Domain name
      PDWORD                   pcchDomain  // [in]  - size of domain buffer
      );

   VOID ReLoadOptions (VOID);

   BOOL GetRegUserName (
      LPTSTR  lpszUser,                 // [out] - User Name from Registry
      LPDWORD lpdwULen,                 // [in]  - size of the buffer
      LPTSTR  lpszDomain,               // [out] - Default Domain Name
      LPDWORD lpdwDLen);                // [out] - size of the buffer

   void RecordFloppyDrives();

   BOOL LaunchNavw();                   // Launches Navw to do prompting

/////////////////////////////////////////////////////////////////////
// Definition update funtions

private:

   BOOL GetDefFileDates(
      LPFILETIME lprNavOptsDat,         // [out] - returns time of NAVOPTS.DAT
      LPFILETIME lprNavStartDat,        // [out] - returns time of NAVSTART.DAT
      LPFILETIME lprExcludeDat,         // [out] - returns time of EXCLUDE.DAT
      LPFILETIME lprExcludeLDat,        // [out] - returns time of EXCLUDEL.DAT
      LPFILETIME lprNavAlertDat,        // [out] - returns time of NAVALERT.DAT
      LPFILETIME lprLongAlertTxt);      // [out] - returns time of LONGMSG.TXT

   BOOL OptsNeedReLoading(void);
   BOOL DefsNeedReLoading(void);
   BOOL DefsSyncAppIds (void);
   BOOL UpdateDefServicePaths(void);

                                        // This is for when people just copy in
                                        // new defs
   void UpdateDefDateInRegistry(void);

public:
                                        // Needs to be called from outside
                                        // of our object by DefinitionMonitorProc
   VOID DefinitionMonitor(VOID);
   VOID OptionsMonitor(VOID);

private:

   BOOL StartDefinitionMonitor(VOID);


};

#endif  // __NAVAPSVC_H__
