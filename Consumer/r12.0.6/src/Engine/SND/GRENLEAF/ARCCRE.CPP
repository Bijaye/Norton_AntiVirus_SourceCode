//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCCRE.CPv   1.0   12 Mar 1998 12:14:46   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCCRE.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:46   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCCRE.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::Create()
//  ALArchiveCreate()
//
// DESCRIPTION
//
//  Two different versions of the Archive creation function.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "_openf.h"

//
// NAME
//
//  ALArchive::Create()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create a new archive from a list of objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::Create( ALEntryList &list )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveCreate( hALArchive this_object, hALEntryList list )
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveCreate Lib "AL20LW"
//    (ByVal this_object&, ByVal list&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveCreate( this_object : hALArchive;
//                            list : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object that
//                  is going to be created.  As always, the C++ version
//                  of this function is a member function, and therefore
//                  doesn't need this argument, since it has implicit
//                  access via this.
//
//  list         :  An ALEntryList that has a batch of storage objects
//                  and compression engines.  Every object in the list
//                  whose mark is set will be added to the output archive
//                  using that entry's compression engine.
//
// DESCRIPTION
//
//  This function creates a new archive, using a list of objects
//  as its input.  It's important to note that this isn't a virtual
//  function, so the base class has to have enough intelligence to
//  be able to add files to *both* types of archives.  It does this
//  through selective use of virtual functions that update directory
//  information, and position output pointers, and all that stuff.
//
//  This function takes care of setting everything up for the compression
//  act, but it holds off on actually doing the compression.  It instead
//  relies on CompressJobs() to do that.
//
// RETURNS
//
//  Upon failure, an ArchiveLib status code < 0.  When successful, a
//  count of the number of files inserted into the archive >= 0.
//
//  Note that after the compression is complete, this function does a
//  ScanStatus, which checks each element in the list.  If any of the
//  objects failed to compress for some reason, ScanStatus will take
//  care of modifying the mStatus member to reflect that.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//
//   April 3, 1996      2.01A : Our documentation said that this function
//                              would return a count of files inserted
//                              when successful.  In reality, it just
//                              returned AL_SUCCESS.  Returning a count seems
//                              like a good idea, and it's easier to change
//                              the code than the manual, so now the function
//                              agrees with the manual.


int AL_PROTO
ALArchive::Create( ALEntryList AL_DLL_FAR &list )  /* Tag public function */
{
//
// Miscellaneous: open the archive, set the archive version, initialize
// the monitor.  If the storage object is broken, quit now!
//
    ALOpenOutputFile archive( *mpArchiveStorageObject );
    list.mrMonitor.ArchiveOperation( AL_ARCHIVE_OPEN, this, 0 );
    if ( mpArchiveStorageObject->mStatus < 0 )
        return mStatus = mpArchiveStorageObject->mStatus;
//
// We don't want to create an archive with duplicate entries, so we check here.
//
    list.UnmarkDuplicates( list,
                           "Duplicate entry in list passed to Create()" );
//
// We don't want to try to insert an archive into itself.  This might not
// be the best check in the world, but I think I need it for right now.
// Ultimately this code should be moved into a virtual function and
// shared between the create and append functions
//
    if ( mpArchiveStorageObject->miStorageObjectType == AL_FILE_OBJECT )
        list.ClearMarks( mpArchiveStorageObject->mName );
    PreCreate();
//
// Set up the monitor.
//
    list.mrMonitor.mlJobSoFar = 0L;
    if ( list.mrMonitor.miMonitorType == AL_MONITOR_JOB )
        list.mrMonitor.mlJobSize = CalculateJobSize( list );
    miCount = 0;
//
// CompressJobs() takes care of actually adding the jobs to the archive.
//
    if ( CompressJobs( list ) < 0 )
        return mStatus;
//
// All the jobs are written, now I can figure out where the
// directory is in the storage object.  I copy it, then write
// it out to the storage object at position 0.
//
    mlDirectoryOffset = mpArchiveStorageObject->Tell();
//
// Once I know the directory offset, it's safe to do
// a PostCreate().
//
    PostCreate();
//
// Return without writing the directory if there is an error in the
// archive storage object.
//
    if ( mpArchiveStorageObject->mStatus < 0 ) {
        list.mrMonitor.ArchiveOperation( AL_ARCHIVE_CLOSE, this, 0 );
        return mStatus = mpArchiveStorageObject->mStatus;
    }
//
// Finally, write out the directory to the storage object.
//
    list.mrMonitor.ArchiveOperation( AL_START_DIRECTORY_WRITE, this, 0 );
    WriteDirectory( list );
//
// Update the monitor, check for errors, and blow.
//
    list.mrMonitor.ArchiveOperation( AL_END_DIRECTORY_WRITE, this, 0 );
    list.mrMonitor.ArchiveOperation( AL_ARCHIVE_CLOSE, this, 0 );
    ScanStatus( list );
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    return miCount;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveCreate( hALArchive this_object, hALEntryList list )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveCreate" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALArchiveCreate" );
    return ((ALArchive *) this_object )->Create( *( (ALEntryList *) list ) );
}

#endif
