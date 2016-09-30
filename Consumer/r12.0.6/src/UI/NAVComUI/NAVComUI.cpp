// NAVComUI.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NAVComUIps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"

#define INITGUID
#include <InitGuid.h>

#include "SymInterfaceInit.h"

#define SIMON_INITGUID
#include <SIMON.h>

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#include "ComponentPepHelper.h"

#include "CommonUIInterface.h"
#include "NAVComUI.h"
#include "NAVComUI_i.c"
#include "AxSysListView32.h"
#include "AxSysListView32OAA.h"

#include "ccResourceLoader.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVComUI"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

bool _InitResources();
#define INIT_RESOURCES() \
    _InitResources();

#define _INIT_GIPLIP
#include "GipLip.h"

#include "OEHeurUI.h"
#include "ScanRepairRemoveWnd.h"
#include "mainscanframe.h"

SYM_OBJECT_MAP_BEGIN()
    INIT_RESOURCES()
    SYM_OBJECT_ENTRY(IID_ICommonUI, CMainScanFrame)
SYM_OBJECT_MAP_END()

CAppModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_AxSysListView32, CAxSysListView32)
    OBJECT_ENTRY(CLSID_AxSysListView32OAA, CAxSysListView32OAA)
END_OBJECT_MAP()
SIMON::CSimonModule _SimonModule;

// Must have a global module class named _ModuleRes for CResourceLoader to work
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
CString _g_csHandlerPath;
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVCOMUILib);
        DisableThreadLibraryCalls(hInstance);
        CString sModuleLocation;
        TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
        long nRet =::GetModuleFileName(hInstance,_szModuleLocation,sizeof(_szModuleLocation));
        ::GetShortPathName(_szModuleLocation,_szModuleLocation,sizeof(_szModuleLocation));
        CString sModuleFileName = _szModuleLocation;
        if(nRet != 0)
        {
            sModuleLocation = _szModuleLocation;
            TCHAR* pLast = _tcsrchr(_szModuleLocation,_T('\\'));
            *pLast = _T('\0');
            sModuleLocation = _szModuleLocation;
        }
        _g_csHandlerPath = sModuleLocation;
        _g_csHandlerPath += _T("\\NavOptRF.dll");
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
        _Module.Term();
	}
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
    // init resources
    _InitResources();

    return _Module.GetClassObject(rclsid, riid, ppv);
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

bool WINAPI CreateOEHeurUI(IOEHeurUI** ppOEHeurUI)
{
	_InitResources();

	COEHeurUI* pOEHeurUI = new COEHeurUI;

	if (pOEHeurUI == NULL)
	{
		CCTRACEE("Error: failed to create Outbound Email Heuristic dialog.");
		return false;
	}

    pOEHeurUI->AddRef();
	*ppOEHeurUI = pOEHeurUI;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// _InitResources - Sets up resource loading
/////////////////////////////////////////////////////////////////////////////
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

