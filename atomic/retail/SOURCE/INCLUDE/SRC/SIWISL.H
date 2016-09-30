//***********************************************************************
//  Copyright (c)1995 - Symantec, Peter Norton Computing Group
//***********************************************************************
//
//  $Header:   S:/INCLUDE/VCS/SIWISL.H_v   1.0   30 Jan 1996 15:47:30   BARRY  
//
//   SIWISL.H - Header file for IShellLink DLL
//
//***********************************************************************
/*  $Log:   S:/include/VCS/siwisl.h_v  $ */
// 
//    Rev 1.0   06 Feb 1997 20:51:20   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:30   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:54   BARRY
// Initial revision.
// 
//    Rev 1.0   25 Apr 1995 17:41:56   KEITH
// Initial revision.
// 
//    Rev 1.7   31 Mar 1995 04:20:26   MDUVALL
// Added SetForegroundWindow() thunk
// 
//    Rev 1.6   14 Mar 1995 17:09:56   MDUVALL
// Changed win16 prototypes for create to take nShowCommand parameter.
// 
//    Rev 1.5   14 Mar 1995 15:49:16   MDUVALL
// Added show command parameter on create
// 
//    Rev 1.4   10 Mar 1995 10:45:38   BRUCEH
// Fixed thunk layer bugs.
// 
// 
//    Rev 1.3   09 Mar 1995 12:49:52   BRUCEH
// Took out definitions of SYM_EXPORT since we're now including platform.h
// 
//    Rev 1.2   08 Mar 1995 17:47:04   BRUCEH
// Added support for win16 version
//
//    Rev 1.1   08 Mar 1995 16:08:34   MDUVALL
// Made exported functions extern "C" to prevent name mangling
//
//
//    Rev 1.0   08 Mar 1995 15:09:22   MDUVALL
// Initial revision.
//***********************************************************************

#ifndef _SIWISL_H_
#define _SIWISL_H_

#define IDS_NUCAPTION 100

#ifdef SYM_WIN32

extern "C"
BOOL _stdcall thk_ThunkConnect32(LPSTR pszDll16, LPSTR pszDll32,
                                 HINSTANCE hInst, DWORD dwReason);
                                 
extern "C"
LONG SYM_EXPORT WINAPI BringToFront(  );

extern "C"
LONG SYM_EXPORT WINAPI ShellLinkCreate(
    LPCSTR lpFolder,
    LPCSTR lpCmdLine,
    LPCSTR lpArguments,
    LPCSTR lpName,
    LPCSTR lpIconPath,
    int nIconIndex,
    int nShowCommand,
    LPCSTR lpDefDir );

extern "C"
LONG SYM_EXPORT WINAPI ShellLinkDestroy (
    LPCSTR lpFolder,
    LPCSTR lpCmdLine,
    LPCSTR lpName);

extern "C"
LONG SYM_EXPORT WINAPI ShellLinkExists (
    LPCSTR lpFolder,
    LPCSTR lpCmdLine,
    LPCSTR lpName);

#else                                   // SYM_WIN32

extern "C"
LONG SYM_EXPORT WINAPI BringToFront(  );

extern "C"
LONG SYM_EXPORT WINAPI BringToFront16(  );

extern "C"
LONG WINAPI ShellLinkCreate(
    LPCSTR lpFolder,
    LPCSTR lpCmdLine,
    LPCSTR lpArguments,
    LPCSTR lpName,
    LPCSTR lpIconPath,
    int nIconIndex,
    int nShowCommand,
    LPCSTR lpDefDir );

extern "C"
LONG WINAPI ShellLinkDestroy(
    LPCSTR lpFolder,
    LPCSTR lpCmdLine,
    LPCSTR lpName);

extern "C"
LONG WINAPI ShellLinkExists(
    LPCSTR lpFolder,
    LPCSTR lpCmdLine,
    LPCSTR lpName);


extern "C"
LONG SYM_EXPORT WINAPI ShellLinkCreate16(LPCSTR lpFolder, 
                                         LPCSTR lpCmdLine,
                                         LPCSTR lpArguments, 
                                         LPCSTR lpName,
                                         LPCSTR lpIconPath, 
                                         int nIconIndex,
                                         int nShowCommand,
                                         LPCSTR lpDefDir);

extern "C"
LONG SYM_EXPORT WINAPI ShellLinkDestroy16(LPCSTR lpFolder, LPCSTR lpCmdLine,
                                          LPCSTR lpName);

extern "C"
LONG SYM_EXPORT WINAPI ShellLinkExists16(LPCSTR lpFolder, LPCSTR lpCmdLine,
                                         LPCSTR lpName);

#endif                                  // SYM_WIN32


#endif
