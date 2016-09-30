//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/MONITOR.CPv   1.0   12 Mar 1998 12:17:00   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/MONITOR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:00   DCHI
// Initial revision.
// 
//************************************************************************

//
// MONITOR.CPP
//
//  Source file for ArchiveLib 1.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALMonitor::operator new()
//  ALMonitor::ALMonitor()
//  ALMonitor::~ALMonitor()
//  deleteALMonitor();
//  ALMonitor::Progress()
//  ALMonitor::ArchiveOperation()
//
// DESCRIPTION
//
//  This file contains all the functions defined for ALMonitor.  This is
//  a base class, and you generally won't use an implementation of it.
//  It doesn't have any pure functions, so if you want a do-nothing
//  monitor, use this guy.  Other than that, the Progress() function actually
//  does a useful calculation for derived classes, so they might call it.
//
// REVISION HISTORY
//
//  May 26, 1994  1.0A    : First release
//
//  January 9, 1995 1.01A : Minor change in constructor
//
//   February 14, 1996  2.0A : New Release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALMonitor::operator new()
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
//  void * ALMonitor::operator new( size_t size )
//
// C SYNOPSIS
//
//  None, this is an internal C++ function.
//
// VB SYNOPSIS
//
//  None.
//
// DELPHI SYNOPSIS
//
//  None.
//
// ARGUMENTS
//
//  size  :  The number of bytes that the compiler has decided will be
//           necessary to construct a new ALMonitor object.
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
ALMonitor::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALMonitor::ALMonitor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The base monitor class constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALMonitor::ALMonitor( ALMonitorType monitor_type );
//
// C SYNOPSIS
//
//  None, C programs must use derived class constructors.
//
// VB SYNOPSIS
//
//  None, VB programs must use derived class constructors.
//
// DELPHI SYNOPSIS
//
//  None, Delphi programs must use derived class constructors.
//
// ARGUMENTS
//
//  monitor_type : One of the enumerated types from ALDEFS.H.  The only
//                 two types supported are AL_MONITOR_OBJECTS and
//                 AL_MONITOR_JOB.
//
// DESCRIPTION
//
//  This function is called when one of the derived classes is creating
//  a new monitor. (It could be called directly, but you aren't likely
//  to instantiate an ALMonitor.)  It has only one thing to do, which
//  is to initialize the miMonitorType data member.  This data member
//  is a const member, so it has to be initialized in an initializer list.
//  It's nice to make it const, because then you can leave it public and
//  nobody gets to jack with it.
//
// RETURNS
//
//  When called dynamically, for example from operator new, this function
//  returns a pointer to a newly created monitor.  Otherwise, it returns
//  nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALMonitor::ALMonitor( ALMonitorType monitor_type )  /* Tag public function */
    : miMonitorType( monitor_type )
{
    mlObjectStart = 0;
    mlObjectSize = -1;
}

//
// NAME
//
//  ALMonitor::~ALMonitor();
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The destructor for the base monitor class.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALMonitor::~ALMonitor();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void deleteALMonitor( hALMonitor this_object )
//
// VB SYNOPSIS
//
//  Declare Sub deleteALMonitor Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALMonitor( this_object : hALMonitor );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALMonitor object that
//                  is going to be deleted.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  The ALMonitor destructor doesn't have to clean up any dynamic
//  storage or anything like that.  As a consequence, all we do is
//  check the validity of this in debug mode.
//
//  Note that C/VB/Delphi programs will always call this function to
//  destroy ALMonitors of any derived class.  Since the dtor is a virtual
//  function, the call will get routed to the correct function via the
//  virtual function mechanism.
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
//   February 14, 1996  2.0A : New Release
//

AL_PROTO ALMonitor::~ALMonitor()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALMonitor: attempt to delete invalid object" );
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALMonitor( hALMonitor this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALMonitor, "deleteALMonitor" );
    delete (ALMonitor *) this_object;
}

#endif

//
// NAME
//
//  ALMonitor::Progress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The virtual function called during compression routines.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALMonitor::Progress( long object_tell,
//                            ALStorage &object );
//
// C SYNOPSIS
//
//  None, this is a virtual callback routine.
//
// VB SYNOPSIS
//
//  None, this is a vritual callback routine.
//
// DELPHI SYNOPSIS
//
//  None, this is a virtual callback routine.
//
// ARGUMENTS
//
//  object_tell  : The current offset withing the object being compressed,
//                 expanded, copied, or processed.
//
//  object       : A reference to the storage object being processed.
//
//
// DESCRIPTION
//
//  This is a virtual function.  ALMonitor::Progress() gets called from
//  YieldTime() inside a storage object, which happens pretty
//  frequently.  Normally the derived class will have its own version
//  of Progress(), so this guy won't get called directly.
//
//  However, most of the derived versions of Progress() will go ahead and
//  call this version anyway.  Why?  Because this guy calculates the values
//  of miRatio and mlByteCount for you.
//
//  The calculated values of miRatio and mlByteCount will differ depending
//  on whether the monitor is of type AL_MONITOR_JOB or AL_MONITOR_OBJECTS.
//
//  In AL_MONITOR_OBJECTS mode, the byte count is going be calculated by
//  taking the current offset of the object and subtracting the starting
//  position of the object.  We have to subtract out the starting position,
//  because sometimes we are going to be monitoring an object that resides
//  in an archive, and its starting position will not be at location 0.
//
//  If we are in AL_MONITOR_JOB mode, the byte count is going to be the
//  same as referred to above, plus the value of mlJobSoFar.  That data
//  member contains the total number of bytes processed in previous objects
//  in this job.  That figure is updated after each object is processed,
//  but not by this class. ALArchiveBase does this for ordinary archiving
//  operations, you can look at that code for hints on how to do this
//  yourself.
//
//  Calculating the ratio is pretty easy.  If you are in AL_MONITOR_OBJECTS
//  mode, you just divide the byte count by the object size.  If you are
//  in AL_MONITOR_JOB mode, you divide the byte count by the job size.  Once
//  again, the job size will have been calculated in advance by whatever
//  process is performing the compression/expansion operation.
//
//  Note that there is one tricky bit here.  If the object size was set to
//  -1 by the calling program, it means this routine has to go out and
//  get the size.  This convenience cuts down on code in the high level
//  routine.
//
// RETURNS
//
//  None.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

void AL_PROTO
ALMonitor::Progress( long object_tell,  /* Tag public function */
                     ALStorage AL_DLL_FAR & object )
{
    mlByteCount = object_tell - mlObjectStart;
    if ( mlObjectSize == -1 )
        mlObjectSize = object.GetSize();
    if ( miMonitorType == AL_MONITOR_JOB ) {
        mlByteCount += mlJobSoFar;
        if ( mlJobSize == 0 )
            miRatio = -1;
        else {
            if ( mlJobSize < 1000000L )
                miRatio = (int)( 100 * mlByteCount / mlJobSize );
            else
                miRatio = (int)( mlByteCount / ( mlJobSize / 100L ) );
        }
    } else {
        if ( mlObjectSize == 0 )
            miRatio = -1;
        else {
            if ( mlObjectSize < 1000000L )
                miRatio = (int)(100 * mlByteCount / mlObjectSize );
            else
                miRatio = (int)( mlByteCount / ( mlObjectSize / 100L ) );
        }
    }
}

//
// NAME
//
//  ALMonitor::ArchiveOperation()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The progress routine called during key archive operations.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALMonitor::ArchiveOperation( ALArchiveOperation operation,
//                                    ALArchive *archive,
//                                    ALEntry *job );
//
// C SYNOPSIS
//
//  None, this is a virtual callback support routine.
//
// VB SYNOPSIS
//
//  None, this is a virtual callback support routine.
//
// DELPHI SYNOPSIS
//
//  None, this is a virtual callback support routine.
//
// ARGUMENTS
//
//  operation   : One of the enumerated values of ALArchiveOperation,
//                used to indicate just what is happening here.
//
//  archive     : A pointer to the archive object.  You can use this
//                in monitor routines to get the name of the archive
//                or other exciting facts.
//
//  job         : A pointer to the ALEntry object.  This lets me know
//                which object is being compressed, expanded, or whatever.
//
// DESCRIPTION
//
//  Derived classes override this function to print informative information
//  about various archiving operations.  The base class does absolutely
//  nothing with this information, it is a do-nothing function.
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
//   February 14, 1996  2.0A : New Release
//

void AL_PROTO
ALMonitor::ArchiveOperation( ALArchiveOperation,  /* Tag public function */
                             ALArchive AL_DLL_FAR *,
                             ALEntry AL_DLL_FAR * )
{
}



