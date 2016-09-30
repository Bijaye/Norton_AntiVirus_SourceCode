//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDREPAIR.H_v   1.2   08 Apr 1997 12:40:54   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDREPAIR.H_v  $
// 
//    Rev 1.2   08 Apr 1997 12:40:54   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.1   14 Mar 1997 16:33:38   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.0   17 Jan 1997 11:25:02   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDREPAIR_H_

#define _WDREPAIR_H_

WD_STATUS WDApplyRepair
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPBOOL              lpbDelete,
    WORD                wID,
    int                 nDepth
);

WD_STATUS WDRepairDoc
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
);

BOOL WDIsFullSetRepair
(
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
);

WD_STATUS WDDoFullSetRepair
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
);

#endif // #ifndef _WDREPAIR_H_

