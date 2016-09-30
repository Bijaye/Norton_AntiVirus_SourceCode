///////////////////////////////////////////////////////////////////////////
// $Header:   S:/CDSTART/VCS/splash.h_v   1.0   29 Jul 1998 14:57:32   DBuches  $
///////////////////////////////////////////////////////////////////////////
//
// splash.h - contains declaration for Splash window class
//
//
//
///////////////////////////////////////////////////////////////////////////
// $Log:   S:/CDSTART/VCS/splash.h_v  $
// 
//    Rev 1.0   29 Jul 1998 14:57:32   DBuches
// Initial revision.
///////////////////////////////////////////////////////////////////////////

#ifndef _SPLASH_H_
#define _SPLASH_H_

///////////////////////////////////////////////////////////////////////////
//
// CSplashWindow
//
///////////////////////////////////////////////////////////////////////////

class CSplashWindow
{
public:
    // Construction.
    CSplashWindow( HINSTANCE hInst, HWND hWndParent, int idHiColorBMP, int idLoColorBMP );
    ~CSplashWindow();


private:
    // Methods
    BOOL Create( int idHiColorBMP, int idLoColorBMP );
    DWORD DisplayableColors();
    void Destroy();
    static LRESULT CALLBACK SplashScreenProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
    // Handle to our window.
    HWND        m_hWnd;

    // Handle of parent window.
    HWND        m_hWndParent;

    // Handle to bitmap being displayed.
    HBITMAP     m_hBitmap;

    // Handle to palette to use for bitmap.
    HPALETTE    m_hPal;

    // Instance handle
    HINSTANCE   m_hInst;

};

///////////////////////////////////////////////////////////////////////////
#define SPLASH_CLASS_NAME       _T("SplashClass")
///////////////////////////////////////////////////////////////////////////

#endif
