/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/fop_disk.c_v   1.6   18 Nov 1996 19:46:14   BMCCORK  $ *
 *                                                                      *
 * Description:                                                         *
 *      Common disk routines usually associated with file operations.   *
 *                                                                      *
 * Contains:                                                            *
 *      DiskGet                                                         *
 *      DiskSet                                                         *
 *      DiskGetFreeSpace                                                *
 *      DiskFreeAvailGet                                                *
 *      DiskGetClusterSize                                              *
 *      DiskGetAllocInfo                                                *
 *      DiskSetLogical                                                  *
 *      DiskGetEquipmentDrives                                          *
 *      DiskSetToSingleDisk                                             *
 *      DiskGetFreeBytes                                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/fop_disk.c_v  $ *
// 
//    Rev 1.6   18 Nov 1996 19:46:14   BMCCORK
// Fixed Win95 build 950 case of DiskGetFreeBytes() to return free bytes, duh
// 
//    Rev 1.5   18 Nov 1996 18:23:16   BMCCORK
// Fixed building of path in DiskGetFreeBytes() and made unsigned explicit
// 
//    Rev 1.4   18 Nov 1996 17:36:58   BMCCORK
// Added DiskGetFreeBytes()
// 
//    Rev 1.3   27 Jun 1996 14:31:36   SPASNIK
// If at first you dont succeed...
// 
// 
//    Rev 1.2   26 Jun 1996 19:49:26   SPASNIK
// os2 is 1 based not 0
// 
//    Rev 1.1   11 Apr 1996 19:13:36   THOFF
// Added DiskGet() and DiskSet() for the OS/2 platform.
// 
//    Rev 1.0   26 Jan 1996 20:22:42   JREARDON
// Initial revision.
// 
//    Rev 1.27   02 Aug 1995 01:32:54   AWELCH
// Syntax error.
// 
//    Rev 1.26   02 Aug 1995 01:06:52   AWELCH
// Merge changes from Quake 7.
// 
//    Rev 1.25   27 Jul 1995 14:22:02   BILL
// Merged branch to trunk
// 
//    Rev 1.24.3.1   29 Jun 1995 11:08:26   HENRI
// DiskGetAllocInfo should assume a media descriptor of F0 not FC
//
//    Rev 1.24.3.0   27 Jun 1995 18:49:18   RJACKSO
// Branch base for version QAK8
//
//    Rev 1.24   13 Dec 1994 11:22:02   BRAD
// Had problems with #ifdef on W32
//
//    Rev 1.23   12 Dec 1994 17:18:36   BRAD
// Added VxD support
//
//    Rev 1.22   04 Oct 1994 15:17:28   BRAD
// Incorporated NEC support
//
//    Rev 1.21   05 May 1994 13:22:08   BRAD
// DX support
//
//    Rev 1.20   23 Mar 1994 16:28:28   BRAD
// Cleaned up for new FindFirst handle stuff
//
//    Rev 1.19   21 Mar 1994 20:02:16   BRAD
// Made more routines visible to WIN32
//
//    Rev 1.18   21 Mar 1994 15:38:34   BRUCE
// Cleaned up pragmas and other compile errors
//
//    Rev 1.17   21 Mar 1994 00:20:00   BRAD
// Cleaned up for WIN32
//
//    Rev 1.16   15 Mar 1994 12:33:28   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.15   25 Feb 1994 15:03:16   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.13   04 Feb 1994 14:40:42   PGRAVES
// Win32 changes:
// 1. DiskGetFreeSpace now returns the number of free clusters, like it should.
// 2. Made DiskFreeAvailGet available for Win32 platform (no code changes).
//
//    Rev 1.12   03 Feb 1994 21:33:22   PGRAVES
// Win32 changes: DiskGet, DiskGetFreeSpace.
//
//    Rev 1.11   13 Oct 1993 15:10:16   MFALLEN
// Added DiskGetClusterSize() changes that Brad made to the branch but not to
// the trunk.
//
//    Rev 1.10   02 Oct 1993 19:09:28   BRUCE
// Added DiskGetClusterSizeEx()
//
//    Rev 1.9   02 Oct 1993 17:45:32   BRUCE
// [FIX] GetDiskType() takes a zero based drive number - subtrct 'A'
//
//    Rev 1.8   31 Aug 1993 21:47:50   BARRY
// DiskFreeAvailGet() now returns ERR if something went wrong
//
//    Rev 1.7   31 Aug 1993 21:42:10   BARRY
// Work-around SmartCan bug with INT 21h fn 36h -- DiskGetFreeSpace()
//
//    Rev 1.6   02 Jul 1993 08:51:58   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.5   05 May 1993 14:31:26   KEITH
// Stub out some low level routines that cause a GP in the OS/2 Mirrors
// environment.
//
//
//    Rev 1.4   15 Feb 1993 21:04:30   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.3   15 Feb 1993 08:25:28   ED
// Removed local copies of functions that used to be in other DLLs.  Now
// that we are one big happy family, everything is "local".
//
//    Rev 1.2   12 Feb 1993 04:28:34   ENRIQUE
// No change.
//
//    Rev 1.1   10 Sep 1992 13:43:56   BRUCE
// Eliminated unnecessary includes
//
//    Rev 1.0   27 Aug 1992 09:35:10   ED
// Initial revision.
//
//    Rev 1.1   12 Aug 1992 07:25:46   ED
// Modified the inline-assembly comments
//
//    Rev 1.0   20 Jul 1992 16:59:20   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#include "disk.h"
#include "undoc.h"
#include "file.h"
#include "xapi.h"

#if defined(SYM_OS2)

UINT SYM_EXPORT WINAPI DiskSet(BYTE driveLetter)
{
    ULONG       disknum;
    ULONG       current;
    ULONG       logical;
    UINT        nDrives = 0;

    for (disknum = 1; disknum <= 26; disknum++)
        {
        DosSetDefaultDisk(disknum);
        DosQueryCurrentDisk(&current, &logical);

        if (current == disknum)
            nDrives++;
        }

//    disknum = CharToUpper (driveLetter) - '@';

    disknum = driveLetter - '@';

    DosSetDefaultDisk( disknum );

    return nDrives;
}

BYTE SYM_EXPORT WINAPI DiskGet(VOID)
{
    ULONG       disknum;
    ULONG       logical;

    DosQueryCurrentDisk(&disknum, &logical);

    return (BYTE) disknum;
}

#else

/*----------------------------------------------------------------------*/
/* Default cluster size for network drives:                             */
/*----------------------------------------------------------------------*/
#define DEFAULT_BYTES_PER_CLUSTER       4096


#if !defined(SYM_VXD)
/*----------------------------------------------------------------------*/
/* DiskGet                                                              */
/*      Gets the current DOS drive letter.                              */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI DiskGet(VOID)
{
#ifdef SYM_WIN32
    char szDirName[SYM_MAX_PATH];

    GetCurrentDirectory(sizeof(szDirName), szDirName);
    return (BYTE)CharUpper((LPTSTR)szDirName[0]);
#else
    auto        BYTE    driveLetter;

    _asm
        {
        mov     ah, 19h                 // Get current DOS drive
        DOS
        add     al, 'A'                 // Convert to drive letter
        mov     driveLetter, al         // Save the drive letter
        }

    return(driveLetter);
#endif
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*/
/* DiskSet                                                              */
/*      Sets the current DOS drive letter and returns the drive count.  */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DiskSet(BYTE driveLetter)
{
    driveLetter = (BYTE) CharToUpper (driveLetter);

    SetDirChangedFlag();

#if defined(SYM_WIN32)
    {
    char        szDisk[10];
    UINT        uCount;
    DWORD       dwDriveMask;
    int         i;


    STRCPY(szDisk, "x:.");
    szDisk[0] = (char)driveLetter;
    SetCurrentDirectory(szDisk);
                                        // Get count of last drive which is
                                        // valid.
    dwDriveMask = GetLogicalDrives();
    uCount = 0;
    for (i = 1; i <= 32; i++, dwDriveMask >>= 1)
        {
        if ( dwDriveMask & 0x01 )
            uCount = i;                 // Not really a count.  Just last valid one
        }

    return(uCount);
    }
#else
    {
    auto        UINT    returnValue;

    _asm
        {
        mov     dl,driveLetter          // Get drive letter parm
        sub     dl, 'A'                 // Convert to drive number
        mov     ah, 0Eh                 // Set current disk
        DOS
        xor     ah,ah                   // convert count to full word
        mov     returnValue, ax         // Save the drive count
        }

    return(returnValue);
    }
#endif
}
#pragma optimize("", on)



#if !defined(SYM_WIN32)

/*----------------------------------------------------------------------*/
/* DiskGetEquipmentDrives                                               */
/*      Gets number of diskette drives from INT 11h.                    */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DiskGetEquipmentDrives(VOID)
{
    UINT        uReturnValue = 0;
    WORD        wEquipment;


    if (HWIsNEC())
        {
        MemCopyPhysical(&wEquipment, MAKELP(0, 0x055C), sizeof(wEquipment), 0);
	wEquipment &= 0xf0ff;
	for (; wEquipment; wEquipment = wEquipment >> 1 )
	    {
	    if (wEquipment & 1)
	    	uReturnValue++;
	    }
        }
    else
        {
        _asm
            {
            int     11h                     // Get current equipment check into AX
            mov     cl, 6                   // Put number of drive in low bits
            shr     ax, cl                  //
            and     ax, 0003h               // Mask out other equipment status
            inc     al                      // Set to number of drives
            mov     uReturnValue, ax        // Return number of drives
            }
        }
    return(uReturnValue);
}
#pragma optimize("", on)


/*---------------------------------------------------------------------*/
/* SetToSingleDrive (Byte dl)                                           */
/*      Sets the drive so that the user will not get a message like:    */
/*                                                                      */
/*              Insert diskette for drive X: and strike any key         */
/*                                                                      */
/*         This message is displayed when one physical drive is         */
/*         actually 2 logical drives.  For example, a single drive      */
/*         system uses the single drive as both A: and B:.  This        */
/*         will make sure that this prompt is not encountered.          */
/* 05/15/90 - Enrique: Modified to run under MSW 3.0                    */
/*                     Removed the call to HWGetEquipmentList ()        */
/*                     and replaced it with a call to                   */
/*                     DiskGetEquipmentDrives.                          */
/* 08/08/88 - Brad: created                                             */
/*----------------------------------------------------------------------*/

#ifndef SYM_MIRRORS

VOID SYM_EXPORT WINAPI DiskSetToSingleDisk (BYTE dl)
{
    if (HWIsNEC())
        return;

    dl = (BYTE) CharToUpper (dl);

    if (!DiskSetLogical (dl))
        {
        auto    UINT    dosVersion;
        auto    UINT    numPhysicalDiskettes;
        auto    BYTE    dn;



        dosVersion = DOSGetVersion();
                                        /* Get number of physical       */
                                        /*   floppy diskettes           */
        numPhysicalDiskettes = DiskGetEquipmentDrives();

        dn = (BYTE)(dl - 'A');          /* Get drive number             */

                                        /* See if we need to set        */
                                        /*   phantom drive indicator    */
                                        /*   in low RAM                 */
                                        /* See if we have to simulate   */
                                        /*   a SetLogicalDrive () call  */

        if ((numPhysicalDiskettes == 1) && /* Only 1 physical diskette? */
             (dosVersion < DOS_3_20) &&  /* DOS < 3.20?                 */
             (dn <= 1))                  /* Drive A: or B:              */
            {
            MemCopyPhysical (MAKELP(0x0040, 0x0104), &dn, 1, MCP_SRC_VIRTUAL | MCP_DEST_PHYSICAL);
            }
        }
}

#else // SYM_MIRRORS
VOID SYM_EXPORT WINAPI DiskSetToSingleDisk (BYTE dl)
{
    dl;

    DebugOutput ( DBF_TRACE, "DiskSetToSingleDisk of dsk_type.c is a stub" );
    return ( 0 );
}
#endif // SYM_MIRRORS


/*----------------------------------------------------------------------*/
/* DiskSetLogical()                                                     */
/*      Sets the logical drive to drive 'driveLetter'.                  */
/*                                                                      */
/*      Returns TRUE if drive is network, else FALSE.                   */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskSetLogical(BYTE driveLetter)
{
    auto        BOOL    returnValue = FALSE;


    driveLetter = (BYTE) CharToUpper (driveLetter);

    if (DOSGetVersion() >= DOS_3_20)
        {
        _asm
        {
        mov     bl, driveLetter         // Get drive letter
        sub     bl, 'A' - 1             // Set to logical drive number
        mov     ax, 440Fh               // Set logical drive function
        DOS
        jc      errorLabel              // Error setting

        mov     ax, TRUE                // Set okay, return
        jmp     short locRet            //    to caller

errorLabel:
        xor     ax, ax                  // Return ERROR (0)

locRet:
        mov     returnValue, ax         // Save the return value
        }
        }

    return(returnValue);
}
#pragma optimize("", on)

#endif                                  // !defined(SYM_WIN32)




#if defined(SYM_WIN32)
/*----------------------------------------------------------------------*/
/* DiskGetFreeBytes()                                                   */
// Returns the number of free bytes available to the caller on the
// given drive (which may be less than the total free space on the drive
// because of an NT Quota).
// Return NOERR on success, ERR on failure.
// This code was borrowed from NUCORE, N32DLIST, NLSTCLS.CPP
/*----------------------------------------------------------------------*/
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DiskGetFreeBytes(// Returns NOERR or ERR
    TCHAR byDriveLetter,
    PULARGE_INTEGER pulFreeBytes)       // Returns number of free bytes available to caller on this drive
{

                                        // Now call the Win32
                                        // GetDiskFreeSpace function to
                                        // get the last parts of the puzzle
                                        // we can get from Win32.
										// Check that we got volume info so
										// we don't ping empty removable drives
										// more than once.
    #ifdef _UNICODE
        const TCHAR FREESPACEFUNCTION[] = _T("GetDiskFreeSpaceExW");
    #else
        const TCHAR FREESPACEFUNCTION[] = _T("GetDiskFreeSpaceExA");
    #endif
    HMODULE     hKernel32 = GetModuleHandle("KERNEL32.DLL");
    BOOL        (WINAPI *pGetDiskFreeSpaceEx)(LPCTSTR, PULARGE_INTEGER ,PULARGE_INTEGER, PULARGE_INTEGER);
    BOOL        bSuccess = FALSE;
    TCHAR       szPath[4];
    
    szPath[0] = byDriveLetter;
    szPath[1] = _T(':');
    szPath[2] = _T('\\');
    szPath[3] = _T(EOS);

    pGetDiskFreeSpaceEx = (int (WINAPI *)(LPCTSTR, PULARGE_INTEGER ,PULARGE_INTEGER, PULARGE_INTEGER))
                            GetProcAddress(hKernel32, FREESPACEFUNCTION);

    if (pGetDiskFreeSpaceEx != NULL)
        {
                                        // This case will be entered for Win95 OSR2 and Windows NT
        ULARGE_INTEGER ulFree;          // Total free bytes on disk (which may be more than returned due to user quotas on NT)
        ULARGE_INTEGER ulTotalNumberOfBytes;

        bSuccess = pGetDiskFreeSpaceEx(szPath, pulFreeBytes, &ulTotalNumberOfBytes, &ulFree);
        }
    else
        {
        /*----------------08-19-96 08:44pm------------------
        ** This branch is for Windows/95 b950
        ** ------------------------------------------------*/
        DWORD dwMaxCluster;
        DWORD dwSectsPerClust;
        DWORD dwBytesPerSect;
        DWORD dwAvailClust;

        if (GetDiskFreeSpace(szPath,
                         &dwSectsPerClust,
                         &dwBytesPerSect,
                         &dwAvailClust,
                         &dwMaxCluster))
			{
            pulFreeBytes->QuadPart = (unsigned __int64)dwSectsPerClust * (unsigned __int64)dwBytesPerSect *
                                     (unsigned __int64)dwAvailClust;
		    bSuccess = TRUE;
			}
        }
    return (bSuccess ? NOERR : ERR);    
}
#endif                                  // #ifdef SYM_WIN32


/*----------------------------------------------------------------------*/
/* DiskGetFreeSpace                                                     */
// Returns the number of free clusters on success, 0 on failure.
/*----------------------------------------------------------------------*/
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DiskGetFreeSpace(BYTE bDriveLetter,
                                    UINT FAR * lpwBytesSector,
                                    UINT FAR * lpwSectorsCluster,
                                    UINT FAR * lpwClusters)
{
#if defined(SYM_WIN32)

    char szRootPathName[4];
    DWORD dwFreeClusters;
    BOOL bSuccess;

    szRootPathName[0] = bDriveLetter;
    szRootPathName[1] = ':';
    szRootPathName[2] = '\\';
    szRootPathName[3] = '\0';
    bSuccess = GetDiskFreeSpace(szRootPathName,
                                lpwSectorsCluster,
                                lpwBytesSector,
                                &dwFreeClusters,
                                lpwClusters);
    return (bSuccess ? dwFreeClusters : 0);

#else

    auto        UINT    wRCode;
    auto        char    szCurrentDir [ SYM_MAX_PATH ] ;

    bDriveLetter = (BYTE) CharToUpper (bDriveLetter);

    // 08/31/93 BarryG:
    // SmartCan will hang the system on fn 36h calls if the critical
    // error handler appears (i.e., no disk in drive, etc.) and the
    // user selects retry twice.  To avoid this problem, we hit the
    // drive with a benign call that SmartCan won't choke on.  If
    // it succeeds, the chance of fn 36h getting a critical error
    // are reduced to near zero.

    if ( DirGet ( bDriveLetter , szCurrentDir ) != 0 )
        {
        * lpwBytesSector    = 0 ;
        * lpwSectorsCluster = 0 ;
        * lpwClusters       = 0 ;
        return (0) ;
        }

    _asm
        {
        push    ds                      // Save registers
        push    si

        mov     dl,bDriveLetter         // get drive letter
        SUB     dl,'A'-1                // convert to BIASED drive number

        MOV     AH,36H                  // get disk information
        DOS
        CMP     AX,0FFFFH               // if error
        JNE     RETURN                  //   zero results

        XOR     AX,AX                   // zeros for error
        XOR     BX,BX
        XOR     CX,CX
        CWD
RETURN:
        LDS     SI,lpwBytesSector       //  sector size in bytes
        MOV     [SI],CX

        LDS     SI,lpwSectorsCluster    //  sectors per allocation unit
        XOR     AH,AH                   //  just in case
        MOV     [SI],AX

        LDS     SI,lpwClusters          //  number of allocation units
        MOV     [SI],DX

        pop     si
        pop     ds

        MOV     wRCode,BX               //  free clusters
        }

    return  (wRCode);
#endif                                  // !SYM_WIN32
}
#pragma optimize("", on)

/*----------------------------------------------------------------------*/
/* DiskFreeAvailGet                                                     */
/*    This function is used to get the space currently being used       */
/*    and the space available.                                          */
/*                                                                      */
/* INPUTS                                                               */
/*    BYTE      dl              the driveletter                         */
/*    LPDWORD   lpdwAvail       the available disk space in bytes.      */
/*    LPDWORD   lpdwUsed        the used disk space                     */
/*                                                                      */
/* RETURNS                                                              */
/*    standard                                                          */
/*----------------------------------------------------------------------*/
UINT SYM_EXPORT WINAPI DiskFreeAvailGet (
    BYTE dl,
    LPDWORD lpdwAvail,
    LPDWORD lpdwUsed)
{
    auto        UINT            wFreeClusters;
    auto        UINT            wBytesSector;
    auto        UINT            wSectorsCluster;
    auto        UINT            wTotalClusters;
//    auto        UINT            wBytesCluster;
    auto        DWORD           dwBytesCluster;


    wFreeClusters = DiskGetFreeSpace(dl,
                                     &wBytesSector,
                                     &wSectorsCluster,
                                     &wTotalClusters);

    dwBytesCluster = (DWORD)wBytesSector * (DWORD)wSectorsCluster;
    *lpdwAvail = dwBytesCluster * (DWORD)wFreeClusters;
    *lpdwUsed = dwBytesCluster *
                       (DWORD)(wTotalClusters - wFreeClusters);

    if ( wTotalClusters == 0 )
        return (ERR) ;
    else
        return (NOERR);
}

#endif


/*----------------------------------------------------------------------*/
/* DiskGetAllocInfo                                                     */
/*      Returns the allocation info for a drive.                        */
/*                                                                      */
/*  Returns:                                                            */
/*      TRUE  - Information is valid                                    */
/*      FALSE - Information is not valid                                */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskGetAllocInfo(BYTE driveLetter,
                                LPBYTE mediaDescriptorPtr,
                                LPBYTE sectorsClusterPtr,
                                UINT FAR * totalClustersPtr,
                                UINT FAR * bytesPerSectorPtr)
{
#if defined(SYM_WIN32)
    char        szDrive[10];
    DWORD       dwSectorsPerCluster, dwBytesPerSector, dwFreeClusters, dwClusters;
    BOOL        bOk;

    *mediaDescriptorPtr = 0xF0;         // Assume "other type" of media descriptor

    STRCPY(szDrive, "x:\\");
    szDrive[0] = (char)driveLetter;
    bOk = GetDiskFreeSpace(szDrive,  &dwSectorsPerCluster,
                                     &dwBytesPerSector,
                                     &dwFreeClusters,
                                     &dwClusters);
    *sectorsClusterPtr  = (BYTE) dwSectorsPerCluster;
    *totalClustersPtr   = (UINT) dwClusters;
    *bytesPerSectorPtr  = (UINT) dwBytesPerSector;
    return(bOk);

#elif defined(SYM_VXD)

    extern BOOL SYM_EXPORT WINAPI __DiskGetAllocInfo(BYTE driveLetter,
                                        LPBYTE mediaDescriptorPtr,
                                        LPBYTE sectorsClusterPtr,
                                        UINT FAR * totalClustersPtr,
                                        UINT FAR * bytesPerSectorPtr);


                                        // Call VSYMKRNL
    return(__DiskGetAllocInfo((BYTE) CharToUpper(driveLetter),
                             mediaDescriptorPtr,
                             sectorsClusterPtr,
                             totalClustersPtr,
                             bytesPerSectorPtr));

#else

    auto        BOOL    returnValue = FALSE;
    auto        BYTE    mediaDescriptor;
    auto        BYTE    sectorsPerCluster;
    auto        UINT    totalClusters;
    auto        UINT    bytesPerSector;


    driveLetter = (BYTE) CharToUpper (driveLetter);

    _asm
        {
        push    ds

        mov     dl, driveLetter         // Get the drive letter
        sub     dl, 'A' - 1             // Convert to a drive NUMBER

        xor     cx, cx                  // Clear for testing later

        clc                             // Set no error to OFF
        mov     ax, 1C00h               // Get allocation info function
        DOS
        jc      notValid                // Not valid, inform user of error

        cmp     al, 0FFh                // Is AL error state?
        je      notValid                // Yes, error

        or      cx, cx                  // Is CX still 0?
        je      notValid                // Yes, error

        test    cx, 1                   // Bytes/Sector MUST be even
        jnz     notValid                // Not even, thus error

// Save returned drive information

        mov     ah, [bx]                // Get Media Descriptor byte

        pop     ds                      // Restore DS

        mov     mediaDescriptor, ah     // Save media descriptor byte
        mov     sectorsPerCluster, al   // Save sectors per cluster
        mov     bytesPerSector, cx      // Save bytes per sector
        mov     totalClusters, dx       // Save total clusters

        mov     ax, TRUE                // Set to valid disk information
        jmp     short locRet            // Exit

notValid:
        pop     ds                      // Restore value
        xor     ax, ax                  // Set to invalid disk information

locRet:
        mov     returnValue, ax         // Get return value
        }

                                        /*------------------------------*/
                                        /* Save the values              */
                                        /*------------------------------*/
    *mediaDescriptorPtr = mediaDescriptor;
    *sectorsClusterPtr  = sectorsPerCluster;
    *totalClustersPtr   = totalClusters;
    *bytesPerSectorPtr  = bytesPerSector;

    return(returnValue);
#endif

}
#pragma optimize("", on)


/*********************************************************************
** DiskGetClusterSize()
**
** Description:
**
** Return values:
**    Cluster size       - Everything went fine
**    -1                 - error
**
***********************************************************************
** 01/06/90 BEM Function created from Dos library's clstrsiz.asm
**********************************************************************/

DWORD SYM_EXPORT WINAPI DiskGetClusterSize (BYTE driveLetter)
{
    auto        DWORD           dwBytesPerCluster;
    auto        BYTE            byMediaDescriptor;
    auto        BYTE            bySectorsPerCluster;
    auto        UINT            uTotalClusters, uBytesPerSector;


    if (DiskGetAllocInfo(driveLetter,
                         &byMediaDescriptor,
                         &bySectorsPerCluster,
                         &uTotalClusters,
                         &uBytesPerSector))
        {
        dwBytesPerCluster = (DWORD) uBytesPerSector *
                            (DWORD) bySectorsPerCluster;
        }
    else
        {
        dwBytesPerCluster = (DWORD) -1;
        }
                                        // Assume default for network
   if (((dwBytesPerCluster == (DWORD)-1) || !dwBytesPerCluster) &&
                                DiskIsNetwork (driveLetter))
      dwBytesPerCluster = DEFAULT_BYTES_PER_CLUSTER;

   return (dwBytesPerCluster);
}

#endif


