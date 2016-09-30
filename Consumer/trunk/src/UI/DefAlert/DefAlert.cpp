////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// DefAlert.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f DefAlertps.mk in the project directory.

#include "stdafx.h"
#define INITIIDS
#include <initguid.h>
#include "syminterface.h"
#include "ccAlertInterface.h"
#include "ccScanInterface.h"
#include "ccVerifyTrustInterface.h"
#include "NAVTrust.h"
#include "ccSettingsInterface.h"
#include "uiNumberDataInterface.h"
#include "uiDateDataInterface.h"
#include "ISDataClientLoader.h"
#include "AvInterfaces.h" // AntiVirus component
#include "isErrorLoader.h"
#include "uiPEPConstants.h"

// IDS stuff
#include "SymIDSI.h"
#include "IDSLoaderLink.h"

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
#include "interfacehelper.h"
#include "SymTheme.h"

#include "ISymMceCmdLoader.h"	// detect optional Media center components

#include "ccLibDllLink.h"

//Resource loading
#include "..\defalertres\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("defalert.loc")
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

ATL::CDynamicStdCallThunk::CThunksHeap ATL::CDynamicStdCallThunk::s_ThunkHeap;

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
	static bool bTrustVerified = false;

	if (!bTrustVerified)
	{
		CSymDLL_ReverseTrustCheck trustcheck;
		HRESULT hrTrust = trustcheck.DoCheck();	// check the calling process
		if (hrTrust == S_OK) 
		{
			bTrustVerified = true;
		}
		else
		{
			CCTRCTXE0(L"Trust check failed");
			return hrTrust;
		}
	}

	_InitResources();

	// Serve up the object.
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
