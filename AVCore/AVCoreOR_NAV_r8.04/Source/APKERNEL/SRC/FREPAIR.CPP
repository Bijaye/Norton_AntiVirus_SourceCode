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

#include "platform.h"
#include "xapi.h"
#include "navap.h"
#include "apkernel.h"
#include "apquar.h"

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

        prEDExtra->bBackupFileCreated = bSuccess;
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
