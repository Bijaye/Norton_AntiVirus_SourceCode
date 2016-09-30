// Copyright 1995-1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/DEVICE.CPv   1.4   31 Aug 1998 18:02:22   jsulton  $
//
// Description:
//      This file defines helper functions for working with devices.
//
// Contains:
//      CreateDeviceList()
//      DestroyDeviceList()
//      GetDevice()
//      GetLogDrvFromPhysDev()
//      GetNextPartition()
//      GetNextDrive()
//      GetNextRemovableDevice()
//
//      AddEntryToDeviceList ()             (local function)
//      AddExtendedPartitionsToDeviceList() (local function)
//      EnumerateDeviceList()               (local function)
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/DEVICE.CPv  $
// 
//    Rev 1.4   31 Aug 1998 18:02:22   jsulton
// Changed SYM_CPU_ALPHA to _ALPHA_ so we build properly.
// 
//    Rev 1.3   26 Aug 1998 14:50:08   jsulton
// Changes made by Thuridion to work on the Alpha platform.
// 
//    Rev 1.2   19 Mar 1998 11:42:42   DALLEE
// Updated for wider DWORD-sized ABSDISKREC fields.
// 
//    Rev 1.1   04 Dec 1997 09:47:24   DALLEE
// Added usage counts for device info list and critical sections around
// create and destroy, so this can be shared by mutiple threads.
// 
//    Rev 1.0   02 Dec 1997 14:28:30   DALLEE
// Initial revision.
//***************************************************************************
// From NEWVIR-NAVOEM device.cpv r1.14
//
//    Rev 1.14   29 Sep 1997 14:29:14   TCASHIN
// VxDServiceInit() has to be #ifdef'd for Win32.
//***************************************************************************

#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "device.h"


//---------------------------------------------------------------------------
//
// Define local function prototypes.
//
//---------------------------------------------------------------------------
BOOL WINAPI AddEntryToDeviceList (
    LPDEVICEINFOREC prDeviceInfo );

BOOL WINAPI AddExtendedPartitionsToDeviceList (
    LPABSDISKREC prAbsDiskRec,
    UINT         uExtendedPartitionLink,
    UINT         uPartition );

BOOL WINAPI EnumerateDeviceList (
    VOID );

// Class for managing critical section usage. Will automatically
// LeaveCriticalSection() as it goes out of scope.

class CCritSec
{
public:
    CCritSec( LPCRITICAL_SECTION pCS );
    ~CCritSec();

    VOID Initialize();
    VOID Delete();

    VOID Enter();
    VOID Leave();

private:
    LPCRITICAL_SECTION  m_pCritSec;
    UINT                m_uEntries;
};

CCritSec::CCritSec( LPCRITICAL_SECTION pCS ) : 
    m_pCritSec( pCS ),
    m_uEntries( 0 )
{
}

CCritSec::~CCritSec()
{
    while ( m_uEntries )
    {
        Leave();
    }
}

VOID CCritSec::Initialize()
{
    InitializeCriticalSection( m_pCritSec );
}

VOID CCritSec::Delete()
{
    DeleteCriticalSection( m_pCritSec );
}

VOID CCritSec::Enter()
{
    m_uEntries++;
    EnterCriticalSection( m_pCritSec );
}

VOID CCritSec::Leave()
{
    SYM_ASSERT( m_uEntries );

    if ( m_uEntries )
    {
        m_uEntries--;
        LeaveCriticalSection( m_pCritSec );
    }
}

//---------------------------------------------------------------------------
//
// The following variables maintain the head of the device info list
// and synchronize access to the list.
// 
// s_uDeviceInfoListUsage and s_prDeviceInfoListStart are guarded by 
// s_rDeviceInfoListCritSec.
//
// Calls to CreateDeviceList() will increment the usage count. The first user
// will create the list. 
// Calls to DestroyDeviceList() will decrement the usage count. If the count
// reaches zero, the list will be deleted.
// DestroyDeviceList() must be called once and only once for each time 
// CreateDeviceList() is called regardless of the success of CreateDeviceList().
//
//---------------------------------------------------------------------------
static CRITICAL_SECTION s_rDeviceInfoListCritSec;

static UINT             s_uDeviceInfoListUsage;
static LPDEVICEINFOREC  s_prDeviceInfoListStart = NULL;


//---------------------------------------------------------------------------
//
// BOOL WINAPI InitDeviceListSynchronization (
//      VOID );
//
// BOOL WINAPI CloseDeviceListSynchronization (
//      VOID );
//
// These routines initializes or deinitialize the synchronization objects 
// which will be used to regulate access the the device info list.
// 
// These must be called before any access to the device info list, and after
// all access is finished. Should be called from process attach/detach in
// DllMain().
//
// Entry:
//      -
//
// Exit:
//      TRUE  if successful
//      FALSE on error (unexpected)
//
//---------------------------------------------------------------------------
BOOL WINAPI InitDeviceListSynchronization (
    VOID )
{
    auto    CCritSec    xDeviceListCritSec( &s_rDeviceInfoListCritSec );

    xDeviceListCritSec.Initialize();

    return ( TRUE );
} // InitDeviceListSynchronization()

BOOL WINAPI CloseDeviceListSynchronization (
    VOID )
{
    auto    CCritSec    xDeviceListCritSec( &s_rDeviceInfoListCritSec );

    xDeviceListCritSec.Delete();

    return ( TRUE );
} // CloseDeviceListSynchronization()


//---------------------------------------------------------------------------
//
// BOOL WINAPI CreateDeviceList (
//      VOID );
//
// This function creates a list with information about all existing
// int 13 - compatible devices on the system.  s_prDeviceInfoListStart will
// be set to point to the first entry of the device list.  Each entry of
// the device list contans a pointer to the next entry.  The last entry
// will have it's next entry pointer set to NULL.
//
// Entry:
//      -
//
// Exit:
//      TRUE  if successful
//            Device list fully initialized
//      FALSE if unable to create full list
//            Device list not initialized or partly initialized.
//
// Note:
//      Even if this function failed, DestroyDeviceList() should still
//      be called to clean up any allocated memory.
//
//---------------------------------------------------------------------------
BOOL WINAPI CreateDeviceList (
    VOID )
{
    auto    BOOL            bError;
    auto    BOOL            bDevicePartitionable, bHavePhysicalInfo;
    auto    BOOL            bNonPartDeviceCount, bPartDeviceCount;
    auto    UINT            uNonPartDeviceCount, uPartDeviceCount;
    auto    UINT            uDevice;
    auto    UINT            uPartitionCounter;
    auto    ABSDISKREC      rAbsDiskRec;
    auto    PartitionRec    rPartition;
    auto    DEVICEINFOREC   rDeviceInfo;
    auto    BYTE huge *     lpMiscBuffer;
    auto    UINT            uOldErrorMode = SEM_FAILCRITICALERRORS;

    auto    CCritSec        xDeviceListCritSec( &s_rDeviceInfoListCritSec );


    xDeviceListCritSec.Enter();

                                        // Register usage regardless of
                                        // success or failure.

    s_uDeviceInfoListUsage++;

                                        // If someone else has been here and
                                        // tried the initialization already,
                                        // then we're done...

    if ( s_uDeviceInfoListUsage > 1 )
    {
        return ( s_prDeviceInfoListStart != NULL );
    }

                                        // Otherwise, we need to create the
                                        // device info list.

#ifdef SYM_WIN32
#ifndef _ALPHA_
                                        // Startup the VxD (NAVSDK.VXD) on Win95.
                                        // Must succeed since we don't have
                                        // the thunk DLL's.

    if ( ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ) &&
         ( FALSE == VxDServicesInit() ) )
    {
        return ( FALSE );
    }

#endif //SYM_CPU_ALPHA
#endif

                                        // Initialize local variables

    bError              = FALSE;
    bNonPartDeviceCount = FALSE;
    bPartDeviceCount    = FALSE;

                                        // Allocate memory for IO buffer.
                                        // Its length should be at least the
                                        // size of the biggest possible disk
                                        // sector.
                                        // Setting this to 1K sectors.

    lpMiscBuffer =  (BYTE huge *) MemAllocPtr ( GHND, 0x400 );
    rAbsDiskRec.buffer = lpMiscBuffer;

    if ( rAbsDiskRec.buffer == NULL )
        return (FALSE);

                                        // Set error mode to disable 
                                        // Abort, Retry, Ignore.

    uOldErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );

                                        // Scan for int - 13 devices and add
                                        // an entry (multi - partitioned
                                        // fixed devices will have more than
                                        // one) for each one of them

    for ( uDevice = 0; uDevice <= 255 && !bError; uDevice++ )
    {
                                        // Initialize local variables.

        rAbsDiskRec.dn = uDevice;
        bHavePhysicalInfo = FALSE;
        bDevicePartitionable = ( uDevice >= 0x80 ) ? TRUE : FALSE;

                                        // Make sure we have a counter for
                                        // this type of devices.

        if ( ( bDevicePartitionable ? bPartDeviceCount :
                                      bNonPartDeviceCount ) == FALSE )
        {
#ifdef SYM_WIN32
            if ( DiskGetPhysicalInfo( &rAbsDiskRec ) != 0 )
            {
                //***********************************************************
                // Kluge Alert!
                //
                // Under NT, we don't get low level device info for floppies
                // when there is no disk in the drive. Hence, we don't make
                // an entry in this device list. Fake it out here, and add
                // the device anyway.
                //***********************************************************

                if ( ( uDevice < 0x80 ) && DiskExists( 'A'+uDevice ) )
                {
                                        // If device is not partitionable,
                                        // then assume there is no MBR.

                    rDeviceInfo.uPhysicalDevice     = uDevice;
                    rDeviceInfo.bVisible            = TRUE;
                    rDeviceInfo.bPartitionable      = FALSE;
                    rDeviceInfo.uPartition          = 0;
                    rDeviceInfo.uExtPartition       = 0;
                    rDeviceInfo.bPartitionBootable  = FALSE;
                    rDeviceInfo.uBootSectorLocation = 0;
                    rDeviceInfo.chPhysicalID        = 0;
                    rDeviceInfo.chLogicalID         = 0;

                                        // Add entry to list

                    bError = AddEntryToDeviceList ( (LPDEVICEINFOREC)&rDeviceInfo ) ? FALSE : TRUE;
                }

                continue;
            }
#else
            if ( _DiskGetPhysicalInfo ( &rAbsDiskRec ) != 0 )
            {
                continue;
            }
#endif

            bHavePhysicalInfo = TRUE;

            if ( bDevicePartitionable == FALSE )
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
                                        // non-partitionable devices

        if ( ( FALSE == bDevicePartitionable ) &&
             ( uNonPartDeviceCount <= uDevice ) )
        {
            continue;
        }

                                        // Check if we are within the
                                        // counter limits of partitionable
                                        // devices

        if ( bDevicePartitionable == TRUE &&
             uPartDeviceCount <= (uDevice - 0x80) )
        {
            continue;
        }

                                        // See if current device exists.

        if ( bHavePhysicalInfo == FALSE &&
#ifdef SYM_WIN32
             DiskGetPhysicalInfo ( &rAbsDiskRec ) != 0 )
#else
             _DiskGetPhysicalInfo ( &rAbsDiskRec ) != 0 )
#endif
        {
            continue;
        }

                                        // Prepare an entry to add to the
                                        // list.

        if ( bDevicePartitionable == FALSE )
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

                                        // Add entry to list

            bError = AddEntryToDeviceList ( (LPDEVICEINFOREC)&rDeviceInfo ) ? FALSE : TRUE;

            continue;
        }

                                        // If device is partitionable, then
                                        // create a separate entry for each
                                        // partition.

        for ( uPartitionCounter = 0;
              uPartitionCounter < 4 && !bError;
              uPartitionCounter++ )
        {
                                        // Get partition information

            if ( DiskGetPartitionTableEntry ( rAbsDiskRec.dn,
                                              uPartitionCounter,
                                              &rPartition,
                                              &rAbsDiskRec ) == FALSE )
            {
                                        // If error, just let go and do
                                        // not report an error.  The user
                                        // is in big trouble any way.
                continue;
            }

                                        // Validate the partition.

            if ( rPartition.system != SYSTEM_DOS_12 &&
                 rPartition.system != SYSTEM_DOS_16 &&
                 rPartition.system != SYSTEM_BIGDOS &&
                 rPartition.system != SYSTEM_EXTENDED &&
                 rPartition.system != SYSTEM_HPFS &&
                 rPartition.bootable == FALSE )
            {
                continue;
            }

                                        // If extended partition, then
                                        // process the extended partition
                                        // chain.

            if ( rPartition.system == SYSTEM_EXTENDED )
            {
                rAbsDiskRec.buffer = lpMiscBuffer;
                bError = AddExtendedPartitionsToDeviceList (
                            (ABSDISKREC FAR *)&rAbsDiskRec,
                            1,
                            uPartitionCounter ) ? FALSE : TRUE;

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

                                        // Add entry to list

            bError = AddEntryToDeviceList ( &rDeviceInfo ) ? FALSE : TRUE;
        }

        rAbsDiskRec.buffer = lpMiscBuffer;
    }


    if ( !bError && ( SystemGetWindowsType() != SYM_SYSTEM_WIN_NT ) )
    {
        EnumerateDeviceList();
    }

                                        // Free allocated memory

    SYM_VERIFY( 0 == MemFreePtr( rAbsDiskRec.buffer ) );

                                        // Reset error mode

    if ( SEM_FAILCRITICALERRORS != uOldErrorMode )
        SetErrorMode( uOldErrorMode );

                                        // Return to caller

    return ( bError ? FALSE : TRUE );
} // CreateDeviceList()


//---------------------------------------------------------------------------
//
// BOOL WINAPI DestroyDeviceList (
//      VOID );
//
// This function destroys the device list pointed by s_prDeviceInfoListStart.
// It deallocates all memory that had been allocated for the list by
// CreateDeviceList().
//
// Entry:
//      -
//
// Exit:
//      TRUE  if successful
//      FALSE if unable to destroy list
//
// Note:
//      This function returns always TRUE.
//
//---------------------------------------------------------------------------
BOOL WINAPI DestroyDeviceList (
    VOID )
{
    auto    LPDEVICEINFOREC prNextDeviceInfo;
    auto    CCritSec        xDeviceListCritSec( &s_rDeviceInfoListCritSec );

    SYM_ASSERT( s_uDeviceInfoListUsage );

    xDeviceListCritSec.Enter();

    if ( s_uDeviceInfoListUsage )
    {
        s_uDeviceInfoListUsage--;

        if ( 0 == s_uDeviceInfoListUsage )
        {
                                        // Free the memory allocated for
                                        // each entry in the list.

            while ( s_prDeviceInfoListStart != NULL )
            {
                prNextDeviceInfo = s_prDeviceInfoListStart->prNextDeviceInfo;
                SYM_VERIFY( 0 == MemFreePtr( s_prDeviceInfoListStart ) );
                s_prDeviceInfoListStart = prNextDeviceInfo;
            }
        }
    }

    return ( TRUE );
} // DestroyDeviceList()


//---------------------------------------------------------------------------
//
// LPDEVICEINFOREC WINAPI GetDeviceListHead (
//      VOID );
//
// This function returns the head of the device list.
//
// Entry:
//      None
//
// Exit:
//      Pointer to the first entry in the device list
//
//---------------------------------------------------------------------------
LPDEVICEINFOREC WINAPI GetDeviceListHead (
    VOID )
{
    return ( s_prDeviceInfoListStart );
} // GetDeviceListHead()


//---------------------------------------------------------------------------
//
// BYTE WINAPI GetLogDrvFromPhysDev(
//       BYTE    byPhysicalDeviceNum,
//       BYTE    byPartitionNum );
//
// This function returns the logical drive letter assigned to the
// specified physical device number.
//
// Entry:
//      byLogicalDrive - Logical drive to find
//
// Exit:
//      Logical drive letter.
//
//---------------------------------------------------------------------------
BYTE WINAPI GetLogDrvFromPhysDev (
    BYTE    byPhysicalDeviceNum,
    BYTE    byPartitionNum )
{
    auto LPDEVICEINFOREC prDeviceInfo;
    auto BYTE            byLogicalDrive = ' ';
    auto BYTE            byPart;

                                        // If there was a virus in the MBR, the
                                        // partition number will be 0xFF. Change it
                                        // to 0.
    if ( byPartitionNum = 0xFF )
        byPart = 0;
    else
        byPart = byPartitionNum;

                                        // Search device list for the
                                        // physical device number and partition.

    for ( prDeviceInfo = s_prDeviceInfoListStart;
          prDeviceInfo != NULL;
          prDeviceInfo = prDeviceInfo->prNextDeviceInfo )
    {
        if ( ( prDeviceInfo->uPhysicalDevice == byPhysicalDeviceNum ) &&
             ( prDeviceInfo->uPartition == byPart ) )
        {
            return ( prDeviceInfo->chLogicalID );
        }
    }

    return ( ' ' );
} // GetLogDrvFromPhysDev()


//---------------------------------------------------------------------------
//
// LPDEVICEINFOREC WINAPI GetNextRemovableDevice (
//      LPDEVICEINFOREC prDeviceInfo );
//
// This function searches the device list for the next removable device.
// The search will start at entry after the entry pointed by prDeviceInfo.
// If prDeviceInfo is NULL, then the search will start at the beginning
// of the list.
//
// Entry:
//      prDeviceInfo - pointer to device info entry
//
// Exit:
//      Pointer to next removable device in the device list, or NULL
//      if no more removable devices
//
//---------------------------------------------------------------------------
LPDEVICEINFOREC WINAPI GetNextRemovableDevice (
    LPDEVICEINFOREC prDeviceInfo )
{
                                        // Get starting search entry.

    prDeviceInfo = prDeviceInfo == NULL ? s_prDeviceInfoListStart :
                                          prDeviceInfo->prNextDeviceInfo;

                                        // Search device list for next
                                        // removable entry.

    while ( prDeviceInfo != NULL &&
            prDeviceInfo->uPhysicalDevice >= 0x80 )
    {
        prDeviceInfo = prDeviceInfo->prNextDeviceInfo;
    }

    return ( prDeviceInfo );
} // GetNextRemovableDevice()


//---------------------------------------------------------------------------
//
// LPDEVICEINFOREC WINAPI GetLogicalDevice (
//      BYTE    byLogicalDrive );
//
// This function searches the device list for the specified logical drive.
// The search will start at the beginning of the list.
//
// Entry:
//      byLogicalDrive - Logical drive to find
//
// Exit:
//      Pointer to the first entry in the device list of this logical drive
//              or NULL if this drive is not found.
//
//---------------------------------------------------------------------------
LPDEVICEINFOREC WINAPI GetLogicalDrive (
    BYTE    byLogicalDrive )
{
    auto LPDEVICEINFOREC prDeviceInfo;

    prDeviceInfo = s_prDeviceInfoListStart;

                                        // Search device list for the
                                        // physical device number.

    while ( prDeviceInfo != NULL &&
            prDeviceInfo->chLogicalID != byLogicalDrive )
    {
        prDeviceInfo = prDeviceInfo->prNextDeviceInfo;
    }

    return ( prDeviceInfo );
} // GetLogicalDrive()


//---------------------------------------------------------------------------
//
// LPDEVICEINFOREC WINAPI GetPhysicalDevice (
//      UINT    uPhysicalDeviceNumber );
//
// This function searches the device list for the specified physical device.
// The search will start at the beginning of the list.
//
// Entry:
//      uPhysicalDeviceNumber - Device number to find
//
// Exit:
//      Pointer to the first entry in the device list of this device number
//              or NULL if this device number is not found.
//
//---------------------------------------------------------------------------
LPDEVICEINFOREC WINAPI GetPhysicalDevice (
    UINT    uPhysicalDeviceNumber )
{
    auto LPDEVICEINFOREC prDeviceInfo;

    prDeviceInfo = s_prDeviceInfoListStart;

                                        // Search device list for the
                                        // physical device number.

    while ( prDeviceInfo != NULL &&
            prDeviceInfo->uPhysicalDevice != uPhysicalDeviceNumber )
    {
        prDeviceInfo = prDeviceInfo->prNextDeviceInfo;
    }

    return ( prDeviceInfo );
} // GetPhysicalDevice()


//---------------------------------------------------------------------------
//
// LPDEVICEINFOREC WINAPI GetNextDevice (
//      LPDEVICEINFOREC prDeviceInfo );
//
// This function searches the device list for the next fixed device.
// The search will start at entry after the entry pointed by prDeviceInfo.
// If prDeviceInfo is NULL, then the search will start at the beginning
// of the list.
//
// Entry:
//      prDeviceInfo - pointer to device info entry
//
// Exit:
//      Pointer to next fixed device in the device list, or NULL if there are
//      no more devices.
//
//---------------------------------------------------------------------------
LPDEVICEINFOREC WINAPI GetNextDevice (
    LPDEVICEINFOREC prDeviceInfo )
{
    auto    LPDEVICEINFOREC pNextDevice;

                                        // Get starting search entry.

    pNextDevice = prDeviceInfo == NULL ? s_prDeviceInfoListStart :
                                         prDeviceInfo->prNextDeviceInfo;

                                        // Search device list for next
                                        // fixed device entry.

    while ( pNextDevice != NULL )
    {
        if ( pNextDevice->uPhysicalDevice >= 0x80 &&
             pNextDevice->uPhysicalDevice != prDeviceInfo->uPhysicalDevice)
        {
            return ( pNextDevice );
        }
        else
        {
            pNextDevice = pNextDevice->prNextDeviceInfo;
        }
    }

    return ( pNextDevice );
} // GetNextDevice()


//---------------------------------------------------------------------------
//
// LPDEVICEINFOREC WINAPI GetNextPartition (
//      LPDEVICEINFOREC prDeviceInfo );
//
// This function searches the device list for the next partition for the.
// current physical drive. The search will start at entry after the entry
// pointed by prDeviceInfo.
// prDeviceInfo must not be NULL.
//
// Entry:
//      prDeviceInfo - pointer to device info entry
//
// Exit:
//      Pointer to next partition of current device in the device list, or NULL
//      if no more partitions.
//
//---------------------------------------------------------------------------
LPDEVICEINFOREC WINAPI GetNextPartition (
    LPDEVICEINFOREC prDeviceInfo )
{
    auto    UINT    uCurrentPartition;
    auto    UINT    uCurrentPhysDevice;

    uCurrentPartition = prDeviceInfo->uPartition;
    uCurrentPhysDevice = prDeviceInfo->uPhysicalDevice;

                                        // Get starting search entry.

    prDeviceInfo = prDeviceInfo->prNextDeviceInfo;

                                        // Search device list for next
                                        // partition.

    while ( prDeviceInfo != NULL )
    {
        if ( prDeviceInfo->uPhysicalDevice == uCurrentPhysDevice &&
             prDeviceInfo->uPartition != uCurrentPartition )
            break;

        prDeviceInfo = prDeviceInfo->prNextDeviceInfo;
    }

    return ( prDeviceInfo );
} // GetNextPartition()


//---------------------------------------------------------------------------
//
// BOOL WINAPI EnumerateDeviceList (
//     VOID );
//
// This function gives logical IDs to all devices in the list pointed
// by s_prDeviceInfoListStart.
//
// Entry:
//      -
//
// Exit:
//      TRUE  if successful
//      FALSE if failed to enumerate the list
//
// Notes:
//      This function returns always TRUE.
//      Performance is very slow, but it should not cause problems
//      because this will be executed only once during initialization.
//      The strings in this function should not be translated.
//
//---------------------------------------------------------------------------
BOOL WINAPI EnumerateDeviceList (
    VOID )
{
    auto LPDEVICEINFOREC prDeviceEntry, prFirstPartition;
    auto UINT            uDeviceCounter;
    auto char            chNextDeviceID, chLastRemovableDeviceID;

                                        // Initialize local variables

    chLastRemovableDeviceID = '\0';

    chNextDeviceID = 'C';

                                        // Step 1: Assign logical and
                                        // physical IDs to non-partitionable
                                        // devices (FDs) (only A: and B:)

    prDeviceEntry = s_prDeviceInfoListStart;

    while ( prDeviceEntry != NULL )
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

    prDeviceEntry = s_prDeviceInfoListStart;

    prFirstPartition = NULL;

    while ( prDeviceEntry != NULL )
    {
        if ( prDeviceEntry->bVisible &&
             prDeviceEntry->bPartitionable &&
             prDeviceEntry->bPartitionBootable )
        {
            if ( prFirstPartition == NULL ||
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

    if ( prFirstPartition != NULL )
    {
        prFirstPartition->chLogicalID = chNextDeviceID++;
    }
    else
    {
                                        // I would like to see this happen...
                                        //&? DALLEE - won't this happen w/ no
                                        // fixed drives? Or on NT, if we don't
                                        // have privileges to open handles to
                                        // physical devices, and we have some
                                        // bogus floppy entries due to the kluge
                                        // in CreateDeviceList()

        SYM_ASSERT ( FALSE );
    }

                                        // Step 3: Assign logical IDs to
                                        // all other partitions.

    do
    {
        prFirstPartition = NULL;

        prDeviceEntry = s_prDeviceInfoListStart;

        while ( prDeviceEntry != NULL )
        {
            if ( prDeviceEntry->bVisible &&
                 prDeviceEntry->bPartitionable &&
                 prDeviceEntry->chLogicalID == '\0' )
            {
                if ( prFirstPartition == NULL ||
                     prDeviceEntry->uPhysicalDevice < prFirstPartition->uPhysicalDevice ||
                     ( prDeviceEntry->uPhysicalDevice == prFirstPartition->uPhysicalDevice &&
                       ( prDeviceEntry->uPartition < prFirstPartition->uPartition ||
                         ( prDeviceEntry->uPartition == prFirstPartition->uPartition &&
                           prDeviceEntry->uExtPartition <= prFirstPartition->uExtPartition ) ) ) )
                {
                    prFirstPartition = prDeviceEntry;
                }
            }

            prDeviceEntry = prDeviceEntry->prNextDeviceInfo;
        }

        if ( prFirstPartition != NULL )
        {
            prFirstPartition->chLogicalID = chNextDeviceID++;
        }

    } while ( prFirstPartition != NULL );

                                        // Step 4: Assign physical IDs to
                                        // all partitionable devices

    prDeviceEntry = s_prDeviceInfoListStart;

    while ( prDeviceEntry != NULL )
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
        prDeviceEntry = s_prDeviceInfoListStart;

        while ( prDeviceEntry != NULL )
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
                                        // Return to caller

    return ( TRUE );
} // EnumerateDeviceList()


//---------------------------------------------------------------------------
//
// BOOL WINAPI AddEntryToDeviceList (
//     LPDEVICEINFOREC prDeviceInfo );
//
// This function adds a new device list entry.  It allocates memory for
// the new entry, copies the device information from *prDeviceInfo to the
// newly allocated block, and then links the entry to the list pointed
// by s_prDeviceInfoListStart.
//
// Entry:
//      prDeviceInfo - pointer to a structure containing the device info
//
// Exit:
//      TRUE  if successful
//      FALSE if failed to create the entry
//
//---------------------------------------------------------------------------
BOOL WINAPI AddEntryToDeviceList (
    LPDEVICEINFOREC prDeviceInfo )
{
    auto LPDEVICEINFOREC prDeviceEntry;

                                        // Validate input parameters

    SYM_ASSERT ( prDeviceInfo != NULL );

                                        // Allocate memory for the entry

    prDeviceEntry =
        (LPDEVICEINFOREC) MemAllocPtr ( GHND, sizeof ( *prDeviceEntry ) );

    if ( prDeviceEntry == NULL )
    {
        return ( FALSE );
    }
                                        // Initialize the entry

    *prDeviceEntry = *prDeviceInfo;

    prDeviceEntry->prNextDeviceInfo = s_prDeviceInfoListStart;

    s_prDeviceInfoListStart = prDeviceEntry;

                                        // Return to caller

    return ( TRUE );
} // AddEntryToDeviceList()


//---------------------------------------------------------------------------
//
// BOOL WINAPI AddExtendedPartitionsToDeviceList (
//      LPABSDISKREC   prAbsDiskRec,
//      UINT           uExtendedPartitionLink,
//      UINT           uPartition );
//
// This function adds an extended partition entry to the device list.  If
// the entry which will be added contains another extended partition entry,
// this function will be recursively called to add the next entry.
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
//      TRUE  if successful
//      FALSE if failed to create the entry/entries
//
// Note:
//      The buffer at prAbsDiskRec->buffer will be destroyed upon return
//
//---------------------------------------------------------------------------
BOOL WINAPI AddExtendedPartitionsToDeviceList (
    LPABSDISKREC prAbsDiskRec,
    UINT         uExtendedPartitionLink,
    UINT         uPartition )
{
    auto LPPARTENTRYREC prPartition;
    auto DEVICEINFOREC  rDeviceInfo;
    auto DWORD          dwTrack;
    auto DWORD          dwSector;

                                        // Validate input parameters

    SYM_ASSERT ( prAbsDiskRec != NULL );
    SYM_ASSERT ( prAbsDiskRec->buffer != NULL );

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

    prAbsDiskRec->dwHead        = prAbsDiskRec->dwStartingHead;
    prAbsDiskRec->dwTrack       = prAbsDiskRec->dwStartingTrack;
    prAbsDiskRec->dwSector      = prAbsDiskRec->dwStartingSector;
    prAbsDiskRec->numSectors  = 1;

    if ( DiskAbsOperation ( READ_COMMAND, prAbsDiskRec ) != 0 )
    {
        return ( FALSE );
    }
                                        // Add partition to the device list

    prPartition = (LPPARTENTRYREC) (prAbsDiskRec->buffer + PART_TABLE_OFFSET);

    if ( prPartition->system == SYSTEM_DOS_12 ||
         prPartition->system == SYSTEM_DOS_16 ||
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

                                        // Add entry to list

        if ( AddEntryToDeviceList ( &rDeviceInfo ) == FALSE )
        {
            return ( FALSE );
        }
    }
                                        // Check if there is another extended
                                        // partition linked to the current
                                        // one

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

                                        // Return to caller

    return ( TRUE );
} // AddExtendedPartitionsToDeviceList()


