//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/LISTDU.CPv   1.0   12 Mar 1998 12:16:54   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/LISTDU.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:54   DCHI
// Initial revision.
// 
//************************************************************************

//
// LISTDU.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryList::DeleteUnmarked()
//  ALEntryListDeleteUnmarked()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALEntryList::DeleteUnmarked()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Delete all unmarked ALEntry objects from the list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryList::DeleteUnmarked();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryListDeleteUnmarked( hALEntryList this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListDeleteUnmarked Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntryListDeleteUnmarked( this_object : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is going to be modified.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  Sometimes you may have a list with a whole bunch of unmarked entries.
//  Those unmarked entries are just sitting there taking up space, so it
//  would be handy to be able to just delete them.  That is what this
//  function does.
//
// RETURNS
//
//  The number of entries that are deleted.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALEntryList::DeleteUnmarked()  /* Tag public function */
{
    ALEntry *job;
    int count = 0;

    job = GetFirstEntry();
    while ( job ) {
        ALEntry *next_job = job->GetNextEntry();
        if ( job->GetMark() == 0 ) {
            count++;
            delete job;
        }
        job = next_job;
    }
    return count;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntryListDeleteUnmarked( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListDeleteUnmarked" );
    return ( (ALEntryList *) this_object )->DeleteUnmarked();
}

#endif
