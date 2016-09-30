// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/datadir.cpv   1.0   28 Jul 1997 16:15:24   DCHI  $
//
// Description:
//  Source file for DataDirAppendName().
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/datadir.cpv  $
// 
//    Rev 1.0   28 Jul 1997 16:15:24   DCHI
// Initial revision.
// 
//************************************************************************

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

#if defined(SYM_UNIX)

    if (lptstrPrevChar == NULL || *lptstrPrevChar != '/')
        STRCAT(lptstrResult, "/");

#else
    if (lptstrPrevChar == NULL || *lptstrPrevChar != '\\')
    {
        // Append a backslash

#if defined(SYM_NTK)
        STRCAT(lptstrResult,L"\\");
#else
        STRCAT(lptstrResult,"\\");
#endif
    }

#endif

    STRCAT(lptstrResult,lptstrFileName);
}

