//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/_NEW.CPv   1.0   12 Mar 1998 12:14:22   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/_NEW.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:22   DCHI
// Initial revision.
// 
//************************************************************************

//
// _NEW.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  PointerInHeap()
//  operator new()
//  operator delete()
//
// DESCRIPTION
//
//  One of the defensive programming measures we have taken in this
//  library is to provide versions of ::new and ::delete that do a
//  little extra work.  Of course, these all go away if _DEBUG isn't
//  defined.
//
//  Basically, there are a couple of things at work here.   First, we
//  have taken over the new operator and delete operator, so we are
//  guaranteed that all C++ memory allocation will take place through
//  these routines.  Therefore, we can take certain liberties with
//  them.
//
//  The most important thing we do when someone wants to allocate
//  memory is to allocate an extra 12 bytes of data beyond what has
//  been requested.  We use 8 bytes at the start of the block and
//  4 bytes at the end of the block for our own purposes.  The pointer
//  we return to the requester is actually at the start of the block
//  plus eight bytes.
//
//  The first four bytes at the start of the block are used to store
//  the size of the block.  The next four bytes hold a long word containing
//  a leading picket, which is just a special word four bytes long.
//  If the user underwrites the block of data for some reason, one of
//  those four bytes will probably be corrupted.  At the end of the block,
//  we store a trailing picket that has the same purpose.  It holds
//  a special pattern of four bytes.  If the user overwrites the block
//  of data, those four bytes will be corrupted.
//
//  We check the pickets when the ::delete operator is called.  That way,
//  when an object is going to be freed, we can instantly detect if it
//  has been abused in some fashion.
//
//  The ::delete operator here also attempts to make sure that the pointer
//  being deleted points to a block that is actually in the heap.  This
//  isn't always possible, but it works under most MS-DOS models, and
//  works under Windows small and medium models.  Under Windows large
//  memory models, we can walk the global heap to look for pointers,
//  but we might not find them, since the RTL might be using a subsegment
//  allocation scheme.
//
//  In addition to checking the heap, under Windows the ::delete function
//  can also call the IsBadWritePtr() function to see if this is just
//  a completely hosed up pointer.
//
//  Note that it is kind of obtrusive to redefine ::new and ::delete.  There
//  is an excellent chance that this will interfere with other libraries,
//  such as MFC.  Fortunately, we have made it easy to get around this.
//  First, it is relatively simple to just delete this module from your
//  library, using:  LIB ALXX-_NEW;  If you don't want to go to that
//  trouble, you can also define AL_DISABLE_NEW and rebuild this module,
//  which should also make it go away.  But if you don't need to make
//  this code disappear, you ought to leave it in, it might save you a lot
//  of trouble some day.
//
//  It's getting harder and harder to support this library, it interferes
//  with things like MFC.  It may be removed from the library sometime soon.
//
// REVISION HISTORY
//
//  May 22, 1994  1.0A    : First release
//
//  July 7, 1994  1.0B    : Had to make some minor adjustments in picket
//                          management to work properly under UNIX.
//
//  January 9, 1995 1.01A : This stuff doesn't work *at all* when I am
//                          am inside Borland's Power Pack DLLs.  This
//                          code gets turned off in that case.
//
//  February 14, 1996 2.0A : New release w/no substantial changes
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <stdio.h>
#include <stdlib.h>
//
// The MS-DOS heapwalk functions are in different header files depending
// on who you are.  Note that I don't walk the heap under MS-DOS with
// Symantec or Watcom.  Not sure if I can.
//
#if defined( AL_BORLAND )
#include <alloc.h>
#elif defined( AL_MICROSOFT )
#include <malloc.h>
#endif
//
// Walking the Windows heap requires TOOLHELP.DLL.  It would be great
// if Borland provided the TOOLHELP API under their DPMI extenders, but
// I don't think they do.  I don't think the heap walk functions are
// available under Win 32s either.
//
#if defined( AL_WINDOWS ) && !defined( AL_FLAT_MODEL )
#include <toolhelp.h>
#endif

//
// If I am using 16 bit Powerpack in a DLL, this stuff doesn't work.  I
// detect it and disable it.
//
#if defined( AL_BORLAND ) && defined( __DPMI16__ )
#ifndef AL_DISABLE_NEW
#define AL_DISABLE_NEW
#endif
#endif

//
// I don't think this will work when I am using the DLL version of
// the Run Time Library under Borland
//
#if defined( AL_BORLAND ) && defined( _RTLDLL )
#ifndef AL_DISABLE_NEW
#define AL_DISABLE_NEW
#endif
#endif

//
// Microsoft VC++ 4.0 doesn't work properly either...
//
#if defined( AL_MICROSOFT ) && (_MSC_VER >= 1000 )
#ifndef AL_DISABLE_NEW
#define AL_DISABLE_NEW
#endif
#endif
/*
 * To completely eliminate this stuff, all you have to do is define
 * AL_DISABLE_NEW before rebuilding the library.
 */
#ifndef AL_DISABLE_NEW
//
// When I pop up an error message, it sometimes helps to know where it came
// from.  This definition is used to create the message box.
//
#if defined( AL_BUILDING_DLL )
#define LIB_TYPE "DLL"
#else
#define LIB_TYPE "Static"
#endif

//
// If Debug is not turned on, none of this stuff happens.  I also don't
// work with Microsoft huge model, things get nasty in there.
//
#if defined( _DEBUG ) && !( defined( AL_MICROSOFT ) && defined( _M_I86HM ) )

//
// int PointerInHeap( void *p )
//
// ARGUMENTS:
//
//  p  : The pointer under test.
//
// RETURNS
//
//  An integer, true or false.
//
// DESCRIPTION
//
//  This function is called by ::delete() to see if the pointer we are
//  trying to delete is in fact in the heap. If it isn't, we could cause
//  quite a bit of trouble if we try to delete it.
//
//  Under MS-DOS, this function just executes the normal heapwalk functions
//  supported by Microsoft and Borland.  Under Windows small memory
//  models, we use the Toohelp API to walk the local heap.  Under all
//  other circumstances, we just give up and always return a true value.
//
// REVISION HISTORY
//
//   May 22, 1994  1.0A  : First release
//
//   February 14, 1996 2.0A : New release
//

//
// This is the Microsoft MS-DOS version.  It seems to also work
// with Win32s, and Windows NT Console.  Very confusing,
// I hope the #ifdefs all work out!
//
// This function just uses the heapwalk RTL function to check for the
// presence of the pointer in the heap.
//
#if defined( AL_MICROSOFT ) &&                                     \
    ( !defined( AL_WINDOWS ) || defined( AL_FLAT_MODEL ) )

int
PointerInHeap( void *p )  /* Tag debug function */
{
    AL_ASSERT( _heapchk() == _HEAPOK, "Heap fails internal consistency check" );
    _HEAPINFO heapinfo;
    heapinfo._pentry = 0;
    while ( _heapwalk( &heapinfo ) == _HEAPOK )
#if defined( AL_FLAT_MODEL )
        if ( heapinfo._pentry == (int *) p )
#else
        if ( heapinfo._pentry == (int __far *) p )
#endif
            return 1;
    return 0;
}

//
// This is the Borland MS-DOS version.  It looks like it also works under
// Win 32s, which might be more reasonable, since we do support Borland
// in that mode. 
//
// Like the previous function, this guy just uses the heapwalk API to
// check the local heap for the presence of the pointer.
//
#elif defined( AL_BORLAND ) && ( !defined( AL_WINDOWS ) || defined( AL_FLAT_MODEL ) )

int
PointerInHeap( void *p )  /* Tag debug function */
{
    AL_ASSERT( heapcheck() == _HEAPOK,
               LIB_TYPE " heap fails internal consistency check" );
    struct heapinfo info;
    info.ptr = 0;
    while ( heapwalk( &info ) == _HEAPOK )
#if defined( AL_LARGE_DATA ) && !defined( AL_FLAT_MODEL )
        if ( info.ptr == (void huge *) p )
            return 1;
#else
        if ( info.ptr == p )
            return 1;
#endif
    return 0;
}

//
// Microsoft is nice enough to support the heapwalk API under Windows
// large memory models also.  This is good, since the TOOLHELP API would
// flounder when confronted with a subsegment allocation strategy.
//
#elif defined( AL_MICROSOFT ) && defined( AL_WINDOWS ) && defined( AL_LARGE_DATA )

int
PointerInHeap( void *p )  /* Tag debug function */
{
    AL_ASSERT( _fheapchk() == _HEAPOK,
               LIB_TYPE " heap fails internal consistency check" );
    _HEAPINFO heapinfo;
    heapinfo._pentry = 0;
    while ( _fheapwalk( &heapinfo ) == _HEAPOK )
        if ( heapinfo._pentry == p )
            return 1;
    return 0;
}

//
// Under Windows small and medium memory models, the TOOLHELP API lets
// us walk the local heap, looking for an entry.  No subsegment allocation
// scheme will get in the way.
//
// I should be able to use this with Watcom, but I am using a little bit of
// inline assembly to get my data segment.  This inline assembly won't
// work with Watcom, so someday I will have to add a little code to
// get things working right with them also.
//
#elif defined( AL_WINDOWS ) && !defined( AL_FLAT_MODEL ) && !defined( AL_LARGE_DATA ) && !defined( AL_WATCOM )

int
PointerInHeap( void *p )  /* Tag debug function */
{
    LOCALENTRY LEntry;
    WORD wHeap;
//
// I need to search the local heap that is in my data segment.
//
    _asm mov ax,ds
    _asm mov wHeap,ax
    LEntry.dwSize = sizeof( LOCALENTRY );
    if ( LocalFirst( &LEntry, (HGLOBAL) wHeap ) ) {
        do {
            if ( LEntry.wAddress == (WORD) p )
                return 1;
        } while ( LocalNext( &LEntry ) );
    }
    return 0;
}

//
// When all else fails, give up!
//
#else
int
PointerInHeap( void * )  /* Tag debug function */
{
    return 1;
}
#endif

//
// void *operator new( size_t size )
//
// ARGUMENTS:
//
//  size  : The amount of memory being requested.
//
// RETURNS
//
//  A pointer to the newly allocated storage area, or a 0 in the event
//  of failure.
//
// DESCRIPTION
//
//  This version of ::new() does what I described at the top of the file.
//  It allocates a block of memory as requested, and includes eight
//  extra bytes.  Four bytes are reserved at the start and end of the memory
//  block for our "pickets".  These pickets hold a fixed pattern in memory
//  that can be tested for accidental modification.  When ::delete() is 
//  called, we check the area to see if the caller munged it, and
//  cause an assertion error if they did.  The other four bytes are needed
//  to keep the size of the block on hand.  Otherwise I wouldn't know how
//  to get to the end of the block to check the trailing picket.
//
//  Note that if you are using set_new_handler() or exceptions, this stuff
//  is probably going to hose you up badly.
//
// REVISION HISTORY
//
//   May 22, 1994  1.0A  : First release
//
//   July 7, 1994  1.0B  : On Sun UNIX, if I tried to write a long out to
//                         a misaligned address, I get a fault.  The leading
//                         pickets will always be properly aligned, but the
//                         trailing picket could land anywhere.  So, under
//                         UNIX, I store the trailing picket a byte at a time.
//
//   Februray 14, 1996  2.0A : New release
//
void *
operator new( size_t size )  /* Tag debug function */
{
    if ( ( (long) size + 12 ) > 65535L )
        return 0;
    char *p = (char *) malloc( size + 12 );
    if ( !p )
        return 0;
    ( (long *) p)[ 0 ] = (long) size;
    ( (long *) p)[ 1 ] = 0x12345678L;
    ( (long *)(p + 8 + size))[ 0 ] = 0xfedcba98L;
    return p + 8;
}

//
// void operator delete( void *ptr )
//
// ARGUMENTS:
//
//  ptr :  A pointer to the memory block the user wishes to delete.
//
// RETURNS
//
//  Nothing.
//
// DESCRIPTION
//
//  After the user has had the chance to muck with this memory block for
//  a while, he or she will want to return it to the heap.  We do a bunch
//  of checks here before that happens, to see if any serious mistakes have
//  been made.  If we detect any serious mistakes, we just abort the
//  program with an assertion error.
//
//  First we check to see if Windows thinks it is even a valid pointer.  If
//  we don't do this, some of the other code here will GPF if you call delete
//  with a really bad pointer.  Those GPFs are a lot less informative than
//  our nice assertion failures.
// 
//  If it looks like it is a valid pointer, the next thing we do is try to
//  see if the pointer is in our heap.  A common mistake is trying to free
//  a pointer twice, or freeing a pointer that has been incremented or
//  decremented.  Either of these can royally foul the heap.
//
//  If it looks like the pointer really is in the heap, there is still one
//  last thing to check.  I take a quick glance at both the leading and
//  trailing pickets to see if either of them have been mangled.  A simple
//  overwrite or underwrite by just one byte can be catastrophic, but we
//  detect it easily here.
//
//  If all of that goes as expected, we are free to finally return the
//  storage to the heap.  Just for good luck, I clear it out first.  That
//  way if anyone is foolish enough to try and use the data after it has
//  been deleted, they will at least see that there is nothing intelligent
//  store there.
//
// REVISION HISTORY
//
//   May 22, 1994  1.0A  : First release
//
//   July 7, 1994  1.0B  : On Sun UNIX, if I tried to read a long from
//                         a misaligned address, I get a fault.  The leading
//                         pickets will always be properly aligned, but the
//                         trailing picket could land anywhere.  So, under
//                         UNIX, I check the trailing picket a byte at a time.
//
//   February 14, 1996 2.0A : New release.

void
operator delete( void *ptr )  /* Tag debug function */
{
#if defined( AL_WINDOWS ) && !defined( AL_FLAT_MODEL )
    AL_ASSERT( !IsBadWritePtr( ptr, 1 ), "delete: delete called for ptr Windows doesn't like" );
#endif
    char *p = (char *) ptr;
    AL_ASSERT( PointerInHeap( p - 8 ),
               "delete: delete called for pointer not found in the " LIB_TYPE " heap" );
    AL_ASSERT( ( (long *) p )[ -1 ] == 0x12345678L,
                "delete : Data corrupted in object's leading picket in the " LIB_TYPE " heap"  );
    size_t size = (size_t) ( (long *) p )[ -2 ];
#if defined AL_UNIX
    long picket = (unsigned char) p[ size ] << 24;
    picket |= (unsigned char) p[ size + 1 ] << 16;
    picket |= (unsigned char) p[ size + 2 ] << 8;
    picket |= (unsigned char) p[ size + 3 ];
#else
    long picket = ( (long *)( p + size ) )[ 0 ];
#endif
    AL_ASSERT( picket == 0xfedcba98L,
                "delete : Data corrupted in object's trailing picket in the " LIB_TYPE " heap" );
    memset( p - 8, size + 12, 0 ); //Clear it before freeing it
    free( ((char *) p - 8 ) );
}

#endif //#ifdef _DEBUG etc.

#endif // #ifdef AL_DISABLE_NEW

