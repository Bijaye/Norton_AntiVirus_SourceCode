// NAVTasks.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NAVTasksps.mk in the project directory.

#include "stdafx.h"
#define INITIIDS
#include "ScanTask.h"
#include "NAVTrust.h"
#include "resource.h"
#include "..\NavTasksRes\ResResource.h"
#include <initguid.h>
#include "ccSettingsInterface.h"
#include "NAVTasks.h"
#include "Scheduler.h"
#include "ScanTaskObj.h"
#include "ScanTasksObj.h"
#include "NAVTasksDlgsObj.h"
#include "ccResourceLoader.h"
// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;
bool	_InitResources();

// Must have a global module class named _ModuleRes for CResourceLoader to work
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

CAppModule _Module;
CNAVInfo g_NAVInfo;

#define INIT_RESOURCES() \
	if( false == _InitResources() )\
	return SYMERR_INVALID_FILE;\

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Scheduler, CScheduler)
OBJECT_ENTRY(CLSID_NAVScanTask, CNAVScanTask)
OBJECT_ENTRY(CLSID_NAVScanTasks, CNAVScanTasks)
OBJECT_ENTRY(CLSID_NAVTasksDlgs, CNAVTasksDlgs)
END_OBJECT_MAP()

SYM_OBJECT_MAP_BEGIN()
INIT_RESOURCES()
SYM_OBJECT_ENTRY(IID_IScanTask, CScanTask)
SYM_OBJECT_MAP_END()

bool	_InitResources()
{
	bool	bRet = true;
	if( NULL == g_ResLoader.GetResourceInstance() ) 
	{
		if( g_ResLoader.Initialize() ) 
		{
			_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
		}
		else
		{
			CCTRACEE(_T("Failed to Initialize resources."));
			bRet = false;
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVTASKSLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	if( _InitResources() )
	{
		return 	_Module.GetClassObject(rclsid, riid, ppv);
	}
	else
	{
		return E_FAIL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}

