//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/FULLTOOL.CPv   1.0   12 Mar 1998 12:16:16   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/FULLTOOL.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:16   DCHI
// Initial revision.
// 
//************************************************************************

//
// FULLTOOL.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  FullTools()
//  newALListFullTools()
//
// DESCRIPTION
//
//  The functions needed to create Entry Lists that use the full toolkit.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New release
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "pkengn.h"
#include "glengn.h"
#include "copyengn.h"
#include "filestor.h"
#include "memstore.h"

//
// NAME
//
//  FullTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with everything in it.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit FullTools()
//
// C SYNOPSIS
//
//  None, C programmers should use the shortcut helper function
//  newALListFullTools() to create an entry list with this toolkit.
//
// VB SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListFullTools() to create an entry list with this toolkit.
//
// DELPHI SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListFullTools() to create an entry list with this toolkit.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  An ALToolKit is a factory that things in the ALArchive class can use
//  to clone new objects.  FullTools() creates a toolkit that can
//  build just about any compressor, decompressor, and storage object.
//  If you are going to be extracting from archives, and you aren't sure
//  what toolkit to use, this is a sure-fire winner.
//
// RETURNS
//
//  A new toolkit.  Note that this isn't a pointer, but rather a copy of
//  a new toolkit.  Usually this is copied into an existing toolkit.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//

//
// The following static objects form templates that are used when
// the Clone() functions are called.
//

static ALGlCompressor c1;
static ALPkCompressor c2;
static ALCopyCompressor c3;

static ALGlDecompressor d1;
static ALPkDecompressor d2;
static ALCopyDecompressor d3;

static ALFile s1;
#if defined( AL_WINDOWS ) && !defined( AL_FLAT_MODEL )
static ALWinMemory s2;
#else
static ALMemory s2;
#endif

AL_LINKAGE ALToolKit AL_FUNCTION
FullTools()  /* Tag public function */
{
    return c1 + c2 + c3 + d1 + d2 + d3 + s1 + s2;
}

//
// NAME
//
//  newALListFullTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an entry list that uses the fully loaded toolkit.
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
//  hALEntryList newALListFullTools( hALMonitor monitor );
//
// VB SYNOPSIS
//
//  Declare Function newALListFullTools Lib "AL20LW" (ByVal monitor&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALListFullTools( monitor : hALMonitor ) : hALEntryList;
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
newALListFullTools( hALMonitor monitor ) /* Tag public function */
{
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "newALListFullTools" );
    return (hALEntryList) new ALEntryList( (ALMonitor *) monitor,
                                           FullTools() );
}

#endif

#endif // #if !defined( AL_DEMO )

