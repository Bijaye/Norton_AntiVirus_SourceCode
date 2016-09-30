#include "SymInterfaceLoader.h"
#include "ccSymInterfaceLoader.h"
#include "IICFManager.h"
#include "ccSymPathProvider.h"
#include "NAVInfo.h"

extern const LPCTSTR sz_ICFMgr_dll;

class IWPPathProvider
{
public:
    static bool GetPath(LPTSTR szPath, size_t& nSize) throw()
	{
		ATL::CRegKey rkInstalledApps;
        DWORD dwSize = nSize;
		if(ERROR_SUCCESS == rkInstalledApps.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps")))
		{
			rkInstalledApps.QueryStringValue(_T("IWP"), szPath, &dwSize);
		}

		return true;
	}
};

typedef CSymInterfaceDLLHelper<&sz_ICFMgr_dll,
                               IWPPathProvider, 
							   cc::CSymInterfaceTrustedCacheLoader,
                               IICFManager, 
                               &IID_ICFManager, 
                               &IID_ICFManager> NAVICFManagerLoader;