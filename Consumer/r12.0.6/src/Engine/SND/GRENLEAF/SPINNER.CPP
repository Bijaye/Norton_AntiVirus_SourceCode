//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/SPINNER.CPv   1.0   12 Mar 1998 12:18:04   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/SPINNER.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:18:04   DCHI
// Initial revision.
// 
//************************************************************************

//
// SPINNER.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALSpinner::operator new()
//  ALSpinner::ALSpinner()
//  newALSpinner()
//  ALSpinner::~ALSpinner()
//  ALSpinner::Progress()
//  ALSpinner::ArchiveOperation()
//
// DESCRIPTION
//
//  This file contains all of the source code to support the ALSpinner
//  class.  ALSpinner displays a little propellor that spins around
//  in order to indicate that something is happening.  So the Progress()
//  guy here doesn't pay any attention to the numbers that he gets, he
//  just spins the propellor a little bit each time it gets called.
//
//  The ArchiveOperation() function works pretty much like the other
//  monitors.  It just formats the messages and prints them out.
//
// REVISION HISTORY
//
//  May 22, 1994  1.0A    : First release
//
//  January 9, 1995 1.01A : Made modifications to arguments to member
//                          functions, so this could be used from inside
//                          a DLL.
//
//   February 14, 1996  2.0A : New release
//
//   March 20, 1996     2.01A : Excluded all the code in this file from
//                              the test drive release.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )  //Exclude all this code from the test drive

#include <iostream>
using namespace std;

#include "spinner.h"

//
// A single static member for the whole class.  At one time I thought
// this would be a good idea, I don't think so any more.  But it doesn't
// really matter unless you have multiple processes running multiple
// monitors.  Since this is MS-DOS only, that doesn't seem likely.
//

int ALSpinner::miSpinIndex;

//
// NAME
//
//  ALSpinner::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++
//
// SHORT DESCRIPTION
//
//  Memory allocator used when ArchiveLib resides in a 16 bit DLL.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "spinner.h"
//
//  void * ALSpinner::operator new( size_t size )
//
// C SYNOPSIS
//
//  None.
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
//           necessary to construct a new ALSpinner object.
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
//   February 14, 1996  2.0A : New release
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALSpinner::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALSpinner::ALSpinner()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++  C
//
// SHORT DESCRIPTION
//
//  Create a new spinner monitor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "spinner.h"
//
//  ALSpinner::ALSpinner( ALMonitorType monitor_type,
//                        ostream &stream );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "spinner.h"
//
//  hALMonitor newALSpinner( enum ALMonitorType monitor_type );
//
// VB SYNOPSIS
//
//  None, no text mode output for VB.
//
// DELPHI SYNOPSIS
//
//  None, no text mode output for Delphi.
//
// ARGUMENTS
//
//  monitor_type  : AL_MONITOR_OBJECTS or AL_MONITOR_JOBS.
//
//  stream        : The stream where the spinner is going to be
//                  printed.  Note that if you are using C, you don't
//                  get a choice here.  That's because C <stdio.h> isn't
//                  aware of streams.  We give you cout by default.  Hope
//                  it all works.
//
// DESCRIPTION
//
//  The constructor for the ALSpinner object just has to initialize
//  a couple of data members.  The real action doesn't start until the
//  archive is opened.
//
//  In theory, you might be able to use this from outside a DLL, for example,
//  with PowerPack.  However, there could be complications when trying to
//  talk to a DLL with a far reference.  Who knows???
//
// RETURNS
//
//  The C++ constructor has no return type.  The C function returns an
//  ALMonitor handle which points to the newly constructed monitor object.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

AL_PROTO
ALSpinner::ALSpinner( ALMonitorType monitor_type,  /* Tag public function */
                      ostream AL_DLL_FAR & stream )
    : ALMonitor( monitor_type ),
      mrStream( stream )
{
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALMonitor AL_FUNCTION
newALSpinner( enum ALMonitorType monitor_type )  /* Tag public function */
{
    ALSpinner *monitor;
    monitor = new ALSpinner( monitor_type, cout );
    return (hALMonitor) monitor;
}

#endif

//
// NAME
//
//  ALSpinner::~ALSpinner()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++
//
// SHORT DESCRIPTION
//
//  Destroy an ALSpinner monitor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "spinner.h"
//
//  ALSpinner::~ALSpinner();
//
// C SYNOPSIS
//
//  C programs must call the base class destructor deleteALMonitor().
//
// VB SYNOPSIS
//
//  VB programs must call the base class destructor deleteALMonitor().
//
// DELPHI SYNOPSIS
//
//  Delphi programs must call the base class destructor deleteALMonitor().
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  The destructor does nothing.  If it ever has to do anything serious,
//  this is where it will happen.
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

AL_PROTO
ALSpinner::~ALSpinner()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALSpinner: attempt to delete invalid object" );
}

//
// NAME
//
//  ALSpinner::Progress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++
//
// SHORT DESCRIPTION
//
//  The progress routine for the spinner monitor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "spinner.h"
//
//  void ALSpinner::Progress( long object_tell,
//                            ALStorage &object );
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
// DESCRIPTION
//
//  This function gets called a lot while an object is being processed.
//  All we do in here is spin the spinner by one position.
//
//  You might notice that this monitor doesn't really care at all about
//  any of the numeric values.  All it does is spin the spinner, regardless
//  of what's happening.
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

void AL_PROTO
ALSpinner::Progress( long object_tell,  /* Tag public function */
                     ALStorage AL_DLL_FAR & object )
{
    ALMonitor::Progress( object_tell, object );
    mrStream << "|/-\\"[ miSpinIndex++ & 3 ] << '\b' << flush;
}

//
// NAME
//
//  ALSpinner::ArchiveOperation()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++
//
// SHORT DESCRIPTION
//
//  Update user interface elements after an archiving operation.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "spinner.h"
//
//  void ALSpinner::ArchiveOperation( ALArchiveOperation operation,
//                                    ALArchive *archive,
//                                    ALEntry *job );
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
//  It would make a lot of sense to move the message formatting up into
//  the base class so I didn't have to duplicate this effort in every
//  derived class.
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

void AL_PROTO
ALSpinner::ArchiveOperation( ALArchiveOperation operation,  /* Tag public function */
                             ALArchive AL_DLL_FAR *archive,
                             ALEntry AL_DLL_FAR *job )
{
    switch ( operation ) {
        case AL_ARCHIVE_OPEN :
            if ( miMonitorType == AL_MONITOR_JOB )
                mrStream << archive->GetStorageObject()->mName.GetName() << " ";
            break;
        case AL_ARCHIVE_CLOSE :
            if ( miMonitorType == AL_MONITOR_JOB )
                mrStream << "\n";
            break;
        case AL_EXTRACTION_OPEN :
        case AL_COPY_OPEN :
        case AL_INSERTION_OPEN :
            if ( miMonitorType == AL_MONITOR_OBJECTS )
                mrStream << job->mpStorageObject->mName.GetSafeName() << " ";
            break;
        case AL_EXTRACTION_CLOSE :
        case AL_COPY_CLOSE :
            if ( miMonitorType == AL_MONITOR_OBJECTS )
                mrStream << "\n";
            break;
        case AL_INSERTION_CLOSE :
            if ( miMonitorType == AL_MONITOR_OBJECTS )
                mrStream << job->CompressionRatio() << "%\n";
            break;
        case AL_END_DIRECTORY_WRITE :
        case AL_END_DIRECTORY_READ :
//            cout << "Done\n";
            break;
        case AL_START_DIRECTORY_WRITE :
//            mrStream << "Writing directory\n";
            break;
        case AL_START_DIRECTORY_READ :
//            mrStream << "Reading directory\n";
            break;
    }
    mrStream << flush;
}

#endif //#if !defined( AL_DEMO )

