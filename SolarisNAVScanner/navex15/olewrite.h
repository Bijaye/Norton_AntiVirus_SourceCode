//************************************************************************
//
// $Header:   S:/NAVEX/VCS/olewrite.h_v   1.2   21 Nov 1996 17:13:50   AOONWAL  $
//
// Description:
//      Contains OLE 2 Storage Object write function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/olewrite.h_v  $
// 
//    Rev 1.2   21 Nov 1996 17:13:50   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 13:00:18   AOONWAL
// No change.
// 
//    Rev 1.0   03 Jan 1996 17:12:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _OLEWRITEH_

#define _OLEWRITEH_

#include "storobj.h"

WORD OLESeekWrite
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
);

#endif
