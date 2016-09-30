//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/PKARCF.CPv   1.0   12 Mar 1998 12:17:20   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/PKARCF.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:20   DCHI
// Initial revision.
// 
//************************************************************************

//
// PKARCF.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALPkArchive::ALPkArchive(char *)
//  newALPkArchive()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "filestor.h"
#include "pkarc.h"

//
// NAME
//
//  ALPkArchive::ALPkArchive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  A PKWare Archive constructor that creates a file object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  ALPkArchive::ALPkArchive( const char *file_name );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "pkarc.h"
//
//  hALArchive newALPkArchive( char *name );
//
// VB SYNOPSIS
//
//  Declare Function newALPkArchiveLib "AL20LW" ( ByVal name$ ) As Long
//
// DELPHI SYNOPSIS
//
//  function newALPkArchive( name : String ) : hALArchive;
//
// ARGUMENTS
//
//  name  :  A character string that gives the name of the file that you
//           want to use for this PkArchive.  The file name should be a
//           legal one for your O/S or environment.
//
// DESCRIPTION
//
//  There are two versions of the ALPkArchive constructor.  The first creates
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
//  returns a pointer to a newly created ALPkArchive object.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New Release
//   March 20, 1996     2.01A : Added the nag object for the test drive
//

#if defined( AL_DEMO ) && !defined( AL_BUILDING_DLL )
static _Nag _nag;
#endif

AL_PROTO
ALPkArchive::ALPkArchive( const char AL_DLL_FAR *file_name )  /* Tag public function */
    : ALArchive( new ALFile( file_name ), 1 )
{
#if defined( AL_DEMO ) && defined( AL_BUILDING_DLL )
static _Nag _nag;
#endif
    miVersion = 0x020;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALArchive AL_FUNCTION
newALPkArchive( char AL_DLL_FAR *name ) /* Tag public function */
{
    ALArchive *archive;

    archive = new ALPkArchive( name );
    return (hALArchive) archive;
}

#endif


