// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/scaninit.h_v   1.0   21 May 1996 12:28:08   DCHI  $
//
// Description:
//  CMDLineScanInit() and CMDLineScanClose() prototypes.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/scaninit.h_v  $
// 
//    Rev 1.0   21 May 1996 12:28:08   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _SCANINIT_H

#define _SCANINIT_H

#include "scanglob.h"

BOOL CMDLineScanInit
(
	LPSCANGLOBAL	lpScanGlobal
);

BOOL CMDLineScanClose
(
	LPSCANGLOBAL	lpScanGlobal
);

#endif  // _SCANINIT_H

