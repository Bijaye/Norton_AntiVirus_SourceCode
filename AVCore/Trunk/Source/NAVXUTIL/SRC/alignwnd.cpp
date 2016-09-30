// Copyright 1996 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/navxutil/VCS/alignwnd.cpv   1.0   06 Feb 1997 21:05:36   RFULLER  $
//									
// Description:
//   This include file provides the implemenation for the alignment
//   functionality.
//									
//****************************************************************************
// $Log:   S:/navxutil/VCS/alignwnd.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:05:36   RFULLER
// Initial revision
// 
//    Rev 1.3   08 Nov 1996 17:14:02   JBRENNA
// 1. Rename internal func AlignWndGetTextSize to AlignWndGetWinTextSize.
// 2. Pull text size computation out of AlignWndGetWinTextSize and put it into
//    exported func AlignWndGetTextSize().
// 
//    Rev 1.2   18 Oct 1996 20:26:28   JBRENNA
// 1. Change gAlignWndFollow to gAlignWndFollowX and make it window sized
//    based instead of contents based.
// 2. Add support for gAlignWndFollowY. This includes adding the hWndFirst
//    and uPadding parameters to AlignWnd().
// 
//    Rev 1.1   10 Oct 1996 17:39:02   JBRENNA
// Add SYM_EXPORT to AlignWnd() and AlignWndResize().
// 
//    Rev 1.0   10 Oct 1996 17:07:42   JBRENNA
// Initial revision.
// 
//    Rev 1.1   27 Sep 1996 15:04:14   JBRENNA
// Make some code easier to read. ... does same thing.
// 
//    Rev 1.0   27 Sep 1996 14:46:14   JBRENNA
// Initial revision.
// 
//****************************************************************************

#include "platform.h"
#include "AlignWnd.h"

                                    // Only useful on Windows platform
#ifndef SYM_WIN
#error AlignWnd.CPP should only be compiled on SYM_WIN platforms.
#endif

//****************************************************************************
// Function Prototypes

static void AlignWndProcess (const HWND hWndMove, const HWND hWndFixed, 
                             const UINT wFlags,   const HWND hWndFirst,
                             const UINT uPadding);
static void AlignWndGetCalcSize (const HWND hWnd, LPSIZE lpSize);
static void AlignWndGetWinTextSize (const HWND hWnd, LPSIZE lpSize, 
                                    BOOL bRemoveAmp = TRUE);
static void AlignWndGetCheckBoxSize (const HWND hWnd, LPSIZE lpSize);

//****************************************************************************
// Public Routines

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
                          const UINT uPadding)
{
                                    // Something must be specified.
    SYM_ASSERT (wFlags);

    SYM_ASSERT (hWndMove);
    SYM_ASSERT (hWndFixed);
    SYM_ASSERT (::IsWindow (hWndMove));
    SYM_ASSERT (::IsWindow (hWndFixed));

                                    // hWndFirst must be non-NULL 
                                    // when ...FollowY specified.
    SYM_ASSERT (!(wFlags & gAlignWndFollowY) ||
                (wFlags & gAlignWndFollowY) && hWndFirst);

                                    // hWndFirst must be NULL when ...FollowY
                                    // not specified.
    SYM_ASSERT ((wFlags & gAlignWndFollowY) ||
                !(wFlags & gAlignWndFollowY) && hWndFirst == NULL);

                                    // hWndFirst must be one of the two windows.
    SYM_ASSERT (hWndFirst == NULL || 
                hWndFirst == hWndMove || hWndFirst == hWndFixed);

                                    // Can only align different controls.
    SYM_ASSERT (hWndMove != hWndFixed);

                                    // Internal flag should not be passed in.
    SYM_ASSERT (0 == (wFlags & gAlignWndResize));

    AlignWndProcess (hWndMove, hWndFixed, wFlags, hWndFirst, uPadding);
}

//----------------------------------------------------------------------------
// AlignWndResize()
//
// Description:
//   Resize the specified window to the size of its contents.
//
// Parameters:
//   hWnd     [in] The window to resize.
//----------------------------------------------------------------------------
// 09/27/96 JBRENNA - Created.
//----------------------------------------------------------------------------
VOID SYM_EXPORT AlignWndResize (const HWND hWnd)
{
    SYM_ASSERT (hWnd);
    SYM_ASSERT (::IsWindow (hWnd));

    AlignWndProcess (hWnd, hWnd, gAlignWndResize, NULL, 0);
}

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
// 11/08/96 JBRENNA - exported.
//----------------------------------------------------------------------------
VOID SYM_EXPORT AlignWndGetTextSize (const HWND hWnd,       LPSTR  lpStr,
                                     const BOOL bRemoveAmp, LPSIZE lpSize)
{
    SYM_ASSERT (hWnd);
    SYM_ASSERT (::IsWindow (hWnd));

    const HANDLE hFont      = GetWindowFont (hWnd);
    const HDC    hDC        = GetDC (hWnd);
    const long   lMaxStrLen = 1024;
    auto  HANDLE hOldFont;
    auto  TCHAR  szNoAmp[lMaxStrLen];
    auto  LPTSTR lpszText;

                                // Make sure the right font is selected.
    if (hFont)
        hOldFont = SelectObject (hDC, hFont);

                                // Remove any '&' from the string if the
                                // the caller instructed us to.
    if (!bRemoveAmp)
        lpszText = lpStr;
    else
    {
        auto LPTSTR lpAmp = STRCHR (lpStr, '&');

                                // Remove any '&' from the string.
        if (!lpAmp)
            lpszText = lpStr;
        else
        {
                                // Remove the '&'. This should be fine with
                                // DBCS strings because the the STRCHR up
                                // above found a Single Byte Char '&'.
            lpszText = szNoAmp;
            STRNCPY (szNoAmp, lpStr, lpAmp - lpStr);

                                // STRNCPY not add EOS automatically
            szNoAmp[lpAmp - lpStr] = EOS;

            STRCAT  (szNoAmp, lpAmp + 1);

                                // Ensure we did not overrun the buffer.
            SYM_ASSERT (STRLEN (szNoAmp) < lMaxStrLen);
        }
    }

                                // Get the width and height of the string.
    GetTextExtentPoint(hDC, lpszText, STRLEN (lpszText), lpSize);

    if (hFont)
        SelectObject (hDC, hOldFont);
    ReleaseDC (hWnd, hDC);
}

//****************************************************************************
// Local Routines

//----------------------------------------------------------------------------
// 09/27/96 JBRENNA - Created.
// 10/10/96 JBRENNA - Add gAlignWndBottom ability.
// 10/18/96 JBRENNA - Add gAlignWndFollowY ability.
//----------------------------------------------------------------------------
static void AlignWndProcess (const HWND hWndMove, const HWND hWndFixed, 
                             const UINT wFlags,   const HWND hWndFirst,
                             const UINT uPadding)
{
    const HWND      hParent = GetParent (hWndFixed);
    auto  RECT      rWndMove;
    auto  RECT      rWndFixed;
    auto  POINT     sMoveToPos;
    auto  SIZE      sMoveToSize;


                                    // Can only align controls of same parent.
    SYM_ASSERT (GetParent (hWndMove) == GetParent (hWndFixed));

    GetWindowRect (hWndMove,  &rWndMove);
    GetWindowRect (hWndFixed, &rWndFixed);

    MapWindowPoints (NULL, hParent, (LPPOINT)&rWndMove,  2);
    MapWindowPoints (NULL, hParent, (LPPOINT)&rWndFixed, 2);

    sMoveToPos.x   = rWndMove.left;
    sMoveToPos.y   = rWndMove.top;
    sMoveToSize.cx = rWndMove.right  - rWndMove.left;
    sMoveToSize.cy = rWndMove.bottom - rWndMove.top;


    if (wFlags & gAlignWndLeft)
        sMoveToPos.x = rWndFixed.left;

    if (wFlags & gAlignWndFollowX)
        sMoveToPos.x = rWndFixed.right + 1 /* move outside of rWndFixed */ + uPadding;

    if (wFlags & gAlignWndResize)
        AlignWndGetCalcSize (hWndFixed, &sMoveToSize);

    if (wFlags & gAlignWndCenterY)
    {
        const UINT uFixedHeight    = rWndFixed.bottom - rWndFixed.top;
        const UINT uFixedTopOffset = uFixedHeight / 2;
        const UINT uMoveHeightD2   = sMoveToSize.cy / 2;

        sMoveToPos.y = rWndFixed.top + uFixedTopOffset - uMoveHeightD2;
    }

                                    // Calc new Y pos for gAlignWndBottom
    if (wFlags & gAlignWndBottom)
        sMoveToPos.y = rWndFixed.bottom - sMoveToSize.cy;

    if ((wFlags & gAlignWndFollowY) && hWndFirst == hWndMove)
    {
        sMoveToPos.y = rWndFixed.top - sMoveToSize.cy - uPadding;
    }
    else if ((wFlags & gAlignWndFollowY) && hWndFirst == hWndFixed)
    {
        const UINT uFixedHeight    = rWndFixed.bottom - rWndFixed.top;

        sMoveToPos.y = rWndFixed.top + uFixedHeight + uPadding;
    }

                                    // Reposition hWndMove to the new location
    MoveWindow (hWndMove, sMoveToPos.x, sMoveToPos.y, 
                sMoveToSize.cx, sMoveToSize.cy, FALSE);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static void AlignWndGetCalcSize (const HWND hWnd, LPSIZE lpSize)
{
    SYM_ASSERT (hWnd);
    SYM_ASSERT (::IsWindow (hWnd));

    const LONG   lStyle             = GetWindowStyle (hWnd);
    auto  BOOL   bProcessed         = FALSE;
    auto  BOOL   bAddGroupStylePad  = FALSE;
    auto  TCHAR  szClassName[SYM_MAX_PATH];

    lpSize->cx = 0;
    lpSize->cy = 0;

                                // Get the class name
    if (GetClassName (hWnd, szClassName, sizeof (szClassName)) == 0)
        return;

    if (0 == STRCMP (szClassName, "Static"))
    {
        AlignWndGetWinTextSize (hWnd, lpSize);
        bProcessed = TRUE;
    }

    if (0 == STRCMP (szClassName, "Button"))
    {
        if (lStyle & (BS_CHECKBOX | BS_AUTOCHECKBOX))
        {
            AlignWndGetCheckBoxSize (hWnd, lpSize);
            bAddGroupStylePad = TRUE;
            bProcessed        = TRUE;
        }
    }

    if (!bProcessed)
    {
        const LPTSTR lpszColon = STRCHR (szClassName, ':');

                                    // If string has a ':', truncate it off.
        if (lpszColon)
            *lpszColon = EOS;

                                    // If this is a spin control, simply use
                                    // the controls Client Rect as the size.
        if (0 == STRCMP (szClassName, "NGSpinButton"))
        {
            auto RECT rSpin;
            GetWindowRect (hWnd, &rSpin);
            lpSize->cx = rSpin.right  - rSpin.left;
            lpSize->cy = rSpin.bottom - rSpin.top;

            bProcessed = TRUE;
        }
    }

                                    // This control type is not currently 
                                    // handled by this routine. Feel free
                                    // to add handling.
    if (!bProcessed)
        SYM_ASSERT (FALSE);

                                    // Add pixels if WS_STYLE is on.
    if (bAddGroupStylePad && (lStyle & WS_GROUP))
    {
        const UINT   uGroupStylePad = 2;

        lpSize->cx += uGroupStylePad;
        lpSize->cy += uGroupStylePad;
    }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static void AlignWndGetWinTextSize (const HWND hWnd, LPSIZE lpSize, 
                                    BOOL bRemoveAmp)
{
    SYM_ASSERT (hWnd);
    SYM_ASSERT (::IsWindow (hWnd));

    auto  TCHAR  szBuffer[SYM_MAX_PATH];
    auto  int    result;

    result = GetWindowText (hWnd, szBuffer, sizeof (szBuffer));
    SYM_ASSERT (result);

    AlignWndGetTextSize (hWnd, szBuffer, bRemoveAmp, lpSize);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static void AlignWndGetCheckBoxSize (const HWND hWnd, LPSIZE lpSize)
{
    SYM_ASSERT (hWnd);
    SYM_ASSERT (::IsWindow (hWnd));

    const UINT    uPadBtnBitmapNText= 4;
    const SIZE    sStdBitmapSize = { 13, 13 };

    AlignWndGetWinTextSize (hWnd, lpSize);

    lpSize->cx += sStdBitmapSize.cx;        // Add bitmp width to overall width
    lpSize->cx += uPadBtnBitmapNText;       // Add padding for Between Bitmap & Text

    lpSize->cy = max (lpSize->cy, sStdBitmapSize.cy);
}
