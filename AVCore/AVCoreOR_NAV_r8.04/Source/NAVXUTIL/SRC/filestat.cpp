// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navxutil/VCS/filestat.cpv   1.0   06 Feb 1997 21:05:28   RFULLER  $
//
// Description:
//      Code to update file statuses depending on action and result.
//      Also has code to build string of items scanned (?).
//
// Contains:
//      UpdateFileStatus()
//      BuildWhatScanned()
//
// See Also:
//************************************************************************
// $Log:   S:/navxutil/VCS/filestat.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:05:28   RFULLER
// Initial revision
// 
//    Rev 1.2   06 Sep 1996 17:26:12   PVENKAT
// Ported Gryphon changes.
// 
//          Rev 1.2   07 Aug 1996 09:26:14   PVENKAT
//      Fixed #63363.  Now we only pass the Full pathname instead of Shorten one 
//      to have ScanEnd Event report the complete pathname.
// 
// 
//    Rev 1.1   28 Jun 1996 12:03:02   MKEATIN
// Ported Changes from LuigiPlus
// 
//    Rev 1.0   30 Jan 1996 15:56:42   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:46   BARRY
// Initial revision.
// 
//    Rev 1.3   22 Dec 1995 17:05:46   GWESTER
// For PileDriver, added a check to make sure that a buffer doesn't overflow
// with file specs.
// 
//    Rev 1.2   12 Apr 1995 17:15:50   MFALLEN
// Use quake NameShortenFilename() instead of MakePathShorter()
// 
//    Rev 1.1   20 Dec 1994 17:20:50   MFALLEN
// symineg.h was removed from nonav.h include file and had to be included here.
// 
//    Rev 1.0   16 Dec 1994 10:37:10   MFALLEN
// Initial revision.
// 
//    Rev 1.6   15 Dec 1994 20:13:46   MFALLEN
// From NIRVANA
// 
//    Rev 1.5   01 Mar 1994 11:52:34   DALLEE
// Update filestatus of registration items for BANKERS.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "syminteg.h"
#include "nonav.h"

MODULE_NAME;

//************************************************************************
// UpdateFileStatus()
//
// Change the status of an item depending on action and success.
//
// Parameters:
//      UINT    uPrevFileStatus         Original state of item.
//      UINT    uProcess                Process identifier -- PROCESS_???
//      BOOL    bCompletionStatus       TRUE-success, FALSE-failure.
//
// Returns:
//      UINT                            New status of item.
//************************************************************************
//  2/25/94 DALLEE, support to update BANKERS registration statuses.
//************************************************************************

UINT SYM_EXPORT WINAPI UpdateFileStatus (
    UINT    uPrevFileStatus,
    UINT    uProcess,
    BOOL    bCompletionStatus
    )
{
    auto    UINT    uStatus;
     
    uStatus = uPrevFileStatus;          // Default to the previous status

    switch (uProcess)
        {
        case PROCESS_REPAIR:
            switch (uPrevFileStatus)
                {
                case FILESTATUS_INFECTED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_REPAIRED : FILESTATUS_NOTREPAIRED;
                    break;

                case FILESTATUS_INOCULATEDCHANGED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_INOCULATEREPAIRED :
                               FILESTATUS_INOCULATEREPAIR_FAILED;
                    break;

                case FILESTATUS_BOOTINFECTED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_BOOTREPAIRED :
                               FILESTATUS_BOOTNOTREPAIRED;
                    break;

                case FILESTATUS_MASTERBOOTINFECTED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_MASTERBOOTREPAIRED :
                               FILESTATUS_MASTERBOOTNOTREPAIRED;
                    break;
                }
            break;

        case PROCESS_DELETE:
            switch (uPrevFileStatus)
                {
                case FILESTATUS_INFECTED:
                case FILESTATUS_NOTREPAIRED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_DELETED :
                               FILESTATUS_NOTDELETED;
                    break;

                case FILESTATUS_NOTINOCULATED:
                case FILESTATUS_INOCULATIONFAILED:
                    uStatus = (bCompletionStatus == TRUE) ?
                                FILESTATUS_INOCULATEDELETED :
                                FILESTATUS_INOCULATEDELETE_FAILED;
                    break;

                case FILESTATUS_INOCULATEDCHANGED:
                case FILESTATUS_REINOCULATIONFAILED:
                case FILESTATUS_INOCULATEREPAIR_FAILED:
                    uStatus = (bCompletionStatus == TRUE) ?
                                FILESTATUS_INOC_CHANGED_DELETED :
                                FILESTATUS_INOC_CHANGED_NOTDELETED;
                    break;

                case FILESTATUS_ERROR:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_ERRORDELETED :
                               FILESTATUS_ERRORNOTDELETED;
                    break;
                }
            break;

        case PROCESS_INOCULATE:
            switch (uPrevFileStatus)
                {
                case FILESTATUS_NOTINOCULATED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_INOCULATED :
                               FILESTATUS_INOCULATIONFAILED;
                    break;

                case FILESTATUS_INOCULATEDCHANGED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_REINOCULATED :
                               FILESTATUS_REINOCULATIONFAILED;
                    break;
                }
            break;

        case PROCESS_EXCLUDE:
            switch (uPrevFileStatus)
                {
                case FILESTATUS_INFECTED:
                case FILESTATUS_NOTREPAIRED:
                case FILESTATUS_NOTDELETED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_EXCLUDE_VIRUSDETECTION :
                               FILESTATUS_EXCLUDE_FAILED;
                    break;

                case FILESTATUS_NOTINOCULATED:
                case FILESTATUS_INOCULATEDCHANGED:
                case FILESTATUS_INOCULATIONFAILED:
                case FILESTATUS_REINOCULATIONFAILED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_EXCLUDE_INOCULATION :
                               FILESTATUS_EXCLUDE_FAILED;
                    break;
#ifdef BANKERS
                case FILESTATUS_NOTREGISTERED:
                case FILESTATUS_REGISTRATIONFAILED:
                case FILESTATUS_REGISTEREDCHANGED:
                case FILESTATUS_REREGISTRATIONFAILED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_EXCLUDE_REGISTRATION:
                               FILESTATUS_EXCLUDE_FAILED;
                    break;
#endif  // BANKERS
                }
            break;
#ifdef BANKERS
        case PROCESS_REGISTER:
            switch (uPrevFileStatus)
                {
                case FILESTATUS_NOTREGISTERED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_REGISTERED:
                               FILESTATUS_REGISTRATIONFAILED;
                    break;
                case FILESTATUS_REGISTEREDCHANGED:
                    uStatus = (bCompletionStatus == TRUE) ?
                               FILESTATUS_REREGISTERED:
                               FILESTATUS_REREGISTRATIONFAILED;
                    break;
                }
            break;
#endif  // BANKERS
        }

    return (uStatus);
} // UpdateFileStatus()

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI BuildWhatScanned (
    LPSTR   lpScanList,
    LPSTR   lpScanned,
    UINT    uSize
    )
{
    #define SIZE_SCANITEMFLD 35
    #define MAX_FULLPATHS   (MAX_NET_PATH_SIZE * 50)

    auto    char    szOneEntry [ SYM_MAX_PATH+1 ];
    auto    BOOL    bDrives [ SYM_MAX_DOS_DRIVES ];
    auto    UINT    uMemOffset = 0;
    auto    UINT    i;
    auto    LPSTR   lpMem;
    auto    LPSTR   lpList;

    auto    DWORD   dwBytesCached = 0;  // Tracks number of bytes 
                                        // loaded into lpMem buffer

    MEMSET (&bDrives, EOS, sizeof(bDrives));
    *lpScanned = EOS;

    if ((lpMem = (LPSTR)MemAllocPtr(GHND, MAX_FULLPATHS)) != NULL)
        {
        lpList = lpScanList;

        STRCPY (szOneEntry, lpList);
        lpList += (STRLEN(lpList) + 1);
        uMemOffset = 0;

        while (*szOneEntry)
            {
            if ( STRLEN(szOneEntry) == 3 )         // Drive scanned
                {
                if ( szOneEntry[1] != '#' )
                    {
                    bDrives [(*szOneEntry - 'A')] = TRUE;
                    }
                }
            else
                {                                   // Make sure that we
                                                    // don't overflow the
                                                    // buffer:
                if (!((dwBytesCached + (STRLEN (szOneEntry)) + 2) >= MAX_FULLPATHS))
                    {
                    dwBytesCached += (STRLEN (szOneEntry) + 1);
                    STRCPY (&lpMem[uMemOffset], szOneEntry);
                    uMemOffset += (STRLEN(szOneEntry) + 1);
                    }
                }

        
            STRCPY (szOneEntry, lpList);
            lpList += (STRLEN(lpList) + 1);
            }

        for ( i = 0, uMemOffset = 0; i < SYM_MAX_DOS_DRIVES; i++ )
            {
            if ( bDrives[i] == TRUE )
                {
                szOneEntry [ uMemOffset++ ] = (i + 'A');
                }
            }

        szOneEntry [ uMemOffset ] = EOS;

        DiskGetDriveList (szOneEntry, lpScanned, uSize);

        lpList = lpMem;
        while (*lpList)
            {
            STRCPY (szOneEntry, lpList);

            //
            // Let us pass the complete path name instead of Shorten version.
            // As this is used for ScanEnd Events and only this event seems 
            // to have the Shorten file name instead of Full name.
            //
            // NameShortenFileName ( lpList, szOneEntry, 20 );

            if ((STRLEN(szOneEntry) + STRLEN(lpScanned) + 5) > uSize)
                break;

            STRCAT (lpScanned, szOneEntry);
            STRCAT (lpScanned, " ");
            lpList += (STRLEN(lpList) + 1);
            }

        MemFreePtr (lpMem);
        }
}

