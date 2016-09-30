//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/XLRPRSHT.H_v   1.0   15 Apr 1998 16:46:08   DCHI  $
//
// Description:
//  Contains prototype for Excel sheet repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XLRPRSHT.H_v  $
// 
//    Rev 1.0   15 Apr 1998 16:46:08   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XLRPRSHT_H_

#define _XLRPRSHT_H_

EXTSTATUS XLRepairSheets
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPSS_STREAM             lpstStream,
    LPWD_SIG_SET            lpstSigSet,
    LPWD_VIRUS_SIG_INFO     lpstVirusSigInfo
);

#endif // _XLRPRSHT_H_

