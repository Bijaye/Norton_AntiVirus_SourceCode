//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRHEUEN.H_v   1.0   05 Nov 1997 12:00:46   DCHI  $
//
// Description:
//  Contains macro heuristics enablement checking prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRHEUEN.H_v  $
// 
//    Rev 1.0   05 Nov 1997 12:00:46   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MCRHEUEN_H_

#define _MCRHEUEN_H_

EXTSTATUS InitMacroHeurEnableState
(
    LPCALLBACKREV1  lpstCallBacks,
    LPTSTR          lpszNAVEXINIFile
);

#if defined(SYM_DOSX) || defined(SYM_WIN16)

extern BOOL gbCheckedMacroHeurEnableState;

BOOL GetMacroHeurEnableState
(
    LPCALLBACKREV1  lpstCallBacks
);

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

#endif // #ifndef _MCRHEUEN_H_
