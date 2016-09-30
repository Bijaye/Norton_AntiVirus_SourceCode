//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/GLARCF.CPv   1.0   12 Mar 1998 12:16:18   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/GLARCF.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:18   DCHI
// Initial revision.
// 
//************************************************************************

//
// GLARCF.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALGlArchive::ALGlArchive(char *)
//  newALGlArchive()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New Release
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "filestor.h"
#include "glarc.h"
#include "_openf.h"

//
// NAME
//
//  ALGlArchive::ALGlArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  A Greenleaf Archive constructor that creates a file object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  ALGlArchive::ALGlArchive( const char *file_name );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "glarc.h"
//
//  hALArchive newALGlArchive( char *name );
//
// VB SYNOPSIS
//
//  Declare Function newALGlArchiveLib "AL20LW" ( ByVal name$& ) As Long
//
// DELPHI SYNOPSIS
//
//  function newALGlArchive( name : String ) : hALArchive;
//
// ARGUMENTS
//
//  name  :  A character string that gives the name of the file that you
//           want to use for this GlArchive.  The file name should be a
//           legal one for your O/S or envrionment.
//
// DESCRIPTION
//
//  There are two versions of the ALGlArchive constructor.  The first creates
//  an archive in a storage object.  The second, this function, creates a new
//  ALFile object using the name you specify as an argument.  This is strictly
//  a convenience, it lets you bypass the extra step required to build the
//  file yourself.
//
//  Since the constructor has created the ALFile object on the fly, we ask
//  the ALArchive constructor to set the delete_in_dtor flag.  This means
//  that when the archive is destroyed, the underlying storage object
//  will be destroyed as well.
//
// RETURNS
//
//  When called by C/VB/Delphi, or with operator new in C+, this constructor
//  returns a pointer to a newly created ALGlArchive object.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALGlArchive::ALGlArchive( const char AL_DLL_FAR *file_name )  /* Tag public function */
    : ALArchive( new ALFile( file_name ), 1 )
{
    miVersion = 0x100;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALArchive AL_FUNCTION
newALGlArchive( char AL_DLL_FAR *name ) /* Tag public function */
{
    ALArchive *archive;

    archive = new ALGlArchive( name );
    return (hALArchive) archive;
}

#endif

#endif // #if !defined( AL_DEMO )

