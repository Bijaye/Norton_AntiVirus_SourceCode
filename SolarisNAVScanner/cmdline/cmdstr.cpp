// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/cmdstr.cpv   1.2   12 Jun 1996 12:11:14   RAY  $
//
// Description:
//  Definition of command options and the bits they set.
//
// Contains:
//  CommandOpts[]
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/cmdstr.cpv  $
// 
//    Rev 1.2   12 Jun 1996 12:11:14   RAY
// Added /NOFILE switch to skip file scanning
// 
//    Rev 1.1   04 Jun 1996 18:14:32   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.0   21 May 1996 12:25:30   DCHI
// Initial revision.
// 
//************************************************************************

#include "avtypes.h"
#include "cmdparse.h"

COPTS_T  CommandOpts[]    = {  {"REPORT=",  OPTION_FLAG_REPORT},
                               {"DOALL",    OPTION_FLAG_DOALL},
                               {"HELP",     OPTION_FLAG_HELP},
                               {"NOBOOT",   OPTION_FLAG_NOBOOT},
                               {"NOPART",   OPTION_FLAG_NOPART},
                               {"NOMEM",    OPTION_FLAG_NOMEM},
                               {"REPAIR",   OPTION_FLAG_REPAIR},
                               {"DELETE",   OPTION_FLAG_DELETE},
                               {"RENAME",   OPTION_FLAG_RENAME},
                               {"SCANHIGH", OPTION_FLAG_SCANHIGH},
                               {"SUB",      OPTION_FLAG_RECURSE},
                               {"NOPOLY",   OPTION_FLAG_NOPOLY},
                               {"SCANWILD", OPTION_FLAG_SCANWILD},
                               {"REMVIR=",  OPTION_FLAG_REMVIR},
                               {"VIRLIST",  OPTION_FLAG_VIRLIST},
                               {"MEMAVAIL", OPTION_FLAG_MEMAVAIL},
                               {"WATCH=",   OPTION_FLAG_WATCH},
                               {"DUMP",     OPTION_FLAG_DUMP},
                               {"NOFILE",   OPTION_FLAG_NOFILE},
                               {NULL, 0}
                           };

