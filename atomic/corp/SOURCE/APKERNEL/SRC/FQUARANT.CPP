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
// $Log:   S:/APKERNEL/VCS/FQUARANT.CPv  $
// 
//    Rev 1.6   08 Jun 1998 18:56:20   RStanev
// Oops.
//
//    Rev 1.5   08 Jun 1998 18:39:10   RStanev
// If the deletion of a file during quarantine fails, then we will fail the
// quarantine.
//
//    Rev 1.4   08 Jun 1998 18:23:42   RStanev
// Split AddFileToQuarantine() into two functions.
//
//    Rev 1.3   12 May 1998 17:33:22   DALLEE
// New parameter in call to AddFileToQuarantine().
// dwStatus set to indicate quarantine ( vs. back up ).
//
//    Rev 1.2   27 Apr 1998 13:44:40   DALLEE
// Implemented QuarantineInfectedFile() with new AddFileToQuarantine()
// function.
//
//    Rev 1.1   01 Apr 1998 16:56:22   DALLEE
// Commented out VLQuarantineFile() call.
// Need to replace it with new non-AVAPI function to be provided real soon now.
//
//    Rev 1.0   19 Feb 1998 16:09:52   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apkernel.h"
#include "QuarAdd.h"

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
    SYM_ASSERT ( phVLVirus );
    SYM_ASSERT ( *phVLVirus );

    auto    BOOL    bSuccess;
    auto    TCHAR   szQuarFile[MAX_QUARANTINE_FILENAME_LEN];
    auto    HFILE   hQuarFile;

    if ( *prAPCfg(hEventPacket)->pszQuarantineLocation  &&
         AddFileToQuarantine ( EDXpszOEMFile(hEventPacket->prEDExtra),
                               QFILE_STATUS_QUARANTINED,
                               prAPCfg(hEventPacket)->pszQuarantineLocation,
                               szQuarFile,
                               &hQuarFile ) )
        {
        // Delete the source file.

        bSuccess = VS_OK == VLDeleteFile ( *phVLVirus );

        AddFileToQuarantineComp ( EDXpszOEMFile(hEventPacket->prEDExtra),
                                  szQuarFile,
                                  hQuarFile,
                                  !bSuccess );

        return ( bSuccess );
        }

    return ( FALSE );
    }
