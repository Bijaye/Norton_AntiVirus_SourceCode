//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/SIMPEXT.CPv   1.0   12 Mar 1998 12:17:54   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/SIMPEXT.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:54   DCHI
// Initial revision.
// 
//************************************************************************

//
// SIMPEXT.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALExtract()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//   February 26, 1996  2.0B : Fixed a bad bug in ALExtract()
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "alsimple.h"
#include "simpmon.h"
#include "pkarc.h"
#include "pkengn.h"
#include "filestor.h"
#include "copyengn.h"

//
// NAME
//
//  ALExtract()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The simplified interface function to extract files from a ZIP file.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  int ALExtract( ALZipDir *z,
//                 int strip_path,
//                 CALLBACK_FN callback )
//
// VB SYNOPSIS
//
//  See arclib.bas for the equivalent VB function.
//
// DELPHI SYNOPSIS
//
//  See arclib.pas for the equivalent Delphi function.
//
// ARGUMENTS
//
//  z             :  The ALZipDir array.  This is the array you will have
//                   have read in using ALReadDir().  Every entry in the
//                   array that has a mark set will be extracted.
//
//  strip_path    :  If this flag is set, the files that are extracted
//                   from the archive will have their paths stripped
//                   before the extraction takes place.
//
//  callback      :  C and C++ use a callback function for monitoring.
//                   This function gets called periodically with
//                   file names and progress ratios.
//
// DESCRIPTION
//
//   The simplified ALExtract function extracts the files you specified
//   in the ALZipDir array.  To do this, the function has to recover
//   the ALArchive pointer that is stashed in the last element of the
//   array.  Then, it builds an ALEntryList that is used to call
//   ALArchive::Extract().
//
// RETURNS
//
//  AL_SUCCESS if things went well, o/w an ArchiveLib error code.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//   February 26, 1996  2.0B : The loop that builds up the ALEntryList
//                             had an incorrect test for termination.
//                             It should test to see if z->size == -1L,
//                             instead, it was testing to see if z->name
//                             was non-null.  Unfortunately, z->name is
//                             non-null for every valid entry in the list.
//
//  March 20, 1996      2.01A : Modified the toolkit passed to the
//                              entry list, so the test drive could
//                              be built without linking memory objects.
//
//   April 3, 1996      2.01A : Modified to deal with the change in return
//                              type from ALArchive::Extract()
//

extern "C"
AL_LINKAGE int AL_FUNCTION ALExtract( ALZipDir AL_DLL_FAR *z,
                                      int strip_path,
                                      CALLBACK_FN callback )
{
    int i;
    for ( i = 0 ; z[ i ].size != -1L ; i++ )
        ;
#if defined( AL_LARGE_DATA )
    ALPkArchive *arc = (ALPkArchive *) z[ i ].compressed_size;
#else
    ALPkArchive *arc = (ALPkArchive *) (int) z[ i ].compressed_size;
#endif
    arc->mfStripPathOnExtract = strip_path;
    ALSimpleMonitor monitor( callback );
    ALEntryList list( &monitor, PkDecompressFileTools() );
    while ( z->size != -1L ) {  //Changed in 2.00B
        ALDecompressor *d;
        if ( z->level == 0 )
            d = new ALCopyDecompressor;
        else
            d = new ALPkDecompressor;
        ALEntry *entry;
        entry = new ALEntry( list,
                             new ALFile( z->name ),
                             0,
                             d );
        _UpdateEntry( entry, z );
        z++;
    }
    i = arc->Extract( list );
    if ( i >= 0 )
        return AL_SUCCESS;
    return i;
}

