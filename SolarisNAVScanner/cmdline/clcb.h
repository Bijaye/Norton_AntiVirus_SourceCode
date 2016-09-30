// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/clcb.h_v   1.3   23 Oct 1996 11:55:52   DCHI  $
//
// Description:
//  Header for callback functions.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/clcb.h_v  $
// 
//    Rev 1.3   23 Oct 1996 11:55:52   DCHI
// Added support for IPC callbacks.
// 
//    Rev 1.2   05 Jun 1996 21:01:40   RAY
// Added boot scanning and repair support
// 
//    Rev 1.1   04 Jun 1996 18:18:10   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.0   21 May 1996 12:26:20   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _CLCB_H

#define _CLCB_H

#include "avtypes.h"

#include "avcb.h"

#include "fileinfo.h"

typedef struct
{
    COOKIETRAY_T        stCookieTray;
    LPSTR               lpszObjectName;
    DWORD               dwLastTime;
    DWORD               dwLevel;
    DWORD               dwProgressValueMax;
} CMD_PROGRESS_T, FAR *LPCMD_PROGRESS;

extern FILECALLBACKS_T      gstCLCBFileCallBacks;
extern GENERALCALLBACKS_T   gstCLCBGeneralCallBacks;
extern PROGRESSCALLBACKS_T  gstCLCBProgressCallBacks;
extern DATAFILECALLBACKS_T  gstCLCBDataFileCallBacks;
extern IPCCALLBACKS_T       gstCLCBIPCCallBacks;
extern MEMORYCALLBACKS_T    gstCLCBMemoryCallBacks;
extern LINEARCALLBACKS      gstCLCBLinearCallBacks;

// Functions also used internally by the command line scanner

CBSTATUS CLCBFileOpen
(
    LPVOID      lpvFileInfo,
    WORD        wOpenMode
);

CBSTATUS CLCBFileClose
(
    LPVOID      lpvFileInfo
);

CBSTATUS CLCBFileGetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPDWORD lpdwInfo
);

CBSTATUS CLCBFileSetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    DWORD   dwInfo
);

CBSTATUS CLCBFileGetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPWORD  lpwTime,
    LPWORD  lpwDate
);

CBSTATUS CLCBFileSetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    WORD    wTime,
    WORD    wDate
);

CBSTATUS CLCBFileDelete
(
    LPVOID  lpvFileInfo
);

CBSTATUS CLCBFileCopy
(
    LPVOID  lpvSrcFileInfo,
    LPVOID  lpvDstFileInfo
);

CBSTATUS CLCBFileRename
(
    LPVOID  lpvOldFileInfo,
    LPWSTR  lpwstrNewName
);

CBSTATUS CLCBFileExists
(
    LPVOID  lpvFileInfo,
    LPBOOL  lpbExists
);

CBSTATUS CLCBLinearOpen
(
    LPVOID  lpvInfo,                // [in] Linear object info
    WORD    wOpenMode               // [in] filler
);

CBSTATUS CLCBLinearClose
(
    LPVOID  lpvInfo                 // [in] Linear object info
);

CBSTATUS CLCBLinearRead
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin reading
    DWORD   dwSectorHigh,           // [in] Sector's (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to read into
    DWORD   dwLength,               // [in] Sectors to read
    LPDWORD lpdwActualLength        // [out] Sectors read
);

CBSTATUS CLCBLinearWrite
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin writing
    DWORD   dwSectorHigh,           // [in] Sector (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to write from
    DWORD   dwLength,               // [in] Sectors to write
    LPDWORD lpdwActualLength        // [in/out] Sectors written
);

CBSTATUS CLCBLinearGetDimensions
(
    LPVOID  lpvInfo,                // [in] Linear object info
    LPBYTE  lpbySides,              // [out] Total Sides
    LPWORD  lpwCylinders,           // [out] Total Cylinders
    LPBYTE  lpbySectors             // [out] Sectors per cylinder
);

CBSTATUS CLCBLinearConvertFromPhysical
(
    LPVOID      lpvInfo,            // [in] Linear object info
    BYTE        bySide,             // [in] Physical side, ...
    WORD        wCylinder,          // [in] cylinder, and ...
    BYTE        bySector,           // [in] sector to translate
    LPDWORD     lpdwSectorLow       // [out] Translated sector low dword
);

CBSTATUS CLCBLinearConvertToPhysical
(
    LPVOID      lpvInfo,            // [in] Linear object info
    DWORD       dwSectorLow,        // [in] Linear sector low dword
    LPBYTE      lpbySide,             // [out] Translated side, ...
    LPWORD      lpwCylinder,          // [out] cylinder, and ...
    LPBYTE      lpbySector            // [out] sector
);

CBSTATUS CLCBLinearIsPartitionedMedia
(
    LPVOID  lpvInfo,
    LPBOOL  lpbIsPartitionedMedia
);

#endif  // _CLCB_H
