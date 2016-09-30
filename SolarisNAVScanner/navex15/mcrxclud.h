// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRXCLUD.H_v   1.1   04 Aug 1998 13:54:02   DCHI  $
//
// Description:
//  This source file contains routines for checking to see whether
//  a macro should be excluded from heuristic emulation.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRXCLUD.H_v  $
// 
//    Rev 1.1   04 Aug 1998 13:54:02   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.0   15 Oct 1997 13:20:42   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MCRXCLUD_H_

#define MACRO_EXCLUDE_TYPE_WD7      0
#define MACRO_EXCLUDE_TYPE_WD8      1
#define MACRO_EXCLUDE_TYPE_X95      2
#define MACRO_EXCLUDE_TYPE_X97      3

#define NUM_MACRO_EXCLUSION_SETS    4

typedef struct tagMACRO_EXCLUSION_SET
{
    DWORD           dwBaseID;
    DWORD           dwCount;
    LPDWORD         lpadwCRCs;
    LPDWORD         lpadwCRCByteCounts;
} MACRO_EXCLUSION_SET_T, FAR *LPMACRO_EXCLUSION_SET;

extern MACRO_EXCLUSION_SET_T gastMacroExclusionSets[NUM_MACRO_EXCLUSION_SETS];

BOOL MacroIsExcluded
(
    int             nExcludeType,
    DWORD           dwCRC,
    DWORD           dwCRCByteCount
);

#endif // #ifndef _MCRXCLUD_H_

