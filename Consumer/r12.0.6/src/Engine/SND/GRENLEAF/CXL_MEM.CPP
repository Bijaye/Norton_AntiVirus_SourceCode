//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_MEM.CPv   1.0   12 Mar 1998 12:15:24   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_MEM.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:24   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_MEM.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALMemoryBaseSetBufferOwner()
//  ALMemoryBaseGetBufferSize()
//  ALMemoryBaseGetBufferOwner()
//
//
// DESCRIPTION
//
//  This file contains the C and VB translation routines to access
//  class ALMemoryBase. Most of the ALStorage routines need to be called
//  via the base class, so most of the functions to use ALMemory
//  from C or VB will be found in the ALStorage routines.
//
//  Functions that simply provide a translation layer for an existing C++
//  function are always located in the same file as the C++ function.  The
//  functions in this file don't have any existing C functions to attach
//  to, since they implement either pure virtual functions or member access
//  routines.
//
// REVISION HISTORY
//
//  May 24, 1994  1.0A   : First release
//
//  July 7, 1994  1.0B   : Added ALMemoryCopyBufferVB() for better VB support
//                         of memory objects.
//
//  August 10, 1994 1.0B : Made a few patches in the #ifdefs around VB
//                         functions, mostly for cosmetic reasons.  I used
//                         to have to test a whole bunch of #defines to
//                         see if I was building a VB DLL.  Now I just
//                         have to test AL_VB.
//
//   February 14, 1996  2.0A  : New release
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

#include "memstore.h"

//
// NAME
//
//  ALMemoryBaseSetBufferOwner()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the value of mfUserOwnsBuffer for an ALMemoryBase object.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the member mfUserOwnsBuffer
//  of ALMemoryBase, so they don't have to have an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  void ALMemoryBaseSetBufferOwner( hALStorage this_object,
//                                   int user_owns_buffer );
//
// VB SYNOPSIS
//
//  Declare Sub ALMemoryBaseSetBufferOwner Lib "AL20LW"
//    (ByVal this_object&, ByVal user_owns_buffer%)
//
// DELPHI SYNOPSIS
//
//  procedure ALMemoryBaseSetBufferOwner( this_object : hALStorage;
//                                        user_owns_buffer : Integer );
//
// ARGUMENTS
//
//  this_object      :  A handle for (pointer to) an ALMemoryBase object.
//
//  user_owns_buffer : The new setting of the flag.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMemoryBase::mfUserOwnsBuffer.  Why would you want to change this
//  data member?  Well, if the ALMemoryBase object is currently the buffer
//  owner, the buffer will be deleted when the ALMemory object is destroyed.
//  You can set the owner of the buffer to be you, the user, and copy
//  the pointer to it.  Then you get to keep it long after the ALMemoryBase
//  object is done with it.
//
//  This function first tests its only argument for correct type (when in
//  debug mode), then casts and modifies the data member.
//
//  This function is like all of the other translation routines in that
//  it is fairly uninformative.  To get the real information about what
//  is happening in the constructor, take a look at the source for the
//  C++ functions in MEMSTORE.CPP.
//
//  Note that this is a universal memory function.  It works with
//  ALMemory, ALWinMemory, and ALHugeMemory.  That isn't the case
//  with all of the memory functions, so be careful!
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALMemoryBaseSetBufferOwner( hALStorage this_object,  /* Tag public function */
                            int user_owns_buffer )
{
    AL_ASSERT_OBJECT( this_object, ALMemoryBase, "ALMemoryBaseSetBufferOwner" );
    ( (ALMemoryBase *) this_object )->mfUserOwnsBuffer = user_owns_buffer;
}

//
// NAME
//
//  ALMemoryBaseGetBufferSize()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Retrieve the user buffer size for an ALMemoryBase object.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the mlUserBufferSize
//  members, so they don't need an access routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  long ALMemoryBaseGetBufferSize( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALMemoryBaseGetBufferSize LIb "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALMemoryBaseGetBufferSize( this_object : hALStorage ) : LongInt;
//
// ARGUMENTS
//
//  this_object  :  A handle for (pointer to) an ALMemoryBase object.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMemoryBase::mlUserBufferSize.  Under C++, this is a public data
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
//  Note that this is a universal memory function.  It works with
//  ALMemory, ALWinMemory, and ALHugeMemory.  That isn't the case
//  with all of the memory functions, so be careful!
//
// RETURNS
//
//  The size of the memory buffer being used as a storage object.  The
//  type of the the size differs between Windows and DOS, simply
//  because Windows can access a lot more memory, especially when
//  using Win32.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALMemoryBaseGetBufferSize( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALMemoryBase, "ALMemoryBaseGetBufferSize" );
    return ( (ALMemoryBase *) this_object )->mlUserBufferSize;
}

//
// NAME
//
//  ALMemoryBaseGetBufferOwner()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Access the mfUserOwnsBuffer member of an ALMemoryBase object.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the member mfUserOwnsBuffer
//  of ALMemoryBase, so they don't have to have an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "memstore.h"
//
//  int ALMemoryBaseGetBufferOwner( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALMemoryBaseGetBufferOwner Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALMemoryBaseGetBufferOwner( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A handle for (pointer to) an ALMemoryBase object.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMemoryBase::mfUserOwnsBuffer.
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
//  Note that this is a universal memory function.  It works with
//  ALMemory, ALWinMemory, and ALHugeMemory.  That isn't the case
//  with all of the memory functions, so be careful!
//
// RETURNS
//
//  An integer flag indicating whether the user owns the buffer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALMemoryBaseGetBufferOwner( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALMemoryBase, "ALMemoryBaseGetBufferOwner" );
    return ( (ALMemoryBase *) this_object )->mfUserOwnsBuffer;
}

//
// NAME
//
//  ALMemoryGetBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C
//
// SHORT DESCRIPTION
//
//  Retrieve the user buffer being used in an ALMemory object.
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
//  char * ALMemoryGetBuffer( hALStorage this_object );
//
// VB SYNOPSIS
//
//  None, VB can only use ALWinMemoryGetBuffer().
//
// DELPHI SYNOPSIS
//
//  None, Delphi can only use ALWinMemoryGetBuffer().
//
// ARGUMENTS
//
//  this_object  :  A handle for (pointer to) an ALMemory object.
//                  This better not be ALHugeMemory or ALWinMemory!
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMemory::mpcUserBuffer.  Under C++, this is a public data
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
//  with standard memory objects created with new ALMemory() or
//  newALMemory().  If your memory object was created using
//  ALHugeMemory() or ALWinMemory(), this function is going to give
//  you big trouble, as in not working!!!!
//
// RETURNS
//
//  A pointer to the memory buffer being uses as a storage object.  The
//  type of the the pointer differs between the various modes of our
//  library.  This buffer is limited to 64Kbytes under real mode DOS
//  or Win16.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALMemoryGetBuffer( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALMemory, "ALMemoryGetBuffer" );
    return ( (ALMemory *) this_object )->mpcUserBuffer;
}

//
// NAME
//
//  ALHugeMemoryGetBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C
//
// SHORT DESCRIPTION
//
//  Retrieve the user buffer being used in an ALHugeMemory object.
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
//  char _huge * ALHugeMemoryGetBuffer( hALStorage this_object );
//
// VB SYNOPSIS
//
//  None, VB can only use ALWinMemoryGetBuffer().
//
// DELPHI SYNOPSIS
//
//  None, Delphi can only use ALWinMemoryGetBuffer().
//
// ARGUMENTS
//
//  this_object  :  A handle for (pointer to) an ALHugeMemory object.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALHugeMemory::mpcUserBuffer.  Under C++, this is a public data
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
//  with standard memory objects created with new ALHugeMemory() or
//  newALHugeMemory().  If your memory object was created using
//  ALMemory() or ALWinMemory(), this function is going to give
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
//   February 14, 1996  2.0A : New release
//

#if !defined( AL_FLAT_MODEL )

extern "C" AL_LINKAGE char _huge * AL_FUNCTION
ALHugeMemoryGetBuffer( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALHugeMemory, "ALHugeMemoryGetBuffer" );
    return ( (ALHugeMemory *) this_object )->mpcUserBuffer;
}

#endif

#endif // #if !defined( AL_DEMO )

