// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/FREPAIR.CPv   1.12   08 Jun 1998 21:10:44   RStanev  $
//
// Description:
//      This file defines Kernel functions for repairing files.
//
// Contains:
//      BackupFileBeforeRepair()
//      ProcessBackupFileAfterRepair()
//      RepairInfectedFile()
//
// See Also:
//      taskscan.cpp
//
// Note:
//      Kernel module
//
//***************************************************************************
// $Log:   S:/APKERNEL/VCS/FREPAIR.CPv  $
// 
//    Rev 1.12   08 Jun 1998 21:10:44   RStanev
// Removed references to pszBackupExtension.
//
//    Rev 1.11   08 Jun 1998 21:06:00   RStanev
// Backup files are now created by quarantine.
//
//    Rev 1.10   20 Feb 1998 21:11:54   RStanev
// Fixed mishandling of virus handles when repairing files.
//
//    Rev 1.9   18 Feb 1998 21:46:32   RStanev
// Not including krnlstrs.h anymore.
//
//    Rev 1.8   18 Feb 1998 21:40:46   RStanev
// Removed unknown virus detection.
//
//    Rev 1.7   17 Feb 1998 20:25:04   RStanev
// Updated copyright.
//
//    Rev 1.6   12 Aug 1997 17:55:48   MDUNN
// Now wiping the backup file after a failed repair so the backup (which is
// infected) doesn't get saved by Smartcan.
//
//    Rev 1.5   14 Jun 1997 17:10:38   RStanev
// Fixed a problem in RepairInfectedFile() when VLRepairFile() returned
// VS_CANT_REPAIR.
//
//    Rev 1.4   02 Jun 1997 10:11:32   RStanev
// Added support for MESSAGE_BOX_COOKIE.
//
//    Rev 1.3   01 Jun 1997 17:19:56   RStanev
// Updated to match changes in the prototype of VLRepairFile().
//
//    Rev 1.2   15 May 1997 01:53:44   RStanev
// Minor changes.
//
//    Rev 1.1   29 Apr 1997 12:04:22   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   26 Feb 1997 16:39:44   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "xapi.h"
#include "navap.h"
#include "apkernel.h"
#include "quaradd.h"

//---------------------------------------------------------------------------
//
// VOID BackupFileBeforeRepair (
//     HEVENTPACKET hEventPacket
//     );
//
// This function creates a backup of a file.  Note, that after this function
// is called, you must call ProcessBackupFileAfterRepair() to either confirm
// the creation of the backup file or to destroy it.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      -
//          hEventPacket members set to represent the status of the
//          operation.
//
//---------------------------------------------------------------------------
VOID BackupFileBeforeRepair (
    HEVENTPACKET hEventPacket
    )
    {
    auto PEDEXTRA prEDExtra;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->pszBackupFile );

                                        // Initialize local variables.

    prEDExtra = hEventPacket->prEDExtra;

                                        // Create backup file.

#if defined(SYM_VXD)
    if ( prAPCfg(prEDExtra)->bBackupBeforeRepair &&
         prEDExtra->uFileCharSet != CHARSET_UNICODE )
#else
    if ( prAPCfg(prEDExtra)->bBackupBeforeRepair )
#endif
        {
        SYM_ASSERT ( sizeof(HFILE) <= sizeof(prEDExtra->hBackupFile) );

        prEDExtra->bBackupFileCreated = AddFileToQuarantine ( EDXpszOEMFile(prEDExtra),
                                                              QFILE_STATUS_BACKUP_FILE,
                                                              prAPCfg(hEventPacket)->pszQuarantineLocation,
                                                              prEDExtra->pszBackupFile,
                                                              (PHFILE)&prEDExtra->hBackupFile );
        }
    else
        {
        prEDExtra->bBackupFileCreated = FALSE;
        }
    }


//---------------------------------------------------------------------------
//
// VOID ProcessBackupFileAfterRepair (
//     HEVENTPACKET hEventPacket,
//     BOOL         bSuccess
//     );
//
// This function restores a file from a backup file, if one was created
// earlier by calling BackupFileBeforeRepair().
//
// Entry:
//      hEventPacket - Event Packet handle
//      bSuccess     - TRUE if the backup file should be left where it is,
//                     FALSE, if the original file should be restored back
//                     from the backup file and the backup file should be
//                     deleted.
//
// Exit:
//      -
//
// Note:
//      If error was encountered, this function will not notify the caller
//
//---------------------------------------------------------------------------
VOID ProcessBackupFileAfterRepair (
    HEVENTPACKET hEventPacket,
    BOOL         bSuccess
    )
    {
    auto PEDEXTRA prEDExtra;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->pszBackupFile );

                                        // Initialize local variables.

    prEDExtra = hEventPacket->prEDExtra;

                                        // Restore file.

    if ( prEDExtra->bBackupFileCreated )
        {
        AddFileToQuarantineComp ( EDXpszOEMFile(hEventPacket->prEDExtra),
                                  prEDExtra->pszBackupFile,
                                  (HFILE)prEDExtra->hBackupFile,
                                  !bSuccess );
        }
    }


//---------------------------------------------------------------------------
//
// BOOL RepairInfectedFile (
//      HEVENTPACKET hEventPacket,
//      PHVLVIRUS    phVLVirus
//      );
//
// This function recursively repairs an infected file using the standard
// AVAPI functions.
//
// Entry:
//      hEventPacket - Event Packet handle
//      phVLVirus    - pointer to a handle to a virus which had infected
//                     the file.
//
// Exit:
//      TRUE if repair succeeded, FALSE if repair failed.  If TRUE,
//      *phVLVirus will be set to contain the handle of the last repaired
//      virus (if recursive repair was not done, then the handle will not
//      be changed).  If FALSE, *phVLVirus will contain the handle of the
//      top-level virus currently infecting the file.
//
//---------------------------------------------------------------------------
BOOL RepairInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    )
    {
    auto UINT               uRecursiveRepairCount;
    auto HVLVIRUS           hVLVirus;
    auto VSTATUS            uVStatus;
    auto MESSAGE_BOX_COOKIE rCBMsgBox;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->hVGlobalContext );
    SYM_ASSERT ( phVLVirus );
    SYM_ASSERT ( *phVLVirus );

                                        // Setup UI for VLRepairFile().

    rCBMsgBox.bUserInterventionAllowed = hEventPacket->prEDExtra->bUserIntervention;

    rCBMsgBox.uTimeOut = prAPCfg(hEventPacket)->uAlertTimeOut;

                                        // Set up recursive repairs.

    uRecursiveRepairCount = MAX_RECURSIVE_FILE_REPAIRS;

                                        // Start repair with the current
                                        // virus handle.

    hVLVirus = *phVLVirus;

                                        // Recursively repair the file,
                                        // until the file is clean or until
                                        // the max number of repairs has been
                                        // reached.  Exit the loop with error
                                        // if a critical condition is
                                        // detected.

    while ( hVLVirus && uRecursiveRepairCount-- )
        {
                                        // Repair the file.

        uVStatus = VLRepairFile ( hVLVirus, &rCBMsgBox );

                                        // Examine the status of the repair.

        switch ( uVStatus )
            {
            case VS_MEMALLOC:

                UIOutOfMemoryError();

            case VS_CANT_REPAIR:

                return ( FALSE );
            }

                                        // Rescan the file.

        uVStatus = VLScanFile ( hEventPacket->prEDExtra->hVGlobalContext,
                                hEventPacket->prEDExtra->pszFile,
                                &hVLVirus );

                                        // If error, then return.

        if ( uVStatus != VS_OK )
            {
            if ( uVStatus == VS_MEMALLOC )
                {
                UIOutOfMemoryError();
                }

            return ( FALSE );
            }

                                        // If we have a new handle, then we
                                        // can update the original handle
                                        // at phVLVirus.

        if ( hVLVirus )
            {
            if ( VLReleaseVirusHandle ( *phVLVirus ) != VS_OK )
                {
                SYM_ASSERT ( FALSE );
                }

            *phVLVirus = hVLVirus;
            }
        }

    return ( hVLVirus ? FALSE : TRUE );
    }
