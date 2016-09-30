//************************************************************************
//
// $Header:   S:/NAVEX/VCS/o97rpr15.h_v   1.1   09 Sep 1998 12:45:32   DCHI  $
//
// Description:
//      Contains Word 8.0/Excel 97 macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/o97rpr15.h_v  $
// 
//    Rev 1.1   09 Sep 1998 12:45:32   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.0   09 Jul 1997 16:19:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _O97RPR15_H_

#define _O97RPR15_H_

EXTSTATUS WD8MacroVirusRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
);

BOOL VBA5IsFullSet
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPWD_SIG_SET        lpstSigSet,         // Signature set to apply
    LPWD_SCAN           lpstScan,           // Ptr to scan structure
    DWORD               dwVBAChildEntry,    // Child of VBA storage
    LPSS_ENUM_SIBS      lpstEnumSibs
);

EXTSTATUS WD8DoFullSetRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer
);

#endif // #ifndef _O97RPR15_H_

