////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Author:  Michael M. Welch 04/29/2005

#pragma once
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"
#include "DHTMLWndCtrlview.h"
#include "DHTMLwndCtrlDlg.h"
#include "ScanEvents.h"
#include "EZEraserObjectsW.h"
#include "NAVComUI.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CRiskDetailsDlg: 
    public CDHTMLWndCtrlDlg< CRiskDetailsDlg >,
    public CScanEventsSource
{
public:

    enum{ IDD = IDD_RISK_DETAILS };

    CRiskDetailsDlg( CScanEventsSink* pSink, ccEraser::IAnomaly* pAnomaly,
                     bool bRiskAssessment, bool bViral );

    BEGIN_MSG_MAP(CRiskDetailsDlg)
        COMMAND_ID_HANDLER(ID_CTRLTAB, OnCtrlTab)
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        MESSAGE_HANDLER( WM_DESTROY, OnDestroy )
        CHAIN_MSG_MAP( CDHTMLWndCtrlDlg< CRiskDetailsDlg > )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    //BEGIN_SYM_DHTML_EVENT_MAP_INLINE( COtherAffectedAreasDlg )
    //    SYM_DHTML_EVENT_AXCONTROL( 1, _T("AxListCtrlTop"), OnItemChanged )
    //END_SYM_DHTML_EVENT_MAP_INLINE(

    BEGIN_SYM_DHTML_EVENT_MAP(CRiskDetailsDlg)
        SYM_DHTML_EVENT_ONCLICK( _T("tab1"), OnTab1BtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("tab2"), OnTab2BtnClick )

        SYM_DHTML_EVENT_AXCONTROL( 1, _T("AxListCtrlTop"), OnItemChanged )

        SYM_DHTML_EVENT_ONMOUSEOVER(_T("overallTitle"), OnOverallBtnClick )
        SYM_DHTML_EVENT_ONMOUSEOVER( _T("performaceTitle"), OnPerformaceBtnClick )
        SYM_DHTML_EVENT_ONMOUSEOVER( _T("privacyTitle"), OnPrivacyBtnClick )
        SYM_DHTML_EVENT_ONMOUSEOVER( _T("removalTitle"), OnRemovalBtnClick )
        SYM_DHTML_EVENT_ONMOUSEOVER( _T("stealthTitle"), OnStealthBtnClick )

        SYM_DHTML_EVENT_ONFOCUS(_T("overallTitle"), OnOverallBtnClick )
        SYM_DHTML_EVENT_ONFOCUS( _T("performaceTitle"), OnPerformaceBtnClick )
        SYM_DHTML_EVENT_ONFOCUS( _T("privacyTitle"), OnPrivacyBtnClick )
        SYM_DHTML_EVENT_ONFOCUS( _T("removalTitle"), OnRemovalBtnClick )
        SYM_DHTML_EVENT_ONFOCUS( _T("stealthTitle"), OnStealthBtnClick )

        SYM_DHTML_EVENT_ONCLICK(_T("overallTitle"), OnOverallBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("performaceTitle"), OnPerformaceBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("privacyTitle"), OnPrivacyBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("removalTitle"), OnRemovalBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("stealthTitle"), OnStealthBtnClick )

        //SYM_DHTML_EVENT_ONCLICK( _T("risk_name"), OnRiskNameClick )

		SYM_DHTML_EVENT_ONCLICK( _T("matrix_risk_name"), OnRiskNameClick )
        SYM_DHTML_EVENT_ONCLICK( _T("details_risk_name"), OnRiskNameClick )

        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("OK"), OnOkBtnClick )
    END_SYM_DHTML_EVENT_MAP()

protected:
    bool m_bRiskAssessment;

    BOOL m_bTab1Visible;

    CString m_strTab1Text;
    CString m_strTab2Text;

    enum // Help ID
    {
        IDH1 = NAV_THREAT_MATRIX,
		IDH2 = NAV_AFFECTED_AREAS
    };

    enum TABS
    {
        TAB1 = 1,
        TAB2,
        TAB3
    };

    CEZAnomaly m_EZAnomaly;

    CComPtr< IAxSysListView32OAA > m_spAxSysListViewTop;
    CComPtr< IAxSysListView32OAA > m_spAxSysListViewBottom;

    void __stdcall OnItemChanged();
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnCtrlTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    
	virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, UINT /*nCtlID*/ = (UINT)-1);
    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
    virtual BOOL OnInitDialog();
    virtual void OnF1Help();

    HRESULT ChangeTab( TABS dwTab );

    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement);
    HRESULT OnOkBtnClick( IHTMLElement* pElement);

    HRESULT OnOverallBtnClick( IHTMLElement* pElement);
    HRESULT OnPerformaceBtnClick( IHTMLElement* pElement);
    HRESULT OnPrivacyBtnClick( IHTMLElement* pElement);
    HRESULT OnRemovalBtnClick( IHTMLElement* pElement);
    HRESULT OnStealthBtnClick( IHTMLElement* pElement);
    HRESULT OnRiskNameClick( IHTMLElement* pElement);

    HRESULT OnTab1BtnClick( IHTMLElement* pElement );
    HRESULT OnTab2BtnClick( IHTMLElement* pElement );
};



