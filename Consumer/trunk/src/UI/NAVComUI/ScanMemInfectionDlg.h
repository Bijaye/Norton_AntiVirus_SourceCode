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
#include "ScanEvents.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"
#include "InputDetection.h"
#include "DHTMLwndCtrlDlg.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CScanMemInfectionDlg : 
    public CDHTMLWndCtrlDlg< CScanMemInfectionDlg >,
    public CInputDetectionSink,
    public CScanEventsSource
{
public:

    enum{ IDD = IDD_SCAN_MEMINFECTION };

    CScanMemInfectionDlg( CScanEventsSink* pEvents, BOOL bEnabledTimout = TRUE );

    BEGIN_MSG_MAP( CScanMemInfectionDlg )
        MESSAGE_HANDLER( WM_CLOSE, OnClose )
        MESSAGE_HANDLER( WM_TIMER, OnTimer )
        MESSAGE_HANDLER( WM_DESTROY, OnDestory )
        CHAIN_MSG_MAP( CDHTMLWndCtrlDlg< CScanMemInfectionDlg > )
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CScanMemInfectionDlg )
        SYM_DHTML_EVENT_ONCLICK( _T("Yes"), OnYesBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("No"), OnNoBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

protected:

    enum // Help ID
    {
        IDH = NAV_WARNING_MEMORY_INFECTION
    };

    int m_nTimeout;
    BOOL m_bEnableTimeout;
    bool m_bInputDetected;

    StahlSoft::CSmartPtr<CInputDetection> m_spInputDetector;

    virtual BOOL OnInitDialog();
    virtual void OnF1Help();

    HRESULT OnYesBtnClick( IHTMLElement* pElement);
    HRESULT OnNoBtnClick( IHTMLElement* pElement);
    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement);

    //
    // CInputDetectionSink
    //
    virtual void OnInputDeteted();
    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
    
    LRESULT OnDestory(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnImputTimeout(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    // Helper
    void HideCountDownText(bool bHide);
};



