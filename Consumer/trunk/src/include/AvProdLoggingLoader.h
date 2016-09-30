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
#include "AvProdLoggingInterface.h"

#ifdef INITIIDS
extern LPCWSTR sz_AvProdLogging_dll = L"NAVLOGV.dll";
#else
extern LPCWSTR sz_AvProdLogging_dll;
#endif

namespace AvProd
{

    namespace AvModuleLogging
    {
        // {DBC9E8E9-29FB-4114-A61F-1A6081BEF006}
        SYM_DEFINE_INTERFACE_ID(CLSID_AutoProtectClientActivities, 
            0xdbc9e8e9, 0x29fb, 0x4114, 0xa6, 0x1f, 0x1a, 0x60, 0x81, 0xbe, 0xf0, 0x6);

        typedef CSymInterfaceManagedDLLHelper<&sz_AvProdLogging_dll,
            ccSym::CNAVPathProvider,
            cc::CSymInterfaceTrustedCacheMgdLoader,
            AvModuleLogging::IAutoProtectClientActivities,
            &CLSID_AutoProtectClientActivities,
            &AvModuleLogging::IID_AutoProtectClientActivities> AutoProtectClientActivities_Loader;


        // {6B7E57FE-7DA5-4fb3-B334-2B21EE4C82C7}
        SYM_DEFINE_INTERFACE_ID(CLSID_ManualScanClientActivities, 
            0x6b7e57fe, 0x7da5, 0x4fb3, 0xb3, 0x34, 0x2b, 0x21, 0xee, 0x4c, 0x82, 0xc7);

        typedef CSymInterfaceManagedDLLHelper<&sz_AvProdLogging_dll,
            ccSym::CNAVPathProvider,
            cc::CSymInterfaceTrustedCacheMgdLoader,
            AvModuleLogging::IManualScanClientActivities,
            &CLSID_ManualScanClientActivities,
            &AvModuleLogging::IID_ManualScanClientActivities> ManualScanClientActivities_Loader;


        // {DF6C1BC0-0451-4ecf-B46E-C885DD6020D8}
        SYM_DEFINE_INTERFACE_ID(CLSID_EmailScanClientActivities, 
            0xdf6c1bc0, 0x451, 0x4ecf, 0xb4, 0x6e, 0xc8, 0x85, 0xdd, 0x60, 0x20, 0xd8);

        typedef CSymInterfaceManagedDLLHelper<&sz_AvProdLogging_dll,
            ccSym::CNAVPathProvider,
            cc::CSymInterfaceTrustedCacheMgdLoader,
            AvModuleLogging::IEmailScanClientActivities,
            &CLSID_EmailScanClientActivities,
            &AvModuleLogging::IID_EmailScanClientActivities> EmailScanClientActivities_Loader;

        // {F292FDC8-D3C2-46cb-AC4C-868A8FDB40AE}
        SYM_DEFINE_INTERFACE_ID(CLSID_ThreatDataEz, 
            0xf292fdc8, 0xd3c2, 0x46cb, 0xac, 0x4c, 0x86, 0x8a, 0x8f, 0xdb, 0x40, 0xae);

        typedef CSymInterfaceManagedDLLHelper<&sz_AvProdLogging_dll,
            ccSym::CNAVPathProvider,
            cc::CSymInterfaceTrustedCacheMgdLoader,
            AvModuleLogging::IThreatDataEz,
            &CLSID_ThreatDataEz,
            &AvModuleLogging::IID_ThreatDataEz> ThreatDataEz_Loader;
    }
}
