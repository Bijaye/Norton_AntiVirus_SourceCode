// Copyright 1993-1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_repkil.cpv   1.11   02 Jul 1998 15:12:58   mdunn  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_repkil.cpv  $
// 
//    Rev 1.11   02 Jul 1998 15:12:58   mdunn
// fixed build 22 breakages on help IDs
// 
//    Rev 1.10   01 Jul 1998 14:11:10   DBuches
// Removed How-To help.
// 
//    Rev 1.9   18 May 1998 22:48:14   tcashin
// Count files deleted and files quarantined separatelu in VirusDlgProc().
// 
//    Rev 1.8   09 Mar 1998 23:43:26   DALLEE
// Changes for new (MFC-based) Repair Wizard.
// 
//    Rev 1.7   18 Feb 1998 16:22:12   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.6   29 Jul 1997 14:48:14   DALLEE
// In ShutdownNavapW(), get NAVAP window class name based on platform.
// 
//    Rev 1.5   15 Jul 1997 14:42:00   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.4   22 May 1997 16:20:58   BILL
// Alpha changes (removed ctl3d)
// 
//    Rev 1.3   22 May 1997 13:42:00   TIVANOV
// new quarantine functionality
// 
//    Rev 1.2   22 May 1997 11:30:52   TIVANOV
// use of the LongCusomMessage
// 
//    Rev 1.1   24 Apr 1997 11:08:38   JBRENNA
// Tesla port: KSACKING r1.1:
// Added password protection to the Immediate Notification dialog if the user
// pressed the exclude button, and you have that item password protected.
// 
//    Rev 1.0   06 Feb 1997 21:02:42   RFULLER
// Initial revision
// 
//    Rev 1.7   16 Oct 1996 19:05:42   DALLEE
// Added code to close NAVAP just before shutting down computer.
// Prevents hang due to VXD's blocked comm thread.
// 
//    Rev 1.6   30 Aug 1996 16:12:06   DALLEE
// Enable internet download code for W95 by removed #ifdef NAVSCANW's.
// 
//    Rev 1.5   11 Jul 1996 08:04:00   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.4   28 Jun 1996 15:59:28   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.3   17 May 1996 14:50:50   RHAYES
// Copy szFullOemPath into repair struct. before calling
// RepairFile. This fixes STS # 53512.
//
//    Rev 1.2   09 May 1996 18:00:14   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.1   22 Feb 1996 14:08:10   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:55:40   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:42   BARRY
// Initial revision.
//
//    Rev 1.57   29 Jan 1996 22:30:56   GWESTER
// In download mode, when we stop at a notification dialog, the main window
// icon (on the taskbar in 95, and on the desktop in NT) will return to a
// doctor's bag with the caption "Norton AntiVirus Scanner"; it won't display an
// intermediate percentage.
//
//    Rev 1.56   27 Jan 1996 16:23:46   GWESTER
// Fixed 60357; in download mode, if the file can't be repaired, we return to
// the "Virus Found" dialog with the focus on the "Abort" button.
//
//    Rev 1.55   25 Jan 1996 19:02:42   GWESTER
// Modified dialog behavior during download mode.
//
//    Rev 1.54   23 Jan 1996 22:43:50   GWESTER
// In download mode, changed the dialogs so that they don't show the
// full path of the infected download.
//
//    Rev 1.53   22 Jan 1996 21:39:26   GWESTER
// Added new help IDs for help buttons in Netscape dialogs.
//
//    Rev 1.52   21 Jan 1996 12:55:12   GWESTER
// Added handling of Netscape download dialogs.
//
//    Rev 1.51   14 Jan 1996 16:50:02   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.50   13 Dec 1995 14:38:10   YVANG
// Fixed the bug in which drive number does not appear in the "Virus Found"
// dialog message, "The master boot record on drive # is infected with ..."
// when displaying an MBR infection (with Immediate notify=ON).
//
//    Rev 1.49   30 Nov 1995 22:41:58   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.48   21 Nov 1995 12:51:18   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//
//    Rev 1.47   09 Nov 1995 16:48:48   JWORDEN
// Use special MBR text for message announcing a system shutdown after
// an MBR virus is detected.
//
//    Rev 1.46   02 Nov 1995 16:35:44   JWORDEN
// Convert MasterBootRec elements from booleans to counters
//
//    Rev 1.45   24 Oct 1995 19:29:50   JWORDEN
// Separate processing for boot records and master boot records
//
//    Rev 1.44   07 Jul 1995 11:16:44   MARKL
// Now calls NavHaltSystem()
//
//    Rev 1.43   03 Jul 1995 09:23:50   KEITH
// Shorten child name as well as the parent zip name for display.
//
//    Rev 1.42   02 Jul 1995 14:00:52   MARKL
// Added bitmasks and flicker free animation for the price of one.
//
//    Rev 1.41   13 Jun 1995 15:53:56   KEITH
// Handle how NAVW continues after a system shutdown is canceled. This would
// be after a memory infection.
// Then...make the shutdown unconditional again anyhow!?
//
//    Rev 1.40   09 May 1995 10:31:02   KEITH
// Correct some nouns
//
//    Rev 1.39   07 May 1995 19:37:48   MFALLEN
// Replaced animation functions.
//
//    Rev 1.38   26 Apr 1995 18:12:32   BRAD
// Added NG_SetGroupBoxSunken()
//
//    Rev 1.37   25 Apr 1995 18:47:32   MFALLEN
// Added and changed help ids with Elizabeth.
//
//    Rev 1.36   24 Apr 1995 18:56:48   BRAD
// Enabled Ctl3d
//
//    Rev 1.35   18 Apr 1995 18:26:08   MFALLEN
// Touch ups as docs suggested.
//
//    Rev 1.34   17 Apr 1995 14:46:22   MFALLEN
// SYM_MAX_PATH + 1
//
//    Rev 1.33   17 Apr 1995 14:04:44   MFALLEN
// DISPLAY_DIALOG flag for special virus handling merged in from inline code.
//
//    Rev 1.32   13 Apr 1995 16:06:50   MFALLEN
// Do not deactive when animating.
//
//    Rev 1.31   11 Apr 1995 13:06:26   MFALLEN
// Buttons that are enabled/disabled had some logic problems.
//
//    Rev 1.30   10 Apr 1995 16:55:52   MFALLEN
// File name trunkate improvement.
//
//    Rev 1.29   06 Apr 1995 19:09:58   MFALLEN
// Cosmetics.
//
//    Rev 1.28   28 Mar 1995 16:56:50   AMATIC
// Changed Help ID for ProblemsFound Dlg.
// n_repkil.cpp -> S:\navwork\vcs\n_repkil.cpv
// Enter description of changes made.
//
//    Rev 1.27   24 Mar 1995 10:03:10   MFALLEN
// Added context sensitive help. I don't think that the ID's are OK but
// Elizabeth told me to add these IDs.
//
//    Rev 1.26   21 Mar 1995 15:58:50   amatic
// Cosmetic UI rework.
// n_repkil.cpp -> S:\navwork\vcs\n_repkil.cpv
// Enter description of changes made.
//
//    Rev 1.25   20 Mar 1995 17:17:28   AMATIC
// Modifed VirusFound appearance.
// n_repkil.cpp -> S:\navwork\vcs\n_repkil.cpv
// Enter description of changes made.
//
//    Rev 1.24   14 Mar 1995 18:30:32   AMATIC
// Added HowTo context help support.
//
//    Rev 1.23   08 Mar 1995 11:47:28   AMATIC
// Added Help Button ...
//
//    Rev 1.22   22 Feb 1995 20:52:50   AMATIC
// Fixed IDS_VIRUSINFILE and replaced GetString By LoadString API.
//
//    Rev 1.21   21 Feb 1995 17:11:06   AMATIC
// Added Context Sensitive Help Support.
//
//    Rev 1.20   17 Feb 1995 09:23:42   MFALLEN
// Only one return per windows callback function.
//
//    Rev 1.19   25 Jan 1995 16:28:28   MFALLEN
// SYS4_HALT
//
//    Rev 1.18   22 Dec 1994 07:57:38   MFALLEN
// Added right click help. Help ID's are still missing.
//
//    Rev 1.17   20 Dec 1994 14:46:10   MFALLEN
// Sami did fixes on CERTLIBN
//
//    Rev 1.16   16 Dec 1994 14:20:16   MFALLEN
// Back using the old navxutil
//
//    Rev 1.15   07 Dec 1994 15:55:24   MFALLEN
// Added old NAV 3.0 activity log.
//
//    Rev 1.14   01 Dec 1994 10:05:04   MFALLEN
// Filename was not displayed correctly.
//
//    Rev 1.13   30 Nov 1994 12:33:28   MFALLEN
// 3D in W16
//
//    Rev 1.12   29 Nov 1994 13:37:18   MFALLEN
// Compiles for W16
//
//    Rev 1.11   21 Nov 1994 13:07:26   MFALLEN
// Instead of commenting out all the functions that does not work, they have
// been stubbed out instead.
//
//    Rev 1.10   16 Nov 1994 12:33:24   MFALLEN
// Commented out some NG_SetChildSmallFont() functions.
//
//    Rev 1.9   08 Nov 1994 15:41:38   MFALLEN
// Compiles fine under W32.
//
//
//    Rev 1.57   27 Oct 1994 00:17:28   DALLEE
// Don't allow boot repairs for defs with the DISPLAY_DIALOG flag set.
// These long repairs are for DOS only.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "ngui.h"
	#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "file.h"
#include "cmbstr.h"
#include "symcfg.h"
#include "stdhelp.h"

#include "navwork.h"
#include "navutil.h"
#include "options.h"
#include "exclude.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navw_hlp.h"
#include "navalog.h"
#include "security.h"

#include "stub.h"
#include "virscan.h"
#include "nonav.h"
#include "navapcom.h"
#include "navwcomm.h"

extern HINSTANCE hInstance;

typedef HBITMAP FAR *LPHBITMAP;

typedef struct
    {
    LPHBITMAP   lphBmpAnimation;
    LPHBITMAP   lphBmpAnimationMask;
    WORD        wTotalFrames;
    WORD        wCurrentFrame;
    UINT        uAnimationTimerID;
    } ANIMATION;

ANIMATION       Animation       = {0};

static			bDownloadAbortFailed = FALSE;


// ************************************************************************ //
// ****                       Local Prototypes                         **** //
// ************************************************************************ //

#ifdef SYM_WIN32

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
(
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,                       // [in] "message"
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
);

#endif

BOOL SYM_EXPORT WINAPI VirusDlgProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

WORD SYM_EXPORT WINAPI HaltCpuDlgProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

VOID LOCAL VirusDlgInit (
    HWND    hDlg,
    LPARAM  lParam
    );

VOID LOCAL ActionButtons (
    HWND    hDlg,
    LPNONAV lpNav,
    WORD    wFileStatus
    );

VOID LOCAL PASCAL AnimationStart (      // Starts Animation
    HWND        hDlg
    ) ;

VOID CALLBACK AnimationNextFrame (
    HWND    hWnd,
    UINT    uMessage,
    UINT    uEventID,
    DWORD   dwTime
    );

VOID LOCAL PASCAL AnimationEnd (
    HWND        hDlg
    );

VOID LOCAL PASCAL AnimationDrawFrame (
    LPDRAWITEMSTRUCT    lpDrawItem
    );

VOID LOCAL ShutDownNavapW ( VOID );

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

BOOL SYM_EXPORT WINAPI VirusFound(LPNONAV lpNav)
{
    extern  HINSTANCE   hInstance;
    auto    int         nStatus;

    auto    LPCTSTR     lpszTemplate;

    LongCustomMessage(NULL);

    if ( PROGRAM_DOWNLOAD == lpNav->lpScan->uProgramMode )
    {
        lpszTemplate = lpNav->Zip.bInZip ? MAKEINTRESOURCE( IDD_DOWNLOADVIRUSZIPDLG ) :
                                           MAKEINTRESOURCE( IDD_DOWNLOADVIRUSDLG );
    }
    else
    {
        lpszTemplate = MAKEINTRESOURCE( IDD_VIRUSDLG );
    }

    bDownloadAbortFailed = FALSE;

    nStatus = DialogBoxParam( hInstance,
                              lpszTemplate,
                              lpNav->hMommyWindow,
                              VirusDlgProc,
                              ( LPARAM ) lpNav );

    if (nStatus == -1)
    {
        nStatus = TRUE;
    }

    return ( (BOOL)nStatus );
} // VirusFound()

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

BOOL SYM_EXPORT WINAPI VirusDlgProc(
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    BOOL                bReturn = FALSE;
    auto    REPAIRFILESTRUCT    Repair;
    auto    BOOL                bStatus;
    auto    LPNONAV             lpNav;
    auto    CHECKSTRUCT         PwCheck;
    auto    UINT                uStatus;

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
            break;
        case WM_INITDIALOG:
                                        // Init the Immediate Notify or Download controls:
            NG_SetFontsInDialog (hDlg, 0);
            VirusDlgInit (hDlg, lParam);

            bReturn = TRUE;
            break;

        case WM_DRAWITEM:
            AnimationDrawFrame ( (LPDRAWITEMSTRUCT)lParam );
            break;

        case WM_DESTROY:
                                        // ------------------------------------
                                        // Stop and Destroy Usr. Animation
                                        // ------------------------------------
            AnimationEnd ( hDlg );
            LP_FREE(hDlg);
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // or standard context sensitive help
                                        // ----------------------------------
            OnContextMenu ( hDlg, wMessage, wParam, lParam );
            bReturn = TRUE;
            break;
#endif
        case WM_COMMAND:
            {
            lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
            if (!lpNav)
                {LP_SET (hDlg, GloballySaved_lpNav);
                 lpNav = (LPNONAV)LP_GET(hDlg);
                 SYM_ASSERT (lpNav);}
#endif

            switch ( GET_WM_COMMAND_ID ( wParam, lParam ) )
                {
                case IDHELP:
                    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
                        {
                                        // Downloading a zip ...
                        if (lpNav->Zip.bInZip == TRUE)
                            {
                            StdHelpFromDialog(
                                  hDlg,
                                  IDH_NAVW_VIRUS_DOWNLOAD_ZIP_HELP_BTN );
                            }
                        else            // Downloading a file ...
                            {
                            StdHelpFromDialog(
                                  hDlg,
                                  IDH_NAVW_VIRUS_DOWNLOAD_FILE_HELP_BTN );
                            }
                        }               // End "if (Download Mode) ..."
                    else
                        {               // Otherwise, not Download Mode ...
//&^                        StdHelpFromDialog (
//                              hDlg,
//                              IDH_NAVW_VIRUS_FOUND_DLG_HELP_BTN );
                        }
                    break;

                case IDOK:
                    break;

                case IDCANCEL:
                    break;

                case IDI_V_REPAIR:
                    if (lpNav->Infected.wStatus == FILESTATUS_BOOTINFECTED ||
                        lpNav->Infected.wStatus == FILESTATUS_MASTERBOOTINFECTED)
                        {
                        bStatus = RepairBoot (lpNav, hDlg, TRUE);

                        if (bStatus == TRUE)
                            {
                            if (lpNav->Infected.wStatus == FILESTATUS_BOOTINFECTED)
                                lpNav->Stats.BootRecs.uCleaned++;
                            else
                                lpNav->Stats.MasterBootRec.uCleaned++;
                            }
                        }
                    else
                        {
                        bStatus = FALSE;

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
                        }

                    lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_REPAIR, bStatus);

                    if (bStatus == TRUE)
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_V_CONTINUE,
                                        GetDlgItem ( hDlg, IDI_V_CONTINUE ),
                                        BN_CLICKED, PostMessage );
                        }
                    else
                        {
                        if ( (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD) && (lpNav->Opt.bDispDelete == TRUE) )
                            {
                                        // In download mode, if repair
                                        // was not possible, automatically
                                        // set the focus to the delete
                                        // button.
                            SetFocus (GetDlgItem(hDlg, IDI_V_DELETE));
                            }
                            else
                            {
                            if (lpNav->Opt.bDispCont == TRUE)
                                SetFocus (GetDlgItem(hDlg, IDI_V_CONTINUE));
                                else
                                SetFocus (GetDlgItem(hDlg, IDI_V_STOP));
                            }
                        }

                    ActionButtons(hDlg, lpNav, lpNav->Infected.wStatus);
                    break;

                case IDI_V_DELETE:
                    if ( (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD) &&
                         (lpNav->Zip.bInZip == TRUE) )
                       {
                                        // This is a special case!
                                        // Take care of this at the end
                                        // of the scan!!!
                       lpNav->lpScan->bDeleteZipFile = TRUE;
                       PostMessage(hDlg, WM_COMMAND, IDI_V_STOP, 0L);

                                        // Bad to break in the middle, but
                                        // much easier to read in this case.
                       break;
                       }

                    bStatus = NavDeleteFile (lpNav, hDlg, TRUE);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uDeleted++;

                    if (!bStatus)
                        {
                        bDownloadAbortFailed = TRUE;
                        }

                    lpNav->Infected.wStatus = UpdateFileStatus (
                        lpNav->Infected.wStatus, PROCESS_DELETE, bStatus);

                                        // Don't save the file if it
                                        // was deleted:
                    if (bStatus == TRUE)
                        lpNav->lpScan->bFileDeleted = TRUE;

                    if (bStatus == TRUE)
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_V_CONTINUE,
                                        GetDlgItem ( hDlg, IDI_V_CONTINUE ),
                                        BN_CLICKED, PostMessage );
                        }
                    else
                        ActionButtons(hDlg, lpNav, lpNav->Infected.wStatus);
                    break;

                case IDI_V_QUARANTINE:
                    bStatus = NavMoveFile (lpNav, hDlg, TRUE);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uQuarantined++;

                    lpNav->Infected.wStatus = UpdateFileStatus (
                        lpNav->Infected.wStatus, PROCESS_QUARANTINE, bStatus);

                                        // Don't save the file if it
                                        // was deleted:
                    if (bStatus == TRUE)
                        lpNav->lpScan->bFileDeleted = TRUE;

                    if (bStatus == TRUE)
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_V_CONTINUE,
                                        GetDlgItem ( hDlg, IDI_V_CONTINUE ),
                                        BN_CLICKED, PostMessage );
                        }
                    else
                        ActionButtons(hDlg, lpNav, lpNav->Infected.wStatus);
                    break;
                
                case IDI_V_EXCLUDE:
                                        // Setup the password check structure.
                    MEMSET (&PwCheck, EOS, sizeof(PwCheck));
                    PwCheck.hWnd         = hDlg;
                    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
                    PwCheck.uID          = PASSWORD_MODIEXCLUSIONS;

                                        // Ask for a password if we need to.
                    if (AskPassword(&PwCheck) == PASSWORD_ASK)
                        uStatus = PasswordCheck (&PwCheck, FALSE, FALSE);
                    else
                        uStatus = TRUE;

                                        // Only proceed if we have the proper
                                        // access.
                    if ( TRUE == uStatus )
                        {
                        bStatus = ExcludeAddFile (lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                                lpNav->Infected.wStatus, PROCESS_EXCLUDE, bStatus);

                        if (bStatus == TRUE )
                            {
                            FORWARD_WM_COMMAND ( hDlg, IDI_V_CONTINUE,
                                            GetDlgItem ( hDlg, IDI_V_CONTINUE ),
                                            BN_CLICKED, PostMessage );
                            }
                        else
                            ActionButtons(hDlg, lpNav, lpNav->Infected.wStatus);
                        }
                    break;

                case IDI_V_INFO:
                    FoundVirusInfo(hDlg, lpNav);
                    break;

                case IDI_V_STOP:
                    AddVirusInfo (lpNav);
                    lpNav->bDoScan = FALSE;
                    EndDialog(hDlg, STOP);
                    break;

                case IDI_V_CONTINUE:
                    AddVirusInfo (lpNav);
                    EndDialog(hDlg, CONTINUE);
                    break;
                }
            }
        }

    return ( bReturn );
}


VOID LOCAL VirusDlgInit (HWND hDlg, LPARAM lParam)
{
    const   int         cMaxMsgSize   = (256 * 2);
    const   int         cShPathConst  = 120;

    auto    LPSTR       lpMsg       [cMaxMsgSize];
    auto    LPSTR       lpString    [cMaxMsgSize];
    auto    char        szZipPath   [SYM_MAX_PATH+1];
    auto    char        szFileName  [SYM_MAX_PATH+1];
    auto    char        szTempName  [SYM_MAX_PATH+1];

    auto    LPNONAV     lpNav;
    auto    UINT        uMsgStatus, uMsgInfo;



    Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
    NG_SetChildSmallFont (hDlg, IDI_V_MSG);
    NAV_CenterDialog (hDlg);

    lpNav = (LPNONAV)lParam;
    LP_SET(hDlg, lpNav);

    switch (lpNav->Infected.wStatus)
        {
        case FILESTATUS_BOOTINFECTED:
            uMsgStatus   = IDS_VIRUSINBOOT;
            uMsgInfo     = IDS_VIRUSINBOOT_EX;
            break;

        case FILESTATUS_MASTERBOOTINFECTED:
            uMsgStatus   = IDS_VIRUSINMASTERBOOT;
            uMsgInfo     = IDS_VIRUSINBOOT_EX;
            break;

        case FILESTATUS_ERROR:
            uMsgStatus   = IDS_FILEERROR;
            uMsgInfo     = IDS_FILEERROR_EX;
            break;

        case FILESTATUS_ZIPINFECTED:
                                        // If this is a download zip,
                                        // the wording is a special case:
            if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
                {
                uMsgStatus   = IDS_VIRUSINDOWNLOADZIP;
                }
            else
                {                       // Otherwise, use the usual wording:
                uMsgStatus   = IDS_VIRUSINZIPFILE;
                }
            uMsgInfo     = IDS_VIRUSINZIPFILE_EX;
            break;

        default:
                                        // If this is a download file,
                                        // the wording is a special case:
            if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
                {
                uMsgStatus   = IDS_VIRUSINDOWNLOADFILE;
                }
            else
                {                       // Otherwise, use the usual wording:
                uMsgStatus   = IDS_VIRUSINFILE;
                }
            uMsgInfo     = IDS_VIRUSINFILE_EX;
            break;
        }

                                        // Try to load Status Msg Format
    if ( ! LoadString (hInstance, uMsgStatus, (LPSTR)lpString, cMaxMsgSize) )
        return;                         // ERROR Loading Resource - RETURN

    if ( lpNav->Infected.wStatus == FILESTATUS_ZIPINFECTED )
        {
        char szTemp[ SYM_MAX_PATH + 1 ];
        ParseZipFileName (lpNav->Infected.szFullPath, szTemp, szFileName);

        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            {                           // If this is download mode,
                                        // show only the zip name (w/o the
                                        // path) of the infected zip :
            if ((NameReturnFile(szTemp, szTempName)) == 0)
                {                       // This call worked. szTempName
                                        // holds the file name without the
                                        // path. Copy it back to szZipPath,
                                        // as if we were never here:
                STRCPY ( szTemp , szTempName );
                }
            }                           // End download mode

        NameShortenFileName(szTemp, szZipPath, cShPathConst/2 );
        NameShortenFileName(szFileName, szTemp, cShPathConst/2 );
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            {
                                        // In download mode, the arguments
                                        // are phrased in a different order
                                        // in the output text.
            SPRINTF((LPSTR)lpMsg, (LPSTR)lpString,
                    (LPSTR)(lpNav->Infected.Notes.lpVirName),
                    (LPSTR)szTemp,
                    (LPSTR)szZipPath);
            }
        else
            {
                                        // Regular mode:
            SPRINTF((LPSTR)lpMsg, (LPSTR)lpString,
                    (LPSTR)szTemp,
                    (LPSTR)szZipPath,
                    (LPSTR)(lpNav->Infected.Notes.lpVirName) );
            }
        }
    else
        {
        if (lpNav->Infected.wStatus == FILESTATUS_MASTERBOOTINFECTED)
            {
            SPRINTF (szZipPath,
                     "%d",
                     lpNav->Infected.szFullPath[0]);
            }
        else
            {
            STRCPY (szZipPath,
                    lpNav->Infected.szFullPath);

            if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
                {                       // If this is download mode,
                                        // show only the file name, not the
                                        // path, of the infected file :
                if (0 == NameReturnFile(szZipPath, szTempName))
                    {                   // This call worked. szTempName
                                        // holds the file name without the
                                        // path. Copy it back to szZipPath,
                                        // as if we were never here:
                    NameShortenFileName ( szTempName,
                                          szZipPath,
                                          cShPathConst );
                    }
                }                       // End download mode
            else
                {
                                        // Not download mode:
                NameShortenFileName ( lpNav->Infected.szFullPath,
                                      szZipPath, cShPathConst );
                }
            }
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
                                        // Arguments are in a different
                                        // order in the download phrasing:
            {
            SPRINTF ((LPSTR)lpMsg, (LPSTR)lpString, (LPSTR)(lpNav->Infected.Notes.lpVirName),
                        (LPSTR)szZipPath);
            }
        else
            {                           // Regular phrasing:
            SPRINTF ((LPSTR)lpMsg, (LPSTR)lpString, (LPSTR)szZipPath,
                                      (LPSTR)(lpNav->Infected.Notes.lpVirName));
            }
        }
                                        // Display File Status Msg:
    SetDlgItemText(hDlg, IDI_V_MSG, (LPSTR)lpMsg);

                                        // Try to load Additional Info string
    if ( uMsgInfo && LoadString(hInstance, uMsgInfo, (LPSTR)lpMsg, cMaxMsgSize))
        {                               // - and question - "what to do?"
        SetDlgItemText(hDlg, IDI_V_MSG_EX, (LPSTR)lpMsg);
        }

                                        // Display the custom "Info ..."
                                        // text message.
    if ( LoadString(hInstance, IDS_VIRUSINDOWNLOAD_INFO, (LPSTR)szTempName, sizeof(szTempName)))
        {
        SPRINTF ((LPSTR)lpMsg, (LPSTR)szTempName, (LPSTR)(lpNav->Infected.Notes.lpVirName));
        SetDlgItemText(hDlg, IDI_MSG_VIRUSINFO, (LPSTR)lpMsg);
        }

                                        // If there is alert message from CPL:
    if ( lpNav->Alert.bDispAlertMsg )
        SetDlgItemText(hDlg, IDI_V_ALERT, lpNav->Alert.szAlertMsg);

    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        {
        if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
            {
            if (lpNav->Zip.bInZip == TRUE)
                {
                NG_SetChildSmallFont (hDlg, IDI_MSG_VIRUSZIPDELETE);
                NG_SetChildSmallFont (hDlg, IDI_MSG_VIRUSZIPCONTINUE);
                NG_SetChildSmallFont (hDlg, IDI_MSG_VIRUSZIPSTOP);
                }
            else
                {
                NG_SetChildSmallFont (hDlg, IDI_MSG_VIRUSREPAIR);
                NG_SetChildSmallFont (hDlg, IDI_MSG_VIRUSDELETE);
                NG_SetChildSmallFont (hDlg, IDI_MSG_VIRUSCONTINUE);
                }
            }
        }

                                        // ---------------------------------
                                        // Enable all the buttons that apply
                                        // ---------------------------------

    ActionButtons (hDlg, lpNav, lpNav->Infected.wStatus);

    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
        {                               // In download mode:
                                        // Restore the main window icon back
                                        // to a doctor's bag, and get rid
                                        // of the "99%" caption:
        DrawNavIcon(lpNav->hMommyWindow, 100);
        }                               // End "if (download mode) ... "
                                        // ---------------------------------
                                        // Start siren animation.
                                        // ---------------------------------
    AnimationStart ( hDlg );
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

WORD SYM_EXPORT WINAPI HaltCPU (HWND hDlg, LPNONAV lpNav)
{
    auto    DLGPROC     lpfn;
    auto    int         nStatus;

    lpfn = (DLGPROC) MakeProcInstance((FARPROC)HaltCpuDlgProc, hInstance);
    nStatus = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CRITERRDLG),
                            hDlg, lpfn, (LPARAM)lpNav);
    FreeProcInstance((FARPROC) lpfn);

                                        // If we failed on the create,
    if ( nStatus == -1 )                // we still want to stop everything.
        {
                                        // Tell AP to unload so we get stuck
                                        // waiting for a blocked AP comm thread
        ShutDownNavapW();
        NavHaltSystem();
        }

    return (nStatus);
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

WORD SYM_EXPORT WINAPI HaltCpuDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    static  LPNONAV     lpNav;
    static  BOOL        bPaint;
    auto    BOOL        bReturn = FALSE;
    auto    char        szZipPath   [SYM_MAX_PATH+1];
    auto    char        szFileName  [SYM_MAX_PATH+1];
    auto    LPSTR       lpMsg;
    auto    LPSTR       lpString;
    auto    UINT        uMsgID;

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
            break;
        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, IDI_CR_MSG);
            NG_SetGroupBoxSunken(hDlg, IDI_CR_BOX);
            NAV_CenterDialog (hDlg);
            bPaint = FALSE;

            lpNav = (LPNONAV)lParam;

            lpMsg = lpString = NULL;
            if ((lpString = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE * 2))     != NULL &&
                (lpMsg    = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE * 4)) != NULL)
                {
                switch (lpNav->Infected.wStatus)
                    {
                    case FILESTATUS_MEMORYINFECTED:
                        uMsgID = IDS_VIRUSINMEM;
                        break;

                    case FILESTATUS_INFECTED:
                        uMsgID = IDS_VIRUSINFILE;
                        break;

                    case FILESTATUS_ZIPINFECTED:
                        uMsgID = IDS_VIRUSINZIPFILE;
                        break;

                    case FILESTATUS_MASTERBOOTINFECTED:
                        uMsgID = IDS_VIRUSINMASTERBOOT;
                        break;

                    default:
                        uMsgID = IDS_VIRUSINBOOT;
                        break;
                    }

                if (LoadString(hInstance, uMsgID, lpString, SIZE_MESSAGE*2))
                    {
                    if (lpNav->Infected.wStatus == FILESTATUS_ZIPINFECTED)
                        {
                        ParseZipFileName (lpNav->Infected.szFullPath,
                                          szZipPath, szFileName);

                        NameShortenFileName ( lpNav->Infected.szFullPath,
                                              szZipPath, 45 );

                        SPRINTF (lpMsg,
                                 lpString,
                                 (LPSTR)szFileName,
                                 (LPSTR)szZipPath,
                                 (LPSTR)lpNav->Infected.Notes.lpVirName);
                        }
                    else
                        {
                        if (lpNav->Infected.wStatus == FILESTATUS_MASTERBOOTINFECTED)
                            {
                            SPRINTF (szZipPath,
                                     "%d",
                                     lpNav->Infected.szFullPath[0]);
                            }
                        else
                            {
                            STRCPY (szZipPath,
                                    lpNav->Infected.szFullPath);
                            NameShortenFileName ( lpNav->Infected.szFullPath,
                                                  szZipPath, 45 );
                            }

                        SPRINTF (lpMsg,
                                 lpString,
                                 (LPSTR)szZipPath,
                                 (LPSTR)lpNav->Infected.Notes.lpVirName);
                        }
                    }

                SetDlgItemText(hDlg, IDI_CR_MSG, lpMsg);

                if ( lpNav->Alert.bDispAlertMsg )
                    SetDlgItemText(hDlg, IDI_CR_ALERT, (LPSTR)lpNav->Alert.szAlertMsg);
                }

            if (lpMsg)
                MemFreePtr(lpMsg);

            if (lpString)
                MemFreePtr(lpString);

                                        // ------------------------------------
                                        // If halting the computer for any
                                        // other reson that memory infection,
                                        // notify other apps about it.
                                        // ------------------------------------
#ifdef SYM_WIN16
            if ( lpNav->Infected.wStatus != FILESTATUS_MEMORYINFECTED )
                {
                SendMessage (HWND_BROADCAST, WM_QUERYENDSESSION, 0, 0L);
                SendMessage (HWND_BROADCAST, WM_ENDSESSION, TRUE, 0L);
                }
#endif
            bReturn = TRUE;
            break;


        case WM_PAINT:
            if ( bPaint == FALSE )
                {
                bPaint = TRUE;
                SetTimer ( hDlg, 2, 3000, NULL );
                }
            break;

        case WM_TIMER:
            {
            KillTimer ( hDlg, 2 );
            UpdateWindow(hDlg);

            LOGSTRUCT   Log = { 0 };

            Log.uEvent       = LOGEV_HALTCPU;
            Log.lpFileName   = lpNav->Infected.szFullPath;
            Log.lpVirusName  = lpNav->Infected.Notes.lpVirName;
            Log.lpAct        = &lpNav->Act;
            LogEvent (&Log);


                                        // Tell AP to unload so we get stuck
                                        // waiting for a blocked AP comm thread
            ShutDownNavapW();
            NavHaltSystem();

                                        // DALLEE: I don't think it gets here
                                        // anymore. Shutdown is unconditional.
                                        // ???

                                        // If we asked for a halt but the
                                        // user elected to cancel the close
                                        // of one of his apps, we will get
                                        // here. Close this dialog and return
                                        // to caller.
            PostQuitMessage(0);
            EndDialog(hDlg, TRUE);      // Tell the caller we are infected.
            }
            break;
        }

    return ( bReturn );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID LOCAL ActionButtons (
    HWND    hDlg,
    LPNONAV lpNav,
    WORD    wFileStatus
    )
{
    extern  FILESTATUSSTRUCT    FileStatus[];

    EnableWindow (GetDlgItem(hDlg, IDI_V_REPAIR),
                  (lpNav->Opt.bDispRepair) ?
                   FileStatus[wFileStatus].byCanRepair : FALSE);
                                    // In viewer mode, we can ALWAYS
                                    // Delete the file, unless we already tried
                                                                            // and failed ...
    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
        EnableWindow (GetDlgItem(hDlg, IDI_V_DELETE), !bDownloadAbortFailed);
    else
        EnableWindow (GetDlgItem(hDlg, IDI_V_DELETE),
                   (lpNav->Opt.bDispDelete) ?
                   FileStatus[wFileStatus].byCanDelete : FALSE);

                                        // ---------------------------------
                                        // This is special case for DIR2
                                        // virus infections.
                                        // ---------------------------------

    if (lpNav->Infected.Notes.wInfo & bitINFO_DIR2)
        {
        EnableWindow (GetDlgItem(hDlg, IDI_V_DELETE), FALSE);
        }

    if (lpNav->Infected.Notes.wControl2 & DISPLAY_DIALOG)
        {
        EnableWindow (GetDlgItem(hDlg, IDI_V_REPAIR), FALSE);
        EnableWindow (GetDlgItem(hDlg, IDI_V_DELETE), FALSE);
        }

    EnableWindow (GetDlgItem(hDlg, IDI_V_EXCLUDE),
                   lpNav->Opt.bDispExcl &&
                   FileStatus[wFileStatus].byCanExclude &&
                   !NavTestOemToAnsi (lpNav->Infected.szFullOemPath));

    EnableWindow (GetDlgItem(hDlg, IDI_V_INFO),
                   FileStatus[wFileStatus].byHasVirusInfo);

                                        // In viewer mode, we should ALWAYS
                                        // Allow Continue or Stop.
    if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
       {
       EnableWindow (GetDlgItem(hDlg, IDI_V_CONTINUE), TRUE);
       EnableWindow (GetDlgItem(hDlg, IDI_V_STOP),     TRUE);
       }
    else
       {
       EnableWindow (GetDlgItem(hDlg, IDI_V_CONTINUE), lpNav->Opt.bDispCont);
       EnableWindow (GetDlgItem(hDlg, IDI_V_STOP),     lpNav->Opt.bAllowScanStop);
       }

    // Guarantee we don't run out of buttons
    if ( !IsWindowEnabled (GetDlgItem(hDlg, IDI_V_REPAIR)) &&
         !IsWindowEnabled (GetDlgItem(hDlg, IDI_V_DELETE)) &&
         !IsWindowEnabled (GetDlgItem(hDlg, IDI_V_EXCLUDE)) &&
         !IsWindowEnabled (GetDlgItem(hDlg, IDI_V_STOP)))
        {
        EnableWindow (GetDlgItem(hDlg, IDI_V_CONTINUE), TRUE );
        }
}

//***************************************************************************
// AnimationStart()
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
//      AnimationEnd, AnimationNextFrame, AnimationDrawFrame
//
//***************************************************************************
// 07/02/1995 MARKL Function Created.
//***************************************************************************
VOID LOCAL PASCAL AnimationStart (      // Starts Animation
    HWND        hDlg
    )
{
                                        // -----------------------------------
                                        // Array of BMP IDs which will be used
                                        // as animated images in a cycle.
                                        // Note: ZERO TERMINATION is a MUST !
    static UINT aBmpIds [] =            // -----------------------------------
        {
        ID_NAVVIRFOUND_ANIM_BMP_1,
        ID_NAVVIRFOUND_ANIM_BMP_2,
        ID_NAVVIRFOUND_ANIM_BMP_3,
        ID_NAVVIRFOUND_ANIM_BMP_4,
        ID_NAVVIRFOUND_ANIM_BMP_5,
        ID_NAVVIRFOUND_ANIM_BMP_6,
        ID_NAVVIRFOUND_ANIM_BMP_7,
        ID_NAVVIRFOUND_ANIM_BMP_8,
        ID_NAVVIRFOUND_ANIM_BMP_9,
        ID_NAVVIRFOUND_ANIM_BMP_10,
        ID_NAVVIRFOUND_ANIM_BMP_11,
        ID_NAVVIRFOUND_ANIM_BMP_12,
        ID_NAVVIRFOUND_ANIM_BMP_13,
        ID_NAVVIRFOUND_ANIM_BMP_14,
        ID_NAVVIRFOUND_ANIM_BMP_15,
        ID_NAVVIRFOUND_ANIM_BMP_16,
        0
        };
                                        // -----------------------------------
                                        // Array of BMP Mask IDs which will be
                                        // used as animated images in a cycle.
    static UINT aBmpMaskIds [] =        // Note: ZERO TERMINATION is a MUST !
        {                               // -----------------------------------
        ID_NAVVIRFOUND_ANIM_MASK_BMP_1,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_2,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_3,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_4,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_5,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_6,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_7,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_8,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_9,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_10,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_11,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_12,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_13,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_14,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_15,
        ID_NAVVIRFOUND_ANIM_MASK_BMP_16,
        0
        };

    WORD        wCurBmp;

                                        // Allocate the memory needed for
                                        // our animation bitmaps and their
                                        // masks.
    Animation.wTotalFrames    = sizeof (aBmpIds) / sizeof (UINT) - 1;
    Animation.lphBmpAnimation =
        (LPHBITMAP)MemAllocPtr ( GMEM_ZEROINIT | GMEM_MOVEABLE,
                                 Animation.wTotalFrames * sizeof (HBITMAP) );
    Animation.lphBmpAnimationMask =
        (LPHBITMAP)MemAllocPtr ( GMEM_ZEROINIT | GMEM_MOVEABLE,
                                 Animation.wTotalFrames * sizeof (HBITMAP) );

    if ( Animation.lphBmpAnimation && Animation.lphBmpAnimationMask )
        {
                                        // Load up our bitmaps and their
                                        // masks.
        for ( wCurBmp = 0;
              aBmpIds[wCurBmp] && ( wCurBmp < Animation.wTotalFrames );
              wCurBmp++ )
            {
            Animation.lphBmpAnimation[wCurBmp] =
                LoadBitmap ( hInstance,
                             MAKEINTRESOURCE ( aBmpIds[wCurBmp] ) );
            Animation.lphBmpAnimationMask[wCurBmp] =
                LoadBitmap ( hInstance,
                             MAKEINTRESOURCE ( aBmpMaskIds[wCurBmp] ) );
            }
        if ( wCurBmp != Animation.wTotalFrames )
            Animation.wTotalFrames = wCurBmp;

                                        // Start our animation.
        Animation.uAnimationTimerID = SetTimer ( hDlg, 6, 100,
                                                 AnimationNextFrame );
        }
}



//***************************************************************************
// AnimationNextFrame()
//
// Description:
//      Forces the animation to move on to the next frame.
//
// Parameters:
//      hDlg                            [in] Dialog box handle
//
// See Also:
//      AnimationDrawFrame
//
//***************************************************************************
// 07/02/1995 MARKL Function Created.
//***************************************************************************
VOID CALLBACK AnimationNextFrame (
    HWND    hWnd,
    UINT    uMessage,
    UINT    uEventID,
    DWORD   dwTime
    )
{
    InvalidateRect ( GetDlgItem ( hWnd, IDC_ANIMATION ), NULL, FALSE );

    Animation.wCurrentFrame++;
    if ( Animation.wCurrentFrame == Animation.wTotalFrames )
        Animation.wCurrentFrame = 0;
}



//***************************************************************************
// AnimationEnd()
//
// Description:
//      Terminates the animation, cleaning up all resources, memory, and
//      timer.
//
// Parameters:
//      hDlg                            [in] Dialog box handle
//
// See Also:
//      AnimationStart
//
//***************************************************************************
// 07/02/1995 MARKL Function Created.
//***************************************************************************
VOID LOCAL PASCAL AnimationEnd (
    HWND        hDlg
    )
{
    WORD wCurBmp;

                                        // Stop our animation!
    if ( Animation.uAnimationTimerID )
        KillTimer ( hDlg, Animation.uAnimationTimerID );

                                        // Free up our resoureces.
    if ( Animation.lphBmpAnimation && Animation.lphBmpAnimationMask )
        {
        for ( wCurBmp = 0; wCurBmp < Animation.wTotalFrames; wCurBmp++ )
            {
            if ( Animation.lphBmpAnimation[wCurBmp] )
                DeleteObject ( Animation.lphBmpAnimation[wCurBmp] );
            if ( Animation.lphBmpAnimationMask[wCurBmp] )
                DeleteObject ( Animation.lphBmpAnimationMask[wCurBmp] );
            }
        }

                                        // Free our memory.
    if ( Animation.lphBmpAnimation )
        MemFreePtr ( Animation.lphBmpAnimation );
    if ( Animation.lphBmpAnimationMask )
        MemFreePtr ( Animation.lphBmpAnimationMask );
    Animation.lphBmpAnimation = Animation.lphBmpAnimationMask = NULL;
}



//***************************************************************************
// AnimationDrawFrame()
//
// Description:
//      This routine draws a frame of animation, taking in to account the
//      background, and doing so such that there is no flicker.
//
// Parameters:
//      lpDrawItem                      [in] DrawItem from ownder draw msg
//
// See Also:
//      AnimationStart, AnimationNextFrame
//
//***************************************************************************
// 07/02/1995 MARKL Function Created.
//***************************************************************************
VOID LOCAL PASCAL AnimationDrawFrame (
    LPDRAWITEMSTRUCT    lpDrawItem
    )
{
    HBITMAP     hBmp            = NULL;
    HBITMAP     hBmpMask        = NULL;
    HBITMAP     hBmpMem         = NULL;
    HBITMAP     hBmpOld         = NULL;
    HDC         hDCMem          = NULL;
    RECT        rBitmap;

                                        // Get the bitmap and the mask we
                                        // will be working with.
    hBmp     = Animation.lphBmpAnimation[Animation.wCurrentFrame];
    hBmpMask = Animation.lphBmpAnimationMask[Animation.wCurrentFrame];

    if ( hBmp && hBmpMask )
        {
                                        // Get the size of the client rect
                                        // and create a compatible DC as
                                        // well as a compatible bitmap the
                                        // size of our client rect.
        hDCMem  = CreateCompatibleDC ( lpDrawItem->hDC );
        hBmpMem = CreateCompatibleBitmap ( lpDrawItem->hDC,
                                           lpDrawItem->rcItem.right -
                                               lpDrawItem->rcItem.left,
                                           lpDrawItem->rcItem.bottom -
                                               lpDrawItem->rcItem.top );

        if ( hDCMem && hBmpMem )
            {
                                        // Select the bitmap into the off-
                                        // screen DC,
            hBmpOld = (HBITMAP)SelectObject ( hDCMem, hBmpMem );

                                        // Adjust our bitmap rectangle
                                        // coordinates.
            rBitmap.left   = 0;
            rBitmap.top    = 0;
            rBitmap.right  = lpDrawItem->rcItem.right - lpDrawItem->rcItem.left;
            rBitmap.bottom = lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;

                                        // In our off-screen DC, erase the
                                        // background such that we don't
                                        // leave trails.
            SetBkColor ( hDCMem, GetSysColor ( COLOR_BTNFACE ) );
            ExtTextOut ( hDCMem, 0, 0, ETO_OPAQUE, &rBitmap, "", 0, NULL );

                                        // In our off-screen DC, now draw
                                        // our bitmap with its mask.
            NGDrawMaskedBitmap ( hDCMem, hBmp, hBmpMask,
                                 lpDrawItem->rcItem.left, lpDrawItem->rcItem.top );

                                        // Now blit the changes to the
                                        // screen DC for a nice smooth
                                        // effect with no flicker.
            BitBlt ( lpDrawItem->hDC,
                     lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
                     lpDrawItem->rcItem.right - lpDrawItem->rcItem.left,
                     lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top,
                     hDCMem,
                     0, 0,
                     SRCCOPY );
            }
        }

                                        // Time for cleanup.
    if ( hDCMem )
        {
        SelectObject ( hDCMem, hBmpOld );
        DeleteDC ( hDCMem );
        }
    if ( hBmpMem )
        {
        DeleteObject ( hBmpMem );
        }
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
            HWND    hDlg,
            UINT    uMsg
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

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
(
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
)
{
	LPNONAV lpNav = (LPNONAV)LP_GET(hDlg);
	if (!lpNav)
		{LP_SET (hDlg, GloballySaved_lpNav);
		lpNav = (LPNONAV)LP_GET(hDlg);
		SYM_ASSERT (lpNav);}
                                        // -----------------------------------
                                        // This array is a cross reference
                                        // between a control id number and a
                                        // help id number that should be
                                        // displayed when the user selects the
                                        // 'What's This' menu entry.
                                        // -----------------------------------


    static  DWORD       dwWhatsThisMap[] =
        {
        IDI_V_REPAIR,   IDH_NAVW_WHATHIS_REPAIR_BTN,
        IDI_V_DELETE,   IDH_NAVW_WHATHIS_DELETE_BTN,
        IDI_V_EXCLUDE,  IDH_NAVW_WHATHIS_EXCLUDE_BTN,
        IDI_V_INFO,     IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_BTN,
        IDI_V_STOP,     IDH_NAVW_WHATHIS_STOP_BTN,
        IDI_V_CONTINUE, IDH_NAVW_WHATHIS_CONTINUE_BTN,
        0,              0
        };

    static  DWORD       dwWhatsThisMap_DownloadFile[] =
        {
        IDI_V_REPAIR,   IDH_NAVW_WHATHIS_VIRUS_FOUND_IN_DWNLD_REPAIR,
        IDI_V_DELETE,   IDH_NAVW_WHATHIS_VIRUS_FOUND_IN_DWNLD_ABORT,
        IDI_V_INFO,     IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_BTN,
        IDI_V_CONTINUE, IDH_NAVW_WHATHIS_VIRUS_FOUND_IN_DWNLD_IGNORE,
        0,              0
        };

    static  DWORD       dwWhatsThisMap_DownloadCompressed[] =
        {
        IDI_V_REPAIR,   IDH_NAVW_WHATHIS_VIRUS_FOUND_IN_DWNLD_REPAIR,
        IDI_V_DELETE,   IDH_NAVW_WHATHIS_VIRUS_FOUND_IN_DWNLD_ABORT,
        IDI_V_INFO,     IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_BTN,
        IDI_V_STOP,     IDH_NAVW_WHATHIS_VIRUS_FOUND_IN_DWNLD_IGNORE_ALL,
        IDI_V_CONTINUE, IDH_NAVW_WHATHIS_VIRUS_FOUND_IN_DWNLD_COMPRSSED_FILE_IGNORE,
        0,              0
        };


	LPDWORD	lpdwWhatsThisPtr = dwWhatsThisMap;
	if (lpNav->lpScan->uProgramMode == PROGRAM_DOWNLOAD)
		{
		if (lpNav->Zip.bInZip == TRUE)
			lpdwWhatsThisPtr = dwWhatsThisMap_DownloadCompressed;
			else
			lpdwWhatsThisPtr = dwWhatsThisMap_DownloadFile;
		}

    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        lpdwWhatsThisPtr,
                        NULL, 
                        NULL,           /* This dlg. does not have any LB Ctrl*/
                        NULL            /* that's why these nulls             */
                       );
}

#endif


//************************************************************************
// ShutDownNavapW()
//
// Close down the agent before shutdown so we don't appear to hang
// waiting on it.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 10/16/1996 DALLEE, created.
// 07/29/1997 DALLEE, match *#%!! changes to NAVAPW window class defines.
// 10/1/97 Mike Dunn -- Bug fix for Roswell 95 NEC. See comments below.
//                      (ported from ROSNEC 2/17/98)
//************************************************************************

VOID LOCAL ShutDownNavapW ( VOID )
{
    auto    HWND        hWndNavapW;
    auto    UINT        uNavapWMessage;


                                        // Tell AP to unload so we get stuck
                                        // waiting for a blocked AP comm thread
    if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        {
        hWndNavapW = FindWindow( NAVAP_WND_CLASS, NULL );
        }
    else
        {
        hWndNavapW = FindWindow( NAVAP_95_WND_CLASS, NULL );
        }

    if ( NULL != hWndNavapW )
        {
        uNavapWMessage = RegisterWindowMessage( NAVW_NAVAPW_COMM );

        if ( NULL != uNavapWMessage )
            {
            // Changed this from a PostMessage() so that the act of
            // closing the Agent is synchronous.  On slow NECs, the timing
            // was such that NAVW started the shutdown before the Agent
            // processed the message, resulting in a hung system (which is
            // the bug that the PostMessage() was supposed to fix).

            SendMessage( hWndNavapW, uNavapWMessage, NAVW_CMD_UNLOADNAVAPW, 0 );
            }
        }
} // ShutDownNavapW()

