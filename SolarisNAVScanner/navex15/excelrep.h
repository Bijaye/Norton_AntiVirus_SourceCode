//************************************************************************
//
// $Header:   S:/NAVEX/VCS/excelrep.h_v   1.5   29 Jan 1998 19:04:38   DCHI  $
//
// Description:
//      Contains Excel macro repair function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/excelrep.h_v  $
// 
//    Rev 1.5   29 Jan 1998 19:04:38   DCHI
// Added support for Excel formula virus scan and repair.
// 
//    Rev 1.4   09 Jul 1997 16:46:38   DCHI
// Change to MCRENG15.
// 
//    Rev 1.3   21 Nov 1996 17:14:20   AOONWAL
// No change.
// 
//    Rev 1.2   29 Oct 1996 13:00:24   AOONWAL
// No change.
// 
//    Rev 1.1   28 Aug 1996 16:11:04   DCHI
// Changed comment on required work buffer size.
// 
//    Rev 1.0   05 Aug 1996 10:39:36   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _EXCELREP_H_

#define _EXCELREP_H_

WORD ExcelBooksRemoveModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPBYTE              lpbyWorkBuffer,         // Work buffer >= 1024 bytes
    LPBYTE              lppsModuleName,         // Pascal string of module
    BOOL                bSingleSheetCase        // Single sheet case?
);

EXTSTATUS ExcelMacroVirusRepair
(
    LPMSOFFICE_SCAN lpstOffcScan,
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
	WORD 			wVID,					// ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
    HFILE           hFile,                  // Handle to file to repair
    UINT            uAttr,
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 1024 bytes
);

#endif // _EXCELREP_H_

