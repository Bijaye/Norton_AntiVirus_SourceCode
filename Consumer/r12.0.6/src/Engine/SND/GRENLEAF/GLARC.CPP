//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/GLARC.CPv   1.0   12 Mar 1998 12:16:18   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/GLARC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:18   DCHI
// Initial revision.
// 
//************************************************************************

//
// GLARC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALGlArchive::operator new()
//  ALGlArchive::ALGlArchive()
//  newALGlArchiveFromStorage()
//  ALGlArchive::~ALGlArchive()
//  ALGlArchive::PreWriteDir()
//  ALGlArchive::PostWriteDir()
//  ALGlArchive::WriteDirEntry()
//  ALGlArchive::ReadDirectory()
//  ALGlArchive::PreCreate()
//
// DESCRIPTION
//
//  This file contains all of the source code for the base class,
//  ALGlArchive. The details on how things get inserted and extracted from an
//  archive will all be found here.
//
// REVISION HISTORY
//
//  May 23, 1994  1.0A  : First release
//
//  July 7, 1994  1.0B  : Minor change to handle a snag with the sun
//                        compilers.
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

#include "glarc.h"
#include "_openf.h"

//
// NAME
//
//  ALGlArchive::operator new()
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
//  #include "glarc.h"
//
//  void * ALGlArchive::operator new( size_t size )
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
//           necessary to construct a new ALGlArchive object.
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
//  This function will be called whenever you dynamically create a new
//  ALGlArchive object (using the new operator.)
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
ALGlArchive::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALGlArchive::ALGlArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The Greenleaf Archive constructors.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  ALGlArchive::ALGlArchive( ALStorage &storage_object );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  hALArchive newALGlArchiveFromStorage( hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Function newALGlArchiveFromStorage Lib "AL20LW"
//    (ByVal storage&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALGlArchiveFromStorage( storage : hALStorage ) : hALArchive;
//
// ARGUMENTS
//
//  storage_object   : A pointer to the storage object that will/does
//                     hold the archive.
//
//
// DESCRIPTION
//
//  This function creates a new ALGlArchive object, using a storage object
//  as the underlying physical container.  GlArchives are siblings to
//  Pk archives.  While they both use the same base class, and support many
//  functions in common, they are essentially incompatible.
//
//  You call this constructor to manipulate any archive, including ones
//  that haven't even been created yet.
//
//  Recall that the base class, ALArchive, has a single argument to *its*
//  constructor that determines whether or not it destroys the underlying
//  storage object in its destructor.  Since we are using user-specified
//  storage in this case, we pass a 0, indicating that we don't want the
//  destructor to destroy the underlying storage object.
//
// RETURNS
//
//  When called from C/VB/Delphi, the translation function returns a handle
//  to a newly created archive object.  When called using the new operator
//  in C++, this function returns a pointer to a newly created archive
//  object.
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
ALGlArchive::ALGlArchive( ALStorage AL_DLL_FAR &storage_object )  /* Tag public function */
    : ALArchive( &storage_object, 0)
{
    miVersion = 0x100;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALArchive AL_FUNCTION
newALGlArchiveFromStorage( hALStorage storage ) /* Tag public function */
{
    AL_ASSERT( ( (ALStorage *) storage)->GoodTag(),
               "storage argument is not a valid ALStorageObject" );
    ALArchive *archive;
    archive = new ALGlArchive( *(ALStorage *) storage );
    return (hALArchive) archive;
}

#endif


//
// NAME
//
//  ALGlArchive::~ALGlArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Destructor for the Greenleaf Archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  ALGlArchive::~ALGlArchive()
//
// C SYNOPSIS
//
//  None, use deleteALArchive();
//
// VB SYNOPSIS
//
//  None, use deleteALArchive();
//
// DELPHI SYNOPSIS
//
//  None, use deleteALArchive();
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This is a virtual destructor for the derived class ALGlArchive.  It
//  has absolutely nothing to do, since all of the important work is done
//  by the base class destructor, ALArchive::~ALArchive().  Note that
//  there is no translation function for C/VB/Delphi to call this function
//  directly.  Instead, they will get here by way of a virtual function
//  call to the base class destructor.
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
ALGlArchive::~ALGlArchive()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALGlArchive(): Attempting to delete invalid ALGlArchive" );
}

//
// NAME
//
//  ALGlArchive::PreWriteDir()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Write out the archive data that precedes the directory.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  void ALGlArchive::PreWriteDir()
//
// C SYNOPSIS
//
//  None, this is an internal function.
//
// VB SYNOPSIS
//
//  None, this is an internal function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The base class, ALArchive, is responsible for all the work done when
//  creating an archive.  However, since Greenleaf and PKWare archives
//  are fairly different, the base class relies on a few virtual functions
//  to perform archive specific chores.
//
//  PreWriteDir() is one of those functions.  It is called after the
//  file pointer has been positioned to write the directory out, but
//  before any directory entries have been written.
//
//  Greenleaf archives store three items directly ahead of the
//  directory: the archive version, any customized archive data, and
//  the archive comment.  This routine just writes all those out, then
//  returns to the calling routine.  The caller is presumably the
//  write directory code in ALArchive, and it should continue by writing
//  out all of its directory entries.
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
ALGlArchive::PreWriteDir()  /* Tag protected function */
{
    mpArchiveStorageObject->WriteGlShort( miVersion );
    WriteArchiveData();
    mpArchiveStorageObject->WriteString( mComment );
}

//
// NAME
//
//  ALGlArchive::PostWriteDir()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Write out the archive data that follows the directory.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  void ALGlArchive::PostWriteDir()
//
// C SYNOPSIS
//
//  None, this is an internal function.
//
// VB SYNOPSIS
//
//  None, this is an internal function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The base class, ALArchive, is responsible for all the work done when
//  creating an archive.  However, since Greenleaf and PKWare archives
//  are fairly different, the base class relies on a few virtual functions
//  to perform archive specific chores.
//
//  PostWriteDir() is one of those functions.  It is called after the
//  directory entires for the archive have all been written out.
//
//  Greenleaf archives store three items directly ahead of the
//  directory: the archive version, any customized archive data, and
//  the archive comment.  This routine just writes all those out, then
//  returns to the calling routine.  The caller is presumably the
//  write directory code in ALArchive, and it should continue by writing
//  out all of its directory entries.
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
ALGlArchive::PostWriteDir()  /* Tag protected function */
{
    long keeper = mpArchiveStorageObject->Tell();
    mpArchiveStorageObject->WriteString( "" );
    mpArchiveStorageObject->Seek( 0L );
    mpArchiveStorageObject->WriteGlLong( mlDirectoryOffset );

    mpArchiveStorageObject->Seek( keeper );
}

//
// NAME
//
//  ALGlArchive::WriteDirEntry()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Write an individual object entry to the archive directory.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  void ALGlArchive::WriteDirEntry( ALEntry &entry );
//
// C SYNOPSIS
//
//  None, internal support function.
//
// VB SYNOPSIS
//
//  None, internal support function.
//
// DELPHI SYNOPSIS
//
//  None, internal support function.
//
// ARGUMENTS
//
//  entry  :  A reference to an entry.  This should correspond to an actual
//            object that is present in the archive.
//
// DESCRIPTION
//
//  The base class function, ALArchive::WriteDirectory(), sits in a loop
//  writing out directory entries, one after the other.  Since it is in
//  the base class, you can't expect it to know the exact format for an
//  entry.  So instead, it relies on this virtual function in derived
//  classes to do the job.
//
//  This function takes care of writing out a directory entry in the format
//  Greenleaf archives use.
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
ALGlArchive::WriteDirEntry( ALEntry AL_DLL_FAR &entry )  /* Tag protected function */
{
    mpArchiveStorageObject->WriteString( entry.mpStorageObject->mName.GetSafeName() );
    if ( entry.mpCompressor ) {
        mpArchiveStorageObject->WriteChar( entry.mpCompressor->miCompressionType );
        entry.mpCompressor->WriteEngineData( mpArchiveStorageObject );
    } else if ( entry.mpDecompressor ) {
        mpArchiveStorageObject->WriteChar( entry.mpDecompressor->miCompressionType );
        entry.mpDecompressor->WriteEngineData( mpArchiveStorageObject );
    } else {
        mStatus.SetError( AL_UNKNOWN_COMPRESSION_TYPE,
                          "No compressor or decompressor for archive"
                          " entry %s",
                          entry.mpStorageObject->mName.GetSafeName() );
        return;
    }
    mpArchiveStorageObject->WriteChar( entry.mpStorageObject->miStorageObjectType );
    entry.mpStorageObject->WriteStorageObjectData( mpArchiveStorageObject );

    mpArchiveStorageObject->WriteGlLong( entry.mpStorageObject->GetSize() );
    mpArchiveStorageObject->WriteGlLong( entry.GetCompressedSize() );
    mpArchiveStorageObject->WriteGlLong( entry.GetCrc32() );
    mpArchiveStorageObject->WriteGlLong( entry.mlCompressedObjectPosition );
    mpArchiveStorageObject->WriteString( entry.GetComment() );
    mpArchiveStorageObject->WriteGlLong( entry.mpStorageObject->mTimeDate.GetUnixTime() );
    mpArchiveStorageObject->WriteGlShort( entry.mpStorageObject->mAttributes.PackedAttributes() );
    if ( mpArchiveStorageObject->mStatus < 0 )
        mStatus = mpArchiveStorageObject->mStatus;
}

//
// NAME
//
//  ALGlArchive::ReadDirectory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read in the directory for a GL format archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  int ALGlArchive::ReadDirectory( ALEntryList &list )
//
// C SYNOPSIS
//
//  None, internal function.
//
// VB SYNOPSIS
//
//  None, internal function.
//
// DELPHI SYNOPSIS
//
//  None, internal function.
//
// ARGUMENTS
//
//  list  : An ALEntryList object that is going to receive all the
//          new ALEntry objects created from reading the directory.
//
// DESCRIPTION
//
//  During the process of creating ArchiveLib 2.0, the WriteDirectory()
//  function was moved up into the base class.  The addition of three
//  virtual helper functions takes care of all the specialized aspects
//  needed to make it happen.
//
//  Well, I haven't gotten around to that for the ReadDirectory function.
//  Maybe in the final release of 2.0, but more likely in the release
//  of ArchiveLib 3.0.  It's not a critical issue, it just helps break
//  the code down into smaller pieces, and is somewhat more efficient by
//  sharing common code.
//
// RETURNS
//
//  AL_SUCCESS or < AL_SUCCESS if things don't work.
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
ALGlArchive::ReadDirectory( ALEntryList AL_DLL_FAR &list )  /* Tag protected function */
{
    list.mrMonitor.ArchiveOperation( AL_START_DIRECTORY_READ, this, 0 );
    ALOpenInputFile archive( *mpArchiveStorageObject );
    if ( mpArchiveStorageObject->mStatus < 0 )
        return mStatus = mpArchiveStorageObject->mStatus;
//
// First I seek to the start of the directory (offset found at 0), and
// read in the version.  This function only supports the directory
// structure defined in version 0x100.
//
    mpArchiveStorageObject->Seek( 0 );
    mpArchiveStorageObject->ReadGlLong( mlDirectoryOffset );
    mpArchiveStorageObject->Seek( mlDirectoryOffset );
    mpArchiveStorageObject->ReadGlShort( miVersion );
    if ( miVersion != 0x100 )
        return mStatus.SetError( AL_INVALID_ARCHIVE,
                                 "%s is not a valid archive file",
                                 mpArchiveStorageObject->mName.GetSafeName() );
//
// Read in any customized archive data defined by a derived class.
//
    ReadArchiveData();
//
// Read in the comment, deleting the old one if necessary.
//
    mpArchiveStorageObject->ReadString( mComment );
//
// Now, the big loop.  I have to read in each entry, one at a time, and
// add it to the list.  If I broke this out into a separate routine it
// would make the whole thing a lot more manageable.
//
    for ( ; ; ) {
        if ( mpArchiveStorageObject->mStatus < 0 )
            return mStatus = mpArchiveStorageObject->mStatus;
        ALName name;
        mpArchiveStorageObject->ReadString( name );
//
// The directory ends with a blank name.
//
        if ( strlen( name ) == 0 )
            break;
//
// Derived classes are responsible for providing a version of
// CreateCompressionEngine() that will convert the engine_type
// integer into a created compression engine.  The derived class is
// then also responsible for reading in the engine data from the archive.
//
        int engine_type = mpArchiveStorageObject->ReadChar();
        ALCompressor *compressor = 0;
        ALDecompressor *decompressor = list.mToolKit.CreateDecompressor( engine_type );
        if ( decompressor )
            decompressor->ReadEngineData( mpArchiveStorageObject );
        else {
            compressor = list.mToolKit.CreateCompressor( engine_type );
            if ( compressor )
                compressor->ReadEngineData( mpArchiveStorageObject );
            else {
                ALName temp;
                mpArchiveStorageObject->ReadString( temp );
//
// I used to treat this as an error, now I don't.  The error will come about
// when you actually try to decompress the object.
//
#if 0
                return mStatus.SetError( AL_CANT_CREATE_ENGINE,
                                         "Failure creating compression engine for object %s",
                                         (const char *) name );
#endif
            }
        }
//
// Now we go through a nearly identical process to create the storage object.
// The derived class is responsible for writing a CreateStorageObject()
// function that converts an object_type integer to a created storage
// object.  The derived class also has to read in the storage object
// data.
//
        int object_type = mpArchiveStorageObject->ReadChar();
        ALStorage *storage_object = list.mToolKit.CreateStorageObject( name, object_type );
        if ( storage_object )
            storage_object->ReadStorageObjectData( mpArchiveStorageObject );
        else {
            ALName temp;
            mpArchiveStorageObject->ReadString( temp );
//
// I used to treat this as an error, now I don't.  The error will come about
// when you actually try to compress the object.
//
#if 0
            return mStatus.SetError( AL_CANT_CREATE_STORAGE_OBJECT,
                                     "Failure creating storage object for object %s",
                                     (const char *) name );
#endif
        }
//
// The rest of the stuff in the entry is pretty straightforward.
//
        mpArchiveStorageObject->ReadGlLong( storage_object->mlSize );
        ALEntry *job = new ALEntry( list, storage_object, compressor, decompressor );
        mpArchiveStorageObject->ReadGlLong( job->mlCompressedSize );
        mpArchiveStorageObject->ReadGlLong( job->mlCrc32 );
        mpArchiveStorageObject->ReadGlLong( job->mlCompressedObjectPosition );
        ALName comment;
        mpArchiveStorageObject->ReadString( comment );
        job->SetComment( comment );
        long unix_time;
        mpArchiveStorageObject->ReadGlLong( unix_time );
        storage_object->mTimeDate.SetTimeDate( unix_time );
        short int packed_attributes;
        mpArchiveStorageObject->ReadGlShort( packed_attributes );
        storage_object->mAttributes.SetFromPackedAttributes( packed_attributes );
    }
    list.mrMonitor.ArchiveOperation( AL_END_DIRECTORY_READ, this, 0 );
    return mStatus;
}

//
// NAME
//
//  ALEntryList::ClearError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Reset the error status for an entry list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  void ALGlArchive::PreCreate()
//
// C SYNOPSIS
//
//  None, internal support function.
//
// VB SYNOPSIS
//
//  None, internal support function.
//
// DELPHI SYNOPSIS
//
//  None, internal support function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//   The Create() function is found in the base class.  It takes care of
//   sticking a bunch of compressed objects into an archive.  It gets
//   ready to do this, then class this function first.
//
//   Since this is a virtual function, the derived class can put any
//   specialized behavior here.  For Greenleaf archives, the only
//   specialized behavior needed is to save enough space for a long
//   in the file.  After the archive is created, the long at offset 0
//   will get a pointer to the start of the directory.
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
ALGlArchive::PreCreate()  /* Tag protected function */
{
//
// The first four bytes in the archive are a long that points to the
// first byte of the directory.  I don't know where the directory is
// going to be, so I just reserve space at this time with a dummy value.
//
    mpArchiveStorageObject->WriteGlLong( 0x12345678L );
}

#endif // #if !defined( AL_DEMO )

