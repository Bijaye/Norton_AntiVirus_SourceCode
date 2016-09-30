// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_misc.cpv   1.9   11 Jun 1998 18:30:30   CEATON  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_misc.cpv  $
// 
//    Rev 1.9   11 Jun 1998 18:30:30   CEATON
// Added ability to disable file sharing based on registry key during file repairs.
// 
//    Rev 1.8   01 Jun 1998 16:06:58   tcashin
// Added support for quarantined compressed files.
// 
//    Rev 1.7   28 May 1998 21:31:56   TCASHIN
// Cleaned up some backup & restore stuff.
// 
//    Rev 1.6   22 May 1998 20:31:00   tcashin
// Make sure the quarantine item is released when we don't restore 
// the backup file.
// 
//    Rev 1.5   22 May 1998 20:05:40   tcashin
// Removed the quarantine item cleanup at the end of RepairFile() since
// this is done in restorefile().
// 
//    Rev 1.4   18 May 1998 22:42:06   tcashin
// Changed parameters in calls to CreateBackup() and RestoreFile() to
// match the changes in NAVXUTIL.
// 
//    Rev 1.3   31 May 1997 18:50:18   MKEATIN
// Added a missing ')'.
// 
//    Rev 1.2   31 May 1997 18:39:48   MKEATIN
// Passed NULL as the lpvCookie parameter in VirusScanRepairFile().
// 
//    Rev 1.1   02 May 1997 11:57:44   TIVANOV
// a new quaranrine case statement with UpdateFileStatus
//
//    Rev 1.0   06 Feb 1997 21:02:44   RFULLER
// Initial revision
//
//    Rev 1.5   10 Sep 1996 17:02:16   JBRENNA
// Changed RepairFile() to pass OEM names into CreateBackup() and
// RestoreFile(). Also, changed the SetFileApisTo* functions to use the macro
// versions. That way the SYM_WIN32 #ifdefs go away.
//
//    Rev 1.4   29 May 1996 12:44:56   RHAYES
// Merged additional OEM<->Ansi Luigi mod.'s.
//
//    Rev 1.3   22 May 1996 17:46:54   RHAYES
// Round # 2 of Dave Allee's OEM<->Ansi mod.'s in Luigi.
//
//    Rev 1.2   21 May 1996 15:06:04   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
//
//    Rev 1.1   14 Feb 1996 17:02:26   JWORDEN
// Process internal file names in OEM mode to allow scan, and repair, and
// innoculation of file names containing certain non-ANSI translatable
// characters.
//
//    Rev 1.0   30 Jan 1996 15:55:44   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:46   BARRY
// Initial revision.
//
//    Rev 1.12   14 Jan 1996 16:50:12   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.11   04 Nov 1995 15:15:36   DALLEE
// For NT: don't inoculate during scan.
//
//    Rev 1.10   09 May 1995 10:31:00   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "nonav.h"
#include "xapi.h"
#include "file.h"
#include "navutil.h"
#include "ctsn.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stub.h"
#include "virscan.h"
#include "inoc.h"

//************************************************************************
//  LOCAL FUNCTION PROTOTYPES
//************************************************************************


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

UINT WINAPI UpdateFileStatus (
    UINT    uPrevFileStatus,
    UINT    uProcess,
    BOOL    bCompletionStatus )
{
    auto    UINT    uStatus = FILESTATUS_NOT_USED;

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

        case PROCESS_QUARANTINE:
            switch (uPrevFileStatus)
                {
                case FILESTATUS_INFECTED:
                case FILESTATUS_NOTREPAIRED:
                    uStatus = (bCompletionStatus == TRUE) ?
                                FILESTATUS_QUARANTINE_INFECTED :
                                FILESTATUS_QUARANTINE_INFECTED_FAILED;
                    break;

                case FILESTATUS_NOTINOCULATED:
                case FILESTATUS_INOCULATIONFAILED:
                    uStatus = (bCompletionStatus == TRUE) ?
                                FILESTATUS_QUARANTINE_NOTINOCULATED :
                                FILESTATUS_QUARANTINE_NOTINOCULATED_FAILED;
                    break;

                case FILESTATUS_INOCULATEDCHANGED:
                case FILESTATUS_REINOCULATIONFAILED:
                case FILESTATUS_INOCULATEREPAIR_FAILED:
                    uStatus = (bCompletionStatus == TRUE) ?
                                FILESTATUS_QUARANTINE_INOCULATEDCHANGED :
                                FILESTATUS_QUARANTINE_INOCULATEDCHANGED_FAILED;
                    break;

                case FILESTATUS_ZIPINFECTED:
                    uStatus = (bCompletionStatus == TRUE) ?
                                FILESTATUS_QUARANTINE_INFECTED_ZIP :
                                FILESTATUS_QUARANTINE_INFECTED_FAILED;
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
                }
            break;
        }

    return (uStatus);
}

#define W95_ROOT_KEY   "SOFTWARE\\Symantec\\Norton Antivirus\\Auto-Protect\\InternalSettings"
#define NT_ROOT_KEY     "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Auto-Protect\\InternalSettings"
#define FILE_SHARING     "FileSharing"

BOOL IsAutoProtectFileSharing()

{
    HKEY hKey;
    LPSTR pKey;
    DWORD dwFileSharingEnabled,dwSize = sizeof( DWORD );

    if ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() )
        pKey = W95_ROOT_KEY;
    else
        pKey = NT_ROOT_KEY;

    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,pKey,0,KEY_QUERY_VALUE,&hKey ) != ERROR_SUCCESS )
        return( FALSE );

    if ( RegQueryValueEx( hKey,FILE_SHARING,NULL,NULL,( LPBYTE ) &dwFileSharingEnabled,&dwSize ) != ERROR_SUCCESS )
        return( FALSE );

    if ( dwFileSharingEnabled == 0 )
        return( TRUE );

    return( FALSE );
}

//************************************************************************
// RepairFile()
//
// This routine backs up (if specified) and attempts to repair a file.
// The file is restored if possible to the original state.
//
// (ANSI/OEM mess: Internally in OEM mode, except for Inoc section.
//
// Parameters:
//      LPREPAIRFILESTRUCT  lpRepair    Info needed to repair file.
//
// Returns:
//      TRUE                            Repair succeeded.
//      FALSE                           Repair failed.
//************************************************************************
// 9/07/93 DALLEE, Check for infection after inoc repairs.
//************************************************************************

BOOL WINAPI RepairFile ( LPREPAIRFILESTRUCT lpRepair )
{
    auto    BOOL        bStatus = FALSE;
    auto    char        szExt [ SYM_MAX_PATH + 1 ];
    auto    UINT        uOrigAttr;
    auto    int         iAttrErr;
    auto    BOOL        bApisWereAnsi = ARE_FILE_APIS_ANSI;
    auto    BOOL        bBackupCreatedOk = FALSE;

#ifdef WIN16
    TSR_OFF;
#endif

    if (bApisWereAnsi)
        SET_FILE_APIS_TO_OEM;

    iAttrErr = FileGetAttr (lpRepair->szFullOemPath, &uOrigAttr);
    if (NOERR == iAttrErr)
        iAttrErr = FileSetAttr (lpRepair->szFullOemPath, uOrigAttr);

    if ( IsAutoProtectFileSharing() == TRUE )
        FileEnableFileSharing( FALSE );

    if (NOERR == iAttrErr)
        {
#ifndef NAVSCANW                          // No file inoculation for Gargoyle.
        bStatus = FALSE;

        if ( lpRepair->bUseInoc == TRUE )
            {
            SET_FILE_APIS_TO_ANSI;

            uOrigAttr = SmartInocGetStatus();

            SmartInocOpen ( lpRepair->szFullPath, FALSE );

            if (IsInoculated(lpRepair->szFullPath) == INOC_CHANGED)
                {
                if ( lpRepair->bCreateBackup == TRUE )
                    {
                    STRCPY(szExt, lpRepair->szBackExt);
                    MEMSET(lpRepair->pQuarBackupItem, 0, sizeof(lpRepair->pQuarBackupItem));
                    CreateBackup (lpRepair);
                    }

                if (InocRepairFile(lpRepair->szFullPath))
                    {
                    auto    N30NOTES    N30Notes;

                    SET_FILE_APIS_TO_OEM;

                    bStatus = TRUE;

                    lpRepair->uVirusSigIndex = VirusScanFile(
                                                        lpRepair->szFullOemPath,
                                                        &N30Notes, TRUE);
                    SET_FILE_APIS_TO_ANSI;

                    bStatus = !lpRepair->uVirusSigIndex;
                    }

                if ((TRUE == lpRepair->bCreateBackup) && (FALSE == bStatus))
                    {
                    RestoreFile (lpRepair);
                    }
                }

            if ( uOrigAttr == SMARTOPEN_RW )
                SmartInocOpen ( lpRepair->szFullOemPath, TRUE );

            SET_FILE_APIS_TO_OEM;
            }

        if ( bStatus == FALSE )
#endif
            {
                                            // -------------------------------
                                            // Save contents since it could
                                            // be modified.
                                            // -------------------------------

            if ( lpRepair->bCreateBackup == TRUE)
                {
                STRCPY(szExt, lpRepair->szBackExt);
                bBackupCreatedOk = CreateBackup (lpRepair);
                }
            
            if (!lpRepair->bCreateBackup || bBackupCreatedOk)
                {            
                if ( VirusScanRepairFile (lpRepair->szFullOemPath,
                                         lpRepair->uVirusSigIndex,
                                         NULL))
                    {
                    bStatus = TRUE;
                    }
                }

            if (TRUE == lpRepair->bCreateBackup && bBackupCreatedOk)
                {
                if (FALSE == bStatus)
                    RestoreFile (lpRepair);
                }                    
            }

        }

#ifdef WIN16
    TSR_ON;
#endif

    if ( IsAutoProtectFileSharing() == TRUE )
        FileEnableFileSharing( TRUE );

    if (bApisWereAnsi)
        SET_FILE_APIS_TO_ANSI;

    return (bStatus);
}
