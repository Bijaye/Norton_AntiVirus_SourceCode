/**********************************************************************
** Copyright 1993 Symantec, Peter Norton Group
**
** File: symaadj.c
**
** cc: "
**
** See Also:
** Future Enhancements:
***********************************************************************
** $Header:   S:/SYMALOG/VCS/SYMAADJ.C_v   1.10   12 Jan 1996 12:12:06   JWORDEN  $
** 05/14/93 WRL is William R. Lee II (BLEE)
**********************************************************************/
#if 0
//&? Don't currently support this feature.  Remove it for now.
#include "symalogi.h"                   /* Project specific dcls     */
                                        /* GetOpenFileName()         */
#include <windowsx.h>                   /* Button_SetCheck()         */
                                        /* Button_GetCheck()         */
#include "ctl3d.h"                      /* Ctl3dCtlColorEx()         */
                                        /* Ctl3dSubclassDlg()        */
#include "nstdhelp.h"                   /* IDHELP StdHelpSetContext()*/
#include "file.h"                       /* READ_WRITE_FILE           */
#include "ngui.h"                       /* NG_CenterDialog()         */
#include "xapi.h"                       /* ConvertLongToString()     */
#include "logio.h"                      // LogOpen() LogSet...() LogClose()

/**********************************************************************
** - Local Prototypes
**********************************************************************/
UINT SYM_EXPORT WINAPI AdjustDlgProc(
    HWND hDlg,                          /* Me!                       */
    UINT Msg,                           /* What!                     */
    WPARAM Param1,                      /*    and                    */
    LPARAM Param2);                     /*    AND                    */

/** ALogAdjust() ******************************************************
**
** This function prints the specified file.  If the pointer to the file
** name is NULL or points to a zero length string, no file is printed;
** the return value indicates the user's choice.  If the pointer is not
** NULL and the user selects "print to file," then the filename is
** returned in the buffer.  This buffer should be at least
** MAX_NET_PATH_SIZE+1 bytes in length.
**
***********************************************************************
** 05/14/93 WRL Function Created
**********************************************************************/
int SYM_EXPORT WINAPI ALogAdjust(           /* Dlg to adjust log file    */
    HWND hWnd,                          /* In: Calling window handle */
    LPSTR szFile)                       /* In: Log file to adjust    */
{
    int      nStatus;                   /* Return value              */
    HFILE    hFile;

    if (nStatus = LogOpen(szFile, READ_WRITE_FILE, &hFile, FALSE)) {
        nStatus = DialogBoxParam(hDLLInst, MAKEINTRESOURCE(IDD_ADJUST_LOG),
                                 hWnd, AdjustDlgProc,MAKELONG(hFile,0));
        LogClose(hFile);
    }
    return nStatus;
} /* ALogAdjust() */

/** AdjustDlgProc() ***************************************************
**
** Future Enhancements:
***********************************************************************
** 05/14/93 WRL Function Created
**********************************************************************/
UINT SYM_EXPORT WINAPI AdjustDlgProc(
    HWND hDlg,                          /* Me!                       */
    UINT Msg,                           /* What!                     */
    WPARAM Param1,                      /*    and                    */
    LPARAM Param2)                      /*    AND                    */
{
    switch (Msg) {
    case GET_WM_CTLCOLOR_:
        return ((UINT)Ctl3dCtlColorEx(Msg, Param1, Param2));

    case WM_INITDIALOG: {
        HFILE hFile;
#       define DWORD_LEN 20
        char szValue[DWORD_LEN+1];
        DWORD Value;

        hFile = (HFILE)LOWORD(Param2);
        SetWindowLong(hDlg, DWL_USER, Param2);

        Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
        NG_CenterDialog(hDlg,GetParent(hDlg));
        NG_SetFontsInDialog(hDlg, 0);

                                        /* Init size edit field      */
        Value = LogGetMaxSize(hFile);
        ConvertLongToString(Value, szValue);
        SetDlgItemText(hDlg, IDI_KB, szValue);

                                        /* Init max age edit field   */
        Value = LogGetMaxEntryAge(hFile);
        ConvertLongToString(Value, szValue);
        SetDlgItemText(hDlg, IDI_DAYS, szValue);

                                        /* Init max entries edit field */
        Value = LogGetMaxEntries(hFile);
        ConvertLongToString(Value, szValue);
        SetDlgItemText(hDlg, IDI_ENTRIES, szValue);

                                        /* Enable first radio button */
        Button_SetCheck(GetDlgItem(hDlg, IDI_CLEAR_ALL), TRUE);
    }  return (TRUE);

                                        /* WM_ACTIVATE: Set help context */
                                        /* for F1 help support.      */
    case GET_WM_ACTIVATE_:
        if (Param1)
            StdHelpSetContext(ALOG_HELP_DLG_ADJUST, ALOG_HELP_DLG_ADJUST);
        break;

    case GET_WM_COMMAND_:
        switch (Param1) {
        case IDI_PR_FILE:
            break;

        case IDOK: {
            HFILE hFile = LOWORD(GetWindowLong(hDlg, DWL_USER));

            if (Button_GetCheck(GetDlgItem(hDlg, IDI_CLEAR_SOME))) {
                char szValue[DWORD_LEN+1];
                HWND hCtl;
                if (Button_GetCheck(hCtl=GetDlgItem(hDlg, IDI_KEEP_SIZE))) {
                    DWORD OrigValue, Value;
                    GetWindowText(hCtl, szValue, DWORD_LEN+1);
                    ConvertStringToLong(&Value, szValue);
                    OrigValue = LogGetMaxSize(hFile);
                    LogSetMaxSize(hFile, Value);
                    LogSetMaxSize(hFile, OrigValue);
                }
                if (Button_GetCheck(hCtl=GetDlgItem(hDlg, IDI_KEEP_DAYS))) {
                    WORD OrigValue, Value;
                    GetWindowText(hCtl, szValue, DWORD_LEN+1);
                    ConvertStringToWord(&Value, szValue);
                    OrigValue = LogGetMaxEntryAge(hFile);
                    LogSetMaxEntryAge(hFile, Value);
                    LogSetMaxEntryAge(hFile, OrigValue);
                }
                if (Button_GetCheck(hCtl=GetDlgItem(hDlg, IDI_KEEP_ENTRIES))) {
                    WORD OrigValue, Value;
                    GetWindowText(hCtl, szValue, DWORD_LEN+1);
                    ConvertStringToWord(&Value, szValue);
                    OrigValue = LogGetMaxEntries(hFile);
                    LogSetMaxEntries(hFile, Value);
                    LogSetMaxEntries(hFile, OrigValue);
                }
            } /* then */
            else if (Button_GetCheck(GetDlgItem(hDlg, IDI_CLEAR_ALL))) {
            }

            EndDialog(hDlg, IDOK);
        }   break;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;

        case IDHELP:
            StdHelpFromDialog(hDlg, ALOG_HELP_DLG_ADJUST);
            break;
        } /* switch (Param1) */
        break;
    } /* switch (Msg) */
    return 0;
} // AdjustDlgProc()
#endif

