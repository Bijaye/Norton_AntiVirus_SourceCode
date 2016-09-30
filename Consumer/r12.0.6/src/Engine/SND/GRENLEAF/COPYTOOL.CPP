//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/COPYTOOL.CPv   1.0   12 Mar 1998 12:15:20   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/COPYTOOL.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:20   DCHI
// Initial revision.
// 
//************************************************************************

//
// COPYTOOL.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  CopyTools()
//  newALListCopyTools()
//
// DESCRIPTION
//
//  The functions needed to create Entry Lists that use the copy toolkit.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New release
//   March 20, 1996     2.01A : Excluded this from the test drive build

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "copyengn.h"
#include "filestor.h"
#include "memstore.h"

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

//
// NAME
//
//  CopyTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with just the Copy engine and standard storage objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit CopyTools()
//
// C SYNOPSIS
//
//  None, C programmers should use the shortcut helper function
//  newALListCopyTools() to create an entry list with this toolkit.
//
// VB SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListCopyTools() to create an entry list with this toolkit.
//
// DELPHI SYNOPSIS
//
//  None, Delphi programmers should use the shortcut helper function
//  newALListCopyTools() to create an entry list with this toolkit.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  An ALToolKit is a factory that things in the ALArchive class can use
//  to clone new objects.  CopyTools() creates a toolkit that can
//  build about any storage object, but only knows about one compression
//  or decompression object: the ALCopyEngine pair.
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
//   February 14, 1996  2.0A : New release
//

AL_LINKAGE ALToolKit AL_FUNCTION
CopyTools()  /* Tag public function */
{
    return ALCopyCompressor() + ALCopyDecompressor() + ALFile()
#if defined( AL_WINDOWS ) && !defined( AL_FLAT_MODEL )
           + ALWinMemory();
#else
           + ALMemory();
#endif
}

//
// NAME
//
//  newALListCopyTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an entry list that uses the copy engine toolkit.
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
//  hALEntryList newALListCopyTools( hALMonitor monitor );
//
// VB SYNOPSIS
//
//  Declare Function newALListCopyTools Lib "AL20LW" (ByVal monitor&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALListCopyTools( monitor : hALMonitor ) : hALEntryList;
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
//   February 14, 1996  2.0A : New release
//

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALEntryList AL_FUNCTION
newALListCopyTools( hALMonitor monitor ) /* Tag public function */
{
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "newALListCopyTools" );
    return (hALEntryList) new ALEntryList( (ALMonitor *) monitor,
                                           CopyTools() );
}

#endif
#endif // #if !defined( AL_DEMO )

