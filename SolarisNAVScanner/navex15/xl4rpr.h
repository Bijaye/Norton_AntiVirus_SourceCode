//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/XL4RPR.H_v   1.0   15 Apr 1998 16:46:10   DCHI  $
//
// Description:
//  Contains prototype for Excel 4.0 macro virus repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XL4RPR.H_v  $
// 
//    Rev 1.0   15 Apr 1998 16:46:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL4RPR_H_

#define _XL4RPR_H_

EXTSTATUS XL4Repair
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPBYTE                  lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO     lpstVirusSigInfo
);

#endif // _XL4RPR_H_

