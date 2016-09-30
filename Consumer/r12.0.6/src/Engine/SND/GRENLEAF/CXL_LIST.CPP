//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_LIST.CPv   1.0   12 Mar 1998 12:15:26   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_LIST.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:26   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_LIST.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryListClearError()
//  ALEntryListSetMarks()
//  ALEntryListClearMarks()
//  ALEntryListGetStatusCode()
//  ALEntryListGetStatusString()
//  ALEntryListGetStatusStringVB()
//  ALEntryListGetStatusDetail()
//  ALEntryListGetStatusDetailVB()
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  pure virtual member functions in ALEntryList, as well as some
//  member access routines.
//
//  Functions that simply provide a translation layer for an existing C++
//  function are always located in the same file as the C++ function.  The
//  functions in this file don't have any existing C functions to attach
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
//  ALEntryList::ClearError()
//
//  See ..\H\ARCLIST.INL for the documentation of this function.  The INL
//  file contains the inlined C++ member function.  The C version can't
//  be defined as inline, so it resides in this source file.
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALEntryListClearError( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListClearError" );
    ( (ALEntryList *) this_object )->ClearError();
}

//
// NAME
//
//  ALEntryList::SetMarks()
//
//  See ..\H\ARCLIST.INL for the documentation of this function.  The INL
//  file contains the inlined C++ member function.  The C version can't
//  be defined as inline, so it resides in this source file.
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntryListSetMarks( hALEntryList this_object,  /* Tag public function */
                     char AL_DLL_FAR *wild_name )
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListSetMarks" );
    return ( (ALEntryList *) this_object )->SetMarks( wild_name );
}

//
// NAME
//
//  ALEntryList::ClearMarks()
//
//  See ..\H\ARCLIST.INL for the documentation of this function.  The INL
//  file contains the inlined C++ member function.  The C version can't
//  be defined as inline, so it resides in this source file.
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALEntryListClearMarks( hALEntryList this_object,  /* Tag public function */
                       char AL_DLL_FAR *wild_name )
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListClearMarks" );
    return ( (ALEntryList *) this_object )->ClearMarks( wild_name );
}

//
// NAME
//
//  ALEntryListGetStatusCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the integer status code for an entry list.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the ALEntryList::mStatus
//  member, so they can directly call ALEntryList::mStatus.GetStatusCode().
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEntryListGetStatusCode( hALEntryList this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListGetStatusCode Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntryListGetStatusCode( this_object : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALEntryList object.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ access function
//  ALEntryList::mStatus::GetStatusCode().  For details on how the member
//  function in ALName works, see ALName::GetStatusCode().
//
//  All that happens here is that the arguments are checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with some casting.
//
// RETURNS
//
//  An integer that contains the current status code for the object.
//  Note that values of < 0 always indicate an error conditions.
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
ALEntryListGetStatusCode( hALEntryList this_object ) /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListGetStatusCode" );
    return ( (ALEntryList *) this_object )->mStatus.GetStatusCode();
}

//
// NAME
//
//  ALEntryListGetStatusString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return the short status string from the ALEntryList::mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALEntryList::mStatus member,
//  so they can call ALStatus::GetStatusString() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALEntryListGetStatusString( hALEntryList this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListGetStatusString Lib "AL20LW"
//    Alias "ALEntryListGetStatusStringVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALEntryListGetStatusString( this_object : hALEntryList ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALEntryList object.
//                  We want to get the string translation of the error
//                  code for this object.
//
// DESCRIPTION
//
//  This is the C wrapper function that provides access to the mStatus
//  member.  This routine calls GetStatusString for the member, returning
//  a short descriptive character string.
//
//  Note that we have a slightly modified function to return strings
//  to VB programmers.
//
// RETURNS
//
//  Always returns a pointer to a short string translation of the
//  current error code.
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
ALEntryListGetStatusString( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListGetStatusString" );
    const char *status = ( (ALEntryList *) this_object)->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALEntryListGetStatusStringVB( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryLisGetStatusStringVB" );
    const char _far *status = ( (ALEntryList *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALEntryListGetStatusStringVB( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryLisGetStatusStringVB" );
    const char *status = ( (ALEntryList *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALEntryListGetStatusDetail()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read the status detail message from an ALEntryList mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALEntryList::mStatus member,
//  so they can call ALStatus::GetStatusDetail() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALEntryListGetStatusDetail( hALEntryList this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListGetStatusDetail Lib "AL20LW"
//    Alias "ALEntryListGetStatusDetailVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALEntryListGetStatusDetail( this_object : hALEntryList ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALEntryList object.
//                  We want to get the detailed string describing this
//                  object's current status.
//
// DESCRIPTION
//
//  This is the C wrapper function for the C++ function
//  ALName::GetStatusDetail(), as implemented for the mStatus member
//  of class ALEntryList. Note that we need a slightly different function
//  to return strings to VB programmers.
//
// RETURNS
//
//  Always returns a pointer to a status detail message.
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
ALEntryListGetStatusDetail( hALEntryList this_object ) /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListGetStatusString" );
    const char *status = ( (ALEntryList *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALEntryListGetStatusDetailVB( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListGetStatusDetailVB" );
    const char _far *status = ( (ALEntryList *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALEntryListGetStatusDetailVB( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListGetStatusDetailVB" );
    const char *status = ( (ALEntryList *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

