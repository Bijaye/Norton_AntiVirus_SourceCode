////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NavLUCallback.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NavLUCallbackps.mk in the project directory.

#include "stdafx.h"
#if !defined(_NO_SIMON_INITGUID)
    #define SIMON_INITGUID
#endif

#define INITIIDS
#include "SymInterface.h"
#include "ccSettingsManagerHelper.h"

#include <SIMON.h>
#include <initguid.h>

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#include "V2LicensingAuthGuids.h"
#include "cltProdInfoPEPConstants.h"
#include "cltProductHelper.h"
#include "uiPEPConstants.h"

#include "resource.h"
//#include "DefAnnuity.h"
#include "navlucbk.h"


#include "NavLUCallback_i.c"
#define BUILDING_NAVLUSTRINGS
#include "strings.h"
#include "luNavCallBack.h"

#include "NAVOptRefresh.h"

CComModule _Module;
SIMON::CSimonModule _SimonModule;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_luNavCallBack, CluNavCallBack)
END_OBJECT_MAP()

//============================================================================================
//
// SymInterface map for ccVerifyTrust static lib.
//
//============================================================================================
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

//Resource loading code
#include "..\navlucbkres\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navlucbk.loc")
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

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
CString _g_csHandlerPath;

bool	_InitResources()
{
	bool	bRet = true;
	if( NULL == g_ResLoader.GetResourceInstance() ) 
	{
		if( g_ResLoader.Initialize() ) 
		{
			_Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
		}
		else
		{
			CCTRACEE(_T("Failed to Initialize resources."));
			bRet = false;
		}
	}
	return bRet;
}

BOOL CNavLUCallbackApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_NAVLUCALLBACKLib);

    // Fetch the handler dll path
    ///////////////////////////////////////////////////////////////////////
    CString sModuleLocation;
    TCHAR _szModuleLocation[_MAX_PATH*2] = {0};
    long nRet =::GetModuleFileName(m_hInstance,_szModuleLocation,sizeof(_szModuleLocation));
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
	//  No trust check. (LU callback interface seems safe enough.)
	INIT_RESOURCES()
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
		CCTRACEE( _T("DllRegisterServer() - Unable to initialize COM") );
		return E_FAIL;
	}

    CNavLu navLu;
    if( !navLu.Initialize() )
    {
        CCTRACEE( _T("DllRegisterServer() - Unable to initialize CNavLu") );
		return E_FAIL;
    }

    try
    {
    	hr = navLu.RegisterCommandLines();
    }
    catch(_com_error& e)
    {
        hr = e.Error();
    }
    catch(exception &ex)
    {
        CCTRCTXE1(_T("Unexpected exception: %s"), ex.what());
        hr = E_UNEXPECTED;
    }

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
            CCTRACEE( _T("DllUnregisterServer() - Unable to initialize COM") );
            return E_FAIL;
        }

        CNavLu navLu;
        if( !navLu.Initialize() )
        {
            CCTRACEE( _T("DllUnregisterServer() - Unable to initialize CNavLu") );
		    return E_FAIL;
        }
        
        try
        {
            navLu.UnregisterCommandLines();

        }
        catch(_com_error &e)
        { CCTRCTXW1(_T("COM exception: %lu"), e.Error()); }
        
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


