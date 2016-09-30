////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "EMailScanUI.h"
#include <AvProdLoggingLoader.h>
#include "EMailThreatDlg.h"

using namespace avScanUI;
using namespace AVModule;
using namespace AvProd;

CEmailScanUI::CEmailScanUI(void)
{
}

CEmailScanUI::~CEmailScanUI(void)
{
    CCTRCTXI0(L"dtor");
}

HRESULT CEmailScanUI::Initialize()
{
    HRESULT hr = InitSkinEngine(m_spSkin);
    LOG_FAILURE(L"Failed to init the skin engine ... continuing", hr);

    // Skin all dialogs
    if(m_spSkin)
    {
        hr = m_spSkin->SetDialogSkin(symtheme::skins::common::dialog);
        LOG_FAILURE(L"Failed to apply the loaded SymTheme to dialogs!", hr);
    }

    return S_OK;
}

HRESULT CEmailScanUI::DisplayThreatUI(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData)
{
    HRESULT hr = E_FAIL;

    // Create an instance of the threat tracker
    CSymPtr<CThreatTracker> spThreatTracker = new CThreatTracker((AVModule::IAVScanBase*)NULL, NULL);
    if(!spThreatTracker)
    {
        LOG_FAILURE_AND_RETURN(L"Failed to create an instance of the threat tracker", E_FAIL);
    }

    // Get the email details
    cc::IStringPtr spSubject, spSender, spRecipient;
    hr = pEmailInfo->GetValue(EmailScanner::AV_EMAILTHREATSDETECTED_SUBJECT, spSubject);
    LOG_FAILURE_AND_RETURN(L"Failed to get the email subject from the input map", hr);
    hr = pEmailInfo->GetValue(EmailScanner::AV_EMAILTHREATSDETECTED_SENDER, spSender);
    LOG_FAILURE_AND_RETURN(L"Failed to get the email sender from the input map", hr);
    hr = pEmailInfo->GetValue(EmailScanner::AV_EMAILTHREATSDETECTED_RECEIPIENT, spRecipient);
    LOG_FAILURE_AND_RETURN(L"Failed to get the email recipient from the input map", hr);

    // Get the threat map
    ISymBasePtr spThreatsBase;
    hr = pEmailInfo->GetValue(EmailScanner::AV_EMAILTHREATSDETECTED_THREATS, AVDataTypes::eTypeMapGuid, spThreatsBase);
    LOG_FAILURE_AND_RETURN(L"Failed to get threat data from input map", hr);

    IAVMapGuidDataQIPtr spMapThreats(spThreatsBase);
    if(!spMapThreats)
    {
        LOG_FAILURE_AND_RETURN(L"Failed to QI the threat map for IAVMapGuidData", hr);
    }

    // Add each item to the threat tracker
    DWORD dwItems = 0;
    spMapThreats->GetCount(dwItems);
    for(DWORD dwIdx = 0; dwIdx < dwItems; ++dwIdx)
    {
        // Get this item
        ISymBasePtr spThreatBase;
        hr = spMapThreats->GetValueByIndex(dwIdx, AVDataTypes::eTypeMapDword, spThreatBase);
        LOG_FAILURE_AND_RETURN(L"Failed to QI the threat map for IAVMapGuidData", hr);

        // QI for the proper interface
        IAVMapDwordDataQIPtr spThreat(spThreatBase);
        if(!spThreat)
        {
            LOG_FAILURE_AND_RETURN(L"Failed to QI the threat map for IAVMapDwordData", E_FAIL);
        }

        // Add this item to the threat tracker
        spThreatTracker->AddThreat(spThreat);
    }

    // Display the property sheet
    symhtml::ISymHTMLDialogPtr spDialog;
    if(SYM_FAILED(symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog)))
    {
        LOG_FAILURE_AND_RETURN(L"Failed to create ISymHTMLDialog", E_FAIL);
    }
    // Create our dialog
    CSymPtr<CEMailThreatDlg> spDlgDoc = new CEMailThreatDlg(spThreatTracker, spSubject->GetStringW(),
                                                            spSender->GetStringW(), spRecipient->GetStringW());
    if(!spDlgDoc)
    {
        LOG_FAILURE_AND_RETURN(L"new of CEMailThreatDlg failed!", E_FAIL);
    }

    // Display the details property sheet UI
    int nResult = 0;
    hr = spDialog->DoModal(NULL, spDlgDoc, &nResult);
    LOG_FAILURE_AND_RETURN(L"DoModal failed", hr);

    switch(nResult)
    {
        case IDNO: // Drop this email
            // Just return the action, as there is no point to filling in the action map
            pReturnData->SetValue(EmailScanner::AV_EMAILTHREATSDETECTEDRETURN_ACTION, (DWORD)EmailScanner::eEmailProcessingAction_DropEmail);
            hr = S_OK;
            break;
        case IDYES: // Apply the selected actions
        {
            // Set the action
            pReturnData->SetValue(EmailScanner::AV_EMAILTHREATSDETECTEDRETURN_ACTION, (DWORD)EmailScanner::eEmailProcessingAction_ProcessThreats);

            // Get the action map
            ISymBasePtr spActionsBase;
            hr = pReturnData->GetValue(EmailScanner::AV_EMAILTHREATSDETECTEDRETURN_ITEMSTOPROCESS, AVDataTypes::eTypeMapGuid, spActionsBase);
            LOG_FAILURE_AND_RETURN(L"Failed to get threat data from input map", hr);

            IAVMapGuidDataQIPtr spMapActions(spActionsBase);
            if(!spMapActions)
            {
                LOG_FAILURE_AND_RETURN(L"Failed to QI the threat map for IAVMapGuidData", hr);
            }

            // Fill in the action map
            hr = spThreatTracker->FillActionsMap(spMapActions);
            break;
        }
        default:
            LOG_FAILURE_AND_RETURN(L"DoModal returned an unexpected result", E_UNEXPECTED);
    }

    return hr;
}

HRESULT CEmailScanUI::InitSkinEngine(symtheme::ISymSkinWindow*& pSkinWindow)
{
    SYMRESULT symRes = SYM_OK;
    symtheme::ISymSkinWindowPtr spSkinWindow;
    symRes = symtheme::loader::ISymSkinWindow::CreateObject(GETMODULEMGR(), &spSkinWindow);
    if(SYM_FAILED(symRes))
    {
        LOG_FAILURE_AND_RETURN(L"Unable to create SymTheme object", E_FAIL);
    }

    // Init the skin engine
    HRESULT hr = spSkinWindow->Init(_Module.GetModuleInstance(), L"avScanUI");
    LOG_FAILURE_AND_RETURN(L"Unable to init the SymTheme object", hr);

    // Load our skin
    hr = spSkinWindow->LoadSkin(symtheme::skins::common::SKINID_SymantecCommon);
    LOG_FAILURE_AND_RETURN(L"Unable to load our SymTheme SMF file", hr);

    if(pSkinWindow)
    {
        pSkinWindow->Release();
        pSkinWindow = NULL;
    }

    pSkinWindow = spSkinWindow.Detach();
    return S_OK;
}
