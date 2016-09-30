// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/scn_boot.cpv   1.39   13 Feb 1997 13:17:26   MKEATIN  $
//
// Description:
//      These are the functions used to scan the Master Boot Record and
//      boot records for viruses.
//      NOTE: You must call VirScanInit() prior to using these functions
//      and VirScanEnd() after all scanning is done to clean up.
//
// Contains:
//      VirScanMasterBoot()
//      VirScanBoot()
//      VirusScanLockUnlockDiskP()
//      VirusScanLockUnlockDiskL()
//      VirusScanMapLogicalToPhysical()
// See Also:
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/scn_boot.cpv  $
// 
//    Rev 1.39   13 Feb 1997 13:17:26   MKEATIN
// Ported changes from the F branch.
// 
//    Rev 1.38.1.1   13 Dec 1996 09:57:34   JBRENNA
// Make GetPartitionStartLoc(), GetPartitionEndLoc(), and
// DetermineStartingSectorAndType() NEC partition table aware. Remove the
// 'AndType' from DetermineStartingSectorAndType() because we do not know
// how to extract the partition type from the NEC partition table (also, the
// type was unused.)
// 
//    Rev 1.38.1.0   23 Oct 1996 13:14:08   RJACKSO
// Branch base for version QAKF
// 
//    Rev 1.38   17 Jul 1996 21:13:14   RAY
// Removed all traces of VirusScanLockNTBootSector() and
// VirusScanUnlockNTBootSector().  Certlib's default callback,
// LocalDiskWritePhysical(), now handles NT Boot Sectors properly
// and these were just getting in the way.
// 
//    Rev 1.37   15 Jul 1996 15:56:04   RAY
// Changed VirusScanMapLogToPhyParams() to return the geometry of the first
// Physical Drive used by a Logical Volume instead of the last one.  This
// allows us to correctly locate the Boot Sector or sectors relative to it.
// 
//    Rev 1.36   10 Jul 1996 00:23:18   DALLEE
// Do cleanup (free mem) when exiting after new error checking introduced
// w/ Luigi code in VirusScanBoot().
// 
//    Rev 1.35   27 Jun 1996 15:22:58   JBRENNA
// Merge LuigiPlus changes into Gryphon:
// JWorden - Add VirusScanMapLogicalToPhysical (). Returns physical drive number
// for the logical drive letter.
// MZaremb - STS 53117 - Added error detection to VirusScanBoot to fix a divide
// by zero error.
// 
//    Rev 1.34   06 Feb 1996 21:30:50   BARRY
// Fix LPSTR to LPBYTE problems due to Unicode effort
// 
//    Rev 1.33   06 Feb 1996 21:24:20   BARRY
// Fix LPSTR to LPBYTE problems due to Unicode effort
// 
//    Rev 1.32   12 Jan 1996 07:11:14   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.27.1.23   15 Dec 1995 15:48:40   DALLEE
// Added local function AllocAlignedPtr() to obtain the aligned and
// base pointers for W32 allocations for DiskAbsOperation().
// Cleaner, easier-to-maintain implementation than before.
// Plus, you can easily VDIFF vs. 1.27.1.21.
// 
//    Rev 1.27.1.22   15 Dec 1995 14:22:58   DALLEE
// For W32 platform, align all buffers passed to DiskAbsOperation() so they
// lie within the same 4K memory page.  Straddling page boundaries will cause
// reads with Promise EIDE 4030+ drivers to silently fail.
// 
//    Rev 1.27.1.21   11 Dec 1995 22:49:00   MKEATIN
// Commented out DiskLocking in VirusScanMasterBoot()
// 
//    Rev 1.27.1.20   04 Dec 1995 14:56:06   MKEATIN
// If VirusScanRepairBoot() fails to initialize stInt13rec return error
// 
//    Rev 1.27.1.19   28 Nov 1995 13:52:20   MKEATIN
// 
// 
//    Rev 1.27.1.18   28 Nov 1995 13:44:04   MKEATIN
// Logical SCSI drives will now get scanned.  if int13rec is not used we
// call the old DiskGetInfo().
// 
//    Rev 1.31   21 Nov 1995 22:57:30   DALLEE
// 1. Move DetermineStartingSector() outside repair boot loop.
// 2. Changed DetermineStartingSector() to ...SectorAndType() to
//    also return type of partition.
// 3. For NT: On hard drive FAT boot repairs, lock the logical volume
//    before the repair, then dismount and unlock after the repair.
//    This causes NT to dump its cache of the boot sector.  Otherwise,
//    NT puts the infected sector back on the next write access to the 
//    logical drive.
// TODO: Need to change VirusScanRepairBoot() API to return error values
//       so we can report failure for couldn't dismount NT FAT volume vs. 
//       the repair just chunked.      
// 
//    Rev 1.27.1.17   21 Nov 1995 12:10:26   DALLEE
// Free repair boot buffer when returning early because disk couldn't be 
// locked.
// 
//    Rev 1.27.1.16   09 Nov 1995 19:51:12   DALLEE
// Changed strncmp to strnIcmp when checking result from QueryDosDevice().
// NT is inconsistent in the case of the string it returns.
// 
//    Rev 1.27.1.15   09 Nov 1995 14:56:06   DCHI
// Changed calls of SCN4_VERIFY_NAME to SCN4_GET_NAME.
// 
//    Rev 1.27.1.14   30 Oct 1995 15:13:48   DALLEE
// 
// Bunch o' boot scan changes:
// 1. Added VirusScanMapLogToPhySector() to cover DiskMapLogToPhySector().
//     - Kluges C: to always return active partition on 1st HD.
//     - For NT, uses DeviceIoControl() to determine drive and partition.
//     - Routes through DMLTPS() for other cases.
// 2. Minor fixes for determining physical drive # in VirusScanRepairBoot().
// 3. DetermineStartingSector() will return 0,0,1 for drives < 0x80 (floppies).
// 
//    Rev 1.27.1.13   24 Oct 1995 17:15:28   CNACHEN
// VirusScanMasterBoot now has a uPhysDriveNum argument.
// 
//    Rev 1.27.1.12   23 Oct 1995 10:31:00   CNACHEN
// Removed definitions for VirusScanLockUnlockDiskP & VirusScanLockUnlockDiskL
// to CERTLIBN...
// 
// 
//    Rev 1.27.1.11   18 Oct 1995 14:35:16   CNACHEN
// Added proper drive locking...
// 
//    Rev 1.27.1.10   16 Oct 1995 14:17:56   RSTANEV
// Fixed revs...  I screwed up by checking in a file from the branch into
// the trunk.  Now everything is OK.  Trunk's rev 1.28 (at the time) was
// deleted and its changes were merged into rev 1.27.1.9...  Confused?
//
//    Rev 1.27.1.9   16 Oct 1995 13:53:16   DCHI
// Changed avex.h to navex.h.
//
//    Rev 1.27.1.8   13 Oct 1995 14:37:40   DCHI
// Changed avex.h to navex.h.
//
//    Rev 1.27.1.7   09 Oct 1995 17:14:26   CNACHEN
// Added ELSE clause to repairboot function.  Used to allocate memory, and if it
// failed, just zero out it out and try to free it.
//
//    Rev 1.27.1.6   25 Sep 1995 10:50:40   CNACHEN
// fixed scan boot problem: now call DiskGetPhysicalInfo before calling
// DiskAbsOperation with respect to reading boot sectors
//
//    Rev 1.27.1.5   25 Sep 1995 10:08:34   CNACHEN
//
//    Rev 1.27.1.4   18 Sep 1995 15:41:00   CNACHEN
//
//    Rev 1.27.1.3   18 Sep 1995 13:40:14   CNACHEN
// fixed bug where external part repair code is receiving wrong drive # from NAV..
//
//    Rev 1.27.1.2   13 Sep 1995 17:04:46   CNACHEN
// Fixed boot scanning for Jackal type viruses.  Ports need to be made to
// Nirvana...
//
//    Rev 1.27.1.1   07 Sep 1995 13:49:10   CNACHEN
// Added external virus detection code.
//
//    Rev 1.27.1.0   12 Jul 1995 18:59:52   MPYE
// Branch base for version QAK8
//
//    Rev 1.27   06 Jul 1995 11:30:44   DALLEE
// Changed toupper() to quake cover CharToUpper().
//
//    Rev 1.26   05 Jul 1995 20:15:10   MARKL
// Fixed defect #39278 where receiving drag/drops from Norton Navigator
// caused a low level GPF.  Bullet proofed scan* routines such that they
// don't choke when passed a lowercase drive spec.
//
//    Rev 1.25   25 May 1995 03:11:20   SZIADEH
// 1. if 'C' drive, peform a physical disk read on the boot sector.
// 2. VirusScanCanAccessDisk alwasy succeedes on reads.
// 3. VirusScanCanAccessDisk is not necessary in DOS platform.
//
//    Rev 1.24   03 May 1995 17:58:08   MFALLEN
// We had problems with scanning peer to peer.
//
//    Rev 1.23   03 May 1995 15:01:34   BRAD
// Only test for locked drive, if Win95
//
//    Rev 1.22   01 May 1995 18:38:34   DALLEE
// Removed previous change.  DiskGetCurrentLockState() is now exported from
// 16 bit SYMNAV.
//
//    Rev 1.21   01 May 1995 16:10:04   DALLEE
// Temoporarily #ifdef'd DiskGetCurrentLockState() out of
// VirusScanCanAccessDisk() for WIN16.  I'll fix this in the WIN16 SYMNAV
// exports section after the ET1 build.
//
//    Rev 1.20   27 Apr 1995 12:17:00   MFALLEN
// Made access function global.
//
//    Rev 1.19   27 Apr 1995 11:50:28   MFALLEN
// Added drive lock checking.
//
//    Rev 1.18   21 Apr 1995 10:28:40   DALLEE
// Added a verify for the floppy DiskAbsOperation(...) in VirusScanBoot(...) to
// insure that any errors returned were valid.
// -CGRANGER
//
//
//    Rev 1.17   07 Feb 1995 17:17:02   SZIADEH
// change the prototype to UINT.
//
//    Rev 1.16   03 Feb 1995 12:41:28   DALLEE
// Two In-N-Out burgers, please, Sami.
// Removed line manually setting virus index to 0 after boot scan.
//
//    Rev 1.15   27 Dec 1994 19:05:38   DALLEE
// Use CERTLIBN for DOS platform now, too.
//
//    Rev 1.14   19 Dec 1994 22:05:20   DALLEE
// Convert .WIN to CERTLIBN
//
//    Rev 1.13   30 Nov 1994 12:20:48   DALLEE
// - Use repair.h with cts4.h, repairn.h with ctsn.h.
// - Updated generic boot repair logic from QAK2 NAV v3.05 inline.
// - Low level disk calls only available for SYM_WIN32 and SYM_DOS.
//   SYM_WIN16 needs to allocate a low DOS buffer to work with CERTLIB4.
//
//    Rev 1.12   29 Nov 1994 15:02:32   MFALLEN
// wControl -> wControl2
//
//    Rev 1.11   28 Nov 1994 15:29:00   MFALLEN
// include sts4.h if W16 platform.
//
//    Rev 1.10   26 Jul 1994 11:54:58   DALLEE
// Initialize DISKREC struct before call to DiskBlockDevice().
// Was causing havoc w/ div by 0, & page faults in following MemAlloc's in .DX
//
//    Rev 1.9   27 May 1994 15:23:56   MFALLEN
// Added boot scannign.
//
//    Rev 1.8   24 May 1994 14:02:50   MFALLEN
// Reintroduced REP4_ME
//
//    Rev 1.7   17 May 1994 12:53:54   DALLEE
// Cast pointer from MemLock() -- Isn't C++ wonderful?
//
//    Rev 1.6   14 May 1994 12:19:50   BRAD
//
//    Rev 1.5   12 May 1994 11:04:06   MFALLEN
// cts4 2 ctsn
//
//    Rev 1.2   02 May 1994 12:55:04   BRAD
// Variable removed, but needed for DOS
//
//    Rev 1.1   22 Apr 1994 18:14:46   MFALLEN
// Changed _loadds to SYM_EXPORT. If the TSR will need _loadds then this must
// be a macro. Without this change the LINK32 will not generate an import lib.
//
//    Rev 1.0   22 Apr 1994 10:59:06   MFALLEN
// Initial revision.
//
//    Rev 1.33   18 Apr 1994 18:53:42   BRAD
// Replaced with SYM_WIN32 functions
//
//    Rev 1.32   15 Mar 1994 11:09:22   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.31   14 Mar 1994 17:01:06   BRAD
// Drive number to drive letter for SYM_WIN32 was wrong.  Need to add 'A' to it.
//
//    Rev 1.30   14 Mar 1994 10:55:54   BRAD
// GetDriveType for SYM_WIN32 takes path
//
//    Rev 1.29   12 Mar 1994 22:45:56   BRAD
// Cleaned up for SYM_WIN32
//
//    Rev 1.28   02 Mar 1994 17:12:36   BRAD
// AWK script applied
//
//    Rev 1.27   11 Feb 1994 15:54:32   MARKL
// Quake 2 changes moved to trunk.
//
//    Rev 1.26.1.6   16 Dec 1993 15:55:42   DALLEE
// Added flag to SCN4_BOOT for whether to call code chunks from VIRSCAN.DAT
//
//    Rev 1.26.1.5   16 Dec 1993 05:28:28   SZIADEH
// move the memcpy call before sector read.
//
//    Rev 1.26.1.4   14 Dec 1993 22:00:52   DALLEE
// Add path to VIRSCAN.DAT to buffer passed to SCN4_BOO for DOS only.
// Cleaned up some of the GlobalDosAlloc Segment/Selector code.
//
//    Rev 1.26.1.3   06 Dec 1993 17:07:06   SZIADEH
// Increased the size of the work buffer passed to SCN4_BOOT and REP4_BOOT to
// 4K from the original size of 1K. Pass path\VIRSCAN.DAT at the start of the
// work buffer.
//
//
//    Rev 1.26.1.2   08 Nov 1993 16:55:00   keith
// OS/2 Mirrors - switch from MemAllocPtr to MemAlloc style to allow for
// zero-based allocation.
//
//    Rev 1.26.1.1   08 Nov 1993 16:25:52   keith
// OS/2 Mirrors - activate the boot scans.
//
//    Rev 1.26.1.0   19 Oct 1993 08:56:16   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.26   14 Oct 1993 13:22:50   MFALLEN
// #ifndef SYM_WIN-ed all the FileCheckError since this function can't be called
// from a dll outside SYMKRNL. The function GP's in Windows since the DS is
// from the calling program. How did this work before ???
//
//    Rev 1.25   12 Oct 1993 16:42:58   PATRICKM
// Baseline from QAK1 branch
//
//    Rev 1.24.1.6   23 Sep 1993 16:15:12   keith
// Stub functions not yet supported
//
//    Rev 1.24.1.5   16 Sep 1993 03:12:52   DALLEE
// Redid double boot infection repair logic.  Added uRepairType parameter to
// VirusScanRepairBoot().
//
//    Rev 1.24.1.4   12 Sep 1993 01:32:50   MFALLEN
// Removed N30NOTES.bFlag since it's not used anymore.
//
//    Rev 1.24.1.3   11 Sep 1993 13:09:02   MFALLEN
// Error when reading the boot record failed.
//
//    Rev 1.24.1.2   07 Sep 1993 02:05:24   DALLEE
// Placed calls to FileCheckError after CERTLIB4 calls in order to clear
// the critical error handler (DOS) and set the _error variable.
//
//    Rev 1.24.1.1   05 Sep 1993 17:44:00   BRAD
// Don't check return values from memory free routines.  Not needed.
//
//    Rev 1.24.1.0   03 Sep 1993 22:52:48   SCOTTP
// Branch base for version QAK1
//
//    Rev 1.24   01 Sep 1993 18:19:24   MFALLEN
// And return value from DIS4_READ_PH() with 0xFFFF
//
//    Rev 1.23   01 Sep 1993 22:22:00   MFALLEN
// Fixed problem with drive not available notification from DIS4_READ_PHYSICAL()
//
//    Rev 1.22   01 Sep 1993 21:33:26   MFALLEN
// REnamed HelloDiskAreYouThere() to DiskAvailable()
//
//    Rev 1.21   01 Sep 1993 21:10:50   MFALLEN
// Added check to see if drive is there.
//
//    Rev 1.20   23 Aug 1993 01:51:22   BARRY
// Added MODULE_NAME and fixed MemUnlock params for VMM debugging
//
//    Rev 1.19   20 Aug 1993 11:55:04   SZIADEH
// Added code to force physical sector read on floppies.
//
//    Rev 1.18   24 Jul 1993 19:19:38   MFALLEN
// Bug
//
//    Rev 1.17   24 Jul 1993 19:01:58   MFALLEN
// Clear all work buffers so memory scanning should not complain about
// virus found in memory.
//
//    Rev 1.16   21 Jul 1993 17:10:10   MFALLEN
// Added generic boot repair.
//
//    Rev 1.15   20 Jul 1993 19:41:56   MFALLEN
// If repair is successful the drive is rescanned and repaired over and over.
// This feature is required for multiple infections.
//
//    Rev 1.14   18 Jun 1993 00:57:30   DALLEE
// Return the Signiture in Memory Index value and set N30NOTES.wVirID
// to the VIRSCAN.INF lookup index value.
//
//    Rev 1.13   17 Jun 1993 15:08:10   CGUC
// Changed memory size allocation from 520 (hard coded) to the define
// SIZE_DOSBUFFER, we tried to clear memory SIZE_DOSBUFFER(1024) when
// we only allocated 520.
//
//
//    Rev 1.12   14 Jun 1993 23:04:08   SZIADEH
// Fixed parameter list and uncommented REP4_BOOT. Added code to clear the work
// buffer after a virus has been detected.
//
//    Rev 1.11   14 Jun 1993 22:07:14   MFALLEN
// Commented out REP4_BOOT() since it is not documented what the new
// parameters should be. Will check with Sami.
//
//    Rev 1.10   10 Jun 1993 02:50:50   CGUC
// Changed SYM_EXPORT to _loadds
//
//    Rev 1.9   06 Jun 1993 21:42:34   MFALLEN
// Boot Repair code is now given a block of memory that is being allocated
// in the low 640K dos area. Will be tested with Sami in the morning.
//
//    Rev 1.8   04 Jun 1993 09:39:32   MFALLEN
// Sami did some changes.
//
//    Rev 1.7   17 May 1993 15:43:04   DALLEE
// Added VirusScanRepairBoot().
//
//    Rev 1.6   28 Apr 1993 09:09:56   MFALLEN
//
//    Rev 1.5   22 Apr 1993 10:38:54   MFALLEN
// N30Notes
//
//
//    Rev 1.4   04 Apr 1993 17:49:28   BRAD
//
//    Rev 1.3   04 Apr 1993 17:39:26   BRAD
// Removed all references to N30.*
//
//    Rev 1.2   26 Mar 1993 14:52:02   DALLEE
// Support for DOS.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "disk.h"
#include "ctsn.h"
#include "repairn.h"
#include "virscan.h"

#include "navex.h"
#include "avexprot.h"

#ifdef SYM_WIN32
  #include <winioctl.h>

  // KLUGE ALERT: PARTITION_INFORMATION structure is 28 bytes long, but
  // depending on structure packing changes to 32 bytes.  WinNT expects
  // 32 to be passed in as the size to DeviceIoControl().

  typedef union tagBYTE_PARTITION_INFORMATION_32BYTE {
    PARTITION_INFORMATION pi;
    BYTE abyFiller [32];
    } PARTITION_INFORMATION_32BYTE;

#endif

// ---------------------------------------------------------------------------
//                          LOCAL DEFINES
// ---------------------------------------------------------------------------

#define     SIZE_DOSBUFFER  (4096 + SYM_MAX_PATH +1)

#define     LOCK_DRIVE      0
#define     UNLOCK_DRIVE    1

#ifdef SYM_WIN
  #define USE_EXECUTABLE_BOOT_CODE     FALSE

#else   // DOS only!!
  #define USE_EXECUTABLE_BOOT_CODE     TRUE

#endif  // ifdef SYM_WIN

// ---------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

BOOL SYM_EXPORT WINAPI VirusScanLockUnlockDiskL (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

static VOID WINAPI GetPartitionStartLoc (LPVOID  lpPartitionBuffer,
                                         UINT    uEntry,
                                         LPWORD  lpwCylinder,
                                         LPBYTE  lpbSector,
                                         LPBYTE  lpbHead);

static VOID WINAPI GetPartitionEndLoc (LPVOID  lpPartitionBuffer,
                                       UINT    uEntry,
                                       LPWORD  lpwCylinder,
                                       LPBYTE  lpbSector,
                                       LPBYTE  lpbHead);

VOID WINAPI GetPartitionType (LPVOID    lpPartitionBuffer,
                              UINT      uEntry,
                              LPWORD    lpbType);

VOID WINAPI VirusScanMapLogToPhyParams (BYTE         dl,
                                        LPINT13REC   lpInt13Package);

static UINT WINAPI DetermineStartingSector (INT13REC FAR * lpstInt13Rec,
                                            LPBYTE         lpRead,
                                            LPWORD         lpwCyl,
                                            LPWORD         lpwHead,
                                            LPWORD         lpwSec);

LPVOID LOCAL AllocAlignedPtr (UINT          uFlags,
                              DWORD         dwBytes,
                              LPVOID FAR *  lpBasePtr,
                              DWORD         dwAlignment);

// ---------------------------------------------------------------------------

extern VIRSCAN ScanData;

MODULE_NAME;

//************************************************************************
// VirScanMasterBoot()
//
// Scans the master boot record.
//
// Parameters:
//     lpVirusName; pointer to a minimum 40 byte long buffer
//                  where the virus name is placed if infect
//     ulpStatus;   pointer to where the completion status of
//                  the function is placed.
//
// Returns:
//    returns 0 if master boot is not infected, otherwise
//    the virus ID number. The return is only valid if the
//    ulpStatus variable is 0.
//
//    ulpStatus; 0 = if everything whent fine otherwise
//               ERR_READMBOOT : problem reading mboot
//               ERR_DOSFREE : couldn't free conv. mem.
//               ERR_DOSALLOC : couldn't get conv. mem.
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanMasterBoot (
    UINT        uPhysDriveNum,
    LPN30NOTES  lpNotes,
    LPUINT      ulpStatus
    )
{
    auto    char        lpVirScan [ SYM_MAX_PATH+1 ] = { 0 };
    auto    HFILE       hFile;
    auto    LPN30       lpN30;
                                        // uVirusSigIndex is the number
                                        // of the boot sig in memory.
                                        // wVirID in the N30 structs is
                                        // the index to the VIRSCAN.INF file.
    auto    UINT        uVirusSigIndex = 0;
    auto    LPSTR       lpOrigBuffer;   // Original buffer (save for MemFree)
    auto    LPBYTE      lpRead;         // Pass to read function
    auto    LPBYTE      lpScan;         // Pass to scan function
    auto    ABSDISKREC  absDisk = { 0 };
//    auto    BOOL        bDiskLocked;
    auto    BYTE        byReadStatus;

// external variables

    auto    WORD            wVirusID, wVirusIndex;
    auto    EXTSTATUS       extResult;

// external variables

//////////////////////////////////////////////////////////////////////////
// The CanAccessDisk functionality will be taken care of when we lock the
// drive for read/write... -Carey 10/18/95
//////////////////////////////////////////////////////////////////////////
//    if ( VirusScanCanAccessDisk ( 'C', DISK_READ_ONLY ) == FALSE )
//        {
//        *ulpStatus = ERR_VIRSCAN_DISKLOCKED;
//        return (uVirusSigIndex);
//        }
//////////////////////////////////////////////////////////////////////////

    *ulpStatus = 0;

                                        // Win32 needs read buffer to reside
                                        // in a single 4K page for caching
                                        // Promise E4030+ controller.
                                        // Since we only read 512 bytes, we
                                        // only need to align on a 512 boundary.
    lpScan = (LPBYTE)AllocAlignedPtr(GHND,
                                    SIZE_DOSBUFFER,
                                    (LPVOID FAR *)&lpOrigBuffer,
                                    PHYSICAL_SECTOR_SIZE);

    if (NULL != lpScan)
        {
#if defined(SYM_DOS)
                                        // DOS scn4_boo expects VIRSCAN.DAT path
                                        // at head of this buffer...
        lpRead = lpScan + MAX_PATH_SIZE + 1; //lpRead = lpScan + SYM_MAX_PATH + 1;
        MEMCPY(lpScan, ScanData.szVirscanFile, SYM_MAX_PATH+1);
#else
        lpRead = lpScan;
#endif

        ///////////////////////////////////////////////////////////////////////
        // Below: absDisk.dn = <physical drive # passed in to function!!>
        ///////////////////////////////////////////////////////////////////////

        absDisk.dn = uPhysDriveNum;
        absDisk.head = 0;
        absDisk.track = 0;
        absDisk.sector = 1;
        absDisk.numSectors = 1;
        absDisk.buffer = lpRead;

        ///////////////////////////////////////////////////////////////////////
        // Lock the drive to do the read.  If we fail, EXIT.
        ///////////////////////////////////////////////////////////////////////

//        bDiskLocked = VirusScanLockUnlockDiskP ( LOCK_DRIVE,
//                                                 absDisk.dn,
//                                                 DISK_READ_ONLY );

//        if (FALSE == bDiskLocked)
//            {
//            MEMSET(ScanData.lpWorkMem, '\0', SIZE_WORK_BUFFER);
//            MEMSET (lpScan, EOS, SIZE_DOSBUFFER);   //clear buffer
//            MemFreePtr(lpOrigBuffer);

//            *ulpStatus = ERR_VIRSCAN_DISKLOCKED;
//            return(uVirusSigIndex);                 // uVirusSigIndex == 0
//            }

        byReadStatus = DiskAbsOperation(READ_COMMAND, &absDisk);

//        VirusScanLockUnlockDiskP ( UNLOCK_DRIVE,
//                                   absDisk.dn,
//                                   DISK_READ_ONLY );

        if ( byReadStatus == 0 )
            {

            //////////////////////////////////////////////////////////////////
            // No disk access is performed in SCN4_BOOT...  If any were,
            // locking would be done on a per-read basis for the time being
            //////////////////////////////////////////////////////////////////

            uVirusSigIndex = SCN4_BOOT (lpScan,
                                        (LPBYTE) ScanData.lpBootScanData,
                                        USE_EXECUTABLE_BOOT_CODE);

// external start

            if (0 == uVirusSigIndex)
                {
                //////////////////////////////////////////////////////////////
                // Drive locking is done on a per-read/write basis in the
                // callbacks...
                //////////////////////////////////////////////////////////////

                extResult = EXTScanPart((LPCALLBACKREV1)ScanData.lpCallbacks,
                                        absDisk.dn,
                                        lpRead,
                                        (LPBYTE)NULL,
                                        EXTERNAL_DETECT_VERSION,
                                        &wVirusID);

                if (EXTSTATUS_OK == extResult)
                    {
                    uVirusSigIndex = 0;
                    }
                else if (EXTSTATUS_VIRUS_FOUND == extResult)
                    {
                    if (EXTSTATUS_OK ==
                        DetermineVirusIndex(ScanData.szVirscanFile,
                                            wVirusID,
                                            &wVirusIndex,
                                            NULL))
                        uVirusSigIndex = wVirusIndex;
                    else
                        uVirusSigIndex = 0;
                    }
                else // other error
                    {
                        uVirusSigIndex = 0;
                    }

                }

// external end

            if (uVirusSigIndex)
                {

                // **************************************************** //
                // Now open Virus def file so we can get the virus name //
                // **************************************************** //
                VirusScanGetDatFile(lpVirScan, sizeof(lpVirScan) - 1);

                if ((hFile = FileOpen (lpVirScan, READ_ONLY_FILE)) != HFILE_ERROR)
                    {
                    SCN4_GET_NAME (hFile, (WORD)uVirusSigIndex,
                                   ScanData.lpWorkMem,
                                   ScanData.lpBootScanData,
                                   lpNotes->lpVirName);
                    FileClose (hFile);

                    lpN30 = (LPN30)ScanData.lpWorkMem;

                    lpNotes->wVirID   = lpN30->wVirID;
                    lpNotes->wType    = lpN30->wType;
                    lpNotes->wControl2= lpN30->wControl2;
                    lpNotes->wInfo    = lpN30->wInfo;
                    lpNotes->wSize    = lpN30->wInfSize;
                    }
                else
                    {
                    lpNotes->wVirID   = 0;
                    lpNotes->wType    = 0;
                    lpNotes->wControl2= 0;
                    lpNotes->wInfo    = 0;
                    lpNotes->wSize    = 0;
                    lpNotes->lpVirName[0] = EOS;
                    }
                }
            }
        else
            {
            *ulpStatus = ERR_VIRSCAN_READMBOOT;
            }


                                        // Clear the scan buffer of virus
                                        // traces so we do not false id
                                        // on a following mem scan.
        MEMSET(ScanData.lpWorkMem, '\0', SIZE_WORK_BUFFER);
        MEMSET (lpScan, EOS, SIZE_DOSBUFFER);   //clear buffer

                                        // Free the work buffer
        MemFreePtr(lpOrigBuffer);
        }
    else
        *ulpStatus = ERR_VIRSCAN_DOSALLOC;

    return (uVirusSigIndex);
} // End VirScanMasterBoot()


//************************************************************************
// VirScanBoot()
//
// Scans the specified boot record.
//
// Parameters:
//     lpVirusName; pointer to a minimum 40 byte long buffer
//                  where the virus name is placed if infect
//     ulpStatus;   pointer to where the completion status of
//                  the function is placed.
//     byDrvNum; Drive number to scan. A == 0, B == 1....
//
// Returns:
//    returns 0 if boot is not infected, otherwise
//    the virus ID number. The return is only valid if the
//    ulpStatus variable is 0.
//
//    ulpStatus; 0 = if everything is fine otherwise
//               ERR_READBOOT : problem reading boot
//               ERR_DOSFREE : couldn't free conv. mem.
//               ERR_DOSALLOC : couldn't get conv. mem.
//               ERR_REMOTEDRIVE : can't scan network drive
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanBoot (
    UINT        uDrive,
    LPN30NOTES  lpNotes,
    LPUINT      lpuStatus
    )
{
    auto    LPN30       lpN30;
    auto    char        szVirScan[SYM_MAX_PATH+1];
    auto    HFILE       hFile;
    auto    UINT        uVirusSigIndex = 0;
    auto    UINT        uErr = 0;
    auto    BOOL        bDiskLocked;
                                        // ----------------------------------
                                        // uVirusSigIndex is the number
                                        // of the boot sig in memory.
                                        // wVirID in the N30 structs is
                                        // the index to the VIRSCAN.INF file.
                                        // ----------------------------------
    auto    UINT        uDiskTypeMajor;
    auto    UINT        uDiskTypeMinor;
    auto    LPSTR       lpOrigBuffer;   // Keep unchanged for MemFreePtr().
    auto    LPBYTE      lpRead;         // Pass to read function
    auto    LPBYTE      lpScan;         // Pass to scan function
    auto    ABSDISKREC  absDisk = { 0 };
    auto    DISKREC     disk    = { 0 };
    auto    WORD        wCyl, wSec, wHead;
    auto    UINT        uTryCount;


                                        // Ensure that uDrive is in upper
                                        // case!!!
    if ( ('a' <= uDrive) && (uDrive <= 'z') )
        uDrive = CharToUpper((char)uDrive);

    *lpuStatus = 0;

    // 1. See if we can access the drive.

//////////////////////////////////////////////////////////////////////////
// The CanAccessDisk functionality will be taken care of when we lock the
// drive for read/write... -Carey 10/18/95
///////////////////////////////////////////////////////////////////////////////
//    if ( VirusScanCanAccessDisk ( (BYTE)uDrive, DISK_READ_ONLY ) == FALSE )
//        {
//        *lpuStatus = ERR_VIRSCAN_DISKLOCKED;
//        return ( 0 );
//        }
///////////////////////////////////////////////////////////////////////////////

    // 2. Determine whether or not the disk has a boot sector.

    DiskGetType((BYTE)uDrive, &uDiskTypeMajor, &uDiskTypeMinor);

    if (TYPE_REMOTE == uDiskTypeMajor)
        {
        *lpuStatus = ERR_VIRSCAN_REMOTEDRIVE;
        return ( 0 );
        }

    // 3. See if we can allocate our memory.

    // Win32 needs read buffer to reside in a single 4K page for caching
    // Promise E4030+ controller.
    // Since we only read 512 bytes, we only need to align on a 512 boundary.

    lpScan = (LPBYTE)AllocAlignedPtr(GHND,
                                    SIZE_DOSBUFFER,
                                    (LPVOID FAR *)&lpOrigBuffer,
                                    PHYSICAL_SECTOR_SIZE);

    if (NULL == lpScan)
        {
        *lpuStatus = ERR_VIRSCAN_DOSALLOC;
        return ( 0 );
        }

    // 4. Set up our status.

    VirusScanGetDatFile(szVirScan, sizeof(szVirScan) - 1);


#if defined(SYM_DOS)

    ///////////////////////////////////////////////////
    // DOS scn4_boo expects VIRSCAN.DAT path
    // at head of this buffer...
    ///////////////////////////////////////////////////

    lpRead = lpScan + MAX_PATH_SIZE  + 1; //lpScan + SYM_MAX_PATH + 1;
    MEMCPY(lpScan, ScanData.szVirscanFile, SYM_MAX_PATH+1);
#else
    lpRead = lpScan;
#endif

    // 5. It's OK to scan. Do so.

    INT13REC    stInt13Rec;

    ////////////////////////////////////////////////////////
    // locate the active partition and find out where its
    // boot sector is on the physical drive.  If there is a
    // problem, abort with ERR_VIRSCAN_READBOOT.  We pass
    // in sector #0 in order to specify the first sector of
    // the given logical drive.
    //
    // No locking need be performed for this function as it
    // translates to a WIN32 API call (says David Buches)
    ////////////////////////////////////////////////////////

    VirusScanMapLogToPhyParams (uDrive, &stInt13Rec);

    // check to see if we have a physical drive or not.

    if (!stInt13Rec.generated)
        {
                                     // Call DiskGetInfo() instead of aborting
        if (FALSE == DiskGetInfo(uDrive, &disk))
        {
            MemFreePtr(lpOrigBuffer);
            *lpuStatus = ERR_VIRSCAN_READBOOT; 
            return ( 0 );
        }
            
        disk.dl = (BYTE) uDrive;     // NavEx will not be used nor will 
        disk.sector = 0;             // viruses be repairable 
        disk.sectorCount = 1;
        disk.buffer = lpRead;
        uErr = DiskBlockDevice(BLOCK_READ, &disk);
        }
    else
        {
        bDiskLocked = VirusScanLockUnlockDiskP ( LOCK_DRIVE,
                                             stInt13Rec.dn,
                                             DISK_READ_ONLY );

        if (FALSE == bDiskLocked)
            {
            MEMSET (ScanData.lpWorkMem, '\0', SIZE_WORK_BUFFER);
            MEMSET (lpScan, '\0', SIZE_DOSBUFFER);
            MemFreePtr(lpOrigBuffer);

            *lpuStatus = ERR_VIRSCAN_DISKLOCKED;
            return ( 0 );
            }

    //////////////////////////////////////////////////////////////////////////
    // 5A. We got a sector from DiskMapLogToPhyParams; however, this may not
    //     be the first sector of the partition, as specified in the MBR of
    //     the drive.  It could be logical sector 0 of the specified partition,
    //     which might be many sectors from the start of the actual partition
    //     as specified in the partition table in the MBR.
    //     We must locate the actual start of the partition (from the MBR) as
    //     this is where the actual boot sector is located for the partition.
    ///////////////////////////////////////////////////////////////////////////

        if (FALSE == DetermineStartingSector(&stInt13Rec,
                                             lpRead,
                                             &wCyl,
                                             &wHead,
                                             &wSec))
            {
            VirusScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                       stInt13Rec.dn,
                                       DISK_READ_ONLY );

            MEMSET (ScanData.lpWorkMem, '\0', SIZE_WORK_BUFFER);
            MEMSET (lpScan, '\0', SIZE_DOSBUFFER);
            MemFreePtr(lpOrigBuffer);

            *lpuStatus = ERR_VIRSCAN_READBOOT;
            return ( 0 );
            }

    //////////////////////////////////////////////////////////////////////////
    // 6. Now read that boot sector!
    //
    // Note: The drive is still locked from before the DetermineStartingSector
    // function call...
    //////////////////////////////////////////////////////////////////////////

        uTryCount = 3;

        do
            {
            absDisk.dn          = stInt13Rec.dn;

#if defined(SYM_DOS) || defined(SYM_WIN32)
            DiskGetPhysicalInfo(&absDisk);

                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
            if (    (0 == absDisk.sectorsPerTrack)
                 || (0 == absDisk.totalTracks)
                 || (0 == absDisk.totalHeads) )
                {
                uErr = TRUE;
                break;
                }
#endif

            absDisk.head        = (BYTE)wHead;
            absDisk.track       = wCyl;
            absDisk.sector      = (BYTE)wSec;
            absDisk.numSectors  = 1;
            absDisk.buffer      = lpRead;

            uErr = DiskAbsOperation(READ_COMMAND, &absDisk);
            }
        while (uErr != 0 && uTryCount--);

    //////////////////////////////////////////////////////////////////////////
    // Now unlock the drive since we're done with all of our reading...
    //////////////////////////////////////////////////////////////////////////

        VirusScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                   stInt13Rec.dn,
                                   DISK_READ_ONLY );
        }
    /////////////////////////////////////////////////
    // If we had an error reading the boot sector,
    // quit out!
    /////////////////////////////////////////////////

    if (uErr)
        {
        // we may have read in the partition sector which also could have a
        // virus, so clear it out!

        MEMSET (ScanData.lpWorkMem, '\0', SIZE_WORK_BUFFER);
        MEMSET (lpScan, '\0', SIZE_DOSBUFFER);
        MemFreePtr(lpOrigBuffer);

        *lpuStatus = ERR_VIRSCAN_READBOOT;
        return ( 0 );
        }

    // 7. Scan using NAV 3.0X technology


    uVirusSigIndex = SCN4_BOOT(lpScan,
                               (LPBYTE) ScanData.lpBootScanData,
                               USE_EXECUTABLE_BOOT_CODE);

    // Was a virus found?  If not, try using EXTernal technology

// external start

    if (!uVirusSigIndex && stInt13Rec.generated)
        {
        EXTSTATUS   extResult;
        WORD        wVirusID;
        WORD        wVirusIndex;

        //////////////////////////////////////////////////////
        // No virus found using NAV 3 engine.  Try "fail-safe"
        // detections!
        //////////////////////////////////////////////////////

        extResult = EXTScanBoot((LPCALLBACKREV1)ScanData.lpCallbacks,
                                (BYTE)stInt13Rec.dn,
                                0xFF,           // don't use this from DOS/W32
                                (BYTE)wSec,
                                wCyl,
                                (BYTE)wHead,
                                lpRead,
                                (LPBYTE)NULL,
                                EXTERNAL_DETECT_VERSION,
                                &wVirusID);


        if (EXTSTATUS_OK == extResult)
            {
            uVirusSigIndex = 0;
            }
        else if (EXTSTATUS_VIRUS_FOUND == extResult)
            {
            if (EXTSTATUS_OK == DetermineVirusIndex(szVirScan,
                                                    wVirusID,
                                                    &wVirusIndex,
                                                    NULL))
                uVirusSigIndex = wVirusIndex;
            else
                uVirusSigIndex = 0;     // error
            }
        else
            uVirusSigIndex = 0;         // error
        }

// external end

    // 8. Now that we've scanned, see what we've cought in our net.


    if (uVirusSigIndex)
        {
        // **************************************************** //
        // Now open virus def file so we can get the virus name //
        // **************************************************** //

        if ((hFile = FileOpen(szVirScan, READ_ONLY_FILE)) != HFILE_ERROR)
            {
            SCN4_GET_NAME (hFile,
                           uVirusSigIndex,
                           ScanData.lpWorkMem,
                           ScanData.lpBootScanData,
                           lpNotes->lpVirName);
            FileClose(hFile);

            lpN30 = (LPN30)ScanData.lpWorkMem;

            lpNotes->wVirID   = lpN30->wVirID;
            lpNotes->wType    = lpN30->wType;
            lpNotes->wControl2= lpN30->wControl2;
            lpNotes->wInfo    = lpN30->wInfo;
            lpNotes->wSize    = lpN30->wInfSize;
            }
        else
            {
            lpNotes->lpVirName[0] = EOS;
            lpNotes->wVirID   = 0;
            lpNotes->wType    = 0;
            lpNotes->wControl2= 0;
            lpNotes->wInfo    = 0;
            lpNotes->wSize    = 0;
            }
        }


    // 9. Clear the scan buffer of virus traces so we do not false id
    //    on a following mem scan.

    MEMSET (ScanData.lpWorkMem, '\0', SIZE_WORK_BUFFER);
    MEMSET (lpScan, '\0', SIZE_DOSBUFFER);
    MemFreePtr(lpOrigBuffer);

    // 10. Return what we found!

    return ( uVirusSigIndex );
} // End VirScanBoot()


//************************************************************************
// VirusScanRepairBoot()
//
// This routine attempts the specified boot repair.
//
// Parameters:
//      BYTE    byDriveLetter           Drive to repair
//                                          Boot repair: 'A', 'B', 'C', ...
//                                          MBR repair: 0x80, 0x81, 0x82, ...
//      WORD    wVirID                  Virus ID number
//
// Returns:
//      TRUE                            Successful repair.
//      FALSE                           Failed.
//************************************************************************
// 5/12/93 DALLEE, Function created.
// 9/16/93 DALLEE, Re-did double infection logic.  Allocations and cleanup
//          untouched.  Repair, verification, and generic repair rewritten.
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanRepairBoot (
    BYTE    byDriveLetter,
    WORD    wVirID,
    UINT    uRepairType
    )
{
    #define SIZE_BOOTREPAIR_BUFFER (4096 + sN30 + SYM_MAX_PATH +1)

    extern  VIRSCAN ScanData;

    auto    char        szBootFile [SYM_MAX_PATH + 1];
    auto    BYTE        byDrive;
    auto    BOOL        bStatus = FALSE;
    auto    N30NOTES    N30Notes;
    auto    UINT        uRepairStatus;
    auto    UINT        i;
    auto    WORD        wOrigVirID;
    auto    BOOL        bTryRepair;
    auto    LPSTR       lpOrigMem;      // Save original for MemFreePtr()
    auto    LPSTR       lpMem;
    auto    BOOL        bDiskLocked;

    auto    INT13REC    stInt13Rec;
    auto    WORD        wHead, wCyl, wSec;

// external start

    auto    HFILE       hFile;
    auto    N30         stN30;
    auto    DISKREC     disk    = { 0 };
    auto    EXTSTATUS   extResult;

// external end

                                        // Ensure that byDriveLetter is in
                                        // upper case!!!
    if ( ('a' <= byDriveLetter) && (byDriveLetter <= 'z') )
        byDriveLetter = CharToUpper ((char)byDriveLetter);

///////////////////////////////////////////////////////////////////////////////
// The CanAccessDisk functionality will be taken care of when we lock the
// drive for read/write... -Carey 10/18/95
///////////////////////////////////////////////////////////////////////////////
//
//    if ( VirusScanCanAccessDisk ( byDriveLetter, DISK_READ_WRITE ) == FALSE )
//        {
//        // ERR_VIRSCAN_DISKLOCKED;
//        return ( FALSE );
//        }
///////////////////////////////////////////////////////////////////////////////

                                        // Get the full path to the Generic
                                        // Boot Repair file.
    VirusScanGetBootFile( szBootFile, SYM_MAX_PATH );

    lpMem = (LPSTR)AllocAlignedPtr(GHND,
                                   SIZE_BOOTREPAIR_BUFFER,
                                   (LPVOID FAR *)&lpOrigMem,
                                   PHYSICAL_SECTOR_SIZE);

    if ( lpMem )
        {
        // next determine the location of the boot sector, if not MBR repair.
        // Do this only once outside repair loop...

        if (uRepairType == BOOT_REPAIR)
            {
            VirusScanMapLogToPhyParams (byDriveLetter, &stInt13Rec);

            if (!stInt13Rec.generated)
                {
                MEMSET(lpMem, EOS, SIZE_BOOTREPAIR_BUFFER);
                MemFreePtr(lpOrigMem);
                return(FALSE);
                }

            byDrive = stInt13Rec.dn;

            bDiskLocked = VirusScanLockUnlockDiskP ( LOCK_DRIVE,
                                                     byDrive,
                                                     DISK_READ_ONLY );

            ///////////////////////////////////////////////////////////////
            // If we can't lock the drive, fail out...
            ///////////////////////////////////////////////////////////////

            if (FALSE == bDiskLocked)
                {
                MEMSET(lpMem, EOS, SIZE_BOOTREPAIR_BUFFER);
                MemFreePtr(lpOrigMem);

                return(FALSE);
                }

            if (FALSE == DetermineStartingSector(&stInt13Rec,
                                                 (LPBYTE)lpMem,
                                                 &wCyl,
                                                 &wHead,
                                                 &wSec))
                {
                wHead = 0xFFFF;
                wCyl = 0xFFFF;
                wSec = 0xFFFF;
                }

            VirusScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                       byDrive,
                                       DISK_READ_ONLY );

            }

                                        //--------------------------------
                                        // Do 2 attempts at repair if repair
                                        // returns success and the boot record
                                        // is still infected.
                                        // If we flip-flopped back to the
                                        // original virus, try generic repair.
                                        //--------------------------------
        wOrigVirID = wVirID;
        for (i = 0, bTryRepair = TRUE; (i < 2) && (bTryRepair == TRUE); i++)
            {

// external start

            // first obtain the N30 entry for the virus we're trying to repair

            if ((hFile = FileOpen (ScanData.szVirscanFile,
                                   READ_ONLY_FILE)) != HFILE_ERROR)
                {
                SCN4_GET_ENTRY (hFile, wVirID, (char *)&stN30);
                FileClose (hFile);
                }

            if (uRepairType == BOOT_REPAIR)
                {
                extResult = EXTRepairBoot((LPCALLBACKREV1)ScanData.lpCallbacks,
                                          EXTERNAL_DETECT_VERSION,
                                          &stN30,
                                          byDrive,
                                          (BYTE)0xFF,    // no boot sec #
                                          (BYTE)wSec,
                                          (WORD)wCyl,
                                          (BYTE)wHead,
                                          (LPBYTE)lpMem);
                }
            else
                {
                byDrive = byDriveLetter;

                extResult = EXTRepairPart((LPCALLBACKREV1)ScanData.lpCallbacks,
                                          EXTERNAL_DETECT_VERSION,
                                          &stN30,
                                          byDrive,        // NAV does nothing else!
                                          (LPBYTE)lpMem);
                }

// external end
            if (extResult != EXTSTATUS_OK)
                {
                bStatus = REP4_BOOT (wVirID, ScanData.szVirscanFile,
                                     lpMem, SIZE_BOOTREPAIR_BUFFER,
                                     byDrive );
                }
            else
                bStatus = TRUE;

            if (bStatus == TRUE)
                {
                switch(uRepairType)
                    {
                    case MBR_REPAIR:
                        wVirID = VirusScanMasterBoot(byDrive, &N30Notes, &uRepairStatus);
                        break;

                    case BOOT_REPAIR:
                        wVirID = VirusScanBoot(byDriveLetter,
                                               &N30Notes,
                                               &uRepairStatus);
                        break;
                    }

                if ((NOERR == uRepairStatus) && wVirID)
                    {
                    bStatus = FALSE;    // We're still infected.
                    }
                else
                    {
                    bTryRepair = FALSE; // Boot record is clean.
                    }
                }
            else
                {
                bTryRepair = FALSE;     // Repair failed, don't try generic.
                }
            }


        if ( bTryRepair &&
             ((wOrigVirID == wVirID) || !(NO_GENERIC & N30Notes.wControl2)) )
            {
            switch (uRepairType)
                {
                case MBR_REPAIR:
                    bStatus = REP4_MBR_GENERIC(lpMem,
                                               byDrive,
                                               REP_MBR_GENERIC);
                    break;

                case BOOT_REPAIR:
                    if(byDrive < FIRST_HD)
                        {
                        BYTE byType = DIS4_ANALYZE_FLOP(byDrive,
                                                        lpMem);

                        bStatus = REP4_BOOT_GENERIC( lpMem,
                                                     byDrive,
                                                     byType );
                        }
                    break;
                }
            }
        }
                                        // --------------------------------
                                        // Clear buffer so memscan should
                                        // not halt the system
                                        // --------------------------------
    MEMSET ( lpMem , EOS, SIZE_BOOTREPAIR_BUFFER );

    MemFreePtr ( lpOrigMem );

    return ( bStatus );
} // VirusScanRepairBoot()


/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI VirusScanCanAccessDisk(// Check if disk can be accessed
    BYTE    byDriveLetter,              // [in] drive letter to verify
    UINT    uRequestedAccess            // [in] requested access right
    );

@Description:
This function is used to query if a drive has been locked by an other disk
utility. If it has we give a specific return code to the application so it
can squack correctly about the situation.

Note: this function does very little in SYM_DOS platform.

Note: This function will always succeeds if DISK_READ_ONLY is requested.
      The reason for this is that we still want to attempt to read/scan
      the MBR and the Boot sector whether they're locked or not. Normally,
      the disk is not locked for read.

10/18/95 CNACHEN
Update: Apparently, we've been crashing on certain HD controlers because we are
        failing to lock the drive before reads.  Therefore, we will attempt to
        lock before all DISK operations...

@Parameters:
$byDriveLetter$ Should be a drive letter from 'A' to 'Z'
$uRequestedAccess$ The possible settings can be:
        DISK_READ_WRITE If the code should check for write access
        DISK_READ_ONLY  If the code should check for read only access

        This has not been plugged in yet because I have to find the correct
        information on what the different lock levels indicate.

@Returns:
    TRUE    if the drive can be accessed for the requested access
    FALSE   if somebody has a lock on the drive for the requested access.

@Include: virscan.h

*******************************************************************************/
#if defined(SYM_DOS)

BOOL SYM_EXPORT WINAPI VirusScanCanAccessDisk (
    BYTE    byDriveLetter,
    UINT    uRequestedAccess )
{
    return (TRUE);
}

#else

BOOL SYM_EXPORT WINAPI VirusScanCanAccessDisk ( // Check if disk can be accessed
    BYTE    byDriveLetter,              // [in] drive letter to verify
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    const   WORD    wNOT_LOCKED =   (WORD)-1;
    const   WORD    wLOCKLEVEL0 =   0;

    auto    BOOL    bCanAccess  =   FALSE;
    auto    WORD    wLockLevel  =   0;
    auto    WORD    wLockPerm   =   0;
    auto    UINT    uStatus     =   0;
    const   BYTE    byDriveNum  =   byDriveLetter - 'A' + 1;

    if ( byDriveLetter >= 'A' && byDriveLetter <= 'Z' )
        {
        if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
            return(TRUE);

        switch ( uRequestedAccess )
            {
            case DISK_READ_WRITE:

                uStatus = DiskGetCurrentLockState (
                                                byDriveNum,
                                                &wLockLevel,
                                                &wLockPerm
                                                );
                if (!uStatus)
                    {
                    if ( wLockLevel == wNOT_LOCKED ||   // Not locked
                         wLockLevel == wLOCKLEVEL0 )    // Assume the same thing (?)
                        {
                        bCanAccess = TRUE;
                        }
                    }
                break;

            case DISK_READ_ONLY:
            default:
                {
                bCanAccess = TRUE;
                }
            }
        }
    else
        {
        bCanAccess = TRUE;
        }

    return ( bCanAccess );
}
#endif


static VOID WINAPI GetPartitionStartLoc (
    LPVOID  lpPartitionBuffer,
    UINT    uEntry,
    LPWORD  lpwCylinder,
    LPBYTE  lpbSector,
    LPBYTE  lpbHead)
{
    if (HWIsNEC())
    {
        LPNECPARTENTRYREC  lpNecPartEntry;

        lpNecPartEntry = (LPNECPARTENTRYREC) ((LPSTR)lpPartitionBuffer +
                                              sizeof (NECPartEntryRec) * uEntry);

        *lpwCylinder = lpNecPartEntry->startCylinder;
        *lpbHead     = 0;
        *lpbSector   = 1;
    }
    else
    {
        LPPARTENTRYREC      lpLocalPartEntry;
        WORD                wTrack;
        BYTE                bySector;

        lpLocalPartEntry = (LPPARTENTRYREC)
                              ((LPSTR)lpPartitionBuffer + PART_TABLE_OFFSET) +
                                uEntry;

                                            /* Unscramble 10-bit cylinder   */

        DiskUnscrambleBIOSCoding (*((LPWORD) &(lpLocalPartEntry->startSector)), &wTrack, &bySector);

        *lpbHead        = lpLocalPartEntry->startHead;
        *lpwCylinder    = wTrack;
        *lpbSector      = bySector;
    }
}


// 12/12/96 JBRENNA: WARNING: NEC's do not have the ending head and sector
// information in the partition table (at least not that we currently know
// about). As such, on NEC's, lpbHead is always set to 0 and lpbSector is
// always set to 1.
static VOID WINAPI GetPartitionEndLoc (
    LPVOID  lpPartitionBuffer,
    UINT    uEntry,
    LPWORD  lpwCylinder,
    LPBYTE  lpbSector,
    LPBYTE  lpbHead)
{
    if (HWIsNEC())
    {
        LPNECPARTENTRYREC  lpNecPartEntry;

        lpNecPartEntry = (LPNECPARTENTRYREC) ((LPSTR)lpPartitionBuffer +
                                              sizeof (NECPartEntryRec) * uEntry);

        *lpwCylinder = lpNecPartEntry->endCylinder;
        *lpbHead     = 0;
        *lpbSector   = 1;
    }
    else
    {
        LPPARTENTRYREC      lpLocalPartEntry;
        WORD                wTrack;
        BYTE                bySector;

        lpLocalPartEntry = (LPPARTENTRYREC)
                              ((LPSTR)lpPartitionBuffer + PART_TABLE_OFFSET) +
                                uEntry;

                                            /* Unscramble 10-bit cylinder   */

        DiskUnscrambleBIOSCoding (*((LPWORD) &(lpLocalPartEntry->endSector)), &wTrack, &bySector);

        *lpbHead        = lpLocalPartEntry->endHead;
        *lpwCylinder    = wTrack;
        *lpbSector      = bySector;
    }
}


VOID WINAPI GetPartitionType (LPVOID    lpPartitionBuffer,
                              UINT      uEntry,
                              LPBYTE    lpbType)
{
    LPPARTENTRYREC      lpLocalPartEntry;

    SYM_ASSERT (!HWIsNEC());            // never call this func on NEC machines.

    lpLocalPartEntry = (LPPARTENTRYREC)
                          ((LPSTR)lpPartitionBuffer + PART_TABLE_OFFSET) +
                            uEntry;

    *lpbType = lpLocalPartEntry->system;
} // GetPartitionType()


//************************************************************************
// VirusScanMapLogToPhyParams()
//
// This routine covers the SYMKRNL function DiskMapLogToPhyParams called with
// sector 0.
// It is used by VirusScanBoot() and VirusScanRepairBoot() to translate
// logical sector 0 on a given drive to it's physical coordinates.
// This cover handles the NT platform, which QAK8 does not.
//
// For virscan's puposes we're most concerned with the active partition
// on the first physical drive when scanning for viruses. So...
//
// *KLUGE ALERT* When getting a request for drive C: sector 0, this routine
// will always return the first sector of the active partition on the
// first physical drive.
//
// Parameters:
//      BYTE        dl                  Drive letter (A, B, C, ...)
//      LPINT13REC  lpInt13Package      [out] Receives physical coordinates
//                                      and other info a la DiskMapLogToPhyParams().
// Returns:
//      Nothing.  See lpInt13Package.
//************************************************************************
// 10/26/95 DALLEE, created.
//************************************************************************

VOID WINAPI VirusScanMapLogToPhyParams (BYTE         dl,
                                        LPINT13REC   lpInt13Package)
{
                                        // Set error condition.
    lpInt13Package->generated = FALSE;

                                        // Special case C: to always scan
                                        // the active partition on the first
                                        // physical drive. Special case does
                                        // not work for NEC, DIS4_LOC_BOOTSECTOR
                                        // fails.
    if ('C' == dl && !HWIsNEC())
        {
        auto    DWORD   dwBootLocation; // [CYL]:[Head,Sector]
        auto    LPSTR   lpWorkBuffer;

        lpWorkBuffer = (LPSTR)MemAllocPtr(GPTR, 1024);
        if (NULL != lpWorkBuffer)
            {
            dwBootLocation = DIS4_LOC_BOOTSECTOR(FIRST_HD, lpWorkBuffer);
            if ((DWORD)-1 != dwBootLocation)
                {
                lpInt13Package->generated = TRUE;
                lpInt13Package->track     = HIWORD(dwBootLocation);
                lpInt13Package->head      = HIBYTE(LOWORD(dwBootLocation));
                lpInt13Package->sector    = LOBYTE(LOWORD(dwBootLocation));
                lpInt13Package->dn        = FIRST_HD;
                }
            MemFreePtr(lpWorkBuffer);
            }
        }
#ifdef SYM_WIN32
                                        // QAK8 DMLTPP() doesn't work for NT,
                                        // use NT specific calls.
    else if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
        {
        static  char    szHard [] = "\\Device\\Harddisk";
        static  char    szFloppy [] = "\\Device\\Floppy";

        auto    char    szDrive [3] = "?:";
        auto    HANDLE  hDrive;
        auto    HANDLE  hPhysDrive;
        auto    char    szDevice [SYM_MAX_PATH] = "\\\\.\\";
        auto    char    szPhysDrive [] = "\\\\.\\PHYSICALDRIVEx";
        auto    DWORD   dwDrive;
        auto    DWORD   dwSectorOffset;
        auto    DWORD   dwTrackOffset;

        auto    PARTITION_INFORMATION_32BYTE    rPartInfo;
        auto    DISK_GEOMETRY                   rDiskGeometry;
        auto    DWORD                           dwBytesReturned;

        szDrive[0] = dl;
        STRCAT(szDevice, szDrive);

                                        // First open handle to drive.
        hDrive = CreateFile(szDevice,
                            GENERIC_READ,
                            FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

        if (INVALID_HANDLE_VALUE != hDrive)
            {
                                        // Get NT's device name to find
                                        // physical drive number.
                                        // "\\Device\\Harddisk0\\Partition1" or
                                        // "\\Device\\Floppy0"
                                        // Case is not consistant, thus
                                        // STRNICMP().
            if (0 != QueryDosDevice(szDrive, szDevice, sizeof(szDevice)))
                {
                                        // If floppy drive
                if (0 == STRNICMP(szDevice, szFloppy, STRLEN(szFloppy)))
                    {
                    ConvertStringToLong(&dwDrive, szDevice + STRLEN(szFloppy));

                    lpInt13Package->generated = TRUE;
                    lpInt13Package->track     = 0;
                    lpInt13Package->head      = 0;
                    lpInt13Package->sector    = 1;
                    lpInt13Package->dn        = (BYTE)dwDrive;
                    }
                                        // Hard drive, we need to find
                                        // offset to this partition.
                else if ( (0 == STRNICMP(szDevice, szHard, STRLEN(szHard))))
                    {
                                        // NT supports volume sets and fault
                                        // tolerant drives that span multiple
                                        // physical disks. It no longer makes
                                        // sense to ask for the dimensions of
                                        // a logical drive. QueryDosDevice
                                        // returns the first physical drive
                                        // used by these volume sets. We must
                                        // use a handle to this physical drive
                                        // to get a drive geometry appropriate
                                        // for finding sectors relative to the
                                        // beginning of the drive.

                                        // Make '\\.\PHYSICALDRIVEx' string
                                        // using results from a previous call
                                        // to QueryDosDevice.
                    szPhysDrive[ STRLEN(szPhysDrive) - 1 ] =
                                                szDevice[ STRLEN(szHard) ];

                                        // Get a handle to the physical drive.
                    hPhysDrive = CreateFile(szPhysDrive,
                                            GENERIC_READ,
                                            FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL );

                    if ( ( INVALID_HANDLE_VALUE != hPhysDrive ) &&
                         ( TRUE == DeviceIoControl(hDrive,
                                     IOCTL_DISK_GET_PARTITION_INFO,
                                     NULL, 0,
                                     &rPartInfo, sizeof(rPartInfo),
                                     &dwBytesReturned, NULL)) &&
                         ( TRUE == DeviceIoControl(hPhysDrive,
                                     IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                     NULL, 0,
                                     &rDiskGeometry, sizeof(rDiskGeometry),
                                     &dwBytesReturned, NULL)) )
                        {
                        CloseHandle ( hPhysDrive );

                                        // Get drive number.
                        ConvertStringToLong(&dwDrive, szDevice + STRLEN(szHard));

                        dwSectorOffset = rPartInfo.pi.StartingOffset.LowPart
                                         / rDiskGeometry.BytesPerSector;

                        dwTrackOffset = dwSectorOffset
                                        / rDiskGeometry.SectorsPerTrack;

                                        // Compute location of sector 0.
                        lpInt13Package->generated = TRUE;

                        lpInt13Package->track =
                            (WORD)(dwTrackOffset / rDiskGeometry.TracksPerCylinder);

                        lpInt13Package->head =
                            (BYTE)(dwTrackOffset % rDiskGeometry.TracksPerCylinder);

                        lpInt13Package->sector =
                            1 + (BYTE)(dwSectorOffset % rDiskGeometry.SectorsPerTrack);

                        lpInt13Package->dn = (BYTE)(dwDrive + FIRST_HD);
                        }
                    }
                }
            CloseHandle(hDrive);
            }
        }
#endif  // SYM_WIN32
                                        // Use QAK call for Win95 and DOS
    else
        {
        DiskMapLogToPhyParams(0, dl, lpInt13Package);
        }
} // VirusScanMapLogToPhyParams()


//************************************************************************
// DetermineStartingSector()
//
// Given CYL, SEC, HEAD, determines which of the 4 top-level partitions this is
// in and returns the starting CYL, SEC, HEAD of that partition where the MBR
// would load the boot sector during bootup and also the type of partition.
// If the drive specified in lpstInt13Rec is an extended partition, then the
// default starting values from lpstInt13Rec are returned.
//
// Parameters:
//
//  lpstInt13Rec: contains proper info for the drive (IN)
//  lpRead: temp buffer to hold MBR
//  lpwCyl, lpwHead, lpwSec: filled in with proper info (OUT)
//
// Returns:
//      TRUE on success
//      FALSE on failure
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

static UINT WINAPI DetermineStartingSector(INT13REC FAR *  lpstInt13Rec,
                                           LPBYTE          lpRead,
                                           LPWORD          lpwCyl,
                                           LPWORD          lpwHead,
                                           LPWORD          lpwSec)
{
    ABSDISKREC      absDisk = { 0 };
    WORD            wCount = 3;
    DWORD           dwSectorInQuestion, dwPartStart, dwPartEnd;
    WORD            wStartCyl, wEndCyl;
    BYTE            byStartSec, byEndSec, byStartHead, byEndHead;
    UINT            uErr, i;


    if (lpstInt13Rec->dn < 0x80)        // not dealing with a hard drive
        {
        *lpwHead    = 0;                // 0,0,1 = boot record on floppies
        *lpwCyl     = 0;
        *lpwSec     = 1;

        return(TRUE);
        }

    // 1. Load the Partition Table from the specified physical hard drive

    absDisk.dn          = lpstInt13Rec->dn;
    absDisk.head        = 0;
    absDisk.track       = 0;
    absDisk.sector      = HWIsNEC()? 2 : 1;     // Partition table on NEC machines is at 2
    absDisk.numSectors  = 1;
    absDisk.buffer      = (LPBYTE)lpRead;

    do
        {
        uErr = DiskAbsOperation(READ_COMMAND, &absDisk);
        }
    while (wCount-- && uErr);

    if (uErr)
        return(FALSE);

    // 2. Go through each of the top-level partitions to determine which one
    //    (if any) contains the specified sector.  Then return the starting
    //    cyl,sec,head of that partition (as specified in the MBR).

    dwSectorInQuestion = ((DWORD)lpstInt13Rec->track << 16) +
                         ((DWORD)lpstInt13Rec->head << 8)   +
                         (DWORD)lpstInt13Rec->sector;

    for (i=0;i<4;i++)
        {
        GetPartitionStartLoc(lpRead,i,&wStartCyl,&byStartSec,&byStartHead);
        GetPartitionEndLoc  (lpRead,i,&wEndCyl,  &byEndSec,  &byEndHead);

        dwPartStart = ((DWORD)wStartCyl << 16) +
                      ((DWORD)byStartHead << 8) +
                       (DWORD)byStartSec;

        dwPartEnd = ((DWORD)wEndCyl << 16) +
                    ((DWORD)byEndHead << 8) +
                     (DWORD)byEndSec;

        if (dwSectorInQuestion >= dwPartStart &&
            dwSectorInQuestion <= dwPartEnd)
            {
            *lpwHead = (WORD)byStartHead;
            *lpwCyl  = wStartCyl;
            *lpwSec  = (WORD)byStartSec;

            return(TRUE);
            }
        }

    // this is probably an extended partition.  use our initial guess.

    *lpwHead    = lpstInt13Rec->head;
    *lpwCyl     = lpstInt13Rec->track;
    *lpwSec     = lpstInt13Rec->sector;

    return(TRUE);
}


//************************************************************************
// AllocAlignedPtr()
//
// This function is used to allocate a buffer correctly aligned for
// DiskAbsOperation().
//
// For W32 platforms, physical reads must not straddle two 4K memory pages -
// reads with Promise 4030+ drivers will silently fail.  Pass the size of
// the largest single read expected in dwAlignment.
// The function will allocate dwBytes+dwAlignment bytes of memory and
// return a pointer to the first dwAlignment-byte aligned mem in the block.
//
// For other platforms, alignment is irrelevent and this function will
// allocate only dwBytes and return a pointer to the beginning of the block
//
// Parameters:
//      UINT    uFlags                  As for MemAllocPtr()
//      DWORD   dwBytes                 Number of usable bytes to allocate.
//      LPVOID FAR *lpBasePtr           [out] Base address (use for MemFreePtr)
//      DWORD   dwAlignment             Desired byte alignment.
//
// Returns:
//      On success, a non-NULL dwAlignment-byte aligned pointer and lpBasePtr
//          hold the address of the non-aligned (actual) memory allocation.
//      Otherwise, NULL and lpBasePtr is also set to NULL.
//************************************************************************
// 12/15/95 DALLEE, function created.
//************************************************************************

LPVOID LOCAL AllocAlignedPtr (UINT          uFlags,
                              DWORD         dwBytes,
                              LPVOID FAR *  lpBasePtr,
                              DWORD         dwAlignment)
{
#ifdef SYM_WIN32

    auto    LPVOID  lpAlignedPtr = NULL;

    *lpBasePtr = MemAllocPtr(uFlags, dwBytes + dwAlignment);

    if (NULL != *lpBasePtr)
        {
        lpAlignedPtr = (LPVOID)((DWORD)*lpBasePtr + dwAlignment
                                - ((DWORD)*lpBasePtr % dwAlignment));
        }

    return (lpAlignedPtr);

#else

    return (*lpBasePtr = MemAllocPtr(uFlags, dwBytes));

#endif
} // AllocAlignedPtr()


//************************************************************************
// VirusScanMapLogicalToPhysical
//
// This routine converts a logical drive letter to a physical drive
// number
//
// Parameters:
//      BYTE byDriveLetter,             // [in]  Drive letter to convert
//
// Returns:
//      Physical drive number
//************************************************************************
// 05/22/96 Created by Jim Worden
//************************************************************************

BYTE SYM_EXPORT WINAPI VirusScanMapLogicalToPhysical (
    BYTE byDriveLetter                  // [in]  Drive letter to convert
)
{
    INT13REC    stInt13Rec;

    VirusScanMapLogToPhyParams (byDriveLetter, &stInt13Rec);

                                        // Make sure there is a physical drive

    if (!stInt13Rec.generated)
        return (0xff);

    return (stInt13Rec.dn);
}
