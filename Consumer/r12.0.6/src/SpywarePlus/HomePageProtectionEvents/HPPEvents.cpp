// HPPEvents.cpp : Implementation of DLL Exports.

#define INITIIDS

#include "stdafx.h"
#include "resource.h"
#include "HPPEvents_h.h"

#include "HPPEventsInterface.h"
#include "ccEventUtil.h"
#include "HPPUEMModule.h"
#include "HPPEventFactory.h"
#include "ccEventManagerHelper.h"
#include "ccSymDebugOutput.h"

#include "HPPEventCommonImpl.h"

#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccSymDelayLoader.h"

cc::CResourceLoader g_Resources(&_AtlBaseModule, _T("HPPRES32.loc"));
cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);


// ccTrace Implementation
ccSym::CDebugOutput g_DebugOutput(_T("HPPEVT32"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

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

#define INIT_RESOURCES() \
	if(false == _InitResources()) \
		return SYMERR_INVALID_FILE;

// The SymInterface object map for this module.
SYM_OBJECT_MAP_BEGIN()     
	INIT_RESOURCES()
	SYM_OBJECT_ENTRY(ccEvtMgr::IID_EventFactoryEx, CHPPEventFactory)
	SYM_OBJECT_ENTRY(ccEvtMgr::IID_EventFactoryEx2, CHPPEventFactory)
	SYM_OBJECT_ENTRY( ccEvtMgr::IID_EventManagerModuleEx, CHPPUEMModule )
SYM_OBJECT_MAP_END()                            

DECLARE_CEVENTEXINT_CLASS(HPPEventCommonImpl, LIBID_HPPEventsLib, IDR_HPPEVENTCOMMON)

class CHPPEventsModule : public CAtlDllModuleT< CHPPEventsModule >
{
public :
	DECLARE_LIBID(LIBID_HPPEventsLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HPPEVENTS, "{6F0E1D22-EA5A-4f4e-8185-6A3F4CCF17FE}")
};

CHPPEventsModule _AtlModule;


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
