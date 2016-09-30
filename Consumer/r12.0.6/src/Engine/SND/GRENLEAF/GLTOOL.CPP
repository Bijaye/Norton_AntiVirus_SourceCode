//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/GLTOOL.CPv   1.0   12 Mar 1998 12:16:22   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/GLTOOL.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:22   DCHI
// Initial revision.
// 
//************************************************************************

//
// GLTOOL.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  GlTools()
//  newALListGlTools()
//
// DESCRIPTION
//
//  The functions needed to create Entry Lists that use
//  both of the greenleaf engines.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New release
//
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "glengn.h"
#include "copyengn.h"
#include "filestor.h"
#include "memstore.h"

//
// NAME
//
//  GlTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with everything but PK compression/decompression.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit GlTools( short int compression_level = AL_GREENLEAF_LEVEL_2 or 4);
//
// C SYNOPSIS
//
//  None, C programmers should use the shortcut helper function
//  newALListGlTools() to create an entry list with this toolkit.
//
// VB SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListGlTools() to create an entry list with this toolkit.
//
// DELPHI SYNOPSIS
//
//  None, Delphi programmers should use the shortcut helper function
//  newALListGlTools() to create an entry list with this toolkit.
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
//  to clone new objects.  GlTools() creates a toolkit that can
//  build the Greenleaf compressors and decompressors, as well as
//  both ALFile and ALMemory storage objects.
//
// RETURNS
//
//  A new toolkit.  Note that this isn't a pointer, but rather an actual
//  toolkit.  Usually this is copied into an existing toolkit.
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
GlTools(    /* Tag public function */
     short int compression_level /* = AL_GREENLEAF_LEVEL_2 or 4 */ )
{
    return ALGlCompressor( compression_level ) +
           ALCopyCompressor() +
           ALGlDecompressor( compression_level ) +
           ALCopyCompressor() +
           ALFile() +
#if defined( AL_WINDOWS ) && !defined( AL_FLAT_MODEL )
           ALWinMemory();
#else
           ALMemory();
#endif
}

//
// NAME
//
//  newALListGlTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an entry list that uses the Greenleaf toolkit.
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
//  hALEntryList newALListGlTools( hALMonitor monitor,
//                                 short int compression_level );
//
// VB SYNOPSIS
//
//  Declare Function newALListGlTools Lib "AL20LW"
//    (ByVal monitor&, ByVal compression_level%) As Long
//
// DELPHI SYNOPSIS
//
//  function newALListGlTools( monitor : hALMonitor;
//                             compression_level : Integer ) : hALEntryList;
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
newALListGlTools( hALMonitor monitor, /* Tag public function */
                  short int compression_level )
{
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "newALListGlTools" );
    if ( compression_level == AL_DEFAULT )
        compression_level = AL_GL_DEFAULT_LEVEL;
    return (hALEntryList) new ALEntryList( (ALMonitor *) monitor,
                                           GlTools( compression_level ) );
}

#endif

#endif // #if !defined( AL_DEMO )

