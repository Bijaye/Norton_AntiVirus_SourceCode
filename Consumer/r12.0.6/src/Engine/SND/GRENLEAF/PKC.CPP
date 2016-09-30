//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/PKC.CPv   1.0   12 Mar 1998 12:17:24   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/PKC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:24   DCHI
// Initial revision.
// 
//************************************************************************

//
// PKC.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  PkCompressTools()
//  newALListPkCompressTools()
//
// DESCRIPTION
//
//  The functions needed to create Entry Lists that use
//  the PkWare compressor.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "pkengn.h"
#include "copyengn.h"
#include "filestor.h"
#include "memstore.h"

//
// NAME
//
//  PkCompressTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit that supports PKWare compressors.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit PkCompressTools( int level = 6,
//                             int window_bits = 13 /* or 15 */,
//                             int mem_level = 6 /* or 8 */ );
//
// C SYNOPSIS
//
//  None, C programmers should use the shortcut helper function
//  newALListPkCompressTools() to create an entry list with this toolkit.
//
// VB SYNOPSIS
//
//  None, VB programmers should use the shortcut helper function
//  newALListPkCompressTools() to create an entry list with this toolkit.
//
// DELPHI SYNOPSIS
//
//  None, Delphi programmers should use the shortcut helper function
//  newALListPkCompressTools() to create an entry list with this toolkit.
//
// ARGUMENTS
//
//  level       : A number between 1 and 9.  1 gives the best speed, 9 gives
//                the best compression.
//
//  window_bits : The base 2 logarithm of the number of bytes in the history
//                buffer.  Values between 8 and 15 are legal.  More bits,
//                more memory.
//
//  mem_level   : How much memory should be allocated for the internal
//                compression state. memLevel=1 uses minimum memory but
//                is slow and reduces compression ratio; memLevel=9 uses
//                maximum memory for optimal speed. (quoted from zlib.h)
//
// DESCRIPTION
//
//  An ALToolKit is a factory that things in the ALArchive class can use
//  to clone new objects.  PkCompressTools() creates a toolkit that can
//  build PKWare compressors.  If your app is only going to be compressing
//  some using PKWare, you can use this function to initialize your
//  your ALEntryList, and avoid linking in support for  PKWare expanders
//  and GL compressors and expanders.
//
//  Note that this toolkit will clone ALFile and ALMemory objects.  If you
//  only want to use ALFile objects, try PkCompressFileTools().
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
PkCompressTools( int level /* = 6 */,  /* Tag public function */
                 int window_bits /* = 13 or 15 */,
                 int mem_level /* = 6 or 8 */)
{
    return ALPkCompressor( level, window_bits, mem_level ) +
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
//  newALListPkCompressTools()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an entry list that uses the PKWare compressors toolkit.
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
//  hALEntryList newALListPkCompressTools( hALMonitor monitor,
//                                         int level,
//                                         int window_bits,
//                                         int mem_level );
//
// VB SYNOPSIS
//
//  Declare Function newALListPkCompressTools Lib "AL20LW"
//    (ByVal monitor&, ByVal level%, ByVal window_bits%, ByVal mem_level%) As Long
//
// DELPHI SYNOPSIS
//
//  function newALListPkCompressTools( monitor : hALMonitor;
//                                     level : Integer;
//                                     window_bits : Integer;
//                                     mem_level : Integer ) : hALEntryList;
//
// ARGUMENTS
//
//  monitor : A pointer to a monitor that will be used whenever we are
//            processing objects in the list.
//
//  level       : A number between 1 and 9.  1 gives the best speed, 9 gives
//                the best compression.
//
//  window_bits : The base 2 logarithm of the number of bytes in the history
//                buffer.  Values between 8 and 15 are legal.  More bits,
//                more memory.
//
//  mem_level   : How much memory should be allocated for the internal
//                compression state. memLevel=1 uses minimum memory but
//                is slow and reduces compression ratio; memLevel=9 uses
//                maximum memory for optimal speed. (quoted from zlib.h)
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
newALListPkCompressTools( hALMonitor monitor,  /* Tag public function */
                          int level,
                          int window_bits,
                          int mem_level )
{
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "newALListPkCompressTools" );
    if ( level == AL_DEFAULT )
        level = 6;
    if ( window_bits == AL_DEFAULT )
        window_bits = AL_PK_DEFAULT_WINDOW_BITS;
    if ( mem_level == AL_DEFAULT )
        mem_level = AL_PK_DEFAULT_MEM_LEVEL;
    return (hALEntryList) new ALEntryList( (ALMonitor *) monitor,
                                           PkCompressTools( level,
                                                            window_bits,
                                                            mem_level ) );
}

#endif

#endif // #if !defined( AL_DEMO )

