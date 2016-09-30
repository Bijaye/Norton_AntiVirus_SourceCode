//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_CMPO.CPv   1.0   12 Mar 1998 12:15:32   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_CMPO.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:32   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_CMPO.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALCompressedGetStatusCode()
//  ALCompressedSetError()
//  ALCompressedGetStatusString()
//  ALCompressedGetStatusStringVB()
//  ALCompressedGetStatusDetail()
//  ALCompressedGetStatusDetailVB()
//  ALCompressedClearError()
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  pure virtual member functions in ALCompressedObject, as well as some
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
//  May 24, 1994  1.0A   : First release
//
//  August 10, 1994 1.0B : Made a few patches in the #ifdefs around VB
//                         functions, mostly for cosmetic reasons.  I used
//                         to have to test a whole bunch of #defines to
//                         see if I was building a VB DLL.  Now I just
//                         have to test AL_VB.
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

#include "cmpobj.h"
#include "_vbutil.h"

//
// NAME
//
//  ALCompressedGetStatusCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the integer status code for an ALCompressedObject.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the
//  ALCompressedObject::mStatus member, so they can directly call
//  ALCompressedObject::mStatus.GetStatusCode().
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedGetStatusCode( hALCompressed this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressedGetStatusCode Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompressedGetStatusCode( this_object : hALCompressed ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALCompressedObject object.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ access function
//  ALCompressedObject.mStatus::GetStatusCode().  For details on how the
//  member function in ALName works, see ALName::GetStatusCode().
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
ALCompressedGetStatusCode( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedGetStatusCode" );
    return ( (ALCompressedObject *) this_object )->mStatus.GetStatusCode();
}

//
// NAME
//
//  ALCompressedSetError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set an error code for the ALCompressedObject.
//
// C++ SYNOPSIS
//
//  None, C++ programmers can directly access the ALCompressedObject::mStatus
//  member, so they call ALCompressedObject::mStatus.SetError().
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  int ALCompressedSetError( hALCompressed this_object,
//                            int error,
//                            char *text );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressedSetError Lib "AL20LW"
//    (ByVal this_object&, ByVal error%, ByVal test$ ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompressedSetError( this_object : hALCompressed;
//                                 error : Integer;
//                                 text : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALCompressedObject.
//                  We are going to set the object's status member
//                  so that it is in an error state.
//
//  error         : The error code to apply to the object.  Values from
//                  ALDEFS.H are good, but it really doesn't matter as
//                  long as you use a negative number.
//
//  text          : The text of the error message you want to associate with
//                  this error.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALName::SetError(), as applied to an ALCompressedObject.  For more
//  details on how the function actually works, check out OBJNAME.CPP.
//
//  All that happens here is that the arguments are checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
// RETURNS
//
//  Returns the error code that you passed it.
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
ALCompressedSetError( hALCompressed this_object,  /* Tag public function */
                      int error_code,
                      char AL_DLL_FAR *text )
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedSetError" );
    ( (ALCompressedObject *) this_object )->mStatus.SetError( error_code, text );
    return error_code;
}

//
// NAME
//
//  ALCompressedGetStatusString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return the short status string from the ALCompressed::mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALCompressedObject::mStatus
//  member, so they can call ALStatus::GetStatusString() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALCompressedGetStatusString( hALCompressed this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressedGetStatusString Lib "AL20LW"
//    Alias "ALCompressedGetStatusStringVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALCompressedGetStatusString( this_object : hALCompressed ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALCompressedObject.
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
ALCompressedGetStatusString( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedGetStatusString" );
    const char *status = ( (ALCompressedObject *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALCompressedGetStatusStringVB( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedGetStatusStringVB" );
    const char _far *status = ( (ALCompressedObject *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALCompressedGetStatusStringVB( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedGetStatusStringVB" );
    const char *status = ( (ALCompressedObject *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALCompressedGetStatusDetail()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read the status detail message from an ALCompressedObject mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALCompressedObject::mStatus
//  member, so they can call ALStatus::GetStatusDetail() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  char *ALCompressedGetStatusDetail( hALCompressed this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressedGetStatusDetail Lib "AL20LW"
//    Alias "ALCompressedGetStatusDetailVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALCompressedGetStatusDetail( this_object : hALCompressed ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALCompressedObject.
//                  We want to get the detailed string describing this
//                  object's current status.
//
// DESCRIPTION
//
//  This is the C wrapper function for the C++ function
//  ALName::GetStatusDetail(), as implemented for the mStatus member
//  of class ALCompressedObject. Note that we need a slightly different function
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
ALCompressedGetStatusDetail( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedGetStatusDetail" );
    const char *status = ( (ALCompressedObject *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALCompressedGetStatusDetailVB( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedGetStatusDetailVB" );
    const char _far *status = ( (ALCompressedObject *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALCompressedGetStatusDetailVB( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedGetStatusDetailVB" );
    const char *status = ( (ALCompressedObject *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALCompressedObject::ClearError()
//
//  This function is documend in H/CMPOBJ.INL, where the inline version of the
//  C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALCompressedClearError( hALCompressed this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressedObject, "ALCompressedClearError" );
    ( (ALCompressedObject *) this_object )->ClearError();
}

#endif // #if !defined( AL_DEMO )

