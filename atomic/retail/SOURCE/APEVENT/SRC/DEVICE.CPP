// Copyright 1995 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/DEVICE.CPv   1.9   01 Feb 1999 14:10:44   MBROWN  $
//
// Description:
//      This file defines NAVAP helper functions for working with
//      devices and device events.
//
// Contains:
//      IsItPhysicalDeviceEvent()           (local inline function)
//      IsItLogicalDeviceEvent()            (local inline function)
//
//      CreateDeviceList()
//      DestroyDeviceList()
//
//      InstallDeviceEventHooks()
//      UninstallDeviceEventHooks()
//
//      GetNextRemovableDevice()
//      GetDeviceInfoBasedOnED()
//
//      AddEntryToDeviceList()              (local function)
//      AddExtendedPartitionsToDeviceList() (local function)
//      EnumerateDeviceList()               (local function)
//
// See Also:
//      apevent.h, navap.h
//
// Notes:
//      Event Manager module.
//
//      The SYM_NTK implementation of the APIs in this file do not create
//      a device list.  All exported functions are either stubbed out, or
//      emulate the device list.  A full device list is not needed because
//      the only device related event that NAVAP for NT intercepts is
//      SE_DEV_MOUNT, and the code that uses it does not need a full device
//      list.
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/DEVICE.CPv  $
// 
//    Rev 1.9   01 Feb 1999 14:10:44   MBROWN
// Backed out of large sector fix for Phoenix
//
//    Rev 1.8   08 Dec 1998 19:51:26   RStanev
// Added support for SE_DEV_READ for NT.
//
//    Rev 1.7   01 Dec 1998 12:28:20   MBROWN
// large sector fix
//
//    Rev 1.6   10 Jun 1998 15:30:52   RStanev
// NEC device enumeration fix.
//
//    Rev 1.5   08 Jun 1998 22:02:56   RStanev
// Added support for FAT32 partitions.
//
//    Rev 1.4   16 Mar 1998 16:07:06   DALLEE
// Updated to match wider fields in ABSDISKREC (changed for > 8G drives).
//
//    Rev 1.3   17 Feb 1998 17:54:48   RStanev
// Ported NEC code from the ROSNEC area (MDUNN).
//
//    Rev 1.2   17 Feb 1998 16:50:08   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 01:05:16   RStanev
// Too many changes to describe.
//
//    Rev 1.0   26 Feb 1997 04:34:10   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "navap.h"
#include "apevent.h"

//---------------------------------------------------------------------------
//
// The following functions determine whether a device event is generated
// because of a physical or logical access.
//
// Note: The strings ':' should not be translated.
//
//---------------------------------------------------------------------------
inline BOOL IsItPhysicalDeviceEvent (
    LPED prEventData
    )
    {
    return ( (BOOL) ( prEventData->SE_ED_lpPathSpec[0] == ':' ) );
    }

inline BOOL IsItLogicalDeviceEvent (
    LPED prEventData
    )
    {
    return ( (BOOL) ( prEventData->SE_ED_lpPathSpec[1] == ':' ) );
    }

//---------------------------------------------------------------------------
//
// Define local function prototypes.
//
//---------------------------------------------------------------------------
BOOL AddEntryToDeviceList (
    PDEVICEINFOREC prDeviceInfo
    );

BOOL AddExtendedPartitionsToDeviceList (
    PABSDISKREC prAbsDiskRec,
    UINT        uExtendedPartitionLink,
    UINT        uPartition
    );

BOOL EnumerateDeviceList (
    VOID
    );

//---------------------------------------------------------------------------
//
// The following variable points to the first device info structure
// in the device info list.
//
//---------------------------------------------------------------------------
PDEVICEINFOREC prDeviceInfoListStart = NULL;

//---------------------------------------------------------------------------
//
// BOOL CreateDeviceList (
//      VOID
//      );
//
// This function creates a list with information about all existing
// int 13 - compatible devices on the system.  prDeviceInfoListStart will
// be set to point to the first entry of the device list.  Each entry in
// the device list contans a pointer to the next entry.  The last entry
// will have it's next entry pointer set to NULL.
//
// Entry:
//      -
//
// Exit:
//      TRUE if successful and the device list is fully initialized, FALSE
//      if unable to create the list.
//
// Syncronization:
//      This function is not reentrant.
//
//---------------------------------------------------------------------------
BOOL CreateDeviceList (
    VOID
    )
    {
#if defined(SYM_NTK)

    return ( TRUE );

#else

    auto BOOL          bError;
    auto BOOL          bDevicePartitionable, bHavePhysicalInfo;
    auto BOOL          bNonPartDeviceCount, bPartDeviceCount;
    auto UINT          uNonPartDeviceCount, uPartDeviceCount;
    auto UINT          uDevice;
    auto UINT          uPartitionCounter;
    auto ABSDISKREC    rAbsDiskRec;
    auto PartitionRec  rPartition;
    auto DEVICEINFOREC rDeviceInfo;

                                        // Make sure the list has not
                                        // been initialized yet.

    SYM_ASSERT ( !prDeviceInfoListStart );

                                        // Initialize local variables.

    bError              = FALSE;

    bNonPartDeviceCount = FALSE;

    bPartDeviceCount    = FALSE;

                                        // Allocate memory for IO buffer.
                                        // Its length should be at least the
                                        // size of the biggest possible disk
                                        // sector.

    rAbsDiskRec.buffer = (BYTE*) MemAllocPtr ( GHND, 0x400 );

    if ( !rAbsDiskRec.buffer )
        {
        UIOutOfMemoryError();
        return ( FALSE );
        }

    if ( bHWIsNEC )
        {
                                        // On NEC we are only looking for
                                        // floppy drives.

        for ( uDevice = 0; uDevice < 26 && !bError; uDevice++ )
            {
                                        // Check hostile devices.

            if ( rNAVAPInternalCfg.bHostileInt13Device[uDevice] != FALSE )
                {
                continue;
                }

                                        // See if current device exists.

            if ( !IOSSrv_Chk_NEC_Floppy ( uDevice ) )
                {
                continue;
                }
                                        // Prepare an entry to add to the
                                        // list.  uPhysicalDevice does not
                                        // correspond to the int1b device
                                        // number.  Use A = 0, B = 1, etc.
                                        // Set it all up here, and don't
                                        // call EnumerateDeviceList().

            rDeviceInfo.uPhysicalDevice     = uDevice;
            rDeviceInfo.bVisible            = TRUE;
            rDeviceInfo.bPartitionable      = FALSE;
            rDeviceInfo.uPartition          = 0;
            rDeviceInfo.uExtPartition       = 0;
            rDeviceInfo.bPartitionBootable  = FALSE;
            rDeviceInfo.uBootSectorLocation = 0;
            rDeviceInfo.chPhysicalID        = 0;
            rDeviceInfo.chLogicalID         = 'A' + uDevice;
            rDeviceInfo.hEvent              = INVALID_EVENT_HANDLE;

                                        // Add entry to list

            bError = AddEntryToDeviceList ( &rDeviceInfo ) ? FALSE : TRUE;
            }

        }
    else
        {
                                        // Scan for int - 13 devices and add
                                        // an entry (multi - partitioned
                                        // fixed devices will have more than
                                        // one) for each one of them.

        for ( uDevice = 0; uDevice <= 255 && !bError; uDevice++ )
            {
                                        // Prepare rAbsDiskRec for this
                                        // device.

            rAbsDiskRec.dn = uDevice;

                                        // Initialize local variables.

            bDevicePartitionable = ( uDevice >= 0x80 ) ? TRUE : FALSE;

            bHavePhysicalInfo = FALSE;

                                        // Check hostile devices.

            if ( rNAVAPInternalCfg.bHostileInt13Device[uDevice] )
                {
                continue;
                }

                                        // Check forced devices and device
                                        // counter.  Ugly and long code, but
                                        // it's used only during
                                        // initialization.

            if ( !rNAVAPInternalCfg.bForcedInt13Device[uDevice] &&
                 !rNAVAPInternalCfg.bIgnoreInt13DeviceCount )
                {
                                        // Make sure we have a counter for
                                        // this type of devices.

                if ( ! ( bDevicePartitionable ? bPartDeviceCount :
                                                bNonPartDeviceCount ) )
                    {
                    if ( DiskGetPhysicalInfo ( &rAbsDiskRec ) != 0 )
                        {
                        continue;
                        }

                    bHavePhysicalInfo = TRUE;

                    if ( !bDevicePartitionable )
                        {
                        uNonPartDeviceCount = rAbsDiskRec.numDrives;

                        bNonPartDeviceCount = TRUE;
                        }
                    else
                        {
                        uPartDeviceCount = rAbsDiskRec.numDrives;

                        bPartDeviceCount = TRUE;
                        }
                    }

                                        // Check if we are within the
                                        // counter limits of
                                        // non-partitionable devices.

                    if ( !bDevicePartitionable &&
                         uNonPartDeviceCount <= uDevice )
                        {
                        continue;
                        }

                                        // Check if we are within the
                                        // counter limits of partitionable
                                        // devices.

                    if ( bDevicePartitionable &&
                         uPartDeviceCount <= uDevice - 0x80 )
                        {
                        continue;
                        }
                }

                                        // See if current device exists.

            if ( !bHavePhysicalInfo &&
                 DiskGetPhysicalInfo ( &rAbsDiskRec ) != 0 )
                {
                continue;
                }

                                        // Prepare an entry to add to the
                                        // list.

            if ( !bDevicePartitionable )
                {

                                        // If device is not partitionable,
                                        // then assume there is no MBR.

                rDeviceInfo.uPhysicalDevice     = rAbsDiskRec.dn;
                rDeviceInfo.bVisible            = TRUE;
                rDeviceInfo.bPartitionable      = FALSE;
                rDeviceInfo.uPartition          = 0;
                rDeviceInfo.uExtPartition       = 0;
                rDeviceInfo.bPartitionBootable  = FALSE;
                rDeviceInfo.uBootSectorLocation = 0;
                rDeviceInfo.chPhysicalID        = 0;
                rDeviceInfo.chLogicalID         = 0;
                rDeviceInfo.hEvent              = INVALID_EVENT_HANDLE;

                                        // Add entry to list.

                bError = AddEntryToDeviceList ( &rDeviceInfo ) ? FALSE : TRUE;

                continue;
                }

                                        // If device is partitionable, then
                                        // create a separate entry for each
                                        // partition.

            for ( uPartitionCounter = 0;
                  uPartitionCounter < 4 && !bError;
                  uPartitionCounter++ )
                {

                                        // Get partition information.

                if ( !DiskGetPartitionTableEntry ( rAbsDiskRec.dn,
                                                   uPartitionCounter,
                                                   &rPartition,
                                                   &rAbsDiskRec ) )
                    {
                                        // If error, just let go and do
                                        // not report an error.  The user
                                        // is in big trouble any way.
                    continue;
                    }

                                        // If extended partition, then
                                        // process the extended partition
                                        // chain.

                if ( rPartition.system == SYSTEM_EXTENDED )
                    {
                    bError = AddExtendedPartitionsToDeviceList (
                                &rAbsDiskRec,
                                1,
                                uPartitionCounter ) ? FALSE : TRUE;

                    continue;
                    }

                                        // Validate the partition.

                if ( rPartition.system != SYSTEM_DOS_12 &&
                     rPartition.system != SYSTEM_DOS_16 &&
                     rPartition.system != SYSTEM_DOS_32 &&
                     rPartition.system != SYSTEM_BIGDOS &&
                     !rPartition.bootable )
                    {
                    continue;
                    }

                                        // If successful, then create an
                                        // entry for the partition.  Either
                                        // one of those partitions can be
                                        // bootable.

                rDeviceInfo.uPhysicalDevice     = rAbsDiskRec.dn;
                rDeviceInfo.bVisible            = TRUE;
                rDeviceInfo.bPartitionable      = TRUE;
                rDeviceInfo.uPartition          = uPartitionCounter;
                rDeviceInfo.uExtPartition       = 0;
                rDeviceInfo.bPartitionBootable  = TRUE;
                rDeviceInfo.uBootSectorLocation = ConvertPhySectorToLong (
                                                      &rAbsDiskRec,
                                                      rAbsDiskRec.dwStartingHead,
                                                      rAbsDiskRec.dwStartingTrack,
                                                      rAbsDiskRec.dwStartingSector
                                                      );
                rDeviceInfo.chPhysicalID        = 0;
                rDeviceInfo.chLogicalID         = 0;
                rDeviceInfo.hEvent              = INVALID_EVENT_HANDLE;

                                        // Add entry to list.

                bError = AddEntryToDeviceList ( &rDeviceInfo ) ? FALSE : TRUE;
                }
            }

                                        // Enumerate the devices found.

        if ( !bError )
            {
            EnumerateDeviceList();
            }
        }

                                        // Free allocated memory.

    MemFreePtr ( rAbsDiskRec.buffer );

                                        // If there was an error, then
                                        // destroy whatever was created and
                                        // fail the function.

    if ( bError )
        {
        if ( !DestroyDeviceList() )
            {
            SYM_ASSERT ( FALSE );
            }

        return ( FALSE );
        }

                                        // Return success.

    return ( TRUE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL DestroyDeviceList (
//      VOID
//      );
//
// This function destroys the device list pointed by prDeviceInfoListStart.
// It deallocates all memory that was allocated for the list by
// CreateDeviceList().
//
// Entry:
//      -
//
// Exit:
//      Returns always TRUE.
//
// Syncronization:
//      This function is not reentrant.
//
//---------------------------------------------------------------------------
BOOL DestroyDeviceList (
    VOID
    )
    {
#if !defined(SYM_NTK)

    auto PDEVICEINFOREC prNextDeviceInfo;

                                        // Free the memory allocated for
                                        // each entry in the list.

    while ( prDeviceInfoListStart )
        {
        SYM_ASSERT ( prDeviceInfoListStart->hEvent == INVALID_EVENT_HANDLE );

        prNextDeviceInfo = prDeviceInfoListStart->prNextDeviceInfo;

        MemFreePtr ( prDeviceInfoListStart );

        prDeviceInfoListStart = prNextDeviceInfo;
        }

#endif

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL InstallDeviceEventHooks (
//     EVENTTYPE    EventType,
//     EVENTFLAGS   EventFlags,
//     EVENTHANDLER EventHandler,
//     REFDATA      RefData
//     );
//
// This function installs hooks for monitoring physical writes to the
// boot records of some of the devices in the device list pointed by
// prDeviceInfoListStart.  Information about the parameters for the
// event hooks will be retrieved from prEventInfo.
//
// Entry:
//      EventType    - Parameter to pass to EventObjectCreate().
//      EventFlags   - Parameter to pass to EventObjectCreate().
//      EventHandler - Parameter to pass to EventObjectCreate().
//      RefData      - Parameter to pass to EventObjectCreate().
//
// Exit:
//      TRUE if successful, FALSE if failed to install all hooks.
//
// Note:
//      The strings in this function should not be translated.
//
// Syncronization:
//      This function is not reentrant.
//
//---------------------------------------------------------------------------
BOOL InstallDeviceEventHooks (
    EVENTTYPE    EventType,
    EVENTFLAGS   EventFlags,
    EVENTHANDLER EventHandler,
    REFDATA      RefData
    )
    {
#if defined(SYM_NTK)

    SYM_ASSERT ( FALSE );

    return ( FALSE );

#else

    auto PDEVICEINFOREC prDeviceEntry;
    auto int            iInvalidHooks;
    auto HEVENT         hEvent;

                                        // Initialize local variables.

    prDeviceEntry = prDeviceInfoListStart;

    iInvalidHooks = 0;

                                        // Initialize hooks for the boot
                                        // records of all partitioned
                                        // devices in the device list, using
                                        // the entry parameters to
                                        // InstallDeviceEventHooks().

    while ( prDeviceEntry )
        {
        if ( prDeviceEntry->bVisible &&
             prDeviceEntry->bPartitionable &&
             prDeviceEntry->hEvent == INVALID_EVENT_HANDLE )
            {
            prDeviceEntry->rSElParam.dwStartingSector =
                prDeviceEntry->uBootSectorLocation;

            prDeviceEntry->rSElParam.dwNumberOfSectors = 1;

            prDeviceEntry->rSElParam.szDeviceFilter[0] = ':';

            prDeviceEntry->rSElParam.szDeviceFilter[1] =
                (TCHAR)(prDeviceEntry->uPhysicalDevice - 0x80 + '0');

            prDeviceEntry->rSElParam.szDeviceFilter[2] = '\0';

            hEvent = EventObjectCreate (
                        EventType,
                        EventFlags,
                        EventHandler,
                        RefData,
                        (LPARAM) &prDeviceEntry->rSElParam.szDeviceFilter );

            prDeviceEntry->hEvent = hEvent;

            if ( hEvent == INVALID_EVENT_HANDLE )
                {
                iInvalidHooks++;
                }
            }

        prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
        }

    return ( iInvalidHooks == 0 );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL UninstallDeviceEventHooks (
//     EVENTTYPE    EventType,
//     EVENTFLAGS   EventFlags,
//     EVENTHANDLER EventHandler,
//     REFDATA      RefData
//     );
//
// This function uninstalls the hooks for monitoring physical writes to
// the boot records of some of the devices in the device list pointed by
// prDeviceInfoListStart.
//
// Entry:
//      EventType    - Parameter passed to EventObjectCreate() during init.
//      EventFlags   - Parameter passed to EventObjectCreate() during init.
//      EventHandler - Parameter passed to EventObjectCreate() during init.
//      RefData      - Parameter passed to EventObjectCreate() during init.
//
// Exit:
//      TRUE if all hooks successfully uninstalled, FALSE if failed to
//      uninstall one more more hooks.
//
// Note:
//      None of the entry parameters are currently used.
//
// Syncronization:
//      This function is not reentrant.
//
//---------------------------------------------------------------------------
BOOL UninstallDeviceEventHooks (
    EVENTTYPE    EventType,
    EVENTFLAGS   EventFlags,
    EVENTHANDLER EventHandler,
    REFDATA      RefData
    )
    {
#if defined(SYM_NTK)

    SYM_ASSERT ( FALSE );

    return ( FALSE );

#else

    auto PDEVICEINFOREC prDeviceEntry;
    auto int            iEventErrors;
    auto BOOL           bDestroyed;

                                        // Initialize local variables.

    prDeviceEntry = prDeviceInfoListStart;

    iEventErrors  = 0;

                                        // Uninstall hooks.

    while ( prDeviceEntry )
        {
        if ( prDeviceEntry->hEvent != INVALID_EVENT_HANDLE )
            {
            bDestroyed = EventObjectDestroy (
                            prDeviceEntry->hEvent
                            );

            if ( bDestroyed )
                {
                prDeviceEntry->hEvent = INVALID_EVENT_HANDLE;
                }
            else
                {
                iEventErrors++;
                }
            }

        prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
        }

    return ( iEventErrors == 0 );

#endif
    }

//---------------------------------------------------------------------------
//
// PDEVICEINFOREC GetNextRemovableDevice (
//      PDEVICEINFOREC prDeviceInfo
//      );
//
// This function searches the device list for the next removable device.
// The search will start at entry after the entry pointed by prDeviceInfo.
// If prDeviceInfo is NULL, then the search will start at the beginning
// of the list.
//
// Entry:
//      prEventInfo - pointer to device info entry
//
// Exit:
//      Pointer to next removable device in the device list, or NULL
//      if no more removable devices
//
//---------------------------------------------------------------------------
PDEVICEINFOREC GetNextRemovableDevice (
    PDEVICEINFOREC prDeviceInfo
    )
    {
#if defined(SYM_NTK)

    SYM_ASSERT ( FALSE );

    return ( NULL );

#else

                                        // Get starting search entry.

    prDeviceInfo = prDeviceInfo ? prDeviceInfo->prNextDeviceInfo :
                                  prDeviceInfoListStart;

                                        // Search device list for next
                                        // removable entry.

    while ( prDeviceInfo &&
            prDeviceInfo->uPhysicalDevice >= 0x80 )
        {
        prDeviceInfo = prDeviceInfo->prNextDeviceInfo;
        }

    return ( prDeviceInfo );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL GetDeviceInfoBasedOnED (
//      LPED           prEventData,
//      PDEVICEINFOREC prDeviceInfo
//      );
//
// This function searches the device list for the device which caused
// an SE_DEV_ event and returns device info.
//
// Entry:
//      prEventData  - pointer to a SYMEvent structure, with which the
//                     client was notified of the event.
//      prDeviceInfo - pointer to a structure to fill.
//
// Exit:
//      TRUE if successful and *prDeviceInfo initialized, FALSE if error or
//      no device found, in which case *prDeviceInfo may be destroyed.
//
// Notes:
//      SYM_NTK: Since device list is not created for this platform, this
//               function will initialize *prDeviceInfo with values based
//               on the prEventData structure.
//      The strings in this function should not be moved to string files.
//
//---------------------------------------------------------------------------
BOOL GetDeviceInfoBasedOnED (
    LPED           prEventData,
    PDEVICEINFOREC prDeviceInfo
    )
    {
#if defined(SYM_NTK)

                                        // Validate input parameters.

    SYM_ASSERT ( prEventData );
    SYM_ASSERT ( prEventData->SE_ED_lpPathSpec );
    SYM_VERIFY_STRING ( prEventData->SE_ED_lpPathSpec );
    if ( STRLEN ( prEventData->SE_ED_lpPathSpec ) != 2 )
        {
        return ( FALSE );
        }
    SYM_ASSERT ( prEventData->SE_ED_etEventType == SE_DEV_MOUNT );
    SYM_ASSERT ( IsItLogicalDeviceEvent(prEventData) );
    SYM_ASSERT ( prDeviceInfo );

                                        // Initialize return values.

    memset ( prDeviceInfo, 0, sizeof ( *prDeviceInfo ) );

                                        // If it is logical event, then
                                        // transfer the information.  The
                                        // only fields really required for
                                        // SYM_NTK are device's logical ID
                                        // and a flag that the device is
                                        // visible.

    if ( IsItLogicalDeviceEvent(prEventData) )
        {
        prDeviceInfo->chLogicalID = CharToUpper ( prEventData->SE_ED_lpPathSpec[0] );

        prDeviceInfo->bVisible = TRUE;

        return ( TRUE );
        }

                                        // If it is not logical event, then
                                        // return error.

    return ( FALSE );

#else

    auto UINT           uPhysicalDevice;
    auto TCHAR          chLogicalDevice;
    auto PDEVICEINFOREC prDeviceEntry;

                                        // Validate input parameters.

    SYM_ASSERT ( prEventData );
    SYM_ASSERT ( prEventData->SE_ED_lpPathSpec );
    SYM_VERIFY_STRING ( prEventData->SE_ED_lpPathSpec );
    if ( STRLEN ( prEventData->SE_ED_lpPathSpec ) != 2 )
        {
        return ( FALSE );
        }
    SYM_ASSERT ( prDeviceInfo );

                                        // Initialize return values.

    memset ( prDeviceInfo, 0, sizeof ( *prDeviceInfo ) );

                                        // Convert event data structure
                                        // to our own search format.  If
                                        // invalid or unsupported event
                                        // data format is found during this
                                        // process, the function will
                                        // immediately return FALSE.

    if ( IsItPhysicalDeviceEvent ( prEventData ) )
        {
                                        // Physical device event.

        uPhysicalDevice = CharToUpper ( prEventData->SE_ED_lpPathSpec[1] );

        SYM_ASSERT ( '0' < 'A' );

        uPhysicalDevice -= uPhysicalDevice < 'A' ? '0' - 0x80 : 'A';
        }
    else if ( IsItLogicalDeviceEvent ( prEventData ) )
        {
                                        // Logical device event.

        chLogicalDevice = CharToUpper ( prEventData->SE_ED_lpPathSpec[0] );
        }
    else
        {
                                        // Invalid or unknown device ID.
        return ( FALSE );
        }

                                        // Filter events and set
                                        // *prDeviceInfo.

    switch ( prEventData->SE_ED_etEventType )
        {
        case SE_DEV_WRITE:

                                        // During SE_DEV_WRITE, we can
                                        // have both logical and physical
                                        // accesses.  We need to perform
                                        // different types of search for each
                                        // one of them.

            if ( IsItPhysicalDeviceEvent ( prEventData ) )
                {
                                        // Process physical writes.

                prDeviceEntry = prDeviceInfoListStart;

                while ( prDeviceEntry &&
                        prDeviceEntry->uPhysicalDevice != uPhysicalDevice )
                    {
                    prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
                    }

                if ( !prDeviceEntry )
                    {
                    return ( FALSE );
                    }

                if ( prDeviceEntry->bPartitionable )
                    {
                                        // If device is partitionable, then
                                        // we need to check whether it is
                                        // being written to the MBR or to a
                                        // boot record.

                    if ( prEventData->SE_ED_dwRangeStart == 0 )
                        {
                                        // Writing to MBR.

                        *prDeviceInfo = *prDeviceEntry;

                        prDeviceInfo->chLogicalID = '\0';
                        }
                    else
                        {
                                        // Physically writing to boot.  We
                                        // have to find out to which record.

                        while ( prDeviceEntry &&
                                ( prDeviceEntry->uPhysicalDevice != uPhysicalDevice ||
                                  !prDeviceEntry->bPartitionable ||
                                  prDeviceEntry->uBootSectorLocation != prEventData->SE_ED_dwRangeStart )
                              )
                            {
                            prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
                            }

                        if ( !prDeviceEntry )
                            {
                            return ( FALSE );
                            }

                        *prDeviceInfo = *prDeviceEntry;

                        prDeviceInfo->chPhysicalID = '\0';
                        }
                    }
                else
                    {
                                        // If device is not partitionable,
                                        // then we should treat the access
                                        // as logical.

                    *prDeviceInfo = *prDeviceEntry;

                    prDeviceInfo->chPhysicalID = '\0';
                    }
                }
            else
                {
                                        // Process logical writes.

                prDeviceEntry = prDeviceInfoListStart;

                while ( prDeviceEntry &&
                        prDeviceEntry->chLogicalID != chLogicalDevice )
                    {
                    prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
                    }

                if ( !prDeviceEntry )
                    {
                    return ( FALSE );
                    }

                                        // Copy device information.

                *prDeviceInfo = *prDeviceEntry;

                                        // Make sure the client knows that
                                        // it is logical access.

                prDeviceInfo->chPhysicalID = '\0';
                }

            break;

        case SE_DEV_FORMAT:

                                        // SE_DEV_FORMAT will be generated
                                        // for physical devices.

            SYM_ASSERT ( IsItPhysicalDeviceEvent ( prEventData ) );

                                        // Process physical formats.

            prDeviceEntry = prDeviceInfoListStart;

            while ( prDeviceEntry &&
                    prDeviceEntry->uPhysicalDevice != uPhysicalDevice )
                {
                prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
                }

            if ( !prDeviceEntry )
                {
                return ( FALSE );
                }

            *prDeviceInfo = *prDeviceEntry;

            prDeviceInfo->chLogicalID = '\0';

            break;

        case SE_DEV_READ:

                                        // SE_DEV_READ will be generated
                                        // for logical devices.

            SYM_ASSERT ( IsItLogicalDeviceEvent ( prEventData ) );

                                        // Search for the drive that is
                                        // being read.

            prDeviceEntry = prDeviceInfoListStart;

            while ( prDeviceEntry &&
                    prDeviceEntry->chLogicalID != chLogicalDevice )
                {
                prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
                }

            if ( !prDeviceEntry )
                {
                return ( FALSE );
                }

                                        // Copy device information.

            *prDeviceInfo = *prDeviceEntry;

                                        // Make sure the client knows that
                                        // it is logical access.

            prDeviceInfo->chPhysicalID = '\0';

            break;

        default:

            SYM_ASSERT ( FALSE );

            return ( FALSE );
        }

    return ( TRUE );

#endif
    }

//---------------------------------------------------------------------------
//
// BOOL AddEntryToDeviceList (
//     PDEVICEINFOREC prDeviceInfo
//     );
//
// This function adds a new device list entry.  It allocates memory for
// the new entry, copies the device information from *prDeviceInfo to the
// newly allocated block, and then links the entry to the list pointed
// by prDeviceInfoListStart.
//
// Entry:
//      prDeviceInfo - pointer to a structure containing the device info
//
// Exit:
//      TRUE if successful, FALSE if error creating the entry.
//
//---------------------------------------------------------------------------
BOOL AddEntryToDeviceList (
    PDEVICEINFOREC prDeviceInfo
    )
    {
    auto PDEVICEINFOREC prDeviceEntry;

                                        // Validate input parameters.

    SYM_ASSERT ( prDeviceInfo );

                                        // Allocate memory for the entry.

    prDeviceEntry =
        (PDEVICEINFOREC) MemAllocPtr ( GHND, sizeof ( *prDeviceEntry ) );

    if ( !prDeviceEntry )
        {
        UIOutOfMemoryError();
        return ( FALSE );
        }

                                        // Initialize the entry.

    *prDeviceEntry = *prDeviceInfo;

    prDeviceEntry->prNextDeviceInfo = prDeviceInfoListStart;

    prDeviceInfoListStart = prDeviceEntry;

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL AddExtendedPartitionsToDeviceList (
//      PABSDISKREC prAbsDiskRec,
//      UINT        uExtendedPartitionLink,
//      UINT        uPartition
//      );
//
// This function adds an extended partition entry to the device list.  If
// the entry which will be added contains another extended partition entry,
// this function will be recursively called to add the next entry.  The
// number of nested calls is currently set to 24.
//
// Entry:
//      prAbsDiskRec           - pointer to a standard ABSDISKREC
//                               containing information about the drive
//                               which this function will work on;
//                               Starting head, track and sector should
//                               contain extended partition's location;
//                               buffer should also be pointing to a
//                               memory block for local usage
//      uExtendedPartitionLink - extended partition link level (if the
//                               extended partition is located in the
//                               MBR, set this parameter to 1; any
//                               consecutively linked extended partitions
//                               should increment this value)
//      uPartition             - partition in the MBR which started the
//                               extended partition chain
//
// Exit:
//      TRUE if successful, FALSE if error creating entry/entries.
//
// Note:
//      The buffer at prAbsDiskRec->buffer will be destroyed upon return
//
//---------------------------------------------------------------------------
BOOL AddExtendedPartitionsToDeviceList (
    PABSDISKREC prAbsDiskRec,
    UINT        uExtendedPartitionLink,
    UINT        uPartition
    )
    {
    auto PPARTENTRYREC prPartition;
    auto DEVICEINFOREC rDeviceInfo;
    auto DWORD         dwTrack;
    auto DWORD         dwSector;

                                        // Validate input parameters.

    SYM_ASSERT ( prAbsDiskRec );
    SYM_ASSERT ( prAbsDiskRec->buffer );

                                        // Make sure that this function is
                                        // not being called recursively too
                                        // many times.  Currently 24 is the
                                        // max.

    if ( uExtendedPartitionLink > 24 )
        {
        return ( FALSE );
        }

                                        // Read the sector with the extended
                                        // partition.

    prAbsDiskRec->dwHead     = prAbsDiskRec->dwStartingHead;
    prAbsDiskRec->dwTrack    = prAbsDiskRec->dwStartingTrack;
    prAbsDiskRec->dwSector   = prAbsDiskRec->dwStartingSector;
    prAbsDiskRec->numSectors = 1;

    if ( DiskAbsOperation ( READ_COMMAND, prAbsDiskRec ) != 0 )
        {
        return ( FALSE );
        }

                                        // Add partition to the device list.

    prPartition = (PPARTENTRYREC) (prAbsDiskRec->buffer + PART_TABLE_OFFSET);

    if ( prPartition->system == SYSTEM_DOS_12 ||
         prPartition->system == SYSTEM_DOS_16 ||
         prPartition->system == SYSTEM_DOS_32 ||
         prPartition->system == SYSTEM_BIGDOS )
        {
        rDeviceInfo.uPhysicalDevice     = prAbsDiskRec->dn;
        rDeviceInfo.bVisible            = TRUE;
        rDeviceInfo.bPartitionable      = TRUE;
        rDeviceInfo.uPartition          = uPartition;
        rDeviceInfo.uExtPartition       = uExtendedPartitionLink;
        rDeviceInfo.bPartitionBootable  = FALSE;
        rDeviceInfo.uBootSectorLocation = DiskGetSectorStartOffset (
                                                  prAbsDiskRec,
                                                  prPartition );
        rDeviceInfo.chPhysicalID        = 0;
        rDeviceInfo.chLogicalID         = 0;
        rDeviceInfo.hEvent              = INVALID_EVENT_HANDLE;

                                        // Add entry to list.

        if ( !AddEntryToDeviceList ( &rDeviceInfo ) )
            {
            return ( FALSE );
            }
        }

                                        // Check if there is another extended
                                        // partition linked to the current
                                        // one.

    prPartition++;

    if ( prPartition->system == SYSTEM_EXTENDED )
        {
        DiskUnscrambleBIOSCoding ( *((LPWORD) &(prPartition->startSector)),
                                   &dwTrack,
                                   &dwSector );

        prAbsDiskRec->dwStartingHead    = prPartition->startHead;
        prAbsDiskRec->dwStartingTrack   = dwTrack;
        prAbsDiskRec->dwStartingSector  = dwSector;

        return ( AddExtendedPartitionsToDeviceList (
                    prAbsDiskRec,
                    uExtendedPartitionLink + 1,
                    uPartition ) );
        }

                                        // Return to caller.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL EnumerateDeviceList (
//     VOID
//     );
//
// This function gives logical IDs to all devices in the list pointed
// by prDeviceInfoListStart.
//
// Entry:
//      -
//
// Exit:
//      Returns always TRUE.
//
// Notes:
//      Performance is very bad, but it should not cause problems because
//      EnumerateDeviceList() is executed only during initialization.
//      The strings in this function should not be translated.
//      This function should not be called for NEC hardware.
//
//---------------------------------------------------------------------------
BOOL EnumerateDeviceList (
    VOID
    )
    {
    auto PDEVICEINFOREC prDeviceEntry, prFirstPartition;
    auto UINT           uDeviceCounter;
    auto TCHAR          chNextDeviceID, chLastRemovableDeviceID;

                                        // Initialize local variables.

    chLastRemovableDeviceID = '\0';

    chNextDeviceID = 'C';

                                        // Step 1: Assign logical and
                                        // physical IDs to non-partitionable
                                        // devices (FDs) (only A: and B:).

    prDeviceEntry = prDeviceInfoListStart;

    while ( prDeviceEntry )
        {
        if ( prDeviceEntry->bVisible &&
             !prDeviceEntry->bPartitionable &&
             prDeviceEntry->uPhysicalDevice <= 1 )
            {
            prDeviceEntry->chLogicalID = prDeviceEntry->chPhysicalID =
                prDeviceEntry->uPhysicalDevice + 'A';

            if ( chLastRemovableDeviceID < prDeviceEntry->chLogicalID )
                {
                chLastRemovableDeviceID = prDeviceEntry->chLogicalID;
                }
            }

        prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
        }

                                        // Step 2: Assign logical ID to the
                                        // first bootable partition on the
                                        // list of partitionable devices.
                                        // Note: the "if" for checking for
                                        // first partition will skip
                                        // extended partitions for obvious
                                        // reasons.

    prDeviceEntry = prDeviceInfoListStart;

    prFirstPartition = NULL;

    while ( prDeviceEntry )
        {
        if ( prDeviceEntry->bVisible &&
             prDeviceEntry->bPartitionable &&
             prDeviceEntry->bPartitionBootable )
            {
            if ( !prFirstPartition ||
                 prDeviceEntry->uPhysicalDevice < prFirstPartition->uPhysicalDevice ||
                 ( prDeviceEntry->uPhysicalDevice == prFirstPartition->uPhysicalDevice &&
                   prDeviceEntry->uPartition < prFirstPartition->uPartition
                 )
               )
                {
                prFirstPartition = prDeviceEntry;
                }
            }

        prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
        }

    if ( prFirstPartition )
        {
        prFirstPartition->chLogicalID = chNextDeviceID++;
        }
    else
        {
                                        // I would like to see this happen...

        SYM_ASSERT ( FALSE );
        }

                                        // Step 3: Assign logical IDs to
                                        // all other partitions.

    do
        {
        prFirstPartition = NULL;

        prDeviceEntry = prDeviceInfoListStart;

        while ( prDeviceEntry )
            {
            if ( prDeviceEntry->bVisible &&
                 prDeviceEntry->bPartitionable &&
                 prDeviceEntry->chLogicalID == '\0' )
                {
                if ( !prFirstPartition ||
                     prDeviceEntry->uPhysicalDevice < prFirstPartition->uPhysicalDevice ||
                     ( prDeviceEntry->uPhysicalDevice == prFirstPartition->uPhysicalDevice &&
                       ( prDeviceEntry->uPartition < prFirstPartition->uPartition ||
                         ( prDeviceEntry->uPartition == prFirstPartition->uPartition &&
                           prDeviceEntry->uExtPartition <= prFirstPartition->uExtPartition
                         )
                       )
                     )
                   )
                    {
                    prFirstPartition = prDeviceEntry;
                    }
                }

            prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
            }

        if ( prFirstPartition )
            {
            prFirstPartition->chLogicalID = chNextDeviceID++;
            }

        } while ( prFirstPartition );

                                        // Step 4: Assign physical IDs to
                                        // all partitionable devices.

    prDeviceEntry = prDeviceInfoListStart;

    while ( prDeviceEntry )
        {
        if ( prDeviceEntry->bVisible &&
             prDeviceEntry->bPartitionable )
            {
            prDeviceEntry->chPhysicalID = prDeviceEntry->uPhysicalDevice -
                                          0x80 + 'C';
            }

        prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
        }

                                        // Step 5: Assign IDs to the rest of
                                        // removable devices that were left
                                        // behind.

    if ( chNextDeviceID == 'C' )
        {
        chNextDeviceID = chLastRemovableDeviceID == '\0' ?
                            'A' : chLastRemovableDeviceID + 1;
        }

    for ( uDeviceCounter = 2; uDeviceCounter <= 255; uDeviceCounter++ )
        {
        prDeviceEntry = prDeviceInfoListStart;

        while ( prDeviceEntry )
            {
            if ( prDeviceEntry->uPhysicalDevice == uDeviceCounter &&
                 prDeviceEntry->bVisible &&
                 !prDeviceEntry->bPartitionable )
                {
                prDeviceEntry->chLogicalID = prDeviceEntry->chPhysicalID =
                    chNextDeviceID++;
                }

            prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
            }
        }

                                        // Return to caller.

    return ( TRUE );
    }
