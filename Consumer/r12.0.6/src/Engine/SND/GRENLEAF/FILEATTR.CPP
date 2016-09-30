//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/FILEATTR.CPv   1.0   12 Mar 1998 12:16:12   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/FILEATTR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:12   DCHI
// Initial revision.
// 
//************************************************************************

//
// FILEATTR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALFileAttributes::operator new()
//  ALFileAttributes::ALFileAttributes()
//  ALFileAttributes::~ALFileAttributes()
//  ALFileAttributes::SetFromPackedAttributes()
//  ALStorageSetFromPackedAtts()
//  ALFileAttributes::PackedAttributes()
//  ALStoragePackedAttributes()
//
// DESCRIPTION
//
//  This file contains all of the source to support the ALFileAttributes
//  class.  Even though this is in theory a standalone, independent class,
//  it really has no life of its own.  It always exists as a data
//  member embedded in ALStorage.  The only reason these functions aren't
//  member functions of ALStorage was simply to modularize things a bit.
//  As it is, it makes pretty good sense as a class of its own.
//
// REVISION HISTORY
//
//  May 25, 1994  1.0A   : First release
//
//  July 7, 1994  1.0B   : I had to #ifdef out a few DOS related
//                         pieces of code for the UNIX version
//                         Note that we don't have support yet for
//                         setting file attributes under UNIX.
//
//  August 10, 1994 1.0B : Added the 9 UNIX file attributes, plus an
//                         additional flag to indicate whether the
//                         the current protection bits are set in UNIX
//                         or DOS mode.  I don't try to translate
//                         DOS protection bits to UNIX or vice-versa,
//                         it just seems too unscientific.
//
//  February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include "fileattr.h"

//
// NAME
//
//  ALFileAttributes::operator new()
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
//  void * ALFileAttributes::operator new( size_t size )
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
//           necessary to construct a new ALFileAttributes object.
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
//  Incidentally, I suspect that this function never gets called.  This
//  object usually only gets instantiated as a member of ALStorage, which
//  means it is probably going to use memory already located in another
//  object.
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
//  February 14, 1996  2.0A : New release
//


#if defined( AL_BUILDING_DLL )
void AL_DLL_FAR * AL_PROTO
ALFileAttributes::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}
#endif

//
// NAME
//
//  ALFileAttributes::ALFileAttributes()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Construct a new ALFileAttributes object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALFileAttributes::ALFileAttributes()
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
//  size         :  The number of bytes necessary to construct a new
//                  object.
//
// DESCRIPTION
//
//  This is the only constructor for class ALFileAttributes.  It does
//  nothing more than set all of the bits to 0.  I was thinking that
//  it might be a good idea to set them to -1 instead, indicating that
//  they are presently in an unknown state.  I would want to set them
//  to 0 when I called Create(), and read them in when I call Open().
//  Just thinking out loud.
//
//  C, VB, and Delphi programmers don't ever get to construct an
//  ALFileAttributes object on their own.  As far as they are concerned,
//  time and date attributes are part of ALStorage, and they us ALStorage
//  translation functions to access them.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
//  None, these objects are only constructed as members of ALStorage.
//
// SEE ALSO
//
// REVISION HISTORY
//
//  February 14, 1996  2.0A : New release
//

AL_PROTO
ALFileAttributes::ALFileAttributes()  /* Tag public function */
{
    miReadOnly        = 0;
    miSystem          = 0;
    miHidden          = 0;
    miArchive         = 0;
    miUnixBitsPresent = 0;
    miUserRead        = 0;
    miUserWrite       = 0;
    miUserExecute     = 0;
    miOtherRead       = 0;
    miOtherWrite      = 0;
    miOtherExecute    = 0;
    miGroupRead       = 0;
    miGroupWrite      = 0;
    miGroupExecute    = 0;
    miDirectory       = 0;
}

//
// NAME
//
//  ALFileAttributes::~ALFileAttributes()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Destroy a file attribute object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALFileAttributes::~ALFileAttributes();
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
//  None.
//
// DESCRIPTION
//
//  The destructor doesn't have to worry about freeing any dynamic
//  storage or anything like that, so it gets to do a great big nothing.
//
// RETURNS
//
//  Nothing, dtors don't have any return values.
//
// EXAMPLE
//
//  None, these objects are only constructed as members of ALStorage.
//
// SEE ALSO
//
// REVISION HISTORY
//
//  February 14, 1996  2.0A : New release
//

AL_PROTO
ALFileAttributes::~ALFileAttributes()  /* Tag public function */
{
}

//
// NAME
//
//  ALFileAttributes::SetFromPackedAttributes()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the ALFileAttributes member from an external packed int.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALFileAttributes::SetFromPackedAttributes( short int attributes )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALStorageSetFromPackedAtts( hALStorage this_object,
//                                   unsigned short int packed_attributes );
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageSetFromPackedAtts Lib "AL20LW"
//    (ByVal this_object&, ByVal packed_attributes%)
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageSetFromPackedAtts( this_object : hALStorage;
//                                        packed_attributes : Integer );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to have its file attributes reset.  This is
//                  a little bit sneaky, because the C/VB/Delphi guys
//                  don't even have an ALFileAttributes class defined.
//                  Instead, their translation functions make it look as if
//                  they are getting the attributes directly from a storage
//                  object.  So the C++ version of this function uses
//                  the mFileAttributes member of ALStorage, and calls
//                  this member function directly.
//
//  packed_attributes  :
//                A set of attributes that have been packed into a
//                short int using our internal bit ordering:
//
//                Label Dir gx gw gr ox ow or ux uw ur X A H S R
//
//                The three group and three other privilege bits
//                aren't used in this carnation of ArchiveLib.  They
//                are available in case we ever decide to run this
//                under UNIX. (Which we did in 1.0).
//
// DESCRIPTION
//
//  The packed format is how we actually store file attributes in an
//  archive.  When we read in the directory from an archive, we can
//  use this function to apply the packed attribute bits to a storage
//  object.  All it has to do is set the 16 bits internal to the
//  class.
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
//  February 14, 1996  2.0A : New release
//

void AL_PROTO
ALFileAttributes::SetFromPackedAttributes( short int attributes )  /* Tag public function */
{
    miReadOnly        = ( attributes & ATTR_READ_ONLY )         != 0;
    miSystem          = ( attributes & ATTR_SYSTEM )            != 0;
    miHidden          = ( attributes & ATTR_HIDDEN )            != 0;
    miArchive         = ( attributes & ATTR_ARCHIVE )           != 0;
    miUnixBitsPresent = ( attributes & ATTR_UNIX_BITS_PRESENT ) != 0;
    miUserRead        = ( attributes & ATTR_USER_READ )         != 0;
    miUserWrite       = ( attributes & ATTR_USER_WRITE )        != 0;
    miUserExecute     = ( attributes & ATTR_USER_EXECUTE )      != 0;
    miOtherRead       = ( attributes & ATTR_OTHER_READ )        != 0;
    miOtherWrite      = ( attributes & ATTR_OTHER_WRITE )       != 0;
    miOtherExecute    = ( attributes & ATTR_OTHER_EXECUTE )     != 0;
    miGroupRead       = ( attributes & ATTR_GROUP_READ )        != 0;
    miGroupWrite      = ( attributes & ATTR_GROUP_WRITE )       != 0;
    miGroupExecute    = ( attributes & ATTR_GROUP_EXECUTE )     != 0;
    miDirectory       = ( attributes & ATTR_DIRECTORY )         != 0;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
ALStorageSetFromPackedAtts( hALStorage this_object,  /* Tag public function */
                            unsigned short int packed_attributes )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetFromPackedAtts" );
    ( (ALStorage *) this_object )->mAttributes.SetFromPackedAttributes( packed_attributes );
}

#endif

//
// NAME
//
//  ALFileAttributes::PackedAttributes()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read the attributes in packed format.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  unsigned short int ALFileAttributes::PackedAttributes()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  unsigned short int ALStoragePackedAttributes( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStoragePackedAttributes Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStoragePackedAttributes( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object whose
//                  attributes are going to be read.  This is
//                  a little bit sneaky, but we have to do it this way
//                  because the C/VB/Delphi guys don't even have an
//                  ALFileAttributes class defined. Instead, their translation
//                  functions make it look as if they are reading the
//                  attributes directly from a storage object.  The C++
//                  version of this function uses the mFileAttributes member
//                  of ALStorage, and calls this member function directly.
//
// DESCRIPTION
//
//  When it comes time to write an archive directory out to disk, we need
//  to store the file attributes in a consistent format.  This format
//  is our internal packed attribute format.  You can get the file attributes
//  in this packed attribute format with a call to this function.
//
// RETURNS
//
//  A set of attributes that have been packed into a short int using our
//  internal bit ordering:
//
//   Label Dir gx gw gr ox ow or ux uw ur X A H S R
//
//   The three group and three other privilege bits aren't used in this
//   carnation of ArchiveLib.  They are available in case we ever decide to
//   run this pup under UNIX. (Which we did in 1.0, but don't do now :( ).
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

unsigned short int AL_PROTO
ALFileAttributes::PackedAttributes()  /* Tag public function */
{
    int result = 0;

    result |= miReadOnly        ? ATTR_READ_ONLY         : 0;
    result |= miSystem          ? ATTR_SYSTEM            : 0;
    result |= miHidden          ? ATTR_HIDDEN            : 0;
    result |= miArchive         ? ATTR_ARCHIVE           : 0;
    result |= miUnixBitsPresent ? ATTR_UNIX_BITS_PRESENT : 0;
    result |= miUserRead        ? ATTR_USER_READ         : 0;
    result |= miUserWrite       ? ATTR_USER_WRITE        : 0;
    result |= miUserExecute     ? ATTR_USER_EXECUTE      : 0;
    result |= miOtherRead       ? ATTR_OTHER_READ        : 0;
    result |= miOtherWrite      ? ATTR_OTHER_WRITE       : 0;
    result |= miOtherExecute    ? ATTR_OTHER_EXECUTE     : 0;
    result |= miGroupRead       ? ATTR_GROUP_READ        : 0;
    result |= miGroupWrite      ? ATTR_GROUP_WRITE       : 0;
    result |= miGroupExecute    ? ATTR_GROUP_EXECUTE     : 0;
    result |= miDirectory       ? ATTR_DIRECTORY         : 0;
    return (unsigned short int ) result;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE unsigned short int AL_FUNCTION
ALStoragePackedAttributes( hALStorage this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStoragePackedAttributes" );
    return ( (ALStorage *) this_object)->mAttributes.PackedAttributes();
}

#endif

// OBSOLETE FUNCTION, but I'm leaving it here "just in case"
//
// void ALFileAttributes::SetFromUnixAttributes( mode_t attributes )
//
// ARGUMENTS:
//
//  attributes  : The set of file attributes as defined for UNIX.
//                These are the attributes you get back from the
//                stat() function call.
//
// RETURNS
//
//  Nothing, a void function.
//
// DESCRIPTION
//
//  When the ALFile class opens a file under UNIX, it reads in
//  the file attributes with a call to stat().  It can then store
//  those file attributes in the ALFileAttributes member of ALStorage
//  by calling this function.
//
//  Converting the data to our internal format is simply a matter of picking
//  bits out of the mode_t word.
//
// REVISION HISTORY
//
//   August 10, 1994 1.0B : First released.
//
//

#if defined( AL_UNIX ) && 0

void AL_PROTO
ALFileAttributes::SetFromUnixAttributes( mode_t mode )  /* Tag obsolete function */
{
    miUnixBitsPresent = 1;
    miUserRead        = ( mode & S_IRUSR ) ? 1 : 0;
    miUserWrite       = ( mode & S_IWUSR ) ? 1 : 0;
    miUserExecute     = ( mode & S_IXUSR ) ? 1 : 0;
    miOtherRead       = ( mode & S_IROTH ) ? 1 : 0;
    miOtherWrite      = ( mode & S_IWOTH ) ? 1 : 0;
    miOtherExecute    = ( mode & S_IXOTH ) ? 1 : 0;
    miGroupRead       = ( mode & S_IRGRP ) ? 1 : 0;
    miGroupWrite      = ( mode & S_IWGRP ) ? 1 : 0;
    miGroupExecute    = ( mode & S_IXGRP ) ? 1 : 0;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
ALStorageSetFromUnixAttributes( hALStorage this_object,  /* Tag obsolete function */
                                mode_t attributes )
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageSetFromUnixAttributes" );
    ( (ALStorage *) this_object)->mAttributes.SetFromUnixAttributes( attributes );
}

#endif

#endif

// OBSOLETE FUNCTION, but I'm leaving it here "just in case.  This needs
// to be added to the header files if it's ever going to go anywhere.
//
// mode_t ALFileAttributes::GetUnixAttributes()
//
// ARGUMENTS:
//
//  None.
//
// RETURNS
//
//  The set of file attributes, packed into the order that UNIX
//  expects them.
//
// DESCRIPTION
//
//  When we are closing a file that we have to set the time stamp and
//  attributes for, this function is used to get the attributes.
//  All it needs to do is repack some bits into the order that UNIX expects.
//
//  Note that if the attribute is presently stored in DOS format, this
//  function just returns the default protection mask.
//
// REVISION HISTORY
//
//   August 10, 1994 1.0B : First released.
//

#if defined( AL_UNIX ) && 0

mode_t AL_PROTO
ALFileAttributes::GetUnixAttributes()  /* Tag obsolete function */
{
    mode_t mode = 0;
    if ( !miUnixBitsPresent ) {
        mode = umask( 0 );
        umask( mode );
    } else {
        mode |= miUserRead ? S_IRUSR : 0;
        mode |= miUserWrite ? S_IWUSR : 0;
        mode |= miUserExecute ? S_IXUSR : 0;
        mode |= miOtherRead ? S_IROTH : 0;
        mode |= miOtherWrite ? S_IWOTH : 0;
        mode |= miOtherExecute ? S_IXOTH : 0;
        mode |= miGroupRead ? S_IRGRP : 0;
        mode |= miGroupWrite ? S_IWGRP : 0;
        mode |= miGroupExecute ? S_IXGRP : 0;
    }
    return mode;
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE mode_t AL_FUNCTION
ALStorageGetUnixAttributes( hALStorage this_object )  /* Tag obsolete function */
{
    AL_ASSERT_OBJECT( this_object, ALStorage, "ALStorageGetUnixAttributes" );
    return ( (ALStorage *) this_object )->mAttributes.GetUnixAttributes();
}

#endif

#endif

