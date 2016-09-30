// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ScanUIMisc.h"
#include <AvProdLoggingLoader.h>
#include "RiskDetailsDlg.h"
#include "ManualQuarantineDlg.h"
#include "QRestoreDlg.h"
#include <resource.h>
#include <ccAlertLoader.h>
#include <NAVSettingsHelperEx.h>
#include <AVInterfaceLoader.h>
#include <OptNames.h>

#include "ProcessSingleThreatDlg.h"

using namespace avScanUI;
using namespace AVModule;
using namespace AvProd;

CScanUIMisc::CScanUIMisc(void)
{
}

CScanUIMisc::~CScanUIMisc(void)
{
    CCTRCTXI0(L"dtor");
}

HRESULT CScanUIMisc::Initialize()
{
    SYMRESULT symRes = m_ldrThreatInfo.Initialize();
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to init the threatinfo loader", symRes);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CScanUIMisc::DisplayVIDDetails(HWND hParentWnd, DWORD dwVID)
{
    // Get the threat info object
    IAVThreatInfoPtr spThreatInfo;
    SYMRESULT symRes = m_ldrThreatInfo.CreateObject(spThreatInfo);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the threatinfo object", symRes);
        return E_FAIL;
    }

    // Create a threat tracker object
    CSymPtr<CThreatTracker> spThreatTracker = new CThreatTracker(spThreatInfo, false);
    if(!spThreatTracker)
    {
        LOG_FAILURE_AND_RETURN(L"new of CThreatTracker failed!", E_OUTOFMEMORY);
    }

    // Get the details for this VID
    IAVMapDwordDataPtr spVIDDispData;
    HRESULT hr = spThreatInfo->GetVIDDisplayData(dwVID, spVIDDispData);
    LOG_FAILURE_AND_RETURN(L"Failed to get display data for VID", hr);

    // Get the threatmap
    IAVMapDwordDataPtr spVIDThreatMap;
    hr = spVIDDispData->GetValue(ThreatProcessor::GetVIDDispOut_ThreatData,
                                 AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spVIDThreatMap));
    LOG_FAILURE_AND_RETURN(L"Failed to get threatmap from display data", hr);

    // Get the anomaly display data
    IAVArrayDataPtr spAnomalyDispInfo;
    hr = spVIDDispData->GetValue(ThreatProcessor::GetVIDDispOut_AnomDispData,
                                 AVDataTypes::eTypeArray, reinterpret_cast<ISymBase*&>(spAnomalyDispInfo));
    LOG_FAILURE(L"Failed to get anomaly display data from display data (not all VIDs have this)", hr);

    // If we have anomaly display data for this item, shove it in the threat map
    // so the threat tracker can find it...
    if(spAnomalyDispInfo)
    {
        hr = spVIDThreatMap->SetValue(0xFFFF, spAnomalyDispInfo);
        LOG_FAILURE(L"Failed to add anomaly display data to threat map", hr);
    }

    // We need a fake threat ID ... All Zeros will do
    GUID idTmp;
    ZeroMemory(&idTmp, sizeof(idTmp));
    spVIDThreatMap->SetValue(ThreatTracking::TrackingData_THREATID, idTmp);

    // Add this item to the threat tracker (for text formatting)
    if(!spThreatTracker->AddThreat(spVIDThreatMap))
    {
        LOG_FAILURE_AND_RETURN(L"Failed to add threatmap to threat tracker", E_FAIL);
    }

    // Create our prompt dialog
    CSymPtr<CRiskDetailsDlg> spDlgDoc = new CRiskDetailsDlg(spThreatTracker, 0, spVIDThreatMap,
                                                            spAnomalyDispInfo, ThreatTracking::ThreatType_Anomaly);
    if(!spDlgDoc)
    {
        LOG_FAILURE_AND_RETURN(L"new of CRiskDetailsDlg failed!", E_OUTOFMEMORY);
    }

    symhtml::ISymHTMLDialogPtr spDialog;
    symRes = symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the ISymHTMLDialog object", symRes);
        return E_FAIL;
    }

    // Display the details property sheet UI
    int nResult = 0;
    hr = spDialog->DoModal(hParentWnd, spDlgDoc, &nResult);
    LOG_FAILURE_AND_RETURN(L"DoModal failed", hr);

    // If we made it here... we succeeded
    return S_OK;
}

HRESULT CScanUIMisc::DisplayThreatDetails(HWND hParentWnd, const GUID& idThreat)
{
    // Get the threat info object
    IAVThreatInfoPtr spThreatInfo;
    SYMRESULT symRes = m_ldrThreatInfo.CreateObject(spThreatInfo);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the threatinfo object", symRes);
        return E_FAIL;
    }

    // Create a threat tracker object
    CSymPtr<CThreatTracker> spThreatTracker = new CThreatTracker(spThreatInfo, false);
    if(!spThreatTracker)
    {
        LOG_FAILURE_AND_RETURN(L"new of CThreatTracker failed!", E_OUTOFMEMORY);
    }

    // Get the details of this threat
    IAVMapDwordDataPtr spThreatMap;
    HRESULT hr = spThreatInfo->GetSingleThreat(idThreat, spThreatMap);
    LOG_FAILURE_AND_RETURN(L"Failed to GetSingleThreat", hr);

    // Add this threat to our threat tracker
    if(!spThreatTracker->AddThreat(spThreatMap))
    {
        LOG_FAILURE_AND_RETURN(L"Failed to add threatmap to threat tracker", E_FAIL);
    }

    // Call the threat tracker to do the rest of the work
    return spThreatTracker->DisplayThreatDetails(hParentWnd, 0);
}

HRESULT CScanUIMisc::ProcessSingleThreat(HWND hParentWnd, const GUID& idThreat) throw()
{
    // Get the threat info object
    IAVThreatInfoPtr spThreatInfo;
    SYMRESULT symRes = m_ldrThreatInfo.CreateObject(spThreatInfo);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the threatinfo object", symRes);
        return E_FAIL;
    }

    // Create our prompt dialog
    CSymPtr<CProcessSingleThreatDlg> spDlgDoc = new CProcessSingleThreatDlg(spThreatInfo, idThreat);
    if(!spDlgDoc)
    {
        LOG_FAILURE_AND_RETURN(L"new of ProcessSingleThreatDlg failed!", E_OUTOFMEMORY);
    }

    symhtml::ISymHTMLDialogPtr spDialog;
    symRes = symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the ISymHTMLDialog object", symRes);
        return E_FAIL;
    }

    // Display the details property sheet UI
    int nResult = 0;
    HRESULT hr = spDialog->DoModal(hParentWnd, spDlgDoc, &nResult);
    LOG_FAILURE_AND_RETURN(L"DoModal failed", hr);

    // Default to failure
    hr = E_FAIL;
    switch(nResult)
    {
    case IDNO:
        hr = S_FALSE;
        break;
    case IDOK:
        {
            if(spDlgDoc->GetSucceeded())
            {
                hr = S_OK;
            }
            break;
        }
    }

    return hr;
}

HRESULT CScanUIMisc::QuarantineAdd(HWND hParentWnd, GUID& idThreat)
{
    // Get the threat info object
    IAVThreatInfoPtr spThreatInfo;
    SYMRESULT symRes = m_ldrThreatInfo.CreateObject(spThreatInfo);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the threatinfo object", symRes);
        return E_FAIL;
    }

    // Create our prompt dialog
    CSymPtr<CManualQuarantineDlg> spDlgDoc = new CManualQuarantineDlg(spThreatInfo);
    if(!spDlgDoc)
    {
        LOG_FAILURE_AND_RETURN(L"new of CManualQuarantineDlg failed!", E_OUTOFMEMORY);
    }

    symhtml::ISymHTMLDialogPtr spDialog;
    symRes = symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the ISymHTMLDialog object", symRes);
        return E_FAIL;
    }

    // Display the details property sheet UI
    int nResult = 0;
    HRESULT hr = spDialog->DoModal(hParentWnd, spDlgDoc, &nResult);
    LOG_FAILURE_AND_RETURN(L"DoModal failed", hr);

    // Default to failure
    hr = E_FAIL;
    switch(nResult)
    {
        case IDCANCEL:
            hr = S_FALSE;
            break;
        case IDOK:
        {
            if(spDlgDoc->GetSucceeded())
            {
                spDlgDoc->GetItemGUID(idThreat);
                hr = S_OK;
            }
            break;
        }
    }

    return hr;
}

HRESULT CScanUIMisc::QuarantineRestore(HWND hParentWnd, const GUID& idThreat)
{
    // Get the threat info object
    IAVThreatInfoPtr spThreatInfo;
    SYMRESULT symRes = m_ldrThreatInfo.CreateObject(spThreatInfo);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the threatinfo object", symRes);
        return E_FAIL;
    }

    // Create our prompt dialog
    CSymPtr<CQRestoreDlg> spDlgDoc = new CQRestoreDlg(spThreatInfo, idThreat);
    if(!spDlgDoc)
    {
        LOG_FAILURE_AND_RETURN(L"new of CQRestoreDlg failed!", E_OUTOFMEMORY);
    }

    symhtml::ISymHTMLDialogPtr spDialog;
    symRes = symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog);
    if(SYM_FAILED(symRes))
    {
        SYM_LOG_FAILURE(L"Failed to create the ISymHTMLDialog object", symRes);
        return E_FAIL;
    }

    // Display the details property sheet UI
    int nResult = 0;
    HRESULT hr = spDialog->DoModal(hParentWnd, spDlgDoc, &nResult);
    LOG_FAILURE_AND_RETURN(L"DoModal failed", hr);

    // Default to failure
    hr = E_FAIL;
    switch(nResult)
    {
        case IDNO:
            hr = S_FALSE;
            break;
        case IDOK:
        {
            if(spDlgDoc->GetSucceeded())
            {
                hr = S_OK;
            }
            break;
        }
    }

    return hr;
}

avScanUI::eRebootPromptResult CScanUIMisc::QueryUserForReboot(HWND hParentWnd)
{
    avScanUI::eRebootPromptResult eRes = avScanUI::eReboot_Error;
    AVLoader_IAVAutoProtect loaderAP;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        // In either case, configure AP so threats get removed after the eventual reboot
        // TODO: In NAVW32 on clean global or eraser scan, set AP:DelayLoadLock=0
        SYMRESULT symRes = loaderAP.Initialize();
        if(SYM_FAILED(symRes))
        {
            SYM_LOG_FAILURE(L"Failed to init the IAVAutoProtect loader!", symRes);
            return avScanUI::eReboot_Error;
        }

        // Get the AP object
        AVModule::IAVAutoProtectPtr spAP;
        symRes = loaderAP.CreateObject(&spAP);
        if(SYM_FAILED(symRes))
        {
            SYM_LOG_FAILURE(L"Failed to create the IAVAutoProtect object!", symRes);
            return avScanUI::eReboot_Error;
        }

        // Enable AP to ensure this threat doesn't spread itself during shutdown
        HRESULT hr = spAP->EnableAP(true);
        LOG_FAILURE(L"Failed to enable AP!", hr);

        // Set AP to start at system start (Set AP::DriversSystemStart=1)
        hr = spAP->SetOption(AVModule::AutoProtect::APOPTION_SYSTEMSTART_DRIVERS, 1);  // 1 = true = enabled
        LOG_FAILURE(L"Failed to set AP to load at system start!", hr);

        // Init the nav opts helper
    	CNAVOptSettingsEx NavOpts;
		if(!NavOpts.Init())
		{
            // This error is not fatal
            LOG_FAILURE(L"NavOpts.Init() returned false!", E_FAIL);
		}

        // Lock the NAVAP:DelayLoad setting (Set NAVAP:DelayLoadLock=1)
        hr = NavOpts.SetValue(NAVAP_DelayLoadLock, 1);
        LOG_FAILURE(L"Failed to set NAVAP:DelayLoadLock=1", hr);        

        //
        // Build the reboot alert
        //
        cc::IAlert2Ptr spRebootAlert;
        cc::ccAlertMgd_IAlert2::CreateObject(GETMODULEMGR(), spRebootAlert);
        if(!spRebootAlert)
            hrx << E_UNEXPECTED;

        // Get product name
        CString cszBrandingProductName = CISVersion::GetProductName();

        // Add Alert Title
        spRebootAlert->SetWindowTitle(cszBrandingProductName);
        spRebootAlert->SetTitleBitmap(_Module.GetResourceInstance(), IDB_OEHALERT_TITLE_RED);
        spRebootAlert->SetAlertTitle(_S(IDS_REBOOTREQUEST_TITLE));
        spRebootAlert->SetBriefDesc(_S(IDS_REBOOTREQUEST_BRIEF_DESC));
        spRebootAlert->SetPromptText(_S(IDS_REBOOTREQUEST_PROMPT_TEXT));

        // Add Action
        spRebootAlert->AddAction(_S(IDS_REBOOTREQUEST_REBOOT_NOW));
        spRebootAlert->AddAction(_S(IDS_REBOOTREQUEST_REBOOT_LATER));

        // Set the default selection
        spRebootAlert->SetRecAction(0);

        UINT iRet = 0;
        if(IsWindow(hParentWnd))
            iRet = spRebootAlert->DisplayAlert(hParentWnd, NULL);
        else
            iRet = spRebootAlert->DisplayAlert();

        // Check the result...
        if(0 == iRet)
            eRes = avScanUI::eReboot_Now;
        else if(1 == iRet)
            eRes = avScanUI::eReboot_Later;
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return eRes;
}

avScanUI::eProcTermResult CScanUIMisc::QueryUserForProcTerm(HWND hParentWnd)
{
#pragma TODO("This needs to use ccAlert...")
    avScanUI::eProcTermResult eRes = avScanUI::eProcTerm_Error;
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        // Get product name
        CString cszBrandingProductName = CISVersion::GetProductName();
        int nRet = MessageBox(hParentWnd, _S(IDS_PROCTERM_PROMPT_TEXT), cszBrandingProductName, MB_OKCANCEL | MB_ICONEXCLAMATION);
        if(nRet == IDOK)
            eRes = avScanUI::eProcTerm_Yes;
        else if(nRet == IDCANCEL)
            eRes = avScanUI::eProcTerm_No;
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return eRes;
}