////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AvSinks.h"
#include <SessionAppClientImpl.h>

#include <AvProdLoggingLoader.h>
#include <AVSubmissionLoader.h>

#include <AvProdServiceRes.h>
#include <AvProdCommands.h>

#include <ccSerializeInterface.h>
#include <ccSymMemoryStreamIMpl.h>
#include <ccSymKeyValueCollectionImpl.h>
#include <ccSymStringImpl.h>
#include <ccInstanceFactory.h>
#include <ccSymInstalledApps.h>
#include <ccSymModuleLifetimeMgrHelper.h> // GETMODULEMGR()
#include <NAVSettingsHelperEx.h> // options
#include <OptNames.h>
#include <navinfo.h>

#include <ISVersion.h>
#include <ISymMceCmdLoader.h>	// detect optional Media center components

using namespace AvProdSvc;
using namespace AvProd;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAvSinks::CAvSinks(void):
m_dwYear(0), m_dwMonth(0), m_dwDay(0), m_dwRev(0)
{
}

CAvSinks::~CAvSinks(void)
{
	CCTRCTXI0(L"");
}

//****************************************************************************
//****************************************************************************
HRESULT CAvSinks::Initialize() throw()
{
	HRESULT hr, hrReturn = S_OK;
	for(;;)
	{
		//
        // In the event the submission engine cannot be init'd
        //  do not fail this call.  It is not a catastrphophic failure.

        SYMRESULT sr = AVSubmit::AVSubmit_IAVSubmissionManagerMgd::CreateObject(GETMODULEMGR(), m_pSubmitMgr.m_p);
		if(SYM_FAILED(sr) || m_pSubmitMgr == NULL)
		{
			CCTRCTXE1(L"Failed to create AVSubmissionManager: 0x%08X", sr);
			break;
		}
		
        hr = m_pSubmitMgr->Initialize(AVSubmit::IAVSubmissionManager::eAsynchronous);
		if(FAILED(hr))
		{
			CCTRCTXE1(L"Failed to init submission manager: 0x%08X", hr);
            m_pSubmitMgr.Release();
			break;
		}

		break;
	}

	return hrReturn;
}



//****************************************************************************
//****************************************************************************
HRESULT CAvSinks::AddSubmission(AVModule::IAVMapBase* pData) throw()
{
	HRESULT hr = E_FAIL;
	for(;;)
	{
		if(m_pSubmitMgr == NULL)
		{
			hr = E_POINTER;
			CCTRCTXE0(L"m_pSubmitMgr == NULL");
			break;
		}
		if(pData == NULL)
		{
			hr = E_INVALIDARG;
			CCTRCTXE0(L"pData == NULL");
			break;
		}
		hr = m_pSubmitMgr->Add(pData);
		if(FAILED(hr))
		{
			CCTRCTXE1(L"Failed to add submission: 0x%08X", hr);
			break;
		}

		hr = S_OK;
		break;
	}
	return hr;
}


//****************************************************************************
//****************************************************************************
void CAvSinks::OnAPProcessingThreatBegin(AVModule::IAVMapDwordData* pData)
{
    CCTRACEI( CCTRCTX _T("Processing Begin"));

    HRESULT hr;
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        // Get File Name
        cc::IStringPtr spFileNameString;
		hr = pData->GetValue(AVModule::AutoProtect::APThreatBeingProcessed_FileName, spFileNameString);
        if(FAILED(hr) || !spFileNameString)
        {
            CCTRACEE( CCTRCTX _T("Unable to get file name string. hr=0x%08X"), hr);
        }

        // Get Threat Name
        cc::IStringPtr spThreatNameString;
        hr = pData->GetValue(AVModule::AutoProtect::APThreatBeingProcessed_Name, spThreatNameString);
        if(FAILED(hr) || !spThreatNameString)
        {
            CCTRACEE( CCTRCTX _T("Unable to get threat name string. hr=0x%08X"), hr);
        }
            
        // Increment Auto-Protect Activity
        if(spFileNameString && spThreatNameString)
            CSvcMgrSingleton::GetSvcMgr()->PushAutoProtectActivity(CW2T(spFileNameString->GetStringW()), CW2T(spThreatNameString->GetStringW()));
        else if(spFileNameString)
            CSvcMgrSingleton::GetSvcMgr()->PushAutoProtectActivity(CW2T(spFileNameString->GetStringW()), NULL);
        else
            CCTRACEE( CCTRCTX _T("Not enough info to generate notification."));

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnAPProcessingThreatComplete(AVModule::IAVMapDwordData* pData)
{
    CCTRACEI( CCTRCTX _T("Processing complete"));

    HRESULT hr; SYMRESULT sr;
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
		//ignore result, since submission status has no effect on AP Processing
		(void) AddSubmission(pData);

		cc::IStringPtr spTempString;
		spTempString.Release();
		hr = pData->GetValue(AVModule::AutoProtect::APProcessingComplete_FileName, spTempString);
		if(SUCCEEDED(hr) && spTempString)
        {
            AVModule::IAVMapDwordDataQIPtr spMapCurThreat;
            do
            {
                ISymBasePtr spTempData;
                hr = pData->GetValue(AVModule::AutoProtect::APProcessingComplete_AllThreatsDetected, AVModule::AVDataTypes::eTypeArray, spTempData);
                if(FAILED(hr) || !spTempData)
                    break;

                AVModule::IAVArrayDataQIPtr spThreatList = spTempData;
                spTempData.Release();
                if(!spThreatList)
                    break;
                
                DWORD dwThreatListSize = NULL;
                spThreatList->GetCount(dwThreatListSize);
                if(!dwThreatListSize)
                    CCTRACEW( CCTRCTX _T("There are no items in the threat list, there should be..."));
                for(DWORD dwThreatListIndex = 0; dwThreatListIndex < dwThreatListSize; dwThreatListIndex++)
                {
                    ISymBasePtr spTempThreatInfo;
                    HRESULT hr = spThreatList->GetValue(dwThreatListIndex, AVModule::AVDataTypes::eTypeMapDword, spTempThreatInfo);
                    if(FAILED(hr))
                    {
                        CCTRACEE( CCTRCTX _T("Unable to retrieve threat info at index %d."), dwThreatListIndex);
                        continue;
                    }

                    AVModule::IAVMapDwordDataQIPtr spThreatInfo = spTempThreatInfo;
                    if(!spThreatInfo)
                    {
                        CCTRACEE( CCTRCTX _T("Unable to get MapDwordData at index %d."), dwThreatListIndex);
                        continue;
                    }

                    PostProcessThreatComplete(spThreatInfo);
                }
                
                if(dwThreatListSize > 1)
                    break; // Only provide threat data if a single threat was found

                spThreatList->GetValue(0, AVModule::AVDataTypes::eTypeMapDword, spTempData);
                spMapCurThreat = spTempData;
                
            } while(FALSE); // scoping only
            
            CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(CW2T(spTempString->GetStringW()), spMapCurThreat);
        }
        
        AvModuleLogging::IAutoProtectClientActivitiesPtr spLog;
        sr = AvModuleLogging::AutoProtectClientActivities_Loader::CreateObject(GETMODULEMGR(), &spLog);
        if(SYM_SUCCEEDED(sr) && spLog)
        {
            spLog->OnAutoProtectRemediationComplete(pData);
        }
    
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

//****************************************************************************
//****************************************************************************
void CAvSinks::PostProcessThreatComplete(AVModule::IAVMapDwordData* pThreatInfo)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        do
        {
            if(S_OK != IsLowRiskThreat(pThreatInfo))
                break; // no need to continue processing
            
            if(CSvcMgr::eRespondIgnore == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eAutoProtectComponent))
                break; // no need to continue processing

            CString cszThreatName;
            cc::IStringPtr spThreatName;
            pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_NAME, spThreatName);
            if(spThreatName)
                cszThreatName = spThreatName->GetStringW();
            
            if(CSvcMgr::eRespondRemediate == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eAutoProtectComponent))
            {
                DWORD dwCurrentState = AVModule::ThreatTracking::ThreatState_FullyRemoved;
                hrx << pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwCurrentState);
                
                if(AVModule::ThreatTracking::ThreatNotRemoved(dwCurrentState))
                {
                    GUID guidThreatId = GUID_NULL;
                    hrx << pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, guidThreatId);
                    
                    CSvcMgrSingleton::GetSvcMgr()->SubmitRemediationRequest(NULL, guidThreatId);
                    
                    // Pulse notifications
                    CSvcMgrSingleton::GetSvcMgr()->PushAutoProtectActivity(_T("PostProcessing"), cszThreatName);
                    CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(_T("PostProcessing"), pThreatInfo);
                }

                break; // no need to continue processing
            }

            if(CSvcMgr::eRespondAskUser == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eAutoProtectComponent))
            {
                SymMCE::ISymMceCmdLoader	mce;

                // do not display notifications if MCE is active
                BOOL bShouldSuppressUI = SYM_SUCCEEDED(mce.DoNotAlertToDesktop(CISVersion::GetProductName(), SymMCE::MCE_ALERT_CHECK_STATE));
                if(bShouldSuppressUI)
                {
                    CCTRACEI( CCTRCTX _T("MCE is active, suppressing UI"));
                    break; // effectively ignore once
                }

                cc::IKeyValueCollectionPtr spCommand;
                spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
                CCTHROW_BAD_ALLOC(spCommand);

                // Serialize the AvModule data into the command data
                ISymBaseQIPtr spAvModuleData = pThreatInfo;
                spCommand->SetValue(AvProd::OnAskUserForThreatRemediation::eThreatInfo, spAvModuleData);
                spAvModuleData.Release();

                DWORD dwAction = AvProd::OnAskUserForThreatRemediation::eIgnoreAlways;
                spCommand->SetValue(AvProd::OnAskUserForThreatRemediation::eAction, dwAction);

                ISymBaseQIPtr spTempCommand = spCommand;

                GUID guidThreatId = GUID_NULL;
                hrx << pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, guidThreatId);

                CSvcMgrSingleton::GetSvcMgr()->SubmitRemediationRequest(spTempCommand, guidThreatId);
                
                // Pulse notifications
                CSvcMgrSingleton::GetSvcMgr()->PushAutoProtectActivity(_T("PostProcessing"), cszThreatName);
                CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(_T("PostProcessing"), pThreatInfo);
                
                CCTRACEI( CCTRCTX _T("Threat processing re-started"));

                break; // no need to continue processing
            }

            CCTRACEE( CCTRCTX _T("Unexpected PostProcessing state"));

        } while(FALSE); // scoping only
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvSinks::IsLowRiskThreat(AVModule::IAVMapDwordData* pThreatTrackInfo)
{
    if(!pThreatTrackInfo)
        return E_INVALIDARG;

    HRESULT hr;
    DWORD dwHasViralCategory;
    hr = pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_HASVIRALCATEGORY, dwHasViralCategory);
    if(SUCCEEDED(hr) && 1 == dwHasViralCategory)
        return S_FALSE;

    DWORD dwThreatMatrixValue = NULL;
    hr = pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, dwThreatMatrixValue);
    if(FAILED(hr))
        return E_UNEXPECTED;
    
    if(SUCCEEDED(hr) && S_OK == IsLowRiskThreat(dwThreatMatrixValue))
        return S_OK;

    return S_FALSE;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvSinks::IsLowRiskThreat(DWORD dwThreatMatrixValue)
{
    if(AVModule::ThreatTracking::ThreatLevel_Low == dwThreatMatrixValue)
        return S_OK;

    return S_FALSE;
}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnAPThreatRedetection(AVModule::IAVMapDwordData* pData)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CCTRACEI( CCTRCTX _T("Handling OnAPThreatRedetection"));

        CString cszThreatName, cszPath;
        cc::IStringPtr spTempString;
        HRESULT hr = pData->GetValue(AVModule::AutoProtect::APRedetection_Name, spTempString);
        if(FAILED(hr) || !spTempString)
            cszThreatName = _T("(not available)");
        else
            cszThreatName = spTempString->GetStringW();


        spTempString.Release();
        hr = pData->GetValue(AVModule::AutoProtect::APRedetection_FileName, spTempString);
        if(FAILED(hr) || !spTempString)
            cszPath = _T("(not available)");
        else
            cszPath = spTempString->GetStringW();
        
        ISymBasePtr spTemp;
        hr = pData->GetValue(AVModule::AutoProtect::APRedetection_ThreatData, AVModule::AVDataTypes::eTypeMapDword, spTemp);
        
        AVModule::IAVMapDwordDataQIPtr spThreatInfo = spTemp;
        if(spThreatInfo)
        {
            PostProcessThreatComplete(spThreatInfo);
        }
        
        CCTRACEI( CCTRCTX _T("Ignored. Risk Name: %s, Path: %s"), (LPCTSTR)cszThreatName, (LPCTSTR)cszPath);    
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnAPThreatBlocked(AVModule::IAVMapDwordData* pData)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CCTRACEI( CCTRCTX _T("Handling OnAPThreatBlocked"));

        AvModuleLogging::IAutoProtectClientActivitiesPtr spLog;
        AvModuleLogging::AutoProtectClientActivities_Loader::CreateObject(GETMODULEMGR(), &spLog);
        if(spLog)
        {
            spLog->OnAutoProtectThreatBlocked(pData);
        }

        //ignore result, since submission status has no effect on AP Processing
		(void) AddSubmission(pData);
        
        CString cszFileName, cszThreatName;
        HRESULT hr;
        
        // Get filename
        cc::IStringPtr spTempString;
        hr = pData->GetValue(AVModule::AutoProtect::APBlocked_FileName, spTempString);
        if(SUCCEEDED(hr) && spTempString)
        {
            cszFileName = CW2T(spTempString->GetStringW());
        }
        else
        {
            cszFileName.Format(_T("NoName Job 0x%08X"), GetTickCount());
            CCTRACEW( CCTRCTX _T("Unable to get filename for block action.  Defaulting to tick count: \"%s\""), cszFileName);
        }
            
        // Get the threat name
        spTempString.Release();
        hr = pData->GetValue(AVModule::AutoProtect::APBlocked_Name, spTempString);
        if(SUCCEEDED(hr) && spTempString)
        {
            cszThreatName = CW2T(spTempString->GetStringW());
        }
        else
        {
            CCTRACEW( CCTRCTX _T("Unable to get threat name for block action."));
        }

        // Pulse the notifications
        CSvcMgrSingleton::GetSvcMgr()->PushAutoProtectActivity(cszFileName, cszThreatName, CSvcMgr::eAvActionBlocked);
        CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(cszFileName, cszThreatName, CSvcMgr::eAvActionBlocked);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnAPBootInfection(AVModule::IAVMapDwordData* pMapThreat)
{
    ccLib::CExceptionInfo exceptionInfo;

    try
    {
        CCTRACEI( CCTRCTX _T("Handling OnAPBootInfection"));

        if( pMapThreat )
        {
            (void) AddSubmission(pMapThreat);

            HRESULT hr = S_OK;
            CString cszFileName, cszThreatName;

            // Threat name
            cc::IStringPtr spTempString;
            hr = pMapThreat->GetValue(AVModule::ThreatTracking::TrackingData_NAME, spTempString);
            if(SUCCEEDED(hr) && spTempString)
            {
                cszThreatName = CW2T(spTempString->GetStringW());
            }
            else
            {
                CCTRACEW( CCTRCTX _T("Unable to get threat name for boot infection."));
            }

            // Drive name
            spTempString.Release();
            hr = pMapThreat->GetValue(AVModule::ThreatTracking::TrackingData_BootRecord_DriveID, spTempString);
            if(SUCCEEDED(hr) && spTempString)
            {
                cszFileName = CW2T(spTempString->GetStringW());
            }
            else
            {
                CCTRACEW( CCTRCTX _T("Unable to get device name for boot infection."));
            }

            // Action taken
            DWORD dwAction = AVModule::ThreatTracking::ThreatState_RemoveNotAttempted;
            if( FAILED(pMapThreat->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwAction)) )
                CCTRACEE( CCTRCTX _T("Unable to get threat state for boot infection."));

            // Convert CAV action to AVPSVC action
            if( AVModule::ThreatTracking::ThreatRemoved(dwAction) )
                dwAction = CSvcMgr::eAvActionRemoved;
            else
                dwAction = CSvcMgr::eAvModuleActionSummaryInvalid;

            // Pulse the notifications
            CSvcMgrSingleton::GetSvcMgr()->PushAutoProtectActivity(cszFileName, cszThreatName, dwAction);
            CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(cszFileName, cszThreatName, dwAction);
        }

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;

}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnEmailSessionStart(DWORD dwSessionID)
{
    CCTRACEI( CCTRCTX _T("E-mail session start for session: %d"), dwSessionID);
    CString cszJobName;
    cszJobName.Format(_T("Email Session: %d"), dwSessionID);
    CSvcMgrSingleton::GetSvcMgr()->PushWork(cszJobName);
}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnEmailSessionFinish(DWORD dwSessionID, AVModule::IAVMapStrData* pStats)
{
    CCTRACEI( CCTRCTX _T("E-mail session finish for session: %d"), dwSessionID);
    CString cszJobName;
    cszJobName.Format(_T("Email Session: %d"), dwSessionID);
    CSvcMgrSingleton::GetSvcMgr()->PopWork(cszJobName);

    if(pStats)
    {
            AvModuleLogging::IEmailScanClientActivitiesPtr spLog;
            SYMRESULT sr = AvModuleLogging::EmailScanClientActivities_Loader::CreateObject(GETMODULEMGR(), &spLog);
            if(SYM_SUCCEEDED(sr) && spLog)
            {
                DWORD dwVirCount = 0, dwNonVirCount;
                pStats->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_VIRSDETECTED, dwVirCount);
                pStats->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_NONVIRALSDETECTED, dwNonVirCount);
                if(dwVirCount || dwNonVirCount) // only log if more than 0 items were detected
                {
                    CCTRACEI( CCTRCTX _T("Generating log entry..."));
                    spLog->OnEmailScanComplete(pStats);
					
					GUID guidScanId;
					HRESULT hr = pStats->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_INSTANCEGUID, guidScanId);
					if(SUCCEEDED(hr))
						CSvcMgrSingleton::GetSvcMgr()->LogEmailScanResults(guidScanId);
                }
            }
            else
                CCTRACEW( CCTRCTX _T("Unable to get logging object!!"));
        }
    else
        CCTRACEW( CCTRCTX _T("No stats provided!!"));

}

//****************************************************************************
//****************************************************************************
void CAvSinks::ShowEmailThreatsUI(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::IKeyValueCollectionPtr spCommand;
		spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(spCommand);

        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");

        // Serialize the AvModule data into the command data
        ISymBaseQIPtr spAvModuleData = pEmailInfo;
        spCommand->SetValue(AvProd::OnEmailThreatsDetected::eEmailInfo, spAvModuleData);
        spAvModuleData.Release();

        spAvModuleData = pReturnData;
        spCommand->SetValue(AvProd::OnEmailThreatsDetected::eReturnData, spAvModuleData);
        spAvModuleData.Release();

        CCTRACEI( CCTRCTX _T("Sending command..."));
        ISymBaseQIPtr spTempCommand = spCommand, spTempReturn;
        hrx << spClient->SendCommand(AvProd::CMDID_OnEmailThreatsDetected, spTempCommand, &spTempReturn);
        cc::IStreamQIPtr spStream = spTempReturn;
        if(!spStream)
            hrx << E_UNEXPECTED;

        cc::ISerializeQIPtr spDeserialize = pReturnData;
        if(!spDeserialize)
            hrx << E_UNEXPECTED;
        
        // Clear this data out before Load()'ing...
        pReturnData->Clear();

        CCTRACEI( CCTRCTX _T("Loading return data..."));
        if(!spDeserialize->Load(spStream))
            hrx << E_UNEXPECTED;

        CCTRACEI( CCTRCTX _T("Command processed successfully."));
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnEmailThreatsDetected(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CCTRACEI( CCTRCTX _T("Handling OnEmailThreatsDetected"));

		HRESULT hrSubmit = AddSubmission(pEmailInfo);
		if(FAILED(hrSubmit))
		{
			CCTRCTXE1(_T("Failed to submit email threats: 0x%08X"), hrSubmit);
		}
		else
		{
			CCTRCTXI0(_T("Submitted detected email threats"));
		}
        
        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");

        CString cszDetails;
        BOOL bActionRequired = FALSE, bThreatsProcessed = FALSE;
        GetEmailThreatDetails(pEmailInfo, pReturnData, cszDetails, bActionRequired, bThreatsProcessed);
        
        // do not display notifications if MCE is active
        SymMCE::ISymMceCmdLoader	mce;

        if(bActionRequired) // user action required
        {
            
            if (SYM_SUCCEEDED(mce.DoNotAlertToDesktop(CISVersion::GetProductName(), SymMCE::MCE_ALERT_CHECK_STATE)))
            {
                // What if user dismisses, or is watching a movie?
                CCTRACEI( CCTRCTX _T("User wants the alert suppressed!!"));
            }
            else
            {
                ShowEmailThreatsUI(pEmailInfo, pReturnData);
            }
        }
        else if(bThreatsProcessed) // threats were processed silently
        {
            // Set the action
            pReturnData->SetValue(AVModule::EmailScanner::AV_EMAILTHREATSDETECTEDRETURN_ACTION, (DWORD)AVModule::EmailScanner::eEmailProcessingAction_ProcessThreats);
        }
        else // done processing
        {
            if(!cszDetails.IsEmpty() && !mce.IsMceMaximized())
            {
                cc::INotifyPtr spNotifyTemp;
                hrx << spClient->CreateNotification(spNotifyTemp);

                cc::INotify3QIPtr spNotify = spNotifyTemp;
                if(!spNotify)
                {
                    CCTRACEE( CCTRCTX _T("Unable to get cc::INotify3 interface."));
                    hrx << E_UNEXPECTED;
                }

                spNotify->SetTitle(_S(IDS_APNOTIFY_TITLE));
                spNotify->SetText(cszDetails);
                spNotify->SetButton(cc::INotify::BUTTONTYPE_MOREINFO, _S(IDS_APNOTIFY_MORE_INFO_TEXT));

                spNotify->SetTimeout(CSvcMgrSingleton::GetSvcMgr()->GetNotificationDisplayTimeout());
                spNotify->SetSize(_I(IDS_APNOTIFY_WIDTH), _I(IDS_APNOTIFY_HEIGHT));
                spNotify->SetProperty(cc::INotify::PROPERTY_TOPMOST, true);
                spNotify->SetProperty(cc::INotify::PROPERTY_MOREINFO_BUTTON, true);

                spNotify->Display(NULL, NULL);
            }
            else if(cszDetails.IsEmpty())
            {
                CCTRACEE( CCTRCTX _T("cszDetails was empty."));
            }
            else // mce.IsMceMaximized() == true
            {
                CCTRACEI( CCTRCTX _T("MCE is maximized, suppressing notification."));
            }
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
}

//****************************************************************************
//****************************************************************************
void CAvSinks::GetEmailThreatDetails(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData, CString& cszDetails, BOOL& bActionRequired, BOOL &bThreatsProcessed)
{
    HRESULT hr;
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        bActionRequired = FALSE;
        bThreatsProcessed = FALSE;

        // Get the action map
        ISymBasePtr spActionsBase;
        hr = pReturnData->GetValue(AVModule::EmailScanner::AV_EMAILTHREATSDETECTEDRETURN_ITEMSTOPROCESS, AVModule::AVDataTypes::eTypeMapGuid, spActionsBase);
        if(!spActionsBase)
            hrx << E_UNEXPECTED;

        AVModule::IAVMapGuidDataQIPtr spMapActions(spActionsBase);
        if(!spMapActions)
            hrx << E_UNEXPECTED;

        // Get the threats
        ISymBasePtr spTempThreatData;
        hrx << pEmailInfo->GetValue(AVModule::EmailScanner::AV_EMAILTHREATSDETECTED_THREATS, AVModule::AVDataTypes::eTypeMapGuid, spTempThreatData);

        AVModule::IAVMapGuidDataQIPtr spThreatData = spTempThreatData;
        if(!spThreatData)
            hrx << E_UNEXPECTED;

        DWORD dwThreatCount = NULL;
        hrx << spThreatData->GetCount(dwThreatCount);
        
        HRESULT hr;
        for(DWORD dwThreatIndex = 0; dwThreatIndex < dwThreatCount; dwThreatIndex++)
        {
            GUID guidThreatId;
            hr = spThreatData->GetKeyAtIndex(dwThreatIndex, guidThreatId);
            if(FAILED(hr))
                continue;

            ISymBasePtr spTempData;
            hr = spThreatData->GetValue(guidThreatId, AVModule::AVDataTypes::eTypeMapDword, spTempData);
            if(FAILED(hr) || !spTempData)
                continue;

            AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
            if(!spMapCurThreat)
                continue;
            
            cc::IStringPtr spThreatName;
            hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_NAME, spThreatName);
            if(FAILED(hr))
                continue;

            DWORD dwOverallThreatMatrix = NULL;
            hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, dwOverallThreatMatrix);
            if(FAILED(hr))
                continue;

            DWORD dwThreatState = NULL;
            hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwThreatState);
            if(FAILED(hr))
                continue;
            
            CString cszThreatState = AVModule::ThreatTracking::ThreatRemoved(dwThreatState) ? _S(IDS_THREAT_STATE_REMOVED) : _S(IDS_THREAT_STATE_NOT_REMOVED);
            cszDetails.FormatMessage(_S(IDS_EMAILSCAN_DETAILS_THREAT_ENTRY), (LPCTSTR)CW2T(spThreatName->GetStringW()), (LPCTSTR)cszThreatState);

            if(AVModule::ThreatTracking::ThreatNotRemoved(dwThreatState))
            {
                bool bIsLowRiskThreat = S_OK == IsLowRiskThreat(dwOverallThreatMatrix);

                if(bIsLowRiskThreat && CSvcMgr::eRespondRemediate == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eEmailComponent))
                {
                    // try to resolve this threat
                    spMapActions->SetValue(guidThreatId, (DWORD)AVModule::EmailScanner::eEmailAction_Resolve);
                    bThreatsProcessed = TRUE;
                    continue;
                }
                else if(bIsLowRiskThreat && CSvcMgr::eRespondIgnore == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eEmailComponent))
                {
                    // ignore this threat
                    continue;
                }
                    
                // user needs to deal with this threat
                bActionRequired = TRUE;
            }

        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

//****************************************************************************
//****************************************************************************
void CAvSinks::OnOEHDetection(AVModule::IAVMapStrData* pOEHInfo, DWORD& dwOEHAction)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CCTRACEI( CCTRCTX _T("Handling OnOEHDetection"));

        SymMCE::ISymMceCmdLoader	mce;

        BOOL bDisplayNotification = TRUE;
        DWORD dwQuarantineResult = 0;
        HRESULT hr = pOEHInfo->GetValue(AVModule::EmailScanner::AV_EMAILOEH_QUARRESULT, dwQuarantineResult);
        if(FAILED(hr))
        {
            if(CSvcMgr::eRespondRemediate == CSvcMgrSingleton::GetSvcMgr()->GetResponseMode(CSvcMgr::eOEHComponent))
            {
                dwOEHAction = AVModule::EmailScanner::OEHACTION_Quarantine;
            }
            else if(CSvcMgr::eRespondIgnore == CSvcMgrSingleton::GetSvcMgr()->GetResponseMode(CSvcMgr::eOEHComponent))
            {
                dwOEHAction = AVModule::EmailScanner::OEHACTION_Ignore;
            }
            else if(!SYM_SUCCEEDED(mce.DoNotAlertToDesktop(CISVersion::GetProductName(), SymMCE::MCE_ALERT_CHECK_STATE)))
            {
                SessionApp::ISessionAppClientPtr spClient;
                hrx << SessionApp::CClientImpl::CreateObject(spClient);
                hrx << spClient->Initialize(L"AvProdSession");

                cc::IKeyValueCollectionPtr spCommand;
                spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
                CCTHROW_BAD_ALLOC(spCommand);

                // Serialize the AvModule data into the command data
                ISymBaseQIPtr spAvModuleData = pOEHInfo;
                spCommand->SetValue(AvProd::OnOEHDetection::eOEHInfo, spAvModuleData);
                spAvModuleData.Release();

                spCommand->SetValue(AvProd::OnOEHDetection::eOEHAction, dwOEHAction);

                CCTRACEI( CCTRCTX _T("Sending command..."));
                ISymBaseQIPtr spTempCommand = spCommand, spTempReturn;
                hrx << spClient->SendCommand(AvProd::CMDID_OnOEHDetection, spTempCommand, &spTempReturn);
                cc::IKeyValueCollectionQIPtr spReturnData = spTempReturn;
                if(!spReturnData)
                    hrx << E_UNEXPECTED;

                if(!spReturnData->GetValue(AvProd::OnOEHDetection::eOEHAction, dwOEHAction))
                    hrx << E_UNEXPECTED;

            }
            else
            {
                // If user is busy, nuke it
                CCTRACEI( CCTRCTX _T("MCE is maximized, suppressing notification and forcing Quarantine operation."));
                dwOEHAction = AVModule::EmailScanner::OEHACTION_Quarantine;
            }
            
            if(AVModule::EmailScanner::OEHACTION_Quarantine == dwOEHAction)
            {
                // Do not display notification.  There will be another callback
                //  generated with a result code
                bDisplayNotification = FALSE;
            }
        }

        if(bDisplayNotification)
        {
            (void) AddSubmission(pOEHInfo);

            AvModuleLogging::IEmailScanClientActivitiesPtr spLog;
            SYMRESULT sr = AvModuleLogging::EmailScanClientActivities_Loader::CreateObject(GETMODULEMGR(), &spLog);
            if(SYM_SUCCEEDED(sr) && spLog)
            {
                CCTRACEI( CCTRCTX _T("Generating log entry..."));
                spLog->OnOEHDetection(pOEHInfo, dwOEHAction);
            }

            CString cszDetails;
            switch(dwOEHAction)
            {
            case AVModule::EmailScanner::OEHACTION_Ignore:
                cszDetails = _S(IDS_OEHACTION_IGNORED);
                break;

            case AVModule::EmailScanner::OEHACTION_Quarantine:
                cszDetails = _S(IDS_OEHACTION_QUARANTINED);
                break;

            case AVModule::EmailScanner::OEHACTION_Abort:
                if(0 != dwQuarantineResult)
                    cszDetails = _S(IDS_OEHACTION_ABORTED_QUARANTINED);
                else
                    cszDetails = _S(IDS_OEHACTION_ABORTED);
                break;

            case AVModule::EmailScanner::OEHACTION_Authorize:
                cszDetails = _S(IDS_OEHACTION_AUTHORIZED);
                break;

            default:
                CCTRACEE( CCTRCTX _T("Unexpected OEH Action value: %d"), dwOEHAction);
                break;
            }

            if(!cszDetails.IsEmpty() && !mce.IsMceMaximized())
            {
                SessionApp::ISessionAppClientPtr spClient;
                hrx << SessionApp::CClientImpl::CreateObject(spClient);
                hrx << spClient->Initialize(L"AvProdSession");

                cc::INotifyPtr spNotifyTemp;
                hrx << spClient->CreateNotification(spNotifyTemp);

                cc::INotify3QIPtr spNotify = spNotifyTemp;
                if(!spNotify)
                {
                    CCTRACEE( CCTRCTX _T("Unable to get cc::INotify3 interface."));
                    hrx << E_UNEXPECTED;
                }

                spNotify->SetTitle(_S(IDS_APNOTIFY_TITLE));
                spNotify->SetText(cszDetails);
                spNotify->SetButton(cc::INotify::BUTTONTYPE_MOREINFO, _S(IDS_APNOTIFY_MORE_INFO_TEXT));

                spNotify->SetTimeout(CSvcMgrSingleton::GetSvcMgr()->GetNotificationDisplayTimeout());
                spNotify->SetSize(_I(IDS_APNOTIFY_WIDTH), _I(IDS_APNOTIFY_HEIGHT));
                spNotify->SetProperty(cc::INotify::PROPERTY_TOPMOST, true);
                spNotify->SetProperty(cc::INotify::PROPERTY_MOREINFO_BUTTON, true);

                spNotify->Display(NULL, NULL);
            }
            else if(cszDetails.IsEmpty())
            {
                CCTRACEW( CCTRCTX _T("Details are empty, that's weird."));

            }
            else // mce.IsMceMaximized() == true
            {
                CCTRACEI( CCTRCTX _T("MCE is maximized, suppressing notification."));
            }

        }
        
        CCTRACEI( CCTRCTX _T("Command processed successfully."));

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

void CAvSinks::OnThreatNotification(AVModule::ThreatTracking::ActionRequiredFlag flagARThreatType, AVModule::IAVMapDwordData* pThreat) throw()
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CCTRACEI( CCTRCTX _T("Handling OnThreatNotification"));

        if(AVModule::ThreatTracking::REBOOT_REQUIRED & flagARThreatType)
        {
            CSvcMgrSingleton::GetSvcMgr()->HandleAvModuleRebootRequest(pThreat);
        }
        else if(AVModule::ThreatTracking::PROCTERM_REQUIRED & flagARThreatType)
        {
            CSvcMgrSingleton::GetSvcMgr()->HandleProcessTerminationRequired(pThreat);
        }

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

void CAvSinks::OnDefinitionsChange( const wchar_t* pcwszDefsDir, 
                                    DWORD dwYear, 
                                    DWORD dwMonth, 
                                    DWORD dwDay, 
                                    DWORD dwRev)
{
    CCTRCTXI5(_T("OnDefinitionsChange() - %d %d %d %d, %s"), dwYear, dwMonth, dwDay, dwRev, pcwszDefsDir == NULL ? _T("(null)") : (LPCTSTR)CW2T(pcwszDefsDir));

    // Per Wes/AvModule, if this callback is hit, then defs have changed. 
    //  They could be older, they could be corrupt, but they have certainly changed.

    // if this is NULL, then defs are bad, don't scan
    if(!pcwszDefsDir)
        return;

    //  We should Launch Scan when defs changed.
    CCTRCTXI0(_T("Defs changed"));

    CNAVOptSettingsEx navSettings;
    if( navSettings.Init() )
    {
        DWORD dwVal = 0;
        if( SUCCEEDED(navSettings.GetValue(SCANNER_DefUpdateScan, dwVal, 0)) 
            && dwVal != 0 )
        {
            // Without cookie scan ...  Cookies will likely be found EVERY time the post-def-update
            // quick scan is performed, which would cause the scan UI to display quite often...
            // instead, we will leave cookies alone until the weekly scheduled scan (or the next user
            // run scan).
            HRESULT hr = LaunchQuickScanFromCurrentUserSession(false);
            if(FAILED(hr))
            {
                LaunchQuickScanFromService();
            }
        }
    }
    else
    {
        CCTRCTXE0 ( _T("failed settings"));
    }

    return;
}

HRESULT CAvSinks::LaunchQuickScanFromCurrentUserSession(bool bIncludeCookies)
{
    CCTRACEI( CCTRCTX _T("Launching Quick Scan from User Session"));

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");

        cc::IKeyValueCollectionPtr spCommand;
        spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
        CCTHROW_BAD_ALLOC(spCommand);
        
        cc::IStringPtr spAdditionalParameters;
        if(bIncludeCookies)
            spAdditionalParameters.Attach(ccSym::CStringImpl::CreateStringImpl(_T("/SESCAN")));
        else
            spAdditionalParameters.Attach(ccSym::CStringImpl::CreateStringImpl(_T("/SESCAN /CK-")));

        spCommand->SetValue((size_t)AvProd::OnLaunchManualScanner::eAdditionalParameters, spAdditionalParameters);

        ISymBaseQIPtr spTempCommand = spCommand, spTempReturn;
        hrx << spClient->SendCommand(AvProd::CMDID_OnLaunchManualScanner, spTempCommand, &spTempReturn);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
    
    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

HRESULT CAvSinks::LaunchQuickScanFromService()
{
    CCTRACEI( CCTRCTX _T("Launching Quick Scan from Service"));

    HRESULT hrReturn = E_FAIL;
    for(;;)    
    {
        TCHAR szNAVW32[] = _T("NAVW32.exe");
        CString cszScannerCmdLine;
        STARTUPINFO rSI;
        PROCESS_INFORMATION rPI;

        ZeroMemory(&rSI, sizeof(rSI));
        rSI.cb = sizeof(rSI);
        ZeroMemory(&rPI, sizeof(rPI));

        CNAVInfo navInfo;
        cszScannerCmdLine = navInfo.GetNAVDir();
        
        PathAppend(cszScannerCmdLine.GetBuffer(cszScannerCmdLine.GetLength() + _countof(szNAVW32) + 1), szNAVW32);
        cszScannerCmdLine.ReleaseBuffer();
        
        PathQuoteSpaces(cszScannerCmdLine.GetBuffer(cszScannerCmdLine.GetLength() + 3));
        cszScannerCmdLine.ReleaseBuffer();
        
        cszScannerCmdLine += _T(" /SESCAN /AUTOMATION");

        CCTRACEI( CCTRCTX _T("Launching Manual Scanner: %s"), cszScannerCmdLine);
        BOOL bRet = CreateProcess(NULL, (LPWSTR)(LPCWSTR)cszScannerCmdLine, NULL, NULL, FALSE, 
            CREATE_NEW_PROCESS_GROUP | IDLE_PRIORITY_CLASS, 
            NULL, NULL, &rSI, &rPI);

        if (bRet)
        {
            CloseHandle(rPI.hProcess);
            CloseHandle(rPI.hThread);
        }

        break;
    }

    return hrReturn;
}


void CAvSinks::Update(const ccSettings::CSettingsChangeEventEx *pEvent,
                    ccSettings::ISettings *pSettings) throw()
{
    CSvcMgrPtr spSvcMgr = CSvcMgrSingleton::GetSvcMgr();

    CString cszKeyName;
    DWORD dwKeyNameSize = 512;
    pEvent->GetKey(cszKeyName.GetBuffer(dwKeyNameSize), dwKeyNameSize);
    cszKeyName.ReleaseBuffer();

    if(!cszKeyName.IsEmpty())
        spSvcMgr->OnSettingsChanged(cszKeyName, pSettings);

    return;
}

void CAvSinks::DisconnectNotification(eAVDisconnectReason reason) throw()
{
    CCTRACEI( CCTRCTX _T("AvModule Disconnect received. reason: %d"), (DWORD)reason);
    CSvcMgrSingleton::GetSvcMgr()->ResetAutoProtectActivity();
    CSvcMgrSingleton::GetSvcMgr()->HandleAvModuleDisconnect();

    return;
}
