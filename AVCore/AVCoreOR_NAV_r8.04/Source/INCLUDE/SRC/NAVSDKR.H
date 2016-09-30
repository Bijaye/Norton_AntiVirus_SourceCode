// Copyright 1995-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// Description:
//
//      This file defines repair functions in the Norton AntiVirus
//      Scanning Library.
//      
//************************************************************************

#ifndef __NAVSDKR_H
#define __NAVSDKR_H

#ifndef __NAVSDK_H
#include "navsdk.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

//***************************************************************************
// Definitions
//***************************************************************************

// 
// Repair Function Return Values
//
// #define VIRLIB_OK               0

#define VIRLIB_MEMALLOC         1
#define VIRLIB_FILE_ERROR       2
#define VIRLIB_CRIT_ERROR       3
#define VIRLIB_CANT_REPAIR      4
#define VIRLIB_DRIVE_ERROR      5
#define VIRLIB_INVALID_ARG      6
#define VIRLIB_ENTRY_UNAVAIL    7
#define VIRLIB_NO_EXIST         8
#define VIRLIB_WRONG_VERSION    9
#define VIRLIB_NO_ACCESS        11
#define VIRLIB_SORTERROR        12
#define VIRLIB_INSCAN_CREATION  13


//***************************************************************************
// Repair Function Prototypes for NAVSDK
//***************************************************************************

NAVSDKAPI WORD WINAPI VirusLibraryRepairFile (  // Repairs specified File
    HVIRUSSCANNER   hScannerHandle,             // [in] Scanner Handle
    LPCSTR          lpszFile );                 // [in] File to delete

NAVSDKAPI WORD WINAPI VirusLibraryRepairSysArea ( // Scans system areas for viruses
    HVIRUSSCANNER   hScannerHandle,     // [in] Scanner Handle
    WORD            wDriveFlags,        // [in] Drive scanning flags
    BYTE            byDrive ) ;         // [in] Drive number designator

#ifdef __cplusplus
}
#endif

#endif // __NAVSDKR_H

