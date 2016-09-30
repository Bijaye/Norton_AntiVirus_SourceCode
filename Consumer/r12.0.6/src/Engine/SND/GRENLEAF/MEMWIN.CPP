//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/MEMWIN.CPv   1.0   12 Mar 1998 12:17:00   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/MEMWIN.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:00   DCHI
// Initial revision.
// 
//************************************************************************

//
// MEMWIN.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALWinMemory::operator new()
//  ALWinMemory::ALWinMemory()
//  newALWinMemory()
//  newALWinMemoryVB32()
//  ALWinMemory::~ALWinMemory()
//  ALWinMemory::_LoadBuffer( long address )
//  ALWinMemory::Delete()
//  ALWinMemory::GrowUserBuffer()
//  ALWinMemory::_FlushBuffer( long address )
//  ALWinMemory::Close()
//  ALWinMemory::Create()
//  ALWinMemory::Open()
//  ALWinMemory::Clone()
//  ALWinMemoryCopyBufferVB()
//
// DESCRIPTION
//
//  This file contains all the code for ALWinMemory.  This works hand
//  in hand with ALMemoryBase.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if defined( AL_VB )
#include "_vbutil.h"
#endif

//
// Problem here with PowerPack and others that use the NT
// API, but don't support the whole thing
//
#ifdef IsBadWritePtr
#undef IsBadWritePtr
#endif

#include <windows.h>
#include "memstore.h"

//
// NAME
//
//  ALWinMemory::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  void * ALWinMemory::operator new( size_t size )
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
//           necessary to construct a new ALWinMemory object.
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
//  NOTE!!!:  Don't get confused about one thing.  This function isn't
//            allocating the buffer that's going to hold the data you
//            write using functions such as WriteChar().  It is just
//            allocating the space for the object itself!
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
//   February 14, 1996  2.0A : New release.
//


#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALWinMemory::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALWinMemory::ALWinMemory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create a Windows based memory object
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  ALWinMemory::ALWinMemory(  const char *buffer_name = "",
//                             char *user_buffer = 0,
//                             DWORD user_buffer_size = 0,
//                             ALCase name_case = AL_MIXED );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  hALStorage newALWinMemory( char *buffer_name,
//                             char *user_buffer,
//                             DWORD user_buffer_size );
//
// VB SYNOPSIS
//
//  Declare Function newALWinMemory Lib "AL20LW"
//    (ByVal buffer_name$, ByVal user_buffer$, ByVal user_buffer_size&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALWinMemory( buffer_name : PChar;
//                           user_buffer : PChar;
//                           user_buffer_size : LongInt ) : hALStorage;
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
//  user_buffer  : If you want the ALMemory class to automatically allocate
//                 a buffer for you, and grow it as necessary, just leave
//                 this pointer set to 0.  If you want to use your own buffer,
//                 which won't have the ability to grow, pass a pointer to
//                 it in this parameter.  Note that under Windows 16 this
//                 is a huge pointer, meaning it can span segments, and
//                 access potentially 16 Mbytes of memory.
//
//  user_buffer_size : If you are passing a pointer to your own buffer,
//                     you need to indicate how large it is here.
//
//  name_case    : This decides whether you want the file name to be
//                 case sensitive when making comparisons.  MS-DOS
//                 file names are case-insensitive.  You can make memory
//                 buffers either mixed case, forced upper, or forced
//                 lower.  The default of mixed case means that comparisons
//                 will be case sensitive, which is fine.  Note that I
//                 force C/VB/Delphi users to take the default of AL_MIXED.
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
//  Under C/VB/Delphi, it returns a pointer to a newly constructed ALWinMemory
//  object.  Ditto if called via new under C++.  O/W, returns nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_PROTO ALWinMemory::ALWinMemory(  /* Tag public function */
                      const char AL_DLL_FAR *buffer_name /* = "" */,
                      char AL_HUGE *user_buffer /* = 0 */,
                      DWORD user_buffer_size /* = 0 */,
                      ALCase name_case /* = AL_MIXED */ )
    : ALMemoryBase( buffer_name, name_case )
{
#if 0
    char buf[ 128 ];
    wsprintf( buf,
              "buffer_name = %s  "
              "user_buffer = %lx  "
              "user_buffer_size = %ld  "
              "name_case = %d",
              buffer_name,
              user_buffer,
              user_buffer_size,
              name_case );
    MessageBox( 0, buf, "Title", MB_OK );
#endif
    if ( user_buffer != 0 ) {
#if 0
        strncpy( buf, user_buffer, 40 );
        buf[ 40 ] = '\0';
        MessageBox( 0, buf, "Preview", MB_OK );
#endif
        mpcUserBuffer = user_buffer;
        mfUserOwnsBuffer = 1;
        mlUserBufferSize = user_buffer_size;
    } else {
        mfUserOwnsBuffer = 0;
        mpcUserBuffer = 0;
        mlUserBufferSize = 0;
    }
    mhUserMemoryHandle = 0;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALStorage AL_FUNCTION
newALWinMemory( char AL_DLL_FAR *buffer_name,  /* Tag public function */
                char AL_HUGE *user_buffer,
                DWORD user_buffer_size )
{
    if ( user_buffer_size == 0 )
        return (hALStorage) new ALWinMemory( buffer_name );
    else
        return (hALStorage) new ALWinMemory( buffer_name, user_buffer, user_buffer_size );
}

#endif

//
// Note that things are a little different here for VB32.
// Instead of *using* the buffer passed here, we just
// make a copy of it.
//
#if defined( AL_VB32 )

extern "C" AL_LINKAGE hALStorage AL_FUNCTION
newALWinMemoryVB32( char AL_DLL_FAR *buffer_name,  /* Tag public function */
                    LPSAFEARRAY *ppsa,
                    size_t user_buffer_size )
{
    if ( user_buffer_size == 0 )
        return (hALStorage) new ALWinMemory( buffer_name );
//
// I have to allocate some space, then do a copy into it.
//
    ALWinMemory *m = new ALWinMemory( buffer_name, 0, 0 );
    m->mhUserMemoryHandle = GlobalAlloc( GMEM_MOVEABLE, user_buffer_size );
    if ( m->mhUserMemoryHandle ) {
        m->mpcUserBuffer = (char AL_HUGE *) GlobalLock( (HGLOBAL) m->mhUserMemoryHandle );
        m->mlUserBufferSize = user_buffer_size;
        unsigned char *p;
        SafeArrayAccessData( *ppsa, (void **) &p );
        memcpy( m->mpcUserBuffer, p, user_buffer_size );
        SafeArrayUnaccessData( *ppsa );
    } else {
        delete m;
        return 0;
    }
    return (hALStorage) m;
}

#endif

//
// NAME
//
//  ALWinMemory::~ALWinMemory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  C++
//
// SHORT DESCRIPTION
//
//  The destructor for ALWinMemory objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  ALWinMemory::~ALWinMemory()
//
// C SYNOPSIS
//
//  None, C programs have to call the base class dtor, deleteALStorage().
//
// VB SYNOPSIS
//
//  None, VB programs have to call the base class dtor, deleteALStorage().
//
// DELPHI SYNOPSIS
//
//  None, Delphi programs have to call the base class dtor, deleteALStorage().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor has just one thing it has to do before this object
//  goes away.  If the buffer that it has been using all along doesn't
//  belong to the user, then it is the class's responsibility to get
//  rid of it.
//
//  Note also that we check the GoodTag() function when in Debug mode.
//  That will help catch really bad mistakes, such as trying to delete
//  an object that is not even an ALMemory object, maybe a beer can.
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
ALWinMemory::~ALWinMemory()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALWinMemory: attempting to delete invalid object" );
    if ( !mfUserOwnsBuffer ) {
        if ( mpcUserBuffer ) {
            GlobalUnlock( (HGLOBAL) mhUserMemoryHandle );
            GlobalFree( (HGLOBAL) mhUserMemoryHandle );
            mhUserMemoryHandle= 0;
            mpcUserBuffer = 0;
        }
    }
    AL_ASSERT( GoodTag(), "~ALWinMemory: attempting to delete invalid object" );
}

//
// NAME
//
//  ALWinMemory::_LoadBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  void ALWinMemory::_LoadBuffer( long address );
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
//   February 14, 1996  2.0A : New release.
//

void AL_PROTO
ALWinMemory::_LoadBuffer( long address )  /* Tag protected function */
{
//
// Some problems passing huge arrays to memcpy, got to do it inline instead
// I think Microsoft says memcpy() will work with huge pointers as long
// as you don't try to use the inline optimizations, but I say why take
// chances...
//
// Another note: AL_HUGE is _huge for win16, but blank for win32.
//
    char AL_HUGE *temp = mpcUserBuffer + address;
    for ( unsigned int i = 0 ; i < muBufferValidData ; i++ )
        mpcBuffer[ i ] = *temp++;
//    memcpy( mpcBuffer, mpcUserBuffer +          address, muBufferValidData );
}

//
// NAME
//
//  ALWinMemory::Delete()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  int ALWinMemory::Delete();
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
//  easy with memory buffers, we just call GlobalFree() to delete the buffer.
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

int AL_PROTO
ALWinMemory::Delete()  /* Tag public function */
{
    if ( !mfUserOwnsBuffer ) {
        GlobalUnlock( (HGLOBAL) mhUserMemoryHandle );
        GlobalFree( (HGLOBAL) mhUserMemoryHandle );
        mhUserMemoryHandle= 0;
        mpcUserBuffer = 0;
    }
    return AL_SUCCESS;
}

//
// NAME
//
//  ALWinMemory::GrowUserBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  int ALWinMemory::GrowUserBuffer( long minimum_new_size );
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
//  temporarily need a boot-load of space.  If you are lucky, GlobalReallcoc()
//  might be able to attempt to avoid this situation.
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
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALWinMemory::GrowUserBuffer( long minimum_new_size )  /* Tag protected function */
{
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mfUserOwnsBuffer )
        return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                   "Attempt to write past the end of a "
                                   "user owned buffer for ALWinMemory "
                                   "%s",
                                   mName.GetSafeName() );
    long trial_size = mlUserBufferSize + 16384;
    GlobalUnlock( (HGLOBAL) mhUserMemoryHandle );
    HGLOBAL new_handle = GlobalReAlloc( (HGLOBAL) mhUserMemoryHandle, trial_size, GMEM_MOVEABLE );
    if ( new_handle == 0 ) {
        trial_size = minimum_new_size;
        new_handle = GlobalReAlloc( (HGLOBAL) mhUserMemoryHandle, trial_size, GMEM_MOVEABLE );
    }
    if ( new_handle == 0 ) {
        mpcUserBuffer = (char AL_HUGE *) GlobalLock( (HGLOBAL) mhUserMemoryHandle );
        return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                 "Allocation failure when attempting to "
                                 "allocate a buffer "
                                 "of %ld bytes for ALMemoryBase "
                                 "%s",
                                 minimum_new_size,
                                 mName.GetSafeName() );
    }
    mpcUserBuffer = (char AL_HUGE *) GlobalLock( new_handle );
    mhUserMemoryHandle = new_handle;
    mlUserBufferSize = trial_size;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALWinMemory::_FlushBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  void ALWinMemory::_FlushBuffer( long address )
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
//   February 14, 1996  2.0A : New release.
//

void AL_PROTO
ALWinMemory::_FlushBuffer( long address )  /* Tag protected function */
{
//
// Can't use memcpy with huge pointers, at least not with the optimized
// versions.
//
        char AL_HUGE *temp = mpcUserBuffer + address;
        for ( unsigned int i = 0 ; i < muWriteIndex ; i++ )
            *temp++ = mpcBuffer[ i ];
//        memcpy( mpcUserBuffer +          mlFilePointer, mpcBuffer, muWriteIndex );
}

//
// NAME
//
//  ALWinMemory::Close()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  C++
//
// SHORT DESCRIPTION
//
//  Close an open big memory buffer object
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALWinMemory::Close()
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
//  a GlobalRealloc() of some sort to give space back to the O/S.
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

int AL_PROTO
ALWinMemory::Close()  /* Tag public function */
{
    ALMemoryBase::Close();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( !mfUserOwnsBuffer && mlSize < mlUserBufferSize ) {
        GlobalUnlock( (HGLOBAL) mhUserMemoryHandle );
        HGLOBAL new_handle = GlobalReAlloc( (HGLOBAL) mhUserMemoryHandle, mlSize, GMEM_MOVEABLE );
        if ( new_handle != 0 )
            mhUserMemoryHandle = new_handle;
        mpcUserBuffer = (char AL_HUGE *) GlobalLock( (HGLOBAL) mhUserMemoryHandle );
        mlUserBufferSize = mlSize;
    }
    return mStatus;
}

//
// NAME
//
//  ALWinMemory::Create()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  int ALWinMemory::Create( long init_size );
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
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALWinMemory::Create( long init_size )  /* Tag public function */
{
    ALMemoryBase::Create();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mpcUserBuffer )
        return AL_SUCCESS; //If a buffer was already created somewhere down the
                           //line, we won't do it again.
    if ( init_size == -1L )
        init_size = 16384;
    mhUserMemoryHandle = GlobalAlloc( GMEM_MOVEABLE, init_size );
    if ( mhUserMemoryHandle ) {
        mpcUserBuffer = (char AL_HUGE *) GlobalLock( (HGLOBAL) mhUserMemoryHandle );
        mlUserBufferSize = init_size;
    } else {
        mpcUserBuffer = 0;
        return mStatus.SetError( AL_CANT_ALLOCATE_MEMORY,
                                 "Allocation failure when attempting to "
                                 "create a buffer "
                                 "of %ld bytes for ALWinMemory "
                                 "%s in Create()",
                                 init_size,
                                 mName.GetSafeName() );
    }
    return mStatus;
}

//
// NAME
//
//  ALWinMemory::Open()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  int ALWinMemory::Open();
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
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALWinMemory::Open()  /* Tag public function */
{
    ALMemoryBase::Open();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mpcUserBuffer == 0 )
        return mStatus.SetError( AL_CANT_OPEN_FILE,
                                 "Attempt to open ALWinMemory %s "
                                 "with no buffer allocated",
                                 mName.GetSafeName() );
    else
        mlSize = mlUserBufferSize;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALWinMemory::Clone()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
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
//  ALStorage ALWinMemory::Clone( const char *name,
//                                int object_type ) const;
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
//   February 14, 1996  2.0A : New release.
//

ALStorage AL_DLL_FAR *
ALWinMemory::Clone( const char AL_DLL_FAR *name,  /* Tag public function */
                    int object_type ) const
{
    switch ( object_type ) {
        case AL_STORAGE_DEFAULT :
        case AL_MEMORY_OBJECT :
            return new ALMemory( name );
    }
    return 0;
}

//
// NAME
//
//  ALWinMemoryCopyBufferVB()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  VB
//
// SHORT DESCRIPTION
//
//  Copy the contents of an ALWinMemory object to a VB string.
//
// C++ SYNOPSIS
//
//  None, only useful to VB programs.
//
// C SYNOPSIS
//
//  None, only useful to VB programs.
//
// VB SYNOPSIS
//
//  Declare Function ALWinMemoryCopyBufferVB Lib "AL20LW"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  None, only useful to VB programs.
//
// ARGUMENTS
//
//  this_object  :  A handle for (pointer to) an ALWinMemory object.
//
// DESCRIPTION
//
//  This VB translation function provides access to the data stored
//  in the buffer of an ALWinMemory object.  It does this by creating a
//  VB string with a copy of the data.  We don't do any checking here,
//  so it is possible to abort VB if the string is too large.
//
//  This function was sort of a missing link in our ability to handle
//  memory objects in VB.  It was always easy to convert a VB string to
//  an ALWinMemory object, but we didn't have any good way to make the
//  reverse trip.
//
// RETURNS
//
//  A VB string that contains the contents of the memory object.
//  Note that the memory object is still there, unchanged, but now
//  you can easily get at its data using VB.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

#if defined( AL_VB )

extern "C" long AL_FUNCTION
ALWinMemoryCopyBufferVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALWinMemory, "ALWinMemoryCopyBufferVB" );
    ( (ALWinMemory *) this_object )->FlushBuffer();
    return ALCreateVBString( (char _far *) ( (ALWinMemory *) this_object )->mpcUserBuffer,
                             (unsigned short int) ( (ALWinMemory *) this_object )->GetSize() );
}

#endif

