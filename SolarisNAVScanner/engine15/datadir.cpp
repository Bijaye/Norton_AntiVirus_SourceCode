// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/DATADIR.CPv   1.0   28 Jul 1997 17:25:08   DCHI  $
//
// Description:
//  Source file for DataDirAppendName().
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/DATADIR.CPv  $
// 
//    Rev 1.0   28 Jul 1997 17:25:08   DCHI
// Initial revision.
// 
//************************************************************************

#include <string.h> 		// NEW_UNIX

#include "platform.h"
#include "callback.h"

#include "navexshr.h"

#include "datadir.h"

//********************************************************************
//
// Function:
//  void DataDirAppendName()
//
// Parameters:
//  lptstrDataDir       Data directory string
//  lptstrFileName      File name to append
//  lptstrResult        Buffer for result
//
// Description:
//  If the data directory name does not end in a backslash,
//  the function adds one before appending the file name.
//  Otherwise, the filename is just appended to the data directory
//  buffer.
//
// Returns:
//  Nothing
//
//********************************************************************

void DataDirAppendName
(
    LPTSTR  lptstrDataDir,
    LPTSTR  lptstrFileName,
    LPTSTR  lptstrResult
)
{
    LPTSTR  lptstr;
    LPTSTR  lptstrPrevChar;

    lptstrPrevChar = NULL;
    lptstr = lptstrDataDir;
    while (*lptstr != '\0')
    {
        lptstrPrevChar = lptstr;
        lptstr = AnsiNext(lptstr);
    }

    // Copy the directory

    STRCPY(lptstrResult,lptstrDataDir);

    // If the string is empty or if the last character is not \,
    // then add a backslash

#ifdef SYM_UNIX							// NEW_UNIX
    if (lptstrPrevChar == NULL || *lptstrPrevChar != '/')	// NEW_UNIX
#else								// NEW_UNIX
    if (lptstrPrevChar == NULL || *lptstrPrevChar != '\\')
#endif								// NEW_UNIX
    {
        // Append a backslash

#if defined(SYM_NTK)
        STRCAT(lptstrResult,L"\\");
#elif defined(SYM_UNIX)						// NEW_UNIX
	STRCAT(lptstrResult,"/");				// NEW_UNIX
#else
        STRCAT(lptstrResult,"\\");
#endif
    }

    STRCAT(lptstrResult,lptstrFileName);
}

