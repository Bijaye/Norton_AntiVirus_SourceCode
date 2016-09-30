// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/scanfnd.c_v   1.0   06 Feb 1997 21:08:52   RFULLER  $
//
// Description:
//      These are the functions for the Problems Found scan results dialog.
//
// Contains:
//      ScanFoundDialog()
//      ScanResultsDialog()
//      ScanFoundListWrite()
//
// See Also:
//      SCANFND.STR for strings and control structures.
//************************************************************************
// $Log:   S:/scand/VCS/scanfnd.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:52   RFULLER
// Initial revision
// 
//    Rev 1.2   30 Sep 1996 15:59:34   JALLEE
// DBCS work.
// 
//    Rev 1.1   08 Aug 1996 13:25:44   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.103   18 Jul 1996 18:26:06   MZAREMB
// Formatting strings were expecting a string and not a number for a few 
// segments of the printing routine. Now fixed.
// 
//    Rev 1.102   01 Jul 1996 15:00:48   JALLEE
// Fixed zip names in problems found dialog.
// 
//    Rev 1.101   13 Jun 1996 14:04:34   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.100   17 May 1996 12:13:46   MZAREMB
// Display dashes instead of numbers for "cleaned" fields to look like
// Windows version.
// 
//    Rev 1.99   06 May 1996 09:07:08   MZAREMB
// Initial Pegasus update: added NAVSCAN #ifdefs.
// 
//    Rev 1.98   11 Mar 1996 18:15:40   MKEATIN
// When passing stats.files.dwscanned to PRPrintFrmtCRLFLine() - you must use
// its address and not its value since it is a long integer and all the quake
// 'printf' type functions take the ADDRESSES of long integers for some reason.
// 
// 
//    Rev 1.97   02 Nov 1995 17:03:42   JWORDEN
// Convert MasterBootRec booleans to counters to allow for multiple MBRs
// 
//    Rev 1.96   01 Nov 1995 10:10:06   JWORDEN
// Fix drive display in update message box when referring to MBR
// 
//    Rev 1.95   24 Oct 1995 20:38:16   JWORDEN
// Process filename for MBR as a single digit drive number
// 
//    Rev 1.94   06 Jun 1995 21:17:16   BARRY
// Removed Other Settings (no ZIP support anymore)
// 
//    Rev 1.93   09 May 1995 16:59:22   DALLEE
// Added CheckJustInoc() function to check for just inoculation
// messages for startup and auto-inoc features.
// -CGRANGE
// 
// 
//    Rev 1.92   17 Apr 1995 21:43:38   DALLEE
// Fix bug - scan results, never show "Compressed files scanned."
// 
//    Rev 1.91   17 Apr 1995 16:20:18   DALLEE
// Added comment for MODULE_NAME;  Mostly to demonstrate build system CHKIN.
// 
//    Rev 1.90   22 Mar 1995 15:39:04   DALLEE
// Pop up Scan Results screen if in startup mode and anything happened
// during the scan.
// 
//    Rev 1.89   21 Mar 1995 14:11:50   DALLEE
// Commented out some unused zip code for NAVBOOT.
// 
//    Rev 1.88   14 Mar 1995 17:46:02   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.87   29 Dec 1994 16:46:10   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.86   28 Dec 1994 14:16:02   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.3   12 Dec 1994 18:11:00   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.2   12 Dec 1994 16:16:52   DALLEE
    // Changed SPRINTF's to FastStringPrints() so format strings wouldn't have
    // to be modified (also preserves %p functionality).
    //
    //    Rev 1.1   05 Dec 1994 18:10:18   DALLEE
    // CVT1 script.
    //
//    Rev 1.85   28 Dec 1994 13:53:38   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:26   DALLEE
    // Initial revision.
    //
    //    Rev 1.81   04 Nov 1993 15:52:14   DALLEE
    // Allow DIR2 repairs once more for DOS scanner.  Repairs are disabled for
    // Windows since the special repair involves low-level tweaking with
    // directory entries and FATs.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "symintl.h"
#include "stddlg.h"
#include "file.h"
#include "dbcs.h"

#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"

#include "navdprot.h"
#include "scand.h"

#ifdef SYM_DOS
 #include <stdio.h>
#endif

// For VMM debugging.
MODULE_NAME;

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define SCAN_DETAILS_CLOSE_BUTTON   ( 0)
#define SCAN_DETAILS_INFO_BUTTON    ( 1)
#define SCAN_DETAILS_HELP_BUTTON    ( 2)

#define SCAN_FOUND_DONE_BUTTON      ( 0)
#define SCAN_FOUND_REPAIR_BUTTON    ( 1)
#define SCAN_FOUND_DELETE_BUTTON    ( 2)
#define SCAN_FOUND_INOCULATE_BUTTON ( 3)
#define SCAN_FOUND_EXCLUDE_BUTTON   ( 4)
#define SCAN_FOUND_INFO_BUTTON      ( 5)
#define SCAN_FOUND_HELP_BUTTON      ( 6)

#define SCAN_RESULTS_CLOSE_BUTTON       0
#define SCAN_RESULTS_PRINT_BUTTON       1
#define SCAN_RESULTS_DETAILS_BUTTON     2
#define SCAN_RESULTS_HELP_BUTTON        3


//************************************************************************
// STATIC VARIABLES
//************************************************************************

STATIC    char            *szSummaryLines;
STATIC    char            *szItems;
STATIC    char            *szType;
STATIC    char            *szInoculation;
STATIC    char            szTime [40];
STATIC    char            *szMemScanned;
BOOL    bInFoundDlg;                    // Either in FOUND or DETAILS


//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern ListRec          lrScanFoundItems;
extern OutputFieldRec  *lpofScanFoundMessage [];
extern LPNONAV          glpNoNav;
extern BOOL             bStartupMode;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL ScanFoundUpdateMessageBox (LPNONAV lpNoNav);
VOID LOCAL PASCAL ScanFoundDisableButtons   (LPNONAV lpNoNav);
VOID LOCAL PASCAL ScanDetailsDisableButtons (LPNONAV lpNoNav);
VOID LOCAL PASCAL UpdateInfoDialog          (DWORD dwEntry);
WORD LOCAL PASCAL ScanDetailsDialog         (LPNONAV lpNoNav);
WORD LOCAL PASCAL ScanDetailsButtonsProc    (DIALOG_RECORD *lpdrDialog);

WORD LOCAL PASCAL ScanFoundButtonsProc      (DIALOG_RECORD *lpdrDialog);
UINT LOCAL PASCAL ScanResultsButtonsProc    (DIALOG_RECORD *lpdrDialog);
VOID LOCAL PASCAL ScanPrintResults(VOID);
VOID LOCAL PASCAL BuildCustomMessage(LPNONAV lpNoNav);

BOOL LOCAL PASCAL CheckJustInoc(void);
BOOL LOCAL PASCAL PrintDashedLines(HFILE * hFile);
BOOL LOCAL PASCAL PrintResultInfected(HFILE * hFile);



//************************************************************************
// BuildCustomMessage
//
//
// Parameters:
//      None
//
// Returns:
//      None
//************************************************************************
//************************************************************************

VOID LOCAL PASCAL BuildCustomMessage(LPNONAV lpNoNav)
{
    extern      char            *lpszScanFoundStrings[];
    extern      char            SZ_CUSTOM_MESSAGE[];
    extern      char            DIALOG_GROUP_END[];

    auto        UINT            i;


    
    for (i = 0; lpszScanFoundStrings[i] != DIALOG_END; i++)
        {                               // We search for this entry to que off
        if ( lpszScanFoundStrings[i] == DIALOG_SKIP_LINE )
            {
            lpszScanFoundStrings[++i] = (lpNoNav->Alert.bDispAlertMsg && 
                                        lpNoNav->Alert.szAlertMsg[0]) ? 
                                        SZ_CUSTOM_MESSAGE : DIALOG_SKIP_LINE;
            break;
            }
        }
}


//************************************************************************
// CheckJustInoc
//
// Description - This fuction determines if  
//               only inoculation messages were reported
//               Used for the startup and auto-inoc features
// Parameters:
//      None
//
// Returns:
//      BOOL - TRUE  - if only inoculation messages
//             FALSE - otherwise
//************************************************************************
//CGRANGE - 5-8-95
//************************************************************************

BOOL LOCAL PASCAL CheckJustInoc()
{

    auto        INFECTEDFILE    rInfected;
    auto        DWORD           dwEntry;
        

    for (dwEntry = 0;;dwEntry++)
        {
        if (!GetInfectedFile (&rInfected, glpNoNav, dwEntry))
	    break;
        if ( rInfected.wStatus != FILESTATUS_INOCULATED )
            return(FALSE);
        }
    return(TRUE);
}


//************************************************************************
// ScanFoundDialog()
//
// This routine displays the Problems Found dialog with the list
// of infected objects, messages about the active object, and buttons
// to delete, repair, exclude, etc.
//
// Parameters:
//      None
//
// Returns:
//      ACCEPT_DIALOG
//      ABORT_DIALOG
//************************************************************************
// 3/2/93 DALLEE Function created.
//************************************************************************

WORD PASCAL ScanFoundDialog (LPNONAV lpNoNav)
{
    extern  DIALOG          dlScanFound;
    extern  ButtonsRec      buScanFoundButtons;
    extern  BOOL            bScreenStarted;
    BOOL            bJustInoc;


    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult = ABORT_DIALOG;

    auto    WORD            wNumEntries = 0;
    auto    LPINFARR        lpInfArr;

                                        // Setup the list information.
    lpInfArr = lpNoNav->lpInfArr;
    while (lpInfArr)
        {
        wNumEntries += lpInfArr->uNumUsed;
        lpInfArr = (LPINFARR) lpInfArr->lpNext;
        }
    lrScanFoundItems.num     = wNumEntries;
    lrScanFoundItems.entries = lpNoNav;


    bJustInoc=CheckJustInoc();
    if (bJustInoc && bStartupMode)
        return(ACCEPT_DIALOG);
    else if (!bScreenStarted)
            {
            InitDisplay(TRUE);              // Force screen init if hidden.
            }

    BuildCustomMessage(lpNoNav);

    bInFoundDlg = TRUE;
    lpdrDialog = DialogOpen2( &dlScanFound,
                              &lrScanFoundItems,
                              lpofScanFoundMessage[0],
                              lpofScanFoundMessage[1],
                              lpofScanFoundMessage[2],
                              lpofScanFoundMessage[3],
                              &buScanFoundButtons,
                              lpNoNav->Alert.szAlertMsg);

    if (NULL != lpdrDialog)
        {
        do
            {
            HyperHelpTopicSet(HELP_DLG_SCAN_RESULTS);

            wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

            if (wResult == ACCEPT_DIALOG)
                {
                wResult = ScanFoundButtonsProc(lpdrDialog);
                }
            }
            while ( (wResult != ABORT_DIALOG) && (wResult != ACCEPT_DIALOG) );

        DialogClose(lpdrDialog, wResult);
        }            

    return (wResult);
} // ScanFoundDialog()


//************************************************************************
// ScanFoundButtonsProc()
//
// This routine handles button presses for the Problems Found
// dialog.
//
// Parameters:
//      DIALOG_RECORD *drDialog
//
// Returns:
//      EVENT_AVAIL                     further processing of event
//      ACCEPT_DIALOG                   Done hit
//************************************************************************
// 3/2/93 DALLEE Function created.
//************************************************************************

WORD LOCAL PASCAL ScanFoundButtonsProc(DIALOG_RECORD *lpdrDialog)
{
    extern      ButtonsRec      buScanFoundButtons;

    auto        BYTE            byValue;
    auto        DIALOG_ITEM     *dItem;
    auto        BOOL            bFromList;


    byValue = buScanFoundButtons.value;

    bFromList = FALSE;
    dItem = DialogGetFocus(lpdrDialog);
    if ( dItem->item == &lrScanFoundItems )
        {
        if ( ButtonIsDisabled(&buScanFoundButtons, SCAN_FOUND_INFO_BUTTON) )
            {
            byValue = 0xFF;
            }
        else
            {
            byValue = SCAN_FOUND_INFO_BUTTON;
            bFromList = TRUE;
            }
        }

    switch (byValue)                    // Current button selected
        {
        case SCAN_FOUND_DONE_BUTTON:
            return (ACCEPT_DIALOG);

#ifndef NAVSCAN
        case SCAN_FOUND_REPAIR_BUTTON:
            ScanFoundRepairDialog();
            break;

        case SCAN_FOUND_DELETE_BUTTON:
            ScanFoundDeleteDialog();
            break;

        case SCAN_FOUND_INOCULATE_BUTTON:
            ScanFoundInocDialog();
            break;

        case SCAN_FOUND_EXCLUDE_BUTTON:
#ifdef USE_PASSWORD    //&?
            if (AskForPassword(PASSWORD_MODIEXCLUSIONS))
#endif
                {
                ScanFoundExcludeDialog();
                }
            break;
#endif

        case SCAN_FOUND_INFO_BUTTON:
            ScanInfoDialog(FALSE);
            break;

        case SCAN_FOUND_HELP_BUTTON:
	    HyperHelpDisplay();
            break;

        default:
            break;
        } // End switch (lpButtons->value)

                                        // Rewrite the list to show the new
                                        // status fields.
    if ( !bFromList )
        ListWrite(&lrScanFoundItems, TRUE, FALSE);
    ScanFoundDisableButtons(glpNoNav);
    ScanFoundUpdateMessageBox(glpNoNav);

    return (EVENT_AVAIL);               // default return value

} // End ScanFoundButtonsProc()


//************************************************************************
// ScanDetailsDialog()
//
// This routine displays the Scan Details dialog with the list
// of infected objects, and  messages about the active object.
//
// Parameters:
//      None
//
// Returns:
//      ACCEPT_DIALOG
//      ABORT_DIALOG
//************************************************************************
// 4/5/93 DALLEE Function created.
//************************************************************************

WORD LOCAL PASCAL ScanDetailsDialog (LPNONAV lpNoNav)
{
    extern  DIALOG          dlScanDetails;
    extern  ButtonsRec      buScanDetailsButtons;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult;
    auto    WORD            wOldHelp;

    auto    WORD            wNumEntries = 0;
    auto    LPINFARR        lpInfArr;
                                        // Setup the list information.
                                        //*** DALLEE, fix this kludge with
                                        // a function call, or store the
                                        // information as you go.
    lpInfArr = lpNoNav->lpInfArr;
    while (lpInfArr)
        {
        wNumEntries += lpInfArr->uNumUsed;
        lpInfArr = (LPINFARR) lpInfArr->lpNext;
        }
    lrScanFoundItems.num     = wNumEntries;
    lrScanFoundItems.entries = lpNoNav;

    BuildCustomMessage(lpNoNav);
                                        //*** DALLEE, end of kludge
    bInFoundDlg = FALSE;
    lpdrDialog = DialogOpen2( &dlScanDetails,
                              &lrScanFoundItems,
                              lpofScanFoundMessage[0],
                              lpofScanFoundMessage[1],
                              lpofScanFoundMessage[2],
                              lpofScanFoundMessage[3],
                              &buScanDetailsButtons,
                              lpNoNav->Alert.szAlertMsg);

    if (NULL != lpdrDialog)
        {
        wOldHelp = HyperHelpTopicSet(HELP_DLG_SCAN_RESULTS);

        do
            {

            wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

            if (wResult == ACCEPT_DIALOG)
                {
                wResult = ScanDetailsButtonsProc(lpdrDialog);
                }
            }
            while ((wResult != ABORT_DIALOG) && (wResult != ACCEPT_DIALOG));

        DialogClose(lpdrDialog, wResult);
        HyperHelpTopicSet(wOldHelp);
        }

    return (wResult);
} // End ScanDetailsDialog()


//************************************************************************
// ScanDetailsButtonsProc()
//
// This routine handles button presses for the Scan Results Details
// dialog.
//
// Parameters:
//      DIALOG_RECORD *drDialog
//
// Returns:
//      EVENT_AVAIL                     further processing of event
//      ACCEPT_DIALOG                   Done hit
//************************************************************************
// 4/5/93 DALLEE Function created.
// 6/29/93 DALLEE, changed to LOCAL PASCAL type and removed unecessary code.
//************************************************************************

WORD LOCAL PASCAL ScanDetailsButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    extern      ButtonsRec      buScanDetailsButtons;

    auto        BYTE            byValue;
    auto        DIALOG_ITEM     *dItem;

    byValue = buScanDetailsButtons.value;

    dItem = DialogGetFocus(lpdrDialog);
    if ( dItem->item == &lrScanFoundItems )
        {
        byValue = (ButtonIsDisabled(&buScanDetailsButtons, SCAN_DETAILS_INFO_BUTTON)) ?
                        0xFF : SCAN_DETAILS_INFO_BUTTON;
        }

    switch (byValue)                    // Current button selected
        {
        case SCAN_DETAILS_CLOSE_BUTTON:
            return (ACCEPT_DIALOG);

        case SCAN_DETAILS_INFO_BUTTON:
            ScanInfoDialog(FALSE);
            break;

        case SCAN_DETAILS_HELP_BUTTON:
            HyperHelpDisplay();
            break;

        default:
            break;
        } // End switch (lpButtons->value)


    return (EVENT_AVAIL);           // default return value

} // End ScanDetailsButtonsProc()


//************************************************************************
// ScanDetailsDisableButtons()
//
// This routine disables the Details Info button based on the status
// of the current list entry.
//
// Parameters:
//      LPNONAV     lpNoNav             Has NAV options information.
//
// Returns:
//      Nothing
//************************************************************************
// 6/29/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL ScanDetailsDisableButtons (LPNONAV lpNoNav)
{
    extern  FILESTATUSSTRUCT    FileStatus[];
    extern  ButtonsRec          buScanDetailsButtons;
    auto    INFECTEDFILE        rInfected;

                                        // Enable the Info button only for
                                        // the correct FILESTATUSes
    GetInfectedFile(&rInfected, lpNoNav, lrScanFoundItems.active_entry);

    ButtonEnable( &buScanDetailsButtons,
                  SCAN_DETAILS_INFO_BUTTON,
                  FileStatus[rInfected.wStatus].byHasVirusInfo);

} // End ScanDetailsDisableButtons()


//************************************************************************
// ScanResultsDialog()
//
// This routine displays the Scan Results dialog.
//
// Parameters:
//      LPNONAV     lpNav               This scan's info.
//
// Returns:
//      Nothing
//************************************************************************
// 3/9/93 DALLEE Function created.
//************************************************************************

VOID PASCAL ScanResultsDialog (LPNONAV lpNoNav)
{
    extern  char            SZ_EMPTY                [];
    extern  char            SZ_FOUND_NONE           [];
    extern  char            SZ_FOUND_FILES          [];
    extern  char            SZ_FOUND_BOOT           [];
    extern  char            SZ_FOUND_FILES_BOOT     [];
    extern  char            SZ_ALL_FILES            [];
    extern  char            SZ_PROGRAM_FILES        [];
    extern  char            SZ_ENABLED              [];
    extern  char            SZ_DISABLED             [];
    extern  char            SZ_YES                  [];
    extern  char            SZ_NO                   [];
    extern  char            SZ_DASH                 [];
    extern  ButtonsRec      buScanResults;
    extern  DIALOG          dlScanResults;
    extern  char            szSelectedDrives        [];

    extern  BOOL            bScreenStarted;
    BOOL            bJustInoc;


    auto    DIALOG_RECORD   *lpdrDialog;
    auto    UINT            uResult;
    auto    LPSTR           lpScanList;
    auto    HMS             hms;
    auto    char            szItemsScanned [SYM_MAX_PATH];

    auto    WORD            wNumEntries = 0;
    auto    LPINFARR        lpInfArr;
    auto    BYTE            szTempNum[4][16];


    //listinfo
   lpInfArr = lpNoNav->lpInfArr;
   while (lpInfArr)
       {
       wNumEntries += lpInfArr->uNumUsed;
       lpInfArr = (LPINFARR) lpInfArr->lpNext;
       }
   lrScanFoundItems.num     = wNumEntries;
   lrScanFoundItems.entries = lpNoNav;

   BuildCustomMessage(lpNoNav);


   bJustInoc=CheckJustInoc();
   if (!(bJustInoc && bStartupMode))
    {
            InitDisplay(TRUE);              // Force screen init if hidden.
            


                                        //-----------------------
                                        // Select Summary String.
                                        //-----------------------
    if ( lpNoNav->Stats.MasterBootRec.uInfected ||
         lpNoNav->Stats.BootRecs.uInfected )
        {
        szSummaryLines = (lpNoNav->Stats.Files.uInfected) ?
                        SZ_FOUND_FILES_BOOT : SZ_FOUND_BOOT;
        }
    else                                // No boot records infected.  Files?
        {
        szSummaryLines = (lpNoNav->Stats.Files.uInfected) ? 
                        SZ_FOUND_FILES : SZ_FOUND_NONE;
        }

                                        //---------------------------------
                                        // Format the Items Scanned string.
                                        //---------------------------------
    if (NULL != (lpScanList = MemLock(lpNoNav->lpScan->hScanList)))
        {
        BuildWhatScanned(lpScanList, szItemsScanned, sizeof(szItemsScanned) -1);
        MemUnlock(lpNoNav->lpScan->hScanList, lpScanList);
        }
    else
        {
        szItemsScanned[0] = EOS;
        }
    szItems      = szItemsScanned;

                                        //---------------------------------
                                        // Set all the information strings.
                                        //---------------------------------
    szType       = lpNoNav->Opt.bScanAll      ? SZ_ALL_FILES : SZ_PROGRAM_FILES;
    szInoculation= lpNoNav->Inoc.bUseInoc     ? SZ_ENABLED   : SZ_DISABLED;
    szMemScanned = lpNoNav->Opt.bMemory       ? SZ_YES       : SZ_NO;


                                        // Break seconds down to hour,min,sec

                                        // Added to existing Kluge
                                        // Check for negative values.
                                        // This appears to be happening during
                                        // zip scanning occasionally.
                                        // jallee 8/15/96

    if ( lpNoNav->Stats.lTimeScan <= 0 )
        lpNoNav->Stats.lTimeScan=1;
    if  ( lpNoNav->Stats.lTimeScan > 1000000 )   // too many days, must be a
        lpNoNav->Stats.lTimeScan = 1;           //   subtraction overflow.
    HMS_Time(lpNoNav->Stats.lTimeScan, &hms);
    NIntlTimeToLongString(szTime, hms.uHour, hms.uMinute, hms.uSecond);

    if ( lpNoNav->Stats.MasterBootRec.uCleaned)
        _ConvertLongToString(lpNoNav->Stats.MasterBootRec.uCleaned, szTempNum[0], 10, 0);
    else
        STRCPY(szTempNum[0], SZ_DASH);

    if ( lpNoNav->Stats.BootRecs.uCleaned)
        _ConvertLongToString(lpNoNav->Stats.BootRecs.uCleaned, szTempNum[1], 10, 0);
    else
        STRCPY(szTempNum[1], SZ_DASH);

    if ( lpNoNav->Stats.Files.uCleaned)
        _ConvertLongToString(lpNoNav->Stats.Files.uCleaned, szTempNum[2], 10, 0);
    else
        STRCPY(szTempNum[2], SZ_DASH);

    lpdrDialog = DialogOpen2( &dlScanResults,
                              szSummaryLines,
                              szItemsScanned,
                              szType,
                              szInoculation,
                              szTime,
                              szMemScanned,
                              SZ_NO,
                              SZ_DASH,
                              lpNoNav->Stats.MasterBootRec.uScanned,
                              lpNoNav->Stats.MasterBootRec.uInfected,
                              (LPVOID)szTempNum[0],
                              lpNoNav->Stats.BootRecs.uScanned,
                              lpNoNav->Stats.BootRecs.uInfected,
                              (LPVOID)szTempNum[1],
                              &lpNoNav->Stats.Files.dwScanned,
                              lpNoNav->Stats.Files.uInfected,
                              (LPVOID)szTempNum[2]);

    ButtonEnable(&buScanResults,SCAN_RESULTS_DETAILS_BUTTON, 
       (BYTE)(lpNoNav->lpInfArr && lpNoNav->lpInfArr->uNumUsed ? TRUE : FALSE));

    do
        {
        HyperHelpTopicSet(HELP_DLG_SCAN_SUMMARY);

        uResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

        if (uResult == ACCEPT_DIALOG)
            {
            uResult = ScanResultsButtonsProc(lpdrDialog);
            }
        } while ((uResult != ACCEPT_DIALOG) && (uResult != ABORT_DIALOG));

    DialogClose(lpdrDialog, uResult);
    }
} // End ScanResultsDialog()


//************************************************************************
// ScanResultsButtonsProc()
//
// This routine handles the button presses in the Scan Results dialog.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//
// Returns:
//      ACCEPT_DIALOG                   Close pressed.
//      EVENT_AVAIL                     Print, Details, Help, or no button.
//************************************************************************
// 3/9/93 DALLEE Function created
//************************************************************************

UINT LOCAL PASCAL ScanResultsButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    extern  LPNONAV     glpNoNav;
    auto    BYTE        byValue;


    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)                    // Current button selected
        {
        case SCAN_RESULTS_CLOSE_BUTTON:
            return (ACCEPT_DIALOG);

        case SCAN_RESULTS_PRINT_BUTTON:
	    ScanPrintResults();
            break;

        case SCAN_RESULTS_DETAILS_BUTTON:
            ScanDetailsDialog(glpNoNav);
            break;

        case SCAN_RESULTS_HELP_BUTTON:
	    HyperHelpDisplay();
            break;
        } // End switch (lpButtons->value)

    return (EVENT_AVAIL);

} // End ExcludeButtonsProc()

/**************************************************************************
 * ScanPrintResults
 *
 * DESCRIPTION  : Print the results of the scan.
 *
 * IN      : none
 *
 * RETURN VALUES: none
 *************************************************************************
 * SKURTZ : 05-21-93 at 11:36:52am Function created
 ************************************************************************* */
VOID LOCAL PASCAL ScanPrintResults(VOID)
{

    extern	char  *		SZ_PRINTINGERROR[];
    extern      char  		SZ_YES[];
    extern      char  		SZ_NO[];
    extern      char        SZ_DASH[];
    extern      char 		SZ_SCAN_RESULTS_TITLE[];
    extern      char            SZ_SCAN_FILENAME[];
    extern	DIALOG 		dlgPrintStatus;
    extern      char            SZ_RESULT_SUMMARY_LINE[];
    extern	char  		SZ_RESULT_ITEMS_SCANNED[];
    extern	char  		SZ_RESULT_FILE_TYPE[];
    extern	char  		SZ_RESULT_INOCULATION[];
    extern	char  		SZ_RESULT_SCAN_TIME[];
    extern	char  		SZ_RESULT_TITLE_LINE[];
    extern	char  		SZ_RESULT_MEMORY[];
    extern	char  		SZ_RESULT_MASTER_BOOT[];
    extern	char  		SZ_RESULT_BOOT[];
    extern	char  		SZ_RESULT_FILES[];

    auto        DIALOG_RECORD * dialog;
    auto  	HFILE		hFile;
    auto        char            szFilename[SYM_MAX_PATH];
    auto	BOOL		bErrorState;
    auto        DWORD           dwStartTime;
    auto        char    szTempNum[16];


    STRCPY(szFilename, SZ_SCAN_FILENAME);
    if ( !SelectPrintDevice(SZ_SCAN_RESULTS_TITLE, szFilename,&hFile) )
        return;

    dialog = DialogOpen2(&dlgPrintStatus,SZ_SCAN_RESULTS_TITLE);

    TimerDelayBegin (&dwStartTime);

    bErrorState = PRReportTitle(&hFile,SZ_SCAN_RESULTS_TITLE);


    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
                                          SZ_RESULT_SUMMARY_LINE,
                                          szSummaryLines);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
	                                  SZ_RESULT_ITEMS_SCANNED,
					  szItems);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					  SZ_RESULT_FILE_TYPE,
					  szType);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					  SZ_RESULT_INOCULATION,
					  szInoculation);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					  SZ_RESULT_SCAN_TIME,
					  szTime);

    if (bErrorState)
        bErrorState = PRPrintNCRLFs(&hFile,1);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					  SZ_RESULT_TITLE_LINE);

    if (bErrorState)
        bErrorState = PrintDashedLines(&hFile);

    if (bErrorState)
        bErrorState = PRPrintNCRLFs(&hFile,1);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					  SZ_RESULT_MEMORY,
                                   	  szMemScanned,
                               		  SZ_NO,
                                      SZ_DASH);
    
    if ( glpNoNav->Stats.MasterBootRec.uCleaned)
        _ConvertLongToString(glpNoNav->Stats.MasterBootRec.uCleaned, szTempNum, 10, 0);
    else
        STRCPY(szTempNum, SZ_DASH);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					  SZ_RESULT_MASTER_BOOT,
                                          glpNoNav->Stats.MasterBootRec.uScanned,
                                          glpNoNav->Stats.MasterBootRec.uInfected,
                                          szTempNum);
    
    if ( glpNoNav->Stats.BootRecs.uCleaned)
        _ConvertLongToString(glpNoNav->Stats.BootRecs.uCleaned, szTempNum, 10, 0);
    else
        STRCPY(szTempNum, SZ_DASH);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					SZ_RESULT_BOOT,
                                        glpNoNav->Stats.BootRecs.uScanned,
                                        glpNoNav->Stats.BootRecs.uInfected,
                                        szTempNum);
         
    if ( glpNoNav->Stats.Files.uCleaned)
        _ConvertLongToString(glpNoNav->Stats.Files.uCleaned, szTempNum, 10, 0);
    else
        STRCPY(szTempNum, SZ_DASH);

    if (bErrorState)
        bErrorState = PRPrintFrmtCRLFLine(&hFile,
					SZ_RESULT_FILES,
                                        &(glpNoNav->Stats.Files.dwScanned),
                                        glpNoNav->Stats.Files.uInfected,
                                        szTempNum);

    if (bErrorState)
        bErrorState = PRPrintNCRLFs(&hFile,1);

    if  (bErrorState &&
        glpNoNav->lpInfArr != NULL)
	{
	if (bErrorState)
	    bErrorState = PrintResultInfected(&hFile);
	}

    FileClose(hFile);
                                        // Make sure show for at least 1 sec
    TimerDelayEnd(1, dwStartTime);

    DialogClose(dialog, EVENT_USED);

    if (!bErrorState)
        StdDlgError(SZ_PRINTINGERROR);

}  // End ScanPrintResults()
/**************************************************************************
 * PrintResultInfected
 *
 * DESCRIPTION  :
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-21-93 at 06:35:48pm Function created
 ************************************************************************* */
BOOL LOCAL PASCAL PrintResultInfected(HFILE * hFile)
{
    extern  char            SZ_RESULT_PRINT_FILE    [];
    extern  char            SZ_RESULT_PRINT_VIRUS   [];
    extern  char            SZ_RESULT_PRINT_STATUS  [];
    extern  char            SZ_PROBLEMS_FOUND_TITLE [];
    extern  char            SZ_SCAN_RESULTS_TITLE   [];

    auto    INFECTEDFILE    rInfected;
                                        // Sorry for the magic constant 60.
                                        // This is for the section header
                                        // "File:   " before the virus name.
    auto    char            szWriteThis [SYM_MAX_PATH + 60];
    auto    DWORD	    dwEntry;


    for (dwEntry = 0;;dwEntry++)
        {

        if (!GetInfectedFile (&rInfected, glpNoNav, dwEntry))
	    break;

        FastStringPrint(szWriteThis,
                        SZ_RESULT_PRINT_FILE,
                        rInfected.szFullPath);

	if (!PRPrintFrmtCRLFLine(hFile,szWriteThis))
	    return(FALSE);

        FastStringPrint(szWriteThis,
                        SZ_RESULT_PRINT_VIRUS,
                        rInfected.Notes.lpVirName);
        if (!PRPrintFrmtCRLFLine(hFile,szWriteThis))
            return(FALSE);

        FastStringPrint(szWriteThis,
                        SZ_RESULT_PRINT_STATUS,
                        ScanFoundReturnStatusString(rInfected.wStatus));
        if (!PRPrintFrmtCRLFLine(hFile,szWriteThis))
            return(FALSE);
   
        if (!PRPrintNCRLFs(hFile,1))
            return(FALSE);
        }

    return(TRUE);
} // End PrintResultInfected()


/**************************************************************************
 * PrintDashedLines
 *
 * DESCRIPTION  : Print a series of dashed lines across the output device
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-21-93 at 06:11:38pm Function created
 ************************************************************************* */
BOOL LOCAL PASCAL PrintDashedLines(HFILE * hFile)
{
    extern	BYTE  		byPaperPageWidth;
    extern	BYTE		byPaperLeftMargin;
    extern	BYTE		byPaperRightMargin;

    if (!PRPrintNChars(hFile,' ',byPaperLeftMargin))
	return(FALSE);

    if ( !PRPrintNChars(hFile,'-',(BYTE)(byPaperPageWidth -
                                  (byPaperLeftMargin + byPaperRightMargin))) )
	return(FALSE);

    if (!PRPrintNCRLFs(hFile,1))
	return(FALSE);

    return(TRUE);

} // End PrintDashedLines()


//************************************************************************
// ScanFoundDisableButtons()
//
// This routine disables buttons based on the scanner prompt options and
// the use inoculation technology option found in the NONAV struct.
//
// Parameters:
//      LPNONAV     lpNoNav             Has NAV options information.
//
// Returns:
//      Nothing
//************************************************************************
// 4/13/93 DALLEE, Function created.
// 6/14/93 SKURTZ, Redid logic
// 6/15/93 DALLEE, Another change in logic.
// 6/29/93 DALLEE, Don't enable delete for boot infections.
// 7/29/93 DALLEE, Last time I touch this function!  Use the FileStatus
//              array to determine what buttons.
// 8/09/93 DALLEE, Positively the last time... Also check vs. NAVOPTIONS.
//************************************************************************

VOID LOCAL PASCAL ScanFoundDisableButtons (LPNONAV lpNoNav)
{
    extern  ButtonsRec          buScanFoundButtons;
    extern  FILESTATUSSTRUCT    FileStatus[];

    auto    UINT                uStatus;
    auto    INFECTEDFILE        rInfected;
    auto    BYTE                byRepair    = FALSE;
    auto    BYTE                byDelete    = FALSE;
    auto    BYTE                byExclude   = FALSE;
    auto    BYTE                byInoculate = FALSE;


    GetInfectedFile(&rInfected, lpNoNav, lrScanFoundItems.active_entry);
    uStatus = (UINT) rInfected.wStatus;

                                        // Get buttons to display options
                                        // for either inoc-type file, or
                                        // infected file.
    if ( (uStatus == FILESTATUS_EXCLUDE_INOCULATION)    ||
         (uStatus == FILESTATUS_INOCULATED)             ||
         (uStatus == FILESTATUS_INOCULATIONFAILED)      ||
         (uStatus == FILESTATUS_INOCULATEDELETED)       ||
         (uStatus == FILESTATUS_INOCULATEDELETE_FAILED) ||
         (uStatus == FILESTATUS_NOTINOCULATED) )
        {
        if (INOC_PROMPT == lpNoNav->Inoc.uActionNewInoc)
            {
            byRepair = FileStatus[uStatus].byCanRepair && lpNoNav->Inoc.bDispRepair;
            byDelete = FileStatus[uStatus].byCanDelete && lpNoNav->Inoc.bDispDelete;
            byExclude = FileStatus[uStatus].byCanExclude && lpNoNav->Inoc.bDispExcl;
            byInoculate = FileStatus[uStatus].byCanInoc && lpNoNav->Inoc.bDispInoc;
            }
        }
    else if ( (uStatus == FILESTATUS_INOC_CHANGED_DELETED)  ||
         (uStatus == FILESTATUS_INOC_CHANGED_NOTDELETED)    ||
         (uStatus == FILESTATUS_INOCULATEREPAIRED)          ||
         (uStatus == FILESTATUS_INOCULATEREPAIR_FAILED)     ||
         (uStatus == FILESTATUS_REINOCULATED)               ||
         (uStatus == FILESTATUS_REINOCULATIONFAILED)        ||
         (uStatus == FILESTATUS_INOCULATEDCHANGED) )
        {
        if (INOC_CHANGE_PROMPT == lpNoNav->Inoc.uActionInocChg)
            {
            byRepair = FileStatus[uStatus].byCanRepair && lpNoNav->Inoc.bDispRepair;
            byDelete = FileStatus[uStatus].byCanDelete && lpNoNav->Inoc.bDispDelete;
            byExclude = FileStatus[uStatus].byCanExclude && lpNoNav->Inoc.bDispExcl;
            byInoculate = FileStatus[uStatus].byCanInoc && lpNoNav->Inoc.bDispInoc;
            }
        }
    else if (SCAN_PROMPT == lpNoNav->Opt.uAction)
        {
        byRepair = FileStatus[uStatus].byCanRepair && lpNoNav->Opt.bDispRepair;
        byExclude = FileStatus[uStatus].byCanExclude && lpNoNav->Opt.bDispExcl;

                                        //*** WARNING ********************
                                        // The special handling here for
                                        // DIR2 is different from the Windows
                                        // code.
                                        // DIR2 changes directory entries to
                                        // point to itself.
                                        // We don't do repairs under Windows
                                        // since it involves too much low-level
                                        // tweaking.  Delete is never allowed
                                        // in DOS nor Windows since this would
                                        // would destroy all infected files and
                                        // invalidate all directories.
                                        //                 -DALLEE 11/03/93
        byDelete = FileStatus[uStatus].byCanDelete &&
                   lpNoNav->Opt.bDispDelete        &&
                   !(rInfected.Notes.wInfo & bitINFO_DIR2);
        }

#ifdef NAVSCAN
    byRepair = FileStatus[uStatus].byCanRepair;
//                 ( (FILESTATUS_BOOTINFECTED == uStatus) ||
//                 (FILESTATUS_MASTERBOOTINFECTED == uStatus) ) &&
//                 !(rInfected.Notes.wInfo & (NO_GENERIC | DISPLAY_DIALOG) );
#endif

    ButtonEnable(&buScanFoundButtons,
                 SCAN_FOUND_REPAIR_BUTTON,
                 byRepair);

    ButtonEnable(&buScanFoundButtons,
                 SCAN_FOUND_DELETE_BUTTON,
                 byDelete);

    ButtonEnable(&buScanFoundButtons,
                 SCAN_FOUND_EXCLUDE_BUTTON,
                 byExclude);

    ButtonEnable(&buScanFoundButtons,
                 SCAN_FOUND_INOCULATE_BUTTON,
                 byInoculate);

    ButtonEnable(&buScanFoundButtons,
                 SCAN_FOUND_INFO_BUTTON,
                 FileStatus[uStatus].byHasVirusInfo);
} // End ScanFoundDisableButtons()


//************************************************************************
// ScanFoundListWrite()
//
// Writes an entry for the Problems Found list of problem files.
//
// Parameters:
//      ListRec     *lplrList           Pointer to list structure.
//      LPVOID      lpEntry             Pointer to start of first entry
//                                          buffer.
//      int         iAttr,              Attribute with which to write.
//      DWORD       dwEntry             Number of entry in list
//
// Returns:
//      nothing
//************************************************************************
// 4/24/93 DALLEE, Function created.
//************************************************************************

VOID ScanFoundListWrite ( ListRec   *lplrList,
                          LPVOID    lpEntry,
                          int       iAttr,
                          DWORD     dwEntry )
{
    auto    INFECTEDFILE    rInfected;
    auto    char            szWriteThis [SCAN_FOUND_ENTRY_WIDTH + 1];

    if (lpEntry == NULL)
        {
        FastWriteSpaces(lplrList->column_width);
        return;
        }

    ColorSaveAll();
    ColorSetAttr(iAttr);

                                        // Get the infected file we want
                                        // and create the string for the
                                        // list box.
    GetInfectedFile (&rInfected, lpEntry, dwEntry);
    ScanFoundCreateEntry(szWriteThis, &rInfected);

    FastWriteLine(szWriteThis, 0, lplrList->column_width);

                                        // Update the description and buttons
    if ( iAttr == INVERSE )
        {
        ScanFoundUpdateMessageBox(glpNoNav);

        if( bInFoundDlg )
            ScanFoundDisableButtons(glpNoNav);
        else
            ScanDetailsDisableButtons(glpNoNav);
        }

    ColorRestoreAll();
} // End ScanFoundListWrite()


//************************************************************************
// ScanFoundUpdateMessageBox()
//
// This routine displays the full filename and the suggested action to
// take in the message group box on the bottom of the Problems Found
// dialog.
//
// Parameters:
//      LPNONAV lpNoNav                 Pointer to current scan's info.
//
// Returns:
//      Nothing.
//************************************************************************
// 6/29/93 DALLEE, Function created.
// 6/30/93 SKURTZ  Added advice text based on infection status.
// 7/29/93 DALLEE, Use lookup function to get message box string.
//************************************************************************

VOID LOCAL PASCAL ScanFoundUpdateMessageBox (LPNONAV lpNoNav)
{
    extern  char            SZ_PROBLEMS_FOUND_FILE_FORMAT[];
    extern  char            SZ_PROBLEMS_FOUND_ZIP_FORMAT[];
    extern  char            SZ_PHYSICAL_DRIVE[];

    auto    INFECTEDFILE    rInfected;
    auto    char            *lpszMessage = NULL;
    auto    char            szStringHolder [GROUP_SIZE_NUM * 2 + 1];
    auto    char            szPath   [80+1];
    auto    char            szZipFile [SYM_MAX_PATH * 2 + 1];


    GetInfectedFile(&rInfected, lpNoNav, lrScanFoundItems.active_entry);

                                        // ---------------------------
                                        // Print the problem filename.
                                        // ---------------------------
    if (rInfected.wStatus == FILESTATUS_ZIPINFECTED)
    {
                                        // Set lpszMessage to point to filename
                                        // within zip.  NULL terminate szZipFile
                                        // at end of zip's filename.
        STRCPY(szZipFile, rInfected.szFullPath);
        lpszMessage = szZipFile;
        while (*lpszMessage != EOS)
            {
            if (*(lpszMessage) == ZIP_SEPARATOR)
                {
                *lpszMessage = EOS;
                lpszMessage++;
                break;
                }
            lpszMessage = AnsiNext(lpszMessage);
            }
        FastStringPrint(szStringHolder,
                SZ_PROBLEMS_FOUND_ZIP_FORMAT,
                lpszMessage,
                szZipFile);
    }
    else
    {
        if (rInfected.wStatus == FILESTATUS_MASTERBOOTINFECTED
        ||  rInfected.wStatus == FILESTATUS_MASTERBOOTREPAIRED
        ||  rInfected.wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
        {
            SPRINTF (szPath, SZ_PHYSICAL_DRIVE, rInfected.szFullPath[0]);
        }
        else
        {
            STRCPY (szPath, rInfected.szFullPath);
        }

        FastStringPrint(szStringHolder,
                        SZ_PROBLEMS_FOUND_FILE_FORMAT,
                        szPath);

    }
    WordWrapVirusComment(GROUP_SIZE_NUM - 5, szStringHolder);
    DialogOutUsingArray(0,2,lpofScanFoundMessage,szStringHolder);

                                        // ----------------------------
                                        // Print the suggestion string.
                                        // ----------------------------
    lpszMessage = ScanFoundReturnMessageBoxString(rInfected.wStatus);

    STRCPY(szStringHolder,lpszMessage);
    WordWrapVirusComment(GROUP_SIZE_NUM - 5, szStringHolder);
    DialogOutUsingArray(2,4,lpofScanFoundMessage,szStringHolder);

} // End ScanFoundUpdateMessageBox()

