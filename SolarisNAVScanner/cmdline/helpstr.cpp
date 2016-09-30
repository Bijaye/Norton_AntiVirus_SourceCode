// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/helpstr.cpv   1.2   12 Jun 1996 12:22:18   RAY  $
//
// Description:
//  Options help strings
//
// Contains:
//  HelpStrings[]
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/helpstr.cpv  $
// 
//    Rev 1.2   12 Jun 1996 12:22:18   RAY
// Added /NOFILE switch
// 
//    Rev 1.1   04 Jun 1996 18:14:50   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.0   22 May 1996 11:05:52   DCHI
// Initial revision.
// 
//************************************************************************

#include "avtypes.h"

LPSTR    HelpStrings[]    = {
"",
"CMDLINE [pathname] [options]",
"",
"   /DELETE                 Delete all infected files",
"   /DOALL                  Scan all files for viruses",
"   /HELP                   Displays this help information",
"   /MEMAVAIL               Display amount of memory available at startup",
"   /NOBOOT                 Skip boot record scan",
"   /NOMEM                  Skip Memory Scan",
"   /NOPART                 Skip Master Boot Record scan",
"   /NOFILE                 Skip File Scan",
"   /NOPOLY                 Disable polymorphic virus scanning",
"   /REPAIR                 Attempt to repair all infected files/boot areas",
"   /REPORT=FILENAME.EXT    Store report information to output file",
"   /REMVIR=VID#            Delete a virus signature (VID# is in hex)",
"   /RENAME                 Rename all infected files to *.VI?",
"   /SCANHIGH               Scan high memory for viruses",
"   /SCANWILD               Only scan for \"wild\" viruses",
"   /SUB                    Recursively scan subdirectories",
"   /VIRLIST                Display the virus list, sorted by VID#",
"   /WATCH=VID#             Watch specified ALG signature. (VID# is in hex)",
NULL
};


