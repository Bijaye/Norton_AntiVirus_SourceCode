////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable : 4996)

#include "ccProviderHelper.h"
#include "ccEventManagerHelper.h"
#include "SymProtectEventsInterface.h"
#include "SymProtectEventsLoader.h"

class CEventProvider
{
public:
    CEventProvider(void);
    virtual ~CEventProvider(void);

    bool GetNewEvent(long nEventType, ccEvtMgr::CEventExPtr& pEvent);
    bool SendConfigEvent(ccEvtMgr::CEventExPtr pEvent);
    void DeleteEvent(ccEvtMgr::CEventExPtr& pEvent);

    bool Initialize(void);
    void Destroy(void);


private:
    ccEvtMgr::CProxyFactoryExPtr    m_ProxyFactory;
    ccEvtMgr::CEventFactoryEx2Ptr   m_EventFactory;
    SymProtectEvt::SymProtectEvents_CEventFactoryEx2  m_EventFactoryLoader;
    SymProtectEvt::SymProtectEvents_CProxyFactoryEx   m_ProxyFactoryLoader;
    CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManagerHelper;
    CSymStaticRefCount<ccEvtMgr::CProviderHelper>     m_ProviderHelper;
    
    friend class CEventTest;
};
