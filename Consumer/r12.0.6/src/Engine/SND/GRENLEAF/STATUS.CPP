//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STATUS.CPv   1.0   12 Mar 1998 12:17:46   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STATUS.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:46   DCHI
// Initial revision.
// 
//************************************************************************

//
// STATUS.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) 1994-1996 Greenleaf Software, Inc.
//  All Rights Reserved
//
// CONTENTS
//
//  ALStatus::operator new()
//  ALStatus::ALStatus()
//  ALStatus::~ALStatus()
//  ALStatus::SetError()
//  ALStatus::GetStatusDetail()
//  ALStatus::operator = ()
//
// DESCRIPTION
//
//  This file contains LibMain() and the WEP() for ArchiveLib DLLs.
//  We don't really do anything exciting in the WEP, it is just
//  here for decoration.  LibMain() has to set up memory allocation
//  for Borland.
//
// REVISION HISTORY
//
//   May 26, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <stdio.h>
#include <stdarg.h>

//
// NAME
//
//  ALStatus::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Memory allocator used when ArchiveLib resides in a 16 bit DLL.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void * ALStatus::operator new( size_t size )
//
// C SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  size  :  The number of bytes that the compiler has decided will be
//           necessary to construct a new ALStatus object.
//
// DESCRIPTION
//
//  When using a DLL, it is easy to get into a dangerous situation when
//  creating objects whose ctor and dtor are both in the DLL.  The problem
//  arises because when you create an object using new, the memory for
//  the object will be allocated from the EXE.  However, when you destroy
//  the object using delete, the memory is freed inside the DLL.  Since
//  the DLL doesn't really own that memory, bad things can happen.
//
//  But, you say, won't the space just go back to the Windows heap regardless
//  of who tries to free it?  Maybe, but maybe not.  If the DLL is using
//  a subsegment allocation scheme, it might do some sort of local free
//  before returning the space to the windows heap.  That is the point where
//  you could conceivably cook your heap.
//
//  By providing our own version of operator new inside this class, we
//  ensure that all memory allocation for the class will be done from
//  inside the DLL, not the EXE calling the DLL.
//
// RETURNS
//
//  A pointer to some memory that should have been pulled out of the
//  heap for the DLL.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 24, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALStatus::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALStatus::ALStatus()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  ALStatus default constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//   ALStatus::ALStatus()
//
// C SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This is the only constructor for objects of class ALStatus.  It
//  initializes the detail length member to 129, which is a const and
//  won't change.  The initial status is AL_SUCCESS, and there is no
//  detail string to start with.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
//  None, this constructor is only used internally.
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 24, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release
//

AL_PROTO
ALStatus::ALStatus()  /* Tag public function */
    : miStatusDetailLength( 129 )
{
    miStatus = AL_SUCCESS;
    mszStatusDetail = 0;
}

//
// NAME
//
//  ALStatus::~ALStatus()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALStatus destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALStatus::~ALStatus()
//
// C SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor has to free up any space allocated for the detailed
//  error status string.  That's all.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
//  None, this destructor is only used internally.
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : First release
//

AL_PROTO
ALStatus::~ALStatus()  /* Tag public function */
{
    if ( mszStatusDetail )
        delete[] mszStatusDetail;
}

//
// NAME
//
//  ALStatus::SetError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Set the ALStatus object to an error state.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStatus::SetError( int error, const char AL_DLL_FAR *fmt, ... );
//
// C SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  error   :  The new error code to set the miStatus member to.  A value
//             less than 0 (AL_SUCCESS) will always be interpreted as
//             an error.
//
//  fmt     :  A sprintf style formatting string.  This is for the
//             message that is going to go into the status detail message.
//
//  ...     :  Any additional arguments needed by the formatting string.
//
//
// DESCRIPTION
//
//  This function is used to set the status of an object to an error state.
//  Normally this is done by sending an error code, along with a detailed
//  message explaining what went wrong and why.  Note that to clear
//  and error state, you can pass AL_SUCCESS for the error code and
//  0 for the format.  The object will look like it is healthy and happy
//  after that.
//
// RETURNS
//
//  The error code that just got passed in.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

int AL_PROTO
ALStatus::SetError( int error, const char AL_DLL_FAR *fmt, ... )  /* Tag public function */
{
    char detail[ 256 ];
    va_list argptr;

    miStatus = error;
    if ( fmt == 0 ) {
        if ( mszStatusDetail )
            delete[] mszStatusDetail;
            mszStatusDetail = 0;
    } else {
        va_start( argptr, fmt );
        vsprintf( detail, fmt, argptr );
        va_end( argptr );
        if ( mszStatusDetail == 0 )
            mszStatusDetail = new char[ miStatusDetailLength ];
        if ( mszStatusDetail ) {
            strncpy( mszStatusDetail, detail, miStatusDetailLength - 1 );
            mszStatusDetail[ miStatusDetailLength - 1 ] = '\0';
        }
    }
    return error;
}

//
// NAME
//
//  ALStatus::GetStatusDetail()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Get a string pointer to the detailed status error message.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  const char * ALStatus::GetStatusDetail() const;
//
// C SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  Whenever we set the error code for an object in ArchiveLib, we
//  call ALStatus::SetError().  At the same time that we set the
//  error code of the object to a non-zero value, we supply a formatted
//  string providing some detail about when and where the error
//  took place, maybe even including some other information provided by the
//  O/S.  That information is stored in the detail string, which is a
//  private data member.  This function provides the ability to get at
//  that detail string.
//
// RETURNS
//
//  Guaranteed to return a valid character string.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

const char AL_DLL_FAR * AL_PROTO
ALStatus::GetStatusDetail() const  /* Tag public function */
{
    if ( mszStatusDetail )
        return mszStatusDetail;
    else if ( miStatus == AL_SUCCESS )
        return "No errors";
    else
        return "Unable to allocate memory for error detail message";
}

//
// NAME
//
//  ALStatus::operator=()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALStatus assignment operator.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALStatus & ALStatus::operator=( ALStatus &rhs );
//
// C SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALStatus is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  rhs  : Another ALStatus object that I want to copy into this object.
//
// DESCRIPTION
//
//  Somewhere in ArchiveLib I want to be able to copy one status
//  into another.  This function does just that.  It has to allocate
//  new space to make a copy of the detail string, and be sure to
//  free up any old space, and all that.
//
// RETURNS
//
//  A reference to this.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

ALStatus AL_DLL_FAR & AL_PROTO
ALStatus::operator=( ALStatus AL_DLL_FAR &rhs )  /* Tag public function */
 {
    if ( rhs.mszStatusDetail == 0 ) {
        if ( mszStatusDetail ) {
            delete[] mszStatusDetail;
            mszStatusDetail = 0;
        }
    } else {
        if ( mszStatusDetail == 0 )
            mszStatusDetail = new char[ miStatusDetailLength ];
        if ( mszStatusDetail )
            strcpy( mszStatusDetail, rhs.mszStatusDetail );
    }
    miStatus = rhs.miStatus;
    return *this;
}

