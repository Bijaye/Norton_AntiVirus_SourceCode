//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/_DBGTXT.CPv   1.0   12 Mar 1998 12:14:24   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/_DBGTXT.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:24   DCHI
// Initial revision.
// 
//************************************************************************

//
// _DEBUG.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  _ALAssertFailure()
//
// DESCRIPTION
//
//  This file contains a support routine used by the assertion macros,
//
// REVISION HISTORY
//
//   May 22, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release
//
//   March 20, 1996     2.01A : Excluded all the code in this file from
//                              the test drive release.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//
// NAME
//
//  _ALAssertFailure()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++
//
// SHORT DESCRIPTION
//
//  Display an error message and abort the program.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void _ALAssertFailure( const char *condition,
//                         const char *filename,
//                         int line,
//                         const char *message,
//                         ... );
//
// C SYNOPSIS
//
//  None, this is an internal support function.
//
// VB SYNOPSIS
//
//  None, this is an internal support function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal support function.
//
// ARGUMENTS
//
//  condition   :  A character string containing the condition that failed,
//                 leading to the assertion.
//
//  filename    :  The name of the file where the assertion error took place.
//
//  line        :  The line in the file where the assertion error took place.
//
//  message     :  The error message associated with the assertion error.
//                 This message is a sprintf() style format string.
//
//  ...         :  Any additional arguments.
//
//
// DESCRIPTION
//
//  The C run time library features an assert() macro, that can be used to
//  abort a program if a given condition isn't true.  It aborts the program
//  by calling a routine that looks something like this.  The AL_ASSERT()
//  macro that we use is even better, because it includes a comment
//  that gets displayed when the abort takes place.  This routine is
//  responsible for displaying that comment, along with the file name and
//  the line number, then aborting the program.  It is called by the
//  AL_ASSERT() macro when the conditional expression argument fails.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

AL_LINKAGE void AL_CFUNCTION
_ALAssertFailure( const char AL_DLL_FAR *condition,  /* Tag debug function */
                  const char AL_DLL_FAR *filename,
                  int line,
                  const char AL_DLL_FAR *message,
                  ... )
{
    char buf1[ 256 ];
    char buf2[ 128 ];
    va_list argptr;

    va_start( argptr, message );
    vsprintf( buf2, message, argptr );
    va_end( argptr );

    sprintf ( buf1,
              "Assertion error, ArchiveLib is aborting the application.\n"
              "Condition = %s\n"
              "File = %s, line = %d\n"
              "%s",
              condition,
              filename,
              line,
              buf2 );
    cerr << buf1 << "\n" << flush;
    abort();
}

#endif //#if !defined( AL_DEMO )

