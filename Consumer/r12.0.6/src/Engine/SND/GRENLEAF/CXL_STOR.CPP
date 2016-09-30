//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_STOR.CPv   1.0   12 Mar 1998 12:15:34   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_STOR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:34   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_STOR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//   ALStorageSetMonitor()
//   ALStorageDelete()
//   ALStorageSetName()
//   ALStorageWildCardMatch()
//   ALStorageChangeExtension()
//   ALStorageChangeExtensionVB()
//   ALStorageChangeTrailingChar()
//   ALStorageChangeTrailingCharVB()
//   ALStorageGetName()
//   ALStorageGetNameVB()
//   ALStorageGetOldName()
//   ALStorageGetOldNameVB()
//   ALStorageGetStatusCode()
//   ALStorageSetError()
//   ALStorageGetStatusString()
//   ALStorageGetStatusStringVB()
//   ALStorageGetStatusDetail()
//   ALStorageGetStatusDetailVB()
//   ALStorageGetUnixTime()
//   ALStorageToJulian()
//   ALStorageFromJulian()
//   ALStorageSetTimeDateFromStruc()
//   ALStorageGetStrucFromTimeDate()
//   ALStorageGetTime()
//   ALStorageSetTime()
//   ALStorageGetDate()
//   ALStorageSetDate()
//   ALStorageSetTimeDateFromUnix()
//   ALStorageFlushBuffer()
//   ALStorageLoadBuffer()
//   ALStorageRename()
//   ALStorageRenameToBackup()
//   ALStorageSeek()
//   ALStorageUnRename()
//   ALStorageGetType()
//   ALStorageGetDosTime()
//   ALStorageGetDosDate()
//   ALStorageValidTimeDate()
//   ALStorageReadOnly()
//   ALStorageSystem()
//   ALStorageHidden()
//   ALStorageArchive()
//   ALStorageDirectory()
//   ALStorageGetSize()
//   ALStorageIsOpen()
//   ALStorageClearError()
//   ALStorageReadChar()
//   ALStorageWriteChar()
//
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  pure virtual member functions in ALStorage, as well as some
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

#include <time.h>
#include "fileattr.h"
#include "_vbutil.h"

//
// NAME
//
//  ALStorageSetMonitor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Assign a monitor to a storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have public access to ALStorage::mpMonitor, so they
//  don't need an access routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALStorageSetMonitor( hALStorage this_object, hALMonitor monitor );
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageSetMonitor Lib "AL20LW"
//    (ByVal this_object&, ByVal monitor&)
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageSetMonitor( this_object : hALStorage;
//                                 monitor : hALMonitor );
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 is have a new monitor assigned to it.
//
//  monitor      : A handle for (pointer to) an ALMonitor object.  This
//                 object is going to be assigned to the storage object.
//
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to access the
//  C++ data member ALStorage::mpMonitor.  This function checks its
//  two handle arguments for correct type (in debug mode), then casts
//  and assigns.
//
//  Normally, assignment of monitor objects to storage objects is done
//  inside the member functions of ALArchive. However, if you want
//  to use a monitor for some operation you are performing on your own,
//  such as a batch file copy, you will have to use this function (along
//  with a couple of others) to get things to work properly.
//
//  Note that 0 is a valid value for a monitor pointer.
//
//  To see how the monitor actually works, check out BARGRAPH.CPP or
//  WINMON.CPP.
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
ALStorageSetMonitor( hALStorage this_object, /* Tag public function */
                     hALMonitor monitor )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetMonitor" );
    if ( monitor )
        AL_ASSERT_OBJECT( monitor, ALMonitor, "ALStorageSetMonitor" );
    ( (ALStorage *) this_object )->mpMonitor = (ALMonitor *) monitor;
}

//
// NAME
//
//  ALStorage::Delete()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Delete the underlying physical object for an ALStorage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::Delete();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageDelete( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageDelete Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageDelete( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 is going to have its underlying physical object deleted.
//
// DESCRIPTION
//
//  Delete() is used to delete the underlying physical object associated
//  with a storage object, for example, a disk file.  The implementation
//  of this function will vary widely across derived classes.
//
//  Note that this function is pure virtual in the base class, so the
//  C++ function doesn't really exist.  Any call to this function will
//  actually get dispatched to the function in one of the derived classes.
//
// RETURNS
//
//  AL_SUCCESS, or some other error code < 0.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Open(), ALStorage::Create()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageDelete( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageDelete" );
    return ( (ALStorage *) this_object)->Delete();
}

//
// NAME
//
//  ALStorageSetName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Assign a new name to the storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have public access to ALStorage::mName, so they
//  don't need an access routine.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALStorageSetName( hALStorage this_object, char *object_name );
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageSetName Lib "AL20LW"
//    (ByVal this_object&, ByVal object_name$)
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageSetName( this_object : hALStorage;
//                              object_name : PChar );
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 is going to have its name changed.
//
//  object_name  : The new name.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to access the
//  C++ member function ALName::operator =(char*), for the mName member
//  of objects of class ALStorage.  This function checks its single
//  handle argument for correct type (in debug mode), then casts
//  and assigns.
//
//  This function changes the name in the ALStorage object, but it doesn't
//  change the underlying name of the physical object (if there is one.)
//  To do that, you need to call ALStorageRename().
//
//  The C/VB translation code doesn't offer much insight into the operation
//  of ALName. See OBJNAME.CPP for the details on that.
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
ALStorageSetName( hALStorage this_object,  /* Tag public function */
                  char AL_DLL_FAR *object_name )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetName" );
    ( (ALStorage *) this_object )->mName = object_name;
}

//
// NAME
//
//  ALStorageWildCardMatch()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Perform a wild card comparison to the name of the storage object.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have public access to the mName member of
//  ALStorage, and can perform a wild card comparison directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageWildCardMatch( hALStorage this_object, char *pattern );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWildCardMatch Lib "AL20LW"
//    (ByVal this_object&, ByVal pattern$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWildCardMatch( this_object : hALStorage;
//                                   pattern : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object     : A handle for (pointer to) the storage object whose
//                      name you want to test.
//
//  pattern         : A regular expression that will be tested for a match.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to call the
//  ALName::WildCardMatch() C++ member function for the mName data
//  member of class ALStorage.  This function checks the handle argument for
//  correct type (in debug mode), then casts and calls the C++ function.
//
//  The C/VB translation code doesn't offer much insight into the operation
//  of ALName::WildCardMatch().  See OBJNAME.CPP for more information.
//
// RETURNS
//
//  1 for a match, 0 for not.
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
ALStorageWildCardMatch( hALStorage this_object,  /* Tag public function */
                        char AL_DLL_FAR *pattern )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWildCardMatch" );
    return ( (ALStorage *) this_object )->mName.WildCardMatch( pattern );
}

//
// NAME
//
//  ALStorageChangeExtension()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Change the extension of the storage object's name.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have public access to the mName member of
//  ALStorage, and call ALName::ChangeExtension() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALStorageChangeExtension( hALStorage this_object,
//                                  char *new_extension );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageChangeExtension Lib
//    Alias "ALStorageChangeExtensionVB"
//    "AL20LW" (ByVal this_object&, ByVal new_extenstion$) As String
//
// DELPHI SYNOPSIS
//
//  function ALStorageChangeExtension( this_object : hALStorage;
//                                     new_extension : PChar ) : PChar;
//
// ARGUMENTS
//
//  this_object     : A handle for (pointer to) the storage object whose
//                      name you want to test.
//
//  new_extension   : A new three letter (maybe) extension you want
//                    to apply to the object name.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to call the
//  ALName::ChangeExtension() C++ member function for the mName data
//  member of class ALStorage.  This function checks the handle argument for
//  correct type (in debug mode), then casts and calls the C++ function.
//
//  Note that the VB version of this function is almost identical.  However,
//  instead of returning a pointer to a character string, this routine calls
//  ALVBCreateString() to build a VB string, which it returns to the
//  calling module.  Don't use the VB function from C, it will blow up.
//  Don't use the C function from VB, because it returns a string pointer,
//  which VB doesn't know how to deal with.
//
//  The C/VB translation code doesn't offer much insight into the operation
//  of ALName::ChangeExtension().  See OBJNAME.CPP for more information.
//
// RETURNS
//
//  A string pointer (or VB string) containing the file name after the
//  new extension has been applied to it.
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
ALStorageChangeExtension( hALStorage this_object,  /* Tag public function */
                          char AL_DLL_FAR *new_extension )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageChangeExtension" );
    return ( (ALStorage *) this_object )->mName.ChangeExtension( new_extension );
}


#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageChangeExtensionVB( hALStorage this_object,  /* Tag public function */
                            char AL_DLL_FAR *new_extension )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageChangeExtensionVB" );
    char _far * p = ( (ALStorage *) this_object )->mName.ChangeExtension( new_extension );
    if ( !p )
        p = "";
    return ALCreateVBString( p, (unsigned short int) _fstrlen( p ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALStorageChangeExtensionVB( hALStorage this_object,  /* Tag public function */
                            char AL_DLL_FAR *new_extension )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageChangeExtensionVB" );
    char * p = ( (ALStorage *) this_object )->mName.ChangeExtension( new_extension );
    if ( !p )
        p = "";
    return SysAllocStringByteLen( p, strlen( p ) );
}

#endif

//
// NAME
//
//  ALStorageChangeTrailingChar()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Change the last character of the storage object's name.
//
// C++ SYNOPSIS
//
//  None.  C++ programs have public access to the mName member of
//  ALStorage, and call ALName::ChangeTrailingChar() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALStorageChangeTrailingChar( hALStorage this_object,
//                                     char new_char );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageChangeTrailingChar Lib "AL20LW"
//    Alias "ALStorageChangeTrailingCharVB"
//    (ByVal this_object&, ByVal new_char%) As String
//
// DELPHI SYNOPSIS
//
//  function ALStorageChangeTrailingChar( this_object : hALStorage;
//                                        new_char : Char ) : PChar;
//
// ARGUMENTS
//
//  this_object     : A handle for (pointer to) the storage object whose
//                    name you want to change.
//
//  new_char        : A new final character you want to apply to the
//                    object name.  Often a "funny" character, like '~'.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to call the
//  ALName::ChangeTrailingChar() C++ member function for the mName data
//  member of class ALStorage.  This function checks the handle argument for
//  correct type (in debug mode), then casts and calls the C++ function.
//
//  Note that the VB version of this function is almost identical.  However,
//  instead of returning a pointer to a character string, this routine calls
//  ALVBCreateString() to build a VB string, which it returns to the
//  calling module.  Don't use the VB function from C, it will blow up.
//  Don't use the C function from VB, because it returns a string pointer,
//  which VB doesn't know how to deal with.
//
//  The C/VB translation code doesn't offer much insight into the operation
//  of ALName::ChangeTrailingChar().  See OBJNAME.CPP for more information.
//
// RETURNS
//
//  A string pointer (or VB string) containing the file name after the
//  new final character has been applied to it.
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
ALStorageChangeTrailingChar( hALStorage this_object,  /* Tag public function */
                             char new_char )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageChangeTrailingChar" );
    return ( (ALStorage *) this_object )->mName.ChangeTrailingChar( new_char );
}


#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageChangeTrailingCharVB( hALStorage this_object,  /* Tag public function */
                               char new_char )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageChangeTrailingChar" );
    char _far * p = ( (ALStorage *) this_object )->mName.ChangeTrailingChar( new_char );
    if ( !p )
        p = "";
    return ALCreateVBString( p, (unsigned short int) _fstrlen( p ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALStorageChangeTrailingCharVB( hALStorage this_object,  /* Tag public function */
                               char new_char )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageChangeTrailingChar" );
    char * p = ( (ALStorage *) this_object )->mName.ChangeTrailingChar( new_char );
    if ( !p )
        p = "";
    return SysAllocStringByteLen( p, strlen( p ) );
}

#endif

//
// NAME
//
//  ALStorageGetName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the name of the storage object.
//
// C++ SYNOPSIS
//
//  None, C++ code has direct access to the mName member of ALStorage,
//  so it doesn't need one of these dinky access routines.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALStorageGetName( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetName Lib "AL20LW"
//    Alias "ALStorageGetNameVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALStorageGetName( this_object : hALStorage ) : PChar;
//
// ARGUMENTS
//
//  this_object     : A handle for (pointer to) the storage object whose
//                    name you want.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to call the
//  ALName::GetSafeName() C++ member function for the mName data
//  member of class ALStorage.  This function checks the handle argument for
//  correct type (in debug mode), then casts and calls the C++ function.
//
//  Note that the VB version of this function is almost identical.  However,
//  instead of returning a pointer to a character string, this routine calls
//  ALVBCreateString() to build a VB string, which it returns to the
//  calling module.  Don't use the VB function from C, it will blow up.
//  Don't use the C function from VB, because it returns a string pointer,
//  which VB doesn't know how to deal with.
//
//  The C/VB translation code doesn't offer much insight into the operation
//  of ALName::GetSafeName().  See OBJNAME.CPP for more information.
//
// RETURNS
//
//  A string pointer (or VB string) containing the file name.
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
ALStorageGetName( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetName" );
    return (char AL_DLL_FAR *) ( (ALStorage *) this_object )->mName.GetSafeName();
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageGetNameVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetName" );
    const char _far *p = ( (ALStorage *) this_object )->mName.GetSafeName();
    if ( !p )
        p = "";
    return ALCreateVBString( p, (unsigned short int) _fstrlen( p ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALStorageGetNameVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetName" );
    const char *p = ( (ALStorage *) this_object )->mName.GetSafeName();
    if ( !p )
        p = "";
    return SysAllocStringByteLen( p, strlen( p ) );
}

#endif

//
// NAME
//
//  ALStorageGetOldName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the old name of the storage object.
//
// C++ SYNOPSIS
//
//  None, C++ code has direct access to the mName member of ALStorage,
//  so it doesn't need one of these dinky access routines.  It can call
//  GetOldName() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALStorageGetOldName( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetOldName Lib "AL20LW"
//    Alias "ALStorageGetOldNameVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALStorageGetOldName( this_object : hALStorage ) : PChar;
//
// ARGUMENTS
//
//  this_object     : A handle for (pointer to) the storage object whose
//                    old name you want.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to call the
//  ALName::GetSafeOldName() C++ member function for the mName data
//  member of class ALStorage.  This function checks the handle argument for
//  correct type (in debug mode), then casts and calls the C++ function.
//
//  Note that the VB version of this function is almost identical.  However,
//  instead of returning a pointer to a character string, this routine calls
//  ALVBCreateString() to build a VB string, which it returns to the
//  calling module.  Don't use the VB function from C, it will blow up.
//  Don't use the C function from VB, because it returns a string pointer,
//  which VB doesn't know how to deal with.
//
//  The C/VB translation code doesn't offer much insight into the operation
//  of ALName::GetSafeOldName().  See OBJNAME.CPP for more information.
//
// RETURNS
//
//  A string pointer (or VB string) containing the old file name.
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
ALStorageGetOldName( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetOldName" );
    return (char AL_DLL_FAR *) ( (ALStorage *) this_object )->mName.GetSafeOldName();
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageGetOldNameVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetOldNameVB" );
    const char _far *p = ( (ALStorage *) this_object )->mName.GetSafeOldName();
    if ( !p )
        p = "";
    return ALCreateVBString( p, (unsigned short int) _fstrlen( p ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALStorageGetOldNameVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetOldNameVB" );
    const char *p = ( (ALStorage *) this_object )->mName.GetSafeOldName();
    if ( !p )
        p = "";
    return SysAllocStringByteLen( p, strlen( p ) );
}

#endif

//
// NAME
//
//  ALStorageGetStatusCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the integer status code for a storage object.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have direct access to the ALStorage::mStatus
//  member, so they can directly call ALStorage::mStatus.GetStatusCode().
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageGetStatusCode( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetStatusCode Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALSTorageGetStatusCode( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ access function
//  ALStorage::mStatus::GetStatusCode().  For details on how the member
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
//  ALStorageGetStatusDetail()
//  ALStorageGetStatusString()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageGetStatusCode( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStatusCode" );
    return ( (ALStorage *) this_object)->mStatus.GetStatusCode();
}

//
// NAME
//
//  ALStorageSetError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set an error code for the storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programmers can directly access the ALStorage::mStatus
//  member, so they call ALStorage::mStatus.SetError().
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageSetError( hALStorage this_object,
//                         int error,
//                         char *text );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageSetError Lib "AL20LW"
//    (ByVal this_object&, ByVal error%, ByVal test$ ) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageSetError( this_object : hALStorage;
//                              error : Integer;
//                              text : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
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
//  ALName::SetError(), as applied to an ALStorage object.  For more
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
//  ALStorageGetStatusCode()
//  ALStorageGetStatusString()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageSetError( hALStorage this_object,  /* Tag public function */
                   int error,
                   char AL_DLL_FAR *text )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetError" );
    ( (ALStorage *) this_object )->mStatus.SetError( error, text );
    return error;
}

//
// NAME
//
//  ALStorageGetStatusString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return the short status string from the ALStorage::mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALStorage::mStatus member,
//  so they can call ALStatus::GetStatusString() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALStorageGetStatusString( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetStatusString Lib "AL20LW"
//    Alias "ALStorageGetStatusStringVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALStorageGetStatusString( this_object : hALStorage ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
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
//  ALStorageGetStatusDetail()
//  ALStorageGetStatusCode()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageGetStatusString( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStatusString" );
    const char *status = ( (ALStorage *) this_object)->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageGetStatusStringVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStatusStringVB" );
    const char _far *status = ( (ALStorage *) this_object)->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALStorageGetStatusStringVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStatusStringVB" );
    const char *status = ( (ALStorage *) this_object)->mStatus.GetStatusString();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALStorageGetStatusDetail()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Return the detailed status string from the ALStorage::mStatus member.
//
// C++ SYNOPSIS
//
//  None.  C++ programmers have access to the ALStorage::mStatus member,
//  so they can call ALStatus::GetStatusDetail() directly, instead of
//  using this translation function.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  char *ALStorageGetStatusDetail( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetStatusDetail Lib "AL20LW"
//    Alias "ALStorageGetStatusDetailVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALStorageGetStatusDetail( this_object : hALStorage ) : PChar;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We want to get the string translation of the error
//                  code for this object.
//
// DESCRIPTION
//
//  This is the C wrapper function that provides access to the mStatus
//  member.  This routine calls GetStatusDetail for the member, returning
//  a short descriptive character string.
//
//  Note that we have a slightly modified function to return strings
//  to VB programmers.
//
// RETURNS
//
//  Always returns a pointer to the detailed status string.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorageGetStatusCode()
//  ALStorageGetStatusString()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageGetStatusDetail( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStatusDetail" );
    const char *status = ( (ALStorage *) this_object )->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return (char AL_DLL_FAR *) status;
}

#if defined( AL_VB )

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageGetStatusDetailVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStatusDetailVB" );
    const char _far *status = ( (ALStorage *) this_object)->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return ALCreateVBString( status, (unsigned short int) _fstrlen( status ) );
}

#elif defined( AL_VB32 )

extern "C" AL_LINKAGE BSTR AL_FUNCTION
ALStorageGetStatusDetailVB( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStatusDetailVB" );
    const char *status = ( (ALStorage *) this_object)->mStatus.GetStatusDetail();
    if ( status == 0 )
        status = "";
    return SysAllocStringByteLen( status, strlen( status ) );
}

#endif

//
// NAME
//
//  ALStorageGetUnixTime()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Calculate the unix time stamp for the given storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call GetUnixTime() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALStorageGetUnixTime( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetUnixTime Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALStorageGetUnixTime( this_object : hALStorage ) : LongInt;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to retrieve the time/date stamp
//                  for this object.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::GetUnixTime(), as applied the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
//  You won't learn much about much about the way ALTimeDate::GetUnixTime()
//  works by looking at this function.  See TIMEDATE.CPP for the
//  details.
//
// RETURNS
//
//  The time/date of the object in unix format.  Unix format is
//  the format that is actually stored in an archive directory
//  to record the object's time stamp.
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
ALStorageGetUnixTime( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetUnixTime" );
    return ( (ALStorage *) this_object)->mTimeDate.GetUnixTime();
}

//
// NAME
//
//  ALStorageToJulian()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Calculate the Unix version of the julian date for the given storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call ToJulian() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//   long ALStorageToJulian( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageToJulian Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALStorageToJulian( this_object : hALStorage ) : LongInt;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to retrieve the time/date stamp
//                  for this object, in Julian date format.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::ToJulian(), as applied the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
//  You won't learn much about much about the way ALTimeDate::ToJulian()
//  works by looking at this function.  See TIMEDATE.CPP for the
//  details.
//
// RETURNS
//
//  The date of the object in julian date format.
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
ALStorageToJulian( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageToJulian" );
    return ( (ALStorage *) this_object)->mTimeDate.ToJulian();
}

//
// NAME
//
//  ALStorageFromJulian()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the date of the storage object from a unix format Julian date.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call ALTimeDate::FromJulian() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//   void ALStorageFromJulian( hALStorage this_object, long jdn );
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageFromJulian Lib "AL20LW"
//    (ByVal this_object&, ByVal jdn&)
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageFromJulian( this_object : hALStorage; jdn : LongInt );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to set the date stamp
//                  for this object, using Julian date format.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::FromJulian(), as applied the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
//  You won't learn much about much about the way ALTimeDate::FromJulian()
//  works by looking at this function.  See TIMEDATE.CPP for the
//  details.
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
ALStorageFromJulian( hALStorage this_object, long jdn ) /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageFromJulian" );
    ( (ALStorage *) this_object)->mTimeDate.FromJulian( jdn );
}

//
// NAME
//
//  ALStorageSetTimeDateFromStruc()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C
//
// SHORT DESCRIPTION
//
//  Set the storage object time stamp from a C 'struct tm' structure.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call ALTimeDate::SetTimeDate() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALStorageSetTimeDateFromStruc( hALStorage this_object,
//                                      struct tm * time_struct );
//
// VB SYNOPSIS
//
//  None, VB doesn't know about struct tm.
//
// DELPHI SYNOPSIS
//
//  None, Delphi doesn't know about struct tm.
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to set the time/date stamp
//                  for this object, using C RTL 'struct tm' format
//                  as our source.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::SetTimeDate(), as applied the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
//  You won't learn much about much about the way ALTimeDate::SetTimeDate()
//  works by looking at this function.  See TIMEDATE.CPP for the
//  details.
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
ALStorageSetTimeDateFromStruc( hALStorage this_object,  /* Tag public function */
                               struct tm AL_DLL_FAR * time_struct )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetTimeDateFromStruc" );
    AL_ASSERT( time_struct != 0, "Null tm struct passed to ALStorageSetTimeDateFromStruc" );
    ( (ALStorage *) this_object )->mTimeDate.SetTimeDate( time_struct );
}

//
// NAME
//
//  ALStorageGetStrucFromTimeDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C
//
// SHORT DESCRIPTION
//
//  Insert the storage object's time stamp into a C 'struct tm' structure.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call ALTimeDate::GetTimeDate() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALStorageGetStrucFromTimeDate( hALStorage this_object,
//                                      struct tm *time_struct );
//
// VB SYNOPSIS
//
//  None, VB doesn't know about struct tm.
//
// DELPHI SYNOPSIS
//
//  None, Delphi doesn't know about struct tm.
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to get the time/date stamp
//                  for this object, with the result going into
//                  the time date struct used by the C run time library.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::GetTimeDate(), as applied the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
//  You won't learn much about much about the way ALTimeDate::GetTimeDate()
//  works by looking at this function.  See TIMEDATE.CPP for the
//  details.
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
ALStorageGetStrucFromTimeDate( hALStorage this_object,  /* Tag public function */
                               struct tm AL_DLL_FAR *time_struct )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetStrucFromTimeDate" );
    AL_ASSERT( time_struct != 0, "Null tm struct passed to ALStorageGetStrucFromTimeDate" );
    ( (ALStorage *) this_object )->mTimeDate.GetTimeDate( time_struct );
}

//
// NAME
//
//  ALStorageGetTime()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  VB Delphi
//
// SHORT DESCRIPTION
//
//  Get the H:M:S values from the storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage.
//
// C SYNOPSIS
//
//  None, C programs can call ALStorageGetStrucFromTimeDate().
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageGetTime Lib "AL20LWD" (ByVal this_object&,
//                                              hour%,
//                                              minute%,
//                                              second% )
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageGetTime( this_object : hALStorage;
//                              var hour : integer;
//                              var minute : integer;
//                              var second : integer );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to get the time/date stamp
//                  for this object.
//
//  hour          : A pointer to a sixteen bit integer that is going
//                  to get the hours portion of the time stamp.
//
//  minute        : A pointer to a sixteen bit integer that is going
//                  to get the minutes portion of the time stamp.
//
//  second        : A pointer to a sixteen bit integer that is going
//                  to get the seconds portion of the time stamp.
//
// DESCRIPTION
//
//  Since VB and Delphi programmers don't have direct access to the struct
//  tm that is normally used to get and receive time/date stamps, we have
//  four access routines.  This is the access routine that retrieves the
//  time stamp for a storage object.
//
//  The actual work done here is extremely simple.  A call is made to
//  the C++ function that fills in the struct tm object, and the appropriate
//  values are stuffed into the parameters used here.
//
// RETURNS
//
//  Nothing.  Values are returned via the integer pointers passed as args.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#if defined( AL_VB ) || defined( AL_VB32 )

extern "C" AL_LINKAGE void AL_FUNCTION
ALStorageGetTime( hALStorage this_object, /* Tag public function */
                  short int AL_DLL_FAR *hour,
                  short int AL_DLL_FAR *minute,
                  short int AL_DLL_FAR *second )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetTime" );
    AL_ASSERT( hour != 0, "Null hour ptr passed to ALStorageGetTime" );
    AL_ASSERT( minute != 0, "Null minute ptr passed to ALStorageGetTime" );
    AL_ASSERT( second != 0, "Null second ptr passed to ALStorageGetTime" );
    struct tm tblock;
    ( (ALStorage *) this_object)->mTimeDate.GetTimeDate( &tblock );
    *hour = (short int) tblock.tm_hour;
    *minute = (short int) tblock.tm_min;
    *second = (short int) tblock.tm_sec;
}

//
// NAME
//
//  ALStorageSetTime()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  VB Delphi
//
// SHORT DESCRIPTION
//
//  Set the H:M:S values for a storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage.
//
// C SYNOPSIS
//
//  None, C programs can call ALStorageSetTimeDateFromStruc().
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageSetTime Lib "AL20LWD" (ByVal this_object&,
//                                              ByVal hour%,
//                                              ByVal minute%,
//                                              ByVal second% )
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageSetTime( this_object : hALStorage;
//                              hour : integer;
//                              minute : integer;
//                              second : integer );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to set the time/date stamp
//                  for this object.
//
//  hour          : A sixteen bit integer that is going
//                  to be put in the hours portion of the time stamp.
//
//  minute        : A sixteen bit integer that is going
//                  to be put in the minutes portion of the time stamp.
//
//  second        : A sixteen bit integer that is going
//                  to be put in the seconds portion of the time stamp.
//
// DESCRIPTION
//
//  Since VB and Delphi programmers don't have direct access to the struct
//  tm that is normally used to get and receive time/date stamps, we have
//  four access routines.  This is the access routine that sets the
//  time stamp for a storage object.
//
//  The actual work done here is extremely simple.  A call is made to
//  the C++ function that retrieves the struct tm object, and the appropriate
//  values are modified using the parameters used here.  Then, a second call
//  is made to modify the time stamp.
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
ALStorageSetTime( hALStorage this_object, /* Tag public function */
                  short int hour,
                  short int minute,
                  short int second )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetTime" );
    AL_ASSERT( ( hour >= 0 && hour <= 23 ) , "Bad hour value passed to ALStorageSetTime" );
    AL_ASSERT( ( minute >= 0 && minute <= 59), "Bad minute value passed to ALStorageSetTime" );
    AL_ASSERT( ( second >= 0 && second <= 59), "Bad second value passed to ALStorageSetTime" );
    struct tm tblock;
    ( (ALStorage *) this_object)->mTimeDate.GetTimeDate( &tblock );
    tblock.tm_hour = (short int) hour;
    tblock.tm_min = (short int) minute;
    tblock.tm_sec = (short int) second;
    ( (ALStorage *) this_object)->mTimeDate.SetTimeDate( &tblock );
}

//
// NAME
//
//  ALStorageGetDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  VB Delphi
//
// SHORT DESCRIPTION
//
//  Get the M/D/Y values from the storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage.
//
// C SYNOPSIS
//
//  None, C programs can call ALStorageGetStrucFromTimeDate().
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageGetDate Lib "AL20LWD" (ByVal this_object&,
//                                              month%,
//                                              date%,
//                                              year% )
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageGetTime( this_object : hALStorage;
//                              var month : integer;
//                              var date : integer;
//                              var year : integer );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to get the time/date stamp
//                  for this object.
//
//  month         : A pointer to a sixteen bit integer that is going
//                  to get the month portion of the date stamp.  The value
//                  for this element will range from 1-12.
//
//  date          : A pointer to a sixteen bit integer that is going
//                  to get the date portion of the datge stamp. The
//                  value will range from 1 to 31.
//
//  year          : A pointer to a sixteen bit integer that is going
//                  to get the year portion of the time stamp.  Expect
//                  the actual year to be stored here, e.g. 1996.
//
// DESCRIPTION
//
//  Since VB and Delphi programmers don't have direct access to the struct
//  tm that is normally used to get and receive time/date stamps, we have
//  four access routines.  This is the access routine that retrieves the
//  date stamp for a storage object.
//
//  The actual work done here is extremely simple.  A call is made to
//  the C++ function that fills in the struct tm object, and the appropriate
//  values are stuffed into the parameters used here.
//
// RETURNS
//
//  Nothing.  Values are returned via the integer pointers passed as args.
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
ALStorageGetDate( hALStorage this_object, /* Tag public function */
                  short int AL_DLL_FAR *month,
                  short int AL_DLL_FAR *date,
                  short int AL_DLL_FAR *year )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetDate" );
    AL_ASSERT( month != 0, "Null month ptr passed to ALStorageGetDate" );
    AL_ASSERT( date != 0, "Null date ptr passed to ALStorageGetDate" );
    AL_ASSERT( year != 0, "Null year ptr passed to ALStorageGetDate" );
    struct tm tblock;
    ( (ALStorage *) this_object)->mTimeDate.GetTimeDate( &tblock );
    *month = (short int) ( tblock.tm_mon + 1 );
    *date = (short int) tblock.tm_mday;
    *year = (short int) (tblock.tm_year + 1900);
}

//
// NAME
//
//  ALStorageSetDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  VB Delphi
//
// SHORT DESCRIPTION
//
//  Set the M/D/Y values for the storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage.
//
// C SYNOPSIS
//
//  None, C programs can call ALStorageGetStrucFromTimeDate().
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageSetDate Lib "AL20LWD" (ByVal this_object&,
//                                              ByVal month%,
//                                              ByVal date%,
//                                              ByVal year% )
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageSetTime( this_object : hALStorage;
//                              month : integer;
//                              date : integer;
//                              year : integer );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to set the time/date stamp
//                  for this object.
//
//  month         : A sixteen bit integer that is going to be stored
//                  in the month portion of the date stamp.  The value
//                  for this element will range from 1-12.
//
//  date          : A sixteen bit integer that is going to be stored
//                  in the date portion of the date stamp. The
//                  value will range from 1 to 31.
//
//  year          : A sixteen bit integer that is going to be stored
//                  in the year portion of the time stamp.  Expect
//                  the actual year to be found here, e.g. 1996.
//
// DESCRIPTION
//
//  Since VB and Delphi programmers don't have direct access to the struct
//  tm that is normally used to get and receive time/date stamps, we have
//  four access routines.  This is the access routine that sets the
//  date stamp for a storage object.
//
//  The actual work done here is extremely simple.  A call is made to
//  the C++ function that fills in the struct tm object, and the appropriate
//  values are modfied using the parameters found here.  A second call
//  is then made to update the time stamp of the object.
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
ALStorageSetDate( hALStorage this_object, /* Tag public function */
                  short int month,
                  short int date,
                  short int year )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetDate" );
    AL_ASSERT( ( month >= 1 && month <= 12 ) , "Bad month value passed to ALStorageSetDate" );
    AL_ASSERT( ( date >= 1 && date <= 31), "Bad date value passed to ALStorageSetDate" );
    struct tm tblock;
    ( (ALStorage *) this_object)->mTimeDate.GetTimeDate( &tblock );
    tblock.tm_year = year - 1900;
    tblock.tm_mon = month - 1;
    tblock.tm_mday = date;
    ( (ALStorage *) this_object)->mTimeDate.SetTimeDate( &tblock );
}

#endif

//
// NAME
//
//  ALStorageSetTimeDateFromUnix()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the storage object's time date stamp from a UNIX time date stamp.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call ALTimeDate::SetTimeDate() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALStorageSetTimeDateFromUnix( hALStorage this_object,
//                                     long unix_time );
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageSetTimeDateFromUnix Lib "AL20LW"
//    (ByVal this_object&, ByVal unix_time&)
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageSetTimeDateFromUnix( this_object : hALStorage;
//                                          unix_time : LongInt );
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//                  We are going to set the time/date stamp
//                  for this object, using the Unix format time/date
//                  format as a source.
//
//  unix_time     : The time date in UNIX format.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::SetTimeDate(), as applied to the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
//  You won't learn much about much about the way ALTimeDate::SetTimeDate()
//  works by looking at this function.  See TIMEDATE.CPP for the
//  details.
//
//  The UNIX time/date format is what ArchiveLib uses internally in archives
//  created using ALGlArchive.
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
ALStorageSetTimeDateFromUnix( hALStorage this_object,  /* Tag public function */
                              long unix_time )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetTimeDateFromUnix" );
    ( (ALStorage *) this_object )->mTimeDate.SetTimeDate( unix_time );
}

//
// NAME
//
//  ALStorage::FlushBuffer()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Flush the I/O buffer for a given storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::FlushBuffer();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageFlushBuffer( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageFlushBuffer Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageFlushBuffer( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object that
//                  is to be flushed.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALStorage::FlushBuffer().  This is virtual function that will only
//  be implemented by a derived class.  FlushBuffer() is used internally
//  by ALStorage to flush the buffer when a write operation overflows
//  the I/O buffer.  It can be called externally to force the buffer out
//  to the physical storage medium.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), the handle is cast to the correct type, and
//  a call is made to the C++ member function.  The int result from
//  the member function is returned unchanged to the calling C or VB
//  procedure.
//
//  Since this is a pure virtual function in the base class, you won't
//  see any C++ code here.
//
// RETURNS
//
//  A value of AL_SUCCESS if flush operation worked properly.  A return
//  of < AL_SUCCESS indicates an error state of some sort.
//
// EXAMPLE
//
// SEE ALSO
//
//   ALStorage::LoadBuffer()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageFlushBuffer( hALStorage this_object ) /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageFlushBuffer" );
    return ( (ALStorage *) this_object )->FlushBuffer();
}

//
// NAME
//
//  ALStorage::LoadBuffer();
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Load a new I/O buffer for a given storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::LoadBuffer( long address );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageLoadBuffer( hALStorage this_object, long address );
//
// VB SYNOPSIS
//
//  Declare Function AlStorageLoadBuffer Lib "AL20LW"
//    (ByVal this_object&, ByVal address&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageLoadBuffer( this_object : hALStorage;
//                                address : LongInt ) : Integer;
//
// ARGUMENTS
//
//  this_object   : A handle for (pointer to) an ALStorage object.
//
//  address       : The address that we want to load data from.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALStorage::LoadBuffer().  This is virtual function that will generally
//  be implemented by a derived class.  LoadBuffer() is used internally
//  by ALStorage to refresh the I/O buffer when a read operation underflows
//  the I/O buffer.  It can be called externally to force a load from a
//  specific address, equivalent to a Seek() call.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), the handle is cast to the correct type, and
//  a call is made to the C++ member function.  The int result from
//  the member function is returned unchanged to the calling C or VB
//  procedure.
//
//  Since this is a pure virtual function in the base class, you won't
//  see any C++ code here.
//
// RETURNS
//
//  An integer >= 0 if the load operation worked properly.  A return
//  of < AL_SUCCESS indicates an error state of some sort.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::FlushBuffer()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageLoadBuffer( hALStorage this_object,  /* Tag public function */
                     long address )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageLoadBuffer" );
    return ( (ALStorage *) this_object )->LoadBuffer( address );
}

//
// NAME
//
//  ALStorage::Rename()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++ C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Change the name of the underlying storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::Rename( const char *new_name = 0,
//                         int delete_on_clash = 1 );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageRename( hALStorage this_object,
//                       char *new_name,
//                       int delete_on_clash );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageRename Lib "AL20LW"
//    (ByVal this_object&,
//     ByVal new_name$,
//     ByVal delete_on_clash%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageRename( this_object : hALStorage;
//                            new_name : PChar;
//                            delete_on_clash : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object      : A handle for (pointer to) an ALStorage object.
//
//  new_name         : The new name you want to assign to the storage
//                     object.  You can pass a parameter of 0 here to
//                     indicate that you have already assigned the new
//                     name to the mName member of the storage object.
//
//  delete_on_clash  : If it turns out that the storage object name is
//                     already in use, you have a clash.  This can happen,
//                     for example, when renaming a file to a backup.
//                     Setting this flag indicates that if there is a clash,
//                     you want to delete the clashing file and let
//                     your object use the name.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALStorage::Rename().  This is virtual function that will generally
//  be implemented by a derived class.  Rename() operates on the operating
//  system level to reassign the name of an object, such as a file.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), the handle is cast to the correct type, and
//  a call is made to the C++ member function.  The int result from
//  the member function is returned unchanged to the calling C or VB
//  procedure.
//
//  This is a pure virtual function in the base class, which explains why
//  there isn't any C++ code.  Anytime this routine is called, it will
//  cause a virtual function call to a Rename() in a derived class.
//
// RETURNS
//
//  AL_SUCCESS if the rename operation worked properly.  AL_RENAME_ERROR
//  may be returned by derived classes if specific problems occur.
//  Additional error codes < AL_SUCCESS may be returned if some other
//  error was detected.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::UnRename()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageRename( hALStorage this_object,  /* Tag public function */
                 char AL_DLL_FAR *new_name,
                 int delete_on_clash )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageRename" );
    AL_ASSERT( new_name != 0, "NULL new_name in ALStorageRename" );
    return ( (ALStorage *) this_object )->Rename( new_name, delete_on_clash );
}

//
// NAME
//
//  ALStorage::RenameToBackup()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++ C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Change the name of the underlying storage object to a standard backup name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::RenameToBackup( int delete_on_clash = 1 );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageRenameToBackup( hALStorage this_object,
//                               int delete_on_clash );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageRenameToBackup Lib "AL20LW"
//    (ByVal this_object&, ByVal delete_on_clash%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageRenameToBackup( this_object : hALStorage;
//                                    delete_on_clash : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object      : A handle for (pointer to) an ALStorage object
//                     that is going to be renamed.
//
//  delete_on_clash  : If it turns out that the new storage name is
//                     already in use, you have a clash.  This can happen,
//                     for example, when renaming a file to a backup.
//                     Setting this flag indicates that if there is a clash,
//                     you want to delete the clashing file and let
//                     your object use the name.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALStorage::RenameToBackup().  This is virtual function that will
//  generally be implemented by a derived class.  RenameToBackup()
//  operates on the operating system level to reassign the name of an object, such as a file.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), the handle is cast to the correct type, and
//  a call is made to the C++ member function.  The int result from
//  the member function is returned unchanged to the calling C or VB
//  procedure.
//
//  This is a pure virtual function in the base class, which explains why
//  there isn't any C++ code.  Anytime this routine is called, it will
//  cause a virtual function call to a Rename() in a derived class.
//
// RETURNS
//
//  AL_SUCCESS if the rename operation worked properly.  AL_RENAME_ERROR
//  may be returned by derived classes if specific problems occur.
//  Additional error codes < AL_SUCCESS may be returned if some other
//  error was detected.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Rename(), ALStorage::UnRename()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageRenameToBackup( hALStorage this_object,  /* Tag public function */
                         int delete_on_clash )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageRenameToBackup" );
    return ( (ALStorage *) this_object )->RenameToBackup( delete_on_clash );
}

//
// NAME
//
//  ALStorage::Seek()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++ C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Seek to a specified location in the storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::Seek( long address );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageSeek( hALStorage this_object, long address );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageSeek Lib "AL20LW"
//    (ByVal this_object&, ByVal address&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageSeek( this_object : hALStorage;
//                          address : LongInt ) : Integer;
//
// ARGUMENTS
//
//  this_object      : A handle for (pointer to) an ALStorage object.
//                     Note that the C++ member function doesn't have
//                     this argument, since it has implicit access to 'this'.
//
//  delete_on_clash  : The address to seek to within the storage object.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALStorage::Seek().  This is virtual function that will
//  generally be implemented by a derived class.  ALStorage::Seek() often
//  operates on the operating system level to seek to a position in a
//  physical or O/S object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), the handle is cast to the correct type, and
//  a call is made to the C++ member function.  The int result from
//  the member function is returned unchanged to the calling C or VB
//  procedure.
//
//  Since this is a virtual function in the base class, there isn't any
//  C++ code to look at here.  Sorry.
//
// RETURNS
//
//  AL_SUCCESS if the seek operation worked properly.  AL_SEEK_ERROR
//  may be returned by derived classes if specific problems occur.
//  Additional error codes < AL_SUCCESS may be returned if some other
//  error was detected.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::LoadBuffer(), ALStorage::Tell()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageSeek( hALStorage this_object,  /* Tag public function */
               long address )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSeek" );
    return ( (ALStorage *) this_object )->Seek( address );
}

//
// NAME
//
//  ALStorage::UnRename()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++ C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Change the underlying object's name back to the previous name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorage::UnRename( int delete_on_clash = 1 );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageUnRename( hALStorage this_object, int delete_on_clash );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageUnRename Lib "AL20LW"
//    (ByVal this_object&, ByVal delete_on_clash%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageUnRename( this_object : hALStorage;
//                              delete_on_clash : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object      : A handle for (pointer to) an ALStorage object.
//                     Note that the C++ member function doesn't
//                     need this argument.
//
//  delete_on_clash  : If it turns out that the new storage name is
//                     already in use, you have a clash.  This can happen,
//                     for example, when renaming a file to a backup.
//                     Setting this flag indicates that if there is a clash,
//                     you want to delete the clashing file and let
//                     your object use the name.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALStorage::UnRename().  This is a virtual function that will
//  generally be implemented by a derived class.  ALStorage::UnRename()
//  operates on the operating system level to reassign the name of an object,
//  such as a file.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), the handle is cast to the correct type, and
//  a call is made to the C++ member function.  The int result from
//  the member function is returned unchanged to the calling C or VB
//  procedure.
//
//  Since this is a virtual function in the base class, there isn't any
//  C++ code to look at here.  Calls to this function are always going
//  to be routed to an implementation in a derived class.
//
// RETURNS
//
//  AL_SUCCESS if the rename operation worked properly.  AL_RENAME_ERROR
//  may be returned by derived classes if specific problems occur.
//  Additional error codes < AL_SUCCESS may be returned if some other
//  error was detected.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::Rename(), ALStorage::Delete()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageUnRename( hALStorage this_object,  /* Tag public function */
                   int delete_on_clash )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageUnRename" );
    return ( (ALStorage *) this_object )->UnRename( delete_on_clash );
}

//
// NAME
//
//  ALStorageGetType()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the integer type code for this storage object.
//
// C++ SYNOPSIS
//
//  None.  C++ code has access to the public data member miStorageObjectType,
//  so no access function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageGetType( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetType Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageGetType( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 you want to get the type of.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to access the
//  C++ data member ALStorage::miStorageObjectType. This function checks
//  its handle argument for correct type (in debug mode), then casts
//  and accesses ALStorage::miStorageObjectType.  It returns the integer
//  result back to the calling procedure unchanged.
//
// RETURNS
//
//  The integer storage type, as defined in ALDEFS.H.  AL_MEMORY_OBJECT
//  and AL_FILE_OBJECT are popular choices. AL_UNDEFINED is possible
//  as well.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::ALStorage()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageGetType( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetType" );
    return ( (ALStorage *) this_object )->miStorageObjectType;
}

//
// NAME
//
//  ALStorageGetDosTime()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C
//
// SHORT DESCRIPTION
//
//  Calculate the MS-DOS date stamp for the given storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call GetDosTime() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  unsigned short int ALStorageGetDosTime( hALStorage this_object );
//
// VB SYNOPSIS
//
//  None, this doesn't seem to be useful for VB programmers.
//
// DELPHI SYNOPSIS
//
//  None, this doesn't seem to be useful for Delphi programmers.
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 you want to get the DOS time for.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::GetDosTime(), as applied the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
// RETURNS
//
//  An unsigned int that has the file access time in the format
//  used by _dos_setfileattr().
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE unsigned short int AL_FUNCTION
ALStorageGetDosTime( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetDosTime" );
    return ( (ALStorage *) this_object )->mTimeDate.GetDosTime();
}

//
// NAME
//
//  ALStorageGetDosDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C
//
// SHORT DESCRIPTION
//
//  Calculate the MS-DOS date word for the given storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have direct access to the mTimeDate member of
//  ALStorage, so they can call GetDosDate() directly.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  unsigned short int ALStorageGetDosDate( hALStorage this_object );
//
// VB SYNOPSIS
//
//  None, this doesn't seem to be useful for VB programmers.
//
// DELPHI SYNOPSIS
//
//  None, this doesn't seem to be useful for Delphi programmers.
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 you want to get the DOS date for.
//
// DESCRIPTION
//
//  This is the C/VB wrapper function for the C++ member function
//  ALTimeDate::GetDosDate(), as applied the mTimeDate data member
//  of the ALStorage object.
//
//  All that happens here is that the handle argument is checked for correct
//  type (when in debug mode), and a call is made to the appropriate
//  member function, with lots of casting.
//
// RETURNS
//
//  An unsigned int that has the file access date in the format
//  used by _dos_setfileattr().
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

extern "C" AL_LINKAGE unsigned short int AL_FUNCTION
ALStorageGetDosDate( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetDosDate" );
    return ( (ALStorage *) this_object )->mTimeDate.GetDosDate();
}

//
// NAME
//
//  ALStorageValidTimeDate()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Check to see if the time date stamp has been set for a storage object.
//
// C++ SYNOPSIS
//
//  None, C++ programs have access to public data member mTimeDate, and
//  can call Valid() directly for it.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageValidTimeDate( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageValidTimeDate Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageValidTimeDate( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  : A handle for (pointer to) the storage object that
//                 you want to check for validity.
//
// DESCRIPTION
//
//  This is the C/VB translation routine that allows you to access the
//  C++ member function ALTimeDate::Valid() for the mTimeDate
//  member of class ALStorage. This function checks its handle argument for
//  correct type (in debug mode), then casts and calls
//  ALTimeDate::Valid().  It returns the integer result back
//  to the calling procedure unchanged.
//
// RETURNS
//
//  A true or false, to indicate whether a valid time has been
//  loaded into the mTimeDate member of the ALStorage object.
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
ALStorageValidTimeDate( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageValidTimeDate" );
    return ( (ALStorage *) this_object )->mTimeDate.Valid();
}


//
// NAME
//
//  ALStorageReadOnly()
//
//
// See ALFileAttributes::ReadOnly() in fileattr.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageReadOnly( hALStorage storage )  /* Tag public function */
{
    AL_ASSERT_OBJECT( storage, ALStorage, "ALStorageReadOnly" );
    return ( (ALStorage *) storage )->mAttributes.ReadOnly();
}

//
// NAME
//
//  ALStorageSystem()
//
// See ALFileAttributes::System() in fileattr.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageSystem( hALStorage storage )  /* Tag public function */
{
    AL_ASSERT_OBJECT( storage, ALStorage, "ALStorageSystem" );
    return ( (ALStorage *) storage )->mAttributes.System();
}

//
// NAME
//
//  ALStorageHidden()
//
// See ALFileAttributes::Hidden() in fileattr.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageHidden( hALStorage storage )  /* Tag public function */
{
    AL_ASSERT_OBJECT( storage, ALStorage, "ALStorageHidden" );
    return ( (ALStorage *) storage )->mAttributes.Hidden();
}

//
// NAME
//
//  ALStorageArchive()
//
// See ALFileAttributes::Archive() in fileattr.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageArchive( hALStorage storage )  /* Tag public function */
{
    AL_ASSERT_OBJECT( storage, ALStorage, "ALStorageArchive" );
    return ( (ALStorage *) storage )->mAttributes.Archive();
}

//
// NAME
//
//  ALStorageDirectory()
//
// See ALFileAttributes::Directory() in fileattr.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageDirectory( hALStorage storage )  /* Tag public function */
{
    AL_ASSERT_OBJECT( storage, ALStorage, "ALStorageDirectory" );
    return ( (ALStorage *) storage )->mAttributes.Directory();
}

//
// NAME
//
//  ALStorageGetSize()
//
// See ALStorage::GetSize() in ..\h\stor.inl for docs
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALStorageGetSize( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetSize" );
    return ( (ALStorage *) this_object)->GetSize();
}

//
// NAME
//
//  ALStorageIsOpen()
//
// See ALStorage::IsOpen() in stor.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageIsOpen( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageIsOpen" );
    return ( (ALStorage *) this_object )->IsOpen();
}

//
// NAME
//
//  ALStorageClearError()
//
// See ALStorage::ClearError() in stor.inl for docs
//

extern "C" AL_LINKAGE void AL_FUNCTION
ALStorageClearError( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageClearError" );
    ( (ALStorage  *) this_object )->ClearError();
}

//
// NAME
//
//  ALStorageReadChar()
//
// See ALStorage::ReadChar() in stor.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageReadChar( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageReadChar" );
    return ( (ALStorage *) this_object )->ReadChar();
}

//
// NAME
//
//  ALStorageWriteChar()
//
// See ALStorage::WriteChar() in stor.inl for docs
//

extern "C" AL_LINKAGE int AL_FUNCTION
ALStorageWriteChar( hALStorage this_object, int c )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageWriteChar" );
    return ( (ALStorage *) this_object )->WriteChar( c );
}
