// Copyright 1993 Symantec, Peter Norton Group
//************************************************************************
// VIRSCAN.H
//
// $Header:   S:/INCLUDE/VCS/virscan.h_v   1.61   28 Aug 1998 18:03:10   JBRENNA  $
//
// Description:
//
//   This header declares items needed to scan for viruses.
//
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/virscan.h_v  $
// 
//    Rev 1.61   28 Aug 1998 18:03:10   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.60.1.2   22 Jul 1998 16:24:40   DBuches
//    Added VirusScanGetScanContext().  Changed return type to LPVOID.
// 2. Rev 1.60.1.1   22 Jul 1998 16:03:38   DBuches
//    Added VirusScanGetScanContext().
// 
//    Rev 1.60   12 May 1998 13:17:54   tcashin
// Added VirusScanCanDeleteFile().
// 
//    Rev 1.59   09 Sep 1997 13:01:58   MKEATIN
// Added the param, lpszNavexInfFile, to VirusScanInit().
// 
//    Rev 1.58   17 Jun 1997 14:58:14   MKEATIN
// Changed the paramter for VirusScanInit() to take an LPAVGLOBALCALLBACKS.
// 
//    Rev 1.57   31 May 1997 16:08:46   MKEATIN
// Added the lpvCookie Parameter to the repair functions
// 
//    Rev 1.56   28 May 1997 08:04:52   TIVANOV
// rename the input params for VirusScanMoveFile InocScanMoveFile
// 
//    Rev 1.55   26 May 1997 15:05:42   BGERHAR
// Enabled VirusScanSetPath() for all platforms, removed VirusScanGetBootFile()
// 
//    Rev 1.54   13 May 1997 13:51:42   TIVANOV
// function prototype for InocScanMoveFile
// 
//    Rev 1.53   08 May 1997 11:42:30   TIVANOV
// new param for VirScanMoveFile - buff to store the new filename
// 
//    Rev 1.52   05 May 1997 14:49:52   TIVANOV
// change VirusScanMoveFile to except a file externtion as well
//
//    Rev 1.51   01 May 1997 08:00:44   TIVANOV
// a prototype for VirusScanMoveFile function
//
//    Rev 1.50   28 Apr 1997 19:28:46   JBRENNA
// Add #include for N30TYPE.H for the LPN30* structure pointer definitions.
//
//    Rev 1.49   28 Apr 1997 13:37:38   MKEATIN
// Removed the include file "ctsn.h".
//
//    Rev 1.48   27 Jun 1996 21:56:46   DALLEE
// LuigiPlus port:
// Added VirusScanMapLogicalToPhysical() prototype.
//
//    Rev 1.47   01 Apr 1996 17:57:38   TCASHIN
// Check if we should not use PAM (like for NAVOEM).
//
//    Rev 1.46   16 Feb 1996 12:12:56   CNACHEN
// Added PAM support (#define USE_PAM)
//
//    Rev 1.45   12 Jan 1996 07:12:24   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.43.1.5   14 Nov 1995 18:26:06   MKEATIN
// Added prototype for NavExInit()
//
//    Rev 1.43.1.4   31 Oct 1995 13:05:14   CNACHEN
// Added (IFDEF'd out PAM definitions)...
//
//    Rev 1.43.1.3   24 Oct 1995 17:15:50   CNACHEN
// VirusScanMasterBoot now has a uPhysDriveNum Argument...
//
//    Rev 1.43.1.2   12 Sep 1995 16:25:04   DCHI
// Added FreeScanFileData() prototype.
//
//    Rev 1.43.1.1   16 Jul 1995 00:20:04   SZIADEH
// added defines for VIRSCAN_TSRFILE and VIRSCAN_TSRBOOT.
//
//    Rev 1.43   27 Apr 1995 12:16:46   MFALLEN
// Made query access function global.
//
//    Rev 1.42   27 Apr 1995 11:50:40   MFALLEN
// Added drive lock checking to boot scanning.
//
//    Rev 1.41   06 Mar 1995 14:40:44   RSTANEV
// Added VirusScanSetPath() prototype.
//
//    Rev 1.40   07 Feb 1995 14:46:02   SZIADEH
// change the type from WORD to UINT on boot and mbr scan functions.
//
//    Rev 1.39   27 Dec 1994 19:16:02   DALLEE
// Use CTSN.H for all platforms (W16 and DOS were using CTS4.H).
//
//    Rev 1.38   19 Dec 1994 21:51:46   DALLEE
// Added HGLOBAL hCallbacks to VIRSCAN struct to hold CALLBACKREV1 struct
// for CERTLIBN.
//
//    Rev 1.37   06 Dec 1994 12:03:42   DALLEE
// Added VIRUSLIST_DONT_DISPLAY_XXX #defines.
//
//    Rev 1.36   01 Dec 1994 08:45:12   MFALLEN
// VirusScanDatFileGetComment
//
//    Rev 1.35   29 Nov 1994 14:55:30   MFALLEN
// Fixed for W16
//
//    Rev 1.34   18 Aug 1994 13:14:38   SZIADEH
// commented out cts4.h, added ctsn.h
//
//    Rev 1.33   06 Jun 1994 17:57:22   MFALLEN
// Added file header version information
//
//    Rev 1.32   26 Apr 1994 16:41:52   BRAD
// Removed PVCS Logs
//************************************************************************



#ifndef _VIRSCAN_H
#define _VIRSCAN_H

#define USE_PAM

//////////////////////////////////////////////////////////////////////////////
// Products that don't use PAM
//////////////////////////////////////////////////////////////////////////////
#if defined(NAVOEM)
#undef USE_PAM
#endif

#include "n30type.h"
#include "virusinf.h"
#include "navcb.h"

//////////////////////////////////////////////////////////////////////////////
// PAM
//////////////////////////////////////////////////////////////////////////////
#ifdef USE_PAM
#include "pamapi.h"
#endif
//////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------
// NOTE:  If the stucture VIRSCANDATHEADER EVER changes, please
//        update the same structure in CTS4.INC.  We are using this
//        in the function SCN4_DELETE (SCN_DEL.ASM) to update
//        the virus count.
//-----------------------------------------------------------------------

#define VIRDAT_NAV30_VERSION   3L
#define VIRDAT_NAV40_VERSION   4L

                                        // Range of virus ID's not to display
                                        // in virus list (in case we add a code
                                        // definition, we don't want it to
                                        // appear in older versions).
// NAV 3.0.4 Don't Display range is 0xF00 - 0xFFF
// NAV 3.0.5 Don't Display range is 0xF80 - 0xFFF

#define VIRUSLIST_DONT_DISPLAY_LOW      0xF80
#define VIRUSLIST_DONT_DISPLAY_HIGH     0xFFF

                                        //-------------------------------
                                        // Header for VIRSCAN.DAT file
                                        //-------------------------------
typedef struct tagVIRSCANDATHEADER
    {
    char        szSignature[16];        // Signature "VIRSCAN.DAT"
    DWORD       dwVersion;              // Numeric version number
    BYTE        byDay;                  // Day of this version
    BYTE        byMonth;                // Month of this version
    WORD        wYear;                  // Year of this version
    WORD        wNumSigs;               // Number of sigs total
    WORD        wNumALGs;               // Number of ALG sigs
    WORD        wNumBOOs;               // Number of BOO sigs
    WORD        wNumCRCs;               // Number of CRC sigs
    WORD        wNumREMs;               // Number of REMed out sigs
    WORD        wHeaderCheckSum;        // check sum of header(see note below)
    WORD        wTotalVir;              // total # of viruses we "detect"
    BYTE        byFiller[218];          // Filler to add extra 'stuff'
    } VIRSCANDATHEADER;

// Note: When calculating header checksum values, be sure to remove the
//        original checksum value and set wHeaderCheckSum to NULL before
//         doing sum. See sample 'C' code in
//          Vishnu/Scan30C/inscanf.c/CheckCheckSum(). ==dad
//

                                        // App needs to declare a buffer of
                                        // this size
#define SIZE_WORK_BUFFER                (4096)
#define NOERR_DAT_CHANGED               2
                                        // Types of scans
#define VIRSCAN_FILE                    (1)
#define VIRSCAN_MEMORY                  (2)
#define VIRSCAN_BOOT                    (3)
#define VIRSCAN_TSRFILE                 (4)
#define VIRSCAN_TSRBOOT                 (5)

                                        // ERROR MESSAGE ID NUMBERS //
#define ERR_VIRSCAN_READMBOOT           (1)
#define ERR_VIRSCAN_DOSFREE             (2)
#define ERR_VIRSCAN_DOSALLOC            (3)
#define ERR_VIRSCAN_READBOOT            (4)
#define ERR_VIRSCAN_REMOTEDRIVE         (5)
#define ERR_VIRSCAN_DISKLOCKED          (6)

                                        // flags for MBR or BOOT repair
#define MBR_REPAIR                      (6)
#define BOOT_REPAIR                     (9)


// ---------------------------------------------------------------------------
//  Defines used with VirusScanCanAccessDisk to query for required disk access
//  permission before attempting to scan boot records.
// ---------------------------------------------------------------------------

#define DISK_READ_WRITE     1
#define DISK_READ_ONLY      2

// ---------------------------------------------------------------------------

typedef struct tagVIRSCAN
{
   HGLOBAL hWorkMem;
   LPSTR   lpWorkMem;

   HGLOBAL hFileScanData;               // File Scan Specific data
   LPSTR   lpFileScanData;

   HGLOBAL hMemoryScanData;             // Memory Scan Specific data
   LPSTR   lpMemoryScanData;

   HGLOBAL hBootScanData;               // Boot Scan Specific data
   LPSTR   lpBootScanData;

   HGLOBAL hCallbacks;                  // Callbacks for CERTLIBN.
   LPSTR   lpCallbacks;                 // Actually LPCALLBACKREV1, but no need
                                        // to include "callback.h" everywhere.

//////////////////////////////////////////////////////////////////////////////
// PAM
//////////////////////////////////////////////////////////////////////////////

#ifdef USE_PAM
   PAMGHANDLE   hGPAM;                  // necessary for proper PAM operation
   PAMLHANDLE   hLPAM;
#endif

//////////////////////////////////////////////////////////////////////////////

   char    szVirscanFile[SYM_MAX_PATH + 1];
} VIRSCAN;


#ifdef __cplusplus
extern "C" {
#endif

//  ********************************************************************** //
//  *****************      Function Prototypes     *********************** //
//  ********************************************************************** //

UINT SYM_EXPORT WINAPI VirusScanInit (
    LPAVGLOBALCALLBACKS lpGlobalCallBacks,
    LPSTR               lpszNavexInfFile );

BOOL SYM_EXPORT WINAPI VirusScanEnd (
    VOID );

#ifdef SYM_DOS

UINT SYM_EXPORT WINAPI NavExInit (
    VOID );

#endif

VOID SYM_EXPORT WINAPI VirusScanSetPath (
    LPSTR lpszFileName
    );

VOID SYM_EXPORT WINAPI VirusScanGetPath (
    LPSTR   lpFileName,
    UINT    uSize );

VOID SYM_EXPORT WINAPI VirusScanGetDatFile (
    LPSTR   lpFileName,
    UINT    uSize );

VOID SYM_EXPORT WINAPI VirusScanGetInfFile (
    LPSTR   lpFileName,
    UINT    uSize );

UINT SYM_EXPORT WINAPI VirusScanFile (
    LPSTR   lpFileName,
    LPN30NOTES lpNotes,
    BOOL       bIsExecutable );

UINT SYM_EXPORT WINAPI VirusScanBoot (
    UINT    uDrive,
    LPN30NOTES lpNotes,
    LPUINT  uStatus );

UINT SYM_EXPORT WINAPI VirusScanMasterBoot (
    UINT uPhysDriveNum,
    LPN30NOTES lpNotes,
    LPUINT  ulpStatus );

UINT SYM_EXPORT WINAPI VirusScanMemory (
    LPN30NOTES lpNotes,
    BOOL    bScanHighMemory );

BOOL SYM_EXPORT WINAPI VirusScanRepairBoot (
    BYTE    byDriveLetter,
    WORD    wVirID,
    UINT    uRepairType,
    LPVOID  lpvCookie);

BOOL SYM_EXPORT WINAPI VirusScanRepairFile (
    LPSTR   lpFileName,
    UINT    uVirusID, 
    LPVOID  lpvCookie);

BOOL SYM_EXPORT WINAPI VirusScanDeleteFile (
    LPSTR,
    UINT );

BOOL SYM_EXPORT WINAPI VirusScanMoveFile(
    LPCTSTR pszNewLocation,
    LPTSTR  pszSourceFile,
    LPCTSTR pszFileExtention,
    LPTSTR  pszFileName,
    UINT    uWipe);

BOOL SYM_EXPORT WINAPI InocScanMoveFile(
    LPCTSTR pszNewLocation,
    LPTSTR  pszSourceFile,
    LPCTSTR pszFileExtention,
    LPTSTR  pszFileName,
    UINT    uWipe);

BOOL SYM_EXPORT WINAPI VirusScanDriveDir (
    LPSTR   lpDir,
    FARPROC lpfnCallBack,
    BOOL    bSubDirs,
    LPARAM  lYourData );

BOOL SYM_EXPORT WINAPI VirusScanWildcard (
    LPSTR   lpPath,
    LPSTR   lpFile,
    FARPROC lpfnCallBack,
    BOOL    bSubDirs,
    LPARAM  lYourData );

HGLOBAL SYM_EXPORT WINAPI GetScanData (
    UINT    uTypeData,
    LPBOOL  bBool);

VOID SYM_EXPORT WINAPI FreeScanFileData (
    LPSTR lpMem);

BOOL WINAPI CreateInScan (
    LPTSTR  lpVirScanPath );

BOOL SYM_EXPORT WINAPI VirusScanDatFileGetComment (
    LPSTR   lpFile,                     // [in] .INF file to pull out from
    UINT    uVirusID,                   // [in] virus ID to query
    LPSTR   lpAlias,                    // [out] alias info placed here
    LPSTR   lpComment );                // [out] comment

BOOL SYM_EXPORT WINAPI VirusScanCanAccessDisk (// Check if disk can be accessed
    BYTE    byDriveLetter,              // [in] drive letter to verify
    UINT    uRequestedAccess            // [in] requested access right
    );


BYTE SYM_EXPORT WINAPI VirusScanMapLogicalToPhysical (
    BYTE byPhysicalDrive                // [out] Physical drive number
    );

BOOL SYM_EXPORT WINAPI VirusScanCanDeleteFile (
    LPSTR   lpFileName
    );


LPVOID SYM_EXPORT WINAPI VirusScanGetScanContext();

//  ********************************************************************** //



#if defined( __cplusplus )
}
#endif  // #if defined( __cplusplus )

#endif
