// Copyright 1995 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/NPSUI/VCS/NPSCFG.CPv   1.4   28 Aug 1998 17:19:46   JBRENNA  $
//
// Description:
//      This module contains the code for
//
// Contains:
//
// See Also:
//
//***********************************************************************
// $Log:   S:/NPSUI/VCS/NPSCFG.CPv  $
// 
//    Rev 1.4   28 Aug 1998 17:19:46   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.3   30 Mar 1998 12:21:38   mdunn
// Added autorun missed events option to Confirmation page.
// 
//    Rev 1.2   15 Jul 1997 15:50:34   DBUCHES
// Added Ctl3d for DEC Alpha
// 
//    Rev 1.1   29 Jun 1997 19:51:00   KSACKIN
// Removed context help and some references to the checkbox control for
// confirmation upon exit.  This checkbox is no longer required on NT.
//
//    Rev 1.0   19 May 1997 00:25:04   KSACKIN
// Initial revision.
//
//    Rev 1.18   07 May 1997 18:52:04   BILL
// Removed ctl3d for Alpha platform
//
//    Rev 1.17   22 Feb 1996 13:04:00   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' and 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()' to use Quake's StdHelp merged with the NAV
// version.
//
//
//    Rev 1.16   11 Jan 1996 09:49:00   JWORDEN
// Port changes from Quake 8
//
//***********************************************************************
#include "platform.h"
#include "stdhelp.h"
#include    <ctl3d.h>
#include "ngui.h"
#include "ctrlpanl.h"
#include "nps.h"
#include "npsres.h"
#include "npsstb.h"
#include "nps_hlp.h"

// ---------------------------------------------------------------------------
//                          LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------

BOOL LOCAL OnInitDialog (               // Handle the WM_INITDIALOG
    HWND    hDlg,                       // [in] from this dialog
    HWND    hwndFocus,                  // [in] about to get focus
    LPARAM  lParam                      // [in] user defined
    );

void LOCAL OnCommand (                  // Handle the WM_COMMAND
    HWND    hDlg,                       // [in] from this dialog
    int     nID,                        // [in] control ID
    HWND    hwndCtl,                    // [in] control hWnd
    UINT    codeNotify                  // [in] notification code
    );

VOID LOCAL OnContextHelp (              // Do help stuff
    HWND    hDlg,                       // [in] from this dialog
    UINT    uMessage,                   // [in] help message to deal with
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
    );

// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI NpsOptsConfirmDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    BOOL    bReturn = FALSE;

    switch ( uMessage )
        {
        case WM_INITDIALOG:
            bReturn = HANDLE_WM_INITDIALOG ( hDlg, wParam, lParam,
                                             OnInitDialog );
            break;

        case WM_COMMAND:
            HANDLE_WM_COMMAND ( hDlg, wParam, lParam, OnCommand );
            break;

                                        // WM_SYSCOLORCHANGE:  For 3D
                                        // effect.
        case WM_SYSCOLORCHANGE:
            Ctl3dColorChange();
            break;
                                        // WM_CTLCOLOR:  For 3D effect.
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx (uMessage, wParam, lParam);
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                     					// or context sensitive help.
                                        // ----------------------------------
            OnContextHelp ( hDlg, uMessage, wParam, lParam );
            break;
#endif
        }

    return ( bReturn );
}

BOOL LOCAL OnInitDialog (               // Handle the WM_INITDIALOG
    HWND    hDlg,                       // [in] from this dialog
    HWND    hwndFocus,                  // [in] about to get focus
    LPARAM  lParam                      // [in] user defined
    )
{
    const   LPNPSOPTIONS    lpOptions= (LPNPSOPTIONS)lParam;

    SYM_ASSERT ( lpOptions );

    SetWindowLong ( hDlg, GWL_USERDATA, (LONG)lpOptions );

                                        // Subclass all of the controls that
                                        // will get the CTL3D look/feel.
    Ctl3dSubclassDlgEx ( hDlg, CTL3D_ALL );
    NG_SetFontsInDialog(hDlg, 0);

    Button_SetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_DELETE),
                      lpOptions->Confirm.bOnDelete );
    Button_SetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_SAVE),
                      lpOptions->Confirm.bOnSave );
    Button_SetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_EDIT),
                      lpOptions->Confirm.bOnEdit );
    Button_SetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_AUTORUN_MISSED),
                      lpOptions->bAutorunMissedEvents );

    return ( TRUE );
}

void LOCAL OnCommand (                  // Handle the WM_COMMAND
    HWND    hDlg,                       // [in] from this dialog
    int     nID,                        // [in] control ID
    HWND    hwndCtl,                    // [in] control hWnd
    UINT    codeNotify                  // [in] notification code
    )
{
    switch ( nID )
        {
        case IDOK:
            {
            LPNPSOPTIONS lpOptions = (LPNPSOPTIONS)GetWindowLong (
                                                        hDlg,
                                                        GWL_USERDATA
                                                        );
            SYM_ASSERT ( lpOptions );

            lpOptions->Confirm.bOnDelete =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_DELETE) );

            lpOptions->Confirm.bOnSave =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_SAVE) );

            lpOptions->Confirm.bOnEdit =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_EDIT) );

            lpOptions->bAutorunMissedEvents =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_CONFIRM_AUTORUN_MISSED ));

            CPL_EndDialog ( hDlg, TRUE );
            }
            break;

        case IDCANCEL:
            CPL_EndDialog ( hDlg, FALSE );
            break;
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

VOID LOCAL OnContextHelp (              // Do help stuff
    HWND    hDlg,                       // [in] from this dialog
    UINT    uMessage,                   // [in] help message to deal with
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
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
        IDC_CONFIRM_DELETE,
        IDH_NAVW_SCHEDULER_DELETE_SCHEDULED_EVENT_CHKBX,


        IDC_CONFIRM_SAVE,
        IDH_NAVW_SCHEDULER_SAVE_A_SCHEDULED_EVENT,

        IDC_CONFIRM_EDIT,
        IDH_NAVW_SCHEDULER_EDIT_A_SCHEDULED_EVENT,

        0,
        0
        };



                                        // -----------------------------------
                                        // Invoke cmn Nav context sensitive help
                                        // -----------------------------------
    StdHelpExOnContextHelp (
                        hDlg,
                        uMessage,
                        wParam,
                        lParam,
                        dwWhatsThisMap,
                        NULL, 
                        NULL,           //  This dlg. doesn't have any  ...
                        NULL            //  ... List Box controls - so NULL here
                        );
}

#endif
