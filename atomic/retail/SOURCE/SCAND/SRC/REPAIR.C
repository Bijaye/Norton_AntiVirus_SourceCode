// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/repair.c_v   1.0   06 Feb 1997 21:08:50   RFULLER  $
//
// Description:
//      These are the functions for the repair files dialog.
//
// Contains:
//      ScanFoundRepairDialog()
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/repair.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:50   RFULLER
// Initial revision
// 
//    Rev 1.3   08 Aug 1996 14:53:48   JBRENNA
// LuigiPlus Port:
//   DALLEE: Moved check for valid drive letter in repair all. This was
//     preventing MBR's from being repaired. Only does drive letter check
//     and sets status to failed if we would have tried a file repair.
// 
//   JWORDEN: Pass file name in Infected.szFullOemPath to match shared
//     functions in NAVXUTIL
// 
//    Rev 1.2   08 Aug 1996 13:26:20   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.48   17 May 1996 12:02:10   MZAREMB
// Validate infection path (drive letter) before taking callback action.
// 
//    Rev 1.47   06 May 1996 09:06:36   MZAREMB
// Initial Pegasus update: added NAVSCAN #ifdefs.
// 
//    Rev 1.46   23 Apr 1996 17:00:00   MZAREMB
// Fixed boot repair progress dialog to remove the "File:" label when 
// repairing boot record(s).  STS# NAV 52517
// 
//    Rev 1.45   04 Mar 1996 18:38:48   MKEATIN
// Ported DX fixes from KIRIN
// 
//    Rev 1.1   31 Jan 1996 16:13:40   MKEATIN
// Some code still sent an array of three strings instead of five to cutstring()
// 
//    Rev 1.0   30 Jan 1996 16:00:08   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:53:22   BARRY
// Initial revision.
// 
//    Rev 1.44.1.0   11 Dec 1995 16:33:24   JREARDON
// Branch base for version LUG1
// 
//    Rev 1.44   13 Nov 1995 13:44:16   JWORDEN
// Prevent MBR "Infected" records from being compared to the skip list
// in RepairAll
// 
//    Rev 1.43   06 Feb 1995 13:37:20   DALLEE
// SmartInocOpen() parameter change: Takes full path of file for which we're 
// opening the database -- necessary to distinguish NAVINOC from NAVINOCL?
// 
//    Rev 1.42   29 Dec 1994 16:46:26   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.41   28 Dec 1994 14:15:20   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   27 Dec 1994 19:36:22   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.1   05 Dec 1994 18:10:14   DALLEE
    // CVT1 script.
    //
//    Rev 1.40   28 Dec 1994 13:52:44   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:12   DALLEE
    // Initial revision.
    //
    //    Rev 1.36   24 Oct 1994 13:17:34   DALLEE
    // Check for NO_GENERIC flags before telling RepairFile() to try using
    // inoculation repairs.
//************************************************************************

#include "platform.h"
#include "stddos.h"
#include "stddlg.h"
#include "file.h"
#include "tsr.h"
#include "ctsn.h"
#include "virscan.h"

#include "navdprot.h"
#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "inoc.h"
#include "tsrcomm.h"

#include "scand.h"


//************************************************************************
// DEFINES
//************************************************************************

#define REPAIR_ONE_BUTTON       0
#define REPAIR_ALL_BUTTON       1
#define REPAIR_CANCEL_BUTTON    2
#define REPAIR_HELP_BUTTON      3

                                        // Defines for type of item to
                                        // repair with Repair All.
#define SCAN_NO_REPAIR          0
#define SCAN_FILE_REPAIR        1
#define SCAN_INOC_REPAIR        2
#define SCAN_BOOT_REPAIR        3


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL RepairAll (LPNONAV lpNoNav, WORD wTypeRepair);

BOOL STATIC PASCAL RepairAllCallback(LPINFECTEDFILE lpInfected,
                                     LPNONAV        lpNoNav,
                                     WORD           wEntry,
                                     LPARAM         dwTypeRepair );

WORD LOCAL PASCAL GetRepairType (WORD wStatus);

//************************************************************************
// LOCAL VARIABLES
//************************************************************************

STATIC 	DIALOG_RECORD   *RepairDialog;
STATIC  char            cLastDrive;


//************************************************************************
// ScanFoundRepairDialog()
//
// This routine displays the Problems Found Repair dialog.
//
// Parameters:
//      None
//
// Returns:
//      ACCEPT_DIALOG                   File(s) repaired.
//      ABORT_DIALOG                    Cancel or ESC hit.
//      FALSE                           Couldn't read INFECTEDFILE info.
//************************************************************************
// 3/3/93 DALLEE Function created.
// 6/24/93 DALLEE, reopen inoculation database read only for repairs.
//************************************************************************

WORD PASCAL ScanFoundRepairDialog (VOID)
{
    extern  LPNONAV         glpNoNav;
    extern  ListRec         lrScanFoundItems;

    extern  char            *LPSZ_SCAN_REPAIR_BUTTONS_LABELS    [];
    extern  char            *LPSZ_SCAN_REPAIR_WHAT_FILES        [];
    extern  char            *LPSZ_SCAN_REPAIR_WHAT_INOC_FILES   [];
    extern  char            *LPSZ_SCAN_REPAIR_WHAT_BOOT_RECORDS [];
    extern  char            SZ_SCAN_REPAIR_TITLE                [];
    extern  char            SZ_SCAN_REPAIR_BOOT_TITLE           [];

    extern  DIALOG          dlScanAction;
    extern  ButtonsRec      buScanActionButtons;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult = FALSE;
    auto    INFECTEDFILE    rInfected;
    auto    BOOL            bDone = FALSE;
    auto    DWORD           dwEntry;
    auto    LPSTR           *lpszQuestion;
    auto    UINT            uRepairType;
    auto    WORD            wOldHelp;

    auto    char            lpMessageBuffer [SCAN_PROMPT_MESSAGE_LINES][46 + 1];
    auto    LPSTR           lpszMessage [SCAN_PROMPT_MESSAGE_LINES] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };
#ifdef     NAVSCAN
    extern  DIALOG          dlRepairBootOnly;
#endif

    dwEntry = lrScanFoundItems.active_entry;

    if (GetInfectedFile(&rInfected, glpNoNav, dwEntry))
        {
        buScanActionButtons.strings = LPSZ_SCAN_REPAIR_BUTTONS_LABELS;
        ScanFoundCreateMessage(lpszMessage, &rInfected);

        uRepairType = GetRepairType(rInfected.wStatus);

        switch (uRepairType)
            {
            case SCAN_BOOT_REPAIR:
                dlScanAction.title = SZ_SCAN_REPAIR_BOOT_TITLE;
                lpszQuestion = LPSZ_SCAN_REPAIR_WHAT_BOOT_RECORDS;
                break;

            case SCAN_FILE_REPAIR:
                dlScanAction.title = SZ_SCAN_REPAIR_TITLE;
                lpszQuestion = LPSZ_SCAN_REPAIR_WHAT_FILES;
                break;

            case SCAN_INOC_REPAIR:
                dlScanAction.title = SZ_SCAN_REPAIR_TITLE;
                lpszQuestion = LPSZ_SCAN_REPAIR_WHAT_INOC_FILES;
                break;
            }

#ifdef NAVSCAN
        if ( uRepairType != SCAN_BOOT_REPAIR ) 
            {
            DialogBox2(&dlRepairBootOnly);
            }
        else
#endif

        if (NULL != (lpdrDialog = DialogOpen2( &dlScanAction,
                                               lpszMessage[0],
                                               lpszMessage[1],
                                               lpszMessage[2],
                                               lpszMessage[3],
                                               lpszMessage[4],
                                               lpszQuestion[0],
                                               lpszQuestion[1])))
            {
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
                    if ( glpNoNav->Inoc.bUseInoc &&
                         (uRepairType == SCAN_INOC_REPAIR) )
                        {
                        cLastDrive = rInfected.szFullPath[0];
                        }

                    switch (buScanActionButtons.value)
                        {
                        case REPAIR_ONE_BUTTON:
                            if (uRepairType == SCAN_BOOT_REPAIR)
                                {
                                RepairBoot(glpNoNav, &rInfected);
                                }
#ifndef NAVSCAN
                            else
                                {
                                if (glpNoNav->Inoc.bUseInoc)
                                    {
                                    glpNoNav->uInocOpenOK
                                        = SmartInocOpen(rInfected.szFullPath, FALSE);
                                    }

                                ScanRepairFile(glpNoNav, &rInfected);

                                if (glpNoNav->Inoc.bUseInoc)
                                    {
                                    SmartInocClose();
                                    glpNoNav->uInocOpenOK = SMARTOPEN_CLOSED ;
                                    }
                                }
#endif
                            ScanActionMessage(&rInfected);
                                        // Update the information.
                            PutInfectedFile(&rInfected, glpNoNav, dwEntry);

                            bDone = TRUE;
                            break;

                        case REPAIR_ALL_BUTTON:
#ifndef NAVSCAN
                            if ( glpNoNav->Inoc.bUseInoc &&
                                 (uRepairType != SCAN_BOOT_REPAIR) )
                                {
                                glpNoNav->uInocOpenOK
                                        = SmartInocOpen(rInfected.szFullPath, FALSE);
                                }
                            RepairAll(glpNoNav,
                                      GetRepairType(rInfected.wStatus));
                            if ( glpNoNav->Inoc.bUseInoc &&
                                 (uRepairType != SCAN_BOOT_REPAIR) )
                                {
                                SmartInocClose();
                                glpNoNav->uInocOpenOK = SMARTOPEN_CLOSED ;
                                }
#endif
                            bDone = TRUE;
                            break;

                        case REPAIR_CANCEL_BUTTON:
                            wResult = ABORT_DIALOG;
                            bDone = TRUE;
                            break;

                        case REPAIR_HELP_BUTTON:
                            HyperHelpDisplay();
                            break;
                        }
                    }
                } while (!bDone);
            DialogClose(lpdrDialog, wResult);
            HyperHelpTopicSet(wOldHelp);
            }
        } // End if (GetInfectedFile(&rInfected...

    return (wResult);
} // End ScanFoundRepairDialog()


//************************************************************************
// RepairAll()
//
// This routine goes through the list of infected files in the NONAV
// struct and attempts to repair any with status FILESTATUS_INFECTED or
// FILESTATUS_INOCULATEDCHANGE depending on the status of the current
// file in the problems found list.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to the scan info we need.
//      WORD        wTypeRepair         Repair Infected, or InocChange files?
//                                          or infected boot records?
// Returns:
//      Nothing
//************************************************************************
// 4/7/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL RepairAll (LPNONAV lpNoNav, WORD wTypeRepair)
{
    extern  DIALOG                  dlFileProgress;
    extern  char                    szFileRepairInProgress[];
    extern  char                    szBootRepairInProgress[];
    extern  char                    *lpszProgressStrings [];
    extern  char                    *lpszBootProgressStrings [];
    extern  char                    szRepairAllTitle[];
    extern  OutputFieldRec          ofScanDirectory;    // for file name.
    extern  OutputFieldRec          ofScanPercentBar;   // % bar
    extern  ButtonsRec              buScanStop;
    extern  OutputFieldRec          ofProgressStatus;

    auto    ProblemsFoundDoAllRec   rRepairAllInfo;
    auto    DWORD                   dwDelayStart;

    MEMSET(&rRepairAllInfo, '\0', sizeof(rRepairAllInfo));
    rRepairAllInfo.wActionType = wTypeRepair;

    dlFileProgress.title  = szRepairAllTitle;
    ofProgressStatus.data = (wTypeRepair == SCAN_BOOT_REPAIR ?
                             szBootRepairInProgress : szFileRepairInProgress);
    // Show different dialog strings if we are repairing boot record(s)
    dlFileProgress.strings= (wTypeRepair == SCAN_BOOT_REPAIR ?
                             lpszBootProgressStrings : lpszProgressStrings);

    RepairDialog = DialogOpen2(&dlFileProgress,
                         &ofScanPercentBar,         // % percent bar
			 &ofProgressStatus,
                         &ofScanDirectory,          // file name.
                         &buScanStop);              // stop button

    TimerDelayBegin ( & dwDelayStart ); // Avoid "flash" if error or short
    ReadAllInfectedFiles(lpNoNav,
                         RepairAllCallback,
                         (LPARAM) &rRepairAllInfo);

    StdDlgUpdateFillBar(&ofScanPercentBar,1,1);
    TimerDelayEnd ( 1, dwDelayStart );
    DialogClose(RepairDialog, ACCEPT_DIALOG);
    
    // Restore original (default) setting
    dlFileProgress.strings= lpszProgressStrings;

} // End RepairAll()


//************************************************************************
// RepairAllCallback()
//
// This routine is called for each problem item to see if it should be
// repaired.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Infected file information.
//      LPNONAV         lpNoNav         Has scan information.
//      WORD            wEntry          Index of INFECTEDFILE in list.
//      LPARAM          dwRepairAllInfo Repair Infected, or InocChange files?
//                                          and skip what drives.
// Returns:
//      TRUE                            Tells ReadAllInfectedFiles to keep
//                                          on going.
//************************************************************************
// 4/7/93 DALLEE, Function created.
// 6/24/93 DALLEE, code to open correct inoc database.
// 6/25/93 SKURTZ, Return FALSE if ScanRepairFiles() fails.
// 6/28/93 DALLEE, Return TRUE if ScanRepairFile() fails -- we still want to
//          attempt repairs on the rest of the infected files.
//************************************************************************

BOOL STATIC PASCAL RepairAllCallback(LPINFECTEDFILE lpInfected,
                                     LPNONAV        lpNoNav,
                                     WORD           wEntry,
                                     LPARAM         dwRepairAllInfo)
{
    extern  OutputFieldRec          ofScanDirectory;    // for file name.
    extern  OutputFieldRec          ofScanPercentBar;   // % bar
    extern  ListRec                 lrScanFoundItems;
    extern  FILESTATUSSTRUCT        FileStatus[];

    auto    UINT                    uEvent;
    auto    ProblemsFoundDoAllRec   *lpRepairAllInfo;
    auto    BYTE                    byDrive;

    uEvent = DialogProcessEvent(RepairDialog,DialogGetEvent());
    if (uEvent == ACCEPT_DIALOG || uEvent == ABORT_DIALOG)
        {
        return(FALSE);
        }

    lpRepairAllInfo = (ProblemsFoundDoAllRec *) dwRepairAllInfo;

    StdDlgUpdateFillBar(&ofScanPercentBar,wEntry,lrScanFoundItems.num);

    byDrive = lpInfected->szFullPath[0];

                                        // If the status of this infectedfile
                                        // matches what we want (infected
                                        // or inoc change) and the user has
                                        // not told us to skip this drive,
                                        // then perform the repair.
    if ((FileStatus[lpInfected->wStatus].byCanRepair)
        &&  (GetRepairType(lpInfected->wStatus) == lpRepairAllInfo->wActionType) )
        {
                                        // Doing boot repair...
        if (lpRepairAllInfo->wActionType == SCAN_BOOT_REPAIR)
            {
            if (!RepairBoot(lpNoNav, lpInfected))
                {
                ScanActionMessage(lpInfected);
                }
            }
#ifndef NAVSCAN
        else                            // Doing file repair...
            {
            if (( byDrive < 'A') ||     // Invalid path or MBR infected?
                ( byDrive > 'Z'))
                {
                // do nothing... Shouldn't get here, but I'm leaving this
                // check so the next line will never GPF. DKA
                }
            else if (lpRepairAllInfo->byaSkippedDrives[byDrive - 'A'])
                {
                lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                                       PROCESS_REPAIR,
                                                       FALSE);
                }
            else switch (DoAllCheckWriteToFile(lpInfected->szFullPath, lpNoNav))
                {
                case SKIPDRIVE_ABORT:
                    lpRepairAllInfo->
                        byaSkippedDrives[byDrive-'A'] = TRUE;
                    //-----------------------------------------
                    // Drop through here is intentional so the
                    // filestatus is updated to repair failed.
                    //-----------------------------------------

                case SKIPDRIVE_IGNORE:
                    lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                                           PROCESS_REPAIR,
                                                           FALSE);
                    break;

                                        //--------------------------------
                default:                // Do the action on this file.
                case SKIPDRIVE_RETRY:   //--------------------------------

                    DialogOutput(&ofScanDirectory,lpInfected->szFullPath);

                                        // Make sure we have the right
                                        // inoculation database open...
                    if ( (lpNoNav->Inoc.bUseInoc) &&
                         (cLastDrive != byDrive) )
                        {
                        SmartInocClose();
                        cLastDrive = byDrive;
                        lpNoNav->uInocOpenOK
                             = SmartInocOpen(lpInfected->szFullPath, FALSE);
                        }

                                        // Do the file repair.
                    if (!ScanRepairFile(lpNoNav, lpInfected))
                        {
                        ScanActionMessage(lpInfected);
                        }

                    if ( (lpNoNav->Inoc.bUseInoc) &&
                         (cLastDrive != byDrive) )
                        {
                        SmartInocClose();
                        lpNoNav->uInocOpenOK = SMARTOPEN_CLOSED ;
                        }
                    break;
                }
            }
#endif
        }

    return (TRUE);
} // End RepairAllCallback()


//************************************************************************
// ScanRepairFile()
//
// This routine attempts to repair an infected file.
//
// Parameters:
//      LPNONAV         lpNoNav         Info on the current scan.
//      LPINFECTEDFILE  lpInfected      Info on file to repair.
//
// Returns:
//      TRUE                            File successfully repaired.
//      FALSE                           Couln't repair it.
//************************************************************************
// 3/28/93 DALLEE, Function created.
//************************************************************************

UINT PASCAL ScanRepairFile (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected)
{
    auto    REPAIRFILESTRUCT    rRepair;
    auto    UINT                uResult;


    rRepair.uVirusSigIndex  = lpInfected->uVirusSigIndex;
    rRepair.bUseInoc        = lpNoNav->Inoc.bUseInoc &&
                                !(NO_GENERIC & lpInfected->Notes.wControl2);
    rRepair.bCreateBackup   = lpNoNav->Gen.bBackupRep;

    STRCPY (rRepair.szFullPath, lpInfected->szFullPath);
    STRCPY (rRepair.szFullOemPath, lpInfected->szFullOemPath);
    STRCPY (rRepair.szBackExt, lpNoNav->Gen.szBackExt);

    TSR_OFF;
    uResult = RepairFile(&rRepair);
    TSR_ON;

    lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                           PROCESS_REPAIR,
                                           uResult);

                                        // Update the NONAV info if we
                                        // repaired the file...
    if (uResult == TRUE)
        {
        lpNoNav->Stats.Files.uCleaned++;
        }

    return (uResult);
} // End ScanRepairFile()


//************************************************************************
// GetRepairType()
//
// This routine returns the type of repair based on the filestatus.
//
// Parameters:
//      WORD    wStatus                 File status whose repair type we
//                                      want to know.
// Returns:
//      SCAN_NO_REPAIR                  Repairs not allowed for this type.
//      SCAN_FILE_REPAIR                Infected file repair.
//      SCAN_INOC_REPAIR                Inoc change repair.
//      SCAN_BOOT_REPAIR                Boot type repair.
//************************************************************************
// 7/30/93 DALLEE, Function created.
//************************************************************************

WORD LOCAL PASCAL GetRepairType (WORD wStatus)
{
    auto    WORD    wRepairType;

    if ( (wStatus == FILESTATUS_INFECTED) ||
         (wStatus == FILESTATUS_NOTDELETED))
        {
        wRepairType = SCAN_FILE_REPAIR;
        }
    else if ((wStatus == FILESTATUS_BOOTINFECTED) ||
             (wStatus == FILESTATUS_MASTERBOOTINFECTED))
        {
        wRepairType = SCAN_BOOT_REPAIR;
        }
    else if (wStatus == FILESTATUS_INOCULATEDCHANGED)
        {
        wRepairType = SCAN_INOC_REPAIR;
        }
    else
        {
        wRepairType = SCAN_NO_REPAIR;
        }

    return (wRepairType);

} // End GetRepairType()

