// AvLogger.h: interface for the CAvLogger class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AvEvents.h"    // AvEvent interface

// For event manager stuff.
#include "ccEventManagerHelper.h"
#include "ccProviderHelper.h"
#include "NAVEventFactoryLoader.h"
#include "NAVEventCommon.h"

class CAvFactory : 	
    public ISymBaseImpl<CSymThreadSafeRefCount>,
	public AV::IAvEventFactory
{
public:
	CAvFactory();
	virtual ~CAvFactory();

	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY( AV::IID_AvEventFactory, IAvEventFactory )
	SYM_INTERFACE_MAP_END()
	
    // IAvEvent Factory
	//
	virtual SYMRESULT BroadcastAvEvent( CEventData& event );
    virtual SYMRESULT SendAvEvent( CEventData& event, CEventData*& pEventReturn );
    virtual SYMRESULT SendAvEvent( CEventData& event, CEventData*& pEventReturn, DWORD dwWaitTime );
    virtual SYMRESULT SendAvEvent( CEventData& event, CEventData*& pEventReturn, DWORD dwWaitTime, HANDLE hTerminate );

private:
    SYMRESULT initialize(bool bBroadcast);
    SYMRESULT destroy();
    SYMRESULT createNAVEvent(ccEvtMgr::CEventEx*& pReturnEvent);
    SYMRESULT sendAvEvent( CEventData& event,
                           CEventData*& pEventReturn,
                           bool bBroadcast = true,
                           DWORD dwWaitTime = 30000, /*30 secs*/ 
                           HANDLE hTerminate = NULL );

    // Disallowed
	CAvFactory( const CAvFactory& );
    CAvFactory& operator =(const CAvFactory&);

    // Pointer to event manager object
    ccEvtMgr::CEventManagerHelper m_EventManager;

	bool				m_bShutdown;
	bool				m_bCOMInitialized;
    DWORD               m_dwThreadId;
    bool                m_bInit;
    bool                m_bBroadcast;

    // Make event operations thread-safe
    ccLib::CCriticalSection m_critEvent;
};
