// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/EDEXTRA.CPv   1.2   17 Feb 1998 16:50:58   RStanev  $
//
// Description:
//      This file contains functions for working with the EDEXTRA
//      structure.
//
// Contains:
//      InitEDExtra()
//      CleanUpEDExtra()
//      ProcessEDExtra()
//
//      EDXSetbFileIsExecutable()
//      EDXSetbAltFileIsExecutable()
//      EDXSetpszOwner()
//      EDXSetpszOEMFile()                  (SYM_VXD only)
//      EDXSetpszAltOEMFile()               (SYM_VXD only)
//      EDXSetpszOEMDevice()                (SYM_VXD only)
//      EDXSetpszOEMOwner()                 (SYM_VXD only)
//
// See Also:
//      apevent.h, evpacket.cpp, evtasks.cpp
//
// Notes:
//      Event Manager module.
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/EDEXTRA.CPv  $
// 
//    Rev 1.2   17 Feb 1998 16:50:58   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 01:09:36   RStanev
// Too many changes to describe.
//
//    Rev 1.0   26 Feb 1997 04:34:12   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apevent.h"
#include "evstrs.h"
#include "filter.h"

//---------------------------------------------------------------------------
//
// Define local constants and macros
//
//---------------------------------------------------------------------------
#define MAX_DEVICE_NAME_LENGTH          2

#define ALIGN4(x)                       ( (x) & 3 ? ((x) + 4) & (~3) : (x) )

//---------------------------------------------------------------------------
//
// BOOL InitEDExtra (
//      PEDEXTRA prEDExtra,
//      REFDATA  uRefData,
//      LPED     pEventData
//      );
//
// This function initializes the *prEDExtra structure with extended event
// data based on the event notification data at *pEventData.
//
// Entry:
//      prEDExtra  - pointer to EDEXTRA structure to be initialized
//      uRefData   - uRefData parameter passed to EventObjectCreate()
//      pEventData - pointer to event structure passed by SYMEVENT
//
// Exit:
//      TRUE  if *prEDExtra initialization succeeded
//            Once this function returns TRUE, there will be a memory
//            block allocated for the use of this structure.  When the
//            user does not need *prEDExtra anymore, this memory block
//            should be deallocated by calling CleanUpEDExtra().
//      FALSE if *prEDExtra initialization failed
//            *prEDExtra members may be destroyed
//
// Note:
//      This function should not attempt to do any file operations with
//      the input parameters.  The only allowed functions are string and
//      memory management.  This is because the check for reserved
//      OS filenames has not been done yet, and messing around with some
//      of them (SYM_VXD: "NETQ") causes problems.
//
//---------------------------------------------------------------------------
BOOL InitEDExtra (
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     pEventData
    )
    {
    auto BOOL          bNeedFile, bNeedAltFile;
    auto BOOL          bNeedDevice, bNeedOwner;
#if defined(SYM_VXD)
    auto BOOL          bNeedOEMFile, bNeedOEMAltFile;
    auto BOOL          bNeedOEMDevice, bNeedOEMOwner;
    auto UINT          uEDCharSet;
#endif // #if defined(SYM_VXD)
    auto BOOL          bNeedBackupFile;
    auto UINT          uBufferOffset;
    auto DEVICEINFOREC rDeviceInfo;

                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );
    SYM_ASSERT ( pEventData );

                                        // Initialize local variables and
                                        // clear *prEDExtra.

    bNeedFile       = FALSE;
    bNeedAltFile    = FALSE;
    bNeedDevice     = FALSE;
    bNeedOwner      = FALSE;
    bNeedBackupFile = FALSE;

#if defined(SYM_VXD)
    bNeedOEMFile    = FALSE;
    bNeedOEMAltFile = FALSE;
    bNeedOEMDevice  = FALSE;
    bNeedOEMOwner   = FALSE;
#endif // #if defined(SYM_VXD)

    uBufferOffset = 0;

    memset ( prEDExtra, 0, sizeof(*prEDExtra) );

                                        // Initialize global EDEXTRA
                                        // members.

    prEDExtra->prSEEventData   = pEventData;
    prEDExtra->uEventType      = pEventData->SE_ED_etEventType;
    prEDExtra->bEventBack      = pEventData->SE_ED_efEventFlags &
                                 SE_FLAGS_COMPLETION ? TRUE : FALSE;
    prEDExtra->uFileAttributes = pEventData->SE_ED_dwAttribute;

#if defined(SYM_VXD)
                                        // Determine current thread's
                                        // character set.

    if ( bVMMIs3x )
        {
        prEDExtra->uFileCharSet = CHARSET_OEM;
        }
    else
        {
        prEDExtra->uFileCharSet = VxDGetCurrentCHARSET();
        }

                                        // Determine the character set of
                                        // the filenames passed to us by
                                        // SYMEvent.

                                        // ~~~TBD~~~ SYMEvent is supposed to
                                        // tell us about the character sets.

    uEDCharSet = pEventData->SE_ED_etEventType != SE_LDR_APPLICATION_EXECUTE ?
                 prEDExtra->uFileCharSet :
                 ( Get_Cur_VM_Handle() == Get_Sys_VM_Handle() ? CHARSET_ANSI :
                                                                CHARSET_OEM );
#endif // #if defined(SYM_VXD)

                                        // Determine what EDEXTRA buffers
                                        // will be needed.

                                        // Note: The following routine can
                                        // be implemented much faster by
                                        // asking the event callback to pass
                                        // flags with a list of buffers
                                        // needed.

    switch ( prEDExtra->uEventType )
        {
        case SE_LDR_APPLICATION_EXECUTE:
        case SE_FS_FILE_CREATE:

#if defined(SYM_VXD)
            bNeedOEMFile    = TRUE;
#endif // #if defined(SYM_VXD)
            bNeedFile       = TRUE;
            bNeedBackupFile = TRUE;

            break;

        case SE_FS_OBJECT_RENAME:

#if defined(SYM_VXD)
            bNeedOEMFile    = TRUE;
            bNeedOEMAltFile = TRUE;
#endif // #if defined(SYM_VXD)
            bNeedFile       = TRUE;
            bNeedAltFile    = TRUE;
            bNeedBackupFile = TRUE;

            break;

        case SE_FS_FILE_OPEN:
        case SE_FS_FILE_CLOSE:
        case SE_FS_FILE_WRITE:
        case SE_FS_OBJECT_SETATTRIBUTES:

#if defined(SYM_VXD)
            bNeedOEMFile    = TRUE;
            bNeedOEMOwner   = TRUE;
#endif // #if defined(SYM_VXD)
            bNeedFile       = TRUE;
            bNeedOwner      = TRUE;
            bNeedBackupFile = TRUE;

            break;

        case SE_DEV_WRITE:
        case SE_DEV_FORMAT:

#if defined(SYM_VXD)
            bNeedOEMDevice = TRUE;
            bNeedOEMOwner  = TRUE;
#endif // #if defined(SYM_VXD)
            bNeedDevice    = TRUE;
            bNeedOwner     = TRUE;

            break;

        case SE_DEV_READ:

#if defined(SYM_VXD)
            bNeedOEMDevice = TRUE;
#endif // #if defined(SYM_VXD)
            bNeedDevice    = TRUE;

            break;

        case SE_MSC_REBOOT:

#if defined(SYM_VXD)
            bNeedOEMDevice = TRUE;
            bNeedOEMOwner  = TRUE;
#endif // #if defined(SYM_VXD)
            bNeedOwner     = TRUE;

            break;

        case SE_DEV_MOUNT:

            bNeedDevice = TRUE;

            break;

        default:

            SYM_ASSERT ( FALSE );

            return ( FALSE );
        }

                                        // Initialize device variables.

    if ( bNeedDevice )
        {
        if ( !GetDeviceInfoBasedOnED ( pEventData, &rDeviceInfo ) )
            {
            return ( FALSE );
            }

        if ( !rDeviceInfo.bVisible )
            {
            return ( FALSE );
            }

        prEDExtra->uPhysicalDevice = rDeviceInfo.uPhysicalDevice;
        prEDExtra->bPartitionable  = rDeviceInfo.bPartitionable;
        prEDExtra->uSectorLocation = rDeviceInfo.uBootSectorLocation;
        prEDExtra->bMBRAccess      = rDeviceInfo.chPhysicalID != '\0';
        prEDExtra->chDeviceID      = prEDExtra->bMBRAccess ?
                                     rDeviceInfo.chPhysicalID :
                                     rDeviceInfo.chLogicalID;

        SYM_ASSERT ( prEDExtra->chDeviceID != '\0' );
        }

                                        // Initialize pszFile and the size
                                        // of its buffer.

    if ( bNeedFile )
        {
#if defined(SYM_VXD)
        if ( prEDExtra->uFileCharSet != uEDCharSet )
            {
            prEDExtra->pszFile = (PTSTR) uBufferOffset;

            prEDExtra->uFileBufferLen =
                ALIGN4 ( SYM_MAX_PATH * ( prEDExtra->uFileCharSet != CHARSET_UNICODE ?
                                          sizeof(char) : sizeof(WCHAR) ) );

            uBufferOffset += prEDExtra->uFileBufferLen;
            }
        else
#endif // #if defined(SYM_VXD)
            {
            prEDExtra->pszFile = pEventData->SE_ED_lpPathSpec;

            SYM_ASSERT ( prEDExtra->pszFile );

            prEDExtra->uFileBufferLen =
#if defined(SYM_VXD)
                prEDExtra->uFileCharSet != CHARSET_UNICODE ?
                strlen ( prEDExtra->pszFile ) + 1 :
#endif // #if defined(SYM_VXD)
                ( wcslen ( (PWSTR)prEDExtra->pszFile ) + 1 ) * sizeof(WCHAR);
            }
        }

                                        // Initialize alternate input file
                                        // pointer and size of buffer.

    if ( bNeedAltFile )
        {
#if defined(SYM_VXD)
        if ( prEDExtra->uFileCharSet != uEDCharSet )
            {
            prEDExtra->pszAltFile = (PTSTR) uBufferOffset;

            prEDExtra->uAltFileBufferLen =
                ALIGN4 ( SYM_MAX_PATH * ( prEDExtra->uFileCharSet != CHARSET_UNICODE ?
                                          sizeof(char) : sizeof(WCHAR) ) );

            uBufferOffset += prEDExtra->uAltFileBufferLen;
            }
        else
#endif // #if defined(SYM_VXD)
            {
            prEDExtra->pszAltFile = pEventData->SE_ED_lpAltPathSpec;

            SYM_ASSERT ( prEDExtra->pszAltFile );

            prEDExtra->uAltFileBufferLen =
#if defined(SYM_VXD)
                prEDExtra->uFileCharSet != CHARSET_UNICODE ?
                strlen ( prEDExtra->pszAltFile ) + 1 :
#endif // #if defined(SYM_VXD)
                ( wcslen ( (PWSTR)prEDExtra->pszAltFile ) + 1 ) * sizeof(WCHAR);
            }
        }

                                        // Pre-initialize other EDEXTRA
                                        // pointers and calculate buffer
                                        // length.

    if ( bNeedOwner )
        {
        prEDExtra->pszOwner = (PTSTR) uBufferOffset;

        prEDExtra->uOwnerBufferLen =
#if defined(SYM_VXD)
            prEDExtra->uFileCharSet != CHARSET_UNICODE ?
                ALIGN4 ( SYM_MAX_PATH ) :
#endif // #if defined(SYM_VXD)
                ALIGN4 ( SYM_MAX_PATH * sizeof(WCHAR) );

        uBufferOffset += prEDExtra->uOwnerBufferLen;
        }

#if defined(SYM_VXD)

    if ( bNeedOEMOwner )
        {
        prEDExtra->pszOEMOwner = (PSTR) uBufferOffset;

        prEDExtra->uOEMOwnerBufferLen = ALIGN4 ( SYM_MAX_PATH );

        uBufferOffset += ALIGN4 ( SYM_MAX_PATH );
        }

    if ( bNeedOEMFile )
        {
        prEDExtra->pszOEMFile = (PSTR) uBufferOffset;

        prEDExtra->uOEMFileBufferLen =
            ALIGN4 ( max ( SYM_MAX_PATH, prEDExtra->uFileBufferLen ) );

        uBufferOffset += prEDExtra->uOEMFileBufferLen;
        }

    if ( bNeedOEMAltFile )
        {
        prEDExtra->pszAltOEMFile = (PSTR) uBufferOffset;

        prEDExtra->uAltOEMFileBufferLen =
            ALIGN4 ( max ( SYM_MAX_PATH, prEDExtra->uAltFileBufferLen ) );

        uBufferOffset += prEDExtra->uAltOEMFileBufferLen;
        }

    if ( bNeedOEMDevice )
        {
        prEDExtra->pszOEMDevice = (PSTR) uBufferOffset;

        prEDExtra->uOEMDeviceBufferLen = ALIGN4 ( MAX_DEVICE_NAME_LENGTH + 1 );

        uBufferOffset += ALIGN4 ( MAX_DEVICE_NAME_LENGTH + 1 );
        }

#endif // #if defined(SYM_VXD)

    if ( bNeedBackupFile )
        {
        prEDExtra->pszBackupFile = (PTSTR) uBufferOffset;

        prEDExtra->uBackupFileBufferLen = ALIGN4 ( SYM_MAX_PATH ) * sizeof(TCHAR);

        uBufferOffset += ALIGN4 ( SYM_MAX_PATH ) * sizeof(TCHAR);
        }

                                        // Allocate memory for buffers.

    if ( uBufferOffset )
        {
        prEDExtra->pvMemoryBuffer = MemAllocPtr ( GHND, uBufferOffset );

        if ( !prEDExtra->pvMemoryBuffer )
            {
            UIOutOfMemoryError();

            return ( FALSE );
            }
        }
                                        // Fixup buffer pointers.

#if defined(SYM_VXD)

    if ( bNeedFile && prEDExtra->uFileCharSet != uEDCharSet )
        {
        *(PUINT)&prEDExtra->pszFile += (UINT) prEDExtra->pvMemoryBuffer;

        VxDGenericStringConvert ( pEventData->SE_ED_lpPathSpec,
                                  uEDCharSet,
                                  prEDExtra->pszFile,
                                  prEDExtra->uFileCharSet );
        }

    if ( bNeedAltFile && prEDExtra->uFileCharSet != uEDCharSet )
        {
        *(PUINT)&prEDExtra->pszAltFile += (UINT) prEDExtra->pvMemoryBuffer;

        VxDGenericStringConvert ( pEventData->SE_ED_lpAltPathSpec,
                                  uEDCharSet,
                                  prEDExtra->pszAltFile,
                                  prEDExtra->uFileCharSet );
        }

    if ( bNeedOEMFile )
        {
        *(PUINT)&prEDExtra->pszOEMFile += (UINT) prEDExtra->pvMemoryBuffer;
        }

    if ( bNeedOEMAltFile )
        {
        *(PUINT)&prEDExtra->pszAltOEMFile += (UINT) prEDExtra->pvMemoryBuffer;
        }

    if ( bNeedOEMDevice )
        {
        *(PUINT)&prEDExtra->pszOEMDevice += (UINT) prEDExtra->pvMemoryBuffer;
        }

    if ( bNeedOEMOwner )
        {
        *(PUINT)&prEDExtra->pszOEMOwner += (UINT) prEDExtra->pvMemoryBuffer;
        }

#endif // #if defined(SYM_VXD)

    if ( bNeedOwner )
        {
        *(PUINT)&prEDExtra->pszOwner += (UINT) prEDExtra->pvMemoryBuffer;
        }

    if ( bNeedBackupFile )
        {
        *(PUINT)&prEDExtra->pszBackupFile += (UINT) prEDExtra->pvMemoryBuffer;
        }

                                        // Lock current configuration.

    if ( !( prEDExtra->prNAVAPCfg = LockNAVAPConfiguration() ) )
        {
        if ( prEDExtra->pvMemoryBuffer )
            {
            MemFreePtr ( prEDExtra->pvMemoryBuffer );
            }

        return ( FALSE );
        }

                                        // Lock AVAPI handle.  For now, do
                                        // this unconditionally, no matter
                                        // what event we are servicing.
                                        // ~~~TBD~~~ Optimize this code.

    if ( !( prEDExtra->hVGlobalContext = LockNAVAPScanConfiguration() ) )
        {
        UnlockNAVAPConfiguration ( prEDExtra->prNAVAPCfg );

        if ( prEDExtra->pvMemoryBuffer )
            {
            MemFreePtr ( prEDExtra->pvMemoryBuffer );
            }

        return ( FALSE );
        }

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// VOID CleanUpEDExtra (
//      PEDEXTRA prEDExtra
//      );
//
// This function releases memory allocated for an EDEXTRA structure.
//
// Entry:
//      prEDExtra  - pointer to an EDEXTRA structure
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID CleanUpEDExtra (
    PEDEXTRA prEDExtra
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );

                                        // Unlock AVAPI handle.

    if ( prEDExtra->hVGlobalContext )
        {
        UnlockNAVAPScanConfiguration ( prEDExtra->hVGlobalContext );
        }

                                        // Unlock configuration.

    UnlockNAVAPConfiguration ( prEDExtra->prNAVAPCfg );

                                        // Release memory, if allocated.

    if ( prEDExtra->pvMemoryBuffer )
        {
        MemFreePtr ( prEDExtra->pvMemoryBuffer );
        }
    }

//---------------------------------------------------------------------------
//
// BOOL ProcessEDExtra (
//      PEDEXTRA prEDExtra
//      );
//
// This function processes a standard EDEXTRA event data structure.
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      TRUE  if SYMEVENT should continue with the event
//      FALSE if SYMEVENT should stop the event
//
//---------------------------------------------------------------------------
BOOL ProcessEDExtra (
    PEDEXTRA prEDExtra
    )
    {
    auto BOOL         bResult;
    auto BOOL         bCheck;
    auto HEVENTPACKET hEventPacket;

                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );

                                        // For file events, ignore OS
                                        // reserved names (SYM_VXD: unless
                                        // they are in UNICODE).

                                        // Note: no need to check if alt
                                        // names are OS reserved...

    if ( prEDExtra->pszFile &&

#if defined(SYM_VXD)

         prEDExtra->uFileCharSet != CHARSET_UNICODE &&

#endif // #if defined(SYM_VXD)

         IsFileNameReserved ( prEDExtra->pszFile ) )
        {
        return ( TRUE );
        }

                                        // Create event packet.

    hEventPacket = NULL;

    bCheck = EventPacketCreate ( &hEventPacket, prEDExtra );

    SYM_ASSERT ( bCheck );
    SYM_ASSERT ( hEventPacket );

    if ( bCheck && hEventPacket )
        {
                                        // Notify Kernel of the Event Packet
                                        // and wait until it has been
                                        // serviced.

        EventPacketProcess ( hEventPacket );

                                        // Set result for the event.

        bResult = EventPacketGetResult ( hEventPacket );

                                        // Destroy Event Packet.

        bCheck = EventPacketDestroy ( &hEventPacket );

        SYM_ASSERT ( bCheck );
        SYM_ASSERT ( !hEventPacket );
        }
    else
        {
                                        // If unable to create event packet,
                                        // get default result.

        return ( GetDefaultEventResult ( NULL, prEDExtra ) );
        }

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// BOOL EDXSetbFileIsExecutable (
//      PEDEXTRA prEDExtra
//      );
//
// This function sets the bFileIsExecutable member of prEDExtra to a
// valid value.
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      Returns the new value of prEDExtra->bFileIsExecutable.
//
//---------------------------------------------------------------------------
BOOL EDXSetbFileIsExecutable (
    PEDEXTRA prEDExtra
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );

                                        // Check filename against lists of
                                        // executable extensions.

    prEDExtra->bFileIsExecutable =
        IsFileInListOfExtensions ( EDXpszOEMFile (prEDExtra),
                                   prAPCfg(prEDExtra)->pszzProgramExtensionList );

    prEDExtra->bFileIsExecutableSet = TRUE;

    return ( prEDExtra->bFileIsExecutable );
    }

//---------------------------------------------------------------------------
//
// BOOL EDXSetbAltFileIsExecutable (
//      PEDEXTRA prEDExtra
//      );
//
// This function sets the bAltFileIsExecutable member of prEDExtra
// to a valid value.
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      Returns the new value of prEDExtra->bAltFileIsExecutable
//
//---------------------------------------------------------------------------
BOOL EDXSetbAltFileIsExecutable (
    PEDEXTRA prEDExtra
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );

                                        // Check filename against lists of
                                        // executable extensions.

    prEDExtra->bAltFileIsExecutable =
        IsFileInListOfExtensions ( EDXpszAltOEMFile ( prEDExtra ),
                                   prAPCfg(prEDExtra)->pszzProgramExtensionList );

    prEDExtra->bAltFileIsExecutableSet = TRUE;

    return ( prEDExtra->bAltFileIsExecutable );
    }

//---------------------------------------------------------------------------
//
// PTSTR EDXSetpszOwner (
//       PEDEXTRA prEDExtra
//       );
//
// Sets the pszOwner member of prEDExtra to the name of the owner of the
// process which generated prEDExtra.  If the name of the owner cannot be
// determined, then szUnknownOwner will be copied to the buffer (in the
// appropriate character set, of course.)
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      Returns prEDExtra->pszOwner.
//
//---------------------------------------------------------------------------
PTSTR EDXSetpszOwner (
    PEDEXTRA prEDExtra
    )
    {
    auto PTSTR pszOwner;

                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );
    SYM_ASSERT ( prEDExtra->pszOwner );
#if defined(SYM_VXD)
    SYM_ASSERT ( prEDExtra->uFileCharSet == CHARSET_UNICODE ?
                 *(PWSTR)prEDExtra->pszOwner == '\0' :
                 *prEDExtra->pszOwner == '\0' );
#else
    SYM_ASSERT ( *prEDExtra->pszOwner == '\0' );
#endif

                                        // Get name of owner.

    pszOwner = SYMEvent_GetCurTask();

    if ( pszOwner )
        {
#if defined(SYM_VXD)
        auto UINT  uEDCharSet;
        auto PTSTR pszOEMOwner;
        auto TCHAR szOEMOwner[SYM_MAX_PATH];

                                        // Determine the character set of
                                        // the filenames passed to us by
                                        // SYMEvent.

                                        // ~~~TBD~~~ SYMEvent is supposed to
                                        // tell us about the character sets.

        uEDCharSet = Get_Cur_VM_Handle() == Get_Sys_VM_Handle() ?
                         CHARSET_ANSI : CHARSET_OEM;

                                        // Validate length of buffers.

        SYM_ASSERT ( ( uEDCharSet == CHARSET_UNICODE ) ?
                     ( wcslen ( (PWSTR)pszOwner ) < SYM_MAX_PATH ) :
                     ( strlen ( pszOwner ) < SYM_MAX_PATH )
                   );

                                        // Make sure owner is valid.

        pszOEMOwner = prEDExtra->pszOEMOwner ? prEDExtra->pszOEMOwner :
                                               szOEMOwner;

        VxDGenericStringConvert ( pszOwner, uEDCharSet,
                                  pszOEMOwner, CHARSET_OEM );

                                        // If all valid, copy owner's name
                                        // to allocated buffer and allow it
                                        // to be excluded.

        if ( *pszOEMOwner != '\0' )
            {
            VxDGenericStringConvert ( pszOwner,
                                      uEDCharSet,
                                      prEDExtra->pszOwner,
                                      prEDExtra->uFileCharSet );

#else
        if ( *pszOwner != '\0' )
            {
            STRCPY ( prEDExtra->pszOwner, pszOwner );

#endif
            prEDExtra->bOwnerIsKnown = TRUE;
            }
        else
            {
                                        // Otherwise, display unknown owner.
            pszOwner = NULL;
            }
        }

    if ( !pszOwner )
        {
                                        // If no owner name, then set the
                                        // owner buffer to szUnknownOwner.

#if defined(SYM_VXD)
        VxDGenericStringConvert ( szUnknownOwner,
                                  CHARSET_OEM,
                                  prEDExtra->pszOwner,
                                  prEDExtra->uFileCharSet );
#else
        STRCPY ( prEDExtra->pszOwner, szUnknownOwner );
#endif

                                        // .. and don't allow it to be
                                        // excluded.

        prEDExtra->bOwnerIsKnown = FALSE;
        }

    return ( prEDExtra->pszOwner );
    }

//---------------------------------------------------------------------------
//
// PSTR EDXSetpszOEMFile (
//      PEDEXTRA prEDExtra
//      );
//
// This function converts prEDExtra->pszFile to OEM and places it at
// prEDExtra->pszOEMFile.
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      Returns prEDExtra->pszOEMFile.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

PSTR EDXSetpszOEMFile (
    PEDEXTRA prEDExtra
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );
    SYM_ASSERT ( prEDExtra->pszFile );
    SYM_ASSERT ( *prEDExtra->pszFile != '\0' );
    SYM_ASSERT ( prEDExtra->pszOEMFile );
    SYM_ASSERT ( *prEDExtra->pszOEMFile == '\0' );

                                        // Convert pszFile to OEM.

    switch ( prEDExtra->uFileCharSet )
        {
        case CHARSET_ANSI:

            SYM_ASSERT ( strlen ( prEDExtra->pszFile ) < SYM_MAX_PATH );

            CharToOemA ( prEDExtra->pszFile, prEDExtra->pszOEMFile );

            break;

        case CHARSET_UNICODE:

            SYM_ASSERT ( wcslen ( (PWSTR)prEDExtra->pszFile ) < SYM_MAX_PATH );

            CharToOemW ( (PWSTR)prEDExtra->pszFile, prEDExtra->pszOEMFile );

            break;

        case CHARSET_OEM:
        default:

            SYM_ASSERT ( strlen ( prEDExtra->pszFile ) < SYM_MAX_PATH );

            prEDExtra->pszOEMFile = prEDExtra->pszFile;

            break;

        }

    return ( prEDExtra->pszOEMFile );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// PSTR EDXSetpszAltOEMFile (
//      PEDEXTRA prEDExtra
//      );
//
// This function converts prEDExtra->pszAltFile to OEM and places it at
// prEDExtra->pszAltOEMFile.
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      Returns prEDExtra->pszAltOEMFile.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

PSTR EDXSetpszAltOEMFile (
    PEDEXTRA prEDExtra
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );
    SYM_ASSERT ( prEDExtra->pszAltFile );
    SYM_ASSERT ( *prEDExtra->pszAltFile != '\0' );
    SYM_ASSERT ( prEDExtra->pszAltOEMFile );
    SYM_ASSERT ( *prEDExtra->pszAltOEMFile == '\0' );

                                        // Convert pszAltFile to OEM.

    switch ( prEDExtra->uFileCharSet )
        {
        case CHARSET_ANSI:

            SYM_ASSERT ( strlen ( prEDExtra->pszAltFile ) < SYM_MAX_PATH );

            CharToOemA ( prEDExtra->pszAltFile, prEDExtra->pszAltOEMFile );

            break;

        case CHARSET_UNICODE:

            SYM_ASSERT ( wcslen ( (PWSTR)prEDExtra->pszAltFile ) < SYM_MAX_PATH );

            CharToOemW ( (PWSTR)prEDExtra->pszAltFile, prEDExtra->pszAltOEMFile );

            break;

        case CHARSET_OEM:
        default:

            SYM_ASSERT ( strlen ( prEDExtra->pszAltFile ) < SYM_MAX_PATH );

            prEDExtra->pszAltOEMFile = prEDExtra->pszAltFile;

            break;

        }

    return ( prEDExtra->pszAltOEMFile );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// PSTR EDXSetpszOEMDevice (
//      PEDEXTRA prEDExtra
//      );
//
// This function converts prEDExtra->pszDevice to OEM and places it at
// prEDExtra->pszOEMDevice.
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      Returns prEDExtra->pszOEMDevice.
//
// Note:
//      The strings in this function should not be translated.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

PSTR EDXSetpszOEMDevice (
    PEDEXTRA prEDExtra
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );
    SYM_ASSERT ( prEDExtra->pszOEMDevice );
    SYM_ASSERT ( *prEDExtra->pszOEMDevice == '\0' );

                                        // Convert pszDevice to its OEM
                                        // (or rather "user-friendly") form.

    prEDExtra->pszOEMDevice[0] = prEDExtra->chDeviceID;
    prEDExtra->pszOEMDevice[1] = ':';
    prEDExtra->pszOEMDevice[2] = '\0';

    return ( prEDExtra->pszOEMDevice );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// PSTR EDXSetpszOEMOwner (
//      PEDEXTRA prEDExtra
//      );
//
// This function converts prEDExtra->pszOwner to OEM and places it at
// prEDExtra->pszOEMOwner.
//
// Entry:
//      prEDExtra - pointer to EDEXTRA structure containing event data.
//
// Exit:
//      Returns prEDExtra->pszOEMOwner.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

PSTR EDXSetpszOEMOwner (
    PEDEXTRA prEDExtra
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prEDExtra );
    SYM_ASSERT ( prEDExtra->pszOwner );
    SYM_ASSERT ( prEDExtra->pszOEMOwner );
    SYM_ASSERT ( *prEDExtra->pszOEMOwner == '\0' );

                                        // Convert pszOwner to OEM.

    switch ( prEDExtra->uFileCharSet )
        {
        case CHARSET_ANSI:

            SYM_ASSERT ( strlen ( EDXpszOwner(prEDExtra) ) < SYM_MAX_PATH );

            CharToOemA ( EDXpszOwner(prEDExtra), prEDExtra->pszOEMOwner );

            break;

        case CHARSET_UNICODE:

            SYM_ASSERT ( wcslen ( (PWSTR)EDXpszOwner(prEDExtra) ) < SYM_MAX_PATH );

            CharToOemW ( (PWSTR)EDXpszOwner(prEDExtra), prEDExtra->pszOEMOwner );

            break;

        case CHARSET_OEM:
        default:

            SYM_ASSERT ( strlen ( EDXpszOwner(prEDExtra) ) < SYM_MAX_PATH );

            prEDExtra->pszOEMOwner = EDXpszOwner(prEDExtra);

            break;

        }

    return ( prEDExtra->pszOEMOwner );
    }

#endif // #if defined(SYM_VXD)
