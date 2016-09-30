// Copyright 1994-1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NPSUI/VCS/NPSLOOK.CPv   1.2   28 Aug 1998 17:19:48   JBRENNA  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NPSUI/VCS/NPSLOOK.CPv  $
// 
//    Rev 1.2   28 Aug 1998 17:19:48   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.1   15 Jul 1997 15:50:42   DBUCHES
// Added Ctl3d for DEC Alpha
// 
//    Rev 1.0   19 May 1997 00:25:14   KSACKIN
// Initial revision.
// 
//    Rev 1.10   07 May 1997 18:52:28   BILL
// Removed ctl3d for Alpha platform
// 
//    Rev 1.9   22 Feb 1996 13:14:08   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' and 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()' to use Quake's StdHelp merged with the NAV
// version.
// 
//    Rev 1.8   11 Jan 1996 09:49:12   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.7.1.1   30 Nov 1995 21:03:24   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
// 
//    Rev 1.7   03 May 1995 13:19:40   MFALLEN
// Moved stdhelp.h higher up in the include order.
// 
//    Rev 1.6   25 Apr 1995 18:39:02   BRAD
// Added ctl3d.h
// 
//    Rev 1.5   25 Apr 1995 18:15:44   BRAD
// Enabled Ctl3d
// 
//    Rev 1.4   31 Mar 1995 18:29:50   DALLEE
// Added/adjusted help id's for right-click.
//************************************************************************

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

BOOL SYM_EXPORT WINAPI NpsOptsLookDlgProc (
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

                                        // -----------------------------------
                                        // Subclass all of the controls that
                                        // will get the CTL3D look/feel.
                                        // -----------------------------------

    Ctl3dSubclassDlgEx ( hDlg, CTL3D_ALL );

    NG_SetFontsInDialog(hDlg, 0);

    Button_SetCheck ( GetDlgItem ( hDlg, IDC_LOOK_SHOWSTATUSBAR ),
                      lpOptions->bStatusBar );

    Button_SetCheck ( GetDlgItem ( hDlg, IDC_LOOK_SHOWTOOLBAR ),
                      lpOptions->ButtonBar.bShow );

    if ( lpOptions->ButtonBar.uStyle & BBS_DRAWTEXTENABLED &&
         lpOptions->ButtonBar.uStyle & BBS_DRAWBITMAPS )
        {
        Button_SetCheck ( GetDlgItem ( hDlg, IDC_LOOK_TEXTPIC ), TRUE );
        }
    else if ( lpOptions->ButtonBar.uStyle & BBS_DRAWTEXTENABLED )
        {
        Button_SetCheck ( GetDlgItem ( hDlg, IDC_LOOK_TEXTONLY ), TRUE );
        }
    else if ( lpOptions->ButtonBar.uStyle & BBS_DRAWBITMAPS )
        {
        Button_SetCheck ( GetDlgItem ( hDlg, IDC_LOOK_PICONLY ), TRUE );
        }
#ifdef SYM_DEBUG
    else
        {
        SYM_ASSERT ( 0 );
        }
#endif
                                        // -----------------------------------
                                        // Initialize button bar and set to
                                        // current style.
                                        // -----------------------------------
    NpsInitButtonBar (
                    hDlg,
                    GetDlgItem ( hDlg, IDC_LOOK_SAMPLETB ),
                    lpOptions->ButtonBar.uStyle,
                    FALSE
                    );

    FORWARD_WM_COMMAND (
                    hDlg,
                    IDC_LOOK_SHOWTOOLBAR,
                    GetDlgItem ( hDlg, IDC_LOOK_SHOWTOOLBAR ),
                    BN_CLICKED,
                    PostMessage
                    );

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
        case IDC_LOOK_SHOWTOOLBAR:
            {
            BOOL bCheck = Button_GetCheck ( hwndCtl );

            Button_Enable ( GetDlgItem ( hDlg, IDC_LOOK_PICONLY ),  bCheck );
            Button_Enable ( GetDlgItem ( hDlg, IDC_LOOK_TEXTONLY ), bCheck );
            Button_Enable ( GetDlgItem ( hDlg, IDC_LOOK_TEXTPIC ),  bCheck );
            
            UINT    uNumButtons = 6;
            if ( IsDlgButtonChecked ( hDlg, IDC_LOOK_TEXTONLY ) )
                uNumButtons = 4;

            for ( UINT uIndex = 0; uIndex < uNumButtons; uIndex++ )
                ButtonBar_EnableButton ( GetDlgItem ( hDlg, IDC_LOOK_SAMPLETB ),
                                         bCheck, uIndex );
            }
            break;

        case IDC_LOOK_PICONLY:
        case IDC_LOOK_TEXTONLY:
        case IDC_LOOK_TEXTPIC:
            {
            if ( codeNotify == BN_CLICKED )
                {
                LPNPSOPTIONS lpOptions = (LPNPSOPTIONS)GetWindowLong (
                                                            hDlg,
                                                            GWL_USERDATA
                                                            );
                SYM_ASSERT ( lpOptions );
                                        // -----------------------------------
                                        // Set button bar to the currect style
                                        // -----------------------------------

                UINT uButtonStyle = NPS_BBARSTYLE_DEFAULT;

                if ( IsDlgButtonChecked ( hDlg, IDC_LOOK_TEXTONLY ) )
                    {
                    uButtonStyle &= ~BBS_DRAWBITMAPS;
                    }
                if ( IsDlgButtonChecked ( hDlg, IDC_LOOK_PICONLY ) )
                    {
                    uButtonStyle &= ~BBS_DRAWTEXTENABLED;
                    }
                                        // ------------------------------------
                                        // BUGBUG: We can't use NpsResetButtonBar
                                        // because Quake button bar is busted.
                                        // Once the initial size of the buttons
                                        // has been set it can't be changed without
                                        // reseting the darn thing.
                                        // ------------------------------------
                NpsInitButtonBar (
                            hDlg, GetDlgItem ( hDlg, IDC_LOOK_SAMPLETB ),
                            uButtonStyle,
                            FALSE
                            );
                }
            }
            break;

        case IDOK:
            {
            LPNPSOPTIONS    lpOptions= (LPNPSOPTIONS)GetWindowLong (
                                                        hDlg,
                                                        GWL_USERDATA
                                                        );

            SYM_ASSERT ( lpOptions );

            lpOptions->bStatusBar =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_LOOK_SHOWSTATUSBAR ) );

            lpOptions->ButtonBar.bShow =
                Button_GetCheck ( GetDlgItem ( hDlg, IDC_LOOK_SHOWTOOLBAR ) );

            if ( Button_GetCheck ( GetDlgItem ( hDlg, IDC_LOOK_TEXTPIC ) ) )
                {
                lpOptions->ButtonBar.uStyle |= BBS_DRAWBITMAPS;
                lpOptions->ButtonBar.uStyle |= BBS_DRAWTEXTENABLED;
                }
            else if ( Button_GetCheck ( GetDlgItem ( hDlg, IDC_LOOK_TEXTONLY ) ) )
                {
                lpOptions->ButtonBar.uStyle &= ~BBS_DRAWBITMAPS;
                lpOptions->ButtonBar.uStyle |= BBS_DRAWTEXTENABLED;
                }
            else if ( Button_GetCheck ( GetDlgItem ( hDlg, IDC_LOOK_PICONLY ) ) )
                {
                lpOptions->ButtonBar.uStyle |= BBS_DRAWBITMAPS;
                lpOptions->ButtonBar.uStyle &= ~BBS_DRAWTEXTENABLED;
                }

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
        IDC_LOOK_SHOWSTATUSBAR,
        IDH_NAVW_SCHEDULER_WHATHIS_DISPLAY_STATUS_BAR_CHKBX,

        IDC_LOOK_SHOWTOOLBAR,
        IDH_NAVW_SCHEDULER_WHATHIS_DISPLAY_TOOLBAR_CHKBX,

        IDC_LOOK_TEXTONLY,
        IDH_NAVW_SCHEDULER_WHATHIS_TEXT_ONLY_BTN,

        IDC_LOOK_PICONLY,
        IDH_NAVW_SCHEDULER_WHATHIS_PICTURE_ONLY_BTN,

        IDC_LOOK_TEXTPIC,
        IDH_NAVW_SCHEDULER_WHATHIS_TEXT_AND_PICTURE_BTN,

        IDC_LOOK_SAMPLETB,
        IDH_NAVW_SCHEDULER_WHATHIS_SAMPLE_TOOLBAR,

        IDC_LOOK_SAMPLETB_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_SAMPLE_TOOLBAR,

        IDC_LOOK_TBSTYLE_GRPBOX,
        IDH_NAVW_SCHEDULER_WHATHIS_TOOLBAR_STYLE_GRPBX,
        
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
