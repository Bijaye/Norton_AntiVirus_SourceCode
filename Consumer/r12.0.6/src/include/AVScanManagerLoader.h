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
        IScanManager,
        &IID_IScanManager,
        &IID_IScanManager> AVScanManager_IScanManagerFactory;
    
    typedef CSymInterfaceDLLHelper<&sz_ScanManager_dll,
        ccSym::CNAVPathProvider,
        cc::CSymInterfaceTrustedLoader,
        ISingleInstance,
        &CLSID_SingleInstance,
        &IID_SingleInstance> AVScanManager_ISingleInstanceFactory;

} // end namespace AV