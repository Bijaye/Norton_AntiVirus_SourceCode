// Copyright 1995 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/NPS/VCS/npsglobl.cpv   1.6   18 Sep 1996 10:16:36   JBRENNA  $
//
// Description:
//      This module contains the code for
//
// Contains:
//
// See Also:
//
//***********************************************************************
// $Log:   S:/NPS/VCS/npsglobl.cpv  $
// 
//    Rev 1.6   18 Sep 1996 10:16:36   JBRENNA
// Add NG_SetFontsInDialog() call when processing WM_INITDIALOG messages.
// This ensures that a DBCS font is used when necessary.
// 
//    Rev 1.5   22 Feb 1996 13:26:20   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
// 
//    Rev 1.4   11 Jan 1996 09:49:10   JWORDEN
// Port changes from Quake 8
//
//***********************************************************************
#include "platform.h"
#include "stdhelp.h"
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

VOID LOCAL OnContextMenu (              // Respond to WM_CONTEXTMENU message
    HWND    hDlg,                       // [in] dialog that sent the message.
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

VOID LOCAL OnHelp (
    HWND    hDlg
    );

VOID LOCAL OnQuestionMark (
    HWND        hDlg,
    LPHELPINFO  lpHelpInfo
    );


// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI NpsOptsGlobalDlgProc (
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

#ifdef SYM_WIN32
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // ----------------------------------
            OnContextMenu ( hDlg, wParam, lParam );
            break;

        case WM_HELP:
                                        // ----------------------------------
                                        // Bring up right click help menu when
                                        // question mark is pressed.
                                        // ----------------------------------
            OnQuestionMark ( hDlg, (LPHELPINFO) lParam );
            bReturn = TRUE;
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
                                        // Setup the Fonts. Finds the correct
                                        // DBCS font if DBCSInstalled().
    NG_SetFontsInDialog (hDlg, 0);

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
                                        // IDOK:  Terminate and save settings if
                                        // required.
        case IDOK:
        case IDCANCEL:
            if ( codeNotify == BN_CLICKED )
                CPL_EndDialog( CPL_GetCurDlg(hDlg), nID );
            break;

        case IDC_GLOBAL_HELP:
            if ( codeNotify == BN_CLICKED )
                OnHelp ( CPL_GetCurDlg(hDlg) );
            break;
        }
}


#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
            HWND    hDlg,
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

VOID LOCAL OnContextMenu (              // Respond to WM_CONTEXTMENU message
    HWND    hDlg,                       // [in] dialog that sent the message.
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    )
{
}

VOID LOCAL OnQuestionMark (
    HWND        hDlg,
    LPHELPINFO  lpHelpInfo
    )
{
    HWND hwndTarget      = (HWND)lpHelpInfo->hItemHandle;
    HWND hwndActiveSheet = CPL_GetCurDlg(hDlg);

    if ( IsWindow(hwndTarget) )
        {
        lpHelpInfo->hItemHandle = (HANDLE) hwndActiveSheet;
        SendMessage ( hwndActiveSheet, WM_HELP, (WPARAM)0, (LPARAM)lpHelpInfo );
        }
}


#endif

VOID LOCAL OnHelp (
    HWND    hDlg
    )
{
    SYM_ASSERT ( hDlg );

    static struct tagGLOBALHELP
        {
        LPSTR   lpDlgID;
        UINT    uHelpID;
        } GlobalHelp[] =
            {
                {
                STR_WHOAMI_LOOK,
                IDH_NAVW_SCHEDULER_APPEARANCE_SETTINGS_HLP_BTN
                },

                {
                STR_WHOAMI_CONFIRMATION,
                IDH_NAVW_SCHEDULER_CONFIRMATION_SETTINGS_HLP_BTN
                },

                {
                STR_WHOAMI_DEFAULTS,
                IDH_NAVW_SCHEDULER_GENERAL_SETTINGS_HLP_BTN
                },

                {
                NULL,
                0
                }
            };

    auto    char    szWhoAmI [ SYM_MAX_PATH ] = { 0 };

    Static_GetText ( GetDlgItem ( hDlg, IDC_WHOAMI ),
                     szWhoAmI, sizeof(szWhoAmI) );

    SYM_ASSERT ( *szWhoAmI );

    for ( UINT uIndex = 0; GlobalHelp[uIndex].lpDlgID; uIndex ++ )
        {
        if ( !STRCMP ( GlobalHelp[uIndex].lpDlgID, szWhoAmI ) )
            {
            StdHelpFromDialog ( hDlg, GlobalHelp[uIndex].uHelpID );
            break;
            }
        }
}



