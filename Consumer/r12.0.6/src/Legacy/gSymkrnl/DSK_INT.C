/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/dsk_int.c_v   1.8   11 May 1998 18:27:32   SKURTZ  $ *
 *                                                                      *
 * Description:                                                         *
 *      The new and improved set of routines used by                    *
 *      DiskMapLogToPhyParams() to map logical drive info to physical   *
 *      drive info.                                                     *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/dsk_int.c_v  $ *
// 
//    Rev 1.8   11 May 1998 18:27:32   SKURTZ
// Refine test for Int13x support.
//
//    Rev 1.7   24 Mar 1998 13:37:20   MBROWN
// Additional work on the DOS block of DiskMapLogToPhyParams
//
//    Rev 1.6   23 Mar 1998 14:58:00   MBROWN
// Fixed DOS block of DiskMapLogToPhyParams
//
//    Rev 1.5   18 Mar 1998 11:43:28   SKURTZ
// Fix compiler errors because of new absdiskrec members
//
//    Rev 1.4   11 Mar 1998 15:05:34   SKURTZ
// Numerous changes for Extended Int13 support
//
//    Rev 1.3   19 Mar 1997 21:44:52   BILL
// Modified files to build properly under Alpha platform
//
//    Rev 1.2   11 Dec 1996 17:57:46   SKURTZ
// Uncommented NEC calls
//
//    Rev 1.1   15 Oct 1996 12:30:40   SKURTZ
// Selectively merging in changes from Quake C
//
//    Rev 1.0   26 Jan 1996 20:23:20   JREARDON
// Initial revision.
//
//    Rev 1.43   12 Jan 1996 16:22:36   RANSCHU
// If HWIsNEC check for valid disk types (as soon as the matching code exists).
//
//    Rev 1.42   15 Dec 1995 14:15:48   DBUCHES
// Fixed DX compile warning.
//
//    Rev 1.41   15 Dec 1995 14:06:28   DBUCHES
// Changes for 32 Bit FAT support.
//
//    Rev 1.40   06 Dec 1995 12:15:38   BILL
// Fixed unreferenced variable warning
//
//    Rev 1.39   02 Aug 1995 01:03:02   AWELCH
// Merge changes from Quake 7.
//
//    Rev 1.38.1.1   24 May 1995 21:29:08   TORSTEN
// Fix to allow the INT13 handler to be overlayed with .RTLink/Plus.
//
//    Rev 1.38.1.0   24 Apr 1995 19:46:52   SCOTTP
// Branch base for version QAK7
//
//    Rev 1.38   11 Apr 1995 12:54:22   HENRI
// Merge changes from branch 6
//
//    Rev 1.37   12 Mar 1995 13:08:08   MARKK
// Fixed problem with DX
//
//    Rev 1.36   27 Dec 1994 15:05:40   BRUCE
// Fixed #ifdefs in diskmaplogtophyparam
//
//    Rev 1.35   27 Dec 1994 13:39:44   SKURTZ
// DX work.  Problems calling a real mode int 13 handler from Protected Mode
// code.  Now, DiskMapLogToPhyParams() allocates some real mode memory and copies
// code (residing in int13dos.asm) into this block.  The real mode int 13 vector
// is pointed to this memory block.
//
//    Rev 1.34   20 Dec 1994 21:08:32   DBUCHES
// DiskMapLogToPhyParam() fix for W32.
//
//    Rev 1.33   26 Oct 1994 20:36:26   BILL
// Added include needed for disk.h
//
//
//    Rev 1.32   25 Oct 1994 19:34:30   HENRI
// Now using the common IOCtlVWin32 for all WIN32 IOCTL functions
//
//    Rev 1.31   21 Oct 1994 20:38:20   DBUCHES
// The DiskMapLogToPhyParam saga continues.  It seems that the Microsoft
// GetDriveMapping DeviceIoControl will always return an Int13 device
// number (often bogus) even if the drive specified is a device driven
// drive.  We now validate the device number returned.
//
//    Rev 1.30   03 Oct 1994 10:36:44   DBUCHES
// Changed DiskGetPhysicalType so that the .generated flag is set based
// on whether the logical drive actually maps to an int 13 device.
//
//    Rev 1.29   31 Aug 1994 16:00:34   DBUCHES
// Fixed DiskMapLogToPhyParams().  We were taking the HIWORD instead of
// the LOWORD for lpRealBuffer on the DX/WIN16 platforms.
//
//
//
//    Rev 1.28   24 Aug 1994 15:00:16   DLEVITON
// Fixed problem making for WIN16.
//
//    Rev 1.27   24 Aug 1994 10:36:26   DBUCHES
// No change.
//
//    Rev 1.26   24 Aug 1994 10:34:58   DBUCHES
//
//    Rev 1.25   23 Jun 1994 17:22:28   HENRI
// Moved 16 bit thunk layers to a separate DLL
//
//    Rev 1.24   22 Jun 1994 12:26:00   HENRI
// Added thunking layers
//
//    Rev 1.23   21 Jun 1994 11:26:44   MARKK
// Change DOSX to Rational
//
//    Rev 1.22   09 Jun 1994 12:31:32   MARKK
// Removed some rational code
//
//    Rev 1.21   01 Jun 1994 09:17:38   MARKK
// Moved the int13 handler to C for DOSX
//
//    Rev 1.20   05 May 1994 13:24:42   MARKK
// DOSX work
//
//    Rev 1.19   04 May 1994 05:51:34   MARKK
// DOSX work
//
//    Rev 1.18   26 Apr 1994 12:06:34   BRAD
// Added DiskMapLogToPhyParams() and DiskGetBootDrive() to Win32s thunks
//
//    Rev 1.17   22 Apr 1994 12:58:22   BRAD
// Added another parameter to DiskAbsBuffer*
//
//    Rev 1.16   22 Apr 1994 12:04:20   BRAD
// Added DiskAbsBuffer* routines
//
//    Rev 1.15   18 Apr 1994 17:40:42   BRAD
// Start at 8K and go down
//
//    Rev 1.14   18 Apr 1994 17:15:30   BRAD
// Removed buffer for DiskMapLogToPhyParams(), DiskGetPhysicalType(), and
// DiskGetHDParams().  Buffer allocated internally.
//
//    Rev 1.13   21 Mar 1994 00:19:56   BRAD
// Cleaned up for WIN32
//
//    Rev 1.12   15 Mar 1994 12:32:16   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.11   04 Mar 1994 13:26:20   BRAD
// WORD -> UINT.
//
//    Rev 1.10   25 Feb 1994 12:22:08   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.9   09 Feb 1994 14:54:24   HENRI
// 1. Reset the gbUseOldWinCode flag to TRUE so that INT 25's are generated
//    using IntWin DPMI services.  This will ensure that an INT 13 will be
//    generated (which it otherwise wouldn't under VFAT).
//
// 2. Set bigFootFlag to TRUE anytime we are testing a VFAT disk.  There is
//    a bug in VFAT which allows old style INT 25's to pass through without
//    an error.
//
//    Rev 1.8   06 Feb 1994 12:10:32   HENRI
// Determine is a bigfoot real mode record should be allocated.
//
//    Rev 1.7   10 Mar 1993 19:45:30   BRAD
// Need to set 'generated' to FALSE, before making OS/2 test.
//
//    Rev 1.6   22 Feb 1993 19:21:10   KEVIN
// Can't map log to phy under os/2 - just skip
// Rework handling of superstor + DRDOS (see comments below)
// Port from nlib
//
//    Rev 1.5   12 Feb 1993 04:28:02   ENRIQUE
// No change.
//
//    Rev 1.4   19 Oct 1992 17:01:50   BILL
// No change.
//
//    Rev 1.3   16 Sep 1992 11:16:52   HENRI
// Added DiskCacheSuspend and DiskCacheResume before and
// after the DiskMapLogToPhyParams function.
//
//    Rev 1.2   10 Sep 1992 19:05:26   BRUCE
// Deleted commented out includes
//
//    Rev 1.1   09 Sep 1992 18:18:50   BRUCE
// Eliminated dependency on nwinutil.h
//
//    Rev 1.0   27 Aug 1992 10:33:44   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#define INCLUDE_VWIN32
#include "disk.h"
#include "xapi.h"
#include "thunk.h"

typedef struct{                /* qword */
   DWORD qword_lo;
   DWORD qword_hi;
} QWORD;				// qw

typedef struct{
        BYTE    dmiAllocationLength;
        BYTE    dmiInfoLength;
        BYTE    dmiFlags;
        BYTE    dmiInt13Unit;
        DWORD   dmiAssiciatedDriveMap;
        QWORD   dmiPartitionStartRBA;
} DRIVE_MAP_INFO;



#if defined(NCD)
#define DISKBIOS_VECT 0x1B
#else
#define DISKBIOS_VECT 0x13
#endif



#if defined(SYM_DOSX)


/* The data at the start of the real mode memory block used by
 * the real mode int 13 handler is constructed thusly.
 *
 * See INT13DOS.ASM
 *
 *
 *
 */

typedef struct{
    BYTE        JmpShortOpCode[2];      // short jump is two bytes long
    ISRPTR      sRMoldInt13Handler;
    INT13REC    sRMInt13Rec;
    BYTE        sRMpreCommand;
//    WORD        sCounter;               //used internally
}RM13HEADER, far * lpRM13HEADER;



static   lpRM13HEADER  lpProtModeMemory;


/**************************************************************************/
/*CreateRealModeJmpTarget
 *
 *
 *PARAMS : In protected mode, setting the int 13 vector has some pitfalls.
 *         This function allocates memory, and creates a far jump opcode
 *         to jump to the real mode int13 vector.
 *
 *RETURNS:
 *
 *
 *************************************************************************
 * Created by: SKURTZ   12-09-94 09:40:01am
 **************************************************************************/
lpRM13HEADER LOCAL CreateRealModeJmpTarget(LPINT13REC lpInt13Package)
{
    extern      WORD            wRMInt13HandlerSize;
    extern      void            RealModeInt13Handler();
    extern      BYTE            RMHandlerHdrSize;

    auto        lpRM13HEADER    dwBuffer;
    auto        ISRPTR          Oldvect;




    lpProtModeMemory = (lpRM13HEADER)GlobalDosAlloc(wRMInt13HandlerSize);
    if (lpProtModeMemory == NULL)
        return(NULL);

    // make real mode address

    dwBuffer = MAKELP(LOWORD(lpProtModeMemory),0);

    // fill in opcodes into this memory address

    MEMMOVE(dwBuffer,
            &RealModeInt13Handler,
            wRMInt13HandlerSize);

    // fill in jump address with old int13 vector address

    if (!d16GetRMVector(DISKBIOS_VECT,&dwBuffer->sRMoldInt13Handler))
        {
        GlobalDosFree(LOWORD(lpProtModeMemory));
        return(NULL);
        }

    MEMMOVE(&dwBuffer->sRMInt13Rec,lpInt13Package,sizeof(INT13REC));

    // make int13 vector address point to this new real mode memory.
    d16SetRMVector(DISKBIOS_VECT,(ISRPTR)MAKELP(HIWORD(lpProtModeMemory),0),&Oldvect);

    return(dwBuffer);

}   /*CreateRealModeJmpTarget*/



/**************************************************************************/
/*DestroyRealModeJmpTarget
 *
 *
 *PARAMS : reset int13 vector back to what it was originally
 *
 *
 *
 *RETURNS:
 *
 *
 *************************************************************************
 * Created by: SKURTZ   12-09-94 09:58:09am
 **************************************************************************/
void PASCAL DestroyRealModeJmpTarget(lpRM13HEADER dwBuffer)
{

    ISRPTR              Oldvect;

    if (dwBuffer != NULL)
        {
        d16SetRMVector(DISKBIOS_VECT,dwBuffer->sRMoldInt13Handler,&Oldvect);
        GlobalDosFree(LOWORD(lpProtModeMemory));
        }

}   /*DestroyRealModeJmpTarget*/

#endif


/**********************************************************************
** DiskMapLogicalToPhyParams
**
** Description:
**	Given a logical DOS sector on a particular drive, determines the
**	corresponding absolute sector (that is, where the logical sector
**	maps to the disk physically).
**
**	The method used is to try to generate an Int 13h Read Sector
**	call by reading a logical sector.  If the int 13h takes place,
**	our installed handler is ready to save the input values and
**	return an error.
**
 *      For Win32, a DeviceIOControl call is made which returns us the
 *      starting physical sector for the partition specified by the
 *      drive letter.  dwSector is then added to this and converted to
 *      physical coordinates.
 *
** Input:
**	sector is a logical DOS sector
**	dl is the drive letter of interest (upper case)
**	int13PackagePtr points to a record containing physical drive info
**
** Output:
**	*int13PackagePtr contains the physical drive number,
**	starting head, track, and sector for the given logical sector.
**
** Notes:
**	Norton cache (NCACHE) must be disabled at the time of the call.
**	(The DOS version of this routine disables it automatically.)
**
**      THE CURRENT VERSION OF THE INT 13 HANDLER ASSUMES IT'S RUNNING
**	IN PROTECTED MODE!
**
**********************************************************************/

VOID SYM_EXPORT WINAPI DiskMapLogToPhyParams (DWORD dwSector, BYTE dl,
                                   LPINT13REC lpInt13Package)
{
#ifdef _M_ALPHA
	return;
#elif defined(SYM_WIN32)

    DRIVE_MAP_INFO      dmi;
    DRIVE_MAP_INFO      FAR *dmiPtr;
    DIOC_REGISTERS      r;
    ABSDISKREC          absDisk;
    WORD                nDrive;
    DWORD                head, sector;
    DWORD                track;


    nDrive = (WORD) (dl - 'A') + 1;

    MEMSET(&dmi, 0, sizeof(DRIVE_MAP_INFO));
    MEMSET(&r, 0, sizeof(DIOC_REGISTERS));
    MEMSET(&absDisk, 0, sizeof(ABSDISKREC));

    dmiPtr = &dmi;
    dmi.dmiAllocationLength = sizeof(DRIVE_MAP_INFO);

    r.reg_EAX=0x440d;           // IOCTL
    r.reg_EBX=nDrive;           // drive (a=1)
    r.reg_ECX=0x86f;            // Sub-function GetDriveMapping
    r.reg_EDX=(DWORD) dmiPtr;   // Pointer to DISK_MAP_INFO struct

    IOCtlVWin32( &r, &r, VWIN32_DIOC_DOS_IOCTL );

                                // Calculate physical disk coordinates
                                // and copy to Int13Rec.
    if( (dmi.dmiInt13Unit < 0x0F) ||
      ((dmi.dmiInt13Unit >= 0x80) && (dmi.dmiInt13Unit <= 0x8F)) ||
      ( HWIsNEC()  &&
	(IsValidNECHDNumber(dmi.dmiInt13Unit) ||
	IsValidNECFloppyNumber(dmi.dmiInt13Unit))
	) )

    {
        absDisk.dn = dmi.dmiInt13Unit;

        // return drive number because DiskGetPhysicalInfo() will always
        // fail on NEC floppies. At least we have this much information
        // out of the call.

        lpInt13Package->dn  = dmi.dmiInt13Unit;


        if(!DiskIsCompressed(dl) && !DiskGetPhysicalInfo(&absDisk))
        {

            ConvertLongToPhySector(&absDisk,
                                   dwSector + dmi.dmiPartitionStartRBA.qword_lo,
                                   &head,
                                   &track,
                                   &sector);

            lpInt13Package->generated   = TRUE;
            lpInt13Package->dwHead      = head;
            lpInt13Package->dwTrack     = track;
            lpInt13Package->dwSector    = sector;
            lpInt13Package->dn          = dmi.dmiInt13Unit;
        }
        else
            lpInt13Package->generated   = FALSE;
    }

#else                                   // to if defined(SYM_WIN32)
                                        // NON-Win32 below:
    auto	UINT	wErr;
    auto        BYTE    bIsBigFooted = FALSE;
    auto        DWORD   dwBigFootSegSel = 0;
    auto        LPBYTE  lpRealBuffer = NULL, lpProtBuffer = NULL;
    auto        UINT    uBufferSize = 0;
#if defined(SYM_WIN)
    extern 	BOOL	gbUseOldWinCode;
    auto	BOOL 	gbOldUseOldWinCode;
#endif                                  // if defined(SYM_WIN)
#if defined(SYM_PROTMODE)
    auto        DWORD   dwBuffer;
#if defined (SYM_DOSX)
    auto        lpRM13HEADER    RMJmpAddress;
#endif                                  // if defined(SYM_DOSX)
#else
    auto        HANDLE  hBufferHandle;
#endif                                  // if defined(SYM_PROTMODE)


    MEMSET(lpInt13Package,0,sizeof(INT13REC));
                                        // Not generated yet
//    lpInt13Package->generated = FALSE;

    if (HIBYTE(DOSGetVersion()) >= 20)	// if os/2 then skip because int25
	return;				// doesn't generate int13

    if ( !DiskAbsBufferGet(&lpRealBuffer, &lpProtBuffer) )
        {
                                        // Allocate scratch buffer
                                        // (Make it VERY large sector size)
        uBufferSize = 8 * 1024;
        while (uBufferSize >= 512)
            {
#if defined(SYM_PROTMODE)
            dwBuffer = GlobalDosAlloc(uBufferSize);
            if (dwBuffer != 0)
                {
#if defined(SYM_DOSX)
                lpRealBuffer = MAKELP(HIWORD(dwBuffer), 0);
#else
                lpRealBuffer = MAKELP(LOWORD(dwBuffer), 0);
#endif
                break;
                }
#else
                                        // MemAllocFixed() does not give us a
                                        // suitably aligned buffer, so we
                                        // have to use MemAlloc() instead...
            if (hBufferHandle = MemAlloc(GMEM_MOVEABLE, uBufferSize))
                if (lpRealBuffer = MemLock(hBufferHandle))
                    break;

#endif
            uBufferSize /= 2;              // Try for smaller buffer
            }
        if ( uBufferSize < 512 )
            return;
        }

    DiskCacheSuspend();

#if defined(SYM_DOSX)
    RMJmpAddress = CreateRealModeJmpTarget(lpInt13Package);
#else
    BIOSInstallInt13Handler (lpInt13Package);
#endif

    				// Try to generate an Int 13h
    bIsBigFooted = FALSE;

    if (DiskIsVFAT(dl) || CanSupportExtendedIO())
        bIsBigFooted = TRUE;


    while (TRUE)
        {
#ifdef  SYM_PROTMODE
#if defined(SYM_WIN)
        gbOldUseOldWinCode = gbUseOldWinCode;
	gbUseOldWinCode = TRUE;
#endif
	if (bIsBigFooted)
	    {
	    dwBigFootSegSel = GlobalDosAlloc(sizeof(BigFootRec));
	    if (dwBigFootSegSel == 0)
	        break;
	    }
#endif                                  // ifdef SYM_PROTMODE

#ifdef  SYM_PROTMODE
    	wErr = _DiskBlockDevice (BLOCK_READ,
                                     bIsBigFooted,
                                     dl,
                                     dwSector,
                                     1,
                                     dwBigFootSegSel,
                                     lpRealBuffer,
                                     lpInt13Package);
#else
    	wErr = _DiskBlockDevice (BLOCK_READ | 0x80,
                                     bIsBigFooted,
                                     dl,
                                     dwSector,
                                     1,
                                     dwBigFootSegSel,
                                     lpRealBuffer,
                                     lpInt13Package);
#endif

#ifdef  SYM_PROTMODE

#ifdef SYM_DOSX
        MEMMOVE(lpInt13Package,
                &RMJmpAddress->sRMInt13Rec,
                sizeof(INT13REC));
#endif                                  // SYM_DOSX


#if defined(SYM_WIN)
	gbUseOldWinCode = gbOldUseOldWinCode;
#endif                                  // SYM_WIN
	if (bIsBigFooted)
	    GlobalDosFree(LOWORD(dwBigFootSegSel));
#endif                                  // ifdef SYM_PROTMODE
	if (wErr == 0 || bIsBigFooted || lpInt13Package->generated)
	    break;

	bIsBigFooted = TRUE;                // Error, try reading with bigfoot ON
	}



#ifdef  SYM_DOSX
    DestroyRealModeJmpTarget(RMJmpAddress);
#else
    BIOSUninstallInt13Handler();
#endif

    DiskCacheResume();

    // Adjust the cylinder number to account for SpeedStor and DRDOS,
    // which allow more than 1024 cyls by using bits from the head number.
    // Note that Disk Manager uses a different scheme to access cyls above
    // 1024, and the INT 13H hook will have already handled that case.
    //
    // Prior versions of Norton Utilities always assumed that the top two
    // head bits were an extension of the cylinder.  Now there are some SCSI
    // controllers (e.g. Adaptec 1540) that really use all 8 bits of the head
    // number as a head number.

#ifdef NCD
    {
    BYTE    dn;
    WORD    wBX;
    WORD    wCX;
    BYTE    bDH;
    BYTE    bDL;
    unsigned long lOffsetSector;


    if (lpInt13Package->generated)
        {
        dn = lpInt13Package->dn;

        if ( !( dn & 0x50 ) )
            {         // IsHD?
             _asm
                {
                mov al, dn
                or       al, 80h                        // Make sure not relativemode
                mov ah, 84h                     // enhansed SENSE
                int 1Bh
                mov wBX, bx                     // SecLen
                mov wCX, cx                     // Cyl
                mov bDH, dh                     // Heads
                mov bDL, dl                     // Sectors
                }


            switch ( wBX )
                {
                case  128:
                        lpInt13Package->sectorLenCode = 0;
                        break;

                case  256:
                        lpInt13Package->sectorLenCode = 1;
                        break;

                case  512:
                        lpInt13Package->sectorLenCode = 2;
                        break;

                case 1024:
                        lpInt13Package->sectorLenCode = 3;
                        break;

                case 2048:
                        lpInt13Package->sectorLenCode = 4;
                        break;

                default:
                        lpInt13Package->sectorLenCode = 0;
                        break;
                }

                if( !( lpInt13Package->dn & 0x80 ) )
                    {  // Is Relative ?
                    lOffsetSector = lpInt13Package->dwHead;           // DH
                    lOffsetSector <<= 8;
                    lOffsetSector += lpInt13Package->dwSector;        // DL
                    lOffsetSector <<= 16;
                    lOffsetSector += lpInt13Package->dwTrack; // CX

                    lpInt13Package->dwTrack   = ( unsigned int )( lOffsetSector / ( unsigned long )( bDL * bDH ) );
                    lpInt13Package->dwHead    = ( unsigned char )( ( lOffsetSector / ( unsigned long)( bDL ) ) % ( unsigned long)( bDH ) );
                    lpInt13Package->dwSector  = ( unsigned char )( lOffsetSector % (unsigned long )( bDL ) );

                    }

            lpInt13Package->dn |= 0x80;
            lpInt13Package->dwSector += 1;    /* NEC BIOS start from 0 */
            }
        }

    }

#else
    {
    auto    AbsDiskRec  absdisk;
    auto    AbsDiskRec  absdiskbasic;
    auto    DWORD       dwBlockNum;

    // The DOS Int25 read apparently only generates an extended
    // int13 when it has to. So if it can use plain int13 params
    // to access a partition, it will do a plain int13 read.
    // Otherwise, it will do an extended int13. For that reason,
    // we can't look at the Int13Package to determine if the
    // drive supports int13 extensions. Instead, we need to make
    // that check here using an alternate function.

    // Find out how many heads the drive has
    MEMSET(&absdisk,0,sizeof(AbsDiskRec));
    MEMSET(&absdiskbasic,0,sizeof(AbsDiskRec));
    absdisk.dn = absdiskbasic.dn = lpInt13Package->dn;
    DiskGetPhysicalInfo(&absdisk);

    if  (lpInt13Package->generated && (absdisk.bIsExtInt13 == ISINT13X_YES))
        {
        // now that we have the physical sector, we can do some math
        // to convert it to head/track/sector and fill out the rest.
        if(lpInt13Package->ExtendedInt13 == ISINT13X_YES)
            {
            // This means that not only is it an extended int13 drive,
            // but the int25 used the extended call to do the read, so
            // the CHS values will be actual.
            ConvertLongToPhySector(&absdisk,
                                   lpInt13Package->block_numLo,
                                   &lpInt13Package->dwHead,
                                   &lpInt13Package->dwTrack,
                                   &lpInt13Package->dwSector);
            }
        else
            {
            // This means that the int25 used the basic int13 to do
            // the read, and the CHS values are in the package are
            // translated values. So convert them to a sector, then
            // convert back to actual CHS values

            // Get translated geometry into our basic absdisk
            DiskGetPhysicalInfoBasic(&absdiskbasic);

            // use it to convert translated params to abs sector
            dwBlockNum = ConvertPhySectorToLong(&absdiskbasic,
                                                lpInt13Package->dwHead,
                                                lpInt13Package->dwTrack,
                                                lpInt13Package->dwSector);

            // and use the abs sector and actual absdisk to get actual
            // params
            ConvertLongToPhySector(&absdisk,
                                   dwBlockNum,
                                   &lpInt13Package->dwHead,
                                   &lpInt13Package->dwTrack,
                                   &lpInt13Package->dwSector);

            }
        }
    else  if (lpInt13Package->generated && lpInt13Package->dwTrack < 1024)
        {
        auto    AbsDiskRec  absdisk;

        // Find out how many heads the drive has
        absdisk.dn = lpInt13Package->dn;
        if (DiskGetPhysicalInfo(&absdisk) == 0 && absdisk.dwTotalHeads < 64)
            {
            // Drive has less than 64 heads, so it should be safe to
            // take the upper two head bits (if any) and use them as an
            // extension of the cylinder number.  If SpeedStor or DRDOS
            // aren't in use, these bits will be zero.
            lpInt13Package->dwTrack |= ((lpInt13Package->dwHead & 0xC0) << 4);
            lpInt13Package->dwHead &= 0x3F;
            }
        }
    }

#endif
                                        // Free scratch buffer
    if ( uBufferSize )
#if defined(SYM_PROTMODE)
        GlobalDosFree(LOWORD(dwBuffer));
#else
        MemUnlock(hBufferHandle, lpRealBuffer);
        MemFree(hBufferHandle);
#endif                                  // if defined(SYM_PROTMODE)
#endif                                  // to #if defined(SYM_WIN32)/#else block
}
