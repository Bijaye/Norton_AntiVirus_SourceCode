#include "platform.h"
#include "navgraph.hpp"

// ----------------------------------------------------------------------------
//                      LOCAL NON C++ PROTOTYPES
// ----------------------------------------------------------------------------

WORD LOCAL DIBNumColors (
    LPBITMAPINFOHEADER   lpbi );

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CNavBitmap::CNavBitmap (
    HINSTANCE   hInstance,
    WORD        wBitmapID )
{
    SYM_ASSERT ( hInstance );
    SYM_ASSERT ( wBitmapID );

    m_hBitmap       = NULL;
    m_hInstance     = hInstance;
    m_wBitmapID     = wBitmapID;
    m_wNumColors    = 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CNavBitmap::~CNavBitmap()
{
    CNavBitmap::Destroy();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BOOL WINAPI CNavBitmap::Create (
    HPALETTE    hPal,
    HDC         hDC )
{
    auto    BOOL    bStatus = FALSE;

    SYM_ASSERT ( m_hInstance );
    SYM_ASSERT ( m_wBitmapID );
    SYM_ASSERT ( hDC );

    if ( hDC )
        {
        HRSRC hRes = FindResource (
                                  m_hInstance,
                                  MAKEINTRESOURCE ( m_wBitmapID ),
                                  RT_BITMAP
                                  );
        if ( hRes )
            {
            HGLOBAL hDIB = LoadResource( m_hInstance, hRes );

            if ( hDIB )
                {
                LPBITMAPINFOHEADER lpbmInfoHdr =
                                (LPBITMAPINFOHEADER) LockResource ( hDIB );

                if ( lpbmInfoHdr && hPal )
                    {
                    LPSTR lpBits = (LPSTR)lpbmInfoHdr + *(LPDWORD)lpbmInfoHdr +
                                    PaletteSize( lpbmInfoHdr );

                    HPALETTE hOldPal = SelectPalette( hDC, hPal, FALSE );

                    m_hBitmap = CreateCompatibleBitmap (
                                            hDC,
                                            (WORD)lpbmInfoHdr->biWidth,
                                            (WORD)lpbmInfoHdr->biHeight
                                            );
                    if ( m_hBitmap )
                        {
                        bStatus = TRUE;

                        SetDIBits (
                                  hDC,
                                  m_hBitmap,
                                  0,
                                  (WORD)lpbmInfoHdr->biHeight,
                                  lpBits,
                                  (LPBITMAPINFO)lpbmInfoHdr,
                                  DIB_RGB_COLORS
                                  );
                        }

                    SelectPalette( hDC, hOldPal, FALSE );
                    }
                else
                    {
                    m_hBitmap = LoadBitmap ( m_hInstance,
                                             MAKEINTRESOURCE(m_wBitmapID) );
                    }

                if ( m_hBitmap )
                    bStatus = TRUE;

                FreeResource( hDIB );
                }
            }
        }

    return ( bStatus );
}

//---------------------------------------------------------------------
//
// Function:   CNavBmp::PaletteSize
//
// Purpose:    Given a pointer to a DIB, returns number of bytes
//             in the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//---------------------------------------------------------------------

WORD CNavBitmap::PaletteSize (
    LPBITMAPINFOHEADER   lpbi )
{
    m_wNumColors = DIBNumColors (lpbi);

    return ( m_wNumColors * sizeof (RGBQUAD) );
}

//---------------------------------------------------------------------
//
// Function:   DIBNumColors
//
// Purpose:    Given a pointer to a DIB, returns a number of colors in
//             the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//---------------------------------------------------------------------

WORD LOCAL DIBNumColors (
    LPBITMAPINFOHEADER   lpbi )
{
    auto    WORD    wBitCount = 0;
    auto    DWORD   dwClrUsed = 0;
                                        // ------------------------------------
                                        // If this is a Windows style DIB, 
                                        // the number of colors in the color
                                        // table can be less than the number
                                        // of bits per pixel allows for (i.e.
                                        // lpbi->biClrUsed can be set to some
                                        // value). If this is the case, return
                                        // the appropriate value.
                                        // ------------------------------------

    dwClrUsed = lpbi->biClrUsed;

    if ( dwClrUsed == 0 )
        {
                                        // ------------------------------------
                                        // Calculate the number of colors in 
                                        // the color table based on the number
                                        // of bits per pixel for the DIB.
                                        // ------------------------------------

        wBitCount = lpbi->biBitCount;

        switch (wBitCount)
            {
            case 1:
                dwClrUsed = 2;
                break;

            case 4:
                dwClrUsed = 16;
                break;

            case 8:
                dwClrUsed = 256;
                break;

            default:
                break;
            }
        }

    return ( (WORD) dwClrUsed );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BOOL WINAPI CNavBitmap::Render (
    HDC     hDC,                        // [in] device context
    LPRECT  lpRect,                     // [in] rectangle where to paint
    BOOL    bStretch )                  // [in] stretch it
{
    SYM_ASSERT ( hDC );
    SYM_ASSERT ( lpRect );
    SYM_ASSERT ( m_hBitmap );

    auto    BOOL    bRendered = FALSE;

    if ( m_hBitmap )
        {
        HDC hMemDC = CreateCompatibleDC( hDC );
        SYM_ASSERT ( hMemDC );

        if ( hMemDC )
            {
            BITMAP  rBitmap;
            GetObject( m_hBitmap, sizeof(rBitmap), &rBitmap );

            SelectObject( hMemDC, m_hBitmap );

            if ( bStretch )
                {
                StretchBlt (
                           hDC,
                           lpRect->left,
                           lpRect->top,
                           lpRect->right - lpRect->left,
                           lpRect->bottom - lpRect->top,
                           hMemDC, 0, 0,
                           rBitmap.bmWidth,
                           rBitmap.bmHeight,
                           SRCCOPY
                           );
                }
            else
                {
                BitBlt (
                       hDC,
                       lpRect->left,
                       lpRect->top,
                       rBitmap.bmWidth,
                       rBitmap.bmHeight,
                       hMemDC,
                       0,0,
                       SRCCOPY
                       );
                }
        
            bRendered = TRUE;

            DeleteDC ( hMemDC );
            }
        }

    return ( bRendered );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

VOID WINAPI CNavBitmap::Destroy (
    VOID )
{
    if ( m_hBitmap )
        DeleteObject( m_hBitmap );

    m_hBitmap = NULL;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DWORD WINAPI CNavBitmap::GetNumColors (
    VOID )
{
    return ( m_wNumColors );
}

// ############################################################################
// ############################################################################
// ##                                                                        ##
// ##                       class CNavBitmapManager;                         ##
// ##                                                                        ##
// ############################################################################
// ############################################################################


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CNavBitmapManager::CNavBitmapManager (
    HWND    hAppTopWindow )
{
    SYM_ASSERT ( hAppTopWindow );

    m_hWnd              =   hAppTopWindow;
    m_bRealize          =   TRUE;
    m_bCanRender        =   TRUE;
    m_hPalette          =   NULL;
    m_uEntriesChanged   =   0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CNavBitmapManager::~CNavBitmapManager ()
{
    if ( m_hPalette )
        DeleteObject ( m_hPalette );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BOOL WINAPI CNavBitmapManager::OnQueryNewPalette (
    VOID )
{
    CNavBitmapManager::NavRealizePalette ( m_hWnd );
    m_bRealize   = FALSE;
    m_bCanRender = TRUE;

    return ( FALSE );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

VOID WINAPI CNavBitmapManager::OnPaletteIsChanging (
    VOID )
{
    m_bCanRender = FALSE;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

VOID WINAPI CNavBitmapManager::OnPaletteChanged (
    WPARAM wParam )
{
    if ( (HWND)wParam != m_hWnd )
        {
        CNavBitmapManager::NavRealizePalette ( m_hWnd );
        m_bRealize = FALSE;
        }

    m_bCanRender = TRUE;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

HNAVBITMAP WINAPI CNavBitmapManager::Create (
    HINSTANCE   hInstance,
    HWND        hDlg,
    WORD        wBitmapID )
{
    auto    BOOL            bError      = FALSE;
    auto    LPNAVBITMAP     lpNavBitmap = NULL;
    auto    HNAVBITMAP      hNavBitmap  = NULL;

    SYM_ASSERT ( hInstance );
    SYM_ASSERT ( hDlg );
    SYM_ASSERT ( wBitmapID );

    if ( m_hPalette == NULL )
        NavCreatePalette ( hInstance, hDlg, wBitmapID );

    if ( NavRealizePalette( hDlg ) == TRUE )
        {
        lpNavBitmap = new CNavBitmap ( hInstance, wBitmapID );

        if ( lpNavBitmap )
            {
            HDC hDC = GetDC ( hDlg );

            if ( hDC && lpNavBitmap->Create ( m_hPalette, hDC ) == TRUE )
                {
                hNavBitmap = lpNavBitmap;
                }
            else
                {
                delete ( lpNavBitmap );
                }

            if ( hDC )
                ReleaseDC ( hDlg, hDC );
            }
        }

    return ( hNavBitmap );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

VOID WINAPI CNavBitmapManager::NavCreatePalette (
    HINSTANCE   hInstance,
    HWND        hDlg,
    WORD        wBitmapID )
{
    SYM_ASSERT ( hInstance );
    SYM_ASSERT ( hDlg );
    SYM_ASSERT ( wBitmapID );

    HDC hDC = GetDC( hDlg );

    if ( hDC )
        {
        HRSRC hRes = FindResource (
                                  hInstance,
                                  MAKEINTRESOURCE ( wBitmapID ),
                                  RT_BITMAP
                                  );
        if ( hRes )
            {
            HGLOBAL hDIB = LoadResource( hInstance, hRes );

            if ( hDIB )
                {
                LPBITMAPINFOHEADER lpbmInfoHdr =
                                (LPBITMAPINFOHEADER) LockResource ( hDIB );

                if ( lpbmInfoHdr && DIBNumColors( lpbmInfoHdr ) >= 256 )
                    {
                    CreateDIBPalette( hDIB );
                    }

                FreeResource( hDIB );
                }
            }

        ReleaseDC( hDlg, hDC );
        }
}

//---------------------------------------------------------------------
//
// Function:   CNavBmp::CreateDIBPalette
//
// Purpose:    Given a handle to a DIB, constructs a logical palette,
//             and returns a handle to this palette.
//
//             Stolen almost verbatim from ShowDIB.
//
// Parms:      hDIB == HANDLE to global memory with a DIB header
//                     (either BITMAPINFOHEADER or BITMAPCOREHEADER)
//---------------------------------------------------------------------

HPALETTE CNavBitmapManager::CreateDIBPalette (
    HANDLE  hDIB )
{
    auto    LPLOGPALETTE        lpPal;
    auto    int                 wNumColors;
    auto    LPBITMAPINFOHEADER  lpbi;
    auto    LPBITMAPINFO        lpbmi;
    auto    LPBITMAPCOREINFO    lpbmc;

    SYM_ASSERT ( hDIB );

    if ( m_hPalette == 0 )
        {
        lpbi         = (LPBITMAPINFOHEADER)LockResource (hDIB);
        lpbmi        = (LPBITMAPINFO) lpbi;
        lpbmc        = (LPBITMAPCOREINFO) lpbi;
        wNumColors   = DIBNumColors (lpbi);

        if ( wNumColors )
            {
            lpPal = (LPLOGPALETTE)MemAllocPtr (
                                          GHND,
                                          sizeof (LOGPALETTE) +
                                          sizeof (PALETTEENTRY) * wNumColors
                                          );
            if ( lpPal )
                {
                lpPal->palVersion    = 0x300;
                lpPal->palNumEntries = wNumColors;

                for ( WORD i = 0;  i < wNumColors;  i++ )
                    {
                    lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
                    lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
                    lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
                    lpPal->palPalEntry[i].peFlags = 0;
                    }

                m_hPalette = CreatePalette (lpPal);

                MemFreePtr ( lpPal );
                }
            }
        }

   return ( m_hPalette );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BOOL WINAPI CNavBitmapManager::NavRealizePalette (
    HWND    hDlg )
{
    auto    BOOL    bStatus = TRUE;

    if ( m_hPalette )
        {
        HDC hDC = GetDC( hDlg );

        if ( hDC )
            {
            bStatus = CNavBitmapManager::NavRealizePalette( hDC );

            if ( bStatus == TRUE && m_uEntriesChanged )
                InvalidateRect ( hDlg, NULL, TRUE );

            ReleaseDC( hDlg, hDC );
            }
        }

    return ( bStatus );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BOOL WINAPI CNavBitmapManager::NavRealizePalette (
    HDC     hDC )
{
    auto    BOOL    bStatus = TRUE;

    if ( m_hPalette )
        {
        SYM_ASSERT ( hDC );

        HPALETTE hOldPal = SelectPalette (
                                         hDC,
                                         m_hPalette,
                                         FALSE
                                         );

        SetSystemPaletteUse ( hDC, SYSPAL_STATIC );
        
        m_uEntriesChanged = RealizePalette( hDC );

        SelectPalette( hDC, hOldPal, FALSE );
        }
    else
        {
        m_uEntriesChanged = 0;
        }

    return ( bStatus );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

BOOL WINAPI CNavBitmapManager::Render (
    HNAVBITMAP  hNavBitmap,
    HDC         hDC,                // [in] device context
    LPRECT      lpRect,             // [in] rectangle where to paint
    BOOL        bStretch )          // [in] stretch it
{
    SYM_ASSERT ( hNavBitmap );
    SYM_ASSERT ( hDC );
    SYM_ASSERT ( lpRect );

    auto    LPNAVBITMAP     lpNavBitmap = hNavBitmap;
    auto    BOOL            bSuccess    = FALSE;

                                        // -----------------------------------
                                        // We can't render if application
                                        // doesn't have input focus and
                                        // some other app changed the palette
                                        // -----------------------------------

    if ( m_bCanRender == TRUE && NavRealizePalette( hDC ) == TRUE )
        {
        bSuccess = lpNavBitmap->Render ( hDC, lpRect, bStretch );
        }

    return ( bSuccess );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

VOID WINAPI CNavBitmapManager::Destroy (
    HNAVBITMAP  hNavBitmap )
{
    SYM_ASSERT ( hNavBitmap );
 
    auto    LPNAVBITMAP     lpNavBitmap = hNavBitmap;

    if ( lpNavBitmap )
        delete ( lpNavBitmap );
}
