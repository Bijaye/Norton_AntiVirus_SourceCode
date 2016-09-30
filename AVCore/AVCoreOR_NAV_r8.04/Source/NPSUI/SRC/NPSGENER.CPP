// Copyright 1995 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/NPSUI/VCS/NPSGENER.CPv   1.2   28 Aug 1998 17:19:48   JBRENNA  $
//
// Description:
//      This module contains the code for
//
// Contains:
//
// See Also:
//
//***********************************************************************
// $Log:   S:/NPSUI/VCS/NPSGENER.CPv  $
// 
//    Rev 1.2   28 Aug 1998 17:19:48   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.1   15 Jul 1997 15:50:38   DBUCHES
// Added Ctl3d for DEC Alpha
// 
//    Rev 1.0   19 May 1997 00:25:24   KSACKIN
// Initial revision.
// 
//    Rev 1.9   07 May 1997 18:52:22   BILL
// Removed ctl3d for Alpha platform
// 
//    Rev 1.8   01 Oct 1996 16:00:56   JBRENNA
// Remove the IDC_DEF_DATETIME option on SYM_WIN32 platforms (Win95).
// 
//    Rev 1.7   22 Feb 1996 13:10:24   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' and 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()' to use Quake's StdHelp merged with the NAV
// version.
// 
//    Rev 1.6   11 Jan 1996 09:49:08   JWORDEN
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

BOOL SYM_EXPORT WINAPI NpsOptsGeneralDlgProc (
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


    Button_SetCheck ( GetDlgItem ( hDlg, IDC_DEF_HIDE ),
                      lpOptions->bHide );

    Button_SetCheck ( GetDlgItem ( hDlg, IDC_DEF_LOAD ),
                      lpOptions->bLoad );

                                    // For Win95, displaying time in title bar
                                    // does not make sense. So, remove the
                                    // options to do that.
#ifndef SYM_WIN32
    Button_SetCheck ( GetDlgItem ( hDlg, IDC_DEF_DATETIME ),
                      lpOptions->bShowTitleTime );
#endif

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

            lpOptions->bHide =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_DEF_HIDE ) );

            lpOptions->bLoad =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_DEF_LOAD ) );

                                    // For Win95, displaying time in title bar
                                    // does not make sense. So, remove the
                                    // options to do that.
#ifndef SYM_WIN32
            lpOptions->bShowTitleTime =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_DEF_DATETIME ) );
#endif

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
        IDC_DEF_HIDE,
        IDH_NAVW_SCHEDULER_WHATHIS_HIDE_WHEN_ICONIZED_CHKBX,

        IDC_DEF_LOAD,
        IDH_NAVW_SCHEDULER_WHATHIS_LOAD_WITH_WINDOWS_CHKBX,

                                    // For Win95, displaying time in title bar
                                    // does not make sense. So, remove the
                                    // options to do that.
#ifndef SYM_WIN32
        IDC_DEF_DATETIME,
        IDH_NAVW_SCHEDULER_WHATHIS_SHOW_CURRENT_DATE_IN_TITLEBAR,
#endif

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
