//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ARCENTRY.CPv   1.0   12 Mar 1998 12:14:54   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ARCENTRY.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:54   DCHI
// Initial revision.
// 
//************************************************************************

//
// ARCENTRY.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntry::operator new()
//  ALEntry::ALEntry()
//  newALEntry()
//  ALEntry::~ALEntry()
//  deleteALEntry()
//  ALEntry::InsertBefore()
//  ALEntry::GetNextEntry()
//  ALEntryGetNextEntry()
//
// DESCRIPTION
//
//  Class ALEntry describes the state of an object that is in an archive.
//  It contains a pointer to a storage object and two engines,
//  which define what goes in the archive and how it is put there.  It
//  also defines how to extract it.  The ALEntry object also contains
//  miscellaneous items that go with the object in the archive, such
//  as its time/date stamp, its CRC, and its comment.
//
//  You have to create an ALEntry *before* you put an object into an
//  archive.  The archiving class member function figure out what you want
//  to do by looking at objects of the ALEntry.  You also have to read
//  the contents of the archive into a list of ALEntry objects before you
//  can extract anything.
//
// REVISION HISTORY
//
//   May 23, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release.  The big change in 2.0 is
//                             that the entry now contains a pointer to
//                             a compressor and a decompressor, instead
//                             of a single pointer to an engine.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALEntry::operator new()
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
//  void * ALEntry::operator new( size_t size )
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
//  size  :  The number of bytes needed to create a new ALEntry object.
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
ALEntry::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}
#endif

//
// NAME
//
//  ALEntry::ALEntry()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The constructor for ALEntry objects.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  ALEntry::ALEntry( ALEntryList &list,
//                    ALStorage *object,
//                    ALCompressor *compressor,
//                    ALDecompressor *decompressor );
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  hALEntry AL_FUNCTION newALEntry( hALEntryList list,
//                                   hALStorage storage,
//                                   hALCompressor compressor,
//                                   hALDecompressor decompressor );
//
// VB SYNOPSIS
//
//  Declare Function newALEntry Lib "AL20LW"
//    (ByVal list&,
//     ByVal storage&,
//     ByVal compressor&,
//     ByVal decompressor&) As Long
//
// DELPHI SYNOPSIS
//
//  function newALEntry( list : hALEntryList;
//                       storage : hALStorage;
//                       compressor : hALCompressor;
//                       decompressor : hALDecompressor ) : hALEntry;
//
// ARGUMENTS
//
//  list        :  A reference to the list the ALEntry object is going to
//                 be linked into. ALEntry objects aren't allowed to exist
//                 without being in a list.
//
//  object      :  A pointer to the storage object that is attached to this
//                 entry.  Remember, this is an unopened storage object,
//                 so it is not consuming very much space.  It is okay
//                 to have a zillion or so of these just lying around.
//                 Don't forget that the ALEntry dtor is going to destroy
//                 this guy for you, don't you dare try it!.
//
//  compressor  :  A pointer to the compression engine that is going to
//                 be used to create/insert the storage object into
//                 the archive.  Just like with the storage object
//                 it is a low cost object, and you can keep lots
//                 of them on hand.  This engine will be destroyed in the
//                 ALEntry dtor, so be sure to give up any claim you might
//                 have on this guy.  If you aren't going to be compressing
//                 the object, a value of 0 is perfectly acceptable here.
//
//  decompressor:  A pointer to the decompression engine that is going to
//                 be used to extractt the storage object from the archive.
//                 Just like with the compression engine, it is a low cost
//                 object, and you can keep lots of them on hand.  This
//                 engine will be destroyed in the ALEntry dtor, so be sure
//                 to give up any claim you might have on this guy.  If you
//                 aren't going to be decompressing the object, a value of 0
//                 is perfectly acceptable here.
//
//
// DESCRIPTION
//
//  This ctor creates a new ALEntry object.  You can do this by hand, but
//  frequently you will ask ArchiveLib to create ALEntry objects for you,
//  maybe by pulling them out of a list box, or reading them in from and
//  archive.  Note that ALEntry objects aren't allowed to ever exist
//  outside a list, each entry absolutely has to appear in a list.
//
//  dtor issues relating to the ALEntry object are very important.  Since
//  ALEntry objects always are part of a list, it made sense for the 
//  ALEntryList destructor to clean up all the entries in its list.  So
//  even though you might have created this ALEntry object, you don't get to
//  delete it, that will be done for you.
//
//  Also, the storage object and compression engine in the ALEntry object
//  are going to be automatically destroyed by the ALEntry dtor.  Don't
//  even think about trying it yourself!
//
//  You can think of an ALEntryList as a directory of an archive, and each
//  ALEntry object in the list is a single entry in that directory.
//
//
// RETURNS
//
//  The C++ constructor doesn't return anything.  The C/VB/Delphi versions
//  of the constructor point to a newly created object, or perhaps to
//  0 if the constructor failed.
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

AL_PROTO
ALEntry::ALEntry( ALEntryList &list,  /* Tag public function */
                  ALStorage *object,
                  ALCompressor *compressor,
                  ALDecompressor *decompressor )
    : mrList( list ) // Initialize our own pointer to the list we will
                     // be a member of.
{
    mpNextItem = this;
    mpPreviousItem = this;
    mpStorageObject = object;
    mpCompressor = compressor;
    mpDecompressor = decompressor;
    mlCompressedSize = -1;
    mlCompressedObjectPosition = -1;
    miMark = 1; //Always construct with the mark turned on
    mszComment = 0;
//
// I check for the object member to be non-zero because of a clunky design
// choice I made a while back.  Each ALEntryList has an ALEntry member that
// points to the first and last members of the list.  I could have (and
// probably should have) made the root of the list just be a pair of pointers,
// instead of a dummy ALEntry.  Anyway, I can tell that dummy entry apart
// from the valid entries by virtue of the fact that it has a null
// pointer in its object pointer.
//
// So anyway, when I create this dummy object, I don't want to try to add
// it to the list, because by definition it is already in the list.  So
// I do a check before adding any ALEntry to the list.
//
    if ( object )
        InsertBefore( *list.mpListHead );
}


#if !defined( AL_NO_C )

extern "C" AL_LINKAGE
hALEntry AL_FUNCTION newALEntry( hALEntryList list,  /* Tag public function */
                                 hALStorage storage,
                                 hALCompressor compressor,
                                 hALDecompressor decompressor )
{
    AL_ASSERT_OBJECT( list, ALEntryList, "newALEntry" );
    AL_ASSERT_OBJECT( storage, ALStorage, "newALEntry" );
    if ( compressor )
        AL_ASSERT_OBJECT( compressor, ALCompressor, "newALEntry" );
    if ( decompressor )
        AL_ASSERT_OBJECT( decompressor, ALDecompressor, "newALEntry" );
    return (hALEntry) new ALEntry( *( (ALEntryList *) list ),
                                   (ALStorage * ) storage,
                                   (ALCompressor *) compressor,
                                   (ALDecompressor *) decompressor );
}

#endif

//
// NAME
//
//  ALEntry::~ALEntry()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Destructor for ALEntry objects.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  ALEntry::~ALEntry()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void deleteALEntry( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALEntry Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALEntry( this_object : hALEntry );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object that
//                  is going to be deleted.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This destructor should normally be called by the ALEntryList dtor.  The
//  list that owns an entry will always try to delete it when the list
//  is deleted.
//
//  The ALEntry object tries to delete four dynamically allocated objects
//  that it has control over:  the storage object, the compression engines,
//  and the comment.  In each case it won't do it if the object pointer
//  is 0.  This provides a convenient mechanism for you to steal a storage
//  object from an ALEntry.  All you have to do is take the pointer, and
//  then sen ALEntry::mpStorageObject to 0.  This is an especially useful
//  thing to do for ALMemory objects.
//
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
//  None, we don't want you to call this on your own.
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 23, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release

AL_PROTO
ALEntry::~ALEntry()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALEntry: Attempting to delete invalid object" );
    if ( mszComment )
        delete[] mszComment;
    if ( mpStorageObject != 0 )
        delete mpStorageObject;
    if ( mpCompressor != 0 )
        delete mpCompressor;
    if ( mpDecompressor !=0 )
        delete mpDecompressor;
    AL_ASSERT( mpNextItem != 0 ,"~ALEntry: next item is null" );
    AL_ASSERT( mpPreviousItem != 0, "~ALEntry: previous item is null" );

    ALEntry *next_job = mpNextItem;
    ALEntry *previous_job = mpPreviousItem;

    if ( next_job != this ) {
        next_job->mpPreviousItem = previous_job;
        previous_job->mpNextItem = next_job;
    }
//
// Note that I check the object twice, one at the start of the dtor, and
// once again at the end.  With all the linked list and dynamic deletion
// being done here, it seems like it would be really easy to hose things
// up if any mistakes were made.
//
    AL_ASSERT( GoodTag(), "~ALEntry: Attempting to delete invalid object" );
}

#if !defined( AL_NO_C )
extern "C" AL_LINKAGE void AL_FUNCTION
deleteALEntry( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "deleteALEntry" );
    delete (ALEntry *) this_object;
}
#endif

//
// NAME
//
//  ALEntry::InsertBefore()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Insert an ALEntry into the list before another job.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntry::InsertBefore( ALEntry &job )
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
//  job : A reference to another job in the target list.
//
// DESCRIPTION
//
//  This function is used inside the ALEntry class to add a new ALEntry
//  object to an ALEntryList.  Since the list is a doubly linked list, the
//  code to do the job is pretty simple.  It would have been a little more
//  complicated if I used a pair of pointers in the ALEntryList to start
//  the list, instead of a dummy ALEntry object.
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
//   May 23, 1994  1.0A  : First release
//
//   February 14, 1996  2.0A : New release

void AL_PROTO
ALEntry::InsertBefore( ALEntry &job ) /* Tag protected function */
{
    mpNextItem = &job;
    mpPreviousItem = job.mpPreviousItem;
    (job.mpPreviousItem)->mpNextItem = this;
    job.mpPreviousItem = this;
}

//
// NAME
//
//  ALEntry::GetNextEntry()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the next entry in the list.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  ALEntry * ALEntry::GetNextEntry()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  hALEntry ALEntryGetNextEntry( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetNextEntry Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetNextEntry( this_object : hALEntry ) : hALEntry;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object being used.
//                  Note that the C++ version of this call doesn't have an
//                  explicit argument for this_object, since it has access to
//                  'this' implicitly.
//
// DESCRIPTION
//
//  This function is used to iterate through an ALEntryList.  Each entry has
//  a pointer to the next and previous entries, so this function is really
//  simple.  The only complication comes from trying to detect the end of
//  the list, which is denoted by the list head instance of ALEntry.  We
//  can tell it apart from all the legitimate entries by the fact that
//  its storage object is 0.
//
// RETURNS
//
//  A pointer to the next entry in the list.  If the next entry is the
//  list head, it means we have reached the end of the list, and a value
//  of 0 is returned.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   May 23, 1994  1.0A  : First release
//

ALEntry AL_DLL_FAR * AL_PROTO
ALEntry::GetNextEntry()  /* Tag public function */
{
    ALEntry *next_entry = this->mpNextItem;
//
// The list head has the special case where both the compression engine
// and storage object pointers are 0, and that makes the end of the list.
//
//    if ( mpNextItem->mpStorageObject == 0 )
//
// Try a new technique.  I need to, because lately I've been making
// a lot of entries with zeros in one or more of the entries.
//
    if ( next_entry == mrList.mpListHead )
        return 0;
    else
        return next_entry;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALEntry AL_FUNCTION
ALEntryGetNextEntry( hALEntry this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALEntry, "ALEntryGetNextEntry" );
    return (hALEntry) ( ((ALEntry *) this_object )->GetNextEntry() );
}

#endif


