//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/FILESTOR.CPv   1.0   12 Mar 1998 12:16:14   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/FILESTOR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:14   DCHI
// Initial revision.
// 
//************************************************************************

//
// FILESTOR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALFile::operator new()
//  ALFile::ALFile()
//  newALFile()
//  ALFile::~ALFile()
//  ALFile::LoadBuffer()
//  ALFile::FlushBuffer()
//  ALFile::Seek()
//  ALFile::Open()
//  ALFile::MakeTempName()
//  ALFile::Create()
//  ALFile::Close()
//  ALFile::RenameToBackup()
//  ALFile::Rename()
//  ALFile::UnRename()
//  ALFile::Delete()
//  ALFile::Clone()
//  ALFile::MakeDirectory()
//
// DESCRIPTION
//
//  This file contains the C++ member functions to support class
//  ALFile.  This class works very closely with the parent class,
//  ALStorage, found in STORAGE.CPP.  You will find in many cases
//  the virtual functions found here in the derived class call
//  the same function in the parent class to help out with some
//  of the work.
//  We don't really do anything exciting in the WEP, it is just
//
// REVISION HISTORY
//
//  May 22, 1994  1.0A  : First release
//
//  July 7, 1994  1.0B  : Had to make a lot of changes to support file
//                        management under UNIX.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#if defined( AL_OS2 )
  #define INCL_DOSFILEMGR
  #include <os2.h>
  #include <io.h>
  #if defined( AL_BORLAND )
    #include <dir.h>
  #else
    #include <direct.h>
  #endif
#else
  #include <io.h>
  #include <dos.h>
  #include <direct.h>
#endif

#include "filestor.h"

//
// NAME
//
//  ALFile::operator new()
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
//  #include "filestor.h"
//
//  void * ALArchive::operator new( size_t size )
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
//           necessary to construct a new ALFile object.
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
//  February 14, 1996  2.0A : New release
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALFile::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALFile::ALFile()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The ALFile constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  ALFile::ALFile( const char *file_name = "",
//                  int buffer_size = 4096,
//                  ALCase name_case = AL_LOWER );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  hALStorage newALFile( char *file_name );
//
// VB SYNOPSIS
//
//  Declare Function newALFile Lib "AL20LW" (ByVal file_name$) As Long
//
// DELPHI SYNOPSIS
//
//  function newALFile( file_name : PChar ) : hALStorage;
//
// ARGUMENTS
//
//  file_name    : The initial file name of the ALFile object you are
//                 creating.  The C++ version has a default argument,
//                 which is a blank string, which will get converted to a
//                 temporary name before opening the actual disk file.
//
//  buffer_size  : The size of the object's I/O buffer.  The default of 4096
//                 should give very good performance.  Note that C/VB/Delphi
//                 users don't get any choice about the buffer size!  This
//                 was done strictly in order to cut back on confusion.
//
//  name_case    : This parameter determines whether the file names will
//                 always be converted to upper case, lower case, or left
//                 in mixed case.  Under MS-DOS, you shouldn't use mixed
//                 case, because the O/S file naming convention is case
//                 insensitive.  ArchiveLib will think "TEMP.BAK" and
//                 "temp.bak" are different, when they really aren't.
//
//                 Once again, C/VB/Delphi users are stuck with the default.
//
//
// DESCRIPTION
//
//  This constructor is used to create a new ALFile object, which will
//  usually be treated as an ALStorage object by ArchiveLib functions.
//  It is important to note that not much happens during construction of
//  this object, the real activity happens after you call the Open()
//  function.  Just creating this object *does not* create a file on disk!
//
// RETURNS
//
//  Nothing, this is a constructor.  The C/VB/Delphi translation routines
//  return a handle pointing to the newly created ALFile object.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::ALStorage()
//
// REVISION HISTORY
//
//  February 14, 1996  2.0A : New release
//

AL_PROTO
ALFile::ALFile( const char AL_DLL_FAR *file_name /* = "" */,  /* Tag public function */
                         int buffer_size /* = 4096 */,
                         ALCase name_case /* = AL_LOWER */)
// Note: if non-msdos, change case parameter to AL_MIXED
    : ALStorage( file_name, buffer_size, AL_FILE_OBJECT, name_case ) {
    miHandle = -1;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALStorage AL_FUNCTION
newALFile( char AL_DLL_FAR *file_name )  /* Tag public function */
{
    return (hALStorage) new ALFile( file_name );
}

#endif

//
// NAME
//
//  ALFile::~ALFile()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALFile destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  ALFile::~ALFile()
//
// C SYNOPSIS
//
//  None, use deleteALStorage();
//
// VB SYNOPSIS
//
//  None, use deleteALStorage();
//
// DELPHI SYNOPSIS
//
//  None, use deleteALStorage();
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor for an ALFile object doesn't have to do much work.
//  The base class destructor will take care of freeing the I/O buffer,
//  and any other loose ends.  All we have to do here is make sure
//  the file gets closed, and that its buffers get flushed to the disk
//  file.
//
//  Note that in debug mode, the destructor also checks this for the
//  correct class type.  This helps flag erroneous or duplicated
//  destructor calls.
//
//  C/VB/Delphi users don't have a specific function to call this guy.
//  Instead, the call deleteALStorage(), which will in turn call the
//  virtual destructor.  If it's an ALFile object, it will get here
//  one way or another.
//
// RETURNS
//
//  Nothing, it's a dtor!
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::~ALStorage(), ALFile::ALFile()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

AL_PROTO
ALFile::~ALFile()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALFile: attempting to delete invalid object" );
    if ( miHandle != -1 )
        Close();
}

//
// NAME
//
//  ALFile::LoadBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Load a new batch of data from the file into the local buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::LoadBuffer( long address )
//
// C SYNOPSIS
//
//  None, use ALStorageLoadBuffer().
//
// VB SYNOPSIS
//
//  None, use ALStorageLoadBuffer().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageLoadBuffer().
//
// ARGUMENTS
//
//  address  : The long offset into the physical file.  A seek/read
//             combination will be executed at this location,
//             so that subsequent calls to read data will start at
//             the given address.
//
// DESCRIPTION
//
//  This function is used in the library whenever a byte needs to be read
//  that isn't present in the current I/O buffer.  It has to use
//  the C RTL function lseek() to go to the correct position in the library.
//  If that works, it uses the C RTL function read() to read in an I/O
//  buffer full of data.
//
//  After that operation is performed, muReadIndex is set to 0, indicating
//  that the next read from the I/O buffer will take place at location 0.
//  mlFilePointer is set to 'address' plus the number of bytes read, so
//  we know where the next read from the file will take place.  And
//  muBufferValidData is set to the count of bytes read in from this
//  location.  That lets us know how far we can read in the I/O buffer
//  before we run out of space.
//
//  Note that if CRC checking has been turned on, we will update the
//  current working CRC value with the new data that has been read
//  in from the buffer.
//
// RETURNS
//
//  AL_SUCCESS, AL_SEEK_ERROR, AL_END_OF_FILE, or possibly another
//  error code < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::LoadBuffer(), ALFile::FlushBuffer(), ALStorage::ReadChar()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALFile::LoadBuffer( long address )  /* Tag public function */
{
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    if ( mlFilePointer != address ) {
        long result = lseek( miHandle, address, SEEK_SET );
        if ( result == -1L )
            return mStatus.SetError( AL_SEEK_ERROR,
                                     "Seek failure on %s.  errno = %d",
                                     mName.GetName(),
                                     errno );
    }
    int result = read( miHandle, mpcBuffer, muBufferSize );
    if ( result == 0 )
        return AL_END_OF_FILE;
    if ( result < 0 )
        return mStatus.SetError( AL_READ_ERROR,
                                   "Read failure on %s.  errno = %d",
                                   mName.GetName(),
                                   errno );
    if ( miUpdateCrcFlag )
        UpdateCrc( result );
    muReadIndex = 0; //Reading can resume at this location in the I/O buffer
    mlFilePointer += result;
    muBufferValidData = result;
    YieldTime();
    return result;
}

//
// NAME
//
//  ALFile::FlushBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Flush the internal read/write buffer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::FlushBuffer()
//
// C SYNOPSIS
//
//  None, use ALStorageFlushBuffer().
//
// VB SYNOPSIS
//
//  None, use ALStorageFlushBuffer().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageFlushBuffer();
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is the counterpart to LoadBuffer(). It gets called
//  when a write operation is poised to overflow the I/O buffer.  This
//  means we need to flush the buffer out to disk, then reset some
//  data members.
//
//  Unlike LoadBuffer(), this function doesn't have an address argument,
//  so we don't have to perform a seek().  Instead, the data will be
//  written out to the current position of the file pointer.  If the
//  write is successful, muWriteIndex is set to 0, indicating that the
//  next write to the I/O buffer can go to position 0.  mlFilePointer is
//  incremented by the length of the write, so we know where the next read
//  or write will occur.  Finally, muBufferValidData is set to 0, indicating
//  that there is no data in the I/O buffer that has been written, and
//  there is no data that can be read.
//
//  C/VB/Delphi programmers don't have a direct translation function for
//  this call.  Instead, the call ALStorageLoadBuffer(), which in turn
//  will use the virtual function calling mechanism to come here.  At least
//  if 'this' is an ALFile object, that is.
//
// RETURNS
//
//  An integer status value, AL_SUCCESS, AL_WRITE_ERROR, or possibly some
//  status code < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::FlushBuffer(), ALFile::LoadBuffer()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALFile::FlushBuffer()  /* Tag public function */
{
    if ( mStatus < 0 )
        return mStatus;
    if ( muWriteIndex != 0 ) {
        if ( miUpdateCrcFlag )
            UpdateCrc( muWriteIndex );
        int result = write( miHandle, mpcBuffer, muWriteIndex );
        if ( (unsigned int) result != muWriteIndex )
            return mStatus.SetError( AL_WRITE_ERROR,
                                     "Write failure on %s.  "
                                     "result = %d, errno = %d",
                                     mName.GetName(),
                                     result,
                                     errno );
        muWriteIndex = 0;
        mlFilePointer += result;
    }
    muReadIndex = 0;
    muBufferValidData = 0;
    YieldTime();
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::Seek()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Seek to a given position in an input file.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::Seek( long address )
//
// C SYNOPSIS
//
//  None, use ALStorageSeek().
//
// VB SYNOPSIS
//
//  None, use ALStorageSeek().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageSeek().
//
// ARGUMENTS
//
//  address  :  The address in the physical file to seek to.
//
// DESCRIPTION
//
//  This is a function the user can call to position the read/write pointer
//  to a new location in the disk file.  If there is any data that has been
//  written to the I/O buffer, it gets flushed first.  After that, we do
//  a seek, and update mlFilePointer to reflect the new reality.  Note that
//  the other important data members will have been updated by FlushBuffer().
//
//  And no, this guy doesn't do a LoadBuffer().  Which is fine if you are
//  going to do a bunch of writes afterwards.  If you are going to read data
//  immediately after Seek(), you would have been better of calling
//  LoadBuffer().
//
//  C/VB/Delphi programmers don't have a direct translation function for
//  this call.  Instead, they should call ALStorageSeek(), which in turn
//  will use the virtual function calling mechanism to come here.  At least
//  if 'this' is an ALFile object, that is.
//
// RETURNS
//
//  AL_SUCCESS, AL_SEEK_ERROR, or possibly some other status code < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Seek(), ALFile::Tell()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALFile::Seek( long address )  /* Tag public function */
{
    FlushBuffer();
    if ( mStatus < 0 )
        return mStatus;
    if ( mlFilePointer != address ) {
        long result = lseek( miHandle, address, SEEK_SET );
        if ( result == -1L )
            return mStatus.SetError( AL_SEEK_ERROR,
                                     "Seek failure on %s.  errno = %d",
                                     mName.GetName(),
                                     errno );
    }
    mlFilePointer = address;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::Open()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Open an ALFile object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::Open()
//
// C SYNOPSIS
//
//  None, use ALStorageOpen().
//
// VB SYNOPSIS
//
//  None, use ALStorageOpen().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageOpen().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This is an important function, because it converts the ALFile
//  object from a dinky little unimportant object, to a big massive
//  thing that is ready to do serious work.
//
//  The first thing we do here is see if we can open the file.  We try
//  to open it with READ/WRITE privileges, but we give up and drop back
//  to READ only if that doesn't work out.
//
//  We then call the base class ALStorage::Open() who takes care of
//  allocating buffers and initializing data members.
//
//  Finally, we have to get the protection attributes and time date
//  stamps for the file.  After those are stored off, the file is ready
//  for abuse.
//
//  C/VB/Delphi programmers don't have a direct translation function for
//  this call.  Instead, they should call ALStorageOpen(), which in turn
//  will use the virtual function calling mechanism to come here.  At least
//  if 'this' is an ALFile object, that is.
//
// RETURNS
//
//  AL_CANT_OPEN_FILE, AL_SUCCESS, or possibly some other error code
//  < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALFile::Create(), ALStorage::Open()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALFile::Open()  /* Tag public function */
{
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    miHandle = open( mName, O_BINARY | O_RDWR );

    if ( miHandle == -1 && errno == EACCES )
        miHandle = open( mName, O_BINARY | O_RDONLY );
    if ( miHandle == -1 )
        return mStatus.SetError( AL_CANT_OPEN_FILE,
                                 "File open failure.  Open of %s returned "
                                 "errno = %d",
                                 mName.GetName(),
                                 errno );

    ALStorage::Open();
    struct stat buf;
    struct tm AL_DLL_FAR *tblock;
    if ( stat( mName, &buf ) == -1 )
        return mStatus.SetError( AL_CANT_OPEN_FILE,
                                 "Couldn't get time, date, and size "
                                 "information for %s.  errno = %d.",
                                 mName.GetName(),
                                 errno );
    mlSize = buf.st_size;
    tblock = localtime( &buf.st_mtime );
    mTimeDate.SetTimeDate( tblock );
#if defined( AL_WINDOWS ) && defined( AL_FLAT_MODEL )
    TIME_ZONE_INFORMATION tzi;
    GetTimeZoneInformation( &tzi );
    long result = mTimeDate.GetUnixTime();
    result -= tzi.Bias * 60;
    mTimeDate.SetTimeDate( result );
#endif
    ReadAttributesFromFileSys();
    if ( mStatus < 0 )
        return mStatus;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::MakeTempName( int i )
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Private function to make a temporary filename.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  void ALFile::MakeTempName( int i )
//
// C SYNOPSIS
//
//  None, this is an internal C++ member function.
//
// VB SYNOPSIS
//
//  None, this is an internal C++ member function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal C++ member function.
//
// ARGUMENTS
//
//  i    : A numeric argument that can somehow be incorporated into
//         the temporary file name.  Create() will call this function
//         while incrementing this number in an attempt to find a unique
//         name.
//
// DESCRIPTION
//
//  This function is called by Create() and other functions when they
//  decide they need to cook up a temporary file name.  The single parameter
//  i is incremented by the calling program so that repeated calls should
//  eventually produce a unique name.
//
//  All this function does to create that unique name is perform a sprintf()
//  into a buffer using a simple template.  The result is copied into the
//  mName member, and is ready to be tried out.
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
//  ALFile::Create()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

void AL_PROTO
ALFile::MakeTempName( int i )  /* Tag protected function */
{
    char name[ 21 ];

    sprintf( name, "~al~%03d.tmp", i );
    mName = name;
}

//
// NAME
//
//  ALFile::Create()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a new physical file using an ALFile object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::Create( long )
//
// C SYNOPSIS
//
//  None, use ALStorageCreate().
//
// VB SYNOPSIS
//
//  None, use ALStorageCreate().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageCreate().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is used to create a new file storage object.  Since
//  we are creating a new object, we must be able to open it with read
//  access.  We will also rudely obliterate any existing file.
//
//  The first thing we do here is call the base class Create() function.
//  It takes care of setting up the I/O buffer and initializing the
//  data members used to support the class.
//
//  Since this is a virtual function in a derived class, C/VB/Delphi
//  programmers aren't going to be able to call it directly.  They
//  will instead call ALStorageCreate(), which will wind up here
//  via the virtual function mechanism.
//
// RETURNS
//
//  AL_SUCCESS, AL_CANT_OPEN_FILE, or possibly some other error code
//  < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Create(), ALFile::Open()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALFile::Create( long )  /* Tag public function */
{
    ALStorage::Create();
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    char AL_DLL_FAR *p = mName;
    int i;
//
// We don't try to make a directory for unnamed temp files.
//
    if ( p == 0 || p[ 0 ] == 0 ) {
        for ( i = 0 ; i < 999 ; i++ ) {
            MakeTempName( i );
            miHandle = open( mName,
                             O_CREAT | O_RDWR | O_BINARY | O_EXCL,
                             S_IREAD | S_IWRITE );
            if ( miHandle != -1 )
                break;
            else if ( errno != EEXIST && errno != EACCES ) {
                mStatus.SetError( AL_CANT_OPEN_FILE,
                                  "Temporary file creation failure.  "
                                  "Open of %s returned errno = %d",
                                  mName.GetName(),
                                  errno );
                mName = "";
                return AL_CANT_OPEN_FILE;
            }
        }
        if ( i == 1000 ) {
            mStatus.SetError( AL_CANT_OPEN_FILE,
                              "Temporary file creation failure.  "
                              "Tried 1000 times to open %s "
                              "(or a name something like that).",
                              mName.GetName() );
            mName = "";
            return AL_CANT_OPEN_FILE;
        }
    } else {
        miHandle = open( mName,
                         O_CREAT | O_RDWR | O_BINARY | O_TRUNC,
                         S_IREAD | S_IWRITE );
//
// Handle failure to make directories here
//
        if ( miHandle == -1 &&
            errno == ENOENT &&
            ( strchr( mName, '/' ) || strchr( mName, '\\' ) ) ) {
            if ( MakeDirectory() ) {
                miHandle = open( mName,
                                 O_CREAT | O_RDWR | O_BINARY | O_TRUNC,
                                 S_IREAD | S_IWRITE );
            }
        }
    }
    if ( miHandle == -1 && mAttributes.Directory() == 0 )
        return mStatus.SetError( AL_CANT_OPEN_FILE,
                                 "File creation failure.  "
                                 "Open of %s returned errno = %d",
                                 mName.GetName(),
                                 errno );
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::Close()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Close the physical file associated with an ALFile object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::Close()
//
// C SYNOPSIS
//
//  None, use ALStorageClose().
//
// VB SYNOPSIS
//
//  None, use ALStorageClose().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageClose().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is called when you are done accessing a file, and want
//  to free up its resources.  The first thing it does is check to see
//  if the file was ever actually opened.  If it was, we flush the output
//  buffer, then calculate and store the file length.  Finally, we close
//  the disk file, then call the base class Close() function to clean up
//  the buffers and deal with other miscellaneous dirty work.
//
//  Since this is a virtual function in a derived class, C/VB/Delphi
//  programmers aren't going to be able to call it directly.  They
//  will instead call ALStorageClose(), which will wind up here
//  via the virtual function mechanism.
//
// RETURNS
//
//  Any status code, hopefully AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Close(), ALFile::Create(), ALFile::Open()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

//
// Note:  eventually I need to move the O/S specific code here into
//        cpp_fs.nt, cpp_fs.os2, and cpp_fs.dos.
//

int AL_PROTO
ALFile::Close()  /* Tag public function */
{
    if ( miHandle == -1 )
        return mStatus;
    FlushBuffer();
    mlSize = filelength( miHandle );
    if ( miCreated && mTimeDate.Valid() ) {
#if defined( AL_WIN32 )
        TIME_ZONE_INFORMATION tzi;
        GetTimeZoneInformation( &tzi );
        long result = mTimeDate.GetUnixTime();
        result += tzi.Bias * 60;
        mTimeDate.SetTimeDate( result );
        FILETIME FileTime;
        LPFILETIME lpFileTime = &FileTime;
        WORD wDosDate = (WORD) mTimeDate.GetDosDate();
        WORD wDosTime = (WORD) mTimeDate.GetDosTime();
        DosDateTimeToFileTime( wDosDate, wDosTime, lpFileTime );
#  if defined( AL_WATCOM )
        SetFileTime( (HANDLE) _os_handle( miHandle ), NULL, NULL, lpFileTime );
#  else
        SetFileTime( (HANDLE) _get_osfhandle( miHandle ), NULL, NULL, lpFileTime );
#  endif
#elif defined( AL_OS2 )
// going to do it all after the file is closed
#else
        _dos_setftime( miHandle,
                       mTimeDate.GetDosDate(),
                       mTimeDate.GetDosTime() );
#endif
    }
    close( miHandle );
    miHandle = -1;
    ALStorage::Close();
    if ( miCreated && mTimeDate.Valid() )
        WriteAttributesToFileSys();
    return mStatus;
}

//
// NAME
//
//  ALFile::RenameToBackup()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Rename the physical file on disk to a suitable backup name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::RenameToBackup( int delete_on_clash = 1 );
//
// C SYNOPSIS
//
//  None, use ALStorageRenameToBackup().
//
// VB SYNOPSIS
//
//  None, use ALStorageRenameToBackup().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageRenameToBackup().
//
// ARGUMENTS
//
//  delete_on_clash  : If this flag is set, it means that we will overwrite
//                     an existing file with this file if the names clash.
//                     For example, if I am renaming TEMP.DAT to TEMP.BAK,
//                     and a TEMP.BAK already exists, I will delete it
//                     before renaming if this arg is set.
//
// DESCRIPTION
//
//  This function is a quick way to rename a storage object. The new
//  name created is the default name, which usually means changing the
//  file extension to ".BAK", from whatever it was.
//
//  You don't see it here, but both the mName member and the physical file
//  name are updated.  That all happens in the Rename() function.
//
//  Since this is a virtual function in a derived class, C/VB/Delphi
//  programmers aren't going to be able to call it directly.  They
//  will instead call ALStorageRenameToBackup(), which will wind up here
//  via the virtual function mechanism.
//
// RETURNS
//
//  AL_SUCCESS or AL_RENAME_ERROR.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALFile::Rename(), ALStorage::RenameToBackup()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALFile::RenameToBackup( int delete_on_clash /* = 1 */ )  /* Tag public function */
{
    mName.ChangeExtension();
    return Rename( 0, delete_on_clash );
}

//
// NAME
//
//  ALFile::Rename()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Rename an existing file on disk.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::Rename( const char *new_name = 0,
//                      int delete_on_clash = 1 );
//
// C SYNOPSIS
//
//  None, use ALStorageRename().
//
// VB SYNOPSIS
//
//  None, use ALStorageRename().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageRename().
//
// ARGUMENTS
//
//  new_name         : A character pointer to a new file name.  If a name is
//                     defined here, the file is renamed to this new value.
//                     If this value is 0, it means that we expect that
//                     the mName member has already been updated with a
//                     new name.  In this case, the old name of the
//                     file is renamed to the new name.
//
//  delete_on_clash  : If this flag is set, it means that we will overwrite
//                     an existing file with this file if the names clash.
//                     For example, if I am renaming TEMP.DAT to TEMP.BAK,
//                     and a TEMP.BAK already exists, I will delete it
//                     before renaming if this arg is set.
//
//
// DESCRIPTION
//
//  This virtual function provides a way to rename a storage object's
//  physical implementation.  It first updates the mName member if a
//  new_name argument is provided.  After that, we unlink the clash file
//  if one exists, then do a simple rename of mName.mszOldName to
//  new_name.
//
//  Note that this function does a lot of error checking in debug mode.
//  It also does a little error checking in release mode.
//
//  Since this is a virtual function in a derived class, C/VB/Delphi
//  programmers aren't going to be able to call it directly.  They
//  will instead call ALStorageRename(), which will wind up here
//  via the virtual function mechanism.
//
// RETURNS
//
//  AL_SUCCESS or AL_RENAME_ERROR.
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
ALFile::Rename( const char AL_DLL_FAR *new_name /* = 0 */,  /* Tag public function */
                int delete_on_clash /* = 1 */ )
{
    AL_ASSERT( miHandle == -1, "Rename: attempting to rename open file" );
    AL_ASSERT( mName.GetName() != 0, "Rename: attempting to rename file with null name" );
    AL_ASSERT( strlen( mName ) > 0, "Rename: attempting to rename file with 0 length name" );

    int status;
    const char AL_DLL_FAR *real_old_name;
    const char AL_DLL_FAR *real_new_name;
    if ( new_name ) {
        real_old_name = mName.GetSafeName();
        real_new_name = new_name;
    } else {
        real_old_name = mName.GetSafeOldName();
        real_new_name = mName.GetSafeName();
    }
#if !defined( AL_WIN32 )
    const char AL_DLL_FAR *p = strrchr( real_new_name, '.' );
    if ( p && strlen( p ) > 4 )
        return mStatus.SetError( AL_RENAME_ERROR,
                            "Error trying to rename %s.  It has a long "
                            "extension, which could lead to inadvertent "
                            "deletion of a file when trying to rename.",
                            real_old_name );
#endif
    if ( delete_on_clash ) {
        if ( mName.mCase == AL_MIXED )
            status = strcmp( real_new_name, real_old_name );
        else
            status = stricmp( real_new_name, real_old_name );
        if ( status == 0 )
            return mStatus.SetError( AL_RENAME_ERROR,
                                     "Error attempting to rename %s to %s.  "
                                     "Can't rename to the same name!",
                                     real_new_name,
                                     real_old_name );
        status = unlink( real_new_name );
        if ( status != 0 && errno != ENOENT )
            return mStatus.SetError( AL_RENAME_ERROR,
                                     "Error deleting %s before renaming %s.  "
                                     "errno = %d",
                                     real_new_name,
                                     real_old_name,
                                     errno );
    }
    status = rename( real_old_name, real_new_name );
    if ( status != 0 )
        return mStatus.SetError( AL_RENAME_ERROR,
                                 "Error renaming %s to %s.  errno = %d",
                                 real_old_name,
                                 real_new_name,
                                 errno );
    if ( new_name != 0 )
        mName = new_name;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::UnRename()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Undo the renaming of a file.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::UnRename( int delete_on_clash = 1 );
///
// C SYNOPSIS
//
//  None, use ALStorageUnRename().
//
// VB SYNOPSIS
//
//  None, use ALStorageUnRename().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageUnRename().
//
// ARGUMENTS
//
//  delete_on_clash  : If this flag is set, it means that we will overwrite
//                     an existing file with this file if the names clash.
//                     For example, if I am renaming TEMP.BAK to TEMP.DAT,
//                     and a TEMP.DAT already exists, I will delete it
//                     before renaming if this arg is set.
//
// DESCRIPTION
//
//  This virtual function provides a way to undo a previous renaming of
//  a storage object's physical name.  We can do this because the mName
//  member of ALStorage keeps track both of the current name of the file,
//  and the old name.  In this case, we just rename the current name to
//  the old name.  Then we update the mName member so it is accurate.
//
//  Note that this function does a lot of error checking in debug mode.
//  It also does a little error checking in release mode.
//
//  Since this is a virtual function in a derived class, C/VB/Delphi
//  programmers aren't going to be able to call it directly.  They
//  will instead call ALStorageRename(), which will wind up here
//  via the virtual function mechanism.
//
// RETURNS
//
//  AL_SUCCESS or AL_RENAME_ERROR.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::UnRename(), ALFile::Rename()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//


int AL_PROTO
ALFile::UnRename( int delete_on_clash /* = 1 */ )  /* Tag public function */
{
    AL_ASSERT( miHandle == -1, "UnRename: attempting to rename open file" );
    AL_ASSERT( mName.GetName() != 0, "UnRename: attempting to rename file with null name" );
    AL_ASSERT( mName.GetOldName() != 0, "UnRename: attempting to rename file with null old name" );
    AL_ASSERT( strlen( mName ) > 0, "UnRename: attempting to rename file with 0 length name" );
    AL_ASSERT( strlen( mName.GetOldName() ) > 0, "UnRename: attempting to rename file with 0 length old name" );

    int status;

    if ( delete_on_clash ) {
        status = unlink( mName.GetOldName() );
        if ( status != 0 && errno != ENOENT )
            return mStatus.SetError( AL_RENAME_ERROR,
                                     "Error deleting %s before renaming %s.  "
                                     "errno = %d",
                                     mName.GetOldName(),
                                     mName.GetName(),
                                     errno );
    }
    status = rename( mName, mName.GetOldName() );
    if ( status != 0 && errno != ENOENT )
        return mStatus.SetError( AL_RENAME_ERROR,
                                 "Error renaming %s to %s.  errno = %d",
                                 mName.GetName(),
                                 mName.GetOldName(),
                                 errno );
    ALStorage::mName = mName.GetOldName();
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::Delete()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Delete the underlying physical file associated with an ALFile object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::Delete()
//
// C SYNOPSIS
//
//  None, use ALStorageDelete().
//
// VB SYNOPSIS
//
//  None, use ALStorageDelete().
//
// DELPHI SYNOPSIS
//
//  None, use ALStorageDelete().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is called to delete the physical object associated with
//  a file.  This simply means calling the unlink() function for the
//  given name.
//
//  Since this is a virtual function in a derived class, C/VB/Delphi
//  programmers aren't going to be able to call it directly.  They
//  will instead call ALStorageDelete(), which will wind up here
//  via the virtual function mechanism.
//
// RETURNS
//
//  AL_DELETE_ERROR or AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALFile::ALFile(), ALStorage::Delete()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//


int AL_PROTO
ALFile::Delete()  /* Tag public function */
{
    AL_ASSERT( miHandle == -1, "Delete: attempting to delete open file" );
    AL_ASSERT( mName.GetName() != 0, "Delete: attempting to delete file with null name"  );
    AL_ASSERT( strlen( mName ) > 0, "Delete: attempting to delete file with 0 length name" );

    int status = unlink( mName );
    if ( status != 0 )
        return mStatus.SetError( AL_DELETE_ERROR,
                                 "Error deleting file %s, errno = %d ",
                                 mName.GetName(),
                                 errno );
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::Clone()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Clone this file object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  ALStorage * ALFile::Clone( const char *name,
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
//                  AL_STORAGE_DEFAULT and AL_FILE_OBJECT will cause this
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
//  Either a pointer to a newly constructed ALFile object, or a zero
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

ALStorage AL_DLL_FAR * AL_PROTO
ALFile::Clone( const char AL_DLL_FAR *name,  /* Tag public function */
               int object_type ) const
{
    switch ( object_type ) {
        case AL_STORAGE_DEFAULT :
        case AL_FILE_OBJECT :
            return new ALFile( name );
    }
    return 0;
}

//
// NAME
//
//  ALFile::MakeDirectory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Make a directory or a series of directories.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::MakeDirectory()
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
//  None.
//
// DESCRIPTION
//
//  If ALFile::Create() fails due to the target directory
//  not existing, it calls this routine.  This guy works his
//  way through a long file name, trying to make directories
//  all the way down, until it finishes or fails.
//
//  For example, if the mName member of this contains a string
//  like c:\mark\al200\temp\temp.c, this routine will attempt
//  to make, in turn:
//                      c:\       (starts at root)
//                      c:\mark
//                      c:\mark\al200
//                      c:\mark\al200\temp
//
// RETURNS
//
//  1 for success, 0 for failure.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO ALFile::MakeDirectory()  /* Tag protected function */
{
    char *p = new char[ strlen( mName ) + 1 ];
    if ( p ) {
        strcpy( p, mName );
//
//   Now I need to work through the entire file name, creating
//   each directory in order.  I do this by stuffing a 0 into
//   successive locations in the string where i find a '/' character,
//   and calling mkdir() at each point.
//
        for ( size_t i = 0 ; p[ i ] ; i++ ) {
            size_t j = i;
            i = strcspn( p + i, "/\\" );
            if ( i == strlen( p + j ) ) {
                delete[] p;
                return 1;
            }
            i += j;
            char temp = p[ i ];
            p[ i ] = '\0';
            if ( mkdir( p ) == -1 && errno != EACCES ) {
                delete[] p;
                return 0;
            }
            p[ i ] = temp;
        }
        delete[] p;
    }
    return 0;
}
