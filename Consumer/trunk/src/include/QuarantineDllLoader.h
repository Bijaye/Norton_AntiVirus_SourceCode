////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccsympathprovider.h"
#include "ccSymInterfaceLoader.h" // Has trusted loader
#include "iquaran.h"
#include "IQuarantineW.h"
#include "ccSymModuleLifetimeMgrHelper.h"

extern const LPCTSTR sz_Quarantine_dll;
                  
typedef CSymInterfaceDLLHelper<&sz_Quarantine_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheLoader,
                               IQuarantineDLL, 
                               &IID_QuarantineDLL, 
                               &IID_QuarantineDLL> QuarDLL_Loader;

typedef CSymInterfaceDLLHelper<&sz_Quarantine_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheLoader,
                               IQuarantineOpts, 
                               &IID_QuarantineOpts, 
                               &IID_QuarantineOpts> QuarOPTS_Loader;

typedef CSymInterfaceDLLHelper<&sz_Quarantine_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheLoader,
                               IQuarantineItem, 
                               &IID_QuarantineItem, 
                               &IID_QuarantineItem> QuarItem_Loader;

typedef CSymInterfaceDLLHelper<&sz_Quarantine_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheLoader,
                               IEnumQuarantineItems, 
                               &IID_EnumQuarantineItems, 
                               &IID_EnumQuarantineItems> EnumQuarItems_Loader;

typedef CSymInterfaceDLLHelper<&sz_Quarantine_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheLoader,
                               IQuarantineDLLW, 
                               &IID_QuarantineDLLW, 
                               &IID_QuarantineDLLW> QuarDLL_LoaderW;

typedef CSymInterfaceDLLHelper<&sz_Quarantine_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheLoader,
                               IQuarantineItemW, 
                               &IID_QuarantineItemW, 
                               &IID_QuarantineItemW> QuarItem_LoaderW;

typedef CSymInterfaceDLLHelper<&sz_Quarantine_dll,
                               ccSym::CNAVPathProvider, 
                               cc::CSymInterfaceTrustedCacheLoader,
                               IEnumQuarantineItemsW, 
                               &IID_EnumQuarantineItemsW, 
                               &IID_EnumQuarantineItemsW> EnumQuarItems_LoaderW;