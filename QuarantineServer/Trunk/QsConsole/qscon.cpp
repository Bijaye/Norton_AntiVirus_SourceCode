/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

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
#include "APController.h"
#include "vpstrutils.h"

CComModule _Module;

//
// Static utility object to enable/disable AP for this process.
// Helper for global DisableAP()/EnableAP() functions.
//
static CAPController s_apController;

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
    CString strApplicationName;
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	AfxEnableControlContainer();
	AfxInitRichEdit();
	_Module.Init(ObjectMap, m_hInstance, &LIBID_QSCONLib);

    s_apController.Initialize();

    BOOL bInit = CWinApp::InitInstance();
	if (bInit)
	{
        // Set our application name correctly so that all of our
        // AFX Message Boxes have the correct title, among other things.
        // This is really cheeze, as this is a constant string that needs to be on the heap.
        strApplicationName.LoadString( IDS_APPLICATION_NAME );
        AfxGetApp()->m_pszAppName = new TCHAR[strApplicationName.GetLength()+1];
        // _tcscpy() okay here since we just allocated the required size.
        _tcscpy( (LPTSTR)(AfxGetApp()->m_pszAppName), strApplicationName );
	}
	return bInit;
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
        reg.Close();
        }

    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("SOFTWARE\\Microsoft\\MMC\\NodeTypes"),
        KEY_WRITE ) ) 
        {
        reg.DeleteSubKey( _T("{26140C85-B554-11D2-8F45-3078302C2030}") );
        reg.Close();
        }

    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, 
        _T("SOFTWARE\\Microsoft\\MMC\\NodeTypes\\{0A624A66-269C-11d3-80F4-00C04F68D969}\\Extensions\\NameSpace"),
        KEY_WRITE ) ) 
        {
        reg.DeleteValue( _T("{26140C85-B554-11D2-8F45-3078302C2030}") );
        reg.Close();
        }
    return _Module.UnregisterServer(TRUE);
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
    s_apController.ProtectProcess();
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
    s_apController.UnprotectProcess();
}
