//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/SIMPDEL.CPv   1.0   12 Mar 1998 12:17:54   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/SIMPDEL.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:54   DCHI
// Initial revision.
// 
//************************************************************************

//
// SIMPDEL.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALDelete()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//   February 26, 1996  2.0B : Fixed a bad bug in ALDelete()
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "alsimple.h"
#include "simpmon.h"
#include "pkarc.h"
#include "copyengn.h"
#include "pkengn.h"
#include "filestor.h"

//
// NAME
//
//  ALDelete()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The simplified interface function to delete files from a ZIP file.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  int ALDelete( ALZipDir *z,
//                CALLBACK_FN callback )
//
// VB SYNOPSIS
//
//  See arclib.bas for VB function.
//
// DELPHI SYNOPSIS
//
//  See arclib.pas for Delphi function.
//
// ARGUMENTS
//
//  z             :  The ALZipDir array.  This is the array you will have
//                   have read in using ALReadDir().  Every entry in the
//                   array that has a mark set will be deleted.
//
//  callback      :  C and C++ use a callback function for monitoring.
//                   This function gets called periodically with
//                   file names and progress ratios.  Note that the
//                   file names passed from ALDelete are the files that
//                   are being *preserved*, not the ones that are being
//                   deleted.
//
// DESCRIPTION
//
//   The simplified ALDelete function deletes the files you specified
//   in the ALZipDir array.  To do this, the function has to recover
//   the ALArchive pointer that is stashed in the last element of the
//   array.  Then, it builds an ALEntryList that is used to call
//   ALArchive::Delete().
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
//
//   February 26, 1996  2.0B : The loop that builds up the ALEntryList
//                             had an incorrect test for termination.
//                             It should test to see if z->size == -1L,
//                             instead, it was testing to see if z->name
//                             was non-null.  Unfortunately, z->name is
//                             non-null for every valid entry in the list.
//
//   March 20, 1996     2.10A : I modified the type of toolkit being used
//                              here so that I wouldn't link in ALMemory
//                              objects.  I'm trying to keep those out
//                              of the test drive.
//
//   April 3, 1996      2.01A : Modified to deal with the change in return
//                              type from ALArchive::Delete()
//

AL_LINKAGE int AL_FUNCTION ALDelete( ALZipDir AL_DLL_FAR *z,
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
    ALSimpleMonitor monitor( callback );
    ALEntryList list( &monitor, PkCompressFileTools() );
    while ( z->size != -1L ) {
        ALCompressor *c;
        if ( z->level == 0 )
            c = new ALCopyCompressor;
        else {
            ALPkCompressor * p = new ALPkCompressor;
            p->option = (ALPkCompressor::_option) (z->level - 1);
            c = p;
        }
        ALEntry *entry;
        entry = new ALEntry( list,
                             new ALFile( z->name ),
                             c,
                             0 );
        _UpdateEntry( entry, z );
        z++;
    }
    ALPkArchive output( "" );
    i = arc->Delete( list, output );
    if ( i >= 0 )
        return AL_SUCCESS;
    return i;
}
