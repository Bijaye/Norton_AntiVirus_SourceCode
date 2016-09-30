//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/XL4SCN.H_v   1.0   15 Apr 1998 16:46:22   DCHI  $
//
// Description:
//      Contains prototype for Excel macro virus scanning.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XL4SCN.H_v  $
// 
//    Rev 1.0   15 Apr 1998 16:46:22   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL4SCN_H_

#define _XL4SCN_H_

BOOL XL4ScanSheet
(
    WORD                    wType,
    LPWD_SCAN               lpstScan,
    DWORD                   dwOffset,
    XL_REC_HDR_T            stRec,
    LPXL_REC_BOUNDSHEET     lpstBoundSheet
);

EXTSTATUS XL4Scan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
);

#endif // _XL4SCN_H_

