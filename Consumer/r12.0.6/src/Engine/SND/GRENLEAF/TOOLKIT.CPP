//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/TOOLKIT.CPv   1.0   12 Mar 1998 12:18:00   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/TOOLKIT.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:18:00   DCHI
// Initial revision.
// 
//************************************************************************

//
// TOOLKIT.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALToolKit::operator new()
//  ALToolKit::ALToolKit()
//  ALToolKit::ALToolKit( const ALToolKit& )
//  ALToolKit::ALToolKit( const ALCompressor & )
//  ALToolKit::ALToolKit( const ALDecompressor & )
//  ALToolKit::ALToolKit( const ALStorage & )
//  ALToolKit::~ALToolKit()
//  deleteALToolKit()
//  ALToolKit::operator+( const ALCompressor & )
//  ALToolKit::operator+( const ALDecompressor & )
//  ALToolKit::operator+( const ALStorage & )
//  operator+( ALCompressor &, ALCompressor & )
//  operator+( ALCompressor &, ALDecompressor & )
//  operator+( ALCompressor &, ALStorage & )
//  operator+( ALDecompressor &, ALCompressor & )
//  operator+( ALDecompressor &, ALDecompressor & )
//  operator+( ALDecompressor &, ALStorage & )
//  operator+( ALStorage &, ALCompressor & )
//  operator+( ALStorage &, ALDecompressor & )
//  operator+( ALStorage &, ALStorage & )
//  ALToolKit::CreateCompressor()
//  ALToolKit::CreateDecompressor()
//  ALToolKit::CreateStorageObject()
//
// DESCRIPTION
//
//  A toolkit is simply a collection of compressors, decompressors,
//  and storage objects.  The exciting part of a toolkit is that
//  I let you add any one of these objects to the kit at any time
//  using operator+().  I also let you construct a toolkit
//  using any of these objects.
//
//  This file contains all the toolkit source code to keep all this
//  working.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALToolKit::operator new()
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
//  void * ALToolKit::operator new( size_t size )
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
//           necessary to construct a new ALToolKit object.
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
//   February 14, 1996  2.0A : New release.
//

#if defined( AL_BUILDING_DLL )

void AL_DLL_FAR * AL_PROTO
ALToolKit::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALToolKit::ALToolKit()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The default toolkit constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit::ALToolKit();
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  An ALToolKit object contains three arrays: a list of compressors,
//  decompressors, and storage objects.  When the default constructor is
//  invoked, the toolkit starts off completely empty, so all of the
//  arrays are set to 0, which is a special case, meaning the list is empty.
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
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALToolKit::ALToolKit()  /* Tag public function */
{
    mpCompressors = 0;
    mpDecompressors = 0;
    mpStorages = 0;
}

//
// NAME
//
//  ALToolKit::ALToolKit(const ALToolKit &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALToolKit copy constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit::ALToolKit( const ALToolKit &rhs );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  rhs  :  A reference to another toolkit.  This toolkit will be
//          copied during the creation of the new toolkit.
//
// DESCRIPTION
//
//  In order to provide easy manipulation of ALToolKit objects, a copy
//  constructor is a necessity.  The copy constructor requires that
//  we create a new toolkit that is just like the rhs argument.
//
//  The easiest way to make these toolkits the same would be to have
//  them both point to the same lists of compressors, decompressors,
//  and storage objects.  But having two containers that are free to jack
//  with the list at the same time would be a real monster.
//
//  So, isntead, the copy constructor creates new lists.  The
//  elements in the list are populated using the Clone() functions,
//  which should produce identical copies of the storage objects,
//  compressors, and decompressors.  When each item in the rsh has been
//  cloned, we have a new ALToolKit that isn't a binary copy of the
//  rhs, but will behave in an identical fashion.
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
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALToolKit::ALToolKit( const ALToolKit AL_DLL_FAR &rhs )  /* Tag public function */
{
    int i;
//
// If the rhs has a list of compressors, clone a copy.  Else set the
// list pointer to 0, indicating no compressors on hand.
//
    if ( rhs.mpCompressors ) {
        for ( i = 0 ; rhs.mpCompressors[ i ] ; i++ )
            ;
        mpCompressors = new ALCompressor AL_DLL_FAR *[ i + 1 ];
        if ( mpCompressors ) {
            for ( i = 0 ; rhs.mpCompressors[ i ] ; i++ )
                mpCompressors[ i ] = rhs.mpCompressors[ i ]->Clone( AL_COMPRESSION_DEFAULT );
            mpCompressors[ i ] = 0;
        }
    } else
        mpCompressors = 0;
//
// If the rhs has a list of decompressors, clone a copy.  Else set the
// list pointer to 0, indicating no decompressors on hand.
//
    if ( rhs.mpDecompressors ) {
        for ( i = 0 ; rhs.mpDecompressors[ i ] ; i++ )
            ;
        mpDecompressors = new ALDecompressor AL_DLL_FAR *[ i + 1 ];
        if ( mpDecompressors ) {
            for ( i = 0 ; rhs.mpDecompressors[ i ] ; i++ )
                mpDecompressors[ i ] = rhs.mpDecompressors[ i ]->Clone( AL_COMPRESSION_DEFAULT );
            mpDecompressors[ i ] = 0;
        }
    } else
        mpDecompressors = 0;
//
// If the rhs has a list of storage objects, clone a copy.  Else set the
// list pointer to 0, indicating no storage objects on hand.
//
    if ( rhs.mpStorages ) {
        for ( i = 0 ; rhs.mpStorages[ i ] ; i++ )
            ;
        mpStorages = new ALStorage AL_DLL_FAR *[ i + 1 ];
        if ( mpStorages ) {
            for ( i = 0 ; rhs.mpStorages[ i ] ; i++ )
                mpStorages[ i ] = rhs.mpStorages[ i ]->Clone( "", AL_STORAGE_DEFAULT );
            mpStorages[ i ] = 0;
        }
    } else
        mpStorages = 0;
}

//
// NAME
//
//  ALToolKit::ALToolKit(const ALCompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Construct a new ALToolKit that contains a copy of the compressor argument.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit::ALToolKit( const ALCompressor &c );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  c    :  A reference to a compressors.  When the toolkit is constructed,
//          it will contain a clone of this compressor.
//
// DESCRIPTION
//
//  ALToolKit contains lists of compressors, decompressors, and
//  storage objects.  In order to make toolkit manipulation as simple
//  as possible, constructors are provided that accept each of these
//  types of objects as arguments.  In this case, the argument is a
//  reference to a compressor.
//
//  The three lists are initialized in the constructor.  The
//  decompressor and storage object lists are initialized to be empty,
//  and the compressor list has a single clone of the compressor
//  specified as the rhs argument.
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
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALToolKit::ALToolKit( const ALCompressor AL_DLL_FAR &c )  /* Tag public function */
{
    mpCompressors = new ALCompressor AL_DLL_FAR *[ 2 ];
    if ( mpCompressors ) {
        mpCompressors[ 0 ] = c.Clone( AL_COMPRESSION_DEFAULT );
        mpCompressors[ 1 ] = 0;
    }
    mpDecompressors = 0;
    mpStorages = 0;
}

//
// NAME
//
//  ALToolKit::ALToolKit(const ALDecompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Construct an ALToolKit that contains a copy of the decompressor argument.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit::ALToolKit( const ALDecompressor &d );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  d    :  A reference to a decompressor.  When the toolkit is constructed,
//          it will contain a clone of this decompressor.
//
// DESCRIPTION
//
//  ALToolKit contains lists of compressors, decompressors, and
//  storage objects.  In order to make toolkit manipulation as simple
//  as possible, constructors are provided that accept each of these
//  types of objects as arguments.  In this case, the argument is a
//  reference to a decompressor.
//
//  The three lists are initialized in the constructor.  The
//  compressor and storage object lists are initialized to be empty,
//  and the decompressor list has a single clone of the decompressor
//  specified as the rhs argument.
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
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALToolKit::ALToolKit( const ALDecompressor AL_DLL_FAR &d )  /* Tag public function */
{
    mpCompressors = 0;
    mpDecompressors = new ALDecompressor AL_DLL_FAR *[ 2 ];
    if ( mpDecompressors ) {
        mpDecompressors[ 0 ] = d.Clone( AL_COMPRESSION_DEFAULT );
        mpDecompressors[ 1 ] = 0;
    }
    mpStorages = 0;
}

//
// NAME
//
//  ALToolKit::ALToolKit(const ALStorage &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Construct an ALToolKit that contains a copy of the storage object argument.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit::ALToolKit( const ALStorage &s );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  s    :  A reference to a storage object.  When the toolkit is constructed,
//          it will contain a clone of this object.
//
// DESCRIPTION
//
//  ALToolKit contains lists of compressors, decompressors, and
//  storage objects.  In order to make toolkit manipulation as simple
//  as possible, constructors are provided that accept each of these
//  types of objects as arguments.  In this case, the argument is a
//  reference to a storage object.
//
//  The three lists are initialized in the constructor.  The
//  compressor and decompressor object lists are initialized to be empty,
//  and the storage object list has a single clone of the storage object
//  specified as the rhs argument.
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
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALToolKit::ALToolKit( const ALStorage AL_DLL_FAR &s )  /* Tag public function */
{
    mpCompressors = 0;
    mpDecompressors = 0;
    mpStorages = new ALStorage AL_DLL_FAR *[ 2 ];
    if ( mpStorages ) {
        mpStorages[ 0 ] = s.Clone( "", AL_STORAGE_DEFAULT );
        mpStorages[ 1 ] = 0;
    }
}

//
// NAME
//
//  ALToolKit::~ALToolKit()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALToolKit destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit::~ALToolKit();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void deleteALToolKit( hALToolKit this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALToolKit Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALToolKit( this_object : hALArchive );
//
// ARGUMENTS
//
//  this_object : A reference or pointer to the ALToolKit object that
//                is going to be destroyed.  Note that the C++ member
//                function doesn't have this_object as an argument, because
//                it has implicit access to it via 'this'.
//
// DESCRIPTION
//
//  To destroy a toolkit, you need to work through each of the three lists,
//  destroying every object in the list.  After destroying all the objects
//  in the list, you then destroy the list itself.  Note that the list may
//  be empty when the destructor is called, in which case it doesn't need
//  to be deleted.  This is flagged by it having a 0 value.
//
//  This is the only member function of ALToolKit that has a C/VB/Delphi
//  translation function.  These languages will get working pointers to
//  constructed toolkits via the toolkit creation functions.  These toolkits
//  must be deleted in order to prevent memory leaks, so we have to have
//  a translation function.
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
//   February 14, 1996  2.0A : New release.
//

AL_PROTO
ALToolKit::~ALToolKit()  /* Tag public function */
{
    int i;

    if ( mpCompressors ) {
        for ( i = 0 ; mpCompressors[ i ] ; i++ )
            delete mpCompressors[ i ];
        delete[] mpCompressors;
    }
    if ( mpDecompressors ) {
        for ( i = 0 ; mpDecompressors[ i ] ; i++ )
            delete mpDecompressors[ i ];
        delete[] mpDecompressors;
    }
    if ( mpStorages ) {
        for ( i = 0 ; mpStorages[ i ] ; i++ )
            delete mpStorages[ i ];
        delete[] mpStorages;
    }
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALToolKit( hALToolKit this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALToolKit, "deleteALToolKit()" );
    delete (ALToolKit *) this_object;
}

#endif

//
// NAME
//
//  ALToolKit::operator+( const ALCompressor &c )
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Add a compressor object to a toolkit.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit &ALToolKit::operator+( const ALCompressor &c );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  c    :  A reference to an ALCompressor object.  A copy of this object
//          will be added to the toolkit.
//
// DESCRIPTION
//
//  This is one of three member operators that allow you to add a new
//  object to an existing toolkit.  The function just has to either create
//  a new list for the object, or expand the existing object by one entry.
//  The object is then copied into the list using the Clone() function.
//
// RETURNS
//
//  A reference to the toolkit.  This is done in standard C++ style to
//  allow cascaded operations.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

ALToolKit AL_DLL_FAR & AL_PROTO
ALToolKit::operator+( const ALCompressor AL_DLL_FAR &c )  /* Tag public function */
{
    if ( mpCompressors ) {
        int i;
        for ( i = 0 ; mpCompressors[ i ] ; i++ )
            ;
        ALCompressor AL_DLL_FAR **p = new ALCompressor AL_DLL_FAR *[ i + 2 ];
        if ( p ) {
            for ( int j = 0 ; j < i ; j++ )
                p[ j ] = mpCompressors[ j ];
            p[ i ] = c.Clone( AL_COMPRESSION_DEFAULT );
            p[ i + 1 ] = 0;
            delete[] mpCompressors;
            mpCompressors = p;
        }

    } else {
        mpCompressors = new ALCompressor AL_DLL_FAR *[ 2 ];
        if ( mpCompressors ) {
            mpCompressors[ 0 ] = c.Clone( AL_COMPRESSION_DEFAULT );
            mpCompressors[ 1 ] = 0;
        }
    }
    return *this;
}

//
// NAME
//
//  ALToolKit::operator+( const ALDecompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Add a decompressor object to a toolkit.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit &ALToolKit::operator+( const ALDecompressor &d );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  d    :  A reference to an ALDecompressor object.  A copy of this object
//          will be added to the toolkit.
//
// DESCRIPTION
//
//  This is one of three member operators that allow you to add a new
//  object to an existing toolkit.  The function just has to either create
//  a new list for the object, or expand the existing list by one entry.
//  The object is then copied into the list using the Clone() function.
//
// RETURNS
//
//  A reference to the toolkit.  This is done in standard C++ style to
//  allow cascaded operations.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

ALToolKit AL_DLL_FAR & AL_PROTO
ALToolKit::operator+( const ALDecompressor AL_DLL_FAR &d )  /* Tag public function */
{
    if ( mpDecompressors ) {
        int i;
        for ( i = 0 ; mpDecompressors[ i ] ; i++ )
            ;
        ALDecompressor AL_DLL_FAR **p = new ALDecompressor AL_DLL_FAR *[ i + 2 ];
        if ( p ) {
            for ( int j = 0 ; j < i ; j++ )
                p[ j ] = mpDecompressors[ j ];
            p[ i ] = d.Clone( AL_COMPRESSION_DEFAULT );
            p[ i + 1 ] = 0;
            delete[] mpDecompressors;
            mpDecompressors = p;
        }
    } else {
        mpDecompressors = new ALDecompressor AL_DLL_FAR *[ 2 ];
        if ( mpDecompressors ) {
            mpDecompressors[ 0 ] = d.Clone( AL_COMPRESSION_DEFAULT );
            mpDecompressors[ 1 ] = 0;
        }
    }
    return *this;
}

//
// NAME
//
//  ALToolKit::operator+( const ALStorage &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Add a storage object to a toolkit.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit &ALToolKit::operator+( const ALStorage &d );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  s    :  A reference to an ALStorage object.  A copy of this object
//          will be added to the toolkit.
//
// DESCRIPTION
//
//  This is one of three member operators that allow you to add a new
//  object to an existing toolkit.  The function just has to either create
//  a new list for the object, or expand the existing list by one entry.
//  The object is then copied into the list using the Clone() function.
//
// RETURNS
//
//  A reference to the toolkit.  This is done in standard C++ style to
//  allow cascaded operations.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

ALToolKit AL_DLL_FAR & AL_PROTO
ALToolKit::operator+( const ALStorage AL_DLL_FAR &s )  /* Tag public function */
{
    if ( mpStorages ) {
        int i;
        for ( i = 0 ; mpStorages[ i ] ; i++ )
            ;
        ALStorage AL_DLL_FAR **p = new ALStorage AL_DLL_FAR *[ i + 2 ];
        if ( p ) {
            for ( int j = 0 ; j < i ; j++ )
                p[ j ] = mpStorages[ j ];
            p[ i ] = s.Clone( "", AL_STORAGE_DEFAULT );
            p[ i + 1 ] = 0;
            delete[] mpStorages;
            mpStorages = p;
        }
    } else {
        mpStorages = new ALStorage AL_DLL_FAR *[ 2 ];
        if ( mpStorages ) {
            mpStorages[ 0 ] = s.Clone( "", AL_STORAGE_DEFAULT );
            mpStorages[ 1 ] = 0;
        }
    }
    return *this;
}

//
// NAME
//
//  operator+( const ALCompressor &,const ALCompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with two ALCompressor objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALCompressor &c1,
//                       const ALCompressor &c2 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  c1   :  A reference to an ALCompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  c2   :  A reference to a second ALCompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALCompressor AL_DLL_FAR & c1,  /* Tag public function */
           const ALCompressor AL_DLL_FAR & c2 )
{
    ALToolKit a( c1 );
    return a + c2;
}

//
// NAME
//
//  operator+( const ALCompressor &,const ALDecompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with and ALCompressor and ALDecompressor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALCompressor &c1,
//                       const ALDecompressor &d1 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  c1   :  A reference to an ALCompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  d1   :  A reference to an ALDecompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALCompressor AL_DLL_FAR & c1,  /* Tag public function */
           const ALDecompressor AL_DLL_FAR & d1 )
{
    ALToolKit a( c1 );
    return a + d1;
}

//
// NAME
//
//  operator+( const ALCompressor &,const ALStorage &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with and ALCompressor and ALStorage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALCompressor &c1,
//                       const ALStorage &s1 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  c1   :  A reference to an ALCompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  s1   :  A reference to an ALStorage object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALCompressor AL_DLL_FAR & c1,  /* Tag public function */
           const ALStorage AL_DLL_FAR & s1 )
{
    ALToolKit a( c1 );
    return a + s1;
}

//
// NAME
//
//  operator+( const ALDecompressor &,const ALCompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with and ALDecompressor and ALCompressor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALDecompressor &d1,
//                       const ALCompressor &c1 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  d1   :  A reference to an ALDecompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  c1   :  A reference to an ALCompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALDecompressor AL_DLL_FAR & d1,  /* Tag public function */
           const ALCompressor AL_DLL_FAR & c1 )
{
    ALToolKit a( d1 );
    return a + c1;
}

//
// NAME
//
//  operator+( const ALDecompressor &,const ALDecompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with two ALDecompressor objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALDecompressor &d1,
//                       const ALDecompressor &d2 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  d1   :  A reference to an ALDecompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  d2   :  A reference to a second ALDecompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALDecompressor AL_DLL_FAR & d1,  /* Tag public function */
           const ALDecompressor AL_DLL_FAR & d2 )
{
    ALToolKit a( d1 );
    return a + d2;
}

//
// NAME
//
//  operator+( const ALDecompressor &,const ALStorage &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with an ALDecompressor and an ALStorage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALDecompressor &d1,
//                       const ALStorage &s1 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  d1   :  A reference to an ALDecompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  s1   :  A reference to an ALStorage object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALDecompressor AL_DLL_FAR & d1,  /* Tag public function */
           const ALStorage AL_DLL_FAR & s1 )
{
    ALToolKit a( d1 );
    return a + s1;
}

//
// NAME
//
//  operator+( const ALStorage &,const ALCompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with an ALStorage and an ALCompressor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALStorage &s1,
//                       const ALCompressor &c1 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  s1   :  A reference to an ALStorage object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  c1   :  A reference to an ALCompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALStorage AL_DLL_FAR & s1,  /* Tag public function */
           const ALCompressor AL_DLL_FAR & c1 )
{
    ALToolKit a( s1 );
    return a + c1;
}

//
// NAME
//
//  operator+( const ALStorage &,const ALDecompressor &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with an ALStorage and an ALDecompressor object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALStorage &s1,
//                       const ALDecompressor &d1 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  s1   :  A reference to an ALStorage object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  d1   :  A reference to an ALDecompressor object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALStorage AL_DLL_FAR & s1,  /* Tag public function */
           const ALDecompressor AL_DLL_FAR & d1 )
{
    ALToolKit a( s1 );
    return a + d1;
}

//
// NAME
//
//  operator+( const ALStorage &,const ALStorage &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create a toolkit with two ALStorage objects.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALToolKit operator+( const ALStorage &s1,
//                       const ALStorage &s2 );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  s1   :  A reference to an ALStorage object.  A copy of this
//          object will be added to the resulting toolkit.
//
//  s2   :  A reference to a second ALStorage object.  A copy of this
//          object will be added to the resulting toolkit.
//
// DESCRIPTION
//
//  This is one of 9 global operators that overload the '+' symbol for
//  ALCompressor, ALDecompressor, and ALStorage objects.  The operator
//  creates an ALToolKit that is initialized with clones of the two
//  input objects.  These operators are provided to make it more convenient
//  to create ALToolKit objects, using syntax like this:
//
//    ALToolKit tk = ALFile() + ALCopyCompressor() + ALHugeMemory().
//
// RETURNS
//
//  A newly created toolkit.  Since the toolkit has a copy constructor
//  defined, we can get away with this, as opposed to returning a pointer
//  to a newly created toolkit object. This is a lot nicer.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   November 13, 1995  2.00A : First release.
//

AL_LINKAGE ALToolKit AL_FUNCTION
operator+( const ALStorage AL_DLL_FAR & s1,  /* Tag public function */
           const ALStorage AL_DLL_FAR & s2 )
{
    ALToolKit a( s1 );
    return a + s2;
}

//
// NAME
//
//  ALToolKit::CreateCompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create an ALCompressor object based on a given type.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALCompressor * ALToolKit::CreateCompressor( int engine_type );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  int engine_type :  An integer that describes the desirede engine type.
//                     This is normally either an integer that was stored
//                     in an archive along with a compressed object, or
//                     the AL_COMPRESSION_DEFAULT that is used to ask for
//                     the first available engine.
//
// DESCRIPTION
//
//  This function and its two siblings constitute the whole reason for the
//  esistence of a toolkit.  When inserting objects int an entry list, this
//  routine is called to build the appropriate engine.  The engine that
//  actually gets selected will depend on which toolkit has been passed
//  in the ALEntryList constructor.  This means that that archiving and
//  entry list functions can operate without any knowledge about specific
//  compression engines, which means they won't link in any engines that
//  aren't included in your toolkit.
//
//  This function works by repeatedly trying to clone each of the engines
//  in the compression engine list.  The Clone() function is called with
//  the engine_type argument.  If the compression engine feels it can
//  construct an engine that matches that type, it will return a cloned
//  copy.  If not, it returns a 0.
//
//  As soon as one of the Clone() calls succeeds, this function will pass
//  back a copy of the created engine.  Otherwise it will keep trying until
//  it runs out of engines to clone.  When it finally fails, it returns
//  a 0.
//
// RETURNS
//
//  Either a pointer to a newly created compression engine, or a 0 if no
//  engine could be created for the given type.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

ALCompressor AL_DLL_FAR * AL_PROTO
ALToolKit::CreateCompressor( int engine_type )  /* Tag public function */
{
    if ( !mpCompressors )
        return 0;
    for ( int i = 0 ; ; ) {
        ALCompressor *compressor;
        if ( mpCompressors[ i ] ) {
            compressor = mpCompressors[ i++ ]->Clone( engine_type );
            if ( compressor )
                return compressor;
        } else
            break;
    }
    return 0;
}

//
// NAME
//
//  ALToolKit::CreateDecompressor()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create an ALDecompressor object based on a given engine type.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALDecompressor * ALToolKit::CreateDecompressor( int engine_type );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  int engine_type :  An integer that describes the desired engine type.
//                     This is normally either an integer that was stored
//                     in an archive along with a compressed object, or
//                     the AL_COMPRESSION_DEFAULT that is used to ask for
//                     the first available engine.
//
// DESCRIPTION
//
//  This function and its two siblings constitute the whole reason for the
//  esistence of a toolkit.  When extracting objects from an archive, this
//  routine is called to build the appropriate engine.  The engine that
//  actually gets built will depend on which toolkit has been passed
//  in the ALEntryList constructor.  This means that that archiving and
//  entry list functions can operate without any knowledge about specific
//  de/compression engines, which means they won't link in any engines that
//  aren't included in your toolkit.
//
//  This function works by repeatedly trying to clone each of the engines
//  in the decompression engine list.  The Clone() function is called with
//  the engine_type argument.  If the decompression engine feels it can
//  construct a copy of itself that matches that type, it will return a cloned
//  decompression engine.  If not, it returns a 0.
//
//  As soon as one of the Clone() calls succeeds, this function will pass
//  back a copy of the created engine.  Otherwise it will keep trying until
//  it runs out of engines to clone.  When it finally fails, it returns
//  a 0.
//
// RETURNS
//
//  Either a pointer to a newly created decompression engine, or a 0 if no
//  engine could be created for the given type.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

ALDecompressor AL_DLL_FAR * AL_PROTO
ALToolKit::CreateDecompressor( int engine_type )  /* Tag public function */
{
    if ( !mpDecompressors )
        return 0;
    for ( int i = 0 ; ; ) {
        ALDecompressor *decompressor;
        if ( mpDecompressors[ i ] ) {
            decompressor = mpDecompressors[ i++ ]->Clone( engine_type );
            if ( decompressor )
                return decompressor;
        } else
            break;
    }
    return 0;
}

//
// NAME
//
//  ALToolKit::CreateStorageObject()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Create an ALStorage object based on a given object type.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALStorage * ALToolKit::CreateStorageObject( int object_type );
//
// C SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALToolkit operations aren't available in C/VB/Delphi.
//
// ARGUMENTS
//
//  int object_type :  An integer that describes the desired object type.
//                     This is normally either an integer that was stored
//                     in an archive along with a compressed object, or
//                     the AL_STORAGE_DEFAULT that is used to ask for
//                     the first available storage object.
//
// DESCRIPTION
//
//  This function and its two siblings constitute the whole reason for the
//  esistence of a toolkit.  When extracting objects from an archvie,
//  this routine is called to build the empty objects.  The object that
//  actually gets built will depend on which toolkit has been passed
//  in the ALEntryList constructor.  This means that that archiving and
//  entry list functions can operate without any knowledge about specific
//  ALStorage objec types, which means they won't link in any types that
//  aren't included in your toolkit.
//
//  This function works by repeatedly trying to clone each of the objects
//  in the storage object list.  The Clone() function is called with
//  the object_type argument.  If the storage object feels it can
//  construct a copy of itself that matches that type, it will return a cloned
//  storage object.  If not, it returns a 0.
//
//  As soon as one of the Clone() calls succeeds, this function will pass
//  back a copy of the storage object.  Otherwise it will keep trying until
//  it runs out of objects to clone.  When it finally fails, it returns
//  a 0.
//
// RETURNS
//
//  Either a pointer to a newly created storage object, or a 0 if no
//  object could be created for the given type.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

ALStorage AL_DLL_FAR * AL_PROTO
ALToolKit::CreateStorageObject( const char AL_DLL_FAR *name,  /* Tag public function */
                                int object_type )
{
    if ( !mpStorages )
        return 0;
    for ( int i = 0 ; ; ) {
        ALStorage *storage;
        if ( mpStorages[ i ] ) {
            storage = mpStorages[ i++ ]->Clone( name, object_type );
            if ( storage )
                return storage;
        } else
            break;
    }
    return 0;
}

