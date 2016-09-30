//************************************************************************
//
// $Header:   S:/NAVEX/VCS/nvx15inf.h_v   1.1   15 Dec 1998 12:16:50   DCHI  $
//
// Description:
//  Contains NAVEX15.INF reader prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/nvx15inf.h_v  $
// 
//    Rev 1.1   15 Dec 1998 12:16:50   DCHI
// Added NAVEXInfCheck() prototype.
// 
//    Rev 1.0   08 Dec 1998 12:53:20   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _NVX15INF_H_

#define _NVX15INF_H_

BOOL NAVEX15InfCheck
(
    LPCALLBACKREV1  lpstCallBacks,
    LPTSTR          lpszNAVEXINIFile
);

#if !defined(NAVEX15) && (defined(SYM_DOSX) || defined(SYM_WIN16))

extern BOOL gbCheckedMacroHeurEnableState;

BOOL GetMacroHeurEnableState
(
    LPCALLBACKREV1  lpstCallBacks
);

extern BOOL gbCheckedXL97EncRepEnableState;

BOOL GetXL97EncRepEnableState
(
    LPCALLBACKREV1  lpstCallBacks
);

#endif // #if !defined(NAVEX15) && (defined(SYM_DOSX) || defined(SYM_WIN16))

#if !defined(NAVEX15) && defined(SYM_WIN)

BOOL NAVEXInfCheck
(
    void
);

#endif // #if !defined(NAVEX15) && defined(SYM_WIN)

#endif // #ifndef _NVX15INF_H_
