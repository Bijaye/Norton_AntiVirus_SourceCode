/* Copyright 1993 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/NPSUI/VCS/NPS.CPv   1.21   28 Aug 1998 17:19:44   JBRENNA  $ *
 *                                                                      *
 * Description:                                                         *
 *      This module contains the code for the Norton Program Scheduler  *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/NPSUI/VCS/NPS.CPv  $ *
// 
//    Rev 1.21   28 Aug 1998 17:19:44   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.20   11 May 1998 12:31:58   mdunn
// Now resizing list control columns so the columns are always wide enough
// to display all data.
// 
//    Rev 1.19   06 May 1998 15:07:46   mdunn
// Now using SYMABOUT for the about box.
// 
//    Rev 1.18   24 Apr 1998 15:17:38   mdunn
// Now signaling the login event after starting the NPS service so that it will
// check for missed events.
// 
//    Rev 1.17   17 Apr 1998 12:16:48   mdunn
// #undef'ed TRACE since it now collides with an MFC function.
// 
//    Rev 1.16   13 Apr 1998 15:33:16   mdunn
// D'oh...that's version _five_ not four.
// 
//    Rev 1.15   13 Apr 1998 15:06:08   mdunn
// Disabled gradient caption bar painting code when running on NT version >= 5
// 
//    Rev 1.14   30 Mar 1998 12:17:48   mdunn
// Disabled NpsUpdateCaption() since it's not really needed anymore (I just
// #ifdef'ed it out).
// Added code to handle reading and writing the autorun missed events option
// from/to the INI file.
// 
//    Rev 1.13   10 Sep 1997 16:14:04   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.12.1.3   25 Aug 1997 17:10:02   KSACKIN
// Now calling directly to the nsched32 help file.  This is so that when NU is
// installed and they register Infodesk as the contents viewer, we will still
// load the NPS help file.  Previously, we would load infodesk, and you would
// not be able to find any scheduler help.
//
//    Rev 1.12.1.2   14 Aug 1997 22:32:36   KSACKIN
// Added support for specifying in the schedule.ini whether or not to allow
// a blank action text, and if now, a custom error message per event type
// if you have a blank action text.
//
//    Rev 1.12.1.1   13 Aug 1997 22:28:00   KSACKIN
// Added support for default values for our Action Text and our Description
// field.
//
//    Rev 1.12.1.0   12 Aug 1997 22:55:54   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.12   06 Aug 1997 09:05:34   JBRENNA
// Update StartScheduler() call to pass a NULL as the new parameter.
//
//    Rev 1.11   16 Jul 1997 16:09:32   DBUCHES
// Nuked version checking.
//
//    Rev 1.10   15 Jul 1997 15:50:24   DBUCHES
// Added Ctl3d for DEC Alpha
//
//    Rev 1.9   11 Jul 1997 23:30:44   KSACKIN
// Fixed some bogus year handling in order to accomplish year 2000 support.
//
//    Rev 1.8   08 Jul 1997 17:06:20   KSACKIN
// Rolled back the changes for the new about box.  We don't need the new
// functionality, which was incorrect for us anyway.
//
//    Rev 1.7   08 Jul 1997 16:10:56   KSACKIN
// Removed a call to load the NPS Options before we configure them.  This call
// was overwriting the current working options for the NPS.  If you had changed
// the options through the confirmation dialogs, the settings would be lost
// when you looked at the options again.
//
//    Rev 1.6   07 Jul 1997 19:45:18   ksackin
// Updated out menu item for InfoDesk to call to the standard help file for
// the scheduler.  We are now calling our menu item Contents, and call to
// NSchedNT.Hlp.
//
//    Rev 1.5   03 Jul 1997 18:15:36   KSACKIN
// Implemented a ListView control for showing the scheduled items.
//
//    Rev 1.4   02 Jul 1997 10:42:18   TIVANOV
// use the new aboutbox
//
//    Rev 1.3   29 Jun 1997 19:49:26   KSACKIN
// Removed gradient title bar support for all versions of windows <= 4.  This
// fixes a bug where we draw the title bar incorrectly on NT 3.51, as well as
// the fact that this was not designed for those operating systems.
//
//    Rev 1.2   17 Jun 1997 15:51:44   KSACKIN
// Removed timer code to start, stop, and kill timer.  Also moved the
// ParseCmdLine() function into this module from NPSDo.cpp.  NPSDO.cpp is no
// longer used in this project.  This is only the configuration UI.
//
//    Rev 1.1   10 Jun 1997 13:21:50   KSACKIN
// Updated to use the NPSAPI.
//
//    Rev 1.0   19 May 1997 00:24:56   KSACKIN
// Initial revision.
//
//    Rev 1.73   07 May 1997 18:51:24   BILL
// Removed ctl3d for Alpha platform
//
//    Rev 1.72   01 Oct 1996 16:02:00   JBRENNA
// Always set the bShowTitleTime option to FALSE when reading from INI file.
// At least always for SYM_WIN32 platforms (Win95).
//
//    Rev 1.71   05 Aug 1996 15:26:58   JBRENNA
// LuigiPlus Port:
//   DALLEE:
//     Delete temp DC and bitmap created during WM_NCPAINT for gradient caption.
//
//   GWESTER:
//     Fixed 57642, in which the caption bar was flickering excessively when
//     it was gaining / losing focus.
//
//   YVANG:
//     Added a comment regarding loading a string of IDS_NPS_INI_SECTION_NAMED,
//     which is now equated to all capitalized string. (Related to Defect
//     No. 57663)
//
//   GWESTER:
//     Ported in gradiant caption bars from NU.
//
//    Rev 1.70   22 Feb 1996 13:19:14   YVANG
// Renamed 1) 'nstdhelp.h' to 'stdhelp.h', 2) 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()', and 3) StdHelpInit() to StdHelpExInit()
// to use Quake's StdHelp merged with the NAV version.
//
//    Rev 1.69   11 Jan 1996 09:47:20   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.68.1.1   21 Nov 1995 20:18:56   MKEATIN
// Added version "95.0.a" in the help 'About' dialog box
//
//    Rev 1.68   06 Jul 1995 20:15:10   MARKL
// Reports correct product name and build version information.
//
//    Rev 1.67   05 Jul 1995 16:19:04   BARRY
// Fixed startup problem with text button bar. (Gary)
//
//    Rev 1.66   28 Jun 1995 18:10:16   KEITH
// Add asserts, and fix an invalidate rect on the desktop!
//
//    Rev 1.65   28 Jun 1995 16:52:02   KEITH
// Similar to the last fix, nfreqs[NFREQ] was being accessed.
//
//    Rev 1.64   28 Jun 1995 16:43:18   KEITH
// Fixed a problem where months[NMONTH] was being accessed, which is one
// beyond the actual size of the array. Also, actually handle the resource
// load errors.
//
//    Rev 1.63   27 Jun 1995 13:47:44   KEITH
// Put OemToAnsi on user and company names after NAVOPTS.DAT read.
//
//    Rev 1.62   24 Jun 1995 16:36:06   MARKL
// Prevented program from running under Windows NT (DUMB!!!) for defect #36309
//
//    Rev 1.61   23 Jun 1995 15:24:56   BARRY
// Temporary fix to missing ContextHelp problem in build 60.
//
//    Rev 1.60   21 Jun 1995 00:51:52   GWESTER
// Fixed references to help files so that InfoDesk is launched
// in a consistent manner.
//
//    Rev 1.59   15 Jun 1995 23:07:16   GWESTER
// Fixed 35522, in which Scheduler doesn't read or
// use the current values of its options.
//
//    Rev 1.58   13 Jun 1995 16:17:06   GWESTER
// Added /MIN command-line switch, which forces the scheduler
// to run minimized. ( Used by NAV Install. )
//
//    Rev 1.57   09 Jun 1995 23:21:40   GWESTER
// Made certain button bar paints itself at startup.
//
//    Rev 1.56   07 Jun 1995 12:55:06   MARKL
// If there is no Win95 tray, then show our icon.
//
//    Rev 1.55   25 May 1995 17:07:34   BARRY
// Corrected DisplayInfoDeskHelp().
//
//    Rev 1.54   25 May 1995 17:04:10   BARRY
// Added DisplayInfoDeskHelp(); set up WM_COMMAND item
// IDM_NPS_INFODESK so that it triggers the same InfoDesk
// as in NAVW.  (Gary)
//
//    Rev 1.53   10 May 1995 14:50:22   MARKL
// Fixed defect 34703 where program would show up in task bar in user
// selected exit application then decided he really wanted to minimize it
// instead.
//
//    Rev 1.52   03 May 1995 13:19:56   MFALLEN
// Moved stdhelp.h higher up in the include order.
//
//    Rev 1.51   03 May 1995 12:58:00   MFALLEN
// Tray icon.
//
//    Rev 1.50   02 May 1995 13:39:10   MFALLEN
// Using quake shell api
//
//    Rev 1.49   01 May 1995 17:39:34   MFALLEN
// InitShellLink() was missing from an area.
//
//    Rev 1.48   27 Apr 1995 20:10:42   BRAD
// Added Ctl3D
//
//    Rev 1.47   25 Apr 1995 20:01:06   BRAD
// Don't install Ctl3d, if Win95
//
//    Rev 1.46   25 Apr 1995 19:52:06   BRAD
// Enabled Ctl3d and got working for NT
//
//    Rev 1.45   25 Apr 1995 19:25:30   BRAD
// Can't use Shell stuff for NT
//
//    Rev 1.44   23 Apr 1995 22:17:38   MFALLEN
// On Win32 platform use IShellLink interface to add application to startup
// group.
//
//    Rev 1.43   21 Apr 1995 18:57:52   MFALLEN
// When restored from a minimized state display toolbar correctly.
//
//    Rev 1.42   18 Apr 1995 14:10:26   MFALLEN
// LFN program scheduling.
//
//    Rev 1.41   31 Mar 1995 18:29:02   DALLEE
// Added/adjusted help id's for right-click.
//
//    Rev 1.40   14 Mar 1995 17:08:22   MFALLEN
// Special confirmation when exiting.
//
//    Rev 1.39   14 Mar 1995 13:02:14   MFALLEN
// Added How To help.
//
//    Rev 1.38   13 Mar 1995 15:34:20   MFALLEN
// Added what's this help. No help file to test with at present time.
//
//    Rev 1.37   06 Mar 1995 10:10:26   MFALLEN
// Made it work for the Win95 platform and added required enhancements to
// the product to give it a native Win95 look and feel.
//
//    Rev 1.36   03 May 1994 23:26:38   BRAD
// Don't use NFileOpen() anymore
//
//    Rev 1.35   30 Mar 1994 11:32:04   PATRICKM
// Checked in from latest trunk src
//
//    Rev 1.36   29 Mar 1994 01:51:22   BRAD
// WIN32 changes
//
//    Rev 1.35   26 Mar 1994 21:00:22   BRAD
// Cleaned up Windows messages
//
//    Rev 1.34   15 Mar 1994 11:58:06   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.33   03 Mar 1994 12:09:48   BRAD
// AWK script applied
//
//    Rev 1.32   10 Feb 1994 12:48:50   PGRAVES
// Add QAK2 changes to trunk.
//
//    Rev 1.27.2.7   16 Dec 1993 20:13:02   MARKL
// Fixed button bar base address logic.
//
//    Rev 1.27.2.6   13 Dec 1993 18:43:10   MARKL
// Removed ifdefs for NDW
//
//    Rev 1.27.2.5   07 Dec 1993 14:01:42   MFALLEN
// Windows wasn't being redrawn when button bar changed.
//
//    Rev 1.27.2.4   09 Nov 1993 23:57:52   BRUCEH
// Hooked up help for toolbar menu item
//
//    Rev 1.27.2.3   07 Nov 1993 17:17:48   BRUCEH
// Added case for help toolbar button so help will come up
//
//    Rev 1.27.2.2   28 Oct 1993 04:43:18   BASIL
// Added the ability to switch off the display of time in icon mode.  This can
// be used to prevent the scheduler from waking screen savers up.
//
//    Rev 1.27.2.1   19 Oct 1993 20:59:32   BASIL
// [FIX] Update toolbar when deleting expired events.
//
//    Rev 1.27.2.0   19 Oct 1993 08:15:04   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.27   18 Oct 1993 20:28:58   BRUCEH
// Fixed about box command msg so about box will come up
//
//    Rev 1.26   18 Oct 1993 19:37:44   BRUCEH
// Added registration block and new function that will check the local
// reg block before looking to NAV for user info
//
//    Rev 1.25   15 Oct 1993 02:03:06   BRUCEH
// Fixed ,
//
//    Rev 1.24   15 Oct 1993 01:57:36   BRUCEH
// modified help links
//
//    Rev 1.23   15 Oct 1993 00:23:54   BASIL
// [FIX] check expired events when size restored [11148]
//
//    Rev 1.22   13 Oct 1993 14:09:16   BASIL
// [FIX] prevent NULL windows and handles (removed the setting
// of default button)  [#11110]
//
//    Rev 1.21   12 Oct 1993 16:49:12   BASIL
// Disable buttonbar resetting in WM_SIZE until dialog has been completly
// initialized, this prevents invalid window messages during window Move's
//
//    Rev 1.20   01 Oct 1993 13:00:04   MARKL
// Now fully supports named events.
//
//    Rev 1.19   30 Sep 1993 09:47:42   MARKL
// Somebody had commented out the IsDialogMessage() calls in the main
// GetMessage loop.  This prevented keyboard input from working in the
// modeless notification dialogs.
//
//    Rev 1.18   29 Sep 1993 14:53:42   MARKL
// No change.
//
//    Rev 1.17   24 Sep 1993 09:32:56   ED
// Version Checking
 ************************************************************************/

// #define     MAIN                        /* So we declare externals      */
#include    "platform.h"
#include    "stdhelp.h"
#include    <ctl3d.h>
#include    <stdlib.h>
#include    <sys\timeb.h>
#include    <time.h>
#include    <commdlg.h>
#include    "undoc.h"                  /* for extra typedefs           */
#include    "ngui.h"
#include    "symintl.h"
#include    "msgdisp.h"                    /* error message stuff          */
#include    "msgdisp.h"
#include    "xapi.h"
#include    "file.h"
#include    "winini.h"
#include    "nps.h"                     /* scheduler includes           */
#include    "npsres.h"
#include    "npsstb.h"
#include    "nps_hlp.h"
#include    "dbcs.h"

#include    "navbnhdr.h"                /* For nav 3.0 registration     */
#include    "navopts.h"                 /* For nav 3.0 registration     */

#define _STRING_SIZES_ONLY_
#include "siunivsl.h"                   /* For regular registration     */
#include "sirginfo.h"

#include "quakever.h"
#include "vlibver.h"

#include "symcfg.h"                     // For command-line structures
#include "navdoabt.h"


//***************************************************************************
// Local definitions
//***************************************************************************
#undef TRACE
#define TRACE ERR_TRACE_ID (_NPS_C)



//***************************************************************************
// Local typedefs
//***************************************************************************

typedef void (WINAPI *LPFN_STDABOUT)(HWND);


/************************************************************************/
/* Local component prototypes                                           */
/************************************************************************/
int PASCAL WinMain(                     /* WinMain function for NPS     */
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow);

WORD LOCAL PASCAL NpsMainWndInit (      /* Initializes LIST dlg box     */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD LOCAL PASCAL NpsAdjustSysMenu (    /* Adjust SYSMENU due to WM_SIZE*/
    HWND        hDlg,                   /* Handle to dialog box         */
    WPARAM      wParam);                /* wParam of WM_SIZE message    */

WORD LOCAL PASCAL NpsSetDefaultFocus (  /* Set Focus to List or Add     */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD LOCAL PASCAL NpsInitGlobals (      /* Inits global variables       */
    VOID);                              /* Takes no parameters          */

WORD LOCAL PASCAL NpsHide (             /* Do we hide our icon?         */
    HWND        hDlg);                  /* Handle to dialog box         */

LRESULT LOCAL NpsMainWndWM_CTLCOLOR(    // Handles WM_CTLCOLOR for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_CTLCOLOR message
    LPARAM      lParam);                // lParam of WM_CTLCOLOR message

VOID LOCAL NpsMainWndWM_ERASEBKGND(     // Handles WM_ERASEBKGND for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_ERASEBKGND message
    LPARAM      lParam);                // lParam of WM_ERASEBKGND message

VOID LOCAL OnMenuSelect (               // Handles WM_MENUSELECT for main wnd
    HWND        hWnd,                   // [in] Main dialog window handle
    UINT        uIDItem                 // [in] Menu ID of selected item
    );

void LOCAL OnCommand (                  // Process WM_COMMAND
    HWND        hDlg,                   // [in] from this window
    int         nID,                    // [in] control id
    HWND        hwndCtl,                // [in] control hWnd
    UINT        codeNotify              // [in] notification code
    );

WORD NpsOnNotify(                       // Process WM_NOTIFY
    HWND hDlg,                          // [in] Handle of the dialog.
    int  nControlID,                    // [in] Control ID for the notification.
    LPNMHDR lpNMHeader                  // [in] Notification message header.
    );

LRESULT LOCAL OnQueryEndSession (       // Process WM_QUERYENDSESSION
    HWND        hDlg                    // [in] from this dialog box
    );

VOID LOCAL OnContextHelp (              // Respond to WM_CONTEXTMENU and WM_HELP
    HWND        hDlg,                   // [in] dialog that sent the message.
    UINT        uMsg,                   // [in] either WM_HELP or WM_CONTEXTMENU
    WPARAM      wParam,                 // [in] wParam from dialog
    LPARAM      lParam                  // [in] lParam from dialog
    );

BOOL LOCAL NpsGetRegInfo (
    LPSTR       lpUser,
    LPSTR       lpComp
    );

VOID LOCAL DisplayInfoDeskHelp (        // Just as the name sez.
    HWND    hWnd                        // [in] parent window handle
    );


VOID PASCAL NPSParseCommandLine (
    LPSTR lpCommandLine                 // The command line.
    );

void VerifySchedulerIsRunning(          // Verify Scheduler is running.
    HWND hWnd                           // [in] Parent dialog.
    );

int DisplayMessage(                     // Message box using String ID's.
    HWND  hWnd,                         // [in] Parent Window
    DWORD dwMessageID,                  // [in] Message string ID.
    DWORD dwTitleID,                    // [in] Title string ID.
    UINT  uType                         // [in] Type of message box.
    );

VOID NpsInitListView(                   // Initialize our main ListView control.
    HWND hDlg );                        // [IN] Parent dialog.

void NpsResizeListColumns ( HWND );

#ifdef SYM_WIN32
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

static BOOL g_bUseGradientPaintCode = TRUE;
#endif

/*----------------------------------------------------------------------*
 * The flag keeps track of whether or not NPS_MSG_INIT has been         *
 * processed, to prevent WM_SIZE processing from calling init buttons   *
 * before dialog initialization is complete (used to caused invalid     *
 * window error messages in window move ...etc)
 *----------------------------------------------------------------------*/

static BOOL  gbNPSInitialized = FALSE;


                                        // User Registration block for
                                        // holding registration data
REGISTRATION    reginfo =
    {
    "Symantec Install for Windows",     // Search string
    sizeof(REGISTRATION),               // size of structure
    "", 0,                              // will hold user name
    "", 0,                              // will hold company
    FALSE, FALSE,
    QUAKE_VERSION_MAJOR,
    QUAKE_VERSION_MINOR,
    0, 0,
    "", 0
    };


                                        // Command-line flag to run
                                        // minimized.
static BOOL  LaunchMinimized = FALSE;


                                        // Command-line switch to run
                                        // minimized:
CFGTEXTREC  rswNPSMinimized = { "MIN",  CFG_SW_EXACT };

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

/************************************************************************/
/* NPS main routines                                                    */
/************************************************************************/




/************************************************************************/
/* Description:                                                         */
/*      This routine is the main entry point for the Scheduler. It:     */
/*              - registers the window class                            */
/*              - creates the main window (dialog)                      */
/*              - initalizes some strings from the resource             */
/*              - reads the events table from disk                      */
/*              - starts the timer going every 60 seconds               */
/*                                                                      */
/* int PASCAL WinMain (hInstance, -- instance handle                    */
/*                     hPrevInstance, -- previous instance              */
/*                     lpCmdLine, -- Command line                       */
/*                     nCmdShow) -- how to display the window           */
/* Return values:                                                       */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/14/91 MSL Made cleanup pass and broke into subroutines.           */
/* 03/05/91 MSL Doesn't register spin button control since this is now  */
/*              in NWIN.                                                */
/* 06/02/91 MSL No longer needs to start the timer since it will get    */
/*              started by the WM_ENABLE message.                       */
/************************************************************************/
int PASCAL WinMain (
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow)
{
    MSG                 msg;
    HWND                hDlg;
    int                 nRet;
    HACCEL      hAccel;                 // Accelerator Table
    extern      HWND    hDlgModelessActive;
    extern      HWND    hWndScheduler;

                                        /* Perform any initialization.  */
    ghInst = hInstance;

                                        // A temporary handle to the button
                                        // bar so it will be be specifically
                                        // painted if nps is starting for
                                        // the first time this window session.
                                        // (Defect 35799)
    HWND hBBar;

    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
        {
        Ctl3dRegister (ghInst);
        Ctl3dAutoSubclass(ghInst);
        }


#ifdef SYM_WIN32
                                        // Check if we're running on NT 5
                                        // and if so, disable the gradient
                                        // caption bar code, cuz NT already
                                        // does gradients.
OSVERSIONINFO rOSInfo;

    ZeroMemory ( &rOSInfo, sizeof(OSVERSIONINFO) );

    rOSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx ( &rOSInfo );

    if ( VER_PLATFORM_WIN32_NT == rOSInfo.dwPlatformId  &&
         rOSInfo.dwMajorVersion >= 5 )
        {
        g_bUseGradientPaintCode = FALSE;
        }
#endif;

    NG_Init();
    NIntlInit (NULL);

#ifdef SYM_WIN32
                                        // -------------------------------------
                                        // Note that we can't rely on
                                        // hPrevInstance. For some reson it is
                                        // never being set to the previous
                                        // instance and behaves just as Win NT.
                                        // -------------------------------------
    HWND hPrev = FindWindow( szAppName, NULL );

    if ( hPrev )
        {
        NpsPrevInstActivate ();
        nRet = FALSE;
        goto WinMainExit;
        }
    else if ( NpsInitGlobals() || NpsRegisterClass () != NOERR )
        {
        nRet = FALSE;
        goto WinMainExit;
        }

#else
                                        // -------------------------------------
                                        // If a previous instance is not running
                                        // register our window classes.
                                        // -------------------------------------
    if ( hPrevInstance )
        {
        NpsPrevInstActivate ();
        nRet = FALSE;
        goto WinMainExit;
        }
    else if ( NpsInitGlobals() || NpsRegisterClass () != NOERR )
        {
        nRet = FALSE;
        goto WinMainExit;
        }
#endif


                                        // Set command-line flags:
    NPSParseCommandLine ( lpCmdLine );


                                        // Check to see if command-line flag
                                        // was set:
    if ( LaunchMinimized )
        {
        nCmdShow = SW_MINIMIZE;
        }

                                        /* Create the dialog box for    */
                                        /* this application.  Note that */
                                        /* we let a modal dialog box    */
                                        /* serve as our application     */
                                        /* window.                      */
    if (NpsDialogCreate (&hDlg, nCmdShow) != NOERR)
        {
        nRet = FALSE;
        goto WinMainExit;
        }

    hWndScheduler = hDlg;

                                        // Paint the button bar specifically
                                        // at startup; if it isn't done
                                        // here, there's a chance
                                        // the button bar will be invisible
                                        // at startup.
    hBBar = GetDlgItem ( hWndScheduler, IDC_NPS_BUTTONBAR );
    if ( hBBar )
        {
        SYM_ASSERT ( hBBar && GetDesktopWindow() != hBBar );
        InvalidateRect ( hBBar, NULL, FALSE );
        }

                                        // Load Accelerator Table
    hAccel = LoadAccelerators (ghInst,
                               MAKEINTRESOURCE (IDA_NPS_ACCELERATOR_TABLE));

                                        /* application making sure that */
                                        /* the modeless dialog box      */
                                        /* messages get handled.        */
    while (GetMessage (&msg, NULL, NULL, NULL))
        {
        if (!TranslateAccelerator (hWndScheduler, hAccel, &msg))
            {
            if (IsWindow (hDlgModelessActive))
               {
               if (IsDialogMessage (hDlgModelessActive, &msg))
                  {
                  continue;
                  }
               }
            if (!IsDialogMessage (hWndScheduler, &msg))
                {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
                }
            }
        }
    nRet = (int)msg.wParam;

                                        // Clean up and exit to caller.
WinMainExit:
    NG_Done();

    Ctl3dUnregister (ghInst);
    UnregisterClass (szAppName, ghInst);
    return (nRet);
}




/************************************************************************/
/* Description:                                                         */
/*      This routine processes the messages for the scheduler           */
/*      list box.                                                       */
/*                                                                      */
/* NOTE:                                                                */
/*      Because this is a "roll your own" dialog box we are supposed to */
/*      call DefDlgProc() so the controls will be handled correctly.    */
/*      Unfortunately not all of Windows behaviors are desirable when   */
/*      this routine is called in all cases.  Below are the messages    */
/*      that are passed to DefWindowProc() to obtain the desired        */
/*      (and EXPECTED you MS goons) effect:                             */
/*              WM_SYSCOMMAND, WM_ERASEBACKGROUND, WM_CLOSE             */
/*                                                                      */
/* LRESULT SYM_EXPORT CALLBACK MainWndProc(Hdlg, - window handle            */
/*                         message,     - message                       */
/*                         wParam,      - wParam of message             */
/*                         lParam)      - lParam of message             */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/14/91 MSL Began cleanup pass.                                     */
/* 02/15/91 MSL Added 3D appearance and continued clean up.             */
/* 02/28/91 MSL added NPS3D define to toggle 3D effect on and off.      */
/*              Exit button was added.  Responds to WIN.INI change in   */
/*              case internationalization was altered.                  */
/* 03/04/91 MSL Now sets hDlgModelessActive.                            */
/* 03/07/91 MSL Calls NpsUpdateCaption() at the appropriate times.      */
/* 03/18/91 MSL No more minimize or exit buttons.                       */
/* 03/29/91 MSL Now calls ShadowWindow() to give that norton look.      */
/* 05/25/91 MSL Handles new HELP item in system menu.                   */
/* 06/02/91 MSL Timers started and stoped in response to WM_ENABLE      */
/*              messages.  This fixes reentrancy problems when modal    */
/*              dialog boxes are up.                                    */
/* 06/14/93 KDY Added case for NPS_MSG_HIDE.                            */
/************************************************************************/
LRESULT SYM_EXPORT CALLBACK MainWndProc (   /* Main Window Proc for NPS     */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* wParam of message            */
    LPARAM      lParam)                 /* lParam of message            */
{
    extern      HWND    hDlgModelessActive;
    extern      char    szTitle[];
    extern      char    szNortonIni[];
    extern      char    szHide[];
    extern      char    szAutoLoad[];
    extern      char    szIniNamedEvents[];
    extern      char    szAppName[];
    extern      char    szCopyright[];
    extern      char    szVersion[];
    auto    LRESULT     lResult = 0;

    switch (wMessage)
        {
                                        /* WM_CREATE:  For initializing */
                                        /* the dialog box as a main     */
                                        /* window for the application.  */
        case WM_CREATE:
           NpsMainWndInit (hDlg);
           break;

                                        /* WM_ACTIVATE:  When our dialog*/
                                        /* box is being activated or    */
                                        /* deactivated.  We must call   */
                                        /* DefDlgProc() for our caption */
                                        /* to paint properly.           */
        case WM_ACTIVATE:
#ifdef SYM_WIN16
           if (GET_WM_ACTIVATE_STATE(wParam, lParam))
               {
               hDlgModelessActive = hDlg;
               StdHelpSetContext (NPS_HELP_DLG_MAIN, NPS_HELP_DLG_MAIN);
               }
#endif
           NpsUpdateCaption (hDlg);
           return (DefDlgProc(hDlg, wMessage, wParam, lParam));
           break;

                                        // WM_ACTIVATEAPP:  When we are
                                        // being activated as an application,
                                        // we need to set up the help system.
        case WM_ACTIVATEAPP:
            if ((BOOL)wParam)
                {
                StdHelpActivate (hDlg, &HelpInfo);
                }
            break;

                                        /* NPS_MSG_INIT:  Posted during */
                                        /* WM_CREATE such that stored   */
                                        /* events may be loaded AFTER   */
                                        /* all initializtion has        */
                                        /* completed.                   */
        case NPS_MSG_INIT:
            {
            Ctl3dSubclassDlgEx(hDlg, CTL3D_ALL);
            NpsSmallFontSet (hDlg, IDC_NPS_LIST);

            LPNPSOPTIONS lpOptions = (LPNPSOPTIONS)GetWindowLong (
                                                    hDlg,
                                                    GWL_USERDATA
                                                    );
            SYM_ASSERT ( lpOptions );

            if  (
                ((lpOptions->bHide) == FALSE) &&
                (!IsIconic ( hDlg ))
                )
                {
                                        // If we know the dialog is full-size
                                        // and visible, just tell the bar
                                        // directly how to show itself without
                                        // dealing with the spaghetti code
                                        // in the "else" statement.
                ShowHideButtonBar (
                            GetDlgItem ( hDlg, IDC_NPS_BUTTONBAR ),
                            lpOptions->ButtonBar.bShow,
                            lpOptions->ButtonBar.uStyle,
                            TRUE
                            );
                }
                else
                {
                ShowHideButtonBar (
                            GetDlgItem ( hDlg, IDC_NPS_BUTTONBAR ),
                            lpOptions->ButtonBar.bShow,
                            lpOptions->ButtonBar.uStyle,
                            (lpOptions->bHide == FALSE || !IsIconic ( hDlg )) ?
                            FALSE : TRUE
                            );
                }

            if ( lpOptions->bStatusBar == FALSE && !IsIconic(hDlg) )
                {
                ShowHideStatusBar (
                                    GetDlgItem ( hDlg, IDC_NPS_STATUS ),
                                    lpOptions->bStatusBar
                                    );
                }

                                        // -----------------------------------
                                        // Update toolbar with disabled
                                        // buttons. If they are enabled later
                                        // no harm done. This is done to we
                                        // look snappy when loading.
                                        // -----------------------------------

            NpsEventControlsDisable (hDlg);
            UpdateWindow ( hDlg );

            NpsUpdateCaption (hDlg);

                                        // -----------------------------------
                                        // Initialize our main list view
                                        // control.
                                        // -----------------------------------
            NpsInitListView( hDlg );

            NpsEventsLoad (hDlg, TRUE);
            NpsEventControlsDisable (hDlg);

                                        // -----------------------------------
                                        // Display default follow help.
                                        // -----------------------------------
            OnMenuSelect ( hDlg, 0 );

            ShowHideTrayIcon ( hDlg, !lpOptions->bHide );

                                        // -----------------------------------
                                        // Verify that the scheduler is
                                        // running, and offer to start if not
                                        // running.
                                        // -----------------------------------
            VerifySchedulerIsRunning( hDlg );

                                        // -----------------------------------
                                        // Enable WM_SIZE re-initializing buttons
                                        // -----------------------------------
            gbNPSInitialized = TRUE;
            }
            break;
                                        /* WM_SETFOCUS:  Guarantees     */
                                        /* that focus is at a valid     */
                                        /* control and, if not, sets it */
                                        /* to ADD.                      */
        case WM_SETFOCUS:
            NpsSetDefaultFocus (hDlg);
            break;

                                        /* WM_WININICHANGE:  The Win.Ini*/
                                        /* file has changed and that    */
                                        /* could affect our processing  */
                                        /* of dates and times.          */
        case WM_WININICHANGE:
           NIntlInit ((LPSTR)lParam);
           NpsEventsLoad (hDlg, FALSE);
           NpsUpdateCaption (hDlg);
           break;

                                        /* WM_TIMECHANGE:  The time     */
                                        /* has changed.                 */
        case WM_TIMECHANGE:
           NpsEventsLoad (hDlg, FALSE);
           NpsUpdateCaption (hDlg);
           if (IsIconic (hDlg))
               {
               SYM_ASSERT ( hDlg && GetDesktopWindow() != hDlg );
               InvalidateRect (hDlg, NULL, TRUE);
               }
           break;


#ifdef CLOCK_WHEN_ICONIZED
                                        /* WM_PAINTICON:  It is time to */
                                        /* paint our clock icon.        */
        case WM_PAINTICON:
           NpsPaintClock (hDlg);
           break;
#endif

                                        /* WM_SYSCOMMAND:  User  has    */
                                        /* interracted with system menu */
                                        /* so process accordingly.  We  */
                                        /* MUST call DefWindowProc()    */
                                        /* instead of DefDlgProc()!!!   */
        case WM_SYSCOMMAND:
            if ( wParam == SC_MINIMIZE )
                {
                NpsHide (hDlg);
                }
            else if ( wParam == SC_CLOSE )
                {
                OnCommand ( hDlg, IDM_NPS_EVENTCLOSE, 0, 0 );
                }

            return (DefWindowProc (hDlg, wMessage, wParam, lParam));
            break;

                                        // WM_ERASEBKGND:  We need to give
                                        // the main window a 3D look.
        case WM_ERASEBKGND:
            NpsMainWndWM_ERASEBKGND (hDlg, wParam, lParam);
            break;

                                        // WM_CTLCOLOR:  Used to give control
                                        // 3D look.
        case WM_CTLCOLOR:
            return (NpsMainWndWM_CTLCOLOR (hDlg, wParam, lParam));

                                        /* WM_SIZE:  We are being made  */
                                        /* into an icon or restored.    */
                                        /* Adjust our system menu.      */
        case WM_SIZE:

#ifdef SYM_WIN16
                                        //-------------------------------
                                        // This might have caused problems
                                        // for Win16 but in Win95 there's
                                        // no problem with the system
                                        // menu.
                                        //-------------------------------
            NpsAdjustSysMenu (hDlg, wParam);
#endif
            if (wParam == SIZE_RESTORED && gbNPSInitialized)
                {
                NpsEventsLoad (hDlg, FALSE);
                NpsEventControlsDisable (hDlg);
                }

            if ( wParam == SIZE_MINIMIZED && !gbNPSInitialized )
                NpsHide (hDlg);
            break;
                                        /* WM_DESTROY:  Time to nuke    */
                                        /* the application.             */
        case WM_DESTROY:
            {
                                        // -----------------------------------
                                        // Save scheduler options.
                                        // -----------------------------------
            SaveNpsOptions ( hDlg );
            StdHelpShutdown (hDlg);

                                        // -----------------------------------
                                        // Take ourselves out of the tray.
                                        // -----------------------------------
            HideTrayIcon ( hDlg );

                                        // -----------------------------------
                                        // Free memory allocated for options.
                                        // -----------------------------------

            auto    LPNPSOPTIONS    lpOptions;
            lpOptions = (LPNPSOPTIONS)GetWindowLong ( hDlg, GWL_USERDATA );

            if ( lpOptions )
                {
                SetWindowLong ( hDlg, GWL_USERDATA, 0L );
                MemFreePtr ( lpOptions );
                }

            PostQuitMessage(0);
            }
            break;

                                        // NPS_MSG_HIDE: Used by
                                        // NpsHide() to delay the
                                        // hiding of the window.
                                        // NpsHide() was attempting a
                                        // ShowWindow(HIDE), but it
                                        // would hide the dialog box
                                        // instead of the icon that
                                        // is created on our behalf.  By
                                        // delaying the call to Show-
                                        // Window(), we're hiding the
                                        // correct window.
        case NPS_MSG_HIDE:
            ShowWindow(hDlg, SW_HIDE);
            break;

        case WM_SYSCOLORCHANGE:
            NG_ColorChange();
            break;

                                        // -----------------------------------
                                        // WM_MENUSELECT:
                                        // The user has selected a menu item
                                        // and we need to provide quick help.
                                        // -----------------------------------
        case WM_MENUSELECT:
            OnMenuSelect ( hDlg, GET_WM_MENUSELECT_CMD ( wParam, lParam ) );
            break;
                                        /* WM_COMMAND:  User has        */
                                        /* interacted with a control so */
                                        /* we must process accordingly. */
        case WM_COMMAND:
            HANDLE_WM_COMMAND ( hDlg, wParam, lParam, OnCommand );
            break;

        case WM_NOTIFY:
            NpsOnNotify( hDlg, (int)wParam, (LPNMHDR)lParam );
            break;

        case WM_VKEYTOITEM:
            {
            if ( LOWORD(wParam) == VK_DELETE )
                {
                FORWARD_WM_COMMAND ( hDlg, IDM_NPS_EVENTREMOVE, 0,
                                     BN_CLICKED, PostMessage );
                lResult = -2;
                }
            else
                {
                lResult = -1;
                }
            }
            break;

                                        // -----------------------------------
                                        // APP_MESSAGE_TRAY_NOTIFY:  The
                                        // Tray icon for our app wants to
                                        // tell us something.
                                        // -----------------------------------
        case APP_MESSAGE_TRAY_NOTIFY:
            lResult = MainDlgTrayNotify ( hDlg, wParam, lParam );
            break;

        case WM_QUERYENDSESSION:
            lResult = OnQueryEndSession ( hDlg );
            break;

#ifdef SYM_WIN32
                                        // -----------------------------------
                                        // Paint the gradiant main window caption:
                                        // -----------------------------------
        case WM_NCACTIVATE:
        case WM_SETTEXT:
            DefWindowProc(hDlg, wMessage, wParam, lParam);

            if ( g_bUseGradientPaintCode  &&  !IsIconic(hDlg))
                {
                RECT    rDraw;
                BOOL    bActive = (wMessage == WM_NCACTIVATE) ? wParam : (GetActiveWindow() == hDlg) ;
                DrawCaption(hDlg, &rDraw, bActive);

                                    // Post a repaint message:
                if ( (!bActive) && (wMessage == WM_NCACTIVATE) )
                    { PostMessage (hDlg, WM_NCPAINT, 1, 0L); }

                }
            return 1;

        case WM_NCPAINT:
            {
            if ( g_bUseGradientPaintCode  &&  !IsIconic(hDlg) )
                return OnNCPaint(hDlg, wMessage, wParam, lParam);
            else
                return DefWindowProc(hDlg, wMessage, wParam, lParam);
            }
#endif

#ifdef SYM_WIN32
        case WM_HELP :                  // Fall through ...
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu
                                        // or standard context help:
                                        // ----------------------------------
            OnContextHelp ( hDlg, wMessage, wParam, lParam );
            break;
#endif
                                        // -----------------------------------
                                        /* Let the DefDlgProc() handle  */
                                        /* messages we don't know how   */
                                        /* to process.                  */
                                        // -----------------------------------
        default:
            lResult = DefDlgProc ( hDlg, wMessage, wParam, lParam );
            break;
        }

    return ( lResult );
} /* MainWndProc () */

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

LRESULT LOCAL OnQueryEndSession (       // Process WM_QUERYENDSESSION
    HWND    hDlg                        // [in] from this dialog box
    )
{
    auto    LRESULT     lAllowTerminate = TRUE;
#if 0
    const   LPNPSOPTIONS lpOptions = (LPNPSOPTIONS)GetWindowLong (
                                        hDlg,
                                        GWL_USERDATA
                                        );

    const   int         nCount = ListView_GetItemCount (
                                        GetDlgItem ( hDlg, IDC_NPS_LIST )
                                        );
    SYM_ASSERT ( lpOptions );

    if ( lpOptions && nCount > 0 )
        {
        CONFSTATUS Status = AskConfirmation (
                                        hDlg,
                                        CONFIRM_EXIT,
                                        lpOptions
                                        );

        if ( Status == CONFSTATUS_DISALLOW )
            lAllowTerminate = FALSE;
        }
#endif
    return ( lAllowTerminate );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void LOCAL OnCommand (                  // Process WM_COMMAND
    HWND    hDlg,                       // [in] from this window
    int     nID,                        // [in] control id
    HWND    hwndCtl,                    // [in] control hWnd
    UINT    codeNotify                  // [in] notification code
    )
{
    switch ( nID )
        {
        case IDM_NPS_OPTSLOAD:
                                        // All entry into the options
                                        // dialog goes through here.
            NpsOptions ( hDlg );
            SaveNpsOptions ( hDlg );
            break;

                                        /* IDC_NPS_LIST:  Interaction    */
                                        /* with list box demands our    */
                                        /* attention.                   */
        case IDC_NPS_LIST:
           NpsEventListMsg (hDlg, codeNotify );
           break;

                                        /* IDC_NPS_ADD:  User wishes to  */
                                        /* add a new event to the list. */
        case IDM_NPS_EVENTADD:
        case IDC_NPS_ADD:
           NpsEventAddMsg (hDlg);
           break;

                                        /* IDC_NPS_EDIT:  User wishes */
                                        /* to modify an existing event. */
        case IDM_NPS_EVENTEDIT:
        case IDC_NPS_EDIT:
           NpsEventEditMsg (hDlg);
           break;

                                        /* IDC_NPS_COPY:    User wishes  */
                                        /* to copy an existing event,   */
                                        /* creating a new event.        */
        case IDM_NPS_EVENTCOPY:
        case IDC_NPS_COPY:
           NpsEventCopyMsg (hDlg);
           break;

                                        /* IDC_NPS_REMOVE:  User wants   */
                                        /* to remove an existing event. */
        case IDM_NPS_EVENTREMOVE:
        case IDC_NPS_REMOVE:
           NpsEventRemoveMsg (hDlg);
           break;

                                        // IDC_NPS_CLOSE:  Terminate the
                                        // application.
        case IDM_NPS_EVENTCLOSE:
        case IDC_NPS_CLOSE:
            {
            DestroyWindow ( hDlg );
            }
            break;

                                        // ID_TRAY_MENU_OPEN:  The user
                                        // wants us to show ourselves so
                                        // do so.
        case ID_TRAY_MENU_RESTORE:
            {
            ShowWindow ( hDlg, SW_SHOW );
            ShowWindow ( hDlg, SW_RESTORE );
            SetForegroundWindow ( hDlg );

                                        // -----------------------------------
                                        // The button bat updating was being
                                        // done during WM_SIZE message processing.
                                        // This is not valid cause you can end
                                        // up in an endless loop. The only
                                        // reson for updating the button bar
                                        // here is because the user can have
                                        // started the application minimized.
                                        // If that happened the INIT processing
                                        // did not took place.
                                        // -----------------------------------

            LPNPSOPTIONS lpOptions = (LPNPSOPTIONS)GetWindowLong (
                                                    hDlg,
                                                    GWL_USERDATA
                                                    );
            SYM_ASSERT ( lpOptions );

            ShowHideButtonBar (
                            GetDlgItem ( hDlg, IDC_NPS_BUTTONBAR ),
                            lpOptions->ButtonBar.bShow,
                            lpOptions->ButtonBar.uStyle,
                            TRUE
                            );

            NpsEventControlsDisable ( hDlg );

            if ( lpOptions->bStatusBar == FALSE )
                {
                ShowHideStatusBar (
                                    GetDlgItem ( hDlg, IDC_NPS_STATUS ),
                                    lpOptions->bStatusBar
                                    );
                }
            }
            break;


        case IDM_NPS_CONTENTS:
        case IDC_NPS_HELP:

            TCHAR szHelpFileName[SYM_MAX_PATH];
            TCHAR szHelpBinary[SYM_MAX_PATH];

            GetModuleFileName( ghInst, szHelpFileName, SYM_MAX_PATH );
            NameStripFile( szHelpFileName );

            if ( LoadString( ghInst, IDS_NPS_HELPFILE, szHelpBinary, SYM_MAX_PATH ) )
                {
                NameAppendFile( szHelpFileName, szHelpBinary );
                WinHelp( hDlg, szHelpFileName, HELP_FINDER, NULL );
                }

            break;

        case IDM_NPS_HELPABOUT:
            {
            HINSTANCE     hInst;
            LPFN_STDABOUT lpfnAbout;
            TCHAR         szAboutDLLPath [ SYM_MAX_PATH ];

            GetModuleFileName ( ghInst, szAboutDLLPath,
                                sizeof (szAboutDLLPath) );

            NameStripFile ( szAboutDLLPath );
            NameAppendFile ( szAboutDLLPath, "navabout.dll" );

            hInst = LoadLibrary ( szAboutDLLPath );
            
            if ( NULL != hInst )
                {
                lpfnAbout = (LPFN_STDABOUT) GetProcAddress (
                                                hInst, "DoStdAbout" );

                if ( NULL != lpfnAbout )
                    {
                    lpfnAbout ( hDlg );
                    }

                FreeLibrary ( hInst );
                }
            }
            break;
        }
}


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine initializes the LIST window for the Norton         */
/*      Program Scheduler.  It modifies the system menu removing the    */
/*      ability to size and maximize the window.  It also posts a       */
/*      WM_USER message such that any events previously stored may be   */
/*      loaded once all of the controls are in place.                   */
/*                                                                      */
/* VERY IMPORTANT NOTE:                                                 */
/*      If the main window dialog box does NOT have the style           */
/*      WS_MINIMIZEBOX, none of this code works.  Windows will assume   */
/*      that you want a dialog box system menu instead of the           */
/*      application menu we desire.                                     */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/12/91 MSL Function created.                                       */
/* 05/25/91 MSL About & Help strings come from NWRES.                   */
/* 06/03/93 KDY Added ButtonBar initialization.                         */
/* 06/10/93 KDY Added code to set menu checks.                          */
/* 06/14/93 KDY bLoad is now set by a call to WinIniLoadCheck().        */
/* 07/22/93 MSL Hooked up Quake Help System                             */
/************************************************************************/

STATUS LOCAL PASCAL NpsMainWndInit (    /* Initializes LIST dlg box     */
    HWND        hDlg                    /* Handle to dialog box         */
    )
{
    auto        HMENU           hMenu;
    auto        char            szHelpFilePath[SYM_MAX_PATH];
    auto        char            szHelpFile[SYM_MAX_PATH];
    extern      HINSTANCE       ghInst;
    extern      HELPSTRUCT      HelpInfo;
    extern      UINT            uHelpBrowseFileMsg;
    auto        STATUS          Status = ERR;
    auto        LPNPSOPTIONS    lpOptions;

    lpOptions = (LPNPSOPTIONS)MemAllocPtr ( GHND, sizeof(NPSOPTIONS) );

    if ( lpOptions )
        {
        Status = NOERR;

        SetWindowLong ( hDlg, GWL_USERDATA, (long) lpOptions );

                                        /* Remove size and maximize     */
                                        /* menu items.                  */

        hMenu = GetSystemMenu (hDlg, FALSE);
        EnableMenuItem (hMenu,
                        SC_SIZE,
                        MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem (hMenu,
                        SC_MAXIMIZE,
                        MF_BYCOMMAND | MF_GRAYED);


        hMenu = GetSubMenu(GetMenu (hDlg), NPS_EVENT_MENU_POS);

        EnableMenuItem (hMenu, IDM_NPS_EVENTEDIT, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem (hMenu, IDM_NPS_EVENTCOPY, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem (hMenu, IDM_NPS_EVENTREMOVE, MF_BYCOMMAND | MF_GRAYED);

                                        // -----------------------------------
                                        // Load scheduler options.
                                        // -----------------------------------
        LoadNpsOptions ( hDlg );
                                        // -----------------------------------
                                        // Set up help system.
                                        // -----------------------------------
        HelpInfo.hinst = ghInst;
        HelpInfo.hwnd  = hDlg;
                                        // Help file name used to be
                                        // IDS_NPS_HELPFILE before the
                                        // invention of InfoDesk:
        // Come back to this:
        //LoadString (ghInst, IDS_INFODESK, szHelpFile, sizeof (szHelpFile));
        LoadString (ghInst, IDS_NPS_HELPFILE, szHelpFile, sizeof (szHelpFile));

        GetModuleFileName (ghInst, szHelpFilePath, sizeof (szHelpFilePath));
        NameReturnPath (szHelpFilePath, szHelpFilePath);
        NameAppendFile (szHelpFilePath, szHelpFile);
        STRCPY (HelpInfo.helpFileName, szHelpFilePath);
        StdHelpExInit (&HelpInfo);

        uHelpBrowseFileMsg = RegisterWindowMessage (HELPMSGSTRING);

                                        /* Post message to ourselves    */
                                        /* such that we may load any    */
                                        /* previous events once all the */
                                        /* controls are up and running. */
        PostMessage (hDlg, NPS_MSG_INIT, 0, 0L);
        }

    return ( Status );
} /* NpsMainWndInit() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine is called whenever we get a WM_SIZE message.  It   */
/*      makes sure that RESTORE is grayed out if we are normal and that */
/*      MINIMIZE is grayed out if we are iconized.                      */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 03/01/91 MSL Function created.                                       */
/************************************************************************/
WORD LOCAL PASCAL NpsAdjustSysMenu (    /* Adjust SYSMENU due to WM_SIZE*/
    HWND        hDlg,                   /* Handle to dialog box         */
    WPARAM      wParam)                 /* wParam of WM_SIZE message    */
{
    HMENU               hMenu;          /* Handle to system menu        */

                                        /* Obtain handle to system menu */
    hMenu = GetSystemMenu (hDlg, FALSE);

                                        /* If we are being iconized,    */
                                        /* enable restore and gray out  */
                                        /* minimize; otherwise enable   */
                                        /* minimize and gray out        */
                                        /* restore.                     */
    if ((WORD)wParam == SIZEICONIC)
        {
        EnableMenuItem (hMenu, SC_RESTORE, MF_ENABLED | MF_BYCOMMAND);
        EnableMenuItem (hMenu, SC_MINIMIZE, MF_GRAYED | MF_BYCOMMAND);
        }
    else if ((WORD)wParam == SIZENORMAL)
        {
        EnableMenuItem (hMenu, SC_RESTORE, MF_GRAYED | MF_BYCOMMAND);
        EnableMenuItem (hMenu, SC_MINIMIZE, MF_ENABLED | MF_BYCOMMAND);
        }
                                        /* Return to caller.            */
    return (NOERR);
} /* NpsAdjustSysMenu () */



/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine makes sure we are hidden if we need to be when we  */
/*      are an icon.                                                    */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 03/18/91 MSL Function created.                                       */
/* 05/16/91 MSL Cleaned up to check state of IDC_NPS_HIDE checkbox.      */
/* 06/05/91 MSL If icon is not to be hidden we must allow Mr. Gates to  */
/*              park it where it wants to if it was hidden before.      */
/* 06/14/93 KDY Changed code to only hide the app if bHide == TRUE.     */
/*              It's up to NpsPrevInstance() to 'show' if necessary.    */
/* 06/07/95 MSL Changed yet again for Win95.  Don't hide unless there   */
/*              is a tray or a reason to do so.                         */
/************************************************************************/

STATUS LOCAL PASCAL NpsHide (           /* Do we hide our icon?         */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    SYM_ASSERT ( hDlg );

    ShowWindow (hDlg, SW_MINIMIZE);

    return (NOERR);
} /* NpsHide() */



/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine checks to see if any events are selected.  If not  */
/*      we disable our event controls, if so we enable them.  If the    */
/*      controls are disabled focus is set to our Add button.           */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/12/91 MSL Function created.                                       */
/************************************************************************/
WORD PASCAL NpsEventControlsDisable (   /* Disables event ctls          */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    HWND        hWndButtonBar;          // Handle to button bar
    int         iLVCount;                /* Number of items in our list view*/
    WORD        wBaseID;
    HMENU       hMenu;
                                        /* Find out if we have a list   */
                                        /* box selection.               */
    hWndButtonBar = GetDlgItem (hDlg, IDC_NPS_BUTTONBAR);
    iLVCount = ListView_GetItemCount (GetDlgItem (hDlg, IDC_NPS_LIST) );
    wBaseID = ButtonBar_GetBase (hWndButtonBar, 0);

    hMenu = GetSubMenu(GetMenu (hDlg), 0);

    if (iLVCount <= 0)
        {
        EnableMenuItem (hMenu, IDM_NPS_EVENTEDIT, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem (hMenu, IDM_NPS_EVENTCOPY, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem (hMenu, IDM_NPS_EVENTREMOVE, MF_BYCOMMAND | MF_GRAYED);
        ButtonBar_EnableButton (hWndButtonBar, FALSE,
                                IDC_NPS_EDIT - ID_NPS_RANGE_FIRST);
        ButtonBar_EnableButton (hWndButtonBar, FALSE,
                                IDC_NPS_COPY - ID_NPS_RANGE_FIRST);
        ButtonBar_EnableButton (hWndButtonBar, FALSE,
                                IDC_NPS_REMOVE - ID_NPS_RANGE_FIRST);
        NpsSetDefaultFocus(hDlg);
        }
    else
        {
        EnableMenuItem (hMenu, IDM_NPS_EVENTEDIT, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem (hMenu, IDM_NPS_EVENTCOPY, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem (hMenu, IDM_NPS_EVENTREMOVE, MF_BYCOMMAND | MF_ENABLED);
        ButtonBar_EnableButton (hWndButtonBar, TRUE,
                                IDC_NPS_EDIT - ID_NPS_RANGE_FIRST);
        ButtonBar_EnableButton (hWndButtonBar, TRUE,
                                IDC_NPS_COPY - ID_NPS_RANGE_FIRST);
        ButtonBar_EnableButton (hWndButtonBar, TRUE,
                                IDC_NPS_REMOVE - ID_NPS_RANGE_FIRST);
        }
    return (NOERR);
} /* NpsEventControlsDisable() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine sets focus to the List Box if there are items in   */
/*      the list box and to the Button Bar if there are no items in the */
/*      list box.  Because Windows is stupid, we have to reset our own  */
/*      default push button.                                            */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/12/91 MSL Function created.                                       */
/* 03/01/91 MSL We must update our default push button ourselves.       */
/* 05/02/91 MSL SetFocus() calls changed to sending WM_NEXTDLGCTL msg.  */
/* 06/07/93 KDY Changed IDC_NPS_ADD to IDC_NPS_BUTTONBAR.                 */
/************************************************************************/
WORD LOCAL PASCAL NpsSetDefaultFocus (  /* Set Focus to List or Add     */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    WORD        wListCount;             /* Number of items in list box  */

                                        /* Set focus to either the list */
                                        /* box or the button bar.       */
    wListCount = ListView_GetItemCount (GetDlgItem (hDlg, IDC_NPS_LIST));
    if (wListCount == 0)
        {
        PostMessage (hDlg,
                     WM_NEXTDLGCTL,
                     (WPARAM)GetDlgItem (hDlg, IDC_NPS_BUTTONBAR),
                     MAKELPARAM (1, 0));
        }
    else if (!IsWindow (GetFocus()))
        {
        PostMessage (hDlg,
                     WM_NEXTDLGCTL,
                     (WPARAM)GetDlgItem (hDlg, IDC_NPS_BUTTONBAR),
                     MAKELPARAM (1, 0));
        }
    else if (!IsWindowEnabled (GetFocus()))
        {
        PostMessage (hDlg,
                     WM_NEXTDLGCTL,
                     (WPARAM)GetDlgItem (hDlg, IDC_NPS_BUTTONBAR),
                     MAKELPARAM (1, 0));
        }
    else
        {
        PostMessage (hDlg,
                     WM_NEXTDLGCTL,
                     (WPARAM)GetDlgItem (hDlg, IDC_NPS_LIST),
                     MAKELPARAM (1, 0));
        }
    return (NOERR);
} /* NpsSetDefaultFocus() */

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine initializes all globals for the Norton Program     */
/*      scheduler application.                                          */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything worked.           */
/*      IDS_NPS_STRING_CANT_LOAD            -- Error loading string         */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 03/05/91 MSL Resources now in NWRES.                                 */
/* 03/29/91 MSL Now inits szBinFile.                                    */
/* 04/15/91 MSL Now inits copyright.                                    */
/* 04/22/91 MSL Now inits file and entry headers.                       */
/* 06/14/93 KDY Removed references to szAutoLoad.                       */
/************************************************************************/
WORD LOCAL PASCAL NpsInitGlobals (      /* Inits global variables       */
    VOID)                               /* Takes no parameters          */
{
    WORD                        wRet;
    WORD                        wIndex;
    extern      BOOL            b24Hour;
    extern      BOOL            b24HourOld;
    extern      char            months[NMONTH][SSIZE];
    extern      char            freqs[NFREQ][SSIZE];
    extern      char            days[NDAY][SSIZE];
    extern      char            th[NORD][SSIZE];
    extern      char            szTitle[];
    extern      char            szNortonIni[];
    extern      char            szHide[];
    extern      char            szButtonStyle[];
    extern      char            szStatusBar[];
    extern      char            szBinFile[];
    extern      char            szBinBakFile[];
    extern      char            szCopyright[];
    extern      char            szVersion[];
    extern      char            szHeaderFile[];
    extern      char            szHeaderEntry[];
    extern      char            szEventMessage[];
    extern      char            szEventProgram[];

                                        /* Initialization               */
    wRet = ERR;
                                        /* Load our title               */
    if (!LoadString (ghInst, IDS_NPS_TITLE, szTitle, ESIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        /* Load our copyright           */
    if (!LoadString (ghInst, IDS_NPS_COPYRIGHT, szCopyright, ESIZE))
        {
        goto NpsInitGlobalsExit;
        }

    if (!LoadString (ghInst, IDS_NPS_VERSION, szVersion, ESIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        /* Load our file header         */
    if (!LoadString (ghInst,
                     IDS_NPS_HEADER_FILE,
                     szHeaderFile,
                     NPS_FILE_HDR_SIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        /* Load our entry header        */
    if (!LoadString (ghInst,
                     IDS_NPS_HEADER_ENTRY,
                     szHeaderEntry,
                     NPS_ENTRY_HDR_SIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        /* Load up the months           */
    for (wIndex = 0; wIndex < NMONTH; wIndex++)
        {
        if (!LoadString (ghInst, IDS_NPS_JAN+wIndex, months[wIndex], SSIZE))
            {
            goto NpsInitGlobalsExit;
            }
        }

                                        /* Load up the frequencies      */
    for (wIndex = 0; wIndex < NFREQ; wIndex++)
        {
        if (!LoadString (ghInst,
                         IDS_NPS_FREQ_ONETIME+wIndex,
                         freqs[wIndex],
                         SSIZE))
            {
            goto NpsInitGlobalsExit;
            }
        }

                                        /* Load days of the week        */
    for (wIndex = 0; wIndex < NDAY; wIndex++)
        {
        if (!LoadString (ghInst,
                         IDS_NPS_SUN+wIndex,
                         days[wIndex],
                         SSIZE))
            {
            goto NpsInitGlobalsExit;
            }
        }

                                        /* Load 'st', 'nd', 'th' endings*/
    for (wIndex = 0; wIndex < NORD; wIndex++)
        {
        if (!LoadString (ghInst, IDS_NPS_ST+wIndex, th[wIndex], SSIZE))
            {
            goto NpsInitGlobalsExit;
            }
        }

                                        /* Load up the runstyles.       */
    for (wIndex = 0; wIndex < NRUNSTYLE; wIndex++)
        {
        if (!LoadString (ghInst, IDS_NPS_RUNTEXT_NORMAL+wIndex,
                         runstyle[wIndex], SSIZE))
            {
            goto NpsInitGlobalsExit;
            }
        }

                                        /* Load "Norton.Ini"            */
    if (!LoadString (ghInst, IDS_NPS_NORTON_INI, szNortonIni, SYM_MAX_PATH))
        {
        goto NpsInitGlobalsExit;
        }
                                        /* Load our bin file name       */
    if (!LoadString (ghInst, IDS_NPS_BIN_FILE, szBinFile, SYM_MAX_PATH))
        {
        goto NpsInitGlobalsExit;
        }
                                        /* Load our bin backup file name*/
    if (!LoadString (ghInst,
                     IDS_NPS_BIN_BAK_FILE,
                     szBinBakFile,
                     SYM_MAX_PATH))
        {
        goto NpsInitGlobalsExit;
        }
                                        /* Load "Hide="                 */
    if (!LoadString (ghInst, IDS_NPS_INI_HIDE, szHide, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        /* Load "TitleTime="                 */
    if (!LoadString (ghInst, IDS_NPS_INI_TITLETIME, szTitleTime, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "NAMEDEVENTS", but the
                                        //   'GetPrivateProfileString()' call
                                        //   in 'NpsNamedEventsLoad()' in
                                        //   NPSADEDT.CPP to retrieve a
                                        //   string from the seciton,
                                        //   [NamedEvents] in SCHEDULE.INI
                                        //   is case-insensitive. The section
                                        //   name was all capitalized so that
                                        //   a localizer will not translate
                                        //   the section name, which will
                                        //   prevent the section name from
                                        //   mis-matching.
    if (!LoadString (ghInst, IDS_NPS_INI_SECTION_NAMED,
                     szIniNamedEvents, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "ButtonStyle="
    if (!LoadString (ghInst, IDS_NPS_INI_BUTTONSTYLE, szButtonStyle, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "ButtonShow="
    if (!LoadString (ghInst, IDS_NPS_INI_BUTTONSHOW, szButtonShow, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "StatusBar="
    if (!LoadString (ghInst, IDS_NPS_INI_STATUS, szStatusBar, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "ConfirmExit="
    if (!LoadString (ghInst, IDS_NPS_INI_CONFIRMEXIT, szConfirmExit, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "ConfirmDelete="
    if (!LoadString (ghInst, IDS_NPS_INI_CONFIRMDELETE, szConfirmDelete, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "ConfirmSave="
    if (!LoadString (ghInst, IDS_NPS_INI_CONFIRMSAVE, szConfirmSave, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "ConfirmEdit="
    if (!LoadString (ghInst, IDS_NPS_INI_CONFIRMEDIT, szConfirmEdit, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "Name="
    if (!LoadString (ghInst, IDS_NPS_INI_KEYWORD_NAME, szIniName, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "Prompt="
    if (!LoadString (ghInst, IDS_NPS_INI_KEYWORD_PROMPT, szIniPrompt, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "CommandLine="
    if (!LoadString (ghInst, IDS_NPS_INI_KEYWORD_CMDLINE, szIniCmdLine, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "StartupDir="
    if (!LoadString (ghInst, IDS_NPS_INI_KEYWORD_STARTUP, szIniStartupDir, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "RunStyle="
    if (!LoadString (ghInst, IDS_NPS_INI_KEYWORD_RUNSTYL, szIniRunStyle, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "DefaultDesc="
    if (!LoadString (ghInst, IDS_NPS_INI_DEF_DESC, szIniDefDesc, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "DefaultActionText="
    if (!LoadString (ghInst, IDS_NPS_INI_DEF_ACTION_TEXT, szIniDefActionText, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "AllowBlankActionText="
    if (!LoadString (ghInst, IDS_NPS_INI_ALLOW_BLANK_ACTION_TXT, szIniAllowBlankActionText, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
                                        // Load "BlankActionTextMessage="
    if (!LoadString (ghInst, IDS_NPS_INI_BLANK_ACTION_TXT_MSG, szIniBlankActionTextMessage, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

    if (!LoadString (ghInst, IDS_NPS_EVENTMESSAGE, szEventMessage, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }
    if (!LoadString (ghInst, IDS_NPS_EVENTPROGRAM, szEventProgram, SSIZE))
        {
        goto NpsInitGlobalsExit;
        }

                                        // We are successful if we go here.
    wRet = NOERR;
                                        /* Report error and exit        */
NpsInitGlobalsExit:
    if (wRet != NOERR)
        {
        wRet = NPS_STRING_CANT_LOAD;
        NMsgBox (ghInst, NULL, wRet, TRACE, NULL, -1);
        }
    return (wRet);
} /* NpsInitGlobals() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine is called when ever we want to get the             */
/*      internationalization stuff for the time.  It sets               */
/*      up global variables that are used throughout the scheduler      */
/*      for format information.                                         */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Function completed.          */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/28/91 MSL Made cleanup pass.  Now looks at iLzero to see if date  */
/*              needs a leading zero and sets up szDateFmt.             */
/* 03/01/91 MSL No longer a local function.                             */
/************************************************************************/
WORD PASCAL NpsInitTime (               /* Inits international time info*/
    VOID)                               /* Takes no parameters          */
{
    char                szBuf[ESIZE];   /* temp buffer                  */
    extern      BOOL    b24Hour;
    extern      char    szTimeSep[SSIZE];
    extern      char    szHourFmt[SSIZE];
    extern      char    ampm[NAM][SSIZE];
                                        /* 12 or 24 hour clock?         */
    GetProfileString ("Intl", "iTime", "0", szBuf, 2);
    b24Hour = atoi((PSTR)szBuf);

                                        /* Get time seperator           */
    GetProfileString ("Intl", "sTime", ":", szTimeSep, 2);

                                        /* Get AM string                */
    GetProfileString ("Intl", "s1159", "AM", ampm[0], 5);

                                        /* Get PM string                */
    GetProfileString ("Intl", "s2359", "PM", ampm[1], 5);

                                        /* Get leading zero info for    */
                                        /* hours fields.                */
    GetProfileString ("Intl", "iTLZero", "0", szBuf, 2);
    if (szBuf[0] == '0')
        {
        STRCPY (szHourFmt, NPS_FORMAT_HR);
        }
    else
        {
        STRCPY (szHourFmt, NPS_FORMAT_HR0);
        }

    return (NOERR);
} /* NpsInitTime() */

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine updates the caption bar of the Program Scheduler   */
/*      with the application name followed by the current time.         */
/*      If we are iconic, only use the time; if we are normal size,     */
/*      use the long date and the time.                                 */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went okay.        */
/*                                                                      */
/************************************************************************/
/* 03/06/91 MSL Function created.                                       */
/* 03/07/91 MSL Different caption if iconic or normal size.             */
/* 05/11/93 MSL Same caption if iconic or normal size (well, the date   */
/*              is in short format instead...)                          */
/************************************************************************/
WORD WINAPI NpsUpdateCaption (          /* Updates the clock in caption */
    HWND        hDlg                    /* Handle to main dialog window */
    )
{
#ifdef SYM_WIN32
    return NOERR;
#else
    struct      tm              *curtm;
    struct      _timeb          curtimeb;
    char                        szCaption[ESIZE];
    char                        szFmtBuf[ESIZE];
    char                        szTimeBuf[SSIZE];
    char                        szDateBuf[ESIZE];
    extern      char            szTitle[];
    extern      BOOL            b24Hour;
    const       LPNPSOPTIONS    lpOptions = (LPNPSOPTIONS)GetWindowLong (
                                                    hDlg,
                                                    GWL_USERDATA
                                                    );
    SYM_ASSERT ( lpOptions );

                                        /* Should we even be here?      */
    if (!IsWindowVisible (hDlg) && IsIconic (hDlg))
        {
        SetWindowText (hDlg, "");
        goto NpsUpdateCaptionExit;
        }

    char szText[128];

                                        //-------------------------------
                                        // If Icon time is disabled, just
                                        // draw program title
                                        //-------------------------------
    if ( IsIconic(hDlg) )
        {
        GetWindowText (hDlg, szText, 127);
                                        //-------------------------------
                                        // Display icon title ONCE to prevent
                                        // waking up screen savers
                                        //-------------------------------
        if (STRCMP(szText, szTitle))
            SetWindowText (hDlg, szTitle);
        goto NpsUpdateCaptionExit;
        }

                                        //-------------------------------
                                        // Get outa here if we shouldn't
                                        // display time in caption.
                                        //-------------------------------

    if ( lpOptions->bShowTitleTime == FALSE )
        {
        GetWindowText (hDlg, szText, 127);
        LoadString ( ghInst, IDS_NPS_TITLE, szCaption, ESIZE );
                                        //-------------------------------
                                        // To prevent flicker with the
                                        // gradiant caption bars, only
                                        // set the window text if it's different:
                                        //-------------------------------
        if (STRCMP(szText, szCaption))
            {
            SetWindowText ( hDlg, szCaption );
            }
        goto NpsUpdateCaptionExit;
        }

                                        /* Find out the current time    */
    _ftime (&curtimeb);
    curtm = localtime (&curtimeb.time);

                                        /* Build time string            */
    NSetIntlTime (szTimeBuf,
                  curtm->tm_hour,
                  curtm->tm_min,
                  0,
                  FALSE,
                  !b24Hour);

                                        /* Build date string based on   */
                                        /* whether or not we are an     */
                                        /* icon.                        */
    if (!IsIconic (hDlg))
        {
        LoadString (ghInst, IDS_NPS_CAPTION, szFmtBuf, ESIZE);
        NSetIntlLongDate (szDateBuf,
                          curtm->tm_mon + 1,
                          curtm->tm_mday,
                          curtm->tm_year,
                          FALSE);
        wsprintf (szCaption,
                  szFmtBuf,
                  (LPSTR)szTitle,
                  (LPSTR)szDateBuf,
                  (LPSTR)szTimeBuf);
        }
    else
        {
        LoadString (ghInst, IDS_NPS_CAPTION_ICONIZED, szFmtBuf, ESIZE);
        NSetIntlDate (szDateBuf,
                      curtm->tm_mon + 1,
                      curtm->tm_mday,
                      curtm->tm_year );
        wsprintf (szCaption,
                  szFmtBuf,
                  (LPSTR)szDateBuf,
                  (LPSTR)szTimeBuf);
        }

                                        /* Display our caption          */

    GetWindowText (hDlg, szText, 127);
                                        //-------------------------------
                                        // To prevent flicker with the
                                        // gradiant caption bars, only
                                        // set the window text if it's different:
                                        //-------------------------------
    if (STRCMP(szText, szCaption))
        {
        SetWindowText (hDlg, szCaption);
        }


                                        /* Return to caller             */
NpsUpdateCaptionExit:
    return (NOERR);
#endif  // ndef SYM_WIN32
} /* NpsUpdateCaption() */



//************************************************************************
// NpsMainWndWM_CTLCOLOR()
//
// This routine handles the WM_CTLCOLOR message for the main dialog window
// of the Norton AntiVirus for Windows.  The routine must return either a
// brush or the value from DefWindowProc() in order for the CTL3D.DLL 3D
// control Dynamic Link Library to give our controls a nice 3D look.  This
// code is taken directly from the CTL3D.DOC documentation.
//
// Parameters:
//      hWnd            [in] Window handle of main dialog window
//      wParam          [in] wParam of WM_CTLCOLOR message
//      lParam          [in] lParam of WM_CTLCOLOR message
//
// Returns:
//      HBRUSH          if Ctl3dCtlColorEx() returns a valid brush
//      DefWindowProc() if Ctl3dCtlColorEx() could not return a valid brush
//************************************************************************
// 03/14/93 MSL Function created.
//************************************************************************
LRESULT LOCAL NpsMainWndWM_CTLCOLOR(    // Handles WM_CTLCOLOR for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_CTLCOLOR message
    LPARAM      lParam)                 // lParam of WM_CTLCOLOR message
{
    LRESULT     lResult;

    HBRUSH      hBrush;

                                        // See if CTL3D.DLL has a brush
                                        // for our control to use.
    hBrush = Ctl3dCtlColorEx(WM_CTLCOLOR, wParam, lParam);
    if (hBrush != (HBRUSH)FALSE)
        {
        lResult = MAKELRESULT (hBrush, 0);
        }
    else
        {

        lResult = DefWindowProc(hWnd, WM_CTLCOLOR, wParam, lParam);
        }
                                        // Return to caller
    return (lResult);
} // NpsMainWndWM_CTLCOLOR()



//************************************************************************
// NpsMainWndWM_ERASEBKGND()
//
// This routine handles the WM_ERASEBKGND message for the main dialog window
// of the Norton AntiVirus for Windows.
//
//
// Parameters:
//      hWnd            [in] Window handle of main dialog window
//      wParam          [in] wParam of WM_ERASEBKGND message
//      lParam          [in] lParam of WM_ERASEBKGND message
//
// Returns:
//      nothing
//************************************************************************
// 03/14/93 MSL Function created.
//************************************************************************
VOID LOCAL NpsMainWndWM_ERASEBKGND(     // Handles WM_ERASEBKGND for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_ERASEBKGND message
    LPARAM      lParam)                 // lParam of WM_ERASEBKGND message
{
    HDC         hDC;
    RECT        rect;


    hDC = (HDC)wParam;

                                        // Paint the background for the
                                        // main window.
    GetClientRect (hWnd, &rect);
    NG_DrawCanvas (hWnd, hDC, &rect, 0);

                                        // Return to caller.
} // NpsMainWndWM_ERASEBKGND()



//************************************************************************
// NpsSmallFontSet()
//
// This routine sets the font of the specified child control to a small
// font.
//
// Parameters:
//      hWnd                    [in] Handle of window owning child
//      wChildID                [in] ID of child control
//
// Returns:
//      nothing
//************************************************************************
// 01/27/93 MSL Function created.
//************************************************************************

VOID WINAPI NpsSmallFontSet (           // Specified control to small font
    HWND        hWnd,                   // Window that owns child
    WORD        wChildID)               // Child control id
{
#ifndef SYM_WIN32
    HFONT       hSmallFont;             // Thin font for status line
    HWND        hWndChild;              // Child Control ID

    if (IsWindow (hWndChild = GetDlgItem (hWnd, wChildID)))
        {
                                        // Set the font for our status field.
        if (NG_FontGet (hWndChild, &hSmallFont, FW_NORMAL, 0) == NOERR)
            {
            SetWindowFont (hWndChild, hSmallFont, FALSE);
            }
        }
#endif
} // NpsSmallFontSet()


//************************************************************************
// NpsGetReginfo()
//
// This function handles getting the user registration information either
// from the local structure or from the NAV registration file.
//
// Parameters:
//      LPSTR - Pointer to the buffer to hold user name
//      LPSTR - Pointer to the buffer to hold the user company
//
// Returns:
//      TRUE if sucessful,  FALSE if not
//************************************************************************
// 10/18/93 BLH Function created.
//************************************************************************

BOOL LOCAL NpsGetRegInfo(LPSTR lpUser, LPSTR lpComp)
{
    const   BYTE            FLIPBITS = 0xFF;
    auto    int             nCtr;
    auto    char            szBinPath[SYM_MAX_PATH];
    auto    BOOL            bRet = TRUE;
    auto    HFILE           hBinFile;
    auto    USEROPT         NavUserOptions;
    auto    REGISTRATION    TempReg;
    auto    LPREGISTRATION  lpReg = &TempReg;

    MEMSET(lpReg, 0, sizeof(REGISTRATION));
    MEMSET(&NavUserOptions, 0, sizeof(NavUserOptions));

    *lpUser = 0;
    *lpComp = 0;

                                        // See if our local registration
                                        // block is filled out

    if (reginfo.nRegNameLen)
        {
        MEMCPY(lpReg, &reginfo, sizeof(REGISTRATION));

        if (reginfo.bFlipped)
            {
            nCtr = 0;

            while(nCtr < reginfo.nRegNameLen)
                lpReg->szRegName[nCtr++] = FLIPBITS - reginfo.szRegName[nCtr];

            nCtr = 0;

            while(nCtr < reginfo.nRegCompanyLen)
                lpReg->szRegCompany[nCtr++] = FLIPBITS - reginfo.szRegCompany[nCtr];
            }
        STRNCPY(lpUser, lpReg->szRegName, lpReg->nRegNameLen);
        STRNCPY(lpComp, lpReg->szRegCompany, lpReg->nRegCompanyLen);
        }
    else                                // Else try to get the info from NAV
        {
        GetModuleFileName(ghInst, szBinPath, sizeof (szBinPath));

        NameReturnPath (szBinPath, szBinPath);
        NameAppendFile (szBinPath, "NAVOPTS.DAT");

        if (hBinFile = FileOpen (szBinPath, OF_READ))
            {
            FileSeek (hBinFile,
                      sizeof (NAVBINHEADER),
                      SEEK_FROM_BEGINNING);
            if (FileRead (hBinFile, &NavUserOptions,
                          sizeof (NavUserOptions)))
                {
                STRCPY(lpUser, NavUserOptions.szName);
                STRCPY(lpComp, NavUserOptions.szCompany);
                OemToAnsi ( lpUser, lpUser );
                OemToAnsi ( lpComp, lpComp );
                }
            FileClose (hBinFile);
            }
        }

    if (!*lpUser)
        bRet = FALSE;

    return(bRet);
}

/* **************************************************************************
@Name:
VOID LOCAL OnMenuSelect (               // Handles WM_MENUSELECT for main wnd
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

VOID LOCAL OnMenuSelect (               // Handles WM_MENUSELECT for main wnd
    HWND    hWnd,                       // [in] Main dialog window handle
    UINT    uIDItem                     // [in] Menu ID of selected item
    )
{
    static struct tagMENUTOHELP
        {
        WORD        wMenuId;
        WORD        wHelpId;
        WORD        wStatusId;
        } MenuToHelp[] =
            {
                {
                IDM_NPS_EVENTADD,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_ADD
                },

                {
                IDM_NPS_EVENTEDIT,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_EDIT
                },

                {
                IDM_NPS_EVENTCOPY,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_COPY
                },

                {
                IDM_NPS_EVENTREMOVE,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_DELETE
                },

                {
                IDM_NPS_EVENTCLOSE,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_EXIT
                },

                {
                IDM_NPS_OPTSLOAD,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_OPTIONS
                },

                {
                IDM_NPS_CONTENTS,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_DEFAULT
                },

                {
                IDM_NPS_HELPABOUT,
                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                IDS_FOLLOWHELP_VERSION
                },

                {
                0,
                0,
                0
                }
            };

    auto    char    szStatusText [ SYM_MAX_PATH ];

                                        // ------------------------------------
                                        // uIDItem == 0 if the user pressed the
                                        // ESC key. Put up default message.
                                        // ------------------------------------
    if ( !uIDItem )
        {
        LoadString ( ghInst, IDS_FOLLOWHELP_DEFAULT, szStatusText,
                     sizeof(szStatusText));

        StatusBar_SetText ( GetDlgItem ( hWnd, IDC_NPS_STATUS ), szStatusText );

                                        // ------------------------------------
                                        // Set up for F1 help on a menu
                                        // item.
                                        // ------------------------------------

        StdHelpSetContext ( IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN,
                            IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN );
        }
    else
        {
        for ( UINT uIndex = 0; MenuToHelp[uIndex].wMenuId; uIndex++ )
            {
            if ( MenuToHelp[uIndex].wMenuId == uIDItem )
                {
                                        // ------------------------------------
                                        // We found a match.  Load the
                                        // quick help text and stuff it
                                        // into the status line.
                                        // ------------------------------------

                LoadString ( ghInst, MenuToHelp[uIndex].wStatusId, szStatusText,
                             sizeof(szStatusText));

                StatusBar_SetText ( GetDlgItem ( hWnd, IDC_NPS_STATUS ), szStatusText );

                                        // ------------------------------------
                                        // Set up for F1 help on a menu
                                        // item.
                                        // ------------------------------------

                StdHelpSetContext (
                                MenuToHelp[uIndex].wHelpId,
                                IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN
                                );
                break;
                }
            }
        }
}

/* **************************************************************************
@Name:
VOID WINAPI SaveLoadNpsOptions (        // Either saves or loads our options
    HWND    hDlg,                       // [in] required for Quack functions
    BOOL    bLoad                       // [in] TRUE to load options
    )

@Description:
This function loads and saves the scheduler options. Preferably use
SaveNpsOptions() or LoadNpsOptions() macro to access this function.

@Parameters:
$hWnd$            [in] Window handle of main dialog window
$bLoad$           [in] TRUE to load options.

@Returns:
      Nothing

************************************************************************** */

VOID WINAPI SaveLoadNpsOptions (        // Either saves or loads our options
    HWND    hDlg,                       // [in] required for Quack functions
    BOOL    bLoad                       // [in] TRUE to load options
    )
{
    auto    LPNPSOPTIONS    lpOptions;
    auto    BOOL            bUseWinIni;



    SYM_ASSERT ( hDlg );
    lpOptions = (LPNPSOPTIONS)GetWindowLong ( hDlg, GWL_USERDATA );
    SYM_ASSERT ( lpOptions );

    bUseWinIni = (SystemGetWindowsType() != SYM_SYSTEM_WIN_95);
    if ( lpOptions && bLoad == TRUE )
        {
        MEMSET ( lpOptions, EOS, sizeof(NPSOPTIONS) );

        lpOptions->ButtonBar.uStyle = (UINT) GetPrivateProfileInt (
                                                    szTitle,
                                                    szButtonStyle,
                                                    NPS_BBARSTYLE_DEFAULT,
                                                    szNortonIni
                                                    );

        lpOptions->ButtonBar.bShow = (BOOL) GetPrivateProfileInt (
                                                    szTitle,
                                                    szButtonShow,
                                                    TRUE,
                                                    szNortonIni
                                                    );

        lpOptions->bHide = (BOOL) GetPrivateProfileInt (
                                                    szTitle,
                                                    szHide,
                                                    FALSE,
                                                    szNortonIni
                                                    );

        lpOptions->bAutorunMissedEvents = GetPrivateProfileInt (szTitle,
                                                    szTitleTime,
                                                    FALSE,
                                                    szNortonIni
                                                    );

        lpOptions->bStatusBar = GetPrivateProfileInt (
                                                    szTitle,
                                                    szStatusBar,
                                                    TRUE,
                                                    szNortonIni
                                                    );

        lpOptions->Confirm.bOnExit = (BOOL) GetPrivateProfileInt (
                                                    szTitle,
                                                    szConfirmExit,
                                                    TRUE,
                                                    szNortonIni
                                                    );

        lpOptions->Confirm.bOnDelete = (BOOL) GetPrivateProfileInt (
                                                    szTitle,
                                                    szConfirmDelete,
                                                    TRUE,
                                                    szNortonIni
                                                    );

        lpOptions->Confirm.bOnSave = (BOOL) GetPrivateProfileInt (
                                                    szTitle,
                                                    szConfirmSave,
                                                    TRUE,
                                                    szNortonIni
                                                    );

        lpOptions->Confirm.bOnEdit = (BOOL) GetPrivateProfileInt (
                                                    szTitle,
                                                    szConfirmEdit,
                                                    TRUE,
                                                    szNortonIni
                                                    );
#ifdef SYM_WIN32
        if (!bUseWinIni)
            {
            char    szCmdLine [ SYM_MAX_PATH + 1 ] = { 0 };
            char    szName    [ SYM_MAX_PATH + 1 ] = { 0 };

            GetModuleFileName ( ghInst, szCmdLine, sizeof(szCmdLine) );

            LoadString ( ghInst, IDS_NPS_TITLE, szName, sizeof(szName) );

            BOOL bExists = ShellLinkExists (
                                    NULL,       // NULL for startup folder
                                    szCmdLine,  // Required
                                    NULL        // Optional
                                    );

            if ( bExists == FALSE )
                bExists = ShellLinkExists (
                                    NULL,       // NULL for startup folder
                                    szCmdLine,  // Required
                                    szName      // Optional
                                    );

            if ( bExists == TRUE )
                lpOptions->bLoad = TRUE;
            else
                lpOptions->bLoad = FALSE;
            }
        else
#endif
            {
            WinIniLoadCheck( NULL, ghInst, NULL, &lpOptions->bLoad );
            }
        }
    else if ( lpOptions )
        {
        auto    char    szBuff [ SSIZE ];

        SPRINTF ((LPSTR)szBuff, "%d", lpOptions->ButtonBar.uStyle );

        WritePrivateProfileString (szTitle, szButtonStyle, (LPCSTR)szBuff,
                                    szNortonIni);

        WritePrivateProfileString (
                                szTitle,
                                szButtonShow,
                                (lpOptions->ButtonBar.bShow == TRUE) ? "1" : "0",
                                szNortonIni
                                );

        WritePrivateProfileString (
                                szTitle,
                                szHide,
                                (lpOptions->bHide == TRUE) ? "1" : "0",
                                szNortonIni
                                );

        WritePrivateProfileString (
                                szTitle,
                                szTitleTime,
                                (lpOptions->bAutorunMissedEvents == TRUE) ? "1" : "0",
                                szNortonIni
                                );

        WritePrivateProfileString (
                                szTitle,
                                szStatusBar,
                                (lpOptions->bStatusBar == TRUE) ? "1" : "0",
                                szNortonIni
                                );

        WritePrivateProfileString (
                                szTitle,
                                szConfirmExit,
                                (lpOptions->Confirm.bOnExit == TRUE) ? "1" : "0",
                                szNortonIni
                                );

        WritePrivateProfileString (
                                szTitle,
                                szConfirmDelete,
                                (lpOptions->Confirm.bOnDelete == TRUE) ? "1" : "0",
                                szNortonIni
                                );

        WritePrivateProfileString (
                                szTitle,
                                szConfirmSave,
                                (lpOptions->Confirm.bOnSave == TRUE) ? "1" : "0",
                                szNortonIni
                                );

        WritePrivateProfileString (
                                szTitle,
                                szConfirmEdit,
                                (lpOptions->Confirm.bOnEdit == TRUE) ? "1" : "0",
                                szNortonIni
                                );

#ifdef SYM_WIN32
        if (!bUseWinIni)
            {
            BOOL bShellProblem = TRUE;

            char    szName    [ SYM_MAX_PATH + 1 ] = { 0 };
            char    szCmdLine [ SYM_MAX_PATH + 1 ] = { 0 };

            GetModuleFileName ( ghInst, szCmdLine, sizeof(szCmdLine) );
            LoadString ( ghInst, IDS_NPS_TITLE, szName, sizeof(szName) );

            BOOL bExists = ShellLinkExists (
                                    NULL,       // NULL for startup folder
                                    szCmdLine,  // Required
                                    NULL        // Optional
                                    );

            if ( bExists == FALSE )
                {
                bExists = ShellLinkExists (
                                    NULL,       // NULL for startup folder
                                    szCmdLine,  // Required
                                    szName      // Optional
                                    );
                }

            if ( lpOptions->bLoad == TRUE )
                {
                if ( bExists == FALSE )
                    {
                    const   LPSTR   lpArguments =   NULL;
                    const   LPSTR   lpIconPath  =   NULL;
                    const   int     nIconIndex  =   0;
                    const   int     nShowCmd    =   SW_SHOWMINNOACTIVE;
                    const   LPSTR   lpDefDir    =   NULL;

                    bShellProblem = !ShellLinkCreate (
                                            NULL,   // NULL = Startup folder
                                            szCmdLine,
                                            lpArguments,
                                            szName,
                                            lpIconPath,
                                            nIconIndex,
                                            nShowCmd,
                                            lpDefDir
                                            );
                    }
                else
                    {
                    bShellProblem = FALSE;
                    }
                }
            else
                {
                if ( bExists == TRUE )
                    {
                    bShellProblem = !ShellLinkDestroy (
                                            NULL,   // NULL = Startup folder
                                            szCmdLine,
                                            szName
                                            );

                    if ( bShellProblem == TRUE )
                        {
                        bShellProblem = !ShellLinkDestroy (
                                            NULL,   // NULL = Startup folder
                                            szCmdLine,
                                            NULL
                                            );
                        }
                    }
                else
                    {
                    bShellProblem = FALSE;
                    }
                }

            if (bShellProblem == TRUE)
                {
                NMsgBox ( ghInst, NULL, NPS_STRING_SHELL_ERROR, TRACE, NULL, -1 );
                }
            }
        else
#endif
            {
            if ( lpOptions->bLoad == TRUE )
                {
                WinIniLoadAppend ( hDlg, ghInst, NULL, FALSE, FALSE );
                }
            else
                {
                WinIniLoadRemove ( hDlg, ghInst, NULL );
                }
            }
        }
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextHelp (
            HWND    hDlg,
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU and WM_HELP
windows message. The purpose of this function is to call the standard symantec help
routines that display an appropiate: floating help menu or context sensitive help

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextHelp (              // Respond to WM_CONTEXTMENU and WM_HELP
    HWND        hDlg,                   // [in] dialog that sent the message.
    UINT        uMsg,                   // [in] either WM_HELP or WM_CONTEXTMENU
    WPARAM      wParam,                 // [in] wParam from dialog
    LPARAM      lParam                  // [in] lParam from dialog
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
        IDC_NPS_LIST,           IDH_NAVW_SCHEDULER_WHATHIS_EVENTS_LSTBX,
        0,                      0
        };



                                        // -----------------------------------
                                        // Invoke cmn Nav context sensitive help
                                        // -----------------------------------
    StdHelpExOnContextHelp (
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        dwWhatsThisMap,
                        NULL, 
                        NULL,           //  This dlg. doesn't have any  ...
                        NULL            //  ... List Box controls - so NULL here
                        );
}

#endif // #if w32 only



/* **************************************************************************

@Name:
VOID LOCAL DisplayInfoDeskHelp (        // Just as the name sez.
    HWND    hWnd                        // [in] parent window handle
    );

@Description:
This function calls WinHelp() with the path to our info desk help file to
display the Symantec standard help.

@Parameters:
$hWnd$       [in] handle to our parent window

@Returns:       Nothing

************************************************************************** */

VOID LOCAL DisplayInfoDeskHelp (        // Just as the name sez.
    HWND    hWnd                        // [in] parent window handle
    )
{
    auto    char    szInfoPath [ SYM_MAX_PATH + 1 ] = { 0 };
    auto    char    szFileName [ SYM_MAX_PATH + 1 ] = { 0 };

    GetModuleFileName ( ghInst, szInfoPath, sizeof(szInfoPath) );
    NameStripFile ( szInfoPath );
    SYM_ASSERT ( STRLEN(szInfoPath) );

    LoadString ( ghInst, IDS_INFODESK, szFileName, sizeof(szFileName) );
    SYM_ASSERT ( *szFileName );

    NameAppendFile ( szInfoPath, szFileName );

    WinHelp ( hWnd, szInfoPath, HELP_FINDER, 0 );
}



/************************************************************************
 *                                                                      *
 * Description: Allows a command line; sets external variables.         *
 *                                                                      *
 * Parameters: LPSTR lpCommandLine                                      *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 06/13/1995 GWESTER Function Created.                                   *
 ************************************************************************/

VOID PASCAL NPSParseCommandLine(LPSTR lpCommandLine)
{


                                        //-------------------------------
                                        // Install switch
                                        //-------------------------------
    if (ConfigSwitchSet (&rswNPSMinimized, NULL))
        {
        LaunchMinimized = TRUE;
        }

}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------


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
//void DrawCaption(HWND hWnd, CRect& rectDraw, BOOL bActive)
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
	DWORD		dwStyle   = GetWindowLong(hWnd, GWL_STYLE);
    int         xBorder   = GetSystemMetrics(/*dwStyle & DS_MODALFRAME ? */SM_CXFIXEDFRAME/* : SM_CXSIZEFRAME*/);
    int         yBorder   = GetSystemMetrics(/*dwStyle & DS_MODALFRAME ? */SM_CYFIXEDFRAME/* : SM_CYSIZEFRAME*/);
    int         xSize   = GetSystemMetrics(SM_CXSIZE);
    int         ySize   = GetSystemMetrics(SM_CYCAPTION) - 1;//2;
    int         xIconSize = GetSystemMetrics(SM_CXSMICON);
    int         yIconSize = GetSystemMetrics(SM_CYSMICON);
    NONCLIENTMETRICS ncm;

                                        // Only custom draw title bar on
                                        // Windows versions of 4.0 and greater.
    if ( LOBYTE(GetVersion()) >= 4 )
        {
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
        hIcon = (HICON)LoadImage (ghInst, MAKEINTRESOURCE(IDI_NPS_ICON), IMAGE_ICON, 16, 16, 0);

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
} // End COffScreenDC_Destructor ()



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

/////////////////////////////////////////////////////////////////////
// void VerifySchedulerIsRunning( HWND hDlg )
//
// Purpose:
//   This function will verify that the scheduler is running, and if
//   not, prompt to start the scheduler for you.
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 06-09-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void VerifySchedulerIsRunning( HWND hDlg )
{
                                        // Is Scheduler Running?
    if ( FALSE == IsSchedulerRunning() )
        {
                                        // Scheduler is not running.  Query to
                                        // start the scheduler?
        if ( IDYES == DisplayMessage(
                hDlg,
                IDS_NPS_START_SCHEDULER,
                IDS_NPS_TITLE,
                MB_YESNO | MB_ICONQUESTION
                ) )
            {
                                        // Start the scheudler.
            if ( FALSE == StartScheduler(NULL) )
                {
                                        // Notify that scheduler could not
                                        // start.
                DisplayMessage(
                    hDlg,
                    IDS_SCHEDULER_COULD_NOT_START,
                    IDS_NPS_TITLE,
                    MB_OK | MB_ICONWARNING
                    );
                }
            else
                {
                                        // Notify that scheduler was started
                DisplayMessage(
                    hDlg,
                    IDS_SCHEDULER_STARTED_SUCCESSFULLY,
                    IDS_NPS_TITLE,
                    MB_OK
                    );

                                        // Signal the service to check for
                                        // missed events.
                HANDLE hEvent;

                hEvent = CreateEvent ( NULL, FALSE, FALSE,
                                       NPS_LOGIN_EVENT_NAME );

                if ( NULL != hEvent )
                    {
                    SetEvent ( hEvent );
                    CloseHandle ( hEvent );
                    }
                }
            }
        else
            {
                                        // Notify that the scheduler is not
                                        // running, and cannot perform tasks.
            DisplayMessage(
                hDlg,
                IDS_NPS_SERVICE_NOT_RUNNING,
                IDS_NPS_TITLE,
                MB_OK | MB_ICONWARNING
                );
            }
        }
}

/////////////////////////////////////////////////////////////////////
// int DisplayMessage( HWND hWnd, DWORD dwMessageID, DWORD dwTitleID, UINT uType )
//
// Purpose:
//    This funciton will be a middle function for a message box.  It
//    will take care of loading the strings and doing error checking.
//    this function will return the return value from your message
//    box, or FALSE if an error occurred.
//
// Parameters:
//    HWND  hWnd        - [IN] Parent Window.
//    DWORD dwMessageID - [IN] Message string ID for this message.
//    DWORD dwTitleID   - [IN] Title string ID for this message.
//    UINT  uType       - [IN] Type of message box.  This is a
//                             passthrough for MessageBox.  Use the
//                             same ID's you would for it.
//
// Returns:
//    FALSE     - An Error occurred;
//    Non False - Return from MessageBox.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-09-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
int DisplayMessage(
    HWND  hWnd,
    DWORD dwMessageID,
    DWORD dwTitleID,
    UINT  uType
    )
{
TCHAR szMessage[SYM_MAX_PATH];
TCHAR szMessageTitle[SYM_MAX_PATH];
int   iRetVal=FALSE;


                                        // Load our message string.
    if ( FALSE == LoadString( ghInst, dwMessageID, szMessage, SYM_MAX_PATH ) )
        goto BailOut;

                                        // Load out title string.
    if ( FALSE == LoadString( ghInst, dwTitleID, szMessageTitle, SYM_MAX_PATH ) )
        goto BailOut;

                                        // Display the message.
    iRetVal = MessageBox(
        hWnd,
        szMessage,
        szMessageTitle,
        uType
        );


BailOut:

                                        // Return success variable.
    return iRetVal;
}

//***************************************************************************
// NpsReturnCmdParams()
//
// Description:
//      Look at the full command line that is passed in and copy the command
// line and parameter parts to their respective buffers.
//
// Parameters:
//      lpszFullCmdLine                 [in] Full command line
//      lpszCmd                         [out] Buffer to receive cmd
//      lpszParams                      [out] Buffer to receive params
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/05/1993 MARKL Function Created.
// 07/01/1995 MARKL Function was destroyed by our buddy now at McAfee.  Fixed.
//***************************************************************************

VOID WINAPI NpsReturnCmdParams (        // Return command and its parameters
    LPCSTR      lpszFullCmdLine,        // [in] Full command line to parse
    LPSTR       lpszCmd,                // [out] Buffer to receive command
    LPSTR       lpszParams)             // [out] Buffer to receive parameters
{
    LPCSTR      lpszFrom;
    LPSTR       lpszTo;
    char        cCheckFor;

    SYM_ASSERT ( lpszFullCmdLine );
    SYM_ASSERT ( lpszCmd );

                                        // First we parse out the command
                                        // line.  This may be a simple SFN
                                        // or a quoted LFN.  See if we are
                                        // looking for the first space or
                                        // a matching quote.
    if ( *lpszFullCmdLine == '\"' )
        cCheckFor = '\"';
    else
        cCheckFor = ' ';

                                        // Copy Command to lpszCmd, making
                                        // sure that if it is quoted, that
                                        // the matching quote makes it into
                                        // the command as well.
    lpszFrom = lpszFullCmdLine;
    lpszTo   = lpszCmd;
    do
        {
        *lpszTo  = *lpszFrom;
        if (DBCSIsLeadByte(*lpszFrom))
            {*(lpszTo+1)  = *(lpszFrom+1);}

        lpszTo   = CharNext ( lpszTo );
        lpszFrom = CharNext ( lpszFrom );
        } while ( (*lpszFrom != cCheckFor) && (*lpszFrom != EOS) );
    if ( *lpszFrom == '\"' )
        {
        *lpszTo  = *lpszFrom;
        if (DBCSIsLeadByte(*lpszFrom))
            {*(lpszTo+1)  = *(lpszFrom+1);}

        lpszTo   = CharNext ( lpszTo );
        lpszFrom = CharNext ( lpszFrom );
        }
    *lpszTo = EOS;

                                        // Nuke any white space between the
                                        // command and its parameters.
    while ( *lpszFrom == ' ' )
        lpszFrom = CharNext ( lpszFrom );

                                        // If there is anything left, its
                                        // parameters.
    if ( lpszParams != NULL)
        STRCPY (lpszParams, lpszFrom);
}

/////////////////////////////////////////////////////////////////////
// BOOL AddColumn( HWND hListView, int nColumnStringID )
//
// Purpose:
//  This function will add a column to the end of a SYSListView32
//  control.  It will do this by loading the string ID passed to it,
//  and then adding a column with that string.
//
// Parameters:
//    HWND hListView      - [IN] Handle to the list view control.
//    int nColumnStringID - [IN] String ID of this column title.
//    DWORD dwIndex       - [IN] Index to add column.
//    DWORD dwWidth       - [IN] Initial width of the column.
//
// Returns:
//    TRUE  - Column added successfully.
//    FALSE - Error adding column.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-23-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL AddColumn(
    HWND hListView,
    int nColumnStringID,
    DWORD dwIndex,
    DWORD dwWidth
    )
{
LV_COLUMN   rColumnToAdd;
TCHAR       szColumnTitle[SYM_MAX_PATH];
BOOL        bRetVal=TRUE;

                                        // Attempt to load the column title
                                        // string.
    if ( LoadString( ghInst,
                     nColumnStringID,
                     szColumnTitle,
                     SYM_MAX_PATH
                     ) )
        {
                                        // Clear all data from our structure.
        memset( &rColumnToAdd, 0, sizeof( rColumnToAdd ) );
        rColumnToAdd.mask = LVCF_TEXT | LVCF_WIDTH;
        rColumnToAdd.pszText = szColumnTitle;
        rColumnToAdd.cx = dwWidth;
        ListView_InsertColumn( hListView, dwIndex, &rColumnToAdd );

        bRetVal = TRUE;
        }
    else
        {
        bRetVal = FALSE;
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// VOID NpsInitListView(
//
// Purpose:
//    Initialize our main list view control.  Add headings, and
//    resize if neccessary.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-23-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID NpsInitListView(
            HWND hDlg
    )
{
HWND    hListView;
RECT    rListViewRect;
DWORD   dwListWidth;

                                        // Get a handle to our control.
    hListView = GetDlgItem( hDlg, IDC_NPS_LIST );

                                        // Calculate width in pixels.
    GetClientRect( hListView, &rListViewRect );
    dwListWidth = rListViewRect.right - rListViewRect.left;

                                        // If we were able to locate our
                                        // control...
    if ( NULL != hListView )
        {
                                        // Add the columns.
        AddColumn( hListView, IDS_NPS_EVENTS_COLUMN_TITLE,    0, dwListWidth/2 );
        AddColumn( hListView, IDS_NPS_FREQUENCY_COLUMN_TITLE, 1, 70 );
        AddColumn( hListView, IDS_NPS_SCHEDULED_COLUMN_TITLE, 2, dwListWidth - (dwListWidth/2) - 70 );
        NpsResizeListColumns ( hDlg );
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    NpsResizeListColumns()
//
// Description:
//  Resizes the list control's columns to that all data is visible
//  (that is, no column is too narrow)
//
// Input:
//  hDlg: [in] Handle of main window.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 5/8/98  Mike Dunn -- function created.
//////////////////////////////////////////////////////////////////////////

void NpsResizeListColumns ( HWND hDlg )
{
HWND hList = GetDlgItem ( hDlg, IDC_NPS_LIST );

    SYM_ASSERT ( NULL != hList );

    ListView_SetColumnWidth ( hList, 0, LVSCW_AUTOSIZE_USEHEADER );
    ListView_SetColumnWidth ( hList, 1, LVSCW_AUTOSIZE_USEHEADER );
    ListView_SetColumnWidth ( hList, 2, LVSCW_AUTOSIZE_USEHEADER );
}

