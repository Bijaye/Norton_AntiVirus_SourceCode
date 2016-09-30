////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Author:  Chirstopher Brown 11/08/2004
//******************************************************************************
#pragma once

#include "DHTMLWndCtrlview.h"
#include "ThunkImpl.h"
#include "isVersion.h"

template <class T>
class SymDHTMLWindowGetMessageThunk: public _ThunkImpl<T> {};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template< class TBase, class TWinBase = CDialogImpl< TBase > >
class CDHTMLWndCtrlDlg: 
    public CSymDHTMLWindow< TBase, TWinBase >,
    public SymDHTMLWindowGetMessageThunk< CDHTMLWndCtrlDlg >
{
public:

    typedef CDHTMLWndCtrlDlg< TBase, TWinBase > CDHTMLWndCtrlDlgType;
    typedef CSymDHTMLWindow< TBase, TWinBase > CSymDHTMLWindowType;

    CDHTMLWndCtrlDlg( LPCTSTR szResName );

    BEGIN_MSG_MAP( CDHTMLWndCtrlDlgType )
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
        COMMAND_ID_HANDLER( ID_F1_HELP, OnF1Help )
        COMMAND_ID_HANDLER( ID_ALT_KEY, OnAltKey )
        CHAIN_MSG_MAP( CSymDHTMLWindowType )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE(CDHTMLWndCtrlDlg)
    END_SYM_DHTML_EVENT_MAP_INLINE()

	static CString staticProductName;

protected:

    virtual BOOL OnInitDialog();
    virtual void OnF1Help();
    virtual void OnAltKey();
    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);

    HACCEL m_hAccel;
	static CString m_strProductName;

    HHOOK m_hGetMsgHook;
    LRESULT GetMsgProc( int nCode, WPARAM wParam, LPARAM lParam );

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnF1Help(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnAltKey(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

template< class TBase, class TWinBase >
CString CDHTMLWndCtrlDlg<TBase, TWinBase>::m_strProductName;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
CDHTMLWndCtrlDlg< TBase, TWinBase >::CDHTMLWndCtrlDlg( LPCTSTR szResName ) :
    CSymDHTMLWindow< TBase, CDialogImpl< TBase> >( szResName ),
    m_hAccel(0)
{
    SymDHTMLWindowGetMessageThunk< CDHTMLWndCtrlDlg >::InitThunk( (TMFP)GetMsgProc, this );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
BOOL CDHTMLWndCtrlDlg< TBase, TWinBase >::OnInitDialog()
{
    __super::OnInitDialog();

    m_hAccel = LoadAccelerators( g_ResLoader.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME) );
    ATLASSERT( m_hAccel );

    const DWORD dwThreadId = ::GetCurrentThreadId();
    m_hGetMsgHook = ::SetWindowsHookEx( WH_GETMESSAGE, (HOOKPROC) SymDHTMLWindowGetMessageThunk< CDHTMLWndCtrlDlg >::GetThunk(), NULL, dwThreadId );
    ATLASSERT( m_hGetMsgHook );

    if( !m_hGetMsgHook )
    {
        CCTRACEE(_T("Unable to set get message windows hook"));
    }

	// Get product name from AVRES.DLL
	if(m_strProductName.IsEmpty())
	{
		m_strProductName = CISVersion::GetProductName();
	}
	
	// Set title window
	SetWindowText(m_strProductName);


    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
LRESULT CDHTMLWndCtrlDlg< TBase, TWinBase >::GetMsgProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    if( (nCode >= 0) && (PM_REMOVE == wParam) )
    {
        LPMSG lpMsg = reinterpret_cast<LPMSG>( lParam );

        if( m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, lpMsg) )
        {
            lpMsg->message = WM_NULL;
            lpMsg->lParam  = 0;
            lpMsg->wParam  = 0;
        }
        else 
        {
            BOOL bResult = FALSE;
            bResult = PreTranslateMessage(lpMsg);

            if( bResult || (!bResult && (lpMsg->message == WM_KEYDOWN && LOWORD(lpMsg->wParam) == VK_TAB)) )
            {
                lpMsg->message = WM_NULL;
                lpMsg->lParam  = 0;
                lpMsg->wParam  = 0;
            }
        }

    }

    return ::CallNextHookEx( m_hGetMsgHook, nCode, wParam, lParam );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
LRESULT CDHTMLWndCtrlDlg< TBase, TWinBase >::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    EndDialog(IDOK);
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
LRESULT CDHTMLWndCtrlDlg< TBase, TWinBase >::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if( m_hGetMsgHook )
        UnhookWindowsHookEx( m_hGetMsgHook );

    if( m_hAccel )
        DestroyAcceleratorTable( m_hAccel );

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
LRESULT CDHTMLWndCtrlDlg< TBase, TWinBase >::OnF1Help(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    OnF1Help();
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
LRESULT CDHTMLWndCtrlDlg< TBase, TWinBase >::OnAltKey(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    OnAltKey();
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
void CDHTMLWndCtrlDlg< TBase, TWinBase >::OnF1Help()
{
    ATLASSERT( /*Override this function to get F1 support...*/ FALSE ); 
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
void CDHTMLWndCtrlDlg< TBase, TWinBase >::OnAltKey()
{
    CComPtr< IHTMLRuleStyle > spStyle;

    HRESULT hr = S_OK;
    hr = GetStyleSheetRule( _T(".hotkey"), &spStyle );

    if( SUCCEEDED(hr) && spStyle )
    {
        _bstr_t bstrDecoration = _T("underline");
        hr = spStyle->put_textDecoration( bstrDecoration );
    } 
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template< class TBase, class TWinBase >
void CDHTMLWndCtrlDlg< TBase, TWinBase >::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
    __super::OnDocumentComplete(pDisp, szUrl);

    //
    // Check if menu mnemonic's should be displayed...
    //
    BOOL bResult = FALSE;
    BOOL bShowMenuUnderline = FALSE;
    bResult = SystemParametersInfo( SPI_GETMENUUNDERLINES, 0, (PVOID)&bShowMenuUnderline, 0 );

    if( bResult && bShowMenuUnderline )
        PostMessage( WM_COMMAND, ID_ALT_KEY );

}

