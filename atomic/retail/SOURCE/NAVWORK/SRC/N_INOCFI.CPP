// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_inocfi.cpv   1.3   02 Jul 1998 15:12:58   mdunn  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_inocfi.cpv  $
// 
//    Rev 1.3   02 Jul 1998 15:12:58   mdunn
// fixed build 22 breakages on help IDs
// 
//    Rev 1.2   15 Jul 1997 14:36:48   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.1   22 May 1997 16:20:52   BILL
// Alpha changes (removed ctl3d)
// 
//    Rev 1.0   06 Feb 1997 21:02:52   RFULLER
// Initial revision
// 
//    Rev 1.4   11 Jul 1996 08:03:32   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.3   24 Jun 1996 21:27:32   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
//
//    Rev 1.2   22 Feb 1996 14:08:00   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.1   14 Feb 1996 17:01:40   JWORDEN
// Process internal file names in OEM mode to allow scan, and repair, and
// innoculation of file names containing certain non-ANSI translatable
// characters.
//
//    Rev 1.0   30 Jan 1996 15:55:26   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:30   BARRY
// Initial revision.
//
//    Rev 1.26   30 Nov 1995 22:42:08   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.25   09 May 1995 10:30:48   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "ngui.h"
	#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "cmbstr.h"
#include "symcfg.h"
#include "stdhelp.h"

#include "navutil.h"
#include "options.h"
#include "exclude.h"
#include "security.h"

#include "stub.h"
#include "virscan.h"
#include "nonav.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navw_hlp.h"

extern   HINSTANCE   hInstance;

// ************************************************************************ //
// ****                       Local Prototypes                         **** //
// ************************************************************************ //

#ifdef SYM_WIN32

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
    (
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

VOID LOCAL OnChangeContextMenu          // Respond to WM_CONTEXTMENU message
    (
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

#endif

BOOL SYM_EXPORT WINAPI InocFileDlgProc  (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

BOOL SYM_EXPORT WINAPI InocFileChangedDlgProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

VOID LOCAL EnableInocChangeButtons (
    HWND    hDlg,
    LPNONAV lpNav
    );

VOID LOCAL PASCAL EnableInocFileButton (
    HWND    hDlg,
    WORD    wIDButton,
    BOOL    bEnabled
    );

VOID LOCAL PASCAL PrepareInocDlgEnd (
    LPNONAV lpNav,
    BOOL    bStop
    );

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI InoculateFile (LPNONAV lpNav)
{
   extern       HINSTANCE       hInstance;
   auto         DLGPROC         lpfn;
   auto         int             nStatus;


   lpfn = (DLGPROC) MakeProcInstance((FARPROC)InocFileDlgProc, hInstance);
   nStatus = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_INOCFILE),
                            lpNav->hMommyWindow, lpfn, (LPARAM)lpNav);
   FreeProcInstance((FARPROC) lpfn);
   if (nStatus == -1)
      nStatus = TRUE;

   return ((BOOL)nStatus);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI InocFileDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    static  BOOL            bVirusInBoot;
    auto    BOOL            bReturn = FALSE;
    auto    CHECKSTRUCT     PwCheck;
    auto    LPNONAV         lpNav;
    auto    LPSTR           lpMsg;
    auto    LPSTR           lpString;
    auto    UINT            uStatus;
    auto    char            szFullPath [ SYM_MAX_PATH+1 ];

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
            break;
        case WM_INITDIALOG:
            NAV_CenterDialog (hDlg);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, IDI_INOCFILE_MSG);
            NG_SetChildSmallFont (hDlg, IDI_INOCFILE_INFO);
            NG_SetGroupBoxSunken(hDlg, IDI_INOCFILE_BOX);
            lpNav = (LPNONAV)lParam;
            LP_SET(hDlg, lpNav);

            lpMsg = lpString = NULL;

            if ((lpString = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE))     != NULL &&
                (lpMsg    = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE * 2)) != NULL)
                {
                NameShortenFileName (
                                    lpNav->Infected.szFullPath,
                                    szFullPath,
                                    70
                                    );

                if (String_Get (IDS_FILEISNOTINOCULATED, lpString, SIZE_MESSAGE))
                    SPRINTF (lpMsg, lpString, (LPSTR)szFullPath,
                             (LPSTR)lpNav->Infected.Notes.lpVirName);

                SetDlgItemText(hDlg, IDI_INOCFILE_MSG, lpMsg);

                if ( lpNav->Alert.bDispAlertMsg )
                    SetDlgItemText (hDlg, IDI_INOCFILE_ALERT,
                                    lpNav->Alert.szAlertMsg);
                }

            if (lpMsg)
                MemFreePtr(lpMsg);

            if (lpString)
                MemFreePtr(lpString);

            if (lpNav->Inoc.uActionNewInoc == INOC_PROMPT)
                {
                EnableInocFileButton(hDlg, IDI_INOCFILE_INOC,
                              lpNav->Inoc.bDispInoc);

                EnableInocFileButton(hDlg, IDI_INOCFILE_STOP,
                              lpNav->Opt.bAllowScanStop);

                EnableInocFileButton(hDlg, IDI_INOCFILE_EXCLUDE,
                              lpNav->Inoc.bDispExcl &&
                              !NavTestOemToAnsi (lpNav->Infected.szFullOemPath));

                EnableInocFileButton(hDlg, IDI_INOCFILE_DELETE,
                              lpNav->Inoc.bDispDelete);

                                        // Continue is always set last
                EnableInocFileButton(hDlg, IDI_INOCFILE_CONTINUE,
                              lpNav->Inoc.bDispCont);
                }
            else
                {
                EnableWindow (GetDlgItem(hDlg, IDI_INOCFILE_INOC), FALSE);
                EnableWindow (GetDlgItem(hDlg, IDI_INOCFILE_STOP), FALSE);
                EnableWindow (GetDlgItem(hDlg, IDI_INOCFILE_CONTINUE), FALSE);
                EnableWindow (GetDlgItem(hDlg, IDI_INOCFILE_EXCLUDE), FALSE);
                EnableWindow (GetDlgItem(hDlg, IDI_INOCFILE_DELETE), FALSE);
                }

            bReturn = TRUE;
            break;
                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
                {
                StdHelpSetContext (HELP_DLG_ASK_ACTION,
                                   HELP_DLG_ASK_ACTION);
                }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

#ifdef SYM_WIN32
        case WM_CONTEXTMENU:
        case WM_HELP:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // ----------------------------------
            OnContextMenu ( hDlg, wMessage, wParam, lParam );
            break;
#endif
        case WM_COMMAND:
            {
            lpNav = (LPNONAV)LP_GET(hDlg);

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDCANCEL:
                    if (IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCFILE_CONTINUE)))
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_INOCFILE_CONTINUE,
                                             GetDlgItem ( hDlg, IDI_INOCFILE_CONTINUE ),
                                             BN_CLICKED, PostMessage );
                        }
                    else if (IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCFILE_STOP)))
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_INOCFILE_STOP,
                                             GetDlgItem ( hDlg, IDI_INOCFILE_STOP ),
                                             BN_CLICKED, PostMessage );
                        }

                    break;

                case IDI_INOCFILE_DELETE:
                    uStatus = NavDeleteFile ( lpNav, hDlg, FALSE );

                    NotifyInocDeleteCompletion ( hDlg, uStatus, lpNav );

                    lpNav->Infected.wStatus = UpdateFileStatus (
                                                    lpNav->Infected.wStatus,
                                                    PROCESS_DELETE,
                                                    uStatus
                                                    );
                    if ( uStatus )
                        {
                        PrepareInocDlgEnd ( lpNav, FALSE );
                        EndDialog(hDlg, CONTINUE);
                        }
                    else
                        {
                        // Just trust me on this...
                        EnableInocFileButton ( hDlg, IDI_INOCFILE_DELETE, FALSE );
                        EnableInocFileButton ( hDlg, IDI_INOCFILE_EXCLUDE, FALSE );
                        EnableInocFileButton ( hDlg, IDI_INOCFILE_INOC, FALSE );
                        }
                    break;

                case IDI_INOCFILE_INOC:
                    if (InocPasswordVerify (hDlg, lpNav) == TRUE)
                        {
                        uStatus = InocThisFile (lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_INOCULATE, uStatus);

                        if (uStatus == TRUE )
                            {
                            PrepareInocDlgEnd ( lpNav, FALSE );
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            {
                            EnableInocFileButton ( hDlg, IDI_INOCFILE_INOC, FALSE );
                            SetFocus (GetDlgItem(hDlg, IDI_INOCFILE_CONTINUE));
                            }
                        }
                    break;

                case IDI_INOCFILE_EXCLUDE:
                    MEMSET (&PwCheck, EOS, sizeof(PwCheck));
                    PwCheck.hWnd         = hDlg;
                    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
                    PwCheck.uID          = PASSWORD_MODIEXCLUSIONS;

                    if (AskPassword(&PwCheck) == PASSWORD_ASK)
                        uStatus = PasswordCheck (&PwCheck, FALSE, FALSE);
                    else
                        uStatus = TRUE;

                    if (uStatus)
                        {
                        uStatus = ExcludeAddFile (lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_EXCLUDE, uStatus);

                        if ( uStatus == TRUE )
                            {
                            PrepareInocDlgEnd ( lpNav, FALSE );
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            {
                            // Just trust me on this...
                            EnableInocFileButton ( hDlg, IDI_INOCFILE_DELETE, FALSE );
                            EnableInocFileButton ( hDlg, IDI_INOCFILE_EXCLUDE, FALSE );
                            EnableInocFileButton ( hDlg, IDI_INOCFILE_INOC, FALSE );
                            }
                        }
                    break;

                case IDI_INOCFILE_CONTINUE:
                    PrepareInocDlgEnd ( lpNav, FALSE );
                    EndDialog(hDlg, CONTINUE);
                    break;

                case IDI_INOCFILE_STOP:
                    PrepareInocDlgEnd ( lpNav, TRUE );
                    EndDialog(hDlg, STOP);
                    break;

                case IDI_INOCFILE_HELP:
                    break;

                case IDHELP:
                    StdHelpFromDialog (
                                    hDlg,
                                    IDH_NAVW_INOCULATE_FILE_DLG_HELP_BTN
                                    );
                    break;
                }
            }
        }

    return ( bReturn );
}

//***************************************************************************
// PrepareInocDlgEnd()
//
// Description:
//      Update lpNav info before we EndDialog()
//
//***************************************************************************
// 09/20/1993 BARRY Function Created.
//***************************************************************************

VOID LOCAL PASCAL PrepareInocDlgEnd ( LPNONAV lpNav, BOOL bStop )
{
    *lpNav->Infected.Notes.lpVirName = EOS;
    AddVirusInfo (lpNav);

    if ( bStop )
        lpNav->bDoScan = FALSE;

    return;
}

//***************************************************************************
// EnableInocFileButton()
//
// Description:
//      Disable the requested button and check if we are out of buttons.
//      If so, enable the continue button.
//
//***************************************************************************
// 09/20/1993 BARRY Function Created.
//***************************************************************************

VOID LOCAL PASCAL EnableInocFileButton ( HWND hDlg, WORD wIDButton, BOOL bEnabled )
{
    EnableWindow (GetDlgItem(hDlg, wIDButton), bEnabled);

    if ( !IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCFILE_STOP)) &&
         !IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCFILE_INOC)) &&
         !IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCFILE_EXCLUDE)) &&
         !IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCFILE_DELETE)))
        {
        EnableWindow(GetDlgItem(hDlg, IDI_INOCFILE_CONTINUE), TRUE);
        }
}


BOOL SYM_EXPORT WINAPI InocFileChanged (LPNONAV lpNav)
{
    auto       DLGPROC           lpfn;
    auto       int               nStatus;


    lpfn = (DLGPROC) MakeProcInstance (
                                (FARPROC)InocFileChangedDlgProc,
                                hInstance
                                );
    SYM_ASSERT ( lpfn );

    nStatus = DialogBoxParam (
                            hInstance,
                            MAKEINTRESOURCE(IDD_INOCFILECHANGED),
                            lpNav->hMommyWindow,
                            lpfn,
                            (LPARAM)lpNav
                            );

    FreeProcInstance((FARPROC) lpfn);

    if (nStatus == -1)
        nStatus = TRUE;

    return ((BOOL)nStatus);
}


BOOL SYM_EXPORT WINAPI InocFileChangedDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    static  BOOL                bVirusInBoot;
    auto    BOOL                bReturn = FALSE;
    auto    REPAIRFILESTRUCT    Repair;
    auto    CHECKSTRUCT         PwCheck;
    auto    LPNONAV             lpNav;
    auto    LPSTR               lpMsg;
    auto    LPSTR               lpString;
    auto    BOOL                bStatus;

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
            break;
        case WM_INITDIALOG:
            NAV_CenterDialog (hDlg);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, IDI_INOCHANGE_MSG);
            NG_SetChildSmallFont (hDlg, IDI_INOCHANGE_INFO);
            NG_SetGroupBoxSunken(hDlg, IDI_INOCHANGE_BOX);
            lpNav = (LPNONAV)lParam;
            LP_SET(hDlg, lpNav);
            lpNav->Infected.wStatus = FILESTATUS_INOCULATEDCHANGED;

            lpMsg = lpString = 0;
            if ((lpString = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE))     != NULL &&
                (lpMsg    = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE * 2)) != NULL)
                {
                if (String_Get (IDS_FILEINOCCHANGED, lpString, SIZE_MESSAGE))
                    SPRINTF (lpMsg, lpString, lpNav->Infected.szFullPath,
                             lpNav->Infected.Notes.lpVirName);

                SetDlgItemText(hDlg, IDI_INOCHANGE_MSG, lpMsg);

                if ( lpNav->Alert.bDispAlertMsg )
                    SetDlgItemText (hDlg, IDI_INOCHANGE_ALERT,
                                    lpNav->Alert.szAlertMsg);
                }

            if (lpMsg)
                MemFreePtr(lpMsg);

            if (lpString)
                MemFreePtr(lpString);

            EnableWindow (GetDlgItem(hDlg, IDI_INOCHANGE_CONTINUE),
                          lpNav->Inoc.bDispCont);
            EnableWindow (GetDlgItem(hDlg, IDI_INOCHANGE_STOP),
                          lpNav->Opt.bAllowScanStop);

            EnableInocChangeButtons(hDlg, lpNav);
            bReturn = TRUE;
            break;
                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
                {
                StdHelpSetContext (HELP_DLG_ASK_ACTION,
                                   HELP_DLG_ASK_ACTION);
                }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // or standard context sensitive help
                                        // ----------------------------------
            OnChangeContextMenu ( hDlg, wMessage, wParam, lParam );
            bReturn = TRUE;
            break;
#endif

        case WM_COMMAND:
            lpNav = (LPNONAV)LP_GET(hDlg);

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDCANCEL:
                    if (IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCHANGE_CONTINUE)))
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_INOCHANGE_CONTINUE,
                                        GetDlgItem ( hDlg, IDI_INOCHANGE_CONTINUE ),
                                        BN_CLICKED, PostMessage );
                        }
                    else if (IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCHANGE_STOP)))
                        {
                        FORWARD_WM_COMMAND ( hDlg, IDI_INOCHANGE_STOP,
                                        GetDlgItem ( hDlg, IDI_INOCHANGE_STOP ),
                                        BN_CLICKED, PostMessage );
                        }

                    break;

                case IDI_INOCHANGE_REPAIR:
                    MEMSET (&Repair, EOS, sizeof(Repair));

                    Repair.bUseInoc         = lpNav->Inoc.bUseInoc;
                    Repair.bCreateBackup    = lpNav->Gen.bBackupRep;
                    Repair.uVirusSigIndex   = lpNav->Infected.uVirusSigIndex;
                    STRCPY (Repair.szFullPath,lpNav->Infected.szFullPath);
                    STRCPY (Repair.szFullOemPath,lpNav->Infected.szFullOemPath);
                    STRCPY (Repair.szBackExt, lpNav->Gen.szBackExt);

                    bStatus = RepairFile (&Repair);

                    NotifyInocRepairCompletion ( hDlg, bStatus, lpNav );

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uCleaned++;

                    lpNav->Infected.wStatus = UpdateFileStatus (
                                    lpNav->Infected.wStatus, PROCESS_REPAIR,
                                    bStatus);

                    if (bStatus == TRUE )
                        {
                        PrepareInocDlgEnd ( lpNav, FALSE );
                        EndDialog(hDlg, CONTINUE);
                        }
                    else
                        EnableInocChangeButtons (hDlg, lpNav);
                    break;

                case IDI_INOCHANGE_DELETE:
                    bStatus = InocDeleteFile (lpNav, hDlg, TRUE);

                    lpNav->Infected.wStatus = UpdateFileStatus (
                                    lpNav->Infected.wStatus, PROCESS_DELETE,
                                    bStatus);

                    if (bStatus == TRUE )
                        {
                        PrepareInocDlgEnd ( lpNav, FALSE );
                        EndDialog(hDlg, CONTINUE);
                        }
                    else
                        EnableInocChangeButtons (hDlg, lpNav);
                    break;

                case IDI_INOCHANGE_INOC:
                    if (InocPasswordVerify (hDlg, lpNav) == TRUE)
                        {
                        bStatus = InocThisFile (lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                                    lpNav->Infected.wStatus, PROCESS_INOCULATE,
                                    bStatus);

                        if (bStatus == TRUE )
                            {
                            PrepareInocDlgEnd ( lpNav, FALSE );
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            EnableInocChangeButtons (hDlg, lpNav);
                        }

                    break;

                case IDI_INOCHANGE_EXCLUDE:
                    MEMSET (&PwCheck, EOS, sizeof(PwCheck));
                    PwCheck.hWnd         = hDlg;
                    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
                    PwCheck.uID          = PASSWORD_MODIEXCLUSIONS;

                    if (AskPassword(&PwCheck) == PASSWORD_ASK)
                        bStatus = PasswordCheck (&PwCheck, FALSE, FALSE);
                    else
                        bStatus = TRUE;

                    if (bStatus)
                        {
                        bStatus = ExcludeAddFile (lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                                    lpNav->Infected.wStatus, PROCESS_EXCLUDE,
                                    bStatus);

                        if (bStatus == TRUE )
                            {
                            PrepareInocDlgEnd ( lpNav, FALSE );
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            EnableInocChangeButtons (hDlg, lpNav);
                        }
                    break;

                case IDI_INOCHANGE_CONTINUE:
                    PrepareInocDlgEnd ( lpNav, FALSE );
                    EndDialog(hDlg, CONTINUE);
                    break;

                case IDI_INOCHANGE_STOP:
                    PrepareInocDlgEnd ( lpNav, FALSE );
                    EndDialog(hDlg, STOP);
                    break;

                case IDHELP:
                    StdHelpFromDialog (
                                    hDlg,
                                    IDH_NAVW_INOC_CHANGED_DLG_HELP_BTN
                                    );
                    break;
                }
            break;
        }

    return ( bReturn );
}

VOID LOCAL EnableInocChangeButtons (HWND hDlg, LPNONAV lpNav)
{
    extern  FILESTATUSSTRUCT FileStatus[];
    auto    BOOL    bDelete = FALSE;
    auto    BOOL    bRepair = FALSE;
    auto    BOOL    bExclude = FALSE;
    auto    BOOL    bInoculate = FALSE;
    auto    WORD    wFileStatus = lpNav->Infected.wStatus;

    bRepair = (lpNav->Inoc.bDispRepair) ?
               FileStatus[wFileStatus].byCanRepair : FALSE;

    bDelete = (lpNav->Inoc.bDispDelete) ?
               FileStatus[wFileStatus].byCanDelete : FALSE;

    bExclude = (lpNav->Inoc.bDispExcl)
               && FileStatus[wFileStatus].byCanExclude
               && !NavTestOemToAnsi (lpNav->Infected.szFullOemPath);

    bInoculate = (lpNav->Inoc.bDispInoc) ?
               FileStatus[wFileStatus].byCanInoc : FALSE;


    EnableWindow (GetDlgItem(hDlg, IDI_INOCHANGE_DELETE),   bDelete);
    EnableWindow (GetDlgItem(hDlg, IDI_INOCHANGE_REPAIR),   bRepair);
    EnableWindow (GetDlgItem(hDlg, IDI_INOCHANGE_EXCLUDE),  bExclude);
    EnableWindow (GetDlgItem(hDlg, IDI_INOCHANGE_INOC),     bInoculate);

    // Guarantee we don't run out of buttons
    if ( !bDelete &&
         !bRepair &&
         !bExclude &&
         !bInoculate &&
         !IsWindowEnabled (GetDlgItem(hDlg, IDI_INOCHANGE_STOP)))
        {
        EnableWindow (GetDlgItem(hDlg, IDI_INOCHANGE_CONTINUE), TRUE );
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

VOID LOCAL OnContextMenu          // Respond to WM_CONTEXTMENU message
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
        IDI_INOCFILE_INOC,          IDH_NAVW_WHATHIS_INOCFILE_INOC,
//&^        IDI_INOCFILE_EXCLUDE,       IDH_NAVW_WHATHIS_INOCFILE_EXCLUDE,
//&^        IDI_INOCFILE_DELETE,        IDH_NAVW_WHATHIS_INOCFILE_DELETE,
        IDI_INOCFILE_CONTINUE,      IDH_NAVW_WHATHIS_INOCFILE_CONTINUE,
        IDI_INOCFILE_STOP,          IDH_NAVW_WHATHIS_INOCFILE_STOP,
        IDI_INOCFILE_INFO,          IDH_NAVW_WHATHIS_INOCFILE_INFO,
        0,                          0
        };

                                        // -----------------------------------
                                        // This array is a set of string id's
                                        // in our resources that are added to
                                        // the 'How To..." menu entry. When one
                                        // of the entries is selected by the
                                        // user the corresponding help is
                                        // displayed.
                                        // -----------------------------------


    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                       hDlg,
                       uMsg,
                       wParam,
                       lParam,
                       dwWhatsThisMap,
                       NULL,
                       NULL,            /* This dlg. does not have any LB Ctrl*/
                       NULL             /* that's why these nulls             */
                       );
}


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

VOID LOCAL OnChangeContextMenu          // Respond to WM_CONTEXTMENU message
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
        IDI_INOCHANGE_REPAIR,       IDH_NAVW_WHATHIS_INOCHANGE_REPAIR,
//&^        IDI_INOCHANGE_DELETE,       IDH_NAVW_WHATHIS_INOCHANGE_DELETE,
        IDI_INOCHANGE_INOC,         IDH_NAVW_WHATHIS_INOCHANGE_REINOC,
//&^        IDI_INOCHANGE_EXCLUDE,      IDH_NAVW_WHATHIS_INOCHANGE_EXCLUDE,
        IDI_INOCHANGE_CONTINUE,     IDH_NAVW_WHATHIS_INOCHANGE_CONTINUE,
        IDI_INOCHANGE_STOP,         IDH_NAVW_WHATHIS_INOCHANGE_STOP,
        0,                          0
        };

                                        // -----------------------------------
                                        // This array is a set of string id's
                                        // in our resources that are added to
                                        // the 'How To..." menu entry. When one
                                        // of the entries is selected by the
                                        // user the corresponding help is
                                        // displayed.
                                        // -----------------------------------


    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                       hDlg,
                       uMsg,
                       wParam,
                       lParam,
                       dwWhatsThisMap,
                       NULL,
                       NULL,            /* This dlg. does not have any LB Ctrl*/
                       NULL             /* that's why these nulls             */
                       );
}

#endif

