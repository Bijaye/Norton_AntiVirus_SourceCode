// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/w7h2scan.h_v   1.0   10 Jun 1998 13:05:52   DCHI  $
//
// Description:
//  Header file for Word 6.0/95 Heuristics Level II scan code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/w7h2scan.h_v  $
// 
//    Rev 1.0   10 Jun 1998 13:05:52   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _W7H2SCAN_H_

#define _W7H2SCAN_H_

#define W7H2_IS_FUNC_BIT_ARRAY_SIZE     4096
#define W7H2_IS_FUNC_MASK               0xFFF

// This should not be increased above 254

#define W7H2_MAX_FUNCS                  128

#define W7H2_RUN_STATE_TO_RUN           255

#define W7H2_STR_BUF_SIZE               256
#define W7H2_RUN_BUF_SIZE               4096

#define W7H2_main_CRC                   0x40D7329B

typedef struct tagW7H2
{
    LPVOID          lpvRootCookie;
    LPSS_STREAM     lpstStream;
    LPWD7ENCKEY     lpstKey;

    WD7_TDT_INFO_T  stTDTInfo;

    // Run buffer

    DWORD           dwRunBufOffset;
    DWORD           dwRunBufEndOffset;
    BYTE            abyRunBuf[W7H2_RUN_BUF_SIZE];

    // Current macro

    DWORD           dwOffset;
    DWORD           dwSize;
    BYTE            byEncryptByte;
    BYTE            abyName[256];
    BOOL            bBigEndian;
    DWORD           dwCRC;

    // Called functions

    int             nNumCalledFuncs;
    BYTE            abyCalledFuncs[W7H2_MAX_FUNCS];

    // Function info

    int             nNumFuncs;
    DWORD           adwFuncNameCRC[W7H2_MAX_FUNCS];
    DWORD           adwFuncOffset[W7H2_MAX_FUNCS];
    DWORD           adwIsFunc[W7H2_IS_FUNC_BIT_ARRAY_SIZE / 32];

    // Function run state

    int             nRunState;
    BYTE            abyRunState[W7H2_MAX_FUNCS];
} W7H2_T, FAR *LPW7H2;

EXTSTATUS W7H2Scan
(
    LPSS_STREAM         lpstStream
);

#endif // #ifndef _W7H2SCAN_H_

