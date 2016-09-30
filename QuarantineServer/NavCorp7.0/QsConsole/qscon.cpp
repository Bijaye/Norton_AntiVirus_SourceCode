// qscon.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f qsconps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "qscon.h"

#include "qscon_i.c"
#include "QSConsole.h"
#include "qscommon.h"
#include "platform.h"
#include "apcomm.h"

CComModule _Module;


typedef BOOL (WINAPI *NAVAPFunctionPointer) ();

// 
// Pointers to AP functions.
// 
NAVAPFunctionPointer g_lpfnDisableAP = NULL;
NAVAPFunctionPointer g_lpfnEnableAP = NULL;




BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_QSConsole, CQSConsole)
OBJECT_ENTRY(CLSID_QSConsoleAbout, CQSConsoleAbout)
END_OBJECT_MAP()

class CQsconApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQsconApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CQsconApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void GetAPFuntions();
};

BEGIN_MESSAGE_MAP(CQsconApp, CWinApp)
	//{{AFX_MSG_MAP(CQsconApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CQsconApp theApp;

BOOL CQsconApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_QSCONLib);

    GetAPFuntions();

    return CWinApp::InitInstance();
}

int CQsconApp::ExitInstance()
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
    // 
    // Remove computer manager extension keys.
    // 
    CRegKey reg;
    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("Software\\Microsoft\\mmc\\nodetypes\\{476E6449-AAFF-11D0-B944-00C04FD8D5B0}\\Dynamic Extensions"),
        KEY_WRITE ) ) 
        {
        reg.DeleteValue( _T("{26140C85-B554-11D2-8F45-3078302C2030}") );
        reg.Close();
        }

    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("SOFTWARE\\Microsoft\\MMC\\NodeTypes\\{476E6449-AAFF-11D0-B944-00C04FD8D5B0}\\Extensions\\NameSpace"),
        KEY_WRITE ) ) 
        {
        reg.DeleteValue( _T("{26140C85-B554-11D2-8F45-3078302C2030}") );
        }

    return _Module.UnregisterServer(TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsole::GetAPFuntions
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/31/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQsconApp::GetAPFuntions()
{
    DWORD dwSize = MAX_PATH * sizeof( TCHAR );
    CString sLibPath;

    // 
    // Open installed apps regkey.
    //     
    CRegKey reg;
    if( ERROR_SUCCESS != reg.Open( HKEY_LOCAL_MACHINE, REGKEY_INSTALLED_APPS, KEY_READ ) )
        return;

    LONG lResult = reg.QueryValue( sLibPath.GetBuffer( MAX_PATH ), REGKEY_INSTALLED_NAV, &dwSize );
    sLibPath.ReleaseBuffer();
    if( ERROR_SUCCESS != lResult )                                
        return;

    // 
    // Build full path to lib.
    // 
    sLibPath += _T("\\");
    sLibPath += NAVAPI_NAME;

    // 
    // Load module.
    // 
    HMODULE hMod = LoadLibrary( sLibPath );
    if( hMod == NULL )
        return;

    // 
    // Get funtion pointers.
    // 
    g_lpfnDisableAP = (NAVAPFunctionPointer) GetProcAddress( hMod, szNAVAPUnprotectProcess );
    g_lpfnEnableAP = (NAVAPFunctionPointer) GetProcAddress( hMod, szNAVAPProtectProcess );

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: EnableAP
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/31/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void EnableAP()
{
    if( g_lpfnDisableAP && g_lpfnEnableAP )
        {
        g_lpfnEnableAP();
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: DisableAP
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/31/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void DisableAP()
{
    if( g_lpfnDisableAP && g_lpfnEnableAP )
        {
        g_lpfnDisableAP();
        }
}





