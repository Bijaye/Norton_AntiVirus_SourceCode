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
#include "CommonUIInterface.h"
#include "AVModuleNames.h"

namespace AV
{
    typedef CSymInterfaceDLLHelper<&sz_NAVCommonUI_dll,
        ccSym::CNAVPathProvider,
        cc::CSymInterfaceTrustedCacheLoader,
        ICommonUI,
        &IID_ICommonUI,
        &IID_ICommonUI> AVCommonUI_ICommonUIFactory;
} // end namespace AV