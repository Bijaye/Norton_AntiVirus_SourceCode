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

#if !defined(BORLAND) && !defined(VC20) && !defined(SYM_UNIX) || defined(SYM_S390)
    #include "platform.h"
#else

    #include <ctype.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #if defined(FAR)
        #undef FAR
    #endif

    #if defined(HUGE)
        #undef HUGE
    #endif

    #if defined(SYM_UNIX)
        #define FAR
        #define HUGE
    #else
        #define FAR             far
        #define HUGE            huge
    #endif  // (SYM_UNIX)

    #if defined(WINAPI)
        #undef WINAPI
    #endif
    #define WINAPI

    #if defined(CALLBACK)
        #undef CALLBACK
    #endif
    #define CALLBACK

    #if defined(SYM_EXPORT)
        #undef SYM_EXPORT
    #endif
    #define SYM_EXPORT

    #if defined(SYM_MAX_PATH)
        #undef SYM_MAX_PATH         // force our definition of this constant
    #endif

    #define SYM_MAX_PATH	512
 
    typedef void                VOID;
    typedef VOID FAR *          LPVOID;
    typedef LPVOID FAR *        LPLPVOID;

    typedef unsigned char       BYTE;
    typedef BYTE FAR *          LPBYTE;
    typedef LPBYTE FAR *        LPLPBYTE;

    typedef unsigned short      WORD;
    typedef WORD FAR *          LPWORD;
    typedef LPWORD FAR *        LPLPWORD;

    typedef signed short        SWORD;
    typedef SWORD FAR *         LPSWORD;
    typedef LPSWORD FAR *       LPLPSWORD;

    typedef unsigned int        UINT;
    typedef UINT FAR *          LPUINT;

    typedef unsigned long       DWORD;
    typedef DWORD FAR *         LPDWORD;
    typedef LPDWORD FAR *       LPLPDWORD;

    typedef signed long         SDWORD;
    typedef SDWORD FAR *        LPSDWORD;
    typedef LPSDWORD FAR *      LPLPSDWORD;

    typedef int                 BOOL;
    typedef BOOL FAR *          LPBOOL;

    typedef int FAR *           LPINT;

    typedef long                LONG;
    typedef long FAR *          LPLONG;

    typedef char FAR *          LPSTR;
    typedef const char FAR *    LPCSTR;
    typedef LPSTR FAR *         LPLPSTR;

    typedef char                TCHAR;
    typedef TCHAR FAR *         LPTSTR;
    typedef const TCHAR FAR *   LPCTSTR;
    typedef LPTSTR FAR *        LPLPTSTR;
    
    typedef unsigned short      wchar_t;
    typedef wchar_t             WCHAR;
    typedef WCHAR FAR *         LPWSTR;
    typedef const WCHAR FAR *   LPCWSTR;
    typedef LPWSTR FAR *        LPLPWSTR;

    #define _T(quote)           quote

    #ifdef SYM_UNIX
        typedef int             HFILE;
        #define HFILE_ERROR     ((HFILE)-1)
    #endif

    typedef void (CALLBACK *FARPROC)(void);

    #if !defined(VC20)
        #if !defined(SYM_UNIX)
            typedef FILE *      HFILE;
            typedef char *      LPWSTR;
            typedef void        VOID;
        #endif

    #endif  // (VC20)

    #if !defined(STRCAT)
        #define STRCAT          strcat
    #endif
    #if !defined(STRCPY)
        #define STRCPY          strcpy
    #endif
    #if !defined(STRNCPY)
        #define STRNCPY         strncpy
    #endif
    #if !defined(STRCMP)
        #define STRCMP          strcmp
    #endif
    #if !defined(STRLEN)
        #define STRLEN          strlen
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

    #define TRUE            1
    #define FALSE           0

    typedef int                 HINSTANCE;
    typedef long                LPARAM;

    #define DLL_PROCESS_DETACH  0
    #define DLL_PROCESS_ATTACH  1

#endif // !defined(BORLAND) && !defined(VC20) && !defined(SYM_UNIX)

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
typedef MUTEX_T FAR *   LPMUTEX;

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

#if !defined (SYM_UNIX)
    typedef LPBYTE  far *LPLPBYTE;
    typedef LPVOID  far *LPLPVOID;
    typedef LPWSTR  far *LPLPWSTR;
    typedef HANDLE FILEHANDLE;
    #define INVALID_FILEHANDLE_VALUE INVALID_HANDLE_VALUE;
#endif  // (SYM_UNIX)

#endif  // _AVTYPES_H (from the top)

