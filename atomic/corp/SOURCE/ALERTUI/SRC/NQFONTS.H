////////////////////////////////////////////////////////////////////////////
// NQFonts.h : header file
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
////////////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/NQFONTS.H_v   1.1   25 Apr 1997 11:04:16   JBRENNA  $
//
// Description:
//
//  This module DBCS-enables the controls within a dialog.  It basically
//  does the same thing as "ngfont.cpp" in the SYMGUI project, but without
//  depending on quake (thanks Stahl!).
//
////////////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/NQFONTS.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:16   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:30   RFULLER
// Initial revision
// 
//    Rev 1.0   21 Jan 1997 18:39:58   SEDWARD
// Initial revision.
//
//
////////////////////////////////////////////////////////////////////////////


// NQFonts.h : implementation file
//
// $Header:
// $Log:

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
                                          HWND     hWnd,                       // [in]    Dialog Box Window
                                          HFONT    hFont,                      // [in]    Font to use
                                          LPTSTR    lpszClass,                  // [in]    Control window class or ""
                                          DWORD    dwStyle                     // [in]    Optional style bit to match
                                          );

int WINAPI NQ_GetDBCSFontSize(VOID);

BYTE WINAPI NQ_GetDBCSCharSet(VOID);



BOOL WINAPI _DBCSInstalled(VOID);
LPTSTR WINAPI NQ_GetDBCSFaceName(VOID);








































