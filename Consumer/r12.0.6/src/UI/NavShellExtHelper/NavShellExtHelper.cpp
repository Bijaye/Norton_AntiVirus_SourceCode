// NavShellExtHelper.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "NavShellExtHelper.h"
#include <initguid.h>
#define SIMON_INITGUID
#include <SIMON.h>
#include <SymInterface.h>
// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#include "ComponentPepHelper.h"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

SIMON::CSimonModule _SimonModule;

class CNavShellExtHelperModule : public CAtlExeModuleT< CNavShellExtHelperModule >
{
public :
	DECLARE_LIBID(LIBID_NavShellExtHelperLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NAVSHELLEXTHELPER, "{ED14BD80-37DB-4A36-BC35-E17A7054038A}")
};

CNavShellExtHelperModule _AtlModule;

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
#ifdef IMPERSONATE_CLIENT
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// This provides a NULL DACL which will allow access to everyone.
    CSecurityDescriptor sd;
    sd.InitializeFromThreadToken();
    hr = CoInitializeSecurity(sd, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    _ASSERTE(SUCCEEDED(hr));
#endif

	int iRet = _AtlModule.WinMain(nShowCmd);

#ifdef IMPERSONATE_CLIENT
	CoUninitialize();
#endif
	return iRet;
}

