// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/navderr.h_v   1.0   06 Feb 1997 20:51:30   RFULLER  $
//
// Description:
//      This header contains all the error level returns for NAVBOOT.
//
// See Also:
//      #defines used to be in NAVDPROT.H; moved them here so they could
//      be included for the NAVBOOT stub without requiring platform.h et al.
//************************************************************************
// $Log:   S:/include/VCS/navderr.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:30   RFULLER
// Initial revision
// 
//    Rev 1.0   07 Aug 1996 13:59:14   JBRENNA
// Initial revision.
// 
//    Rev 1.0   10 Jul 1996 18:35:20   DALLEE
// Initial revision.
//************************************************************************

#ifndef __NAVDERR_H
#define __NAVDERR_H


// These are the DOS ERRORLEVEL return values for NAVD:
#define ERRORLEVEL_NONE         ((BYTE)0)
#define ERRORLEVEL_VIRUSFOUND   ((BYTE)1)   // Virus found (at any time)
#define ERRORLEVEL_SELFCHECK    ((BYTE)2)   // Self-check failed on startup
#define ERRORLEVEL_DOSAPERROR   ((BYTE)3)   // Unable to execute DOS AP
                                            // (NAVBOOT only)
#define ERRORLEVEL_EXECUTETSR   ((BYTE)100) // "Launch TSR" return code
                                            // (NAVBOOT only)
#define ERRORLEVEL_VIRUSFOUND_EXECUTETSR ((BYTE)101) // Virus found, plus the
                                            // NAVBOOT.EXE stub should launch
                                            // the DOS TSR. (NAVBOOT.OVL only -
                                            // NAVBOOT.EXE will translate to
                                            // VIRUSFOUND on exit)
#define ERRORLEVEL_INTERRUPT    ((BYTE)254) // Scan interrupted by user
                                            // (applies to last scan only)
#define ERRORLEVEL_OTHER        ((BYTE)255) // Incompatible config
                                            // (XT, DOS2, out of memory, etc.)

#endif  // __NAVDERR_H
