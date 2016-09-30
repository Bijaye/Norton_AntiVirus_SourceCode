/////////////////////////////////////////////////////////////////////
// UNIUTIL.H - Created - 02/03/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/UNIUTIL.H_v   1.0   24 Apr 1997 11:31:56   JBRENNA  $
//
// Description:
//    Header file for the Unicode Utility static library.  This lib
//    will contain utility function such as Quake Functions or other
//    misc. functions that need to be in Unicode.  This Lib was
//    created so that all of our Unicode components can share these
//    funcitons.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/UNIUTIL.H_v  $
// 
//    Rev 1.0   24 Apr 1997 11:31:56   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:46   RFULLER
// Initial revision
// 
//    Rev 1.2   06 Feb 1997 22:55:26   TSmith
// Added 'WINAPI' to 'NavGetDomainName' declarations.
// 
//    Rev 1.1   06 Feb 1997 21:23:28   TSmith
// Added declarations for functions 'NavGetDomainNameW' and 'NavGetDomainNameA'.
// 
//    Rev 1.0   03 Feb 1997 17:02:12   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__UNIUTIL_H)
#define __UNIUTIL_H


/////////////////////////////////////////////////////////////////////
// Function Prototypes.

VOID  WINAPI NQ_SetFontsInDialog(
                                HWND    hDlg,                       // [in]     Dialog window
                                DWORD   dwReserved                  // [in]     Reserved (user zero)
                                );
WORD  WINAPI NQ_FontGet(
                                HWND  hWnd,                         // Window to get DC from...
                                HFONT FAR *lphFont,                 // If NULL, then free cached fonts
                                int nWeight,                        // FW_NORMAL, FW_BOLD, or custom wt
                                int nHeight )  ;                    // If zero, then use default height

VOID  WINAPI NQ_SetControlFont(
                                HWND     hWnd,                      // [in]    Dialog Box Window
                                HFONT    hFont,                     // [in]    Font to use
                                LPTSTR   lpszClass,                 // [in]    Control window class or ""
                                DWORD    dwStyle                    // [in]    Optional style bit to match
                                );

int WINAPI NQ_GetDBCSFontSize(VOID);

BYTE WINAPI NQ_GetDBCSCharSet(VOID);

BOOL WINAPI _DBCSInstalled(VOID);
LPTSTR WINAPI NQ_GetDBCSFaceName(VOID);

#if defined( UNICODE )
    #define NavGetDomainName NavGetDomainNameW
#else
    #define NavGetDomainName NavGetDomainNameA
#endif

BOOL WINAPI NavGetDomainNameW(
    IN  LPWSTR lpszServerName,          // Unicode name of system to get domaion name for
    IN  UINT   cbDomainName,            // Size in bytes of buffer pointed to by lpszDomainName
    OUT LPWSTR lpszDomainName           // Buffer to hold Unicode domain name
    );

BOOL WINAPI NavGetDomainNameA(
    IN  LPSTR lpszServerName,           // Ansi name of system to get domain name for
    IN  UINT   cbDomainName,            // Size in bytes of buffer pointed to by lpszDomainName
    OUT LPSTR lpszDomainName            // Buffer to hold Ansi domain name
    );

#endif // if !defined(__UNIUTIL_H)
