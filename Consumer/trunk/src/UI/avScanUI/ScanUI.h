////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ScanUIInterfaces.h>
#include <AVInterfaceLoader.h>
#include <AVDefines.h>
#include <SymTheme.h>
#include "FrameDocument.h"
#include <resource.h>
#include "mainfrm.h"
#include "ThreatTracker.h"
#include "avScanUIInternalInterfaces.h"
#include "AVSubmissionInterface.h"
#include <AvProdLoggingLoader.h>

namespace avScanUI
{
// CScanUI
class CScanUI :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public avScanUI::IScanUI,
    public AVModule::IAVProgressSink,
    public avScanUI::IScanUICallbacksInt,
    public ccLib::CThread,
    public AVModule::IAVThreatActionRequiredSink
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(avScanUI::IID_ScanUI, avScanUI::IScanUI)
        SYM_INTERFACE_ENTRY(AVModule::IID_AVProgressSink, AVModule::IAVProgressSink)
        SYM_INTERFACE_ENTRY(avScanUI::IID_ScanUICallbacksInt, avScanUI::IScanUICallbacksInt)
        SYM_INTERFACE_ENTRY(AVModule::IID_AVThreatActionRequiredSink, AVModule::IAVThreatActionRequiredSink)
    SYM_INTERFACE_MAP_END()

public:
    CScanUI(void);
    virtual ~CScanUI(void);

    // IScanUI methods:
    virtual bool Initialize() throw();
    virtual bool SetScanName(LPCWSTR pszName) throw();
    virtual void SetLowRiskMode(eRiskMode eMode) throw();
    virtual void SetMediumRiskMode(eRiskMode eMode) throw();
    virtual void SetHighRiskMode(eRiskMode eMode) throw();
    virtual avScanUI::eScanResult HandleScan(AVModule::IAVScanBase* pScanner, eUIMode eMode, NAVWRESULT& navWResult) throw();

    // IAVProgressSink methods:
    virtual void OnProgress(AVModule::ManualScanner::eMSState eState, AVModule::IAVMapStrData* pMapData) throw();
    virtual void OnScanComplete(AVModule::IAVMapStrData* pResults) throw();

    // IScanUICallbacksInt methods:
    virtual void RefreshThreatLists();
    virtual void SetScanStats(const ScanStatistics& scanStats);
    virtual void GetScanStats(ScanStatistics& scanStats, bool bRaw = false);
    virtual void UpdateScanStats();
    virtual HRESULT ShowError(DWORD dwErrorId, DWORD dwResId);
    virtual HRESULT LaunchHelp(DWORD dwHelpID, HWND hParentWnd);
    virtual HRESULT SwitchToTab(LPCSTR pszTabID);
    virtual HRESULT LaunchMsgCenter();
	virtual HRESULT GetScanTitle(cc::IString*& pScanTitleString);
    virtual HRESULT GetScanInstanceId(REFGUID guidScanInstanceId);

    // IAVThreatActionRequiredSink methods:
    virtual void OnThreatNotification(AVModule::ThreatTracking::ActionRequiredFlag flagARThreatType, AVModule::IAVMapDwordData* pThreat) throw();

protected:
    virtual int Run() throw();
    bool InitSkinEngine();
    void UpdateSubStats(AVModule::IAVMapDwordData* pMap, ScanStatistics& scanStats);
    int ShowScanUI(bool bStartScan, AVModule::IAVProgressSink* pUISync);
    bool HandleThreat(AVModule::IAVMapDwordData* pThreatMap, bool bUpdateOnly);

protected:
	symtheme::ISymSkinWindowPtr m_spSkinWindow;
    CMainFrame m_wndFrame;
    CSymPtr<CFrameDocument> m_spFrameDoc;
    AVModule::ManualScanner::eMSState m_eCurState;
    AVModule::IAVScanBasePtr m_spScanner;
    AVModule::AVLoader_IAVThreatInfo m_ldrThreatInfo;
    eUIMode m_eMode;
    ccLib::CEvent m_evtScanDone;
    NAVWRESULT m_navWResult;
    CSymPtr<CThreatTracker> m_spThreatTracker;
	AVSubmit::IAVSubmissionManagerPtr m_pSubmissions;
    CStringW m_sScanTitle;
    ccLib::CMutex m_mtxProgLock;
	HICON m_hIcon;
    bool m_bScanAborted;
    bool m_bInEraserScan;

    // Risk modes
    eRiskMode m_eLowMode;
    eRiskMode m_eMediumMode;
    eRiskMode m_eHighMode;

    // Scan stats
    ScanStatistics m_scanStats;
    ccLib::CMutex m_mtxStats;
    bool m_bAutoStats;
    GUID m_guidScanInstanceId;
};

} // namespace