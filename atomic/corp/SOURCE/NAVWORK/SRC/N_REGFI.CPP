// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_regfi.cpv   1.1   15 Jul 1997 14:41:56   DBUCHES  $
//
// Description:
//      Routines for Register File and Registered File Changed dialogs.
//
// Contains:
//      RegisterFile()
//      RegFileChanged()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_regfi.cpv  $
// 
//    Rev 1.1   15 Jul 1997 14:41:56   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.0   06 Feb 1997 21:02:50   RFULLER
// Initial revision
// 
//    Rev 1.2   11 Jul 1996 08:03:44   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.1   22 Feb 1996 14:08:06   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:55:32   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:36   BARRY
// Initial revision.
//
//    Rev 1.10   30 Nov 1995 22:42:10   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.9   09 May 1995 10:30:28   KEITH
// Correct some nouns
//
//    Rev 1.8   26 Apr 1995 18:12:24   BRAD
// Added NG_SetGroupBoxSunken()
//
//    Rev 1.7   24 Apr 1995 18:56:52   BRAD
// Enabled Ctl3d
//
//    Rev 1.6   20 Dec 1994 14:46:36   MFALLEN
// Sami did fixes on CERTLIBN
//
//    Rev 1.5   30 Nov 1994 12:33:32   MFALLEN
// 3D in W16
//
//    Rev 1.4   29 Nov 1994 13:37:12   MFALLEN
// Compiles for W16
//
//    Rev 1.3   08 Nov 1994 15:42:12   MFALLEN
// Compiles fine under W32.
//
//
//    Rev 1.1   11 Mar 1994 03:54:32   DALLEE
// Added password checking before registering files.
//
//    Rev 1.0   01 Mar 1994 11:49:48   DALLEE
// Initial revision.
//************************************************************************

#ifdef BANKERS

#include "platform.h"
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
#include "virscan.h"
#include "exclude.h"
#include "inoc.h"
#include "security.h"
#include "nonav.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navw_hlp.h"


//************************************************************************
// EXTERNS
//************************************************************************

extern  FILESTATUSSTRUCT    FileStatus [];
extern  HINSTANCE           hInstance;


// ************************************************************************ //
// ****                       Local Prototypes                         **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI RegFileDlgProc           (HWND, UINT, WPARAM, LPARAM);
BOOL SYM_EXPORT WINAPI RegFileChangedDlgProc    (HWND, UINT, WPARAM, LPARAM);
VOID LOCAL  PASCAL EnableRegChangeButtons   (HWND hDlg, LPNONAV lpNav);
VOID LOCAL  PASCAL EnableRegFileButtons     (HWND hDlg, LPNONAV lpNav);
VOID LOCAL  PASCAL PrepareRegDlgEnd         (LPNONAV lpNav, BOOL bStop );


//************************************************************************
// RegisterFile()
//
// This routine calls up the Register File immediate response prompt dialog.
//
// Parameters:
//      LPNONAV     lpNav               Info on current scan and current file.
//
// Returns:
//      TRUE                            To stop scan.
//      != TRUE                         continue scanning.
//************************************************************************
//  2/28/94 DALLEE, from Inoculate File
//************************************************************************

BOOL SYM_EXPORT WINAPI RegisterFile (LPNONAV lpNav)
{
    extern  HINSTANCE   hInstance;
    auto    DLGPROC     lpfn;
    auto    int         nStatus;


    lpfn = (DLGPROC) MakeProcInstance((FARPROC)RegFileDlgProc, hInstance);

    nStatus = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_REGFILE),
                             lpNav->hMommyWindow, lpfn, (LPARAM)lpNav);

    FreeProcInstance((FARPROC) lpfn);

    if (nStatus == -1)
        {
        nStatus = TRUE;
        }

    return ((BOOL)nStatus);
} // RegisterFile()


//************************************************************************
// RegFileDlgProc()
//
// Procedure for Register Files immediate response dialog.
//
// Parameters:
//
// Returns:
//
//************************************************************************
//  2/28/94 DALLEE, from InocFileDlgProc
//************************************************************************

BOOL SYM_EXPORT WINAPI RegFileDlgProc (HWND hDlg, UINT wMessage, WPARAM wParam,
                                    LPARAM lParam)
{
    auto    CHECKSTRUCT     PwCheck;
    auto    LPNONAV         lpNav;
    auto    LPSTR           lpMsg;
    auto    LPSTR           lpString;
    auto    UINT            uStatus;
    auto    char            szFullPath [SYM_MAX_PATH + 1];

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam));


        case WM_INITDIALOG:
            NAV_CenterDialog (hDlg);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, IDI_REGFILE_MSG);
            NG_SetChildSmallFont (hDlg, IDI_REGFILE_INFO);
            NG_SetGroupBoxSunken(hDlg, IDI_REGFILE_BOX);
            lpNav = (LPNONAV)lParam;
            LP_SET(hDlg, lpNav);

            lpMsg = lpString = NULL;
            if ((lpString = MemAllocPtr(GHND, SIZE_MESSAGE))     != NULL &&
                (lpMsg    = MemAllocPtr(GHND, SIZE_MESSAGE * 2)) != NULL)
                {
                STRCPY (szFullPath, lpNav->Infected.szFullPath);
                MakePathShorter (szFullPath, 5);

                if (String_Get (IDS_FILEISNOTREGISTERED, lpString, SIZE_MESSAGE))
                    {
                    SPRINTF(lpMsg, lpString, (LPSTR)szFullPath,
                            (LPSTR)lpNav->Infected.Notes.lpVirName);
                    }

                SetDlgItemText(hDlg, IDI_REGFILE_MSG, lpMsg);
                }

            if (lpMsg)
                MemFreePtr(lpMsg);

            if (lpString)
                MemFreePtr(lpString);


            if (lpNav->Alert.bDispAlertMsg)
                {
                SetDlgItemText(hDlg, IDI_REGFILE_ALERT,
                               lpNav->Alert.szAlertMsg);
                }

                                        // Set initial enabled/disabled state
                                        // of buttons.
            EnableRegFileButtons(hDlg, lpNav);

            return (TRUE);

                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case GET_WM_ACTIVATE_:
            if (wParam)
                {
                StdHelpSetContext (HELP_DLG_ASK_ACTION,
                                   HELP_DLG_ASK_ACTION);
                }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

        case GET_WM_COMMAND_:
            {
            lpNav = LP_GET(hDlg);

            switch (wParam)
                {
                case IDCANCEL:
                    if (IsWindowEnabled (GetDlgItem(hDlg, IDI_REGFILE_CONTINUE)))
                        {
                        PostMessage(hDlg, GET_WM_COMMAND_, IDI_REGFILE_CONTINUE, 0L);
                        }
                    else if (IsWindowEnabled (GetDlgItem(hDlg, IDI_REGFILE_STOP)))
                        {
                        PostMessage(hDlg, GET_WM_COMMAND_, IDI_REGFILE_STOP, 0L);
                        }
                    break;

                case IDI_REGFILE_REG:
                    if (RegPasswordVerify (hDlg, lpNav) == TRUE)
                        {
                        uStatus = RegisterThisFile(lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_REGISTER, uStatus);

                        if (uStatus == TRUE )
                            {
                            PrepareRegDlgEnd(lpNav, FALSE);
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            {
                            EnableRegFileButtons(hDlg, lpNav);
                            SetFocus(GetDlgItem(hDlg, IDI_REGFILE_CONTINUE));
                            }
                        }
                    break;

                case IDI_REGFILE_EXCLUDE:
                    MEMSET (&PwCheck, 0, sizeof(PwCheck));
                    PwCheck.hWnd         = hDlg;
                    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
                    PwCheck.uID          = PASSWORD_MODIEXCLUSIONS;

                    if (AskPassword(&PwCheck) == PASSWORD_ASK)
                        uStatus = PasswordCheck(&PwCheck, FALSE, FALSE);
                    else
                        uStatus = TRUE;

                    if (uStatus)
                        {
                        uStatus = ExcludeAddFile(lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_EXCLUDE, uStatus);

                        if (uStatus == TRUE)
                            {
                            PrepareRegDlgEnd(lpNav, FALSE);
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            {
                            EnableRegFileButtons(hDlg, lpNav);
                            }
                        }
                    break;

                case IDI_REGFILE_CONTINUE:
                    PrepareRegDlgEnd(lpNav, FALSE);
                    EndDialog(hDlg, CONTINUE);
                    break;

                case IDI_REGFILE_STOP:
                    PrepareRegDlgEnd(lpNav, TRUE);
                    EndDialog(hDlg, STOP);
                    break;
                }
            }
        }

    return (FALSE);
} // RegFileDlgProc()


//***************************************************************************
// PrepareRegDlgEnd()
//
// Description:
//      Update lpNav info before we EndDialog()
//
//***************************************************************************
//  2/28/94 DALLEE, from PrepareInocDlgEnd()
//***************************************************************************

VOID LOCAL PASCAL PrepareRegDlgEnd (LPNONAV lpNav, BOOL bStop)
{
    *lpNav->Infected.Notes.lpVirName = EOS;
    AddVirusInfo (lpNav);

    if (bStop)
        lpNav->bDoScan = FALSE;

    return;
} // PrepareRegDlgEnd()


//************************************************************************
// EnableRegFileButtons()
//
// This routine enables/disables the register file buttons depending
// on the options settings and the status of the current file.
//
// Parameters:
//      HWND    hDlg                    Handle of Register File dialog.
//      LPNONAV lpNav                   Info on options and current file.
//
// Returns:
//      Nothing.
//************************************************************************
//  2/28/94 DALLEE, created.
//************************************************************************

VOID LOCAL PASCAL EnableRegFileButtons (HWND hDlg, LPNONAV lpNav)
{
    auto    BOOL    bRegister;
    auto    BOOL    bExclude;
    auto    BOOL    bStop;
    auto    BOOL    bContinue;

    auto    WORD    wStatus;


    wStatus = lpNav->Infected.wStatus;


    bRegister = lpNav->lpScan->lpNavOptions->monitor.bDispReg &&
                FileStatus[wStatus].byCanRegister;

    bExclude  = lpNav->lpScan->lpNavOptions->monitor.bDispExcl &&
                FileStatus[wStatus].byCanExclude;

    bStop     = lpNav->lpScan->lpNavOptions->monitor.bDispStop &&
                lpNav->Opt.bAllowScanStop;

                                        // Enable continue if no other button
                                        // is set.
    bContinue = lpNav->lpScan->lpNavOptions->monitor.bDispCont ||
                (!bRegister && !bExclude && !bStop);


    EnableWindow(GetDlgItem(hDlg, IDI_REGFILE_REG),     bRegister);
    EnableWindow(GetDlgItem(hDlg, IDI_REGFILE_EXCLUDE), bExclude);
    EnableWindow(GetDlgItem(hDlg, IDI_REGFILE_STOP),    bStop);
    EnableWindow(GetDlgItem(hDlg, IDI_REGFILE_CONTINUE),bContinue);
} // EnableRegFileButtons()


//************************************************************************
// RegFileChanged()
//
// This routine calls up the Registered File Changed immediate response
// prompt dialog.
//
// Parameters:
//      LPNONAV     lpNav               Info on current scan and current file.
//
// Returns:
//      TRUE                            To stop scan.
//      != TRUE                         continue scanning.
//************************************************************************
//  2/28/94 DALLEE, from InocFileChanged()
//************************************************************************

BOOL SYM_EXPORT WINAPI RegFileChanged (LPNONAV lpNav)
{
    auto    DLGPROC     lpfn;
    auto    int         nStatus;

    lpfn = (DLGPROC)MakeProcInstance((FARPROC)RegFileChangedDlgProc, hInstance);

    nStatus = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_REGFILECHANGED),
                             lpNav->hMommyWindow, lpfn, (LPARAM)lpNav);

    FreeProcInstance((FARPROC) lpfn);

    if (nStatus == -1)
        {
        nStatus = TRUE;
        }

    return ((BOOL)nStatus);
} // RegFileChanged()


//************************************************************************
// RegFileChangedDlgProc()
//
// Handles messages to the Registered File Changed immediate response
// prompt dialog.
//
// Parameters:
//
// Returns:
//
//************************************************************************
//  2/28/94 DALLEE, from InocFileChangedDlgProc()
//************************************************************************

BOOL SYM_EXPORT WINAPI RegFileChangedDlgProc (HWND hDlg, UINT wMessage,
                                          WPARAM wParam, LPARAM lParam)
{
    auto    CHECKSTRUCT         PwCheck;
    auto    LPNONAV             lpNav;
    auto    LPSTR               lpMsg;
    auto    LPSTR               lpString;
    auto    UINT                uStatus;

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam));

        case WM_INITDIALOG:
            NAV_CenterDialog(hDlg);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont(hDlg, IDI_REGCHANGE_MSG);
            NG_SetChildSmallFont(hDlg, IDI_REGCHANGE_INFO);
            NG_SetGroupBoxSunken(hDlg, IDI_REGCHANGE_BOX);
            lpNav = (LPNONAV)lParam;
            LP_SET(hDlg, lpNav);

            lpMsg = lpString = 0;
            if ( (lpString = MemAllocPtr(GHND, SIZE_MESSAGE))     != NULL &&
                 (lpMsg    = MemAllocPtr(GHND, SIZE_MESSAGE * 2)) != NULL )
                {
                if (String_Get (IDS_FILEREGCHANGED, lpString, SIZE_MESSAGE))
                    {
                    SPRINTF(lpMsg, lpString, lpNav->Infected.szFullPath,
                            lpNav->Infected.Notes.lpVirName);
                    }

                SetDlgItemText(hDlg, IDI_REGCHANGE_MSG, lpMsg);
                }

            if (lpMsg)
                MemFreePtr(lpMsg);

            if (lpString)
                MemFreePtr(lpString);


            if (lpNav->Alert.bDispAlertMsg)
                {
                SetDlgItemText(hDlg, IDI_REGCHANGE_ALERT,
                               lpNav->Alert.szAlertMsg);
                }

            EnableRegChangeButtons(hDlg, lpNav);
            return (TRUE);

                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case GET_WM_ACTIVATE_:
            if (wParam)
                {
                StdHelpSetContext (HELP_DLG_ASK_ACTION,
                                   HELP_DLG_ASK_ACTION);
                }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

        case GET_WM_COMMAND_:
            lpNav = LP_GET(hDlg);

            switch (wParam)
                {
                case IDCANCEL:
                    if (IsWindowEnabled (GetDlgItem(hDlg, IDI_REGCHANGE_CONTINUE)))
                        {
                        PostMessage(hDlg, GET_WM_COMMAND_, IDI_REGCHANGE_CONTINUE, 0L);
                        }
                    else if (IsWindowEnabled (GetDlgItem(hDlg, IDI_REGCHANGE_STOP)))
                        {
                        PostMessage(hDlg, GET_WM_COMMAND_, IDI_REGCHANGE_STOP, 0L);
                        }

                    break;

                case IDI_REGCHANGE_REG:
                    if (RegPasswordVerify (hDlg, lpNav) == TRUE)
                        {
                        uStatus = RegisterThisFile(lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_REGISTER, uStatus);

                        if (uStatus == TRUE)
                            {
                            PrepareRegDlgEnd(lpNav, FALSE);
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            {
                            EnableRegChangeButtons(hDlg, lpNav);
                            SetFocus(GetDlgItem(hDlg, IDI_REGCHANGE_CONTINUE));
                            }
                        }
                    break;

                case IDI_REGCHANGE_EXCLUDE:
                    MEMSET (&PwCheck, 0, sizeof(PwCheck));
                    PwCheck.hWnd         = hDlg;
                    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
                    PwCheck.uID          = PASSWORD_MODIEXCLUSIONS;

                    if (AskPassword(&PwCheck) == PASSWORD_ASK)
                        uStatus = PasswordCheck(&PwCheck, FALSE, FALSE);
                    else
                        uStatus = TRUE;

                    if (uStatus)
                        {
                        uStatus = ExcludeAddFile(lpNav, hDlg, TRUE);

                        lpNav->Infected.wStatus = UpdateFileStatus (
                            lpNav->Infected.wStatus, PROCESS_EXCLUDE, uStatus);

                        if (uStatus == TRUE)
                            {
                            PrepareRegDlgEnd(lpNav, FALSE);
                            EndDialog(hDlg, CONTINUE);
                            }
                        else
                            {
                            EnableRegChangeButtons(hDlg, lpNav);
                            }
                        }
                    break;

                case IDI_REGCHANGE_CONTINUE:
                    PrepareRegDlgEnd(lpNav, FALSE);
                    EndDialog(hDlg, CONTINUE);
                    break;

                case IDI_REGCHANGE_STOP:
                    PrepareRegDlgEnd(lpNav, TRUE);
                    EndDialog(hDlg, STOP);
                    break;
                }
            break;
        }

    return (FALSE);
} // RegFileChangeDlgProc()


//************************************************************************
// EnableRegChangeButtons()
//
// This routine enables/disables the register file buttons depending
// on the options settings and the status of the current file.
//
// Parameters:
//      HWND    hDlg                    Handle of Register File dialog.
//      LPNONAV lpNav                   Info on options and current file.
//
// Returns:
//      Nothing.
//************************************************************************
//  2/28/94 DALLEE, created.
//************************************************************************

VOID LOCAL PASCAL EnableRegChangeButtons (HWND hDlg, LPNONAV lpNav)
{
    auto    BOOL    bRegister;
    auto    BOOL    bExclude;
    auto    BOOL    bStop;
    auto    BOOL    bContinue;

    auto    WORD    wStatus;


    wStatus = lpNav->Infected.wStatus;


    bRegister = lpNav->lpScan->lpNavOptions->monitor.bDispReg &&
                FileStatus[wStatus].byCanRegister;

    bExclude  = lpNav->lpScan->lpNavOptions->monitor.bDispExcl &&
                FileStatus[wStatus].byCanExclude;

    bStop     = lpNav->lpScan->lpNavOptions->monitor.bDispStop &&
                lpNav->Opt.bAllowScanStop;

                                        // Enable continue if no other button
                                        // is set.
    bContinue = lpNav->lpScan->lpNavOptions->monitor.bDispCont ||
                (!bRegister && !bExclude && !bStop);


    EnableWindow(GetDlgItem(hDlg, IDI_REGCHANGE_REG),     bRegister);
    EnableWindow(GetDlgItem(hDlg, IDI_REGCHANGE_EXCLUDE), bExclude);
    EnableWindow(GetDlgItem(hDlg, IDI_REGCHANGE_STOP),    bStop);
    EnableWindow(GetDlgItem(hDlg, IDI_REGCHANGE_CONTINUE),bContinue);
} // EnableRegChangeButtons()


//************************************************************************
// RegPasswordVerify()
//
// This routine asks for the password before allowing the user to register
// files.
//
// Parameters:
//      HWND    hDlg                    Parent for our password dialog.
//      LPNONAV lpNav                   Has options info.
//
// Returns:
//      TRUE                            Correct password, or not required.
//      FALSE                           Incorrect password.
//************************************************************************
// 04/11/94 DALLEE, from InocPasswordVerify()
//************************************************************************

BOOL WINAPI RegPasswordVerify (HWND hDlg, LPNONAV lpNav)
{
    auto    BOOL        bStatus = TRUE;
    auto    CHECKSTRUCT PwCheck;

    MEMSET (&PwCheck, EOS, sizeof(PwCheck));
    PwCheck.hWnd         = hDlg;
    PwCheck.lpNavOptions = lpNav->lpScan->lpNavOptions;
    PwCheck.uID          = PASSWORD_REGFILES;

    if (AskPassword(&PwCheck) == PASSWORD_ASK)
        {
        bStatus = PasswordCheck (&PwCheck, FALSE, FALSE);
        }

    return (bStatus);
} // RegPasswordVerify()

#endif  // BANKERS
