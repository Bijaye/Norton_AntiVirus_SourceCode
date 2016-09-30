//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDSCAN.H_v   1.7   27 May 1997 16:51:32   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDSCAN.H_v  $
// 
//    Rev 1.7   27 May 1997 16:51:32   DCHI
// Modifications to support dynamic allocation of hit memory and new
// Office 97 CRC.
// 
//    Rev 1.6   10 Apr 1997 16:49:10   DCHI
// Added #define of WD_SCAN_FLAG_MACRO_IS_FULL_SET.
// 
//    Rev 1.5   08 Apr 1997 12:40:58   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.4   07 Apr 1997 18:11:18   DCHI
// Added MVP support.
// 
//    Rev 1.3   14 Mar 1997 16:33:46   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.2   13 Feb 1997 13:34:50   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.1   23 Jan 1997 11:09:04   DCHI
// Fixed global variable usage problem.
// 
//    Rev 1.0   17 Jan 1997 11:25:04   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDSCAN_H_

#define _WDSCAN_H_

typedef int WD_STATUS;

#define WD_STATUS_OK                0
#define WD_STATUS_ERROR             1
#define WD_STATUS_VIRUS_FOUND       2
#define WD_STATUS_SIG_HIT           3
#define WD_STATUS_REPAIR_USE_SIG    4
#define WD_STATUS_REPAIR_USE_OTHER  5
#define WD_STATUS_REPAIR_CUSTOM     6
#define WD_STATUS_REPAIRED          7

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

#define WD_RUN_BUF_SIZE     256

// The extra buffer must be at least 256 bytes to accomodate
//  scanning for the longest possible string.

#define WD_EXTRA_BUF_SIZE   256

typedef struct tagWD7_SCAN_INFO
{
    // XOR Encryption byte

    BYTE                    byEncrypt;

    // Starting offset in stream of bytes to scan

    long                    lStartOffset;

    LPMACRO_TABLE_INFO      lpstMacroTableInfo;
    LPMACRO_INFO_T          lpstMacroInfo;
    LPWDENCKEY              lpstKey;
} WD7_SCAN_INFO_T, FAR *LPWD7_SCAN_INFO;

#include "vbalznt.h"

typedef struct tagVBA5_SCAN_INFO
{
    LPLZNT                  lpstLZNT;
} VBA5_SCAN_INFO_T, FAR *LPVBA5_SCAN_INFO;

#define WD_SCAN_TYPE_WD7        0
#define WD_SCAN_TYPE_VBA5       1

#define WD_SCAN_FLAG_FULL_SET           0x0001
#define WD_SCAN_FLAG_MACRO_IS_FULL_SET  0x0002

typedef struct tagWD_SCAN_T
{
    LPCALLBACKREV1          lpstCallBack;
    LPOLE_FILE              lpstOLEStream;

    WORD                    wScanType;

    union
    {
        WD7_SCAN_INFO_T     stWD7;
        VBA5_SCAN_INFO_T    stVBA5;
    } uScan;

    BYTE                    abyName[WD_NAME_BUF_SIZE];

    DWORD                   dwCRC;

    // Size of stream

    long                    lSize;

    // Offset in macro of run buffer

    long                    lRunBufOffset;

    // Number of bytes in run buffer

    int                     nNumRunBufBytes;

    // Offset in run buffer of current step

    int                     nRunBufStepOffset;

    // Buffer for run scan

    BYTE                    abyRunBuf[WD_RUN_BUF_SIZE];

    // Offset in macro of extra buffer

    long                    lExtraBufOffset;

    // Number of bytes in extra buffer

    int                     nNumExtraBufBytes;

    // Buffer for sig scan

    BYTE                    abyExtraBuf[WD_EXTRA_BUF_SIZE];

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
} WD_SCAN_T, FAR *LPWD_SCAN;

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
} WD_SIG_SET_T, FAR *LPWD_SIG_SET;

extern WD_VIRUS_SIG_INFO_T FAR gstMVPVirusSigInfo;

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
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
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

#endif // #ifndef _WDSCAN_H_



