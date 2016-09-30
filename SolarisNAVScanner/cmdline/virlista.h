// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/virlista.h_v   1.0   21 May 1996 12:29:14   DCHI  $
//
// Description:
//  VirusListDisplay() and VirusListDeleteVirusVID() function prototypes.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/virlista.h_v  $
// 
//    Rev 1.0   21 May 1996 12:29:14   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _VIRLISTA_H

#define _VIRLISTA_H

#include "scanglob.h"

void VirusListDisplay
(
    LPSCANGLOBAL        lpScanGlobal
);

void VirusListDeleteVirusVID
(
    LPSCANGLOBAL        lpScanGlobal
);

#endif  // _VIRLISTA_H
