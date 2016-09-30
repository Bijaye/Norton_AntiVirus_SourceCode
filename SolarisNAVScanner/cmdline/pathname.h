// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/pathname.h_v   1.2   12 Aug 1996 18:34:54   DCHI  $
//
// Description:
//  Prototypes for functions dealing with pathnames.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/pathname.h_v  $
// 
//    Rev 1.2   12 Aug 1996 18:34:54   DCHI
// Modifications for UNIX.
// 
//    Rev 1.1   03 Jun 1996 17:21:34   DCHI
// Changes to support building in BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.0   21 May 1996 12:27:30   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _PATHNAME_H

#define _PATHNAME_H

#ifdef BORLAND
#include <dir.h>
#endif // BORLAND

#include <stdlib.h>

#if defined(SYM_UNIX)
#define PATHNAME_MAX_VOLUME         1
#define PATHNAME_MAX_DIR            512
#define PATHNAME_MAX_NAME           256
#define PATHNAME_MAX_EXT            256
#else
#define PATHNAME_MAX_VOLUME         _MAX_DRIVE
#define PATHNAME_MAX_DIR            _MAX_DIR
#define PATHNAME_MAX_NAME           _MAX_FNAME
#define PATHNAME_MAX_EXT            _MAX_EXT
#endif

#if defined(VC20) || defined(SYM_WIN32) || \
    defined(SYM_DOS) || defined(SYM_DOSX) || defined(SYM_UNIX)

#define DRIVE                       0x10
#define DIRECTORY                   0x08
#define FILENAME                    0x04
#define EXTENSION                   0x02

#endif // VC20 || SYM_WIN32 || SYM_DOS || SYM_DOSX

#define PATHNAME_HAS_VOLUME         DRIVE
#define PATHNAME_HAS_DIR            DIRECTORY
#define PATHNAME_HAS_NAME           FILENAME
#define PATHNAME_HAS_EXT            EXTENSION

#if defined(SYM_UNIX)
#define DIR_SEPARATOR   '/'
#else
#define DIR_SEPARATOR   '\\'
#endif

void PathNameGetName
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrName
);

BOOL PathNameHasWildCard
(
    LPSTR           lpstrPathName
);

BOOL PathNameIsDirectory
(
    LPSTR           lpstrPathName
);

BOOL PathNameIsVolume
(
    LPSTR           lpstrPathName
);

void PathNameChangeExtension
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrNewExtension
);

int PathNameSplit
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrVolume,
    LPSTR           lpstrDir,
    LPSTR           lpstrName,
    LPSTR           lpstrExt
);

void PathNameMerge
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrVolume,
    LPSTR           lpstrDir,
    LPSTR           lpstrName,
    LPSTR           lpstrExt
);

#endif  // _PATHNAME_H

