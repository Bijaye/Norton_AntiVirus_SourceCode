//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCAA.CPv   1.0   12 Mar 1998 12:14:52   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCAA.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:52   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCAA.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::Append()
//  ALArchiveAppendFromArchive()
//
// DESCRIPTION
//
//  This file contains the append from archive function.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
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
//  ALArchive::Append()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Append a list of objects from an input archive to an existing archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::Append( ALArchive &source_archive,
//                         ALEntryList &source_list )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveAppendFromArchive( hALArchive this_object,
//                                  hALArchive input_archive,
//                                  hALEntryList list );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveAppendFromArchive Lib "AL20LW"
//    (ByVal this_object&,
//     ByVal source_archive&,
//     ByVal source_list&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveAppendFromArchive( this_object : hALArchive;
//                                       source_archive : hALArchive;
//                                       source_list : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :   A reference or pointer to the ALArchive object that
//                   is going to have the batch of files appended to it.
//                   As always, the C++ version of this function doesn't
//                   need 'this_object' as an argument, since C++
//                   member functions get an explicit copy of this passed
//                   when they are called.
//
//  source_archive : A reference to the archive that contains the objects
//                   that are going to be added to this.  Since these
//                   files are already compressed, this function can
//                   really haul butt by just doing binary copies from the
//                   source archive to this_object.
//
//  list         :  A reference to the ALEntryList that has copies of
//                  all the storage objects to be inserted.  The list also
//                  has the offset of all the objects in the source archive,
//                  and their compressed sizes.
//
// DESCRIPTION
//
//  This function is called to insert a batch of objects into an archive.
//  You might have noticed by looking at the header file ARC.H that this
//  is not a virtual function, which means its behavior has to be
//  generalized enough to support both Greenleaf and PKWare archives.
//  All the specialization is done through the virtual functions that
//  control directory information, object positioning, and so on.
//
//  All of the bookwork is done in this function.  Note that in order
//  for this to work properly, we have to read in the entire directory
//  into memory, because we are going to append to an existing file, and
//  we assume the directory is at the end of the archive.  So the process
//  basically consists of first reading the directory, then copying
//  all the new jobs, then writing out the new combined directory.  The
//  copying is done in another function, CopyJobs().
//
// RETURNS
//
//  Upon failure, an ArchiveLib status code < 0.  When successful, a
//  count of the number of files appended to the archive >= 0.
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
//                              would return a count of files appended
//                              when successful.  In reality, it just
//                              returned AL_SUCCESS.  Returning a count seems
//                              like a good idea, and it's easier to change
//                              the code than the manual, so now the function
//                              agrees with the manual.

int AL_PROTO
ALArchive::Append( ALArchive AL_DLL_FAR &source_archive,  /* Tag public function */
                   ALEntryList AL_DLL_FAR &source_list )
{
    ALEntryList old_list( source_list );
//
// Open the storage object for this.
//
    ALOpenInputFile archive( *mpArchiveStorageObject );
//
// I read in the current directory for this archive.  I am going to
// write over the directory with new stuff, so I will have to write it
// back out later.
//
    ReadDirectory( old_list );
    if ( mStatus < 0 )
        return mStatus;
//
// The list of new objects I am going to add needs to be scanned for
// duplicates.  First I clear duplicate entries from the list itself.
// Then I clear any duplicates between the current list and the
// stuff already in the archive.
//
    source_list.UnmarkDuplicates( source_list, "Duplicate entry in list passed to Append()" );
    source_list.UnmarkDuplicates( old_list, "Duplicate entry in list passed to Append()" );
//
// Here is where I have to add new code to reflect the difference
// between the two versions of Append()
//
//
// Set up the monitor.
//
    source_list.mrMonitor.mlJobSoFar = 0L;
    if ( source_list.mrMonitor.miMonitorType == AL_MONITOR_JOB )
        source_list.mrMonitor.mlJobSize = CalculateCompressedJobSize( source_list );
    PreCreate();
//
// CopyJobs() takes care of actually adding the jobs to the archive.
//
    CopyJobs( source_archive, source_list );
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
        source_list.mrMonitor.ArchiveOperation( AL_ARCHIVE_CLOSE, this, 0 );
        return mStatus = mpArchiveStorageObject->mStatus;
    }
//
// Now I write the old directory out, and then add in the new
// directory entries.
//
    source_list.mrMonitor.ArchiveOperation( AL_START_DIRECTORY_WRITE, this, 0 );
    miCount = 0;
    PreWriteDir();
    ALEntry *job = old_list.GetFirstEntry();
    while ( job ) {
        if ( job->miMark ) {
            miCount++;
            WriteDirEntry( *job );
        }
        job = job->GetNextEntry();
    }
    job = source_list.GetFirstEntry();
    int append_count = 0;
    while ( job ) {
        if ( job->miMark ) {
            miCount++;
            append_count++;
            WriteDirEntry( *job );
        }
        job = job->GetNextEntry();
    }
    PostWriteDir();
//
// Update the monitor, check for errors, then leave.
//
    source_list.mrMonitor.ArchiveOperation( AL_END_DIRECTORY_WRITE, this, 0 );
    source_list.mrMonitor.ArchiveOperation( AL_ARCHIVE_CLOSE, this, 0 );
    ScanStatus( source_list );
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    return append_count;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveAppendFromArchive( hALArchive this_object,  /* Tag public function */
                            hALArchive input_archive,
                            hALEntryList list )
{
    AL_ASSERT( ((ALArchive *) this_object)->GoodTag(),
               "archive argument is not a valid ALArchiveBase" );
    AL_ASSERT( ((ALArchive *) input_archive)->GoodTag(),
               "input archive argument is not a valid ALArchiveBase" );
    AL_ASSERT( ((ALEntryList *) list)->GoodTag(),
               "list argument is not a valid ALEntryList" );
    return ((ALArchive *) this_object )->Append( *(ALArchive *) input_archive, *( (ALEntryList *) list ) );
}
#endif

#endif // #if !defined( AL_DEMO )

