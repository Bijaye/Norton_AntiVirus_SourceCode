//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCLIST.CPv   1.0   12 Mar 1998 12:14:56   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCLIST.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:56   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCLIST.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryList::operator new()
//  ALEntryList::ALEntryList()
//  newALEntryList()
//  ALEntryList::ALEntryList(ALEntryList&)
//  ALEntryList::~ALEntryList()
//  deleteALEntryList()
//  ALEntryList::GetFirstEntry()
//  ALEntryListGetFirstEntry()
//
// DESCRIPTION
//
//  ALEntryList is simply a class that is used to keep a linked list of
//  ALEntry objects together.  ALEntryList objects are passed to the high
//  level ALArchiveBase functions for common operations, such as Create()
//  Extract(), and so on.  You get an ALEntryList back when you read the
//  directory of an archive.  The ALEntryList object tells you everything
//  there is to know about the object stored in the archive.
//
//  In archive lib 2.0, entry lists now contain a toolkit also.  The
//  toolkit is used when extracting objects from an archive, or when
//  adding items to the list.  It provides a factory that is used to
//  create new storage objects and engines.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALEntryList::operator new()
//
// PLATFORMS/ENVIRONMENTS
//
//  Windows
//  C++
//
// SHORT DESCRIPTION
//
//  Memory allocation operator needed with DLL.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  void * ALEntryList::operator new( size_t size )
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
//  size  :  The number of bytes needed to create a new ALEntryList object.
//
// DESCRIPTION
//
//  When using a DLL, it is easy to create a dangerous situation when
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
//
// RETURNS
//
//  A pointer to the newly allocated storage area, or 0 if no storage
//  was available.
//
// EXAMPLE
//
//  None.
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 23, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALEntryList::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALEntryList::ALEntryList()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create a new ALEntry list
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALEntryList::ALEntryList( ALMonitor * monitor,
//                            ALToolKit toolkit )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  hALEntryList newALEntryList( hALMonitor monitor, hALToolkit toolkit );
//
// VB SYNOPSIS
//
//  Declare Function newALEntryList Lib "AL20LW"
//    (ByVal monitor&, ByVal toolkit) As Long
//
// DELPHI SYNOPSIS
//
//  function newALEntryList( monitor : hALMonitor;
//                           toolkit : hALTookit ) : hALEntryList;
//
// ARGUMENTS
//
//  monitor : A pointer to a monitor that will be used whenever we are
//            processing objects in the list.  If no argument is supplied,
//            (in the C++ version), the default argument value of 0 is used.
//            When the ctor sees that the value of the monitor pointer is 0,
//            it assigns the default monitor instead.
//
//  toolkit : The toolkit is a collection of storage objects, compressors,
//            and decompressors.  This toolkit is used whenever the archving
//            software needs to create one of the three types of objects.
//            The components you put in your toolkit will determine what sort
//            of compression and storage your archives will use.
//
//
// DESCRIPTION
//
//  Constructing an ALEntryList object doesn't take much work.  I have to
//  initialize three data members.  The first is the pointer to the monitor
//  that will be used when processing objects in the list.  The second is
//  a copy of a toolkit that you plan to use.  The final data element is
//  the root of the linked list, which is a dummy ALEntry object.  Note
//  that the root is created as a dummy by setting the storage object pointer
//  to 0.
//
//  The default monitor is defined below.  If you don't specify a real
//  monitor, you get the default, which is a do nothing function.  Everyone
//  can share one instance of the default monitor, because it doesn't have
//  any data members to be concerned about.
//
//
// RETURNS
//
//  Nothing in C++.  A handle (pointer) to the newly created list when
//  called from C, VB, or Delph.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//   February 14, 1996  2.0A : New release, with the toolkit marking a major
//                             change.
//

//
// This default monitor does absolutely nothing.  It's what I
// use as the monitor if the user doesn't specify one.
//

ALMonitor _ALDefaultMonitor( AL_MONITOR_OBJECTS );

AL_PROTO
ALEntryList::ALEntryList( ALMonitor AL_DLL_FAR * monitor,  /* Tag public function */
                          ALToolKit toolkit )
    : mrMonitor( monitor ? *monitor : _ALDefaultMonitor ),
      mToolKit( toolkit )
{
    mpListHead = new ALEntry( *this, 0, 0, 0 );
}

#if !defined( AL_NO_C )
//
// This fn is defined for C users, but I don't think it
// does them any good.  It's just too difficult to deal
// with toolkit issues.  instead, I have a bunch of
// special constructors that add a specific toolkit.
//

extern "C" AL_LINKAGE hALEntryList AL_FUNCTION
newALEntryList( hALMonitor monitor, hALToolKit toolkit )  /* Tag public function */
{
    if ( monitor != 0 )
        AL_ASSERT(  ((ALMonitor *) monitor)->GoodTag(),
                    "monitor argument is not a valid ALMonitor" );
    AL_ASSERT_OBJECT( toolkit, ALToolKit, "newALEntryList" );
    ALEntryList *list = new ALEntryList( (ALMonitor *) monitor,
                                         *( ( ALToolKit *) toolkit ) );
    return (hALEntryList) list;
}

#endif

//
// NAME
//
//  ALEntryList::ALEntryList()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALEntryList copy constructor
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALEntryList::ALEntryList( ALEntryList &rhs );
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
//  rhs    :  A reference to an existing ALEntryList object that is going
//            to be copied into the new object.
//
// DESCRIPTION
//
//  There are times in ArchiveLib where one entry list needs to be copied
//  to another.  When this occurs, we don't care about the objects in the
//  list, we just want to make sure we get a copy of the toolkit!  That's
//  what this is all about.
//
//  Note that even though this is a public function, it isn't ordinarily
//  something you want to use.
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
//   November 13, 1995  2.00A : First release.
//
//   February 14, 1996  2.0A : New release

AL_PROTO
ALEntryList::ALEntryList( ALEntryList AL_DLL_FAR &rhs )  /* Tag public function */
    : mrMonitor( rhs.mrMonitor ), mToolKit( rhs.mToolKit )
{
    mpListHead = new ALEntry( *this, 0, 0, 0 );
}

//
// NAME
//
//  ALEntryList::~ALEntryList()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The ALEntryList destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALEntryList::~ALEntryList();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void deleteALEntryList( hALEntryList this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALEntryList Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALEntryList( this_object : hALEntryList );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is going to be destroyed.  Note that the C++ version
//                  of this member function call uses 'this' as an implicit
//                  reference, instead of using this argument.
//
// DESCRIPTION
//
//  The destructor for ALEntryList goes through the list and deletes every
//  ALEntry object it finds.  Note that this also causes the ALEntry
//  object to destroy its storage object and compression engine.  Once
//  the whole list is obliterated, the list head ALEntry object can be
//  safely deleted.  Then the whole thing is done.
//
//  The toolkit destructor is automatically called as part of this process,
//  since it is a data member of the entry list object.
//
// RETURNS
//
//  None, destructors don't get to return anything.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//
//   February 14, 1996  2.0A : New release


AL_PROTO
ALEntryList::~ALEntryList()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALEntryList: attempting to delete invalid object" );
    ALEntry *job = GetFirstEntry();
    while ( job ) {
        ALEntry *next_job = job->GetNextEntry();
        delete job;
        job = next_job;
    }
    if ( mpListHead )
        delete mpListHead;
    AL_ASSERT( GoodTag(), "~ALEntryList: attempting to delete invalid object" );
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALEntryList( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "deleteALEntryList" );
    delete (ALEntryList *) this_object;
}

#endif

//
// NAME
//
//  ALEntryList::GetFirstEntry()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the first entry in a list.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  ALEntry * ALEntryList::GetFirstEntry()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  hALEntry ALEntryListGetFirstEntry( hALEntryList this_object )
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListGetFirstEntry Lib "AL20LW" (ByVal tihs_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryListGetFirstEntry( this_object : hALEntryList ) : hALEntry;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is in use.  Note that the C++ version of this function
//                  doesn't have an explicit argument to access this_object,
//                  since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  If you are going to iterate through the entire list, this function is
//  used to start you off.  It gets the first entry in the list by calling
//  GetNextEntry() for the list head.  Don't worry about what happens if
//  the list is empty, the GetNextEntry() code figures that out with no
//  problem, and returns a 0.
//
// RETURNS
//
//  A pointer to the first valid ALEntry object in the list, or 0 if there
//  are no entries.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 23, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release


ALEntry AL_DLL_FAR * AL_PROTO
ALEntryList::GetFirstEntry()  /* Tag public function */
{
    return mpListHead->GetNextEntry();
}


#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALEntry AL_FUNCTION
ALEntryListGetFirstEntry( hALEntryList this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntryList, "ALEntryListGetFirstEntry" );
    return (hALEntry) ( ((ALEntryList *) this_object )->GetFirstEntry() );

}

#endif

