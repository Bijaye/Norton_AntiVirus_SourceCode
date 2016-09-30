// NavLUCallback.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NavLUCallbackps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "DefAnnuity.h"
#include "navlucbk.h"

#include "NavLUCallback_i.c"
#define BUILDING_NAVLUSTRINGS
#include "strings.h"
#include "luNavCallBack.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_luNavCallBack, CluNavCallBack)
END_OBJECT_MAP()

class CNavLUCallbackApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavLUCallbackApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNavLUCallbackApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CNavLUCallbackApp, CWinApp)
	//{{AFX_MSG_MAP(CNavLUCallbackApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CNavLUCallbackApp theApp;

BOOL CNavLUCallbackApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_NAVLUCALLBACKLib);
    return CWinApp::InitInstance();
}

int CNavLUCallbackApp::ExitInstance()
{
    _Module.Term();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
	HRESULT hr;

	hr = _Module.RegisterServer(TRUE);

	if ( FAILED( CoInitialize( NULL ) ) )
	{
		ATLTRACE( _T("RegisterCmdLines() - Unable to initialize COM") );
		return E_FAIL;
	}

	hr = CNavLu::RegisterCommandLines( DefAnnuitySuccess );

	CoUninitialize();

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
try
    {
	if ( FAILED( CoInitialize( NULL ) ) )
	{
		ATLTRACE( _T("RegisterCmdLines() - Unable to initialize COM") );
		return E_FAIL;
	}

	CNavLu::UnregisterCommandLines();

	CoUninitialize();

    return _Module.UnregisterServer(TRUE);
    }
catch (...)
    {
    // Fail silently since reporting an error wouldn't do much good
    // anyway.
    return S_OK;
    }
}


