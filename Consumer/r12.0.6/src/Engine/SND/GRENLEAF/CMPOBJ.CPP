//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CMPOBJ.CPv   1.0   12 Mar 1998 12:15:26   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CMPOBJ.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:26   DCHI
// Initial revision.
// 
//************************************************************************

//
// CMPOBJ.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALCompressedObject::operator new()
//  ALCompressedObject::ALCompressedObject()
//  newALCompressed()
//  ALCompressedObject::~ALCompressedObject()
//  deleteALCompressed()
//  ALCompressedObject::Insert()
//  ALCompressedInsert()
//  ALCompressedObject::Extract()
//  ALCompressedExtract()
//  ALCompressedObject::WriteHeaderData()
//  ALCompressedObject::ReadHeaderData()
//
// DESCRIPTION
//
//  This file contains all the support code for the ALCompressedObject
//  class.  This class is sort of a poor-man's archive, with just one
//  file, no flexibility, and super-low overhead.
//
// REVISION HISTORY
//
//  May 23, 1994  1.0A   : First release
//
//  August 10, 1994 1.0B : Modified the ALCompressedObject class just a bit
//                         to make it somewhat more useful.  First, the
//                         ReadHeaderData() and WriteHeaderData()
//                         functions now get called with a pointer to
//                         their target storage object.  Second, modified the
//                         extract function so that it calls ReadHeaderData()
//                         real early in the process.  One of the reasons
//                         for these changes was to allow a compressed
//                         object that can save the last letter of the
//                         original file when compressed, and restore
//                         it when extracting.  So you can compress
//                         BOB.DAT to BOB.DA_.  When you restore BOB.DA_,
//                         a derived class can automatically set the
//                         output file name to be BOB.DAT.
//
//   February 14, 1996  2.0A  : New release
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "_openf.h"

//
// NAME
//
//  ALCompressedObject::operator new()
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
//  #include "cmpobj.h"
//
//  void * ALCompressedObject::operator new( size_t size )
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
//  size  :  The number of bytes needed to create a new ALCompressedObject.
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
ALCompressedObject::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALCompressedObject::ALCompressedObject()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create a new CompressedObject.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  ALCompressedObject::ALCompressedObject( ALStorage &storage_object,
//                                          ALCompressor *compressor,
//                                          ALDecompressor *decompressor );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  hALCompressed newALCompressed( hALStorage storage,
//                                 hALCompressor compressor,
//                                 hALDecompressor decompressor );
//
// VB SYNOPSIS
//
//  Declare Function newALCompressed Lib "AL20LW"
//    (ByVal storage&, ByVal compressor&, ByVal decompressor&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALCompressed( storage : hALStorage;
//                            compressor : hALCompressor,
//                            decompressor : hALDecompressor ) :  hALCompressed;
//
// ARGUMENTS
//
//  storage_object     : A reference to the storage object that is going
//                       to get the compressed data.
//
//  compressor         : A reference to the compression engine that will
//                       be used to insert an object a storage object into
//                       the compressed object.  If you are only going to
//                       be extracting, feel free to use a 0 for this argument.
//
//  decompressor       : A reference to the decompression engine that will
//                       be used to extract an object a storage object from
//                       the compressed object.  If you are only going to
//                       be inserting, feel free to use a 0 for this argument.
//
//
// DESCRIPTION
//
//  An ALCompressedObject is a storage object that gets a single compressed
//  object packed into it.  You get to call Insert() or Extract(), to
//  put the object in or take it out.  Compressed objects don't get all
//  the fancy options that Archives do.  For example, you have to know in
//  advance what sort of compression engine and storage object you are
//  going to use to put things in and take things out.  You don't get to
//  store comments or time date stamps, or anything like that.
//
//  The one piece of flexibility you do get the ALCompressedObject is
//  the ability to derive a new class from this base, then use the
//  new class to write some custom data out to the object.
//
//  This constructor stores references to the object being used to hold the
//  compressed data, and the engines being used to pack and unpack it.
//
// RETURNS
//
//  For C++, nothing, this is a constructor.  For the other languages,
//  a handle pointing to the compressed object.
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
ALCompressedObject::ALCompressedObject( /* Tag public function */
           ALStorage AL_DLL_FAR & storage_object,
           ALCompressor AL_DLL_FAR * compressor,
           ALDecompressor AL_DLL_FAR * decompressor )
{
    mpCompressor = compressor;
    mpDecompressor = decompressor;
    mpStorageObject = &storage_object;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALCompressed AL_FUNCTION
newALCompressed( hALStorage storage,  /* Tag public function */
                 hALCompressor compressor,
                 hALDecompressor decompressor )
{
    AL_ASSERT_OBJECT( storage, ALStorage, "newALCompressed" );
    if ( compressor )
        AL_ASSERT_OBJECT( compressor, ALCompressor, "newALCompressed" );
    if ( decompressor )
        AL_ASSERT_OBJECT( decompressor, ALDecompressor, "newALCompressed" );
    return (hALCompressed)
      new ALCompressedObject( *(ALStorage *) storage,
                              (ALCompressor *) compressor,
                              (ALDecompressor *) decompressor );
}

#endif

//
// NAME
//
//  ALCompressedObject::~ALCompressedObject()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Reset the error status for an entry list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  ALCompressedObject::~ALCompressedObject()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  void deleteALCompressed( hALCompressed this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALCompressed Lib "AL120LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALCompressed( this_object : hALCompressed );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALCompressedObject that
//                  is going to be destroyed.  Note that the C++ version
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This destructor has nothing important to do.  The debug version
//  checks the object type for validity, but that's it.
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
ALCompressedObject::~ALCompressedObject()  /* Tag public function */
{
    AL_ASSERT_OBJECT( this, ALCompressedObject, "~ALCompressedObject" );
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALCompressed( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "deleteALCompressed" );
    delete (ALCompressedObject *) this_object;
}

#endif

//
// NAME
//
//  ALCompressedObject::Insert()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Insert a storage object into an ALCompressedObject.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedObject::Insert( ALStorage &input_object )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedInsert( hALCompressed this_object,
//                         hALStorage input_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressedInsert Lib "AL20LW"
//    (ByVal this_object&, ByVal input_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompressedInsert( this_object : hALCompressed;
//                               input_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALCompressedObject that
//                  is going to have an object inserted into it.  Note that
//                  the C++ version of this call doesn't have an explicit
//                  this_object argument, since it has access to 'this'
//                  implicitly.
//
// input_object :   A pointer or reference to the storage object that is
//                  going to be inserted into the compressed object.
//
// DESCRIPTION
//
//  The compressed object has this format:
//
//    long uncompressed_size
//    long compressed_size
//    DWORD crc_32
//    Any data from derived classes
//    unsigned char compressed_object[]
//
//  Writing all this out is pretty straightforward, although you might
//  note that it is going to require at least one seek() back to the
//  start of the compressed object after the compression is done.
//
// RETURNS
//
//  AL_SUCCESS if everything worked properly, or < AL_SUCCESS if an
//  error was encountered.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//

int AL_PROTO
ALCompressedObject::Insert( ALStorage AL_DLL_FAR &input_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this, ALCompressedObject, "Insert" );
    AL_ASSERT_OBJECT( &input_object, ALStorage, "Insert" );
    if ( mStatus < AL_SUCCESS )
        return mStatus;
//
// Here is where we open the input and the output.
//
    ALOpenFiles files( input_object, *mpStorageObject );
//
// We first write out the uncompressed size, which we already know.  We
// then save the current position, and write placeholder longs out for
// what will become the compressed size and the CRC-32.
//
    mpStorageObject->WriteGlLong( input_object.GetSize() );
    long saved_pos = mpStorageObject->Tell();
    mpStorageObject->WriteGlLong( 0xfedcba98L ); //Temporary
    mpStorageObject->WriteGlLong( 0x01234567L );  //Temporary
//
// If a derived class has any header data to write out, this is where it
// will be performed.  The base class writes 0 bytes here.
//
    WriteHeaderData( &input_object );
    long start = mpStorageObject->Tell();
//
// Next, perform the compression.  Once that is done we can calculate
// the compressed size.  The CRC-32 will have been calculated on the fly
// as the compression was performed.
//
    mpCompressor->Compress( input_object, *mpStorageObject );
    long compressed_size = mpStorageObject->Tell() - start;
    if ( mpCompressor->mStatus < 0 )
        return mStatus = mpCompressor->mStatus;
//
// Go back to the spot we remembered, and write out the compressed
// size and the CRC. At that point, the compressed object is complete.
//
    mpStorageObject->Seek( saved_pos );
    mpStorageObject->WriteGlLong( compressed_size );
    mpStorageObject->WriteGlLong( ~input_object.GetCrc32() );
    if ( mpStorageObject->mStatus < 0 )
        return mStatus = mpStorageObject->mStatus;
    return AL_SUCCESS;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALCompressedInsert( hALCompressed this_object,  /* Tag public function */
                    hALStorage input_object )
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedInsert" );
    AL_ASSERT_OBJECT( input_object, ALStorage, "ALCompressedInsert" );
    return ( (ALCompressedObject *) this_object )->Insert( * (ALStorage *) input_object );
}

#endif

//
// NAME
//
//  ALCompressedObject::Extract()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Extract a storage object from an ALCompressedObject.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedObject::Extract( ALStorage &input_object )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedExtract( hALCompressed this_object,
//                           hALStorage output_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressedExtract Lib "AL20LW"
//    (ByVal this_object&, ByVal output_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompressedExtract( this_object : hALCompressed;
//                                output_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A reference or pointer to the ALCompressedObject that
//                  contains the compressed object.  Note that
//                  the C++ version of this call doesn't have an explicit
//                  this_object argument, since it has access to 'this'
//                  implicitly.
//
//  output_object : A reference or pointer to the ALStorage object that
//                  is going receive the extracted storage object.
//
//
// DESCRIPTION
//
//  Extracting the data to a new storage object is easy.  We read in
//  all the header data so that we can do a little error checking along the
//  way, but that's all.
//
// RETURNS
//
//  AL_SUCCESS if everything worked properly, or < AL_SUCCESS if an
//  error was encountered.
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
ALCompressedObject::Extract( ALStorage AL_DLL_FAR &output_object )  /* Tag public function */
{
    long compressed_length;
    long crc32;

    AL_ASSERT_OBJECT( this, ALCompressedObject, "Extract" );
    AL_ASSERT_OBJECT( &output_object, ALStorage, "Extract" );
    if ( mStatus < AL_SUCCESS )
        return mStatus;
//
// Open the input file.
//
    ALOpenInputFile input_file( *mpStorageObject );
//
// Now read in all the data stored at the start of the object,
// including any header data created by derived classes.  If we are
// using the base class, there won't be any additional data bytes there.
//
    mpStorageObject->ReadGlLong( output_object.mlSize );
    mpStorageObject->ReadGlLong( compressed_length );
    mpStorageObject->ReadGlLong( crc32 );
    ReadHeaderData( &output_object );
    if ( mpStorageObject->mStatus < 0 )
        return mStatus = mpStorageObject->mStatus;
//
// Open the output file.
//
    ALOpenOutputFile output_file( output_object );
//
// Extract the data and store it in the storage object specified
// as an argument.
//
    if ( mpDecompressor->Decompress( *mpStorageObject,
                                     output_object,
                                     compressed_length ) < 0 )
        return mStatus = mpDecompressor->mStatus;
//
// A little error checking leads to an error return if things didn't
// go well, or AL_SUCCESS if things did.
//
    if ( mpStorageObject->mStatus < 0 )
        return mStatus = mpStorageObject->mStatus;
    if ( crc32 != ~output_object.GetCrc32() )
        return mStatus.SetError( AL_CRC_ERROR,
                                 "CRC32 differs between %s and %s",
                                 mpStorageObject->mName.GetName(),
                                 output_object.mName.GetName() );
    return AL_SUCCESS;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALCompressedExtract( hALCompressed this_object,  /* Tag public function */
                     hALStorage output_object )
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedExtract" );
    AL_ASSERT_OBJECT( output_object, ALStorage, "ALCompressedExtract" );
    return ( (ALCompressedObject *) this_object )->Extract( * (ALStorage *) output_object );
}

#endif


//
// NAME
//
//  ALCompressedObject::WriteHeaderData();
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Write customized data to an ALCompressedObject.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedObject::WriteHeaderData( ALStorage * storage /* = 0 */  )
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
//  storage  :  A pointer to the storage object that is going to be inserted
//              into the ALCompressed object.  Be sure not to confuse this
//              with mpStorageObject, which is the member that points to the
//              storage object that's going to hold the compressed data.
//
// DESCRIPTION
//
//  Derived classes can override this function and use it to add
//  additional data bytes to the header of a compressed object.  Note
//  that this data does not have to be written out in any particular
//  format, we have no portability concerns here.  It is up to the
//  derived class to insure that the data is written in an internally
//  consistent format so that ReadHeaderData() can always position the
//  file pointer to the correct start of data.
//
//  This function shows the default version of WriteHeaderData, and it does
//  absolutely nothing.
//
// RETURNS
//
//  An integer containing an ArchiveLib status code, hopefully this
//  will be AL_SUCCESS.  Write now, the Insert() member function doesn't
//  pay attention to the return code, so it really doesn't matter what
//  you put here.
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
ALCompressedObject::WriteHeaderData( ALStorage AL_DLL_FAR * /* = 0 */  ) /* Tag protected function */
{
    return AL_SUCCESS;
}

//
// NAME
//
//  ALCompressedObject::ReadHeaderData();
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read customized data from an ALCompressedObject.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedObject::ReadHeaderData( ALStorage * storage /* = 0 */  )
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
//  storage  :  A pointer to the storage object that is going to receive the
//              object extracted from the ALCompressedObject.  Be sure not to
//              confuse this with mpStorageObject, which is the member that
//              points to the storage object that's holding the compressed
//              data.
//
// DESCRIPTION
//
//  Derived classes can override this function and use it to read
//  additional data bytes from the header of a compressed object.  Note
//  that this data does not have to be written out in any particular
//  format, we have no portability concerns here.  It is up to the
//  derived class to insure that the data is written in an internally
//  consistent format so that the Extract() function can always position the
//  file pointer to the correct start of data.
//
//  This function shows the default version of ReadHeaderData, and it does
//  absolutely nothing.
//
// RETURNS
//
//  An integer containing an ArchiveLib status code, hopefully this
//  will be AL_SUCCESS.  Write now, the Extract() member function doesn't
//  pay attention to the return code, so it really doesn't matter what
//  you put here.
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
ALCompressedObject::ReadHeaderData( ALStorage AL_DLL_FAR * /* = 0 */ )  /* Tag protected function */
{
    return AL_SUCCESS;
}

#endif // #if !defined( AL_DEMO )

