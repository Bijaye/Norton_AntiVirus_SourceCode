//************************************************************************
//
// $Header:   S:/NAVEX/VCS/VBA5REP.H_v   1.2   08 May 1997 15:36:26   DCHI  $
//
// Description:
//      Contains Word 8.0/Excel 97 macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/VBA5REP.H_v  $
// 
//    Rev 1.2   08 May 1997 15:36:26   DCHI
// Added office 97 conversion of template to document and added criteria
// for absence of AutoText before converting to document for both Word versions.
// 
//    Rev 1.1   08 Apr 1997 12:40:44   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.0   14 Mar 1997 16:33:06   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _VBA5REP_H_

#define _VBA5REP_H_

BOOL WD8HasAutoText
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for WordDocument stream
    LPBOOL              lpbHasAutoText
);

BOOL WD8ChangeToDocument
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile         // OLE file for WordDocument stream
);

EXTSTATUS WD8MacroVirusRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,   // Virus sig info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
);

EXTSTATUS XL97MacroVirusRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,   // Virus sig info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
);

EXTSTATUS WD8DoFullSetRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
);

EXTSTATUS XL97DoFullSetRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
);

#endif // #ifndef _VBA5REP_H_

