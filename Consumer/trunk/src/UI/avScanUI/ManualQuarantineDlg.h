////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymHTML.h>
#include <SymHTMLDocumentImpl.h>
#include <SymHTMLDialogDocumentImpl.h>
#include <resource.h>
#include <AVInterfaces.h>
#include "GetFileName.h"

class CManualQuarantineDlg :
    public symhtml::CSymHTMLDocumentImpl,
    public symhtml::CSymHTMLDialogDocumentImpl,
    public AVModule::IAVThreatProcessingSink,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{

public:
    DECLARE_HTML_DOCUMENT(L"MANUALQUARANTINE.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
        SYM_INTERFACE_ENTRY(AVModule::IID_AVThreatProcessingSink, AVModule::IAVThreatProcessingSink)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CManualQuarantineDlg)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CManualQuarantineDlg)
        HTML_EVENT_BUTTON_CLICK(L"add", OnAdd)
        HTML_EVENT_BUTTON_CLICK(L"cancel", OnCancel)
        HTML_EVENT_BUTTON_CLICK(L"close", OnClose)
        HTML_EVENT_BUTTON_CLICK(L"browse", OnBrowse)
        HTML_EVENT_BUTTON_STATE_CHANGE(L"remove", OnRemove)
    END_HTML_EVENT_MAP()

public:
    CManualQuarantineDlg(AVModule::IAVThreatInfo* pThreatInfo) :
        m_bRemove(false),
        m_spThreatInfo(pThreatInfo),
        m_bSucceeded(false),
        m_hIcon(NULL),
        m_bAllowClose(true)
    {
        ZeroMemory(&m_idItem, sizeof(GUID));

        // Register ourselves as a processing sink
        m_spThreatInfo->RegisterThreatProcessingSink(this);

        // Init the loader
        SYMRESULT symRes = m_ldrAVMapDwordData.Initialize();
        if(SYM_FAILED(symRes))
        {
            SYM_LOG_FAILURE(L"Failed to init the IAVMapDwordData loader", symRes);
        }
    }
    virtual ~CManualQuarantineDlg(void)
    {
        CCTRCTXI0(L"dtor");

        // Unregister our sink
        m_spThreatInfo->UnRegisterThreatProcessingSink();

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
        sTitle.LoadString(IDS_TITLE_MQUARANTINE);
        hr = SetWindowText(sTitle);

        // Load our icons
        CISVersion::GetProductIcon(m_hIcon);
        hr = SetIcon(m_hIcon, false);
        
        return hr;
    }

    HRESULT OnAdd(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Get the input
        cc::IStringPtr spDescription;
        cc::IStringPtr spFileName;
        GetElementText("desc", &spDescription);
        GetElementText("filename", &spFileName);

        // Make sure they entered a desc and filename
        HWND hWnd;
        GetDocHwnd(hWnd);
        if(spDescription->GetLength() == 0)
        {
            // Display the message
            CString sMsg;
            sMsg.LoadString(IDS_DESC_MISSING);
            MessageBox(hWnd, sMsg, CISVersion::GetProductName(), MB_OK | MB_ICONERROR);

            return S_OK;
        }

        if(spFileName->GetLength() == 0)
        {
            // Display the message
            CString sMsg;
            sMsg.LoadString(IDS_PATH_MISSING);
            MessageBox(hWnd, sMsg, CISVersion::GetProductName(), MB_OK | MB_ICONERROR);

            return S_OK;
        }

        // Turn off errors
        UINT nOldErrMode = SetErrorMode(SEM_FAILCRITICALERRORS);

        // Get flags for the volume
        DWORD dwFlags = 0;
        bool bGVIFailed = true;
        UINT nDriveType = DRIVE_UNKNOWN;
        wchar_t szVolume[256];
        if(GetVolumePathName(spFileName->GetStringW(), szVolume, 256))
        {
            wchar_t szVName[256];
            wchar_t szFSName[256];
            DWORD dwMaxCompLen;
            if(GetVolumeInformation(szVolume, szVName, 255, NULL, &dwMaxCompLen, &dwFlags, szFSName, 255))
            {
                bGVIFailed = false;
            }

            nDriveType = GetDriveType(szVolume);
        }

        // Validate the filename
        DWORD dwBadFlags = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_OFFLINE;
        DWORD dwAttrib = GetFileAttributesW(spFileName->GetStringW());
        if(bGVIFailed || dwAttrib == -1 || (dwAttrib & dwBadFlags) != 0)
        {           
            // Display the message
            CString sMsg;
            sMsg.LoadString(IDS_INVALID_PATH);
            MessageBox(hWnd, sMsg, CISVersion::GetProductName(), MB_OK | MB_ICONERROR);

            return S_OK;
        }

        // Check if the file is on a read-only volume
        if(dwFlags & FILE_READ_ONLY_VOLUME)
        {
            // Display the message
            CString sMsg;
            sMsg.LoadString(IDS_INVALID_READONLY);
            MessageBox(hWnd, sMsg, CISVersion::GetProductName(), MB_OK | MB_ICONERROR);

            return S_OK;
        }

        // Check if this is a network volume
        if(nDriveType == DRIVE_REMOTE)
        {
            // Display the message
            CString sMsg;
            sMsg.LoadString(IDS_INVALID_NETWORK);
            MessageBox(hWnd, sMsg, CISVersion::GetProductName(), MB_OK | MB_ICONERROR);

            return S_OK;
        }

        // Restore the error mode
        SetErrorMode(nOldErrMode);

        // Create the action data object
        AVModule::IAVMapDwordDataPtr spActionData;
        SYMRESULT symRes = m_ldrAVMapDwordData.CreateObject(spActionData);
        if(SYM_FAILED(symRes))
        {
#pragma  TODO("CED")
            SYM_LOG_FAILURE(L"Failed to create an IAVMapDwordData object", symRes);
            return S_OK;
        }

        // Fill in the action data
        HRESULT hr = spActionData->SetValue(AVModule::ThreatProcessor::QuarIn_FileToQuarantine, spFileName->GetStringW());
        LOG_FAILURE_AND_RETURN(L"Failed to set the file being quarantined", hr);
        hr = spActionData->SetValue(AVModule::ThreatProcessor::QuarIn_Remove, m_bRemove);
        LOG_FAILURE_AND_RETURN(L"Failed to set the remove flag", hr);
        hr = spActionData->SetValue(AVModule::ThreatProcessor::QuarIn_SpecifyThreatName, spDescription->GetStringW());
        LOG_FAILURE_AND_RETURN(L"Failed to set the \"threat name\"", hr);

        // Prevent the oncomplete call until we are done
        ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);

        // Add the file to quarantine
        GUID idUnused;
        hr = m_spThreatInfo->Process(AVModule::ThreatProcessor::ProcessingAction_ManualQuarantine, idUnused, spActionData);
        if(FAILED(hr))
        {
            // Change the UI to reflect failure
            SwitchToStatusMode(true);

            CStringW sFmt;
            sFmt.LoadString(IDS_QBADD_FAILURE);
            CStringW sErr;
            sErr.Format(sFmt, hr);
            SetElementText("statustext", sErr, -1, TRUE);
            return S_OK;
        }

        // Disable the close button
        DisableClose(true);

        // Switch to pending mode
        SwitchToStatusMode(false);

        return S_OK;
    }

    HRESULT OnCancel(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDCANCEL);
        return S_OK;
    }

    HRESULT OnClose(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDOK);
        return S_OK;
    }

    HRESULT OnBrowse(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Get our HWND
        HWND hWnd;
        GetDocHwnd(hWnd);

        // Load our two strings for the save dialog
        CStringW sTitle, sFilter;
        sTitle.LoadString(IDS_QUAR_TITLE);
        sFilter.LoadString(IDS_QUAR_FILTER);

	    // Since we can't save a string with '\0' embedded we use the hash (#)
	    // as a place holder and replace it with the nils.
        sFilter.Replace(L'#', L'\0');

        // Have the user pick the export location/file
        CStringW sBuffer;
        CGetFileName gfn;
        gfn.hwndOwner = hWnd;
        gfn.lpstrFilter = sFilter;
        gfn.lpstrFile = sBuffer.GetBuffer(8192);
        gfn.nMaxFile = 8192;
        gfn.lpstrTitle = sTitle;
        gfn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

        BOOL bRes = gfn.Open(true);
        sBuffer.ReleaseBuffer();
        if(bRes)
        {
            // Replace the filename text
            SetElementText("filename", sBuffer, -1, TRUE);
        }

        return S_OK;
    }

    HRESULT OnRemove(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        m_bRemove = !m_bRemove;
        return S_OK;
    }

    // IAVThreatProcessingSink
    virtual void OnThreatProcessingProgress(DWORD dwRemainingItems, AVModule::IAVMapDwordData* pCurItemResults, AVModule::IAVMapDwordData* pRetryParams) throw()
    {
        // This should never be called when adding to quarantine
    }

    virtual void OnThreatProcessingComplete(AVModule::IAVMapDwordData* pProcessingResults) throw()
    {
        // Wait until OnAdd is done
        ccLib::CSingleLock csl(&m_cs, INFINITE, FALSE);

        // Enable the close button
        DisableClose(false);

        // Enable the "close" button (any which way... we are done)
        SetElementEnabled("close", true, TRUE);

        AVModule::ThreatProcessor::eManualQuarantineResultCodes eRes;
        HRESULT hr = pProcessingResults->GetValue(AVModule::ThreatProcessor::ProcessOut_ResultCode, (DWORD&)eRes);
        if(FAILED(hr))
        {
            CStringW sFmt;
            sFmt.LoadString(IDS_QBADD_FAILURE);
            CStringW sErr;
            sErr.Format(sFmt, hr);
            SetElementText("statustext", sErr, -1, TRUE);
            return;
        }

        switch(eRes)
        {
            case AVModule::ThreatProcessor::ManualQuarResult_Succeeded:
            {
                // We succeeded
                m_bSucceeded = true;

                // Get the new GUID
                pProcessingResults->GetValue(AVModule::ThreatProcessor::ProcessOut_ThreatID, m_idItem);

                // Tell the user we succeeded...
                CStringW sMsg;
                sMsg.LoadString(IDS_QBADD_SUCCEESS);
                SetElementText("statustext", sMsg, -1, TRUE);
                break;
            }
            case AVModule::ThreatProcessor::ManualQuarResult_Failed:
            case AVModule::ThreatProcessor::ManualQuarResult_FailedToLoadEraser:
            case AVModule::ThreatProcessor::ManualQuarResult_NoFileProvided:
            case AVModule::ThreatProcessor::ManualQuarResult_GenerateAnomalyFailed:
            case AVModule::ThreatProcessor::ManualQuarResult_NoItemsFound:
            case AVModule::ThreatProcessor::ManualQuarResult_AnomalyBackupFailed:
            {
                // We failed
                m_bSucceeded = false;

                // Tell the user we failed...
                CStringW sFmt;
                sFmt.LoadString(IDS_QBADD_POSTFAIL);
                CStringW sMsg;
                sMsg.Format(sFmt, eRes);
                SetElementText("statustext", sMsg, -1, TRUE);
                break;
            }
        }
    }

    // Member access
    bool GetSucceeded()
    {
        return m_bSucceeded;
    }

    void GetItemGUID(GUID& idNewItem)
    {
        memcpy(&idNewItem, &m_idItem, sizeof(GUID));
    }

protected:
    HRESULT SwitchToStatusMode(bool bEnableClose)
    {
        HRESULT hr = SetElementHidden("addform", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide addform", hr);
        hr = SetElementHidden("add", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide add", hr);
        hr = SetElementHidden("cancel", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide cancel", hr);
        hr = SetElementHidden("statusform", true, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to show statusform", hr);
        hr = SetElementHidden("close", true, FALSE);
        LOG_FAILURE_AND_RETURN(L"Failed to show close", hr);
        hr = SetElementEnabled("close", bEnableClose, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to enable/disable close", hr);

        // No progress... for now
        hr = SetElementHidden("statusprog", false, TRUE);
        LOG_FAILURE_AND_RETURN(L"Failed to hide statusprog", hr);

        return S_OK;
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
    bool m_bRemove;
    AVModule::IAVThreatInfoPtr m_spThreatInfo;
    AVModule::AVLoader_IAVMapDwordData m_ldrAVMapDwordData;
    GUID m_idItem;
    bool m_bSucceeded;
    ccLib::CCriticalSection m_cs;
    HICON m_hIcon;
    bool m_bAllowClose;
};
