////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SvcMgr.h"

#include <ccInstanceFactory.h>
#include <ccSymMemoryImpl.h>
#include <ccSymMemoryStreamImpl.h>
#include <ccSymStringImpl.h>
#include <ccServiceLoader.h>
#include <ccSettingsManagerHelper.h>

#include "SvcWorkerJob.h"
#include <SessionAppClientImpl.h>
#include <AvProdCommands.h>
#include <ccSerializeInterface.h>
#include <ccSymKeyValueCollectionImpl.h>
#include <ccInstanceFactory.h>

#include <ccEraserInterface.h>

#include <AvProdLoggingLoader.h>

#include <AvProdServiceRes.h>

#include <OptNames.h>

#include <ISVersion.h>
#include <ISymMceCmdLoader.h>	// detect optional Media center components

#define SZ_EMAIL_SETTINGS_PATH                      _T("Norton Antivirus\\NAVOPTS.DAT\\NAVEMAIL")
#define SZ_EMAIL_SETTINGS_RESPONSE_MODE             _T("ResponseMode")
#define SZ_EMAIL_SETTINGS_OEH_RESPONSE_MODE         _T("OEHResponseMode")

#define SZ_AVPRODSVC_SETTINGS_PATH                  _T("Norton Antivirus\\NAVOPTS.DAT\\NAVAP")
#define SZ_AVPRODSVC_SETTINGS_NOTIFICATION_TIMEOUT  _T("NotificationTimeout")
#define SZ_AVPRODSVC_SETTINGS_RESPONSE_MODE         _T("ResponseMode")

using namespace AvProd;
using namespace AvProdSvc;

CSvcMgrPtr CSvcMgrSingleton::m_spAppMgr;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CSvcMgr::CSvcMgr(void) :
m_dwConnectionAttemptCount_CommServer(0),
m_dwConnectionAttemptCount_AvModule(0),
m_dwConnectionAttemptCount_SettingsObserver(0),
m_bCommServerConnected(false),
m_bAvModuleSinksConnected(false),
m_bSettingsObserverConnected(false),
m_dwLowRiskResponseMode_AutoProtect(eRespondAskUser), // Default to ask user
m_dwLowRiskResponseMode_EMail(eRespondAskUser), // Default to ask user
m_dwResponseMode_OEH(eRespondAskUser), // Default to ask user
m_dwNotificationDisplayTimeout(7000), // default to 7 seconds
m_bRequestRebootAlready(FALSE), m_bDisableRebootDialog(FALSE), m_bPendingRebootRequest(FALSE),
m_dwDisableRebootDialogLockCount(NULL),
m_bShouldDisplayProcessingBegin(TRUE),
m_dwProcTermDefault(0),
m_bRequestProcTermAlready(FALSE)
{
}

CSvcMgr::~CSvcMgr(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::Initialize()
{   
	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
        CCTRACEI( CCTRCTX _T("Creating AutoProtect notification worker thread."));
        hrx << m_cAutoProtectNotificationsThread.CreateThread();

        CCTRACEI( CCTRCTX _T("Creating re-connect/re-start worker thread."));
        hrx << m_cWorkerThread.CreateThread();
		
        CCTRACEI( CCTRCTX _T("Creating AvSink"));
        CAvSinks* pSinks = NULL;
        hrx << CAvSinks::CreateInstance(pSinks);
        hrx << CAvSinks::CreateObjectFromInstance(pSinks, m_spAvSink);
        hrx << pSinks->Initialize();

        CCTRACEI( CCTRCTX _T("Creating re-connect job."));
        SpawnDelayedInitialization();

		CCTRACEI( CCTRCTX _T("Initialize Phase 1 successfull."));
	}
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(_T("Unable to initialize. Error: %s"), exceptionInfo.GetDescription());
        return E_FAIL;
    }

    CCTRCTXI0(_T("Initialization completed."));
    return S_OK;
}

void CSvcMgr::SpawnDelayedInitialization(DWORD dwTimeout)
{
    // Set timer to attempt reconnect
    CProcessReconnect* pAsyncJob = new (std::nothrow) CProcessReconnect;
    if(pAsyncJob)
    {
        CSmartRefCountRunnableJob  smRunnable( new (std::nothrow) CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );
        if(smRunnable != NULL)
        {
            pAsyncJob->m_dwWaitTimeout = dwTimeout;
            m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
        }
    }
    
    return;
}

void CSvcMgr::HandleAvModuleDisconnect(bool bAsync)
{
    if(bAsync)
    {
        CProcessAsyncAvModuleDisconnect* pAsyncJob = new (std::nothrow) CProcessAsyncAvModuleDisconnect;
        if(pAsyncJob)
        {
            CSmartRefCountRunnableJob  smRunnable( new (std::nothrow) CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );
            if(smRunnable != NULL)
            {
                m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
            }
        }

        return;
    }
    
    m_spAvAutoProtect.Release();
    m_spAvThreatInfo.Release();
    m_spAvEmail.Release();
    m_spAvDefs.Release();
    m_spAvServerNotification.Release();
    m_bAvModuleSinksConnected = false;
    

    SpawnDelayedInitialization();
}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::InitializeCommServer()
{   
    // prevent unintentional re-init
    if(m_bCommServerConnected)
        return S_OK;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_dwConnectionAttemptCount_CommServer++;
        CCTRACEI( CCTRCTX _T("Init'ing SvcComm. Attempt #%d"), m_dwConnectionAttemptCount_CommServer);
        
        // Create server listener
        CCTRACEI( CCTRCTX _T("::::::Creating SvcComm"));
        hrx << CSvcComm::CreateObject(m_spCommandHandler);
        SYMRESULT sr = ccService::ccServiceMgd_IComLib::CreateObject(GETMODULEMGR(), m_spCommLib);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        CCTRACEI( CCTRCTX _T("::::::Creating Comm Channel %s"), AvProdSvc::CommChannelGeneral);
        hrx << m_spCommLib->CreateServer(AvProdSvc::CommChannelGeneral, &m_spCommServer);

        CCTRACEI( CCTRCTX _T("::::::Registering commands"));
        hrx << m_spCommServer->RegisterCommand(AvProdSvc::CMDID_DisableRebootDialog, m_spCommandHandler);
        hrx << m_spCommServer->RegisterCommand(AvProdSvc::CMDID_EnableRebootDialog, m_spCommandHandler);
		hrx << m_spCommServer->RegisterCommand(AvProdSvc::CMDID_LogManualScanResults, m_spCommandHandler);
        
        CCTRACEI( CCTRCTX _T("::::::Listen()'ing"));
        hrx << m_spCommServer->Listen();
        
        //
        //  All done connecting Comm Server
        m_bCommServerConnected = true;
        m_dwConnectionAttemptCount_CommServer = 0;
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        m_spCommServer.Release();
        m_spCommLib.Release();
        m_spCommandHandler.Release();

        CCTRACEE( CCTRCTX _T("%s"), exceptionInfo.GetFullDescription());
        CCTRACEE( CCTRCTX _T("Unable to initialize Comm Server. Connection attempt: %d, Error: %s"), m_dwConnectionAttemptCount_CommServer, exceptionInfo.GetDescription());

        // limit restarts to 30( ~
        if(30 >= m_dwConnectionAttemptCount_CommServer)
        {
            return E_FAIL;
        }
        else
        {
            CCTRACEE( CCTRCTX _T("Unable to initialize Comm Server. Cancelling re-connection operations. Connection attempt: %d, Error: %s"), m_dwConnectionAttemptCount_CommServer, exceptionInfo.GetDescription());
            return S_FALSE;
        }

    }

    CCTRACEI( CCTRCTX _T("Comm Server initialization completed."));
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::InitializeAvModuleSinks()
{   
    // prevent unintentional re-init
    if(m_bAvModuleSinksConnected)
        return S_OK;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_dwConnectionAttemptCount_AvModule++;
        CCTRACEI( CCTRCTX _T("Init'ing AvModule Sinks. Attempt #%d"), m_dwConnectionAttemptCount_AvModule);

        SYMRESULT sr;

        //
        //  Load the AvModule objects
        CCTRACEI( CCTRCTX _T("::::::Connecting to AvModule::IAVServerNotification"));
        AVModule::mgdAVLoader_IAVServerNotification AvServerNotificationLoader;
        sr = AvServerNotificationLoader.CreateObject(GETMODULEMGR(), m_spAvServerNotification);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        CCTRCTXI0(_T("::::::Connecting to AvModule::IAVDefInfo"));
        AVModule::mgdAVLoader_IAVDefInfo DefLoader;
        sr = DefLoader.CreateObject (GETMODULEMGR(), &m_spAvDefs);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        CCTRACEI( CCTRCTX _T("::::::Connecting to AvModule::IAVEMail"));
        AVModule::mgdAVLoader_IAVEmail AvEmailLoader;
        sr = AvEmailLoader.CreateObject(GETMODULEMGR(), m_spAvEmail);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        CCTRACEI( CCTRCTX _T("::::::Connecting to AvModule::IAVThreatInfo"));
        AVModule::mgdAVLoader_IAVThreatInfo AvThreatInfoLoader;
        sr = AvThreatInfoLoader.CreateObject(GETMODULEMGR(), m_spAvThreatInfo);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        CCTRACEI( CCTRCTX _T("::::::Connecting to AvModule::IAVAutoProtect"));
        AVModule::mgdAVLoader_IAVAutoProtect AvAutoProtectLoader;
        sr = AvAutoProtectLoader.CreateObject(GETMODULEMGR(), m_spAvAutoProtect);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        //
        // Set other sinks to the master sink
        AVModule::IAVServerNotificationSinkQIPtr spAvServerNotificationSink = m_spAvSink;
        AVModule::IAVDefInfoSinkQIPtr spAvDefsSink = m_spAvSink;
        AVModule::IAVEmailSinkQIPtr spEmailSink = m_spAvSink;
        AVModule::IAVThreatActionRequiredSinkQIPtr spAvActionRequiredSink = m_spAvSink;
        AVModule::IAVAutoProtectSinkQIPtr spAvApSink = m_spAvSink;

        //
        // Register each of the sinks
        CCTRCTXI0 (_T("::::::Registering Server Notification Sink"));
        hrx << m_spAvServerNotification->Register(AVModule::IAVServerNotification::AVServer_AVMain, spAvServerNotificationSink);

        CCTRCTXI0 (_T("::::::Registering Definitions Sink"));
        hrx << m_spAvDefs->RegisterSink (spAvDefsSink);

        CCTRACEI( CCTRCTX _T("::::::Registering EMail Sink"));
        hrx << m_spAvEmail->RegisterSink(spEmailSink);

        CCTRACEI( CCTRCTX _T("::::::Registering Action Required Sink"));
        hrx << m_spAvThreatInfo->RegisterThreatActionRequiredSink(AVModule::ThreatTracking::REBOOT_REQUIRED|AVModule::ThreatTracking::PROCTERM_REQUIRED, spAvActionRequiredSink);

        CCTRACEI( CCTRCTX _T("::::::Registering AutoProtect Sink"));
        hrx << m_spAvAutoProtect->RegisterAPEventSink(spAvApSink);

        //
        // All done connecting AvModule Sinks
        m_bAvModuleSinksConnected = true;
        m_dwConnectionAttemptCount_AvModule = 0;
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        m_spAvAutoProtect.Release();
        m_spAvThreatInfo.Release();
        m_spAvEmail.Release();
        m_spAvDefs.Release();
        m_spAvServerNotification.Release();

        CCTRACEE( CCTRCTX _T("%s"), exceptionInfo.GetFullDescription());
        CCTRACEE( CCTRCTX _T("Unable to initialize AvModule Sinks. Connection attempt: %d, Error: %s"), m_dwConnectionAttemptCount_AvModule, exceptionInfo.GetDescription());

        // limit restarts to 30( ~
        if(30 >= m_dwConnectionAttemptCount_AvModule)
        {
            return E_FAIL;
        }
        else
        {
            CCTRACEE( CCTRCTX _T("Unable to initialize AvModule Sinks. Cancelling re-connection operations. Connection attempt: %d, Error: %s"), m_dwConnectionAttemptCount_AvModule, exceptionInfo.GetDescription());
            return S_FALSE;
        }
    }

    CCTRACEI( CCTRCTX _T("AvModule Sinks initialization completed."));
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::InitializeSettingsObserver()
{   
    // prevent unintentional re-init
    if(m_bSettingsObserverConnected)
        return S_OK;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_dwConnectionAttemptCount_SettingsObserver++;
        CCTRACEI( CCTRCTX _T("Init'ing Settings Observer. Attempt #%d"), m_dwConnectionAttemptCount_SettingsObserver);

        // Propagate initial settings values
        hrx << InitializeSettings();

        SYMRESULT sr;
        CCTRACEI( CCTRCTX _T("::::::Creating subject loader"));
        sr = m_ISubjectLoader.CreateObject(&m_spSubject);
        if(SYM_FAILED(sr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create for settings change notification object."));
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }

        CCTRACEI( CCTRCTX _T("::::::Creating NAVEMAIL observer"));
        SettingsEventHelper::IObserverQIPtr spObserver = m_spAvSink;
        sr = m_spSubject->Attach(spObserver, ccSettings::CSettingsChangeEventEx::Global, SZ_EMAIL_SETTINGS_PATH);
        if(SYM_FAILED(sr))
        {
            CCTRACEW( CCTRCTX _T("Unable to register for settings NAVEMAIL change notifications."));
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }

        CCTRACEI( CCTRCTX _T("::::::Creating AVPRODSVC observer"));
        sr = m_spSubject->Attach(spObserver, ccSettings::CSettingsChangeEventEx::Global, SZ_AVPRODSVC_SETTINGS_PATH);
        if(SYM_FAILED(sr))
        {
            CCTRACEW( CCTRCTX _T("Unable to register for settings AVPRODSVC change notifications."));
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }

        //
        //  All done connecting Settings Observers
        m_bSettingsObserverConnected = true;
        m_dwConnectionAttemptCount_SettingsObserver = 0;
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        m_spSubject.Release();

        CCTRACEE( CCTRCTX _T("%s"), exceptionInfo.GetFullDescription());
        CCTRACEE( CCTRCTX _T("Unable to initialize Settings Observer. Connection attempt: %d, Error: %s"), m_dwConnectionAttemptCount_SettingsObserver, exceptionInfo.GetDescription());

        // limit restarts to 30( ~
        if(30 >= m_dwConnectionAttemptCount_SettingsObserver)
        {
            return E_FAIL;
        }
        else
        {
            CCTRACEE( CCTRCTX _T("Unable to initialize Settings Observer. Cancelling re-connection operations. Connection attempt: %d, Error: %s"), m_dwConnectionAttemptCount_SettingsObserver, exceptionInfo.GetDescription());
            return S_FALSE;
        }

    }

    CCTRACEI( CCTRCTX _T("Settings Observer initialization completed."));
    return S_OK;
}

HRESULT CSvcMgr::PrepareDestroy()
{
	CCTRCTXI0(L"Entering");
	
    // begin tearing down worker threads
    m_cWorkerThread.RequestExit();
	m_cAutoProtectNotificationsThread.RequestExit();

    HRESULT hr = S_OK;
	if(m_spAvAutoProtect !=NULL)
	{
		CCTRCTXI0(L"Unregistering AP Sink...");
        hr = m_spAvAutoProtect->UnregisterAPEventSink();
		if(FAILED(hr))
			CCTRCTXE1(L"Failed to unregister AP Sink: 0x%08X", hr);
	}
	if(m_spAvDefs != NULL)
	{
		CCTRCTXI0(L"Unregistering Defs Sink...");
		hr = m_spAvDefs->UnregisterSink();
		if(FAILED(hr))
			CCTRCTXE1(L"Failed to unregister def sink: 0x%08X", hr);
	}
	if(m_spAvEmail != NULL)
	{
		CCTRCTXI0(L"Unregistering email sink...");
		hr = m_spAvEmail->UnregisterSink();
		if(FAILED(hr))
			CCTRCTXE1(L"Failed to unregister email sink: 0x%08X", hr);
	}
    if(m_spAvThreatInfo != NULL)
    {
        CCTRCTXI0(L"Unregistering action required sink...");
        hr = m_spAvThreatInfo->UnregisterThreatActionRequiredSink();
        if(FAILED(hr))
            CCTRCTXE1(L"Failed to unregister action required sink: 0x%08X", hr);
    }
    if(m_spAvServerNotification != NULL)
    {
        CCTRCTXI0(L"Unregistering server notification sink...");
        m_spAvServerNotification->Unregister();
        if(FAILED(hr))
            CCTRCTXE1(L"Failed to unregister server notification sink: 0x%08X", hr);
    }

    if(m_spSubject != NULL)
	{
		CCTRCTXI0(L"Detaching settings change observer");
		SettingsEventHelper::IObserverQIPtr spObserver = m_spAvSink;
        SYMRESULT sr;

        sr = m_spSubject->Detach(spObserver, ccSettings::CSettingsChangeEventEx::Global, SZ_EMAIL_SETTINGS_PATH);
        if(SYM_FAILED(sr))
            CCTRCTXE1(L"Failed to detach NAVEMAIL observer: 0x%08X", sr);
        
        sr = m_spSubject->Detach(spObserver, ccSettings::CSettingsChangeEventEx::Global, SZ_AVPRODSVC_SETTINGS_PATH);
        if(SYM_FAILED(sr))
            CCTRCTXE1(L"Failed to detach AVPRODSVC observer: 0x%08X", sr);
	}

	CCTRCTXI0(L"Exiting");
	return S_OK;
}
//****************************************************************************
//****************************************************************************
void CSvcMgr::Destroy()
{
	CCTRACEI( CCTRCTX _T("Tearing down..."));
    m_spAvAutoProtect.Release();
    m_spAvEmail.Release();
    m_spAvDefs.Release();
    m_spSubject.Release();
    m_spAvSink.Release();

	m_cWorkerThread.RequestExit();
    m_cAutoProtectNotificationsThread.RequestExit();
    
    CCTRACEI( CCTRCTX _T("Tearing down working thread..."));
	if(WAIT_OBJECT_0 != WaitForSingleObject(m_cWorkerThread.GetThreadHandle(), 5000))
	{
		CCTRACEE( CCTRCTX _T("Worker thread did not close in time.  Terminating!!"));
		m_cWorkerThread.TerminateThread();
	}

    CCTRACEI( CCTRCTX _T("Tearing down AutoProtect notifications working thread..."));
    if(WAIT_OBJECT_0 != WaitForSingleObject(m_cAutoProtectNotificationsThread.GetThreadHandle(), 5000))
    {
        CCTRACEE( CCTRCTX _T("Worker thread did not close in time.  Terminating!!"));
        m_cAutoProtectNotificationsThread.TerminateThread();
    }

	CCTRACEI( CCTRCTX _T("Disconnecting COMM server..."));
	if(m_spCommServer)
        m_spCommServer->Disconnect();

	CCTRCTXI0(L"Releaseing svc ptrs");
	
	m_spCommServer.Release();
	m_spCommandHandler.Release();
	m_spCommLib.Release();

	CCTRACEI( CCTRCTX _T("Tear down complete..."));
}


//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::InitializeSettings()
{
    ccSettings::CSettingsManagerHelper ccSettingsHelper;
    ccSettings::ISettingsManagerPtr spSettingsManager;

    SYMRESULT sr;
    HRESULT hrReturn = E_FAIL;

    // Setup the settings manager helper
    sr = ccSettingsHelper.Create(spSettingsManager);
    if(SYM_FAILED(sr) || !spSettingsManager)
    {
        CCTRACEW( CCTRCTX _T("CSettingsManagerHelper::Create() failed. sr = 0x%08X"), sr);
        return E_FAIL;
    }

    // Retrieve our settings block
    ccSettings::ISettingsPtr spSettings;
    sr = spSettingsManager->GetSettings(SZ_EMAIL_SETTINGS_PATH, &spSettings);
    if(SYM_SUCCEEDED(sr) || spSettings)
    {
        hrReturn = SynchronizeEmailSettings(spSettings);
    }
    else
    {
        CCTRACEW( CCTRCTX _T("GetSettings(SZ_EMAIL_SETTINGS_PATH) failed. sr = 0x%08X"), sr);
    }

    spSettings.Release();
    sr = spSettingsManager->GetSettings(SZ_AVPRODSVC_SETTINGS_PATH, &spSettings);
    if(SYM_SUCCEEDED(sr) || spSettings)
    {
        hrReturn = SynchronizeAvProdSvcSettings(spSettings, false);
        if(S_FALSE == hrReturn)
        {
            spSettingsManager->PutSettings(spSettings);
        }
    }
    else
    {
        CCTRACEW( CCTRCTX _T("GetSettings(SZ_AVPRODSVC_SETTINGS_PATH) failed. sr = 0x%08X"), sr);
    }

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::OnSettingsChanged(LPCWSTR szSettingsKey, ccSettings::ISettings* pSettings)
{
    // if the ccSettings key is removed or changed, the pSettings parameter is NULL
    if(!pSettings)
        return;
    
    if(0 == _tcsicmp(szSettingsKey, SZ_EMAIL_SETTINGS_PATH))
    {
        SynchronizeEmailSettings(pSettings);
    }
    else if(0 == _tcsicmp(szSettingsKey, SZ_AVPRODSVC_SETTINGS_PATH))
    {
        SynchronizeAvProdSvcSettings(pSettings);
    }

    return;
}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::SynchronizeEmailSettings(ccSettings::ISettings *pSettings)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CCTRACEI( CCTRCTX _T("Synchronizing E-mail settings."));

        // Get E-mail Low Risk Response Mode
        for(;;)
        {
            DWORD dwAlertMode = NULL;
            DWORD dwNewResponseMode = NULL;

            // Scan Incoming
            SYMRESULT sr = pSettings->GetDword(SZ_EMAIL_SETTINGS_RESPONSE_MODE, dwAlertMode);
            if(SYM_FAILED(sr))
            {
                CCTRACEE( CCTRCTX _T("ISettings::GetDword(%s) failed. err: 0x%08X"), SZ_EMAIL_SETTINGS_RESPONSE_MODE, sr);
                break;
            }

            if(RESPONSE_MODE_AUTO == dwAlertMode)
            {
                dwNewResponseMode = eRespondRemediate;
            }
            else if(RESPONSE_MODE_IGNORE == dwAlertMode)
            {
                dwNewResponseMode = eRespondIgnore;
            }
            else if(RESPONSE_MODE_ASK == dwAlertMode)
            {
                dwNewResponseMode = eRespondAskUser;
            }
            else
            {
                CCTRACEE( CCTRCTX _T("Unexpected %s:%s value: %d"), SZ_EMAIL_SETTINGS_PATH, SZ_EMAIL_SETTINGS_RESPONSE_MODE, dwAlertMode);
                break;
            }

            if(m_dwLowRiskResponseMode_EMail != dwNewResponseMode)
            {
                m_dwLowRiskResponseMode_EMail = dwNewResponseMode;
                CCTRACEI( CCTRCTX _T("Low Risk Response Mode for E-mail has been changed to: %d"), m_dwLowRiskResponseMode_EMail);
            }

            break;
        }

        // Get OEH Response Mode
        for(;;)
        {
            DWORD dwAlertMode = NULL;
            DWORD dwNewResponseMode = NULL;

            // Scan Incoming
            SYMRESULT sr = pSettings->GetDword(SZ_EMAIL_SETTINGS_OEH_RESPONSE_MODE, dwAlertMode);
            if(SYM_FAILED(sr))
            {
                CCTRACEE( CCTRCTX _T("ISettings::GetDword(%s) failed. err: 0x%08X"), SZ_EMAIL_SETTINGS_OEH_RESPONSE_MODE, sr);
                break;
            }

            if(RESPONSE_MODE_AUTO == dwAlertMode)
            {
                dwNewResponseMode = eRespondRemediate;
            }
            else if(RESPONSE_MODE_IGNORE == dwAlertMode)
            {
                dwNewResponseMode = eRespondIgnore;
            }
            else if(RESPONSE_MODE_ASK == dwAlertMode)
            {
                dwNewResponseMode = eRespondAskUser;
            }
            else
            {
                CCTRACEE( CCTRCTX _T("Unexpected %s:%s value: %d"), SZ_EMAIL_SETTINGS_PATH, SZ_EMAIL_SETTINGS_OEH_RESPONSE_MODE, dwAlertMode);
                break;
            }

            if(m_dwResponseMode_OEH != dwNewResponseMode)
            {
                m_dwResponseMode_OEH = dwNewResponseMode;
                CCTRACEI( CCTRCTX _T("Mode for OEH has been changed to: %d"), m_dwResponseMode_OEH);
            }

            break;
        }

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        return E_FAIL;
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::SynchronizeAvProdSvcSettings(ccSettings::ISettings *pSettings, bool bReadOnly)
{
    HRESULT hrReturn = S_OK;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        for(;;)
        {
            DWORD dwTimeout = NULL;
            // Scan Incoming
            SYMRESULT sr = pSettings->GetDword(SZ_AVPRODSVC_SETTINGS_NOTIFICATION_TIMEOUT, dwTimeout);
            if(SYM_SUCCEEDED(sr) && 0 != dwTimeout) // 0 is an invalid value for the timeout
            {
                if(m_dwNotificationDisplayTimeout != dwTimeout)
                {
                    CCTRACEI( CCTRCTX _T("Changing notification timeout from %d to %d"), m_dwNotificationDisplayTimeout, dwTimeout);
                    m_dwNotificationDisplayTimeout = dwTimeout;
                }
            }
            else if(!bReadOnly) // it is OK to write values to this settings object
            {
                // must need to initialize this value
                CCTRACEI( CCTRCTX _T("Initializing notification timeout value to: %d"), m_dwNotificationDisplayTimeout);
                sr = pSettings->PutDword(SZ_AVPRODSVC_SETTINGS_NOTIFICATION_TIMEOUT, m_dwNotificationDisplayTimeout);
                if(SYM_FAILED(sr))
                {
                    CCTRACEE( CCTRCTX _T("ISettings::GetDword(%s) failed. err: 0x%08X"), SZ_AVPRODSVC_SETTINGS_NOTIFICATION_TIMEOUT, sr);
                    break;
                }

                // return S_FALSE so calling app knows to commit changes
                hrReturn = S_FALSE;
            }
            else // unexpected
            {
                CCTRACEE( CCTRCTX _T("ISettings::GetDword(%s) failed. err: 0x%08X"), SZ_AVPRODSVC_SETTINGS_NOTIFICATION_TIMEOUT, sr);
                break;
            }
            
            break;
        }

        for(;;)
        {
            DWORD dwAlertMode = NULL;
            DWORD dwNewResponseMode = NULL;

            // Response Mode
            SYMRESULT sr = pSettings->GetDword(SZ_AVPRODSVC_SETTINGS_RESPONSE_MODE, dwAlertMode);
            if(SYM_FAILED(sr))
            {
                CCTRACEE( CCTRCTX _T("ISettings::GetDword(%s) failed. err: 0x%08X"), SZ_AVPRODSVC_SETTINGS_RESPONSE_MODE, sr);
                break;
            }

            if(RESPONSE_MODE_AUTO == dwAlertMode)
            {
                dwNewResponseMode = eRespondRemediate;
            }
            else if(RESPONSE_MODE_IGNORE == dwAlertMode)
            {
                dwNewResponseMode = eRespondIgnore;
            }
            else if(RESPONSE_MODE_ASK == dwAlertMode)
            {
                dwNewResponseMode = eRespondAskUser;
            }
            else
            {
                CCTRACEE( CCTRCTX _T("Unexpected %s:%s value: %d"), SZ_AVPRODSVC_SETTINGS_PATH, SZ_AVPRODSVC_SETTINGS_RESPONSE_MODE, dwAlertMode);
                break;
            }
            
            if(m_dwLowRiskResponseMode_AutoProtect != dwNewResponseMode)
            {
                m_dwLowRiskResponseMode_AutoProtect = dwNewResponseMode;
                CCTRACEI( CCTRCTX _T("Low Risk Response Mode for AutoProtect has been changed to: %d"), m_dwLowRiskResponseMode_AutoProtect);
            }

            break;
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        return E_FAIL;
    }

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::OnDisableRebootDialog(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                            ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Command Received."));
    
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    m_dwDisableRebootDialogLockCount++;
    m_bDisableRebootDialog = TRUE;

    CCTRACEI( CCTRCTX _T("m_dwDisableRebootCount = %d"), m_dwDisableRebootDialogLockCount);
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::OnEnableRebootDialog(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                                            ISymBase* pBase, ISymBase** ppOutBase)
{
    CCTRCTXI0(_T("Command Received."));
    
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    if(0 == m_dwDisableRebootDialogLockCount)
        return;

    m_dwDisableRebootDialogLockCount--;

    CCTRACEI( CCTRCTX _T("m_dwDisableRebootCount = %d"), m_dwDisableRebootDialogLockCount);
    if(0 == m_dwDisableRebootDialogLockCount)
    {
        cc::IKeyValueCollectionQIPtr spCommand = pBase;
        if(!spCommand)
        {
            CCTRACEE( CCTRCTX _T("Unable to get command data.  Unexpected!!"));
            return;
        }

        m_bDisableRebootDialog = FALSE;

        bool bProcessPendingRebootRequests = true;
        spCommand->GetValue((size_t)AvProdSvc::EnableRebootDialog::eProcessPendingRebootRequests, bProcessPendingRebootRequests);
        if(!bProcessPendingRebootRequests)
            m_bPendingRebootRequest = FALSE;

        bool bForceRebootDialog = false;
        spCommand->GetValue((size_t)AvProdSvc::EnableRebootDialog::eForceRebootDialog, bForceRebootDialog);

        CCTRACEI( CCTRCTX _T("m_bPendingRebootRequest = %s, m_bShouldDisplayProcessingBegin = %s, bForceRebootDialog = %s"), m_bPendingRebootRequest ? _T("True") : _T("False"), m_bShouldDisplayProcessingBegin ? _T("True") : _T("False"), bForceRebootDialog ? _T("True") : _T("False"));
        if((m_bPendingRebootRequest && m_bShouldDisplayProcessingBegin) || bForceRebootDialog)
        {
            CCTRACEI( CCTRCTX _T("Generating Async Reboot Request"));
            
            // no need to hold up caller, generate async reboot request
            CProcessAsyncRebootRequest* pAsyncJob = new CProcessAsyncRebootRequest;
            if(pAsyncJob)
            {
                CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );
                m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
            }
        }
    }
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::OnLogManualScanResults(const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
								   ISymBase* pBase, ISymBase** ppOutBase)
{
	CCTRCTXI0(_T("Command Received."));
	cc::IKeyValueCollectionQIPtr spCommand = pBase;
	if(!spCommand)
	{
		CCTRACEE( CCTRCTX _T("Unable to get command data.  Unexpected!!"));
		return;
	}
	
	GUID guidScanId;
	if(spCommand->GetValue((size_t)AvProdSvc::LogManualScanResults::eManualScanInstanceId, guidScanId))
		LogManualScanResults(guidScanId);

	return;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::HandleAvModuleRebootRequest(AVModule::IAVMapDwordData* pThreat)
{
    if(m_bDisableRebootDialog || !m_bShouldDisplayProcessingBegin)
    {
        CCTRACEI( CCTRCTX _T("Reboot Dialog Disabled.  Suppressing."));
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

        m_bPendingRebootRequest = TRUE;
        return;
    }
    
    if(m_bRequestRebootAlready)
    {
        CCTRACEI( CCTRCTX _T("User already responded to reboot dialog.  Suppressing."));
        return;
    }
    
    QueryUserForReboot();

    return;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::ProcessAsyncRebootRequest()
{
    QueryUserForReboot();

    return;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::QueryUserForReboot()
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        // do not display notifications if MCE is active
        SymMCE::ISymMceCmdLoader	mce;
        BOOL bShouldSuppressUI = SYM_SUCCEEDED(mce.DoNotAlertToDesktop(CISVersion::GetProductName(), SymMCE::MCE_ALERT_CHECK_STATE));
        if(bShouldSuppressUI)
            return;

        { // intentional: scoping critical section
            //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
            ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
            //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

            m_bPendingRebootRequest = FALSE;
        }

        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");

        cc::IKeyValueCollectionPtr spCommand;
        spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
        CCTHROW_BAD_ALLOC(spCommand);

        CCTRACEI( CCTRCTX _T("Sending command..."));
        ISymBaseQIPtr spTempCommand = spCommand, spTempReturn;
        hrx << spClient->SendCommand(AvProd::CMDID_OnRebootRequired, spTempCommand, &spTempReturn);

        CCTRACEI( CCTRCTX _T("Querying return data for key/value collection..."));
        cc::IKeyValueCollectionQIPtr spReturnData = spTempReturn;
        if(!spReturnData)
            hrx << E_UNEXPECTED;

        CCTRACEI( CCTRCTX _T("Querying key/value collection for Action..."));
        DWORD dwAction = NULL;
        if(!spReturnData->GetValue(AvProd::OnRebootRequired::eAction, dwAction))
            hrx << E_UNEXPECTED;

        if(1 == dwAction)
        {
            // Reboot
            CSvcMgrSingleton::GetSvcMgr()->RestartComputer();
        }
        else if(2 == dwAction)
        {
            // Don't ask again
            m_bRequestRebootAlready = TRUE;
        }
        else
        {
            // error/unexpected
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::RestartComputer()
{
    CCTRACEI( CCTRCTX _T("Initiating restart of this computer"));
    
    HRESULT hrReturn = E_FAIL;

    for(;;)
    {
        BOOL bRet;
        HANDLE hToken; 
        TOKEN_PRIVILEGES tkp; 

        bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, & hToken);
        if(!bRet)
        {
            DWORD dwErr = GetLastError();
            CCTRACEE( CCTRCTX _T("OpenProcessToken() failed. LastError: %d"), dwErr);
            hrReturn = HRESULT_FROM_WIN32(dwErr);
            break;
        }
        // open and check the privileges for to perform the actions
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, & tkp.Privileges[0].Luid); 

        tkp.PrivilegeCount = 1; 
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

        bRet = AdjustTokenPrivileges(hToken, FALSE, & tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        if(!bRet)
        {
            DWORD dwErr = GetLastError();
            CCTRACEE( CCTRCTX _T("AdjustTokenPrivileges() failed. LastError: %d"), dwErr);
            hrReturn = HRESULT_FROM_WIN32(dwErr);
            break;
        }

        bRet = InitiateSystemShutdownEx(NULL, NULL, 0, TRUE, TRUE, SHTDN_REASON_MAJOR_SYSTEM | SHTDN_REASON_MINOR_SECURITY);
        if(!bRet)
        {
            DWORD dwErr = GetLastError();
            CCTRACEE( CCTRCTX _T("InitiateSystemShutdownEx() failed. LastError: %d"), dwErr);
            hrReturn = HRESULT_FROM_WIN32(dwErr);
            break;
        }

        CCTRACEI( CCTRCTX _T("InitiateSystemShutdownEx() success."));
        hrReturn = S_OK;
        break;
    }
    
    return hrReturn;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::ScheduleNotifyAutoProtectActivity(BOOL bWorking, LPCTSTR szThreatName, DWORD dwAvAction)
{
    // If there are any pending notifications, cancel them
    if(IsPendingAutoProtectNotification())
    {
        CCTRACEW( CCTRCTX _T("cancelling pending AP notification bWorking==%s"), bWorking ? _T("TRUE") : _T("FALSE"));
        SetEvent(m_shAutoProtectNotificationEvent);
        m_shAutoProtectNotificationEvent.CloseHandle(); // destroy event handle to cancel pending message        
    }
    
    CProcessAutoProtectNotification *pAsyncJob = new CProcessAutoProtectNotification;
    if(pAsyncJob)
    {
        CCTRACEI( CCTRCTX _T("scheduling AP notification bWorking==%s"), bWorking ? _T("TRUE") : _T("FALSE"));
        CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );

        m_shAutoProtectNotificationEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        pAsyncJob->m_hCancelNotification = m_shAutoProtectNotificationEvent;
        
        pAsyncJob->m_bWorking = bWorking;
        pAsyncJob->m_cszThreatName = szThreatName;
        pAsyncJob->m_dwTimeout = 5000;
        pAsyncJob->m_dwAvAction = dwAvAction;
        m_cAutoProtectNotificationsThread.GetQueuePointer()->PushItem(smRunnable);
    }
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::NotifyAutoProtectActivity(BOOL bWorking, LPCTSTR szThreatName, DWORD dwAvAction)
{
    if(bWorking && !m_bShouldDisplayProcessingBegin)
        return; // no need to "begin" message again
    else if(bWorking && m_bShouldDisplayProcessingBegin)
        m_bShouldDisplayProcessingBegin = FALSE; // suppress "begin" message
    else if(!bWorking)
        m_bShouldDisplayProcessingBegin = TRUE; // enable "begin" message

    if(!bWorking && (CSvcMgr::eAvActionBlocked == dwAvAction || CSvcMgr::eAvActionRedetect == dwAvAction))
        return; // this was to suppress further block events, no need to show UI

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        SymMCE::ISymMceCmdLoader	mce;

        // do not show notifications when MCE is active
        if(mce.IsMceMaximized())
        {
            CCTRACEI( CCTRCTX _T("MCE is maximized, suppressing notification."));
            return;
        }

        CCTRACEI( CCTRCTX _T("Setting up notification."));
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
        
        bool bShowUnresolvedThreatNotification = false;

        // Get product name
        CString cszNotifyText;
        if(bWorking && szThreatName && CSvcMgr::eAvActionBlocked == dwAvAction)
            cszNotifyText.FormatMessage(_S(IDS_APNOTIFY_THREAT_BLOCKED_TEXT), szThreatName);
        else if(bWorking && szThreatName)
            cszNotifyText.FormatMessage(_S(IDS_APNOTIFY_TEXT_START_THREATNAME), szThreatName);
        else if(bWorking && !szThreatName)
            cszNotifyText.FormatMessage(_S(IDS_APNOTIFY_TEXT_START_NOTHREATNAME), szThreatName);
        else // finished processing
        {
            // only check on finished messages
            DWORD dwUnresolvedThreatCount = NULL;
            CSvcMgrSingleton::GetSvcMgr()->GetCountOfUnresolvedThreats(dwUnresolvedThreatCount);

            // Find an action string
            UINT iAvAction = NULL;
            if(CSvcMgr::eAvActionIgnored == dwAvAction)
                iAvAction = IDS_APNOTIFY_AVACTION_IGNORED;
            else if(CSvcMgr::eAvActionRemoved == dwAvAction)
                iAvAction = IDS_APNOTIFY_AVACTION_REMOVED;

            // If this wasn't an action we have a string for,
            //  just use the generic notify text.
            if(!iAvAction)
                szThreatName = NULL;

            if(dwUnresolvedThreatCount) // there are unresolved threats
            {
                bShowUnresolvedThreatNotification = true;
                CCTRACEW( CCTRCTX _T("There are %d unresolved threats"), dwUnresolvedThreatCount);
            }
            else if(!szThreatName || NULL == *szThreatName)
            {
                cszNotifyText = _S(IDS_APNOTIFY_TEXT_FINISHED_NOTHREATNAME);
            }
            else
            {
                cszNotifyText.FormatMessage(_S(IDS_APNOTIFY_TEXT_FINISHED_THREATNAME), szThreatName, _S(iAvAction));
            }

            if(!bShowUnresolvedThreatNotification)
            {
                spNotify->SetProperty(cc::INotify::PROPERTY_MOREINFO_BUTTON, true);
                spNotify->SetButton(cc::INotify::BUTTONTYPE_MOREINFO, _S(IDS_APNOTIFY_MORE_INFO_TEXT));
            }
        }

        if(!bShowUnresolvedThreatNotification)
        {
            spNotify->SetTitle(_S(IDS_APNOTIFY_TITLE));
            spNotify->SetText(cszNotifyText);

            spNotify->SetTimeout(CSvcMgrSingleton::GetSvcMgr()->GetNotificationDisplayTimeout());
            spNotify->SetSize(_I(IDS_APNOTIFY_WIDTH), _I(IDS_APNOTIFY_HEIGHT));
            spNotify->SetProperty(cc::INotify::PROPERTY_TOPMOST, true);

            spNotify->Display(NULL, NULL);
            CCTRACEI( CCTRCTX _T("Normal notification complete."));
        }
        else // display unresolved threats notification
        {
            spNotify->SetTitle(_S(IDS_APNOTIFY_TITLE));
            spNotify->SetText(_S(IDS_APNOTIFY_TEXT_UNRESOLVED_THREATS));

            spNotify->SetTimeout(CSvcMgrSingleton::GetSvcMgr()->GetNotificationDisplayTimeout());
            spNotify->SetSize(_I(IDS_APNOTIFY_WIDTH), _I(IDS_APNOTIFY_HEIGHT));
            spNotify->SetProperty(cc::INotify::PROPERTY_TOPMOST, true);

            spNotify->Display(NULL, NULL);
            CCTRACEI( CCTRCTX _T("Unresolved notification complete."));
        }

        if(m_bPendingRebootRequest)
        {
            // no need to hold up this thread, generate async reboot request
            CProcessAsyncRebootRequest* pAsyncJob = new CProcessAsyncRebootRequest;
            if(pAsyncJob)
            {
                CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );
                m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
            }
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo); 

    return;
}

//****************************************************************************
//****************************************************************************
bool CSvcMgr::IsPendingAutoProtectNotification()
{
    if(!m_shAutoProtectNotificationEvent)
        return false;

    DWORD dwRet = ::WaitForSingleObject(m_shAutoProtectNotificationEvent, 0);
    if(dwRet == WAIT_TIMEOUT) // event is not signalled
    {
        // there is a pending notification
        return true;
    }

    return false;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::SubmitRemediationRequest(ISymBase* pCommand, GUID guidThreatId)
{
    
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock lock ( &m_RemediationRequestsLock, INFINITE, FALSE );
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    
    MAP_GUID_2_SYMBASEOBJ::iterator itRemediationRequest = m_mapRemediationRequests.find(guidThreatId);
    if(itRemediationRequest != m_mapRemediationRequests.end())
        (*itRemediationRequest).second = pCommand;
    else
        m_mapRemediationRequests[guidThreatId] = pCommand;
    
    return;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::GenerateRemediationRequests()
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock lock ( &m_RemediationRequestsLock, INFINITE, FALSE );
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    
    MAP_GUID_2_SYMBASEOBJ::iterator itRemediationRequest;
    for(itRemediationRequest = m_mapRemediationRequests.begin(); itRemediationRequest != m_mapRemediationRequests.end(); itRemediationRequest++)
    {
        // no need to hold up this thread, generate async remediation request
        CProcessAutoProtectRemediationAlert* pAsyncJob = new CProcessAutoProtectRemediationAlert;
        if(pAsyncJob)
        {
            pAsyncJob->m_cszJobName.Format(_T("Remediation Request: 0x%08X"), (DWORD_PTR)pAsyncJob);
            PushAutoProtectActivity(pAsyncJob->m_cszJobName, NULL);
            pAsyncJob->m_spCommand = (*itRemediationRequest).second;
            memcpy(&(pAsyncJob->m_guidThreatTrackId), (*itRemediationRequest).first, sizeof(GUID));

            CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );
            m_cAutoProtectNotificationsThread.GetQueuePointer()->PushItem(smRunnable);

            CCTRACEI( CCTRCTX _T("ApNotifyThreat job count: %d"), m_cAutoProtectNotificationsThread.GetSize());
        }
    }

    // Clear out the pending requests
    m_mapRemediationRequests.erase(m_mapRemediationRequests.begin(), m_mapRemediationRequests.end());

}

//****************************************************************************
//****************************************************************************
HRESULT CSvcMgr::GetCountOfUnresolvedThreats(DWORD &dwCount)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        HRESULT hr; SYMRESULT sr;
        AVModule::mgdAVLoader_IAVThreatInfo AvThreatInfoLoader;
        AVModule::IAVThreatInfoPtr spAvThreatInfo;
        sr = AvThreatInfoLoader.CreateObject(GETMODULEMGR(), spAvThreatInfo);
        if(SYM_FAILED(sr))
        {
            CCTRACEE( CCTRCTX _T("Problem getting AvThreatInfoLoader. sr=0x%08X"), sr);
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }

        AVModule::mgdAVLoader_IAVMapDwordData AvMapDwordDataLoader;
        AVModule::IAVMapDwordDataPtr spAvCustomOptions;
        sr = AvMapDwordDataLoader.CreateObject(GETMODULEMGR(), spAvCustomOptions);
        if(SYM_FAILED(sr))
        {
            CCTRACEE( CCTRCTX _T("Problem creating IAVMapDWordDataPtr. sr=0x%08X"), sr);
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }

        spAvCustomOptions->SetValue(AVModule::ThreatTracking::GetThreatBy_RESOLVED_UNRESOLVED_FLAG, (DWORD)0);

        AVModule::IAVArrayDataPtr spThreatData;
        hr = spAvThreatInfo->GetThreatsWithFilter(spAvCustomOptions, spThreatData);
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("Problem getting unresolved threats. hr=0x%08X"), hr);
            hrx << hr;
        }
        
        DWORD dwThreatInfoCount = NULL;
        hrx << spThreatData->GetCount(dwThreatInfoCount);

        DWORD dwUnresolvedThreatCount = 0;
        for(DWORD dwThreatInfoIndex = 0; dwThreatInfoIndex < dwThreatInfoCount; dwThreatInfoIndex++)
        {
            HRESULT hr;
            ISymBasePtr spTempData;
            hr = spThreatData->GetValue(dwThreatInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
            if(FAILED(hr) || !spTempData)
                continue;

            AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
            if(!spMapCurThreat)
                continue;
        
            // AvModule and us are not on the same wavelength when
            //  it comes to figuring resolved threats...
            if(!ThreatHasBeenResolved(spMapCurThreat))
            {
                dwUnresolvedThreatCount++;

                cc::IStringPtr spThreatName;
                spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_NAME, spThreatName);
                if(spThreatName)
                {
                    CCTRACEI( CCTRCTX _T("%s is unresolved."), (LPCTSTR)CW2T(spThreatName->GetStringW()));
                }
                else
                {
                    CCTRACEW( CCTRCTX _T("Found unresolved threat, unable to get name."));
                }
            }
        }

        dwCount = dwUnresolvedThreatCount;
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo); 

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(_T("Failed to get unresolved threat count. Error: %s"), exceptionInfo.GetDescription());
        return E_FAIL;
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
bool CSvcMgr::ThreatHasBeenResolved(AVModule::IAVMapDwordData* pThreatTrackInfo)
{
    if(!pThreatTrackInfo)
        return false; // should throw exception?

    DWORD dwThreatStateId = NULL, dwThreatTypeId = NULL;
    pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwThreatStateId);
    pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, dwThreatTypeId);
    
    // This logic is essentially mirrored in NAVLOGV/Message.cpp
    //  TODO: Should make a shared class so this doesn't get out of date...
    // 

    // Get threat resolved
    bool bThreatHasBeenResolved = false;
    if(  AVModule::ThreatTracking::ThreatRemoved(dwThreatStateId) 
        || AVModule::ThreatTracking::ThreatState_BackupOnly == dwThreatStateId 
        || AVModule::ThreatTracking::ThreatType_Email == dwThreatTypeId )
    {
        bThreatHasBeenResolved = true;
    }

    bool bManualRemovalRequired = false;
    bManualRemovalRequired = !bThreatHasBeenResolved && (AVModule::ThreatTracking::ThreatState_DoNotDelete == dwThreatStateId);

    HRESULT hr;
    ISymBasePtr spTempObj;
    
    bool bHasDependencyFlag = false;
    hr = pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVModule::AVDataTypes::eTypeMapDword, spTempObj);
    if(SUCCEEDED(hr) && spTempObj)
    {
        AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempObj;
        if(spTempObj)
        {
            DWORD dwDependencyFlag = (DWORD)-1;
            spMapCurThreat->GetValue(ccEraser::IAnomaly::Dependency, dwDependencyFlag);
            bHasDependencyFlag = dwDependencyFlag != 0 && dwDependencyFlag != (DWORD)-1;
        }
    }

    DWORD dwRequiresReboot = NULL, dwRequiresProcTerm = NULL;
    pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_REBOOTREQUIRED, dwRequiresReboot);
    pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_PROCTERMREQUIRED, dwRequiresProcTerm);

    bool bAttentionRequired =   0 != dwRequiresReboot 
        || bManualRemovalRequired
        || bHasDependencyFlag;

    bool bThreatHasBeenIgnored = false;
    if( !bThreatHasBeenResolved 
        && (AVModule::ThreatTracking::ThreatState_RemoveNotAttempted  == dwThreatStateId) 
        && !bAttentionRequired )
    {
        bThreatHasBeenIgnored = true;
    }

    return bThreatHasBeenResolved || bThreatHasBeenIgnored;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::PushWork(LPCTSTR szJobName)
{
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
        if(0 == m_lstWorkingJobs.size())
            SetWorkingState(TRUE);

        m_lstWorkingJobs.push_back(szJobName);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::PopWork(LPCTSTR szJobName)
{
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

        LST_STRINGS::iterator itJobs;
        for(itJobs = m_lstWorkingJobs.begin(); itJobs != m_lstWorkingJobs.end(); itJobs++)
        {
            if(0 == (*itJobs).Compare(szJobName))
            {
                m_lstWorkingJobs.erase(itJobs);
                break;
            }
        }

        if(0 == m_lstWorkingJobs.size())
            SetWorkingState(FALSE);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);
}


//****************************************************************************
//****************************************************************************
void CSvcMgr::ResetAutoProtectActivity()
{
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

        m_lstWorkingJobs.clear();
        SetWorkingState(FALSE);
    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::PushAutoProtectActivity(LPCTSTR szJobName, LPCTSTR szThreatName, DWORD dwAvAction)
{
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

        CCTRACEI( CCTRCTX _T("Enter(%s): AutoProtectActivityJobs count: %d"), szJobName, m_lstAutoProtectActivityJobs.size());

        PushWork(szJobName);

        if(0 == m_lstAutoProtectActivityJobs.size())
            ScheduleNotifyAutoProtectActivity(TRUE, szThreatName, dwAvAction);

        m_lstAutoProtectActivityJobs.push_back(szJobName);
        CCTRACEI( CCTRCTX _T("Exit(%s): AutoProtectActivityJobs count: %d"), szJobName, m_lstAutoProtectActivityJobs.size());
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::PopAutoProtectActivity(LPCTSTR szJobName, AVModule::IAVMapDwordData* pThreatInfo)
{
    CString cszThreatName;
    DWORD dwAvAction = CSvcMgr::eAvModuleActionSummaryInvalid;

    if(!pThreatInfo)
    {
        PopAutoProtectActivity(szJobName, cszThreatName, dwAvAction);
        return;
    }

    cc::IStringPtr spTempString;
    spTempString.Release();
    HRESULT hr = pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_NAME, spTempString);
    if(spTempString)
    {
        cszThreatName = CW2T(spTempString->GetStringW());
    }

    bool bThreatHasBeenResolved = ThreatHasBeenResolved(pThreatInfo);

    DWORD dwThreatState = NULL;
    hr = pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwThreatState);
    if(SUCCEEDED(hr))
    {
        if(bThreatHasBeenResolved && AVModule::ThreatTracking::ThreatState_RemoveNotAttempted == dwThreatState)
            dwAvAction = CSvcMgr::eAvActionIgnored;
        else if(bThreatHasBeenResolved && AVModule::ThreatTracking::ThreatRemoved(dwThreatState))
            dwAvAction = CSvcMgr::eAvActionRemoved;
    }
    
    // Increment the threat processing count
    CGuidHelper guidThreatTrackId;
    hr = pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, guidThreatTrackId);
    if(SUCCEEDED(hr) && GUID_NULL != guidThreatTrackId)
    {
        AddToThreatProcessingCount(guidThreatTrackId);
    }

    PopAutoProtectActivity(szJobName, cszThreatName, dwAvAction);
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::PopAutoProtectActivity(LPCTSTR szJobName, CString& cszThreatName, DWORD dwAvAction)
{
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        ccLib::CSingleLock csl(&m_WorkingJobsLock, INFINITE, FALSE);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

        CCTRACEI( CCTRCTX _T("Enter(%s): AutoProtectActivityJobs count: %d"), szJobName, m_lstAutoProtectActivityJobs.size());

        LST_STRINGS::iterator itJobs;
        for(itJobs = m_lstAutoProtectActivityJobs.begin(); itJobs != m_lstAutoProtectActivityJobs.end(); itJobs++)
        {
            if(0 == (*itJobs).Compare(szJobName))
            {
                m_lstAutoProtectActivityJobs.erase(itJobs);
                break;
            }
        }

        if(0 == m_lstAutoProtectActivityJobs.size())
        {
            // Generate any pending remediation requests
            GenerateRemediationRequests();
        }

        if(0 == m_lstAutoProtectActivityJobs.size())
        {

            // Reset the threat processing count, and find out
            //  if we processed more than 1 unique threat id
            size_t nThreatsProcessed = ClearThreatProcessingCount();
            if(nThreatsProcessed != 1)
            {
                // More than 1 threats were processed
                ScheduleNotifyAutoProtectActivity(FALSE, NULL, dwAvAction);
            }
            else
            {
                ScheduleNotifyAutoProtectActivity(FALSE, cszThreatName, dwAvAction);
            }

        }

        PopWork(szJobName);
        CCTRACEI( CCTRCTX _T("Exit(%s): AutoProtectActivityJobs count: %d"), szJobName, m_lstAutoProtectActivityJobs.size());
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::SetWorkingState(BOOL bWorking)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        CStringW cswInfoText;
        SessionApp::ISessionAppClientPtr spClient;
        cswInfoText.LoadString(IDS_APNOTIFY_TEXT_PROCESSING);
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");

        if(bWorking)
            hrx << spClient->SetCurrentNotifyState(SessionApp::NotifyState::eWorking, (DWORD)-1, cswInfoText.GetString());
        else
            hrx << spClient->SetCurrentNotifyState(SessionApp::NotifyState::eWorking, 0, NULL);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::LogEmailScanResults(REFGUID guidScanId, bool bAsync)
{
	if(bAsync)
	{
		// no need to hold up this thread, generate async remediation request
		CProcessLogEmailScanResults* pAsyncJob = new CProcessLogEmailScanResults;
		if(pAsyncJob)
		{
			memcpy(&(pAsyncJob->m_guidScanId), &guidScanId, sizeof(GUID));

			CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );
			m_cAutoProtectNotificationsThread.GetQueuePointer()->PushItem(smRunnable);
		}
		
		return;
	}

	AvModuleLogging::IEmailScanClientActivitiesPtr spLog;
	SYMRESULT sr = AvModuleLogging::EmailScanClientActivities_Loader::CreateObject(GETMODULEMGR(), &spLog);
	if(SYM_FAILED(sr) || !spLog)
		return;

	spLog->LogEmailScanResults(guidScanId);
    return;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::LogManualScanResults(REFGUID guidScanId, bool bAsync)
{
	if(bAsync)
	{
		// no need to hold up this thread, generate async remediation request
		CProcessLogManualScanResults* pAsyncJob = new CProcessLogManualScanResults;
		if(pAsyncJob)
		{
			memcpy(&(pAsyncJob->m_guidScanId), &guidScanId, sizeof(GUID));

			CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pAsyncJob) );
			m_cAutoProtectNotificationsThread.GetQueuePointer()->PushItem(smRunnable);
		}
		
		return;
	}

	AvModuleLogging::IManualScanClientActivitiesPtr spLog;
	SYMRESULT sr = AvModuleLogging::ManualScanClientActivities_Loader::CreateObject(GETMODULEMGR(), &spLog);
	if(SYM_FAILED(sr) || !spLog)
		return;

	spLog->LogManualScanResults(guidScanId);
    return;
}

//****************************************************************************
//****************************************************************************
void CSvcMgr::AddToThreatProcessingCount(REFGUID guidThreatTrackId)
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock csl(&m_ThreatProcessingCountLock, INFINITE, FALSE);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    
    MAP_GUID_2_DWORD::iterator itThreatProcessingCount = m_mapThreatProcessingCount.find(guidThreatTrackId);
    if(itThreatProcessingCount != m_mapThreatProcessingCount.end())
        (*itThreatProcessingCount).second++;
    else
        m_mapThreatProcessingCount[guidThreatTrackId] = 1;

    return;
}

//****************************************************************************
//****************************************************************************
size_t CSvcMgr::ClearThreatProcessingCount()
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock csl(&m_ThreatProcessingCountLock, INFINITE, FALSE);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    size_t nReturn = m_mapThreatProcessingCount.size();
    m_mapThreatProcessingCount.erase(m_mapThreatProcessingCount.begin(), m_mapThreatProcessingCount.end());
    return nReturn;
}

void CSvcMgr::HandleProcessTerminationRequired(AVModule::IAVMapDwordData* pThreat)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        HRESULT hr; SYMRESULT sr;
        DWORD dwAction = m_dwProcTermDefault;

        // do not display notifications if MCE is active
        SymMCE::ISymMceCmdLoader	mce;
        BOOL bShouldSuppressUI = SYM_SUCCEEDED(mce.DoNotAlertToDesktop(CISVersion::GetProductName(), SymMCE::MCE_ALERT_CHECK_STATE));

        DWORD dwDetectedBy = -1;
        bool bSignalAutoProtectActivity = false;

        // If this was generated by an AP event, then we need to
        //  signal auto-protect activity
        hr = pThreat->GetValue(AVModule::ThreatTracking::TrackingData_SCANNERTYPES_DETECTED_BY, dwDetectedBy);
        if(AVModule::ThreatTracking::ScannerTypeAutoProtect == dwDetectedBy)
        {
            CSvcMgrSingleton::GetSvcMgr()->PushAutoProtectActivity(_T("ProcTerm"), NULL);
            bSignalAutoProtectActivity = true;
        }

        if(!m_bRequestProcTermAlready && !bShouldSuppressUI)
        {
            SessionApp::ISessionAppClientPtr spClient;
            hrx << SessionApp::CClientImpl::CreateObject(spClient);
            hrx << spClient->Initialize(L"AvProdSession");

            cc::IKeyValueCollectionPtr spCommand;
            spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
            CCTHROW_BAD_ALLOC(spCommand);

            // Serialize the AvModule data into the command data
            ISymBaseQIPtr spAvModuleData = pThreat;
            if(spAvModuleData)
            {
                spCommand->SetValue(AvProd::OnProcTermRequired::eThreatInfo, spAvModuleData);
                spAvModuleData.Release();
            }

            CCTRACEI( CCTRCTX _T("Sending command..."));
            ISymBaseQIPtr spTempCommand = spCommand, spTempReturn;
            hrx << spClient->SendCommand(AvProd::CMDID_OnProcTermRequired, spTempCommand, &spTempReturn);

            CCTRACEI( CCTRCTX _T("Querying return data for key/value collection..."));
            cc::IKeyValueCollectionQIPtr spReturnData = spTempReturn;
            if(!spReturnData)
                hrx << E_UNEXPECTED;

            CCTRACEI( CCTRCTX _T("Querying key/value collection for Action..."));
            if(!spReturnData->GetValue(AvProd::OnProcTermRequired::eAction, dwAction))
                hrx << E_UNEXPECTED;
        }
        else
        {
            if(bShouldSuppressUI)
                CCTRACEI( CCTRCTX _T("MCE is active, suppressing UI"));

            CCTRACEI( CCTRCTX _T("Skipping notification."));
        }

        if(0x01 & dwAction)
        {
            CCTRACEI( CCTRCTX _T("User action was 1, attempting threat processing."));
            GUID guidThreatId = GUID_NULL;
            hr = pThreat->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, guidThreatId);
            if(SUCCEEDED(hr))
            {
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

                CCTRACEI( CCTRCTX _T("Forcing process terminations"));
                spAvCustomOptions->SetValue(AVModule::ThreatProcessor::FixIn_AllowProcTerm, true);
                spAvCustomOptions->SetValue(AVModule::ThreatProcessor::FixIn_RunDetectAnomalies, true);

                CCTRACEI( CCTRCTX _T("Attempting to process this threat"));
                AVModule::IAVMapDwordDataPtr spReturnData;
                hr = spAvThreatInfo->ProcessAndWait(AVModule::ThreatProcessor::ProcessingAction_Fix, guidThreatId, spAvCustomOptions, spReturnData);

                if(bSignalAutoProtectActivity)
                {
                    // Get updated threatinfo data to send to the AP notify
                    AVModule::IAVMapDwordDataPtr spThreatInfo;
                    spAvThreatInfo->GetSingleThreat(guidThreatId, spThreatInfo);

                    if(spThreatInfo)
                    {
                        CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(_T("ProcTerm"), spThreatInfo);
                        bSignalAutoProtectActivity = false;
                    }
                }

                CCTRACEI( CCTRCTX _T("Threat processing re-started"));
            }
        }
        else
        {
            CCTRACEE( CCTRCTX _T("Unexpected action value: %d"), dwAction);
        }

        if(0x10 & dwAction)
        {
            CCTRACEI( CCTRCTX _T("User chose not to be bugged with dialogs anymore"));
            m_bRequestProcTermAlready = TRUE;
            if(0x01 & dwAction)
                m_dwProcTermDefault = 0x01;
            else
                m_dwProcTermDefault = 0x00;
        }

        // if user cancelled an AP proc-term, signal that
        //  we're done processing
        if(bSignalAutoProtectActivity)
            CSvcMgrSingleton::GetSvcMgr()->PopAutoProtectActivity(_T("ProcTerm"), pThreat);

    }
    CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

    return;
}

