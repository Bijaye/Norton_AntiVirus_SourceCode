// Copyright 1995 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NPSUI/VCS/NPSOPTS.CPv   1.3   30 Mar 1998 12:24:54   mdunn  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NPSUI/VCS/NPSOPTS.CPv  $
// 
//    Rev 1.3   30 Mar 1998 12:24:54   mdunn
// Added autorun missed events option.
// 
//    Rev 1.2   03 Jul 1997 18:15:54   KSACKIN
// Implemented a ListView control for showing the scheduled items.
//
//    Rev 1.1   29 Jun 1997 19:51:56   KSACKIN
// Removed the General options tab from our options.  We are not using them
// when operating under NT.
//
//    Rev 1.0   19 May 1997 00:25:22   KSACKIN
// Initial revision.
//
//    Rev 1.8   22 Feb 1996 13:26:18   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.7   11 Jan 1996 09:49:16   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.6.1.1   30 Nov 1995 21:03:36   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.6   05 Jul 1995 17:41:04   BARRY
// Fixed dialog sizing problem when button bar is hidden. (Gary)
//
//    Rev 1.5   04 Jul 1995 00:38:30   BARRY
// Modified redraw of button bar. (Gary W.)
//
//    Rev 1.4   28 Jun 1995 18:10:20   KEITH
// Add asserts, and fix an invalidate rect on the desktop!
//***************************************************************************

#include "platform.h"
#include "stdhelp.h"
#include "ngui.h"
#include "ctrlpanl.h"
#include "nps.h"
#include "npsres.h"
#include "npsstb.h"
#include "nps_hlp.h"

static struct tagCONTROLPANELSTUFF
{
    UINT        uIconID;
    DLGPROC     lpfnProc;
    UINT        uDialogID;
    UINT        uTitleID;
    UINT        uLabelID;
} Cpl[] =
{
//   {
//   // ********************************************************************* //
//   // **************           General Options                 ************ //
//   // ********************************************************************* //
//
//   IDI_NPS_ICON,
//   (DLGPROC)NpsOptsGeneralDlgProc,
//   IDD_OPTIONS_DEFAULTS,
//   IDS_NPSOPTS_TITLE_DEFAULTS,
//   IDS_NPSOPTS_LABEL_DEFAULTS
//   },

   {
   // ********************************************************************* //
   // **************        Confirmation Options               ************ //
   // ********************************************************************* //

   IDI_NPS_ICON,
   (DLGPROC)NpsOptsConfirmDlgProc,
   IDD_OPTIONS_CONFIRMATION,
   IDS_NPSOPTS_TITLE_CONFIRM,
   IDS_NPSOPTS_LABEL_CONFIRM
   },

   {
   // ********************************************************************* //
   // **************        Appearance Options                 ************ //
   // ********************************************************************* //

   IDI_NPS_ICON,
   (DLGPROC)NpsOptsLookDlgProc,
   IDD_OPTIONS_LOOK,
   IDS_NPSOPTS_TITLE_LOOK,
   IDS_NPSOPTS_LABEL_LOOK
   },

   {
   // ********************************************************************* //
   // **************              Global Dialog                ************ //
   // ********************************************************************* //

   IDI_NPS_ICON,
   (DLGPROC)NpsOptsGlobalDlgProc,
   IDD_OPTIONS_GLOBAL,
   0,
   0
   },

   {  0, NULL, 0, 0, 0  }
};

// ---------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------

BOOL CALLBACK EnumChildProc (           // Move children around in parent
    HWND    hwnd,                       // handle of child window
    LPARAM  lParam                      // application-defined value
    );

// ---------------------------------------------------------------------------

VOID WINAPI NpsOptions (                // Show scheduler options
    HWND    hParent                     // [in] moma
    )
{
    const   WORD            wControls = 2;
    auto    CPL_CB          cb;
    auto    NPSOPTIONS      Options;
    auto    LPNPSOPTIONS    lpOptions;

    lpOptions = (LPNPSOPTIONS)GetWindowLong ( hParent, GWL_USERDATA );
    SYM_ASSERT ( lpOptions );

    if ( lpOptions )
        {
        MEMCPY ( &Options, lpOptions, sizeof(Options) );

        const   UINT    uBytes = sizeof(CPL) * ( wControls + 2 );
        auto    LPCPL   lpCpl  = (LPCPL)MemAllocPtr ( GHND, uBytes );

        if ( lpCpl )
            {
            MEMSET ( lpCpl, EOS, uBytes );
            MEMSET ( &cb, EOS, sizeof(cb) );

            for ( UINT uIndex = 0; Cpl[uIndex].lpfnProc; uIndex++ )
                {
                lpCpl[uIndex].dwSize           =   sizeof(CPL);
                lpCpl[uIndex].dlgprocCPL       =   Cpl[uIndex].lpfnProc;
                lpCpl[uIndex].uResIDTemplate   =   Cpl[uIndex].uDialogID;
                lpCpl[uIndex].uResIDTitle      =   Cpl[uIndex].uTitleID;
                lpCpl[uIndex].uResIDLabel      =   Cpl[uIndex].uLabelID;
                lpCpl[uIndex].hInst            =   ghInst;
                lpCpl[uIndex].bSkip            =   FALSE;
                lpCpl[uIndex].bHideListBox     =   FALSE;
                lpCpl[uIndex].uHelpID          =   0;  // Some kind of context menu info.
                lpCpl[uIndex].lParamInit       =   (LPARAM)&Options;
                lpCpl[uIndex].wTemplateFlag    =   CPLF_NORMAL;
                lpCpl[uIndex].hIcon            =   LoadIcon ( ghInst,
                                                        MAKEINTRESOURCE(Cpl[uIndex].uIconID ));
                }

            cb.dwSize           =   sizeof(cb);
            cb.hwndOwner        =   hParent;
            cb.hInst            =   ghInst;
            cb.uListBoxLabelID  =   IDS_NPSOPTS_CATEGORY;
            cb.wTotalControls   =   wControls;
            cb.lpcpl            =   lpCpl;
            cb.wOptions         =   CPLO_TAB_STYLE | CPLO_CPL_DLG;
            cb.wIndex           =   0;
            cb.lpcszInitFile    =   NULL;

            if ( CPL_Dialog ( &cb, NULL ) )
                {
                auto    BOOL    bUpdateTitleTime = FALSE;
                auto    BOOL    bUpdateStatusBar = FALSE;
                auto    BOOL    bUpdateButtonBar = FALSE;

                if ( lpOptions->bAutorunMissedEvents !=
                     Options.bAutorunMissedEvents )
                    bUpdateTitleTime = TRUE;

                if ( lpOptions->bStatusBar != Options.bStatusBar )
                    bUpdateStatusBar = TRUE;

                if ( MEMCMP ( &lpOptions->ButtonBar, &Options.ButtonBar,
                              sizeof(Options.ButtonBar ) ) )
                    {
                    bUpdateButtonBar = TRUE;
                    }

                                        // -----------------------------------
                                        // Add / remove ourselves from the
                                        // tray depending on the options.
                                        // -----------------------------------

                if ( lpOptions->bHide != Options.bHide )
                    {
                    ShowHideTrayIcon ( hParent, !Options.bHide );
                    }

                                        // -----------------------------------
                                        // Save all user options.
                                        // -----------------------------------

                MEMCPY ( lpOptions, &Options, sizeof(Options) );

                                        // -----------------------------------
                                        // Check if we're to do something
                                        // with the our title.
                                        // -----------------------------------

                if ( bUpdateTitleTime == TRUE )
                    {
                    NpsUpdateCaption ( hParent );
                    }

                                        // -----------------------------------
                                        // Check if we're to do something
                                        // with the button bar.
                                        // -----------------------------------

                if ( bUpdateButtonBar == TRUE )
                    {
                    ShowHideButtonBar (
                                    GetDlgItem ( hParent, IDC_NPS_BUTTONBAR ),
                                    Options.ButtonBar.bShow,
                                    Options.ButtonBar.uStyle,
                                    TRUE
                                    );

                    NpsEventControlsDisable ( hParent );
                    }

                                        // -----------------------------------
                                        // Check if we're to do something
                                        // with the status bar
                                        // -----------------------------------

                if ( bUpdateStatusBar == TRUE )
                    {
                    ShowHideStatusBar (
                                    GetDlgItem ( hParent, IDC_NPS_STATUS ),
                                    Options.bStatusBar
                                    );
                    }
                }
                                        // -----------------------------------
                                        // Clean house.
                                        // -----------------------------------

            for ( uIndex = 0; Cpl[uIndex].lpfnProc; uIndex++ )
                {
                DestroyIcon ( lpCpl[uIndex].hIcon );
                }

            MemFreePtr ( lpCpl );
            }
        }
}


VOID WINAPI ShowHideButtonBar (         // Visualize button bar as required
    HWND    hWndButtonBar,              // [in] button bar to adjust
    BOOL    bShow,                      // [in] TRUE to show buttonbar
    UINT    uStyle,                     // [in] style to show it with
    BOOL    bInit                       // [in] called during init
    )
{
    SYM_ASSERT ( hWndButtonBar );
    const   HWND    hParent  = GetParent ( hWndButtonBar );
    SYM_ASSERT ( hParent );

#ifndef QUAKE_STATUSBAR_FIXED
    static  UINT    uPathetic[] =
        {
        IDC_NPS_LIST,
        IDC_NPS_BUTTONBAR,
        IDC_NPS_STATUS,
        0
        };
#endif
                                        // -----------------------------------
                                        // If parent is iconic do not attempt
                                        // to adjust the size of the buttbar.
                                        // GetWindowRect() returns Guam!
                                        // -----------------------------------

    if ( !IsIconic ( hParent ) )
        {
#ifdef WINDOWS_FUNCTION_FIXED
                                        // -----------------------------------
                                        // Damn it! How hard can it be to
                                        // write this function.
                                        // -----------------------------------

        const   BOOL    bVisible = IsWindowVisible ( hWndButtonBar );
#else
        BOOL bVisible;
        LONG lStyle = GetWindowLong ( hWndButtonBar, GWL_STYLE );

        if ( lStyle & WS_VISIBLE )
            bVisible = TRUE;
        else
            bVisible = FALSE;
#endif

#ifndef SYM_WIN32                       // Take this out of W32 platform
        SetWindowRedraw ( hParent, FALSE );
#endif                                  // SYM_WIN32

        if ( bShow == TRUE )
            {
            auto    RECT    rcBeforeChange;

            if ( bVisible == TRUE )
                {
                GetClientRect ( hWndButtonBar, &rcBeforeChange );
                }
            else
                {
                MEMSET ( &rcBeforeChange, EOS, sizeof(rcBeforeChange) );
                }

            NpsInitButtonBar (
                            GetParent ( hWndButtonBar ),
                            hWndButtonBar,
                            uStyle,
                            bInit
                            );

            auto    RECT    rcAfterChange;

            GetClientRect ( hWndButtonBar, &rcAfterChange );

                                        // -----------------------------------
                                        // If the are than the button bar
                                        // occupies is different from the area
                                        // that it had before, adjust window.
                                        // -----------------------------------

            if ( MEMCMP ( &rcBeforeChange, &rcAfterChange, sizeof(RECT) ) )
                {
                auto    RECT    rcWindow;
                const   int     nDifference = rcAfterChange.bottom -
                                              rcBeforeChange.bottom;

                GetWindowRect ( hParent, &rcWindow );


                MoveWindow (
                        hParent,
                        rcWindow.left, rcWindow.top,
                        rcWindow.right  - rcWindow.left,
                        rcWindow.bottom - rcWindow.top
                        + nDifference,
                        TRUE
                        // FALSE
                        );


#ifdef QUAKE_PROGRESSBAR_FIXED
                EnumChildWindows ( hParent, EnumChildProc, (LPARAM)nDifference );
#else
                for ( UINT uIndex = 0; uPathetic[uIndex]; uIndex++ )
                    {
                    EnumChildProc ( GetDlgItem ( hParent, uPathetic[uIndex]),
                                   (LPARAM)nDifference );
                    }
#endif
                }

            ShowWindow ( hWndButtonBar, SW_SHOW );
            EnableWindow ( hWndButtonBar, TRUE );
            }
        else
            {
                                        // -----------------------------------
                                        // Button bar is hiddent. Adjust window
                                        // accordingly.
                                        // -----------------------------------
            auto    RECT    rcWindow;
            GetClientRect ( hWndButtonBar, &rcWindow );

            const   int     nDifference = 0 - rcWindow.bottom;

            GetWindowRect ( hParent, &rcWindow );

            int nHeight = rcWindow.bottom - rcWindow.top + nDifference;

                                        // Don't truncate the window if the
                                        // bar has already been chopped off:
            if (IsWindowVisible(hWndButtonBar))
                {
                MoveWindow (
                        hParent,
                        rcWindow.left, rcWindow.top,
                        rcWindow.right  - rcWindow.left,
                        nHeight,
                        // FALSE
                        TRUE
                        );
                }



#ifdef QUAKE_PROGRESSBAR_FIXED
            EnumChildWindows ( hParent, EnumChildProc, (LPARAM)nDifference );
#else
#ifdef SYM_WIN32                       // Only do this in W32 platform
            if (IsWindowVisible(hWndButtonBar))
                {
                                        // Don't move the components
                                        // if the window is already hidden:
                                        // (They've already been moved.)
#endif                                  // End SYM_WIN32
            for ( UINT uIndex = 0; uPathetic[uIndex]; uIndex++ )
                {
                EnumChildProc ( GetDlgItem ( hParent, uPathetic[uIndex]),
                               (LPARAM)nDifference );
                }
#ifdef SYM_WIN32                       // Only do this in W32 platform
                }
#endif                                  // SYM_WIN32
#endif
            ShowWindow ( hWndButtonBar, SW_HIDE );
            EnableWindow ( hWndButtonBar, FALSE );
            }

        SYM_ASSERT ( hParent && GetDesktopWindow() != hParent );
#ifndef SYM_WIN32                       // Take this out of W32 platform
        SetWindowRedraw ( hParent, TRUE );
#endif                                  // SYM_WIN32
        InvalidateRect ( hParent, NULL, TRUE );
        }
}


VOID WINAPI ShowHideStatusBar (         // Visualize status bar as required
    HWND    hWndStatusBar,              // [in] status bar to adjust
    BOOL    bShowStatusBar              // [in] TRUE to show status bar
    )
{
    SYM_ASSERT ( hWndStatusBar );
    const   HWND    hParent = GetParent ( hWndStatusBar );
    SYM_ASSERT ( hParent );

#ifdef WINDOWS_FUNCTION_FIXED
                                        // -----------------------------------
                                        // Damn it! How hard can it be to
                                        // write this function.
                                        // -----------------------------------
    const   BOOL    bVisible = IsWindowVisible ( hWndStatusBar );
#else
    BOOL bVisible       = FALSE;

    if ( GetWindowLong ( hWndStatusBar, GWL_STYLE ) & WS_VISIBLE )
        bVisible = TRUE;
#endif

    if ( bVisible != bShowStatusBar )
        {
        RECT    rcClientStatus;
        RECT    rcWindowParent;

        GetClientRect ( hWndStatusBar, &rcClientStatus );
        GetWindowRect ( hParent, &rcWindowParent );

        if ( bShowStatusBar == TRUE )
            {
            ShowWindow ( hWndStatusBar, SW_SHOW );

            MoveWindow ( hParent,
                         rcWindowParent.left, rcWindowParent.top,
                         rcWindowParent.right - rcWindowParent.left,
                         rcWindowParent.bottom - rcWindowParent.top
                         + rcClientStatus.bottom,
                         FALSE );
            }
        else
            {
            ShowWindow ( hWndStatusBar, SW_HIDE );

            MoveWindow ( hParent,
                         rcWindowParent.left, rcWindowParent.top,
                         rcWindowParent.right - rcWindowParent.left,
                         rcWindowParent.bottom - rcWindowParent.top
                         - rcClientStatus.bottom,
                         FALSE );
            }

                                        // This always fails:
        //SYM_ASSERT ( GetParent ( hParent ) );
                                        // This doesn't crash if the first
                                        // parameter is NULL anyway:
        InvalidateRect ( GetParent ( hParent ), NULL, TRUE );
        }
}

BOOL CALLBACK EnumChildProc (           // Move children around in parent
    HWND    hChild,                     // handle of child window
    LPARAM  lParam                      // application-defined value
    )
{
    SYM_ASSERT ( hChild );

    const   HWND    hParent  = GetParent    ( hChild );
    const   UINT    uChildID = GetDlgCtrlID ( hChild );
    const   int     nDiff    = (int)lParam;
    auto    RECT    rcWnd;

    if ( uChildID != IDC_NPS_BUTTONBAR )
        {
        GetClientRect ( hChild, &rcWnd );

        MapWindowPoints(
                    hChild,             // handle of window to be mapped from
                    hParent,            // handle of window to be mapped to
                    (LPPOINT)&rcWnd,	// address of structure array with points to map
                    2                   // number of structures in array
                    );

                                        // -----------------------------------
                                        // HACK
                                        // If the listbox suddenly starts
                                        // growing in size then Microsoft
                                        // fixed the mapping problem.
                                        // -----------------------------------
        if ( uChildID == IDC_NPS_LIST )
            {
            rcWnd.left -= 2;
            rcWnd.top -= 2;
            rcWnd.right += 2;
            rcWnd.bottom += 2;
            }

        MoveWindow ( hChild, rcWnd.left, rcWnd.top + nDiff,
                     rcWnd.right  - rcWnd.left,
                     rcWnd.bottom - rcWnd.top , TRUE );
        }

    return ( TRUE );
}

