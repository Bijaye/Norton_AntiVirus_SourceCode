//************************************************************************
//
// $Header:   S:/NAVEX/VCS/NEPESCAN.H_v   1.4   12 Oct 1998 10:59:42   CNACHEN  $
//
// Description:
//      Header file for Windows scanning engine.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/NEPESCAN.H_v  $
// 
//    Rev 1.4   12 Oct 1998 10:59:42   CNACHEN
// Added support for AOL pws heuristics.
// 
// 
//    Rev 1.3   27 Jul 1998 11:40:30   DKESSNE
// added prototype for EXTPELocateSectionwithRVA
// 
//    Rev 1.2   21 Nov 1996 17:14:34   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:59:50   AOONWAL
// No change.
// 
//    Rev 1.0   05 Feb 1996 17:23:26   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _NEPESCANH_

#define _NEPESCANH_

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "navexshr.h"
#include "winconst.h"

EXTSTATUS EXTScanFileWindows
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD          lpwVID,                 // Virus ID storage on hit
    LPWININFO       lpstNEHeader,           // Filled in for NE files
    LPDWORD         lpdwNEHeaderOffset      // 0 for non-NE, non-zero for valid NE
);

EXTSTATUS EXTPELocateSectionWithRVA
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS lpstPEInfo,             // PE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
	DWORD			dwRVA,					// RVA to search for
    LPDWORD         lpdwOffset,         	// where is this section from TOF?
    LPDWORD         lpdwSecLen,             // how long is this section?
    LPDWORD         lpdwFlags,              // section flags
	LPDWORD			lpdwRVABase             // RVA in memory
);

#endif

