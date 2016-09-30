//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCSC.CPv   1.0   12 Mar 1998 12:14:42   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCSC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:42   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCSC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::SetComment()
//  ALArchiveSetComment()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALArchive::SetComment()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set an archive object's comment.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::SetComment( char *comment )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveSetComment( hALArchive this_object, char *comment )
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveSetComment Lib "AL20LW"
//    (ByVal this_object&, ByVal comment$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntrySetComment( this_object : hALEntry;
//                              comment : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchive object.
//                  We are going to change the comment in archive, although
//                  the new comment won't be stored in the archive until
//                  we do a WriteDirectory().  Note that the C++ member
//                  function doesn't have this argument.  Why not?  Doesn't
//                  need it, since it has the implicit 'this'.
//
//  comment       : An ASCII string that will be the new comment.  Note
//                  that this gets passed properly from both C and VB.
//
// DESCRIPTION
//
//  The archive object has a comment member, that is blank when first
//  constructed.  It can be set to something interesting either by
//  reading in a new comment along with the archive directory, or by
//  setting it using this function.
//
// RETURNS
//
//  AL_SUCCESS, if things went well, AL_CANT_ALLOCATE_MEMORY if allocation
//  of the character array failed.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//
//   February 14, 1996  2.0A : New release

int AL_PROTO
ALArchive::SetComment( char AL_DLL_FAR * comment )  /* Tag public function */
{
    if ( comment == 0 )
        mComment = "";
    else
        mComment = comment;
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveSetComment( hALArchive this_object,  /* Tag public function */
                     char AL_DLL_FAR *comment )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveSetComment" );
    if ( comment == 0 )
        comment = "";
    return ( (ALArchive *) this_object )->SetComment( comment );
}

#endif
