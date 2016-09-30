// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/NOTIFY.CPv   1.12   25 Jul 1997 15:17:40   DBUCHES  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/NOTIFY.CPv  $
//
//    Rev 1.12   25 Jul 1997 15:17:40   DBUCHES
// Fixed problems with displaying long file names in NotifyUser dialog.
//
//    Rev 1.11   15 Jul 1997 14:57:24   DBUCHES
// Added Ctl3d support for DEC Alpha
//
//    Rev 1.10   24 Jun 1997 15:57:22   TIVANOV
// fix the multi long msg box aperance
//
//    Rev 1.9   20 Jun 1997 14:16:52   TIVANOV
// fix the nt351 message box
//
//    Rev 1.8   19 Jun 1997 13:49:34   TIVANOV
// more code to support NT service alerting
//
//    Rev 1.7   18 Jun 1997 16:30:14   TIVANOV
// remove the OemtoANSI call
//
//    Rev 1.6   05 Jun 1997 10:12:50   TIVANOV
// remove my last change
//
//    Rev 1.5   04 Jun 1997 11:01:40   TIVANOV
// move LongCustomMsg outsite of the ifdef statement
//
//    Rev 1.4   22 May 1997 17:32:42   DALLEE
// Changed NavOemToAnsi() call in LongCustomMessage() to OemToAnsi().
// We don't really need the obscene OEM/ANSI checking of the former, and
// this causes NAVAPW to pull at least one additional .OBJ.
// (Long story, but NAVAPW compiles locally a few of the OBJ's from here).
//
//    Rev 1.3   22 May 1997 13:59:38   TIVANOV
// fix a bug with the readin buffer termination NULL
//
//    Rev 1.2   21 May 1997 16:43:32   TIVANOV
// LongCustomMessage supports the new large alert message
//
//    Rev 1.1   21 May 1997 13:15:46   BILL
// Removed ctl3d
//
//    Rev 1.0   06 Feb 1997 21:05:04   RFULLER
// Initial revision
//
//    Rev 1.4   08 Nov 1996 15:10:04   MKEATIN
// Call SetForegroundWindow() at the WM_INITDIALOG command to force the
// dialog to the foreground.
//
//    Rev 1.3   14 Sep 1996 19:13:28   DALLEE
// Added header.
//************************************************************************

#include "platform.h"

#ifdef SYM_WIN

#include "xapi.h"
#include "ngui.h"
#include <time.h>
#include "ctl3d.h"
#include "symcfg.h"
#include "navutil.h"
#include "resexts.h"
#include "stbexts.h"

extern HINSTANCE hInstance;
#if defined (WIN32)
    #define IS_WIN32 TRUE
#else
    #define IS_WIN32 FALSE
#endif
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
                                        // These values were taken from
                                        // winuser.h.  Unfortunately, you can
                                        // only have one of the values defined
                                        // at one time, so I have appended the
                                        // version to the end

#define MB_SERVICE_NOTIFICATION_40     0x00200000L
#define MB_SERVICE_NOTIFICATION_35     0x00040000L
static BOOL bShowMsg;

// ************************************************************************ //
// ****                     Local Prototypes                           **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI NotifyDlgProc (HWND, UINT, WPARAM, LPARAM);

// ************************************************************************ //

VOID SYM_EXPORT WINAPI NotifyUser (
    LPNOTIFYSTRUCT  lpNotify
    )
{
    auto          DLGPROC        lpfn;
    auto          int            nStatus;

    lpfn = (DLGPROC) MakeProcInstance (
                                    (FARPROC)NotifyDlgProc,
                                    hInstance
                                    );
    SYM_ASSERT ( lpfn );

    nStatus = DialogBoxParam (
                            hInstance,
                            MAKEINTRESOURCE(IDD_NOTIFY),
                            lpNotify->hParent,
                            lpfn,
                            (LPARAM)lpNotify
                            );

    SYM_ASSERT ( nStatus != -1 );

    FreeProcInstance((FARPROC) lpfn);
}


BOOL SYM_EXPORT WINAPI NotifyDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    #define SIZE_MESSAGE    256
    static  LPNOTIFYSTRUCT  lpNotify;
    auto    LPSTR           lpMsg;
    auto    LPSTR           lpString;
    auto    char            szFullPath [ SYM_MAX_PATH+1 ];

    switch(message)
        {
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx(message, wParam, lParam));
        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetGroupBoxSunken(hDlg, ID_NOTI_BOX);
            NG_SetChildSmallFont (hDlg, ID_NOTI_RAP);
            NG_CenterDialog (hDlg, GetParent(hDlg));
            SetForegroundWindow(hDlg);

            // ***************************************************** //
            // Get our pointer and associate it with this dialog box //
            // ***************************************************** //

            lpNotify = (LPNOTIFYSTRUCT)lParam;

            // *************************************************** //
            // Now prepare the message to be displayed to the user //
            // *************************************************** //

            lpString = lpMsg = NULL;

            if ((lpString = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE))      != NULL &&
                (lpMsg    = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE * 2))  != NULL)
                {
                // **************************************************** //
                // If the user specified a string ID for the dialog box //
                // title, then use it.                                  //
                // **************************************************** //

                if (lpNotify->uTitleID)
                    {
                    LoadString (lpNotify->hInstance, lpNotify->uTitleID,
                                lpMsg, SIZE_MESSAGE * 2);
                    SetWindowText (hDlg, lpMsg);
                    }

                if (LoadString (lpNotify->hInstance, lpNotify->uMsgID,
                                lpString, SIZE_MESSAGE))
                    {
                    RECT rect;
                    HWND hCtrl;
                    //
                    // Get ID_NOTI_RAP - *not* ID_NOTI_BOX...
                    //
                    hCtrl = GetDlgItem( hDlg, ID_NOTI_RAP );
                    GetClientRect( hCtrl, &rect);

                    HDC dc;
                    HFONT hFont, hFontOld;
                    hFont = (HFONT) SendMessage( hCtrl, WM_GETFONT, 0, 0L );

                    if (lpNotify->lpZipFileName)
                        {
                        // Shorten path to fit control
                        dc = GetDC(hCtrl);
                        hFontOld = (HFONT)SelectObject(dc, hFont);
                        STRCPY(szFullPath, lpNotify->lpZipFileName);

                        DrawTextEx( dc,
                                    szFullPath,
                                    -1,
                                    &rect,
                                    DT_PATH_ELLIPSIS
                                  | DT_MODIFYSTRING
                                  | DT_NOPREFIX
                                  | DT_SINGLELINE
                                  | DT_CALCRECT,
                                    NULL);

                        SelectObject(dc, hFontOld );
                        ReleaseDC( hCtrl, dc );

                        SPRINTF (lpMsg, lpString,
                                 (LPSTR)lpNotify->lpFileName,
                                 (LPSTR)szFullPath,
                                 (LPSTR)lpNotify->lpVirusName);
                        }
                    else if (lpNotify->lpFileName || lpNotify->lpVirusName)
                        {
                        // Shorten path to fit control
                        dc = GetDC(hCtrl);
                        hFontOld = (HFONT)SelectObject(dc, hFont);
                        STRCPY(szFullPath, lpNotify->lpFileName);

                        DrawTextEx( dc,
                                    szFullPath,
                                    -1,
                                    &rect,
                                    DT_PATH_ELLIPSIS
                                  | DT_MODIFYSTRING
                                  | DT_NOPREFIX
                                  | DT_SINGLELINE
                                  | DT_CALCRECT,
                                    NULL );

                        SelectObject(dc, hFontOld );
                        ReleaseDC( hCtrl, dc );

                        SPRINTF (lpMsg, lpString,
                                 (LPSTR)szFullPath,
                                 (LPSTR)lpNotify->lpVirusName);
                        }
                    else
                        STRCPY (lpMsg, lpString);
                    }

                SetDlgItemText(hDlg, ID_NOTI_RAP, lpMsg);

                if ( lpNotify->lpAlertMsg )
                    SetDlgItemText (hDlg, ID_NOTI_ALERT, lpNotify->lpAlertMsg);
                }

            if (lpMsg)
                MemFreePtr(lpMsg);

            if (lpString)
                MemFreePtr(lpString);

            if (lpNotify->uRemoveAfter)
                SetTimer (hDlg, 1, lpNotify->uRemoveAfter*1000, NULL);

            return (TRUE);

        case WM_TIMER:
            FORWARD_WM_COMMAND ( hDlg, IDOK, GetDlgItem ( hDlg, IDOK ),
                             BN_CLICKED, SendMessage );
            break;

        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam,lParam))
                {
                case IDOK:
                case IDCANCEL:
                    if ( lpNotify->uRemoveAfter )
                        KillTimer (hDlg, 1);
                    EndDialog(hDlg, 0);
                    break;
                }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;
        }

    return (FALSE);
}

BOOL SYM_EXPORT WINAPI GetShowMsg()
{
    return bShowMsg;
}

void SYM_EXPORT WINAPI SetShowMsg()
{
    bShowMsg = !bShowMsg;
}

VOID SYM_EXPORT WINAPI LongCustomMessage(HWND hDlg)
{
    if (!bShowMsg)
    {
        SetShowMsg();

        auto HFILE hFile;
        TCHAR  filename[SYM_MAX_PATH + 1] = {0};
        const int fsize = 204;

        GetStartDir(hInstance, filename, sizeof(filename));
        STRCAT(filename, _T("\\"));

        if (LoadString (hInstance, IDS_DEF_LONGCUSTOMMESSAGE, filename+STRLEN(filename), sizeof(filename) - STRLEN(filename)))
        {
            if ((hFile = FileOpen(filename, READ_ONLY_FILE)) != HFILE_ERROR)
            {
                //allocate memory
                auto HGLOBAL hmsg     = MemAlloc(GHND, fsize);
                auto HGLOBAL hmsgAnsi = MemAlloc(GHND, fsize*2);

                if ((NULL != hmsg) & (NULL != hmsgAnsi))
                {
                    LPTSTR lpszzMsg     = (LPTSTR) MemLock(hmsg);
                    LPTSTR lpszzMsgAnsi = (LPTSTR) MemLock(hmsgAnsi);

                    //read the file in

                    FileRead(hFile, lpszzMsg, fsize - 1);

                    //OemToAnsi(lpszzMsg, lpszzMsgAnsi);

                    TCHAR title[100] = {0};
                    LoadString(hInstance, IDS_NAV, title, sizeof(title));

                    if (IS_NT)
                    {
                        OSVERSIONINFO rOSVersion;
                        UINT          uiMessageBoxFlags = 0;

                        if ( GetVersionEx(&rOSVersion) && (rOSVersion.dwMajorVersion >= 4) )
                        {
                            uiMessageBoxFlags = MB_SERVICE_NOTIFICATION_40;
                        }
                        else
                        {
                                                                // If something fails, we should use
                                                                // the old value for
                                                                // MB_SERVICE_NOTIFICATION

                            //uiMessageBoxFlags = MB_SERVICE_NOTIFICATION_35;
                        }
                        MessageBox(hDlg, lpszzMsg, title, MB_ICONASTERISK|MB_OK|uiMessageBoxFlags);
                    }
                    else
                        MessageBox(hDlg, lpszzMsg, title, MB_ICONASTERISK|MB_OK);

                    MemUnlock(hmsgAnsi);
                    MemUnlock(hmsg);
                }

                if ( NULL != hmsgAnsi )
                    MemFree  (hmsgAnsi);

                if ( NULL != hmsg )
                    MemFree  (hmsg);

                FileClose(hFile);
            }
        }
    }
}
#endif
