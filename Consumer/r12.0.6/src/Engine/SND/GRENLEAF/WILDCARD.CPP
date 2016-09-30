//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/WILDCARD.CPv   1.0   12 Mar 1998 12:17:40   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/WILDCARD.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:40   DCHI
// Initial revision.
// 
//************************************************************************

//
// WILDCARD.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALWildCardExpander::operator new()
//  ALWildCardExpander::ALWildCardExpander()
//  newALExpander()
//  newALExpanderWithDelimiters()
//  ALWildCardExpander::IsDelimiter()
//  ALWildCardExpander::~ALWildCardExpander()
//  deleteALExpander()
//  ALWildCardExpander::GetNextWildName()
//
// DESCRIPTION
//
//  This file contains most of the source code for the base class
//  ALWildCardExpander.  The wild card expansion code is a state
//  driven routine, which keeps track of its entire state between
//  calls.  So you can call it once to get a new file name, then
//  do some processing.  When you call it again later, you will
//  get the next file name in the sequence.
//
//  The way the wild card class handles searching through subdirectories
//  is by keeping a link pointer to a subdirectory search.  When it
//  is time to open up a subdirectory search, we create a new file
//  expander, and assign its pointer to our link pointer.  As long as
//  the link is active, we keep searching there.  When the link runs
//  out of files to return, we continue searching in our own directory.
//
//  A lot of this code is easier to deal with because we use the
//  ALName class.  That makes it easy to strip file names and
//  paths apart, and even easier to put them back together again.
//
//  Note that the system specific portions of the wildcard expansion
//  class are isolated in separate directories, e.g. CPP_FS.NT\*.CPP
//
// REVISION HISTORY
//
//  May 26, 1994  1.0A  : First release
//
//  July 7, 1994  1.0B  : Added support for UNIX, which meant creating a
//                        completely new version of ALGetNextFile.
//
//   February 14, 1996  2.0A : New release.
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

#include <stdlib.h>
#include "wildcard.h"

//
// NAME
//
//  ALWildCardExpander::operator new()
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
//  #include "wildcard.h"
//
//  void * ALWildCardExpander::operator new( size_t size )
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
//           necessary to construct a new ALWildCardExpander object.
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
ALWildCardExpander::operator new( size_t size )  /* Tag protected function */
{
    return ::new char[ size ];
}
#endif

//
// NAME
//
//  ALWildCardExpander::ALWildCardExpander()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an ALWildCardExpander object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  ALWildCardExpander::ALWildCardExpander( const char *wild_file_list,
//                                          int traverse_flag = 0,
//                                          ALCase name_case = AL_LOWER );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  hALExpander newALExpander( char *wild_file_list,
//                             int traverse_flag,
//                             ALCase name_case );
//
// VB SYNOPSIS
//
//  Declare Function newALExpander Lib "AL20LW"
//    (ByVal wild_file_list$, ByVal traverse_flag%, ByVal name_case%) As Long
//
// DELPHI SYNOPSIS
//
//  function newALExpander( wild_file_list : String;
//                          traverse_flag : Integer;
//                          name_case : Integer ) : hALExpander;
//
// ARGUMENTS
//
//  wild_file_list  : A list of wild card file specifications, separated
//                    by commas, semicolons, or spaces, maybe looking
//                    something like this: "*.CPP, BOB.DAT, *.*"
//
//  traverse_flag   : A flag that indicates whether you want to traverse
//                    all subdirectories under the current path.  Note that
//                    under C++, this defaults to a 0.  No such convenience
//                    for C/VB/Delphi.
//
//  name_case       : An indicator of whether you want all the returned
//                    file names forced to a certain case.  Note that
//                    under C++, this defaults to AL_LOWER (usually).  No
//                    such convenience for C/VB/Delphi.
//
// DESCRIPTION
//
//  The constructor for the expander has to set up a bunch of data members
//  that will all be used during the expansion process.  The mCase
//  member is easy to understand.  All of the objname objects that
//  we create are going to be force to a certain case by this
//  using this data member.  miTraverseFlag is just our copy of the
//  input parameter.  And the mState variable keeps track of what we
//  are doing in between function calls.  We set it to GET_NEXT_WILD_NAME,
//  which means we will be doing that the first time we get called.
//
//  mInputLine is where we keep a copy of the list of wild card file
//  specifications passed by the calling program.  Each time we take
//  a new file name out of mInputLine, we remove it from the ALName
//  object, making mInputLine just a little shorter.
//
//  The mResultFileName member is the storage area where we keep a copy
//  of the file name created by the expander.  This is our local copy,
//  when it gets returned to the calling program they need to make
//  their own copy of it and leave ours alone.
//
//  Every time we get asked to get a new file, the very first thing
//  we do is check to see if the mpNextExpander member is pointing
//  to a new expander object.  If it is, we ask him to provide
//  the next file name, instead of giving it ourselves.  When he
//  doesn't have any file names left to give, we destroy him and
//  set that pointer back to 0.  Here in the constructor, the smart
//  thing to do is set him to 0 for starters.
//
//  mpcDelimiters is a short string that is used to define all the
//  characters that can be file name delimiters.  They are used to
//  separate file names in the input line.  By default, the delimiters
//  are set to be space, tab, or commas.  Note that there is a second
//  constructor for this object that lets users specify their own delimiter
//  objects.
//
//  Note that every OS has different ways to keep track of a wild card
//  expansion.  Rather than try to deal with that in here, I set it up
//  by calling the member function ConstructOsData().  Various versions of
//  this function will be linked in depending on which library is
//  being built.
//
//
// RETURNS
//
//  A newly constructed ALWildCardExpander object.  If you are calling
//  this function from C/VB/Delphi, you are going to get a handle that
//  is synonymous with a pointer to this object.
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
ALWildCardExpander::ALWildCardExpander(  /* Tag public function */
          const char AL_DLL_FAR *wild_file_list,
          int traverse_flag /* = 0 */,
          ALCase name_case /* = AL_LOWER */ )
    : mCase( name_case ),
      mResultFileName( "", name_case )
{
    mInputLine = wild_file_list;
    mState = GET_NEXT_WILD_NAME;
    mpNextExpander = 0;
    miTraverseFlag = traverse_flag;
    mpcDelimiters = " ,\t";
    ConstructOsData();
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALExpander AL_FUNCTION
newALExpander( char AL_DLL_FAR *wild_file_list,  /* Tag public function */
               int traverse_flag,
               ALCase name_case )
{
    ALWildCardExpander *expander;

    expander = new ALWildCardExpander( wild_file_list, traverse_flag, name_case );
    return (hALExpander) expander;
}

#endif

//
// NAME
//
//  ALWildCardExpander::ALWildCardExpander()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Create an ALWildCardExpander object, with specific file name delimiters.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  ALWildCardExpander::ALWildCardExpander( const char *wild_file_list,
//                                          const char *delimiters,
//                                          int traverse_flag = 0,
//                                          ALCase name_case = AL_LOWER );
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  hALExpander newALExpanderWithDelimiters( char *wild_file_list,
//                                           char *delimiters,
//                                           int traverse_flag,
//                                           ALCase name_case );
//
// VB SYNOPSIS
//
//  Declare Function newALExpanderWithDelimiters Lib "AL20LW"
//    (ByVal wild_file_list$,
//     ByVal delimiters$
//     ByVal traverse_flag%,
//     ByVal name_case%) As Long
//
// DELPHI SYNOPSIS
//
//  function newALExpanderWithDelimiters( wild_file_list : String;
//                                        delimiters : String;
//                                        traverse_flag : Integer;
//                                        name_case : Integer ) : hALExpander;
//
// ARGUMENTS
//
//  All arguments are the same as the previous constructor, except:
//
//  delimiters:  A C-style character string that contains a list of
//               characters that are used to delimit file names.  Specify
//               this argument to use delimiters other than the defaults.
//
// DESCRIPTION
//
//  This constructor is identical to the previous constructor, except for
//  the addition of the delimiters argument.  This argument lets you specify
//  delimiters other than the default for parsing your list of input file
//  names.
//
// RETURNS
//
//  A newly constructed ALWildCardExpander object.  If you are calling
//  this function from C/VB/Delphi, you are going to get a handle that
//  is synonymous with a pointer to this object.
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
ALWildCardExpander::ALWildCardExpander(  /* Tag public function */
          const char AL_DLL_FAR *file_list,
          const char AL_DLL_FAR *delimiters,
          int traverse_flag /* = 0 */,
          ALCase name_case /* = AL_LOWER */ )
    : mCase( name_case ),
      mResultFileName( "", name_case )
{
    mInputLine = file_list;
    mState = GET_NEXT_WILD_NAME;
    mpNextExpander = 0;
    miTraverseFlag = traverse_flag;
    mpcDelimiters = delimiters;
    ConstructOsData();
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE hALExpander AL_FUNCTION
newALExpanderWithDelimiters( char AL_DLL_FAR *wild_file_list,  /* Tag public function */
                             char AL_DLL_FAR *delimiters,
                             int traverse_flag,
                             ALCase name_case )
{
    ALWildCardExpander *expander;

    expander = new ALWildCardExpander( wild_file_list,
                                       delimiters,
                                       traverse_flag,
                                       name_case );
    return (hALExpander) expander;
}

#endif

//
// NAME
//
//  ALWildCardExpander::IsDelimiter()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Check to see if a character is a delimiter.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  int ALWildCardExpander::IsDelimiter( char c );
//
// C SYNOPSIS
//
//  None, this is an internal protected function.
//
// VB SYNOPSIS
//
//  None, this is an internal protected function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal protected function.
//
// ARGUMENTS
//
//  c  :  Internally, each character in the list of input wild names is
//        checked using this function.  If the function returns a true
//        value, it means the character is a delimiter, and we need to
//        terminate the current file name.
//
// DESCRIPTION
//
//  This function checks to see if a character is a delimiter by simply
//  looking through the list of delimiters, one by one, and seeing if
//  any of the inputs match.
//
// RETURNS
//
//  True or false.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//

int AL_PROTO
ALWildCardExpander::IsDelimiter( char c )  /* Tag protected function */
{
    const char *p = mpcDelimiters;

    while ( *p )
        if ( *p++ == c )
            return 1;
    return 0;
}

//
// NAME
//
//  ALWildCardExpander::~ALWildCardExpander()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  The destructor for the ALWildCardExpander.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  ALWildCardExpander::~ALWildCardExpander();
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  void deleteALExpander( hALExpander this_object );
//
// VB SYNOPSIS
//
//  Declare Sub deleteALExpander Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure deleteALExpander( this_object : hALExpander );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALWildCardExpander that
//                  is going to be destroyed.  Note that the C++ member
//                  function version of this call doesn't have an explicit
//                  argument here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  There are a couple of big deals we need to worry about in the
//  destructor an ALWildCardExpander.  First, we have to worry about
//  any additional handlers we created to search subdirectories.  If
//  this destructor is being called before our search is done, we
//  may have some of those expander objects just hanging around out
//  there.  We take care of the by checking the mpNextExpander member.
//  If it isn't set to 0, we delete the dynamically created expander.
//
//  Second, we have to worry about closing up any OS resources that were
//  created to implement the expander.  We used to do that in the code
//  right here, but that got tedious, so now it all gets moved to the
//  DestroyOsData() function.  Which one of those gets linked in all depends
//  on which version of the library is being built.
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
ALWildCardExpander::~ALWildCardExpander()  /* Tag public function */
{
    if ( mpNextExpander )
        delete mpNextExpander;
    if ( mpOsData )
        DestroyOsData();
}

#if !defined( AL_NO_C )

extern "C" AL_LINKAGE void AL_FUNCTION
deleteALExpander( hALExpander this_object )  /* Tag public function */
{
    AL_ASSERT_OBJECT( this_object, ALWildCardExpander, "deleteALExpander" );
    delete (ALWildCardExpander *) this_object;
}

#endif

//
// NAME
//
//  ALWildCardExpander::GetNextWildName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Get the next wild name from the input list of wild names.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "wildcard.h"
//
//  int ALWildCardExpander::GetNextWildName();
//
// C SYNOPSIS
//
//  None, this is an internal protected function.
//
// VB SYNOPSIS
//
//  None, this is an internal protected function.
//
// DELPHI SYNOPSIS
//
//  None, this is an internal protected function.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  This function is called internally to get the next file spec out of
//  the input line.  This is simply a matter of parsing past all the
//  delimiter characters.  The resulting file spec is stored in
//  data member mFullWildName.  That member will be the one used to
//  kick off the next wild card search.
//
// RETURNS
//
//  1 if it got a new file spec, 0 if it didn't.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release.
//
//   March 25, 1996     2.01A : Added support for quote marks, so you
//                              can specify filenames with spaces. Just
//                              put double quotes around the name!
//
int AL_PROTO
ALWildCardExpander::GetNextWildName()  /* Tag protected function */
{
    char wild_spec[ _MAX_PATH ];
    int i = 0;
    char AL_DLL_FAR *p = mInputLine;
    int eating_quote = 0;

    for ( ; ; p++ ) {
#if defined( AL_WIN32 )
        if ( *p == '\"' ) {
            eating_quote = 1;
            p++;
            break;
        }
#endif
        if ( !IsDelimiter( *p ) )
            break;
    }
    for ( ; ; p++ ) {
        if ( *p == '\0' )
            break;
        if ( !eating_quote && IsDelimiter( *p ) )
            break;
#if defined( AL_WIN32 )
        if ( eating_quote && *p == '\"' ) {
            p++;
            break;
        }
#endif
        wild_spec[ i++ ] = (char) *p;
        if ( i >= ( _MAX_PATH - 2 ) )
            return 0;
    }
    wild_spec[ i++ ] = '\0';
    if ( i <= 1 )
         return 0;
    mFullWildName = wild_spec;
    mInputLine = p;
    return 1;
}

