// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/READSIG.H_v   1.0   28 Feb 1997 11:33:36   MKEATIN  $
//
// Description:  Header for DCHI new memory scanning code. Scans with
//               INSCANM.DAT files that are larger than 64k.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/READSIG.H_v  $
// 
//    Rev 1.0   28 Feb 1997 11:33:36   MKEATIN
// Initial revision.
//************************************************************************

#ifndef _READSIG_H_

#define _READSIG_H_

#include "scanmem.h"

LPMEMSIGS VLScanMemGetScanData
(
    HVCONTEXT           hContext,
    HFILE               hFile
);

void VLScanMemFree
(
    HVCONTEXT           hContext,
    LPMEMSIGS           lpMemSigs
);

#endif
