////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVEvents.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NAVEventsps.mk in the project directory.

#include "stdafx.h"

#include "NAVEventResource.h"       // main symbols
#include <initguid.h>

#include "NAVEvents.h"
#include "NAVEvents_i.c"

#include "NAVEventSubscriber.h"
#include "NAVEventProvider.h"

#include "NAVEventCommon.h"
#include "NAVEventCommonInt.h"

#include "InterfaceHelper.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_NAVEventCommonInt, CNAVEventCommonInt)
OBJECT_ENTRY_NON_CREATEABLE(CNAVEventSubscriber)
OBJECT_ENTRY_NON_CREATEABLE(CNAVEventProvider)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVEVENTSLib);
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
			return hrTrust;
		}
	}

	// Serve up the object.
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


