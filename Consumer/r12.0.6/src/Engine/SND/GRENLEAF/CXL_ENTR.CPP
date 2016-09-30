//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_ENTR.CPv   1.0   12 Mar 1998 12:15:24   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_ENTR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:24   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_ENTR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryGetCompressedSize()
//  ALEntryGetCrc32()
//  ALEntryGetComment()
//  ALEntryGetCommentVB()
//  ALEntrySetMark()
//  ALEntryClearMark()
//  ALEntrySetMarkState()
//  ALEntryGetMark()
//  ALEntryGetStorage()
//  ALEntrySetStorage()
//  ALEntryGetCompressor()
//  ALEntrySetCompressor()
//  ALEntrySetDecompressor()
//  ALEntryGetCompressedPosition()
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  pure virtual member functions in ALEntry, as well as some
//  member access routines.
//
//  Functions that simply provide a translation layer for an existing C++
//  function are always located in the same file as the C++ function.  The
//  function sign this file don't have any existing C functions to attach
//  to, since they implement either pure virtual functions or member access
//  routines.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "_vbutil.h"

//
// NAME
//
//  ALEntry::GetCompressedSize()
//
//  This function is documend in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALEntryGetCompressedSize( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetCompressedSize" );
    return ( (ALEntry *) this_object )->GetCompressedSize();
}


//
// NAME
//
//  ALEntry::GetCrc32()
//
//  This function is documend in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALEntryGetCrc32( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetCrc32" );
    return ( (ALEntry *) this_object )->GetCrc32();
}

//
// NAME
//
//  ALEntry::GetComment()
//
//  This function is documented in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALEntryGetComment( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetComment" );
    return (char AL_DLL_FAR *) ( (ALEntry *) this_object )->GetComment();
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALEntryGetCommentVB( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetCommentVB" );
    char _far *p = (char _far *) ( (ALEntry *) this_object )->GetComment();
    if ( p == 0 )
        p = "";
    return ALCreateVBString( p, (unsigned short int) _fstrlen( p ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALEntryGetCommentVB( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetCommentVB" );
    char *p = (char *) ( (ALEntry *) this_object )->GetComment();
    if ( p == 0 )
        p = "";
    return SysAllocStringByteLen( p, strlen( p ) );
}

#endif

//
// NAME
//
//  ALEntry::SetMark()
//
//  This function is documented in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALEntrySetMark( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntrySetMark" );
    ((ALEntry *) this_object )->SetMark();
}

//
// NAME
//
//  ALEntry::ClearMark()
//
//  This function is documend in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//


extern "C" AL_LINKAGE void AL_FUNCTION
ALEntryClearMark( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryClearMark" );
    ((ALEntry *) this_object )->ClearMark();
}

//
// NAME
//
//  ALEntry::SetMarkState()
//
//  This function is documend in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//


extern "C" AL_LINKAGE void AL_FUNCTION
ALEntrySetMarkState( hALEntry this_object,  /* Tag public function */
                     short int new_state )
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntrySetMarkState" );
    ((ALEntry *) this_object )->SetMarkState( new_state );
}

//
// NAME
//
//  ALEntry::GetMark()
//
//  This function is documend in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntryGetMark( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetMark" );
    return ( (ALEntry *) this_object )->GetMark();
}

//
// NAME
//
//  ALEntryGetStorage()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return a handle (pointer) to the storage object contained in an ALEntry.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the mpStorageObject member
//  of ALEntry, and thus do not need an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  hALStorage ALEntryGetStorage( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetStorage Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetStorage( this_object : hALEntry ) : hALStorage;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an ALEntry object.
//
// DESCRIPTION
//
//  This function provides a translation layer for C and VB to access the
//  C++ data member ALEntry::mpStorageObject.  For more information on
//  what this data member actually does, see ARCENTRY.CPP.
//
//  Like most of the translation functions, this routine checks the
//  arguments for correct type (in debug mode), then casts this_object
//  to the desired class, and gets the data member.  The data member
//  is passed right back to the calling routine after being cast to the
//  appropriate C/VB type.
//
// RETURNS
//
//  A valid handle for (pointer to) an ALStorage object. It is possible
//  to get a return value of 0, since an ALEntry is not required to have
//  a valid ALStorage object attached to it.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE hALStorage AL_FUNCTION
ALEntryGetStorage( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetStorage" );
    return (hALStorage) ( (ALEntry *) this_object )->mpStorageObject;
}

//
// NAME
//
//  ALEntrySetStorage()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the storage object in an ALEntry object.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the mpStorageObject member
//  of ALEntry, and thus do not need an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALEntrySetStorage( hALEntry this_object, hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntrySetStorage Lib "AL20LW"
//    (ByVal this_object&, ByVal storage&)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntrySetStorage( this_object : hALEntry;
//                               storage : hALStorage );
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an ALEntry object.
//
//  storage      : A handle for (pointer to) an ALStorage object that
//                 will now be attached to the ALEntry object.  Note
//                 that a value of 0 is acceptable.
//
// DESCRIPTION
//
//  This function provides a translation layer for C and VB to access the
//  C++ data member ALEntry::mpStorageObject.  For more information on
//  what this data member actually does, see ARCENTRY.CPP.
//
//  Like most of the translation functions, this routine checks the
//  arguments for correct type (in debug mode), then casts this_object
//  to the desired class, and accesses the data member.
//
// RETURNS
//
//  Nothing
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
ALEntrySetStorage( hALEntry this_object,  /* Tag public function */
                   hALStorage storage )
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntrySetStorage" );
    if ( storage != 0 )
        AL_ASSERT_OBJECT( storage, ALStorage, "ALEntrySetStorage" );
    ( (ALEntry *) this_object )->mpStorageObject = (ALStorage *) storage;
}

//
// NAME
//
//  ALEntryGetCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return a handle (pointer) to the compressor contained in an ALEntry.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the mpCompressor member
//  of ALEntry, and thus do not need an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  hALCompressor ALEntryGetCompressor( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetCompressor Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetCompressor( this_object : hALEntry ) : hALCompressor;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an ALEntry object.
//
// DESCRIPTION
//
//  This function provides a translation layer for C and VB to access the
//  C++ data member ALEntry::mpCompressor.  For more information on
//  what this data member actually does, see ARCENTRY.CPP.
//
//  Like most of the translation functions, this routine checks the
//  arguments for correct type (in debug mode), then casts this_object
//  to the desired class, and gets the data member.  The data member
//  is passed right back to the calling routine after being cast to the
//  appropriate C/VB type.
//
// RETURNS
//
//  A valid handle for (pointer to) an ALCompressor object. It is possible
//  to get a return value of 0, since an ALEntry is not required to have
//  a valid ALCompressor object attached to it.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE hALCompressor AL_FUNCTION
ALEntryGetCompressor( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetCompressor" );
    return (hALCompressor) ( (ALEntry *) this_object )->mpCompressor;
}

//
// NAME
//
//  ALEntryGetDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return a handle (pointer) to the decompressor contained in an ALEntry.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the mpDecompressor member
//  of ALEntry, and thus do not need an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  hALDecompressor ALEntryGetDecompressor( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetDecompressor Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetDecompressor( this_object : hALEntry ) : hALDecompressor;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an ALEntry object.
//
// DESCRIPTION
//
//  This function provides a translation layer for C and VB to access the
//  C++ data member ALEntry::mpDecompressor.  For more information on
//  what this data member actually does, see ARCENTRY.CPP.
//
//  Like most of the translation functions, this routine checks the
//  arguments for correct type (in debug mode), then casts this_object
//  to the desired class, and gets the data member.  The data member
//  is passed right back to the calling routine after being cast to the
//  appropriate C/VB type.
//
// RETURNS
//
//  A valid handle for (pointer to) an ALDecompressor object. It is possible
//  to get a return value of 0, since an ALEntry is not required to have
//  a valid ALDecompressor object attached to it.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE hALDecompressor AL_FUNCTION
ALEntryGetDecompressor( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetDecompressor" );
    return (hALDecompressor) ( (ALEntry *) this_object )->mpDecompressor;
}

//
// NAME
//
//  ALEntrySetCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the compressor object in an ALEntry object.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the mpCompressor member
//  of ALEntry, and thus do not need an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALEntrySetCompressor( hALEntry this_object, hALCompressor compressor );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntrySetCompressor Lib "AL20LW"
//    (ByVal this_object&, ByVal compressor&)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntrySetCompressor( this_object : hALEntry;
//                                  compressor : hALCompressor );
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an ALEntry object.
//
//  compressor   : A handle for (pointer to) an ALCompressor that
//                 will now be attached to the ALEntry object. Note
//                 that a value of 0 is acceptable.
//
// DESCRIPTION
//
//  This function provides a translation layer for C and VB to access the
//  C++ data member ALEntry::mpCompressor.  For more information on
//  what this data member actually does, see ARCENTRY.CPP.
//
//  Like most of the translation functions, this routine checks the
//  arguments for correct type (in debug mode), then casts this_object
//  to the desired class, and accesses the data member.
//
// RETURNS
//
//  Nothing
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALEntrySetCompressor( hALEntry this_object,  /* Tag public function */
                      hALCompressor compressor)
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntrySetCompressor" );
    if ( compressor!= 0 )
        AL_ASSERT_OBJECT( compressor, ALCompressor, "ALEntrySetCompressor" );
    ( (ALEntry *) this_object )->mpCompressor = (ALCompressor *) compressor;
}

//
// NAME
//
//  ALEntrySetDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the decompressor object in an ALEntry object.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the mpDecompressor member
//  of ALEntry, and thus do not need an access function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALEntrySetDecompressor( hALEntry this_object,
//                               hALDecompressor decompressor );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntrySetDecompressor Lib "AL20LW"
//    (ByVal this_object&, ByVal decompressor&)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntrySetDecompressor( this_object : hALEntry;
//                                    decompressor : hALDecompressor );
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an ALEntry object.
//
//  decompressor : A handle for (pointer to) an ALDecompressor that
//                 will now be attached to the ALEntry object. Note
//                 that a value of 0 is acceptable.
//
// DESCRIPTION
//
//  This function provides a translation layer for C and VB to access the
//  C++ data member ALEntry::mpDecompressor.  For more information on
//  what this data member actually does, see ARCENTRY.CPP.
//
//  Like most of the translation functions, this routine checks the
//  arguments for correct type (in debug mode), then casts this_object
//  to the desired class, and accesses the data member.
//
// RETURNS
//
//  Nothing
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
ALEntrySetDecompressor( hALEntry this_object,  /* Tag public function */
                        hALDecompressor decompressor)
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntrySetDecompressor" );
    if ( decompressor!= 0 )
        AL_ASSERT_OBJECT( decompressor, ALDecompressor, "ALEntrySetDecompressor" );
    ( (ALEntry *) this_object )->mpDecompressor = (ALDecompressor *) decompressor;
}

//
// NAME
//
//  ALEntry::GetCompressedPosition()
//
//  This function is documend in H/ARCENTRY.INL, where the inline version of
//  the C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALEntryGetCompressedPosition( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetCompressedPosition" );
    return ( (ALEntry *) this_object )->GetCompressedObjectPosition();
}

