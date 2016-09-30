//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/xl5rpr.h_v   1.1   09 Sep 1998 12:45:36   DCHI  $
//
// Description:
//  Contains prototype for Excel 5.0/95 macro virus repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl5rpr.h_v  $
// 
//    Rev 1.1   09 Sep 1998 12:45:36   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.0   15 Apr 1998 16:46:12   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL5RPR_H_

#define _XL5RPR_H_

BOOL XL5EncryptedRepairCheck
(
    LPMSOFFICE_SCAN     lpstOffcScan
);

EXTSTATUS XL5Repair
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPBYTE                  lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO     lpstVirusSigInfo
);

#endif // _XL5RPR_H_

