//--------------------------------------------------------------------
// File Name: AVGLOB.CPP
// Description: init certlib low level io functions.
//
// BOOL SYM_EXPORT WINAPI InitCertlib (
//        LPCALLBACKREV1 lpCallBackTable,
//        WORD           wFlag );
//--------------------------------------------------------------------
// $Header:   S:/CERTLIBN/VCS/avglob.cpv   1.19   17 Mar 1998 11:54:20   MKEATIN  $
//--------------------------------------------------------------------
// $Log:   S:/CERTLIBN/VCS/avglob.cpv  $
// 
//    Rev 1.19   17 Mar 1998 11:54:20   MKEATIN
// Missed a few lines for those disk.h changes.
// 
//    Rev 1.18   17 Mar 1998 11:41:20   MKEATIN
// Updated variables to match those found in the new disk.h
// 
//    Rev 1.17   17 Jul 1996 21:17:10   RAY
// Fixed LocalDiskPhysicalWrite() to write sectors under NT using a
// logical drive handle where appropriate.  Added LocalNTDiskWriteLogical(),
// LocalNTDeterminePartNum(), and LocalNTDetermineDriveLetter().
// 
//    Rev 1.16   27 Jun 1996 15:36:34   DALLEE
// LuigiPlus port:
// Changes to InitCertlib() to maintain a static table of callbacks.
// Loses functionality -- local routines here will always be used by
// CERTLIBN; routines passed in the callback structure will be ignored.
// 
// 
//    Rev 1.15   03 May 1996 14:53:58   DALLEE
// Modified ASSERT in FileWrite to allow NULL buffer if writing 
// zero bytes to truncate.
// 
//    Rev 1.14   09 Apr 1996 10:34:48   RSTANEV
// Initializing lpgCallback to NULL.  This forces its definition in the
// current data segment, not ".bss".  It makes it easier on SYM_NTK.
//
//    Rev 1.13   02 Feb 1996 19:39:50   RSTANEV
// Changed prototypes of some functions to match callback structure.
//
//    Rev 1.12   11 Jan 1996 16:45:34   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.10.1.6   15 Dec 1995 14:29:22   DALLEE
// For W32 Read Physical, allocate a temporary buffer which is aligned
// on a 4K memory page boundary.  Straddling two 4K pages will cause
// reads from Promise EIDE 4030+ drivers to silently fail.
//
// Also, Read/Write physical: Moved the DiskGetPhysicalInfo() call before
// the Lock volume call - not necessary to lock for this call and it simplified
// the error handling.
//
// And finally, Read/Write physical: When exiting, use the same check vs.
// what location we read to determine whether to call Unlock volume or not.
//
//    Rev 1.10.1.5   11 Dec 1995 22:45:28   MKEATIN
// Modified Disklocking of MBR's in LocalDiskReadPhysical and
// LocalDiskWritePhysical
//
//    Rev 1.10.1.4   23 Oct 1995 13:28:34   CNACHEN
// Changed SYM_W32 to SYM_WIN32
//
//    Rev 1.10.1.3   23 Oct 1995 10:43:02   CNACHEN
// Moved VirusScanLockUnlockDiskP and VirusScanLockUnlockDiskL into AVGLOB
// from Virscan project...
//
//
//    Rev 1.10.1.2   19 Oct 1995 10:08:08   CNACHEN
// Aded IFDEF's for WIN32 platform and file locking...
//
//    Rev 1.10.1.1   18 Oct 1995 14:33:18   CNACHEN
// Added drive locking and unlocking...
//
//    Rev 1.10.1.0   12 Jul 1995 18:58:06   MPYE
// Branch base for version QAK8
//
//    Rev 1.10   22 Feb 1995 14:05:26   DALLEE
// #ifdef'd DiskGetPhysicalInfo() call for DOS and W32.
//
//    Rev 1.9   21 Feb 1995 19:08:40   DALLEE
// Make call to DiskGetPhysicalInfo() in physical read/write functions.
// ABSDISKREC struct must have physical size info for W32 DiskAbsOperation().
// &? Currently, DiskGetPhysicalInfo() requires the thunking .DLL's -- this
//    must be fixed before ship.
//
//    Rev 1.8   01 Feb 1995 16:53:50   SZIADEH
// used the flag INIT_CERTLIB_USE_DEFAULT to used Quake functions.
// Otherwise, provide your own functions and pass 0 for the flag.
//
//    Rev 1.7   21 Dec 1994 20:13:10   DALLEE
// DOS, DX only - Added LOADDS to all callbacks.
//
//    Rev 1.6   20 Dec 1994 14:30:02   JMILLARD
// fix SYM_NLM problem
//
//    Rev 1.5   19 Dec 1994 17:55:44   DALLEE
// FileDelete() callback should return TRUE/FALSE, not NOERR/ERR as
// Quake FileDelete() does.
//
//    Rev 1.4   18 Dec 1994 15:29:44   SZIADEH
// added ctsn.h to the include list.
//
//    Rev 1.3   13 Dec 1994 19:07:14   JMILLARD
// ifndef SYM_NLM unneeded stuff
//
//    Rev 1.2   12 Dec 1994 20:42:54   SZIADEH
// added InitCertlib() to initialize the callback structure.
//
//    Rev 1.1   12 Dec 1994 20:37:36   SZIADEH
// Initial revision
//--------------------------------------------------------------------

#include "platform.h"
#include "file.h"
#include "xapi.h"
#include "disk.h"
#include "ctsn.h"
#include "callback.h"
#include "virscan.h"

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
  #include "winioctl.h"
  #include "LocalNT.h"
#endif


// DOS Certlib callbacks must have LOADDS

#ifdef SYM_DOS
  #define DOS_LOADDS    LOADDS
#else
  #define DOS_LOADDS
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// this overrides the forced convention there are no _ in front of names
// this is a Novell convention

#ifdef SYM_NLM
#define lpgCallBack _lpgCallBack
#endif

LPCALLBACKREV1 lpgCallBack = NULL;

#ifdef __cplusplus
}
#endif

#ifndef SYM_NLM

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

#define LOCK_DRIVE      0
#define UNLOCK_DRIVE    1

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskP ( // Locks / unlock physical drive
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

@Description:
This function is used to prepare a physical drive for disk read/write.

Note: this function does very little in SYM_DOS platform.

@Parameters:
$uOperation$ This parameter should contain 0 or 1.  If 0, the function will
             lock the drive, if 1, the function will unlock the drive.
$byDrive$ Should be an int-13 drive ID
$uRequestedAccess$ The possible settings can be:
        DISK_READ_WRITE If the code should lock/unlock write access
        DISK_READ_ONLY  If the code should lock/unlock read only access

@Returns:
    TRUE    if the operation was successful
    FALSE   if somebody has a lock on the drive for the requested access, or
            error.

@Include: virscan.h

*******************************************************************************/
#if defined(SYM_DOS)

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    return (TRUE);
}

#elif defined(SYM_WIN32) && !defined(SYM_VXD)

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
        return(TRUE);

    switch ( uRequestedAccess )
        {
        case DISK_READ_ONLY:

            switch ( uOperation )
                {
                case 0:

                    if ( DiskLockPhysicalVolume ( byDrive, 1, 1 ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockPhysicalVolume ( byDrive, 2, 1 ) )
                        {
                        DiskUnlockPhysicalVolume ( byDrive );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockPhysicalVolume ( byDrive );
                    DiskUnlockPhysicalVolume ( byDrive );

                    break;

                default:
                    return(FALSE);
                }

            break;

        case DISK_READ_WRITE:

            switch ( uOperation )
                {
                case 0:

                    if ( !VirusScanLockUnlockDiskP ( 0, byDrive, DISK_READ_ONLY ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockPhysicalVolume ( byDrive, 3, 1 ) )
                        {
                        VirusScanLockUnlockDiskP ( 1, byDrive, DISK_READ_ONLY );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockPhysicalVolume ( byDrive );

                    VirusScanLockUnlockDiskP ( 1, byDrive, DISK_READ_ONLY );

                    break;

                default:
                    return(FALSE);
                }

            break;

        default:
            return ( FALSE );
        }

    return ( TRUE );
}
#endif

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskL ( // Locks / unlock logical drive
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

@Description:
This function is used to prepare a logical drive for disk read/write.

Note: this function does very little in SYM_DOS platform.

@Parameters:
$uOperation$ This parameter should contain 0 or 1.  If 0, the function will
             lock the drive, if 1, the function will unlock the drive.
$byDrive$ Should be a drive ID - A, B, C, etc
$uRequestedAccess$ The possible settings can be:
        DISK_READ_WRITE If the code should lock/unlock write access
        DISK_READ_ONLY  If the code should lock/unlock read only access

@Returns:
    TRUE    if the operation was successful
    FALSE   if somebody has a lock on the drive for the requested access, or
            error.

@Include: virscan.h

*******************************************************************************/
#if defined(SYM_DOS)

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskL (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    return (TRUE);
}

#elif defined(SYM_WIN32) && !defined(SYM_VXD)

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskL (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
        return(TRUE);

    switch ( uRequestedAccess )
        {
        case DISK_READ_ONLY:

            switch ( uOperation )
                {
                case 0:

                    if ( DiskLockLogicalVolume ( byDrive, 1, 1 ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockLogicalVolume ( byDrive, 2, 1 ) )
                        {
                        DiskUnlockLogicalVolume ( byDrive );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockLogicalVolume ( byDrive );
                    DiskUnlockLogicalVolume ( byDrive );

                    break;

                default:
                    return(FALSE);
                }

            break;

        case DISK_READ_WRITE:

            switch ( uOperation )
                {
                case 0:

                    if ( !VirusScanLockUnlockDiskL ( 0, byDrive, DISK_READ_ONLY ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockLogicalVolume ( byDrive, 3, 1 ) )
                        {
                        VirusScanLockUnlockDiskL ( 1, byDrive, DISK_READ_ONLY );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockLogicalVolume ( byDrive );

                    VirusScanLockUnlockDiskL ( 1, byDrive, DISK_READ_ONLY );

                    break;

                default:
                    return(FALSE);
                }

            break;

        default:
            return ( FALSE );
        }

    return ( TRUE );
}
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
UINT SYM_EXPORT DOS_LOADDS WINAPI LocalDiskReadPhysical(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive )
{
    BYTE    byResult;
    BYTE    byCounter = 0;

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = bDrive;

#if defined(SYM_DOS) || defined(SYM_WIN32)
    byResult = DiskGetPhysicalInfo(&stDiskRec);
                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( 1 );
        }
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0))  // don't lock first partition table
        {
        if ( VirusScanLockUnlockDiskP ( LOCK_DRIVE,
                                        bDrive,
                                        DISK_READ_ONLY ) == FALSE)
            {
            return(1);                  // error!
            }
        }
#endif

#ifdef SYM_WIN32
    // Win32 platform needs special handling for Promise EIDE 4030+ controller.
    // Buffer must not straddle two 4K memory pages.
    // To keep things simple, we'll only allow reads of up to 4K.

    byResult = SYM_ERROR_INVALID_FUNCTION;  // assume failure.

    if (bCount * PHYSICAL_SECTOR_SIZE <= 4096)
        {
        auto    LPBYTE  lpLocalBuffer;

        lpLocalBuffer = (LPBYTE)MemAllocPtr(GHND, 4096 * 2);

        if (NULL != lpLocalBuffer)
            {
                                        // Get 4K aligned buffer.
            stDiskRec.buffer = (LPBYTE)((DWORD)lpLocalBuffer + 4096
                                        - ((DWORD)lpLocalBuffer % 4096));

            stDiskRec.dwHead        = bHead;
            stDiskRec.dwTrack       = wCylinder;
            stDiskRec.dwSector      = bSector;
            stDiskRec.numSectors    = bCount;

            do
                {
                byResult = DiskAbsOperation(READ_COMMAND, &stDiskRec);
                byCounter++;
                }
            while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

                                    // On success, copy these sectors over.
            if (byResult == SYM_ERROR_NO_ERROR)
                {
                MEMCPY(lpBuffer, stDiskRec.buffer, bCount * PHYSICAL_SECTOR_SIZE);
                }

            MemFreePtr(lpLocalBuffer);
            }
         }
#else
    stDiskRec.dwHead        = bHead;
    stDiskRec.dwTrack       = wCylinder;
    stDiskRec.dwSector      = bSector;
    stDiskRec.numSectors    = bCount;
    stDiskRec.buffer        = (BYTE huge *) lpBuffer;

    do
        {
        byResult = DiskAbsOperation(READ_COMMAND, &stDiskRec);
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0))
        {
        VirusScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                   bDrive,
                                   DISK_READ_ONLY );
        }
#endif

    return( (byResult == SYM_ERROR_NO_ERROR) ? 0 : 1 );
}

UINT SYM_EXPORT DOS_LOADDS WINAPI LocalDiskWritePhysical (
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive )
{

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

                                        // The following block fixes the
                                        // overwrites that were happening
                                        // on NT FAT partitions.
    if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT &&
         bDrive >= FIRST_HD )
        {
        BYTE    byDriveLetter;
        BYTE    byPartNum;
        DWORD   dwLinearOffset;
        DWORD   dwBytes;

                                        // Do these sectors lie within a
                                        // valid partition?
        byPartNum = LocalNTDeterminePartNum( bDrive,
                                             bHead,
                                             wCylinder,
                                             bSector,
                                             bCount,
                                             &dwLinearOffset );

                                        // Is this partition currently mapped
                                        // to a drive letter?
         if ( MAX_PARTITIONS != byPartNum &&
              0 != ( byDriveLetter = LocalNTDetermineDriveLetter( bDrive,
                                                                  byPartNum )))
            {
                dwBytes = bCount * SECTOR_SIZE;

                                        // Write to the logical drive
                return ( LocalNTDiskWriteLogical ( byDriveLetter,
                                                   dwLinearOffset,
                                                   lpBuffer,
                                                   &dwBytes ));
            }
        }
                                        // Otherwise, continue as before...
#endif  // SYM_WIN32 && !SYM_VXD && !SYM_NTK


    BYTE    byResult;
    BYTE    byCounter = 0;

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = bDrive;

#if defined(SYM_DOS) || defined(SYM_WIN32)
    byResult = DiskGetPhysicalInfo(&stDiskRec);

                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( 1 );
        }
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0 && bSector == 1))  // don't lock MBR
        {
        if ( VirusScanLockUnlockDiskP ( LOCK_DRIVE,
                                        bDrive,
                                        DISK_READ_WRITE ) == FALSE)
            {
            return(1);                  // error!
            }
        }
#endif

    stDiskRec.dwHead        = bHead;
    stDiskRec.dwTrack       = wCylinder;
    stDiskRec.dwSector      = bSector;
    stDiskRec.numSectors    = bCount;
    stDiskRec.buffer        = (BYTE huge *) lpBuffer;

    do
        {
        byResult = DiskAbsOperation(WRITE_COMMAND, &stDiskRec);
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0 && bSector == 1))  // don't lock MBR
        {
        VirusScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                   bDrive,
                                   DISK_READ_WRITE );
        }
#endif

    return( (byResult == SYM_ERROR_NO_ERROR) ? 0 : 1 );
}



#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
                                        // These functions are helpers for
                                        // LocalDiskWritePhysical under NT

//--------------------------------------------------------------------
// LocalNTDeterminePartNum()
//
// Given a series of physical sectors (drive number, starting coordinates,
// and a sector count) this function returns the partition number (0-3) in
// which the specified sectors reside.
//
// If these sectors lies outside of all partitions, stradle a partition
// boundary, or if an error is encountered while determining the partition
// number, MAX_PARTITIONS (4) is returned.
//--------------------------------------------------------------------
BYTE LocalNTDeterminePartNum
(
    BYTE    byDrive,
    BYTE    byHead,
    WORD    wCylinder,
    BYTE    bySector,
    BYTE    bySectors,
    LPDWORD lpdwLinearOffset
)
{
    BYTE    byResult;
    BYTE    byCounter = 0;
    DWORD   dwLinearOffset;
    WORD    wMBROffset;
    BYTE    byPartNum;
    BYTE    abyMBR [ SECTOR_SIZE ];

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = byDrive;

    byResult = DiskGetPhysicalInfo(&stDiskRec);

    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( MAX_PARTITIONS );
        }

    dwLinearOffset = (((DWORD) wCylinder *
                       stDiskRec.dwTotalHeads *
                       stDiskRec.dwSectorsPerTrack ) +
                      ((DWORD) byHead * stDiskRec.dwSectorsPerTrack ) +
                      (bySector - 1));

    // Read MBR for this physical disk

    stDiskRec.dwHead          = 0;
    stDiskRec.dwTrack         = 0;
    stDiskRec.dwSector        = 1;
    stDiskRec.numSectors    = 1;
    stDiskRec.buffer        = (BYTE huge *) abyMBR;

    do
        {
        byResult = DiskAbsOperation( READ_COMMAND, &stDiskRec );
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

    if ( byResult != SYM_ERROR_NO_ERROR )
        return ( MAX_PARTITIONS );

    for ( byPartNum = 0, wMBROffset = 0x1BE;
          byPartNum < MAX_PARTITIONS;
          ++byPartNum, wMBROffset += 0x10 )
        {
        if ( 0 != *((LPBYTE) abyMBR + wMBROffset + 4 ) &&
             dwLinearOffset >= *((LPDWORD) ( abyMBR + wMBROffset + 8 )) &&
             ( dwLinearOffset + bySectors ) <= *((LPDWORD) ( abyMBR + wMBROffset + 8 ))
                               + *((LPDWORD) ( abyMBR + wMBROffset + 12 )))
            {
            // byPartNum now indicates the first partition entry with a
            // valid non-zero filesystem byte which contains (inclusively)
            // the sector to be written.

            break;
            }
        }

    if ( NULL != lpdwLinearOffset )
        {
        dwLinearOffset -= *((LPDWORD) ( abyMBR + wMBROffset + 8 ));
        dwLinearOffset *= SECTOR_SIZE;
        *lpdwLinearOffset = dwLinearOffset;
        }

    return ( byPartNum );
}



//--------------------------------------------------------------------
// LocalNTDetermineDriveLetter()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE LocalNTDetermineDriveLetter
(
    BYTE    byDrive,
    BYTE    byPartNum
)
{
    BYTE   byResult = 0;
    BYTE   abyDevices[ SYM_MAX_PATH * 2 ];
    BYTE   abyDIOCBuffer[ SYM_MAX_PATH * 2 ];
    BOOL   bDriveLetterFound = FALSE;
    TCHAR  tszThisDevice[] = _T( "\\Device\\HarddiskX\\PartitionX" );
    LPTSTR lptstrDevice;

    // create the string we're trying to match
    tszThisDevice[16] = '0' + ( byDrive - FIRST_HD );
    tszThisDevice[27] = '0' + byPartNum + 1;

    // get a list of devices
    if ( 0 == QueryDosDevice( NULL, (char *) abyDevices, SYM_MAX_PATH * 2 ))
        {
        return( 0 );
        }

    lptstrDevice = (LPTSTR) abyDevices;

    while( lptstrDevice[0] != 0 )
        {
        if( lptstrDevice[1] == ':' &&
            0 != QueryDosDevice( lptstrDevice, (char *) abyDIOCBuffer, SYM_MAX_PATH * 2 ) &&
            !STRICMP( (char *) abyDIOCBuffer, tszThisDevice ))
            {
            bDriveLetterFound = TRUE;
            break;
            }
        lptstrDevice += STRLEN( lptstrDevice ) + 1;
        }

    if( TRUE == bDriveLetterFound )
        {
        byResult =(BYTE) lptstrDevice[0];
        }

    return( byResult );
}


//--------------------------------------------------------------------
// LocalNTDiskWriteLogical()
//
// Given a drive letter, a linear byte offset, some data, and a length,
// this function writes length bytes of the data to the logical drive
// starting at the linear byte offset.  The return values are identical
// to LocalDiskWritePhysical():
//
// 0 == success
// 1 == failure
//--------------------------------------------------------------------
BYTE LocalNTDiskWriteLogical
(
    BYTE    byDriveLetter,
    DWORD   dwLinearOffset,
    LPVOID  lpvBuffer,
    LPDWORD lpdwSectors
)
{
    TCHAR  tszThisLogicalDrive[] = _T( "\\\\.\\X:" );
    HANDLE hLogicalDrive;
    BOOL   bLocked;
    BOOL   bDismounted;
    BOOL   bWritten;
    DWORD  dwDummy;

    tszThisLogicalDrive[4] =(TCHAR) byDriveLetter;

    hLogicalDrive = CreateFile( tszThisLogicalDrive,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL );

    if( INVALID_HANDLE_VALUE == hLogicalDrive )
        {
        return( 1 );
        }

    dwDummy = SetFilePointer( hLogicalDrive, dwLinearOffset, NULL, FILE_BEGIN );

    if( dwDummy != dwLinearOffset )
        {
        CloseHandle( hLogicalDrive );
        return( 1 );
        }

    bLocked = DeviceIoControl ( hLogicalDrive,
                                FSCTL_LOCK_VOLUME,
                                NULL,
                                0,
                                NULL,
                                0,
                                &dwDummy,
                                NULL );

    bWritten = WriteFile( hLogicalDrive, lpvBuffer, *lpdwSectors, lpdwSectors, NULL );

    if( bLocked )
        {
        bDismounted = DeviceIoControl ( hLogicalDrive,
                                        FSCTL_DISMOUNT_VOLUME,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        &dwDummy,
                                        NULL );

        bLocked = !DeviceIoControl ( hLogicalDrive,
                                     FSCTL_UNLOCK_VOLUME,
                                     NULL,
                                     0,
                                     NULL,
                                     0,
                                     &dwDummy,
                                     NULL );
        }

    CloseHandle( hLogicalDrive );

    if ( !bWritten )
        {
        return( 1 );
        }

    return( 0 );
}

#endif // SYM_WIN32 && !SYM_VXD && !SYM_NTK



//--------------------------------------------------------------------
//--------------------------------------------------------------------
HFILE SYM_EXPORT DOS_LOADDS WINAPI LocalFileOpen (
    LPTSTR  lpName,
    UINT    uOpenMode )
{
    SYM_ASSERT ( lpName );

    return ( FileOpen ( lpName, uOpenMode ) );
}

HFILE SYM_EXPORT DOS_LOADDS WINAPI LocalFileClose (
    HFILE   hHandle )
{
    SYM_ASSERT ( hHandle );

    return ( FileClose ( hHandle ) );
}

UINT SYM_EXPORT DOS_LOADDS WINAPI LocalFileRead (
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes )
{
    SYM_ASSERT ( lpBuffer );
    SYM_ASSERT ( uBytes );
    SYM_ASSERT ( hHandle );

    return ( FileRead ( hHandle, lpBuffer, uBytes ) );
}

UINT SYM_EXPORT DOS_LOADDS WINAPI LocalFileWrite (
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes )
{
    SYM_ASSERT ( lpBuffer || (0 == uBytes) );
    SYM_ASSERT ( hHandle );

    return ( FileWrite ( hHandle, lpBuffer, uBytes ) );
}

DWORD SYM_EXPORT DOS_LOADDS WINAPI LocalFileSeek (
    HFILE   hHandle,
    LONG    lOffset,
    int     nFlag )
{
    SYM_ASSERT ( hHandle );

    return ( FileSeek ( hHandle, lOffset, nFlag ) );
}

DWORD SYM_EXPORT DOS_LOADDS WINAPI LocalFileSize (
    HFILE   hHandle )
{
    return ( FileLength ( hHandle ) );
}

BOOL SYM_EXPORT DOS_LOADDS WINAPI LocalFileGetDateTime (
    HFILE   hHandle,
    UINT    uType,
    LPWORD  lpwDate,
    LPWORD  lpwTime )
{
    auto    WORD        wDosDate = 0;
    auto    WORD        wDosTime = 0;
    auto    FILETIME    FileTime = { 0 };

    (void) uType;

    SYM_ASSERT ( hHandle );
    SYM_ASSERT ( lpwDate );
    SYM_ASSERT ( lpwTime );

    FileGetTime ( hHandle, TIME_LASTWRITE, &FileTime );
    FileTimeToDosTime( &FileTime, &wDosDate, &wDosTime );

    *lpwDate = wDosDate;
    *lpwTime = wDosTime;

    return ( TRUE );
}

BOOL SYM_EXPORT DOS_LOADDS WINAPI LocalFileSetDateTime (
    HFILE   hHandle,
    UINT    uType,
    WORD    wDate,
    WORD    wTime )
{
    auto    FILETIME    FileTime = { 0 };

    (void) uType;

    SYM_ASSERT ( hHandle );

    DosTimeToFileTime ( wDate, wTime, &FileTime );
    FileSetTime ( hHandle, TIME_LASTWRITE, &FileTime );
    return ( TRUE );
}

BOOL SYM_EXPORT DOS_LOADDS WINAPI LocalFileDelete (
    LPTSTR  lpFilename )
{
    SYM_ASSERT ( lpFilename );

    return ( FileDelete(lpFilename) == NOERR ? TRUE : FALSE );
}

UINT SYM_EXPORT DOS_LOADDS WINAPI LocalFileGetAttr (
    LPTSTR  lpFilename,
    LPUINT  lpuAttr )
{
    SYM_ASSERT ( lpFilename );

    return ( FileGetAttr ( lpFilename, lpuAttr ) );
}

UINT SYM_EXPORT DOS_LOADDS WINAPI LocalFileSetAttr (
    LPTSTR  lpFilename,
    UINT    uAttr )
{
    SYM_ASSERT ( lpFilename );

    return ( FileSetAttr ( lpFilename, uAttr ) );
}

HFILE SYM_EXPORT DOS_LOADDS WINAPI LocalFileCreate (
    LPTSTR  lpFilename,
    UINT    uAttr )
{
    SYM_ASSERT ( lpFilename );

    return ( FileCreate ( lpFilename, uAttr ) );
}

LPVOID SYM_EXPORT DOS_LOADDS WINAPI LocalTempMemoryAlloc (
    DWORD    dwSize )
{
    return ( MemAllocPtr ( GHND, dwSize ) );
}

VOID SYM_EXPORT DOS_LOADDS WINAPI LocalTempMemoryFree (
    LPVOID  lpBuffer )
{
    MemFreePtr ( lpBuffer );
}

LPVOID SYM_EXPORT DOS_LOADDS WINAPI LocalPermMemoryAlloc (
    DWORD   dwSize )
{
    return ( MemAllocPtr ( GHND, dwSize ) );
}

VOID SYM_EXPORT DOS_LOADDS WINAPI LocalPermMemoryFree (
    LPVOID  lpBuffer )
{
    MemFreePtr ( lpBuffer );
}



////////////////////////////////////////////////////////////////////////////
//
// InitCertlib()
//
// 1. Initializes the global callback table, lpgCallBack.
//
// 2. Initializes the passed callback buffer, lpCallBackTable.
//
// 3. Presently always returns TRUE.
//
// Major Fix: 03/25/96  MKEATIN
//
// NOTE: It looks like after the last change, you can no longer specify custom
// callback functions to CERTLIBN using this routine -- the LocalXXX functions
// above will always be employed.  -DALLEE
////////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT WINAPI InitCertlib (
    LPCALLBACKREV1 lpCallBackTable,
    WORD           wFlag )
{
//-------------------------------------------------------------------------
//
//   The global pointer lpgCallBack should not be changed, nor should it
//   dissappear.  So, lpgCallback will now always point to the static
//   buffer CallBackTable declared below.
//
//-------------------------------------------------------------------------

    static CALLBACKREV1 CallBackTable;


//-------------------------------------------------------------------------
//
//   lpgCallBack only needs to be initialize once only at the start of a
//   program.  So, the flag bGlobalTableHasBeenInialized will enforce this
//   behavior. The Code below initializes CallBackTable and sets lpgCallback
//   to point to it - until the program exits.
//
//-------------------------------------------------------------------------

    static BOOL bGlobalTableHasBeenInitialized = FALSE;

    if ( bGlobalTableHasBeenInitialized == FALSE )
       {
       CallBackTable.FileOpen           = LocalFileOpen;
       CallBackTable.FileClose          = LocalFileClose;
       CallBackTable.FileRead           = LocalFileRead;
       CallBackTable.FileWrite          = LocalFileWrite;
       CallBackTable.FileSeek           = LocalFileSeek;
       CallBackTable.FileSize           = LocalFileSize;
       CallBackTable.FileGetDateTime    = LocalFileGetDateTime;
       CallBackTable.FileSetDateTime    = LocalFileSetDateTime;
       CallBackTable.FileDelete         = LocalFileDelete;
       CallBackTable.FileGetAttr        = LocalFileGetAttr;
       CallBackTable.FileSetAttr        = LocalFileSetAttr;
       CallBackTable.FileCreate         = LocalFileCreate;
       CallBackTable.SDiskReadPhysical  = LocalDiskReadPhysical;
       CallBackTable.SDiskWritePhysical = LocalDiskWritePhysical;
       CallBackTable.TempMemoryAlloc    = LocalTempMemoryAlloc;
       CallBackTable.TempMemoryFree     = LocalTempMemoryFree;
       CallBackTable.PermMemoryAlloc    = LocalPermMemoryAlloc;
       CallBackTable.PermMemoryFree     = LocalPermMemoryFree;

       lpgCallBack                      = &CallBackTable;
       bGlobalTableHasBeenInitialized   = TRUE;
       }


//-------------------------------------------------------------------------
//
//   The code below initializes the buffer passed - lpCallBackTable
//   (which is used in NavEx for example)  This should probably be written
//   as another function, but is kept here to accomodate current calls to
//   this function.
//
//-------------------------------------------------------------------------

    if ( wFlag & INIT_CERTLIB_USE_DEFAULT )
       {
       lpCallBackTable->FileOpen           = LocalFileOpen;
       lpCallBackTable->FileClose          = LocalFileClose;
       lpCallBackTable->FileRead           = LocalFileRead;
       lpCallBackTable->FileWrite          = LocalFileWrite;
       lpCallBackTable->FileSeek           = LocalFileSeek;
       lpCallBackTable->FileSize           = LocalFileSize;
       lpCallBackTable->FileGetDateTime    = LocalFileGetDateTime;
       lpCallBackTable->FileSetDateTime    = LocalFileSetDateTime;
       lpCallBackTable->FileDelete         = LocalFileDelete;
       lpCallBackTable->FileGetAttr        = LocalFileGetAttr;
       lpCallBackTable->FileSetAttr        = LocalFileSetAttr;
       lpCallBackTable->FileCreate         = LocalFileCreate;
       lpCallBackTable->SDiskReadPhysical  = LocalDiskReadPhysical;
       lpCallBackTable->SDiskWritePhysical = LocalDiskWritePhysical;
       lpCallBackTable->TempMemoryAlloc    = LocalTempMemoryAlloc;
       lpCallBackTable->TempMemoryFree     = LocalTempMemoryFree;
       lpCallBackTable->PermMemoryAlloc    = LocalPermMemoryAlloc;
       lpCallBackTable->PermMemoryFree     = LocalPermMemoryFree;
       }

    return ( TRUE );
}

#endif      // ifndef SYM_NLM

// eof
