#pragma once

#include "ccSymInterfaceLoader.h"
#include "ccSymPathProvider.h"
#include "ExclusionManagerInterface.h"
#include "AVModuleNames.h"

namespace NavExclusions
{
    typedef CSymInterfaceDLLHelper<&AV::sz_Exclusions_dll,
        ccSym::CNAVPathProvider,
        cc::CSymInterfaceTrustedCacheLoader,
        IExclusionManager,
        &IID_IExclusionManager,
        &IID_IExclusionManager> N32Exclu_IExclusionManagerFactory;
} // end namespace NavExclusions
