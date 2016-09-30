//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wd7heusc.h_v   1.0   09 Jul 1997 16:23:58   DCHI  $
//
// Description:
//      Header for Word 6.0/7.0 heuristic macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wd7heusc.h_v  $
// 
//    Rev 1.0   09 Jul 1997 16:23:58   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WD7HEUSC_H_

#define _WD7HEUSC_H_

EXTSTATUS WD7DoHeuristicScanRepair
(
    LPSS_STREAM             lpstStream,
    LPWD7ENCKEY             lpstKey,
    BOOL                    bRepair
);

EXTSTATUS WD7HeuristicScan
(
    LPMSOFFICE_SCAN         lpstOffcScan
);

#endif // #ifndef _WD7HEUSC_H_

