////////////////////////
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
#include <isSymTheme.h>

class CQRestoreDlg :
    public symhtml::CSymHTMLDocumentImpl,
    public symhtml::CSymHTMLDialogDocumentImpl,
    public AVModule::IAVThreatProcessingSink,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{

public:
    DECLARE_HTML_DOCUMENT(L"QUARANTINERESTORE.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
        SYM_INTERFACE_ENTRY(AVModule::IID_AVThreatProcessingSink, AVModule::IAVThreatProcessingSink)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CQRestoreDlg)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CQRestoreDlg)
        HTML_EVENT_BUTTON_CLICK(L"yes", OnYes)
        HTML_EVENT_BUTTON_CLICK(L"no", OnNo)
        HTML_EVENT_BUTTON_CLICK(L"close", OnClose)
        HTML_EVENT_BUTTON_STATE_CHANGE(L"exclude", OnExclude)
    END_HTML_EVENT_MAP()

public:
    CQRestoreDlg(AVModule::IAVThreatInfo* pThreatInfo, const GUID& idThreat) :
        m_spThreatInfo(pThreatInfo),
        m_bSucceeded(false),
        m_idThreat(idThreat),
        m_dwTotalItems(0),
        m_hIcon(NULL),
        m_bExclude(true),
        m_bAllowClose(true)
    {
        // Init the loader
        SYMRESULT symRes = m_ldrAVMapDwordData.Initialize();
        if(SYM_FAILED(symRes))
        {
            SYM_LOG_FAILURE(L"Failed to init the IAVMapDwordData loader", symRes);
        }
    }
    virtual ~CQRestoreDlg(void)
    {   
        CCTRCTXI0(L"dtor");

        if(m_hIcon)
            DestroyIcon(m_hIcon);

        TRACE_REF_COUNT(m_spThreatInfo);
    }

    // Event handlers
    virtual HRESULT OnDocumentComplete() throw()
    {
        // Call the base OnDocumentComplete
        HRESULT hr = symhtml::CSymHTMLDocumentImpl::OnDocumentComplete();
        if(FAILED(hr))
            return hr;

        // Get/Set our title
        CStringW sTitle;
        sTitle.LoadString(IDS_TITLE_QRESTORE);
        hr = SetWindowText(sTitle);

        // Load our icons
        CISVersion::GetProductIcon(m_hIcon);
        hr = SetIcon(m_hIcon, false);

        // Create a threat tracker...
        bool bIsViral = false;
        bool bIsManualQuarantine = false;
        CSymPtr<CThreatTracker> spThreatTracker(new CThreatTracker(m_spThreatInfo, false));
        if(spThreatTracker)
        {
            // Get this threat
            AVModule::IAVMapDwordDataPtr spThreatMap;
            hr = m_spThreatInfo->GetSingleThreat(m_idThreat, spThreatMap);
            LOG_FAILURE_AND_RETURN(L"Failed to get threatmap", hr);

            // Check if the threat is viral
            hr = spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_HASVIRALCATEGORY, bIsViral);
            LOG_FAILURE(L"Failed to get viral flag", hr);

            // Was this a manual quarantine item?
            AVModule::ThreatTracking::eScannerTypes eDetectedBy = AVModule::ThreatTracking::ScannerTypeManualScanner;
            hr = spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_SCANNERTYPES_DETECTED_BY, (DWORD&)eDetectedBy);
            LOG_FAILURE(L"Failed to get SCANNERTYPES_DETECTED_BY", hr);
            if(eDetectedBy == AVModule::ThreatTracking::ScannerTypeManualQuarantine)
                bIsManualQuarantine = true;


            // Add this threat to the threat tracker
            if(spThreatTracker->AddThreat(spThreatMap))
            {
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

                sTmp = spThreatTracker->GenerateOutputForItem(L"%{TLEVEL}%", 0);
                if(!sTmp.IsEmpty())
                {
                    hr = SetElementText("tlevelfield", sTmp, -1, TRUE);
                    LOG_FAILURE(L"Failed to set threat level", hr);
                }
            }
        }

        // Check the exclude button
        symhtml::ISymHTMLElementPtr spExclude;
        if(SUCCEEDED(GetElement("exclude", &spExclude)) && spExclude)
        {
            // If the threat is viral or manual quarantine, uncheck/hide the "exclude" checkbox
            if(bIsViral || bIsManualQuarantine)
            {
                m_bExclude = false;
                spExclude->SetElementState(0, symhtml::state::checked, FALSE);
                SetElementDisplayed("exclude", FALSE, TRUE);
            }
            else
            {
                m_bExclude = true;
                spExclude->SetElementState(symhtml::state::checked, 0, TRUE);
            }
        }

        return hr;
    }

    HRESULT OnYes(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Create the action data object
        AVModule::IAVMapDwordDataPtr spActionData;
        SYMRESULT symRes = m_ldrAVMapDwordData.CreateObject(spActionData);
        if(SYM_FAILED(symRes))
        {
            SYM_LOG_FAILURE(L"Failed to create an IAVMapDwordData object", symRes);
#pragma  TODO("CED?")
            // Change the UI to reflect failure
            SwitchToStatusMode(true);

            CStringW sFmt;
            sFmt.LoadString(IDS_QBREST_FAILURE);
            CStringW sErr;
            sErr.Format(sFmt, symRes);
            SetElementText("statustext", sErr, -1, TRUE);
            return S_OK;
        }

        // We always want progress
        HRESULT hr = spActionData->SetValue(AVModule::ThreatProcessor::RestoreIn_ProvideProgress, true);
        LOG_FAILURE_AND_RETURN(L"Failed to set the progress flag", hr);

        // We want overwrite warnings
        hr = spActionData->SetValue(AVModule::ThreatProcessor::RestoreIn_CheckPresenceBeforeRestoring, true);
        LOG_FAILURE(L"Failed to set the check presence flag", hr);

        // Prevent the oncomplete call until we are done
        ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);

        // Register ourselves as a processing sink
        m_spThreatInfo->RegisterThreatProcessingSink(this);

        // If we are to exclude the item, do that before the restore
        if(m_bExclude)
        {
            AVModule::IAVMapDwordDataPtr spRet;
            hr = m_spThreatInfo->ProcessAndWait(AVModule::ThreatProcessor::ProcessingAction_Exclude, m_idThreat, NULL, spRet);
            LOG_FAILURE(L"Failed to exclude threat...", hr);

            if(spRet)
            {
                AVModule::ThreatProcessor::eExcludeResultCodes eRes;
                spRet->GetValue(AVModule::ThreatProcessor::ProcessOut_ResultCode, (DWORD&)eRes);

                if(eRes != AVModule::ThreatProcessor::ExcludeResult_Succeeded)
                {
                    CCTRCTXE1(L"Exclude failed, result=%d", eRes);
                }
            }
        }

        // Start the restore from quarantine
        hr = m_spThreatInfo->Process(AVModule::ThreatProcessor::ProcessingAction_Restore, m_idThreat, spActionData);
        if(FAILED(hr))
        {
            // Change the UI to reflect failure
            SwitchToStatusMode(true);

            CStringW sFmt;
            sFmt.LoadString(IDS_QBREST_FAILURE);
            CStringW sErr;
            sErr.Format(sFmt, hr);
            SetElementText("statustext", sErr, -1, TRUE);

            // Unregister our sink
            m_spThreatInfo->UnRegisterThreatProcessingSink();

            return S_OK;
        }

        // Disable the close button
        DisableClose(true);

        // Switch to pending mode
        SwitchToStatusMode(false);

        return S_OK;
    }

    HRESULT OnNo(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDNO);
        return S_OK;
    }

    HRESULT OnClose(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDOK);
        return S_OK;
    }

    HRESULT OnExclude(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        m_bExclude = !m_bExclude;
        return S_OK;
    }

    // IAVThreatProcessingSink
    virtual void OnThreatProcessingProgress(DWORD dwRemainingItems, AVModule::IAVMapDwordData* pCurItemResults, AVModule::IAVMapDwordData* pRetryParams) throw()
    {
        // Lock
        ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);

	    // Apply skin (we must do this every time since we can be called on random threads)
	    CISSymTheme isSymTheme;
	    HRESULT hr = isSymTheme.Initialize(GetModuleHandle(NULL));
	    if(FAILED(hr))
	    {
		    CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr);
	    }

        bool bPromptForPath = false;

        // Does this target already exist?
        bool bAlreadyExists = false;
        hr = pCurItemResults->GetValue(AVModule::ThreatProcessor::RestoreProgressIn_RemediationExists, bAlreadyExists);
        if(SUCCEEDED(hr) && bAlreadyExists)
        {
            // Get the remediation display data
            AVModule::IAVMapDwordDataPtr spDispData;
            hr = pCurItemResults->GetValue(AVModule::ThreatProcessor::RetoreProgressIn_RemDispData, AVModule::AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spDispData));
            LOG_FAILURE(L"Failed to get remediation display data", hr);
            if(SUCCEEDED(hr))
            {
                // Get the remediation type ... we only prompt for files
                ccEraser::eObjectType eType;
                hr = spDispData->GetValue(AVModule::ThreatTracking::RemDisp_RemediationType, (DWORD&)eType);
                LOG_FAILURE(L"Failed to get remediation type", hr);
                if(SUCCEEDED(hr) && (eType == ccEraser::FileRemediationActionType || eType == ccEraser::InfectionRemediationActionType))
                {
                    // Get the target
                    cc::IStringPtr sTarget;
                    hr = spDispData->GetValue(AVModule::ThreatTracking::RemDisp_Target, sTarget);
                    LOG_FAILURE(L"Failed to get remediation type", hr);
                    if(SUCCEEDED(hr))
                    {
                        // Get our HWND
                        HWND hWnd;
                        GetDocHwnd(hWnd);

                        // Prompt the user to overwrite
                        CString sTitle, sMsgFmt, sMsg;
                        sTitle.LoadString(IDS_TITLE_QRESTORE);
                        sMsgFmt.LoadString(IDS_EXISTS);
                        sMsg.Format(sMsgFmt, sTarget->GetStringW());

                        int nRes = MessageBox(hWnd, sMsg, sTitle, MB_YESNO | MB_ICONQUESTION);
                        if(nRes == IDYES)
                        {
                            // Just return to CAV and it will use the default eraser behavior here...
                            // Which for files is overwrite
                            return;
                        }
                        else
                        {
                            bPromptForPath = true;
                        }
                    }
                }
                else
                {
                    // Just return to CAV and it will use the default eraser behavior here...
                    return;
                }
            }
        }
     
        // Does this item require a path?
        ccEraser::eResult eRes = ccEraser::Fail;
        cc::IStringPtr spReqPath;
        hr = pCurItemResults->GetValue(AVModule::ThreatProcessor::RestoreProgressIn_RequiresPath, spReqPath);
        if(SUCCEEDED(hr))
        {
            bPromptForPath = true;
        }

        if(bPromptForPath)
        {
            // If we don't have a path to use... get one
            while(m_sRestoreToPath.IsEmpty())
            {
                // Ask the user for a path
                hr = BrowsePrompt();

                // If the user canceled the browse, ask them if they want to cancel
                if(hr == S_FALSE)
                {
                    // Get our HWND
                    HWND hWnd;
                    GetDocHwnd(hWnd);

                    // Load our strings
                    CStringW sCancelTitle, sCancelMsg;
                    sCancelMsg.LoadString(IDS_QBREST_CANCELMSG);
                    sCancelTitle.LoadString(IDS_QBREST_CANCELTITLE);

                    int nResult = MessageBox(hWnd, sCancelMsg, sCancelTitle, MB_YESNO | MB_ICONQUESTION);
                    if(nResult == IDNO)
                        continue;
                    else
                    {
                        // Cancel the restore...
                        pRetryParams->SetValue(AVModule::ThreatProcessor::RestoreProgressOut_AbortAndRedoPrevious, true);
                        break;
                    }
                }
                else if(FAILED(hr))
                {
                    // Tell the user we failed...
                    CStringW sFmt;
                    sFmt.LoadString(IDS_QBREST_FAILURE);
                    CStringW sMsg;
                    sMsg.Format(sFmt, hr);
                    SetElementText("statustext", sMsg, -1, TRUE);

                    // Cancel the restore...
                    pRetryParams->SetValue(AVModule::ThreatProcessor::RestoreProgressOut_AbortAndRedoPrevious, true);

                    break;
                }
            }

            // If we have a path, use it for a retry
            pRetryParams->SetValue(AVModule::ThreatProcessor::RestoreProgressOut_RetryWithPath, m_sRestoreToPath);

            // We are done
            return;
        }

        // Update the progress control
        symhtml::ISymHTMLElementPtr spProg;
        hr = GetElement("progress", &spProg);
        if(SUCCEEDED(hr))
        {
            CStringW sVal;

            // Do we have a new remaining count?
            if((dwRemainingItems + 1) > m_dwTotalItems)
            {
                m_dwTotalItems = dwRemainingItems + 1;
                sVal.Format(L"%d", m_dwTotalItems);
                spProg->SetAttributeByName("maxvalue", sVal);
            }

            // Update the current value
            sVal.Format(L"%d", m_dwTotalItems - dwRemainingItems);
            spProg->SetAttributeByName("value", sVal);

            // Redraw our progress
            spProg->UpdateElement(TRUE);
        }

        // What is the current result?
        hr = pCurItemResults->GetValue(AVModule::ThreatProcessor::RestoreProgressIn_eResult, (DWORD&)eRes);
        CStringW sStatus;
        switch(eRes)
        {
            case ccEraser::Success:
                sStatus.LoadString(IDS_ERASER_SUCCESS);
                break;
            case ccEraser::NothingToDo:
            case ccEraser::False:
                sStatus.LoadString(IDS_ERASER_NOTHINGTODO);
                break;
            case ccEraser::Abort:
                sStatus.LoadString(IDS_ERASER_ABORT);
                break;
            default:
            {
                CStringW sFmt;
                sFmt.LoadString(IDS_ERASER_FAILURE);
                sStatus.Format(sFmt, eRes);
                break;
            }
        }

        // Get the remediation display data...
        AVModule::IAVMapDwordDataPtr spDisplayData;
        hr = pCurItemResults->GetValue(AVModule::ThreatProcessor::RetoreProgressIn_RemDispData, AVModule::AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spDisplayData));
        LOG_FAILURE(L"Failed to get remediation action display data", hr);

        CStringW sTarget;

        if(spDisplayData)
        {
            // If this item doesn't support undo, say so
            DWORD dwSupportsUndo = 1;
            hr = spDisplayData->GetValue(AVModule::ThreatTracking::RemDisp_SupportsUndo, dwSupportsUndo);
            if(SUCCEEDED(hr) && dwSupportsUndo == 0 && eRes == ccEraser::False)
                sStatus.LoadString(IDS_UNDONOTSUPPORTED);

            // Get the target type
            ccEraser::eObjectType eType;
            hr = spDisplayData->GetValue(AVModule::ThreatTracking::RemDisp_RemediationType, (DWORD&)eType);
            LOG_FAILURE(L"Failed to get remediation type!", hr);

            // Get the target string
            cc::IStringPtr spTarget;
            hr = spDisplayData->GetValue(AVModule::ThreatTracking::RemDisp_Target, spTarget);
            LOG_FAILURE(L"Failed to get remediation target!", hr);
            if(spTarget)
                sTarget = spTarget->GetStringW();
            else
            {
                // Use the remediation type to display a generic string
                switch(eType)
                {
                    CASE_LOAD_STRING(ccEraser::RegistryRemediationActionType, sTarget, IDS_REM_REG);
                    CASE_LOAD_STRING(ccEraser::FileRemediationActionType, sTarget, IDS_REM_FILE);
                    CASE_LOAD_STRING(ccEraser::ProcessRemediationActionType, sTarget, IDS_REM_PROC);
                    CASE_LOAD_STRING(ccEraser::BatchRemediationActionType, sTarget, IDS_REM_BATCH);
                    CASE_LOAD_STRING(ccEraser::INIRemediationActionType, sTarget, IDS_REM_INI);
                    CASE_LOAD_STRING(ccEraser::ServiceRemediationActionType, sTarget, IDS_REM_SERV);
                    CASE_LOAD_STRING(ccEraser::COMRemediationActionType, sTarget, IDS_REM_COM);
                    CASE_LOAD_STRING(ccEraser::HostsRemediationActionType, sTarget, IDS_REM_HOST);
                    CASE_LOAD_STRING(ccEraser::LSPRemediationActionType, sTarget, IDS_REM_LSP);
                    CASE_LOAD_STRING(ccEraser::BrowserCacheRemediationActionType, sTarget, IDS_REM_CACHE);
                    CASE_LOAD_STRING(ccEraser::SystemRemediationActionType, sTarget, IDS_REM_SYS);
                    CASE_LOAD_STRING(ccEraser::CookieRemediationActionType, sTarget, IDS_REM_COOKIE);
                    default:
                        CCTRACEE(CCTRCTX L"Unknown remediation type! (%d)", eType);
                        ATLASSERT(FALSE);
                        break;
                }

                // If we loaded a string, remove the "%d" and then trim it
                if(!sTarget.IsEmpty())
                {
                    sTarget.Replace(L"%d", L"");
                    sTarget.Trim();
                }
            }
        }

        // Make sure we always display something...
        if(sTarget.IsEmpty())
            sTarget.LoadString(IDS_UNSUPPORTEDTYPE);

        // Build the <tr> for the list
        CStringW sFmt, sItemHTML;
        sFmt.LoadString(IDS_QBREST_ITEM);
        sItemHTML.Format(sFmt, sTarget.GetString(), sStatus.GetString());

        // Add this item to the list...
        AppendElementHtml("itemtable", sItemHTML, -1, TRUE);
    }

    virtual void OnThreatProcessingComplete(AVModule::IAVMapDwordData* pProcessingResults) throw()
    {
        // Lock
        ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);

        // Enable the close button
        DisableClose(false);

        // Enable the "close" button (any which way... we are done)
        SetElementEnabled("close", true, TRUE);

        // Unregister our sink
        m_spThreatInfo->UnRegisterThreatProcessingSink();

        AVModule::ThreatProcessor::eRestoreResultCodes eRes;
        HRESULT hr = pProcessingResults->GetValue(AVModule::ThreatProcessor::ProcessOut_ResultCode, (DWORD&)eRes);
        if(FAILED(hr))
        {
            CStringW sFmt;
            sFmt.LoadString(IDS_QBREST_FAILURE);
            CStringW sErr;
            sErr.Format(sFmt, hr);
            SetElementText("statustext", sErr, -1, TRUE);
            return;
        }

        switch(eRes)
        {
            case AVModule::ThreatProcessor::RestoreResult_Succeeded:
            case AVModule::ThreatProcessor::RestoreResult_PartialSuccess:
            {
                // We succeeded
                m_bSucceeded = true;

                // Tell the user we succeeded...
                CStringW sMsg;
                sMsg.LoadString(IDS_QBREST_SUCCEESS);
                SetElementText("statustext", sMsg, -1, TRUE);
                break;
            }
            case AVModule::ThreatProcessor::RestoreResult_Aborted:
            case AVModule::ThreatProcessor::RestoreResult_Failed:
            case AVModule::ThreatProcessor::RestoreResult_ThreatNotBackedUp:
            case AVModule::ThreatProcessor::RestoreResult_InvalidThreatID:
            case AVModule::ThreatProcessor::RestoreResult_InvalidThreatType:
            case AVModule::ThreatProcessor::RestoreResult_FailedToLoadccEraser:
            case AVModule::ThreatProcessor::RestoreResult_FailedToLoadAnomaly:
            case AVModule::ThreatProcessor::RestoreResult_FailedToLoadBackupData:
            case AVModule::ThreatProcessor::RestoreResult_FailedRequiresPath:
            {
                // We failed
                m_bSucceeded = false;

                // Tell the user we failed...
                CStringW sFmt;
                sFmt.LoadString(IDS_QBREST_POSTFAIL);
                CStringW sMsg;
                sMsg.Format(sFmt, eRes);
                SetElementText("statustext", sMsg, -1, TRUE);
                break;
            }
        }

        // Unregister our sink
        m_spThreatInfo->UnRegisterThreatProcessingSink();
    }

    // Member access
    bool GetSucceeded()
    {
        return m_bSucceeded;
    }

protected:
    HRESULT SwitchToStatusMode(bool bEnableClose)
    {
        HRESULT hr = SetElementHidden("restform", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide restform", hr);
        hr = SetElementHidden("yes", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide yes", hr);
        hr = SetElementHidden("no", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide no", hr);
        hr = SetElementHidden("prompt", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide prompt", hr);

        hr = SetElementHidden("statusform", true, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to show statusform", hr);
        hr = SetElementHidden("close", true, FALSE);
        LOG_FAILURE_AND_RETURN(L"Failed to show close", hr);
        hr = SetElementEnabled("close", bEnableClose, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to enable/disable close", hr);

        return S_OK;
    }

    HRESULT BrowsePrompt()
    {
        ccLib::CCoInitialize coInit;
        HRESULT hr = coInit.Initialize(ccLib::CCoInitialize::eSTAModel);
        if(SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE)
        {
            // Get our HWND
            HWND hWnd;
            GetDocHwnd(hWnd);

            // Get the global allocator
            CComPtr<IMalloc> spMalloc;
            hr = SHGetMalloc(&spMalloc);
            LOG_FAILURE_AND_RETURN(L"Failed to get the global allocator", hr);

            // Get the title
            CStringW sTitle;
            sTitle.LoadString(IDS_QBREST_BROWSETITLE);

            // Build the browse info struct
            CStringW sDisp;
            BROWSEINFO bi;
            ZeroMemory(&bi, sizeof(bi));
            bi.hwndOwner = hWnd;
            bi.lpszTitle = sTitle;
            bi.pszDisplayName = sDisp.GetBuffer(8192);
            bi.ulFlags = 0;//BIF_NEWDIALOGSTYLE;

            // Browse
            hr = E_FAIL;
            LPITEMIDLIST pList = SHBrowseForFolder(&bi);
            sDisp.ReleaseBuffer();
            if(pList)
            {
                // Convert the selected PIDL to a path
                CStringW sPath;
                SHGetPathFromIDList(pList, sPath.GetBuffer(8192));
                sPath.ReleaseBuffer();
                if(!sPath.IsEmpty())
                {
                    // Set the restore to path
                    m_sRestoreToPath = sPath;
                    hr = S_OK;
                }

                // Free the ITEMIDLIST
                spMalloc->Free(pList);
                pList = NULL;
            }
            else
                hr = S_FALSE;
        }

        return hr;
    }

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
    }

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
    CStringW m_sRestoreToPath;
    GUID m_idThreat;
    AVModule::IAVThreatInfoPtr m_spThreatInfo;
    AVModule::AVLoader_IAVMapDwordData m_ldrAVMapDwordData;
    bool m_bSucceeded;
    ccLib::CCriticalSection m_cs;
    DWORD m_dwTotalItems;
    HICON m_hIcon;
    bool m_bExclude;
    bool m_bAllowClose;
};
