//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/CXL_MON.CPv   1.0   12 Mar 1998 12:15:28   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/CXL_MON.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:28   DCHI
// Initial revision.
// 
//************************************************************************

//
// CXL_MON.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALMonitorSetObjectSize()
//  ALMonitorSetObjectStart()
//  ALMonitorSetJobSize()
//  ALMonitorSetJobSoFar()
//
// DESCRIPTION
//
//  This file contains all the C translation layer routines for the
//  member access routines for class ALMonitor.
//
//  Functions that simply provide a translation layer for an existing C++
//  function are always located in the same file as the C++ function.  The
//  functions in this file don't have any existing C functions to attach
//  to, since they implement either pure virtual functions or member access
//  routines.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New release
//   March 20, 1996     2.01A : Excluded this from the test drive build
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )       // Don't include these fns in the test drive

//
// NAME
//
//  ALMonitorSetObjectSize()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the object size of a monitor before beginning an operation.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the mlObjectSize member
//  of the monitor class, so no translation function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALMonitorSetObjectSize( hALMonitor this_object, long size );
//
// VB SYNOPSIS
//
//  Declare Function ALMonitorSetObjectSize Lib "AL20LW"
//    (ByVal this_object&, ByVal object_size&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALMonitorSetObjectSize( this_object : hALMonitor;
//                                   object_size : LongInt ) : LongInt;
//
// ARGUMENTS
//
//  this_object   : The monitor whose object size member needs to be set.
//
//  object_size   : The new value to be assigned to mlObjectSize.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMonitor::mlObjectSize.  Why would you want to change this
//  data member?  Normally this data member is set up by the member functions
//  of ALArchive before performing an operation.  If you are trying
//  to use a monitor to provide feedback on an operation of your own,
//  such as a file copy, you would  have to set the data member up
//  using this function.
//
//  This function first tests its handle argument for correct type (when in
//  debug mode), then casts and modifies the data member.
//
//  This function is like all of the other translation routines in that
//  it is fairly uninformative.  To get the real scoop on monitor objects,
//  look at MONITOR.CPP and various archive source files.
//
// RETURNS
//
//  The long size value.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1995  2.0A : New release
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALMonitorSetObjectSize( hALMonitor this_object,  /* Tag public function */
                        long object_size )
{
    AL_ASSERT_OBJECT( this_object, ALMonitor, "ALMonitorSetObjectSize" );
    return ( (ALMonitor *) this_object )->mlObjectSize = object_size;
}

//
// NAME
//
//  ALMonitorSetObjectStart()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the object start value of a monitor before beginning an operation.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the mlObjectStart member
//  of the monitor class, so no translation function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALMonitorSetObjectStart( hALMonitor this_object, long object_start );
//
// VB SYNOPSIS
//
//  Declare Function ALMonitorSetObjectStart Lib "AL20LW"
//    (ByVal this_object&, ByVal object_start&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALMonitorSetObjectStart( this_object : hALMonitor;
//                                    object_start : LongInt ) : LongInt;
//
// ARGUMENTS
//
//  this_object   : The monitor whose object start member needs to be set.
//
//  object_start  : The new value to be assigned to mlObjectStart.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMonitor::mlObjectStart.  Why would you want to change this
//  data member?  Normally this data member is set up by the member functions
//  of ALArchiveBase before performing an operation.  If you are trying
//  to use a monitor to provide feedback on an operation of your own,
//  such as a file copy, you would  have to set the data member up
//  using this function.
//
//  This function first tests its handle argument for correct type (when in
//  debug mode), then casts and modifies the data member.
//
//  This function is like all of the other translation routines in that
//  it is fairly uninformative.  To get the real scoop on monitor objects,
//  look at MONITOR.CPP and ARCHIVEB.CPP.
//
// RETURNS
//
//  The long object_start value just assigned.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1995  2.0A : New release
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALMonitorSetObjectStart( hALMonitor this_object, /* Tag public function */
                         long object_start )
{
    AL_ASSERT_OBJECT( this_object, ALMonitor, "ALMonitorSetObjectStart" );
    return ( (ALMonitor *) this_object )->mlObjectStart = object_start;
}

//
// NAME
//
//  ALMonitorSetJobSize()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the job size value of a monitor before beginning an operation.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the mlJobSize member
//  of the monitor class, so no translation function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALMonitorSetJobSize( hALMonitor this_job, long job_size );
//
// VB SYNOPSIS
//
//  Declare Function ALMonitorSetJobSize Lib "AL20LW"
//    (ByVal this_object&, ByVal job_size&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALMonitorSetJobSize( this_object : hALMonitor;
//                                job_size : LongInt ) : LongInt;
//
// ARGUMENTS
//
//  this_object   : The monitor whose job size member needs to be set.
//
//  job_size      : The new value to be assigned to mlJobSize.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMonitor::mlJobSize.  Why would you want to change this
//  data member?  Normally this data member is set up by the member functions
//  of ALArchive before performing an operation.  If you are trying
//  to use a monitor to provide feedback on an operation of your own,
//  such as a file copy, you would  have to set the data member up
//  using this function.
//
//  This function first tests its handle argument for correct type (when in
//  debug mode), then casts and modifies the data member.
//
//  This function is like all of the other translation routines in that
//  it is fairly uninformative.  To get the real scoop on monitor objects,
//  look at MONITOR.CPP and various archive source files.
//
// RETURNS
//
//  The long job_size value.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//

extern "C" AL_LINKAGE long AL_FUNCTION
ALMonitorSetJobSize( hALMonitor this_object, /* Tag public function */
                     long job_size )
{
    AL_ASSERT_OBJECT( this_object, ALMonitor, "ALMonitorSetJobSize" );
    return ( (ALMonitor *) this_object )->mlJobSize = job_size;
}

//
// NAME
//
//  ALMonitorSetJobSoFar()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the "job so far" value of a monitor before beginning an operation.
//
// C++ SYNOPSIS
//
//  None, C++ programmers have public access to the mlJobSoFar member
//  of the monitor class, so no translation function is needed.
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALMonitorSetJobSoFar( hALMonitor this_object, long job_so_far );
//
// VB SYNOPSIS
//
//  Declare Function ALMonitorSetJobSoFar Lib "AL20LW"
//    (ByVal this_object&, ByVal job_so_far&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALMonitorSetJobSoFar( this_object : hALMonitor;
//                                 job_so_far : LongInt ) : LongInt;
//
// ARGUMENTS
//
//  this_object   : The monitor whose object start member needs to be set.
//
//  job_so_far    : The new value to be assigned to mlJobSoFar.
//
// DESCRIPTION
//
//  This C/VB translation function provides access to the C++ data member
//  ALMonitor::mlJobSoFar.  Why would you want to change this
//  data member?  Normally this data member is set up by the member functions
//  of ALArchive before performing an operation.  If you are trying
//  to use a monitor to provide feedback on an operation of your own,
//  such as a file copy, you would  have to set the data member up
//  using this function.
//
//  This function first tests its handle argument for correct type (when in
//  debug mode), then casts and modifies the data member.
//
//  This function is like all of the other translation routines in that
//  it is fairly uninformative.  To get the real scoop on monitor objects,
//  look at MONITOR.CPP and the archive routines.
//
// RETURNS
//
//  The long job_so_far value just assigned.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//


extern "C" AL_LINKAGE long AL_FUNCTION
ALMonitorSetJobSoFar( hALMonitor this_object, /* Tag public function */
                      long job_so_far )
{
    AL_ASSERT_OBJECT( this_object, ALMonitor, "ALMonitorSetJobSoFar" );
    return ( (ALMonitor *) this_object )->mlJobSoFar = job_so_far;
}

#endif // #if !defined( AL_DEMO )

