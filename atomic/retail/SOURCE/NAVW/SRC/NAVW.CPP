// Copyright 1992-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVW/VCS/navw.cpv   1.73   29 Oct 1998 18:55:58   CEATON  $
//
// Description:
//      This is the main entry point for the Norton AntiVirus Windows
//      Clinic.
//
// Contains (as exports):
//      NavMainWndProc(), NavMainWnd3DEnumProc()
//
// See Also:
//      Navwmcmd.cpp
//************************************************************************
// $Log:   S:/NAVW/VCS/navw.cpv  $
// 
//    Rev 1.73   29 Oct 1998 18:55:58   CEATON
// Modified call to GetSystemWorksKey() to return two parameters.
// 
// 
//    Rev 1.72   21 Oct 1998 17:08:46   CEATON
// Replaced registry key text with #defined value in ABOUTSW.H.
// 
//    Rev 1.71   20 Oct 1998 20:30:30   CEATON
// Changed registry key enabling SystemWorks upsell dialog box.
// 
// 
//    Rev 1.70   20 Oct 1998 10:06:02   rchinta
// Added What's this help ID for NSW button on the main UI.
// 
//    Rev 1.69   13 Oct 1998 20:43:38   rchinta
// Changed NavMainWndCreate the MainUI with/without SystemWorks
// upsell button based on the SystemWorks registry.
// 
//    Rev 1.68   15 Sep 1998 18:47:00   CEATON
// Added initialization function for file notifications, and rearranged order of
// function calls so that file notifications don't become recursive.
// 
// 
//    Rev 1.67   14 Aug 1998 11:55:52   mdunn
// Changed trial-expired message to support varying-length trial periods.
// 
//    Rev 1.66   27 Jul 1998 15:05:16   DBuches
// Added code to remove additional buttons from the toolbar if we have to.
//
//    Rev 1.65   23 Jul 1998 16:13:24   tcashin
// In IsQuarantineValid(), check to see if QCONSOLE exists.
//
//    Rev 1.64   10 Jul 1998 12:21:34   DBuches
// Added call to Load StatusIcons().
//
//    Rev 1.63   07 Jul 1998 17:47:04   tcashin
// When checking for the /HMEM switch, specify CFG_SW_MINIMUM.
//
//    Rev 1.62   01 Jul 1998 13:58:38   DBuches
// Added more help topics.
//
//    Rev 1.61   29 Jun 1998 16:32:46   VGLOSTE
// Added a couple of necessary #defines for the new trialware code.
//
//    Rev 1.60   29 Jun 1998 15:19:26   VGLOSTE
// Changed the setup trialware to respect an override regkey that can be used
// to lengthen the trialware period.
//
//    Rev 1.59   26 Jun 1998 16:10:20   jtaylor
// Removed Quarantine and AP from OEM versions.
//
//    Rev 1.58   26 Jun 1998 09:05:42   tcashin
// Re-arranged the checks for the /HMEM command line switch.
//
//    Rev 1.57   20 Jun 1998 13:38:04   tcashin
// Always update the secondary status in the status timer function since
// we're no longer counting the quarantine items every time the timer runs.
//
//    Rev 1.56   17 Jun 1998 17:18:16   DBuches
// Disable AP enable button if virus defs are bad.
//
//    Rev 1.55   15 Jun 1998 10:35:56   DBuches
// Disable Updating of quarantine status if we are launched with the /HMEM switch.
//
//    Rev 1.54   11 Jun 1998 12:48:42   tcashin
// Added the quarantine monitor thread.
//
//    Rev 1.53   05 Jun 1998 09:41:12   DBuches
// Fixed problem with removing toolbar buttons when dialog is too small.  Wasn't accounting for gripper in calculation.
//
//    Rev 1.52   01 Jun 1998 16:25:42   DBuches
// Disble Scan Path menu item when no defs are present.
//
//    Rev 1.51   01 Jun 1998 15:01:32   DBuches
// Added /? help.
//
//    Rev 1.50   01 Jun 1998 12:53:10   mdunn
// Fixed 113525 - disabled gradient caption bar painting on 98 and NT 5+
// since those versions of Windows already do gradients.
//
//    Rev 1.49   28 May 1998 15:32:52   DBuches
// Now, only enable AP Enable/Disable button if the options say we can.
//
//    Rev 1.48   22 May 1998 00:11:20   JBRENNA
// Rearranged the COM include files.
//
//    Rev 1.47   19 May 1998 15:13:34   DBuches
// Remove the scheduler button if there is no room for it.
//
//    Rev 1.46   18 May 1998 13:37:50   DALLEE
// Removed keyboard accelerator - unused now that main UI toolbar no longer
// has hotkeys.
// Moved INITGUID and "iquaran.h" before "navutil.h" which has a nested
// include of "iquaran.h". Was no allocating the GUIDs.
//
//    Rev 1.45   18 May 1998 21:00:14   tcashin
// Init/Uninit the COM library during startup and shutdown.
//
//    Rev 1.44   29 Apr 1998 14:21:10   DBuches
// Hooked up toolbar tooltips.
//
//    Rev 1.43   28 Apr 1998 09:14:22   DALLEE
// Legitimate (fairly) updates for AP, Quar, and Def status displayed
// in main dialog.
// May still need to move Quarantine status updates to a separate thread
// if it's a big time hit.
//
//    Rev 1.42   23 Apr 1998 21:04:50   DALLEE
// Kluged updates of real-time status displays on main UI for
// usability.
//
//    Rev 1.41   14 Apr 1998 23:37:42   DALLEE
// Added Quarantine toolbar button and menu item.
// Removed code related to previously nuked AP toolbar button.
//
//    Rev 1.40   10 Apr 1998 14:40:46   MKEATIN
// Reverted back to -r1.37.  The options kluging for Atomic is done in
// optsmask.cpp in navxtuil.
//
//    Rev 1.37   01 Apr 1998 14:47:34   DALLEE
// Replaced button bar with new flat IE style toolbar.
// Removed old unused NAVNET stuff and some W16 vestiges also.
//
//    Rev 1.36   16 Mar 1998 19:42:20   MKEATIN
// Added Decomposer Init and DeInit routines.
//
//    Rev 1.35   13 Mar 1998 17:00:28   tcashin
// Process the UWM_DISPLAY_OPTIONS message from AP. This will put up
// the options dialog when Navw is running.
//
//    Rev 1.34   02 Dec 1997 10:47:10   JBRENNA
// In IsLiveUpdateValid, pass the full path (rather than the NAVLU32.EXE
// filename only) to NavLuIsLuInstalled(). This was causing the LiveUpdate
// button to disappear when the current directory is not the NAV install
// directory.
//
//    Rev 1.33   01 Dec 1997 19:05:14   jtaylor
// Added code to remove the Product Support Online submenu for Scanner only.
//
//    Rev 1.32   01 Dec 1997 16:18:08   jtaylor
// AP button and menu item are now removed from scanner only products
//
//    Rev 1.31   24 Nov 1997 19:34:06   jtaylor
// Added code to remove the innoculation menu item and the seperator above it on scanner only installs for Windows 95.
//
//    Rev 1.30   20 Nov 1997 11:38:10   jtaylor
// Added IsLiveUpdateValid function to determine if LiveUpdate DLL s32live1.dll
// and navlu32.exe are installed.  Used it to switch the LiveUpdate button and
// menu items.
//
//    Rev 1.29   14 Aug 1997 16:50:16   MKEATIN
// Call ExcludeTerminateEx with the proper flag so we only free the memory.
// We were saving EXCLUDE.DAT every time we exited NAV causing NAVAP to
// reload in configuration and therefore leaving a 'hole' in AP protection
// during that time.
//
//
//    Rev 1.28   24 Jul 1997 16:14:26   DBUCHES
// 2 changes for this rev.
//   1) Swapped position of LiveUpdate and AutoProtect toolbar buttons for
//      NT platform
//   2) Removed references to URLs from Help menu for NT3.51 systems.
//
//    Rev 1.27   21 Jul 1997 17:45:16   DBUCHES
// Fixed several problems relating to handling SysCommand Messages
//
// We now make sure a scan is in progress AND hScanMain is valid before
// performing special processing of WM_SYSCOMMAND messages.
//
//    Rev 1.26   21 Jul 1997 01:58:40   BGERHAR
// Removed /SHOWRESULTS (not implementing) and obsolete /NORESULTS implementation
//
//    Rev 1.25   18 Jul 1997 18:03:22   DBUCHES
// Need to quit if we cannot open NAVOPTS.DAT.
//
//    Rev 1.24   18 Jul 1997 17:25:18   BGERHAR
// [WIP] Add /SHOWRESULTS command line, fix /NORESULTS, add SetFocus() for after DDE with NT 351
//
//    Rev 1.23   15 Jul 1997 14:12:18   DBUCHES
// Added Ctl3d for DEC Alpha
//
//    Rev 1.22   11 Jul 1997 16:35:14   DBUCHES
// Added special case code for undocumented /VISIBLE switch.
//
//    Rev 1.21   10 Jul 1997 14:09:00   MKEATIN
// TrialWare will now look for NAV_VERSION_MAJOR and MINOR instead of the
// hard coded 2, 0.
//
//    Rev 1.20   02 Jul 1997 14:50:14   DBUCHES
// Fixed problem with handling WM_SYSCOMMAND message.  You *MUST* mask
// off the 4 low-order bits of WParam, as they are used internally by
// Windows.
//
// This fixes several bugs relating to minimizing and restoring NAVW and
// the scanning window.
//
//    Rev 1.19   01 Jul 1997 14:35:14   TIVANOV
// move functions that are specific to navabout
//
//    Rev 1.18   26 Jun 1997 16:38:20   TIVANOV
// checking the command line for the new switch NORESULTS
//
//    Rev 1.17   18 Jun 1997 21:51:08   JBRENNA
// Add call to NAVSetDefsPath before calling VLRegisterVirusDB. This tells
// our AVAPI1 callbacks the location of the definition files.
// Also, integrated a fix from MKEATIN ... only use high memory scanning if
// the command line specified high memory scanning.
//
//    Rev 1.16   18 Jun 1997 11:29:44   MKEATIN
// Added &gstNAVCallBacks to VLScanInit().
//
//    Rev 1.15   17 Jun 1997 15:57:26   TIVANOV
// remove a line of code that updates the usage.dat with wrong info
//
//    Rev 1.14   13 Jun 1997 08:51:50   TIVANOV
// change GetDefinitionDate to return the def version number
//
//    Rev 1.13   11 Jun 1997 15:41:08   DALLEE
// Start RPC server on 95 now also, if running in normal program mode.
// Maybe should use a mutex to protect against the double RPC start problem
// on W95 instead, though.
//
//    Rev 1.12   10 Jun 1997 14:00:44   TIVANOV
// new messages for the about box
//
//    Rev 1.11   10 Jun 1997 14:01:16   DBUCHES
// Removed unecessary call to GetInfectionData().
//
//    Rev 1.10   09 Jun 1997 16:54:44   DBUCHES
// Ifdef'd out Ctl3d stuff for non-intel platfomrs.
//
//    Rev 1.9   04 Jun 1997 15:06:18   JBRENNA
// 1. Remove the g_bOnrampIsRunning processing. NAVW no longer waits for
//    LiveUpdate to finish before it continues processing.
// 2. Update LoadVirusDefs to use shared directory when creating a path
//    to VIRSCAN1.DAT.
// 3. Call NavVDefsVirusScanInit instead of VirusScanInit(),
//    Call NavVDefsVirusScanDeInit instead of VirusScanEnd()
//    The NavVDefs* functions use CDefUtils for shared definitions maintenance.
//
//    Rev 1.8   02 Jun 1997 18:50:40   JBRENNA
// Renamed NavIsLuInstalled() to NavLuIsLuInstalled().
//
//    Rev 1.7   16 May 1997 17:22:32   DALLEE
// Removed conflicting TRACE defines now that we're define SYM_MFC.
// TRACE seemed unused here.
// Removed references to "Netw.h" NAVNET-specific header file.
//
//    Rev 1.6   10 May 1997 02:05:30   JBRENNA
// Call NavIsLuInstalled() instead of OnrampIsInstalled().
//
//    Rev 1.5   22 Apr 1997 15:48:54   JBRENNA
// Add IDM_TOOLS_AP to the table in NavMainWndWM_MENUSELECT. This allows
// Tesla's AP button bar button to have help. Rename IDS_NOWINNT to
// IDS_NOTFORWINNT.
//
//    Rev 1.4   27 Mar 1997 15:50:56   DALLEE
// Removed old unused NDW drag drop code.
//
//    Rev 1.3   26 Mar 1997 21:05:26   DALLEE
// Fixed a typo of mine from HYDRA port in rev 1.1. "SymTril" to "SymTrial".
//
// Also, removed a few more superfluous #ifdef SYM_WIN32's I missed last rev.
//
//    Rev 1.2   26 Mar 1997 20:25:52   DALLEE
// Removed NAVNET and WIN16 code and #ifdefs.
// Only valid remaining targets are W32 - NAVW, NAVWNT, NAVSCANW.
// Did some manual "pretty printing" of the source as well.
//
//    Rev 1.1   06 Mar 1997 17:21:30   DALLEE
// Port from HYDRA.15
//     Updated trial-ware.
//     Network alert support.
//
//    Rev 1.0   06 Feb 1997 21:01:28   RFULLER
// Initial revision
//
//    Rev 1.46   05 Nov 1996 13:51:32   MKEATIN
// Added bCRCCheckPassed flag.  ExcludeTerminate() should not be called if
// the CRC check failed.  If it is, it will unconditionally delete
// exclude.dat since ExcludeInit() was never called. (ExcludeTerminate()
// deletes exclude.dat if the number of entries is zero)
//************************************************************************

#include "platform.h"                   // Platform independence
#include "xapi.h"
#include "ngui.h"                       // Norton look / feel
#include <ctl3d.h>                      // Microsoft 3D look


#include <commdlg.h>                    // Microsoft common dialog
#include "disk.h"                       // For drive types
#include "ndrvlist.h"                   // For Norton drive list control
#include "msgdisp.h"
#include "stdhelp.h"
#include "msgreg.h"
#include "navw_hlp.h"                   // Help ids
#include "aboutsw.h"

#ifndef INITGUID                        // Define INITGUID only once per module
  #define  INITGUID
  #include "objbase.h"
  #include "iquaran.h"
  #include "quar32.h"
#endif

#include "navutil.h"

#ifdef NAV_PREVIEW
  #include "tryint.h"
  #define DEFAULT_TRIAL_LENGTH 30
  #define MAX_TRIAL_LENGTH 90
#endif

#include "navstat.h"
#include "symstat.h"                    // Low level disk arbitration

#include "virscan.h"
#include "virlist.h"
#include "nonav.h"
#include "inoc.h"

#include "navwres.h"                    // NAV resource identifiers
#include "navwstb.h"                    // NAV string table entries
#include "exclude.h"
#include "security.h"
#include "navapcom.h"
#include "cmbstr.h"
#include "logio.h"
#include "navw.h"
#include "navalog.h"
#include "navaplog.h"
#include "navbin.h"
#include "common.h"
#include "file.h"
#include "dbcs.h"
#include "NavLuUtils.h"
#include "LinkSrv.h"                    // For RPC Server.

#include "navver.h"                     // #define NAV_APP_CLINIC

#include "servenfy.h"                   // for InitNLMMessenger function
#include "NavVDefs.h"                   // for NavVDefs* functions.

#include "n32behav.h"                   // Behavioral DLL.
#include "NavAppIds.h"
#include "DefUtils.h"
#include "navcomm.h"
#include "navcb.h"
#include "n_decomp.h"

#include "maintool.h"

#include "n32behav.h" // Personality DLL

#define WM_NAVW_INITDIALOG  ( WM_USER + 200 )

//************************************************************************
// Local definitions
//************************************************************************

					// Base name for memory-mapped files
					// to pass queued file requests.
					// Do not translate.
#define QUEUEFILE_MAPPEDFILE_NAME (TEXT("QueuedFileRequest"))


#define STATUS_TIMER_ID         4
#define STATUS_TIMER_TIMEOUT    3000    // in milliseconds.


//************************************************************************
// Local Typedefs
//************************************************************************
typedef struct tagMENUTOHELP
    {
    WORD        wMenuId;
    WORD        wHelpId;
    WORD        wStatusId;
    } MENUTOHELP;
typedef MENUTOHELP NEAR    *NPMENUTOHELP;
typedef MENUTOHELP FAR     *LPMENUTOHELP;

					// Request buffer for scanning a file
					// with a previous instance.
typedef struct tagQUEUEDFILEREQUEST
    {
    UINT            uResponse;
    TCHAR           szFile [SYM_MAX_PATH];
    } QUEUEDFILEREQUEST, FAR *LPQUEUEDFILEREQUEST;

//************************************************************************
// Local Prototypes
//************************************************************************

STATUS LOCAL SetNavAppPath (VOID);      // Add AppPath to environment.

BOOL LOCAL IsLiveUpdateValid(VOID);     // Returns true if LU is present

BOOL LOCAL IsQuarantineValid();         // True if quarantine present.

VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the message
    UINT    uMsg,                       // [in] " message "
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
);

BOOL LOCAL NavPrevInstActivate (        // Activates previous instance of NAV
    HINSTANCE hPrevInstance,
    LPNAVMAIN lpNavMain
    );

VOID LOCAL DoStartupChores (
    HWND        hWnd,
    WPARAM      wParam,
    LPNAVOPTIONS lpNavOptions
    );

BOOL LOCAL NetworkDrivesExist (         // Figure out if any network drives
    VOID                                // Nothing
    );

BOOL LOCAL NavRegisterClasses(          // Registers class used by NAV
    VOID);                              // No parameters

HWND LOCAL NavMainWndCreate (           // Creates main dialog wnd
    VOID                                // [in] Command Line to process
    );

LRESULT SYM_EXPORT CALLBACK NavMainWndProc (// Main dialog window proc
    HWND        hWnd,                   // [in] Dialog box handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam);                // [in] lParam to process

VOID LOCAL NavMainWndWM_CREATE(         // Handles WM_CREATE for main window
    HWND          hWnd);                // [in] Window handle

VOID LOCAL NavMainWndWM_INITDIALOG(     // Handles WM_CREATE for main window
    HWND      hWnd,                     // [in] Window handle
    LPNAVMAIN lpNavMain);               // [in] Data for main window

VOID WINAPI ActivateNetDrives (         // This function initializes drive LB
    HWND        hWnd,                   // [in] main window handle
    LPNAVMAIN   lpNav,                  // [in] good ol nav pointer
    LPTSTR       lpDrives                // [in] drives if any to add
    );

LRESULT LOCAL NavMainWndWM_CTLCOLOR(    // Handles WM_CTLCOLOR for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_CTLCOLOR message
    LPARAM      lParam);                // lParam of WM_CTLCOLOR message

VOID LOCAL NavMainWndWM_ERASEBKGND(     // Handles WM_ERASEBKGND for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_ERASEBKGND message
    LPARAM      lParam);                // lParam of WM_ERASEBKGND message

VOID LOCAL NavMainWndWM_MENUSELECT (    // Handles WM_MENUSELECT for main wnd
    HWND        hWnd,                   // [in] Main dialog window handle
    UINT        uIDItem);               // [in] Menu ID of selected item

BOOL LOCAL CheckIntegrity(VOID);

BOOL LOCAL ShouldProcessCommandLine (
    LPTSTR   lpszCmdLine
    );

UINT LOCAL QueueFileToPrevInst (HWND hPrevWnd);
UINT LOCAL NavMainWndNAV_MSG_QUEUEFILE (DWORD dwRequestBufferID);

int OnNCPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID DrawCaption(HWND hWnd, LPRECT lprDraw, BOOL bActive);
BOOL GradientFillRect(HDC pDC, LPRECT rect, COLORREF lColor, int nSteps, BOOL bVertical);

					//-------------------------------
					// In porting C++ code over from
					// NU, we need to fake the derived
					// class COffScreenDC:
					//-------------------------------
VOID LOCAL COffScreenDC_Constructor ( HDC pxDC, LPRECT rxRect ) ;
VOID LOCAL COffScreenDC_Destructor ( VOID ) ;
VOID LOCAL COffScreenDC_Transfer ( VOID ) ;

#ifdef NAV_PREVIEW
BOOL LOCAL SetupTrial( VOID );
BOOL LOCAL IsTrialValid( VOID );
BOOL LOCAL IsTrialPeriodOverridden(int *pTrialLength);
#endif

BOOL LOCAL IsBadPlatform ( VOID );

VOID LOCAL NavMainInitToolBar ( HWND hWnd );
VOID LOCAL NavMainAddToolBarButtons( CNavMainToolBarCtrl *pToolBar );
VOID LOCAL NavMainResizeControls ( HWND hWnd );
BOOL LOCAL HandleToolbarTip( WPARAM wParam, LPARAM lParam );

static VOID CALLBACK NavMainStatusTimer( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );

//************************************************************************
// Globals
//************************************************************************

NAVMAIN         NavMain;
HINSTANCE       ghInst;                 // Instance handle for application
HBITMAP         ghBmpOpening;           // Opening bitmap
HWND            ghWndMain;              // Main window / dialog box handle
int             gnCmdShow = SW_HIDE;    // How to display main window.
TCHAR           gszCmdLine[SYM_MAX_PATH+1] = {0};

BOOL            bCommandLine    = FALSE;
BOOL            g_bSageRun      = FALSE;
BOOL            g_bRunFromShell = FALSE;

WORD            gwNAVPresent = PLATFORM_NONE,
		gwPlatform[PLATFORM_COUNT] = {PLATFORM_NAV_95, PLATFORM_NAV_3};


HELPSTRUCT      HelpInfo            = { 0 };

WORD            g_wStartupTask      = 0;
WORD            wHelpCommonDialogID = 0;
WORD            wHelpBrowseMsg      = 0;
NAVOPTIONS      navOptions          = { 0 };    // Option Settings

BOOL            g_bNavapwOptions    = FALSE;
					// Number of the panel entry passed
UINT            g_uiOptionNumber;       // on the command line.
HWND            ghwndLastFocused    = NULL;

					//-------------------------------
					// In porting C++ code over from
					// NU, we need to fake the derived
					// class COffScreenDC. Fortunately,
					// only one instance is used at a
					// time, so we can use statics for
					// the class variables:
					//-------------------------------
static HDC thisCOffScreenDC;
static BOOL m_bTransfered;
static HBITMAP m_xBitmap;
static HBITMAP m_pxOldBitmap;
static HDC m_pxDC;
static RECT m_xRect;
static BOOL s_bUseGradientPaintCode = TRUE;


static BOOL s_bCommandLineHelp = FALSE;

/* **************************************************************************
@Name:
int PASCAL WinMain (                    // Main entry point
    HINSTANCE   hInstance,              // Instance handle
    HINSTANCE   hPrevInstance,          // Handle to previous instance
    LPTSTR       lpszCmdLine,            // Command line passed to program
    int         nCmdShow                // Command show style
    )

@Description:
 This is the main entry point for the Norton AntiVirus for Windows.

@Parameters:
$hInstance$     Instance handle given to us by Windows.
$hPrevInstance$ Previous instance handle. Valid only for W16
$lpszCmdLine$   pointer to command line given during startup.
$nCmdShow$      Flag setting on how we should visualize ourselves.

@Include: stdhelp.h
************************************************************************** */

int PASCAL WinMain (                    // Main entry point
    HINSTANCE   hInstance,              // Instance handle
    HINSTANCE   hPrevInstance,          // Handle to previous instance
    LPTSTR       lpszCmdLine,            // Command line passed to program
    int         nCmdShow                // Command show style
    )
{
    MSG         msg;                    // Message structure
    int         nRet = 0;               // Return code
    CFGTEXTREC  rSwitchShell = {0};

    ghInst = hInstance;

					 // Check the preview date and then run
#ifdef NAV_PREVIEW
    SetupTrial();

    if ( !IsTrialValid() )
	{
	return ( 0 );
	}
#endif // NAV_PREVIEW
					// IsBadPlatform() checks for
					// NT on 95 and vice versa.  Also
					// has no access to desktop check for
					// NT scheduled scans.
    if ( IsBadPlatform() )
	{
	return (0);
	}

                                        // Check the OS version, and disable
                                        // the gradient caption bar on
                                        // 98 and NT 5+
OSVERSIONINFO rOSInfo;

    ZeroMemory ( &rOSInfo, sizeof(OSVERSIONINFO) );

    rOSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx ( &rOSInfo );

    if ( rOSInfo.dwMajorVersion > 4 ||      // NT 5 or later
         ( rOSInfo.dwMajorVersion == 4 &&
           rOSInfo.dwMinorVersion > 0 ))    // 98
        {
        s_bUseGradientPaintCode = FALSE;
        }


    DBCSSetInternalTable ();

					// Set our AppPath if not done for us.
					// Then dynamically load S32STAT.DLL
    SetNavAppPath();

    if ( NOERR != NavStatInit() )
	{
	TCHAR    szErrorStarting [200];
	TCHAR    szNoSymStat [200];

	LoadString(hInstance, IDS_ERROR_STARTING, szErrorStarting, sizeof(szErrorStarting)-1);
	LoadString(hInstance, IDS_NO_SYMSTAT, szNoSymStat, sizeof(szNoSymStat)-1);
	MessageBox(NULL, szNoSymStat, szErrorStarting, MB_OK | MB_ICONEXCLAMATION);

	return ( 0 );
	}

    if ( NavPrevInstActivate ( hPrevInstance, &NavMain ) )
	{
	NavStatEnd();
	return ( 0 );
	}
					// -----------------------------------
					// Determine if this is the regular
					// version, or a custom version:
					// -----------------------------------
    SetCustomVersion();

					// -----------------------------------
					// Disable the VXD for this thread.
					// -----------------------------------
    TalkToVxD ( VXDACTION_DISABLE );

					// -----------------------------------
					// Start RPC server so NAVAP can
					// notify us if it launches later.
					//
					// Kluge: On Win95, only attempt to
					// start NAVW's RPC server from one
					// process. If a second (i.e. download)
					// instance tries to start a 2nd
					// RPC server, it will fault on exit.
					// -----------------------------------
    if ( ( SYM_SYSTEM_WIN_95 != SystemGetWindowsType() ) ||
	 ( FALSE == NavMain.bDownloadMode ) )
	{
	NavStartRPCServer();
	}

    SetHandleCount (30);
    SetErrorMode ( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

					// -----------------------------------
					// This fix is required because NDW
					// attempts to hide us and when that
					// happens the user can't do anything
					// with NAVW
					// -----------------------------------
    if (nCmdShow == SW_HIDE)
	nCmdShow = SW_SHOWNORMAL;

    gnCmdShow = nCmdShow;

					//------------------------------------
					// Uppercase all non-DBCS characters
					// that are parameters from the
					// command line.
					//------------------------------------
    ConfigParamToUpperNonDbcs();

					// -----------------------------------
					// Start NAVSERVE building our list
					// -----------------------------------
    InitNLMMessenger();

					// -----------------------------------
					// Enable Ctrl3D if not under new shell.
					// Doesn't need to be explorer shell,
					// just needs new look functionality.
					// -----------------------------------
    if (FALSE == WinInfoIsNewShell())
	{
	Ctl3dRegister (ghInst);
	Ctl3dAutoSubclass(ghInst);
	}

					// -----------------------------------
					// GUI classes initialization
					// -----------------------------------
    NG_Init();

                    //
                    // Initialize status icon array.
                    //
    LoadStatusIcons();

    auto    INITCOMMONCONTROLSEX    iccex;

    memset( &iccex, 0, sizeof( iccex ) );
    iccex.dwSize = sizeof( iccex );
    iccex.dwICC  = ICC_COOL_CLASSES | ICC_WIN95_CLASSES;

    InitCommonControlsEx( &iccex );


    if (STRLEN (lpszCmdLine) != 0)
	{                               // bCommandLine is set elsewhere
	STRCPY (gszCmdLine, lpszCmdLine);

	// if Virscan.dat or Virscan2.dat is missing/corrupted
	// display a message and exit (Command Line)
	if ( NavVDefsVirusScanInit() == ERR)
	    {
	    NOTIFYSTRUCT    Notify  =   { 0 };

	    Notify.hParent       = GetDesktopWindow();
	    Notify.uMsgID        = IDS_NODEFS;
	    Notify.uTitleID      = IDS_NAVW;
	    Notify.lpAlertMsg    = NULL;
	    Notify.hInstance     = hInstance;

	    NotifyUser (&Notify);
	    return ( 0 );
	    }
	}

    if ( ReadNavBin   ( &navOptions ) == ERR ||
	 ReadNavStart ( &navOptions ) == ERR )
	{
	g_wStartupTask = NAVW_STARTUP_NAVBIN;
	}


#ifndef NAVSCANW                        // If this is Gargoyle, don't
					// check the AutoProtection options.

					// -----------------------------------
					// Make sure that the options in the
					// registry matches NAVW options.
					// -----------------------------------

    BOOL bLoadVxdStartup = RegistryIsVxDStartup();

    if ( navOptions.startup.bLoadVxD == TRUE )
	{
	if ( bLoadVxdStartup == FALSE )
	    {
	    if ( RegistryAddVxD() == FALSE )
		{
		g_wStartupTask = NAVW_STARTUP_VXDADDERROR;
		}
	    }
	}
    else
	{
	if ( bLoadVxdStartup == TRUE )
	    {
	    if ( RegistryRemoveVxD() == FALSE )
		{
		g_wStartupTask = NAVW_STARTUP_VXDREMOVEERROR;
		}
	    }
	}
#endif

					// -----------------------------------
					// This flag indicates if NAV should
					// process command lines.
					// -----------------------------------
    bCommandLine =  ShouldProcessCommandLine ( lpszCmdLine );


					// ----------------------------------
					// /HIMEM: Only scan high memory if
					//         this switch is set.
					// ----------------------------------

    CFGTEXTREC  rSwitch = {String_Ptr(IDS_HIMEM), NULL,};
    navOptions.general.bScanHighMemory = ConfigSwitchSet(&rSwitch, NULL);

                    // ----------------------------------
                    // /HMEM: Set by the shell extension
                    //        if it launched us.
                    // ----------------------------------

    rSwitchShell.lpSwitch = String_Ptr(IDS_MEMHANDLE);
    rSwitchShell.wFlags   = CFG_SW_MINIMUM;

    g_bRunFromShell = ConfigSwitchSet(&rSwitchShell, NULL);


    NAVDecomposerInit();
    InoculateInit();
    NavworkInit();

    String_Init(ghInst, NULL);

    if ( NavRegisterClasses() &&
	(ghWndMain = NavMainWndCreate()) != 0 )
	{
    NavMain.hScanMain= ghWndMain;
					// -------------------------------
					// Main messasge loop. Process all
					// messages until our app has been
					// terminated.
					// -------------------------------
	while ( GetMessage ( &msg, NULL, 0, 0) )
	    {
	    if ( !IsDialogMessage( ghWndMain, &msg ) )
		{
		TranslateMessage ( &msg );
		DispatchMessage  ( &msg );
		}
	    }

	String_End();
	nRet = msg.wParam;
	}

    if ( !g_bRunFromShell )
        {
        StopStatusMonitor(&NavMain);
        }

    InocEnd();
    InoculateEnd();
    NavworkEnd();
    NAVDecomposerEnd();
    NavVDefsVirusScanDeInit();

    Ctl3dUnregister (ghInst);

    NG_Done();

					// -----------------------------------
					// If started, stop listening on
					// RPC server for NAVAP activating
					// messages.
					// -----------------------------------
    if ( ( SYM_SYSTEM_WIN_95 != SystemGetWindowsType() ) ||
	 ( FALSE == NavMain.bDownloadMode ) )
	{
	NavStopRPCServer();
	}

					// -----------------------------------
					// Re-enable the VXD for this thread.
					// -----------------------------------
    TalkToVxD( VXDACTION_ENABLE );

					// Register with SymStat
    NavStatFreeInstance( NavMain.bDownloadMode ? NAV_APP_CLINIC
					       : NAV_APP_DOWNLOAD );
    NavStatEnd();

    return ( nRet );
} // WinMain()


/* **************************************************************************
@Name:
BOOL LOCAL NavRegisterClasses(           // Registers class used by NAV
    VOID                                 // No parameters
    );

@Description:
This routine registers the classes required by the Norton Antivirus
Windows Clinic.  The first is the main application dialog (we use a
dialog as our main window).  The second class is for the window that
handles the opening bitmap.

@Parameters: None

@Include: stdhelp.h
************************************************************************** */

BOOL LOCAL NavRegisterClasses(          // Registers class used by NAV
    VOID                                // No parameters
    )
{
    auto    WNDCLASSEX  wcex;
    auto    BOOL        bSuccess;

					// Register main window class.

    memset( &wcex, 0, sizeof( wcex ) );

    wcex.cbSize         = sizeof( wcex );
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = NavMainWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = DLGWINDOWEXTRA;
    wcex.hInstance      = ghInst;
    wcex.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground  = ( HBRUSH )( COLOR_BTNFACE + 1 );
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = NAV_MAIN_WND_CLASS;

    wcex.hIconSm = ( HICON )LoadImage( wcex.hInstance,
				       MAKEINTRESOURCE( IDI_NAVW_APP_SMALL ),
				       IMAGE_ICON,
				       16, 16 , 0 );
    wcex.hIcon   = ( HICON )LoadImage( wcex.hInstance,
				       MAKEINTRESOURCE( IDI_NAVW_APP ),
				       IMAGE_ICON,
				       32, 32, 0 );

    bSuccess = ( RegisterClassEx( &wcex ) ? TRUE : FALSE );

					// Register splash screen class

    memset( &wcex, 0, sizeof( wcex ) );

    wcex.cbSize         = sizeof( wcex );
    wcex.style          = 0;
    wcex.lpfnWndProc    = NavOpeningBmpWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = ghInst;
    wcex.hCursor        = NULL;
    wcex.hbrBackground  = ( HBRUSH )( COLOR_BTNFACE + 1 );
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = NAV_OPENING_BMP_CLASS;
    wcex.hIconSm        = NULL;
    wcex.hIcon          = NULL;

    bSuccess = ( bSuccess && RegisterClassEx( &wcex ) );

    return ( bSuccess );
} // NavRegisterClasses()


//************************************************************************
// SetNavAppPath()
//
// This routine reads the NAV path from the registry and prepends it
// to the path for our current environment.
// (Kluge for NT where AppPath isn't set automatically)
//
// Parameters:
//  None.
//
// Returns:
//  NOERR on success.  ERR if AppPath key not found.
//************************************************************************
// 10/15/95 DALLEE, created.
//************************************************************************

STATUS LOCAL SetNavAppPath (VOID)
{
    auto    HKEY    hAppPathKey = NULL;
    auto    TCHAR    szAppPathKey [SYM_MAX_PATH];
    auto    TCHAR    szPathValue [SYM_MAX_PATH];

    auto    LPTSTR   lpPathBuffer = NULL;
    auto    LPTSTR   lpOrigPathBuffer = NULL;
    auto    DWORD   dwBufferSize;
    auto    DWORD   dwOrigPathSize;
    auto    DWORD   dwBytesRead;

    auto    STATUS  nStatus = ERR;


    if ( (0 == LoadString(ghInst, IDS_NAV_APP_PATH_KEY, szAppPathKey, sizeof(szAppPathKey)-1))
	    ||
	 (0 == LoadString(ghInst, IDS_PATH, szPathValue, sizeof(szPathValue)-1)) )
	{
	goto BailOut;
	}

					// Find size of buffer to allocate.
					// Add size of AppPath value and
					// Path env var.
    if ( (ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, szAppPathKey, &hAppPathKey))
	    ||
	 (ERROR_SUCCESS != RegQueryValueEx(hAppPathKey, szPathValue,
					   NULL, NULL, NULL, &dwBufferSize)) )
	{
	goto BailOut;
	}
					// Give bogus buffer with size 0
					// to find size of orig path.
    dwOrigPathSize = GetEnvironmentVariable(szPathValue, szPathValue, 0);
    dwBufferSize += dwOrigPathSize;

					// Get buffer for new path var.
    lpPathBuffer = (LPTSTR) MemAllocPtr(GMEM_MOVEABLE, dwBufferSize);
    if (NULL == lpPathBuffer)
	{
	goto BailOut;
	}
					// Put app path in front of new path.
    dwBytesRead = dwBufferSize;
    if (ERROR_SUCCESS != RegQueryValueEx(hAppPathKey, szPathValue, NULL,
					 NULL, (LPBYTE)lpPathBuffer, &dwBytesRead))
	{
	goto BailOut;
	}

					// If there is an existing path...
					// Need to check if AppPath is already
					// set. If not, then combine the two.
    if (0 != dwOrigPathSize)
	{
					// Get buffer and read old path.
	lpOrigPathBuffer = (LPTSTR) MemAllocPtr(GMEM_MOVEABLE, dwOrigPathSize);
	if (NULL == lpOrigPathBuffer)
	    {
	    goto BailOut;
	    }
	if (dwOrigPathSize <= GetEnvironmentVariable(szPathValue,
						     lpOrigPathBuffer,
						     dwOrigPathSize))
	    {
	    goto BailOut;
	    }

					// If already present, return success.
	if (0 == STRNICMP(lpPathBuffer, lpOrigPathBuffer, STRLEN(lpPathBuffer)))
	    {
	    nStatus = NOERR;
	    goto BailOut;
	    }

					// Append existing path.
	STRCAT(lpPathBuffer, ";");
	STRCAT(lpPathBuffer, lpOrigPathBuffer);
	}

					// Set the new environment variable.
    if (TRUE == SetEnvironmentVariable(szPathValue, lpPathBuffer))
	{
	nStatus = NOERR;
	}

BailOut:
    if (NULL != hAppPathKey)
	RegCloseKey(hAppPathKey);
    if (NULL != lpPathBuffer)
	MemFreePtr(lpPathBuffer);
    if (NULL != lpOrigPathBuffer)
	MemFreePtr(lpOrigPathBuffer);

    return (nStatus);
} // SetNavAppPath()


/* **************************************************************************
@Name:
VOID LOCAL NavPrevInstActivate (        // Activate previous instance
    HINSTANCE   hPrevInstance,          // [in] previous instance handle
    LPNAVMAIN   lpNavMain               // [in] nav structure pointer
    );

@Description:
If a previous instance of Norton AntiVirus for Windows is running, this
routine hunts it down and then activates it.
NOTE:  To simply use SetActiveWindow() to the NAVW window class is not
enough.  We must also show its owned popups.  This allows the caption
and input focus to appear correctly in the case that a modal dialog box
is up.

@Parameters: None

@Include: stdhelp.h
************************************************************************** */
// 08/22/1996 DALLEE, removed the old #ifdef SYM_WIN16 version.
//      Check Kirin product version 1.30 if you really want to see that
//      source.
// 08/30/1996 DALLEE, post WM_SYSCOMMAND SC_RESTORE to activate previous
//      instance - klugey hide/restore/move/minimize stuff is grouped there.
//***************************************************************************

BOOL LOCAL NavPrevInstActivate (        // Activate previous instance
    HINSTANCE   hPrevInstance,          // [in] previous instance handle
    LPNAVMAIN   lpNavMain               // [in] nav structure pointer
    )
{
    #define NAV_SCANWINDOW_CLASS "ScanWindow"
    HWND    hPrevWnd;

    auto    CFGTEXTREC  rSwitch = {0};
    auto    TCHAR       szDownload [SYM_MAX_PATH];

    szDownload[0] = EOS;

    LoadString(ghInst, IDS_DOWNLOAD, szDownload, sizeof(szDownload)-1);

    rSwitch.lpSwitch = szDownload;
    rSwitch.wFlags = CFG_SW_EXACT;

    lpNavMain->bDownloadMode = (BOOL)ConfigSwitchSet(&rSwitch, NULL);

					// Register with SymStat
    if ( ERR != NavStatAllocInstance ( lpNavMain->bDownloadMode ?
				       NAV_APP_DOWNLOAD : NAV_APP_CLINIC) )
	return ( FALSE );
					// If we can't register, we must
					// already be registered, get the inst.
    hPrevWnd = NavStatGetInstanceHwnd (lpNavMain->bDownloadMode ?
				       NAV_APP_DOWNLOAD : NAV_APP_CLINIC);

					// If there isn't a window, assume that
					// it hasn't been created yet. Don't let
					// another instance start.

    if ( hPrevWnd == NULL )
	{
					// Slight hack for trying to start
					// multiple instances at the same time:
					// Allow 10 seconds for the first to
					// finish WM_CREATE and register its
					// window handle.
	SleepEx ( 10000, FALSE );
	hPrevWnd = NavStatGetInstanceHwnd ( NAV_APP_CLINIC );
	if ( hPrevWnd == NULL )
	    {
	    return ( TRUE );
	    }
	}

    if (lpNavMain->bDownloadMode)
	{
					// Read command line and pass a file
					// scan request to the previous inst.
	switch (QueueFileToPrevInst(hPrevWnd))
	    {
					// Previous instance accepted request.
					// Don't load this NAV, and don't
					// restore previous minimized window.
	    case QUEUEFILE_WILL_SCAN:
		return (TRUE);

					// No response from previous instance.
	    case QUEUEFILE_NO_RESPONSE:
					// Allow a second instance - first
		return (FALSE);         // didn't respond (maybe was exiting).

					// Error sending.
	    case QUEUEFILE_NOT_SENT:
	    default:
		// Follow same rules as request denied.

					// Previous instance denied request -
					// probably not in download mode.
	    case QUEUEFILE_WONT_SCAN:
		return (FALSE);         // For now, allow second instance.
	    }
	}

					// We have an instance already, use it.
    HWND hScanWindow = FindWindow(NAV_SCANWINDOW_CLASS, NULL);

    if ( hScanWindow )
	hPrevWnd = hScanWindow;
					// Restore through WM_SYSCOMMAND
					// since that code knows how to
					// restore/move/hide the scan
					// and main windows correctly.
    SetForegroundWindow(hPrevWnd);
    PostMessage(hPrevWnd, WM_SYSCOMMAND, SC_RESTORE, 0);

    return ( TRUE );
} // NavPrevInstActivate()


//************************************************************************
// QueueFileToPrevInst()
//
// This routine reads the downloaded file from the command line and sends
// a scan request via NAV_MSG_QUEUEFILE to the window of the previous
// instance.
// The request is sent in a QUEUEDFILEREQUEST structure via a memory mapped
// file. The mapped file name is based on the HINSTANCE of the requester
// which is passed as the LPARAM of a NAV_MSG_QUEUEFILE message.
//
// Parameters:
//      HWND    hPrevWnd                Previous instance's main window.
//
// Returns:
//      UINT    QUEUEFILE_NOT_SENT      Error sending message.
//              QUEUEFILE_NO_RESPONSE   No response from previous instance
//              QUEUEFILE_WILL_SCAN     Previous instance accepted request.
//              QUQUEFILE_WONT_SCAN     Denied request - previous instance is
//                                          probably not in download mode.
//************************************************************************
// 01/22/95 DALLEE, created.
//************************************************************************

UINT LOCAL QueueFileToPrevInst (HWND hPrevWnd)
{
    auto    TCHAR               szRequestBufferName [SYM_MAX_PATH + 1];
    auto    HANDLE              hRequestBuffer = NULL;
    auto    LPQUEUEDFILEREQUEST lpRequestBuffer = NULL;

    auto    UINT        uReturn = QUEUEFILE_NOT_SENT;
    auto    TCHAR       szzDownloadedFile [SYM_MAX_PATH + 1];
    auto    BOOL        bScanSubs;      // Dummy var for GatherCommandLineNames

					// ------------------------------------
					// Read file to scan from command line.
					// ------------------------------------
    szzDownloadedFile[0] = szzDownloadedFile[1] = EOS;

    GatherCommandLineNames(szzDownloadedFile,
			   sizeof(szzDownloadedFile),
			   &bScanSubs,
			   NULL);

    if (EOS == szzDownloadedFile[0])
	{
					// No files to scan.
	goto BailOut;
	}

					// ------------------------------
					// Create memory mapped file with
					// request buffer.
					// ------------------------------
    SPRINTF(szRequestBufferName, "%s%ld",
	    QUEUEFILE_MAPPEDFILE_NAME,
	    (DWORD)ghInst);

    hRequestBuffer = CreateFileMapping((HANDLE)0xFFFFFFFF,
				       NULL,
				       PAGE_READWRITE,
				       0, sizeof(QUEUEDFILEREQUEST),
				       szRequestBufferName);
    if (NULL == hRequestBuffer)
	{
	goto BailOut;
	}

    lpRequestBuffer = (LPQUEUEDFILEREQUEST)MapViewOfFile(hRequestBuffer,
							 FILE_MAP_ALL_ACCESS,
							 0, 0,
							 0);
    if (NULL == lpRequestBuffer)
	{
	goto BailOut;
	}
					// Fill in request buffer with
					// default response and the name of
					// the file to scan.
    MEMSET(lpRequestBuffer, 0, sizeof(QUEUEDFILEREQUEST));

    lpRequestBuffer->uResponse = QUEUEFILE_NO_RESPONSE;

    STRNCPY(lpRequestBuffer->szFile,
	    szzDownloadedFile,
	    sizeof(lpRequestBuffer->szFile)-1);

					// ----------------------------------
					// Send request to previous instance.
					// ----------------------------------
    SendMessage(hPrevWnd, NAV_MSG_QUEUEFILE, 0, (LPARAM)ghInst);
    uReturn = lpRequestBuffer->uResponse;

BailOut:
    if (NULL != lpRequestBuffer)
	UnmapViewOfFile(lpRequestBuffer);
    if (NULL != hRequestBuffer)
	CloseHandle(hRequestBuffer);

    return(uReturn);
} // QueueFileToPrevInst()


//************************************************************************
// NavMainWndNAV_MSG_QUEUEFILE()
//
// This routine handles requests to scan web-browser files from subsequent
// instances of NAV.
// The request is sent in a QUEUEDFILEREQUEST structure via a memory mapped
// file. The mapped file name is based on the HINSTANCE of the requester
// which is passed as the LPARAM of a NAV_MSG_QUEUEFILE message.
//
// Parameters:
//      DWORD   dwRequestBufferID       Used to construct mapped file name.
//
// Returns:
//      QUEUEFILE_NO_RESPONSE           If unable to open request buffer.
//      QUEUEFILE_WILL_SCAN             If in download mode and able to scan.
//      QUEUEFILE_WONT_SCAN             Otherwise.
//************************************************************************
// 01/22/95 DALLEE, created.
//************************************************************************

UINT LOCAL NavMainWndNAV_MSG_QUEUEFILE (DWORD dwRequestBufferID)
{
    auto    TCHAR               szRequestBufferName [SYM_MAX_PATH + 1];
    auto    HANDLE              hRequestBuffer = NULL;
    auto    LPQUEUEDFILEREQUEST lpRequestBuffer = NULL;

    auto    UINT                uReturn = QUEUEFILE_NO_RESPONSE;

					// ------------------------------
					// Open memory mapped file with
					// request buffer.
					// ------------------------------
    SPRINTF(szRequestBufferName, "%s%ld",
	    QUEUEFILE_MAPPEDFILE_NAME,
	    dwRequestBufferID);

    hRequestBuffer = CreateFileMapping((HANDLE)0xFFFFFFFF,
				       NULL,
				       PAGE_READWRITE,
				       0, sizeof(QUEUEDFILEREQUEST),
				       szRequestBufferName);

					// Fail unless we've opened a
					// pre-existing mapped file.
    if ( (NULL == hRequestBuffer) || (ERROR_ALREADY_EXISTS != GetLastError()) )
	{
	goto BailOut;
	}

    lpRequestBuffer = (LPQUEUEDFILEREQUEST)MapViewOfFile(hRequestBuffer,
							 FILE_MAP_ALL_ACCESS,
							 0, 0,
							 0);
    if (NULL == lpRequestBuffer)
	{
	goto BailOut;
	}

					// Reset default return to won't scan
					// now that we've got the request.
    lpRequestBuffer->uResponse = QUEUEFILE_WONT_SCAN;
    uReturn = QUEUEFILE_WONT_SCAN;

    if (TRUE == NavMain.bDownloadMode)
	{
	if (NOERR == QueueAddFile(&NavMain.lpDownloadedFiles,
				  lpRequestBuffer->szFile))
	    {
					// Only reply success if we're in
					// web-browser download mode and we
					// successfully queue the file.
	    lpRequestBuffer->uResponse = QUEUEFILE_WILL_SCAN;
	    uReturn = QUEUEFILE_WILL_SCAN;
	    }
	}

BailOut:
    if (NULL != lpRequestBuffer)
	UnmapViewOfFile(lpRequestBuffer);
    if (NULL != hRequestBuffer)
	CloseHandle(hRequestBuffer);

    return(uReturn);
} // NavMainWndNAV_MSG_QUEUEFILE()


//************************************************************************
// QueueAddFile()
//
// This routine adds a file to the queue of files to be scanned
// in web-browser download mode.
// The queue is maintained in the NavMain structure.
//
// Parameters:
//      LPQUEUEDFILE FAR *lpNodePtr     Address of first node pointer.
//      LPTSTR            lpszFilename  Name of file to store.
//
// Returns:
//      NOERR on success.
//      ERR if file was not added (couldn't allocate memory).
//************************************************************************
// 01/22/95 DALLEE, created.
//************************************************************************

STATUS WINAPI QueueAddFile (LPQUEUEDFILE FAR *lpNodePtr, LPCTSTR lpszFilename)
{
    auto    LPQUEUEDFILE        lpNewFile;

					// Allocate a new node.
    lpNewFile = (LPQUEUEDFILE)MemAllocPtr(GHND, sizeof(QUEUEDFILE));

    if (NULL == lpNewFile)
	{
	return (ERR);
	}

					// Copy in new filename.
    STRNCPY(lpNewFile->szFile, lpszFilename, sizeof(lpNewFile->szFile)-1);


					// Get pointer to last node pointer.
    while (NULL != *lpNodePtr)
	{
	lpNodePtr = &((*lpNodePtr)->lpNextFile);
	}

					// Add link to new node.
    *lpNodePtr = lpNewFile;

    return (NOERR);
} // QueueAddFile()


//************************************************************************
// QueueGetFile()
//
// This routine removes the first node from the queue and returns the
// name of the downloaded file to scan.
//
// Parameters:
//      LPQUEUEDFILE FAR *lpNodePtr     Address of first node pointer.
//      LPTSTR  lpszFilename            Return buffer, minimum size is
//                                          SYM_MAX_PATH * sizeof(TCHAR)
//
// Returns:
//      NOERR if a filename was returned.
//      ERR if the queue was empty.
//************************************************************************
// 01/22/95 DALLEE, created.
//************************************************************************

STATUS WINAPI QueueGetFile (LPQUEUEDFILE FAR *lpNodePtr, LPTSTR lpszFilename)
{
    auto    LPQUEUEDFILE    lpNode;

    lpNode = *lpNodePtr;

					// Check for empty list.
    if (NULL == lpNode)
	{
	return (ERR);
	}
					// Copy first file. Assume valid
					// length since this is handled in
					// AddFileToQueue().
    STRCPY(lpszFilename, lpNode->szFile);

					// Remove node.
    *lpNodePtr = lpNode->lpNextFile;
    MemFreePtr(lpNode);

    return (NOERR);
} // QueueGetFile()


//************************************************************************
// QueueRemoveAll()
//
// This routine frees all remaining queued downloaded files.
//
// Parameters:
//      LPQUEUEDFILE FAR *lpNodePtr     Address of first node pointer.
//
// Returns:
//      Nothing.
//************************************************************************
// 01/23/95 DALLEE, created
//************************************************************************

VOID WINAPI QueueRemoveAll (LPQUEUEDFILE FAR *lpNodePtr)
{
    auto    LPQUEUEDFILE    lpNode;
    auto    LPQUEUEDFILE    lpNext;

    for (lpNode = *lpNodePtr; NULL != lpNode; lpNode = lpNext)
	{
	lpNext = lpNode->lpNextFile;
	MemFreePtr(lpNode);
	}

    *lpNodePtr = NULL;
} // QueueRemoveAll()


/* **************************************************************************
@Name:
HWND LOCAL NavMainWndCreate (           // Creates main dialog wnd
    LPCSTR  lpcszCmdLine                // [in] Command Line to process
    )

@Description:
This routine creates the main dialog window for the Norton AntiVirus
Windows Clinic.  The main dialog always comes up hidden.  It is only
after our bitmap has gone away that we show the window.

@Parameters:
$lpcszCmdLine$  Command line to process.

@Returns:
      NULL    - Error creating window; not created.
      hWnd    - Valid window handle

************************************************************************** */

HWND LOCAL NavMainWndCreate (           // Creates main dialog wnd
    VOID
    )
{
    auto    HWND    hWndRet = NULL;     // Return value


    HKEY hKey= 0,hParentKey;
	DWORD dwEnable, dwType, dwSize;
    TCHAR szKey[ SYM_MAX_PATH ];
    BOOL bUpsell = FALSE;
    
    if ( !NAV_IS_OEM() )
    {
        dwEnable = 0;
        dwSize = sizeof(dwEnable);

        GetSystemWorksKey( &hParentKey,szKey );

        if (ERROR_SUCCESS == RegOpenKeyEx(hParentKey, szKey, 0, KEY_READ, &hKey))
        {
            if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("Enable"), 0, &dwType, (LPBYTE)&dwEnable, &dwSize ))
            {
                if(dwEnable == 1)
                    bUpsell = TRUE;
                else
                    bUpsell = FALSE;
                RegCloseKey(hKey);
            }
        }
    }
                    // -----------------------------
					// Create window and display it.
					// -----------------------------

    if( !bUpsell )
        hWndRet = CreateDialog (ghInst, MAKEINTRESOURCE (IDD_SCANMAIN),
			   NULL, NULL);
    else
        hWndRet = CreateDialog (ghInst, MAKEINTRESOURCE (IDD_SCANMAIN_SYSWORKS),
			   NULL, NULL);
 

    if (IsWindow(hWndRet))
	{
	DragAcceptFiles (hWndRet, TRUE);
	}

    return ( hWndRet );
}


/* **************************************************************************
@Name:
LRESULT SYM_EXPORT CALLBACK NavMainWndProc (// Main dialog window proc
    HWND        hWnd,                   // [in] Dialog box handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam                  // [in] lParam to process
    );

@Description:
This is the main dialog box procedure for the Norton AntiVirus for Windows.

@Parameters:
$hDlg$          [in] Dialog Box handle
$wMessage$      [in] message to process
$wParam$        [in] wParam of message
$lParam$        [in] lParam of message

@Returns:       Appropriate value as expected by caller.

@Note:
Because this is a "roll your own" dialog box we are supposed to
call DefDlgProc() so the controls will be handled correctly.
Unfortunately not all of Windows behaviors are desirable when
this routine is called in all cases.  Below are the messages
that are passed to DefWindowProc() to obtain the desired
(and EXPECTED you MS goons) effect:
	WM_SYSCOMMAND, WM_CLOSE

************************************************************************** */

LRESULT SYM_EXPORT CALLBACK NavMainWndProc (// Main dialog window proc
    HWND        hWnd,                   // [in] Dialog box handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam                  // [in] lParam to process
    )
{
    auto    LRESULT     lResult = 0;
    auto    TCHAR       szFileName [ SYM_MAX_PATH+1 ];
#ifdef NAVSCANW
    #define LOGOUT_CAPTION_LEN  50
    auto    TCHAR       szLogoutCaption [ LOGOUT_CAPTION_LEN+1 ];
    auto    UINT        LogoutStringId;
#endif
    auto    HWND        hwndTopWindow = NULL;

    static  BOOL        bCRCCheckPassed = FALSE;

    auto    CNVReBarCtrl    reBar;
    auto    REBARBANDINFO   rBandInfo;
    auto    int             i;

					// ---------------------------------
					// This if() statement is required
					// to support registered custom
					// messages.
					// ---------------------------------
    TRANSMSG ( uMessage )

    switch ( uMessage )
	{
					// -----------------------------------
					// WM_CREATE:  Initialize the
					// main dialog.
					// -----------------------------------
	case WM_CREATE:
	    NavStatSetInstanceHwnd ( NavMain.bDownloadMode ? NAV_APP_DOWNLOAD :
							     NAV_APP_CLINIC, hWnd );
	    if (CheckIntegrity())
		{
		if ( GetCustomVersion() == VERSION_NETSCAPE )
		    {                   // Title is "Internet Scanner"; set at runtime
		    LoadString (ghInst, IDS_NAVW_INTERNET, szFileName, sizeof(szFileName));
		    SetWindowText (hWnd, szFileName);
		    }

        // Set up online help
        HelpInfo.hinst = ghInst;
		HelpInfo.hwnd  = hWnd;
		GetStartDir (ghInst, HelpInfo.helpFileName,
			     sizeof(HelpInfo.helpFileName));
		LoadString (ghInst, IDS_NAVW_HLP, szFileName,
			    sizeof(szFileName));

		NameAppendFile (HelpInfo.helpFileName, szFileName);
		StdHelpExInit (&HelpInfo);
		bCRCCheckPassed = TRUE;

		NavMainWndWM_CREATE (hWnd);
		NG_CenterDialog (hWnd, GetDesktopWindow());

		CoInitialize(NULL);
		}
	    else
		    lResult = -1;

	    break;

	case WM_SYSCOLORCHANGE:
	    NG_ColorChange();

	    memset( &rBandInfo, 0, sizeof( rBandInfo ) );

	    rBandInfo.cbSize = sizeof( rBandInfo );
	    rBandInfo.fMask  = RBBIM_COLORS;
	    rBandInfo.clrFore = GetSysColor( COLOR_BTNTEXT );
	    rBandInfo.clrBack = GetSysColor( COLOR_BTNFACE );

	    reBar.m_hWnd = GetDlgItem( hWnd, ID_SCANMAIN_REBAR );

	    i = reBar.GetBandCount();
	    while ( i-- )
	    {
		reBar.SetBandInfo( i, &rBandInfo );
	    }

	    break;

					// -----------------------------------
					// Initailize the dialog by subclassing
					// all of the controls.
					// -----------------------------------
	case WM_NAVW_INITDIALOG:
	    NavMainWndWM_INITDIALOG (hWnd, &NavMain);
	    break;
					// -----------------------------------
					// WM_ACTIVATE:  Another help
					// kluge for F1.
					// -----------------------------------
	case WM_ACTIVATE:
	    if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
		{
					// ----------------------------------
					// When NAVW window is overlapped by
					// PROGMAN, the control that had the
					// focus, lost the input focus. When
					// NAVW is activated we must give a
					// control the input focus otherwise
					// no control can be selected with KB
					// ----------------------------------
		StdHelpSetContext (IDH_NAVW_MAIN_WINDOW_HELP_BTN,
				   IDH_NAVW_MAIN_WINDOW_HELP_BTN);

		if ( IsWindow ( ghwndLastFocused ) )
		    SetFocus ( ghwndLastFocused );
		else
		    SetFocus ( GetDlgItem ( hWnd, IDOK ) );
		}
	    else
		ghwndLastFocused = GetFocus();

	    break;

	case WM_DROPFILES:
	    DragDrop(hWnd, (HDROP)wParam, &NavMain);
	    break;

	case WM_PAINT:
	    if ( g_wStartupTask )
		{
		PostMessage (hWnd, NAV_MSG_STARTUP, g_wStartupTask, 0L);
		g_wStartupTask = 0;
		}

	    lResult = DefDlgProc (hWnd, uMessage, wParam, lParam);
	    break;

	case NAV_MSG_STARTUP:
	    DoStartupChores (hWnd, wParam, &navOptions);
	    break;

	case WM_ACTIVATEAPP:
					// -----------------------------------
					// If being activated, set it.
					// -----------------------------------
	    if ( (BOOL) wParam )
		{
		StdHelpActivate (hWnd, &HelpInfo);
		}
	    break;
					// -----------------------------------
					// WM_CTLCOLOR:  Used to give control
					// 3D look.
					// -----------------------------------
	case WM_CTLCOLOR:
	    NavMain.hFocus = GET_WM_CTLCOLOR_HWND(wParam, lParam, uMessage);
	    lResult = NavMainWndWM_CTLCOLOR (hWnd, wParam, lParam);
	    break;
					// -----------------------------------
					// WM_DESTROY:  Terminate the
					// application.
					// -----------------------------------
	case WM_DESTROY:
					// -----------------------------------
					// Free Net User memory
					// -----------------------------------
	    if (navOptions.netUser.hNetUserItem)
		{
		MemUnlock(navOptions.netUser.hNetUserItem);
		MemFree(navOptions.netUser.hNetUserItem);
		}

	    if (bCRCCheckPassed)
		{
		ExcludeTerminateEx ( &navOptions.exclude, EXCLEND_DONTSAVEFREEMEM );
		}

					// Kill status update timer.
	    KillTimer( hWnd, STATUS_TIMER_ID );


					// Make sure we're no longer in
					// download mode and clear queue.
	    NavMain.bDownloadMode = FALSE;
	    QueueRemoveAll(&NavMain.lpDownloadedFiles);

	    CoUninitialize();

	    StdHelpShutdown (hWnd);
	    PostQuitMessage(0);
	    break;

	case WM_NOTIFY:
	    // Process tooltip message for toolbar
	    if( !HandleToolbarTip( wParam, lParam ) )
		lResult = DefDlgProc ( hWnd, uMessage, wParam, lParam );
	    break;


#ifdef NAVSCANW
	case WM_QUERYENDSESSION:
	    // If we are on Windows 95, always perform the logout. Only
	    // prompt the user when on Windows NT.
	    if (SystemGetWindowsType () == SYM_SYSTEM_WIN_95)
		lResult = TRUE;         // Proceed with the Logout
	    else
		{
					// Set the default text for logout
					// here:
		LogoutStringId = IDS_NAVWNT_LOGOUTPROMPT;
					// If we have a special case,
					// replace that text with more
					// descriptive text:
		if ( NavMain.hScanMain && NavMain.bScanInProgress )
		    {
		    if ( IsWindow ( NavMain.hScanMain ) )
			{
					// We're scanning:
			LogoutStringId = IDS_NAVWNT_LOGOUTDURINGSCAN;
			}
		    }

		// To load strings, use buffer szLogoutCaption for the
		// caption, and buffer szFileName for the dialog body text.
		if ( GetCustomVersion() == VERSION_NETSCAPE )
		    {                   // Title is "Internet Scanner"; set at runtime
		    LoadString ( ghInst, IDS_NAVW_INTERNET, szLogoutCaption, sizeof (szLogoutCaption));
		    }
		else
		    {
		    LoadString ( ghInst, IDS_NAVW, szLogoutCaption, sizeof (szLogoutCaption));
		    }
		LoadString ( ghInst, LogoutStringId, szFileName, sizeof (szFileName));

					// Find the proper window to put the logout
					// message on top of ...
		hwndTopWindow = NULL;

		if ( NavMain.hScanMain && NavMain.bScanInProgress )
		    {
		    if ( IsWindow ( NavMain.hScanMain ) )
			{
			hwndTopWindow = GetTopWindow(NavMain.hScanMain);
			if (!hwndTopWindow)
			    {
			    hwndTopWindow = NavMain.hScanMain;
			    }
			}
		    }
		else if (hWnd)
		    {
		    hwndTopWindow = GetTopWindow(hWnd);
		    if (!hwndTopWindow)
			{
			hwndTopWindow = hWnd;
			}
		    }

		if ( IDYES == MessageBox(hwndTopWindow,szFileName,szLogoutCaption,MB_YESNO|MB_ICONHAND|MB_SETFOREGROUND|MB_SYSTEMMODAL) )
		    {
		    lResult = TRUE;     // Proceed with the Logout
		    }
		else
		    {
		    lResult = FALSE;    // Abort the Logout
		    }
		}
	    break;

	case WM_ENDSESSION:
	    if ((BOOL)wParam)
		{
		ExitProcess(0);         // Session is ending; Exit w/ Cleanup
		}
	    lResult = FALSE;            // Return 0 if we process this message
	    break;
#endif                                  // NAVSCANW

	case WM_HELP :                  // Fall through ...
	case WM_CONTEXTMENU:
					// ----------------------------------
					// Bring up right click help menu
					// or standard context help:
					// ----------------------------------

	    OnContextHelp ( hWnd, uMessage, wParam, lParam );
	    break;

	case UWM_NAVAP_ACTIVATING:
					// -----------------------------------
					// If the VxD sent us a message telling
					// that it just started up then we
					// must disable it for our instance.
					// -----------------------------------
	    TalkToVxD ( VXDACTION_DISABLE );
	    break;

					// -----------------------------------
					// UWM_DISPLAY_OPTIONS:
					// AP is asking to put up the options
					// panel when we're already active.
					// -----------------------------------
	case UWM_DISPLAY_OPTIONS:

	    NavMainWndWM_COMMAND (hWnd, IDM_TOOLS_OPTIONS, 0,
				  &NavMain);

	    break;

					// -----------------------------------
					// WM_COMMAND:  Process what the user
					// wants us to do.
					// -----------------------------------
	case WM_COMMAND:
	    NavMainWndWM_COMMAND (hWnd, wParam, lParam,
				  &NavMain);
	    break;
					// -----------------------------------
					// WM_ERASEBKGND:  We need to give
					// the drive list box a 3D look.
					// -----------------------------------
	case WM_ERASEBKGND:
	    NavMainWndWM_ERASEBKGND (hWnd, wParam, lParam);
	    break;
					// -----------------------------------
					// WM_MENUSELECT:
					// The user has selected a menu item
					// and we need to provide quick help.
					// -----------------------------------
	case WM_MENUSELECT:
	    NavMainWndWM_MENUSELECT (hWnd, GET_WM_MENUSELECT_CMD(wParam,lParam));
	    break;
					// -----------------------------------
					// WM_SYSCOMMAND:  This message must
					// go to DefWindowProc() since the
					// routine DefDlgProc() can't handle
					// it correctly (MS GOONS!).
					// -----------------------------------
	case WM_SYSCOMMAND:
					// -----------------------------------
					// Fix the relationship between
					// 2 overlapped windows:
					// when main wnd is restored while the
					// scanning is active(in background),
					// scanner should be a top-most wnd and
					// main should be hidden.
					// Note that scanner dlg has been passed
					// as global "NavMain" structure member!
					// -------------------------------------
					// DALLEE - updated 08/30/1996 for
					// NT 3.51, NT 4.0, and Win95.
	    if (SC_RESTORE == (wParam & 0xFFF0) )
		{
					// Currently scanning...
		if ( IsWindow(NavMain.hScanMain) && NavMain.bScanInProgress )
		    {
		    if ( NavMain.bDownloadMode )
			{               // Running in download mode:
					// Don't open up the main window or
					// scan window.
			lResult = 0;
			}
		    else
			{               // Not download mode:
					// Hide main window if no taskbar,
					// and show scan window.
			if (!SystemWin95ShellPresent())
			    {
			    ShowWindow(hWnd, SW_HIDE);
			    }

			ShowWindow(NavMain.hScanMain, SW_SHOW);

			lResult = 0;
			}
		    }
		else                    // Not scanning...
		    {
		    lResult = DefWindowProc (hWnd, uMessage, wParam, lParam);

		    HMENU hmnuSys = GetSystemMenu(hWnd, FALSE);
		    if ( hmnuSys )
			{
			EnableMenuItem( hmnuSys,
					SC_CLOSE,
					MF_BYCOMMAND | MF_ENABLED );
			}
		    }
		}
					// Forward close msg if scanning.
	    else if ( (SC_CLOSE == (wParam & 0xFFF0)) && (IsWindow(NavMain.hScanMain) && NavMain.bScanInProgress) )
		{
		PostMessage(NavMain.hScanMain, uMessage, wParam, lParam);
		}
	    else
		{
		lResult = DefWindowProc (hWnd, uMessage, wParam, lParam);
		}
	    break;

					// -----------------------------------
					// WM_INITMENU: If scanning and this
					// is the system menu, reset RESTORE
					// and CLOSE based on scan dlg.
					// -----------------------------------
	case WM_INITMENUPOPUP:
	    {
	    if ( (BOOL) HIWORD(lParam) &&
		 IsIconic(hWnd) &&
		 (IsWindow(NavMain.hScanMain) && NavMain.bScanInProgress) )
		{
		HMENU hmnuSys = GetSystemMenu(hWnd, FALSE);

		EnableMenuItem( hmnuSys,
				SC_CLOSE,
				NavMain.Scan.bAllowScanStop &&
				IsWindowEnabled(NavMain.hScanMain) ?
				    (MF_BYCOMMAND | MF_ENABLED) :
				    (MF_BYCOMMAND | MF_GRAYED) );
		EnableMenuItem( hmnuSys,
				SC_RESTORE,
				IsWindowVisible(NavMain.hScanMain) ?
				    (MF_BYCOMMAND | MF_GRAYED) :
				    (MF_BYCOMMAND | MF_ENABLED) );
		}
	    else
		{
		lResult = DefDlgProc(hWnd, uMessage, wParam, lParam);
		}
	    break;
	    }

					// -----------------------------------
					// WM_CLOSE:  The user wishes to
					// terminate the application.  If
					// NAVNETW, see if the user wishes
					// to save the infomation first.
					// -----------------------------------
	case WM_CLOSE:
	    if (g_bSageRun)
		{
		if ( NavMain.bScanInProgress && NavMain.hScanMain && IsWindow (NavMain.hScanMain) )
		    {
		    FORWARD_WM_COMMAND ( NavMain.hScanMain,
					 IDCANCEL,
					 0,
					 0,
					 PostMessage );
		    break;
		    }
		}

					// -----------------------------------
					// Not allowed closing while iconic!!!
					// -----------------------------------
	    if ( IsIconic(hWnd) && (IsWindow(NavMain.hScanMain) && NavMain.bScanInProgress) )
		return(TRUE);

	    FORWARD_WM_COMMAND ( hWnd, ID_SCANMAIN_EXIT,
				 GetDlgItem(hWnd, ID_SCANMAIN_EXIT),
				 BN_CLICKED, PostMessage );
	    break;

					// -----------------------------------
					// NAV_MSG_QUEUEFILE: Another instance
					// is requesting that we scan a
					// web-browser downloaded file.
					// -----------------------------------
	case NAV_MSG_QUEUEFILE:
	    lResult = NavMainWndNAV_MSG_QUEUEFILE((DWORD)lParam);
	    break;

					// -----------------------------------
					// Paint the gradiant main window caption:
					// -----------------------------------
//        case WM_WINDOWPOSCHANGED:
	case WM_NCACTIVATE:
	case WM_SETTEXT:
	    DefWindowProc(hWnd, uMessage, wParam, lParam);

	    if( s_bUseGradientPaintCode  &&  !IsIconic(hWnd))
		{
		RECT    rDraw;
		BOOL    bActive = (uMessage == WM_NCACTIVATE) ? wParam : (GetActiveWindow() == hWnd) ;
		DrawCaption(hWnd, &rDraw, bActive);
		}
	    return 1;

	case WM_NCPAINT:
	    if( s_bUseGradientPaintCode  &&  !IsIconic(hWnd))
		{
		return OnNCPaint(hWnd, uMessage, wParam, lParam);
		}
	    else
		{
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
		}

					// -----------------------------------
					// default:  Let Windows handle the
					// message since we do not want to
					// deal with it.
					// -----------------------------------
	default:
	    lResult = DefDlgProc ( hWnd, uMessage, wParam, lParam );
	}

    return ( lResult );
} // NavMainWndProc()


/* **************************************************************************
@Name:
VOID LOCAL DoStartupChores (            // Do startup tasks.
    HWND            hWnd,               // [in] main window handle
    WPARAM          wParam,             // [in] Task ID to perform
    LPNAVOPTIONS    lpNavOptions        // [in] pointer to nav options
    );

@Description:
This functions perform the specified startup tasks such as notifications to
the user about different error conditions that occured during startup.

@Parameters:
$hWnd$          [in] Main window handle
$wParam$        [in] Startup task ID number.
$lpNavOptions$  [in] pointer to nav options

@Returns:       Nothing

************************************************************************** */
// 08/22/1996 DALLEE, W95 bad platform check to beginning of WinMain
//      at same spot NT check was implemented (under W95 this check only
//      happened if splash screen was shown.. gah!)
//***************************************************************************

VOID LOCAL DoStartupChores (            // Do startup tasks.
    HWND            hWnd,               // [in] main window handle
    WPARAM          wParam,             // [in] Task ID to perform
    LPNAVOPTIONS    lpNavOptions        // [in] pointer to nav options
    )
{
    auto    NOTIFYSTRUCT    Notify;

    switch ( wParam )
	{
	case NAVW_STARTUP_STARTSCAN:
	    UpdateWindow(hWnd);

	    FORWARD_WM_COMMAND ( hWnd, IDOK, GetDlgItem(hWnd, IDOK),
				 BN_CLICKED, PostMessage );
	    break;

	case NAVW_STARTUP_NAVBIN:
	    UpdateWindow(hWnd);

	    MEMSET (&Notify, EOS, sizeof(Notify));

	    Notify.hParent       = hWnd;
	    Notify.uMsgID        = IDS_NONAVBIN;
	    if ( GetCustomVersion() == VERSION_NETSCAPE )
		{                       // Title is "Internet Scanner"; set at runtime
		Notify.uTitleID      = IDS_NAVW_INTERNET;
		}
	    else
		{
		Notify.uTitleID      = IDS_NAVW;
		}
	    Notify.lpAlertMsg    = NULL;
	    Notify.hInstance     = ghInst;
	    NotifyUser (&Notify);

	    // Need to terminate app here.
	    DestroyWindow( hWnd );
	    break;

	case NAVW_STARTUP_NODEFS:
	    MEMSET (&Notify, '\0', sizeof(Notify));
	    Notify.hParent       = hWnd;
	    Notify.uMsgID        = IDS_NODEFS;
	    if ( GetCustomVersion() == VERSION_NETSCAPE )
		{                       // Title is "Internet Scanner"; set at runtime
		Notify.uTitleID      = IDS_NAVW_INTERNET;
		}
	    else
		{
		Notify.uTitleID      = IDS_NAVW;
		}
	    Notify.lpFileName    = NULL;
	    Notify.lpVirusName   = NULL;
	    Notify.lpAlertMsg    = NULL;
	    Notify.uRemoveAfter  = NULL;
	    Notify.hInstance     = ghInst;

	    NotifyUser (&Notify);

					// Disable appropriate buttons and
					// menu items.

	    Button_Enable ( GetDlgItem(hWnd, IDOK), FALSE );
        Button_Enable ( GetDlgItem(hWnd, IDC_STATUS_AP_BUTTON), FALSE );

	    {
	    HMENU hMenu = GetMenu ( hWnd );

	    EnableMenuItem ( hMenu,
			     IDM_TOOLS_VIRUSLIST,
			     MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	    EnableMenuItem ( hMenu,
			     IDM_SCAN_SELECTED,
			     MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	    EnableMenuItem ( hMenu,
			     IDM_SCAN_DIRECTORY,
			     MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	    EnableMenuItem ( hMenu,
			     IDM_SCAN_FILE,
			     MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	    EnableMenuItem ( hMenu,
                 IDM_SCAN_PATH,
			     MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

	    }
	    break;
	}
}


/* **************************************************************************
@Name:
VOID LOCAL NavMainWndWM_CREATE(         // Handles WM_CREATE for main window
    HWND    hWnd                        // [in] Window handle
    );

@Description:
Creates the opening bitmap and posts a WM_INITDIALOG message to our-
selves.  We must do this since (as a window faking that it is a dialog)
we really cannot begin until all of our child controls are in place.
We also initialize our NavMainWnd and NavScanSpec structures here.

@Parameters:
$hWnd$          [in] Main window handle

@Returns:       Nothing

************************************************************************** */

VOID LOCAL NavMainWndWM_CREATE(         // Handles WM_CREATE for main window
    HWND    hWnd                        // [in] Window handle
    )
{
    if (bCommandLine == FALSE)
	{
	ShowWindow (hWnd, SW_HIDE);
	NavOpeningBmpCreate (NULL);
	}
    else
	{
					// Special case for /VISIBLE switch.
					// We must ALWAYS display ourselves
					// when the /VISIBLE switch is specified
	CFGTEXTREC config = {_T("VISIBLE"), CFG_SW_EXACT, 0, 0, 0 };
	if(ConfigSwitchSet(&config, NULL))
	    ShowWindow (hWnd, SW_NORMAL);
	else
	    {
	    ShowWindow (hWnd, SW_SHOWMINIMIZED);
	    ShowWindow (hWnd, SW_HIDE);
	    }

	UpdateWindow (hWnd);
	}

					// -----------------------------------
					// Delay processing until everything
					// is ready.
					// -----------------------------------
    PostMessage (hWnd, WM_NAVW_INITDIALOG, 0, 0L);
}


/* **************************************************************************
@Name:
VOID LOCAL NavMainWndWM_INITDIALOG(     // Handles WM_CREATE for main window
    HWND        hWnd,                   // [in] Window handle
    LPNAVMAIN   lpNav                   // [in] Data for main window
    )

@Description:
Initializes the drive list and other controls based on the values of
the passed structures after all of the proper controls are subclassed
for the 3D effect.
We also initialize the button bar here.  Note that the button bar
supports the following menu options:  Options, Virus List, Scheduler,
Activity Log.
The font for the ID_SCANMAIN_STATUS field is set here.

@Parameters:
$hWnd$          [in] Main window handle
$lpNavMain$     [in] Data structure for main window

@Returns:       Nothing

************************************************************************** */

VOID LOCAL NavMainWndWM_INITDIALOG(     // Handles WM_CREATE for main window
    HWND        hWnd,                   // [in] Window handle
    LPNAVMAIN   lpNav                   // [in] Data for main window
    )
{

    Ctl3dSubclassDlgEx ( hWnd, CTL3D_ALL );

    wHelpBrowseMsg = RegisterWindowMessage (HELPMSGSTRING);

    NavMainResizeControls(hWnd);

					// ---------------------------------
					// Read current settings
					// and update system file lists,
					// if necessary.
					// ---------------------------------
    if (NeedToUpdateStartupFileLists())
	{
	UpdateStartupFileLists();
	}

    InocInit(&navOptions);

    if ( LoadVirusDefs() == FALSE )
	{
					// ---------------------------------
					// Initialization failed. Indicate
					// scan in progress to inhibit any
					// scan attempt.
					// ---------------------------------

	lpNav->bScanInProgress = TRUE;
	}

    ExcludeInit(&navOptions.exclude);   // Load EXCLUDE.BIN

    LoadNetUserList(&navOptions.netUser);

					// -----------------------------------
					// Initialize our window's data
					// structure.
					// -----------------------------------
    NavMainOptionsLoad (hWnd, lpNav, NULL);

					// Remove inappropriate menu items.
    HMENU hmMainMenu = GetMenu( hWnd );
    if ( hmMainMenu )
	{
	HMENU hmToolsPopupMenu = GetSubMenu ( hmMainMenu , 1 );
	if ( hmToolsPopupMenu )
	    {
	    if ( !SchedulerIsInstalled() )
		{
		DeleteMenu( hmToolsPopupMenu , IDM_TOOLS_SCHEDULER , MF_BYCOMMAND );
		}

	    if ( !IsLiveUpdateValid() )
		{
		DeleteMenu( hmToolsPopupMenu, IDM_TOOLS_ONRAMP, MF_BYCOMMAND );
		}

	    if ( !IsQuarantineValid() )
		{
		DeleteMenu( hmToolsPopupMenu, IDM_TOOLS_QUARANTINE, MF_BYCOMMAND );
		}
	    }
	}

    // If running on NT3.51, remove references to URLs from help menu
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx( &os );

    if( os.dwPlatformId == VER_PLATFORM_WIN32_NT &&
	os.dwMajorVersion < 4 )
	{
	if ( hmMainMenu )
	    {
	    HMENU hmToolsPopupMenu = GetSubMenu ( hmMainMenu , 2 );
	    if ( hmToolsPopupMenu )
		{
		hmToolsPopupMenu = GetSubMenu ( hmToolsPopupMenu , 1 );
		if ( hmToolsPopupMenu )
		    {
		    DeleteMenu (hmToolsPopupMenu, IDM_HELP_PSO_AVSUPPORT, MF_BYCOMMAND);
		    DeleteMenu (hmToolsPopupMenu, IDM_HELP_PSO_SARC, MF_BYCOMMAND);
		    }
		}
	    }
	}
					// ---------------------------------
					// Remove the product Support online
					// menu from the scanner only product.
					// Use the same positions as defined
					// above.  UGLY!  GetSubMenu will
					// return NULL if the item is not a menu
					// ---------------------------------
    if ( hmMainMenu && NAV_IS_SCANONLY())
	{
	HMENU hmToolsPopupMenuParent = GetSubMenu ( hmMainMenu , 2 );
	if ( hmToolsPopupMenuParent )
	    {
	    HMENU hmToolsPSO = GetSubMenu ( hmToolsPopupMenuParent , 1 );
	    if ( hmToolsPSO )
		DeleteMenu(hmToolsPopupMenuParent,1, MF_BYPOSITION);
	    }
	}


    NavMainInitToolBar(hWnd);

    // Start the quarantine monitor.  However do not start it if we were launched
    // with the /HMEM switch (by the shell extension )
    GetInitialDefinitionsDate();

    // Get initial states for real time status panels.
    UpdateApStatus( &lpNav->rApStatus );
    UpdateQuarantineZeroNewItems( &lpNav->rQuarantineStatus );
    UpdateQuarantineStatus( &lpNav->rQuarantineStatus );
    UpdateDefinitionsStatus( &lpNav->rDefinitionsStatus );

    DisplayApStatus( hWnd, lpNav );
    DisplaySecondaryStatus( hWnd, lpNav );

    if ( !g_bRunFromShell )
        {
        StartStatusMonitor(lpNav);
        }

    // Start timer for gettting status updates.

    SetTimer( hWnd, STATUS_TIMER_ID, STATUS_TIMER_TIMEOUT, NavMainStatusTimer );


#if !defined(NAVSCANW)
					// -----------------------------------
					// Convert the activity log
					// spool file to make sure that
					// VxD items are properly entered
					// in to the log.
					// -----------------------------------
    {
    TCHAR        szSpoolFile[SYM_MAX_PATH];

    NameReturnPath ( navOptions.activity.szLogFile, szSpoolFile );
    NameAppendFile ( szSpoolFile, SPOOL_FILE_NAME );
    NavApLogSpoolFileProcess (szSpoolFile);
    }
#endif
					// -----------------------------------
					// If the user passed some information
					// to us on the command line, post a
					// start scan message. The logic that
					// processed the start scan message
					// will figure out what's going on.
					// -----------------------------------

    if( s_bCommandLineHelp )
        {
        StdHelpFromDialog ( hWnd, IDH_NAVW_COMMAND_LINE_SWITCHES );
        }

    if ( bCommandLine == TRUE )
        {
        PostMessage (hWnd, NAV_MSG_STARTUP, NAVW_STARTUP_STARTSCAN, 0L);
        }
    else
	;                               // SetFocus(GetDlgItem(hWnd, IDOK));

} // NavMainWndWM_INITDIALOG()


/* **************************************************************************
@Name:
BOOL WINAPI LoadVirusDefs (              // Initialize virus scanning engine
    VOID                                // Nothing
    );

@Description:
This function initializes our virus scanning engine.

@Parameters:    Nothing

@Returns:       TRUE - if no error occured while starting up the scan engine
		FALSE- if the virus definition files were not found.

************************************************************************** */

BOOL WINAPI LoadVirusDefs (             // Initialize virus scanning engine
    VOID                                // Nothing
    )
{
    auto    BOOL         bStatus;
    auto    LOGSTRUCT    Log;

    if ((bStatus = NavVDefsVirusScanInit()) == ERR)
	{
	bStatus = FALSE;
	g_wStartupTask = NAVW_STARTUP_NODEFS;
	}
    else if (bStatus == NOERR_DAT_CHANGED)
	{
	bStatus = TRUE;
	Log.uEvent       = LOGEV_VIRLISTCHANGE;
	Log.lpFileName   = NULL;
	Log.lpVirusName  = NULL;
	Log.lpAct        = &navOptions.activity;
	LogEvent (&Log);
	}
    else
	bStatus = TRUE;

					// Need to update registry about
					// the virus defs.  This is a kluge
					// since new calls rely on AVAPI
					// and not the old CERTLIB apis.
					// We rely on a VirList hack to
					// help us in getting a hVirContext
					// without file i/o callbacks.

    if (bStatus)
    {
	HVCONTEXT        hVirContext = NULL;
	TCHAR            szVirscanDat [SYM_MAX_PATH];
	TCHAR            szVirscanInf [SYM_MAX_PATH];

					// Get the definitions directory
	VirusScanGetPath (szVirscanDat, sizeof(szVirscanDat));

	STRCPY (szVirscanInf, szVirscanDat);

	NameAppendFile (szVirscanDat, "VIRSCAN1.DAT");
	NameAppendFile (szVirscanInf, "VIRSCAN.INF");

	if ( hVirContext =
	     InitVirusLibrary ( szVirscanDat, szVirscanInf ) )
	{
	    RegistryAddVirusDefDate (hVirContext);
	}

	UnloadVirusLibrary (hVirContext);
    }

    return (bStatus);
} // LoadVirusDefs();

/* **************************************************************************

@Name:
LRESULT LOCAL NavMainWndWM_CTLCOLOR(    // Handles WM_CTLCOLOR for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_CTLCOLOR message
    LPARAM      lParam                  // lParam of WM_CTLCOLOR message
    );

@Description:
This routine handles the WM_CTLCOLOR message for the main dialog window
of the Norton AntiVirus for Windows.  The routine must return either a
brush or the value from DefWindowProc() in order for the CTL3D.DLL 3D
control Dynamic Link Library to give our controls a nice 3D look.  This
code is taken directly from the CTL3D.DOC documentation.

@Parameters:
$hWnd$          [in] Window handle of main dialog window
$wParam$        [in] wParam of WM_CTLCOLOR message
$lParam$        [in] lParam of WM_CTLCOLOR message

@Returns:
      HBRUSH          if Ctl3dCtlColorEx() returns a valid brush
      DefWindowProc() if Ctl3dCtlColorEx() could not return a valid brush

************************************************************************** */

LRESULT LOCAL NavMainWndWM_CTLCOLOR(    // Handles WM_CTLCOLOR for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_CTLCOLOR message
    LPARAM      lParam                  // lParam of WM_CTLCOLOR message
    )
{
    LRESULT     lResult;
    HBRUSH      hBrush = NULL;

					// -----------------------------------
					// See if CTL3D.DLL has a brush
					// for our control to use.
					// -----------------------------------
    hBrush = Ctl3dCtlColorEx(WM_CTLCOLOR, wParam, lParam);

    if ( hBrush != NULL )
	{
	lResult = MAKELRESULT (hBrush, 0);
	}
    else
	{
	lResult = DefWindowProc(hWnd, WM_CTLCOLOR, wParam, lParam);
	}

    return (lResult);
} // NavMainWndWM_CTLCOLOR()


/* **************************************************************************
@Name:
VOID LOCAL NavMainWndWM_ERASEBKGND(     // Handles WM_ERASEBKGND for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_ERASEBKGND message
    LPARAM      lParam                  // lParam of WM_ERASEBKGND message
    );

@Description:
This routine handles the WM_ERASEBKGND message for the main dialog window
of the Norton AntiVirus for Windows.  First we paint the canvas of the
main dialog window then we paint the 3D sunken bevel for the drives
list box.

@Parameters:
      hWnd            [in] Window handle of main dialog window
      wParam          [in] wParam of WM_ERASEBKGND message
      lParam          [in] lParam of WM_ERASEBKGND message

@Returns:
      Nothing

************************************************************************** */

VOID LOCAL NavMainWndWM_ERASEBKGND(     // Handles WM_ERASEBKGND for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_ERASEBKGND message
    LPARAM      lParam                  // lParam of WM_ERASEBKGND message
    )
{
    HDC         hDC = (HDC)wParam;
    RECT        rect;
    POINT       pt;

#define BORDER_3D       1

    hDC = (HDC)wParam;
					// -----------------------------------
					// Paint the background for the
					// main window.
					// -----------------------------------
    GetClientRect (hWnd, &rect);
    NG_DrawCanvas (hWnd, hDC, &rect, 0);

					// -----------------------------------
					// Now paint the 3D bevel for the
					// sunken list box.
					// -----------------------------------
    GetWindowRect(GetDlgItem (hWnd, ID_SCANMAIN_DRIVE_LIST), &rect);

    pt.x = rect.left;
    pt.y = rect.top;

    ScreenToClient(hWnd, &pt);

    rect.left = pt.x - BORDER_3D;
    rect.top  = pt.y - BORDER_3D;

    pt.x = rect.right;
    pt.y = rect.bottom;

    ScreenToClient(hWnd, &pt);

    rect.right  = pt.x + BORDER_3D;
    rect.bottom = pt.y + BORDER_3D;

    NG_Draw3DFrame(hWnd, hDC, &rect, NG_LOWERED, BORDER_3D);

} // NavMainWndWM_ERASEBKGND()


/* **************************************************************************
@Name:
VOID LOCAL NavMainWndWM_MENUSELECT (    // Handles WM_MENUSELECT for main wnd
    HWND    hWnd,                       // [in] Main dialog window handle
    UINT    uIDItem                     // [in] Menu ID of selected item
    );

@Description:
This routine handles the WM_MENUSELECT message for the main dialog window
of the Norton AntiVirus for Windows.  It places in to the status field
the text that best describes the currently selected menu item.

@Parameters:
$hWnd$            [in] Window handle of main dialog window
$wIDItem$         [in] Id of menu item selected

@Returns:
      Nothing

************************************************************************** */

VOID LOCAL NavMainWndWM_MENUSELECT (    // Handles WM_MENUSELECT for main wnd
    HWND    hWnd,                       // [in] Main dialog window handle
    UINT    uIDItem                     // [in] Menu ID of selected item
    )
{
    static MENUTOHELP   MenuToHelp[] = // Contains menu to quick help
	{
	    { IDM_SCAN_SELECTED,    0,  IDS_STATUS_SCAN_SELECTED },
	    { IDM_SCAN_DIRECTORY,   0,  IDS_STATUS_SCAN_DIRECTORY },
	    { IDM_SCAN_FILE,        0,  IDS_STATUS_SCAN_FILE },
	    { IDM_SCAN_EXIT,        0,  IDS_STATUS_SCAN_EXIT },
	    { IDM_TOOLS_SCHEDULER,  0,  IDS_STATUS_TOOLS_SCHEDULER },
	    { IDM_TOOLS_VIRUSLIST,  0,  IDS_STATUS_TOOLS_VIRUSLIST },
	    { IDM_TOOLS_ACTIVITYLOG,0,  IDS_STATUS_TOOLS_ACTIVITYLOG },
	    { IDM_TOOLS_QUARANTINE, 0,  IDS_STATUS_TOOLS_QUARANTINE },
	    { IDM_TOOLS_ONRAMP,     0,  IDS_STATUS_TOOLS_ONRAMP },
	    { IDM_HELP_ABOUT,       0,  IDS_STATUS_HELP_ABOUT },
	    { IDM_TOOLS_OPTIONS,    0,  IDS_STATUS_TOOLS_OPTIONS },
	    { IDM_HELP_INFODESK,    0,  IDS_STATUS_HELP_INFODESK },
	    { 0, 0, 0 }
	};

    auto    UINT            uCurId;

					// ------------------------------------
					// uIDItem == 0 if the user pressed the
					// ESC key. Put up default message.
					// ------------------------------------
    if ( !uIDItem )
	{
	NavMainWndStatusUpdate (hWnd, IDS_STATUS_DEFAULT);

	StdHelpSetContext (IDH_NAVW_MAIN_WINDOW_HELP_BTN,
			   IDH_NAVW_MAIN_WINDOW_HELP_BTN);
	}
    else
	{
	for ( LPMENUTOHELP lpMenuToHelp = (LPMENUTOHELP)MenuToHelp;
	      ((uCurId = lpMenuToHelp->wMenuId) != 0);
	      lpMenuToHelp++ )
	    {
	    if ( uCurId == uIDItem )
		{
					// ------------------------------------
					// We found a match.  Load the
					// quick help text and stuff it
					// into the status line.
					// ------------------------------------
		NavMainWndStatusUpdate (
				hWnd,
				lpMenuToHelp->wStatusId
				);
		break;
		}
	    }
	}
}


/* **************************************************************************
@Name:
VOID WINAPI NavMainWndStatusUpdate(     // Updates status line of main wnd
    HWND    hWnd,                       // [in] Main dialog window handle
    WORD    wIDStatus                   // [in] ID of string to display
    );

@Description:
This routine updates the status line of the main window dialog for the
Norton AntiVirus for Windows.  It loads the specified string from the
string table and then places it in the ID_SCANMAIN_STATUS field of the
main window.

@Parameters:
$hWnd$            [in] Window handle of main dialog window
$wIDItem$         [in] Id of status string to display

@Returns:
      Nothing

************************************************************************** */

VOID WINAPI NavMainWndStatusUpdate(     // Updates status line of main wnd
    HWND    hWnd,                       // [in] Main dialog window handle
    WORD    wIDStatus                   // [in] ID of string to display
    )
{
    TCHAR        szStatusText[80];

    LoadString (ghInst,
		wIDStatus,
		szStatusText,
		sizeof(szStatusText));

    StatusBar_SetText (GetDlgItem (hWnd, ID_SCANMAIN_STATUS), szStatusText);
}

/* **************************************************************************

@Name:
VOID WINAPI ActivateNetDrives (         // This function initializes drive LB
    HWND        hWnd,                   // [in] main window handle
    LPNAVMAIN   lpNav,                  // [in] good ol nav pointer
    LPTSTR       lpDrives                // [in] drives if any to add
    );

@Description:
This function initializes the contents if the NAV drive listbox depending
on the currect options.

@Parameters:
$hWnd$            [in] Window handle of main window
$lpNav$           [in] our nav pointer
$lpDrives$        [in] any additional drives to add to drive listbox.

@Returns:
      Nothing

************************************************************************** */

VOID WINAPI ActivateNetDrives (         // This function initializes drive LB
    HWND        hWnd,                   // [in] main window handle
    LPNAVMAIN   lpNav,                  // [in] good ol nav pointer
    LPTSTR       lpDrives                // [in] drives if any to add
    )
{
    auto    HWND    hWndNetDrives;
    auto    HWND    hDriveList;
    auto    WORD    wDriveMask;

    hWndNetDrives = GetDlgItem (hWnd, ID_SCANMAIN_DRIVES_NETWORK);
    hDriveList    = GetDlgItem (hWnd, ID_SCANMAIN_DRIVE_LIST);
    wDriveMask    = NDRV_ALL_FLOPPIES | NAV_NDRV_FIXED;

					// ------------------------------------
					// If network scanning is allowed, and
					// we have a network then enable all
					// the network drives.
					// ------------------------------------
    if ( lpNav->Scan.bAllowNetScan && NetworkDrivesExist() )
	{
					// ------------------------------------
					// Network drives are now allowed
					// ------------------------------------
	Button_Enable (hWndNetDrives, TRUE);
	ShowWindow(hWndNetDrives, SW_SHOW);
	wDriveMask |= NDRV_NETWORK;
	}
    else
	{
					// ------------------------------------
					// Network drives are no longer allowed.
					// ------------------------------------
	Button_Enable (hWndNetDrives, FALSE);
	}

    NDL_InitDrives (hDriveList, wDriveMask, NULL, "");

    if (lpDrives)
	{
	NDL_SelectByName (hDriveList, lpDrives);
	}
    else
	{
					// ------------------------------------
					// Preselect any of the check boxes.
					// ------------------------------------
	if (lpNav->Scan.bPresFlop)
	    NDL_SelectByMask (hDriveList, NDRV_ALL_FLOPPIES, TRUE);

	if (lpNav->Scan.bPresHard)
	    NDL_SelectByMask (hDriveList, NAV_NDRV_FIXED, TRUE);

	if (lpNav->Scan.bPresNet)
	    NDL_SelectByMask (hDriveList, NDRV_NETWORK, TRUE);
	}
}


//*************************************************************************
// IsQuarantineValid()
//
// BOOL LOCAL IsQuarantineValid( )
//
// This routine determines whether quarantine is installed on the machine.
//
// We check with both the personality DLL and a runtime check to see if
// QCONSOLE.EXE really exists.
//
// Returns:
//      TRUE if quarantine is installed on the machine. Otherwise, FALSE.
//
//*************************************************************************
// 04/14/1998 DALLEE, created - header added.
//*************************************************************************

BOOL LOCAL IsQuarantineValid()
{
    auto    BOOL            bValid;
    auto    IQuarantineDLL  *pIQuarantine;
    auto    TCHAR           szConsoleFileName [ SYM_MAX_PATH + 1 ];
    auto    TCHAR           szFullPathName [ SYM_MAX_PATH + 1 ];
    auto    HANDLE          hFind;
    auto    WIN32_FIND_DATA stFindData = {0};

    bValid = FALSE;

    // If Quarantine is not a feature of this installation, return FALSE
    if( !NAV_IS_QUARANTINE() )
        {
        return FALSE;
        }

    // Get console name.
    GetStartDir( ghInst, szFullPathName, sizeof( szFullPathName ) );
    LoadString( ghInst, IDS_QUARANTINE_CONSOLE, szConsoleFileName, sizeof( szConsoleFileName ) );
    NameAppendFile( szFullPathName, szConsoleFileName );

    // If QCONSOLE.EXE is not in NAV's directory, return FALSE.
    hFind = FindFirstFile(szFullPathName, &stFindData);

    if ( hFind == INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }
    else
        FindClose ( hFind );


    if ( SUCCEEDED( CoInitialize( NULL ) ) )
    {
	if ( SUCCEEDED( CoCreateInstance( CLSID_QuarantineDLL,
					  NULL,
					  CLSCTX_INPROC_SERVER,
					  IID_QuarantineDLL,
					  ( LPVOID * ) &pIQuarantine ) ) )
	{
	    pIQuarantine->Release();

	    bValid = TRUE;
	}

	CoUninitialize();
    }

    return ( bValid );
} // IsQuarantineValid()


/* **************************************************************************

@Name:
BOOL LOCAL IsLiveUpdateValid (          // Figure out if LU is present for NAV
    VOID                                // Nothing
    );

@Description:
This function figures out if LU for NAV is installed.  It checks to see if
the shared DLLs are there and checks for the presence of NavLu32.exe in the
installation directory.

@Parameters: Nothing

@Returns:
      TRUE  The LiveUpdateDlls are there and NavLu32.exe was found.
      FALSE One of the above conditions was not satisfied.

************************************************************************** */

BOOL LOCAL IsLiveUpdateValid (          // Figure out if LU is present for NAV
    VOID                                // Nothing
    )
{
    auto    TCHAR   szPath[ SYM_MAX_PATH ] = { 0 };
    auto    TCHAR   szLUFilename [SYM_MAX_PATH] = { 0 };
    LPTSTR  lpszLast = NULL;
    LPTSTR  lpszName = NULL;
    int     nStringLength;

    // Check to see if the GetModuleFileName failed.
    nStringLength = GetModuleFileName ( NULL, szPath, sizeof(szPath) / sizeof(TCHAR) );
    if( 0 == nStringLength )
	return FALSE;

    // Check to see if there was a '\\'
    lpszLast = _tcsrchr ( szPath, '\\' );
    if( NULL == lpszLast )
	return FALSE;

    // Move the the first character after the last '\\'
    lpszName = _tcsinc(lpszLast);

    // Load the name of the LU file.
    // If the string load fails, NavLuIsLuInstalled will return FALSE.
    LoadString(ghInst, IDS_LIVEUPDATE, szLUFilename, sizeof(szLUFilename) / sizeof(TCHAR));

    // Append the filename to the path
    _tcscpy ( lpszName, szLUFilename);

    return NavLuIsLuInstalled(szPath);
}

/* **************************************************************************

@Name:
BOOL LOCAL NetworkDrivesExist (         // Figure out if any network drives
    VOID                                // Nothing
    );

@Description:
This function figures out if there are any network drives.

@Parameters: Nothing

@Returns:
      TRUE  There are network drives
      FALSE There aren't any

************************************************************************** */

BOOL LOCAL NetworkDrivesExist (         // Figure out if any network drives
    VOID                                // Nothing
    )
{
    auto    BOOL    bExist = FALSE;
    auto    BYTE    byDrive;

    for ( byDrive = 'A'; byDrive <= 'Z'; byDrive ++ )
	if ( DiskIsNetwork ( byDrive ) )
	    {
	    bExist = TRUE;
	    break;
	    }

    return (bExist);
}

/* **************************************************************************

@Name:
VOID WINAPI NavMainOptionsLoad (        // Initializes Main Window Params
    HWND        hWnd,                   // [in] Handle to main window
    LPNAVMAIN   lpNav,                  // [in/out] Buffer to options record
    LPTSTR       lpDrives                // [in] drives to preselect
    );

@Description:
God knows why this function ever ended up being as it is now.

@Parameters:
$hWnd$          [in] Handle to main window
$lpNav$         [in/out] Buffer to options record
$lpDrives$      [in] drives to preselect

@Returns:       Nothing

************************************************************************** */

VOID WINAPI NavMainOptionsLoad (        // Initializes Main Window Params
    HWND        hWnd,                   // [in] Handle to main window
    LPNAVMAIN   lpNav,                  // [in/out] Buffer to options record
    LPTSTR       lpDrives                // [in] drives to preselect
    )
{
    lpNav->Scan = navOptions.scanner;
    ActivateNetDrives (hWnd, lpNav, lpDrives);

    // Set enable/disable state for AP enable button.

    EnableWindow( GetDlgItem( hWnd, IDC_STATUS_AP_BUTTON ), navOptions.tsr.bUnloadable );

}


/* **************************************************************************

@Name:
BOOL LOCAL CheckIntegrity (             // Check how we are doing.
    VOID                                // Nothing
    );

@Description:
Checks if the Norton AntiVirus program is infected with a virus.

@Parameters:    Nothing

@Returns:
    TRUE    NAV is doing just fine.
    FALSE   Something is screwy around here.

************************************************************************** */

BOOL LOCAL CheckIntegrity (             // Check how we are doing.
    VOID                                // Nothing
    )
{
    auto        BOOL            bStatus = FALSE;
    auto        NOTIFYSTRUCT    Notify;


    if (NavSelfTest (ghInst) != SELF_TEST_SUCCESS)
	{
					// Look for special infections that
					// need to save in-memory info.

	MEMSET (&Notify, EOS, sizeof(Notify));

	Notify.hParent       = NULL;

	if ( GetCustomVersion() == VERSION_NETSCAPE )
	    {                           // Title is "Internet Scanner"; set at runtime
	    Notify.uTitleID  = IDS_NAVW_INTERNET;
	    }
	else
	    {
	    Notify.uTitleID  = IDS_NAVW;
	    }

	Notify.lpAlertMsg    = (navOptions.alert.bDispAlertMsg) ?
				navOptions.alert.szAlertMsg : NULL;

	Notify.hInstance     = ghInst;
	Notify.uMsgID        = IDS_NAVWMODIFIED;
	NotifyUser (&Notify);
	}
    else
	bStatus = TRUE;

    return ( bStatus );
}


/* **************************************************************************
@Name: VOID LOCAL OnContextHelp (
	    HWND    hDlg,
	    WPARAM  wParam,
	    LPARAM  lParam
	    )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU and WM_HELP
windows message. The purpose of this function is to call the standard symantec help
routines that display an appropiate: floating help menu or context sensitive help.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
)
{

					// -----------------------------------
					// This array is a cross reference
					// between a control id number and a
					// help id number that should be
					// displayed when the user selects the
					// 'What's This' menu entry.
					// -----------------------------------
    static  DWORD       dwWhatsThisMap[] =
	{
	IDD_SCANMAIN,               IDH_NAVW_WHATHIS_MAIN_WINDOW,
	IDOK,                       IDH_NAVW_WHATHIS_SCAN_BTN,
	ID_SCANMAIN_DRIVE_LIST,     IDH_NAVW_MAIN_WINDOW_DRIVE_LIST,
	ID_SCANMAIN_DRIVES_FLOPPY,  IDH_NAVW_DRIVE_TYPES,
	ID_SCANMAIN_DRIVES_HARD,    IDH_NAVW_DRIVE_TYPES,
	ID_SCANMAIN_DRIVES_NETWORK, IDH_NAVW_DRIVE_TYPES,
	ID_SCANMAIN_DRIVETYPE_TEXT, IDH_NAVW_DRIVE_TYPES,
    IDC_STATUS_AP_BUTTON,       IDH_NAVW_WHATHIS_MAINWIN_DISABLE_BTN,
    IDC_STATUS_2_BUTTON,        IDH_NAVW_WHATHIS_MAINWIN_INFO_BTN,
    IDC_SYSTEM_WORKS,            IDH_NAVW_WHATHIS_MAINWIN_NSW_BTN,
	0,                          0
	};

				       // Invoke cmn Nav context sensitive help
    NavDlgOnContextHelp (
			hDlg, uMsg,
			wParam, lParam,
			dwWhatsThisMap, NULL,
			NULL,    //  This dlg. doesn't have any  ...
			NULL     //  ... List Box controls - so NULL here
			);
} // OnContextHelp()


//************************************************************************
// ShouldProcessCommandLine()
//
// Early check of the command line to see if we were launched with
// specific actions to take.
// A TRUE return signals later processing of the command line and
// automatic execution of actions/options passed.
//
// DALLEE - Poor design. Should nuke this and consolidate command
// line handling.
//
// Parameters:
//      LPTSTR  lpszCmdLine             Command line passed to app
//
// Returns:
//      TRUE    if any command line options other than /DEFAULT
//      FALSE   otherwise
//************************************************************************
// 03/26/1997 DALLEE, finally added a function header to this thing.
//************************************************************************

BOOL LOCAL ShouldProcessCommandLine (
    LPTSTR   lpszCmdLine
    )
{
    auto    CFGTEXTREC  cfgRec  =   { 0 };
    auto    TCHAR       szDefaultSwitch [ SYM_MAX_PATH ] = { 0 };
    auto    BOOL        bProcess = FALSE;

    if ( *lpszCmdLine )
	{
	LoadString ( ghInst, IDS_DEFAULT, szDefaultSwitch,
		     sizeof(szDefaultSwitch) );

	cfgRec.lpSwitch = szDefaultSwitch;
	cfgRec.wFlags = CFG_SW_EXACT;

					// -----------------------------------
					// If the default switch is set and
					// this is the only command line
					// switch then do not process
					// command lines further.
					// -----------------------------------

	if ( ConfigSwitchSet(&cfgRec, NULL) &&
	   ( ConfigParamCount() + ConfigSwitchCount() ) ==  1 )
	    {
	    bProcess = FALSE;
	    }
    else
	    {
        cfgRec.lpSwitch = _T("?");
        cfgRec.wFlags = CFG_SW_EXACT;

        if ( ConfigSwitchSet(&cfgRec, NULL) &&
           ( ConfigParamCount() + ConfigSwitchCount() ) ==  1 )
            {
            s_bCommandLineHelp = TRUE;
            bProcess = FALSE;
            }
        else
            {
            bProcess = TRUE;
            }
	    }
	}

    return ( bProcess );
} // ShouldProcessCommandLine()


/* **************************************************************************

@Name:
LRESULT LOCAL OnNCPaint(                // Handles caption painting for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of caption painting message
    LPARAM      lParam                  // lParam of caption painting message
    );

@Description:
Function handles the painting of the caption bar of the main window, giving it a "shaded"
look. This function works under both 95 and NT (OldShell and NewShell).

@Parameters:
$hWnd$          [in] Window handle of main dialog window
$msg$           [in] message
$wParam$        [in] wParam of message
$lParam$        [in] lParam of message

@Returns:
LRESULT         [out] Msg return

05/23/96 GWESTER Ported in from Norton Utilities
************************************************************************** */
int OnNCPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    RECT       rectDraw;
    RECT       rectWindow;
    RECT       rectOffset=rectDraw;

    // Get the dimensions of the entire window
    GetWindowRect(hWnd, (LPRECT)&rectWindow);

    // Draw our part of the caption (first 80% of the caption bar)
    DrawCaption(hWnd, &rectDraw, GetActiveWindow() == hWnd);

    // Offset our drawing rectangle
    OffsetRect (&rectDraw, rectWindow.left, rectWindow.top);

    // Create a region which includes all areas of the NC area
    // except that which DrawCaption painted into.
    // Note that we do NOT use the HRGN passed to us from the WM_NCPAINT
    // message, we use a brand new region.
    HRGN    rgnWnd;             // The total window region
    HRGN    rgnDest;            // The final region
    HRGN    rgnExclude;         // The area which we have already painted

    rgnWnd = CreateRectRgn(rectWindow.left, rectWindow.top, rectWindow.right, rectWindow.bottom);
    rgnExclude = CreateRectRgn(rectDraw.left, rectDraw.top, rectDraw.right, rectDraw.bottom);
    rgnDest = CreateRectRgn(rectWindow.left, rectWindow.top, rectWindow.right, rectWindow.bottom);
    CombineRgn (rgnDest, rgnWnd, rgnExclude, RGN_DIFF);

    // Now tell windows to paint the rest of the client area
    int nRet = DefWindowProc(hWnd, uMsg, (WPARAM)rgnDest, lParam);

    DeleteObject ( rgnWnd );
    DeleteObject ( rgnDest );
    DeleteObject ( rgnExclude );

    // Now tell windows to paint the rest of the client area
    return nRet;
}

/* **************************************************************************

@Name:
VOID DrawCaption(HWND hWnd, LPRECT lprDraw, BOOL bActive)

@Description:
Function handles the painting of the caption bar of the main window, giving it a "shaded"
look. This function works under both 95 and NT (OldShell and NewShell).

05/23/96 GWESTER Ported in from Norton Utilities
************************************************************************** */

VOID DrawCaption(HWND hWnd, LPRECT lprDraw, BOOL bActive)
{
    long        lColor,lTextColor;
    RECT        rect, rectTemp;
    BOOL        fLightToDark;
    WORD        wTitleButtons;
    HICON       hIcon;
    int         bkMode;
    TCHAR       abBuffer[300];
    HDC         hDC = GetWindowDC(hWnd);
    //CDC         *hdc = CDC::FromHandle(hDC);
    HDC         hdc = hDC;
    DWORD       dwStyle   = GetWindowLong(hWnd, GWL_STYLE);
    int         xBorder   = GetSystemMetrics(/*dwStyle & DS_MODALFRAME ? */SM_CXFIXEDFRAME/* : SM_CXSIZEFRAME*/);
    int         yBorder   = GetSystemMetrics(/*dwStyle & DS_MODALFRAME ? */SM_CYFIXEDFRAME/* : SM_CYSIZEFRAME*/);
    int         xSize   = GetSystemMetrics(SM_CXSIZE);
    int         ySize   = GetSystemMetrics(SM_CYCAPTION) - 1;//2; //- 1;
    int         xIconSize = GetSystemMetrics(SM_CXSMICON);
    int         yIconSize = GetSystemMetrics(SM_CYSMICON);
    NONCLIENTMETRICS ncm;
					// Account for 3 buttons in title.
					// Windows 95 style.
    wTitleButtons = 3;

    rect.left = xBorder;
    rect.top = yBorder;
    rect.bottom = rect.top + ySize;
    GetWindowRect(hWnd, &rectTemp);
    rect.right = rectTemp.right - rectTemp.left -
		 xBorder * 2 -
		 (xSize * wTitleButtons) + 3;

    (*lprDraw).left = rect.left;
    (*lprDraw).top = rect.top;
    (*lprDraw).right = rect.right;
    (*lprDraw).bottom = rect.bottom;

					// Get correct color based on
					// if we are active or not.
    if(bActive)
	lColor = GetSysColor(COLOR_ACTIVECAPTION);
    else
	lColor = GetSysColor(COLOR_INACTIVECAPTION);

					//
					// Off-screen drawing code.
					//
    COffScreenDC_Constructor (hdc, &rect);
    //CDC*            pDC = &dcOS;
    HDC pDC = thisCOffScreenDC;
					// Draw gradient caption bar
    fLightToDark = GradientFillRect(pDC, &rect, lColor, 50, FALSE);

					// Set text to white or grey depending on active/inactive state
    lTextColor = bActive ? RGB(0xFF,0xFF,0xFF) : GetSysColor(COLOR_GRAYTEXT);

					// Draw the icon
    //hIcon = LoadIcon(ghInst, MAKEINTRESOURCE(IDI_NAVW_APP_SMALL));
    hIcon = (HICON)LoadImage (ghInst, MAKEINTRESOURCE(IDI_NAVW_APP_SMALL), IMAGE_ICON, 16, 16, 0);

    DrawIconEx(pDC,
	       rect.left+2,
	       rect.top +1,
	       hIcon,
	       xIconSize,
	       yIconSize,
	       0,
	       NULL,DI_NORMAL);

					// Get correct font.
    HFONT font;
    HFONT oldFont;
    ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
    font = CreateFontIndirect(&ncm.lfCaptionFont);

					// Draw the text.
    oldFont = (HFONT)(SelectObject(pDC, font));
    GetWindowText(hWnd, abBuffer,sizeof(abBuffer)/sizeof(abBuffer[0]));

    bkMode = SetBkMode(pDC, TRANSPARENT);
    SetTextColor(pDC, lTextColor);
    rect.left += xSize + 2;
    DrawText(pDC, abBuffer, strlen(abBuffer), &rect, DT_SINGLELINE | DT_VCENTER| DT_LEFT | DT_END_ELLIPSIS);

    // better transfer onscreen before releasing the dc.
    COffScreenDC_Transfer();
					// Clean up.
					// Clean up.
    SetBkMode(pDC, bkMode);
    SelectObject(pDC, oldFont);
    DeleteObject(font);
    ReleaseDC(hWnd, hdc);

    COffScreenDC_Destructor();
}

//////////////////////////////////////////////////////////////////////
// GradientFillRect
//
// Fills a rectangle with a gradient fill.
//
// From Norton Navigator title bar drawing code.  Updated to use MFC and pulled
// into discreet function. - Rowan

BOOL GradientFillRect(HDC pDC, LPRECT rect, COLORREF lColor, int nSteps, BOOL bVertical)
{

    HBRUSH      brush;
    int         idx,xStep,xRange;
    long        crRed,crBlue,crGreen;
    RECT        rcBand;
    BOOL        bLightToDark=FALSE;

					    // Set fill range to 80% of width
    xRange = (bVertical ? ( (*rect).bottom - (*rect).top ) : ( (*rect).right - (*rect).left )) * 4 / 5;
					    // Width of each step

    xStep = xRange / nSteps;

					    // We always start with black.  MS does too.
    crRed = crBlue = crGreen = 0;

					    // Set-up the current fill block
    if(!bVertical)
	{
	rcBand.top = (*rect).top;
	rcBand.bottom = (*rect).bottom;
	}
    else
	{
	rcBand.left = (*rect).left;
	rcBand.right = (*rect).right;
	}
					    // Fill the gradient
    for (idx = 0; idx<nSteps; idx++)
	{
					    // Increment our fill block.  If we
					    // are in the first or last part of
					    // the total fill range, make the block
					    // one fifth of the width...
	if(bVertical)
	    {
	    rcBand.top = (*rect).top + xRange*idx/nSteps +
			  ((bLightToDark && idx) ? (((*rect).bottom)-((*rect).top)) / 5 : 0);
	    rcBand.bottom = rcBand.top + xStep +
			  ((bLightToDark && !idx) ? (((*rect).bottom)-((*rect).top)) / 5 : 0) + 1;
	    }
	else
	    {
	    rcBand.left = (*rect).left + xRange*idx/nSteps +
			  ((bLightToDark && idx) ? (((*rect).right)-((*rect).left)) / 5 : 0);
	    rcBand.right = rcBand.left + xStep +
			  ((bLightToDark && !idx) ? (((*rect).right)-((*rect).left)) / 5 : 0) + 1;
	    }
					    // Fill the rect
	brush = CreateSolidBrush(RGB(crRed,crGreen,crBlue));
	if(brush)
	    {
	    FillRect(pDC, &rcBand, brush);
	    DeleteObject(brush);
	    }

					    // Increment our color steps, taking into
					    // account rounding.
	crRed += (GetRValue(lColor) - crRed)     / (nSteps - idx);
	crGreen += (GetGValue(lColor) - crGreen) / (nSteps - idx);
	crBlue += (GetBValue(lColor) - crBlue)   / (nSteps - idx);
	}
					    // Fill in the right most part
					    // of the gradient
    if (!bLightToDark)
	{
	if(bVertical)
	    {
	    rcBand.top = (*rect).top + xRange;
	    rcBand.bottom = (*rect).bottom;
	    }
	else
	    {
	    rcBand.left = (*rect).left + xRange;
	    rcBand.right = (*rect).right;
	    }

	brush = CreateSolidBrush(lColor);
	if(brush)
	    {
	    FillRect(pDC, &rcBand, brush);
	    DeleteObject(brush);
	    }
	}

    return(bLightToDark);
}


VOID LOCAL COffScreenDC_Constructor ( HDC pxDC, LPRECT rxRect )
{
    thisCOffScreenDC = NULL;

    m_bTransfered = FALSE ;
    m_pxDC = pxDC ;
    m_xRect.left = (*rxRect).left ;
    m_xRect.top = (*rxRect).top ;
    m_xRect.right = (*rxRect).right ;
    m_xRect.bottom = (*rxRect).bottom ;

					// setup bitmap and DC
    thisCOffScreenDC = CreateCompatibleDC( pxDC ) ;
    m_xBitmap = CreateCompatibleBitmap ( pxDC, (*rxRect).right - (*rxRect).left, (*rxRect).bottom - (*rxRect).top ) ;
    m_pxOldBitmap = (HBITMAP)(SelectObject( thisCOffScreenDC , m_xBitmap ));
    SetViewportOrgEx ( thisCOffScreenDC , -((*rxRect).left) , -((*rxRect).top) , NULL );
}                                       // End COffScreenDC_Constructor ()



VOID LOCAL COffScreenDC_Destructor ( VOID )
{

					// transfer if necessary
    if( ! m_bTransfered )
	COffScreenDC_Transfer();
					// restore old bitmap
    SelectObject( thisCOffScreenDC , m_pxOldBitmap );

    if (NULL != m_xBitmap)
	{
	DeleteObject(m_xBitmap);
	m_xBitmap = NULL;
	}

    if (NULL != thisCOffScreenDC)
	{
	DeleteDC(thisCOffScreenDC);
	thisCOffScreenDC = NULL;
	}
}                                       // End COffScreenDC_Destructor ()



VOID LOCAL COffScreenDC_Transfer( VOID )
{

					// transfer bitmap
    BitBlt
	(   m_pxDC,
	    m_xRect.left, m_xRect.top,
	    m_xRect.right - m_xRect.left,
	    m_xRect.bottom - m_xRect.top,
	    thisCOffScreenDC,
	    m_xRect.left, m_xRect.top,
	    SRCCOPY );
    m_bTransfered = TRUE;

}                                       // End COffScreenDC_Transfer ()


//************************************************************************
// IsBadPlatform()
//
// Checks for running on 95 if NT only and vice versa.
// Please add weirdo platforms checks here instead of littering willy
// nilly through startup chores and winmain, etc.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 08/22/1996 DALLEE, grouped NT and 95 platform checks here instead
//      of cluttering WinMain().
//************************************************************************

BOOL LOCAL IsBadPlatform ( VOID )
{
    auto    NOTIFYSTRUCT Notify     = {0};
    auto    BOOL         bReturn    = FALSE;

#ifdef NAVWNT
					// If we're building for NT but
					// executing in W95, exit gracefully
    if (SYM_SYSTEM_WIN_95 == SystemGetWindowsType())
	{
	TCHAR   szErrorStarting[200];
	TCHAR   szNotFor95[2000];

	LoadString(ghInst, IDS_ERROR_STARTING, szErrorStarting, sizeof(szErrorStarting)-1);
	LoadString(ghInst, IDS_NOTFORWIN95, szNotFor95, sizeof(szNotFor95)-1);
	MessageBox(NULL, szNotFor95, szErrorStarting, MB_OK | MB_ICONEXCLAMATION);

	bReturn = TRUE;
	}

					// Make sure scheduled scans in NT
					// have access to atoms for LPSET/GET
    if (NULL == OpenWindowStation("WinSta0", FALSE, WINSTA_ACCESSGLOBALATOMS))
	{
	Notify.hParent    = GetDesktopWindow();
	Notify.uMsgID     = IDS_RUNENVIRONMENT;
	Notify.uTitleID   = IDS_NAVW;
	Notify.lpAlertMsg = NULL;
	Notify.hInstance  = ghInst;

	NotifyUser (&Notify);
	bReturn = TRUE;
	}
#endif

					// Don't run W95 version on NT platform.
#if !defined(NAVSCANW) && !defined(NAVWNT)
    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
	{
	MEMSET (&Notify, EOS, sizeof(Notify));
	Notify.hParent       = GetDesktopWindow();
	Notify.uMsgID        = IDS_NOTFORWINNT;
	Notify.uTitleID      = IDS_NAVW;
	Notify.hInstance     = ghInst;

	NotifyUser (&Notify);
	bReturn = TRUE;
	}
#endif

    return (bReturn);
} // IsBadPlatform()


//************************************************************************
// NavMainInitToolBar()
//
// VOID LOCAL NavMainInitToolBar ( HWND hWnd )
//
// This routine initializes the tool bar (tough to figure that one out).
//
// Calls NavMainAddToolBarButtons() for adding all buttons.
// Make changes there for adding subtracting things from the toolbar.
//
// Parameters:
//      HWND    hWnd        Handle of main dialog.
//
// Returns:
//      Nothing.
//************************************************************************
// 09/24/1996 DALLEE, move button bar initialization here from main init dlg.
//************************************************************************

VOID LOCAL NavMainInitToolBar ( HWND hWnd )
{
    // Static classes for Bar controls - these need to remain in scope for
    // the duration of the main window.

    static  CNavMainToolBarCtrl toolBar;
    static  CNVReBarCtrl        reBar;

    auto    REBARBANDINFO       rBandInfo;
    auto    RECT                rToolBarRect;

    if ( toolBar.Create( ghInst,
			 WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_GROUP \
			    | CCS_TOP | CCS_NORESIZE | CCS_NODIVIDER \
			    | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
			 NULL,
			 hWnd,
			 ID_SCANMAIN_TOOLBAR ) )
    {
	// Add buttons.

	NavMainAddToolBarButtons( &toolBar );


	//
	// Check to see if the toolbar is too wide.
	//

	RECT rRect;
	toolBar.GetItemRect( 0, &rRect );

	RECT rClientRect;
	GetClientRect( hWnd, &rClientRect );

	int iButtonCount = toolBar.GetButtonCount();
	if( ( 20 + (rRect.right - rRect.left) * iButtonCount) > rClientRect.right )
	    {
	    // It is, so remove scheduler button.  If scheduler is not
        // installed, this removes the activity log button.
        toolBar.DeleteButton( 2 );

        // Try again.  If we are still too small, remove the virus list button.
        iButtonCount = toolBar.GetButtonCount();
        if( ( 20 + (rRect.right - rRect.left) * iButtonCount) > rClientRect.right )
            {
            toolBar.DeleteButton( 1 );
            }
	    }

	//
	// Display the toolbar
	//

	::ShowWindow( toolBar.m_hWnd, SW_SHOW );

	// Create ReBar (IE-style "cool bar") for nice border effect.

	memset( &rToolBarRect, 0, sizeof( rToolBarRect ) );
	toolBar.GetItemRect( 0, &rToolBarRect );

	reBar.Create( ghInst,
		      WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN \
			| WS_CLIPSIBLINGS | WS_GROUP \
			| RBS_VARHEIGHT | RBS_BANDBORDERS | RBS_FIXEDORDER,
		      NULL,
		      hWnd,
		      ID_SCANMAIN_REBAR );

	memset( &rBandInfo, 0, sizeof( rBandInfo ) );

	rBandInfo.cbSize     = sizeof( rBandInfo );
	rBandInfo.fMask      = RBBIM_STYLE | RBBIM_CHILD | RBBIM_COLORS | RBBIM_ID | RBBIM_CHILDSIZE;
	rBandInfo.fStyle     = RBBS_FIXEDBMP | RBBS_FIXEDSIZE | RBBS_CHILDEDGE | 0x80; // 0x80 - always show gripper
	rBandInfo.clrFore    = GetSysColor( COLOR_BTNTEXT );
	rBandInfo.clrBack    = GetSysColor( COLOR_BTNFACE );
	rBandInfo.hwndChild  = toolBar;
	rBandInfo.cxMinChild = rToolBarRect.right - rToolBarRect.left + 11; // Rough fudge factor for gripper. Min width accuracy not that important...
	rBandInfo.cyMinChild = rToolBarRect.bottom - rToolBarRect.top;
	rBandInfo.wID        = ID_SCANMAIN_TOOLBAR;

	reBar.InsertBand( -1, &rBandInfo );
    }
} // NavMainInitToolBar()


//*************************************************************************
// NavMainAddToolBarButtons()
//
// VOID LOCAL NavMainAddToolBarButtons(
//      CNavMainToolBarCtrl *pToolBar )
//
// This routine adds all the buttons to the toolbar.
// Called from NavMainInitToolBar().
//
// Add platform/version checks and add subtract buttons from toolbar here.
//
// Returns:
//      Nothing.
//*************************************************************************
// 04/01/1998 DALLEE, created - header added.
//*************************************************************************

VOID LOCAL NavMainAddToolBarButtons( CNavMainToolBarCtrl *pToolBar )
{
    auto    HBITMAP     hSizeBitmap;
    auto    BITMAP      rSizeBitmapInfo;

    auto    NAVTBBUTTON rButton;

    auto    int         nWidth  = 22;   // Size of buttons (with default)
    auto    int         nHeight = 20;

    //
    // Set size of buttons based on options button.
    //

    hSizeBitmap = (HBITMAP) LoadImage( ghInst,
				       MAKEINTRESOURCE( IDB_SCANMAIN_BBAR_OPTIONS ),
				       IMAGE_BITMAP,
				       0, 0,
				       LR_DEFAULTCOLOR );

    if ( hSizeBitmap )
    {
	if ( GetObject( (HGDIOBJ) hSizeBitmap, sizeof( rSizeBitmapInfo ), &rSizeBitmapInfo ) )
	{
	    nWidth  = rSizeBitmapInfo.bmWidth;
	    nHeight = rSizeBitmapInfo.bmHeight;
	}

	DeleteObject( (HGDIOBJ) hSizeBitmap );
    }

    pToolBar->SetButtonSize( nWidth, nHeight );
    pToolBar->SetBitmapSize( nWidth, nHeight );

    //
    // Add buttons to toolbar
    //

    // Options

    memset( &rButton, 0, sizeof( rButton ) );

    rButton.hInst      = ghInst;
    rButton.uBitmap    = IDB_SCANMAIN_BBAR_OPTIONS;
    rButton.uHotBitmap = 0;
    rButton.uString    = IDS_SCANMAIN_BBAR_OPTIONS;
    rButton.uID        = IDM_TOOLS_OPTIONS;

    pToolBar->AddButton( &rButton );

    // Virus List

    // rButton.hInst      = ghInst;
    rButton.uBitmap    = IDB_SCANMAIN_BBAR_VIRUSLIST;
    rButton.uHotBitmap = 0;
    rButton.uString    = IDS_SCANMAIN_BBAR_VIRUSLIST;
    rButton.uID        = IDM_TOOLS_VIRUSLIST;

    pToolBar->AddButton( &rButton );

    // Scheduler

    if ( SchedulerIsInstalled() )
    {
	// rButton.hInst      = ghInst;
	rButton.uBitmap    = IDB_SCANMAIN_BBAR_SCHEDULER;
	rButton.uHotBitmap = 0;
	rButton.uString    = IDS_SCANMAIN_BBAR_SCHEDULER;
	rButton.uID        = IDM_TOOLS_SCHEDULER;

	pToolBar->AddButton( &rButton );
    }

    // Activity Log

    // rButton.hInst      = ghInst;
    rButton.uBitmap    = IDB_SCANMAIN_BBAR_ACTIVITYLOG;
    rButton.uHotBitmap = 0;
    rButton.uString    = IDS_SCANMAIN_BBAR_ACTIVITYLOG;
    rButton.uID        = IDM_TOOLS_ACTIVITYLOG;

    pToolBar->AddButton( &rButton );

    // Quarantine

    if ( IsQuarantineValid() )
    {
	// rButton.hInst      = ghInst;
	rButton.uBitmap    = IDB_SCANMAIN_BBAR_QUARANTINE;
	rButton.uHotBitmap = 0;
	rButton.uString    = IDS_SCANMAIN_BBAR_QUARANTINE;
	rButton.uID        = IDM_TOOLS_QUARANTINE;

	pToolBar->AddButton( &rButton );
    }

    // Live Update

    if ( IsLiveUpdateValid() )
    {
	// rButton.hInst      = ghInst;
	rButton.uBitmap    = IDB_SCANMAIN_BBAR_ONRAMP;
	rButton.uHotBitmap = 0;
	rButton.uString    = IDS_SCANMAIN_BBAR_ONRAMP;
	rButton.uID        = IDM_TOOLS_ONRAMP;

	pToolBar->AddButton( &rButton );
    }

    //
    // Resize toolbar after adding bitmaps and text.
    //

    pToolBar->AutoSize();


} // NavMainAddToolBarButtons()


//************************************************************************
// NavMainResizeControls()
//
// VOID LOCAL NavMainResizeControls ( HWND hWnd )
//
// This routine handles all the resize and font setting of the NAV main
// window controls. This routine should be called once during
// WM_INITDIALOG for the main dialog.
//
// Parameters:
//      HWND    hWnd                    Handle of main window.
//
// Returns:
//      Nothing.
//************************************************************************
// 09/24/1996 DALLEE, Moved NG_xxx routines here from WM_INITDIALOG and added
//              resizing of groupboxes to match drive list control.
//************************************************************************

VOID LOCAL NavMainResizeControls ( HWND    hWnd )
{
    auto    HWND    hControl;

    auto    RECT    rControlRect;
    auto    POINT   rControlPos;
    auto    int     nHeight;

    NG_SetFontsInDialog (hWnd, 0);
    NG_SetChildSmallFont (hWnd, ID_SCANMAIN_STATUS);

    NG_SetGroupBoxSunken (hWnd, ID_SCANMAIN_DRIVES_GROUP);
    NG_SetGroupBoxSunken (hWnd, ID_SCANMAIN_BUTTONS_GROUP);
    NG_SetChildSmallFont (hWnd, ID_SCANMAIN_DRIVES_FLOPPY);
    NG_SetChildSmallFont (hWnd, ID_SCANMAIN_DRIVES_HARD);
    NG_SetChildSmallFont (hWnd, ID_SCANMAIN_DRIVES_NETWORK);
    NG_SetChildSmallFont (hWnd, ID_SCANMAIN_DRIVES_SELECTED);

					// Resize groupboxes to match
					// height of drive list which has
					// integral height set.

    hControl = GetDlgItem(hWnd, ID_SCANMAIN_DRIVE_LIST);

    if ( FALSE != GetWindowRect(hControl, &rControlRect) )
	{
	const short wBorderHeightFudgeFactor = 8;
	const short wTopFudgeFactor          = -1;

	nHeight = rControlRect.bottom - rControlRect.top;
	nHeight += wBorderHeightFudgeFactor;

					// Reset height of drives groupbox

	hControl = GetDlgItem(hWnd, ID_SCANMAIN_DRIVES_GROUP);
	if ( FALSE != GetWindowRect(hControl, &rControlRect) )
	    {
	    rControlPos.x = rControlRect.left;
	    rControlPos.y = rControlRect.top + wTopFudgeFactor;

	    ScreenToClient(hWnd, &rControlPos);

	    SetWindowPos( hControl, NULL,
			  rControlPos.x,
			  rControlPos.y,
			  rControlRect.right - rControlRect.left,
			  nHeight,
			  SWP_NOZORDER );
	    }

					// Reset height of buttons groupbox

	hControl = GetDlgItem(hWnd, ID_SCANMAIN_BUTTONS_GROUP);
	if ( FALSE != GetWindowRect(hControl, &rControlRect) )
	    {
	    rControlPos.x = rControlRect.left;
	    rControlPos.y = rControlRect.top + wTopFudgeFactor;

	    ScreenToClient(hWnd, &rControlPos);

	    SetWindowPos( hControl, NULL,
			  rControlPos.x,
			  rControlPos.y,
			  rControlRect.right - rControlRect.left,
			  nHeight,
			  SWP_NOZORDER );
	    }
	}
} // NavMainResizeControls()


//************************************************************************
// NavMainStatusTimer()
//
// static VOID CALLBACK NavMainStatusTimer (
//      HWND     hWnd,                  // Main window
//      UINT     uMessage,              // WM_TIMER
//      UINT     uId,                   // Timer ID
//      DWORD    dwTime )               // Current system time
//
// This is the timer callback which updates the status (AP, Quar, Defs)
// information in the main dialog.
//
// Returns:
//************************************************************************
// 06/11/1997 DALLEE, created.
//************************************************************************

static VOID CALLBACK NavMainStatusTimer( HWND     hWnd,
					 UINT     uMessage,
					 UINT     uId,
					 DWORD    dwTime )
{
    if ( UpdateApStatus( &NavMain.rApStatus ) )
    {
	DisplayApStatus( hWnd, &NavMain );
    }

	DisplaySecondaryStatus( hWnd, &NavMain );

} // NavMainStatusTimer()


//************************************************************************
// HandleToolbarTip()
//
// BOOL LOCAL HandleToolbarTip( WPARAM wParam, LPARAM lParam )
//
// This routine handles the main toolbar tooltip notification message.  This
// messages comes via WM_NOTIFY.
//
//
// Returns: TRUE if message was handled.
//          FALSE otherwise.
//************************************************************************
// 04/29/1998 DBUCHES, created.
//************************************************************************

BOOL LOCAL HandleToolbarTip( WPARAM wParam, LPARAM lParam )
{
    TOOLTIPTEXT *pTip = (TOOLTIPTEXT *) lParam;
    int         iStrID;


    // Is this a tooltip request?
    if( pTip->hdr.code != TTN_NEEDTEXT )
	{
	return FALSE;
	}

    // Get correct tooltip text.
    switch( pTip->hdr.idFrom )
	{
	case IDM_TOOLS_OPTIONS:
	    iStrID = IDS_SCANMAIN_BBAR_OPTIONS_TT;
	    break;
	case IDM_TOOLS_VIRUSLIST:
	    iStrID = IDS_SCANMAIN_BBAR_VIRUSLIST_TT;
	    break;
	case IDM_TOOLS_SCHEDULER:
	    iStrID = IDS_SCANMAIN_BBAR_SCHEDULER_TT;
	    break;
	case IDM_TOOLS_ACTIVITYLOG:
	    iStrID = IDS_SCANMAIN_BBAR_ACTIVITYLOG_TT;
	    break;
	case IDM_TOOLS_QUARANTINE:
	    iStrID = IDS_SCANMAIN_BBAR_QUARANTINE_TT;
	    break;
	case IDM_TOOLS_ONRAMP:
	    iStrID = IDS_SCANMAIN_BBAR_ONRAMP_TT;
	    break;

	default:
	    return TRUE;
	}

    // Save off instance.
    pTip->hinst = ghInst;

    // Save off text
    pTip->lpszText = MAKEINTRESOURCE( iStrID );
    return TRUE;

}



#ifdef NAV_PREVIEW

//************************************************************************
// IsTrialPeriodOverridden(int *pTrialLength)
//
// Checks to see if the trial period is overridden by the key value
// \\HKEY_LOCAL_MACHINE\SOFTWARE\Symantec\SymSE\NAVTrialLength
//
// Parameters:
//		int *pTrialLength	Points to an integer containing the current
//							trial length.  If the length is overridden,
//							the new value is written to this integer
//
// Returns:
//      TRUE    if trial length overridden.
//      FALSE   trial length not overridden.
//************************************************************************
//
//************************************************************************

BOOL LOCAL IsTrialPeriodOverridden(int *pTrialLength)
{	HKEY hKey;
	DWORD dwLength, dwType, dwSize;
	BOOL bResult = FALSE;

	if(RegOpenKey(	HKEY_LOCAL_MACHINE,
					_T("SOFTWARE\\Symantec\\SymSE"),
					&hKey) == ERROR_SUCCESS)
	{	dwSize = sizeof(dwLength);
		if(RegQueryValueEx(	hKey,
							_T("NAVTrialLength"),
							NULL,
							&dwType,
							(LPBYTE) &dwLength,
							&dwSize) == ERROR_SUCCESS &&
							dwType == REG_DWORD)
		{	*pTrialLength = dwLength;
			bResult = TRUE;
		}
		RegCloseKey(hKey);
	}
	return bResult;
}

//************************************************************************
// SetupTrial()
//
// ?? Creates a 30 day trial period for NAV?
//
// Parameters:
//
// Returns:
//      TRUE    if trial object created.
//      FALSE   on error.
//************************************************************************
// ???
//************************************************************************

BOOL LOCAL SetupTrial()
{
    auto    ITrialWare  *pTrial;
    auto    BOOL        fSucceeded = FALSE;
	auto	int			iTrial = DEFAULT_TRIAL_LENGTH;

    // Initalize COM
    CoInitialize(NULL);

    // attempt to create trial object
    HRESULT hResult = CoCreateInstance( CLSID_TrialWare,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_ITrialWare,
					(LPVOID *)&pTrial );

    if ( SUCCEEDED( hResult ) )
	{
	// attempt to initialize trial object
	if ( SUCCEEDED( pTrial->Initialize( TEXT("Symantec"),
					    TEXT("Norton AntiVirus"),
					    NAV_VERSION_MAJOR,
					    NAV_VERSION_MINOR ) ) )
	    {
			// Change the trial length if the override key exists
			// in the registry
			IsTrialPeriodOverridden(&iTrial);
			if(iTrial > MAX_TRIAL_LENGTH)
			   iTrial = MAX_TRIAL_LENGTH;

	    // display error message and cancel operation if call failed
	    if ( SUCCEEDED( pTrial->Setup( iTrial, 0L )))
		{
		fSucceeded = TRUE;
		}
	    }

	// release TrialWare object
	pTrial->Release();
	}

    // Balance call to CoInitalize;
    CoUninitialize();

    return ( fSucceeded );
}

BOOL LOCAL IsTrialValid()
{
    auto    ITrialWare  *pTrial;
    auto    BOOL        bExpired = TRUE;

    // Initalize COM
    CoInitialize(NULL);

    // attempt to create trial object
    if ( SUCCEEDED( CoCreateInstance( CLSID_TrialWare,
				      NULL,
				      CLSCTX_INPROC_SERVER,
				      IID_ITrialWare,
				      (LPVOID *)&pTrial ) ) )
	{
	// attempt to initialize trial object

	if ( SUCCEEDED( pTrial->Initialize( TEXT( "Symantec" ),
					    TEXT( "Norton AntiVirus" ),
					    NAV_VERSION_MAJOR,
					    NAV_VERSION_MINOR ) ) )
	    {
	    if ( pTrial->IsValid( HWND_DESKTOP, TRUE, NULL ) == S_OK )
		{
		bExpired = FALSE;
		}
	    else
		{
		char szFormat [1024], szMessage [1024], szTitle [100];
        DWORD dwTrialLength = 30, dwBufSize = sizeof(DWORD);
        LONG  lRet;
        HKEY  hkey;

		LoadString (ghInst, IDS_LICENSEEXPIRED, szFormat, sizeof(szFormat) - 1);
		LoadString (ghInst, IDS_NAVW, szTitle, sizeof(szTitle) - 1);

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

        SPRINTF ( szMessage, szFormat, dwTrialLength );

		MessageBox(NULL, szMessage, szTitle, MB_OK | MB_ICONINFORMATION);
		}
	    }

	pTrial->Release();
	}

    // Balance call to CoInitalize;
    CoUninitialize();

    return ( !bExpired );
}

#endif // NAV_PREVIEW
