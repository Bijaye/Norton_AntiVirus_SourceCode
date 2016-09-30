#ifndef _NAVX_BITMAP_
#define _NAVX_BITMAP_

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CNavBitmap
{
public:
    CNavBitmap (
        HINSTANCE   hInstance,
        WORD        wBitmapID
        );

    ~CNavBitmap();

    BOOL WINAPI Create (
        HPALETTE    hPal,
        HDC         hDC
        );

    BOOL WINAPI Render (
        HDC         hDC,                // [in] device context
        LPRECT      lpRect,             // [in] rectangle where to paint
        BOOL        bStretch            // [in] stretch it
        );

    VOID WINAPI Destroy (
        VOID
        );

    DWORD WINAPI GetNumColors (
        VOID
        );

private:
    WORD PaletteSize (
        LPBITMAPINFOHEADER   lpbi
        );

private:
    HBITMAP     m_hBitmap;
    HINSTANCE   m_hInstance;
    WORD        m_wBitmapID;
    WORD        m_wNumColors;
};

typedef class CNavBitmap FAR * LPNAVBITMAP;
typedef class CNavBitmap FAR * HNAVBITMAP;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CNavBitmapManager
{
public:
    CNavBitmapManager ( HWND hAppTopWindow );
   ~CNavBitmapManager ();

    BOOL WINAPI OnQueryNewPalette (
        VOID
        );

    VOID WINAPI OnPaletteIsChanging (
        VOID
        );

    VOID WINAPI OnPaletteChanged (
        WPARAM      wParam
        );

    HNAVBITMAP WINAPI Create (
        HINSTANCE   hInstance,
        HWND        hDlg,
        WORD        wBitmapID
        );

    BOOL WINAPI Render (
        HNAVBITMAP  hNavBitmap,
        HDC         hDC,                // [in] device context
        LPRECT      lpRect,             // [in] rectangle where to paint
        BOOL        bStretch            // [in] stretch it
        );

    VOID WINAPI Destroy (
        HNAVBITMAP  hBitmap
        );

private:

    WORD WINAPI PaletteSize (
        LPBITMAPINFOHEADER   lpbi
        );

    BOOL WINAPI NavRealizePalette (
        HWND        hDlg
        );

    BOOL WINAPI NavRealizePalette (
        HDC         hDC
        );

    VOID WINAPI NavCreatePalette (
        HINSTANCE   hInstance,
        HWND        hDlg,
        WORD        wBitmapID
        );

    HPALETTE CreateDIBPalette (
        HANDLE      hDIB
        );

private:
    HWND            m_hWnd;
    BOOL            m_bRealize;
    BOOL            m_bCanRender;
    HPALETTE        m_hPalette;
    UINT            m_uEntriesChanged;

};

typedef class CNavBitmapManager FAR * LPNAVBITMAPMANAGER;

#endif // _NAVX_BITMAP_
