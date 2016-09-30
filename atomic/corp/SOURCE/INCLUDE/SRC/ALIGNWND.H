// Copyright 1996 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/include/VCS/alignwnd.h_v   1.0   06 Feb 1997 20:51:42   RFULLER  $
//									
// Description:
//   This include file provides the constants, typdefs, prototypes, etc.
//   in order to provide control alignment functionality.
//									
//****************************************************************************
// $Log:   S:/include/VCS/alignwnd.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:42   RFULLER
// Initial revision
// 
//    Rev 1.3   08 Nov 1996 17:28:58   JBRENNA
// Add AlignWndGetTextSize() prototype.
// 
//    Rev 1.2   18 Oct 1996 20:28:32   JBRENNA
// 1. Change gAlignWndFollow to gAlignWndFollowX.
// 2. Add gAlignWndFollowY.
// 3. Update AlignWnd() prototype.
// 
//    Rev 1.1   10 Oct 1996 17:36:58   JBRENNA
// Add extern "C" and SYM_EXPORT to function prototypes.
// 
//    Rev 1.0   10 Oct 1996 17:07:22   JBRENNA
// Initial revision.
// 
//    Rev 1.0   27 Sep 1996 14:47:10   JBRENNA
// Initial revision.
// 
//****************************************************************************

#ifndef _ALIGNWND_H
#define _ALIGNWND_H
                                    // Only useful on Windows platform
#ifdef SYM_WIN

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

//****************************************************************************
// Provides the alignment types. See the functions for more info.

enum {
gAlignWndResize    = (1 << 0),          // Reserved
gAlignWndLeft      = (1 << 1),
gAlignWndFollowX   = (1 << 2),
gAlignWndFollowY   = (1 << 3),
gAlignWndCenterY   = (1 << 4),
gAlignWndBottom    = (1 << 5),
};

//****************************************************************************
// Prototypes

//----------------------------------------------------------------------------
// AlignWnd()
//
// Description:
//   This function moves the hWndMove window such that it is aligned to the
//   hWndFixed window. The wFlags parameter determines the alignment that
//   is processed. Note that some wFlags settings are not based on window size.
//   Instead, the flag cause the routine to compute the content size of
//   hWndFixed to determine where hWndMove should be moved. (Example, 
//   gAlignWndFollowX).
//
// Parameters:
//   hWndMove      [in] The window being aligned. This window moves.
//   hWndFixed     [in] The window defining where hWndMove should move. This
//                      window does not move.
//   hWndFirst     [in] The first window for a follow attribute. Must be 
//                      either hWndMove or hWndFixed. See the gAlignWndFollowY
//                      flag for more info.
//   uPadding      [in] Pixel padding between the windows.
//   wFlags        [in] A bitwise field of operations:
//
//       gAlignWndLeft    - Align left sides of the windows
//                          (Window size based)
//       gAlignWndFollowX - Align windows such that hWndMove window directly
//                          follows hWndFixed. Only moves hWndMove on the X
//                          axis. (Window size based)
//       gAlignWndFollowY - Aligns windows such that hWndFirst is the top
//                          window and the non-hWndFirst is just below it. 
//                          Only moves hWndMove on the Y axis.
//                          (Window size based)
//       gAlignWndCenterY - Align windows such that they center on horzitonal
//                          plane. (Contents based)
//       gAlignWndBottom  - Align windows such that the bottoms line up.
//                          (Window size based)
//----------------------------------------------------------------------------
// 09/27/96 JBRENNA - Created.
// 10/10/96 JBRENNA - Add gAlignWndBottom ability.
// 10/18/96 JBRENNA - Add gAlignWndFollowY ability.
//----------------------------------------------------------------------------
VOID SYM_EXPORT AlignWnd (const HWND hWndMove, const HWND hWndFixed,
                          const UINT wFlags,   const HWND hWndFirst, 
                          const UINT uPadding);

//----------------------------------------------------------------------------
// AlignWndResize()
//
// Description:
//   Resize the specified window to the size of its contents.
//
// Parameters:
//   hWnd     [in] The window to resize.
//----------------------------------------------------------------------------
VOID SYM_EXPORT AlignWndResize (const HWND hWnd);

//----------------------------------------------------------------------------
// AlignWndGetTextSize()
//
// Description:
//   Get the size associated with the specified string in the font specified
//   hWnd. The bRemoveAmp parameter determines if this routine should strip
//   the '&' from the string before calculating the text size. This is useful
//   for controls in which Windows translates the '&' to an underline of the
//   next word.
//
// Parameters:
//   hWnd       [in]  Determines the current font.
//   lpStr      [in]  String to calculate the size of.
//   bRemoveAmp [in]  T/F: Remove '&' character before calculation? If you
//                    know there is no '&', set this to FALSE.
//   lpSize     [out] Size of the string.
//----------------------------------------------------------------------------
VOID SYM_EXPORT AlignWndGetTextSize (const HWND hWnd,       LPSTR  lpStr,
                                     const BOOL bRemoveAmp, LPSIZE lpSize);

#ifdef __cplusplus
}
#endif

#endif // SYM_WIN
#endif // _ALIGNWND_H
