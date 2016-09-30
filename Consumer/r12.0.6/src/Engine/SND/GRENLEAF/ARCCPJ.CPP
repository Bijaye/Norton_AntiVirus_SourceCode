//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCCPJ.CPv   1.0   12 Mar 1998 12:14:48   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCCPJ.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:48   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCPJ.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::CopyJobs()
//
// DESCRIPTION
//
//  An internal support routine for ArchiveLib.
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
//  ALArchive::CopyJobs()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Copies an entire batch of objects from one archive to another.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::CopyJobs( ALArchive &source_archive,
//                           ALEntryList &source_list );
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
//  source_archive : This is a reference to an existing archive that holds
//                   a bunch of compressed objects.  This function is going
//                   to take selected items from that archive and copy them
//                   into this.
//
//  list          :  This is an ALEntryList that refers to a batch of storage
//                   objects found in the source_archive.  They are already
//                   compressed, so we don't even need the engines that are
//                   probably in the list.  What I do need is the compressed
//                   length and offsets of the objects.
//
// DESCRIPTION
//
//  When Append() or Create() is called to copy a batch of storage
//  objects from one archive to another, they call this routine to do the
//  easy part, which is copying the data into the output.  All
//  we have to do here is iterate through the input list, copying
//  all the marked jobs, and updating the various stats for each job
//  in the entry list, such as the offset.
//
//  This isn't a virtual function, so it has to be universal enough to
//  work on both PK and GL archives.  It handles the differences between
//  the two mostly by using the PreCompress() and PostCcompress() virtual
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
ALArchive::CopyJobs( ALArchive AL_DLL_FAR &source_archive,  /* Tag protected function */
                     ALEntryList AL_DLL_FAR &source_list )
{
//
// Open the storage object attached to the input archive.  The storage object
// attached to this is already open.
//
    ALOpenInputFile input( *(source_archive.mpArchiveStorageObject) );
 //
 // This loop iterates through all of the entries in the list, picking off
 // only the marked entries.
 //
    ALEntry *job = source_list.GetFirstEntry();
    while ( job ) {
        if ( job->miMark ) {
//
// We need to keep track of the position in the archive where the compressed
// data is going to go.
//
            miCount++;
            source_archive.mpArchiveStorageObject->Seek( job->mlCompressedObjectPosition );
            source_archive.PreCopyInput( *job );
            job->mlCompressedObjectPosition = mpArchiveStorageObject->Tell();
            PreCompress( *job );
//
// Attach the monitor to the storage object that is going to be inserted
// in the archive.
//
            source_list.mrMonitor.mlObjectStart = source_archive.mpArchiveStorageObject->Tell();
            source_list.mrMonitor.mlObjectSize = job->mlCompressedSize;
            source_archive.mpArchiveStorageObject->mpMonitor = &source_list.mrMonitor;
            source_list.mrMonitor.ArchiveOperation( AL_COPY_OPEN, this, job );
//
// Copy compressed data here
//
            for ( long l = 0 ; l < job->mlCompressedSize; l++ ) {
                int c = source_archive.mpArchiveStorageObject->ReadChar();
                mpArchiveStorageObject->WriteChar( c );
            }
//
// This should shoot the monitor to the end of the line
//
            source_archive.mpArchiveStorageObject->YieldTime();
            source_archive.mpArchiveStorageObject->mpMonitor = 0;
//
// This doesn't make any sense.  How could the compression engine
// have a bad status?
//
//            if ( job->mpCompressionEngine->mStatus < 0 )
//                return mStatus = job->mpCompressionEngine->mStatus;
            source_archive.PostCopyInput( *job );
            PostCompress( *job );
//
// Update the monitor now that the copy is complete.
//
            source_list.mrMonitor.ArchiveOperation( AL_COPY_CLOSE, this, job );
            source_list.mrMonitor.mlJobSoFar += job->mlCompressedSize;
            source_archive.mpArchiveStorageObject->mpMonitor = 0;
            if ( source_archive.mpArchiveStorageObject->mStatus < 0 )
                return mStatus = source_archive.mpArchiveStorageObject->mStatus;
            if ( mpArchiveStorageObject->mStatus < 0 )
                return mStatus = mpArchiveStorageObject->mStatus;
        }
        job = job->GetNextEntry();
        if ( mStatus < 0 )
            break;
    }
    return mStatus;
}

