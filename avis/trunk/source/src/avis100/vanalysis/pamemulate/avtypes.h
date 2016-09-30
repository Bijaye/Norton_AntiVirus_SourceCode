// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/avtypes.h_v   1.7   20 Nov 1996 15:23:48   AOONWAL  $
//
// Description:
//  AVAPI 2.0 essential constant and type definitions.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/avtypes.h_v  $
// 
//    Rev 1.7   20 Nov 1996 15:23:48   AOONWAL
// No change.
// 
//    Rev 1.6   28 Oct 1996 14:31:28   CNACHEN
// Added LPLPSTR defintion.
// 
//    Rev 1.5   28 Oct 1996 14:25:14   DJesch
// Resolved some redefinition problems when including PLATFORM.H
//
//    Rev 1.4   22 Oct 1996 12:15:14   AOONWAL
// No change.
//
//    Rev 1.3   12 Aug 1996 17:43:12   DCHI
// Modifications for compilation on UNIX.
//
//    Rev 1.2   27 Jun 1996 18:04:24   DCHI
// Modifications for stand-alone compilation under VC20.
//
//    Rev 1.1   10 Jun 1996 10:44:26   RAY
// Added linear open modes
//
//    Rev 1.0   04 Jun 1996 12:57:10   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _AVTYPES_H

#define _AVTYPES_H

#if defined(VC20)

#include <windows.h>

#endif

#if !defined(BORLAND) && !defined(VC20) && !defined(SYM_UNIX)

#include "platform.h"

#else

#include <stdio.h>
#include <memory.h>

#if defined(SYM_MAX_PATH)
#undef SYM_MAX_PATH         // force our definition of this constant
#endif
#define SYM_MAX_PATH	512

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int    UINT;
typedef unsigned long	DWORD;
typedef int             BOOL;
typedef WORD            *LPWORD;
typedef int             *LPINT;
typedef long            LONG;
typedef char            *LPSTR;
typedef char            *LPTSTR;
typedef BYTE            *LPBYTE;
typedef void            *LPVOID;
typedef DWORD			*LPDWORD;
typedef BOOL            *LPBOOL;
typedef long            *LPLONG;
typedef LPSTR           *LPLPSTR;

#if !defined(VC20)

typedef FILE *          HFILE;
typedef char            *LPWSTR;
typedef void            VOID;

#ifdef SYM_UNIX
#define FAR
#else
#define FAR             far
#endif

#endif

#if !defined(MEMSET)
#define MEMSET          memset
#endif
#if !defined(MEMCPY)
#define MEMCPY          memcpy
#endif
#if !defined(MEMCMP)
#define MEMCMP          memcmp
#endif

#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2

#define TRUE            1
#define FALSE           0

#endif

typedef long            MUTEX_T;
typedef MUTEX_T         *LPMUTEX;

#define MEM_FIXME       0
#define DATE_TIME_FIXME 0

#ifndef FILE_OPEN_READ_ONLY
#define FILE_OPEN_READ_ONLY     0       // read only (file must exist)
#define FILE_OPEN_WRITE_ONLY    1       // truncate/create
#define FILE_OPEN_READ_WRITE    2       // read/write (file must exist)
#endif

#ifndef LINEAR_OPEN_READ_ONLY
#define LINEAR_OPEN_READ_ONLY   0       // read only  (for scanning)
#define LINEAR_OPEN_READ_WRITE  2       // read/write (for repair)
#endif


typedef LPBYTE          *LPLPBYTE;
typedef LPVOID          *LPLPVOID;
typedef LPWSTR          *LPLPWSTR;

#endif  // _AVTYPES_H

