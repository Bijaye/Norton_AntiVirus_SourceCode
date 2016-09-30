#include "StdAfx.h"

#include "HPPEventsInterface.h"
#include "HPPSessionApp.h"

#include "SSPRunnableJob.h"
#include "SSPProcessEventBase.h"
#include "SSPGenericProcessEvent.h"
#include "SSPProcessHomePageChangedEvent.h"
#include "SSPChangeRegKey.h"
#include "SSPProcessIEStartupEvent.h"
#include "SSPProcessHPPFirstRun.h"
#include "SSPProcessDelayEvent.h"
#include "ProcessSysMonNotifyEvent.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"

#include "WindowsSecurityAccount.h"

#include "HPPEventEz.h"

const LPCSTR PATH_NAME_SYMANTEC = _T("Symantec");

#define WSZ_REGKEY_HKLM L"\\REGISTRY\\MACHINE"
#define WSZ_REGKEY_HKU  L"\\REGISTRY\\USER"

#define WSZ_REGKEY_IE_HOME_PAGE  L"software\\microsoft\\internet explorer\\main"
#define WSZ_REGVAL_IE_HOME_PAGE  L"start page"

CHPPSessionApp::CHPPSessionApp(void) :
m_dwLazyStartHomePageDelay(NULL),
m_bHomePageChangeEventSync(FALSE)
{
    m_pTheApp = this;
}

CHPPSessionApp::~CHPPSessionApp(void)
{
    m_pTheApp = NULL;
}
    
HRESULT CHPPSessionApp::Initialize()
{
    BOOL bRet;
    CString cszUserApplicationPath;
    
    m_dwSysMonNotifyEventCount = NULL;
    m_dwHPPHomePageChangedEvents = NULL;
    m_dwIncomingEventCount = NULL;

    IHPPSettingsInterfaceQIPtr spSettings = this;
    spSettings->ReadSettings();
    
    long lFeatureEnabled = HPP::HPP_FEATURE_STATE_ENABLED;
    spSettings->GetFeatureState(&lFeatureEnabled);
    if(HPP::HPP_FEATURE_STATE_ENABLED != lFeatureEnabled)
    {
        CCTRACEE(_T("CHPPSessionApp::Initialize - HPP feature is disabled.  Exiting.\n"));
        return E_FAIL;
    }

    bRet = SHGetSpecialFolderPath(GetDesktopWindow(), cszUserApplicationPath.GetBuffer(MAX_PATH), CSIDL_APPDATA, TRUE);
    cszUserApplicationPath.ReleaseBuffer();
    if(bRet)
    {
        //
        //  Add "SYMANTEC" to the App Data path
        ::PathAppend(cszUserApplicationPath.GetBuffer(MAX_PATH), CString(PATH_NAME_SYMANTEC));
        cszUserApplicationPath.ReleaseBuffer();
        if(-1 == ::GetFileAttributes(cszUserApplicationPath))
        {
            bRet = ::CreateDirectory(cszUserApplicationPath, NULL);
            if(!bRet)
            {
                DWORD dwErr = GetLastError();
                CCTRACEE(_T("CHPPSessionApp::Initialize() - CreateDirectory(\"%s\") failed. GetLastError() = 0x%08X"), cszUserApplicationPath, dwErr);
            }
        }

        bRet = m_EventLog.Initialize(cszUserApplicationPath, _T("PendingAlertsQueue.log"));
        if(!bRet)
        {
            CCTRACEE(_T("CHPPSessionApp::Initialize() - Unable to init pending alerts queue."));
        }
    }
    else
    {
        DWORD dwErr = GetLastError();
        CCTRACEE(_T("CHPPSessionApp::Initialize() - SHGetSpecialFolderPath(CSIDL_APPDATA) failed. GetLastError() = 0x%08X"), dwErr);
    }

    CString cszCurrentProcessPath;
    GetModuleFileName(NULL, cszCurrentProcessPath.GetBuffer(MAX_PATH), MAX_PATH);
    cszCurrentProcessPath.ReleaseBuffer();
    PathMakePretty(cszCurrentProcessPath.GetBuffer());
    cszCurrentProcessPath.ReleaseBuffer();  
    m_cszCurrentProcessPath = cszCurrentProcessPath;

    // init and start the app window
    m_wndAppWnd.Create( NULL, CRect(0,0,0,0), NULL );
    m_wndAppWnd.m_spUnknownApp = m_pTheApp;

    // create the main worker thread
    m_cWorkerThread.CreateThread();

    // set up the Event Manager subscriber
    if(ccEvtMgr::CEventManagerHelper::IsEventManagerActive())
    {
        ResetEventManagerConnections();   
    }
    else
    {
        // Event Manager is not active, sit around and wait until it is.
        m_wndAppWnd.PostMessage(HPPAPPWND::RM_CCEVTISDOWN);
    }
    
    m_spHPPEventHelper = new CHPPEventHelper;
    if(!m_spHPPEventHelper)
        return E_OUTOFMEMORY;

    CCTRACEI( _T("CHPPSessionApp::Initialize - Registering HPP client with SysMon.") );
    m_spHPPEventHelper->RegisterHPPClient();
    
    LONG lBlockBehavior = HPP::HPP_ACTION_BLOCK, lAlertFrequency = HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS;
    spSettings->GetDefaultBlockAction(&lBlockBehavior);
    spSettings->GetAlertFrequency(&lAlertFrequency);

    m_spHPPEventHelper->CheckAndAddSingleUserRules(CHPPEventEz::GetCurrentUserSID(), lBlockBehavior, lAlertFrequency);

    return S_OK;
}

void CHPPSessionApp::ConfirmSysMonRules()
{
    CCTRACEI( _T("CHPPSessionApp::ConfirmSysMonRules - Re-building SysMon rule set for HPP.") );
    
    IHPPSettingsInterfaceQIPtr spSettings = this;
    if(spSettings)
    {
        LONG lBlockBehavior, lAlertFrequency;
		spSettings->GetDefaultBlockAction(&lBlockBehavior);
		spSettings->GetAlertFrequency(&lAlertFrequency);
        m_spHPPEventHelper->ModifyHPPRules(lBlockBehavior);
    }
    return;
}

void CHPPSessionApp::Destroy()
{
    DWORD dwRet = NULL;
    
    m_spHPPEventHelper->Destroy();
    m_spHPPEventHelper.Release();

    // All done, cleanup
    m_spSubscriber.Release();

    // Ask nicely
    m_cWorkerThread.RequestExit();

    // Insure compliance
    dwRet = WaitForSingleObject(m_cWorkerThread.GetThreadHandle(), 2000);

    // Insist
    if(dwRet == WAIT_TIMEOUT)
        m_cWorkerThread.TerminateThread(NULL);

}

HRESULT CHPPSessionApp::HandleEventManagerShutdown()
{
    // Event Manager is not active, sit around and wait until it is.
    m_wndAppWnd.PostMessage(HPPAPPWND::RM_CCEVTISDOWN);

    return S_OK;
}

HRESULT CHPPSessionApp::ResetEventManagerConnections()
{
    HRESULT hr;

    m_spSubscriber.Release();

    // Event manager is active, go ahead and sign up
    m_spSubscriber = new CSubscriber;
    if(!m_spSubscriber)
    {
        CCTRACEE( _T("CHPPSessionApp::ResetEventManagerConnections - Failed to create Subscriber object.") );
        return E_FAIL;
    }
    
    hr = m_spSubscriber->Initialize(m_pTheApp);
    if(FAILED(hr))
    {
        CCTRACEE( _T("CHPPSessionApp::ResetEventManagerConnections - Unable to initialize Subscriber object.") );
        return E_FAIL;
    }

    IHPPSettingsInterfaceQIPtr spSettings = this;
    spSettings->ReadSettings();

    return S_OK;
}

HRESULT CHPPSessionApp::PushIncomingEventToQueue(ccEvtMgr::CEventEx* pEvent)
{
    // Create a generic processing event so we can process
    //  asynchronously.
    CSSPGenericProcessEvent *pProcessEvent = new CSSPGenericProcessEvent;
    if(pProcessEvent)
    {
        long lType = pEvent->GetType();
        // If this is a settings changed event, then take
        //  time to re-read settings immediately.
        if(HPP::Event_ID_OptionsChanged == lType)
        {
            IHPPSettingsInterfaceQIPtr spSettings = this;
            spSettings->ReadSettings();
        }

        CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pProcessEvent) );

        pProcessEvent->SetEventObject(pEvent);
        pProcessEvent->SetUnknownApp(m_pTheApp);

        m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);

        DWORD dwCount;
        
        if(CC_SM_NOTIFICATION_EVENT == lType)
            dwCount = InterlockedIncrement(&m_dwSysMonNotifyEventCount);
        else if(HPP::Event_ID_HPPNotifyHomePage == lType)
            dwCount = InterlockedIncrement(&m_dwHPPHomePageChangedEvents);
        else
            dwCount = -1;

        CCTRACEI(_T("HPP::Incoming Event - type: 0x%08X, count: 0x%08d"), lType, dwCount);
    }

    return S_OK;
}

HRESULT CHPPSessionApp::HandleHomePageChangedEvent(ccEvtMgr::CEventEx* pEvent)
{
    CCTRACEI(_T("CHPPSessionApp::HandleHomePageChangedEvent - Enter.\r\n"));
    HRESULT hres;
    CHPPEventCommonInterfaceQIPtr spCommon = pEvent;
    if(!spCommon)
        return E_FAIL;
	
    // set the tray icon flashing to notify user something funny is happening
#pragma message(AUTO_FUNCNAME "TODO: Make a wrapper function for flashing the tray icon.")
#ifdef _DEBUG
    //::PostMessage(m_wndAppWnd, HPPAPPWND::RM_FLASHTRAYICON, 5000, NULL);
#endif

	CreateActivityLogFromHomePageChangedEvent(pEvent);

	LONG lRegKey = 0;
	if(spCommon->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionLocation, &lRegKey))
	{
		if(HPP::HPP_LOCATIONS_HKLM == lRegKey)
			return S_OK;   //if change was to HKLM, log but don't generate an alert
	}
	
	LPWSTR wszProcessPath = NULL;
	_bstr_t cbszEventProcessPath;
	DWORD dwActorProcessId = 0, dwCurrentProcessId = GetCurrentProcessId();
	LONG lPID;
	
	if(spCommon->GetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessPath, cbszEventProcessPath.GetAddress()) &&
		spCommon->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propProcessID, &lPID))
	{
		wszProcessPath = cbszEventProcessPath.copy();
		dwActorProcessId = lPID;
		if((dwCurrentProcessId == dwActorProcessId) 
			 && (0 == CHPPEventEz::GetHPPAppPath().CompareNoCase(wszProcessPath)) )
		{
			// ignore events generated by ccApp
			 return S_OK;
		}
	
		// If this was NAVW32, then we know it means
		//  business.  Just clear any pending alerts.
		if(0 == CHPPEventEz::GetNAVW32Path().CompareNoCase(wszProcessPath))
		{
			// Clear the alert queue
			m_EventLog.ClearLog();
			
			// ignore events generated by NAVW32
			return S_OK;
		}
	}
  
    // if alert frequency is set to alert on home page change,
    //  then show the alert ui.
	LONG lAlertFrequency;
	hres = GetAlertFrequency(&lAlertFrequency);
	if(FAILED(hres))
		return hres;

    // If the user want's to see alerts when their home page changes, show them now
    if(HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_CHANGE == lAlertFrequency)
    {
        // Create a job that knows how to process the home page changed event.                                            
        CSSPProcessHomePageChangedEvent *pProcessEvent = new CSSPProcessHomePageChangedEvent;
        if(pProcessEvent)
        {
            CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pProcessEvent) );

            pProcessEvent->SetEventObject(pEvent);
            pProcessEvent->SetUnknownApp(m_pTheApp);

            // careful here, this could be circular if we're not careful
            m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
        }
    }
    else if(HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS == lAlertFrequency)
    {
        // log the event to a local log file for later retrieval

        ULONGLONG uIndex = NULL;
        DWORD dwIndexCheck = NULL;
        CError::ErrorType eError = m_EventLog.SaveEvent(*pEvent, uIndex, dwIndexCheck);
        if (eError != CError::eNoError)
        {
            CCTRACEE(_T("CHPPSessionApp::ProcessEvent() : m_EventLog.SaveEvent() != CError::eNoError"));
        }
    }

    CCTRACEI(_T("CHPPSessionApp::HandleHomePageChangedEvent - Exit.\r\n"));
    return S_OK;
}

HRESULT CHPPSessionApp::CreateActivityLogFromHomePageChangedEvent(ccEvtMgr::CEventEx* pEvent)
{

    CHPPEventCommonInterfaceQIPtr spChangedEvent = pEvent;
    if(!spChangedEvent)
	{
        return E_FAIL;
	}
    SYMRESULT sr;
    HRESULT hr;

    ccEvtMgr::CEventEx *pLogEvent;
    sr = m_spHPPEventHelper->NewHPPEvent(HPP::Event_ID_LogEntry_HomePageActivity, &pLogEvent);
    if(SYM_FAILED(sr))
	{
        return S_OK;
	}
    CHPPEventCommonInterfaceQIPtr spLogEvent = pLogEvent;
	  
	if(!pLogEvent)
	{
        return S_OK;
	}
	CHPPEventEz::PROPERTY_MAP pPropMap[] = 
	{
		{HPP::Event_LogEntry_HomePageActivity_propUserName,       HPP::Event_HPPNotifyHomePage_propUserName,             CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propUserSID,        HPP::Event_HPPNotifyHomePage_propUserSID,              CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propProcessID,      HPP::Event_HPPNotifyHomePage_propProcessID,            CHPPEventEz::PROPTYPE_LONG, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propProcessName,    HPP::Event_HPPNotifyHomePage_propProcessName,          CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propProcessPath,    HPP::Event_HPPNotifyHomePage_propProcessPath,          CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propModuleName,     HPP::Event_HPPNotifyHomePage_propModuleName,           CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propModulePath,     HPP::Event_HPPNotifyHomePage_propModulePath,           CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propStartValue,     HPP::Event_HPPNotifyHomePage_propStartValue,           CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propActionValue,    HPP::Event_HPPNotifyHomePage_propActionValue,          CHPPEventEz::PROPTYPE_BSTR, E_FAIL},
		{HPP::Event_LogEntry_HomePageActivity_propActionResult,   HPP::Event_HPPNotifyHomePage_propActionResult,         CHPPEventEz::PROPTYPE_LONG, E_FAIL},
        {HPP::Event_LogEntry_HomePageActivity_propActionLocation, HPP::Event_HPPNotifyHomePage_propActionLocation,       CHPPEventEz::PROPTYPE_LONG, E_FAIL},
	};
	hr = CHPPEventEz::CopyEventData(spLogEvent, spChangedEvent, pPropMap, CCDIMOF(pPropMap));

	if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPSessionApp::CreateActivityLogFromHomePageChangedEvent - CHPPEventEz::CopyEventData returned 0x%08X"), hr);
    }

    hr = CHPPEventEz::AddCurrentSystemTime(spLogEvent, HPP::Event_LogEntry_HomePageActivity_propTimestamp);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPSessionApp::CreateActivityLogFromHomePageChangedEvent - CHPPEventEz::AddCurrentSystemTime returned 0x%08X"), hr);
    }

    long lEventId = 0;
    if(m_spHPPEventHelper->CreateEvent(*pLogEvent, true, lEventId) != ccEvtMgr::CError::eNoError)
    {
        CCTRACEI(_T("CHPPSessionApp::CreateActivityLogFromHomePageChangedEvent - CreateEvent() failed."));
    }

    spLogEvent.Release();
    m_spHPPEventHelper->DeleteHPPEvent(pLogEvent);

   return S_OK;
}

HRESULT CHPPSessionApp::HandleBBNotifyEvent(ccEvtMgr::CEventEx* pEvent)
{
    CCTRACEI(_T("CHPPSessionApp::HandleBBNotifyEvent - Enter.\r\n"));
    SYMRESULT sr;

    IBBSysMonNotificationEventQIPtr spSysMonNotify = pEvent;
    if(!spSysMonNotify)
    {
        CCTRACEE(_T("CHPPSessionApp::HandleBBNotifyEvent - unexpected.  Could not QI for IBBSysMonNotificationEvent interface"));
        return S_OK;
    }
    
    if(!spSysMonNotify->IsClientInterested((GUID)CLSID_HPPSysMonClient))
        return S_OK;

    ISysMonNotificationData &sysMonData = spSysMonNotify->ReferenceNotificationData();

    //
    // Events we are interested in:
    //  a.) IE Process reading home page in our session
    //  b.) Changes to our HKCU home page key regardless of session
    //  c.) Changes to the HKLM home page key that originated from our session
	//

    BOOL bIEProcessReadingHomePageKey = FALSE;
    BOOL bAffectsCurrentUserHomePage = FALSE;
    BOOL bAffectsLocalMachineHomePage = FALSE;
    BOOL bActorIsIEProcess = FALSE;
    BOOL bIsInThisSession = FALSE;

    // Get the event action type
    SYSMON_ACTION_TYPE sysmonAction = bbSystemMonitorEvt::ACTION_UNKNOWN;
    sr = sysMonData.GetAction(sysmonAction);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::HandleBBNotifyEvent - sysMonData.GetAction() failed."));
    }

    // Get the target path for comparison
    LPWSTR wszActorPath = NULL;
    sr = sysMonData.GetActorProcessPath(wszActorPath);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::HandleBBNotifyEvent - sysMonData.GetTargetPath() failed."));
    }

    // Get the terminal session this originated in
    DWORD dwNotifySession = 0;
    sr = sysMonData.GetTerminalSession(dwNotifySession);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::HandleBBNotifyEvent - sysMonData.GetTerminalSession() failed."));
    }
    
    // Get the target path for comparison
    LPWSTR wszTargetPath = NULL;
    sr = sysMonData.GetTargetPath(wszTargetPath);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::HandleBBNotifyEvent - sysMonData.GetTargetPath() failed."));
        return FALSE;
    }

    // Is this regarding the HKCU home page key?
    if(0 == CHPPEventEz::GetHKCUHomePageKeyExt().CompareNoCase(wszTargetPath))
    {
        bAffectsCurrentUserHomePage = TRUE;
    }
    // Is this regarding the HKLM home page key?
    else if(0 == CHPPEventEz::GetHKLMHomePageKeyExt().CompareNoCase(wszTargetPath))
    {
        bAffectsLocalMachineHomePage= TRUE;
    }

    // Is the actor Internet Explorer?
    if(0 == CHPPEventEz::GetInternetExplorerPath().CompareNoCase(wszActorPath))
    {
        // This event originated in IE
        bActorIsIEProcess = TRUE;
    }

    // Did this event originate in our session?
    if(CHPPEventEz::GetCurrentSessionID() == dwNotifySession)
    {
        bIsInThisSession = TRUE;
    }

    // a.) IE Process reading home page in our session
    if((ACTION_READ_REGVALUE == sysmonAction) && bActorIsIEProcess && bIsInThisSession && (bAffectsCurrentUserHomePage || bAffectsLocalMachineHomePage))
    {
        CCTRACEI(_T("CHPPSessionApp::HandleBBNotifyEvent - interesting IE reading Home Page."));
        GenerateHPPIEHomePageAccessedEventFromSysMonNotify(&sysMonData);
        return S_OK;
    }

    //  b.) Changes to our HKCU home page key regardless of session
    if((ACTION_WRITE_REGVALUE == sysmonAction) && bAffectsCurrentUserHomePage)
    {
        CCTRACEI(_T("CHPPSessionApp::HandleBBNotifyEvent - interesting change event on HKCU Home Page."));
        GenerateHPPHomePageChangeEventFromSysMonNotify(&sysMonData);
        return S_OK;
    }

    //  c.) Changes to the HKLM home page key that originated from our session
    if((ACTION_WRITE_REGVALUE == sysmonAction) && bAffectsLocalMachineHomePage && bIsInThisSession)
    {
        CCTRACEI(_T("CHPPSessionApp::HandleBBNotifyEvent - interesting change event on HKLM Home Page."));
        GenerateHPPHomePageChangeEventFromSysMonNotify(&sysMonData);
        return S_OK;
    }

    CCTRACEI(_T("CHPPSessionApp::HandleBBNotifyEvent - uninteresting event"));
    CCTRACEI(_T("CHPPSessionApp::HandleBBNotifyEvent - Exit.\r\n"));
    return S_OK;

}

HRESULT CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify(ISysMonNotificationData *pSysMonNotifyData)
{
    CCTRACEI(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - Enter.\r\n"));
    HRESULT hr; 
    SYMRESULT sr;

    ccEvtMgr::CEventEx *pEvent;
    sr = m_spHPPEventHelper->NewHPPEvent(HPP::Event_ID_IE_Started, &pEvent);
    if(SYM_FAILED(sr))
        return S_OK;

    // Get the process id that this originated in
    DWORD dwProcessId = 0;
    sr = pSysMonNotifyData->GetActorPID(dwProcessId);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - sysMonData.GetActorPID() failed."));
    }

    // Get the process id that this originated in
    DWORD dwThreadId = 0;
    sr = pSysMonNotifyData->GetActorThreadID(dwThreadId);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - sysMonData.GetActorThreadID() failed."));
    }

    BOOL bWasTriggeredByOptionDialog = FindExplorerOptionsDialog(dwProcessId, dwThreadId);
    if(!bWasTriggeredByOptionDialog)
    {
        CCTRACEI(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - IE read NOT generated from Options Dialog."));
        CHPPEventCommonInterfaceQIPtr spCommonEvent = pEvent;
        if(!spCommonEvent)
            return S_OK;

        // Add current user name
        hr = CHPPEventEz::AddCurrentUserName(spCommonEvent, HPP::Event_IE_Started_propUserName);
        if(FAILED(hr))
        {
            CCTRACEE(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - CHPPEventEz::AddCurrentUserName returned 0x%08X"), hr);
        }

        // Get current user SID
        hr = CHPPEventEz::AddCurrentUserSID(spCommonEvent, HPP::Event_IE_Started_propUserSID);
        if(FAILED(hr))
        {
            CCTRACEE(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - CHPPEventEz::AddCurrentUserSID returned 0x%08X"), hr);
        }

        // Get current session id
        hr = CHPPEventEz::AddCurrentTerminalSessionId(spCommonEvent, HPP::Event_IE_Started_propSessionID);
        if(FAILED(hr))
        {
            CCTRACEE(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - CHPPEventEz::AddCurrentTerminalSessionId returned 0x%08X"), hr);
        }
        
        // Set Actor Process id
        if(!spCommonEvent->SetPropertyLONG(HPP::Event_IE_Started_propProcessID, dwProcessId))
        {
            CCTRACEE(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - unabled to set process id."));
        }

        // Set Actor Thread id
        if(!spCommonEvent->SetPropertyLONG(HPP::Event_IE_Started_propThreadID, dwThreadId))
        {
            CCTRACEE(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - unabled to set thread id."));
        }


        // reset the home page change sync
		m_bHomePageChangeEventSync = false;

        // Create a delayed job
        CSSPProcessDelayEvent *pProcessEvent = new CSSPProcessDelayEvent;
        if(pProcessEvent)
        {
            CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pProcessEvent) );

            pProcessEvent->SetEventObject(pEvent);
            pProcessEvent->SetUnknownApp(m_pTheApp);
            pProcessEvent->SetDelayValue(100);

            // careful here, this could be circular if we're not careful
            m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
        }
    
        spCommonEvent.Release();
    }
    else
    {
        CCTRACEI(_T("CHPPSessionApp::GenerateHPPIEHomePageAccessedEventFromSysMonNotify - IE read generated from Options Dialog."));
    }


    m_spHPPEventHelper->DeleteHPPEvent(pEvent);


    return S_OK;
}


HRESULT CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify(ISysMonNotificationData *pSysMonNotifyData)
{
    CCTRACEI(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - Enter.\r\n"));
    HRESULT hr; SYMRESULT sr;

    // Was this event an action on the IE start page value?
    LPWSTR wszTargetPath = NULL;
    sr = pSysMonNotifyData->GetTargetPath(wszTargetPath);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - sysMonData.GetTargetPath() failed."));
    }

	

    LPWSTR wszProcessPath = NULL;
    sr = pSysMonNotifyData->GetActorProcessPath(wszProcessPath);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - sysMonData.GetActorProcessPath() failed."));
    }

   // Get actor process id
    DWORD dwActorProcessId = 0;
    sr = pSysMonNotifyData->GetActorPID(dwActorProcessId);

    // Was this the user key or the machine key?
    BOOL bWasUserKey = (0 == wcsstr(wszTargetPath, L"REGISTRY\\MACHINE"));

    // Find out what the block/allow action was
    SYSMON_BLOCK_DISPOSITION sysmonBlockDesired = BLOCK, sysmonBlockActual = BLOCK;
    sr = pSysMonNotifyData->GetBlockDisposition(sysmonBlockDesired, sysmonBlockActual);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - sysMonData.GetBlockDisposition() failed."));
    }

    
    ULONG ulNewValueType = NULL, ulOldValueType = NULL;
    ULONG ulNewValueSize = NULL, ulOldValueSize = NULL;

    LPWSTR wszNewValue, wszOldValue;        
    sr = pSysMonNotifyData->GetRegSetValueData(ulOldValueType, (LPCVOID&)wszOldValue, ulOldValueSize, (LPCVOID&)wszNewValue, ulNewValueSize);
    if(SYM_FAILED(sr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - sysMonData.GetRegSetValueData() failed."));
    }

    if(0 == wcsicmp(wszOldValue, wszNewValue))
    {
        CCTRACEI(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - a change was detected, but the old value and new value were identical"));
        return S_OK;
    }

    ccEvtMgr::CEventEx *pEvent;
    sr = m_spHPPEventHelper->NewHPPEvent(HPP::Event_ID_HPPNotifyHomePage, &pEvent);
    if(SYM_FAILED(sr))
        return S_OK;

    CHPPEventCommonInterfaceQIPtr spCommonEvent = pEvent;
    if(!spCommonEvent)
        return S_OK;

    hr = CHPPEventEz::AddCurrentUserName(spCommonEvent, HPP::Event_HPPNotifyHomePage_propUserName);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - CHPPEventEz::AddCurrentUserName() failed.\n"));
    }

    hr = CHPPEventEz::AddCurrentUserSID(spCommonEvent, HPP::Event_HPPNotifyHomePage_propUserSID);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - CHPPEventEz::AddCurrentUserSID() failed.\n"));
    }

    hr = CHPPEventEz::AddCurrentTerminalSessionId(spCommonEvent, HPP::Event_HPPNotifyHomePage_propSessionID);
    if(FAILED(hr))
    {
        CCTRACEE(_T("CHPPSessionApp::GenerateHPPHomePageChangeEventFromSysMonNotify - CHPPEventEz::AddCurrentTerminalSessionId() failed.\n"));
    }

    // Set current process id
    if(dwActorProcessId)
    {
        spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propProcessID, dwActorProcessId);
    }

    // Set current process path and name
    if(wszProcessPath)
    {
        CStringW cwszProcessPath = wszProcessPath;
        _bstr_t cbszValue = cwszProcessPath;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessPath, cbszValue);

        PathStripPathW(cwszProcessPath.GetBuffer(MAX_PATH));
        cwszProcessPath.ReleaseBuffer();

        cbszValue = cwszProcessPath;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessName, cbszValue);
    }

    _bstr_t cbszHomePageStartValue = wszOldValue;
    _bstr_t cbszHomePageActionValue = wszNewValue;

    spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propStartValue, cbszHomePageStartValue);
    spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propActionValue, cbszHomePageActionValue);
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionLocation, bWasUserKey ? HPP::HPP_LOCATIONS_HKCU : HPP::HPP_LOCATIONS_HKLM);
    
    if(ALLOW == sysmonBlockActual)
    {
        spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, HPP::HPP_ACTION_ALLOW);
    }
    else if(BLOCK == sysmonBlockActual)
    {
        spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, HPP::HPP_ACTION_BLOCK);
    }

    // Set the home page change sync since an
    //  attempt has beeen made
    m_bHomePageChangeEventSync = true;
    HandleHomePageChangedEvent(pEvent);

    spCommonEvent.Release();
    m_spHPPEventHelper->DeleteHPPEvent(pEvent);

    return S_OK;
}

HRESULT CHPPSessionApp::HandleIEStartedEvent(ccEvtMgr::CEventEx* pEvent)
{
    CCTRACEI(_T("CHPPSessionApp::HandleIEStartedEvent - Enter.\r\n"));
	HRESULT hres;
    CHPPEventCommonInterfaceQIPtr spCommon = pEvent;
    if(!spCommon)
        return E_FAIL;
	
    // If a change event came in shortly after the read
    //  we can ignore the read.
    if(m_bHomePageChangeEventSync)
    {
        CCTRACEI(_T("CHPPSessionApp::HandleIEStartedEvent - discarding event due to recent change notification."));
        return S_OK;
    }

    LONG lAlertFrequency;
	hres = GetAlertFrequency(&lAlertFrequency);
	if(FAILED(hres))
		return hres;
    
    LONG lFirstRunDialog;
    hres = GetFirstRunDialog(&lFirstRunDialog);
    if(FAILED(hres))
        return hres;

    if(HPP::HPP_FIRST_RUN_DIALOG_SHOW == lFirstRunDialog && HPP::HPP_ALERT_FREQUENCY_NEVER != lAlertFrequency)
    {
        // Create a job that knows how to process multiple home page changed events.                                            
        CSSPProcessHPPFirstRun *pProcessEvent = new CSSPProcessHPPFirstRun;
        if(pProcessEvent)
        {
            CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pProcessEvent) );

            pProcessEvent->SetEventObject(pEvent);
            pProcessEvent->SetUnknownApp(m_pTheApp);

            // Clear the alert queue
            m_EventLog.ClearLog();

            // careful here, this could be circular if we're not careful
            m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
        }
    }    
    else if(HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS == lAlertFrequency)
    {
        // Create a job that knows how to process multiple home page changed events.                                            
        CSSPProcessIEStartupEvent *pProcessEvent = new CSSPProcessIEStartupEvent;
        if(pProcessEvent)
        {
            CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pProcessEvent) );

            pProcessEvent->SetEventObject(pEvent);
            pProcessEvent->SetUnknownApp(m_pTheApp);

            CopyPendingAlertsToJobObj(pProcessEvent);

            // careful here, this could be circular if we're not careful
            m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
        }
    }
    
    CCTRACEI(_T("CHPPSessionApp::HandleIEStartedEvent - Exit.\r\n"));
    return S_OK;
}

HRESULT CHPPSessionApp::HandleUserRequestChangeHomePageEvent(ccEvtMgr::CEventEx* pEvent)
{
    CCTRACEI(_T("CHPPSessionApp::UserRequestHandleChangeHomePageEvent - Enter.\r\n"));
    HRESULT hrReturn = E_FAIL;
    
    CHPPEventCommonInterfaceQIPtr spCommon = pEvent;
    if(!spCommon)
        return E_FAIL;

    // Get the session that this event was sent to
    LONG lSessionId = NULL;
    if(!spCommon->GetPropertyLONG(HPP::Event_HPPChangeHomePage_propSessionID, &lSessionId))
    {
        CCTRACEE(_T("CHPPSessionApp::HandleUserRequestChangeHomePageEvent - couldn't query session id.\r\n"));
        return E_FAIL;
    }

    // Confirm that this event was sent to
    // this session
    if(lSessionId != CHPPEventEz::GetCurrentSessionID())
    {
        // No need to continue processing, these event
        //  wasn't for us.
        return S_OK;
    }
    
    _bstr_t cbszNewHomePage;
	CStringW cwszCurrentUserHomePage;

    if(spCommon->GetPropertyBSTR(HPP::Event_HPPChangeHomePage_propNewHomePage, cbszNewHomePage.GetAddress()))
    {
        LONG lLocation = HPP::HPP_LOCATIONS_HKCU;
        spCommon->GetPropertyLONG(HPP::Event_HPPChangeHomePage_propLocation, &lLocation);

        hrReturn = SwitchHomePage(pEvent, cbszNewHomePage, HPP::HPP_LOCATIONS_HKCU == lLocation);
    }
    else
    {
        CCTRACEE(_T("CHPPSessionApp::HandleUserRequestChangeHomePageEvent - couldn't query home page.\r\n"));
    }

    CCTRACEI(_T("CHPPSessionApp::HandleUserRequestChangeHomePageEvent - Exit.\r\n"));
    return hrReturn;
}

HRESULT CHPPSessionApp::HandleOptionsChangedEvent(ccEvtMgr::CEventEx* pEvent)
{
	CCTRACEI(_T("CHPPSessionApp::HandleOptionsChangedEvent - Enter.\r\n"));
	
    CHPPEventCommonInterfaceQIPtr spCommon = pEvent;
    if(!spCommon)
        return E_FAIL;

    // Get the session that this event was sent to
    LONG lSessionId = NULL;
    if(!spCommon->GetPropertyLONG(HPP::Event_HPPOptionsChanged_propSessionID, &lSessionId))
    {
        CCTRACEE(_T("CHPPSessionApp::HandleOptionsChangedEvent - couldn't query session id.\r\n"));
        return E_FAIL;
    }

    // Confirm that this event was sent to
    // this session
    if(lSessionId != CHPPEventEz::GetCurrentSessionID())
    {
        // No need to continue processing, these event
        //  wasn't for us.
        return S_OK;
    }
    
    IHPPSettingsInterfaceQIPtr spSettings = this;

    LONG lStartupBehavior = HPP::HPP_STARTUP_RUN_AT_STARTUP;
    spSettings->GetRunAtStartup(&lStartupBehavior);

    if(lStartupBehavior == HPP::HPP_STARTUP_MANUAL_STARTUP)  //the user has just disabled HPP
    {
        CCTRACEI( _T("CHPPSessionApp::HandleOptionsChangedEvent - Removing SysMon rule set for HPP.") );
        m_spHPPEventHelper->RemoveHPPRules();
    }
    else
    {
        ConfirmSysMonRules();
    }
    
	return S_OK;
}

HRESULT CHPPSessionApp::ProcessEvent(ccEvtMgr::CEventEx* pEvent)
{
    CCTRACEI(_T("CHPPSessionApp::ProcessEvent - Enter.\r\n"));
    long lType = pEvent->GetType();
    switch(lType)
    {
        case HPP::Event_ID_HPPChangeHomePage:
            HandleUserRequestChangeHomePageEvent(pEvent);
            break;

		case HPP::Event_ID_OptionsChanged:
			HandleOptionsChangedEvent(pEvent);
			break;

        case HPP::Event_ID_IE_Started:
            HandleIEStartedEvent(pEvent);
            break;

        case CC_SM_NOTIFICATION_EVENT:
            HandleBBNotifyEvent(pEvent);
            break;

        default:
            CCTRACEI(_T("CHPPSessionApp::ProcessEvent - Unrecognized event type handled.\r\n"));

            // Never Commit broadcast events
            if(!pEvent->GetBroadcast() && m_spSubscriber)
            {
                m_spSubscriber->CommitEvent(*pEvent);
            }
            break;
    }

    CCTRACEI(_T("CHPPSessionApp::ProcessEvent - Exit.\r\n"));

    return S_OK;
}

HRESULT CHPPSessionApp::SwitchHomePage(ccEvtMgr::CEventEx* pEvent, LPCTSTR szNewHomePageValue, BOOL bChangeUserKey, DWORD dwThreadId)
{
    // Create a worker to switch the home page
    CSSPChangeRegKey *pTheJob = new CSSPChangeRegKey;
    if(pTheJob)
    {
        CSmartRefCountRunnableJob  smRunnable( new CMTAWorkerThread::CContainedRunnableRefCount(pTheJob) );

        pTheJob->SetEventObject(pEvent);
        pTheJob->SetUnknownApp(m_pTheApp);
        pTheJob->SetNewHomePage(szNewHomePageValue, bChangeUserKey, dwThreadId);
     
        m_cWorkerThread.GetQueuePointer()->PushItem(smRunnable);
    }

    return S_OK;
}

HRESULT CHPPSessionApp::CopyPendingAlertsToJobObj(CSSPProcessIEStartupEvent *pJob)
{

    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
	ccLib::CSingleLock lock ( &m_critEvent, INFINITE, FALSE );
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    SYSTEMTIME stStart, stStop;
    ccEvtMgr::CLogFactoryEx::EventInfo* pEventInfoArray = NULL;
    long nContextId = NULL, nEventCount = NULL;
    CError::ErrorType eError;

    ZeroMemory(&stStart, sizeof(SYSTEMTIME));
    GetSystemTime(&stStop);

    eError = m_EventLog.SearchEvents(stStart, stStop, false, 1000, nContextId, pEventInfoArray, nEventCount);
    if (eError != CError::eNoError)
    {
        CCTRACEE(_T("CMainDlg::OnBnClickedBtnReadLog() : m_EventLog.SearchEvents() != CError::eNoError"));
    }

    if(pEventInfoArray)
    {
        CCTRACEI(_T("CMainDlg::OnBnClickedBtnReadLog() : m_EventLog.SearchEvents() returned %d events"), nEventCount);
        long nCount = NULL;
        for(nCount = 0; nCount < nEventCount; nCount++)
        {
            CEventExPtr spEvent;

            eError = m_EventLog.LoadEvent(pEventInfoArray[nCount].m_uIndex, pEventInfoArray[nCount].m_dwIndexCheck, nContextId, spEvent.m_p);
            if (eError != CError::eNoError)
            {
                CCTRACEE(_T("CMainDlg::OnBnClickedBtnReadLog() : m_EventLog.LoadEvent() != CError::eNoError"));
            }
            else
            {
                pJob->m_lstEvents.push_back(spEvent);
            }
        }

        m_EventLog.DeleteSearchData(pEventInfoArray);
    }

    m_EventLog.ClearLog();
	
    return S_OK;
}

void CHPPSessionApp::ShowInternetExplorerControlPanel()
{
    typedef BOOL (WINAPI *LAUNCHCPL) (HWND);
    HMODULE hInetcpl;
    HWND hParent = NULL;
    LAUNCHCPL cpl;

    hInetcpl = LoadLibrary("inetcpl.cpl");

    if (hInetcpl == NULL)
    {
        // Error loading module -- fail as securely as possible
        return;
    }

    cpl = (LAUNCHCPL)GetProcAddress(hInetcpl, "LaunchInternetControlPanel");
    if(cpl)
    {
        cpl(hParent);
    }

    FreeLibrary(hInetcpl);

    return;
}

BOOL NavigateToHomePage(HWND hwnd)
{
    UINT WM_HTML_GETOBJECT = RegisterWindowMessage("WM_HTML_GETOBJECT");

    DWORD_PTR dwpRes = NULL;
    LRESULT lRes = SendMessageTimeout(hwnd, WM_HTML_GETOBJECT, NULL, NULL, SMTO_ABORTIFHUNG, 1000, &dwpRes);
    if(dwpRes)
    {
        //
        // Looks like we found a valid HTML window
        //

        CComPtr<IDispatch> pDispatch;
        HRESULT hr;

        // Do a little active accessibility magic and get an IDispatch interface
        if( S_OK == ObjectFromLresult(dwpRes, __uuidof(IDispatch), NULL, (void**)&pDispatch))
        {
            // Now get the Service interface to use to find the WebBrowser interface
            CComPtr<IServiceProvider> isp;
            hr = pDispatch.QueryInterface(&isp);
            if(SUCCEEDED(hr))
            {
                CComPtr<IWebBrowser2> spBrowser;
                hr = isp->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spBrowser);
                if(SUCCEEDED(hr))
                {
                    // Eureka!!
                    spBrowser->GoHome();
                }
                else
                {
                    CCTRACEE("NavigateToHomePage - Unable to get Browser interface from HTML frame - 0x%08X\r\n", hr);
                }
            }
        }
    }

    return TRUE;
}

BOOL CALLBACK CHPPSessionApp_EnumChildWindowsProc(HWND hwnd, LPARAM lParam)
{
    CString cszClassName;
    ::GetClassName(hwnd, cszClassName.GetBuffer(MAX_PATH), MAX_PATH);
    cszClassName.ReleaseBuffer();

    // Is this an internet explorer browser window?
    if(0 == cszClassName.CompareNoCase(_T("Internet Explorer_Server")))
    {
        // looks like we found one
        NavigateToHomePage(hwnd);
    }
    else
    {
        // recurse
        EnumChildWindows(hwnd, CHPPSessionApp_EnumChildWindowsProc, NULL);
    }

    return TRUE;
}

BOOL CALLBACK CHPPSessionApp_EnumThreadWindowsProc(HWND hwnd, LPARAM lParam)
{
    // browser window won't be a top level window
    EnumChildWindows(hwnd, CHPPSessionApp_EnumChildWindowsProc, NULL);
    return TRUE;
}

void CHPPSessionApp::NavigateToHomePageByThread(DWORD dwThreadId)
{
    EnumThreadWindows(dwThreadId, CHPPSessionApp_EnumThreadWindowsProc, NULL);
    return;
}

struct FIND_IEDLG_INFO {DWORD dwProcId; DWORD dwThreadId; BOOL bFound;} ;

BOOL IsWindowIEOptionDialog(HWND hwnd)
{
    //
    // An IE options dialog is not "unique", in the sense
    //  that we can just query somehow "are you an IE options dialog?".
    //  So, instead we examine the window, and see if it matches
    //  a profile that "looks" like an ie options window.
    //
    
    CString cszWindowClass;
    ::GetClassName(hwnd, cszWindowClass.GetBuffer(100), 100);
    cszWindowClass.ReleaseBuffer();

    // Is this a Win32 dialog class?
    if(0 == cszWindowClass.CompareNoCase("#32770"))
    {
        INT iRet = NULL;

        // Is this window visible?
        DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
        if(!(WS_VISIBLE & dwStyle))
        {
            CCTRACEI(_T("IsWindowIEOptionDialog() - Dialog is not visible."));
            return FALSE;
        }

        // tab control
        HWND hwndChild = ::GetDlgItem(hwnd, 0x3020 /* determined using Spy++ */ );
        iRet = ::GetClassName(hwndChild, cszWindowClass.GetBuffer(100), 100);
        cszWindowClass.ReleaseBuffer();
        if(!iRet || (0 != cszWindowClass.CompareNoCase("SysTabControl32")))
        {
            CCTRACEI(_T("IsWindowIEOptionDialog() - Dialog does not have SysTabControl32."));
            return FALSE;
        }

        // OK button
        hwndChild = ::GetDlgItem(hwnd, IDOK);
        iRet = ::GetClassName(hwndChild, cszWindowClass.GetBuffer(100), 100);
        cszWindowClass.ReleaseBuffer();
        if(!iRet || (0 != cszWindowClass.CompareNoCase("Button")))
        {
            CCTRACEI(_T("IsWindowIEOptionDialog() - Dialog does not have OK button."));
            return FALSE;
        }

        // Cancel button
        hwndChild = ::GetDlgItem(hwnd, IDCANCEL);
        iRet = ::GetClassName(hwndChild, cszWindowClass.GetBuffer(100), 100);
        cszWindowClass.ReleaseBuffer();
        if(!iRet || (0 != cszWindowClass.CompareNoCase("Button")))
        {
            CCTRACEI(_T("IsWindowIEOptionDialog() - Dialog does not have cancel button."));
            return FALSE;
        }

        // Apply button
        hwndChild = ::GetDlgItem(hwnd, 0x3021 /* determined using Spy++ */);
        iRet = ::GetClassName(hwndChild, cszWindowClass.GetBuffer(100), 100);
        cszWindowClass.ReleaseBuffer();
        if(!iRet || (0 != cszWindowClass.CompareNoCase("Button")))
        {
            CCTRACEI(_T("IsWindowIEOptionDialog() - Dialog does not have apply button."));
            return FALSE;
        }
        
        // Must be the IE options dialog
        return TRUE;
    }

    return FALSE;
}

BOOL CALLBACK FindIEOptionsDlg_EnumChildWindows(HWND hwnd, LPARAM lParam)
{
    //
    // Determine if this window is an IE options dialog
    //

    FIND_IEDLG_INFO *pFindInfo = (FIND_IEDLG_INFO*)lParam;
    
    if(IsWindowIEOptionDialog(hwnd))
    {
        pFindInfo->bFound = TRUE;
        return FALSE;
    }

    return TRUE;
}

BOOL CALLBACK FindIEOptionsDlg_EnumTopLevelWindows(HWND hwnd, LPARAM lParam)
{
    //
    //  If a window matches the proc/thread ids we're interested
    //   in, see if it is an IE options dialog, and see
    //   if any of it's children are.
    //
    
    BOOL bRet = TRUE;
    DWORD dwThreadId = NULL, dwProcId = NULL;
    FIND_IEDLG_INFO *pFindInfo = (FIND_IEDLG_INFO*)lParam;

    dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcId);
    if(dwProcId == pFindInfo->dwProcId && dwThreadId == pFindInfo->dwThreadId)    
    {
        if(IsWindowIEOptionDialog(hwnd))
        {
            pFindInfo->bFound = TRUE;
            return FALSE;
        }

        EnumChildWindows(hwnd, FindIEOptionsDlg_EnumChildWindows, (LPARAM)pFindInfo);
    }

    if(pFindInfo->bFound)
        return FALSE;

    return TRUE;
}


BOOL CHPPSessionApp::FindExplorerOptionsDialog(DWORD dwProcId, DWORD dwThreadId)
{
    //
    //  Enumerate through all top level windows, pass
    //   in the process and thread id of the windows we
    //   want to filter on.  If a window that is part
    //   of the proc/thread passed in looks like an IE
    //   options dialog, then return TRUE.
    //

    BOOL bRet = FALSE;
    FIND_IEDLG_INFO cFindInfo;
    cFindInfo.dwProcId = dwProcId;
    cFindInfo.dwThreadId = dwThreadId;
    cFindInfo.bFound = FALSE;

    EnumWindows(FindIEOptionsDlg_EnumTopLevelWindows, (LPARAM)&cFindInfo);

    return cFindInfo.bFound;
}