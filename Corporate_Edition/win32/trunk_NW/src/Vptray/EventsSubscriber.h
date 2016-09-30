// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "EraserNotify.h"
#include "ccSymMultiEventFactoryHelper.h"
#include "ccEventManagerEx.h"
#include "ccSymCommonClientInfo.h"
#include "ccSettingsChangeEventEx.h"
#include "ccSetEvtLoader.h"
#include "ccProSubLoader.h"
#include "ccEventManagerHelper.h"
#include "ccSubscriberHelper.h"
#include "ccThread.h"
#include "VPExceptionHandling.h"

#include <queue>

typedef ::std::queue<ccEvtMgr::CEventEx*> EVENT_QUEUE;
typedef ::std::map<long, long> SubscriberIdMap;

//////////////////////////////////////////////////////////////////////
// CEventsSubscriber
//
// Subscriber to Event Manager events.
//
class CEventsSubscriber :
	public ccEvtMgr::CSubscriberHelper,
	public ccLib::CThread
{
public:
	CEventsSubscriber();
	virtual ~CEventsSubscriber();

	BOOL Start();
	void Stop();
	void Destroy();
	void SetTrayWnd(const HWND hTrayWnd);

	// CSubscriberEx overrides
	ccEvtMgr::CError::ErrorType OnEvent(long nSubscriberId,  const ccEvtMgr::CEventEx& Event, CSubscriberEx::EventAction& eAction);
	ccEvtMgr::CError::ErrorType OnShutdown();

protected:
	// Thread overrides
	virtual int Run();

private:
	// Queue related
	void EmptyQueue();
	void ProcessQueue();

	// Event Factory
	BOOL InitializeFactories();
	void UninitializeFactories();

	// Event related
	ccEvtMgr::CError::ErrorType CopyEvent(const ccEvtMgr::CEventEx& Event, ccEvtMgr::CEventEx* &pEventCopy);
	ccEvtMgr::CError::ErrorType DeleteEvent(ccEvtMgr::CEventEx* pEvent);
	ccEvtMgr::CError::ErrorType Register();

	// Members
	bool m_bInitFactories;
	bool m_bCoInit;
	HWND m_hTrayWnd;
	CEraserNotify m_eraserNotify;
	HANDLE m_hThread;
	HANDLE m_hQueueEvent;
	EVENT_QUEUE m_eventQueue;
	ccLib::CCriticalSection m_critQueue;
	cc::ccSetEvt_CEventFactoryEx2 m_EventFactoryLoader;
    ccEvtMgr::CEventFactoryEx2Ptr m_pEventFactory;
	cc::ccProSub_CProxyFactoryEx m_ProxyFactoryLoader;
    ccEvtMgr::CProxyFactoryExPtr m_pProxyFactory;
	CSymStaticRefCount<CSymMultiEventFactoryHelper> m_EventFactories;
	CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManager;
};
