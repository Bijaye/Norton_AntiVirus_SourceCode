//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/PKENGNC.CPv   1.0   12 Mar 1998 12:17:24   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/PKENGNC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:24   DCHI
// Initial revision.
// 
//************************************************************************

//
// PKENGNC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALPkCompressor::operator new()
//  ALPkCompressor::ALPkCompressor()
//  newALPkCompressor()
//  ALPkCompressor::~ALPkCompressor()
//  ALPkCompressor::Compress()
//  ALPkCompressor::Clone()
//
// DESCRIPTION
//
//  All of the source required to support the PKWare compatible compressor.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "copyengn.h"
#include "pkengn.h"
#include "_openf.h"
#include "zutil.h"
#include "zlib.h"

//
// NAME
//
//  ALPkCompressor::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Memory allocator used when ArchiveLib resides in a 16 bit DLL.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  void * ALPkCompressor::operator new( size_t size )
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
//  size  :  The number of bytes that the compiler has decided will be
//           necessary to construct a new ALPkCompressor object.
//
// DESCRIPTION
//
//  When using a DLL, it is easy to get into a dangerous situation when
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
//  Incidentally, I suspect that this function never gets called.  If an
//  object of a derived archive class is being created, it should use
//  its own new operator, rendering this one useless.
//
// RETURNS
//
//  A pointer to some memory that should have been pulled out of the
//  heap for the DLL.
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
ALPkCompressor::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALPkCompressor::ALPkCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Constructor for the PkWare deflate compressor engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  ALPkCompressor::ALPkCompressor( int level = 6,
//                                  int window_bits = 13 /* or 15 */,
//                                  int mem_level = 6 /* or 8 */ )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  hALCompressor newALPkCompressor( int level, int window_bits, int mem_level );
//
// VB SYNOPSIS
//
//  Declare Function newALPkCompressor Lib "AL20LW"
//    (ByVal level%, ByVal window_bits%, ByVal mem_level% ) As Long
//
// DELPHI SYNOPSIS
//
//  function newALPkCompressor( level : Integer;
//                              window_bits : Integer;
//                              mem_level : Integer ) : hALCompressor;
//
// ARGUMENTS
//
//  level       : A number between 1 and 9.  1 gives the best speed, 9 gives
//                the best compression.
//
//  window_bits :
//
//  mem_level   : How much memory should be allocated for the internal
//                compression state. memLevel=1 uses minimum memory but
//                is slow and reduces compression ratio; memLevel=9 uses
//                maximum memory for optimal speed. (quoted from zlib.h)
//
//  Note that the C/VB/Delphi version of the ctor can use default parameter
//  values by simply passing AL_DEFAULT for each of the different numeric
//  parameters.
//
// DESCRIPTION
//
//  This constructor creates a new ALPkCompressor.  This compressor by
//  itself is a low cost object, with just a couple of data members that
//  help it remember what parameters to use when it comes time to
//  compress.  The compressor becomes a resource user when you call the
//  Compress() member function.
//
//  Create compressor objects when you are initializing a toolkit, or when
//  compressing files by hand, or when using an ALCompressedObject.
//
// RETURNS
//
//  The constructor doesn't return anything to C++ programmers.  C/VB/Delphi
//  programmers get a handle that points to the newly created object.
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
ALPkCompressor::ALPkCompressor( int level /* = 6 */,  /* Tag public function */
                                int window_bits /* = 13 or 15 */,
                                int mem_level /* = 8 or 6 */ )
    : ALCompressor( AL_COMPRESSION_DEFLATE, "" )
{
    miWindowBits = window_bits;
    miMemLevel = mem_level;
    switch ( level ) {
        case 9 :
            mszCompressionType = "Deflate Maximum";
            option = MAXIMUM;
            miLevel = level;
            break;
        case 8 :
        case 7 :
        case 6 :
        case 5 :
        case 4 :
        case 3 :
        case 2 :
            option = NORMAL;
            mszCompressionType = "Deflate Normal";
            miLevel = level;
            break;
        case 1 :
            option = FAST;
            mszCompressionType = "Deflate Fast";
            miLevel = 1;
            break;
        case 0 :
            option = NORMAL;
            mszCompressionType = "Stored";
            miLevel = 0;
            break;
        default :
            option = NORMAL;
            mszCompressionType = "???";
            miLevel = -1;
            break;
    }
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALCompressor AL_FUNCTION
newALPkCompressor( int level,  /* Tag public function */
                   int window_bits,
                   int mem_level )
{
    if ( level == AL_DEFAULT )
        level = 6;
    if ( window_bits == AL_DEFAULT )
        window_bits = AL_PK_DEFAULT_WINDOW_BITS;
    if ( mem_level == AL_DEFAULT )
        mem_level = AL_PK_DEFAULT_MEM_LEVEL;
    return (hALCompressor) new ALPkCompressor( level, window_bits, mem_level );
}

#endif

//
// NAME
//
//  ALPkCompressor::~ALPkCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The PKWare deflate Compressor destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  ALPkCompressor::~ALPkCompressor()
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
//   February 14, 1996  2.0A : New release
//

AL_PROTO
ALPkCompressor::~ALPkCompressor()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALPkCompressor: Attempt to delete invalid object" );
}

//
// I've tried two different ways to feed data to the compressor.
// The "normal" way is to just give the engine a pointer to a buffer,
// then work on filling that buffer by calling my own Read routines.
// The hacked way ought to work better.  When hacked, I use the buffer
// in the storage object, instead of creating a standalong buffer.  This
// means the compressor has to be a friend of ALStorage, but that's
// manageable.  Right now, Hacked seems to be okay, and it saves some space
// so that's what we're doing.
//
// Changing to unhacked is possible, but the buffer allocation should then
// be done dynamically, as this static buffer's going to have trouble
// when inside a DLL.
//

#define HACK_INPUT
#define HACK_OUTPUT

#if !defined( HACK_OUTPUT )
const int OUTBUF_LEN = 1024;
static char outbuf[ OUTBUF_LEN ];
#endif

#if !defined( HACK_INPUT )
const int INBUF_LEN = 1024;
char inbuf[ INBUF_LEN ];
#endif

//
// NAME
//
//  ALPkCompressor::Compress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Compress using the deflate algorithm.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  int ALPkCompressor::Compress( ALStorage& input,
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
//  which is found mostly in deflate.cpp
//
//  It's kind of hard to see what's going on here with the #ifdefs, but
//  it really isn't too complicated.  The deflate code wants a zstream
//  structure to work on.  We set up zstream to contain buffer pointers
//  to both the input and output streams.  We then sit in a loop calling
//  deflate() repeatedly.  The deflate() code compresses as much data
//  as it can, and returns.  After each call, I flush all the data I can,
//  load as much new data as I can, then try it again.  This repeats until
//  deflate() finally indicates that it's done.
//
//  After deflate is done, we have to check for errors on
//  any of the other objects involved in the compression, and return the
//  cumulative result.
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
//   February 14, 1996  2.0A : New release
//

int AL_PROTO
ALPkCompressor::Compress( ALStorage AL_DLL_FAR &input,  /* Tag public function */
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

    int result;
    z_stream stream;

    stream.zalloc = 0;
    stream.zfree = 0;
    result = deflateInit2( &stream,
                           miLevel,
                           DEFLATED,
                           -miWindowBits,
                           miMemLevel,
                           0 );
    if ( result != 0 ) {
        deflateEnd( &stream );
        return mStatus.SetError( AL_CANT_CREATE_ENGINE,
                                 "Error creating deflate engine. "
                                 "ZLIB returned %d\n",
                                 result );
    }
#if defined( HACK_OUTPUT )
    output.FlushBuffer();
    stream.avail_out = output.muBufferSize;
    stream.next_out = (Byte ZL_FAR *) output.mpcBuffer;
#else
    stream.avail_out = OUTBUF_LEN;
    stream.next_out = (Byte ZL_FAR *) outbuf;
#endif
    stream.avail_in = 0;
#if !defined( HACK_INPUT )
    stream.next_in = (Byte ZL_FAR *) inbuf;
#endif
    for ( ; ; ) {
        if ( stream.avail_in == 0 ) {
#if !defined( HACK_INPUT )
            stream.avail_in = input.ReadBuffer( (unsigned char *) inbuf, INBUF_LEN );
            stream.next_in = (unsigned char *) inbuf;
            if ( stream.avail_in == 0 )
                break;
#else
            if ( input.muReadIndex >= input.muBufferValidData ) {
                if ( input.LoadBuffer( input.mlFilePointer ) < 0 )
                    break;
            }
            stream.avail_in = input.muBufferValidData - input.muReadIndex;
            stream.next_in = input.mpcBuffer + input.muReadIndex;
            input.muReadIndex += input.muBufferValidData;
#endif
        }
        result = deflate( &stream, Z_NO_FLUSH );
#if defined( HACK_OUTPUT )
        if ( stream.avail_out != output.muBufferSize ) {
            output.muWriteIndex = output.muBufferSize - stream.avail_out;
            output.FlushBuffer();
            stream.avail_out = output.muBufferSize;
            stream.next_out = (Byte ZL_FAR *) output.mpcBuffer;
        }
#else
        if ( stream.avail_out != OUTBUF_LEN ) {
            output.WriteBuffer( (unsigned char*) outbuf, OUTBUF_LEN - stream.avail_out );
            stream.avail_out = OUTBUF_LEN;
            stream.next_out = (Byte ZL_FAR *) outbuf;
        };
#endif
        if ( output.mStatus < AL_SUCCESS || result != Z_OK) {
            deflateEnd( &stream );
            return mStatus = output.mStatus;
        }
    }
//
// Now its time to start flushing
//
    for ( ; ; ) {
        result = deflate( &stream, Z_FINISH );
#if defined( HACK_OUTPUT )
        if ( stream.avail_out != output.muBufferSize ) {
            output.muWriteIndex = output.muBufferSize - stream.avail_out;
            output.FlushBuffer();
            stream.avail_out = output.muBufferSize;
            stream.next_out = (Byte ZL_FAR *) output.mpcBuffer;
#else
        if ( stream.avail_out != OUTBUF_LEN ) {
            output.WriteBuffer( (unsigned char *) outbuf, OUTBUF_LEN - stream.avail_out );
            stream.avail_out = OUTBUF_LEN;
            stream.next_out = (Byte ZL_FAR *) outbuf;
#endif
        };
        if ( result == Z_STREAM_END )
            break;
    }
    deflateEnd( &stream );

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
//  ALPkCompressor::Clone()
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
//  #include "pkengn.h"
//
//  ALPkCompressor::Clone( int engine_type );
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
//                  AL_COMPRESSION_DEFAULT, AL_COMPRESSION_DEFLATE,
//                  AL_DEFLATE_COPY, or AL_COPY will cause this function to
//                  create a clone.  Any other
//                  value (for example, AL_GREENLEAF), will return a 0,
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
//  pointer to a PKWare engine to the archiving code, and it can then in
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
//   February 14, 1996  2.0A : New release
//

ALCompressor AL_DLL_FAR * AL_PROTO
ALPkCompressor::Clone( int engine_type ) const  /* Tag public function */
{
    _option pk_option;
    ALPkCompressor *compressor;
    switch ( engine_type ) {
        case AL_COMPRESSION_COPY :
        case AL_COMPRESSION_DEFLATE_COPY :
            return new ALCopyCompressor();
        case AL_COMPRESSION_DEFLATE + 3 :
            pk_option = SUPER_FAST;
            break;
        case AL_COMPRESSION_DEFLATE + 2 :
            pk_option = FAST;
            break;
        case AL_COMPRESSION_DEFLATE + 1 :
            pk_option = MAXIMUM;
            break;
        case AL_COMPRESSION_DEFLATE + 0 :  /* Normal */
        case AL_COMPRESSION_DEFAULT :
            pk_option = NORMAL;
            break;
        default :
            return 0;
    }
    compressor = new ALPkCompressor( miLevel, miWindowBits, miMemLevel );
    if ( compressor )
        compressor->option = pk_option;
    return compressor;
}

