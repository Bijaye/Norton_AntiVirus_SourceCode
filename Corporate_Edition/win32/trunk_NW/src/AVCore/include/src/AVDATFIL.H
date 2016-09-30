// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/AVDATFIL.H_v   1.3   08 Jul 1997 17:49:40   MKEATIN  $
//
// Description:
//  AVAPI 2.0 datafile access definitions and structures.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/AVDATFIL.H_v  $
// 
//    Rev 1.3   08 Jul 1997 17:49:40   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.2   02 Jul 1997 10:17:50   CNACHEN
// added proper w32/ntk pragma pack(push/pops)
// 
// 
//    Rev 1.1   18 Apr 1997 13:44:20   MKEATIN
// Latest header from NEWVIR/INCLUDE
// 
//    Rev 1.2   20 Nov 1996 15:24:12   AOONWAL
// No change.
// 
//    Rev 1.1   22 Oct 1996 12:06:18   AOONWAL
// No change.
// 
//    Rev 1.0   04 Jun 1996 12:57:06   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _AVDATFIL_H

#define _AVDATFIL_H

#include "avtypes.h"
#include "avcb.h"

#define DATA_FILE_SIGNATURE             0x434D5953UL

typedef unsigned int DFSTATUS;

#define DFSTATUS_OK                     0
#define DFSTATUS_ERROR                  1

// file structures

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    DWORD       dwSignature;                /* signature for data file */
    DWORD       dwVersionNumber;
    BYTE        byMon, byDay, byYear;       /* last revision */
    BYTE        byFiller;
    WORD        wNumSections;
    DWORD       dwTableOffset;
    DWORD       dwCRC;                      /* integrity CRC */
    BYTE        byPadding[10];             /* pad to 32 bytes */
} DATA_FILE_HEADER_T, FAR *LPDATA_FILE_HEADER;

typedef struct
{
    DWORD       dwIdent;
    DWORD       dwVersionNumber;
    DWORD       dwStartOffset;
    DWORD       dwLength;
    BYTE        byMon, byDay, byYear;       /* last revision */
} DATA_FILE_TABLE_T, FAR *LPDATA_FILE_TABLE;

#define sDATA_FILE_TABLE  sizeof(DATA_FILE_TABLE_T)

#define sDATAFILEHEADER         sizeof(DATA_FILE_HEADER_T)

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif


DFSTATUS EngOpenDataFile
(
    LPDATAFILECALLBACKS lpDataFileCallBacks,
    WORD                wDataFileNum,
    WORD                wOpenMode,
    LPLPVOID            lplpvFileInfo
);

DFSTATUS EngLookUp
(
    LPDATAFILECALLBACKS   lpDataFileCallBacks,
    LPVOID                lpvFileInfo,
    DWORD                 dwIdent,
    LPDATA_FILE_TABLE     lpDataFileSection,
    LPBOOL                lpbSectionFound
);

DFSTATUS EngCloseDataFile
(
    LPDATAFILECALLBACKS    lpDataFileCallBacks,
    LPVOID                 lpvFileInfo
);

#endif  // _AVDATFIL_H

