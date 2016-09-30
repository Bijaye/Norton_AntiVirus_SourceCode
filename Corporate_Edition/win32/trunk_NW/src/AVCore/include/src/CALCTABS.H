// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//****************************************************************************
//
// $Header:   S:/INCLUDE/VCS/CalcTabs.h_v   1.0   01 Nov 1996 12:07:36   JBRENNA  $
//
// Description:
//   This header file is provides 3 inline functions used to dynamically
//   calculate top stops for a List Box control based on the column's header
//   position.
//
//   These are inline functions because they are needed in the VIRLIST, NPS,
//   and NAVWORK projects. These projects do not share any utility DLL. So,
//   inline functions are used. Unforunate that they don't share a DLL because
//   these inline functions are rather large.
//
//   Note, the below routines are confirmed to work on Windows 95. The should
//   also work on Windows NT and Windows 3.1, but this has not been tested.
//****************************************************************************
// $Log:   S:/INCLUDE/VCS/CalcTabs.h_v  $
// 
//    Rev 1.0   01 Nov 1996 12:07:36   JBRENNA
// Initial revision.
// 
//****************************************************************************

#ifndef CALC_TABS_H_INCLUDED
#define CALC_TABS_H_INCLUDED

#ifndef __cplusplus
#error This header file only compiles under a C++ compiler.
#endif

#ifndef SYM_WIN
#error This header file only compiles for Windows.
#endif

//****************************************************************************
// Description:
//   Converts Pixels to Dialog Units for the X axis. Note that the result is
//   rounded up for positive numbers and rounded down for negative numbers.
//
// Parameters:
//   BaseUnits    [in] The base units for this dialog. The return value from
//                     either GetDialogBaseUnits() or CalcDialogBaseUnits().
//   lPixels      [in] The pixels to convert to dialog units.
//
// Returns:
//   dialog unit equivalent of lPixels.
//
//****************************************************************************
// 11/01/1996 Jim Brennan - created.
//****************************************************************************
inline LONG PixelsToDlgUnitsX (const LONG lBaseUnits, const LONG lPixels)
{
    LONG lResult;

    lResult = ((lPixels * 4) * 10) / LOWORD (lBaseUnits);

    if (lResult > 0)
        lResult += 5;
    else
        lResult -= 5;

    lResult /= 10;

    return lResult;
}

//****************************************************************************
// Description:
//   Accurately calculates the base units given the current font for the
//   specified window. This is based on a MSDN algorithm (PSS ID #: Q145994).
//   This function differs from GetDialogBaseUnits() in that this function
//   uses the current font. GetDialogBaseUnits() assumes the current font is
//   the system font.
//
// Parameters:
//   hWnd    [in] A window that has the same font as the dialog.
//
// Returns:
//   LOWORD       Horizontal Dialog Base Unit (X)
//   LOWORD       Vertical   Dialog Base Unit (Y)
//****************************************************************************
// 11/01/1996 Jim Brennan - created.
//****************************************************************************
inline LONG CalcDialogBaseUnits (HWND hWnd)
{
    const HDC        hDC   = GetDC (hWnd);
    const HANDLE     hFont = GetWindowFont (hWnd);
    auto  HANDLE     hOldFont;
    auto  TEXTMETRIC tm;
    auto  SIZE       sSize;
    auto  int        nBaseUnitX;
    auto  int        nBaseUnitY;

    if (hFont)
        hOldFont = SelectObject (hDC, hFont);

//    LOGFONT fontInfo;
//    GetObject (hFont, sizeof (LOGFONT), &fontInfo);

    GetTextMetrics (hDC, &tm);
    nBaseUnitY = tm.tmHeight;

    GetTextExtentPoint(hDC,
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &sSize);
    nBaseUnitX = (sSize.cx / 26 + 1) / 2;

    if (hFont)
        SelectObject (hDC, hOldFont);
    ReleaseDC (hWnd, hDC);

    return MAKELONG (nBaseUnitX, nBaseUnitY);
}

//****************************************************************************
// Description:
//   Calculate nNumElems number of tab stops based on the locations of the
//   specified controls. This ensures that the columns in hList (List box
//   control) line up with the locations of there header windows.
//
// Parameters:
//   hList         [in]   List Box that contains the data.
//   lphWnds       [in]   Array of header windows.
//   lpnTabStops   [out]  Array of calculated tab stops based on the positions
//                        of the lphWnds windows.
//   nNumElems     [in]   Number of elements in lphWnds and lpnTabStops.
//
//****************************************************************************
// 11/01/1996 Jim Brennan - Created.
//****************************************************************************
inline void CalcTabStops(const HWND hList,
                         HWND FAR *lphWnds, int FAR *lpnTabStops,
                         const int nNumElems)
{
    SYM_ASSERT (IsWindow (hList));
    SYM_ASSERT (lphWnds);
    SYM_ASSERT (lpnTabStops);
    SYM_ASSERT (nNumElems > 0);
    SYM_ASSERT_CMD()                    // Make sure passed real windows.
    {
        for (int i = 0; i < nNumElems; ++i)
            SYM_ASSERT (IsWindow (lphWnds[i]));
    }

    auto   LONG lBaseUnits = CalcDialogBaseUnits (lphWnds[0]);
    auto   RECT rCtrl;
    auto   int  nCnt;

    for (nCnt = 0; nCnt < nNumElems; ++nCnt)
    {
        GetClientRect (lphWnds[nCnt],  &rCtrl);

        MapWindowPoints (lphWnds[nCnt], hList, (LPPOINT)&rCtrl, 2);

        lpnTabStops[nCnt] = PixelsToDlgUnitsX (lBaseUnits, rCtrl.left);
    }
}

#endif // CALC_TABS_H_INCLUDED
