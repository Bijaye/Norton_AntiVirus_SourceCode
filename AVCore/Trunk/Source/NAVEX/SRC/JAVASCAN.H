//************************************************************************
//
// $Header:   S:/NAVEX/VCS/javascan.h_v   1.2   21 Nov 1996 17:13:10   AOONWAL  $
//
// Description:
//      Header file for Java scanning engine.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/javascan.h_v  $
// 
//    Rev 1.2   21 Nov 1996 17:13:10   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 13:00:12   AOONWAL
// No change.
// 
//    Rev 1.0   04 Apr 1996 14:54:24   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _JAVASCANH_

#define _JAVASCANH_

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "navexshr.h"

EXTSTATUS EXTScanFileJava
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
);

#endif

