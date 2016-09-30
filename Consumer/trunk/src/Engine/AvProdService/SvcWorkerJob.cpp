////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SvcWorkerJob.h"
#include <AvProdWidgets.h>

#include "SvcMgr.h"

#include <SessionAppClientImpl.h>
#include <AvProdCommands.h>

using namespace AvProdSvc;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CWorkerJob::CWorkerJob()
{
	m_hRunning = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CWorkerJob::~CWorkerJob()
{
}

//****************************************************************************
//****************************************************************************
HRESULT CWorkerJob::IsRunning()
{
	HRESULT hrReturn = S_OK;

	DWORD dwRet = WaitForSingleObject(m_hRunning, 0);
	if(WAIT_TIMEOUT == dwRet)
	{
		hrReturn = S_OK;
	}
	else if(WAIT_OBJECT_0 == dwRet)
	{
		hrReturn = S_FALSE;
	}
	else
	{
		hrReturn = E_FAIL;
	}

	return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CWorkerJob::RequestExit()
{
	BOOL bFailed = SetEvent(m_hRunning);

	return bFailed ? E_FAIL : S_OK;
}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessReconnect::CProcessReconnect(void) :
m_dwWaitTimeout(2000)
{
}

//****************************************************************************
//****************************************************************************
CProcessReconnect::~CProcessReconnect(void)
{
}

//********************************************* *******************************
//****************************************************************************
HRESULT CProcessReconnect::Run()
{
    CCTRACEI(_T("CProcessReconnect::Run() waiting %d milliseonds for reconnect."), m_dwWaitTimeout);

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eMTAModel);

        DWORD dwRet = WaitForSingleObject(m_hRunning, m_dwWaitTimeout);
		if(WAIT_TIMEOUT == dwRet)
		{
			// Initialize the AvAppManager
			CSvcMgrPtr spSvcMgr = CSvcMgrSingleton::GetSvcMgr();
            
            bool bSuccess = false;

            if(SUCCEEDED(spSvcMgr->InitializeCommServer()))
            {
                if(SUCCEEDED(spSvcMgr->InitializeSettingsObserver()))
                {
                    if(SUCCEEDED(spSvcMgr->InitializeAvModuleSinks()))
                    {
                        bSuccess = true;
                    }
                }
            }
		
            if(!bSuccess)
                spSvcMgr->SpawnDelayedInitialization();
            else
                CCTRACEI( CCTRCTX _T("Initialize Phase 2 successfull."));
        }
        
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
	
	if(exceptionInfo.IsException())
	{
		CCTRACEE( CCTRCTX _T("Exception handled. Error: %s"), exceptionInfo.GetDescription());
	}

    return S_OK;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessAsyncRebootRequest::CProcessAsyncRebootRequest(void)
{
}

//****************************************************************************
//****************************************************************************
CProcessAsyncRebootRequest::~CProcessAsyncRebootRequest(void)
{
}

//********************************************* *******************************
//****************************************************************************
HRESULT CProcessAsyncRebootRequest::Run()
{
    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eMTAModel);

        // Initialize the AvAppManager
		if(CSvcMgrSingleton::GetSvcMgr())
		    CSvcMgrSingleton::GetSvcMgr()->ProcessAsyncRebootRequest();
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
	
	if(exceptionInfo.IsException())
	{
		CCTRACEE( CCTRCTX _T("Exception handled. Error: %s"), exceptionInfo.GetDescription());
	}

    return S_OK;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessAutoProtectNotification::CProcessAutoProtectNotification(void)
{
    m_dwTimeout = NULL;
    m_hCancelNotification = NULL;
    m_bWorking = FALSE;
    m_dwAvAction = CSvcMgr::eAvModuleActionSummaryInvalid;
}

//****************************************************************************
//****************************************************************************
CProcessAutoProtectNotification::~CProcessAutoProtectNotification(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CProcessAutoProtectNotification::Run()
{
    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eMTAModel);

        bool bCreateNotification = true;
        if(!m_bWorking)
        {
            HANDLE hWaitArray[] = {m_hRunning, m_hCancelNotification};
            DWORD dwRet = ::WaitForMultipleObjects(_countof(hWaitArray), hWaitArray, FALSE, m_dwTimeout);
            switch(dwRet)
            {
                case WAIT_TIMEOUT:     // timeout expired, show notification
                    bCreateNotification = true;
                    break;

                default: // anything, cancel notification
                    bCreateNotification = false;
                    break;
            }
            
            // We're done with this event
            ::SetEvent(m_hCancelNotification);
        }
        
        if(bCreateNotification)
        {
            // Initialize the AvAppManager
            if(CSvcMgrSingleton::GetSvcMgr())
                CSvcMgrSingleton::GetSvcMgr()->NotifyAutoProtectActivity(m_bWorking, m_cszThreatName, m_dwAvAction);
        }

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
	
	if(exceptionInfo.IsException())
	{
		CCTRACEE( CCTRCTX _T("Exception handled. Error: %s"), exceptionInfo.GetDescription());
	}

    return S_OK;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessAutoProtectRemediationAlert::CProcessAutoProtectRemediationAlert(void)
{
}

//****************************************************************************
//****************************************************************************
CProcessAutoProtectRemediationAlert::~CProcessAutoProtectRemediationAlert(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CProcessAutoProtectRemediationAlert::Run()
{
    CCTRACEI( CCTRCTX _T("Beginning AP remediation alert."));
    
    bool bSignalAutoProtectActivity = true;

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eMTAModel);

        do // intentional scopying
        {
            // Let's assume we mean business
            DWORD dwAction = AvProd::OnAskUserForThreatRemediation::eRemediate;

            if(CSvcMgr::eRespondIgnore == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eAutoProtectComponent))
            {
                break; // user must have changed their mind
            }

            // Get connected, and get a fresh ThreatInfo
            SYMRESULT sr;
            CCTRACEI( CCTRCTX _T("Connecting to AvModule::IAVThreatInfo"));
            AVModule::mgdAVLoader_IAVThreatInfo AvThreatInfoLoader;
            AVModule::IAVThreatInfoPtr spAvThreatInfo;
            sr = AvThreatInfoLoader.CreateObject(GETMODULEMGR(), spAvThreatInfo);
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

            CCTRACEI( CCTRCTX _T("Loading maps for custom options"));
            AVModule::mgdAVLoader_IAVMapDwordData AvMapDwordDataLoader;
            AVModule::IAVMapDwordDataPtr spAvCustomOptions;
            sr = AvMapDwordDataLoader.CreateObject(GETMODULEMGR(), spAvCustomOptions);
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

            AVModule::IAVMapDwordDataPtr spThreatInfo;
            hrx << spAvThreatInfo->GetSingleThreat(m_guidThreatTrackId, spThreatInfo);
            
            // If this threat is already removed, our work is done
            DWORD dwThreatState = NULL;
            hrx << spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwThreatState);
            if(AVModule::ThreatTracking::ThreatRemoved(dwThreatState))
            {
                CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(m_cszJobName, spThreatInfo);
                bSignalAutoProtectActivity = false;
                break;
            }

            // If we need to ask the user, do so
            if(  (CSvcMgr::eRespondAskUser == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eAutoProtectComponent))
               && m_spCommand)
            {
                SessionApp::ISessionAppClientPtr spClient;
                hrx << SessionApp::CClientImpl::CreateObject(spClient);
                hrx << spClient->Initialize(L"AvProdSession");

                ISymBasePtr spTempReturn;
                hrx << spClient->SendCommand(AvProd::CMDID_OnAskUserForThreatRemediation, m_spCommand, &spTempReturn);

                cc::IKeyValueCollectionQIPtr spReturnData = spTempReturn;
                if(!spReturnData)
                    hrx << E_UNEXPECTED;

                if(!spReturnData->GetValue(AvProd::OnAskUserForThreatRemediation::eAction, dwAction))
                    hrx << E_UNEXPECTED;

                if(dwAction == AvProd::OnAskUserForThreatRemediation::eIgnoreOnce)
                    break; // no need to continue processing
            }

            // Process exclusion choice
            if(AvProd::OnAskUserForThreatRemediation::eIgnoreAlways == dwAction)
            {
                CCTRACEI( CCTRCTX _T("Attempting to exclude this threat"));
                AVModule::IAVMapDwordDataPtr spProcessReturnData;
                hrx << spAvThreatInfo->ProcessAndWait(AVModule::ThreatProcessor::ProcessingAction_Exclude, m_guidThreatTrackId, spAvCustomOptions, spProcessReturnData);
            
            }
            else if(AvProd::OnAskUserForThreatRemediation::eRemediate == dwAction)
            {
                // If user was asked, then we can go ahead and terminate processes
                //   use the Proc Term method to remediate.
                if(  (CSvcMgr::eRespondAskUser == CSvcMgrSingleton::GetSvcMgr()->GetLowRiskResponseMode(CSvcMgr::eAutoProtectComponent))
                    && m_spCommand)
                {
                    CCTRACEI( CCTRCTX _T("Forcing process terminations"));
                    spAvCustomOptions->SetValue(AVModule::ThreatProcessor::FixIn_AllowProcTerm, true);
                }

                // This slows things down a bit, but ensures a good cleanup
                spAvCustomOptions->SetValue(AVModule::ThreatProcessor::FixIn_RunDetectAnomalies, true);

                CCTRACEI( CCTRCTX _T("Attempting to process this threat"));
                AVModule::IAVMapDwordDataPtr spProcessReturnData;
                hrx << spAvThreatInfo->ProcessAndWait(AVModule::ThreatProcessor::ProcessingAction_Fix, m_guidThreatTrackId, spAvCustomOptions, spProcessReturnData);
                
                DWORD dwResultCode = NULL;
                spProcessReturnData->GetValue(AVModule::ThreatProcessor::ProcessOut_ResultCode, dwResultCode);

                // A process termination is required, ask the user first
                if(AVModule::ThreatProcessor::FixResult_RequiresProcTerm == dwResultCode)
                {
                    spThreatInfo.Release();
                    hrx << spAvThreatInfo->GetSingleThreat(m_guidThreatTrackId, spThreatInfo);
                    CSvcMgrSingleton::GetSvcMgr()->HandleProcessTerminationRequired(spThreatInfo);
                }
            }

            // Get updated threatinfo data to send to the AP notify
            spThreatInfo.Release();
            hrx << spAvThreatInfo->GetSingleThreat(m_guidThreatTrackId, spThreatInfo);
            CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(m_cszJobName, spThreatInfo);
            bSignalAutoProtectActivity = false;

        } while(FALSE); // intentional scopying

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(bSignalAutoProtectActivity)
        CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(m_cszJobName, NULL);

    if(exceptionInfo.IsException())
    {
        CCTRACEE( CCTRCTX _T("Exception handled. Error: %s"), exceptionInfo.GetDescription());
    }

    return S_OK;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessAsyncAvModuleDisconnect::CProcessAsyncAvModuleDisconnect(void)
{
}

//****************************************************************************
//****************************************************************************
CProcessAsyncAvModuleDisconnect::~CProcessAsyncAvModuleDisconnect(void)
{
}

//********************************************* *******************************
//****************************************************************************
HRESULT CProcessAsyncAvModuleDisconnect::Run()
{
    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eMTAModel);

        // Initialize the AvAppManager
        if(CSvcMgrSingleton::GetSvcMgr())
            CSvcMgrSingleton::GetSvcMgr()->HandleAvModuleDisconnect(false);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRACEE( CCTRCTX _T("Exception handled. Error: %s"), exceptionInfo.GetDescription());
    }

    return S_OK;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessLogEmailScanResults::CProcessLogEmailScanResults(void)
{
}

//****************************************************************************
//****************************************************************************
CProcessLogEmailScanResults::~CProcessLogEmailScanResults(void)
{
}

//********************************************* *******************************
//****************************************************************************
HRESULT CProcessLogEmailScanResults::Run()
{
    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eMTAModel);

        // Initialize the AvAppManager
        if(CSvcMgrSingleton::GetSvcMgr())
            CSvcMgrSingleton::GetSvcMgr()->LogEmailScanResults(m_guidScanId, false);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRACEE( CCTRCTX _T("Exception handled. Error: %s"), exceptionInfo.GetDescription());
    }

    return S_OK;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessLogManualScanResults::CProcessLogManualScanResults(void)
{
}

//****************************************************************************
//****************************************************************************
CProcessLogManualScanResults::~CProcessLogManualScanResults(void)
{
}

//********************************************* *******************************
//****************************************************************************
HRESULT CProcessLogManualScanResults::Run()
{
	//
	//  Protect ourselves from the evil that
	//   we are capable of.
	//
	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eMTAModel);

		// Initialize the AvAppManager
		if(CSvcMgrSingleton::GetSvcMgr())
			CSvcMgrSingleton::GetSvcMgr()->LogManualScanResults(m_guidScanId, false);
	}
	CCCATCHMEM(exceptionInfo)
		CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
	{
		CCTRACEE( CCTRCTX _T("Exception handled. Error: %s"), exceptionInfo.GetDescription());
	}

	return S_OK;
};

