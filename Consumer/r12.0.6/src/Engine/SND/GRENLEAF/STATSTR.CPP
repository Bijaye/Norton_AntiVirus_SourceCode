//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/STATSTR.CPv   1.0   12 Mar 1998 12:17:46   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/STATSTR.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:17:46   DCHI
// Initial revision.
// 
//************************************************************************

//
// STATSTR.CPP
//
//  Source file for ArchiveLib 2.0
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStatus::GetStatusString()
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
//  ALStatus::GetStatusString()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Get the ASCII name for an error code.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  const char * ALStatus::GetStatusString()
//
// C SYNOPSIS
//
//  None, ALStatus isn't exported to C/VB/Delphi.
//
// VB SYNOPSIS
//
//  None, ALStatus isn't exported to C/VB/Delphi.
//
// DELPHI SYNOPSIS
//
//  None, ALStatus isn't exported to C/VB/Delphi.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  Rather than just printing an error code number, it is usually more
//  helpful to translate that number into ASCII text, so a user or
//  programmer can read the description.  This function is used to
//  do just that.  It translates the current error code into a short
//  ASCII text string.  Note that this is not the same as the detail
//  string, which is tailored for each specific occurrence of an error code.
//
// RETURNS
//
//  A pointer to a string containing a short ASCII translation of
//  the current error code.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

const char AL_DLL_FAR * AL_PROTO
ALStatus::GetStatusString()  /* Tag public function */
{
    switch ( miStatus ) {
        case AL_SUCCESS                   : return "Success";
        case AL_END_OF_FILE               : return "End of file";
        case AL_CANT_OPEN_BUFFER          : return "Can't allocate buffer";
        case AL_CANT_CREATE_ENGINE        : return "Can't create compression engine";
        case AL_CANT_CREATE_STORAGE_OBJECT: return "Can't create storage object";
        case AL_CANT_ALLOCATE_MEMORY      : return "Memory allocation failure";
        case AL_RENAME_ERROR              : return "Error renaming file";
        case AL_CANT_OPEN_FILE            : return "Can't open file";
        case AL_SEEK_ERROR                : return "Seek error";
        case AL_READ_ERROR                : return "Read error";
        case AL_WRITE_ERROR               : return "Write error";
        case AL_DELETE_ERROR              : return "File deletion error";
        case AL_ILLEGAL_PARAMETER         : return "Illegal parameter";
        case AL_INTERNAL_ERROR            : return "Internal error";
        case AL_USER_ABORT                : return "User abort";
        case AL_SERVER_NOT_PRESENT        : return "Server not present";
        case AL_COMPRESSION_TYPE_MISMATCH : return "Mismatch in compression type";
        case AL_NEED_LENGTH               : return "Missing length parameter";
        case AL_CRC_ERROR                 : return "CRC Error";
        case AL_COMPARE_ERROR             : return "Comparison error";
        case AL_UNKNOWN_COMPRESSION_TYPE  : return "Unknown compression type";
        case AL_UNKNOWN_STORAGE_OBJECT    : return "Unknown type of storage object";
        case AL_INVALID_ARCHIVE           : return "Invalid archive";
        case AL_LOGIC_ERROR               : return "Logic error";
        case AL_BACKUP_FAILURE            : return "Could not create backup";
        case AL_GETSEL_ERROR              : return "Error getting selections from list box";
        case AL_DUPLICATE_ENTRY           : return "Duplicate entry";
        default                           : return "Unknown error";
    }
}

