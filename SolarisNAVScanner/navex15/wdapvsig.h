//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDAPVSIG.H_v   1.1   23 Jan 1997 11:09:28   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDAPVSIG.H_v  $
// 
//    Rev 1.1   23 Jan 1997 11:09:28   DCHI
// Fixed global variable usage problem.
// 
//    Rev 1.0   17 Jan 1997 11:24:52   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDAPVSIG_H_

#define _WDAPVSIG_H_

WD_STATUS WDApplyVirusSig
(
    LPBYTE              lpabySig,
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPBYTE              lpbyStack,
    LPWORD              lpwID
);

WD_STATUS WDApplyUseSigRepair
(
    LPBYTE              lpabySig,
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
);

#endif // #ifndef _WDAPVSIG_H_

