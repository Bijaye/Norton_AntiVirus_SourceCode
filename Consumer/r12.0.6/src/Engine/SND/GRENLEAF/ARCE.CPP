//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCE.CPv   1.0   12 Mar 1998 12:14:50   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCE.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:50   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCE.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::Extract()
//  ALArchiveExtract()
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
//  ALArchive::Extract()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Extract objects from an archive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::Extract( ALEntryList AL_DLL_FAR &list )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveExtract( hALArchive this_object,
//                        hALEntryList list );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveExtract Lib "AL20LW"
//    (ByVal this_object&, ByVal list&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveExtract( this_object : hALArchive;
//                             list : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object that
//                  contains the storage objects to be extracted.  In C++,
//                  we bypass this argument, since the implicit 'this'
//                  pointer takes care of the same thing nicely.
//
//  list         :  An ALEntry list.  All of the marked objects in the
//                  list will be extracted.  Note that all the information
//                  in the list needs to be correct, e.g. file offset, CRC,
//                  etc.  If you did a ReadDir(), that probably took care
//                  of it for you.  Plus, the list needs to have good
//                  decompressors ready to go.
//
// DESCRIPTION
//
//  This routine just has to step through an entry list, and extract
//  every marked object.  Note that this isn't a virtual function, so
//  it needs has to be able to generalize between PK and GL archives.
//  It does so with the help of virtual functions like PreCompress()
//  that set things up for a specific archive type.
//
//  One big obstacle to extraction is the ability to set up a good
//  decompressor.  When reading in the archive list, ArchiveLib will
//  attempt to set up a decompressor that will work with your compression
//  method.  As long as that decompressor has been added to the toolkit
//  used to construct the list, you should be okay.
//
// RETURNS
//
//  Upon failure, an ArchiveLib status code < 0.  When successful, a
//  count of the number of files extracted from the archive >= 0.
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
ALArchive::Extract( ALEntryList AL_DLL_FAR &list )  /* Tag public function */
{
//
// Open the input storage object, if not already open.  Let the monitor
// know about it.
//
    ALOpenInputFile archive( *mpArchiveStorageObject );
    list.mrMonitor.ArchiveOperation( AL_ARCHIVE_OPEN, this, 0 );
//
// Get rid of any duplicate entries, and set up the monitor sizes.
//
//  Maybe it's okay?  The one thing that unmarking duplicates
//  screws up is a bunch of files with blank names.  The blank
//  name causes a temporary file to be created with a unique name,
//  but not until the file is created!
//
//    list.UnmarkDuplicates( list, "Duplicate entry in list passed to Extract()" );
    list.mrMonitor.mlJobSoFar = 0L;
    if ( list.mrMonitor.miMonitorType == AL_MONITOR_JOB )
        list.mrMonitor.mlJobSize = CalculateCompressedJobSize( list );
//
// This loop iterates through the entire ALEntryList.  We only care about
// ALEntry objects that have their mark set.
//
    int extract_count = 0;
    ALEntry *job = list.GetFirstEntry();
    while ( job ) {
        if ( mfStripPathOnExtract )
            job->mpStorageObject->mName.StripPath();
        if ( job->miMark ) {
//
// Go to the correct input position in this, and set up the monitor for
// this particular object.
//
            extract_count++;
            list.mrMonitor.ArchiveOperation( AL_EXTRACTION_OPEN, this, job );
            mpArchiveStorageObject->Seek( job->mlCompressedObjectPosition );
            PreDecompress( *job );
            list.mrMonitor.mlObjectStart = mpArchiveStorageObject->Tell();
            list.mrMonitor.mlObjectSize = job->mlCompressedSize;
            mpArchiveStorageObject->mpMonitor = &list.mrMonitor;
//
// Extract it, then check the CRC.
//
            if ( !job->mpDecompressor )
                return mStatus.SetError(
                    AL_CANT_CREATE_ENGINE,
                    "Attempt to decompress without a decompressor" );
            job->mpStorageObject->Create( job->mpStorageObject->mlSize );
            job->mpDecompressor->Decompress( *mpArchiveStorageObject,
                                             *job->mpStorageObject,
                                             job->mlCompressedSize );
            job->mpStorageObject->Close();
            PostDecompress( *job );
            if ( job->mpStorageObject->GetCrc32() != job->GetCrc32() )
                job->mpStorageObject->mStatus.SetError(
                        AL_CRC_ERROR,
                        "CRC32 was supposed to be %08lx, was %08lx",
                        job->GetCrc32(),
                        job->mpStorageObject->GetCrc32() );
//
// Update the monitor data, and yield some time. Note that I turn off
// the monitor at this point so it doesn't jump around while I seek to the
// next position in the archive.
//
//
// The following two lines were reversed in 2.0.  Updating JobSoFar
// before individual file stats were done causes a big jump ahead
// in total progress, but it isn't warranted.
//
            mpArchiveStorageObject->YieldTime();
            list.mrMonitor.mlJobSoFar  += job->mlCompressedSize;
            mpArchiveStorageObject->mpMonitor = 0;
            list.mrMonitor.ArchiveOperation( AL_EXTRACTION_CLOSE, this, job );
            job->mpStorageObject->mpMonitor = 0;
        }
        job = job->GetNextEntry();
    }
//
// Update the monitor, then scan the list for status errors.
//
    list.mrMonitor.ArchiveOperation( AL_ARCHIVE_CLOSE, this, 0 );
    ScanStatus( list );
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    return extract_count;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveExtract( hALArchive this_object,  /* Tag public function */
                  hALEntryList list )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveExtract" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALArchiveExtract" );
    return ((ALArchive *) this_object )->Extract( *( (ALEntryList *) list ) );
}

#endif

