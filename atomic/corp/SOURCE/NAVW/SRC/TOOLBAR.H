// Copyright 1998 Symantec Corporation. 
//*************************************************************************
//
// $Header:   S:/NAVW/VCS/toolbar.h_v   1.0   01 Apr 1998 14:43:26   DALLEE  $
//
// Description:
//      Header for NAVW toolbar controls.
//
// Contains:
//      Declaration of CNVReBarCtrl   
//                     CNVToolBarCtrl 
//*************************************************************************
// $Log:   S:/NAVW/VCS/toolbar.h_v  $
// 
//    Rev 1.0   01 Apr 1998 14:43:26   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __TOOLBAR_H
#define __TOOLBAR_H

#ifndef _INC_COMMCTRL
#include <commctrl.h>
#endif


//
// CNVReBarCtrl - Non-MFC Rebar (Internet Explorer "Coolbar") control
//

class CNVReBarCtrl
{
// Construction
public:
    CNVReBarCtrl();
    BOOL Create( HINSTANCE hInst, DWORD dwStyle, LPCRECT pRect, HWND hParent, UINT uID );

// Attributes
public:
    HWND    m_hWnd;
    operator HWND() const;

	BOOL GetBarInfo(LPREBARINFO lp) const
        { SYM_ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage( m_hWnd, RB_GETBARINFO, 0, (LPARAM)lp); }
	BOOL SetBarInfo(LPREBARINFO lp)
        { SYM_ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage( m_hWnd, RB_SETBARINFO, 0, (LPARAM)lp); }

	BOOL GetBandInfo(int iBand, LPREBARBANDINFO lp) const
        { SYM_ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage( m_hWnd, RB_GETBANDINFO, iBand, (LPARAM)lp); }
	BOOL SetBandInfo(int iBand, LPREBARBANDINFO lp)
        { SYM_ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage( m_hWnd, RB_SETBANDINFO, iBand, (LPARAM)lp); }

	int GetBandCount() const
        { SYM_ASSERT(::IsWindow(m_hWnd));
		  return (int)SendMessage( m_hWnd, RB_GETBANDCOUNT, 0, 0L); }
	int GetRowCount() const
        { SYM_ASSERT(::IsWindow(m_hWnd));
        return (int)SendMessage( m_hWnd, RB_GETROWCOUNT, 0, 0L); }
	int GetRowHeight(int nWhich) const
        { SYM_ASSERT(::IsWindow(m_hWnd));
	     return (int)SendMessage( m_hWnd, RB_GETROWHEIGHT, (WPARAM)nWhich, 0L); }

// Operations
public:
	BOOL DeleteBand(int nWhich)
        { SYM_ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage( m_hWnd, RB_INSERTBAND, (WPARAM)nWhich, 0L ); }
	BOOL InsertBand(int iWhere, LPREBARBANDINFO lp)
        { SYM_ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage( m_hWnd, RB_INSERTBAND, (WPARAM)iWhere, (LPARAM)lp); }

    virtual BOOL DestroyWindow();

// Implementation
public:
    virtual ~CNVReBarCtrl();
};


//
// CNVToolBarCtrl - Generic Non-MFC toolbar control
//

class CNVToolBarCtrl
{
// Construction
public:
    CNVToolBarCtrl();
    BOOL Create( HINSTANCE hInst, DWORD dwStyle, LPCRECT pRect, HWND hParent, UINT uID );

// Attributes
public:
    HWND    m_hWnd;
    operator HWND() const;

    int GetButtonCount() const;
    void SetButtonStructSize( int nSize = sizeof( TBBUTTON ) );
    int GetBitmap( int nID ) const;
    BOOL SetBitmap( int nButtonID, int nBitmapIndex );
    UINT GetBitmapFlags() const;
    BOOL GetButton( int nIndex, LPTBBUTTON lpButton ) const;
    int GetButtonText( int nID, LPSTR lpszText ) const;
    int GetButtonText( int nID, LPWSTR lpszText ) const;
    BOOL GetItemRect( int nIndex, LPRECT lpRect ) const;
    int GetRows() const;
    void SetRows( int nRows, BOOL bLarger, LPRECT lpRect );
    int GetState( int nID ) const;
    BOOL SetState( int nID, int nState );
    HWND GetToolTips() const;
    void SetToolTips( HWND hToolTips );
    BOOL IsButtonChecked( int nID ) const;
    BOOL IsButtonEnabled( int nID ) const;
    BOOL IsButtonHidden( int nID ) const;
    BOOL IsButtonIndeterminate( int nID ) const;
    BOOL IsButtonPressed( int nID ) const;
    BOOL SetBitmapSize( int nWidth, int nHeight );
    BOOL SetButtonSize( int nWidth, int nHeight );
    BOOL SetButtonWidth( int nWidth );
    BOOL SetCmdID( int nIndex, int nID );
    HWND SetParent( HWND hParent );
    HIMAGELIST GetDisabledImageList();
    HIMAGELIST SetDisabledImageList( HIMAGELIST hImageList );
    HIMAGELIST GetHotImageList();
    HIMAGELIST SetHotImageList( HIMAGELIST hImageList );
    HIMAGELIST GetImageList();
    HIMAGELIST SetImageList( HIMAGELIST hImageList );
    int GetTextRows() const;
    void SetMaxTextRows( int nRows );

// Operations
public:
    int AddBitmap( int nNumBitmaps, HINSTANCE hResInst, UINT uID );
    int AddBitmap( int nNumBitmaps, HBITMAP hBitmap );
    BOOL AddButtons( int nNumButtons, LPCTBBUTTON lpButtons );
    int AddString( HINSTANCE hResInst, UINT uID );
    int AddStrings( LPCSTR lpszStrings );
    int AddStrings( LPCWSTR lpszStrings );
    void AutoSize();
    int CommandToIndex( int nID ) const;
    BOOL DeleteButton( int nIndex );
    BOOL EnableButton( int nID, BOOL bEnable = TRUE );
    BOOL HideButton( int nID, BOOL bHide = TRUE );
    BOOL Indeterminate( int nID, BOOL bIndeterminate = TRUE );
    BOOL InsertButton( int nIndex, LPTBBUTTON lpButton );
    BOOL PressButton( int nID, BOOL bPress = TRUE );
	void SaveState( HKEY hKeyRoot, LPCSTR lpszSubKey, LPCSTR lpszValue );
	void SaveState( HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue );
	void RestoreState( HKEY hKeyRoot, LPCSTR lpszSubKey, LPCSTR lpszValue );
	void RestoreState( HKEY hKeyRoot, LPCWSTR lpszSubKey, LPCWSTR lpszValue );

    virtual BOOL DestroyWindow();

// Implementation
public:
    virtual ~CNVToolBarCtrl();
};


#endif // !__TOOLBAR_H
