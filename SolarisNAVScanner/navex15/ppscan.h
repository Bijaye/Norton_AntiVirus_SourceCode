//************************************************************************
//
// $Header:   S:/NAVEX/VCS/ppscan.h_v   1.0   26 Nov 1997 18:37:12   DCHI  $
//
// Description:
//  Contains header for PowerPoint macro scanner.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/ppscan.h_v  $
// 
//    Rev 1.0   26 Nov 1997 18:37:12   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _PPSCAN_H_

#define _PPSCAN_H_

EXTSTATUS PPScan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    BOOL                    bRepair,
    LPBYTE                  lpbyWorkBuffer,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo,
    LPBOOL                  lpbMVPApproved
);

#endif // #ifndef _PPSCAN_H_

