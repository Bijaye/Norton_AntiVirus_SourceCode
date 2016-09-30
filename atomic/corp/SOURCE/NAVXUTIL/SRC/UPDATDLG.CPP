// Copyright 1993 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/navxutil/VCS/updatdlg.cpv   1.0   06 Feb 1997 21:05:22   RFULLER  $
//
// Description:
//      Windows "Updating TSR" dialog
//
// Contains:
//      UpdateTsrDlg
//      UpdateTsrDlgProc
//
//***************************************************************************
// $Log:   S:/navxutil/VCS/updatdlg.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:05:22   RFULLER
// Initial revision
// 
//    Rev 1.1   10 Jul 1996 18:46:24   TIVANOV
// fix for the dialog font initilization
//
//    Rev 1.0   30 Jan 1996 15:56:38   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:40   BARRY
// Initial revision.
//
//    Rev 1.2   09 Mar 1995 15:49:42   DALLEE
// Removed "windowsx.h" - we pick it up from platform.h with <> brackets.
// Since this comes right from the SDK, we don't want to pick it up
// as a dependency w/ MKDEPS.
//
//    Rev 1.1   16 Dec 1994 10:59:54   MFALLEN
// Temporarely commented out TSR_
//
//    Rev 1.0   16 Dec 1994 10:37:02   MFALLEN
// Initial revision.
//
//    Rev 1.7   15 Dec 1994 20:14:08   MFALLEN
// From NIRVANA
//
//    Rev 1.6   11 Sep 1993 17:18:14   MFALLEN
// Renamed WM_USER
//
//    Rev 1.5   10 Sep 1993 15:46:02   MFALLEN
// Removed SetTimer()
//
//    Rev 1.4   31 Aug 1993 16:25:04   BARRY
// Changed file name to avoid duplicate UPDATE.C in NAVD
//***************************************************************************

#include "platform.h"
#ifdef SYM_WIN

#include "ngui.h"                       // Norton look / feel
#include "ctl3d.h"
#include "navutil.h"
#include "cmbstr.h"
#include "resexts.h"
#include "stbexts.h"
#include "tsr.h"

// ************************************************************* //
// Structure required to update the TSR when nav options changed //
// ************************************************************* //

typedef struct tagUPDATETSRSTRUCT
{
    UINT        uReserved;
} UPDATETSRSTRUCT, FAR * LPUPDATETSRSTRUCT;

// ************************************************************************ //
// ****                       Local Prototypes                         **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI UpdateTsrDlgProc (HWND, WORD, WPARAM, LPARAM);

// ************************************************************************ //

extern HINSTANCE hInstance;

VOID SYM_EXPORT WINAPI UpdateTsrDlg (HWND hDlg)
{
    auto    UPDATETSRSTRUCT     Tsr;
    auto    NOTIFYSTRUCT        Notify;
    auto    DLGPROC             lpfn;
    auto    int                 nStatus;

    lpfn = (DLGPROC) MakeProcInstance ((FARPROC)UpdateTsrDlgProc,
                                        hInstance);

    nStatus = DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_UPDATETSR),
                             hDlg, lpfn, (LPARAM)(LPUPDATETSRSTRUCT)&Tsr);
    FreeProcInstance((FARPROC) lpfn);

    if (Tsr.uReserved)
        Tsr.uReserved = IDS_ERR_NAVTSR;
    else
        Tsr.uReserved = IDS_RELOAD_TSR;

                                        // ------------------------------------
                                        // Show a message to the user if the
                                        // DOS TSR is present or if the Win TSR
                                        // is present or if dudu occured.
                                        // ------------------------------------
    if (TSRIsLoaded(TSR_ID_NAV) || GetModuleHandle("NAVTSRW") ||
        Tsr.uReserved != IDS_RELOAD_TSR)
        {
        MEMSET (&Notify, EOS, sizeof(Notify));

        Notify.hParent   = hDlg;
        Notify.uMsgID    = Tsr.uReserved;
        Notify.uTitleID  = IDS_NAV;
        Notify.hInstance = hInstance;

        NotifyUser (&Notify);
        }
}


// ************************************************************************ //

BOOL SYM_EXPORT WINAPI UpdateTsrDlgProc (HWND hDlg, WORD message, WPARAM wParam,
                                   LPARAM lParam)
{
    #define WM_TSRUSER  (WM_USER + 180)

    auto    LPUPDATETSRSTRUCT   lpTsr;
    auto    char                szWorkDir[SYM_MAX_PATH+1];

    switch(message)
        {
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx(message, wParam, lParam));

        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_CenterDialog (hDlg, GetParent(hDlg));
            NG_SetChildSmallFont (hDlg, ID_UPDATETSR_TEXT);

            lpTsr = (LPUPDATETSRSTRUCT)lParam;
            LP_SET (hDlg, lpTsr);
            WORD_SET(hDlg, 0);

            Static_SetText(GetDlgItem(hDlg, ID_UPDATETSR_TEXT),
                            String_Ptr(IDS_UPDATINGTSR));

            return (TRUE);

        case WM_PAINT:
            if (!WORD_GET(hDlg))
                {
                WORD_SET(hDlg, TRUE);
                PostMessage(hDlg, WM_TSRUSER, 0, 0L);
                }
            break;

        case WM_TSRUSER:
            lpTsr = (LPUPDATETSRSTRUCT)LP_GET(hDlg);

            GetStartDir (hInstance, szWorkDir, SYM_MAX_PATH+1);
#if 0
            lpTsr->uReserved = UpdateTSR (szWorkDir);
#endif
            EndDialog (hDlg, 0);
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            WORD_FREE(hDlg);
            break;
        }

   return (FALSE);
}

#endif
