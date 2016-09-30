////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ccSymInterfaceLoader.h>
#include <ccSymPathProvider.h>
#include "AvProdSvcClientInterface.h"

#ifdef INITIIDS
extern LPCWSTR sz_AvProdSvc_dll = L"AVPSVC32.dll";
#else
extern LPCWSTR sz_AvProdSvc_dll;
#endif

namespace AvProdSvc
{

    // {DBC9E8E9-29FB-4114-A61F-1A6081BEF006}
    SYM_DEFINE_INTERFACE_ID(CLSID_AvProdSvcClient, 
        0xdbc9e8e9, 0x29fb, 0x4114, 0xa6, 0x1f, 0x1a, 0x60, 0x81, 0xbe, 0xf0, 0x6);

    typedef CSymInterfaceManagedDLLHelper<&sz_AvProdSvc_dll,
        ccSym::CNAVPathProvider,
        cc::CSymInterfaceTrustedCacheMgdLoader,
        AvProdSvc::IAvProdSvcClient,
        &CLSID_AvProdSvcClient,
        &AvProdSvc::IID_AvProdSvcClient> AvProdSvcClient_MgdLoader;

    typedef CSymInterfaceDLLHelper<&sz_AvProdSvc_dll,
        ccSym::CNAVPathProvider,
        cc::CSymInterfaceTrustedLoader,
        AvProdSvc::IAvProdSvcClient,
        &CLSID_AvProdSvcClient,
        &AvProdSvc::IID_AvProdSvcClient> AvProdSvcClient_Loader;

}
