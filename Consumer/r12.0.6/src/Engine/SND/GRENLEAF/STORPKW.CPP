//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STORPKW.CPv   1.0   12 Mar 1998 12:17:42   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STORPKW.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:42   DCHI
// Initial revision.
// 
//************************************************************************

//
// STORPKW.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStorage::WritePkShort()
//  ALStorageWritePkShort()
//  ALStorage::WritePkLong()
//  ALStorageWritePkLong()
//
// DESCRIPTION
//
//  This file contains the modules used to write shorts and longs
//  in PKWare's endian order.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <string.h>

//
// NAME
//
//  ALStorage::WritePkShort()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Write a short data word using PKWare's format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::WritePkShort( short short_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageWritePkShort( hALStorage this_object, short short_data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWritePkShort Lib "AL20LW"
//    (ByVal this_object&, ByVal short_data&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWritePkShort( this_object : hALStorage;
//                                  Var short_data : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  will receive the short word.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  short_data  :   The short data element that is going to be written
//                  to the storage object in PKWare format.
//
// DESCRIPTION
//
//  This function writes short integers in native PKWare format.  It is
//  complementary to WriteGlShort() which writes data in Greenleaf
//  format.  Unfortunately, Greenleaf format and PKWare format are
//  not identical.
//
// RETURNS
//
//  An ArchiveLib status code, ranging from AL_SUCCESS to any of the
//  error codes < 0.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::WritePkShort( short int short_data )  /* Tag public function */
{
    WriteChar( short_data );
    WriteChar( short_data >> 8 );
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageWritePkShort( hALStorage this_object,  /* Tag public function */
                       short int short_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWritePkShort" );
    return ( (ALStorage *) this_object)->WritePkShort( short_data );
}

#endif

//
// NAME
//
//  ALStorage::WritePkLong()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Write a long data word using PKWare's format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::WritePkLong( long long_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageWritePkLong( hALStorage this_object, long long_data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWritePkLong Lib "AL20LW"
//    (ByVal this_object&, ByVal long_data&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWritePkLong( this_object : hALStorage;
//                                 long_data : LongInt ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  will receive the long word.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  long_data   :   The long data element that is going to be written
//                  to the storage object in PKWare format.
//
// DESCRIPTION
//
//  This function writes long integers in native PKWare format.  It is
//  complementary to the WriteGlLong which writes data in Greenleaf
//  format.  Unfortunately, Greenleaf format and PKWare format are
//  not identical.
//
// RETURNS
//
//  An ArchiveLib status code, ranging from AL_SUCCESS to any of the
//  error codes < 0.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::WritePkLong( long long_data )  /* Tag public function */
{
    WriteChar( (int) long_data );
    WriteChar( (int) ( long_data >> 8  ) );
    WriteChar( (int) ( long_data >> 16 ) );
    WriteChar( (int) ( long_data >> 24 ) );
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageWritePkLong( hALStorage this_object,  /* Tag public function */
                      long long_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWritePkLong" );
    return ( (ALStorage *) this_object)->WritePkLong( long_data );
}

#endif


