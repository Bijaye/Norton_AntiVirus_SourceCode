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
class CHandleLowThreatsDlg : 
    public CDHTMLWndCtrlDlg< CHandleLowThreatsDlg >,
    public CScanEventsSource
{
public:

    enum{ IDD = IDD_HANDLE_LOW_THREATS };

    CHandleLowThreatsDlg(bool &bAlertAgain);

    BEGIN_MSG_MAP( CHandleLowThreatsDlg )
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        CHAIN_MSG_MAP( CDHTMLWndCtrlDlg< CHandleLowThreatsDlg > )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CHandleLowThreatsDlg )
        SYM_DHTML_EVENT_ONCLICK( _T("btnContinue"), OnOkBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("btnAbort"), OnCancelBtnClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

protected:

    enum // Help ID
    {
        IDH = 0
    };

    // Disallowed
    CHandleLowThreatsDlg();

    virtual BOOL OnInitDialog();

    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
    virtual void OnF1Help();
    virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, UINT /*nCtlID*/ = (UINT)-1);

    HRESULT OnOkBtnClick( IHTMLElement* pElement);
    HRESULT OnCancelBtnClick( IHTMLElement* pElement);

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    int m_iNeverAlertChecked;
    bool* m_pbAlertAgain;
};



