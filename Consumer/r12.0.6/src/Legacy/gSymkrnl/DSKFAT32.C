// Copyright 1996 Symantec Corporation
///*********************************************************************
//
// $Header:   S:/SYMKRNL/VCS/Dskfat32.c_v   1.25   17 Jun 1997 19:21:06   MBROWN  $
//
// Description:
// This file contains those functions which relate to Microsoft's
// 32-bit FAT file system.
//
// *********************************************************************
//
// $Log:   S:/SYMKRNL/VCS/Dskfat32.c_v  $
// 
//    Rev 1.25   17 Jun 1997 19:21:06   MBROWN
// The DOS block of DiskWriteFSInfo needed to handle the free count being
// 0xffffffff, which NDD passes in to set the count to zero, since zero can't be
// passed in to WIN block.
// 
//    Rev 1.24   15 May 1997 10:39:42   MBROWN
// Fixed typo.
//
//    Rev 1.23   14 May 1997 19:24:46   MBROWN
// Needed to make copy of constant diskrec to modify values.
//
//    Rev 1.22   14 May 1997 18:58:52   MBROWN
// Changed FAT mirroring fn's to take a LPCDISKREC instead of a DISKREC.
//
//    Rev 1.21   14 May 1997 17:15:28   MBROWN
// More FAT mirroring work.
//
//    Rev 1.20   13 May 1997 17:07:30   MBROWN
// Modified DOS version of mirroring API so that we just read from the boot
// record. API no longer able to set properties in DOS.
//
//    Rev 1.19   13 May 1997 11:48:36   MBROWN
// Attempted bug fix for DOS versions of FAT mirroring API.
//
//    Rev 1.18   09 May 1997 16:12:38   MBROWN
// Fixed bugs in FAT32 FAT Mirroring API.
//
//    Rev 1.17   08 May 1997 18:45:12   MBROWN
// Removed Win specific error code from DOS block.
//
//    Rev 1.16   08 May 1997 18:29:54   MBROWN
// Implemented FAT32 FAT Mirroring API.
//
//    Rev 1.15   02 May 1997 13:18:58   MBROWN
// Fixed bug in DOS path of DiskWriteAllocInfo. I was incorrectly checking the
// return value from DiskBlockDevice.
//
//    Rev 1.14   25 Apr 1997 15:48:58   MBROWN
// Fixed casting problem.
//
//    Rev 1.13   25 Apr 1997 10:54:56   MBROWN
// Changed the DOS version of DiskWriteFSInfo() so that we read in the
// FAT32EXBOOTREC, modify it, and write it back to the disk rather than
// use the int 21 fn, which didn't seem to work.
//
//    Rev 1.12   23 Apr 1997 17:41:32   MBROWN
// Fixed DiskReadFSInfo(). Was casting to the wrong structure type.
//
//    Rev 1.11   23 Apr 1997 15:29:30   MBROWN
// Implemented DiskReadFSInfo().
//
//    Rev 1.10   18 Apr 1997 17:00:52   MARKK
// Added ability to set the cluster count in FSInfo
//
//    Rev 1.9   19 Mar 1997 21:35:42   BILL
// Modified files to build properly under Alpha platform
//
//    Rev 1.8   13 Mar 1997 11:01:52   MARKK
// Hooked up DiskWriteFSInfo
//
//    Rev 1.7   07 Aug 1996 16:58:46   DBUCHES
// Fixed DiskGet/SetRootSCN for Dx platform.
//
//    Rev 1.6   26 Mar 1996 16:32:52   MARKK
// DiskSetRootSCN now works!
//
//    Rev 1.5   21 Mar 1996 12:35:16   MARKK
// Added setting of root scn, more to follow
//
//    Rev 1.3   26 Feb 1996 10:32:50   MARKK
// Fixed call for Root SCN
//
//    Rev 1.2   23 Feb 1996 12:28:20   MARKK
// Clean up
//
//    Rev 1.1   23 Feb 1996 12:16:14   MARKK
// Added code for DiskSetRootSCN
//
//    Rev 1.1   23 Feb 1996 12:07:38   MARKK
// Added code for DiskSetRootSCN
//
//    Rev 1.0   26 Jan 1996 20:23:14   JREARDON
// Initial revision.
//
//    Rev 1.0   10 Jan 1996 14:54:08   HENRI
// Initial revision.
//

#include <dos.h>
#include "platform.h"
#define INCLUDE_VWIN32
#include "xapi.h"
#include "disk.h"
#include "syscall.h"

#include <dos.h>

MODULE_NAME;

// RETURN:
// If successful, the cluster number of the root directory
// If failed, returns 0
CLUSTER SYM_EXPORT WINAPI DiskGetRootSCN(LPDISKREC lpDisk)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return 0;
#else
    auto        SDPDFormatStruc rFormat;

    MEMSET(&rFormat, 0, sizeof(rFormat));
    rFormat.SetDPB_Size = sizeof(rFormat);
    rFormat.SetDPB_Level = SDPDCurMaxLevel;
    rFormat.SetDPB_Function = SetDPB_GetSetRootDirClus;
                                        // Value1 field implements the SET behavior
                                        // by setting it to 0xFFFFFFFF, the SET is
                                        // ignored.
    rFormat.SetDPB_Value1 = 0xFFFFFFFF;

    if (IsFAT32(*lpDisk))
        {
#ifdef SYM_WIN32
        auto        SYM_REGS        regs;

        MEMSET(&regs, 0, sizeof(regs) );
        regs.EAX = (Get_Set_DriveInfo << 8) + Set_DPBForFormat;
        regs.ECX = sizeof(rFormat);
        regs.EDX = lpDisk->dn + 1;
        regs.EDI = (DWORD) &rFormat;

        VxDIntBasedCall( 0x21, &regs );

                                                           // If carry, save off error
        if ( regs.EFlags & 0x00000001 )
            return( (CLUSTER) 0xFFFFFFFF) ;
        else
            return( (CLUSTER) rFormat.SetDPB_Value2 );
#else
        auto        union REGS      regs;
        auto        struct SREGS    sregs;
        auto        DWORD           dwSegSel;
        auto        LPBYTE          lpBuf;

        dwSegSel = GlobalDosAlloc(sizeof(SDPDFormatStruc));
        if (dwSegSel == NULL)
            return (FALSE);

        lpBuf = (LPBYTE) MAKELP (dwSegSel, 0);
        MEMCPY(lpBuf, &rFormat, sizeof(SDPDFormatStruc));

        regs.x.ax = (Get_Set_DriveInfo << 8) + Set_DPBForFormat;
        regs.x.cx = rFormat.SetDPB_Size;
        regs.x.dx = lpDisk->dn+1;
        regs.x.di = 0;
        sregs.es =  HIWORD(dwSegSel);

        IntWin(0x21, &regs, &sregs);

        MEMCPY(&rFormat, lpBuf, sizeof(SDPDFormatStruc));
        GlobalDosFree(LOWORD(dwSegSel));

        if (regs.x.cflag)
            return( (CLUSTER) 0xFFFFFFFF) ;
        else
            return( (CLUSTER) rFormat.SetDPB_Value2 );

#endif

        }
    else
        return 0;
#endif // _M_ALPHA
}

// RETURN:
// If successful, the cluster number that the root directory was set to
// If failed, returns 0
BOOL SYM_EXPORT WINAPI DiskSetRootSCN(LPDISKREC lpDisk, CLUSTER clRootSCN)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
    auto	DeviceParamRec	deviceParams;
    auto        SDPDFormatStruc rFormat;

    MEMSET(&rFormat, 0, sizeof(rFormat));
    rFormat.SetDPB_Size = sizeof(rFormat);
    rFormat.SetDPB_Level = SDPDCurMaxLevel;
    rFormat.SetDPB_Function = SetDPB_GetSetRootDirClus;
    rFormat.SetDPB_Value1 = (DWORD) clRootSCN;

#ifdef SYM_WIN32
    {
    auto        SYM_REGS        regs;

    MEMSET(&regs, 0, sizeof(regs));
    regs.EAX = (Get_Set_DriveInfo << 8) + Set_DPBForFormat;
    regs.ECX = rFormat.SetDPB_Size;
    regs.EDX = lpDisk->dn + 1;
    regs.EDI = (DWORD)&rFormat;
    VxDIntBasedCall( 0x21, &regs );
    if ( regs.EFlags & 0x00000001 )
        return (FALSE);
    }
#else
    {
    auto        union REGS      regs;
    auto        struct SREGS    sregs;
    auto        DWORD           dwSegSel;
    auto        LPBYTE          lpBuf;

    dwSegSel = GlobalDosAlloc(sizeof(SDPDFormatStruc));
    if (dwSegSel == NULL)
        return (FALSE);

    lpBuf = (LPBYTE) MAKELP (dwSegSel, 0);
    MEMCPY(lpBuf, &rFormat, sizeof(SDPDFormatStruc));

    regs.x.ax = (Get_Set_DriveInfo << 8) + Set_DPBForFormat;
    regs.x.cx = rFormat.SetDPB_Size;
    regs.x.dx = lpDisk->dn + 1;
    regs.x.di = 0;
    sregs.es =  HIWORD(dwSegSel);

    IntWin(0x21, &regs, &sregs);

    GlobalDosFree(LOWORD(dwSegSel));

    if (regs.x.cflag)
        return (FALSE);
    }
#endif

    MEMSET(&deviceParams, 0, sizeof(deviceParams));
    DiskGenericIOCTL(IOCTL_GET_PARAMS, lpDisk->dl, &deviceParams);

    deviceParams.BPB.dwRootSCN = (DWORD) clRootSCN;

    deviceParams.specialFunctions = IOCTL_SF_SET_DEFAULT +
      IOCTL_SF_USE_ALL_PARAMS +
      IOCTL_SF_SAME_SIZE;
                                        /* Zero out the track data      */

    deviceParams.wSectorsPerTrack = 0;

    return DiskGenericIOCTL(IOCTL_SET_PARAMS, lpDisk->dl, &deviceParams);
#endif
}

// RETURN:
// If successful, TRUE
// If failed, FALSE
BOOL SYM_EXPORT WINAPI DiskReadFSInfo(LPDISKREC lpDisk, LPFILESYSINFO lpFSInfo)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
    BOOL    bRet = FALSE;
    LPVOID  lpBuffer;

	SYM_ASSERT(lpDisk);
    if(!IsFAT32(*lpDisk))
        return bRet;

                                        // Doesn't need to be fixed, DiskBlock uses its own buffers...
    if((lpBuffer = MemAllocPtr(GHND,lpDisk->bytesPerSector)) == NULL)
        return bRet;

    lpDisk->buffer = (HPBYTE)lpBuffer;
    lpDisk->sector = 0;
    lpDisk->sectorCount = 1;

										// Read FAT32 boot record
    if(!DiskBlockDevice(BLOCK_READ,lpDisk))
        {
        FAT32BPBBOOTREC _far * lpf32br = (FAT32BPBBOOTREC _far *) lpBuffer;

        lpDisk->buffer = (HPBYTE)lpBuffer;
        lpDisk->sector = lpf32br->BPB.wFSInfoSector;
        lpDisk->sectorCount = 1;

        if((lpDisk->sector > 0) && (lpDisk->sector < lpf32br->BPB.startOfFat) && (lpDisk->sector != 0xffff))
			{
			MEMSET(lpBuffer, 0, lpDisk->bytesPerSector);
                                        // Read in FAT32EXBOOTREC
			if(!DiskBlockDevice(BLOCK_READ,lpDisk))
				{
				SYM_ASSERT(lpFSInfo);
                MEMCPY(lpFSInfo, &((FAT32EXBOOTREC _far *) lpBuffer)->FSInfo, sizeof(FILESYSINFO));
				bRet = TRUE;
				}
			}
        }

	MemFreePtr(lpBuffer);

    return bRet;

#endif // _M_ALPHA
}

// RETURN:
// If successful, TRUE
// If failed, FALSE
BOOL SYM_EXPORT WINAPI DiskWriteFSInfo(LPDISKREC lpDisk, LPFILESYSINFO lpFSInfo)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
	SYM_ASSERT(lpDisk);
    if(!IsFAT32(*lpDisk))
        return FALSE;

#ifdef SYM_WIN32
    {
    auto        SDPDFormatStruc rFormat;
    auto        SYM_REGS        regs;

    MEMSET(&rFormat, 0, sizeof(rFormat));
    rFormat.SetDPB_Size = sizeof(rFormat);
    rFormat.SetDPB_Level = SDPDCurMaxLevel;
    rFormat.SetDPB_Function = SetDPB_SetAllocInfo;
    rFormat.SetDPB_Value1 = lpFSInfo->dwFreeClusterCount;
    rFormat.SetDPB_Value2 = lpFSInfo->dwNextFreeCluster;


    MEMSET(&regs, 0, sizeof(regs));
    regs.EAX = (Get_Set_DriveInfo << 8) + Set_DPBForFormat;
    regs.ECX = rFormat.SetDPB_Size;
    regs.EDX = lpDisk->dn + 1;
    regs.EDI = (DWORD)&rFormat;
    VxDIntBasedCall( 0x21, &regs );
    if ( regs.EFlags & 0x00000001 )
        return (FALSE);
    }
#else
    {
    // Read in the FAT32 boot rec, change the
    // appropriate value and write it back.
    BOOL    bRet = FALSE;
    LPVOID  lpBuffer;

                                            // Doesn't need to be fixed, DiskBlock uses its own buffers...
    if((lpBuffer = MemAllocPtr(GHND,lpDisk->bytesPerSector)) == NULL)
        return bRet;

    lpDisk->buffer = (HPBYTE)lpBuffer;
    lpDisk->sector = 0;
    lpDisk->sectorCount = 1;

										// Read FAT32 boot record
    if(!DiskBlockDevice(BLOCK_READ,lpDisk))
        {
        FAT32BPBBOOTREC _far * lpf32br       = (FAT32BPBBOOTREC _far *) lpBuffer;

        lpDisk->buffer = (HPBYTE)lpBuffer;
        lpDisk->sector = lpf32br->BPB.wFSInfoSector;
        lpDisk->sectorCount = 1;

        if((lpDisk->sector > 0) && (lpDisk->sector < lpf32br->BPB.startOfFat) && (lpDisk->sector != 0xffff))
			{
			MEMSET(lpBuffer, 0, lpDisk->bytesPerSector);
                                        // Read in FAT32EXBOOTREC
			if(!DiskBlockDevice(BLOCK_READ,lpDisk))
				{
				SYM_ASSERT(lpFSInfo);
                                        // Modify the appropriate values
                if(lpFSInfo->dwFreeClusterCount != 0)
                    ((FAT32EXBOOTREC _far *) lpBuffer)->FSInfo.dwFreeClusterCount = lpFSInfo->dwFreeClusterCount;
                                        // 0xffffffff tells the Windows f'n to recalculate.
                                        // NDD will pass this value in when the count is supposed
                                        // to be zero, because zero can't be passed to the 
                                        // windows f'n. But in DOS we just slap the value down.
                else if(lpFSInfo->dwFreeClusterCount == 0xffffffff)
                    ((FAT32EXBOOTREC _far *) lpBuffer)->FSInfo.dwFreeClusterCount = 0L;

                if(lpFSInfo->dwNextFreeCluster != 0)
                    ((FAT32EXBOOTREC _far *) lpBuffer)->FSInfo.dwNextFreeCluster = lpFSInfo->dwNextFreeCluster;

                                        // Slap down the FAT32EXBOOTREC with
                                        // the modified values
                if(!DiskBlockDevice(BLOCK_WRITE, lpDisk))
                    bRet = TRUE;
				}
			}
        }

	MemFreePtr(lpBuffer);

    return bRet;


    }
#endif

    return (TRUE);
#endif // _M_ALPHA
}



BOOL SYM_EXPORT WINAPI DiskGetActiveFAT(LPCDISKREC lpDisk, UINT FAR * lpuActiveFAT)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
	BOOL	bRet;
	DWORD	dwValue;

	SYM_ASSERT(lpDisk);
    if(!IsFAT32(*lpDisk))
        return FALSE;

	bRet = DiskGetSetActFATandMirr(lpDisk, 0xffffffff, &dwValue);

	if(bRet)
		*lpuActiveFAT = (UINT) (dwValue & ActMirr_ActiveFATMsk);

	return(bRet);

#endif // _M_ALPHA
}



BOOL SYM_EXPORT WINAPI DiskNoFATMirrAndSetActiveFAT(LPCDISKREC lpDisk, UINT uActiveFAT)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
	SYM_ASSERT(lpDisk);
    if(!IsFAT32(*lpDisk))
        return FALSE;

#ifdef SYM_WIN32
    return(DiskGetSetActFATandMirr(lpDisk, ActMirr_NoFATMirror | uActiveFAT, NULL));
#else // DOS
	return(FALSE);
#endif // SYM_WIN32

#endif // _M_ALPHA
}


BOOL SYM_EXPORT WINAPI DiskGetFATMirrStatus(LPCDISKREC lpDisk, LPBOOL lpbIsFATMirrored)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
	BOOL	bRet;
	DWORD	dwValue;

	SYM_ASSERT(lpDisk);
    if(!IsFAT32(*lpDisk))
        return FALSE;

	bRet = DiskGetSetActFATandMirr(lpDisk, 0xffffffff, &dwValue);

	if(bRet)
        *lpbIsFATMirrored = (dwValue & ActMirr_NoFATMirror) > 0 ? FALSE : TRUE;

	return(bRet);

#endif // _M_ALPHA
}


BOOL SYM_EXPORT WINAPI DiskEnableFATMirr(LPCDISKREC lpDisk)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
	SYM_ASSERT(lpDisk);
    if(!IsFAT32(*lpDisk))
        return FALSE;

#ifdef SYM_WIN32
	return(DiskGetSetActFATandMirr(lpDisk, 0x00000000, NULL));
#else // DOS
	return(FALSE);
#endif // SYM_WIN32

#endif // _M_ALPHA
}


BOOL SYM_EXPORT WINAPI DiskGetSetActFATandMirr(LPCDISKREC lpDisk, DWORD dwValue1, DWORD FAR * lpdwValue2)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
	auto		bRet = TRUE;

	SYM_ASSERT(lpDisk);
    if(!IsFAT32(*lpDisk))
        return FALSE;

#ifdef SYM_WIN32
    {
    auto        SDPDFormatStruc rFormat;
    auto        SYM_REGS        regs;

    MEMSET(&rFormat, 0, sizeof(rFormat));
    rFormat.SetDPB_Size = sizeof(rFormat); // return value?
    rFormat.SetDPB_Level = SDPDCurMaxLevel;
    rFormat.SetDPB_Function = SetDPB_GetSetActFATandMirr;
    rFormat.SetDPB_Value1 = dwValue1;

    MEMSET(&regs, 0, sizeof(regs));
    regs.EAX = (Get_Set_DriveInfo << 8) + Set_DPBForFormat;
    regs.ECX = rFormat.SetDPB_Size;
    regs.EDX = lpDisk->dn + 1;
    regs.EDI = (DWORD)&rFormat;
    VxDIntBasedCall( 0x21, &regs );
    if ( regs.EFlags & 0x00000001 )
		{
										// If we tried to SET active FAT or Mirror
										// property and failed to write to the disk,
										// we must set the BPB back to the original
										// state.
		if(regs.EAX == ERROR_GEN_FAILURE)
			{
										// If we attempted to disable mirroring (bit 7 hi)
										// then now re-enable (bit 7 lo)
			if(dwValue1 & ActMirr_NoFATMirror)
				rFormat.SetDPB_Value1 = dwValue1 ^ ActMirr_NoFATMirror;
			else						// We attempted to enable mirroring (bit 7 lo)
										// so now re-disable it (bit 7 hi)
				rFormat.SetDPB_Value1 = ActMirr_NoFATMirror;

			MEMSET(&regs, 0, sizeof(regs));
			regs.EAX = (Get_Set_DriveInfo << 8) + Set_DPBForFormat;
			regs.ECX = rFormat.SetDPB_Size;
			regs.EDX = lpDisk->dn + 1;
			regs.EDI = (DWORD)&rFormat;
			VxDIntBasedCall( 0x21, &regs );
			}
        bRet = FALSE;
		}

	if(lpdwValue2)
		*lpdwValue2 = rFormat.SetDPB_Value2;
    }
#else // DOS/WIN16
    {
    // Read in the FAT32 boot rec to determine mirroring properties
    LPVOID  lpBuffer;
    DISKREC disk;

    MEMCPY(&disk, lpDisk, sizeof(disk));

	bRet = FALSE;
                                        // Doesn't need to be fixed, DiskBlock uses its own buffers...
    if((lpBuffer = MemAllocPtr(GHND, disk.bytesPerSector)) == NULL)
        return bRet;

    disk.buffer = (HPBYTE)lpBuffer;
    disk.sector = 0;
    disk.sectorCount = 1;

										// Read FAT32 boot record
    if(!DiskBlockDevice(BLOCK_READ, &disk))
        {
        FAT32BPBBOOTREC _far * lpf32br = (FAT32BPBBOOTREC _far *) lpBuffer;

		if(lpdwValue2)
			{
			*lpdwValue2 = (DWORD) (*((WORD*) &(lpf32br->BPB.rExtendedBPBFlags)));
			}

		bRet = TRUE;
        }

	MemFreePtr(lpBuffer);
    }
#endif // SYM_WIN32

    return (bRet);

#endif // _M_ALPHA
}


