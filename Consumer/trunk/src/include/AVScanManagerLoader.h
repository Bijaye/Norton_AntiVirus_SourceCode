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
#include "ScanMgrInterface.h"
#include "AVModuleNames.h"
#include "SingleInstanceInterface.h"

namespace AV
{
    typedef CSymInterfaceDLLHelper<&sz_ScanManager_dll,
        ccSym::CNAVPathProvider,
        cc::CSymInterfaceTrustedLoader,
        IScanManagerW,
        &IID_IScanManagerW,
        &IID_IScanManagerW> AVScanManager_IScanManagerFactory;
    
    typedef CSymInterfaceDLLHelper<&sz_ScanManager_dll,
        ccSym::CNAVPathProvider,
        cc::CSymInterfaceTrustedLoader,
        ISingleInstance,
        &CLSID_SingleInstance,
        &IID_SingleInstance> AVScanManager_ISingleInstanceFactory;

} // end namespace AV