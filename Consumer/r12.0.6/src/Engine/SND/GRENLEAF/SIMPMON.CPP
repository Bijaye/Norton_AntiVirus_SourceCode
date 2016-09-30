//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/SIMPMON.CPv   1.0   12 Mar 1998 12:17:56   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/SIMPMON.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:56   DCHI
// Initial revision.
// 
//************************************************************************

//
// SIMPMON.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALSimpleMonitor::operator new()
//  ALSimpleMonitor::ALSimpleMonitor()
//  ALSimpleMonitor::ALSimpleMonitor()
//  newALSimpleMonitor()
//  ALSimpleMonitor::~ALSimpleMonitor()
//  ALSimpleMonitor::ArchiveOperation()
//
// DESCRIPTION
//
//  This file contains all the source code to support the simple
//  monitor class, ALSimpleMonitor.  This class is hidden from the
//  users of the simple interface.  Their view of the monitor is
//  via a callback function (C and C++), or a set of three windows
//  that receive text messages (VB and Delphi).
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//   March 20, 1996     2.01A : Added the include to simpmon.h.  I need
//                              that because I modified alsimple.h a
//                              little bit for the test drive version.
//
#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif
#include "alsimple.h"
#include "simpmon.h"

//
// NAME
//
//  ALSimpleMonitor::operator new()
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
//  #include "alsimple.h"
//
//  void * ALSimpleMonitor::operator new( size_t size )
//
// C SYNOPSIS
//
//  None, ALSimpleMonitor is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALSimpleMonitor is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALSimpleMonitor is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  size  :  The number of bytes that the compiler has decided will be
//           necessary to construct a new ALSimpleMonitor object.
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
//   February 14, 1996  2.0A : New Release
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALSimpleMonitor::operator new( size_t size )  /* Tag private function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALSimpleMonitor::ALSimpleMonitor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  One of two constructors for ALSimpleMonitor().
//
// C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  ALSimpleMonitor::ALSimpleMonitor( CALLBACK_FN fn )
//
// C SYNOPSIS
//
//  None, the simple monitor is only used internally by our library.
//
// VB SYNOPSIS
//
//  None, the simple monitor is only used internally.
//
// DELPHI SYNOPSIS
//
//  None, the simple monitor is only used internally.
//
// ARGUMENTS
//
//  fn    :  The callback function provided by the user.  This is
//           passed to functions like ALExtract as a parameter,
//           and passed here by the library routine.
//
// DESCRIPTION
//
//  C and C++ programmers use the ALSimpleMonitor object via a callback
//  function.  As the simple interface function progresses, the callback
//  function is called, which lets the programmer update the user
//  interface.
//
//  Note that under Windows, VB and Delphi use window handles instead
//  of a callback function.  Since this constructor is designed to
//  use a callback function, I zero out the window handles, just in case.
//
// RETURNS
//
//  Nothing, this is a constructor.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO ALSimpleMonitor::ALSimpleMonitor( CALLBACK_FN fn ) /* Tag private function */
  : ALMonitor( AL_MONITOR_JOB )
{
    mpCallbackFunction = fn;
#if defined( AL_VB ) || defined( AL_VB32 )
    mhTextWindow = 0;
    mhFileProgressWindow = 0;
    mhJobProgressWindow = 0;
#endif
}

//
// NAME
//
//  ALSimpleMonitor::ALSimpleMonitor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  VB  Delphi
//
// SHORT DESCRIPTION
//
//  One of two constructors for ALSimpleMonitor().
//
// C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  ALSimpleMonitor::ALSimpleMonitor( HWND text_window,
//                                    HWND file_progress_window,
//                                    HWND job_progress_window )
//
// C SYNOPSIS
//
//  hALMonitor newALSimpleMonitor( HWND text_window,
//                                 HWND file_progress_window,
//                                 HWND job_progress_window )
//
// VB SYNOPSIS
//
//  Declare Function newALSimpleMonitor Lib "AL20LW"
//  ( ByVal text_window%,
//    ByVal file_progress_window%,
//    ByVal job_progress_window%) As Long
//
// DELPHI SYNOPSIS
//
//  function newALSimpleMonitor( text_window : Hwnd;
//                               file_progress_window : Hwnd;
//                               job_progress_window : Hwnd ) : hALMonitor;
//
// ARGUMENTS
//
//  text_window          : The handle for the window that will receive
//                         file names as they are processed.
//
//  file_progress_window : The handle for the window that will receive
//                         updates on the percentage of each file that
//                         has been processed.
//
//  job_progress_window  : The handle for the window that will receive
//                         updates on the percentage of the entire job that
//                         has been processed.
//
// DESCRIPTION
//
//  VB and Delphi  programmers use the ALSimpleMonitor object via a set
//  of three window handles.  As the simple interface function progresses
//  through a function, these windows are sent file name an progress amounts
//  via SetWindowText() calls.
//
//  The internal code used to implement the simple interface functions
//  under VB or Delphi create monitors using this version of the ctor.
//
// RETURNS
//
//  Nothing for C++, o/w a handle for a new monitor.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

#if defined( AL_VB ) || defined( AL_VB32 )

AL_PROTO ALSimpleMonitor::ALSimpleMonitor( HWND text_window,  /* Tag private function */
                                           HWND file_progress_window,
                                           HWND job_progress_window )
  : ALMonitor( AL_MONITOR_JOB )
{
    mpCallbackFunction = 0;
    mhTextWindow = text_window;
    mhFileProgressWindow = file_progress_window;
    mhJobProgressWindow = job_progress_window;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALMonitor AL_FUNCTION
newALSimpleMonitor( /* Tag protected function */
                     HWND text_window,
                     HWND file_progress_window,
                     HWND job_progress_window )
{
    ALSimpleMonitor *monitor;
    monitor = new ALSimpleMonitor( text_window, file_progress_window, job_progress_window );
    return (hALMonitor) monitor;
}

#endif
#endif

//
// NAME
//
//  ALSimpleMonitor::~ALSimpleMonitor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The destructor for ALSimpleMonitor.
//
// C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  ALSimpleMonitor::~ALSimpleMonitor()
//
// C SYNOPSIS
//
//  C programmers should call deleteALMonitor()
//
// VB SYNOPSIS
//
//  VB programmers should call deleteALMonitor()
//
// DELPHI SYNOPSIS
//
//  Delphi programmers should call deleteALMonitor()
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor for ALSimpleMonitor doesn't have to do anything.
//
// RETURNS
//
//  Nothing, destructor doesn't do anything.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO ALSimpleMonitor::~ALSimpleMonitor() /* Tag private function */
{
}

//
// NAME
//
//  ALSimpleMonitor::ArchiveOperation()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Update user interface elements after an archiving operation.
//
// C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALSimpleMonitor::ArchiveOperation( ALArchiveOperation operation,
//                                          ALArchive *archive,
//                                          ALEntry *job );
//
// C SYNOPSIS
//
//  None, this is a protected member function for internal use.
//
// VB SYNOPSIS
//
//  None, this is a protected member function for internal use.
//
// DELPHI SYNOPSIS
//
//  None, this is a protected member function for internal use.
//
// ARGUMENTS
//
//  operation : One of the values from the enumerated type ALArchiveOperation.
//              It is simply a list of possible operations that the archive
//              operation might take, such as opening a file, closing a
//              file, etc.
//
//  archive   : A pointer to the archive object currently being worked on.
//
//  job       : A pointer to an ALEntry object that defines the ALStorage
//              object presently being worked on.
//
// DESCRIPTION
//
//  During the course of an Archiving operation, the functions in
//  ALArchive will get the urge to spit out a message.  They do
//  so by calling this member function.  All of the messages should
//  be self-explanatory.
//
//  The only message the simple monitor is interested in are messages
//  generated when a file is opened.  We ignore everything else.
//
//  Note that there are two different ways this monitor can work.  If a
//  callback function is defined, we call it with the file name, and set
//  the two numeric values to -1, which is an invalid value.  If a window
//  handle is defined, on the other hand, we just use SetWindowText() to
//  stuff the file name into the window.
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
//   February 16, 1996  2.0A : First release
//

void AL_PROTO
ALSimpleMonitor::ArchiveOperation( ALArchiveOperation operation, /* Tag private function */
                              ALArchive AL_DLL_FAR *,
                              ALEntry AL_DLL_FAR *job )
{
    switch ( operation ) {
        case AL_EXTRACTION_OPEN :
        case AL_COPY_OPEN :
        case AL_INSERTION_OPEN :
            if ( mpCallbackFunction )
                mpCallbackFunction( job->mpStorageObject->mName.GetSafeName(), -1, -1 );
#if defined( AL_VB ) || defined( AL_VB32 )
            if ( mhTextWindow )
                SetWindowText( mhTextWindow, job->mpStorageObject->mName.GetSafeName() );
#endif
              break;
    }
}

//
// NAME
//
//  ALSimpleMonitor::Progress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The progress routine for the simple monitor.
//
// C++ SYNOPSIS
//
//  #include "alsimple.h"
//
//  void ALSimpleMonitor::Progress( long object_tell,
//                                  ALStorage &object );
//
// C SYNOPSIS
//
//  None, this is a protected member function for internal use.
//
// VB SYNOPSIS
//
//  None, this is a protected member function for internal use.
//
// DELPHI SYNOPSIS
//
//  None, this is a protected member function for internal use.
//
// ARGUMENTS
//
//  object_tell  : The current offset within the object being processed.
//                 Since this function is always called from inside
//                 ALStorage::Yield(), the location will always be known.
//
// object        : The object being compressed, extracted, copied, or
//                 whatever.
//
//
// DESCRIPTION
//
//  While an archiving operation is in progress, this monitor function
//  will get called sometimes.  It gets called by the Yield() function
//  inside the ALStorage object.  The Yield function figures out what
//  the current offset is inside the storage object, and passes that
//  as a parameter.
//
//  This routine does a little bit of work to calculate the job
//  and file progress ratios.  Then it sends them to the calling process
//  in one of two ways.  If a callback function has been defined, it
//  passes them as integer arguments to the function (with the file name
//  pointer set to 0.)  If the two window handles have been defined, it
//  formats the numbers into ASCII and sends them via SetWindowText().
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
//   February 15, 1996  2.0A : First release
//

void AL_PROTO ALSimpleMonitor::Progress( long object_tell, /* Tag private function */
                                         ALStorage AL_DLL_FAR & object )
{
//
// mlByteCount is no good, because we are in job mode
//
    int file_ratio;
    ALMonitor::Progress( object_tell, object );
    long byte_count = object_tell - mlObjectStart;
    if ( mlObjectSize == 0 )
         file_ratio = -1;
    else {
        if ( mlObjectSize < 1000000L )
             file_ratio = (int)(100 * byte_count / mlObjectSize );
    else
        file_ratio = (int)( byte_count / ( mlObjectSize / 100L ) );
    }
    if ( mpCallbackFunction )
        mpCallbackFunction( 0, file_ratio, miRatio );
#if defined( AL_VB ) || defined( AL_VB32 )
    char buf[ 24 ];
    if ( file_ratio != -1 && mhFileProgressWindow ) {
        wsprintf( buf, "%d", file_ratio );
        SetWindowText( mhFileProgressWindow, buf );
    }
    if ( miRatio != -1 && mhJobProgressWindow ) {
        wsprintf( buf, "%d", miRatio );
        SetWindowText( mhJobProgressWindow, buf );
    }
#endif
}


