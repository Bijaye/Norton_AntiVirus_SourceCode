/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/dsk_accs.c_v   1.14   02 Feb 1998 19:59:10   Basil  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/dsk_accs.c_v  $ *
// 
//    Rev 1.14   02 Feb 1998 19:59:10   Basil
// If the full path to the symkrnl vxd is specified, bypass the logic that
// determines the path based on current module handle.  This fixes the problem
// of loading the symkrnl.vxd via symkrlib (SDL)
// 
//    Rev 1.13   13 Nov 1997 22:27:42   MARKK
// Test level lock to make sure it succeeded.
// 
//    Rev 1.12   18 Sep 1997 22:24:16   ASANJAB
// Fixed problem with loading symkrnl.vxd in high ASCII path
// 
//    Rev 1.11   13 Aug 1997 13:59:34   bsobel
// Fixed case where missing vxd would cause crash
// 
//    Rev 1.10   10 Aug 1997 20:39:46   bsobel
// Changed default behaviour to load symkrnl.vxd from the same directory as symkrnl.dll.
// If this fails the default searching behaviour takes over.  This was done to fix a problem
// with using Symkrnl is explorer extensions who's path info didn't include the directory
// holding the vxd.
// 
//    Rev 1.9   28 Mar 1997 16:58:44   BILL
// Made two more functions stubs for Alpha (DiskVCacheFlush and Invalidate)
// 
//    Rev 1.8   19 Mar 1997 21:41:08   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.7   09 Oct 1996 15:20:54   MARKK
// Don't fail a level 2 lock of the patch can't be applied.
// 
//    Rev 1.6   24 Sep 1996 14:14:12   MARKK
// Moved over old tomahawk fix to patch Win95 build 950
// 
//    Rev 1.5   29 Aug 1996 18:05:22   DHERTEL
// Merged changed from Quake E:
// CreateFile() should use OPEN_EXISTING instead of NULL
// 
//    Rev 1.4   29 Jul 1996 11:44:42   Basil
// Modified SymkrnlVxDSetName() to convert given name to shortname, since
// the VxD routines will not work with long filenames.
// 
//    Rev 1.3   02 Apr 1996 15:56:28   MARKK
// Fixed DiskMuckWithSwapFile
// 
//    Rev 1.2   02 Apr 1996 10:01:32   MARKK
// Added DiskMuckWithSwapfile
// 
//    Rev 1.1   27 Feb 1996 18:10:22   Basil
// Added SymkrnlVxDGet/SetName functions so SDL can change the VxD name.
// 
//    Rev 1.0   26 Jan 1996 20:21:58   JREARDON
// Initial revision.
// 
//    Rev 1.41   13 Dec 1995 14:06:52   DBUCHES
// 32 Bit FAT work in progress.
//
//    Rev 1.40   23 Oct 1995 18:10:52   PLINHARDT
//
// Changes to function IOCtlVWin32:
// 1) The function is not supported under NT, it has been changed to
//    return an INVALID FUNCTION error under NT.
// 2) If the function fails to obtain the VWIN32 handle, it used to
//    returned an error the first time and then appear to succeed on
//    subsequent calls.
//    This behavior is fixed to return an error each time.
// 3) Most callers of this functions don't care about the return status
//    The function was changed to return carry and status in EAX on any
//    error that occurs.
//
//    Rev 1.39   22 May 1995 11:29:40   SCOTTP
// now use qmodule_symkrnl_vxd to specify the vxd
// name
//
//    Rev 1.38   25 Apr 1995 17:11:40   BRAD
// Must pass a variable for the return size for NT
//
//    Rev 1.37   19 Apr 1995 20:10:34   DAVID
// Cleaned up the management of the Symkrnl VxD
//
//    Rev 1.36   10 Apr 1995 13:27:40   BRUCE
// Added FileIsOpen2()
//
//    Rev 1.35   07 Mar 1995 22:26:14   DAVID
// Added DiskGetCurrentLockState()
//
//    Rev 1.34   27 Feb 1995 14:26:52   HENRI
// Only init the symrknl vxd once
//
//    Rev 1.33   09 Feb 1995 21:20:44   MARKK
// Added params to DiskEnumerateMemoryFiles
//
//    Rev 1.32   02 Feb 1995 17:56:24   BRAD
// Added SYMKRNL VxD changes from QAK6
//
//    Rev 1.31   18 Jan 1995 18:18:50   DBUCHES
// Fixed DiskLockLogical/PhysicalVolume.  Needed to check carry and return
// AX register if set.
//
//    Rev 1.30   11 Jan 1995 16:47:18   SKURTZ
// DX only: Locking/unlocking physical and logical volumes must return 0 if
// the carry flag is cleared. The regs.x.AX register only holds the error codes
// if the carry flag is set.
//
//    Rev 1.29   02 Nov 1994 14:03:32   MARKK
// It helps if you check the return registers
//
//    Rev 1.28   31 Oct 1994 17:05:00   MARKK
// Fixed syntax error
//
//    Rev 1.27   31 Oct 1994 16:43:48   MARKK
// Added DiskEnumerateMemoryFiles and DiskPollWrites
//
//    Rev 1.26   26 Oct 1994 20:36:12   BILL
// Added include needed for disk.h
//
//    Rev 1.25   25 Oct 1994 19:34:20   HENRI
// Now using the common IOCtlVWin32 for all WIN32 IOCTL functions
//
//    Rev 1.24   05 Oct 1994 11:30:26   MARKK
// Fixed problem with enumerating open files
//
//    Rev 1.23   08 Sep 1994 14:32:54   MARKK
// Fixed Win16 error
//
//    Rev 1.22   08 Sep 1994 11:33:52   MARKK
// Fake error return from enumerateopenfiles since we aren't catching the error
//
//    Rev 1.21   30 Aug 1994 14:32:06   BRIANF
// Fixed Geo Comments.
//
//    Rev 1.20   24 Aug 1994 15:00:22   DLEVITON
// Fixed problem making for WIN16.
//
//    Rev 1.19   24 Aug 1994 14:24:04   MARKK
// Added file enumeration and swap file functions
//
//    Rev 1.18   03 Aug 1994 11:39:54   MARKK
// Fixed error return on IOCTL
//
//    Rev 1.17   06 Jul 1994 13:49:38   DBUCHES
// Need to MEMSET regs for Win16/DX platforms
//
//    Rev 1.16   06 Jul 1994 12:31:44   DBUCHES
// Fixed win16 complie errors and added win32 code for DiskLock/UnlockLogicalVolume
//
//    Rev 1.15   05 Jul 1994 16:22:48   DBUCHES
// Same change for DiskLock/UnlockPhysicalVolume().
//
//    Rev 1.14   05 Jul 1994 15:12:10   DBUCHES
// Changed DiskLock/UnlockLogicalvolume() to use Int() calls instead of inline assembly.
//
//    Rev 1.13   30 Jun 1994 21:37:18   DBUCHES
// Fixed compile errors for DX platform
//
//    Rev 1.12   29 Jun 1994 20:00:04   DBUCHES
// Fixed compile errors
//
//    Rev 1.11   29 Jun 1994 18:45:30   HENRI
// Added Windows 4.0 Exclusive Lock Functions
//
//    Rev 1.10   04 May 1994 05:52:04   MARKK
// DOSX work
//
//    Rev 1.9   21 Mar 1994 00:20:24   BRAD
// Cleaned up for WIN32
//
//    Rev 1.8   15 Mar 1994 12:32:50   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.7   25 Feb 1994 15:03:10   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.5   02 Jul 1993 08:51:52   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.4   12 Feb 1993 04:27:38   ENRIQUE
// No change.
//
//    Rev 1.3   19 Oct 1992 17:01:48   BILL
// No change.
//
//    Rev 1.2   10 Sep 1992 19:05:26   BRUCE
// Deleted commented out includes
//
//    Rev 1.1   09 Sep 1992 18:18:52   BRUCE
// Eliminated dependency on nwinutil.h
//
//    Rev 1.0   27 Aug 1992 10:33:40   ED
// Initial revision.
 ************************************************************************/

/*----------------------------------------------------------------------*/
/*      This module manages the disk accessed flag in DOS's internal    */
/*      disk parameter block.  This flag can be reset so DOS will       */
/*      discard its buffered device information and re-read everything  */
/*      from scratch on the next disk access.  This must be done        */
/*      whenever disk structures like the FAT are directly modified     */
/*      by programs like Speed Disk.                                    */
/*                                                                      */
/*      Don't confuse these routines with the similarly named functions */
/*      that manage the DOS 4 IOCTL block device accessability flag.    */
/*                                                                      */
/*      BOOL DiskResetAccessFlag (BYTE dl);                             */
/*      BOOL DiskAccessFlagAddr (BYTE dl, LPBYTE FAR* lplpFlag);        */
/*      void DiskFreeAccessFlagAddr (LPBYTE FAR* lplpFlag);             */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*@Library: SYMKRNL */

#include <dos.h>
#include "platform.h"
#include "quakever.h"
#define INCLUDE_VWIN32
#include "disk.h"
#include "xapi.h"
#include "file.h"
#ifdef SYM_WIN32
#include "symkvxd.h"
#include "vxdload.h"
#endif

#if !defined(SYM_WIN32)

/*----------------------------------------------------------------------*
 * Reset the internal Disk Paramter Block disk accessed flag to         *
 * "never accessed" for the drive letter 'dl'.                          *
 *                                                                      *
 * Note the the location of the flag changed in DOS 4.x.                *
 * The flag is either 0 (DISK_ACCESSED(or -1 (DISK_NOT_ACCESSED).       *
 *                                                                      *
 *  Returns:                                                            *
 *      TRUE if okay, else FALSE                                        *
 *----------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI DiskResetAccessFlag (BYTE dl)
{
    register    UINT            offset;
    auto        ShortDiskRec    disk;
    auto        BOOL            ok;
    auto        LPBYTE          drivePtr;
    auto        DWORD           startOfRootDir;
    auto        DWORD           sectorsPerFat;
    auto        BYTE            oldAccessed;
#ifdef  SYM_PROTMODE
    auto        UINT            wSelector;
#endif

    dl = (BYTE) CharToUpper(dl);

    ok = DiskGetDriverTablePtr (dl, (ShortDiskRec FAR * FAR *) &drivePtr);

    if (!ok)                            /* If error, return FALSE       */
        return(FALSE);

#ifdef  SYM_PROTMODE
                                        /* Get a selector               */
    if (SelectorAlloc(FP_SEG(drivePtr), 0xFFFF, &wSelector))
        return(FALSE);

                                        /* Change pointer so uses selector */
    FP_SEG(drivePtr) = wSelector;
#endif

                                /* Move into local buffer       */
    disk = *((ShortDiskRec FAR *) drivePtr);

                                        /* Is this a DOS 4.0 type?      */
                                        /* If yes, the offset is 0x18,  */
                                        /*    else the offset is 0x17   */
    offset = (DiskIsBigTable ((DISKREC FAR *) &disk, &startOfRootDir,
                        &sectorsPerFat, &ok)) ? 0x18 : 0x17;

                                        /* Set Access Flag to NEVER ACCESSED */
    oldAccessed = *(drivePtr + offset); /* Get old value                     */
    if (oldAccessed == DISK_ACCESSED)   /* Safety measure: Make sure at 0    */
        *(drivePtr + offset) = DISK_NOT_ACCESSED;

#ifdef  SYM_PROTMODE
    SelectorFree(wSelector);            /* Free the selector            */
#endif

    return (ok);
}
#endif


#if !defined(SYM_WIN32)
/*----------------------------------------------------------------------*
 * Returns the address of the internal Disk Paramter Block disk         *
 * accessed flag for the drive letter 'dl'.                             *
 *                                                                      *
 * Note the the location of the flag changed in DOS 4.x.                *
 * The flag is either 0 (DISK_ACCESSED(or -1 (DISK_NOT_ACCESSED).       *
 *                                                                      *
 * The very act of getting the address of the access flag changes the   *
 * value of the flag to DISK_ACCESSED.                                  *
 *                                                                      *
 * When the flag address is no longer needed, DiskFreeAccessFlagAddr()  *
 * must be called.                                                      *
 *                                                                      *
 *  Returns:                                                            *
 *      TRUE if okay, else FALSE                                        *
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskAccessFlagAddr (BYTE dl, LPBYTE FAR* lplpFlag)
{
    register    UINT            offset;
    auto        ShortDiskRec    disk;
    auto        BOOL            ok;
    auto        LPBYTE          drivePtr;
    auto        DWORD           startOfRootDir;
    auto        DWORD           sectorsPerFat;
#ifdef  SYM_PROTMODE
    auto        UINT            wSelector;
#endif

    dl = (BYTE) CharToUpper(dl);
    *lplpFlag = NULL;

    ok = DiskGetDriverTablePtr (dl, (ShortDiskRec FAR * FAR *) &drivePtr);

    if (!ok)                            /* If error, return FALSE       */
        return(FALSE);

#ifdef  SYM_PROTMODE
                                        /* Get a selector               */
    if (SelectorAlloc(FP_SEG(drivePtr), 0xFFFF, &wSelector))
        return(FALSE);

                                        /* Change pointer so uses selector */
    FP_SEG(drivePtr) = wSelector;
#endif

                                        /* Move into local buffer       */
    disk = *((ShortDiskRec FAR *) drivePtr);

                                        /* Is this a DOS 4.0 type?      */
                                        /* If yes, the offset is 0x18,  */
                                        /*    else the offset is 0x17   */
    offset = (DiskIsBigTable ((DISKREC FAR *) &disk, &startOfRootDir,
                        &sectorsPerFat, &ok)) ? 0x18 : 0x17;

    *lplpFlag = drivePtr + offset;      /* Get the flag address         */

    return (ok);                        /* Selector remains allocated!  */
}
#endif

#if !defined(SYM_WIN32)
/*----------------------------------------------------------------------*
 * Frees the selector (if any(created by DiskAccessFlagAddr().          *
 *----------------------------------------------------------------------*/

void SYM_EXPORT WINAPI DiskFreeAccessFlagAddr (LPBYTE FAR* lplpFlag)
{
    if (*lplpFlag != NULL)
        {
#ifdef  SYM_PROTMODE
        LPBYTE  lpFlag = *lplpFlag;

        SelectorFree(FP_SEG(lpFlag));
#endif
        *lplpFlag = NULL;
        }
}
#endif  // !defined(SYM_WIN32)




/*@API:**********************************************************************
@Declaration: UINT DiskLockLogicalVolume(BYTE dl, BYTE byLockLevel,
BYTE byPermissions)

@Description:
Locks the logical volume. Locking a volume allows the lock owner to control
access to the files on the volume.

@Parameters:
$dl$ Drive Letter to lock.

$byLockLevel$
Specifies the level of the lock. Must be one of these values:
0   Excludes everybody but lock owner. No open files allowed.
1   Allows open files. Reads and writes based on permissions.
2   Must first obtain a level 1 lock. Allows reads, does not permit writes.
3   Must first obtain a level 2 lock. Does not permit reads and writes.

$byPermissions$
Specifies the read and write permissions for a level 1 lock.
Must be one of these values:

0   Allows reads, does not permit writes.
1   Allows reads and writes.

The level 0 lock stands by itself, while the level 1, 2, and 3
locks form a hierarchy.  For example, if the level 1 lock does
not permit writes, then the level 2 lock has no effect except
that it must be taken in order to proceed to a level 3 lock.
In effect, the level 2 and 3 locks constitute critical sections
on a level 1 lock.

@Returns:
0 for no errors, otherwise the error code is returned.
(The return value is the value of the AX register after the IOCTL call)

@See: DiskUnlockLogicalVolume, DiskLockPhysicalVolume, DiskUnlockPhysicalVolume

@Include: disk.h

@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskLockLogicalVolume(BYTE dl, BYTE byLockLevel, BYTE byPermissions)
{
    auto UINT  uError = 0;
    
#ifdef _M_ALPHA // WES Don't support even the setup of this function under Alpha
	SYM_ASSERT(FALSE);
	uError = 1;

#elif defined(SYM_WIN32)
    extern  HANDLE          hSymkrnlVxD;
    static  BOOL            bVWin32Patched = FALSE;
    auto    BYTE            byDriveNum;       // 0=default, 1=A, B=2, ...
    auto    DIOC_REGISTERS  regsIn;
    auto    DIOC_REGISTERS  regsOut;

    //
    //  Before allowing the app to enter a level 2 lock, make sure VWin32.VxD
    //  will not cause a deadlock.  If necessary, SYMKVxD will patch and fix
    //  VWin32.  If we cannot insure a deadlock, we prevent our apps from
    //  entering a level 2 lock by failing this call.
    //

    if (!bVWin32Patched  && byLockLevel==2) // ask VxD to perform VWin32 patching
    {
        if (!hSymkrnlVxD)       // make sure VxD is loaded
            VxDServicesInit();

        if (!hSymkrnlVxD)
            return(1);          // return non-zero if SYMKVxD is not loaded

        DeviceIoControl(hSymkrnlVxD,                // SYMKVxD Handle
                           SYMKVxD_FN_PATCHVWIN32,  // Function 6: SYMKVxD_FN_PATCHVWIN32
                           (LPVOID)&bVWin32Patched, // input buffer
                           sizeof(bVWin32Patched),  // input buffer size
                           NULL,                    // no output buffer
                           0,                       // no output buffer size
                           NULL, NULL);             // sync call

        bVWin32Patched = TRUE;
    }

    byDriveNum = dl - 'A' + 1;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byLockLevel << 8;
    regsIn.reg_EBX |= byDriveNum;
    regsIn.reg_ECX = 0x0000084A;
    regsIn.reg_EDX = byPermissions;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
                                        // If carry, save off error
    if ( regsOut.reg_Flags & 0x00000001 )
        {
        uError = regsOut.reg_EAX;
        if (uError == 0)
            uError = ERR;
        }
    else
        {
        WORD wLock = 0;
        
        DiskGetCurrentLockState(byDriveNum, &wLock, NULL);
        if (wLock != (WORD) byLockLevel)
            uError = ERR;
        }

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    auto BYTE byDriveNum;       // 0=default, 1=A, B=2, ...
    union REGS regs;
    struct SREGS sregs;

    byDriveNum = dl - 'A' + 1;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;         // generic IOCTL
    regs.h.bh = byLockLevel;    // level of the lock
    regs.h.bl = byDriveNum;     // drive to lock
    regs.h.ch = 0x08;           // device category
    regs.h.cl = 0x4A;           // Lock Logical Volume
    regs.h.dl = byPermissions;  // permission flags

    Int(0x21,&regs,&sregs);     // Int 21h

    if (regs.x.cflag)
        {
        uError = regs.x.ax;
        if (uError == 0)
            uError = ERR;
        }
    else
        {
        WORD wLock = 0;
        
        DiskGetCurrentLockState(byDriveNum, &wLock, NULL);
        if (wLock != (WORD) byLockLevel)
            uError = ERR;
        }


#endif

    return(uError);
}


/*@API:**********************************************************************
@Declaration: UINT DiskLockPhysicalVolume(BYTE byDriveNum, BYTE byLockLevel,
BYTE byPermissions)

@Description:
Locks the physical volume. Locking a volume allows the lock owner
to control access to the files on the volume. A single physical
volume may be divided into more than one logical volume, also called
a partition.

@Parameters:
$byDriveNum$

Drive to lock. Must be one of these values:

00 - 7Fh        Floppy disk
80 - FFh        Fixed disk

$byLockLevel$

Specifies the level of the lock. Must be one of these values:

0   Excludes everybody but lock owner. No open files allowed.
1   Allows open files. Reads and writes based on permissions.
2   Must first obtain a level 1 lock. Allows reads, does not permit writes.
3   Must first obtain a level 2 lock. Does not permit reads and writes.

$byPermissions$

Specifies the read and write permissions for a level 1 lock. Must be one of these values:

0   Allows reads, does not permit writes.
1   Allows reads and writes.
The level 0 lock stands by itself, while the level 1, 2, and 3 locks
form a hierarchy. For example, if the level 1 lock does not permit
writes, then the level 2 lock has no effect except that it must be
taken in order to proceed to a level 3 lock. In effect, the level 2
and 3 locks constitute critical sections on a level 1 lock.

@Returns:
0 for no errors, otherwise the error code is returned.
(The return value is the value of the AX register after the IOCTL call)

@See: DiskUnlockPhysicalVolume, DiskUnlockLogicalVolume, DiskLockLogicalVolume

@Include: disk.h

@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskLockPhysicalVolume(BYTE byDriveNum, BYTE byLockLevel, BYTE byPermissions)
{
    auto UINT  uError = 0;

#ifdef _M_ALPHA
	uError = 1;
#elif defined (SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byLockLevel << 8;
    regsIn.reg_EBX |= byDriveNum;
    regsIn.reg_ECX = 0x0000084B;
    regsIn.reg_EDX = byPermissions;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
                                                           // If carry, save off error
    if ( regsOut.reg_Flags & 0x00000001 )
        uError = regsOut.reg_EAX;

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;         // generic IOCTL
    regs.h.bh = byLockLevel;    // level of the lock
    regs.h.bl = byDriveNum;     // drive to lock
    regs.h.ch = 0x08;           // device category
    regs.h.cl = 0x4B;           // Lock Physical Volume
    regs.h.dl = byPermissions;  // permission flags

    Int(0x21,&regs,&sregs);     // Int 21h

    if (regs.x.cflag)
        uError = regs.x.ax;
    else
        uError = 0;

#endif

    return(uError);
}




/*@API:**********************************************************************
@Declaration: UINT DiskUnlockLogicalVolume(BYTE dl)

@Description:
Unlocks the logical volume according to the lock hierarchy.
Only the lock owner can release the lock on a volume.

@Parameters:
$dl$ Drive Letter to lock.

Calling Unlock Logical Volume on a level 0 lock or on a level 1 lock
completely releases the lock on the volume. Level 2 and 3 locks are
released in reverse order. For example, calling Unlock Logical Volume
on a level 3 lock causes it to first become a level 2 lock, then a level
1 lock, and then the lock on the volume is completely released.
When a level 3 lock (which does not permit reads or writes) is released
and becomes a level 2 lock, all queued reads are started up.
When the level 2 lock (which allows reads and does not permit writes)
is released, all queued writes are started up if the permissions on the
level 1 lock allows writes. Otherwise, the queued writes are started when
the level 1 lock is released.

@Returns:
0 for no errors, otherwise the error code is returned.
(The return value is the value of the AX register after the IOCTL call)

@See: DiskLockLogicalVolume, DiskLockPhysicalVolume, DiskLockPhysicallVolume

@Include: disk.h

@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskUnlockLogicalVolume(BYTE dl)
{
    auto UINT  uError = 0;


#ifdef _M_ALPHA
	uError = 1;
#elif defined (SYM_WIN32)
    auto BYTE byDriveNum;       // 0=default, 1=A, B=2, ...
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    byDriveNum = dl - 'A' + 1;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byDriveNum;
    regsIn.reg_ECX = 0x0000086A;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
                                                           // If carry, save off error
    if ( regsOut.reg_Flags & 0x00000001 )
        uError = regsOut.reg_EAX;

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    auto BYTE byDriveNum;       // 0=default, 1=A, B=2, ...
    union REGS regs;
    struct SREGS sregs;

    byDriveNum = dl - 'A' + 1;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;         // generic IOCTL
    regs.h.bl = byDriveNum;     // drive to lock
    regs.h.ch = 0x08;           // device category
    regs.h.cl = 0x6A;           // UnLock Logical Volume

    Int(0x21,&regs,&sregs);     // Int 21h

    if (regs.x.cflag)
        uError = regs.x.ax;
    else
        uError = 0;


#endif

    return(uError);

}

/*@API:**********************************************************************
@Declaration: UINT SYM_EXPORT WINAPI DiskUnlockPhysicalVolume(BYTE byDriveNum)

@Description:
Unlocks the physical volume according to the lock hierarchy.
Only the lock owner can release the lock on a volume.


@Parameters:
$byDriveNum$

Drive to unlock. Must be one of these values:

00 - 7Fh        Floppy disk
80 - FFh        Fixed disk

NOTE: The Win32 SDK (MSDOS.HLP) says that byDriveNum "Can be 0 for default
drive, 1 for A, 2 for B, and so on."

Calling Unlock Physical Volume on a level 0 lock or on a level 1
lock completely releases the lock on the volume. Level 2 and 3 locks
are released in reverse order. For example, calling Unlock Physical
Volume on a level 3 lock causes it to first become a level 2 lock,
then a level 1 lock, and then the lock on the volume is completely
released.
When a level 3 lock (which does not permit reads or writes) is released
and becomes a level 2 lock, all queued reads are started up. When the
level 2 lock (which allows reads and does not permit writes) is released,
all queued writes are started up if the permissions on the level 1 lock
allows writes. Otherwise, the queued writes are started when the level
1 lock is released.

@Returns:
0 for no errors, otherwise the error code is returned.
(The return value is the value of the AX register after the IOCTL call)

@See: DiskLockPhysicalVolume, DiskUnlockLogicalVolume, DiskLockLogicalVolume
@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskUnlockPhysicalVolume(BYTE byDriveNum)
{
    auto UINT  uError = 0;

#ifdef _M_ALPHA
	uError = 1;
#elif defined (SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byDriveNum;
    regsIn.reg_ECX = 0x0000086B;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
                                                           // If carry, save off error
    if ( regsOut.reg_Flags & 0x00000001 )
        uError = regsOut.reg_EAX;

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;         // generic IOCTL
    regs.h.bl = byDriveNum;     // drive to unlock
    regs.h.ch = 0x08;           // device category
    regs.h.cl = 0x6B;           // UnLock Physical Volume

    Int(0x21,&regs,&sregs);     // Int 21h


    if (regs.x.cflag)
        uError = regs.x.ax;
    else
        uError = 0;

#endif

    return(uError);

}

/*@API:**********************************************************************
@Declaration: UINT SYM_EXPORT WINAPI DiskEnumerateOpenFiles(BYTE byDriveNum, LPBYTE lpszFileName, UINT uFileIndex)

@Description:
This function returns information about one file at a time. To enumerate
all open files, the function must be called repeatedly with the FileIndex
parameter set to a new value for each call. FileIndex should be set to zero
initially, then incremented by one for each subsequent call. The function
returns ERROR_NO_MORE_FILES when all open files on the volume have been
enumerated.

This function may return inconsistent results when used to enumerate files
on an active volume (that is, on a volume where other processes may be
openinga nd closing files). Applications should use Lock Logical Volume
(Interrupt 21h Function 440Dh Minor Code 4Ah) to take a level-3 lock before
enumerating open files.

@Parameters:
$byDriveNum$

Drive to enumerate.

1 for A, 2 for B and so on.

$lpszFileName$
Buffer to receive the file name.

$uFileIndex$

Index of open file to look for.


@Returns:
0 for no errors, otherwise the error code is returned.
(The return value is the value of the AX register after the IOCTL call)

@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskEnumerateOpenFiles(BYTE byDriveNum, LPBYTE lpszFileName, UINT uFileIndex)
{
    auto UINT  uError = 0;

#ifdef _M_ALPHA
    SYM_ASSERT(FALSE);
	uError = 1;
#elif defined(SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    *lpszFileName = EOS;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byDriveNum;
    regsIn.reg_ECX = 0x0000086D;
    regsIn.reg_EDX = (DWORD) lpszFileName;
    regsIn.reg_ESI = uFileIndex;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
                                        // If carry, save off error
    if ( regsOut.reg_Flags & 0x00000001 )
        uError = regsOut.reg_EAX;

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    *lpszFileName = EOS;

    regs.x.ax = 0x440D;                 // generic IOCTL
    regs.h.bl = byDriveNum;             // drive to unlock
    regs.x.cx = 0x086D;                 // Enumerate Open Files
    regs.x.si   = uFileIndex;

    Int(0x21,&regs,&sregs);             // Int 21h

    uError = regs.x.ax;

#define ERROR_NO_MORE_FILES     18

#endif

    if (!*lpszFileName)
        uError = ERROR_NO_MORE_FILES;

    return(uError);

}

/*@API:**********************************************************************
@Declaration: UINT SYM_EXPORT WINAPI DiskEnumerateMemoryFiles(BYTE byDriveNum, LPBYTE lpszFileName, UINT uFileIndex, LPWORD lpwType, LPWORD lpwOpenMode)

@Description:
This function returns information about one file at a time. To enumerate
all open files, the function must be called repeatedly with the FileIndex
parameter set to a new value for each call. FileIndex should be set to zero
initially, then incremented by one for each subsequent call. The function
returns ERROR_NO_MORE_FILES when all open files on the volume have been
enumerated.

This function may return inconsistent results when used to enumerate files
on an active volume (that is, on a volume where other processes may be
openinga nd closing files). Applications should use Lock Logical Volume
(Interrupt 21h Function 440Dh Minor Code 4Ah) to take a level-3 lock before
enumerating open files.

@Parameters:
$byDriveNum$

Drive to enumerate.

1 for A, 2 for B and so on.

$lpszFileName$
Buffer to receive the file name.

$uFileIndex$

Index of open file to look for.

$lpwType$

If no error one of the following:

    ENUM_NORMAL_FILE
    ENUM_MEMORY_FILE
    ENUM_UNMOVABLE_FILE

$lpwOpenMode$

If no error one of the following:

    Access modes
        OPEN_ACCESS_READONLY (0000h)
        OPEN_ACCESS_WRITEONLY (0001h)
        OPEN_ACCESS_READWRITE (0002h)
        OPEN_ACCESS_RO_NOMODLASTACCESS (0004h)
    Share modes
        OPEN_SHARE_COMPATIBLE (0000h)
        OPEN_SHARE_DENYREADWRITE (0010h)
        OPEN_SHARE_DENYWRITE (0020h)
        OPEN_SHARE_DENYREAD (0030h)
        OPEN_SHARE_DENYNONE (0040h)

    Note that if a memory-mapped file is returned ENUM_MEMORY_FILE,
    the value returned in the lpwOpenMode is limited to one of the
    following values:

        OPEN_ACCESS_READONLY (0000h)
        OPEN_ACCESS_READWRITE (0002h)

@Returns:
0 for no errors, otherwise the error code is returned.
(The return value is the value of the AX register after the IOCTL call)

@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskEnumerateMemoryFiles(BYTE byDriveNum, LPBYTE lpszFileName, UINT uFileIndex, LPWORD lpwType, LPWORD lpwOpenMode)
{
    auto UINT  uError = 0;

#ifdef _M_ALPHA
    SYM_ASSERT(FALSE);
	uError = 1;
#elif defined(SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    *lpszFileName = EOS;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byDriveNum;
    regsIn.reg_ECX = 0x0000086D;
    regsIn.reg_EDX = (DWORD) lpszFileName;
    regsIn.reg_ESI = uFileIndex;
    regsIn.reg_EDI = 1;

    uError = IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
    *lpwType = (WORD) regsOut.reg_ECX;
    *lpwOpenMode = (WORD) regsOut.reg_EAX;

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    *lpszFileName = EOS;

    regs.x.ax = 0x440D;                 // generic IOCTL
    regs.h.bl = byDriveNum;             // drive to unlock
    regs.x.cx = 0x086D;                 // Enumerate Memory Files
    regs.x.si = uFileIndex;
    regs.x.di = 1;

    Int(0x21,&regs,&sregs);             // Int 21h

    if (regs.x.cflag)
        uError = regs.x.ax;
    else
        {
        *lpwType = (WORD) regs.x.cx;
        *lpwOpenMode = (WORD) regs.x.ax;
        }

#define ERROR_NO_MORE_FILES     18

#endif

    if (!*lpszFileName)
        uError = ERROR_NO_MORE_FILES;

    return(uError);

}

/*@API:**********************************************************************
@Declaration: BOOL SYM_EXPORT WINAPI FileIsOpen2(LPCSTR lpszFullPathName)

@Description:
This function will return TRUE if the given file is open, FALSE otherwise.

@Parameters:
$lpszFullPathName$

Fully qualified file name with path.

@Returns:
This function will return TRUE if the given file is open, FALSE otherwise.

@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
BOOL SYM_EXPORT WINAPI FileIsOpen2(LPCSTR lpszFullPathName)
{
    BOOL bOpen = FALSE;

#if defined(SYM_WIN32) || defined(SYM_DOS) || defined(SYM_PROTMODE)

    BYTE byDriveNum = CharToUpper(*lpszFullPathName) - 'A' + 1;
    UINT uIndex = 0;
    char szPath[SYM_MAX_PATH];
    char szOpenFileName[SYM_MAX_PATH];

    szPath[0] = szOpenFileName[0] = EOS;
    NameReturnPath(lpszFullPathName, szPath);

    while (DiskEnumerateOpenFiles(byDriveNum, szOpenFileName, uIndex++) == NOERR)
        if (!STRICMP(szOpenFileName, lpszFullPathName))
            {
            bOpen = TRUE;
            break;
            }
#endif

    return(bOpen);
}


/*@API:**********************************************************************
@Declaration: UINT SYM_EXPORT WINAPI DiskGetWindowsSwapFile(LPBYTE lpszFileName)

@Description:
Retrieves information about the swap file.

@Parameters:
$lpszFileName$

Buffer to receive the file name.


@Returns:
0 for no errors, otherwise the error code is returned.
(The return value is the value of the AX register after the IOCTL call)

@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskGetWindowsSwapFile(LPBYTE lpszFileName)
{
    auto UINT  uError = 0;

#ifdef _M_ALPHA
    SYM_ASSERT(FALSE);
	uError = 1;
#elif defined(SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_ECX = 0x0000086E;
    regsIn.reg_EDX = (DWORD) lpszFileName;

    uError = IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;                 // generic IOCTL
    regs.x.cx = 0x086E;                 // Enumerate Open Files

    Int(0x21,&regs,&sregs);             // Int 21h

    uError = regs.x.ax;


#endif

    return(uError);

}

/*@API:**********************************************************************
@Declaration: BOOL SYM_EXPORT WINAPI DiskPollWrites(BYTE byDriveNum)

@Description:
Checks to see if any writes have occurred to a drive.

@Parameters:
$byDriveNum$

Drive number, A=1, B=2 etc, to be checked.


@Returns:
FALSE if no writes,
TRUE if writes.

@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
BOOL SYM_EXPORT WINAPI DiskPollWrites(BYTE byDriveNum)
{
    auto UINT  uError = 0;

#ifdef _M_ALPHA
    SYM_ASSERT(FALSE);
    uError = 1;
#elif defined(SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    MEMSET(&regsIn, 0, sizeof(regsIn) );
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byDriveNum;
    regsIn.reg_ECX = 0x0000086C;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);

    uError = regsOut.reg_EAX;

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;                 // generic IOCTL
    regs.h.bl = byDriveNum;             // drive to unlock
    regs.x.cx = 0x086C;                 // Enumerate Memory Files

    Int(0x21,&regs,&sregs);             // Int 21h

    uError = regs.x.ax;

#define ERROR_NO_MORE_FILES     18

#endif

    return (uError != 0);
}

/*@API:**********************************************************************
@Declaration: BOOL SYM_EXPORT WINAPI DiskGetCurrentLockState(BYTE byDriveNum, LPWORD lpwLockLevel, LPWORD lpwLockPermissions)

@Description:
Retrieves the current volume lock level and permissions on the specified drive

@Parameters:
$byDriveNum$

Drive number, default=0, A=1, B=2 etc, to be checked.

$lpwLockLevel$

Pointer to WORD to receive the current lock level, or NULL.
The lock level may be either -1 (if the volume is not locked), 0, 1, 2, or 3.

$lpwLockPermissions$

Pointer to WORD to receive the lock permission bits, or NULL.

@Returns:
0 on success.
Error code on failure.

@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskGetCurrentLockState(BYTE byDriveNum, LPWORD lpwLockLevel, LPWORD lpwLockPermissions)
{
    UINT uError = 0;

#ifdef _M_ALPHA
    SYM_ASSERT(FALSE);
	uError = 1;
#elif defined(SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    MEMSET(&regsIn, 0, sizeof(regsIn));
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = byDriveNum;
    regsIn.reg_ECX = 0x00000870;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
    
                                        // If carry, save off error
    if (regsOut.reg_Flags & 0x00000001)
        uError = regsOut.reg_EAX;
    else
        {
        if (lpwLockLevel)
            *lpwLockLevel = (WORD)regsOut.reg_EAX;
        if (lpwLockPermissions)
            *lpwLockPermissions = (WORD)regsOut.reg_ECX;
        uError = 0;
        }

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;         // generic IOCTL
    regs.h.bl = byDriveNum;     // drive to unlock
    regs.h.ch = 0x08;           // device category
    regs.h.cl = 0x70;           // Get Current Lock State

    Int(0x21,&regs,&sregs);     // Int 21h


    if (regs.x.cflag)
        uError = regs.x.ax;
    else
        {
        if (lpwLockLevel)
            *lpwLockLevel = regs.x.ax;
        if (lpwLockPermissions)
            *lpwLockPermissions = regs.x.cx;
        uError = 0;
        }

#endif

    return uError;
}

/*@API:**********************************************************************
@Declaration: BOOL SYM_EXPORT WINAPI DiskMuckWithSwapFile(BOOL bStart)

@Description:
This new ioctl basically is used to bracket the window during which
the swapfile is moved. The ioctl is called with the BEGIN flag to
indicate the swap file is going to be moved and with the END flag to
indicate the swap file movement is finished. If the swapfile is moved
outside of this window, all bets are off. If the swapfile is moved within
the BEGIN/END sequence, then the FAT cache is invalidated before every
access to the swapfile. 

Caveats:
1. All calls must be made while within the level 3 lock and must be made
   by the lock owner.
2. The invalidating of the FAT cache happens only during the level 3 lock;
   the FAT cache is kept around at any other lock level.
3. If a BEGIN call is made and the END call is not made and the level 3 lock
   released; the next time the level 3 lock is taken, the BEGIN flag
   automatically becomes active again. An explicit END call has to be made to
   go back to default behaviour. Obviously, this is not very good for
   performance. 
4. If the level 1 lock is released without making the END call, the entire
   state goes away and we are back to normal behaviour.
5. If an END call is made without the corresponding BEGIN call, it is failed.
6. Any number of BEGIN calls can be made, they are equivalent to a single
   BEGIN i.e. no counter is maintained.

@Parameters:
$bStart$

True if you are starting to move swap file, FALSE if you are done.

@Returns:
0 on success.
Error code on failure.

@Include: disk.h
@Compatibility: Win16, Win32, DOS
*****************************************************************************/
UINT SYM_EXPORT WINAPI DiskMuckWithSwapFile(BOOL bStart)
{
    UINT uError = 0;

#ifdef _M_ALPHA
    SYM_ASSERT(FALSE);
	uError = 1;
#elif defined(SYM_WIN32)
    DIOC_REGISTERS regsIn;
    DIOC_REGISTERS regsOut;

    MEMSET(&regsIn, 0, sizeof(regsIn));
    regsIn.reg_EAX = 0x0000440D;
    regsIn.reg_EBX = bStart ? 1 : 2;
    regsIn.reg_ECX = 0x0000484C;

    IOCtlVWin32(&regsIn, &regsOut, VWIN32_DIOC_DOS_IOCTL);
    
                                        // If carry, save off error
    if (regsOut.reg_Flags & 0x00000001)
        uError = regsOut.reg_EAX;
    else
        uError = 0;

#elif defined(SYM_DOS) || defined(SYM_PROTMODE)

    union REGS regs;
    struct SREGS sregs;

    MEMSET(&regs,0,sizeof(regs));
    MEMSET(&sregs,0,sizeof(sregs));

    regs.x.ax = 0x440D;         // generic IOCTL
    regs.h.bl = bStart ? 1 : 2;
    regs.h.ch = 0x48;           // device category
    regs.h.cl = 0x4C;

    Int(0x21,&regs,&sregs);     // Int 21h


    if (regs.x.cflag)
        uError = regs.x.ax;
    else
        uError = 0;

#endif

    return uError;
}


#ifdef SYM_WIN32
#ifndef _M_ALPHA
HANDLE hVWin32;
LRESULT SYM_EXPORT WINAPI IOCtlVWin32(DIOC_REGISTERS *lpRegsIn, DIOC_REGISTERS *lpRegsOut, DWORD  dwIoControlCode)
{
    DWORD dwLastError = 0;
    UINT cbBytesReturned;

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
        dwLastError = ERROR_INVALID_FUNCTION;
        lpRegsOut->reg_EAX = dwLastError;
        lpRegsOut->reg_Flags |= 0x00000001;
        }
    else
        {
        if (hVWin32 == NULL)
            hVWin32 = CreateFile( "\\\\.\\VWIN32", 0, FILE_SHARE_WRITE, NULL,
                  OPEN_EXISTING, 0, NULL);
        if (hVWin32 != INVALID_HANDLE_VALUE)
            {
            if (!DeviceIoControl(hVWin32, dwIoControlCode,
                        lpRegsIn, sizeof(*lpRegsIn),
                        lpRegsOut, sizeof(*lpRegsOut),
                        &cbBytesReturned,
                        NULL))
                {
                dwLastError = lpRegsOut->reg_EAX;
                }
            }
        else
            {
            dwLastError = GetLastError();
            lpRegsOut->reg_EAX = dwLastError;
            lpRegsOut->reg_Flags |= 0x00000001;
            hVWin32 = NULL;
            }
        }
    return (dwLastError);
}


////////////////////////////////////////////////////////////////////////
// Data for accessing the Symkrnl VxD 
////////////////////////////////////////////////////////////////////////

static char g_szSYMKRNLVXD[SYM_MAX_PATH] =  QMODULE_SYMKRNL_VXD; // VxD name
static W32IOCTLDATA g_symkrnlIoCtlData;   	// VxD control data
HANDLE hSymkrnlVxD = NULL;			// VxD handle


////////////////////////////////////////////////////////////////////////
// These functions allow applications to get/change the default VxD name 
// Currently used by SDL
////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI SymkrnlVxDSetName (LPCSTR lpszVxDName)
{
    // Note the VxD loading works with short names only, so
    // we convert the given name to shortname.
    NameReturnShortName(lpszVxDName, g_szSYMKRNLVXD);
}

LPCSTR SYM_EXPORT WINAPI SymkrnlVxDGetName (void)
{
    return (LPCSTR)g_szSYMKRNLVXD;
}

////////////////////////////////////////////////////////////////////////
// This function loads a dynamic VxD and returns a handle to it
////////////////////////////////////////////////////////////////////////
HANDLE WINAPI SymkrnlVxDInit(LPSTR lpszVxDName, W32IOCTLDATA* lpW32IoCtlData)
{
    HANDLE  hVxD;
    UINT    uErr;
    char    szBuf[SYM_MAX_PATH];

    uErr = LoadVxD ( lpW32IoCtlData, lpszVxDName, VXDLOAD_ALL );

    if (uErr != VXDLOAD_ERR_NONE)
        {
        ShowVxDError( uErr, lpszVxDName, lpW32IoCtlData->base.W32IO_ErrorCode );
        return(NULL);
        }

    STRCPY(szBuf, "\\\\.\\");
    STRCAT(szBuf, lpszVxDName);
	AnsiToOem(szBuf, szBuf);
    if ((hVxD=CreateFile(szBuf, 0,0,0,OPEN_EXISTING,0,0)) == INVALID_HANDLE_VALUE)
        {
        ShowVxDError( VXDLDR_ERR_FILE_OPEN_ERROR, lpszVxDName, lpW32IoCtlData->base.W32IO_ErrorCode );
        FreeVxD ( lpW32IoCtlData );
        return(NULL);
        }

    return(hVxD);
}


////////////////////////////////////////////////////////////////////////
// This function unloads a dymamic VxD
////////////////////////////////////////////////////////////////////////
void WINAPI SymkrnlVxDTerminate(HANDLE hVxD, LPSTR lpszVxDName, W32IOCTLDATA* lpW32IoCtlData)
{
    if (hVxD)
        {
        CloseHandle(hVxD);

	if (FreeVxD ( lpW32IoCtlData ) != VXDLOAD_ERR_NONE)
	    ShowVxDError( VXDLDR_ERR_DEVICE_UNLOADABLE, lpszVxDName, lpW32IoCtlData->base.W32IO_ErrorCode );
	}
}


//***********************************************************************
//**     _VolFlush - volume based flush/invalidate
//
// Entry TOS     = logical unit number to flush/invalidate
//       TOS+4   = flags
//                 0                     ; just flush the cache
//                 VOL_DISCARD_CACHE     ; flush and invalidate the cache
//                 VOL_REMOUNT           ; flush and invalidate the cache
//                                       ; & remount the vol (recompute
//                                       ; the FAT "geometry" variables)
//
// Exit  (eax) = non-zero if error on flush/invalidate
//               zero if NO ERROR
// Uses  C registers  (eax, ecx, edx, flags)
//
//***********************************************************************
                                        // These are the values from IFS.H which
                                        // when included causes a mess
                                        // of conflicts.
#define VOL_FLUSH_CACHE     0
#define VOL_DISCARD_CACHE       1
#define VOL_REMOUNT                 2
BOOL _VolFlush( BYTE driveLetter, int flushType )
{
#define VOLFLUSH_SERVICE_ID     0x004C
        DWORD dwResult;

    return( VxDStkBasedCall( (IFSMgr_Device_ID << 16) + VOLFLUSH_SERVICE_ID,
                                         &dwResult,
                     2,                 // number of paramters.
                     driveLetter - 'A', flushType ) );
}
#endif // _M_ALPHA

//***********************************************************************
// Just flush the cache
//***********************************************************************
BOOL SYM_EXPORT WINAPI DiskVCacheFlush(BYTE driveLetter)
{
#ifndef _M_ALPHA
    return( _VolFlush(driveLetter, VOL_FLUSH_CACHE ) );
#else
	return FALSE;
#endif	
}

//***********************************************************************
// Flush and invalidate the cache
//***********************************************************************
BOOL SYM_EXPORT WINAPI DiskVCacheInvalidate( BYTE driveLetter )
{
#ifndef _M_ALPHA
    return( _VolFlush(driveLetter, VOL_DISCARD_CACHE) );
#else
	return FALSE;
#endif	
}


#ifndef _M_ALPHA
BOOL _VxDRegBasedCall( DWORD dwVxDService, int vxdFuncNum, LPSYM_REGS lpRegs )
{
    extern  HANDLE     hSymkrnlVxD;
    auto    VxDParams  params;
    auto    BOOL       bSuccess;
    auto    DWORD      dwBytesReturned;

    if (!hSymkrnlVxD)
	    VxDServicesInit();

    if (!hSymkrnlVxD)
        return(FALSE);

    MEMSET( &params, 0, sizeof(params) );
    params.CallNum = dwVxDService;      // Call or Interrupt number
    params.InEAX = lpRegs->EAX;
    params.InEBX = lpRegs->EBX;
    params.InECX = lpRegs->ECX;
    params.InEDX = lpRegs->EDX;
    params.InESI = lpRegs->ESI;
    params.InEDI = lpRegs->EDI;
    params.InEBP = lpRegs->EBP;

                                        // Call VxD to perform operation
    bSuccess = DeviceIoControl(hSymkrnlVxD,// SysWatch VxD Handle
                   vxdFuncNum,          // Function
                   (LPVOID)&params,     // input buffer
                   sizeof(params),      // input buffer size
                   NULL,                // no output buffer
                   0,                   // no output buffer size
                   &dwBytesReturned, NULL);         // sync call

    lpRegs->EAX   = params.OutEAX;
    lpRegs->EBX   = params.OutEBX;
    lpRegs->ECX   = params.OutECX;
    lpRegs->EDX   = params.OutEDX;
    lpRegs->ESI   = params.OutESI;
    lpRegs->EDI   = params.OutEDI;
    lpRegs->FS    = (WORD) params.OutFS;
    lpRegs->GS    = (WORD) params.OutGS;
    lpRegs->EBP   = params.OutEBP;
    lpRegs->EFlags = params.OutEFLAGS;

    return( bSuccess );
}

BOOL SYM_EXPORT WINAPI VxDStkBasedCall( DWORD dwVxDService, LPDWORD lpdwResult, DWORD dwNumParams, ... )
{
    auto 	VxDPushParams 	pushParams;
    extern  HANDLE     	hSymkrnlVxD;
    auto 	DWORD 		i;
    auto   	BOOL 		bSuccess;
    auto    DWORD       dwBytesReturned;


    if (!hSymkrnlVxD)
	    VxDServicesInit();

    if (!hSymkrnlVxD)
        return(FALSE);
                                        // Service # for _VolFlush which
                                        // is otherwise defined for VxD's
                                        // in IFSMGR.INC.
    pushParams.S_CallNum =  dwVxDService;
    pushParams.S_NumP    =  dwNumParams; // # parameters wto be passed
                                         // into S_P array.  And they follow:
    for(i = 0; i < dwNumParams; i++)
        pushParams.S_P[i] =  (&dwNumParams)[1 + i];

                                        // Call VxD to perform operation
    bSuccess = DeviceIoControl(hSymkrnlVxD,// SysWatch VxD Handle
                   SYMKVxD_FN_STKBASED, // Function 4: NUVXD_FN_IOSSRV
                   (LPVOID)&pushParams, // input buffer
                   sizeof(pushParams),  // input buffer size
                   NULL,                // no output buffer
                   0,                   // no utput buffer size
                   &dwBytesReturned, NULL);         // sync call
        *lpdwResult = pushParams.S_OutEAX;
        return (bSuccess);
}

BOOL SYM_EXPORT WINAPI VxDRegBasedCall( DWORD dwVxDService, LPSYM_REGS lpRegs )
{
     return(_VxDRegBasedCall( dwVxDService, SYMKVxD_FN_REGBASED, lpRegs ));
}

BOOL SYM_EXPORT WINAPI VxDIntBasedCall( int Interrupt, LPSYM_REGS lpRegs )
{
     return(_VxDRegBasedCall( (DWORD) Interrupt, SYMKVxD_FN_VXDINT, lpRegs ));
}


//***********************************************************************
// Call this function to force the Symkrnl VxD to load.
//***********************************************************************
BOOL SYM_EXPORT WINAPI VxDServicesInit()
{
    static BOOL bFirstTime = TRUE;        

                                        // We only want to call this functions
                                        // once, otherwise we'll get
    if (bFirstTime)
        {
        bFirstTime = FALSE;
        if (!hSymkrnlVxD)
        	{
            // WES 08/10/97.  Symkrnl is being loaded in context of explorer
            // by some NU shell extensions.  None of the default finding logic
            // works so we begin by building what should be the full path to
            // the symkrnl vxd (assuming it is in the same directory as the
            // dll).
    		char szSymKrnlPathLong[SYM_MAX_PATH];
    		char szSymKrnlPathShort[SYM_MAX_PATH];
			// Basil 02/01/98 SDL links to symkrlib statically, so the GeModuleHandle
			// code will not work because QMODULE_SYMKRNL is not loaded. 
			// SDL is the only client that sets g_szSYMKRNLVXD to the full path
			// of the VxD (default is symkrnl.vxd with no path), so use this to
			// bypass the GetModuleHandle code.
			if(!NameIsFull(g_szSYMKRNLVXD))
				{
    			HMODULE hModule = GetModuleHandle(QMODULE_SYMKRNL);
    			if(hModule == NULL)
    				return FALSE;
				if(GetModuleFileName(hModule,szSymKrnlPathLong,SYM_MAX_PATH) == 0)
					return FALSE;
				NameStripFile(szSymKrnlPathLong);
				NameAppendFile(szSymKrnlPathLong,g_szSYMKRNLVXD);
				NameAppendExtension(szSymKrnlPathLong,"VXD");
				}
			else
				{
				STRCPY(szSymKrnlPathLong,g_szSYMKRNLVXD);
				}
            
			szSymKrnlPathShort[0] = 0;
			NameReturnShortName(szSymKrnlPathLong,szSymKrnlPathShort);
            // If the short path is empty the file probably doesn't exist.  Fall
            // back to using default name.
			if(szSymKrnlPathShort[0] == 0)
				strcat(szSymKrnlPathShort,g_szSYMKRNLVXD);
            hSymkrnlVxD = SymkrnlVxDInit(szSymKrnlPathShort, &g_symkrnlIoCtlData);
            }
        }
        
    if (!hSymkrnlVxD)
        return FALSE;
    else
        return TRUE;
}

//***********************************************************************
// Call this function to force the Symkrnl VxD to unload.
// This should only be called by DLLMain() when Symkrnl is going away.
//***********************************************************************
void WINAPI VxDServicesTerminate()
{
    if (hSymkrnlVxD)
        {
	SymkrnlVxDTerminate(hSymkrnlVxD, g_szSYMKRNLVXD, &g_symkrnlIoCtlData);
        hSymkrnlVxD = NULL;
        }
}

#endif // _M_ALPHA
#endif /* SYM_WIN32 */
