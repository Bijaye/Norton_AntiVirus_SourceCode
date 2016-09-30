//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ENTRDUP.CPv   1.0   12 Mar 1998 12:16:10   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ENTRDUP.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:10   DCHI
// Initial revision.
// 
//************************************************************************

//
// ENTRDUP.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntry::Duplicate()
//  ALEntryDuplicate()
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
//  ALEntry::Duplicate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Check a list for a duplicate name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntry::Duplicate( ALEntryList &list )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryDuplicate( hALEntry this_object,
//                        hALEntryList list );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryDuplicate Lib "AL20LW"
//    (ByVal this_object&, ByVal list&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntryDuplicate( this_object : hALEntry;
//                             list : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object that
//                  is going to be compared to the list.  Note that the
//                  C++ member function version of this call doesn't have
//                  an argument called 'this_object', since it has access
//                  to 'this' implicitly.
//
//  list         :  A reference to an ALEntryList.  This is the list that
//                  will be scanned for duplicates.
//
// DESCRIPTION
//
//  This function is used to scan a list of ALEntry objects to see if
//  any of them have the same name as this.  Unmarked objects are ignored.
//  All the function does is zip through the ALEntryList, checking each
//  marked member for an ASCII match with the name of the storage object
//  pointed to by this.  You can see that the case sensitivity of this
//  is observed when making the comparison.
//
// RETURNS
//
//  0 if the entry is not duplicated, 1 if it is.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

int AL_PROTO
ALEntry::Duplicate( ALEntryList &list )  /* Tag public function */
{
    char AL_DLL_FAR *name = mpStorageObject->mName;
    int case_sensitive = mpStorageObject->mName.mCase == AL_MIXED;
    ALEntry *job = list.GetFirstEntry();
    while ( job ) {
        if ( job->GetMark() && job != this ) {
            if ( case_sensitive ) {
                if ( strcmp( name, job->mpStorageObject->mName ) == 0 )
                    return 1;
            } else {
                if ( stricmp( name, job->mpStorageObject->mName ) == 0 )
                    return 1;
            }
        }
        job = job->GetNextEntry();
    }
    return 0;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntryDuplicate( hALEntry this_object,  /* Tag public function */
                  hALEntryList list )
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryDuplicate" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALEntryDuplicate" );
    return ( (ALEntry *) this_object )->Duplicate( * (ALEntryList *) list );
}

#endif
