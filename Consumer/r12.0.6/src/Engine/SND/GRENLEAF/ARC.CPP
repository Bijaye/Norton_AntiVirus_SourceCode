//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARC.CPv   1.0   12 Mar 1998 12:14:52   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:52   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::ALArchive( ALStorage * )
//  ALArchive::operator new()
//  ALArchive::ScanStatus()
//  ALArchive::CalculateJobSize()
//  ALArchive::CalculateCompressedJobSize()
//  ALArchive::WriteArchiveData()
//  ALArchive::ReadArchiveData()
//  ALArchive::~ALArchive()
//  deleteALArchive()
//
// DESCRIPTION
//
//  This file contains the ALArchive ctor, dtor, and any functions
//  that will *always* be linked (mostly because they are virtual.)
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALArchive::ALArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Constructor for ALArchive, the base Archive class.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALArchive::ALArchive( ALStorage *storage_object,
//                        short int delete_in_dtor )
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
//  storage_object :  A pointer to the storage object that will contain
//                    the archive.
//
//  delete_in_dtor :  When a user creates an archive, it will ordinarily
//                    be with a storage object that will be managed by
//                    their code.  When that's the case, we will ordinarily
//                    expect them to be responsible for its destruction.
//                    However, there are times when a temporary archive
//                    might be created that isn't under the users control.
//                    This flag controls whether the Archive destructor
//                    will destroy the storage object as well.
//
// DESCRIPTION
//
//  The base class ALArchive is very simple.  It has a few data members
//  to initialize, and not much else.  The complexity in Archive classes
//  is found in the derived classes ALGlArchive and ALPkArchive.  Both
//  of them use this base class to initialize their storage object
//  and the flag that indicates whether the storage object should be
//  destroyed in the ALArchive destructor.
//
//  Although this is a public function, it isn't likely that there will
//  ever be a good reason for an end user to call it.  It really isn't much
//  good to anyone but the constructors for the archiving classes.
//
//  For this reason, there also isn't any reason to have a C/VB/Delphi
//  translation routine.  Any attempt to construct an ALArchive by itself
//  is doomed to failure anyway, since it has numerous pure virtual
//  functions.
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
//   February 14, 1996  2.0A : New release, added the members to control
//                             whether or not to strip path information.
//

AL_PROTO
ALArchive::ALArchive( ALStorage AL_DLL_FAR *storage_object,  /* Tag public function */
                      short int delete_in_dtor )
    : mfDeleteStorageObject( delete_in_dtor )
{
    mfStripPathOnInsert = 0;
    mfStripPathOnExtract = 0;
    mpArchiveStorageObject = storage_object;
    mlDirectoryOffset = -1L;
    miVersion = -1;
}

//
// NAME
//
//  ALArchive::operator new()
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
//           necessary to construct a new ALArchive object.
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
ALArchive::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALArchive::ScanStatus()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Scan the list of entries for errors after an archiving operation
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALArchive::ScanStatus( ALEntryList &list )
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
//  list  :  The list of entries that have just been processed, in one of
//           the exciting functions, such as Create(), Delete(), etc.
//
// DESCRIPTION
//
//  After an archive operation, I use this function to update the
//  status member of the archive.  If the archive doesn't already
//  have an error, I check through all the storage objects and
//  compression engines to see if any of them hosed up.  Any error
//  of any sort by any of them is copied into the archive status.
//  The whole point of this is to ensure that if
//  ALArchive.mStatus == AL_SUCCESS, it means everything worked.
//
// RETURNS
//
//  None.  This function sort of has a return, it will update
//  the member mStatus with an error code if one is found.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

void AL_PROTO
ALArchive::ScanStatus( ALEntryList AL_DLL_FAR &list ) /* Tag protected function */
{
    if ( mStatus < AL_SUCCESS )
        return;
    ALEntry *job = list.GetFirstEntry();
    while ( job ) {
        if ( job->mpStorageObject->mStatus < AL_SUCCESS ) {
            mStatus.SetError( job->mpStorageObject->mStatus,
                              "%s: %s",
                              job->mpStorageObject->mName.GetSafeName(),
                              job->mpStorageObject->mStatus.GetStatusDetail() );
            return;
        }
        job = job->GetNextEntry();
    }
}

//
// NAME
//
//  ALArchive::CalculateJobSize()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Calculate the total number of bytes in a job.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALArchive::CalculateJobSize( ALEntryList &list )
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
//  list  :  The list of entries in the job.  The calculation function
//           will iterator through this list.
//
// DESCRIPTION
//
//  If a monitor is running in AL_MONITOR_JOB mode, we need to add up
//  the sizes of all the storage objects we are going to process, so
//  that we can accurately track our progress from 0 to 100%.  In many
//  cases, the sizes of all the files will not yet be known, which means
//  this routine will have to open the files up and check the values.
//  That is why we only call this routine when we have to.
//
// RETURNS
//
//  This function is used to calculate the total number of bytes that
//  are going to have to be moved when performing a Create() or Append()
//  operation.  We need that info in order to set up a monitor properly
//  when its mode is AL_MONITOR_JOB.  Naturally, we don't really care
//  about the total size when the monitor is in AL_MONITOR_OBJECTS mode.
//  Anyway, it returns the total unocmpressed size of all the objects added up.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

long AL_PROTO
ALArchive::CalculateJobSize( ALEntryList AL_DLL_FAR &list ) /* Tag protected function */
{
    long total = 0;
    ALEntry *job = list.GetFirstEntry();
    while ( job ) {
        if ( job->miMark ) {
            long obj_size;
            if ( ( obj_size = job->mpStorageObject->GetSize() ) == -1 ) {
                job->mpStorageObject->Open();
                obj_size = job->mpStorageObject->GetSize();
                job->mpStorageObject->Close();
                if ( obj_size == -1 )
                    return -1;
            }
            total += obj_size;
        }
        job = job->GetNextEntry();
    }
    return total;
}

//
// NAME
//
//  ALArchive::CalculateCompressedJobSize()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Calculate the total number of bytes in a job.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALArchive::CalculateCompressedJobSize( ALEntryList &list )
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
//  list  :  The list of entries in the job.  The calculation function
//           will iterator through this list.
//
// DESCRIPTION
//
//  When we are monitoring an Extract() command, the monitor object
//  gets attached to the Archive, not to the objects that are getting
//  sucked out of it.  This means that progress is being measured
//  against the compressed objects, not the true size objects.  So
//  before I start the extract, I call this function to see just how
//  much compressed space is taken up by the compressed objects in
//  the archive.
//
// RETURNS
//
//  The total size of a bunch of compressed objects, not the uncompressed
//  size.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

long AL_PROTO
ALArchive::CalculateCompressedJobSize( ALEntryList AL_DLL_FAR &list ) /* Tag protected function */
{
    long total = 0;
    ALEntry *job = list.GetFirstEntry();
    while ( job ) {
        if ( job->miMark ) {
            if ( job->mlCompressedSize  == -1 )
                return -1;
            else
                total += job->mlCompressedSize;
        }
        job = job->GetNextEntry();
    }
    return total;
}

//
// NAME
//
//  ALArchive::WriteArchiveData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Write out any customized archive data.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::WriteArchiveData()
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
//  None.
//
// DESCRIPTION
//
//  Derived classes can write out customized archive data, for whatever
//  reasons they deem necessary.  Our base class has nothing that it
//  needs to save, so it just writes out a zero length string, which takes
//  two bytes to save.  Instead of using WriteString like I ought to, for
//  some reason I write the 0 out directly.
//
// RETURNS
//
//  AL_SUCCESS if everything writes out okay, or < AL_SUCCESS for trouble.
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
ALArchive::WriteArchiveData()  /* Tag protected function */
{
    return mpArchiveStorageObject->WriteGlShort( 0 );
}

//
// NAME
//
//  ALArchive::ReadArchiveData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read in any customized archive data.
//
// C++ SYNOPSIS
//
//  int ALArchive::ReadArchiveData()
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
//  None.
//
// DESCRIPTION
//
//  The base class doesn't store anything in the archive specific
//  data area.  That means that when I am reading the archive specific
//  data in, I should see a zero length string, which is the same thing
//  as a single short of value 0.  I read it in and verify it here.
//
//  Note that derived classes are free to override this function, but
//  nothing we ship with ArchiveLib does so.
//
// RETURNS
//
//  AL_SUCCESS if things went well, < AL_SUCCESS it things go sour.
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
ALArchive::ReadArchiveData()  /* Tag protected function */
{
    short temp;
    mpArchiveStorageObject->ReadGlShort( temp );
    AL_ASSERT( temp == 0, "ReadArchiveData(): archive data != 0" );
    return mpArchiveStorageObject->mStatus;
}

//
// NAME
//
//  ALArchive::~ALArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The base class destructor for ALArchive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//   ALArchive::~ALArchive()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void deleteALArchive( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALArchive Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALArchive( this_object : hALArchive );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object that
//                  is going to be destroyed.  The C++ version of this
//                  function doesn't have an argument, because it knows
//                  implicitly who to destroy.
//
// DESCRIPTION
//
//  The destructor for the base class doesn't have to do too much work.
//  All it has to do is destroy the storage object, if that service was
//  requested when the object was constructed.
//
//  It's a little deceptive to show the C/VB/Delphi translation here
//  as well.  Since the archive being destroyed will always be in a base
//  class, the virtual function call will direct a call to deleteALArchive
//  down to the derived class destructor before coming back up here.  But
//  I don't really have a better place to put it.
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
ALArchive::~ALArchive()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALArchive(): Attempting to delete invalid ALArchive" );
    if ( mpArchiveStorageObject && mfDeleteStorageObject )
        delete mpArchiveStorageObject;
    AL_ASSERT( GoodTag(), "~ALArchive::Attempting to delete invalid ALArchive" );
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALArchive( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "deleteALArchive()" );
    delete (ALArchive *) this_object;
}

#endif
