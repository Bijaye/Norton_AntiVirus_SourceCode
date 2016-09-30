// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/navdini.c_v   1.0   06 Feb 1997 21:08:48   RFULLER  $
//
// Description:
//      These are the functions to load the NAV options structures.
//
// Contains:
//      LoadNoNavIni()
//
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/navdini.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:48   RFULLER
// Initial revision
// 
//    Rev 1.1   08 Aug 1996 13:27:58   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.14   29 Dec 1994 16:46:12   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.13   28 Dec 1994 14:15:46   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.1   05 Dec 1994 18:10:12   DALLEE
    // CVT1 script.
    //
//    Rev 1.12   28 Dec 1994 13:53:20   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:10   DALLEE
    // Initial revision.
    //
    //    Rev 1.10   18 Aug 1993 07:34:40   DALLEE
    // Initialize lpNetUserItem fiels in lpNoNav.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"

#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"

#include "navdprot.h"


//************************************************************************
// LoadNoNavIni()
//
// This routine loads all the options data from the .INI file to
// the NONAV structure pointed to by lpNoNav.
//
// Parameters:
//      LPNONAV lpNoNav                 Structure to load
//
// Returns:
//      Nothing.
//************************************************************************
// 3/17/93 DALLEE Function created.
//************************************************************************

VOID PASCAL LoadNoNavIni (LPNONAV lpNoNav)
{
    extern      NAVOPTIONS      navOptions;
                                        // Load all our standard options
                                        // structures.
    lpNoNav->Opt            = navOptions.scanner;
    lpNoNav->Gen            = navOptions.general;
    lpNoNav->Alert          = navOptions.alert;
    lpNoNav->Act            = navOptions.activity;
    lpNoNav->Inoc           = navOptions.inoc;

    lpNoNav->lpNetUserItem  = navOptions.netUser.lpNetUserItem;

    lpNoNav->lpExclude      = &navOptions.exclude;
} // End LoadNoNavIni()

