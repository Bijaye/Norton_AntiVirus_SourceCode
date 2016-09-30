// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/scanboot.h_v   1.1   12 Jun 1996 12:16:18   RAY  $
//
// Description:
//  CMDLineScanBoot() prototype.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/scanboot.h_v  $
// 
//    Rev 1.1   12 Jun 1996 12:16:18   RAY
// Reorganized boot scanning code
// 
//    Rev 1.0   05 Jun 1996 21:01:50   RAY
// Initial revision.
// 
//************************************************************************

#ifndef _SCANBOOT_H

#define _SCANBOOT_H

#include "scanglob.h"

#define INVALID_DRIVE_NUM 0xFF

BOOL CMDLineScanBootArea
(
    LPSCANGLOBAL    lpScanGlobal
);

#endif  // _SCANBOOT_H

