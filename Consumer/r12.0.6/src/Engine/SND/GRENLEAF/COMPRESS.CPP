//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/COMPRESS.CPv   1.0   12 Mar 1998 12:15:36   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/COMPRESS.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:36   DCHI
// Initial revision.
// 
//************************************************************************

//
// COMPRESS.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALCompressor::operator new()
//  ALCompressor::ALCompressor()
//  ALCompressor::~ALCompressor()
//  deleteALCompressor()
//  ALCompress()
//
// DESCRIPTION
//
//  This file contains the base class functions for the class ALCompress().
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
//  ALCompressor::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  C++
//
// SHORT DESCRIPTION
//
//  Memory allocation operator needed with DLL.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void * ALCompressor::operator new( size_t size )
//
// C SYNOPSIS
//
//  None.
//
// VB SYNOPSIS
//
//  None.
//
// DELPHI SYNOPSIS
//
//  None.
//
// ARGUMENTS
//
//  size  :  The number of bytes needed to create a new ALCompressor object.
//
// DESCRIPTION
//
//  When using a DLL, it is easy to create a dangerous situation when
//  creating objects whose ctor and dtor are both in the DLL.  The problem
//  arises because when you create an object using new, the memory for
//  the object will be allocated from the EXE.  However, when you destroy
//  the object using delete, the memory is freed inside the DLL.  Since
//  the DLL doesn't really own that memory, bad things can happen.
//
//  But, you say, won't the space just go back to the Windows heap regardless
//  of who tries to free it?  Maybe, but maybe not.  If the DLL is using
//  a subsegment allocation scheme, it might do some sort of local free
//  before returning the space to the windows heap.  That is the point where
//  you could conceivably cook your heap.
//
//  By providing our own version of operator new inside this class, we
//  ensure that all memory allocation for the class will be done from
//  inside the DLL, not the EXE calling the DLL.
//
// RETURNS
//
//  A pointer to the newly allocated storage area, or 0 if no storage
//  was available.
//
// EXAMPLE
//
//  None.
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 23, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALCompressor::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}
#endif

//
// NAME
//
//  ALCompressor::ALCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Construct a compressor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALCompressor::ALCompressor( ALCompressionType compression_type_int,
//                              const char *compression_type_string );
//
// C SYNOPSIS
//
//  None.
//
// VB SYNOPSIS
//
//  None.
//
// DELPHI SYNOPSIS
//
//  None.
//
// ARGUMENTS
//
//  compression_type_int    :  An integer used to indicate the compression
//                             type.  This needs to be selected from the
//                             enumerated type defined in ALDEFS.H.  This
//                             value is usually what gets stored in an
//                             archive to indicate what compression type was
//                             used.
//
//  compression_type_string :  A pointer to a C-style string describing the
//                             type of compression.  This isn't used in
//                             any algorithms, it's on hand as a U/I aid.
//
// DESCRIPTION
//
//  This is the base class constructor for Compressor objects.  Since this
//  class has a pure virtual function, you aren't going to be able to
//  call this guy directly.  Instead, it is called from the constructor
//  for guys such as ALGlCompressor and ALPkCompressor.
//
//  All we do here is init a couple of common members, and invoke the
//  constructor for the absolute base class, ALEngine.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release


AL_PROTO
ALCompressor::ALCompressor( ALCompressionType compression_type_int,  /* Tag public function */
                            const char AL_DLL_FAR *compression_type_string )
  : ALEngine( compression_type_int, compression_type_string )
{
}

//
// NAME
//
//  ALCompressor::~ALCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB Delphi
//
// SHORT DESCRIPTION
//
//  The ALCompressor destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALCompressor::~ALCompressor();
//
// C SYNOPSIS
//
//  void deleteALCompressor( hALCompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALCompressor Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALCompressor( this_object : hALCompressor );
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This destructor will be called as the derived class is destroyed, such
//  as ALPkCompressor or ALGlCompressor.  It doesn't have to do anything, so
//  conceivably this function could just be eliminated, and we could use
//  the compiler-generated one.  But, I guess we just don't like to leave
//  anything up to chance.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

AL_PROTO
ALCompressor::~ALCompressor()  /* Tag public function */
{
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALCompressor( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "deleteALCompressor()" );
    delete (ALCompressor *) this_object;
}

#endif

//
// NAME
//
//  ALCompressor::Compress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Compress from an input object to an output object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALCompressor::Compress( ALStorage &input_object,
//                              ALStorage &output_object );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALCompress( hALCompressor this_object,
//                  hALStorage input,
//                  hALStorage output );
//
// VB SYNOPSIS
//
//  Declare Function ALCompress Lib "AL20LW"
//    (ByVal this_object&, ByVal input&, ByVal output& ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompress( this_object : hALCompressor;
//                       input : hALStorage;
//                       output : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALCompressor object that
//                  is going to perform the compression.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  input        :  A pointer or reference to the input storage object
//                  that is going to be compressed.
//
//  output       :  A pointer or reference to the output object that is
//                  going to receive the compressed data.
//
// DESCRIPTION
//
//  An ALCompressor object only knows how to do one thing, and that is
//  expressed by way of this function.  Compress() takes an input file,
//  and reads until it hits an EOF.  Output bytes are written to the
//  output object in some compressed format.  The exact format is determined
//  by the derived class.  ALCompressor is an abstract base class that
//  can't be instantiated by itself.
//
//  Since the class can't be instantiated by itself, it's kind of bogus
//  to even suggest that ALCompressor::Compress() is a separate function,
//  as I have done here.  In reality, the only thing that really exists
//  here is a C function that can be used to call the virtual function
//  set up by a derived class.
//
// RETURNS
//
//  A success or failure code.  Hopefully AL_SUCCESS, but values less
//  than 0 are possible (as errors.)
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALCompress( hALCompressor this_object, /* Tag public function */
            hALStorage input,
            hALStorage output )
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompress" );
    AL_ASSERT_OBJECT( input, ALStorage, "ALCompress" );
    AL_ASSERT_OBJECT( output, ALStorage, "ALCompress" );
    return ( (ALCompressor *) this_object )->Compress( *(ALStorage *) input, *(ALStorage *) output );
}

#endif


