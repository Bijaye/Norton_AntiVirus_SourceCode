//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ENTRSC.CPv   1.0   12 Mar 1998 12:16:12   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ENTRSC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:12   DCHI
// Initial revision.
// 
//************************************************************************

//
// ENTRSC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntry::SetComment()
//  ALEntrySetComment()
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
//  ALEntry::SetComment()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the comment for an individual entry.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntry::SetComment( const char *comment );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntrySetComment( hALEntry this_object,
//                         char *comment );
//
// VB SYNOPSIS
//
//  Declare Function ALEntrySetComment Lib "AL20LW"
//    (ByVal this_object&, ByVal comment$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntrySetComment( this_object : hALEntry;
//                              comment : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object that
//                  is going to get the new comment.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  comment :       The new comment that is going to be associated with the
//                  ALEntry object.
//
// DESCRIPTION
//
//  Before adding an object to an archive, you may want to change or set
//  its comment.  You do so by calling this function before performing any
//  operation that will write the directory, such as Create() or
//  WriteDirectory().  It has to dynamically allocate the space in the
//  ALEntry object in order to store the new comment.  This is good for
//  you, because it means you don't have to worry about who owns the comment
//  you just passed in.
//
// RETURNS
//
//  AL_SUCCESS if the new comment was set, < 0 if an error occurred.
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
ALEntry::SetComment( const char AL_DLL_FAR *comment )  /* Tag public function */
{
    if ( mszComment )
        delete[] mszComment;
    if ( comment ) {
        mszComment = new char[ strlen( comment ) + 1 ];
        if ( mszComment )
            strcpy( mszComment, comment );
        else
            return mrList.mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                            "Failed to allocate memory when "
                                            "adding comment to storage object %s",
                                            (char AL_DLL_FAR *) mpStorageObject->mName );
    } else
        mszComment = 0;
    return AL_SUCCESS;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntrySetComment( hALEntry this_object,  /* Tag public function */
                   char AL_DLL_FAR *comment )
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntrySetComment" );
    if ( comment == 0 )
        comment = "";
    return ( (ALEntry *) this_object )->SetComment( comment );
}

#endif

