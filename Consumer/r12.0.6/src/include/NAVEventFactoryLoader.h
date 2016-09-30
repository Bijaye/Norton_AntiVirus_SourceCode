#pragma once

#include "ccSymInterfaceLoader.h"
#include "ccSymPathProvider.h"

#include "ccEventFactoryEx.h"
#include "ccProxyFactoryEx.h"
#include "AvEvents.h"
#include "ccSymInstalledApps.h"
#include "AVModuleNames.h"

namespace AV
{
typedef CSymInterfaceManagedDLLHelper<&sz_NAVEventFactory_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheMgdLoader,
                               ccEvtMgr::CEventFactoryEx, 
                               &ccEvtMgr::IID_EventFactoryEx, 
                               &ccEvtMgr::IID_EventFactoryEx> NAVEventFactory_CEventFactoryEx;

typedef CSymInterfaceManagedDLLHelper<&sz_NAVEventFactory_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheMgdLoader,
                               ccEvtMgr::CEventFactoryEx2, 
                               &ccEvtMgr::IID_EventFactoryEx2, 
                               &ccEvtMgr::IID_EventFactoryEx2> NAVEventFactory_CEventFactoryEx2;

typedef CSymInterfaceManagedDLLHelper<&sz_NAVEventFactory_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheMgdLoader,
                               ccEvtMgr::CProxyFactoryEx, 
                               &ccEvtMgr::IID_ProxyFactoryEx, 
                               &ccEvtMgr::IID_ProxyFactoryEx> NAVEventFactory_CProxyFactoryEx;

typedef CSymInterfaceManagedDLLHelper<&sz_NAVEventFactory_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheMgdLoader,
                               AV::IAvEventFactory, 
                               &AV::IID_AvEventFactory, 
                               &AV::IID_AvEventFactory> NAVEventFactory_IAVEventFactory;


} // end namespace AV