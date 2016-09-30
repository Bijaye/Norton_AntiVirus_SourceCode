// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1999 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
// NavLu.h - created 6/10/99 7:35:09 PM
//
// $Header$
//
// Description:
//      External header for Iliad's LiveUpdate helper DLL. NAVLU.DLL
//
// Contains:
//      DoLiveUpdate()
//*************************************************************************
// $Log$
//*************************************************************************

#ifndef __NAVLU_H
#define __NAVLU_H

#ifdef  BUILDING_NAVLU
  #define NAVLUAPI __declspec( dllexport )
#else
  #define NAVLUAPI __declspec( dllimport )
#endif

#include "vpcommon.h"

// DoLiveUpdate() flags

#define NAVLU_SILENT         0x000000001

// DoLiveUpdateEx() return values

#define NAVLURET_SUCCESS	     0x000000000
#define	NAVLURET_INVALID_LIC     0x000000001
#define	NAVLURET_ALREADY_RUNNING 0x000000002
#define NAVLURET_ERROR           0x000000003

//*************************************************************************
// LOGCALLBACKFN
//
// Function pointer type for log events.
//*************************************************************************

typedef DWORD (WINAPI *LOGCALLBACKFN)( EVENTBLOCK* );

//*************************************************************************
// LICENSECALLBACKFN
//
// Function pointer type for checking for licenses.
//*************************************************************************

typedef BOOL (WINAPI *LICENSECALLBACKFN)( BOOL silent_mode, BOOL* abort );

#ifdef __cplusplus
extern "C" {
#endif

//*************************************************************************
// NAVLUAPI WORD WINAPI DoLiveUpdate (
//      const char* lpszzPackageTypeFilters, Double NULL terminated list of filter strings.
//                                            These are PackageType's listed in the
//                                            CommandLines section of NAV's registry which
//                                            should NOT be requested.
//      DWORD dwFlags,                       Flags NAVLU_xxx from NAVLU.H
//      LICENSECALLBACKFN fnCheckLicense     Ptr to function for calling back to check for a license.
//
// Command lines requested should be in the registry under [NAV main key]\LiveUpdate\CmdLines.
//
// Returns:
//      DWORD    LOWORD is the LiveUpdate result code, see LiveUpdate.h
//               HIWORD is one of the NAVLURET_ values from NavLu.h
//*************************************************************************

NAVLUAPI DWORD WINAPI DoLiveUpdate ( const char* lpszzPackageTypeFilters, DWORD dwFlags, LICENSECALLBACKFN fnCheckLicense );

#ifdef __cplusplus
}
#endif

#endif // __NAVLU_H

