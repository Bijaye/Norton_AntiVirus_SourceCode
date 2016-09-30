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
#include "EZEraserObjects.h"


/*
static const TCHAR REPAIR_SELECTION[] = _T("REPAIR");
static const TCHAR QUARANTINE_SELECTION[] = _T("QUARANTINE");
static const TCHAR DELETE_SELECTION[] = _T("DELETE");
static const TCHAR REMOVE_SELECTION[] = _T("REMOVE");
static const TCHAR EXCLUDE_SELECTION[] = _T("EXCLUDE");
static const TCHAR IGNORE_SELECTION[] = _T("IGNORE");
static const TCHAR REVIEW_SELECTION[] = _T("REVIEW");
*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CManualRemovalWnd : 
    public CSymDHTMLWindow< CManualRemovalWnd, CBaseClientWnd >,
    public CScanEventsSource
{
public:

    typedef CSymDHTMLWindow< CManualRemovalWnd, CBaseClientWnd > CBaseDHTMLWnd;

    CManualRemovalWnd( CScanEventsSink* pSink );

    DECLARE_WND_CLASS( "SymManualRemovalWnd" )

    BEGIN_MSG_MAP(CManualRemovalWnd)
//        COMMAND_ID_HANDLER(ID_CTRLTAB, OnCtrlTab)
        CHAIN_MSG_MAP( CBaseClientWnd )
        CHAIN_MSG_MAP( CBaseDHTMLWnd )
    END_MSG_MAP()


    BEGIN_SYM_DHTML_EVENT_MAP_INLINE(CManualRemovalWnd)
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Back"), OnCancelBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Cancel"), OnCancelBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("ThreatName"), OnThreatNameClick)
        SYM_DHTML_EVENT_ONCLICK( _T("ThreatLinkNote"), OnThreatNameClick)
       SYM_DHTML_EVENT_AXCONTROL( 4, _T("AxListCtrl"), OnThreatNameClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

    void Initialize(BOOL bEmailScan);

protected:

    ccEraser::IAnomalyListPtr m_spAnomalyList;
    CComPtr< IAxSysListView32 > m_spAxSysListView;
    CEZAnomaly m_EZAnomaly;

    enum // Help ID
    {         
        IDH = NAVW_RISK_MANUAL_REMOVAL
    };

    enum TABS
    {
        TAB1 = 1,
        TAB2,
        TAB3
    };

    virtual LRESULT OnAltKey();
    virtual LRESULT OnGetHelpID();
    virtual LRESULT OnKillActive();
    virtual LRESULT OnSetActive();

    void __stdcall OnThreatNameClick();

    virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, 
                                UINT /*nCtlID*/ = (UINT)-1);
//    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);


    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement );
    HRESULT OnCancelBtnClick( IHTMLElement* pElement );
    HRESULT OnThreatNameClick( IHTMLElement* pElement );
//    LRESULT OnCtrlTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

//    void SetCurrentAnomalySelection( CEZAnomaly* pEZAnomaly, LPCTSTR pcszAction);

    bool m_bEmailScan;
};


