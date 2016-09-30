//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wdscan15.h_v   1.4   04 Aug 1998 13:54:16   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wdscan15.h_v  $
// 
//    Rev 1.4   04 Aug 1998 13:54:16   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.3   11 May 1998 18:05:34   DCHI
// Changes for menu repair and
// for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.2   15 Apr 1998 17:24:48   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.1   10 Mar 1998 13:41:14   DCHI
// Modified to use new O97 CRC.
// 
//    Rev 1.0   09 Jul 1997 16:17:18   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDSCAN15_H_

#define _WDSCAN15_H_

typedef int WD_STATUS;

#define WD_STATUS_OK                0
#define WD_STATUS_ERROR             1
#define WD_STATUS_VIRUS_FOUND       2
#define WD_STATUS_SIG_HIT           3
#define WD_STATUS_REPAIR_USE_SIG    4
#define WD_STATUS_REPAIR_USE_OTHER  5
#define WD_STATUS_REPAIR_CUSTOM     6
#define WD_STATUS_REPAIRED          7
#define WD_STATUS_MENU_REPAIR       8

typedef signed char     SBYTE;
typedef LPBYTE FAR *    LPLPBYTE;

typedef struct tagWD_SIG_INFO_T
{
    WORD                    wBaseID;
    LPBYTE                  lpabySigs;
} WD_SIG_INFO_T, FAR *LPWD_SIG_INFO;

typedef struct tagWD_NAMED_CRC_SIG_INFO_T
{
    WORD                    wCRCIndex;
    WORD                    wBaseID;
    LPBYTE                  lpabySigs;
} WD_NAMED_CRC_SIG_INFO_T, FAR *LPWD_NAMED_CRC_SIG_INFO;

typedef struct tagWD_VIRUS_SIG_INFO_T
{
    WORD                    wID;
    LPBYTE                  lpabySig;
} WD_VIRUS_SIG_INFO_T, FAR *LPWD_VIRUS_SIG_INFO;

typedef LPWD_VIRUS_SIG_INFO FAR *LPLPWD_VIRUS_SIG_INFO;

typedef struct tagWD_SIG_SET_T FAR *LPWD_SIG_SET;
typedef struct tagWD_SCAN_T FAR *LPWD_SCAN;

typedef WD_STATUS (*LPF_WD_CUSTOM_SCAN)
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
);

typedef LPF_WD_CUSTOM_SCAN FAR *LPLPF_WD_CUSTOM_SCAN;

typedef WD_STATUS (*LPF_WD_CUSTOM_REPAIR)
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPBOOL              lpbDelete,
    WORD                wID
);

typedef LPF_WD_CUSTOM_REPAIR FAR *LPLPF_WD_CUSTOM_REPAIR;

// The name buffer must be at least 256 bytes to accomodoate
//  the longest possible name

#define WD_NAME_BUF_SIZE    256

// The run buffer should be reasonable in size for efficiency and speed

//#define WD_RUN_BUF_SIZE     256
#define DEF_WD_RUN_BUF_SIZE     3072

// The extra buffer must be at least 256 bytes to accomodate
//  scanning for the longest possible string.

//#define WD_EXTRA_BUF_SIZE   256
#define DEF_WD_EXTRA_BUF_SIZE   1024

typedef struct tagWD7_SCAN_INFO
{
    // XOR Encryption byte

    BYTE                    byEncrypt;

    // Starting offset in stream of bytes to scan

    long                    lStartOffset;
    DWORD                   dwMacroOffset;
    DWORD                   dwMacroSize;
    BYTE                    byMacroEncryptByte;

    LPWD7_MACRO_SCAN        lpstMacroScan;
} WD7_SCAN_INFO_T, FAR *LPWD7_SCAN_INFO;

typedef struct tagVBA5_SCAN_INFO
{
    LPLZNT                  lpstLZNT;

    DWORD                   dwCRC2StartOffset;
} VBA5_SCAN_INFO_T, FAR *LPVBA5_SCAN_INFO;

typedef struct tagXL5_SCAN_INFO
{
    DWORD                   dwTableOffset;
    DWORD                   dwTableSize;
    DWORD                   dwMacroOffset;
    DWORD                   dwMacroSize;
    XL5_DIR_INFO_T          stDirInfo;
} XL5_SCAN_INFO_T, FAR *LPXL5_SCAN_INFO;

typedef struct tagXL4_SCAN_INFO
{
    LPXL5ENCKEY             lpstKey;

    DWORD                   dwBOFOffset;
} XL4_SCAN_INFO_T, FAR *LPXL4_SCAN_INFO;

typedef struct tagA2_SCAN_INFO
{
    A2_T                    stA2;
    A2_OBJ_ACCESS_T         stObj;
} A2_SCAN_INFO_T, FAR *LPA2_SCAN_INFO;

typedef struct tagA97_SCAN_INFO
{
    A97_T                   stA97;
    A97LZNT_T               stLZNT;
    A97_OBJ_ACCESS_T        stObj;
    DWORD                   dwStartOffset;
} A97_SCAN_INFO_T, FAR *LPA97_SCAN_INFO;

#define WD_SCAN_TYPE_WD7        0
#define WD_SCAN_TYPE_VBA5       1
#define WD_SCAN_TYPE_XL5        2
#define WD_SCAN_TYPE_XL4_95     3
#define WD_SCAN_TYPE_XL4_97     4
#define WD_SCAN_TYPE_A2         5
#define WD_SCAN_TYPE_A97_MACRO  6
#define WD_SCAN_TYPE_A97_VBMOD  7
#define WD_SCAN_TYPE_A97_RAW    8
#define WD_SCAN_TYPE_XL5_DIR    9

#define WD_SCAN_FLAG_FULL_SET           0x0001
#define WD_SCAN_FLAG_MACRO_IS_FULL_SET  0x0002

typedef struct tagWD_SCAN_T
{
    LPSS_STREAM             lpstStream;

    WORD                    wScanType;

    union
    {
        WD7_SCAN_INFO_T     stWD7;
        VBA5_SCAN_INFO_T    stVBA5;
        XL5_SCAN_INFO_T     stXL5;
        XL4_SCAN_INFO_T     stXL4;
        LPA2_SCAN_INFO      lpstA2;
        LPA97_SCAN_INFO     lpstA97;
    } uScan;

    BYTE                    abyName[WD_NAME_BUF_SIZE];

    DWORD                   dwCRC;
    DWORD                   dwMVPCRC;

    // Size of stream

    long                    lSize;

    // Offset in macro of run buffer

    long                    lRunBufOffset;

    // Number of bytes in run buffer

    int                     nNumRunBufBytes;

    // Offset in run buffer of current step

    int                     nRunBufStepOffset;

    // Buffer for run scan

    int                     nRunBufSize;
    LPBYTE                  abyRunBuf;

    // Offset in macro of extra buffer

    long                    lExtraBufOffset;

    // Number of bytes in extra buffer

    int                     nNumExtraBufBytes;

    // Buffer for sig scan

    int                     nExtraBufSize;
    LPBYTE                  abyExtraBuf;

    // Macro/module count

    WORD                    wMacroCount;

    // Flags

    WORD                    wFlags;

    // Pointers to sig hit arrays

    LPBYTE                  lpabyNameSigInterMacroHit;
    LPBYTE                  lpabyNameSigIntraMacroHit;
    LPBYTE                  lpabyNamedCRCSigHit;
    LPBYTE                  lpabyMacroSigHit;
    LPBYTE                  lpabyCRCHit;
} WD_SCAN_T, FAR *LPWD_SCAN, FAR * FAR *LPLPWD_SCAN;

typedef struct tagWD_SIG_SET_T
{
    // Name sig info

    long                    lNumNameSetOffsetSigs;
    LPBYTE                  lpabyNameSetOffsetSigs;
    long                    lNumNameEndOffsetSigs;
    LPBYTE                  lpabyNameEndOffsetSigs;
    LPWD_SIG_INFO           lpastRawNameSigIndex;
    LPBYTE                  lpabyNameSigRawByteHit;
    long                    lNumNameSigs;

    // Named CRC sig info

    long                    lNumNamedCRCSigSets;
    long                    lNumNamedCRCSigs;
    LPWD_NAMED_CRC_SIG_INFO lpastNamedCRCSigInfo;

    // Macro sig info

    long                    lNumMacroSetOffsetSigs;
    LPBYTE                  lpabyMacroSetOffsetSigs;
    long                    lNumMacroEndOffsetSigs;
    LPBYTE                  lpabyMacroEndOffsetSigs;
    LPWD_SIG_INFO           lpastRawMacroSigIndex;
    LPBYTE                  lpabyMacroSigRawByteHit;
    long                    lNumMacroSigs;

    // CRC info

    long                    lNumCRCs;
    LPDWORD                 lpadwCRCs;

    // Virus signatures

    long                    lNumVirusSigs;
    LPWD_VIRUS_SIG_INFO     lpastVirusSigInfo;

    // Custom scan and repair arrays

    LPLPF_WD_CUSTOM_SCAN    lpapfCustomScan;
    LPLPF_WD_CUSTOM_REPAIR  lpapfCustomRepair;

    // Named CRC sig full set

    long                    lNamedCRCSigFullSetLen;
    LPBYTE                  lpabyNamedCRCSigFullSet;

    // Macro sig full set

    long                    lMacroSigFullSetLen;
    LPBYTE                  lpabyMacroSigFullSet;

    // Menu Repair Signatures

    long                    lMenuRepairSigArraySize;
    LPBYTE                  lpabyMenuRepairSigs;
    
} WD_SIG_SET_T, FAR *LPWD_SIG_SET;

extern WD_VIRUS_SIG_INFO_T FAR gstMVPVirusSigInfo;

WD_STATUS WDAllocScanStruct
(
    LPVOID          lpvRootCookie,
    LPLPWD_SCAN     lplpstScan,
    int             nRunBufSize,
    int             nExtraBufSize
);

WD_STATUS WDFreeScanStruct
(
    LPVOID          lpvRootCookie,
    LPWD_SCAN       lpstScan
);

WD_STATUS WDApplyNameSigs
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
);

WD_STATUS WDScanMacro
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan
);

void WDInitHitBitArrays
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan
);

WD_STATUS WDScanDoc
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo,
    LPBOOL                  lpbMVPApproved
);

WD_STATUS VBA5ScanModule
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan
);

WD_STATUS WDApplyVirusSignatures
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
);

WD_STATUS WDScanXL4Macro
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
);

WD_STATUS WDScanA2Macros
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
);

#endif // #ifndef _WDSCAN15_H_



