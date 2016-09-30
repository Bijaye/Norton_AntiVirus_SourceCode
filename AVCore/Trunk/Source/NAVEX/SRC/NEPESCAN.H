//************************************************************************
//
// $Header:   S:/NAVEX/VCS/nepescan.h_v   1.2   21 Nov 1996 17:14:34   AOONWAL  $
//
// Description:
//      Header file for Windows scanning engine.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/nepescan.h_v  $
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

EXTSTATUS EXTScanFileWindows
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
);

#endif

