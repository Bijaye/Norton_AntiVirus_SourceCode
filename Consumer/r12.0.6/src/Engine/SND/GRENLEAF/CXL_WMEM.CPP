//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_WMEM.CPv   1.0   12 Mar 1998 12:15:40   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_WMEM.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:40   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_WMEM.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALWinMemoryGetHandle()
//  ALWinMemoryGetBuffer()
//
// DESCRIPTION
//
//  These two functions provide the C interface to member functions
//  of ALWinMemory.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "memstore.h"

//
// NAME
//
//  ALWinMemoryGetHandle()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the Windows handle for an ALWinMemory object's buffer.
//
// C++ SYNOPSIS
//
//  None. C++ programmers have direct access to the mhUserMemoryHandle
//  member, and thus don't need a translation routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  UINT ALWinMemoryGetHandle( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALWinMemoryGetHandle Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALWinMemoryGetHandle( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to an ALWinMemory object.
//                  Note that in release mode, the library doesn't
//                  get to do any sophisticate type checking on this
//                  argument.  So don't pass it a handle to an ALMemory
//                  or ALHugeMemory object.  You will be sorry if you do!!!
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALWinMemory::mhUserMemoryHandle.  Under C++, this is a public data
//  member that the programmer is free to access or modify.
//
//  This function first tests its only argument for correct type (when in
//  debug mode), then casts and accesses the data member.  The value of
//  the data member is then returned unchanged to the calling C or VB
//  procedure.
//
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

extern "C" AL_LINKAGE UINT AL_FUNCTION
ALWinMemoryGetHandle( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALWinMemory, "ALMemorySetBufferOwner" );
    return (UINT) ( (ALWinMemory *) this_object )->mhUserMemoryHandle;
}

//
// NAME
//
//  ALWinMemoryGetBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Retrieve a pointer to the user buffer being used in an ALWinMemory object.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the mpcUserBuffer
//  members, so they don't need an access routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  char AL_HUGE * ALWinMemoryGetBuffer( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALWinMemoryGetBuffer Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALWinMemoryGetBuffer( this_object : hALStorage ) : PChar;
//
//  Don't know if Delphi can handle 16 bit huge pointers...
//
// ARGUMENTS
//
//  this_object  :  A handle for (pointer to) an ALWinMemory object.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALWinMemory::mpcUserBuffer.  Under C++, this is a public data
//  member that the programmer is free to access or modify.
//
//  This function first tests its only argument for correct type (when in
//  debug mode), then casts and accesses the data member.  The value of
//  the data member is then returned unchanged to the calling C or VB
//  procedure.
//
//  This function is like all of the other translation routines in that
//  it is fairly uninformative.  To get the real information about what
//  is happening in the constructor, take a look at the source for the
//  C++ functions in MEMSTORE.CPP.
//
//  IMPORTANT:  This function should only be used in conjunction
//  with standard memory objects created with new ALWinMemory() or
//  newALWinMemory().  If your memory object was created using
//  ALMemory() or ALHugeMemory(), this function is going to give
//  you big trouble, as in not working!!!!
//
// RETURNS
//
//  A pointer to the memory buffer being uses as a storage object.  The
//  type of the the pointer differs between the various modes of our
//  library.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

extern "C" AL_LINKAGE char AL_HUGE * AL_FUNCTION
ALWinMemoryGetBuffer( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALWinMemory, "ALWinMemoryGetBuffer" );
    return ( (ALWinMemory *) this_object )->mpcUserBuffer;
}


