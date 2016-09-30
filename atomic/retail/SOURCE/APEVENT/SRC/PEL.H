// Copyright 1995 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/PEL.H_v   1.3   08 Jul 1998 15:51:58   RStanev  $
//
// Description:
//      This file declares types and function prototypes for working with
//      the Process Exclusion List (PEL).  Some of these APIs are exported
//      to other NAVAP modules.
//
// Contains:
//      PELInit()
//      PELDeInit()
//
//      AddCurrentThreadToPEL1()
//      ForceCurrentThreadIntoPEL1()
//      IsCurrentThreadInPEL1()
//      DeleteCurrentThreadFromPEL1()
//
//      AddCurrentProcessToPEL2()
//      GetCurrentProcessPEL2Count()
//      DeleteCurrentProcessFromPEL2()
//
// See Also:
//      pel.cpp, navap.h
//
// Note:
//      Event manager module
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/PEL.H_v  $
// 
//    Rev 1.3   08 Jul 1998 15:51:58   RStanev
// Prototyped ForceCurrentThreadIntoPEL1().
//
//    Rev 1.2   17 Feb 1998 16:42:58   RStanev
// Updated copyright.  Cleaned up.
//
//    Rev 1.1   15 May 1997 00:57:52   RStanev
// Cleaned up.
//
//    Rev 1.0   26 Feb 1997 04:34:24   RSTANEV
// Initial revision.
//
//***************************************************************************

#ifndef _PEL_H_
#define _PEL_H_

//---------------------------------------------------------------------------
//
// Define PEL function prototypes.
//
//---------------------------------------------------------------------------
BOOL PELInit (
    VOID
    );

BOOL PELDeInit (
    VOID
    );

BOOL AddCurrentThreadToPEL1 (
    VOID
    );

BOOL ForceCurrentThreadIntoPEL1 (
    VOID
    );

BOOL IsCurrentThreadInPEL1 (
    VOID
    );

VOID DeleteCurrentThreadFromPEL1 (
    VOID
    );

BOOL AddCurrentProcessToPEL2 (
    VOID
    );

int GetCurrentProcessPEL2Count (
    VOID
    );

VOID DeleteCurrentProcessFromPEL2 (
    VOID
    );

#endif // _PEL_H_
