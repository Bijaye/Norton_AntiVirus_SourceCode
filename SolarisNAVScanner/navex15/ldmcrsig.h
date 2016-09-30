// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/LDMCRSIG.H_v   1.2   15 Dec 1998 12:14:04   DCHI  $
//
// Description:
//
//  This source file contains prototypes for routines for loading
//  the macro signature set.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/LDMCRSIG.H_v  $
// 
//    Rev 1.2   15 Dec 1998 12:14:04   DCHI
// Updated for P97 signature set.
// 
//    Rev 1.1   04 Aug 1998 13:54:12   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.0   09 Jul 1997 16:15:22   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _LDMCRSIG_H_

#define _LDMCRSIG_H_

#include "w7h2sght.h"

// Heuristic exclusion signature data

#define WD7_EXCLUSION_BASE_ID   650
#define X95_EXCLUSION_BASE_ID   652
#define WD8_EXCLUSION_BASE_ID   660
#define X97_EXCLUSION_BASE_ID   662

// Macro scanning signature data

#define WD7_BASE_ID             600
#define WD8_BASE_ID             610
#define XL97_BASE_ID            620
#define XL95_BASE_ID            630
#define XL4_BASE_ID             640
#define A2_BASE_ID              670
#define A97_BASE_ID             680
#define P97_BASE_ID             6100

#define W7H2_BASE_ID            690

#define ID_OFFSET_INFO_SECTION  0
#define ID_OFFSET_DATA_SECTION  1
#define ID_OFFSET_CRC_SECTION   2

#define POOL_RAW_NAME_SIG_INDEX     0
#define POOL_NAMED_CRC_SIG_INFO     1
#define POOL_RAW_MACRO_SIG_INDEX    2
#define POOL_CRC_POOL               3
#define POOL_VIRUS_SIG_INFO         4

#if defined( __MACINTOSH__ )
typedef struct tagMACRO_SIG_SET_INFO
{
    DWORD           dwBaseID;
    LPWD_SIG_SET    lpstSigSet;
    DWORD           dwNumPools;
    LPBYTE FAR *    lplpabyPools;
} MACRO_SIG_SET_INFO_T, FAR *LPMACRO_SIG_SET_INFO;
#endif

#define NUM_MACRO_SIG_SETS      8

#define LOAD_MACRO_SIG_INFO_BUF_SIZE    4096

typedef struct tagLOAD_MACRO_SIG
{
    LPCALLBACKREV1      lpstCallBack;
    HFILE               hFile;

    DWORD               dwInfoOffset;
    DWORD               dwInfoLength;

    DWORD               dwNumInfoBufBytes;
    DWORD               dwInfoBufIndex;
    BYTE                abyInfoBuf[LOAD_MACRO_SIG_INFO_BUF_SIZE];

    WORD                wNumPools;
    LPBYTE FAR *        lpalpbyPools;

} LOAD_MACRO_SIG_T, FAR *LPLOAD_MACRO_SIG;

#define VDF_SIGNATURE               0x434D5953UL

// file structures

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagVDF_HEADER
{
    DWORD       dwSignature;            // signature for data file
    DWORD       dwVersionNumber;
    BYTE        byMon, byDay, byYear;   // last revision
    BYTE        byFiller;
    WORD        wNumSections;
    DWORD       dwTableOffset;
    DWORD       dwCRC;                  // integrity CRC
    BYTE        byPadding[10];          // pad to 32 bytes
} VDF_HEADER_T, FAR *LPVDF_HEADER;

typedef struct tagVDF_TABLE
{
    DWORD       dwIdent;
    DWORD       dwVersionNumber;
    DWORD       dwStartOffset;
    DWORD       dwLength;
    BYTE        byMon, byDay, byYear;   // last revision
} VDF_TABLE_T, FAR *LPVDF_TABLE;

#define sVDF_TABLE  sizeof(VDF_TABLE_T)

#define sVDF_HEADER sizeof(VDF_HEADER_T)

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

BOOL LoadMacroSigs
(
    LPCALLBACKREV1      lpstCallBack,
    LPTSTR              lpszDataDir
);

BOOL UnloadMacroSigs
(
    LPCALLBACKREV1      lpstCallBack
);

#if defined( __MACINTOSH__ )
BOOL VDFLookUp
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwIdent,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwLength
);

BOOL LoadWORDValue
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPWORD              lpwValue
);

BOOL LoadPoolInfo
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPBYTE FAR *        lplpbyPool
);

BOOL LoadWORDandPoolInfo
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPWORD              lpwValue,
    LPBYTE FAR *        lplpbyPool
);

BOOL LoadMacroSigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPWD_SIG_SET        lpstSigSet,
    LPDWORD             lpdwPoolCount,
    LPBYTE FAR * FAR *  lplpalpbyPools
);

BOOL LoadExclusionSigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPDWORD             lpdwSigCount,
    LPDWORD FAR *       lplpdwCRCs,
    LPDWORD FAR *       lplpdwCRCByteCounts
);

BOOL LoadW7H2SigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPW7H2_SIG_SET      lpstSigSet
);

BOOL VDFLookUp
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwIdent,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwLength
);

BOOL LoadWORDValue
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPWORD              lpwValue
);

BOOL LoadPoolInfo
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPBYTE FAR *        lplpbyPool
);

BOOL LoadWORDandPoolInfo
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPWORD              lpwValue,
    LPBYTE FAR *        lplpbyPool
);

BOOL LoadMacroSigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPWD_SIG_SET        lpstSigSet,
    LPDWORD             lpdwPoolCount,
    LPBYTE FAR * FAR *  lplpalpbyPools
);

BOOL LoadExclusionSigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPDWORD             lpdwSigCount,
    LPDWORD FAR *       lplpdwCRCs,
    LPDWORD FAR *       lplpdwCRCByteCounts
);

BOOL LoadW7H2SigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPW7H2_SIG_SET      lpstSigSet
);
#endif

#endif // _#ifndef _LDMCRSIG_H_

