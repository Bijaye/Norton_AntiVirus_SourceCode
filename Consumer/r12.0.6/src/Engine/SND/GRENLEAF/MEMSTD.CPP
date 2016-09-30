//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/MEMSTD.CPv   1.0   12 Mar 1998 12:16:58   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/MEMSTD.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:58   DCHI
// Initial revision.
// 
//************************************************************************

//
// MEMSTD.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALMemory::operator new()
//  ALMemory::ALMemory()
//  newALMemory()
//  ALMemory::~ALMemory()
//  ALMemory::_LoadBuffer()
//  ALMemory::Delete()
//  ALMemory::GrowUserBuffer()
//  ALMemory::_FlushBuffer()
//  ALMemory::Close()
//  ALMemory::Create()
//  ALMemory::Open()
//  ALMemory::Clone()
//
// DESCRIPTION
//
//  This file contains the C++ member functions to support class
//  ALMemory.  This class works very closely with the parent class,
//  ALMemoryBase, found in MEMSTORE.CPP.
//
// REVISION HISTORY
//
//  May 22, 1994  1.0A  : First release
//
//  July 7, 1994  1.0B  : Had to make a lot of changes to support file
//                        management under UNIX.
//
//   February 14, 1996  2.0A : New Release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "memstore.h"

#include <stdlib.h>  // might be using malloc()!

const size_t max_alloc = (size_t) ~0;

//
// NAME
//
//  ALMemory::operator new()
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
//  #include "memstore.h"
//
//  void * ALMemory::operator new( size_t size )
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
//           necessary to construct a new ALMemory object.
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
//   February 14, 1996  2.0A : New Release
//


#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALMemory::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif


//
// NAME
//
//  ALMemory::ALMemory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Constructs a new ALMemory object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  ALMemory::ALMemory( const char *buffer_name = "",
//                      char *user_buffer = 0 ,
//                      size_t user_buffer_size = 0,
//                      ALCase name_case = AL_MIXED );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  hALStorage newALMemory( char *buffer_name,
//                          char *user_buffer,
//                          int user_buffer_size );
//
// VB SYNOPSIS
//
//  None, VB should use newALWinMemory();
//
// DELPHI SYNOPSIS
//
//  None, Delphi should use newALWinMemory();
//
// ARGUMENTS
//
//  buffer_name  : An arbitrary name assigned to the buffer.  Buffer
//                 names don't have to be unique, because buffers aren't
//                 named at the operating system level.  But if you are
//                 going to insert the storage object into an archive, the
//                 name needs to be unique so that you will be able to
//                 extract it properly.
//
//  user_buffer  : If you want the ALMemory class to automatically
//                 allocate a buffer for you, and grow it as necessary, just
//                 leave this pointer set to 0.  If you want to use your own
//                 buffer, which won't have the ability to grow, pass a
//                 pointer to it in this parameter.
//
//  user_buffer_size : If you are passing a pointer to your own buffer,
//                     you need to indicate how large it is here.  Since
//                     we are dealing with standard buffers here, this is of
//                     type size_t.
//
//  name_case    : This decides whether you want the file name to be
//                 case sensitive when making comparisons.  MS-DOS
//                 file names are case-insensitive.  You can make memory
//                 buffers either mixed case, forced upper, or forced
//                 lower.  The default of mixed case means that comparisons
//                 will be case sensitive, which is fine.
//
//                 Note that I don't even give the option of using name_case
//                 to C users.  They are going to get the default
//                 value assigned to their OS/environment.
//
// DESCRIPTION
//
//  This constructor calls the base class constructor in an initializer
//  list, which takes care of most of the dirty work right away.  After that
//  is done, all the constructor has to do is initialize a few data members.
//  That should be self-explanatory.  Remember that if the user doesn't
//  supply a buffer, we are going to allocate it for her, but not until
//  there is actually a demand for memory.
//
// RETURNS
//
//  If memory allocation is being done dynamically, this function returns
//  a pointer/handle to a new ALMemory object.  If it is called as
//  a static or automatic constructor (only possible in C++), it returns
//  nothing.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALMemoryBase::ALMemoryBase(), ALStorage::ALStorage(),
//  ALMemory::~ALMemory()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALMemory::ALMemory( const char AL_DLL_FAR *buffer_name /* = "" */,  /* Tag public function */
                    char AL_DLL_FAR *user_buffer /* = 0 */ ,
                    size_t user_buffer_size /* = 0 */,
                    ALCase name_case /* = AL_MIXED */ )
    : ALMemoryBase( buffer_name, name_case )
{
    if ( user_buffer != 0 ) {
        mpcUserBuffer = user_buffer;
        mfUserOwnsBuffer = 1;
        mlUserBufferSize = user_buffer_size;
    } else {
        mfUserOwnsBuffer = 0;
        mpcUserBuffer = 0;
        mlUserBufferSize = 0;
    }
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALStorage AL_FUNCTION
newALMemory( char AL_DLL_FAR *buffer_name,  /* Tag public function */
             char AL_DLL_FAR *user_buffer,
             size_t user_buffer_size )
{
    if ( user_buffer_size == 0 )
        return (hALStorage) new ALMemory( buffer_name );
    else
        return (hALStorage) new ALMemory( buffer_name,
                                          user_buffer,
                                          user_buffer_size );
}

#endif

//
// NAME
//
//  ALMemory::~ALMemory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Destroy an ALMemory object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  ALMemory::~ALMemory();
//
// C SYNOPSIS
//
//  None, use the base class destructor: deleteALStorage().
//
// VB SYNOPSIS
//
//  None, use the base class destructor: deleteALStorage().
//
// DELPHI SYNOPSIS
//
//  None, use the base class destructor: deleteALStorage().
//
// ARGUMENTS
//
//  None, you don't get any for a destructor.
//
// DESCRIPTION
//
//  The destructor has just one thing it has to do before this object
//  goes away.  If the buffer that it has been using all along doesn't
//  belong to the user, then it is the class's responsibility to get
//  rid of it.  We do so here, using the standard version of free.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALMemory::ALMemory()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALMemory::~ALMemory()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALMemory: attempting to delete invalid object" );
    if ( !mfUserOwnsBuffer ) {
        if ( mpcUserBuffer ) {
            free( mpcUserBuffer );
            mpcUserBuffer = 0;
        }
    }
    AL_ASSERT( GoodTag(), "~ALMemory: attempting to delete invalid object" );
}

//
// NAME
//
//  ALMemory::_LoadBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read memory from the big buffer into the local I/O buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  void ALMemory::_LoadBuffer( long address );
//
// C SYNOPSIS
//
//  None, internal protected function.
//
// VB SYNOPSIS
//
//  None, internal protected function.
//
// DELPHI SYNOPSIS
//
//  None, internal protected functoin.
//
// ARGUMENTS
//
//  address      :  The offset in the memory object that is going to be
//                  loaded.
//
// DESCRIPTION
//
//  External users of an ALStorage class perform all of their access via
//  a local I/O buffer.  Functions such as ReadChar() and WriteChar() look
//  at a thing called mpcBuffer for their data.  When reading from
//  mpcBuffer, you are going to run out of data from time to time.  When
//  this happens, you will generate a call to the virtual function
//  LoadBuffer().
//
//  As it happens, all of the ALMemory objects share a common version
//  of LoadBuffer().  LoadBuffer() still has to call something a little
//  more specialized though, and that's where this version of _LoadBuffer()
//  comes into play.  It just performs a memcpy() routine to actually move
//  data out of the big memory buffer and into the local I/O buffer
//  used by ReadChar() et. al.
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

void AL_PROTO
ALMemory::_LoadBuffer( long address )  /* Tag protected function */
{
    memcpy( mpcBuffer, mpcUserBuffer + (size_t) address, muBufferValidData );
}

//
// NAME
//
//  ALMemory::Delete()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Delete the underlying buffer for the memory object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemory::Delete();
//
// C SYNOPSIS
//
//  None, C programs use the base class function deleteALStorage().
//
// VB SYNOPSIS
//
//  None, VB programs use the base class function deleteALStorage().
//
// DELPHI SYNOPSIS
//
//  None, Delphi programs use the base class function deleteALStorage().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is analogous to the unlink() RTL function for files.  It
//  has to close the file, and get rid of its big buffer.  This is fairly
//  easy with memory buffers, we just call the appropriate version of
//  free() to delete the buffer.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALMemory::Create(), ALStorage::Delete()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALMemory::Delete()  /* Tag public function */
{
    if ( !mfUserOwnsBuffer ) {
        free( mpcUserBuffer );
        mpcUserBuffer = 0;
    }
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemory::GrowUserBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Enlarge the user buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemory::GrowUserBuffer( long minimum_new_size );
//
// C SYNOPSIS
//
//  None, this is an internal protected C++ function.
//
// VB SYNOPSIS
//
//  None, this is an internal protected C++ function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal protected C++ function.
//
// ARGUMENTS
//
//  minimum_new_size : This is the size that the caller absolutely must
//                     have to successfully perform a write.  Anything
//                     less than this won't do.
//
// DESCRIPTION
//
//  Sometimes a write to a memory object goes past the current end of the
//  buffer.  When this happens, code in the base class calls this
//  function to attempt to enlarge the buffer.
//
//  Enlarging the buffer is tricky, because you have to allocate new space,
//  then copy the old buffer into the new buffer.  This means you
//  temporarily need a boot-load of space.  If you are lucky, the realloc()
//  function might be able to attempt to avoid this situation.
//
//  We try to enlarge things by a fixed amount, large enough to prevent
//  thrashing.  But if that doesn't fly, we can fall back and try to
//  enlarge to the minimum acceptable size.
//
// RETURNS
//
//  AL_SUCCESS if all went well, some error code < AL_SUCCESS if not.
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
ALMemory::GrowUserBuffer( long minimum_new_size )  /* Tag protected function */
{
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mfUserOwnsBuffer )
        return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                 "Attempt to write past the end of a "
                                 "user owned buffer for ALMemory "
                                 "%s",
                                 mName.GetSafeName() );
    if ( minimum_new_size >= max_alloc )
        return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                   "Attempt to allocate a buffer "
                                   "of %ld bytes for ALMemory "
                                   "%s",
                                   minimum_new_size,
                                   mName.GetSafeName() );
    long trial_size = mlUserBufferSize + 16384;
    if ( trial_size >= max_alloc )
        trial_size = max_alloc - 16;
    if ( trial_size >= minimum_new_size ) {
        char *new_buf = (char *) realloc( mpcUserBuffer, (size_t) trial_size );
        if ( new_buf ) {
            mpcUserBuffer = new_buf;
            mlUserBufferSize = (size_t) trial_size;
            return AL_SUCCESS;
        }
    }
    char *new_buf = (char *) realloc( mpcUserBuffer, (size_t) minimum_new_size );
    if ( new_buf ) {
        mpcUserBuffer = new_buf;
        mlUserBufferSize = (size_t) minimum_new_size;
        return AL_SUCCESS;
    }
    return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                             "Allocation failure when attempting to "
                             "allocate a buffer "
                             "of %ld bytes for ALMemory "
                             "%s",
                             minimum_new_size,
                             mName.GetSafeName() );
}

//
// NAME
//
//  ALMemory::_FlushBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Flush data to the big buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  void ALMemory::_FlushBuffer( long address )
//
// C SYNOPSIS
//
//  None, internal protected C++ function.
//
// VB SYNOPSIS
//
//  None, internal protected C++ function.
//
// DELPHI SYNOPSIS
//
//  None, internal protected C++ function.
//
// ARGUMENTS
//
//  address : The address in the big buffer where the flush should write
//            to.
//
// DESCRIPTION
//
//  When performing WriteChar() or WriteBuffer() operations, ALStorage
//  causes output to be directed to a small I/O buffer.  When this I/O
//  buffer gets full, a call to ALFlushBuffer() is generated, which is
//  supposed to dump that memory to a physical device.
//
//  When ALMemoryBase gets a call to FlushBuffer(), it handles almost
//  everything on its own.  The one thing it can't handle, however, is
//  the routine to copy the I/O buffer out to the big memory object.
//  It has to really on this dinky virtual function to do the job.
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

void AL_PROTO
ALMemory::_FlushBuffer( long address )  /* Tag protected function */
{
    memcpy( mpcUserBuffer + (size_t) address, mpcBuffer, muWriteIndex );
}

//
// NAME
//
//  ALMemory::Close()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Close an open memory buffer object
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemory::Close()
//
// C SYNOPSIS
//
//  C programs should use the base class function ALStorageClose().
//
// VB SYNOPSIS
//
//  VB programs should use the base class function ALStorageClose().
//
// DELPHI SYNOPSIS
//
//  Delphi programs should use the base class function ALStorageClose().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  Close() is supposed to do the same thing to a memory buffer as fclose()
//  in the RTL does to a file.  The most important thing we are concerned
//  about is that the I/O buffer gets freed up by the base class, so this
//  suddenly might not be a giant heavyweight object any more.
//
//  After freeing things up in the base class, we check to see if
//  we have allocated more space than we really need.  If so, we do
//  a realloc() of some sort to give space back to the O/S.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALMemory::Open(), ALStorage::Close()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALMemory::Close()  /* Tag public function */
{
    ALMemoryBase::Close();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( ! mfUserOwnsBuffer ) {
        if ( mlSize != 0 ) {
            char *new_buf = (char *) realloc( mpcUserBuffer, (size_t) mlSize );
            if ( new_buf ) {
                mpcUserBuffer = new_buf;
                mlUserBufferSize = mlSize;
            }
        } else {
            free( mpcUserBuffer );
            mpcUserBuffer = 0;
        }
    }
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemory::Create()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create the memory storage object big buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemory::Create( long init_size );
//
// C SYNOPSIS
//
//  C programs should use the base class function ALStorageCreate().
//
// VB SYNOPSIS
//
//  VB programs should use the base class function ALStorageCreate().
//
// DELPHI SYNOPSIS
//
//  Delphi programs should use the base class function ALStorageCreate().
//
// ARGUMENTS
//
//  init_size  :  When you create an ALMemory object of any kind, you can
//                write out data to it at your own pace, without having any
//                idea how much space you will need.  The storage object
//                tries to increase its size every time you fill up
//                the current huge buffer.  Well, if you know in advance how
//                much space you are going to need, you can allocate the
//                whole buffer at once, and avoid all that extra work.  So
//                some calls to Create() now pass on an initial size using
//                this argument.
//
// DESCRIPTION
//
//  This is like creating a new file.  If there isn't a memory buffer
//  already assigned to this object, we create one, with an initial
//  allocation of 16Kbytes, or more if requested.
//
// RETURNS
//
//  Either AL_SUCCESS, or an unfriendly error code.
//
// EXAMPLE
//
// SEE ALSO
//
// ALStorage::Create(), ALMemory::Open()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALMemory::Create( long init_size )  /* Tag public function */
{
    ALMemoryBase::Create();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mpcUserBuffer )
        return AL_SUCCESS; //If a buffer was already created somewhere down the
                           //line, we won't do it again.
    if ( init_size == -1L )
        init_size = 16384;
    if ( init_size > max_alloc )
        return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                 "Allocation failure when attempting to "
                                 "initialize a buffer "
                                 "of %ld bytes for ALMemory "
                                 "%s in _Create()",
                                 init_size,
                                 mName.GetSafeName() );
    mpcUserBuffer = (char *) malloc( (size_t) init_size );
    mlUserBufferSize = init_size;
    if ( mpcUserBuffer == 0 )
        return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                 "Allocation failure when attempting to "
                                 "create a buffer "
                                 "of %ld bytes for ALMemory "
                                 "%s in _Create()",
                                 mlUserBufferSize,
                                 mName.GetSafeName() );
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemory::Open()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Open an existing memory storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemory::Open();
//
// C SYNOPSIS
//
//  C programs should use the base class function ALStorageOpen().
//
// VB SYNOPSIS
//
//  VB programs should use the base class function ALStorageOpen().
//
// DELPHI SYNOPSIS
//
//  Delphi programs should use the base class function ALStorageOpen().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This is like opening an existing file.  Since there is supposed to be
//  an existing memory buffer already, we gripe if we can't find one.
//
// RETURNS
//
//  Either AL_SUCCESS, or an unfriendly error code.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Open(), ALMemory::Create()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALMemory::Open()  /* Tag public function */
{
    ALMemoryBase::Open();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mpcUserBuffer == 0 )
        return mStatus.SetError( AL_CANT_OPEN_FILE,
                                   "Attempt to open ALMemory %s "
                                   "with no buffer allocated",
                                   mName.GetSafeName() );
    else
        mlSize = mlUserBufferSize;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemory::Clone()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Clone this memory based storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  ALStorage ALMemory::Clone( const char *name,
//                             int object_type ) const;
//
// C SYNOPSIS
//
//  None, this is an internal C++ function
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
//  name         :  The desired name of the new object.  Usually this will
//                  be a name found in an Archive directory.
//
//  object_type  :  The type of object we want to create.  Only
//                  AL_STORAGE_DEFAULT and AL_MEMORY_OBJECT will cause this
//                  function to succeed.
//
// DESCRIPTION
//
//  The virtual Clone() function is used by archiving programs to act
//  as a virtual constructor.  When preparing to create storage objects
//  based on the contents of an Archive directory, the archiving code can
//  call Clone() for all the storage objects in its toolkit until it finds
//  one that responds to its object type.
//
//  For example, if an archive contained an AL_MEMORY object, and we were
//  extracting, and an ALFile object was in the toolkit, it would call
//  ALFile::Clone() from the toolkit object, with an object type of
//  AL_MEMORY_OBJECT.  This Clone() function would fail.  Hopefully, there
//  would be a memory based storage object in the toolkit that would
//  respond properly to the Clone() call.
//
//  Another object in the same archive might have an AL_FILE_OBJECT type.
//  When the archiving code called Clone() again with that object type,
//  we would successfully create the new File object in Clone().
//
// RETURNS
//
//  Either a pointer to a newly constructed ALHugeMemory object, or a zero
//  in case of error.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

ALStorage AL_DLL_FAR *
ALMemory::Clone( const char AL_DLL_FAR *name,  /* Tag public function */
                 int object_type ) const
{
    switch ( object_type ) {
        case AL_STORAGE_DEFAULT :
        case AL_MEMORY_OBJECT :
            return new ALMemory( name );
    }
    return 0;
}

