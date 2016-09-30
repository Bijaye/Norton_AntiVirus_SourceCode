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
