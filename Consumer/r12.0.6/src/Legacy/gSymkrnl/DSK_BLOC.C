/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/DSK_BLOC.C_v   1.5   10 Aug 1997 15:21:14   bsobel  $
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 * Copyright 1992-1993 Symantec Corporation                             *
 *                                                                      *
 ************************************************************************/

// $Log:   S:/SYMKRNL/VCS/DSK_BLOC.C_v  $
// 
//    Rev 1.5   10 Aug 1997 15:21:14   bsobel
// Changed to dynamically load thunk dll on win32 only when accessed for the first time.
// 
//    Rev 1.4   19 Mar 1997 21:37:30   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.3   26 Sep 1996 12:13:54   RCHINTA
// Merging changes from Quake C (NEC: error reporting in
// Win_DiskBlockDevice()).
// 
//    Rev 1.2   29 Aug 1996 18:13:42   DHERTEL
// Merge changed from Quake E:
// Change to DiskBlockDevice() for Safe mode.  We no longer make the thunked
// call in safe mode.
// 
//    Rev 1.1   08 Feb 1996 13:07:14   DBUCHES
// Fixed DiskBlockDevice() DX/Win16.  We need to make sure we normalize on selector boundries!
//
//    Rev 1.0   26 Jan 1996 20:21:52   JREARDON
// Initial revision.
//
//    Rev 1.46   15 Dec 1995 14:07:02   DBUCHES
// Changes for 32Bit FAT support.
//
//    Rev 1.45   07 Dec 1995 14:54:48   DBUCHES
// Fixed WIN platform compile errors.
//
//    Rev 1.44   07 Dec 1995 13:24:08   DBUCHES
// More 32 bit fat work.
//
//    Rev 1.43   07 Dec 1995 12:02:54   DBUCHES
// Fixed dx compiler warning.
//
//    Rev 1.42   04 Dec 1995 16:00:06   MARKK
// Work for 32 bit FAT
//
//    Rev 1.41   20 Jun 1995 18:17:24   RGLENN
// Renamed local define UNKNOWN_ERROR to DISK_BLOCK_ALLOC_ERROR and moved it to disk.h
//
//    Rev 1.40   13 Mar 1995 15:48:06   HENRI
// Force DiskBlockDevice to thunk when in fail safe mode
//
//    Rev 1.39   30 Jan 1995 13:23:28   MARKK
// Activated DiskGetBlockDeviceError under win32
//
//    Rev 1.38   09 Dec 1994 16:47:46   BILL
// Updated SystemIsNT
//
//    Rev 1.37   22 Nov 1994 16:44:28   BRAD
// Added INT 25/26 support for WinNT
//
//    Rev 1.36   26 Oct 1994 20:35:42   BILL
// Added include needed for disk.h
//
//    Rev 1.35   25 Oct 1994 19:34:12   HENRI
// Now using the common IOCtlVWin32 for all WIN32 IOCTL functions
//
//    Rev 1.34   23 Jun 1994 17:21:30   HENRI
// Moved 16 bit thunk layers to a separate DLL
//
//    Rev 1.33   22 Jun 1994 12:25:32   HENRI
// Added thunking layers
//
//    Rev 1.32   01 Jun 1994 11:16:08   MARKK
// Removed old ifdef
//
//    Rev 1.31   31 May 1994 09:20:06   MARKK
// Make DOSX use DPMI
//
//    Rev 1.30   13 May 1994 14:25:02   MARKK
// DOSX can only handle 32K at a time
//
//    Rev 1.29   04 May 1994 05:51:18   MARKK
// DOSX work
//
//    Rev 1.28   25 Apr 1994 17:44:56   BRAD
// Added DiskBlockDevice() and DiskGetInfo() to WIN32s thunk
//
//    Rev 1.27   14 Apr 1994 19:51:00   BRAD
// Move CriticalSectionBegin/End to IntWin, rather than all over the place
//
//    Rev 1.26   15 Mar 1994 12:33:04   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.25   25 Feb 1994 15:03:50   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.23   15 Feb 1994 09:07:06   MARKK
// Fixed bug in WFW where int26 would return with flags off the stack!
//
//
//    Rev 1.22   09 Feb 1994 20:32:46   MARKK
// Fixed selector problem under windows with dos diskblockdevice
//
//    Rev 1.21   09 Feb 1994 14:54:10   HENRI
// Made bUseOldWinCode a global variable (renamed to gbUseOldWinCode)
//
//    Rev 1.20   07 Feb 1994 17:42:06   MARKK
// Ram disk 1 sector fix under DOS
//
//    Rev 1.19   06 Feb 1994 12:09:26   HENRI
// Added DiskBlockUseOldWinCode as a public function for both DOS & Windows.
//
//    Rev 1.18   04 Feb 1994 15:57:56   MARKK
// And so begins the great experiment.  If your SYMCFG.INI file does not
// the following:
//
// [SYMKRNL]
// USEDOSALLOC=1
//
// then we will use the DOS version of DiskBlockDevice and let the Windows
// translation services handle the rest.
//
// If you do have the above lines then we will continue to allocate DOS
// memory ourselves and break up the request.
//
// Any problems caused by this change must be pointed out IMMEDIATELY!
//
//    Rev 1.17   03 Feb 1994 14:11:46   MARKK
// Fixed sharable dos memory bug
//
//    Rev 1.16   24 Jan 1994 16:56:54   MARKK
// Changed AllocateBuffer stuff to handle multiple clients
//
//    Rev 1.15   10 Nov 1993 18:33:32   JOHN
// Ported fixes from QAK2 branch.
//
//    Rev 1.14.2.2   10 Nov 1993 17:36:22   JOHN
// Added code to normalize huge buffer pointer in DiskBlockDevice().
//
//    Rev 1.14.2.1   03 Nov 1993 17:45:14   JOHN
// Added CriticalSectionBegin/End calls in _DiskBlockDevice() around the
// IntWin() call.  The critical section is only used when doing a logical
// to physical mapping, not for normal disk I/O's.
//
//    Rev 1.14.2.0   19 Oct 1993 09:26:50   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.14   28 Sep 1993 15:10:52   JOHN
// Fixed #ifdef so code works on DOS platform.
//
//    Rev 1.13   28 Sep 1993 10:07:10   MARKK
// Added two new routines
//
//    Rev 1.12   13 Aug 1993 11:55:16   MARKK
// Changed the detection of RAM drive in DiskBlock to use the
// new field wMajorType instead of number of FATs.
//
//
//    Rev 1.11   02 Jul 1993 08:51:52   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.10   15 Feb 1993 21:19:04   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.9   12 Feb 1993 04:27:42   ENRIQUE
// No change.
//
//    Rev 1.8   12 Jan 1993 14:18:20   CRAIG
// Scorpio update: Ability to redirect DiskBlockDevice()'s call
// to _DiskBlockDevice().  Added DiskBlockSetProc() and
// DiskBlockClearProc().  This all allows programs to create
// "virtual" disk drives 0: through 9:, supplying their own
// low-level sector read/write function.
//
//    Rev 1.7   03 Dec 1992 08:00:48   MARKK
// Fixed normalize address error under dos
//
//    Rev 1.6   04 Nov 1992 12:48:08   MARKK
// Nuked
//
//    Rev 1.5   19 Oct 1992 17:01:52   BILL
// No change.
//
//    Rev 1.4   02 Oct 1992 13:58:16   MARKK
// Fixed buffer address bug
//
//    Rev 1.3   16 Sep 1992 18:15:00   HENRI
// Fixed order of parameters used to call _DiskBlockDevice.
//
//    Rev 1.2   10 Sep 1992 19:05:24   BRUCE
// Deleted commented out includes
//
//    Rev 1.1   09 Sep 1992 18:18:48   BRUCE
// Eliminated dependency on nwinutil.h
//
//    Rev 1.0   27 Aug 1992 10:33:46   ED
// Initial revision.

#include <dos.h>
#include "platform.h"
#define INCLUDE_VWIN32
#include "disk.h"
#include "xapi.h"
#include "symcfg.h"
#include "thunk.h"
#include "quakever.h"

#if defined(SYM_WIN)                    // Only valid for WIN
#if defined(SYM_DEBUG)
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) OutputDebugString(QMODULE_SYMKRNL": "szOut)
    #endif
#else
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) 
    #endif
#endif  
#endif

static  UINT            wLastBlockDeviceError;

#if !defined(SYM_WIN32)

// I am sick of trying to remember the setup for GlobalDosAlloc.

#define REAL_MODE(dw)               HIWORD(dw)
#define PROT_MODE(dw)               LOWORD(dw)

// This is the maximum sectors that can be read in 1 gulp when in
// WIN 3.0 STANDARD mode.  You can read any size in other modes of
// WIN 3.0, or any mode of WIN 3.1.  Doesn't matter what the sector
// size is either.

#define MAX_STANDARD_SECTORS    8


/*----------------------------------------------------------------------*
 * Global Variables                                                     *
 *----------------------------------------------------------------------*/

static  DISKBLOCKPROC   alpfnDiskBlockDevice[10];
static  DWORD           gdwBigFootSegSel;
static  UINT            gwBufferSize;
static  DWORD           gdwBufferSegSel;
static	WORD		gwGlobalAllocatedCount;

#if defined(SYM_WIN16)

BOOL			gbUseOldWinCode;

DWORD LOCAL PASCAL MyDosAlloc(DWORD dwBytes);
void LOCAL PASCAL MyDosFree(WORD wSel);

#endif

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *									*
 * 	These two routines will allocate memory to be used by		*
 * 	DiskBlockDevice on all future calls.  This allows a program to	*
 * 	allocate its low-dos memory up front.				*
 * 									*
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 * 									*
 * 	NOERR		If buffer was allocated successfully.		*
 *  DISK_BLOCK_ALLOC_ERROR   If memory cannot be allocated.          *
 * 									*
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/28/1993 MARKK Function Created.                                   *
 ************************************************************************/

UINT SYM_EXPORT WINAPI DiskBlockAllocateBuffer(void)
{
#if defined(SYM_WIN16)

    DiskBlockUseOldWinCode(TRUE);

    if (! gbUseOldWinCode)
	return(NOERR);

    if (++gwGlobalAllocatedCount > 1)
	return (NOERR);

    if ((gdwBigFootSegSel = MyDosAlloc(sizeof(BigFootRec))) == NULL)
    return (DISK_BLOCK_ALLOC_ERROR);

    // Can only read 64K - 1 bytes per read/write call

    gwBufferSize = 0xFFFF;

    // Allocate the largest possible I/O buffer in DOS memory.

    while (gwBufferSize > 0)
	{
	gdwBufferSegSel = MyDosAlloc(gwBufferSize);

	if (gdwBufferSegSel == NULL)
	    gwBufferSize -= 1024;
	else
	    break;
	}

    if (gdwBufferSegSel == NULL)
	{
	MyDosFree(PROT_MODE(gdwBigFootSegSel));
    return (DISK_BLOCK_ALLOC_ERROR);
	}

#endif

    return (NOERR);
}

UINT SYM_EXPORT WINAPI DiskBlockDeAllocateBuffer(void)
{
#if defined(SYM_WIN16)

    if (! gbUseOldWinCode)
	return(NOERR);

    if (gwGlobalAllocatedCount == 0)
	return (ERR);

    if (--gwGlobalAllocatedCount == 0)
	{
	MyDosFree(PROT_MODE(gdwBigFootSegSel));
	MyDosFree(PROT_MODE(gdwBufferSegSel));
	}
#endif

    return (NOERR);
}

#if defined(SYM_WIN16)

BOOL SYM_EXPORT WINAPI DiskBlockUseOldWinCode(BOOL bForceReinit)
{
    static	BYTE		szName[SYM_MAX_PATH];

    if (!bForceReinit && szName[0] != EOS)
	return(gbUseOldWinCode);

    if (szName[0] == EOS)
	{
	ConfigGetSymantecDir(szName);
	NameAppendFile(szName, "SYMCFG.INI");
	}

    gbUseOldWinCode = GetPrivateProfileInt("SYMKRNL", "USEDOSALLOC", 0, szName);

    return(gbUseOldWinCode);
}


#pragma optimize("leg", off)

/************************************************************************
 * This function is necessary because the DOS allocations are not	*
 * shareable and thus when the original caller goes away so does the	*
 * memory.  This requests that the memory be sharable and be fixed to	*
 * locate it in the first meg.  We must insure this.			*
 ************************************************************************/

DWORD LOCAL PASCAL MyDosAlloc(DWORD dwBytes)
{
    auto	DWORD		dwRet = 0;
    auto	HANDLE		hHandle;
    auto	DWORD		dwSeg;

    hHandle = GlobalAlloc(GMEM_FIXED | GMEM_SHARE, dwBytes);

    if (hHandle == NULL)
	return 0;

    dwSeg = GetSelectorBase((UINT) hHandle);

    if (dwSeg > 0xFFFF0)
	{
	GlobalFree(hHandle);
	return 0;
	}

    dwRet = (DWORD)MAKELP((WORD)(dwSeg >> 4), (WORD) hHandle);

    return (dwRet);
}

void LOCAL PASCAL MyDosFree(WORD wSel)
{

    GlobalFree((HANDLE) wSel);
}


#else

/*----------------------------------------------------------------------*
 * DOS Version always uses the 'new' method.				*
 *----------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI DiskBlockUseOldWinCode(BOOL bForceReinit)
{
    return (FALSE);
}

#endif

#pragma optimize("", on)

/*********************************************************************
** DiskBlockDevice()
**
**      Performs direct reads or writes on a block device using
**      int 25h or int 26h.
**
**      Block device drivers can only handle requests for less than
**      64K of data at once.  This routine automatically splits up
**      larger requests.
**
**      The Windows implementations of int 25h and int 26h have
**      several bugs:
**
**      First, error reporting is very poor.  On reads, the carry is
**      set on error but no error code is returned.  On writes, no
**      errors are reported at all.  Using DPMI directly solves these
**      problems.
**
**      Second, in Win 3.0 Standard mode, trying to read more than 8
**      sectors (of any size) in one gulp caused the floppy drive to
**      be accessed and the error DRIVE NOT READY to be returned.
**      This bug is fixed in Win 3.1.  Fixed using smaller requests
**      in Win 3.0 standard mode.
**
**      Third, in both Win 3.0 and Win 3.1, trying to read more than
**      one sector at a time on a RAMDRIVE with 256 byte sectors
**      causes problems such as garbled data returned for every 8th
**      sector and GPs on some machines.  Problems with other RAM disks,
**      like VDISK, have also been reported by some testers.  Fixed
**      using smaller requests on RAM disks (of any sector size, just
**      to be safe).
**
**      This routine assumes PROTECTED MODE.
**
** Input:
**
**      'wCommand' is BLOCK_READ or BLOCK_WRITE
**
**      'lpDisk' is an initialized DiskRecord with the I/O request
**      fields filled in (sectorCount, sector, buffer, etc)
**
**      'lpInt13Package' is used only by DiskMapLogToPhyParams().
**      All other callers should pass NULL.  This routine sets the
**      trigger flag in the package so the int 13h hook routine
**      knows that all int 13h's related to swapping are over and
**      the interrupt it should analyze is about to happen.
**
** Returns:
**      0 if no error
**      int 25h/26h error code otherwise
**
**********************************************************************/

#if defined(SYM_PROTMODE)

UINT LOCAL PASCAL Win_DiskBlockDevice(
    UINT        wCommand,
    BYTE        byIsBigFooted,
    BYTE        byDrive,
    DWORD       dwStartSector,
    UINT        wSectorsThisCall,
    DWORD       dwBigFootSegSel,
    LPBYTE      lpDosBuffer,
    INT13REC FAR *lpInt13Package);

// ***********************************************************************
// START WINDOWS VERSION
// ***********************************************************************

UINT LOCAL PASCAL WinDiskBlockDevice (UINT wCommand, LPDISKREC lpDisk)
{
    auto        DWORD           dwBigFootSegSel;
    auto        DWORD           dwMaxSectorsPerCall;
    auto        DWORD           dwBufferSegSel;
    auto        LPBYTE          lpDosBuffer;
    auto        LPBYTE          lpWinBuffer;
    auto        LPBYTE          lpNormalizedBuffer;
    auto        HPBYTE          hpBuf;
    auto        DWORD           dwStartSector;
    auto        DWORD           dwSectorCount;
    auto        DWORD           dwSectorsThisCall;
    auto        DWORD           dwBytesThisCall;
    auto        UINT            wErr = NOERR;
    auto        BYTE            byDrvIdx;

    // Allocate BigFoot packet in DOS mem

    if (! gwGlobalAllocatedCount)
	{
	if ((dwBigFootSegSel = GlobalDosAlloc(sizeof(BigFootRec))) == NULL)
	    {
        wErr = DISK_BLOCK_ALLOC_ERROR;
	    goto done;
	    }

    // Can only read 32k bytes per read/write call

    dwMaxSectorsPerCall = ((DWORD)0x8000 / (DWORD)lpDisk->bytesPerSector);

#ifdef	SYM_WIN16
	// If Windows 3.0 Standard mode, can only read 8 sectors (of any size)
	// per read/write call due to bug in the driver.  Fixed in Win 3.1.

	if ((GetWinFlags() & WF_STANDARD) && GetVersion() == 0x0003)
        dwMaxSectorsPerCall = min(dwMaxSectorsPerCall, 8);
#endif

	// If a RAM disk, read only one sector at a time to get around
	// Windows bugs.

	if (lpDisk->wMajorType == TYPE_RAMDISK)
        dwMaxSectorsPerCall = 1;

	// Allocate the largest possible I/O buffer in DOS memory.

    while (dwMaxSectorsPerCall > 0)
	    {
        dwBufferSegSel = GlobalDosAlloc(dwMaxSectorsPerCall * lpDisk->bytesPerSector);

	    if (dwBufferSegSel == NULL)
        dwMaxSectorsPerCall /= 2;
	    else
		break;
	    }
	}
    else
	{
	// If a RAM disk, read only one sector at a time to get around
	// Windows bugs.

	dwBigFootSegSel = gdwBigFootSegSel;
	dwBufferSegSel = gdwBufferSegSel;

	// Can only read 64K - 1 bytes per read/write call

    dwMaxSectorsPerCall = (gwBufferSize / lpDisk->bytesPerSector) - 1;

#ifdef	SYM_WIN16
	// If Windows 3.0 Standard mode, can only read 8 sectors (of any size)
	// per read/write call due to bug in the driver.  Fixed in Win 3.1.

	if ((GetWinFlags() & WF_STANDARD) && GetVersion() == 0x0003)
        dwMaxSectorsPerCall = min(dwMaxSectorsPerCall, 8);
#endif

	// If a RAM disk, read only one sector at a time to get around
	// Windows bugs.

	if (lpDisk->wMajorType == TYPE_RAMDISK)
        dwMaxSectorsPerCall = 1;
	}

    if (dwBufferSegSel == NULL)
        {
	if (! gwGlobalAllocatedCount)
	    GlobalDosFree(PROT_MODE(dwBigFootSegSel));
        wErr = DISK_BLOCK_ALLOC_ERROR;
        goto done;
        }

    lpDosBuffer = MAKELP(REAL_MODE(dwBufferSegSel), 0);
    lpWinBuffer = MAKELP(PROT_MODE(dwBufferSegSel), 0);

    // Get I/O parameters from disk record.
    // Don't alter the disk record itself.

    dwSectorCount = lpDisk->sectorCount;
    dwStartSector = lpDisk->sector;
    hpBuf = lpDisk->buffer;
    byDrvIdx = (lpDisk->dl >= '0' && lpDisk->dl <= '9')? lpDisk->dl - '0': (BYTE) -1;

    // Read or Write as many sectors as we can in each loop pass

    while (dwSectorCount > 0)
        {
        dwSectorsThisCall = min(dwSectorCount, dwMaxSectorsPerCall);
        dwBytesThisCall = dwSectorsThisCall * (DWORD)lpDisk->bytesPerSector;

	// Create new selector to normalize the huge buffer pointer

	lpNormalizedBuffer = MemoryNormalizeAddress(hpBuf);

        // If writing, fill up DOS buffer with next chunk of input buffer

        if (wCommand == BLOCK_WRITE)
            _fmemcpy(lpWinBuffer, lpNormalizedBuffer, (WORD)dwBytesThisCall);

        // If a block device substitute handler has been assigned, call it.

        if (byDrvIdx != (BYTE) -1 && alpfnDiskBlockDevice[byDrvIdx])
            {
            wErr = alpfnDiskBlockDevice[byDrvIdx]( wCommand, lpDisk, &dwStartSector,
                                                   &((WORD)(dwSectorsThisCall)), NULL, dwBufferSegSel,
                                                   dwBigFootSegSel );
            }
        else
            {
            // Perform INT 25h or 26h call using DPMI

            wErr = Win_DiskBlockDevice(wCommand, (BYTE)(lpDisk->byIOType == DR_IO_BIGFOOT), lpDisk->dl,
                                    dwStartSector, (WORD)dwSectorsThisCall, dwBigFootSegSel,
                                    lpDosBuffer, NULL);

            // Fix for bug in Zenith DOS 3.3+: If UNKNOWN error on a small
            // partition, try again using BigFoot call.

            if (wErr == 0x0207 && !(lpDisk->byIOType == DR_IO_BIGFOOT) && DOSGetVersion() >= DOS_3_30)
                {
                wErr = Win_DiskBlockDevice(wCommand, TRUE, lpDisk->dl,
                                        dwStartSector, (WORD)dwSectorsThisCall,
                                        dwBigFootSegSel, lpDosBuffer, NULL);
                }
            }

        // If still have an error, all done

        if (wErr)
	    {
	    FreeNormalizedAddress(lpNormalizedBuffer);
            break;
	    }

        // If reading, fill up next chunk of output buffer with DOS buffer.

        if (wCommand == BLOCK_READ)
            _fmemcpy(lpNormalizedBuffer, lpWinBuffer, (WORD)dwBytesThisCall);

        // Update I/O parameters for next pass

        dwSectorCount -= dwSectorsThisCall;
        dwStartSector += dwSectorsThisCall;
        hpBuf += dwBytesThisCall;

	// Free the selector used for our normalized buffer pointer

	FreeNormalizedAddress(lpNormalizedBuffer);
        }

    // Free buffers.

    if (!gwGlobalAllocatedCount)
	{
	GlobalDosFree(PROT_MODE(dwBigFootSegSel));
	GlobalDosFree(PROT_MODE(dwBufferSegSel));
	}

    // Save error for later access. Used after FATRead(), etc.

done:
    wLastBlockDeviceError = wErr;
    return (wErr);
}


UINT LOCAL PASCAL Win_DiskBlockDevice(
    UINT        wCommand,
    BYTE        byIsBigFooted,
    BYTE        byDrive,
    DWORD       dwStartSector,
    UINT        wSectorsThisCall,
    DWORD       dwBigFootSegSel,
    LPBYTE      lpDosBuffer,
    INT13REC FAR *lpInt13Package)
{
    auto        union REGS      regs;
    auto        struct SREGS    sregs;
    auto        BigFootRec FAR* lpBigFoot;
    auto        UINT            wErr;

    if( DOSGetVersion() < DOS_7_10 )
        {
                                            // Set up drive number.
        regs.x.ax = (UINT) byDrive - 'A';

                                            // Set up BigFoot package using
                                            // Windows ptr, then set up input
                                            // values using real mode ptr
        if (byIsBigFooted)
            {
            lpBigFoot = MAKELP(PROT_MODE(dwBigFootSegSel), 0);
            lpBigFoot->startingSector = dwStartSector;
            lpBigFoot->sectorCount    = wSectorsThisCall;
            lpBigFoot->buffer         = lpDosBuffer;
            sregs.ds  = REAL_MODE(dwBigFootSegSel);
            regs.x.bx = 0;
            regs.x.cx = 0xFFFF;             // BigFoot signal
            }
        else
            {
                                            // Set up input values using real
                                            // mode i/o buffer ptr
            sregs.ds  = FP_SEG(lpDosBuffer);
            regs.x.bx = FP_OFF(lpDosBuffer);
            regs.x.cx = wSectorsThisCall;
            regs.x.dx = (UINT) dwStartSector;
            }
        }
    else
        {

        regs.x.si = (wCommand == BLOCK_WRITE) ? 1 : 0;
        lpBigFoot = MAKELP(PROT_MODE(dwBigFootSegSel), 0);
        lpBigFoot->startingSector = dwStartSector;
        lpBigFoot->sectorCount    = wSectorsThisCall;
        lpBigFoot->buffer         = lpDosBuffer;
        sregs.ds  = REAL_MODE(dwBigFootSegSel);
        regs.x.ax = 0x7305;
        regs.x.bx = 0;
        regs.x.cx = 0xFFFF;             // BigFoot signal
        regs.x.dx = (byDrive - 'A') + 1;
        wCommand = 0x21;
        }
                                            // If called from DiskMapLogToPhyParams
                                            // then set flag for our Int 13h hook.
    if (lpInt13Package != NULL)
    {
        lpInt13Package->trigger = 1;
    }

    IntWin((BYTE) wCommand, &regs, &sregs);

    if (lpInt13Package != NULL)
    {
        lpInt13Package->trigger = 0;
    }


                                        // If error, save only AH value.
                                        // If AH is zero, set to unknown error
    if (regs.x.cflag)
        {
 #if defined(NCD)
     /*
      *
      * SKURTZ :10-09-95 04:03:31pm
      *
      * Appears that on an NEC, these errors are different than from
      * regular machines.
      *
      *
      */

        if (regs.x.ax == 0x8100)
            wErr = WRITE_PROTECTED;
        else if (regs.x.ax == 0x8102)
            wErr = TIME_OUT;
        else
            wErr = (regs.x.ax & 0xFF00);

#else
        wErr = (regs.x.ax & 0xFF00);
#endif

         if (wErr == 0)
             wErr = DISK_BLOCK_ALLOC_ERROR;
        }
    else
        wErr = NOERR;

    return (wErr);
}

#endif   // SYM_PROTMODE


#pragma optimize("", off)
UINT SYM_EXPORT WINAPI _DiskBlockDevice(
    UINT        wCommand,
    BYTE        byIsBigFooted,
    BYTE        byDrive,
    DWORD       dwStartSector,
    UINT        wSectorsThisCall,
    DWORD       dwBigFootSegSel,
    LPBYTE      lpDosBuffer,
    INT13REC FAR *lpInt13Package)
{
#ifdef	SYM_DOSX
    return Win_DiskBlockDevice(wCommand,
			       byIsBigFooted,
			       byDrive,
			       dwStartSector,
			       wSectorsThisCall,
			       dwBigFootSegSel,
			       lpDosBuffer,
			       lpInt13Package);
#else
    auto        WORD            returnValue;
    auto        BYTE            byAH;
    auto        BigFootRec      rBigFoot;
    auto        LPBYTE          lpBigFootBuffer;

#ifdef	SYM_WIN16
    DiskBlockUseOldWinCode(FALSE);

    if (gbUseOldWinCode)
	return Win_DiskBlockDevice(wCommand,
				   byIsBigFooted,
				   byDrive,
				   dwStartSector,
				   wSectorsThisCall,
				   dwBigFootSegSel,
				   lpDosBuffer,
				   lpInt13Package);
#endif


                                        /*------------------------------*/
                                        /* If being called from         */
                                        /* DiskMapLogToPhyParams(), set */
                                        /* a flag to be examined by     */
                                        /* our Int 13h hook routine.    */
                                        /*------------------------------*/
    if (lpInt13Package != NULL)
        lpInt13Package->trigger = 1;    /* OK to generate a fake error  */

                                        /*------------------------------*/
                                        /* If bigfoot, stuff packet     */
                                        /*------------------------------*/
    byDrive = byDrive - (BYTE) 'A';

    if (byIsBigFooted)
        {
        rBigFoot.startingSector = dwStartSector;
        rBigFoot.sectorCount    = wSectorsThisCall;
        rBigFoot.buffer         = lpDosBuffer;
        lpBigFootBuffer         = (LPBYTE) &rBigFoot;
        }

    _asm
        {
        push    ds                      // Save registers
        push    si                      //
        push    di                      //

        //-------------------------------------------------------
        // Use BigFoot interface?
        //-------------------------------------------------------
        cmp     byIsBigFooted, TRUE     // Is the drive a BigFoot disk?
        jne     oldInterface            // No. Use old interface

        //-------------------------------------------------------
        // Fill registers needed for bigFoot interface.
        // First fill packet with correct values.
        //-------------------------------------------------------
        mov     cx, 0FFFFh              // Signal BigFoot interface
        lds     bx, lpBigFootBuffer     // Point to start of packet
        jmp     short doDiskOp          // Now read the disk

        //-------------------------------------------------------
        // Use old-fashioned INT 25h/INT 26h interface
        //-------------------------------------------------------
oldInterface:
        mov     cx, wSectorsThisCall            // count of sectors
        mov     dx, WORD PTR dwStartSector      // First sector number
        lds     bx, lpDosBuffer

doDiskOp:
        mov     di, 0FFFCh              // Magic PC-MOS number (ignored by DOS)
        mov     al, byDrive             // Get drive number
        xor     ah, ah                  // Clear error code
        push    bp                      // Save BP, since destroyed

        cmp     wCommand, BLOCK_WRITE   // Is this a write (or read)?
        je      writeDisk               // Yes. Write disk

	push	sp			// This is to fix a problem with
					// Rational's dos extender.  They
					// return with the stack the same as
					// when we left, thus making it
					// necessary for us to assertain the
					// state upon return.
        int     25h                     // No. Read from disk
        jmp     short testError         // Continue

writeDisk:
	push	sp			// This is to fix a problem with
					// Rational's dos extender.  They
					// return with the stack the same as
					// when we left, thus making it
					// necessary for us to assertain the
					// state upon return.

        int     26h                     // Write to disk
testError:
	jc	er1
	pop	dx			// See if WFW left the stack correct
	cmp	dx, sp
	je	popbp
	pop	dx
popbp:	pop	bp
	jmp	short noError

er1:
	pop	dx			// See if WFW left the stack correct
	cmp	dx, sp
	je	popbp1
	pop	dx
popbp1:	pop	bp
	jmp	short errorFound

        pop     dx                      // Remove flags
        pop     bp                      // Restore BP, since destroyed
        jc      errorFound              // Carry flag set if error

noError:
	xor     ax,ax                   // No error occurred
        jmp     short locRet            // Return to caller

errorFound:
        xor     al, al                  // Don't use AL
        mov     byAH, ah                // Save AH value
        or      ah, ah                  // Make sure AH is non-zero
        jne     locRet                  // Non-zero, return

        mov     ah, 2                   // 2 is Unknown error

locRet:
        pop     di                      // Restore registers
        pop     si                      //
        pop     ds                      //
        mov     returnValue, ax         // Get return value
        }

                                        /*------------------------------*/
                                        /* Windows 3.0 doesn't return   */
                                        /* the TRUE error code. In fact,*/
                                        /* it doesn't return the error  */
                                        /* code at all, just the CY is  */
                                        /* set.  We will simulate the   */
                                        /* interrupt, to get the TRUE   */
                                        /* error code.                  */
                                        /*------------------------------*/
    if (lpInt13Package != NULL)
        lpInt13Package->trigger = 0;

    wLastBlockDeviceError = returnValue;

    return(returnValue);
#endif
}
#pragma optimize("", on)


BYTE SYM_EXPORT WINAPI DiskBlockSetProc( DISKBLOCKPROC lpfnDiskBlockProc )
{
    UINT        wIndex;

    for (wIndex = 0; wIndex < 10; wIndex++)
        {
        if (alpfnDiskBlockDevice[wIndex] == NULL)
            {
            alpfnDiskBlockDevice[wIndex] = lpfnDiskBlockProc;
            return (BYTE) (wIndex + '0');
            }
        }

    return (BYTE) FALSE;
}


BOOL SYM_EXPORT WINAPI DiskBlockClearProc( BYTE byDriveLetter, DISKBLOCKPROC lpfnDiskBlockProc )
{
    UINT        wIndex;

    if (byDriveLetter >= '0' && byDriveLetter <= '9')
        {
        wIndex = (UINT) (byDriveLetter - '0');

        if (alpfnDiskBlockDevice[wIndex] == lpfnDiskBlockProc)
            {
            alpfnDiskBlockDevice[wIndex] = NULL;
            return TRUE;
            }
        }

    return FALSE;
}

#endif   // SYM_WIN32


UINT SYM_EXPORT WINAPI DiskGetBlockDeviceError(VOID)
{
    return (wLastBlockDeviceError);
}

// ***********************************************************************
// START DOS VERSION
//
// NOTE: The following fields MUST be filled in before calling:
//              lpDisk->sector
//              lpDisk->sectorCount
//              lpDisk->buffer
//              lpDisk->dl
//              lpDisk->isBigFooted
//              lpDisk->wMajorType
//              lpDisk->bytesPerSector
// ***********************************************************************

UINT SYM_EXPORT WINAPI DiskBlockDevice (UINT uCommand, LPDISKREC lpDisk)
{
    auto        UINT            uErr = NOERR;

#ifdef SYM_WIN32
#ifndef _M_ALPHA
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
#endif        	
        char        szDrive[SYM_MAX_PATH];
        HANDLE      hFile;
        DWORD       dwOffset;
        DWORD       dwProcessed;

                                        // Open the drive as a file.
        wsprintf(szDrive, "\\\\.\\%c:", lpDisk->dl);
        hFile = CreateFile(szDrive, GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return(DISK_BLOCK_ALLOC_ERROR);

                                        // Convert to a linear offset
        dwOffset = lpDisk->sector * lpDisk->bytesPerSector;
        SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);

        if (uCommand == BLOCK_READ)
            {
            uErr = ReadFile(hFile, lpDisk->buffer,
                         (DWORD) lpDisk->sectorCount * lpDisk->bytesPerSector, &dwProcessed, NULL) ?
                         NOERR : DISK_BLOCK_ALLOC_ERROR;
            }
        else if (uCommand == BLOCK_WRITE)
            {
            uErr = WriteFile(hFile, lpDisk->buffer,
                         (DWORD) lpDisk->sectorCount * lpDisk->bytesPerSector, &dwProcessed, NULL) ?
                         NOERR : DISK_BLOCK_ALLOC_ERROR;
            }
        else
            uErr = DISK_BLOCK_ALLOC_ERROR;
        CloseHandle(hFile);
#ifndef _M_ALPHA
        }
    else// if (!GetSystemMetrics(SM_CLEANBOOT))
        {
        auto        BigFootRec      rBigFoot;

                                        // New Extended Int21 0x7305
                                        // Absolute Disk Read/Write.
        if( lpDisk->byIOType == DR_IO_EXTENDED )
            {
            auto        SYM_REGS        regs;

            rBigFoot.startingSector = lpDisk->sector;
            rBigFoot.sectorCount    = (WORD)lpDisk->sectorCount;
            rBigFoot.buffer         = lpDisk->buffer;

            regs.EAX = 0x7305;
            regs.EBX =(DWORD) &rBigFoot;  // bigfoot buffer
            regs.ECX =(DWORD) 0xFFFF;     // bigfoot indicator
            regs.EDX = (lpDisk->dl - 'A') + 1; // Drive number is 1 based!
            regs.ESI = ( uCommand == BLOCK_WRITE) ? 1 : 0;

            VxDIntBasedCall( 0x21, &regs );

            if (regs.EFlags & 1) // need an equate for a CY flag mask!!!
                if (!(uErr = (WORD) (regs.EAX & 0x0FF00))) // if no error code
                    uErr = 0x200;                             // Unknown error
            }
        else
            {
            auto        DIOC_REGISTERS  reg;
            //
            //  Setup registers for an Int25/26
            //
            reg.reg_EAX=(DWORD) (lpDisk->dl - 'A'); // drive (0=A)
            reg.reg_EBX=(DWORD) lpDisk->buffer;      // buffer
            reg.reg_ECX=(DWORD) lpDisk->sectorCount; // number of sectors
            reg.reg_EDX= lpDisk->sector;           // starting sector number

            reg.reg_EDI=0x0FFFC;          // PC-MOS number???? (no effect on DOS)

            if (lpDisk->byIOType == DR_IO_BIGFOOT)
                {
                rBigFoot.startingSector = lpDisk->sector;
                rBigFoot.sectorCount    = (WORD)lpDisk->sectorCount;
                rBigFoot.buffer         = lpDisk->buffer;

                reg.reg_EBX=(DWORD) &rBigFoot;  // bigfoot buffer
                reg.reg_ECX=(DWORD) 0xFFFF;     // bigfoot indicator
                }
            //
            //  We have setup the registers for a read/write operation
            //
            IOCtlVWin32( &reg, &reg, (uCommand == BLOCK_WRITE) ? VWIN32_DIOC_DOS_INT26 : VWIN32_DIOC_DOS_INT25 );
            //
            //  Check to see if an error occurred
            //
            if (reg.reg_Flags & 1) // need an equate for a CY flag mask!!!
                if (!(uErr = (WORD) (reg.reg_EAX & 0x0FF00))) // if no error code
                    uErr = 0x200;                             // Unknown error
            }
        }

#if 0
    else                                //Must be in fail safe mode
        {
        static FARPROC	 lpfnDiskBlockDevice;
        extern HINSTANCE hInstThunkDll;

        if (lpfnDiskBlockDevice == NULL)
            {
			if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
			    {
		        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
		        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
		        }
		    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
	             {
	             lpfnDiskBlockDevice = GetProcAddress(hInstThunkDll, "DiskBlockDevice32" );
	             if (lpfnDiskBlockDevice == NULL)
	                 return uErr = ERR;
	             }
            else
                 return uErr = ERR;
            }
        if (uErr == NOERR)
            uErr = lpfnDiskBlockDevice(uCommand, lpDisk);
	}
#endif // 0
#endif // _M_ALPHA
    wLastBlockDeviceError = uErr;
    return (uErr);

#elif defined(SYM_DOSX)
    return WinDiskBlockDevice(uCommand, lpDisk);
#else
    auto        UINT            wMaxSectorsPerCall;
    auto        HPBYTE          hpBuf;
    auto        DWORD           dwStartSector;
    auto        DWORD           dwSectorCount;
    auto        DWORD           dwSectorsThisCall;
    auto        DWORD           dwBytesThisCall;
    auto        UINT            wErr = NOERR;
    auto        BYTE            byDrvIdx;
    auto        LPBYTE          lpNormalizedBuffer;

#ifdef	SYM_WIN16
    DiskBlockUseOldWinCode(FALSE);
    if (gbUseOldWinCode)
	return WinDiskBlockDevice(uCommand, lpDisk);
#endif

    // Can only read 64K - 1 bytes per read/write call
    wMaxSectorsPerCall = (0xFFFF / lpDisk->bytesPerSector) - 1;

    if (lpDisk->wMajorType == TYPE_RAMDISK)
	wMaxSectorsPerCall = 1;

    // Get I/O parameters from disk record.
    // Don't alter the disk record itself.

    dwSectorCount = lpDisk->sectorCount;
    dwStartSector = lpDisk->sector;
    hpBuf = lpDisk->buffer;
    byDrvIdx = (lpDisk->dl >= '0' && lpDisk->dl <= '9')? lpDisk->dl - '0': (BYTE) -1;

    // Read or Write as many sectors as we can in each loop pass

    while (dwSectorCount > 0)
        {
        dwSectorsThisCall = min(dwSectorCount, wMaxSectorsPerCall);
        dwBytesThisCall = dwSectorsThisCall * lpDisk->bytesPerSector;

	lpNormalizedBuffer = MemoryNormalizeAddress(hpBuf);

        // If a block device substitute handler has been assigned, call it.

        if (byDrvIdx != (BYTE) -1 && alpfnDiskBlockDevice[byDrvIdx])
            {
            wErr = alpfnDiskBlockDevice[byDrvIdx]( uCommand, lpDisk, &dwStartSector,
                                                   &((WORD)dwSectorsThisCall), lpNormalizedBuffer, NULL, NULL );
            }
        else
            {
            // Perform INT 25h or 26h call

            wErr = _DiskBlockDevice(uCommand, (BYTE)(lpDisk->byIOType == DR_IO_BIGFOOT), lpDisk->dl,
                                    dwStartSector, (WORD)dwSectorsThisCall, NULL,
                                    lpNormalizedBuffer, NULL);

            // Fix for bug in Zenith DOS 3.3+
            // If UNKNOWN error on small partition, try again using BigFoot call.

            if (wErr == 0x0207 && !(lpDisk->byIOType == DR_IO_BIGFOOT) && DOSGetVersion() >= DOS_3_30)
                {
                wErr = _DiskBlockDevice(uCommand, TRUE, lpDisk->dl,
                                        dwStartSector, (WORD)dwSectorsThisCall, NULL,
                                        lpNormalizedBuffer, NULL);
                }
            }

        // If still have an error, all done

        if (wErr)
	    {
	    FreeNormalizedAddress(lpNormalizedBuffer);
            break;
	    }

        // Update I/O parameters for next pass

        dwSectorCount -= dwSectorsThisCall;
        dwStartSector += dwSectorsThisCall;
        hpBuf += dwBytesThisCall;
	FreeNormalizedAddress(lpNormalizedBuffer);
        }

    // Save error for later access. Used after FATRead(), etc.

    wLastBlockDeviceError = wErr;
    return (wErr);
#endif
}



