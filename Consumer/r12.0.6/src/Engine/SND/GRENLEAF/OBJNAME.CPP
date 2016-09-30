//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/OBJNAME.CPv   1.0   12 Mar 1998 12:17:16   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/OBJNAME.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:16   DCHI
// Initial revision.
// 
//************************************************************************

//
// OBJNAME.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALName::operator new()
//  ALName::operator+()
//  ALName::ALName( const char *, ALCase )
//  ALName::ALName( const ALName & )
//  ALName::operator=( const char * )
//  ALName::operator = ( const ALName & )
//  ALName::~ALName()
//  ALName::ChangeExtension()
//  ALName::ChangeTrailingChar()
//  ALName::GetSafeName()
//  ALName::GetSafeOldName()
//  ALName::operator char *()
//  ALName::StripFileName()
//  ALName::StripPath()
//  ALName::WildCardMatch()
//  ALName::Strcpy()
//
// DESCRIPTION
//
//  This file contains all the source code to support class ALName.
//  Class ALName doesn't really do much outside of ALStorage, where
//  it shows up as the mName data member.  It does make cameo appearances
//  elsewhere, such as in the wild card expansion code, but those
//  are pretty limited.
//
// REVISION HISTORY
//
//  May 26, 1994  1.0A  : First release
//
//  July 7, 1994  1.0B  : Lots of petty stuff to make Sun UNIX happy.
//
//   February 14, 1996  2.0A : New Release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <string.h>
#include <ctype.h>

#include "_match.h"

//
// NAME
//
//  ALName::operator new()
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
//  void * ALName::operator new( size_t size )
//
// C SYNOPSIS
//
//  None, ALName is not exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALName is not exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  size  :  The number of bytes that the compiler has decided will be
//           necessary to construct a new ALName object.
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
ALName::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}

#endif

//
// NAME
//
//  ALName::operator+()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Add a C-style string to an ALName object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName ALName::operator+( const char *rhs )
//
// C SYNOPSIS
//
//  None, ALName isn't supported for C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALName isn't supported for C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALName isn't supported for C/VB/Delphi.
//
// ARGUMENTS
//
//  rhs  : The character pointer that is going to be added to the
//         ALName object.  This will often be the string component
//         of another ALName object, cast to type const char *.
//
// DESCRIPTION
//
//  This is one of those C++ functions that makes converts out of C
//  programmers.  It allows me to add two strings together to create
//  a third.  I really like that.
//
//  The implementation is pretty easy.  I allocate a new character buffer of
//  the correct length, then copy the two strings into it.  I use this
//  result as the initializer for a new ALName object, and return that.
//
//  Note that the new string will have the same case sensitivity as 'this'.
//
// RETURNS
//
//  A newly created ALName object.  This disappears quickly, but can
//  be copied into a result object using either the assignment operator
//  or the copy constructor.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

ALName AL_PROTO
ALName::operator+( const char AL_DLL_FAR *rhs )  /* Tag public function */
{
    int l1 = ( rhs ) ? strlen( rhs ) : 0;
    int l2 = ( mszName ) ? strlen( mszName ) : 0;
    char *p = new char[ l1 + l2 + 1 ];
    if ( p ) {
        strcpy( p, mszName );
        if ( rhs )
            strcat( p, rhs );
    }
    ALName result( p, mCase );
    if ( p )
        delete[] p;
    return result;
}


//
// NAME
//
//  ALName::ALName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Construct an ALName object from a C-style string.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName::ALName( const char *s = "",  ALCase name_case = AL_MIXED );
//
// C SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// VB SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// ARGUMENTS
//
//  s        : The initial string value of the new object.  Note that
//             you can pass a null pointer here and all will still be okay.
//
//  name_case: The case sensitivity of the new ALName object.  Will its
//             contents be mixed, or will it always be forced to upper
//             or lower?
//
// DESCRIPTION
//
//  This constructor first initializes the mCase member in an initializer.
//  mCase is a const member, which is nice, because you can make it public.
//  But, it means you can't initialize it *in* the constructor, you have
//  to do it before the body.
//
//  Things are pretty easy after that.  We allocate enough space to hold
//  the initializer string and copy it in.  The old name gets set to 0, since
//  this name hasn't been around long enough to have been renamed.
//  And that's it.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
//  This function is only used as part of
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

AL_PROTO
ALName::ALName( const char AL_DLL_FAR *s /* = "" */,  /* Tag public function */
                ALCase name_case /* = AL_MIXED */ )
     : mCase( name_case )
{
    mszName = new AL_DLL_FAR char[ strlen( s ) + 1 ];
    if ( mszName )
        Strcpy( s );
     mszOldName = 0;
}

//
// NAME
//
//  ALName::ALName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALName copy constructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName::ALName( const ALName &rhs );
//
// C SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi.
//
// ARGUMENTS
//
//  rhs   :  A reference to another ALName object.  This is the ALName
//           we are going to copy.
//
// DESCRIPTION
//
//  This is the copy constructor.  It is very nearly the same as the
//  other constructor.
//
//  This constructor first initializes the mCase member in an initializer.
//  mCase is a const member, which is nice, because you can make it public.
//  But, it means you can't initialize it *in* the constructor, you have
//  to do it before the body.
//
//  Things are pretty easy after that.  We allocate enough space to hold
//  a copy of the string in the rhs, and then copy it.  The old name gets
//  set to 0, since this name hasn't been around long enough to have been
//  renamed.  We could have copied the old name from the rhs, but I think
//  this way makes more sense.
//  And that's it.
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

AL_PROTO
ALName::ALName( const ALName AL_DLL_FAR &rhs )  /* Tag public function */
     : mCase( rhs.mCase )
{
     const char AL_DLL_FAR *s = rhs.GetSafeName();
     mszName = new AL_DLL_FAR char[ strlen( s ) + 1 ];
     if ( mszName )
          Strcpy( s );
     mszOldName = 0;
}

//
// NAME
//
//  ALName::operator=()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  ALName assignment operator, from C-style string.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName & ALName::operator=( const char * rhs );
//
// C SYNOPSIS
//
//  None, ALName is not exported to C/VB/Delphi libraries.
//
// VB SYNOPSIS
//
//  None, ALName is not exported to C/VB/Delphi libraries.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not exported to C/VB/Delphi libraries.
//
// ARGUMENTS
//
//  rhs  :  This is the character string that we are going to assign to this.
//
// DESCRIPTION
//
//  This function performs roughly the same function as the first
//  constructor, but there is a twist.  When we assign a new name to
//  an ALName object, we also make a copy of the previous value and place
//  it in the old name data member.
//
// RETURNS
//
//  A reference to 'this'.  We need to do it that way so we can do:
//
//    a = b = c = "test";
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

ALName AL_DLL_FAR & AL_PROTO
ALName::operator=( const char AL_DLL_FAR * rhs )  /* Tag public function */
{
    if ( rhs == 0 )
        rhs = "";
    if ( rhs == mszName ) // Pathological?
        return *this;

    char AL_DLL_FAR *new_name = new AL_DLL_FAR char[ strlen( rhs ) + 1 ];
    if ( new_name ) {
        if ( mszOldName )
            delete[] mszOldName;
        mszOldName = mszName;
        mszName = new_name;
        Strcpy( rhs );
    } else {
        if ( mszOldName )
            delete[] mszOldName;
        mszOldName = mszName;
        mszName = 0;
    }
    return *this;
}

//
// NAME
//
//  ALName::operator=()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Assignment operator, from ALName object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName & ALName::operator=( const ALName & rhs )
//
// C SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi.
//
// ARGUMENTS
//
//  rhs   :  The right hand side of the assignment operator.
//
// DESCRIPTION
//
//  This function is very similar to the copy constructor, but it has
//  one additional twist.  When we copy the rhs string value into our
//  string, we move our old name into the backup copy member,
//  mszOldName.  That way, if we change our mind, we can easily
//  switch back to the old name.
//
// RETURNS
//
//  A reference to 'this'.  Like in the above function, we do this so
//  that you can stack assignments:
//
//      a = b = c = "Dummy";
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

ALName AL_DLL_FAR & AL_PROTO
ALName::operator=( const ALName AL_DLL_FAR & rhs )  /* Tag public function */
{
    return *this = rhs.GetName();
}

//
// NAME
//
//  ALName::~ALName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The ALName destructor.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName::~ALName()
//
// C SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// VB SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// ARGUMENTS
//
//  None, dtor takes on args.
//
// DESCRIPTION
//
//  All this destructor has to do is free up the two pieces of
//  dynamic memory.  I just can't get out of that old C habit of
//  checking a pointer for NULL before deleting it.
//
//  In debug mode, I check the state of this before and after freeing
//  the dynamically allocated memory, in hopes of catching any
//  heap errors near their source.
//
// RETURNS
//
//  None, dtor returns nothing.
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
ALName::~ALName()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALName: attempt to delete invalid object" );
    if ( mszName )
        delete[] mszName;
    if ( mszOldName )
        delete[] mszOldName;
    AL_ASSERT( GoodTag(), "~ALName: attempt to delete invalid object" );
}

//
// NAME
//
//  ALName::ChangeExtension()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Change the extension of what is presumably a file name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName& ALName::ChangeExtension( const char *new_extension = ".bak" );
//
// C SYNOPSIS
//
//  None, ALName isn't available to C/VB/Delphi dudes.
//
// VB SYNOPSIS
//
//  None, ALName isn't available to C/VB/Delphi dudes.
//
// DELPHI SYNOPSIS
//
//  None, ALName isn't available to C/VB/Delphi dudes.
//
// ARGUMENTS
//
//  new_extension  :  The new extension you want to apply to the name.
//
// DESCRIPTION
//
//  This function is used to change the extension of a filename stored
//  in an ALName object.  A lot of times you will want to do this
//  in order to create a backup.  For example, you could change
//  TEMP.DAT to TEMP.BAK.
//
//  This function makes a copy of the current name in mszOldName, so we
//  can keep track of it later.  It then searches for the '.' character
//  in the new file name, and sticks the new extension there.
//
//  If you apply this function to a filename, you can then rename the file
//  by calling ALStorage::Rename() with no argument.  When you do this,
//  the rename function uses the saved OldName and current name as its
//  arguments for the rename() function.
//
// RETURNS
//
//  A reference to 'this'.  This is nice, because it lets you do things
//  like this:
//
//     fopen( name.ChangeExtension( ".OBJ" ), "rb" );
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
// I need to fix this code up to handle extensions with and without a
// leading '.' character.  Maybe you can use a leading character
// other than '.' to indicate a different type of extension?
//

ALName AL_DLL_FAR & AL_PROTO
ALName::ChangeExtension( const char AL_DLL_FAR *new_extension /* = ".bak" */ )  /* Tag public function */
{
    AL_ASSERT( new_extension != 0, "ChangeExtension: new extension is null" );
    AL_ASSERT( mszName, "ChangeExtension: current name is null" );

    char *file_name = new char[ strlen( mszName ) + strlen( new_extension ) + 1 ];
    if ( mszOldName )
        delete[] mszOldName;
    mszOldName = mszName;
    mszName = file_name;

    if ( !file_name )
        return *this;
    strcpy( mszName, mszOldName );
    char *p = strrchr( file_name, '.' );
    if ( p )
        strcpy( p, new_extension );
    else
        strcat( mszName, new_extension );
    switch ( mCase ) {
        case AL_UPPER : strupr( mszName ); break;
        case AL_LOWER : strlwr( mszName ); break;
//
// Note here that I don't change the case for the
// AL_MIXED_UP file system!
//
        default       : break;
    }
    return *this;
}

//
// NAME
//
//  ALName::ChangeTrailingChar()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Change the final character in what is presumably a file name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName& ALName::ChangeTrailingChar( char new_char = '@' );
//
// C SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// VB SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not visible to C/VB/Delphi programmers.
//
// ARGUMENTS
//
//  new_char  :  The new character to use as the last name of the file
//               name.
//
// DESCRIPTION
//
//  This function is used to change the the last character of the extension
//  in filename stored in an ALName object.  A lot of times you will want to
//  do this in order to create a backup.  For example, you could change
//  TEMP.DAT to TEMP.DA@.
//
//  This function makes a copy of the current name in mszOldName, so we
//  can keep track of it later.  It then searches for the end of the
//  current file name, and changes it.
//
//  Note that if the filename doesn't have an extension, we do something
//  funny.  Instead of just changing the last character, we create a 
//  new extension, and append that instead.  So if the filename is
//  "TEST", the new name will be "TEST.@".
//
//  If you apply this function to a filename, you can then rename the file
//  by calling ALStorage::Rename() with no argument.  When you do this, 
//  the rename function uses the saved OldName and current name as its
//  arguments for the rename() function.
//
// RETURNS
//
//  A reference to this.  This is nice, because it lets you do things
//  like this:
//
//     fopen( name.ChangeTrailingChar(), "rb" );
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

ALName AL_DLL_FAR & AL_PROTO
ALName::ChangeTrailingChar( char new_char /* = '@' */ )  /* Tag public function */
{
    AL_ASSERT( mszName != 0, "ChangeTrailingChar: current name is null" );

    char *file_name = new char[ strlen( mszName ) + 2 ];
    if ( mszOldName )
        delete[] mszOldName;
    mszOldName = mszName;
    mszName = file_name;
    switch ( mCase ) {
        case AL_UPPER : new_char = (char) toupper( new_char ); break;
        case AL_LOWER : new_char = (char) tolower( new_char ); break;
        default       : break;
    }
    if ( !file_name )
        return *this;
    strcpy( mszName, mszOldName );
    char *p;
    if ( ( p = strrchr( mszName, '.' ) ) != 0 ) {
        if ( p[ 1 ] == '0' ) {
            p[ 1 ] = new_char;
            p[ 2 ] = '\0';
        } else
            mszName[ strlen( mszName ) - 1 ] = new_char;
    } else {
        char new_extension[ 3 ];
        new_extension[ 0 ] ='.';
        new_extension[ 1 ] = new_char;
        new_extension[ 2 ] = 0;
        strcat( mszName, new_extension );
    }
    return *this;
}

//
// NAME
//
//  ALName::GetSafeName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Get a C-style string pointer that won't be NULL.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  const char * ALName::GetSafeName() const
//
// C SYNOPSIS
//
//  None, ALName is not visible outside of C++.
//
// VB SYNOPSIS
//
//  None, ALName is not visible outside of C++.
//
// DELPHI SYNOPSIS
//
//  None, ALName is not visible outside of C++.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is used anywhere you want to retrieve the contents of
//  the ALName object using a C-style pointer.  Since the value of
//  mszName can be NULL from time to time, you might have problems.
//  This function fixes that by guaranteeing it won't return a NULL.
//  If the value of mszName is NULL, this function instead returns a
//  pointer to a legitimate empty string.
//
// RETURNS
//
//  This function normally returns the value of mszName.  However, if
//  mszName is currently a null pointer, we return a pointer to an
//  empty string instead.  This means you can use the return value from
//  this function anywhere you want without checking for its NULLity.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

const char AL_DLL_FAR * AL_PROTO
ALName::GetSafeName() const  /* Tag public function */
{
    if ( mszName )
        return mszName;
    else
        return "";
}

//
// NAME
//
//  ALName::GetSafeOldName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Get a C-style char * to the old name, guaranteed non-NULL.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  const char * ALName::GetSafeOldName() const
//
// C SYNOPSIS
//
//  None, ALName not supported for C/VB/Delphi users.
//
// VB SYNOPSIS
//
//  None, ALName not supported for C/VB/Delphi users.
//
// DELPHI SYNOPSIS
//
//  None, ALName not supported for C/VB/Delphi users.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is used anywhere you want to print the contents of
//  the old name in the ALName object using a C-style pointer.  Since
//  the value of mszOldName can be NULL from time to time, you might have
//  problems. This function fixes that by guaranteeing it won't return a NULL.
//  If the value of mszOldName is NULL, this function instead returns a
//  pointer to a legitimate empty string.
//
// RETURNS
//
//  This function normally returns the value of mszOldName.  However, if
//  mszOldName is currently a null pointer, we return a pointer to an
//  empty string instead.  This means you can use the return value from
//  this function anywhere you want without checking for its NULLity.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

const char AL_DLL_FAR * AL_PROTO
ALName::GetSafeOldName() const  /* Tag public function */
{
    if ( mszOldName )
        return mszOldName;
    else
        return "";
}

//
// NAME
//
//  ALName::operator char *()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Cast an ALName to a char *.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName::operator char *()  const
//
// C SYNOPSIS
//
//  None, ALName is not available outside C++;
//
// VB SYNOPSIS
//
//  None, ALName is not available outside C++;
//
// DELPHI SYNOPSIS
//
//  None, ALName is not available outside C++;
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This is the casting operator that pretty much lets me use
//  an ALName object anywhere I use a char *.  There are some strange
//  Microsoft compiler problems that make me use this goofy STRINGF
//  typedef instead of char *, but it all adds up the same in
//  the wash.
//
// RETURNS
//
//  A pointer to the string inside the object.  If the string is
//  presently a null pointer, we return a pointer to an empty string
//  instead.  See ALName::GetSafeName() for an explanation of why
//  this is.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

#if defined( AL_MICROSOFT ) && ( AL_MICROSOFT < 800 ) && defined( AL_BUILDING_DLL ) //??????
AL_PROTO
ALName::operator char *()  const  /* Tag public function */
#else
AL_PROTO ALName::operator const STRINGF() const
#endif
{
    if ( mszName )
        return mszName;
    else
        return "";
}

//
// NAME
//
//  ALName::StripFileName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Strip the file name from an ALName, leaving just the path info.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName& ALName::StripFileName()
//
// C SYNOPSIS
//
//  None, can't use ALName from C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, can't use ALName from C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, can't use ALName from C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  In the wildcard parsing code, sometimes I need to get the path
//  of a file, which means stripping off the filename and extension.
//  This is pretty easy to do, I just find the right spot and stick
//  a string termination character in that position.
//
// RETURNS
//
//  A reference to this.  This is nice, because you can strip the
//  file name from an object and use it in the same operation.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//

ALName AL_DLL_FAR & AL_PROTO
ALName::StripFileName()  /* Tag public function */
{
    if ( mszName ) {
        char *p = strrchr( mszName, '\\' );
        if ( p == 0 )
            p = strrchr( mszName, ':' );
        if ( p )
            p[ 1 ] = '\0';
        else
            mszName[ 0 ] = '\0';
    }
    return *this;
}

//
// NAME
//
//  ALName::StripPath()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Strip the path component from an ALName, leaving just the file name.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALName& ALName::StripPath()
//
// C SYNOPSIS
//
//  None, can't use ALName from C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, can't use ALName from C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, can't use ALName from C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  In the wildcard parsing code, sometimes I need to get the naked
//  file name file, which means stripping off the path and drive stuff.
//  This function does just that.
//
// RETURNS
//
//  A reference to this.  This is nice, because you can strip the
//  path from an object and use it in the same operation.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New Release
//
//   March 29, 1996     2.01A : PKWare format archives use a forward
//                              slash instead of a reverse slash.  I
//                              wasn't taking that into account when
//                              stripping the path.

ALName AL_DLL_FAR & AL_PROTO
ALName::StripPath()  /* Tag public function */
{
    if ( mszName ) {
        char *p = strrchr( mszName, '\\' );
        if ( p == 0 )
            p = strrchr( mszName, '/' );
        if ( p == 0 )
            p = strrchr( mszName, ':' );
        if ( p ) {
            p++;
            char *s = mszName;
            while( ( *s++ = *p++ ) != 0 )
                ;
            *s = '\0';
        } //If not p, path is already stripped
    }
    return *this;
}

//
// NAME
//
//  ALName::WildCardMatch()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Test a name for a match against a wild card pattern.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALName::WildCardMatch( const char *pattern );
//
// C SYNOPSIS
//
//  None, ALName isn't exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALName isn't exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALName isn't exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  pattern : A pointer to a regular expression, including wildcards
//            and sets.  It can get pretty complicated.
//
// DESCRIPTION
//
//  This is a super-duper powerful function.  It is used to compare
//  a regular expression to the contents of an ALName.  The real meat
//  here is in the public domain string matching code found in _MATCH.CPP.
//
//  The tricky bit here is that we have to make a comparison based
//  on the case sensitivity of this.  Rather than trying to modify
//  the code in _MATCH.CPP, I just make a new copy of the pattern, and
//  mangle the case according to what this expects.
//
// RETURNS
//
//  0 if the pattern doesn't match the object name, 1 if it does.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New Release
//
//   March 21, 1996     2.01A : Under Windows 95, file names are case
//                              sensitive, but tests for duplicates should
//                              be done in a case-insensitive manner.  This
//                              is because the O/S preserves the case of
//                              file names, but it doesn't allow duplicates
//                              that only differ by case!
//
//   April 2, 1996      2.01A : The existing wild card matching code had
//                              a problem with "*.*".  If a file has no
//                              extension, it is stored in the archive with
//                              no '.' char.  Unfortunately, this means it
//                              won't match when compared to "*.*".  I could
//                              fix this by forcing every file name to
//                              have a "." when read in from the ZIP file or
//                              the directory, but that would be unattractive.
//                              Instead, I am modifying the wild card match
//                              routine so that the semantics of a pattern
//                              ending in ".*" is changed to "*".
int AL_PROTO
ALName::WildCardMatch( const char AL_DLL_FAR *pattern )  /* Tag public function */
{
    int error;
    int result;
    char *p1 = new char[ strlen( pattern ) + 1 ];
    if ( !p1 )
        return 0;
    char *p2 = new char[ strlen( mszName ) + 1 ];
    if ( !p2 ) {
        delete[] p1;
        return 0;
    }
    strcpy( p1, pattern );
    strcpy( p2, mszName );

    switch ( mCase ) {
        case AL_UPPER    : strupr( p1 ); break;
        case AL_LOWER    : strlwr( p1 ); break;
        case AL_MIXED_UP : strlwr( p1 ); strlwr( p2 ); break;
        default          : break;
    }
    int l = strlen( p1 );
    if ( l >= 3 && p1[ l - 1 ] == '*' && p1[ l - 2 ] == '.' ) {
        p1[ l - 2 ] = '*';
        p1[ l - 1 ] = '\0';
    }
    if ( !is_valid_pattern( p1, &error ) )
        result = 0;
    else if ( matche( p1, p2 ) == MATCH_VALID )
        result = 1;
    else
        result = 0;
    if ( p1 )
        delete[] p1;
    if ( p2 )
        delete[] p2;
    return result;
}

//
// NAME
//
//  ALName::Strcpy()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Copy a string while obeying upper/lower case characteristics.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALName::Strcpy( const char *s );
//
// C SYNOPSIS
//
//  None, can't use ALName in C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, can't use ALName in C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, can't use ALName in C/VB/Delphi.
//
// ARGUMENTS
//
//  s  :  A C-style character string to be copied into mszName.
//
// DESCRIPTION
//
//  Whenever I am going to copy a string into mszName, I need to convert
//  it to the case that this object expects.  If it is AL_UPPER or
//  AL_LOWER, that means copying and then converting to either all upper
//  case or all lower case.  I do that here.
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
ALName::Strcpy( const char AL_DLL_FAR *s )  /* Tag protected function */
{
    strcpy( mszName, s );
     switch ( mCase ) {
        case AL_UPPER : strupr( mszName ); break;
        case AL_LOWER : strlwr( mszName ); break;
//
// Note that I preserve case for both AL_MIXED *and*
// AL_MIXED_UP!
//
        default       : break;
     }
}


