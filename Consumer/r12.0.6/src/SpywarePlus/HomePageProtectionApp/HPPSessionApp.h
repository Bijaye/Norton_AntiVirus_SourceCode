#pragma once

#include "SSPWorkerThread.h"
#include "SSPRunnableJob.h"

#include "Subscriber.h"

#include "HPPAppWnd.h"
#include "HPPAppInterface.h"
#include "HPPSettingsImpl.h"
#include "HPPEventHelper.h"
#include "EventLogHelper.h"

#include "SSPProcessIEStartupEvent.h"

#include "BBEventID.h"
#include "BBSystemMonitorEventsInterface.h"

using namespace bbSystemMonitorEvt;

class CHPPSessionApp : 
    public ISymBaseImpl< CSymThreadSafeRefCount >, 
    public IHPPSessionAppInterface,
    public CHPPSettingsImpl
{
public:
    CHPPSessionApp(void);
    virtual ~CHPPSessionApp(void);
    
    SYM_INTERFACE_MAP_BEGIN()                
        SYM_INTERFACE_ENTRY( IID_HPPSessionAppInterface, IHPPSessionAppInterface )
        SYM_INTERFACE_ENTRY( IID_HPPSettingsInterface, IHPPSettingsInterface)
    SYM_INTERFACE_MAP_END()                  


    // IHPPSessionAppInterface methods
    virtual HRESULT Initialize();
    virtual void Destroy(); 
    virtual HRESULT HandleEventManagerShutdown();
    virtual HRESULT ResetEventManagerConnections();
    virtual HRESULT PushIncomingEventToQueue(ccEvtMgr::CEventEx* pEvent);
    virtual HRESULT ProcessEvent(ccEvtMgr::CEventEx* pEvent);
    virtual HRESULT SwitchHomePage(ccEvtMgr::CEventEx* pEvent, LPCTSTR szNewHomePageValue, BOOL bChangeUserKey, DWORD dwThreadId = NULL);
    virtual void ConfirmSysMonRules();
    virtual void ShowInternetExplorerControlPanel();
    virtual void NavigateToHomePageByThread(DWORD dwThreadId);

protected:
    virtual HRESULT CopyPendingAlertsToJobObj(CSSPProcessIEStartupEvent *pJob);

    HRESULT HandleHomePageChangedEvent(ccEvtMgr::CEventEx* pEvent);
    HRESULT HandleIEStartedEvent(ccEvtMgr::CEventEx* pEvent);
    HRESULT HandleUserRequestChangeHomePageEvent(ccEvtMgr::CEventEx* pEvent);
	HRESULT HandleOptionsChangedEvent(ccEvtMgr::CEventEx* pEvent);

    HRESULT HandleBBNotifyEvent(ccEvtMgr::CEventEx* pEvent);

    HRESULT GenerateHPPHomePageChangeEventFromSysMonNotify(ISysMonNotificationData *pSysMonNotifyData);
    HRESULT GenerateHPPIEHomePageAccessedEventFromSysMonNotify(ISysMonNotificationData *pSysMonNotifyData);

    HRESULT CreateActivityLogFromHomePageChangedEvent(ccEvtMgr::CEventEx* pEvent);
    BOOL FindExplorerOptionsDialog(DWORD dwProcId, DWORD dwThreadId);

    CHPPAppWnd m_wndAppWnd;
    CMTAWorkerThread m_cWorkerThread;
    CMTAWorkerThread m_cUIWorkerThread;

    CSymPtr<CSubscriber> m_spSubscriber;

	IHPPSessionAppInterface *m_pTheApp;
    
    HPP::CEventLogHelperT<HPP::HPPEventFactory_CEventFactoryEx> m_EventLog;
    
    CHPPEventHelperPtr m_spHPPEventHelper;

    DWORD m_dwLazyStartHomePageDelay;
	ccLib::CCriticalSection m_critEvent;
    
    CString m_cszCurrentProcessPath;

    LONG m_dwSysMonNotifyEventCount;
    LONG m_dwHPPHomePageChangedEvents;
    LONG m_dwIncomingEventCount;

    bool m_bHomePageChangeEventSync;

};
