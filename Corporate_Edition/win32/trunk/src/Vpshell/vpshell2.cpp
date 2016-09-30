// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// vpshell2.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f vpshell2ps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#define INITIIDSCComModule _Module;
#include "DarwinResCommon.h"
#include "initguid.h"
#include "vpshell2.h"
#include "VpshellEx.h"

#include "OSMigrat.h" //EA for Apllication migration of Win2K


// class id for virus procted shell extentions
//{BDA77241-42F6-11d0-85E2-00AA001FE28C}
const CLSID CLSID_VpshellEx = { 0xBDA77241, 0x42F6, 0x11D0, { 0x85, 0xE2, 0x0, 0xAA, 0x0, 0x1F, 0xE2, 0x8C } };


CComModule _Module;

DARWIN_DECLARE_RESOURCE_LOADER(_T("VpShellRes.dll"), _T("VpShell"))

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_VpshellEx, CVpshellEx)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_Module.Init(ObjectMap, hInstance);
		DisableThreadLibraryCalls(hInstance);
		//EA Application Migration for Win2K
		if(DetermineOSMigrationMessage() == 95)
		{
			DllUnregisterServer();
			return FALSE;
		}
		else if(DetermineOSMigrationMessage() == 98)
		{
			DllUnregisterServer();
			return FALSE;
		}
		//EA - END Application Migration for Win2K
		
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
HRESULT InitResources()
{
	HRESULT hr = S_OK;

	if(NULL == g_ResLoader.GetResourceInstance())
	{
		if(SUCCEEDED(hr = g_ResLoader.Initialize()))
		{
			_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
		}
		else
		{
			hr = E_FAIL;
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	HRESULT hr;

	if(FAILED(hr = InitResources()))
		return hr;
	else
		return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	HRESULT hr;

	if(FAILED(hr = InitResources()))
		return hr;
	else
		return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer();
	//_Module.UnRegisterTypeLib();
	return S_OK;
}



