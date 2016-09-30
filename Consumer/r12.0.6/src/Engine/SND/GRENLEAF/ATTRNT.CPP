//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ATTRNT.CPv   1.0   12 Mar 1998 12:14:56   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ATTRNT.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:56   DCHI
// Initial revision.
// 
//************************************************************************

//
// ATTRNT.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALFile::ReadAttributesFromFileSys()
//  ALFile::WriteAttributesToFileSys()
//
// DESCRIPTION
//
//  These two files provide system-dependent functionality for ALFile.
//  It takes different code to set the attributes for files depending
//  on which O/S you are targetting.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// Problem here with PowerPack and others that use the NT
// API, but don't support the whole damn thing
//

#ifdef IsBadWritePtr
#undef IsBadWritePtr
#endif

#include <windows.h>
#include "filestor.h"
#include "fileattr.h"

//
// NAME
//
//  ALFile::ReadAttributesFromFileSys()
//
// PLATFORMS/ENVIRONMENTS
//
//  Win32
//  C++
//
// SHORT DESCRIPTION
//
//  Read in the attributes of a file under Win32.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::ReadAttributesFromFileSys();
//
// C SYNOPSIS
//
//  None, this internal protected function has no C version.
//
// VB SYNOPSIS
//
//  None, this internal protected function has no VB version.
//
// DELPHI SYNOPSIS
//
//  None, this internal protected function has no Delphi version.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  When an ALFile object is opened, we have to read the file attributes.
//  This is an O/S dependent function, but I don't want to have too many
//  #ifdefs, so I package it in a separate directory that gets compiled
//  in an environment dependent fashion.  This particular file gets
//  compiled for any library that's using the Win32 file system.  At this
//
//  All this function has to do is call the GetFileAttributes( () function,
//  then translate the returned bits into the appropriate internal attribute
//  bits.
//
// RETURNS
//
//  AL_SUCCESS or AL_CANT_OPEN_FILE.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALFile::ReadAttributesFromFileSys()  /* Tag protected function */
{
    DWORD attributes = GetFileAttributes( mName );
    if ( attributes == 0xFFFFFFFF )
        return AL_CANT_OPEN_FILE;
    mAttributes.miUnixBitsPresent = 0;
    mAttributes.miReadOnly = ( attributes & FILE_ATTRIBUTE_READONLY ) != 0;
    mAttributes.miSystem = ( attributes & FILE_ATTRIBUTE_SYSTEM ) != 0;
    mAttributes.miHidden = ( attributes & FILE_ATTRIBUTE_HIDDEN ) != 0;
    mAttributes.miArchive = ( attributes & FILE_ATTRIBUTE_ARCHIVE ) != 0;
    return AL_SUCCESS;
}

//
// NAME
//
//  ALFile::WriteAttributesToFileSys()
//
// PLATFORMS/ENVIRONMENTS
//
//  Win32
//  C++
//
// SHORT DESCRIPTION
//
//  Set the attributes of a file under Win32.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "filestor.h"
//
//  int ALFile::WriteAttributesToFileSys();
//
// C SYNOPSIS
//
//  None, this internal protected function has no C version.
//
// VB SYNOPSIS
//
//  None, this internal protected function has no VB version.
//
// DELPHI SYNOPSIS
//
//  None, this internal protected function has no Delphi version.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  When an ALFile object is closed, we have to set the file attributes.
//  This is an O/S dependent function, but I don't want to have too many
//  #ifdefs, so I package it in a separate directory that gets compiled
//  in an environment dependent fashion.  This particular file gets
//  compiled for any library that's using the MS-DOS file system.
//
//  All this function has to do is call the WriteAttributesToFileSys()
//  function, with the appopriate bits set.  This means I have to translate
//  the internal bits into the format expected by the Win32 API.
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
//   February 14, 1996  2.0A : New release.
//

void AL_PROTO
ALFile::WriteAttributesToFileSys()  /* Tag protected function */
{
    DWORD attributes = 0;
    if ( !mAttributes.miUnixBitsPresent ) {
        attributes |= mAttributes.miReadOnly ? FILE_ATTRIBUTE_READONLY : 0;
        attributes |= mAttributes.miSystem ? FILE_ATTRIBUTE_SYSTEM : 0;
        attributes |= mAttributes.miHidden ? FILE_ATTRIBUTE_HIDDEN : 0;
        attributes |= mAttributes.miArchive ? FILE_ATTRIBUTE_ARCHIVE : 0;
    }
    SetFileAttributes( mName, attributes );
}

