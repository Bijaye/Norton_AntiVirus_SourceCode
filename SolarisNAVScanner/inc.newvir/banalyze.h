// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/banalyze.h_v   1.0   14 May 1997 16:49:30   CNACHEN  $
//
// Description:
//      Behavior Analyzer data and prototypes.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/banalyze.h_v  $
// 
//    Rev 1.0   14 May 1997 16:49:30   CNACHEN
// Initial revision.
// 
//************************************************************************


#ifndef _BA_H_

#define _BA_H_

#include "behav.h"

BOOL BADetectVirus
(
    WORD                    wHeurStreamLen,
    LPBYTE                  lpbyHeurCode,
    LPBEHAVE_MONITOR        pstBehavior,
    LPBOOL                  lpbFoundVirus,
    LPWORD                  lpwVirusID
);

#endif 
