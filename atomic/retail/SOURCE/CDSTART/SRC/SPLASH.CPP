///////////////////////////////////////////////////////////////////////////
// $Header:   S:/CDSTART/VCS/splash.cpv   1.0   29 Jul 1998 14:57:30   DBuches  $
///////////////////////////////////////////////////////////////////////////
//
// splash.cpp - contains implementation for Splash window class
//
//
//
///////////////////////////////////////////////////////////////////////////
// $Log:   S:/CDSTART/VCS/splash.cpv  $
// 
//    Rev 1.0   29 Jul 1998 14:57:30   DBuches
// Initial revision.
///////////////////////////////////////////////////////////////////////////


#include "platform.h"
#include "splash.h"
#include "cdstart.h"

///////////////////////////////////////////////////////////////////////////
//
// CSplashWindow::CSplashWindow()
//
// Description: Constructor.
//
//
///////////////////////////////////////////////////////////////////////////
// 7/29/1998 DBUCHES - Function created
///////////////////////////////////////////////////////////////////////////

CSplashWindow::CSplashWindow( HINSTANCE hInst, HWND hWndParent, int idHiColorBMP, int idLoColorBMP )
{
    //
    // Set some defaults
    //
    m_hBitmap = NULL;
    m_hPal = NULL;
    m_hWndParent = NULL;

    //
    // Save off instance handle
    //
    m_hInst = hInst;

    //
    // Register our class
    //
    WNDCLASSEX  wcex;

    ZeroMemory( &wcex, sizeof( WNDCLASSEX ) );

    wcex.cbSize         = sizeof( wcex );
    wcex.style          = 0;//CS_HREDRAW | CS_VREDRAW;//0;
    wcex.lpfnWndProc    = CSplashWindow::SplashScreenProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = m_hInst;
    wcex.hCursor        = NULL;
    wcex.hbrBackground  = ( HBRUSH )( COLOR_BTNFACE + 1 );
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = SPLASH_CLASS_NAME;
    wcex.hIconSm        = NULL;
    wcex.hIcon          = NULL;

    //
    // Register and create our window.
    //
    if( RegisterClassEx( &wcex ) )
        {
        Create( idHiColorBMP, idLoColorBMP );
        }

}

///////////////////////////////////////////////////////////////////////////
//
// CSplashWindow::CSplashWindow()
//
// Description: Destructor.
//
///////////////////////////////////////////////////////////////////////////
// 7/29/1998 DBUCHES - Function created
///////////////////////////////////////////////////////////////////////////

CSplashWindow::~CSplashWindow()
{
    //
    // Destroy our window.
    //
    Destroy();

    //
    // Unregister our class
    //

    UnregisterClass( SPLASH_CLASS_NAME, m_hInst );

    //
    // Free resources.
    //
    if( m_hBitmap )
        {
        DeleteObject( m_hBitmap );
        m_hBitmap = NULL;
        }
    if( m_hPal )
        {
        DeleteObject( m_hPal );
        m_hPal = NULL;
        }
}

///////////////////////////////////////////////////////////////////////////
//
// CSplashWindow::Create()
//
// Description: Creates our splash window.
//
///////////////////////////////////////////////////////////////////////////
// 7/29/1998 DBUCHES - Function created
///////////////////////////////////////////////////////////////////////////

BOOL CSplashWindow::Create( int idHiColorBMP, int idLoColorBMP )
{
    //
    // Save off parent window.
    //
    m_hWndParent = GetDesktopWindow();

    //
    // Load the proper bitmap.
    //
    m_hBitmap = LoadResourceBitmap( m_hInst, ( DisplayableColors() >= 256 ) ?
                                   MAKEINTRESOURCE( idHiColorBMP ) : MAKEINTRESOURCE( idLoColorBMP ) , &m_hPal );

    if( m_hBitmap == NULL )
        {
        return FALSE;
        }

    //
    // Get bitmap dimensions
    //
    BITMAP bm;
    ZeroMemory( &bm, sizeof( BITMAP ) );
    GetObject( m_hBitmap, sizeof(BITMAP), (LPTSTR)&bm );

    //
    // Compute location of window.
    //
    RECT rectParent;
    GetClientRect( m_hWndParent, &rectParent );

    int xPos = (rectParent.right - rectParent.left - bm.bmWidth ) / 2;
    int yPos = (rectParent.bottom - rectParent.top - bm.bmHeight ) / 2;

    //
    // Create our window.
    //
    m_hWnd = CreateWindow( SPLASH_CLASS_NAME,
                           _T( "Splash" ),
                           WS_POPUP,
                           xPos,
                           yPos,
                           bm.bmWidth,
                           bm.bmHeight,
                           m_hWndParent,
                           (HMENU) 0,
                           m_hInst,
                           NULL );

    if( m_hWnd == NULL )
        {
        return FALSE;
        }

    //
    // Set this pointer for window
    //
    SetWindowLong( m_hWnd, GWL_USERDATA, (LPARAM) this );


    //
    // Display the window.
    //
    ShowWindow( m_hWnd, SW_SHOWNORMAL );


    //
    // Bring this window to the top
    //
    SetWindowPos( m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );


    //
    // Everything looks good.
    //
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//
// CSplashWindow::Destroy()
//
// Description: Kills our window.
//
///////////////////////////////////////////////////////////////////////////
// 7/29/1998 DBUCHES - Function created
///////////////////////////////////////////////////////////////////////////

void CSplashWindow::Destroy()
{
    if( m_hWnd )
        {
        DestroyWindow( m_hWnd );
        m_hWnd = NULL;
        }
}

///////////////////////////////////////////////////////////////////////////
//
// CSplashWindow::DisplayableColors()
//
// Description: Returns the number of displayable colors.
//
///////////////////////////////////////////////////////////////////////////
// 7/29/1998 DBUCHES - Function created
///////////////////////////////////////////////////////////////////////////

DWORD CSplashWindow::DisplayableColors()
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
}

///////////////////////////////////////////////////////////////////////////
//
// CSplashWindow::SplashScreenProc()
//
// Description: Window proc for splash screens.
//
///////////////////////////////////////////////////////////////////////////
// 7/29/1998 DBUCHES - Function created
///////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK CSplashWindow::SplashScreenProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    //
    // Get this pointer
    //
    CSplashWindow* pThis = (CSplashWindow*) GetWindowLong( hWnd, GWL_USERDATA );

    switch( uMsg )
        {
        case WM_LBUTTONDOWN:
            //
            // Hide window if user clicks on it
            //
            ShowWindow( hWnd, SW_HIDE );
            break;


        case WM_PAINT:
            {
            HDC             hdc;
            RECT            rect;
            PAINTSTRUCT     ps;

            BeginPaint(hWnd, &ps);
            hdc = ps.hdc;

            GetClientRect(hWnd, &rect);

            NCD_DrawBitmap(hdc, &rect, pThis->m_hBitmap, pThis->m_hPal, TRUE);

            EndPaint(hWnd, &ps);
            return(FALSE);
            }

                        // Someone has realized their palette
        case WM_PALETTECHANGED:
            {
            HDC         hdc;
            HPALETTE    hOldPal;

            if ((HWND)wParam != hWnd && pThis->m_hPal != NULL)
                {
                hdc = GetDC(hWnd);
                hOldPal = SelectPalette(hdc, pThis->m_hPal, FALSE);
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

            if (pThis->m_hPal != NULL)
                {
                hdc = GetDC(hWnd);

                hOldPal = SelectPalette(hdc, pThis->m_hPal, FALSE);
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
        }

    return( DefWindowProc( hWnd, uMsg, wParam, lParam ) );
}

