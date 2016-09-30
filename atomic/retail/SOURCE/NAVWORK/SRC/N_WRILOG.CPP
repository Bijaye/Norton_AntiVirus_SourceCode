// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_wrilog.cpv   1.2   15 Jul 1997 14:42:36   DBUCHES  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_wrilog.cpv  $
// 
//    Rev 1.2   15 Jul 1997 14:42:36   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.1   22 May 1997 16:21:20   BILL
// Alpha changes (removed ctl3d)
// 
//    Rev 1.0   06 Feb 1997 21:02:38   RFULLER
// Initial revision
// 
//    Rev 1.4   11 Jul 1996 08:03:42   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.3   28 Jun 1996 15:11:28   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.2   21 May 1996 15:15:18   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
//
//    Rev 1.1   22 Feb 1996 14:08:04   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:55:32   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:34   BARRY
// Initial revision.
//
//    Rev 1.30   17 Jan 1996 22:40:16   YVANG
// For NAVSCANW: Modified to hide an ID_WRITELOG_BOX control for 3-D frame
// with run-time check under Win95.
//
//    Rev 1.29   16 Jan 1996 11:46:16   YVANG
// Changed NAVWNT to NAVSCANW.  Added NT run-time check for 3-D frame.
//
//    Rev 1.28   30 Nov 1995 22:42:00   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.27   22 Nov 1995 13:26:28   YVANG
// For NAVWNT: Changed ID_WRITE_BOX to ID_WRITELOG_BOX.
//
//    Rev 1.26   21 Nov 1995 16:48:52   YVANG
// For NAVWNT: Added a call, NG_SetGroupBoxSunken(), to make the progress bars
// of the Writing Activity Log diaglog to be displayed in 3-D frame.
//
//
//    Rev 1.25   21 Nov 1995 12:49:14   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//
//    Rev 1.24   16 Jun 1995 16:56:00   KEITH
// Remove a failing WORD_FREE that has no apparent WORD_SET and causes
// an error in the debug kernel.
//
//    Rev 1.23   02 Jun 1995 18:39:04   BARRY
// Removed animation
//
//    Rev 1.22   09 May 1995 10:30:30   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include <windowsx.h>
#include "ngui.h"
#include "stdhelp.h"
	#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "navgraph.hpp"
#include "stack.hpp"

#include "stub.h"
#include "virscan.h"
#include "symcfg.h"
#include "navutil.h"
#include "options.h"
#include "inoc.h"
#include "nonav.h"
#include "navw_hlp.h"
#include "cmbstr.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navwork.h"

extern  HINSTANCE       hInstance;

// ************************************************************************ //
// ****                      Local Prototypes                          **** //
// ************************************************************************ //


BOOL SYM_EXPORT WINAPI WriteLogDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    );

VOID LOCAL DoInitDialog (
    HWND    hDlg,
    LPARAM  lParam
    );

BOOL SYM_EXPORT WINAPI UpdateWriteLogFillbar (
    WORD    wWritten,
    WORD    wToWrite,
    DWORD   dwUser
    );

VOID LOCAL PaintMyIcon (
    HWND    hDlg
    );

// ************************************************************************ //


UINT SYM_EXPORT WINAPI WriteNavLogs(HWND hDlg, LPNONAV lpNav)
{
    auto        DLGPROC         lpfn;
    auto        int             nStatus;

    if ( lpNav->lpInfArr->uNumUsed )
        {
        EnableWindow (hDlg, FALSE);
        lpfn = (DLGPROC) MakeProcInstance ((FARPROC)WriteLogDlgProc, hInstance);

        nStatus = DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_WRITELOG),
                              hDlg, lpfn, (LPARAM)lpNav);
        FreeProcInstance((FARPROC) lpfn);
        EnableWindow (hDlg, TRUE);
        }

    return ((nStatus == -1) ? 0 : nStatus);
}


// ************************************************************************ //

BOOL SYM_EXPORT WINAPI WriteLogDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    static  BOOL        bSaveDoScan;
    static  BOOL        bPaintProcessed = FALSE;
    auto    LPNONAV     lpNav;
    auto    UINT        i;
    auto    BOOL        bReturn = FALSE;
#ifdef SYM_WIN32
    auto    BOOL bApisWereAnsi = AreFileApisANSI ();
#endif

    switch(message)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(message, wParam, lParam);
            break;
        case WM_INITDIALOG:
            NG_SetFontsInDialog (hDlg, 0);
            lpNav = (LPNONAV)lParam;
                                        // ----------------------------------
                                        // The contents of bDoScan is saved
                                        // since it contains the information
                                        // required to log if the user inter-
                                        // rupted the scan or not.
                                        // ----------------------------------
            bSaveDoScan         = lpNav->bDoScan;
            bPaintProcessed     = FALSE;
            DoInitDialog(hDlg, lParam);

#ifdef NAVSCANW
            if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
                NG_SetGroupBoxSunken(hDlg, ID_WRITELOG_BOX);
            else
                ShowWindow(GetDlgItem(hDlg, ID_WRITELOG_BOX), SW_HIDE);
#endif
            bReturn = TRUE;
            break;

        case WM_PAINT:
            if ( bPaintProcessed == FALSE )
                {
                bPaintProcessed = TRUE;
                PostMessage(hDlg, WM_NAVWORKUSER, 0, 0L);
                }

            if (IsIconic(hDlg))
                PaintMyIcon(hDlg);
            break;

        case WM_NAVWORKUSER:
            UpdateWindow(hDlg);
            lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
            if (!lpNav)
                {LP_SET (hDlg, GloballySaved_lpNav);
                 lpNav = (LPNONAV)LP_GET(hDlg);
                 SYM_ASSERT (lpNav);}
#endif

            lpNav->hMommyWindow = hDlg;

#ifdef SYM_WIN32
                                        // ----------------------------------
                                        // File Apis are switch to OEM mode
                                        // back in ScanProc. To make the log
                                        // store properly we're gonna
                                        // temporarily switch 'em back
                                        // ----------------------------------

            bApisWereAnsi = AreFileApisANSI ();
            if (!bApisWereAnsi)
                SetFileApisToANSI ();
#endif
            i = WriteAllFileLogRecords (&lpNav->Act, lpNav->lpInfArr,
                                    UpdateWriteLogFillbar, (DWORD)lpNav);

#ifdef SYM_WIN32
            if (!bApisWereAnsi)
                SetFileApisToOEM ();
#endif
            if ( i )
                {
                NotifyLogWriteError(hDlg, lpNav);
                }

            lpNav->bDoScan = bSaveDoScan;
            EndDialog(hDlg, i);
            break;

        case WM_DESTROY:
            Fillbar_Close(GetDlgItem(hDlg, ID_DOALL_FILLBAR));
            LP_FREE(hDlg);

            // There doesn't appear to be any call to WORD_SET or use of
            // NAV_WORD in this module. We do get a fail on this line though.
            // WORD_FREE(hDlg);
            break;

        case WM_COMMAND:
            lpNav = (LPNONAV)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
            if (!lpNav)
                {LP_SET (hDlg, GloballySaved_lpNav);
                 lpNav = (LPNONAV)LP_GET(hDlg);
                 SYM_ASSERT (lpNav);}
#endif

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDCANCEL:
                    lpNav->bDoScan = FALSE;
                    break;
                }
            break;
        }

   return ( bReturn );
}

VOID LOCAL PaintMyIcon (HWND hDlg)
{
    auto    PAINTSTRUCT     ps;
    auto    HICON           hIcon;

    BeginPaint    (hDlg, &ps);

    hIcon = LoadIcon (hInstance, MAKEINTRESOURCE(ID_ICON_NAV0));

    DrawIcon (ps.hdc, 0, 0, hIcon);

    if (hIcon)
        DestroyIcon (hIcon);

    EndPaint (hDlg, &ps);
}

VOID SYM_EXPORT WINAPI NotifyLogWriteError (HWND hDlg, LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));
    Notify.hParent       = hDlg;
    Notify.uMsgID        = IDS_ERR_WRILOG;
    Notify.uTitleID      = IDS_NAV;
    if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                               // Title is "Internet Scanner"; set at runtime.
    Notify.uTitleID      = IDS_NAV_INTERNET;
        }
    Notify.lpFileName    = lpNav->Act.szLogFile;
    Notify.lpAlertMsg    = (lpNav->Alert.bDispAlertMsg) ?
                            lpNav->Alert.szAlertMsg : NULL;
    Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
                            lpNav->Alert.uSeconds : NULL;
    Notify.hInstance     = hInstance;

    NotifyUser (&Notify);
}

VOID LOCAL DoInitDialog (HWND hDlg, LPARAM lParam)
{
    auto    FILLBARSTYLE    fbStyle;
    auto    LPNONAV         lpNav;
    auto    HWND            hFillbar;


    Ctl3dSubclassDlg(hDlg, CTL3D_ALL);

    NAV_CenterDialog (hDlg);

    lpNav = (LPNONAV)lParam;
    LP_SET (hDlg, lpNav);

    lpNav->bDoScan = TRUE;

    // ********************************************************* //
    // Initialize fillbar with all the currently required values //
    // ********************************************************* //

    hFillbar = GetDlgItem(hDlg, ID_WRITELOG_FILLBAR);

    fbStyle.nStruclen         = sizeof(fbStyle);
    fbStyle.b3D               = TRUE;
    fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
    fbStyle.nRightBottomWidth = FB_USEDEFAULT;
    fbStyle.crColor           = RGB(255, 0, 0);

    Fillbar_SetRange(hFillbar, 0, 100);
    Fillbar_Show(hFillbar, &fbStyle);

    if (IsIconic(GetParent(hDlg)))
        ShowWindow(hDlg, SW_SHOWMINIMIZED);
}

BOOL SYM_EXPORT WINAPI UpdateWriteLogFillbar (
    WORD    wWritten,
    WORD    wToWrite,
    DWORD   dwUser )
{
    auto    LPNONAV lpNav;
    auto    HWND    hDlg = NULL;
    auto    DWORD   dwPercent;
    auto    MSG     msg;

    lpNav = (LPNONAV)dwUser;

    while (NavPeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
        if (!IsDialogMessage (lpNav->hMommyWindow, &msg))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }

    dwPercent = ((DWORD)wWritten * 100) / wToWrite;

    Fillbar_SetLevel (GetDlgItem(lpNav->hMommyWindow, ID_WRITELOG_FILLBAR),
                      (UINT)dwPercent);

    return ( lpNav->bDoScan );
}


