//////////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1993 - 2003, 2005 Symantec Corporation. All rights reserved.
// 
// Description: This file contains the prototypes and typedefs for runtime DLL
// version checking
// 
// $Header:   S:/INCLUDE/VCS/version.h_v   1.1   14 Jun 1996 11:20:16   MBARNES  $
// 
// $Log:   S:/INCLUDE/VCS/version.h_v  $
// 
//    Rev 1.1   14 Jun 1996 11:20:16   MBARNES
// Changed VERREC to include a uRevision data member
// 
//    Rev 1.0   26 Jan 1996 20:17:50   RJACKSO
// Initial revision.
// 
//    Rev 1.14   27 Aug 1994 18:32:20   DOUGN
// Ported from QAK4
// 
//    Rev 1.13   26 Apr 1994 16:36:52   BRAD
// Removed PVCS Logs
/////////////////////////////////////////////////////////////////////////

#ifndef INCL_VERSION
#define INCL_VERSION

#include "quakever.h"

#define SYMKRNL_VERSION_MAJOR   QUAKE_VERSION_MAJOR
#define SYMKRNL_VERSION_MINOR   QUAKE_VERSION_MINOR
#define SYMKRNL_BUILD_NUMBER    QUAKE_BUILD_NUMBER

#define SYMUTIL_VERSION_MAJOR   QUAKE_VERSION_MAJOR
#define SYMUTIL_VERSION_MINOR   QUAKE_VERSION_MINOR
#define SYMUTIL_BUILD_NUMBER    QUAKE_BUILD_NUMBER

typedef struct VERREC
    {
    LPCSTR      lpName;     // name of module, may be NULL
                            // do not include extension
    UINT        uMajor;
    UINT        uMinor;
    UINT        uRevision;  // must be 0
    UINT        uBuild;
    DWORD       dwReserved; // must be 0
#ifdef _OS2
    } VERREC, * FAR LPVERREC;
#else
    } VERREC, FAR * LPVERREC;
#endif


//////////////////////////////////////////////////////////////////////////
// Function pointer types for <DLL-Name>CheckVersion functions.
//////////////////////////////////////////////////////////////////////////
#ifdef _OS2
typedef void (*WINAPI CHECKVERCALLBACK_TYP)
    ( HINSTANCE hInst, LPVERREC lpExpected, LPVERREC lpFound );

typedef void (*WINAPI DLLCHECKVERSION_TYP)
    ( LPVERREC lpVersion, CHECKVERCALLBACK_TYP CallBack );
#else
typedef void (WINAPI *CHECKVERCALLBACK_TYP)
    ( HINSTANCE hInst, LPVERREC lpExpected, LPVERREC lpFound );

typedef void (WINAPI *DLLCHECKVERSION_TYP)
    ( LPVERREC lpVersion, CHECKVERCALLBACK_TYP CallBack );
#endif

#ifdef __cplusplus                    // start prototypes
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
// SYMKRNLCheckVersion
//
// A DLL which links to SYMKRNL calls this function inside of its own
// <DLL-Name>CheckVersion function, in order to ensure that the version
// of SYMKRNL it is linked to is late enough.
//    
// Input:   lpExpected  expected version 
//          CallBack    function to call in case of mismatch
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI SYMKRNLCheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack );
    

//////////////////////////////////////////////////////////////////////////
// SYMUTILCheckVersion
//
// A DLL which links to SYMUTIL calls this function inside of its own
// <DLL-Name>CheckVersion function, in order to ensure that the version
// of SYMUTIL it is linked to is late enough.
//    
// Input:   lpExpected  expected version 
//          CallBack    function to call in case of mismatch
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI SYMUTILCheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack );
void SYM_EXPORT WINAPI SYMVWRCheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack );
void SYM_EXPORT WINAPI SYMVIDCheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack );
    

//////////////////////////////////////////////////////////////////////////
//  SYMCheckVersions
//
//  An application calls this function before calling any of the Symantec
//  DLLs it links to, in order to ensure that the versions of the DLLs are
//  recent enough. The app passes in an array of DLLs to check. 
//  SYMCheckVersions calls the <DLL-Name>CheckVersion function for each of
//  of DLLs, using a run-time link. If any of the DLLs is too old, can't
//  be found, or doesn't SYM_EXPORT a <DLL-Name>CheckVersion function, a dialog
//  box informs the user of the nature of the problem. 
//  For code simplicity, this function always returns NOERR under platforms
//  other than Windows.
//
//  Input:  hWnd        app window handle, to use as parent, may be NULL
//          uCount      number of DLLs in the list to check
//          lpVer       array of VERREC, uCount in number
//  Return: ERR         a mismatch was found and the user didn't override
//          NOERR       no mismatch found, or the user chose to override
//////////////////////////////////////////////////////////////////////////
int SYM_EXPORT WINAPI SYMCheckVersions( HWND hWnd, UINT uCount, LPVERREC lpVer );

BOOL SYM_EXPORT WINAPI SYMIsWin310or311(void);

#ifdef __cplusplus                    // end of prototypes
}
#endif

#endif  // INCL_VERSION

