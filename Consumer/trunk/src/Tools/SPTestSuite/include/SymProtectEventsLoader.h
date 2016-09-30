////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccSymInterfaceLoader.h"
#include "ccSymPathProvider.h"

#include "ccEventFactoryEx.h"
#include "ccProxyFactoryEx.h"
#include "SymProtectDirectInterface.h"

namespace SymProtectEvt
{

extern const LPCTSTR sz_SymProtectEvents_dll ;

typedef CSymInterfaceDLLHelper<&sz_SymProtectEvents_dll,
                               ccSym::CBBPathProvider,
                               cc::CSymInterfaceTrustedLoader,
                               ccEvtMgr::CEventFactoryEx,
                               &ccEvtMgr::IID_EventFactoryEx,
                               &ccEvtMgr::IID_EventFactoryEx> SymProtectEvents_CEventFactoryEx;

typedef CSymInterfaceDLLHelper<&sz_SymProtectEvents_dll,
                               ccSym::CBBPathProvider,
                               cc::CSymInterfaceTrustedLoader,
                               ccEvtMgr::CEventFactoryEx2,
                               &ccEvtMgr::IID_EventFactoryEx2,
                               &ccEvtMgr::IID_EventFactoryEx2> SymProtectEvents_CEventFactoryEx2;

typedef CSymInterfaceDLLHelper<&sz_SymProtectEvents_dll,
                               ccSym::CBBPathProvider,
                               cc::CSymInterfaceTrustedLoader,
                               ccEvtMgr::CProxyFactoryEx,
                               &ccEvtMgr::IID_ProxyFactoryEx,
                               &ccEvtMgr::IID_ProxyFactoryEx> SymProtectEvents_CProxyFactoryEx;

typedef CSymInterfaceDLLHelper<&sz_SymProtectEvents_dll,
                               ccSym::CBBPathProvider,
                               cc::CSymInterfaceTrustedLoader,
                               SymProtectEvt::IFallbackConfig,
                               &SymProtectEvt::IID_FallbackConfig,
                               &SymProtectEvt::IID_FallbackConfig> SymProtectEvents_IFallbackConfig;

} // namespace
