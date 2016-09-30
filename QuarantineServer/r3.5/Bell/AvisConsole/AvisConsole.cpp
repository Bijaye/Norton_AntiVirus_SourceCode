/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


// AvisConsole.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f AvisConsoleps.mk in the project directory.

#include "platform.h"
#include "stdafx.h"
#include "resource.h"     
#include <initguid.h>
#include "AvisConsole.h"

#include "AvisConsole_i.c"
#include "AvisCon.h"
              

extern "C"
{
    extern HMODULE hModuleForDll; 
}


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_AvisCon, CAvisCon)
OBJECT_ENTRY(CLSID_AvisConAbout, CAvisConAbout)
END_OBJECT_MAP()

class CAvisConsoleApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvisConsoleApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAvisConsoleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAvisConsoleApp, CWinApp)
	//{{AFX_MSG_MAP(CAvisConsoleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAvisConsoleApp theApp;

BOOL CAvisConsoleApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_AVISCONSOLELib);
   
    hModuleForDll = _Module.GetModuleInstance();   // jhill 4/19

    return CWinApp::InitInstance();
}

int CAvisConsoleApp::ExitInstance()
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
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    // Remove extension entries: Scope pane
    CRegKey reg;
    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("Software\\Microsoft\\mmc\\nodetypes\\{26140C87-B554-11D2-8F45-3078302C2030}\\Extensions\\PropertySheet"),
        KEY_WRITE ) ) 
    {
        reg.DeleteValue( _T("{6A0B6707-FE02-11D2-9798-00C04F688464}") );
        reg.Close();
    }

    // Remove extension entries: Result pane
    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("Software\\Microsoft\\mmc\\nodetypes\\{1DAE69DE-B621-11d2-8F45-3078302C2030}\\Extensions\\ContextMenu"),
        KEY_WRITE ) ) 
    {
        reg.DeleteValue( _T("{6A0B6707-FE02-11D2-9798-00C04F688464}") );
        reg.Close();
    }

     if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("Software\\Microsoft\\mmc\\nodetypes\\{1DAE69DE-B621-11d2-8F45-3078302C2030}\\Extensions\\PropertySheet"),
        KEY_WRITE ) ) 
    {
        reg.DeleteValue( _T("{6A0B6707-FE02-11D2-9798-00C04F688464}") );
        reg.Close();
    }

     if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("Software\\Microsoft\\mmc\\nodetypes\\{1DAE69DE-B621-11d2-8F45-3078302C2030}\\Extensions\\Toolbar"),
        KEY_WRITE ) ) 
    {
        reg.DeleteValue( _T("{6A0B6707-FE02-11D2-9798-00C04F688464}") );
        reg.Close();
    }

    return _Module.UnregisterServer(TRUE);
}


