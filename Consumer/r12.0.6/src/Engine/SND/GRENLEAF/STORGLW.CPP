//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STORGLW.CPv   1.0   12 Mar 1998 12:17:52   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STORGLW.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:52   DCHI
// Initial revision.
// 
//************************************************************************

//
// STORGLW.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStorage::WriteGlShort()
//  ALStorageWriteGlShort()
//  ALStorage::WriteGlLong()
//  ALStorageWriteGlLong()
//
// DESCRIPTION
//
//  This file contains the modules used to write shorts and longs
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
//  ALStorage::WriteGlShort()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Write a short data word using Greenleaf's portable format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::WriteGlShort( short short_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageWriteGlShort( hALStorage this_object, short short_data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWriteGlShort Lib "AL20LW"
//    (ByVal this_object&, ByVal short_data%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWriteGlShort( this_object : hALStorage;
//                                  short_data : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  be written to the storage object.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  short_data  :   The short data element that is going to be written to
//                  the storage object in Greenleaf format.
//
// DESCRIPTION
//
//  This function writes short integers in Greenleaf portable format.  It is
//  complementary to ReadGlShort() which reads data in Greenleaf
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
//  ALStorage::WritePkShort(), ALStorage::WriteGlLong()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//


int AL_PROTO
ALStorage::WriteGlShort( short int short_data )  /* Tag public function */
{
    WriteChar( short_data >> 8 );
    WriteChar( short_data );
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageWriteGlShort( hALStorage this_object,  /* Tag public function */
                       short int short_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWriteGlShort" );
    return ( (ALStorage *) this_object)->WriteGlShort( short_data );
}

#endif

//
// NAME
//
//  ALStorage::WriteGlLong()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Write a long data word using Greenleaf's portable format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::WriteGlLong( long long_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageWriteGlLong( hALStorage this_object, long long_data );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWriteGlLong Lib "AL20LW"
//    (ByVal this_object&, ByVal long_data&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWriteGlLong( this_object : hALStorage;
//                                 long_data : Long Int ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  be written to the storage object.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  long_data  :    The long data element that is going to be written to
//                  the storage object in Greenleaf format.
//
// DESCRIPTION
//
//  This function writes long integers in Greenleaf Gl format.  It is
//  complementary to ReadGlLong() which reads data in Greenleaf
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
//  ALStorage::ReadGlLong(), ALStorage::WriteGlShort()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::WriteGlLong( long long_data )  /* Tag public function */
{
    WriteChar( (int) ( long_data >> 24 ) );
    WriteChar( (int) ( long_data >> 16 ) );
    WriteChar( (int) ( long_data >> 8  ) );
    WriteChar( (int) long_data );
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageWriteGlLong( hALStorage this_object,  /* Tag public function */
                      long int long_data )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWriteGlLong" );
    return ( (ALStorage *) this_object)->WriteGlLong( long_data );
}

#endif

