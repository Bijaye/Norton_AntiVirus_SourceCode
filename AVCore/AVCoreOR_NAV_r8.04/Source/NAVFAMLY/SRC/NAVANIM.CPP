#include "platform.h"
#include "stack.hpp"
#include "navgraph.hpp"                 // To draw bitmaps
#include "navanim.hpp"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

CNavAnimate::CNavAnimate ()
{
    m_lpBmpManager      =   NULL;
    m_lpBitmapHandles   =   NULL;
    m_uCurrentBitmap    =   0;
    m_bEnabled          =   TRUE;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

CNavAnimate::~CNavAnimate()
{

    if ( m_lpBmpManager && m_lpBitmapHandles )
        {
        BOOL        bStatus = FALSE;
        HNAVBITMAP  hBitmap = NULL;
        
        while ( (bStatus = m_lpBitmapHandles->Pop((LPSTR)&hBitmap)) == TRUE )
            {
            SYM_ASSERT ( hBitmap );

            m_lpBmpManager->Destroy ( hBitmap );
            }
        }

    if ( m_lpBitmapHandles )
        {
        delete ( m_lpBitmapHandles );
        m_lpBitmapHandles = NULL;
        }

    if ( m_lpBmpManager )
        {
        delete ( m_lpBmpManager );
        m_lpBmpManager = NULL;
        }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI CNavAnimate::Init (
    HINSTANCE   hInstance,
    HWND        hDlg,
    UINT        uBitmapIDArray[]
    )
{
    auto    BOOL    bSuccess = FALSE;

    m_lpBmpManager = new CNavBitmapManager ( hDlg );

    if ( m_lpBmpManager )
        {
        m_lpBitmapHandles = new CStackObjects ( sizeof(HNAVBITMAP) );

        if ( m_lpBitmapHandles )
            {
            for ( UINT uIndex = 0; uBitmapIDArray[uIndex]; uIndex++ )
                {
                HNAVBITMAP hBitmap = m_lpBmpManager->Create (
                                                hInstance,
                                                hDlg,
                                                uBitmapIDArray[uIndex]
                                                );
                if ( hBitmap )
                    {
                    m_lpBitmapHandles->Push ( (LPSTR)&hBitmap );
                    }
                else
                    {
                    break;
                    }
                }

            if ( !uBitmapIDArray[uIndex] )
                {
                bSuccess = TRUE;
                }
            }
        }

    return ( bSuccess );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI CNavAnimate::OnPaletteMessage (
    UINT    uMessage,
    WPARAM  wParam
    )
{
    auto    BOOL    bReturn = FALSE;

    if ( m_lpBmpManager )
        {
        switch ( uMessage )
            {
            case WM_QUERYNEWPALETTE:
                bReturn = m_lpBmpManager->OnQueryNewPalette ();
                break;

            case WM_PALETTEISCHANGING:
                m_lpBmpManager->OnPaletteIsChanging ();
                break;

            case WM_PALETTECHANGED:
                m_lpBmpManager->OnPaletteChanged ( wParam );
                break;
            }
        }

    return ( bReturn );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI CNavAnimate::OnDrawNextItem (
    LPDRAWITEMSTRUCT    lpDrawItem
    )
{
    CNavAnimate::OnDrawItem ( lpDrawItem, FALSE );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI CNavAnimate::OnDrawStretchNextItem (
    LPDRAWITEMSTRUCT    lpDrawItem
    )
{
    CNavAnimate::OnDrawItem ( lpDrawItem, TRUE );
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI CNavAnimate::OnDrawItem (
    LPDRAWITEMSTRUCT    lpDrawItem,
    BOOL                bStretch
    )
{
    SYM_ASSERT ( lpDrawItem );

    if ( m_lpBmpManager && m_lpBitmapHandles && lpDrawItem )
        {
        if ( lpDrawItem->itemAction == ODA_DRAWENTIRE ||
            (lpDrawItem->itemAction &  ODA_SELECT) )
            {
            BOOL        bStatus = FALSE;
            HNAVBITMAP  hBitmap = NULL;
        
            bStatus = m_lpBitmapHandles->GetObject(
                                                m_uCurrentBitmap,
                                                (LPSTR)&hBitmap
                                                );

            SYM_ASSERT ( bStatus == TRUE );
            SYM_ASSERT ( hBitmap );

            if ( bStatus == TRUE && hBitmap )
                {
                m_lpBmpManager->Render (
                                        hBitmap,
                                        lpDrawItem->hDC,
                                        &lpDrawItem->rcItem,
                                        bStretch
                                        );
                }

            if ( m_bEnabled == TRUE )
                {
                m_uCurrentBitmap ++;

                if ( m_uCurrentBitmap >= m_lpBitmapHandles->GetCount() )
                    m_uCurrentBitmap = 0;
                }
            }
        }
}

VOID WINAPI CNavAnimate::Enable(
    VOID
    )
{
    m_bEnabled = TRUE;
}

VOID WINAPI CNavAnimate::Disable (
    VOID
    )
{
    m_bEnabled = FALSE;
}

