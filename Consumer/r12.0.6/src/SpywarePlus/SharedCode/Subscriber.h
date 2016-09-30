#pragma once

#include <queue>
#include "SyncHelper.h"
#include "ccSubscriberHelper.h"
#include "ccProSubLoader.h"
#include "ccEventManagerHelper.h"
#include "ccSymMultiEventFactoryHelper.h"

// This class implements a typical example of a subscriber class 
// that subscribes to two events

class CSubscriber : public ccEvtMgr::CSubscriberHelper
{
public:
    CSubscriber(void) throw();
    virtual ~CSubscriber(void) throw();

private:
    CSubscriber(const CSubscriber&) throw();
    CSubscriber& operator=(const CSubscriber&) throw();

public:
    bool Initialize(ISymBase *pUnknownApp) throw();
    void Destroy();

public:
    // CSubscriberEx overrides
    virtual ccEvtMgr::CError::ErrorType OnEvent(long nSubscriberId, 
                                      const ccEvtMgr::CEventEx& Event,
                                      EventAction& eAction) throw();
    virtual ccEvtMgr::CError::ErrorType OnShutdown() throw();

protected:
    bool PushAlertToQueue(const ccEvtMgr::CEventEx* pEvent) throw();

protected:
    // Helper for Proxy Factories
    cc::ccProSub_CProxyFactoryEx m_CProxyFactoryCreatorEx;
    ccEvtMgr::CProxyFactoryExPtr m_pCProxyFactoryEx;

    // Event Factory helper class to manage the factories we use
    CSymStaticRefCount<CSymMultiEventFactoryHelper> m_EventFactories;

    // Our Event Manager Helper class
	CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManagerHelper;

    // Parent App component
    CSymQIPtr<ISymBase, &IID_SymBase> m_spUnknownApp;

};
