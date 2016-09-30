//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCA.CPv   1.0   12 Mar 1998 12:14:52   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCA.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:52   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCA.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::Append()
//  ALArchiveAppend()
//
// DESCRIPTION
//
//  This file contains the archive Append function.
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
//  ALArchive::Append()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Append a list of objects to an existing archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::Append( ALEntryList AL_DLL_FAR &list )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveAppend( hALArchive this_object, hALEntryList list )
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveAppend Lib "AL20LW" (ByVal this_object&, ByVal list&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveAppend( this_object : hALArchive;
//                            list : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object that
//                  is going to have the list of files appended to it.
//                  As always, the C++ version of this function doesn't
//                  need 'this_object' as an argumnent, since C++
//                  member functions get an explicit copy of this passed
//                  when they are called.
//
//  list         :  A reference to the ALEntryList that has copies of
//                  all the storage objects to be inserted, as well
//                  as the compression engines need to insert them.
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
//  basically consists of first reading the directory, then compressing
//  all the new jobs, then writing out the new combined directory.  The
//  compresssion is done in another function, CompressJobs().
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
//   February 14, 1996  2.0A  : New release
//
//   April 3, 1996      2.01A : Our documentation said that this function
//                              would return a count of files appended
//                              when successful.  In reality, it just
//                              returned AL_SUCCESS.  Returning a count seems
//                              like a good idea, and it's easier to change
//                              the code than the manual, so now the function
//                              agrees with the manual.

int AL_PROTO
ALArchive::Append( ALEntryList AL_DLL_FAR &list )  /* Tag public function */
{
    ALEntryList old_list(list);
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
    list.UnmarkDuplicates( list, "Duplicate entry in list passed to Append()" );
    list.UnmarkDuplicates( old_list, "Duplicate entry in list passed to Append()" );
//
// We don't want to try to insert an archive into itself.  This might not
// be the best check in the world, but I think I need it for right now.
// Ultimately this code should be moved into a virtual function and
// shared between the create and append functions
//
    if ( mpArchiveStorageObject->miStorageObjectType == AL_FILE_OBJECT )
        list.ClearMarks( mpArchiveStorageObject->mName );
//
// I get the monitor set up, for the batch of entries I am about to do.
//
    list.mrMonitor.ArchiveOperation( AL_ARCHIVE_OPEN, this, 0 );
    list.mrMonitor.mlJobSoFar = 0L;
    if ( list.mrMonitor.miMonitorType == AL_MONITOR_JOB )
        list.mrMonitor.mlJobSize = CalculateJobSize( list );
//
// The new entries start at the position currently occupied by the
// directory.  I seek to that point, then call AddJobs() to do the
// dirty work.
//
    mpArchiveStorageObject->Seek( mlDirectoryOffset );
    CompressJobs( list );
    mlDirectoryOffset = mpArchiveStorageObject->Tell();
    if ( mStatus < 0 ) {
        list.mrMonitor.ArchiveOperation( AL_ARCHIVE_CLOSE, this, 0 );
        return mStatus;
    }
//
// Now I write the old directory out, and then add in the new
// directory entries.
//
    list.mrMonitor.ArchiveOperation( AL_START_DIRECTORY_WRITE, this, 0 );
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
    job = list.GetFirstEntry();
    int append_count = 0; //2.01a
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
    list.mrMonitor.ArchiveOperation( AL_END_DIRECTORY_WRITE, this, 0 );
    list.mrMonitor.ArchiveOperation( AL_ARCHIVE_CLOSE, this, 0 );
    ScanStatus( list );
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    return append_count;
 }

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveAppend( hALArchive this_object, hALEntryList list )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveAppend" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALArchiveAppend" );
    return ((ALArchive *) this_object )->Append( *( (ALEntryList *) list ) );
}

#endif

