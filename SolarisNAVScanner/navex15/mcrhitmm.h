//************************************************************************
//
// $Header:   S:/NAVEX/VCS/mcrhitmm.h_v   1.0   27 May 1997 16:45:44   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/mcrhitmm.h_v  $
// 
//    Rev 1.0   27 May 1997 16:45:44   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MCRHITMM_H_
#define _MCRHITMM_H_

#define HIT_MEM_WD7         0
#define HIT_MEM_WD8         1
#define HIT_MEM_XL97        2

#if 1

#define ALLOC_MACR_SIG_HIT_MEM

BOOL AllocMacroSigHitMem
(
    LPCALLBACKREV1      lpstCallBack,
    LPBYTE FAR *        lplpbyHitMem,
    int                 nType
);

void FreeMacroSigHitMem
(
    LPCALLBACKREV1      lpstCallBack,
    LPBYTE              lpbyHitMem
);

void AssignMacroSigHitMem
(
    LPBYTE              lpbyHitMem,
    LPWD_SCAN           lpstScan
);

#else

#define FreeMacroSigHitMem(a,b)

#endif

#endif // #ifndef _MCRHITMM_H_
