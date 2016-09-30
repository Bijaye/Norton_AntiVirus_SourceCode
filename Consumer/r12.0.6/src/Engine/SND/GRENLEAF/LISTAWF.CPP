//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/LISTAWF.CPv   1.1   20 Apr 1998 11:27:14   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/LISTAWF.CPv  $
// 
//    Rev 1.1   20 Apr 1998 11:27:14   DCHI
// Added AddFile(name) to ALEntryList.
// 
//    Rev 1.0   12 Mar 1998 12:16:52   DCHI
// Initial revision.
// 
//************************************************************************

//
// LISTAWF.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryList::AddWildCardFiles()
//  ALEntryListAddWildCardFiles()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "filestor.h"
#include "wildcard.h"

//
// NAME
//
//  ALEntryList::AddWildCardFiles()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Adds file names to an existing entry list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryList::
//    AddWildCardFiles( const char *pattern = "*.*",
//                      int traverse_flag = 0  );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryListAddWildCardFiles( hALEntryList this_object,
//                                   char AL_DLL_FAR *pattern,
//                                   int traverse_flag );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListAddWildCardFiles Lib "AL20LW"
//    ( ByVal this_object&,
//      ByVal pattern$,
//      ByVal traverse_flag% ) As Integer
//
//
// DELPHI SYNOPSIS
//
//  function ALEntryListAddWildCardFiles( this_object : hALEntryList;
//                                        pattern : String;
//                                        traverse_flag : Integer ) : Integer;
//
//
// ARGUMENTS
//
//  this_object  :  This identifies the list that is going to have a list
//                  of new wild card items added to it.  C++ doesn't
//                  have this parameter, since AddWildCardFiles is a member
//                  function of ALEntryList, and therefore gets an
//                  implicit pointer to the object in question.
//
//  pattern      :  A pointer to a C-style null terminated string.  This
//                  string contains a wild card pattern that will be expanded
//                  into a list of file names.  Note that the wild card
//                  expansion is done by the host operating system, so
//                  specific wild card characters and expansion rules
//                  will vary.  Under C+, this argument defaults to "*.*".
//
//  traverse_flag:  This integer should be set to 0 if you don't want to
//                  search all subdirectories under the current one.  Set
//                  it to a non-zero value and the wild card expansion will
//                  expand the wild card pattern in every directory it can
//                  find under the current directory.  Under C++, this
//                  argument has a default value of 0.
//
// DESCRIPTION
//
//  This function expands a wild card file specification into a batch
//  of new entries for the list.  The expansion part of this is pretty
//  simple, since it just uses the ALWildCardExpander class to do all
//  the work.  The expander class expands the file spec as requested,
//  and repeatedly returns names.
//
//  The job of this function is then to take those names and turn them
//  into list entries.  This is accomplished by asking the toolkit
//  to create the storage object, compression engine, and decompression
//  engine.  Since the toolkit is supplied by the user when the entry list
//  is created, this puts control over the type of entries added to the
//  list under programmer control, not library control.
//
// RETURNS
//
//  Unconditionally returns an integer indicating how many entries
//  were added to the list.
//
// CAVEATS
//
//  This function will happily add duplicate objects to a list.  If you
//  perform multiple updates on a list, it is probably a good idea to
//  scan for and delete duplicates afterwards.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

int AL_PROTO
ALEntryList::AddWildCardFiles( const char AL_DLL_FAR *pattern /* = "*.*" */,  /* Tag public function */
                               int traverse_flag /* = 0  */ )
{
    AL_ASSERT( pattern != 0, "AddWildCardFiles: null parameter for wild_spec" );
    ALWildCardExpander files( pattern, traverse_flag );
    int count = 0;

    char AL_DLL_FAR *new_name;
    while ( ( new_name = files.GetNextFile() ) != 0 ) {
        new ALEntry( *this,
                     mToolKit.CreateStorageObject(  new_name, AL_STORAGE_DEFAULT ),
                     mToolKit.CreateCompressor( AL_COMPRESSION_DEFAULT ),
                     mToolKit.CreateDecompressor( AL_COMPRESSION_DEFAULT ) );
        count++;
    }
    return count;
}

//
// NAME
//
//  ALEntryList::AddFile()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Adds a single file name to an existing entry list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryList::
//    AddFile( const char *name );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryListAddFile( hALEntryList this_object,
//                          char AL_DLL_FAR *name );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListAddFile Lib "AL20LW"
//    ( ByVal this_object&,
//      ByVal name$ ) As Integer
//
//
// DELPHI SYNOPSIS
//
//  function ALEntryListAddFile( this_object : hALEntryList;
//                               name : String ) : Integer;
//
//
// ARGUMENTS
//
//  this_object  :  This identifies the list that is going to have a list
//                  of new wild card items added to it.  C++ doesn't
//                  have this parameter, since AddWildCardFiles is a member
//                  function of ALEntryList, and therefore gets an
//                  implicit pointer to the object in question.
//
//  name         :  A pointer to a C-style null terminated string.  This
//                  string contains a file name that is added to the list.
//
// DESCRIPTION
//
//  The job of this function is to take the file name and add it to the
//  list entries.  This is accomplished by asking the toolkit
//  to create the storage object, compression engine, and decompression
//  engine.  Since the toolkit is supplied by the user when the entry list
//  is created, this puts control over the type of entries added to the
//  list under programmer control, not library control.
//
// RETURNS
//
//  Unconditionally returns an integer indicating how many entries
//  were added to the list.
//
// CAVEATS
//
//  This function will happily add duplicate objects to a list.  If you
//  perform multiple updates on a list, it is probably a good idea to
//  scan for and delete duplicates afterwards.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   April 20, 1998: Darren Chi
//

int AL_PROTO
ALEntryList::AddFile( const char AL_DLL_FAR *name )
{
    AL_ASSERT( name != 0, "AddFile: null parameter for AddFile(name) " );
    ALEntry *pEntry;

    pEntry = new ALEntry( *this,
                          mToolKit.CreateStorageObject( name, AL_STORAGE_DEFAULT ),
                          mToolKit.CreateCompressor( AL_COMPRESSION_DEFAULT ),
                          mToolKit.CreateDecompressor( AL_COMPRESSION_DEFAULT ) );

    if (pEntry == NULL)
        return(0);

    return(1);
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE
int AL_FUNCTION
ALEntryListAddWildCardFiles( hALEntryList this_object,  /* Tag public function */
                             char AL_DLL_FAR *pattern,
                             int traverse_flag )
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListAddWildCardFiles" );
    return ( (ALEntryList *) this_object )->AddWildCardFiles( pattern, traverse_flag );
}

#endif

