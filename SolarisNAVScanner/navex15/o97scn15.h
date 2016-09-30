//************************************************************************
//
// $Header:   S:/NAVEX/VCS/o97scn15.h_v   1.0   09 Jul 1997 16:18:00   DCHI  $
//
// Description:
//      Header for VBA 5 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/o97scn15.h_v  $
// 
//    Rev 1.0   09 Jul 1997 16:18:00   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _O97SCN15_H_

#define _O97SCN15_H_

EXTSTATUS VBA5Scan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo,
    LPBOOL                  lpbMVPApproved
);

#endif // #ifndef _O97SCN15_H_

