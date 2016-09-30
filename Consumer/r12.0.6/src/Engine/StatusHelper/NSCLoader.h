#pragma once

#include "ccSymInterfaceLoader.h" // Has trusted loader
#include "ccSymInstalledApps.h"

#include "nscIConsoleServer.h"
#include "nscIWrapper.h"

extern const LPCTSTR sz_NSCAPI_dll;

class CNSCPathProvider
{
public:
    static bool GetPath(LPTSTR szPath, size_t& dwSize) throw()
    {
		ATL::CRegKey rkInstalledApps;
		if(ERROR_SUCCESS == rkInstalledApps.Open(HKEY_LOCAL_MACHINE,
											_T("Software\\Symantec\\InstalledApps"), KEY_READ))
		{
            ccLib::CString sPath;
            if(ccSym::CInstalledApps::GetInstAppsDirectory(_T("NSC"), sPath))
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
                  
typedef CSymInterfaceDLLHelper<&sz_NSCAPI_dll,
                                CNSCPathProvider, 
                                cc::CSymInterfaceTrustedCacheLoader,
                                nsc::IConsoleServer, 
                                &nsc::IID_nscIConsoleServer, 
                                &nsc::IID_nscIConsoleServer> NSC_IConsoleServerLoader;
