//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_PKNG.CPv   1.0   12 Mar 1998 12:15:16   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_PKNG.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:16   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_PKNGN.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALPkCompressorLevel()
//  ALPkDecompressorLevel()
//  ALPkCompressorSetLevel()
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines that provide
//  member access for class ALPkCompressor and class ALPkDecompressor.
//
//  Functions that simply provide a translation layer for an existing C++
//  function are always located in the same file as the C++ function.  The
//  functions in this file don't have any existing C functions to attach
//  to, since they implement either pure virtual functions or member access
//  routines.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "pkengn.h"

//
// NAME
//
//  ALPkCompressorLevel()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the compression level for a PKWare compression engine.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the option member
//  of the compressor class, so no access function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  short int ALPkCompressorLevel( hALCompressor this_object )
//
// VB SYNOPSIS
//
//  Declare Function ALPkCompressorLevel Lib "AL20LW"
//    (ByVal this_object& ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALPkCompressorLevel( this_object : hALCompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object   : The compressor whose option member needs to be examined.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALPkCompressor::option.  The function returns a number ranging from
//  0 to 4, whose values correspond to PKWare compression levels
//  Copy, Normal, Maximum, Fast, and SuperFast.
//
//  This function first tests its handle argument for correct type (when in
//  debug mode), then casts and returns the data member.  Note that I
//  increment the member by 1 when returning it.  I forget why.
//
// RETURNS
//
//  The option integer stored in the compression engine.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE short int AL_FUNCTION
ALPkCompressorLevel( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALPkCompressor, "ALPkCompressorLevel" );
    return (short int) ( ( (ALPkCompressor *) this_object )->option + 1 );
}

//
// NAME
//
//  ALPkDecompressorLevel()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the compression level for a PKWare decompression engine.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the option member
//  of the decompressor class, so no access function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  short int ALPkDecompressorLevel( hALDecompressor this_object )
//
// VB SYNOPSIS
//
//  Declare Function ALPkDecompressorLevel Lib "AL20LW"
//    (ByVal this_object& ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALPkDecompressorLevel( this_object : hALDecompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object   : The handle of the decompressor whose option member
//                  needs to be examined.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALPkDecompressor::option.  The function returns a number ranging from
//  0 to 4, whose values correspond to PKWare compression levels
//  Copy, Normal, Maximum, Fast, and SuperFast.
//
//  This function first tests its handle argument for correct type (when in
//  debug mode), then casts and returns the data member.  Note that I
//  increment the member by 1 when returning it.  I forget why.
//
// RETURNS
//
//  The option integer stored in the decompression engine.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE short int AL_FUNCTION
ALPkDecompressorLevel( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALPkDecompressor, "ALPkDecompressorLevel" );
    return (short int) (( (ALPkDecompressor *) this_object )->option + 1);
}


//
// NAME
//
//  ALPkCompressorSetLevel()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the compression level for a PKWare compression engine.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the option member
//  of the compressor class, so no access function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  void ALPkCompressorSetLevel( hALCompressor this_object,
//                               short int level )
//
// VB SYNOPSIS
//
//  Declare Sub ALPkCompressorSetLevel Lib "AL20LW"
//    (ByVal this_object&, ByVal level% ) As Integer
//
// DELPHI SYNOPSIS
//
//  procedure ALPkCompressorSetLevel( this_object : hALCompressor;
//                                    level : Integer );
//
// ARGUMENTS
//
//  this_object   : The handle of the Compressor whose option member
//                  needs to be modified.
//
//  level         : The new setting a number from 0 to 4.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALPkCompressor::option.  The member is set to a number ranging from
//  0 to 4, whose values correspond to PKWare compression levels
//  Copy, Normal, Maximum, Fast, and SuperFast.
//
//  This function first tests its handle argument for correct type (when in
//  debug mode), then modifies the data member.  Note that I
//  decrement the member by 1 when storing it.  I forget why.
//
// RETURNS
//
//  None.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALPkCompressorSetLevel( hALCompressor this_object, /* Tag public function */
                        short int level )
{
    AL_ASSERT_OBJECT( this_object, ALPkCompressor, "ALPkCompressorSetLevel" );
    ((ALPkCompressor *) this_object )->option = (ALPkCompressor::_option) level;
}
