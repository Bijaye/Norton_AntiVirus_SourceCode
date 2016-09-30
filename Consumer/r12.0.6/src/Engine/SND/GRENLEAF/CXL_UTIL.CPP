//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_UTIL.CPv   1.0   12 Mar 1998 12:15:38   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_UTIL.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:38   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_UTIL.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  StripFileName()
//  StripFileNameVB()
//  StripPath()
//  StripPathVB()
//
// DESCRIPTION
//
//  This file contains a couple of C/VB functions that are used to
//  mangle file names.  They defied ordinary categorization, so they
//  ended up here.  The deal is that they operate on objects of class
//  ALName, but there is no C/VB translation for ALName.  Because of
//  this, we provide these versions that return native string types.
//
// REVISION HISTORY
//
//  May 25, 1994  1.0A   : First release
//
//  August 10, 1994 1.0B : Made a few patches in the #ifdefs around VB
//                         functions, mostly for cosmetic reasons.  I used
//                         to have to test a whole bunch of #defines to
//                         see if I was building a VB DLL.  Now I just
//                         have to test AL_VB.
//
//  February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "_vbutil.h"

//
// NAME
//
//  StripPath()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Strip the path component from a string that contains a file name.
//
// C++ SYNOPSIS
//
//  None, C++ can use the ALName member function to do this a lot easier.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *StripPath( char *filename );
//
// VB SYNOPSIS
//
//  Declare Function StripPath Lib "AL20LW"
//    Alias "StripPathVB"
//    (ByVal file_name$) As String
//
// DELPHI SYNOPSIS
//
//  function StripPath( file_name : PChar ) : PChar;
//
// ARGUMENTS
//
//  filename  :  An ordinary VB or C string that contains a file name.
//
// DESCRIPTION
//
//  This function takes a file name, and strips off any path and drive
//  information, leaving the file and extension.  These functions are
//  very handy when it comes to wild card expansion, which is why
//  they are in ArchiveLib.
//
//  If you want to see how the C++ member functions perform these
//  amazing feats, see OBJNAME.CPP.
//
//  Note that the C/Delphi version performs an in-place copy.  Since we
//  know that the result is always going to be the same size or smaller
//  than the existing string, this is a safe strategy.
//
// RETURNS
//
//  Either a C or VB string type, with the drive and path stripped,
//  leaving just a filename and extension.  Note that
//  the C version of the function copies over your existing string,
//  whereas the VB version creates a new VB string.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//  February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
StripPath( char AL_DLL_FAR *filename )  /* Tag public function */
{
    ALName temp = filename;
    temp.StripPath();
    strcpy( filename, temp );
    return filename;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
StripPathVB( char AL_DLL_FAR *filename ) /* Tag public function */
{
    ALName temp = filename;
    char _far *p = temp.StripPath();
    if ( !p )
        p = "";
    return ALCreateVBString( p, (unsigned short int) _fstrlen( p ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
StripPathVB( char AL_DLL_FAR *filename ) /* Tag public function */
{
    ALName temp = filename;
    char *p = temp.StripPath();
    if ( !p )
        p = "";
    return SysAllocStringByteLen( p, strlen( p ) );
}

#endif

//
// NAME
//
//  StripFileName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Strip the name component from a file name string, leaving path and drive.
//
// C++ SYNOPSIS
//
//  None, C++ can use the ALName member function to do this a lot easier.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *StripFileName( char *filename );
//
// VB SYNOPSIS
//
//  Declare Function StripFileName Lib "AL20LW"
//    Alias "StripFileNameVB"
//    (ByVal file_name$) As String
//
// DELPHI SYNOPSIS
//
//  function StripFileName( file_name : PChar ) : PChar;
//
// ARGUMENTS
//
//  filename  :  An ordinary VB or C string that contains a file name.
//
// DESCRIPTION
//
//  This function takes a file name, and strips off any name
//  information, leaving the path and drive.  These functions are
//  very handy when it comes to wild card expansion, which is why
//  they are in ArchiveLib.
//
//  If you want to see how the C++ member functions perform these
//  amazing feats, see OBJNAME.CPP.
//
//  Note that the C/Delphi version performs an in-place copy.  Since we
//  know that the result is always going to be the same size or smaller
//  than the existing string, this is a safe strategy.
//
// RETURNS
//
//  Either a C or VB string type, with the name stripped,
//  leaving just a path and drive.  Note that
//  the C version of the function copies over your existing string,
//  whereas the VB version creates a new VB string.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//  February 14, 1996  2.0A : New release
//

extern "C" char AL_DLL_FAR * AL_FUNCTION
StripFileName( char AL_DLL_FAR *filename )  /* Tag public function */
{
    ALName temp = filename;
    temp.StripFileName();
    strcpy( filename, temp );
    return filename;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
StripFileNameVB( char AL_DLL_FAR *filename ) /* Tag public function */
{
    ALName temp = filename;
    char _far *p = temp.StripFileName();
    if ( !p )
        p = "";
    return ALCreateVBString( p, (unsigned short int) _fstrlen( p ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
StripFileNameVB( char AL_DLL_FAR *filename ) /* Tag public function */
{
    ALName temp = filename;
    char *p = temp.StripFileName();
    if ( !p )
        p = "";
    return SysAllocStringByteLen( p, strlen( p ) );
}

#endif


