//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRHEUSC.H_v   1.1   10 Jun 1998 13:20:46   DCHI  $
//
// Description:
//      Contains Microsoft Office heuristic macro scan function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRHEUSC.H_v  $
// 
//    Rev 1.1   10 Jun 1998 13:20:46   DCHI
// Added W7H2
// 
//    Rev 1.0   09 Jul 1997 16:16:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MCRHEUSC_H_

#define _MCRHEUSC_H_

EXTSTATUS MSOfficeHeuristicScan
(
    LPMSOFFICE_SCAN         lpstMSOfficeScan,
    LPWORD                  lpwHeuristicVID
);

#endif // #ifndef _MCRHEUSC_H_

