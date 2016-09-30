//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/exclscan.h_v   1.3   21 Nov 1996 17:14:12   AOONWAL  $
//
// Description:
//      Contains prototype for Excel macro virus scanning.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/exclscan.h_v  $
// 
//    Rev 1.3   21 Nov 1996 17:14:12   AOONWAL
// No change.
// 
//    Rev 1.2   29 Oct 1996 13:00:24   AOONWAL
// No change.
// 
//    Rev 1.1   28 Aug 1996 16:13:08   DCHI
// Changed comment on required work buffer size.
// 
//    Rev 1.0   25 Jul 1996 18:12:50   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _EXCLSCAN_H_

#define _EXCLSCAN_H_

EXTSTATUS EXTScanFileExcelMacro
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE          lpbyWorkBuffer,         // Work buffer >= 1024 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
);

#endif // _EXCLSCAN_H_

