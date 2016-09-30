// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/filefind.h_v   1.0   27 May 1997 14:45:54   CNACHEN  $
//
// Description:
//  Prototypes for volume and directory searches and traversals.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/HEUR/VCS/filefind.h_v  $
// 
//    Rev 1.0   27 May 1997 14:45:54   CNACHEN
// Initial revision.
// 
//    Rev 1.2   12 Aug 1996 18:34:48   DCHI
// Modifications for UNIX.
// 
//    Rev 1.1   03 Jun 1996 17:21:16   DCHI
// Changes to support building in BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.0   21 May 1996 12:26:54   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _FILEFIND_H

#define _FILEFIND_H

#if defined(BORLAND) || defined(SYM_DOS) || defined(SYM_DOSX)
#include <dos.h>
#endif // BORLAND || SYM_DOS || SYM_DOSX

#if defined(VC20) || defined(SYM_WIN32)
#include <io.h>
#endif // VC20 || SYM_WIN32

#if defined(SYM_UNIX)
#include <dirent.h>
#endif // SYM_UNIX

#ifdef BORLAND
#define FF_ATTRIB_NORMAL        FA_NORMAL
#define FF_ATTRIB_RDONLY        FA_RDONLY
#define FF_ATTRIB_HIDDEN        FA_HIDDEN
#define FF_ATTRIB_SYSTEM        FA_SYSTEM
#define FF_ATTRIB_LABEL         FA_LABEL
#define FF_ATTRIB_DIREC         FA_DIREC
#define FF_ATTRIB_ARCH          FA_ARCH
#endif // BORLAND

#if defined(VC20) || defined(SYM_WIN32) || \
    defined(SYM_DOS) || defined(SYM_DOSX)

#define FF_ATTRIB_NORMAL        _A_NORMAL
#define FF_ATTRIB_RDONLY        _A_RDONLY
#define FF_ATTRIB_HIDDEN        _A_HIDDEN
#define FF_ATTRIB_SYSTEM        _A_SYSTEM
#define FF_ATTRIB_LABEL         0x08
#define FF_ATTRIB_DIREC         _A_SUBDIR
#define FF_ATTRIB_ARCH          _A_ARCH

#endif // VC20 || SYM_WIN32 || SYM_DOS || SYM_DOSX

#ifdef SYM_UNIX
#define FF_ATTRIB_NORMAL        0x00
#define FF_ATTRIB_RDONLY        0x01
#define FF_ATTRIB_HIDDEN        0x02
#define FF_ATTRIB_SYSTEM        0x04
#define FF_ATTRIB_LABEL         0x08
#define FF_ATTRIB_DIREC         0x10
#define FF_ATTRIB_ARCH          0x20
#endif

#define FF_MAX_NAME_LEN         SYM_MAX_PATH

typedef struct
{
    DWORD       dwFileAttributes;
    LPSTR       szFileName;

#if defined(BORLAND) || defined(SYM_DOS) || defined(SYM_DOSX)
    struct find_t       stFindData;
#endif // BORLAND || SYM_DOS || SYM_DOSX

#if defined(VC20) || defined(SYM_WIN32)
    long                lFindHandle;
    struct _finddata_t  stFindData;
#endif // VC20 || SYM_WIN32

#if defined(SYM_UNIX)
    DIR *               pDir;
    struct dirent *     pDirEnt;
    char                szFileSpec[512];
    int                 nAfterSlashIndex;
    char                szDirPath[1024];
#endif // SYM_UNIX

} FILEFINDDATA_T, FAR *LPFILEFINDDATA;

BOOL FindFirstFile
(
    LPSTR           lpstrFileSpec,
    LPFILEFINDDATA  lpFileFindData
);

BOOL FindNextFile
(
    LPFILEFINDDATA  lpFileFindData
);

void CloseFileFind
(
    LPFILEFINDDATA lpFileFindData
);

BOOL ChangeVolume
(
    LPSTR       lpstrNewVolume
);

BOOL ChangeDirectory
(
    LPSTR       lpstrNewDirectory
);

BOOL ChangeFullVolDir
(
    LPSTR       lpstrNewFullVolDir
);

BOOL GetCurrentDirectory
(
    LPSTR       lpstrCurrentDirectory
);

#endif  // _FILEFIND_H

