// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/delete.c_v   1.0   06 Feb 1997 20:56:28   RFULLER  $
//
// Description:
//      These are the functions for the delete files dialog.
//
// Contains:
// See Also:
//************************************************************************
// $Log:   S:/navc/VCS/delete.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:28   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:28   MKEATIN
// Initial revision.
// 
//    Rev 1.4   07 Nov 1996 17:20:54   JBELDEN
// removed call to code removed from other file
// 
//    Rev 1.3   07 Nov 1996 12:49:00   JBELDEN
// cleaned up and removed some dialog box related items
//
//    Rev 1.2   31 Oct 1996 17:04:50   JBELDEN
// removed some dialogbox code
//
//    Rev 1.1   16 Oct 1996 17:47:58   JBELDEN
// stubbed out scanfounddeletedialog.  Not used in ttyscan
//
//    Rev 1.0   02 Oct 1996 12:58:44   JBELDEN
// Initial revision.
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

    auto    ProblemsFoundDoAllRec   rDeleteAllInfo;
    auto    DWORD                   dwDelayStart;

    MEMSET(&rDeleteAllInfo, '\0', sizeof(rDeleteAllInfo));
    rDeleteAllInfo.wActionType = uDeleteType;

    TimerDelayBegin ( & dwDelayStart ); // Avoid "flash" if error or short
    ReadAllInfectedFiles(lpNoNav,
                         DeleteAllFilesCallback,
                         (LPARAM) &rDeleteAllInfo);
    TimerDelayEnd ( 1, dwDelayStart );

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
    extern  FILESTATUSSTRUCT        FileStatus[];

    auto    UINT                    uEvent;
    auto    ProblemsFoundDoAllRec   *lpDeleteAllInfo;
    auto    BYTE                    byDrive;


    if (uEvent == ACCEPT_DIALOG || uEvent == ABORT_DIALOG)
        {
        return(FALSE);
        }

    lpDeleteAllInfo = (ProblemsFoundDoAllRec *) dwDeleteAllInfo;


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

                if (!DeleteFile(lpNoNav, lpInfected))
                    {
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

