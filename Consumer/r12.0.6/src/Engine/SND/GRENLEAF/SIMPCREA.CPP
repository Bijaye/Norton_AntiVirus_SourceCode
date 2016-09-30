//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/SIMPCREA.CPv   1.0   12 Mar 1998 12:18:00   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/SIMPCREA.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:18:00   DCHI
// Initial revision.
// 
//************************************************************************

//
// SIMPCREA.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALCreate()
//  ALCreateVB()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//   March 20, 1996     2.01A : Minor changes
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "alsimple.h"
#include "simpmon.h"
#include "pkarc.h"

//
// NAME
//
//  ALCreate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The simplified interface function to create a PKZip file.
//
// C/C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  int ALCreate( char *name,
//                char *file_list,
//                int strip_path,
//                CALLBACK_FN callback )
//
// VB SYNOPSIS
//
//  Declare Function ALCreate Lib "AL20LWD" Alias "ALCreateVB"
//  ( ByVal archive_name$,
//    ByVal input_files$,
//    ByVal strip_path%,
//    ByVal text_window%,
//    ByVal file_progress_window%,
//    ByVal job_progress_window% ) As Integer
//
// DELPHI SYNOPSIS
//
// function ALCreate( archive_name : String;
//                    input_files : String;
//                    strip_path : integer;
//                    text_window : Hwnd;
//                    file_progress_window : Hwnd;
//                    job_progress_window : Hwnd ) : integer;
//
// ARGUMENTS
//
//  archive_name      :  The name of a ZIP file.  It will be created, so
//                       if it already exists, it will be obliterated.
//
//  input_files       :  A list of file names, including wildcards.  File
//                       names should be separated by white space or commas.
//
//  strip_path        :  A flag indicating whether or not the paths should
//                       be stripped from the file names before the names
//                       are written to the ZIP directory.
//
//  callback          :  C and C++ use a callback function for monitoring.
//                       This function gets called periodically with
//                       file names and progress ratios.
//
//  text_window       :  VB and Delphi get file names sent in ASCII to this
//                       text_window.  A value of zero prevents messages
//                       from being sent.
//
//  file_progress_window : VB and Delphi get the percentage of completion
//                         for individual files sent to a window.  These
//                         are ASCII values such as "42".
//
//  job_progress_window : VB and Delphi get the percentage of completion
//                        for the complete job sent to a window.  These
//                        are ASCII values such as "42".
//
// DESCRIPTION
//
//   The simplified ALCReate function just creates a zip file using
//   a batch of inputfiles. You don't get any flexibility when doing it this
//   way, but it is nice and easy.  The only difference between the
//   VB/Delphi and C/C++ versions is that one uses a callback function
//   and the other gets messages via text windows.
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
//   March 20, 1996     2.01A : Two changes.  First, the two char * parameters
//                              to this fn are now const char *.  That
//                              helps C++ programmers who want to pass
//                              CString values.  Second, the toolkit
//                              used by the entry list no longer uses
//                              ALMemory objects.  This is good for the
//                              test drive.
//
//   April 3, 1996      2.01A : Modified to deal with the change in return
//                              type from ALArchive::Create()
//

extern "C" AL_LINKAGE
int AL_FUNCTION ALCreate( const char AL_DLL_FAR *name,
                          const char AL_DLL_FAR *file_list,
                          int strip_path,
                          CALLBACK_FN callback )
{
    ALSimpleMonitor monitor( callback );
    ALPkArchive arc( name );
    arc.mfStripPathOnInsert = strip_path;
    ALEntryList list( &monitor, PkCompressFileTools() );
    list.AddWildCardFiles( file_list );
    list.UnmarkDuplicates( list );
    int i = arc.Create( list );
    if ( i >= 0 )
        return AL_SUCCESS;
    return i;
}

#if defined( AL_VB ) || defined( AL_VB32 )

extern "C" AL_LINKAGE
int AL_FUNCTION ALCreateVB( char AL_DLL_FAR *name,
                            char AL_DLL_FAR *file_list,
                            int strip_path,
                            HWND text_window,
                            HWND file_progress_window,
                            HWND job_progress_window )
{
    ALSimpleMonitor monitor( text_window,
                             file_progress_window,
                             job_progress_window );
    ALPkArchive arc( name );
    arc.mfStripPathOnInsert = strip_path;
    ALEntryList list( &monitor, PkCompressFileTools() );
    list.AddWildCardFiles( file_list );
    list.UnmarkDuplicates( list );
    int i = arc.Create( list );
    if ( i >= 0 )
        return AL_SUCCESS;
    return i;
}

#endif

