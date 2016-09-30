// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// NVReBar.cpp - created 03/25/1998 12:06:19 PM
//
// $Header:   S:/NAVW/VCS/toolbar.cpv   1.0   01 Apr 1998 14:43:26   DALLEE  $
//
// Description:
//      Implementation of CNVReBarCtrl   - Generic Non-MFC rebar control.
//                    and CNVToolBarCtrl - Generic toolbar control.
//
// Contains:
//      All CNVReBarCtrl methods.
//      All CNVToolBarCtrl methods.
//*************************************************************************
// $Log:   S:/NAVW/VCS/toolbar.cpv  $
// 
//    Rev 1.0   01 Apr 1998 14:43:26   DALLEE
// Initial revision.
//*************************************************************************

#include "platform.h"
#include "toolbar.h"


//*************************************************************************
// CNVReBarCtrl
//*************************************************************************

//*************************************************************************
// CNVReBarCtrl::CNVReBarCtrl()
//
// public
// CNVReBarCtrl::CNVReBarCtrl( )
//
// Constructor for CNVReBarCtrl - Non-MFC rebar control class.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

CNVReBarCtrl::CNVReBarCtrl()
{
    m_hWnd = NULL;
} // CNVReBarCtrl::CNVReBarCtrl()


//*************************************************************************
// CNVReBarCtrl::~CNVReBarCtrl()
//
// public virtual
// CNVReBarCtrl::~CNVReBarCtrl( )
//
// Destructor for CNVReBarCtrl - Non-MFC rebar control class.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

CNVReBarCtrl::~CNVReBarCtrl()
{
} // CNVReBarCtrl::~CNVReBarCtrl()


//*************************************************************************
// CNVReBarCtrl::Create()
//
// public
// BOOL CNVReBarCtrl::Create(
//      HINSTANCE  hInst,               [in] application instance
//      DWORD      dwStyle,             [in] win, commctrl, and rebar styles
//      LPCRECT    pRect,               [in] initial window position - may be NULL
//      HWND       hParent,             [in] parent window
//      UINT       uID )                [in] rebar control ID in parent
//
// This routine creates a rebar window and attaches it to the CNVReBarCtrl
// class.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVReBarCtrl::Create( HINSTANCE  hInst,
                           DWORD      dwStyle, 
                           LPCRECT    pRect, 
                           HWND       hParent, 
                           UINT       uID )
{
    auto    int     x, y, width, height;
    auto    BOOL    bSuccess;

    // Get initial position.

    if ( NULL != pRect )
    {
        x      = pRect->left;
        y      = pRect->top;
        width  = pRect->right - pRect->left;
        height = pRect->bottom - pRect->top;
    }
    else
    {
        x      = \
        y      = \
        width  = \
        height = 0;
    }

    // Create rebar.

    m_hWnd = ::CreateWindowEx( 0,
                               REBARCLASSNAME,
                               NULL,
                               dwStyle,
                               x, y,
                               width, height,
                               hParent,
                               reinterpret_cast<HMENU>( uID ),
                               hInst,
                               this );

    if ( m_hWnd )
    {
        bSuccess = TRUE;
    }
    else
    {
        bSuccess = FALSE;
    }

    return ( bSuccess );
} // CNVReBarCtrl::Create()


//*************************************************************************
// CNVReBarCtrl::operator HWND()
//
// public
// CNVReBarCtrl::operator HWND( ) const
//
// Conversion to HWND.
//
// Returns: 
//      Handle to associated rebar control.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

CNVReBarCtrl::operator HWND() const
{ 
    return ( m_hWnd );
} // CNVReBarCtrl::operator HWND()


//*************************************************************************
// CNVReBarCtrl::DestroyWindow()
//
// public virtual
// BOOL CNVReBarCtrl::DestroyWindow( )
//
// This method destroys the associated rebar window.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVReBarCtrl::DestroyWindow()
{
    auto    BOOL    bSuccess;

    bSuccess = FALSE;

    // Bail out early if no associated window.

    if ( NULL == m_hWnd )
    {
        goto BailOut;
    }

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    if ( ::DestroyWindow( m_hWnd ) )
    {
        m_hWnd   = NULL;
        bSuccess = TRUE;
    }

BailOut:
    return ( bSuccess );
} // CNVReBarCtrl::DestroyWindow()


//*************************************************************************
// CNVToolBarCtrl
//*************************************************************************

//*************************************************************************
// CNVToolBarCtrl::CNVToolBarCtrl()
//
// public
// CNVToolBarCtrl::CNVToolBarCtrl( )
//
// Constructor for CNVToolBarCtrl - Non-MFC toolbar control class.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

CNVToolBarCtrl::CNVToolBarCtrl()
{
    m_hWnd = NULL;
} // CNVToolBarCtrl::CNVToolBarCtrl()


//*************************************************************************
// CNVToolBarCtrl::~CNVToolBarCtrl()
//
// public virtual
// CNVToolBarCtrl::~CNVToolBarCtrl( )
//
// Destructor for CNVToolBarCtrl - Non-MFC toolbar control class.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

CNVToolBarCtrl::~CNVToolBarCtrl()
{
} // CNVToolBarCtrl::~CNVToolBarCtrl()


//*************************************************************************
// CNVToolBarCtrl::Create()
//
// public
// BOOL CNVToolBarCtrl::Create(
//      HINSTANCE  hInst,               [in] application instance
//      DWORD      dwStyle,             [in] win, commctrl, and toolbar styles
//      LPCRECT    pRect,               [in] initial window position - may be NULL
//      HWND       hParent,             [in] parent window
//      UINT       uID )                [in] toolbar control ID in parent
//
// This routine creates a toolbar window and attaches it to the CNVToolBarCtrl
// class. dwStyle must specify WM_CHILD. Other common style bits are CCS_TOP, 
// TBSTYLE_TOOLTIPS, or TBSTYLE_FLAT.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::Create( HINSTANCE  hInst,
                             DWORD      dwStyle, 
                             LPCRECT    pRect, 
                             HWND       hParent, 
                             UINT       uID )
{
    auto    int     x, y, width, height;
    auto    BOOL    bSuccess;

    SYM_ASSERT( WS_CHILD & dwStyle );

    // Get initial position.

    if ( NULL != pRect )
    {
        x      = pRect->left;
        y      = pRect->top;
        width  = pRect->right - pRect->left;
        height = pRect->bottom - pRect->top;
    }
    else
    {
        x      = \
        y      = \
        width  = \
        height = 0;
    }

    // Create toolbar.

    m_hWnd = ::CreateWindowEx( 0,
                               TOOLBARCLASSNAME,
                               NULL,
                               dwStyle,
                               x, y,
                               width, height,
                               hParent,
                               reinterpret_cast<HMENU>( uID ),
                               hInst,
                               this );

    if ( m_hWnd )
    {
        // Must initialize new toolbar control with button struct size.

        SetButtonStructSize();
        bSuccess = TRUE;
    }
    else
    {
        bSuccess = FALSE;
    }

    return ( bSuccess );
} // CNVToolBarCtrl::Create()


//*************************************************************************
// CNVToolBarCtrl::operator HWND()
//
// public
// CNVToolBarCtrl::operator HWND( ) const
//
// Conversion to HWND.
//
// Returns: 
//      Handle to associated toolbar control.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

CNVToolBarCtrl::operator HWND() const
{ 
    return ( m_hWnd );
} // CNVToolBarCtrl::operator HWND()


//*************************************************************************
// CNVToolBarCtrl::GetButtonCount()
//
// public
// int CNVToolBarCtrl::GetButtonCount( ) const
//
// Returns: 
//      The number of buttons in the toolbar.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::GetButtonCount() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_BUTTONCOUNT, 0, 0 ) );
} // CNVToolBarCtrl::GetButtonCount()


//*************************************************************************
// CNVToolBarCtrl::SetButtonStructSize()
//
// public
// void CNVToolBarCtrl::SetButtonStructSize(
//      int nSize = sizeof( TBBUTTON ) )    [in] size of struct TBBUTTON
//
// This method initializes the toolbar control with the size of the
// TBBUTTON structure. This must be done before adding either bitmaps or
// buttons.
// Note: This routine is automatically called by the Create() method; clients
// should not be required to call this routine.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

void CNVToolBarCtrl::SetButtonStructSize( int nSize )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    ::SendMessage( m_hWnd, TB_BUTTONSTRUCTSIZE, nSize, 0 );
} // CNVToolBarCtrl::SetButtonStructSize()


//*************************************************************************
// CNVToolBarCtrl::GetBitmap()
//
// public
// int CNVToolBarCtrl::GetBitmap(
//      int nID )  const                [in] command ID of button
//
// This method returns the index of the bitmap associated with the
// specified button.
//
// Returns: 
//      The zero-based index of the bitmap on success.
//      0 on error (ambiguous, but that's what Windows provides...)
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::GetBitmap( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_GETBITMAP, nID, 0 ) );
} // CNVToolBarCtrl::GetBitmap()


//*************************************************************************
// CNVToolBarCtrl::SetBitmap()
//
// public
// BOOL CNVToolBarCtrl::SetBitmap(
//      int nButtonID,                  [in] command ID of button
//      int nBitmapIndex )              [in] zero-based index of bitmap
//
// This method changes the bitmap associated with the specified button.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::SetBitmap( int nButtonID, int nBitmapIndex )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_CHANGEBITMAP, nButtonID, MAKELPARAM( nBitmapIndex, 0 ) ) );
} // CNVToolBarCtrl::SetBitmap()


//*************************************************************************
// CNVToolBarCtrl::GetBitmapFlags()
//
// public
// UINT CNVToolBarCtrl::GetBitmapFlags( ) const
//
// Gets the bitmap flags. Useful for determining size of bitmaps to display.
//
// Returns: 
//      TBBF_LARGE if the display supports large bitmaps -- width is at least
//      120 pixel per logical inch.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

UINT CNVToolBarCtrl::GetBitmapFlags() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_GETBITMAPFLAGS, 0, 0 ) );
} // CNVToolBarCtrl::GetBitmapFlags()


//*************************************************************************
// CNVToolBarCtrl::GetButton()
//
// public
// BOOL CNVToolBarCtrl::GetButton(
//      int         nIndex,             [in] zero-based index of button
//      LPTBBUTTON  lpButton )  const   [out] receives button information
//
// This method retrieves information about the specified button.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::GetButton( int nIndex, LPTBBUTTON lpButton ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpButton );

    return ( ::SendMessage( m_hWnd, TB_GETBUTTON, nIndex, reinterpret_cast<LPARAM>( lpButton ) ) );
} // CNVToolBarCtrl::GetButton()


//*************************************************************************
// CNVToolBarCtrl::GetButtonText()
//
// public
// int CNVToolBarCtrl::GetButtonText(
//      int     nID,                    [in] command ID of button
//      LPTSTR  lpszText )  const       [out] receives button text
//
// This method retrieves the string associated with the specified button.
//
// Returns: 
//      On success, the number of characters copied to lpszText, not including
//      the terminating null. On error, -1.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::GetButtonText( int nID, LPSTR lpszText ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpszText );

    return ( ::SendMessage( m_hWnd, TB_GETBUTTONTEXTA, nID, reinterpret_cast<LPARAM>( lpszText ) ) );
} // CNVToolBarCtrl::GetButtonText()

int CNVToolBarCtrl::GetButtonText( int nID, LPWSTR lpszText ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpszText );

    return ( ::SendMessage( m_hWnd, TB_GETBUTTONTEXTW, nID, reinterpret_cast<LPARAM>( lpszText ) ) );
} // CNVToolBarCtrl::GetButtonText()


//*************************************************************************
// CNVToolBarCtrl::GetItemRect()
//
// public
// BOOL CNVToolBarCtrl::GetItemRect(
//      int     nIndex,                 [in] index of button
//      LPRECT  lpRect ) const          [out] receives bounding rect of button
//
// This method retrieves the bounding rectangle of the specified button.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::GetItemRect( int nIndex, LPRECT lpRect ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpRect );

    return ( ::SendMessage( m_hWnd, TB_GETITEMRECT, nIndex, reinterpret_cast<LPARAM>( lpRect ) ) );
} // CNVToolBarCtrl::GetItemRect()


//*************************************************************************
// CNVToolBarCtrl::GetRows()
//
// public
// int CNVToolBarCtrl::GetRows( ) const
//
// The method retrives the number of rows in a wrappable toolbar.
//
// Returns: 
//      Number of rows in the toolbar.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::GetRows() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_GETROWS, 0, 0 ) );
} // CNVToolBarCtrl::GetRows()


//*************************************************************************
// CNVToolBarCtrl::SetRows()
//
// public
// void CNVToolBarCtrl::SetRows(
//      int     nRows,                  [in] number of rows
//      BOOL    bLarger,                [in] TRUE: create more rows on error
//      LPRECT  lpRect )                [out] new bounding rect of toolbar. May be NULL.
//
// This method sets the number of rows of buttons in the toolbar.
// Groups of buttons will not be separated. If the exact number of rows
// requested cannot be created, more or less rows will be created based on
// the setting of bLarger.
// 
// Returns:
//      Nothing.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

void CNVToolBarCtrl::SetRows( int nRows, BOOL bLarger, LPRECT lpRect )
{
    auto    RECT    rDummyRect;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    // Pass address of dummy rectangle, if caller doesn't care.

    if ( NULL == lpRect )
    {
        lpRect = &rDummyRect;
    }

    ::SendMessage( m_hWnd, TB_SETROWS, MAKEWPARAM( nRows, bLarger), reinterpret_cast<LPARAM>( lpRect ) );
} // // CNVToolBarCtrl::SetRows()


//*************************************************************************
// CNVToolBarCtrl::GetState()
//
// public
// int CNVToolBarCtrl::GetState(
//      int nID ) const                 [in] command ID of button
//
// This method retrieves the state information of the specified button.
// State information may contain a combination of the following flags:
//  TBSTATE_CHECKED         Has TBSTYLE_CHECKED style and is pressed
//  TBSTATE_ENABLED         Enabled and accepts input
//  TBSTATE_HIDDEN          Not visible, not enabled
//  TBSTATE_INDETERMINATE   Grayed - in indeterminate state
//  TBSTATE_PRESSED         Being pressed
//  TBSTATE_WRAP            Last enabled button before a row break in toolbar
//
// Returns: 
//      On success, button state flags. On error, -1.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::GetState( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_GETSTATE, nID, 0 ) );
} // CNVToolBarCtrl::GetState()


//*************************************************************************
// CNVToolBarCtrl::SetState()
//
// public
// BOOL CNVToolBarCtrl::SetState(
//      int nID,                        [in] command ID of button
//      int nState )                    [in] new state setting
//
// This method sets the state of the specified button.
// The state may be a combination of the following flags:
//  TBSTATE_CHECKED         Has TBSTYLE_CHECKED style and is pressed
//  TBSTATE_ENABLED         Enabled and accepts input
//  TBSTATE_HIDDEN          Not visible, not enabled
//  TBSTATE_INDETERMINATE   Grayed - in indeterminate state
//  TBSTATE_PRESSED         Being pressed
//  TBSTATE_WRAP            Last enabled button before a row break in toolbar
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/24/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::SetState( int nID, int nState )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_SETSTATE, nID, MAKELPARAM( nState, 0 ) ) );
} // CNVToolBarCtrl::SetState()


//*************************************************************************
// CNVToolBarCtrl::GetToolTips()
//
// public
// HWND CNVToolBarCtrl::GetToolTips( ) const
//
// This method returns the handle of the tooltip control associated with
// a toolbar.
//
// Returns: 
//      Window handle of tooltip control, or NULL if none.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HWND CNVToolBarCtrl::GetToolTips() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( reinterpret_cast<HWND>( ::SendMessage( m_hWnd, TB_GETTOOLTIPS, 0, 0 ) ) );
} // CNVToolBarCtrl::GetToolTips()


//*************************************************************************
// CNVToolBarCtrl::SetToolTips()
//
// public
// void CNVToolBarCtrl::SetToolTips(
//      HWND hToolTips )                [in] handle of new tooltip control
//
// This method associates a tooltip control with the toolbar.
// Note: Buttons added the the toolbar before calling this method will NOT
// be registered with the tooltip control.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

void CNVToolBarCtrl::SetToolTips( HWND hToolTips )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( ( NULL == hToolTips ) || ::IsWindow( hToolTips ) );

    ::SendMessage( m_hWnd, TB_GETTOOLTIPS, 0, 0 );
} // CNVToolBarCtrl::SetToolTips()


//*************************************************************************
// CNVToolBarCtrl::IsButtonChecked()
//
// public
// BOOL CNVToolBarCtrl::IsButtonChecked(
//      int nID ) const                 [in] command ID of button
//
// This method determines whether the specified button is checked --
// has TBSTYLE_CHECKED style and is pressed.
//
// Returns: 
//      TRUE if checked. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::IsButtonChecked( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_ISBUTTONCHECKED, nID, 0 ) );
} // CNVToolBarCtrl::IsButtonChecked()


//*************************************************************************
// CNVToolBarCtrl::IsButtonEnabled()
//
// public
// BOOL CNVToolBarCtrl::IsButtonEnabled(
//      int nID ) const                 [in] command ID of button
//
// This method determines whether the specified button is enabled.
//
// Returns: 
//      TRUE if enabled. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::IsButtonEnabled( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_ISBUTTONENABLED, nID, 0 ) );
} // CNVToolBarCtrl::IsButtonEnabled()


//*************************************************************************
// CNVToolBarCtrl::IsButtonHidden()
//
// public
// BOOL CNVToolBarCtrl::IsButtonHidden(
//      int nID ) const                 [in] command ID of button
//
// This method determines whether the specified button is hidden.
//
// Returns: 
//      TRUE if hidden. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::IsButtonHidden( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_ISBUTTONHIDDEN, nID, 0 ) );
} // CNVToolBarCtrl::IsButtonHidden()


//*************************************************************************
// CNVToolBarCtrl::IsButtonIndeterminate()
//
// public
// BOOL CNVToolBarCtrl::IsButtonIndeterminate(
//      int nID ) const                 [in] command ID of button
//
// This method determines whether the specified button is grayed.
//
// Returns: 
//      TRUE if grayed. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::IsButtonIndeterminate( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_ISBUTTONINDETERMINATE, nID, 0 ) );
} // CNVToolBarCtrl::IsButtonIndeterminate()


//*************************************************************************
// CNVToolBarCtrl::IsButtonPressed()
//
// public
// BOOL CNVToolBarCtrl::IsButtonPressed(
//      int nID ) const                 [in] command ID of button
//
// This method determines whether the specified button is pressed.
//
// Returns: 
//      TRUE if button is pressed. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::IsButtonPressed( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_ISBUTTONPRESSED, nID, 0 ) );
} // CNVToolBarCtrl::IsButtonPressed()


//*************************************************************************
// CNVToolBarCtrl::SetBitmapSize()
//
// public
// BOOL CNVToolBarCtrl::SetBitmapSize(
//      int nWidth,                     [in] width in pixels of bitmaps
//      int nHeight )                   [in] height in pixels of bitmaps
//
// This method sets the size of the bitmaps to be added to the toolbar.
// Can only be called before adding bitmaps to the toolbar. If not set,
// the default size is 16 pixels wide by 15 pixels high.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::SetBitmapSize( int nWidth, int nHeight )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_SETBITMAPSIZE, 0, MAKELPARAM( nWidth, nHeight ) ) );
} // CNVToolBarCtrl::SetBitmapSize()


//*************************************************************************
// CNVToolBarCtrl::SetButtonSize()
//
// public
// BOOL CNVToolBarCtrl::SetButtonSize(
//      int nWidth,                     [in] width of buttons
//      int nHeight )                   [in] height of buttons
//
// This method sets the size of the buttons to be added to the toolbar.
// Can only be called before adding buttons to the toolbar. If not set,
// the default size is 24 pixels wide by 22 pixels high.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::SetButtonSize( int nWidth, int nHeight )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_SETBUTTONSIZE, 0, MAKELPARAM( nWidth, nHeight ) ) );
} // CNVToolBarCtrl::SetButtonSize()


//*************************************************************************
// CNVToolBarCtrl::SetButtonWidth()
//
// BOOL CNVToolBarCtrl::SetButtonWidth(
//      int nWidth )
//
// This method sets the width of the toolbar buttons.
//
//&? Verify behavior of TB_SETBUTTONWIDTH
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::SetButtonWidth( int nWidth )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_SETBUTTONWIDTH, 0, MAKELPARAM( nWidth, 0 ) ) );
} // CNVToolBarCtrl::SetButtonWidth()


//*************************************************************************
// CNVToolBarCtrl::SetCmdID()
//
// public
// BOOL CNVToolBarCtrl::SetCmdID(
//      int nIndex,                     [in] zero-based index of button
//      int nID )                       [in] new command ID
//
// This method assigns a new command ID to the specified button.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::SetCmdID( int nIndex, int nID )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_SETCMDID, nIndex, nID ) );
} // CNVToolBarCtrl::SetCmdID()


//*************************************************************************
// CNVToolBarCtrl::SetParent()
//
// public
// HWND CNVToolBarCtrl::SetParent(
//      HWND hParent )                  [in] new notification window
//
// This method changes the window which receives the toolbar's notification
// messages. 
// Note: The actual parent window (as reported by GetParent()) is
// not changed.
//
// Returns: 
//      Handle of previous notification window. NULL, if none.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HWND CNVToolBarCtrl::SetParent( HWND hParent )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( ::IsWindow( hParent ) );

    return ( reinterpret_cast<HWND>( ::SendMessage( m_hWnd, TB_SETPARENT, reinterpret_cast<WPARAM>( hParent ), 0 ) ) );
} // CNVToolBarCtrl::SetParent()


//*************************************************************************
// CNVToolBarCtrl::GetDisabledImageList()
//
// public
// HIMAGELIST CNVToolBarCtrl::GetDisabledImageList( )
//
// This method retrieves the handle of the disabled image list
// associated with a toolbar.
//
// Returns: 
//      Handle of disabled image list on success. Otherwise, NULL.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVToolBarCtrl::GetDisabledImageList()
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( reinterpret_cast<HIMAGELIST>( ::SendMessage( m_hWnd, TB_GETDISABLEDIMAGELIST, 0, 0 ) ) );
} // CNVToolBarCtrl::GetDisabledImageList()


//*************************************************************************
// CNVToolBarCtrl::SetDisabledImageList()
//
// public
// HIMAGELIST CNVToolBarCtrl::SetDisabledImageList(
//      HIMAGELIST hImageList )         [in] handle of new image list
//
// This method sets the disabled image list associated with the toolbar.
//
// Returns: 
//      Handle of the previous image list if any on success. Otherwise, NULL.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVToolBarCtrl::SetDisabledImageList( HIMAGELIST hImageList )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( reinterpret_cast<HIMAGELIST>( ::SendMessage( m_hWnd, TB_SETDISABLEDIMAGELIST, 0, reinterpret_cast<LPARAM>( hImageList ) ) ) );
} // CNVToolBarCtrl::SetDisabledImageList()


//*************************************************************************
// CNVToolBarCtrl::GetHotImageList()
//
// public
// HIMAGELIST CNVToolBarCtrl::GetHotImageList( )
//
// This method retrieves the handle of the hot image list associated with 
// a toolbar.
//
// Returns: 
//      Handle of hot image list on success. Otherwise, NULL.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVToolBarCtrl::GetHotImageList()
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( reinterpret_cast<HIMAGELIST>( ::SendMessage( m_hWnd, TB_GETHOTIMAGELIST, 0, 0 ) ) );
} // CNVToolBarCtrl::GetHotImageList()


//*************************************************************************
// CNVToolBarCtrl::SetHotImageList()
//
// public
// HIMAGELIST CNVToolBarCtrl::SetHotImageList(
//      HIMAGELIST hImageList )         [in] handle of new image list
//
// This method sets the hot image list associated with the toolbar.
//
// Returns: 
//      Handle of the previous image list if any on success. Otherwise, NULL.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVToolBarCtrl::SetHotImageList( HIMAGELIST hImageList )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( reinterpret_cast<HIMAGELIST>( ::SendMessage( m_hWnd, TB_SETHOTIMAGELIST, 0, reinterpret_cast<LPARAM>( hImageList ) ) ) );
} // CNVToolBarCtrl::SetHotImageList()


//*************************************************************************
// CNVToolBarCtrl::GetImageList()
//
// public
// HIMAGELIST CNVToolBarCtrl::GetImageList( )
//
// This method retrieves the handle of the image list associated with a toolbar.
//
// Returns: 
//      Handle of hot image list on success. Otherwise, NULL.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVToolBarCtrl::GetImageList()
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( reinterpret_cast<HIMAGELIST>( ::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0 ) ) );
} // CNVToolBarCtrl::GetImageList()


//*************************************************************************
// CNVToolBarCtrl::SetImageList()
//
// public
// HIMAGELIST CNVToolBarCtrl::SetImageList(
//      HIMAGELIST hImageList )         [in] handle of new image list
//
// This method sets the image list associated with the toolbar.
// Note: The standard method for setting the normal bitmaps to be displayed
// is with the AddBitmaps() method. The normal image list is handled
// automatically by the toolbar control.
//
// Returns: 
//      Handle of the previous image list if any on success. Otherwise, NULL.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVToolBarCtrl::SetImageList( HIMAGELIST hImageList )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( reinterpret_cast<HIMAGELIST>( ::SendMessage( m_hWnd, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>( hImageList ) ) ) );
} // CNVToolBarCtrl::SetImageList()


//*************************************************************************
// CNVToolBarCtrl::GetTextRows()
//
// public
// int GetTextRows( ) const
//
// This method returns the number of rows of text which can be displayed
// for each button.
//
//&? Verify behavior of TB_GETTEXTROWS
//
// Returns: 
//      Number of rows of text displayable per button.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::GetTextRows() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_GETTEXTROWS, 0, 0 ) );
} // CNVToolBarCtrl::GetTextRows()


//*************************************************************************
// CNVToolBarCtrl::SetMaxTextRows()
//
// void SetMaxTextRows(
//      int nRows )
//
// This method sets the maximum number of rows which can be displayed
// for button text.
//
//&? Verify behavior of TB_SETMAXTEXTROWS
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

void CNVToolBarCtrl::SetMaxTextRows( int nRows )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    ::SendMessage( m_hWnd, TB_SETMAXTEXTROWS, nRows, 0 );
} // CNVToolBarCtrl::SetMaxTextRows()


//*************************************************************************
// CNVToolBarCtrl::AddBitmap()
//
// public
// int CNVToolBarCtrl::AddBitmap(
//      int         nNumBitmaps,          [in] number of images in bitmap
//      HINSTANCE   hResInst,             [in] resource module instance
//      UINT        uID )                 [in] resource ID of bitmap with images
//
// This method adds one or more images to a toolbar.
//
// See MSDN help on TBADDBITMAP for information on adding standard Windows
// bitmaps to the toolbar.
//
// Returns: 
//      Zero-based index of first new image on success. On error, -1.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::AddBitmap( int nNumBitmaps, HINSTANCE hResInst, UINT uID )
{
    auto    TBADDBITMAP     rAddBitmap;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    rAddBitmap.hInst = hResInst;
    rAddBitmap.nID   = uID;

    return ( ::SendMessage( m_hWnd, TB_ADDBITMAP, nNumBitmaps, reinterpret_cast<LPARAM>( &rAddBitmap ) ) );
} // CNVToolBarCtrl::AddBitmap()


//*************************************************************************
// CNVToolBarCtrl::AddBitmap()
//
// public
// int CNVToolBarCtrl::AddBitmap(
//      int     nNumBitmaps,            [in] number of images in bitmap
//      HBITMAP hBitmap )               [in] handle of bitmap with images
//
// This method adds one or more images to a toolbar.
//
// Returns: 
//      Zero-based index of first new image on success. On error, -1.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::AddBitmap( int nNumBitmaps, HBITMAP hBitmap )
{
    auto    TBADDBITMAP     rAddBitmap;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( hBitmap );

    rAddBitmap.hInst = NULL;
    rAddBitmap.nID   = reinterpret_cast<UINT>( hBitmap );

    return ( ::SendMessage( m_hWnd, TB_ADDBITMAP, nNumBitmaps, reinterpret_cast<LPARAM>( &rAddBitmap ) ) );
} // CNVToolBarCtrl::AddBitmap()


//*************************************************************************
// CNVToolBarCtrl::AddButtons()
//
// public
// BOOL CNVToolBarCtrl::AddButtons(
//      int         nNumButtons,        [in] number of buttons to add
//      LPCTBBUTTON lpButtons )         [in] array of TBBUTTON structures
//
// This method adds one or more buttons to the toolbar.
//
// typedef struct _TBBUTTON {
//      int iBitmap;                    zero-based image index
//      int idCommand;                  command ID
//      BYTE fsState;                   state flags
//      BYTE fsStyle;                   style flags
//      DWORD dwData;                   application defined data
//      int iString;                    zero-based text index
// } TBBUTTON, NEAR* PTBBUTTON, FAR* LPTBBUTTON;
// typedef const TBBUTTON FAR* LPCTBBUTTON;
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::AddButtons( int nNumButtons, LPCTBBUTTON lpButtons )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpButtons );

    return ( ::SendMessage( m_hWnd, TB_ADDBUTTONS, nNumButtons, reinterpret_cast<LPARAM>( lpButtons ) ) );
} // CNVToolBarCtrl::AddButtons()


//*************************************************************************
// CNVToolBarCtrl::AddString()
//
// public
// int CNVToolBarCtrl::AddString(
//      HINSTANCE hResInst,             [in] resource module instance
//      UINT      uID )                 [in] string resource ID
//
// This method adds a new string to the toolbar's list of button text strings.
//
//&? TODO: Fix this so we don't have to compile separate UNICODE/ANSI?
//
// Returns: 
//      Zero-based index of new string on success. Otherwise, -1.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::AddString( HINSTANCE hResInst, UINT uID )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_ADDSTRING, reinterpret_cast<WPARAM>( hResInst ), MAKELPARAM( uID, 0 ) ) );
} // CNVToolBarCtrl::AddString()


//*************************************************************************
// CNVToolBarCtrl::AddStrings()
//
// public
// int CNVToolBarCtrl::AddStrings(
//      LPCTSTR lpszzStrings )          [in] list of strings to add
//
// This method adds one or more new strings to the toolbar's list of button
// text strings.
// Note: lpszzStrings is a list of one or more strings which must be 
// terminated by two null characters.
//
// Returns: 
//      Zero-based index of first new string on success. Otherwise, -1.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::AddStrings( LPCSTR lpszStrings )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpszStrings );

    return ( ::SendMessage( m_hWnd, TB_ADDSTRINGA, 0, reinterpret_cast<LPARAM>( lpszStrings ) ) );
} // CNVToolBarCtrl::AddStrings()

int CNVToolBarCtrl::AddStrings( LPCWSTR lpszStrings )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpszStrings );

    return ( ::SendMessage( m_hWnd, TB_ADDSTRINGW, 0, reinterpret_cast<LPARAM>( lpszStrings ) ) );
} // CNVToolBarCtrl::AddStrings()


//*************************************************************************
// CNVToolBarCtrl::AutoSize()
//
// public
// void CNVToolBarCtrl::AutoSize( )
//
// This method causes the toolbar to be resized. The toolbar may need to be
// resized after adding new button strings, the parent window resizes, or
// the button or bitmap size is changed.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

void CNVToolBarCtrl::AutoSize()
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    ::SendMessage( m_hWnd, TB_AUTOSIZE, 0, 0 );
} // CNVToolBarCtrl::AutoSize()


//*************************************************************************
// CNVToolBarCtrl::CommandToIndex()
//
// public
// int CNVToolBarCtrl::CommandToIndex(
//      int nID ) const                 [in] command id of button
//
// This method returns the zero-based index of the button with the
// specified command ID.
//
// Returns: 
//      Zero-based index of button on success.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

int CNVToolBarCtrl::CommandToIndex( int nID ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_COMMANDTOINDEX, nID, 0 ) );
} // CNVToolBarCtrl::CommandToIndex()


//*************************************************************************
// CNVToolBarCtrl::DeleteButton()
//
// public
// BOOL CNVToolBarCtrl::DeleteButton(
//      int nIndex )                       [in] zero-based index of button
//
// This routine removes the specified button from the toolbar.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::DeleteButton( int nIndex )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_DELETEBUTTON, nIndex, 0 ) );
} // CNVToolBarCtrl::DeleteButton()


//*************************************************************************
// CNVToolBarCtrl::EnableButton()
//
// public
// BOOL CNVToolBarCtrl::EnableButton(
//      int     nID,                    [in] command ID of button
//      BOOL    bEnable = TRUE )        [in] TRUE to enable, FALSE to disable
//
// This method enables or disables the specified button.
// Note: Multiple states can be set by using the SetState() method.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::EnableButton( int nID, BOOL bEnable )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_ENABLEBUTTON, nID, MAKELPARAM( bEnable, 0 ) ) );
} // CNVToolBarCtrl::EnableButton()


//*************************************************************************
// CNVToolBarCtrl::HideButton()
//
// public
// BOOL CNVToolBarCtrl::HideButton(
//      int     nID,                    [in] command ID of button
//      BOOL    bHide = TRUE )          [in] TRUE to hide, FALSE to show
//
// This method hides or shows the specified button.
// Note: Multiple states can be set by using the SetState() method.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::HideButton( int nID, BOOL bHide )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_HIDEBUTTON, nID, MAKELPARAM( bHide, 0 ) ) );
} // CNVToolBarCtrl::HideButton()


//*************************************************************************
// CNVToolBarCtrl::Indeterminate()
//
// public
// BOOL CNVToolBarCtrl::Indeterminate(
//      int     nID,                    [in] command ID of button
//      BOOL    bHide = TRUE )          [in] TRUE to gray, FALSE to ungray.
//
// This sets or unsets the indeterminate state of the specified button.
// Note: Multiple states can be set by using the SetState() method.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::Indeterminate( int nID, BOOL bIndeterminate )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_INDETERMINATE, nID, MAKELPARAM( bIndeterminate, 0 ) ) );
} // CNVToolBarCtrl::Indeterminate()


//*************************************************************************
// CNVToolBarCtrl::InsertButton()
//
// public
// BOOL CNVToolBarCtrl::InsertButton(
//      int         nIndex,             [in] zero-based insertion location
//      LPTBBUTTON  lpButton )          [in] new button information
//
// This method inserts a single button to the left of the specified button.
//
// typedef struct _TBBUTTON {
//      int iBitmap;                    zero-based image index
//      int idCommand;                  command ID
//      BYTE fsState;                   state flags
//      BYTE fsStyle;                   style flags
//      DWORD dwData;                   application defined data
//      int iString;                    zero-based text index
// } TBBUTTON, NEAR* PTBBUTTON, FAR* LPTBBUTTON;
// typedef const TBBUTTON FAR* LPCTBBUTTON;
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::InsertButton( int nIndex, LPTBBUTTON lpButton )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpButton );

    return ( ::SendMessage( m_hWnd, TB_INSERTBUTTON, nIndex, reinterpret_cast<LPARAM>( lpButton ) ) );
} // CNVToolBarCtrl::InsertButton()


//*************************************************************************
// CNVToolBarCtrl::PressButton()
//
// public
// BOOL CNVToolBarCtrl::PressButton(
//      int     nID,                    [in] command ID of button
//      BOOL    bPress = TRUE )         [in] TRUE to press button, FALSE to release
//
// This method sets or unsets the pressed state of the specified button.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::PressButton( int nID, BOOL bPress )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( ::SendMessage( m_hWnd, TB_PRESSBUTTON, nID, MAKELPARAM( bPress, 0 ) ) );
} // CNVToolBarCtrl::PressButton()


//*************************************************************************
// CNVToolBarCtrl::SaveState()
//
// public
// void CNVToolBarCtrl::SaveState(
//      HKEY    hKeyRoot,               [in] registry key
//      LPCTSTR lpszSubKey,             [in] registry subkey name
//      LPCTSTR lpszValue )             [in] registry value name
//
// This method saves the current state of the toolbar to the specified
// value of the specified registry key.
// The state can later be restored by a call to RestoreState().
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

void CNVToolBarCtrl::SaveState( HKEY hKeyRoot, LPCSTR lpszSubKey, LPCSTR lpszValue )
{
    auto    TBSAVEPARAMSA   rSaveParams;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( hKeyRoot );
    SYM_ASSERT( lpszSubKey );
    SYM_ASSERT( lpszValue );

    rSaveParams.hkr          = hKeyRoot;
    rSaveParams.pszSubKey    = lpszSubKey;
    rSaveParams.pszValueName = lpszValue;

    ::SendMessage( m_hWnd, TB_SAVERESTOREA, TRUE, reinterpret_cast<LPARAM>( &rSaveParams ) );
} // CNVToolBarCtrl::SaveState()

void CNVToolBarCtrl::SaveState( HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue )
{
    auto    TBSAVEPARAMSW   rSaveParams;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( hKeyRoot );
    SYM_ASSERT( lpszSubKey );
    SYM_ASSERT( lpszValue );

    rSaveParams.hkr          = hKeyRoot;
    rSaveParams.pszSubKey    = lpszSubKey;
    rSaveParams.pszValueName = lpszValue;

    ::SendMessage( m_hWnd, TB_SAVERESTOREW, TRUE, reinterpret_cast<LPARAM>( &rSaveParams ) );
} // CNVToolBarCtrl::SaveState()


//*************************************************************************
// CNVToolBarCtrl::RestoreState()
//
// public
// void CNVToolBarCtrl::RestoreState(
//      HKEY    hKeyRoot,               [in] registry key
//      LPCSTR  lpszSubKey,             [in] registry subkey name
//      LPCSTR  lpszValue )             [in] registry value name
//
// This method restores the state of the toolbar previously saved by
// a call to SaveState().
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

void CNVToolBarCtrl::RestoreState( HKEY hKeyRoot, LPCSTR lpszSubKey, LPCSTR lpszValue )
{
    auto    TBSAVEPARAMSA   rSaveParams;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( hKeyRoot );
    SYM_ASSERT( lpszSubKey );
    SYM_ASSERT( lpszValue );

    rSaveParams.hkr          = hKeyRoot;
    rSaveParams.pszSubKey    = lpszSubKey;
    rSaveParams.pszValueName = lpszValue;

    ::SendMessage( m_hWnd, TB_SAVERESTOREA, FALSE, reinterpret_cast<LPARAM>( &rSaveParams ) );
} // CNVToolBarCtrl::RestoreState()

void CNVToolBarCtrl::RestoreState( HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue )
{
    auto    TBSAVEPARAMSW   rSaveParams;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( hKeyRoot );
    SYM_ASSERT( lpszSubKey );
    SYM_ASSERT( lpszValue );

    rSaveParams.hkr          = hKeyRoot;
    rSaveParams.pszSubKey    = lpszSubKey;
    rSaveParams.pszValueName = lpszValue;

    ::SendMessage( m_hWnd, TB_SAVERESTOREW, FALSE, reinterpret_cast<LPARAM>( &rSaveParams ) );
} // CNVToolBarCtrl::RestoreState()


//*************************************************************************
// CNVToolBarCtrl::DestroyWindow()
//
// public virtual
// BOOL CNVToolBarCtrl::DestroyWindow( )
//
// This method destroys the associated toolbar window.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 03/25/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVToolBarCtrl::DestroyWindow()
{
    auto    BOOL    bSuccess;

    bSuccess = FALSE;

    // Bail out early if no associated window.

    if ( NULL == m_hWnd )
    {
        goto BailOut;
    }

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    if ( ::DestroyWindow( m_hWnd ) )
    {
        m_hWnd   = NULL;
        bSuccess = TRUE;
    }

BailOut:
    return ( bSuccess );
} // CNVToolBarCtrl::DestroyWindow()


