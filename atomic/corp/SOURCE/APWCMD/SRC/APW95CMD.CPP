// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/Apw95cmd.cpv   1.13   29 Jun 1998 22:20:54   RStanev  $
//
// Description:
//      Source for NAVAPW external commands W95 implementation.
//
//      These are all the commands which can be issued by the
//      UI whose implementation will change based on
//      platform or product.
//      These routines will be contained in APWCMD.DLL.
//      Install will copy the correct version based on platform.
//
//      APWCMD.DLL clients should only directly call ApwCmdNew(). This will
//      return a pointer to a CApwCmd class. All AP Agent UI communication to
//      the AP Service should be done through this class.
//
// Contains:
//     ApwCmdNew ()
//     CApw95Cmd::CApw95Cmd ()
//     CApw95Cmd::~CApw95Cmd ()
//     CApw95Cmd::Init()
//     CApw95Cmd::DeInit()
//     CApw95Cmd::Options()
//     CApw95Cmd::StartDriver()
//     CApw95Cmd::StopDriver()
//     CApw95Cmd::WaitForStartupPending()
//     CApw95Cmd::EnableDriver()
//     CApw95Cmd::DisableDriver()
//     CApw95Cmd::GetStatusOfDriver()
//     CApw95Cmd::ShutdownScan()
//     CApw95Cmd::ConfirmApwComTasksReceived()
//
// See Also:
//      APWCMD.H for command class which is actually used by the UI.
//************************************************************************
// $Log:   S:/APWCMD/VCS/Apw95cmd.cpv  $
// 
//    Rev 1.13   29 Jun 1998 22:20:54   RStanev
// Added new functionality to support asynchronous processing of tasks
// triggered by driver notifications. This is a workaround for a problem
// with RPC in Win9x.
//
//    Rev 1.12   02 Jun 1998 01:20:32   RStanev
// Merged Roswell fixes.
//
//    Rev 1.11   27 Apr 1998 16:10:02   DALLEE
// COM initialization/deinitialization.
//
//    Rev 1.10   13 Mar 1998 17:41:26   tcashin
// Use Navw's options dialog if he's running and not doing anything else.
//
//    Rev 1.9   18 Feb 1998 18:27:16   DALLEE
//
//    Rev 1.8   21 Aug 1997 00:14:20   DALLEE
// Call SvcProcessSpooledEventProgress to log driver's spooled
// progress messages on Init().
// Also configuration path.
//
//    Rev 1.7   12 Aug 1997 11:58:02   JBRENNA
// Add call to SvcUpdateDefDateInRegistry() when ApwCmd is Init-ing. This
// ensures that the registry is updated when we startup as well as when
// we change to new definitions.
//
//    Rev 1.6   11 Aug 1997 14:12:36   DALLEE
// Fix to last change - previous rev only required at least one module
// initialized. Now requires all known modules to be successfully initialized.
//
//    Rev 1.5   10 Aug 1997 23:48:46   DALLEE
// In GetStatus(), return APSTATUS_ERROR if any NAVAP modules failed to
// initialize. Currently only module checked is AVAPI.
//
//    Rev 1.4   27 Jun 1997 19:25:06   DALLEE
// 1. Moved NAVAP starting notification for Navw here from AP Agent UI.
// 2. Fix for remembering init of VxD - intialize m_bSentNAVAPInit to FALSE.
// 3. Updated some comments.
//
//    Rev 1.3   23 Jun 1997 20:25:08   DALLEE
// 1. Moved deinit of RPC server. Must come before deletion of m_pNAVAP.
// 2. Send controlled init based on status query after loading VxD.
//    Only send controlled deinit if we sent init.
// 3. Removed old alert options.
//
//    Rev 1.2   06 Jun 1997 17:28:54   JBRENNA
// Integrate the Hawking (CDefUtils) functionality to pick up definitions
// from the shared directory.
//
//    Rev 1.1   05 Jun 1997 17:13:42   DALLEE
// Added activity log and alert options.
//
//    Rev 1.0   02 Jun 1997 03:42:20   DALLEE
// Initial revision.
//************************************************************************

// Ensure that we define and initialize the strings defined in NavAppIDs.h
#define NAVAPPIDS_BUILDING

#define STRICT

#include "platform.h"
#include <windowsx.h>
#include <tchar.h>

#include "drvcomm.h"
#include "apcomm.h"
#include "navrpc.h"
#include "linksrv.h"
#include "linkcbk.h"
#include "apscomm.h"

#include "servenfy.h"

#include "NavAppIds.h"

#include "apw.h"
#include "apw95cmd.h"
#include "apwutil.h"
#include "msgreg.h"

//************************************************************************
// Data types
//************************************************************************


//************************************************************************
// Globals
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

extern HINSTANCE   g_hInstance;

CApw95Cmd   *g_pCmd;

#ifdef __cplusplus
}
#endif


//************************************************************************
// Statics
//************************************************************************

//&? DriverComm name for NAVAP device driver.

static TCHAR s_szNAVAP [] = _T("NAVAP");


//************************************************************************
// Local prototypes
//************************************************************************


//************************************************************************
// APWCMD.DLL implementation
//************************************************************************

//************************************************************************
// ApwCmdNew()
//
// Allocate a NAV 95 specific instance of a CApwCmd class.
// Pointer returned should be freed with delete.
//
// Required export for an APWCMD.DLL implementation.
// This is the only routine which should be called directly by
// an APWCMD.DLL client. All other functionality is accessed through
// the members of the generic CApwCmd class returned by this function.
//
// Returns:
//  CApwCmd *       Pointer to command class on success.
//  NULL            on error.
//************************************************************************
// 05/31/1997 DALLEE, created.
//************************************************************************

APWCMDAPI CApwCmd * WINAPI ApwCmdNew ()
{
    return ( new CApw95Cmd );
} // ApwCmdNew()


//************************************************************************
// CApw95Cmd implementation - Win 95 specific CApwCmd class.
//************************************************************************

//************************************************************************
// CApw95Cmd::CApw95Cmd()
//
// Constructor for command object.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/31/1997 DALLEE, created.
//************************************************************************

CApw95Cmd::CApw95Cmd ()
{
    hComInterfaceWnd    = NULL;
    InterlockedExchange ( &lComInterfaceMsgSent, 0 );

    *m_szConfigPath     = EOS;

    m_pNAVAP            = NULL;

    m_bNAVAPLoaded      = FALSE;
    m_bNAVAPInitialized = FALSE;
    m_bSentNAVAPInit    = FALSE;
    m_bNAVAPCallback    = FALSE;

    m_bLogKnown         = TRUE;
    m_bLogUnknown       = FALSE;
    m_bLogInocChange    = FALSE;
    m_bLogVirusLike     = FALSE;
    m_bLogStartEnd      = TRUE;
    m_bLogVirusList     = FALSE;
    m_bLogQuarantine    = TRUE;
    m_bLogKeepUpTo      = FALSE;
    m_uLogKiloBytes     = 0;
    *m_szLogFile        = EOS;

    // Used to have to track alert options.
    // Now calling ProcessCVirusAlert() which reads options and decides what
    // to send and where to send it.
    // However, doesn't seem quite legit to read/query options there.

    // m_bAlertNLM          = FALSE;
    // m_bAlertNtSvc        = FALSE;
    // m_bAlertNetUser      = FALSE;
    // m_bAlertNetCon       = FALSE;
    // m_bAlertKnown        = FALSE;
    // m_bAlertUnknown      = FALSE;
    // m_bAlertInocChange   = FALSE;
    // m_bAlertVirusLike    = FALSE;
    // m_bAlertStartEnd     = FALSE;
    // m_bAlertVirusList    = FALSE;
    // *m_szAlertNtSvcServer = EOS;

    m_bDefMonitorExit    = FALSE;

    // Initialize the CDefUtils applciation identifier. This is what will be
    // used for the shared definition usage counts.

    m_DefUtilsAppId1.InitWindowsApp (g_szNavAppId1Ap95);
    m_DefUtilsAppId2.InitWindowsApp (g_szNavAppId2Ap95);

    g_pCmd = this;
} // CApw95Cmd::CApw95Cmd()


//************************************************************************
// CApw95Cmd::~CApw95Cmd()
//
// Destructor for command object.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

CApw95Cmd::~CApw95Cmd ()
{
    if ( m_pNAVAP )
    {
        delete ( m_pNAVAP );
        m_pNAVAP = NULL;
    }

    hComInterfaceWnd    = NULL;
    InterlockedExchange ( &lComInterfaceMsgSent, 0 );

    m_bNAVAPLoaded       = FALSE;
    m_bNAVAPInitialized  = FALSE;
    m_bSentNAVAPInit     = FALSE;
    m_bNAVAPCallback     = FALSE;

    g_pCmd = NULL;
} // CApw95Cmd::~CApw95Cmd()


//************************************************************************
// CApw95Cmd::Init()
//
// Initialize command object. This must be called prior to any of the
// CApw95Cmd members.
//
// Parameters:
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::Init ( VOID )
{
    auto    BOOL        bSuccess;
    auto    DWORD       dwStatus;
    auto    DWORD       dwBytesReturned;

    auto    NAVAP_STATUSPACKET  rNavapStatus;

    static  LINKCALLBACKS       rSvcRequestServerCallbacks =
                                    { ::SvcHandleRequestCommand };

    // Initialize locals.

    bSuccess = FALSE;

    // Should only be calling once.

    if ( NULL != m_pNAVAP )
    {
        SYM_ASSERT( FALSE );

        //&? ::SetLastError( ? );

        goto BailOut;
    }

    // COM initialization required for Quarantine.

    CoInitialize( NULL );

    // Get configuration directory.

    if ( 0 == ::GetModuleFileName( g_hInstance,
                                   m_szConfigPath,
                                   sizeof( m_szConfigPath ) / sizeof( TCHAR ) ) )
    {
        goto BailOut;
    }

    ::ApwNameStrip( m_szConfigPath );

    // Load options.

    if ( FALSE == SvcLoadSvcOptions() )
    {
        //&? SetLastError( ?? )

        goto BailOut;
    }

    // Process spooled event file, if any.

    SvcProcessSpooledEventProgress();

    // Initialize NLM alerts.

    InitNLMMessenger();

    // Create new communication object.

    m_pNAVAP = new DriverComm;

    if ( NULL == m_pNAVAP )
        goto BailOut;

    // Set location of driver.

    if ( FALSE == m_pNAVAP->SetVxDLocation( m_szConfigPath ) )
        goto BailOut;

    // Open communications with driver if present, load if necessary.

    m_bNAVAPLoaded = m_pNAVAP->LoadAndOpen( s_szNAVAP );

    if ( FALSE == m_bNAVAPLoaded )
        goto BailOut;

    // Let NAVW know that NAVAP has started.

    SvcNotifyNavw( UWM_NAVAP_ACTIVATING );

    // Unprotect current process before registering callbacks.

    m_pNAVAP->Send( NAVAP_COMM_UNPROTECTPROCESS );

    // Register our service callbacks.

    m_bNAVAPCallback = SvcRegisterCallback();

    if ( FALSE == m_bNAVAPCallback )
        goto BailOut;

    // Initialize driver.

    // First check if statically loaded. If so, no need to send init.

    m_bNAVAPInitialized =
            ( m_pNAVAP->Send( NAVAP_COMM_GETSTATUS,
                              NULL,
                              0,
                              &rNavapStatus,
                              sizeof( rNavapStatus ),
                              &dwBytesReturned ) ) &&
            ( dwBytesReturned == sizeof( rNavapStatus ) ) &&
            ( rNavapStatus.dwDriverImageUnloadable == 0 );

    if ( FALSE == m_bNAVAPInitialized )
    {
        m_bNAVAPInitialized =
                ( m_pNAVAP->Send( NAVAP_COMM_CONTROLLEDINIT,
                                  NULL,
                                  0,
                                  &dwStatus,
                                  sizeof( dwStatus ),
                                  &dwBytesReturned ) ) &&
                ( dwBytesReturned == sizeof( dwStatus ) ) &&
                ( dwStatus == ERROR_SUCCESS );

        // Need to remember if we sent INIT to driver.

        m_bSentNAVAPInit = m_bNAVAPInitialized;
    }

    if ( FALSE == m_bNAVAPInitialized )
        goto BailOut;

    bSuccess = TRUE;

    // Disregarding of whether the driver was loaded statically or
    // dynamically, and disregarding the status of the next
    // operation, send a message telling ap that the agent is loading.

    m_pNAVAP->Send ( NAVAP_COMM_WIN95AGENTINIT );

    // Enable the driver.

    EnableDriver();

    // Start fakey NAVAP Service RPC server.

    m_SvcRequestServer.SetProtocol( NAVAPSVC_PROTOCOL,
                                    NAVAPSVC_ENDPOINT,
                                    FALSE );

    m_SvcRequestServer.SetCallbacks( &rSvcRequestServerCallbacks );

    m_SvcRequestServer.StartListen();

    // Start watching the virus definitions for newer definitions.

    SvcUpdateDefDateInRegistry ();
    SvcStartDefinitionMonitor();

BailOut:
    return ( bSuccess );
} // CApw95Cmd::Init()


//************************************************************************
// CApw95Cmd::Init()
//
// This function works just like Init(VOID), except that it remembers a
// handle to a window which receives APWM_COM_TASKS every time the class
// determines that there may be a need for com work.
//
// Parameters:
//      hParent - handle to the window to receive APWM_COM_TASKS.
//
// Returns:
//      same as Init(VOID).
//************************************************************************
// 06/27/1998 RSTANEV, created.
//************************************************************************

BOOL CApw95Cmd::Init ( HWND hParent )
{
    auto    BOOL        bSuccess;

    hComInterfaceWnd = hParent;

    bSuccess = Init();

    if ( !bSuccess )
        hComInterfaceWnd = NULL;

    return ( bSuccess );
} // CApw95Cmd::Init()


//************************************************************************
// CApw95Cmd::DeInit()
//
// Matching uninitialization for command object.
// See CmdInit()
//
// Parameters:
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

BOOL CApw95Cmd::DeInit ( VOID )
{
    auto    BOOL    bSuccess;
    auto    DWORD   dwStatus;
    auto    DWORD   dwBytesReturned;

    // Initialize locals.

    bSuccess = TRUE;

    // Stop RPC server before deleting m_pNAVAP.
    // Handler is dependent on DriverComm.

    m_SvcRequestServer.StopListen();

    // DeInit DriverComm connection to NAVAP if necessary.

    if ( m_pNAVAP )
    {
        // Send a message to the driver telling it that the agent is bailing
        // out on it.

        m_pNAVAP->Send ( NAVAP_COMM_WIN95AGENTDEINIT );

        if ( m_bSentNAVAPInit )
        {
            bSuccess = ( m_pNAVAP->Send( NAVAP_COMM_CONTROLLEDDEINIT,
                                         NULL,
                                         0,
                                         &dwStatus,
                                         sizeof( dwStatus ),
                                         &dwBytesReturned ) ) &&
                       ( dwBytesReturned == sizeof( dwStatus ) ) &&
                       ( dwStatus == ERROR_SUCCESS );
        }

        delete m_pNAVAP;
        m_pNAVAP = NULL;
    }

    hComInterfaceWnd    = NULL;
    InterlockedExchange ( &lComInterfaceMsgSent, 0 );

    m_bNAVAPLoaded      = FALSE;
    m_bNAVAPInitialized = FALSE;
    m_bSentNAVAPInit    = FALSE;
    m_bNAVAPCallback    = FALSE;

    // Get the Definition Monitor to stop watching for new defs.

    m_bDefMonitorExit   = TRUE;

    // NOTE: Not deinitializing NLM alerts here. According to comments,
    // DeInitNLMMessenger() should only be called on process exit in
    // N32SERVE's DllMain().
    //
    // DeInitNLMMessenger()

    // Matching close for COM.

    CoUninitialize();

    return ( bSuccess );
} // CApw95Cmd::DeInit()


//************************************************************************
// CApw95Cmd::Options()
//
// This routine displays the NAV options panel.
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
// 03/12/1998 TCASHIN, modified to use current Navw window if present
//************************************************************************

BOOL CApw95Cmd::Options( HWND hParent )
{
    //&? Bogons for some text used: NAVW program name, NAVW class.

    auto    HINSTANCE   hNAVW;
    auto    TCHAR       szNAVW [ MAX_PATH ];
    auto    BOOL        bSuccess;
    auto    HWND        hNavwWnd;
    auto    HWND        hCurrWnd;

    // Intialize locals.

    bSuccess = FALSE;

    // Any Navw windows open?
    if (::ApwGetCurrentNavWindow( (LPHANDLE)&hNavwWnd, (LPHANDLE)&hCurrWnd))
    {
        // Yep. Open the current Navw window

        ShowWindow( hCurrWnd, SW_RESTORE );
        SetForegroundWindow(hCurrWnd);

        // If it is the main window only, then tell
        // the window proc to put up the Options.

        if ( hCurrWnd == hNavwWnd )
            {
            SvcNotifyNavw( UWM_DISPLAY_OPTIONS );
            }

        bSuccess = TRUE;

        }
    else
    {
        // Get name of NAVW and starting directory.

        ::_tcscpy( szNAVW, m_szConfigPath );
        ::ApwNameCat( szNAVW, _T("NAVW32.EXE") );

        // Launch NAVW.
        //&? copied old behavior - AP Wnd is parent of NAVW, is this legit?
        // Call to Dave's new options DLL directly? Need to synchronize
        // access to options panel?

        hNAVW = ::ShellExecute( hParent,
                                _T("open"),
                                szNAVW,
                                _T("/O:1"),
                                m_szConfigPath,
                                SW_SHOWNORMAL );

        if ( ( UINT ) hNAVW <= HINSTANCE_ERROR )
        {
            // Handle errors in CApwCmd client.
        }
        else
        {
            bSuccess = TRUE;
        }
    }

    return ( bSuccess );
} // CApw95Cmd::Options()


BOOL CApw95Cmd::StartDriver( VOID )
{
    auto    DWORD       dwStatus;
    auto    DWORD       dwBytesReturned;
    auto    BOOL        bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    // Verify had successful init.

    if ( NULL == m_pNAVAP )
    {
        // Not properly initialized.

        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    // If driver is now loaded and uninitialized, initialize it.

    if ( ( m_bNAVAPLoaded ) &&
         ( !m_bNAVAPInitialized ) )
    {
        m_bNAVAPInitialized =
                ( m_pNAVAP->Send( NAVAP_COMM_CONTROLLEDINIT,
                                  NULL,
                                  0,
                                  &dwStatus,
                                  sizeof(dwStatus),
                                  &dwBytesReturned ) ) &&
                ( dwBytesReturned == sizeof( dwStatus ) ) &&
                ( dwStatus == ERROR_SUCCESS );

        m_bSentNAVAPInit = m_bNAVAPInitialized;
    }

    bSuccess = m_bNAVAPInitialized;

BailOut:
    return ( bSuccess );
} // CApw95Cmd::StartDriver()


BOOL CApw95Cmd::StopDriver( VOID )
{
    auto    DWORD       dwStatus;
    auto    DWORD       dwBytesReturned;
    auto    BOOL        bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    // Verify had successful init.

    if ( NULL == m_pNAVAP )
    {
        // Not properly initialized.

        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    // If driver is loaded and initialized, try to deinitialize.

    if ( ( m_bNAVAPLoaded ) &&
         ( m_bNAVAPInitialized ) )
    {
        if ( ( m_pNAVAP->Send( NAVAP_COMM_CONTROLLEDDEINIT,
                               NULL,
                               0,
                               &dwStatus,
                               sizeof( dwStatus ),
                               &dwBytesReturned ) ) &&
             ( dwBytesReturned == sizeof( dwStatus ) ) &&
             ( dwStatus == ERROR_SUCCESS ) )
        {
            m_bNAVAPInitialized = FALSE;
            m_bSentNAVAPInit    = FALSE;
        }
    }

    bSuccess = !m_bNAVAPInitialized;

BailOut:
    return ( bSuccess );
} // Apw95::CmdStopDriver()


BOOL CApw95Cmd::WaitForStartupPending( VOID )
{
    return ( AP_STATE_ENABLED == GetStatusOfDriver() );
} // CApw95Cmd::WaitForStartupPending()


BOOL CApw95Cmd::EnableDriver( VOID )
{
    auto    BOOL        bSuccess;

    const   DWORD       dwEnable = 1;

    if ( NULL == m_pNAVAP )
    {
        // Should never be called after failed init.

        SYM_ASSERT( FALSE );

        bSuccess = FALSE;
    }
    else
    {
        m_pNAVAP->Send ( NAVAP_COMM_SETENABLEDDISABLED,
                         ( LPVOID ) &dwEnable,
                         sizeof( DWORD ) );
    }

    return ( bSuccess );
} // CApw95Cmd::EnableDriver()


BOOL CApw95Cmd::DisableDriver( VOID )
{
    auto    BOOL        bSuccess;

    const   DWORD       dwDisable = 0;

    if ( NULL == m_pNAVAP )
    {
        // Should never be called after failed init.

        SYM_ASSERT( FALSE );

        bSuccess = FALSE;
    }
    else
    {
        bSuccess = m_pNAVAP->Send ( NAVAP_COMM_SETENABLEDDISABLED,
                                    ( LPVOID ) &dwDisable,
                                    sizeof( DWORD ) );
    }

    return ( bSuccess );
} // CApw95Cmd::DisableDriver()


AP_STATE CApw95Cmd::GetStatusOfDriver( VOID )
{
    auto    NAVAP_STATUSPACKET  rNavapStatus;

    auto    DWORD               dwStatus;
    auto    DWORD               dwBytesReturned;
    auto    AP_STATE            nState;

    // Initialize locals.

    nState = AP_STATE_ERROR;

    if ( ( NULL != m_pNAVAP ) &&
         ( m_bNAVAPInitialized ) )
    {
        // Get status of driver.

        if ( ( m_pNAVAP->Send( NAVAP_COMM_GETENABLEDDISABLED,
                               NULL,
                               0,
                               &dwStatus,
                               sizeof( dwStatus ),
                               &dwBytesReturned ) ) &&
             ( sizeof( dwStatus ) == dwBytesReturned ) )
        {
            nState = dwStatus ? AP_STATE_ENABLED : AP_STATE_DISABLED;
        }

        // Check whether all modules were successfully initialized.

        if ( ( m_pNAVAP->Send( NAVAP_COMM_GETSTATUS,
                               NULL,
                               0,
                               &rNavapStatus,
                               sizeof( rNavapStatus ),
                               &dwBytesReturned ) ) &&
             ( dwBytesReturned == sizeof( rNavapStatus ) ) )
        {
            #define NAVAP_ALL_MODULES   ( NAVAP_INITIALIZEDMODULES_AVAPI )

            // Assertion is here to catch new modules reported by driver.
            // Need to OR new defines into NAVAP_ALL_MODULES if it fires.

            SYM_ASSERT( 0 == ( rNavapStatus.dwInitializedModules & ~NAVAP_ALL_MODULES ) );

            if ( NAVAP_ALL_MODULES != ( rNavapStatus.dwInitializedModules & NAVAP_ALL_MODULES ) )
            {
                nState = AP_STATE_ERROR;
            }
        }
    }

    return ( nState );
} // CApw95Cmd::GetStatusOfDriver()


BOOL CApw95Cmd::ShutdownScan( HWND hParent )
{
    //&? Not required in W95 implementation.
    //&? NAVAP.VXD performs shutdown scan.

    return ( FALSE );
} // CApw95Cmd::ShutdownScan()

VOID CApw95Cmd::ConfirmApwComTasksReceived( VOID )
{
    // The next time NAVAP driver sends a packet that needs to be
    // processed with com interface, the agent should post another
    // message.

    InterlockedExchange ( &lComInterfaceMsgSent, 0 );
} // CApw95Cmd::ConfirmApwComTasksReceived()
