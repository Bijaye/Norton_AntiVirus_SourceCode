////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccSymInterfaceLoader.h" // Has trusted loader
#include "ccSymInstalledApps.h"

#include "IWPSettingsInterface.h"

extern const LPCTSTR sz_IWPSettings_dll;

class CIWPPathProvider
{
public:	// dwSize is bytes, not characters!
    static bool GetPath(LPTSTR szPath, size_t& dwSize) throw()
    {
		ATL::CRegKey rkInstalledApps;
		if(ERROR_SUCCESS == rkInstalledApps.Open(HKEY_LOCAL_MACHINE,
											_T("Software\\Symantec\\InstalledApps"), KEY_READ))
		{
            ccLib::CString sPath;
            if(ccSym::CInstalledApps::GetInstAppsDirectory(_T("IWP"), sPath))
            {
				if ( (sPath.GetLength()+1)*sizeof(TCHAR) < dwSize )
				{
					_tcsncpy(szPath, sPath, sPath.GetLength()+1);
					return true;
				}
				else
					return false;
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