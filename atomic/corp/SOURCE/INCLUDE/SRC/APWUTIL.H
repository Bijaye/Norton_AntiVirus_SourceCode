// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/apwutil.h_v   1.3   13 Mar 1998 17:32:44   tcashin  $
//
// Description:
//      Header for NAVAPW utility functions.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/apwutil.h_v  $
// 
//    Rev 1.3   13 Mar 1998 17:32:44   tcashin
// Added ApwActivateNavw, ApwGetCurrentNavWindow and ApwGetNavwExePath.
// 
//    Rev 1.2   11 Aug 1997 17:04:56   DALLEE
// Added ApwUpdateUI() for service to notify UI to resync the displayed
// status of the driver.
// 
//    Rev 1.1   11 Jun 1997 12:05:40   DALLEE
// Added GetWindowsPlatform(), ApwNotifyNavw().
// 
//    Rev 1.0   02 Jun 1997 02:46:54   DALLEE
// Initial revision.
//************************************************************************

#ifndef __APWUTIL_H
#define __APWUTIL_H

#include "pshpack1.h"
  #include "navbnhdr.h"
  #include "navopts.h"
  #include "options.h"
#include "poppack.h"

// BUILDING_APWUTIL must be defined and only defined when building
// the APWUTIL project.

#ifdef  BUILDING_APWUTIL
  #define APWUTILAPI __declspec( dllexport )
#else
  #define APWUTILAPI __declspec( dllimport )
#endif

//************************************************************************
// Data types
//************************************************************************

typedef struct tagAPWMESSAGEINFO {      // Data to pass to APWMessage().
    HWND        hParent;                // Parent for message box.
    HINSTANCE   hInstance;              // Module handle for string resources.
    UINT        uMessage;               // String ID of message template.
    UINT        uTitle;                 // Title ID (optional).
    union { LPTSTR lpszFileName;        // First replacement string.
            LPTSTR lpszString1; };      //   (usually filename)
    union { LPTSTR lpszVirusName;       // Second replacement string.
            LPTSTR lpszString2; };      //   (usually virus name)
    union { LPTSTR lpszZipName;         // Third replacement string.
            LPTSTR lpszString3; };      //   (usually archive name)
    LPTSTR      lpszAlert;              // Alert message text.
    BOOL        bBogon;
} APWMESSAGEINFO, *PAPWMESSAGEINFO;


//************************************************************************
// Defines

#ifndef EOS
    #define EOS _T('\0')
#endif


//************************************************************************
// Prototypes
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

APWUTILAPI BOOL WINAPI ApwDlgSet3d ( HWND hWnd );
APWUTILAPI BOOL WINAPI ShellTrayPresent ();
APWUTILAPI BOOL WINAPI NewShellPresent ();
APWUTILAPI DWORD WINAPI GetWindowsPlatform ();
APWUTILAPI BOOL WINAPI ApwActivate ();
APWUTILAPI BOOL WINAPI ApwShutdown ();
APWUTILAPI BOOL WINAPI ApwMinimize ();
APWUTILAPI BOOL WINAPI ApwCenterWindow ( HWND hWnd, HWND hParent );
APWUTILAPI BOOL WINAPI ApwMessage ( APWMESSAGEINFO *pMsgInfo );
APWUTILAPI BOOL WINAPI ApwReadNavopts ( _NAVOPTIONS *pNavopts );
APWUTILAPI BOOL WINAPI ApwNotifyNavw ( DWORD dwMessage );
APWUTILAPI BOOL WINAPI ApwUpdateUI ();
APWUTILAPI BOOL WINAPI ApwActivateNavw ();
APWUTILAPI BOOL WINAPI ApwGetCurrentNavWindow (LPHANDLE lpMainWindow, LPHANDLE lpCurrentWindow);
APWUTILAPI void WINAPI ApwGetNavwExePath ( LPSTR lpszNavw, LPSTR lpszDir );

APWUTILAPI LPTSTR WINAPI ApwNameStrip( LPTSTR lpszPath );
APWUTILAPI LPTSTR WINAPI ApwNameAppendSeparator( LPTSTR lpszPath );
APWUTILAPI LPTSTR WINAPI ApwNameCat( LPTSTR lpszPath, LPCTSTR lpszFileName );
APWUTILAPI LPTSTR WINAPI ApwNameSkipVolume( LPCTSTR lpszPath );
APWUTILAPI LPTSTR WINAPI ApwNameSkipDirectory( LPCTSTR lpszPath );
APWUTILAPI LPTSTR WINAPI ApwNameSkipSection( LPCTSTR lpszPath );
APWUTILAPI LPTSTR WINAPI ApwNameToLower ( LPTSTR lpszFile );
APWUTILAPI BOOL   WINAPI ApwNameMatchSection ( LPCTSTR lpszWild, LPCTSTR lpszFile );

APWUTILAPI DWORD WINAPI GetLongPathName( LPCTSTR   lpszShort, 
                                         LPTSTR    lpszLong, 
                                         DWORD     cchBuffer );

#ifdef __cplusplus
}
#endif

#endif // __APWUTIL_H
