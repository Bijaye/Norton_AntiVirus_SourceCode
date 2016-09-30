////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ScanUI.h"
#include <ccEraserInterface.h>
#include "AVSubmissionLoader.h"
#include "OptNames.h"

// Help
#include <SymHelpLauncher.h>

// CED Includes
#include <isErrorLoader.h>
#include <isErrorIDs.h>
#include <isErrorResource.h> // error resource id's
#include <AVccModuleID.h>

// NPC Includes
#include <uiProviderInterface.h>
#include <uiNISDataElementGuids.h>

// ISData Includes
#include <isDataClientLoader.h>

extern ccSym::CDebugOutput g_DebugOutput;

using namespace avScanUI;
using namespace AVModule;
using namespace AvProd;

CScanUI::CScanUI(void) :
    m_eCurState((ManualScanner::eMSState)-1),
    m_bAutoStats(false),
    m_bScanAborted(false),
    m_eLowMode(eRiskMode_Normal),
    m_eMediumMode(eRiskMode_Normal),
    m_eHighMode(eRiskMode_Normal),
    m_bInEraserScan(true),
	m_hIcon(NULL)
{
    ZeroMemory(&m_scanStats, sizeof(m_scanStats));
    ZeroMemory(&m_guidScanInstanceId, sizeof(GUID));
	m_scanStats.nScanType = AVModule::ManualScanner::eScanType_Context; // default context scan
	m_scanStats.nScanResult = 1; // default scan aborted
}

CScanUI::~CScanUI(void)
{
    CCTRCTXI0(L"dtor");

    // Shut down our thread
    if(IsThread())
        Terminate(INFINITE);

	if(m_hIcon)
		DestroyIcon(m_hIcon);

	TRACE_REF_COUNT(m_spSkinWindow);
    TRACE_REF_COUNT(m_spFrameDoc);
    TRACE_REF_COUNT(m_spScanner);
    TRACE_REF_COUNT(m_spThreatTracker);
	TRACE_REF_COUNT(m_pSubmissions);
}

bool CScanUI::Initialize()
{
    // Create our unnamed scan done event
    m_evtScanDone.Create(NULL, TRUE, FALSE, NULL, FALSE);

	// Initialize the AV Submission Manager
	SYMRESULT sr = AVSubmit::AVSubmit_IAVSubmissionManagerMgd::CreateObject(GETMODULEMGR(), m_pSubmissions.m_p);
	if(SYM_SUCCEEDED(sr) && m_pSubmissions != NULL)
	{
		HRESULT hr = m_pSubmissions->Initialize(AVSubmit::IAVSubmissionManager::eAsynchronous);
		if(FAILED(hr))
		{
			CCTRCTXE1(L"Failed to init AVSubmission Mgr: 0x%08X", hr);
			//non-fatal error
		}
	}
	else
	{
		CCTRCTXE1(L"Failed to load AV Submission Mgr: 0x%08X", sr);
		//non-fatal error
	}

    // Init our locks
    m_mtxProgLock.Create(NULL, FALSE, NULL, FALSE);
    m_mtxStats.Create(NULL, FALSE, NULL, FALSE);
    
    return true;
}

bool CScanUI::SetScanName(LPCWSTR pszName)
{
    m_sScanTitle = pszName;
    return true;
}

void CScanUI::SetLowRiskMode(eRiskMode eMode)
{
    m_eLowMode = eMode;
}

void CScanUI::SetMediumRiskMode(eRiskMode eMode)
{
    m_eMediumMode = eMode;
}

void CScanUI::SetHighRiskMode(eRiskMode eMode)
{
    m_eHighMode = eMode;
}

avScanUI::eScanResult CScanUI::HandleScan(AVModule::IAVScanBase* pScanner, eUIMode eMode, NAVWRESULT& navWResult)
{
    CCTRCTXI0(L"--Enter");

    // Set thread options
    m_Options.m_bNoCRTThread = FALSE;
    m_Options.m_bPumpMessages = TRUE;
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eAutoModel;
    m_Options.m_bWaitForInitInstance = TRUE;

    // Save the UI mode
    m_eMode = eMode;

    // Save the scanner object
    m_spScanner = pScanner;

    // Create the UI thread
    if(!Create(NULL, 0, 0))
        return avScanUI::SRESULT_ERROR_SCANUI;

    // Wait for the UI thread to exit
    WaitForExit(INFINITE);
    CCTRCTXI0(L"UI Thread Exited ... cleaning up");

    // Set the result code
    navWResult = m_navWResult;

    // We are done with out copy of the scanner...
    // NOTE: we do a manual release here because the scanner might still have refs to us
    TRACE_REF_COUNT(m_spScanner);
    m_spScanner.Release();

    // Tell the threat tracker to cleanup its circular references
    // (it has us, we have it ... it has the scanner, the scanner has it)
    m_spThreatTracker->CleanupCircularRefs();

    // If the scan was aborted, return as such
    avScanUI::eScanResult eRes = avScanUI::SRESULT_OK;
    if(m_bScanAborted)
        eRes = avScanUI::SRESULT_ABORT;

    CCTRCTXI0(L"--Exit");
    return eRes;
}

void CScanUI::OnProgress(ManualScanner::eMSState eState, IAVMapStrData* pMapData)
{
    CCTRCTXI1(L"Called (eState=%d)", eState);

    ccLib::CSingleLock csl(&m_mtxProgLock, INFINITE, TRUE);
		
    // eMSState_ThreatDetected is a TEMPORARY state... it always immediately returns to the prior state.
    if(eState != m_eCurState && eState != ManualScanner::eMSState_ThreatDetected)
    {
		CAtlStringW sStatus;
        m_eCurState = eState;
        switch(eState)
        {
            case ManualScanner::eMSState_Inactive:
                m_bAutoStats = false;
                if(m_spFrameDoc)
                    m_spFrameDoc->StopProgress();
                sStatus.LoadString(IDS_STATUS_INACTIVE);
                break;
            case ManualScanner::eMSState_Starting:
                m_bAutoStats = false;
                sStatus.LoadString(IDS_STATUS_STARTING);
                break;
            case ManualScanner::eMSState_Active:
                m_bAutoStats = true;
                if(m_spFrameDoc)
                    m_spFrameDoc->StartProgress();

                if(m_bInEraserScan)
                    sStatus.LoadString(IDS_STATUS_ACTIVE_CCE);
                else
                    sStatus.LoadString(IDS_STATUS_ACTIVE);

                break;
            case ManualScanner::eMSState_Pausing:
                m_bAutoStats = true;
                sStatus.LoadString(IDS_STATUS_PAUSING);
                break;
            case ManualScanner::eMSState_Paused:
                m_bAutoStats = true;
                if(m_spFrameDoc)
                    m_spFrameDoc->StopProgress();
                sStatus.LoadString(IDS_STATUS_PAUSED);
                break;
            case ManualScanner::eMSState_Resuming:
                m_bAutoStats = true;
                sStatus.LoadString(IDS_STATUS_RESUMING);
                break;
            case ManualScanner::eMSState_Stopping:
                m_bAutoStats = false;
                sStatus.LoadString(IDS_STATUS_STOPPING);
                break;
            case ManualScanner::eMSState_Stopped:
                m_bAutoStats = false;
                if(m_spFrameDoc)
                    m_spFrameDoc->StopProgress();
                sStatus.LoadString(IDS_STATUS_STOPPED);
                break;
            case ManualScanner::eMSState_Finishing:
                m_bAutoStats = false;
                sStatus.LoadString(IDS_STATUS_FINISHING);
                break;
            case ManualScanner::eMSState_Done:
                m_bAutoStats = false;
                if(m_spFrameDoc)
                    m_spFrameDoc->StopProgress();
                sStatus.LoadString(IDS_STATUS_DONE);
                break;
            case ManualScanner::eMSState_TransitionToFileScan:
            {
                // No longer in an eraser scan
                m_bInEraserScan = false;
                break;
            }
            case ManualScanner::eMSState_Error:
            {
                sStatus.LoadString(IDS_STATUS_ERROR);
                if(pMapData)
                {
                    DWORD dwError = (DWORD)ManualScanner::eMSError_Unknown;
                    DWORD dwExtError;
                    pMapData->GetValue(ManualScanner::AV_SCANERROR_MAINERRORCODE, dwError);
                    pMapData->GetValue(ManualScanner::AV_SCANERROR_EXTERRORCODE, dwExtError);
					CCTRCTXE2(L"Scan errorCode: 0x%08X, extendedErrorCode: 0x%08X", dwError, dwExtError);

                    // What error is this?
                    bool bClose = true;
                    switch(dwError)
                    {
                        case ManualScanner::eMSError_InvalidVirusDefs:
                            ShowError(ERR_AVSUI_DEFS_INVALID, IDS_BAD_DEFS);
                            break;
                        case ManualScanner::eMSError_FeatureDisabled:
                            ShowError(ERR_AVSUI_FEATURE_DISABLED, IDS_FEATURE_DISABLED);
                            break;
                        case ManualScanner::eMSError_ccScanFailure:
                            ShowError(ERR_AVSUI_CCSCAN_FAILURE, 0);
                            break;
                        case ManualScanner::eMSError_ccEraserScanFailure:
                            ShowError(ERR_AVSUI_CCERASER_FAILURE, 0);
                            break;
                        case ManualScanner::eMSError_Exception:
                            ShowError(ERR_AVSUI_AVCOMP_EXCEPTION, 0);
                            break;
                        case ManualScanner::eMSError_Unknown:
                            ShowError(ERR_AVSUI_AVCOMP_UNKNOWN, 0);
                            break;
                        case ManualScanner::eMSError_ThreatProcessingTimeout:
                            // dwExtError is # of threats avscan was waiting for
                            bClose = false;
                            ShowError(ERR_AVSUI_AVCOMP_TPROCESSING_TIMEOUT, 0);
                            break;
                        case ManualScanner::eMSError_ccEraserVersionToOld:
                            ShowError(ERR_AVSUI_DEFS_TOO_OLD, IDS_OLD_DEFS);
                            break;
                        case ManualScanner::eMSError_FailedToLoadccScanIScannerw:
                            ShowError(ERR_AVSUI_CCSCAN_FAILLOAD, 0);
                            break;
                        case ManualScanner::eMSError_FailedToLoadccEraserIEraser:
                            ShowError(ERR_AVSUI_CCERASER_FAILLOAD, 0);
                            break;
                    }

                    // Mark the scan as done
                    if(m_spFrameDoc)
                    {
						m_spFrameDoc->ScanDone();

                        // Close our window if appropriate
                        if(bClose)
                            m_spFrameDoc->CloseDocument();
						}
                }
                break;
            }
            case ManualScanner::eMSState_ProcessingThreats:
            {
                m_bAutoStats = false;
                if(pMapData)
                {
                    DWORD dwCount = 0;
                    HRESULT hr = pMapData->GetValue(ManualScanner::AV_SCANPROCESSING_REMAININGTHREATS, dwCount);
                    LOG_FAILURE(L"Failed to get count of threats to be processed!", hr);
                    
                    // Get the correct status string...
                    if(dwCount == 0)
                    {
                        sStatus.LoadString(IDS_PROCESSINGTHREATS);
                        sStatus.Replace(L"%d", L"");
                    }
                    else if(dwCount == 1)
                    {
                        sStatus.LoadString(IDS_PROCESSINGTHREAT);
                    }
                    else
                    {
                        CStringW sFmt;
                        sFmt.LoadString(IDS_PROCESSINGTHREATS);
                        sStatus.Format(sFmt, dwCount);
                    }

                    // Switch the frame in to "processing" mode...
                    if(m_spFrameDoc)
                        m_spFrameDoc->ProcessingMode(dwCount);
                }                
            }
        }

        // Update the status
        if(m_spFrameDoc && !sStatus.IsEmpty())
            m_spFrameDoc->SetStatus(sStatus);
    }

    if(eState == ManualScanner::eMSState_ThreatDetected)
    {
        // A new threat was found, process it and add it to the list(s)
        IAVMapDwordDataPtr spMapThreat;
        pMapData->GetValue(ManualScanner::AV_SCANTHREAT_DETECTIONDATA, AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMapThreat));
        if(spMapThreat)
        {
            // If we are done scanning and are just processing threats, we need to take a few extra actions
            if(m_eCurState == ManualScanner::eMSState_ProcessingThreats)
            {
                // Update the progress
                if(m_spFrameDoc)
                    m_spFrameDoc->ThreatProcessed();

                // Manually update the stats (once in this state, the stats are no longer updated by AVComp)
                UpdateScanStats();
            }

            bool bAddToMap = true;

            // Don't add suspicious COH threats
            ccEraser::IAnomaly::Heuristic_Rank eRank = ccEraser::IAnomaly::Positive;
            HRESULT hr = spMapThreat->GetValue(ThreatTracking::TrackingData_Anomaly_HeuristicRank, (DWORD&)eRank);
            if(SUCCEEDED(hr) && eRank == ccEraser::IAnomaly::Suspicious)
            {
                CCTRCTXW0(L"Found suspicious COH threat... not adding to threattracker, but still queueing for submission");
                bAddToMap = false;
            }

            // Don't add clean stealth file
            ThreatTracking::eThreatTypes eType;
            hr = spMapThreat->GetValue(ThreatTracking::TrackingData_THREAT_TYPE, (DWORD&)eType);
            if(SUCCEEDED(hr) && eType == ThreatTracking::ThreatType_CleanStealthFile)
            {
                CCTRCTXW0(L"Found clean stealth file... not adding to threattracker, but still queueing for submission");
                bAddToMap = false;
            }

            // Don't add stealthed threats
            ThreatTracking::eThreatState eCurState;
            hr = spMapThreat->GetValue(ThreatTracking::TrackingData_CURRENT_STATE, (DWORD&)eCurState);
            if(SUCCEEDED(hr) && eCurState == ThreatTracking::ThreatState_CannotRemediate)
            {
                CCTRCTXW0(L"Found stealthed threat... not adding to threattracker, but still queueing for submission");
                bAddToMap = false;
            }

            bool bAdded = false;
            if(bAddToMap)
            {
                // Add to the threat tracker.
                //
                bAdded = HandleThreat(spMapThreat, false);
            }

            // Info logging
            if(bAdded && g_DebugOutput.GetDebugType() == ccLib::CDebugOutput::eInformationDebug)
            {
                // Output info about the threat that was detected...
                size_t iThreat = m_spThreatTracker->GetThreatCount() - 1;
                CStringW sOutput = m_spThreatTracker->GenerateOutputForItem(L"Threat Detected: Name=%{NAME}%, VID=%{VID}%, STATE=%{STATE}%, TYPE=%{TYPE}%, GUID=%{TID}%", iThreat);
                sOutput.Insert(0, CCTRCTX);
                CCTRACEI(sOutput);
            }

			// Add to the submission if the necessary conditions are met.
			//
			if(m_pSubmissions != NULL)
			{
				// The Submission Manager will determine which type of submissions to generate and submit, if any.
				HRESULT hr = m_pSubmissions->Add(spMapThreat);
				if(FAILED(hr))
					CCTRCTXE1(L"Failed to queue detection for submission: 0x%08X", hr);
			}
			else
			{
				CCTRACEE(CCTRCTX L"ThreatDetected State without Submission object");
			}
        }
        else
		{
            CCTRACEE(CCTRCTX L"ThreatDetected State without AV_SCANTHREAT_DETECTIONDATA");
		}
    }

    if(eState == ManualScanner::eMSState_Active && pMapData)
    {
        ScanStatistics scanStats;
        GetScanStats(scanStats, true);

        cc::IStringPtr spFile;
        if(m_spFrameDoc && SUCCEEDED(pMapData->GetValue(ManualScanner::AV_SCANPROGRESS_CURRENTITEM, spFile)))
        {
            m_spFrameDoc->SetCurFile(spFile->GetStringW());
        }

        // Try to get all the stats
        DWORD dwTmpVal;
        if(SUCCEEDED(pMapData->GetValue(ManualScanner::AV_SCANPROGRESS_ITEMSSCANNED, dwTmpVal)))
        {
            CCTRCTXI1(L"ITEMSSCANNED=%d", dwTmpVal);
            scanStats.nTotalScanned = dwTmpVal;
        }
        if(SUCCEEDED(pMapData->GetValue(ManualScanner::AV_SCANPROGRESS_VIRSDETECTED, dwTmpVal)))
            scanStats.nVirDetected = dwTmpVal;
        if(SUCCEEDED(pMapData->GetValue(ManualScanner::AV_SCANPROGRESS_VIRSREMOVED, dwTmpVal)))
            scanStats.nVirRemoved = dwTmpVal;
        if(SUCCEEDED(pMapData->GetValue(ManualScanner::AV_SCANPROGRESS_NONVIRALSDETECTED, dwTmpVal)))
            scanStats.nNonVirDetected = dwTmpVal;
        if(SUCCEEDED(pMapData->GetValue(ManualScanner::AV_SCANPROGRESS_NONVIRALSREMOVED, dwTmpVal)))
            scanStats.nNonVirRemoved = dwTmpVal;

        // Get the eraser stats object
        AVModule::IAVMapDwordDataPtr spEraserStatsMap;
        if(SUCCEEDED(pMapData->GetValue(ManualScanner::AV_SCANPROGRESS_ERASERSTATS, AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spEraserStatsMap))))
            UpdateSubStats(spEraserStatsMap, scanStats);

        // Update the stats...
        SetScanStats(scanStats);
    }

}

bool CScanUI::HandleThreat(AVModule::IAVMapDwordData* pThreatMap, bool bUpdateOnly)
{
    // Get the overall risk level for this threat
    eRiskMode eMode = eRiskMode_Normal;
    ThreatTracking::eThreatLevels eLevel = (ThreatTracking::eThreatLevels)-1;
    HRESULT hr = pThreatMap->GetValue(ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
    LOG_FAILURE(L"Failed to get overall risk level for threat ... taking it as received from CAV", hr);
    if(SUCCEEDED(hr))
    {
        // Get the correct mode...
        switch(eLevel)
        {
            case ThreatTracking::ThreatLevel_Low:
                eMode = m_eLowMode;
                break;
            case ThreatTracking::ThreatLevel_Med:
                eMode = m_eMediumMode;
                break;
            case ThreatTracking::ThreatLevel_High:
                eMode = m_eHighMode;
                break;
        }
    }

    // Add the threat
    size_t iNewIndex = -1;
    bool bAdded = m_spThreatTracker->AddThreat(pThreatMap, bUpdateOnly, &iNewIndex);
    if(!bAdded)
        CCTRCTXE0(L"Failed to add threat to threat tracker");

    // Handle the threat, based on mode
    switch(eMode)
    {
        case eRiskMode_Normal:
        {
            // Nothing special to do ... just Refresh the threat list
            RefreshThreatLists();
            break;
        }
        case eRiskMode_IgnoreOnce:
        {
            // Set the state to "Ignored Once"
            m_spThreatTracker->SetThreatAction(iNewIndex, CThreat::eIntAct_Ignore);
            m_spThreatTracker->TakeThreatAction(iNewIndex, m_wndFrame.m_hWnd);

            // TakeThreatAction() will call RefreshThreatList()
            break;
        }
        case eRiskMode_Exclude:
        {
            // Exclude this item
            m_spThreatTracker->SetThreatAction(iNewIndex, CThreat::eIntAct_Exclude);
            m_spThreatTracker->TakeThreatAction(iNewIndex, m_wndFrame.m_hWnd);
            // TakeThreatAction() will call RefreshThreatList()
            break;
        }
    }

    return bAdded;
}

void CScanUI::OnScanComplete(AVModule::IAVMapStrData* pResults)
{
    CCTRCTXI0(L"--Enter");
    ccLib::CSingleLock csl(&m_mtxProgLock, INFINITE, TRUE);

    // Stats are no longer free...
    m_bAutoStats = false;

    if(m_spFrameDoc)
    {
        CStringW sText;
        sText.LoadString(IDS_STATUS_DONE);

        m_spFrameDoc->SetStatus(sText);
        m_spFrameDoc->ScanDone();
    }

    // Get the final stats
    if(pResults)
    {
        // Was the scan aborted?
        HRESULT hr = pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_ABORT, m_bScanAborted);

        ScanStatistics scanStats;
        GetScanStats(scanStats, true);

        // Try to get all the stats
        DWORD dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_ITEMCOUNT, dwTmpVal)))
            scanStats.nTotalScanned = dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_VIRSDETECTED, dwTmpVal)))
            scanStats.nVirDetected = dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_VIRSREMOVED, dwTmpVal)))
            scanStats.nVirRemoved = dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_NONVIRALSDETECTED, dwTmpVal)))
            scanStats.nNonVirDetected = dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_NONVIRALSREMOVED, dwTmpVal)))
            scanStats.nNonVirRemoved = dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_SCANTIME, dwTmpVal)))
            scanStats.nScanTime = dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_SCANTYPE, dwTmpVal)))
            scanStats.nScanType = dwTmpVal;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_ABORT, dwTmpVal)))
            scanStats.nScanResult = dwTmpVal;

        // Get the eraser stats object
        AVModule::IAVMapDwordDataPtr spEraserStatsMap;
        if(SUCCEEDED(pResults->GetValue(ManualScanner::AV_SCANCOMPLETE_ERASERSTATS, AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spEraserStatsMap))))
            UpdateSubStats(spEraserStatsMap, scanStats);

        // Pass the results map to threat tracker (for export)
        m_spThreatTracker->SetScanResults(pResults);

        // Update the stats...
        SetScanStats(scanStats);

	}

    // Restore our window (if minimized)
    if(m_wndFrame.IsWindow() && m_wndFrame.IsIconic())
    {
        m_wndFrame.ShowWindow(SW_RESTORE);
    }
    
    // Signal the scan done event
    m_evtScanDone.SetEvent();

    if(m_spFrameDoc)
    {
        m_spFrameDoc->LastCallback();
    }

    CCTRCTXI0(L"--Exit");
}

void CScanUI::OnThreatNotification(ThreatTracking::ActionRequiredFlag flagARThreatType, IAVMapDwordData* pThreat)
{
    // Make sure this is the notification we want
    if((flagARThreatType & ThreatTracking::THREAT_RESOLVED) == ThreatTracking::THREAT_RESOLVED)
    {
        // Update this threat in the threat map
        HandleThreat(pThreat, true);

        // Manually update the stats (once in this state, the stats are no longer updated by AVComp)
        UpdateScanStats();
    }
}

void CScanUI::UpdateSubStats(AVModule::IAVMapDwordData* pMap, ScanStatistics& scanStats)
{
    if(!pMap)
        return;

    // Try to get all the stats
    DWORD dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::RegistryDetectionActionType, dwTmpVal)))
        scanStats.nRegistryItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::FileDetectionActionType, dwTmpVal)))
        scanStats.nFileItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::ProcessDetectionActionType, dwTmpVal)))
        scanStats.nProcessItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::BatchDetectionActionType, dwTmpVal)))
        scanStats.nBatchItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::INIDetectionActionType, dwTmpVal)))
        scanStats.nINIItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::ServiceDetectionActionType, dwTmpVal)))
        scanStats.nServiceItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::StartupDetectionActionType, dwTmpVal)))
        scanStats.nStartupItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::COMDetectionActionType, dwTmpVal)))
        scanStats.nCOMItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::HostsDetectionActionType, dwTmpVal)))
        scanStats.nHostsItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::DirectoryDetectionActionType, dwTmpVal)))
        scanStats.nDirectoryItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::LSPDetectionActionType, dwTmpVal)))
        scanStats.nLSPItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::BrowserCacheDetectionActionType, dwTmpVal)))
        scanStats.nBrowserCacheItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::CookieDetectionActionType, dwTmpVal)))
        scanStats.nCookieItems = dwTmpVal;
    if(SUCCEEDED(pMap->GetValue(ccEraser::ApplicationHeuristicDetectionActionType, dwTmpVal)))
        scanStats.nApplicationHeuristicItems = dwTmpVal;
}

int CScanUI::Run()
{
    CCTRCTXI0(L"--UI Thread Begin");

    AVModule::IAVProgressSinkQIPtr spUISync(this);
    if(!spUISync)
    {
        LOG_FAILURE(L"Failed to QI for IAVProgressSink", E_NOINTERFACE);
        m_navWResult = NAVWRESULT_ERROR;
        return -1;
    }

    avScanUI::IScanUICallbacksIntQIPtr spCallbacks(this);
    if(!spCallbacks)
    {
        LOG_FAILURE(L"Failed to QI for IScanUICallbacksInt", E_NOINTERFACE);
        m_navWResult = NAVWRESULT_ERROR;
        return -1;
    }

    // Setup the threat tracker
    m_spThreatTracker = new CThreatTracker(m_spScanner, spCallbacks);
    if(!m_spThreatTracker)
    {
        m_navWResult = NAVWRESULT_ERROR;
        return -1;
    }

    // Register for threat resolved notifications
    SYMRESULT symRes = m_ldrThreatInfo.Initialize();
    SYM_LOG_FAILURE(L"Failed to init the IThreatInfo loader", symRes);
    IAVThreatInfoPtr spThreatInfo;
    symRes = m_ldrThreatInfo.CreateObject(spThreatInfo);
    SYM_LOG_FAILURE(L"Failed to create an IThreatInfo object", symRes);

    HRESULT hr;
    if(spThreatInfo)
    {
        CCTRCTXI0(L"Registering ThreatActionRequiredSink");
        hr = spThreatInfo->RegisterThreatActionRequiredSink(ThreatTracking::THREAT_RESOLVED, this);
        LOG_FAILURE(L"RegisterThreatActionRequiredSink failed!", hr);
    }

    // Normal and No-Results modes need use the full UI...
    // Automation mode just waits on the "finished" event
    // Infections-Only mode just waits on the "finished" event, then
    //   displays the result UI if any infections were found
    int nRet = 0;
    if(m_eMode == eUIMode_Normal || m_eMode == eUIMode_NoResults)
    {
        // We want eraser stats (only for visible scan)...
        AVModule::IAVMapStrDataPtr spOptions;
        hr = m_spScanner->GetOptions(spOptions);
        LOG_FAILURE(L"Failed to get scanner options", hr);
        if(SUCCEEDED(hr))
        {
            hr = spOptions->SetValue(ManualScanner::AV_SCANOPTION_PROVIDEERASERSTATS, true);
            LOG_FAILURE(L"Failed to enable eraser stats", hr);

            hr = ::CoCreateGuid(&m_guidScanInstanceId);
            LOG_FAILURE(L"Failed to create GUID", hr);
            if(SUCCEEDED(hr))
            {
                hr = spOptions->SetValue(ManualScanner::AV_SCANOPTION_SCANID, m_guidScanInstanceId);
                LOG_FAILURE(L"Failed to set scan instance id", hr);
            }
        }

        // Create the main document
        m_spFrameDoc = new CFrameDocument(m_spScanner);
        
        // Show the scan UI
        TRACE_REF_COUNT(this);
        nRet = ShowScanUI(true, spUISync);
        TRACE_REF_COUNT(this);
    }
    else
    {
        // For all forms of silent scan, we don't want progress (except state changes)
        AVModule::IAVMapStrDataPtr spOptions;
        hr = m_spScanner->GetOptions(spOptions);
        LOG_FAILURE(L"Failed to get scanner options", hr);
        if(SUCCEEDED(hr))
        {
            hr = spOptions->SetValue(ManualScanner::AV_SCANOPTION_PROGINTERVAL, (DWORD)0);
            LOG_FAILURE(L"Failed to set progress interval", hr);

            hr = ::CoCreateGuid(&m_guidScanInstanceId);
            LOG_FAILURE(L"Failed to create GUID", hr);
            if(SUCCEEDED(hr))
            {
                hr = spOptions->SetValue(ManualScanner::AV_SCANOPTION_SCANID, m_guidScanInstanceId);
                LOG_FAILURE(L"Failed to set scan instance id", hr);
            }
        }
        
        // Start the scan
        hr = m_spScanner->Scan(spUISync);
        if(SUCCEEDED(hr))
        {
            // All other modes wait for the finished event
            ccLib::CMessageLock msgLock(TRUE, FALSE);
            DWORD dwResult = msgLock.Lock(m_evtScanDone, INFINITE);
            if(dwResult == WAIT_OBJECT_0)
            {
                // If this is Infection-Only mode, and we have infections, 
                // .OR. if this is OnlyResults mode (infections or not),
                // we need to display the results UI
                if(((m_navWResult == NAVWRESULT_INFECTIONS_REMAIN
                     || m_navWResult == NAVWRESULT_ALLINFECTIONS_CLEAN)
                    && m_eMode == eUIMode_OnlyInfections)
                   || m_eMode == eUIMode_OnlyResults)
                {
                    // Create the main document in "results only mode"
                    m_spFrameDoc = new CFrameDocument(m_spScanner, true);
                   
                    // Show the scan UI
                    nRet = ShowScanUI(false, spUISync);
                }
            }
            else
            {
                CCTRACEE(CCTRCTX L"CMessageLock::Lock() failed! (dwResult=%d)", dwResult);
                nRet = NAVWRESULT_ERROR;
            }
        }
        else
            m_navWResult = NAVWRESULT_ERROR;
    }

    // Copy the abort flag before we release the frame doc
    if(m_spFrameDoc)
    {
        m_bScanAborted = m_spFrameDoc->GetScanAborted();
    }

    // We are done with the frame document... and it is holding refs to us
    // So, we will release it here instead of waiting for the destructor
    // (which would never get called)
    TRACE_REF_COUNT(m_spFrameDoc);
    m_spFrameDoc.Release();

    // Unregister for threat resolved notifications
    if(spThreatInfo)
    {
        CCTRCTXI0(L"Unregistering ThreatActionRequiredSink");
		hr = spThreatInfo->UnregisterThreatActionRequiredSink();
        LOG_FAILURE(L"UnregisterThreatActionRequiredSink failed!", hr);
    }

    // Dump a few refcounts
	TRACE_REF_COUNT(m_spSkinWindow);
    TRACE_REF_COUNT(m_spScanner);
    TRACE_REF_COUNT(m_spThreatTracker);
	TRACE_REF_COUNT(m_pSubmissions);

    // End of thread
    CCTRCTXI0(L"--UI Thread End");
    return nRet;
}

int CScanUI::ShowScanUI(bool bStartScan, AVModule::IAVProgressSink* pUISync)
{
    CCTRCTXI0(L"--Enter");
    if(!m_spFrameDoc || !m_spScanner)
        return -1;

    // Create a message loop and add it to our module
    CMessageLoop msgLoop;
    _Module.AddMessageLoop(&msgLoop);

    // Create the skin engine
    if(!InitSkinEngine())
    {
        // This error is NOT fatal!  We just will not look as good.
        CCTRACEE(CCTRCTX L"Failed to initialize SymTheme!");
    }

    // Load our window title
    CStringW sTitle;
    sTitle = CISVersion::GetProductName();
    if(!m_sScanTitle.IsEmpty())
        sTitle += L" - " + m_sScanTitle;

	// Load our icons
    CISVersion::GetProductIcon(m_hIcon);

    // Pass on the threat tracker
    m_spFrameDoc->SetThreatTracker(m_spThreatTracker);

    // Create our scan UI
    m_wndFrame.SetMainDocument(m_spFrameDoc);
    m_wndFrame.CreateEx();
    m_wndFrame.SetWindowText(sTitle);
	m_wndFrame.SetIcon(m_hIcon);
    m_wndFrame.ShowWindow(SW_SHOW);

    // Skin the scan window (with the dialog skin)
    HRESULT hr;
    if(m_spSkinWindow)
    {
        // This error is NOT fatal!  We just will not look as good.
        hr = m_spSkinWindow->SetWindowSkin(m_wndFrame, symtheme::skins::common::mainframe_logobottom);
        LOG_FAILURE(L"Failed to apply the loaded SymTheme to our window!", hr);

        // This error is NOT fatal!  We just will not look as good.
        hr = m_spSkinWindow->SetDialogSkin(symtheme::skins::common::dialog);
        LOG_FAILURE(L"Failed to apply the loaded SymTheme to dialogs!", hr);
    }

    // Start the scan (if needed)
    if(bStartScan)
        hr = m_spScanner->Scan(pUISync);
    else
        hr = S_OK;

    // Pump messages
    int nRet = -1;
    if(SUCCEEDED(hr))
    {
        // Run the main msg loop
        nRet = msgLoop.Run();
    }
    else
        m_navWResult = NAVWRESULT_ERROR;

    // Remove our message loop from the module
    _Module.RemoveMessageLoop();

    // The mainfrm is now done with the document
    m_wndFrame.ReleaseMainDocument();

    CCTRCTXI0(L"--Exit");
    return nRet;
}

bool CScanUI::InitSkinEngine()
{
    SYMRESULT symRes = SYM_OK;
	if(m_spSkinWindow != NULL)
	{
		CCTRCTXI0(_T("Skin Engine already initialized"));
		return true;
	}

    symtheme::ISymSkinWindowPtr spSkinWindow;
    symRes = symtheme::loader::ISymSkinWindow::CreateObject(GETMODULEMGR(), &spSkinWindow);
    if(SYM_FAILED(symRes))
    {
        CCTRACEE(CCTRCTX L"Unable to create SymTheme object");
        ATLASSERT(FALSE);
        return false;
    }

    // Init the skin engine
    HRESULT hr = spSkinWindow->Init(_Module.GetModuleInstance(), L"avScanUI"); 
    if(FAILED(hr))
    {
        LOG_FAILURE(L"Unable to init the SymTheme object", hr);
        ATLASSERT(FALSE);
        return false;
    }

    // Load our skin
    hr = spSkinWindow->LoadSkin(symtheme::skins::common::SKINID_SymantecCommon);
    if(FAILED(hr))
    {
        LOG_FAILURE(L"Unable to load our SymTheme SMF file", hr);

		//Be sure the SkinMagic library unregisters all window hooks. The DLL will be unloaded shortly.
		spSkinWindow->Exit();

        ATLASSERT(FALSE);
        return false;
    }

    m_spSkinWindow = spSkinWindow;
    return true;
}

void CScanUI::RefreshThreatLists()
{
    if(m_spFrameDoc)
        m_spFrameDoc->RefreshThreatList();
}

void CScanUI::SetScanStats(const ScanStatistics& scanStats)
{
    // Lock access to the stats
    ccLib::CSingleLock csl(&m_mtxStats, INFINITE, TRUE);

    // Save the stats
    m_scanStats = scanStats;

    // Re-get the possibly adjusted stats
    ScanStatistics scanStatsAdjusted;
    GetScanStats(scanStatsAdjusted);

    // Figure out *CURRENT* return code (can change every time we have new stats)...
    int nDetected = scanStatsAdjusted.nVirDetected + scanStatsAdjusted.nNonVirDetected;
    int nRemoved = scanStatsAdjusted.nVirRemoved + scanStatsAdjusted.nNonVirRemoved;
    if(nDetected == 0)
        m_navWResult = NAVWRESULT_NOINFECTIONS;
    else if(nDetected != 0 && nDetected > nRemoved)
        m_navWResult = NAVWRESULT_INFECTIONS_REMAIN;
    else if(nDetected != 0 && nDetected == nRemoved)
        m_navWResult = NAVWRESULT_ALLINFECTIONS_CLEAN;

    // Pass on the stats to the frame doc (if we have one)...
    if(m_spFrameDoc)
        m_spFrameDoc->RefreshScanStats(scanStatsAdjusted);
}

void CScanUI::GetScanStats(ScanStatistics& scanStats, bool bRaw)
{
    // Lock access to the stats
    ccLib::CSingleLock csl(&m_mtxStats, INFINITE, TRUE);

    // Return our stats
    scanStats = m_scanStats;

    // Subtract the COH items that we are hiding
    if(!bRaw)
    {
        // Always use out internal found and resolved counts
        m_spThreatTracker->GetThreatCountEx(scanStats.nVirDetected, scanStats.nNonVirDetected, CThreat::eTTState_All);
        m_spThreatTracker->GetThreatCountEx(scanStats.nVirRemoved, scanStats.nNonVirRemoved, CThreat::eTTState_Done);
    }
}

HRESULT CScanUI::GetScanTitle(cc::IString*& pScanTitleString)
{
	cc::IStringPtr spTempScanTitleString;
	spTempScanTitleString.Attach(ccSym::CStringImpl::CreateStringImpl((LPCWSTR)m_sScanTitle));
	if(!spTempScanTitleString)
		return E_OUTOFMEMORY;

    // Release existing item
    if(pScanTitleString)
    {
        pScanTitleString->Release();
        pScanTitleString = NULL;
    }

	SYMRESULT sr = spTempScanTitleString->QueryInterface(cc::IID_String, (void**)&pScanTitleString);
	return SYM_SUCCEEDED(sr) ? S_OK : E_NOINTERFACE;
}

HRESULT CScanUI::GetScanInstanceId(REFGUID guidScanInstanceId)
{
    memcpy((LPVOID)&guidScanInstanceId, &m_guidScanInstanceId, sizeof(GUID));
    return S_OK;
}

void CScanUI::UpdateScanStats()
{
    // Get the current stats
    ScanStatistics scanStats;
    GetScanStats(scanStats);

    // Refresh
    if(m_spFrameDoc)
        m_spFrameDoc->RefreshScanStats(scanStats);
}

HRESULT CScanUI::ShowError(DWORD dwErrorId, DWORD dwResId)
{
	HRESULT hr = S_OK;
	ISShared::ISShared_IError loader;
	ISShared::IErrorPtr spError;
    LPCWSTR pszMsg = NULL;
    CStringW sMsg;

    if(dwResId == 0)
    {
        // No resource ID specified, use the generic text
        pszMsg = NULL;
        dwResId = IDS_ISSHARED_ERROR_INTERNAL;
    }
    else
    {
        CStringW sFmt;
        CStringW sBrandingProductName = CISVersion::GetProductName();

        // Load the custom error string
        sFmt.LoadString(dwResId);

        // Put the product title in the error
        sMsg.Format(sFmt, sBrandingProductName.GetString());

        // Set our vars
        pszMsg = sMsg;
        dwResId = 0;
    }

	if(SYM_SUCCEEDED(loader.CreateObject(GETMODULEMGR(), &spError)))
	{
		hr = spError->Show(AV_MODULE_ID_AVSCANUI, dwErrorId, dwResId, NULL, pszMsg, S_OK, true, true, NULL);
	}

	return hr;
}

HRESULT CScanUI::LaunchHelp(DWORD dwHelpID, HWND hParentWnd)
{
    CCTRCTXI1(L"Launching help with ID %d", dwHelpID);

    isshared::CHelpLauncher Help;
    HWND hWndHelp = Help.LaunchHelp(dwHelpID, hParentWnd);
    if(!hWndHelp)
    {
        CCTRCTXE0(L"Failed to launch Help");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CScanUI::SwitchToTab(LPCSTR pszTabID)
{
    m_wndFrame.SwitchToTab(pszTabID);
    return S_OK;
}

HRESULT CScanUI::LaunchMsgCenter()
{
    HRESULT hr = S_OK;
    ISShared::ISShared_IProvider ProviderLoader;
    ui::IProviderPtr spISElementProvider;

	if (SYM_SUCCEEDED(ProviderLoader.CreateObject(GETMODULEMGR(), &spISElementProvider)))
	{
		ui::IElementPtr pElement;
		hr = spISElementProvider->GetElement(ISShared::CLSID_NIS_ManageQuarantine, pElement);
		if (SUCCEEDED(hr))
			hr = pElement->Configure(::GetDesktopWindow(), NULL);
		else
			LOG_FAILURE(L"Failed to get Quarantine element", hr);
	}
	else
		CCTRCTXE0(L"Failed to create the element provider");
	
	return hr;
}
