//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCD.CPv   1.0   12 Mar 1998 12:14:50   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCD.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:50   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCD.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::Delete()
//  ALArchiveDelete()
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
//  ALArchive::Delete()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Delete a number of objects from an archive
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::Delete( ALEntryList &list,
//                         ALArchive &destination );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveDelete( hALArchive this_object,
//                       hALEntryList list,
//                       hALArchive output_archive );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveDelete Lib "AL20LW"
//    (ByVal this_object&, ByVal list&, ByVal object_archive&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveDelete( this_object : hALArchive;
//                            list : hALEntryList;
//                            output_archive : hALArchive ) : Integer;
//
// ARGUMENTS
//
//  this_object    :  A reference or pointer to the ALArchive object that
//                    is going to have some objects deleted.  The C++
//                    version of this call doesn't have an argument called
//                    this_object, because it has implicit access to this,
//                    being a member function and all.
//
//  list           :  The list of objects that are supposed to be removed
//                    from this_object.  Remeber that only items that have
//                    their marks set will actually be deleted.
//
//  output_archive :  Delete() doesn't actually modify the existing archive.
//                    Instead, it creates a new archive that has all the
//                    desired objects deleted.  The original archive is
//                    unchanged after the operation.  So anyway, this output
//                    archive is where the whole del gets copied.
//
// DESCRIPTION
//
//  This function is used to delete objects from an archive.  Most everyone
//  knows how they want this to work.  You delete objects from an archive
//  and presto, the same archive now exists in a slim-trim new version.
//  This function implements that, but there is some trickery needed to make
//  it happen.
//
//  The trickery is needed because we don't really delete objects from the
//  existing archive.  Instead, we perform the deletion by copying selected
//  objects to the output archive.  When that's done, the output archive
//  looks like the one you want.  Unfortunately, it probably has the wrong
//  name at this point.
//
//  So, to get around this problem, after the Delete() operation we do one
//  last thing.  The Rename() functions are called on to rename the original
//  input archive to a backup name.  The output file is then renamed to have
//  the same name as the input file.
//
//  This all works great if your output archive and your input archive are
//  both in the same directory.  It even works properly if the input and
//  output are simply on the same volume. But... the rename of the
//  output archive will fail if the input and output are on different
//  volumes.  That's because the rename() C RTL function won't move a file
//  from one volume to another.
//
// RETURNS
//
//  Upon failure, an ArchiveLib status code < 0.  When successful, a
//  count of the number of files deleted from the archive >= 0.
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
//                              would return a count of files deleted
//                              when successful.  In reality, it just
//                              returned AL_SUCCESS.  Returning a count seems
//                              like a good idea, and it's easier to change
//                              the code than the manual, so now the function
//                              agrees with the manual.

int AL_PROTO
ALArchive::Delete( ALEntryList AL_DLL_FAR &list,  /* Tag public function */
                   ALArchive AL_DLL_FAR &output_archive )
{
    output_archive.SetComment( mComment );
    list.ToggleMarks();
    output_archive.Create( *this, list );
    list.ToggleMarks();
    ALName temp = mpArchiveStorageObject->mName;
    mpArchiveStorageObject->RenameToBackup();
    output_archive.mpArchiveStorageObject->Rename( (const char *) temp );
    if ( output_archive.mStatus < 0 )
        return mStatus = output_archive.mStatus;
    if ( mStatus < AL_SUCCESS )
        return mStatus;
    ALEntry *entry = list.GetFirstEntry();
    int delete_count = 0;
    while ( entry ) {
        if ( entry->miMark )
            delete_count++;
        entry = entry->GetNextEntry();
    }
    return delete_count;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveDelete( hALArchive this_object,  /* Tag public function */
                 hALEntryList list,
                 hALArchive output_archive )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveDelete" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALArchiveDelete" );
    AL_ASSERT_OBJECT( output_archive, ALArchive, "IALArchiveDelete" );
    return ((ALArchive *) this_object )->Delete( *( (ALEntryList *) list ), *( (ALArchive *) output_archive ) );
}

#endif
