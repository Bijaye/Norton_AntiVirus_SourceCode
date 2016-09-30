#pragma once

#include "ccSymInterfaceLoader.h"
#include "ccSymPathProvider.h"

#include "ccSymInstalledApps.h"

#include "HPPUtilityInterface.h"
#include "HPPModuleNames.h"

namespace HPP
{


typedef CSymInterfaceDLLHelper<&sz_HPPUtility_dll,
                               ccSym::CNAVPathProvider, 
                               CSymInterfaceLoader,
                               IHPPUtilityInterface, 
                               &CLSID_HPPUtility, 
                               &IID_HPPUtilityInterface> CHPPUtilityLoader;


} // end namespace HPP