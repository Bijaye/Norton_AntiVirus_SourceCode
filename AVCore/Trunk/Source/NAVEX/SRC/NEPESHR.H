//************************************************************************
//
// $Header:   S:/NAVEX/VCS/nepeshr.h_v   1.2   21 Nov 1996 17:14:48   AOONWAL  $
//
// Description:
//      Header for Windows scanner/repair shared functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/nepeshr.h_v  $
// 
//    Rev 1.2   21 Nov 1996 17:14:48   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:59:52   AOONWAL
// No change.
// 
//    Rev 1.0   08 Feb 1996 10:39:32   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _NEPESHRH_

#define _NEPESHRH_

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "navexshr.h"

WORD DetermineWindowsType
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE          lpbyWorkBuffer,         // Work buffer >= 512 bytes
    LPDWORD         lpdwWinHeaderOffset     // Offset of WIN header from TOF
);

EXTSTATUS GetPESectionInfo
(
    LPCALLBACKREV1  	lpCallBack,             // File op callbacks
    HFILE          		hFile,					// Handle to file to scan
    LPBYTE         		lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           	dwWinHeaderOffset,      // where's the WIN header start?
    DWORD            	dwSecNum,               // what section are we
												// 	interested in (0-base)?
    LPDWORD         	lpdwFileOffset,         // where is this section from TOF?
    LPDWORD         	lpdwSecLen,             // how long is this section?
    LPDWORD          	lpdwFlags,              // section flags
	LPDWORD				lpdwRVABase             // RVA in memory
);

#endif
