/* Copyright 1993 Symantec Corporation                                  */
/*----------------------------------------------------------------------*
 *                                                                      *
 * $Header:   S:/SYMRSQ/VCS/system.cpv   1.3   16 Mar 1998 16:56:54   MKEATIN  $ *
 *                                                                      *
 * Description:                                                         *
 *   Handles system files for formatting                                *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 *----------------------------------------------------------------------*
 * $Log:   S:/SYMRSQ/VCS/system.cpv  $ *
// 
//    Rev 1.3   16 Mar 1998 16:56:54   MKEATIN
// Made changes to match those made in the new disk.h
// 
//    Rev 1.2   12 Aug 1996 16:36:20   JBRENNA
// LuigiPlus Port:
//   GWESTER: Fixed broken 2nd and 3rd Rescue disks.
//   GWESTER: Fixed boot disk creation on FAT16 for DOS 7.1.
//   DALLEE: Use FAT32 (Win95-Style) boot sector in place of NT's whammy-dyne
//     boot code for dual boots from FAT volumes.
// 
//    Rev 1.1   12 Aug 1996 13:56:08   JBRENNA
// Sync with CORE \ RESCUEW \ system.cpp. It now compiles again.
// 
//    Rev 1.0   07 May 1996 17:09:06   DALLEE
// Initial revision.
// 
//    Rev 1.19.2.1   05 Jul 1995 09:58:30   GEORGEL
// Set the progress bar to 0 before copying system files
// 
//    Rev 1.19.2.0   27 Jun 1995 18:46:06   RJACKSO
// Branch base for version QAK8
// 
//    Rev 1.19   11 Apr 1995 17:42:34   Basil
// Merge of quake 6 changes
// 
//    Rev 1.18.1.7   27 Mar 1995 14:57:56   GEORGEL
// Fixed 33315 32492
// 
//    Rev 1.18.1.6   20 Mar 1995 12:23:28   GEORGEL
// Kludge to get 16-bit version to build
// 
//    Rev 1.18.1.5   17 Mar 1995 18:12:14   GEORGEL
// Fixed 32719
// 
//    Rev 1.18.1.4   01 Mar 1995 03:30:28   GEORGEL
// Fixed 31368
// 
//    Rev 1.18.1.3   19 Feb 1995 01:11:26   GEORGEL
// For Win32 MSDOS.SYS should just be a 0 byte file
// 
//    Rev 1.18.1.2   09 Feb 1995 20:29:36   MARKK
// Work to make better
// 
//    Rev 1.18.1.1   03 Feb 1995 16:57:34   MARKK
// Removed all thunk stuff
// 
//    Rev 1.18.1.0   13 Jan 1995 20:22:06   SCOTTP
// Branch base for version QAK6
// 
//    Rev 1.18   17 Nov 1994 14:30:34   GEORGEL
// If SYSing a floppy for Win95, don't put MSDOS.SYS on the floppy.
// 
//    Rev 1.17   17 Nov 1994 02:18:34   Basil
// Thunking changes
//
//    Rev 1.16   07 Oct 1994 15:07:08   BRAD
// Added NEC support
//
//    Rev 1.15   04 Oct 1994 14:12:18   brianf
// Ifdef'd memset call.
//
//    Rev 1.14   30 Sep 1994 08:17:32   GEORGEL
//
//    Rev 1.13   18 Sep 1994 16:59:56   GEORGEL
// Kludged it to work with SYMKRNL
//
//    Rev 1.12   06 May 1994 11:05:24   HENRI
// Added ExternFAT variable
//
//    Rev 1.11   04 May 1994 01:10:42   BRAD
// Don't use NFileOpen()
//
//    Rev 1.10   26 Mar 1994 23:39:32   BRAD
// Use portable windows messages
//
//    Rev 1.9   26 Mar 1994 14:28:24   BRAD
// New date/time
//
//    Rev 1.8   15 Mar 1994 11:08:46   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.7   25 Feb 1994 13:00:16   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.6   08 Dec 1993 17:43:28   DAVID
// Created a DOS version of SYMFMTx.LIB containing SFReportSystemFiles().
//
//    Rev 1.5   08 Sep 1993 19:31:42   DAVID
// Put back all of the ClipToFillBarDialog() and UnClip() calls from
// earlier file revisions, but replaced the guts of those routines with
// code that turns the system modal state of the fill bar dialog
// on and off in place of the cursor clipping calls.  This is better
// than the previous attempt at making the fill bar dialog system modal,
// because the error messages are no longer unmoveable.  NDW's DiskCopy
// uses the same technique.
//
//    Rev 1.4   07 Sep 1993 15:19:56   DAVID
// [FIX]  Made sure that all message boxes have an icon.
//
//    Rev 1.3   16 Jul 1993 12:11:38   BASIL
// Removed cursor clipping
//
//    Rev 1.2   25 Jun 1993 15:40:18   BASIL
// changed struct sysfileinfo name to SFsysfileinfo
//
//    Rev 1.1   25 Jun 1993 11:26:38   BASIL
// Added support for dblspace.bin
//
//    Rev 1.0   17 Jun 1993 18:10:20   BASIL
// Initial revision.
 *----------------------------------------------------------------------*/

#include <windows.h>
#include <memory.h>
#include <string.h>
#include "platform.h"
#include "xapi.h"

#include "disk.h"
#include "file.h"
#include "msgdisp.h"

#include "formatst.h"

#include "symfmt.h"
#include "format.h"
#include "ngui.h"                    /* Definitions for fill bar stuff            */


#define BOOT_SECTOR_LENGTH    512

/*----------------------------------------------------------------------*/
/* Externs                                                              */
/*----------------------------------------------------------------------*/

extern void   YieldCpu(void);
extern AbsDiskRec adr;
#if 0
extern "C" BYTE  _cdecl   bootProgram[];
extern "C" WORD  _cdecl   bootProgramSize;
#endif

/*----------------------------------------------------------------------*/
/* Local Prototypes                                                     */
/*----------------------------------------------------------------------*/

BOOL LOCAL CreateAndWriteFile(SFSysFileRec *sysFilePtr);
WORD LOCAL CreateEmptyFAT(VOID);
WORD LOCAL WriteBootRecord(VOID);
WORD LOCAL WriteFats(VOID);
WORD LOCAL InitializeFAT (VOID);
WORD LOCAL CountBadClusters(VOID);
WORD LOCAL GetExistingFAT(VOID);
VOID LOCAL LogicalSectorToPhysicalSector (DWORD sector, LPDISKREC diskPtr, LPABSDISKREC absDiskPtr);

BOOL WINAPI NavFmt_CanSupportExtendedIO();

/*----------------------------------------------------------------------*/
/* Local Variables                                                      */
/*----------------------------------------------------------------------*/
DWORD dwFirstBadCluster;                /* First bad cluster on disk        */
WORD wBadClusters;                      /* Total bad clusters               */
char szVolumeLabel[VOLUME_LENGTH+1];

/*----------------------------------------------------------------------*/
/* WriteBootAndFAT                                                      */
/*        Writes the boot record and FAT out.                           */
/*                                                                      */
/*        Returns:                                                      */
/*                NOERR            if no error                          */
/*                ERROR_BOOT       if error writing boot record         */
/*                ERROR_FAT        if error writing Fat                 */
/*----------------------------------------------------------------------*/

WORD WriteBootAndFAT (VOID)
{
    extern SFStateRec ExternState;
    auto   WORD       returnValue;


    returnValue = NOERR;
                                        /*------------------------------*/
                                        /* Don't write out new system   */
                                        /*        files if simulation   */
                                        /*        mode                  */
                                        /*------------------------------*/
    if (!ExternState.simulation)
    {
                                        /*------------------------------*/
                                        /* Count bad clusters before    */
                                        /*    writing                   */
                                        /*------------------------------*/
        wBadClusters = CountBadClusters ();
                                        /* Write out new boot record    */
        if (WriteBootRecord() != NOERR)
            returnValue = ERROR_BOOT;
        else if (WriteFats() != NOERR)  /* Write out new Fat            */
            returnValue = ERROR_FAT;
    }

    return (returnValue);
}



/*----------------------------------------------------------------------*/
// Perform operation, using physical disk i/o, rather than logical.
/*----------------------------------------------------------------------*/
UINT SYM_EXPORT DiskBlockDeviceProcNEC(UINT wCommand,
                                       LPDISKREC lpDiskRec,
                                       DWORD     FAR *lpdwStartSector,
                                       WORD      FAR *lpwSectorsToRead,
                                       LPVOID    lpvBuffer,
                                       DWORD     dwBufferSegSel,
                                       DWORD     dwBigFootSegSel )
{
    BYTE        byErr;
    UINT        uRet;


                                        /*------------------------------*/
                                        /* Write the new root directory,*/
                                        /* 1 sector at a time.          */
                                        /*------------------------------*/
    adr.sectorLenCode = (BYTE)(lpDiskRec->bytesPerSector / 256 );
    if ( adr.sectorLenCode >= 3 )
        adr.sectorLenCode = 3;
    LogicalSectorToPhysicalSector (*lpdwStartSector, lpDiskRec, &adr);
    adr.numSectors = (BYTE) *lpwSectorsToRead;
    adr.buffer     = lpDiskRec->buffer;
    byErr = DiskAbsOperation (WRITE_COMMAND, &adr);
    uRet = 0;
    if (byErr)
        {
        if (byErr = 0x60)
            uRet = TIME_OUT;
        else if (byErr == 0x70)
            uRet = WRITE_PROTECTED;
        else
            uRet = DISK_PHYSICAL_ERROR;
        }
    return (uRet);
}


/*----------------------------------------------------------------------*/
/* WriteRootDir                                                         */
/*        Writes out a new root directory of the disk.                  */
/*----------------------------------------------------------------------*/
WORD WriteRootDir (VOID)
{
    extern DiskRec      ExternDiskRec;
    extern SFStateRec   ExternState;
    auto   WORD         returnValue;
    auto   GLOBALHANDLE hSectorBuffer;
    auto   LPBYTE       lpSectorBuffer;
    auto   WORD         i;



    returnValue = NOERR;

    hSectorBuffer = MemAlloc(GMEM_MOVEABLE + GMEM_ZEROINIT,
                            ExternDiskRec.bytesPerSector);
    if (hSectorBuffer == NULL)
        return(DISK_MEMORY_ERROR);

    lpSectorBuffer = (LPBYTE)MemLock(hSectorBuffer);
    if (lpSectorBuffer == NULL)
        {
        MemFree(hSectorBuffer);
        return(DISK_MEMORY_ERROR);
        }
                                    /*------------------------------*/
                                    /* Write the new root directory,*/
                                    /* 1 sector at a time.          */
                                    /*------------------------------*/
#ifndef	SYM_WIN32
    if (HWIsNEC())
        DiskBlockSetProc(DiskBlockDeviceProcNEC);
#endif

    ExternDiskRec.sectorCount = 1;
    ExternDiskRec.buffer = lpSectorBuffer;
    for (i = ExternDiskRec.startOfRootDir; i < ExternDiskRec.dwStartOfDataEx; i++)
        {
        ExternDiskRec.sector = i;
        if (DiskBlockDevice(BLOCK_WRITE, &ExternDiskRec))
            {
            returnValue = DISK_PHYSICAL_ERROR;
            break;
            }
        }

#ifndef	SYM_WIN32
    if (HWIsNEC())
        DiskBlockClearProc(ExternDiskRec.dl, DiskBlockDeviceProcNEC);
#endif

                                    /*------------------------------*/
                                    /* Free sector buffer           */
                                    /*------------------------------*/
    MemUnlock(hSectorBuffer);
    MemFree(hSectorBuffer);

    return(returnValue);
}




/*----------------------------------------------------------------------*/
/* SetVolume                                                            */
/*        Sets the disk volume label to the 11-BYTE label               */
/*        specified in the global string, 'szVolumeLabel'               */
/*                                                                      */
/*        Returns:                                                      */
/*                NOERR                    no error                     */
/*                DISK_MEMORY_ERROR        Memory error                 */
/*                DISK_PHYSICAL_ERROR      Physical error               */
/*                ERR                      Can't find spot for label    */
/*----------------------------------------------------------------------*/

WORD SetVolume (VOID)
{
    extern DiskRec    ExternDiskRec;
    extern SFStateRec ExternState;
    extern char       szVolumeLabel[];
    auto   WORD       wReturnValue;


    wReturnValue = NOERR;
    if (!ExternState.simulation)
    {
        wReturnValue = VolumeLabelSet(ExternDiskRec.dl, TRUE, szVolumeLabel);
    }

    return(wReturnValue);
}



/*----------------------------------------------------------------------*/
/* WriteSystemFiles()                                                   */
/*        Copies the saved system file images from memory to the disk.  */
/*                                                                      */
/*        Returns:                                                      */
/*                NOERR                if successful                    */
/*                UNSUITABLE_SYS       if no free space                 */
/*                BAD_SYS              if error                         */
/*----------------------------------------------------------------------*/

WORD WriteSystemFiles (VOID)
{
    auto char             OldProgressMessage[MAX_SCRATCH];
    auto char             NewProgressMessage[MAX_SCRATCH];
    extern   DiskRec      ExternDiskRec;
    extern   SFStateRec   ExternState;
    extern   SFSysInfoRec ExternSysInfo;
    register WORD         i;
    auto     DWORD        sizeNeeded;
    auto     DWORD        dwClusters;
    auto     WORD         wRet=NOERR;


    if (ExternState.simulation)         /* If simulation, skip          */
        return (NOERR);
                                        /*------------------------------*/
                                        /* Make sure there is enough    */
                                        /*   space on the dest. disk    */
                                        /*   for the system files at    */
                                        /*   the beginning of the disk  */
                                        /*------------------------------*/
    sizeNeeded = ExternSysInfo.sysFile [BIOS_COM_FILE].size +
      ExternSysInfo.sysFile [DOS_COM_FILE].size;
    dwClusters = ConvertSizeToCluster (&ExternDiskRec, sizeNeeded);

                                        /*------------------------------*/
                                        /* First clusters on disk free? */
                                        /*------------------------------*/
    if (dwFirstBadCluster <= (dwClusters + 1))
    {
        wRet = UNSUITABLE_SYS;
        goto LocRet;
    }

    GetDlgItemText(
      hFillDialog,
      DCF_PROGRESS_LABEL,
      (LPSTR)OldProgressMessage,
      MAX_SCRATCH);

                                        /*--------------------------------------*/
                                        /* Set title to Copying System Files... */
                                        /*--------------------------------------*/
    NMsgGet(hinst_SYMFMT,FORMAT_COPYING_SYSTEM, NewProgressMessage, MAX_SCRATCH);
    SetWindowText(hFillDialog, (LPSTR) NewProgressMessage);

    if (SendMessage(hFillDialog, UWM_FB_SET_FILL_LEVEL, (WPARAM)0, 0L))
        wGlobalStopFormat = TRUE;

                                        /*------------------------------*/
                                        /* Write out system files       */
                                        /*------------------------------*/
    ClipToFillBarDialog();
    for (i = 0; i < ExternSysInfo.wSysFileCount; i++)
    {

                                        /* Set drive letter to the one  */
                                        /*        we just formatted     */
        ExternSysInfo.sysFile [i].fileName [0] = ExternDiskRec.dl;

                                        /* Write the system file        */
        if (!CreateAndWriteFile(&(ExternSysInfo.sysFile [i])))
        {
            wRet = BAD_SYS;
            goto LocRet;
        }
                                        /*------------------------------*/
                                        /* Set Appropriate attributes   */
                                        /*------------------------------*/
        FileSetAttr (ExternSysInfo.sysFile [i].fileName,
                     ExternSysInfo.sysFile [i].wAttributes);

        if (SendMessage(hFillDialog, UWM_FB_SET_FILL_LEVEL,
			(WPARAM)((i+1)*100/ExternSysInfo.wSysFileCount), 0L))
	    wGlobalStopFormat = TRUE;

        YieldCpu();

        if (wGlobalStopFormat)
            break;
    }

LocRet:

    UnClip();

    SetDlgItemText(
      hFillDialog,
      DCF_PROGRESS_LABEL,
      (LPSTR)OldProgressMessage);
    return (NOERR);                     /* Successful transfer                */
}



/*----------------------------------------------------------------------*/
/* CreateAndWriteFile                                                   */
/*        Creates a system file and writes its contents out to the new  */
/*        formatted disk.                                               */
/*                                                                      */
/*        Returns TRUE if successful, else FALSE                        */
/*----------------------------------------------------------------------*/

BOOL LOCAL CreateAndWriteFile (SFSysFileRec *sysFilePtr)
{
    auto HFILE  handle;
    auto HPBYTE hpBuffer;
    auto BOOL   returnValue;



    returnValue = TRUE;
                                        /*------------------------------*/
                                        /* Create file first            */
                                        /*------------------------------*/
    if ((handle = FileCreate (sysFilePtr->fileName, 0)) == HFILE_ERROR)
        return (FALSE);

    sysFilePtr->size = sysFilePtr->size;  /* Get size of file             */

                                        /*------------------------------*/
                                        /* Lock buffer down             */
                                        /*------------------------------*/
    hpBuffer = (HPBYTE)MemLock(sysFilePtr->hBuffer);
    if (hpBuffer == NULL)
    {
        FileClose(handle);
        return(FALSE);
    }


                                        /*------------------------------*/
                                        /* If WIN32, then MSDOS.SYS should */
                                        /* be 0 length (this is how     */
                                        /* Microsoft does it anyway)    */
                                        /*------------------------------*/
#ifdef SYM_WIN32
                                        /* For Win95, skip MSDOS.SYS   */
    char szFilename[SYM_MAX_PATH];
    NameReturnFile(sysFilePtr->fileName, szFilename);
    STRUPR(szFilename);
    if (STRCMP(szFilename, "MSDOS.SYS") != 0)
    {
#endif
                                        
                                        /* Write file data              */
    if (FileHugeWrite (handle, hpBuffer, sysFilePtr->size) != sysFilePtr->size)
        returnValue = FALSE;
#ifdef SYM_WIN32
    }
#endif
        
    MemUnlock(sysFilePtr->hBuffer);

    if (returnValue)
    {
        FILETIME        fileTime;
                                        /* Set date/time of file        */
        DosTimeToFileTime(sysFilePtr->date, sysFilePtr->time, &fileTime);
        FileSetTime (handle, TIME_LASTWRITE, &fileTime);
    }

    FileClose (handle);
    
    return (returnValue);

}


/*----------------------------------------------------------------------*/
/* GetFAT                                                               */
/*        Gets the FAT.                                                 */
/*----------------------------------------------------------------------*/

WORD GetFAT (BOOL bUseOldFAT)
{
    auto WORD wResult;
    auto WORD wReturnValue;

                                        /*------------------------------*/
                                        /* Should we use the old FAT?   */
                                        /*------------------------------*/
    wReturnValue = NOERR;
    if (bUseOldFAT)
    {
                                        /*------------------------------*/
                                        /* We only do this if QUICK mode*/
                                        /* and the disk is preformatted */
                                        /*------------------------------*/
        wResult = GetExistingFAT();
        if (wResult != NOERR)
        {
            if (wResult == DISK_MEMORY_ERROR)
            {
                //then tell user that not enough memory for Quick Format,
                wResult = NMessageDisplay(hinst_SYMFMT,
                    hFillDialog,
                    FORMAT_OUT_OF_MEMORY,
                    FORMAT_CAPTION,
                    MB_OK | MB_ICONINFORMATION,
                    NULL,
                    NULL);
            }
            else
            {
                if (wResult == DISK_PHYSICAL_ERROR)
                {
                    // Then tell user that error reading existing FAT
                    // and disk can't be quick formatted, but can be
                    // DOS formatted.

                wResult = NMessageDisplay(hinst_SYMFMT,
                    hFillDialog,
                    FORMAT_ERROR_READING_FAT,
                    FORMAT_CAPTION,
                    MB_OK | MB_ICONINFORMATION,
                    NULL,
                    NULL);

                }
            }
            wReturnValue = ERR;         /* Return Error                 */
        }
    }
    else //bUseOldFAT == FALSE
    {
                                        /*------------------------------*/
                                        /* Create empty FAT             */
                                        /*------------------------------*/
        if (InitializeFAT() != NOERR)
        {
                wResult = NMessageDisplay(hinst_SYMFMT,
                    hFillDialog,
                    FORMAT_OUT_OF_MEMORY,
                    FORMAT_CAPTION,
                    MB_OKCANCEL | MB_ICONINFORMATION,
                    NULL,
                    NULL);
            wReturnValue = ERR;         /* Return Error                 */
        }
    }

    return (wReturnValue);              /* No error                     */
}




/*----------------------------------------------------------------------*/
/* InitializeFAT                                                        */
/*        Allocates enough space for a FAT for the disk and sets it     */
/*        to empty state.                                               */
/*                                                                      */
/*        RETURNS:                                                      */
/*                NOERR     if no error                                 */
/*                ERR       Memory error                                */
/*----------------------------------------------------------------------*/

WORD LOCAL InitializeFAT (VOID)
{
    extern DiskRec ExternDiskRec;
    extern FAT	   ExternFAT;
    auto   WORD    wReturnValue;



    wReturnValue = ERR;

    FATFree(&ExternFAT);

    if (FATAlloc(&ExternDiskRec, &ExternFAT) == NOERR)
        wReturnValue = CreateEmptyFAT();

    if (wReturnValue != NOERR)
        FATFree(&ExternFAT);

    return(wReturnValue);
}



/*----------------------------------------------------------------------*/
/* CreateEmptyFAT                                                       */
/*        Clears a FAT table, complete with correct media descriptor    */
/*        byte.                                                         */
/*                                                                      */
/*        The FAT is locked in memory.                                  */
/*                                                                      */
/*        RETURNS:                                                      */
/*                ERR                Can't lock FAT buffer              */
/*                NOERR                No error                         */
/*----------------------------------------------------------------------*/

WORD LOCAL CreateEmptyFAT (VOID)
{
    extern DiskRec ExternDiskRec;
    extern FAT	   ExternFAT;
    auto   DWORD   dwIndex;
    auto   WORD    wReturnValue;
    auto   HPBYTE  lpFAT;
    auto   HPBYTE  lpFATDup;




    wReturnValue = ERR;
    lpFATDup = lpFAT = (HPBYTE) ExternFAT.hpData;
                                        /*------------------------------*/
                                        /* Clear entire FAT with 0's    */
                                        /*------------------------------*/
    for (dwIndex = 0; dwIndex < ExternDiskRec.dwSectorsPerFATEx; dwIndex++)
        {
#ifdef SYM_WIN16           // memset will not work for Win16, use _fmemset.
            _fmemset(lpFATDup, 0, ExternDiskRec.bytesPerSector);
#else
            memset(lpFATDup, 0, ExternDiskRec.bytesPerSector);
#endif
            lpFATDup += ExternDiskRec.bytesPerSector;
        }
                                        /*------------------------------*/
                                        /* Set up media descriptor      */
                                        /*------------------------------*/
    lpFAT[0] = ExternDiskRec.mediaDescriptor;
                                    /* Put in reserved FF's         */
    *((HPWORD) (lpFAT + 1)) = 0xFFFF;

                                        /*------------------------------*/
                                        /* If 16-bit, need 1 more       */
                                        /*------------------------------*/
    if (IsFAT16(ExternDiskRec))
        lpFAT[3] = 0xFF;
                                        /*------------------------------*/
                                        /* Unlock the FAT buffer        */
                                        /*------------------------------*/
    wReturnValue = NOERR;

    return(wReturnValue);
}




/*----------------------------------------------------------------------*/
/* CountBadClusters                                                     */
/*        Returns the number of bad clusters marked in the Fat.         */
/*----------------------------------------------------------------------*/

WORD LOCAL CountBadClusters (VOID)
{
    extern   DiskRec ExternDiskRec;
    extern   FAT     ExternFAT;
    extern   DWORD   dwFirstBadCluster;
    auto     DWORD   dwIndex;
    auto     WORD    count;



    count = 0;
    dwFirstBadCluster = END_CLUSTER;      /* Make it worst case                */
    for (dwIndex = 2; dwIndex <= ExternDiskRec.dwMaxClusterEx; dwIndex++)
    {
        if (FATGet (&ExternFAT, dwIndex) == BAD_CLUSTER)
        {                               /* Find first bad cluster        */
            if (dwFirstBadCluster == END_CLUSTER)
                dwFirstBadCluster = dwIndex;
            count++;
        }
    }

    return(count);
}



/*----------------------------------------------------------------------*/
/* WriteBootRecord                                                      */
/*        Puts a valid DOS boot record at DOS sector 0.                 */
/*                                                                      */
/*        Returns:                                                      */
/*                NOERR                    No error                     */
/*                DISK_MEMORY_ERROR        Memory error                 */
/*                DISK_PHYSICAL_ERROR      Physical error               */
/*----------------------------------------------------------------------*/

WORD LOCAL WriteBootRecord (VOID)
{
    auto   char           zCaption[MAX_CAPTION];
    auto   char           zScratch[MAX_SCRATCH];
    auto   WORD           wResult;
    extern DiskRec        ExternDiskRec;
    extern BPBRec         ExternBPB;
    extern SFSysInfoRec   ExternSysInfo;
    extern SFStateRec     ExternState;
    auto   BYTE           baseDrive;
    auto   WORD           sizeToMove;
    auto   WORD           dosVersion;
    auto   LPBYTE         lpBuffer = NULL;
    auto   WORD           wErr;
    auto   WORD           wRet;
    auto   char           szTemp[30];

    auto  char            zDriveName[3];

    zDriveName[0] = ExternDiskRec.dl;
    zDriveName[1] = ':';
    zDriveName[2] = EOS;

                                        /*------------------------------*/
                                        /* Skip writing out a new boot, */
                                        /* if we can.                   */
                                        /* We can if we aren't          */
                                        /* physically formatting the    */
                                        /* diskette AND not SYSing      */
                                        /* the diskette.                */
                                        /*------------------------------*/
    if (!ExternState.bDoFormat && !(ExternSysInfo.validSysInfo && (ExternState.system == SYS)))
        return (NOERR);

                                        /*------------------------------*/
                                        /* Allocate space for the Boot  */
                                        /* Record.                      */
                                        /*------------------------------*/
    lpBuffer = (LPBYTE)MemAllocPtr(GHND, ExternDiskRec.bytesPerSector);
    if (lpBuffer == NULL)
        {
        wRet = DISK_MEMORY_ERROR;
        goto locRet;
        }


    if (HWIsNEC())
        {
        auto LPBYTE lpBootBuffer;

#ifndef	SYM_WIN32
        DiskBlockSetProc(DiskBlockDeviceProcNEC);
#endif
                                    /*------------------------------*/
                                    /*  NEC, write the boot record  */
                                    /*     from the DOS disk        */
                                    /*------------------------------*/
        lpBootBuffer = (LPBYTE)MemLock(ExternSysInfo.hBootBuffer);
        if (lpBootBuffer == NULL)
            {
            wRet = DISK_MEMORY_ERROR;
            goto locRet;
            }

        MEMMOVE(lpBuffer, lpBootBuffer, ExternDiskRec.bytesPerSector);

        MemUnlock(ExternSysInfo.hBootBuffer);

        baseDrive = 0;
        sizeToMove = 19;
        }
    else
        {
        #define NT_BOOT_SIG_OFFSET  0x1DC

        static  BYTE    SZ_NT_BOOT_SIG [] = "NTLDR";
        auto    BOOL    bHaveBootSector = FALSE;

        dosVersion = DOSGetVersion();

#ifndef BOOTPLAN
        if ( (FALSE == NavFmt_CanSupportExtendedIO())
#else
                                        // For safety, we assume that this is FAT32
                                        // until we can prove otherwise:
        auto BOOL bIsFAT32Case = IsFAT32Case ();

        if ( (FALSE == bIsFAT32Case)
#endif
        &&  ExternSysInfo.validSysInfo && (ExternState.system == SYS))
            {
            auto LPBYTE lpBootBuffer;

                                        /*------------------------------*/
                                        /*  SYS, write the boot record  */
                                        /*     from the DOS disk        */
                                        /*------------------------------*/
            lpBootBuffer = (LPBYTE)MemLock(ExternSysInfo.hBootBuffer);
            if (lpBootBuffer == NULL)
                {
                wRet = DISK_MEMORY_ERROR;
                goto locRet;
                }

                                        // If we're on NT (yeah, right),
                                        // or this isn't NT FAT boot code
                                        // go ahead and write it.
                                        // Else, if it's not NT, but this is
                                        // NT FAT boot code, then we want
                                        // to use the pre-canned version.
            if ( (SYM_SYSTEM_WIN_NT == SystemGetWindowsType()) ||
                 MEMCMP( lpBootBuffer + NT_BOOT_SIG_OFFSET,
                         SZ_NT_BOOT_SIG,
                         sizeof(SZ_NT_BOOT_SIG) - 1 ) )
                {
                MEMMOVE(lpBuffer, lpBootBuffer, ExternDiskRec.bytesPerSector);

                bHaveBootSector = TRUE;
                }

            MemUnlock(ExternSysInfo.hBootBuffer);
            }
        if (!bHaveBootSector)
            {
                                        /*------------------------------*/
                                        /* Either not SYS, or FAT32     */
                                        /* non-floppy-bootable sector.  */
                                        /* Therefore write our custom   */
                                        /* boot program to the boot     */
                                        /* record.                      */
                                        /*------------------------------*/

                                        /*------------------------------*/
                                        /* Move the boot program from   */
                                        /*   the resource file, into    */
                                        /*   the write buffer           */
                                        /*------------------------------*/
            HRSRC hRes;
            HGLOBAL hBoot;
            LPVOID  lpBoot;

            if (HWIsNEC())
                {
                hRes = FindResource(hinst_SYMFMT, "NEC_BOOT_SECTOR", "NECBOOTSECTOR");// No Localization Needed
                }
            else if (SYM_SYSTEM_WIN_95 == SystemGetWindowsType())
                {
#ifndef BOOTPLAN
                hRes = FindResource(hinst_SYMFMT, "F32BOOT_SECTOR", "F32BOOTSECTOR");// No Localization Needed
#else
                                        // If we don't have a FAT32 boot sector available,
                                        // use the FAT16 one on non-SYS disks:
                hRes = FindResource(hinst_SYMFMT, "BOOT_SECTOR", "BOOTSECTOR");// No Localization Needed
#endif
                }
            else
                {
                hRes = FindResource(hinst_SYMFMT, "BOOT_SECTOR", "BOOTSECTOR");// No Localization Needed
                }

            if(hRes)
                {
                hBoot = LoadResource(hinst_SYMFMT, hRes);

                if(hBoot)
                    {
                    lpBoot = (LPVOID) LockResource(hBoot);

#if SYM_WIN32
                    memcpy((LPSTR)lpBuffer, (LPSTR)lpBoot, BOOT_SECTOR_LENGTH);
#else
                    MEMMOVE ((LPSTR)lpBuffer, (LPSTR)lpBoot, BOOT_SECTOR_LENGTH);
#endif

                    UnlockResource(hBoot);
                    FreeResource(hBoot);
                    }
                else
                    {
                    FreeResource(hBoot);
                    wRet = DISK_PHYSICAL_ERROR;
                    goto locRet;
                    }

                }
            else
                {
                wRet = DISK_PHYSICAL_ERROR;
                goto locRet;
                }
            }
                                        /*------------------------------*/
                                        /* Put boot signature in buffer */
                                        /*------------------------------*/
        *((LPWORD) (lpBuffer + 510)) = (WORD) 0xAA55;
                                        /*------------------------------*/
                                        /* Set base drive number        */
                                        /*------------------------------*/
        baseDrive = 0;                      /* Only format floppies                */
                                        /*------------------------------*/
                                        /* Different DOSs use different */
                                        /* size BPBs                    */
                                        /*------------------------------*/
        if (dosVersion < DOS_3_20)
            {
            sizeToMove = 19;                /* Only move 19 BYTEs           */
            lpBuffer [0x1E] = baseDrive;
            lpBuffer [0x1F] = 0;            /* Only format floppies         */
            }
        else
            {
            sizeToMove = 21;                /* Move 21 BYTEs                */

                                        /* See if we can put more BPB   */
                                        /* info on disk by looking at   */
                                        /* the JMP xxx NOP to see what  */
                                        /* the offset is for the JMP    */
            if (*lpBuffer == 0xEB && lpBuffer [2] == 0x90)
                {
                if (lpBuffer [1] >= (BYTE) 0x22)
                    sizeToMove += 4;        /* Add the BigFoot sectors      */
                }
            lpBuffer [509] = baseDrive;
                                        /*------------------------------*/
                                        /* If DOS 4.0 boot record, fill */
                                        /* in serial number and other   */
                                        /* fields.  Don't touch the     */
                                        /* Volume Label field.  DOS will*/
                                        /* use the one in the root by   */
                                        /* default.                     */
                                        /* '29h' is the extended boot   */
                                        /* record signature             */
                                        /*------------------------------*/
            if ((dosVersion >= DOS_4_00) &&
                (lpBuffer [0x26] == EXTENDED_BOOT_RECORD_SIGNATURE))
                {
                lpBuffer [0x24] = baseDrive;
                lpBuffer [0x25] = 0;        /* Reserved                     */

                                        /* Get Serial Number            */
                *((DWORD FAR *) (lpBuffer + 0x27)) = DiskGetSerialNumber();

                                        /* Set to default Volume Label  */
                                        /* We do memory move, we don't  */
                                        /* copy EOS                     */
                DiskGetDefaultVolume(szTemp);
                MEMMOVE ((const LPBYTE)(lpBuffer + 0x2B), szTemp, lstrlen(szTemp));

                                        /* Set File System Type         */
                                        /* We do memory move, we don't  */
                                        /* copy EOS                     */
                DiskGetFATStr((BYTE)(IsFAT16(ExternDiskRec) ? FAT_16_TYPE :
                                FAT_12_TYPE), szTemp);
                MEMMOVE((const LPBYTE)(lpBuffer + 0x36), szTemp, lstrlen(szTemp));
                }
            }
        }

                                        /*------------------------------*/
                                        /* Move BPB to boot record      */
                                        /*------------------------------*/
    MEMMOVE(lpBuffer + 11, &ExternBPB, sizeToMove);
                                        /*------------------------------*/
                                        /* Write out boot record        */
                                        /*------------------------------*/
    ExternDiskRec.sector = 0;
    ExternDiskRec.sectorCount = 1;
    ExternDiskRec.buffer = lpBuffer;
    while (TRUE)
        {
        wErr = DiskBlockDevice(BLOCK_WRITE, &ExternDiskRec);

        if (!wErr)
            break;

        if (wErr == TIME_OUT)
            {
            NMsgGet(hinst_SYMFMT,
                    FORMAT_INSERT_ORIGINAL_DISK,
                    zScratch,
                    MAX_SCRATCH,
                    (LPSTR) zDriveName);

            NStringLoad(FORMAT_CAPTION, zCaption, MAX_CAPTION);
            wResult = MessageDisplay(
                            hFillDialog,
                            zScratch,
                            zCaption,
                            MB_OKCANCEL | MB_ICONINFORMATION,
                            NULL,
                            NULL);

            if(wResult == IDOK)
                continue;
            }
        else if (wErr == WRITE_PROTECTED)
            {
            wResult = NMessageDisplay(hinst_SYMFMT,
                            hFillDialog,
                            FORMAT_REMOVE_WRITE_PROTECT,
                            FORMAT_CAPTION,
                            MB_OKCANCEL | MB_ICONINFORMATION,
                            NULL,
                            NULL);

            if(wResult == IDOK)
                continue;
            }
        break;
        }

    wRet = (wErr == 0) ? NOERR : DISK_PHYSICAL_ERROR;

locRet:
#ifndef	SYM_WIN32
    if (HWIsNEC())
        DiskBlockClearProc(ExternDiskRec.dl, DiskBlockDeviceProcNEC);
#endif

                                    /*------------------------------*/
                                    /* Free the buffer              */
                                    /*------------------------------*/
    if (lpBuffer)
        MemFreePtr(lpBuffer);
                                    /*------------------------------*/
                                    /* Return error state           */
                                    /*------------------------------*/
    return (wRet);
}




/*----------------------------------------------------------------------*/
/* WriteFats                                                            */
/*        Writes the new FATs to the disk.                              */
/*                                                                      */
/*        Returns:                                                      */
/*                NOERR                    No error                     */
/*                DISK_MEMORY_ERROR        Memory error                 */
/*                DISK_PHYSICAL_ERROR      Physical error               */
/*----------------------------------------------------------------------*/

WORD LOCAL WriteFats (VOID)
{
    auto   char         zCaption[MAX_CAPTION];
    auto   char         zScratch[MAX_SCRATCH];
    auto   WORD         wErr;
    extern SFSysInfoRec ExternSysInfo;
    extern FAT	 	ExternFAT;
    extern SFStateRec   ExternState;
    extern DiskRec      ExternDiskRec;
    auto   WORD         wResult;
    auto  char          zDriveName[3];

    zDriveName[0] = ExternDiskRec.dl;
    zDriveName[1] = ':';
    zDriveName[2] = EOS;

                                        /*------------------------------*/
                                        /* Mark all sectors in FAT as   */
                                        /* dirty.                       */
                                        /*------------------------------*/
    FATMarkEntireFATDirty(&ExternFAT);
                                        /*------------------------------*/
                                        /* Write out all copies of the  */
                                        /* FAT.                         */
                                        /*------------------------------*/
#ifndef	SYM_WIN32
    if (HWIsNEC())
        DiskBlockSetProc(DiskBlockDeviceProcNEC);
#endif

    while(TRUE)
        {
        wResult = FATWrite(&ExternFAT);
        if (wResult == NOERR)
            break;
                                        /*------------------------------*/
                                        /* Get the last error we had    */
                                        /*------------------------------*/
        wErr = DiskGetBlockDeviceError();
        if (wErr == TIME_OUT)
            {
            NMsgGet(hinst_SYMFMT,
                FORMAT_INSERT_ORIGINAL_DISK,
                zScratch,
                MAX_SCRATCH,
                (LPSTR) zDriveName);

            NStringLoad(FORMAT_CAPTION, zCaption, MAX_CAPTION);

            wResult = MessageDisplay(
                hFillDialog,
                zScratch,
                zCaption,
                MB_OKCANCEL | MB_ICONINFORMATION,
                NULL,
                NULL);

            continue;
            }
        else if (wErr == WRITE_PROTECTED)
            {
            wResult = NMessageDisplay(hinst_SYMFMT,
                hFillDialog,
                FORMAT_REMOVE_WRITE_PROTECT,
                FORMAT_CAPTION,
                MB_OKCANCEL | MB_ICONINFORMATION,
                NULL,
                NULL);

            continue;
            }
        break;
        }

#ifndef	SYM_WIN32
    if (HWIsNEC())
        DiskBlockClearProc(ExternFAT.Disk.dl, DiskBlockDeviceProcNEC);
#endif

    return(wResult);
}




/*----------------------------------------------------------------------*/
/* GetExistingFAT                                                       */
/*    Gets the FAT currently on the disk.  We need to go through the    */
/*    existing FAT, setting all entries to UNUSED, except the BAD       */
/*    clusters.                                                         */
/*                                                                      */
/*    Returns:                                                          */
/*            NOERR                    If no error                      */
/*            DISK_MEMORY_ERROR        Memory error                     */
/*            DISK_PHYSICAL_ERROR      Physical error                   */
/*----------------------------------------------------------------------*/

WORD LOCAL GetExistingFAT (VOID)
{
    extern    DiskRec ExternDiskRec;
    extern    FAT     ExternFAT;
    auto      DWORD   dwCluster;
    auto      DWORD   dwMaxCluster;
    auto      WORD    wResult;



                                        /*------------------------------*/
                                        /* Read the FAT from the disk   */
                                        /*------------------------------*/
    wResult = FATRead(&ExternFAT, &ExternDiskRec, 0);
    if (wResult != NOERR)
        return(wResult);
                                        /*------------------------------*/
                                        /* Find all non-BAD clusters    */
                                        /* and set them to UNUSED.      */
                                        /*------------------------------*/
    dwMaxCluster = ExternDiskRec.dwMaxClusterEx;
    for (dwCluster = 2; dwCluster <= dwMaxCluster; dwCluster++)
    {
        if (FATGet(&ExternFAT, dwCluster) != BAD_CLUSTER)
        {
                                        /*------------------------------*/
                                        /* Non-Bad entry found, mark as */
                                        /*        free space            */
                                        /*------------------------------*/
            FATPut(&ExternFAT, dwCluster, UNUSED_CLUSTER);
        }
    }

    return(NOERR);
}




VOID LOCAL LogicalSectorToPhysicalSector (DWORD sector, LPDISKREC diskPtr, LPABSDISKREC absDiskPtr)
{
    DWORD           sectorOffset;
    DWORD           result;
    DWORD           modResult;
    DWORD           totalHeads;

                                             /* Find total sector offset */
    sectorOffset = diskPtr->sectorsBeforePartition + sector;
    result    = (sectorOffset / absDiskPtr->dwSectorsPerTrack);
    modResult = (WORD)(sectorOffset % absDiskPtr->dwSectorsPerTrack);
    totalHeads = absDiskPtr->dwEndingHead + 1;

                                             /* Compute sector offset    */
    absDiskPtr->dwSector = (BYTE)( modResult + 1 );
                                             /* Compute track offset     */
    absDiskPtr->dwTrack  = (WORD)( result / totalHeads );
                                             /* Compute head offset      */
    absDiskPtr->dwHead   = (BYTE)( result % totalHeads );
}


/*----------------------------------------------------------------------*/
/*NavFmt_CanSupportExtendedIO                                           */
/*                                                                      */
/*                                                                      */
/*RETURNS: TRUE if we can use the Nashville Extended Int21 calls for    */
/*              Disk IO.                                                */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/
/* From DBUCHES in SYMKRNL  12-15-95 01:36:53pm                         */
/*----------------------------------------------------------------------*/

BOOL WINAPI NavFmt_CanSupportExtendedIO()
{
#if defined(SYM_WIN32)

  #ifndef DOS_7_10
    #define DOS_7_10    0x070A
  #endif

    static      UINT    uDOSVer = 0;
                                        // Only make Int21 call once!
    if (0 == uDOSVer)
        {
        if(SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
            {
            uDOSVer = DOS_7_00;
            }
        else
            {
            auto        SYM_REGS        regs;

            MEMSET(&regs, 0, sizeof(SYM_REGS));

            regs.EAX = 0x00003000;
            VxDIntBasedCall( 0x21, &regs );

            uDOSVer = (((regs.EAX & 0xFF00) >> 8) |
                       ((regs.EAX & 0x00FF) << 8));
            }
        }

    return(uDOSVer >= DOS_7_10);
#else
    return (FALSE);
#endif
}

