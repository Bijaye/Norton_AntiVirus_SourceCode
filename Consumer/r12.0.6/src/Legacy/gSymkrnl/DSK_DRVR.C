/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/Dsk_drvr.c_v   1.2   21 Aug 1998 16:34:08   MBROWN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *	DiskFindDPB							*
 *      DiskGetDriverTable                                              *
 *      DiskGetDriverTablePtr                                           *
 *      DiskIsBigTable                                                  *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/Dsk_drvr.c_v  $ *
// 
//    Rev 1.2   21 Aug 1998 16:34:08   MBROWN
// Ported in a couple SKURTZ bug fixes from L branch
// 
//    Rev 1.1   19 Mar 1997 21:43:48   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.0   26 Jan 1996 20:22:18   JREARDON
// Initial revision.
// 
//    Rev 1.36   15 Jan 1996 15:29:20   RTROLLO
// Initialize clRootSCN to 0 in FAT16&12 cases
//
//    Rev 1.35   09 Jan 1996 15:26:40   DBUCHES
// Fixed CanSupportExtendedIO for Win32.  Now uses DOSVersion.
//
//    Rev 1.34   20 Dec 1995 17:43:14   DBUCHES
// Fixed DiskGetDriverTablePtr().  Need to make sure selector is allocated before we free it.
//
//    Rev 1.33   15 Dec 1995 14:06:26   DBUCHES
// Changes for 32 Bit FAT support.
//
//    Rev 1.32   13 Dec 1995 14:06:50   DBUCHES
// 32 Bit FAT work in progress.
//
//    Rev 1.31   04 Dec 1995 16:00:10   MARKK
// Work for 32 bit FAT
//
//    Rev 1.30   02 Aug 1995 01:08:58   AWELCH
// Merge changes from Quake 7.
//
//    Rev 1.29   19 Jun 1995 20:46:56   HENRI
// Convert lpDrivePtr in DiskGetDriverTable to a NORMALIZED seg/off real mode ptr
//
//    Rev 1.28   19 Jun 1995 19:58:30   HENRI
// MemCopyPhysical calls for values below 0xA0000 assume a linear offset (not seg/offset)
//
//    Rev 1.27   25 Apr 1995 12:58:42   BRAD
// Added NT support
//
//    Rev 1.26   21 Apr 1995 11:24:02   TONY
// If DiskFindDPB() detects it's in an OS/2 VDM, it simply returns FALSE.
//
//    Rev 1.25   11 Apr 1995 12:48:20   HENRI
// Merge changes from branch 6
//
//    Rev 1.24   28 Feb 1995 20:12:24   BARRY
// Made compile for DOS platform
//
//    Rev 1.23   27 Feb 1995 14:38:34   HENRI
// Removed thunking for DiskGetInfo & DiskGetDriverTablePtr
//
//    Rev 1.22   04 Oct 1994 16:13:16   DBUCHES
// Added thunk for DiskGetDriverTable
//
//    Rev 1.21   10 May 1994 12:29:18   MARKK
// DOSX work
//
//    Rev 1.20   09 May 1994 14:38:24   MARKK
// DOSX work
//
//    Rev 1.19   09 May 1994 09:18:48   MARKK
// DOSX work
//
//    Rev 1.18   29 Apr 1994 17:15:18   MARKK
// DOSX work
//
//    Rev 1.17   26 Apr 1994 17:51:18   BRAD
// Use SYM_MAX_PATH only
//
//    Rev 1.16   15 Mar 1994 12:33:26   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.15   25 Feb 1994 15:02:54   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.13   02 Jul 1993 08:51:52   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.12   15 Feb 1993 21:03:56   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.11   15 Feb 1993 08:24:30   ED
// Removed local copies of functions that used to be in other DLLs.  Now
// that we are one big happy family, everything is "local".
//
//    Rev 1.10   12 Feb 1993 04:27:54   ENRIQUE
// No change.
//
//    Rev 1.9   22 Dec 1992 15:29:12   JOHN
// Fixed the fix of DiskFindDPB()
//
//    Rev 1.8   18 Dec 1992 14:48:40   DAVID
// Fixed Windows version of DiskFindDPB()
//
//    Rev 1.7   18 Dec 1992 03:15:26   JOHN
// Added DiskFindDPB()
//
//    Rev 1.6   19 Oct 1992 17:02:10   BILL
// Added changes to handle const typedef
//
//    Rev 1.5   15 Sep 1992 10:11:14   ED
// Removed NSTRING.H
//
//    Rev 1.4   10 Sep 1992 19:05:26   BRUCE
// Deleted commented out includes
//
//    Rev 1.3   09 Sep 1992 18:18:38   BRUCE
// Eliminated dependency on nwinutil.h
//
//    Rev 1.2   09 Sep 1992 14:21:48   HENRI
// Removed code from DOS platform because it was only
// needed under Windows.
//
//    Rev 1.1   08 Sep 1992 12:46:58   HENRI
// Removed use of __DirGet for the DOS platform.
//
//    Rev 1.0   27 Aug 1992 10:33:34   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#include "undoc.h"
#include "file.h"
#include "disk.h"
#include "xapi.h"

BOOL LOCAL DiskGetDPBEx(BYTE dl, LPSHORTDISKREC lpBuffer);

#ifndef SYM_WIN32
/*----------------------------------------------------------------------*
 * DiskFindDPB()							*
 *									*
 * Finds the address of the DPB for drive 'dl'.  This function walks	*
 * the DPB chain to find the address; it doesn't call INT 21/AH=32.	*
 * This avoids any disk I/O, but does mean that the information in	*
 * the DPB may not be valid.						*
 *									*
 * Returns FALSE if drive 'dl' is invalid, otherwise TRUE.		*
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskFindDPB(BYTE dl, LPDISKREC FAR *lplpDPB)
{
    auto	LPBYTE		lpCVT;
    auto	LPBYTE		lpDPB;
    auto	LPBYTE		lpRealModeDPB;
    auto	UINT		uNextPointerOffset;
    auto	BYTE		byDriveNum;
    auto    BYTE        byMajor = 0;
#ifdef SYM_PROTMODE
    auto	UINT		wSelector;
#endif
                                        //  Check for OS/2.  If OS/2, this often winds up in
                                        //  an infinite loop.
#if defined( SYM_DOS ) || defined(SYM_WIN)
        _asm {
            push    si
            push    di
            push    es
            push    ds
            push    bp
            mov     ax, 03000h
            int     21h
            mov     [byMajor], al       ;   Real OS version, returns >= 10 for OS/2
            pop     bp
            pop     ds
            pop     es
            pop     di
            pop     si
            }
    if (byMajor >= 10) return ( FALSE );    //  Under OS/2, return FALSE, DPB bad.
#endif
    *lplpDPB = NULL;

    byDriveNum = (BYTE) (CharToUpper(dl) - 'A');

    // Pointer to next DPB is at slightly different location in DOS 4.0+
    uNextPointerOffset = (DOSGetVersion() < 0x400) ? 0x0018 : 0x0019;

    // Get real or protected mode pointer to CVT
    DOSGetInternalVariables(&lpCVT);

    // Pointer to first DPB is the first DWORD in CVT

#ifdef SYM_PROTMODE
    // Get protected mode pointer to DPB (convert segment into selector)
    if (SelectorAlloc(FP_SEG(lpCVT), 0xFFFF, &wSelector))
	return(FALSE);
    FP_SEG(lpCVT) = wSelector;
#endif

    lpRealModeDPB = * (LPBYTE FAR *) lpCVT;

#ifdef SYM_PROTMODE
    SelectorFree(wSelector);
#endif

    // Walk DPB chain
    while (FP_OFF(lpRealModeDPB) != 0xFFFF)
	{
#ifdef SYM_PROTMODE
	// Get protected mode pointer to DPB (convert segment into selector)
	if (SelectorAlloc(FP_SEG(lpRealModeDPB), 0xFFFF, &wSelector))
	    return(FALSE);
	FP_SEG(lpDPB) = wSelector;
        FP_OFF(lpDPB) = FP_OFF(lpRealModeDPB);
#else
        lpDPB = (LPBYTE)lpRealModeDPB;
#endif

	// Have we found the drive we're looking for?
	if (*lpDPB == byDriveNum)
	    {
#ifdef SYM_PROTMODE
	    SelectorFree(wSelector);
#endif
	    *lplpDPB = (LPDISKREC) lpRealModeDPB;
	    return(TRUE);
	    }

	// Advance to next entry in chain
	lpRealModeDPB = * (LPBYTE FAR *)(lpDPB + uNextPointerOffset);

#ifdef SYM_PROTMODE
	SelectorFree(wSelector);
#endif
	}

    return(FALSE);
}

#endif //SYM_WIN32


/*----------------------------------------------------------------------*
 *  This module will get a disk table that will be used when performing	*
 *	absolute DOS sector reads, using the Undocumented DOS function	*
 *	that returns the disk driver characteristics.			*
 *									*
 *  Returns:								*
 *	TRUE if info gotten okay, else FALSE				*
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskGetDriverTable (BYTE dl, LPDISKREC lpDiskPtr)
{
    auto        ShortDiskRec FAR *lpDrivePtr;
    auto        DWORD   dwStartOfRootDir;
    auto        DWORD   dwSectorsPerFat;
    auto        BOOL 	bOK;
    register 	UINT	wOffset;
#ifdef SYM_WIN32
    auto        DWORD   dwRealMode;
#endif
#ifdef  SYM_PROTMODE
    auto    UINT    wSelector = 0;
#endif
    auto    BOOL    bExtendedIO;


    dl = (BYTE) CharToUpper(dl);

#ifdef	SYM_WIN32
                                        // If NT, we get most of the drive
                                        // info, but not the layout info
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
        char    szDrive[10];
        DWORD   dwSectorsPerCluster, dwBytesPerSector;
        DWORD   dwFreeClusters, dwClusters;


        STRCPY(szDrive, "x:\\");
        szDrive[0] = dl;
        if (GetDiskFreeSpace(szDrive, &dwSectorsPerCluster,
                         &dwBytesPerSector, &dwFreeClusters,
                         &dwClusters))
            {
            MEMSET(lpDiskPtr, 0, sizeof(*lpDiskPtr));
            lpDiskPtr->dn = dl - 'A';
            lpDiskPtr->dl = dl;
            lpDiskPtr->bytesPerSector = (WORD) dwBytesPerSector;
            lpDiskPtr->sectorsPerCluster = (BYTE) dwSectorsPerCluster;
            lpDiskPtr->dwMaxClusterEx = dwClusters - 1;
            if (lpDiskPtr->dwMaxClusterEx < (UINT)0x0FF6)
                lpDiskPtr->byFATType = DR_FAT12;
            else if (lpDiskPtr->dwMaxClusterEx < 0xFFFF)
                lpDiskPtr->byFATType = DR_FAT16;
            else
                lpDiskPtr->byFATType = DR_FAT32;
            lpDiskPtr->bytesPerCluster = dwBytesPerSector * dwSectorsPerCluster;

            return(TRUE);
            }
        else
            return(FALSE);
        }
#endif

    bExtendedIO = CanSupportExtendedIO();

    if( bExtendedIO )
        {
        auto    BYTE    buffer[sizeof(ShortDiskRec) + sizeof (WORD)];
        auto    LPBYTE  lpBy;

                    /*   Get Driver info thru DOS       */
                    /*   using INT 21h, function 7302h  */

        bOK = DiskGetDPBEx(dl, (LPSHORTDISKREC) buffer);


		if (!bOK)
			{
			goto UseNoneExtended;
			}

                                        // 1st WORD of buffer contains
                                        // the size of data returned.
                                        // skip over this.
        lpBy = &buffer[sizeof(WORD)];
        lpDrivePtr = (LPSHORTDISKREC) lpBy;

                                        // Fill in DiskRec structure.
        ShortDiskRecToLongDiskRec((LPSHORTDISKREC)lpDrivePtr, lpDiskPtr);
        }
    else
        {
UseNoneExtended:

					/* Get Driver info thru DOS	  */
					/*   using INT 21h, function 32h  */
        bOK = DiskGetDriverTablePtr (dl, (ShortDiskRec FAR * FAR*) &lpDrivePtr);

					/* If error, return FALSE	*/
        if (!bOK)
            return(FALSE);

#ifdef  SYM_PROTMODE
					/* Get a selector		*/
        if (SelectorAlloc(FP_SEG(lpDrivePtr), 0xFFFF, &wSelector))
            return(FALSE);

                        /* Change pointer so uses selector */
        FP_SEG(lpDrivePtr) = wSelector;
#endif

					/* Move into local buffer	*/
#ifdef	SYM_WIN32
        if ( (DWORD)lpDrivePtr < 0xA0000 )
            {
            auto    ShortDiskRec    sDr;
                                            // Normalize to a seg/offset real mode ptr
            dwRealMode = (( (DWORD) lpDrivePtr & 0x000FFFF0) << 12) + ((DWORD) lpDrivePtr & 0x0000000F);
            MemCopyPhysical(&sDr, (LPBYTE) dwRealMode, sizeof(ShortDiskRec), 0);

            ShortDiskRecToLongDiskRec(&sDr, lpDiskPtr);
            }
        else
#endif
            ShortDiskRecToLongDiskRec((LPSHORTDISKREC)lpDrivePtr, lpDiskPtr);

        }

					/* Adjust to actual amount	*/
    (lpDiskPtr->sectorsPerCluster)++;

					/* Get media descriptor offset,	*/
					/*   and also compute start of	*/
					/*   root and sectors/Fat	*/
    wOffset = (DiskIsBigTable (lpDiskPtr, &dwStartOfRootDir, &dwSectorsPerFat,
    				&bOK)) ? 0x17 : 0x16;

					/* Set these 2 fields		*/
    lpDiskPtr->startOfRootDir = (WORD)dwStartOfRootDir;

    lpDiskPtr->dwSectorsPerFATEx = dwSectorsPerFat;
					/* Initialize the media		*/
					/*    descriptor BYTE		*/
#ifdef	SYM_WIN32
    if ( (DWORD)lpDrivePtr < 0xA0000 )
        {
                                        // Normalize to a seg/offset real mode ptr
                                        // Normalize to a seg/offset real mode ptr
        MemCopyPhysical(&lpDiskPtr->mediaDescriptor, (((LPBYTE)dwRealMode) + wOffset), sizeof(BYTE), 0);
        }
    else
#endif
    lpDiskPtr->mediaDescriptor = *(((LPBYTE)lpDrivePtr) + wOffset);

    					/* Free the selector		*/
#ifdef  SYM_PROTMODE
    if(wSelector)
        SelectorFree(wSelector);
#endif
    return (bOK);
}


/*----------------------------------------------------------------------*/
/* DiskGetDriverTablePtr						*/
/*	Gets the internal drive table ptr. for a drive.			*/
/*									*/
/*	RETURNS:							*/
/*		TRUE		if successful				*/
/*		FALSE		if error				*/
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskGetDriverTablePtr(BYTE drive, ShortDiskRec FAR * FAR *lpDrivePtr)
{
    auto	BOOL		bReturn = FALSE;

#ifdef SYM_WIN32
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
#else	
    auto        SYM_REGS        regs;

    drive = (BYTE) CharToUpper(drive);       // Make sure this is upper-case

    regs.EAX = 0x00003200;
    regs.EBX = 0x0000FFFF;
    regs.EDX = (drive - ('A' - 1));
    VxDIntBasedCall( 0x21, &regs );

#define CARRY_FLAG     0x00000001

    if (!(regs.EFlags & CARRY_FLAG || (regs.EBX == 0x0000FFFF) || (regs.EAX == 0x000000FF)))
	{
        *lpDrivePtr = (LPSHORTDISKREC) regs.EBX;
	bReturn = TRUE;
	}
#endif // _M_ALPHA
#else

#ifdef  SYM_PROTMODE
    auto	union REGS	regs;
    auto	struct SREGS	sregs;
#ifdef  SYM_WIN
    auto        UINT            wErr;
    auto        char            szPathSpec[SYM_MAX_PATH];
    auto        UINT            wPrevErr;

    drive = (BYTE) CharToUpper(drive);       // Make sure this is upper-case

                                        // Check to see if we're in standard
                                        // mode, since there appears to be
                                        // a bug in the DPMI driver if a
                                        // critical error (int24h) occurs
                                        // during a call Int21h,Fcn32h.
                                        // We need to surface the critical
                                        // error before making this call!
                                        // Only do this if we have a floppy
                                        // drive.
    if ((GetWinFlags() & WF_STANDARD) && !DiskIsFixed(drive))
        {
                                        // NOTE: We can't call DiskValidCheck
                                        // since it is in NWIN.DLL and we
                                        // don't want any more cross-linking
        wErr = 0;

        wPrevErr = SetErrorMode(1);

        if (!DiskIsCDRom(drive))
            wErr = DirGet (drive, szPathSpec);
        else
            wErr = VolumeLabelGet (drive, szPathSpec);

        SetErrorMode(wPrevErr);

        if (wErr == -1)                  // If fatal error occured, return
            return (bReturn);
        }
#endif /* SYM_WIN16 */

    regs.h.ah = 0x32;
    regs.x.bx = 0xFFFF;
    regs.h.dl = (BYTE) (drive - ('A' - 1));
    IntWin(0x21, &regs, &sregs);
    if (!(regs.x.cflag || (regs.x.bx == 0xFFFF) || (regs.h.al == 0xFF)))
	{
    	*lpDrivePtr = MAKELP (sregs.ds, regs.x.bx);
	bReturn = TRUE;
	}
#else
    _asm
        {
        push    ds
        push    di

        mov     ah, 32h
        mov     bx, 0FFFFh
        mov     dl, drive
        sub     dl, 'A' - 1
        int     21h
        jc      Is_Error
        cmp     bx, 0FFFFh
        jz      Is_Error
        cmp     al, 0FFh
        jz      Is_Error

        les     di, lpDrivePtr
        mov     es:[di], bx
        mov     es:[di+2], ds
        inc     bReturn
Is_Error:
        pop     di
        pop     ds
        }

#endif /* SYM_PROTMODE */

#endif /* SYM_WIN32    */

    return(bReturn);
}


BOOL LOCAL DiskGetDPBEx(BYTE drive, LPSHORTDISKREC lpBuffer)
{
#ifdef SYM_WIN32
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
    auto        SYM_REGS        regs;

    drive = (BYTE) CharToUpper(drive);          // Make sure this is upper-case

    MEMSET(&regs, 0, sizeof(SYM_REGS));

                                        // Int 21, function 7302
                                        // Get Extended DPB.
    regs.EAX = 0x00007302;
    regs.EDI = (DWORD)lpBuffer;
    regs.ECX = sizeof(ShortDiskRec) + sizeof (WORD);
    regs.EDX = (drive - 'A') + 1;

    VxDIntBasedCall( 0x21, &regs );
                                        // Check Carry.
    if (regs.EFlags & 0x00000001)
        return(FALSE);
    else
        return(TRUE);
#endif // _M_ALPHA
#elif defined(SYM_PROTMODE)
    auto    union   REGS  regs;
    auto    struct  SREGS    sregs;
    auto    DWORD   dwSegSel;
    auto    BOOL    bRet = FALSE;

    dwSegSel = (DWORD) GlobalDosAlloc(sizeof(ShortDiskRec) + sizeof (WORD));

    if(!dwSegSel)
        return(FALSE);


    drive = (BYTE) CharToUpper(drive);          // Make sure this is upper-case

    MEMSET(&regs, 0, sizeof(regs));
    MEMSET(&sregs, 0, sizeof(sregs));

    regs.x.ax = 0x7302;
    regs.x.dx = (drive - 'A') + 1;
    regs.x.cx = sizeof(ShortDiskRec) + sizeof (WORD);
    sregs.es  = HIWORD(dwSegSel);
    regs.x.di   = 0x0;
                                        // Int 21, function 7302
                                        // Get Extended DPB.
    IntWin(0x21, &regs, &sregs);
                                        // Check Carry
    if (!(regs.x.cflag))
        {
        MEMCPY(lpBuffer, MAKELP(LOWORD(dwSegSel), 0), sizeof(ShortDiskRec) + sizeof (WORD));
        bRet = TRUE;
        }

    GlobalDosFree(LOWORD(dwSegSel));
    return(bRet);

#else

    auto    WORD    wSize = sizeof(ShortDiskRec) + sizeof(WORD);
    auto    BOOL    bRet;
	auto	BYTE    driveNum;

    drive= (BYTE) CharToUpper(drive);          // Make sure this is upper-case
	driveNum = (drive - 'A') + 1;			   // drive number is 1 based.

    _asm
        {
        push    ds
        push    di

        mov ax, 7302h               ;Int 21, function 7302 GetExtDPB

		mov  dl,driveNum

        mov cx, wSize               ;size of buffer;

        les di, [lpBuffer]          ;Get addr of buffer.

        int 21h

        mov bRet, 1
        jnc noerror
        mov bRet, 0

noerror:
        pop     di
        pop     ds
        }

    return(bRet);
#endif

}
#pragma optimize("", on)


/*----------------------------------------------------------------------*
 *  Computes the 'startOfRootDir' and the 'SectorsPerFat' fields of the	*
 *	variable structure that DOS uses internally to keep track of	*
 *	disk characteristics.						*
 *									*
 *  Returns TRUE if the table is using the DOS4 type, else returns FALSE*
 *									*
 *  A flag is set to TRUE if the information in the table looks correct,*
 *	else this flag is set to FALSE.					*
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskIsBigTable (LPCDISKREC diskPtr, DWORD FAR * startOfRootPtr,
                         DWORD FAR * sectorsPerFatPtr, UINT FAR * okPtr)
{
    auto    DWORD    startOfRootDir;
    auto    DWORD    sectorsPerFat;


                        /* Set to invalid value for now */
    *startOfRootPtr = *sectorsPerFatPtr = 0;

    					/* Is this a valid table?	*/
    if (!(diskPtr->bytesPerSector) || !(diskPtr->numberFats))
        {
    *okPtr = FALSE;     /* No, return to user       */
    return (FALSE);     /* Use DOS 3 table format   */
    }

    *okPtr = TRUE;      /* Info is valid        */

                        /* FAT32 disks use a DOS 4 comapatible DPB */
    if(IsFAT32(*diskPtr))
        {
        *sectorsPerFatPtr = diskPtr->dwSectorsPerFATEx;
        *startOfRootPtr   = diskPtr->startOfRootDir;
        return(TRUE);
        }

                        /* Compute start of root dir    */
    *startOfRootPtr = startOfRootDir = diskPtr->dwStartOfDataEx -
    		      (diskPtr->maxRootEntries /
		      (diskPtr->bytesPerSector / sizeof (DIRENTRYREC)));

    					/* Compute sectors per Fat	*/
    *sectorsPerFatPtr = sectorsPerFat = (startOfRootDir - diskPtr->startOfFat)
    			/ diskPtr->numberFats;

                        /* If offsets are in UINT   */
                        /*   fields,then DOS 4.0 table  */
    return ((BOOL) ((startOfRootDir == diskPtr->startOfRootDir) &&
                     (sectorsPerFat  == diskPtr->dwSectorsPerFATEx)));
}

void SYM_EXPORT WINAPI ShortDiskRecToLongDiskRec(LPSHORTDISKREC lpsDr, LPDISKREC lpDiskPtr)
{
                                        // Copy over common values.
    lpDiskPtr->dn                = lpsDr->driveNumber;
    lpDiskPtr->driverEntry       = lpsDr->driverEntry;
    lpDiskPtr->bytesPerSector    = lpsDr->bytesPerSector;
    lpDiskPtr->sectorsPerCluster = lpsDr->sectorsPerCluster;
    lpDiskPtr->reserved          = lpsDr->reserved;
    lpDiskPtr->startOfFat        = lpsDr->startOfFat;
    lpDiskPtr->numberFats        = lpsDr->numberFats;
    lpDiskPtr->maxRootEntries    = lpsDr->maxRootEntries;
    lpDiskPtr->startOfRootDir    = lpsDr->startOfRootDir;

    if( lpsDr->sectorsPerFat )          // If this value is non-zero
        {                               // then we are on a FAT12/16 volume
                                        // so the values in the old DPB
                                        // are correct, so copy them over.
        lpDiskPtr->dwStartOfDataEx   = lpsDr->startOfData;
        lpDiskPtr->dwMaxClusterEx    = lpsDr->maxCluster;
        lpDiskPtr->dwSectorsPerFATEx = lpsDr->sectorsPerFat;
        lpDiskPtr->byFATType         = (lpDiskPtr->dwMaxClusterEx < (UINT)0x0FF6) ?
                                        DR_FAT12 : DR_FAT16;
        lpDiskPtr->byIOType          = IsFAT16(*lpDiskPtr) ? DR_IO_BIGFOOT : DR_IO_NORMAL;
                                        // We need to initaliaze rootSCN to 0
                                        // so that users don't get confused. - ROWANT
        lpDiskPtr->clRootSCN         = 0;

        }
    else                                // FAT32 volume
        {
                                        // Get proper values from new extended
                                        // DPB.
        auto    LPSHORTDISKREC    lpDPB = (LPSHORTDISKREC) lpsDr;

        lpDiskPtr->dwStartOfDataEx      = lpDPB->dwStartOfData;
        lpDiskPtr->dwMaxClusterEx       = lpDPB->dwMaxCluster;
        lpDiskPtr->dwSectorsPerFATEx    = lpDPB->dwSectorsPerFAT;
        lpDiskPtr->clRootSCN            = lpDPB->dwFirstRootCluster;
        lpDiskPtr->byFATType            = DR_FAT32;
        lpDiskPtr->byIOType             = DR_IO_EXTENDED;
        }

}

//////////////////////////////////////////////////////////////////////////
//CanSupportExtendedIO
//
//
//RETURNS: TRUE if we can use the Nashville Extended Int21 calls for Disk
//              IO.
//
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   12-15-95 01:36:53pm
//////////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI CanSupportExtendedIO()
{
    return( (BOOL) (DOSGetVersion() >= DOS_7_10) );

}


