//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/PKENGND.CPv   1.0   12 Mar 1998 12:17:24   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/PKENGND.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:24   DCHI
// Initial revision.
// 
//************************************************************************

//
// PKENGND.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALPkDecompressor::operator new()
//  ALPkDecompressor::ALPkDecompressor()
//  newALPkDecompressor()
//  ALPkDecompressor::~ALPkDecompressor()
//  ALPkDecompressor::Decompress()
//  ALPkDecompressor::Clone()
//
// DESCRIPTION
//
//  All of the C++ source to support the PKWare decompressor class.
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
//  ALPkDecompressor::operator new()
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
//  void * ALPkDecompressor::operator new( size_t size )
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
//           necessary to construct a new ALPkDecompressor object.
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
ALPkDecompressor::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALPkDecompressor::ALPkDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Constructor for the PkWare deflate decompressor engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  ALPkDecompressor::ALPkDecompressor();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  hALDecompressor newALPkDecompressor( void );
//
// VB SYNOPSIS
//
//  Declare Function newALPkDecompressor Lib "AL20LW" As Long
//
// DELPHI SYNOPSIS
//
//  function newALPkDecompressor : hALDecompressor;
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This constructor creates a new ALPkDecompressor.  This decompressor by
//  itself is almost a no-cost object, with no derived class data members that
//  need to be initialized.  The decompressor becomes a resource user when you
//  call the Decompress() member function.
//
//  Create decompressor objects when you are initializing a toolkit, or when
//  decompressing files by hand, or when using an ALCompressedObject.
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
ALPkDecompressor::ALPkDecompressor()  /* Tag public function */
    : ALDecompressor( AL_COMPRESSION_DEFLATE, "Inflate" )
{
    option = NORMAL;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALDecompressor AL_FUNCTION
newALPkDecompressor( void )  /* Tag public function */
{
    return (hALDecompressor) new ALPkDecompressor;
}

#endif

//
// NAME
//
//  ALPkDecompressor::~ALPkDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The PKWare deflate Decompressor destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  ALPkDecompressor::~ALPkDecompressor()
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
//   February 14, 1996  2.0A : New release
//

AL_PROTO
ALPkDecompressor::~ALPkDecompressor()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALPkDecompressor: Attempt to delete invalid object" );
}

//
// I've tried two different ways to feed data to the decompressor.
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
//  ALPkDecompressor::Decompress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Decompress using the deflate algorithm.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkengn.h"
//
//  int ALPkDecompressor::Decompress( ALStorage& input,
//                                    ALStorage& output );
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
// DESCRIPTION
//
//  This is the virtual function that is called to decompress data.
//  This section of code is really just a front end to the real engine,
//  which is found in many source files in ..\CPP_ZLIB\*.CPP.
//
//  It's kind of hard to see what's going on here with the #ifdefs, but
//  it really isn't too complicated.  The inflate code wants a zstream
//  structure to work on.  We set up zstream to contain buffer pointers
//  to both the input and output streams.  We then sit in a loop calling
//  inflate() repeatedly.  The inflate() code decompresses as much data
//  as it can, and returns.  After each call, I flush all the data I can,
//  load as much new data as I can, then try it again.  This repeats until
//  inflate() finally indicates that it's done.
//
//  After decompression is done, we have to check for errors on
//  any of the other objects involved in the compression, and return the
//  cumulative result.
//
//  This function will almost always be called indirectly, by means of
//  a virtual function call off the base class.  That's why you won't
//  see any C, VB, or Delphi functions here.  Those languages will only
//  be able to call the Decompress() routine by way of the base class.
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
ALPkDecompressor::Decompress( ALStorage AL_DLL_FAR & input, /* Tag public function */
                              ALStorage AL_DLL_FAR & output,
                              long length )
{
    ALOpenFiles files( input, output );
    if ( input.mStatus < AL_SUCCESS )
        return mStatus = input.mStatus;
    if ( output.mStatus < AL_SUCCESS )
        return mStatus = output.mStatus;
    output.InitCrc32();
    z_stream stream;
    int result;

    memset( &stream, 0, sizeof( z_stream ) );
    stream.avail_in = 0;
#if !defined( HACK_INPUT )
    stream.next_in = (Byte ZL_FAR *) inbuf;
#endif
#if defined( HACK_OUTPUT )
    output.FlushBuffer();
    stream.avail_out = output.muBufferSize;
    stream.next_out = (Byte ZL_FAR *) output.mpcBuffer;
#else
    stream.avail_out = OUTBUF_LEN;
    stream.next_out = (Byte ZL_FAR *) outbuf;
#endif
    stream.zalloc = 0;
    stream.zfree = 0;
    inflateInit2( &stream, -MAX_WBITS );
    for ( ; ; ) {
        if ( stream.avail_in == 0 ) {
#if !defined( HACK_INPUT )
            stream.avail_in = input.ReadBuffer( (unsigned char *) inbuf, INBUF_LEN );
            stream.next_in = (unsigned char *) inbuf;
#else
            if ( input.muReadIndex >= input.muBufferValidData ) {
                if ( input.LoadBuffer( input.mlFilePointer ) < 0 )
                    stream.avail_in = 0;
                else
                    stream.avail_in = input.muBufferValidData;
            } else
                stream.avail_in = input.muBufferValidData - input.muReadIndex;
//
// The next few lines are designed to prevent overshoot of the input
// data stream.
//
            if ( length != -1 ) {
                if ( (long) stream.avail_in > length )
                    stream.avail_in = (unsigned short) length;
                length -= stream.avail_in;
            }
            stream.next_in = input.mpcBuffer + input.muReadIndex;
            input.muReadIndex += stream.avail_in;
#endif
        }
        result = inflate( &stream, Z_NO_FLUSH );
#if defined( HACK_OUTPUT )
        if ( stream.avail_out != output.muBufferSize ) {
            output.muWriteIndex = output.muBufferSize - stream.avail_out;
            output.FlushBuffer();
            stream.next_out = output.mpcBuffer;
            stream.avail_out = output.muBufferSize;
            output.muWriteIndex = 0;
#else
        if ( stream.avail_out != OUTBUF_LEN ) {
            output.WriteBuffer( (unsigned char *) outbuf, OUTBUF_LEN - stream.avail_out );
            stream.avail_out = OUTBUF_LEN;
            stream.next_out = (Byte ZL_FAR *) outbuf;
#endif
        } else
            break;
        if ( result != Z_OK )
            break;
    }
    inflateEnd( &stream );

    if ( input.mStatus < AL_SUCCESS )
        return mStatus = input.mStatus;
    if ( output.mStatus < AL_SUCCESS )
        return mStatus = output.mStatus;
    return mStatus;
}

//
// NAME
//
//  ALPkDecompressor::Clone()
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
//  #include "pkengn.h"
//
//  ALPkDecompressor::Clone( int engine_type );
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
//  ALDecompressor, and can be called to create a duplicate of an
//  existing decompression engine.
//
//  Why is this useful?  It is useful because it allows us to use
//  what is the equivalent of a virtual constructor.  We can pass a
//  pointer to a PKWare engine to the archiving code, and it can then in
//  turn stuff copies of that engine into an ALEntryList without
//  having any idea what sort of decompression engine it is actually creating.
//
// RETURNS
//
//  A copy of a newly created decompression engine.  When this routine is
//  called, it will usually be called via a virtual function from a pointer
//  to a base class object, which means the resulting pointer will be
//  treated as an ALDecompressor * by the code, not an ALPkDecompressor.
//
//  If this routine doesn't know how to create an engine of the correct type,
//  it returns a 0.  When performing decompression from an archive, the
//  Clone() functions might be called with an unknown compression type.  For
//  example, if you try to unzip a PkWAre archive, but are using a
//  Greenleaf-only toolkit.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//
ALDecompressor AL_DLL_FAR * AL_PROTO
ALPkDecompressor::Clone( int engine_type ) const  /* Tag public function */
{
    _option pk_option;
    ALPkDecompressor *decompressor;
    switch ( engine_type ) {
        case AL_COMPRESSION_COPY :
        case AL_COMPRESSION_DEFLATE_COPY :
            return new ALCopyDecompressor();
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
    decompressor = new ALPkDecompressor;
    if ( decompressor )
        decompressor->option = pk_option;
    return decompressor;
}


