//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/MEMSTORE.CPv   1.0   12 Mar 1998 12:17:00   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/MEMSTORE.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:00   DCHI
// Initial revision.
// 
//************************************************************************

//
// MEMSTORE.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALMemoryBase::operator new()
//  ALMemoryBase::ALMemoryBase()
//  ALMemoryBase::~ALMemoryBase()
//  ALMemoryBase::LoadBuffer()
//  ALMemoryBase::Rename()
//  ALMemoryBase::RenameToBackup()
//  ALMemoryBase::UnRename()
//  ALMemoryBase::Seek()
//  ALMemoryBase::FlushBuffer()
//  ALMemoryBase::Close()
//  ALMemoryBase::Create()
//  ALMemoryBase::Open()
//
// DESCRIPTION
//
//  This file contains all the C++ member functions to support the
//  ALMemoryBase class.  ALMemoryBase doesn't do anything on its
//  own.  It instead provides a framework for the three derived
//  memory classes: ALWinMemory, ALMemory, and ALHugeMemory.
//
//  The only thing those classes do that ALMemory can't do is to
//  allocate, free, and copy memory.  Since each of three classes
//  has their own pointer type, that has to be viewed here as an
//  abstraction.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "memstore.h"

//
// Need this for malloc()
//
#include <stdlib.h>

//
// NAME
//
//  ALMemoryBase::operator new()
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
//  void * ALMemoryBase::operator new( size_t size )
//
// C SYNOPSIS
//
//  None, this is an internal C++ function.
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
//           necessary to construct a new ALMemoryBase object.
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
//  Very important:  this new operator is called to allocate the
//  storage for the ALMemoryBase object itself.  This has nothing to do
//  with the storage buffer that the memory object will be using
//  later on.  In other words, this new operator is responsible for
//  no more than a couple of dozen bytes, not potentially hundreds
//  of Kbytes.
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
ALMemoryBase::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALMemoryBase::ALMemoryBase()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The base class constructor for ALMemory objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  ALMemoryBase::ALMemoryBase( const char *buffer_name = "",
//                              ALCase name_case = AL_MIXED );
//
// C SYNOPSIS
//
//  None, this is an internal class.
//
// VB SYNOPSIS
//
//  None, this is an internal class.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal class.
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
//  name_case    : This decides whether you want the file name to be
//                 case sensitive when making comparisons.  MS-DOS
//                 file names are case-insensitive.  You can make memory
//                 buffers either mixed case, forced upper, or forced
//                 lower.  The default of mixed case means that comparisons
//                 will be case sensitive, which is fine.
//
//
// DESCRIPTION
//
//  There are three different memory base storage objects in ArchiveLib
//  2.0: ALMemory, ALWinMemory, and ALHugeMemory.  The only substantial
//  difference between these three is in the pointer type they use to
//  point to their memory buffer.  So they can share a lotta code.  That
//  code is all combined into a single base class, ALMemoryBase.  The
//  base class code has to rely on a few virtual functions to take over
//  when pointer manipulation is needed.
//
//  This constructor doesn't do anything except pass the appropriate
//  initialization information on to the base class constructor.
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
ALMemoryBase::ALMemoryBase(  /* Tag public function */
    const char AL_DLL_FAR *buffer_name /* = "" */,
    ALCase name_case /* = AL_MIXED */ )
        : ALStorage( buffer_name, 4096, AL_MEMORY_OBJECT, name_case )
{
}

//
// NAME
//
//  ALMemoryBase::~ALMemoryBase()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The destructor for the base memory storage class.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  ALMemoryBase::~ALMemoryBase();
//
// C SYNOPSIS
//
//  None, destroy using base class function deleteALStorage().
//
// VB SYNOPSIS
//
//  None, destroy using base class function deleteALStorage().
//
// DELPHI SYNOPSIS
//
//  None, destroy using base class function deleteALStorage().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The most important thing memory objects have to do in their destructor
//  is free up the big buffer that underlies the object.  This has to be
//  handled by the derived classes, since only they know how to handle
//  specific pointer types.  That leaves this destructor with not much
//  to do!
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
ALMemoryBase::~ALMemoryBase()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALMemoryBase: attempting to delete invalid object" );
}

//
// NAME
//
//  ALMemoryBase::LoadBuffer();
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Load a new batch of data from the big buffer into the I/O buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::LoadBuffer( long address );
//
// C SYNOPSIS
//
//  None, this is a protected C++ internal function.
//
// VB SYNOPSIS
//
//  None, this is a protected C++ internal function.
//
// DELPHI SYNOPSIS
//
//  None, this is a protected C++ internal function.
//
// ARGUMENTS
//
//  address      :  The desired offset in the storage object to load from.
//                  This is used to calculate a simple offset into the
//                  big buffer.
//
// DESCRIPTION
//
//  This is a virtual function the ALStorage functions rely on when reading
//  data.  Anytime someone tries to do a ReadChar() or ReadBuffer(), and
//  it turns out that the I/O buffer has been exhausted, this function
//  is called.
//
//  The simple job of this function is to read as many bytes as possible out
//  of the giant memory block allocated for the derived class object, and stick
//  it into the I/O buffer, which caches it for calls to ReadChar()
//  and friends.
//
//  This works fine unless you try to go past the end of the buffer,
//  since there is nothing there we flag that as an error.
//
//  This function takes care of lots of member data maintenance, but the
//  one thing it won't do is actually load the data from the big buffer.
//  It has to use the virtual function _LoadBuffer() in the derived class
//  to to actually move data out of the big buffer and into the I/O buffer.
//
// RETURNS
//
//  AL_SEEK_ERROR if we try to read past the end of file.  AL_END_OF_FILE
//  if we just run out of data.  Otherwise an int indicating how many bytes
//  are now in the buffer.
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
ALMemoryBase::LoadBuffer( long address )  /* Tag protected function */
{
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mlFilePointer != address ) {
        if ( mlFilePointer > mlUserBufferSize )
            return mStatus.SetError( AL_SEEK_ERROR,
                                     "Attempt to read past end of the "
                                     "buffer in ALMemoryBase %s",
                                     mName.GetName() );
    }
    long load = mlUserBufferSize - address;
    if ( load > (long) muBufferSize )
        muBufferValidData = muBufferSize;
    else
        muBufferValidData = (size_t) load;
    if ( muBufferValidData <= 0 ) {
        muReadIndex = 0;
        return AL_END_OF_FILE;
    }
    _LoadBuffer( address );
    if ( miUpdateCrcFlag )
        UpdateCrc( muBufferValidData );
    muReadIndex = 0; //Reading can resume at this location
    mlFilePointer += muBufferValidData;
    YieldTime();
    return muBufferValidData;
}

//
// NAME
//
//  ALMemoryBase::Rename()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Rename a memory storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::Rename( const char *new_name = 0,
//                            int delete_on_clash = 1 );
//
// C SYNOPSIS
//
//  Use the base class function ALStorageRename().
//
// VB SYNOPSIS
//
//  Use the base class function ALStorageRename().
//
// DELPHI SYNOPSIS
//
//  Use the base class function ALStorageRename().
//
// ARGUMENTS
//
//  new_name        : The new name of the buffer.
//
//  delete_on_clash : This argument makes sense with files.  What it says
//                    is that if you try to rename BOB.DAT to BOB.BAK,
//                    and it turns out that there is another BOB.BAK, should
//                    you delete the other one?  With memory buffers,
//                    there is no clash, cause the OS doesn't care about
//                    unique names.  So we ignore this parm.
//
// DESCRIPTION
//
//  This function is supposed to rename the underlying physical object.
//  But in the case of memory buffers, the underlying physical object
//  doesn't actually have a name, so this is really just a local rename.
//
// RETURNS
//
//  Always returns AL_SUCCESS.
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
ALMemoryBase::Rename( const char AL_DLL_FAR *new_name /* = 0 */,  /* Tag public function */
                      int /* delete_on_clash = 1 */ )
{
    if ( new_name )
        mName = new_name;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemoryBase::RenameToBackup()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Renames the storage object to a backup name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::RenameToBackup( int delete_on_clash /* = 1 */ )
//
// C SYNOPSIS
//
//  C programs should use the base class function ALStorageRenameToBackup().
//
// VB SYNOPSIS
//
//  VB programs should use the base class function ALStorageRenameToBackup().
//
// DELPHI SYNOPSIS
//
//  Delphi programs should use the base class function
//  ALStorageRenameToBackup().
//
// ARGUMENTS
//
//  delete_on_clash : This argument makes sense with files.  What it says
//                    is that if you try to rename BOB.DAT to BOB.BAK,
//                    and it turns out that there is another BOB.BAK, should
//                    you delete the other one?  With memory buffers,
//                    there is no clash, cause the OS doesn't care about
//                    unique names.  So it doesn't matter what value you
//                    pass to the Rename() function, it is going to be
//                    ignored.
//
// DESCRIPTION
//
//  This function renames an object.  But instead of making you sweat in
//  order to come up with a new name, it just uses the default name
//  that we use to assign a backup name.
//
// RETURNS
//
//  Always returns AL_SUCCESS.
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
ALMemoryBase::RenameToBackup( int delete_on_clash /* = 1 */ )  /* Tag public function */
{
    mName.ChangeExtension();
    return Rename( 0, delete_on_clash );
}

//
// NAME
//
//  ALMemoryBase::UnRename()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Restore the old name of the object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::UnRename( int delete_on_clash = 1 );
//
// C SYNOPSIS
//
//  C programs use the base class function ALStorageUnRename().
//
// VB SYNOPSIS
//
//  VB programs use the base class function ALStorageUnRename().
//
// DELPHI SYNOPSIS
//
//  Delphi programs use the base class function ALStorageUnRename().
//
// ARGUMENTS
//
//  delete_on_clash : This argument makes sense with files.  What it says
//                    is that if you try to rename BOB.DAT to BOB.BAK,
//                    and it turns out that there is another BOB.BAK, should
//                    you delete the other one?  With memory buffers,
//                    there is no clash, cause the OS doesn't care about
//                    unique names.  So we just ignore it here.
//
//
// DESCRIPTION
//
//  If you decide you didn't really want to rename an object after all, you
//  can call this function to get the old name back!  It takes advantage of
//  the fact that ALName objects always remember their old name after getting
//  a new one.
//
// RETURNS
//
//  Always returns AL_SUCCESS.
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
ALMemoryBase::UnRename( int /* delete_on_clash = 1 */)  /* Tag public function */
{
    AL_ASSERT( mName.GetName() != 0, "UnRename: trying to rename with a null name" );
    AL_ASSERT( mName.GetOldName() != 0, "UnRename: trying to rename with a null old name" );
    AL_ASSERT( strlen( mName ) > 0, "UnRename: trying to rename with a zero length name"  );
    AL_ASSERT( strlen( mName.GetOldName() ) > 0, "UnRename: trying to rename with a zero length old name" );

    ALStorage::mName = mName.GetOldName();
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemoryBase::Seek()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Seek to a specified offset in the big memory buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::Seek( long address );
//
// C SYNOPSIS
//
//  C programs should use the base class functin ALStorageSeek().
//
// VB SYNOPSIS
//
//  VB programs should use the base class functin ALStorageSeek().
//
// DELPHI SYNOPSIS
//
//  Delphi programs should use the base class functin ALStorageSeek().
//
// ARGUMENTS
//
//  address  :  The address in the memory object to go to.  The read and
//              write pointers will now be repositioned to this point.
//
// DESCRIPTION
//
//  This function acts just like the seek() function in the C runtime
//  library.  It flushes the current I/O buffers, and then moves the file
//  read and write pointers to a new spot, specified by the address.  if
//  there is no memory there, you will get an error.  Note that this
//  makes it not quite like the C run time library, since it the C RTL can
//  create new storage space with a seek().  But I don't think we need that
//  capability yet.
//
// RETURNS
//
//  AL_SEEK_ERROR if we can't get to that point in the buffer.  Otherwise
//  AL_SUCCESS.
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
ALMemoryBase::Seek( long address )  /* Tag public function */
{
    FlushBuffer();
    if ( mStatus < 0 )
        return mStatus;

    if ( mlFilePointer != address ) {
        if ( mlFilePointer > mlUserBufferSize )
            return mStatus.SetError( AL_SEEK_ERROR,
                                     "Attempt to read past end of the "
                                     "buffer in ALMemoryBase %s",
                                     mName.GetName() );
    }
    mlFilePointer = address;
    return AL_SUCCESS;
}


//
// NAME
//
//  ALMemoryBase::FlushBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Flush the I/O buffer of any newly written data.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::FlushBuffer()
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
//  None.
//
// DESCRIPTION
//
//  This routine is called when the I/O buffer is filled up. It means
//  you have filled up the cache with what is usually 4K bytes of data.
//  This routine is also called if you have hot data in the I/O buffer
//  and you decide to do a seek(), or a read().
//
//  All we have to do here is take the hot data in the I/O buffer and
//  write it out to our big memory object.  Since the base class doesn't
//  know anything about the pointer types used in derive classes, that
//  piece of work is done in a virtual function callled _FlushBuffer().
//
//
// RETURNS
//
//  AL_CANT_ALLOCATE_MEMORY, if we run out.  Otherwise, AL_SUCCESS.
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
ALMemoryBase::FlushBuffer()  /* Tag protected function */
{
    if ( mStatus < 0 )
        return mStatus;
//
// If the write index is 0, we can skip all this stuff, because there
// is nothing in the buffer to flush out.
//
    if ( muWriteIndex != 0 ) {
        if ( miUpdateCrcFlag )
            UpdateCrc( muWriteIndex );
        if ( (long) ( muWriteIndex + mlFilePointer ) > mlUserBufferSize )
            if ( GrowUserBuffer( muWriteIndex + mlFilePointer ) < 0 )
                return mStatus;
        _FlushBuffer( mlFilePointer );
        mlFilePointer += muWriteIndex;
        muWriteIndex = 0;
        if ( mlSize < mlFilePointer )
            mlSize = mlFilePointer;
    }
    muReadIndex = 0;
    muBufferValidData = 0;
    YieldTime();
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemoryBase::Close()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Close the memory object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::Close();
//
// C SYNOPSIS
//
//  C programs must use the base class function, ALStorageClose().
//
// VB SYNOPSIS
//
//  VB programs must use the base class function, ALStorageClose().
//
// DELPHI SYNOPSIS
//
//  Delphi programs must use the base class function, ALStorageClose().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  Close() is supposed to do the same thing as fclose() in the run
//  time library.  The most important thing we are concerned about is
//  that the I/O buffer gets freed up by the base class, so this suddenly
//  might not be a giant heavyweight object any more.
//
//  Note that the virtual Close() function is called in the base class,
//  which does significant work in addition to what you see here.
//
// RETURNS
//
//  AL_SUCCESS, or various error codes that filter up or down from other
//  routines.
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
ALMemoryBase::Close()  /* Tag public function */
{
    if ( mpcBuffer == 0 )
        return mStatus;
    FlushBuffer();
    ALStorage::Close();
    return mStatus;
}

//
// NAME
//
//  ALMemoryBase::Create()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a new memory storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::Create( long size )
//
// C SYNOPSIS
//
//  None, C programs should use ALStorageCreate().
//
// VB SYNOPSIS
//
//  None, VB programs should use ALStorageCreate().
//
// DELPHI SYNOPSIS
//
//  None, Delphi programs should use ALStorageCreate().
//
// ARGUMENTS
//
//  size  :  This is the desired size of the memory object.  This parameter
//           is important to the derived class, which will use this argument
//           to initialize a buffer.  At this level, we don't care, and
//           therefore we don't use it.
//
// DESCRIPTION
//
//  This is like creating a new file.  If there isn't a memory buffer
//  already assigned to this object, we create one, with an initial
//  allocation of 16Kbytes.  Note that this class will never be constructed
//  on its own, it is safe to assume that some additional work is being
//  passed off to the derived class.
//
//  Note that the virtual Create() function is called in the base class,
//  which does significant work in addition to what you see here.
//
// RETURNS
//
//  AL_SUCCESS, AL_CANT_ALLOCATE_MEMORY, or various error codes that
//  filter on down from other routines.
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
ALMemoryBase::Create( long )  /* Tag public function */
{
    ALStorage::Create();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( (char AL_DLL_FAR *) mName == 0 || strlen( mName ) == 0 )
        mName = "AL.TMP";
    if ( mfUserOwnsBuffer )
        return AL_SUCCESS;  //If the user supplied the buffer, we take what's available
    return AL_SUCCESS;
}

//
// NAME
//
//  ALMemoryBase::Open()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Open a memory storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBase::Open();
//
// C SYNOPSIS
//
//  None, C programs should use the base class function ALStorageOpen().
//
// VB SYNOPSIS
//
//  None, VB programs should use the base class function ALStorageOpen().
//
// DELPHI SYNOPSIS
//
//  None, Delphi programs should use the base class function ALStorageOpen().
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
//  Note that the virtual Open() function is called in the base class,
//  which does significant work in addition to what you see here.
//
// RETURNS
//
//  AL_SUCCESS, AL_CANT_OPEN_FILE, or various error codes that
//  filter on down from other routines.
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
ALMemoryBase::Open()  /* Tag public function */
{
    ALStorage::Open();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    else
        return AL_SUCCESS;
}


