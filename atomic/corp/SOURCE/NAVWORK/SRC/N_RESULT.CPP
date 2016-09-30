// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_result.cpv   1.22   28 Jul 1998 19:23:34   tcashin  $
//
// Description:
//      Scan Results dialog and the repair/delete/exclude... dialogs
//      called from it.
//
// Contains:
//      ShowScanResDlg()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_result.cpv  $
// 
//    Rev 1.22   28 Jul 1998 19:23:34   tcashin
// Updated parameters in the call to MarkCompressedFilesQuarantined() in
// ScanResultsQuarantine(). Fixed some problems in MarkCompressedFilesQ()
// because we were trying to go through the infected items all wrong. Defect
// #119269.
// 
//    Rev 1.21   13 Jul 1998 18:35:56   tcashin
// Removed extra calls to NetAlert().
// 
//    Rev 1.20   10 Jul 1998 11:50:08   DBuches
// Removed unused help topics.
// 
//    Rev 1.19   02 Jul 1998 16:16:38   mdunn
// Commented out a stray undefined help ID.
// 
//    Rev 1.18   02 Jul 1998 15:13:00   mdunn
// fixed build 22 breakages on help IDs
// 
//    Rev 1.17   01 Jul 1998 16:37:10   tcashin
// Check for a null handle before calling TieldToWindows in the
// MarkCompressedFileQuarantined() function.
// 
//    Rev 1.16   01 Jul 1998 14:11:12   DBuches
// Removed How-To help.
// 
//    Rev 1.15   04 Jun 1998 11:14:54   tcashin
// Help button in quarantine all dialog was using the wrong ID.
// 
//    Rev 1.14   01 Jun 1998 16:10:28   tcashin
// Added support to quarantine infected compressed/zip files.
// 
//    Rev 1.13   21 May 1998 15:25:30   tcashin
// Added what's this help for the scan results quarantine button.
// 
//    Rev 1.12   18 May 1998 23:00:04   tcashin
// Count files deleted and quarantined. Fix the title in the Quarantine-All
// dialog box.
// 
//    Rev 1.11   27 Apr 1998 18:06:10   TCASHIN
// Added quarantine alerts.
// 
//    Rev 1.10   20 Apr 1998 23:08:34   tcashin
// Enable the quarantine button in the problems found dialog box.
// 
//    Rev 1.9   03 Apr 1998 13:07:46   tcashin
// Change #define to new one for quarantine.
// 
//    Rev 1.8   10 Aug 1997 22:53:08   DBUCHES
// Now, results dialog correctly handles custom response.
//
//    Rev 1.7   15 Jul 1997 14:42:04   DBUCHES
// Added Ctl3d support for DEC Alpha
//
//    Rev 1.6   13 Jun 1997 09:43:54   TIVANOV
// fix the disabled buttons in the PF dialog under quarantind
//
//    Rev 1.5   29 May 1997 11:38:52   TIVANOV
// fixing an internal compiler error - turn off the p-code generation, this is
// for debug compile only
//
//    Rev 1.4   22 May 1997 16:20:58   BILL
// Alpha changes (removed ctl3d)
//
//    Rev 1.3   22 May 1997 15:57:24   TIVANOV
// remove forgotten prototype AGAIN
//
//    Rev 1.1   02 May 1997 11:55:06   TIVANOV
// new quarantine functioality with the new quarantine button
//
//    Rev 1.0   06 Feb 1997 21:02:34   RFULLER
// Initial revision
//
//    Rev 1.10   09 Sep 1996 16:17:22   PVENKAT
// Ported Gryphon changes.
//
//
//
//      Removed the GroupSunken call in 'Details of Scan' and 'Problems Found' dlg.
//
//    Rev 1.9   07 Aug 1996 15:47:12   JBRENNA
// LuigiPlus Port:
//   DALLE: Bullet proof routines to get filestatus status and help strings.
//   Returns "" instead of trying to load string ID 0.
//
//
//    Rev 1.8   11 Jul 1996 08:03:36   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.7   02 Jul 1996 17:43:48   DJESCH
// Added a 'breal' after the WM_CTLCOLOR case.
//
//    Rev 1.6   01 Jul 1996 13:52:36   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.5   21 May 1996 11:52:12   GWESTER
// Added a parameter to ActUponFileStatus() for Repair Wizard.
//
//    Rev 1.4   09 May 1996 17:59:20   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.3   04 Apr 1996 16:51:22   YVANG
// Ported from Kirin.
//
//    Rev 1.3   03 Apr 1996 14:53:18   YVANG
// DBCS enable.
//
//    Rev 1.2   22 Feb 1996 14:08:02   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.1   14 Feb 1996 17:03:14   JWORDEN
// Process internal file names in OEM mode to allow scan, and repair, and
// innoculation of file names containing certain non-ANSI translatable
// characters.
//
//    Rev 1.0   30 Jan 1996 15:55:28   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:32   BARRY
// Initial revision.
//
//    Rev 1.56   17 Jan 1996 13:40:34   YVANG
// Deleted //&? MOREWORK comment.
//
//    Rev 1.55   16 Jan 1996 11:50:18   YVANG
// Changed NAVWNT to NAVSCANW.
//
//    Rev 1.54   30 Nov 1995 22:42:06   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.53   21 Nov 1995 13:08:36   YVANG
// For NAVWNT: Removed "How to" string RE: "Respond to inoculation changes"
// topic.
//
//    Rev 1.52   21 Nov 1995 12:50:12   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//
//    Rev 1.51   06 Nov 1995 13:51:40   DALLEE
// For NAVWNT: removed memory and inoculation text/buttons from dialogs.
//
//    Rev 1.50   02 Nov 1995 16:35:52   JWORDEN
// Convert MasterBootRec elements from booleans to counters
//
//    Rev 1.49   01 Nov 1995 12:25:08   JWORDEN
// Fix MBR file name on file info dialog
//
//    Rev 1.48   24 Oct 1995 19:31:44   JWORDEN
// Separate processing for boot records and MBR's
// Process MBR file name as a single byte drive number
//
//    Rev 1.47   14 Jul 1995 02:32:30   KEITH
// Add FILESTATUS_INOCULATEREPAIR_FAILED to the ScanResultDelete
// function so that these could actually be deleted.
//
//    Rev 1.46   11 Jul 1995 11:25:00   BARRY
// Removed ContextHelp from Delete All dialog; #39700. (Gary)
//
//    Rev 1.45   08 Jul 1995 17:56:42   BARRY
// Fixed ContextHelp link. (Gary)
//
//    Rev 1.44   09 Jun 1995 20:47:08   GWESTER
// Modified help links.
//
//    Rev 1.43   08 Jun 1995 19:29:12   KEITH
// Use the new NameDisplayShortenFileName for the "Problems Found" dialog
// the same as the rest. Also, change NameShortenFileNameForControl to
// wrap the text even if it does not contain a space.
//
//    Rev 1.42   08 Jun 1995 11:15:58   GWESTER
// Modified How To... help links.
//
//    Rev 1.41   09 May 1995 10:30:56   KEITH
// Correct some nouns
//
//    Rev 1.40   02 May 1995 20:39:08   MFALLEN
// Small help ID change for Elizabeth.
//
//    Rev 1.39   01 May 1995 13:21:18   MFALLEN
// Help fixes for Elizabeth
//
//    Rev 1.38   26 Apr 1995 20:01:46   MFALLEN
// After the user took an action on the file always set the focus to the listbox.
//
//    Rev 1.37   26 Apr 1995 18:12:28   BRAD
// Added NG_SetGroupBoxSunken()
//
//    Rev 1.36   25 Apr 1995 18:47:22   MFALLEN
// Added and changed help ids with Elizabeth.
//
//    Rev 1.35   25 Apr 1995 15:59:24   MFALLEN
// Exclude fail was not updating buttons correctly.
//
//    Rev 1.34   25 Apr 1995 10:37:16   MFALLEN
// Added 'What's this...' to all close buttons.
//
//    Rev 1.33   24 Apr 1995 18:56:50   BRAD
// Enabled Ctl3d
//
//    Rev 1.32   19 Apr 1995 10:15:08   MFALLEN
// Truncate status field filename.
//
//    Rev 1.31   17 Apr 1995 14:46:24   MFALLEN
// SYM_MAX_PATH + 1
//
//    Rev 1.30   17 Apr 1995 14:04:46   MFALLEN
// DISPLAY_DIALOG flag for special virus handling merged in from inline code.
//
//    Rev 1.29   11 Apr 1995 13:06:56   MFALLEN
// Buttons that are enabled/disabled had some logic problems.
//
//    Rev 1.28   06 Apr 1995 15:03:32   MFALLEN
// Show more of the infected zip filename.
//
//    Rev 1.27   03 Apr 1995 09:31:28   MFALLEN
// No What's this for help buttons.
//
//    Rev 1.26   28 Mar 1995 19:08:22   MFALLEN
// I think to have added the correct help IDs for the help button.
//
//    Rev 1.25   27 Mar 1995 18:46:56   MFALLEN
// Fixed help.
//
//    Rev 1.24   24 Mar 1995 17:16:04   MFALLEN
// Added help to Yes/No type dialog boxes.
//
//    Rev 1.23   15 Mar 1995 21:58:00   AMATIC
//
//    Rev 1.22   14 Mar 1995 18:30:02   AMATIC
// Added HowTo context help support.
//
//    Rev 1.21   17 Feb 1995 09:23:40   MFALLEN
// Only one return per windows callback function.
//
//    Rev 1.20   14 Feb 1995 12:09:26   AMATIC
// Added and fixed Context Help support.
//
//    Rev 1.19   13 Feb 1995 17:58:54   AMATIC
// Added context help - partialy.
//
//    Rev 1.18   06 Feb 1995 15:56:36   MFALLEN
// Trunkate LFNs
//
//    Rev 1.17   31 Jan 1995 14:08:00   MFALLEN
// Fixed with correct help ID's
//
//    Rev 1.16   09 Jan 1995 20:25:24   MFALLEN
// More right click help stuff.
//
//    Rev 1.15   09 Jan 1995 20:08:02   MFALLEN
// Right click help, not tested!
//
//    Rev 1.14   28 Dec 1994 08:54:28   MFALLEN
// Bad HIWORD(lParam) was being used instead of GET_WM_COMMAND_CMD()
//
//    Rev 1.13   22 Dec 1994 07:57:40   MFALLEN
// Added right click help. Help ID's are still missing.
//
//    Rev 1.12   20 Dec 1994 14:46:22   MFALLEN
// Sami did fixes on CERTLIBN
//
//    Rev 1.11   16 Dec 1994 14:20:46   MFALLEN
// Back using the old navxutil
//
//    Rev 1.10   01 Dec 1994 09:56:34   MFALLEN
// Minor changes to display filenames correctly.
//
//    Rev 1.9   30 Nov 1994 12:33:38   MFALLEN
// 3D in W16
//
//    Rev 1.8   29 Nov 1994 13:36:56   MFALLEN
// Compiles for W16
//
//    Rev 1.7   21 Nov 1994 13:07:02   MFALLEN
// Instead of commenting out all the functions that does not work, they have
// been stubbed out instead.
//
//    Rev 1.6   14 Nov 1994 12:51:20   MFALLEN
// Commented out all functions that deal with passwords, TSR enabeling and network
// alerts.
//
//    Rev 1.5   08 Nov 1994 15:42:08   MFALLEN
// Compiles fine under W32.
//
//
//    Rev 1.79   27 Oct 1994 00:17:52   DALLEE
// Don't allow boot repairs for defs with the DISPLAY_DIALOG flag set.
// These long repairs are for DOS only.
//
//    Rev 1.78   24 Oct 1994 12:55:20   DALLEE
// Check NO_GENERIC flag in N30NOTES.wControl2 before setting bUseInoc to
// call inoculation repairs.
//
//    Rev 1.77   05 Oct 1994 20:05:18   DALLEE
// Update statistics after repairing boot records.
//
//    Rev 1.76   11 Mar 1994 03:54:52   DALLEE
// Added password checking before registering files.
//
//    Rev 1.75   09 Mar 1994 15:57:44   DALLEE
// Fixed Reg button enable code for registration changed files.
//
//    Rev 1.74   04 Mar 1994 17:48:48   DALLEE
// Added support to exclude registration items from Problems Found dialog.
//
//    Rev 1.73   02 Mar 1994 19:51:32   DALLEE
// Added Register to the functions available from Problems Found dialog.
//************************************************************************

#include "platform.h"
#include "nunavstb.h"
#include "nonav.h"
#include "xapi.h"
#include "disk.h"
#include "ngui.h"
#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stdhelp.h"
#include "symcfg.h"

#pragma optimize( "p", off  )   // Turns off p-code
#include "navutil.h"
#pragma optimize( "p", on  )   // Turns on p-code

#include "exclude.h"
#include "options.h"
#include "navw_hlp.h"
#include "cmbstr.h"
#include "nunavdlg.h"
#include "security.h"

#include "stub.h"
#include "virscan.h"
#include "inoc.h"
#include "filestat.h"


//************************************************************************
// EXTERNS and GLOBALS
//************************************************************************

extern  HINSTANCE   hInstance;
static  BOOL        g_bDetails = FALSE;

//************************************************************************
// PROTOTYPES
//************************************************************************

VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the message
    UINT    uMsg,                       // [in] "message"
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
);


BOOL SYM_EXPORT WINAPI ScanResDlgProc    (HWND, UINT, WPARAM, LPARAM);

VOID LOCAL  ScanResultDelete        (HWND hDlg);
VOID LOCAL  ScanResultExclude       (HWND hDlg);
VOID LOCAL  ScanResultRepair        (HWND hDlg);
VOID LOCAL  ShrinkWindow            (HWND hDlg);
VOID LOCAL  ScanResultQuarantine    (HWND hDlg);
UINT LOCAL  GetActionStatus         (LPNONAV lpNav, LPINFECTEDFILE  lpInfected );



#ifndef NAVSCANW  // No Inoc in NT
VOID LOCAL  ScanResultInoculate     (HWND hDlg);
#endif


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

VOID SYM_EXPORT WINAPI ShowScanResDlg (
    HWND    hDlg,
    LPNONAV lpNav,
    BOOL    bDetails
    )
{
    extern  HINSTANCE   hInstance;
    auto    DLGPROC     lpfn;
    auto    int         nStatus;
                                        // -----------------------------------
                                        // Used to determine what help ID to
                                        // display when help button is pressed.
                                        // -----------------------------------
    g_bDetails = bDetails;

    lpfn = (DLGPROC) MakeProcInstance (
                                    (FARPROC)ScanResDlgProc,
                                    hInstance
                                    );

    if ( lpfn )
        {
        nStatus = DialogBoxParam (
                                hInstance,
                                MAKEINTRESOURCE ( (bDetails) ?
                                                  IDD_DETAILS : IDD_SCANRESDLG),
                                hDlg,
                                lpfn,
                                (LPARAM)lpNav
                                );

        FreeProcInstance((FARPROC) lpfn);
        }
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

BOOL SYM_EXPORT WINAPI ScanResDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    static  BOOL            bPaintProcessed = TRUE;
    auto    BOOL            bReturn = FALSE;
    auto    LPINFECTEDFILE  lpInfected;
    auto    NOTIFYSTRUCT    Notify;
    auto    LPNONAV         lpNav;
    auto    WORD            wIndex;
    auto    HWND            hListBox;

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
            break;

        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);

            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, ID_SR_LISTBOX);
            NG_SetChildSmallFont (hDlg, ID_SR_RAP);
            NG_SetChildSmallFont (hDlg, ID_SR_RAP2);
            lpNav = (LPNONAV)lParam;
            LP_SET  (hDlg, lpNav);

                                        // ------------------------------------
                                        // If the listbox is full, then
                                        // generate a timer message so
                                        // Windows has time to display the
                                        // existing information, and infor
                                        // the user that the listbox is full.
                                        // ------------------------------------

            bPaintProcessed = ShowInfected (
                                        GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                        lpNav
                                        );

            ListBox_SetCurSel(GetDlgItem(hDlg, ID_SR_LISTBOX), 0);

            if ( lpNav->Alert.bDispAlertMsg )
                SetDlgItemText(hDlg, ID_SR_ALERT, lpNav->Alert.szAlertMsg);
            else
                ShrinkWindow (hDlg);


            NAV_CenterDialog (hDlg);

            FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                            GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                            LBN_SELCHANGE, PostMessage );

            bReturn = TRUE;
            break;


        case WM_PAINT:
            if ( bPaintProcessed == FALSE )
                {
                bPaintProcessed = TRUE;

                FORWARD_WM_COMMAND ( hDlg, WM_NAVWORKUSER,
                                     0, 0, PostMessage );
                }

            break;

        case WM_NAVWORKUSER:
            UpdateWindow(hDlg);
            lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
            if (!lpNav)
                {
                LP_SET (hDlg, GloballySaved_lpNav);
                lpNav = (LPNONAV)LP_GET(hDlg);
                SYM_ASSERT (lpNav);
                }
#endif

            // ************************************************* //
            // The listbox has most likely reached it's maximum  //
            // limit. Inform the user to perform required action //
            // and re-start scanning.                            //
            // ************************************************* //

            MEMSET (&Notify, '\0', sizeof(Notify));
            Notify.hParent       = hDlg;
            Notify.uMsgID        = IDS_LISTBOXFULL;
            Notify.uTitleID      = IDS_NAV;
            if ( GetCustomVersion() == VERSION_NETSCAPE )
                {                   // Title is "Internet Scanner"; set at runtime.
            Notify.uTitleID      = IDS_NAV_INTERNET;
                }
            Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                                     lpNav->Alert.uSeconds : NULL;
            Notify.hInstance     = hInstance;

            NotifyUser (&Notify);

            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu,
                                        // or standard context help
                                        // ----------------------------------
            OnContextHelp ( hDlg, wMessage, wParam, lParam );
            break;
#endif

        case WM_COMMAND:
            lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
            if (!lpNav)
                {
                LP_SET (hDlg, GloballySaved_lpNav);
                lpNav = (LPNONAV)LP_GET(hDlg);
                SYM_ASSERT (lpNav);
                }
#endif

            switch ( GET_WM_COMMAND_ID ( wParam, lParam ) )
                {
                case ID_SR_LISTBOX:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_SELCHANGE)
                        {
                        hListBox = GET_WM_COMMAND_HWND ( wParam, lParam );
                        wIndex = ListBox_GetCurSel(hListBox);

                        if (wIndex != (WORD)LB_ERR)
                            {
                            lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (
                                                            hListBox,
                                                            wIndex
                                                            );

                            ActUponFileStatus (hDlg, lpNav, lpInfected, 80);
                            }
                        }
                    else if (GET_WM_COMMAND_CMD(wParam,lParam) == LBN_DBLCLK)
                        {
                        FORWARD_WM_COMMAND (
                                        hDlg,
                                        ID_SR_INFO,
                                        GetDlgItem ( hDlg, ID_SR_INFO ),
                                        BN_CLICKED,
                                        PostMessage
                                        );
                        }
                    break;

                case IDCANCEL:
                case ID_SR_DONE:
                    EndDialog(hDlg, 0);
                    break;

                case IDHELP:
                    StdHelpFromDialog (
                                    hDlg,
                                    (g_bDetails) ?
                                    IDH_NAVW_SCAN_DETAILS_HELP_BTN :
                                    IDH_NAVW_PROBLEMS_FOUND_DLG_HELP_BTN
                                    );
                    break;

                case ID_SR_REPAIR:
                    ScanResultRepair (hDlg);
                    SetFocus ( GetDlgItem ( hDlg, ID_SR_LISTBOX ) );
                    break;

#ifndef NAVSCANW  // No Inoc in NT
                case ID_SR_INOC:
                    ScanResultInoculate (hDlg);
                    SetFocus ( GetDlgItem ( hDlg, ID_SR_LISTBOX ) );
                    break;
#endif

                case ID_SR_EXCL:
                    ScanResultExclude (hDlg);
                    SetFocus ( GetDlgItem ( hDlg, ID_SR_LISTBOX ) );
                    break;

                case ID_SR_DELETE:
                    ScanResultDelete (hDlg);
                    SetFocus ( GetDlgItem ( hDlg, ID_SR_LISTBOX ) );
                    break;

                case ID_SR_QUARANTINE:
                    ScanResultQuarantine (hDlg);
                    SetFocus ( GetDlgItem ( hDlg, ID_SR_LISTBOX ) );
                    break;

                case ID_SR_INFO:
                    hListBox = GetDlgItem(hDlg, ID_SR_LISTBOX);
                    wIndex = ListBox_GetCurSel(hListBox);

                    if (wIndex != (WORD)LB_ERR)
                        {
                        lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (
                                                     hListBox, wIndex);
                        if (OkToShowFileInfo(lpInfected->wStatus) == TRUE)
                            ShowScanInfoDlg(hDlg, hListBox, lpNav, TRUE);
                        }
                    break;
                }

            break;
        }

    return ( bReturn );
}

VOID LOCAL ShrinkWindow (HWND hDlg)
{
    auto    RECT    RectParent;
    auto    RECT    RectAlert;

    GetWindowRect (hDlg, &RectParent);
    GetWindowRect (GetDlgItem (hDlg, ID_SR_ALERTFRAME), &RectAlert);

    ShowWindow (hDlg, SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SR_ALERTFRAME), SW_HIDE);

    RectParent.bottom -= (RectAlert.bottom - RectAlert.top + 5);

    MoveWindow (hDlg, RectParent.left, RectParent.top,
                RectParent.right - RectParent.left,
                RectParent.bottom - RectParent.top, TRUE);
}


//************************************************************************
// ActUponFileStatus()
//
// This routine displays the filename and help line for the current
// list box entry in the Scan Results screen.
// It also enables/disables buttons based on the status of the current
// entry.
//
// Parameters:
//
// Returns:
//      Nothing
//************************************************************************
//  3/01/94 DALLEE, added support for registration button.
//              Grouped and moved checks vs. filestatus to the end.
//************************************************************************

VOID SYM_EXPORT WINAPI ActUponFileStatus (
    HWND            hDlg,
    LPNONAV         lpNav,
    LPINFECTEDFILE  lpInfected,
    int             iStaticMaxChars )
{
    auto    BOOL    bShowInoc   = FALSE;
    auto    BOOL    bShowRepair = FALSE;
    auto    BOOL    bShowDelete = FALSE;
    auto    BOOL    bShowExclude = FALSE;
    auto    BOOL    bShowQuarantine = FALSE;

    auto    char    szWork      [ SYM_MAX_PATH * 2];
    auto    char    szFormat    [ SYM_MAX_PATH + 1 ];
    auto    char    szZipPath   [ SYM_MAX_PATH + 1 ];
    auto    char    szFileName  [ SYM_MAX_PATH + 1 ];
    auto    UINT    uInocAction = 0;

    auto    UINT    wMajorType,
                    wMinorType;

    if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED 
    ||  lpInfected->wStatus == FILESTATUS_QUARANTINE_INFECTED_ZIP )
        {
        if (String_Get (IDS_ZIPFOLLOWHELP, szFormat, sizeof(szFormat)))
            {
            char szTemp [ SYM_MAX_PATH + 1 ];

            ParseZipFileName (lpInfected->szFullPath, szTemp, szFileName);
                                        // Shorten the text going into the static to
                                        // parameter iStaticMaxChars :
            NameShortenFileName ( szTemp, szZipPath, iStaticMaxChars );
            SPRINTF (szWork, szFormat, (LPSTR)szFileName, (LPSTR)szZipPath);
            Static_SetText (GetDlgItem(hDlg, ID_SR_RAP), szWork);
            }
        }
    else
        if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
        ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
        ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
            {
            SPRINTF (szWork, String_Ptr (IDS_PHYS_DRIVE), lpInfected->szFullPath[0]);
            Static_SetText (GetDlgItem(hDlg, ID_SR_RAP), szWork);
            }
        else
            {
            NameDisplayShortenedFileName ( GetDlgItem(hDlg, ID_SR_RAP),
                                           lpInfected->szFullPath,
                                           2 );
            }

    Static_SetText (GetDlgItem(hDlg, ID_SR_RAP2),
                    FileStatus[lpInfected->wStatus].uFollowHelp ?
                     String_Ptr(FileStatus[lpInfected->wStatus].uFollowHelp) :
                     "" );

    if (IsInoculateStatus(lpInfected->wStatus))
        {
                                        // This if() statement is to only
                                        // enable buttons if the corresponding
                                        // inoculation options is set to PROMPT
        if ( lpInfected->wStatus == FILESTATUS_NOTINOCULATED ||
             lpInfected->wStatus == FILESTATUS_INOCULATIONFAILED )
            {
            uInocAction = lpNav->Inoc.uActionNewInoc;
            }
        else
            {
            // if (lpInfected->wStatus == FILESTATUS_INOCULATEDCHANGED ||
            //     lpInfected->wStatus == FILESTATUS_REINOCULATIONFAILED)
            /*
                I commented out this code since we have a lot of inoculation
                statuses to watch out for and I think it makes sense to only
                check for statuses that require the new inoculation action flag.
                Any other inoculate status must require the inoc change
                action flag.
            */


            uInocAction = lpNav->Inoc.uActionInocChg;
            }

        if (uInocAction == INOC_PROMPT)
            {
            bShowRepair  = lpNav->Inoc.bDispRepair;
            bShowDelete  = lpNav->Inoc.bDispDelete;
            bShowExclude = lpNav->Inoc.bDispExcl;
            bShowQuarantine = lpNav->Opt.bDispQuarantine;
            }
        }
    else
        {
        UINT uStatus = GetActionStatus(lpNav, lpInfected);

        if ( (uStatus != SCAN_REPORT) &&
             (uStatus != SCAN_HALTCPU) )
            {
            bShowRepair  = lpNav->Opt.bDispRepair;
            bShowDelete  = lpNav->Opt.bDispDelete;
            bShowExclude = lpNav->Opt.bDispExcl;
            bShowQuarantine = lpNav->Opt.bDispQuarantine;
            }
        }

                                        // --------------------------------
                                        // Note that the inoculation button
                                        // is not enabled if we aren't to
                                        // inoculate on floppies
                                        // --------------------------------
    if ( (uInocAction == INOC_PROMPT) &&
         (lpNav->Inoc.bDispInoc == TRUE) )
        {
        DiskGetType (*lpInfected->szFullPath, &wMajorType, &wMinorType);

        if (wMajorType == TYPE_REMOVEABLE)
            {
            if (lpNav->Inoc.bInocFloppies)
                bShowInoc = TRUE;
            }
        else
            bShowInoc = TRUE;
        }

                                        // -------------------------------
                                        // After setting T/F by the options,
                                        // only enable if the file status
                                        // allows that operation.
                                        // -------------------------------
    bShowRepair  = bShowRepair  && FileStatus[lpInfected->wStatus].byCanRepair;
    bShowQuarantine = bShowQuarantine && FileStatus[lpInfected->wStatus].bycanQuarantine;
    bShowDelete  = bShowDelete  && FileStatus[lpInfected->wStatus].byCanDelete;
    bShowInoc    = bShowInoc    && FileStatus[lpInfected->wStatus].byCanInoc;
    bShowExclude = bShowExclude && FileStatus[lpInfected->wStatus].byCanExclude
                                && !NavTestOemToAnsi (lpInfected->szFullOemPath);

                                        // -----------------------------------
                                        // This is a special case for files
                                        // infected with the DIR2 virus.
                                        // -----------------------------------
    if (lpInfected->Notes.wInfo & bitINFO_DIR2)
        {
        bShowInoc   = FALSE;
        bShowDelete = FALSE;
        }

                                        // -----------------------------------
                                        // Special case for long repairs.
                                        // DISPLAY_DIALOG repairs are only
                                        // possible in DOS. (ex. One-Half-Boot)
                                        // -----------------------------------
    if (lpInfected->Notes.wControl2 & DISPLAY_DIALOG)
        {
        bShowRepair = FALSE;
        }

    EnableWindow (GetDlgItem(hDlg, ID_SR_INOC),     bShowInoc);
    EnableWindow (GetDlgItem(hDlg, ID_SR_REPAIR),   bShowRepair);
    EnableWindow (GetDlgItem(hDlg, ID_SR_DELETE),   bShowDelete);
    EnableWindow (GetDlgItem(hDlg, ID_SR_EXCL),     bShowExclude);
    EnableWindow (GetDlgItem(hDlg, ID_SR_QUARANTINE), bShowQuarantine);
    EnableWindow (GetDlgItem(hDlg, ID_SR_INFO),
                  FileStatus[lpInfected->wStatus].byHasVirusInfo);
} // ActUponFileStatus()


//************************************************************************
// GetActionStatus()
//
// This routine returns the proper repair action based on the
// custom response settings.
//
// Parameters:
//      lpNav - current NONAV struct
//      lpInfected - current infected item
//
// Returns:
//      UINT action to perform
//************************************************************************
// 08/10/97 09:35pm - DBUCHES Funcion created
//************************************************************************
UINT LOCAL GetActionStatus( LPNONAV lpNav, LPINFECTEDFILE  lpInfected )
{

    // If custom response is not selected, just return the
    // action type requested.
    if( !lpNav->lpScan->lpNavOptions->customScan.bSelected )
        return lpNav->Opt.uAction;

    // Check for boot type.
    if( lpInfected->Notes.wInfo &
        (bitINFO_FLOPBOOT | bitINFO_HDISKBOOT | bitINFO_MBOOTREC))
        return lpNav->lpScan->lpNavOptions->customScan.uchBoot;

    // Check for Macro virus type.
    if (lpInfected->Notes.wControl2 & bitINFO_MACRO)
        return lpNav->lpScan->lpNavOptions->customScan.uchMacro;

    // Must be a a regular file virus.
    return lpNav->lpScan->lpNavOptions->customScan.uchFile;

}



VOID LOCAL ScanResultRepair (HWND hDlg)
{
    auto    REPAIRFILESTRUCT    Repair;
    auto    LPINFECTEDFILE      lpInfected;
    auto    DOALLSTRUCT         Dall;
    auto    ASKQUESTSTRUCT      Ask;
    auto    LPNONAV             lpNav;
    auto    WORD                wIndex;
    auto    HWND                hListBox;
    auto    BOOL                bStatus;


    lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
    if (!lpNav)
        {
        LP_SET (hDlg, GloballySaved_lpNav);
        lpNav = (LPNONAV)LP_GET(hDlg);
        SYM_ASSERT (lpNav);
        }
#endif
    hListBox = GetDlgItem(hDlg, ID_SR_LISTBOX);
    wIndex = ListBox_GetCurSel(hListBox);

    if (wIndex != (WORD)LB_ERR)
        {
        lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (hListBox, wIndex);
        MEMCPY (&lpNav->Infected, lpInfected, sizeof(INFECTEDFILE));

        switch (lpInfected->wStatus)
            {
            case FILESTATUS_INFECTED:
                Ask.uType = ASKQUESTION_REPAIR_INFECTED;
                break;

            case FILESTATUS_INOCULATEDCHANGED:
                Ask.uType = ASKQUESTION_REPAIR_INOCHANGED;
                break;

            case FILESTATUS_BOOTINFECTED:
                Ask.uType = ASKQUESTION_REPAIR_BOOT;
                break;

            case FILESTATUS_MASTERBOOTINFECTED:
                Ask.uType = ASKQUESTION_REPAIR_MASTER_BOOT;
                break;

            default:
                Ask.uType = 0;
                break;
            }

        if (Ask.uType)
            {
            Ask.hDlg          = hDlg;
            Ask.lpFileName    = lpNav->Infected.szFullPath;
            Ask.lpVirusName   = lpNav->Infected.Notes.lpVirName;

            switch (AskQuestion(&Ask))
                {
                case ASKQUESTION_ONLYONE:
                    switch (lpInfected->wStatus)
                        {
                        case FILESTATUS_INOCULATEDCHANGED:
                        case FILESTATUS_INFECTED:
                            Repair.bUseInoc         = lpNav->Inoc.bUseInoc &&
                                !(NO_GENERIC & lpNav->Infected.Notes.wControl2);
                            Repair.bCreateBackup    = lpNav->Gen.bBackupRep;
                            Repair.uVirusSigIndex   = lpNav->Infected.uVirusSigIndex;
                            STRCPY (Repair.szFullPath, lpNav->Infected.szFullPath);
                            STRCPY (Repair.szFullOemPath, lpNav->Infected.szFullOemPath);
                            STRCPY (Repair.szBackExt, lpNav->Gen.szBackExt);

                            bStatus = RepairFile (&Repair);

                            if (bStatus == TRUE)
                                lpNav->Stats.Files.uCleaned++;

                            NotifyRepairCompletion (hDlg, bStatus, lpNav);
                            break;

                        case FILESTATUS_MASTERBOOTINFECTED:
                        case FILESTATUS_BOOTINFECTED:
                            bStatus = RepairBoot(lpNav, hDlg, TRUE);

                            if (bStatus == TRUE)
                                {
                                if (lpNav->Infected.wStatus == FILESTATUS_BOOTINFECTED)
                                    lpNav->Stats.BootRecs.uCleaned++;
                                else
                                    lpNav->Stats.MasterBootRec.uCleaned++;
                                }
                            break;
                        }

                    lpInfected->wStatus = UpdateFileStatus (
                                                        lpInfected->wStatus,
                                                        PROCESS_REPAIR,
                                                        bStatus);

                    ListBox_DeleteString(hListBox, wIndex);
                    ShowInfectFile(hListBox, lpInfected, wIndex);
                    ListBox_SetCurSel(hListBox, wIndex);

                                        // Update status bar

                    FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                                GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                LBN_SELCHANGE, PostMessage );
                    break;

                case ASKQUESTION_ALL:
                    MEMSET (&Dall, EOS, sizeof(Dall));

                    Dall.uType      = Ask.uType;
                    Dall.hListbox   = hListBox;
                    Dall.lpNav      = lpNav;
                    DoAll (hDlg, &Dall);

                    ListBox_SetCurSel(hListBox, 0);

                    FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                                GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                LBN_SELCHANGE, PostMessage );
                    break;
                }
            }
        }
}

#ifndef NAVSCANW  // No Inoc in NT.

VOID LOCAL ScanResultInoculate (HWND hDlg)
{
    auto    LPINFECTEDFILE  lpInfected;
    auto    DOALLSTRUCT     Dall;
    auto    ASKQUESTSTRUCT  Ask;
    auto    LPNONAV         lpNav;
    auto    WORD            wIndex;
    auto    HWND            hListBox;
    auto    BOOL            bStatus;

    lpNav = (LPNONAV)LP_GET(hDlg);
    hListBox = GetDlgItem(hDlg, ID_SR_LISTBOX);
    wIndex = ListBox_GetCurSel(hListBox);

    if (InocPasswordVerify (hDlg, lpNav) == FALSE)
        wIndex = (WORD)LB_ERR;

    if (wIndex != (WORD)LB_ERR)
        {
        lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (hListBox, wIndex);
        MEMCPY (&lpNav->Infected, lpInfected, sizeof(INFECTEDFILE));

        switch (lpInfected->wStatus)
            {
            case FILESTATUS_INOCULATEDCHANGED:
                Ask.uType = ASKQUESTION_INOCULATE_INOCHANGED;
                break;

            case FILESTATUS_NOTINOCULATED:
            case FILESTATUS_REPAIRED:
                Ask.uType = ASKQUESTION_INOCULATE_ALLCLEANFILES;
                break;
            default:
                Ask.uType = 0;
                break;
            }

        if (Ask.uType)
            {
            Ask.hDlg          = hDlg;
            Ask.lpFileName    = lpNav->Infected.szFullPath;
            Ask.lpVirusName   = lpNav->Infected.Notes.lpVirName;

            switch (AskQuestion(&Ask))
                {
                case ASKQUESTION_ONLYONE:
                    *lpNav->Infected.Notes.lpVirName = EOS;

                    // ************************************** //
                    // In inoculation has changed, remove the //
                    // previous inoculation informatin from   //
                    // the database, and then inoculate file. //
                    // ************************************** //

                    if (lpInfected->wStatus == FILESTATUS_INOCULATEDCHANGED)
                        InocDelFile (lpNav->Infected.szFullPath);

                    bStatus = InocThisFile (lpNav, hDlg, TRUE);

                    lpInfected->wStatus = UpdateFileStatus (
                                                        lpInfected->wStatus,
                                                        PROCESS_INOCULATE,
                                                        bStatus);

                    ListBox_DeleteString(hListBox, wIndex);
                    ShowInfectFile(hListBox, lpInfected, wIndex);
                    ListBox_SetCurSel(hListBox, wIndex);
                    break;

                case ASKQUESTION_ALL:
                    MEMSET (&Dall, EOS, sizeof(Dall));
                    if (lpInfected->wStatus == FILESTATUS_INOCULATEDCHANGED)
                        Dall.uType  = ASKQUESTION_INOCULATE_INOCHANGED;
                    else
                        Dall.uType  = ASKQUESTION_INOCULATE_ALLCLEANFILES;
                    Dall.hListbox   = hListBox;
                    Dall.lpNav      = lpNav;
                    DoAll (hDlg, &Dall);

                    ListBox_SetCurSel(hListBox, 0);
                    break;
                }
            }

        ActUponFileStatus (hDlg, lpNav, lpInfected, 80);
        }
}
#endif  // !NAVSCANW


//************************************************************************
// ScanResultExclude()
//
// This routine brings up the exclude file dialog from the Problems
// Found screen at the end of scanning.
//
// Parameters:
//      HWND    hDlg                    Handle to parent window.
//
// Returns:
//      Nothing.
//************************************************************************
//  3/04/94 DALLEE, added question for registration exclusions.
//************************************************************************

VOID LOCAL ScanResultExclude (HWND hDlg)
{
    auto    LPINFECTEDFILE  lpInfected;
    auto    CHECKSTRUCT     PwCheck;
    auto    ASKQUESTSTRUCT  Ask;
    auto    LPNONAV         lpNav;
    auto    WORD            wIndex;
    auto    HWND            hListBox;
    auto    UINT            uStatus;

    lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
    if (!lpNav)
        {
        LP_SET (hDlg, GloballySaved_lpNav);
        lpNav = (LPNONAV)LP_GET(hDlg);
        SYM_ASSERT (lpNav);
        }
#endif
    hListBox = GetDlgItem(hDlg, ID_SR_LISTBOX);
    wIndex = ListBox_GetCurSel(hListBox);

    MEMSET (&PwCheck, EOS, sizeof(PwCheck));
    PwCheck.hWnd         = hDlg;
    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
    PwCheck.uID          = PASSWORD_MODIEXCLUSIONS;

    if (AskPassword(&PwCheck) == PASSWORD_ASK)
        uStatus = PasswordCheck (&PwCheck, FALSE, FALSE);
    else
        uStatus = TRUE;

    if ( wIndex != (WORD)LB_ERR && uStatus == TRUE )
        {
        lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (hListBox, wIndex);
        MEMCPY (&lpNav->Infected, lpInfected, sizeof(INFECTEDFILE));

        switch (lpInfected->wStatus)
            {
            case FILESTATUS_NOTDELETED:
            case FILESTATUS_NOTREPAIRED:
            case FILESTATUS_INFECTED:
            case FILESTATUS_QUARANTINE_INFECTED_FAILED:
                Ask.uType = ASKQUESTION_EXCLUDE_KNOWNVIRUS;
                break;

            case FILESTATUS_NOTINOCULATED:
            case FILESTATUS_INOCULATIONFAILED:
            case FILESTATUS_QUARANTINE_INOCULATEDCHANGED_FAILED:
                Ask.uType = ASKQUESTION_EXCLUDE_NOTINOCULATED;
                break;

            case FILESTATUS_INOCULATEDCHANGED:
            case FILESTATUS_REINOCULATIONFAILED:
                Ask.uType = ASKQUESTION_EXCLUDE_INOCHANGES;
                break;

            default:
                Ask.uType = 0;
                break;
            }

        if (Ask.uType)
            {
            Ask.hDlg          = hDlg;
            Ask.lpFileName    = lpNav->Infected.szFullPath;
            Ask.lpVirusName   = lpNav->Infected.Notes.lpVirName;

            switch (AskQuestion(&Ask))
                {
                case ASKQUESTION_ONLYONE:
                    uStatus = ExcludeAddFile (lpNav, hDlg, TRUE);

                    lpInfected->wStatus = UpdateFileStatus (
                                                    lpInfected->wStatus,
                                                    PROCESS_EXCLUDE,
                                                    uStatus);

                    ListBox_DeleteString ( hListBox, wIndex );
                    ShowInfectFile ( hListBox, lpInfected, wIndex );
                    ListBox_SetCurSel ( hListBox, wIndex );

                                    // -----------------------------------
                                    // Update status bar
                                    // -----------------------------------

                    FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                                    GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                    LBN_SELCHANGE, PostMessage );
                    break;
                }
            }
        }
}

VOID LOCAL ScanResultDelete (HWND hDlg)
{
    auto    YESNOSTRUCT     YesNo;
    auto    LPINFECTEDFILE  lpInfected;
    auto    DOALLSTRUCT     Dall;
    auto    ASKQUESTSTRUCT  Ask;
    auto    LPNONAV         lpNav;
    auto    BOOL            bStatus;
    auto    WORD            wIndex;
    auto    HWND            hListBox;

    lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
    if (!lpNav)
        {
        LP_SET (hDlg, GloballySaved_lpNav);
        lpNav = (LPNONAV)LP_GET(hDlg);
        SYM_ASSERT (lpNav);
        }
#endif
    hListBox = GetDlgItem(hDlg, ID_SR_LISTBOX);
    wIndex = ListBox_GetCurSel(hListBox);

    if (wIndex != (WORD)LB_ERR)
        {
        lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (hListBox, wIndex);

        MEMCPY (&lpNav->Infected, lpInfected, sizeof(lpNav->Infected));

        switch (lpInfected->wStatus)
            {
            case FILESTATUS_INFECTED:
                Ask.uType = ASKQUESTION_DELETE_INFECTED;
                break;

            case FILESTATUS_NOTINOCULATED:
                Ask.uType = ASKQUESTION_DELETE_UNINOCULATED;
                break;

            case FILESTATUS_INOCULATIONFAILED:
            case FILESTATUS_REINOCULATIONFAILED:
            case FILESTATUS_INOCULATEREPAIR_FAILED:
            case FILESTATUS_QUARANTINE_INOCULATEDCHANGED_FAILED:
                Ask.uType = ASKQUESTION_DELETE_INOCULATERROR;
                break;

            case FILESTATUS_INOCULATEDCHANGED:
                Ask.uType = ASKQUESTION_DELETE_INOCHANGED;
                break;

            case FILESTATUS_ERROR:
                Ask.uType = ASKQUESTION_DELETE_ERROR;
                break;

            case FILESTATUS_NOTREPAIRED:
                Ask.uType = ASKQUESTION_DELETE_NOTREPAIRED;
                break;

            default:
                Ask.uType = 0;
                break;
            }

        if (Ask.uType)
            {
            Ask.hDlg          = hDlg;
            Ask.lpFileName    = lpNav->Infected.szFullPath;
            Ask.lpVirusName   = lpNav->Infected.Notes.lpVirName;

            switch (AskQuestion(&Ask))
                {
                case ASKQUESTION_ONLYONE:
                    bStatus = NavDeleteFile (lpNav, hDlg, TRUE);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uDeleted++;

                    if (lpInfected->wStatus != FILESTATUS_ZIPINFECTED)
                        {
                        lpInfected->wStatus = UpdateFileStatus (
                                lpInfected->wStatus, PROCESS_DELETE, bStatus);

                        ListBox_DeleteString(hListBox, wIndex);
                        ShowInfectFile(hListBox, lpInfected, wIndex);
                        ListBox_SetCurSel(hListBox, wIndex);
                        }

                    FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                                GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                LBN_SELCHANGE, PostMessage );
                    break;

                case ASKQUESTION_ALL:
                    {
                    MEMSET (&YesNo, EOS, sizeof(YesNo));
                    YesNo.hParent       =   hDlg;
                    YesNo.uYesButtonID  =   IDS_BUTTON_DELETE;
                    YesNo.uNoButtonID   =   IDS_BUTTON_CANCEL;
                    YesNo.uTitleID      =   IDS_TITLE_DELETEFILE;
                    YesNo.hInstance     =   hInstance;

                    static DWORD    dwWhatsThisDeleteInocFailed[] =
                        {
                        0,          0
                        };

                    static DWORD    dwWhatsThisDeleteAllFiles[] =
                        {
                        0,          0
                        };

                    if (lpInfected->wStatus == FILESTATUS_INOCULATIONFAILED ||
                        lpInfected->wStatus == FILESTATUS_REINOCULATIONFAILED)
                        {
                        YesNo.uQuestionID = IDS_YESNO_DELETEALLINOCFAILED;
                        YesNo.lpdwWhatsThisArray = dwWhatsThisDeleteInocFailed;
                        YesNo.uHelpBtnID = 0;
                        }
                    else
                        {
                        YesNo.uQuestionID = IDS_YESNO_DELETEALLFILES;
                        YesNo.lpdwWhatsThisArray = dwWhatsThisDeleteAllFiles;
                        YesNo.uHelpBtnID = IDH_NAVW_CONFIRM_DELETEALLFILES_BTN;
                        }

                    if ( AskYesNo (&YesNo) == TRUE )
                        {
                        MEMSET (&Dall, EOS, sizeof(Dall));
                        Dall.uType      = Ask.uType;
                        Dall.hListbox   = hListBox;
                        Dall.lpNav      = lpNav;
                        DoAll (hDlg, &Dall);

                        ListBox_SetCurSel(hListBox, 0);

                        FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                                GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                LBN_SELCHANGE, PostMessage );
                        }
                    }
                    break;
                }
            }
        }
}

VOID LOCAL ScanResultQuarantine (HWND hDlg)
{
    auto    YESNOSTRUCT     YesNo;
    auto    LPINFECTEDFILE  lpInfected;
    auto    DOALLSTRUCT     Dall;
    auto    ASKQUESTSTRUCT  Ask;
    auto    LPNONAV         lpNav;
    auto    BOOL            bStatus;
    auto    WORD            wIndex;
    auto    HWND            hListBox;
    auto    char            szZipFileName  [ SYM_MAX_PATH + 1 ] = {0};


    lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
    if (!lpNav)
    {
        LP_SET (hDlg, GloballySaved_lpNav);
        lpNav = (LPNONAV)LP_GET(hDlg);
        SYM_ASSERT (lpNav);
    }
#endif
    hListBox = GetDlgItem(hDlg, ID_SR_LISTBOX);
    wIndex = ListBox_GetCurSel(hListBox);

    if (wIndex != (WORD)LB_ERR)
    {
        lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (hListBox, wIndex);

        MEMCPY (&lpNav->Infected, lpInfected, sizeof(lpNav->Infected));

        switch (lpInfected->wStatus)
        {
            case FILESTATUS_INFECTED:
            case FILESTATUS_NOTINOCULATED:
            case FILESTATUS_INOCULATIONFAILED:
            case FILESTATUS_REINOCULATIONFAILED:
            case FILESTATUS_INOCULATEREPAIR_FAILED:
            case FILESTATUS_INOCULATEDCHANGED:
            case FILESTATUS_NOTREPAIRED:
            case FILESTATUS_ERROR:
            case FILESTATUS_ZIPINFECTED:

                Ask.uType = ASKQUESTION_QUARANTINE_ALL_INFECTED;
                break;

            default:
                Ask.uType = 0;
                break;
        }

        if (Ask.uType)
        {
            if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
            {
                MEMCPY(szZipFileName, lpNav->Infected.szFullPath, _tcslen(lpNav->Infected.szFullPath));
                RemoveZipInfected ((LPSTR)&szZipFileName[0]);
                Ask.lpFileName  = szZipFileName;
            }
            else
                Ask.lpFileName  = lpNav->Infected.szFullPath;

            Ask.hDlg        = hDlg;
            Ask.lpVirusName = lpNav->Infected.Notes.lpVirName;

            switch (AskQuestion(&Ask))
            {
                case ASKQUESTION_ONLYONE:
                {
                    if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
                    {
                        bStatus = NavMoveFile (lpNav, hDlg, TRUE);

                        if (bStatus)
                            MarkCompressedFilesQuarantined(lpNav, lpInfected, NULL, hListBox, hDlg);

                    }
                    else
                    {
                        bStatus = NavMoveFile (lpNav, hDlg, TRUE);

                        if (bStatus == TRUE)
                            lpNav->Stats.Files.uQuarantined++;

                        if (lpInfected->wStatus != FILESTATUS_ZIPINFECTED)
                        {
                            lpInfected->wStatus = UpdateFileStatus (
                                lpInfected->wStatus, PROCESS_QUARANTINE, bStatus);

                            ListBox_DeleteString(hListBox, wIndex);
                            ShowInfectFile(hListBox, lpInfected, wIndex);
                            ListBox_SetCurSel(hListBox, wIndex);
                        }
                    }

                    FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                                GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                LBN_SELCHANGE, PostMessage );
                }
                break;

                case ASKQUESTION_ALL:
                {
                    MEMSET (&YesNo, EOS, sizeof(YesNo));
                    YesNo.hParent       =   hDlg;
                    YesNo.uYesButtonID  =   IDS_BUTTON_MOVE;
                    YesNo.uNoButtonID   =   IDS_BUTTON_CANCEL;
                    YesNo.uTitleID      =   IDS_TITLE_MOVEFILE;
                    YesNo.hInstance     =   hInstance;

                    static DWORD    dwWhatsThisQuarantineAllFiles[] =
                    {
                        0,          0
                    };

                    YesNo.uQuestionID = IDS_YESNO_QUARANTINEALLFILES;
                    YesNo.lpdwWhatsThisArray = dwWhatsThisQuarantineAllFiles;
                    YesNo.uHelpBtnID = IDH_NAVW_QUARAN_DLG_HLP_BTN;

                    if ( AskYesNo (&YesNo) == TRUE )
                    {
                        MEMSET (&Dall, EOS, sizeof(Dall));
                        Dall.uType      = Ask.uType;
                        Dall.hListbox   = hListBox;
                        Dall.lpNav      = lpNav;
                        DoAll (hDlg, &Dall);

                        ListBox_SetCurSel(hListBox, 0);

                        FORWARD_WM_COMMAND ( hDlg, ID_SR_LISTBOX,
                                GetDlgItem ( hDlg, ID_SR_LISTBOX ),
                                LBN_SELCHANGE, PostMessage );
                    }
                }
                break;
            }
        }
    }
}


VOID SYM_EXPORT WINAPI RemoveZipInfected (LPSTR lpZipFullPath)
{
    auto    UINT    uIndex;
    auto    LPSTR   lpStr;

    lpStr = SYMstrrchr ((LPCSTR)lpZipFullPath, (int)ZIP_SEPARATOR);
    if (NULL != lpStr)
        {
        uIndex = lpStr - lpZipFullPath;
        lpZipFullPath [uIndex] = EOS;
        }
}


BOOL SYM_EXPORT WINAPI OkToShowFileInfo (UINT uFileStatus)
{
    extern FILESTATUSSTRUCT FileStatus[];

    return ((BOOL)FileStatus[uFileStatus].byHasVirusInfo);
}

BOOL SYM_EXPORT WINAPI IsInoculateStatus (WORD wFileStatus)
{
    auto    BOOL    bStatus = FALSE;

    if (wFileStatus == FILESTATUS_INOCULATED ||
        wFileStatus == FILESTATUS_INOCULATIONFAILED ||
        wFileStatus == FILESTATUS_INOCULATEREPAIRED ||
        wFileStatus == FILESTATUS_INOCULATEREPAIR_FAILED ||
        wFileStatus == FILESTATUS_INOCULATEDELETED ||
        wFileStatus == FILESTATUS_INOCULATEDELETE_FAILED ||
        wFileStatus == FILESTATUS_REINOCULATED ||
        wFileStatus == FILESTATUS_REINOCULATIONFAILED ||
        wFileStatus == FILESTATUS_NOTINOCULATED ||
        wFileStatus == FILESTATUS_INOCULATEDCHANGED)
        bStatus = TRUE;

    return (bStatus);
}


#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextHelp (
            HWND    hDlg,
            UINT    uMsg,
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU windows
message. The purpose of this function is to call the standard symantec help
routines that display an appropiate floating help menu.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the message
    UINT    uMsg,                       // [in]"message"
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
        ID_SR_LISTBOX,      IDH_NAVW_WHATHIS_PROBLEMS_FOUND_DLG,
        ID_SR_DONE,         IDH_NAVW_WHATHIS_PROBLEMS_FOUND_DLG_DONE_BTN,
        ID_SR_REPAIR,       IDH_NAVW_WHATHIS_PROBLEMS_FOUND_DLG_REPAIR_BTN,
        ID_SR_DELETE,       IDH_NAVW_WHATHIS_PROBLEMS_FOUND_DLG_DELETE_BTN,
        ID_SR_INOC,         IDH_NAVW_WHATHIS_PROBLEMS_FOUND_DLG_INOCULATE_BTN,
        ID_SR_EXCL,         IDH_NAVW_WHATHIS_PROBLEMS_FOUND_DLG_EXCLUDE_BTN,
        ID_SR_INFO,         IDH_NAVW_WHATHIS_PROBLEMS_FOUND_DLG_INFO,
        ID_SR_QUARANTINE,   IDH_NAVW_WHATHIS_PROBLEMS_FOUND_QUARAN_BTN,
        0,                  0
        };

    static  DWORD       dwDetailsOnlyWhatsThisMap[] =
        {
//&^        ID_SR_LISTBOX,  IDH_NAVW_WHATHIS_SCAN_DETAILS_DLG,
//&^        IDD_DETAILS,    IDH_NAVW_WHATHIS_SCAN_DETAILS_DLG,
        ID_SR_DONE,     IDH_NAVW_WHATHIS_SCAN_DETAILS_CLOSE_BTN,
        ID_SR_INFO,     IDH_NAVW_WHATHIS_SCAN_DETAILS_INFO_BTN,
        0,              0
        };


    BOOL bProblemsFoundDlg = ( GetDlgItem(hDlg, ID_SR_REPAIR) != NULL );
                                        // otherwise it is "Details" dialog

    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
    ( (bProblemsFoundDlg) ? dwWhatsThisMap \
                                 : dwDetailsOnlyWhatsThisMap ),
                        NULL,
                        NULL,           /* This dlg. does not have any LBoxs */
                        NULL            /* that's why these nulls            */
                       );
}

#endif // #if win32 only


/* **************************************************************************
@Name: VOID SYM_EXPORT WINAPI MarkCompressedFilesQuarantined
            LPNONAV         lpNav, 
            LPINFECTEDFILE  lpInfectedZipFile, 
            HWND            hListbox, 
            HWND            hDlg )

@Description:
This function goes through the list of infected items looking for entries that
are contained in the specified zip file lpInfectedZipFile. When it finds a 
match, the FILESTATUS is updated and the quarantine count is bumped up. 

@Parameters:
$lpNav$             Pointer to a NONAV structure        

$lpInfectedZipFile$ Pointer to the iunfected zip file

$hListBox$          Handle of a list box (i.e. Problems Found)

$hDlg$              Handle of the current window.

************************************************************************** */
VOID SYM_EXPORT WINAPI MarkCompressedFilesQuarantined( 
                        LPNONAV lpNav, LPINFECTEDFILE  lpInfectedZipFile, LPINFARR lpInfArr, HWND hListbox, HWND hDlg )
{
    auto    LPINFARR        lpBlockList;
    auto    LPINFECTEDFILE  lpInfected;
    auto    UINT            uNumItems;
    auto    UINT            uCurrentItem;
    auto    UINT            i;
    auto    char            szZipFileName  [ SYM_MAX_PATH + 1 ] = {0};
    auto    BOOL            bFoundBlock = FALSE;

    MEMCPY(szZipFileName, lpInfectedZipFile->szFullPath, sizeof(lpInfectedZipFile->szFullPath));
    RemoveZipInfected (szZipFileName);

    if ( NULL == hListbox )
        {
        uNumItems = lpNav->Stats.Files.uInfected +
                    lpNav->Stats.BootRecs.uInfected +
                    lpNav->Stats.MasterBootRec.uInfected;

        // Loop through all blocks of problem items.

        for ( lpBlockList = lpNav->lpInfArr, uCurrentItem = 0;
              lpBlockList != NULL;
              lpBlockList = ( LPINFARR ) lpBlockList->lpNext )
            {

            // If we're given a lpInfArr, then check if this is the
            // right one.

            if ( lpInfArr && !bFoundBlock )
                {
                if ( lpInfArr != lpBlockList )
                    continue;
                else
                    bFoundBlock = TRUE;
                }

            lpInfected = ( LPINFECTEDFILE ) MemLock( lpBlockList->hInf );

            // Go through all the infected items in this block.

            if ( lpInfected )
                {
                for ( i = 0; 
                      i < lpBlockList->uNumUsed;
                      i++, uCurrentItem++, lpInfected++ )
                    {
                    if ( hDlg )
                        YieldToWindows(hDlg);
                    
                    if ( strstr(lpInfected->szFullPath,szZipFileName) )
                        {
                        lpInfected->wStatus = UpdateFileStatus (lpInfected->wStatus, PROCESS_QUARANTINE,
                                                TRUE);

                        lpNav->Stats.Files.uQuarantined++;
                        }
                    }

                MemUnlock( lpBlockList->hInf );
                }
            else
                {
                // Unable to lock this block's entries, just exit.

                break;
                }
            }
        }
    else
        {
        // Old style: Use list box in Problems Found for iteration.

        SetWindowRedraw(hListbox, FALSE);

        uNumItems = ListBox_GetCount(hListbox);

        if (uNumItems != (UINT)LB_ERR)
            {
            for (i = 0; i < uNumItems; i++)
                {
                if ( hDlg )
                    YieldToWindows(hDlg);

                lpInfected = (LPINFECTEDFILE)ListBox_GetItemData(hListbox, i);

                if (lpInfected)
                    {
                    if ( strstr(lpInfected->szFullPath,szZipFileName) ) 
                        {
                        lpInfected->wStatus = UpdateFileStatus (lpInfected->wStatus, PROCESS_QUARANTINE,
                                                TRUE);

                        ListBox_DeleteString(hListbox, i);
                        ShowInfectFile(hListbox, lpInfected, i);
                        ListBox_SetCurSel(hListbox, i);

                        lpNav->Stats.Files.uQuarantined++;
                        }
                    }
                }
            ListBox_SetCurSel(hListbox, uNumItems - 1);
            }

        SetWindowRedraw ( hListbox, TRUE );
        }
}


