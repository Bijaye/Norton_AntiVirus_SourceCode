// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUI/VCS/apwmain.cpv   1.26   25 Aug 1998 19:22:00   CEATON  $
//
// Description:
//      Source for NAVAPW main window.
//
// Contains:
//      WinMain()
//      ApwMainWndProc()
//
// See Also:
//************************************************************************
// $Log:   S:/APWUI/VCS/apwmain.cpv  $
// 
//    Rev 1.26   25 Aug 1998 19:22:00   CEATON
// Added modification to disable NAVAP.VXD before Windows is shutdown to disable
// the floppy scanning of drive A:. This addresses the problem with hot-swappable
// floppies that are removed before shutdown.
// 
//    Rev 1.25   14 Aug 1998 12:24:52   mdunn
// Changed trial-expired message to support varying-length trial periods
// 
//    Rev 1.24   14 Jul 1998 14:58:24   mdunn
// Moved preprocessor stuff around to get the trialware GUIDs properly defined.
// 
//    Rev 1.23   29 Jun 1998 22:50:18   RStanev
// All calls to CApwCmd::Init() have now been updated to pass either a handle
// or NULL.
//
//    Rev 1.22   29 Jun 1998 22:21:34   RStanev
// CApwCmd now takes HWND as a parameter.  In addition, added processing
// for APWM_COM_TASKS message, which triggers calls to the quarantine APIs
// for checking the incoming directory.
//
//    Rev 1.21   27 May 1998 13:09:18   mdunn
// Nuked ApwActivatePrevInst() since we don't want the Agent UI shown any more.
//
//    Rev 1.20   13 Mar 1998 17:57:08   tcashin
// Start Navw when the "Start Nav" menu item is selected.
//
//    Rev 1.19   18 Feb 1998 18:32:38   DALLEE
// Moved InitNLMMessenger() call to APWCMD Apw95Cmd.cpp.
// APWUI should only contain UI and rely on the APWCMD project for guts.
//
//    Rev 1.18   17 Feb 1998 14:49:24   MKEATIN
// Roswell Port:
//
//     Rev 1.17   16 Jan 1998 12:42:04   CEATON
//  Added call to InitNLMMessenger(), which is necessary for alerting NAV NLMs.
//
//
//    Rev 1.17   26 Jan 1998 13:57:12   DALLEE
// Mask low 4 bits of uCmd in WM_SYSCOMMAND.
//
//    Rev 1.16   11 Aug 1997 17:08:56   DALLEE
// Added magic wParam and lParam values to APWM_SHUTDOWN so we don't respond
// if we ever get this message by accident.
// Added APWM_UPDATE_UI handling - notification that we should resync the
// displayed status of the driver with the current status.
//
//    Rev 1.15   05 Aug 1997 18:05:16   DALLEE
// Added call to ApwActivate() before ExitWindowsEx() when shutting down.
// Main thread was yielding in ExitWindowsEx() and never woke up again.
//
// Activating the main window seems to work around the problem - so does
// moving the mouse over the main dialog if visible - but I'm still not
// sure of the root of the problem.
//
//    Rev 1.14   29 Sep 1997 16:30:42   DALLEE
// Moved trialware check after single instance check.
// On startup, we got two copies of NAVAPW - one from VxD, one from Run
// command. Starting two of the same RPC servers ( happens in pCmd->Init() )
// causes the first process to exit to crash.
//
//    Rev 1.13   17 Jul 1997 12:07:58   DALLEE
// Show minimized in all cases now when launched for the first time.
//
//    Rev 1.12   15 Jul 1997 17:54:58   MKEATIN
// If the trial period is expired we now disable the VxD.
//
//    Rev 1.11   11 Jul 1997 17:42:04   MKEATIN
// Added trial version code
//
//    Rev 1.10   09 Jul 1997 13:04:56   DALLEE
// Initialize CApwCmd before using it in WM_SHUTDOWN.
// Egads, how did this ever work?
//
//    Rev 1.9   27 Jun 1997 19:17:54   DALLEE
// No longer send NAVAP starting message to NAVW.
// This will be handled by the AP service.
//
//    Rev 1.8   24 Jun 1997 16:59:30   DALLEE
// Handling NAVW_CMD_UNLOADNAVAPW: Disable driver before deinit in
// response to unload command from NAVW.
// This will bypass the floppy scanning ( and the annoying blue screen ).
//
//    Rev 1.7   23 Jun 1997 18:12:56   DALLEE
// Fix for showing minimized if /LOADQUIET, and not previously loaded by VxD.
//
//    Rev 1.6   17 Jun 1997 18:04:12   DALLEE
// 1. Added shutdown bitmap replacement code when shutting down due to virus.
// 2. DeInit() command object before shutting down Windows due to virus.
//
//    Rev 1.5   16 Jun 1997 11:04:46   DALLEE
// Show minimized, no active on /LOADQUIET.
//
//    Rev 1.4   11 Jun 1997 12:14:54   DALLEE
// Added ApwTrayTimer() to wait for tray if not present on startup.
// Don't allow exit unless on NT.
// Notify NAVW when starting so NAVW can disable AP for its process.
//
//    Rev 1.3   10 Jun 1997 14:51:40   DALLEE
// Added registered message handling for NAVW NAVAPW communication.
// Allows update of icon/disable options, and unloading auto-protect.
//
//    Rev 1.2   09 Jun 1997 00:04:36   DALLEE
// Added WM_ENDSESSION handling to perform cleanup - prevent hang on shutdown.
//
//    Rev 1.1   05 Jun 1997 17:10:14   DALLEE
// Added disabling of "Disable" button based on options.
//
//    Rev 1.0   02 Jun 1997 13:56:56   DALLEE
// Initial revision.
//************************************************************************

#define STRICT

#include "platform.h"
#include <windowsx.h>
#include <tchar.h>
#include <ctl3d.h>

#include "resource.h"
#include "apw.h"
#include "apwutil.h"
#include "apwcfg.h"
#include "apwtray.h"

#include "drvcomm.h"

#include "navwcomm.h"
#include "msgreg.h"

#include "navver.h"

#define INITGUID
#include "iquaran.h"

#ifdef NAV_PREVIEW
    #include "tryint.h"
#endif

#undef INITGUID


//************************************************************************
// DEFINES
//************************************************************************

#define APW_WND_STYLE   (CS_HREDRAW | CS_VREDRAW )

// Values for timer used to wait for shell tray to appear.

#define TRAY_TIMER_ID       1
#define TRAY_TIMER_TIMEOUT  5000        // in milliseconds.


//************************************************************************
// TYPEDEFS
//************************************************************************


//************************************************************************
// GLOBALS
//************************************************************************

const   char    g_szClassName[] = APW_CLASS_NAME;
const   char    g_szSingleInstMutex[] = _T("Norton AntiVirus Agent Is Running");


//************************************************************************
// PROTOTYPES
//************************************************************************

#ifdef __cplusplus
extern "C"  {
#endif

LRESULT __declspec( dllexport ) WINAPI ApwMainWndProc (
    HWND      hWnd,
    UINT      uMessage,
    WPARAM    wParam,
    LPARAM    lParam );

#ifdef __cplusplus
}
#endif

static BOOL ApwMainWM_CREATE ( HWND hWnd, LPCREATESTRUCT lpCreateStruct );
static VOID ApwMainWM_SYSCOMMAND ( HWND hWnd, UINT cmd, int x, int y );
static VOID ApwMainWM_COMMAND ( HWND hWnd, int nID, HWND hWndControl, UINT uCode );
static VOID ApwMainWM_ENDSESSION ( HWND hWnd, BOOL bEnding );
static VOID ApwMainWM_DESTROY ( HWND hWnd );
static LRESULT ApwMainAPWM_ACTIVATE ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_INITDIALOG ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_SHUTDOWN ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_UPDATE_UI ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainAPWM_COM_TASKS ( HWND hWnd, WPARAM wParam, LPARAM lParam );
static LRESULT ApwMainDefaultProc ( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam );
static BOOL ApwRegisterClass ( HINSTANCE hInst );
static HWND ApwCreateMainWnd ( HINSTANCE hInst );
static VOID ApwMainDlgUpdate ( VOID );
static VOID CALLBACK ApwTrayTimer( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
static BOOL ApwReplaceShutdownBitmaps ( VOID );
#ifdef NAV_PREVIEW
  static BOOL IsTrialValid( HINSTANCE hInstance );
#endif
static BOOL SkipShutdownScan( VOID );


//************************************************************************
// CODE
//************************************************************************

//************************************************************************
// WinMain()
//
// You gotta eat, you gotta work, you gotta sleep, you gotta ski...
// ...and you gotta have a WinMain().
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/18/1997 DALLEE, created
//************************************************************************

int PASCAL WinMain ( HINSTANCE  hInst,
                     HINSTANCE  hPrevInst,
                     LPSTR      lpszCmdLine,
                     int        nCmdShow )
{
    auto    MSG         msg;
    auto    HWND        hMainWnd;

    auto    HANDLE      hSingleInstMutex;

    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    auto    BOOL        bLoadQuiet;
    auto    int         nReturn;

    // Initialize locals.

    bLoadQuiet  = FALSE;
    nReturn     = 0;
    pCmd        = NULL;

    hSingleInstMutex = NULL;

    // Save instance handle.

    pConfig = ApwGetConfig();
    pConfig->SetInstance( hInst );

    // Get external command implementation.

    pCmd = ApwCmdNew();

    if ( NULL == pCmd )
    {
        //&? Need bad error message? Unlikely ever to fail...

        goto BailOut;
    }

    pConfig->SetCmd( pCmd );

    //&? Kluged command line handling here. If we ever have
    //&? more than one option, need to remove this and do it right.
    //&? Function to parse command line, set state in config object.

    if ( NULL != _tcsstr( lpszCmdLine, _T("/LOADQUIET") ) )
    {
        bLoadQuiet = TRUE;
    }

    // Activate previous instance, if any.

    hSingleInstMutex = CreateMutex( NULL, FALSE, g_szSingleInstMutex );

    if ( NULL == hSingleInstMutex )
    {
        goto BailOut;
    }

    if ( ERROR_ALREADY_EXISTS == GetLastError() )
    {
        // No longer activating the previous instance, since we don't
        // want the Agent UI shown any more.

        goto BailOut;
    }

#ifdef NAV_PREVIEW
     if ( !IsTrialValid( hInst ) )
     {
         goto BailOut;
     }
#endif

    // Register class.

    if ( FALSE == ApwRegisterClass( hInst ) )
    {
        goto BailOut;
    }

    // CTL3D initialization.

#ifdef _X86_
    if ( FALSE == NewShellPresent() )
    {
        Ctl3dRegister( hInst );
    }
#endif

    // Create main window/dialog.

    hMainWnd = ApwCreateMainWnd( hInst );

    if ( NULL != hMainWnd )
    {
        // Show main window.
        // Displaying hidden/minimized in all cases now...

        ApwMinimize();

        // Message loop.

        while ( GetMessage( &msg, NULL, 0, 0 ) )
        {
            if ( !IsDialogMessage( hMainWnd, &msg ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }

        nReturn = msg.wParam;
    }

#ifdef _X86_
    if ( FALSE == NewShellPresent() )
    {
        Ctl3dUnregister( hInst );
    }
#endif

BailOut:
    if ( NULL != pCmd )
    {
        pConfig->SetCmd( NULL );
        delete pCmd;
    }

    if ( NULL != hSingleInstMutex );
        CloseHandle( hSingleInstMutex );

    return ( nReturn );
} // WinMain()


//************************************************************************
// ApwRegisterClass()
//
// static BOOL ApwRegisterClass (
//      HINSTANCE   hInst );            // Our instance.
//
// This routine registers NAVAPW's window class.
//
// Returns:
//      TRUE        on success.
//      FALSE       failed.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static BOOL ApwRegisterClass ( HINSTANCE  hInst )
{
    auto    WNDCLASSEX  wndclass;

    wndclass.cbSize         = sizeof( WNDCLASSEX );
    wndclass.style          = APW_WND_STYLE;
    wndclass.lpfnWndProc    = ApwMainWndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = sizeof( LONG ) * APWL_MAX;
    wndclass.hInstance      = hInst;
    wndclass.hIcon          = LoadIcon( hInst, MAKEINTRESOURCE( IDI_APW_APP_ENABLED ) );
    wndclass.hIconSm        = ( HICON ) LoadImage ( hInst,
                                                    MAKEINTRESOURCE( IDI_APW_APP_ENABLED ),
                                                    IMAGE_ICON,
                                                    16, 16, 0 );
    wndclass.hCursor        = NULL;
    wndclass.hbrBackground  = ( HBRUSH ) ( COLOR_BTNFACE + 1 );
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = g_szClassName;

    return ( RegisterClassEx( &wndclass ) ? TRUE : FALSE );
} // ApwRegisterClass()


//************************************************************************
// ApwCreateMainWnd()
//
// static HWND ApwCreateMainWnd (
//      HINSTANCE   hInst )             // Our instance.
//
// This routine creates NAVAPW's main window/dialog.
//
//
// Returns:
//      HWND    Handle of main window.
//      NULL    if not created.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static HWND ApwCreateMainWnd ( HINSTANCE    hInst )
{
    auto    CApwConfig  *pConfig;
    auto    HWND        hWnd;

    // Create main window. Actually a dialog using our registered class.

    hWnd = CreateDialog( hInst,
                         MAKEINTRESOURCE( IDD_APW_MAIN ),
                         NULL,
                         NULL );

    pConfig = ApwGetConfig();
    pConfig->SetMainWnd( hWnd );

    return ( hWnd );
} // ApwCreateMainWnd()


//************************************************************************
// ApwMainWndProc()
//
// Main window procedure.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

LRESULT __declspec( dllexport ) WINAPI ApwMainWndProc ( HWND      hWnd,
                                                        UINT      uMessage,
                                                        WPARAM    wParam,
                                                        LPARAM    lParam )
{
    auto    LONG    lReturn = 0;

    switch ( uMessage )
    {
#ifdef _X86_
    case WM_SYSCOLORCHANGE:
        if ( FALSE == NewShellPresent() )
        {
            Ctl3dColorChange();
            InvalidateRect( hWnd, NULL, TRUE );
        }
        lReturn = DefDlgProc( hWnd, uMessage, wParam, lParam );
        break;
#endif

    case WM_CREATE:
        lReturn = HANDLE_WM_CREATE( hWnd, wParam, lParam,
                                    ApwMainWM_CREATE );
        break;

    case WM_COMMAND:
        lReturn = HANDLE_WM_COMMAND( hWnd, wParam, lParam,
                                     ApwMainWM_COMMAND );
        break;

    case WM_SYSCOMMAND:
        lReturn = HANDLE_WM_SYSCOMMAND( hWnd, wParam, lParam,
                                        ApwMainWM_SYSCOMMAND );
        break;

    case WM_ENDSESSION:
        lReturn = HANDLE_WM_ENDSESSION( hWnd, wParam, lParam,
                                        ApwMainWM_ENDSESSION );
        break;

    case WM_DESTROY:
        lReturn = HANDLE_WM_DESTROY( hWnd, wParam, lParam,
                                     ApwMainWM_DESTROY );
        break;

    case APWM_INITDIALOG:
        lReturn = ApwMainAPWM_INITDIALOG( hWnd, wParam, lParam );
        break;

    case APWM_ACTIVATE:
        lReturn = ApwMainAPWM_ACTIVATE( hWnd, wParam, lParam );
        break;

    case APWM_TRAY_NOTIFY:
        lReturn = ApwTrayNotify( hWnd, wParam, lParam );
        break;

    case APWM_SHUTDOWN:
        lReturn = ApwMainAPWM_SHUTDOWN( hWnd, wParam, lParam );
        break;

    case APWM_UPDATE_UI:
        lReturn = ApwMainAPWM_UPDATE_UI( hWnd, wParam, lParam );
        break;

    case APWM_COM_TASKS:
        lReturn = ApwMainAPWM_COM_TASKS( hWnd, wParam, lParam );
        break;

    default:
        // ApwMainDefaultProc() handles custom registered messages
        // (for communication with NAVW), or passes through to DefDlgProc().

        lReturn = ApwMainDefaultProc( hWnd, uMessage, wParam, lParam );
        break;
    }

    return ( lReturn );
} // ApwMainWndProc()


//************************************************************************
// ApwMainWM_SYSCOMMAND()
//
// static VOID ApwMainWM_SYSCOMMAND (
//      HWND        hWnd,               // Handle of main window.
//      UINT        uCmd,               // Command issued.
//      int         x,                  // Window position.
//      int         y );
//
//
// Respond to WM_SYSCOMMAND for main window. Need special handling for
// the minimize command.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
//      int         nID                 Control ID.
//      HWND        hWndControl         Window handle of control.
//      UINT        uCode               Notification code.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_SYSCOMMAND ( HWND hWnd, UINT uCmd, int x, int y )
{
    // Mask low 4 bits of uCmd - internal use by Windows only.

    switch ( uCmd & 0xFFFFFFF0 )
    {
    case SC_MINIMIZE:
        // Conceal main window - either minimize, or hide.

        ApwMinimize();
        break;

    default:
        FORWARD_WM_SYSCOMMAND( hWnd, uCmd, x, y, DefWindowProc );
        break;
    }
} // ApwMainWM_SYSCOMMAND()


//************************************************************************
// ApwMainWM_COMMAND()
//
// Respond to WM_COMMAND for main window.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
//      int         nID                 Control ID.
//      HWND        hWndControl         Window handle of control.
//      UINT        uCode               Notification code.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_COMMAND ( HWND    hWnd,
                                int     nID,
                                HWND    hWndControl,
                                UINT    uCode )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    // Get configuration.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    SYM_ASSERT( pConfig );
    SYM_ASSERT( pCmd );

    switch ( nID )
    {
    case IDC_MAIN_MINIMIZE:
        // Conceal main window - either minimize, or hide.

        ApwMinimize();
        break;

    case IDM_TRAY_OPEN:
        // Open our main window.

        ApwActivateNavw();
        break;

    case IDC_MAIN_ENABLE:
    case IDM_TRAY_ENABLE:
        //&? Bogon! Should use separate function probably...

        switch( pConfig->GetState() )
        {
        case AP_STATE_ENABLED:
            pCmd->DisableDriver();
            break;

        case AP_STATE_DISABLED:
            pCmd->EnableDriver();
            break;
        }

        pConfig->SetState( pCmd->GetStatusOfDriver() );
        ApwMainDlgUpdate();
        ApwUpdateTrayIcon();
        break;

    case IDC_MAIN_OPTIONS:
    case IDM_TRAY_OPTIONS:

        pCmd->Options( hWnd );
        break;

    case IDOK:

        break;

    case IDCANCEL:
        // Only allow exit here on NT where UI is separate from
        // service and driver.

        if ( VER_PLATFORM_WIN32_NT == GetWindowsPlatform() )
        {
            DestroyWindow( hWnd );
        }
        else
        {
            ApwMinimize();
        }
        break;

    default:
        break;
    }
} // ApwMainWM_COMMAND()


//************************************************************************
// ApwMainWM_CREATE()
//
// Do startup chores in WM_CREATE of main window.
// Also posts APWM_INITDIALOG message to signal initialization which
// needs to take place after child controls are created.
//
// Parameters:
//      HWND            hWnd            Handle of window being created.
//      LPCREATESTRUCT  lpCreateStruct  Window creation info.
//
// Returns:
//      TRUE                            Continue creation of window.
//      FALSE                           Fail CreateWindow().
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static BOOL ApwMainWM_CREATE ( HWND hWnd, LPCREATESTRUCT lpCreateStruct )
{
    // Center window before first WM_NCPAINT.

    ApwCenterWindow( hWnd, NULL );

    // Send fakey dialog init message to main window.

    PostMessage( hWnd, APWM_INITDIALOG, 0, 0 );

    return ( TRUE );
} // ApwMainWM_CREATE()


//************************************************************************
// ApwMainWM_ENDSESSION()
//
// static VOID ApwMainWM_ENDSESSION (
//      HWND hWnd,                      // Handle of main window.
//      BOOL bEnding )                  // TRUE if session is really ending.
//
// If session is ending, need to perform cleanup here.
//
// Returns:
//      Nothing.
//************************************************************************
// 06/08/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_ENDSESSION ( HWND hWnd, BOOL bEnding )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    if ( bEnding )
    {
        // Get rid of tray icon if present.

        pConfig = ApwGetConfig();
        pCmd    = pConfig->GetCmd();

        pConfig->SetShowIcon( FALSE );
        ApwUpdateTrayIcon();

        // DeInit the command module.

        if ( FALSE == pConfig->GetDoingShutdown() )
        {
            DWORD AvailableDrives;

            AvailableDrives = GetLogicalDrives();

            pConfig->SetDoingShutdown( TRUE );

            if ( !( AvailableDrives & 0x1 ) || SkipShutdownScan() )
                pCmd->DisableDriver();

            pCmd->DeInit();
        }
    }
} // ApwMainWM_ENDSESSION()

//************************************************************************
// SkipShutdownScan()
//
// Check for SkipShutdownScan key to disable VXD on shutdown so that the
// floppy drive is not scanned - problem on some machines when the floppy
// drive is swapped out, but GetLogicalDrive returns a valid A drive
// (Dell Latitude CP running Win95).
//
// Parameters:
//
// Returns:
//      Nothing.
//************************************************************************
// 02/05/1999 jshock
//************************************************************************
static BOOL SkipShutdownScan()
{
	DWORD dwSkipScan = 0, dwBufSize = sizeof(DWORD);
	LONG  lRet;
	HKEY  hkey;

	// read in SkipShutdownScan key under Auto-protect
	lRet = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, 
			_T("Software\\Symantec\\Norton AntiVirus\\Auto-Protect"),
			0, KEY_QUERY_VALUE, &hkey );

	if ( ERROR_SUCCESS == lRet )
		{
		lRet = RegQueryValueEx (
				hkey, _T("SkipShutdownScan"), NULL,
				NULL, (LPBYTE) &dwSkipScan, &dwBufSize );

		RegCloseKey ( hkey );
		}

	return (BOOL)dwSkipScan;
}

//************************************************************************
// ApwMainWM_DESTROY()
//
// Do cleanup chores in WM_DESTROY of main window.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainWM_DESTROY ( HWND    hWnd )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    // Get rid of tray icon if present.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    pConfig->SetShowIcon( FALSE );
    ApwUpdateTrayIcon();

    // DeInit the command module.

    if ( FALSE == pConfig->GetDoingShutdown() )
    {
        pConfig->SetDoingShutdown( TRUE );

        pCmd->DeInit();
    }

    PostQuitMessage( 0 );
} // ApwMainWM_DESTROY()


//************************************************************************
// ApwMainAPWM_INITDIALOG()
//
// static LRESULT ApwMainAPWM_INITDIALOG (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// This routine is called in response to the APWM_INITDIALOG custom message
// which is a kluged surrogate for a WM_INITDIALOG since our main dialog is
// really a window not a dialog.
// This routine calls all NAVAPW initialization routines.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_INITDIALOG( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    // Set 3d look of dialog.

    ApwDlgSet3d( hWnd );

    if ( FALSE == NewShellPresent() )
    {
        // Force repaint of non-client area on NT 3.51 to see
        // 3d border on a thick frame dialog style main window.
        // At this point, we're already past the first WM_NCPAINT.

        auto    HRGN        hNCRegion;
        auto    RECT        rRect;

        GetWindowRect( hWnd, &rRect );
        hNCRegion = CreateRectRgnIndirect( &rRect );

        SendMessage( hWnd, WM_NCPAINT, ( WPARAM ) hNCRegion, 0 );

        DeleteObject( hNCRegion );
    }

    // Get configuration.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    pConfig->ReadNavopts();

    pConfig->SetState( AP_STATE_LOADING );

    ApwMainDlgUpdate();
    ApwUpdateTrayIcon();

    // Ensure window is painted before doing long init tasks.

    UpdateWindow( hWnd );

    // Intialize command module for service control.

    if ( FALSE == pCmd->Init( hWnd ) )
    {
        //&? Need error message function? a la old NAVAPW?

        auto    APWMESSAGEINFO  rMsgInfo;

        memset( &rMsgInfo, 0, sizeof( rMsgInfo ) );

        rMsgInfo.hInstance = pConfig->GetInstance();
        rMsgInfo.hParent   = hWnd;
        rMsgInfo.uMessage  = IDS_ERR_LOAD_DRIVER;
        rMsgInfo.uTitle    = IDS_APW_CAPTION;

        ApwMessage( &rMsgInfo );
    }

    pConfig->SetState( pCmd->GetStatusOfDriver() );

    ApwMainDlgUpdate();
    ApwUpdateTrayIcon();

    // If started before the shell, set up a timer so we can minimize
    // into the tray once it appears.

    if ( FALSE == ShellTrayPresent() )
    {
        SetTimer( hWnd, TRAY_TIMER_ID, TRAY_TIMER_TIMEOUT, ApwTrayTimer );
    }

    return ( 0 );
} // ApwMainAPWM_INITDIALOG()


//************************************************************************
// ApwMainAPWM_ACTIVATE()
//
// static LRESULT ApwMainAPWM_ACTIVATE (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// This routine restores and activates the NAVAPW main window in
// response to the APWM_ACTIVATE custom message.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_ACTIVATE ( HWND      hWnd,
                                      WPARAM    wParam,
                                      LPARAM    lParam )
{
    ApwActivate();

    return ( 0 );
} // ApwMainAPWM_ACTIVATE()


//************************************************************************
// ApwMainAPWM_SHUTDOWN()
//
// static LRESULT ApwMainAPWM_SHUTDOWN (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// Handle requests from other threads in NAVAPW to shutdown windows.
// Backend implementation for ApwShutdown. Please do not post this
// message directly.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_SHUTDOWN ( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    // Get configuration.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    if ( ( APWM_SHUTDOWN_MAGIC_WPARAM != wParam ) ||
         ( APWM_SHUTDOWN_MAGIC_LPARAM != lParam ) )
    {
        // Someone sent a shutdown message w/o the magic passwords.

        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    // Shutdown Windows.

    if ( FALSE == pConfig->GetDoingShutdown() )
    {
        pConfig->SetDoingShutdown( TRUE );

        ApwReplaceShutdownBitmaps();
        pCmd->DeInit();

        // Klugery: Show and restore main window before shutdown.
        // Main thread yields in ExitWindowsEx() and never wakes up
        // otherwise. Explanation anyone?

        ApwActivate();

        ExitWindowsEx( EWX_FORCE | EWX_SHUTDOWN, 0 );
    }

BailOut:
    return ( 0 );
} // ApwMainAPWM_SHUTDOWN()


//************************************************************************
// ApwMainAPWM_UPDATE_UI()
//
// static LRESULT ApwMainAPWM_UPDATE_UI (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// Handle notification to resync UI with current state of driver.
// Service may send this message if it may have changed the current state
// of the driver.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainAPWM_UPDATE_UI ( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    // Get configuration.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    // Get current status and update UI.

    pConfig->SetState( pCmd->GetStatusOfDriver() );

    ApwMainDlgUpdate();
    ApwUpdateTrayIcon();

    return ( 0 );
} // ApwMainAPWM_UPDATE_UI()


//************************************************************************
// ApwMainAPWM_COM_TASKS()
//
// static LRESULT ApwMainAPWM_COM_TASKS (
//      HWND    hWnd,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// This routine performs com tasks upon requests by the driver's callback.
// It currently processes the items in quarantine's incoming directory.
//
// Returns:
//      0 Always.
//************************************************************************
// 05/27/1998 RSTANEV, created.
//************************************************************************

static LRESULT ApwMainAPWM_COM_TASKS ( HWND      hWnd,
                                      WPARAM    wParam,
                                      LPARAM    lParam )
{
    auto    CApwConfig      *pConfig;
    auto    CApwCmd         *pCmd;
    auto    IQuarantineDLL  *pIQuarantine;

    // Get configuration.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    // Notify the sender of the message that we have received it.

    pCmd->ConfirmApwComTasksReceived();

    // Now we can process the quarantine items.  Note, that this whole thing
    // of sending a message is just to work around a problem, where
    // CoCreateInstance() once used in a thread other than the main APW
    // thread, it causes RPC to hang on shutdown.

    CoInitialize( NULL );

    if ( SUCCEEDED( CoCreateInstance( CLSID_QuarantineDLL,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_QuarantineDLL,
                                      ( LPVOID * ) &pIQuarantine ) ) )
    {
        if ( SUCCEEDED( pIQuarantine->Initialize() ) )
        {
            pIQuarantine->ProcessAllIncomingFiles();
        }

    pIQuarantine->Release();
    }

    CoUninitialize();

    return ( 0 );
} // ApwMainAPWM_COM_TASKS()


//************************************************************************
// ApwReplaceShutdownBitmaps()
//
// This routine replaces the standard Windows shutdown bitmaps with
// NAV's custom virus found bitmaps.
// Should be called when shutting down Windows 95 due to a virus.
//
// Returns:
//  TRUE on success,
//  FALSE otherwise.
//************************************************************************
// 06/16/1997 DALLEE, guts taken from NavHaltSystem() in NAVXUTIL.
//************************************************************************

static BOOL ApwReplaceShutdownBitmaps( VOID )
{
    auto    TCHAR       szWinDir [ MAX_PATH ];

    auto    TCHAR       szNavFile [ MAX_PATH ];
    auto    TCHAR       szWinFile [ MAX_PATH ];
    auto    TCHAR       szSaveFile [ MAX_PATH ];

    auto    TCHAR       szWinInitFile [ MAX_PATH ];

    auto    TCHAR       szName [ MAX_PATH ];

    auto    CApwConfig  *pConfig;
    auto    HINSTANCE   hInstance;
    auto    DWORD       dwResult;
    auto    BOOL        bSuccess;

#ifdef _UNICODE
    // Unicode needs some hoop jumping which I haven't implemented to
    // get the conversion to OEM on WritePrivateProfileString().

    SYM_ASSERT( FALSE );
    return ( FALSE );
#endif

    // Initialize locals.

    bSuccess  = FALSE;
    pConfig   = ApwGetConfig();
    SYM_ASSERT( pConfig );

    hInstance = pConfig->GetInstance();
    SYM_ASSERT( hInstance );

    // Verify Win95 platform.

    if ( VER_PLATFORM_WIN32_WINDOWS != GetWindowsPlatform() )
    {
        goto BailOut;
    }

    // Get Windows dir and construct various filenames which will be used.

    dwResult = GetWindowsDirectory( szWinDir, MAX_PATH );

    if ( ( 0 == dwResult ) ||
         ( MAX_PATH <= dwResult ) )
    {
        goto BailOut;
    }

    // Get WININIT.INI

    if ( 0 == LoadString( hInstance, IDS_WININIT_INI, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szWinInitFile, szWinDir );
    ApwNameCat( szWinInitFile, szName );

    // Get names of shutdown BMPs: NAV, Win95, SaveWin95.

    if ( 0 == LoadString( hInstance, IDS_SHUTDOWN_BANNER_NAV, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szNavFile, szWinDir );
    ApwNameCat( szNavFile, szName );

    if ( 0 == LoadString( hInstance, IDS_SHUTDOWN_BANNER_WIN, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szWinFile, szWinDir );
    ApwNameCat( szWinFile, szName );

    if ( 0 == LoadString( hInstance, IDS_SHUTDOWN_BANNER_SAVE, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szSaveFile, szWinDir );
    ApwNameCat( szSaveFile, szName );

    // Backup the Win95 shutdown BMP file.

    CopyFile( szWinFile, szSaveFile, FALSE );

    // Copy custom NAV BMP over Win95 shutdown BMP.

    CopyFile( szNavFile, szWinFile, FALSE );

    // Update the WININIT.INI to restore the Win95 shutdown BMP
    // and delete the saved BMP.
    // Note that WININIT.INI expects OEM data since it is processed
    // before Windows is properly loaded.

    CharToOem( szWinFile, szWinFile );
    CharToOem( szSaveFile, szSaveFile );

    // Get Wininit.ini rename section name.

    if ( 0 == LoadString( hInstance, IDS_WININIT_RENAME, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;

    // Restore Win95 BMP.

    WritePrivateProfileString( szName,
                               szWinFile,
                               szSaveFile,
                               szWinInitFile );

    // Delete saved copy.

    WritePrivateProfileString( szName,
                               szSaveFile,
                               _T(""),
                               szWinInitFile );


    // Get names of wait for shutdown BMPs: NAV, Win95, SaveWin95.

    if ( 0 == LoadString( hInstance, IDS_SHUTDOWN_WAIT_NAV, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szNavFile, szWinDir );
    ApwNameCat( szNavFile, szName );

    if ( 0 == LoadString( hInstance, IDS_SHUTDOWN_WAIT_WIN, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szWinFile, szWinDir );
    ApwNameCat( szWinFile, szName );

    if ( 0 == LoadString( hInstance, IDS_SHUTDOWN_WAIT_SAVE, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;
    _tcscpy( szSaveFile, szWinDir );
    ApwNameCat( szSaveFile, szName );

    // Backup the Win95 wait for shutdown BMP file.

    CopyFile( szWinFile, szSaveFile, FALSE );

    // Copy custom NAV BMP over Win95 wait for shutdown BMP.

    CopyFile( szNavFile, szWinFile, FALSE );

    // Update the WININIT.INI to restore the Win95 wait for shutdown BMP
    // and delete the saved BMP.
    // Note that WININIT.INI expects OEM data since it is processed
    // before Windows is properly loaded.

    CharToOem( szWinFile, szWinFile );
    CharToOem( szSaveFile, szSaveFile );

    // Get Wininit.ini rename section name.

    if ( 0 == LoadString( hInstance, IDS_WININIT_RENAME, szName, sizeof( szName ) / sizeof( TCHAR ) ) )
        goto BailOut;

    // Restore Win95 BMP.

    WritePrivateProfileString( szName,
                               szWinFile,
                               szSaveFile,
                               szWinInitFile );

    // Delete saved copy.

    WritePrivateProfileString( szName,
                               szSaveFile,
                               _T(""),
                               szWinInitFile );

    bSuccess = TRUE;

BailOut:
    return ( bSuccess );
} // ApwReplaceShutdownBitmaps()


//************************************************************************
// ApwMainDefaultProc()
//
// static LRESULT ApwMainDefaultProc (
//      HWND    hWnd,                   // Main window.
//      UINT    uMessage,               // Message ID
//      WPARAM  wParam,
//      LPARAM  lParam )
//
// This routine checks whether an unhandled message is one of our
// registered messages for communication with other NAV executables and
// makes the appropriate response.
// If not registered, the message is passed on to DefDlgProc().
//
// Returns:
//      Depends on message.
//************************************************************************
// 06/10/1997 DALLEE, created.
//************************************************************************

static LRESULT ApwMainDefaultProc ( HWND    hWnd,
                                    UINT    uMessage,
                                    WPARAM  wParam,
                                    LPARAM  lParam )
{
    auto    LRESULT     lResult;

    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    // Get configuration.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    SYM_ASSERT( pConfig );
    SYM_ASSERT( pCmd );

    // Custom message used in communication with NAVW.

    if ( pConfig->GetCustomMessageID() == uMessage )
    {
        lResult = TRUE;

        switch ( wParam )
        {
        case NAVW_CMD_SHOWHIDEICON:
            pConfig->SetShowIcon( lParam ? TRUE : FALSE );
            ApwUpdateTrayIcon();
            break;

        case NAVW_CMD_CANBEDISABLED:
            pConfig->SetCanDisable( lParam ? TRUE : FALSE );
            ApwMainDlgUpdate();
            break;

        case NAVW_CMD_UNLOADNAVAPW:
            pCmd->DisableDriver();
            DestroyWindow( hWnd );
            break;

        default:
            // Somebody sent a seemingly bogus message.
            // Is this something new that needs to be handled?

            SYM_ASSERT( FALSE );
            break;
        }
    }
    else
    {
        lResult = DefDlgProc( hWnd, uMessage, wParam, lParam );
    }

    return ( lResult );
} // ApwMainDefaultProc()


//************************************************************************
// ApwMainDlgUpdate()
//
// static VOID ApwMainDlgUpdate ( VOID );
//
// This updates the main dialog controls based on the current state of
// the driver.
// Note that this routine uses the current value in the config object.
// It does not directly query the driver, nor set the config value.
//
// Returns:
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

static VOID ApwMainDlgUpdate ( VOID )
{
    #define MAX_BUTTON_TEXT 80
    #define MAX_STATUS_TEXT 200

    auto    CApwConfig      *pConfig;
    auto    HWND            hWnd;
    auto    HINSTANCE       hInst;

    auto    TCHAR           szText [ max( MAX_BUTTON_TEXT, MAX_STATUS_TEXT ) + 1 ];
    auto    BOOL            bEnableDisableActive;
    auto    UINT            uEnableID;
    auto    UINT            uStatusID;
    auto    UINT            uIcon;
    auto    HICON           hIcon;

    // Initialize locals.

    pConfig = ApwGetConfig();
    hWnd    = pConfig->GetMainWnd();
    hInst   = pConfig->GetInstance();

    // Get enabled/disabled UI ID's.

    switch ( pConfig->GetState() )
    {
    case AP_STATE_ENABLED:
        uEnableID = IDS_MAIN_DISABLE_BUTTON;
        uStatusID = IDS_MAIN_STATUS_ENABLED;
        uIcon     = IDI_APW_APP_ENABLED;
        bEnableDisableActive = pConfig->GetCanDisable();
        break;

    case AP_STATE_LOADING:
        uEnableID = IDS_MAIN_DISABLE_BUTTON;
        uStatusID = IDS_MAIN_STATUS_LOADING;
        uIcon     = IDI_APW_APP_ENABLED;
        bEnableDisableActive = FALSE;
        break;

    case AP_STATE_DISABLED:
    default:
        uEnableID = IDS_MAIN_ENABLE_BUTTON;
        uStatusID = IDS_MAIN_STATUS_DISABLED;
        uIcon     = IDI_APW_APP_DISABLED;
        bEnableDisableActive = TRUE;
        break;
    }

    // Set Enable/Disable button text.
    // Also disable "Disable" button according to options.

    if ( 0 != LoadString( hInst,
                          uEnableID,
                          szText,
                          sizeof( szText ) / sizeof( TCHAR ) ) )
    {
        SetDlgItemText( hWnd, IDC_MAIN_ENABLE, szText );
    }

    EnableWindow( GetDlgItem( hWnd, IDC_MAIN_ENABLE ), bEnableDisableActive );

    // Set main status text.

    if ( 0 != LoadString( hInst,
                          uStatusID,
                          szText,
                          sizeof( szText ) / sizeof( TCHAR ) ) )
    {
        SetDlgItemText( hWnd, IDC_MAIN_STATUS, szText );
    }

    // Kluge to make icons repaint in NT 3.51. Set icons to NULL
    // before doing InvalidateRect().

    SetClassLong( hWnd, GCL_HICON, ( LONG ) NULL );
    SetClassLong( hWnd, GCL_HICONSM, ( LONG ) NULL );

    InvalidateRect( hWnd, NULL, FALSE );

    // Set large icon.

    hIcon = LoadIcon( hInst, MAKEINTRESOURCE( uIcon ) );

    Static_SetIcon( GetDlgItem( hWnd, IDC_MAIN_ICON), hIcon );
    SetClassLong( hWnd, GCL_HICON, ( LONG ) hIcon );
    pConfig->SetIcon( hIcon );

    // Set small icon.

    hIcon = ( HICON ) LoadImage ( hInst,
                                  MAKEINTRESOURCE( uIcon ),
                                  IMAGE_ICON,
                                  16, 16, 0 );

    SetClassLong( hWnd, GCL_HICONSM, ( LONG ) hIcon );
    pConfig->SetSmallIcon( hIcon );
} // ApwMainDlgUpdate()


//************************************************************************
// ApwTrayTimer()
//
// static VOID CALLBACK ApwTrayTimer (
//      HWND     hWnd,                  // Main window
//      UINT     uMessage,              // WM_TIMER
//      UINT     uId,                   // Timer ID
//      DWORD    dwTime )               // Current system time
//
// This is the callback routine for the timer which waits for the
// shell tray window to appear.
// If NAVAPW is started before the shell and the tray window are present,
// a timer is started and this routine is called to check for the
// tray and add our icon and fix the minimize/hide state of the main
// window.
//
// Returns:
//************************************************************************
// 06/11/1997 DALLEE, created.
//************************************************************************

static VOID CALLBACK ApwTrayTimer( HWND     hWnd,
                                   UINT     uMessage,
                                   UINT     uId,
                                   DWORD    dwTime )
{
    // Check if shell tray has appeared yet.

    if ( ShellTrayPresent() )
    {
        // Update tray icon.

        ApwUpdateTrayIcon();

        // If main window was minimized, hide instead now that tray is present.

        if ( IsIconic( hWnd ) )
        {
            ApwMinimize();
        }

        KillTimer( hWnd, uId );
    }
} // ApwTrayTimer()


#ifdef NAV_PREVIEW
//************************************************************************
// IsTrialValid()
//
// static BOOL IsTrialValid(
//      HINSTANCE hInstance )           // This apps instance handle
//
// This function check to see if our Trial period is currently valid.
// If not the Vxd is disabled.
//
// Returns: TRUE if the trial period is currently valid -
//          FALSE otherwise
//
//************************************************************************
// 07/11/1997 MKEATIN, created.
//************************************************************************

static BOOL IsTrialValid( HINSTANCE hInstance )
{

    auto    CApwConfig  *pConfig;
    auto    CApwCmd     *pCmd;

    auto    BOOL        bExpired;
    auto    ITrialWare  *pTrial;


    // Initialize locals.

    bExpired = TRUE;

    // Get configuration.

    pConfig = ApwGetConfig();
    pCmd    = pConfig->GetCmd();

    SYM_ASSERT( pConfig );
    SYM_ASSERT( pCmd );

    // Initalize COM

    CoInitialize(NULL);

    // Create trial object

    if ( SUCCEEDED( CoCreateInstance( CLSID_TrialWare,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_ITrialWare,
                                      (LPVOID*)&pTrial )))
    {
        // Initialize trial object

        if ( SUCCEEDED( pTrial->Initialize( TEXT( "Symantec" ),
                                            TEXT( "Norton AntiVirus" ),
                                            NAV_VERSION_MAJOR,
                                            NAV_VERSION_MINOR )))
        {
            if ( pTrial->IsValid( HWND_DESKTOP, TRUE, NULL ) == S_OK )
            {
                bExpired = FALSE;
            }
            else
            {
                auto    TCHAR   szText [ 1024 ];
                auto    TCHAR   szFormat [1024];
                auto    TCHAR   szCaption [ 100 ];
                DWORD dwTrialLength = 30, dwBufSize = sizeof(DWORD);
                LONG  lRet;
                HKEY  hkey;

                pCmd->Init( NULL );
                pCmd->DisableDriver();
                pCmd->DeInit();

                LoadString (hInstance, IDS_LICENSEEXPIRED, szFormat, sizeof( szFormat ) / sizeof( TCHAR ) - 1 );
                LoadString (hInstance, IDS_APW_CAPTION,    szCaption, sizeof( szCaption ) / sizeof( TCHAR ) - 1 );

                // Read in the trial period length for use in the message.
                lRet = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, 
                                      _T("software\\Symantec\\Norton AntiVirus"),
                                      0, KEY_QUERY_VALUE, &hkey );

                if ( ERROR_SUCCESS == lRet )
                     {
                     lRet = RegQueryValueEx (
                                hkey, _T("TrialLength"), NULL,
                                NULL, (LPBYTE) &dwTrialLength, &dwBufSize );

                     RegCloseKey ( hkey );
                     }

                wsprintf ( szText, szFormat, dwTrialLength );

                MessageBox( NULL, szText, szCaption, MB_OK | MB_ICONINFORMATION );
            }
        }
        pTrial->Release();
    }

    // Balance call to CoInitalize;

    CoUninitialize();

    return ( !bExpired );
} // IsTrialValid()

#endif  // NAV_PREVIEW



