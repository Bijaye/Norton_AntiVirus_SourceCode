// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/apw95cmd.h_v   1.10   01 Jul 1998 14:35:52   CEATON  $
//
// Description:
//      Header for NAVAPW external commands W95 implementation.
//
//      These are all the commands which can be issued by the
//      UI whose implementation will change based on
//      platform or product.
//      These routines will be contained in APWCMD.DLL.
//      Install will copy the correct version based on platform.
//
// Contains:
//
// See Also:
//      APWCMD.H for command base class which is actually used by the UI.
//************************************************************************
// $Log:   S:/APWCMD/VCS/apw95cmd.h_v  $
// 
//    Rev 1.10   01 Jul 1998 14:35:52   CEATON
// Changed reference from CDefUtils to derived class CDefUtilsEx (supports Win32
// file notifications).
// 
// 
//    Rev 1.9   29 Jun 1998 22:19:58   RStanev
// Added new functionality to support asynchronous processing of tasks
// triggered by driver notifications. This is a workaround for a problem
// with RPC in Win9x.
//
//    Rev 1.8   28 May 1998 06:59:24   RStanev
// Reloading defs if the heuristic settings in navopts change.
//
//    Rev 1.7   21 Aug 1997 00:16:00   DALLEE
// Added SvcProcessSpooledEventProgress() and m_szConfigPath.
//
//    Rev 1.6   13 Aug 1997 12:39:30   DALLEE
// Added CApw95OptionsTime class to watch for changes to options files.
//
//    Rev 1.5   27 Jun 1997 19:28:16   DALLEE
// Added Navw notification functions.
// Regrouped some functions and member variables, added comments.
//
//    Rev 1.4   23 Jun 1997 20:32:06   DALLEE
// 1. Commented out old alert options. Don't presently need to track these.
// 2. Removed separate NLM and NT Forward alert functions.
//    Now happens with a single call into ProcessCVirusAlert() in NAVSERVE.
// 3. Added m_bSentNAVAPInit to remember whether we initialized the VxD
//    and whether or not we should deinitialize on exit.
//
//    Rev 1.3   11 Jun 1997 15:57:46   JBRENNA
// Add alerting functions and member variables.
//
//    Rev 1.2   06 Jun 1997 17:27:24   JBRENNA
// Integrate the Hawking (CDefUtils) functionality to pick up definitions
// from the shared directory.
//
//    Rev 1.1   05 Jun 1997 17:16:18   DALLEE
// Added activity log and alert options.
//
//    Rev 1.0   02 Jun 1997 03:41:36   DALLEE
// Initial revision.
//************************************************************************

#ifndef __APW95CMD_H
#define __APW95CMD_H

#ifndef __APWCMD_H
#include "apwcmd.h"
#endif

#ifndef __DEFUTILS_H__
#include "DefUtils.h"                   // CDefUtils class
#include "DefUtlEx.h"                   // CDefUtilsEx class
#endif

#include "pshpack1.h"
  #include "navbnhdr.h"
  #include "navopts.h"
  #include "options.h"
#include "poppack.h"


//************************************************************************
// class CApw95OptionsTime
//
// Class to watch times of NAV 95 options files.

class CApw95OptionsTime
{
public:
// Constructor
    CApw95OptionsTime();

// Operations
public:
    BOOL Init( LPCTSTR lpszOptionsDir );
    BOOL Update();
    BOOL operator==( const CApw95OptionsTime &xTime );
    BOOL operator!=( const CApw95OptionsTime &xTime );
    CApw95OptionsTime & operator=( const CApw95OptionsTime &xTime );

// Attributes
public:

// Implementation
public:
    virtual ~CApw95OptionsTime();

protected:
    #define MAX_OPTIONS_FILE_LEN 12
    #define NUM_OPTIONS_FILES    6

    // Static array of options file names is defined in APW95DEF.H

    static  const LPCTSTR   m_alpszOptionsFiles [ NUM_OPTIONS_FILES ];

    struct tagOPTTIME
    {
        TCHAR       szFileName [ MAX_PATH ];
        FILETIME    ftFileTime;
    } m_aOptTimes [ NUM_OPTIONS_FILES ];
};


//************************************************************************
// class CApw95Cmd
//
// Win95 specific version of CApwCmd class.

class CApw95Cmd : public CApwCmd
{
public:
// Constructor
    CApw95Cmd();

// Operations
public:
    // CApwCmd methods used by AP Agent UI.

    virtual BOOL    Init( VOID );
    virtual BOOL    Init( HWND hParent );
    virtual BOOL    DeInit( VOID );

    virtual BOOL     Options( HWND hParent );
    virtual BOOL     StartDriver( VOID );
    virtual BOOL     StopDriver( VOID );
    virtual BOOL     WaitForStartupPending( VOID );
    virtual BOOL     EnableDriver( VOID );
    virtual BOOL     DisableDriver( VOID );
    virtual AP_STATE GetStatusOfDriver( VOID );
    virtual BOOL     ShutdownScan( HWND hParent );
    virtual VOID     ConfirmApwComTasksReceived( VOID );

    // end CApwCmd methods.

// Attributes
public:

    HWND        hComInterfaceWnd;
    LONG        lComInterfaceMsgSent;

// Implementation
public:
    virtual ~CApw95Cmd();

    VOID SvcCallback ( DWORD    dwServiceCode,
                       PVOID    pvInBuffer,
                       DWORD    dwInBufferSize,
                       PVOID    pvOutBuffer,
                       DWORD    dwOutBufferSize,
                       PDWORD   pdwBytesReturned );

    DWORD SvcHandleRequestCommand ( PCMDPACKET lpCommandPacket );
    VOID SvcDefinitionMonitor();

protected:
    TCHAR m_szConfigPath [ MAX_PATH ];

    // For NAVAP driver communication.

    BOOL SvcRegisterCallback();
    VOID SvcHandleEventProgress ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo );
    BOOL SvcProcessSpooledEventProgress();

    DriverComm  *m_pNAVAP;
    BOOL        m_bNAVAPLoaded;
    BOOL        m_bNAVAPInitialized;
    BOOL        m_bSentNAVAPInit;
    BOOL        m_bNAVAPCallback;

    // Logging

    BOOL SvcLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo );
    BOOL SvcShouldLogEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo );
    BOOL SvcGetLogIDs ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo,
                        LPBYTE lpbyALogID,
                        LPUINT lpuMessageID,
                        LPUINT lpuActionID );
    VOID SvcGetEventLogInfo ( NAVAP_EVENTPROGRESSREPORTPACKET   *pEventInfo,
                              LPTSTR    szUser,
                              WORD      wMaxUserChars,
                              LPTSTR    szVirus,
                              WORD      wMaxVirusChars,
                              LPTSTR    szFilename,
                              WORD      wMaxFilenameChars,
                              LPTSTR    szAuxFilename,
                              WORD      wMaxAuxFilenameChars);

    BOOL m_bLogKnown;
    BOOL m_bLogUnknown;
    BOOL m_bLogInocChange;
    BOOL m_bLogVirusLike;
    BOOL m_bLogStartEnd;
    BOOL m_bLogVirusList;
    BOOL m_bLogQuarantine;
    BOOL m_bLogKeepUpTo;
    UINT m_uLogKiloBytes;
    BOOL m_bEnableHeuristicScan;
    UINT m_uHeuristicLevel;

    TCHAR m_szLogFile [ MAX_PATH ];

    // Alerting

    BOOL SvcAlertEvent ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo );
    BOOL SvcAlertShouldSend ( NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo );

    // Used to have to track alert options.
    // Now calling ProcessCVirusAlert() which reads options and decides what
    // to send and where to send it.
    // However, doesn't seem quite legit to read/query options there.

    // BOOL  m_bAlertNLM;
    // BOOL  m_bAlertNtSvc;
    // BOOL  m_bAlertNetUser;
    // BOOL  m_bAlertNetCon;
    // BOOL  m_bAlertKnown;
    // BOOL  m_bAlertUnknown;
    // BOOL  m_bAlertInocChange;
    // BOOL  m_bAlertVirusLike;
    // BOOL  m_bAlertStartEnd;
    // BOOL  m_bAlertVirusList;
    // TCHAR m_szAlertNtSvcServer[ MAX_PATH ];

    // Service emulation.

    CLinkServer m_SvcRequestServer;     // RPC server for AP Service requests.

    BOOL SvcReloadApConfig();
    BOOL SvcReloadApEngine();
    BOOL SvcLoadSvcOptions();
    BOOL SvcDefsSyncAppIds();
    BOOL SvcDefsNeedReLoading();
    VOID SvcUpdateDefDateInRegistry();
    BOOL SvcStartDefinitionMonitor();
    BOOL SvcOptionsNeedReLoading();

    // Navw notification.

    BOOL SvcNotifyNavw ( DWORD dwMessage );

    // Definitions monitor.

    CDefUtilsEx m_DefUtilsAppId1;
    CDefUtilsEx m_DefUtilsAppId2;
    BOOL      m_bDefMonitorExit;

    // Options monitor.

    CApw95OptionsTime m_OptionsTime;
};


//************************************************************************
// Win95 AP Service substitute info

#ifdef __cplusplus
extern "C" {
#endif

// Need non-class function for callback - just pass through to CApw95Cmd.

DWORD SvcHandleRequestCommand ( LPVOID lpCommandPacket );

// Need non-class function for start address for async threads.

VOID __cdecl SvcAsyncNotifyNavw ( LPVOID dwMessage );

#ifdef __cplusplus
}
#endif

#define  MAX_MESSAGE   1024
#define  MAX_ACTION    1024
#define  MAX_USER      80
#define  MAX_VIRUS     256
#define  MAX_APP_NAME  80

#endif // __APW95CMD_H
