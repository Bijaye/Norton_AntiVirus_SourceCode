//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xl97rpr.h_v   1.0   08 Sep 1998 17:08:56   DCHI  $
//
// Description:
//  Contains Excel 97 macro repair function code header.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl97rpr.h_v  $
// 
//    Rev 1.0   08 Sep 1998 17:08:56   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL97RPR_H_

#define _XL97RPR_H_

BOOL XL97EncryptedRepairCheck
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBOOL              lpbEncrypted
);

BOOL XL97ReplaceVBASrcWithMsg
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT
);

BOOL XL97RepairEpilogue
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPLZNT              lpstLZNT,
    BOOL                bEncrypted
);

EXTSTATUS XL97MacroVirusRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
);

EXTSTATUS XL97DoFullSetRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer
);

#endif // #ifndef _XL97RPR_H_

