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
#include "DHTMLWndCtrlView.h"
#include "BaseClientWnd.h"
#include "ScanEvents.h"
#include "NAVComUI.h"
#include "EZEraserObjectsW.h"

static const TCHAR REPAIR_SELECTION[] = _T("REPAIR");
static const TCHAR DELETE_SELECTION[] = _T("DELETE");
static const TCHAR REMOVE_SELECTION[] = _T("REMOVE");
static const TCHAR REVIEW_SELECTION[] = _T("REVIEW");
static const TCHAR IGNORE_SELECTION[] = _T("IGNORE");
static const TCHAR EXCLUDE_SELECTION[] = _T("EXCLUDE");
static const TCHAR QUARANTINE_SELECTION[] = _T("QUARANTINE");

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CScanRepairRemoveWnd : 
    public CSymDHTMLWindow< CScanRepairRemoveWnd, CBaseClientWnd >,
    public CScanEventsSource
{
public:

    typedef CSymDHTMLWindow< CScanRepairRemoveWnd, CBaseClientWnd > CBaseDHTMLWnd;

    CScanRepairRemoveWnd( CScanEventsSink* pSink );

    DECLARE_WND_CLASS( _T("SymScanRepairRemoveWnd" ))

    BEGIN_MSG_MAP(CScanRepairRemoveWnd)
        MESSAGE_HANDLER( WM_PAINT, OnPaint )
        COMMAND_ID_HANDLER(ID_CTRLTAB, OnCtrlTab)
        CHAIN_MSG_MAP( CBaseClientWnd )
        CHAIN_MSG_MAP( CBaseDHTMLWnd )
    END_MSG_MAP()


    BEGIN_SYM_DHTML_EVENT_MAP_INLINE(CScanRepairRemoveWnd)
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Apply"), OnApplyBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Back"), OnCancelBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Cancel"), OnCancelBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("tab1"), OnTab1BtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("tab2"), OnTab2BtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("tab1HC"), OnTab1BtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("tab2HC"), OnTab2BtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("QuarantineNameText"), OnLaunchQuarantineClick)
        SYM_DHTML_EVENT_ONCLICK( _T("email_info"), OnEmailInfoClick )
        SYM_DHTML_EVENT_AXCONTROL( 1, _T("AxListCtrl"), OnItemChanged )
        SYM_DHTML_EVENT_AXCONTROL( 2, _T("AxListCtrl"), OnThreatTypeClick )
        SYM_DHTML_EVENT_AXCONTROL( 3, _T("AxListCtrl"), OnThreatRiskClick )
        SYM_DHTML_EVENT_AXCONTROL( 4, _T("AxListCtrl"), OnThreatNameClick )
        SYM_DHTML_EVENT_AXCONTROL( 5, _T("AxListCtrl"), 
                                   OnDisplayQuarantineNote )
        SYM_DHTML_EVENT_ONMOUSEOVER( _T("email_info"), OnMouseOverEmailInfo )
        SYM_DHTML_EVENT_ONMOUSEOUT( _T("email_info"), OnMouseOutEmailInfo )
        SYM_DHTML_EVENT_ONBLUR( _T("email_info"), OnMouseOutEmailInfo )
        SYM_DHTML_EVENT_ONKEYUP( _T("email_info"), OnMouseOverEmailInfo )
    END_SYM_DHTML_EVENT_MAP_INLINE()

    void Initialize(BOOL bEmailScan);

protected:

CScanEventsSink * m_pSink;

    BOOL m_bTab1Visible;
    BOOL m_bTab2Visible;

    CString m_strTab1Text;
    CString m_strTab2Text;

    ccEraser::IAnomalyListPtr m_spAnomalyList;
    CComPtr< IAxSysListView32 > m_spAxSysListView;
    CEZAnomaly m_EZAnomaly;

    enum // Help ID
    {
        IDH = NAV_SCAN_DETAILS
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

    void __stdcall OnItemChanged();
    void __stdcall OnThreatTypeClick();
    void __stdcall OnThreatNameClick();
    void __stdcall OnThreatRiskClick();
    void __stdcall OnDisplayQuarantineNote();

    virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, 
                                UINT /*nCtlID*/ = (UINT)-1);
    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    HRESULT ChangeTab( TABS dwTab );

    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement );
    HRESULT OnApplyBtnClick( IHTMLElement* pElement );
    HRESULT OnCancelBtnClick( IHTMLElement* pElement );
    HRESULT OnTab1BtnClick( IHTMLElement* pElement );
    HRESULT OnTab2BtnClick( IHTMLElement* pElement );
    HRESULT OnEmailInfoClick( IHTMLElement* pElement );
    HRESULT OnLaunchQuarantineClick( IHTMLElement* pElement );
    HRESULT OnMouseOverEmailInfo( IHTMLElement* pElement );
    HRESULT OnMouseOutEmailInfo( IHTMLElement* pElement );
    LRESULT OnCtrlTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    void SetCurrentAnomalySelection( CEZAnomaly* pEZAnomaly, LPCTSTR pcszAction);

    BOOL m_bHighContrastMode;
    bool m_bEmailScan;
};


