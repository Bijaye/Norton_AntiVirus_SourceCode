// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/delete.c_v   1.0   06 Feb 1997 21:08:42   RFULLER  $
//
// Description:
//      These are the functions for the delete files dialog.
//
// Contains:
//      ScanFoundDeleteDialog()
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/delete.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:42   RFULLER
// Initial revision
// 
//    Rev 1.2   08 Aug 1996 13:26:08   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.31   17 May 1996 12:01:44   MZAREMB
// Validate infection path (drive letter) before taking callback action.
// 
//    Rev 1.30   04 Mar 1996 18:38:40   MKEATIN
// Ported DX fixes from KIRIN
// 
//    Rev 1.1   31 Jan 1996 16:15:32   MKEATIN
// Some code still sent an array of three strings instead of five to cutstring()
// 
//    Rev 1.0   30 Jan 1996 16:00:08   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:53:22   BARRY
// Initial revision.
// 
//    Rev 1.29.1.0   11 Dec 1995 16:33:32   JREARDON
// Branch base for version LUG1
// 
//    Rev 1.29   29 Dec 1994 16:46:24   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.28   28 Dec 1994 14:15:48   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   27 Dec 1994 19:36:18   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.1   05 Dec 1994 18:10:10   DALLEE
    // CVT1 script.
    //
//    Rev 1.27   28 Dec 1994 13:53:22   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:02   DALLEE
    // Initial revision.
    //
    //    Rev 1.23   18 Sep 1993 14:40:00   BARRY
    // Add minimum delay for progress bar when doing ALL
//************************************************************************

#include "platform.h"
#include "stddos.h"
#include "stddlg.h"
#include "tsr.h"
#include "ctsn.h"
#include "virscan.h"

#include "navdprot.h"

#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "tsrcomm.h"

#include "scand.h"


//************************************************************************
// DEFINES
//************************************************************************

#define DELETE_ONE_BUTTON       0
#define DELETE_ALL_BUTTON       1
#define DELETE_CANCEL_BUTTON    2
#define DELETE_HELP_BUTTON      3

#define SCAN_NO_DELETE          0
#define SCAN_INFECTED_DELETE    1
#define SCAN_NOT_INOC_DELETE    2
#define SCAN_CHANGED_DELETE     3


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL DeleteAllFiles    (LPNONAV lpNoNav, UINT uDeleteType);
WORD LOCAL PASCAL GetDeleteType     (WORD wStatus);

BOOL STATIC PASCAL DeleteAllFilesCallback(LPINFECTEDFILE lpInfected,
                                          LPNONAV        lpNoNav,
                                          WORD           wEntry,
                                          LPARAM         dwInocDelete);


//************************************************************************
// LOCAL VARIABLES
//************************************************************************

STATIC 	DIALOG_RECORD   *DeleteDialog;

//************************************************************************
// ScanFoundDeleteDialog()
//
// This routine displays the Problems Found Delete dialog.
//
// Parameters:
//      None
//
// Returns:
//      ACCEPT_DIALOG                   File(s) deleted.
//      ABORT_DIALOG                    Cancel or ESC hit.
//      FALSE                           Couldn't read INFECTEDFILE info.
//************************************************************************
// 3/3/93 DALLEE Function created.
//************************************************************************

WORD PASCAL ScanFoundDeleteDialog (VOID)
{
    extern  LPNONAV         glpNoNav;
    extern  ListRec         lrScanFoundItems;

    extern  char            *LPSZ_SCAN_DELETE_BUTTONS_LABELS    [];
    extern  char            *LPSZ_DELETE_WHAT_FILES             [];
    extern  char            *LPSZ_DELETE_WHAT_INOC_FILES        [];
    extern  char            *LPSZ_DELETE_WHAT_CHANGED_FILES     [];
    extern  char            SZ_SCAN_DELETE_TITLE                [];

    extern  DIALOG          dlScanAction;
    extern  ButtonsRec      buScanActionButtons;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult = FALSE;
    auto    INFECTEDFILE    rInfected;
    auto    UINT            uDeleteType;
    auto    BOOL            bDone = FALSE;
    auto    DWORD           dwEntry;
    auto    WORD            wOldHelp;
    auto    LPSTR           *lpszQuestion;

    auto    char            lpMessageBuffer [SCAN_PROMPT_MESSAGE_LINES]
                                            [SCAN_PROMPT_MESSAGE_WIDTH + 1];
    auto    LPSTR           lpszMessage [SCAN_PROMPT_MESSAGE_LINES] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

    dwEntry = lrScanFoundItems.active_entry;

    if (GetInfectedFile(&rInfected, glpNoNav, dwEntry))
        {
        ScanFoundCreateMessage(lpszMessage, &rInfected);

        buScanActionButtons.strings = LPSZ_SCAN_DELETE_BUTTONS_LABELS;
        dlScanAction.title          = SZ_SCAN_DELETE_TITLE;

        uDeleteType = GetDeleteType(rInfected.wStatus);

        switch (uDeleteType)
            {
            case SCAN_INFECTED_DELETE:
                lpszQuestion = LPSZ_DELETE_WHAT_FILES;
                break;

            case SCAN_NOT_INOC_DELETE:
                lpszQuestion = LPSZ_DELETE_WHAT_INOC_FILES;
                break;

            case SCAN_CHANGED_DELETE:
                lpszQuestion = LPSZ_DELETE_WHAT_CHANGED_FILES;
                break;
            }

        lpdrDialog = DialogOpen2( &dlScanAction,
                                  lpszMessage[0],
                                  lpszMessage[1],
                                  lpszMessage[2],
                                  lpszMessage[3],
                                  lpszMessage[4],
                                  lpszQuestion[0],
                                  lpszQuestion[1] );

        wOldHelp = HyperHelpTopicSet(HELP_DLG_ASK_ACTION);

        do
            {
            wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

            if (wResult == ABORT_DIALOG)
                {
                bDone = TRUE;
                }
                                        // Check for a button hit.
            if (wResult == ACCEPT_DIALOG)
                {
                switch (buScanActionButtons.value)
                    {
                    case DELETE_ONE_BUTTON:
                        DeleteFile(glpNoNav, &rInfected);
                        ScanActionMessage(&rInfected);

                        PutInfectedFile(&rInfected, glpNoNav, dwEntry);
                        bDone = TRUE;

                        break;

                    case DELETE_ALL_BUTTON:
                        DeleteAllFiles(glpNoNav, uDeleteType);
                        bDone = TRUE;
                        break;

                    case DELETE_CANCEL_BUTTON:
                        wResult = ABORT_DIALOG;
                        bDone = TRUE;
                        break;

                    case DELETE_HELP_BUTTON:
                        HyperHelpDisplay();
                        break;
                    }
                }
            } while (!bDone);
        DialogClose(lpdrDialog, wResult);
        HyperHelpTopicSet(wOldHelp);
        }

    return (wResult);
} // End ScanFoundDeleteDialog()


//************************************************************************
// DeleteAllFiles()
//
// This routine goes through the list of infected files in the NONAV
// struct and attempts to delete any with status inoculation change, or
// with status infected, depending on the status of the current list
// entry.
//
// Parameters:
//      LPNONAV lpNoNav                 Pointer to all the info we need.
//      UINT    uDeleteType             Type of files to delete.
//
// Returns:
//      Nothing
//************************************************************************
// 4/7/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DeleteAllFiles (LPNONAV lpNoNav, UINT uDeleteType)
{
    extern  DIALOG                  dlFileProgress;
    extern  char                    szFileDeleteInProgress[];
    extern  char                    szDeleteAllTitle[];
    extern  OutputFieldRec          ofScanDirectory;    // for file name.
    extern  OutputFieldRec          ofScanPercentBar;   // % bar
    extern  ButtonsRec              buScanStop;
    extern  OutputFieldRec          ofProgressStatus;
    extern  ListRec                 lrScanFoundItems;

    auto    ProblemsFoundDoAllRec   rDeleteAllInfo;
    auto    DWORD                   dwDelayStart;

    MEMSET(&rDeleteAllInfo, '\0', sizeof(rDeleteAllInfo));
    rDeleteAllInfo.wActionType = uDeleteType;

    dlFileProgress.title  = szDeleteAllTitle;
    ofProgressStatus.data = szFileDeleteInProgress;

    DeleteDialog = DialogOpen2(&dlFileProgress,
			 &ofScanPercentBar,             // % percent bar
			 &ofProgressStatus,
			 &ofScanDirectory,		// file name.
			 &buScanStop);			// stop button

    TimerDelayBegin ( & dwDelayStart ); // Avoid "flash" if error or short
    ReadAllInfectedFiles(lpNoNav,
                         DeleteAllFilesCallback,
                         (LPARAM) &rDeleteAllInfo);

    StdDlgUpdateFillBar(&ofScanPercentBar,1,1);
    TimerDelayEnd ( 1, dwDelayStart );
    DialogClose(DeleteDialog, ACCEPT_DIALOG);

} // End DeleteAllFiles()


//************************************************************************
// DeleteAllFilesCallback()
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Infected file information.
//      LPNONAV         lpNoNav         Has scan information.
//      WORD            wEntry          Index of INFECTEDFILE in list.
//      LPARAM          dwDeleteAllInfo Delete Infected, or Inoc change files?
//                                          and skip what drive.
// Returns:
//      TRUE                            Tells ReadAllInfectedFiles to keep
//                                          on going.
//************************************************************************
// 4/7/93 DALLEE, Function created.
//************************************************************************

BOOL STATIC PASCAL DeleteAllFilesCallback(LPINFECTEDFILE lpInfected,
                                          LPNONAV        lpNoNav,
                                          WORD           wEntry,
                                          LPARAM         dwDeleteAllInfo )
{
    extern  OutputFieldRec          ofScanDirectory;    // for file name.
    extern  OutputFieldRec          ofScanPercentBar;   // % bar
    extern  ListRec                 lrScanFoundItems;
    extern  FILESTATUSSTRUCT        FileStatus[];

    auto    UINT                    uEvent;
    auto    ProblemsFoundDoAllRec   *lpDeleteAllInfo;
    auto    BYTE                    byDrive;

    uEvent = DialogProcessEvent(DeleteDialog,DialogGetEvent());

    if (uEvent == ACCEPT_DIALOG || uEvent == ABORT_DIALOG)
        {
        return(FALSE);
        }

    lpDeleteAllInfo = (ProblemsFoundDoAllRec *) dwDeleteAllInfo;

    StdDlgUpdateFillBar(&ofScanPercentBar,wEntry,lrScanFoundItems.num);

    byDrive = lpInfected->szFullPath[0];

    if (( byDrive < 'A') ||             // Invalid path or MBR infected 
        ( byDrive > 'Z'))
        { 
        return (TRUE);
        }
                                        // Depending on the first file,
                                        // delete only inoc change items,
                                        // or infected items.
                                        // Also check to see if user skipped
                                        // this drive.
    if (lpDeleteAllInfo->byaSkippedDrives[byDrive - 'A'])
        {
        lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                               PROCESS_DELETE,
                                               FALSE);
        }
    else if ( (FileStatus[lpInfected->wStatus].byCanDelete) &&
         (GetDeleteType(lpInfected->wStatus) == lpDeleteAllInfo->wActionType) )
        {
        switch (DoAllCheckWriteToFile(lpInfected->szFullPath, lpNoNav))
            {
            case SKIPDRIVE_ABORT:
                lpDeleteAllInfo->
                    byaSkippedDrives[byDrive - 'A'] = TRUE;
                //-----------------------------------------
                // Drop through here is intentional so the
                // filestatus is updated to delete failed.
                //-----------------------------------------

            case SKIPDRIVE_IGNORE:
                lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                                       PROCESS_DELETE,
                                                       FALSE);
                break;

                                        //--------------------------------
            default:                    // Go ahead and wipe this file.
            case SKIPDRIVE_RETRY:       //--------------------------------
                DialogOutput(&ofScanDirectory,lpInfected->szFullPath);

                if (!DeleteFile(lpNoNav, lpInfected))
                    {
                    ScanActionMessage(lpInfected);
                    }
                break;
            }
        }

    return (TRUE);
} // End DeleteAllFilesCallback()


//************************************************************************
// DeleteFile()
//
// This routine attempts to delete an infected file.
//
// Parameters:
//      LPNONAV         lpNoNav         Info on the current scan.
//      LPINFECTEDFILE  lpInfected      Info on file to delete.
//
// Returns:
//      TRUE                            File successfully deleted.
//      FALSE                           Couln't delete it.
//************************************************************************
// 4/7/93 DALLEE, Function created.
//************************************************************************

WORD PASCAL DeleteFile (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected)
{
    auto    WORD    wResult = FALSE;

    if (!(lpInfected->Notes.wInfo & bitINFO_DIR2))
        {
                                        // Disable TSR first.
        TSR_OFF;
        wResult = VirusScanDeleteFile(lpInfected->szFullPath, 1);
                                        // Reenable TSR.
        TSR_ON;

        if (wResult == TRUE)
            {
            lpNoNav->Stats.Files.uCleaned++;
            }
        }

    lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                           PROCESS_DELETE,
                                           wResult);
    return (wResult);
} // End DeleteFile()


//************************************************************************
// GetDeleteType()
//
// This routine returns the type of deletion based on the filestatus.
//
// Parameters:
//      WORD    wStatus                 File status whose deletion type we
//                                      want to know.
// Returns:
//      SCAN_NO_DELETE                  Deletions not allowed for this type.
//      SCAN_INFECTED_DELETE            Infected file deletion.
//      SCAN_NOT_INOC_DELETE            Not inoculated deletion.
//      SCAN_CHANGED_DELETE             Inoculation changed deletion.
//************************************************************************
// 9/03/93 DALLEE, Function created.
//************************************************************************

WORD LOCAL PASCAL GetDeleteType (WORD wStatus)
{
    auto    WORD    wDeleteType;

    if ( (wStatus == FILESTATUS_INFECTED) ||
         (wStatus == FILESTATUS_NOTREPAIRED) )
        {
        wDeleteType = SCAN_INFECTED_DELETE;
        }
    else if ((wStatus == FILESTATUS_NOTINOCULATED) ||
             (wStatus == FILESTATUS_INOCULATIONFAILED))
        {
        wDeleteType = SCAN_NOT_INOC_DELETE;
        }
    else if ((wStatus == FILESTATUS_INOCULATEDCHANGED) ||
             (wStatus == FILESTATUS_INOCULATEREPAIR_FAILED) ||
             (wStatus == FILESTATUS_REINOCULATIONFAILED))
        {
        wDeleteType = SCAN_CHANGED_DELETE;
        }
    else
        {
        wDeleteType = SCAN_NO_DELETE;
        }

    return (wDeleteType);

} // End GetDeleteType()

