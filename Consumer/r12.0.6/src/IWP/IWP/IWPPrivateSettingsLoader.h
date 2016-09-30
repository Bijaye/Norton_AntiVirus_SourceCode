#pragma once

#include "ccSymInterfaceLoader.h"

#include "IWPPrivateSettingsInterface.h"
#include "IWPSettingsLoader.h"

typedef CSymInterfaceManagedDLLHelper<&sz_IWPSettings_dll,
                                      CIWPPathProvider, 
                                      cc::CSymInterfaceTrustedCacheMgdLoader,
                                      IWP::IIWPPrivateSettings, 
                                      &IWP::IID_IWPPrivateSettings, 
                                      &IWP::IID_IWPPrivateSettings> IWP_IWPPrivateSettings;