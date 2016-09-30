// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/sigex.h_v   1.5   03 Nov 1997 16:00:36   CNACHEN  $
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
// $Log:   S:/INCLUDE/VCS/sigex.h_v  $
// 
//    Rev 1.5   03 Nov 1997 16:00:36   CNACHEN
// Added FAR keywords to pointers.
// 
//    Rev 1.4   23 May 1997 18:14:16   CNACHEN
// Added new CRC support.
// 
//    Rev 1.3   20 Nov 1996 15:23:24   AOONWAL
// No change.
// 
//    Rev 1.2   22 Oct 1996 12:04:34   AOONWAL
// No change.
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
    // standard wildcard sigs

    SigType FAR *lpSigPtr;
    WORD    wNumSigs;

    // standard CRC32s

    WORD    wNumCRCs;
    DWORD FAR  *lpdwCRCs;

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

