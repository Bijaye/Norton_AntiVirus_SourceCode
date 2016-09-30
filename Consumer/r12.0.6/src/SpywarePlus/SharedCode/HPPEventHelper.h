#pragma once

#include <vector>

// For event manager stuff.
#include "ccEventManagerHelper.h"
#include "ccProviderHelper.h"
#include "ccProSubLoader.h"
#include "ccSymMultiEventFactoryHelper.h"
#include "HPPEventHelperLoader.h"


#include "SysMonEnumTypes.h"
#include "SysMonEventTypes.h"

EXTERN_C const GUID CLSID_HPPSysMonClient;
EXTERN_C const GUID CLSID_HPPPlaceholderSysMonClient;

typedef std::vector<CStringW> LST_CSTRINGW;

class CHPPEventHelper : 	
    public ccEvtMgr::CProviderHelper
{
public:
    CHPPEventHelper();
    virtual ~CHPPEventHelper();

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(ccEvtMgr::IID_ProviderEx, ccEvtMgr::CProviderEx)
    SYM_INTERFACE_MAP_END()

    //
    // CProviderEx
    //
    virtual ccEvtMgr::CError::ErrorType OnEvent(long nProviderId, const ccEvtMgr::CEventEx& Event);
    virtual ccEvtMgr::CError::ErrorType OnShutdown();

    //
    // HPP Events helper methods
    //
    SYMRESULT NewEvent(long lEventType, ccEvtMgr::CEventEx **pEvent);
    SYMRESULT NewHPPEvent(long lEventType, ccEvtMgr::CEventEx **pEvent);
    SYMRESULT DeleteHPPEvent(ccEvtMgr::CEventEx *pEvent);

    //
    // BB SysMon helper methods
    //
    bool GetStatus(bool& bEnabled, bool& bError);

    bool RegisterClient(const GUID& guid, ULONG Priority = 0 );
    bool UnregisterClient( const GUID& guid);

    bool CheckRegistrationStatus( bbSystemMonitorEvt::SYSMON_CLIENT_STATUS& status, ULONG& Priority );

    bool SetMonitorRule();

    bool SetNotifyOnReadRegValue(const GUID& guidClient, const wchar_t* pszPath, const wchar_t* pszValueName, const wchar_t* pszActorPath = NULL );
    bool SetNotifyRegValue(const GUID& guidClient, const wchar_t* pszPath, const wchar_t* pszValueName, const wchar_t* pszActorPath = NULL );
    bool SetBlockRegValue(const GUID& guidClient, const wchar_t* pszPath, const wchar_t* pszValueName=NULL, const wchar_t* pszActorPath = NULL );
    bool SetBlockRegKey(const GUID& guidClient, const wchar_t* pszPath, const wchar_t* pszActorPath = NULL );
    bool SetNotifyRegCreateKey( const GUID& guidClient, const wchar_t* pszPath, const wchar_t* pszActorPath = NULL );

    bool SetProcessExclusionRegValue(const GUID& guidClient, const wchar_t* pszPath, const wchar_t* pszValueName=NULL, const wchar_t* pszActorPath = NULL );

    bool DeleteAllRules(const GUID& guidClient);

    bool CreateRule( const GUID& guidClient, 
        bbSystemMonitorEvt::SYSMON_TARGET_TYPE sysmonTargetType,
        bbSystemMonitorEvt::SYSMON_ACTION_TYPE sysmonActionType,
        bbSystemMonitorEvt::SYSMON_BLOCK_DISPOSITION sysmonBlockDisposition,
        const wchar_t* pszTargetPath, 
        const wchar_t* pszTargetValueName,
        const wchar_t* pszActorPath = NULL, 
        BOOL bNotify = TRUE,
        bbSystemMonitorEvt::NTSTATUS lReturnCode = -1 );


    
    //
    // BB SysMon helper methods
    //
    bool RegisterHPPClient();

    bool CreateHPPRules(LONG lBlockBehavior );
    bool CreateHPPOptionalRules(LPCWSTR wszHomePageKey, LONG lBlockBehavior);
    bool CreateHPPUserOptionalRules(LPCWSTR wszUserSID, LONG lBlockBehavior);
    HRESULT CheckAndAddSingleUserRules(LPCWSTR wszUserSID, LONG lBlockBehavior, BOOL bCreate = FALSE);
    bool ModifyHPPRules(LONG lBlockBehavior);

    bool RemoveHPPRules();

    bool UnRegisterHPPClient();
    bool PrepHPPForUninstall();

    SYMRESULT Destroy();

private:
    bool SendEvent(ccEvtMgr::CEventExPtr pEvent, 
        ccEvtMgr::CEventEx** ppReturnEvent);
	bool SendEventAsynchronous(ccEvtMgr::CEventExPtr pEvent);
        
    SYMRESULT Initialize(void);
    
    SYMRESULT PopulateBaseProperties(ccEvtMgr::CEventEx *pEvent);

    HRESULT BuildAllUsersList(LST_CSTRINGW &lstAllUsersList);
    HRESULT GetAllUsersList(LST_CSTRINGW &lstAllUsersList);
    HRESULT PutAllUsersList(LST_CSTRINGW &lstAllUsersList, BOOL bCreate = TRUE);

    bool CreatePlaceholderSysMonClient();
    bool RemovePlaceholderSysMonClient();

    // Disallowed
    CHPPEventHelper( const CHPPEventHelper& );
    CHPPEventHelper& operator =(const CHPPEventHelper&);

    // Pointer to event manager object
    HPP::HPPEventFactory_CEventFactoryEx2 m_EventFactoryLoader;
    CSymStaticRefCount<CSymMultiEventFactoryHelper> m_pEventFactories;

    cc::ccProSub_CProxyFactoryEx m_ProxyFactoryLoader;
    ccEvtMgr::CProxyFactoryExPtr m_pProxyFactory;

    CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManager;

    long				m_nProviderId;
    bool				m_bCOMInitialized;
    DWORD               m_dwThreadId;
    bool                m_bInit;

    // Make event operations thread-safe
    ccLib::CCriticalSection m_critEvent;
};

typedef CSymPtr<CHPPEventHelper> CHPPEventHelperPtr;

