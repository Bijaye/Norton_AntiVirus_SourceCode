// Copyright 1998 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/INCLUDE/VCS/defannuity.h_v   1.2   17 Apr 1998 15:04:10   RStanev  $
//
// Description:
//   Header file for the def annuity.
//
//****************************************************************************
// $Log:   S:/INCLUDE/VCS/defannuity.h_v  $
// 
//    Rev 1.2   17 Apr 1998 15:04:10   RStanev
// Merged branch changes.
// 
//    Rev 1.0.1.5   13 Apr 1998 18:48:52   RStanev
// Prototyped DefAnnuityQueryStatus.
// 
//    Rev 1.0.1.4   08 Apr 1998 19:35:46   RStanev
// Added DefAnnuityGetDownloadStatus().
// 
//    Rev 1.0.1.3   31 Mar 1998 15:49:56   RStanev
// Added dwConfirmationNumber to DefAnnuityInitialize().
// 
//    Rev 1.0.1.2   30 Mar 1998 21:50:44   RStanev
// Changed prototypes of functions to make IS happy.
// 
//    Rev 1.0.1.1   30 Mar 1998 12:25:52   RStanev
// Changed the prototype of DefAnnuityInitialize() to include parameters
// from INSTBIN.
// 
//    Rev 1.0.1.0   29 Mar 1998 01:14:02   RStanev
// Branch base for version QAKG
// 
//    Rev 1.0   29 Mar 1998 01:13:58   RStanev
// Initial revision.
// 
//****************************************************************************

enum {
    DefAnnuitySuccess = 0,
    DefAnnuityError,
    DefAnnuityUpdateAll,
    DefAnnuityUpdatePatches,
    DefAnnuityCancelUpdate,
    DefAnnuityDownloadAllowed,
    DefAnnuityDownloadNotAllowed
};

#if !defined(DEFANNUITYAPI)
#define DEFANNUITYAPI __declspec(dllimport)
#endif

#if defined(__cplusplus)
extern "C" {
#endif

DWORD DEFANNUITYAPI DefAnnuityInitialize (
    HWND hWnd,
    DWORD dwVersion,
    DWORD dwEnabled,
    DWORD dwConfirmationNumber,
    DWORD dwFreeLength,
    DWORD dwSubscriptionLength,
    DWORD dwFreeEndWarning,
    DWORD dwSubscriptionEndWarning
    );

DWORD DEFANNUITYAPI __stdcall DefAnnuityUninitialize (
    HWND hWnd
    );

DWORD DEFANNUITYAPI __stdcall DefAnnuityGetDownloadStatus (
    void
    );

DWORD DEFANNUITYAPI __stdcall DefAnnuityPreUpdateCheck (
    HWND hWnd,
    BOOL bQuiet,
    LPBYTE lpbyBuffer
    );

DWORD DEFANNUITYAPI __stdcall DefAnnuityPostUpdateCheck (
    HWND hWnd,
    BOOL bQuiet,
    LPBYTE lpbyBuffer
    );

DWORD DEFANNUITYAPI __stdcall DefAnnuityQueryStatus (
    LPDWORD pdwVersion,
    LPDWORD pdwInstallDate,
    LPDWORD pdwSubscriptionDate,
    LPLONG plExipresIn
    );

DWORD DEFANNUITYAPI DefAnnuityCreateSession(LPCTSTR szIntopts);

#if defined(__cplusplus)
}
#endif