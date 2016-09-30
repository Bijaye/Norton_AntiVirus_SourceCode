// HomePageProtectionScriptableObj.cpp : Implementation of DLL Exports.

#define INITIIDS

#include "stdafx.h"
#include "resource.h"
#include "HomePageProtectionScriptableObj.h"

#include "HPPEventHelper.h"
#include "ccLogFactoryHelper.h"
#include "BBEventId.h"
#include "BBSystemMonitorEventsInterface.h"
#include "SymProtectEventsInterface.h"

#include "HPPUtilityInterface.h"
#include "HPPUtilityImpl.h"

#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"

#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccSymDelayLoader.h"

cc::CResourceLoader g_Resources(&_AtlBaseModule, _T("HPPRES32.loc"));

cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);

// TRACE implementation
ccSym::CDebugOutput g_Log(_T("HPPSCR32"));
IMPLEMENT_CCTRACE(::g_Log);

// ccDelayLoader implementation
ccSym::CDelayLoader g_DelayLoader;


bool _InitResources()
{
	bool bRet = true;
	if(NULL == g_Resources.GetResourceInstance())
	{
		if(g_Resources.Initialize())
		{
			_AtlBaseModule.SetResourceInstance(g_Resources.GetResourceInstance());
		}
		else
		{
			CCTRACEE(_T("Failed to Initialize resources."));
			bRet = false;
		}
	}
	return bRet;
}

#define INIT_RESOURCES_SYMINTERFACE() \
	if(false == _InitResources()) \
		return SYMERR_INVALID_FILE;

#define INIT_RESOURCES_MSCOM() \
    if(false == _InitResources()) \
    return FALSE;

bool _DoTrustCheck()
{
    static bool bTrustVerified = false;

    if (!bTrustVerified)
    {
        // Load ccVerifyTrust.dll
        ccVerifyTrust::IVerifyTrustPtr pVerifyTrust;
        if (SYM_SUCCEEDED(ccVerifyTrust::ccVerifyTrustMgd_IVerifyTrust::CreateObject(GETMODULEMGR(),pVerifyTrust.m_p)) &&
            pVerifyTrust != NULL)
        {
            if (pVerifyTrust->Create(true) == ccVerifyTrust::eNoError)
            {
                // Always check trust, regardless of policy
                pVerifyTrust->SetEnabled(true);
                
                // Check trust on current process
                if (pVerifyTrust->VerifyCurrentProcess(ccVerifyTrust::eSymantecSignature) != ccVerifyTrust::eVerifyError)
                {
                    // We have determined that the current process is a Symantec signed image.
                    bTrustVerified = true;
                }   
            }
        }
    }

    return bTrustVerified;
}

// Verify Trust macro
#define VERIFY_TRUST() \
    if(false == _DoTrustCheck()) \
    return E_ACCESSDENIED;


// The SymInterface object map for this module.
SYM_OBJECT_MAP_BEGIN() 
	INIT_RESOURCES_SYMINTERFACE()
    SYM_OBJECT_ENTRY( HPP::CLSID_HPPUtility,  CHPPUtilityImpl )
SYM_OBJECT_MAP_END()                            


class CHomePageProtectionScriptableObjModule : public CAtlDllModuleT< CHomePageProtectionScriptableObjModule >
{
public :
	DECLARE_LIBID(LIBID_HomePageProtectionScriptableObjLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HOMEPAGEPROTECTIONSCRIPTABLEOBJ, "{A7503E3F-F7CB-4B87-95C8-FE58A5B70BBC}")
};

CHomePageProtectionScriptableObjModule _AtlModule;


// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if(DLL_PROCESS_ATTACH == dwReason)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hInstance);
    }

    BOOL bRet = _AtlModule.DllMain(dwReason, lpReserved);
    return bRet;
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    VERIFY_TRUST();

    INIT_RESOURCES_MSCOM();

    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}
