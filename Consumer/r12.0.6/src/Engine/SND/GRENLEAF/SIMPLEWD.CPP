//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/SIMPLEWD.CPv   1.0   12 Mar 1998 12:17:48   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/SIMPLEWD.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:48   DCHI
// Initial revision.
// 
//************************************************************************

//
// SIMPLEWD.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  _UpdateEntry()
//  ALSetName()
//  ALSetComment()
//  ALWriteDir()
//  ALWriteDirEntryVB()
//  ALWriteDirEntryDelphi()
//
// DESCRIPTION
//
//  These functions are used collectively to support the simplified
//  interface write directory functions.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New Release
//
//   March 20, 1006     2.01A : A few mods for the test drive release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <stdlib.h>
#include "filestor.h"
#include "copyengn.h"
#include "pkarc.h"
#include "pkengn.h"
#include "alsimple.h"

//
// NAME
//
//  _UpdateEntry()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  A helper function used by ALWriteDir().
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void _UpdateEntry( ALEntry *entry,
//                     ALZipDir *z )
//
// VB SYNOPSIS
//
//  Not used.
//
// DELPHI SYNOPSIS
//
//  Not used.
//
// ARGUMENTS
//
//  entry         :  A pointer to an ALEntry object.  The contents
//                   of the ALZipDir entry are going to be inserted
//                   into this entry.
//
//  z             :  A pointer to an ALZipDir entry.  The contents of
//                   this entry are going to be reformatted and
//                   stuffed into the ALEntry object.
//
// DESCRIPTION
//
//  The ALWriteDir function has to take an ALZipDir array and convert it
//  to an ALEntryList object in order to write the directory out to a
//  zip file.  This is a helper function that helps accomplish that.
//
//  The work is pretty mundane, it's broken out like this to help
//  cut things down to size.  See ALWriteDir for details on what part
//  of the job is handled in its routine, and what stuff gets handed
//  off to do here.
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

void AL_FUNCTION _UpdateEntry( ALEntry AL_DLL_FAR *entry, /* Tag private function */
                               ALZipDir AL_DLL_FAR *z )
{
        entry->mszComment = new char[ strlen( z->comment ) + 1 ];
        if ( entry->mszComment )
            strcpy( entry->mszComment, z->comment );
        entry->mlCompressedSize = z->compressed_size;
        entry->mlCompressedObjectPosition = z->compressed_position;
        entry->mlCrc32 = z->crc;
        entry->miMark = z->mark;
        entry->mpStorageObject->mlSize = z->size;
        struct tm tblock;
        tblock.tm_mon   = z->month - 1;
        tblock.tm_mday  = z->date;
        tblock.tm_year  = z->year - 1900;
        tblock.tm_hour  = z->hour;
        tblock.tm_min   = z->minute;
        tblock.tm_sec   = z->second;
        short int atts = 0;
        if ( z->r )
            atts |= ATTR_READ_ONLY;
        if ( z->a )
            atts |= ATTR_ARCHIVE;
        if ( z->s )
            atts |= ATTR_SYSTEM;
        if ( z->h )
            atts |= ATTR_HIDDEN;
        if ( z->d )
            atts |= ATTR_DIRECTORY;
        entry->mpStorageObject->mAttributes.SetFromPackedAttributes( atts );
        entry->mpStorageObject->mTimeDate.SetTimeDate( &tblock );
}

//
// NAME
//
//  ALSetName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  C++
//
// SHORT DESCRIPTION
//
//  A function that updates the file name in an ALZipDir entry.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALSetName( ALZipDir *z, const char *name )
//
// VB SYNOPSIS
//
//  Not used.
//
// DELPHI SYNOPSIS
//
//  Not used.
//
// ARGUMENTS
//
//  z             :  A pointer to an ALZipDir entry.  The contents of
//                   the filename member are going to be updated.
//
//  name          :  The new name for the entry.
//
// DESCRIPTION
//
//  The name and comment members of the ALZipDir array are dynamically
//  allocated.  To prevent those nasty DLL/EXE problems with memory
//  allocation, we create this routine.  It takes care of the memory
//  allocation/freeing business.  The old name is deleted from the
//  ALZipDir entry, then new space is allocate for the new name, and a
//  point to it is stuffed into the entry.
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
//   February 14, 1996  2.0A  : New Release
//
//   March 20, 1996     2.01A : Changed the name arg to be a const char *,
//                              this lets you pass CString parameters
//                              directly. (ANSI string someday as well).

extern "C" AL_LINKAGE
void AL_FUNCTION ALSetName( ALZipDir AL_DLL_FAR *z,
                            const char AL_DLL_FAR *name )
{
    if ( z->name )
        delete[] z->name;
    z->name = new char[ strlen( name ) + 1 ];
    if ( z->name )
        strcpy( z->name, name );
}

//
// NAME
//
//  ALSetComment()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  C++
//
// SHORT DESCRIPTION
//
//  A function that updates the file comment in an ALZipDir entry.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALSetComment( ALZipDir *z, const char *comment )
//
// VB SYNOPSIS
//
//  Not used.
//
// DELPHI SYNOPSIS
//
//  Not used.
//
// ARGUMENTS
//
//  z             :  A pointer to an ALZipDir entry.  The contents of
//                   the comment member are going to be updated.
//
//  name          :  The new comment for the entry.
//
// DESCRIPTION
//
//  The name and comment members of the ALZipDir array are dynamically
//  allocated.  To prevent those nasty DLL/EXE problems with memory
//  allocation, we created this routine.  It takes care of the memory
//  allocation/freeing business.  The old comment is deleted from the
//  ALZipDir entry, then new space is allocate for the new comment, and a
//  pointer to it is stuffed into the entry.
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
//   February 14, 1996  2.0A  : New Release
//
//   March 20, 1996     2.01A : Changed the comment arg to be a const char *,
//                              this lets you pass CString parameters
//                              directly. (ANSI string someday as well).

extern "C" AL_LINKAGE
void AL_FUNCTION ALSetComment( ALZipDir AL_DLL_FAR *z,
                               const char AL_DLL_FAR *comment )
{
    if ( z->comment )
        delete[] z->comment;
    z->comment = new char[ strlen( comment ) + 1 ];
    if ( z->comment )
        strcpy( z->comment, comment );
}

//
// NAME
//
//  ALWriteDir()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  C++
//
// SHORT DESCRIPTION
//
//  This function writes an ALZipDir array out to a ZIP file.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  int ALWriteDir( ALZipDir AL_DLL_FAR *z )
//
// VB SYNOPSIS
//
//  See arclib.bas for the VB implementation.
//
// DELPHI SYNOPSIS
//
//  See arclib.pas for the Delphi implementation.
//
// ARGUMENTS
//
//  z             :  A pointer to an ALZipDir array.  The contents of
//                   this array are going to be written out to the
//                   ZIP file, completely replacing its present directory.
//
// DESCRIPTION
//
//  This function writes a new directory out to a ZIP file.  This is
//  a reasonable thing to do if read in the directory from the same
//  ZIP file, and have udpated some file names, comments, permission
//  bits, etc.
//
// RETURNS
//
//  A standard ArchiveLib return, AL_SUCCESS if things went well, and
//  something < 0 if things went bad.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New Release
//
//   March 20, 1996     2.01A : Modified the list argument so that it
//                              uses PKCompressFileTools(). This allows
//                              it to function properly in the test drive,
//                              where memory objects aren't available.

extern "C"
AL_LINKAGE int AL_FUNCTION ALWriteDir( ALZipDir AL_DLL_FAR *z )
{
    int i;
    for ( i = 0 ; z[ i ].size != -1L ; i++ )
        ;
#if defined( AL_LARGE_DATA )
    ALPkArchive *arc = (ALPkArchive *) z[ i ].compressed_size;
#else
    ALPkArchive *arc = (ALPkArchive *) (int) z[ i ].compressed_size;
#endif
    ALEntryList list( 0, PkCompressFileTools() );
//
// This loop is building the new elements of the ALEntryList, one
// at a time. We take care of creating the compressor and ALFile
// object here, and leave the rest of the stuff up to the _UpdateEntry()
// function.
//
    while ( z->size != -1L ) {
        ALCompressor *c;
        if ( z->level == 0 )
            c = new ALCopyCompressor;
        else {
            ALPkCompressor * p = new ALPkCompressor;
            p->option = (ALPkCompressor::_option) ( z->level - 1 );
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
    arc->SetComment( z->comment );
    return arc->WriteDirectory( list );
}

//
// NAME
//
//  ALWriteDirEntryVB()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  VB
//
// SHORT DESCRIPTION
//
//  This is a helper function for the VB version of ALWriteDir.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALWriteDirEntryVB( ALZipDir *z,
//                          char *name,
//                          char *comment,
//                          ALEntryList *list )
//
// VB SYNOPSIS
//
//  Declare Sub ALWriteDirEntryVB Lib "AL20LWD" ( z As ALZipDir,
//                                                ByVal filename$,
//                                                ByVal comment$,
//                                                ByVal list& )
//
// DELPHI SYNOPSIS
//
//  See arclib.pas for the Delphi implementation.
//
// ARGUMENTS
//
//  z             :  A pointer to an ALZipDir array entry.  This
//                   function is going to add a new entry to the
//                   ALEntryList using the data in this entry.
//
//  filename      :  VB passes the filename as a parameter so the
//                   C++ code doesn't have to interpret it as an
//                   entry in the ALZipDir array.  This is strictly
//                   a convenience for this internal function.
//
//  comment       :  The file comment is passed as a parameter for
//                   the same reason.
//
//  list          :  A pointer to an ALEntryList object.  The VB version
//                   of ALWriteDir() is going to build up a complete
//                   copy of the ALZipDir in this list, then write
//                   it out using ALArchive::WriteDirectory().
//
// DESCRIPTION
//
//  VB has its own version of ALWriteDir().  That routine creates an
//  ALEntryList on its own, then writes it out to the Zip file using
//  the WriteDirectory() function.  Building the list is done one
//  entry at a time by calling this function.
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

#if defined( AL_VB ) || defined( AL_VB32 )

extern "C"
AL_LINKAGE void AL_FUNCTION ALWriteDirEntryVB( ALZipDir AL_DLL_FAR *z,
                                               char AL_DLL_FAR *name,
                                               char AL_DLL_FAR *comment,
                                               ALEntryList AL_DLL_FAR *list )
{
    ALCompressor *c;
    ALDecompressor *d;
    if ( z->level == 0 ) {
        c = new ALCopyCompressor;
        d = new ALCopyDecompressor;
    } else {
        ALPkCompressor * p = new ALPkCompressor;
        p->option = (ALPkCompressor::_option) ( z->level - 1 );
        c = p;
        d = new ALPkDecompressor;
    }
    ALEntry *entry;
    char *p1;
    if ( name )
        p1 = name;
    else
        p1 = "";
    entry = new ALEntry( *list,
                         new ALFile( p1 ),
                         c,
                         d );
    char AL_DLL_FAR *temp = z->comment;
    if ( comment )
        z->comment = comment;
    else
        z->comment = "";
    _UpdateEntry( entry, z );
    z->comment = temp;
}

//
// NAME
//
//  ALWriteDirEntryDelphi()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  Delphi
//
// SHORT DESCRIPTION
//
//  This is a helper function for the Delphi version of ALWriteDir.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALWriteDirEntryDelphi( ALZipDir *z,
//                              ALEntryList *list )
//
// VB SYNOPSIS
//
//  None.
//
// DELPHI SYNOPSIS
//
//  procedure ALWriteDirEntryDelphi( z : ALZipDirEntry;
//                                   list : hALEntryList );
//
// ARGUMENTS
//
//  z             :  A pointer to an ALZipDir array entry.  This
//                   function is going to add a new entry to the
//                   ALEntryList using the data in this entry.
//
//  list          :  A pointer to an ALEntryList object.  The Delphi version
//                   of ALWriteDir() is going to build up a complete
//                   copy of the ALZipDir in this list, then write
//                   it out using ALArchive::WriteDirectory().
//
// DESCRIPTION
//
//  Delphi has its own version of ALWriteDir().  That routine creates an
//  ALEntryList on its own, then writes it out to the Zip file using
//  the WriteDirectory() function.  Building the list is done one
//  entry at a time by calling this function.
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
#include <stdio.h>

extern "C"
AL_LINKAGE void AL_FUNCTION ALWriteDirEntryDelphi( char AL_DLL_FAR *z_dummy,
                                                   ALEntryList AL_DLL_FAR *list )
{
    z_dummy += 4;  // The Delphi structure isn't exactly the same size!!!
    ALZipDir *z = (ALZipDir *) z_dummy;
    ALCompressor *c;
    ALDecompressor *d;
    if ( z->level == 0 ) {
        c = new ALCopyCompressor;
        d = new ALCopyDecompressor;
    } else {
        ALPkCompressor * p = new ALPkCompressor;
        p->option = (ALPkCompressor::_option) ( z->level - 1 );
        c = p;
        d = new ALPkDecompressor;
    }
    ALEntry *entry;
    char *p1;
    if ( z->name )
        p1 = z->name ;
    else
        p1 = "";
    entry = new ALEntry( *list,
                         new ALFile( p1 ),
                         c,
                         d );
    char AL_DLL_FAR *temp = z->comment;
    if ( !z->comment )
        z->comment = "";
    _UpdateEntry( entry, z );
    z->comment = temp;
}

#endif

