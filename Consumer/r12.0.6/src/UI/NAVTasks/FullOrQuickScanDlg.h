//******************************************************************************
// Copyright © 2005 Symantec Corporation.
// ---------------------------------------------------------------------------
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// All rights reserved.
//
//******************************************************************************
#pragma once

#include "DHTMLWndCtrlview.h"
#include "..\NavTasksRes\ResResource.h"

#include "DHTMLwndCtrlDlg.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CFullOrQuickScanDlg : 
    public CDHTMLWndCtrlDlg< CFullOrQuickScanDlg >
{
public:

    enum{ IDD = IDD_FULLORQUICKSCAN };

    //
    // Order of the following enum MUST match the order in which
    // the radio buttons are defined on the dialog
    //
    enum{ USER_ACTION_FULL = 0, USER_ACTION_QUICK = 1 };

    CFullOrQuickScanDlg();

    BEGIN_MSG_MAP( CFullOrQuickScanDlg )
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        CHAIN_MSG_MAP( CDHTMLWndCtrlDlg< CFullOrQuickScanDlg > )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CFullOrQuickScanDlg )
        SYM_DHTML_EVENT_ONCLICK( _T("OK"), OnOKBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Cancel"), OnCancelBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Quick"), OnClickActionRadio )
        SYM_DHTML_EVENT_ONCLICK( _T("Full"), OnClickActionRadio )
    END_SYM_DHTML_EVENT_MAP_INLINE()

    long GetUserAction(){ return m_lUserAction; };

protected:

    virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, 
                                UINT /*nCtlID*/ = (UINT)-1);
    virtual BOOL OnInitDialog();

    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
    virtual void OnF1Help();

    HRESULT OnOKBtnClick( IHTMLElement* pElement);
    HRESULT OnCancelBtnClick( IHTMLElement* pElement);
    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement);
    HRESULT OnClickActionRadio( IHTMLElement* pElement );

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                    BOOL& /*bHandled*/);

    long m_lUserAction;

    bool m_bNoSpyware;
};
