//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ENTRCR.CPv   1.0   12 Mar 1998 12:16:12   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ENTRCR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:12   DCHI
// Initial revision.
// 
//************************************************************************

//
// ENTRCR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntry::CompressionRatio()
//  ALEntryCompressionRatio()
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
//  ALEntry::CompressionRatio()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return the compression ratio for an entry in terms of 0-100%
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntry::CompressionRatio()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryCompressionRatio( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryCompressionRatio Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntryCompressionRatio( this_object : hALEntry ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object that
//                  you are intereseted in.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This calculates and returns the compression ratio.  We don't store the
//  ratio in ALEntry, because it is so darned easy to calculate when
//  we need it.  However, there are going to be times when we don't have
//  it.
//
// RETURNS
//
//  The integer representing the compression ratio.  The ratio is a number
//  from 0 to 100 (or maybe more) with 0 being perfect compression.
//
//  It is possible to get a -1 back from this routine if the compression
//  ratio is not presently known.  This will be the case if you have
//  not created the archive yet, or have a new object that hasn't been
//  inserted yet.
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
ALEntry::CompressionRatio()  /* Tag public function */
{
    long uncompressed_size = mpStorageObject->GetSize();

    if ( uncompressed_size  <= 0 )
        return -1;
    if ( mlCompressedSize <= 0 )
       return -1;
    return (int) ( 100 * mlCompressedSize / uncompressed_size );
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntryCompressionRatio( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryCompressionRatio" );
    return ( (ALEntry *) this_object )->CompressionRatio();
}

#endif

