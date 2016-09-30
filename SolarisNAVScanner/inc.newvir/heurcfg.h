// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/heurcfg.h_v   1.5   10 Jul 1997 15:59:38   CNACHEN  $
//
// Description:
//
//  This file has bloodhound config information structures.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/heurcfg.h_v  $
// 
//    Rev 1.5   10 Jul 1997 15:59:38   CNACHEN
// Added table for carry flags for int 21
// 
//    Rev 1.4   09 Jul 1997 15:41:08   CNACHEN
// Added dwFlags argument to config, also added #define for flags and # of config
// profiles
// 
//    Rev 1.3   14 May 1997 11:37:04   CNACHEN
// 
//    Rev 1.2   14 May 1997 11:35:14   CNACHEN
// Added filler data...
// 
//    Rev 1.1   14 May 1997 11:34:26   CNACHEN
// 
//************************************************************************

#ifndef _HEURCFG_H
#define _HEURCFG_H

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD            wStringLen;
    BYTE            abyString[32];
} HEUR_STRING_T, FAR *LPHEUR_STRING;

typedef struct
{
    WORD            wNumStrings;
    BYTE            abyFirstByteHash[256];
    HEUR_STRING_T   astBehaveStrings[32];
} HEUR_STRING_INFO_T, FAR *LPHEUR_STRING_INFO;

typedef struct
{
    WORD        wFillWord;              // fill unused memory with this

    WORD        wMOVSWriteThreshold;
    WORD        wIPEOFThreshold;
    WORD        wReadWriteBaitThreshold;
    WORD        wSuspiciousOpcodeThreshold;
    WORD        wSuspiciousIntThreshold;

    WORD        wMOVSWriteClosePercent;
    WORD        wIPEOFWriteClosePercent;

    WORD        wDisreguardCleanBehaviorInstrCount;

    WORD        wAssumedVirusSize;              // 4096 was the default
    WORD        wMinEXEVirusSize;               // 128

    WORD        wMaxEncryptedVirusIterations;   // required to decrypt (assumed virus size * 6)
    WORD        wMinEncryptedVirusIterations;   // to detect decryption

    WORD        wMinModifiedByteRange;          // to detect decryption
    WORD        wMaxIterForEPAdjust;            // entry-point adjustment instr count <= this
    WORD        wMaxIterForRangeAdjust;         // max iteration to adjust flood region

    WORD        wNormalInterruptVectorMax;      // anything higher is "strange"
    WORD        wNormalLegitInt21SubFuncMax;    // anything higher is "strange"
    WORD        wMinCallSubroutineSize;

    BYTE        bySuspiciousInts[256];          // 0=ok, !0 = funky
    BYTE        bySetCarryForInt21[128];        // TRUE means set carry flag

    DWORD       dwFlags;                        // flags

    BYTE        byFiller[256];                  // 256 bytes of filler data

    HEUR_STRING_INFO_T  stStringInfo;           // heuristics strings

} PAMConfigType, FAR *LPPAMCONFIG;

#define MAX_CONFIG_LEVELS   3

#define HEUR_FLAG_ABORT_CALL_LARGE_DISP     0x00000001UL


#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif
