//************************************************************************
//
// $Header:   S:/NAVEX/VCS/XL4MREP.H_v   1.0   29 Jan 1998 19:05:16   DCHI  $
//
// Description:
//   Contains Excel 5.0/95/97 Excel 4.0 macro repair function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XL4MREP.H_v  $
// 
//    Rev 1.0   29 Jan 1998 19:05:16   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL4MREP_H_

#define _XL4MREP_H_

EXTSTATUS Excel4MacroVirusRepair
(
    LPMSOFFICE_SCAN lpstOffcScan,
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    WORD            wVID,                   // ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
    HFILE           hFile,                  // Handle to file to repair
    UINT            uAttr,
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 1024 bytes
);

#endif // #ifndef _XL4MREP_H_

