// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/SIGEX.H_v   1.1   06 Sep 1996 14:04:26   CNACHEN  $
//
// Description:
//
//  Contains data structures and constants used in the PAM signature
//  exclusion system.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/SIGEX.H_v  $
// 
//    Rev 1.1   06 Sep 1996 14:04:26   CNACHEN
// Updated header to use proper #pragma pack() for NTK/WIN32.
// 
//    Rev 1.1   06 Sep 1996 14:01:14   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.0   01 Feb 1996 10:17:46   CNACHEN
// Initial revision.
// 
//    Rev 1.4   23 Oct 1995 12:51:00   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.3   19 Oct 1995 18:39:24   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************



/* only include "types.h" until ported over to Quake */

#define SIG_EXCLUDE_LEN 32
#define WILD_WORD       0xFFFF

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif


typedef struct
{
    WORD    wSigLen;
    WORD    wSig[SIG_EXCLUDE_LEN];
} SigType;

#define sSIGTYPE        sizeof(SigType)

typedef struct
{
    SigType *lpSigPtr;
    WORD    wNumSigs;
} SigList;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

typedef SigList *ExcludeContext;

typedef unsigned int ESTATUS;

#define ESTATUS_OK                  0
#define ESTATUS_FILE_ERROR          1
#define ESTATUS_MEM_ERROR           2
#define ESTATUS_NEED_TO_SCAN        3
#define ESTATUS_NO_SCAN             4

