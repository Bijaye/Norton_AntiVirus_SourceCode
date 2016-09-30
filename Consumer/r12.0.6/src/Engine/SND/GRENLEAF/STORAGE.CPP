//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STORAGE.CPv   1.0   12 Mar 1998 12:17:48   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STORAGE.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:48   DCHI
// Initial revision.
// 
//************************************************************************

//
// STORAGE.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStorage::operator new()
//  ALStorage::ALStorage()
//  ALStorage::~ALStorage()
//  deleteALStorage()
//  ALStorage::Open()
//  ALStorageOpen()
//  ALStorage::Create()
//  ALStorageCreate()
//  ALStorage::Close()
//  ALStorageClose()
//  ALStorage::ReadBuffer()
//  ALStorageReadBuffer()
//  ALStorageReadBufferVB32()
//  ALStorage::WriteBuffer()
//  ALStorageWriteBuffer()
//  ALStorageWriteBufferVB32()
//  ALStorage::WriteString()
//  ALStorageWriteString()
//  ALStorage::ReadString()
//  ALStorage::Tell()
//  ALStorageTell()
//  ALStorage::WriteStorageObjectData()
//  ALStorage::ReadStorageObjectData()
//  ALStorage::ReadCopyright()
//  ALStorageReadCopyright()
//  ALStorageReadCopyrightVB()
//
// DESCRIPTION
//
//  This file contains all of the source code for the member functions
//  of ALStorage.  AlStorage has pure virtual functions, so you can't
//  ever instantiate one of these guys.
//
// REVISION HISTORY
//
//  May 26, 1994  1.0A  : First release
//
//  July 7, 1994  1.0B  : Minor bug fixes
//
//   February 14, 1996  2.0A : New release.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "_vbutil.h"
#include <string.h>

//
// NAME
//
//  ALStorage::operator new()
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
//
//  void * ALStorage::operator new( size_t size )
//
// C SYNOPSIS
//
//  None, internal C++ member function.
//
// VB SYNOPSIS
//
//  None, internal C++ member function.
//
// DELPHI SYNOPSIS
//
//  None, internal C++ member function.
//
// ARGUMENTS
//
//  size  :  The number of bytes that the compiler has decided will be
//           necessary to construct a new ALStorage object.
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
//  Note that this function will probably never be called.  You can't
//  instantiate an ALStorage object, because it has some pure virtual
//  functions.  So only derived classes get instantiated, and they
//  really ought to have their own new operators.

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
//   May 24, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release.
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALStorage::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALStorage::ALStorage()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALStorage constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALStorage::ALStorage( const char *file_name,
//                        size_t size,
//                        const enum ALStorageType object_type,
//                        ALCase name_case = AL_MIXED );
//
// C SYNOPSIS
//
//  None, this function is not available to C programmers.
//
// VB SYNOPSIS
//
//  None, this function is not available to VB programmers.
//
// DELPHI SYNOPSIS
//
//  None, this function is not available to Delphi programmers.
//
// ARGUMENTS
//
//  file_name     :  The name to assign to the mName data member of the
//                   newly created storage object.
//
//  size          :  The size of the I/O buffer that is going to be used
//                   for the storage object.  ALFile uses 4096 as a default.
//
//  object_type   :  The type of object, as defined in ALDEFS.H.  Good
//                   values include AL_FILE_OBJECT and AL_MEMORY_OBJECT.
//
//  name_case     :  The case sensitivity of the object name.  For objects
//                   such as ALFile, AL_MIXED is a no-no.  Those objects
//                   need to be forced to convert names to all upper
//                   or all lower, because the operating system considers
//                   file names to be case insensitive.
//
// DESCRIPTION
//
//  The constructor for ALStorage gets called from the constructor of
//  derived classes.  It has to initialize all sorts of data members.
//  First, in the initializer list, it sets up the mName data member,
//  as well as muBufferSize and miStorageObjectType.  The latter two
//  data members are set to be const so I can make them public, which
//  means we have to initialize them in the initializer list.
//
//  In the body of the constructor, we initialize a bunch of data members,
//  none of which mean anything at this point.
//
// RETURNS
//
//  Nothing, it is a constructor.
//
// EXAMPLE
//
//  N/A, not part of the public API.
//
// SEE ALSO
//
//  ALFile::ALFile(), ALMemory::ALMemory()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALStorage::ALStorage( const char AL_DLL_FAR *file_name,  /* Tag public function */
                      size_t size,
                      const enum ALStorageType object_type,
                      ALCase name_case /* = AL_MIXED */ )
    : mName( file_name, name_case ),
      miStorageObjectType( object_type ),
      muBufferSize( size )
{
    mpcBuffer = 0;
    muBufferValidData = 0;
    muWriteIndex = 0;
    muReadIndex = 0;
    mlFilePointer = 0;
    miUpdateCrcFlag = 0;
    mlCrc32 = 0xffffffffL;
    mlSize = -1L;
    mpMonitor = 0;
    miCreated = 0;
    if ( mName.GetName() == 0 )
        mStatus.SetError( AL_CANT_OPEN_BUFFER,
                          "Allocation of buffer failed in "
                          "ALStorage constructor" );
}

//
// NAME
//
//  ALStorage::~ALStorage()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The ALStorage destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALStorage::~ALStorage();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void deleteALStorage( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALStorage Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALStorage( this_object : hALStorage );
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 is going to be destroyed.  Note that the C++ version
//                 of this function doesn't have this argument, since it
//                 has implicit access to 'this'.
//
// DESCRIPTION
//
//  In debug mode, we first check to make sure we are destroying the
//  right type of object.
//
//  The only thing left to do is free up the I/O buffer if it is still
//  allocated.  This piece of work probably isn't necessary.  Since this
//  is a virtual destructor, we will be called after the destructors
//  for the derived class.  Any derived class that is doing its job
//  will make sure that it calls Close() before destroying itself.  If
//  it doesn't, it will probably be leaving unfinished business behind
//  that we aren't going to be able to deal with here.  Even so, we will
//  be diligent in our attention to detail.
//
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
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALStorage::~ALStorage()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALStorage: attempting to delete invalid object" );
    if ( mpcBuffer )
        Close();
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALStorage( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "deleteALStorage" );
    delete (ALStorage *) this_object;
}

#endif

//
// NAME
//
//  ALStorage::Open()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Open a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::Open()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageOpen( hALStorage this_object )
//
// VB SYNOPSIS
//
//  Declare Function ALStorageOpen Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageOpen( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be opened.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  Any derived class needs to have its own Open() function.  However,
//  the derived class can also call this Open() function in the base
//  class to do some odds and ends for it.  The most important thing it
//  does is allocate the I/O buffer, which is what makes ALStorage a
//  relatively fast way to read and write data.  Although the buffer
//  is in place, there is no data in it, so this guy also sets up the
//  indices and pointers to reflect that.
//
//  Upon exit, all you need to to is start reading or writing, and the
//  whole thing should be ready to go.
//
// RETURNS
//
//  AL_SUCCESS, or AL_CANT_OPEN_BUFFER on memory allocation failure.
//  If the object was already in an error state, it is very possible to
//  get some other error code < 0.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Create(), ALStorage::Close()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::Open()  /* Tag public function */
{
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( muBufferSize != 0 )
        mpcBuffer = new unsigned char[ muBufferSize ];
    muBufferValidData = 0;
    muWriteIndex = 0;
    muReadIndex = 0;
    mlFilePointer = 0;
    miUpdateCrcFlag = 0;
    mlCrc32 = 0xffffffffL;
    if ( mpcBuffer == 0 )
        return mStatus.SetError( AL_CANT_OPEN_BUFFER,
                                 "Allocation of buffer failed in Open()" );
    return AL_SUCCESS;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageOpen( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageOpen" );
    return ( (ALStorage *) this_object )->Open();
}

#endif

//
// NAME
//
//  ALStorage::Create();
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create a new storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::Create( long desired_size = - 1 );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageCreate( hALStorage this_object, long desired_size );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageCreate Lib "AL20LW"
//    (ByVal this_object&, ByVal desired_size&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageCreate( this_object : hALStorage;
//                            desired_size : Long ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be created.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  desired_size :  If you know in advance how much space a storage object
//                  is going to occupy, you can pass that number as the
//                  desired_size argument.  Some storage types, such as
//                  ALMemory, can make good use of this information.  Others
//                  such as ALFile, could care less.  In any case, the base
//                  class, ALStorage, ignores the info.  If you don't know
//                  how many bytes the storage object is going to need,
//                  pass a -1L as the argument.
//
// DESCRIPTION
//
//  This function is nearly identical to ALStorage::Open().
//
//  Any derived class needs to have its own Create() function.  However,
//  the derived class can also call this Create() function in the base
//  class to do some odds and ends for it.  The most important thing it
//  does is allocate the I/O buffer, which is what makes ALStorage a
//  relatively fast way to read and write data.  Although the buffer
//  is in place, there is no data in it, so this guy also sets up the
//  indices and pointers to reflect that.
//
// RETURNS
//
//  AL_SUCCESS, or AL_CANT_OPEN_BUFFER on memory allocation failure.
//  If the object was already in an error state, it is very possible to
//  get some other error code < 0.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Delete(), ALStorage::Open()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::Create( long /* desired_size = - 1 */ )  /* Tag public function */
{
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    mpcBuffer = new unsigned char[ muBufferSize ];
    muBufferValidData = 0;
    muWriteIndex = 0;
    muReadIndex = 0;
    mlFilePointer = 0;
    miUpdateCrcFlag = 0;
    mlSize = 0; //If the file has been opened previously, mlSize might be non-zero
    mlCrc32 = 0xffffffffL;
    miCreated = 1;
    if ( mpcBuffer == 0 )
        return mStatus.SetError( AL_CANT_OPEN_BUFFER,
                                 "Allocation of buffer failed in Open()" );
    return AL_SUCCESS;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageCreate( hALStorage this_object, /* Tag public function */
                 long desired_size )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageCreate" );
    if ( desired_size == AL_DEFAULT )
        desired_size = -1;
    return ( (ALStorage *) this_object )->Create( desired_size );
}

#endif

//
// NAME
//
//  ALStorage::Close()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Close an ALStorage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::Close();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageClose( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageClose Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageClose( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be closed.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  Just like with Open(), most derived classes will have their own
//  versions of Close().  They can call this version to delete the I/O
//  buffer if they feel like it is too hard to do themselves.
//
// RETURNS
//
//  The current integer status of the object.  Hopefully this will be
//  AL_SUCCESS, but it could well be a value < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALFile::Create(), ALFile::Open()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::Close()  /* Tag public function */
{
    if ( mpcBuffer ) {
        delete[] mpcBuffer;
        mpcBuffer = 0;
    }
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageClose( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageClose" );
    return ( (ALStorage *) this_object )->Close();
}

#endif

//
// NAME
//
//  ALStorage::ReadBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read in a buffer full of data.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  size_t ALStorage::ReadBuffer( unsigned char *buf, size_t length );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  size_t ALStorageReadBuffer( hALStorage this_object,
//                              unsigned char *buffer,
//                              size_t length );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageReadBuffer Lib "AL20LW"
//    (ByVal this_object&, ByVal buffer$, ByVal length%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageReadBuffer( this_object : hALStorage;
//                                buffer : PChar;
//                                length : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be read.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  buf    :  The buffer that is going to receive input characters.
//
//  length :  The number of bytes you want to read.
//
// DESCRIPTION
//
//  We could write a simple version of this function by just calling
//  ReadChar() over and over, but it would be nice to do things
//  a little more efficiently.  Since we have this nice big buffer
//  full of data ready to read, it makes sense to copy big chunks of
//  it in one fell swoop.  That is what this guy does.  It sits in a loop
//  doing a memcpy() followed by LoadBuffer() until all of the data
//  that has been asked for got moved.  As data is read in, we have to
//  update the data member muReadIndex.  Other data members will get
//  updated by LoadBuffer().
//
// RETURNS
//
//  The number of bytes read in, always.  If this function generates an
//  error, it will be found in the mStatus member.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::ReadChar(), ALStorage::WriteBuffer()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

size_t AL_PROTO
ALStorage::ReadBuffer( unsigned char AL_DLL_FAR *buf,  /* Tag public function */
                       size_t length )
{
    size_t bytes_left_to_read = length;
    size_t buffer_bytes_available;

    while ( bytes_left_to_read ) {
        buffer_bytes_available = muBufferValidData - muReadIndex;
        if ( buffer_bytes_available == 0 ) {
            if ( LoadBuffer( mlFilePointer ) < 0 )
                return length - bytes_left_to_read;
            buffer_bytes_available = muBufferValidData;
        }
        if ( bytes_left_to_read <= buffer_bytes_available ) {
            memcpy( buf, mpcBuffer + muReadIndex, bytes_left_to_read );
            muReadIndex += bytes_left_to_read;
            return length;
        } else {
            memcpy( buf, mpcBuffer + muReadIndex, buffer_bytes_available );
            buf += buffer_bytes_available;
            bytes_left_to_read -= buffer_bytes_available;
            muReadIndex += buffer_bytes_available;
            if ( LoadBuffer( mlFilePointer ) < 0 )
                return length - bytes_left_to_read;
        }
    }
    return length;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE size_t AL_FUNCTION
ALStorageReadBuffer( hALStorage this_object,  /* Tag public function */
                     unsigned char AL_DLL_FAR *buffer,
                     size_t length )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageReadBuffer" );
    AL_ASSERT( buffer != 0, "Passed a null buffer to ALStorageReadBuffer" );
    return ( (ALStorage * ) this_object )->ReadBuffer( buffer, length );
}

#endif

#if defined( AL_VB32 )
extern "C" AL_LINKAGE size_t AL_FUNCTION
ALStorageReadBufferVB32( hALStorage this_object,  /* Tag public function */
                         LPSAFEARRAY *ppsa,
                         size_t length )
{
    unsigned char *p;
    size_t result;

    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageReadBufferVB32" );
    AL_ASSERT( SafeArrayGetDim( *ppsa ) > 0, "Array with < 1 dimensions in ALStorageWriteBufferVB32" );
//
// This doesn't work, don't know why
//
//    int j = SafeArrayGetElemsize( *ppsa );
//    wsprintf( buffer, "Array dim = %d  j = %d  length = %d", i, j, length );
//    char buffer[ 81 ];
//    MessageBox( 0, buffer, buffer, MB_OK );
//    AL_ASSERT( j >= (int) length, "Buffer too small in ALStorageReadBufferVB32" );
//    int j =
    SafeArrayAccessData( *ppsa, (void **) &p );
    result = ( (ALStorage * ) this_object )->ReadBuffer( p, length );
    SafeArrayUnaccessData( *ppsa );
    return result;
}

#endif


//
// NAME
//
//  ALStorage::WriteBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Write a buffer to a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  size_t ALStorage::WriteBuffer( const unsigned char *buf,
//                                 size_t length );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  size_t ALStorageWriteBuffer( hALStorage this_object,
//                               unsigned char *buffer,
//                               size_t length );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWriteBuffer Lib "AL20LW"
//    (ByVal this_object&, ByVal buffer$, ByVal length%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWriteBuffer( this_object : hALStorage;
//                                 buffer : PChar;
//                                 length : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be written to.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  buf    :  The buffer that is contains the output data.
//
//  length :  The number of bytes you want to write.
//
// DESCRIPTION
//
//  We could write a simple version of this function by just calling
//  WriteChar() over and over, but it would be nice to do things
//  a little more efficiently.  Since we have this nice big buffer
//  just waiting for data, it makes sense to copy big chunks to
//  it in one fell swoop.  That is what this guy does.  It sits in a loop
//  doing a memcpy() followed by FlushBuffer() until all of the data
//  that was ready to go has been sent. As data is written, we have to
//  update the data member muWriteIndex.  Other data members will get
//  updated by FlushBuffer().
//
// RETURNS
//
//  The number of bytes written, always.  If this function generates an
//  error, it will be found in the mStatus member.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::WriteChar(), ALStorage::ReadBuffer()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

size_t AL_PROTO
ALStorage::WriteBuffer( const unsigned char AL_DLL_FAR *buf,  /* Tag public function */
                        size_t length )
{
    size_t buffer_bytes_free;
    size_t write_bytes_left = length;

    if ( mStatus < 0 )
        return 0;
    while ( write_bytes_left > 0 ) {
        buffer_bytes_free = muBufferSize - muWriteIndex;
        if ( buffer_bytes_free == 0 ) {
            if ( FlushBuffer() < 0 )
                return length - write_bytes_left;
            buffer_bytes_free = muBufferSize;
        }
        if ( write_bytes_left <= buffer_bytes_free ) {
            memcpy( mpcBuffer + muWriteIndex, buf, write_bytes_left );
            muWriteIndex += write_bytes_left;
            return length;
        } else {
            memcpy( mpcBuffer + muWriteIndex, buf, buffer_bytes_free );
            muWriteIndex += buffer_bytes_free;
            buf += buffer_bytes_free;
            write_bytes_left -= buffer_bytes_free;
            if ( FlushBuffer() < 0 )
                return length - write_bytes_left;
        }
    }
    return length;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE size_t AL_FUNCTION
ALStorageWriteBuffer( hALStorage this_object,  /* Tag public function */
                      unsigned char AL_DLL_FAR *buffer,
                      size_t length )

{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWriteBuffer" );
    AL_ASSERT( buffer != 0, "Passed a null buffer to ALStorageWriteBuffer" );
    return ( (ALStorage * ) this_object )->WriteBuffer( buffer, length );
}

#endif

#if defined( AL_VB32 )

extern "C" AL_LINKAGE size_t AL_FUNCTION
ALStorageWriteBufferVB32( hALStorage this_object,  /* Tag public function */
                          LPSAFEARRAY *ppsa,
                          size_t length )

{
    unsigned char *p;
    size_t result;

    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWriteBufferVB32" );
    AL_ASSERT( SafeArrayGetDim( *ppsa ) > 0, "Array with < 1 dimensions in ALStorageWriteBufferVB32" );
//    i = SafeArrayGetElemsize( *ppsa );
//    AL_ASSERT( i >= (int)length, "Buffer too small in ALStorageWriteBufferVB32" );
//    int j =
    SafeArrayAccessData( *ppsa, (void **) &p );
    result = ( (ALStorage * ) this_object )->WriteBuffer( p, length );
    SafeArrayUnaccessData( *ppsa );
    return result;
}

#endif

//
// NAME
//
//  ALStorage::WriteString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Write a null terminated string to a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::WriteString( const char AL_DLL_FAR *string_data );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageWriteString( hALStorage this_object,
//                            char *string );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWriteString Lib "AL20LW"
//    (ByVal this_object&, ByVal string_data$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWriteString( this_object : hALStorage;
//                                 string_data : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to have a string written to it.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  string_data  :  A C-style null terminated string to be written out in
//                  our portable format.
//
// DESCRIPTION
//
//  We write random length data to archive directories using this special
//  format, which is a 16 bit int describing the length of the data,
//  followed by the data itself.  All of the storage objects and compression
//  engines write their own private data out using this format.  This
//  means that even if another class doesn't understand the content of data
//  stored in this format, at least it knows how to read it in so as to
//  move past it.
//
//  This function won't write just any random data, it is specifically
//  oriented towards C strings.  This means it is mostly used to write
//  file names and comments.  Their are a few places where classes
//  write private data that isn't kept in C strings, they just manually
//  write the length with WriteGlShort(), followed by the data.
//
// RETURNS
//
//  AL_SUCCESS if things work, or an error code < AL_SUCCESS if an error
//  occurs writing the data out.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::ReadString()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::WriteString( const char AL_DLL_FAR *string_data )  /* Tag public function */
{
    short unsigned int len;
    if ( string_data != 0 )
        len = (short unsigned int) strlen( string_data );
    else
        len = 0;
    WriteGlShort( len );
    if ( len )
        WriteBuffer( (unsigned char *) string_data, len );
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageWriteString( hALStorage this_object,  /* Tag public function */
                      char AL_DLL_FAR *string )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWriteString" );
    AL_ASSERT( string != 0, "Passing null string in ALStorageWriteString()" );
    return ( (ALStorage *) this_object )->WriteString( string );
}


#endif

//
// NAME
//
//  ALStorage::ReadString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read a string from a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::ReadString( ALName &name,
//                             short int length = -1 );
//
// C SYNOPSIS
//
//  None, we have problems with this in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, we have problems with this in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, we have problems with this in C/VB/Delphi.
//
// ARGUMENTS
//
//  name  :  A reference to the storage object that is going to be loaded
//           up with a string.  Note that using an ALName object is a good
//           thing here, because we are going to have to dynamically allocate
//           data based on how long the input string is.  If we returned a
//           char *, we would have problems, because the pointer would be
//           allocated inside the DLL, but the user would have to free it
//           outside the DLL.  With an ALName, the data will be freed inside
//           the DLL when the ALName object is destroyed.
//
//           We can't pass around this ALName object to C/VB/Delphi, so they
//           don't get access to this function.  Sorry.
//
//  length : Normally, you read the length in from the storage object,
//           and then read in the data.  But sometimes, you know the
//           length in advance, and don't want to read it from the
//           storage object.  In that case, pass the length in this arg.
//           O/w, pass -1.
//
// DESCRIPTION
//
//  This function is used internally by ArchiveLib.  It is used to read
//  random length blocks of data out of archives (or other storage objects).
//
// RETURNS
//
//  AL_SUCCESS, or not.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::WriteString()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::ReadString( ALName AL_DLL_FAR &name,  /* Tag public function */
                       short int length /* = -1 */ )
{
    if ( length == -1 ) {
        if ( ReadGlShort( length ) < 0 )
        return mStatus.SetError( AL_READ_ERROR,
                                 "Error reading string length"
                                 "in ReadString() for object %s",
                                  mName.GetSafeName() );
    }
    char *new_string = new char[ length + 1 ];
    if ( new_string ) {
        ReadBuffer( (unsigned char *) new_string, length );
        new_string[ length ] = '\0';
        name = new_string;
        delete[] new_string;
        return AL_SUCCESS;
    }
    return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                             "Error allocating buffer space in call "
                             "to ReadString() for object %s",
                              mName.GetSafeName() );
}

//
// NAME
//
//  ALStorage::Tell()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return the current offset in the storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALStorage::Tell();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALStorageTell( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageTell Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALStorageTell( this_object : hALStorage ) : LongInt;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to have its offset returned.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  Because we are using buffered I/O here, figuring out the current
//  position of the read write pointer is just a tiny bit more complicated
//  than just checking a pointer.  We have to find the physical location of
//  the file pointer, then add in any offset created by the presence of
//  data in the I/O buffer.
//
// RETURNS
//
//  A long integer indicating the current position of the read/write
//  pointer for the file.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Seek(), ALStorage::WriteBuffer()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

long AL_PROTO
ALStorage::Tell()  /* Tag public function */
{
    if ( muWriteIndex )
        return mlFilePointer + muWriteIndex;
    else
        return mlFilePointer - muBufferValidData + muReadIndex;

}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageTell( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageTell" );
    return ( (ALStorage *) this_object )->Tell();
}

#endif

//
// NAME
//
//  ALStorage::WriteStorageObjectData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Write out class-specific storage object data.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::WriteStorageObjectData( ALStorage * archive );
//
// C SYNOPSIS
//
//  None, this is an internal callback virtual function.
//
// VB SYNOPSIS
//
//  None, this is an internal callback virtual function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal callback virtual function.
//
// ARGUMENTS
//
//  archive : A pointer to the storage object where we are going to
//            write the private data.
//
// DESCRIPTION
//
//  All storage objects have the ability to create a private data block
//  that will be stored along with the directory when creating an archive.
//  None of the classes predefined in ArchiveLib use this data block, which
//  means they use this function instead of providing their own virtual
//  substitute. This function writes a private data block of exactly 0
//  bytes in length.  Our internal storage format means that a block
//  of 0 bytes length takes 2 bytes to store.
//
// RETURNS
//
//  AL_SUCCESS if things went okay, otherwise an error code < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::WriteStorageObjectData( ALStorage AL_DLL_FAR * archive )  /* Tag public function */
{
    return archive->WriteGlShort( 0 );
}

//
// NAME
//
//  ALStorage::ReadStorageObjectData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read in class specific data from an archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::ReadStorageObjectData( ALStorage * archive );
//
// C SYNOPSIS
//
//  None, this is an internal callback virtual function.
//
// VB SYNOPSIS
//
//  None, this is an internal callback virtual function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal callback virtual function.
//
// ARGUMENTS
//
//  archive : A pointer to the storage object where we are going to
//            read in the private data..
//
// DESCRIPTION
//
//  All storage objects have the ability to create a private data block
//  that will be stored along with the directory when creating an archive.
//  None of the classes predefined in ArchiveLib use this data block, which
//  means they use this function instead of providing their own virtual
//  substitute. This function reads a private data block of exactly 0
//  bytes in length.  Our internal storage format means that a block
//  of 0 bytes length takes 2 bytes to store.
//
//  In debug mode, we get really bent out of shape if this data block
//  doesn't look exactly like we expect it to.
//
// RETURNS
//
//  AL_SUCCESS if things went okay, otherwise an error code < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::WriteStorageObjectData()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALStorage::ReadStorageObjectData( ALStorage AL_DLL_FAR * archive )  /* Tag public function */
{
    short int temp;
    int status = archive->ReadGlShort( temp );
    AL_ASSERT( temp == 0, "ReadStorageObjectData: stored data is not null" );
    return status;
}

//
// NAME
//
//  ALStorage::ReadCopyright()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read a copyright string for human display.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  char * ALStorage::ReadCopyright();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char * ALReadCopyright( void );
//
// VB SYNOPSIS
//
//  Declare Function ALReadCopyright Lib "AL20FW"
//    Alias "ALReadCopyrightVB"
//    As String
//
// DELPHI SYNOPSIS
//
//  function ALReadCopyright : PChar;
//
// ARGUMENTS
//
//  None.  Note that we don't have the usual this_object argument, because
//  this is a static function.  It's only in the ALStorage class to avoid
//  name space clutter.
//
// DESCRIPTION
//
//  It is a good idea for us to have a copyright notice embedded in the
//  library.  Hopefully, this notice will show up in any executables linked
//  using this library, or in the DLL they link to.
//
// RETURNS
//
//  A pointer to a copyright notice.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

char *_al_copyright =  "Copyright (c) 1994-1996 Greenleaf Software, Inc.\n"
                       "All Rights Reserved.\n";

char AL_DLL_FAR * AL_PROTO
ALStorage::ReadCopyright()  /* Tag public function */
{
    return _al_copyright;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALReadCopyright( void ) /* Tag public function */
{
    return _al_copyright;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALReadCopyrightVB( void )  /* Tag public function */
{
    return ALCreateVBString( _al_copyright,
                            (unsigned short int) _fstrlen( _al_copyright ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALReadCopyrightVB( void )  /* Tag public function */
{
    return SysAllocStringByteLen( _al_copyright, strlen( _al_copyright ) );
}

#endif /* #if defined( AL_VB ) */

#endif /* #if !defined( AL_NO_C ) */

