// CSymCorpUIScansView

#pragma once
#include "stdafx.h"
#include "ViewEx.h"
#include "StaticEx.h"
#include "ButtonEx.h"
#define PROTECTIONPROVIDER_HELPERTYPES_WITHNAMESPACE
#import "ProtectionProvider.tlb" raw_interfaces_only exclude("wireHWND", "_RemotableHandle", "__MIDL_IWinTypes_0009")
#include "ProtectionProvider.h"

class CSymCorpUIScansView : public CViewEx
{
// Message map functions
protected:

// MFC
public:
    enum { IDD = IDD_VIEW_SCANS };
    virtual void OnDraw( CDC* pDC );
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual void OnInitialUpdate();
    virtual void OnUpdate( CView* sender, LPARAM hintParam, CObject* hintObject );
    void OnRClickScan( NMHDR* notifyInfo, LRESULT* result );
	void OnDoubleClickScan( NMHDR* notifyInfo, LRESULT* result );
    void OnQuickScanBtn();
    void OnFullScanBtn();
    void OnPolicyScanBtn();
    void OnCreateScanBtn();
    DECLARE_MESSAGE_MAP()

// Constructor-destructor
public:    
	CSymCorpUIScansView();
	virtual ~CSymCorpUIScansView();
    DECLARE_DYNCREATE(CSymCorpUIScansView)

private:
    // ** FUNCTION MEMBERS **
    // Adds scanToAdd to the scans list view.
    HRESULT AddScanToListView( ProtectionProviderLib::IProtection_ConfigureableScan* scanToAdd );
	// Update the data for the specified scan in the listview
	HRESULT UpdateScanInListView( ProtectionProviderLib::IProtection_ConfigureableScan* scanToUpdate );

    // ** CONSTANTS **
    static const DWORD ScanListColumn_Name         = 0;
    static const DWORD ScanListColumn_Type         = 1;
    static const DWORD ScanListColumn_LastRun      = 2;
    static const DWORD ScanListColumn_NextRun      = 3;
    static const DWORD ScanListColumn_Actions      = 4;

    // ** DATA MEMBERS **
    // Controls
    CListCtrl           scanListCtrl;
    CButtonEx           createScanButton;
    // Quick scan block
    CStaticEx           quickScanTitle;
    CStaticEx           quickScanGraphic;
    CStaticEx           quickScanDescription;
    CStaticEx           quickScanDurationCaption;
    CStaticEx           quickScanDurationTime;
    CButtonEx           quickScanButton;
    // Full scan block
    CStaticEx           fullScanTitle;
    CStaticEx           fullScanGraphic;
    CStaticEx           fullScanDescription;
    CStaticEx           fullScanDurationCaption;
    CStaticEx           fullScanDurationTime;
    CButtonEx           fullScanButton;
    // Policy scan block
    CStaticEx           policyScanTitle;
    CStaticEx           policyScanGraphic;
    CStaticEx           policyScanDescription;
    CStaticEx           policyScanDurationCaption;
    CStaticEx           policyScanDurationTime;
    CButtonEx           policyScanButton;

    ProtectionProviderLib::IProtection_ConfigureableScan_Container*     scans;
};
