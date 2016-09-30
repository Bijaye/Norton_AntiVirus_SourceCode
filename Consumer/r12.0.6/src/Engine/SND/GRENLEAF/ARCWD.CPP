//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCWD.CPv   1.0   12 Mar 1998 12:14:50   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCWD.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:50   DCHI
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
//  ALArchive::WriteDirectory()
//  ALArchiveWriteDirectory()
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
//  ALArchive::WriteDirectory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Writes the directory out to the archive object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchive::WriteDirectory( ALEntryList &list );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveWriteDirectory( hALArchive this_object,
//                               hALEntryList list );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveWriteDirectory Lib "AL20LW"
//    (ByVal this_object&, ByVal list&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveWriteDirectory( this_object : hALArchive;
//                                    list : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object that
//                  is going to have its directory updated.  The C++
//                  member function version gets by without a copy of
//                  this_object, since it has the implicit 'this' pointer
//                  to rely on.
//
// DESCRIPTION
//
//  An archive is pretty simple.  It has a bunch of compressed objects, and
//  a directory.  You can read the entire directory in an archive into an
//  ALEntryList using the ReadDirectory() command.  You can then jack with
//  the entries in the list as much as you want, then write the directory
//  back out with a WriteDirectory() command.
//
//  It probably wouldn't be a good idea to change some of the items in the
//  directory.  For example, the checksum of the object, it's compression
//  method, and its location in the archive are all inviolate.  On the
//  other hand, changing the object name, its protection, comment or
//  timestamp all make sense.
//
//  This function is used by internal functions in the library, but you
//  are welcome to use it as well.  Just be sure that the ALEntryList you
//  are using is one that makes sense for your Archive.  In particular,
//  you probably should have read in the ALEntryList from the archive in
//  advance.
//
// RETURNS
//
//  The status of the archive.  If things are going well, this should
//  be AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : First release.  This used to be an archive
//                             specific function.  Now it is done using
//                             this function with a mix of virtual helpers.

#include "_openf.h"

int AL_PROTO
ALArchive::WriteDirectory( ALEntryList AL_DLL_FAR &list )  /* Tag public function */
{
    ALOpenInputFile archive( *mpArchiveStorageObject );

    miCount = 0;
    mpArchiveStorageObject->Seek( mlDirectoryOffset );

    PreWriteDir();

    ALEntry *job = list.GetFirstEntry();
    while ( job ) {
        if ( job->miMark ) {
            miCount++;
            if ( mfStripPathOnInsert )
                job->mpStorageObject->mName.StripPath();
            WriteDirEntry( *job );
        }
        job = job->GetNextEntry();
    }
    PostWriteDir();
    return mStatus;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveWriteDirectory( hALArchive this_object,  /* Tag public function */
                         hALEntryList list )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveWriteDirectory" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALArchiveWriteDirectory" );
    return ( (ALArchive *) this_object )->WriteDirectory( *( (ALEntryList *) list ) );
}

#endif
