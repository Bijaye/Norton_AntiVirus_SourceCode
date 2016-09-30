/**********************************************************************
** Copyright 1993 Symantec, Peter Norton Group
**
** File: symaprt.c
**
** cc: "
**
** See Also:
** Future Enhancements:
***********************************************************************
** $Header:   S:/SYMALOG/VCS/SYMAPRT.C_v   1.36   12 Jan 1996 12:12:46   JWORDEN  $
** 05/09/93 WRL is William R. Lee II (BLEE)
**            - Moved to Quake from NIRVANA:NAVUTIL print.c
**********************************************************************/
#include "symalogi.h"                   /* Project specific dcls     */
#include <commdlg.h>                    /* OFN_*                     */
                                        /* GetOpenFileName()         */
#include <windowsx.h>                   /* Button_SetCheck()         */
#include <windows.h>                    /* Button_SetCheck()         */
                                        /* Button_GetCheck()         */
#include "ctl3d.h"                      /* Ctl3dCtlColorEx()         */
                                        /* Ctl3dSubclassDlg()        */
#include "xapi.h"                       /* NameAppendFile()          */
#include "file.h"                       /* FileMove()                */
#include "nstdhelp.h"                   /* IDHELP StdHelpSetContext()*/
#include "nmail.h"                      /* NMailIsMailAvailable()    */
                                        /* StdHelpFromDialog()       */
#include "ngui.h"                       /* Tunnel_GetText() TN_TUNNEL*/
                                        /* NG_CenterDialog()         */
#include "npt.h"                        /* NptPrintAsciiFile()       */
                                        /* NPT_DRAFTMODE_OFF         */
#include "hstring.h"
#include "symarc.h"

char SZ_PRINT_BROWSE_HOOK[] = "PRINT_BROWSE_HOOK";
UINT uHelpPrintBrowseMsg = 0;

/**********************************************************************
** - Local Prototypes
**********************************************************************/

BOOL LOCAL DiskIsReadOnly (LPSTR lpFilename);

UINT SYM_EXPORT WINAPI PrintDlgProc(
    HWND hDlg,                          /* Me!                       */
    UINT Msg,                           /* What!                     */
    WPARAM Param1,                      /*    and                    */
    LPARAM Param2);                     /*    AND                    */
VOID WINAPI BrowsePrintFile(            /* Print to file select dlg  */
    HWND hDlg,
    WORD wHelpID);
UINT SYM_EXPORT CALLBACK BrowseHookProc(    /* COMMDLG file select hook  */
    HWND hWnd,
    UINT wMessage,
    WPARAM wParam,
    LPARAM lParam);

BOOL LOCAL NameValidateDir (LPSTR lpFile);

UINT SYM_EXPORT WINAPI OverwriteOrAppendDlgProc (HWND hDlg, 
                                             UINT uMessage, 
                                             WPARAM wParam,
                                             LPARAM lParam);


/** ALogPrint() *******************************************************
**
** This function prints the specified file.  If the pointer to the file
** name is NULL or points to a zero length string, no file is printed;
** the return value indicates the user's choice.  If the pointer is not
** NULL and the user selects "print to file," then the filename is
** returned in the buffer.  This buffer should be at least
** MAX_PATH_SIZE_SIZE+1 bytes in length.
**
***********************************************************************
** 05/09/93 WRL Function moved from print.c in NAVUTIL
**********************************************************************/
int SYM_EXPORT WINAPI ALogPrint(            /* Dlg to print a file       */
    LPPRINTDLGSTRUCT lpPrintDlgStruct)  // [in]
{
    int      nStatus;                   /* Return value              */


    nStatus = DialogBoxParam(hDLLInst, MAKEINTRESOURCE(IDD_PRINT), 
                             lpPrintDlgStruct->hParent,
                             PrintDlgProc, 
                             (LPARAM)lpPrintDlgStruct);
    return ((nStatus == -1) ? ALOG_PR_ERROR : nStatus);

} /* ALogPrint() */

/** PrintDlgProc() ****************************************************
**
** Future Enhancements:
** - Check for file existence before "printing to file"
***********************************************************************
** 05/09/93 WRL Function moved from print.c in NAVUTIL
**********************************************************************/
UINT SYM_EXPORT WINAPI PrintDlgProc(
    HWND hDlg,                          /* Me!                       */
    UINT Msg,                           /* What!                     */
    WPARAM wParam,                      /*    and                    */
    LPARAM lParam)                      /*    AND                    */
{
    auto    char    szTempFile  [SYM_MAX_PATH+1];
    auto    char    lpWork      [SYM_MAX_PATH+1];
    auto    UINT    uStatus;                /* Return value              */
    auto    HFILE   hFile;
    auto    HGLOBAL hBuffer;
    auto    HPBYTE  hpBuffer;
    auto    DWORD   dwBufferSize;
    auto    int     nResult;
    auto    LPSTR   lpszFile;
    auto    LPPRINTDLGSTRUCT lpPrintDlgStruct;
    auto    OVERWRITESTRUCT  overWriteStruct;


    switch (Msg)
        {
        case WM_CTLCOLOR:
            return ((UINT)Ctl3dCtlColorEx(Msg, wParam, lParam));

        case WM_INITDIALOG:
            lpPrintDlgStruct = (LPPRINTDLGSTRUCT) lParam;

            SetWindowLong(hDlg, DWL_USER, lParam); /* For future ref.   */

            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_CenterDialog(hDlg,GetParent(hDlg));
            NG_SetFontsInDialog(hDlg, 0);
                                       /* Get default file name     */
            LoadString(hDLLInst, IDS_DEFPRINTFILE, lpWork, sizeof(lpWork));

            Tunnel_LimitText(GetDlgItem(hDlg, IDI_PR_FILENAME), SYM_MAX_PATH);

                                       /* Tunnel_SetText            */
            SetDlgItemText ( hDlg, IDI_PR_FILENAME, 
                lpPrintDlgStruct->szFile[0] ? lpWork :
                                              lpPrintDlgStruct->szFile );

            if ( lpPrintDlgStruct->szTitle[0] != EOS )
                SetWindowText ( hDlg, lpPrintDlgStruct->szTitle );
            else 
                {
                LoadString(hDLLInst, IDS_PRINTTITLE, lpWork, sizeof(lpWork));
                SetWindowText(hDlg, lpWork);
                }

            Button_SetCheck(GetDlgItem(hDlg, IDI_PR_PRINTER), TRUE);

            EnableWindow(GetDlgItem(hDlg, IDI_PR_FILENAME), FALSE);

            if (!NMailIsMailAvailable()) 
                {
                EnableWindow(GetDlgItem(hDlg, IDI_PR_USER), FALSE);
                ShowWindow(GetDlgItem(hDlg, IDI_PR_USER), SW_HIDE);
                }

                                        // Register our help message
            uHelpPrintBrowseMsg = RegisterWindowMessage (HELPMSGSTRING);

            return (TRUE);

                                      /* WM_ACTIVATE: Set help context */
                                      /* for F1 help support.      */
        case WM_ACTIVATE:
            if (GET_WM_ACTIVATE_STATE(wParam, lParam))
                {
                lpPrintDlgStruct = 
                                (LPPRINTDLGSTRUCT)GetWindowLong(hDlg, DWL_USER);

                StdHelpSetContext (lpPrintDlgStruct->wHelpID,
                                   lpPrintDlgStruct->wHelpID);
                }
            break;

        case WM_COMMAND:
            lpPrintDlgStruct = (LPPRINTDLGSTRUCT)GetWindowLong(hDlg, DWL_USER);
            switch (GET_WM_COMMAND_ID(wParam, lParam))
                {
                case IDI_PR_FILE:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == BN_CLICKED)
                        EnableWindow(GetDlgItem(hDlg, IDI_PR_FILENAME), TRUE);
                    break;

                case IDI_PR_PRINTER:
                case IDI_PR_USER:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == BN_CLICKED)
                        EnableWindow(GetDlgItem(hDlg, IDI_PR_FILENAME), FALSE);
                    break;

                /**************************************************************
                ** If a fileid was specified, then print it.  If not, just
                ** return the exit code.  If the user selected a file to print,
                ** and a valid pointer to a filename buffer is passed, return
                ** the user's selected filename w/o performing copy.
                **************************************************************/

                case IDOK:
                    if (Button_GetCheck(GetDlgItem(hDlg, IDI_PR_FILE)))
                        {
                        Tunnel_GetText (GetDlgItem (hDlg, IDI_PR_FILENAME),
                                        szTempFile, SYM_MAX_PATH);
            
                        AnsiUpper (szTempFile);

                        if (NameValidatePath (szTempFile) == FALSE ||
                            NameValidateDir  (szTempFile) == FALSE ||
                            DiskIsReadOnly   (szTempFile))
                            {
                            MessageBeep((WORD)-1);
                            SetFocus (GetDlgItem (hDlg, IDI_PR_FILENAME));

                            LoadString (hDLLInst, IDS_BADFILE, lpWork,
                                        sizeof(lpWork));

                            MessageBox(hDlg, lpWork, NULL, MB_ICONSTOP | MB_OK);
                            break;
                            }
                        }
                                        //============================
                                        // If the user wants to build the 
                                        // info NOW, ask him to build it
                                        //============================
                    if ( lpPrintDlgStruct->lpfnBuild )
                        {
                        hBuffer = (*lpPrintDlgStruct->lpfnBuild)(lpPrintDlgStruct);

                        if ( hBuffer == NULL )
                            uStatus = ALOG_PR_ERROR;
                        else
                            {
                            hpBuffer = (HPBYTE) MemLock(hBuffer);
                                        // Now stuff this into a temporary file
                            GetTempFileName (0, "", 0, szTempFile);
                            if ((hFile = _lopen (szTempFile, OF_WRITE)) != HFILE_ERROR)
                                {
                                dwBufferSize = hstrlen(hpBuffer);

                                if ( FileHugeWrite (hFile, hpBuffer, dwBufferSize)
                                               != dwBufferSize)
                                    uStatus = ALOG_PR_ERROR;

                                FileClose (hFile);
                                }
                            else
                                uStatus = ALOG_PR_ERROR;

                            MemRelease(hBuffer);
                            MemFree(hBuffer);
                            }

                        if ( uStatus == ALOG_PR_ERROR )
                            goto locRet;
                        else
                            lpszFile = szTempFile;
                        }
                    else
                        {
                        lpszFile = lpPrintDlgStruct->szFile;
                        }
                                        //============================
                                        // PRINTER?
                                        //============================
                    if (Button_GetCheck(GetDlgItem(hDlg, IDI_PR_PRINTER))) 
                        {
                        if (lpszFile && lpszFile[0])
                            {
                            if ( lpPrintDlgStruct->szTitle[0] )
                                STRCPY ( lpWork, lpPrintDlgStruct->szTitle );
                            else
                                LoadString ( hDLLInst, IDS_PRINTTITLE,
                                             lpWork, sizeof(lpWork) );

                            uStatus = NptPrintAsciiFile(hDlg, NULL, lpWork,
                                                lpszFile, 
                                                lpPrintDlgStruct->bWordWrap,
                                                NPT_DRAFTMODE_OFF);

                            if ( !uStatus || uStatus == NPT_USERCANCEL )
                                uStatus = ALOG_PR_PRINTER;
                            else
                                uStatus = ALOG_PR_ERROR;
                            }
                        else 
                            uStatus = ALOG_PR_PRINTER;
                        }
                                        //============================
                                        // FILE?
                                        //============================
                    else if (Button_GetCheck(GetDlgItem(hDlg, IDI_PR_FILE))) 
                        {
                        if (lpszFile && lpszFile[0]) 
                            {
                            Tunnel_GetText(GetDlgItem(hDlg, IDI_PR_FILENAME),
                                           lpWork, sizeof(lpWork));

                                        // If file already exists, then
                                        // ask user if he wants to overwrite
                                        // it or append to it
                            nResult = OVERWRITE_FILE;
                            if ( FileExists(lpWork) )
                                {
                                overWriteStruct.lpszFile = lpWork;
                                overWriteStruct.wHelpID  = lpPrintDlgStruct->wHelpIDOverwrite;
                                nResult = OverwriteOrAppendDlg(hDlg, &overWriteStruct);
                                }

                            if ( nResult != OVERWRITE_APPEND_CANCEL )
                                {
                                if ( nResult == OVERWRITE_FILE )
                                    {
                                    FileDelete(lpWork);

                                    nResult = FileCopy(lpszFile, lpWork, NULL);
                                    }
                                else
                                    {
                                    nResult = FileAppend(lpszFile, lpWork, NULL);
                                    }

                                uStatus = (!nResult) ? ALOG_PR_FILE : ALOG_PR_ERROR;
                                }
                            else
                                uStatus = ALOG_PR_FILE;     // Pretend like we did it
                            } /* then */
                        else if (!lpszFile[0]) 
                            {
                            Tunnel_GetText(GetDlgItem(hDlg, IDI_PR_FILENAME),
                                        lpszFile, SYM_MAX_PATH);
                            uStatus = ALOG_PR_FILE;
                            } /* else */
                        else 
                            uStatus = ALOG_PR_FILE;
                        }
                                        //============================
                                        // MAIL TO USER?
                                        //============================
                    else if (Button_GetCheck(GetDlgItem(hDlg, IDI_PR_USER))) 
                        {
                        Tunnel_GetText(GetDlgItem(hDlg, IDI_PR_FILENAME),
                                       lpWork, sizeof(lpWork));

                        if (lpWork && lpWork[0]) 
                            {
                            FileCopy(lpszFile, lpWork, NULL);

                            NMailSendDocuments(lpWork, hDlg);
                                        // Assume message was sent okay.  Out
                                        // of our control from here!
                                        // Problem is that non-0 return code
                                        // could just mean the person aborted
                                        // the login or send process.
                            uStatus = ALOG_PR_USER;

                            FileDelete(lpWork);

                        } /* then */
                    else 
                        uStatus = ALOG_PR_USER;
                    }
                else
                    uStatus = ALOG_PR_ERROR;

                                        // Delete the temporary file                                                
locRet:
                FileDelete(szTempFile);

                EndDialog(hDlg, uStatus);
                break;

            case IDCANCEL:
                EndDialog(hDlg, ALOG_PR_CANCEL);
                break;

            case IDI_PR_FILENAME:
                if (GET_WM_COMMAND_CMD(wParam, lParam) == TN_TUNNEL)
                    {
                    lpPrintDlgStruct = 
                             (LPPRINTDLGSTRUCT)GetWindowLong(hDlg, DWL_USER);

                    BrowsePrintFile(hDlg, lpPrintDlgStruct->wHelpIDBrowse);
                    }
                break;

            case IDHELP:
                lpPrintDlgStruct = 
                             (LPPRINTDLGSTRUCT)GetWindowLong(hDlg, DWL_USER);

                StdHelpFromDialog(hDlg, lpPrintDlgStruct->wHelpID);
                break;
            } /* switch (wParam) */
        break;

        default:
            if ( Msg == uHelpPrintBrowseMsg )
                {
                lpPrintDlgStruct = 
                             (LPPRINTDLGSTRUCT)GetWindowLong(hDlg, DWL_USER);

                StdHelpFromDialog (hDlg, lpPrintDlgStruct->wHelpIDBrowse);
                }
    } /* switch (Msg) */

    return 0;
}

BOOL LOCAL NameValidateDir (LPSTR lpFile)
{
    auto    char    szFullPath [ SYM_MAX_PATH+1 ];
    auto    BOOL    bStatus;
    auto    UINT    wAttr;

    STRCPY (szFullPath, lpFile);

    NameStripFile (szFullPath);

    bStatus = FileGetAttr (szFullPath, &wAttr);

    if (bStatus == ERR || wAttr != FA_DIR)
        bStatus = FALSE;
    else
        bStatus = TRUE;

    return (bStatus);
}


BOOL LOCAL DiskIsReadOnly (LPSTR lpFilename)
{
    auto        BOOL            bReadOnly = TRUE;
    auto        char            szTempFile [SYM_MAX_PATH+1];
    auto        char            szFileName [SYM_MAX_PATH+1];
    auto        HFILE           hFile;
    auto        UINT            uFileWrite;
    auto        UINT            uFileClose;
    auto        UINT            uCount;
#define MAX_TRIES       100

                                        // ----------------------------------
                                        // Attempt to write to a temporary
                                        // file on the same directory as
                                        // the output file
                                        // ----------------------------------
    STRCPY(szTempFile, lpFilename);
    NameStripFile(szTempFile);
    for (uCount = 0; uCount < MAX_TRIES; uCount++)
        {
        SPRINTF(szFileName, "%06u.TMP", uCount);
        NameAppendFile(szTempFile, szFileName);
        if ( !FileExists (szTempFile) )
            break;            

        NameStripFile(szTempFile);
        }
    if ( uCount >= MAX_TRIES )
        return(bReadOnly);

    hFile = _lcreat(szTempFile, 0);
                                        // Write some junk out 
    if ( hFile != HFILE_ERROR )
        {
        uFileWrite = _lwrite(hFile, szTempFile, 1);
        uFileClose = _lclose(hFile);
                                        // Delete the file
        FileDelete(szTempFile);

        if ((uFileWrite != HFILE_ERROR) && (uFileClose != HFILE_ERROR))
            bReadOnly = FALSE;
        }

    return (bReadOnly);
}

/**********************************************************************
**
**********************************************************************/
UINT SYM_EXPORT WINAPI OverwriteOrAppendDlg(HWND hDlg, LPOVERWRITESTRUCT lpOverwriteStruct)
{
   auto         DLGPROC         lpfn;
   auto         int             nStatus;



   lpfn = (DLGPROC) MakeProcInstance((FARPROC) OverwriteOrAppendDlgProc, 
                                     hDLLInst);
   nStatus = DialogBoxParam(hDLLInst, MAKEINTRESOURCE(IDD_OVERWRITE_APPEND),
                            hDlg, lpfn, (LPARAM)lpOverwriteStruct);
   FreeProcInstance((FARPROC) lpfn);

   return ((nStatus == -1) ? OVERWRITE_APPEND_CANCEL : nStatus);
}



/**********************************************************************
**
**********************************************************************/

UINT SYM_EXPORT WINAPI OverwriteOrAppendDlgProc (HWND hDlg, 
                                             UINT uMessage, 
                                             WPARAM wParam,
                                             LPARAM lParam)
{
   auto         char            szOut    [256+1];
   auto         char            szAnother[256+1];


   switch (uMessage)
      {
      case WM_CTLCOLOR:
         return ((UINT)Ctl3dCtlColorEx (uMessage, wParam, lParam));

      case WM_INITDIALOG:
         {
         LPOVERWRITESTRUCT lpOverwriteStruct = (LPOVERWRITESTRUCT) lParam;

         SetWindowLong(hDlg, DWL_USER, lParam); /* For future ref.   */

         Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
         NG_CenterDialog (hDlg, GetParent(hDlg));
         NG_SetFontsInDialog (hDlg, 0);

         LoadString (hDLLInst, IDS_ZAPFILE, szOut, sizeof(szOut));
         SPRINTF(szAnother, szOut, (LPSTR) lpOverwriteStruct->lpszFile);
         Static_SetText(GetDlgItem(hDlg, IDI_ZAP_TEXT1), szAnother);
         }
         return (TRUE);
         break;

      case WM_DESTROY:
         break;
                                        // WM_ACTIVATE:  Set help context
                                        // for F1 help support.
      case WM_ACTIVATE:
         if (GET_WM_ACTIVATE_STATE(wParam, lParam))
            {
            LPOVERWRITESTRUCT lpOverwriteStruct = 
                                (LPOVERWRITESTRUCT) GetWindowLong(hDlg, DWL_USER);

            StdHelpSetContext (lpOverwriteStruct->wHelpID,
                               lpOverwriteStruct->wHelpID);
            }
         break;

      case WM_COMMAND:
         switch (GET_WM_COMMAND_ID(wParam, lParam))
            {
            case IDI_OVERWRITE:                 // Overwrite //
               EndDialog(hDlg, OVERWRITE_FILE);
               break;

            case IDI_APPEND:
               EndDialog(hDlg, APPEND_FILE);
               break;

            case IDCANCEL:
               EndDialog(hDlg, OVERWRITE_APPEND_CANCEL);
               break;

            case IDHELP:
               {
               LPOVERWRITESTRUCT lpOverwriteStruct = 
                                (LPOVERWRITESTRUCT) GetWindowLong(hDlg, DWL_USER);

               StdHelpFromDialog (hDlg, lpOverwriteStruct->wHelpID);
               }
               break;
            }
         break;
      }

   return (FALSE);
}


/**********************************************************************
**
**********************************************************************/
VOID WINAPI BrowsePrintFile(HWND hDlg, WORD wBrowseHelpID)
{
    OPENFILENAME OFName;                // Common dialog
    char         szzFilterSpec[80];     // Filter list spec
    char         szTitle[40];           // Dialog Title
    char         szFileSpec[SYM_MAX_PATH+1];
    BOOL         bRet = FALSE;          // Return code
    LPSTR        lpTemp;


    LoadString(hDLLInst, IDS_COMMDLG_TITLE, szTitle, sizeof(szTitle));
    LoadString(hDLLInst, WIDS_COMMDLG_FILTER, szzFilterSpec,
               sizeof(szzFilterSpec));
                                         /* Replace delims with \0    */
    for (lpTemp = STRTOK(szzFilterSpec,"|"); lpTemp;
         lpTemp = STRTOK(NULL,"|"));
                                         /* What is our default spec? */
    Tunnel_GetText(GetDlgItem(hDlg, IDI_PR_FILENAME), szFileSpec,
                   sizeof(szFileSpec));

                                         /* Set up the OFName structure */
                                         /* such that common dialog may */
                                         /* prompt for the files to scan. */
    MEMSET(&OFName, 0, sizeof(OFName));
    OFName.lStructSize       = sizeof(OFName);
    OFName.hwndOwner         = hDlg;
    OFName.hInstance         = hDLLInst;
    OFName.lpstrFilter       = szzFilterSpec;
    OFName.lpstrFile         = szFileSpec;
    OFName.nMaxFile          = sizeof(szFileSpec);
    OFName.lpstrTitle        = szTitle;
                                        // Removed Help button
    OFName.Flags             = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
                               // OFN_SHOWHELP | 
                               OFN_ENABLEHOOK;
    OFName.lpstrDefExt       = szzFilterSpec;
    OFName.lpfnHook          = BrowseHookProc;
    OFName.lCustData         = wBrowseHelpID;

    GetOpenFileName(&OFName);

    STRLWR(szFileSpec);
                                        // Update our edit field.
    Tunnel_SetText (GetDlgItem (hDlg, IDI_PR_FILENAME), szFileSpec);
}

/**********************************************************************
**
**********************************************************************/
UINT SYM_EXPORT CALLBACK BrowseHookProc(
    HWND hWnd,
    UINT wMessage,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (wMessage) {
    case WM_INITDIALOG:
        Ctl3dSubclassDlg(hWnd, CTL3D_ALL);
        NG_CenterDialog(hWnd,GetParent(hWnd));
        SetProp(hWnd, SZ_PRINT_BROWSE_HOOK, (HANDLE)((LPOPENFILENAME) lParam)->lCustData);
// Make sure ALL browse dialogs do this.  Right now, only this one does, so
// we will remove it for now.  BRK
//        NG_SetFontsInDialog(hWnd, 0);
        return (FALSE);
                                        // WM_ACTIVATE:  Another help
                                        // kluge for F1.
    case WM_ACTIVATE:
         if (GET_WM_ACTIVATE_STATE(wParam, lParam))
             {
             WORD       wHelpID;

             wHelpID = (WORD) GetProp(hWnd, SZ_PRINT_BROWSE_HOOK);
             StdHelpSetContext (wHelpID, wHelpID);
             }
         return (FALSE);

    case WM_DESTROY:
        RemoveProp(hWnd, SZ_PRINT_BROWSE_HOOK);
        return (FALSE);

    case WM_CTLCOLOR:
        return ((UINT)Ctl3dCtlColorEx(wMessage, wParam, lParam));

    default:
        return (FALSE);
    }

    return (TRUE);
}



