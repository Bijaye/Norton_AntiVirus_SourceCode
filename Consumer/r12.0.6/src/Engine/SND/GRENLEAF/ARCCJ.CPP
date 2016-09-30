//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCCJ.CPv   1.0   12 Mar 1998 12:14:48   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCCJ.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:48   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCCJ.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::CompressJobs()
//
// DESCRIPTION
//
//  This is an internal archiving routine.
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
//  ALArchive::CompressJobs()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Compress an entire batch of input files into the Archive storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::CompressJobs( ALEntryList &list )
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
//  list  :  This is an ALEntryList that refers to a batch of storage
//           objects and compression engines.  The objects are inserted,
//           one by one, into the mpStorage object of this, using the
//           compression engine supplied for each.
//
// DESCRIPTION
//
//  When Append() or Create() is called to shove a bunch of storage
//  objects into an archive, they call this routine to do the
//  easy part, which is compressing the data into the output.  All
//  we have to do here is iterate through the input list, compressing
//  all the marked jobs, and updating the various stats for each job
//  in the entry list, such as compressed size, offset, crc, and so on.
//
//  This isn't a virtual function, so it has to be universal enough to
//  work on both PK and GL archives.  It handles the differences between
//  the two mostly by using the PreCreate() and PostCreate() virtual
//  functions before and after each job gets stuffed.
//
// RETURNS
//
//  An ArchiveLib status code, hopefully AL_SUCCESS.  Note that if any
//  of the objects has a failure, we can pick that up later, when we
//  scan all the input jobs..
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
ALArchive::CompressJobs( ALEntryList AL_DLL_FAR &list )  /* Tag protected function */
{
    list.mrMonitor.mlObjectStart = 0L; // This will be true for all input jobs
 //
 // This loop iterates through all of the entries in the list, picking off
 // only the marked entries.
 //
    ALEntry *job = list.GetFirstEntry();
    while ( job ) {
        if ( job->miMark ) {
//
// We need to keep track of the position in the archive where the compressed
// data is going to go.
//
            miCount++;
            job->mlCompressedObjectPosition = mpArchiveStorageObject->Tell();
//
// Attach the monitor to the storage object that is going to be inserted
// in the archive.
//
            list.mrMonitor.ArchiveOperation( AL_INSERTION_OPEN, this, job );
            list.mrMonitor.mlObjectSize = -1L; // This means we ask for it in ALMonitor, after the object is opened
            job->mpStorageObject->mpMonitor = &list.mrMonitor;
//
// Compress the object into the archive.  Then store the resulting CRC
// the compressed size in the ALEntry object.
//
            if ( job->mpCompressor == 0 )
                return mStatus.SetError( AL_CANT_CREATE_ENGINE,
                                         "No compression engine for storage object" );
            if ( job->mpStorageObject == 0 )
                return mStatus.SetError( AL_CANT_CREATE_STORAGE_OBJECT,
                                         "No storage object for compression engine" );
            PreCompress( *job );
            job->mpCompressor->Compress( *job->mpStorageObject,
                                         *mpArchiveStorageObject );
            job->mlCrc32 = job->mpStorageObject->GetCrc32();
            job->mpStorageObject->mpMonitor = 0;
            if ( job->mpCompressor->mStatus < 0 )
                return mStatus = job->mpCompressor->mStatus;
            job->mlCompressedSize = mpArchiveStorageObject->Tell() -
                                    job->mlCompressedObjectPosition;
            PostCompress( *job );
//
// Update the monitor
//
            list.mrMonitor.mlJobSoFar += job->mpStorageObject->GetSize();
            list.mrMonitor.ArchiveOperation( AL_INSERTION_CLOSE, this, job );
        }
        job = job->GetNextEntry();
        if ( mStatus < 0 )
            break;
    }
    return mStatus;
}

