//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STORPKR.CPv   1.0   12 Mar 1998 12:17:44   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STORPKR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:44   DCHI
// Initial revision.
// 
//************************************************************************

//
// STORPKR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStorage::ReadPkShort()
//  ALStorageReadPkShort()
//  ALStorage::ReadPkLong()
//  ALStorageReadPkLong()
//
// DESCRIPTION
//
//  This file contains the modules used to read shorts and longs
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

//
// NAME
//
//  ALStorage::ReadPkShort()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read a short data word using PKWare's format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::ReadPkShort( short &short_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageReadPkShort( hALStorage this_object, short *short_data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageReadPkShort Lib "AL20LW"
//    (ByVal this_object&, short_data%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageReadPkShort( this_object : hALStorage;
//                                 Var short_data : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  contains the short word to be read.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  short_data   :  A pointer to the short data element that is going to
//                  receive the word read in from the storage object.
//
// DESCRIPTION
//
//  This function reads short integers in native PKWare format.  It is
//  complementary to the ReadGlShort, which reads data in Greenleaf
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
ALStorage::ReadPkShort( short int AL_DLL_FAR & short_data )  /* Tag public function */
{
    short_data = (short int) ReadChar();
    short_data |= (short int) ( ReadChar() << 8 );
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageReadPkShort( hALStorage this_object,  /* Tag public function */
                      short int AL_DLL_FAR *short_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageReadPkShort" );
    AL_ASSERT( short_data != 0, "Null pointer passed to function" );
    return ( (ALStorage *) this_object )->ReadPkShort( *short_data );
}

#endif

//
// NAME
//
//  ALStorage::ReadPkLong()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read a long data word using PKWare's format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::ReadPkLong( long &long_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageReadPkLong( hALStorage this_object, long *data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageReadPkLong Lib "AL20LW"
//    (ByVal this_object&, long_data&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageReadPkLong( this_object : hALStorage;
//                                Var long_data : LongInt ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  contains the long word to be read.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  long_data   :   A pointer to the long data element that is going to
//                  receive the word read in from the storage object.
//
// DESCRIPTION
//
//  This function reads long integers in native PKWare format.  It is
//  complementary to the ReadGlLong, which reads data in Greenleaf
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
ALStorage::ReadPkLong( long AL_DLL_FAR & long_data )  /* Tag public function */
{
    long_data = (long) ReadChar();
    long_data |= (long) ReadChar() << 8;
    long_data |= (long) ReadChar() << 16;
    long_data |= (long) ReadChar() << 24;
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageReadPkLong( hALStorage this_object,  /* Tag public function */
                     long AL_DLL_FAR *long_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageReadPkLong" );
    AL_ASSERT( long_data != 0, "Null pointer passed to function" );
    return ( (ALStorage *) this_object )->ReadPkLong( *long_data );
}

#endif


