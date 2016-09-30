//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/PKDF.CPv   1.0   12 Mar 1998 12:17:20   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/PKDF.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:20   DCHI
// Initial revision.
// 
//************************************************************************

//
// PKDF.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  PkDecompressFileTools()
//  newALListPkDecompressFileTools()
//
// DESCRIPTION
//
//  The functions needed to create Entry Lists that use
//  the PkWare decompressor and ALFile objects.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "pkengn.h"
#include "copyengn.h"
#include "filestor.h"

//
// NAME
//
//  PkDecompressFileTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit that supports PKWare decompressors.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit PkDecompressFileTools();
//
// C SYNOPSIS
//
//  None, C programmers should use the shortcut helper function
//  newALListPkDecompressFileTools() to create an entry list with this
//  toolkit.
//
// VB SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListPkDecompressFileTools() to create an entry list with this
//  toolkit.
//
// DELPHI SYNOPSIS
//
//  None, Delphi programmers should use the shortcut helper function
//  newALListPkDecompressFileTools() to create an entry list with this
//  toolkit.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  An ALToolKit is a factory that things in the ALArchive class can use
//  to clone new objects.  PkDecompressFileTools() creates a toolkit that can
//  build PKWare decompressors.  If your app is only going to be
//  decompressing ALFile objects, you can use the toolkit returned
//  by this function in your ALEntryList, and avoid linking in support for
//  compression objects, Greenleaf compressors and decompressors, and
//  ALMemory objects.
//
// RETURNS
//
//  A new toolkit.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_LINKAGE ALToolKit AL_FUNCTION
PkDecompressFileTools()  /* Tag public function */
{
    return ALPkDecompressor() +
           ALCopyDecompressor() +
           ALFile();
}

//
// NAME
//
//  newALListPkDecompressFileTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an entry list that uses the PKWare and ALFile toolkit.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers can create and manipulate toolkits easily.
//  This isn't the case for C/VB/Delphi programmers, so we give them
//  this special function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  hALEntryList newALListPkDecompressFileTools( hALMonitor monitor );
//
// VB SYNOPSIS
//
//  Declare Function newALListPkDecompressFileTools Lib "AL20LW"
//    (ByVal monitor& ) As Long
//
// DELPHI SYNOPSIS
//
//  function newALListPkDecompressFileTools( monitor : hALMonitor ) : hALEntryList;
//
// ARGUMENTS
//
//  monitor : A pointer to a monitor that will be used whenever we are
//            processing objects in the list.
//
// DESCRIPTION
//
// C++ programmers pass a toolkit argument to the ALEntryList constructor,
// which determines what sort of compressors and storage objects they are
// expecting to use.  C/VB/Delphi programmers can do the same thing as well,
// but it's a little bit harder for them.  They have to manually create
// a toolkit, get its handle, then pass that handle to the constructor.
// After that, they need to destroy the toolkit.
//
// As a shortcut way to bypass all that work, we provide this function that
// creates an ALEntryList with the appropriate toolkit.
//
// RETURNS
//
//  A handle to the newly created ALEntryList object.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALEntryList AL_FUNCTION
newALListPkDecompressFileTools( hALMonitor monitor )  /* Tag public function */
{
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "newALListPkDecompressFileTools" );
    return (hALEntryList) new ALEntryList( (ALMonitor *) monitor,
                                           PkDecompressFileTools() );
}

#endif

