//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STORCMP.CPv   1.0   12 Mar 1998 12:17:48   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STORCMP.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:48   DCHI
// Initial revision.
// 
//************************************************************************

//
// STORCMP.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStorage::Compare()
//  ALStorageCompare()
//
// REVISION HISTORY
//
//  May 26, 1994  1.0A  : First release
//
//  January 1, 1995 1.01A : Deleted include of storage.h.  This header file
//                          was renamed to stor.h, but I never really needed
//                         to include it anyway.
//
//   February 14, 1996  2.0A  : New release.
//
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include <string.h>

#include "_openf.h"

//
// NAME
//
//  ALStorage::Compare()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Compare two storage objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::Compare( ALStorage &test_object );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageCompare( hALStorage this_object, hALStorage test_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageCompare Lib "AL20FW"
//    (ByVal this_object&, ByVal test_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageCompare( this_object : hALStorage;
//                             test_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be compared.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  test_object  :  Another storage object that will be compared against
//                  this_object.
//
// DESCRIPTION
//
//  This function provides a convenient way to test this_object
//  against another.  Note that if the comparison fails, the status
//  code of this object will be set to an error state.  You will need
//  to clear that error if you intend to use this object again.
//
// RETURNS
//
//  AL_SUCCESS if the two files match.  AL_COMPARE_ERROR if the files
//  don't match.  An error code < AL_SUCCESS is possible if some other
//  error takes place during the process.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::GetCrc32()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::Compare( ALStorage AL_DLL_FAR & test_object )  /* Tag public function */
{
    ALOpenInputFile in1( test_object );
    ALOpenInputFile in2( *this );

    if ( test_object.mStatus < 0 )
       return mStatus = test_object.mStatus;
    if ( GetSize() != test_object.GetSize() )
        return mStatus.SetError( AL_COMPARE_ERROR,
                                 "Comparison failed.  "
                                 "Objects %s and %s are two different sizes.",
                                 mName.GetSafeName(),
                                 test_object.mName.GetSafeName() );
    long position = 0;
    for ( ; ; ) {
        int c = ReadChar();
        if ( c < 0 )
            break;
        if ( c != test_object.ReadChar() )
            return mStatus.SetError( AL_COMPARE_ERROR,
                                     "Objects %s and %s differed at position %ld",
                                     mName.GetSafeName(),
                                     test_object.mName.GetSafeName(),
                                     position );
         position++;
    }
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageCompare( hALStorage this_object,  /* Tag public function */
                  hALStorage test_object )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageCompare" );
    AL_ASSERT_OBJECT( test_object, ALStorage, "ALStorageCompare" );
    return ( (ALStorage *) this_object )->Compare( *(ALStorage *) test_object );
}

#endif

#endif // #if !defined( AL_DEMO )

