#pragma once

#include "ccSymInterfaceLoader.h"
#include "ccSymPathProvider.h"

#include "ccEventFactoryEx.h"
#include "ccProxyFactoryEx.h"
#include "ccSymInstalledApps.h"

#include "HPPEventsInterface.h"
#include "HPPModuleNames.h"

namespace HPP
{


typedef CSymInterfaceDLLHelper<&sz_HPPEventHelper_dll,
                               ccSym::CNAVPathProvider, 
                               CSymInterfaceLoader,
                               ccEvtMgr::CEventFactoryEx, 
                               &ccEvtMgr::IID_EventFactoryEx, 
                               &ccEvtMgr::IID_EventFactoryEx> HPPEventFactory_CEventFactoryEx;

typedef CSymInterfaceDLLHelper<&sz_HPPEventHelper_dll,
                               ccSym::CNAVPathProvider, 
                               CSymInterfaceLoader,
                               ccEvtMgr::CEventFactoryEx2, 
                               &ccEvtMgr::IID_EventFactoryEx2, 
                               &ccEvtMgr::IID_EventFactoryEx2> HPPEventFactory_CEventFactoryEx2;

typedef CSymInterfaceDLLHelper<&sz_HPPEventHelper_dll,
                               ccSym::CNAVPathProvider, 
                               CSymInterfaceLoader,
                               ccEvtMgr::CProxyFactoryEx, 
                               &ccEvtMgr::IID_ProxyFactoryEx, 
                               &ccEvtMgr::IID_ProxyFactoryEx> HPPEventFactory_CProxyFactoryEx;


} // end namespace AV