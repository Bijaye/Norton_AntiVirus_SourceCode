// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/msxi.h_v   1.2   16 Dec 1998 14:17:46   DCHI  $
//
// Description:
//  MSX internal function prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/msxi.h_v  $
// 
//    Rev 1.2   16 Dec 1998 14:17:46   DCHI
// Added #define of MSX_L1__VBA_PROJECT.
// 
//    Rev 1.1   15 Dec 1998 12:12:56   DCHI
// Added MSX_L1_POWERPOINT.
// 
//    Rev 1.0   08 Dec 1998 12:53:06   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MSXI_H_

#define _MSXI_H_

#define MSX_ALL_CLEAN       0
#define MSX_NORMAL_SCAN     1
#define MSX_L2_FILTER       2

#define MSX_L1_UNKNOWN      0
#define MSX_L1_WORDDOCUMENT 1
#define MSX_L1_BOOK         2
#define MSX_L1_WORKBOOK     3
#define MSX_L1_DIR          4
#define MSX_L1__VBA_PROJECT 5
#define MSX_L1_POWERPOINT   6
#define MSX_L2__VBA_PROJECT 7
#define MSX_L2_VBA          8

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagMSX_DBENTRY
{
    BYTE            byPrev;         // Previous MRU entry
    BYTE            byNext;         // Next MRU entry
    WORD            wL1CRCSize;     // Low 16-bits of first-level CRC size
    DWORD           dwL1CRC;        // First-level CRC
    DWORD           dwL2CRCSize;    // Size of second-level CRC
    DWORD           dwL2CRC;        // Second-level CRC
} MSX_DBENTRY_T, FAR *LPMSX_DBENTRY;

#define MSX_SIGNATURE               0x434D5953      // SYMC
#define MSX_VERSION                 0x00010000      // 0.1.0.0

#define MSX_DB_NUM_ENTRIES      128

typedef struct tagMSX_DB
{
    DWORD           dwSignature;
    DWORD           dwVersion;
    BYTE            byMonth, byDay, byYear;
    BYTE            byFiller;
    DWORD           dwCRCCheck;

    BYTE            byMRU;          // The MRU entry
    BYTE            byLRU;          // The LRU entry
    BYTE            byMRUDepth;     // Depth of updated MRU entries
    BYTE            byFiller2;

    MSX_DBENTRY_T   astEntries[MSX_DB_NUM_ENTRIES];
} MSX_DB_T, FAR *LPMSX_DB;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

typedef struct tagMSX
{
    LPVOID              lpvCookie;      // Callback cookie value
    LPBYTE              lpabyWorkBuf;
    int                 nWorkBufSize;
    LPMSX_DB            lpstDB;
} MSX_T, FAR *LPMSX;

//////////////////////////////////////////////////////////////////////
//
// G l o b a l s
//
//////////////////////////////////////////////////////////////////////

extern MSX_DB_T FAR     gstMSXDB;
extern LPVOID           glpvMSXMutex;

//////////////////////////////////////////////////////////////////////
//
// F u n c t i o n    p r o t o t y p e s
//
//////////////////////////////////////////////////////////////////////

BOOL MSXCRCStream
(
    LPSS_STREAM         lpstStream,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwStartOffset,
    DWORD               dwEndOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
);

BOOL MSXL1WordDocumentCRC
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwMCDOffset,
    WORD                wMCDCount,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
);

BOOL MSXL2WordDocumentCRC
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwMCDOffset,
    WORD                wMCDCount,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
);

BOOL MSXXLFindFirstBoundsheet
(
    LPSS_STREAM         lpstStream,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwBoundSheetOffset
);

BOOL MSXXL4SheetNameSetCRC
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwBoundSheetOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
);

BOOL MSXXL4SheetSetCRC
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwBoundSheetOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
);

BOOL MSXL2_VBA_PROJECTCRC
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
);

BOOL MSXL2VBACRC
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
);

BOOL MSXDBL1Match
(
    LPMSX               lpstMSX,
    DWORD               dwCRC,
    DWORD               dwCRCSize
);

BOOL MSXDBL2Match
(
    LPMSX               lpstMSX,
    DWORD               dwCRC,
    DWORD               dwCRCSize
);

BOOL MSXDBAdd
(
    LPMSX               lpstMSX,
    DWORD               dwL1CRC,
    DWORD               dwL1CRCSize,
    DWORD               dwL2CRC,
    DWORD               dwL2CRCSize
);

void MSXDBZero
(
    LPMSX_DB            lpstDB
);

void MSXDBMergeMRUEntries
(
    LPMSX_DB            lpstSrcDB,
    LPMSX_DB            lpstDstDB
);

int MSXL1Filter
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream
);

int MSXL2Filter
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream
);

BOOL MSXAddCRCs
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream
);

#endif // #ifndef _MSXI_H_

