// SavMainUI.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "SavMainUI.h"
#include "dlldatax.h"


class CSavMainUIModule : public CAtlDllModuleT< CSavMainUIModule >
{
public :
	DECLARE_LIBID(LIBID_SavMainUILib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SAVMAINUI, "{5C2FCE47-AC1E-43D5-8E5C-038570953896}")
};

CSavMainUIModule _AtlModule;

class CSavMainUIApp : public CWinApp
{
public:

// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CSavMainUIApp, CWinApp)
END_MESSAGE_MAP()

CSavMainUIApp theApp;

BOOL CSavMainUIApp::InitInstance()
{
    BOOL returnValBOOL = FALSE;
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
    returnValBOOL = CWinApp::InitInstance();

    // I added these 2 calls myself, seemed like they should be there
    AfxEnableControlContainer();
    InitCommonControls();

    return returnValBOOL;
}

int CSavMainUIApp::ExitInstance()
{
    return CWinApp::ExitInstance();
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (hr != S_OK)
        return hr;
#endif
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}

