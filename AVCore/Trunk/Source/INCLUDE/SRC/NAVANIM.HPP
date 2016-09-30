#ifndef _NAV_ANIM_
#define _NAV_ANIM_

class CNavAnimate
{
public:
    CNavAnimate ();
   ~CNavAnimate ();

    BOOL WINAPI Init (
        HINSTANCE           hInstance,
        HWND                hDlg,
        UINT                uBitmapIDArray[]
        );

    BOOL WINAPI OnPaletteMessage (
        UINT                uMessage,
        WPARAM              wParam
        );

    VOID WINAPI OnDrawNextItem (
        LPDRAWITEMSTRUCT    lpDrawItem
        );

    VOID WINAPI OnDrawStretchNextItem (
        LPDRAWITEMSTRUCT    lpDrawItem
        );

    VOID WINAPI Enable(
        VOID
        );

    VOID WINAPI Disable(
        VOID
        );

private:

    VOID WINAPI OnDrawItem (
        LPDRAWITEMSTRUCT    lpDrawItem,
        BOOL                bStretch
        );


    BOOL                m_bEnabled;
    UINT                m_uCurrentBitmap;
    LPSTACKOBJECT       m_lpBitmapHandles;
    LPNAVBITMAPMANAGER  m_lpBmpManager;
};

typedef class CNavAnimate FAR * LPNAVANIMATE;

#endif


