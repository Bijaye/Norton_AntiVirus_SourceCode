// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/navstat.h_v   1.0   06 Feb 1997 20:51:44   RFULLER  $
//
// Description:
//      Dynamically load S32STAT.DLL and function pointers.  Provide
//      SYMSTAT covers to other NAV Dll's so we don't statically link
//      to SYMSTAT.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/include/VCS/navstat.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:44   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:10   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:34   BARRY
// Initial revision.
// 
//    Rev 1.0   15 Nov 1995 21:43:18   DALLEE
// Initial revision.
//************************************************************************

#ifndef __NAVSTAT_H
#define __NAVSTAT_H

#ifdef __cplusplus
extern "C" {
#endif


// Error codes for NavStatInit() and NavStatEnd()

// NOERR or...
#define ERR_NAVSTAT_DLL_NOT_FOUND       (STATUS) 1
#define ERR_NAVSTAT_FUNCTION_NOT_FOUND  (STATUS) 2
#define ERR_NAVSTAT_DLL_NOT_FREED       (STATUS) 3


// Function Prototypes all W32

#ifdef SYM_WIN32

STATUS SYM_EXPORT WINAPI NavStatInit ();

STATUS SYM_EXPORT WINAPI NavStatEnd ();

STATUS SYM_EXPORT WINAPI NavStatAllocInstance (LPCSTR   lpProject);

STATUS SYM_EXPORT WINAPI NavStatFreeInstance (LPCSTR    lpProject);

HWND   SYM_EXPORT WINAPI NavStatGetInstanceHwnd (LPCSTR lpProject);

STATUS SYM_EXPORT WINAPI NavStatSetInstanceHwnd (LPCSTR lpProject,
                                                 HWND   hwndProject);

BOOL   SYM_EXPORT WINAPI NavStatGetLastRunDateTime (LPCSTR      lpProduct,
                                                    LPCSTR      lpProject,
                                                    BYTE        bDL,
                                                    SYSTEMTIME  *pst);

BOOL   SYM_EXPORT WINAPI NavStatSetLastRunDateTime (LPCSTR  lpProduct,
                                                    LPCSTR  lpProject,
                                                    BYTE    bDL);

DWORD  SYM_EXPORT WINAPI NavStatGetDriveState (BYTE     bDl,
                                               UINT     uEntry,
                                               LPSTR    lpTranslatedName,
                                               UINT     nMaxAppName);

STATUS SYM_EXPORT WINAPI NavStatSetDriveState (BYTE     bDl,
                                               UINT     uEntry,
                                               DWORD    dwState,
                                               LPCSTR   lpTranslatedName);

void   SYM_EXPORT WINAPI NavStatDisplayAlreadyOwnedError (HWND      hWndParent,
                                                          char      cDrive,
                                                          LPCSTR    lpszTitle);

#endif  // SYM_WIN32

#ifdef __cplusplus
}
#endif

#endif  // __NAVSTAT_H
