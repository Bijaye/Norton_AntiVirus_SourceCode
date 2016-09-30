/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/PRM_SFT.C_v   1.0   26 Jan 1996 20:21:16   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/PRM_SFT.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:21:16   JREARDON
// Initial revision.
// 
//    Rev 1.38   02 Aug 1995 00:49:18   AWELCH
// Merge changes from Quake 7.
// 
//    Rev 1.37.2.1   14 Jun 1995 15:41:16   MBARNES
// Added #ifdefs to check if NO_SYMEVENT was defined; if so, SYMKRNL will not
// attempt to load SYMEVNT?.DLL.  This is to avoid problems when the SYMEVENT
// DLL may be loadable, but the SYMEVENT VXD is not installed.
// 
//    Rev 1.37.2.0   24 Apr 1995 19:44:06   SCOTTP
// Branch base for version QAK7
// 
//    Rev 1.37   09 Nov 1994 17:17:00   BRAD
// Removed MyNameToUnpacked() and used the one in Quake
// 
//    Rev 1.36   18 Aug 1994 17:20:08   LCOHEN
// Cleaned up for platforms other than SYM_WIN16.
//
//    Rev 1.35   05 Jul 1994 17:52:34   GVOGEL
// Added libray info for Geologist.
//
//    Rev 1.34   05 Jul 1994 17:47:24   GVOGEL
// Fixed error reported by Geologist build.
//
//    Rev 1.33   07 Jun 1994 02:59:44   GVOGEL
// Added comment blocks for Geologist project.
//
//    Rev 1.32   23 May 1994 15:33:58   MARKK
// Fixed for DOSX
//
//    Rev 1.31   18 May 1994 16:37:34   BRAD
// Removed reference to Toolhelp.h.  Not used
//
//    Rev 1.30   05 May 1994 17:51:42   BRAD
// We won't be doing VFAT stuff under DOS yet.  Remove OFT processing for DOSX
//
//    Rev 1.29   05 May 1994 15:56:36   MARKK
// DOSX work
//
//    Rev 1.28   05 May 1994 12:25:34   BRAD
// DX stuff
//
//    Rev 1.27   04 May 1994 06:25:26   MARKK
// DOSX work
//
//    Rev 1.26   04 May 1994 06:13:50   MARKK
// DOSX work
//
//    Rev 1.25   03 May 1994 18:24:34   BRAD
// Added DOSGetPSP()
//
//    Rev 1.24   31 Mar 1994 10:10:34   BRAD
// SYMEVENT field name keeps changing
//
//    Rev 1.23   28 Mar 1994 14:48:34   BRAD
// Use new DOS macro
//
//    Rev 1.22   16 Mar 1994 19:29:16   BRAD
// field renamed
//
//    Rev 1.21   15 Mar 1994 23:31:28   LCOHEN
// VFAT support and Bruce M.'s EXPORT changes.
//
//    Rev 1.20   10 Mar 1994 18:40:20   LCOHEN
// Added VFAT support to SYMKrnl.
//
//    Rev 1.19   25 Feb 1994 15:03:22   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.17   02 Feb 1994 11:35:20   MARKK
// Moved DOSFlushAllWriteFiles to SYMFAT
//
//    Rev 1.16   13 Jan 1994 13:31:54   MARKK
// Exported some previously local SFT routines
//
//    Rev 1.15   12 Jan 1994 16:30:46   MARKK
// Don't need to convert address of PSP
//
//    Rev 1.14   12 Jan 1994 16:07:18   MARKK
// Added DOSFlushAllWriteFiles
//
//    Rev 1.13   12 Sep 1993 17:06:08   HENRI
// Fixed DOSGetInfoOnAllOpenFilesInt which did not work correctly when all
// drives were specified.
//
//    Rev 1.12   23 Aug 1993 10:18:04   BARRY
// Added MODULE_NAME for VMM debugging
//
//    Rev 1.11   02 Jul 1993 08:52:12   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.10   28 Apr 1993 15:59:20   BRUCE
// [FIX] #undef MAX_PATH to include toolhelp.h
//
//    Rev 1.9   30 Mar 1993 10:57:54   HENRI
// Moved GetProtModePtr() to PRM_WIN.C
//
//    Rev 1.8   17 Feb 1993 09:51:06   MARKK
// Changed GetOpenFilesOnDrive
//
//    Rev 1.7   12 Feb 1993 04:26:56   ENRIQUE
// No change.
//
//    Rev 1.6   04 Feb 1993 10:04:14   MARKK
// Fixed allocation error
//
//    Rev 1.5   03 Feb 1993 14:55:42   MARKK
// Fixed error in return value
//
//    Rev 1.4   03 Feb 1993 12:57:38   MARKK
// Added DOSGetOpenFilesOnDrive
//
//    Rev 1.3   25 Dec 1992 14:33:20   HENRI
// Modified all remaining functions (from previous
// revision) for use in Windows (i.e. Protected Mode Addressing).
//
//    Rev 1.2   25 Dec 1992 13:20:40   HENRI
// The following routines have been modified to work
// properly under Windows:
// 1. DOSGetProcessFileHandleCount( WORD wPSP )
// 2. DOSGetSystemFileHandleCount(void)
// 3. DOSGetOpenFileCount(BYTE byDriveLetter, BYTE bOnlyIfWritten)
//
//    Rev 1.1   23 Dec 1992 14:54:50   HENRI
// Requires <wPSP> parameter on many functions for windows compatibility.
//
//    Rev 1.0   23 Dec 1992 14:11:26   HENRI
// Initial revision.
//
//    Rev 1.2   18 Dec 1992 13:13:58   CRAIG
// Disable global optimizer in in-line assembly.
//
//    Rev 1.1   15 Sep 1992 10:57:08   HENRI
// Added include of DOS.H
//
//    Rev 1.0   09 Sep 1992 17:05:58   HENRI
// Initial revision.
 ************************************************************************/

/*@Library: SYMKRNL*/

#include <dos.h>
#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "file.h"
#include "symevent.h"
#include <stdlib.h>                     // Microsoft globals
#include "stddos.h"

MODULE_NAME;

/* Function definitions local to prm_sft.c */

WORD LOCAL PASCAL        DOSGetInfoOnAllOpenFilesInt (WORD wMaxFiles, OpenFileInfoRec far *lpInfo, BYTE byDrive);
void LOCAL PASCAL        ConvertDos2SFT              (const Dos2SFT far *lpSFT, OpenFileInfoRec far *lpInfo);
void LOCAL PASCAL        ConvertDos3SFT              (const Dos31SFT far *lpSFT, OpenFileInfoRec far *lpInfo);
void LOCAL PASCAL        ConvertDos4SFT              (const Dos4SFT far *lpSFT, OpenFileInfoRec far *lpInfo);
void LOCAL PASCAL        ConvertVFAT311OFT       (const VFAT311OFT far *lpOFT, OpenFileInfoRec far *lpInfo);
#ifdef SYM_WIN16
BOOL LOCAL PASCAL        VFATGetFirstOFT         (LPFI lpFI);
BOOL LOCAL PASCAL        VFATGetNextOFT          (LPFI lpFI);
#endif


/*----------------------------------------------------------------------*
 * DOSGetPSP                                                            *
 *                                                                      *
 * Returns the DOS PSP.                                                 *
 *----------------------------------------------------------------------*/

WORD SYM_EXPORT WINAPI DOSGetPSP(VOID)
{
    WORD        wPSP;

#if defined(SYM_DOS)
    wPSP = _psp;
#else
    _asm
	{
	mov     ah, 51H
	DOS
	mov     wPSP, bx
	}
#endif

    return ( wPSP );
}

/*@API:**********************************************************************
@Declaration: WORD DOSGetProcessFileHandleCount(WORD wPSP)

@Description:
Gets the number of handles available to a process.  This is 20 unless special
arrangements have been made.

@Parameters:
$wPSP$ The process's PSP value.

@Returns:
Number of handles available to a process.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
WORD SYM_EXPORT PASCAL DOSGetProcessFileHandleCount(WORD wPSP)
{
#if !defined(SYM_WIN)
    wPSP = _psp;
#endif

    if (_osmajor < 3)
	return(20);

    return( *( (LPWORD) MAKELP( wPSP, 0x0032 ) ) );
}

/*@API:**********************************************************************
@Declaration: WORD DOSGetSystemFileHandleCount()

@Description:
This function returns the maximum number of handles in the system.

@Returns:
The return values is the maximum number of handles available in the system (this
is set by the FILES=nnn entry in CONFIG.SYS).

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
WORD SYM_EXPORT PASCAL DOSGetSystemFileHandleCount(void)
{
    static      UINT            wHandleCount = 0;
    auto        SFTHeaderRec    far *lpSFTBlock;
#ifdef SYM_PROTMODE
    auto        SFTHeaderRec    far *lpNextSFTBlock;
#endif
					/*------------------------------*/
					/* Only count the handles once  */
					/* -- It can't change out from  */
					/* under us!                    */
					/*------------------------------*/
    if (wHandleCount != 0)
	return(wHandleCount);

					/*------------------------------*/
					/* Walk the list of SFT blocks, */
					/* counting number of entries   */
					/* in each                      */
					/*------------------------------*/
    lpSFTBlock = GetProtModePtr( DOSGetFirstSFT() );

    while (FP_OFF(lpSFTBlock) != 0xFFFF)
	{
	wHandleCount += lpSFTBlock->wFileCount;

#ifdef SYM_PROTMODE
	lpNextSFTBlock = lpSFTBlock->lpNext;
	SelectorFree( FP_SEG( lpSFTBlock ) );
	lpSFTBlock = GetProtModePtr(lpNextSFTBlock);
#else
	lpSFTBlock = lpSFTBlock->lpNext;
#endif

	}

#ifdef SYM_PROTMODE
    if (FP_SEG(lpSFTBlock) != NULL)
	SelectorFree( FP_SEG( lpSFTBlock ) );
#endif

    return(wHandleCount);
}


/*@API:**********************************************************************
@Declaration: WORD DOSGetOpenFileCount(BYTE byDriveLetter, BOOL bOnlyIfWritten)

@Description:
This function gets the number of files open on a particular drive. \n

@Parameters:
$byDriveLetter$ Drive letter to search for open files.

$bOnlyIfWritten$ Only count files that have been updated to.

@Returns:
The return value is the number of open files on drive.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
/*                                                                      *
 * Enhancement for VFAT 3.11:  Check to see if the drive selected is    *
 * being managed by VFAT.  If so, we traverse the VFAT OFTs and get the *
 * requested information.  Else, we default to traversing the SFTs.     *
 *----------------------------------------------------------------------*/
WORD SYM_EXPORT PASCAL DOSGetOpenFileCount(BYTE byDriveLetter, BYTE bOnlyIfWritten)
{
#ifdef SYM_PROTMODE
    auto        SFTHeaderRec    far *lpNextSFTBlock;
#endif
    auto        SFTHeaderRec    far *lpSFTBlock;
    auto        SFTEntryRec     far *lpSFTEntry;
    auto        BYTE            byDriveNumber;
    auto        UINT            wFile;
    auto        UINT            wNumOpen;
    auto        LPBYTE          byDeviceDriverHeader;

    byDriveNumber = CharToUpper(byDriveLetter) - 'A';

    wNumOpen = 0;

#ifdef SYM_WIN16
		    /*------------------------------*/
		    /* If VFAT drive, walk OFT list,*/
		    /* getting number of open files */
		    /* found.                       */
		    /*------------------------------*/
    if (DiskIsVFATClient(byDriveLetter))
    {
	auto FI           fi;
	auto UINT         wSel;
	auto LPVFAT311OFT lpV311OFT;

	FP_SEG(lpV311OFT) = wSel = AllocSelector(HIWORD((LPVOID)&fi));
	FP_OFF(lpV311OFT) = 0;

	if (VFATGetFirstOFT(&fi) && wSel) // Traverse OFTs looking for files on this drive
	{
	    do
	    {
		if (fi.fiiDrv == byDriveNumber) // found this drive
		{
		    SetSelectorBase(wSel,fi.filHandle); // set pointer to this OFT

		    if (!bOnlyIfWritten || lpV311OFT->byOpen)
			wNumOpen++; // increment open count
		}
	    } while(VFATGetNextOFT(&fi));
	}

	if (wSel)
	    SelectorFree(wSel);

	return(wNumOpen);
    }
#endif

    lpSFTBlock = GetProtModePtr( DOSGetFirstSFT() );
    while (FP_OFF(lpSFTBlock) != 0xFFFF)
	{
	lpSFTEntry = (SFTEntryRec far *)lpSFTBlock->SFT;
	for (wFile = 0; wFile < lpSFTBlock->wFileCount; wFile++)
	    {
	    switch (_osmajor)
		{
		case 2:
		    if ((lpSFTEntry->Dos2.byNumHandles != 0) &&
			(lpSFTEntry->Dos2.byDrive == (BYTE)(byDriveNumber+1)))
			{
			if (!bOnlyIfWritten || ((lpSFTEntry->Dos2.byDevAttr & 0x40) == 0))
			    wNumOpen++;
			}
		    break;

		default:
		case 3:
		case 4:
		case 5:
		    byDeviceDriverHeader = GetProtModePtr(lpSFTEntry->Dos30.lpDriver);
		    if ((lpSFTEntry->Dos30.wNumHandles != 0) &&
			((lpSFTEntry->Dos30.wDeviceInfo & 0x0080) == 0) &&
			(*byDeviceDriverHeader == byDriveNumber))
			{
			if (!bOnlyIfWritten || ((lpSFTEntry->Dos30.wDeviceInfo & 0x0040) == 0))
			    wNumOpen++;
			}
#ifdef SYM_PROTMODE
		    SelectorFree( FP_SEG( byDeviceDriverHeader ) );
#endif
		    break;
		}

	    (BYTE far *)lpSFTEntry += DOSSFTEntrySize();
	    }


#ifdef SYM_PROTMODE
	lpNextSFTBlock = lpSFTBlock->lpNext;
	SelectorFree( FP_SEG( lpSFTBlock ) );
	lpSFTBlock = GetProtModePtr(lpNextSFTBlock);
#else
	lpSFTBlock = lpSFTBlock->lpNext;
#endif
	}

#ifdef SYM_PROTMODE
    if (FP_SEG(lpSFTBlock) != NULL)
	SelectorFree( FP_SEG( lpSFTBlock ) );
#endif

    return(wNumOpen);
}

/*@API:**********************************************************************
@Declaration: BYTE DOSGetOpenFileName(BYTE nType, WORD wHandle, ASCIIZ pszName,
WORD wPSP)

@Description:
This function obtains the name of an open file from a process or system file
handle. \n


@Parameters:
$nType$ Specifies the type of file handle:  PROCESS_FILE_HANDLE or
SYSTEM_FILE_HANDLE.

$wHandle$ The file handle of the given nType.

$pszName$ ASCIIZ for filename that is returned by the function.

$wPSP$ The PSP's process (only used for PROCESS_FILE_HANDLE types).

@Returns:
The return value is TRUE if the name was obtained; FALSE otherwise.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
BYTE SYM_EXPORT PASCAL DOSGetOpenFileName(int nType, WORD wHandle, BYTE *pszName, WORD wPSP)
{
    auto        OpenFileInfoRec Info;

#if !defined(SYM_WIN)
    wPSP = _psp;
#endif

    if (!DOSGetOpenFileInfo(nType, wHandle, &Info, wPSP))
	return(FALSE);

    STRCPY(pszName, Info.szName);

    return(TRUE);
}

/*@API:**********************************************************************
@Declaration: BYTE DOSGetOpenFileInfo(BYTE nType, WORD wHandle, OpenFileRec far
* lpInfo, WORD wPSP)

@Description:
Obtains information on an open file given its handle only.

@Parameters:
$nType$ Specifies the type of file handle:  PROCESS_FILE_HANDLE or
SYSTEM_FILE_HANDLE.

$wHandle$ The file handle of the given nType.

$lpInfo$ Pointer to a record which receives the information.

$wPSP$ The PSP's process (only used for PROCESS_FILE_HANDLE types).

@Returns:
TRUE is information was obtained.  FALSE if information was not obtained.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
BYTE SYM_EXPORT PASCAL DOSGetOpenFileInfo(int nType, WORD wHandle, OpenFileInfoRec far *lpInfo, WORD wPSP)
{
    auto        SFTEntryRec     far *lpSFT;

#if !defined(SYM_WIN)
    wPSP = _psp;
#endif

    /* Convert process handle to system handle, if needed */
    if (nType == PROCESS_FILE_HANDLE)
	{
	wHandle = DOSGetSystemFileHandle(wHandle, wPSP);
	if (wHandle >= 0x00FF)
	    return(FALSE);
	}

    /* Get pointer to start of SFT entry */
    lpSFT = DOSGetSystemFileTableEntry(wHandle);
    if (lpSFT == NULL)
	return(FALSE);

    /* Convert DOS version-specific info to our format */
    DOSConvertSFTToOurFormat(lpSFT, lpInfo);

#ifdef SYM_PROTMODE
    SelectorFree( FP_SEG( lpSFT ) );
#endif

    return(TRUE);
}

/*@API:**********************************************************************
@Declaration: WORD DOSGetInfoOnAllOpenFiles(WORD wMaxFiles, OpenFileInfoRec far
*lpInfo)

@Description:
Obtains information on all open files and returns it in a buffer list.

@Parameters:
$wMaxFiles$ Specifies the maximum number of info records that lpInfo can hold.

$lpInfo$ Pointer that holds a buffer array of OpenFileInfoRec records.

@Returns:
number of info records obtained.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
/*                                                                      *
 * Enhancement for VFAT 3.11:  Check to see if the drive selected is    *
 * being managed by VFAT.  If so, we traverse the VFAT OFTs and get the *
 * requested information.  Else, we default to traversing the SFTs.     *
 * The modification is in DOSGetInfoOnAllOpenFilesInt() and only when   *
 * a specific drive is specified.  This is because if a specific drive  *
 * is not specified, we have no way of determining an open file which   *
 * is accounted for by a VFAT OFT and an SFT.  Therefore, until we have *
 * the ability to determine a VFAT owned SFT, we will return incomplete *
 * information on a query for all open files on the system.             *
 *----------------------------------------------------------------------*/
WORD SYM_EXPORT PASCAL DOSGetInfoOnAllOpenFiles(WORD wMaxFiles, OpenFileInfoRec far *lpInfo)
{

    return DOSGetInfoOnAllOpenFilesInt(wMaxFiles, lpInfo, EOS);
}

WORD LOCAL PASCAL DOSGetInfoOnAllOpenFilesInt(WORD wMaxFiles, OpenFileInfoRec far *lpInfo, BYTE byDrive)
{
    auto        SFTHeaderRec    far *lpSFTBlock;
    auto        SFTEntryRec     far *lpSFTEntry;
    auto    UINT        wFilesFound=0;
    auto        UINT            wFile;
    auto        WORD            wSFTSeg;
    auto        WORD            wSFTEntry;
    auto        LPVOID          lpFirstSFT;
    auto    BYTE        byDriveNumber;
#ifdef SYM_PROTMODE
    auto        SFTHeaderRec    far *lpNextSFTBlock;
#endif

    byDriveNumber = CharToUpper(byDrive) - 'A';
					/*------------------------------*/
					/* Make sure we don't ask for   */
					/* more files than system has   */
					/*------------------------------*/
    if (wMaxFiles > DOSGetSystemFileHandleCount())
	wMaxFiles = DOSGetSystemFileHandleCount();

#ifdef SYM_WIN16
					/*------------------------------*/
		    /* If VFAT drive, walk OFT list,*/
		    /* getting information on each  */
		    /* file found.                  */
					/*------------------------------*/
    if (DiskIsVFATClient(byDrive))
    {
	auto FI           fi;
	auto UINT         wSel;
	auto LPVFAT311OFT lpV311OFT;

	FP_SEG(lpV311OFT) = wSel = AllocSelector(HIWORD((LPVOID)&fi));
	FP_OFF(lpV311OFT) = 0;

	if (VFATGetFirstOFT(&fi) && wSel) // Traverse OFTs looking for files on this drive
	{
	    do
	    {
		if (fi.fiiDrv == byDriveNumber) // found file on this drive
		{
		    SetSelectorBase(wSel,fi.filHandle);    // set pointer to this OFT
		    ConvertVFAT311OFT(lpV311OFT, lpInfo);  // convertion

		    lpInfo->wSFTEntry = 0xFFFF;       // indicate OFT entry
		    lpInfo->lpSFT = (void far *) fi.filHandle; // pointer to entry

		    lpInfo++;
		    wFilesFound++;
		}

	    } while(VFATGetNextOFT(&fi) && (wFilesFound < wMaxFiles));
	}
	if (wSel)
	    SelectorFree(wSel);

	return(wFilesFound);
    }
#endif

					/*------------------------------*/
					/* Walk SFT list, getting       */
					/* information for each file    */
					/*------------------------------*/
    wFilesFound = 0;
    wSFTEntry = 0;
    lpFirstSFT = DOSGetFirstSFT();
    lpSFTBlock = GetProtModePtr( lpFirstSFT );
    wSFTSeg = FP_SEG(lpFirstSFT);
    while (FP_OFF(lpSFTBlock) != 0xFFFF)
	{
	lpSFTEntry = (SFTEntryRec far *)lpSFTBlock->SFT;
	for (wFile = 0; wFile < lpSFTBlock->wFileCount; wFile++)
	    {
	    DOSConvertSFTToOurFormat(lpSFTEntry, lpInfo);
	    lpInfo->wSFTEntry = wSFTEntry;
	    lpInfo->lpSFT = MK_FP(wSFTSeg, FP_OFF(lpSFTEntry));

	    if (lpInfo->wNumHandles > 0 && !lpInfo->bCharDevice)
		{
		if (byDrive == EOS || lpInfo->byDriveLetter == byDrive)
			{
			lpInfo++;
			wFilesFound++;
			}
		}
	    (BYTE far *)lpSFTEntry += DOSSFTEntrySize();
	    wSFTEntry++;

	    if (wFilesFound == wMaxFiles)
		{
#ifdef SYM_PROTMODE
		SelectorFree( FP_SEG( lpSFTBlock ) );
#endif
		return(wFilesFound);
		}
	    }

	wSFTSeg = FP_SEG(lpSFTBlock->lpNext);

#ifdef SYM_PROTMODE
	lpNextSFTBlock = lpSFTBlock->lpNext;
	SelectorFree( FP_SEG( lpSFTBlock ) );
	lpSFTBlock = GetProtModePtr(lpNextSFTBlock);
#else
	lpSFTBlock = lpSFTBlock->lpNext;
#endif
	}

#ifdef SYM_PROTMODE
    if (FP_SEG(lpSFTBlock) != NULL)
	SelectorFree( FP_SEG( lpSFTBlock ) );
#endif

    return(wFilesFound);
}

/*@API:**********************************************************************
@Declaration: WORD DOSGetOpenFilesOnDrive(BYTE byDrive, LPHANDLE lpHandle)

@Description:
This function obtains information on all open files for a particular drive.

@Returns:
The return value is a handle and a count of the information obtained.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
WORD SYM_EXPORT PASCAL DOSGetOpenFilesOnDrive(BYTE byDrive, LPHANDLE lpHandle)
{
    auto        OpenFileInfoRec far    *lpInfo;
    auto        UINT                    wFilesFound;
    auto        HGLOBAL                 hHandle;

    byDrive = CharToUpper(byDrive);

    wFilesFound = DOSGetSystemFileHandleCount();

    hHandle = MemAlloc(GMEM_MOVEABLE, wFilesFound * sizeof(*lpInfo));

    *lpHandle = hHandle;

    if (hHandle == NULL)
	return (0);

    lpInfo = MemLock(hHandle);

    wFilesFound = DOSGetInfoOnAllOpenFilesInt(wFilesFound, lpInfo, byDrive);

    MemUnlock(hHandle, lpInfo);

    if (wFilesFound != 0)
	{
	*lpHandle = MemReAlloc(hHandle, wFilesFound * sizeof(*lpInfo), GMEM_MOVEABLE);
	}
    else
	{
	MemFree(hHandle);
	*lpHandle = NULL;
	}

    return (wFilesFound);
}

/*@API:**********************************************************************
@Declaration: BYTE DOSIsFileOpen(BYTE byDriveLetter, WORD wStartCluster)

@Description:
This function indicates if a given file is open.

@Parameters:
$byDriveLetter$ [[To be completed]]

$wStartCluster$ [[To be completed]]

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
BYTE SYM_EXPORT PASCAL DOSIsFileOpen(BYTE byDriveLetter, WORD wStartCluster)
{
    auto        SFTHeaderRec    far *lpSFTBlock;
    auto        SFTEntryRec     far *lpSFTEntry;
    auto        BYTE            byDriveNumber;
    auto        UINT            wFile;
    auto        LPBYTE          byDeviceDriverHeader;
#ifdef SYM_PROTMODE
    auto        SFTHeaderRec    far *lpNextSFTBlock;
#endif

    byDriveNumber = (BYTE)(byDriveLetter - 'A');

#ifdef SYM_WIN16
					/*------------------------------*/
		    /* If VFAT drive, walk OFT list */
					/*------------------------------*/
    if (DiskIsVFATClient(byDriveLetter))
    {
	auto FI           fi;

	if (VFATGetFirstOFT(&fi)) // Traverse OFTs looking for files on this drive
	{
	    do
	    {
		if ((fi.fiiDrv == (int)byDriveNumber) &&
		    (fi.fiwSCN == wStartCluster))
		    return (TRUE);

	    } while(VFATGetNextOFT(&fi));
	}

	return(FALSE);
    }
#endif



    lpSFTBlock = GetProtModePtr( DOSGetFirstSFT() );
    while (FP_OFF(lpSFTBlock) != 0xFFFF)
	{
	lpSFTEntry = (SFTEntryRec far *)lpSFTBlock->SFT;
	for (wFile = 0; wFile < lpSFTBlock->wFileCount; wFile++)
	    {
	    switch (_osmajor)
		{
		case 2:
		    if ((lpSFTEntry->Dos2.byNumHandles != 0) &&
			(lpSFTEntry->Dos2.byDrive == (BYTE)(byDriveNumber+1)) &&
			(lpSFTEntry->Dos2.CB.Block.wStartCluster == wStartCluster))
			{
			return(TRUE);
			}
		    break;

		default:
		case 3:
		case 4:
		case 5:
		    byDeviceDriverHeader = GetProtModePtr(lpSFTEntry->Dos30.lpDriver);
		    if ((lpSFTEntry->Dos30.wNumHandles != 0) &&
			((lpSFTEntry->Dos30.wDeviceInfo & 0x0080) == 0) &&
			(*byDeviceDriverHeader == byDriveNumber) &&
			(lpSFTEntry->Dos30.wStartCluster == wStartCluster))
			{
#ifdef SYM_PROTMODE
			SelectorFree( FP_SEG( byDeviceDriverHeader ) );
#endif
			return(TRUE);
			}
#ifdef SYM_PROTMODE
		    SelectorFree( FP_SEG( byDeviceDriverHeader ) );
#endif
		    break;
		}

	    (BYTE far *)lpSFTEntry += DOSSFTEntrySize();
	    }

#ifdef SYM_PROTMODE
	lpNextSFTBlock = lpSFTBlock->lpNext;
	SelectorFree( FP_SEG( lpSFTBlock ) );
	lpSFTBlock = GetProtModePtr(lpNextSFTBlock);
#else
	lpSFTBlock = lpSFTBlock->lpNext;
#endif
	}

#ifdef SYM_PROTMODE
    if (FP_SEG(lpSFTBlock) != NULL)
	SelectorFree( FP_SEG( lpSFTBlock ) );
#endif
    return(FALSE);
}

/*@API:**********************************************************************
@Declaration: WORD DOSGetSystemFileHandle(WORD wProcessHandle, WORD wPSP)

@Description:
This function gets the system file handle from a process file handle.

@Parameters:
$wProcessHandle$ The handle whose matching system file handled is desired.

$wPSP$ The process's PSP value.

@Returns:
The matching system file handle if found, 0x00FF if the process handle isn't
open, 0xFFFF if the process handle is out of range.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle DOSGetSystemFileTableEntry

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
WORD SYM_EXPORT PASCAL DOSGetSystemFileHandle(WORD wProcessHandle, WORD wPSP)
{
    auto        BYTE            far *lpbyHandleTable;
    auto        WORD            wHandle;

#if !defined(SYM_WIN)
    wPSP = _psp;
#endif

    if (wProcessHandle >= DOSGetProcessFileHandleCount(wPSP))
	return( 0xFFFF );

    lpbyHandleTable = GetProtModePtr( *((BYTE far * far *) MAKELP(wPSP, 0x0034)) );

    if (lpbyHandleTable == NULL)
	return( 0xFFFF );

    wHandle = lpbyHandleTable[wProcessHandle] & 0x00FF;

#ifdef SYM_PROTMODE
    SelectorFree( FP_SEG( lpbyHandleTable ) );
#endif

    return( wHandle );

}

/*@API:**********************************************************************
@Declaration: SFTEntryRec far * DOSGetSystemFileTableEntry(WORD wHandle)

@Description:
This function returns a pointer to an SFT entry given a system file handle.

@Parameters:
$wHandle$ A system file handle.

@Returns:
The return value is a pointer to a newly created SFTEntryRec that is filled
with the information from wHandle. If the handle is out of range NULL is
returned. It is the caller's responsibility to dispose of this pointer. \n

It is up to higher levels of code to interpret the SFT entry based on the DOS
version.

@Comments:
For the Windows implementations, the pointer returned is a protected mode
pointer. After accessing this pointer, the caller must free the selector with
which this pointer was created.  To do this, call SelectorFree and pass the
hi-word (by using FP_SEG) as the parameter.

@See: SelectorFree DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
SFTEntryRec far * SYM_EXPORT PASCAL DOSGetSystemFileTableEntry(WORD wHandle)
{
    auto        SFTHeaderRec    far *lpSFTBlock;
#ifdef SYM_PROTMODE
    auto        SFTHeaderRec    far *lpNextSFTBlock;
#endif

    lpSFTBlock = GetProtModePtr( DOSGetFirstSFT() );
    while (FP_OFF(lpSFTBlock) != 0xFFFF)
	{
	if (wHandle < lpSFTBlock->wFileCount)
	    {
	    return((SFTEntryRec far *)(lpSFTBlock->SFT + (DOSSFTEntrySize() * wHandle)));
	    }
	wHandle -= lpSFTBlock->wFileCount;

#ifdef SYM_PROTMODE
	lpNextSFTBlock = lpSFTBlock->lpNext;
	SelectorFree( FP_SEG( lpSFTBlock ) );
	lpSFTBlock = GetProtModePtr(lpNextSFTBlock);
#else
	lpSFTBlock = lpSFTBlock->lpNext;
#endif
	}


    return(NULL);
}

#ifdef SYM_WIN16
/*----------------------------------------------------------------------*
 * VFATGetFirstOFT()                                                    *
 * VFATGetNextOFT()                                                     *
 *                                                                      *
 * SymEvent provides functions that will allow us to traverse the OFT   *
 * list in VFAT.  The reason we kept these functions in SYMEvnt1.DLL    *
 * is to maintain the private interface which between the DLL and VxD.  *
 * In the future, we might create a SYMKrnl.386 which this function     *
 * communicate with directly.                                           *
 *                                                                      *
 *----------------------------------------------------------------------*/
typedef BOOL (WINAPI far *GETVFATOFT)(LPFI);

static GETVFATOFT   lpfnGetFirstOFT,lpfnGetNextOFT;
extern HINSTANCE        hInst_SYMEVENT;

BOOL LOCAL PASCAL VFATGetFirstOFT (LPFI lpFI)
{
#if !defined (NO_SYMEVENT)
    // Load SYMEVENT.DLL if not already loaded
    if (hInst_SYMEVENT == NULL)
	hInst_SYMEVENT = LoadLibrary(QMODULE_SYMEVNT ".DLL");
#endif

    // Get exported function address
    if (hInst_SYMEVENT > HINSTANCE_ERROR && lpfnGetFirstOFT == NULL)
	lpfnGetFirstOFT = (GETVFATOFT) GetProcAddress(hInst_SYMEVENT, "GetFirstOFT");

    // Return NULL if we couldn't load SYMEVENT or find exported function
    if (hInst_SYMEVENT <= HINSTANCE_ERROR || lpfnGetFirstOFT == NULL)
	return(NULL);

    // Call function in SYMEVENT to get first OFT
    return (*lpfnGetFirstOFT)(lpFI);
}

BOOL LOCAL PASCAL VFATGetNextOFT (LPFI lpFI)
{
#if !defined (NO_SYMEVENT)
    // Load SYMEVENT.DLL if not already loaded
    if (hInst_SYMEVENT == NULL)
	hInst_SYMEVENT = LoadLibrary(QMODULE_SYMEVNT ".DLL");
#endif
    
    // Get exported function address
    if (hInst_SYMEVENT > HINSTANCE_ERROR && lpfnGetNextOFT == NULL)
	lpfnGetNextOFT = (GETVFATOFT) GetProcAddress(hInst_SYMEVENT, "GetNextOFT");

    // Return NULL if we couldn't load SYMEVENT or find exported function
    if (hInst_SYMEVENT <= HINSTANCE_ERROR || lpfnGetNextOFT == NULL)
	return(NULL);

    // Call function in SYMEVENT to get next OFT
    return (*lpfnGetNextOFT)(lpFI);
}

#endif


/*@API:**********************************************************************
@Declaration: UINT DOSSFTEntrySize()

@Description:
This function returns the size of an SFT entry based on the DOS version.

@Returns:
The return value is the size of an SFT entry.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
UINT SYM_EXPORT PASCAL DOSSFTEntrySize(void)
{
    switch (_osmajor)
	{
	case 2:
	    return(sizeof(Dos2SFT));

	case 3:
	    switch (_osminor)
		{
		case 0:
		    return(sizeof(Dos30SFT));

		default:
		    return(sizeof(Dos31SFT));
		}

	default:  /* Assume future DOS versions are like DOS 4 and 5 */
	case 4:
	case 5:
	    return(sizeof(Dos4SFT));
	}
}



/*@API:**********************************************************************
@Declaration: SFTHeaderRec far * PASCAL DOSGetFirstSFT()

@Description:
This function returns a pointer to the first SFT block.

@Returns:
The return value is a pointer to the first SFT block.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("leg", off)    /* Can't global optimize inline assembly */
SFTHeaderRec far * SYM_EXPORT PASCAL DOSGetFirstSFT(void)
{
    static      SFTHeaderRec    far *lpRealModeFirstSFTBlock = NULL;

    if (lpRealModeFirstSFTBlock == NULL)
	{
#ifdef  SYM_DOSX
	auto    LPDWORD         lpDWord;
	auto    LPBYTE          lpByte;

	DOSGetInternalVariables(&lpByte);
	lpDWord = GetProtModePtr(lpByte);

	lpRealModeFirstSFTBlock = (SFTHeaderRec far *)lpDWord[1];
	SelectorFree(FP_SEG(lpDWord));
#else
	_asm
	    {
	    mov     ah,52H
	    DOS

	    mov     ax,es:[bx+4]
	    mov     word ptr [lpRealModeFirstSFTBlock],ax
	    mov     ax,es:[bx+6]
	    mov     word ptr [lpRealModeFirstSFTBlock+2],ax
	    }
#endif
	}

    return(lpRealModeFirstSFTBlock);
}

#pragma optimize("", on)

/*@API:**********************************************************************
@Declaration: void DOSConvertSFTToOurFormat(SFTEntryRec far *lpSFTEntry,
OpenFileInfoRec far *lpInfo)

@Description:
This function converts a SFT entry into an OpenFileInfoRec.

@See: DOSGetProcessFileHandleCount DOSGetSystemFileHandleCount
      DOSGetOpenFileCount DOSGetOpenFileName DOSGetOpenFileInfo
      DOSGetInfoOnAllOpenFiles DOSGetSystemFileHandle
      DOSGetSystemFileTableEntry
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
void SYM_EXPORT PASCAL DOSConvertSFTToOurFormat(SFTEntryRec far *lpSFTEntry, OpenFileInfoRec far *lpInfo)
{
    /* Start by clearing out the information record */
    _fmemset(lpInfo, 0, sizeof(OpenFileInfoRec));

    /* Perform DOS version-specific conversion */
    switch (_osmajor)
	{
	case 2:
	    ConvertDos2SFT(&lpSFTEntry->Dos2, lpInfo);
	    break;

	case 3:
	    /* May not work on strange OEM DOS 3's (e.g. Zenith and Wyse) */
	    ConvertDos3SFT(&lpSFTEntry->Dos31, lpInfo);
	    break;

	default:  /* Assume future DOS versions are like DOS 4 and 5 */
	case 4:
	case 5:
	    ConvertDos4SFT(&lpSFTEntry->Dos4, lpInfo);
	    break;
	}
}

/*----------------------------------------------------------------------*
 * ConvertDos2SFT()                                                     *
 *                                                                      *
 * Convert a DOS 2 SFT                                                  *
 *----------------------------------------------------------------------*/

void LOCAL PASCAL ConvertDos2SFT(const Dos2SFT far *lpSFT, OpenFileInfoRec far *lpInfo)
{
    NameToUnpacked(lpInfo->szName, (LPSTR) lpSFT->sName);

    lpInfo->wNumHandles         = lpSFT->byNumHandles;
    lpInfo->bCharDevice         = (BYTE)(lpSFT->byDrive == 0);
    lpInfo->bNetworkFile        = FALSE;
    lpInfo->bModified           = (BYTE)((lpSFT->byDevAttr & 0x40) == 0);
    lpInfo->byDriveLetter       = (BYTE)(lpSFT->byDrive + '@');
    lpInfo->wOpenMode           = lpSFT->byOpenMode;
    lpInfo->byAttr              = lpSFT->byAttr;
    lpInfo->wDate               = lpSFT->wDate;
    lpInfo->wTime               = lpSFT->wTime;
    lpInfo->dwSize              = lpSFT->dwSize;
    lpInfo->dwPosition          = lpSFT->dwPosition;
    lpInfo->wStartCluster       = lpSFT->CB.Block.wStartCluster;
    lpInfo->dwDirSector         = 0xFFFF;
    lpInfo->byDirEntryNumber    = 0xFF;
    lpInfo->wOwnerPSP           = 0xFFFF;
    lpInfo->lpDriver            = lpSFT->CB.Char.lpDriver;
}

/*----------------------------------------------------------------------*
 * ConvertDos3SFT()                                                     *
 *                                                                      *
 * Convert a DOS 3 SFT                                                  *
 *----------------------------------------------------------------------*/

void LOCAL PASCAL ConvertDos3SFT(const Dos31SFT far *lpSFT, OpenFileInfoRec far *lpInfo)
{
    auto        Dos30SFT        far *lpDos30SFT = (Dos30SFT far *)lpSFT;

    lpInfo->wNumHandles         = lpSFT->wNumHandles;
    lpInfo->bCharDevice         = (BYTE)((lpSFT->wDeviceInfo & 0x0080) != 0);
    lpInfo->bNetworkFile        = (BYTE)((lpSFT->wDeviceInfo & 0x8000) != 0);
    lpInfo->bModified           = (BYTE)((lpSFT->wDeviceInfo & 0x0040) == 0);
    lpInfo->byDriveLetter       = (BYTE)((lpSFT->wDeviceInfo & 0x1F) + 'A');
    lpInfo->wOpenMode           = lpSFT->wOpenMode;
    lpInfo->byAttr              = lpSFT->byAttr;
    lpInfo->wDate               = lpSFT->wDate;
    lpInfo->wTime               = lpSFT->wTime;
    lpInfo->dwSize              = lpSFT->dwSize;
    lpInfo->dwPosition          = lpSFT->dwPosition;
    lpInfo->wStartCluster       = lpSFT->wStartCluster;
    lpInfo->dwDirSector         = (DWORD) lpSFT->wDirSector;
    lpInfo->lpDriver            = lpSFT->lpDriver;

    if (_osminor == 0)
	{
	/* Handle DOS 3.0 */
	NameToUnpacked(lpInfo->szName, lpDos30SFT->sName);
	lpInfo->byDirEntryNumber = (BYTE)(lpDos30SFT->wDirEntryOffset / 32);
	lpInfo->wOwnerPSP        = lpDos30SFT->wOwnerPSP;
	}
    else
	{
	/* Handle DOS 3.1 - 3.3 */
	NameToUnpacked(lpInfo->szName, (LPSTR) lpSFT->sName);
	lpInfo->byDirEntryNumber = lpSFT->byDirEntryNumber;
	lpInfo->wOwnerPSP        = lpSFT->wOwnerPSP;
	}

    if (_osminor == 31)
	{
	/* Handle Compaq DOS 3.31 and DRDOS (which emulates 3.31) */
	lpInfo->dwDirSector = (DWORD) lpSFT->wDirSector +
			      ((DWORD)(lpSFT->byDirEntryNumber & 0xF0)) << 12;
	lpInfo->byDirEntryNumber = (BYTE)(lpSFT->byDirEntryNumber & 0x0F);
	}
}

/*----------------------------------------------------------------------*
 * ConvertDos4SFT()                                                     *
 *                                                                      *
 * Convert a DOS 4 or 5 SFT                                             *
 *----------------------------------------------------------------------*/

void LOCAL PASCAL ConvertDos4SFT(const Dos4SFT far *lpSFT, OpenFileInfoRec far *lpInfo)
{
    NameToUnpacked(lpInfo->szName, (LPSTR) lpSFT->sName);

    lpInfo->wNumHandles         = lpSFT->wNumHandles;
    lpInfo->bCharDevice         = (BYTE)((lpSFT->wDeviceInfo & 0x0080) != 0);
    lpInfo->bNetworkFile        = (BYTE)((lpSFT->wDeviceInfo & 0x8000) != 0);
    lpInfo->bModified           = (BYTE)((lpSFT->wDeviceInfo & 0x0040) == 0);
    lpInfo->byDriveLetter       = (BYTE)((lpSFT->wDeviceInfo & 0x1F) + 'A');
    lpInfo->wOpenMode           = lpSFT->wOpenMode;
    lpInfo->byAttr              = lpSFT->byAttr;
    lpInfo->wDate               = lpSFT->wDate;
    lpInfo->wTime               = lpSFT->wTime;
    lpInfo->dwSize              = lpSFT->dwSize;
    lpInfo->dwPosition          = lpSFT->dwPosition;
    lpInfo->wStartCluster       = lpSFT->wStartCluster;
    lpInfo->dwDirSector         = lpSFT->dwDirSector;
    lpInfo->byDirEntryNumber    = lpSFT->byDirEntryNumber;
    lpInfo->wOwnerPSP           = lpSFT->wOwnerPSP;
    lpInfo->lpDriver            = lpSFT->lpDriver;
}

#ifdef SYM_WIN
/*----------------------------------------------------------------------*
 * ConvertVFAT311OFT()                                                  *
 *                                                                      *
 * Convert a DOS 4 or 5 SFT                                             *
 *----------------------------------------------------------------------*/

void LOCAL PASCAL ConvertVFAT311OFT(const VFAT311OFT far *lpOFT, OpenFileInfoRec far *lpInfo)
{
    auto UINT           wSel;
    auto LPVFAT311VT    lpVT;

    FP_SEG(lpVT) = wSel = AllocSelector(HIWORD((LPVOID)&wSel));
    FP_OFF(lpVT) = 0;

    SetSelectorBase(wSel, lpOFT->dwVolTable);

    NameToUnpacked(lpInfo->szName, (LPSTR) lpOFT->sName);

    lpInfo->wNumHandles   = 1;      // cannot get this yet
    lpInfo->bCharDevice   = FALSE;  // VFAT 3.11 does not handle char devices
    lpInfo->bNetworkFile  = FALSE;  // VFAT only handles local files
    lpInfo->bModified     = lpOFT->byOpen; // open for write (modified?)
    if (wSel)
	lpInfo->byDriveLetter = (BYTE) lpVT->byDrvID + 'A';
    lpInfo->wOpenMode     = NULL;  // cannot get this yet
    lpInfo->byAttr        = NULL;  // cannot get this yet
    lpInfo->wDate         = NULL;  // cannot get this yet
    lpInfo->wTime         = NULL;  // cannot get this yet
    lpInfo->dwSize        = NULL;  // cannot get this yet
    lpInfo->dwPosition    = NULL;  // cannot get this yet
    lpInfo->wStartCluster = lpOFT->wStartCluster;
    lpInfo->dwDirSector   = lpOFT->dwDirSector;
    lpInfo->byDirEntryNumber = lpOFT->byDirOffset & 0x0f; // only low nibble valid
    lpInfo->wOwnerPSP     = NULL;  // cannot get this yet
    lpInfo->lpDriver      = NULL;  // cannot get this yet

    if (wSel)
	SelectorFree(wSel);
}
#endif
