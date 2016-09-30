//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_ARC.CPv   1.0   12 Mar 1998 12:15:30   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_ARC.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:30   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_ARCH.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchiveReadDirectory()
//  ALArchiveGetComment()
//  ALArchiveGetCommentVB()
//  ALArchiveGetStorage()
//  ALArchiveSetError()
//  ALArchiveGetStatusCode()
//  ALArchiveGetStatusString()
//  ALArchiveGetStatusStringVB()
//  ALArchiveGetStatusStringVB()
//  ALArchiveGetStatusDetail()
//  ALArchiveGetStatusDetailVB()
//  ALArchiveGetVersion()
//  ALArchiveClearError()
//  ALArchiveSetStripOnInsert()
//  ALArchiveSetStripOnExtract()
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  pure virtual member functions in ALArchive, as well as some
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
//  May 22, 1994  1.0A   : First release
//
//  August 10, 1994 1.0B : Made a few patches in the #ifdefs around VB
//                         functions, mostly for cosmetic reasons.  I used
//                         to have to test a whole bunch of #defines to
//                         see if I was building a VB DLL.  Now I just
//                         have to test AL_VB.
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "_vbutil.h"

//
// NAME
//
//  ALArchive::ReadDirectory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read an archive directory into an ALEntryList object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  virtual int ALArchive::ReadDirectory( ALEntryList &list );
//
//  Note that this is a pure virtual function, and does not have
//  an implementation in the base class ALArchive.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveReadDirectory( hALArchive this_object,
//                              hALEntryList list );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveReadDirectory Lib "AL20LW"
//    (ByVal this_object, ByVal list&)
//
// DELPHI SYNOPSIS
//
//  function ALArchiveReadDirectory( this_object : hALEntryList;
//                                   list : hALEntryList ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchiveBase object.
//                  This is the archive that is going to have its directory
//                  read into the list.  Be sure to note that the C++
//                  member function version of this function doesn't need
//                  'this_object', since it has implicit access to 'this'.
//
//  list          : A handle for (pointer to) an ALEntryList object.
//                  The list is going to receive descriptions for all
//                  of the compressed objects stored in the archive.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ function
//  ALArchiveBase::ReadDirectory(ALEntrylist&).  This is a pure
//  virtual function, so the base class doesn't do anything.  To
//  see what is *really* happening when you call this function, you
//  need to look at ALPkArchive::ReadDirectory() or
//  ALGlArchive::ReadDirectory().
//
//  However, the overview of both versions of the function is identical.
//  The archive is opened, and a directory of ALEntry objects is added
//  to the list.  There should be a single new ALEntry added to the list
//  for each object in the archive.  The actual storage object type,
//  and compression/decompression engine types will be determined by the
//  integer values stored in the archive, combined with the the toolkit
//  owned by the ALEntryList.
//
//
// RETURNS
//
//  AL_SUCCESS for a successful read, < AL_SUCCESS for a failure.
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
ALArchiveReadDirectory( hALArchive this_object,  /* Tag public function */
                        hALEntryList list )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveReadDirectory" );
    AL_ASSERT_OBJECT( list, ALEntryList, "ALArchiveReadDirectory" );
    return ((ALArchive *) this_object )->ReadDirectory( *( (ALEntryList *) list ) );
}

//
// NAME
//
//  ALArchive::GetComment()
//
//  This function is documented in H/ARC.INL, where the inline version of the
//  C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALArchiveGetComment( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetComment" );
    return (char *) ( (ALArchive *) this_object )->GetComment();
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALArchiveGetCommentVB( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetCommentVB" );
    const char _far *status = ( (ALArchive *) this_object )->GetComment();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALArchiveGetCommentVB( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetCommentVB" );
    const char *status = ( (ALArchive *) this_object )->GetComment();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALArchive::GetStorageObject()
//
//  This function is documend in H/ARC.INL, where the inline version of the
//  C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE hALStorage AL_FUNCTION
ALArchiveGetStorage( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT( ((ALArchive *) this_object)->GoodTag(),
               "archive argument is not a valid ALArchive" );
    ALStorage *obj = ((ALArchive *) this_object)->GetStorageObject();
    return (hALStorage) obj;
}

//
// NAME
//
//  ALArchiveSetError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set an error code for the Archive object.
//
// C++ SYNOPSIS
//
//  None, C++ programmers can directly access the ALArchive::mStatus
//  member, so they call ALArchive::mStatus.SetError().
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveSetError( hALArchive this_object,
//                         int error,
//                         char *text );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveSetError Lib "AL20LW"
//    (ByVal this_object&, ByVal error%, ByVal test$ ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveSetError( this_object : hALArchive;
//                              error : Integer;
//                              text : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchive object.
//                  We are going to set the archive's status member
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
//  ALName::SetError(), as applied to an ALArchive object.  For more
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
ALArchiveSetError( hALArchive this_object,  /* Tag public function */
                   int error,
                   char AL_DLL_FAR *text )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveSetError" );
    ( (ALArchive *) this_object )->mStatus.SetError( error, text );
    return error;
}

//
// NAME
//
//  ALArchiveGetStatusCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the integer status code for an Archive.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the ALArchive::mStatus
//  memeber, so they can directly call ALArchive::mStatus.GetStatusCode().
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALArchiveGetStatusCode( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveGetStatusCode Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveGetStatusCode( this_object : hALArchive ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchive object.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ access function
//  ALArchive.mStatus::GetStatusCode().  For details on how the member
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
ALArchiveGetStatusCode( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive , "ALArchiveGetStatusCode" );
    return ( (ALArchive *) this_object )->mStatus.GetStatusCode();
}

//
// NAME
//
//  ALArchiveGetStatusString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return the short status string from the Archive::mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALArchive::mStatus member,
//  so they can call ALStatus::GetStatusString() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALArchiveGetStatusString( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveGetStatusString Lib "AL20LW"
//    Alias "ALArchiveGetStatusStringVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALArchiveGetStatusString( this_object : hALArchive ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchive object.
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
ALArchiveGetStatusString( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetStatusString" );
    const char *status = ( (ALArchive *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALArchiveGetStatusStringVB( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetStatusStringVB" );
    const char _far *status = ( (ALArchive *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALArchiveGetStatusStringVB( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetStatusStringVB" );
    const char *status = ( (ALArchive *) this_object )->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALArchiveGetStatusDetail()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read the status detail message from an ALArchive mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALArchive::mStatus member,
//  so they can call ALStatus::GetStatusDetail() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALArchiveGetStatusDetail( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveGetStatusDetail Lib "AL20LW"
//    Alias "ALArchiveGetStatusDetailVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALArchiveGetStatusDetail( this_object : hALArchive ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchive object.
//                  We want to get the detailed string describing this
//                  object's current status.
//
// DESCRIPTION
//
//  This is the C wrapper function for the C++ function
//  ALName::GetStatusDetail(), as implemented for the mStatus member
//  of class ALArchive. Note that we need a slightly different function
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
ALArchiveGetStatusDetail( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetStatusDetail" );
    const char *status = ( (ALArchive *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALArchiveGetStatusDetailVB( hALArchive archive )  /* Tag public function */
{
    AL_ASSERT_OBJECT( archive, ALArchive, "ALArchiveGetStatusDetailVB" );
    const char _far *status = ( (ALArchive *) archive)->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALArchiveGetStatusDetailVB( hALArchive archive )  /* Tag public function */
{
    AL_ASSERT_OBJECT( archive, ALArchive, "ALArchiveGetStatusDetailVB" );
    const char *status = ( (ALArchive *) archive)->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALArchive::GetVersion()
//
//  This function is documend in H/ARC.INL, where the inline version of the
//  C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALArchiveGetVersion( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveGetVersion" );
    return ( (ALArchive *) this_object )->GetVersion();
}

//
// NAME
//
//  ALArchive::ClearError()
//
//  This function is documented in H/ARC.INL, where the inline version of the
//  C++ member function is found.  The source for the C/Delphi/VB versions
//  can't be inlined, so it is found here.
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALArchiveClearError( hALArchive this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveClearError" );
    ( (ALArchive *) this_object )->ClearError();
}

//
// NAME
//
//  ALArchiveSetStripOnInsert()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the insertion strip flag for an archive object.
//
// C++ SYNOPSIS
//
//  None, C++ programmers can directly access the ALArchive
//  member and set it true or false manually.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALArchiveSetStripOnInsert( hALArchive this_object, int flag )
//
// VB SYNOPSIS
//
//  Declare Sub ALArchiveSetStripOnInsert Lib "AL20LW"
//    (ByVal this_object&, ByVal flag% )
//
// DELPHI SYNOPSIS
//
//  procedure ALArchiveSetStripOnInsert( this_object : hALArchive;
//                                       flag : Integer );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchive object.
//                  We are going to modify the archive's strip memeber.
//
//  flag          : The new setting for the flag.
//
// DESCRIPTION
//
//  When inserting members into an archive, you will frequently be using
//  fully qualified file names, which include a path.  If you want to
//  store just the file name, and throw away the path component, you just
//  set the mfStripOnInsert flag, and the rest is automatic.
//
//  C++ programmers can modify this element directly, so there is no
//  C++ version of this function.
//
// RETURNS
//
//  None, this is a void function.
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
ALArchiveSetStripOnInsert( hALArchive this_object, int flag )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveSetStripOnInsert" );
    ((ALArchive *) this_object)->mfStripPathOnInsert = flag;
}


//
// NAME
//
//  ALArchiveSetStripOnExtract()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the extraction strip flag for an archive object.
//
// C++ SYNOPSIS
//
//  None, C++ programmers can directly access the ALArchive
//  member and set it true or false manually.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALArchiveSetStripOnExtract( hALArchive this_object, int flag )
//
// VB SYNOPSIS
//
//  Declare Sub ALArchiveSetStripOnExtract Lib "AL20LW"
//    (ByVal this_object&, ByVal flag% )
//
// DELPHI SYNOPSIS
//
//  procedure ALArchiveSetStripOnExtract( this_object : hALArchive;
//                                        flag : Integer );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALArchive object.
//                  We are going to modify the archive's strip memeber.
//
//  flag          : The new setting for the flag.
//
// DESCRIPTION
//
//  When extracting objects from an archive, you will frequently be using
//  fully qualified file names, which include a path.  If you want to
//  extract the file into the current directory, and ignore the path
//  component, you just set the mfStripOnExtract flag, and the rest is
//  automatic.
//
//  C++ programmers can modify this element directly, so there is no
//  C++ version of this function.
//
// RETURNS
//
//  None, this is a void function.
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
ALArchiveSetStripOnExtract( hALArchive this_object, int flag )
{
    AL_ASSERT_OBJECT( this_object, ALArchive, "ALArchiveSetStripOnExtract" );
    ((ALArchive *) this_object)->mfStripPathOnExtract = flag;
}


