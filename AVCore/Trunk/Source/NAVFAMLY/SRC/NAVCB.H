// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVFAMLY/VCS/navcb.h_v   1.0   28 Apr 1997 13:45:48   MKEATIN  $
//
// Description:
//  Header for callback functions.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/NAVFAMLY/VCS/navcb.h_v  $
// 
//    Rev 1.0   28 Apr 1997 13:45:48   MKEATIN
// Initial revision.
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

#ifndef _NAV_H

#define _NAV_H

#include "avtypes.h"

#include "avcb.h"

typedef struct
{
    COOKIETRAY_T        stCookieTray;
    LPSTR               lpszObjectName;
    DWORD               dwLastTime;
    DWORD               dwLevel;
    DWORD               dwProgressValueMax;
} CMD_PROGRESS_T, FAR *LPCMD_PROGRESS;

extern FILECALLBACKS_T      gstNAVFileCallBacks;
extern GENERALCALLBACKS_T   gstNAVGeneralCallBacks;
extern PROGRESSCALLBACKS_T  gstNAVProgressCallBacks;
extern DATAFILECALLBACKS_T  gstNAVDataFileCallBacks;
extern IPCCALLBACKS_T       gstNAVIPCCallBacks;
extern MEMORYCALLBACKS_T    gstNAVMemoryCallBacks;
extern LINEARCALLBACKS      gstNAVLinearCallBacks;

// Functions also used internally by the command line scanner

CBSTATUS NAVFileOpen
(
    LPVOID      lpvFileInfo,
    WORD        wOpenMode
);

CBSTATUS NAVFileClose
(
    LPVOID      lpvFileInfo
);

CBSTATUS NAVFileGetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPDWORD lpdwInfo
);

CBSTATUS NAVFileSetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    DWORD   dwInfo
);

CBSTATUS NAVFileGetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPWORD  lpwTime,
    LPWORD  lpwDate
);

CBSTATUS NAVFileSetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    WORD    wTime,
    WORD    wDate
);

CBSTATUS NAVFileDelete
(
    LPVOID  lpvFileInfo
);

CBSTATUS NAVFileCopy
(
    LPVOID  lpvSrcFileInfo,
    LPVOID  lpvDstFileInfo
);

CBSTATUS NAVFileRename
(
    LPVOID  lpvOldFileInfo,
    LPWSTR  lpwstrNewName
);

CBSTATUS NAVFileExists
(
    LPVOID  lpvFileInfo,
    LPBOOL  lpbExists
);

CBSTATUS NAVLinearOpen
(
    LPVOID  lpvInfo,                // [in] Linear object info
    WORD    wOpenMode               // [in] filler
);

CBSTATUS NAVLinearClose
(
    LPVOID  lpvInfo                 // [in] Linear object info
);

CBSTATUS NAVLinearRead
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin reading
    DWORD   dwSectorHigh,           // [in] Sector's (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to read into
    DWORD   dwLength,               // [in] Sectors to read
    LPDWORD lpdwActualLength        // [out] Sectors read
);

CBSTATUS NAVLinearWrite
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin writing
    DWORD   dwSectorHigh,           // [in] Sector (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to write from
    DWORD   dwLength,               // [in] Sectors to write
    LPDWORD lpdwActualLength        // [in/out] Sectors written
);

CBSTATUS NAVLinearGetDimensions
(
    LPVOID  lpvInfo,                // [in] Linear object info
    LPBYTE  lpbySides,              // [out] Total Sides
    LPWORD  lpwCylinders,           // [out] Total Cylinders
    LPBYTE  lpbySectors             // [out] Sectors per cylinder
);

CBSTATUS NAVLinearConvertFromPhysical
(
    LPVOID      lpvInfo,            // [in] Linear object info
    BYTE        bySide,             // [in] Physical side, ...
    WORD        wCylinder,          // [in] cylinder, and ...
    BYTE        bySector,           // [in] sector to translate
    LPDWORD     lpdwSectorLow       // [out] Translated sector low dword
);

CBSTATUS NAVLinearConvertToPhysical
(
    LPVOID      lpvInfo,            // [in] Linear object info
    DWORD       dwSectorLow,        // [in] Linear sector low dword
    LPBYTE      lpbySide,             // [out] Translated side, ...
    LPWORD      lpwCylinder,          // [out] cylinder, and ...
    LPBYTE      lpbySector            // [out] sector
);

CBSTATUS NAVLinearIsPartitionedMedia
(
    LPVOID  lpvInfo,
    LPBOOL  lpbIsPartitionedMedia
);

#endif  // _NAV_H
