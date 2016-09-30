// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// During restore, we should have a list of each item restored and it's state...

#include <SymHTML.h>
#include <SymHTMLDocumentImpl.h>
#include <SymHTMLDialogDocumentImpl.h>
#include <resource.h>
#include <AVInterfaces.h>
#include "ThreatTracker.h"

// AvProdServce reboot suppression
#include <AvProdSvcClientLoader.h>

class CProcessSingleThreatDlg :
    public symhtml::CSymHTMLDocumentImpl,
    public symhtml::CSymHTMLDialogDocumentImpl,
    public AVModule::IAVThreatProcessingSink,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{

public:
    DECLARE_HTML_DOCUMENT(L"PROCESSSINGLETHREAT.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
        SYM_INTERFACE_ENTRY(AVModule::IID_AVThreatProcessingSink, AVModule::IAVThreatProcessingSink)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CProcessSingleThreatDlg)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CProcessSingleThreatDlg)
        HTML_EVENT_BUTTON_CLICK(L"remove", OnRemove)
        HTML_EVENT_BUTTON_CLICK(L"cancel", OnCancel)
        HTML_EVENT_BUTTON_CLICK(L"finish", OnFinish)
    END_HTML_EVENT_MAP()

public:
    CProcessSingleThreatDlg(AVModule::IAVThreatInfo* pThreatInfo, const GUID& idThreat) :
        m_spThreatInfo(pThreatInfo),
        m_bSucceeded(false),
        m_idThreat(idThreat),
        m_dwTotalItems(0),
        m_hIcon(NULL),
        m_bExclude(true),
        m_bRebootRequired(false),
        m_dwTotalRemediationActions(0),
        m_bAllowClose(true)
    {


    };

    virtual ~CProcessSingleThreatDlg(void)
    {
        CCTRCTXI0(L"dtor");

        if(m_hIcon)
            DestroyIcon(m_hIcon);

        if(m_spAvProdSvcClient)
        {
            HRESULT hr = m_spAvProdSvcClient->EnableRebootDialog(m_spAvProdSvcClientCookie, false, m_bRebootRequired);
            LOG_FAILURE(L"EnableRebootDialog() failed.", hr);
        }

        TRACE_REF_COUNT(m_spAvProdSvcClientCookie);
        TRACE_REF_COUNT(m_spAvProdSvcClient);
        TRACE_REF_COUNT(m_spThreatInfo);
    };

    // Event handlers
    virtual HRESULT OnDocumentComplete() throw()
    {
        // Call the base OnDocumentComplete
        HRESULT hr = symhtml::CSymHTMLDocumentImpl::OnDocumentComplete();
        LOG_FAILURE_AND_RETURN(L"CSymHTMLDocumentImpl::OnDocumentComplete() failed.", hr);

        ENTER_SCOPED_SECTION()
        {
            //
            // It is not critical that this object is available
            //  don't bother returning an error if this process fails.

            // Create the reboot suppression object
            m_AvProdSvcClientLoader.CreateObject(m_spAvProdSvcClient);
            if(!m_spAvProdSvcClient) // non-fatal
                LOG_FAILURE_AND_BREAK(L"Unable to create AvProdSvcClient", E_OUTOFMEMORY);

            // Initialize the reboot suppression object
            hr = m_spAvProdSvcClient->Initialize();
            LOG_FAILURE_AND_BREAK(L"Unable to initialize AvProdSvcClient", hr);

            // Suppress reboot dialog
            hr = m_spAvProdSvcClient->DisableRebootDialog(&m_spAvProdSvcClientCookie);
            LOG_FAILURE_AND_BREAK(L"Unable to disable AvProdSvcClient reboot dialog", hr);
        }
        LEAVE_SCOPED_SECTION();

        // Get/Set our title
        CStringW sTitle;
        sTitle.LoadString(IDS_TITLE_TPROC);
        hr = SetWindowText(sTitle);
        LOG_FAILURE_AND_RETURN(L"Unable to set window text.", hr);

        // Load our icons
        CISVersion::GetProductIcon(m_hIcon);
        hr = SetIcon(m_hIcon, false);
        LOG_FAILURE_AND_RETURN(L"Unable to set window icon.", hr);
        
        // Get this threat
        AVModule::IAVMapDwordDataPtr spThreatMap;
        hr = m_spThreatInfo->GetSingleThreat(m_idThreat, spThreatMap);
        LOG_FAILURE_AND_RETURN(L"Failed to get threatmap", hr);

        // Refresh threat details
        hr = UpdateDetails(spThreatMap);
        LOG_FAILURE_AND_RETURN(L"Failed to update details", hr);
        
        return S_OK;
    };

    virtual HRESULT UpdateDetails(AVModule::IAVMapDwordData* pThreatMap)
    {
        HRESULT hr;
        // Create a threat tracker...
        bool bIsViral = false;
        bool bIsManualQuarantine = false;
        CSymPtr<CThreatTracker> spThreatTracker(new CThreatTracker(m_spThreatInfo, false));
        if(!spThreatTracker)
            LOG_FAILURE_AND_RETURN(L"Failed to update details", E_OUTOFMEMORY);
        // Add this threat to the threat tracker
        if(!spThreatTracker->AddThreat(pThreatMap))
            LOG_FAILURE_AND_RETURN(L"CThreatTracker::AddThreat() failed", E_UNEXPECTED);

        // Get the display items
        CStringW sTmp;
        sTmp = spThreatTracker->GenerateOutputForItem(L"%{NAME}%", 0);
        if(!sTmp.IsEmpty())
        {
            hr = SetElementText("namefield", sTmp, -1, TRUE);
            LOG_FAILURE(L"Failed to set threat name", hr);
        }

        sTmp = spThreatTracker->GenerateOutputForItem(L"%{TCATS}%", 0);
        if(!sTmp.IsEmpty())
        {
            hr = SetElementText("catfield", sTmp, -1, TRUE);
            LOG_FAILURE(L"Failed to set threat cats", hr);
        }

        sTmp = spThreatTracker->GenerateOutputForItem(L"%{TLEVEL}% ", 0);
        if(!sTmp.IsEmpty())
        {
            hr = SetElementText("tlevelfieldText", sTmp, -1, TRUE);
            LOG_FAILURE(L"Failed to set threat level", hr);
        }

        sTmp = spThreatTracker->GenerateOutputForItem(L"%{STATE}%", 0);
        if(!sTmp.IsEmpty())
        {
            hr = SetElementText("tstatefield", sTmp, -1, TRUE);
            LOG_FAILURE(L"Failed to set threat state", hr);
        }

        // Overall threat level
        AVModule::ThreatTracking::eThreatLevels eLevel;
        hr = pThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
        if(SUCCEEDED(hr))
        {
            hr = UpdateTLevelGraphic(eLevel, "tlevelimage");
            LOG_FAILURE(L"Failed update tlevel image", hr);
        }
        else
        {
            LOG_FAILURE(L"Failed to get overall tlevel", hr);
        }

        spThreatTracker->CleanupCircularRefs();

        return hr;
    };

    HRESULT OnRemove(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        HRESULT hr;

        ENTER_SCOPED_SECTION() 
        {
            // Create the action data object
            AVModule::IAVMapDwordDataPtr spActionData;
            m_AVMapDwordDataLoader.CreateObject(spActionData);
            if(!spActionData)
                LOG_FAILURE_AND_BREAK(L"Failed to set the run detect anomalies flag", E_OUTOFMEMORY);

            // We want to get progress
            hr = spActionData->SetValue(AVModule::ThreatProcessor::FixIn_ProvideProgress, true);
            LOG_FAILURE_AND_BREAK(L"Failed to set the run provide progress flag", hr);

            // We want to re-detect this threat
            hr = spActionData->SetValue(AVModule::ThreatProcessor::FixIn_RunDetectAnomalies, true);
            LOG_FAILURE_AND_BREAK(L"Failed to set the run detect anomalies flag", hr);

            // Allow process terminations
            hr = spActionData->SetValue(AVModule::ThreatProcessor::FixIn_AllowProcTerm, true);
            LOG_FAILURE_AND_BREAK(L"Failed to set the allow proc term flag", hr);

            // Prevent the oncomplete call until we are done
            ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);

            // Register ourselves as a processing sink
            hr = m_spThreatInfo->RegisterThreatProcessingSink(this);
            LOG_FAILURE_AND_BREAK(L"Unable to register threat processing sink", hr);

            // Start the restore from quarantine
            hr = m_spThreatInfo->Process(AVModule::ThreatProcessor::ProcessingAction_Fix, m_idThreat, spActionData);
            LOG_FAILURE_AND_BREAK(L"Process() failed.", hr);

            // Disable the close button
            DisableClose(true);
            
            CStringW sFmt;
            sFmt.LoadString(IDS_TPROC_STARTING);
            CStringW sErr;
            sErr.Format(sFmt, hr);
            SetElementText("statustext", sErr, -1, TRUE);
            
            // Switch to pending mode
            SwitchToStatusMode(false);

            hr = S_OK;
        } 
        LEAVE_SCOPED_SECTION()

        if(FAILED(hr))
        {
            // Change the UI to reflect failure
            SwitchToStatusMode(true);

            CStringW sFmt;
            sFmt.LoadString(IDS_TPROC_FAILURE);
            CStringW sErr;
            sErr.Format(sFmt, hr);
            SetElementText("statustext", sErr, -1, TRUE);
        }

        return S_OK;
    };

    HRESULT OnCancel(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDOK);
        return S_OK;
    };

    HRESULT OnFinish(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDOK);
        return S_OK;
    };

    // IAVThreatProcessingSink
    virtual void OnThreatProcessingProgress(DWORD dwRemainingItems, AVModule::IAVMapDwordData* pCurItemResults, AVModule::IAVMapDwordData* pRetryParams) throw()
    {
        // Lock
        ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);
        
        HRESULT hr;

        ENTER_SCOPED_SECTION() 
        {
            static DWORD dwRedetectProgress = 0;
            if(0 == dwRedetectProgress)
            {
                hr = pCurItemResults->GetValue(AVModule::ThreatProcessor::FixProgressIn_RedetectionProgress, dwRedetectProgress);
            }
            
            // Redect is in progress, nothing to do
            if(0 == dwRedetectProgress)
                break;

            // Update the progress control
            symhtml::ISymHTMLElementPtr spProg;
            hr = GetElement("progress", &spProg);
            if(FAILED(hr) || !spProg)
                LOG_FAILURE_AND_BREAK(L"Failed to set the allow proc term flag", E_UNEXPECTED);

            CString cszValue;
            if(dwRemainingItems > m_dwTotalRemediationActions)
            {
                m_dwTotalRemediationActions = dwRemainingItems;
                
                cszValue.Format(L"%d", m_dwTotalRemediationActions);
                spProg->SetAttributeByName("maxvalue", cszValue);

            }
        
            // Update the current value to 1
            cszValue.Format(L"%d", m_dwTotalRemediationActions - dwRemainingItems);
            spProg->SetAttributeByName("value", cszValue);

            // Redraw our progress
            spProg->UpdateElement(TRUE);
        } 
        LEAVE_SCOPED_SECTION()
        
        return;
    };

    virtual void OnThreatProcessingComplete(AVModule::IAVMapDwordData* pProcessingResults) throw()
    {
        // Lock
        ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);

        HRESULT hr;
        
        // Enable the close button
        DisableClose(false);

        // Update the progress control
        symhtml::ISymHTMLElementPtr spProg;
        hr = GetElement("progress", &spProg);
        if(SUCCEEDED(hr))
        {
            CStringW sVal;

            // Update the current value to 1
            sVal.Format(L"%d", m_dwTotalRemediationActions);
            spProg->SetAttributeByName("value", sVal);

            // Redraw our progress
            spProg->UpdateElement(TRUE);
        }
        
        // Get this threat
        AVModule::IAVMapDwordDataPtr spThreatMap;
        hr = pProcessingResults->GetValue(AVModule::ThreatProcessor::ProcessOut_ThreatData, AVModule::AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spThreatMap));
        if(SUCCEEDED(hr) && spThreatMap)
        {
            hr = UpdateDetails(spThreatMap);
        }
        else
        {
            // Get this threat
            hr = m_spThreatInfo->GetSingleThreat(m_idThreat, spThreatMap);
            if(SUCCEEDED(hr) && spThreatMap)
            {
                hr = UpdateDetails(spThreatMap);
            }
        }

        // Enable the "close" button (any which way... we are done)
        SetElementEnabled("finish", true, TRUE);

        AVModule::ThreatProcessor::eRestoreResultCodes eRes;
        hr = pProcessingResults->GetValue(AVModule::ThreatProcessor::ProcessOut_ResultCode, (DWORD&)eRes);
        if(FAILED(hr))
        {
            CStringW sFmt;
            sFmt.LoadString(IDS_TPROC_FAILURE);
            CStringW sErr;
            sErr.Format(sFmt, hr);
            SetElementText("statustext", sErr, -1, TRUE);
            return;
        }

        // Assume generic failure.
        m_bSucceeded = false;
        DWORD dwStatusTextFmt = IDS_TPROC_FIXFAILED;

        // Override the assumption above as necessary.
        switch(eRes)
        {
            case AVModule::ThreatProcessor::FixResult_Succeeded:
                m_bSucceeded = true;
                dwStatusTextFmt = IDS_TPROC_FIXSUCCEEDED;
                break;
            case AVModule::ThreatProcessor::FixResult_PartiallySucceeded:
                m_bSucceeded = true;
                dwStatusTextFmt = IDS_TPROC_FIXPARTIALLYSUCCEEDED;
                break;
            case AVModule::ThreatProcessor::FixResult_PreviouslyRemoved:
                m_bSucceeded = true;
                dwStatusTextFmt = IDS_TPROC_FIXPREVIOUSLYREMOVED;
                break;
            case AVModule::ThreatProcessor::FixResult_RequiresProcTerm:
                dwStatusTextFmt = IDS_TPROC_FIXREQUIRESPROCTERM;
                break;
            case AVModule::ThreatProcessor::FixResult_ThreatNoLongerFound:
                m_bSucceeded = true;
                dwStatusTextFmt = IDS_TPROC_FIXTHREATNOLONGERFOUND;
                break;
            case AVModule::ThreatProcessor::FixResult_BackupFailed:
                dwStatusTextFmt = IDS_TPROC_FIXBACKUPFAILED;
                break;
            case AVModule::ThreatProcessor::FixResult_InvalidThreatID:
                dwStatusTextFmt = IDS_TPROC_FIXINVALIDTHREATID;
                break;
            case AVModule::ThreatProcessor::FixResult_InvalidTheatType:
                dwStatusTextFmt = IDS_TPROC_FIXINVALIDTHEATTYPE;
                break;
            case AVModule::ThreatProcessor::FixResult_InvalidAnomalyBackup:
                dwStatusTextFmt = IDS_TPROC_FIXINVALIDANOMALYBACKUP;
                break;
            case AVModule::ThreatProcessor::FixResult_FailedToLoadIScannerw:
                dwStatusTextFmt = IDS_TPROC_FIXFAILEDTOLOADISCANNERW;
                break;
            case AVModule::ThreatProcessor::FixResult_FailedToLoadIEraser4:
                dwStatusTextFmt = IDS_TPROC_FIXFAILEDTOLOADIERASER4;
                break;
            case AVModule::ThreatProcessor::FixResult_FailedToDetect_ButInitiallyDetectedUnderImpersonation:
                m_bSucceeded = false;
                dwStatusTextFmt = IDS_TPROC_FIXFAILEDTOREDETECTWOIMPERSONATION;
                break;
            case AVModule::ThreatProcessor::FixResult_CannotRemediate:
                dwStatusTextFmt = IDS_TPROC_FIXCANNOTREMEDIATE;
                break;
            case AVModule::ThreatProcessor::FixResult_DoNotDelete:
                dwStatusTextFmt = IDS_TPROC_FIXDONOTDELETE;
                break;
        }

        // Tell the user how we fared...
        CStringW sFmt;
        sFmt.LoadString(dwStatusTextFmt);
        CStringW sMsg;
        sMsg.Format(sFmt, eRes);
        SetElementText("statustext", sMsg, -1, TRUE);

        if(spThreatMap)
        {
            DWORD dwRebootRequired = 0;
            spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_REBOOTREQUIRED, dwRebootRequired);
            m_bRebootRequired = dwRebootRequired != 0;
        }
    
        // Unregister our sink
        m_spThreatInfo->UnRegisterThreatProcessingSink();

    };

    // Member access
    bool GetSucceeded()
    {
        return m_bSucceeded;
    };

protected:
    HRESULT SwitchToStatusMode(bool bEnableFinish)
    {
        HRESULT hr;
        hr = SetElementHidden("remove", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide remove", hr);
        hr = SetElementHidden("cancel", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide cancel", hr);
        hr = SetElementHidden("resolveRow", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide action list", hr);
        hr = SetElementHidden("prompt", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide prompt", hr);

        hr = SetElementHidden("spacerRow", true, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to show spacer row", hr);
        hr = SetElementHidden("statusRow", true, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to show status row", hr);
        hr = SetElementHidden("progressRow", true, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to show progress row", hr);
        hr = SetElementHidden("finish", true, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to show finish", hr);
        hr = SetElementEnabled("finish", bEnableFinish, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to enable/disable finish", hr);

        return S_OK;
    };

    virtual HRESULT GetCBSelection(symhtml::ISymHTMLElement* lpElement, int& nValue)
    {
        CCTRACEI(CCTRCTX L"Enter");
        HRESULT hr = S_OK;

        if(!lpElement)
        {
            CCTRACEE(CCTRCTX L"Element pointer NULL");
            return E_POINTER;
        }

        cc::IStringPtr pString;

        symhtml::ISymHTMLElementPtr pChildElement;
        hr = lpElement->FindFirstElement("caption", &pChildElement);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Could not get caption element");
            return hr;
        }

        hr = pChildElement->GetElementInnerText(&pString);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Could not get caption text");
            return hr;
        }

        hr = lpElement->FindFirstElement("option:checked", &pChildElement);
        if(FAILED(hr) || S_FALSE == hr)
        {
            CCTRACEE(CCTRCTX L"Could not get option element");
            return hr;
        }

        LPCWSTR szValue = NULL;
        hr = pChildElement->GetAttributeByName("value", &szValue);
        if(FAILED(hr) || S_FALSE == hr)
        {
            CCTRACEE(CCTRCTX L"Could not get value attribute");
            return hr;
        }

        nValue = _wtoi(szValue);

        return hr;
    };

    HRESULT SetElementHidden(LPCSTR szID, BOOL bDisplay, BOOL bRenderNow = FALSE)
    {
        HRESULT hr = S_OK;

        symhtml::ISymHTMLElementPtr spElement;
        hr = GetElement(szID, &spElement);
        if(FAILED(hr))
            return hr;

        LPCWSTR szwValue = bDisplay ? NULL : L"1";
        hr = spElement->SetAttributeByName("hide", szwValue);

        if(SUCCEEDED(hr))
            spElement->UpdateElement(bRenderNow);

        return hr;
    };

    HRESULT UpdateTLevelGraphic(AVModule::ThreatTracking::eThreatLevels eLevel, LPCSTR pszElem)
    {
        CStringW sGraphic;
        sGraphic.Format(L"damage_%d.gif", ((DWORD)eLevel) + 1);

        symhtml::ISymHTMLElementPtr spImg;
        HRESULT hr = GetElement(pszElem, &spImg);
        if(FAILED(hr))
            return hr;

        return spImg->SetAttributeByName("src", sGraphic);
    };
    
    HRESULT DisableClose(bool bDisabled)
    {
        m_bAllowClose = !bDisabled;
        return CSymHTMLDialogDocumentImpl::DisableClose(bDisabled);
    };
    
    HRESULT OnClose() throw()
    {
        return m_bAllowClose ? S_OK : S_FALSE;
    }

protected:
    cc::IStringPtr m_spAvProdSvcClientCookie;
    AvProdSvc::AvProdSvcClient_Loader m_AvProdSvcClientLoader;
    AvProdSvc::IAvProdSvcClientPtr m_spAvProdSvcClient;

    AVModule::AVLoader_IAVMapDwordData m_AVMapDwordDataLoader;
    AVModule::IAVThreatInfoPtr m_spThreatInfo;

    CStringW m_sRestoreToPath;
    GUID m_idThreat;
    bool m_bSucceeded;
    bool m_bRebootRequired;
    ccLib::CCriticalSection m_cs;
    DWORD m_dwTotalItems;
    HICON m_hIcon;
    bool m_bExclude;

    DWORD m_dwTotalRemediationActions;

    bool m_bAllowClose;
};
