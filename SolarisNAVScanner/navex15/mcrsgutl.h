//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRSGUTL.H_v   1.3   15 Dec 1998 12:13:46   DCHI  $
//
// Description:
//      Contains prototypes for signature hit array allocation functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRSGUTL.H_v  $
// 
//    Rev 1.3   15 Dec 1998 12:13:46   DCHI
// Added extern declaration for P97 signature set.
// 
//    Rev 1.2   11 May 1998 18:04:36   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.1   15 Apr 1998 17:23:50   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.0   09 Jul 1997 16:25:56   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MCRSGUTL_H_

#define _MCRSGUTL_H_

extern WD_SIG_SET_T gstWD7SigSet;
extern WD_SIG_SET_T gstWD8SigSet;
extern WD_SIG_SET_T gstXL97SigSet;
extern WD_SIG_SET_T gstXL95SigSet;
extern WD_SIG_SET_T gstXL4SigSet;
extern WD_SIG_SET_T gstA2SigSet;
extern WD_SIG_SET_T gstA97SigSet;
extern WD_SIG_SET_T gstP97SigSet;

BOOL AllocMacroSigHitMem
(
    LPVOID                  lpvRootCookie,
    LPMSOFFICE_SCAN         lpstMSOfficeScan
);

BOOL FreeMacroSigHitMem
(
    LPVOID                  lpvRootCookie,
    LPMSOFFICE_SCAN         lpstMSOfficeScan
);

BOOL AssignMacroSigHitMem
(
    LPMSOFFICE_SCAN         lpstMSOfficeScan,
    LPWD_SCAN               lpstScan
);

#endif // #ifndef _MCRSGUTL_H_
