#pragma once

#include "ccSymInterfaceLoader.h" // Has trusted loader
#include "ccSymInstalledApps.h"

#include "IWPSettingsInterface.h"

extern const LPCTSTR sz_IWPSettings_dll;

class CIWPPathProvider
{
public:
    static bool GetPath(LPTSTR szPath, size_t& dwSize) throw()
    {
		ATL::CRegKey rkInstalledApps;
		if(ERROR_SUCCESS == rkInstalledApps.Open(HKEY_LOCAL_MACHINE,
											_T("Software\\Symantec\\InstalledApps"), KEY_READ))
		{
            ccLib::CString sPath;
            if(ccSym::CInstalledApps::GetInstAppsDirectory(_T("IWP"), sPath))
            {
                _tcsncpy(szPath, sPath, dwSize);
                return true;
            }
            else
            {
                return false;
            }
		}

        return false;
    }
};
                  
typedef CSymInterfaceManagedDLLHelper<&sz_IWPSettings_dll,
                                      CIWPPathProvider, 
                                      cc::CSymInterfaceTrustedCacheMgdLoader,
                                      IWP::IIWPSettings, 
                                      &IWP::IID_IWPSettings, 
                                      &IWP::IID_IWPSettings> IWP_IWPSettings;

typedef CSymInterfaceDLLHelper<&sz_IWPSettings_dll,
                                      CIWPPathProvider, 
                                      cc::CSymInterfaceTrustedLoader,
                                      IWP::IIWPSettings, 
                                      &IWP::IID_IWPSettings, 
                                      &IWP::IID_IWPSettings> IWP_IWPSettingsNoCache;