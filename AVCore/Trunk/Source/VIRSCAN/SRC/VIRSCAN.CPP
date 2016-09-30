// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/virscan.cpv   1.37   28 Aug 1998 17:39:32   JBRENNA  $
//
// Description: The file contains all the virscan layer implementation.
//              It is now an AVAPI 1.0 client.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/virscan.cpv  $
// 
//    Rev 1.37   28 Aug 1998 17:39:32   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.36.1.2   22 Jul 1998 16:24:40   DBuches
//    Added VirusScanGetScanContext().  Changed return type to LPVOID.
// 2. Rev 1.36.1.1   22 Jul 1998 16:03:38   DBuches
//    Added VirusScanGetScanContext().
// 
//    Rev 1.36   14 May 1998 07:37:06   tcashin
// Default return value for CanDeleteFile() needs to be FALSE.
// 
//    Rev 1.35   12 May 1998 13:17:24   tcashin
// Added VirusScanCanDeleteFile().
// 
//    Rev 1.34   16 Mar 1998 16:23:22   MKEATIN
// Made changes to match those in the new disk.h
// 
//    Rev 1.33   18 Feb 1998 14:17:26   mdunn
// Ported NEC code from the H branch.
// 
//    Rev 1.32   09 Sep 1997 13:01:00   MKEATIN
// Added the param, lpszNavexInfFile, to VirusScanInit().
// 
//    Rev 1.31   11 Aug 1997 13:36:56   MKEATIN
// Simplified some logic in VirusScanMapDriveLetter().
// 
//    Rev 1.30   11 Aug 1997 12:41:50   MKEATIN
// Changed '512' to SECTOR_SIZE and  fixed some comments.
// 
//    Rev 1.29   05 Aug 1997 14:14:26   MKEATIN
// Fixed comments.
// 
//    Rev 1.28   05 Aug 1997 13:18:20   MKEATIN
// Convert the physical sector of our logical boot record to a linear sector
// and compare that to linear sector in the partition entry.
// 
//    Rev 1.27   31 Jul 1997 16:48:46   MKEATIN
// We now relativeSectors and numSectors to get the physical params.
// 
//    Rev 1.26   24 Jul 1997 17:19:06   MKEATIN
// If VirusScanMapDriverLetter() fails during VirsusScanBoot() we read the 
// the boot sector with DiskBlocDevice().  DiskBlockDevice() may erroneously
// report success - so we copy a signature string into our buffer before the
// read and report failure if it's still there after the read.
// 
//    Rev 1.25   22 Jul 1997 21:59:44   MKEATIN
// Did some work on VirusScanBoot().
// 
//    Rev 1.24   22 Jul 1997 20:00:12   MKEATIN
// If VirusScanMapDriveLetter() fails, we now go get the boot record with
// a call to DiskBlocDevice() and scan the buffer - just as in the old
// virscan.
// 
//    Rev 1.23   16 Jul 1997 12:53:04   MKEATIN
// Fixed comments.
// 
//    Rev 1.22   16 Jul 1997 12:47:06   MKEATIN
// In VirusScanMapDriveLetter(), if the boot record's starting track number
// of a given logical drive is within a used partition then that is our 
// partition number.
// 
//    Rev 1.21   26 Jun 1997 16:23:04   MKEATIN
// VirusScanMapDriveLetter now decides a disk is partitionable if the 
// physical drive number is >= 0x80.
// 
//    Rev 1.20   24 Jun 1997 17:58:28   MKEATIN
// VirusScanMapDriveLetter now returns ERR on NT if it doesn't find a floppy
// or hard disk.
// 
//    Rev 1.19   18 Jun 1997 16:45:50   MKEATIN
// VirusScanSetPath now calls NAVSetDefsPath().
// 
//    Rev 1.18   17 Jun 1997 15:00:58   MKEATIN
// VirusScanInit() now takes an LPAVGLOBALCALLBACKS and passes it to
// VLScanInit().
// 
//    Rev 1.17   09 Jun 1997 13:21:06   MKEATIN
// extern-ed gstNAVEX32CallBacks. 
// 
//    Rev 1.16   09 Jun 1997 03:26:08   BGERHAR
// AreWildDefs as C function
// 
//    Rev 1.15   09 Jun 1997 02:51:30   BGERHAR
// Support wild def set for DOS platform
// 
//    Rev 1.14   06 Jun 1997 15:14:46   MKEATIN
// Passed a local PCALLBACKREV2 to VLScanInit() for WIN32.
// 
//    Rev 1.13   03 Jun 1997 14:01:42   MKEATIN
// Don't call VirusScanMapDriveLetter when repairing MBR's.  We already 
// have the drive number.
// 
// 
//    Rev 1.12   31 May 1997 17:36:00   JTAYLOR
// Updated the default virus definition file to virscan1.dat.
// 
//    Rev 1.11   31 May 1997 16:08:18   MKEATIN
// Added the lpvCookie parameter to Repair functions
// 
//    Rev 1.10   28 May 1997 08:03:22   TIVANOV
// change the code to the new API in avapi1 - VLMoveFile and VLMoveFileEx
// 
//    Rev 1.9   26 May 1997 15:09:04   BGERHAR
// Fixed problem with local prototype
// 
//    Rev 1.8   26 May 1997 15:05:30   BGERHAR
// Enabled VirusScanSetPath() for all platforms, removed VirusScanGetBootFile()
// 
//    Rev 1.7   22 May 1997 18:18:56   MKEATIN
// VirusScanMapDriveLetter() now works properly on NT.
// 
//    Rev 1.6   13 May 1997 13:50:34   TIVANOV
// new InocScanMoveFile function - created to support the quarantine functionali
// ty
// 
//    Rev 1.5   08 May 1997 16:50:42   MKEATIN
// Now, pass NULL instead of lpgCallBack to VLScanInit().
// 
//    Rev 1.4   08 May 1997 11:40:58   TIVANOV
// new param for VirScanMoveFile - buff to store the new filename
// 
//    Rev 1.3   06 May 1997 18:50:34   JTAYLOR
// Fixed two bugs and made the code more consistant.
//
//    Rev 1.2   05 May 1997 14:47:26   TIVANOV
// change VirusScanMoveFile to except a file extention as well
//
//    Rev 1.1   01 May 1997 07:59:26   TIVANOV
// implement VirusscanMoveFile function
//
//    Rev 1.0   29 Apr 1997 12:27:36   jtaylor
// Initial revision.
//************************************************************************

#include "platform.h"
#include "avapi.h"
#include "virscan.h"
#include "xapi.h"
#include "disk.h"
#include "navcb.h"

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

#ifndef SYM_NLM
UINT SYM_EXPORT WINAPI NAVEXSDiskReadPhysical
(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive 
);
#endif

CBSTATUS SYM_EXPORT NAVLinearIsPartitionedMedia
(
    LPVOID lpvInfo,
    LPBOOL lpbPartitioned
);

static HVCONTEXT hContext = NULL;
static char      SZ_STAR_STAR[]  = "*.*";

//************************************************************************
// Functions and Structures copied from ctsn.h during move to avenge.
//************************************************************************
#ifdef SYM_DOSX
    extern "C"
    {
        BOOL  WINAPI FIL4_FIND_ALL      (FARPROC,LPARAM,BOOL,LPSTR);
        BOOL  WINAPI FIL4_FIND_WILD     (LPSTR,FARPROC,LPARAM,BOOL,LPSTR);
    }
#endif

//************************************************************************
// Local Function Declarations.
//************************************************************************
STATUS LOCAL VirusScanMapDriveLetter (
    BYTE   byDriveLetter,
    LPBYTE lpbyDriveNumber,
    LPBYTE lpbyPartEntry
);

VOID LOCAL VirusCopyData ( 
    HVLVIRUS hVirus,
    LPUINT lpuVirusSigIndex,
    LPN30NOTES lpNotes 
);

//************************************************************************

//************************************************************************
// VirScanInit()
//
//  Initializes all the scanning specific buffers,
//  which implies that this function should be the
//  first to be called before scanning begins.
//  If this function returns ERR, don't even try
//  to scan, just call VirScanEnd() to clean up.
//
// Parameters:  None
//
// Returns:
//  NOERR;      If everything is ok.
//  ERR;        If there is a problem
//
//************************************************************************
// 3/\93 Martin created.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanInit 
(
    LPAVGLOBALCALLBACKS lpGlobalCallBacks, 
    LPSTR               lpszNavexInfFile
)
{
    auto    char    szDatFile [SYM_MAX_PATH + 1];
    auto    char    szInfFile [SYM_MAX_PATH + 1];

    if (hContext)
    {
        return (NOERR);
    }

    hContext = VLScanInit(1, lpGlobalCallBacks, 0);

                                        // Make sure we have a valid context
    if( hContext == NULL )
        return(ERR);

                                        // Find the Dat and Inf files
    VirusScanGetDatFile( szDatFile, SYM_MAX_PATH );
    VirusScanGetInfFile( szInfFile, SYM_MAX_PATH );

                                        // Register the Virus Database
    if (VLRegisterVirusDB( hContext,
                           szDatFile,
                           szInfFile,
                           lpszNavexInfFile) != VS_OK )
    {
        return (ERR);
    }

    return(NOERR);
}



//************************************************************************
// VirusScanEnd()
//
//  Cleans up after VirScanInit(). This function
//  should be called to terminate scanning.
//
// Parameters: none
//
// Returns:
//  TRUE if OK, or FALSE if error
//
//************************************************************************
// 3/\93 Martin function created.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************
BOOL SYM_EXPORT WINAPI VirusScanEnd (VOID)
{
    if (VLScanClose(hContext) != VS_OK)
    {
        return (FALSE);
    }

    hContext = NULL;

    return (TRUE);
}


//************************************************************************
// VirusScanGetScanContext()
//
// This routine returns the hande to the AVAPI scan context used by
// VirScan.  For comaptibility, it is cast to an LPVOID.
//
//************************************************************************
// 07/22/98 DBUCHES created.
//************************************************************************
LPVOID SYM_EXPORT WINAPI VirusScanGetScanContext()
{
    return (LPVOID) hContext;
}


//************************************************************************
// VirusScanSetPath
//
// Sets the full path to the Virus Definitions file, without the filename
// This function is available to all but SYM_WIN platforms.
//
// Parameters:
//          lpPath - pointer to an ASCIIZ string containing path to
//                   VIRSCAN.DAT.
//
// Returns:
//          -
//
//************************************************************************
// 03/06/95 RAD     created.
// 05/26/97 BGERHAR Enabled for all platforms in preparation for CDefUtils
//************************************************************************

static char  gszVirusScanPath[SYM_MAX_PATH] = {EOS};
static LPSTR glpszVirusScanPath = NULL;

VOID SYM_EXPORT WINAPI VirusScanSetPath (
    LPSTR lpszFileName
    )
{
    if ( STRLEN ( lpszFileName ) < sizeof ( gszVirusScanPath ) )
    {
        STRNCPY ( gszVirusScanPath, lpszFileName, sizeof ( gszVirusScanPath ) );
        glpszVirusScanPath = gszVirusScanPath;
    }

    NAVSetDefsPath(lpszFileName);
}


//************************************************************************
// VirScanGetPath
//
//  Returns the full path of the Virus Definitions file, without the filename
//  Note that it is dependent on where the VIRSCAN.DLL is located.
//
//  ANSI/OEM mess: This function seems to be called with file api's in
//  both ANSI and OEM modes. The string returned must be in the corresponding
//  character set.
//  GetModuleFileName() only returns LFN's. Depending on the mode of the
//  file api's and whether the filename has non-translatable characters,
//  we may get an unusable path.
//  This is kluged to work for ANSI names that don't map to OEM.
//  (It's possible to fix the other case as well, but I'm assuming a normal
//  install -running in ANSI mode- was used to select the program directory)
//
// Parameters: none
//
// Returns:
//
//************************************************************************
// 04/04/93 BRAD created.
// 05/20/96 DALLEE, kluge to return valid OEM path if a valid ANSI path
//      exists.
// 05/26/97 BGERHAR Enabled glpszVirusScanPath for all platforms
//************************************************************************

VOID SYM_EXPORT WINAPI VirusScanGetPath(LPSTR lpFileName, UINT uSize)
{
    if ( glpszVirusScanPath )
        {
        STRNCPY ( lpFileName, glpszVirusScanPath, uSize );
        }
    else
        {
        lpFileName[0] = EOS;
        }
}

//************************************************************************
// VirScanGetDatFile
//
//  Returns the full path of the Virus Definitions file.
//  Note that it is dependent on where the VIRSCAN.DLL
//
// Parameters:
//      LPSTR   lpFileName              Buffer to receive the filename.
//      UINT    uSize                   Maximum length of the filename.
//
// Returns:
//
//************************************************************************
// 04/04/93 BRAD created.
//************************************************************************

#ifdef SYM_DOS
extern "C" BOOL PASCAL AreDefsWild(VOID);
#endif

VOID SYM_EXPORT WINAPI VirusScanGetDatFile(LPSTR lpFileName, UINT uSize)
{
    VirusScanGetPath(lpFileName, uSize);

#ifdef SYM_DOS

    if (AreDefsWild())
        NameAppendFile(lpFileName, "VIRWILD1.DAT");
    else
#endif
        // NOTE: Else above is attached
        NameAppendFile(lpFileName, "VIRSCAN1.DAT");
}

//************************************************************************
// VirScanGetInfFile
//
//  Returns the full path of the Virus Information file.
//  Note that it is dependent on where the VIRSCAN.DLL
//
// Parameters:
//      LPSTR   lpFileName              Buffer to receive the filename.
//      UINT    uSize                   Maximum length of the filename.
//
// Returns:
//
//************************************************************************
// 04/04/93 BRAD created.
//************************************************************************

VOID SYM_EXPORT WINAPI VirusScanGetInfFile(LPSTR lpFileName, UINT uSize)
{
    VirusScanGetPath(lpFileName, uSize);
    NameAppendFile(lpFileName, "VIRSCAN.INF");
}

//************************************************************************
// VirusCopyData()
//
// Copy the information needed to fill out a LPN30NOTES structure
// and a VirusSigIndex from a LPHVLVIRUS.
//
// Parameters:
//  hVirus;           The source of the virus informaion
//  lpuVirusSigIndex; A place to store the virus EntryNumber
//  lpNotes;          Structure to be filled with virus information.
//
// Returns:
//
//************************************************************************
// 4/23/97 JTAYLOR created.
//************************************************************************
VOID LOCAL VirusCopyData( HVLVIRUS hVirus,
                          LPUINT lpuVirusSigIndex,
                          LPN30NOTES lpNotes )
{
    // Copy the virus data
    *lpuVirusSigIndex = hVirus->uDBEntryNum;
    N30 *lpN30 = &(hVirus->stVirusInfoBlock);

    lpNotes->wVirID   = lpN30->wVirID;
    lpNotes->wType    = lpN30->wType;
    lpNotes->wControl2= lpN30->wControl2;
    lpNotes->wInfo    = lpN30->wInfo;
    lpNotes->wSize    = lpN30->wInfSize;

    STRCPY(lpNotes->lpVirName,lpN30->lpVName);
}

//************************************************************************
// VirusScanFile()
//
// Scans a given file for known viruses
//
// Parameters:
//  lpFileName;    Full path to the file to scan
//  lpNotes;       Structure to be filled with virus information.
//  bIsExecutable; Unused.
//
// Returns:
//  0 combined with cleared lpNotes = Error
//  0 = File is not infected
//  0 > File is infected, this is the virus ID
//
//************************************************************************
// 3/\93 Martin created.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanFile (LPSTR lpFileName,
                                   LPN30NOTES lpNotes,
                                   BOOL bIsExecutable)
{
    HVLVIRUS hVirus;

    VSTATUS result = VLScanFile( hContext,
                                 lpFileName,
                                 &hVirus );

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
    {
        UINT uVirusSigIndex;
        VirusCopyData( hVirus,
                       &uVirusSigIndex,
                       lpNotes );

        VLReleaseVirusHandle( hVirus );
        return(uVirusSigIndex);
    }
    else if( result != VS_OK )
    {
        MEMSET(lpNotes, EOS, sizeof(N30NOTES));
        return(0);
    }

    return(0);
}

//************************************************************************
// VirScanBoot()
//
// Scans the specified boot record.
//
// Parameters:
//    uDrive;       The Drive number to scan
//    lpNotes;      The Virus information if found
//    lpuStatus;    The completion status of the function
//
// Returns:
//    returns 0 if boot is not infected, otherwise
//    the virus ID number. The return is only valid if the
//    ulpStatus variable is 0.
//
//    ulpStatus; 0 = if everything is fine otherwise
//               ERR_VIRSCAN_READBOOT    : problem reading boot
//               ERR_VIRSCAN_DOSFREE     : couldn't free conv. mem.
//               ERR_VIRSCAN_DOSALLOC    : couldn't get conv. mem.
//               ERR_VIRSCAN_REMOTEDRIVE : can't scan network drive
//************************************************************************
// 3/\93 Martin created.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanBoot (
    UINT        uDrive,
    LPN30NOTES  lpNotes,
    LPUINT      lpuStatus
    )
{
    HVLVIRUS hVirus;
    BYTE     byDriveNumber, byPartEntry;
    LPBYTE   lpBootBuffer = NULL;         
    DISKREC  disk    = { 0 };

    *lpuStatus    = 0;

    if (VirusScanMapDriveLetter(uDrive,
                                &byDriveNumber,
                                &byPartEntry) == ERR)
    {
                                        // if we fail to get a physical
                                        // drive and partition number,
                                        // we'll attempt a logical read

        lpBootBuffer = (LPBYTE)MemAllocPtr(GHND, 8 * SECTOR_SIZE);
                             
        if (!lpBootBuffer)
            {
            *lpuStatus = ERR_VIRSCAN_DOSALLOC;
            return(0);
            }
                                        // setup a signature in our buffer

            STRCPY((LPSTR)lpBootBuffer, "SYMC");

            if (FALSE == DiskGetInfo(uDrive, &disk))
            {
                MemFreePtr(lpBootBuffer);
                *lpuStatus = ERR_VIRSCAN_READBOOT;
                return(0);
            }

            disk.dl = (BYTE) uDrive;    // NavEx will not be used nor will
            disk.sector = 0;            // viruses be repairable
            disk.sectorCount = 1;
            disk.buffer = lpBootBuffer;

            if (DiskBlockDevice(BLOCK_READ, &disk))
            {
               MemFreePtr(lpBootBuffer);
               *lpuStatus = ERR_VIRSCAN_READBOOT;
               return(0);
            }                           // Did will really read our boot
                                        // sector? SYMC should be overwritten
                                        // if so.

            if (!STRNCMP((LPSTR)lpBootBuffer, "SYMC", 4))
            {
               MemFreePtr(lpBootBuffer);
               *lpuStatus = ERR_VIRSCAN_READBOOT;
               return(0);
            }
    }

    VSTATUS result = VLScanBoot( hContext,
                                 byDriveNumber,
                                 byPartEntry,
                                 lpBootBuffer,
                                 &hVirus );
    if (lpBootBuffer)
    {
        MemFreePtr(lpBootBuffer);
    }
                                        // check to see if a virus was found
    switch ( result )
    {
        case VS_OK:

            if( hVirus != NULL )
            {
                UINT uVirusSigIndex;
                VirusCopyData( hVirus,
                               &uVirusSigIndex,
                               lpNotes );

                VLReleaseVirusHandle( hVirus );
                return(uVirusSigIndex);
            }
            break;

        case VS_NO_ACCESS:

            *lpuStatus = ERR_VIRSCAN_REMOTEDRIVE;
            break;

        case VS_MEMALLOC:

            *lpuStatus = ERR_VIRSCAN_DOSALLOC;
            break;

        default:
            *lpuStatus = ERR_VIRSCAN_READBOOT;
            break;
    }

    return(0);
}

//************************************************************************
// VirScanMasterBoot()
//
// Scans the master boot record.
//
// Parameters:
//    uPhysDriveNum;    The Drive number to scan
//    lpNotes;          The Virus information if found
//    lpuStatus;        The completion status of the function
//
// Returns:
//    returns 0 if boot is not infected, otherwise
//    the virus ID number. The return is only valid if the
//    ulpStatus variable is 0.
//
//    ulpStatus; 0 = if everything is fine otherwise
//               ERR_VIRSCAN_READMBOOT : problem reading mboot
//               ERR_VIRSCAN_DOSFREE   : couldn't free conv. mem.
//               ERR_VIRSCAN_DOSALLOC  : couldn't get conv. mem.
//************************************************************************
// 3/\93 Martin created.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanMasterBoot (
    UINT        uPhysDriveNum,
    LPN30NOTES  lpNotes,
    LPUINT      lpuStatus
    )
{
    HVLVIRUS hVirus;

    *lpuStatus = 0;

    VSTATUS result = VLScanPart( hContext,
                                 uPhysDriveNum,
                                 NULL,
                                 &hVirus );

                                        // Check to see if a virus was found
    switch ( result )
    {
        case VS_OK:
            {
                if( hVirus != NULL )
                {
                    UINT uVirusSigIndex;
                    VirusCopyData( hVirus,
                                   &uVirusSigIndex,
                                   lpNotes );

                    VLReleaseVirusHandle( hVirus );
                    return(uVirusSigIndex);
                }
                break;
            }
        case VS_NO_ACCESS:
            {
                *lpuStatus = ERR_VIRSCAN_REMOTEDRIVE;
                break;
            }
        case VS_MEMALLOC:
            {
                *lpuStatus = ERR_VIRSCAN_DOSALLOC;
                break;
            }
        default:
            {
                *lpuStatus = ERR_VIRSCAN_READMBOOT;
                break;
            }
    }

    return(0);
}

//************************************************************************
// VirusScanMemory()
//
// Scans either the first 640K or the first 1 Meg. of memory.
//
// Parameters:
//    lpNotes;          The Virus information if found
//    bScanHighMemory;  True if scan >640k; False to stop at 640k
//
// Returns:
//  uVirusID;       0 == not infected
//           non-zero == memory infected
//************************************************************************
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************
UINT SYM_EXPORT WINAPI VirusScanMemory (
     LPN30NOTES lpNotes,
     BOOL bScanHighMemory)
{
    WORD wFlags;

                                        // Convert bScanHighMemory into
                                        // the flags for VLScanMemory
    if( bScanHighMemory )
        wFlags = VL_SCANHI;
    else
        wFlags = VL_SCAN640;

    HVLVIRUS hVirus;

    VSTATUS result = VLScanMemory( hContext, wFlags, &hVirus );

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
    {
        UINT uVirusSigIndex;
        VirusCopyData( hVirus, &uVirusSigIndex, lpNotes );

        VLReleaseVirusHandle( hVirus );
        return(uVirusSigIndex);
    }

    return(0);
}

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
//      UINT    uRepairType             The type of Repair to perform:
//                                          MBR_REPAIR or BOOT_REPAIR
//
//
// Returns:
//      TRUE                            Successful repair.
//      FALSE                           Failed.
//************************************************************************
// 5/12/93 DALLEE, Function created.
// 9/16/93 DALLEE, Re-did double infection logic.  Allocations and cleanup
//          untouched.  Repair, verification, and generic repair rewritten.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanRepairBoot (
    BYTE    byDriveLetter,
    WORD    wVirID,
    UINT    uRepairType,
    LPVOID  lpvCookie
    )
{
    HVLVIRUS hVirus;

    BYTE byDriveNumber, byPartEntry;

    VSTATUS result;
                                        // Scan the right kind of Boot record
    if( uRepairType == MBR_REPAIR )
    {  
        byDriveNumber = byDriveLetter;

        result = VLScanPart( hContext,
                             byDriveNumber,
                             NULL,
                             &hVirus );
    }
    else
    {
        if (VirusScanMapDriveLetter(byDriveLetter,
                                    &byDriveNumber,
                                    &byPartEntry) == ERR)
        {
            return(FALSE);
        }

        result = VLScanBoot( hContext,
                             byDriveNumber,
                             byPartEntry,
                             NULL,
                             &hVirus );
    }

    VSTATUS RepairSuccessfull = FALSE;

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
    {
        if( VS_OK == VLRepairItem( hVirus, lpvCookie ) )
            RepairSuccessfull = TRUE;

        VLReleaseVirusHandle( hVirus );
    }

    return(RepairSuccessfull);
}

//************************************************************************
// VirScanRepairFile()
//
// Attempts to repair an infected file.
//
// Parameters:
//  lpFileName;         Full Path to the file to repair.
//  uVirusID;           The virus ID obtained from scanning.
//
// Returns:
//  FALSE; error happened
//  TRUE; we did our best
//
//************************************************************************
// 3/\93 Martin created.
// 7/12/93 DALLEE, Do only REPAIR_ITERATIONS repeated repair attempts.
//          Scan and get a new uVirusID after each repair attempt.
//          Close file opened for repairs before rescanning.
// 9/01/93 DALLEE, Set attributes to NORMAL so we can open the file R/W.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanRepairFile (LPSTR  lpFileName, 
                                            UINT   uVirusID,
                                            LPVOID lpvCookie)
{
    HVLVIRUS hVirus;

                                        // rescan the file to get the hVirus
    VSTATUS result = VLScanFile( hContext, lpFileName, &hVirus );

    VSTATUS RepairSuccessfull = FALSE;

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
    {
        if( VS_OK == VLRepairItem( hVirus, lpvCookie ) )
            RepairSuccessfull = TRUE;

        VLReleaseVirusHandle( hVirus );
    }

    return(RepairSuccessfull);
}

//************************************************************************
// VirScanDelFile()
//
// Deletes the specified file.
//
// Parameters:
//  lpFullPath;         of the file to delete
//  uWipe;              unused
//
// Returns:
//  TRUE;  everything when fine
//  FALSE; error happened
//
//************************************************************************
// 3/\93 Martin created.
// 04/21/97 Rewritten to call AVAPI 1.0 - JTAYLOR
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanDeleteFile(LPSTR lpFullPath, UINT uWipe)
{
    HVLVIRUS hVirus;

                                        // rescan the file to get the hVirus
    VSTATUS result = VLScanFile( hContext, lpFullPath, &hVirus );

    BOOL bSuccessfull = FALSE;

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
    {
        if( VS_OK == VLDeleteFile( hVirus ) )
            bSuccessfull = TRUE;

        VLReleaseVirusHandle( hVirus );
    }

    return(bSuccessfull);
}

//************************************************************************
// VirusScanMoveFile()
//
// Moves the specified file.
//
// Parameters:
//  pszNewLocation      new path name to move the file
//  pszMovedFile;       of the file to move
//  pszFileExtention    file extention for the moved file
//  pszFileName         buffer for to remember the new file name
//  uWipe;              unused
//
// Returns:
//  TRUE;  everything when fine
//  FALSE; error happened
//
//************************************************************************
// 3/97 Tchavdar created.
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanMoveFile(LPCTSTR pszNewLocation,   LPTSTR pszSourceFile, 
                                         LPCTSTR pszFileExtention, LPTSTR pszFileName, UINT uWipe)
{
    HVLVIRUS hVirus;
                                        // rescan the file to get the hVirus
    VSTATUS result = VLScanFile(hContext, pszSourceFile, &hVirus);

    BOOL bSuccessfull = FALSE;

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
    {
        if( VS_OK == VLMoveFile(hVirus, pszNewLocation, pszFileExtention, pszFileName))
            bSuccessfull = TRUE;

        VLReleaseVirusHandle( hVirus );
    }

    return(bSuccessfull);
}

//************************************************************************
// InocScanMoveFile()
//
// Moves the specified file.
//
// Parameters:
//  pszNewLocation      new path name to move the file
//  pszMovedFile;       of the file to move
//  pszFileExtention    file extention for the moved file
//  pszFileName         buffer for to remember the new file name
//  uWipe;              unused
//
// Returns:
//  TRUE;  everything when fine
//  FALSE; error happened
//
//************************************************************************
// 5/97 Tchavdar created.
//************************************************************************
BOOL SYM_EXPORT WINAPI InocScanMoveFile(LPCTSTR pszNewLocation,   LPTSTR pszSourceFile, 
                                        LPCTSTR pszFileExtention, LPTSTR pszFileName, UINT uWipe)
{
    BOOL bSuccessfull = FALSE;

                                        // Check to see if a virus was found
    if( VS_OK == VLMoveFileEx(pszSourceFile, pszNewLocation, pszFileExtention, pszFileName))
        bSuccessfull = TRUE;

    return(bSuccessfull);
}

//************************************************************************
// VirScanDriveDir()
//
//  Does a super fast findfirst findnext on the specified drive\directory
//  (plus optional filespec). For every file found, the user specified
//  callback function is called.
//
//  Examples:
//     INPUT:                              DIRECTORY            FILESPEC
//      C:\WORK (an existing directory) -> C:\WORK              *.*
//      C:\WORK (a file)                -> C:\                  WORK.
//      C:\WORK\*.C                     -> C:\WORK              *.C
//      C:\WORK\                        -> C:\WORK              *.*
//      C:\WORK\file.ext                -> C:\WORK              FILE.EXT
//
//      NOTE:  If the sub-dir flag is on, FILESPEC will be searched in
//             all sub-directories from DIRECTORY.
//
// Parameters:
//  lpfnCallBack; should be a function expecting:
//  UINT WINAPI _loadds CallBackName (LPSTR lpFileName,
//                                   DTA_ FAR *lpDta,
//                                   LPARAM lYourData)
//  NOTES:
//         1.  for DOS use the __loadds modifier for the callback funtion
//             if SYM_EXPORT doesn't take care of loading the correct DS on entry.
//
//         2.  The callback function is responsible for calling
//             the actual scanning routine (such as VirScanFile()) to
//             detect whether or not the current file is infected.
//
// Returns:
//  TRUE                                Scan went normally
//  FALSE                               Error: Could not switch to directory,
//                                          or could not allocate memory for
//                                          working buffer.
//************************************************************************
// 3/\93 Martin created.
// 8/16/93 BarryG Updated to support filespec (including wildcards)
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanDriveDir (LPSTR lpPath, FARPROC lpfnCallBack,
                                          BOOL bSubDirs, LPARAM lYourData)
{
    char szDir  [SYM_MAX_PATH + 1];
    char szFile [SYM_MAX_PATH + 1];

    // Split lpPath into Directory and Filespec components

    if ( NameIsWildcard ( lpPath ) ||
        (NameGetType    ( lpPath ) & NGT_IS_FILE) )
    {
        NameSeparatePath ( lpPath , szDir , szFile ) ;
    }
    else                            // If not really a dir, we'll catch below
    {
        STRCPY ( szDir , lpPath ) ;
        STRCPY ( szFile , SZ_STAR_STAR) ;
    }

    return VirusScanWildcard(szDir, szFile, lpfnCallBack, bSubDirs, lYourData) ;
}

//************************************************************************
// VirScanWildcard()
//
//  Does a super fast findfirst findnext on the specified path & filespec.
//  For every file found, the user specified callback function is called.
//
// Parameters:
//  lpfnCallBack; should be a function expecting:
//  UINT WINAPI _loadds CallBackName (LPSTR lpFileName,
//                                   DTA_ FAR *lpDta,
//                                   LPARAM lYourData)
//  lpPath -- Drive\directory path to start search
//  lpFilespec -- Filespec to search and match.  May contain wildcards
//  bSubDirs -- Flag indicating whether to search sub-directories
//
//  NOTES:
//         1.  for DOS use the __loadds modifier for the callback funtion
//             if SYM_EXPORT doesn't take care of loading the correct DS on entry.
//
//         2.  The callback function is responsible for calling
//             the actual scanning routine (such as VirScanFile()) to
//             detect whether or not the current file is infected.
//
// Returns:
//  TRUE                                Scan went normally
//  FALSE                               Error: Could not switch to directory,
//                                          or could not allocate memory for
//                                          working buffer.
//************************************************************************
// 8/16/93 BarryG Created.  Should replace VirusScanDriveDir
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanWildcard (LPSTR lpPath, LPSTR lpFile ,
                                       FARPROC lpfnCallBack,
                                       BOOL bSubDirs, LPARAM lYourData)
{
    char szCWD[SYM_MAX_PATH + 1];       // Current working directory //
    BYTE byDisk;
    BOOL bStatus = TRUE;

    HGLOBAL hBuffer;
    LPSTR   lpBuffer;


    if ( (hBuffer = MemAlloc (GHND, SIZE_WORK_BUFFER)) == 0 )
        return (FALSE);

    if ( (lpBuffer = (LPSTR)MemLock (hBuffer)) != NULL )
        {
        STRUPR (lpPath);

        if (lpFile == NULL || *lpFile == EOS)
            lpFile = SZ_STAR_STAR;

        // ************************************************************* //
        // Save current working drive/directory so the user should curse //
        // because I change his directory.                               //
        // ************************************************************* //

        byDisk = DiskGet();

        // ***************************************************** //
        // Now we try to switch to the specified drive/directory //
        // ***************************************************** //

        DiskSet (lpPath[0]);
        DirGet  (lpPath[0], szCWD);     // Save current dir on this drive

        // FIL4_FIND_ALL is faster but doesn't handle wildcards other than *.*

        if ( DirSet (lpPath) == ERR )
            {
            bStatus = FALSE;
            if (NameIsRoot(lpPath))
                {
                                        // Try to move up from current dir,
                                        // until we can't.
                auto        char    szDirectory[SYM_MAX_PATH+1];


                if ( DirGet(*lpPath, szDirectory) != ERR )
                    {
                    do {
                        NameStripFile(szDirectory);
                       }
                    while (DirSet(szDirectory) != ERR &&
                           !NameIsRoot(szDirectory));

                    bStatus = TRUE;
                    }
                }
            }

        if ( bStatus )
            {
/* CERTLIB4 is not kosher */
#ifdef SYM_DOS
            if ( STRCMP (lpFile, SZ_STAR_STAR) == 0 )
                FIL4_FIND_ALL (lpfnCallBack, lYourData, bSubDirs, lpBuffer);
            else
                FIL4_FIND_WILD (lpFile, lpfnCallBack, lYourData, bSubDirs, lpBuffer);
#endif
            bStatus = FALSE;
            }

        // *********************************************************** //
        // We switch back to the drive directory where we started from //
        // *********************************************************** //

        DirSet(szCWD);                  // Restore this drive's directory
        DiskSet(byDisk);                // Restore original drive

        MemRelease( hBuffer );
        }
    else                                // Failed to lock memory
        {
        bStatus = FALSE;
        }

    MemFree(hBuffer);

    return (bStatus);
} // End VirScanDriveDir()


//************************************************************************
// GetScanData()
//
// Obtains scan information from the appropriate .DAT file.
// If the file does not exist or the date differs from the virus def file,
// it is recreated from the virus def file.
//
// Parameters:
//      UINT    uTypeData;  type of scan information requested
//                          SCANDATA_FILE; file specific info
//                          SCANDATA_BOOT: boot specific info
//                          SCANDATA_MEM: memory specific info
// Returns:
//      Handle to a block of memory which should be freed
//      when not needed anymore. A NULL is returned if error.
//
//************************************************************************
// 7/28/93 CAREY created
// 04/21/97 Stubbed during conversion to AVAPI 1.0 - JTAYLOR
//************************************************************************

HGLOBAL SYM_EXPORT WINAPI GetScanData (UINT uTypeData, BOOL FAR *bReCreated)
{
    return(NULL);
}

//************************************************************************
// FreeScanFileData()
//
// Frees memory containing alg sigs.
//
// Parameters:
//      LPSTR   lpMem;      // Pointer to memory block containing
//                          //  pointer and handle information to
//                          //  memory blocks containing ALG sigs
// Comments:
//      Call this function to release the memory allocated
//      for algorithmic signatures.
//
//************************************************************************
// 9/7/95 DARREN created
// 04/21/97 Stubbed during conversion to AVAPI 1.0 - JTAYLOR
//************************************************************************

VOID SYM_EXPORT WINAPI FreeScanFileData(LPSTR lpMem)
{
        ;
}

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

// 04/21/97 Stubbed during conversion to AVAPI 1.0 - JTAYLOR
*******************************************************************************/
BOOL SYM_EXPORT WINAPI VirusScanCanAccessDisk (
    BYTE    byDriveLetter,
    UINT    uRequestedAccess )
{
    return (FALSE);
}

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
// 04/21/97 Rewritten during conversion to AVAPI 1.0 - JTAYLOR
//************************************************************************

BYTE SYM_EXPORT WINAPI VirusScanMapLogicalToPhysical (
    BYTE byDriveLetter                  // [in]  Drive letter to convert
)
{
    BYTE byDriveNumber, byPartEntry;

    VirusScanMapDriveLetter(byDriveLetter,
                            &byDriveNumber,
                            &byPartEntry);
    return(byDriveNumber);
}

//************************************************************************
// VirusScanDatFileGetComment
//
// This routine reads in the virus information from the .inf file.
//
// Parameters:
//    LPSTR   lpFile,                     // [in] .INF file to pull out from
//    UINT    uVirusID,                   // [in] virus ID to query
//    LPSTR   lpAlias,                    // [out] alias info placed here
//    LPSTR   lpComment                   // [out] comment
//
// Returns:
//      True if success;
//      False if fail;
//************************************************************************
// 05/22/96 Created by Jim Worden
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanDatFileGetComment (
    LPSTR   lpFile,                     // [in] .INF file to pull out from
    UINT    uVirusID,                   // [in] virus ID to query
    LPSTR   lpAlias,                    // [out] alias info placed here
    LPSTR   lpComment )                 // [out] comment
{
   auto     DWORD    dwOffset;
   auto     LPSTR    lpMem;
   auto     BOOL     bStatus = TRUE;
   auto     UINT     hFile;
   auto     INF_HEADER  inf;

   // ************************************************************** //
   // Now try to open the complemetary file where we expect it to be //
   // ************************************************************** //

   if ((hFile = FileOpen(lpFile, READ_ONLY_FILE)) != HFILE_ERROR)
      {
      // ******************************************************** //
      // Allocate a chunk of memory big enough to store a comment //
      // and an alias string.                                     //
      // ******************************************************** //

      lpMem = (LPSTR)MemAllocPtr (GHND, SIZE_ALIAS + SIZE_COMMENT + 10);

      LPSTR lpFreeThis = lpMem;

      if ( lpMem )
         {
         FileSeek (hFile, SIZE_COPYRIGHT, SEEK_SET);
         FileRead (hFile,&inf, sizeof(INF_HEADER));

         if (inf._i_version != 0)
             {
             bStatus = FALSE;
             goto freeMem;              // Need to free memory/close file
             }

         if (uVirusID > inf._i_highest_vid)
             uVirusID = 0;


         FileSeek (hFile, uVirusID * inf._i_size, SEEK_CUR);
         FileRead (hFile, lpMem, inf._i_size);
         if (4 == inf._i_size)
             {
             dwOffset = *(DWORD FAR *)lpMem;
             }
         else
             {
             dwOffset = *(UINT FAR *)lpMem;
             }
         dwOffset = (dwOffset * inf._i_multiplier) + inf._i_no_info;

         FileSeek (hFile, dwOffset, SEEK_SET);

         FileRead (hFile, lpMem, SIZE_ALIAS + SIZE_COMMENT + 2);

         // **************************************************** //
         // First comes the alias field. If its length is longer //
         // than the maximum possible alias field then dump it.  //
         // **************************************************** //
         if (STRLEN(lpMem) <= SIZE_ALIAS)
            {
           #ifdef SYM_WIN
            OemToAnsi (lpMem, lpAlias);
           #else
            STRCPY(lpAlias, lpMem);
           #endif

            lpMem += (STRLEN(lpMem) + 1);

            // *********************************** //
            // The same goes for the comment field //
            // *********************************** //
            if (STRLEN(lpMem) <= SIZE_COMMENT)
                {
               #ifdef SYM_WIN
                OemToAnsi (lpMem, lpComment);
               #else
                STRCPY(lpAlias, lpMem);
               #endif
                }
            else
                {
                lpComment[0] = EOS;
                bStatus = FALSE;
                }
            }
         else
            {
            lpAlias[0] = EOS;
            lpComment[0] = EOS;
            bStatus = FALSE;
            }
freeMem:
         MemFreePtr (lpFreeThis);
         }

      FileClose (hFile);
      }
   else
      bStatus = FALSE;

   return (bStatus);
}

//************************************************************************
// VirusScanMapDriveLetter
//
// This routine maps a logical drive letter to its physical drive number
// and partition entry.
//
// Parameters:
//      BYTE byDriveLetter,             // [in]  Drive letter to map
//      LPBYTE lpbyDriveNumber,         // [out] Drive number
//      LPBYTE byPartEntry,             // [out] Partition Entry
//
// Returns:
//      NOERR if succussful, otherwise ERR
//************************************************************************
// 04/24/97 Created by MKEATIN
//************************************************************************

STATUS LOCAL VirusScanMapDriveLetter (
    BYTE   byDriveLetter,
    LPBYTE lpbyDriveNumber,
    LPBYTE lpbyPartEntry
)
{
    ABSDISKREC     stAbsDisk = {0};
    INT13REC       stInt13Rec;
    LPPARTENTRYREC lpPartEntryRec;
    BYTE           byPartEntry;
    DWORD          dwLinearBootSector;

                                        // Initialize memory.
    memset (&stInt13Rec, 0, sizeof (stInt13Rec));


    if (HWIsNEC())                               
    {                                   // this is a NEC
        if (DiskIsFixed(byDriveLetter)) 
        {
            return (ERR);               // only scan floppy boots on NEC
        }
        *lpbyDriveNumber = byDriveLetter - 'A';
        *lpbyPartEntry = 0;
        return (NOERR);
    }
                                        // for drive C, scan the active 
                                        // partition on drive 0x80 if
                                        // possible
    if (byDriveLetter == 'C')
    {
        stAbsDisk.dn = 0x80;

        DiskGetPhysicalInfo(&stAbsDisk);

        stAbsDisk.dwHead     = 0;
        stAbsDisk.dwTrack    = 0;
        stAbsDisk.dwSector   = 1;
        stAbsDisk.numSectors = 1;
        stAbsDisk.buffer = (HPBYTE)MemAllocPtr(GHND, 8 * PHYSICAL_SECTOR_SIZE);

        if (!stAbsDisk.buffer)
        {
            return (ERR);
        }

        if (!DiskAbsOperation(READ_COMMAND, &stAbsDisk))
        {
            for (byPartEntry = 0; byPartEntry < 4; byPartEntry++)
            {
                lpPartEntryRec =    
                    (LPPARTENTRYREC)(stAbsDisk.buffer + PART_TABLE_OFFSET) +        
                    byPartEntry;    

                                            // Is this an unused partition entry?    

                if (lpPartEntryRec->system == SYSTEM_NOT_USED)    
                    continue;    
                                            // for drive 'C' we scan the    
                                            // active partition only    

                if (lpPartEntryRec->bootable)    
                {    
                    *lpbyDriveNumber = stAbsDisk.dn;    
                    *lpbyPartEntry   = byPartEntry;       

                    MemFreePtr(stAbsDisk.buffer);    
                    return (NOERR);    
                }    
            }
        }
        MemFreePtr(stAbsDisk.buffer);    
    }
                                        // Okay, this is not drive 'C'  - or
                                        // it is drive 'C' but there is no
                                        // active partition on drive 0x80

#ifdef SYM_WIN32

    if(SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
    {
        OSVERSIONINFO   OSVersionInfo      ;
        BOOL            bOsVersionInfoValid;

        ZeroMemory(&OSVersionInfo, sizeof(OSVersionInfo));
        OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        bOsVersionInfoValid = GetVersionEx((LPOSVERSIONINFO)&OSVersionInfo);
        if (TRUE == bOsVersionInfoValid)
        {
            if (VER_PLATFORM_WIN32_NT == OSVersionInfo.dwPlatformId)
            {

                if (OSVersionInfo.dwMajorVersion >= 5) //for Windows 2000 and beyond
                {

                    char                         szDrive[3]            = "?:"     ;
                    char                         szDevice[SYM_MAX_PATH]= "\\\\.\\";
                    static  char                 szFloppy [] = "\\Device\\Floppy" ;

                    HANDLE                       DeviceHandle                     ;
                    DWORD                        dwBytesReturned                  ;
                    STORAGE_DEVICE_NUMBER        StorageDeviceInfo                ;
                    PARTITION_INFORMATION_32BYTE PartitionInfo                    ;
                    DISK_GEOMETRY                DiskGeometryInfo                 ; 

                    DWORD                        dwSectorOffset                   ;
                    DWORD                        dwTrackOffset                    ;
                    DWORD                        dwDeviceQueryStatus              ;
                    DWORD                        dwDrive                          ;


                    szDrive[0] = byDriveLetter;
                    STRCAT(szDevice, szDrive);

                    DeviceHandle =
                        CreateFile(szDevice,
                                   GENERIC_READ,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL
                                  );

                    //
                    // The following logic handles the case where the
                    // floppy drive needs to be handled under Windows 2000.
                    //
                    dwDeviceQueryStatus = QueryDosDevice(szDrive, szDevice, sizeof(szDevice));

                    if ( (0 != dwDeviceQueryStatus                           ) &&
                         (0 == STRNICMP(szDevice, szFloppy, STRLEN(szFloppy)))
                       )
                    {
                        ConvertStringToLong(&dwDrive, szDevice + STRLEN(szFloppy));

                        stInt13Rec.generated = TRUE;
                        stInt13Rec.dwTrack   = 0;
                        stInt13Rec.dwHead    = 0;
                        stInt13Rec.dwSector  = 1;
                        stInt13Rec.dn        = (BYTE)dwDrive;
                    }//end if (0 != dwDeviceQueryStatus && (0==STRNICMP...)

                    //
                    // The following logic handles the case where the
                    // DeviceHandle is valid and the device information
                    // needs to be obtained for basic disks under
                    // Windows 2000.
                    //
                    // These IOCTL calls will initially be made in
                    // the case where byDriveLetter refers to a
                    // floppy drive, but they will all fail.
                    // Therefore this "if" statement is essentially
                    // invalid for floppy drives.
                    //
                    if ( ( INVALID_HANDLE_VALUE != DeviceHandle ) &&
                         ( TRUE == DeviceIoControl(DeviceHandle,
                                     IOCTL_STORAGE_GET_DEVICE_NUMBER,
                                     NULL, 0,
                                     &StorageDeviceInfo, sizeof(StorageDeviceInfo),
                                     &dwBytesReturned, NULL)) &&
                         ( TRUE == DeviceIoControl(DeviceHandle,
                                     IOCTL_DISK_GET_PARTITION_INFO,
                                     NULL, 0,
                                     &PartitionInfo, sizeof(PartitionInfo),
                                     &dwBytesReturned, NULL)) &&
                         ( TRUE == DeviceIoControl(DeviceHandle,
                                     IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                     NULL, 0,
                                     &DiskGeometryInfo, sizeof(DiskGeometryInfo),
                                     &dwBytesReturned, NULL)) )
                    {

                        dwSectorOffset = PartitionInfo.pi.StartingOffset.LowPart
                                         / DiskGeometryInfo.BytesPerSector;

                        dwTrackOffset = dwSectorOffset
                                        / DiskGeometryInfo.SectorsPerTrack;

                                        // Compute location of sector 0.
                        stInt13Rec.generated = TRUE;

                        stInt13Rec.dwTrack =
                            (WORD)(dwTrackOffset / DiskGeometryInfo.TracksPerCylinder);

                        stInt13Rec.dwHead =
                            (BYTE)(dwTrackOffset % DiskGeometryInfo.TracksPerCylinder);

                        stInt13Rec.dwSector =
                            1 + (BYTE)(dwSectorOffset % DiskGeometryInfo.SectorsPerTrack);

                        stInt13Rec.dn = (BYTE)(StorageDeviceInfo.DeviceNumber + FIRST_HD);

                    }//end if

                    if ( INVALID_HANDLE_VALUE != DeviceHandle )
                        CloseHandle(DeviceHandle);
                }//end if (OSVersionInfo.dwMajorVersion >= 5)

                else//do the "Windows NT 4.0 and before" thing
                {

                    static  char    szHard   [] = "\\Device\\Harddisk";
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

                    szDrive[0] = byDriveLetter;
                    STRCAT(szDevice, szDrive);

                                                    // First open handle to drive.
                    hDrive = CreateFile(szDevice,
                                        GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
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

                                stInt13Rec.generated = TRUE;
                                stInt13Rec.dwTrack   = 0;
                                stInt13Rec.dwHead    = 0;
                                stInt13Rec.dwSector  = 1;
                                stInt13Rec.dn        = (BYTE)dwDrive;
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
                                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                        NULL,
                                                        OPEN_EXISTING,
                                                        FILE_ATTRIBUTE_NORMAL,
                                                        NULL );

                                if ( INVALID_HANDLE_VALUE != hPhysDrive ) 
                                    {
                                    if ( ( TRUE == DeviceIoControl(hDrive,
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
                                                    // Get drive number.
                                        ConvertStringToLong(&dwDrive, szDevice + STRLEN(szHard));

                                        dwSectorOffset = rPartInfo.pi.StartingOffset.LowPart
                                                         / rDiskGeometry.BytesPerSector;

                                        dwTrackOffset = dwSectorOffset
                                                        / rDiskGeometry.SectorsPerTrack;

                                                    // Compute location of sector 0.
                                        stInt13Rec.generated = TRUE;

                                        stInt13Rec.dwTrack =
                                            (WORD)(dwTrackOffset / rDiskGeometry.TracksPerCylinder);

                                        stInt13Rec.dwHead =
                                            (BYTE)(dwTrackOffset % rDiskGeometry.TracksPerCylinder);

                                        stInt13Rec.dwSector =
                                            1 + (BYTE)(dwSectorOffset % rDiskGeometry.SectorsPerTrack);

                                        stInt13Rec.dn = (BYTE)(dwDrive + FIRST_HD);
                                        }

                                    CloseHandle ( hPhysDrive );
                                }//end if (INVALID_HANDLE != hPhysDrive,
                                 //                          IOCTL_GET_PARTITION_INFO and
                                 //                          IOCTL_GET_DRIVE_GEOMETRY
                            }
                            else // Not szFloppy or szHarddisk
                            {
                                CloseHandle(hDrive);
                                return (ERR);
                            }
                        }//end if (0 != QueryDosDevice(szDrive, szDevice, sizeof(szDevice)))
                        else
                        {
                            CloseHandle(hDrive);
                            return (ERR);
                        }
                        CloseHandle(hDrive);
                    }
                    else
                    {
                        return (ERR);
                    }
                }//end else NT 4.0 Version

            }//end if (VER_PLATFORM_WIN32_NT == OSVersionInfo.dwPlatformId)
        }//end if (TRUE == bOsVersionInfoValid)
    }//end if(SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
    else
#endif  // SYM_WIN32

    DiskMapLogToPhyParams( 0,
                           byDriveLetter,
                           &stInt13Rec );

    if (!stInt13Rec.generated)          // do we have physical info?
    {
        return (ERR);
    }

    BOOL bPartitioned;

    if (NAVLinearIsPartitionedMedia((LPVOID)stInt13Rec.dn, &bPartitioned) != CBSTATUS_OK)
    {
        return (ERR);
    }

    if (!bPartitioned)
    {
        *lpbyDriveNumber = stInt13Rec.dn;
        *lpbyPartEntry   = 0;
        return (NOERR);
    }

    if (HWIsNEC())
    {                                   // no hard drive boot scanning on NEC
        return (ERR);
    }

    MEMSET(&stAbsDisk, 0, sizeof(stAbsDisk));

    stAbsDisk.dn = stInt13Rec.dn;

    DiskGetPhysicalInfo(&stAbsDisk);

    stAbsDisk.dwHead   = 0;
    stAbsDisk.dwTrack  = 0;
    stAbsDisk.dwSector = 1;
    stAbsDisk.numSectors = 1;
    stAbsDisk.buffer = (HPBYTE)MemAllocPtr(GHND, 8 * PHYSICAL_SECTOR_SIZE);

    if ( !stAbsDisk.buffer )
    {
        return (ERR);
    }
                                        // read the MBR

   // 6/16/1999: Mike Keating: This was crashing on Magento-Optical drives.
   // Possibly because the buffer is on aligned on a 4k boundry. Calling
   // NAVEXSDiskReadPhysical which does to 4k alignment.
   // if (DiskAbsOperation(READ_COMMAND, &stAbsDisk))
   // {
   //     MemFreePtr(stAbsDisk.buffer);
   //     return (ERR);
   // }

    if (NAVEXSDiskReadPhysical(stAbsDisk.buffer, 1, 0, 1, 0, stInt13Rec.dn))
    {
        MemFreePtr(stAbsDisk.buffer);
        return (ERR);
    }
                                        // Get the linear offset of our boot record
                                        
    dwLinearBootSector = ConvertPhySectorToLong(&stAbsDisk, 
                                                stInt13Rec.dwHead,
                                                stInt13Rec.dwTrack,
                                                stInt13Rec.dwSector);


                                        // find the partition entry

    for (byPartEntry = 0; byPartEntry < 4; byPartEntry++)
    {
        lpPartEntryRec =        
            (LPPARTENTRYREC)(stAbsDisk.buffer + PART_TABLE_OFFSET) +    
            byPartEntry;    

                                        // Is this an unused partition entry?    

        if (lpPartEntryRec->system == SYSTEM_NOT_USED)    
            continue;    

                                        // Is our boot sector within this
                                        // partition?

        if (dwLinearBootSector >= lpPartEntryRec->relativeSectors &&    
            dwLinearBootSector < (lpPartEntryRec->relativeSectors +    
                                  lpPartEntryRec->numSectors))    
        {    
            *lpbyDriveNumber = stInt13Rec.dn;    
            *lpbyPartEntry   = byPartEntry;    // found it!    

            MemFreePtr(stAbsDisk.buffer);    
            return (NOERR);    
        }    
    }

    MemFreePtr(stAbsDisk.buffer);
    return (ERR);
}

//************************************************************************
// VirusScanCanDeleteFile()
//
// Checks if the specified file.can be deleted
//
// Parameters:
//  lpFileName pointer to the file name
//
// Returns:
//  TRUE;  the file can be deleted
//  FALSE; the file cannot be deleted
//
//************************************************************************
// 5/6/98 TCashin created.
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanCanDeleteFile (LPSTR lpFileName)
{
    HVLVIRUS hVirus;
    BOOL bSuccessfull = FALSE;

                                        // rescan the file to get the hVirus
    VSTATUS result = VLScanFile(hContext, lpFileName, &hVirus);

                                        // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
    {
        if( TRUE == VLCanDeleteItem(hVirus))
            bSuccessfull = TRUE;
        else
            bSuccessfull = FALSE;

        VLReleaseVirusHandle( hVirus );
    }

    return(bSuccessfull);
}
