// Copyright 1994 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVAPW/VCS/NAVAPW.CPv   1.17   14 Aug 1998 15:51:54   mdunn  $
//
// Description:
//      NAVAPW.CPP -- Contians main routines for the Norton AntiVirus
//      Auto-Protect Windows 32-bit Agent.  This program is a 32-bit
//      client of the NAVAP VxD and resides in the tray of the task
//      bar.
//
// NOTE:
//      If AUTOLOAD_NAVAP_VXD is defined, then we will auto-load the Norton
//      AntiVirus Auto-Protect VxD for referencing, and dereference it when
//      we are done.  This is defined should be toggled in NAVAPW.H.
//
// Contains:
//      WinMain()
//      MainDlgRegisterClass()
//      IsMainDlgPrevInstance()
//      MainDlgProc()                   <<<=== EXPORTED
//      MainDlgInit()
//      MainDlgDeInit()
//      MainDlgCfgInit()
//      MainDlgShowEnableDisable()
//      MainDlgEnableDisable()
//      MainDlg_WM_COMMAND()
//      MainDlgUpdateInfoBox()
//      MainDlgTrayNotify()
//      MainDlgPopupTrayMenu()
//      MainDlgActivateNavw()
//      MainDlgGetCurrentNavWindow()
//      MainDlgNavwThrdWinEnum()
//      MainDlgGetNavwExePath()
//      NavApLoadConfig()
//      SendTrayMessage()
//
//***************************************************************************
// $Log:   S:/NAVAPW/VCS/NAVAPW.CPv  $
// 
//    Rev 1.17   14 Aug 1998 15:51:54   mdunn
// Added support for varying-length trial periods
// 
//    Rev 1.16   14 Jul 1998 14:47:32   mdunn
// Moved some preprocessor stuff around to get the trialware component's GUIDs
// defined properly.
// 
//    Rev 1.15   22 Jun 1998 15:18:42   PVENKAT
// Fixed 117154.  On non-admin account, if 'Open Nav' is selected, then only 
// explorer is started not NAVW.  This was because we use registry to find the
// complete pathname of NAVW and opening of Registry was done with KEY_ALL_ACCESS
// which will fail on NT if the current user is non-Admin.  Changed to KEY_READ
// as there is no update only read.
// 
// 
//    Rev 1.14   11 Jun 1998 16:00:46   DBuches
// Fixed problem with enabling the enable/Disable AP menu item.
// 
//    Rev 1.13   27 May 1998 14:20:52   mdunn
// No longer activating a previous instance of the Agent... we don't want
// the Agent UI coming up any more.
// 
//    Rev 1.12   02 Apr 1998 18:20:40   DALLEE
// Changed double left click from open AP Agent to open/launch
// NAVW. We aren't displaying the agent anymore.
// 
//    Rev 1.11   02 Apr 1998 18:17:34   DALLEE
// Set "Open Norton AntiVirus" to be default tray right click menu item.
// Gives it a bold look so user knows it's also the default double
// left click action.
// 
//    Rev 1.10   26 Mar 1998 15:51:38   DALLEE
// Removed opening splash screen code. We're no longer displaying it.
// Also minor fix for tray popup menu - should PostMessage() main window
// after menu closes.
// 
//    Rev 1.9   16 Mar 1998 14:56:20   tcashin
// Start Navw or activate the correct Navw window from the tray
// menu. Also display Navw's options if Navw is running.
// 
//    Rev 1.8   11 Feb 1998 10:55:08   DALLEE
// Now minimize instead of allowing close.
// Old behavior can be restored by setting reg value "AllowCloseAgent".
// 
//    Rev 1.7   18 Aug 1997 14:13:12   DALLEE
// Added navRpcCriticalSection required by NavRpcXXX() functions we stole
// from NAVXUTIL.
// 
//    Rev 1.6   31 Jul 1997 18:19:26   JBRENNA
// Switch from NAVWNT_PREVIEW to NAV_PREVIEW ... this matches the
// NAVW project.
// 
//    Rev 1.5   28 Jul 1997 10:29:40   DBUCHES
// Fixed problem launching NAVW options screen while NAVW is already running.
// 
//    Rev 1.4   18 Jul 1997 13:36:34   MKEATIN
// Changed the caption on the trial version expriration message box to
// IDS_APP_CAPTION.
// 
//    Rev 1.3   16 Jul 1997 17:10:14   MKEATIN
// Changed IDS_LICENSEEXPAIRED to IDS_LICENSEEXAPIRED.  Also, Initialize
// trial version as Norton AntiVirus - not Norton AntiVirus NT.
// 
// 
//    Rev 1.2   10 Jul 1997 14:23:38   MKEATIN
// Removed a stray letter
// 
//    Rev 1.1   10 Jul 1997 14:16:34   MKEATIN
// TrialWare will now look for NAV_VERSION_MAJOR and NAV_VERSION_MINOR
// instead hard coded values.
// 
//    Rev 1.0   24 Apr 1997 21:27:52   ksackin
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:12:24   RFULLER
// Initial revision
// 
//    Rev 1.3   03 Feb 1997 19:36:26   JBELDEN
// changed NAVWNT_PREVIEW code to use symtrial COM object
// instead of hard coded date.
// 
//    Rev 1.2   28 Jan 1997 05:28:28   DSACKIN
// If we aren't to display the icon, we should hide it very first thing.
//
//    Rev 1.1   09 Dec 1996 09:46:00   TIVANOV
// new define for linear preview date
//
//    Rev 1.0   08 Nov 1996 12:13:36   DKEEFER
// Initial revision
//
//    Rev 1.32   22 Oct 1996 13:47:26   JBELDEN
// added code for time-bomb for navwnt_preview
//***************************************************************************


//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include "windowsx.h"
#include "xapi.h"
#include "file.h"
#include "msgdisp.h"
#include "ngui.h"
#include "ctl3d.h"                      // Microsoft 3D look

#ifdef NAV_PREVIEW
    #define INITGUID
#endif

#include "navutil.h"                    // For NavXUtil.DLL routines
#include "ctsn.h"
#include "navaplog.h"                   // For log file routines
#include "navapw.h"                     // Auto-Protect Agent items
#include "navapcom.h"                   // Agent <-> VxD communication items
#include "navapstb.h"                   // Agent string table ids.
#include "navapres.h"                   // Agent resource ids.
#include "navwcomm.h"                   // NAVW32 communication ids.
#include "navver.h"
#include "symcfg.h"
#include "navw.h"

#ifdef NAV_PREVIEW
    #include "tryint.h"
#endif


#include "apscomm.h"                    // For Constants used in talking with APSVC
#include "svcctrl.h"                    // For Service Communications

#include "apscli.h"                     // For Service Communications

//***************************************************************************
// Global Definitions and Variables.  PLEASE DO NOT add globals directly
// to this section.  They should be part of g_Cfg and accessed via the
// macros defined in NAVAPW.H.  Thanx.
//***************************************************************************
#define         TRACE                   ERR_TRACE_ID (_NAVAPW_CPP)

/////////////////////////////////////////////////////////////////////
// Defines

#define     SZ_NAVW_PROGRAM     _T("NAVWNT.EXE")
#define     SZ_NAVW_WNDCLASS    _T("nav")
#define     SZ_SCAN_WNDCLASS    _T("ScanWindow")
#define     MAX_WNDCLASS        512


/////////////////////////////////////////////////////////////////////
// Global Variables

NAVAPW_CONFIG       g_Cfg                   = { 0 };
HINSTANCE           hInstance               = NULL;
CRITICAL_SECTION    *navRpcCriticalSection  = NULL;


CNAVAPSVCClient g_TalkAPService;

//***************************************************************************
// Local function prototypes
//***************************************************************************
LRESULT PASCAL MainDlgDefaultProc (     // Handles default messages
    HWND        hDlg,                   // [in] Window handle
    UINT        uMsg,                   // [in] Message to process
    WPARAM      wParam,                 // [in] wParam of message
    LPARAM      lParam);                // [in] lParam of message

static VOID CALLBACK TrayTimerProc (
    HWND  hwnd,
    UINT  uMsg,
    UINT  idEvent,
    DWORD dwTime
    );

VOID LOCAL UpdateTrayIcon (             // Updates tray icon
    HWND    hDlg                        // [in] navapw window handle
    );

void PASCAL MainDlgOptions (
    HWND hDlg );

BOOL PASCAL NewShellTrayPresent (       // Returns TRUE if tray present
    void);                              // no parameters

#ifdef NAV_PREVIEW
BOOL LOCAL IsTrialValid(VOID);
#endif


//***************************************************************************
// WinMain()
//
// Description:
//      Main entry point for the Auto-Protect Win32 Agent.  This routine
// simply creates the main dialog box for the application.
//
// Parameters:
//      hInstance       - [IN] Instance handle assinged by Windows
//      hPrevInstance   - [IN] Handle of any previous instance
//      lpszCmdLine     - [IN] Command line passed to application
//      nCmdShow        - [IN] How to show the application's main window
//
// Return Value:
//      FALSE           - Traditional WinMain value
//
// See Also:
//      MainDlgRegisterClass()
//      MainDlgProc()
//
//***************************************************************************
// 12/05/1994 MARKL Function Created.
//***************************************************************************
int PASCAL WinMain (                    // Main window function
    HINSTANCE   hInst,                  // [IN] Instance handle
    HINSTANCE   hPrevInstance,          // [IN] Previous instance handle
    LPSTR       lpszCmdLine,            // [IN] Command line to process
    int         nCmdShow)               // [IN] How to show main window
{
    auto    HWND        hWndMain        = NULL;
    auto    MSG         msg             = {0};
    auto    CFGREGKEY   cfg             = {0};

    auto    DWORD       dwType, dwValue, dwValueSize;
    auto    LONG        lRegResult;
    auto    CFGTEXTREC  CfgRecord;
    auto    BOOL        bLaunched = FALSE;
    auto    char        szSwitch[20];
    auto    HANDLE      hMutex;         // For use with previous instance

                                        // Store instance handle and how
                                        // the application wants to be
                                        // displayed for the
                                        // rest of the application.
    CFG_SET_INSTANCE (hInstance = hInst);

                                         //check the preview date and then run
#ifdef NAV_PREVIEW
	if(!IsTrialValid())
        {
            return FALSE;
        }
#endif
                                        // Get the Windows type and save it
                                        // for later (easier performance).
    CFG_SET_WINDOWS_VERSION( SystemGetWindowsType() );

                                        // Lets to a little parameter checking.
    ConfigParamToUpper();

    if ( LoadString (
            CFG_GET_INSTANCE,
            IDS_CMD_LAUNCHED,
            szSwitch,
            sizeof (szSwitch)) )
    {
      MEMSET (&CfgRecord, 0, sizeof(CfgRecord));
      CfgRecord.lpSwitch = &szSwitch[0];
      CfgRecord.wFlags   = CFG_SW_EXACT;

      if ( CfgRecord.lpSwitch )
         bLaunched = ConfigSwitchSet (&CfgRecord, NULL);
    }

                                        // No longer activating the prev
                                        // instance since we don't want the
                                        // Agent UI shown any more.
    CFG_SET_ACTIVATE_PREV_INST (FALSE);

    if (bLaunched)
    {
       CFG_SET_CMDSHOW (SW_SHOWNORMAL);
    }
    else
    {
      CFG_SET_CMDSHOW (SW_SHOWMINNOACTIVE);
    }

                                        // See if close unloads the agent or
                                        // just minimizes it.
                                        // Default is to minimize. On NT with
                                        // reg value AllowCloseAgent, we go back
                                        // to NAVNT's old behavior of allowing 
                                        // user to exit the agent.
    CFG_SET_ALLOW_CLOSE_AGENT ( FALSE );

    if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {                
        cfg.hkey        = HKEY_LOCAL_MACHINE;
        cfg.lpszProduct = NAV_PRODUCT_NAME;
        cfg.lpszVersion = NAV_MODULE_AUTOPROTECT;
        cfg.lpszName    = NAV_ALLOWCLOSEAGENT;

        dwType = REG_DWORD;
        dwValueSize = sizeof(dwValue);

        lRegResult = ConfigRegQueryValue ( &cfg, NULL, &dwType, &dwValue, &dwValueSize );

        if ( lRegResult == ERROR_SUCCESS &&
             dwValueSize == sizeof(dwValue) &&
             dwType == REG_DWORD &&
             dwValue != 0 )
            {
            CFG_SET_ALLOW_CLOSE_AGENT ( TRUE );
            }
        }

                                        // Use a Mutex to make sure we are
                                        // the only one here.
    hMutex = CreateMutex(NULL, FALSE, TEXT("Norton AntiVirus Agent Is Running"));

    if (NULL == hMutex)
        {
                                        // An error (like out of memory) has occurred.
        return FALSE;
        }

                                        // Make sure we are the only process
                                        // with a handle to our named mutex.

    if (ERROR_ALREADY_EXISTS == GetLastError())
        {
        CloseHandle(hMutex);
                                        // Make him active now
        ActivatePrevInstance();

        return FALSE;
        }

                                        // Initialize critical section for
                                        // NavRpc functions stolen from
                                        // NAVXUTIL.

    navRpcCriticalSection = new CRITICAL_SECTION;

    if ( NULL != navRpcCriticalSection )
        {
        InitializeCriticalSection( navRpcCriticalSection );
        }
    else
        {
        SYM_ASSERT( FALSE );
        }

    __try
        {
                                        // Register our dialog's window class.
            
        MainDlgRegisterClass();


                                        // Initialize Norton Look-Feel
                                        // library.
        NG_Init();
                                        // -----------------------------------
                                        // Enable Ctrl3D only if running on NT
                                        // -----------------------------------
        if ( SystemGetWindowsType() != SYM_SYSTEM_WIN_95 )
           {
           Ctl3dRegister (CFG_GET_INSTANCE);
           Ctl3dAutoSubclass(CFG_GET_INSTANCE);
           }

        hWndMain = MainDlgCreate();

                                        // Create our main window, letting
                                        // a dialog box represent it.
        if (hWndMain)
           {
           ShowWindow (hWndMain, CFG_GET_CMDSHOW);
                                        // Process messages for our
                                        // application.
           while (GetMessage (&msg, NULL, 0, 0))
                 {
                 if (!IsDialogMessage (hWndMain, &msg))
                    {
                    TranslateMessage (&msg);
                    DispatchMessage (&msg);
                    }
                 }
           }

        Ctl3dUnregister (CFG_GET_INSTANCE);
                                        // We are done with the NG routines.
        NG_Done();

        }
    __finally
        {
                                        // ALWAYS Destroy the mutex!!
        CloseHandle(hMutex);

                                        // ...and free critical section object.
        if( navRpcCriticalSection != NULL )
            {
            delete navRpcCriticalSection;
            }
        }
                                        // Program has terminated.
    return (FALSE);
} // WinMain()



//************************************************************************
// MainDlgRegisterClass()
//
// This routine registers the class required by the Norton Antivirus
// Auto-Protect Agent.
//
// Returns:
//      Nothing
//************************************************************************
// 12/18/94 MSL Function created.
//************************************************************************
VOID PASCAL MainDlgRegisterClass(       // Registers class used by agent
    VOID)                               // No parameters
{
    WNDCLASSEX  WndClass;               // Window class info structure

                                        // Fill in the registration
                                        // structure with the appropriate
                                        // information for the main dialog
                                        // window class.
    WndClass.cbSize             = sizeof (WndClass);
    WndClass.style              = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc        = (WNDPROC)MainDlgProc;
    WndClass.cbClsExtra         = 0;
    WndClass.cbWndExtra         = DLGWINDOWEXTRA;
    WndClass.hInstance          = CFG_GET_INSTANCE;
    WndClass.hIcon              = LoadIcon (CFG_GET_INSTANCE,
                                            MAKEINTRESOURCE (IDI_NAVAPW_APP_ENABLED));

    WndClass.hIconSm            = (HICON)LoadImage (
                                            CFG_GET_INSTANCE,
                                            MAKEINTRESOURCE (IDI_NAVAPW_APP_ENABLED),
                                            IMAGE_ICON,
                                            16,16,0
                                            );
    WndClass.hCursor            = NULL;
    WndClass.hbrBackground      = (HBRUSH)(COLOR_BTNFACE + 1);
    WndClass.lpszMenuName       = NULL;
    WndClass.lpszClassName      = NAVAP_WND_CLASS;

                                // Register our class
    RegisterClassEx (&WndClass);
} // MainDlgRegisterClass()


//************************************************************************
// ActivatePrevInstance()
//
// This function will make an attempt to activate a previous instance
// of NAVAPW.  Because the previous instance may not have a window active
// yet, we will make the attempt to get the window handle several times
// with a short delay between them.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
//************************************************************************
// 01/09/95 MSL Function created.
// 07/11/96 DSACKIN - Function changed to activate prev instance instead of
//                  - checking for prev instance.
//************************************************************************
#define MAX_ACTIVATE_ATTEMPTS 5
#define ACTIVATE_SLEEP_MS 1000
VOID PASCAL ActivatePrevInstance(
    VOID)                               // No parameters
{
    HWND        hWndPrev        = NULL;
    BOOL        bRet            = FALSE;
    int         nCount;

                                        // Are we allowed to activate the
                                        // previous instance?
    if (FALSE == CFG_GET_ACTIVATE_PREV_INST)
        return;

                                        // See if there is a previous
                                        // instance.

    for (nCount = 0; nCount < MAX_ACTIVATE_ATTEMPTS; nCount++)
        {
        hWndPrev = FindWindow (NAVAP_WND_CLASS, NULL);

        if (IsWindow (hWndPrev))
            {
                                        // Now that we've found it, activate it
            if (IsIconic (hWndPrev))
                {
                OpenIcon (hWndPrev);
                }

                                        // Display our dialog
            PostMessage ( hWndPrev, APP_MESSAGE_DISPLAY_DIALOG, 0, 0 );

                                        // Skip out of this loop if we found
                                        // the other window
            break;
            }

                                        // Sleep our wait and try again.
        Sleep (ACTIVATE_SLEEP_MS);
        }

} // ActivatePrevInstance()


//************************************************************************
// MainDlgCreate()
//
// Description:
//      Creates the main dialog window for the Norton AntiVirus Auto-
// Protect Agent.
//
// Returns:
//      HWND                            - Handle of main dialog window;
//                                      - NULL if not created.
//************************************************************************
// 01/09/95 MSL Function created.
//************************************************************************
HWND PASCAL MainDlgCreate (             // Create main window for AP agent
    VOID)                               // No parameters
{
    HWND             hWnd    = NULL;
    WINDOWPLACEMENT  rPlacement;
    int              nCx,nCy;

                                        // Create the main window.  Let a
                                        // dialog box serve as this window
                                        // using Petzold's trick.
    hWnd = CreateDialog (CFG_GET_INSTANCE,
                         MAKEINTRESOURCE (IDD_MAIN),
                         NULL,
                         NULL);

    if (NULL != hWnd)
    {
        rPlacement.length = sizeof (WINDOWPLACEMENT);

        if (GetWindowPlacement ( hWnd, &rPlacement ))
        {
            nCx = rPlacement.rcNormalPosition.right  - rPlacement.rcNormalPosition.left;
            nCy = rPlacement.rcNormalPosition.bottom - rPlacement.rcNormalPosition.top;

            rPlacement.rcNormalPosition.left = (GetSystemMetrics(SM_CXSCREEN) - nCx) / 2;
            rPlacement.rcNormalPosition.top  = (GetSystemMetrics(SM_CYSCREEN) - nCy) / 2;

            rPlacement.rcNormalPosition.right  = rPlacement.rcNormalPosition.left + nCx;
            rPlacement.rcNormalPosition.bottom = rPlacement.rcNormalPosition.top  + nCy;

            rPlacement.showCmd = SW_HIDE;

            rPlacement.length = sizeof (WINDOWPLACEMENT);

            SetWindowPlacement ( hWnd, &rPlacement );
        }
    }

                                        // Return to caller.
    return (hWnd);
} // MainDlgCreate()



//************************************************************************
// MainDlgPaintIcon()
//
// Description:
//      Paints the correct Icon based on if we are enabled / disabled
//
// Returns:
//    none.
//
//************************************************************************
// 05/23/96 DSACKIN - Function Created
//************************************************************************
VOID PASCAL MainDlgPaintIcon (
    HWND hDlg)                          // Handle of dialog to paint
{
PAINTSTRUCT ps;
HDC         hDC;
HICON       hIcon;
RECT        rect;

   hDC = BeginPaint(hDlg, &ps);

   GetClientRect(hDlg, &rect);

   DefWindowProc(hDlg, WM_ICONERASEBKGND, (WPARAM)hDC, 0L);

   hIcon = LoadIcon(
              CFG_GET_INSTANCE,
              CFG_GET_ENABLE ?
                 MAKEINTRESOURCE(IDI_NAVAPW_APP_ENABLED) :
                 MAKEINTRESOURCE(IDI_NAVAPW_APP_DISABLED));

                              // Draw the icon
   DrawIcon(hDC, 0, 0, hIcon);
   EndPaint(hDlg, &ps);
} // MainDlgPaintIcon()



//***************************************************************************
// MainDlgProc()
//
// Description:
//      This is the main dialog box procedure for the main window of the
//      Auto-Protect Win32 Agent.
//
// NOTES:
//      1)  We are using a dialog box as our main application window.
//      2)  Some of the initialization required by this dialog box needs
//          to be delayed.  Note that we delay not only some configuration
//          initialization, but also communications.  This ensures that the
//          dialog box is actually ready to have controls updated and
//          painted when we need them to be.
//      3)  Because this is a "roll your own" dialog box class we are
//          supposed to call DefDlgProc() so the controls will handle
//          correctly.  Unfortunately not all of Windows behaviors are
//          desirable when this routine is called in all cases.  Below are
//          the messages that are passed to DefWindowProc() to obtain the
//          desired (and EXPECTED you MS goons) effect:
//              WM_SYSCOMMAND, WM_CLOSE
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//      uMsg            - [IN] Message to process
//      wParam          - [IN] wParam of message
//      lParam          - [IN] lParam of message
//
// Return Value:
//      As expected for each message.
//
// See Also:
//      MainDlgInit()
//      NavApCommInit()
//      MainDlgTrayNotify()
//      MainDlgWM_COMMAND()
//      MainDlgDefaultProc()
//
//***************************************************************************
// 12/05/1994 MARKL Function Created.
//***************************************************************************
LRESULT SYM_EXPORT CALLBACK MainDlgProc ( // Main Dialog Box Proc
    HWND        hDlg,                   // [IN] Dialog box handle
    UINT        uMsg,                   // [IN] Message to process
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam)                 // [IN] lParam of message
{
    switch (uMsg)
        {
                                        // WM_CREATE:  Our window is there
                                        // but our children are out to
                                        // lunch; simulate the dialog
                                        // manager and post a
                                        // APP_MESSAGE_DO_INIT message
                                        // to postpone things while all of
                                        // the child windows create during
                                        // DefDlgProc().
        case WM_CREATE:
            CFG_SET_HDLG (hDlg);
            PostMessage (hDlg, APP_MESSAGE_DO_INIT, 0, 0);
            break;

                                        // APP_MESSAGE_DO_INIT:  Time to
                                        // do our program initialization.
        case APP_MESSAGE_DO_INIT:
            MainDlgInit (hDlg);
            PostMessage (hDlg, APP_MESSAGE_DO_COMM_INIT, 0, 0);

            if ( (SW_SHOWMINNOACTIVE == CFG_GET_CMDSHOW) &&
                 (NewShellTrayPresent()) )
               ShowWindow (hDlg, SW_HIDE);

                                        // The First time, we need to paint
                                        // ourselves again.
            UpdateWindow(hDlg);

            break;

                                        // APP_MESSAGE_DO_COMM_INIT:  It is
                                        // time to initialize with our
                                        // communications component so
                                        // we can talk with the VxD.
        case APP_MESSAGE_DO_COMM_INIT:
            MainDlgDeviceCommInit (hDlg);
            break;

                                        // APP_MESSAGE_ERR_NOTIFY:  The
                                        // UI needs to tell the user that
                                        // something has gone wrong.
        case APP_MESSAGE_ERR_NOTIFY:
            MainDlgErrNotify (hDlg, LOWORD (wParam), LOWORD (lParam));
            break;

                                        // APP_MESSAGE_TRAY_NOTIFY:  The
                                        // Tray icon for our app wants to
                                        // tell us something.
        case APP_MESSAGE_TRAY_NOTIFY:
            return (MainDlgTrayNotify (hDlg, wParam, lParam));

                                        // APP_MESSAGE_DO_SHUTDOWN:  We
                                        // have been told to shut down
                                        // windows by the VxD.
        case APP_MESSAGE_DO_SHUTDOWN:
            // Initiate a WM_COMMAND/ID_MAIN_UNLOAD command to cleanup
            // since we no longer get messages when we use the EWX_FORCE
            // flag in ExitWindowsEx().
            FORWARD_WM_COMMAND (hDlg, ID_MAIN_UNLOAD,
                                GetDlgItem (hDlg, ID_MAIN_UNLOAD),
                                BN_PUSHED,
                                MainDlgProc);
            CFG_SET_TERMINATING_AGENT (TRUE);

                                        // Not allowed to halt the system
                                        // under Windows NT.
            if ( SYM_SYSTEM_WIN_NT != CFG_GET_WINDOWS_VERSION )
               NavHaltSystem();
            break;

                                        // APP_MESSAGE_DO_RESTART:  We have
                                        // been told to restart windows.
        case APP_MESSAGE_DO_RESTART:
            CFG_SET_TERMINATING_AGENT (TRUE);

            if ( !ExitWindowsEx ( EWX_REBOOT, 0L ) )
            {
                                        // Tell the VxD that Windows didn't
                                        // exit as we expected.
               CFG_SET_TERMINATING_AGENT (FALSE);

            }
            break;
                                        // APP_MESSAGE_DISPLAY_DIALOG:  We
                                        // have been told to display our
                                        // agent's dialog.
        case APP_MESSAGE_DISPLAY_DIALOG:
            ShowWindow (hDlg, SW_RESTORE);
            ShowWindow (hDlg, SW_SHOW);
            SetForegroundWindow (hDlg);
            SetActiveWindow (hDlg);
            SetFocus ( GetDlgItem(hDlg,IDOK) );
            break;

                                        // WM_SYSCOLORCHANGE:  User has
                                        // changed system colors so react.
        case WM_SYSCOLORCHANGE:
            NG_ColorChange();
            break;

                                        // WM_MOVE:  Should we hide our
                                        // window.
        case WM_MOVE:
            if (IsWindowVisible (hDlg) &&
                IsIconic(hDlg) &&
                CFG_GET_HIDE_ICON)
                {
                ShowWindow (hDlg, SW_HIDE);
                }
            break;

                                        // WM_COMMAND:  Handle any commands
                                        // coming in from the user.  We must
                                        // also force this to DefDlgProc().
        case WM_COMMAND:
            MainDlgWM_COMMAND (hDlg, wParam, lParam);
            return (DefDlgProc (hDlg, uMsg, wParam, lParam));
            break;

                                        // WM_DESTROY:  The application is
                                        // terminating so remove us from
                                        // the tray and quit the application.
        case WM_DESTROY:
            CFG_SET_HIDE_ICON ( TRUE );
            UpdateTrayIcon ( hDlg );
            PostQuitMessage (0);
            break;

                                        // WM_ENDSESSION:  Windows is
                                        // terminating.  Adios!
        case WM_ENDSESSION:
            // This is a safety net to assure we never call this code twice
            // (once before calling ExitWindowsEx (w/EWX_FORCE)); this is
            // in case Windows ever changes and passes this message through
            // during our an ExitWindows()...

                                        // Check to see if the user decided
                                        // not to shut down the machine.  If
                                        // this is the case, don't forward on
                                        // the message to our shutdown code.
            if (CFG_GET_IGNORE_SHUTDOWN)
            {
                                        // Turn off our ignore flag until next time.
               CFG_SET_IGNORE_SHUTDOWN(FALSE);
               break;
            }

            if ( ! CFG_GET_UNLOADING_SHUTDOWN )
               {
               CFG_SET_UNLOADING_SHUTDOWN ( TRUE );
               FORWARD_WM_COMMAND (hDlg, ID_MAIN_UNLOAD,
                                   GetDlgItem (hDlg, ID_MAIN_UNLOAD),
                                   BN_PUSHED,
                                   MainDlgProc);
               }
            break;

                                        // WM_QUERYENDSESSION:  Windows is
                                        // asking to terminate.
        case WM_QUERYENDSESSION :
            // Before allowing Windows to shut down, we need to perform
            // the shutdown scan.

            if ( CFG_GET_ENABLE )       // Only scan if we are enabled.
            {
               CFG_SET_IGNORE_SHUTDOWN(TRUE);
                                        // And the scan says continue
               if (!CmdShutdownScan(hDlg))
               {
                  return FALSE;
               }
               else
               {
                                        // If they are OK, clear the flag.
                  CFG_SET_IGNORE_SHUTDOWN(FALSE);
               }

            }
                                        // The CmdShutdownScan will continue
                                        // to scan as long as there is an
                                        // infected disk in the drive.  If we get
            return TRUE;                // here, we are clean or disabled.
            break;

                                        // WM_SYSCOMMAND:  This
                                        // message must go to
                                        // DefWindowProc() since the
                                        // routine DefDlgProc() can't handle
                                        // it correctly (MS GOONS!).
        case WM_SYSCOMMAND:
            switch ( wParam & 0xFFFFFFF0 )
                {
                case SC_CLOSE:
                    if ( CFG_GET_ALLOW_CLOSE_AGENT )
                        {
                        return ( DefWindowProc( hDlg, uMsg, wParam, lParam ) );
                        }
                    else
                        {
                        // !!! Drop through to minimize behavior below !!!
                        }
                                
                case SC_MINIMIZE:
                        // !!! Keep SC_CLOSE above. !!!
                    
                    FORWARD_WM_COMMAND (hDlg, IDOK,
                                        GetDlgItem (hDlg, IDOK),
                                        BN_PUSHED,
                                        SendMessage);
                    break;

                case SC_MAXIMIZE:
                    ShowWindow (hDlg, SW_RESTORE);
                    break;

                default:
                    return (DefWindowProc (hDlg, uMsg, wParam, lParam));
                }
            break;

                                        // WM_CLOSE:  This message must go to
                                        // DefWindowProc() since the
                                        // routine DefDlgProc() can't handle
                                        // it correctly (MS GOONS!).
        case WM_CLOSE:
            return (DefWindowProc (hDlg, uMsg, wParam, lParam));

                                        // The icon we display when iconified
                                        // should be different if we are disabled
        case WM_PAINT:
            if ( IsIconic(hDlg) )
               MainDlgPaintIcon(hDlg);
            else
               return DefWindowProc (hDlg, uMsg, wParam, lParam);
            break;

        case WM_QUERYDRAGICON:
           return ((LRESULT) LoadIcon(
                      CFG_GET_INSTANCE,
                      CFG_GET_ENABLE ?
                         MAKEINTRESOURCE(IDI_NAVAPW_APP_ENABLED) :
                         MAKEINTRESOURCE(IDI_NAVAPW_APP_DISABLED)));
           break;

                                        // Default:  Let DefDlgProc handle
                                        // this message since we couldn't
                                        // or didn't.
        default:
            return (MainDlgDefaultProc (hDlg, uMsg, wParam, lParam));
        }

                                        // We've handled the message so
                                        // return 0L to caller.
    return (0L);
} // MainDlgProc()


//***************************************************************************
// MainDlgInit()
//
// Description:
//      This routine initializes the main dailog for the Auto-Protect Agent.
//      It also references the Auto-Protect VxD and causes it to load if not
//      already there.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
// See Also:
//      SendTrayMessage()
//      MainDlgCfgInit()
//
//***************************************************************************
// 12/05/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL MainDlgInit (               // Initialize main dialog
    HWND        hDlg)                   // [IN] Dialog box handle
{
#define TRAY_CHECK_TIME_INTERVAL        5000

auto  HANDLE   hProcess;

                                        // Adjust our process to low priority
    hProcess = GetCurrentProcess();

                                        // Set our process to the lowest
                                        // classification.
    SetPriorityClass( hProcess, IDLE_PRIORITY_CLASS );

                                        // The hProcess handle does NOT need
                                        // to be freed because it is not really
                                        // a handle, but a pseudohandle or a
                                        // constant value specifying only
                                        // the current process.

    g_TalkAPService.Connect();

    SYM_ASSERT (CFG_GET_INSTANCE);
    NG_SetFontsInDialog(hDlg, 0);
                                        // Check for Windows NT and nuke.
    if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
        {
        NOTIFYSTRUCT    Notify;
        MEMSET (&Notify, EOS, sizeof(Notify));
        Notify.hParent       = GetDesktopWindow();
        Notify.uMsgID        = IDS_ERR_AP_NOWIN95;
        Notify.uTitleID      = IDS_APP_CAPTION;
        Notify.hInstance     = CFG_GET_INSTANCE;
        NotifyUser (&Notify);
        DestroyWindow (hDlg);
        return;
        }
                                        // Initialize our configuration
                                        // block.
    MainDlgCfgInit (hDlg);

                                        // If we should start off hidden, we
                                        // should make that happen now.
    if ( !NewShellTrayPresent() && CFG_GET_HIDE_ICON )
        ShowWindow ( hDlg, SW_HIDE );

                                        // Load string table entries that
                                        // we want to cache.
    LoadString (CFG_GET_INSTANCE, IDS_APP_CAPTION,
                CFG_GET_CAPTION_TEXT_PTR,
                AP_CAPTION_TEXT_SIZE);
    LoadString (CFG_GET_INSTANCE, IDS_BUTTON_ENABLE,
                CFG_GET_BUTTON_ENABLE_PTR, BUTTON_TEXT_SIZE);
    LoadString (CFG_GET_INSTANCE, IDS_BUTTON_DISABLE,
                CFG_GET_BUTTON_DISABLE_PTR, BUTTON_TEXT_SIZE);
    LoadString (CFG_GET_INSTANCE, IDS_TRAY_TIP_ENABLED,
                CFG_GET_TRAY_TIP_ENABLED_PTR, TRAY_TIP_TEXT_SIZE);
    LoadString (CFG_GET_INSTANCE, IDS_TRAY_TIP_DISABLED,
                CFG_GET_TRAY_TIP_DISABLED_PTR, TRAY_TIP_TEXT_SIZE);

                                        // Load our tray icons.
    CFG_SET_TRAY_ICON_ENABLED ( LoadImage (CFG_GET_INSTANCE,
                                   MAKEINTRESOURCE (IDI_NAVAPW_TRAY_ENABLED),
                                   IMAGE_ICON, 16, 16, 0) );
    CFG_SET_TRAY_ICON_DISABLED ( LoadImage (CFG_GET_INSTANCE,
                                    MAKEINTRESOURCE (IDI_NAVAPW_TRAY_DISABLED),
                                    IMAGE_ICON, 16, 16, 0) );

                                        // -----------------------------------
                                        // Register message so NAVW32 can talk
                                        // with us.
                                        // -----------------------------------

    CFG_SET_MSG_ID_FROM_NAVW( RegisterWindowMessage ( NAVW_NAVAPW_COMM ) );

                                        // -----------------------------------
                                        // Start timer to indicate our state
                                        // -----------------------------------
    CFG_SET_TRAY_TIMER_ID (SetTimer ( hDlg, 1, TRAY_CHECK_TIME_INTERVAL,
                                      &TrayTimerProc ) );

    SYM_ASSERT ( CFG_GET_TRAY_TIMER_ID );

                                        // Add ourselves to the tray.
    UpdateTrayIcon ( hDlg );

} // MainDlgInit()



//***************************************************************************
// MainDlgDeInit()
//
// Description:
//      This routine deinitializes the main dailog for the Auto-Protect
//      Agent.  It terminates connections, removes the tray item,
//      dereferences our Auto-Protect VxD (which will stay loaded if
//      referenced by anybody else, including Windows) and ends the
//      application by ending the main dialog.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
// See Also:
//      NavAppCommDeInit()
//      SendTrayMessage()
//
//***************************************************************************
// 12/05/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL MainDlgDeInit (             // DeInitialize main dialog
    HWND        hDlg)                   // [IN] Dialog box handle
{
    SYM_ASSERT (CFG_GET_INSTANCE);

                                        // Flag that we are in the process
                                        // of terminating.
    CFG_SET_TERMINATING_AGENT (TRUE);

    SYM_ASSERT ( CFG_GET_TRAY_TIMER_ID );

    if ( CFG_GET_TRAY_TIMER_ID )
        KillTimer ( hDlg, CFG_GET_TRAY_TIMER_ID );

    g_TalkAPService.DisConnect();       // Close our connection to APW

                                        // End this program by terminating
                                        // the main dialog box window.
    DestroyWindow (hDlg);
} // MainDlgDeInit()


//***************************************************************************
// MainDlgCfgInit()
//
// Description:
//      Initializes the configuration block for the Norton AntiVirus
//      Auto-Protect Windows Agent.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
// See Also:
//      NavApLoadConfig()
//
//***************************************************************************
// 12/06/1994 MARKL Function Created.
// 05/03/1996 DSACKIN - Function changed for NT platform
//***************************************************************************
VOID PASCAL MainDlgCfgInit (            // Initializes configuration block
    HWND        hDlg)                   // [IN] Dialog box handle
{
    if (NavApLoadConfig() == ERR)
        {
        PostMessage (hDlg,
                     APP_MESSAGE_ERR_NOTIFY,
                     IDS_ERR_AP_NO_OPTIONS_FILE,
                     FALSE);
        }
} // MainDlgCfgInit()


//***************************************************************************
// MainDlgCheckStatus()
//
// Description:
//      Checks the status of the device and updates our enabled status
//
// Parameters:
//      NONE
//
// Return Value:
//      UINT                            - NOERR we are talking with the VxD;
//                                      - ERR we are not doing so fine.
//
//***************************************************************************
// 05/03/1996 DSACKIN - Function Created.
//***************************************************************************
UINT PASCAL MainDlgCheckStatus(         // Check Device Status
    VOID)                               // No parameters
{
UINT   uReturn = NOERR;
BOOL   bDeviceEnabled = FALSE;

   switch ( CmdGetStatusOfDriver() )
   {
      case NAVAPSVC_ENABLED:
         bDeviceEnabled = TRUE;
         break;

      case NAVAPSVC_DISABLED:
         bDeviceEnabled = FALSE;
         break;

      default:
         uReturn = ERR;
   }

   CFG_SET_ENABLE (bDeviceEnabled);

   return uReturn;
} // MainDlgDeviceInit()


//***************************************************************************
// MainDlgDeviceCommInit()
//
// Description:
//      Initializes communications between the Agent and the Device.  Once
//      communications is up and running, we go through some initialization
//      with the device itself.  If any of this fails, we bail the Agent.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
//***************************************************************************
// 12/09/1994 MARKL Function Created.
// 05/03/1995 DSACKIN - ReWorked for NT.
//***************************************************************************
VOID PASCAL MainDlgDeviceCommInit (     // Initializes comm with device
    HWND        hDlg)                   // [IN] Dialog box handle
{

                                        // First check the status.  If the
                                        // device isn't loaded, it will show
                                        // as disabled.
   MainDlgCheckStatus();
                                        // Show our status.
   MainDlgShowEnableDisable (hDlg);

} // MainDlgDeviceCommInit()


//***************************************************************************
// MainDlgShowEnableDisable()
//
// Description:
//      Sets the appropriate visual controls to reflect whether Auto-Protect
// is enabled or disabled.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
// See Also:
//      SendTrayMessage()
//      MainDlgUpdateInfoBox()
//
//***************************************************************************
// 12/05/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL MainDlgShowEnableDisable(   // Shows appropriate states
    HWND        hDlg)                   // [IN] Dialog box handle
{

HICON hIcon;

    SYM_ASSERT (hDlg);

    if (CFG_GET_ENABLE)
        {
                                        // Show the enabled icon
        ShowWindow (GetDlgItem (hDlg, ID_MAIN_ICON_DISABLED), SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, ID_MAIN_ICON_ENABLED),  SW_SHOW);

                                        // Show the enable icon in the
                                        // tray.
        UpdateTrayIcon ( hDlg );

                                        // Make the button say "Disable"
        Button_SetText (GetDlgItem (hDlg, ID_MAIN_ENABLE_DISABLE),
                        CFG_GET_BUTTON_DISABLE_PTR);

                                        // Update the information box to
                                        // tell the user we are enabled.
        MainDlgUpdateInfoBox (MAKEINTRESOURCE (IDS_NAVAP_ENABLED));

                                        // Update our minimized Icon
                                        // If LoadIcon fails, we will get
                                        // the system icon.
        hIcon = LoadIcon (CFG_GET_INSTANCE, MAKEINTRESOURCE(IDI_NAVAPW_APP_ENABLED) );

                                        // Remove the HICON so we can paint it
                                        // ourselves.
        SetClassLong (hDlg, GCL_HICON, (LONG) NULL);
        }
    else
        {
                                        // Show the disabled
        ShowWindow (GetDlgItem (hDlg, ID_MAIN_ICON_ENABLED),  SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, ID_MAIN_ICON_DISABLED), SW_SHOW);

                                        // Show the disabled icon in the
                                        // tray.
        UpdateTrayIcon ( hDlg );

                                        // Make the button say "Enable"
        Button_SetText (GetDlgItem (hDlg, ID_MAIN_ENABLE_DISABLE),
                        CFG_GET_BUTTON_ENABLE_PTR);

                                        // Update the information box to
                                        // tell the user we are disabled.
        MainDlgUpdateInfoBox (MAKEINTRESOURCE (IDS_NAVAP_DISABLED));

                                        // Update our minimized Icon
                                        // If LoadIcon fails, we will get
                                        // the system icon.
        hIcon = LoadIcon (CFG_GET_INSTANCE, MAKEINTRESOURCE(IDI_NAVAPW_APP_DISABLED) );

                                        // Remove the HICON so we can paint it
                                        // ourselves.
        SetClassLong (hDlg, GCL_HICON, (LONG) NULL);
        }

                                        // Refresh the controls to paint
                                        // properly.
                                        // Fix for defect #60517: The AP should
                                        // ALWAYS be allowed to enable.. Just
                                        // not to be disabled.
    EnableWindow (
        GetDlgItem (hDlg, ID_MAIN_ENABLE_DISABLE),
        (FALSE == CFG_GET_ENABLE) ?
           TRUE :
           CFG_GET_ALLOW_ENABLE_DISABLE);
    InvalidateRect (GetDlgItem (hDlg, ID_MAIN_ENABLE_DISABLE), NULL, FALSE);
    UpdateWindow (GetDlgItem (hDlg, ID_MAIN_ENABLE_DISABLE));

    EnableWindow (
        GetDlgItem (hDlg, ID_MAIN_UNLOAD),
        CFG_GET_ENABLE);

    InvalidateRect (GetDlgItem (hDlg, ID_MAIN_ICON_ENABLED), NULL, FALSE);
    UpdateWindow (GetDlgItem (hDlg, ID_MAIN_ICON_ENABLED));
    InvalidateRect (GetDlgItem (hDlg, ID_MAIN_ICON_DISABLED), NULL, FALSE);
    UpdateWindow (GetDlgItem (hDlg, ID_MAIN_ICON_DISABLED));

    if (IsIconic(hDlg))
        {
        InvalidateRect (hDlg, NULL, FALSE);
        UpdateWindow (hDlg);
        }

                                        // Now set the HICON to what it should be.
    SetClassLong (hDlg, GCL_HICON, (LONG) hIcon);

                                        // Return to caller.
} // MainDlgShowEnableDisable()



//***************************************************************************
// MainDlgEnableDisable()
//
// Description:
//      This routine enables or disables Auto-Protect, based on what the
// user wants to do.  The UI is then updated to reflect the new state
// the Device is told to go to.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
// See Also:
//      MainDlgShowEnableDisable()
//
//***************************************************************************
// 12/05/1994 MARKL Function Created.
// 05/06/1996 DSACKIN - Reworked for NT to talk to NAVAP Service
// 06/14/1996 DSACKIN - Don't update our status.  Let the timer check the status
//***************************************************************************
VOID PASCAL MainDlgEnableDisable (      // Enable/Disable Auto-Protect
    HWND        hDlg)                   // [IN] Dialog box handle
{
   SYM_ASSERT (hDlg);

   if ( CFG_GET_ENABLE )
      CmdDisableDriver();
   else
   {
                                        // Check for an error communicating
                                        // with the driver.
      if ( ERR == MainDlgCheckStatus() )
      {
                                        // Update the info box.
         MainDlgUpdateInfoBox (MAKEINTRESOURCE (IDS_NAVAP_STARTING));

                                        // Could take a while. Set the wait cursor
         SetCursor (LoadCursor (NULL, MAKEINTRESOURCE (IDC_WAIT)));

                                        // Try to start the driver.
         CmdStartDriver();

                                        // OK now... Once we get here, the
                                        // NAVAPSVC has told me that it is
                                        // started.  We can now send in a
                                        // thread that will return when either
                                        // an error occurs or NAVAPSVC is
                                        // done initializing.
         CFG_SET_STARTUP_PENDING (TRUE);

         CmdWaitForStartupPending();

         CFG_SET_STARTUP_PENDING (FALSE);

                                        // Restore the cursor
         SetCursor (LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW)));
      }
      else
         CmdEnableDriver();
   }
                                        // First, check the status again.
   MainDlgCheckStatus();

                                        // Update the user interface to
                                        // reflect this new state.
   MainDlgShowEnableDisable (hDlg);

} // MainDlgEnableDisable()


//***************************************************************************
// MainDlgWM_COMMAND()
//
// Description:
//      This routine handles the processing of the WM_COMMAND message for
//      the main dialog proc.
//
//      hDlg            - [IN] Dialog box handle
//      wParam          - [IN] wParam of message
//      lParam          - [IN] lParam of message
//
// Return Value:
//      BOOL            - TRUE if we handled message; FALSE otherwise
//
// See Also:
//      MainDlgEnableDisable()
//      MainDlgDeInit()
//
//***************************************************************************
// 12/07/1994 MARKL Function Created.
//***************************************************************************
BOOL PASCAL MainDlgWM_COMMAND (         // Handles WM_COMMAND message
    HWND        hDlg,                   // [IN] Dialog box handle
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam)                 // [IN] lParam of message
{
    SYM_ASSERT (hDlg);

    switch (GET_WM_COMMAND_ID (wParam, lParam))
        {
                                        // ID_MAIN_ENABLE_DISABLE or
                                        // ID_TRAY_MENU_ENABLE_DISABLE:
                                        // Either enable or disable
                                        // autoprotect, toggling the UI
                                        // to reflect the new state.
        case ID_MAIN_ENABLE_DISABLE:
        case ID_TRAY_MENU_ENABLE_DISABLE:
            MainDlgEnableDisable (hDlg);
            break;

                                        // ID_TRAY_MENU_OPTIONS:  The user
                                        // user wants options.
        case ID_MAIN_OPTIONS:
        case ID_TRAY_MENU_OPTIONS:
            MainDlgOptions (hDlg);
            break;

                                        // ID_TRAY_MENU_OPEN:  The user
                                        // wants us to show ourselves so
                                        // start Navw. If it is already 
                                        // running, show the current 
                                        // window.
        case ID_TRAY_MENU_OPEN:
            MainDlgActivateNavw ();
            break;

                                        // IDOK & IDCANCEL:
                                        // Hide our interface if there is
                                        // a tray, otherwise minimize.
        case IDOK:
        case IDCANCEL:
            if ( NewShellTrayPresent() || CFG_GET_HIDE_ICON )
                ShowWindow ( hDlg, SW_HIDE );
            else
                ShowWindow ( hDlg, SW_SHOWMINNOACTIVE );
            break;

                                        // ID_MAIN_UNLOAD &
                                        // ID_TRAY_MENU_UNLOAD:
                                        // Kill the Agent.
        case ID_MAIN_UNLOAD:
        case ID_TRAY_MENU_UNLOAD:
            MainDlgDeInit (hDlg);
            break;

                                        // We didn't handle it.
        default:
            return (FALSE);
        }

                                        // We handled it so return TRUE
    return (TRUE);
} // MainDlgWM_COMMAND()


//***************************************************************************
// MainDlgDefaultProc()
//
// Description:
//      This routine handles the default case of the message processing
//      for the main window procedure.  It tests to see if it is a registered
//      window message for NAVAPW <-> NAVW communications and handles those
//      causes or simply forwards the message to DefDlgProc() if not.
//
//      NOTE:  Code cut/paste from Default: case to clean up dialog proc.
//
// Parameters:
//      HWND    hDlg,                   [in] Window handle
//      UINT    uMsg,                   [in] Message to process
//      WPARAM  wParam,                 [in] wParam of message
//      LPARAM  lParam                  [in] lParam of message
//
// Return Value:
//      Typical LRESULT for DefDlgProc/DefWindowProc.
//
//***************************************************************************
// 06/01/1995 MARK LAWLER Function Created.
//***************************************************************************
LRESULT PASCAL MainDlgDefaultProc (     // Handles default messages
    HWND        hDlg,                   // [in] Window handle
    UINT        uMsg,                   // [in] Message to process
    WPARAM      wParam,                 // [in] wParam of message
    LPARAM      lParam)                 // [in] lParam of message
{
                                        // -----------------------------------
                                        // Check if command should be processed
                                        // from NAVW32
                                        // -----------------------------------
    if ( CFG_GET_MSG_ID_FROM_NAVW &&
         CFG_GET_MSG_ID_FROM_NAVW == uMsg )
        {
        switch ( wParam )
            {
            case NAVW_CMD_SHOWHIDEICON:
                CFG_SET_HIDE_ICON ( !(BOOL)lParam );
                UpdateTrayIcon ( hDlg );
                break;

            case NAVW_CMD_CANBEDISABLED:
                {
                                // -----------------------------------
                                // If in 'Disable' state and NAVW32
                                // indicated that we should not allow
                                // NAVAPW to be disabled then DO NOT
                                // touch the enabling of the control.
                                // This would allow the user to press
                                // the enable button and THEN the
                                // button is dimmed.
                                // -----------------------------------

                if ( !CFG_GET_ENABLE && (BOOL)lParam == FALSE )
                    ;
                else
                    {
                    EnableWindow ( GetDlgItem (hDlg, ID_MAIN_ENABLE_DISABLE),
                                   (BOOL)lParam );
                    }

                CFG_SET_ALLOW_ENABLE_DISABLE ( (BOOL)lParam );
                }

                break;

            case NAVW_CMD_UNLOADNAVAPW:
                FORWARD_WM_COMMAND ( hDlg,
                                     ID_MAIN_UNLOAD,
                                     GetDlgItem (hDlg, ID_MAIN_UNLOAD),
                                     BN_PUSHED,
                                     MainDlgProc );
                break;
            }
        }
    else
        {
        return (DefDlgProc (hDlg, uMsg, wParam, lParam));
        }

                                        // Return default
    return (0L);
} // MainDlgDefaultProc()



//***************************************************************************
// MainDlgUpdateInfoBox()
//
// Description:
//      This routine updates the text in the information box.
//
// Parameters:
//      pszInfo                         - [IN] String (or string ID)
//
//***************************************************************************
// 12/20/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL MainDlgUpdateInfoBox (      // Updates Info box in main dailog
    LPSTR       pszInfo)                // [IN] String (or string ID)
{
    char        szInfoText[1024];
    LPSTR       pszInfoText;

                                        // Do we load a string or do we
                                        // use the one supplied?
    if (HIWORD (pszInfo))
        {
        pszInfoText = pszInfo;
        }
    else
        {
        LoadString (CFG_GET_INSTANCE, LOWORD (pszInfo),
                    szInfoText, sizeof (szInfoText));
        pszInfoText = szInfoText;
        }

                                        // Display the string and make
                                        // sure it paints.
    Static_SetText (
        GetDlgItem (CFG_GET_HDLG, ID_MAIN_CURRENTSTATUS),
        pszInfoText);
    InvalidateRect (GetDlgItem (CFG_GET_HDLG, ID_MAIN_CURRENTSTATUS),
                    NULL, TRUE);

                                        // Return to caller.
} // MainDlgUpdateInfoBox()



//***************************************************************************
// MainDlgErrNotify()
//
// Description:
//      Tells the user that there has been an error.  Also automatically
//      shuts down the application if required.
//
// Parameters:
//      hDlg                            - [IN] Dialog box handle
//      uErrID                          - [IN] Error message to display
//      bTerminate                      - [IN] TRUE to terminate app
//
//***************************************************************************
// 12/09/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL MainDlgErrNotify (          // Notify user about an error
    HWND        hDlg,                   // [IN] Dialog box handle
    UINT        uErrID,                 // [IN] Error message to display
    BOOL        bTerminate)             // [IN] TRUE to terminate app
{
    NOTIFYSTRUCT    Notify = {0};

    SYM_ASSERT (hDlg);
    SYM_ASSERT (uErrID);
    SYM_ASSERT (CFG_GET_INSTANCE);

                                        // If we are terminating the app,
                                        // have the UI look disabled, but
                                        // don't actually disable protection.
    if (bTerminate)
        {
        CFG_SET_ENABLE (FALSE);
        }
    ShowWindow (hDlg, SW_RESTORE);
    ShowWindow (hDlg, SW_SHOW);
    SetForegroundWindow (hDlg);
    SetActiveWindow (hDlg);
    UpdateWindow (hDlg);
    MainDlgShowEnableDisable (hDlg);

                                        // Display the error message to
                                        // the user.
    Notify.hParent       = hDlg;
    Notify.uMsgID        = uErrID;
    Notify.uTitleID      = IDS_APP_CAPTION;
    Notify.hInstance     = CFG_GET_INSTANCE;
    NotifyUser (&Notify);

                                        // If we are terminating the app,
                                        // then just pretend he closed it
                                        // himself.
    if (bTerminate)
        {
        FORWARD_WM_COMMAND (hDlg, ID_MAIN_UNLOAD,
                            GetDlgItem (hDlg, ID_MAIN_UNLOAD),
                            BN_PUSHED,
                            MainDlgProc);
        }

                                        // Return to caller.
} // MainDlgErrNotify()


//***************************************************************************
// MainDlgTrayNotify()
//
// Description:
//      Handles the APP_MESSAGE_TRAY_NOTIFY message that our window
//      receives when the tray wants to talk to us.
//
// NOTES:
//      1)  Must perform the IsWindowEnabled() check to make sure that
//          our window is active and thier isn't a subdialog or error
//          message being displayed.  To allow a tray popup menu to
//          be presented to the user, you can get in to serious trouble.
//      2)  There is a bug in that the popup menu doesn't close when you
//          click outside of the area even though we pass NULL as the
//          rectangle parameter to TrackPopupMenu()
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//      wParam          - [IN] wParam of message
//      lParam          - [IN] lParam of message
//
// Return Value:
//      BOOL            - TRUE if we handled message; FALSE otherwise
//
// See Also:
//      MainDlgPopupTrayMenu()
//
//***************************************************************************
// 12/07/1994 MARKL Function Created.
//***************************************************************************
BOOL PASCAL MainDlgTrayNotify (         // Handles tray notifications
    HWND        hDlg,                   // [IN] Dialog box handle
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam)                 // [IN] lParam of message
{
    SYM_ASSERT (hDlg);

                                        // Make the tray item do what the
                                        // user is requesting it to do.
    switch (lParam)
        {
                                        // WM_RBUTTONDOWN:  The tray icon
                                        // wants us to show a menu.
        case WM_RBUTTONUP:
            if (wParam == ID_TRAY_ICON)
                {
                if (IsWindowEnabled (hDlg))
                    {
                    MainDlgPopupTrayMenu (hDlg);
                    }
                else
                    {
                    MessageBeep (0);
                    }
                }
            break;

                                        // WM_LBUTTONDBLCLK:  The user
                                        // wants us to show ourselves so
                                        // do so.
        case WM_LBUTTONDBLCLK:
            MainDlgActivateNavw();
            break;

        default:
            return (FALSE);
        }

                                        // We handled it so return TRUE
    return (TRUE);
} // MainDlgTrayNotify()



//***************************************************************************
// MainDlgPopupTrayMenu()
//
// Description:
//      Displays a popup menu for our tray item.  This allows the user to
//      Open, Enable/Disable, Unload, or Options for Auto-Protect.  Note
//      that depending on the configuration the menu may not display
//      Enable/Disable, Unload, or Options.
//
// NOTE:
//      If the user selects an option, it will be send as a WM_COMMAND
//      message to the calling dialog proc (in this case MainDlgProc).
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
//***************************************************************************
// 12/06/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL MainDlgPopupTrayMenu (      // Build and display tray popup menu
    HWND        hDlg)                   // [IN] Dialog box handle
{
#define         MENU_TEXT_SIZE  40
    HMENU       hPopup = NULL;
    char        szMenuText[MENU_TEXT_SIZE];
    POINT       pMouse;
    HWND        hNavwWnd = NULL;
    HWND        hCurrWnd = NULL;

    SYM_ASSERT (hDlg);
    SYM_ASSERT (CFG_GET_INSTANCE);

                                        // Create the Popup menu
    if (hPopup = CreatePopupMenu ())
        {
                                        // Add the OPEN item as the first
                                        // item.
        LoadString (CFG_GET_INSTANCE, IDS_TRAY_MENU_OPEN,
                    szMenuText, sizeof (szMenuText));
        AppendMenu (hPopup, MF_STRING, ID_TRAY_MENU_OPEN, szMenuText);
        AppendMenu (hPopup, MF_SEPARATOR, 0, NULL);

        SetMenuDefaultItem( hPopup, ID_TRAY_MENU_OPEN, FALSE );

                                        // Add either ENABLE or DISABLED
                                        // item.
        if (!CFG_GET_ENABLE)
            {
            LoadString (CFG_GET_INSTANCE, IDS_TRAY_MENU_ENABLE,
                        szMenuText, sizeof (szMenuText));
            }
        else
            {
            LoadString (CFG_GET_INSTANCE, IDS_TRAY_MENU_DISABLE,
                        szMenuText, sizeof (szMenuText));
            }

        AppendMenu (hPopup, MF_STRING,
                    ID_TRAY_MENU_ENABLE_DISABLE, szMenuText);

        if (!CFG_GET_ALLOW_ENABLE_DISABLE )// && CFG_GET_ENABLE)
            {
            EnableMenuItem (hPopup, ID_TRAY_MENU_ENABLE_DISABLE,
                            MF_BYCOMMAND | MF_GRAYED);
            }

#ifdef UNLOAD_BUTTON
                                        // Add the Unload item.
        LoadString (CFG_GET_INSTANCE, IDS_TRAY_MENU_UNLOAD,
                    szMenuText, sizeof (szMenuText));
        AppendMenu (hPopup, MF_STRING, ID_TRAY_MENU_UNLOAD, szMenuText);

        if (!CFG_GET_ALLOW_UNLOAD)
            {
            EnableMenuItem (hPopup, ID_TRAY_MENU_UNLOAD,
                            MF_BYCOMMAND | MF_GRAYED);
            }
#endif

                                        // Add the Options item.
        LoadString (CFG_GET_INSTANCE, IDS_TRAY_MENU_OPTIONS,
                    szMenuText, sizeof (szMenuText));
        AppendMenu (hPopup, MF_STRING, ID_TRAY_MENU_OPTIONS, szMenuText);


        // Disable the options menu if the main UI is up and
        // other dialog boxes are open.

        if (MainDlgGetCurrentNavWindow( (LPHANDLE)&hNavwWnd, (LPHANDLE)&hCurrWnd))
            {
            if ( hNavwWnd != hCurrWnd)
                EnableMenuItem (hPopup, ID_TRAY_MENU_OPTIONS,
                                MF_BYCOMMAND | MF_GRAYED);
            }

                                        // Finally time to show the menu
                                        // to our user and allow him to
                                        // select an option.
        SetMenuDefaultItem(hPopup, 0, TRUE);
        GetCursorPos (&pMouse);

        // Kluge part 1: Put our Wnd in foreground before calling TrackPopupMenu
        // See notes in W32 SDK KBase --
        // "PRB: Menus for Notification Icons Don't Work Properly"

        SetForegroundWindow(CFG_GET_HDLG);

        TrackPopupMenu (hPopup, 0, pMouse.x, pMouse.y, 0, hDlg, NULL);

        // Kluge part 2: Bogon message to cause a task switch to us

        PostMessage( hDlg, WM_USER, 0, 0 );

        DestroyMenu( hPopup );
        }
} // MainDlgTrayMenu()


//***************************************************************************
// NavApLoadConfig()
//
// Description:
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 06/14/1995 MARK LAWLER Function Created.
//***************************************************************************
UINT PASCAL NavApLoadConfig (           // Loads configuration file for agent
    VOID)                               // No parameters
{
    NAVOPTIONS  navOptions      = {0};
    UINT        uRet            = NOERR;

                                        // Read configuration from the
                                        // NAVOPTS.DAT file to see what is
                                        // going on.
    if ((uRet = ReadNavBin (&navOptions)) != ERR)
        {
                                        // Set our configuration block
                                        // with the appropriate values.
        CFG_SET_TERMINATING_AGENT (FALSE);
        CFG_SET_HIDE_ICON (navOptions.tsr.bHideIcon);
        CFG_SET_ALLOW_ENABLE_DISABLE (navOptions.tsr.bUnloadable);
#ifdef UNLOAD_BUTTON
        CFG_SET_ALLOW_UNLOAD (navOptions.tsr.bUnloadable);
#else
        CFG_SET_ALLOW_UNLOAD (FALSE);
#endif
        STRCPY (CFG_GET_ACTLOG_SPOOL_FILE_PTR, navOptions.activity.szLogFile);
        }

                                        // Return to caller
    return (uRet);
} // NavApLoadConfig()


/*@API:*************************************************************************
@Declaration:

VOID LOCAL UpdateTrayIcon (
    HWND    hDlg
    );

@Description:
This function adds and removes the tray icon for NAVAPW. This functionality
was moved into this function so the code that accomplishes this task is in one
place. This is specially important when NAVW32 talks with NAVAPW telling
him that it's tray icon should be added or removed from tray.

This routine also updates our window class's small icon.

@Parameters:
$hDlg$          NAVAPW window handle

@Returns:       Nothing.

@Include:       None, local function.

*******************************************************************************/

VOID LOCAL UpdateTrayIcon (             // Updates tray icon
    HWND    hDlg                        // [in] navapw window handle
    )
{
    static  BOOL    bIconInTray =   FALSE;
    static  BOOL    bEnabled    =   FALSE;

    if ( NewShellTrayPresent() )
        {
                                        // -----------------------------------
                                        // Check if icon should be hidden.
                                        // -----------------------------------
        if ( CFG_GET_HIDE_ICON )
            {
            if ( bIconInTray == TRUE )
                {
                bIconInTray = FALSE;

                                        // -----------------------------------
                                        // Take ourselves out of the tray.
                                        // -----------------------------------

                SendTrayMessage ( hDlg, NIM_DELETE, NULL, NULL );
                }
            }
        else
            {
            if ( bIconInTray == FALSE )
                {
                                        // Not in tray?  Use NIM_ADD
                bIconInTray = TRUE;

                if ( CFG_GET_ENABLE )
                    {
                    bEnabled = TRUE;

                    SendTrayMessage ( hDlg, NIM_ADD,
                                      CFG_GET_TRAY_ICON_ENABLED,
                                      CFG_GET_TRAY_TIP_ENABLED_PTR );
                    SetClassLong ( hDlg, GCL_HICONSM,
                                   (LONG)CFG_GET_TRAY_ICON_ENABLED);
                    }
                else
                    {
                    bEnabled = FALSE;

                    SendTrayMessage ( hDlg, NIM_ADD,
                                      CFG_GET_TRAY_ICON_DISABLED,
                                      CFG_GET_TRAY_TIP_DISABLED_PTR );
                    SetClassLong ( hDlg, GCL_HICONSM,
                                   (LONG)CFG_GET_TRAY_ICON_DISABLED);
                    }
                }
            else
                {
                if ( CFG_GET_ENABLE )
                    {
                    if ( bEnabled == FALSE )
                        {
                        bEnabled = TRUE;

                        SendTrayMessage ( hDlg, NIM_MODIFY,
                                          CFG_GET_TRAY_ICON_ENABLED,
                                          CFG_GET_TRAY_TIP_ENABLED_PTR );
                        SetClassLong ( hDlg, GCL_HICONSM,
                                       (LONG)CFG_GET_TRAY_ICON_ENABLED);
                        }
                    }
                else
                    {
                    if ( bEnabled == TRUE )
                        {
                        bEnabled = FALSE;

                        SendTrayMessage ( hDlg, NIM_MODIFY,
                                          CFG_GET_TRAY_ICON_DISABLED,
                                          CFG_GET_TRAY_TIP_DISABLED_PTR );
                        SetClassLong ( hDlg, GCL_HICONSM,
                                       (LONG)CFG_GET_TRAY_ICON_DISABLED );
                        }
                    }
                }
            }
        }
    else
        {
        bIconInTray = FALSE;
        }
}


//***************************************************************************
// SendTrayMessage()
//
// Description:
//      Processes the translation of various tray messages into the
// appropriate Shell_NotifyIcon() call by loading the NOTIFYICONDATA
// structure with the appropriate data.
//
// Parameters:
//      hDlg            - [IN] Dialog handle owning tray item
//      dwMessage       - [IN] Message to send tray icon
//      hIcon           - [IN] Icon to set tray icon to
//      lpszTip         - [IN] Tip to assign to icon
//
// Return Value:
//      BOOL            - FALSE if failed; TRUE if okay
//
// See Also:
//      TRAYNOT sample program in Win95 SDK.
//
//***************************************************************************
// 12/05/1994 MARKL Function Created.
//***************************************************************************
BOOL PASCAL SendTrayMessage (           // Setup for call to Shell_NotifyIcon
    HWND        hDlg,                   // [IN] Dialog hwnd owning tray icon
    DWORD       dwMessage,              // [IN] Message to send tray icon
    HICON       hIcon,                  // [IN] Icon to set tray icon to
    LPSTR       lpszTip)                // [IN] Tip to assign to tray icon
{
   NOTIFYICONDATA      TrayIconData;

   SYM_ASSERT (hDlg);
                                        // Initialize the TrayIconData
                                        // structure.

   TrayIconData.cbSize                 = sizeof (NOTIFYICONDATA);
   TrayIconData.uID                    = ID_TRAY_ICON;
   TrayIconData.uFlags                 = NIF_MESSAGE | NIF_ICON | NIF_TIP;
   TrayIconData.uCallbackMessage       = APP_MESSAGE_TRAY_NOTIFY;

   TrayIconData.hWnd                   = hDlg;
   TrayIconData.hIcon                  = hIcon;

                                        // Load the correct tip string.
   if (lpszTip)
   {
      STRNCPY (TrayIconData.szTip, lpszTip, sizeof (TrayIconData.szTip));
   }
   else
   {
      TrayIconData.szTip[0] = EOS;
   }

                                        // Tell the tray icon to do what
                                        // we just set up.
   return (Shell_NotifyIcon (dwMessage, &TrayIconData));
} // SendTrayMessage()



//***************************************************************************
//
// static VOID CALLBACK TrayTimerProc (
//     HWND  hwnd,
//     UINT  uMsg,
//     UINT  idEvent,
//     DWORD dwTime
//     );
//
// Description:
//      This function is called every TRAY_CHECK_TIME_INTERVAL millisecond
//      and it makes sure that the NAVAP icon is in the tray.
//
// Parameters:
//      Look at Windows 95 SDK
//
// Return Value:
//      -
//
// Note:
//      None of the strnigs in this function should be translated!
//
//***************************************************************************
static VOID CALLBACK TrayTimerProc (
    HWND  hwnd,
    UINT  uMsg,
    UINT  idEvent,
    DWORD dwTime
    )
{
BOOL bEnabled;

                                        // Don't update our status if we are
                                        // in the startup pending state.
   if (CFG_GET_STARTUP_PENDING)
      return;

   bEnabled = CFG_GET_ENABLE;
                                        // First, check the status again.
   MainDlgCheckStatus();

   if ( bEnabled != CFG_GET_ENABLE )
   {
                                        // Update the user interface to
                                        // reflect this new state.
      MainDlgShowEnableDisable (hwnd);

                                        // Since the tray may have appeared
                                        // later on, make sure we then hide
                                        // the minimized app for a smooth
                                        // transition.
      if (IsIconic (hwnd) && IsWindowVisible (hwnd) && NewShellTrayPresent() )
      {
         ShowWindow (hwnd, SW_HIDE);
      }
   }
}


//***************************************************************************
//
// void PASCAL MainDlgOptions(
//     HWND hDlg );
//
// Description:
//
// Parameters:
//
// Return Value:
//
// Note:
//      no strings to be translated.
//
//***************************************************************************
// 05/31/1995 Keith Mund Function Created.
//***************************************************************************
void PASCAL MainDlgOptions (
    HWND    hDlg )
{
    char            szModulePath[SYM_MAX_PATH];
    char            szStartDir[SYM_MAX_PATH];
    HWND            hNavwWnd = NULL;
    HWND            hCurrWnd = NULL;
    HINSTANCE       hInstModule;
    NOTIFYSTRUCT    Notify = {0};

    GetStartDir ( CFG_GET_INSTANCE, szStartDir, sizeof(szStartDir) );
    STRCPY ( szModulePath, szStartDir );
    NameAppendFile ( szModulePath, SZ_NAVW_PROGRAM );


    // Any Navw windows open?
    if (MainDlgGetCurrentNavWindow( (LPHANDLE)&hNavwWnd, (LPHANDLE)&hCurrWnd))
        {    
        ShowWindow( hCurrWnd, SW_RESTORE );
        SetForegroundWindow(hCurrWnd);

        // If it is the main window only, then tell
        // the window proc to put up the Options.

        if ( hCurrWnd == hNavwWnd )
            {
            g_TalkAPService.SendCommand(NAVAPSVC_DISPLAY_NAVW_OPTIONS);
            }
        }
    else
        {
        hInstModule = ShellExecute ( hDlg,
                                     "open",
                                     szModulePath,
                                     "/O:1",
                                     szStartDir,
                                     SW_SHOWNORMAL );

        if ( (WORD)hInstModule <= 32 )
            {
            unsigned    uMsgID;

            switch ( (WORD)hInstModule )
                {
                case 0:
                case 8:
                    uMsgID = IDS_ERR_AP_CANT_EXEC_NAVW_MEM;
                    break;

                case 2:
                case 3:
                case 11:
                    uMsgID = IDS_ERR_AP_CANT_EXEC_NAVW_BAD_FILE;
                    break;

                default:
                    uMsgID = IDS_ERR_AP_CANT_EXEC_NAVW;
                    break;
                }

            Notify.hParent          = hDlg;
            Notify.uMsgID           = uMsgID;
            Notify.uTitleID         = IDS_APP_CAPTION;
            Notify.hInstance        = CFG_GET_INSTANCE;
            NotifyUser (&Notify);
            }
        }

    return;

}



//***************************************************************************
// NewShellTrayPresent()
//
// Description:
//      Returns TRUE if the Windows 95 Tray is present.  This will let the
//      application know if it can plug in to the tray or not.
//
// Parameters:
//      none
//
// Return Value:
//      BOOL    - TRUE if tray is present, false otherwise.
//***************************************************************************
// 06/06/1995 MARK LAWLER Function Created.
// 05/02/1996 DSACKIN - Changed to be NewShell function (also used by NT4.0)
//***************************************************************************
BOOL PASCAL NewShellTrayPresent (       // Returns TRUE if tray present
    void)                               // no parameters
{
    BOOL bNewShellTrayPresent = FALSE;

                                        // Check to see if the tray is there.
                                        // Return to caller
    if ( FindWindow ( "Shell_TrayWnd", NULL ) != NULL )
        bNewShellTrayPresent = TRUE;

    return (bNewShellTrayPresent);
} // NewShellTrayPresent()


#ifdef NAV_PREVIEW

BOOL LOCAL IsTrialValid()
{
    // Initalize COM
    CoInitialize(NULL);

    ITrialWare* pTrial;
    BOOL bIsValid = FALSE;

    // attempt to create trial object
    if ( SUCCEEDED( CoCreateInstance( CLSID_TrialWare, 
                                      NULL,
                                      CLSCTX_INPROC_SERVER, 
                                      IID_ITrialWare, 
                                      (LPVOID*)&pTrial )))
    {
        // attempt to initialize trial object

        if ( SUCCEEDED( pTrial->Initialize( TEXT( "Symantec" ),
                                            TEXT( "Norton AntiVirus" ), 
                                            NAV_VERSION_MAJOR, 
                                            NAV_VERSION_MINOR )))
        {
            if ( pTrial->IsValid( HWND_DESKTOP, TRUE, NULL ) == S_OK )
                bIsValid = TRUE;
            else
            {
               char  szFormat[1024], szText[1024], szCaption[100];
               DWORD dwTrialLength = 30, dwBufSize = sizeof(DWORD);
               HKEY  hkey;
               LONG  lRet;

               LoadString (hInstance, IDS_LICENSEEXPIRED, szFormat, sizeof(szFormat) - 1);
               LoadString (hInstance, IDS_APP_CAPTION, szCaption, sizeof(szCaption) - 1);

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

               MessageBox(NULL, szText, szCaption, MB_OK | MB_ICONINFORMATION);
            }

        }

        pTrial->Release();
    }

    // Balance call to CoInitalize;
    CoUninitialize();

    return (bIsValid);
}

#endif  // NAV_PREVIEW


//************************************************************************
// MainDlgActivateNavw()
//
// BOOL PASCAL MainDlgActivateNavw ()
//
// This launches or restores the NAVW. 
//
// Returns:
//      TRUE on success.
//      FALSE otherwise.
//************************************************************************
// 03/10/1998 TCASHIN, created.
//************************************************************************

BOOL PASCAL MainDlgActivateNavw ()
{
    auto    TCHAR               szNavwExePath [SYM_MAX_PATH];
    auto    TCHAR               szNavwDir [SYM_MAX_PATH];
    auto    HWND                hNavwWnd  = NULL;
    auto    HWND                hCurrWnd  = NULL;
    auto    SHELLEXECUTEINFO    NavwStartInfo = {0};

    // Look for Navw's current window
    if (MainDlgGetCurrentNavWindow( (LPHANDLE)&hNavwWnd, (LPHANDLE)&hCurrWnd))
    {    
        // Open it if there is one.
        ShowWindow( hCurrWnd, SW_RESTORE );
        return ( SetForegroundWindow( hCurrWnd ) );
    }
	else
	{
        // Start Navw using ShellExecuteEx()
        MainDlgGetNavwExePath ( szNavwExePath, szNavwDir );

        NavwStartInfo.cbSize = sizeof(NavwStartInfo);
        NavwStartInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;
        NavwStartInfo.lpVerb = "Open";
        NavwStartInfo.lpFile = szNavwExePath;
        NavwStartInfo.nShow  = SW_SHOW;

        return ( ShellExecuteEx(&NavwStartInfo) );
	}

} // MainDlgActivateNavw()


//************************************************************************
// MainDlgGetCurrentNavWindow()
//
// BOOL PASCAL MainDlgGetCurrentNavWindow (lpMainWindow, lpCurrentWindow)
//
// This return the handle of Navw's main window and
// the handle of the currently open windows (dialog, etc.).
//
// See warning re: bug in GetLastActivePopup()
//
// Returns:
//      TRUE if any Navw windows are open
//      FALSE otherwise.
//************************************************************************
// 03/10/1998 TCASHIN, created.
//************************************************************************

BOOL PASCAL MainDlgGetCurrentNavWindow (LPHANDLE lpMainWindow, LPHANDLE lpCurrentWindow)
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
                                  MainDlgNavwThrdWinEnum, 
                                  MAKELPARAM(LOWORD(&hNavwScanWnd), HIWORD(&hNavwScanWnd)));

            if ( hNavwScanWnd )
            {
                *lpCurrentWindow = hNavwScanWnd;
            }
        }

        return ( bResult );

    }
}


//************************************************************************
// MainDlgNavwThrdWinEnum()
//
// static BOOL CALLBACK MainDlgNavwThrdWinEnum ( HWND hWnd, LPARAM lParam )
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

static BOOL CALLBACK MainDlgNavwThrdWinEnum ( HWND hWnd, LPARAM lParam )
{

    const   TCHAR           szScanWindowClass [] = SZ_SCAN_WNDCLASS;
    auto    TCHAR            szClass [ MAX_WNDCLASS ];
    auto    HWND            *lphScanWnd;

    GetClassName( hWnd, szClass, MAX_WNDCLASS );

    if ( 0 == _tcsicmp( szScanWindowClass, szClass ) )
    {
    	lphScanWnd = (HWND *)MAKELP(HIWORD(lParam), LOWORD(lParam));
        *lphScanWnd = hWnd;        
        return (FALSE);
    }
    else
        return (TRUE);

} //MainDlgNavwThrdWinEnum


//////////////////////////////////////////////////////////////////////////
// MainDlgGetNavwExePath
//
// Purpose:
//    Set lpszNavw to the full path to NAVW32/NAVWNT based on platform
//
// Parameters:
//    lpszNavw  [out] - The full path to NAV.
//    lpszDir   [out] - The directory portion of lpszNavw.
//
//////////////////////////////////////////////////////////////////////////
// 05/28/1997 JBRENNA - Function Created
// 03/12/1998 TCASHIN - Moved from ApwNtCmd.cpp
//////////////////////////////////////////////////////////////////////////

void PASCAL MainDlgGetNavwExePath ( LPSTR lpszNavw, LPSTR lpszDir )
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
    if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT )
        lpszExeKey = szNavNTKey;
    else
        lpszExeKey = szNav95Key;
        
    //  Get the OtherSwitches value from the CmdLines key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, lpszExeKey, 0, KEY_READ, &hBaseKey))
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
}
