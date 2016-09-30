////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymInterface.h"
#include "NAVEvents.h"
#include "NAVEventUtils.h"
#include "NAVEventCommon.h"

#include "ccSubscriberEx.h"

// Identifier was truncated to x characters in the debug information
#pragma warning(disable:4786)

#include <map>

#define UM_EVENT WM_USER + 1
#define UM_SHUTDOWN WM_USER + 2

typedef std::map <long,long> mapSubData;
typedef mapSubData::iterator iterSubData;

class CTestSubscriberSub : 
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public CSubscriberEx
{
public:
	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY(IID_SubscriberEx, CSubscriberEx)
	SYM_INTERFACE_MAP_END()  

    CTestSubscriberSub() 
    {
        m_pEventFactory = NULL;
        m_piSubscriber = NULL;
        m_hWnd = NULL;
        m_pCurrentEvent = NULL;
        m_eAction = CSubscriberEx::eNoAction;
        m_lModuleID = NULL;
    }
    virtual ~CTestSubscriberSub() 
    {
        Disconnect();
    }

private:
    CTestSubscriberSub(const CTestSubscriberSub&);
    CTestSubscriberSub& operator =(const CTestSubscriberSub&);

public:
    BOOL Connect();
    BOOL Disconnect();
    BOOL Register(HWND hWnd, 
                  long nPriority,
                  long nEventType,
                  long& nSubscriberId);
    BOOL Unregister();
    void DeleteEvent(CEventEx* pEvent);
    BOOL CommitEvent(CEventEx* pEvent);
    BOOL CancelEvent(CEventEx* pEvent);

    virtual CError::ErrorType OnEvent(long nSubscriberId, 
                                      const CEventEx& Event,
                                      CSubscriberEx::EventAction& eAction);
    /*
    virtual CError::ErrorType OnEvent(long nSubscriberId, 
                                      IEventEx* piEvent,
                                      CSubscriberEx::EventAction& eAction);*/
    virtual CError::ErrorType OnShutdown();
    void SetOnEventAction(int nAction);

protected:
    long m_lModuleID;

    mapSubData m_mapEventSubData;   // Event Type, Subscriber ID

	//CNAVEventCommon* m_pCurrentEvent;
    CEventEx* m_pCurrentEvent;
    CEventFactoryEx* m_pEventFactory;
    ISubscriberEx* m_piSubscriber;
    HWND m_hWnd;
    EVENTMANAGERLib::IEventManagerPtr m_piEventManager;
    CSubscriberEx::EventAction m_eAction;
};


