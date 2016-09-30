//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/LISTTM.CPv   1.0   12 Mar 1998 12:16:52   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/LISTTM.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:52   DCHI
// Initial revision.
// 
//************************************************************************

//
// LISTTM.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryList::ToggleMarks()
//  ALEntryListToggleMarks()
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
//  ALEntryList::ToggleMarks()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Toggle the marks of every ALEntry object in a list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryList::ToggleMarks();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryListToggleMarks( hALEntryList this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListToggleMarks Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntryListToggleMarks( this_object : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is going to be modified.  Note that the C++
//                  version of this function doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This simple member function just goes through the entire list,
//  toggling the mark state of every entry.  In other words, if the mark
//  was previously set, it will now be cleared, and vice versa.
//
// RETURNS
//
//  A count of the number of entries whose mark was changed.
//  (Just the total number of entries.)
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//

int AL_PROTO
ALEntryList::ToggleMarks()  /* Tag public function */
{
    int count = 0;
    ALEntry *job = GetFirstEntry();
    while ( job ) {
        job->SetMarkState( (short int) !job->GetMark() );
        job = job->GetNextEntry();
        count++;
    }
    return count;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntryListToggleMarks( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListToggleMarks" );
    return ( (ALEntryList *) this_object )->ToggleMarks();
}

#endif

