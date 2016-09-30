// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/navdini.c_v   1.0   06 Feb 1997 20:56:30   RFULLER  $
//
// Description:
//      These are the functions to load the NAV options structures.
//
// Contains:
//      LoadNoNavIni()
//
// See Also:
//************************************************************************
// $Log:   S:/navc/VCS/navdini.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:30   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:34   MKEATIN
// Initial revision.
// 
//    Rev 1.0   02 Oct 1996 12:59:00   JBELDEN
// Initial revision.
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

