/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/dsk_misc.c_v   1.27   21 Aug 1998 16:37:20   MBROWN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      DiskResetDOS                                                    *
 *      DiskComputeDriveSize                                            *
 *      DiskGetMaxDrive                                                 *
 *      DiskReset                                                       *
 *      DiskGetSerialNumber                                             *
 *      DiskIOCTL                                                       *
 *      DiskIOCTLError                                                  *
 *      DiskGetAccessFlag                                               *
 *      DiskSetAccessFlag                                               *
 *      DiskGetLockedTable                                              *
 *      DiskSetLockedTable                                              *
 *      DiskSetFormatType                                               *
 *      DiskGetDASDType                                                 *
 *      DiskSetDASDType                                                 *
 *      VolumeLabelGet                                                  *
 *      VolumeLabelSet                                                  *
 *      GetVolumeInformation (NT Kernel mode/Native API)                *
 *      DiskGetFileSCN                                                  *                *
 *      DiskCheckLBAProblem						*
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/dsk_misc.c_v  $ *
// 
//    Rev 1.27   21 Aug 1998 16:37:20   MBROWN
// Ported bug fixes from L branch
// 
//    Rev 1.26   17 Apr 1998 16:52:18   MBROWN
// More large drive support fixes
// 
//    Rev 1.25   16 Mar 1998 17:34:20   MBROWN
// Removed check for last track < 1024 from LBA check
//
//    Rev 1.24   16 Mar 1998 16:47:00   MBROWN
// Removed 8GB+ drive filter from LBA check
//
//    Rev 1.23   11 Mar 1998 15:05:36   SKURTZ
// Numerous changes for Extended Int13 support
//
//    Rev 1.22   06 Nov 1997 18:06:42   MBROWN
// Modified filtering of 8GB drives in LBA check
//
//    Rev 1.21   14 Oct 1997 18:10:10   MBROWN
// Make sure we skip the drive wrap test for drives > 8GB, since we get false
// errors on these drives due to Windows screwed up partitioning scheme.
//
//    Rev 1.20   10 Aug 1997 15:21:52   bsobel
// Changed to dynamically load thunk dll on win32 only when accessed for the first time.
//
//    Rev 1.19   19 Mar 1997 21:44:18   BILL
// Modified files to build properly under Alpha platform
//
//    Rev 1.18   06 Dec 1996 03:22:48   FCHAPA
// Fixed _DiskCheckLBAProblem() for NEC machines.
//
//    Rev 1.17   04 Dec 1996 05:42:40   FCHAPA
// Fixed _DiskCheckForLBAProblem()
//
//    Rev 1.16   03 Dec 1996 12:24:52   BMCCORK
// Corrected comments
//
//    Rev 1.15   03 Dec 1996 12:09:54   BMCCORK
// Fixed DiskGetBootDrive() for NT 3.51 and NT 4
//
//    Rev 1.14   22 Nov 1996 15:02:30   MARKK
// Removed call to DirSetSCN
//
//    Rev 1.13   12 Nov 1996 16:41:40   SPASNIK
// For 32bit platform try the registry key
// first then fall back to the thunk mechanism
//
//    Rev 1.12   11 Sep 1996 16:09:24   DHERTEL
// Compute sectorsBeforePartition in _DiskCheckLBAProblem.
//
//    Rev 1.11   26 Aug 1996 10:57:56   MZAREMB
// Ifdef'd the DiskGetAccessFlag() and DiskSetAccessFlag() for DOSX only.
//
//    Rev 1.10   23 Aug 1996 11:29:40   MZAREMB
// Added a buffer to hold the disk access byte flags for DiskGetAccessFlag() and
// DiskSetAccessFlag() to correct the situation where the floppy disk grinds
// excessively because of multiple calls to these routines.
//
//    Rev 1.9   22 Aug 1996 13:07:30   DHERTEL
// Merged changes from E branch.
//
//    Rev 1.8   22 Jul 1996 18:13:34   RSTANEV
// SYM_NTK: Using the NT_SUCCESS() macro.
//
//    Rev 1.7   24 Jun 1996 19:03:16   RSTANEV
// Now using the Zv...() wraps instead of the Zw...() APIs.
//
//    Rev 1.6   16 Apr 1996 17:15:30   RSTANEV
// DiskGetBootDriveEx() should not compile for SYM_NTK and SYM_VXD.
//
//    Rev 1.5   16 Apr 1996 15:29:00   PMARTIN
// Added DiskGetBootDriveEx(), which is a cover for DiskGetBootDrive()
//
//    Rev 1.4   03 Apr 1996 10:33:52   SKURTZ
// Added DiskGetFileSCN()
//
//    Rev 1.3   28 Feb 1996 12:39:00   DBUCHES
// Fixed DiskGet/SetAccessFlag() for DOS 7.10.
//
//    Rev 1.2   09 Feb 1996 18:42:26   RSTANEV
// Restored back to use Zw... functions.
//
//    Rev 1.1   02 Feb 1996 12:48:52   RSTANEV
// Cleaned up for SYM_NTK platform.
//
//    Rev 1.0   26 Jan 1996 20:23:18   JREARDON
// Initial revision.
//
//    Rev 1.90   23 Jan 1996 16:30:48   RANSCHU
// Oops, typed in the wrong variable name & neglected to verify that it compiled.
//
//    Rev 1.89   23 Jan 1996 16:05:22   RANSCHU
// Initialize clEOF and clBAD fields in DISKREC with FAT specific values.
//
//    Rev 1.88   23 Jan 1996 15:43:28   RSTANEV
// Changed VolumeLabelGet() for NT Kernel platform and added parameter
// checks in GetVolumeInformation().
//
//    Rev 1.87   19 Jan 1996 14:07:06   RSTANEV
// Converted to Unicode, and added SYM_NTK platform.
//
//    Rev 1.86   10 Jan 1996 18:11:30   DBUCHES
// Set the IOType to extended if we are running on DOS7.1 or greater.
//
//    Rev 1.85   09 Jan 1996 13:53:38   DBUCHES
// Fixed ConvertSizeToCluster().  Needed to cast to DWORD.
//
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#define INCLUDE_VWIN32
#include "disk.h"
#include "cdisk.h"
#include "xapi.h"
#include "file.h"
#include "thunk.h"
#include <stackerw.h>
#include <dos.h>
#ifdef SYM_OS2
#   include <stdos2.h>
#endif
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

MODULE_NAME;

/*----------------------------------------------------------------------*
 * Returns TRUE if the volume supports LFN's and FALSE otherwise. This  *
 * function can be used to determine if NameWildcardMatch() should be   *
 * passed WILDCARDMATCH_SHORT or WILDCARDMATCH_LONG. Even though the    *
 * WILDCARDMATCH_QUERY would figure this out it imposes a tremendous    *
 * speed penalty. For products such as the scanners this isn't acceptable *
 *                                                                      *
 * NOTE: The lpVolumePath can point to a UNC path or any other valid    *
 * DOS like path such as C:\WORKING\MYFILE.EXE                          *
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI VolumeSupportsLFN (
    LPCTSTR lpVolumePath )
{
#if defined(SYM_OS2)
                                        //  NOTE: No UNC for OS/2
    HMODULE     ModuleHandle;
    HFILE filehandle;
    CHAR        szModuleName[3];
    APIRET      rc;

    PBYTE  pszFSDName;
    PBYTE  pFSData;
    ULONG  cbBuffer;
    BYTE         fsqBuffer[sizeof(FSQBUFFER2) + (3 * CCHMAXPATH)];
    PFSQBUFFER2  pfsqBuffer = (PFSQBUFFER2) fsqBuffer;

                                        //  Uses drive letter, with colon.
    strcpy(szModuleName, lpVolumePath);
    szModuleName[2]='\0';
    cbBuffer = sizeof(fsqBuffer);

    rc = DosQueryFSAttach(szModuleName, 0, FSAIL_QUERYNAME, pfsqBuffer, &cbBuffer);

    if (rc != 0)
        return ( FALSE );

    pszFSDName = pfsqBuffer->szName + pfsqBuffer->cbName + 1;
    if (!strcmp(pszFSDName, "HPFS")) return ( TRUE );
    return ( FALSE );
#else
    auto    BOOL    bSupportsLFN = FALSE;
    auto    DWORD   dwComponentLength = 0;
                                        // ----------------------------------
                                        // Application error.
                                        // ----------------------------------
    SYM_ASSERT ( lpVolumePath );

    bSupportsLFN = VolumeInformationGet (
                                        lpVolumePath,
                                        NULL,
                                        0,
                                        NULL,
                                        &dwComponentLength,
                                        NULL,
                                        NULL,
                                        0
                                        );

    bSupportsLFN = (bSupportsLFN && dwComponentLength > 12) ? TRUE :
                                                              FALSE;
    return ( bSupportsLFN );
#endif
}

/*----------------------------------------------------------------------*
 * Returns the size of drive in bytes given its (initialized) disk      *
 * record.                                                              *
 *----------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
DWORD SYM_EXPORT WINAPI DiskComputeDriveSize (LPCDISKREC lpDisk)
{
    auto        UINT    bytesPerCluster;

                                        /* Compute bytes per cluster    */
    bytesPerCluster = lpDisk->bytesPerSector * lpDisk->sectorsPerCluster;

                                        /* Return size of disk (bytes)  */
    return (((DWORD) bytesPerCluster * (lpDisk->dwMaxClusterEx - 1)) +
             (lpDisk->bytesPerSector * (DWORD) lpDisk->dwStartOfDataEx));
}
#endif // #ifndef OS2

/*----------------------------------------------------------------------*/
/* DiskGetMaxDrive                                                      */
/*      Returns the maximum drive letter we can access.                 */
/*----------------------------------------------------------------------*/
#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
BYTE SYM_EXPORT WINAPI DiskGetMaxDrive(VOID)
{
    return ((BYTE)('A'+ DiskSet(DiskGet())));
}
#endif // #ifndef OS2


/*----------------------------------------------------------------------*/
/* DiskGetSerialNumber                                                  */
/*      Gets a DOS 4.0 serial number that will be slapped on the        */
/*      boot record of a DOS 4.0+ formatted disk.  The                  */
/*      serial number is encoded and the method is the                  */
/*      same method DOS 4.0 uses.                                       */
/*                                                                      */
/*      NOTE: Converted to 'C' by BRAD on 3/20/94.                      */
/*----------------------------------------------------------------------*/
#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
DWORD SYM_EXPORT WINAPI DiskGetSerialNumber (VOID)
{
    UINT        uMonth, uDay, uYear;
    UINT        uHour, uMinute, uSecond, uHundredth;
    WORD        wHi, wLo;

    DOSGetDate(&uMonth, &uDay, &uYear);
    DOSGetTime(&uHour, &uMinute, &uSecond, &uHundredth);

    wHi = (WORD)(((uSecond << 8) + uHundredth) + ((uMonth << 8) + uDay));
    wLo = (WORD)(uYear + ((uHour << 8) + uMinute));

    return(MAKELONG(wLo, wHi));
}
#endif // #ifndef OS2


/*----------------------------------------------------------------------*/
/* CLNTOSEC - Converts a cluster number to a sector number.             */
/*----------------------------------------------------------------------*/
#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
DWORD SYM_EXPORT WINAPI ConvertClusterToSector (LPCDISKREC lpDisk, CLUSTER cln)
{
                                        /*------------------------------*/
                                        /* Check to see if legal        */
                                        /*------------------------------*/
    if ((cln < (UINT) 2) || (cln > lpDisk->dwMaxClusterEx))
      return (0L);
                                        /*------------------------------*/
                                        /* Adjust so base 0             */
                                        /*------------------------------*/
    cln -= (UINT) 2;

    return ((DWORD) lpDisk->dwStartOfDataEx +
            ((DWORD) cln * (DWORD) lpDisk->sectorsPerCluster));
}
#endif // #ifndef OS2


/*----------------------------------------------------------------------*/
/*      Looks at a Disk Record to determine if all of its fields are    */
/*      in synch.                                                       */
/*----------------------------------------------------------------------*/
#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
BOOL SYM_EXPORT WINAPI DiskIsValidDiskRec (LPCDISKREC lpDisk)
{

    if(IsFAT32(*lpDisk))
        return((BOOL)
            ((lpDisk->numberFats) &&
             (lpDisk->numberFats < 5) &&
             (lpDisk->dwMaxClusterEx > 2) &&
             (lpDisk->bytesPerSector) &&
             !(lpDisk->bytesPerSector & 0x000F) &&
             MathIsPower2((UINT) lpDisk->sectorsPerCluster) &&
             (!(lpDisk->bytesPerSector % 64)) &&
             (!(lpDisk->maxRootEntries % 2)) &&
             (lpDisk->clRootSCN > 1) &&
             (lpDisk->startOfFat < lpDisk->dwStartOfDataEx)));
    else
        return ((BOOL)
            ((lpDisk->startOfFat < lpDisk->startOfRootDir) &&
             (lpDisk->startOfRootDir < lpDisk->dwStartOfDataEx) &&
             (lpDisk->numberFats) &&
             (lpDisk->numberFats < 5) &&
             (lpDisk->dwMaxClusterEx > 2) &&
             (lpDisk->dwSectorsPerFATEx) &&
             (lpDisk->maxRootEntries) &&
             (lpDisk->bytesPerSector) &&
             !(lpDisk->bytesPerSector & 0x000F) &&
             MathIsPower2((UINT) lpDisk->sectorsPerCluster) &&
             (!(lpDisk->bytesPerSector % 64)) &&
             (!(lpDisk->maxRootEntries % 2)) &&
             (!((lpDisk->maxRootEntries * (DWORD) sizeof(DIRENTRYREC)) %
                    lpDisk->bytesPerSector))));
}

#endif // #ifndef OS2


/*----------------------------------------------------------------------*
 *  This module will get a disk table that will be used when performing *
 *      absolute DOS sector reads.                                      *
 *                                                                      *
 *  Returns:                                                            *
 *      TRUE                    Got disk characteristics okay           *
 *      FALSE                   Can't get valid disk characteristics    *
 *                                                                      *
 *----------------------------------------------------------------------*
 * 01/09/90     Brad: Once again, I changed it back to ONLY use the     *
 *              undocumented DOS function, 32h.  Before, we were        *
 *              looking at the BPB on the Boot Record. But after much   *
 *              studying, DOS rarely even uses the BPB in the Boot      *
 *              Record.  If we decide to modify this stuff again, we    *
 *              should use the DOS 3.2+ function, (440Dh), which will   *
 *              return the BPB of the disk. From this, we can easily    *
 *              compute the diskRec (The old routine used in NU 4.5     *
 *              already has the conversion code).                       *
 *----------------------------------------------------------------------*/
#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
BOOL SYM_EXPORT WINAPI DiskGetInfo (BYTE dl, LPDISKREC lpDisk)
{
    static BYTE     byIsDiskGetInfoActive=FALSE;

    dl = (BYTE) CharToUpper(dl);

    DiskReset ();                       /* Reset the disk system          */

    lpDisk->dl = dl;            /* Save the drive letter          */

                                        /*------------------------------*/
                                        /* Get Driver info thru DOS     */
                                        /*------------------------------*/
    if (!DiskGetDriverTable (dl, lpDisk))
        return (FALSE);

                                        /* Is this a 16-bit Fat or 12-bit */
    if (lpDisk->dwMaxClusterEx < (UINT)0x0FF6)
        {
        lpDisk->clEOF = FAT12VALUEMASK&~(DWORD)7;
        lpDisk->byFATType = DR_FAT12;
        }
    else if (lpDisk->dwMaxClusterEx < 0xFFFF)
        {
        lpDisk->clEOF = FAT16VALUEMASK&~(DWORD)7 ;
        lpDisk->byFATType = DR_FAT16;
        }
    else
        {
        lpDisk->clEOF = FAT32VALUEMASK&~(DWORD)7;
        lpDisk->byFATType = DR_FAT32;
        }
    lpDisk->clBAD = lpDisk->clEOF-1;

                                        /* Calculate sectors in root    */
    lpDisk->sectorsInRoot = (WORD)lpDisk->dwStartOfDataEx -
                                                lpDisk->startOfRootDir;

                                        /* Calculate Bytes Per Cluster  */
    lpDisk->bytesPerCluster = lpDisk->bytesPerSector *
                                  lpDisk->sectorsPerCluster;

    lpDisk->totalSectors = lpDisk->dwStartOfDataEx +
           (lpDisk->dwMaxClusterEx - 1) * (DWORD) lpDisk->sectorsPerCluster;

                                        /*------------------------------*/
                                        /* We now need to set the       */
                                        /* bigFoot flag.  Base it on    */
                                        /* the total sectors in the     */
                                        /* partition.  If there are     */
                                        /* more than 64K, then use      */
                                        /* the BigFoot interface.       */
                                        /*------------------------------*/
    if(DOSGetVersion() < DOS_7_10)
        {
        if (lpDisk->totalSectors <= 0x0000FFFFL)
            lpDisk->byIOType = DR_IO_NORMAL;
        else
            lpDisk->byIOType = DR_IO_BIGFOOT;
        }
    else
        {
        lpDisk->byIOType = DR_IO_EXTENDED;
        }

    /*----------------08-12-93 02:33pm------------------
    ** Get Major, Minor and Mask for drive.
    ** ------------------------------------------------*/
    if(byIsDiskGetInfoActive == FALSE)
        {
        UINT    uMajorType, uMinorType;

        byIsDiskGetInfoActive=TRUE;
        DiskGetType(lpDisk->dl, &uMajorType, &uMinorType);
        lpDisk->wMajorType = uMajorType;
        lpDisk->wMinorType = uMinorType;
        lpDisk->wMask = DiskGetMask(lpDisk->wMajorType, lpDisk->wMinorType);

        byIsDiskGetInfoActive=FALSE;
        }

                                        /*------------------------------*/
                                        /* Make sure ALL values are     */
                                        /*      valid                   */
                                        /* Make this test as difficult  */
                                        /*  to pass as possible, since  */
                                        /*  invalid values will destroy */
                                        /*  a lpDisk->          */
                                        /*------------------------------*/
    return ((BOOL) DiskIsValidDiskRec(lpDisk));
}
#endif  //  #ifndef SYM_OS2 && SYM_VXD

/*----------------------------------------------------------------------*/
/* Converts a sector number to a cluster number.                        */
/*----------------------------------------------------------------------*/
#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
DWORD SYM_EXPORT WINAPI ConvertSectorToCluster (LPCDISKREC lpDisk, DWORD dwSector)
{
    auto        CLUSTER    clCluster;

                                        /* Check to see if legal        */
    if (dwSector < lpDisk->dwStartOfDataEx)
        return (0);

    clCluster = ((dwSector - lpDisk->dwStartOfDataEx) /
                                        (DWORD)lpDisk->sectorsPerCluster) + 2;
    return((clCluster > lpDisk->dwMaxClusterEx) ? 0 : clCluster);
}


/*----------------------------------------------------------------------*/
/*      Converts a size to a cluster count.                             */
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI ConvertSizeToCluster (LPCDISKREC lpDisk, DWORD size)
{
    register    DWORD    clusters;


    clusters = (size / (DWORD)lpDisk->bytesPerCluster);
    if (size % (DWORD)lpDisk->bytesPerCluster)
        clusters++;

    return (clusters);
}
#endif // #ifndef SYM_OS2 && SYM_VXD && SYM_NTK


#if !defined(SYM_OS2) && !defined(SYM_VXD)
BOOL SYM_EXPORT WINAPI VolumeInformationGet(
    LPCTSTR lpRootPathName,
    LPTSTR lpVolumeNameBuffer,
    DWORD dwVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPTSTR  lpFileSystemNameBuffer,
    DWORD dwFileSystemNameSize)
{
        auto    BOOL            bRet = FALSE;

#if defined(SYM_WIN32) || defined(SYM_NTK)

    auto    TCHAR   szRoot [ SYM_MAX_PATH ] = { 0 };

    NameReturnRoot ( lpRootPathName, szRoot );
    if (!NameIsRoot(szRoot))
        {
        if (!NameIsUNC(lpRootPathName))
            if (lpRootPathName[1] == ':')
                {
                szRoot[0] = lpRootPathName[0];
                szRoot[1] = lpRootPathName[1];
                szRoot[2] = PATH_CHAR;
                szRoot[3] = EOS;
                }
            else                        // If no drive, assume current disk
                {
#if defined(SYM_NTK)
                return ( FALSE );       // no concept of current disk
#else
                szRoot[0] = DiskGet();
                szRoot[1] = ':';
                szRoot[2] = PATH_CHAR;
                szRoot[3] = EOS;
#endif
                }
        }

    bRet = GetVolumeInformation (
                        szRoot,
                        lpVolumeNameBuffer,
                        dwVolumeNameSize,
                        lpVolumeSerialNumber,
                        lpMaximumComponentLength,
                        lpFileSystemFlags,
                        lpFileSystemNameBuffer,
                        dwFileSystemNameSize
                        );

#else

    auto    WORD            wMaxComponentLength;
    auto    WORD            wFileNameSize;
    auto    WORD            wFlags;



    wMaxComponentLength = 0;
    wFlags = 0;
    wFileNameSize = (WORD) dwFileSystemNameSize;

                                    // Test if the Long Filename
                                    // int 21H Functions are available
    _asm
        {
        push    ds
        push    es
        push    di
                                // es:di points to buffer to receive
                                // file system name
        les     di,lpFileSystemNameBuffer
                                // cx = length of that file system
                                // name buffer
        mov     cx,wFileNameSize

                                // Volume to check
        lds     dx,lpRootPathName

        mov     ax, 71A0h       // Get Volume Information function
        stc
        DOS
        jc      failure 	// skip ahead if error

        mov     bRet, 1         // success
                                // save maximum component length
        mov     wMaxComponentLength,cx
                                // save the file system flags
        mov     wFlags, bx

        jmp     short success   // It worked

failure:
        mov     bRet, 0         // failure
success:
        pop    di
        pop    es
        pop    ds
        }

    if (lpMaximumComponentLength)
        *lpMaximumComponentLength = wMaxComponentLength;
    if (lpFileSystemFlags)
        *lpFileSystemFlags = wFlags;
#endif

    return (bRet);
}
#endif          //  os2

/*----------------------------------------------------------------------*/
/* VolumeLabelGet                                                       */
/*    This function is used to get the volume label.                    */
/*                                                                      */
/* NOTES                                                                */
/*      Uses DiskGetInfo() where possible to get volume labels with     */
/*      spaces.  FindFirst calls do not support this (per Enrique)      */
/*                                                                      */
/* INPUTS                                                               */
/*    TCHAR     byDriveLetter    the drive letter                       */
/*    LPTSTR    lpName           buffer that contains the volume label  */
/*                               on exit.                               */
/*                                                                      */
/* RETURNS                                                              */
/*      (UINT)NOERR             No error.  Label in 'lpName'            */
/*      ERR             Error getting label.                            */
/*----------------------------------------------------------------------*/
/* \\?? ?? Created                                                  */
/* 10/29/93 BG Preserve DTA before calling FileFindFirst                */
/*----------------------------------------------------------------------*/
#if !defined(SYM_VXD)
UINT SYM_EXPORT WINAPI VolumeLabelGet(TCHAR byDriveLetter, LPTSTR lpName)
{
    byDriveLetter = (TCHAR) CharToUpper(byDriveLetter);


#if defined(SYM_WIN32)
    {
    auto    BOOL    bOk;
    auto    TCHAR   szDrive[10]                     = {0};
    auto    TCHAR   szVolume[SYM_MAX_PATH]          = {0};
    auto    TCHAR   szFileSystemName[SYM_MAX_PATH]  = {0};
    auto    DWORD   dwMaxComponentLength            = 0;
    auto    DWORD   dwFileSystemFlags               = 0;

    STRCPY(szDrive, _T("x:\\"));
    szDrive[0] = byDriveLetter;

    bOk = GetVolumeInformation(
                    szDrive,
                    szVolume,
                    sizeof(szVolume)/sizeof(*szVolume),
                    NULL,
                    &dwMaxComponentLength,
                    &dwFileSystemFlags,
                    szFileSystemName,
                    sizeof ( szFileSystemName )/sizeof(*szFileSystemName)
                    );

                                        // -----------------------------------
                                        // Only copy the volume name if the
                                        // function returned success. If the
                                        // function failed it returns garbage
                                        // so set an EOF in the target buffer.
                                        // -----------------------------------
    if ( bOk == TRUE )
        STRCPY(lpName, szVolume);
    else
        {
#if defined(SYM_DEBUG)
        DWORD dwCheckMe = GetLastError();
#endif
        *lpName = EOS;
        }

    return ((bOk) ? NOERR : ERR);
    }
#elif defined(SYM_NTK)
    {
    auto    BOOL    bOk;
    auto    TCHAR   szDrive[4];

    szDrive[0] = byDriveLetter;
    szDrive[1] = ':';
    szDrive[2] = '\\';
    szDrive[3] = '\0';

    bOk = GetVolumeInformation(
                    szDrive,
                    lpName,
                    SYM_MAX_PATH,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    0
                    );

    if ( !bOk )
        {
        *lpName = EOS;
        }

    return ((bOk) ? NOERR : ERR);
    }
#elif defined(SYM_OS2)
    {
    UINT  wRetVal;
    FSINFO fsinfo;


    *lpName = EOS;
    *(lpName+11) = EOS;

    wRetVal = DosQFSInfo(byDriveLetter - '@', FSIL_VOLSER,
                         &fsinfo, sizeof(FSINFO));

    if(!wRetVal)
        {
        STRNCPY(lpName, fsinfo.vol.szVolLabel, 11);
        return (NOERR);
        }
    else
        {
        return (ERR);
        }
    }
#else
    {
    static      char            szVol[] = "x:\\*.*";
    auto        UINT            wRetVal;
    auto        UINT            i, j;
    auto        UINT            nDirEntriesPerSector;
    auto        BOOL            done;
    auto        DISKREC         dr;
    auto        UINT            wErr;
    auto        HANDLE          hSectorBuffer;
    auto        LPDIRENTRYREC   lpSectorBuffer;
    auto        FINDDATA        findData;
    auto        HFIND           hFind;


    wRetVal = (UINT)ERR;
    *lpName = EOS;
                                        /* Check if the we are on a network, */
                                        /* assigned, substituted, or OS/2    */
                                        /* HPFS drive.                       */
                                        /* 10/15/93 Barry:                   */
                                        /* NOTE: Should be using DiskIsHPFS()*/
                                        /*       but it always returns FALSE */
                                        /*       since I don't know how to   */
                                        /*       detect them for real yet.   */
    if (!DiskIsBlockDevice(byDriveLetter) || DOSGetVersion() >= DOS_OS2_2_00)
        {
 hack:
        lpName[0] = EOS;
        szVol[0] = byDriveLetter;
                                        // Use low-level function, which
                                        // takes attributes
        hFind = FileFindFirstAttr(szVol, FA_VOLUME, &findData);

        if (hFind != INVALID_HANDLE_VALUE)
            {
                                        // If the volume label is greater than
                                        // 8 character a dot is inserted at
                                        // the ninth position.
                                        // Remove the dot if present
            if (STRLEN(findData.szFileName) > 8 && findData.szFileName[8] == '.')
                STRCPY(findData.szFileName + 8, findData.szFileName + 9);

            STRCPY(lpName, findData.szFileName);
            wRetVal = (UINT)NOERR;

            FileFindClose(hFind);
            }
        else
            {
            wErr = DOSGetExtendedError();
                                        /* If not really an error, but  */
                                        /* just no label.               */
            if (wErr == NO_MORE_FILES)
                wRetVal = (UINT)NOERR;
            }


        }
    else if (DiskGetInfo(byDriveLetter, &dr))
        {
        if (IsFAT32(dr))		// FAT32 will not support the code
            goto hack;                  // below

        lpName[0] = EOS;

        hSectorBuffer = MemAlloc(GMEM_MOVEABLE, dr.bytesPerSector * dr.sectorsPerCluster );
        if (!hSectorBuffer)
            return (wRetVal);

        if (lpSectorBuffer = (LPDIRENTRYREC)MemLock(hSectorBuffer))
            {
            nDirEntriesPerSector = dr.bytesPerSector / sizeof(DIRENTRYREC);

            dr.buffer = (HPBYTE) lpSectorBuffer;
            dr.sectorCount = 1;

            done = FALSE;
            wRetVal = (UINT)NOERR;
            for (dr.sector = (DWORD) dr.startOfRootDir, i = 0;
                    !done && i < dr.sectorsInRoot ; dr.sector++, i++)
                {
                                        /*------------------------------*/
                                        /* If physical error, don't     */
                                        /* continue and just return that*/
                                        /* we can't find label.         */
                                        /*------------------------------*/
                if (DiskBlockDevice (BLOCK_READ, &dr))
                    {
                    wRetVal = (UINT)ERR;
                    break;
                    }

                for (j = 0; !done && j < nDirEntriesPerSector; j++)
                    {
                    if (lpSectorBuffer[j].d_name [0] == DIR_EOD)
                        {
                        done = TRUE;
                        }
                    else if ((lpSectorBuffer[j].d_name [0] != DIR_ERASED) &&
                              (lpSectorBuffer[j].d_attr & FA_VOLUME) &&
                               !IsLongNameDirEntry(&lpSectorBuffer[j]))
                        {
                        MEMCPY(lpName, lpSectorBuffer[j].d_name, VOLUME_LENGTH);

                        lpName[VOLUME_LENGTH] = 0;
                        done = TRUE;
                        }
                    }
                }

            MemUnlock(hSectorBuffer, lpSectorBuffer);
            }

        MemFree(hSectorBuffer);
        }

    NOemToAnsi(lpName,lpName);      // convert to Ansi
    return (wRetVal);
    }
#endif
}
#endif


#if defined(SYM_WIN16) || defined(SYM_DOS)

/*----------------------------------------------------------------------*/
/* VolumePrepareEntry                                                   */
/*    This function was ported from DOS NCD.                            */
/*                                                                      */
/* INPUTS                                                               */
/*    LPDIRENTRYREC lpdRec    the directory entry record.            */
/*    LPSTR  lpLabel             the volume label.                      */
/*                                                                      */
/* RETURNS                                                              */
/*    VOID                                                              */
/*----------------------------------------------------------------------*/
VOID LOCAL PASCAL VolumePrepareEntry(LPDIRENTRYREC lpdRec, LPSTR lpLabel)
{
    auto        UINT            date, time;
    auto        LPSTR           lpTemp;
    auto        int             i;


    DateGetCurrentDateTime(&date, &time);

    lpdRec->d_date = date;
    lpdRec->d_time = time;
    lpdRec->d_attr = FA_VOLUME;
    lpdRec->d_size = 0L;
    lpdRec->d_scnLo = lpdRec->d_scnHi = 0;

    for(lpTemp = (LPSTR) lpdRec->d_name, i=0; *lpLabel; lpTemp++, lpLabel++, i++)
        *lpTemp = *lpLabel;
                                        /* Pad remainder of label with  */
                                        /* spaces.                      */
    for (; i < VOLUME_LENGTH; lpTemp++, i++)
        *lpTemp = ' ';
}

#endif


/*----------------------------------------------------------------------*/
/* VolumeLabelSet                                                       */
/*     This function is used to set a volume label.                     */
/*                                                                      */
/* INPUTS                                                               */
/*    BYTE      byDriveLetter           the drive to change the label   */
/*    BOOL      bRemoveOldLabel         Should we remove old label?     */
/*    LPSTR     lpLabel                 the volume label                */
/*                                                                      */
/* RETURNS                                                              */
/*      ERR                     Error setting                           */
/*      (UINT)NOERR                     No error                                */
/*----------------------------------------------------------------------*/
#if !defined(SYM_VXD) && !defined(SYM_NTK)
UINT SYM_EXPORT WINAPI VolumeLabelSet(
    BYTE byDriveLetter,
    BOOL bRemoveOldLabel,
    LPCSTR lpLabel)
{
#if defined(SYM_OS2)
    return (FALSE);
#elif defined(SYM_WIN32)
    char        szDrive[10];

    STRCPY(szDrive, "x:\\");
    szDrive[0] = (char) byDriveLetter;
    return ( SetVolumeLabel(szDrive, lpLabel) ? NOERR : ERR);

#else
    auto        UINT            wRetVal = (UINT) ERR;
    auto        UINT            i, j;
    auto        UINT            nDirEntriesPerSector;
    auto        BOOL            done;
    auto        DISKREC         dr;
    auto        HANDLE          hSectorBuffer;
    auto        LPDIRENTRYREC   lpSectorBuffer;
    auto        char            szOemLabel[SYM_MAX_PATH];



    NAnsiToOem(lpLabel,szOemLabel);      // convert to OEM
                                        /*------------------------------*/
                                        /* Can't set label on network,  */
                                        /* assigned, substed, or OS/2   */
                                        /* HPFS drive.                  */
                                        /*------------------------------*/
                                        /* 10/15/93 Barry:                   */
                                        /* NOTE: Should be using DiskIsHPFS()*/
                                        /*       but it always returns FALSE */
                                        /*       since I don't know how to   */
                                        /*       detect them for real yet.   */
    if (!DiskIsBlockDevice(byDriveLetter) || DOSGetVersion() >= DOS_OS2_2_00 )
        return(wRetVal);

    if (DiskGetInfo(byDriveLetter, &dr))
        {
        hSectorBuffer = MemAlloc(GMEM_MOVEABLE, dr.bytesPerSector * dr.sectorsPerCluster );
        if (hSectorBuffer == NULL)
            return (wRetVal);

        if (lpSectorBuffer = (LPDIRENTRYREC)MemLock(hSectorBuffer))
            {
            nDirEntriesPerSector = dr.bytesPerSector / sizeof(DIRENTRYREC);

            dr.buffer = (HPBYTE) lpSectorBuffer;
            dr.sectorCount = 1;

                                        /*------------------------------*/
                                        /* First remove old label       */
                                        /*------------------------------*/
            if (bRemoveOldLabel)
                {
                done = FALSE;
                for (dr.sector = (DWORD)dr.startOfRootDir, i = 0;
                    !done && (UINT)i < dr.sectorsInRoot ; dr.sector++, i++)
                    {
                    if (DiskBlockDevice (BLOCK_READ, &dr))
                        break;

                    for (j=0; !done && j<nDirEntriesPerSector; j++)
                        {
                        if (lpSectorBuffer[j].d_name [0] == DIR_EOD)
                            {
                            done = TRUE;
                            }
                        else if ((lpSectorBuffer[j].d_name [0] != DIR_ERASED) &&
                                 (lpSectorBuffer[j].d_attr & FA_VOLUME) &&
                                  !IsLongNameDirEntry(&lpSectorBuffer[j]))
                            {
                            lpSectorBuffer[j].d_name[0] = DIR_ERASED;
                            DiskBlockDevice (BLOCK_WRITE, &dr);
                            done = TRUE;
                            }
                        }
                    }
                }
                                        /*------------------------------*/
                                        /* Now add new volume label     */
                                        /*------------------------------*/
            done = FALSE;
            if (szOemLabel[0])
                {
                for (dr.sector = (DWORD)dr.startOfRootDir, i = 0;
                        !done && i < dr.sectorsInRoot ; dr.sector++, i++)
                    {
                    if (DiskBlockDevice (BLOCK_READ, &dr))
                        break;

                    for (j=0; !done && j < nDirEntriesPerSector; j++)
                        {
                        if (lpSectorBuffer[j].d_name [0] == DIR_ERASED ||
                             lpSectorBuffer[j].d_name [0] == DIR_EOD)
                            {
                            VolumePrepareEntry(&lpSectorBuffer[j],szOemLabel);
                            if (!DiskBlockDevice (BLOCK_WRITE, &dr))
                                wRetVal = (UINT)NOERR;
                            done = TRUE;
                            }
                        }
                    }
                }
            else
                wRetVal = (UINT)NOERR;

            MemUnlock(hSectorBuffer, lpSectorBuffer);
            }

        MemFree(hSectorBuffer);
        }

    return (wRetVal);
#endif
}
#endif  // !defined(SYM_VXD) && !defined(SYM_NTK)



/************************************************************************
 *                                                                      *
 * Description: Takes an unformatted list of drives (NULL TERMINATED)   *
 *              and formats it for output.                              *
 *                                                                      *
 *              e.g. 'ABCDEG\0'     ->     'A:-E: G:\0'                 *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/17/1991 ???? Function Created.                                    *
 * 07/19/1993 Martin, Ported to SYMKRNL                                 *
 ************************************************************************/
#if !defined(SYM_NTK)
VOID SYM_EXPORT WINAPI DiskGetDriveList (LPSTR lpUnformatted, LPSTR lpFormatList,
                                     UINT uFormatListSize)
{

    typedef struct
        {
        BYTE    byFirst;
        BYTE    byLast;
        } DRIVESDEF, FAR * LPDRIVESDEF;

    auto    LPDRIVESDEF     lpSelected;        // pointer to drive array
    auto    DRIVESDEF       rSelected[26];      // 26-drive array
    auto    BYTE            szDrives[6];        // scratch string
    auto    UINT            wIndex;             // counter
    auto    UINT            wLen;               // length of drive str

    /*
    **  Get one selected drive at a time.
    **  Store them in the array.
    **  If there are consecutive drives, the first drive name is stored in
    **    byFirst and the last in byLast; otherwise the drive name is stored
    **    in both.
    */

    MEMSET (rSelected, 0, sizeof (rSelected));

    lpSelected = rSelected;

    wLen = STRLEN(lpUnformatted);

    for (wIndex = 0; wIndex < wLen; wIndex++)
        {
        if ((BYTE)(lpSelected->byLast + 1) != lpUnformatted[wIndex])
            {
            if (lpSelected->byFirst != 0)
                lpSelected++;

            lpSelected->byFirst = lpUnformatted[wIndex];
            }

        lpSelected->byLast = lpUnformatted[wIndex];
        }

    /*
    **  Append the name to the area string.
    **  If it is a single drive, express in the form of X:.
    **  If it is a drive range, express in the form of X:-X:.
    */
    szDrives [1] = ':';
    szDrives [4] = ':';
    szDrives [5] = EOS;
    lpFormatList [0] = EOS;

    for (lpSelected = rSelected; lpSelected->byFirst; lpSelected++)
        {
        szDrives [0] = lpSelected->byFirst;
        szDrives [3] = lpSelected->byLast;
        szDrives [2] = (BYTE) (szDrives [0] == szDrives [3] ? EOS : '-');

        /*
        **  Check if the length limit will be exceeded.
        */
        wLen = STRLEN (lpFormatList) + STRLEN (szDrives);

        if (wLen > uFormatListSize)
            break;

        /*
        **  Append the new name at the end with a trailing space if possible.
        */
        STRCAT (lpFormatList, szDrives);
        lpFormatList += wLen;

        if (wLen < uFormatListSize)
            {
            *lpFormatList++ = ' ';
            *lpFormatList = EOS;
            }
        }
}
#endif // !defined(SYM_NTK)

#if !defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
/*----------------------------------------------------------------------*/
/* _DiskGetBootDrive()                                                  */
/*      Returns the boot drive letter, using DOS 4.0+ service, else     */
/*      returns 0                                                       */
/*----------------------------------------------------------------------*/
BYTE SYM_EXPORT WINAPI _DiskGetBootDrive(VOID)
{
#if defined(SYM_OS2) || defined(SYM_MIRRORS)

    ULONG                       ulQSV;

    DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE,
                   (PVOID)&ulQSV, sizeof(ulQSV));

    return (BYTE) ulQSV+'@';
#else   //  not OS2
    auto        BYTE            byBootDrive;

    if (DOSGetVersion() >= DOS_4_00)
        {
        _asm
        {
        mov     ax,3305h                ; Get boot drive
        DOS
        add     dl,'A' - 1              ; Adjust drive number to letter
        mov     byBootDrive, dl         ; Save return value
        }
        }
    else
        {
        char szEnvValue[SYM_MAX_PATH];

                                        // assume COMSPEC points to boot drive
        DOSGetEnvironment("COMSPEC", szEnvValue, sizeof(szEnvValue));
        if (szEnvValue[0])
            {
            byBootDrive = CharToUpper(szEnvValue[0]);
            }
        else
            {
            byBootDrive = DiskGetFirstFixedDrive();
            }
        }

    return(byBootDrive);
#endif
}
#endif


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Determines the boot drive, or the drive that contains the       *
 *      startup files (AUTOEXEC.BAT, CONFIG.SYS).  On systems with      *
 *      compressed drives, the system could actually boot on C:, but    *
 *      then switch the boot drive to D:.  This routine will detect     *
 *      the real drive.                                                 *
 *                                                                      *
 *      NOTE: This code was stolen from NDIAGS in NU 7.0.               *
 *                                                                      *
 *      NOTE: THIS FUNCTION SHOULD BE CONVERTED TO RETURN A TCHAR       *
 *                                                                      *
 *      Under NT 4 and Win95, the boot drive is retrieved from          *
 *      SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup\BootDir     *
 *      Under NT 3.5x, the boot drive is taken from the first letter    *
 *      of the SystemRoot environment variable.                         *
 *      If these techniques fail, then thunking is tried (maybe)        *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the boot drive letter, or 0 if an error occurred.       *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 09/15/1993 ED Function Created.                                      *
 ************************************************************************/
#if !defined(SYM_VXD) && !defined(SYM_NTK)
BYTE SYM_EXPORT WINAPI DiskGetBootDrive(VOID)
{
#if defined(SYM_WIN32)
   HKEY          RegKey;
   LONG          lRet           = 0;
   DWORD         dwSize         = 0;
   DWORD         dwValueType    = 0;
   unsigned long nDirNameLength = 0;
   TCHAR         szDir[SYM_MAX_PATH];
                                        // In NT 4 and Win95, the registry will work.
                                        // In NT 3.5x, the SystemRoot environment variable
                                        // will be used.
                                        // If all else fails, we'll try thunks (but I doubt
                                        // that code path ever gets taken).
   memset( szDir, 0x00, sizeof( szDir ));
   nDirNameLength = sizeof( szDir );

                                        // For NT 4....
   lRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                        _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"),
                        0,
                        KEY_READ,
                        &RegKey);

   if (lRet == ERROR_SUCCESS)
       {
       lRet = RegQueryValueEx (RegKey,
                               "BootDir",
                               NULL,
                               &dwValueType,
                               (LPBYTE)szDir,
                               &nDirNameLength);

       RegCloseKey( RegKey );

       SYM_ASSERT(lRet == ERROR_SUCCESS);
                                        //&? BEM - Until we convert this function to return a TCHAR, we'll hack it to
                                        //&? return a byte.  This WILL break under Unicode!!
       return(((LPCSTR)szDir)[0] );
       }
   else if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)

       {                                // For NT 3.5x, use the SystemRoot environment varialbe
       TCHAR szSystemRoot[SYM_MAX_PATH];
       DWORD dwSize;

       ZeroMemory(szSystemRoot, sizeof(szSystemRoot));
       dwSize = GetEnvironmentVariable("SystemRoot", szSystemRoot, sizeof(szSystemRoot));
       SYM_ASSERT(dwSize);
                                        //&? BEM - Until we convert this function to return a TCHAR, we'll hack it to
                                        //&? return a byte.  This WILL break under Unicode!!
       return ((LPCSTR)szSystemRoot)[0];
       }
#ifndef _M_ALPHA
   else
       {                                // UNLIKELY CODE PATH...
       	static FARPROC	   lpfnDiskGetBootDrive = NULL;
       	extern HINSTANCE   hInstThunkDll;

       	if (lpfnDiskGetBootDrive == NULL)
			{
			if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
			    {
		        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
		        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
		        }
		    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
	            {
	            lpfnDiskGetBootDrive = GetProcAddress(hInstThunkDll,"DiskGetBootDrive32" );
	     		if (lpfnDiskGetBootDrive == NULL)
	    	    	return FALSE;
               	}
	        else
	            return FALSE;
	        }

        if (lpfnDiskGetBootDrive == NULL)
            return FALSE;
	    else
	        return( lpfnDiskGetBootDrive() );
       }
#endif // ifndef _M_ALPHA
#else
    BYTE        byBootDrive;

    if (byBootDrive = _DiskGetBootDrive())
        {
#if !defined(SYM_NLM) && !defined(SYM_OS2) && !defined(SYM_MIRRORS)
        BYTE        byTempLetter;
        BYTE        bySequence;
        WORD        bSwapped;

        // Now check for Compressed Drives!!!

        if (DiskIsSuperStor(byBootDrive))
            {
            byBootDrive = DiskGetSuperStorSwapDrive(byBootDrive);
            }

        else if (DiskIsDoubleSpace(byBootDrive))
            {
            if (_DiskIsDoubleSpace(byBootDrive, &byTempLetter,
                                    &bSwapped, &bySequence))
                {
                byBootDrive = byTempLetter;
                }
            }

        else if (DiskIsStacker(byBootDrive))
            {
#ifndef SYM_DOSX
            struct stacker_drive_info_struct driveInfo;
            s_ushort    status;

            stacker_status(&status);

            driveInfo.driveLetter = byBootDrive;
            if (stacker_driveInfo(&driveInfo) == SE_SUCCESSFUL)
                {
            //  Upon return of stacker_driveinfo the real drive is stored
            //  in hostdriveletter!
                byBootDrive = driveInfo.hostDriveLetter;
                }
#else
            auto    PERUNIT   UnitData;

            if( StackerGetUnitData(byBootDrive, &UnitData) )
                byBootDrive = UnitData.lg_filedrive;
            else
                return(byBootDrive);
#endif
            }
#endif          // SYM_NLM
        }

    return (byBootDrive);
#endif
}
/***************************************************************************
 *                                                                         *
 *  DiskGetBootDriveEx()                                                   *
 *                                                                         *
 *  Determines the boot drive letter. Used as a cover when running under   *
 *  Windows NT since the thunks don't work under NT.                       *
 *                                                                         *
 *  Returns:                                                               *
 *      THUNK_NO_ERROR              success                                                    *
 *      THUNK_ERROR_INVALID_FUNC    function not available                 *
 *                                                                         *
 ***************************************************************************/
DWORD SYM_EXPORT WINAPI DiskGetBootDriveEx(LPBYTE lpbyDrive)
    {
    DWORD   dwRetval = THUNK_NO_ERROR;
    BYTE    byTempDrive;

    byTempDrive = DiskGetBootDrive();

    if ( byTempDrive )
        *lpbyDrive = byTempDrive;
    else
        dwRetval = THUNK_ERROR_INVALID_FUNC;

    return (dwRetval);
    }
#endif // #if !defined(SYM_VXD) && !defined(SYM_NTK)


#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)

/*----------------------------------------------------------------------*
 *  This module will flush the entire buffer system for a disk drive.   *
 *      It will flush the dirty buffers using the documented RESET_DISK *
 *      Also it will flush the disk so it thinks that a drive has never *
 *      been accessed, hence forcing it to reread the disk during next  *
 *      normal DOS access.                                              *
 *                                                                      *
 *  Returns:                                                            *
 *      NOTHING                                                         *
 *----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI DiskResetDOS (BYTE dl)       /* Letter of drive to reset     */
{
#ifdef _M_ALPHA
	return;
#elif defined(SYM_WIN32)
    auto	DIOC_REGISTERS      r;

    MEMSET(&r, 0, sizeof(DIOC_REGISTERS));

    r.reg_EAX = 0x710d;           // IOCTL
    r.reg_ECX = 1;
    r.reg_EDX = (BYTE)CharToUpper(dl) - '@';

    IOCtlVWin32( &r, &r, VWIN32_DIOC_DOS_IOCTL );
#else
#ifdef	SYM_WIN
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_31)
	return;
#endif

    DiskReset ();                       /* Flush dirty buffers          */

    dl = (BYTE) CharToUpper(dl);
    DiskResetAccessFlag (dl);           /* Now mark disk so DOS thinks  */
                                        /* it has never been accessed   */
#endif
}


/*----------------------------------------------------------------------*/
/* DiskReset                                                            */
/*      Resets the disk and diskette drive system for ALL drives.       */
/*      Writes ALL dirty buffers to disk, but retains copies in memory. */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
VOID SYM_EXPORT WINAPI DiskReset(VOID)
{
#if defined(SYM_WIN32)
    return;
#elif defined(SYM_DOSX)
                                                // Use DPMI for DiskReset
     auto    union REGS      regs;
     auto    struct SREGS    sregs;

     MEMSET(&regs, 0, sizeof(regs));
     MEMSET(&sregs, 0, sizeof(sregs));
     regs.h.ah = 0x0D;
     IntWin(0x21, &regs, &sregs);

#else

#ifdef	SYM_WIN
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_31)
	return;
#endif

    _asm
        {
        mov     ah, 0Dh                 ; Disk reset function
        DOS
        }
#endif
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*
 *      Builds an accessTable, where accessTable[0] is 'A', and so on.  *
 *      Also unlocks locked drives.                                     *
 *----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI DiskGetLockedTable(LPBYTE lockedTablePtr)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return;
#else
    static FARPROC	lpfnDiskGetLockedTable;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnDiskGetLockedTable == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
		    SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
		    hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
		    }
		if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnDiskGetLockedTable = GetProcAddress(hInstThunkDll,"DiskGetLockedTable32" );
		    if (lpfnDiskGetLockedTable == NULL)
				return;
		    }
		else
		    return;
	}

    lpfnDiskGetLockedTable(lockedTablePtr);
#endif // _M_ALPHA
#else
    auto        UINT            i;
    auto        LPBYTE          tablePtr;
    auto        BYTE            currentDrive;
    auto        BYTE            maxDl;
    auto        BYTE            dl;



                                        /* First clear all to FALSE     */
    tablePtr = lockedTablePtr;
    for (i = 0; i < SYM_MAX_DOS_DRIVES; i++)
        *lockedTablePtr++ = FALSE;

                                        /* Now see which ones are       */
                                        /* locked, if correct DOS       */
    if (DOSGetVersion() < DOS_4_00)
        return;

    currentDrive = DiskGet();
                                        /* Get all DOS logical drives   */
                                        /*      that are legal          */
    maxDl = (BYTE) (min(DiskSet (currentDrive), SYM_MAX_DOS_DRIVES) + 'A');

    for (dl = 'A'; dl <= maxDl; dl++)
        {
        if (DiskIsFixed(dl))
            {
            if (DiskGetAccessFlag(dl) == DISALLOW_ACCESS)
                {
                tablePtr[dl - 'A'] = TRUE;
                DiskSetAccessFlag(dl, ALLOW_ACCESS);
                }
            }
        }

   DiskSet (currentDrive);              /* Reset the drive letter       */
#endif
}



#if !defined(SYM_WIN32) && !defined (SYM_VXD)

/*----------------------------------------------------------------------*
 * DiskIOCTL()                                                          *
 *                                                                      *
 * Perform an IOCTL read or write on a character or block device.       *
 *                                                                      *
 * Returns number of bytes transferred or ERR (-1) if error.            *
 * Call DiskIOCTLError() to obtain actual error code.                   *
 *----------------------------------------------------------------------*/

static UINT uLastIoctlError;

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DiskIOCTL(BYTE byCommand, WORD wDriveOrHandle,
                             LPVOID lpBuffer, UINT uBufferSize)
{
    auto        UINT            uRetValue;

    // Convert drive letter to drive number if doing a block device command
    // (only do conversion if it looks like caller passed us a letter)
    if ((byCommand == IOCTL_BLOCK_READ || byCommand == IOCTL_BLOCK_WRITE) &&
        (wDriveOrHandle >= 'A'))
        {
        wDriveOrHandle = (CharToUpper((BYTE)wDriveOrHandle) - 'A') + 1;
        }

    _asm
        {
        push    ds
        mov     ah,44h
        mov     al,[byCommand]
        mov     bx,[wDriveOrHandle]
        mov     cx,[uBufferSize]
        lds     dx,[lpBuffer]
        DOS
        pop     ds
        jc      gotError
        mov     [uRetValue],ax
        mov     [uLastIoctlError],0
        jmp     short done
gotError:
        mov     [uRetValue],-1
        mov     [uLastIoctlError],ax
done:
        }

    return(uRetValue);
}
#pragma optimize("", on)

/*----------------------------------------------------------------------*
 * DiskIOCTLError()                                                     *
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI DiskIOCTLError(void)
{
    return(uLastIoctlError);
}


/*----------------------------------------------------------------------*/
/*      Contains routines for getting/setting the DOS 4.0 LOCK state.   */
/*                                                                      */
/*      Those wild and crazy guys at Microsoft forgot to support all    */
/*      of the minor codes for the Generic IOCTL call, so we have to    */
/*      resort to calling DPMI ourselves.                               */
/*                                                                      */
/*      This is the list they say is accurate for Win 3.0 and 3.1:      */
/*                                                                      */
/*      Supported:                      Not Supported:                  */
/*      ---------------                 ---------------------           */
/*      40 set params                   41 write track                  */
/*      42 format track                 47 set access flag (that us!)   */
/*      60 get params                   61 read track                   */
/*      62 verify track                 67 get access flag (thats us!)  */
/*      68 ???                                                          */
/*                                                                      */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* DiskGetAccessFlag                                                    */
/*      Returns the access type for drive 'dl'                          */
/*----------------------------------------------------------------------*/

#ifdef SYM_DOSX
static BYTE    byAccessFlags[32] = { 0 };
#endif

#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI DiskGetAccessFlag(BYTE driveLetter)
{
    auto        BYTE    returnValue = ALLOW_ACCESS;

    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_4_00)
        {
#ifdef  SYM_PROTMODE
        auto    union REGS      regs;
        auto    struct SREGS    sregs;
        auto    DWORD           dwDosBuffer;
        auto    UINT            wSeg;
        auto    LPBYTE          lpbyDosBuffer;

#ifdef SYM_DOSX
        if ( byAccessFlags[(driveLetter - 'A' + 1)] )
            return (byAccessFlags[(driveLetter - 'A' + 1)]);
#endif
                                        // Allocate a parameter block
        dwDosBuffer = GlobalDosAlloc(2);
        if (dwDosBuffer == NULL)
            return (returnValue);

                                        // Get paragraph-segment value
                                        // for use in DPMI call
        wSeg = HIWORD(dwDosBuffer);

                                        // Build pointer from selector
                                        // and initialize parameter block
        lpbyDosBuffer = MAKELP(LOWORD(dwDosBuffer), 0x0000);
        lpbyDosBuffer[0] = lpbyDosBuffer[1] = 0;

        if(CanSupportExtendedIO())
            {

                                            // Set up for DMPI call

            regs.x.ax = 0x440D;             // IOCTL, Generic block dev i/o control
            regs.h.bl =
            (BYTE) (driveLetter - 'A' + 1); // drive code (0=default, 1=A, etc)
            regs.h.ch = 0x48;               // major code = disk drive
            regs.h.cl = 0x67;               // minor code = GET access flag
            sregs.ds  = wSeg;               // segment of parameter block
            regs.x.dx = 0;                  // offset  of parameter block

            IntWin(0x21, &regs, &sregs);

            if (regs.x.cflag)
                goto tryOldMethod;

            }
        else
            {
tryOldMethod:
                                            // Set up for DMPI call

            regs.x.ax = 0x440D;             // IOCTL, Generic block dev i/o control
            regs.h.bl =
            (BYTE) (driveLetter - 'A' + 1); // drive code (0=default, 1=A, etc)
            regs.h.ch = 0x08;               // major code = disk drive
            regs.h.cl = 0x67;               // minor code = GET access flag
            sregs.ds  = wSeg;               // segment of parameter block
            regs.x.dx = 0;                  // offset  of parameter block

            IntWin(0x21, &regs, &sregs);
            }

                                        // Only override the ALLOW_ACCESS
                                        // return value if there's no error
        if (regs.x.cflag == 0)
            returnValue = lpbyDosBuffer[1];

        GlobalDosFree(LOWORD(dwDosBuffer));

#ifdef SYM_DOSX
        byAccessFlags[(driveLetter - 'A' + 1)] = returnValue;
#endif
#else
        auto    UINT    wValue = 0;


        if(CanSupportExtendedIO())
            {


        _asm    {
                mov     bl, driveLetter
                sub     bl, 'A'-1       // drive code (0=default, 1=A, etc)
                mov     ax, 440Dh       // IOCTL, Generic block dev i/o control
                mov     cx, 4867h       // major = drive, minor = GET flag
                lea     dx, wValue      // parameter block (assume DS == SS!)
                DOS
                                        // Only override the ALLOW_ACCESS
                                        // return value if there's no error
                jc      TryOldMethod
                }
            }
        else
            {
TryOldMethod:
            _asm    {
                    mov     bl, driveLetter
                    sub     bl, 'A'-1       // drive code (0=default, 1=A, etc)
                    mov     ax, 440Dh       // IOCTL, Generic block dev i/o control
                    mov     cx, 0867h       // major = drive, minor = GET flag
                    lea     dx, wValue      // parameter block (assume DS == SS!)
                    DOS
                                            // Only override the ALLOW_ACCESS
                                            // return value if there's no error
                    jc      Bad_Call
                    }

            }

        returnValue = HIBYTE( wValue );
Bad_Call:;

#endif
        }

    return (returnValue);
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*/
/* DiskSetAccessFlag                                                    */
/*      Sets the access type for drive 'dl'                             */
/*                                                                      */
/*      Returns:                                                        */
/*              TRUE    successful                                      */
/*              FALSE   unsuccessful                                    */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskSetAccessFlag(BYTE driveLetter, BYTE accessType)
{
    auto        BYTE            returnValue = FALSE;

    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_4_00)
        {
#ifdef  SYM_PROTMODE
        auto    union REGS      regs;
        auto    struct SREGS    sregs;
        auto    DWORD           dwDosBuffer;
        auto    UINT            wSeg;
        auto    LPBYTE          lpbyDosBuffer;

                                        // Allocate a parameter block
        dwDosBuffer = GlobalDosAlloc(2);
        if (dwDosBuffer == NULL)
            return (returnValue);

                                        // Get paragraph-segment value
                                        // for use in DPMI call
        wSeg = HIWORD(dwDosBuffer);

                                        // Build pointer from selector
                                        // and initialize parameter block
        lpbyDosBuffer = MAKELP(LOWORD(dwDosBuffer), 0x0000);
        lpbyDosBuffer[0] = 0;
        lpbyDosBuffer[1] = accessType;

#ifdef SYM_DOSX
        byAccessFlags[(driveLetter - 'A' + 1)] = accessType;
#endif
        if(CanSupportExtendedIO())
            {


                                            // Set up for DMPI call

            regs.x.ax = 0x440D;             // IOCTL, Generic block dev i/o control
            regs.h.bl =
            (BYTE) (driveLetter - 'A' + 1); // drive code (0=default, 1=A, etc)
            regs.h.ch = 0x48;               // major code = disk drive
            regs.h.cl = 0x47;               // minor code = SET access flag
            sregs.ds  = wSeg;               // segment of parameter block
            regs.x.dx = 0;                  // offset  of parameter block

            IntWin(0x21, &regs, &sregs);

            if (regs.x.cflag)
                goto tryOld;

            }
        else
            {
tryOld:
            regs.x.ax = 0x440D;             // IOCTL, Generic block dev i/o control
            regs.h.bl =
            (BYTE) (driveLetter - 'A' + 1); // drive code (0=default, 1=A, etc)
            regs.h.ch = 0x08;               // major code = disk drive
            regs.h.cl = 0x47;               // minor code = SET access flag
            sregs.ds  = wSeg;               // segment of parameter block
            regs.x.dx = 0;                  // offset  of parameter block

            IntWin(0x21, &regs, &sregs);
            }

                                        // Only override the FALSE
                                        // return value if there's no error
        if (regs.x.cflag == 0)
            returnValue = TRUE;

        GlobalDosFree(LOWORD(dwDosBuffer));
#else
        auto    UINT    wValue = (UINT) (accessType << 8);


        if(CanSupportExtendedIO())
            {
            _asm    {
                    mov     bl, driveLetter
                    sub     bl, 'A'-1       // drive code (0=default, 1=A, etc)
                    mov     ax, 440Dh       // IOCTL, Generic block dev i/o control
                    mov     cx, 4847h       // major = drive, minor = SET flag
                    lea     dx, wValue      // parameter block (assume DS == SS!)
                    DOS
                                            // Only override the FALSE
                                            // return value if there's no error
                    jc      TryOld
                    inc     returnValue     // set return to TRUE
                    }
            }

        else
            {
TryOld:
            _asm    {
                    mov     bl, driveLetter
                    sub     bl, 'A'-1       // drive code (0=default, 1=A, etc)
                    mov     ax, 440Dh       // IOCTL, Generic block dev i/o control
                    mov     cx, 0847h       // major = drive, minor = SET flag
                    lea     dx, wValue      // parameter block (assume DS == SS!)
                    DOS
                                            // Only override the FALSE
                                            // return value if there's no error
                    jc      Bad_Call
                    inc     returnValue     // set return to TRUE
Bad_Call:
                    }

            }

#endif
        }

    return (returnValue);
}
#pragma optimize("", on)




/*----------------------------------------------------------------------*
 *                                                                      *
 * void DiskSetLockedTable(Byte lockedTable[26])                        *
 *      Locks a drive that is locked.  Note that the lockedTable has    *
 *      already been built by 'DiskGetLockedTable()'.                   *
 *----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI DiskSetLockedTable(LPBYTE lockedTablePtr)
{
    auto        BYTE            dl;


    for (dl = 'A'; dl <= SYM_MAX_DOS_DRIVES; dl++, lockedTablePtr++)
        {
        if (*lockedTablePtr)
            {
            DiskSetAccessFlag(dl, DISALLOW_ACCESS);
            }
        }

}




/**********************************************************************
** DiskSetFormatType
**
** Description:
**      Sets the media type for formatting, at the same time returning the
**      diskette parameter table.  The physical characteristics are
**      in 'absDiskRecordPtr'.
**
** Returns:
**      0 if no error, else error code
**
**********************************************************************/

#ifdef  SYM_PROTMODE

UINT SYM_EXPORT WINAPI DiskSetFormatType (LPCABSDISKREC absDiskRecPtr,
                                   DisketteParamRecord far *disketteParamPtr)
{
    auto        BYTE            cylinders;
    auto        BYTE            sectorsPerTrack;
    auto        union REGS      regs;
    auto        struct SREGS    sregs;
    auto        LPVOID          lpSrc;
    auto        UINT            wSelector;


    if (HWIsNEC())
	return (0);

    cylinders = (BYTE) (absDiskRecPtr->dwEndingTrack & 0x000000FF);

    sectorsPerTrack = (BYTE)
                      (((absDiskRecPtr->dwEndingTrack >> 2) & 0x00C0)
                        | absDiskRecPtr->dwEndingSector);

    regs.h.ah = 0x18;                   // Set Media Type for Format
    regs.h.ch = cylinders;              // number of cylinders
    regs.h.cl = sectorsPerTrack;        // sectors per track
    regs.h.dl = absDiskRecPtr->dn;      // drive (00h-7Fh floppy disk)
    regs.h.dh = 0;                      // clear normal head number
    regs.x.di = 0xFFFF;                 // illegal value to detect if supported

    IntWin(0x13, &regs, &sregs);

    if (regs.x.cflag || regs.x.di == 0xFFFF || regs.h.ah != 0)
        {
        if (regs.h.ah == 0)             // if status is zero
            regs.h.ah = 1;              // then change it to Illegal Function
        return ((UINT) regs.h.ah);
        }
    else
        {
                                        // get real mode far addr to disk
                                        // parameter table
        lpSrc = MAKELP(sregs.es, regs.x.di);

                                        // get a selector
        if (SelectorAlloc(FP_SEG(lpSrc), 0xFFFF, &wSelector))
            return(1);

                                        // change pointer to use selector
        FP_SEG(lpSrc) = wSelector;

                                        // copy table
        MEMMOVE(disketteParamPtr, lpSrc, 11);

                                        // free the selector
        SelectorFree(wSelector);
        return (0);
        }
}


#else           // DOS version

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DiskSetFormatType (LPCABSDISKREC absDiskRecPtr,
                                   DisketteParamRecord far *disketteParamPtr)
{
    auto        BYTE    cylinders;
    auto        BYTE    sectorsPerTrack;
    auto        UINT    wReturn;


    cylinders = (BYTE) (absDiskRecPtr->dwEndingTrack & 0x0000FF);

    sectorsPerTrack = (BYTE)
                      (((absDiskRecPtr->dwEndingTrack >> 2) & 0x00C0)
                        | absDiskRecPtr->dwEndingSector);

    _asm
        {
	push	ds
        lds     bx, absDiskRecPtr       ; Get ptr. to abs. disk info

                                        ; Set up track, sector
        mov     ch, cylinders           ; number of cylinders
        mov     cl, sectorsPerTrack     ; sectors per track

        mov     dl, [bx]LPABSDISKREC.dn ; Get the drive number
        xor     dh, dh                  ; Clear normal head number

        mov     di, 0FFFFh              ; illegal value to detect if supported

        mov     ah, 18h                 ; Set Media Type for Format
        stc                             ;
        int     13h                     ; Abs. sector service
        jc      errorLabel              ; If error, return errror

        cmp     di, 0FFFFh              ; Is ptr. value legal?
        je      errorLabel              ; No, return error

        or      ah, ah                  ; Is the error state = 0?
        jne     errorLabel              ; No, return error

                                        ; Move ES:DI values into DS:SI
                                        ; since DS:SI => ES:DI
        mov     si, di                  ; SI has to point to table
        push    es
        pop     ds
        les     di, disketteParamPtr    ; Get ptr. to local table

                                        ; Move table into our local diskette
                                        ; parameter table
        mov     cx, 11                  ; Move 11 bytes (Table length)
        cld                             ; Forward direction
        rep     movsb                   ; Move 11 byte table
        xor     ax, ax                  ; Return that table is valid
        jmp     short returnLabel

errorLabel:
        xchg    ah, al                  ; Reverse error code
        xor     ah, ah                  ; Clear high-byte
        or      ax, ax                  ; Is it 0?
        jnz     returnLabel             ; No, return
        mov     ax, 1                   ; Return 'Invalid function'

returnLabel:
	pop	ds
        mov     wReturn, ax
        }

    return (wReturn);
}
#pragma optimize("", on)

#endif


#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI DiskSetDASDType (BYTE driveNum, BYTE DASDType)
{
#ifdef  SYM_PROTMODE
    auto        union REGS      regs;
    auto        struct SREGS    sregs;


    if (HWIsNEC())
	return (0);

    regs.h.dl = driveNum;
    regs.h.al = DASDType;
    regs.h.ah = 0x17;
    IntWin(0x13, &regs, &sregs);
    return(regs.h.al);
#else
    BYTE        byReturn;


    if (HWIsNEC())
	return (0);

    _asm
        {
        mov     dl, driveNum            ; Get physical drive number
        mov     al, DASDType            ; Get DASD type

        mov     ah, 17h                 ; Set DASD Type for Format service
        stc                             ; Force to error condition
        int     13h                     ; Physical disk service handler
        mov     byReturn, al
        }

    return (byReturn);
#endif
}
#pragma optimize("", on)

#endif  // !defined(SYM_WIN32) && !defined(SYM_VXD)


/*----------------------------------------------------------------------*/
/* DiskGetDASDType                                                      */
/*      Gets the DASD type for a physical drive.                        */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskGetDASDType(BYTE drive, LPBYTE DASDTypePtr)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
    static FARPROC      lpfnDiskGetDASDType;
    extern HINSTANCE    hInstThunkDll;


    if (HWIsNEC())
        {
		*DASDTypePtr = NEC_DRV_TYPE;
		return (FALSE);
        }

    if (lpfnDiskGetDASDType == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
            lpfnDiskGetDASDType = GetProcAddress(hInstThunkDll,"DiskGetDASDType32" );
            if (lpfnDiskGetDASDType == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnDiskGetDASDType(drive, DASDTypePtr) );

#endif // _M_ALPHA
#elif defined(SYM_PROTMODE)

    auto        struct SREGS    sregs;
    auto        union REGS      regs;


    if (HWIsNEC())
        {
	*DASDTypePtr = NEC_DRV_TYPE;
	return (FALSE);
        }

    regs.h.dl = drive;
    regs.h.ah = 0x15;
    IntWin(0x13, &regs, &sregs);
    *DASDTypePtr = regs.h.ah;           /* Get the DASD type            */

    return((BOOL) (regs.x.cflag || (regs.h.ah >= 9)));
#else
    auto        BOOL    bReturn = TRUE;

    if (HWIsNEC())
        {
	*DASDTypePtr = NEC_DRV_TYPE;
	return (FALSE);
        }

    _asm
        {
        mov     dl, drive
        mov     ah, 15h
        int     13h
        les     bx, DASDTypePtr
        mov     es:[bx], ah             /* Get the DASD type            */

        jnc     No_Error
        cmp     ah, 9
        jb      No_Error
        dec     bReturn                 // set return value to FALSE
No_Error:
        }

    return (bReturn);
#endif //SYM_WIN32
}
#pragma optimize("", on)


#endif  // !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// BOOL GetVolumeInformation();
//
// For information on this function, refer to the Windows API.  The only
// difference is, that this function is documented to not write to memory
// pointed to by a NULL formal parameter.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)
BOOL
WINAPI
GetVolumeInformation(
    LPCWSTR lpRootPathName,
    LPWSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPWSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize
    )
    {
    auto NTSTATUS        status;
    auto BOOL            bResult;
    auto HANDLE          hRoot;
    auto IO_STATUS_BLOCK IoStatusBlock;
    auto struct
             {
             union
                 {
                 FILE_FS_VOLUME_INFORMATION    FsVol;
                 FILE_FS_ATTRIBUTE_INFORMATION FsAttr;
                 };
             WCHAR szPadding[SYM_MAX_PATH];
             } NtVolInfo;

    SYM_ASSERT ( lpRootPathName );
    SYM_VERIFY_STRING ( lpRootPathName );

    hRoot = FileOpenRootHandle ( lpRootPathName );

    if ( hRoot == NULL )
        {
        return ( FALSE );
        }

    bResult = TRUE;

    if ( lpVolumeNameBuffer || lpVolumeSerialNumber )
        {
        status = ZvQueryVolumeInformationFile ( hRoot,
                                                &IoStatusBlock,
                                                &NtVolInfo,
                                                sizeof(NtVolInfo),
                                                FileFsVolumeInformation );

        if ( NT_SUCCESS ( status ) )
            {
            if ( lpVolumeNameBuffer && nVolumeNameSize )
                {
                SYM_VERIFY_BUFFER ( lpVolumeNameBuffer, nVolumeNameSize * sizeof(TCHAR) );
                NtVolInfo.FsVol.VolumeLabelLength /= sizeof(TCHAR);
                if ( nVolumeNameSize > NtVolInfo.FsVol.VolumeLabelLength )
                    {
                    nVolumeNameSize = NtVolInfo.FsVol.VolumeLabelLength;
                    }
                else
                    {
                    nVolumeNameSize--;
                    }
                STRNCPY ( lpVolumeNameBuffer, NtVolInfo.FsVol.VolumeLabel, nVolumeNameSize );
                lpVolumeNameBuffer[nVolumeNameSize] = '\0';
                }

            lpVolumeSerialNumber && ( *lpVolumeSerialNumber = NtVolInfo.FsVol.VolumeSerialNumber );
            }
        else
            {
            bResult = FALSE;
            }
        }

    if ( bResult && ( lpMaximumComponentLength || lpFileSystemFlags || lpFileSystemNameBuffer ) )
        {
        status = ZvQueryVolumeInformationFile ( hRoot,
                                                &IoStatusBlock,
                                                &NtVolInfo,
                                                sizeof(NtVolInfo),
                                                FileFsAttributeInformation );

        if ( NT_SUCCESS ( status ) )
            {
            lpMaximumComponentLength && ( *lpMaximumComponentLength = NtVolInfo.FsAttr.MaximumComponentNameLength );

            lpFileSystemFlags && ( *lpFileSystemFlags = NtVolInfo.FsAttr.FileSystemAttributes );

            if ( lpFileSystemNameBuffer && nFileSystemNameSize )
                {
                SYM_VERIFY_BUFFER ( lpFileSystemNameBuffer, nFileSystemNameSize * sizeof(TCHAR) );
                NtVolInfo.FsAttr.FileSystemNameLength /= sizeof(TCHAR);
                if ( nFileSystemNameSize > NtVolInfo.FsAttr.FileSystemNameLength )
                    {
                    nFileSystemNameSize = NtVolInfo.FsAttr.FileSystemNameLength;
                    }
                else
                    {
                    nFileSystemNameSize--;
                    }
                STRNCPY ( lpFileSystemNameBuffer, NtVolInfo.FsAttr.FileSystemName, nFileSystemNameSize );
                lpFileSystemNameBuffer[nFileSystemNameSize] = '\0';
                }
            }
        else
            {
            bResult = FALSE;
            }
        }

    FileCloseRootHandle ( hRoot );

    return ( bResult );
    }
#endif

//////////////////////////////////////////////////////////////////////////
//DiskGetFileSCN
//
//
//PARAMS : Pass in a fully qualified path for file or directory
//         Note that this call is only available for Win32 platform
//         Calls in DOS will always fail (as per MS documentation).
//
//         Microsoft says that this call should be made under a level 3
//         lock.
//
//         dwCharSet = BCS_WANSI for Windows ANSI char set
//                     BCS_OEM   for Current OEM character set
//                     BCS_UNICODE for Unicode character set
//
//         (defined in IFS.H)
//
//RETURNS: success or failure
//         if success, lpSCN will contain starting cluster for file.
//////////////////////////////////////////////////////////////////////////
BOOL  SYM_EXPORT WINAPI   DiskGetFileSCN(LPSTR lpFilename,LPCLUSTER lpSCN,LPDISKREC lpdisk,DWORD dwCharSet)
{
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#elif defined(SYM_WIN32)
    auto DIOC_REGISTERS      r;


    MEMSET(&r, 0, sizeof(DIOC_REGISTERS));

    r.reg_EAX = 0x0000440d;           // IOCTL
    r.reg_EBX = dwCharSet;
    r.reg_ECX = 0x4871;           // try 48 device catagory first
    r.reg_EDX = (DWORD) lpFilename;

    IOCtlVWin32( &r, &r, VWIN32_DIOC_DOS_IOCTL);
    if (r.reg_Flags & 1)
        {
        // 48xx version failed. fall back to 08 version.
        MEMSET(&r, 0, sizeof(DIOC_REGISTERS));
        r.reg_EAX = 0x0000440d;           // IOCTL
        r.reg_EBX = dwCharSet;
        r.reg_ECX = 0x0871;
        r.reg_EDX = (DWORD) lpFilename;
        IOCtlVWin32( &r, &r, VWIN32_DIOC_DOS_IOCTL);
        if (r.reg_Flags & 1)
            return(FALSE);
        }

    *lpSCN = (r.reg_EDX << 16) |  r.reg_EAX;

    if (*lpSCN < 2 || *lpSCN > lpdisk->dwMaxClusterEx)
        return(FALSE);


    return(TRUE);
#else
    return(FALSE);
#endif
}   //DiskGetFileSCN



/****************************************************************************
Declaration: UINT _DiskCheckLBAProblem(BYTE byDriveLetter, LPBOOL lpbHasProblem)

Description:
Tests if a drive requires LBA (Linear Block Addressing) but does not have
LBA in effect.

Parameters:
byDriveLetter
Drive letter to test.

$lpbHasProblem$
Pointer to result.  The result is TRUE if LBA is required but missing,
FALSE otherwise.

Returns:
NOERR if the test was successful.
ERR otherwise.

See: DiskCheckLBAProblem

Include: disk.h

Compatibility: Win16, Win32, DOS
*****************************************************************************/

#if defined(SYM_WIN16) || defined(SYM_WIN32) || defined(SYM_DOS)

STATIC BOOL WINAPI _DiskCheckLBAProblem(BYTE byDriveLetter, LPBOOL lpbHasProblem)
{
    auto        ABSDISKREC      absDisk;
    auto        ABSDISKREC      absDiskBasic;
    auto        DISKREC         diskrec;
    auto        DWORD           dwNumPhysicalSectors;
    auto        DWORD           dwNumPhysicalSectorsActual;
    auto        DWORD           dwNumPhysicalSectorsBasic;
    auto        LPBYTE          lpMBR;
    auto        LPBYTE          lpSectorBuffer;
    auto        DWORD           dwSector;
    auto        DWORD           head, sector, track;
    auto        BOOL            bError = FALSE;
    auto        INT13REC        int13Package;



    *lpbHasProblem = FALSE;

    MEMSET(&absDisk, 0, sizeof(ABSDISKREC));
    MEMSET(&absDiskBasic, 0, sizeof(ABSDISKREC));
    MEMSET(&diskrec, 0, sizeof(DISKREC));
    MEMSET(&int13Package, 0, sizeof(INT13REC));

    // Get the Int13 unit number.  If this is a device
    // driven drive, return NOERR
    DiskMapLogToPhyParams(0L, byDriveLetter, &int13Package);
    if (!int13Package.generated)
        return NOERR;

    // Save off Int13 unit number.
    absDisk.dn = absDiskBasic.dn = int13Package.dn;

    // Get actual disk parameters from windows.
    DiskGetPhysicalInfo(&absDisk);
    if(absDisk.bIsExtInt13 == ISINT13X_YES)
        {
#ifndef SYM_WIN32 // DOS/WIN
        DiskGetPhysicalInfoBasic(&absDiskBasic);
#else           // WIN32
        DiskGetPhysicalInfoEx(&absDiskBasic, &absDisk);
#endif
        }
    else
        {
        // If this isn't an extended int13 drive, then just use the translated
        // values that are in absDisk to fill out absDiskBasic
        absDiskBasic = absDisk;
        }

    // Get logical info
    DiskGetInfo(byDriveLetter, &diskrec);

    // Compute sectorsBeforePartition.
    diskrec.sectorsBeforePartition = ConvertPhySectorToLong(&absDisk,
                    int13Package.dwHead,
                    int13Package.dwTrack,
                    int13Package.dwSector);

    // This is the value we get using extended int13 info and compensating
    // for the diagnostic cylinder (adding an extra cylinder)
    dwNumPhysicalSectorsActual = ConvertPhySectorToLong(&absDisk, absDisk.dwTotalHeads,
                                 (absDisk.dwTotalTracks + 1), absDisk.dwSectorsPerTrack);

    // This is the value we get using plain int13 info and compensating
    // for the diagnostic cylinder (adding an extra cylinder)
    dwNumPhysicalSectorsBasic = ConvertPhySectorToLong(&absDiskBasic, absDiskBasic.dwTotalHeads,
                                (absDiskBasic.dwTotalTracks + 1), absDiskBasic.dwSectorsPerTrack);

    // Go with whichever of the above has the higher value
    dwNumPhysicalSectors = (dwNumPhysicalSectorsActual > dwNumPhysicalSectorsBasic) ? dwNumPhysicalSectorsActual : dwNumPhysicalSectorsBasic;

    // If we are on a drive smaller than 528Mb, no LBA.
    // Return NOERR
    if( diskrec.totalSectors + diskrec.sectorsBeforePartition < 0x100000 )
        return NOERR;

    // If there are more logical sectors than physical sectors
    // there is a LBA problem! However, we've seen cases where the CHS totals
    // indicate the drive is smaller than it really is, so also look at the total
    // sectors field of the ABSDISKREC before complaining.
    if( ((diskrec.totalSectors + diskrec.sectorsBeforePartition) > dwNumPhysicalSectors) &&
        ((diskrec.totalSectors + diskrec.sectorsBeforePartition) > absDisk.dwMaxSectorsLo) )
        {
        *lpbHasProblem = TRUE;
        return NOERR;
	    }

    // Alloc buffers.  If we have a failure, return err
    if((lpMBR = (LPBYTE) MemAllocFixed(512)) == NULL)
        return ERR;

    if((lpSectorBuffer = (LPBYTE) MemAllocFixed(512)) == NULL)
        {
        MemFreeFixed(lpMBR);
        return ERR;
        }

    // Read in MBR
    absDisk.dwHead = 0;
    absDisk.dwTrack = 0;
    absDisk.dwSector = 1;
    absDisk.buffer = lpMBR;
    absDisk.numSectors = 1;
    if(DiskAbsOperation(READ_COMMAND, &absDisk))
        {
        bError = TRUE;
        goto cleanup;
        }

    // Compute physical coordinates of 1st sector beyond 528Mb
    dwSector = 0x00100000;
    ConvertLongToPhySector(&absDisk, dwSector,
                            &head, &track, &sector);

    // If we get a track number > 1023, we have a mapping problem!

// Not true anymore due to extended int13 support
/*    if(!HWIsNEC() && (track > 0x3ff))
        {
        *lpbHasProblem = TRUE;
        goto cleanup;
        }*/

    // Read 1st sector past 512mb mark
    // this value is zero based.
    absDisk.dwHead = head;
    absDisk.dwTrack = track;
    absDisk.dwSector = sector;
    absDisk.buffer = lpSectorBuffer;
    absDisk.numSectors = 1;

    if(DiskAbsOperation(READ_COMMAND, &absDisk))
        {
        bError = TRUE;
        goto cleanup;
        }

    // If we find the MBR at the last sector, we've wrapped!
    if(MEMCMP(lpMBR, lpSectorBuffer, 512) == 0)
        {
        *lpbHasProblem = TRUE;
        goto cleanup;
        }

cleanup:
    // Cleanup.
    MemFreeFixed(lpMBR);
    MemFreeFixed(lpSectorBuffer);
    return (bError) ? ERR : NOERR;
}

#endif


/*@API:**********************************************************************
@Declaration: UINT DiskCheckLBAProblemEx(BYTE byDriveLetter, LPBOOL lpbHasProblem, LPBYTE lpbyRealDriveLetter)

@Description:
Tests if a drive requires LBA (Linear Block Addressing) but does not have
LBA in effect.  If it is a compressed drive, the host drive is tested.

@Parameters:
$byDriveLetter$
Drive letter to test.

$lpbHasProblem$
Pointer to result.  The result is TRUE if LBA is required but missing,
FALSE otherwise.

$lpbyRealDriveLetter$
Pointer to buffer to receive drive letter to report.  If byDriveLetter is a
compressed drive, this is the host; otherwise, it is byDriveLetter.
This parameter may be NULL.

@Returns:
NOERR if the test was successful.
ERR otherwise.

@See:

@Include: disk.h

@Compatibility: Win16, Win32, DOS
*****************************************************************************/

#if defined(SYM_WIN16) || defined(SYM_WIN32) || defined(SYM_DOS)

UINT SYM_EXPORT WINAPI DiskCheckLBAProblem(BYTE byDriveLetter, LPBOOL lpbHasProblem, LPBYTE lpbyRealDriveLetter)
{
    BYTE byRealDriveLetter;

    *lpbHasProblem = FALSE;

    if (DiskIsDoubleSpace(byDriveLetter))
        {
        WORD bSwapped;
        BYTE bySequence;
        if (!_DiskIsDoubleSpace(byDriveLetter, &byRealDriveLetter, &bSwapped, &bySequence))
	    return ERR;
        }

#if FALSE  // defined(SYM_WIN16) || defined(SYM_DOS)
    else if (DiskIsSuperStor(byDriveLetter))
	{
        // byRealDriveLetter = DiskGetSuperStorSwapDrive(byDriveLetter);
        byRealDriveLetter = DiskGetSuperStorHostDrive(byDriveLetter);
        if (byRealDriveLetter == 0)
            return ERR;
        }
#endif

#if FALSE  // defined(SYM_DOS)
    else if (DiskIsStacker(byDriveLetter))
        {
#if defined(SYM_DOSX)
	PERUNIT UnitData;
	if (StackerGetUnitData(byDriveLetter, &UnitData))
	    byRealDrivLetter = UnitData.lg_filedrive);
#else
        s_ushort status;
        if (stacker_status(&status) == SE_SUCCESSFUL)
	    {
	    struct stacker_drive_info_struct driveInfo;
	    driveInfo.driveLetter = byDriveLetter;
	    if (stacker_driveInfo(&driveInfo) != SE_SUCCESSFUL || TRUE)  // ignore return code?
                {
		// stacker_freeApiSelectors();  // needed?
		return ERR;
		}
	    byRealDriveLetter = driveInfo.hostDriveLetter;
	    // stacker_freeApiSelectors();  // needed?
	    }
        else
	    return ERR;
#endif
        }
#endif
    else
        byRealDriveLetter = byDriveLetter;

    if (lpbyRealDriveLetter != NULL)
        *lpbyRealDriveLetter = byRealDriveLetter;

    return _DiskCheckLBAProblem(byRealDriveLetter, lpbHasProblem);
}

#endif



