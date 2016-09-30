//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_CR.CPv   1.0   12 Mar 1998 12:15:36   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_CR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:36   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_CR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALCompressorClearError()
//  ALCompressorSetError()
//  ALCompressorGetStatusCode()
//  ALCompressorGetStatusString()
//  ALCompressorGetStatusStringVB()
//  ALCompressorGetStatusDetail()
//  ALCompressorGetStatusDetailVB()
//  ALCompressorGetTypeCode()
//  ALCompressorGetTypeString()
//  ALCompressorGetTypeStringVB()
//
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  pure virtual member functions in ALCompressor, as well as some
//  member access routines.
//
//  Functions that simply translate for an existing C++ function
//  are always found attached to that function.  These functions don't
//  have any existing C functions to attach to, since they implement
//  either pure virtual functions or member access routines.
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
//  ALCompressor::ClearError()
//
//  This function is documented in H/ENGN.INL, where the inline version of the
//  C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALCompressorClearError( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorClearError" );
    ( (ALCompressor *) this_object )->ClearError();
}

//
// NAME
//
//  ALCompressorSetError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the error code and text for a compressor engine.
//
// C++ SYNOPSIS
//
//  None, C++ programs can access ALCompressor::mStatus directly, so they
//  don't need this little crutch routine to do the job for them.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALCompressorSetError( hALCompressor this_object,
//                            int error,
//                            char *text );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressorSetError Lib "AL20LW"
//    (ByVal this_object&, ByVal error_code%, ByVal text$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompressorSetError( this_object : hALCompressor;
//                                 error_code : Integer;
//                                 text : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALCompressor object.
//                  We are going to set the engine's status member
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
//  ALName::SetError(), as applied to an ALCompressor object.
//  For more details on how the function actually works, check out
//  OBJNAME.CPP.
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
ALCompressorSetError( hALCompressor this_object,  /* Tag public function */
                      int error,
                      char AL_DLL_FAR *text )
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorSetError" );
    ( (ALCompressor *) this_object )->mStatus.SetError( error, text );
    return error;
}

//
// NAME
//
//  ALCompressorGetStatusCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the status code for a compression engine.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mStatus member of the
//  ALCompressor object, so they can call
//  ALCompressor::mStatus.GetStatusCode() directly, instead of resorting to
//  a translation routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALCompressorGetStatusCode( hALCompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressorGetStatusCode Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompressorGetStatusCode( this_object : hALCompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALCompressor object that
//                  we want the status code for.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ function
//  ALName::GetStatusCode() as implemented for objects of type
//  ALCompressor.  For details on how the member
//  function actually works, take a look at OBJNAME.CPP.
//
//  This function is necessary because I don't provide any sort of
//  access function to actually get at the mStatus member of the
//  ALCompressor.
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
ALCompressorGetStatusCode( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetStatusCode" );
    return ( (ALCompressor *) this_object )->mStatus.GetStatusCode();
}

//
// NAME
//
//  ALCompressorGetStatusString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the short status string for the ALCompressor status.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mStatus member of the
//  ALCompressor object, so they can call
//  ALCompressor::mStatus.GetStatusString() directly, instead of resorting to
//  a translation routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALCompressorGetStatusString( hALCompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressorGetStatusString Lib "AL20LW"
//    Alias "ALCompressorGetStatusStringVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALCompressorGetStatusString( this_object : hALCompressor ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALCompressor object.
//                  We want to get the string translation of the error
//                  code for this object.
//
// DESCRIPTION
//
//  This is the C wrapper function for the C++ function
//  ALName::GetStatusString(), as implemented for class ALCompressor.
//  Note that we need a slightly different function to return strings
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
ALCompressorGetStatusString( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetStatusString" );
    const char *status = ( (ALCompressor *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALCompressorGetStatusStringVB( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object , ALCompressor, "ALCompressorGetStatusStringVB" );
    const char _far *status = ( (ALCompressor *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALCompressorGetStatusStringVB( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object , ALCompressor, "ALCompressorGetStatusStringVB" );
    const char *status = ( (ALCompressor *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALCompressorGetStatusDetail()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the detailed status string for the ALCompressor status.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mStatus member of the
//  ALCompressor object, so they can call
//  ALCompressor::mStatus.GetStatusDetail() directly, instead of resorting to
//  a translation routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALCompressorGetStatusDetail( hALCompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressorGetStatusDetail Lib "AL20LW"
//    Alias "ALCompressorGetStatusDetailVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALCompressorGetStatusDetail( this_object : hALCompressor ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALCompressor object.
//                  We want to get the string translation of the error
//                  code for this object.
//
// DESCRIPTION
//
//  This is the C wrapper function for the C++ function
//  ALName::GetStatusDetail(), as implemented for class ALCompressor.
//  Note that we need a slightly different function to return strings
//  to VB programmers.
//
// RETURNS
//
//  Always returns a pointer to a detailed status string.
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
ALCompressorGetStatusDetail( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetStatusDetail" );
    const char *status = ( (ALCompressor *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALCompressorGetStatusDetailVB( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetStatusDetailVB" );
    const char _far *status = ( (ALCompressor *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALCompressorGetStatusDetailVB( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetStatusDetailVB" );
    const char *status = ( (ALCompressor *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALCompressorGetTypeCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the type code of an ALCompressor engine.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the miCompressionType member
//  of the ALCompressor object, so they can acces it without any help.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALCompressorGetTypeCode( hALCompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressorGetTypeCode Lib "AL20LW"
//    (ByVal this_object& ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALCompressorGetTypeCode( this_object : hALCompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an object of type
//                 ALCompressor.
//
// DESCRIPTION
//
//  This function is the C/VB translation function that provides access
//  to the C++ data member ALCompressor::miCompressionType.  Since
//  C and VB can't access the class object directly, they have to go through
//  this function as an intermediary.
//
// RETURNS
//
//  The compression engine type, found in enum ALCompressionType in
//  ALDEFS.H.
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
ALCompressorGetTypeCode( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetTypeCode" );
    return ( (ALCompressor *) this_object )->miCompressionType;
}

//
// NAME
//
//  ALCompressorGetTypeString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the type description string for an ALCompressor engine.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mszCompressionType member
//  of the ALCompressor object, so they can access it without any help.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALCompressorGetTypeString( hALCompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALCompressorGetTypeString Lib "AL20LW"
//    Alias "ALCompressorGetTypeStringVB"
//    (ByVal this_object& ) As String
//
// DELPHI SYNOPSIS
//
//  function ALCompressorGetTypeString( this_object : hALCompressor ) : PChar;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an object of type
//                 ALCompressor.
//
// DESCRIPTION
//
//  This function is the C/VB translation function that provides access
//  to the C++ data member ALCompressor::mszCompressionType.  Since
//  C and VB can't access the class object directly, they have to go through
//  this function as an intermediary.
//
//  Note that we have a slightly different function that allows VB users to
//  get back a true string object.
//
// RETURNS
//
//  The compression engine description string, which is usually defined
//  as part of the compression class code.
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
ALCompressorGetTypeString( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetTypeString" );
    return (char AL_DLL_FAR *) ( (ALCompressor *) this_object )->mszCompressionType;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALCompressorGetTypeStringVB( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetTypeStringVB" );
    const char _far *status = ( (ALCompressor *) this_object )->mszCompressionType;
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALCompressorGetTypeStringVB( hALCompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALCompressor, "ALCompressorGetTypeStringVB" );
    const char *status = ( (ALCompressor *) this_object )->mszCompressionType;
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif
