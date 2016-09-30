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
#include "BaseClientWnd.h"
#include "ScanEvents.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CScanResultsWnd :  
    public CScanEventsSource,
    public CSymDHTMLWindow< CScanResultsWnd, CBaseClientWnd >
{
public:
 
    typedef CSymDHTMLWindow< CScanResultsWnd, CBaseClientWnd > CBaseDHTMLWnd;

    CScanResultsWnd( CScanEventsSink* pSink );

    DECLARE_WND_CLASS( _T("SymScanResultsWnd") )

    BEGIN_MSG_MAP(CScanResultsWnd)
        CHAIN_MSG_MAP( CBaseClientWnd )
        CHAIN_MSG_MAP( CBaseDHTMLWnd )
    END_MSG_MAP()

    BEGIN_SYM_DHTML_EVENT_MAP_INLINE( CScanResultsWnd )
        //
        // Changing "View Details" from a button to a text link.
        // 
        //SYM_DHTML_EVENT_ONCLICK( _T("Details"), OnDetailsBtnClick )
        
        SYM_DHTML_EVENT_ONCLICK( _T("ViewDetailsText"), OnDetailsBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("ManualRemovalText"), 
                                 OnManualRemovalBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("ManualRemoval"), OnManualRemovalBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("QuarantineNameText"), 
                                 OnQuarantineBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Finished"), OnFinishedBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("Quarantine"), OnQuarantineBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("MoreInfo"), OnMoreInfoBtnClick )
        SYM_DHTML_EVENT_ONCLICK( _T("RenewSubscription"), 
                                 OnRenewSubscriptionBtnClick )
        
    END_SYM_DHTML_EVENT_MAP_INLINE()

protected:

    enum // Help ID
    {
        IDH = NAV_SCAN_RESULTS
    };

    ccEraser::IAnomalyListPtr m_spAnomalyList;

    long m_nTotalFilesScanned;
    long m_nThreatsDetected;
    long m_nThreatsDeletedRemoved;
    long m_nThreatsRemaining;
    long m_nThreatsQuarantined;

    bool m_bLaunchLiveUpdate;
    bool m_bDefsOldOrExpired;
    bool m_bNoSpyware;
    bool m_bQuarantineNoteApplies;

    CScanEventsSink *m_pSink;

    virtual LRESULT OnAltKey();
    virtual LRESULT OnGetHelpID();
    virtual LRESULT OnKillActive();
    virtual LRESULT OnSetActive();

    virtual BOOL DoDataExchange(BOOL /*bSaveAndValidate*/ = FALSE, 
                                UINT /*nCtlID*/ = (UINT)-1);
    virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);

    HRESULT OnDetailsBtnClick( IHTMLElement* pElement );
    HRESULT OnManualRemovalBtnClick( IHTMLElement* pElement );
    HRESULT OnQuarantineBtnClick( IHTMLElement* pElement );
    HRESULT OnFinishedBtnClick( IHTMLElement* pElement );
    HRESULT OnMoreInfoBtnClick( IHTMLElement* pElement);
    HRESULT OnRenewSubscriptionBtnClick( IHTMLElement* pElement);

};



