// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xlheusc.H_v   1.0   04 Aug 1998 11:18:32   DCHI  $
//
// Description:
//  Contains top-level Excel 5.0/95/97 heuristic macro scan header.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xlheusc.H_v  $
// 
//    Rev 1.0   04 Aug 1998 11:18:32   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XLHEUSC_H_

#define _XLHEUSC_H_

EXTSTATUS XL5HeuristicScan
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    BOOL                bRepair
);

EXTSTATUS XL97HeuristicScan
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    BOOL                bRepair
);

#endif // #ifndef _XLHEUSC_H_

