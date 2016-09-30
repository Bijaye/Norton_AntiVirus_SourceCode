// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUTIL/VCS/apwutil.cpv   1.9   01 Oct 1998 13:04:50   tcashin  $
//
// Description:
//      Source for NAVAPW utility functions.
//
// Contains:
//      ApwActivate()
//      ApwDlgSet3d()
//      ApwMinimize()
//      ApwUpdateUI()
//      ApwShutdown()
//      ShellTrayPresent()
//      NewShellPresent()
//      ApwMinimize()
//      ApwReadNavopts()
//      ApwCenterWindow()
//      ApwNotifyNavw()
//      ApwActivateNavw()
//      ApwGetCurrentNavWindow()
//      ApwGetNavwExePath()
//      GetWindowsPlatform()
//
// See Also:
//************************************************************************
// $Log:   S:/APWUTIL/VCS/apwutil.cpv  $
// 
//    Rev 1.9   01 Oct 1998 13:04:50   tcashin
// Put back the check for Atomics navopts version number.
// 
//    Rev 1.8   23 Sep 1998 17:25:14   tcashin
// Bumped the navopts version number for Barstow.
// 
//    Rev 1.7   31 Jul 1998 10:11:32   tcashin
// Update the NAVOPTS.DAT version number for Atomic.
// 
//    Rev 1.6   13 Mar 1998 17:30:34   tcashin
// Added ApwActivateNavw, ApwGetCurrentNavWindow, and ApwGetNavwExePath
// to support AP's launching and interaction with Navw.
// 
//    Rev 1.5   11 Aug 1997 17:05:46   DALLEE
// Added magic parameters to APWM_SHUTDOWN message to protect against
// accidentally receiving and this message and shutting down Windows.
// Added ApwUpdateUI() to signal UI to resync dispalyed status of the driver.
// 
//    Rev 1.4   27 Jun 1997 19:21:32   DALLEE
// Protect ApwNotifyNavw() against re-entrance.
// Also updated some comments.
// 
//    Rev 1.3   20 Jun 1997 11:32:48   DBUCHES
// Bumped NAVOPTS header version.  Now uses NAV_VERSION_97.
// 
//    Rev 1.2   11 Jun 1997 12:09:26   DALLEE
// Added GetWindowsPlatform() and ApwNotifyNavw().
// 
//    Rev 1.1   05 Jun 1997 19:43:18   DALLEE
// In ApwCenterWindow(), replaced SetWindowPlacement() with SetWindowPos().
// SetWindowPlacement() had strange behaviour when used on main window
// launched minimized and not yet displayed.
// 
//    Rev 1.0   02 Jun 1997 02:48:36   DALLEE
// Initial revision.
//************************************************************************

#define STRICT

#include "platform.h"
#include <windowsx.h>
#include <tchar.h>
#include <ctl3d.h>

#include "apw.h"
#include "apwutil.h"
#include "apwcfg.h"

#include "linkcli.h"
#include "navrpc.h"

#include "resource.h"


//************************************************************************
// Local prototypes
//************************************************************************

static BOOL CALLBACK ApwDlgSet3dEnum ( HWND hWnd, LPARAM lParam );
static BOOL CALLBACK ApwNavwThrdWinEnum ( HWND hWnd, LPARAM lParam );


//************************************************************************
// Defines
//************************************************************************
#define     SZ_NAVW_WNDCLASS    _T("nav")
#define     SZ_SCAN_WNDCLASS    _T("ScanWindow")


//************************************************************************
// Globals
//************************************************************************

#ifdef __cplusplus
extern "C"  {
#endif

HINSTANCE g_hUtilInst;

#ifdef __cplusplus
}
#endif

static  CRITICAL_SECTION    s_CritSecRpcClient;


//************************************************************************
// DLL junk
//************************************************************************

//************************************************************************
// DllMain()
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/27/1997 DALLEE, created.
//************************************************************************

BOOL WINAPI DllMain ( HINSTANCE   hInstDLL,
                      DWORD       dwReason,
                      LPVOID      lpvReserved )
{
    switch ( dwReason )
    {
    case DLL_PROCESS_ATTACH:
        // Save HINSTANCE on process attach.

        g_hUtilInst = hInstDLL;

        // Critical section for RPC communication to NAVW.

        InitializeCriticalSection( &s_CritSecRpcClient );
        break;

    case DLL_PROCESS_DETACH:
        // Critical section for RPC communication to NAVW.

        DeleteCriticalSection( &s_CritSecRpcClient );
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }

    return ( TRUE );
} // DllMain()


//************************************************************************
// Utiltiy functions
//************************************************************************

//************************************************************************
// ApwDlgSet3d()
//
// APWUTILAPI BOOL WINAPI ApwDlgSet3d (
//      HWND    hWnd );                 // Dialog to give 3d look.
//
// This routine will first check if the new Windows 4.0 GUI is present.
// If not (as in NT 3.51), it will enumerate all child dialog controls 
// looking for new style groupboxes and sunken static controls to give them
// a 3d look, then call Ctl3dSubclassDlgEx().
//
// Returns:
//      TRUE    on success.
//      FALSE   otherwise.
//************************************************************************
// 05/23/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwDlgSet3d ( HWND hWnd )
{
    auto    BOOL    bSuccess;

    if ( NewShellPresent() )
    {
        // New style GUI is present, no need to do anything.

        bSuccess = TRUE;
    }
    else
    {
        // Need to add SS_BLACKFRAME to controls and subclass dialog for 3d.

        bSuccess = EnumChildWindows( hWnd, ApwDlgSet3dEnum, 0 );
#ifdef _X86_
        Ctl3dSubclassDlgEx( hWnd, CTL3D_ALL );
#endif
    }

    return ( bSuccess );
} // ApwDlgSet3d()

static BOOL CALLBACK ApwDlgSet3dEnum ( HWND hWnd, LPARAM lParam )
{
    const   TCHAR   szStaticClass [] = _T("Static");
    const   TCHAR   szButtonClass [] = _T("Button");

    #define MAX_CLASS   ( max( sizeof(szStaticClass), sizeof(szButtonClass) ) / sizeof(TCHAR) + 1 )

    auto    DWORD   dwStyle;
    auto    TCHAR   szClass [ MAX_CLASS ];

    auto    RECT    rRect;
    auto    POINT   rPoint;
    auto    HWND    hParent;

    dwStyle = GetWindowStyle( hWnd );
    GetClassName( hWnd, szClass, sizeof( szClass ) / sizeof( TCHAR ) );

    // Set SS_BLACKFRAME for groupboxes.

    if ( ( BS_GROUPBOX == ( dwStyle & BS_GROUPBOX ) ) &&
         ( 0 == _tcsicmp( szButtonClass, szClass ) ) )
    {
        dwStyle |= SS_BLACKFRAME;
        SetWindowLong( hWnd, GWL_STYLE, dwStyle );
    }

    // For sunken static controls, need to create another child window with
    // a sunken border. ( Setting SS_BLACKFRAME on a static control would create
    // a sunken frame with no text displayed )

    if ( ( 0 == _tcsicmp( szStaticClass, szClass ) ) &&
         ( SS_SUNKEN == ( dwStyle & SS_SUNKEN ) ) )
    {
        hParent = GetParent( hWnd );

        GetWindowRect( hWnd, &rRect );

        // Klugery: make new frame larger by one unit per side
        // so we don't draw right against the text of the old control.

        if ( rRect.left )
            rRect.left--;
        if ( rRect.top )
            rRect.top--;
        rRect.right++;
        rRect.bottom++;

        rPoint.x = rRect.left;
        rPoint.y = rRect.top;

        ScreenToClient( GetParent( hWnd ), &rPoint );

        CreateWindow( szStaticClass,
                      _T(""),
                      WS_CHILD | WS_VISIBLE | SS_BLACKFRAME,
                      rPoint.x, rPoint.y,
                      rRect.right - rRect.left, rRect.bottom - rRect.top,
                      GetParent( hWnd ),
                      ( HMENU ) -1,
                      GetWindowInstance( hWnd ),
                      NULL );
    }

    return ( TRUE );
} // ApwDlgSet3dEnum()


//************************************************************************
// ShellTrayPresent()
//
// This routine returns whether or not the new shell tray window is
// currently present.
// Note that NAVAPW may start before the shell, so during execution
// the result of this function may change. Do not cache the result.
//
// Returns:
//  TRUE    if tray is currently present.
//  FALSE   otherise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ShellTrayPresent ()
{
    return ( NULL != FindWindow( "Shell_TrayWnd", NULL ) );
} // ShellTrayPresent()


//************************************************************************
// NewShellPresent()
//
// This routine returns whether or not the updated GUI from Windows
// versions >= 4.0 is present.
//
// Returns:
//  TRUE    if new style GUI is present.
//  FALSE   otherise.
//************************************************************************
// 05/22/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI NewShellPresent ()
{
    auto    OSVERSIONINFO   rVersionInfo;
    auto    BOOL            bNewShell;
    
    // Initialize locals.

    bNewShell = FALSE;

    memset( &rVersionInfo, 0, sizeof( rVersionInfo ) );
    rVersionInfo.dwOSVersionInfoSize = sizeof( rVersionInfo );

    // See if we have Windows version 4.0 or greater.

    if ( GetVersionEx( &rVersionInfo ) )
    {
        bNewShell = rVersionInfo.dwMajorVersion >= 4;
    }

    return ( bNewShell );
} // NewShellPresent()


//************************************************************************
// GetWindowsPlatform()
//
// APWUTILAPI DWORD WINAPI GetWindowsPlatform ()
//
// This routine returns the Windows platform ID.
//
// Returns:
//  VER_PLATFORM_WIN32s	                Win32s on Windows 3.1. 
//  VER_PLATFORM_WIN32_WINDOWS	        Win32 on Windows 95.
//  VER_PLATFORM_WIN32_NT	            Win32 on Windows NT.
//  -1 on error (unexpected).
//************************************************************************
// 06/11/1997 DALLEE, created.
//************************************************************************

APWUTILAPI DWORD WINAPI GetWindowsPlatform ()
{
    auto    OSVERSIONINFO   rVersionInfo;
    auto    DWORD           dwPlatform;
    
    // Initialize locals.

    dwPlatform = ( DWORD ) -1;

    memset( &rVersionInfo, 0, sizeof( rVersionInfo ) );
    rVersionInfo.dwOSVersionInfoSize = sizeof( rVersionInfo );

    // Retrieve platform ID from version info.

    if ( GetVersionEx( &rVersionInfo ) )
    {
        dwPlatform = rVersionInfo.dwPlatformId;
    }

    return ( dwPlatform );
} // GetWindowsPlatform()


//************************************************************************
// ApwActivate()
//
// APWUTILAPI BOOL WINAPI ApwActivate ()
//
// This routine restores and activates the NAVAPW main window.
//
// This restores the current process's main window -- activation of
// previous instances is handled through FindWindow() and
// PostMessage( APWM_ACTIVATE ).
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwActivate ()
{
    auto    CApwConfig  *pConfig;
    auto    HWND        hMainWnd;

    // Get main window.

    pConfig = ApwGetConfig();
    hMainWnd = pConfig->GetMainWnd();

    // Restore window and put in foreground.

    ShowWindow( hMainWnd, SW_RESTORE );

    return ( SetForegroundWindow( hMainWnd ) );
} // ApwActivate()


//************************************************************************
// ApwActivateNavw()
//
// APWUTILAPI BOOL WINAPI ApwActivateNavw ()
//
// This launches or restores the NAVW -- activation of previous
// instances is handled through FindWindowEx and GetLastActivePopup()
//
// See warning re: bug in GetLastActivePopup()
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 03/10/1998 TCASHIN, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwActivateNavw ()
{
    auto    TCHAR               szNavwExePath [SYM_MAX_PATH];
    auto    TCHAR               szNavwDir [SYM_MAX_PATH];
    auto    HWND                hNavwWnd  = NULL;
    auto    HWND                hCurrWnd  = NULL;
    auto    SHELLEXECUTEINFO    NavwStartInfo = {0};

    // Look for Navw's Main window
    if ( ApwGetCurrentNavWindow((LPHANDLE)&hNavwWnd, (LPHANDLE)&hCurrWnd) )
    {
        // Open it if there is one.
        ShowWindow( hCurrWnd, SW_RESTORE );
        return ( SetForegroundWindow( hCurrWnd ) );
    }
	else
	{
        // Start Navw using ShellExecuteEx()
        ApwGetNavwExePath ( szNavwExePath, szNavwDir );

        NavwStartInfo.cbSize = sizeof(NavwStartInfo);
        NavwStartInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;
        NavwStartInfo.lpVerb = "Open";
        NavwStartInfo.lpFile = szNavwExePath;
        NavwStartInfo.nShow  = SW_SHOW;

        return ( ShellExecuteEx(&NavwStartInfo) );
	}

} // ApwActivateNavw()


//************************************************************************
// ApwGetCurrentNavWindow()
//
// APWUTILAPI BOOL WINAPI MainDlgGetCurrentNavWindow (lpMainWindow, lpCurrentWindow)
//
// This return the handle of Navw's main window and
// the handle of the currently open window (dialog, etc.).
//
// See warning re: bug in GetLastActivePopup()
//
// Returns:
//      TRUE if any Navw windows are open
//      FALSE otherwise.
//************************************************************************
// 03/10/1998 TCASHIN, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwGetCurrentNavWindow (LPHANDLE lpMainWindow, 
                                               LPHANDLE lpCurrentWindow)
{
    auto    HWND    hNavwMainWnd  = NULL;
    auto    HWND    hNavwScanWnd  = NULL;
    auto    HWND    hCurNavwPopupWnd = NULL;
    auto    BOOL    bResult = FALSE;

    // Look for Navw's Main window
    hNavwMainWnd = ::FindWindowEx(NULL,NULL, SZ_NAVW_WNDCLASS, NULL);
    
    if ( hNavwMainWnd == NULL )
        return ( bResult );
    else
    {
        // We have a Navw main window.
        *lpMainWindow = hNavwMainWnd;
        bResult = TRUE;

        // Look for Navw's interesting windows. 
        hCurNavwPopupWnd = GetLastActivePopup ( hNavwMainWnd );
        *lpCurrentWindow = hCurNavwPopupWnd;

        // Warning ... Warning ... Warning
        // 
        // This is here to work around a bug in GetLastActivePopup(). If the
        // last active popup is currently minimized then the HWND returned is 
        // the parent.
        //
        // This only happens in the case of ScanWindow! So look at the windows owned
        // by the current thread until ScanWindow is found.
        //
        // This really sucks...
        //

        if ( hCurNavwPopupWnd == hNavwMainWnd )
        {
            ::EnumThreadWindows ( GetWindowThreadProcessId(hNavwMainWnd,NULL), 
                                  ApwNavwThrdWinEnum, 
                                  MAKELPARAM(LOWORD(&hNavwScanWnd), HIWORD(&hNavwScanWnd)));

            if ( hNavwScanWnd )
            {
                *lpCurrentWindow = hNavwScanWnd;
            }
        }

        return ( bResult );

    }
} // ApwGetCurrentNavWindow()


//************************************************************************
// ApwNavwThrdWinEnum()
//
// static BOOL CALLBACK ApwNavwThrdWinEnum ( HWND hWnd, LPARAM lParam )
//
// This checks to see if a given window handle is our ScanWindow
// class. If so, return the HWND to the caller.
//
// Returns:
//      TRUE on to continue Window enumeration
//      FALSE to stop enumerating.
//************************************************************************
// 03/10/1998 TCASHIN, created.
//************************************************************************

static BOOL CALLBACK ApwNavwThrdWinEnum ( HWND hWnd, LPARAM lParam )
{
    auto    TCHAR            szClass [ SYM_MAX_PATH ];
    auto    HWND            *lphScanWnd;

    GetClassName( hWnd, szClass, sizeof( szClass ) / sizeof( TCHAR ) );

    if ( 0 == _tcsicmp( SZ_SCAN_WNDCLASS, szClass ) )
    {
    	lphScanWnd = (HWND *)MAKELP(HIWORD(lParam), LOWORD(lParam));
        *lphScanWnd = hWnd;        
        return (FALSE);
    }
    else
        return (TRUE);

} //ApwNavwThrdWinEnum


//************************************************************************
// ApwShutdown()
//
// APWUTILAPI BOOL WINAPI ApwShutdown ()
//
// This routine tells NAVAPW to signal a shutdown. It may be called at any
// from any thread.
//
// Currently implemented as a PostMessage() to NAVAPW's main window.
//
// Returns:
//      TRUE if message was sent (doesn't guarantee shutdown will occur).
//      FALSE if message was not sent.
//************************************************************************
// 05/18/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwShutdown ()
{
    auto    CApwConfig  *pConfig;
    auto    HWND        hMainWnd;

    // Get main window.

    pConfig = ApwGetConfig();
    hMainWnd = pConfig->GetMainWnd();

    // Send message to have NAVAPW shut down windows.
    // Klugey use of PostMessage() will allow current thread to finish
    // processing AP callback if called from DriverComm ServiceCallback().
    // Need to modify this to truly separate AP service from UI.

    return ( PostMessage( hMainWnd,
                          APWM_SHUTDOWN,
                          APWM_SHUTDOWN_MAGIC_WPARAM,
                          APWM_SHUTDOWN_MAGIC_LPARAM ) );
} // ApwShutdown()


//************************************************************************
// ApwMinimize()
//
// APWUTILAPI BOOL WINAPI ApwMinimize ()
//
// This routine minimizes or hides the NAVAPW main window based on
// current options and presence of shell tray window.
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 05/28/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwMinimize ()
{
    auto    CApwConfig  *pConfig;
    auto    int         nCmdShow;

    // Get main window.

    pConfig = ApwGetConfig();

    // Figure out what to do with main window.

    if ( !ShellTrayPresent() && pConfig->GetShowIcon() )
    {
        nCmdShow = SW_MINIMIZE;
    }
    else
    {
        nCmdShow = SW_HIDE;
    }

    return ( ShowWindow( pConfig->GetMainWnd(), nCmdShow ) );
} // ApwMinimize()


//************************************************************************
// ApwCenterWindow()
//
// APWUTILAPI BOOL WINAPI ApwCenterWindow (
//      HWND    hWnd,                   // Window to center
//      HWND    hParent );              // Parent window to center on,
//                                      //   NULL to center on desktop.
//
// Yes, yet another instance of window centering code.
//
// This routine centers a window relative to its parent. If there is no
// parent, or the parent is hidden or minimized, the window will be 
// centered on the desktop.
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 05/28/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwCenterWindow ( HWND hWnd, HWND hParent )
{
    auto    RECT            rWindow;
    auto    RECT            rParent;
    auto    LONG            lWidth;
    auto    LONG            lHeight;

    auto    BOOL            bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    memset( &rWindow, 0, sizeof( rWindow ) );

    // Find position of parent window.

    if ( ( NULL == hParent ) ||
         ( IsIconic( hParent ) ) ||
         ( FALSE == IsWindowVisible( hParent ) ) )
    {
        // If no parent or parent is hidden or invisible, center on desktop.

        hParent = GetDesktopWindow();
    }

    if ( FALSE == GetWindowRect( hParent, &rParent ) )
        goto BailOut;

    // Find size of window to center.

    if ( FALSE == GetWindowRect( hWnd, &rWindow ) )
        goto BailOut;

    lWidth  = rWindow.right  - rWindow.left;
    lHeight = rWindow.bottom - rWindow.top;

    // New postion is ( center of parent ) - ( 1/2 size of window )

    rWindow.left = ( ( rParent.left + rParent.right  ) - ( lWidth  ) ) / 2;
    rWindow.top  = ( ( rParent.top  + rParent.bottom ) - ( lHeight ) ) / 2;

    if ( 0 > rWindow.left )
        rWindow.left = 0;

    if ( 0 > rWindow.top )
        rWindow.top = 0;

    // Move that window.

    bSuccess = SetWindowPos( hWnd, NULL, 
                             rWindow.left,
                             rWindow.top,
                             lWidth,
                             lHeight,
                             SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW );

BailOut:
    return ( bSuccess );
} // ApwCenterWindow()


//************************************************************************
// ApwReadNavopts()
//
// This routine loads the relevent auto protect options from NAVOPTS.DAT.
//
// Returns:
//  TRUE    on success.
//  FALSE   otherwise.
//************************************************************************
// 05/31/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwReadNavopts( _NAVOPTIONS *pNavopts )
{
    auto    CApwConfig      *pConfig;

    auto    TCHAR           szNavoptsPath [ MAX_PATH ];
    auto    TCHAR           szNavoptsName [ MAX_PATH ];
    auto    HANDLE          hNavopts;

    auto    DWORD           dwBytesRead;
    auto    UINT            uOldErrorMode;
    auto    BOOL            bSuccess;

#include <pshpack1.h>
    auto    struct  { NAVBINHEADER  rHeader;
                      _NAVOPTIONS   rNavopts; } rNavoptsDat;
#include <poppack.h>

    // Initialize locals.

    hNavopts = INVALID_HANDLE_VALUE;
    uOldErrorMode = SEM_FAILCRITICALERRORS;
    bSuccess = FALSE;

    pConfig = ApwGetConfig();

    // Get name of NAVOPTS.DAT file.

    if ( ( 0 == GetModuleFileName( pConfig->GetInstance(),
                                   szNavoptsPath, 
                                   sizeof( szNavoptsPath ) / sizeof( TCHAR ) ) ) ||
         ( 0 == LoadString( g_hUtilInst, 
                            IDS_NAVOPTS_DAT, 
                            szNavoptsName,
                            sizeof( szNavoptsName ) / sizeof( TCHAR ) ) ) )
    {
        goto BailOut;
    } 

    ApwNameStrip( szNavoptsPath );
    ApwNameCat( szNavoptsPath, szNavoptsName );

    // Open NAVOPTS.DAT

    uOldErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );

    hNavopts = CreateFile( szNavoptsPath,
                           GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL );

    if ( INVALID_HANDLE_VALUE == hNavopts )
        goto BailOut;

    // Read NAVOPTS data.

    if ( ( FALSE == ReadFile( hNavopts, 
                              &rNavoptsDat,
                              sizeof( rNavoptsDat ),
                              &dwBytesRead,
                              NULL ) ) ||
         ( sizeof( rNavoptsDat ) != dwBytesRead ) )
    {
        goto BailOut;
    }

    if ( NAV_VERSION_50 != rNavoptsDat.rHeader.dwVersion )
    {
        // Assertion is only here to signal cases when the version number
        // changes during development. This is still a valid run time error 
        // and we must handle it properly.

        SYM_ASSERT( FALSE );
        goto BailOut;
    }

    bSuccess = TRUE;

    // Store options.

    memcpy( pNavopts, &rNavoptsDat.rNavopts, sizeof( *pNavopts ) );

BailOut:
    if ( INVALID_HANDLE_VALUE != hNavopts )
        CloseHandle( hNavopts );

    if ( SEM_FAILCRITICALERRORS != uOldErrorMode )
        SetErrorMode( uOldErrorMode );

    return ( bSuccess );
} // ApwReadNavopts()


//************************************************************************
// ApwNotifyNavw()
//
// APWUTILAPI BOOL WINAPI ApwNotifyNavw ( 
//      DWORD    dwMessage );           // Message to send
//
// This routine sends a command message to NAVW.
// Currently implemented as a link to NAVW's RPC server.
//
// Possible commands/messages are:
//      UWM_NAVAP_ACTIVATING
//
// Returns:
//  TRUE    on success.
//  FALSE   otherwise.
//************************************************************************
// 06/11/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwNotifyNavw ( DWORD dwMessage )
{
    auto    CLinkClient         client;
    auto    BOOL                bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    // Protect against re-entrancy -- CLinkClient utilizes some
    // statics for RPC binding info.

    EnterCriticalSection( &s_CritSecRpcClient );

    // Connect to NAVW's RPC server. Address empty for local machine.

    if ( ( client.SetProtocol( NAVW_PROTOCOL,
                               _T(""),
                               NAVW_ENDPOINT ) ) &&
         ( client.Connect() ) )
    {
        // Send command message.

        if ( client.SendCommand( dwMessage ) )
        {
            bSuccess = TRUE;
        }

        client.DisConnect();
    }

    LeaveCriticalSection( &s_CritSecRpcClient );

    return ( bSuccess );
} // ApwNotifyNavw()


//************************************************************************
// ApwUpdateUI()
//
// APWUTILAPI BOOL WINAPI ApwUpdateUI ()
//
// This routine notifies the NAVAPW UI to resync its UI with the current
// state of the driver.
// Currently used after the service portion does an automatic reload
// of definitions (on error the UI needs to reflect a disabled state).
//
// Returns:
//  TRUE    if message sent.
//  FALSE   on error - agent not present or couldn't send message..
//************************************************************************
// 08/11/1997 DALLEE, created.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwUpdateUI ( )
{
    auto    HWND        hWnd;
    auto    BOOL        bSuccess;

    // Initialize locals.

    bSuccess = FALSE;

    // Find Agent window.

    hWnd = FindWindow( APW_CLASS_NAME, NULL );

    // Send update message.

    if ( NULL != hWnd )
    {
        bSuccess = PostMessage( hWnd, APWM_UPDATE_UI, 0, 0 );
    }

    return ( bSuccess );
} // ApwUpdateUI()


//************************************************************************
// ApwGetNavwExePath
//
// Purpose:
//    Set lpszNavw to the full path to NAVW32/NAVWNT based on platform
//
// Parameters:
//    lpszNavw  [out] - The full path to NAV.
//    lpszDir   [out] - The directory portion of lpszNavw.
//
//************************************************************************
// 05/28/1997 JBRENNA - Function Created
// 03/12/1998 TCASHIN - Moved from ApwNtCmd.cpp
//************************************************************************

APWUTILAPI void WINAPI ApwGetNavwExePath ( LPSTR lpszNavw, LPSTR lpszDir )
{
    LPCTSTR    szNavNTKey   = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVWNT.EXE");
    LPCTSTR    szNav95Key   = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVW32.EXE");
    LPCTSTR    lpszExeKey   = NULL;
    LPCTSTR    szValue      = _T("");

    TCHAR      szValueBuf[SYM_MAX_PATH];
    DWORD      dwValueBufLen = sizeof (szValueBuf);
    HKEY       hBaseKey;
    DWORD      dwValueType;

    // Empty the strings.
    *lpszNavw = '\0';
    *lpszDir  = '\0';

    //  Use the reg key for the platform we're on.
    if ( GetWindowsPlatform() == VER_PLATFORM_WIN32_NT )
        lpszExeKey = szNavNTKey;
    else
        lpszExeKey = szNav95Key;
        
    //  Get the OtherSwitches value from the CmdLines key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, lpszExeKey, 0, KEY_ALL_ACCESS, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen)
                _tcscpy (lpszNavw, szValueBuf);
        }
    }

    RegCloseKey (hBaseKey);

    // If we were successful at setting up lpszNavw, copy the path portion
    // to lpszDir.
    if (*lpszNavw)
    {
        LPSTR lpszCurSrc  = lpszNavw;
        LPSTR lpszCurDest = lpszDir;
        LPSTR lpszLastDir = lpszDir;
        int   iChar;

        while (*lpszCurSrc != '\0')
        {
            iChar = *lpszCurSrc;

            // Copy the character.
            *lpszCurDest = *lpszCurSrc;
            ++lpszCurSrc;
            ++lpszCurDest;

            if (isleadbyte (iChar))
                continue;

            // Now, we are guaranteed that the character is SBCS.

            // Memorize the position of the last \ in the destination string.
            if (iChar == '\\')
                lpszLastDir = (lpszCurDest - 1);
        }

        // Terminate the lpszDir string at the last \ character. (Does not
        // include the \ character).
        *lpszLastDir = '\0';
    }
} // ApwGetNavwExePath

