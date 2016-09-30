//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/COPYC.CPv   1.0   12 Mar 1998 12:15:28   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/COPYC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:28   DCHI
// Initial revision.
// 
//************************************************************************

//
// COPYC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//   ALCopyCompressor::operator new()
//   ALCopyCompressor::ALCopyCompressor()
//   newALCopyCompressor()
//   ALCopyCompressor::~ALCopyCompressor()
//   ALCopyCompressor::Compress()
//   ALCopyCompressor::Clone()
//
// DESCRIPTION
//
//  This file contains the source needed to support the compressor class
//  ALCopyCompressor.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"

#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "copyengn.h"
#include "_openf.h"

//
// NAME
//
//  ALCopyCompressor::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The memory allocator used with DLL versions of ArchiveLib.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "copyengn.h"
//
//  void * ALCopyCompressor::operator new( size_t size )
//
// C SYNOPSIS
//
//  None, this is an internal C++ function.
//
// VB SYNOPSIS
//
//  None, this is an internal C++ function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal C++ function.
//
// ARGUMENTS
//
//  size  :  The amount of storage that needs to be allocated for
//           this object.
//
// DESCRIPTION
//
//  When using the DLL version of ArchiveLib, it is a good idea to
//  allocate the storage for objects from inside the DLL, since they
//  will be freed inside the DLL.  If we don't have the new operator
//  for a class, its storage will be allocated from the EXE before
//  the constructor code is called.  Then, when it is time to free
//  the storage, the delete operator will be called inside the DLL.
//  Not good, right?
//
//  By providing our own version of operator new inside this class, we
//  ensure that all memory allocation for the class will be done from
//  inside the DLL, not the EXE calling the DLL.
//
//
// RETURNS
//
//  A pointer to the storage.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#if defined( AL_BUILDING_DLL )
void AL_DLL_FAR * AL_PROTO
ALCopyCompressor::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}
#endif

//
// NAME
//
//  ALCopyCompressor::ALCopyCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Default constructor for the copy compression engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "copyengn.h"
//
//  ALCopyCompressor::ALCopyCompressor();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "copyengn.h"
//
//  hALCompressor newALCopyCompressor();
//
// VB SYNOPSIS
//
//  Declare Function newALCopyCompressor Lib "AL20LW" () As Long
//
// DELPHI SYNOPSIS
//
//  function newALCopyCompressor : hALCompressor;
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function creates a new copy compression engine.  The copy
//  engine is used to do simply binary copies of an uncompressed
//  object into an archive or other compressed object.  Since no compression
//  is taking place, the copy compressor is supposed to be fast and
//  efficient.
//
//  Note that under Visual Basic or C, it is up to the user to destroy
//  this engine by calling deleteCompressor().  C++ users only need to
//  call the destructor explicitly when they have created the object
//  dynamically using the new operator.
//
// RETURNS
//
//  The C++ function returns nothing.  The C and VB functions return the
//  handle of the newly created compressor.  A value of 0 for this handle
//  means the object could not be properly created.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

AL_PROTO
ALCopyCompressor::ALCopyCompressor()  /* Tag public function */
    : ALCompressor( AL_COMPRESSION_COPY, "Binary copy" )
{
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE
hALCompressor AL_FUNCTION newALCopyCompressor() /* Tag public function */
{
    return (hALCompressor) new ALCopyCompressor();
}

#endif

//
// NAME
//
//  ALCopyCompressor::~ALCopyCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Destructor for the copy compression engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "copyengn.h"
//
//  ALCopyCompressor::~ALCopyCompressor();
//
// C SYNOPSIS
//
//  None, see deleteCompressor().
//
// VB SYNOPSIS
//
//  None, see deleteCompressor().
//
// DELPHI SYNOPSIS
//
//  None, see deleteCompressor().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor has absolutely nothing to do.  In the debug
//  versions of the library, the dtor checks to be sure that it
//  is operating on the right type of object.
//
//  Note that more often than not, this virtual function will be
//  called using the base class.  In fact, C/VB/Delphi users don't
//  even have a choice about it.
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
//  May 22, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release

AL_PROTO
ALCopyCompressor::~ALCopyCompressor()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALCopyCompressor: Attempt to delete invalid object" );
}

//
// NAME
//
//  ALCopyCompressor::Compress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Perform a binary copy of input to output.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "copyengn.h"
//
//  int ALCopyCompressor::Compress( ALStorage& input,
//                                  ALStorage& output );
//
// C SYNOPSIS
//
//  None, see the base class function ALCompress().
//
// VB SYNOPSIS
//
//  None, see the base class function ALCompress().
//
// DELPHI SYNOPSIS
//
//  None, see the base class function ALCompress().
//
// ARGUMENTS
//
//  input  : A reference to the input storage object.
//
//  output : A reference to the output storage object.
//
// DESCRIPTION
//
//  This is ostensibly a compression engine, but really all it does
//  is copy input directly to the output.  The most exciting thing it
//  does during the entire process is initialize CRC checking.
//
//  This function will almost always be called indirectly, by means of
//  a virtual function call off the base class.  That's why you won't
//  see any C, VB, or Delphi functions here.  Those languages will only
//  be able to call the Compress() routine by way of the base class.
//
// RETURNS
//
//  AL_SUCCESS, or < AL_SUCCESS if something bad happens.
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

int AL_PROTO
ALCopyCompressor::Compress( ALStorage AL_DLL_FAR &input,  /* Tag public function */
                            ALStorage AL_DLL_FAR &output )
{
//
// Open the input and output files, and initialize CRC 32 checking.
//
    ALOpenFiles files( input, output );
    if ( input.mStatus < AL_SUCCESS )
        return mStatus = input.mStatus;
    if ( output.mStatus < AL_SUCCESS )
        return mStatus = output.mStatus;
    input.InitCrc32();
//
// Now read all the data from the input file, and write it to the
// output file.
//
    int c;
    for ( ; ; ) {
        c = input.ReadChar();
        if ( c < 0 )
            break;
        output.WriteChar( c );
    }
//
// Finally, check on the error status codes, then return.
//
    if ( input.mStatus < AL_SUCCESS )
        return mStatus = input.mStatus;
    if ( output.mStatus < AL_SUCCESS )
        return mStatus = output.mStatus;
    return mStatus;
}

//
// NAME
//
//  ALCopyCompressor::Clone()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a copy of an existing Compressor
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "copyengn.h"
//
//  ALCopyCompressor::Clone( int engine_type );
//
// C SYNOPSIS
//
//  No C equivalent.
//
// VB SYNOPSIS
//
//  No VB equivalent.
//
// DELPHI SYNOPSIS
//
//  No Delphi equivalent.
//
// ARGUMENTS
//
//  engine_type  :  This argument indicates what sort of engine the
//                  caller wants to create.  A value of either
//                  AL_COMPRESSION_DEFAULT or AL_COMPRESSION_COPY will
//                  cause this function to create a clone.  Any other
//                  value (for example, AL_DEFLATE), will return a 0,
//                  indicating that this object doesn't know how to
//                  perform that sort of compression.
//
// DESCRIPTION
//
//  Although this is a public function, it isn't really of any use
//  to end users.  Clone() is a virtual function for the base class
//  ALCompressor, and can be called to create a duplicate of an
//  existing compression engine.
//
//  Why is this useful?  It is useful because it allows us to use
//  what is the equivalent of a virtual constructor.  We can pass a
//  pointer to a copy engine to the archiving code, and it can then in
//  turn stuff copies of that engine into an ALEntryList without
//  having any idea what sort of compression engine it is actually creating.
//
// RETURNS
//
//  A copy of a newly created compression engine.  When this routine is
//  called, it will usually be called via a virtual function from a pointer
//  to a base class object, which means the resulting pointer will be
//  treated as an ALCompressor * by the code.
//
//  If this routine doesn't know how to create an engine of the correct type,
//  it returns a 0.  When performing compression into an archive, the
//  Clone() functions will usually be called with the AL_DEFAULT_COMPRESSION
//  engine type, meaning they are happy to take a copy of whatever engine
//  happens to show up first in the toolkit.
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


ALCompressor AL_DLL_FAR * AL_PROTO
ALCopyCompressor::Clone( int engine_type ) const  /* Tag public function */
{
    switch ( engine_type ) {
        case AL_COMPRESSION_DEFAULT :
        case AL_COMPRESSION_COPY :
            return new ALCopyCompressor();
    }
    return 0;
}

