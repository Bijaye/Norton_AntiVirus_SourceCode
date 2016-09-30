//******************************************************************************
// Copyright © 2004 Symantec Corporation.
//------------------------------------------------------------------------------
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// All rights reserved.
//
// Author:  Chirstopher Brown 11/08/2004
//******************************************************************************
#pragma once
#include "DHTMLWndCtrlView.h"
#include "BaseClientWnd.h"
#include "ScanEvents.h"
#include "NAVComUI.h"

const DWORD UM_PROGRESS_UPDATE = ::RegisterWindowMessage(_T("{43D02672-6784-2222-B5F4-1558DFD82D31}") );
const DWORD END_PROGRESS_VALUE = 101;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CScanRepairProgressWnd : 
    public CSymDHTMLWindow< CScanRepairProgressWnd, CBaseClientWnd >,
    public CScanEventsSource
{
public:

    typedef CSymDHTMLWindow< CScanRepairProgressWnd, CBaseClientWnd > CBaseDHTMLWnd;

    CScanRepairProgressWnd( CScanEventsSink* pSink );

    DECLARE_WND_CLASS( "SymScanRepairProgressWnd" )

    BEGIN_MSG_MAP(CScanRepairProgressWnd)
        MESSAGE_HANDLER( UM_PROGRESS_UPDATE, OnUpdateProgress )
        CHAIN_MSG_MAP( CBaseClientWnd )
        CHAIN_MSG_MAP( CBaseDHTMLWnd )
    END_MSG_MAP()


    BEGIN_SYM_DHTML_EVENT_MAP_INLINE(CScanRepairProgressWnd)
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Abort"), OnAbortClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

    void OnUpdateRepairProgress(unsigned long nItemsRemaining, bool& bAbort);

protected:

    bool m_bAbort;

    enum // Help ID
    {
        IDH = IDH_NAVW_REPAIR_WIZARD_DLG_HELP_BTN
    };

    _bstr_t m_bstrOuterProgressHTML;

    int m_nProgress;
    DWORD m_nTotalItems;
    DWORD m_nItemsRemaining;

    void StartProgressTimer();
    void EndProgressTimer();

    // CBaseClientWnd
	virtual BOOL OnInitDialog();
    virtual LRESULT OnAltKey();
    virtual LRESULT OnGetHelpID();
    virtual LRESULT OnKillActive();
    virtual LRESULT OnSetActive();

    virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, UINT /*nCtlID*/ = (UINT)-1);

    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement );
    HRESULT OnAbortClick( IHTMLElement* pElement );

    LRESULT OnUpdateProgress(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);   
};


