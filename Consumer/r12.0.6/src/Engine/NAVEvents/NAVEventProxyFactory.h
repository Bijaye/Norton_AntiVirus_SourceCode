#pragma once

#include "ccProxyFactoryImpl.h"
#include "NAVEventSubscriber.h"
#include "NAVEventProvider.h"

class CNAVEventProxyFactory : 
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public ccEvtMgr::CProxyFactoryImpl<ccEvtMgr::CProxyFactoryEx, 
                                       CNAVEventProvider,
                                       INAVEventProvider,
                                       CNAVEventSubscriber,
                                       INAVEventSubscriber>
{
public:
    CNAVEventProxyFactory(void);
    virtual ~CNAVEventProxyFactory(void);

	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY(ccEvtMgr::IID_ProxyFactoryEx, ccEvtMgr::CProxyFactoryEx)
	SYM_INTERFACE_MAP_END()
};
