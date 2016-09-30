//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/WILDNT.CPv   1.0   12 Mar 1998 12:18:06   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/WILDNT.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:18:06   DCHI
// Initial revision.
// 
//************************************************************************

//
// WILDNT.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALWildCardExpander::ConstructOsData()
//  ALWildCardExpander::DestroyOsData()
//  ALWildCardExpander::GetNextFile()
//
// DESCRIPTION
//
//  This file contains the OS specific routines need to expand
//  wild card file names.  Different versions of these three functions
//  exist for DOS, Win95, and OS/2.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

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
#include "wildcard.h"

/*
 *  mFindFileData   : Under Win32s, this holds data about the file we
 *                    found.
 *
 *  mFindFileHandle : Under Win32s, this is a handle used during the
 *                    wildcard expansion.
 *
 */

class ALWildCardOsData {  /* Tag internal class */
    public :
        WIN32_FIND_DATA mFindFileData;
        HANDLE mFindFileHandle;
};

//
// NAME
//
//  ALWildCardExpander::ConstructOsData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Win32
//  C++
//
// SHORT DESCRIPTION
//
//  Allocate space for a wildcard search under the Win32 file system.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard
//
//  void ALWildCardExpander::ConstructOsData();
//
// C SYNOPSIS
//
//  No C translation for this internal protected function.
//
// VB SYNOPSIS
//
//  No VB translation for this internal protected function.
//
// DELPHI SYNOPSIS
//
//  No Delphi translation for this internal protected function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The process of wild card file expansion differs drastically for
//  all the different environments we support.  Since each system
//  is going to have different data requirements, we don't try to
//  allocate it in the base class.  Instead, we just keep a pointer to
//  the data, and allocate it in this member function.  Different versions
//  of this member function show up in different directories, and the
//  BUILD.INI file directs the compiler as to which to include.
//
//  This module supports the Win32 file system.  The Win32 wild card
//  expansion state is stored in two different structures.  mFindFileData
//  holds the data that describes the found file.  mFindFileHandle is
//  a handle that describes the current expander.  Note that neither of
//  these is initialized here, we just allocate space for them.
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
ALWildCardExpander::ConstructOsData()  /* Tag protected function */
{
    mpOsData = new ALWildCardOsData;
    if ( mpOsData )
        mpOsData->mFindFileHandle = INVALID_HANDLE_VALUE;
}

//
// NAME
//
//  ALWildCardExpander::DestroyOsData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Win32
//  C++
//
// SHORT DESCRIPTION
//
//  Destroy the space allocated for the wildcard search under Win32.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard
//
//  void ALWildCardExpander::DestroyOsData();
//
// C SYNOPSIS
//
//  No C translation for this internal protected function.
//
// VB SYNOPSIS
//
//  No VB translation for this internal protected function.
//
// DELPHI SYNOPSIS
//
//  No Delphi translation for this internal protected function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The process of wild card file expansion differs drastically for
//  all the different environments we support.  Since each system
//  is going to have different data requirements, we don't try to
//  allocate it in the base class.  Instead, we just keep a pointer to
//  the data, and allocate it in the member function ConstructOsData().
//
//  If we are constructing something when the search starts, it makes sense
//  that we need to destroy it when the search is over.  That's what this
//  guy does.
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
ALWildCardExpander::DestroyOsData()  /* Tag protected function */
{
    if ( mpOsData != 0 ) {
        if ( mpOsData->mFindFileHandle != INVALID_HANDLE_VALUE )
        FindClose( mpOsData->mFindFileHandle );
        delete mpOsData;
    }
    mpOsData = 0;
}

//
// NAME
//
//  ALWildCardExpander::GetNextFile()
//
// PLATFORMS/ENVIRONMENTS
//
//  Win32
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Search for an additional file in the wildcard expansion.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  char *ALWildCardExpander::GetNextFile();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  char *ALExpanderGetNextFile( hALExpander this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALExpanderGetNextFile Lib "AL20LW"
//    Alias "ALExpanderGetNextFileVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALExpanderGetNextFile( this_object : hALExpander ) : PChar;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALWildCardExpander that
//                  is performing the expansion.  Note that the C++ member
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  The ALWildCardExpander has what amounts to six different internal
//  states.  They are:
//
//              Searching subdirectories, using another object
//
//              Extracting the next wild spec from the input line
//
//              Expanding the wild card to get the first matching file
//
//              Expanding the wild card to get the next matching file
//
//              Looking for the first subdirectory
//
//              Looking for the next subdirectory
//
//  For the most part, we keep track of the state using the mState
//  variable.  However, we keep track of whether we are searching
//  subdirectories by examining the pointer to the next expander.  If
//  it is non-null, it means we are in that state.
//
// RETURNS
//
//  In the event that this routine is able to come up with a new
//  file name, it returns a character pointer to the name, which
//  is kept in member variable mResultFileName.  If no new file
//  name could be cooked up, we return a 0, which means you are
//  done.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

char AL_DLL_FAR * AL_PROTO
ALWildCardExpander::GetNextFile()  /* Tag public function */
{
    if ( mpOsData == 0 )
        return 0;
    for ( ; ; ) {
//
// If the pointer to the next expander is set, it means we are working
// on a subdirectory, so I have to let him do the work.  If the subdirectory
// search fails, I continue right back where I was when interrupted.
//
        if ( mpNextExpander ) {
            char *p = mpNextExpander->GetNextFile();
            if ( p )
                return p;          // Return the name if he found one
            delete mpNextExpander; // If not, he is toast
            mpNextExpander = 0;
        }
        switch ( mState ) {
//
// This is where I get the next wild spec from the input line.  If
// there aren't any more, I return 0, because we are done.  If there
// is one, I set up the member variable that will be used in the
// rest of the search, and set up the state so that next I will get
// get the first file name.
//
            case GET_NEXT_WILD_NAME :
                if ( GetNextWildName() == 0 )
                    return 0;
                mWildPathOnly = mFullWildName;
                mWildPathOnly.StripFileName();
                mWildNameOnly = mFullWildName;
                mWildNameOnly.StripPath();
                mState = GET_FIRST_FILE_NAME;
                break;
//
// Once I have a wild spec, time to start getting file names.
// FindFirstFile() does it for me.  if there aren't any files, I
// either go on to search directories, or go the the next wild
// name in the input line.  If there is a name, I return it to
// the calling procedure.
//
            case GET_FIRST_FILE_NAME :
                mpOsData->mFindFileHandle = FindFirstFile( mFullWildName, &mpOsData->mFindFileData );
                if ( mpOsData->mFindFileHandle == INVALID_HANDLE_VALUE ) {
                    if ( miTraverseFlag )
                        mState = GET_FIRST_DIRECTORY;
                    else
                        mState = GET_NEXT_WILD_NAME;
                    break;
                }
                mState = GET_NEXT_FILE_NAME;
                if ( mpOsData->mFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    break;
                mResultFileName = ALName( mWildPathOnly + mpOsData->mFindFileData.cFileName );
                return mResultFileName;
//
// Time to get another file name with FindNextFile().  If there aren't
// any more, I clean up, and either get the next name for the input
// line or start searching subdirectories. If there was a name, I return
// it to the calling procedure.
//
            case GET_NEXT_FILE_NAME :
                if ( !FindNextFile( mpOsData->mFindFileHandle, &mpOsData->mFindFileData ) ) {
                    FindClose( mpOsData->mFindFileHandle );
                    mpOsData->mFindFileHandle = INVALID_HANDLE_VALUE;
                    if ( miTraverseFlag )
                        mState = GET_FIRST_DIRECTORY;
                    else
                        mState = GET_NEXT_WILD_NAME;
                    break;
                }
                if ( mpOsData->mFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    break;
                mResultFileName = ALName( mWildPathOnly + mpOsData->mFindFileData.cFileName );
                return mResultFileName;
//
// The procedure to get the first subdirectory is an awful lot like that
// we use to get the first file.  If we find a valid subdirectory, we create
// a new expander to deal with its wildcards.  If we find a file, but
// it isn't a subdirectory, we keep on searching.  If we don't find
// anything, we are going to go back and check out the next file spec
// from the input line.
//
            case GET_FIRST_DIRECTORY :
                mpOsData->mFindFileHandle = FindFirstFile( mWildPathOnly + "*.*", &mpOsData->mFindFileData );
                if ( mpOsData->mFindFileHandle == INVALID_HANDLE_VALUE ) {
                    mState = GET_NEXT_WILD_NAME;
                    break;
                }
                mState = GET_NEXT_DIRECTORY;
                if ( mpOsData->mFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                    if ( strcmp( mpOsData->mFindFileData.cFileName, ".." ) == 0 )
                        break;
                    if ( strcmp( mpOsData->mFindFileData.cFileName, "." ) == 0 )
                        break;
                    mpNextExpander = new ALWildCardExpander( mWildPathOnly + mpOsData->mFindFileData.cFileName + "\\" + (char *) mWildNameOnly, 1, mCase );
                }
                break;
//
// This works the same as the state where I get the first directory.
// The only difference here is that if I run out of file names in the
// directory, I have to call FindClose() to clean up after myself.
//
            case GET_NEXT_DIRECTORY :
                if ( !FindNextFile( mpOsData->mFindFileHandle, &mpOsData->mFindFileData ) ) {
                    FindClose( mpOsData->mFindFileHandle );
                    mpOsData->mFindFileHandle = INVALID_HANDLE_VALUE;
                    mState = GET_NEXT_WILD_NAME;
                    break;
                }
                if ( mpOsData->mFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                    if ( strcmp( mpOsData->mFindFileData.cFileName, ".." ) == 0 )
                        break;
                    if ( strcmp( mpOsData->mFindFileData.cFileName, "." ) == 0 )
                        break;
                    mpNextExpander = new ALWildCardExpander( mWildPathOnly + mpOsData->mFindFileData.cFileName + "\\" + (char *) mWildNameOnly, 1 );
                }
                break;
            default :
                return 0;
        }
    }
}


