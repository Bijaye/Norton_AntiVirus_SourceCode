// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/Apw95svc.cpv   1.14   29 Jun 1998 22:21:00   RStanev  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/APWCMD/VCS/Apw95svc.cpv  $
// 
//    Rev 1.14   29 Jun 1998 22:21:00   RStanev
// Added new functionality to support asynchronous processing of tasks
// triggered by driver notifications. This is a workaround for a problem
// with RPC in Win9x.
//
//    Rev 1.13   09 Jun 1998 20:13:56   RStanev
// Process quarantine's incoming directory for every successful action by AP.
//
//    Rev 1.12   05 Jun 1998 14:09:48   RStanev
// More fixes for reloading defs when heuristic options change.
//
//    Rev 1.11   28 May 1998 07:00:18   RStanev
// Reloading defs if the heuristic settings in navopts change.
//
//    Rev 1.10   28 Apr 1998 09:04:42   DALLEE
// Added handling for NAVAPSVC_AP_DISABLE and NAVAPSVC_AP_ENABLE
// RPC commands.
//
//    Rev 1.9   27 Apr 1998 16:10:34   DALLEE
// On QuarantineEvents, process incoming quarantine items.
//
//    Rev 1.8   02 Apr 1998 12:40:04   DALLEE
// Enabled GetStatus RPC request handling. For communicating status
// to NAVW.
//
//    Rev 1.7   21 Aug 1997 00:19:26   DALLEE
// Added SvcProcessSpooledEventProgress() to process driver's spooled events.
// Update options date/time when reloading options.
//
//    Rev 1.6   11 Aug 1997 17:07:14   DALLEE
// Added call to ApwUpdateUI() when signaling driver to reload options or
// virus engine. State of driver may change on error and UI needs to
// reget and display the current state of the driver.
//
//    Rev 1.5   27 Jun 1997 19:36:04   DALLEE
// 1. Separated AP options reload and engine reload -- new function
//    SvcReloadApEngine() in addition to SvcReloadApConfig().
// 2. Moved NAVW notification here from UI piece. New function
//    SvcNotifyNavw() also starts separate thread for notification --
//    RPC time out is heinously long on W95.
//
//    Rev 1.4   23 Jun 1997 21:01:36   DALLEE
// Commented out old alert options. Not presently necessary since
// ProcessCVirusAlert() will read/query options on its own.
//
//    Rev 1.3   11 Jun 1997 15:53:16   JBRENNA
// 1. Have SvcHandleEventProgress start calling SvcAlertEvent.
// 2. Add m_bAlertNtSvc and m_szAlertNtSvcServer initialization in
//    SvcLoadSvcOptions.
//
//    Rev 1.2   05 Jun 1997 17:31:28   DALLEE
// Oops... g_hCmdInst to g_hInstance.
//
//    Rev 1.1   05 Jun 1997 17:26:02   DALLEE
// Moved logging code to apw95log.cpp.
// Added own representation for activity and alert options.
//
//    Rev 1.0   02 Jun 1997 03:42:56   DALLEE
// Initial revision.
//************************************************************************

#define STRICT

#include "platform.h"
#include <windowsx.h>
#include <tchar.h>
#include <process.h>

#include "drvcomm.h"
#include "apcomm.h"
#include "linksrv.h"
#include "apscomm.h"
#include "actions.h"

#include "apw.h"
#include "apwutil.h"
#include "apw95cmd.h"

#include "apw95res.h"
#include "apwres.h"

#define INITGUID
#include "iquaran.h"
#undef INITGUID

//************************************************************************
// External data
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

extern HINSTANCE    g_hInstance;
extern CApw95Cmd    *g_pCmd;

#ifdef __cplusplus
}
#endif


//************************************************************************
// Local prototypes
//************************************************************************

static VOID SvcCallback ( DWORD    dwServiceCode,
                          PVOID    pvInBuffer,
                          DWORD    dwInBufferSize,
                          PVOID    pvOutBuffer,
                          DWORD    dwOutBufferSize,
                          PDWORD   pdwBytesReturned );


//************************************************************************
// Win95 Service substitute implementation.
//************************************************************************

//************************************************************************
// NAVAP driver callback routines.
//************************************************************************

//************************************************************************
// CApw95Cmd::SvcRegisterCallback()
//
// BOOL CApw95Cmd::SvcRegisterCallback ()
//
// This routine registers this process as NAVAP's request handler.
// Should be called after successfully loading/opening driver.
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcRegisterCallback ()
{
    // Driver requests which will be handled by a separate thread.
    // List includes all current known NAVAP requests.

    static DWORD s_adwNAVAPRequests [] = { NAVAP_COMM_EVENTPROGRESSREPORT,
                                           NAVAP_COMM_SCANMOUNTEDDEVICE,
                                           NAVAP_COMM_SHUTDOWN,
                                           0 };

    SYM_ASSERT( m_pNAVAP );

    return ( m_pNAVAP->DesignateServiceThreads( s_adwNAVAPRequests,
                                                1,
                                                ::SvcCallback ) &&
             m_pNAVAP->RegisterServiceCallback( ::SvcCallback ) );
} // CApw95Cmd::SvcRegisterCallback()


//************************************************************************
// SvcCallback()
//
// Service callback implementation for communication with NAVAP driver.
// This routine merely passes the driver's request on to
// CApw95Cmd::SvcCallback()
//
// Returns:
//      Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

static VOID SvcCallback ( DWORD    dwServiceCode,
                          PVOID    pvInBuffer,
                          DWORD    dwInBufferSize,
                          PVOID    pvOutBuffer,
                          DWORD    dwOutBufferSize,
                          PDWORD   pdwBytesReturned )
{
    if ( g_pCmd )
    {
        g_pCmd->SvcCallback( dwServiceCode,
                             pvInBuffer,
                             dwInBufferSize,
                             pvOutBuffer,
                             dwOutBufferSize,
                             pdwBytesReturned );
    }
    else
    {
        SYM_ASSERT( FALSE );
    }
} // SvcCallback()


//************************************************************************
// CApw95Cmd::SvcCallback()
//
// VOID CApw95Cmd::SvcCallback (
//      DWORD    dwServiceCode,
//      PVOID    pvInBuffer,
//      DWORD    dwInBufferSize,
//      PVOID    pvOutBuffer,
//      DWORD    dwOutBufferSize,
//      PDWORD   pdwBytesReturned )
//
// Service callback implementation for communication with NAVAP driver.
// This routine fields all requests from the NAVAP driver.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApw95Cmd::SvcCallback ( DWORD    dwServiceCode,
                              PVOID    pvInBuffer,
                              DWORD    dwInBufferSize,
                              PVOID    pvOutBuffer,
                              DWORD    dwOutBufferSize,
                              PDWORD   pdwBytesReturned )
{
    switch( dwServiceCode )
    {
    case NAVAP_COMM_EVENTPROGRESSREPORT:

        SvcHandleEventProgress( ( NAVAP_EVENTPROGRESSREPORTPACKET * ) pvInBuffer );
        break;

    case NAVAP_COMM_SHUTDOWN:
        // Send delayed shutdown message to main thread.

        ApwShutdown();
        break;

    case NAVAP_COMM_SCANMOUNTEDDEVICE:
        // Should not receive this command on W95.

        SYM_ASSERT( FALSE );
        break;

    default:
        // We should probably explicitly handle all messages.
        // Assert here so new messages aren't ignored.

        SYM_ASSERT( FALSE );
        break;
    }
} // CApw95Cmd::SvcCallback()


//************************************************************************
// CApw95Cmd::SvcHandleEventProgress()
//
// BOOL CApw95Cmd::SvcHandleEventProgress (
//      NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
//
// This routine handles the NAVAP_COMM_EVENTPROGRESSREPORT service
// request from NAVAP.
// This request is sent by NAVAP after it has responded to an event.
// This routine is responsible for logging the event and sending
// any required alerts, and also processing any quarantine items sent
// to the Quarantine's INCOMING folder.
//
// Returns:
//      Nothing.
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

VOID CApw95Cmd::SvcHandleEventProgress (
        NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo )
{
    // Process the INCOMING items in Quarantine for any successful AP action.

    if ( pEventInfo->dwActionCompleted &&
         hComInterfaceWnd != NULL &&
         !InterlockedExchange ( &lComInterfaceMsgSent, 1 ) )
    {
        PostMessage ( hComInterfaceWnd, APWM_COM_TASKS, (WPARAM)0, (LPARAM)0 );
    }

    // Log event and send alert.

    SvcLogEvent( pEventInfo );
    SvcAlertEvent( pEventInfo );
} // CApw95Cmd::SvcHandleEventProgress()


//************************************************************************
// CApw95Cmd::SvcProcessSpooledEventProgress()
//
// BOOL CApw95Cmd::SvcProcessSpooledEventProgress ()
//
// This routine handles the NAVAP_COMM_EVENTPROGRESSREPORT packets
// for events which happened before the agent was loaded.
// NAVAP will have saved this to a spool file for us.
//
// File format is:  4 bytes     DWORD   dwPacketLength;
//                  ? bytes     NAVAP_EVENTPROGRESSREPORTPACKET rPacket;
//                  ...
//
// This routine is responsible for logging the events and sending
// any required alerts.
//
// Returns:
//      TRUE    on success (also if no log file).
//      FALSE   on error.
//************************************************************************
// 08/19/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcProcessSpooledEventProgress ( )
{
    // Crude rule of thumb for maximum event progress packet size in bytes.

    #define MAX_PACKET_SIZE     ( 20 * 1024 )

    // Locals

    auto    TCHAR   szSpoolFileName [ MAX_PATH ];   // Raw filename.
    auto    TCHAR   szSpoolFile [ MAX_PATH ];       // Full path.
    auto    HANDLE  hSpoolFile;

    auto    NAVAP_EVENTPROGRESSREPORTPACKET *pEventInfo;
    auto    DWORD   dwPacketSize;
    auto    DWORD   dwBytesRead;
    auto    DWORD   dwMaxStringIndex;

    auto    BOOL    bSuccess;

    // Initialize locals.

    hSpoolFile  = INVALID_HANDLE_VALUE;
    pEventInfo  = NULL;
    bSuccess    = FALSE;

    // Construct name of spool file.

    if ( 0 == ::LoadString( g_hInstance,
                            IDS_AP_SPOOL_FILE,
                            szSpoolFileName,
                            sizeof( szSpoolFileName ) / sizeof( TCHAR ) ) )
    {
        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    if ( ( ::_tcslen( m_szConfigPath ) + ::_tcslen( m_szConfigPath ) + 2 ) >
         ( sizeof( szSpoolFile ) / sizeof( TCHAR ) ) )
    {
        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    ::_tcscpy( szSpoolFile, m_szConfigPath );
    ::ApwNameCat( szSpoolFile, szSpoolFileName );

    // Open spool file if present.

    hSpoolFile = ::CreateFile( szSpoolFile,
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL );

    if ( INVALID_HANDLE_VALUE == hSpoolFile )
    {
        if ( ERROR_FILE_NOT_FOUND == GetLastError() )
        {
            bSuccess = TRUE;
        }
        goto BailOut;
    }

    // Process event packets.

    for (;;)
    {
        // Read packet length.

        if ( ( FALSE == ReadFile( hSpoolFile,
                                  &dwPacketSize,
                                  sizeof( dwPacketSize ),
                                  &dwBytesRead,
                                  NULL ) ) ||
             ( sizeof( dwPacketSize ) != dwBytesRead ) )
        {
            // No more packets.

            bSuccess = TRUE;
            break;
        }

        // Rough validation of packet size.

        if ( ( sizeof( NAVAP_EVENTPROGRESSREPORTPACKET ) > dwPacketSize ) ||
             ( MAX_PACKET_SIZE < dwPacketSize ) )
        {
            // Assertion is only to signal bad spool file.
            // This is still a valid error condition which must be handled.

            SYM_ASSERT( FALSE );
            goto BailOut;
        }

        // Allocate packet buffer and read packet.

        // Allocating packet size plus sizeof( WCHAR ) of zeroed memory to
        // ensure free-form string data at end of packet is terminated
        // with WCHAR EOS.

        pEventInfo = ( NAVAP_EVENTPROGRESSREPORTPACKET * )
            GlobalAllocPtr( GHND, dwPacketSize + sizeof( WCHAR ) );

        if ( NULL == pEventInfo )
        {
            goto BailOut;
        }

        if ( ( FALSE == ReadFile( hSpoolFile,
                                  pEventInfo,
                                  dwPacketSize,
                                  &dwBytesRead,
                                  NULL ) ) ||
             ( dwPacketSize != dwBytesRead ) )
        {
            // Assertion is only to signal bad spool file.
            // This is still a valid error condition which must be handled.

            SYM_ASSERT( FALSE );
            goto BailOut;
        }

        // Validate pointers to string data at end of packet.
        // 1. Maximum index is end of packet minus beginning of string data
        //    minus one for zero-based.
        // 2. Validation of dwMovedRenamedFileIndex is not required.
        //    Part of a union with dwFileCausingTroubleIndex.


        dwMaxStringIndex = ( WCHAR * )( ( LPBYTE )pEventInfo + dwPacketSize ) -
                           pEventInfo->szStringArray - 1;

        if ( ( dwMaxStringIndex < pEventInfo->dwConfigurationPathIndex ) ||
             ( dwMaxStringIndex < pEventInfo->dwVirusNameIndex ) ||
             ( dwMaxStringIndex < pEventInfo->dwFileDeviceInTroubleIndex ) ||
             ( dwMaxStringIndex < pEventInfo->dwFileCausingTroubleIndex ) )
        {
            // Assertion is only to signal bad spool file.
            // This is still a valid error condition which must be handled.

            SYM_ASSERT( FALSE );
            goto BailOut;
        }

        // Process packet.

        SvcHandleEventProgress( pEventInfo );

        GlobalFreePtr( pEventInfo );
        pEventInfo = NULL;
    }

BailOut:
    if ( INVALID_HANDLE_VALUE != hSpoolFile )
    {
        // Close and delete file.
        // Whether success or error, we've done as much processing with it
        // as possible.

        CloseHandle( hSpoolFile );
        DeleteFile( szSpoolFile );
    }

    if ( NULL != pEventInfo )
        GlobalFreePtr( pEventInfo );

    return ( bSuccess );
} // CApw95Cmd::SvcProcessSpooledEventProgress()


//************************************************************************
// NAVAP Service RPC Server routines
//************************************************************************

//************************************************************************
// SvcHandleRequestCommand()
//
// DWORD SvcHandleRequestCommand (
//      LPVOID lpCommandPacket );
//
// This is the handler for the NAVAP service RPC server.
// Merely a pass-through for the CApw95Cmd member HandleRequestCommand().
//
// Returns:
//      See Capw95Cmd::HandleRequestCommand().
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

DWORD SvcHandleRequestCommand ( LPVOID lpCommandData )
{
    auto    DWORD   dwResult;

    if ( g_pCmd )
    {
        dwResult = g_pCmd->SvcHandleRequestCommand( ( PCMDPACKET ) lpCommandData );
    }
    else
    {
        SYM_ASSERT( FALSE );
        dwResult = ( DWORD ) - 1;
    }

    return ( dwResult );
} // SvcHandleRequestCommand()


//************************************************************************
// CApw95Cmd::SvcHandleRequestCommand()
//
// DWORD CApw95Cmd::SvcHandleRequestCommand (
//      LPVOID lpCommandPacket );
//
// This is the handler for the NAVAP service RPC server.
//
// Returns:
//      Return value depends on request.
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

DWORD CApw95Cmd::SvcHandleRequestCommand ( PCMDPACKET lpCommandPacket )
{
    auto    DWORD   dwResult;
    auto    BOOL    bEnableHeuristicScanSave;
    auto    UINT    uHeuristicLevelSave;

    switch ( lpCommandPacket->dwCommand )
    {
    case NAVAPSVC_RELOADCONFIG:

        dwResult = ( SvcReloadApConfig() ? NAVAPSVC_SUCCESS : NAVAPSVC_ERROR );

        // Calling SvcReloadSvcOptions here as well, since we only get the
        // one request from NAVW. Use this message from NAVW to reload the
        // fakey service's options.

        bEnableHeuristicScanSave = m_bEnableHeuristicScan;
        uHeuristicLevelSave = m_uHeuristicLevel;

        SvcLoadSvcOptions();

        if ( bEnableHeuristicScanSave != m_bEnableHeuristicScan ||
             uHeuristicLevelSave != m_uHeuristicLevel )
            {
                                // Reload the virus definitions
            SvcReloadApEngine();
            }

        // Save new date of options files being used.

        m_OptionsTime.Update();

        break;

    case NAVAPSVC_RELOADOPTIONS:

        dwResult = ( SvcLoadSvcOptions() ? NAVAPSVC_SUCCESS : NAVAPSVC_ERROR );
        break;

    case NAVAPSVC_GET_STATUS:

        switch ( GetStatusOfDriver() )
        {
        case AP_STATE_ENABLED:
            dwResult = NAVAPSVC_ENABLED;
            break;

        case AP_STATE_DISABLED:
            dwResult = NAVAPSVC_DISABLED;
            break;

        case AP_STATE_ERROR:
        default:
            dwResult = NAVAPSVC_ERROR;
            break;
        }
        break;

    case NAVAPSVC_AP_ENABLE:
        dwResult = EnableDriver();
        ::ApwUpdateUI();
        break;

    case NAVAPSVC_AP_DISABLE:
        dwResult = DisableDriver();
        ::ApwUpdateUI();
        break;

    // No other commands implemented yet.

    case NAVAPSVC_AP_SCANFLOPPYA:
    case NAVAPSVC_EXEC_PROGRAM:
    case NAVAPSVC_WAIT_FOR_STARTUP:
    case NAVAPSVC_PAUSE_WATCHING_DEFS:
    case NAVAPSVC_CONTINUE_WATCHING_DEFS:
    case NAVAPSVC_ALERTSVC_READY:
    default:
        dwResult = NAVAPSVC_ERROR;
        break;
    }

    return ( dwResult );
} // SvcHandleRequestCommand()


//************************************************************************
// CApw95Cmd::SvcReloadApConfig()
//
// BOOL CApw95Cmd::SvcReloadApConfig ()
//
// This routine signals the NAVAP driver to reload its options.
//
// Returns:
//      TRUE    on success
//      FALSE   otherwise
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcReloadApConfig ()
{
    auto    DWORD   dwResult;
    auto    DWORD   dwBytesReturned;

    auto    BOOL    bSuccess;

    SYM_ASSERT( m_pNAVAP );

    // Initialize locals.

    bSuccess = TRUE;

    // Reload configuration.

    if ( ( FALSE == m_pNAVAP->Send( NAVAP_COMM_RELOADCONFIG,
                                    NULL,
                                    0,
                                    &dwResult,
                                    sizeof( dwResult ),
                                    &dwBytesReturned ) ) ||
         ( sizeof( dwResult ) != dwBytesReturned ) ||
         ( ERROR_SUCCESS != dwResult ) )
    {
        bSuccess = FALSE;
    }

    // Notify NAVAPW to resync UI in case we've caused a change in
    // the driver's state.

    ApwUpdateUI();

    return ( bSuccess );
} // CApw95Cmd::SvcReloadApConfig()


//************************************************************************
// CApw95Cmd::SvcReloadApEngine()
//
// BOOL CApw95Cmd::SvcReloadApEngine ()
//
// This routine signals the NAVAP driver to reload its virus engine.
//
// Returns:
//      TRUE    on success
//      FALSE   otherwise
//************************************************************************
// 06/26/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcReloadApEngine ()
{
    auto    DWORD   dwResult;
    auto    DWORD   dwBytesReturned;

    auto    BOOL    bSuccess;

    SYM_ASSERT( m_pNAVAP );

    // Initialize locals.

    bSuccess = TRUE;

    // Unload engine.

    if ( ( FALSE == m_pNAVAP->Send( NAVAP_COMM_UNLOADAVAPI,
                                    NULL,
                                    0,
                                    &dwResult,
                                    sizeof( dwResult ),
                                    &dwBytesReturned ) ) ||
         ( sizeof( dwResult ) != dwBytesReturned ) ||
         ( ERROR_SUCCESS != dwResult ) )
    {
        bSuccess = FALSE;
    }

    // Reload engine.

    if ( ( FALSE == m_pNAVAP->Send( NAVAP_COMM_LOADAVAPI,
                                    NULL,
                                    0,
                                    &dwResult,
                                    sizeof( dwResult ),
                                    &dwBytesReturned ) ) ||
         ( sizeof( dwResult ) != dwBytesReturned ) ||
         ( ERROR_SUCCESS != dwResult ) )
    {
        bSuccess = FALSE;
    }

    // Notify NAVAPW to resync UI in case we've caused a change in
    // the driver's state.

    ApwUpdateUI();

    return ( bSuccess );
} // CApw95Cmd::SvcReloadApEngine()


//************************************************************************
// CApw95Cmd::SvcLoadSvcOptions()
//
// BOOL CApw95Cmd::SvcLoadSvcOptions ()
//
// This routine loads the service's relevent settings from NAVOPTS.DAT.
//
// Returns:
//      TRUE    on success
//      FALSE   otherwise
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcLoadSvcOptions ()
{
    auto    _NAVOPTIONS rNavopts;
    auto    TCHAR       szFileName [ MAX_PATH ];
    auto    BOOL        bSuccess;

    // Read raw options.

    bSuccess = ::ApwReadNavopts( &rNavopts );

    // Save binary settings.

    m_bLogKnown            = rNavopts.activity.bKnown;
    m_bLogUnknown          = rNavopts.activity.bUnknown;
    m_bLogInocChange       = rNavopts.activity.bInocChange;
    m_bLogVirusLike        = rNavopts.activity.bVirusLike;
    m_bLogStartEnd         = rNavopts.activity.bStartEnd;
    m_bLogVirusList        = rNavopts.activity.bVirusList;
    m_bLogQuarantine       = rNavopts.activity.bQuarantine;
    m_bLogKeepUpTo         = rNavopts.activity.bKeepUpTo;
    m_uLogKiloBytes        = rNavopts.activity.uKiloBytes;
    m_bEnableHeuristicScan = rNavopts.tsr.bEnableHeuristicScan;
    m_uHeuristicLevel      = rNavopts.tsr.uHeuristicLevel;

    // Used to have to track alert options.
    // Now calling ProcessCVirusAlert() which reads options and decides what
    // to send and where to send it.
    // However, doesn't seem quite legit to read/query options there.

    // m_bAlertNetUser     = rNavopts.alert.bAlertNetUsr;
    // m_bAlertNetCon      = rNavopts.alert.bAlertNetCon;
    // m_bAlertNLM         = rNavopts.alert.bAlertNLM;
    // m_bAlertNtSvc       = TRUE;
    // m_bAlertKnown       = rNavopts.alert.bKnown;
    // m_bAlertUnknown     = rNavopts.alert.bUnknown;
    // m_bAlertInocChange  = rNavopts.alert.bInoculation;
    // m_bAlertVirusLike   = rNavopts.alert.bVirusLike;
    // m_bAlertStartEnd    = rNavopts.alert.bStartEnd;
    // m_bAlertVirusList   = rNavopts.alert.bVirListChange;
    //&? debug  _tcscpy(m_szAlertNtSvcServer, _T("\\JBrennanP133"));

    //*********************
    // Save string options.

    // Get name of activity log file.

    *m_szLogFile = EOS;

    ::OemToCharBuff( rNavopts.activity.szLogFile,
                     szFileName,
                     sizeof( szFileName ) / sizeof( TCHAR ) );

    if ( EOS == *szFileName )
    {
        if ( 0 == ::LoadString( g_hInstance,
                                IDS_ACTIVITY_LOG,
                                szFileName,
                                sizeof( szFileName ) / sizeof( TCHAR ) ) )
        {
            goto NoLogName;
        }
    }

    // If log path is missing directory or volume, use default volume and
    // directory.

    if ( ( ::ApwNameSkipDirectory( szFileName ) == ::ApwNameSkipVolume( szFileName) ) ||
         ( ::ApwNameSkipVolume( szFileName ) == szFileName ) )
    {
        // Use startup directory default volume and/or dir.

        ::_tcscpy( m_szLogFile, m_szConfigPath );
        ::ApwNameCat( m_szLogFile, ::ApwNameSkipDirectory( szFileName ) );
    }
    else
    {
        ::_tcscpy( m_szLogFile, szFileName );
    }

NoLogName:

    return ( bSuccess );
} // CApw95Cmd::SvcLoadSvcOptions()


//************************************************************************
// CApw95Cmd::SvcNotifyNavw()
//
// BOOL CApw95Cmd::SvcNotifyNavw ( DWORD dwMessage )
//
// This routine starts a separate thread to send a command message to NAVW.
// Uses m_hNotifyNavwMutex to signal
//
// Possible commands/messages are:
//      UWM_NAVAP_ACTIVATING
//
// Returns:
//      TRUE    on success
//      FALSE   otherwise
//************************************************************************
// 06/27/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::SvcNotifyNavw ( DWORD dwMessage )
{
    // Start thread to send notification.

    return ( -1 != ::_beginthread( ::SvcAsyncNotifyNavw, 0, ( LPVOID ) dwMessage ) );
} // CApw95Cmd::SvcNotifyNavw()


//************************************************************************
// SvcAsyncNotifyNavw()
//
// VOID __cdecl SvcAsyncNotifyNavw ( LPVOID dwMessage )
//
// Merely passes on to CApw95Cmd::SvcAsyncNotifyNavw(), routine used by
// CApw95Cmd::SvcNotifyNavw to send asynchronous notifications to NAVW.
//
// Possible commands/messages are:
//      UWM_NAVAP_ACTIVATING
//************************************************************************
// 06/27/1997 DALLEE, created.
//************************************************************************

VOID SvcAsyncNotifyNavw ( LPVOID dwMessage )
{
    // Send notification to Navw.

    ApwNotifyNavw( ( DWORD ) dwMessage );

    _endthread();
} // SvcAsyncNotifyNavw()


