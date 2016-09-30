//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/LISTUNMD.CPv   1.0   12 Mar 1998 12:16:54   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/LISTUNMD.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:54   DCHI
// Initial revision.
// 
//************************************************************************

//
// LISTUNMD.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryList::UnmarkDuplicates()
//  ALEntryListUnmarkDuplicates()
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
//  ALEntryList::UnmarkDuplicates()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Clean the duplicates from a list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALEntryList::UnmarkDuplicates( ALEntryList &list,
//                                      const char *error_message = 0 );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALEntryListUnmarkDuplicates( hALEntryList this_object,
//                                    hALEntryList list,
//                                    char *error_message );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntryListUnmarkDuplicates Lib "AL20LW"
//    (ByVal this_object&, ByVal list&, ByVal error_message&)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntryListUnmarkDuplicates( this_object : hALEntryList;
//                                         list : hALEntryList;
//                                         error_message : PChar );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is going to be modified .  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  list           :  The list that is going to be compared to this.
//
//  error_message  : Each entry in this that turns out to have a duplicate
//                   entry in the list argument will not only be unmarked,
//                   it will also have its error status set, but only if
//                   an error message is provided.
//
// DESCRIPTION
//
//  I think this function is a little confusing.  At first blush, you would
//  probably expect this function to scan all the items in a single list,
//  and unmark any object that turn out to have duplicates elsewhere
//  in the list.  Unfortunately, it doesn't work that way.
//
//  Instead, this function goes through the list specified by this, and
//  checks to see if each entry in this appears in the list specified by
//  the list parameter.  This means that we are working with two different
//  lists, which certainly offers plenty of chances to get confused.
//
//  Anyway, each entry in this that turns out to have a duplicate gets its
//  mark cleared.  If the calling program specifies an error message,
//  the entry also gets its mStatus error member set to flag this as an
//  error.
//
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
//   February 14, 1996  2.0A : New Release
//

void AL_PROTO
ALEntryList::UnmarkDuplicates( ALEntryList &list,  /* Tag public function */
                               const char AL_DLL_FAR *error_message /* = 0 */ )
{
    ALEntry *job = GetFirstEntry();
    while ( job ) {
        if ( job->GetMark() ) {
            if ( job->Duplicate( list ) ) {
                job->ClearMark();
                if ( error_message && error_message[ 0 ] != '\0' )
                    job->mpStorageObject->mStatus.SetError(
                            AL_DUPLICATE_ENTRY,
                            error_message );
            }
        }
        job = job->GetNextEntry();
    }
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
ALEntryListUnmarkDuplicates( hALEntryList this_object,  /* Tag public function */
                             hALEntryList list,
                             char AL_DLL_FAR *error_message )
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListUnmarkDuplicates" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALEntryListUnmarkDuplicates" );
    ( (ALEntryList *) this_object )->UnmarkDuplicates( * (ALEntryList *) list, error_message );
}


#endif

