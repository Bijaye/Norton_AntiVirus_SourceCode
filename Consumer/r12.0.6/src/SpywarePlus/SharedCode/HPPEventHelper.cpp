#include "StdAfx.h"
#include "HPPEventHelper.h"
#include "SymProtectEventsLoader.h"

#include "HPPEventsInterface.h"
#include "BBSystemMonitorEventsInterface.h"

#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"

#include "HPPEventEz.h"

using namespace cc;
using namespace ccEvtMgr;
using namespace bbSystemMonitorEvt;

#define WSZ_REGKEY_HKLM L"\\REGISTRY\\MACHINE"
#define WSZ_REGKEY_HKU  L"\\REGISTRY\\USER"

#define WSZ_REGKEY_IE_HOME_PAGE  L"software\\microsoft\\internet explorer\\main"
#define WSZ_REGVAL_IE_HOME_PAGE  L"start page"

#define HPP_CCSETTINGS_USERLIST_KEY L"Norton Antivirus\\HPP\\AllUsersList"

// Event timeout after which the helper functions will return errors
static const int HPPEEVENTHELPER_EVENT_TIMEOUT_MS = 60000;
const TCHAR g_cszCCApp[]        = _T("\\ccApp.exe");      // AP agent exe

// {78561DC2-BC89-4569-BB57-18FEE075FF85}
const GUID CLSID_HPPSysMonClient = 
{0x78561dc2, 0xbc89, 0x4569, {0xbb, 0x57, 0x18, 0xfe, 0xe0, 0x75, 0xff, 0x85}};

// {9681C3F5-1C4C-4b7a-9922-7B55DA1DB27E}
static const GUID CLSID_HPPPlaceholderSysMonClient = 
{ 0x9681c3f5, 0x1c4c, 0x4b7a, { 0x99, 0x22, 0x7b, 0x55, 0xda, 0x1d, 0xb2, 0x7e } };

CHPPEventHelper::CHPPEventHelper(void) :
m_nProviderId( 0 ),
m_bInit(false)
{
}

CHPPEventHelper::~CHPPEventHelper(void)
{
    Destroy();
}

///////////////////////////////////////////////////////////////////////////////
// CHPPEventHelper::OnEvent()

ccEvtMgr::CError::ErrorType CHPPEventHelper::OnEvent(long nProviderId, const ccEvtMgr::CEventEx& Event)
{
    // No work for us here.
    return ccEvtMgr::CError::eNoError;
}

///////////////////////////////////////////////////////////////////////
////////
// CHPPEventHelper::OnShutdown()

ccEvtMgr::CError::ErrorType CHPPEventHelper::OnShutdown()
{
    CCTRACEI(_T("CHPPEventHelper::OnShutdown - ccEvtMgr shutting down."));

    // Event manager is shutting down.  	
    m_bInit = false;

    return ccEvtMgr::CError::eNoError;
}


///////////////////////////////////////////////////////////////////////////////
// CHPPEventHelper::initialize()

SYMRESULT CHPPEventHelper::Initialize(void)
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock lock ( &m_critEvent, INFINITE, FALSE );
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    if(!ccEvtMgr::CEventManagerHelper::IsEventManagerActive())
        return SYMERR_UNKNOWN;
    
    // Load the path to the event factory - SPPEvents.dll
    char* EVENT_FACTORY_ARRAY[2];

    ATL::CString cszHomePageEventsDll, cszSysMonEventsDll;

#pragma message(AUTO_FUNCNAME "TODO: fix up PathProvider to pick up correct dir for NAV")
    size_t iSize = MAX_PATH;
    //ccSym::CModulePathProvider::GetPath(cszHomePageEventsDll.GetBuffer(iSize), iSize);
	ccSym::CNAVPathProvider::GetPath(cszHomePageEventsDll.GetBuffer(iSize), iSize);
    cszHomePageEventsDll.ReleaseBuffer();

    DWORD dwSize = iSize + _tcslen(HPP::sz_HPPEventHelper_dll) + 1;
    PathAppend(cszHomePageEventsDll.GetBuffer(dwSize), HPP::sz_HPPEventHelper_dll);

    iSize = MAX_PATH;
    ccSym::CBBPathProvider::GetPath(cszSysMonEventsDll.GetBuffer(iSize), iSize);
    cszSysMonEventsDll.ReleaseBuffer();

    dwSize = iSize + _tcslen(SymProtectEvt::sz_SymProtectEvents_dll) + 1;
    PathAppend(cszSysMonEventsDll.GetBuffer(dwSize), SymProtectEvt::sz_SymProtectEvents_dll);

    EVENT_FACTORY_ARRAY[0] = (LPTSTR)(LPCTSTR)cszHomePageEventsDll;
    EVENT_FACTORY_ARRAY[1] = (LPTSTR)(LPCTSTR)cszSysMonEventsDll;


    // Connect to the Event Manager
    BOOL bAllSucceeded = TRUE;

    for(;;)
    {
        // Load the Event factories
        if(m_pEventFactories.Create(EVENT_FACTORY_ARRAY, CCDIMOF(EVENT_FACTORY_ARRAY)) == FALSE)
        {
            bAllSucceeded = FALSE;
            break;
        }

        // Create a ProxyFactory       
        if(SYM_FAILED(m_ProxyFactoryLoader.CreateObject(&m_pProxyFactory)))
        {
            bAllSucceeded = FALSE;
            break;
        }

        // Create an instance of the event manager with this factory
        if(m_EventManager.Create(&m_pEventFactories, m_pProxyFactory) == FALSE)
        {
            bAllSucceeded = FALSE;
            break;
        }

        // Register all the events for this factory
        if(CProviderHelper::Create(&m_EventManager, FALSE) == FALSE)
        {
            bAllSucceeded = FALSE;
            break;
        }
        
        break;
    }

    if(!bAllSucceeded)
    {
        Destroy();
        return SYMERR_UNKNOWN;
    }


    // All is well
    return SYM_OK;
}

SYMRESULT CHPPEventHelper::Destroy()
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    ccLib::CSingleLock lock ( &m_critEvent, INFINITE, FALSE );
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

    // Destroy in reverse order
    CProviderHelper::Destroy();
    m_EventManager.Destroy();
    m_pProxyFactory.Release();
    m_pEventFactories.Destroy();

    return SYM_OK;
}

SYMRESULT CHPPEventHelper::NewEvent(long lEventType, ccEvtMgr::CEventEx **pReturnEvent)
{
    CCTRACEI ( "CHPPEventHelper::NewEvent" );
    //
    // Create an event.
    //
    SYMRESULT result = SYMERR_UNKNOWN;

    // Make sure ccEvtMgr is running
    if(!ccEvtMgr::CEventManagerHelper::IsEventManagerActive())
    {
        CCTRACEI(_T("CHPPEventHelper::NewEvent - ccEvtMgr is not running."));
   
        if(m_bInit)
        {
            m_bInit = false;
            Destroy();
        }

        return SYMERR_UNKNOWN;
    }

    //
    //  Make sure everything is init'd properly
    if(!m_bInit)
    {
        result = Initialize();

        m_bInit = SYM_SUCCEEDED(result);

        if(!m_bInit)
            return result;
    }

    try
    {
        // Clear output.
        *pReturnEvent = NULL;

        if( CError::eNoError != m_pEventFactories.NewEvent( lEventType, *pReturnEvent))
        {
            CCTRACEE ( "CHPPEventHelper::createNAVEvent:SYMERR_OUTOFMEMORY" );
            m_pEventFactories.DeleteEvent(*pReturnEvent);
            *pReturnEvent = NULL;
            return SYMERR_OUTOFMEMORY;
        }
        return SYM_OK;
    }
    catch(...)
    {
        CCTRACEE ( "CreateAvEvent:CreateAvEvent exception thrown!" );
    }

    // If we get here, bad things have happened.
    // Clean up.
    if( *pReturnEvent )
    {
        m_pEventFactories.DeleteEvent(*pReturnEvent);
        *pReturnEvent = NULL;
    }

    return result;
}


SYMRESULT CHPPEventHelper::NewHPPEvent(long lEventType, ccEvtMgr::CEventEx **pReturnEvent)
{
    CCTRACEI ( "CHPPEventHelper::NewHPPEvent" );
    
    SYMRESULT sr = SYMERR_UNKNOWN;
    sr = NewEvent(lEventType, pReturnEvent);
    if(SYM_SUCCEEDED(sr) && *pReturnEvent)
    {
        sr = PopulateBaseProperties(*pReturnEvent);
    }

    return sr;
}

SYMRESULT CHPPEventHelper::DeleteHPPEvent(ccEvtMgr::CEventEx *pEvent)
{
    if( !pEvent || !m_bInit || !ccEvtMgr::CEventManagerHelper::IsEventManagerActive())
        return SYMERR_UNKNOWN;

    m_pEventFactories.DeleteEvent(pEvent);
    
    return SYM_OK;
}

// Core function to send a ccEvt event and wait for it to be canceled
bool CHPPEventHelper::SendEventAsynchronous(CEventExPtr pEvent)
{
    // Create a provider to send events
    CSymStaticRefCount<CProviderHelper> ProviderHelper;
    if( !ProviderHelper.Create(&m_EventManager, FALSE) )
    {
        CCTRACEE(_T("CHPPEventHelper::SendEventAsynchronous() - ProviderHelper.Create() failed.\n"));
        return false;
    }

    // Send the event and wait for it to return
    long nEventId = 0;
    CError::ErrorType res;
    res = ProviderHelper.CreateEvent(*pEvent, false, nEventId);
    if( CError::eNoError != res )
    {
        CCTRACEE(_T("CHPPEventHelper::SendEventAsynchronous() - ProviderHelper.CreateEvent() failed, eRes=%d\n"), res);
		//Defect 1-4BJHJL
	    ProviderHelper.Destroy();
		return false;
	}
    // Destroy the provider helper (relying on the destructor can be destructive)
    ProviderHelper.Destroy();

    return true;
}
// Core function to send a ccEvt event and wait for it to be canceled
bool CHPPEventHelper::SendEvent(CEventExPtr pEvent, 
                                           CEventEx** ppReturnEvent)
{
    // Create a provider to send events
    CSymStaticRefCount<CProviderHelper> ProviderHelper;
    if( !ProviderHelper.Create(&m_EventManager, TRUE) )
    {
        CCTRACEE(_T("CSymProtectControlHelper::SendSPEvent() - ProviderHelper.Create() failed.\n"));
        return false;
    }

    // Send the event and wait for it to return
    long nEventId = 0;
    CError::ErrorType res;
    CEventExPtr pReturnEventPlaceholder;
    res = ProviderHelper.CreateEvent(*pEvent, false, nEventId, NULL, 
                        HPPEEVENTHELPER_EVENT_TIMEOUT_MS, TRUE, 
                        pReturnEventPlaceholder.m_p);
    if( CError::eNoError != res )
    {
        CCTRACEE(_T("CHPPEventHelper::SendEvent() - ProviderHelper.CreateEvent() failed, eRes=%d\n"), res);
		//Defect 1-4BJHJL
	    ProviderHelper.Destroy();
		return false;
	}

    // If the caller wants the resulting event back...
    if( ppReturnEvent )
    {
        // Make an addref'd copy
        *ppReturnEvent = pReturnEventPlaceholder;
        (*ppReturnEvent)->AddRef();
    }

    // Destroy the provider helper (relying on the destructor can be destructive)
    ProviderHelper.Destroy();

    return true;
}


SYMRESULT CHPPEventHelper::PopulateBaseProperties(ccEvtMgr::CEventEx *pEvent)
{
    CHPPEventCommonInterfaceQIPtr spCommon = pEvent;
    if(!spCommon)
        return SYMERR_UNKNOWN;
    
    CHPPEventEz::AddCurrentUserName(spCommon, HPP::Event_Base_propUserName);
    CHPPEventEz::AddCurrentUserSID(spCommon, HPP::Event_Base_propUserSID);
    CHPPEventEz::AddCurrentTerminalSessionId(spCommon, HPP::Event_Base_propSessionID);

    // Set current version
    spCommon->SetPropertyLONG(HPP::Event_Base_propVersion, 0x00010000);


    return SYM_OK;
}

bool CHPPEventHelper::GetStatus(bool& bEnabled, bool& bError)
{
    // Initialize parameters with suitably paranoid values:
    bEnabled = false;
    bError = true;

    // Create a Query event if one hasn't already been created
    ccEvtMgr::CEventExPtr spNewEvent;
    NewEvent( IBBSymProtectQueryConfigEvent::TypeId,
              &spNewEvent);
    if( !spNewEvent )
    {
        CCTRACEE(_T("CHPPEventHelper::GetStatus() - NewEvent(IBBSymProtectQueryConfigEvent) failed.\n"));
        return false;
    }

    // Send the event and wait to examine the result
    ccEvtMgr::CEventExPtr pReturnEvent;
    if( !SendEvent(spNewEvent, &pReturnEvent) )
    {
        CCTRACEE(_T("CHPPEventHelper::GetStatus() - SendEvent(m_pQueryEvent) failed.\n"));
        return false;
    }

    // Cast the returned event back to a CQueryStateEventEx
    IBBSymProtectQueryConfigEventQIPtr pQuery = pReturnEvent;
    if( pQuery == NULL )
    {
        CCTRACEE(_T("CHPPEventHelper::GetStatus() - QI failed"));
        return false;
    }

    // Extract the state data
    SYMRESULT symRes = pQuery->GetSymProtectComponentState( bEnabled, bError );
    if( SYM_FAILED(symRes) )
    {
        CCTRACEE(_T("CHPPEventHelper::GetStatus() - pQuery->GetSymProtectComponentState() failed (0x%08X)\n"), symRes);
        return false;
    }

    return true;
}

bool CHPPEventHelper::CreatePlaceholderSysMonClient()
{
    CCTRACEI(_T("CHPPEventHelper::CreatePlaceholderSysMonClient - attempting to create placeholder SysMon client."));

    bool bRet;
    
    bRet = RegisterClient(CLSID_HPPPlaceholderSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::CreatePlaceholderSysMonClient - Unable to create placeholder SysMon client."));
        return false;
    }

    // Create placeholder rule on bogus key
    bRet = CreateRule(CLSID_HPPPlaceholderSysMonClient, 
                      TARGET_REGVALUE, 
                      ACTION_WRITE_REGVALUE, 
                      ALLOW, 
                      WSZ_REGKEY_HKLM L"\\Software\\Symantec", 
                      L"HPPPlaceHolderKey", 
                      NULL, FALSE);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::CreatePlaceholderSysMonClient - Unable to create placeholder SysMon rule."));
        return false;
    }
    
    CCTRACEI(_T("CHPPEventHelper::CreatePlaceholderSysMonClient - placeholder SysMon client successfully created."));
    return true;
}

bool CHPPEventHelper::RemovePlaceholderSysMonClient()
{
    CCTRACEI(_T("CHPPEventHelper::RemovePlaceholderSysMonClient - attempting to remove placeholder SysMon client."));

    bool bRet;

    bRet = DeleteAllRules(CLSID_HPPPlaceholderSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::RemovePlaceholderSysMonClient - Unable to remove placeholder SysMon rule."));
        return false;
    }
    
    bRet = UnregisterClient(CLSID_HPPPlaceholderSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::RemovePlaceholderSysMonClient - Unable to unregister placeholder SysMon client."));
        return false;
    }
    
    CCTRACEI(_T("CHPPEventHelper::RemovePlaceholderSysMonClient - placeholder SysMon client successfully removed."));
    return true;
}

bool CHPPEventHelper::RegisterClient(const GUID& guid, ULONG Priority )
{
    ccEvtMgr::CEventExPtr spEvent;
    bool bReturn = false;

    // Create a Query event if one hasn't already been created
    NewEvent(IBBSysMonClientRegistrationEvent::TypeId, &spEvent);
    if( !spEvent )
    {
        CCTRACEE(_T("CHPPEventHelper::RegisterClient() - NewEvent(IBBSysMonClientRegistrationEvent) failed.\n"));
        return false;
    }

    IBBSysMonClientRegistrationEventQIPtr spRegisterEvent = spEvent;
    if(!spRegisterEvent)
        return false;

    spRegisterEvent->RegisterClient(guid, Priority);
    spRegisterEvent.Release();

    // Send the event and wait to examine the result
    CEventExPtr spReturnedEvent;
    if( !SendEvent(spEvent, &spReturnedEvent) )
    {
        CCTRACEE(_T("CHPPEventHelper::RegisterClient() - SendEvent(spRegisterEvent) failed.\n"));
        return false;
    }

    spRegisterEvent = spReturnedEvent;
    if(spRegisterEvent)
    {
        SYSMON_CLIENT_STATUS status;
        ULONG priority;
        SYMRESULT sr;

        sr = spRegisterEvent->CheckRegistrationStatus(status, priority);
        if(SYM_SUCCEEDED(sr))
        {
            bReturn = (status == REGISTERED_ENABLED);
        }
		else
		{
			CCTRACEE(_T("CHPPEventHelper::RegisterClient() - CheckRegistrationStatus failed.\n"));
		}
    }

    return bReturn;
}

bool CHPPEventHelper::UnregisterClient( const GUID& guid )
{
    ccEvtMgr::CEventExPtr spEvent;
    bool bReturn = false;

    // Create a Query event if one hasn't already been created
    NewEvent(IBBSysMonClientRegistrationEvent::TypeId, &spEvent);
    if( !spEvent )
    {
        CCTRACEE(_T("CHPPEventHelper::RegisterClient() - NewEvent(IBBSysMonClientRegistrationEvent) failed.\n"));
        return false;
    }

    IBBSysMonClientRegistrationEventQIPtr spRegisterEvent = spEvent;
    if(!spRegisterEvent)
        return false;

    spRegisterEvent->UnregisterClient(guid);
    spRegisterEvent.Release();

    // Send the event and wait to examine the result
    CEventExPtr spReturnedEvent;
    if( !SendEvent(spEvent, &spReturnedEvent) )
    {
        CCTRACEE(_T("CHPPEventHelper::RegisterClient() - SendEvent(spRegisterEvent) failed.\n"));
        return false;
    }

    spRegisterEvent = spReturnedEvent;
    if(spRegisterEvent)
    {
        SYSMON_CLIENT_STATUS status;
        ULONG priority;
        SYMRESULT sr;

        sr = spRegisterEvent->CheckRegistrationStatus(status, priority);
        if(SYM_SUCCEEDED(sr))
        {
            bReturn = (status == NOT_REGISTERED);
        }
		else
		{
			CCTRACEE(_T("CHPPEventHelper::RegisterClient() - Check Registration Status Failed failed.\n"));
		}
    }

    return bReturn;
}

bool CHPPEventHelper::CheckRegistrationStatus( bbSystemMonitorEvt::SYSMON_CLIENT_STATUS& status, ULONG& Priority )
{
    return false;
}

bool CHPPEventHelper::SetMonitorRule()
{
    return false;
}

bool CHPPEventHelper::DeleteAllRules(const GUID& guidClient)
{
    ccEvtMgr::CEventExPtr spEvent;

    // Create a Rule Set Event
    NewEvent(IBBSysMonRulesetSetEvent::TypeId, &spEvent);
    if( !spEvent )
    {
        CCTRACEE(_T("CHPPEventHelper::SetNotifyRegValue() - NewEvent(IBBSysMonRulesetSetEvent) failed.\n"));
        return false;
    }


    IBBSysMonRulesetSetEventQIPtr spRuleSetEvent = spEvent;
    if(spRuleSetEvent)
    {
        ULONG ulRuleId = 0x01;

        spRuleSetEvent->SetClientGUID(guidClient);
        spRuleSetEvent->DeleteAllRules();

        // Send the event and wait to examine the result
        CEventExPtr spReturnedEvent;
        if( !SendEvent(spEvent, &spReturnedEvent) )
        {
            CCTRACEE(_T("CHPPEventHelper::RegisterClient() - SendEvent(spRegisterEvent) failed.\n"));
            return false;
        }
		
        IBBSysMonRulesetSetEventQIPtr spReturnedRuleSetEvent = spReturnedEvent;
        if(spReturnedRuleSetEvent)
        {
            SYMRESULT sr = spReturnedRuleSetEvent->GetRuleId(ulRuleId);
			if(SYM_FAILED(sr))
			{
				CCTRACEE(_T("CHPPEventHelper::DeleteAllRules() - GetRuleId failed.\n"));
			}
            ulRuleId = NULL;

            spReturnedRuleSetEvent.Release();
        }

    }


    return true;
}

bool CHPPEventHelper::SetNotifyOnReadRegValue(  const GUID& guidClient, 
                                                  const wchar_t* pszPath, 
                                                  const wchar_t* pszValueName,
                                                  const wchar_t* pszActorPath )
{
    return CreateRule(guidClient, TARGET_REGVALUE, ACTION_READ_REGVALUE, ALLOW, pszPath, pszValueName, pszActorPath);
}

bool CHPPEventHelper::SetNotifyRegValue( const GUID& guidClient, 
                                            const wchar_t* pszPath, 
                                            const wchar_t* pszValueName,
                                            const wchar_t* pszActorPath )
{
    return CreateRule(guidClient, TARGET_REGVALUE, ACTION_WRITE_REGVALUE, ALLOW, pszPath, pszValueName, pszActorPath);
}

bool CHPPEventHelper::SetBlockRegValue( const GUID& guidClient, 
                                           const wchar_t* pszPath, 
                                           const wchar_t* pszValueName,
                                           const wchar_t* pszActorPath )
{
    return CreateRule(guidClient, TARGET_REGVALUE, ACTION_WRITE_REGVALUE, BLOCK, pszPath, pszValueName, pszActorPath);
}

bool CHPPEventHelper::SetNotifyRegCreateKey( const GUID& guidClient, 
                                                const wchar_t* pszPath, 
                                                const wchar_t* pszActorPath )
{
    return CreateRule(guidClient, TARGET_REGKEY, ACTION_CREATE_REGKEY, ALLOW, pszPath, NULL, pszActorPath);
}

bool CHPPEventHelper::SetBlockRegKey( const GUID& guidClient, 
                                         const wchar_t* pszPath,
                                         const wchar_t* pszActorPath )
{
    return CreateRule(guidClient, TARGET_REGKEY, ACTION_CREATE_REGKEY, BLOCK, pszPath, NULL, pszActorPath );
}

bool CHPPEventHelper::SetProcessExclusionRegValue( const GUID& guidClient, 
                                                      const wchar_t* pszPath, 
                                                      const wchar_t* pszValueName,
                                                      const wchar_t* pszActorPath )
{
   return CreateRule(guidClient, TARGET_REGVALUE, ACTION_WRITE_REGVALUE, ALLOW, pszPath, pszValueName, pszActorPath, TRUE);
}


bool CHPPEventHelper::CreateRule( const GUID& guidClient, 
                                     SYSMON_TARGET_TYPE sysmonTargetType,
                                     SYSMON_ACTION_TYPE sysmonActionType,
                                     SYSMON_BLOCK_DISPOSITION sysmonBlockDisposition,
                                     const wchar_t* pszTargetPath, 
                                     const wchar_t* pszTargetValueName,
                                     const wchar_t* pszActorPath,
                                     BOOL bNotify,
                                     NTSTATUS lReturnCode )
{
    ccEvtMgr::CEventExPtr spEvent;
    bool bReturn = false;

    // Create a Rule Set Event
    NewEvent(IBBSysMonRulesetSetEvent::TypeId, &spEvent);
    if( !spEvent )
    {
        CCTRACEE(_T("CHPPEventHelper::SetNotifyRegValue() - NewEvent(IBBSysMonRulesetSetEvent) failed.\n"));
        return false;
    }


    IBBSysMonRulesetSetEventQIPtr spRuleSetEvent = spEvent;
    if(spRuleSetEvent)
    {
        spRuleSetEvent->SetClientGUID(guidClient);

        ISysMonRuleData* pSysMonRuleData = &spRuleSetEvent->ReferenceRuleData();
        if(pSysMonRuleData)
        {
            pSysMonRuleData->Enable();

            if(bNotify)
			{
                pSysMonRuleData->EnableNotify();
			}
            pSysMonRuleData->SetTargetType(sysmonTargetType);

            pSysMonRuleData->SetTargetPath(pszTargetPath, pszTargetValueName);

            if(pszActorPath)
                pSysMonRuleData->SetActorPath(pszActorPath);

            if(lReturnCode != -1)
                pSysMonRuleData->SetReturnCode(lReturnCode);

            pSysMonRuleData->SetAction(sysmonActionType);
            pSysMonRuleData->SetBlockDisposition(sysmonBlockDisposition);
			
            if( !SendEventAsynchronous(spEvent) )
            {
                CCTRACEE(_T("CHPPEventHelper::RegisterClient() - SendEvent(spRegisterEvent) failed.\n"));
                return false;
            }

            bReturn = true;
        }
        else
        {
            CCTRACEE(_T("CHPPEventHelper::RegisterClient() - bad spSysMonRuleData\n"));
            return false;
        }
    }
    else
    {
        CCTRACEE(_T("CHPPEventHelper::RegisterClient() - bad spRuleSetEvent\n"));
        return false;
    }
	
    return bReturn;
}

bool CHPPEventHelper::RegisterHPPClient()
{
    return RegisterClient(CLSID_HPPSysMonClient);
}

bool CHPPEventHelper::CreateHPPUserOptionalRules(LPCWSTR wszUserSID, LONG lBlockBehavior)
{
    CStringW cwszUserHomePage;
    cwszUserHomePage.Format(L"%s\\%s\\%s", WSZ_REGKEY_HKU, wszUserSID, WSZ_REGKEY_IE_HOME_PAGE);
    
    return CreateHPPOptionalRules(cwszUserHomePage, lBlockBehavior);
}

bool CHPPEventHelper::CreateHPPOptionalRules(LPCWSTR wszHomePageKey, LONG lBlockBehavior)
{
    BOOL bRet;

    // Setup rule for notification when IE reads the home page
    bRet = SetNotifyOnReadRegValue( CLSID_HPPSysMonClient, 
        wszHomePageKey, 
        WSZ_REGVAL_IE_HOME_PAGE, 
        CHPPEventEz::GetInternetExplorerPath() );
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::CreateHPPRules - SetNotifyRegValue(%S, %S) failed.\r\n"), wszHomePageKey, CHPPEventEz::GetInternetExplorerPath());
    }

    // Setup rules related to blocking behavior
    if(HPP::HPP_ACTION_BLOCK == lBlockBehavior)
    {
        bRet = SetBlockRegValue( CLSID_HPPSysMonClient, 
            wszHomePageKey, 
            WSZ_REGVAL_IE_HOME_PAGE, 
            NULL);
        if(!bRet)
        {
            CCTRACEE(_T("CHPPEventHelper::CreateHPPRules - SetBlockRegValue(%S) failed.\r\n"), wszHomePageKey);
        }

    }
    else if(HPP::HPP_ACTION_ALLOW == lBlockBehavior)
    {
        bRet = SetNotifyRegValue( CLSID_HPPSysMonClient, 
            wszHomePageKey, 
            WSZ_REGVAL_IE_HOME_PAGE, 
            NULL );
        if(!bRet)
        {
            CCTRACEE(_T("CHPPEventHelper::CreateHPPRules - SetBlockRegValue(%S) failed.\r\n"), wszHomePageKey);
        }

    }



    // Set up HPP App so it can change the home page key
    bRet = SetProcessExclusionRegValue( CLSID_HPPSysMonClient, 
        wszHomePageKey, 
        WSZ_REGVAL_IE_HOME_PAGE, 
        CHPPEventEz::GetHPPAppPath() );

    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::CreateHPPRules - SetProcessExclusionRegValue(%S, %S) failed.\r\n"), wszHomePageKey, CHPPEventEz::GetHPPAppPath());
    }
    
	// Set up NAVW32 App so it can change the home page key
	bRet = SetProcessExclusionRegValue( CLSID_HPPSysMonClient, 
		wszHomePageKey, 
		WSZ_REGVAL_IE_HOME_PAGE, 
		CHPPEventEz::GetNAVW32Path() );

	if(!bRet)
	{
		CCTRACEE(_T("CHPPEventHelper::CreateHPPRules - SetProcessExclusionRegValue(%S, %S) failed.\r\n"), wszHomePageKey, CHPPEventEz::GetNAVW32Path());
	}



    return S_OK;
}

bool CHPPEventHelper::CreateHPPRules(LONG lBlockBehavior)
{
    HRESULT hr = E_FAIL;

    // Get the list of users
    LST_CSTRINGW lstAllUsersList;
    hr = GetAllUsersList(lstAllUsersList);
    if(FAILED(hr))
    {
        // If the all users list has not been created yet, create it
        hr = BuildAllUsersList(lstAllUsersList);
        if(SUCCEEDED(hr))
        {
            // Put the all users list into ccSettings
            hr = PutAllUsersList(lstAllUsersList);
            if(FAILED(hr))
            {
                lstAllUsersList.clear();
                CCTRACEE(_T("CHPPEventHelper::CreateHPPRules - failed to put user list."));
            }
        }
        else
        {
            CCTRACEE(_T("CHPPEventHelper::CreateHPPRules - failed to build user list."));
        }
    }
    
    //
    // So... BB SysMon can't:
    //  a.) Enumerate rules
    //  b.) Return a count of rules for a client
    //  c.) Confirm a client is already registered
    //
    //   That means that we must always create the client and the rule,
    //   then delete the rules, and re-create the client.  Lame.
    //
    //   HackMaster P - 8/11/2005
    //

    // Make sure there is at least one placeholder rule
    CreatePlaceholderSysMonClient();

    CreateHPPOptionalRules(CHPPEventEz::GetHKLMHomePageKey(), lBlockBehavior);

    if(lstAllUsersList.size())
    {
        for(DWORD dwCounter = 0; dwCounter < lstAllUsersList.size(); dwCounter++)
        {
            CreateHPPUserOptionalRules(lstAllUsersList[dwCounter], lBlockBehavior);
        }
    }

    // Remove the extra placeholder rules
    RemovePlaceholderSysMonClient();
    
    //
    //  Because we can't currently query to see how many rules are associated with
    //  a particular client, we must create the rules here to ensure they remain.
    //
    // Create one placeholder rule
    CreatePlaceholderSysMonClient();

    return true;
}

bool CHPPEventHelper::ModifyHPPRules(LONG lBlockBehavior)
{
    LST_CSTRINGW lstAllUsersList;
    HRESULT hr;
    
    hr = GetAllUsersList(lstAllUsersList);
    if(SUCCEEDED(hr))
    {
        // If there are no users, create the first
        if(0 == lstAllUsersList.size())
        {
            // Add this user to the list
            lstAllUsersList.push_back(CHPPEventEz::GetCurrentUserSID());

            hr = PutAllUsersList(lstAllUsersList, TRUE);
            if(FAILED(hr))
            {
                CCTRACEE(_T("CHPPEventHelper::ModifyHPPRules - unable to create initial user."));
            }
        }
    }

    BOOL bRet = FALSE;
    bRet = DeleteAllRules(CLSID_HPPSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::ModifyHPPRules - DeleteAllRules(CLSID_HPPSysMonClient) failed.\r\n"));
    }

    bRet = CreateHPPRules(lBlockBehavior);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::ModifyHPPRules - CreateHPPRules() failed.\r\n"));
    }

    return true;
}

bool CHPPEventHelper::RemoveHPPRules()
{
    BOOL bRet = FALSE;
    bRet = DeleteAllRules(CLSID_HPPSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::RemoveHPPRules - RemoveHPPRules(CLSID_HPPSysMonClient) failed.\r\n"));
    }
    
    bRet = DeleteAllRules(CLSID_HPPPlaceholderSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::RemoveHPPRules - RemoveHPPRules(CLSID_HPPPlaceholderSysMonClient) failed.\r\n"));
    }

    return true;
}

bool CHPPEventHelper::PrepHPPForUninstall()
{
    BOOL bRet = FALSE;
    bRet = UnRegisterHPPClient();
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::PrepHPPForUninstall - UnRegisterHPPClient() failed.\r\n"));
    }

    bRet = RemovePlaceholderSysMonClient();
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::PrepHPPForUninstall - RemovePlaceholderSysMonClient() failed.\r\n"));
    }

    return true;
}

bool CHPPEventHelper::UnRegisterHPPClient()
{
    BOOL bRet = FALSE;
    bRet = DeleteAllRules(CLSID_HPPSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::UnRegisterHPPClient - RemoveHPPRules() failed.\r\n"));
    }

    bRet = UnregisterClient(CLSID_HPPSysMonClient);
    if(!bRet)
    {
        CCTRACEE(_T("CHPPEventHelper::UnRegisterHPPClient - UnregisterClient(CLSID_HPPSysMonClient) failed.\r\n"));
    }

    RemovePlaceholderSysMonClient();

    return true;
}

HRESULT CHPPEventHelper::GetAllUsersList(LST_CSTRINGW &lstAllUsersList)
{
    ccSettings::CSettingsManagerHelper ccSettingsHelper;
    ccSettings::ISettingsManagerPtr spSettingsManager;

    SYMRESULT sr;
    
    // Setup the settings manager helper
    sr = ccSettingsHelper.Create(spSettingsManager);
    if(SYM_FAILED(sr) || !spSettingsManager)
        return E_FAIL;

    // Retrieve our settings block, if the block doesn't
    //  exist, then fail out.
    ccSettings::ISettingsPtr spHPPSettings;
    sr = spSettingsManager->GetSettings(HPP_CCSETTINGS_USERLIST_KEY, &spHPPSettings);
    if(SYM_FAILED(sr) || !spHPPSettings)
        return E_FAIL;

    // Enum the values, and place them into the list
    ccSettings::IEnumValuesPtr spEnum;
    sr = spHPPSettings->EnumValues(&spEnum);
    if(SYM_FAILED(sr) || !spEnum)
        return E_FAIL;

    DWORD dwCount;
    sr = spEnum->GetCount(dwCount);
    if(SYM_FAILED(sr))
        return E_FAIL;

    for(DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
    {
        DWORD dwNameSize = NULL, dwItemType = NULL;
        sr = spEnum->GetItem(dwIndex, (LPWSTR)NULL, dwNameSize, dwItemType);
        if(SYM_SETTINGS_ERROR_BUFFER_TOO_SMALL == sr)
        {   
            CStringW cwszValue;
            dwNameSize++;
            sr = spEnum->GetItem(dwIndex, cwszValue.GetBuffer(dwNameSize), dwNameSize, dwItemType);
            cwszValue.ReleaseBuffer();
            if(SYM_SUCCEEDED(sr))
            {
                lstAllUsersList.push_back(cwszValue);
            }
        }
    }

    return S_OK;
}

HRESULT CHPPEventHelper::PutAllUsersList(LST_CSTRINGW &lstAllUsersList, BOOL bCreate)
{
    ccSettings::CSettingsManagerHelper ccSettingsHelper;
    ccSettings::ISettingsManagerPtr spSettingsManager;

    SYMRESULT sr;
    
    // Setup the settings manager helper
    sr = ccSettingsHelper.Create(spSettingsManager);
    if(SYM_FAILED(sr) || !spSettingsManager)
        return E_FAIL;

    // Retrieve our settings block, if the block doesn't
    ccSettings::ISettingsPtr spHPPSettings;
    sr = spSettingsManager->GetSettings(HPP_CCSETTINGS_USERLIST_KEY, &spHPPSettings);
    if(SYM_FAILED(sr) || !spHPPSettings)
    {
        // If the caller has requested a failure
        //  if the settings don't exist, bail
        if(!bCreate)
            return E_FAIL;

        // If the caller wishes the data block
        //  to be created, then give it a try
        sr = spSettingsManager->CreateSettings(HPP_CCSETTINGS_USERLIST_KEY, &spHPPSettings);
        if(SYM_FAILED(sr) || !spHPPSettings)
            return E_FAIL;
    }

    bool bNewData = false;
    DWORD dwSize = 0;
    for(DWORD dwIndex = 0; dwIndex < lstAllUsersList.size(); dwIndex++)
    {
        // Put any values from the list into the data block
        sr = spHPPSettings->GetString(lstAllUsersList[dwIndex], NULL, dwSize);
        if(SYM_SETTINGS_NOT_FOUND == sr)
        {
            sr = spHPPSettings->PutString(lstAllUsersList[dwIndex], L"");
            if(SYM_FAILED(sr))
                continue;

            bNewData = true;
        }
    }

    // If there weren't any updates, don't bother
    // sending it up to ccSettings
    if(!bNewData)
        return S_FALSE;

    // Apparently there were updates, put
    //  them into the ccSettings store
    sr = spSettingsManager->PutSettings(spHPPSettings);
    if(SYM_FAILED(sr))
        return E_FAIL;

    return S_OK;
}

HRESULT CHPPEventHelper::CheckAndAddSingleUserRules(LPCWSTR wszUserSID, LONG lBlockBehavior, BOOL bCreate)
{
    LST_CSTRINGW lstAllUsersList;
    HRESULT hr;

    hr = GetAllUsersList(lstAllUsersList);
    if(FAILED(hr))
    {
        return hr;
    }

    // This call should not create the first user
    if(0 == lstAllUsersList.size())
        return S_OK;

    // Clear the list
    lstAllUsersList.clear();

    // Add this user to the list
    lstAllUsersList.push_back(wszUserSID);

    hr = PutAllUsersList(lstAllUsersList, bCreate);
    if(S_OK == hr)
    {
        CCTRACEI(_T("CHPPEventHelper::CheckAndAddSingleUserRules - Looks like user %S needs rules created for them."), wszUserSID);

        CreateHPPUserOptionalRules(wszUserSID, lBlockBehavior);
    }
    
    return hr;
}

HRESULT CHPPEventHelper::BuildAllUsersList(LST_CSTRINGW &lstAllUsersList)
{
    LONG lRes = ERROR_SUCCESS;
    CRegKey crkUsers;

    // Open up the HKEY_USERS key
    lRes = crkUsers.Open(HKEY_USERS, _T(""), KEY_READ);
    if(ERROR_SUCCESS != lRes)
    {
        CCTRACEE(_T("CHPPEventHelper::GetAllUsersList - couldn't open users key."));
        return E_FAIL;
    }

    // Query the HKEY_USERS key to find out how
    //  many subkeys there are, and the size needed
    DWORD dwSubKeys = NULL, dwMaxSubKeyLen = NULL;
    lRes = RegQueryInfoKeyW(crkUsers, NULL, NULL, NULL, &dwSubKeys, &dwMaxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);
    if(ERROR_SUCCESS != lRes)
    {
        CCTRACEE(_T("CHPPEventHelper::GetAllUsersList - couldn't query users key."));
        return E_FAIL;
    }

    // Enum the HKEY_USERS key to get a list of
    // SIDs for users.
    CStringW cwszKeyName;
    DWORD dwKeyNameSize = (NULL != dwMaxSubKeyLen) ? (dwMaxSubKeyLen + 1) : MAX_PATH;
    for(DWORD dwCounter = 0; dwCounter < dwSubKeys; dwCounter++)
    {
        // Query the index for a subkey
        lRes = RegEnumKeyW(crkUsers, dwCounter, cwszKeyName.GetBuffer(dwKeyNameSize), dwKeyNameSize);
        cwszKeyName.ReleaseBuffer();

        // If there was an error, continue
        if(ERROR_SUCCESS != lRes)
        {
            CCTRACEE(_T("CHPPEventHelper::GetAllUsersList - couldn't enum users key."));
            continue;
        }

        // If there was an empty string returned, continue
        if(cwszKeyName.IsEmpty())
        {
            CCTRACEE(_T("CHPPEventHelper::GetAllUsersList - RegEnumKeyW returned unexpected empty string."));
            continue;
        }

        // If the string "_Classes" is found, then
        //  this is not a key we're interested in
        if(NULL != StrStrIW(cwszKeyName, L"_Classes"))
            continue;


        lstAllUsersList.push_back(cwszKeyName);
    }


    return S_OK;
}
