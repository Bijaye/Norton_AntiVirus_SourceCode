// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1995 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/PREPAIR.H_v   1.1   06 Sep 1996 14:04:22   CNACHEN  $
//
// Description:
//
//  This source file contains data structures and constants used for
//  PAM file repairs.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/PREPAIR.H_v  $
// 
//    Rev 1.1   06 Sep 1996 14:04:22   CNACHEN
// Updated header to use proper #pragma pack() for NTK/WIN32.
// 
//    Rev 1.1   06 Sep 1996 14:01:18   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.0   01 Feb 1996 10:17:48   CNACHEN
// Initial revision.
// 
//    Rev 1.8   19 Dec 1995 19:08:10   CNACHEN
// Added prefetch queue support!
// 
// 
//    Rev 1.7   14 Dec 1995 10:49:24   CNACHEN
// Fixed repair stuff...
// 
//    Rev 1.6   11 Dec 1995 17:34:16   CNACHEN
// Changed ERS to contain status of repeat bytes, segment prefixes, and
// addr/operand overrides within the flags word at F000:FEFE
// 
//    Rev 1.5   24 Oct 1995 17:00:38   CNACHEN
// Changed AH=4C to AH=FD for repair termination
// 
//    Rev 1.4   23 Oct 1995 12:51:32   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.3   19 Oct 1995 18:39:04   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************


#define REPAIR_SIG_LEN          16
#define REPAIR_SIG_WILDCARD     0xFFFFU
#define MAX_REPAIR_SIGS         32
#define TRANSFER_BUFFER_SIZE    128
#define REPAIR_SEG              0xF000U
#define REPAIR_FOUNDATION_OFF   0x0000U
#define REPAIR_OVERLAY_OFF      0x8000U
#define REPAIR_HOST_TOF_OFF     0xFF00U
#define REPAIR_FAILURE          0xFFFFU
#define REPAIR_VIRUS_REGS       0xFED0U
#define REPAIR_STACK_OFF        0xFED0U     // right below VIRUS registers
#define REPAIR_DONE_BIT         0x8000U     // OR this with 21/fd return value

#define REPAIR_EAX_OFF          0xFED0U
#define REPAIR_EBX_OFF          0xFED4U
#define REPAIR_ECX_OFF          0xFED8U
#define REPAIR_EDX_OFF          0xFEDCU
#define REPAIR_ESI_OFF          0xFEE0U
#define REPAIR_EDI_OFF          0xFEE4U
#define REPAIR_EBP_OFF          0xFEE8U
#define REPAIR_ESP_OFF          0xFEECU
#define REPAIR_CS_OFF           0xFEF0U
#define REPAIR_DS_OFF           0xFEF2U
#define REPAIR_ES_OFF           0xFEF4U
#define REPAIR_SS_OFF           0xFEF6U
#define REPAIR_FS_OFF           0xFEF8U
#define REPAIR_GS_OFF           0xFEFAU
#define REPAIR_IP_OFF           0xFEFCU
#define REPAIR_FLAGS_OFF        0xFEFEU

#define REPAIR_FLAG_O           0x0001
#define REPAIR_FLAG_D           0x0002
#define REPAIR_FLAG_I           0x0004
#define REPAIR_FLAG_T           0x0008
#define REPAIR_FLAG_S           0x0010
#define REPAIR_FLAG_Z           0x0020
#define REPAIR_FLAG_A           0x0040
#define REPAIR_FLAG_P           0x0080
#define REPAIR_FLAG_C           0x0100

#define REPAIR_FLAG_REPZ        0x0200
#define REPAIR_FLAG_REPNZ       0x0400
#define REPAIR_FLAG_OP_OVER     0x0800
#define REPAIR_FLAG_ADDR_OVER   0x1000

#define REPAIR_NO_SEG_PREFIX    0x0000      // no prefix used
#define REPAIR_CS_SEG_PREFIX    0x2000
#define REPAIR_DS_SEG_PREFIX    0x4000
#define REPAIR_ES_SEG_PREFIX    0x6000
#define REPAIR_FS_SEG_PREFIX    0x8000
#define REPAIR_GS_SEG_PREFIX    0xA000
#define REPAIR_SS_SEG_PREFIX    0xC000


#define REPAIR_READ_FUNC        0x3F
#define REPAIR_WRITE_FUNC       0x40
#define REPAIR_SEEK_FUNC        0x42
#define REPAIR_TERMINATE_FUNC   0xFD
#define REPAIR_WRITE_THRU_ON    0xFE
#define REPAIR_WRITE_THRU_OFF   0xFF

#define REPAIR_INTERRUPT            2
#define REPAIR_STRATEGY             4
#define REPAIR_PREFETCH_8           8
#define REPAIR_PREFETCH_16         16
#define REPAIR_PREFETCH_32         32

#define REPAIR_DECRYPT              1
#define REPAIR_ERS                  2

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif


typedef struct
{
    WORD wSigLen;
    WORD wSig[REPAIR_SIG_LEN];
} repair_sig_type;

typedef struct
{
    WORD    wVirusID;
    DWORD   dwMaxIter;
    DWORD   dwMaxRepairProgIter;
    WORD    wNumSigs;
    WORD    wCodeSize;
    WORD    wFlags;
} repair_bundle_type;


typedef struct
{
    DWORD                   dwBundleOffset;
    repair_bundle_type      repair_bundle_info;
    repair_sig_type         repair_sig[MAX_REPAIR_SIGS];
} sig_list_type;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif


// There are two repair sections in the MASTER.DAT file.  The first repair
// section contains the "Repair Foundation" code, which will be loaded into
// VM at F000:0.
//
// The second repair section contains bundles of virus specific repair code.
// Each bundle is composed of the following information:
//
// WORD                 wNumRepairBundles;
//
// repair_bundle_type   repair_bundle;
// repair_sig_type      repair_sig[repair_bundle.wNumSig];
// BYTE                 RepairOverlay[repair_bundle].wCodeSize];
//
// repair_bundle_type   repair_bundle;
// repair_sig_type      repair_sig[repair_bundle.wNumSig];
// BYTE                 RepairOverlay[repair_bundle].wCodeSize];
//
// etc
//
//
// If one of the repair signature match (or there are no repair signatures),
// then the "Repair Foundation" is loaded, then the "Repair Overlay" code
// is loaded at F000:8000, and execution commences at F000:8000.
