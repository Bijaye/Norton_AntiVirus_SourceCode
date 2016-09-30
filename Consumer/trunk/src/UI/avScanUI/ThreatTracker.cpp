////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ThreatTracker.h"
#include <ccEraserInterface.h>
#include <ccSymMemoryStreamImpl.h>
#include <ccSymModuleLifetimeMgrHelper.h>
#include <SymHTMLLoader.h>
#include "RiskDetailsDlg.h"
#include <resource.h>
#include <ThreatCatInfo.h>
#include "ScanUIMisc.h"
#include <ISResource.h> // IS Shared resources
#include <isSymTheme.h>

#define GUID_FORMAT_STR(xGUID) L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", \
                                xGUID.Data1, xGUID.Data2, xGUID.Data3, \
                                xGUID.Data4[0], xGUID.Data4[1], xGUID.Data4[2], xGUID.Data4[3], \
                                xGUID.Data4[4], xGUID.Data4[5], xGUID.Data4[6], xGUID.Data4[7]
#define THREAT_FAILURE(xID, xMsg, xHR)  CStringW sGUID; \
                                        sGUID.Format(GUID_FORMAT_STR(xID)); \
                                        CCTRACEE(CCTRCTX xMsg L" - TID=%s, HR=%08X", sGUID, xHR);

using namespace AVModule;

CThreatTracker::CThreatTracker(AVModule::IAVScanBase* pScanner, avScanUI::IScanUICallbacksInt* pScanUI) :
    m_mtxData(NULL, FALSE, NULL, FALSE),
    m_eProcTermRes(avScanUI::eProcTerm_Error)
{
    m_bSinkRegistered = false;
    m_spScanner = pScanner;
    m_spScanUI = pScanUI;

    if(m_spScanner)
    {
        HRESULT hr = m_spScanner->RegisterThreatProcessingSink(this);
        if(SUCCEEDED(hr))
            m_bSinkRegistered = true;
    }

    SYMRESULT symRes = m_loaderMapDwordData.Initialize();
    SYM_LOG_FAILURE(L"Failed to init the IAVMapDwordData loader", symRes);
}

CThreatTracker::CThreatTracker(AVModule::IAVThreatInfo* pThreatInfo, bool bRegisterSink) :
    m_mtxData(NULL, FALSE, NULL, FALSE),
    m_eProcTermRes(avScanUI::eProcTerm_Error)
{
    m_spThreatInfo = pThreatInfo;
    m_bSinkRegistered = false;

    if(m_spThreatInfo && bRegisterSink)
    {
        HRESULT hr = m_spThreatInfo->RegisterThreatProcessingSink(this);
        if(SUCCEEDED(hr))
            m_bSinkRegistered = true;
    }

    SYMRESULT symRes = m_loaderMapDwordData.Initialize();
    SYM_LOG_FAILURE(L"Failed to init the IAVMapDwordData loader", symRes);
}

CThreatTracker::~CThreatTracker(void)
{
    CCTRCTXI0(L"dtor");
    CleanupCircularRefs();
}

void CThreatTracker::CleanupCircularRefs()
{
    // Trace all ref counts
    TRACE_REF_COUNT(m_spScanner);
    TRACE_REF_COUNT(m_spThreatInfo);
    TRACE_REF_COUNT(m_spScanUI);
    TRACE_REF_COUNT(m_spScanResults);
    TRACE_REF_COUNT(m_spDlgDoc);

    // Start cleaning up...
    if(m_bSinkRegistered && m_spScanner)
        m_spScanner->UnRegisterThreatProcessingSink();
    if(m_bSinkRegistered && m_spThreatInfo)
        m_spThreatInfo->UnRegisterThreatProcessingSink();
    
    // If we had a sink, we just unregistered it
    m_bSinkRegistered = false;

    // We are done with the scan results
    m_spScanResults.Release();

    // We are done with the scanner
    m_spScanner.Release();
    m_spThreatInfo.Release();

    // We are done with the scanUI
    m_spScanUI.Release();
}

avScanUI::IScanUICallbacksInt* CThreatTracker::GetScanUI()
{
    return m_spScanUI;
}

void CThreatTracker::SetScanResults(AVModule::IAVMapStrData* pResults)
{
    m_spScanResults = pResults;
}

size_t CThreatTracker::GetThreatCount()
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    return m_arrThreats.GetCount();
}

void CThreatTracker::GetThreatCountEx(int& nViral, int& nNonViral, CThreat::eTTState eState)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Reset the counts
    nViral = 0;
    nNonViral = 0;

    // Loop to get the counts
    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        CThreat& cThreat = m_arrThreats.GetAt(iIndex);

        // Check if this threat is included...
        if(cThreat.IsThreatIncluded(eState))
        {
            if(cThreat.IsViral())
                ++nViral;
            else
                ++nNonViral;
        }
    }
}

bool CThreatTracker::AddThreat(IAVMapDwordData* pThreatMap, bool bReplaceOnly, size_t* piNewIndex)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this items GUID
    GUID idThreat;
    HRESULT hr = pThreatMap->GetValue(ThreatTracking::TrackingData_THREATID, idThreat);
    if(FAILED(hr))
    {
        CCTRACEE(CCTRCTX L"Failed to retrieve threat ID! hr=%08X", hr);
        return false;
    }

    // See if we already have this guid
    size_t iIndex;
    if(true == FindThreatIndexByID(idThreat, iIndex))
    {
        // Update this indexes threatmap
        m_arrThreats[iIndex].SetThreatMap(pThreatMap);
        if(piNewIndex)
            *piNewIndex = iIndex;

        return true;
    }

    // If we are only to replace existing items and we have made it here... just return
    if(bReplaceOnly)
    {
        CCTRACEE(CCTRCTX L"Called with bReplaceOnly=true and threat doesn't already exist in the threat map... returning");
        return false;
    }

    // Add this threat to the tracking array
    iIndex = m_arrThreats.Add(pThreatMap);
    if(piNewIndex)
        *piNewIndex = iIndex;

    return true;
}

bool CThreatTracker::IsThreatIncluded(size_t iThreatIndex, CThreat::eTTState eState)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    return m_arrThreats[iThreatIndex].IsThreatIncluded(eState);
}

HRESULT CThreatTracker::DisplayThreatDetails(HWND hParentWnd, size_t iThreatIndex, bool bStartOnFirstPage)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Display the property sheet
    HRESULT hr = E_FAIL;
    symhtml::ISymHTMLDialogPtr spDialog;
    if(SYM_SUCCEEDED(symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog)))
    {
        CThreat& cThreat = m_arrThreats.GetAt(iThreatIndex);

        // Get the requested threat map
        AVModule::IAVMapDwordDataPtr spThreatMap = cThreat.GetThreatMap();
        if(!spThreatMap)
        {
            CCTRACEE(CCTRCTX L"Failed to get threat map!");
            return E_FAIL;
        }

        // Get the threats ID
        GUID idThreat = cThreat.GetID();

        //
        // Get the type
        ThreatTracking::eThreatTypes eType;
        hr = spThreatMap->GetValue(ThreatTracking::TrackingData_THREAT_TYPE, (DWORD&)eType);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat type!", hr);

        // Get the threats display data (if it is an ThreatType_Anomaly)
        IAVArrayDataPtr spArrDispInfo;
        if(eType == ThreatTracking::ThreatType_Anomaly)
        {
            hr = GetAnomalyDisplayData(spThreatMap, idThreat, false, spArrDispInfo);
            LOG_FAILURE_AND_RETURN(L"GetAnomalyDisplayData failed", hr);
        }

        // Create our prompt dialog
        LPCSTR pszStartTab = TAB_RISK;
        if(!bStartOnFirstPage)
            pszStartTab = TAB_DETAILS;

        CSymPtr<CRiskDetailsDlg> spDlgDoc = new CRiskDetailsDlg(this, iThreatIndex, spThreatMap, spArrDispInfo, eType, pszStartTab);
        if(!spDlgDoc)
        {
            CCTRACEE(CCTRCTX L"new of CExportProgressDlg failed!");
            return E_OUTOFMEMORY;
        }

        // Display the details property sheet UI
        int nResult = 0;
        hr = spDialog->DoModal(hParentWnd, spDlgDoc, &nResult);
        LOG_FAILURE_AND_RETURN(L"DoModal failed", hr);
    }

    return hr;
}

HRESULT CThreatTracker::LaunchResponsePage(size_t iThreatIndex)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this threat
    CThreat& cThreat = m_arrThreats.GetAt(iThreatIndex);

    // Get the URL for this item
    CStringW sURL;
    sURL = GenerateOutputForItem(cThreat, L"%{INFOURL}%", iThreatIndex);

    // Launch the browser
    bool bRes = GotoURL(sURL, SW_SHOWNORMAL);
    HRESULT hr = E_FAIL;
    if(bRes)
        hr = S_OK;

    return hr;
}

void CThreatTracker::SetThreatAction(size_t iThreatIndex, CThreat::eIntThreatAction eAction)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Set the action...
    m_arrThreats.GetAt(iThreatIndex).SetAction(eAction);
}

CThreat::eIntThreatAction CThreatTracker::GetThreatAction(size_t iThreatIndex)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    return m_arrThreats.GetAt(iThreatIndex).GetAction();
}

HRESULT CThreatTracker::TakeThreatAction(size_t iThreatIndex, HWND hParentWnd, bool bSkipReview)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this threat
    CThreat& cThreat = m_arrThreats.GetAt(iThreatIndex);

    // Get this threats action
    CThreat::eIntThreatAction eAction = cThreat.GetAction();

    // If the action is none, return immediately
    if(eAction == CThreat::eIntAct_None)
    {
        // No action selected...
        return S_FALSE;
    }

    // Only take action on threats that require attention...
    bool bThreatNeedsAttention = cThreat.IsThreatIncluded(CThreat::eTTState_RequireAttention);
    if(!bThreatNeedsAttention)
        return S_FALSE;

    // Get the threats ID
    GUID idThreat = cThreat.GetID();

    // Action data map, in case we need to allow a procterm or set some other option
    IAVMapDwordDataPtr spActionData;

    // Call Process(), based on the action
    HRESULT hr = E_FAIL;
    switch(eAction)
    {
        case CThreat::eIntAct_Review:
        {
            if(bSkipReview)
            {
                hr = S_OK;
                break;
            }

            // Get the URL for this item
            CStringW sURL;
            sURL = GenerateOutputForItem(cThreat, L"%{INFOURL}%", iThreatIndex);

            // Launch the browser
            bool bRes = GotoURL(sURL, SW_SHOWNORMAL);
            if(bRes)
            {
                hr = S_OK;

                // Flip the internal threat state
                cThreat.SetIntState(CThreat::eIntState_Reviewed);

                // Update state and clear the action
                cThreat.SetAction(CThreat::eIntAct_None);

                // Refresh the UI
                m_spScanUI->RefreshThreatLists();
                m_spScanUI->UpdateScanStats();
            }
            else
                hr = E_FAIL;

            break;
        }
        case CThreat::eIntAct_Fix:
        {
            bool bProcTermNeeded = false;
            hr = cThreat.GetThreatMap()->GetValue(ThreatTracking::TrackingData_PROCTERMREQUIRED, bProcTermNeeded);
            if(SUCCEEDED(hr) && bProcTermNeeded)
            {
                // Check the prior result...
                if(m_eProcTermRes != avScanUI::eProcTerm_YesToAll || m_eProcTermRes != avScanUI::eProcTerm_NoToAll)
                {
                    // Get an instance of the misc ui class
                    CSymPtr<avScanUI::CScanUIMisc> spMiscUI = new avScanUI::CScanUIMisc();
                    if(spMiscUI)
                    {
                        // Init the misc ui class
                        hr = spMiscUI->Initialize();
                        LOG_FAILURE(L"Failed to init CScanUIMisc", hr);

                        // Load SymTheme on this thread (for dialogs)
	                    CISSymTheme isSymTheme;
	                    HRESULT hr = isSymTheme.Initialize(GetModuleHandle(NULL));
	                    if(FAILED(hr))
	                    {
		                    CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr);
	                    }

                        // Prompt
                        m_eProcTermRes = spMiscUI->QueryUserForProcTerm(hParentWnd);
                    }
                }

                // Create an instance of the action data map
                SYMRESULT symRes = m_loaderMapDwordData.CreateObject(&spActionData);
                SYM_LOG_FAILURE(L"Failed to create an IAVMapDwordData object", symRes);

                // Is a procterm ok with the user?
                if(spActionData
                   && (m_eProcTermRes == avScanUI::eProcTerm_Yes || m_eProcTermRes == avScanUI::eProcTerm_YesToAll))
                {
                    hr = spActionData->SetValue(ThreatProcessor::FixIn_AllowProcTerm, true);
                    LOG_FAILURE(L"Failed to set proc term option", hr);
                }
            }

            // *****************************************
            // ***Fix FALLS through to exclude, because only the proc term portion (above) is different!
            // *****************************************
        }
        case CThreat::eIntAct_Exclude:
        {
            ThreatProcessor::eProcessingAction eRealAction;
            if(eAction == CThreat::eIntAct_Fix)
                eRealAction = ThreatProcessor::ProcessingAction_Fix;
            else
                eRealAction = ThreatProcessor::ProcessingAction_Exclude;

            // Send this action request to the scanner
            hr = m_spScanner->Process(eRealAction, idThreat, spActionData);
            if(SUCCEEDED(hr))
            {
                // This threat is now pending
                cThreat.SetIntState(CThreat::eIntState_Pending);
            }
            else
            {
#pragma  TODO("We need to report the failure to the user")
                THREAT_FAILURE(idThreat, L"IAVScanBase::Process failed", hr);
            }

            // Refresh the UI
            m_spScanUI->RefreshThreatLists();

            break;
        }
        case CThreat::eIntAct_Ignore:
        {
            // Just flip the internal threat state
            cThreat.SetIntState(CThreat::eIntState_Ignored);

            // Update state and clear the action
            cThreat.SetAction(CThreat::eIntAct_None);

            // Refresh the UI
            m_spScanUI->RefreshThreatLists();
            m_spScanUI->UpdateScanStats();

            // Succeeded
            hr = S_OK;

            break;
        }
    }

    return hr;
}

HRESULT CThreatTracker::FillActionsMap(IAVMapGuidData* pActionMap)
{
    HRESULT hr = S_OK;

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        // Get this threat
        CThreat& cThreat = m_arrThreats.GetAt(iIndex);

        // Get the threats ID
        GUID idThreat = cThreat.GetID();

        // Only add threats that have an action
        CThreat::eIntThreatAction eAction = cThreat.GetAction();
        switch(eAction)
        {
            case CThreat::eIntAct_Fix:
            {
                hr = pActionMap->SetValue(idThreat, (DWORD)EmailScanner::eEmailAction_Resolve);
                break;
            }
            case CThreat::eIntAct_Exclude:
            {
                hr = pActionMap->SetValue(idThreat, (DWORD)EmailScanner::eEmailAction_Exclude);
                break;
            }
            case CThreat::eIntAct_Ignore:
            {
                hr = pActionMap->SetValue(idThreat, (DWORD)EmailScanner::eEmailAction_Ignore);
                break;
            }
        }

        LOG_FAILURE(L"Failed to add action to map", hr);
    }

    return hr;
}

// Returns false if no actions are applied...
HRESULT CThreatTracker::ApplyAllThreatActions(HWND hParentWnd)
{
    HRESULT hrRet = E_FAIL;

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Loop through all items
    size_t iApplied = 0;
    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        // Last param, true, is to keep us from "Reviewing" a bunch of threats
        HRESULT hr = TakeThreatAction(iIndex, hParentWnd, true);
        if(FAILED(hr))
        {
            hrRet = hr;
            break;
        }
        else if(hr == S_OK) // not SUCCEEDED!!!
            ++iApplied;
    }

    // If no actions were applied, return S_FALSE
    if(iApplied == 0)
        hrRet = S_FALSE;

    return hrRet;
}

bool CThreatTracker::GetThreatDirty(size_t iThreatIndex)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this item
    CThreat& cThreat = m_arrThreats.GetAt(iThreatIndex);

    // return the dirty flag
    return cThreat.GetDirty();
}

void CThreatTracker::SetThreatDirty(size_t iThreatIndex, bool bDirty)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this item
    CThreat& cThreat = m_arrThreats.GetAt(iThreatIndex);

    // Set the dirty flag
    cThreat.SetDirty(bDirty);
}

HRESULT CThreatTracker::GetAnomalyCategoryStats(CStringW& sCategoryStats, CThreat::eTTState eState, MAP_CAT_COUNT* pRawMap)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Clear the return buffer
    sCategoryStats.Empty();

    // Get counts of each type
    MAP_CAT_COUNT mapCounts;

    // Loop through all items and build up the counts
    HRESULT hr;
    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        CThreat& cThreat = m_arrThreats.GetAt(iIndex);
        IAVMapDwordData* pThreatMap = cThreat.GetThreatMap();

        // Check if this threat is included...
        if(cThreat.IsThreatIncluded(eState))
        {
            // Get the categories
            IAVArrayDataPtr spArrCats;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_THREATCATEGORIES, AVDataTypes::eTypeArray, reinterpret_cast<ISymBase*&>(spArrCats));
            LOG_FAILURE_AND_RETURN(L"Failed to get threat categories!", hr);

            // Get the category count and loop through them
            DWORD dwCats;
            hr = spArrCats->GetCount(dwCats);
            LOG_FAILURE_AND_RETURN(L"Failed to get threat category count!", hr);
            for(DWORD dwIdx = 0; dwIdx < dwCats; ++dwIdx)
            {
                ccEraser::IAnomaly::Anomaly_Category eCategory;
                if(SUCCEEDED(spArrCats->GetValue(dwIdx, (DWORD&)eCategory)))
                {
                    // group the category
                    eCategory = GroupCategory(eCategory);

                    // Bump the count for this category
                    MAP_CAT_COUNT::CPair* pPair = mapCounts.Lookup(eCategory);
                    if(pPair)
                        pPair->m_value++;
                    else
                        mapCounts[eCategory] = 1;
                }
            }
        }
    }

    // Copy the raw map to the callers buffer, if wanted
    if(pRawMap)
    {
        for(DWORD dwIdx = 0; dwIdx <= ccEraser::IAnomaly::Last_Category; ++dwIdx)
        {
            MAP_CAT_COUNT::CPair* pPair = mapCounts.Lookup((ccEraser::IAnomaly::Anomaly_Category)dwIdx);
            if(pPair)
            {
                pRawMap->SetAt(pPair->m_key, pPair->m_value);
            }
        }
    }

    // Build the substats html...
    CString sFmt, sTmp, sCat;
    if(eState == CThreat::eTTState_Done)
        sFmt.LoadString(IDS_SUBSTATS_FIXED);
    else
        sFmt.LoadString(IDS_SUBSTATS_FOUND);

    // Walk the list of types...
    CThreatCatInfo ctiCatInfo;
    for(DWORD dwIdx = 0; dwIdx <= ccEraser::IAnomaly::Last_Category; ++dwIdx)
    {
        MAP_CAT_COUNT::CPair* pPair = mapCounts.Lookup((ccEraser::IAnomaly::Anomaly_Category)dwIdx);
        if(pPair)
        {
            ctiCatInfo.GetCategoryText(pPair->m_key, sCat.GetBuffer(256), 256);
            sCat.ReleaseBuffer();
            sTmp.Format(sFmt, sCat.GetString(), sCat.GetString(), sCat.GetString(), sCat.GetString(), pPair->m_value);
            sCategoryStats += sTmp;
        }
    }

    // If the string is empty
    if(sCategoryStats.IsEmpty())
    {
        if(eState == CThreat::eTTState_Done)
            sTmp.LoadString(IDS_SUBSTATS_NONE_FIXED);
        else
            sTmp.LoadString(IDS_SUBSTATS_NONE);

        sCategoryStats.Format(sFmt, L"none", L"none", sTmp, L"none", 0);
    }

    return S_OK;
}

HRESULT CThreatTracker::GetTypeCountsForThreat(size_t iThreatIndex, MAP_TYPE_COUNT& mapCounts)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this threat and its ID
    CThreat& cThreat = m_arrThreats.GetAt(iThreatIndex);

    // Call the real method
    return GetTypeCountsForThreat(cThreat, mapCounts);
}

HRESULT CThreatTracker::GetTypeCountsForThreat(CThreat& cThreat, MAP_TYPE_COUNT& mapCounts)
{
    HRESULT hr = E_FAIL;

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this threats ID
    GUID idThreat = cThreat.GetID();

    // Get the requested threat map
    AVModule::IAVMapDwordDataPtr spThreatMap = cThreat.GetThreatMap();
    if(!spThreatMap)
    {
        CCTRACEE(CCTRCTX L"Failed to get threat map!");
        return E_FAIL;
    }

    // Get the type
    ThreatTracking::eThreatTypes eType;
    hr = spThreatMap->GetValue(ThreatTracking::TrackingData_THREAT_TYPE, (DWORD&)eType);
    LOG_FAILURE_AND_RETURN(L"Failed to get threat type!", hr);

    return GetTypeCountsForThreat(spThreatMap, eType, mapCounts);
}

HRESULT CThreatTracker::GetTypeCountsForThreat(IAVMapDwordData* pThreatMap, ThreatTracking::eThreatTypes eType,
                                               MAP_TYPE_COUNT& mapCounts, bool bFilterForUser)
{
    HRESULT hr = E_FAIL;

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Try to get the anomaly stats
    IAVMapDwordDataPtr spAnomalyStats;
    hr = pThreatMap->GetValue(ThreatTracking::TrackingData_Anomaly_Stats,
                              AVModule::AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spAnomalyStats));

    // If we have anomaly stats... use them
    if(SUCCEEDED(hr) && spAnomalyStats)
    {
        // Get a count of items
        DWORD dwCount = 0;
        hr = spAnomalyStats->GetCount(dwCount);
        LOG_FAILURE_AND_RETURN(L"Failed to get anomaly stats count", hr);

        for(DWORD dwIdx = 0; dwIdx < dwCount; ++dwIdx)
        {
            // Get the type of this item
            ccEraser::eObjectType eRemType;
            hr = spAnomalyStats->GetKeyAtIndex(dwIdx, (DWORD&)eRemType);
            LOG_FAILURE_AND_CONTINUE(L"Failed to get get at index!", hr);

            // Get the count for this type
            DWORD dwTypeCount = 0;
            hr = spAnomalyStats->GetValue((DWORD)eRemType, dwTypeCount);
            LOG_FAILURE_AND_CONTINUE(L"Failed to get count for type", hr);

            // Group some types
            eRemType = GroupType(eRemType);

            MAP_TYPE_COUNT::CPair* pPair = mapCounts.Lookup(eRemType);
            if(pPair)
                pPair->m_value += dwTypeCount;
            else
                mapCounts[eRemType] = dwTypeCount;
        }
    }
    else if(eType == AVModule::ThreatTracking::ThreatType_Compressed
            || eType == AVModule::ThreatTracking::ThreatType_Email)
    {
        // Compressed is always 1 file
        mapCounts[ccEraser::FileRemediationActionType] = 1;
    }
    else if(eType == AVModule::ThreatTracking::ThreatType_BR
            || eType == AVModule::ThreatTracking::ThreatType_MBR)
    {
        // Compressed is always 1 file
        mapCounts[ccEraser::SystemRemediationActionType] = 1;
    }

    return S_OK;
}

HRESULT CThreatTracker::GetTargetsForThreat(size_t iThreatIndex, ccEraser::eObjectType eDispType, CAtlArray<CStringW>& arrTargets)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this threat and its ID
    CThreat& cThreat = m_arrThreats.GetAt(iThreatIndex);

    // Call the real method
    return GetTargetsForThreat(cThreat, eDispType, arrTargets);
}

HRESULT CThreatTracker::GetTargetsForThreat(CThreat& cThreat, ccEraser::eObjectType eDispType, CAtlArray<CStringW>& arrTargets)
{
    HRESULT hr = E_FAIL;

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this threats ID
    GUID idThreat = cThreat.GetID();

    // Get the requested threat map
    AVModule::IAVMapDwordDataPtr spThreatMap = cThreat.GetThreatMap();
    if(!spThreatMap)
    {
        CCTRACEE(CCTRCTX L"Failed to get threat map!");
        return E_FAIL;
    }

    // Get the type
    ThreatTracking::eThreatTypes eType;
    hr = spThreatMap->GetValue(ThreatTracking::TrackingData_THREAT_TYPE, (DWORD&)eType);
    LOG_FAILURE_AND_RETURN(L"Failed to get threat type!", hr);

    // Get the threats display data (if it is an ThreatType_Anomaly)
    IAVArrayDataPtr spDisplayData;
    if(eType == ThreatTracking::ThreatType_Anomaly)
    {
        hr = GetAnomalyDisplayData(spThreatMap, idThreat, false, spDisplayData);        
        LOG_FAILURE_AND_RETURN(L"GetAnomalyDisplayData failed", hr);
    }

    return GetTargetsForThreat(spDisplayData, eType, spThreatMap, eDispType, arrTargets);
}

HRESULT CThreatTracker::GetTargetsForThreat(IAVArrayData* pDisplayData, ThreatTracking::eThreatTypes eType, IAVMapDwordData* pThreatMap,
                                            ccEraser::eObjectType eDispType, CAtlArray<CStringW>& arrTargets, bool bFilterForUser, CAtlArray<CStringW>* pActions)
{
    HRESULT hr = E_FAIL;

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    try
    {
        // If filtering... get our threads token and replacement string
        bool bUseInfectionStats = false;
        CStringW sNoAccess;
        CAccessToken accToken, impToken;
        if(bFilterForUser)
        {
            sNoAccess.LoadString(IDS_NOACCESS_TO_OBJECT);

            if(!accToken.GetEffectiveToken(TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_IMPERSONATE))
            {
                LOG_FAILURE_AND_RETURN(L"Failed to open thread token", HRESULT_FROM_WIN32(GetLastError()));
            }
            if(!accToken.CreateImpersonationToken(&impToken))
            {
                LOG_FAILURE_AND_RETURN(L"Failed to open thread token", HRESULT_FROM_WIN32(GetLastError()));
            }
        }

        // If we have display data ... go through it
        if(pDisplayData)
        {
            DWORD dwCount = 0;
            hr = pDisplayData->GetCount(dwCount);
            LOG_FAILURE_AND_RETURN(L"Failed to get display data count", hr);

            for(DWORD dwIdx = 0; dwIdx < dwCount; ++dwIdx)
            {
                // Get this item
                AVModule::IAVMapDwordDataPtr spMapInfo;
                hr = pDisplayData->GetValue(dwIdx, AVModule::AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMapInfo));
                LOG_FAILURE_AND_CONTINUE(L"Failed to get individual remediation details map!", hr);

                // Get the type of this threat
                ccEraser::eObjectType eRemType;
                hr = spMapInfo->GetValue(AVModule::ThreatTracking::RemDisp_RemediationType, (DWORD&)eRemType);
                LOG_FAILURE_AND_CONTINUE(L"Failed to get individual remediation type!", hr);

                // Group some types
                eRemType = GroupType(eRemType);

                // Is this the type we want?
                if(eRemType == eDispType)
                {
                    // Check this item for a security descriptor, if requested
                    if(bFilterForUser && !AccessCheck(spMapInfo, pThreatMap, impToken.GetHandle()))
                    {
                        arrTargets.Add(sNoAccess);
                        if(pActions)
                            pActions->Add(_S(IDS_NA));
                    }
                    else
                    {
                        // Get the target
                        cc::IStringPtr spTarget;
                        hr = spMapInfo->GetValue(AVModule::ThreatTracking::RemDisp_Target, spTarget);
                        LOG_FAILURE(L"Failed to get remediation target!", hr);

                        if(spTarget)
                        {
                            // Remove the \\?\ conext that comes from ccEraser/ccScan
                            CString sTarget = spTarget->GetStringW();
                            sTarget.Replace(L"\\\\?\\", L"");

                            // Add this target to the output array
                            arrTargets.Add(sTarget);
                            if(pActions && pThreatMap)
                            {
                                // Get the overall threat status
                                AVModule::ThreatTracking::eThreatState eThState;
                                hr = pThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, (DWORD&)eThState);
                                LOG_FAILURE_AND_RETURN(L"Failed to get threat state", hr);

                                // Translate and add...
                                pActions->Add(TranslateRemediationActionTaken(eRemType, spMapInfo, eThState));
                            }
                        }
                    }
                }
            }
        }
        else if(eType == AVModule::ThreatTracking::ThreatType_Compressed
                || eType == AVModule::ThreatTracking::ThreatType_Email)
        {
            // Check this item for a security descriptor, if requested
            if(bFilterForUser && !AccessCheck(NULL, pThreatMap, impToken.GetHandle()))
            {
                arrTargets.Add(sNoAccess);
                if(pActions)
                    pActions->Add(_S(IDS_NA));
            }
            else
            {
                // Get the compressed threat info
                ISymBasePtr spArrBase;
                hr = pThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_Compressed_COMPONENTS,
                                        AVModule::AVDataTypes::eTypeArray, spArrBase);
                LOG_FAILURE_AND_RETURN(L"Failed to get compressed components from threat map!", hr);

                AVModule::IAVArrayDataQIPtr spArrayItems(spArrBase);
                if(!spArrayItems)
                {
                    hr = E_NOINTERFACE;
                    LOG_FAILURE_AND_RETURN(L"Failed to QI for AVArrayData!", hr);
                }

                // Get a count
                DWORD dwCount = 0;
                hr = spArrayItems->GetCount(dwCount);
                LOG_FAILURE_AND_RETURN(L"Failed to get count of compressed components", hr);

                // Load our "inside of" string and order flag
                CString sInsideOf, sInsideOfRev;
                sInsideOf.LoadString(IDS_INSIDEOF);
                sInsideOfRev.LoadString(IDS_REVERSEINSIDEOF);

                // Check the order flag
                bool bReverse = false;
                if(sInsideOfRev[0] == L'1')
                    bReverse = true;

                // Loop through the items
                CStringW sInside;
                for(DWORD dwIdx = (bReverse ? dwCount : 0);
                    (bReverse ? dwIdx > 0 : dwIdx < dwCount);
                    (bReverse ? --dwIdx : ++dwIdx))
                {
                    // Get the target
                    cc::IStringPtr spComponent;
                    hr = spArrayItems->GetValue((bReverse ? dwIdx - 1 : dwIdx), spComponent);
                    LOG_FAILURE_AND_RETURN(L"Failed to get component text!", hr);


                    // Add our separator string
                    if(!sInside.IsEmpty())
                        sInside += sInsideOf;

                    // Remove the \\?\ conext that comes from ccEraser/ccScan
                    CString sFixedComp = spComponent->GetStringW();
                    sFixedComp.Replace(L"\\\\?\\", L"");

                    sInside += L"[";
                    sInside += sFixedComp;
                    sInside += L"]";
                }

                // Add this target to the output array
                arrTargets.Add(sInside);
                bUseInfectionStats = true;
            }
        }
        else if(eType == AVModule::ThreatTracking::ThreatType_BR
                || eType == AVModule::ThreatTracking::ThreatType_MBR)
        {
            // Get the target
            cc::IStringPtr spDrive;
            hr = pThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_BootRecord_DriveID, spDrive);
            LOG_FAILURE_AND_RETURN(L"Failed to get drive ID!", hr);
#pragma  TODO("What should we really do with this?")

            CString sTmp = L"Drive ";
            sTmp += spDrive->GetStringW();

            // Add this target to the output array
            arrTargets.Add(sTmp);

            bUseInfectionStats = true;
        }

        // Get the action taken
        if(bUseInfectionStats && pActions)
        {
            DWORD dwStatus = 0;
            hr = pThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_INFECTIONSTATUS, dwStatus);
            LOG_FAILURE_AND_RETURN(L"Failed to get compressed infection status!", hr);

            CString sAction;
            switch(dwStatus)
            {
                CASE_LOAD_STRING(ccScanw::IScanwInfection::INFECTED, sAction, IDS_INFECTED)
                CASE_LOAD_STRING(ccScanw::IScanwInfection::REPAIRED, sAction, IDS_REPAIRED)
                CASE_LOAD_STRING(ccScanw::IScanwInfection::DELETED, sAction, IDS_DELETED)
                default:
                {
                    CCTRCTXW1(L"Unexpected infection status: %d", dwStatus);
                    sAction.LoadString(IDS_NA);
                    break;
                }
            }

            pActions->Add(sAction);
        }
    }
    catch(CAtlException& e)
    {
        CCTRCTXE1(L"ATL Exception: 0x%08X", e.m_hr);
    }

    return S_OK;
}

CStringW CThreatTracker::TranslateRemediationActionTaken(ccEraser::eObjectType eRemType, AVModule::IAVMapDwordData* pMapRemInfo, AVModule::ThreatTracking::eThreatState eThState)
{
    // If the overall state for this risk is excluded or backup only, all items are that
    if(eThState == ThreatTracking::ThreatState_Excluded)
    {
        return _S(IDS_STATE_EXCLUDED);
    }
    else if(eThState == ThreatTracking::ThreatState_BackupOnly)
    {
        return _S(IDS_STATE_BACKUPONLY);
    }

    // Get the remediation state and result to build the status string
    ccEraser::IRemediationAction::Action_State eState;
    HRESULT hr = pMapRemInfo->GetValue(AVModule::ThreatTracking::RemDisp_State, (DWORD&)eState);
    LOG_FAILURE(L"Unable to get remediation state", hr);

    ccEraser::IRemediationAction::Operation_Type eOpType;
    hr = pMapRemInfo->GetValue(AVModule::ThreatTracking::RemDisp_OperationType, (DWORD&)eOpType);
    LOG_FAILURE(L"Unable to get operation type", hr);

    // Get the remediation result
    ccEraser::eResult eRemResult = ccEraser::False;
    bool bHaveResult = pMapRemInfo->Contains(AVModule::ThreatTracking::RemDisp_RemediationResult);
    if(bHaveResult)
    {
        hr = pMapRemInfo->GetValue(AVModule::ThreatTracking::RemDisp_RemediationResult, (DWORD&)eRemResult);
        LOG_FAILURE(L"Failed to get remediation result when contained return true!", hr);
        if(FAILED(hr))
            bHaveResult = false;
    }
    else
    {
        // Item is lacking a result...
        return _S(IDS_NOACTION_TAKEN);
    }

    // If this item requires a reboot, return that
    if(eRemResult == ccEraser::RebootRequired)
    {
        return _S(IDS_STATE_REBOOTREQ);
    }

    // Certain results are displayed generically for all types
    CString sStatus;
    switch(eRemResult)
    {
        CASE_LOAD_STRING(ccEraser::AccessDenied, sStatus, IDS_ACCESS_DENIED)
        CASE_LOAD_STRING(ccEraser::FileNotFound, sStatus, IDS_NOT_DETECTED)
        CASE_LOAD_STRING(ccEraser::NothingToDo, sStatus, IDS_NOACTION_REQUIRED)
        CASE_LOAD_STRING(ccEraser::Deleted, sStatus, IDS_DELETED)
        CASE_LOAD_STRING(ccEraser::Repaired, sStatus, IDS_REPAIRED)
        default:
        {
            if(ccEraser::Succeeded(eRemResult))
            {
                // Report based on operation type
                switch(eOpType)
                {
                    case ccEraser::IRemediationAction::Delete:
                    case ccEraser::IRemediationAction::Remove:
                        sStatus.LoadString(IDS_DELETED);
                        break;
                    case ccEraser::IRemediationAction::Suspend:
                    case ccEraser::IRemediationAction::Stop:
                    case ccEraser::IRemediationAction::Terminate:
                        sStatus.LoadString(IDS_TERMINATED);
                        break;
                    case ccEraser::IRemediationAction::Move:
                    case ccEraser::IRemediationAction::CreateEmptyFile:
                    case ccEraser::IRemediationAction::DeleteLine:
                    case ccEraser::IRemediationAction::Set:
                    case ccEraser::IRemediationAction::SetIPAddress:
                    case ccEraser::IRemediationAction::SetDomainName:
                    default:
                        sStatus.LoadString(IDS_REPAIRED);
                        break;
                }
            }
            else
            {
                // Report based on attempted operation type
                switch(eOpType)
                {
                    case ccEraser::IRemediationAction::Delete:
                    case ccEraser::IRemediationAction::Remove:
                        sStatus.LoadString(IDS_DELETE_FAILED);
                        break;
                    case ccEraser::IRemediationAction::Suspend:
                    case ccEraser::IRemediationAction::Stop:
                    case ccEraser::IRemediationAction::Terminate:
                        sStatus.LoadString(IDS_TERM_FAILED);
                        break;
                    case ccEraser::IRemediationAction::Move:
                    case ccEraser::IRemediationAction::CreateEmptyFile:
                    case ccEraser::IRemediationAction::DeleteLine:
                    case ccEraser::IRemediationAction::Set:
                    case ccEraser::IRemediationAction::SetIPAddress:
                    case ccEraser::IRemediationAction::SetDomainName:
                    default:
                        // For all other failure types just using "Failed" text
                        sStatus.LoadString(IDS_OPERATION_FAILED);
                        break;
                }
            }
        }
    }

    return sStatus;
}

bool CThreatTracker::AccessCheck(AVModule::IAVMapDwordData* pMapInfo, AVModule::IAVMapDwordData* pThreatMap, HANDLE hClientTotken)
{
    // Get the descriptor string ...
    // Items without security descriptors are shown to all users 
    cc::IStringPtr spSecDescString;
    HRESULT hr;

    // Do we have anomaly display data?
    if(pMapInfo)
    {
        hr = pMapInfo->GetValue(ThreatTracking::RemDisp_SecurityDescriptorString, spSecDescString);
        if(FAILED(hr))
            return true;
    }
    else if(pThreatMap)
    {
        // Try for a compressed file security descriptor
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_Compressed_SecrurityDescriptor, spSecDescString);
        if(FAILED(hr))
            return true;
    }
    else
    {
        // Odd... just return true
        CCTRCTXW0(L"Called with both mapinfo and threatmap NULL");
        return true;
    }

    // Convert the security descriptor to a string
    SECURITY_DESCRIPTOR* pSD;
    if(ConvertStringSecurityDescriptorToSecurityDescriptor(spSecDescString->GetStringW(), SDDL_REVISION_1, (PSECURITY_DESCRIPTOR*)&pSD, NULL))
    {
        // We only care about read access
        GENERIC_MAPPING gmAccess = {GENERIC_READ, 0, 0, 0};
        DWORD dwDesired = STANDARD_RIGHTS_READ;

        // Do the access check
        BYTE byTmp[4096];
        PPRIVILEGE_SET pprivSet = (PPRIVILEGE_SET)byTmp;
        DWORD dwPSLen = 4096;
        DWORD dwAccess = 0;
        BOOL bAccess = FALSE;
        BOOL bRet = ::AccessCheck(pSD, hClientTotken, dwDesired, &gmAccess, pprivSet, &dwPSLen, &dwAccess, &bAccess);
        DWORD dwErr = GetLastError();

        // Free the security descriptor
        LocalFree(pSD);

        // If AccessCheck() succeeded .AND. the user has access, return true
        if(bRet && bAccess && dwAccess == dwDesired)
        {
            // The user has access
            return true;
        }
        else if(!bRet)
        {
            CCTRCTXE1(L"AccessCheck failed 0x%08X", HRESULT_FROM_WIN32(dwErr));
        }
    }

    // If we made it here, the user doesn't have access
    return false;
}

HRESULT CThreatTracker::GetAnomalyDisplayData(IAVMapDwordData* pThreatMap, const GUID& idThreat, bool bCheckPresence, IAVArrayData*& pDisplayData)
{
    HRESULT hr = E_NOINTERFACE;
    if(m_spScanner)
        hr = m_spScanner->GetAnomalyDisplayData(idThreat, bCheckPresence, pDisplayData);
    else if(m_spThreatInfo)
    {
        // See if the anomaly display data was stuffed in to the threat map (this call is cheap)
        if(pThreatMap)
            hr = pThreatMap->GetValue(0xFFFF, AVDataTypes::eTypeArray, reinterpret_cast<ISymBase*&>(pDisplayData));

        // If it wasn't in the threat map, try to find it by threat guid 
        if(FAILED(hr))
            hr = m_spThreatInfo->GetAnomalyDisplayData(idThreat, bCheckPresence, pDisplayData);
    }

    return hr;
}

ccEraser::eObjectType CThreatTracker::GroupType(ccEraser::eObjectType eRemType)
{
    switch(eRemType)
    {
        case ccEraser::InfectionRemediationActionType:
        case ccEraser::DirectoryRemediationActionType:
            return ccEraser::FileRemediationActionType;
    }

    // If it wasn't in a group (above), return it as is
    return eRemType;
}

ccEraser::IAnomaly::Anomaly_Category CThreatTracker::GroupCategory(ccEraser::IAnomaly::Anomaly_Category eCat)
{
    switch(eCat)
    {
        // Malicious, Reserved Malicious, and Heuristic all display as "Virus" ... so we group them
        case ccEraser::IAnomaly::Malicious:
        case ccEraser::IAnomaly::ReservedMalicious:
        case ccEraser::IAnomaly::Heuristic:
            return ccEraser::IAnomaly::Viral;
    }

    // If it wasn't in a group (above), return it as is
    return eCat;
}

bool CThreatTracker::RebootRequired()
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Loop through all items
    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        CThreat& cThreat = m_arrThreats.GetAt(iIndex);
        IAVMapDwordData* pItem = cThreat.GetThreatMap();

        // Get this threats state
        bool bRebootRequired = false;
        HRESULT hr = pItem->GetValue(ThreatTracking::TrackingData_REBOOTREQUIRED, bRebootRequired);
        if(SUCCEEDED(hr) && bRebootRequired)
        {
            return true;
        }
    }

    return false;
}

bool CThreatTracker::GetIndexArrayForState(CThreat::eTTState eState, ARRAY_INDEXES& arrIndexes)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Empty the incoming array
    arrIndexes.RemoveAll();

    // Loop through all items
    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        CThreat& cThreat = m_arrThreats.GetAt(iIndex);

        // Check if this threat is included
        if(cThreat.IsThreatIncluded(eState))
        {
            // Add it to the return array
            arrIndexes.Add((ARRAY_INDEXES::INARGTYPE)iIndex);
        }
    }

    return true;
}

CStringW CThreatTracker::GenerateOutput(LPCWSTR pszTemplate, CThreat::eTTState eState)
{
    CStringW sRet;

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Loop through all items
    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        CThreat& cThreat = m_arrThreats.GetAt(iIndex);

        // Check if this threat is included
        if(cThreat.IsThreatIncluded(eState))
        {
            sRet += GenerateOutputForItem(cThreat, pszTemplate, iIndex);
        }
    }

    return sRet;
}

CStringW CThreatTracker::GenerateOutputForItem(LPCWSTR pszTemplate, size_t iIndex)
{
    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Get this item
    CThreat& cThreat = m_arrThreats.GetAt(iIndex);

    // Generate the output
    CStringW sRet = GenerateOutputForItem(cThreat, pszTemplate, iIndex);

    return sRet;
}

CStringW CThreatTracker::GenerateOutputForItem(CThreat& cThreat, LPCWSTR pszTemplate, size_t iIndex)
{
    IAVMapDwordData* pThreatMap = cThreat.GetThreatMap();
    if(!pThreatMap)
    {
        CCTRCTXE0(L"Failed to get threat map!");
        return L"";
    }

    // Get the threat type
    ThreatTracking::eThreatTypes eType;
    HRESULT hr = pThreatMap->GetValue(ThreatTracking::TrackingData_THREAT_TYPE, (DWORD&)eType);
    LOG_FAILURE(L"Failed to get threat type!", hr);

    return GenerateOutputForItem(pszTemplate, iIndex, pThreatMap, eType, NULL);
}

CStringW CThreatTracker::GenerateOutputForItem(LPCWSTR pszTemplate, size_t iIndex, IAVMapDwordData* pThreatMap,  
                                               ThreatTracking::eThreatTypes eType, IAVArrayData* pDisplayData)
{
    CStringW sRet = pszTemplate;

    // Quickly replace all index refs
    CStringW sIdx;
    sIdx.Format(L"%d", iIndex);
    sRet.Replace(L"%{IDX}%", sIdx);

    // Replace all of our tokens
    HRESULT hr;
    int nStartPos = sRet.Find(L"%{");
    while(nStartPos != -1)
    {
        int nEndPos = sRet.Find(L"}%", nStartPos);
        if(nEndPos == -1)
        {
            CCTRACEE(CCTRCTX L"Mismatched token identifiers!");
            ATLASSERT(FALSE);
            break;
        }

        // Get the token, then remove it from the string
        CStringW sToken = sRet.Mid(nStartPos + 2, nEndPos - (nStartPos + 2));
        sRet.Delete(nStartPos, (nEndPos - nStartPos) + 2);

        // Get the replacement value
        CStringW sValue;
        hr = GetReplacementValue(sToken, pThreatMap, eType, pDisplayData, iIndex, sValue);
        if(SUCCEEDED(hr))
        {
            // Fill in the replacement value
            sRet.Insert(nStartPos, sValue);
        }
        else
        {
            CCTRACEE(CCTRCTX L"Failed to get a replacement value for %s (hr=%08X)", sToken.GetString(), hr);
            sRet.Insert(nStartPos, L"");
        }

        // Find the next start token
        nStartPos = sRet.Find(L"%{");
    }

    return sRet;
}

HRESULT CThreatTracker::GetReplacementValue(LPCWSTR pszKey, IAVMapDwordData* pThreatMap, ThreatTracking::eThreatTypes eType, 
                                            IAVArrayData* pDisplayData, size_t iIndex, CStringW& sRet)
{
    if(!pThreatMap)
    {
        CCTRACEE(CCTRCTX L"Input threat is not valid!");
        return E_INVALIDARG;
    }

    HRESULT hr;
    if(wcsicmp(pszKey, L"TID") == 0)
    {
        GUID id;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_THREATID, id);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat ID!", hr);
        sRet.Format(GUID_FORMAT_STR(id));
    }
    else if(wcsicmp(pszKey, L"TYPE") == 0)
    {
        switch(eType)
        {
            CASE_LOAD_STRING(ThreatTracking::ThreatType_Anomaly, sRet, IDS_TYPE_ANOMALY);
            CASE_LOAD_STRING(ThreatTracking::ThreatType_Compressed, sRet, IDS_TYPE_COMPRESSED);
            CASE_LOAD_STRING(ThreatTracking::ThreatType_Email, sRet, IDS_TYPE_EMAIL);
            CASE_LOAD_STRING(ThreatTracking::ThreatType_BR, sRet, IDS_TYPE_BR);
            CASE_LOAD_STRING(ThreatTracking::ThreatType_MBR, sRet, IDS_TYPE_MBR);
            default:
                CCTRACEE(CCTRCTX L"Invalid threat type! (%d)", eType);
                ATLASSERT(FALSE);
                return E_FAIL;
        }
    }
    else if(wcsicmp(pszKey, L"STATE") == 0)
    {
        CThreat::eIntThreatState eIntState = m_arrThreats[iIndex].GetIntState();
        if(eIntState == CThreat::eIntState_Pending)
        {
            sRet.LoadString(IDS_STATE_PENDING);
        }
        else if(eIntState == CThreat::eIntState_Ignored)
        {
            sRet.LoadString(IDS_STATE_IGNOREONCE);
        }
        else if(eIntState == CThreat::eIntState_Reviewed)
        {
            sRet.LoadString(IDS_STATE_REVIEWED);
        }
        else
        {
            ThreatTracking::eThreatState eCurState;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_CURRENT_STATE, (DWORD&)eCurState);
            LOG_FAILURE_AND_RETURN(L"Failed to get threat state!", hr);
            switch(eCurState)
            {
                CASE_LOAD_STRING(ThreatTracking::ThreatState_FullyRemoved, sRet, IDS_STATE_FULLREMOVE);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_PartiallyRemoved, sRet, IDS_STATE_PARTREMOVE);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_RemoveNotAttempted, sRet, IDS_STATE_REMOVENOTATTEMPTED);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_CompressedInProcessing, sRet, IDS_STATE_PROCESSING);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_Excluded, sRet, IDS_STATE_EXCLUDED);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_BackupOnly, sRet, IDS_STATE_BACKUPONLY);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_RemoveFailed, sRet, IDS_STATE_REMOVEFAILED);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_DoNotDelete, sRet, IDS_STATE_DND);
                default:
                    CCTRACEE(CCTRCTX L"Invalid threat state!");
                    ATLASSERT(FALSE);
                    return E_FAIL;
            }

            // Check for "Reboot Required" ... if true, display that instead of state
            bool bRebootRequired = false;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_REBOOTREQUIRED, bRebootRequired);
            if(SUCCEEDED(hr) && bRebootRequired)
            {
                sRet.LoadString(IDS_STATE_REBOOTREQ);
            }
        }
    }
    else if(wcsicmp(pszKey, L"STATEDESC") == 0)
    {
        CThreat::eIntThreatState eIntState = m_arrThreats[iIndex].GetIntState();
        if(eIntState == CThreat::eIntState_Pending)
        {
            sRet.LoadString(IDS_SDESC_PENDING);
        }
        else if(eIntState == CThreat::eIntState_Ignored)
        {
            sRet.LoadString(IDS_SDESC_IGNOREONCE);
        }
        else if(eIntState == CThreat::eIntState_Reviewed)
        {
            sRet.LoadString(IDS_STATE_REVIEWED);
        }
        else
        {
            ThreatTracking::eThreatState eCurState;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_CURRENT_STATE, (DWORD&)eCurState);
            LOG_FAILURE_AND_RETURN(L"Failed to get threat state!", hr);
            switch(eCurState)
            {
                CASE_LOAD_STRING(ThreatTracking::ThreatState_FullyRemoved, sRet, IDS_SDESC_FULLREMOVE);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_PartiallyRemoved, sRet, IDS_SDESC_PARTREMOVE);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_RemoveNotAttempted, sRet, IDS_SDESC_REMOVENOTATTEMPTED);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_CompressedInProcessing, sRet, IDS_SDESC_PROCESSING);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_Excluded, sRet, IDS_SDESC_EXCLUDED);
                CASE_LOAD_STRING(ThreatTracking::ThreatState_BackupOnly, sRet, IDS_SDESC_BACKUPONLY);
                case ThreatTracking::ThreatState_RemoveFailed:
                {
                    // Check if the remove failed because this threat is in a non-recomposable container
                    DWORD dwCanModify = 1;
                    hr = pThreatMap->GetValue(ThreatTracking::TrackingData_Compressed_CANMODIFYCONTAINEDFILE, dwCanModify);
                    if(SUCCEEDED(hr) && dwCanModify == 0)
                        sRet.LoadString(IDS_SDESC_CANTRECOMPOSE);
                    else
                        sRet.LoadString(IDS_SDESC_REMOVEFAILED);
                    break;
                }
                CASE_LOAD_STRING(ThreatTracking::ThreatState_DoNotDelete, sRet, IDS_SDESC_DND);
                default:
                    CCTRACEE(CCTRCTX L"Invalid threat state!");
                    ATLASSERT(FALSE);
                    return E_FAIL;
            }

            // Check for "Reboot Required" ... if true, display that instead of state
            bool bRebootRequired = false;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_REBOOTREQUIRED, bRebootRequired);
            if(SUCCEEDED(hr) && bRebootRequired)
            {
                sRet.LoadString(IDS_SDESC_REBOOTREQ);
            }
        }
    }
    else if(wcsnicmp(pszKey, L"DEPENDS", 7) == 0)
    {
        // Get the options
        CStringW sYesOnly = GetParamValue(pszKey, L"YESONLY");
        CStringW sDash = GetParamValue(pszKey, L"DASH");
        CStringW sPlain = GetParamValue(pszKey, L"PLAIN");

        // dependency flag is inside the threat level data
        IAVMapDwordDataPtr spMapLevels;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMapLevels));
        LOG_FAILURE_AND_RETURN(L"Failed to get individual threat levels map!", hr);
        if(spMapLevels)
        {
            DWORD dwDepends;
            hr = spMapLevels->GetValue(ccEraser::IAnomaly::Dependency, dwDepends);
            LOG_FAILURE_AND_RETURN(L"Failed to get dependency flag individual threat levels map!", hr);

            if(dwDepends == 0 && sYesOnly[0] != L'1')
                sRet.LoadString(IDS_HAS_NO_DEPENDENCIES);
            else if(dwDepends != 0)
            {
                if(sPlain[0] == L'1')
                    sRet.LoadString(IDS_HAS_DEPENDENCIES_PLAIN);
                else
                    sRet.LoadString(IDS_HAS_DEPENDENCIES);
            }
        }

        if(!sRet.IsEmpty() && sDash[0] == L'1')
            sRet.Insert(0, L" - ");

        hr = S_OK;
    }
    else if(wcsicmp(pszKey, L"VID") == 0)
    {
        DWORD dwVID;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_VID, dwVID);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat VID!", hr);
        sRet.Format(L"%lu", dwVID);
    }
    else if(wcsicmp(pszKey, L"NAME") == 0)
    {
        cc::IStringPtr spName;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_NAME, spName);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat name!", hr);
        sRet = spName->GetStringW();
    }
    else if(wcsicmp(pszKey, L"TCATS") == 0)
    {
        CThreatCatInfo ctiCatInfo;

        IAVArrayDataPtr spArrCats;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_THREATCATEGORIES, AVDataTypes::eTypeArray, reinterpret_cast<ISymBase*&>(spArrCats));
        LOG_FAILURE_AND_RETURN(L"Failed to get threat categories!", hr);
        DWORD dwCats;
        hr = spArrCats->GetCount(dwCats);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat category count!", hr);
        for(DWORD dwIdx = 0; dwIdx < dwCats; ++dwIdx)
        {
            ccEraser::IAnomaly::Anomaly_Category eCategory;
            if(SUCCEEDED(spArrCats->GetValue(dwIdx, (DWORD&)eCategory)))
            {
                // Call CThreatCatInfo to get the category name
                CStringW sCat;
                ctiCatInfo.GetCategoryText(eCategory, sCat.GetBuffer(256), 256);
                sCat.ReleaseBuffer();

                if(!sRet.IsEmpty() && !sCat.IsEmpty())
                    sRet += L", ";

                sRet += sCat;
            }
        }
    }
    else if(wcsicmp(pszKey, L"DESCTCATS") == 0)
    {
        CThreatCatInfo ctiCatInfo;

        IAVArrayDataPtr spArrCats;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_THREATCATEGORIES, AVDataTypes::eTypeArray, reinterpret_cast<ISymBase*&>(spArrCats));
        LOG_FAILURE_AND_RETURN(L"Failed to get threat categories!", hr);
        DWORD dwCats;
        hr = spArrCats->GetCount(dwCats);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat category count!", hr);
        for(DWORD dwIdx = 0; dwIdx < dwCats; ++dwIdx)
        {
            ccEraser::IAnomaly::Anomaly_Category eCategory;
            if(SUCCEEDED(spArrCats->GetValue(dwIdx, (DWORD&)eCategory)))
            {
                // Call CThreatCatInfo to get the category name
                CStringW sCat, sDesc;
                ctiCatInfo.GetCategoryTextAndDesc(eCategory, sCat.GetBuffer(256), 256, sDesc.GetBuffer(1024), 1024);
                sCat.ReleaseBuffer();
                sDesc.ReleaseBuffer();

                // Put a space between sentences
                if(!sRet.IsEmpty() && !sDesc.IsEmpty())
                    sRet += L" ";

                // Add this sentence to the output
                sRet += sDesc;
            }
        }
    }
    else if(wcsicmp(pszKey, L"DETTIME") == 0)
    {
        DWORD dwDetTime;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_DETECTION_TIME, dwDetTime);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat detection time!", hr);
    }
    else if(wcsicmp(pszKey, L"REMTIME") == 0)
    {
        DWORD dwRemTime;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_REMOVAL_TIME, dwRemTime);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat removal time!", hr);
    }
    else if(wcsicmp(pszKey, L"INFOURL") == 0)
    {
        // Get the format string
        CStringW sURLFmt;
        CISVersion::LoadString (sURLFmt, IDS_BRANDING_THREAT_INFO_URL);

        // Get the VID
        DWORD dwVID;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_VID, dwVID);
        LOG_FAILURE_AND_RETURN(L"Failed to get threat VID!", hr);

        // Build the full URL
        sRet.Format(sURLFmt, dwVID);
    }
    else if(wcsnicmp(pszKey, L"TLEVEL", 6) == 0)
    {
        HRESULT hr = E_FAIL;
        ThreatTracking::eThreatLevels eLevel;
        CStringW sProp = GetParamValue(pszKey, L"PROP");
        if(!sProp.IsEmpty())
        {
            // Prop param was specified, get the correct individual tlevel
            IAVMapDwordDataPtr spMapLevels;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMapLevels));
            LOG_FAILURE_AND_RETURN(L"Failed to get individual threat levels map!", hr);
            if(spMapLevels)
            {
                DWORD dwLevel = _wtol(sProp);
                if(dwLevel >= ccEraser::IAnomaly::Stealth
                   && dwLevel <= ccEraser::IAnomaly::Privacy)
                {
                    hr = spMapLevels->GetValue(dwLevel, (DWORD&)eLevel);
                    LOG_FAILURE_AND_RETURN(L"Failed to get threat level!", hr);
                }
                else
                {
                    LOG_FAILURE_AND_RETURN(L"Invalid threat level requested!", E_INVALIDARG);
                }
            }
        }
        else
        {
            // Overall threat level
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
            LOG_FAILURE_AND_RETURN(L"Failed to get global threat level!", hr);
        }

        // All failures above result in a return... so we must succeed to get here
        switch(eLevel)
        {
            CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Low, sRet, IDS_TL_LOW);
            CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Med, sRet, IDS_TL_MED);
            CASE_LOAD_STRING(ThreatTracking::ThreatLevel_High, sRet, IDS_TL_HIGH);
            default:
                CCTRACEE(CCTRCTX L"Invalid threat level!");
                ATLASSERT(FALSE);
                return E_FAIL;
        }
    }
    else if(wcsicmp(pszKey, L"TLIMG") == 0)
    {
        // Overall threat level
        ThreatTracking::eThreatLevels eLevel;
        HRESULT hr = pThreatMap->GetValue(ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
        LOG_FAILURE_AND_RETURN(L"Failed to get global threat level!", hr);

        // All failures above result in a return... so we must succeed to get here
        switch(eLevel)
        {
            case ThreatTracking::ThreatLevel_Low:
                sRet = L"alert_icon_yellow.png";
                break;
            case ThreatTracking::ThreatLevel_Med:
                sRet = L"alert_icon_yellow.png";
                break;
            case ThreatTracking::ThreatLevel_High:
                sRet = L"alert_icon_red.png";
                break;
            default:
                CCTRACEE(CCTRCTX L"Invalid threat level!");
                ATLASSERT(FALSE);
                return E_FAIL;
        }
    }
    else if(wcsicmp(pszKey, L"TLINT") == 0)
    {
        // Overall threat level
        ThreatTracking::eThreatLevels eLevel;
        HRESULT hr = pThreatMap->GetValue(ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
        LOG_FAILURE_AND_RETURN(L"Failed to get global threat level!", hr);

        // All failures above result in a return... so we must succeed to get here
        switch(eLevel)
        {
            case ThreatTracking::ThreatLevel_Low:
                sRet = L"3";
                break;
            case ThreatTracking::ThreatLevel_Med:
                sRet = L"2";
                break;
            case ThreatTracking::ThreatLevel_High:
                sRet = L"1";
                break;
            default:
                CCTRACEE(CCTRCTX L"Invalid threat level!");
                ATLASSERT(FALSE);
                return E_FAIL;
        }
    }
    else if(wcsnicmp(pszKey, L"DESCTLEVEL", 10) == 0)
    {
        HRESULT hr = E_FAIL;
        ThreatTracking::eThreatLevels eLevel;
        DWORD dwLevel;
        CStringW sProp = GetParamValue(pszKey, L"PROP");
        if(!sProp.IsEmpty())
        {
            // Prop param was specified, get the correct individual tlevel
            IAVMapDwordDataPtr spMapLevels;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMapLevels));
            LOG_FAILURE_AND_RETURN(L"Failed to get individual threat levels map!", hr);
            if(spMapLevels)
            {
                dwLevel = _wtol(sProp);
                if(dwLevel >= ccEraser::IAnomaly::Stealth
                   && dwLevel <= ccEraser::IAnomaly::Privacy)
                {
                    hr = spMapLevels->GetValue(dwLevel, (DWORD&)eLevel);
                    LOG_FAILURE_AND_RETURN(L"Failed to get threat level!", hr);
                }
                else
                {
                    LOG_FAILURE_AND_RETURN(L"Invalid threat level requested!", E_INVALIDARG);
                }
            }
        }
        else
        {
            // Overall threat level
            dwLevel = 0;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
            LOG_FAILURE_AND_RETURN(L"Failed to get global threat level!", hr);
        }

        // Check the threat level first (to reduce code in the switch statement below)
        if(eLevel > ThreatTracking::ThreatLevel_High)
        {
            CCTRACEE(CCTRCTX L"Invalid threat level!");
            ATLASSERT(FALSE);
            return E_FAIL;
        }

        // All failures above result in a return... so we must succeed to get here
        switch(dwLevel)
        {
            case 0:     // Overall
            {
                switch(eLevel)
                {
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Low, sRet, IDS_THREAT_MATRIX_DESC_OVERALL_LOW);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Med, sRet, IDS_THREAT_MATRIX_DESC_OVERALL_MED);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_High, sRet, IDS_THREAT_MATRIX_DESC_OVERALL_HIGH);
                }
                break;
            }
            case ccEraser::IAnomaly::Stealth:
            {
                switch(eLevel)
                {
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Low, sRet, IDS_THREAT_MATRIX_DESC_STEALTH_LOW);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Med, sRet, IDS_THREAT_MATRIX_DESC_STEALTH_MED);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_High, sRet, IDS_THREAT_MATRIX_DESC_STEALTH_HIGH);
                }
                break;
            }
            case ccEraser::IAnomaly::Removal:
            {
                switch(eLevel)
                {
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Low, sRet, IDS_THREAT_MATRIX_DESC_REMOVE_LOW);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Med, sRet, IDS_THREAT_MATRIX_DESC_REMOVE_MED);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_High, sRet, IDS_THREAT_MATRIX_DESC_REMOVE_HIGH);
                }
                break;
            }
            case ccEraser::IAnomaly::Performance:
            {
                switch(eLevel)
                {
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Low, sRet, IDS_THREAT_MATRIX_DESC_PERF_LOW);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Med, sRet, IDS_THREAT_MATRIX_DESC_PERF_MED);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_High, sRet, IDS_THREAT_MATRIX_DESC_PERF_HIGH);
                }
                break;
            }
            case ccEraser::IAnomaly::Privacy:
            {
                switch(eLevel)
                {
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Low, sRet, IDS_THREAT_MATRIX_DESC_PRIV_LOW);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_Med, sRet, IDS_THREAT_MATRIX_DESC_PRIV_MED);
                    CASE_LOAD_STRING(ThreatTracking::ThreatLevel_High, sRet, IDS_THREAT_MATRIX_DESC_PRIV_HIGH);
                }
                break;
            }
        }
    }
    else if(wcsnicmp(pszKey, L"REMDETAILS", 10) == 0)
    {
        CString sFmtVer = GetParamValue(pszKey, L"FMT");
        CString sCSS = GetParamValue(pszKey, L"CLASS");
        CString sType = GetParamValue(pszKey, L"TYPE");
        IAVArrayDataPtr spDispData;

        // Get counts of each type
        MAP_TYPE_COUNT mapCounts;
        hr = GetTypeCountsForThreat(pThreatMap, eType, mapCounts, true);
        LOG_FAILURE_AND_RETURN(L"Failed to get per type counts for threat", hr);

        // Load the header...
        CStringW sHeader, sTmp, sTmpB;
        if(sFmtVer[0] == L'T')  // Table
            sHeader.LoadString(IDS_REMTYPES_TH);
        else if(sFmtVer[0] == L'X')   // XML
            sHeader = L"<Remediations>";

        // Add the header to the output
        sRet += sHeader;

        // Walk the list of types...
        CAtlArray<CStringW> arrTargets;
        for(DWORD dwIdx = ccEraser::FirstRemediationType; dwIdx <= ccEraser::LastRemediationType; ++dwIdx)
        {
            // Get this item from the map
            MAP_TYPE_COUNT::CPair* pPair = mapCounts.Lookup((ccEraser::eObjectType)dwIdx);
            if(pPair)
            {
                if(sFmtVer[0] == L'X')  // XML
                    sTmpB = GetXMLTagForEraserType(pPair->m_key, true, false);
                else
                {
                    switch(pPair->m_key)
                    {
                        CASE_LOAD_STRING_PLURAL(ccEraser::RegistryRemediationActionType, sTmpB, pPair->m_value, IDS_REM_REG, IDS_REM_REGS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::FileRemediationActionType, sTmpB, pPair->m_value, IDS_REM_FILE, IDS_REM_FILES);
                        CASE_LOAD_STRING_PLURAL(ccEraser::ProcessRemediationActionType, sTmpB, pPair->m_value, IDS_REM_PROC, IDS_REM_PROCS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::BatchRemediationActionType, sTmpB, pPair->m_value, IDS_REM_BATCH, IDS_REM_BATCHS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::INIRemediationActionType, sTmpB, pPair->m_value, IDS_REM_INI, IDS_REM_INIS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::ServiceRemediationActionType, sTmpB, pPair->m_value, IDS_REM_SERV, IDS_REM_SERVS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::COMRemediationActionType, sTmpB, pPair->m_value, IDS_REM_COM, IDS_REM_COMS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::HostsRemediationActionType, sTmpB, pPair->m_value, IDS_REM_HOST, IDS_REM_HOSTS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::LSPRemediationActionType, sTmpB, pPair->m_value, IDS_REM_LSP, IDS_REM_LSPS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::BrowserCacheRemediationActionType, sTmpB, pPair->m_value, IDS_REM_CACHE, IDS_REM_CACHES);
                        CASE_LOAD_STRING_PLURAL(ccEraser::SystemRemediationActionType, sTmpB, pPair->m_value, IDS_REM_SYS, IDS_REM_SYSS);
                        CASE_LOAD_STRING_PLURAL(ccEraser::CookieRemediationActionType, sTmpB, pPair->m_value, IDS_REM_COOKIE, IDS_REM_COOKIES);
                        default:
                            CCTRACEE(CCTRCTX L"Unknown remediation type! (%d)", pPair->m_key);
                            ATLASSERT(FALSE);
                            break;
                    }
                }

                // Fill in the count
                sTmp.Format(sTmpB, pPair->m_value);

                // If we are doing full output... get the target list for this item
                CStringW sTargets;
                if(sType[0] == L'F')
                {
                    // Make sure we have display data
                    if(!spDispData)
                    {
                        // Get the display data
                        if(pDisplayData)
                            spDispData = pDisplayData;
                        else
                        {
                            // Get the threats ID
                            GUID idThreat = m_arrThreats[iIndex].GetID();
                            hr = GetAnomalyDisplayData(pThreatMap, idThreat, false, spDispData);
                            LOG_FAILURE(L"Failed to get anomaly display data", hr);
                        }
                    }

                    CAtlArray<CStringW> arrActions;
                    arrTargets.RemoveAll();
                    hr = GetTargetsForThreat(spDispData, eType, pThreatMap, pPair->m_key, arrTargets, true, &arrActions);
                    LOG_FAILURE_AND_RETURN(L"Failed to get targets for threat!", hr);

                    // Add a header based on type
                    if(sFmtVer[0] == L'T')  // Table
                        sTargets.Format(L"<table id=\"TargetsTable%d\" class=\"TargetsTable\">", pPair->m_key);
                    else if(sFmtVer[0] == L'D')  // DIV
                        sTargets.Format(L"<div id=\"TargetsDiv%d\" class=\"TargetsDiv\">", pPair->m_key);

                    // Loop through all the targets
                    CStringW sTargetTmp;
                    for(size_t iIdx = 0; iIdx < arrTargets.GetCount(); ++iIdx)
                    {
                        // Build this target
                        if(sFmtVer[0] == L'T')  // Table
                            sTargetTmp.Format(L"<tr id=\"TargetsTR%d%d\" class=\"TargetTR\"><td>%s</td></tr>", pPair->m_key, iIdx, arrTargets[iIdx].GetString());
                        else if(sFmtVer[0] == L'D')  // DIV
                            sTargetTmp.Format(L"<div id=\"TypeDiv%d%d\" class=\"TargetDIV\">%s</div>", pPair->m_key, iIdx, arrTargets[iIdx].GetString());
                        else if(sFmtVer[0] == L'P')  // Plain text
                        {
                            sTargetTmp.Format(L"%s - %s", arrTargets[iIdx].GetString(), arrActions[iIdx].GetString());
                            if(!sTargets.IsEmpty())
                                sTargetTmp.Insert(0, L"\r\n");
                        }
                        else if(sFmtVer[0] == L'X')  // XML
                        {
                            CString sTagFmt = GetXMLTagForEraserType(pPair->m_key, false, false);
                            CString sTagEnd = GetXMLTagForEraserType(pPair->m_key, false, true);

                            // Fill in the "actiontaken"
                            CString sTag;
                            sTag.Format(sTagFmt, arrActions[iIdx].GetString());

                            // Put it all together
                            sTargetTmp.Format(L"%s%s%s", sTag.GetString(), arrTargets[iIdx].GetString(), sTagEnd.GetString());
                        }
                        else
                            sTargetTmp = L"";

                        // Add it to the details list...
                        sTargets += sTargetTmp;
                    }

                    // Add a footer based on type
                    if(sFmtVer[0] == L'T')  // Table
                        sTargets += L"</table>";
                    else if(sFmtVer[0] == L'D')  // DIV
                        sTargets += L"</div>";
                }

                // Add a row for this item
                if(sFmtVer[0] == L'T')  // Table
                    sTmpB.Format(L"<tr id=\"TypeRow%d\" class=\"%s\"><td>%s%s</td></tr>", pPair->m_key, sCSS.GetString(), sTmp.GetString(), sTargets.GetString());
                else if(sFmtVer[0] == L'D')  // DIV
                    sTmpB.Format(L"<div id=\"TypeDiv%d\" class=\"%s\">%s%s</div>", pPair->m_key, sCSS.GetString(), sTmp.GetString(), sTargets.GetString());
                else if(sFmtVer[0] == L'P')  // Plain text
                    sTmpB.Format(L"%s\r\n%s\r\n", sTmp.GetString(), sTargets.GetString());
                else if(sFmtVer[0] == L'X')  // XML
                    sTmpB.Format(L"%s%s", sTmp.GetString(), sTargets.GetString());
                else
                {
                    hr = E_INVALIDARG;
                    LOG_FAILURE_AND_BREAK(L"Invalid 'FMT' specified for REMDETAILS!", hr);
                }

                // Close the tag (if XML)
                if(sFmtVer[0] == L'X')  // XML
                    sTmpB += GetXMLTagForEraserType(pPair->m_key, true, true);

                // Add this item to the Output
                sRet += sTmpB;
            }
        }

        // Add the footer
        if(!sRet.IsEmpty())
        {
            if(sFmtVer[0] == L'X')   // XML
                sRet += L"</Remediations>";
        }

        hr = S_OK;
    }
    else if(wcsnicmp(pszKey, L"ACTIONDROPDOWN", 14) == 0)
    {
        CStringW sPreID = GetParamValue(pszKey, L"PREID");
        CStringW sExclude, sFix, sRev, sIgnore;
        CStringW sID;
        sID.Format(L"%s%d", sPreID.GetString(), iIndex);

        // Get this items current "action"
        CThreat::eIntThreatAction eAction = m_arrThreats[iIndex].GetAction();
        if(eAction == CThreat::eIntAct_None)
        {
            // Select the default action for this item
            m_arrThreats[iIndex].DefaultAction();

            // Get the new action
            eAction = m_arrThreats[iIndex].GetAction();
        }

        // Get this threats state
        DWORD dwState = 0;
        hr = pThreatMap->GetValue(ThreatTracking::TrackingData_CURRENT_STATE, dwState);
        LOG_FAILURE(L"Failed to get threat state!", hr);

        // If the state is a remove failure, set the action to "review"
        if(ThreatTracking::RemoveFailure(dwState))
        {
            eAction = CThreat::eIntAct_Review;
            m_arrThreats[iIndex].SetAction(eAction);
        }

        CStringW sExSel, sFixSel, sRevSel, sIgnSel;
        CStringW sExStar, sFixStar, sRevStar, sIgnStar;
        switch(eAction)
        {
            case CThreat::eIntAct_None:     // none
                CCTRCTXW0(L"Creating dropdown for threat with an action of NONE");
                break;
            case CThreat::eIntAct_Fix:      // fix
                sFixSel = L"selected";
                sFixStar = L"*";
                break;
            case CThreat::eIntAct_Exclude:  // exclude
                sExSel = L"selected";
                sExStar = L"*";
                break;
            case CThreat::eIntAct_Review:   // review
                sRevSel = L"selected";
                sRevStar = L"*";
                break;
            case CThreat::eIntAct_Ignore:   // ignore
                sIgnSel = L"selected";
                sIgnStar = L"*";
                break;
        }

        // Load our resources
        sExclude.LoadString(IDS_OPT_EXCLUDE);
        sFix.LoadString(IDS_OPT_FIX);
        sRev.LoadString(IDS_OPT_REVIEW);
        sIgnore.LoadString(IDS_OPT_IGNORE);

        sRet.Format(L"<div class=\"actiondd\"><select id=\"%s\" type=\"select-dropdown\">", sID.GetString());

        // If the scanUI object is NULL, we are in email mode and should offer fix (named delete), exclude, and ignore...
        CStringW sTmp;
        if(m_spScanUI == NULL)
        {
            // Add the fix option
            sFix.LoadString(IDS_OPT_FIX_EMAIL);
            sTmp.Format(L"<option value=\"%d\" %s>%s%s</option>", CThreat::eIntAct_Fix, sFixSel.GetString(), sFix.GetString(), sFixStar.GetString());
            sRet += sTmp;

            // Only offer the exclude option if this threat is non-viral
            if(!m_arrThreats[iIndex].IsViral())
            {
                sTmp.Format(L"<option value=\"%d\" %s>%s%s</option>", CThreat::eIntAct_Exclude, sExSel.GetString(), sExclude.GetString(), sExStar.GetString());
                sRet += sTmp;
            }

            // Add the ignore state
            sTmp.Format(L"<option value=\"%d\" %s>%s%s</option>", CThreat::eIntAct_Ignore, sIgnSel.GetString(), sIgnore.GetString(), sIgnStar.GetString());
            sRet += sTmp;
        }
        else
        {
            // Only offer the fix option if this items status is not "do not delete"
            // AND
            // A prior removal did not fail, otherwise, the user is given "Review" instead of "Fix".
            if(dwState != ThreatTracking::ThreatState_DoNotDelete
                && !ThreatTracking::RemoveFailure(dwState))
            {
                sTmp.Format(L"<option value=\"%d\" %s>%s%s</option>", CThreat::eIntAct_Fix, sFixSel.GetString(), sFix.GetString(), sFixStar.GetString());
                sRet += sTmp;
            }
            else
            {
                // Do not delete items require manual removal...
                // For these, we tell the user to look at the Symantec website.
                sTmp.Format(L"<option value=\"%d\" %s>%s%s</option>", CThreat::eIntAct_Review, sRevSel.GetString(), sRev.GetString(), sRevStar.GetString());
                sRet += sTmp;
            }

            // Offer "Ignore Once", if this is a Low risk threat
            ThreatTracking::eThreatLevels eLevel = (ThreatTracking::eThreatLevels)-1;
            hr = pThreatMap->GetValue(ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
            LOG_FAILURE(L"Failed to get overall risk level for threat ... taking it as received from CAV", hr);
            if(eLevel == ThreatTracking::ThreatLevel_Low)
            {
                // Add the ignore state
                sTmp.Format(L"<option value=\"%d\" %s>%s%s</option>", CThreat::eIntAct_Ignore, sIgnSel.GetString(), sIgnore.GetString(), sIgnStar.GetString());
                sRet += sTmp;
            }            

            // Only offer the exclude option if this threat is non-viral
            if(!m_arrThreats[iIndex].IsViral())
            {
                sTmp.Format(L"<option value=\"%d\" %s>%s%s</option>", CThreat::eIntAct_Exclude, sExSel.GetString(), sExclude.GetString(), sExStar.GetString());
                sRet += sTmp;
            }
        }

        // Close the select statement
        sRet += L"</select></div>";

    }

    return S_OK;
}

CStringW CThreatTracker::GetXMLTagForEraserType(ccEraser::eObjectType eType, bool bOuter, bool bClose)
{
    CString sTag;
    switch(eType)
    {
        case ccEraser::RegistryRemediationActionType:
	        sTag = L"Registry";
	        break;
        case ccEraser::FileRemediationActionType:
	        sTag = L"File";
	        break;
        case ccEraser::ProcessRemediationActionType:
	        sTag = L"Process";
	        break;
        case ccEraser::BatchRemediationActionType:
	        sTag = L"Batch";
	        break;
        case ccEraser::INIRemediationActionType:
	        sTag = L"INI";
	        break;
        case ccEraser::ServiceRemediationActionType:
	        sTag = L"Service";
	        break;
        case ccEraser::COMRemediationActionType:
	        sTag = L"COM";
	        break;
        case ccEraser::HostsRemediationActionType:
	        sTag = L"Hosts";
	        break;
        case ccEraser::LSPRemediationActionType:
	        sTag = L"LSP";
	        break;
        case ccEraser::BrowserCacheRemediationActionType:
	        sTag = L"BrowserCache";
	        break;
        case ccEraser::SystemRemediationActionType:
	        sTag = L"System";
	        break;
        case ccEraser::CookieRemediationActionType:
	        sTag = L"Cookie";
	        break;
    }

    CString sAttribs;
    if(bOuter)
    {
        sTag += L"Targets";
        sAttribs = L" count=\"%d\"";
    }
    else if(!bClose)
    {
        sAttribs = L" actiontaken=\"%s\"";
    }

    CString sRet;
    if(bClose)
        sRet.Format(L"</%s>", sTag.GetString());
    else
        sRet.Format(L"<%s%s>", sTag.GetString(), sAttribs.GetString());

    return sRet;
}

CStringW CThreatTracker::GetParamValue(LPCWSTR pszKey, LPCWSTR pszParamName)
{
    CStringW sValue = pszKey;
    CStringW sParamSrch = pszParamName;
    sParamSrch += L"=";
    int nParamStart = sValue.Find(sParamSrch);
    if(nParamStart != -1)
    {
        sValue = sValue.Mid(nParamStart + sParamSrch.GetLength());
        int nParamEnd = sValue.FindOneOf(L",;]}");
        if(nParamEnd != -1)
            return sValue.Left(nParamEnd);
    }

    return L"";
}

bool CThreatTracker::ExportThreats(LPCWSTR pszOutputFile, HWND hParentWnd, bool bShowProgress)
{
    // Create our progress dialog
    m_sExportFile = pszOutputFile;

    // Clear out the dialog holder
    m_spDlgDoc = NULL;

    // Lock the data array (keep the thread from going until we are ready)
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Create the UI thread
    if(!Create(NULL, 0, 0))
        return false;

    // If we are to show progress, we can just wait for the DoModal
    // to return, otherwise we must wait for the thread to exit.
    HRESULT hr = S_OK;
    if(bShowProgress)
    {
        m_spDlgDoc = new CExportProgressDlg();

        // Get the dialog object
        bool bDisplayed = false;
        symhtml::ISymHTMLDialogPtr spDialog;
        if(SYM_SUCCEEDED(symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog)))
        {
            if(!m_spDlgDoc)
            {
                CCTRACEE(CCTRCTX L"new of CExportProgressDlg failed!");
                PostTerminate();
                return false;
            }

            // Release the thread
            sLock.Unlock();

            // Display the progress UI
            int nResult = 0;
            hr = spDialog->DoModal(hParentWnd, m_spDlgDoc, &nResult);
            if(SUCCEEDED(hr))
                bDisplayed = true;
            else
                CCTRACEE(CCTRCTX L"DoModal failed... waiting for export thread to exit (hr=%08X)", hr);
        }
        
        // We failed at some point... we need to wait for the thread to exit
        if(!bDisplayed)
        {
            // Release the thread
            sLock.Unlock();

            WaitForExit(INFINITE);
        }
    }
    else
    {
        // Release the thread
        sLock.Unlock();

        // Wait for the UI thread to exit
        WaitForExit(INFINITE);
    }

    return SUCCEEDED(hr);
}

int CThreatTracker::Run()
{
    CCTRCTXI0(L"--ThreatTracker Thread Begin");

    // Lock the data array
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // If we are displaying UI, wait for the UI
    if(m_spDlgDoc)
    {
        BOOL bRet = FALSE;
        while(S_FALSE == m_spDlgDoc->IsDocumentComplete())
        {
            // Wait 100ms
            bRet = WaitForTerminate(100);
        }

        // If were terminated while waiting, exit
        if(bRet)
            return 0;
    }

    // Buffers for each state
    CStringW sResolved, sUnresolved;

    // Load the template
    CString sTemplate, sBody, sInfo, sCounts;
    if(m_sExportFile.Right(3).CompareNoCase(L"xml") == 0)
    {
        sTemplate.LoadString(IDS_EXPORT_XML_TEMPLATE);
        sBody.LoadString(IDS_EXPORT_XML_BODY);
        sInfo.LoadString(IDS_EXPORT_XML_INFO);
        sCounts.LoadString(IDS_EXPORT_XML_COUNTS);
    }
    else
    {
        sTemplate.LoadString(IDS_EXPORT_TXT_TEMPLATE);
        sBody.LoadString(IDS_EXPORT_TXT_BODY);
        sInfo.LoadString(IDS_EXPORT_TXT_INFO);
        sCounts.LoadString(IDS_EXPORT_TXT_COUNTS);
    }

    // Export the scan details
    DWORD dwScanTime = 0;
    HRESULT hr = m_spScanResults->GetValue(ManualScanner::AV_SCANCOMPLETE_SCANTIME, dwScanTime);
    LOG_FAILURE(L"Failed to get scan time from results map", hr);

    AVModule::IAVMapStrDataPtr spScanOpts;
    hr = m_spScanResults->GetValue(ManualScanner::AV_SCANCOMPLETE_SCANOPTIONS, AVDataTypes::eTypeMapString, reinterpret_cast<ISymBase*&>(spScanOpts));
    LOG_FAILURE(L"Failed to get scan options from results map", hr);
    // Put all the options together
    CStringW sOpts;
    CRegKey rkExtras;
    if(ERROR_SUCCESS == rkExtras.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Symantec\\Norton AntiVirus\\ExportExtra", KEY_READ)
        && spScanOpts)
    {
        DWORD dwCount;
        spScanOpts->GetCount(dwCount);
        for(DWORD dwItem = 0; dwItem < dwCount; ++dwItem)
        {
            CStringW sItem, sValue;
            cc::IStringPtr spKey;
            spScanOpts->GetKeyAtIndex(dwItem, spKey);
            AVDataTypes::eDataTypes eType;
            spScanOpts->GetItemValueType(spKey->GetStringW(), eType);
            switch(eType)
            {
                case AVDataTypes::eTypeDWORD:
                {
                    DWORD dwTmp;
                    spScanOpts->GetValue(spKey->GetStringW(), dwTmp);
                    sValue.Format(L"%lu", dwTmp);
                    break;
                }
                case AVDataTypes::eTypeINT64:
                {
                    __int64 nVal;
                    spScanOpts->GetValue(spKey->GetStringW(), nVal);
                    sValue.Format(L"%d", nVal);
                    break;
                }
                case AVDataTypes::eTypeSTRING:
                {
                    cc::IStringPtr spVal;
                    spScanOpts->GetValue(spKey->GetStringW(), spVal);
                    if(spVal)
                        sValue = spVal->GetStringW();
                    break;
                }
                case AVDataTypes::eTypeGUID:
                {
                    GUID idVal;
                    spScanOpts->GetValue(spKey->GetStringW(), idVal);
                    sValue.Format(GUID_FORMAT_STR(idVal));
                }
                // All other types unsupported
            }

            // Put it together
            sItem.Format(L"%s=%s", spKey->GetStringW(), sValue.GetString());

            // Add it to the list
            if(!sOpts.IsEmpty())
                sOpts += L", ";

            sOpts += sItem;
        }
    }

    AVModule::IAVArrayDataPtr spScanItems;
    hr = m_spScanResults->GetValue(ManualScanner::AV_SCANCOMPLETE_SCANITEMS, AVDataTypes::eTypeArray, reinterpret_cast<ISymBase*&>(spScanItems));
    LOG_FAILURE(L"Failed to get scan items from results map", hr);
    // Put the targets together
    CStringW sTargets;
    if(spScanItems)
    {
        DWORD dwCount;
        spScanItems->GetCount(dwCount);
        for(DWORD dwItem = 0; dwItem < dwCount; ++dwItem)
        {
            cc::IStringPtr spVal;
            spScanItems->GetValue(dwItem, spVal);

            // Add it to the list
            if(spVal)
            {
                if(!sTargets.IsEmpty())
                    sTargets += L", ";

                sTargets += spVal->GetStringW();
            }
        }
    }

    // Get the scan stats
    CStringW sScanCounts;
    if(m_spScanUI)
    {
        // sCounts
        avScanUI::ScanStatistics scanStats;
        m_spScanUI->GetScanStats(scanStats);

        // Put all the stats together
        int nTotalScanned = scanStats.nTotalScanned;
        int nFound = scanStats.nVirDetected + scanStats.nNonVirDetected;
        int nRemoved = scanStats.nVirRemoved + scanStats.nNonVirRemoved;
        int nAttention = nFound - nRemoved;
        int nFiles = scanStats.nFileItems + scanStats.nDirectoryItems;
        int nRegistry = scanStats.nRegistryItems;
        int nProcesses = scanStats.nProcessItems + scanStats.nServiceItems + scanStats.nStartupItems + scanStats.nCOMItems;
        int nBrowserNet = scanStats.nBrowserCacheItems + scanStats.nCookieItems + scanStats.nHostsItems + scanStats.nLSPItems;
        int nHeuristic = scanStats.nApplicationHeuristicItems;
        int nOther = scanStats.nBatchItems + scanStats.nINIItems;

        // Put it in to text/xml
        sScanCounts.Format(sCounts, nTotalScanned, nFiles, nRegistry, nProcesses, nBrowserNet,
                           nHeuristic, nOther, nFound, nRemoved, nAttention);
    }

    // Build the stats
    CStringW sScanInfo;
    sScanInfo.Format(sInfo, dwScanTime, sOpts.GetString(), sTargets.GetString(), sScanCounts.GetString());

    // Loop through all items
    size_t iItems = m_arrThreats.GetCount();
    for(size_t iIndex = 0; iIndex < iItems; ++iIndex)
    {
        // Check if we have been canceled
        if(m_spDlgDoc && m_spDlgDoc->GetCanceled())
        {
            m_spDlgDoc->EndDialog(IDCANCEL);
            break;
        }

        // Get this threats ID
        GUID idThreat;
        CThreat& cThreatItem = m_arrThreats.GetAt(iIndex);
        IAVMapDwordData* pThreatItem = cThreatItem.GetThreatMap();
        HRESULT hr = pThreatItem->GetValue(ThreatTracking::TrackingData_THREATID, idThreat);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Failed to get threat ID!", hr);
            continue;
        }

        // Get this threats state
        DWORD dwState;
        hr = pThreatItem->GetValue(ThreatTracking::TrackingData_CURRENT_STATE, dwState);
        if(FAILED(hr))
        {
            THREAT_FAILURE(idThreat, L"Failed to get the anomaly stream", hr);
        }

        // Generate the details for this threat based on its state
        bool bThreatNeedsAttention = cThreatItem.IsThreatIncluded(CThreat::eTTState_RequireAttention);

        // For exported results, "Reviewed" is not considered resolved
        if(cThreatItem.GetIntState() == CThreat::eIntState_Reviewed)
            bThreatNeedsAttention = true;

        if(bThreatNeedsAttention)
        {
            sUnresolved += GenerateOutputForItem(cThreatItem, sTemplate, iIndex);
        }
        else
        {
            sResolved += GenerateOutputForItem(cThreatItem, sTemplate, iIndex);
        }

        // Update the progress
        int nProg = (int)((iIndex * 100) / iItems);
        if(m_spDlgDoc)
        {
            m_spDlgDoc->SetProgress(nProg);
        }
    }

    if(m_spDlgDoc && !m_spDlgDoc->GetCanceled())
    {
        // Build the content
        CStringW sOutput;
        sOutput.Format(sBody, sScanInfo.GetString(), sResolved.GetString(), sUnresolved.GetString());

        // Write the file data to the user selected output file
        HANDLE hFile = CreateFile(m_sExportFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE)
        {
            DWORD dwWritten = 0;

            // Output the UNICODE BOM
		    WORD wBOM = 0xFEFF;
            if(0 == WriteFile(hFile, &wBOM, sizeof(WORD), &dwWritten, NULL) || dwWritten != sizeof(WORD))
            {
                HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
                LOG_FAILURE(L"Failed to write UNICODE BOM!", hr);
            }

            // Write the contents
            DWORD dwCount = sOutput.GetLength() * sizeof(wchar_t);
            if(0 == WriteFile(hFile, sOutput.GetString(), dwCount, &dwWritten, NULL) || dwWritten != dwCount)
            {
                HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
                LOG_FAILURE(L"Failed to write contents!", hr);
            }

            // Close the file
            CloseHandle(hFile);
        }
    }

    // Close if it is still around
    HWND hWnd;
    if(m_spDlgDoc && SUCCEEDED(m_spDlgDoc->GetHwnd(&hWnd)) && IsWindow(hWnd))
    {
        // Close the dialog
        m_spDlgDoc->EndDialog(IDOK);
    }

    CCTRCTXI0(L"--ThreatTracker Thread End");
    return 0;
}

bool CThreatTracker::FindThreatIndexByID(const GUID& idThreat, size_t& iIndex)
{
    // Lock the data array  (this lock doesn't make much since because after
    // this function returns with an index, the data is unlocked and can change)
    ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

    // Find our matching index  (not very efficient)
    for(size_t i = 0; i < m_arrThreats.GetCount(); ++i)
    {
        // Is this correct item?
        if(memcmp(&(m_arrThreats.GetAt(i).GetID()), &idThreat, sizeof(GUID)) == 0)
        {
            iIndex = i;
            return true;
        }
    }

    return false;
}

// IAVThreatProcessingSink
void CThreatTracker::OnThreatProcessingProgress(DWORD dwRemainingItems, IAVMapDwordData* pCurItemResults, IAVMapDwordData* pRetryParams) throw()
{
    // Purposely empty...
}

//ThreatProcessor::eThreatProcessingOutput
void CThreatTracker::OnThreatProcessingComplete(IAVMapDwordData* pProcessingResults)
{
    // We must scope the data lock, because the RefreshCall will end up using the lock
    {
        // Lock the data array
        ccLib::CSingleLock sLock(&m_mtxData, INFINITE, TRUE);

        // Get this items GUID
        GUID idThreat;
        HRESULT hr = pProcessingResults->GetValue(ThreatProcessor::ProcessOut_ThreatID, idThreat);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Failed to retrieve threat ID! hr=%08X", hr);
            return;
        }
        
        // Find our matching index
        size_t iIndex;
        if(false == FindThreatIndexByID(idThreat, iIndex))
        {
            THREAT_FAILURE(idThreat, L"Failed to find a matching threat in our tracking data", HRESULT_FROM_WIN32(ERROR_NO_MATCH));
            return;
        }

        // Regardless of the result, this threat is no longer pending
        m_arrThreats[iIndex].SetIntState(CThreat::eIntState_CAV);

        // Get the result code
        DWORD dwResult;
        hr = pProcessingResults->GetValue(ThreatProcessor::ProcessOut_ResultCode, dwResult);
        if(FAILED(hr))
        {
            THREAT_FAILURE(idThreat, L"Failed to retrieve the result code", hr);
            return;
        }

        // What type of action was attempted?
        ThreatProcessor::eProcessingAction eActionAttempted;
        hr = pProcessingResults->GetValue(ThreatProcessor::ProcessOut_ActionAttempted, (DWORD&)eActionAttempted);
        if(FAILED(hr))
        {
            THREAT_FAILURE(idThreat, L"Failed to retrieve the action attempted", hr);
            return;
        }

        // Check if the action succeeded or failed (based on the type)
        bool bResolved = false;
        switch(eActionAttempted)
        {
            case ThreatProcessor::ProcessingAction_Fix:
            {
                switch(dwResult)
                {
                    case ThreatProcessor::FixResult_Succeeded:
                    case ThreatProcessor::FixResult_PreviouslyRemoved:
                    case ThreatProcessor::FixResult_RequiresProcTerm:
                    case ThreatProcessor::FixResult_ThreatNoLongerFound:
                        bResolved = true;
                        break;

                    case ThreatProcessor::FixResult_BackupFailed:
                    case ThreatProcessor::FixResult_Failed:
                    case ThreatProcessor::FixResult_InvalidThreatID:
                    case ThreatProcessor::FixResult_InvalidTheatType:
                    case ThreatProcessor::FixResult_InvalidAnomalyBackup:
                    case ThreatProcessor::FixResult_FailedToLoadIScannerw:
                    case ThreatProcessor::FixResult_FailedToLoadIEraser4:
                    case ThreatProcessor::FixResult_CannotRemediate:
                    case ThreatProcessor::FixResult_DoNotDelete:
                    default:
                    {
                        CStringW sLog, sGUID;
                        sGUID.Format(GUID_FORMAT_STR(idThreat));
                        sLog.Format(L"%s Fix action failed for threat(%s) with result=%d", CCTRCTX, sGUID.GetString(), dwResult);
                        CCTRACEE(sLog);
                        break;
                    }
                }
                break;
            }
            case ThreatProcessor::ProcessingAction_Exclude:
            {
                // eExcludeResultCodes
                switch(dwResult)
                {
                    case ThreatProcessor::ExcludeResult_Succeeded:
                        bResolved = true;
                        break;
                    case ThreatProcessor::ExcludeResult_Failed:
                    case ThreatProcessor::ExcludeResult_ExclusionEngineError:
                    case ThreatProcessor::ExcludeResult_InvalidThreatID:
                    case ThreatProcessor::ExcludeResult_ThreatIsViral:
                    default:
                    {
                        CStringW sLog, sGUID;
                        sGUID.Format(GUID_FORMAT_STR(idThreat));
                        sLog.Format(L"%s Fix action failed for threat(%s) with result=%d", CCTRCTX, sGUID.GetString(), dwResult);
                        CCTRACEE(sLog);
                        break;
                    }
                }
                break;
            }
            default:
            {
                THREAT_FAILURE(idThreat, L"Invalid action attempted!", eActionAttempted);
                return;
            }
        }

        // If there is new threat data in the input data, use it
        IAVMapDwordDataPtr spMapThreat;
        hr = pProcessingResults->GetValue(ThreatProcessor::ProcessOut_ThreatData, AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMapThreat));
        if(SUCCEEDED(hr))
        {
            // If resolved, update our stats
            // ... Also, if resolved, clear the action to take
            if(bResolved)
            {
                m_arrThreats[iIndex].SetAction(CThreat::eIntAct_None);
            }

            // Update the threatmap
            m_arrThreats[iIndex].SetThreatMap(spMapThreat);
        }
        else if(!bResolved)
        {
            // Make sure the threat shows a failure state
            IAVMapDwordDataPtr spMap = m_arrThreats[iIndex].GetThreatMap();
            if(spMap)
                spMap->SetValue(ThreatTracking::TrackingData_CURRENT_STATE, (DWORD)ThreatTracking::ThreatState_RemoveFailed);
        }
    }

    // Refresh the UI
    m_spScanUI->RefreshThreatLists();
    m_spScanUI->UpdateScanStats();
}

#pragma  TODO("This must exist in a lib somewhere...")
bool CThreatTracker::GotoURL(LPCWSTR pszUrl, int nShowcmd)
{
	CStringW sEXE;
	bool bGotExe = false;
    LONG lRes;
    ULONG ulLen;

	// Try the "http" and "https" registrations
    CRegKey rkHTTP;
	if(rkHTTP.Open(HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", KEY_READ) == ERROR_SUCCESS)
	{
        ulLen = 1024;
        lRes = rkHTTP.QueryStringValue(NULL, sEXE.GetBuffer(ulLen + 1), &ulLen);
        sEXE.ReleaseBuffer();
		if(lRes == ERROR_SUCCESS && DoesFileExist(sEXE))
			bGotExe = true;
	}			
	
    if(!bGotExe && rkHTTP.Open(HKEY_CLASSES_ROOT, _T("https\\shell\\open\\command")) == ERROR_SUCCESS)
	{
        ulLen = 1024;
        lRes = rkHTTP.QueryStringValue(NULL, sEXE.GetBuffer(ulLen + 1), &ulLen);
        sEXE.ReleaseBuffer();
		if(lRes == ERROR_SUCCESS && DoesFileExist(sEXE))
			bGotExe = true;
	}			

	// Try the ".htm" and ".html" extensions
    CRegKey rkHTM;
	if(!bGotExe && rkHTM.Open(HKEY_CLASSES_ROOT, L".htm\\shell\\open\\command", KEY_READ) == ERROR_SUCCESS)
	{
        ulLen = 1024;
        lRes = rkHTM.QueryStringValue(NULL, sEXE.GetBuffer(ulLen + 1), &ulLen);
        sEXE.ReleaseBuffer();
		if(lRes == ERROR_SUCCESS && DoesFileExist(sEXE))
			bGotExe = true;
	}			
	
    if(!bGotExe && rkHTM.Open(HKEY_CLASSES_ROOT, _T(".html\\shell\\open\\command")) == ERROR_SUCCESS)
	{
        ulLen = 1024;
        lRes = rkHTM.QueryStringValue(NULL, sEXE.GetBuffer(ulLen + 1), &ulLen);
        sEXE.ReleaseBuffer();
		if(lRes == ERROR_SUCCESS && DoesFileExist(sEXE))
			bGotExe = true;
	}			

	// Launch the EXE
	if(bGotExe)
	{
		int nPos = sEXE.Find(L"\"%1\"");
		if(nPos == -1)							// No quotes found
		{
			nPos = sEXE.Find(L"%1");     		// Check for %1, without quotes 
			if(nPos == -1)				    	// No parameter at all...
				nPos = sEXE.GetLength();
		}

        // Remove the param
        sEXE = sEXE.Left(nPos);

        // Add the URL
        sEXE += L" ";
        sEXE += pszUrl;

        // Launch the browser
	    STARTUPINFO	si;
	    PROCESS_INFORMATION	pi;
        memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.wShowWindow = nShowcmd;
		if(CreateProcess(0, sEXE.GetBuffer(), 0, 0, FALSE, 0, 0, 0, &si, &pi))
		{
            sEXE.ReleaseBuffer();
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
            return true;
		}

        sEXE.ReleaseBuffer();
    }

	return false;
}

bool CThreatTracker::DoesFileExist(LPCTSTR pszFile) throw()
{
	// Make a copy, and remove any quotes and/or params
	CString sFile = pszFile;
	CString sJustFile;
    const WCHAR szAppExtension[] = L".EXE";

    sFile.MakeUpper();

    // look for the exe extension
	int nEnd = sFile.Find(szAppExtension) + (int)wcslen(szAppExtension);

    WCHAR ch = sFile.GetAt(nEnd);

    // look for the first set of quotes
    int nStart = sFile.Find(L'"');

    if(nStart == -1 || nStart > nEnd)
    {
        // error or quotes are past extension
        nStart = 0;
    }
    else
    {
        // increment the start position 
        nStart++;
        // decrement the end
        nEnd--;
    }

    sJustFile = sFile.Mid(nStart, nEnd);

	// Check if it exists
	DWORD dwAttrib = GetFileAttributes(sJustFile);
	if(dwAttrib == (DWORD)-1)
		return false;

	return true;
}