// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/repair.c_v   1.0   06 Feb 1997 20:56:28   RFULLER  $
//
// Description:
//      These are the functions for the repair files dialog.
//
// Contains:
//      ScanFoundRepairDialog()
// See Also:
//************************************************************************
// $Log:   S:/navc/VCS/repair.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:28   RFULLER
// Initial revision
// 
//    Rev 1.1   27 Jan 1997 15:54:08   MKEATIN
// Copied szFullPath into the repair structer's szFullOemPath member.  This
// field is used in CreateBackup().  So, CreateBackup now works, although there
// is no OEM to ANSI funcionality.
// 
// 
//    Rev 1.0   31 Dec 1996 15:20:30   MKEATIN
// Initial revision.
// 
//    Rev 1.4   07 Nov 1996 17:23:36   JBELDEN
// 
//    Rev 1.3   07 Nov 1996 12:43:54   JBELDEN
// cleaned up and removed some dialog box related items.
//
//    Rev 1.2   31 Oct 1996 17:05:06   JBELDEN
// removed some dialog code
//
//    Rev 1.1   16 Oct 1996 18:28:54   JBELDEN
// stubbed out scanfoundrepairdialog. Not used in ttyscan.
//
//    Rev 1.0   02 Oct 1996 12:58:50   JBELDEN
// Initial revision.
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

STATIC  char            cLastDrive;


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
    extern  char                    szFileRepairInProgress[];
    extern  char                    szBootRepairInProgress[];
    extern  char                    *lpszProgressStrings [];
    extern  char                    *lpszBootProgressStrings [];
    extern  char                    szRepairAllTitle[];

    auto    ProblemsFoundDoAllRec   rRepairAllInfo;
    auto    DWORD                   dwDelayStart;

    MEMSET(&rRepairAllInfo, '\0', sizeof(rRepairAllInfo));
    rRepairAllInfo.wActionType = wTypeRepair;

    TimerDelayBegin ( & dwDelayStart ); // Avoid "flash" if error or short
    ReadAllInfectedFiles(lpNoNav,
                         RepairAllCallback,
                         (LPARAM) &rRepairAllInfo);

    TimerDelayEnd ( 1, dwDelayStart );


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
    extern  FILESTATUSSTRUCT        FileStatus[];

    auto    ProblemsFoundDoAllRec   *lpRepairAllInfo;
    auto    BYTE                    byDrive;


    lpRepairAllInfo = (ProblemsFoundDoAllRec *) dwRepairAllInfo;

    byDrive = lpInfected->szFullPath[0];

    if (( byDrive < 'A') ||             // Invalid path or MBR infected
        ( byDrive > 'Z'))
        {
        return (TRUE);
        }
                                        // If the status of this infectedfile
                                        // matches what we want (infected
                                        // or inoc change) and the user has
                                        // not told us to skip this drive,
                                        // then perform the repair.
    if (lpRepairAllInfo->byaSkippedDrives[byDrive - 'A'])
        {
        lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                               PROCESS_REPAIR,
                                               FALSE);
        }
    else
        if ((FileStatus[lpInfected->wStatus].byCanRepair)
        &&  (GetRepairType(lpInfected->wStatus) == lpRepairAllInfo->wActionType) )
        {
        if (lpRepairAllInfo->wActionType == SCAN_BOOT_REPAIR)
            {
            if (!RepairBoot(lpNoNav, lpInfected))
                {
                }
            }
#ifndef NAVSCAN
        else
            {
            switch (DoAllCheckWriteToFile(lpInfected->szFullPath, lpNoNav))
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

                                        // Make sure we have the right
                                        // inoculation database open...
                    if ( (lpNoNav->Inoc.bUseInoc) &&
                         (cLastDrive != lpInfected->szFullPath[0]) )
                        {
                        SmartInocClose();
                        cLastDrive = byDrive;
                        lpNoNav->uInocOpenOK
                             = SmartInocOpen(lpInfected->szFullPath, FALSE);
                        }

                                        // Do the file repair.
                    if (!ScanRepairFile(lpNoNav, lpInfected))
                        {
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
    STRCPY (rRepair.szFullOemPath, lpInfected->szFullPath);
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

