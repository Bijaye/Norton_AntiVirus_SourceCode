//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_GLNG.CPv   1.0   12 Mar 1998 12:15:38   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_GLNG.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:38   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_GLNG.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//   ALGlCompressorLevel()
//   ALGlDecompressorLevel()
//
// DESCRIPTION
//
//  This file contains a couple of member access routines for ALGlCompressor
//  and ALGlDecompressor.
//
//  Functions that simply provide a translation layer for an existing C++
//  function are always located in the same file as the C++ function.  The
//  function sign this file don't have any existing C functions to attach
//  to, since they implement either pure virtual functions or member access
//  routines.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New release
//
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "glengn.h"

//
// NAME
//
//  ALGlCompressorLevel()
//
//  See H/GLENGN.INL for details on how this function works.  The inline
//  C++ version of the function can be found there, along with its
//  documentation

extern "C" AL_LINKAGE short int AL_FUNCTION
ALGlCompressorLevel( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALGlCompressor, "ALGlCompressorLevel" );
    return ( (ALGlCompressor *) this_object )->CompressionLevel();
}


//
// NAME
//
//  ALGlDecompressorLevel()
//
//  See H/GLENGN.INL for details on how this function works.  The inline
//  C++ version of the function can be found there, along with its
//  documentation

extern "C" AL_LINKAGE short int AL_FUNCTION
ALGlDecompressorLevel( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALGlDecompressor, "ALGlDecompressorLevel" );
    return ( (ALGlDecompressor *) this_object )->CompressionLevel();
}

#endif // #if !defined( AL_DEMO )

