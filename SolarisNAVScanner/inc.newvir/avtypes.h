// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/AVTYPES.H_v   1.11   15 Oct 1998 14:43:58   AOONWAL  $
//
// Description:
//  AVAPI 2.0 essential constant and type definitions.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/AVTYPES.H_v  $
// 
//    Rev 1.11   15 Oct 1998 14:43:58   AOONWAL
// Changed #ifdef (SYM_UNIX) to #ifdef SYM_UNIX
// 
//    Rev 1.10   28 Sep 1998 18:12:26   ddrew
// Changes so that we can sync up Solaris port
// 
//    Rev 1.11   28 Sep 1998 17:03:06   ddrew
// Original back in for now
// 
//    Rev 1.7   20 Jul 1998 15:54:42   MKEATIN
// Changed more *'s to far *'s.
// 
//    Rev 1.6   20 Jul 1998 15:49:08   MKEATIN
// declared some *'s to far *'s.
// 
//    Rev 1.5   02 May 1997 16:40:20   MKEATIN
// Added defines for SEEK_CUR, SEEK_SET, and SEEK_END if not defined.
// 
//    Rev 1.4   18 Apr 1997 13:43:46   MKEATIN
// Latest header from NEWVIR/INCLUDE
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

#if defined(VC20) && !defined(SYM_UNIX)
    #include <windows.h>
#endif

#if !defined(BORLAND) && !defined(VC20) && !defined(SYM_UNIX)
    #include "platform.h"
#else

    #include <stdio.h>
    #include <memory.h>
    #include <string.h>

    #if defined(SYM_MAX_PATH)
	    #undef SYM_MAX_PATH         // force our definition of this constant
    #endif

    #define SYM_MAX_PATH        512

    #if defined(SYM_UNIX)
        #define FAR 
        #define HUGE
    #else
        #if !defined(FAR)
            #define FAR far
        #endif
        #if !defined(HUGE)
            #define HUGE huge
        #endif
    #endif

    typedef unsigned char   BYTE;
    typedef unsigned short  WORD;
    typedef unsigned int    UINT;
    typedef unsigned long   DWORD;
    typedef int             BOOL;
    typedef WORD            FAR *LPWORD;
    typedef int             FAR *LPINT;
    typedef long            LONG;
    typedef char            FAR *LPSTR;
    typedef char            FAR *LPTSTR;
    typedef BYTE            FAR *LPBYTE;
    typedef void            FAR *LPVOID;
    typedef DWORD           FAR *LPDWORD;
    typedef BOOL            FAR *LPBOOL;
    typedef long            FAR *LPLONG;
    typedef LPSTR           FAR *LPLPSTR;

    #if defined(SYM_UNIX)
        typedef FILE            *HFILE;
    #endif

    #if !defined(VC20) && !defined(SYM_UNIX)
        typedef FILE            *HFILE;
        typedef char            *LPWSTR;
        typedef void            VOID;
    #endif

    #if defined(SYM_UNIX)
        typedef WORD            *PWORD;             
        typedef signed short    SWORD;
        typedef signed long     SDWORD;
        typedef unsigned int    *LPUINT;
        typedef const char      *LPCSTR;
        typedef const char      *LPCTSTR;
        typedef char            WCHAR;
        typedef char            TCHAR;
        typedef char            *LPWSTR;
        typedef char            **LPLPWSTR;
        #define VOID            void
        #define WINAPI
        #define _T(x)           x
    #endif

#endif // !defined(BORLAND) && !defined(VC20) && !defined(SYM_UNIX)


#if !defined(MEMSET)
    #define MEMSET          memset
#endif
#if !defined(MEMCPY)
    #define MEMCPY          memcpy
#endif
#if !defined(MEMCMP)
    #define MEMCMP          memcmp
#endif
#if !defined(STRCPY)
    #define STRCPY          strcpy
#endif
#if !defined(STRCAT)
    #define STRCAT          strcat
#endif
#if !defined(STRLEN)
    #define STRLEN          strlen
#endif
#if !defined(STRNCPY)
    #define STRNCPY         strncpy
#endif
    
#if !defined(TRUE)
    #define TRUE            1
    #define FALSE           0
#endif

#if !defined(SEEK_SET)
    #define SEEK_SET        0
#endif
#if !defined(SEEK_CUR)
    #define SEEK_CUR        1
#endif
#if !defined(SEEK_END)
    #define SEEK_END        2
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

#if !defined(SYM_UNIX)
    typedef HANDLE FILEHANDLE;
#endif
#define INVALID_FILEHANDLE_VALUE INVALID_HANDLE_VALUE;

typedef LPBYTE  FAR *LPLPBYTE;
typedef LPVOID  FAR *LPLPVOID;
typedef LPWSTR  FAR *LPLPWSTR;

#endif  // _AVTYPES_H (from the top)



