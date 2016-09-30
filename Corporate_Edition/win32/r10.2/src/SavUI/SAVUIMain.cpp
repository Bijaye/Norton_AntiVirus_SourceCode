// SavUI.cpp : Implementation of WinMain

#include "stdafx.h"
#include "SavAssert.h"
#include "RegUtils.h"
#include "vpcommon.h"

#include "resource.h"
#include "SavUI.h"

class CSavUIModule : public CAtlExeModuleT< CSavUIModule >
{
public :
	DECLARE_LIBID(LIBID_SavUILib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SAVUI, "{2E259067-4D17-4873-9D0E-BC56A2A99124}")
};

CSavUIModule _AtlModule;


//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR lpCmdLine, int nShowCmd)
{
    if( 0 == stricmp(lpCmdLine, _T("/RegServer")) )
    {
        SAVVERIFY(SUCCEEDED(_AtlModule.UpdateRegistryFromResourceS(IDR_VIRUSFOUNDCOMADAPTER,  TRUE)));
        SAVVERIFY(SUCCEEDED(_AtlModule.UpdateRegistryFromResourceS(IDR_RESULTSVIEWCOMADAPTER, TRUE)));
        SAVVERIFY(SUCCEEDED(_AtlModule.RegisterServer(TRUE)));
    }
    else if( 0 == stricmp(lpCmdLine, _T("/UnregServer")) )
    {
        SAVVERIFY(SUCCEEDED(_AtlModule.UpdateRegistryFromResourceS(IDR_VIRUSFOUNDCOMADAPTER,  FALSE)));
        SAVVERIFY(SUCCEEDED(_AtlModule.UpdateRegistryFromResourceS(IDR_RESULTSVIEWCOMADAPTER, FALSE)));
        SAVVERIFY(SUCCEEDED(_AtlModule.RegisterServer(FALSE)));
    }

    CSecurityDescriptor		objSecurityDescriptor;
    objSecurityDescriptor.InitializeFromProcessToken();
    CoInitializeSecurity(objSecurityDescriptor, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT , RPC_C_IMP_LEVEL_IDENTIFY, NULL, NULL, NULL);

	// Initialize RegUtils.
    HKEY hMainKey;
		// This project defines RU_USE_STATIC_MAINKEY, so it doesn't need to
		// declare hMainKey with global scope.
    DWORD dwErr = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
    if ( dwErr == ERROR_SUCCESS )
	{
		InitRegUtils(hMainKey);
	}

    return _AtlModule.WinMain(nShowCmd);
}

