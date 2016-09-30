//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/GLENGND.CPv   1.0   12 Mar 1998 12:16:18   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/GLENGND.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:18   DCHI
// Initial revision.
// 
//************************************************************************

//
// GLENGND.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALGlDecompressor::operator new()
//  ALGlDecompressor::ALGlDecompressor()
//  newALGlDecompressor()
//  ALGlDecompressor::~ALGlDecompressor()
//  ALGlDecompressor::Decompress()
//  ALGlDecompressor::WriteEngineData()
//  ALGlDecompressor::ReadEngineData()
//  ALGlDecompressor::Clone()
//
// DESCRIPTION
//
//  This file contains the front end to the Greenleaf decompression engine.
//  This contains everything but the actual low level decompression
//  code, which can be found in _RE.CPP .  That source files is shrouded
//  though, so you won't get a tremendous amount of detail!
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New Release
//
//   March 20, 1996     2.01A : Added #ifdefs to exclude this code from
//                              the test drive version of the library

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO ) //Exclude all code in this file from the test drive

#include "glengn.h"
#include "_openf.h"
#include "_r.h"

//
// NAME
//
//  ALGlDecompressor::operator new()
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
//  #include "glengn.h"
//
//  void * ALGlDecompressor::operator new( size_t size )
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
//   February 14, 1996  2.0A : New Release
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALGlDecompressor::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALGlDecompressor::ALGlDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Constructor for the Greenleaf decompressor engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  ALGlDecompressor::ALGlDecompressor(
//      short int compression_level = AL_GREENLEAF_LEVEL_2 /* or 4 */ );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  hALDecompressor newALGlDecompressor( short int compression_level );
//
// VB SYNOPSIS
//
//  Declare Function newALGlDecompressor Lib "AL20LW"
//    (ByVal compression_level% ) As Long
//
// DELPHI SYNOPSIS
//
//  function newALGlDecompressor( compression_level : Integer ) : hALCompressor;
//
// ARGUMENTS
//
//  compression_level   : This is one of the enumerated types found in ALDEFS.H,
//                        namely AL_GREENLEAF_LEVEL_0 through
//                        AL_GREENLEAF_LEVEL_4.  Level 4 gives the most
//                        compression, but takes up the most memory as well.
//                        Note that 16 bit programs default to level 2,
//                        32 bit programs default to level 4.
//
// DESCRIPTION
//
//  The Greenleaf Decompressor has a very simple constructor.  All it has
//  to do is initialize a data member, then pass some additional
//  data up to the base class ctor.  Note that when it isn't actually
//  decompressing, this object is pretty teeny.
//
//  Note that under Visual Basic or C, it is up to the user to destroy
//  this engine by calling deleteDecompressor().  C++ users only need to
//  call the destructor explicitly when they have created the object
//  dynamically using the new operator.
//
// RETURNS
//
//  The C++ function returns nothing.  The C and VB functions return the
//  handle of the newly created decompressor.  A value of 0 for this handle
//  means the object could not be properly created.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 26, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALGlDecompressor::ALGlDecompressor(  /* Tag public function */
    short int compression_level /* = AL_GREENLEAF_LEVEL_2 */ )
    :  ALDecompressor( AL_COMPRESSION_GREENLEAF, "Greenleaf" )
{
    miCompressionLevel = compression_level;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALDecompressor AL_FUNCTION
newALGlDecompressor( short int compression_level )  /* Tag public function */
{
    if ( compression_level == AL_DEFAULT )
#if defined( AL_FLAT_MODEL )
        compression_level = AL_GREENLEAF_LEVEL_4;
#else
        compression_level = AL_GREENLEAF_LEVEL_2;
#endif
    return (hALDecompressor)
        new ALGlDecompressor( compression_level );
}

#endif

//
// NAME
//
//  ALGlDecompressor::~ALGlDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The Greenleaf Decompressor destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  ALGlDecompressor::~ALGlDecompressor()
//
// C SYNOPSIS
//
//  None.  C programmers need to call deleteALDecompressor().
//
// VB SYNOPSIS
//
//  None.  VB programmers need to call deleteALDecompressor().
//
// DELPHI SYNOPSIS
//
//  None.  Delphi programmers need to call deleteALDecompressor().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor for objects of this class doesn't have to do
//  anything.  In debug mode, we at least check for the validity
//  of the object.
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
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALGlDecompressor::~ALGlDecompressor() /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALGlDecompressor: attempt to delete invalid object" );
}

//
// NAME
//
//  ALGlDecompressor::Decompress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Decompress using Greeenleaf's algorithms.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  int ALGlDecompressor::Decompress( ALStorage &input,
//                                    ALStorage &output,
//                                    long compressed_length );
//
// C SYNOPSIS
//
//  None, see the base class function ALDecompress().
//
// VB SYNOPSIS
//
//  None, see the base class function ALDecompress().
//
// DELPHI SYNOPSIS
//
//  None, see the base class function ALDecompress().
//
// ARGUMENTS
//
//  input  : A reference to the input storage object.
//
//  output : A reference to the output storage object.
//
//  compressed_length : A long value indicating how long the compressed
//                      object is.  This helps to tell the decompressor
//                      when to quit.
// DESCRIPTION
//
//  This is the virtual function that is called to expand a compressed
//  object. This section of code is really just a front end to the real
//  engine, which is found in _RE.CPP.  The first thing we do here
//  is create an RExpand object, which allocates all of the
//  storage we need to perform the decompression.  In a tight memory
//  situation, that may well fail, so we check its status before moving
//  on.  If it succeeded, we can call the low level expansion function
//  to do the real work.
//
//  After the expand function returns, we have to check for errors on
//  any of the other objects involved in the expansion, and return the
//  cumulative result.
//
//  This function now properly supports the incompressible option.  When
//  the type of compression is selectec, via a compression level of
//  AL_GREENLEAF_COPY, we just do a straight binary copy here, instead
//  of calling the actual compressor.
//
//  This function will almost always be called indirectly, by means of
//  a virtual function call off the base class.  That's why you won't
//  see any C, VB, or Delphi functions here.  Those languages will only
//  be able to call the Compress() routine by way of the base class.
//
// RETURNS
//
//  AL_SUCCESS in the event of a success, an error code < AL_SUCCESS
//  if a failure occurred.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALGlDecompressor::Decompress( ALStorage AL_DLL_FAR &input,  /* Tag public function */
                              ALStorage AL_DLL_FAR &output,
                              long compressed_length )
{
    ALOpenFiles files( input, output );
    if ( input.mStatus < 0 )
        return mStatus = input.mStatus;
    else if ( output.mStatus < 0 )
        return mStatus = output.mStatus;

    output.InitCrc32();
    if ( miCompressionLevel == AL_GREENLEAF_COPY ) {
        int c;
        for ( ; compressed_length ; compressed_length-- ) {
            c = input.ReadChar();
            if ( c < 0 )
                break;
            output.WriteChar( c );
        }
    } else {
        RExpand re( input, output, compressed_length, miCompressionLevel + 10 );

        if ( re.mStatus < 0 )
            return mStatus = re.mStatus;
        else
            re.Expand();
        if ( re.mStatus < 0 )
            return mStatus = re.mStatus;
    }
    if ( input.mStatus < 0 )
        return mStatus = input.mStatus;
    else if ( output.mStatus < 0 )
        return mStatus = output.mStatus;
    return mStatus;
}

//
// NAME
//
//  ALGlDecompressor::WriteEngineData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Write the Greenleaf specific engine data to a file.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  int ALGlDecompressor::WriteEngineData( ALStorage *archive );
//
// C SYNOPSIS
//
//  None, this is an internal C++ protected function.
//
// VB SYNOPSIS
//
//  None, this is an internal C++ protected function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal C++ protected function.
//
// ARGUMENTS
//
//  archive      :  A pointer to the storage object where the data is to
//                  be written.  Under normal circumstances, this will always
//                  be either the storage object that holds the archive, or
//                  the storage object that holds an ALComprepssedObject.
//
// DESCRIPTION
//
//  Every decompression engine used in ArchiveLib gets the opportunity
//  to store data it needs to save in order to characterize its decompression
//  process.  The Greenleaf decompression engine only needs to save a single
//  integer, which contains the compression level used.  This is the
//  function that does so.
//
//  Data like this is stored in string format, which consists of a single
//  short integer describing the number of bytes in the string, followed
//  by the string.  We store in this portable format so that even a program
//  that doesn't know about compression engines would be able to read in
//  archive directory data.
//
// RETURNS
//
//  AL_SUCCESS if the data was written properly, else an error code
//  less than AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALGlDecompressor::WriteEngineData( ALStorage AL_DLL_FAR * archive )  /* Tag protected function */
{
    archive->WriteGlShort( 2 );
    return archive->WriteGlShort( miCompressionLevel );
}

//
// NAME
//
//  ALGlDecompressor::ReadEngineData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read the Greenleaf specific engine data from a file.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  int ALGlCompressor::ReadEngineData( ALStorage *archive );
//
// C SYNOPSIS
//
//  None, this is an internal C++ protected function.
//
// VB SYNOPSIS
//
//  None, this is an internal C++ protected function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal C++ protected function.
//
// ARGUMENTS
//
//  archive      :  A pointer to the storage object where the data is to
//                  be read.  Under normal circumstances, this will always
//                  be either the storage object that holds the archive, or
//                  the storage object that holds an ALCompressedObject.
//
// DESCRIPTION
//
//  Every decompression engine used in ArchiveLib gets the opportunity
//  to store data it needs to save in order to characterize its decompression
//  process.  The Greenleaf decompression engine only needs to save a single
//  integer, which contains the compression level used.
//
//  During the creation of the decompression engine, this function gets called
//  in order to load the engine's private data.  All we do is read in
//  the compression level, along with a little error checking.
//
//  Data like this is stored in string format, which consists of a single
//  short integer describing the number of bytes in the string, followed
//  by the string.  We store in this portable format so that even a program
//  that doesn't know about decompression engines would be able to read in
//  archive directory data.
//
// RETURNS
//
//  AL_SUCCESS if the data was read in properly, else an error code
//  less than AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALGlDecompressor::ReadEngineData( ALStorage AL_DLL_FAR * archive )  /* Tag protected function */
{
    short temp;
    archive->ReadGlShort( temp );
    AL_ASSERT( temp == 2, "ReadEngineData: engine data size is not 2, it should be" );
    return archive->ReadGlShort( miCompressionLevel );
}

//
// NAME
//
//  ALGlDecompressor::Clone()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a copy of an existing Decompressor
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  ALGlDecompressor::Clone( int engine_type );
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
//                  AL_COMPRESSION_DEFAULT or AL_COMPRESSION_GREENLEAF
//                  will cause this function to create a clone.  Any other
//                  value (for example, AL_DEFLATE), will return a 0,
//                  indicating that this object doesn't know how to
//                  perform that sort of decompression.
//
// DESCRIPTION
//
//  Although this is a public function, it isn't really of any use
//  to end users.  Clone() is a virtual function for the base class
//  ALDecompressor, and can be called to create a duplicate of an
//  existing decompression engine.
//
//  Why is this useful?  It is useful because it allows us to use
//  what is the equivalent of a virtual constructor.  We can pass a
//  pointer to a Greenleaf engine to the archiving code, and it can then in
//  turn stuff copies of that engine into an ALEntryList without
//  having any idea what sort of compression engine it is actually creating.
//
// RETURNS
//
//  A copy of a newly created compression engine.  When this routine is
//  called, it will usually be called via a virtual function from a pointer
//  to a base class object, which means the resulting pointer will be
//  treated as an ALDecompressor * by the code.
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
//   February 14, 1996  2.0A : New Release
//

ALDecompressor AL_DLL_FAR * AL_PROTO
ALGlDecompressor::Clone( int engine_type ) const  /* Tag public function */
{
    switch ( engine_type ) {
        case AL_COMPRESSION_DEFAULT :
        case AL_COMPRESSION_GREENLEAF :
            return new ALGlDecompressor( miCompressionLevel );
    }
    return 0;
}

#endif //#if !defined( AL_DEMO )

