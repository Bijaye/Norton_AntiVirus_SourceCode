// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/w7h2sght.h_v   1.0   10 Jun 1998 13:05:58   DCHI  $
//
// Description:
//  Header file for Word 6.0/95 Heuristics Level II scan sig hit code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/w7h2sght.h_v  $
// 
//    Rev 1.0   10 Jun 1998 13:05:58   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _W7H2SIGS_H_

#define _W7H2SIGS_H_

//////////////////////////////////////////////////////////////////////
// Signature sets
//////////////////////////////////////////////////////////////////////

typedef struct tagW7H2_SIG_SET
{
    // Referenced Cmds

    int             nNumCmds;
    LPWORD          lpawCmd;

    // Referenced Dlgs

    int             nNumDlgs;
    LPWORD          lpawDlg;

    // Str signatures

    int             nNumStrSigs;
    LPBYTE FAR *    lpalpabyStrSigs;
    LPWORD          lpawStrSigIdx;
    LPBYTE          lpabyStrSigCatScores;

    // Line signatures

    int             nNumLineSigs;
    LPBYTE FAR *    lpalpabyLineSigs;
    LPBYTE          lpabyLineSigCatScores;

    // Func signatures

    int             nNumFuncSigs;
    LPBYTE FAR *    lpalpabyFuncSigs;
    LPBYTE          lpabyFuncSigCatScores;

    // Categories

    int             nNumCats;

    // Set signatures

    int             nNumSetSigs;
    LPBYTE FAR *    lpalpabySetSigs;
    LPWORD          lpawVID;
} W7H2_SIG_SET_T, FAR *LPW7H2_SIG_SET;

#define W7H2_SCAN_FLAG_ENCRYPTED_STR    0x00000001

typedef struct tagW7H2_SCAN
{
    LPVOID          lpvRootCookie;

    LPW7H2_SIG_SET  lpstSigSet;

    DWORD           dwHitMemSize;

    DWORD           dwLExtraFlags;
    LPBYTE          lpabyLCmdHit;
    LPBYTE          lpabyLDlgHit;
    LPBYTE          lpabyLOpHit;
    LPBYTE          lpabyLStrSigHit;
    LPBYTE          lpabyDlgValue;

    DWORD           dwFExtraFlags;
    LPBYTE          lpabyFCmdHit;
    LPBYTE          lpabyFDlgHit;
    LPBYTE          lpabyFOpHit;
    LPBYTE          lpabyFStrSigHit;
    LPBYTE          lpabyFLineSigHit;
    LPBYTE          lpabyFCatScore;

    DWORD           dwGExtraFlags;
    LPBYTE          lpabyGCmdHit;
    LPBYTE          lpabyGDlgHit;
    LPBYTE          lpabyGOpHit;
    LPBYTE          lpabyGLineSigHit;
    LPBYTE          lpabyGFuncSigHit;
    LPBYTE          lpabyGCatScore;

} W7H2_SCAN_T, FAR *LPW7H2_SCAN;

LPW7H2_SCAN W7H2ScanCreate
(
    LPVOID          lpvRootCookie,
    LPW7H2_SIG_SET  lpstSigSet
);

BOOL W7H2ScanDestroy
(
    LPW7H2_SCAN     lpstScan
);

void W7H2ScanInit
(
    LPW7H2_SCAN     lpstScan
);

void W7H2ScanCmdHit
(
    LPW7H2_SCAN     lpstScan,
    WORD            wCmd
);

void W7H2ScanDlgHit
(
    LPW7H2_SCAN     lpstScan,
    WORD            wDlg,
    int             nValue
);

void W7H2ScanOpHit
(
    LPW7H2_SCAN     lpstScan,
    BYTE            byOp
);

void W7H2ScanStrHit
(
    LPW7H2_SCAN     lpstScan,
    LPBYTE          lpabyStr
);

void W7H2ScanDoLineSigs
(
    LPW7H2_SCAN     lpstScan
);

void W7H2ScanDoFuncSigs
(
    LPW7H2_SCAN     lpstScan
);

int W7H2ScanDoSetSigs
(
    LPW7H2_SCAN     lpstScan
);

#endif // #ifndef _W7H2SIGS_H_

