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
class CKnownToHaveDependenciesWarningDlg : 
    public CDHTMLWndCtrlDlg< CKnownToHaveDependenciesWarningDlg >,
    public CScanEventsSource
{
public:

    enum{ IDD = IDD_KNOWN_DEPENDENCIES_WARNING };

    CKnownToHaveDependenciesWarningDlg();

    BEGIN_MSG_MAP( CKnownToHaveDependenciesWarningDlg )
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        CHAIN_MSG_MAP( CDHTMLWndCtrlDlg< CKnownToHaveDependenciesWarningDlg > )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CKnownToHaveDependenciesWarningDlg )
        SYM_DHTML_EVENT_ONCLICK( _T("btnContinue"), OnContinueBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("btnAbort"), OnAbortBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

protected:

    enum // Help ID
    {
        IDH = NAV_WARNING_REMOVE_RISK
    };

    virtual BOOL OnInitDialog();

    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
    virtual void OnF1Help();

    HRESULT OnContinueBtnClick( IHTMLElement* pElement);
    HRESULT OnAbortBtnClick( IHTMLElement* pElement);
    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement); 

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};



