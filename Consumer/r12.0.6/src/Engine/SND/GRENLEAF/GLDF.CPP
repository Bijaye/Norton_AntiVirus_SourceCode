//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/GLDF.CPv   1.0   12 Mar 1998 12:16:20   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/GLDF.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:20   DCHI
// Initial revision.
// 
//************************************************************************

//
// GLDF.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  GlDecompressFileTools()
//  newALListGlDecompressFileTools()
//
// DESCRIPTION
//
//  The functions needed to create Entry Lists that use
//  the greenleaf decompressor and file objects.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//
//   March 20, 1996     2.01A : Added the #ifdef to exclude this
//                              code from the test drive

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )

#include "glengn.h"
#include "copyengn.h"
#include "filestor.h"

//
// NAME
//
//  GlDecompressFileTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit that supports Greenleaf decompressors and ALFile objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit GlDecompressFileTools();
//
// C SYNOPSIS
//
//  None, C programmers should use the shortcut helper function
//  newALListGlDecompressFileTools() to create an entry list with this
//  toolkit.
//
// VB SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListGlDecompressFileTools() to create an entry list with this
//  toolkit.
//
// DELPHI SYNOPSIS
//
//  None, Delphi programmers should use the shortcut helper function
//  newALListGlDecompressFileTools() to create an entry list with this
//  toolkit.
//
// ARGUMENTS
//
//  None, we have to get the compression level from the archive, so there
//  isn't any point in defining it here.
//
// DESCRIPTION
//
//  An ALToolKit is a factory that things in the ALArchive class can use
//  to clone new objects.  GlDecompressFileTools() creates a toolkit that can
//  build Greenleaf decompressors and file objects.  If your app is only
//  going to be decompressing files, you can use this toolkit to initialize
//  your ALEntryList, and avoid linking in a bunch of support for
//  ALMemory objects, compressors, and so on.
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
GlDecompressFileTools()  /* Tag public function */
{
    return ALGlDecompressor() + ALCopyDecompressor() + ALFile();
}

//
// NAME
//
//  newALListGlDecompressFileTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an entry list that uses the Greenleaf and ALFile toolkit.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers can create and manipulate toolkits easily.
//  This isn't the case for C/VB/Delphi programmers, so we give them
//  a special function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  hALEntryList newALListGlDecompressFileTools( hALMonitor monitor );
//
// VB SYNOPSIS
//
//  Declare Function newALListGlDecompressFileTools Lib "AL20LW"
//    (ByVal monitor&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALListGlDecompressFileTools( monitor : hALMonitor ) : hALEntryList;
//
// ARGUMENTS
//
//  monitor : A pointer to a monitor that will be used whenever we are
//            processing objects in the list.
//
//            Note that we have to get the compression level from the archive,
//            so there isn't any point in defining it as an argument here.
//
// DESCRIPTION
//
// C++ programmers pass a toolkit argument to the ALEntryList constructor,
// which determines what sort of decompressors and storage objects they are
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
newALListGlDecompressFileTools( hALMonitor monitor ) /* Tag public function */
{
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "newALListGlDecompressFileTools" );
    return (hALEntryList) new ALEntryList( (ALMonitor *) monitor,
                                           GlDecompressFileTools() );
}

#endif

#endif //#if !defined( AL_DEMO )

