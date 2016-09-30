//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_DCR.CPv   1.0   12 Mar 1998 12:15:30   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_DCR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:30   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_DCR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALDecompressorClearError()
//  ALDecompressorSetError()
//  ALDecompressorGetStatusCode()
//  ALDecompressorGetStatusString()
//  ALDecompressorGetStatusStringVB()
//  ALDecompressorGetStatusDetail()
//  ALDecompressorGetStatusDetailVB()
//  ALDecompressorGetTypeCode()
//  ALDecompressorGetTypeString()
//  ALDecompressorGetTypeStringVB()
//
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  pure virtual member functions in ALDecompressor, as well as some
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
//  ALDecompressor::ClearError()
//
//  This function is documented in H/ENGN.INL, where the inline version of the
//  C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALDecompressorClearError( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorClearError" );
    ( (ALDecompressor *) this_object )->ClearError();
}

//
// NAME
//
//  ALDecompressorSetError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the error code and text for a Decompressor engine.
//
// C++ SYNOPSIS
//
//  None, C++ programs can access ALDecompressor::mStatus directly, so they
//  don't need this little crutch routine to do the job for them.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALDecompressorSetError( hALDecompressor this_object,
//                              int error,
//                              char *text );
//
// VB SYNOPSIS
//
//  Declare Function ALDecompressorSetError Lib "AL20LW"
//    (ByVal this_object&, ByVal error_code%, ByVal text$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALDecompressorSetError( this_object : hALDecompressor;
//                                   error_code : Integer;
//                                   text : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALDecompressor object.
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
//  ALName::SetError(), as applied to an ALDecompressor object.
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
ALDecompressorSetError( hALDecompressor this_object,  /* Tag public function */
                        int error,
                        char AL_DLL_FAR *text )
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorSetError" );
    ( (ALDecompressor *) this_object )->mStatus.SetError( error, text );
    return error;
}

//
// NAME
//
//  ALDecompressorGetStatusCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the status code for a decompression engine.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mStatus member of the
//  ALDecompressor object, so they can call
//  ALDecompressor::mStatus.GetStatusCode() directly, instead of resorting to
//  a translation routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALDecompressorGetStatusCode( hALDecompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALDecompressorGetStatusCode Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALDecompressorGetStatusCode( this_object : hALDecompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALDecompressor object that
//                  we want the status code for.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ function
//  ALName::GetStatusCode() as implemented for objects of type
//  ALDecompressor.  For details on how the member
//  function actually works, take a look at OBJNAME.CPP.
//
//  This function is necessary because I don't provide any sort of
//  access function to actually get at the mStatus member of the
//  ALDecompressor.
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
ALDecompressorGetStatusCode( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetStatusCode" );
    return ( (ALDecompressor *) this_object )->mStatus.GetStatusCode();
}

//
// NAME
//
//  ALDecompressorGetStatusString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the short status string for the ALDecompressor status.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mStatus member of the
//  ALDecompressor object, so they can call
//  ALDecompressor::mStatus.GetStatusString() directly, instead of resorting to
//  a translation routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALDecompressorGetStatusString( hALDecompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALDecompressorGetStatusString Lib "AL20LW"
//    Alias "ALDecompressorGetStatusStringVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALDecompressorGetStatusString( this_object : hALDecompressor ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALDecompressor object.
//                  We want to get the string translation of the error
//                  code for this object.
//
// DESCRIPTION
//
//  This is the C wrapper function for the C++ function
//  ALName::GetStatusString(), as implemented for class ALDecompressor.
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
ALDecompressorGetStatusString( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetStatusString" );
    const char *status = ( (ALDecompressor *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALDecompressorGetStatusStringVB( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object , ALDecompressor, "ALDecompressorGetStatusStringVB" );
    const char _far *status = ( (ALDecompressor *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALDecompressorGetStatusStringVB( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object , ALDecompressor, "ALDecompressorGetStatusStringVB" );
    const char *status = ( (ALDecompressor *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALDecompressorGetStatusDetail()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the detailed status string for the ALDecompressor status.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mStatus member of the
//  ALDecompressor object, so they can call
//  ALDecompressor::mStatus.GetStatusDetail() directly, instead of resorting to
//  a translation routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALDecompressorGetStatusDetail( hALDecompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALDecompressorGetStatusDetail Lib "AL20LW"
//    Alias "ALDecompressorGetStatusDetailVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALDecompressorGetStatusDetail( this_object : hALDecompressor ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALDecompressor object.
//                  We want to get the string translation of the error
//                  code for this object.
//
// DESCRIPTION
//
//  This is the C wrapper function for the C++ function
//  ALName::GetStatusDetail(), as implemented for class ALDecompressor.
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
ALDecompressorGetStatusDetail( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetStatusDetail" );
    const char *status = ( (ALDecompressor *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALDecompressorGetStatusDetailVB( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetStatusDetailVB" );
    const char _far *status = ( (ALDecompressor *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALDecompressorGetStatusDetailVB( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetStatusDetailVB" );
    const char *status = ( (ALDecompressor *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALDecompressorGetTypeCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the type code of an ALDecompressor engine.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the miCompressionType member
//  of the ALDecompressor object, so they can acces it without any help.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALDecompressorGetTypeCode( hALDecompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALDecompressorGetTypeCode Lib "AL20LW"
//    (ByVal this_object& ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALDecompressorGetTypeCode( this_object : hALDecompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an object of type
//                 ALDecompressor.
//
// DESCRIPTION
//
//  This function is the C/VB translation function that provides access
//  to the C++ data member ALDecompressor::miCompressionType.  Since
//  C and VB can't access the class object directly, they have to go through
//  this function as an intermediary.
//
// RETURNS
//
//  The decompression engine type, found in enum ALCompressionType in
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
ALDecompressorGetTypeCode( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetTypeCode" );
    return ( (ALDecompressor *) this_object )->miCompressionType;
}

//
// NAME
//
//  ALDecompressorGetTypeString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the type description string for an ALDecompressor engine.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have direct access to the mszCompressionType member
//  of the ALDecompressor object, so they can access it without any help.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALDecompressorGetTypeString( hALDecompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALDecompressorGetTypeString Lib "AL20LW"
//    Alias "ALDecompressorGetTypeStringVB"
//    (ByVal this_object& ) As String
//
// DELPHI SYNOPSIS
//
//  function ALDecompressorGetTypeString( this_object : hALDecompressor ) : PChar;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) an object of type
//                 ALDecompressor.
//
// DESCRIPTION
//
//  This function is the C/VB translation function that provides access
//  to the C++ data member ALDecompressor::mszCompressionType.  Since
//  C and VB can't access the class object directly, they have to go through
//  this function as an intermediary.
//
//  Note that we have a slightly different function that allows VB users to
//  get back a true string object.
//
// RETURNS
//
//  The decompressor engine description string, which is usually defined
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
ALDecompressorGetTypeString( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetTypeString" );
    return (char AL_DLL_FAR *) ( (ALDecompressor *) this_object )->mszCompressionType;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALDecompressorGetTypeStringVB( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetTypeStringVB" );
    const char _far *status = ( (ALDecompressor *) this_object )->mszCompressionType;
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALDecompressorGetTypeStringVB( hALDecompressor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALDecompressor, "ALDecompressorGetTypeStringVB" );
    const char *status = ( (ALDecompressor *) this_object )->mszCompressionType;
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif
