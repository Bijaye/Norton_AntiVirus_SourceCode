// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_save.cpv   1.3   10 Aug 1997 22:48:24   DALLEE  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_save.cpv  $
// 
//    Rev 1.3   10 Aug 1997 22:48:24   DALLEE
// Removed advotext from regular NAVWNT SaveAs dialogs - only show advotext
// for Scanner versions now.
// 
//    Rev 1.2   15 Jul 1997 14:42:08   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.1   22 May 1997 16:21:00   BILL
// Alpha changes (removed ctl3d)
// 
//    Rev 1.0   06 Feb 1997 21:02:38   RFULLER
// Initial revision
// 
//    Rev 1.5   12 Nov 1996 10:28:16   GWESTER
// Ported in fix 74094 from Hydra. The Save As... dialog now uses Netscape's
// reg key to store the default save location, so that you don't have to go
// to the save location every time you download. The fix is Netscape 3.0
// specific.
// 
//    Rev 1.4   07 Aug 1996 15:38:18   JBRENNA
// LuigiPlus Port: DALLEE: If user doesn't type extension (easy to do if hide
// known extesions is on), then append the default extension. If user ends
// input with a '.' then no extension is added.
// 
// Changed old define for buffer sizes from MAX_PATH_SIZE to SYM_MAX_PATH.
// 
// 
//    Rev 1.3   11 Jul 1996 08:03:58   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.2   28 Jun 1996 14:17:18   GWESTER
// Ported in fixes / changes from Luigi Plus.
//
//    Rev 1.1   22 Feb 1996 14:08:08   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:55:36   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:38   BARRY
// Initial revision.
//
//    Rev 1.5   29 Jan 1996 23:54:48   GWESTER
// In download mode, 0-file zips will trigger the Save As... dialog (instead of
// just bypassing it). This fixes Sphinx 48220.
//
//    Rev 1.4   29 Jan 1996 22:29:02   GWESTER
// In download mode, when we stop at a notification dialog, the main window
// icon (on the taskbar in 95, and on the desktop in NT) will return to a
// doctor's bag with the caption "Norton AntiVirus Scanner"; it won't display an
// intermediate percentage.
//
//    Rev 1.3   29 Jan 1996 15:17:10   GWESTER
// Added dialog to notify user when the downloaded file can't be saved. Also,
// added flag to Save As... so that user is prompted before overwriting a file
// from the Save As... dialog.
//
//    Rev 1.2   25 Jan 1996 19:03:58   GWESTER
// Used different text in Save As... dialogs depending upon whether this is a
// zip or not.
//
//    Rev 1.1   21 Jan 1996 18:05:18   GWESTER
// Added NT-specific downloading behavior.
//
//    Rev 1.0   21 Jan 1996 12:50:48   GWESTER
// Initial revision.
//
//************************************************************************

#include "platform.h"
#include "commdlg.h"
#include "xapi.h"
#include "ngui.h"
	#include "ctl3d.h"

#include "file.h"
#include "cmbstr.h"
#include "symcfg.h"
#include "stdhelp.h"

#include "navwork.h"
#include "navutil.h"
#include "options.h"
#include "exclude.h"
#include "navw_hlp.h"
#include "navalog.h"

#include "stub.h"
#include "virscan.h"
#include "nonav.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navw.h"


// ************************************************************************ //
// ** External Variables                                                 ** //
// ************************************************************************ //
extern   HINSTANCE   hInstance;

// ************************************************************************ //
// ** Local defines                                                      ** //
// ************************************************************************ //

#define PROP_ICON1                  100
#define PROP_ICON2                  101
#define PROP_LOGOBMP                102
#define PROP_LOGOBMPMASK            103

#define NETSCAPE_REG_FILESAVE_PATH  "Software\\Netscape\\Netscape Navigator\\Main"

// ************************************************************************ //
// ** Local Structures                                                   ** //
// ************************************************************************ //
typedef struct tagINFOSTRUCT
{
   BOOL         bInfected;                  // Are we infected?
   BOOL         bInZip;                     // Is this a zip?
   HICON        hDialogIcon;                // Our Logo Icon
   HICON        hAdvoTextIcon;              // Our AdvoText Icon
   HBITMAP      hLogoBmp;                   // Custom Logo (Netscape, etc.)
   HBITMAP      hLogoBmpMask;               // Mask for Custom Logo
} INFOSTRUCT, FAR * LPINFOSTRUCT;


// ************************************************************************ //
// ** Local prototypes                                                   ** //
// ************************************************************************ //

BOOL WINAPI SaveDownloadedFile (        // If we need to save it do it here.
    HWND        hDlg,                   // [in] Dialog box Handle
    LPNONAV     lpNav,                  // [in] Info on our scan
    BOOL        bInfected);             // [in] Was the file infected?

UINT SYM_EXPORT CALLBACK SaveAsHookProc (   // Hook proc for Log File Browse
    HWND        hDlg,                   // [in] Dialog box handle
    UINT        wMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam);                // [in] lParam to process

VOID WINAPI SaveAsWM_INITDIALOG (HWND hDlg, LPOPENFILENAME lpSaveInfo);

VOID WINAPI SaveAsWM_DESTROY (          // DeInitializes about box
    HWND        hDlg);                  // [in] Handle of dialog box

VOID WINAPI DrawSaveAsIcons (           // Draw application's icon */
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem);       // [in] What needs to be done

VOID WINAPI DrawCustomLogo (            // Draw application's custom logo bitmap
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem);       // [in] What needs to be done

VOID WINAPI DrawCustomSeparator (       // Draw application's separator
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem);       // [in] What needs to be done

VOID WINAPI ShrinkDialog (
    HWND             hDlg);             // [in] Handle to dialog box

// ************************************************************************ //
// ** The code!                                                          ** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI TryToSaveTheFile (HWND hDlg, LPNONAV lpNav)
{
   auto    BOOL   bStatus;

                                        // Restore the main window icon back
                                        // to a doctor's bag, and get rid
                                        // of the "99%" caption:
   auto HWND hWndOwner = (HWND)GetWindow((lpNav->hMommyWindow), GW_OWNER);
   if ( hWndOwner )
        {
        DrawNavIcon (hWndOwner, 100);
        if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
            {
                                        // Under NT, we still have to
                                        // restore the main window icon from
                                        // the animated scanner back
                                        // into the doctor's bag:
            auto HICON hIcon = NULL;

            hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON_NAV0));
            if ((hIcon) && (hWndOwner))
                {
                SetClassLong (hWndOwner, GCL_HICON, NULL);
                InvalidateRect (hWndOwner, NULL, TRUE);
                SetClassLong (hWndOwner, GCL_HICON, (LONG)hIcon);
                UpdateWindow (hWndOwner);
                DestroyIcon (hIcon);
                }
            }
        }

   if ( (lpNav->Stats.Files.dwScanned > 0L) || (lpNav->lpScan->bCompressedDownload) )
   {
      if (lpNav->lpScan->bDeleteZipFile == TRUE)
      {
         STRCPY (lpNav->Infected.szFullPath, lpNav->lpScan->szSaveFileName);

         bStatus = (FileDelete (lpNav->Infected.szFullPath) == NOERR);

         NotifyDeleteCompletion (hDlg, bStatus, lpNav);
      }
      else if (lpNav->lpScan->bFileDeleted == FALSE)
      {
         if (lpNav->Stats.Files.uInfected > lpNav->Stats.Files.uCleaned)
            SaveDownloadedFile(hDlg, lpNav, TRUE);
         else
            SaveDownloadedFile(hDlg, lpNav, FALSE);
      }
   }
}



BOOL WINAPI SaveDownloadedFile (HWND hDlg, LPNONAV lpNav, BOOL bInfected)
{
   auto    OPENFILENAME    SaveName;   // Common dialog
   auto    char            szSaveFileName [SYM_MAX_PATH+1];
   auto    char            szFileName [SYM_MAX_PATH+1];
   auto    char            szReturnTitle [SYM_MAX_PATH+1];
   auto    BOOL            bReturn = TRUE; // Assume success!

   auto    char            szFilterBuffer [SYM_MAX_PATH+1];

   auto    char            szTitleBuffer [SYM_MAX_PATH+1];
   auto    BOOL            bTitleOk;

   auto    INFOSTRUCT      rInfo;

   auto    BOOL            bMoveFileReturn;
   auto    DWORD           dwMoveFileError;

   auto    HKEY            hRegKey = 0;
   auto    char            szNetscapeSaveDir [SYM_MAX_PATH+1];
   auto    DWORD           dwSizeofNetscapeSaveDir = sizeof(szNetscapeSaveDir);

                                        // This is our starting filename.
   STRCPY (szSaveFileName, lpNav->lpScan->szSaveFileName);

                                        // Now that we have the file,
                                        // Save it so that we can copy.
   NameReturnFile(szSaveFileName, szFileName);

                                        // Get the Dialog Title
   bTitleOk = LoadString (hInstance,
                          bInfected ?
                             IDS_SAVETITLE_INFECTED :
                             IDS_SAVETITLE_CLEAN,
                          szTitleBuffer,
                          sizeof(szTitleBuffer));

   NavLoadString (hInstance,
                  IDS_SAVE_FILTER,
                  szFilterBuffer,
                  sizeof(szFilterBuffer));

   rInfo.bInfected   = bInfected;       // Setup to pass into custom save!

                                        // Was this a zip?
   rInfo.bInZip      = lpNav->lpScan->bCompressedDownload;

                                        // Grab our standard Icon.
   rInfo.hDialogIcon = LoadIcon (hInstance,
                                 MAKEINTRESOURCE (ID_ICON_NAV0));
                                        // And our AdvoText Icon.
   rInfo.hAdvoTextIcon = LoadIcon (hInstance,
                                   MAKEINTRESOURCE (ID_ICON_ADVOTEXT));

   if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                               // Prepare to draw the Netscape logo:
        rInfo.hLogoBmp = LoadBitmap ( hInstance, MAKEINTRESOURCE ( IDB_NETSCAPELOGO ) );
        rInfo.hLogoBmpMask = LoadBitmap ( hInstance, MAKEINTRESOURCE ( IDB_NETSCAPELOGO_MASK ) );
        }
        else
        {                               // No custom logo:
        rInfo.hLogoBmp = NULL;
        rInfo.hLogoBmpMask = NULL;
        }

                                        // Prepare SAVEAS struct
   SaveName.lStructSize       = sizeof(OPENFILENAME);
   SaveName.hwndOwner         = hDlg;
   SaveName.hInstance         = hInstance;
   SaveName.lpstrFilter       = szFilterBuffer;
   SaveName.lpstrCustomFilter = NULL;
   SaveName.nMaxCustFilter    = 0;
   SaveName.nFilterIndex      = 0;
   SaveName.lpstrFile         = szFileName;
   SaveName.nMaxFile          = sizeof(szFileName);
   SaveName.lpstrFileTitle    = szReturnTitle;
   SaveName.nMaxFileTitle     = sizeof(szReturnTitle);
   SaveName.lpstrInitialDir   = NULL;

                                        // Grab Netscape's save directory 
                                        // (works with Navigator 3+)
   
   if(ERROR_SUCCESS == (RegOpenKeyEx(HKEY_CURRENT_USER,
                                    NETSCAPE_REG_FILESAVE_PATH,
                                    0,
                                    KEY_ALL_ACCESS,
                                    &hRegKey)))	
        {
        if(ERROR_SUCCESS == RegQueryValueEx( hRegKey,
                                             "Default Save Dir",	// Do Not Translate
                                             NULL,
                                             NULL,
                                             (LPBYTE)szNetscapeSaveDir,
                                             &dwSizeofNetscapeSaveDir))
            {
            SaveName.lpstrInitialDir = szNetscapeSaveDir;
            }

        }

   SaveName.lpstrTitle        = bTitleOk ?
                                   szTitleBuffer : NULL;      // If the title load failed.
   if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
   		{
    	SaveName.Flags        = OFN_FILEMUSTEXIST
        	                  | OFN_PATHMUSTEXIST
            	              | OFN_HIDEREADONLY
                	          | OFN_EXPLORER
                    	      | OFN_LONGNAMES
                        	  | OFN_ENABLETEMPLATE
                              | OFN_ENABLEHOOK
                              | OFN_OVERWRITEPROMPT ;
    	}
    	else
    	{
    	SaveName.Flags        = OFN_FILEMUSTEXIST
                              | OFN_PATHMUSTEXIST
                              | OFN_HIDEREADONLY
                              | OFN_LONGNAMES
                              | OFN_ENABLETEMPLATE
                              | OFN_ENABLEHOOK
                              | OFN_OVERWRITEPROMPT ;
    	}
   SaveName.nFileOffset       = 0;      // Output only!
   SaveName.nFileExtension    = 0;      // Output only!
   SaveName.lpstrDefExt       = NULL;
   SaveName.lCustData         = (LPARAM) (LPINFOSTRUCT) &rInfo;
   SaveName.lpfnHook          = SaveAsHookProc;

   if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
    	{
        if (bInfected)
    	    SaveName.lpTemplateName    = MAKEINTRESOURCE(IDD_DOWNLOADSAVEINFECTED_95);
            else
    	    SaveName.lpTemplateName    = MAKEINTRESOURCE(IDD_DOWNLOADSAVEAS_95);
    	}
    	else
    	{
        if (bInfected)
        	SaveName.lpTemplateName    = MAKEINTRESOURCE(IDD_DOWNLOADSAVEINFECTED_NT);
            else
        	SaveName.lpTemplateName    = MAKEINTRESOURCE(IDD_DOWNLOADSAVEAS_NT);
    	}

   if (GetSaveFileName (&SaveName))
        {
                                        // Update Netscape's save dir with the 
                                        // user's latest choice.  Works just like
                                        // mother's own this way.
        if(hRegKey)
            {
            NameReturnPath(szFileName, szNetscapeSaveDir);
            RegSetValueEx(hRegKey,
                          "Default Save Dir",	// Do Not Translate
                          NULL,
                          REG_SZ,
                          (LPBYTE)szNetscapeSaveDir,
                          STRLEN(szNetscapeSaveDir));
            }

                                        // Check for lost extension. Happens
                                        // when hiding known extensions and
                                        // modifying filename text edit.
        if ( (0 != SaveName.nFileExtension) &&
             (EOS == szFileName[SaveName.nFileExtension]) &&
             ('.' != szFileName[SaveName.nFileExtension-1]) )
            {
            NameReturnExtensionEx(szSaveFileName,
                                  szReturnTitle,
                                  sizeof(szReturnTitle));
            NameAppendExtension(szFileName, szReturnTitle);
            }

                                        // szSaveFileName is where the file
                                        // is now.

                                        // szFileName is what the user wants
                                        // to save it as.
        if ((STRICMP(szSaveFileName, szFileName)) != 0)
            {
                                        // The user specifed a different
                                        // filename than where the file
                                        // is now.

                                        // MoveFile() won't overwrite. If the target file
                                        // already exists, delete it.

            FINDDATA UnusedRec;         // Required for Quake call
            if ((FileFindFirst(szFileName, &UnusedRec)) != INVALID_HANDLE_VALUE)
                {
                FileDelete (szFileName);
                }

                                        // Send it to the new location:
            bMoveFileReturn = MoveFile(szSaveFileName, szFileName);

            if ( bMoveFileReturn == TRUE )
                {
                                        // Everything worked:
                bReturn = TRUE;
                }
                else
                {
                                        // Some error occurred:
                dwMoveFileError = GetLastError();

                if ( dwMoveFileError == ERROR_DISK_FULL )
                    {
                                        // Out of disk space:
                    LoadString ( hInstance, IDS_DOWNLOAD_OUTOFSPACE, szReturnTitle, sizeof (szReturnTitle));
                    SPRINTF (szTitleBuffer,szReturnTitle,szFileName);
                    }
                    else
                    {
                                        // Any other error:
                    LoadString ( hInstance, IDS_DOWNLOAD_CANNOTCREATE, szReturnTitle, sizeof (szReturnTitle));
                    SPRINTF (szTitleBuffer,szReturnTitle,szFileName);
                    }
                                        // In loading strings, reuse
                                        // szReturnTitle to load the caption
                                        // and szTitleBuffer to load the
                                        // body text.
                if ( GetCustomVersion() == VERSION_NETSCAPE )
                    {                   // Title is "Internet Scanner"; set at runtime.
                    LoadString ( hInstance, IDS_NAV_INTERNET, szReturnTitle, sizeof (szReturnTitle));
                    }
                    else
                    {
                    LoadString ( hInstance, IDS_NAV, szReturnTitle, sizeof (szReturnTitle));
                    }
                MessageBox (NULL,szTitleBuffer,szReturnTitle,MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
                bReturn = FALSE;
                }
            }
        }
   else
        {
                                        // Cancel was pressed in Save As ...
                                        // dialog:

        FileDelete ( szSaveFileName );  // Delete the original file
        bReturn = FALSE;
        }                               // End "Cancel pressed"

                                        // Keep things tidy!
    if(hRegKey)                         // Close the Registry if needed
        {
        RegCloseKey(hRegKey);
        hRegKey = 0;
        }

   if (rInfo.hDialogIcon)
      DestroyIcon (rInfo.hDialogIcon);

   if (rInfo.hAdvoTextIcon)
      DestroyIcon (rInfo.hAdvoTextIcon);

   if (rInfo.hLogoBmp)
      DeleteObject (rInfo.hLogoBmp);

   if (rInfo.hLogoBmpMask)
      DeleteObject (rInfo.hLogoBmpMask);

   return bReturn;
}





//************************************************************************
// SaveAsHookProc()
//
// This procedure is for hooking the common dialog SaveAs Dialog Box.
// The purpose of this hook is to provide a 3D look via CTL3D.
//
// Parameters:
//      hDlg            [in] Dialog Box handle
//      wMessage        [in] message to process
//      wParam          [in] wParam of message
//      lParam          [in] lParam of message
//
// Returns:
//      FALSE           Didn't process message
//      TRUE            Processed message, no need for COMMDLG.DLL to
//                      process it.
//************************************************************************
// 01/17/95 MSL Function created.
//************************************************************************
UINT SYM_EXPORT CALLBACK SaveAsHookProc (   // Hook proc for Log File Browse
    HWND        hDlg,                   // [in] Dialog box handle
    UINT        wMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam)                 // [in] lParam to process
{
    LPNMHDR pnmhTemp;

    switch (wMessage)
        {
                                        // WM_INITDIALOG:  Set up for 3D
                                        // look/feel.
        case WM_INITDIALOG:
            SaveAsWM_INITDIALOG(hDlg, (LPOPENFILENAME) lParam);
            return (FALSE);

        case WM_CTLCOLOR:
            return ((UINT)Ctl3dCtlColorEx(WM_CTLCOLOR, wParam, lParam));
        case WM_DRAWITEM:

            switch (((LPDRAWITEMSTRUCT)lParam)->CtlID)
                {
                                        /* We need to paint the icon    */
                                        /* for the box.                 */
                case ID_DLG_ICON1:
                case ID_DLG_ICON2:
                    DrawSaveAsIcons (hDlg, (LPDRAWITEMSTRUCT)lParam);
                    break;

                case IDI_CS_NETSCAPELOGO:
                    DrawCustomLogo (hDlg, (LPDRAWITEMSTRUCT)lParam);
                    break;

                case IDI_CS_STATIC_3DSEPARATOR:
                    DrawCustomSeparator (hDlg, (LPDRAWITEMSTRUCT)lParam);
                    break;

                default:
                    return FALSE;
                }
           break;

        case WM_DESTROY:
           SaveAsWM_DESTROY (hDlg);
           break;

        case WM_NOTIFY:
            pnmhTemp = (LPNMHDR) lParam;
            if (pnmhTemp)
                {
                if (pnmhTemp->code == CDN_INITDONE)
                    {
                                        // Windows has finished expanding the dialog. We
                                        // can now tamper with it:
                    if ( ( GetCustomVersion() != VERSION_NETSCAPE ) && ( SystemGetWindowsType() == SYM_SYSTEM_WIN_95 ) )
                        {               // If this is not Netscape-specific, collect the
                                        // open space, and shrink the dialog:
                        ShrinkDialog (hDlg);
                        }
                    NG_CenterDialog (GetParent(hDlg), GetDesktopWindow());
                    }
                }
            break;

        default:
            return (FALSE);
        }

                                        // Return to caller
    return (TRUE);
} // SaveAsHookProc()


VOID WINAPI SaveAsWM_INITDIALOG (HWND hDlg, LPOPENFILENAME lpSaveInfo)
{
auto    char          szInfectedMessage     [ SYM_MAX_PATH + 1 ];
auto    char          szInfectedMessageLong [ 2 * (SYM_MAX_PATH + 1) ];
auto    LPINFOSTRUCT  lpInfo;

   lpInfo = (LPINFOSTRUCT)(lpSaveInfo->lCustData);

   Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
   NG_CenterDialog (hDlg, GetDesktopWindow());

   NG_SetFontsInDialog (hDlg, 0);
   NG_SetChildSmallFont (hDlg, IDI_CS_MSG_AREWEINFECTED);
   NG_SetChildSmallFont (hDlg, IDI_CS_MSG_AREWEINFECTED2);
   NG_SetChildSmallFont (hDlg, IDI_CS_MSG_AREWEINFECTED3);
   NG_SetChildSmallFont (hDlg, IDI_CS_MSG_ADVOTEXT);
   NG_SetChildSmallFont (hDlg, IDI_CS_MSG_PRODUCT);
   NG_SetChildSmallFont (hDlg, IDI_CS_MSG_CUSTOMVERSION);

   if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
                                        // Use the standard font on all of
                                        // the NT controls:
        NG_SetChildSmallFont (hDlg, 1090);
        NG_SetChildSmallFont (hDlg, IDI_CS_FILENAME);
        NG_SetChildSmallFont (hDlg, IDI_CS_FILEBOX);
        NG_SetChildSmallFont (hDlg, IDI_CS_DIRECTORYLABEL);
        NG_SetChildSmallFont (hDlg, IDI_CS_DIRECTORY);
        NG_SetChildSmallFont (hDlg, IDI_CS_DIRECTORYBOX);
        NG_SetChildSmallFont (hDlg, 1089);
        NG_SetChildSmallFont (hDlg, IDI_CS_FILETYPES);
        NG_SetChildSmallFont (hDlg, 1091);
        NG_SetChildSmallFont (hDlg, IDI_CS_DRIVES);
        NG_SetChildSmallFont (hDlg, 1);
        NG_SetChildSmallFont (hDlg, 2);
        NG_SetChildSmallFont (hDlg, 1038);
        NG_SetChildSmallFont (hDlg, 1040);
        }

   if (lpInfo->bInfected)
        {
        if (lpInfo->bInZip)
            {
                                        // Use the infected zip warning at
                                        // the top of the dialog:
            LoadString (hInstance,
               IDS_SAVE_INFECTEDZIP_LINE1,
               szInfectedMessageLong,
               sizeof(szInfectedMessageLong));

            LoadString (hInstance,
               IDS_SAVE_INFECTEDZIP_LINE2,
               szInfectedMessage,
               sizeof(szInfectedMessage));

            STRCAT ( szInfectedMessageLong, szInfectedMessage );

            SetDlgItemText (hDlg,
                   IDI_CS_MSG_AREWEINFECTED3,
                   szInfectedMessageLong);

            ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED3) , SW_SHOW );
            ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED2) , SW_HIDE );
            ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED) , SW_HIDE );
            }
            else
            {
                                        // Use the infected file warning at
                                        // the top of the dialog:
            LoadString (hInstance,
               IDS_SAVE_INFECTED_LINE1,
               szInfectedMessageLong,
               sizeof(szInfectedMessageLong));

            LoadString (hInstance,
               IDS_SAVE_INFECTED_LINE2,
               szInfectedMessage,
               sizeof(szInfectedMessage));

            STRCAT ( szInfectedMessageLong, szInfectedMessage );

            SetDlgItemText (hDlg,
                   IDI_CS_MSG_AREWEINFECTED2,
                   szInfectedMessageLong);

            ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED3) , SW_HIDE );
            ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED2) , SW_SHOW );
            ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED) , SW_HIDE );
            }
        }
        else
        {
                                        // File is clean.
                                        // Use the seal-of-approval text
                                        // at the top of the dialog:
        LoadString (hInstance,
               IDS_SAVE_CLEAN,
               szInfectedMessage,
               sizeof(szInfectedMessage));

        SetDlgItemText (hDlg,
                   IDI_CS_MSG_AREWEINFECTED,
                   szInfectedMessage);

        ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED3) , SW_HIDE );
        ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED2) , SW_HIDE );
        ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_AREWEINFECTED) , SW_SHOW );
        }


                                        // Load the Advotext strings. Reuse our "infected
                                        // text" buffers :
	LoadString (hInstance,
               IDS_ADVOTEXT_LINE1,
               szInfectedMessageLong,
               sizeof(szInfectedMessageLong));

	LoadString (hInstance,
               IDS_ADVOTEXT_LINE2,
               szInfectedMessage,
               sizeof(szInfectedMessage));

	STRCAT ( szInfectedMessageLong, szInfectedMessage );

	SetDlgItemText (hDlg,
                   IDI_CS_MSG_ADVOTEXT,
                   szInfectedMessageLong);

   if ( GetCustomVersion() != VERSION_NETSCAPE )
        {                               // If this isn't Netscape, hide the Netscape logo:
        ShowWindow ( GetDlgItem (hDlg, IDI_CS_NETSCAPELOGO) , SW_HIDE );
        }

   if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                               // Set version and custom version; reuse szInfectedMessage
        LoadString (hInstance, IDS_NAV_INTERNET, szInfectedMessage, sizeof(szInfectedMessage));
        SetDlgItemText (hDlg, IDI_CS_MSG_PRODUCT, szInfectedMessage);
        LoadString (hInstance, IDS_NAV_NETSCAPEVERSION, szInfectedMessage, sizeof(szInfectedMessage));
        SetDlgItemText (hDlg, IDI_CS_MSG_CUSTOMVERSION, szInfectedMessage);
        }
        else
        {                               // Set version and custom version; reuse szInfectedMessage
        ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_PRODUCT) , SW_HIDE );
        ShowWindow ( GetDlgItem (hDlg, IDI_CS_MSG_CUSTOMVERSION) , SW_HIDE );
        }


   if ( GetCustomVersion() == VERSION_NETSCAPE )
        {                               // If this is the Netscape Save As..., show the
                                        // separator:
        if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
    	    {                               // Show the 3D separator; hide the flat one:
            ShowWindow ( GetDlgItem (hDlg, IDI_CS_STATIC_3DSEPARATOR) , SW_SHOW );
            }
            else
    	    {                               // Show the flat separator; hide the 3D one:
            ShowWindow ( GetDlgItem (hDlg, IDI_CS_STATIC_SEPARATOR) , SW_SHOW );
            }
        }

                                        // Make our ICONs / BMPs accessible in draw!
   SetProp (hDlg,
             MAKEINTRESOURCE(PROP_ICON1),
             (HANDLE)lpInfo->hDialogIcon);

   SetProp (hDlg,
             MAKEINTRESOURCE(PROP_ICON2),
             (HANDLE)lpInfo->hAdvoTextIcon);

   SetProp (hDlg,
             MAKEINTRESOURCE(PROP_LOGOBMP),
             (HANDLE)lpInfo->hLogoBmp);

   SetProp (hDlg,
             MAKEINTRESOURCE(PROP_LOGOBMPMASK),
             (HANDLE)lpInfo->hLogoBmpMask);


                                        // Windows has finished expanding the dialog. We
                                        // can now tamper with it:
    if ( ( GetCustomVersion() != VERSION_NETSCAPE ) && ( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT ) )
        {                               // If this is not Netscape-specific, collect the
                                        // open space, and shrink the dialog:
        ShrinkDialog (hDlg);
        NG_CenterDialog (GetParent(hDlg), GetDesktopWindow());
        }


}


//**********************************************************************//
//                                                                      //
// Description:                                                         //
//      This routine deinitializes the SaveAs Dialog Box.               //
//                                                                      //
//**********************************************************************//
// 01/17/96 DSACKING: Function Created                                  //
//**********************************************************************//
VOID WINAPI SaveAsWM_DESTROY (          // DeInitializes about box
    HWND        hDlg)                   // [in] Handle of dialog box
{

                                        // Destroy our Icon properties.
    RemoveProp (hDlg, MAKEINTRESOURCE(PROP_ICON1));
    RemoveProp (hDlg, MAKEINTRESOURCE(PROP_ICON2));

                                        // Destroy our Bitmap properties.
    RemoveProp (hDlg, MAKEINTRESOURCE(PROP_LOGOBMP));
    RemoveProp (hDlg, MAKEINTRESOURCE(PROP_LOGOBMPMASK));
}

//**********************************************************************//
//                                                                      //
// Description:                                                         //
//      Draw the SaveAs Dialog's Icons                                  //
//                                                                      //
//**********************************************************************//
// 01/17/96 DSACKING : Function Taken from StdAbout                     //
//**********************************************************************//
VOID WINAPI DrawSaveAsIcons (           // Draw application's icon */
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem)        // [in] What needs to be done
{
    HICON       hIcon;
    HBRUSH      hOldBrush = NULL;
    HBRUSH      brButtonFace = NULL;
    int         nProperty;

    if (lpDrawItem->CtlID == ID_DLG_ICON1)
       nProperty = PROP_ICON1;
    else
       nProperty = PROP_ICON2;

    if ((hIcon = (HICON)GetProp (hDlg, MAKEINTRESOURCE(nProperty))) != NULL)
        {
        InflateRect (&(lpDrawItem->rcItem), 5, 5);
        brButtonFace = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
        hOldBrush = (HBRUSH)SelectObject (lpDrawItem->hDC, brButtonFace);
        Rectangle (lpDrawItem->hDC,
                   lpDrawItem->rcItem.left,
                   lpDrawItem->rcItem.top,
                   lpDrawItem->rcItem.right,
                   lpDrawItem->rcItem.bottom);
        DrawIcon (lpDrawItem->hDC,
                  0, 0, hIcon);
        if (hOldBrush != NULL)
            {
            SelectObject (lpDrawItem->hDC, hOldBrush);
            }
        DeleteObject ( brButtonFace );
        }
}                                       // End DrawSaveAsIcons()


//**********************************************************************//
//                                                                      //
// Description:                                                         //
//      Draw the SaveAs Dialog's Custom Logo Bitmap                     //
//                                                                      //
//**********************************************************************//
// 03/02/96 GWESTER : Function Created                                  //
//**********************************************************************//
VOID WINAPI DrawCustomLogo (            // Draw application's custom logo
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem)        // [in] What needs to be done
{



    HBITMAP     hBmp            = NULL;
    HBITMAP     hBmpMask        = NULL;
    HBITMAP     hBmpMem         = NULL;
    HBITMAP     hBmpOld         = NULL;
    HDC         hDCMem          = NULL;
    BITMAP      BitmapInfo;
    RECT        rBitmap;

                                        // Get the bitmap and the mask we
                                        // will be working with.
    hBmp     = (HBITMAP)GetProp(hDlg, MAKEINTRESOURCE(PROP_LOGOBMP));
    hBmpMask = (HBITMAP)GetProp(hDlg, MAKEINTRESOURCE(PROP_LOGOBMPMASK));

    if ( hBmp && hBmpMask )
        {
                                        // Get the size of the client rect
                                        // and create a compatible DC as
                                        // well as a compatible bitmap the
                                        // size of our client rect.
        hDCMem  = CreateCompatibleDC ( lpDrawItem->hDC );
        hBmpMem = CreateCompatibleBitmap ( lpDrawItem->hDC,
                                           lpDrawItem->rcItem.right -
                                               lpDrawItem->rcItem.left,
                                           lpDrawItem->rcItem.bottom -
                                               lpDrawItem->rcItem.top );

        if ( hDCMem && hBmpMem )
            {
                                        // Select the bitmap into the off-
                                        // screen DC.
            hBmpOld = (HBITMAP)SelectObject ( hDCMem, hBmpMem );

                                        // Adjust our bitmap rectangle
                                        // coordinates.
            rBitmap.left   = 0;
            rBitmap.top    = 0;
            rBitmap.right  = lpDrawItem->rcItem.right - lpDrawItem->rcItem.left;
            rBitmap.bottom = lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top;

                                        // In our off-screen DC, erase the
                                        // background such that we don't
                                        // leave trails.
            SetBkColor ( hDCMem, GetSysColor ( COLOR_BTNFACE ) );
            ExtTextOut ( hDCMem, 0, 0, ETO_OPAQUE, &rBitmap, "", 0, NULL );

                                        // In our off-screen DC, now draw
                                        // our bitmap with its mask.
            GetObject ( hBmp, sizeof ( BITMAP ), &BitmapInfo );
            NGDrawMaskedBitmap ( hDCMem, hBmp, hBmpMask,
                                 (lpDrawItem->rcItem.right - lpDrawItem->rcItem.left) / 2 -
                                     BitmapInfo.bmWidth / 2,
                                 lpDrawItem->rcItem.top );

                                        // Now blit the changes to the
                                        // screen DC for a nice smooth
                                        // effect with no flicker.
            BitBlt ( lpDrawItem->hDC,
                     lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
                     lpDrawItem->rcItem.right - lpDrawItem->rcItem.left,
                     lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top,
                     hDCMem,
                     0, 0,
                     SRCCOPY );
            }
        }

                                        // Time for cleanup.
    if ( hDCMem )
        {
        SelectObject ( hDCMem, hBmpOld );
        DeleteDC ( hDCMem );
        }
    if ( hBmpMem )
        {
        DeleteObject ( hBmpMem );
        }

}                                       // End DrawCustomLogo()



//**********************************************************************//
//                                                                      //
// Description:                                                         //
//      Draw the SaveAs Dialog's Custom Separator                       //
//                                                                      //
//**********************************************************************//
// 03/02/96 GWESTER : Function Created                                  //
//**********************************************************************//
VOID WINAPI DrawCustomSeparator (       // Draw application's separator
    HWND             hDlg,              // [in] Handle to dialog box
    LPDRAWITEMSTRUCT lpDrawItem)        // [in] What needs to be done
{

    HPEN        hLightPen = NULL;
    HPEN        hDarkPen = NULL;
    HPEN        hOldPen = NULL;

    hLightPen = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_3DHILIGHT) );
    hDarkPen = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_3DSHADOW) );

    if ( (!hLightPen) || (!hDarkPen) ) {return;}

    hOldPen = (HPEN)SelectObject (lpDrawItem->hDC, hDarkPen);
    MoveToEx (lpDrawItem->hDC,lpDrawItem->rcItem.left,lpDrawItem->rcItem.top,NULL);
    LineTo (lpDrawItem->hDC,lpDrawItem->rcItem.right,lpDrawItem->rcItem.top);
    SelectObject (lpDrawItem->hDC, hLightPen);
    MoveToEx (lpDrawItem->hDC,lpDrawItem->rcItem.left,(lpDrawItem->rcItem.top)+1,NULL);
    LineTo (lpDrawItem->hDC,lpDrawItem->rcItem.right,(lpDrawItem->rcItem.top)+1);

    if (hOldPen != NULL)
        {
        SelectObject (lpDrawItem->hDC, hOldPen);
        }
    DeleteObject ( hLightPen );
    DeleteObject ( hDarkPen );

}                                       // End DrawCustomSeparator()



//**********************************************************************//
//                                                                      //
// Description:                                                         //
//      If the Save As... dialog is not for Netscape, move up the       //
//      location of the Advotext, and pull up the bottom of the dialog. //
//                                                                      //
//**********************************************************************//
// 03/02/96 GWESTER : Function Created                                  //
//**********************************************************************//
VOID WINAPI ShrinkDialog (
    HWND             hDlg)              // [in] Handle to dialog box
{
    auto    RECT    rCoords1,
                    rCoords2;
    auto    POINT   pCoord1;
    auto    int     iShrinkAmount;

                                        // For freeware Scanner versions,
                                        // move advotext to where the
                                        // separator is.
                                        // Full versions shrink dialog and
                                        // hide advotext.

                                        // Get location of the separator and
                                        // the advotext:

    GetWindowRect( GetDlgItem( hDlg, IDI_CS_STATIC_3DSEPARATOR ), &rCoords1 );
    GetWindowRect( GetDlgItem( hDlg, IDI_CS_MSG_ADVOTEXT ), &rCoords2 );


                                        // Calculate the shrink amount:

#if defined( NAVSCANW ) && !defined( NAVWNT )
    iShrinkAmount = rCoords2.top - rCoords1.top;
#else
    iShrinkAmount = rCoords2.bottom - rCoords1.top;
#endif

    pCoord1.x = rCoords2.left; pCoord1.y = rCoords2.top;
    ScreenToClient( hDlg, &pCoord1 );

#if defined( NAVSCANW ) && !defined( NAVWNT )
                                        // Move the advotext:
    SetWindowPos( GetDlgItem( hDlg, IDI_CS_MSG_ADVOTEXT ),
                  NULL,
                  pCoord1.x,
                  (pCoord1.y - iShrinkAmount),
                  0,0,
                  SWP_NOSIZE | SWP_NOZORDER );
#endif

                                        // Shrink the dialog vertically:
    if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT )
        {                               // In NT, center the dialog itself:
        GetWindowRect( hDlg, &rCoords1 );
        SetWindowPos( hDlg, NULL,
                      0, 0,
                      rCoords1.right-rCoords1.left,
                      (rCoords1.bottom-rCoords1.top) - iShrinkAmount,
                      SWP_NOMOVE | SWP_NOZORDER );
        }
    else
        {                               // In 95, center the parent of the dlg:
        GetWindowRect( GetParent( hDlg ), &rCoords1 );
        SetWindowPos( GetParent( hDlg ), NULL,
                      0, 0,
                      rCoords1.right-rCoords1.left,
                      (rCoords1.bottom-rCoords1.top)-iShrinkAmount,
                      SWP_NOMOVE | SWP_NOZORDER );
        }
} // End ShrinkDialog()


