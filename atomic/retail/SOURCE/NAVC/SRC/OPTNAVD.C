// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/optnavd.c_v   1.0   06 Feb 1997 20:56:16   RFULLER  $
//
// Description:
//      These are the functions and (non-language) structures
//      used by multiple Options CP Dialogs.
//
// Contains:
//
// See Also:
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/navc/VCS/optnavd.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:16   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:19:48   MKEATIN
// Initial revision.
// 
//    Rev 1.2   07 Nov 1996 15:13:34   JBELDEN
// this is just a stub for code referenced in an external library
//
//    Rev 1.1   04 Nov 1996 17:25:18   JBELDEN
// stubbed out code related to dialogs
//
//    Rev 1.0   02 Oct 1996 12:30:26   JBELDEN
// Initial revision.
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"

#include "navdprot.h"
#include "options.h"                    // For SIZE_EXT definition



#define SZ_SPACE_STRING                 (SZ_EXT_DELIMITERS + 1)


//************************************************************************
// STATIC VARIABLES
//************************************************************************

static  char SZ_EXT_DELIMITERS [] = "\t ";


//************************************************************************
// GLOBAL VARIABLES
//************************************************************************

BYTE byAcceptCPDialog;


//************************************************************************
// ProgramExtDialog()
//
// This routine displays the list of recognized program file extensions
// and allows the user to add or delete entries, or restore the default
// list of extensions.
//
// Parameters:
//      LPSTR   szExt                   [in] Current program file extensions
//                                      [out] New extensions if accepted
//
//      LPCSTR  szDef                   List of default extensions
//
// Returns:
//      Nothing.
//************************************************************************
// 2/23/93 DALLEE Function created.
//************************************************************************

VOID PASCAL ProgramExtDialog (LPSTR szExt, LPCSTR szDef)
{
#ifndef TTYSCAN

#endif
} // End ProgramExtDialog()



