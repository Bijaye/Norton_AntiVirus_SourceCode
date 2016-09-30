// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/aolpw.h_v   1.2   05 Aug 1998 11:37:26   CNACHEN  $
//
// Description:
//   This file contains info for the aol password stealing trojan heuristics
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/aolpw.h_v  $
// 
//    Rev 1.2   05 Aug 1998 11:37:26   CNACHEN
// 
//    Rev 1.1   03 Aug 1998 16:43:30   CNACHEN
// 
//    Rev 1.0   03 Aug 1998 15:27:40   CNACHEN
// Initial revision.
// 
//    Rev 1.0   03 Aug 1998 15:27:22   CNACHEN
// Initial revision.
//
//***************************************************************************

#ifndef _AOLPW_H_

#define _AOLPW_H_

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

EXTSTATUS AOLPWBloodhound
(
    LPCALLBACKREV1  lpCallBack,
    HFILE           hFile,
    LPWININFO       lpstNEInfo,             // NE header
    DWORD           dwWinHeaderOffset,
    LPBYTE          lpbyWorkBuffer,
    LPWORD          lpwVID
);

EXTSTATUS LoadAOLData
(
    LPCALLBACKREV1  lpCallBack,
    LPTSTR          lpszNAVEXDataDir
);

EXTSTATUS FreeAOLData
(
    LPCALLBACKREV1  lpCallBack
);

EXTSTATUS ZeroAOLData
(
    void
);


typedef struct
{
    DWORD           dwVersion;
    DWORD           dwNumStrings;
    DWORD           dwNumProfiles;
} AOLPW_HEADER_T, FAR *LPAOLPW_HEADER;

#define AOLPW_REMAINING_SIG_LEN       27
#define AOLPW_TOTAL_SIG_LEN           (AOLPW_REMAINING_SIG_LEN + sizeof(WORD))

typedef struct
{
    WORD            wEncryptedSigLen;
    WORD            wFirstWORD;
    BYTE            byEncryptedSig[AOLPW_REMAINING_SIG_LEN];
    BYTE            bCleanFile;         // effectively a bool
} AOLPW_SIG_T, FAR *LPAOLPW_SIG;

#define MAX_STRINGS_PER_PROFILE         16

typedef struct
{
    WORD            wVID;
    WORD            wNumStringsToMatch;
    WORD            wIndexList[MAX_STRINGS_PER_PROFILE];
} AOLPW_PROFILE_T, FAR *LPAOLPW_PROFILE;

typedef struct
{
    DWORD           dwNumStrings;
    DWORD           dwNumProfiles;
    LPAOLPW_SIG     lpstSigArray;
    LPAOLPW_PROFILE lpstProfileArray;
} AOLPW_INFO_T, FAR *LPAOLPW_INFO;

#define AOLPW_SECTION_ID            11000

#define AOLPW_BUFFER_SIZE           16384

#define MAX_AOLPW_STRINGS           256
#define MAX_AOLPW_PROFILES          128

#define MODULE_TABLE_SIZE           64

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

#endif // #define _AOLPW_H_
