//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/pkarc.cpv   1.2   16 Jun 1998 16:09:46   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/pkarc.cpv  $
// 
//    Rev 1.2   16 Jun 1998 16:09:46   DCHI
// Modified PreCompress() to strip out size of absolute path info
// (i.e., "C:\") from local header size.
// 
//    Rev 1.1   08 Jun 1998 11:24:02   DCHI
// Modified PostCompress() to strip out absolute path info (i.e., "C:\")
// from local header.  The info is already stripped out of the central
// header.  So before this mod, the headers differed causing problems
// for UNZIP/ZIP.
// 
//    Rev 1.0   12 Mar 1998 12:17:22   DCHI
// Initial revision.
// 
//************************************************************************

//
// PKARC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALPkArchive::operator new()
//  ALPkArchive::ALPkArchive()
//  newALPkArchiveFromStorage()
//  ALPkArchive::~ALPkArchive()
//  ALPkArchive::WriteArchiveData()
//  ALPkArchive::ReadArchiveData()
//  ALPkArchive::LocateEcd()
//  ALPkArchive::ReadEcd()
//  ALGlArchive::ReadDirectory()
//  ALPkArchive::PreCompress()
//  ALPkArchive::PreDecompress()
//  ALPkArchive::PostCompress()
//  ALPkArchive::PreWriteDir()
//  ALPkArchive::WriteDirEntry()
//  ALPkArchive::PostWriteDir()
//
// DESCRIPTION
//
//  This file contains all of the source code for the class ALPkArchive.
//  The details on how things get inserted and extracted from a PKZip
//  archive will all be found here.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <time.h>
#include <ctype.h>
#include "filestor.h"
#include "pkarc.h"
#include "pkengn.h"
#include "_openf.h"

//
// NAME
//
//  ALPkArchive::operator new()
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
//  #include "pkarc.h"
//
//  void * ALPkArchive::operator new( size_t size )
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
//           necessary to construct a new ALPkArchive object.
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
//  ALPkArchive object (using the new operator.)
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
ALPkArchive::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALPkArchive::ALPkArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The PKWare Archive constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  ALPkArchive::ALPkArchive( ALStorage &storage_object );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  hALArchive newALPkArchiveFromStorage( hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Function newALPkArchiveFromStorage Lib "AL20LW"
//    (ByVal storage&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALPkArchiveFromStorage( storage : hALStorage ) : hALArchive;
//
// ARGUMENTS
//
//  storage_object   : A pointer to the storage object that will/does
//                     hold the archive.
//
//
// DESCRIPTION
//
//  This function creates a new ALPkArchive object, using a storage object
//  as the underlying physical container.  PkArchives are siblings to
//  Gl archives.  While they both use the same base class, and support many
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
//  object. O/w it returns nothing.
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
ALPkArchive::ALPkArchive( ALStorage AL_DLL_FAR &storage_object )  /* Tag public function */
    : ALArchive( &storage_object, 0)
{
    miVersion = 0x020;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALArchive AL_FUNCTION
newALPkArchiveFromStorage( hALStorage storage ) /* Tag public function */
{
    AL_ASSERT( ( (ALStorage *) storage)->GoodTag(),
               "storage argument is not a valid ALStorageObject" );
    ALArchive *archive;
    archive = new ALPkArchive( *(ALStorage *) storage );
    return (hALArchive) archive;
}

#endif

//
// NAME
//
//  ALPkArchive::~ALPkArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Destructor for the PKWare Archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  ALPkArchive::~ALPkArchive()
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
//  This is a virtual destructor for the derived class ALPkArchive.  It
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
ALPkArchive::~ALPkArchive()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALPkArchive(): Attempting to delete invalid ALPkArchive" );
}

//
// NAME
//
//  ALPkArchive::WriteArchiveData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Virtual function to write class-specific archive data.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  int ALPkArchive::WriteArchiveData()
//
// C SYNOPSIS
//
//  None, this is a C++ specific callback virtual function.
//
// VB SYNOPSIS
//
//  None, this is a C++ specific callback virtual function.
//
// DELPHI SYNOPSIS
//
//  None, this is a C++ specific callback virtual function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  ArchiveLib supports this virtual function in its base class.  It is
//  here to allow the user to derive new archive classes, and then store
//  additional archive-specific info when creating archives.  PKWare
//  archives don't really have any space on hand for archive specific
//  data, so this function turns into a nop.  We can't just nuke it though,
//  because the base class writes a couple of bytes out.  Those two bytes
//  would destroy any PKWare archives.
//
// RETURNS
//
//  AL_SUCCESS, guaranteed.
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
ALPkArchive::WriteArchiveData()  /* Tag protected function */
{
    return AL_SUCCESS;
}

//
// NAME
//
//  ALPkArchive::ReadArchiveData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Virtual function to read class-specific archive data.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  int ALPkArchive::ReadArchiveData()
//
// C SYNOPSIS
//
//  None, this is a C++ specific callback virtual function.
//
// VB SYNOPSIS
//
//  None, this is a C++ specific callback virtual function.
//
// DELPHI SYNOPSIS
//
//  None, this is a C++ specific callback virtual function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  ArchiveLib supports this virtual function in its base class.  It is
//  here to allow the user to derive new archive classes, and then store
//  and retrieve additional archive-specific info to/from archives.  PKWare
//  archives don't really have any space on hand for archive specific
//  data, so this function turns into a nop.  We can't just nuke it though,
//  because the base class tries to read in a couple of bytes.  Those two bytes
//  aren't there in any PKWare format archives.
//
// RETURNS
//
//  AL_SUCCESS, guaranteed.
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
ALPkArchive::ReadArchiveData()  /* Tag protected function */
{
    return AL_SUCCESS;
}


//
// NAME
//
//  ALPkArchive::LocateEcd()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Find the end of the central directory.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  int ALPkArchive::LocateEcd()
//
// C SYNOPSIS
//
//  None, this is an internal protected support function.
//
// VB SYNOPSIS
//
//  None, this is an internal protected support function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal protected support function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The first step in reading a PKWare format archive is to find the
//  end of the central directory.  Unfortunately, this is kind of an
//  iffy proposition.  Why?  Because you just have to blindly search
//  from the end of the ZIP file, working backwards, until you find
//  a magic cookie:  'P', 'K', 5, 6.
//
//  The 5 and 6 are not printable ASCII, which helps, but it is still
//  a pretty gruesome way to handle things.  Yuck.  Anyway, it is the
//  job of this function to search backwards through the archive, looking
//  for the magic cookie.  If I find it, I save it in the class member
//  that keeps track of the ECD offset.
//
//  I limit the search to around 64K, because that's what the InfoZip
//  guys do.  I think they are counting on it not being legal to have
//  a central directory bigger than 64K.
//
// RETURNS
//
//  AL_SUCCESS if the ECD was found, AL_INVALID_ARCHIVE if not.  If
//  it was found, mEcd.mlOffset is set to the file offset.
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
ALPkArchive::LocateEcd()  /* Tag protected function */
{
    const int BUF_SIZE = 256;
    char buf[ BUF_SIZE + 4 ];

    long offset = mpArchiveStorageObject->GetSize();
    long left_to_check = offset < 66000L ? offset : 66000L;
    buf[ 0 ] = '\0'; //prevent false trigger the first time through
    for ( ; ; ) {
        int number_to_read;
        if ( left_to_check == 0 )
            break;
        if ( left_to_check < BUF_SIZE )
            number_to_read = (int) left_to_check;
        else
            number_to_read = BUF_SIZE;
        offset -= number_to_read;
        memcpy( buf + BUF_SIZE, buf, 4 );
        mpArchiveStorageObject->Seek( offset );
        mpArchiveStorageObject->ReadBuffer( (unsigned char *) buf, number_to_read );
        for ( int i = number_to_read ; i >= 0 ; i-- ) {
            if ( buf[ i + 0 ] == 'P' &&
                 buf[ i + 1 ] == 'K' &&
                 buf[ i + 2 ] == 5   &&
                 buf[ i + 3 ] == 6 ) {
                mEcd.mlOffset = offset + i;
                return AL_SUCCESS;
            }
        }
        left_to_check -= number_to_read;
    }
    mEcd.mlOffset = -1L;
    return mStatus.SetError( AL_INVALID_ARCHIVE,
                             "Can't locate end of central directory in "
                             "ZIP file %s",
                             mpArchiveStorageObject->mName.GetSafeName() );
}

//
// NAME
//
//  ALPkArchive::ReadEcd()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read in the End of Central Directory record from a PKWare archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  int ALPkArchive::ReadEcd()
//
// C SYNOPSIS
//
//  None, this is an internal protected support function.
//
// VB SYNOPSIS
//
//  None, this is an internal protected support function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal protected support function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  Once the PKWare code finds the ECD, this routine is called to
//  read in some important data items.  These are all crucial to the
//  successful use of the archive later.  Note that most of the stuff
//  goes into the mEcd member, which is unique to this derived class.
//  However, the comment and directory offset are universal, and go
//  into members of the base class.
//
// RETURNS
//
//  AL_SUCCESS, always.  Any error in reading will be reflected into
//  the ALArchive object itself, and should be picked up later.
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
ALPkArchive::ReadEcd()  /* Tag protected function */
{
    mpArchiveStorageObject->Seek( mEcd.mlOffset + 4 );
    mpArchiveStorageObject->ReadPkShort( mEcd.muThisDiskNumber );
    mpArchiveStorageObject->ReadPkShort( mEcd.muStartCentralDirDiskNumber );
    mpArchiveStorageObject->ReadPkShort( mEcd.muThisDiskCentralDirEntries );
    mpArchiveStorageObject->ReadPkShort( mEcd.muTotalCentralDirEntries );
    mpArchiveStorageObject->ReadPkLong( mEcd.mlSizeCentralDir );
    mpArchiveStorageObject->ReadPkLong( mlDirectoryOffset );
    short int comment_length;
    mpArchiveStorageObject->ReadPkShort( comment_length );
    mpArchiveStorageObject->ReadString( mComment, comment_length );
    return AL_SUCCESS;
}

//
// NAME
//
//  ALPkArchive::ReadDirectory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read in the directory of a PKWare archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  int ALPkArchive::ReadDirectory( ALEntryList &list );
//
// C SYNOPSIS
//
//  C programs call this function via a translation function from
//  the base class: ALArchiveReadDirectory()
//
// VB SYNOPSIS
//
//  VB programs call this function via a translation function from
//  the base class: ALArchiveReadDirectory()
//
// DELPHI SYNOPSIS
//
//  Delphi programs call this function via a translation function from
//  the base class: ALArchiveReadDirectory()
//
// ARGUMENTS
//
//  list  : An ALEntryList object that is going to receive all the
//          new ALEntry objects created from reading the directory.
//
// DESCRIPTION
//
//  ReadDirectory has to rip through the directory of the PKWare archive,
//  creating an ALEntry for every single entry in the directory.  While
//  this is conceptually pretty simple, there is a lot of crap in the
//  directory that has to be read in and massaged.  Hence, it is an
//  exceptionally long piece of code.
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
//  AL_SUCCESS, or some other miscellaneous error code.
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
ALPkArchive::ReadDirectory( ALEntryList AL_DLL_FAR &list )  /* Tag public function */
{
    list.mrMonitor.ArchiveOperation( AL_START_DIRECTORY_READ, this, 0 );
    ALOpenInputFile archive( *mpArchiveStorageObject );
    if ( mpArchiveStorageObject->mStatus < 0 )
        return mStatus = mpArchiveStorageObject->mStatus;
    if ( LocateEcd() < AL_SUCCESS )
        return mStatus;
//
// Now I have to read in the end of central directory information
//
    ReadEcd();
    mpArchiveStorageObject->Seek( mlDirectoryOffset );
//
// Now, the big loop.  I have to read in each entry, one at a time, and
// add it to the list.  If I broke this out into a separate routine it
// would make the whole thing a lot more manageable.
//
    for ( ; ; ) {
        char buf[ 4 ];
        if ( mpArchiveStorageObject->ReadBuffer( (unsigned char *) buf, 4 ) != 4 )
            return mStatus = mpArchiveStorageObject->mStatus;
//
// Remember that 'P', 'K', 5, 6 is the End of Central Directory record.
//
        if ( buf[ 0 ] == 'P' && buf[ 1 ] == 'K' && buf[ 2 ] == 5 && buf[ 3 ] == 6 )
            break;
        if ( buf[ 0 ] != 'P' || buf[ 1 ] != 'K' || buf[ 2 ] != 1 || buf[ 3 ] != 2 )
            return mStatus.SetError( AL_INVALID_ARCHIVE,
                                     "Bad record in central directory of "
                                     "ZIP file %s",
                                      mpArchiveStorageObject->mName.GetSafeName() );
//
// Skip over the next four bytes, then get the compression method
//
        int k;
        for ( k = 0 ; k < 4 ; k++ )
            mpArchiveStorageObject->ReadChar();
        short int gp_bits;
        mpArchiveStorageObject->ReadPkShort( gp_bits );
        short int method;
        mpArchiveStorageObject->ReadPkShort( method );
//
// Derived classes are responsible for providing a version of
// CreateCompressionEngine() that will convert the engine_type
// integer into a created compression engine.  The derived class is
// then also responsible for reading in the engine data from the archive.
//
        if ( gp_bits & 1 )
            method = -1;
        else if ( method == 8 )
            method = (short int) (method + ( ( gp_bits >> 1 ) & 3 ));
        method = (short int)( method + 100 );
        ALDecompressor *decompressor = list.mToolKit.CreateDecompressor( method );
#if 0
//
// We'll be letting this slide from now on
//
        if ( !decompressor )
            return mStatus.SetError( AL_CANT_CREATE_ENGINE,
                                     "Failure creating compression engine "
                                     "in ZIP file %s",
                                     mpArchiveStorageObject->mName.GetSafeName()  );
#endif
        ALCompressor *compressor = list.mToolKit.CreateCompressor( method );
//
// Now we go through a nearly identical process to create the storage object.
// The derived class is responsible for writing a CreateStorageObject()
// function that converts an object_type integer to a created storage
// object.  The derived class also has to read in the storage object
// data.
//
        ALStorage *storage_object = list.mToolKit.CreateStorageObject( "", AL_FILE_OBJECT );
        if ( !storage_object ) {
            if ( compressor )
                delete compressor;
            delete decompressor;
            return mStatus.SetError( AL_CANT_CREATE_STORAGE_OBJECT,
                                     "Failure creating storage object in "
                                     "ZIP file %s",
                                     mpArchiveStorageObject->mName.GetSafeName() );
        }
        ALEntry *job = new ALEntry( list, storage_object, compressor, decompressor );
        short int pktime;
        short int pkdate;
        mpArchiveStorageObject->ReadPkShort( pktime );
        mpArchiveStorageObject->ReadPkShort( pkdate );
        struct tm tblock;
        tblock.tm_year = ( ( pkdate >> 9 ) & 0x7f ) + 1980 - 1900;
        tblock.tm_mon = ( ( pkdate >> 5 ) & 0xf ) - 1;
        tblock.tm_mday = pkdate & 0x1f;
        tblock.tm_hour = (pktime >> 11) & 0x1f;
        tblock.tm_min =  (pktime >> 5 ) & 0x3f;
        tblock.tm_sec = (pktime & 0x1f) * 2;
        tblock.tm_wday = 0;
        tblock.tm_yday = 0;
        tblock.tm_isdst = 0;
        storage_object->mTimeDate.SetTimeDate( &tblock );
        mpArchiveStorageObject->ReadPkLong( job->mlCrc32 );
        job->mlCrc32 = ~job->mlCrc32;
        mpArchiveStorageObject->ReadPkLong( job->mlCompressedSize );
        mpArchiveStorageObject->ReadPkLong( storage_object->mlSize );
        short int filename_length;
        mpArchiveStorageObject->ReadPkShort( filename_length );
        short int extra_length;
        mpArchiveStorageObject->ReadPkShort( extra_length );
        short int comment_length;
        mpArchiveStorageObject->ReadPkShort( comment_length );
        short int disk_number_start;
        mpArchiveStorageObject->ReadPkShort( disk_number_start );
        short int internal_attributes;
        long int external_attributes;
        mpArchiveStorageObject->ReadPkShort( internal_attributes );
        mpArchiveStorageObject->ReadPkLong( external_attributes );
        int packed = 0;
        if ( external_attributes & 1 )
            packed |= ATTR_READ_ONLY;
        if ( external_attributes & 4 )
            packed |= ATTR_SYSTEM;
        if ( external_attributes & 2 )
            packed |= ATTR_HIDDEN;
        if ( external_attributes & 0x20 )
            packed |= ATTR_ARCHIVE;
        if ( external_attributes & 0x10 )
            packed |= ATTR_DIRECTORY;
        if ( external_attributes & 0x08 )
            packed |= ATTR_LABEL;
        storage_object->mAttributes.SetFromPackedAttributes( (short int) packed );
        mpArchiveStorageObject->ReadPkLong( job->mlCompressedObjectPosition );
        mpArchiveStorageObject->ReadString( storage_object->mName, filename_length );
        for ( k = 0 ; k < extra_length ; k++ )
            mpArchiveStorageObject->ReadChar();
        ALName comment;
        mpArchiveStorageObject->ReadString( comment, comment_length );
        job->SetComment( comment );
        if ( mpArchiveStorageObject->mStatus < 0 )
            return mStatus = mpArchiveStorageObject->mStatus;
    }
    list.mrMonitor.ArchiveOperation( AL_END_DIRECTORY_READ, this, 0 );
    return mStatus;
}

//
// NAME
//
//  ALPkArchive::PreCompress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Take care of details before inserting an object in an archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  void ALPkArchive::PreCompress( ALEntry &entry )
//
// C SYNOPSIS
//
//  This is a protected callback routine, and has no C equivalent.
//
// VB SYNOPSIS
//
//  This is a protected callback routine, and has no VB equivalent.
//
// DELPHI SYNOPSIS
//
//  This is a protected callback routine, and has no Delphi equivalent.
//
// ARGUMENTS
//
//  entry  :  This is the ALEntry that describes the object that is
//            about to be compressed.
//
// DESCRIPTION
//
//  This is a virtual function that gets called by the CompressJobs()
//  function in the base class.  The virtual function is supposed to
//  take care of any odds and ends that need to be done immediately
//  before an object is compressed into the archive.
//
//  In the case of PKWare archives, this means seeking past the local
//  directory entry, which is going to be 30 bytes long, plus the length
//  of the object name.
//
//  When done compressing, we will jump back to the original spot and write
//  out the directory information.
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
//   February 14, 1996  2.0A  : New Release
//
//   March 25, 1996     2.01A : Had a problem here.  The size of the
//                              name was changing during the process
//                              of insertion, as the path name could
//                              be stripped.  This meant I possible was
//                              leaving too much room for the name.  This
//                              new code anticipates the stripping of
//                              the name.
//

void AL_PROTO
ALPkArchive::PreCompress( ALEntry AL_DLL_FAR &entry )  /* Tag protected function */
{
    ALName temp = entry.mpStorageObject->mName;
    if ( mfStripPathOnInsert )
        temp.StripPath();
    const char *name = temp.GetSafeName();

//
// PKWare doesn't like absolute path information, so I have to strip the
// info from the name
//
    if ( strlen( name ) > 2 && isalpha( name[ 0 ] ) && name[ 1 ] == ':' )
        name += 2;
    if ( strlen( name ) > 1 && name[ 0 ] == '\\' )
        name++;

    mpArchiveStorageObject->Seek( mpArchiveStorageObject->Tell() +
                                  30 + strlen( name ) );
}

//
// NAME
//
//  ALPkArchive::PreDecompress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Take care of details before extracting a file.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  void ALPkArchive::PreDecompress( ALEntry &entry );
//
// C SYNOPSIS
//
//  This is an internal C++ support routine, no C equivalent.
//
// VB SYNOPSIS
//
//  This is an internal C++ support routine, no C equivalent.
//
// DELPHI SYNOPSIS
//
//  This is an internal C++ support routine, no C equivalent.
//
// ARGUMENTS
//
//  entry  :  A reference to the ALEntry object that is about to be
//            decompressed.  Note that this function doesn't use this
//            argument.
//
// DESCRIPTION
//
//  Extracting a batch of jobs is left up to the base class.  As the
//  base class code is busy extracting jobs, it calls this routine
//  *before* the extraction, and PostDecompress *after* the extraction.
//
//  Unlike GL Archvies, PKWare archives have a local directory entry
//  that sits right in front of the compressed data.  So this virtual
//  function takes care of skipping over that local data, and leaving
//  the file pointer sitting right at the start of the compressed object.
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
ALPkArchive::PreDecompress( ALEntry AL_DLL_FAR &/*entry*/ )  /* Tag protected function */
{
    int ii;
    for ( ii = 0 ; ii < 26 ; ii++ )
        mpArchiveStorageObject->ReadChar();
    short int filename_length;
    short int extra_length;
    mpArchiveStorageObject->ReadPkShort( filename_length );
    mpArchiveStorageObject->ReadPkShort( extra_length );
    for ( ii = 0 ; ii < (filename_length + extra_length ) ; ii++ )
        mpArchiveStorageObject->ReadChar();
}

//
// NAME
//
//  ALPkArchive::PreCopyInput()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Take care of details before copying a compressed object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  void ALPkArchive::PreCopyInput( ALEntry &entry );
//
// C SYNOPSIS
//
//  This is an internal C++ support routine, no C equivalent.
//
// VB SYNOPSIS
//
//  This is an internal C++ support routine, no C equivalent.
//
// DELPHI SYNOPSIS
//
//  This is an internal C++ support routine, no C equivalent.
//
// ARGUMENTS
//
//  entry  :  A reference to the ALEntry object that is about to be
//            copied.
//
// DESCRIPTION
//
//  Copying a batch of compressed objects from one archive to another is
//  left up to the base class.  As the base class code is busy copying jobs,
//  it calls this routine *before* the copy, and PostCoppyInput *after* the
//  copy.
//
//  Unlike GL Archvies, PKWare archives have a local directory entry
//  that sits right in front of the compressed data.  So this virtual
//  function takes care of skipping over that local data, and leaving
//  the file pointer sitting right at the start of the compressed object.
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
//   March 25, 1996     2.01A : Fixed a bug in filename processing.
//
void AL_PROTO
ALPkArchive::PreCopyInput( ALEntry AL_DLL_FAR &/* entry */ )  /* Tag protected function */
{
    short name_length;
    int i;
//
//  I had to change the way I was processing this section of the
//  archive.  I was counting on the name in the ECD being the same as
//  the name in the local header.  Not always so!  For example, if I
//  read the dir, then rename a file and write the dir, this code
//  would break!
//
    for ( i = 0 ; i < 26 ; i++ )
        mpArchiveStorageObject->ReadChar();
    mpArchiveStorageObject->ReadPkShort( name_length );
    for ( i = 0 ; i < (name_length + 2 ) ; i++ )
        mpArchiveStorageObject->ReadChar();
}

//
// NAME
//
//  ALPkArchive::PostCompress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Take care of details after inserting a compressed object in an archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  void ALPkArchive::PostCompress( ALEntry &entry )
//
// C SYNOPSIS
//
//  This is a protected callback routine, and has no C equivalent.
//
// VB SYNOPSIS
//
//  This is a protected callback routine, and has no VB equivalent.
//
// DELPHI SYNOPSIS
//
//  This is a protected callback routine, and has no Delphi equivalent.
//
// ARGUMENTS
//
//  entry  :  This is the ALEntry that describes the object that has
//            just been compressed.
//
// DESCRIPTION
//
//  This is a virtual function that gets called by the CompressJobs()
//  function in the base class.  The virtual function is supposed to
//  take care of any odds and ends that need to be done immediately
//  after object is compressed into the archive.
//
//  In the case of PKWare archives, this means seeking back to the
//  correct spot in the archive, then writing out local directory
//  information.
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
//   March 25, 1996     2.01A : Had a problem here.  The size of the
//                              name was changing during the process
//                              of insertion, as the path name could
//                              be stripped.  This meant I possible was
//                              leaving too much room for the name.  This
//                              new code anticipates the stripping of
//                              the name.
//


void AL_PROTO
ALPkArchive::PostCompress( ALEntry AL_DLL_FAR &entry )  /* Tag protected function */
{
    ALName temp = entry.mpStorageObject->mName;
    if ( mfStripPathOnInsert )
        temp.StripPath();
    const char *name = temp.GetSafeName();

//
// PKWare doesn't like absolute path information, so I have to strip the
// info from the name
//
    if ( strlen( name ) > 2 && isalpha( name[ 0 ] ) && name[ 1 ] == ':' )
        name += 2;
    if ( strlen( name ) > 1 && name[ 0 ] == '\\' )
        name++;

    entry.mlCompressedSize = mpArchiveStorageObject->Tell() -
                             entry.mlCompressedObjectPosition -
                             30 - strlen( name );
//
//  Now I have to write the local file header
//
    long keeper = mpArchiveStorageObject->Tell();
    mpArchiveStorageObject->Seek( entry.mlCompressedObjectPosition );
    mpArchiveStorageObject->WriteChar( 'P' );
    mpArchiveStorageObject->WriteChar( 'K' );
    mpArchiveStorageObject->WriteChar( 3 );
    mpArchiveStorageObject->WriteChar( 4 );
    mpArchiveStorageObject->WritePkShort( 20 );
    short int flags;
    short int method;
    switch ( entry.mpCompressor->miCompressionType ) {
        case AL_COMPRESSION_COPY :
            method = 0;
            flags = 0;
            break;
        case AL_COMPRESSION_DEFLATE :
            method = 8;
            switch ( ( (ALPkCompressor AL_DLL_FAR *) entry.mpCompressor)->miLevel ) {
                case 9 :  flags = 2; break;
                case 1 :  flags = 4; break;
                default : flags = 0; break;
            }
            break;
        default :
            method = -1;
            flags = 0;
        }
        mpArchiveStorageObject->WritePkShort( flags );
        mpArchiveStorageObject->WritePkShort( method );
        mpArchiveStorageObject->WritePkShort( entry.mpStorageObject->mTimeDate.GetDosTime() );
        mpArchiveStorageObject->WritePkShort( entry.mpStorageObject->mTimeDate.GetDosDate() );
        mpArchiveStorageObject->WritePkLong( ~entry.mlCrc32 ); //CRC
        mpArchiveStorageObject->WritePkLong( entry.mlCompressedSize ); //compressed size
        mpArchiveStorageObject->WritePkLong( entry.mpStorageObject->GetSize() ); //un compressed size
        mpArchiveStorageObject->WritePkShort( (short int) strlen( name ) ); //un compressed size
        mpArchiveStorageObject->WritePkShort( 0 );
        mpArchiveStorageObject->WriteBuffer( (unsigned char *) name, strlen( name ) );
        mpArchiveStorageObject->Seek( keeper );
}

//
// NAME
//
//  ALPkArchive::PreWriteDir()
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
//  void ALPkArchive::PreWriteDir()
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
//  PKWare archives don't have to write anything special out at the
//  start of the central directory, but they do have one important
//  thing to do in this routine.  They need to keep track of exactly
//  where the start of the directory is.  That gets figured out here
//  and stored in the archive object, for later use.
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
ALPkArchive::PreWriteDir()  /* Tag protected function */
{
    mlStartOfCentralDir = mpArchiveStorageObject->Tell();
}

//
// NAME
//
//  ALPkArchive::WriteDirEntry()
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
//  #include "pkkrc.h"
//
//  void ALPkArchive::WriteDirEntry( ALEntry &entry );
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
//  PKWare archives use.  There is a lot of crud to write out, but
//  it is all fairly straightforward.
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
ALPkArchive::WriteDirEntry( ALEntry AL_DLL_FAR &entry )  /* Tag protected function */
{
    mpArchiveStorageObject->WriteChar( 'P' );
    mpArchiveStorageObject->WriteChar( 'K' );
    mpArchiveStorageObject->WriteChar( 1 );
    mpArchiveStorageObject->WriteChar( 2 );
    mpArchiveStorageObject->WritePkShort( 20 );
    mpArchiveStorageObject->WritePkShort( 20 );
    short int flags;
    short int method;
    ALCompressionType ctype;
    if ( entry.mpCompressor )
        ctype = entry.mpCompressor->miCompressionType;
    else if ( entry.mpDecompressor )
        ctype = entry.mpDecompressor->miCompressionType;
    else
        ctype = AL_COMPRESSION_DEFAULT;
    switch ( ctype ) {
                case AL_COMPRESSION_COPY :
                    method = 0;
                    flags = 0;
                    break;
                case AL_COMPRESSION_DEFLATE :
                    method = 8;
                    int pk_option;
                    if ( entry.mpCompressor )
                        pk_option = ((ALPkCompressor AL_DLL_FAR *) entry.mpCompressor)->option;
                    else
                        pk_option = ((ALPkDecompressor AL_DLL_FAR *) entry.mpDecompressor)->option;
                    switch ( pk_option ) {
                        case ALPkCompressor::FAST       : flags = 4; break;
                        case ALPkCompressor::SUPER_FAST : flags = 6; break;
                        case ALPkCompressor::MAXIMUM    : flags = 2; break;
                        case ALPkCompressor::NORMAL     :
                        default                         : flags = 0; break;
                    }
                    break;
                 default :
                     method = -1;
                     flags = 0;
            }
            mpArchiveStorageObject->WritePkShort( flags );
            mpArchiveStorageObject->WritePkShort( method );
            mpArchiveStorageObject->WritePkShort( entry.mpStorageObject->mTimeDate.GetDosTime() );
            mpArchiveStorageObject->WritePkShort( entry.mpStorageObject->mTimeDate.GetDosDate() );
            mpArchiveStorageObject->WritePkLong( ~entry.mlCrc32 ); //CRC
            mpArchiveStorageObject->WritePkLong( entry.mlCompressedSize ); //compressed size
            mpArchiveStorageObject->WritePkLong( entry.mpStorageObject->GetSize() ); //un compressed size
            const char *name = entry.mpStorageObject->mName.GetSafeName();
//
// PKWare doesn't like absolute path information, so I have to strip the
// info from the name
//
            if ( strlen( name ) > 2 && isalpha( name[ 0 ] ) && name[ 1 ] == ':' )
                name += 2;
            if ( strlen( name ) > 1 && name[ 0 ] == '\\' )
                name++;
            mpArchiveStorageObject->WritePkShort( (short int) strlen( name ) );
            mpArchiveStorageObject->WritePkShort( 0 );
            const char *comment = entry.mszComment;
            if ( comment == 0 )
                comment = "";
            mpArchiveStorageObject->WritePkShort( (short int) strlen( comment ) );
            mpArchiveStorageObject->WritePkShort( 1 );
            mpArchiveStorageObject->WritePkShort( 0 ); //internal attributes
            short int packed_attributes = entry.mpStorageObject->mAttributes.PackedAttributes();
            long int external_attributes = 0;
            if ( packed_attributes & ATTR_READ_ONLY )
                external_attributes |= 1;
            if ( packed_attributes & ATTR_SYSTEM )
                external_attributes |= 4;
            if ( packed_attributes & ATTR_HIDDEN )
                external_attributes |= 2;
            if ( packed_attributes & ATTR_ARCHIVE )
                external_attributes |= 0x20;
            if ( packed_attributes & ATTR_DIRECTORY )
                external_attributes |= 0x10;
            if ( packed_attributes & ATTR_LABEL )
                external_attributes |= 0x08;
            mpArchiveStorageObject->WritePkLong( external_attributes );
            mpArchiveStorageObject->WritePkLong( entry.mlCompressedObjectPosition );
            mpArchiveStorageObject->WriteBuffer( (unsigned char *) name, strlen( name ) );
            mpArchiveStorageObject->WriteBuffer( (unsigned char * ) comment, strlen( comment ) );
}

//
// NAME
//
//  ALPkGlArchive::PostWriteDir()
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
//  #include "pkglarc.h"
//
//  void ALPkGlArchive::PostWriteDir()
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
//  PKWare archives have a special end of central directory record
//  that needs to be added to the end of the archive.  It has a fair
//  amount of junk in it, but it is all pretty straightforward.
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
ALPkArchive::PostWriteDir()  /* Tag protected function */
{
//
// Now I'm going to write the end of central dir record
//
    long size_of_cdir = mpArchiveStorageObject->Tell() - mlStartOfCentralDir;
    mpArchiveStorageObject->WriteChar( 'P' );
    mpArchiveStorageObject->WriteChar( 'K' );
    mpArchiveStorageObject->WriteChar( 5 );
    mpArchiveStorageObject->WriteChar( 6 );
    mpArchiveStorageObject->WritePkShort( 0 );  //number of this disk
    mpArchiveStorageObject->WritePkShort( 0 );  //number of disk w/central dir
    mpArchiveStorageObject->WritePkShort( (short int) miCount );
    mpArchiveStorageObject->WritePkShort( (short int) miCount );
    mpArchiveStorageObject->WritePkLong( size_of_cdir );
    mpArchiveStorageObject->WritePkLong( mlStartOfCentralDir );
    const char *comment = mComment.GetSafeName();
    mpArchiveStorageObject->WritePkShort( (short int) strlen( comment ) );
    mpArchiveStorageObject->WriteBuffer( (unsigned char *) comment, strlen( comment ) );
}
