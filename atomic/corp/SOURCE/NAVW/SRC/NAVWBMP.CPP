// Copyright 1992-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVW/VCS/navwbmp.cpv   1.4   22 Jul 1998 12:54:52   JBRENNA  $
//
// Description:
//      This file contains the routines required for processing the
//      opening bitmap for the Norton AntiVirus for Windows.
//
// Contains (as exports):
//
// See Also:
//***************************************************************************
// $Log:   S:/NAVW/VCS/navwbmp.cpv  $
// 
//    Rev 1.4   22 Jul 1998 12:54:52   JBRENNA
// Show the 256 color splash screen when running in 640x480 size with 256 or
// greater color mode.
// 
//    Rev 1.3   09 Dec 1997 01:41:56   jtaylor
// Added a scanner only splash panel with runtime switch
//
//    Rev 1.2   19 Jul 1997 14:39:20   GWESTER
// Added code to display the 256-color splash when under at least 256 colors
// and 800x600 resolution.
//
//    Rev 1.1   06 Mar 1997 17:19:50   DALLEE
// Removed network and password stub references.
// Related to HYDRA.15 port: no longer required.
//
//    Rev 1.0   06 Feb 1997 21:01:28   RFULLER
// Initial revision
//
//    Rev 1.3   23 Aug 1996 16:24:28   DALLEE
// Removed ifdef 0'd call to check WinNT startup chore from WM_DESTROY of
// the splash screen.  What kind of craziness was that?
// Check now occurs before splash screen (at same time as NT's check for 95
// happens).
//
//    Rev 1.2   02 Aug 1996 17:26:10   PVENKAT
// Fixed #62862.  Setting the Cursor to Arrow at the end of Splash Window.
//
//    Rev 1.1   16 Apr 1996 15:05:06   YVANG
// Ported from Kirin.
//
//    Rev 1.1   28 Mar 1996 14:15:50   jworden
// DBCS Enable
//
//    Rev 1.0   30 Jan 1996 15:55:14   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:16   BARRY
// Initial revision.
//
//    Rev 1.29   20 Nov 1995 15:59:44   DALLEE
// Changed to use NAV_INSTALLKEY_PRODUCT when constructing registry key for
// whether to display splash screen.
// This will be "Norton AntiVirus NT" for Gargoyle, and "Norton AntiVirus" for
// Luigi.
// (NAV_PRODUCT_NAME is currently "Norton AntiVirus" for both).
//
//    Rev 1.28   22 Sep 1995 13:38:10   DALLEE
// Reenable product for NT.  (Gargoyle)
//
//    Rev 1.27   19 Sep 1995 10:58:56   RSTANEV
// Now the space for the bitmap will not be cleared.
//
//    Rev 1.26   19 Sep 1995 10:44:42   RSTANEV
// The splash screen will not show if a certain registry key is set.  The
// timeout will still exist, due to initialization problems.
//
//    Rev 1.25   19 Sep 1995 08:32:18   JWORDEN
// Remove PostMessage for GET_NEW_INI from WM_DESTROY
//
//    Rev 1.24   02 Jul 1995 18:39:28   MARKL
// Fixes defect 39280.  The bitmap selects either a 16 or 256 bitmap
// for the opening screen, and the framing looks great using the
// appropriate quake routines.
//
//    Rev 1.23   30 Jun 1995 13:11:24   Sheathco
// Changed to have boarder around first bit map.
//
//    Rev 1.22   25 Jun 1995 09:41:30   KEITH
// Just adding/fixing the file header.
//***************************************************************************

#include "platform.h"                   // Platform independence

#ifdef SYM_WIN16
    #include "syminteg.h"               // For network routines
#endif

#include "ngui.h"
#include "navutil.h"
#include "navw.h"                       // NAV Windows Clinic stuff
#include "navwres.h"                    // NAV resource identifiers
#include "navver.h"
#include "n32behav.h"


//************************************************************************
// Local variables
//************************************************************************

static  BOOL        bShowBitmap = TRUE;

HPALETTE    g_hPalette = NULL;          // Handle to the background palette


//************************************************************************
// Local definitions
//************************************************************************

#define NAV_OPENING_BMP_DELAY   3500



//************************************************************************
// Local Prototypes
//************************************************************************

VOID LOCAL NavOpeningBmpWM_PAINT (      // Handles WM_PAINT for opening wnd
    HWND    hWnd                        // [in] handle of window to paint to
    );

DWORD SYM_EXPORT DisplayableColors (
    VOID
    );

BOOL WINAPI NCD_DrawBitmap (
    HDC hDC,
    LPRECT lpRect,
    HBITMAP hBitmap,
    HPALETTE hPalette,
    WORD wFlags);

HBITMAP WINAPI LoadResourceBitmap (
    HINSTANCE hInstance,
    LPSTR lpString,
    HPALETTE FAR *lphPalette);

HPALETTE WINAPI CreateDIBPalette (
    LPBITMAPINFO lpbmi,
    LPINT lpiNumColors);

VOID _inline NRealizeThePalette(HDC hdc)
{
    UINT        uNumColors = 0;
    DWORD       dwError = 0;

    uNumColors = RealizePalette(hdc);

    if (uNumColors == GDI_ERROR)
        dwError = GetLastError();

    return;
}


/* **************************************************************************

@Name:
HWND WINAPI NavOpeningBmpCreate (       // Creates opening credits wnd
    HWND        hWndParent              // [in] Handle of parent
    );

@Description:
This function creates the opening bitmap.

@Parameters:
$hWndParent$    Parent window handle

@Returns:
    Handle to the created splash bitmap window.

************************************************************************** */

HWND WINAPI NavOpeningBmpCreate (       // Creates opening credits wnd
    HWND        hWndParent              // [in] Handle of parent
    )
{
    auto    BITMAP      Bitmap       = {0};
    auto    HWND        hWndRet      = NULL;
    auto    UINT        uBorderWidth = GetSystemMetrics (SM_CYCAPTION) / 2;
    extern  HBITMAP     ghBmpOpening;
    extern  HINSTANCE   ghInst;
    static CFGREGKEY cfg =
        {
        HKEY_LOCAL_MACHINE,
        NAV_INSTALLKEY_PRODUCT,
        NAV_MODULE_CLINIC,
        NULL,
        NULL,
        NULL,
        NAV_DISABLESPLASHSCREEN,
        NULL
        };
    auto DWORD dwType, dwValue, dwValueSize;
    auto LONG  lRegResult;

                                        // See if we need opening bitmap
    bShowBitmap = TRUE;

    dwType = REG_DWORD;

    dwValueSize = sizeof(dwValue);

    lRegResult = ConfigRegQueryValue ( &cfg, NULL, &dwType, &dwValue, &dwValueSize );

    if ( lRegResult == ERROR_SUCCESS &&
        dwValueSize == sizeof(dwValue) &&
        dwType == REG_DWORD &&
        dwValue != FALSE )
        {
        bShowBitmap = FALSE;
        }

                                        // -----------------------------------
                                        // Load the bitmap, and find out how large
                                        // it is. We only use the large, 256-color
                                        // bitmap if we have at least 256 colors.
                                        // Otherwise, we use the 16 color splash.
                                        // -----------------------------------
    if (DisplayableColors() >= 256)
        { ghBmpOpening = LoadResourceBitmap(ghInst, MAKEINTRESOURCE(NAV_IS_SCANONLY() ? IDB_OPENING_SCAN_256_BMP : IDB_OPENING_256_BMP),
                                            &g_hPalette); }
    else
        { ghBmpOpening = LoadResourceBitmap(ghInst, MAKEINTRESOURCE(NAV_IS_SCANONLY() ? IDB_OPENING_SCAN_16_BMP : IDB_OPENING_16_BMP),
                                            &g_hPalette); }

    if (ghBmpOpening != NULL)
        {
        GetObject (ghBmpOpening, sizeof(BITMAP), (LPTSTR)&Bitmap);

                                        // -----------------------------------
                                        // Create the window such that the
                                        // bitmap is centered on the screen.
                                        // -----------------------------------

        hWndRet = CreateWindow (NAV_OPENING_BMP_CLASS, NULL,
                     WS_POPUP,
                     ((GetSystemMetrics(SM_CXSCREEN) - Bitmap.bmWidth ) / 2) -
                         uBorderWidth,
                     ((GetSystemMetrics(SM_CYSCREEN) - Bitmap.bmHeight) / 2) -
                         uBorderWidth,
                     Bitmap.bmWidth + (uBorderWidth * 2),
                     Bitmap.bmHeight + (uBorderWidth * 2),
                     hWndParent, NULL, ghInst, NULL);
        if (hWndRet != NULL)
            {
            ShowWindow (hWndRet, bShowBitmap ? SW_SHOWNORMAL : SW_HIDE );
            UpdateWindow (hWndRet);
            }
    }
                                        // -----------------------------------
                                        // Return if window was created
                                        // or not.
                                        // -----------------------------------
    return (hWndRet);
}


/* **************************************************************************

@Name:
LRESULT SYM_EXPORT WINAPI NavOpeningBmpWndProc ( // Opening bitmap window proc
    HWND        hWnd,                   // [in] Window handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam                  // [in] lParam to process
    )

@Description:
This is the window procedure for the opening bitmap of the Norton
AntiVirus for Windows.  It paints the opening bitmap within the client
area of the window.  The window terminates after an elapsed time has
gone by or after the user clicks on it with the mouse button.

@Parameters:
$hWnd$            [in] window handle
$wMessage$        [in] message to process
$wParam$          [in] wParam of message
$lParam$          [in] lParam of message

@Returns:
    Appropriate value as expected by caller.

************************************************************************** */

LRESULT SYM_EXPORT WINAPI NavOpeningBmpWndProc ( // Opening bitmap window proc
    HWND        hWnd,                   // [in] Window handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] wParam to process
    LPARAM      lParam                  // [in] lParam to process
    )
{
    static      UINT    uTimer;
    extern      int     gnCmdShow;
    extern      HWND    ghWndMain;

    switch ( uMessage )
        {
                                        // -----------------------------------
                                        // WM_CREATE:  Start a timer so
                                        // we close after the appropraite
                                        // delay.
                                        // -----------------------------------
        case WM_CREATE:
             uTimer = SetTimer (hWnd, 1, NAV_OPENING_BMP_DELAY, NULL);
             break;

                                        // -----------------------------------
                                        // WM_PAINT:  Paint the opening
                                        // bitmap.
                                        // -----------------------------------
        case WM_PAINT:
             if ( bShowBitmap )
                {
                NavOpeningBmpWM_PAINT (hWnd);
                }
             break;

                                        // -----------------------------------
                                        // WM_LBUTTONDOWN:  User wishes to
                                        // get on with the show.  Terminate
                                        // the window.
                                        // -----------------------------------
        case WM_TIMER:
        case WM_LBUTTONDOWN:
            KillTimer (hWnd, uTimer);
            DestroyWindow (hWnd);
            break;

                                        // -----------------------------------
                                        // WM_DESTROY:  We no longer need our
                                        // opening bitmap in memory.
                                        // -----------------------------------
        case WM_DESTROY:
            {
            extern      HBITMAP ghBmpOpening;

            if (ghBmpOpening)
                {
                DeleteObject (ghBmpOpening);
                ghBmpOpening = NULL;
                }

                                        // Delete the main background palette
            if (g_hPalette != NULL)
                {
                DeleteObject((HGDIOBJ)g_hPalette);
                g_hPalette = NULL;
                }

                                        // Tell the app it is okay to display
                                        // itself.
            ShowWindow (ghWndMain, gnCmdShow);
            SetCursor (LoadCursor (NULL, IDC_ARROW));
            }
            break;

                                        // Someone has realized their palette
        case WM_PALETTECHANGED:
            {
            HDC         hdc;
            HPALETTE    hOldPal;

            if ((HWND)wParam != hWnd && g_hPalette != NULL)
                {
                hdc = GetDC(hWnd);
                hOldPal = SelectPalette(hdc, g_hPalette, FALSE);
                NRealizeThePalette(hdc);
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);

                if (hOldPal)
                    SelectPalette(hdc, hOldPal, FALSE);

                ReleaseDC(hWnd, hdc);
                }
            break;
            }

                                        // Query the changed palette to adjust
                                        // our colors to the new palette
        case WM_QUERYNEWPALETTE:
            {
            HDC         hdc;
            HPALETTE    hOldPal;

            if (g_hPalette != NULL)
                {
                hdc = GetDC(hWnd);

                hOldPal = SelectPalette(hdc, g_hPalette, FALSE);
                NRealizeThePalette(hdc);

                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);

                if (hOldPal)
                    SelectPalette(hdc, hOldPal, FALSE);

                ReleaseDC(hWnd, hdc);
                return(TRUE);
                }
            return(FALSE);
            }

                                        // -----------------------------------
                                        // default:  Let Windows handle the
                                        // message since we do not want to
                                        // deal with it.
                                        // -----------------------------------
        default:
            return ( DefWindowProc ( hWnd, uMessage, wParam, lParam ) );
        }

    return (0L);
} // NavOpeningBmpWndProc()

/* **************************************************************************

@Name:
VOID LOCAL NavOpeningBmpWM_PAINT (      // Handles WM_PAINT for opening wnd
    HWND        hWnd                    // [in] handle of window to paint to
    );

@Description:
This routine handles the WM_PAINT message for the NavOpeningBmpWndProc()
routine.  It paints the opening bitmap within the client area of the
opening bitmap window.

@Parameters:
$hWnd$          [in] Handle to window to paint opening bitmap to.

@Returns:       Nothing

************************************************************************** */

VOID LOCAL NavOpeningBmpWM_PAINT (      // Handles WM_PAINT for opening wnd
    HWND        hWnd                    // [in] handle of window to paint to
    )
{
    auto    PAINTSTRUCT ps;             // Paint structure
    auto    HDC         hDC;            // Device context to paint to
    auto    RECT        rClient;
    auto    RECT        rBitmap;
    auto    WORD        wBorder         = GetSystemMetrics (SM_CYCAPTION) / 2;
    auto    WORD        wFrameWidth     = GetSystemMetrics (SM_CXBORDER);
    extern  HBITMAP     ghBmpOpening;

                                        // -----------------------------------
                                        // Get the device context that we
                                        // are to paint to.  If we have a
                                        // valid bitmap loaded, go ahead and
                                        // paint.
                                        // -----------------------------------
    hDC = BeginPaint (hWnd, &ps);

    if ( ghBmpOpening != NULL )
        {
                                        // Let NGUI draw our bitmap with
                                        // a nice 3D frame around it for
                                        // us.
        GetClientRect ( hWnd, &rClient );
        rBitmap.top    = 0;
        rBitmap.left   = 0;
        rBitmap.right  = rClient.right - rClient.left;
        rBitmap.bottom = rClient.bottom - rClient.top;
        NG_Draw3DFrame ( hWnd, hDC, &rBitmap, NG_RAISED, wFrameWidth );
        rBitmap.left   = rBitmap.left + wBorder + wFrameWidth;
        rBitmap.top    = rBitmap.top + wBorder + wFrameWidth;
        NCD_DrawBitmap(hDC, &rBitmap, ghBmpOpening, g_hPalette, TRUE);
        }
                                        // -----------------------------------
                                        // Clean up our current DC.
                                        // -----------------------------------
    EndPaint (hWnd, &ps);

} // NavOpeningBmpWM_PAINT()



//***************************************************************************
// DisplayableColors()
//
// Description:
//      Returns information on the displayable colors for the caller to
//      determine the type of bitmap he wishes to display.
//
// Parameters:
//      NONE
//
// Return Value:
//      DWORD   reflecting the color depth
//
//***************************************************************************
// 07/02/1995 MARKL Function Created.
//***************************************************************************
DWORD SYM_EXPORT DisplayableColors (
    VOID
    )
{
    HDC         hDC = GetDC ( GetDesktopWindow () );
    int         nColorDepth;
    DWORD       dwDisplayableColors;

    if ( GetDeviceCaps ( hDC, RASTERCAPS ) & RC_PALETTE )
        dwDisplayableColors = GetDeviceCaps ( hDC, SIZEPALETTE );
    else
        {
        nColorDepth = GetDeviceCaps ( hDC, BITSPIXEL ) *
                      GetDeviceCaps ( hDC, PLANES );
        if ( nColorDepth  >= 32 )
            dwDisplayableColors = 0xFFFFFFFF;
        else
            dwDisplayableColors = 1L << nColorDepth;
        }

    ReleaseDC ( GetDesktopWindow (), hDC );
    return ( dwDisplayableColors );
} // DisplayableColors()


///////////////////////////////////////////////////////////////////////////
//
// NCD_DrawBitmap
//
// Params:
//          HDC - Device context
//          LPRECT - Rect to draw the bitmap in
//          HBITMAP - handle to the bitmap to draw
//          HPALETTE - handle to the palette to use
//          WORD - flags, 0 to stretch bitmap to rect, !0 to put bitmap as is
//
// Returns:
//
///////////////////////////////////////////////////////////////////////////
// 07/17/97 - GWester, Function ported in from CDStart
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

BOOL WINAPI NCD_DrawBitmap(HDC hDC, LPRECT lpRect, HBITMAP hBitmap,
                           HPALETTE hPalette, WORD wFlags)
{
    HDC         hDCTmp;
    BOOL        bRet = FALSE;
    BITMAP      bitmap;
    HPALETTE    hOldPal = NULL;

                                        // Create a compatible device context
                                        // and select the bitmap to display
    hDCTmp = CreateCompatibleDC(hDC);
    SelectObject(hDCTmp, hBitmap);
    SetMapMode(hDCTmp, GetMapMode(hDC));

    if (hPalette != NULL)
        {
        hOldPal = SelectPalette(hDC, hPalette, FALSE);
        NRealizeThePalette(hDC);
        SelectPalette(hDCTmp, hPalette, FALSE);
        NRealizeThePalette(hDCTmp);
        }
                                        // Retrieve the bitmap

    if (GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap))
        {
        if (!wFlags)
            {
            StretchBlt(hDC,
                       lpRect->left,
                       lpRect->top,
                       lpRect->right - lpRect->left,
                       lpRect->bottom - lpRect->top,
                       hDCTmp,
                       0,
                       0,
                       bitmap.bmWidth,
                       bitmap.bmHeight,
                       SRCCOPY);
            }
        else
            {
            BitBlt(hDC,
                   lpRect->left,
                   lpRect->top,
                   lpRect->right - lpRect->left,
                   lpRect->bottom - lpRect->top,
                   hDCTmp,
                   0,
                   0,
                   wFlags == 1 ? SRCCOPY : SRCPAINT);
            }
        bRet = TRUE;

        if (hOldPal)
            SelectPalette(hDC, hOldPal, FALSE);
        }

    DeleteDC(hDCTmp);                   // Free allocated resources

    return(bRet);
}


///////////////////////////////////////////////////////////////////////////
//
// LoadResourceBitmap - This function loads a bitmap as a DIB so as to
//                      retrieve the palette information.
//
// Params:
//          HINSTANCE - The program's instance handle
//          LPSTR - The resource id of the bitmap
//          HPALETTE FAR * - handle that will receive the palette handle
//
// Returns: HBITMAP - handle to the bitmap (equiv to LoadBitmap)
//
///////////////////////////////////////////////////////////////////////////
// 07/17/97 - GWester, Function ported in from CDStart
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

HBITMAP WINAPI LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
                                  HPALETTE FAR *lphPalette)
{
    int                 iNumColors;
    HDC                 hdc;
    HRSRC               hRsrc;
    HGLOBAL             hGlobal;
    HBITMAP             hBitmapFinal = NULL;
    LPBITMAPINFOHEADER  lpbi;

    if (hRsrc = FindResource(hInstance, lpString, RT_BITMAP))
       {
       hGlobal = LoadResource(hInstance, hRsrc);
       lpbi = (LPBITMAPINFOHEADER)LockResource(hGlobal);

       hdc = GetDC(NULL);
       *lphPalette =  CreateDIBPalette((LPBITMAPINFO)lpbi, &iNumColors);

       if (*lphPalette)
          {
          SelectPalette(hdc, *lphPalette, FALSE);
          NRealizeThePalette(hdc);
          }

       hBitmapFinal = CreateDIBitmap(hdc,
                                     (LPBITMAPINFOHEADER)lpbi,
                                     (LONG)CBM_INIT,
                                     (LPSTR)lpbi + lpbi->biSize + iNumColors *
                                     sizeof(RGBQUAD),
                                     (LPBITMAPINFO)lpbi,
                                     DIB_RGB_COLORS );

       ReleaseDC(NULL, hdc);
       UnlockResource(hGlobal);
       FreeResource(hGlobal);
       }

    return (hBitmapFinal);
}


///////////////////////////////////////////////////////////////////////////
//
// CreateDIBPalette - Creates a logical palette from the bitmap loaded
//                    as a DIB file
//
// Params:
//          LPBITMAPINFO - info from the bitmap
//          LPINT - pointer to hold the number of colors
//
// Returns: HPALETtE - handle to the new palette
//
///////////////////////////////////////////////////////////////////////////
// 07/17/97 - GWester, Function ported in from CDStart
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

HPALETTE WINAPI CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
   int                  iCtr;
   HANDLE               hLogPal;
   HPALETTE             hPal = NULL;
   LPLOGPALETTE         lpPal;
   LPBITMAPINFOHEADER   lpbi;

   lpbi = (LPBITMAPINFOHEADER)lpbmi;

   if (lpbi->biBitCount <= 8)
       *lpiNumColors = (1 << lpbi->biBitCount);
   else
       *lpiNumColors = 0;  // No palette needed for 24 BPP DIB

   if (*lpiNumColors)
      {
      hLogPal = GlobalAlloc(GHND, sizeof (LOGPALETTE) +
                            sizeof (PALETTEENTRY) * (*lpiNumColors));

      lpPal = (LPLOGPALETTE)GlobalLock (hLogPal);
      lpPal->palVersion    = 0x300;
      lpPal->palNumEntries = *lpiNumColors;

      for (iCtr = 0;  iCtr < *lpiNumColors;  iCtr++)
         {
         lpPal->palPalEntry[iCtr].peRed   = lpbmi->bmiColors[iCtr].rgbRed;
         lpPal->palPalEntry[iCtr].peGreen = lpbmi->bmiColors[iCtr].rgbGreen;
         lpPal->palPalEntry[iCtr].peBlue  = lpbmi->bmiColors[iCtr].rgbBlue;
         lpPal->palPalEntry[iCtr].peFlags = 0;
         }

      hPal = CreatePalette (lpPal);
      GlobalUnlock(hLogPal);
      GlobalFree(hLogPal);
   }

   return hPal;
}

