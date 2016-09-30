// Copyright 1996-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/navwork.h_v   1.3   16 Mar 1998 19:00:12   MKEATIN  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/navwork.h_v  $
// 
//    Rev 1.3   16 Mar 1998 19:00:12   MKEATIN
// Made changes for the new decomposer
// 
//    Rev 1.2   03 Jul 1997 15:01:50   TIVANOV
// fix custom notify responce
//
//    Rev 1.1   25 Feb 1997 14:43:34   DALLEE
// Moved GetPhysicalDriveLetter() prototype here from stub.h
//************************************************************************

#ifndef _NAVWORK_H
#define _NAVWORK_H

#include "nonav.h"
                                        // Structure used by EnumFixedDrives
typedef struct
{
    char cDriveLetter;                  // Logical drive letter
    BYTE byPhysicalDrive;               // Physical drive number (80h, 81h
    BOOL bDriveLocked;                  // TRUE - We have the lock

} DRIVELIST, * LPDRIVELIST;


VOID    UpdateScanStatistics(HWND hDlg);

VOID ZipNameAppendChildToParent (
    LPSTR lpParent,
    LPSTR lpChild,
    int   ncDest );

VOID WINAPI ValidateAndSaveFileInfo(
    LPNONAV     lpNav,                  // NAV Structure
    LPCTSTR     lpFileName,             // Long name of the file.
    LPCTSTR     lpShortName,            // if (bIsFullName) SFN of file.
                                        // if (!bIsFullName) Ansi file name
    BOOL        bIsFullName             // TRUE if scanning a single file
    );

BOOL FileInExeList (              // Checks if the file's extension is
    LPCTSTR     lpExtensions,           // a one that is in the list of
    LPCTSTR     lpFile                  // program file extensions.
    );


#ifdef SYM_MIRRORS
    // Per Mirrors docs, always call Yield before PeekMessage()
    // NavPeekMessage found in n_scan.c.

    BOOL NavPeekMessage(MSG FAR* a, HWND b, UINT c, UINT d, UINT e);
#else
    #define NavPeekMessage PeekMessage
#endif

                                        // N_MEMSCN.CPP
BOOL EnumFixedDrives
(
    LPDRIVELIST lpDriveList             // [out] List of hard drives
);


                                        // Returns the target drive letter
                                        // for a SUBST'd drive so we can scan
                                        // the boot record correctly.
BYTE GetPhysicalDriveLetter( char bDrive );

BOOL IsCutomNotify(LPNONAV lpNav);
#endif // _NAVWORK_H
