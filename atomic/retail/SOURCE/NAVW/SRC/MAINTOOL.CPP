// Copyright 1998 Symantec Corporation. 
//*************************************************************************
//
// $Header:   S:/NAVW/VCS/maintool.cpv   1.0   01 Apr 1998 14:43:06   DALLEE  $
//
// Description:
//
// Contains:
//      All CNavMainToolBarCtrl methods.
//*************************************************************************
// $Log:   S:/NAVW/VCS/maintool.cpv  $
// 
//    Rev 1.0   01 Apr 1998 14:43:06   DALLEE
// Initial revision.
//*************************************************************************

#include "platform.h"
#include "MainTool.h"
#include "navwres.h"


//*************************************************************************
// CNavMainToolBarCtrl::CNavMainToolBarCtrl()
//
// public
// CNavMainToolBarCtrl::CNavMainToolBarCtrl( )
//
// Construct CNavMainToolBarCtrl() - NAV's main dialog toolbar.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/30/1998 DALLEE, created - header added.
//*************************************************************************

CNavMainToolBarCtrl::CNavMainToolBarCtrl()
{
    m_hNormalImageList = NULL;
    m_hHotImageList    = NULL;

    m_sizeImage.cx = 16;
    m_sizeImage.cy = 15;
} // CNavMainToolBarCtrl::CNavMainToolBarCtrl()


//*************************************************************************
// CNavMainToolBarCtrl::~CNavMainToolBarCtrl()
//
// public virtual
// CNavMainToolBarCtrl::~CNavMainToolBarCtrl( )
//
// Destructor for CNavMainToolBarCtrl() - NAV's main dialog toolbar.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/30/1998 DALLEE, created - header added.
//*************************************************************************

CNavMainToolBarCtrl::~CNavMainToolBarCtrl()
{
    DestroyImageLists();
} // CNavMainToolBarCtrl::~CNavMainToolBarCtrl()


//*************************************************************************
// CNavMainToolBarCtrl::SetBitmapSize()
//
// public
// BOOL CNavMainToolBarCtrl::SetBitmapSize(
//      int nWidth,                     [in] width in pixels of bitmaps
//      int nHeight )                   [in] height in pixels of bitmaps
//
// This method sets the size of the bitmaps to be added to the toolbar.
// Can only be called before adding bitmaps to the toolbar. If not set,
// the default size is 16 pixels wide by 15 pixels high.
//
// Override of the base CNVToolBarCtrl() method in order to track
// size of bitmap for image list creation.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNavMainToolBarCtrl::SetBitmapSize( int nWidth, int nHeight )
{
    auto    BOOL    bSuccess;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    bSuccess = CNVToolBarCtrl::SetBitmapSize( nWidth, nHeight );

    if ( bSuccess )
    {
        m_sizeImage.cx = nWidth;
        m_sizeImage.cy = nHeight;
    }

    return ( bSuccess );
} // CNavMainToolBarCtrl::SetBitmapSize()


//*************************************************************************
// CNavMainToolBarCtrl::AddButton()
//
// public
// int CNavMainToolBarCtrl::AddButton(
//      LPNAVTBBUTTON lpButton )
//
// This method adds a button to NAV's toolbar.
//
// Returns: 
//      Zero-based index of new button on success. Otherwise, -1.
//*************************************************************************
// 03/30/1998 DALLEE, created - header added.
//*************************************************************************

int CNavMainToolBarCtrl::AddButton( LPNAVTBBUTTON lpButton )
{
    #define MAX_BUTTON_TEXT     80

    auto    TBBUTTON    rTBButton;
    auto    HBITMAP     hNormalBitmap;
    auto    HBITMAP     hHotBitmap;

    auto    TCHAR       szButtonText[ MAX_BUTTON_TEXT ];

    auto    int         nIndex;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpButton );
    SYM_ASSERT( lpButton->hInst );

    // Set up locals

    nIndex          = -1;
    hNormalBitmap   = NULL;
    hHotBitmap      = NULL;
    *szButtonText   = _T('\0');

    // Make sure we have our toolbar and we have our own image lists
    // to play with

    if ( NULL == m_hWnd )
    {
        goto BailOut;
    }

    if ( ( NULL == m_hNormalImageList ) &&
         ( FALSE == CreateImageLists() ) )
    {
        goto BailOut;
    }

    // Set up Windows TBBUTTON struct

    memset( &rTBButton, 0, sizeof( rTBButton ) );

    rTBButton.idCommand = lpButton->uID;
    rTBButton.fsState   = TBSTATE_ENABLED;
    rTBButton.fsStyle   = TBSTYLE_BUTTON;
    rTBButton.dwData    = lpButton->dwUserData;

    // Load string and terminate with two null characters.
    // Note we're not using the AddString() which takes a string ID --
    // the underlying TB_ADDSTRING message seems to be broken.
    // If it does work for anyone, please let me know. Thanks, DALLEE.

    LoadString( lpButton->hInst,
                lpButton->uString,
                szButtonText,
                ( sizeof( szButtonText ) / sizeof( *szButtonText ) ) - 1 );

    szButtonText[ _tcslen( szButtonText ) ] = _T('\0');

    rTBButton.iString = AddStrings( szButtonText );

    // Load images

    hNormalBitmap = (HBITMAP) LoadImage( lpButton->hInst,
                                         MAKEINTRESOURCE( lpButton->uBitmap ),
                                         IMAGE_BITMAP,
                                         0, 0,
                                         LR_DEFAULTCOLOR );

    if ( NULL == hNormalBitmap )
        goto BailOut;

    hHotBitmap    = (HBITMAP) LoadImage( lpButton->hInst,
        MAKEINTRESOURCE( lpButton->uHotBitmap ? lpButton->uHotBitmap : lpButton->uBitmap ),
                                         IMAGE_BITMAP,
                                         0, 0,
                                         LR_DEFAULTCOLOR );

    if ( NULL == hHotBitmap )
        goto BailOut;

    rTBButton.iBitmap = ImageList_AddMasked( m_hNormalImageList,
                                             hNormalBitmap,
                                             RGB( 255, 0, 255 ) );

    ImageList_AddMasked( m_hHotImageList,
                         hHotBitmap ,
                         RGB( 255, 0, 255 ) );

    // Add button to toolbar

    nIndex = AddButtons( 1, &rTBButton );

BailOut:
    if ( hNormalBitmap )
        DeleteObject( hNormalBitmap );

    if ( hHotBitmap )
        DeleteObject( hHotBitmap );

    return ( nIndex );
} // CNavMainToolBarCtrl::AddButton()


//*************************************************************************
// CNavMainToolBarCtrl::CreateImageLists()
//
// protected
// BOOL CNavMainToolBarCtrl::CreateImageLists( )
//
// This method creates and assigns the normal and hot image lists to be used
// by NAV's toolbar control.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/30/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNavMainToolBarCtrl::CreateImageLists()
{
    auto    BOOL        bSuccess;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( NULL == m_hNormalImageList );
    SYM_ASSERT( NULL == m_hHotImageList );

    m_hNormalImageList = ImageList_Create( m_sizeImage.cx, m_sizeImage.cy, ILC_COLOR4 | ILC_MASK, 1, 1 );
    m_hHotImageList    = ImageList_Create( m_sizeImage.cx, m_sizeImage.cy, ILC_COLOR4 | ILC_MASK, 1, 1 );

    if ( m_hNormalImageList && m_hHotImageList )
    {
        SetImageList( m_hNormalImageList );
        SetHotImageList( m_hHotImageList );

        bSuccess = TRUE;
    }
    else
    {
        DestroyImageLists();
        bSuccess = FALSE;
    }

    return ( bSuccess );
} // // CNavMainToolBarCtrl::CreateImageLists()


//*************************************************************************
// CNavMainToolBarCtrl::DestroyImageLists()
//
// protected
// void CNavMainToolBarCtrl::DestroyImageLists( )
//
// This method destroys the image lists created for NAV's toolbar, if any.
//
// Returns:
//      Nothing.
//*************************************************************************
// 03/30/1998 DALLEE, created - header added.
//*************************************************************************

void CNavMainToolBarCtrl::DestroyImageLists()
{
    if ( m_hNormalImageList )
    {
        ImageList_Destroy( m_hNormalImageList );
        m_hNormalImageList = NULL;
    }

    if ( m_hHotImageList )
    {
        ImageList_Destroy( m_hHotImageList );
        m_hHotImageList    = NULL;
    }
} // // CNavMainToolBarCtrl::DestroyImageLists()

