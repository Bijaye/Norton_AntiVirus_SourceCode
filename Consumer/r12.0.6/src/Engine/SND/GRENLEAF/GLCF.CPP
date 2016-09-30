//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/GLCF.CPv   1.0   12 Mar 1998 12:16:18   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/GLCF.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:18   DCHI
// Initial revision.
// 
//************************************************************************

//
// GLCF.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  GlCompressFileTools()
//  newALListGlCompressFileTools()
//
// DESCRIPTION
//
//  The functions needed to create Entry Lists that use
//  the greenleaf compressor and file objects.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New release
//
//   March 20, 1996     2.01A : Added the #ifdef to exclude this
//                              code from the test drive
//
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
//  GlCompressFileTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit that supports Greenleaf compressors and ALFile objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit GlCompressFileTools(
//      short int compression_level = AL_GREENLEAF_LEVEL_2 or 4 );
//
// C SYNOPSIS
//
//  None, C programmers should use the shortcut helper function
//  newALListGlCompressFileTools() to create an entry list with this toolkit.
//
// VB SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListGlCompressFileTools() to create an entry list with this toolkit.
//
// DELPHI SYNOPSIS
//
//  None, Delphi programmers should use the shortcut helper function
//  newALListGlCompressFileTools() to create an entry list with this toolkit.
//
// ARGUMENTS
//
//  compression_level : An integer value somewhere between AL_GREENLEAF_COPY
//                      and AL_GREENLEAF_LEVEL_4.  ...LEVEL_0 through
//                      ...LEVEL_4 all perform compression at varying levels,
//                      using varying amounts of memory.
//
// DESCRIPTION
//
//  An ALToolKit is a factory that things in the ALArchive class can use
//  to clone new objects.  GlCompressFileTools() creates a toolkit that can
//  build Greenleaf compressors and file objects.  If your app is only
//  going to be compressing files, you can use this toolkit to initialize
//  your ALEntryList, and avoid linking in a bunch of support for
//  ALMemory objects, decompressors, and so on.
//
// RETURNS
//
//  A new toolkit.  Not a reference or a pointer, so you better use this
//  as part of an assignment statement, or as an argument to a ctor.
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
GlCompressFileTools(   /* Tag public function */
    short int compression_level /* = AL_GREENLEAF_LEVEL_2 or 4 */ )
{
    return ALGlCompressor( compression_level ) +
           ALCopyCompressor() +
           ALFile();
}

//
// NAME
//
//  newALListGlCompressFileTools()
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
//  hALEntryList newALListGlCompressFileTools( hALMonitor monitor,
//                                             short int compression_level );
//
// VB SYNOPSIS
//
//  Declare Function newALListGlCompressFileTools Lib "AL20LW"
//    (ByVal monitor&, ByVal compression_level%) As Long
//
// DELPHI SYNOPSIS
//
//  function newALListGlCompressFileTools( monitor : hALMonitor;
//                                         compression_level : Integer ) : hALEntryList;
//
// ARGUMENTS
//
//  monitor : A pointer to a monitor that will be used whenever we are
//            processing objects in the list.
//
//  compression_level : An integer value somewhere between AL_GREENLEAF_COPY
//                      and AL_GREENLEAF_LEVEL_4.  ...LEVEL_0 through
//                      ...LEVEL_4 all perform compression at varying levels,
//                      using varying amounts of memory.
//
//  Note that you can use the C++ default parameter values by simply passing
//  AL_DEFAULT for any of the three numeric parameters shown above.
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
newALListGlCompressFileTools( hALMonitor monitor, /* Tag public function */
                              short int compression_level )
{
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "newALListGlCompressFileTools" );
    if ( compression_level == AL_DEFAULT )
        compression_level = AL_GL_DEFAULT_LEVEL;
    return (hALEntryList) new ALEntryList( (ALMonitor *) monitor,
                                           GlCompressFileTools( compression_level ) );
}

#endif

#endif //#if !defined( AL_DEMO )

