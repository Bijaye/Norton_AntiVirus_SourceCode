////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Author:  Chirstopher Brown 11/08/2004

#pragma once

#include "DHTMLWndCtrlview.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"

#include "ScanEvents.h"
#include "DHTMLwndCtrlDlg.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CConfirmDlg : 
    public CDHTMLWndCtrlDlg< CConfirmDlg >
{
public:

    enum{ IDD = IDD_CONFIRM };

    CConfirmDlg( UINT nResID );

    BEGIN_MSG_MAP( CConfirmDlg )
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
        CHAIN_MSG_MAP( CDHTMLWndCtrlDlg< CConfirmDlg > )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CConfirmDlg )
        SYM_DHTML_EVENT_ONCLICK( _T("Ok"), OnOkBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Cancel"), OnCancelBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Yes"), OnYesBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("No"), OnNoBtnClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

protected:

    UINT m_nResIDMessage;

    virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, UINT /*nCtlID*/ = (UINT)-1);
    virtual BOOL OnInitDialog();

    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);

    HRESULT OnOkBtnClick( IHTMLElement* pElement);
    HRESULT OnCancelBtnClick( IHTMLElement* pElement);
    HRESULT OnYesBtnClick( IHTMLElement* pElement);
    HRESULT OnNoBtnClick( IHTMLElement* pElement);
    
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};



