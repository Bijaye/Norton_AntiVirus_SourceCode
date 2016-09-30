//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/LISTSMS.CPv   1.0   12 Mar 1998 12:16:52   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/LISTSMS.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:52   DCHI
// Initial revision.
// 
//************************************************************************

//
// LISTSMS.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryList::SetMarkState()
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
//  ALEntryList::SetMarkState()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Set a group of ALEntry objects to a given state.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryList::SetMarkState( const char *name, short int new_state )
//
// C SYNOPSIS
//
//  None, this is an internal protected C++ function.
//
// VB SYNOPSIS
//
//  None, this is an internal protected C++ function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal protected C++ function.
//
// ARGUMENTS
//
//  name       :  The object name, specifying which storage objects are
//                to have their state set.  This name can include
//                wild card characters.  Note that passing a null
//                pointer here will cause a match to *every* object name.
//
//  new_state  :  The new state that the ALEntry mark should be set to.
//
// DESCRIPTION
//
//  This protected function is used internally to help out a couple of the
//  public functions.  It rips through every entry of the list, checks to
//  see if storage object associate with the entry has a name that matches
//  the wildcard specification, and sets the mark if it does.
//
// RETURNS
//
//  A count of the number of ALEntry objects whose state was changed.
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
ALEntryList::SetMarkState( const char AL_DLL_FAR *name,  /* Tag protected function */
                           short int new_state )
{
    int count = 0;

    ALEntry *job = GetFirstEntry();
    while ( job ) {
        if ( name ) {
            if ( job->mpStorageObject->mName.WildCardMatch( name ) ) {
                job->SetMarkState( new_state );
                count++;
            }
        } else {
            job->SetMarkState( new_state );
            count++;
        }
        job = job->GetNextEntry();
    }
    return count;
}


