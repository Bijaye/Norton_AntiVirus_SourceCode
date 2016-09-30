// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPProtectionProviderUI.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "HPPProtectionProviderUI.h"



class CHPPProtectionProviderUIModule : public CAtlDllModuleT< CHPPProtectionProviderUIModule >
{
public :
	DECLARE_LIBID(LIBID_HPPProtectionProviderUILib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_HPPPROTECTIONPROVIDERUI, "{163A15AA-F8B0-4A44-8B5D-8C40F9B46E66}")
};

CHPPProtectionProviderUIModule _AtlModule;

class CHPPProtectionProviderUIApp : public CWinApp
{
public:

// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CHPPProtectionProviderUIApp, CWinApp)
END_MESSAGE_MAP()

CHPPProtectionProviderUIApp theApp;

BOOL CHPPProtectionProviderUIApp::InitInstance()
{
	CWinApp::InitInstance();    
	AfxEnableControlContainer();
	return TRUE;
}

int CHPPProtectionProviderUIApp::ExitInstance()
{
    return CWinApp::ExitInstance();
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
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
