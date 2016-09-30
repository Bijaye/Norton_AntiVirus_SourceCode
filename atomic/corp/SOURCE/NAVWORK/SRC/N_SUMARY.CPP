// Copyright 1995 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_sumary.cpv   1.12   02 Jul 1998 15:13:00   mdunn  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVWORK/VCS/n_sumary.cpv  $
// 
//    Rev 1.12   02 Jul 1998 15:13:00   mdunn
// fixed build 22 breakages on help IDs
// 
//    Rev 1.11   01 Jul 1998 14:11:10   DBuches
// Removed How-To help.
// 
//    Rev 1.10   26 Jun 1998 15:13:06   jtaylor
// Dissabled Quaranine items if Quarantine is not enabled.
//
//    Rev 1.9   07 Jun 1998 15:21:00   tcashin
// Tweaked MyHideMbrBrControls() for the new scanning dialogs. This
// caused the new dialogs to be totally screwed up on NEC machines.
//
//    Rev 1.8   18 May 1998 23:48:12   tcashin
// Set the new memory control as scanned or not scanned. Also, always
// update the repaired, quarantined, and deleted file count controls.
//
//    Rev 1.7   13 May 1998 09:59:30   tcashin
// Make sure szShort in FillSummaryBox is initialized.
//
//    Rev 1.6   12 May 1998 14:14:38   tcashin
// Fill in the new quarantined and deleted controls in the
// summary dialog.
//
//    Rev 1.5   18 Feb 1998 16:22:14   mdunn
// Ported NEC code from the ROSNEC area.
//
//    Rev 1.4   02 Sep 1997 21:48:18   DALLEE
// Moved call to toggle off shown state of custom message from
// ShowScanSumDlg() to NavWorkEnd() in n_scnstr.cpp -- the Scan Results
// dialog is not always shown, and the custom message was therfore not
// shown on subsequent scans.
//
//    Rev 1.3   21 Jul 1997 01:40:16   BGERHAR
// Removed hacked /NORESULTS handling (now using uProgramMode=PROGRAM_AUTOMATED)
//
//    Rev 1.2   18 Jul 1997 17:27:40   BGERHAR
// Slight change for /NORESULTS rework
//
//    Rev 1.1   15 Jul 1997 14:42:20   DBUCHES
// Added Ctl3d support for DEC Alpha
//
//    Rev 1.0   30 Jun 1997 16:31:12   KSACKIN
// Initial revision.
//
//    Rev 1.3   26 Jun 1997 16:43:10   TIVANOV
// check if the user is usiing the NORESULTS command line switch
//
//    Rev 1.2   24 Jun 1997 16:04:10   TIVANOV
// clear the LongCustomMsg status - SetShowMsg
//
//    Rev 1.1   22 May 1997 16:21:16   BILL
// Alpha changes (removed ctl3d)
//
//    Rev 1.0   06 Feb 1997 21:02:44   RFULLER
// Initial revision
//
//    Rev 1.6   27 Jan 1997 15:01:06   JBRENNA
// On NEC, hide the MBR / BR summary information at run-time.
//
//    Rev 1.5   12 Nov 1996 22:25:38   GWESTER
// Fixed string problem where scanning multiple drives would show up
// in the dialog as "C:\" .
//
//    Rev 1.4   09 Sep 1996 16:18:16   PVENKAT
// Fixed #67819.  Scan Results now displays with ellipses (...) for long names.
//
//    Rev 1.3   11 Jul 1996 08:03:40   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.2   28 Jun 1996 15:27:24   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.1   22 Feb 1996 14:08:04   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:55:30   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:34   BARRY
// Initial revision.
//
//    Rev 1.36   23 Jan 1996 11:21:00   YVANG
// Added #ifndef NAVSCANW #else swiches around DialogBoxParam(,IDD_SCANSUMDLG,,,).
// Also for #ifdef NAVSCANW case, changed to use different dialog templates
// under Win95 and NT.
//
//
//
//    Rev 1.35   14 Jan 1996 16:50:06   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.34   30 Nov 1995 22:42:04   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.33   21 Nov 1995 12:50:24   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//
//    Rev 1.32   06 Nov 1995 13:51:36   DALLEE
// For NAVWNT: removed memory and inoculation text/buttons from dialogs.
//
//    Rev 1.31   02 Nov 1995 16:35:42   JWORDEN
// Convert MasterBootRec elements from booleans to counters
//
//    Rev 1.30   03 Jul 1995 09:38:44   KEITH
// Taped on a header
//***************************************************************************

#include "platform.h"
#include "nunavdlg.h"
#include "ngui.h"
#include "nonav.h"
#include "xapi.h"
#include "disk.h"
#include "file.h"
#include "hstring.h"
#include <windowsx.h>
#include "npt.h"
	#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#ifndef SYM_DOS
    #include "n32behav.h" // Personality DLL
#endif

#include "stdhelp.h"
#include "symcfg.h"
#include "symintl.h"

#include "navutil.h"
#include "options.h"

#include "stub.h"
#include "navw_hlp.h"
#include "cmbstr.h"
#include "nunavstb.h"
#include "msgdisp.h"                    // symALogPrint()
#include "AlignWnd.h"
#include "navcomm.h"

extern HINSTANCE hInstance;

// ************************************************************************ //
// ****     Func.Prototypes which actially reside in VIRUSDBF.DLL      **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI PrintToFile (HWND, LPSTR, HGLOBAL);

// ************************************************************************ //
// ****                     Local Prototypes                           **** //
// ************************************************************************ //

#ifdef SYM_WIN32
VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the message
    UINT    uMsg,                       // [in]"message"
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
);
#endif

BOOL SYM_EXPORT WINAPI ScanSumDlgProc (HWND, UINT, WPARAM, LPARAM);
HGLOBAL SYM_EXPORT WINAPI BuildResultInfoForPrinting(LPPRINTDLGSTRUCT lpPrintDlgStruct);
static void MyHideMbrBrControls (HWND hDlg);

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI ShowScanSumDlg (
    HWND    hDlg,
    LPNONAV lpNav
    )
{
    extern  HINSTANCE   hInstance;
    auto    DLGPROC     lpfn;
    auto    int         nStatus;

    lpfn = (DLGPROC) MakeProcInstance (
				    (FARPROC)ScanSumDlgProc,
				    hInstance
				    );

    SYM_ASSERT ( lpfn );

#ifndef NAVSCANW
    nStatus = DialogBoxParam (
			    hInstance,
			    MAKEINTRESOURCE(IDD_SCANSUMDLG),
			    hDlg,
			    lpfn,
			    (LPARAM)lpNav
			    );
#else
    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
	nStatus = DialogBoxParam (
				hInstance,
				MAKEINTRESOURCE(IDD_SCANSUMDLG_NT),
				hDlg,
				lpfn,
				(LPARAM)lpNav
				);
    else
	nStatus = DialogBoxParam (
				hInstance,
				MAKEINTRESOURCE(IDD_SCANSUMDLG),
				hDlg,
				lpfn,
				(LPARAM)lpNav
				);
#endif

    SYM_ASSERT ( nStatus != -1 );

    FreeProcInstance((FARPROC) lpfn);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI ScanSumDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    BOOL            bReturn = FALSE;
    auto    PRINTDLGSTRUCT  printDlgStruct;
    auto    NOTIFYSTRUCT    Notify;
    auto    LPNONAV         lpNav;
    auto    char            szDefFile [ SYM_MAX_PATH+1 ];
    auto    char            szWork    [ SYM_MAX_PATH+1 ];

    switch(wMessage)
	{
	case WM_CTLCOLOR:
	    bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
	    break;
	case WM_INITDIALOG:
	    NAV_CenterDialog (hDlg);
	    Ctl3dSubclassDlg(hDlg, CTL3D_ALL);

				    // Hide the MBR / BR controls from the
				    // summary dialog when we are on NEC.
	    if (HWIsNEC())
	    {
		MyHideMbrBrControls (hDlg);
	    }

	    NG_SetFontsInDialog (hDlg, 0);
	    NG_SetChildSmallFont (hDlg, ID_SS_SCANSUM);
	    NG_SetChildSmallFont (hDlg, ID_SS_ITSCAN);
	    NG_SetChildSmallFont (hDlg, ID_SS_SCANTYP);
	    NG_SetChildSmallFont (hDlg, ID_SS_OTHER);
	    NG_SetChildSmallFont (hDlg, ID_SS_SCANTIM);
#ifndef NAVSCANW
	    NG_SetChildSmallFont (hDlg, ID_SS_INOC);
	    NG_SetChildSmallFont (hDlg, ID_SS_MEMSCAN);
	    NG_SetChildSmallFont (hDlg, ID_SS_MEMINF);
	    NG_SetChildSmallFont (hDlg, ID_SS_MEMCLN);
#endif
	    NG_SetChildSmallFont (hDlg, ID_SS_MBSCAN);
	    NG_SetChildSmallFont (hDlg, ID_SS_MBINF);
	    NG_SetChildSmallFont (hDlg, ID_SS_MBCLN);
	    NG_SetChildSmallFont (hDlg, ID_SS_BRSCAN);
	    NG_SetChildSmallFont (hDlg, ID_SS_BRINF);
	    NG_SetChildSmallFont (hDlg, ID_SS_BRCLN);
	    NG_SetChildSmallFont (hDlg, ID_SS_FILSCAN);
	    NG_SetChildSmallFont (hDlg, ID_SS_FILINF);
	    NG_SetChildSmallFont (hDlg, ID_SS_FILCLN);
	    NG_SetChildSmallFont (hDlg, ID_SS_FILQUAR);
	    NG_SetChildSmallFont (hDlg, ID_SS_FILDEL);
	    NG_SetGroupBoxSunken (hDlg, ID_SS_BOX1);
	    NG_SetGroupBoxSunken (hDlg, ID_SS_BOX2);
	    lpNav = (LPNONAV)lParam;

#ifndef SYM_DOS
        if( !NAV_IS_QUARANTINE() )
            {
            EnableWindow(GetDlgItem(hDlg, ID_SS_QUARTXT), FALSE);
            EnableWindow(GetDlgItem(hDlg, ID_SS_FILQUAR), FALSE);
            EnableWindow(GetDlgItem(hDlg, ID_SS_MBQUAR), FALSE);
            EnableWindow(GetDlgItem(hDlg, ID_SS_BRQUAR), FALSE);
            }
#endif

        FillSummaryBox(hDlg, lpNav);
	    LP_SET(hDlg, lpNav);

	    EnableWindow(GetDlgItem(hDlg, ID_SS_DETAILS),
			(lpNav->lpInfArr) ? TRUE : FALSE);

	    bReturn = TRUE;
	    break;

#ifdef SYM_WIN32
	case WM_HELP:
	case WM_CONTEXTMENU:
					// ----------------------------------
					// Bring up right click help menu,
					// or context sensitive help
					// ----------------------------------
	    OnContextHelp( hDlg, wMessage, wParam, lParam );
	    break;
#endif
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
		case IDOK:
		case IDCANCEL:
		    EndDialog(hDlg, 0);
		    break;

		case ID_SS_PRINT:
		    {
					// ---------------------------------
					// Build a full path to the default
					// print file.
					// ---------------------------------

		    GetStartDir(hInstance, szDefFile, sizeof(szDefFile) );

		    NameReturnLongName ( szDefFile, szWork, TRUE );
		    STRCPY ( szDefFile, szWork );

		    LoadString (hInstance, IDS_DEF_RESULTSPRINTFILE, szWork,
				sizeof(szWork));

		    NameAppendFile (szDefFile, szWork);

#ifdef SYM_WIN16
		    STRLWR(szDefFile);
#endif
		    LoadString (hInstance, IDS_PRINTSUMARY, szWork, sizeof(szWork));

					// ---------------------------------
					// Fill in Print Dlg Struct
					// ---------------------------------

		    MEMSET(&printDlgStruct, 0, sizeof(printDlgStruct));

		    printDlgStruct.dwSize   = sizeof(printDlgStruct);
		    printDlgStruct.hParent  = hDlg;
		    printDlgStruct.lpfnBuild= BuildResultInfoForPrinting;
		    printDlgStruct.wHelpID  = HELP_DLG_PRINT;
		    printDlgStruct.wHelpIDOverwrite = HELP_DLG_OVERWRITE;
		    printDlgStruct.wHelpIDBrowse    = HELP_DLG_PRINT_BROWSE;

		    STRCPY ( printDlgStruct.szTitle, szWork );
		    STRCPY ( printDlgStruct.szFile, szDefFile );

					// ---------------------------------
					// Set Printer dialog context Help
					// controls ids:
					// ---------------------------------
		    NavPrnDlgSetContextHelpIds( &printDlgStruct );


					// ---------------------------------
					// See how user wants the output
					// directed and do so.
					// ---------------------------------

		    PRINTERR Status = NPTPrintDialog (&printDlgStruct);

		    if ( Status != PRINTERR_OK && Status != PRINTERR_CANCEL )
			{
			MEMSET (&Notify, '\0', sizeof(Notify));
			Notify.hParent       = hDlg;
			Notify.uMsgID        = IDS_PRINTNOK;
			Notify.uTitleID      = IDS_NAV;
			if ( GetCustomVersion() == VERSION_NETSCAPE )
			    {           // Title is "Internet Scanner"; set at runtime.
			Notify.uTitleID      = IDS_NAV_INTERNET;
			    }
			Notify.lpFileName    = lpNav->Infected.szFullPath;
			Notify.lpVirusName   = lpNav->Infected.Notes.lpVirName;
			Notify.lpAlertMsg    = (lpNav->Alert.bDispAlertMsg) ?
						lpNav->Alert.szAlertMsg : NULL;
			Notify.uRemoveAfter  = (lpNav->Alert.bRemoveAfter) ?
						lpNav->Alert.uSeconds : NULL;
			Notify.hInstance     = hInstance;

			NotifyUser (&Notify);
			}
		    }
		break;

	    case ID_SS_DETAILS:
		ShowScanResDlg(hDlg, lpNav, TRUE);
		break;

	    case IDHELP:
		StdHelpFromDialog (
				hDlg,
				IDH_NAVW_SCAN_RESULTS_DLG_HELP_BTN
				);
		break;
	    }
	 break;

      case WM_DESTROY:
	 LP_FREE(hDlg);
	 break;
      }

    return ( bReturn );
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI FillSummaryBox(HWND hDlg, LPNONAV lpNav)
{
    auto    char    lpWork[SIZE_WORK + 1];
    auto    LPSTR   lpDrives;
    auto    HMS     Hms;
    auto    UINT    uStringID;
    auto    TCHAR   szShort[SYM_MAX_PATH + 1]={0};


    if ((lpNav->Stats.Files.uInfected && lpNav->Stats.BootRecs.uInfected) ||
	(lpNav->Stats.Files.uInfected && lpNav->Stats.MasterBootRec.uInfected))
	uStringID = IDS_FILESANDBOOTINFECTEDFOUND;
    else if (lpNav->Stats.BootRecs.uInfected ||
	     lpNav->Stats.MasterBootRec.uInfected)
	uStringID = IDS_BOOTINFECTEDFOUND;
    else if (lpNav->Stats.Files.uInfected)
	uStringID = IDS_INFECTEDFOUND;
    else
	uStringID = IDS_NOINFECTEDFOUND;

    Static_SetText(GetDlgItem(hDlg, ID_SS_SCANSUM), String_Ptr(uStringID));

    // ****************************** //
    // Was memory scanned?            //
    // ****************************** //

    if (SYM_SYSTEM_WIN_NT != SystemGetWindowsType())
	{
	if (lpNav->Opt.bMemory)
	    Static_SetText(GetDlgItem(hDlg, ID_SS_MEMSCAN), String_Ptr(IDS_MEMSCANNED));
	else
	    Static_SetText(GetDlgItem(hDlg, ID_SS_MEMSCAN), String_Ptr(IDS_MEMNOTSCANNED));
	}

    // ****************************** //
    // Display all the drives scanned //
    // ****************************** //

    if ((lpDrives = (LPSTR)MemLock(lpNav->lpScan->hScanList)) != NULL)
	{
	MEMSET (lpWork, EOS, SIZE_WORK+1);
	BuildWhatScanned (lpDrives, lpWork, SIZE_WORK+1);

	// Display only 20 chars.  If grater than 20, then display it
	// with ... .  This is only for display and we donot want this
	// to go to activity log.
	//
	NameShortenFileName (lpWork, szShort, 20);
	Static_SetText(GetDlgItem(hDlg, ID_SS_ITSCAN), szShort);
	MemUnlock(lpNav->lpScan->hScanList);
	}

    // ********************** //
    // Type of scan performed //
    // ********************** //

    Static_SetText(GetDlgItem(hDlg, ID_SS_SCANTYP),
		String_Ptr((lpNav->Opt.bScanAll) ? IDS_ALLFILES : IDS_PROGFILESONLY));

					// -----------------------------------
					// If Inoculating Directories
					// -----------------------------------
#ifndef NAVSCANW  // No Inoc in NT.
    if ( *lpNav->lpScan->szScanFileSpec )
	{
	Static_SetText ( GetDlgItem ( hDlg, ID_SS_INOC),
			 String_Ptr(IDS_ENABLED)
			);
	}
    else
	{
	Static_SetText ( GetDlgItem ( hDlg, ID_SS_INOC ),
			 String_Ptr((lpNav->Inoc.bUseInoc) ? IDS_ENABLED :
							     IDS_DISABLED));
	}
#endif
					// -----------------------------------
					// Other Settings, If ZIP scanning is
					// enabled but scanning started from
					// tools inoculation. than we haven't
					// touched any ZIP files so indicate
					// that ZIPs were now scanned.
					// -----------------------------------

    if ( lpNav->Gen.bScanZipFiles)
	{
	if ( lpNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY )
	    {
	    Static_SetText(GetDlgItem(hDlg, ID_SS_OTHER),
			   String_Ptr (IDS_NOZIPS));
	    }
	else
	    {
	    Static_SetText(GetDlgItem(hDlg, ID_SS_OTHER),
			   String_Ptr (IDS_ZIPS));
	    }
	}
    else
	{
	Static_SetText(GetDlgItem(hDlg, ID_SS_OTHER), String_Ptr (IDS_NOZIPS));
	}

    // ******************************* //
    // Display effective scanning time //
    // ******************************* //
					// Don't want 0 seconds for scanning
    if ( lpNav->Stats.lTimeScan == 0 )
	lpNav->Stats.lTimeScan++;

    HMS_Time(lpNav->Stats.lTimeScan, &Hms);

    NIntlTimeToLongString(lpWork, Hms.uHour, Hms.uMinute, Hms.uSecond);

    Static_SetText(GetDlgItem(hDlg, ID_SS_SCANTIM), lpWork);

    // ****************************** //
    // Display statistics information //
    // ****************************** //
    ScanRes_ScanMemBootStat (hDlg, lpNav);

    ConvertLongToString (lpNav->Stats.Files.dwScanned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_FILSCAN), lpWork);

    ConvertWordToString (lpNav->Stats.Files.uInfected, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_FILINF), lpWork);

    ConvertWordToString(lpNav->Stats.Files.uCleaned, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_FILCLN), lpWork);

    ConvertWordToString(lpNav->Stats.Files.uQuarantined, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_FILQUAR), lpWork);

    ConvertWordToString(lpNav->Stats.Files.uDeleted, lpWork);
    Static_SetText(GetDlgItem(hDlg, ID_SS_FILDEL), lpWork);

}

//********************************************************************
//********************************************************************
HGLOBAL SYM_EXPORT WINAPI BuildResultInfoForPrinting(LPPRINTDLGSTRUCT lpPrintDlgStruct)
{
    auto        HWND            hWnd;
    auto        LPNONAV         lpNav;
    auto        HGLOBAL         hBuffer;


    hWnd = lpPrintDlgStruct->hParent;
    lpNav = (LPNONAV)LP_GET(hWnd);
#ifdef NAVSCANW                           // Verify lpNav if this is NT
    if (!lpNav)
	{LP_SET (hWnd, GloballySaved_lpNav);
	 lpNav = (LPNONAV)LP_GET(hWnd);
	 SYM_ASSERT (lpNav);}
#endif
    hBuffer = BuildResultInfo(hWnd, lpNav);
    return(hBuffer);
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
	    HWND    hDlg,
	    WPARAM  wParam,
	    LPARAM  lParam
	    )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU ad WM_HELP
windows message. The purpose of this function is to call the standard symantec
help routines that display an appropiate floating help menu or standard help.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */
VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the message
    UINT    uMsg,                       // [in]"message"
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
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
//&^	IDD_SCANSUMDLG,     IDH_NAVW_WHATHIS_SCAN_RESULTS_DLG,
	IDOK,               IDH_NAVW_WHATHIS_SCAN_RESULTS_CLOSE_BTN,
	ID_SS_PRINT,        IDH_NAVW_WHATHIS_SCAN_RESULTS_PRINT_BTN,
	ID_SS_DETAILS,      IDH_NAVW_WHATHIS_SCAN_RESULTS_DETAILS_BTN,
	ID_SS_SUMM_GRPBOX,  IDH_NAVW_WHATHIS_SCAN_RESULTS_SUMMARY_GRPBOX,
	ID_SS_ITEM_GRPBOX,  IDH_NAVW_WHATHIS_SCAN_RESULTS_ITEM_GRPBOX,
	0,              0
	};


    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling     */
		       hDlg,
		       uMsg,
		       wParam,
		       lParam,
		       dwWhatsThisMap,
		       NULL, 
		       NULL,            /* This dlg. does not have any LB Ctrls -  */
		       NULL             /* that's why these nulls                  */
		       );
}
#endif

//***************************************************************************
// Description:
//   Hide the MBR and BR summary controls from the summary dialog. Also, move
//   the File information up to just below the Memory information.
//
// Parameters:
//   hDlg   [in] Summary dialog to rearrange.
//***************************************************************************
// 2/17/98 Mike Dunn -- Ported NEC code from ROSNEC, so everything comes
//                      out looking good on NECs.
//***************************************************************************

static void MyHideMbrBrControls (HWND hDlg)
{
    const  UINT uPadding      = 50 /*pixels*/;
    const  UINT uColPadding   = 65 /*pixels*/;
    auto   HWND hMBRLabel     = GetDlgItem (hDlg, ID_SS_MBRECTXT);
    auto   HWND hBrLabel      = GetDlgItem (hDlg, ID_SS_BRTXT);
    auto   HWND hFileLabel    = GetDlgItem (hDlg, ID_SS_FILESTXT);

    auto   HWND hFileScanned  = GetDlgItem (hDlg, ID_SS_FILSCAN);
    auto   HWND hFileRepaired = GetDlgItem (hDlg, ID_SS_FILCLN);
    auto   HWND hFileInfected = GetDlgItem (hDlg, ID_SS_FILINF);
    auto   HWND hFileQuar     = GetDlgItem (hDlg, ID_SS_FILQUAR);
    auto   HWND hFileDel      = GetDlgItem (hDlg, ID_SS_FILDEL);

    // Move the "Boot Record:" information left.

    AlignWnd (GetDlgItem (hDlg, ID_SS_BRTXT), hFileLabel,
	      gAlignWndFollowX, hFileLabel, uPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SS_BRSCAN), hFileScanned,
	      gAlignWndFollowX, hFileScanned, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SS_BRINF), hFileInfected,
	      gAlignWndFollowX, hFileInfected, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SS_BRCLN), hFileRepaired,
	      gAlignWndFollowX, hFileRepaired, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SS_BRQUAR), hFileQuar,
	      gAlignWndFollowX, hFileQuar, uColPadding);

    AlignWnd (GetDlgItem (hDlg, ID_SS_BRDEL), hFileDel,
	      gAlignWndFollowX, hFileDel, uColPadding);

    // Hide the MBR infomation

    ShowWindow (GetDlgItem (hDlg, ID_SS_MBRECTXT), SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SS_MBSCAN),   SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SS_MBINF),    SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SS_MBCLN),    SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SS_MBDEL),    SW_HIDE);
    ShowWindow (GetDlgItem (hDlg, ID_SS_MBQUAR),   SW_HIDE);
}

