// Copyright 1995-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// Description:
//
//      This file defines functions to access the Norton AntiVirus
//      Scanning engine.
//
//************************************************************************

#ifndef __NAVSDK_H
#define __NAVSDK_H


#if !defined(_NAVSDK_)
    #define NAVSDKAPI DECLSPEC_IMPORT
#else
    #define NAVSDKAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

//***************************************************************************
// Definitions
//***************************************************************************

//
// Interface version
//
#define VIRLIB_INTERFACE_VERSION    0x000000002

//
// Buffer Sizes
//
#define VIRLIB_DBCOUNT_SIZE     12
#define VIRLIB_DBDATE_SIZE      8
#define VIRLIB_FUNC_FLAG_SIZE   2
#define VIRLIB_INFECT_SIZE      8
#define VIRLIB_MAX_PATH_SIZE    260
#define VIRLIB_MAX_DESC_SIZE    300
#define VIRLIB_RESERVE_SIZE     16
#define VIRLIB_VIRUS_NAME_SIZE  22
#define VIRLIB_MAX_APP_ID_SIZE  80
#define VIRLIB_BOOT_BUFFER_SIZE 512

//
// Return Values
//
#define VIRLIB_OK               0

#define VIRLIB_CLEAN            0
#define VIRLIB_INFECTED         1
#define VIRLIB_ERROR            2
#define VIRLIB_NOT_ENAB         3
#define VIRLIB_NO_LOGICAL       4
#define VIRLIB_CANT_DELETE      10
#define VIRLIB_NOT_BOOT_DEVICE  100

//
// Virus Library Functions Supported (VIRUSLIBINIT.FuncEnab)
//
#define VIRLIB_SCANFILES        0x0001          // Scan files.
#define VIRLIB_DELFILES         0x0002          // Delete files.
#define VIRLIB_SCANMEMORY       0x0001          // Scan memory.
#define VIRLIB_SCANSYSAREA      0x0001          // Scan disk drive system areas.
#define VIRLIB_LOGICALDRV       0x0002          // Logical drives are supported.

//
// Virus Library Heuristic Scanning Level Values
//
#define VIRLIB_HEURISTICS_DEFAULT   0
#define VIRLIB_HEURISTICS_NONE      1
#define VIRLIB_HEURISTICS_LOW       2
#define VIRLIB_HEURISTICS_MEDIUM    3
#define VIRLIB_HEURISTICS_HIGH      4

//
// Virus Library Disk System Area Scanning Flags
//
#define VIRLIB_SCAN_DRIVE_FLAG  0x0001          // Scan a specific drive.
#define VIRLIB_FIXED_FLAG       0x0002          // Scan all fixed drives.
#define VIRLIB_REMOVABLE_FLAG   0x0004          // Scan all removable drive.
#define VIRLIB_PHYSICAL_DRIVES  0x8000          // Make all drive references physical

                                                // VirusLibraryScanSysBuffer() -
#define VIRLIB_BOOT_RECORD_BUFFER        0x0008 // buffer is a boot record.
#define VIRLIB_MASTER_BOOT_RECORD_BUFFER 0x0010 // buffer is a master boot record.

//
// Virus Library Memory Scanning Flags
//
#define VIRLIB_UPPER_FLAG       0x0001          // Scan upper memory.

//
// Virus Info Flags
//
#define VIRINFO_INFECT_FILES    0x00000001      // Virus infects FILES in general.
#define VIRINFO_INFECT_EXE      0x00000002      // Virus infects EXE's.
#define VIRINFO_INFECT_COM      0x00000004      // Virus infects COM's.
#define VIRINFO_INFECT_SYS      0x00000008      // Virus infects SYS's.
#define VIRINFO_INFECT_MBR      0x00000010      // Virus infects MBR's.
#define VIRINFO_INFECT_FLOPPY   0x00000020      // Virus infects floppies.
#define VIRINFO_INFECT_HD_BOOT  0x00000040      // Virus infects HD boot sectors.
#define VIRINFO_INFECT_WILD     0x00000080      // Virus is in the wild.
#define VIRINFO_INFECT_MEM_RES  0x00000100      // Virus can go memory resident.
#define VIRINFO_INFECT_SIZE_ST  0x00000200      // Virus is size stealth.
#define VIRINFO_INFECT_FULL_ST  0x00000400      // Virus is full stealth.
#define VIRINFO_INFECT_TRIGGER  0x00000800      // Virus has a trigger.
#define VIRINFO_INFECT_ENCRYPT  0x00001000      // Virus is encrypting.
#define VIRINFO_INFECT_POLYMOR  0x00002000      // Virus is polymorphic.
#define VIRINFO_INFECT_MULTIPRT 0x00004000      // Virus is multipartite.
#define VIRINFO_CAN_REPAIR      0x00008000      // Virus can be repaired.
#define VIRINFO_CAN_DELETE      0x00010000      // Virus can be deleted.
#define VIRINFO_MACRO           0x00020000      // Virus spreads through macros.
#define VIRINFO_WINDOWS         0x00040000      // Virus infects Windows executables.

//
// Self Test Values
//
#define VIRLIB_SELFTEST_OK      0               // Passed Self Test.
#define VIRLIB_SELFTEST_NO_CRC  1               // File has not been stamped.
#define VIRLIB_SELFTEST_FAIL    2               // CRC does not match.
#define VIRLIB_SELFTEST_ERROR   3               // Error reading file.


//***************************************************************************
// Structures
//***************************************************************************

#include <pshpack1.h>

typedef struct
{
    unsigned short  SysAreaFlags;
    unsigned short  ReserveFlag1;
    unsigned short  MemFlags;
    unsigned short  FileFlags;
    unsigned char   ReserveFlag2[8];

}   VIRLIBFUNCFLAGS;

typedef struct tagScannerHandle FAR *HVIRUSSCANNER;

typedef BOOL (WINAPI FAR * PROGRESSFUNC)( DWORD dwProgress, 
                                          DWORD dwTotal, 
                                          LPVOID lpvUserData );

typedef struct
{
    HVIRUSSCANNER   hScannerHandle;
    DWORD           dwInterfaceVersion;
    char            szAppID          [VIRLIB_MAX_APP_ID_SIZE + 1];
    char            szVirusDefDir    [VIRLIB_MAX_PATH_SIZE + 1];
    WORD            wVirusDateYear;
    WORD            wVirusDateMonth;
    WORD            wVirusDateDay;
    DWORD           dwVirusDefRev;
    DWORD           dwVirusDBCount;
    VIRLIBFUNCFLAGS FuncEnable;
    PROGRESSFUNC    Progress;
    BOOL            bDisableNAVAPBySession;
    BOOL            bManageVirusDefs;
    WORD            wHeuristicScanningLevel;
    char            Reserved         [VIRLIB_RESERVE_SIZE * 2];

}   VIRUSLIBINIT, FAR *LPVIRUSLIBINIT;

typedef struct
{
    BYTE            byInfectedLogicalDrive;
    BYTE            byInfectedPhysicalDrive;
    BYTE            byInfectedPartitionNumber;
    char            szInfectedFileName [VIRLIB_MAX_PATH_SIZE + 1];
    DWORD           dwVirusFlags;
    char            szVirusName        [VIRLIB_VIRUS_NAME_SIZE + 1];
    char            szVirusSize        [VIRLIB_INFECT_SIZE + 1];
    char            szVirusDescription [VIRLIB_MAX_DESC_SIZE + 1];
    WORD            wVirusID;
    char            Reserved           [(VIRLIB_RESERVE_SIZE * 2) - sizeof(WORD)];

}   VIRUSINFO, FAR *LPVIRUSINFO;

#include <poppack.h>


//***************************************************************************
// Function Prototypes for NAVSDK
//***************************************************************************

NAVSDKAPI VOID WINAPI VirusLibraryClose (   // Unload Virus Library
    HVIRUSSCANNER   hScannerHandle);        // [in] Scanner Handle

NAVSDKAPI WORD WINAPI VirusLibraryDeleteFile (  // Deletes specified File
    HVIRUSSCANNER   hScannerHandle,         // [in] Scanner Handle
    LPCSTR          lpszFile);              // [in] File to delete

NAVSDKAPI WORD WINAPI VirusLibraryGetLastVirusInfo (    // Get last virus info
    HVIRUSSCANNER   hScannerHandle,         // [in] Scanner Handle
    LPVIRUSINFO     lpVirInfo);             // [out] Virus Library Info structure

NAVSDKAPI WORD WINAPI VirusLibraryInit (    // Initialize virus library
    LPVIRUSLIBINIT  lpVirLibInit);          // [in|out] Virus Library Init structure

NAVSDKAPI WORD WINAPI VirusLibraryScanFile (// Scans File for viruses
    HVIRUSSCANNER   hScannerHandle,         // [in] Scanner Handle
    LPCSTR          lpszFile,               // [in] File to scan
    LPSTR           lpszVirusNameBuf);      // [out] Buffer to receive virus name

NAVSDKAPI WORD WINAPI VirusLibraryScanMem ( // Scans Memory for viruses
    HVIRUSSCANNER   hScannerHandle,         // [in] Scanner Handle
    WORD            wMemFlags,              // [in] Memory scanning flags
    LPVOID          lpvUserData,            // [in] User data for optional progress callback.
    LPSTR           lpszVirusNameBuf);      // [out] Buffer to receive virus name

NAVSDKAPI WORD WINAPI VirusLibraryScanSysArea ( // Scans Drive System Area
    HVIRUSSCANNER   hScannerHandle,         // [in] Scanner Handle
    WORD            wDriveFlags,            // [in] Drive scanning flags
    LPBYTE          lpDrive,                // [in|out] Physical drive number
    LPSTR           lpszVirusNameBuf);      // [out] Buffer to receive virus name

NAVSDKAPI WORD WINAPI VirusLibraryScanSysBuffer (   // Scans a buffer for boot viruses
    HVIRUSSCANNER   hScannerHandle,         // [in] Scanner Handle
    WORD            wBufferFlags,           // [in] Flags describing buffer content
    LPBYTE          lpSysBuffer,            // [in] Buffer with boot sector image
    LPSTR           lpszVirusNameBuf);      // [out] Buffer to receive virus name

NAVSDKAPI WORD WINAPI VirusLibrarySelfTest (// Checks integrity of specified File
    LPCSTR          lpszFile);              // [in] File to test

#ifdef __cplusplus
}
#endif

#endif // __NAVSDK_H

