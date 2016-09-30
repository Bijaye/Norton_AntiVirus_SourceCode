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
#include "PausableKRProgressCtrl.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CScanProgressWnd : 
    public CScanEventsSource,
    public CSymDHTMLWindow< CScanProgressWnd, CBaseClientWnd >
{
public:

    typedef CSymDHTMLWindow< CScanProgressWnd, CBaseClientWnd > CBaseDHTMLWnd;

    CScanProgressWnd( CScanEventsSink* pSink );
    void OverrideProgressTimeOut(UINT nTimeOut);

    DECLARE_WND_CLASS( _T("SymScanProgressWnd") )

    BEGIN_MSG_MAP(CScanProgressWnd)
        MESSAGE_HANDLER( WM_TIMER, OnTimer )
        MESSAGE_HANDLER( WM_DESTROY, OnDestory )
        CHAIN_MSG_MAP( CBaseClientWnd )
        CHAIN_MSG_MAP( CBaseDHTMLWnd )
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE(CScanProgressWnd)
        SYM_DHTML_EVENT_ONCLICK( _T("PauseScan"), OnPauseBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("ResumeScan"), OnResumeBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("StopScan"), OnStopBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
    END_SYM_DHTML_EVENT_MAP_INLINE()

    void PauseKnightRiderBar();
    void ResumeKnightRiderBar();

protected:
    CPausableKRProgressCtrl m_FakeProgress;

    enum // Help ID
    {
        IDH = IDH_NAVW_REPAIR_WIZARD_DLG_HELP_BTN
    };

    bool m_bPaused;
    bool m_bNoSpyware;
    bool m_bAdditionalScan;

    CString m_strCurrentFolder;
    long m_nTotalScanned;
    long m_nThreatsDetected;
    long m_nOtherThreatsDetected;

    UINT m_nTimeOut;

    void StartProgressPolling();
    void EndProgressPolling();
    
    virtual LRESULT OnAltKey();
    virtual LRESULT OnGetHelpID();
    virtual LRESULT OnKillActive();
    virtual LRESULT OnSetActive();

    virtual void UpdateProgress();
    
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestory(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    HRESULT OnPauseBtnClick( IHTMLElement* pElement);
    HRESULT OnResumeBtnClick( IHTMLElement* pElement);
    HRESULT OnStopBtnClick( IHTMLElement* pElement);
    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement);

    // Cacheing the element interfaces for the items we update on timer
    CComPtr<IDispatch> m_spdispScanned;
    CComPtr<IDispatch> m_spdispThreats;
    CComPtr<IDispatch> m_spdispOtherThreats;
    CComPtr<IDispatch> m_spdispCurrentItem;
};




