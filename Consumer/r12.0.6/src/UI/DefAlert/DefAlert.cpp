
// DefAlert.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f DefAlertps.mk in the project directory.

#include "stdafx.h"
#define INITIIDS
#include "syminterface.h"
#include "ccAlertInterface.h"
#include "ccScanInterface.h"
#include "ccVerifyTrustInterface.h"
#include "NAVTrust.h"
#include "ccSettingsInterface.h"
#include "ISWSCHelper_Loader.h"
#include "WSCHelper.h"
#include "NAVDefutilsLoader.h"

#include "resource.h"
#include "DefAlert.h"
#include "DefAlert_i.c"
#include "SymNavDefAlert.h"
#include "globalmutex.h"
#include "ccappplugin.h"

#include "Simon.h"
#include "SSSecurity.CPP"
#include "NavOptionRefreshHelperInterface.h"
#include "SymAlertInterface.h"
#include "DefAlertPlugin.h"
#include "NAVLicensingAlert.h"
#include "NAVSubscriptionAlert.h"
#include "NAVDefinitionsAlert.h"
#include "NAVALUDisabledAlert.h"
#include "NAVAlertDlg.h"
#include "NetDetectController.h"                // For AutoLiveUpdate
#include "NetDetectController_i.c"

#include "ISymMceCmdLoader.h"	// detect optional Media center components

//Resource loading
#include "..\defalertres\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("defalert.loc")
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);


///////////////////////////////////////////////////////////////////////////////
// COM INTERFACE

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_SymNavDefAlert, CSymNavDefAlert)
END_OBJECT_MAP()

///////////////////////////////////////////////////////////////////////////////
// SYM INTERFACE

// {A3684D18-A8D6-4f54-86FB-A11DF7C5E1DB}
SYM_DEFINE_OBJECT_ID(DEFALERT_OBJECT_ID, 
	0xa3684d18, 0xa8d6, 0x4f54, 0x86, 0xfb, 0xa1, 0x1d, 0xf7, 0xc5, 0xe1, 0xdb);

DECLARE_CCAPP_PLUGIN(DEFALERT_OBJECT_ID)

SYM_OBJECT_MAP_BEGIN()                          
	SYM_OBJECT_ENTRY(DEFALERT_OBJECT_ID, CDefAlertPlugin) 
SYM_OBJECT_MAP_END()                            

///////////////////////////////////////////////////////////////////////////////
// SIMON INTERFACE

SIMON::CSimonModule _SimonModule;

BEGIN_SIMON_OBJECT_MAP(SimonObjectMap)
	SIMON_OBJECT_ENTRY(CLSID_CLicensingAlert, CLicensingAlert)
	SIMON_OBJECT_ENTRY(CLSID_CSubscriptionAlert, CSubscriptionAlert)
	SIMON_OBJECT_ENTRY(CLSID_CDefinitionsAlert, CDefinitionsAlert)
	SIMON_OBJECT_ENTRY(CLSID_CALUDisabledAlert, CALUDisabledAlert)
	SIMON_OBJECT_ENTRY(CLSID_CAlertDlg, CAlertDlg)
END_SIMON_OBJECT_MAP()

///////////////////////////////////////////////////////////////////////////////

CString _g_csHandlerPath;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hInstance);
			_SimonModule.Init(SimonObjectMap);
			_Module.Init(ObjectMap, hInstance, &LIBID_DEFALERTLib);
			
			// Enable support for OLE container windows.
			AtlAxWinInit();

            // Fetch the handler dll path
            ///////////////////////////////////////////////////////////////////////
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
		break;

	case DLL_PROCESS_DETACH:
		{
			_SimonModule.Term();
			_Module.Term();
		}
		break;

    case DLL_THREAD_ATTACH:
        {

        }
        break;
    case DLL_THREAD_DETACH:
        {

        }
        break;
	}
    
	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE.

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}


void _InitResources()
{
	if( NULL == g_ResLoader.GetResourceInstance() ) 
	{
		if( g_ResLoader.Initialize() ) 
	    {
            _Module.SetResourceInstance( g_ResLoader.GetResourceInstance());
	    }
	}
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type.

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	_InitResources();
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry.

STDAPI DllRegisterServer(void)
{
    // Registers object, typelib and all interfaces in typelib.

	_InitResources();
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry.

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}

//=======================================================================
//== SimonGetClassObject == SimonGetClassObject == SimonGetClassObject ==
//=======================================================================

// this function must be implemented and exported from your library.
HRESULT SIMON_STDMETHODCALLTYPE SimonGetClassObject(
  REFSIMON_CLSID rclsid,    //CLSID for the class object
  REFSIMON_IID riid,        //Reference to the identifier of the interface 
                            // that communicates with the class object
  void** ppv               //Address of output variable that receives the 
                            // interface pointer requested in riid
)
{
    return _SimonModule.GetClassObject(rclsid,riid,ppv);
}
