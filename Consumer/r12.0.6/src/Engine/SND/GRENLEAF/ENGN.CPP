//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ENGN.CPv   1.0   12 Mar 1998 12:16:12   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ENGN.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:16:12   DCHI
// Initial revision.
// 
//************************************************************************

//
// ENGN.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEngine::operator new()
//  ALEngine::ALEngine()
//  ALEngine::WriteEngineData()
//  ALEngine::ReadEngineData()
//  ALEngine::~ALEngine()
//
// DESCRIPTION
//
//  This file contains functions from the base class of ALEngine.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif

//
// NAME
//
//  ALEngine::operator new()
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
//  void * ALEngine::operator new( size_t size )
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
//           necessary to construct a new ALEngine object.
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
//  Incidentally, I suspect that this function never gets called.  If an
//  object of a derived archive class is being created, it should use
//  its own new operator, rendering this one useless.
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

#if defined( AL_BUILDING_DLL )
void AL_DLL_FAR * AL_PROTO
ALEngine::operator new( size_t size )  /* Tag internal function */
{
    return ::new char[ size ];
}
#endif

//
// NAME
//
//  ALEngine::ALEngine()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Constructor for the base class ALEngine
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALEngine::ALEngine( ALCompressionType compression_type_int,
//                      const char *compression_type_string );
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
//  compression_type_int    :  An integer used to indicate the compression
//                             type.  This needs to be selected from the
//                             enumerated type defined in ALDEFS.H.  This
//                             value is usually what gets stored in an
//                             archive to indicate what compression type was
//                             used.
//
//  compression_type_string :  A pointer to a C-style string describing the
//                             type of compression.  This isn't used in
//                             any algorithms, it's on hand as a U/I aid.
//
// DESCRIPTION
//
//  ALEngine is a base class that is a parent to both ALCompressor and
//  ALDecompressor.  It is really not much of a class, and only exists for
//  as a placeholder for a couple of data members (initialized in the ctor),
//  and a couple of virtual functions.  Since this class has a couple of
//  pure virtual functions, you can't invoke it directly.  It gets called
//  as part of the construction of the derived classes.
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
ALEngine::ALEngine( ALCompressionType compression_type_int,  /* Tag public function */
                    const char AL_DLL_FAR *compression_type_string )
    : miCompressionType( compression_type_int ),
      mszCompressionType( compression_type_string )
{
}

//
// NAME
//
//  ALEngine::WriteEngineData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Writes out private engine data.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEngine::WriteEngineData( ALStorage * archive );
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
//  archive  :  A pointer to the storage object that is usually the target of
//              the compression.  This doesn't have to be an archive,
//              so maybe the parameter has a bad name.  You could just
//              as easily be writing this out to an ALCompressedObject,
//              or even a raw binary compressed file.
//
// DESCRIPTION
//
//  Compression engines can write private data out to the archive
//  directory to provide customization information. For example, the
//  Greenleaf compression engine writes its compression level
//  using this function.  By default, there is no data, which is what
//  this function writes out, a 0 length string.
//
//  Any engine that doesn't have any customized data that needs to be
//  stored can just use the default version of this function right here.
//
// RETURNS
//
//  AL_SUCCESS, or < AL_SUCCESS if something bad happens.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

int AL_PROTO
ALEngine::WriteEngineData( ALStorage AL_DLL_FAR * archive )  /* Tag protected function */
{
    return archive->WriteGlShort( 0 );
}

//
// NAME
//
//  ALEngine::ReadEngineData()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read the customized engine data from a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALEngine::ReadEngineData( ALStorage * archive )
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
//  archive  :  A pointer to the storage object that is usually the source of
//              the decompression.  This doesn't have to be an archive,
//              so maybe the parameter has a bad name.  You could just
//              as easily be reading this in from an ALCompressedObject,
//              or even a raw binary compressed file.
//
// DESCRIPTION
//
//  Compression engines can write private data out to the archive
//  directory to provide customization information. For example, the
//  Greenleaf compression engine reads its compression level
//  using this function.  By default, no data is read  in.  This
//  function expects to find a zero length string, and complains
//  with a fatal error if it doesn't.
//
//  Any engine that doesn't have any customized data that needs to be
//  stored can just use the default version of this function right here.
//
// RETURNS
//
//  AL_SUCCESS, or < AL_SUCCESS if something bad happened.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

int AL_PROTO
ALEngine::ReadEngineData( ALStorage AL_DLL_FAR * archive )  /* Tag protected function */
{
    short temp;
    int status = archive->ReadGlShort( temp );
    AL_ASSERT( temp == 0, "ReadEngineData: engine data not 0 length" );
    return status;
}

//
// NAME
//
//  ALEngine::~ALEngine()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  The destructor for ALEngine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALEngine::~ALEngine()
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
//  This virtual destructor has virtually nothing to do.  At least in
//  the debug version of the library it checks the type of this.
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
ALEngine::~ALEngine()  /* Tag public function */
{
    AL_ASSERT( GoodTag(), "~ALEngine: attempt to delete invalid object" );
}

