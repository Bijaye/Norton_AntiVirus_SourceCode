//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STORGLR.CPv   1.0   12 Mar 1998 12:17:50   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STORGLR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:50   DCHI
// Initial revision.
// 
//************************************************************************

//
// STORGLR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStorage::ReadGlShort()
//  ALStorageReadGlShort()
//  ALStorage::ReadGlLong()
//  ALStorageReadGlLong()
//
// DESCRIPTION
//
//  This file contains the modules used to read shorts and longs
//  in Greenleaf's endian order.
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
//  ALStorage::ReadGlShort()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read a short word in Greenleaf format from a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::ReadGlShort( short int & short_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageReadGlShort( hALStorage this_object,
//                            short int AL_DLL_FAR *short_data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageReadGlShort Lib "AL20LW"
//    (ByVal this_object&, short_data%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageReadGlShort( this_object : hALStorage;
//                                 Var short_data : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  has the short word we are going to read.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  short_data  :  A reference to a 16 bit integer that is going to
//                 have data read in from this storage object.
//
// DESCRIPTION
//
//  In order to make sure our archives can be read and written on all sorts
//  of systems, we have a few functions that are used to read numerical
//  data in a portable fashion.  This function reads short integers in
//  little endian format (which is not native Intel format).  The
//  complementary function, WriteGlShort(), writes short integers out
//  using the same format.
//
// RETURNS
//
//  AL_SUCCESS if all goes well.  Otherwise, some error code < AL_STATUS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::ReadGlLong(), ALStorage::WriteGlShort()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::ReadGlShort( short int AL_DLL_FAR & short_data )  /* Tag public function */
{
    short_data = (short int) ( ReadChar() << 8 );
    short_data |= (short int) ReadChar();
    return mStatus;
}

#if !defined( NO_AL_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageReadGlShort( hALStorage this_object,  /* Tag public function */
                      short int AL_DLL_FAR *short_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageReadGlShort" );
    AL_ASSERT( short_data != 0, "Null pointer passed to function" );
    return ( (ALStorage *) this_object )->ReadGlShort( *short_data );
}

#endif

//
// NAME
//
//  ALStorage::ReadGlLong()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read a long word in Greenleaf format from a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::ReadGlLong( long int & long_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageReadGlLong( hALStorage this_object,
//                           long int AL_DLL_FAR *long_data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageReadGlLong Lib "AL20LW"
//    (ByVal this_object&, long_data&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageReadGlLong( this_object : hALStorage;
//                                Var long_data : Long Int ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  has the long word we are going to read.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  long_data   :  A reference to a 32 bit integer that is going to
//                 have data read in from this storage object.
//
// DESCRIPTION
//
//  In order to make sure our archives can be read and written on all sorts
//  of systems, we have a few functions that are used to read numerical
//  data in a portable fashion.  This function reads long integers in
//  little endian format (which is not native Intel format).  The
//  complementary function, WriteGlLong(), writes long integers out
//  using the same format.
//
// RETURNS
//
//  AL_SUCCESS if all goes well.  Otherwise, some error code < AL_STATUS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::WriteGlLong(), ALStorage::ReadGlShort()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::ReadGlLong( long & long_data )  /* Tag public function */
{
    long_data = (long) ReadChar() << 24;
    long_data |= (long) ReadChar() << 16;
    long_data |= (long) ReadChar() << 8;
    long_data |= ReadChar();
    return mStatus;
}

#if !defined( NO_AL_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageReadGlLong( hALStorage this_object,  /* Tag public function */
                     long int AL_DLL_FAR *long_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageReadGlLong" );
    AL_ASSERT( long_data != 0, "Null pointer passed to function" );
    return ( (ALStorage *) this_object )->ReadGlLong( *long_data );
}

#endif

