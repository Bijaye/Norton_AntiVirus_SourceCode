// Copyright 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/FQUARANT.CPv   1.6   08 Jun 1998 18:56:20   RStanev  $
//
// Description:
//      This file defines Kernel functions for quarantining infected files.
//
// Contains:
//      QuarantineInfectedFile()
//
// See Also:
//      taskscan.cpp
//
// Note:
//      Kernel module
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apkernel.h"
#include "apquar.h"

//---------------------------------------------------------------------------
//
// BOOL QuarantineInfectedFile (
//     HEVENTPACKET hEventPacket,
//     PHVLVIRUS    phVLVirus
//     );
//
// This function quarantines an infected file.
//
// Entry:
//      hEventPacket - Event Packet handle
//      phVLVirus    - Pointer to a variable containing the handle to
//                     the virus detected in the file described by
//                     hEventPacket.
//
// Exit:
//      TRUE if operation successful, or FALSE if error.
//
//---------------------------------------------------------------------------
BOOL QuarantineInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    )
    {
                                        // Validate input parameters.
    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->pszBackupFile );
    SYM_ASSERT ( phVLVirus );
    SYM_ASSERT ( *phVLVirus );

    auto    BOOL  bSuccess;
    auto    HFILE hQuarFile;

    if ( *prAPCfg(hEventPacket)->pszQuarantineLocation  &&
         AddFileToQuarantine ( EDXpszOEMFile(hEventPacket->prEDExtra),
                               QFILE_STATUS_QUARANTINED,
                               prAPCfg(hEventPacket)->pszQuarantineLocation,
                               hEventPacket->prEDExtra->pszBackupFile,
                               &hQuarFile ) )
        {
        // Delete the source file.

        bSuccess = VS_OK == VLDeleteFile ( *phVLVirus );

        AddFileToQuarantineComp ( EDXpszOEMFile(hEventPacket->prEDExtra),
                                  hEventPacket->prEDExtra->pszBackupFile,
                                  hQuarFile,
                                  !bSuccess );

        hEventPacket->prEDExtra->bBackupFileCreated = bSuccess;

        return ( bSuccess );
        }

    return ( FALSE );
    }
