//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/SIMPLERD.CPv   1.0   12 Mar 1998 12:17:54   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/SIMPLERD.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:54   DCHI
// Initial revision.
// 
//************************************************************************

//
// SIMPLERD.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALFreeDir()
//  ALReadDir()
//  ALReadDirEntryVB()
//
// DESCRIPTION
//
//  These functions are used collectively to support the simplified
//  interface read directory functions.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//   March 20, 1996     2.01A : A couple of mods to accomodate the test
//                              drive version of ArchiveLib.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <stdlib.h>
#include <string.h>
#include "pkarc.h"
#include "pkengn.h"
#include "alsimple.h"

//
// NAME
//
//  ALFreeDir()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The simplified interface function frees the memory allocated for
//  an ALZipDir.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALFreeDir( ALZipDir *z )
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
//                   have read in using ALReadDir().
//
// DESCRIPTION
//
//  Any time you read in the directory from a PKZIP file using ALReadDir(),
//  you must eventually delete it using this function.  It takes care of
//  freeing up the space used by the array itself, by the file names, the
//  file comments, the archive comment, and the archive object itself.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

extern "C" AL_LINKAGE
void AL_FUNCTION ALFreeDir( ALZipDir AL_DLL_FAR *z )
{
    int i;
    for ( i = 0 ; ; i++ ) {
        if ( z[ i ].comment )
            delete[] z[ i ].comment;
        delete[] z[ i ].name;
        if ( z[ i ].size == -1L )
            break;
    }
#if defined( AL_LARGE_DATA )
    ALPkArchive *p = (ALPkArchive *) z[ i ].compressed_size;
#else
    ALPkArchive *p = (ALPkArchive *) (int) z[ i ].compressed_size;
#endif
    delete p;

    delete[] z;
}

//
// NAME
//
//  ALReadDir()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The simplified interface function reads in the directory of a ZIP file.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  ALZipDir * ALReadDir( const char *filename,
//                        int *count,
//                        int *error )
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
//  filename      :  The name of the zip archive whose directory
//                   you want to read.
//
//  count         :  A pointer to an integer that is going to
//                   receive the count of items in the directory.
//
//  error         :  A pointer to an integer that is going to
//                   receive the status from the ALReadDir operation.
//
// DESCRIPTION
//
//  This function reads in the directory information from a ZIP
//  file, then takes each entry and inserts it into an ALZipDir array.
//  This means it has to take each record and reformat the data so
//  that it fits in this fixed array.
//
// RETURNS
//
//  The pointer to an ALZipDir array.  If a memory allocation failure
//  occurs, you just get a 0 back.  This function also returns the
//  count and error information as described above.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//   March 20, 1996   2.01A  : Modified the filename parameter to be const
//                             char * instead of char *. This lets you pass
//                             CString arguments.  I also modified the
//                             toolkit being passed so that I don't use
//                             any ALMemory objects, that not being present
//                             in the test drive version.
//
//                             Note that changing the toolkit means that
//                             the resulting entries only have a decompressor,
//                             not a compressor.  Because of this, there
//                             are a couple of places where I used to check
//                             the compressor value, but now I check the
//                             the decompressor.
//

extern "C" AL_LINKAGE
ALZipDir AL_DLL_FAR * AL_FUNCTION
ALReadDir( const char AL_DLL_FAR *filename,
           int AL_DLL_FAR *count,
           int AL_DLL_FAR *error )
{
    ALPkArchive *arc = new ALPkArchive( filename );
    ALEntryList list( 0, PkDecompressFileTools() );
    ALEntry *entry;
    int dummy;

    if ( !error )
        error = &dummy;
    *error = arc->ReadDirectory( list );

    if ( *error != AL_SUCCESS )
        return 0;
    if ( !count )
        count = &dummy;
    *count = 0;
    for ( entry = list.GetFirstEntry();
          entry;
          entry = entry->GetNextEntry() )
        *count = *count + 1;
    ALZipDir *ALZip = new ALZipDir[ *count + 1 ];
    if ( !ALZip )
        return 0;
    int i = 0;
    for ( entry = list.GetFirstEntry();
          entry;
          entry = entry->GetNextEntry() ) {
        ALZip[ i ].name = new char[ strlen( entry->mpStorageObject->mName ) + 1 ];
        if ( ALZip[ i ].name )
            strcpy( ALZip[ i ].name, entry->mpStorageObject->mName );
        const char *p = entry->GetComment();
        if ( !p )
            p = "";
        ALZip[ i ].comment = new char[ strlen( p ) + 1 ];
        if ( ALZip[ i ].comment )
            strcpy( ALZip[ i ].comment, p );
        ALZip[ i ].compressed_size = entry->GetCompressedSize();
        ALZip[ i ].compressed_position = entry->GetCompressedObjectPosition();
        ALZip[ i ].size = entry->mpStorageObject->GetSize();
        ALZip[ i ].crc    = entry->GetCrc32();
        ALZip[ i ].mark = 1;
        struct tm tblock;
        entry->mpStorageObject->mTimeDate.GetTimeDate( &tblock );
        ALZip[ i ].month  = (short int) ( tblock.tm_mon + 1 );
        ALZip[ i ].date   = (short int) tblock.tm_mday;
        ALZip[ i ].year   = (short int)( tblock.tm_year + 1900 );
        ALZip[ i ].hour   = (short int) tblock.tm_hour;
        ALZip[ i ].minute = (short int) tblock.tm_min;
        ALZip[ i ].second = (short int) tblock.tm_sec;
        ALZip[ i ].r = (unsigned char) entry->mpStorageObject->mAttributes.ReadOnly();
        ALZip[ i ].a = (unsigned char) entry->mpStorageObject->mAttributes.Archive();
        ALZip[ i ].s = (unsigned char) entry->mpStorageObject->mAttributes.System();
        ALZip[ i ].h = (unsigned char) entry->mpStorageObject->mAttributes.Hidden();
        ALZip[ i ].d = (unsigned char) entry->mpStorageObject->mAttributes.Directory();
        if ( entry->mpDecompressor->miCompressionType == AL_COMPRESSION_COPY )
            ALZip[ i ]. level = 0;
        else
            ALZip[ i ].level = (unsigned char) (((ALPkDecompressor *)(entry->mpDecompressor))->option + 1);
        i++;
    }
    ALZip[ i ].name = new char[ strlen( filename ) + 1 ];
    if ( ALZip[ i ].name )
        strcpy( ALZip[ i ].name, filename );
    const char *p = arc->GetComment();
    if ( p == 0 )
        p = "";
    ALZip[ i ].comment = new char[ strlen( p ) + 1 ];
    if ( ALZip[ i ].comment )
        strcpy( ALZip[ i ].comment, p );
    ALZip[ i ].size = -1L;
//
// Here's the cute part!
//
#if defined( AL_LARGE_DATA )
    ALZip[ i ].compressed_size = (long) arc;
#else
    ALZip[ i ].compressed_size = (int) arc;
#endif
    return ALZip;
}

//
// NAME
//
//  ALReadDirEntryVB()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  VB
//
// SHORT DESCRIPTION
//
//  This is a helper routine called by the VB version of ALReadDir.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALReadDirEntryVB( ALZipDir *z,
//                         ALEntry *entry)
//
// VB SYNOPSIS
//
//  Declare Sub ALReadDirEntryVB Lib "AL20LWD" ( z As ALZipDir,
//                                               ByVal entry& )
//
// DELPHI SYNOPSIS
//
//  None.
//
// ARGUMENTS
//
//  z             :  A pointer to an entry in an ALZipDir array.
//
//  entry         :  A pointer to an ALEntry object.  This ALEntry
//                   object is going to have its information reformatted
//                   and stuffed into the ALZipDir array.
//
// DESCRIPTION
//
//  When reading in the directory information, VB goes through entry
//  by entry, using this function to take the ALEntry data and stuffing
//  into the ALZipDir entry.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//   March 20, 1996   2.01A  : Note that changing the toolkit in the previous
//                             function means that the resulting entries only
//                             have a decompressor, not a compressor.
//                             Because of this, there are a couple
//                             of places where I used to check
//                             the compressor value, but now I check the
//                             the decompressor.
//
//   March 26, 1996   2.01A  : When reading Zip 1.x archives, it is possible
//                             to end up with a directory entry that has
//                             a 0 for a decompressor.  I deal with that
//                             now by performing an explicit test.
//
#if defined( AL_VB ) || defined( AL_VB32 )

extern "C" AL_LINKAGE
void AL_FUNCTION ALReadDirEntryVB( ALZipDir AL_DLL_FAR *z,
                                   ALEntry AL_DLL_FAR *entry)
{
   z->compressed_size = entry->GetCompressedSize();
   z->compressed_position = entry->GetCompressedObjectPosition();
   z->size = entry->mpStorageObject->GetSize();
   z->crc    = entry->GetCrc32();
   z->mark = 1;
   struct tm tblock;
   entry->mpStorageObject->mTimeDate.GetTimeDate( &tblock );
   z->month  = (short int) ( tblock.tm_mon + 1 );
   z->date   = (short int) tblock.tm_mday;
   z->year   = (short int)( tblock.tm_year + 1900 );
   z->hour   = (short int) tblock.tm_hour;
   z->minute = (short int) tblock.tm_min;
   z->second = (short int) tblock.tm_sec;
   z->r = (unsigned char) entry->mpStorageObject->mAttributes.ReadOnly();
   z->a = (unsigned char) entry->mpStorageObject->mAttributes.Archive();
   z->s = (unsigned char) entry->mpStorageObject->mAttributes.System();
   z->h = (unsigned char) entry->mpStorageObject->mAttributes.Hidden();
   z->d = (unsigned char) entry->mpStorageObject->mAttributes.Directory();
   if ( entry->mpDecompressor == 0 ||
        entry->mpDecompressor->miCompressionType == AL_COMPRESSION_COPY )
       z->level = 0;
   else
       z->level = (unsigned char) (((ALPkCompressor *)(entry->mpDecompressor))->option + 1);
}

#endif

