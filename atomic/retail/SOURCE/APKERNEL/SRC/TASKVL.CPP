// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/TASKVL.CPv   1.5   01 Feb 1999 14:13:04   MBROWN  $
//
// Description:
//      This file defines functions for processing tasks related to
//      virus-like activity events.
//
// Contains:
//      TaskNotifyFileAttrChange()
//      TaskNotifyFileWrite()
//      TaskNotifyHDFormat()
//      TaskNotifyHDMBRWrite()
//      TaskNotifyHDBootWrite()
//      TaskNotifyFDBootWrite()
//
//      DeviceIsBootMBRChanging()
//
// See Also:
//      krnlapi.cpp
//
// Notes:
//      Kernel module
//
//      Supports SYM_VXD platform only.
//
//***************************************************************************
// $Log:   S:/APKERNEL/VCS/TASKVL.CPv  $
// 
//    Rev 1.5   01 Feb 1999 14:13:04   MBROWN
// Backed out of large sector fix for Phoenix
//
//    Rev 1.3   16 Mar 1998 17:59:00   DALLEE
// Match CORE change: Wider (DWORD size) CHS fields in ABSDISKREC.
//
//    Rev 1.2   17 Feb 1998 20:25:14   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 02:09:24   RStanev
// Minor changes.
//
//    Rev 1.0   26 Feb 1997 16:39:52   RSTANEV
// Initial revision.
//
//***************************************************************************

#if !defined(SYM_VXD)
#error This file supports only SYM_VXD!
#endif

#include "platform.h"
#include "disk.h"
#include "navap.h"
#include "apkernel.h"

//---------------------------------------------------------------------------
//
// Local function prototypes.
//
//---------------------------------------------------------------------------
BOOL DeviceIsBootMBRChanging (
    HEVENTPACKET hEventPacket
    );

//---------------------------------------------------------------------------
//
// BOOL TaskNotifyFileAttrChange (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_NOTIFY_FILEATTR_CHANGE task.  It sets up
// the generic Virus-Like activity UI for servicing this task.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//            hEventPacket members set so that the event callback
//            knows what steps to take from now on.
//
//---------------------------------------------------------------------------
BOOL TaskNotifyFileAttrChange (
    HEVENTPACKET hEventPacket
    )
    {
    auto AVACTION     uAVAction;
    auto AVACTIONDATA rAVActionData;
    auto BOOL         bActionCompleted = FALSE;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // Check if owner is excluded.

    uAVAction = NAVAPIsFileExcluded (
                    hEventPacket,
                    EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                    excRO_CHANGE ) ?
                AVACTION_LAST : AVACTION_FIRST;

    while ( uAVAction != AVACTION_LAST )
        {

                                        // Get an action to take.

        uAVAction = UIVirusLikeActivity ( hEventPacket,
                                          AVCONTEXT_FILEROATTRCHANGE,
                                          uAVAction,
                                          bActionCompleted,
                                          &rAVActionData );

                                        // Assume the action will complete.

        bActionCompleted = TRUE;

                                        // Take the action.

        switch ( uAVAction )
            {
            case AVACTION_STOP:

                EventPacketSetResult ( hEventPacket, FALSE );

                break;

            case AVACTION_CONTINUE:

                EventPacketSetResult ( hEventPacket, TRUE );

                break;

            case AVACTION_EXCLUDE:

                if ( NAVAPExcludeFile (
                         hEventPacket,
                         EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                         excRO_CHANGE ) )
                    {
                    EventPacketSetResult ( hEventPacket, TRUE );
                    }
                else
                    {
                    bActionCompleted = FALSE;
                    }

                break;

            case AVACTION_FIRST:
            case AVACTION_LAST:

                break;

            default:

                SYM_ASSERT ( FALSE );

                bActionCompleted = FALSE;
            }
        }

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL TaskNotifyFileWrite (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_NOTIFY_FILE_WRITE task.  It sets up
// the generic Virus-Like activity UI for servicing this task.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//            hEventPacket members set so that the event callback
//            knows what steps to take from now on.
//
//---------------------------------------------------------------------------
BOOL TaskNotifyFileWrite (
    HEVENTPACKET hEventPacket
    )
    {
    auto AVACTION     uAVAction;
    auto AVACTIONDATA rAVActionData;
    auto BOOL         bActionCompleted = FALSE;
    auto BOOL         bAllowWriting;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // Check if owner is excluded.

    if ( NAVAPIsFileExcluded ( hEventPacket,
                               EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                               excWR_ATLIST )
       )
        {
        bAllowWriting = TRUE;

        uAVAction = AVACTION_LAST;
        }
    else
        {
        bAllowWriting = FALSE;

        uAVAction = AVACTION_FIRST;
        }

    while ( uAVAction != AVACTION_LAST )
        {

                                        // Get an action to take.

        uAVAction = UIVirusLikeActivity ( hEventPacket,
                                          AVCONTEXT_WRITETOFILE,
                                          uAVAction,
                                          bActionCompleted,
                                          &rAVActionData );

                                        // Assume the action will complete.

        bActionCompleted = TRUE;

                                        // Take the action.

        switch ( uAVAction )
            {
            case AVACTION_STOP:

                EventPacketSetResult ( hEventPacket, FALSE );

                bAllowWriting = FALSE;

                break;

            case AVACTION_CONTINUE:

                EventPacketSetResult ( hEventPacket, TRUE );

                bAllowWriting = TRUE;

                break;

            case AVACTION_EXCLUDE:

                if ( NAVAPExcludeFile (
                         hEventPacket,
                         EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                         excWR_ATLIST ) )
                    {
                    EventPacketSetResult ( hEventPacket, TRUE );
                    }
                else
                    {
                    bActionCompleted = FALSE;
                    }

                bAllowWriting = TRUE;

                break;

            case AVACTION_FIRST:
            case AVACTION_LAST:

                break;

            default:

                SYM_ASSERT ( FALSE );

                bActionCompleted = FALSE;
            }
        }

                                        // If writing allowed, then do not
                                        // warn anymore.

    if ( bAllowWriting )
        {
        FileMonitorClearAttributes ( hEventPacket,
                                     FILEMONITOR_CONFIRMWRITE );
        }

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL TaskNotifyHDFormat (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_NOTIFY_HDFORMAT task.  It sets up
// the generic Virus-Like activity UI for servicing this task.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//            hEventPacket members set so that the event callback
//            knows what steps to take from now on.
//
//---------------------------------------------------------------------------
BOOL TaskNotifyHDFormat (
    HEVENTPACKET hEventPacket
    )
    {
    auto AVACTION     uAVAction;
    auto AVACTIONDATA rAVActionData;
    auto BOOL         bActionCompleted = FALSE;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // Check if owner is excluded.

    uAVAction = NAVAPIsFileExcluded (
                    hEventPacket,
                    EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                    excLOW_FORMAT ) ?
                AVACTION_LAST : AVACTION_FIRST;

    while ( uAVAction != AVACTION_LAST )
        {

                                        // Get an action to take.

        uAVAction = UIVirusLikeActivity ( hEventPacket,
                                          AVCONTEXT_HDFORMAT,
                                          uAVAction,
                                          bActionCompleted,
                                          &rAVActionData );

                                        // Assume the action will complete.

        bActionCompleted = TRUE;

                                        // Take the action.

        switch ( uAVAction )
            {
            case AVACTION_STOP:

                EventPacketSetResult ( hEventPacket, FALSE );

                break;

            case AVACTION_CONTINUE:

                EventPacketSetResult ( hEventPacket, TRUE );

                break;

            case AVACTION_EXCLUDE:

                if ( NAVAPExcludeFile (
                         hEventPacket,
                         EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                         excLOW_FORMAT ) )
                    {
                    EventPacketSetResult ( hEventPacket, TRUE );
                    }
                else
                    {
                    bActionCompleted = FALSE;
                    }

                break;

            case AVACTION_FIRST:
            case AVACTION_LAST:

                break;

            default:

                SYM_ASSERT ( FALSE );

                bActionCompleted = FALSE;
            }
        }

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL TaskNotifyHDMBRWrite (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_NOTIFY_HDMBR_WRITE task.  It sets up
// the generic Virus-Like activity UI for servicing this task.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//            hEventPacket members set so that the event callback
//            knows what steps to take from now on.
//
//---------------------------------------------------------------------------
BOOL TaskNotifyHDMBRWrite (
    HEVENTPACKET hEventPacket
    )
    {
    auto AVACTION     uAVAction;
    auto AVACTIONDATA rAVActionData;
    auto BOOL         bActionCompleted = FALSE;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // Check if MBR is really changing.

    if ( !DeviceIsBootMBRChanging ( hEventPacket ) )
        {
        return ( TRUE );
        }

                                        // Check if owner is excluded.

    uAVAction = NAVAPIsFileExcluded (
                    hEventPacket,
                    EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                    excWR_HBOOT ) ?
                AVACTION_LAST : AVACTION_FIRST;

    while ( uAVAction != AVACTION_LAST )
        {

                                        // Get an action to take.

        uAVAction = UIVirusLikeActivity ( hEventPacket,
                                          AVCONTEXT_HDMBRWRITE,
                                          uAVAction,
                                          bActionCompleted,
                                          &rAVActionData );

                                        // Assume the action will complete.

        bActionCompleted = TRUE;

                                        // Take the action.

        switch ( uAVAction )
            {
            case AVACTION_STOP:

                EventPacketSetResult ( hEventPacket, FALSE );

                break;

            case AVACTION_CONTINUE:

                EventPacketSetResult ( hEventPacket, TRUE );

                break;

            case AVACTION_EXCLUDE:

                if ( NAVAPExcludeFile (
                         hEventPacket,
                         EDXpszOEMOwner(hEventPacket->prEDExtra),
                         excWR_HBOOT ) )
                    {
                    EventPacketSetResult ( hEventPacket, TRUE );
                    }
                else
                    {
                    bActionCompleted = FALSE;
                    }

                break;

            case AVACTION_FIRST:
            case AVACTION_LAST:

                break;

            default:

                SYM_ASSERT ( FALSE );

                bActionCompleted = FALSE;
            }
        }

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL TaskNotifyHDBootWrite (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_NOTIFY_HDBOOT_WRITE task.  It sets up
// the generic Virus-Like activity UI for servicing this task.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//            hEventPacket members set so that the event callback
//            knows what steps to take from now on.
//
//---------------------------------------------------------------------------
BOOL TaskNotifyHDBootWrite (
    HEVENTPACKET hEventPacket
    )
    {
    auto AVACTION     uAVAction;
    auto AVACTIONDATA rAVActionData;
    auto BOOL         bActionCompleted = FALSE;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // Check if boot sector is really
                                        // changing.

    if ( !DeviceIsBootMBRChanging ( hEventPacket ) )
        {
        return ( TRUE );
        }

                                        // Check if owner is excluded.

    uAVAction = NAVAPIsFileExcluded (
                    hEventPacket,
                    EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                    excWR_HBOOT ) ?
                AVACTION_LAST : AVACTION_FIRST;

    while ( uAVAction != AVACTION_LAST )
        {

                                        // Get an action to take.

        uAVAction = UIVirusLikeActivity ( hEventPacket,
                                          AVCONTEXT_HDBOOTWRITE,
                                          uAVAction,
                                          bActionCompleted,
                                          &rAVActionData );

                                        // Assume the action will complete.

        bActionCompleted = TRUE;

                                        // Take the action.

        switch ( uAVAction )
            {
            case AVACTION_STOP:

                EventPacketSetResult ( hEventPacket, FALSE );

                break;

            case AVACTION_CONTINUE:

                EventPacketSetResult ( hEventPacket, TRUE );

                break;

            case AVACTION_EXCLUDE:

                if ( NAVAPExcludeFile (
                         hEventPacket,
                         EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                         excWR_HBOOT ) )
                    {
                    EventPacketSetResult ( hEventPacket, TRUE );
                    }
                else
                    {
                    bActionCompleted = FALSE;
                    }

                break;

            case AVACTION_FIRST:
            case AVACTION_LAST:

                break;

            default:

                SYM_ASSERT ( FALSE );

                bActionCompleted = FALSE;
            }
        }

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL TaskNotifyFDBootWrite (
//      HEVENTPACKET hEventPacket
//      );
//
// This function services the TASK_NOTIFY_FDBOOT_WRITE task.  It sets up
// the generic Virus-Like activity UI for servicing this task.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      always TRUE
//            hEventPacket members set so that the event callback
//            knows what steps to take from now on.
//
//---------------------------------------------------------------------------
BOOL TaskNotifyFDBootWrite (
    HEVENTPACKET hEventPacket
    )
    {
    auto AVACTION     uAVAction;
    auto AVACTIONDATA rAVActionData;
    auto BOOL         bActionCompleted = FALSE;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

                                        // Check if boot sector is really
                                        // changing.

    if ( !DeviceIsBootMBRChanging ( hEventPacket ) )
        {
        return ( TRUE );
        }

                                        // Check if owner is excluded.

    uAVAction = NAVAPIsFileExcluded (
                    hEventPacket,
                    EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                    excWR_FBOOT ) ?
                AVACTION_LAST : AVACTION_FIRST;

    while ( uAVAction != AVACTION_LAST )
        {

                                        // Get an action to take.

        uAVAction = UIVirusLikeActivity ( hEventPacket,
                                          AVCONTEXT_FDBOOTWRITE,
                                          uAVAction,
                                          bActionCompleted,
                                          &rAVActionData );

                                        // Assume the action will complete.

        bActionCompleted = TRUE;

                                        // Take the action.

        switch ( uAVAction )
            {
            case AVACTION_STOP:

                EventPacketSetResult ( hEventPacket, FALSE );

                break;

            case AVACTION_CONTINUE:

                EventPacketSetResult ( hEventPacket, TRUE );

                break;

            case AVACTION_EXCLUDE:

                if ( NAVAPExcludeFile (
                         hEventPacket,
                         EDXpszOEMOwner ( hEventPacket->prEDExtra ),
                         excWR_FBOOT ) )
                    {
                    EventPacketSetResult ( hEventPacket, TRUE );
                    }
                else
                    {
                    bActionCompleted = FALSE;
                    }

                break;

            case AVACTION_FIRST:
            case AVACTION_LAST:

                break;

            default:

                SYM_ASSERT ( FALSE );

                bActionCompleted = FALSE;
            }
        }

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL DeviceIsBootMBRChanging (
//      HEVENTPACKET hEventPacket
//      );
//
// This function checks whether a write to the boot/MBR sector of a drive
// really changes the sector.  This function will return FALSE only if
// if the volume label/OEM ID in the boot sector is changing, or if the new
// boot/MBR is the same as the one already on the disk.
//
// Entry:
//      hEventPacket - Event Packet handle
//
// Exit:
//      TRUE  if the boot sector is really changing, or error
//      FALSE if the boot sector's volume/OEM ID is only changing, or no
//            change at all
//
// Note:
//      VOL_OFFSET and VOL_LENGTH represent the location of the volume
//      label in a boot sector.
//      OEMID_OFFSET and OEMID_LENGTH represent the location of the OEM ID
//      string in the boot sector.
//
//---------------------------------------------------------------------------
#define VOL_OFFSET   0x2B
#define VOL_LENGTH   11
#define OEMID_OFFSET 0x03
#define OEMID_LENGTH 8

BOOL DeviceIsBootMBRChanging (
    HEVENTPACKET hEventPacket
    )
    {
    auto BOOL       bResult;
    auto ABSDISKREC rAbsDiskRec;
    auto PVOID      pvReadBuffer;
    auto UINT       uSectorLength, uTempCounter;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->prSEEventData );
    SYM_ASSERT ( hEventPacket->prEDExtra->prSEEventData->SE_ED_lpDTA );

    if ( !hEventPacket->prEDExtra->prSEEventData->SE_ED_lpDTA )
        {
        return ( FALSE );
        }

                                        // Initialize local variables.

    bResult = TRUE;

                                        // Allocate buffer to read boot/MBR.

    pvReadBuffer = MemAllocPtr ( GHND, 0x400 );

    if ( !pvReadBuffer )
        {
        UIOutOfMemoryError();

        return ( FALSE );
        }

                                        // Read the boot or MBR.

    memset ( &rAbsDiskRec, 0, sizeof(rAbsDiskRec) );

    rAbsDiskRec.dn = hEventPacket->prEDExtra->uPhysicalDevice;

    DiskGetPhysicalInfo ( &rAbsDiskRec );

    rAbsDiskRec.dwHead      = 0;
    rAbsDiskRec.dwTrack     = 0;
    rAbsDiskRec.dwSector    = 1;
    rAbsDiskRec.numSectors  = 1;
    rAbsDiskRec.buffer      = (PBYTE)pvReadBuffer;

    if ( !hEventPacket->prEDExtra->bMBRAccess )
        {
        ConvertLongToPhySector ( &rAbsDiskRec,
                                 hEventPacket->prEDExtra->uSectorLocation,
                                 &rAbsDiskRec.dwHead,
                                 &rAbsDiskRec.dwTrack,
                                 &rAbsDiskRec.dwSector );
        }

    if ( DiskAbsOperation ( READ_COMMAND, &rAbsDiskRec ) != 0 )
        {
        bResult = FALSE;
        }

                                        // If original sector read, then
                                        // compare to the new one.

    if ( bResult )
        {
                                        // If write to the boot sector, then
                                        // make the volume label/OEM ID
                                        // fields in the source & destination
                                        // buffers equal.

        if ( !hEventPacket->prEDExtra->bMBRAccess )
            {
            memcpy ( (PVOID) ((UINT)pvReadBuffer + VOL_OFFSET),
                     (PVOID) ((UINT)hEventPacket->prEDExtra->prSEEventData->SE_ED_lpDTA + VOL_OFFSET),
                     VOL_LENGTH );

            memcpy ( (PVOID) ((UINT)pvReadBuffer + OEMID_OFFSET),
                     (PVOID) ((UINT)hEventPacket->prEDExtra->prSEEventData->SE_ED_lpDTA + OEMID_OFFSET),
                     OEMID_LENGTH );
            }

                                        // Calculate sector length.

        uSectorLength = 0x80;

        for ( uTempCounter = rAbsDiskRec.sectorLenCode;
              uTempCounter;
              uTempCounter-- )
            {
            uSectorLength <<= 1;
            }

                                        // Compare both sectors.

        bResult = memcmp ( pvReadBuffer,
                           hEventPacket->prEDExtra->prSEEventData->SE_ED_lpDTA,
                           uSectorLength ) == 0 ? FALSE : TRUE;
        }

    MemFreePtr ( pvReadBuffer );

    return ( bResult );
    }
