//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xl97eren.h_v   1.0   08 Sep 1998 17:06:08   DCHI  $
//
// Description:
//  Contains XL97 encrypted repair enablement checking prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl97eren.h_v  $
// 
//    Rev 1.0   08 Sep 1998 17:06:08   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL97EREN_H_

#define _XL97EREN_H_

EXTSTATUS InitXL97EncRepEnableState
(
    LPCALLBACKREV1  lpstCallBacks,
    LPTSTR          lpszNAVEXINIFile
);

#if defined(SYM_DOSX) || defined(SYM_WIN16)

extern BOOL gbCheckedXL97EncRepEnableState;

BOOL GetXL97EncRepEnableState
(
    LPCALLBACKREV1  lpstCallBacks
);

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

#endif // #ifndef _XL97EREN_H_
