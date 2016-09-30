//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/GLENGNC.CPv   1.0   12 Mar 1998 12:16:20   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/GLENGNC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:20   DCHI
// Initial revision.
// 
//************************************************************************

//
// GLENGNC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALGlCompressor::operator new()
//  ALGlCompressor::ALGlCompressor()
//  newALGlCompressor()
//  ALGlCompressor::~ALGlCompressor()
//  ALGlCompressor::Compress()
//  ALGlCompressor::WriteEngineData()
//  ALGlCompressor::ReadEngineData()
//  ALGlCompressor::Clone()
//
// DESCRIPTION
//
//  This file contains the front end to the Greenleaf compression engine.
//  This contains everything but the actual low level compression
//  and expansion code, which can be found in _RE.CPP and _RC.CPP.  Those
//  two source files are shrouded though, so you won't get a tremendous
//  amount of detail!
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New Release
//
//   March 20, 1996     2.01A : Excluded this from the test drive build
//


#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "glengn.h"
#include "_openf.h"
#include "_r.h"


//
// NAME
//
//  ALGlCompressor::operator new()
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
//  void * ALGlCompressor::operator new( size_t size )
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
ALGlCompressor::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALGlCompressor::ALGlCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Constructor for the Greenleaf compressor engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  ALGlCompressor::ALGlCompressor(
//      short int compression_level = AL_GREENLEAF_LEVEL_2 /* or 4 */
//      short int fail_uncompressible = 0 );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  hALCompressor newALGlCompressor( short int compression_level,
//                                   short int fail_uncompressible );
//
// VB SYNOPSIS
//
//  Declare Function newALGlCompressor Lib "AL20LW"
//    (ByVal compression_level%, ByVal fail_uncompressible% ) As Long
//
// DELPHI SYNOPSIS
//
//  function newALGlCompressor( compression_level : Integer;
//                              fail_uncompressible : Integer ) : hALCompressor;
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
//  fail_uncompressible : This flag is used to indicate the disposition
//                        of an uncompressible file.  If this flag is set,
//                        the compression of an incompressible file will
//                        be interrupted, and the file will be recompressed
//                        using a binary straight copy.  Note that this requires
//                        a Seek() operation!
//
// DESCRIPTION
//
//  The Greenleaf Compressor has a very simple constructor.  All it has
//  to do is initialize a couple of data members, then pass some additional
//  data up to the base class ctor.  Note that when it isn't actually
//  compressing, this object is pretty teeny.
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
//   May 26, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALGlCompressor::ALGlCompressor( short int compression_level /* = AL_GREENLEAF_LEVEL_2 */,  /* Tag public function */
                                short int fail_uncompressible /* = 0 */ )
    :  ALCompressor( AL_COMPRESSION_GREENLEAF, "Greenleaf" )
{
    miCompressionLevel = compression_level;
    miFailUncompressible = fail_uncompressible;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALCompressor AL_FUNCTION
newALGlCompressor( short int compression_level,  /* Tag public function */
                   short int fail_uncompressible )
{
    if ( compression_level == AL_DEFAULT )
#if defined( AL_FLAT_MODEL )
        compression_level = AL_GREENLEAF_LEVEL_4;
#else
        compression_level = AL_GREENLEAF_LEVEL_2;
#endif
    return (hALCompressor)
        new ALGlCompressor( compression_level, fail_uncompressible );
}

#endif

//
// NAME
//
//  ALGlCompressor::~ALGlCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The Greenleaf Compressor destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  ALGlCompressor::~ALGlCompressor()
//
// C SYNOPSIS
//
//  None.  C programmers need to call deleteALCompressor().
//
// VB SYNOPSIS
//
//  None.  VB programmers need to call deleteALCompressor().
//
// DELPHI SYNOPSIS
//
//  None.  Delphi programmers need to call deleteALCompressor().
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
ALGlCompressor::~ALGlCompressor()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALGlCompressor: attempt to delete invalid object" );
}

//
// NAME
//
//  ALGlCompressor::Compress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Compress using Greeenleaf's algorithms.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  int ALGlCompressor::Compress( ALStorage& input,
//                                ALStorage& output );
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
//  This is the virtual function that is called to compress data.
//  This section of code is really just a front end to the real engine,
//  which is found in _RC.CPP.  The first thing we do here
//  is create an RCompress object, which allocates all of the
//  storage we need to perform the compression.  In a tight memory
//  situation, that may well fail, so we check its status before moving
//  on.  If it succeeded, we can call the low level compression function
//  to do the real work.
//
//  After the compress function returns, we have to check for errors on
//  any of the other objects involved in the compression, and return the
//  cumulative result.
//
//  If the miFailUncompressible option is set, there is always a possiblity
//  that the compressor will return an indication that the file could
//  not be compressed.  If this is the case, we change the compression
//  level in this to AL_GREENLEAF_COPY, then perform a binary copy of
//  the data.
//
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
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALGlCompressor::Compress( ALStorage AL_DLL_FAR &input,  /* Tag public function */
                          ALStorage AL_DLL_FAR &output )
{
    int incompressible;

    ALOpenFiles files( input, output );
        if ( input.mStatus < AL_SUCCESS )
            return mStatus = input.mStatus;
        if ( output.mStatus < AL_SUCCESS )
            return mStatus = output.mStatus;

    long input_start = input.Tell();
    long output_start = output.Tell();
    input.InitCrc32();
    if ( miCompressionLevel != AL_GREENLEAF_COPY ) {
        RCompress rc( input,
                      output,
                      miCompressionLevel + 10,
                      miFailUncompressible );

        if ( rc.mStatus < 0 )
            return mStatus = rc.mStatus;
        else
            incompressible = rc.Compress();
        if ( rc.mStatus < 0 )
            return mStatus = rc.mStatus;
        else if ( input.mStatus < 0 )
            return mStatus = input.mStatus;
        else if ( output.mStatus < 0 )
            return mStatus = output.mStatus;
    } else
        incompressible = 1;
    if ( incompressible ) {
        input.Seek( input_start );
        output.Seek( output_start );
        input.InitCrc32();
        miCompressionLevel = AL_GREENLEAF_COPY;
        int c;
        for ( ; ; ) {
            c = input.ReadChar();
            if ( c < 0 )
                break;
            output.WriteChar( c );
        }
        if ( input.mStatus < AL_SUCCESS )
            return mStatus = input.mStatus;
        if ( output.mStatus < AL_SUCCESS )
            return mStatus = output.mStatus;
    }
    return mStatus;
}

//
// NAME
//
//  ALGlCompressor::WriteEngineData()
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
//  int ALGlCompressor::WriteEngineData( ALStorage *archive );
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
//  Every compression engine used in ArchiveLib gets the opportunity
//  to store data it needs to save in order to characterize its compression
//  process.  The Greenleaf compression engine only needs to save a single
//  integer, which contains the compression level used.  This is the
//  function that does so.
//
//  Data like this is stored in string format, which consists of a single
//  short integer describing the number of bytes in the string, followed
//  by the string.  We store in this portable format so that even a program
//  that doesn't know about compression engines would be able to read in
//  archive directory data.
//
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
ALGlCompressor::WriteEngineData( ALStorage AL_DLL_FAR * archive )  /* Tag protected function */
{
    archive->WriteGlShort( 2 );
    return archive->WriteGlShort( miCompressionLevel );
}

//
// NAME
//
//  ALGlCompressor::ReadEngineData()
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
//  Every compression engine used in ArchiveLib gets the opportunity
//  to store data it needs to save in order to characterize its compression
//  process.  The Greenleaf compression engine only needs to save a single
//  integer, which contains the compression level used.
//
//  During the creation of the compression engine, this function gets called
//  in order to load the engine's private data.  All we do is read in
//  the compression level, along with a little error checking.
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
ALGlCompressor::ReadEngineData( ALStorage AL_DLL_FAR * archive )  /* Tag protected function */
{
    short temp;
    archive->ReadGlShort( temp );
    AL_ASSERT( temp == 2, "ReadEngineData: engine data size is not 2, it should be" );
    return archive->ReadGlShort( miCompressionLevel );
}

//
// NAME
//
//  ALGlCompressor::Clone()
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
//  #include "glengn.h"
//
//  ALGlCompressor::Clone( int engine_type );
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
//  pointer to a Greenleaf engine to the archiving code, and it can then in
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
//   February 14, 1996  2.0A : New Release
//


ALCompressor AL_DLL_FAR * AL_PROTO
ALGlCompressor::Clone( int engine_type ) const  /* Tag public function */
{
    switch ( engine_type ) {
        case AL_COMPRESSION_DEFAULT :
        case AL_COMPRESSION_GREENLEAF :
            return new ALGlCompressor( miCompressionLevel, miFailUncompressible );
    }
    return 0;
}

#endif // #if !defined( AL_DEMO )

