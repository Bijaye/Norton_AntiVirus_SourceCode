// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/exclude.c_v   1.0   06 Feb 1997 20:56:28   RFULLER  $
//
// Description:
//      These are the functions for handling exclusions during scanning and
//      from the Problems Found dialog after scanning.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/navc/VCS/exclude.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:28   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:26   MKEATIN
// Initial revision.
// 
//    Rev 1.2   07 Nov 1996 14:43:20   JBELDEN
// cleaned up and removed unused code
// 
//    Rev 1.1   31 Oct 1996 16:29:54   JBELDEN
// stubbed out dialog code
//
//    Rev 1.0   02 Oct 1996 12:59:06   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "stddos.h"
#include "ctsn.h"

#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "navdprot.h"
#include "exclude.h"

#include "scand.h"

//************************************************************************
// ScanExcludeInfectedFile()
//
// This routine excludes the specified file from the type of activity
// that the status flag indicates.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Info on file to exclude
//
// Returns:
//      TRUE                            File excluded
//      FALSE                           Not excluded.
//************************************************************************
// 5/12/93 DALLEE, Function created.
//************************************************************************

BOOL PASCAL ScanExcludeInfectedFile (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected)
{
    extern  NAVOPTIONS  navOptions;
    extern  BOOL        bUpdateExclusions;

    auto    UINT        uResult;
    auto    EXCLUDEITEM rExclude;

                                        // Clear the exclude record first.
    MEMSET(&rExclude, '\0', sizeof(EXCLUDEITEM));

    STRCPY(rExclude.szText, lpInfected->szFullPath);

    switch (lpInfected->wStatus)
        {
        case FILESTATUS_NOTINOCULATED:
        case FILESTATUS_INOCULATEDCHANGED:
        case FILESTATUS_INOCULATIONFAILED:
        case FILESTATUS_REINOCULATIONFAILED:
            rExclude.wBits = excINOC_CHANGE;
            break;

        case FILESTATUS_INFECTED:
        case FILESTATUS_NOTREPAIRED:
        case FILESTATUS_NOTDELETED:
            rExclude.wBits = excVIRUS_FOUND;
            break;

                                        // Default means I do not know from
        default:                        // what type of activity to exclude
            return (FALSE);             // the file.
        }

    uResult = ExcludeAdd(&navOptions.exclude, &rExclude);

    lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                           PROCESS_EXCLUDE,
                                           uResult);

                                        // If this took care of one of our
                                        // 'infected' files, add to our
                                        // 'cleaned' stats.
    if ((TRUE == uResult) && (excVIRUS_FOUND == rExclude.wBits))
        {
        lpNoNav->Stats.Files.uCleaned++;
        }

    bUpdateExclusions = TRUE;           // Remember to save exclusions on exit
    return (uResult);

} // End ScanExcludeInfectedFile()

