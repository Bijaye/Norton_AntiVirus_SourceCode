//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDAPSIG.H_v   1.0   17 Jan 1997 11:24:50   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDAPSIG.H_v  $
// 
//    Rev 1.0   17 Jan 1997 11:24:50   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDAPSIG_H_

#define _WDAPSIG_H_

WD_STATUS WDApplySig
(
    LPBYTE              lpabySig,
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPBOOL              lpbHit
);

#endif // #ifndef _WDAPSIG_H_

