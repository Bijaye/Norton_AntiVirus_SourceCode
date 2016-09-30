//******************************************************************************
// Copyright © 2004 Symantec Corporation.
// ---------------------------------------------------------------------------
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// All rights reserved.
//
// Author:  Chirstopher Brown 11/08/2004
//******************************************************************************
#pragma once

#include "DHTMLWndCtrlview.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"

#include "ScanEvents.h"
#include "DHTMLwndCtrlDlg.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CScanConditionalActionDlg : 
    public CDHTMLWndCtrlDlg< CScanConditionalActionDlg >,
    public CScanEventsSource
{
public:

    enum{ IDD = IDD_SCAN_CONDITIONAL };

    CScanConditionalActionDlg( CScanEventsSink* pSink, const long lThreatsRemaining, const UINT nResDescription, const UINT nHelpId = NULL );

    BEGIN_MSG_MAP( CScanConditionalActionDlg )
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        CHAIN_MSG_MAP( CDHTMLWndCtrlDlg< CScanConditionalActionDlg > )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CScanConditionalActionDlg )
        SYM_DHTML_EVENT_ONCLICK( _T("OK"), OnOKBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

protected:

//    enum // Help ID
//    {
//        IDH = IDH_NAVW_REPAIR_WIZARD_DLG_HELP_BTN
//    };

    long m_lThreatsRemaining;
    UINT m_nResIDDescription;
    UINT m_nHelpId;

    virtual BOOL OnInitDialog();

    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
    virtual void OnF1Help();

    HRESULT OnOKBtnClick( IHTMLElement* pElement);
    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement); 

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};



