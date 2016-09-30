/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/PRM_MEM.C_v   1.10   27 Jun 1997 14:18:04   RStanev  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      MemorySearch                                                    *
 *      MemoryHugeMove                                                  *
 *      MemAvail                                                        *
 *      MemReAlloc                                                      *
 *      MemAlloc                                                        *
 *      MemoryHugeCopy                                                  *
 *      MemCopyPhysical                                                 *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/PRM_MEM.C_v  $ *
//
//    Rev 1.10   27 Jun 1997 14:18:04   RStanev
// Fixed compilation errors when compiling with SYM_MONITOR_MEMORY_USAGE.
//
//    Rev 1.9   06 Jan 1997 12:09:28   RSTANEV
// Added SYM_VXD versions of MemAllocPtr(), MemFreePtr() and MemReAllocPtr().
//
//    Rev 1.8   17 May 1996 16:44:14   RSTANEV
// Replaced DbgPrint() calls with KdPrint().
//
//    Rev 1.7   13 May 1996 11:35:14   RSTANEV
// Memory tracking will be activated only if SYM_MONITOR_MEMORY_USAGE is
// defined.
//
//    Rev 1.6   07 May 1996 15:15:34   RSTANEV
// Added extra debug message for SYM_NTK.
//
//    Rev 1.5   06 May 1996 21:44:14   RSTANEV
// Added MemReAllocPtr() for SYM_NTK and enhanced error checking and memory
// tracking.
//
//    Rev 1.4   11 Apr 1996 14:05:30   RSTANEV
// SYM_PARANOID is now used only with /DSYM_DEBUG or /DPRERELEASE.  Also, we
// change the local tags of allocated memory block right before freeing it.
//
//    Rev 1.3   27 Feb 1996 13:48:58   RSTANEV
// MemAllocPtr() for SYM_NTK platform now tolerates GMEM_SHARE.
//
//    Rev 1.2   22 Feb 1996 15:52:08   RSTANEV
// Added paranoid checks in MemAllocPtr() and MemFreePtr() for SYM_NTK.
//
//    Rev 1.1   08 Feb 1996 15:03:42   DBUCHES
// Added MemoryHugeCompare().
//
//    Rev 1.0   26 Jan 1996 20:22:12   JREARDON
// Initial revision.
//
//    Rev 1.49   11 Dec 1995 16:38:54   RSTANEV
// Added SYM_NTK platform support.
//
//    Rev 1.48   11 Apr 1995 17:20:56   HENRI
// Merged changes from branch 6
//
//    Rev 1.47   07 Feb 1995 22:04:40   DALLEE
// SYM_DOS MemCopyPhysical() -- Added call to MEMCPY() if StealthCopy() fails.
// StealthCopy() only works if QEMM is in memory in stealth mode.
// (may also only work for stealth areas of memory... Have to test this later.)
//
//    Rev 1.46   19 Jan 1995 17:49:46   MARKL
// MemCopyPhysical() now builds for VxD.
//
//    Rev 1.45   22 Dec 1994 03:03:34   JMILLARD
// install MemCopyPhysical for NLM
//
//    Rev 1.44   20 Dec 1994 15:52:52   BRUCE
// Changed back to duplicating code in memrealloc()
//
//    Rev 1.43   20 Dec 1994 14:54:36   BRUCE
//
// Fixed inconsistent use of HANDLE/HGLOBAL - all are HGLOBAL.
// Also changed MemReAlloc() to use MemReAllocPtr() for Win32 case (since the
// HGLOBAL is really a LPVOID, why not use a single piece of code?)
//
//    Rev 1.42   20 Dec 1994 10:34:20   BRIANF
// Modified MemFree to return the results of the GlobalFree call.
//
//    Rev 1.41   20 Dec 1994 08:47:04   MARKK
// Fixed MemReAlloc to correctly use GlobalReAlloc
//
//    Rev 1.40   20 Dec 1994 05:53:24   ENRIQUE
// Restored MemReAlloc to call GlobalReAlloc.
//
//    Rev 1.39   19 Dec 1994 18:39:22   BRIANF
// Made the following changes to the Win32 MemXXX functions:
//
// - Added MemAlloc and MemFree.  They used to be macros in
// platform.h.
// - MemAlloc now locks memory immediately following allocating.
// It returns a pointer casted as a handle.
// - MemFree now unlocks the memory before freeing it.
// - I changed MemRealloc to unlock memory before freeing it and
// to lock it back after reallocating.
//
//    Rev 1.38   07 Dec 1994 15:32:00   JMILLARD
//
// make dummy version of MemCopyPhysical for NLM for now
//
//    Rev 1.37   18 Nov 1994 12:10:04   BRAD
// Added better error checking, so won't blow up if IOCTL.VXD doesn't exist
//
//    Rev 1.36   16 Nov 1994 18:00:30   BRAD
// No longer thunk for MemCopyPhysical()
//
//    Rev 1.35   22 Sep 1994 14:12:44   TONY
// Changed OS2GetTicks to GetTickCount()
//
//
//    Rev 1.34   15 Sep 1994 09:48:44   TONY
// Removed VMMClearMem(), VMMWinFlags() calls under OS/2.  These should have been added for DOS only!
//
//    Rev 1.33   15 Sep 1994 09:29:38   TONY
// OS/2 fix - NULL
//
//    Rev 1.32   14 Sep 1994 15:03:14   SZIADEH
// zeored out the top work in ecx in memcopyphysical.
//
//    Rev 1.31   21 Jul 1994 14:00:20   BRAD
// Renamed VMM.H to DOSVMM.H
//
//    Rev 1.30   11 Jul 1994 15:30:04   TONY
// Moved Quake 4 OS/2 changes to trunk
//
//    Rev 1.29   28 Jun 1994 08:40:02   MFALLEN
// MemAvail() wasn't working for W32.
//
//    Rev 1.28   23 Jun 1994 17:22:06   HENRI
// Moved 16 bit thunk layers to a separate DLL
//
//    Rev 1.27   22 Jun 1994 12:25:56   HENRI
// Added thunking layers
//
//    Rev 1.26   13 May 1994 13:46:34   MARKK
// Dosx work
//
//    Rev 1.25   05 May 1994 12:27:02   BRAD
// DX stuff
//
//    Rev 1.24   04 May 1994 11:22:32   MARKK
// DOSX work
//
//    Rev 1.23   26 Apr 1994 15:44:42   BRAD
// Added MemCopyPhysical() as Win32s thunk
//
//    Rev 1.22   22 Apr 1994 14:08:22   BRAD
// For WIN32, need to return FALSE
//
//    Rev 1.21   22 Apr 1994 14:06:30   BRAD
// This will only work for WIN16
//
//    Rev 1.20   18 Apr 1994 17:02:00   SKURTZ
// Some DOS compile problems: MemCopyPhysical() must return a value. Also
// changed some references from dwBYTEs to dwBytes which stopped the
// compile under DOS.
//
//
//    Rev 1.19   18 Apr 1994 14:36:46   BRAD
// Added MemCopyPhysical()
//
//    Rev 1.18   15 Mar 1994 12:33:22   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.17   25 Feb 1994 12:21:26   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.16   16 Feb 1994 13:03:00   PGRAVES
// Win32.
//
//    Rev 1.15   28 Sep 1993 18:24:02   JOHN
// Moved MemoryCopyROM() to new file PRM_ROM.C
//
//    Rev 1.14   28 Sep 1993 14:22:32   ED
// Tried to implement a Windows version of MemoryCopyROM, but gave up.
//
//    Rev 1.13   23 Sep 1993 12:56:10   ED
// Added MemoryCopyROM, which used to be in ROMCOPY.ASM.  Now ROMCOPY.ASM
// contains just a subroutine used to deal with QEMM.
//
//    Rev 1.12   02 Jul 1993 08:52:12   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.11   15 Feb 1993 21:04:36   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.10   12 Feb 1993 04:26:26   ENRIQUE
// No change.
//
//    Rev 1.9   05 Feb 1993 08:49:54   BRUCE
// Added MemoryHugeCopy()
//
//    Rev 1.8   07 Jan 1993 22:56:26   JOHN
// (1) Changed DOS version of MemAvail() to use correct VMMAvail() parameters.
// (2) Fixed DOS version of MemReAlloc() to actually zero the new memory when
//     the GMEM_ZEROINIT flag is set and the block is being expanded.
//
//    Rev 1.7   31 Dec 1992 12:06:14   MARKK
// Fixed bug in clear mem
//
//    Rev 1.6   15 Sep 1992 08:34:20   ED
// Windows cleanup
//
//    Rev 1.5   10 Sep 1992 18:39:04   BRUCE
// Eliminated unnecesary includes
//
//    Rev 1.4   03 Sep 1992 12:50:48   ED
// Added _MEMAVAIL
//
//    Rev 1.3   03 Sep 1992 11:39:46   ED
// Added MODULE_NAME
//
//    Rev 1.2   03 Sep 1992 07:23:32   ED
// Fixed parameter lists
//
//    Rev 1.1   02 Sep 1992 11:25:18   ED
// Cleaned up errors and warnings for VMM calls
//
//    Rev 1.11   02 Sep 1992 07:12:22   ED
// Added better handle validation.
//
//    Rev 1.10   01 Sep 1992 11:08:22   ED
// Fixed pointer validation
//
//    Rev 1.9   01 Sep 1992 10:59:44   ED
// Added pointer validation
//
//    Rev 1.8   20 Aug 1992 14:52:28   ED
// Fixed the handle used in MemFree.
//
//    Rev 1.7   20 Aug 1992 14:45:34   ED
// Added parameter validation to the main memory routines
//
//    Rev 1.6   19 Aug 1992 10:55:54   ED
// Fixed up some stuff
//
//    Rev 1.5   19 Aug 1992 10:44:52   ED
// Added the Sub... memory functions
//
//    Rev 1.4   14 Aug 1992 15:28:28   ED
// Fixed up the DOS memory stuff.  We still need the VMMFlags function to
// be implemented.
//
//    Rev 1.3   11 Aug 1992 14:02:18   ED
// Added lock count checking in _MEMUNLOCK, so the pointer doesn't get
// killed inside a nested block.
//
// Modified _MEMFREE to "force" a block to be freed by automatically
// unlocking it enough times to get the lock count down to zero.
//
//    Rev 1.2   04 Aug 1992 10:35:48   ED
// Disabled all VMM stuff, since VMM is not ready yet
//
//    Rev 1.1   31 Jul 1992 11:29:00   ED
// Updated the DOS side of the memory functions, and added MEMSIZE and
// MEMCOMPACT
//
//    Rev 1.0   20 Jul 1992 14:21:16   ED
// Initial revision.
 ************************************************************************/

#include "platform.h"
#include "xapi.h"

#if !defined(SYM_NTK)

#include "symkrnl.h"
#include "thunk.h"

#ifdef SYM_NLM

#include "page_mem.h"               // page table access routines

#else   // ifdef SYM_NLM

#if defined(SYM_DOS)
MODULE_NAME;

#include "dosvmm.h"

void LOCAL PASCAL VMMClearMem (HPBYTE lpData, DWORD dwBYTEs);
void LOCAL PASCAL VMMWinFlags(HGLOBAL h, DWORD size, UINT flags);

#endif

#if defined(SYM_OS2)
    #define NULL 0
#endif

#if !defined(SYM_VXD)

/*----------------------------------------------------------------------*/
/* MemorySearch                                                         */
/*      Searches a buffer for a sub-buffer of characters.               */
/*                                                                      */
/*      Returns:                                                        */
/*              Returns TRUE if string is found, else FALSE.            */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI MemorySearch(
    LPVOID lpBuffer,
    UINT wBufferLen,
    LPVOID lpStr,
    UINT wStrLen
)
{
    BOOL bRet = FALSE;
    char c;
    LPSTR p, pMax;


    c = *((LPSTR)lpStr);

    p = (LPSTR)lpBuffer;

    pMax = (LPSTR)lpBuffer + wBufferLen - wStrLen + 1;

    while (p < pMax)
        {
        if (*p == c)                    // Found first character.
            {
            if (MEMCMP(p, lpStr, wStrLen) == 0)
                {
                bRet = TRUE;
                break;
                }
            }
        ++p;
        }

    return (bRet);
}


/*----------------------------------------------------------------------*
 * MemoryHugeMove()                                                     *
 *                                                                      *
 * This procedure copies data from one far location to another,         *
 * permitting copying of blocks larger then 64K.                        *
 *----------------------------------------------------------------------*/
#ifndef SYM_WIN32

#define MAX_FMOVE       0xFFF0          // Max bytes to move with _fmemmove()

VOID SYM_EXPORT WINAPI MemoryHugeMove (void huge *dest, void huge *src, DWORD count)
{
                                        // Forward copying order
                                        // If more than 64K, use recursion,
                                        // copying first block first.

    if ((HPBYTE) dest < (HPBYTE) src || ((HPBYTE) src) + count < (HPBYTE) dest)
        {
        if (count > MAX_FMOVE)
            {
            MEMMOVE(dest, src, MAX_FMOVE);
            MemoryHugeMove((HPBYTE) dest + MAX_FMOVE,
                           (HPBYTE) src  + MAX_FMOVE, count - MAX_FMOVE);
            }
        else
            MEMMOVE(dest, src, (WORD) count);
        }

                                        // Reverse copying order
                                        // If more than 64K, use recursion,
                                        // copying last block first.
    else
        {
        if (count > MAX_FMOVE)
            {
            MEMMOVE((HPBYTE) dest + count - 1 - MAX_FMOVE,
                      (HPBYTE) src  + count - 1 - MAX_FMOVE, MAX_FMOVE);
            MemoryHugeMove(dest, src, count - MAX_FMOVE);
            }
        else
            MEMMOVE(dest, src, (WORD) count);
        }
}
#endif                                  // #ifndef SYM_WIN32

////////////////////////////////////////////////////////////////////////
//
//   MemoryHugeCopy()
//
// Description:
//
//   Copy data into a huge buffer in amounts > 64k.
//
//   This function was copied from the June 1992 Windows/Dos Developer's
//   Journal, page 59, listing 1 (slightly modified to return dest buffer).
//
//   Returns the address of the destination buffer (like _fmemcpy)
//
// See Also:
//
//
////////////////////////////////////////////////////////////////////////
//  2/ 4/93 BEM Function created.
////////////////////////////////////////////////////////////////////////
#ifndef SYM_WIN32
void huge * SYM_EXPORT WINAPI MemoryHugeCopy(
    void huge *dest,
    const void huge *src,
    DWORD dwBytes)
{
    WORD  wSize;
    char huge *to = (char huge *)dest;
    char huge *from = (char huge *)src;

    while (dwBytes)
	{
	if (dwBytes > 32768U)
	    wSize = 32768U;
	else
	    wSize = (WORD) dwBytes;

	_fmemcpy(to, from, wSize);
	to += wSize;
	from += wSize;
	dwBytes -= wSize;
	}

    return(dest);
}
#endif                                  // #ifndef SYM_WIN32


//////////////////////////////////////////////////////////////////////////
//MemoryHugeCompare
//
//  Compares huge blocks of memory.
//
//
//PARAMS : 2 pointers to data to be compared.
//         number of bytes to compare.
//
//
//RETURNS: 0 if blocks are identical
//         non-zero otherwise
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   02-08-96 02:24:27pm
//////////////////////////////////////////////////////////////////////////
#ifndef SYM_WIN32
int SYM_EXPORT WINAPI MemoryHugeCompare(HPBYTE hpData1, HPBYTE hpData2, DWORD dwBytes)
{
    UINT        wCount;
    LPBYTE      lpPtr1;
    LPBYTE      lpPtr2;

    while (dwBytes != 0)
        {
        lpPtr1 = MemoryNormalizeAddress(hpData1);
        lpPtr2 = MemoryNormalizeAddress(hpData2);

        if (dwBytes > 0x8000)
            wCount = 0x8000;
        else
            wCount = (UINT) dwBytes;

        if( MEMCMP(lpPtr1, lpPtr2, wCount) != 0 )
            {
            FreeNormalizedAddress(lpPtr1);
            FreeNormalizedAddress(lpPtr2);
            return(1);
            }

        FreeNormalizedAddress(lpPtr1);
        FreeNormalizedAddress(lpPtr2);

        hpData1 += wCount;
        hpData2 += wCount;
        dwBytes -= wCount;
        }

    return(0);
}
#endif

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

void SYM_EXPORT WINAPI MemAvail(DWORD FAR *lpdwFree, DWORD FAR *lpdwLargest)
{
#ifdef SYM_WIN32
    auto    MEMORYSTATUS    MemStat = {0};

    MemStat.dwLength = sizeof(MemStat);
    GlobalMemoryStatus ( &MemStat );

    *lpdwFree = *lpdwLargest = MemStat.dwAvailVirtual;

#elif defined(SYM_WIN)
    *lpdwFree = GetFreeSpace(0);

    *lpdwLargest = GlobalCompact(0);
#elif defined(SYM_DOS)
    DWORD       dwJunk;

    VMMAvail(lpdwLargest, lpdwFree, &dwJunk);
#else
    *lpdwFree = *lpdwLargest = 0;               // Not supported
#endif
}

/*------------------------------------------------------------------------*/
/*    The DOS version is a cover for VMMReAlloc, but it also includes     */
/*    code to interpret the Windows memory flags.                         */
/*                                                                        */
/*    The Windows version includes code to fix an apparent bug in         */
/*    GlobalReAlloc in the Windows SDK.  It appears to work correctly     */
/*    until an attempt is made to allocate beyond 64K.                    */
/*    The solution to the problem is to allocate multiples of 64 beyond   */
/*    the first 64K.  The developer is shielded from have to worry about  */
/*    allocate memory in 64K increments.  Developers should continue to   */
/*    request the amount of memory they actually want and not be concerned*/
/*    with the limitations of Global reallocation.                        */
/*                                                                        */
/* INPUTS                                                                 */
/*    Same as GlobalReAlloc                                               */
/*                                                                        */
/* RETURNS                                                                */
/*    Same as GlobalReAlloc                                               */
/*------------------------------------------------------------------------*/

#define ONE_SEGMENT     (DWORD)65534
#define HEADER_SIZE     16

HGLOBAL SYM_EXPORT WINAPI MemReAlloc(HGLOBAL hMem, DWORD dwBYTEs, UINT wFlags)
{
#if defined(SYM_WIN32)

    hMem = GlobalPtrHandle((LPVOID) hMem);
    GlobalUnlock(hMem);
    hMem = GlobalReAlloc(hMem, dwBYTEs, wFlags);
    return (HGLOBAL)GlobalLock(hMem);

#elif defined(SYM_WIN)
    auto        DWORD           dwSize;
    auto        WORD            wNumSegments;

    if (dwBYTEs <= ONE_SEGMENT)
        {
                                        /* Nothing to fix.  It actually works*/
        return (GlobalReAlloc(hMem, dwBYTEs, wFlags));
        }
                                        /* Get the size of the object.       */
    dwSize = GlobalSize(hMem);
                                        /* Global Size includes the header   */
                                        /* information.                      */
    dwSize -= HEADER_SIZE;

                                        /* The object is already larger      */
                                        /* enough for the allocation.        */
    if (dwSize >= dwBYTEs)
        return (hMem);
                                        /* Calculate the number of required  */
                                        /* segments.                         */
    wNumSegments = (WORD)(dwBYTEs/((DWORD)ONE_SEGMENT));
                                        /* Make sure to round up to the next */
                                        /* segment boundary.                 */
    if (dwBYTEs % ONE_SEGMENT)
        wNumSegments++;

    return (GlobalReAlloc(hMem,
                          (DWORD)(((DWORD)wNumSegments) * ONE_SEGMENT),
                          wFlags));
#elif defined(SYM_DOS) || defined(SYM_OS2)
    DWORD       dwOldSize;
    HPBYTE      hpPtr;

    dwOldSize = VMMSize(hMem);

    if ((hMem = VMMReAlloc((HGLOBAL)hMem, dwBYTEs)) == NULL)
	return (NULL);

    if (wFlags & GMEM_ZEROINIT)
        {
        if (dwOldSize < dwBYTEs)
            {
            hpPtr = (BYTE HUGE *) VMMLock(hMem);

            hpPtr += dwOldSize;
            hpPtr = MemoryNormalizeAddress(hpPtr);

#ifdef SYM_DOS
            VMMClearMem(hpPtr, dwBYTEs - dwOldSize);

	    FreeNormalizedAddress(hpPtr);
#endif

            VMMUnlock(hMem, hpPtr);
            }

        wFlags &= ~GMEM_ZEROINIT;
        }

#ifdef SYM_DOS
    VMMWinFlags(hMem, dwBYTEs, wFlags);
#endif

    return (hMem);
#endif                                  // not SYM_WIN
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

#if defined(SYM_DOS)

HGLOBAL SYM_EXPORT WINAPI MemAlloc(UINT wFlags, DWORD dwBytes)
{

    HGLOBAL     hMem;

    hMem = VMMAlloc(dwBytes);

    if (hMem != NULL)
        VMMWinFlags(hMem, dwBytes, wFlags);

    return (hMem);
}

        // -------------------------------------------------------
        // The following routines simulate Windows functionality
        // -------------------------------------------------------
void LOCAL PASCAL VMMClearMem(HPBYTE lpData, DWORD dwBytes)
{
    UINT        wCount;
    LPBYTE      lpPtr;

    while (dwBytes != 0)
        {
        lpPtr = MemoryNormalizeAddress(lpData);

        if (dwBytes > 0x8000)
            wCount = 0x8000;
        else
            wCount = (UINT) dwBytes;

        MEMSET(lpPtr, 0, wCount);

	FreeNormalizedAddress(lpPtr);

        lpData += wCount;
        dwBytes -= wCount;
        }
}

void LOCAL PASCAL VMMWinFlags(HGLOBAL h, DWORD size, UINT flags)
{
    HPBYTE      lpData;

    if (flags & GMEM_ZEROINIT)
        {
        lpData = VMMLock(h);
        VMMClearMem(lpData, size);
        VMMUnlock(h, lpData);
        }

    if (flags & GMEM_FIXED)
        {
        _VMMLock(h);
        }
}
#endif


//**************************************************************************
//**************************************************************************

#if defined(SYM_OS2)
typedef struct {
    ULONG       PhysAddr;
    USHORT      Length;
    }   LDTParm;

typedef struct {
    USHORT      LDTOff;
    USHORT      LDTSel;
    }   LDTData;
#endif

#endif // ifndef SYM_VXD

#endif  // ifndef SYM_NLM


//******************************************************************************
//  Name:           MemCopyPhysical()
//  Author:         Tony McNamara
//  Created:        10-Feb-93
//  Purpose:        Copy contents of physical physical memory to buffer
//  Limitations:    Limited to first 1MB of RAM.
//                  size of copy limited to 16 bit value (64K)
//                  NLM has no limits on location
//  Parameters:     lpSrc is the start address
//                  lpDest is a pointer to the buffer to copy RAM to
//                  wSize is the amount of data to copy, which MUST fit into
//                          the buffer - NLM can handle more than 64K
//                  wFlags specifies whether the src or dest are physical
//                          addresses or virtual (if 0, then will treat
//                          lpSrc as physical, lpDest as virtual).
// NOTE:            For OS/2, uFlags unsupported.  Currently, src is physical,
//                          dest is virtual.
//                  NLM ignores uFlags as well. Basically same rules as OS/2
//
// Returns:
//                  TRUE, if okay
//                  FALSE, if error
//******************************************************************************
BOOL SYM_EXPORT WINAPI MemCopyPhysical(LPVOID lpDest, LPVOID lpSrc, WORD wSize, UINT uFlags)
{
#if defined(SYM_WIN32) || defined(SYM_PROTMODE) || defined(SYM_VXD)

    if ( uFlags == 0 )
        uFlags = MCP_SRC_PHYSICAL | MCP_DEST_VIRTUAL;

    if ( uFlags & MCP_SRC_PHYSICAL )
        if ((lpSrc = GetProtModePtr(lpSrc)) == NULL)
            return(FALSE);

    if ( uFlags & MCP_DEST_PHYSICAL )
        if ((lpDest = GetProtModePtr(lpDest)) == NULL)
            return(FALSE);

    MEMCPY(lpDest, lpSrc, wSize);

    if ( uFlags & MCP_SRC_PHYSICAL )
        SelectorFree(FP_SEG(lpSrc));

    if ( uFlags & MCP_DEST_PHYSICAL )
        SelectorFree(FP_SEG(lpDest));

    return(TRUE);

#elif defined(SYM_OS2)
    HFILE       DevHandle;
    ULONG       ulAction;
    ULONG       DataLengthInOut;
    LDTParm     VidParm;
    LDTData     VidData;
    ULONG       ParmInOut;
    APIRET      rc;


    if ( uFlags == 0 )
        uFlags = MCP_SRC_PHYSICAL | MCP_DEST_VIRTUAL;

    rc = DosOpen(  "Screen$", &DevHandle, &ulAction, 0, 0,
                    OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                    OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, 0);

    VidParm.PhysAddr =  lpSrc;
    VidParm.Length   =  wSize;

    ParmInOut       = sizeof(VidParm);
    DataLengthInOut = 0;
    VidData.LDTOff  = 0;
    VidData.LDTSel  = 0;
    rc = DosDevIOCtl(DevHandle, 3, 0x75,// Function: Get LDT with offset
                     &VidParm, sizeof(VidParm), &ParmInOut, (PVOID) &VidData,
                     sizeof(VidData), &DataLengthInOut);

    if (rc != 0)
        return (FALSE);

    _asm {
        pushfd;
        push    es;
        push    edi;
        push    esi;
        push    ecx;
        push    ebx;
        push    eax;

        movzx   ecx,  wSize;             //  Repeat count
        xor     ebx, ebx;
        xor     eax, eax;
        xor     esi, esi;
        xor     edi, edi;
        push    ds;
        lds     si,  VidData;
        mov     eax, lpDest;           // Load dest into ES:DI
        push    eax;
        pop     edi;
        cld;                            //  Move up.
        rep     movsb;

        pop     ds;
        pop     eax;
        pop     ebx;
        pop     ecx;
        pop     esi;
        pop     edi;
        pop     es;
        popfd;
        }
    ParmInOut = sizeof(VidData.LDTSel);

    rc = DosDevIOCtl(DevHandle, 3, 0x71,// Function: Deallocate selector
                     &VidData.LDTSel, sizeof(VidData.LDTSel),
                     &ParmInOut, 0, 0, 0);
    DosClose(DevHandle);
    return (TRUE);

#elif defined(SYM_DOS)
                                        // For DOS, make sure we aren't stealthed
    if (!StealthCopy (lpDest, lpSrc, wSize))
        {
        MEMCPY(lpDest, lpSrc, wSize);
        }
    return(TRUE);

#elif defined(SYM_NLM)

    DWORD   dwBytesInBlock;
    DWORD   dwBytesRemaining = wSize;

    LPBYTE  lpWalkSrc = lpSrc;
    LPBYTE  lpWalkDest = lpDest;

    (void)  uFlags;

    // routine provides a copy of phyical memory for the caller - any memory
    // the NLM doesn't have access to will be filled with zeroes. This will
    // be changed in the future to provide access when it is possible for me
    // to modify the page tables.

    while (dwBytesRemaining > 0)
        {

        // handle any missing memory at the start of this section

        dwBytesInBlock = BytesToFirstValidPage( lpWalkSrc, dwBytesRemaining );
        if (dwBytesInBlock > 0)
            {

            MEMSET( lpWalkDest, MISSING_MEMORY, dwBytesInBlock );

            lpWalkDest += dwBytesInBlock;
            lpWalkSrc += dwBytesInBlock;

            dwBytesRemaining -= dwBytesInBlock;

            }
        else
            {

            // copy a present block of memory

            dwBytesInBlock = CountValidPageBytes( lpWalkSrc, dwBytesRemaining );
            if (dwBytesInBlock > 0)
                {

                MEMCPY( lpWalkDest, lpWalkSrc, dwBytesInBlock );

                lpWalkDest += dwBytesInBlock;
                lpWalkSrc += dwBytesInBlock;

                dwBytesRemaining -= dwBytesInBlock;

                }
            }
        }

    return(TRUE);

#endif

}

#if !defined(SYM_NLM) && !defined(SYM_VXD)

#ifdef SYM_OS2                          //  Functions Brad missed
FARPROC GetProcAddress(HINSTANCE hiHnd, LPCSTR pName)
{
   PFN   pfn;

   DosQueryProcAddr(    (HMODULE) hiHnd,
                        0,
                        pName,
                        &pfn);
   return pfn;

}

ULONG   GetTickCount()
//*************************************************************************
//***   Name:       GetTickCount() for OS2                              ***
//***   Author:     Tony McNamara                                       ***
//***   Created:    07-March-94                                         ***
//***   Purpose:    Returns timer ticks (PC-DOS standard).              ***
//***               (Modified from the 32-bit millisecond counter value ***
//***               returned by OS/2, for code compatibility.)          ***
//***               (Created solely for code simplicity.)               ***
//*************************************************************************
{
    ULONG       dwTicks;

    DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, &dwTicks, sizeof(dwTicks));
    return (dwTicks / (ULONG) 55);
}   //  ***     End of  OS2GetTicks()   ***


#endif

#ifdef SYM_WIN32

/*-----------------12-15-94 04:24pm-----------------
 MemAlloc uses GlobalAlloc to allocate a block of
 memory.  MemAlloc passes the handle returned from
 GlobalAlloc to GlobalLock to automatically lock that
 block of memory.  GlobalLock returns a pointer to
 that block of memory.  MemAlloc returns that pointer
 type-casted as a handle.  If GlobalAlloc or GlobalLock
 fail, MemAlloc will return a NULL.
--------------------------------------------------*/
HGLOBAL SYM_EXPORT WINAPI MemAlloc(UINT uFlags, DWORD dwBytes)
{
    auto    HGLOBAL hHandle;
    auto    LPVOID  lpVoid;

    hHandle = GlobalAlloc(uFlags, dwBytes);
    if ( hHandle == NULL )
        return (NULL);

    lpVoid = GlobalLock(hHandle);
    return (HGLOBAL)(lpVoid);
}

/*-----------------12-15-94 04:27pm-----------------
 MemFree is the counterpart of MemAlloc.  MemFree uses
 GlobalPtrHandle to convert the incoming pointer (casted
 as a handle) to a real handle(?).  GlobalUnlock uses
 the real handle to unlock the memory block and then
 GlobalFree frees the memory block.
--------------------------------------------------*/
HGLOBAL SYM_EXPORT WINAPI MemFree(HGLOBAL hHandle)
{
    hHandle = GlobalPtrHandle((LPVOID) hHandle);
    GlobalUnlock(hHandle);
    return GlobalFree(hHandle);
}

#endif  // ifdef WIN32

#endif  // ifndef SYM_NLM && ndef SYM_VXD

#endif // #if !defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// Define SYM_NTK and SYM_VXD implementations of MemAllocPtr(), MemFreePtr()
// and MemReAllocPtr().
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK) || defined(SYM_VXD)

#define NTK_MEM_TAG                 'SdaR'
#define PARANOID_MEM_TAG_START      '>=SR'
#define PARANOID_MEM_TAG_END        'SR=<'
#define PARANOID_MEM_TAG_START_FREE '>=sr'
#define PARANOID_MEM_TAG_END_FREE   'sr=<'
#define PARANOID_MEM_TAG_FILL       '-'

#if defined(SYM_MONITOR_MEMORY_USAGE)

#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static ULONG ulTotalPagedAllocated = 0;
static ULONG ulTotalNonPagedAllocated = 0;

#define DRIVER_NTK_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#endif

//---------------------------------------------------------------------------
//
// PVOID MemAllocPtr ( UINT uFlags, DWORD dwBytes );
//
// This is SYM_NTK and SYM_VXD specific implementation of this function.
//
// This function always allocates 2 * sizeof(DWORD) extra bytes and fills
// them with DWORD[0] = uFlags and DWORD[1] = dwBytes.  The returned pointer
// is the one right after the second DWORD.
//
// When SYM_PARANOID is defined, this function allocates another 2 *
// sizeof(DWORD) extra bytes.  In this case the first and the last DWORDs of
// the allocated memory are set to the values of PARANOID_MEM_TAG_START and
// PARANOID_MEM_TAG_END, respectively.  The memory after the first DWORD is
// then treated as specified above.  Also, if uFlags does not specify
// GMEM_ZERONINT, the newly allocated memory will be initialized to the value
// of PARANOID_MEM_TAG_FILL.
//
// Under SYM_NTK, a switch to the kernel stack is completed prior to making
// the call to ExAllocatePoolWithTag (EvAllocatePoolWithTag).  The support
// routines to switch stacks use per thread data which require heap allocations.
// Hence, a third parameter is added under the SYM_NTK platform which allow
// these support routines to bypass the stack switching logic.
//---------------------------------------------------------------------------
#if defined(SYM_NTK)
#undef MemAllocPtr
PVOID MemAllocPtr ( UINT uFlags, DWORD dwBytes ) {return MemAllocPtrEx (uFlags, dwBytes, FALSE);}
#define MemAllocPtr(f,b)     MemAllocPtrEx(f,b,FALSE)

PVOID MemAllocPtrEx ( UINT uFlags, DWORD dwBytes, BOOLEAN bIgnoreStack )
#else
PVOID MemAllocPtr  ( UINT uFlags, DWORD dwBytes )
#endif
    {
    auto   PVOID      pvMemory;

#if defined(SYM_MONITOR_MEMORY_USAGE)

#if defined(SYM_NTK)
    auto   KSPIN_LOCK SpinLock;
    static char       szDbg[] = "SYMKRNL memory allocated += %08X: Paged = %08X, NonPaged = %08X\n";
#elif defined(SYM_VXD)
    static char       szDbg[] = "SYMKRNL memory allocated += #EBX: Paged = #ECX, NonPaged = #EDX\r\n";
#endif

#endif

    SYM_ASSERT ( ( uFlags & (~(GMEM_MOVEABLE|GMEM_ZEROINIT|GMEM_SHARE)) ) == 0 );
    SYM_ASSERT ( dwBytes );

    dwBytes += 2 * sizeof(DWORD);

#if (defined(SYM_DEBUG) || defined(PRERELEASE)) && defined(SYM_PARANOID)
    dwBytes += 2 * sizeof(DWORD);
#endif

#if defined(SYM_NTK)

    if ( bIgnoreStack )
    {
        SYM_ASSERT ( IsSPInKernelStack() );

        pvMemory = ExAllocatePoolWithTag (
                   ( uFlags & GMEM_MOVEABLE ) ? PagedPool : NonPagedPool,
                   dwBytes,
                   NTK_MEM_TAG );
    }
    else
        pvMemory = EvAllocatePoolWithTag (
                   ( uFlags & GMEM_MOVEABLE ) ? PagedPool : NonPagedPool,
                   dwBytes,
                   NTK_MEM_TAG );

#elif defined(SYM_VXD)

    pvMemory = _HeapAllocate (
                   dwBytes,
                   ( uFlags & GMEM_ZEROINIT ) ? HEAPZEROINIT : 0 );

#endif

    if ( pvMemory )
        {
        if ( uFlags & GMEM_ZEROINIT )
            {
            SYM_VERIFY_BUFFER ( pvMemory, dwBytes );
#if !defined(SYM_VXD)
            MEMSET ( pvMemory, 0, dwBytes );
#endif
            }
#if (defined(SYM_DEBUG) || defined(PRERELEASE)) && defined(SYM_PARANOID)
        else
            {
            SYM_VERIFY_BUFFER ( pvMemory, dwBytes );
            MEMSET ( pvMemory, PARANOID_MEM_TAG_FILL, dwBytes );
            }

        dwBytes -= 2 * sizeof(DWORD);
        *((PDWORD)pvMemory)++ = PARANOID_MEM_TAG_START;
        *(PDWORD)( (PBYTE)pvMemory + dwBytes ) = PARANOID_MEM_TAG_END;

#endif

#if defined(SYM_MONITOR_MEMORY_USAGE)

#if defined(SYM_NTK)

        KeInitializeSpinLock ( &SpinLock );

        if ( uFlags & GMEM_MOVEABLE )
            {
            ExInterlockedAddUlong ( &ulTotalPagedAllocated, dwBytes - 2 * sizeof(DWORD), &SpinLock );
            }
        else
            {
            ExInterlockedAddUlong ( &ulTotalNonPagedAllocated, dwBytes - 2 * sizeof(DWORD), &SpinLock );
            }

        KdPrint (( szDbg,
                   dwBytes - 2 * sizeof(DWORD),
                   ulTotalPagedAllocated,
                   ulTotalNonPagedAllocated ));

#elif defined(SYM_VXD)

        if ( uFlags & GMEM_MOVEABLE )
            {
            _asm mov eax,[dwBytes]
            _asm sub eax,8 // 2 * sizeof(DWORD)
            _asm add [ulTotalPagedAllocated],eax
            }
        else
            {
            _asm mov eax,[dwBytes]
            _asm sub eax,8 // 2 * sizeof(DWORD)
            _asm add [ulTotalNonPagedAllocated],eax
            }

        _asm pushad
        _asm lea esi,[szDbg]
        _asm mov ebx,[dwBytes]
        _asm sub ebx,8 // 2 * sizeof(DWORD)
        _asm mov ecx,[ulTotalPagedAllocated]
        _asm mov edx,[ulTotalNonPagedAllocated]
        VMMCall(Out_Debug_String)
        _asm popad

#endif

#endif

        *((PDWORD)pvMemory)++ = uFlags;
        *((PDWORD)pvMemory)++ = dwBytes - 2 * sizeof(DWORD);
        }

    return ( pvMemory );
    }

//---------------------------------------------------------------------------
//
// PVOID MemFreePtr ( PVOID pvMemory );
//
// This is SYM_NTK and SYM_VXD specific implementation of this function.
//
// If SYM_PARANOID is defined, the tags of the memory block at pvMemory
// will be verified.  Refer to the documentation of MemAllocPtr() on the
// format of the memory allocations.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#undef MemFreePtr
PVOID MemFreePtr ( PVOID pvMemory ) {return MemFreePtrEx (pvMemory, FALSE);}
#define MemFreePtr(p)        MemFreePtrEx(p,FALSE)

PVOID MemFreePtrEx ( PVOID pvMemory, BOOLEAN bIgnoreStack )
#else
PVOID MemFreePtr  ( PVOID pvMemory )
#endif
    {
#if defined(SYM_MONITOR_MEMORY_USAGE)

#if defined(SYM_NTK)
    auto   KSPIN_LOCK SpinLock;
    static char       szDbg[] = "SYMKRNL memory allocated -= %08X: Paged = %08X, NonPaged = %08X\n";
#elif defined(SYM_VXD)
    static char       szDbg[] = "SYMKRNL memory allocated -= #EBX: Paged = #ECX, NonPaged = #EDX\r\n";
#endif
    auto   UINT       uFlags;
    auto   DWORD      dwBytes;

#endif

    SYM_ASSERT ( pvMemory );
    SYM_ASSERT ( *((PDWORD)pvMemory - 1) );
    SYM_VERIFY_BUFFER ( pvMemory, *((PDWORD)pvMemory - 1) );

    (PDWORD)pvMemory -= 2;

#if (defined(SYM_DEBUG) || defined(PRERELEASE)) && defined(SYM_PARANOID)
    (PDWORD)pvMemory -= 1;
    SYM_ASSERT ( *(PDWORD)pvMemory == PARANOID_MEM_TAG_START );
    *(PDWORD)pvMemory = PARANOID_MEM_TAG_START_FREE;
    SYM_VERIFY_BUFFER ( pvMemory, *((PDWORD)pvMemory + 2) + 4 * sizeof(DWORD) );
    SYM_ASSERT ( *(PDWORD)((PBYTE)pvMemory + *((PDWORD)pvMemory + 2) + 3 * sizeof(DWORD)) == PARANOID_MEM_TAG_END );
    *(PDWORD)((PBYTE)pvMemory + *((PDWORD)pvMemory + 2) + 3 * sizeof(DWORD)) = PARANOID_MEM_TAG_END_FREE;
#endif

#if defined(SYM_MONITOR_MEMORY_USAGE)

    uFlags  = *((PDWORD)pvMemory + 1);
    dwBytes = *((PDWORD)pvMemory + 2);

#if defined(SYM_NTK)

    KeInitializeSpinLock ( &SpinLock );

    if ( uFlags & GMEM_MOVEABLE )
        {
        ExInterlockedAddUlong ( &ulTotalPagedAllocated, 0-dwBytes, &SpinLock );
        }
    else
        {
        ExInterlockedAddUlong ( &ulTotalNonPagedAllocated, 0-dwBytes, &SpinLock );
        }

    KdPrint (( szDbg,
               dwBytes,
               ulTotalPagedAllocated,
               ulTotalNonPagedAllocated ));

#elif defined(SYM_VXD)

    if ( uFlags & GMEM_MOVEABLE )
        {
        _asm mov eax,[dwBytes]
        _asm sub [ulTotalPagedAllocated],eax
        }
    else
        {
        _asm mov eax,[dwBytes]
        _asm sub [ulTotalNonPagedAllocated],eax
        }

        _asm pushad
        _asm lea esi,[szDbg]
        _asm mov ebx,[dwBytes]
        _asm mov ecx,[ulTotalPagedAllocated]
        _asm mov edx,[ulTotalNonPagedAllocated]
        VMMCall(Out_Debug_String)
        _asm popad

#endif

#endif

#if defined(SYM_NTK)

    if ( bIgnoreStack )
    {
        SYM_ASSERT ( IsSPInKernelStack() );

        ExFreePool ( pvMemory );
    }
    else
        EvFreePool ( pvMemory );

    return ( NULL );

#elif defined(SYM_VXD)

    return ( _HeapFree ( pvMemory, 0 ) ? NULL : pvMemory );

#endif
    }

//---------------------------------------------------------------------------
//
// PVOID MemReAllocPtr (
//      PVOID pvMemory,
//      DWORD dwBytes,
//      UINT  uFlags
//      );
//
// This is SYM_NTK and SYM_VXD specific implementation of this function.
// It will fail if !dwBytes, or if GMEM_MODIFY or GMEM_NOCOMPACT flag is
// specified.
//
//---------------------------------------------------------------------------
PVOID MemReAllocPtr (
    PVOID pvMemory,
    DWORD dwBytes,
    UINT  uFlags
    )
    {
    auto PVOID pvNewBlock;
    auto DWORD dwOldSize;

    SYM_ASSERT ( dwBytes );
    SYM_ASSERT ( (uFlags & GMEM_MODIFY) == 0 );
    SYM_ASSERT ( (uFlags & GMEM_NOCOMPACT) == 0 );

    if ( !dwBytes ||
         ( uFlags & GMEM_MODIFY ) != 0 ||
         ( uFlags & GMEM_NOCOMPACT ) != 0 )
        {
        SYM_ASSERT ( FALSE );
        return ( NULL );
        }

    SYM_ASSERT ( pvMemory );
    SYM_ASSERT ( ( uFlags & (~(GMEM_MOVEABLE|GMEM_ZEROINIT|GMEM_SHARE)) ) == 0 );
    SYM_ASSERT ( *((PDWORD)pvMemory - 1) );

    pvNewBlock = MemAllocPtr ( uFlags, dwBytes );

    if ( pvNewBlock )
        {
#if (defined(SYM_DEBUG) || defined(PRERELEASE)) && defined(SYM_PARANOID)
        (PDWORD)pvMemory -= 3;
        SYM_ASSERT ( *(PDWORD)pvMemory == PARANOID_MEM_TAG_START );
        SYM_VERIFY_BUFFER ( pvMemory, *((PDWORD)pvMemory + 2) + 4 * sizeof(DWORD) );
        SYM_ASSERT ( *(PDWORD)((PBYTE)pvMemory + *((PDWORD)pvMemory + 2) + 3 * sizeof(DWORD)) == PARANOID_MEM_TAG_END );
        (PDWORD)pvMemory += 3;
#endif

        dwOldSize = *((PDWORD)pvMemory - 1);

        SYM_VERIFY_BUFFER ( pvMemory, dwOldSize );

        MEMCPY ( pvNewBlock,
                 pvMemory,
                 dwBytes > dwOldSize ? dwOldSize : dwBytes );

        MemFreePtr ( pvMemory );
        }

    return ( pvNewBlock );
    }

#endif // #if defined(SYM_NTK) || defined(SYM_VXD)
