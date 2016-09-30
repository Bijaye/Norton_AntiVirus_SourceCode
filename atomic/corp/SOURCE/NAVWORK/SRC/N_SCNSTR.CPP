// Copyright 1993-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_scnstr.cpv   1.36   22 Sep 1998 22:07:42   tcashin  $
//
// Description:
//      Entry point for NAVW virus scan.
//
// Contains:
//      ScanStart()
//      ScanDlgProc()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_scnstr.cpv  $
// 
//    Rev 1.36   22 Sep 1998 22:07:42   tcashin
// Log the start of a scan.
// 
//    Rev 1.35   17 Sep 1998 16:35:20   tcashin
// Called LogEventEx() in NavWorkEnd() to log scan results stats.
// 
//    Rev 1.34   26 Jun 1998 15:13:04   jtaylor
// Dissabled Quaranine items if Quarantine is not enabled.
//
//    Rev 1.33   25 Jun 1998 20:23:54   jtaylor
// Only attempt to forward Quarantine files if Quarantine is a features of this install.
//
//    Rev 1.32   17 Jun 1998 23:49:18   tcashin
// Need to call forwardfiles() earlier.
//
//    Rev 1.30   07 Jun 1998 15:19:24   tcashin
// Tweaked MyHideMbrBrControls() for the new scanning dialogs. This
// caused the new dialogs to be totally screwed up on NEC machines.
//
//    Rev 1.29   03 Jun 1998 18:35:06   tcashin
// Moved the quarantine forwarding code to n_forwrd.cpp.
//
//    Rev 1.28   18 May 1998 23:45:58   tcashin
// Set the font for the new file delete and quarantine controls.
//
//    Rev 1.27   12 May 1998 15:14:36   jbelden
// Ported to changes from Roswell area.  1.  fixed problem where
// "scan interrupted"was placed in the activity log incorrectly when
// scanning from the command line and 2.  if no one is logged in and
// there is a scheduled scan on NT, if a virus is detected and no loner
// wait for somebody to log in to write to the activity log.  This
// is in the case of automated is true.
//
//    Rev 1.26   06 May 1998 14:51:50   DBuches
// Added ability to start and stop animation.
//
//    Rev 1.25   13 Apr 1998 22:43:20   tcashin
// Now call Quarantine's ForwardAllFiles function in NavWorkEnd().
//
//    Rev 1.24   06 Apr 1998 15:22:26   DBuches
// Changes for use of animation common control.
//
//    Rev 1.23   06 Apr 1998 12:27:04   MKEATIN
// Removed calls to NavZipInit() and NavZipCleanup().
//
//    Rev 1.22   18 Feb 1998 16:22:14   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.21   02 Sep 1997 21:47:34   DALLEE
// Added call to toggle off shown state of long custom message on
// WM_NAVWORKEND. This should reset it for the next scan.
//
//    Rev 1.20   20 Aug 1997 12:35:14   DBUCHES
// oops.  szDrives is a NULLNULL list.  Fixed problems with updating last scanned registry information.
//
//    Rev 1.19   13 Aug 1997 15:57:20   DBUCHES
// Updated saving of last scan data.
//
//    Rev 1.18   12 Aug 1997 21:12:36   BGERHAR
// Add UNC to the allow network scan hack
//
//    Rev 1.17   11 Aug 1997 01:02:32   KSACKIN
// Now make call to NavZipProcessFile() instead of ZipProcessFile().  This is
// one level of indirection so that we can dynamically link to the NavZip dll.
//
//    Rev 1.16   31 Jul 1997 11:58:42   BGERHAR
// Turn off immediate notify if /NORESULT is used
//
//    Rev 1.15   30 Jul 1997 20:03:22   BGERHAR
// No ZIP scanning for DEC alpha
//
//    Rev 1.14   26 Jul 1997 21:58:46   BGERHAR
// Activity log now works with /NORESULTS. Also fixed minor issue with /NORESULTS and /HMEM
//
//    Rev 1.13   25 Jul 1997 18:27:36   DBUCHES
// Added check for network drives when network drive scanning is disabled.
//
//    Rev 1.12   21 Jul 1997 01:42:00   BGERHAR
// Added PROGRAM_AUTOMATED support for /NORESULTS switch
//
//    Rev 1.11   18 Jul 1997 19:45:38   GWESTER
// Modified the animation to use either the 16-color one (on 16 or 256 color
// resolution) or the 256-color one (if running more than 256-color
// resolution).
//
//    Rev 1.10   17 Jul 1997 16:10:40   DBUCHES
// Removed un-neccessary Custom response code.
//
//    Rev 1.9   15 Jul 1997 14:42:16   DBUCHES
// Added Ctl3d support for DEC Alpha
//
//    Rev 1.8   03 Jul 1997 15:01:46   TIVANOV
// fix custom notify responce
//
//    Rev 1.7   24 Jun 1997 16:02:54   TIVANOV
// remove a flag introduced for the LongCustomMsg
//
//    Rev 1.6   06 Jun 1997 15:05:56   PVENKAT
// Added ScanInProgress Registry Key processing (Setting it to "1" when scanning
// and to "0" when done.  (This is to be used in Savanna).
//
//
//
//    Rev 1.5   22 May 1997 16:21:14   BILL
// Alpha changes (removed ctl3d)
//
//    Rev 1.4   22 May 1997 11:30:08   TIVANOV
// use of the LongCusomMessage
//
//    Rev 1.3   14 Apr 1997 19:23:02   DALLEE
// Use uProgramMode to determine if in an interactive scanning mode
// when checking whether to display Mem/Boot Scan, Problems Found, and
// Scan Results dialogs.
// Dialogs should now display correctly for scans launched by NAVUI(NAV32.EXE).
//
// Some housecleaning in NavWorkEnd() so I could understand the logic and
// make the previous modification.
//
// Removed ShouldLaunchWizardUsingDialog() function which always
// returned FALSE and related unused Repair Wizard code fragments.
//
// Removed some of the more ancient log comments.
//
//    Rev 1.2   07 Apr 1997 18:47:04   DALLEE
// Work to accommodate new SCANITEM input to ScanStart().
// DeleteSkippedItems() modifies SCANITEM array as well as old input buffer.
// hScanList buffer is now locked on input since SCANITEM array has
// pointers into buffer.
//
//    Rev 1.1   25 Feb 1997 12:12:48   DALLEE
// Port from HYDRA.15:
//
//            Rev 1.106   30 Sep 1996 16:17:22   CCOTRIL
//         Ability to cancel scan now depends on config setting.  Fixes STS61674.
//
//    Rev 1.0   06 Feb 1997 21:02:56   RFULLER
// Initial revision
//
//    Rev 1.22   27 Jan 1997 15:00:04   JBRENNA
// On NEC, hide the MBR / BR scanning information at run-time.
//
//    Rev 1.21   23 Sep 1996 13:41:32   DALLEE
// Finished removing klugery around end of scan logging mentioned in
// rev 1.18. Had left over buffer on stack, and OemToAnsi() conversion on
// what was now already an ANSI string.
//
// Also removed hard coded 4-space tabs in the code.  Grr....
//
//    Rev 1.20   23 Sep 1996 12:50:40   DALLEE
// Pulled last change. Center dialog was already being done in
// WM_CREATE based on parent window in CREATESTRUCT.
//
//    Rev 1.19   13 Sep 1996 13:44:42   TIVANOV
// port from Gryphone - center the dialog
//
//    Rev 1.18   09 Sep 1996 16:17:58   PVENKAT
// Ported Gryphon changes.
//
//
//      Fixed #63363.  Now we get szFullPath to have complete directory and usign
//      this  to log events.  On ScanEnd, only this will have what we scanned.
//
//    Rev 1.17   01 Sep 1996 20:30:08   DALLEE
// After hiding scan dialog, added SetActiveWindow() before minimizing
// main window. Slight kluge for making NAV show up for Alt-Tab list when
// minimized.
//
//    Rev 1.16   30 Aug 1996 12:58:30   DALLEE
// Rewrote hide/show/minimize junk for main window and scan dialog.
// Removed most of the special case NT/95 checks and replaced with check vs.
// taskbar present (SystemWin95ShellPresent).
// Changes are in ScanDlgProc(), DoInitialization(), and NavWorkEnd().
//
//    Rev 1.15   19 Aug 1996 14:02:56   GWESTER
// For Kirin, removed the "#ifdef NAVSCANW" statements around download
// and "GloballySaved_..." code, just like in LuigiPlus. This code will
// now get compiled whether you're compiling for Win95 or NT.
//
//    Rev 1.14   26 Jul 1996 14:01:06   TIVANOV
// fix the task bar registration
//
//    Rev 1.13   25 Jul 1996 08:50:26   PVENKAT
// Fixed #61914.  Activity log correctly displays ANSI path.
//
//
//    Rev 1.12   18 Jul 1996 10:38:12   TIVANOV
// fix the task bar registration and the icon paining
//
//    Rev 1.11   15 Jul 1996 10:13:26   PVENKAT
// Fixed #56065.  Activitylog always contains Full path name in the case LFN.
//
//
//    Rev 1.10   12 Jul 1996 09:25:58   TIVANOV
// fix which brings the after scan dialogs to the foreground
//
//    Rev 1.9   11 Jul 1996 08:03:54   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.8   28 Jun 1996 15:07:12   DALLEE
// Moved necessary variables from scan dialog proc to NavWorkUser().
// NavWorkUser() was created last rev by cut and paste from the
// (fat) dialog proc.
//
//    Rev 1.7   28 Jun 1996 14:50:52   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.6   26 Jun 1996 10:55:24   TIVANOV
// changes for the icon button on the taskbar(NT4.0)
//
//    Rev 1.5   20 Jun 1996 19:00:38   GWESTER
// Ported in fixes 48198 and 49040 from PileDriver.
// 48198: When a scan is launched from Norton File Manager, we provide a
// confirmation dialog when no viruses are found.
// 49040: When minimizing NAV during a scan, the icon goes into the correct
// place on the desktop.
//
//    Rev 1.4   21 May 1996 15:13:50   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
//
//    Rev 1.3   09 May 1996 17:57:36   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.2   04 Apr 1996 16:47:16   YVANG
// Ported from Kirin.
//
//    Rev 1.2   03 Apr 1996 14:53:08   YVANG
// DBCS enable.
//
//    Rev 1.1   22 Feb 1996 14:08:14   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:56:00   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:02   BARRY
// Initial revision.
//
//    Rev 1.84   30 Jan 1996 00:40:26   GWESTER
// In download mode, no activity logging will be done. (This will probably
// change.)
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "file.h"
#include "navutil.h"
#include "nonav.h"
#include "ngui.h"
#include "symcfg.h"
#include "symnet.h"
#include "netobj.h"
#include "options.h"
#include "cmbstr.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navwork.h"
#include "navalog.h"
#include "stdhelp.h"
#include "navwzip.h"
#include "syminteg.h"
#include "ctl3d.h"
#include "navoptx.h"

#include "stub.h"
#include "virscan.h"
#include "inoc.h"
#include "AlignWnd.h"

#include "dbcs.h"

#ifdef SYM_WIN32
    #include "navstat.h"
    #include "symstat.h"
    #include "navver.h"
    #include "commctrl.h"               // For animation control
    #include "n32behav.h"               // Personality DLL.
#endif

#include "navcmdid.h"

extern HINSTANCE hInstance;


#ifdef SYM_WIN32

// ************************************************************************ //
// ****         globals and statics                                    **** //
// ************************************************************************ //

typedef HBITMAP FAR *LPHBITMAP;

typedef struct
    {
    LPHBITMAP   lphBmpAnimation;
    LPHBITMAP   lphBmpAnimationMask;
    WORD        wTotalFrames;
    WORD        wCurrentFrame;
    UINT        uAnimationTimerID;
    } ANIMATION;


static  HWND    s_hwndOwner     =   NULL;

LPNONAV GloballySaved_lpNav     =   NULL;

static TCHAR    SZ_SCANINPROGRESS_KEY[] = TEXT ("SOFTWARE\\Symantec\\Norton AntiVirus\\ScanInProgress");

#endif

// ************************************************************************ //
// ****                    Local Prototypes                            **** //
// ************************************************************************ //
LRESULT LOCAL NavMainWndWM_CTLCOLOR(    // Handles WM_CTLCOLOR for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_CTLCOLOR message
    LPARAM      lParam);                // lParam of WM_CTLCOLOR message

BOOL LOCAL DeleteSkippedItems (         // Delete items that are skipped.
    LPSTR       lpItemList,             // [in] from this list.
    LPSCANITEM  lpScanItems );          // [in] and from this index.

LRESULT LOCAL DoInitialization (
    HWND    hDlg,
    LPNONAV lpNav,
    LPBOOL  lpbPaintProcessed );

BOOL LOCAL CheckForFixedDisk (          // Check for a Fixed disk because
    VOID);                              // there are things we can't do for
                                        // clients without fixed disks

BOOL LOCAL NavWorkEnd (                 // Done with all the scan related
    HWND    hDlg);                      // stuff

BOOL LOCAL NavWorkInit (                // initialize all the scan related
    HWND    hDlg,                       // stuff with information provided
    LPSCANSTRUCT lpScan);               // by the parent.

VOID SYM_EXPORT WINAPI SkipDrive (      // callback function required by
    LPSTR FAR *lplp,                    // GetBytesToScan()
    DWORD   dwUserData);

UINT SYM_EXPORT WINAPI DriveError (     // callback function required by
    BYTE    byDrive,                    // GetBytesToScan()
    DWORD   dwUserData);

VOID LOCAL PASCAL ScanAnimationStart (      // Starts Animation
    HWND        hDlg
    ) ;

VOID LOCAL PASCAL ScanAnimationEnd (
    HWND        hDlg
    );

VOID LOCAL PASCAL StampScanTimeInRegistry (
    LPNONAV     lpNav,
    LPSTR       lpDrives
    );

BOOL LOCAL PASCAL IsDriveInListing (
    LPSTR       lpAllDrives,
    LPSTR       lpThisDrive
    );

VOID PASCAL ScanAnimationEnable (
    HWND        hDlg
    );

VOID PASCAL ScanAnimationDisable (
    HWND        hDlg
    );


void LOCAL NavWorkUser(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL LOCAL PASCAL CheckForNetworkDrivesHack( HWND hWnd, LPSTR lpScanList );

BOOL LOCAL BypassRepairWizard ( VOID );

static void MyHideMbrBrControls (HWND hDlg);

DWORD LOCAL DisplayableColors ( VOID );

BOOL LOCAL GetPath(TCHAR * szTemp, int size);
//************************************************************************

//************************************************************************
// ScanStart()
//
// HWND SYM_EXPORT WINAPI ScanStart (LPSCANSTRUCT lpScan)
//
// This routine opens the modeless scan dialog.
// WM_CREATE, WM_INITDIALOG, and WM_PAINT are all processed before return.
// The message to start scanning has been posted, but actual scanning
// will not have begun yet.
//
// Note that the scan engine must be initialized prior to calling this
// function.
//
// Parameters:
//      LPSCANSTRUCT    lpScan      Initial info for this scan.
//
// Returns:
//      HWND    Handle to scan window on success
//      NULL    on failure.
//************************************************************************
// 01/08/1993 MFALLEN, created.
// 08/26/1996 DALLEE, updated comments.
//************************************************************************

HWND SYM_EXPORT WINAPI ScanStart (LPSCANSTRUCT lpScan)
{
    auto    HWND            hScanWin;

#ifndef NAVSCANW
    hScanWin =  CreateDialog(
                            hInstance,
                            MAKEINTRESOURCE (IDD_SCANDLG),
                            lpScan->hDlg,
                            NULL
                            );
#else
    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        hScanWin =  CreateDialog(
                                hInstance,
                                MAKEINTRESOURCE (IDD_SCANDLG_NT),
                                lpScan->hDlg,
                                NULL
                                );
    else
        hScanWin =  CreateDialog(
                                hInstance,
                                MAKEINTRESOURCE (IDD_SCANDLG),
                                lpScan->hDlg,
                                NULL
                                );
#endif

    if (hScanWin)
        {
        s_hwndOwner = lpScan->hDlg;

        hScanWin = (HWND)SendMessage ( hScanWin,
                                       WM_INITDIALOG,
                                       0,
                                       (LPARAM)lpScan );
        }

    return (hScanWin);
} // ScanStart()


//***************************************************************************/
// RegisterScanInProgress ()                                                */
//                                                                          */
//  This function creates and updates the registry entry to allow other     */
//  applications to know whether NAV is scanning or not.                    */
//                                                                          */
// Parameters:                                                              */
//      bScan       TRUE    if are starting scan                            */
//                  FALSE   scan finished.                                  */
//                                                                          */
// Returns:                                                                 */
//      BOOL        TRUE    if successful                                   */
//                  FALSE   failed.                                         */
//***************************************************************************/
//  06/03/97    PVENKAT     Function created.                               */
//***************************************************************************/
BOOL
RegisterScanInProgress (BOOL bScan)
{
    LONG    lErr = ERROR_SUCCESS;       // Assume success.
    TCHAR   szData[MAX_PATH + 1];       // Let's write the data as string
    HKEY    hKey;                       // Key.
    DWORD   dwDisposition;              // We can check for existence using
                                        // this variable.

    // Try to create the key, if already exists dwDisposition will say
    // that, and we donot need check that, as we are only interested to
    // in the presence of the key.

    lErr = RegCreateKeyEx (HKEY_LOCAL_MACHINE,          // Key
                                SZ_SCANINPROGRESS_KEY,      // Subkey
                                0,                          // Reserved
                                NULL,                       // Classname
                                REG_OPTION_NON_VOLATILE,    // options
                                KEY_ALL_ACCESS,             // SAM attr
                                NULL,                       // security desc
                                &hKey,                      // Handle to key
                                &dwDisposition);            //

    // Make a string value of '1' or '0'
    wsprintf (szData, TEXT ("%d"), bScan ? 1 : 0);

    // If we have the key then lets update it.
    if (lErr == ERROR_SUCCESS)
    {
        // Set the value of the key based on the parameter.
        lErr = RegSetValue (hKey,
                        NULL,
                        REG_SZ,
                        szData,
                        STRLEN (szData));

        // We are done, close the registry
        RegCloseKey (hKey);
    }

    return (lErr == ERROR_SUCCESS);     // Status.
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

LRESULT SYM_EXPORT CALLBACK ScanDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam )
{
    static  BOOL            bPaintProcessed = FALSE;
    static  BOOL            bWasIcon = FALSE;
    extern  BOOL            bMemoryScanned;
    auto    LPNONAV         lpNav;
    auto    UINT            uStatus = 0;
    auto    LRESULT         lResult = 0;

    switch ( uMessage )
        {
        case WM_CTLCOLOR:
            return (NavMainWndWM_CTLCOLOR (hDlg, wParam, lParam));

        case WM_CREATE:
            if ( GetCustomVersion() == VERSION_NETSCAPE )
                {                       // Title is "Internet Scanner"; set at runtime
                SetWindowText (hDlg, String_Ptr(IDS_NAV_INTERNET));
                }
            NG_CenterDialog (hDlg, ((LPCREATESTRUCT)lParam)->hwndParent);
            RegisterScanInProgress (TRUE);
            break;

        case WM_QUERYOPEN:
            if ( GetCustomVersion() == VERSION_NETSCAPE )
                {                       // Title is "Internet Scanner"; set at runtime.
                SetWindowText (hDlg, String_Ptr(IDS_NAV_INTERNET));
                }
            else
                {
                SetWindowText (hDlg, String_Ptr(IDS_NAV));
                }
            lResult = TRUE;
            break;

        case WM_WINDOWPOSCHANGED:
            break;

        case WM_INITDIALOG:
            NG_SetFontsInDialog (hDlg, 0);

            // This call initializes options and saves lpNav
            // (includes lpNav->lpScan) with LP_SET.

            if (!NavWorkInit (hDlg, (LPSCANSTRUCT) lParam))
                DestroyWindow(hDlg);    // memory alloc failed, destroy dlg

            lpNav = (LPNONAV)LP_GET(hDlg);
            if (!lpNav)
                {LP_SET (hDlg, GloballySaved_lpNav);
                 lpNav = (LPNONAV)LP_GET(hDlg);
                 SYM_ASSERT (lpNav);}

            // This call does the UI & dialog specific initialization:
            // Minimize/Hide/Move/Show hDlg and parent, DrawIcon,
            // Set title text, etc.

            lResult = DoInitialization (hDlg, lpNav,
                                        &bPaintProcessed );
            break;

        case WM_PAINT:
            if ( bPaintProcessed == FALSE )
                {
                bPaintProcessed = TRUE;
                PostMessage(hDlg, WM_NAVWORKUSER, 0, 0L);
                }

                                        // -----------------------------------
                                        // Note that when a paint message is
                                        // received we must make sure to update
                                        // our animation bitmap. The problem
                                        // solved is when the window is inactive
                                        // an being overlapped by an other
                                        // window we paint the bitmap.
                                        // -----------------------------------

            lResult = DefDlgProc (hDlg, uMessage, wParam, lParam);

            UpdateWindow ( hDlg );

            break;

        case WM_NAVWORKUSER:
            NavWorkUser(hDlg, uMessage, wParam, lParam);
            break;

        case WM_COMMAND:
            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDCANCEL:
                case ID_SC_STOP:
                    lpNav = (LPNONAV)LP_GET(hDlg);
                                          // Verify lpNav if this is NT
                    if (!lpNav)
                        {LP_SET (hDlg, GloballySaved_lpNav);
                        lpNav = (LPNONAV)LP_GET(hDlg);
                        SYM_ASSERT (lpNav);}

                    if (lpNav->Opt.bAllowScanStop == TRUE)      // Only allow exit if configured to allow it
                        lpNav->bDoScan = FALSE;
                }
            break;


        case WM_SYSCOMMAND:
            lpNav = (LPNONAV)LP_GET(hDlg);
                                          // Verify lpNav if this is NT
            if (!lpNav)
                {LP_SET (hDlg, GloballySaved_lpNav);
                 lpNav = (LPNONAV)LP_GET(hDlg);
                 SYM_ASSERT (lpNav);}

                                        // ------------------------------------
                                        // Fix Minimize Ownee Bug - relation
                                        // of 2 overlapped top frame windows:
                                        // (when owner is minimized - ownee will
                                        // be hidden !
                                        // ------------------------------------
            if (SC_MINIMIZE == wParam)
                {
                ShowWindow(hDlg, SW_HIDE);

                                        // If taskbar is not present, then
                                        // main window was hidden, not
                                        // minimized, and we need to show it.
                if (!SystemWin95ShellPresent())
                    {
                                        // DALLEE: Trust me on this --
                                        // SetActiveWindow() ensures we
                                        // show up alt-tab'ing in NT 3.51.
                    SetActiveWindow(GetParent(hDlg));

                    ShowWindow(GetParent(hDlg), SW_MINIMIZE);

                                        // DALLEE: 2nd and last kluge --
                                        // Use StatsTimer() to immediately set
                                        // icon and percent done in main wnd.
                    StatsTimer(hDlg, 0, 0, 0);
                    }

                lResult = 0;
                }
            else if (SC_CLOSE == wParam)
                {
                if (lpNav->Opt.bAllowScanStop != FALSE)
                    FORWARD_WM_COMMAND ( hDlg, ID_SC_STOP,
                                         GetDlgItem ( hDlg, ID_SC_STOP ),
                                         BN_CLICKED, PostMessage );
                }
            else
                {
                lResult = DefWindowProc (hDlg, uMessage, wParam, lParam);
                }

            break;

        case WM_DESTROY:
                                        // ------------------------------------
                                        // Stop and delete Animation (if any)
                                        // ------------------------------------
            ScanAnimationEnd ( hDlg );

            PostMessage(s_hwndOwner, WM_SYSCOMMAND, SC_RESTORE, 0l);

            PostMessage(s_hwndOwner, WM_COMMAND, IDC_STATUS_QUAR_ENUM, 0l);

            RegisterScanInProgress (FALSE);

            lResult = DefWindowProc (hDlg, uMessage, wParam, lParam);
            break;
                                        // default:  Let Windows handle the
                                        // message since we do not want to
                                        // deal with it.
        case WM_CLOSE:
        default:
            lResult = DefDlgProc (hDlg, uMessage, wParam, lParam);
        }

    return (lResult);
}

BOOL LOCAL CheckForFixedDisk(VOID)
{
    auto     ABSDISKREC  AbsDiskInfo;   // To see if a HD is present
    auto     BOOL        bReturn = TRUE;// Return value

    if ( HWIsNEC() )
        {
        return FALSE;
        }

                                        // Add a call to see if we have any
                                        // fixed disks on this system.
    AbsDiskInfo.dn = 0x80;              // Look at the C drive

    if (DiskGetPhysicalInfo(&AbsDiskInfo) != 0)
        {                               // Did an error occurr?
        bReturn = FALSE;
        }

    return (bReturn);
}


//************************************************************************
// DoInitialization()
//
// Second "initialization" routine called for scan dialog.
// See also NavWorkInit().
//
// Sets up all controls, does the minimize/hide/move kluge to main window,
// and
//
// Parameters:
//      HWND    hDlg
//      LPNONAV lpNav
//      LPBOOL  lpbPaintProcessed
//
// Returns:
//
//************************************************************************
// 08/30/1996 DALLEE, r1.15 commented, made readable (I hope).
//      Removed 16-bit code fragments.
//      Removed NT kluges for automation testing and icon painting (these
//          problems have since been fixed)
//      Merged NT and 95 source. Working for NT 3.51, NT 4.0, and Win95.
//************************************************************************

LRESULT LOCAL DoInitialization (
    HWND    hDlg,
    LPNONAV lpNav,
    LPBOOL  lpbPaintProcessed )
{
    auto    FILLBARSTYLE    fbStyle;

    DragAcceptFiles (lpNav->lpScan->hDlg, FALSE);

    Ctl3dSubclassDlgEx(hDlg, CTL3D_ALL);

#ifndef NAVSCANW
    if (SYM_SYSTEM_WIN_NT != SystemGetWindowsType())
        {
        NG_SetChildSmallFont (hDlg, ID_SC_MEMSCAN);
        NG_SetChildSmallFont (hDlg, ID_SC_MEMINF);
        NG_SetChildSmallFont (hDlg, ID_SC_MEMCLN);
        }
#endif
    NG_SetChildSmallFont (hDlg, ID_SC_DIR);
    NG_SetChildSmallFont (hDlg, ID_SC_MBSCAN);
    NG_SetChildSmallFont (hDlg, ID_SC_MBINF);
    NG_SetChildSmallFont (hDlg, ID_SC_MBCLN);
    NG_SetChildSmallFont (hDlg, ID_SC_BRSCAN);
    NG_SetChildSmallFont (hDlg, ID_SC_BRINF);
    NG_SetChildSmallFont (hDlg, ID_SC_BRCLN);
    NG_SetChildSmallFont (hDlg, ID_SC_FILSCAN);
    NG_SetChildSmallFont (hDlg, ID_SC_FILINF);
    NG_SetChildSmallFont (hDlg, ID_SC_FILCLN);
    NG_SetChildSmallFont (hDlg, ID_SC_FILQUAR);
    NG_SetChildSmallFont (hDlg, ID_SC_FILDEL);
    NG_SetGroupBoxSunken (hDlg, ID_SC_BOX1);
    NG_SetGroupBoxSunken (hDlg, ID_SC_BOX2);

#ifdef NAVSCANW
                                        //&? DKA: Change next check for Win ver
                                        // with updated GUI?  (W95 & NT4.0?)
    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        NG_SetGroupBoxSunken (hDlg, ID_SC_BOX3);
    else
        ShowWindow(GetDlgItem(hDlg, ID_SC_BOX3), SW_HIDE);
#endif

    // Hide the MBR / BR controls from the summary dialog when
    // we are on NEC.
    if (HWIsNEC())
        {
        MyHideMbrBrControls (hDlg);
        }

                                        // ----------------------------------
                                        // Initialize fillbars
                                        // ----------------------------------
    fbStyle.nStruclen         = sizeof(FILLBARSTYLE);
    fbStyle.b3D               = TRUE;
    fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
    fbStyle.nRightBottomWidth = FB_USEDEFAULT;
    fbStyle.crColor           = FB_USEDEFAULT;

    Fillbar_SetRange(GetDlgItem(hDlg, ID_SC_FILLBAR), 0, 100);
    Fillbar_Show(GetDlgItem(hDlg, ID_SC_FILLBAR), &fbStyle);

                                        // -----------------------------------
                                        // Indicate what we're doing.
                                        // -----------------------------------
    if (lpNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY)
        {
        Static_SetText (GetDlgItem(hDlg, ID_SC_WORKINGON),
                        String_Ptr(IDS_SCANDLG_INOCULATING));
        }
    else if (lpNav->lpScan->bMemOnly)
        {
        Static_SetText (GetDlgItem(hDlg, ID_SC_WORKINGON),
                        String_Ptr(IDS_SCANDLG_SCANING));
        }
    else
        {
        Static_SetText (GetDlgItem(hDlg, ID_SC_WORKINGON),
                        String_Ptr(IDS_SCANDLG_SCANINGFILES));
        }

                                        // -----------------------------------
                                        // If we're not allowed to stop scannig
                                        // deactivate CLOSE from system menu.
                                        // -----------------------------------
    if (lpNav->Opt.bAllowScanStop == FALSE)
        {
        EnableWindow ( GetDlgItem(hDlg, ID_SC_STOP), FALSE);
        EnableMenuItem (GetSystemMenu(hDlg, FALSE), SC_CLOSE,
                    MF_BYCOMMAND | MF_GRAYED);
        }
    else
        {
        SetFocus ( GetDlgItem(hDlg, ID_SC_STOP) );
        }

    ScanAnimationStart ( hDlg );

                                        // ----------------------------------
                                        // Dissable the Quarantine controls
                                        // if Quarantine is not enabled.
                                        // ----------------------------------
#ifndef SYM_DOS
        if( !NAV_IS_QUARANTINE() )
            {
            EnableWindow(GetDlgItem(hDlg, ID_SS_QUARTXT), FALSE);
            EnableWindow(GetDlgItem(hDlg, ID_SC_FILQUAR), FALSE);
            EnableWindow(GetDlgItem(hDlg, ID_SC_MBQUAR), FALSE);
            EnableWindow(GetDlgItem(hDlg, ID_SC_BRQUAR), FALSE);
            }
#endif

                                        // ------------------------------
                                        // Decide how to show scan dialog
                                        // and hide/minimize/move parent.
                                        // ------------------------------
    switch (lpNav->lpScan->uProgramMode)
        {
                                        // Interactive mode or NAVUI mode
                                        // should be normal scan dialog with
                                        // main window minimized if taskbar
                                        // present, or hidden otherwise.
        case PROGRAM_NAVUILAUNCH:
        case PROGRAM_INTERACTIVE:
            *lpbPaintProcessed = FALSE;
            ShowWindow(GetParent(hDlg),
                       SystemWin95ShellPresent() ? SW_SHOWMINNOACTIVE : SW_HIDE);

            ShowWindow(hDlg, SW_SHOWNORMAL);
            UpdateWindow(hDlg);
            break;

                                        // Command line and download modes
                                        // should hide the scan dialog
                                        // and show a minimized main window.
        case PROGRAM_COMMANDLINE:
        case PROGRAM_AUTOMATED:
        case PROGRAM_DOWNLOAD:
        default:
            *lpbPaintProcessed = TRUE;

            ShowWindow(hDlg, SW_HIDE);
            UpdateWindow(hDlg);

            PostMessage(hDlg, WM_NAVWORKUSER, 0, 0);

                                        // DALLEE: Trust me on this --
                                        // SetActiveWindow() ensures we
                                        // show up alt-tab'ing in NT 3.51.
            SetActiveWindow(GetParent(hDlg));
            ShowWindow(GetParent(hDlg), SW_MINIMIZE);
            break;
        }

    DrawNavIcon (hDlg, 0);

    return (MAKELPARAM(hDlg, 0));
} // DoInitialization()


//************************************************************************
// NavMainWndWM_CTLCOLOR()
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
// 12/21/92 MSL Function created.
//************************************************************************

LRESULT LOCAL NavMainWndWM_CTLCOLOR(    // Handles WM_CTLCOLOR for main wnd
    HWND        hWnd,                   // [in] Window handle
    WPARAM      wParam,                 // wParam of WM_CTLCOLOR message
    LPARAM      lParam)                 // lParam of WM_CTLCOLOR message
{
    LRESULT     lResult;
    HBRUSH      hBrush;

                                        // See if CTL3D.DLL has a brush
                                        // for our control to use.
    hBrush = Ctl3dCtlColorEx( WM_CTLCOLOR, wParam, lParam);
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
} // NavMainWndWM_CTLCOLOR()


//************************************************************************
// NavWorkInit()
//
// First "initialization" routine called for scan dialog.
// See also DoInitialization().
//
// Allocates and initializes lpNav structure referenced everywhere.
// Reads options into lpNav and kluges some based on
// interactive/command line/download mode.
//
// Parameters:
//      HWND            hDlg        Handle of scan dialog.
//      LPSCANSTRUCT    lpScan      Initial info for this scan.
//
// Returns:
//      TRUE        on success.
//      FALSE       failure -- need to abort scan.
//************************************************************************
// 08/30/1996 DALLEE, commented, made readable.
//************************************************************************

BOOL LOCAL NavWorkInit (HWND hDlg, LPSCANSTRUCT lpScan)
{
    auto    BOOL        bStatus = FALSE;
    auto    HGLOBAL     hNonav;
    auto    LPNONAV     lpNav;
    auto    TCHAR       szTempPath[SYM_MAX_PATH*2] = {0};


    // ********************************************************** //
    // Allocate a block of memory the size of our nonav structure //
    // ********************************************************** //

    if ((hNonav = MemAlloc(GHND, sizeof(NONAV))) != NULL)
        {
        bStatus = TRUE;
        lpNav = (LPNONAV)MemLock (hNonav);

                                        // ---------------------------------
                                        // For NT logouts, save a handle:
                                        // ---------------------------------
        GloballySaved_lpNav = lpNav;

        LP_SET(hDlg, lpNav);
                                        // ---------------------------------
                                        // Obtain relevant information from
                                        // the INI file
                                        // ---------------------------------
        ReadIniScanner (hInstance, lpNav, lpScan->lpNavOptions);

        lpNav->lpScan = lpScan;

        switch (lpScan->uProgramMode)
            {
            case PROGRAM_AUTOMATED:     // Automated
                lpNav->Opt.bPrestoNotify = FALSE;

                // ...fall through to commandline overrides...

            case PROGRAM_COMMANDLINE:   // Regular command line mode.
                if (lpScan->bMemOnly == TRUE)
                    lpNav->Opt.bMemory = TRUE;
                else if (lpScan->uScanMemory != SWITCH_DONTCHANGE)
                    lpNav->Opt.bMemory = lpScan->uScanMemory;

                if (lpScan->bBootOnly == TRUE)
                    {
                    lpNav->Opt.bMasterBootRec = TRUE;
                    lpNav->Opt.bBootRecs      = TRUE;
                    }
                else if (lpScan->uScanBoot != SWITCH_DONTCHANGE)
                    {
                    lpNav->Opt.bMasterBootRec = lpScan->uScanBoot;
                    lpNav->Opt.bBootRecs      = lpScan->uScanBoot;
                    }
                break;

                                        // Web-browser command line mode.
            case PROGRAM_DOWNLOAD:
                lpNav->Inoc.bUseSystemInoc = FALSE;
                lpNav->Inoc.bUseInoc = FALSE;

                lpNav->Gen.bScanZipFiles = TRUE;

                lpNav->Opt.bMemory = FALSE;
                lpNav->Opt.bMasterBootRec = FALSE;
                lpNav->Opt.bBootRecs = FALSE;

                                        // Automatically display these buttons:
                lpNav->Opt.bDispRepair = TRUE;

                lpNav->Opt.uAction = SCAN_DOWNLOADPROMPT;
                lpNav->Opt.bPrestoNotify = TRUE;

                                        // Don't generate a backup .VIR
                                        // file when saving a download:
                lpNav->Gen.bBackupRep = FALSE;

                                        // Scan all files, so that a
                                        // 0-exe zip will still be scanned:
                lpNav->Opt.bScanAll = TRUE;

                                        // Don't log anything for the start or end of scans:
                lpNav->Act.bStartEnd = FALSE;
                break;

            default:
                // Just use our normal options.
                break;
            }
		
        // Read in the Scan Files With No Extension value from NAVOPTX.DAT
        if ( GetPath(szTempPath, sizeof(szTempPath) / sizeof(szTempPath[0])) )
        {
            _tcscat(szTempPath, NAVOPTX_FILE);

            lpNav->ExtendedOpts.bScanNoExtFile = 
                  GetPrivateProfileInt(NAVOPTX_SECT_SCANOPTS,
                                       NAVOPTX_SCAN_NOEXT_FILES,
                                       0,
                                       szTempPath);
        }
	    else
            lpNav->ExtendedOpts.bScanNoExtFile = FALSE;

#ifdef _ALPHA_
                                        // No ZIP scan on DEC currently
        lpNav->Gen.bScanZipFiles = FALSE;
#endif

        lpNav->hNonav = hNonav;

        // *********************************************** //
        // Initialize all the network specific information //
        // *********************************************** //
        if (lpNav->Alert.bAlertNetUsr)
            lpNav->lpNetUserItem = lpScan->lpNavOptions->netUser.lpNetUserItem;


        // *********************************************** //
        // Get Exclusions Info                             //
        // *********************************************** //
        lpNav->lpExclude = &lpScan->lpNavOptions->exclude;
 

        // *********************************************** //
        // Log Scan Start                                  //
        // *********************************************** //

        LOGSTRUCTEX   Log = { 0 };

        Log.uEvent = LOGEV_SCANSTART;
        Log.lpFileName   = lpNav->Infected.szFullPath;
        Log.lpVirusName  = NULL;
        Log.lpAct        = &lpNav->Act;
        Log.lpNoNav      = reinterpret_cast<LPVOID>(lpNav);

        if (LogEventEx (&Log))
            NotifyLogWriteError (lpNav->lpScan->hDlg, lpNav);

        }

    return (bStatus);
} // NavWorkInit()


BOOL LOCAL NavWorkEnd(HWND hDlg)
{
    auto    BOOL            bStatus = FALSE;
    auto    LPNONAV         lpNav = NULL;
    auto    HGLOBAL         hNonav;
    auto    NOTIFYSTRUCT    Notify;

    auto    DWORD           dwWizardResult = 0;
    auto    BOOL            bWizardWasUsed = FALSE;

    auto    char            szNotifyBodyText [ SYM_MAX_PATH+1 ];
    auto    char            szNotifyCaption  [ SYM_MAX_PATH+1 ];

                                        // Updates the scan statistics
    UpdateScanStatistics(hDlg);

                                        // Get LPNONAV
    lpNav = (LPNONAV)LP_GET(hDlg);
    if (!lpNav)
        {
        LP_SET (hDlg, GloballySaved_lpNav);
        lpNav = (LPNONAV)LP_GET(hDlg);
        }

    if (lpNav)
        {
        bStatus = TRUE;

        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            {                           // Running in download mode ...
                                        // Log what happened :
            if (lpNav->lpInfArr)
                {
                LongCustomMessage(hDlg);
                if (lpNav->lpScan->bDeleteZipFile == TRUE)
                    {
                                        // If we aborted a zip, we need to doctor the
                                        // information slightly so that the Activity
                                        // log functions will accept it ...
                    auto LPINFARR lpInfArr = (LPINFARR)(lpNav->lpInfArr);

                    if (lpInfArr->hInf)
                        {
                        auto LPINFECTEDFILE lpInfected = (LPINFECTEDFILE)MemLock (lpInfArr->hInf);

                        if (lpInfArr->uNumUsed > 0)
                            {
                            STRCPY (lpInfected[0].szFullPath, lpNav->lpScan->szSaveFileName);

                                        // Set the proper file status for logging:
                            lpInfected[0].wStatus = FILESTATUS_DELETED;
                            }
                        }
                    }

                if (WriteNavLogs (hDlg, lpNav))
                    bStatus = FALSE;
                }
            }
        else if (lpNav->lpScan->uProgramMode == PROGRAM_AUTOMATED)
            {
            //
            // In the AUTOMATED case, there is no UI, so write to
            // activity log directly instead of creating a dialog and
            // writing from the dialog which waits for an user to logon
            // to the system.
            //
            if (lpNav->lpInfArr)
                {
                    if ( lpNav->lpInfArr->uNumUsed )
                    {
                        // Write to the activity log
                        if ( WriteAllFileLogRecords ( &lpNav->Act,
                                                lpNav->lpInfArr,
                                                NULL,
                                                (DWORD) lpNav ) )
                        {
                            NotifyLogWriteError ( hDlg, lpNav ) ;
                        }
                    }
                }
            }
        else                            // All other (interactive) cases
            {
                                        // If there were any problems found:
            if (lpNav->lpInfArr)
                {
                if ( FALSE == lpNav->Opt.bPrestoNotify && FALSE == IsCutomNotify(lpNav))
                    {
                    if ( ( lpNav->Opt.uAction == SCAN_PROMPT ) &&
                         ( lpNav->Opt.bDispRepair || lpNav->Opt.bDispDelete ) &&
                         IsStatusTypeInList( lpNav, ST_LIVEVIRUS )  &&
                         !BypassRepairWizard() )
                                        // Enter here ONLY IF all of these are TRUE :
                                        // 1) "How To Respond When A Virus Is Found" is
                                        //    set to "Prompt";
                                        // 2) The "Repair" and/or "Delete" buttons are
                                        //    enabled (on the Scanner Options tab)
                                        // 3) Live viruses have been detected in the scan;
                                        // 4) The Reg key which bypasses the Repair Wizard
                                        //    is not found.
                        {
                        LongCustomMessage(hDlg);
                        LaunchCleanupWizard(hDlg, lpNav, REPAIRWIZARD_DONTPANIC, &dwWizardResult);

                        if ( dwWizardResult & RWIZFLAG_USEADVANCED )
                            {
                            // Call ShowScanResDlg() below to show
                            // Problems Found dialog.
                            }
                        else
                            {
                            // Indicate wizard was used to prevent display
                            // of Problems Found and Scan Results dialogs.
                            bWizardWasUsed = TRUE;
                            }
                        }
                    }

                if ( !bWizardWasUsed )
                    {
                    if ( FALSE == lpNav->Opt.bPrestoNotify )
                        {
                                        // Problems Found dialog
                        LongCustomMessage(hDlg);
                        ShowScanResDlg( hDlg, lpNav, FALSE );
                        }
                                        // Summary of Scan dialog
                    ShowScanSumDlg( hDlg, lpNav );
                    }

                if ( WriteNavLogs( hDlg, lpNav) )
                    bStatus = FALSE;

                ShowWindow (lpNav->lpScan->hDlg, SW_RESTORE);
                SetForegroundWindow(s_hwndOwner);
                }
                                        // Otherwise, no problems found;
                                        // Show summary if interactive scan.
            else if ( (PROGRAM_INTERACTIVE == lpNav->lpScan->uProgramMode) ||
                      (PROGRAM_NAVUILAUNCH == lpNav->lpScan->uProgramMode) )
                {
                ShowScanSumDlg( hDlg, lpNav );

                ShowWindow( hDlg, SW_HIDE );
                ShowWindow( lpNav->lpScan->hDlg, SW_RESTORE );
                }
            }

                                        // If we were launched by Norton File
                                        // Manager or for right click scanning
                                        // and no viruses were found, then
                                        // we should display positive feedback
                                        // that no viruses were found.

        if ( lpNav->lpScan->uProgramMode != PROGRAM_AUTOMATED &&
             lpNav->lpScan->bHMemLaunch &&
             lpNav->bDoScan &&
             lpNav->lpScan->wCallWhenDone &&
             (NULL == lpNav->lpInfArr) )
            {
            LoadString( hInstance,
                        IDS_NOVIRUSESDETECTED,
                        szNotifyBodyText,
                        sizeof(szNotifyBodyText) );

            LoadString( hInstance,
                        IDS_NAV,
                        szNotifyCaption,
                        sizeof(szNotifyCaption) );

            MessageBox( hDlg,
                        szNotifyBodyText,
                        szNotifyCaption,
                        MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND );
            }

        SmartInocClose();
        lpNav->uInocOpenOK = SMARTOPEN_CLOSED;


        if (lpNav->bExcludeDirty)       // Save exclusions, if changed.
            {                           // Don't free memory.
            if (NOERR == ExcludeTerminate(lpNav->lpExclude, FALSE))
                {
                lpNav->bExcludeDirty = FALSE;
                }
            else
                {
                MEMSET (&Notify, EOS, sizeof(Notify));
                Notify.hParent       = hDlg;
                Notify.uMsgID        = IDS_EXCLUDEERR;
                Notify.uTitleID      = IDS_NAV;
                if ( GetCustomVersion() == VERSION_NETSCAPE )
                    {                  // Title is "Internet Scanner"; set at runtime.
                    Notify.uTitleID      = IDS_NAV_INTERNET;
                    }
                Notify.hInstance     = hInstance;
                NotifyUser (&Notify);
                }
            }

                                        // ----------------------------------
                                        // If status is still OK then there's
                                        // probably available disk space to
                                        // write the activity log file.
                                        // ----------------------------------
        if ( bStatus == TRUE )
            {
            MEMSET (lpNav->Infected.szFullPath, EOS,
                    sizeof(lpNav->Infected.szFullPath));

            LPSTR lpDrives = (LPSTR)MemLock(lpNav->lpScan->hScanList);

            SYM_ASSERT ( lpDrives );

            BuildWhatScanned (
                             lpDrives,
                             lpNav->Infected.szFullPath,
                             sizeof(lpNav->Infected.szFullPath)
                             );

            StampScanTimeInRegistry (lpNav, lpDrives);

            MemUnlock(lpNav->lpScan->hScanList);

                                        // Log the end of the scan, or
                                        // that it was interrupted:
            LOGSTRUCTEX   Log = { 0 };


            if (lpNav->lpScan->uProgramMode == PROGRAM_COMMANDLINE &&
                (lpNav->lpScan->bBootOnly == TRUE || lpNav->lpScan->bMemOnly == TRUE))
                Log.uEvent = LOGEV_SCANEND;
            else if ( lpNav->bDoScan )
                Log.uEvent = LOGEV_SCANEND;
            else
                Log.uEvent = LOGEV_SCANINTERRUPTED;

            Log.lpFileName   = lpNav->Infected.szFullPath;
            Log.lpVirusName  = NULL;
            Log.lpAct        = &lpNav->Act;
            Log.lpNoNav      = reinterpret_cast<LPVOID>(lpNav);

            if (LogEventEx (&Log))
                NotifyLogWriteError (lpNav->lpScan->hDlg, lpNav);
            }                           // End "if (bStatus == TRUE) ..."

        FreeLinkList(lpNav);

                                        // -----------------------------------
                                        // Indicate that we're done with
                                        // scanning and are ready for another
                                        // scanning session.
                                        // -----------------------------------

        if ( lpNav->lpScan->lpBoolScanning )
            *(lpNav->lpScan->lpBoolScanning) = FALSE;

                                        // -----------------------------------
                                        // Unlock and free the allocated memory
                                        // for our Nonav structure
                                        // -----------------------------------

        if (lpNav->lpScan->wCallWhenDone)
            FORWARD_WM_COMMAND ( lpNav->lpScan->hDlg,
                                 lpNav->lpScan->wCallWhenDone,
                                 0, 0, PostMessage );

                                        // -----------------------------------
                                        // Make sure that we indicate that we'll
                                        // accept drag/drop again
                                        // -----------------------------------
        DragAcceptFiles (lpNav->lpScan->hDlg, TRUE);


                                        // -----------------------------------
                                        // Forward quarantined files to the server
                                        // If Quarantine is enabled in this install.
                                        // -----------------------------------
        if( NAV_IS_QUARANTINE() )
            {
            ForwardFiles(lpNav->lpScan->hDlg, lpNav);
            }

                                        // -----------------------------------
                                        // Free all ScanStart()'s input
                                        // buffers.
                                        // -----------------------------------
        hNonav = lpNav->hNonav;

        MemRelease( lpNav->lpScan->hScanList );
        MemFree( lpNav->lpScan->hScanList );

        MemFreePtr( lpNav->lpScan->lpScanItems );

        MemFreePtr( lpNav->lpScan );

        MemUnlock( hNonav );
        MemFree( hNonav );

        LP_FREE( hDlg );
                                        // ---------------------------------
                                        // For NT logouts, NULL the handle:
                                        // ---------------------------------
        GloballySaved_lpNav = NULL;
        }

                                        // ---------------------------------
                                        // Make sure to toggle off static
                                        // flag that long custom message
                                        // has been displayed.
                                        // ---------------------------------
    if ( FALSE != GetShowMsg() )
        SetShowMsg();

    return (bStatus);
}


VOID SYM_EXPORT WINAPI SkipDrive (LPSTR FAR *lplp, DWORD dwUserData)
{
    (*lplp)[1] = '#';

    while ( **lplp != EOS )
        (*lplp) = AnsiNext (*lplp);

    (*lplp) = AnsiNext (*lplp);
}


UINT SYM_EXPORT WINAPI DriveError ( BYTE byDrive, DWORD dwUserData )
{
    auto    DRIVEPROBLEMSTRUCT  DriveAccess;
    auto    LPNONAV   lpNav;

    lpNav = (LPNONAV)dwUserData;

    MEMSET (&DriveAccess, EOS, sizeof(DriveAccess));
    DriveAccess.hParent         = lpNav->hMommyWindow;
    DriveAccess.byDrive         = byDrive;
    DriveAccess.bHideContinue   = TRUE;


#ifdef SYM_WIN32
    STATUS Status;

    Status = NavStatSetDriveState ( byDrive,
                                    0,
                                    DRIVESTATUS_FOREGROUND,
                                    NAV_APP_CLINIC );

    if (Status != NOERR )
    {
        DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT;
        NavStatGetDriveState ( DriveAccess.byDrive,
                               0,
                               DriveAccess.szWho,
                               sizeof (DriveAccess.szWho) );
    }
    else
#endif
    if ( VirusScanCanAccessDisk ( byDrive, DISK_READ_ONLY ) == FALSE )
        DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DISKLOCKED;
    else
        DriveAccess.uInfoMsgID = DRIVEACCESS_MSG_DRIVE;

#ifdef SYM_WIN32
    NavStatSetDriveState ( byDrive,
                           0,
                           DRIVESTATUS_FREE,
                           NAV_APP_CLINIC );
#endif

    if ( lpNav->Alert.bRemoveAfter )
        DriveAccess.uTimeOutSeconds = lpNav->Alert.uSeconds;

    return ( DriveAccessProblem (&DriveAccess) );
}


/*@API:*************************************************************************
@Declaration:
BOOL LOCAL DeleteSkippedItems (
    LPSTR   lpItemList )

@Description:
This function deletes all the skipped items from the list of items to scan.

&? add more klugery here for new input

@Parameters:
$lpItemList$ list of items to scan.

@Returns:
FALSE if there are items left to be scanned, TRUE if the list is empty.

@Compatibility: Win16 and Win32
*******************************************************************************/

BOOL LOCAL DeleteSkippedItems (         // Delete items that are skipped.
    LPSTR       lpItemList,             // [in] from this list.
    LPSCANITEM  lpScanItems )           // [in] and from this index.
{
    #define SKIPPED_ITEM    '#'
    auto    LPSTR           lpList = lpItemList;
    auto    LPSCANITEM      lpIndex;

    SYM_ASSERT ( lpList );
    SYM_ASSERT ( *lpList );
    SYM_ASSERT ( lpItemList == lpScanItems->lpszPathName );

                                        // -----------------------------------
                                        // While we have items to search through
                                        // -----------------------------------
    while ( *lpList )
        {
                                        // -----------------------------------
                                        // Should this item be skipped?
                                        // -----------------------------------
        if ( lpList[1] == SKIPPED_ITEM )
            {
                                        // -----------------------------------
                                        // Yep, so find where it ends.
                                        // -----------------------------------
            for ( UINT uMoveFrom = 0; lpList[uMoveFrom]; uMoveFrom++ )
                ;

            uMoveFrom++;

                                        // -----------------------------------
                                        // Is there another item after this?
                                        // -----------------------------------
            if ( lpList[uMoveFrom] )
                {
                                        // -----------------------------------
                                        // Yeah. Find number of chars to move.
                                        // -----------------------------------
                for ( UINT uNumToMove = uMoveFrom; ; uNumToMove++ )
                    {
                    if ( lpList[uNumToMove] == EOS &&
                         lpList[uNumToMove+1] == EOS )
                        break;
                    }

                uNumToMove -= uMoveFrom;

                                        // -----------------------------------
                                        // The +2 is to move the double NULL.
                                        // -----------------------------------
                uNumToMove++;
                uNumToMove++;

                MEMMOVE ( lpList, &lpList[uMoveFrom], uNumToMove );
                }
            else
                {
                                        // -----------------------------------
                                        // Nai. Zero out item.
                                        // -----------------------------------
                MEMSET ( lpList, EOS, uMoveFrom );
                }

                                        // -----------------------------------
                                        // Now update the index of scan items.
                                        // -----------------------------------
            for ( lpIndex = lpScanItems;
                  lpIndex->lpszPathName;
                  lpIndex++ )
                {
                // MEMCPY( lpIndex, lpIndex + 1, sizeof( *lpIndex ) );

                *lpIndex = *(lpIndex + 1);

                                        // Need to adjust the same as
                                        // lpList if not the NULL last entry.
                if ( lpIndex->lpszPathName )
                    {
                    lpIndex->lpszPathName -= uMoveFrom;
                    }
                }
            }
        else
            {
                                        // -----------------------------------
                                        // This item OK. Advance to next.
                                        // -----------------------------------
            lpScanItems++;
            lpList += ( STRLEN(lpList) + 1 );
            }
        }

    return ( STRLEN(lpItemList) ? FALSE : TRUE );
} // DeleteSkippedItems()


//***************************************************************************
// ScanAnimationStart()
//
// Description:
//      Initiates animation for the siren.  Loads up the bitmaps and masks
//      we'll need, as well as gets the show on the road for us.
//
// Parameters:
//      hDlg                            [in] Dialog box handle
//      lpAnimation                     [out] receives animation ptr
//
// See Also:
//      ScanAnimationEnd, ScanAnimationNextFrame, ScanAnimationDrawFrame,
//      ScanAnimationEnable
//
//***************************************************************************
// 07/18/1997 GWESTER Updated with new animation (3 frames only)
// 07/02/1995 MARKL Function Created.
//***************************************************************************
VOID LOCAL PASCAL ScanAnimationStart (      // Starts Animation
    HWND        hDlg
    )
{
    // Get animation control
    HWND hWndCtrl = GetDlgItem( hDlg, IDC_ANIMATION );
    SYM_ASSERT( hWndCtrl );

    int iResourceID = ( DisplayableColors() > 256 ) ? ID_SCAN_ANIMATION256 : ID_SCAN_ANIMATION16;


    Animate_Open( hWndCtrl, MAKEINTRESOURCE( iResourceID ) );


    // Start animation sequence
    Animate_Play( hWndCtrl, 0, -1, -1 );
}



//***************************************************************************
// ScanAnimationEnd()
//
// Description:
//      Terminates the animation, cleaning up all resources, memory, and
//      timer.
//
// Parameters:
//      hDlg                            [in] Dialog box handle
//
// See Also:
//      ScanAnimationStart, ScanAnimationDisable()
//
//***************************************************************************
// 07/02/1995 MARKL Function Created.
//***************************************************************************
VOID LOCAL PASCAL ScanAnimationEnd (
    HWND        hDlg
    )
{
    // Get animation control
    HWND hWndCtrl = GetDlgItem( hDlg, IDC_ANIMATION );
    SYM_ASSERT( hWndCtrl );

    // End the animation sequence
    Animate_Close( hWndCtrl );

}

VOID PASCAL ScanAnimationEnable (
    HWND        hDlg
    )
{
    // Get animation control
    HWND hWndCtrl = GetDlgItem( hDlg, IDC_ANIMATION );
    SYM_ASSERT( hWndCtrl );

    // Start animation sequence
    Animate_Play( hWndCtrl, 0, -1, -1 );
}

VOID PASCAL ScanAnimationDisable (
    HWND        hDlg
    )
{
    // Get animation control
    HWND hWndCtrl = GetDlgItem( hDlg, IDC_ANIMATION );
    SYM_ASSERT( hWndCtrl );

    // Stop playing for now.
    Animate_Stop( hWndCtrl );

}



//***************************************************************************
// StampScanTimeInRegistry()
//
// Description:
//      In NT, if all of the system's local drives were scanned, and the
//      scan was completed, update the LastScan value in the NT Registry.
//
//      It doesn't matter how the scan was launched. Since we have to
//      catch cases like command line-parameters "\A", "\L", "C: D: F:",
//      etc., as well as manual scans, the best way is to examine NAV's
//      list of examined drives when it finishes the scan.
//
//***************************************************************************
// 12/12/1995 GWESTER Function Created.
//***************************************************************************
VOID LOCAL PASCAL StampScanTimeInRegistry (
    LPNONAV     lpNav,
    LPSTR       lpDrives
    )
{
    auto    char    cDrive;
    auto    UINT    uStatus;
    auto    UINT    uMajor, uMinor;
    char    szWholeDrive [20];
    BOOL    bAllScanned = TRUE;

    if ( (!lpNav) || (!lpDrives) )
        return;

    if ( !(lpNav->lpScan) )
        return;

    for ( cDrive = 'A'; cDrive <= 'Z'; cDrive++ )
        {
        if ( DiskExists(cDrive) )
            {
            uStatus = DiskGetType(cDrive, &uMajor, &uMinor);

                                    // For a local disk we want all local
                                    // drives except for floppies.  Note
                                    // that TYPE_LOGICAL is for compressed
                                    // drives and fixes a defect.
            if (uStatus == NOERR &&
                (uMajor == TYPE_FIXED ||
                 uMajor == TYPE_RAMDISK ||
                 uMajor == TYPE_ENCRYPTED ||
                 uMajor == TYPE_LOGICAL))
                {
                                    // If you got here, cDrive is
                                    // a local drive that must be
                                    // scanned before the Registry
                                    // time stamp can be updated.

                                    // Make sure that the WHOLE
                                    // drive "i" (as opposed to just
                                    // one of it's folders) is in the
                                    // string lpNav->Infected.szFullPath,
                                    // which is NAV's way of recording
                                    // what it scanned:
                szWholeDrive[0] = cDrive;
                szWholeDrive[1] = ':';
                szWholeDrive[2] = '\\';
                szWholeDrive[3] = EOS;

                if (!IsDriveInListing ( lpDrives, szWholeDrive ))
                    {               // Drive was not fully scanned from
                                    // the root of the drive.
                    bAllScanned = FALSE;
                    break;
                    }
                }
            }
        }                               // End for loop

                                        // If lpNav->DoScan is TRUE, the
                                        // scan was completed.
                                        // Also, the bScanSubs flag must
                                        // always be TRUE.
    if ( bAllScanned && lpNav->bDoScan && lpNav->lpScan->bScanSubs || 
         RegistryCheckLastScanUpdateKey() )
        {                               // Update the Registry
                                        // timestamp.
        SYSTEMTIME rTimeStamp;
        LPSTR lpOneDrive;
        char szDrives[MAX_PATH];
        ZeroMemory( szDrives, MAX_PATH * sizeof(char) );


        for (lpOneDrive = lpDrives ; lpOneDrive != NULL && *lpOneDrive != EOS;
             lpOneDrive = SzzFindNext (lpOneDrive))
             {
             lstrcat(szDrives, lpOneDrive);
             lstrcat(szDrives, " ");
             }

                                        // System Time, NOT Local Time:
        GetSystemTime (&rTimeStamp);
        RegistryStampLastScanInfo ( (LPVOID)lpNav, szDrives, &rTimeStamp );

        }
}                                       // End StampScanTimeInRegistry()


//***************************************************************************
// IsDriveInListing()
//
// Description:
//      Looks for the string ThisDrive "X:\\" in the array of strings
//      AllDrives.  AllDrives is a series of strings, all separated
//      by null characters; it is terminated by an additional
//      null character.
//
//      Returns TRUE if ThisDrive was found, FALSE otherwise.
//
//***************************************************************************
// 12/16/1995 GWESTER Function Created.
//***************************************************************************
BOOL LOCAL PASCAL IsDriveInListing (
    LPSTR       lpAllDrives,
    LPSTR       lpThisDrive
    )
{
    LPSTR lpOneDrive;

    if ( (!lpAllDrives) || (!lpThisDrive) )
        return (FALSE);

    for (lpOneDrive = lpAllDrives ; lpOneDrive != NULL && *lpOneDrive != EOS;
         lpOneDrive = SzzFindNext (lpOneDrive))
        {
        if ( STRCMP (lpOneDrive, lpThisDrive) == 0 )
            return (TRUE);
        }

    return (FALSE);
}                                       // End IsDriveInListing()



/* **************************************************************************
@Name: BOOL LOCAL BypassRepairWizard ( VOID )

@Description:
Checks the Registry to see if the "NoRepairWizard" value is present. It doesn't matter
what kind of value it is, or what the value is set to; only whether it exists or not.

@Parameters:
    NONE

@Parameters:
    TRUE if the value is found
    FALSE otherwise

************************************************************************** */
BOOL LOCAL BypassRepairWizard ( VOID )
{
    char szRegValue [SYM_MAX_PATH];
    char szValueName [SYM_MAX_PATH];

    if (LoadString (hInstance, IDS_REPWIZ_BYPASSREGKEY, szValueName, sizeof(szValueName)))
        {
        CFGREGKEY cfg;
        cfg.hkey = HKEY_LOCAL_MACHINE;
        cfg.lpszProduct = NAV_INSTALLKEY_PRODUCT;
        cfg.lpszVersion = NAV_MODULE_CLINIC;
        cfg.lpszProject = NULL;
        cfg.lpszSection = NULL;
        cfg.lpszEntry = NULL;
        cfg.lpszName = szValueName;
        cfg.lpszSwitch = NULL;

        auto LONG  lRegResult;
        auto DWORD dwType, dwValueSize;
        dwType = REG_SZ;
        dwValueSize = sizeof(szRegValue);

        lRegResult =
            ConfigRegQueryValue ( &cfg, NULL, &dwType, (LPBYTE)szRegValue, &dwValueSize );

        if (lRegResult == ERROR_SUCCESS)
            {
            return TRUE;
            }
        }                               // End LoadString() succeeded

    return FALSE;
}                                       // End BypassRepairWizard()


//************************************************************************
// NavWorkUser()
//
// Respond to custom message WM_NAVWORKUSER in scan dialog.
// This routine is responsible for launching all the scanning that happens
// here.
// Also does some kluging and saving of options that don't apply.
//
// Parameters:
//      Pass through all params from the scan dialog proc.
//
// Returns:
//      Nothing.
//************************************************************************
// 06/28/1996 ??GWESTER moved from scandialog proc?
// 06/28/1996 DALLEE, fine comments you see here.
//************************************************************************

void LOCAL NavWorkUser(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    static  BOOL            bLastScanHigh = FALSE;

    auto    LPNONAV         lpNav;
    auto    UINT            uStatus = 0;
    auto    BOOL            bUseSystemInoc;
    auto    BOOL            bMasterBootRec;

    extern  BOOL bMemoryScanned;

    UpdateWindow(hDlg);

    lpNav = (LPNONAV)LP_GET(hDlg);

                                          // Verify lpNav if this is NT
    if (!lpNav)
        {
        LP_SET (hDlg, GloballySaved_lpNav);
        lpNav = (LPNONAV)LP_GET(hDlg);
        SYM_ASSERT (lpNav);
        }

    bUseSystemInoc = lpNav->Inoc.bUseSystemInoc;
    bMasterBootRec = lpNav->Opt.bMasterBootRec;

                                // If No fixed disk, disable these Options
    if (!CheckForFixedDisk())
        {
        lpNav->Inoc.bUseSystemInoc = FALSE;
        lpNav->Opt.bMasterBootRec = FALSE;
        }

    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
        {
        lpNav->lpScan->bFileDeleted = FALSE;  // Assume not deleted.
        }

    // If the options have changed, update our flag
    if ((lpNav->Opt.bMemory == FALSE) || (lpNav->Gen.bScanHighMemory != bLastScanHigh))
        bMemoryScanned = FALSE;
                                // -----------------------------------
                                // If Memory Scan or Boot Record Scan
                                // Options are turned on, perform them.
                                // -----------------------------------
    if (lpNav->Opt.bMemory ||
        lpNav->Opt.bBootRecs ||
        lpNav->Opt.bMasterBootRec ||
        lpNav->lpScan->bMemOnly ||
        lpNav->lpScan->bBootOnly)
        {
        bLastScanHigh = lpNav->Gen.bScanHighMemory;

        if ( (PROGRAM_INTERACTIVE == lpNav->lpScan->uProgramMode) ||
             (PROGRAM_NAVUILAUNCH == lpNav->lpScan->uProgramMode) )
            {
            uStatus = MemScanDlg (hDlg, lpNav);
            }
        else
            {
                                // -----------------------------------
                                // Otherwise perform the memory and
                                // boot scanning without displaying
                                // anything to the user.
                                // -----------------------------------

                                // No memory scan in NT.
            if (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT)
                {
                if ( lpNav->lpScan->bBootOnly == FALSE )
                    {
                    if ( (lpNav->Opt.bMemory == TRUE &&
                            bMemoryScanned == FALSE) ||
                         lpNav->lpScan->bMemOnly == TRUE )
                        {
                        bMemoryScanned = TRUE;

                                // -----------------------------------
                                // Update icon to show that we're doing
                                // something.
                                // -----------------------------------
                        DrawNavIcon (hDlg, 0);

                        TSR_OFF
                        lpNav->Infected.uVirusSigIndex = VirusScanMemory(
                                        &lpNav->Infected.Notes,
                                        lpNav->Gen.bScanHighMemory);
                        uStatus = lpNav->Infected.uVirusSigIndex
                                        ? TRUE
                                        : FALSE;
                        TSR_ON

                        if (lpNav->Infected.uVirusSigIndex)
                            ReportVirusInMem (hDlg, lpNav);
                        }
                    }
                }

            if ( uStatus == FALSE && lpNav->lpScan->bMemOnly == FALSE )
                {
                if (lpNav->Opt.bMasterBootRec == TRUE ||
                    lpNav->lpScan->bBootOnly == TRUE)
                    {
                                // -----------------------------------
                                // Update icon to show that we're doing
                                // something.
                                // -----------------------------------
                    DrawNavIcon (hDlg, 0);

                    uStatus = ScanMasterBootRecord (hDlg, lpNav, 0);
                    }

                if (!uStatus )
                    {
                    if (lpNav->Opt.bBootRecs == TRUE ||
                        lpNav->lpScan->bBootOnly == TRUE)
                        {
                                // -----------------------------------
                                // Update icon to show that we're doing
                                // something.
                                // -----------------------------------
                        DrawNavIcon (hDlg, 0);

                        uStatus = ScanBootRecord (hDlg, lpNav, 0);

                                // -----------------------------------
                                // Boot record was scanned. Now remove
                                // all the items that could not be
                                // scanned if the return status is 0
                                // This indicates that scanning should
                                // proceed.
                                // -----------------------------------

                        if ( !uStatus )
                            {
                            LPSTR lpScanList = (LPSTR)MemLock (
                                            lpNav->lpScan->hScanList
                                            );

                            SYM_ASSERT ( lpScanList );

                            uStatus = DeleteSkippedItems ( lpScanList,
                                                           lpNav->lpScan->lpScanItems );

                            MemUnlock ( lpNav->lpScan->hScanList );
                            }
                        }
                    }
                }
            }
        }
                                // ----------------------------------
                                // If memory and boot scanning
                                // has been done, check the
                                // system integrity
                                // ----------------------------------
#ifndef NAVSCANW
    if ( uStatus == FALSE && lpNav->lpScan->bMemOnly == FALSE &&
            lpNav->lpScan->bBootOnly == FALSE)
        {
        if (lpNav->Inoc.bUseSystemInoc == TRUE)
            {
#ifdef SYM_WIN32
            SetFileApisToOEM ();
#endif
            if (!lpNav->lpScan->wCallWhenDone)
                uStatus = !Integrity (hDlg, lpNav);
            else
                DoWholeIntegScenario (hDlg, lpNav, FALSE);
#ifdef SYM_WIN32
            SetFileApisToANSI ();
#endif
            }
        }
#endif

    if ( uStatus == FALSE && lpNav->lpScan->bMemOnly == FALSE &&
            lpNav->lpScan->bBootOnly == FALSE)
        {
        uStatus = 0;
                                // ----------------------------------
                                // Note that it is important that the
                                // memory is being scanned prior we
                                // access any disk.
                                // ----------------------------------
        lpNav->hMommyWindow = hDlg;

        {
                                // ----------------------------------
                                // This block contains slightly changed
                                // code from NAV 3.0 due to enhancements
                                // made on GetBytesToScan() function.
                                // ----------------------------------
        LPSTR lpScanList = (LPSTR)MemLock (lpNav->lpScan->hScanList);
        SYM_ASSERT ( lpScanList );

        lpNav->dwTotalKBytes = GetBytesToScan (
                        lpScanList,
                        lpNav->lpScan->bScanSubs,
                        SkipDrive,
                        DriveError,
                        (DWORD)lpNav
                        );

        uStatus = DeleteSkippedItems ( lpScanList,
                                       lpNav->lpScan->lpScanItems );

        // Make sure we are not attempting to scan network
        // drives if 'disable network drives' is set.
        if( lpNav->lpScan->lpNavOptions->scanner.bAllowNetScan == FALSE &&
            CheckForNetworkDrivesHack( hDlg, lpScanList ) == TRUE )
            {
            goto bailout;
            }


        MemUnlock ( lpNav->lpScan->hScanList );
        }

        if (!uStatus)
            {
                                // -----------------------------------
                                // Show status of memory and boot scan
                                // -----------------------------------

            ScanMemBootStat(hDlg, lpNav);

            lpNav->bDoScan = TRUE;
            lpNav->lpTimerProc = MakeProcInstance ((FARPROC)StatsTimer,
                                            hInstance);

                                // -------------------------------------
                                // ScanProc() is the file scanning loop.
                                // -------------------------------------

            ScanProc(hDlg, lpNav);

            if (lpNav->Alert.bBeep)
                {
                if (lpNav->Opt.bPrestoNotify == FALSE && FALSE == IsCutomNotify(lpNav))
                    {
                    if (lpNav->Stats.Files.uInfected      ||
                        lpNav->Stats.BootRecs.uInfected   ||
                        lpNav->Stats.MasterBootRec.uInfected )
                        {
                        MessageBeep ( MB_ICONASTERISK );
                        }
                    else
                        {
                        MessageBeep ( MB_ICONASTERISK );
                        }
                    }
                else
                    {
                                // Don't beep at the end if we are
                                // in download or automated mode!!!
                    if (lpNav->lpScan->uProgramMode != PROGRAM_DOWNLOAD &&
                        lpNav->lpScan->uProgramMode != PROGRAM_AUTOMATED)
                        MessageBeep ( MB_ICONASTERISK );
                    }
                }
            }


        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            TryToSaveTheFile(hDlg, lpNav);

        if (lpNav->lpTimerProc)
            {
            FreeProcInstance(lpNav->lpTimerProc);
            lpNav->lpTimerProc = NULL;
            }
        }

bailout:
    if (lpNav->bDoScan == TRUE)
        {
        Fillbar_SetLevel(GetDlgItem(hDlg, ID_SC_FILLBAR), 100);
        DrawNavIcon (hDlg, 100);
        }
    else
        DrawNavIcon (hDlg, 100);
                                    // Restore Scan Options
    lpNav->Inoc.bUseSystemInoc = bUseSystemInoc;
    lpNav->Opt.bMasterBootRec  = bMasterBootRec;

    SetForegroundWindow(hDlg);
    NavWorkEnd(hDlg);


    Fillbar_Close(GetDlgItem(hDlg, ID_SC_FILLBAR));
    DestroyWindow(hDlg);
} // NavWorkUser()


//***************************************************************************
// Description:
//   Hide the MBR and BR controls from the dialog. Also, move
//   the File information up to just below the Memory information.
//
// Parameters:
//   hDlg   [in] Dialog to rearrange.
//***************************************************************************
// 2/17/98 Mike Dunn -- Ported NEC code from ROSNEC so everything comes out
//                      looking correct on NECs.
//***************************************************************************

static void MyHideMbrBrControls (HWND hDlg)
{
    const  UINT uPadding      = 50 /*pixels*/;
    const  UINT uColPadding   = 65 /*pixels*/;
    auto   HWND hMBRLabel    = GetDlgItem (hDlg, ID_SC_MBRECTXT);
    auto   HWND hBrLabel     = GetDlgItem (hDlg, ID_SC_BRTXT);
    auto   HWND hFileLabel    = GetDlgItem (hDlg, ID_SC_FILESTXT);
    auto   HWND hFileScanned  = GetDlgItem (hDlg, ID_SC_FILSCAN);
    auto   HWND hFileRepaired = GetDlgItem (hDlg, ID_SC_FILCLN);
    auto   HWND hFileInfected = GetDlgItem (hDlg, ID_SC_FILINF);
    auto   HWND hFileQuar     = GetDlgItem (hDlg, ID_SC_FILQUAR);
    auto   HWND hFileDel      = GetDlgItem (hDlg, ID_SC_FILDEL);


    // Move the "Boot Record:" information to the left.

    AlignWnd (GetDlgItem (hDlg, ID_SC_BRTXT), hFileLabel,
              gAlignWndFollowX, hFileLabel, uPadding);
    
    AlignWnd (GetDlgItem (hDlg, ID_SC_BRSCAN), hFileScanned,
              gAlignWndFollowX, hFileScanned, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SC_BRINF), hFileInfected,
              gAlignWndFollowX, hFileInfected, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SC_BRCLN), hFileRepaired,
              gAlignWndFollowX, hFileRepaired, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SC_BRQUAR), hFileQuar,
              gAlignWndFollowX, hFileQuar, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SC_BRDEL), hFileDel,
              gAlignWndFollowX, hFileDel, uColPadding);

    // Hide the MBR infomation

    ShowWindow (GetDlgItem (hDlg, ID_SC_MBRECTXT), SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SC_MBSCAN),   SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SC_MBINF),    SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SC_MBCLN),    SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SC_MBDEL),    SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SC_MBQUAR),   SW_HIDE);
}


//***************************************************************************
// DisplayableColors()
//
// Description:
//      Returns information on the displayable colors for the caller to
//      determine the type of bitmap he wishes to display.
//
// Parameters:
//      NONE
//
// Return Value:
//      DWORD   reflecting the color depth
//
//***************************************************************************
// 07/18/1997 GWESTER Ported in from NAVW project
// 07/02/1995 MARKL Function Created.
//***************************************************************************
DWORD LOCAL DisplayableColors (
    VOID
    )
{
    HDC         hDC = GetDC ( GetDesktopWindow () );
    int         nColorDepth;
    DWORD       dwDisplayableColors;

    if ( GetDeviceCaps ( hDC, RASTERCAPS ) & RC_PALETTE )
        dwDisplayableColors = GetDeviceCaps ( hDC, SIZEPALETTE );
    else
        {
        nColorDepth = GetDeviceCaps ( hDC, BITSPIXEL ) *
                      GetDeviceCaps ( hDC, PLANES );
        if ( nColorDepth  >= 32 )
            dwDisplayableColors = 0xFFFFFFFF;
        else
            dwDisplayableColors = 1L << nColorDepth;
        }

    ReleaseDC ( GetDesktopWindow (), hDC );
    return ( dwDisplayableColors );
} // DisplayableColors()


//***************************************************************************
// CheckForNetworkDrivesHack()
//
// Description:
//      This routine enumerates a list of items to scan, looking for
//      references to network drives.  *NOTE* a better way of handling this
//      should eventually be implemented.  This is just a quick hack to
//      satisfy our feature hungry customers.
//
// Parameters:
//      HWND hWnd - Parent window
//      LPSTR lpScanList - List of items to scan
//
// Return Value:
//      BOOL   TRUE if we found a network reference in the Scan list
//             FALSE if not.
//
//***************************************************************************
// 7/25/97 DBUCHES - Function created
//***************************************************************************
BOOL LOCAL PASCAL CheckForNetworkDrivesHack( HWND hWnd, LPSTR lpScanList )
{
    auto    BOOL            bRet = FALSE;
    auto    NOTIFYSTRUCT    Notify;
    auto    LPSTR           lpList = lpScanList;
    auto    TCHAR           szTestBuffer[] = _T("x:\\");

    for (; *lpList ; lpList = SzzFindNext (lpList))
        {
        // If it's not a candidate, keep looking...

        if ( DBCSIsLeadByte(lpList[0]) )
            continue;

        // If we have a drive letter, check to see if it is a
        // network volume
        // If we have a UNC, assume it's a network volume

        if ( (lpList[0] == _T('\\') && lpList[1] == _T('\\'))
          || (!DBCSIsLeadByte(lpList[0]) && lpList[1] == _T(':')))
            {

            // Have drive letter -- check for network volume

            if ( lpList[1] == _T(':') )
                {
                szTestBuffer[0] = lpList[0];

                if( DRIVE_REMOTE != GetDriveType( szTestBuffer ) )
                    continue;           // Mistaken -- not a net drive
                }

            // Uh oh... found a network drive.  Inform the user
            // of the bad news...
            MEMSET (&Notify, EOS, sizeof(Notify));
            Notify.hParent       = hWnd;
            Notify.uMsgID        = IDS_ILLEGALNETWORKSCAN;
            Notify.uTitleID      = IDS_NAV;
            Notify.hInstance     = hInstance;
            Notify.lpFileName    = lpList;
            NotifyUser (&Notify);
            bRet = TRUE;
            break;
            }
        }

    return bRet;
}



//************************************************************************
// GetPath()
//
// Get the path of the NAV directory.
//
// Returns:
//      TRUE        on success.
//      FALSE       failure
//************************************************************************
// 10/5/1998 TCASHIN
//************************************************************************
BOOL LOCAL GetPath(TCHAR * szTemp, int size)
{
    auto HKEY   hKey = 0;
    auto BOOL   bRet = FALSE;

    MEMSET (szTemp, 0, size);

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_READ, &hKey))
    {
#ifdef NAVWNT
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAVNT"), 0, 0, (LPBYTE)szTemp, (LPDWORD)&size))
#else
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAV95"), 0, 0, (LPBYTE)szTemp, (LPDWORD)&size))
#endif
        {
            bRet = TRUE;
            RegCloseKey(hKey);
        }
    }

    return ( bRet );
}
