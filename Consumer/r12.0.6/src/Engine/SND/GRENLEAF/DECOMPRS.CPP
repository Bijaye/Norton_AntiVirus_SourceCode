//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/DECOMPRS.CPv   1.0   12 Mar 1998 12:16:06   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/DECOMPRS.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:06   DCHI
// Initial revision.
// 
//************************************************************************

//
// DECOMPRS.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALDecompressor::operator new()
//  ALDecompressor::ALDecompressor()
//  ALDecompressor::~ALDecompressor()
//  deleteALDecompressor()
//  ALDecompress()
//
// DESCRIPTION
//
//  This file contains functions from the base class of ALDecompressor.
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
//  ALDecompressor::operator new()
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
//  void * ALDecompressor::operator new( size_t size )
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
//  size  :  The number of bytes needed to create a new ALDecompressor object.
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
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALDecompressor::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALDeompressor::ALDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Construct a decompressor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALDecompressor::ALDecompressor( ALCompressionType compression_type_int,
//                                  const char *compression_type_string );
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
//  This is the base class constructor for Decompressor objects.  Since this
//  class has a pure virtual function, you aren't going to be able to
//  call this guy directly.  Instead, it is called from the constructor
//  for guys such as ALGlDecompressor and ALPkDecompressor.
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
//   November 13, 1995  2.00A : First release.
//
//   February 14, 1996  2.0A : New release


AL_PROTO
ALDecompressor::ALDecompressor( ALCompressionType compression_type_int,  /* Tag public function */
                                const char AL_DLL_FAR *compression_type_string )
  : ALEngine( compression_type_int, compression_type_string )
{
}

//
// NAME
//
//  ALDecompressor::~ALDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The ALDecompressor destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALDecompressor::~ALDecompressor();
//
// C SYNOPSIS
//
//  void deleteALDecompressor( hALDecompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALDecompressor Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALDecompressor( this_object : hALDecompressor );
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This destructor will be called as the derived class is destroyed, such
//  as ALPkDecompressor or ALGlDecompressor.  It doesn't have to do anything,
//  so conceivably this function could just be eliminated, and we could use
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
//   November 13, 1995  2.00A : First release.
//
//   February 14, 1996  2.0A : New release

AL_PROTO
ALDecompressor::~ALDecompressor()  /* Tag public function */
{
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALDecompressor( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "deleteALDecompressor()" );
    delete (ALDecompressor *) this_object;
}

#endif

//
// NAME
//
//  ALDecompressor::Decompress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Decompress from an input object to an output object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALDecompressor::Decompress( ALStorage &input_object,
//                                  ALStorage &output_object );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALDecompress( hALDecompressor this_object,
//                    hALStorage input,
//                    hALStorage output );
//
// VB SYNOPSIS
//
//  Declare Function ALDecompress Lib "AL20LW"
//    (ByVal this_object&, ByVal input&, ByVal output& ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALDecompress( this_object : hALDecompressor;
//                         input : hALStorage;
//                         output : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALDecompressor object that
//                  is going to perform the decompression.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  input        :  A pointer or reference to the input storage object
//                  that is going to be decompressed.
//
//  output       :  A pointer or reference to the output object that is
//                  going to receive the decompressed data.
//
// DESCRIPTION
//
//  An ALDecompressor object only knows how to do one thing, and that is
//  expressed by way of this function.  Decompress() takes an input file,
//  and reads until it hits an EOF.  Output bytes are written to the
//  output object in plaintext.  The exact format of the input is determined
//  by the derived class.  ALDecompressor is an abstract base class that
//  can't be instantiated by itself.
//
//  Since the class can't be instantiated by itself, it's kind of bogus
//  to even suggest that ALDecompressor::Decompress() is a separate function,
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
//   November 13, 1995  2.00A : First release.
//
//   February 14, 1996  2.0A : New release

#if !defined( AL_NO_C )
extern "C" AL_LINKAGE int AL_FUNCTION
ALDecompress( hALDecompressor this_object, /* Tag public function */
              hALStorage input,
              hALStorage output,
              long compressed_length )
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompress" );
    AL_ASSERT_OBJECT( input, ALStorage, "ALDecompress" );
    AL_ASSERT_OBJECT( output, ALStorage, "ALDecompress" );
    return ( (ALDecompressor *) this_object )->Decompress( *(ALStorage *) input, *(ALStorage *) output, compressed_length );
}
#endif

