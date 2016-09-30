// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/fileinfo.h_v   1.3   27 Jun 1996 13:13:52   DCHI  $
//
// Description:
//  File information structure.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/fileinfo.h_v  $
// 
//    Rev 1.3   27 Jun 1996 13:13:52   DCHI
// Use HANDLE for hFile on WIN32 platforms.
// 
//    Rev 1.2   04 Jun 1996 18:18:20   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.1   03 Jun 1996 17:21:24   DCHI
// Changes to support building in BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.0   21 May 1996 12:27:12   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _FILEINFO_H

#define _FILEINFO_H

#include <stdio.h>

#include "avtypes.h"

#if defined(SYM_UNIX)

// use the FILEINFO_T type from avcb.h
typedef FILEINFO_T* LPFILEINFO;

#else

typedef struct
{
#if defined(VC20) || defined(SYM_WIN32) || defined(SYM_UNIX)
    HFILE   hFile;
#else
    FILE *  hFile;
#endif

    char    lpstrPathName[SYM_MAX_PATH+1];
} FILEINFO_T, FAR *LPFILEINFO;

#endif

#endif  // _FILEINFO_H
