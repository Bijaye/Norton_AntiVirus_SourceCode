//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/BARGRAPH.CPv   1.0   12 Mar 1998 12:15:08   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/BARGRAPH.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:15:08   DCHI
// Initial revision.
// 
//************************************************************************

//
// BARGRAPH.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALBarGraph::operator new()
//  ALBarGraph::ALBarGraph()
//  newALBarGraph()
//  ALBarGraph::~ALBarGraph()
//  ALBarGraph::Progress()
//  ALBarGraph::ArchiveOperation()
//
// DESCRIPTION
//
//  This file contains all of the member functions from class ALBarGraph.
//  ALBarGraph is a monitor class that is only used under MS-DOS
//  console operations.  This guy prints out a little graph, then
//  fills it in as the progress proceeds from 0 to 100%.
//
// REVISION HISTORY
//
//  May 21, 1994  1.0A  : First release
//
//  July 8, 1994  1.0B  : Changed the default characters under UNIX to
//                        normal ASCII, not IBMPC non-printables.  They
//                        are defined by the macros FOREGROUND and
//                        BACKGROUND.
//
//  January 9, 1995  1.01A : Made modifications to argument and parameter
//                           definitions so that the bargraph could be used
//                           as part of a DOS DLL.
//
//   February 14, 1996  2.0A : New release
//
//   March 20, 1996     2.01A : Excluded all the code in this file from
//                              the test drive release.

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#if !defined( AL_DEMO )

#include <iostream>
using namespace std;

#include <iomanip>
using namespace std;


#include "bargraph.h"

#define FOREGROUND '\xb1'
#define BACKGROUND '\xb0'

//
// NAME
//
//  ALBarGraph::operator new()
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
//  #include "bargraph.h"
//
//  void * ALBarGraph::operator new( size_t size )
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
//           necessary to construct a new ALBarGraph object.
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
ALBarGraph::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALBarGraph::ALBarGraph()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++  C
//
// SHORT DESCRIPTION
//
//  Create a new bar graph monitor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "bargraph.h"
//
//  ALBarGraph::ALBarGraph( ALMonitorType monitor_type,
//                          ostream AL_DLL_FAR& stream,
//                          int bar_length = 25 );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "bargraph.h"
//
//  hALMonitor newALBarGraph( enum ALMonitorType monitor_type );
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
//  stream        : The stream where the little graph is going to be
//                  printed.  Note that if you are using C, you don't
//                  get a choice here.  That's because C <stdio.h> isn't
//                  aware of streams.  We give you cout by default.  Hope
//                  it all works.
//
//  bar_length    : The length of the bar.  C programmers get no choice here,
//                  have to use the default value of 25.
//
//
// DESCRIPTION
//
//  This constructor initializes things, but it doesn't have any real
//  work to do.  It initializes the three local data members for
//  the object, and initializes ALMonitor with its data member, and
//  that's that.  We don't actually draw the graph yet, that happens
//  later when one of the Archive operations takes place.
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
ALBarGraph::ALBarGraph( ALMonitorType monitor_type,  /* Tag public function */
                        ostream AL_DLL_FAR& stream,
                        int bar_length /* =25 */ )
     : ALMonitor( monitor_type ),
        mrStream( stream ),
        miBarLength( bar_length )
{
     miCurrentOffset = 0;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALMonitor AL_FUNCTION
newALBarGraph( enum ALMonitorType monitor_type )  /* Tag public function */
{
    ALBarGraph *monitor;
    monitor = new ALBarGraph( monitor_type, cout );
    return (hALMonitor) monitor;
}

#endif

//
// NAME
//
//  ALBarGraph::~ALBarGraph()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++
//
// SHORT DESCRIPTION
//
//  Destroy an ALBarGraph monitor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "bargraph.h"
//
//  ALBarGraph::~ALBarGraph();
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
ALBarGraph::~ALBarGraph()  /* Tag public function */
{
     AL_ASSERT( GoodTag(), "~ALBarGraph: attempt to delete invalid object" );
}

//
// NAME
//
//  ALBarGraph::Progress()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console
//  C++
//
// SHORT DESCRIPTION
//
//  The progress routine for the BarGraph monitor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "bargraph.h"
//
//  void ALBarGraph::Progress( long object_tell,
//                              ALStorage &object );
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
//  All we do in here is update the position on the bargraph.  Note
//  that we can't go backwards!
//
//  You might notice here that we don't check on whether we are monitoring
//  Objects or Jobs, we just go with the value found in miRatio.  There
//  is a simple reason for that.  At the start of the function, we call
//  ALMonitor::Progress().  The base class function checks to see which
//  mode we are in, and calculates the miRatio value according to whatever
//  mode we are in.
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
ALBarGraph::Progress( long object_tell,  /* Tag public function */
                      ALStorage AL_DLL_FAR & object )
{
     ALMonitor::Progress( object_tell, object );
     if ( miRatio >= 0 ) {
          int new_offset = ( miBarLength * miRatio ) / 100;
          while ( miCurrentOffset < new_offset ) {
                mrStream << FOREGROUND;
                miCurrentOffset++;
          }
     }
     mrStream << flush;
}

//
// NAME
//
//  ALBarGraph::ArchiveOperation()
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
//  #include "bargraph.h"
//
//  void ALBarGraph::ArchiveOperation( ALArchiveOperation operation,
//                                     ALArchive *archive,
//                                     ALEntry *job );
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
ALBarGraph::ArchiveOperation( ALArchiveOperation operation,  /* Tag public function */
                              ALArchive AL_DLL_FAR *archive,
                              ALEntry AL_DLL_FAR *job )
{
    int i;
//
// Note that I am using character 0xb0 to draw the graph originally,
// and 0xb1 to redraw it indicating progress.  Maybe it would be
// better to make these data members so people could change their
// values...
//
    switch ( operation ) {
        case AL_ARCHIVE_OPEN :
            if ( miMonitorType == AL_MONITOR_JOB ) {
                mrStream << archive->GetStorageObject()->mName.GetSafeName()
                         << " ";
                for ( i = 0 ; i < miBarLength ; i++ )
                    mrStream << BACKGROUND;
                for ( i = 0 ; i < miBarLength ; i++ )
                    mrStream << '\b';
            }
                break;

        case AL_ARCHIVE_CLOSE :
            if ( miMonitorType == AL_MONITOR_JOB ) {
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << '\b';
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << ' ';
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << '\b';
                mrStream << "\n";
                miCurrentOffset = 0;
            }
                break;
        case AL_EXTRACTION_OPEN :
        case AL_COPY_OPEN :
        case AL_INSERTION_OPEN :
            if ( miMonitorType == AL_MONITOR_OBJECTS ) {
                mrStream << job->mpStorageObject->mName.GetSafeName() << " ";
                for ( i = 0 ; i < miBarLength ; i++ )
                    mrStream << BACKGROUND;
                for ( i = 0 ; i < miBarLength ; i++ )
                    mrStream << '\b';
            }
            break;
        case AL_EXTRACTION_CLOSE :
        case AL_COPY_CLOSE :
            if ( miMonitorType == AL_MONITOR_OBJECTS ) {
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << '\b';
//
// I'm trying an alternative here that was necessitated
// by 'extracting' directory entries.
//
#if 0
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << ' ';
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << '\b';
#else
                for ( i = 0 ; i < miBarLength ; i++ )
                    mrStream << ' ';
                for ( i = 0 ; i < miBarLength ; i++ )
                    mrStream << '\b';
#endif
                mrStream << "\n";
                miCurrentOffset = 0;
            }
            break;
        case AL_INSERTION_CLOSE :
                if ( miMonitorType == AL_MONITOR_OBJECTS ) {
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << '\b';
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << ' ';
                for ( i = 0 ; i < miCurrentOffset ; i++ )
                    mrStream << '\b';
                mrStream << job->CompressionRatio() << "%\n";
                miCurrentOffset = 0;
            }
            break;
//
// I decided the aesthetics were better without these messages.  I am
// leaving them here to look at in case we want to try them again
// some day.
//
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


