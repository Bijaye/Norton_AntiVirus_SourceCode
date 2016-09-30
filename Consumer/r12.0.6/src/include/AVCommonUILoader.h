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