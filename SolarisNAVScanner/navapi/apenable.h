// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/apenable.h_v   1.0   22 Jun 1998 22:28:00   DALLEE  $
//
// Description:
//      Header for enabling/disabling AP routines.
//
// Contents:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/apenable.h_v  $
// 
//    Rev 1.0   22 Jun 1998 22:28:00   DALLEE
// Initial revision.
//************************************************************************

#ifndef __APENABLE_H
#define __APENABLE_H

// For internal use in NAVAPI

// NavapCommInit() should be called during NAVEngineInit().
// A matching call to NavapCommDeinit() should be made in NAVEngineClose().

// NavapUnprotectProcess() will be called once in NAVEngineInit() and
// once on each call to a scanning or repair function.

BOOL NavapCommInit();
BOOL NavapCommDeinit();

VOID NavapProtectProcess();
VOID NavapUnprotectProcess();

#endif


