////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptions.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NAVOptionsps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "NAVOptions.h"
#include "uiPEPConstants.h"

#include "NAVOptions_i.c"
#include "NAVOptionsObj.h"
#include "ThreatsByVID.h"

#include "SNDHelper.h"
#include "SymHTMLDialog.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVOpts"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

DEFINE_GUID(GUID_MSOfficeAntiVirus, 0xde1f7eef, 0x1851, 0x11d3, 0x93, 0x9e, 0x0, 0x04, 0xac, 0x1a, 0xbe, 0x1f);
// CATID for Microsoft Office AntiVirus COM category
// {56FFCC31-D398-11d0-B2AE-00A0C908FA49}
DEFINE_GUID(CATID_MSOfficeAntiVirus, 0x56ffcc30, 0xd398, 0x11d0, 0xb2, 0xae, 0x0, 0xa0, 0xc9, 0x8, 0xfa, 0x49);

CComModule _Module;
COSInfo  g_OSInfo;
CNAVInfo g_NAVInfo;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_NAVOptions,   CNAVOptions)
    OBJECT_ENTRY(CLSID_ThreatsByVID, CThreatsByVID)
END_OBJECT_MAP()

//Resource loading
#include "..\navoptionsres\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navopts.loc")
CAtlBaseModule _ModuleRes;
HMODULE g_ResModule;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

ATL::CDynamicStdCallThunk::CThunksHeap ATL::CDynamicStdCallThunk::s_ThunkHeap;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVOPTIONSLib);
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

// Don't use this in DllMain!!
void _InitResources()
{
	if( NULL == g_ResLoader.GetResourceInstance() ) 
	{
		if( g_ResLoader.Initialize() ) 
		{
            g_ResModule = g_ResLoader.GetResourceInstance();
            _Module.SetResourceInstance( g_ResLoader.GetResourceInstance());
		}
	}
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

    _InitResources();
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    _InitResources();
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}

bool ShowScriptErrors(void)
{
	static enum { showScriptErrors_NA
	            , showScriptErrors_No
	            , showScriptErrors_Yes } s_eShowScriptErrors(showScriptErrors_NA);

	if (showScriptErrors_NA == s_eShowScriptErrors)
	{
		DWORD dwScriptErrors;
        DWORD dwSize = sizeof(dwScriptErrors);
		CRegKey hShowErrors;
		if (ERROR_SUCCESS == hShowErrors.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"))
		 && ERROR_SUCCESS == hShowErrors.QueryValue(_T("ShowScriptErrors"), NULL, &dwScriptErrors, &dwSize))
			s_eShowScriptErrors = dwScriptErrors ? showScriptErrors_Yes : showScriptErrors_No;
	}

	return showScriptErrors_Yes == s_eShowScriptErrors;
}

HRESULT Error(PCTSTR pszClass, UINT uiIDS, PCTSTR pszLocation)
{
	TCHAR szError[1024];

	::LoadString(g_ResModule, uiIDS, szError, SIZEOF(szError));
	CCTRACEE(_T("%s::%s: %s"), pszClass, pszLocation, szError);

	return S_FALSE;
}

HRESULT Error(PCTSTR pszClass, PCTSTR pszLocation, HRESULT hr)
{
	USES_CONVERSION;
	HRESULT _hr;
	CComBSTR sbSource, sbDesc;
	CComPtr<IErrorInfo> spErrorInfo;

	if (ShowScriptErrors())
		return hr ;

	// Check if ErrorInfo is available
	if (S_OK == (_hr = ::GetErrorInfo(0UL, &spErrorInfo))
	 && SUCCEEDED(_hr = spErrorInfo->GetSource(&sbSource))
	 && SUCCEEDED(_hr = spErrorInfo->GetDescription(&sbDesc)))
		CCTRACEE(_T("%s::%s: %s, %s"), pszClass, pszLocation, OLE2T(sbSource), OLE2T(sbDesc));
	else  // Format our own (cheesey) message
		CCTRACEE(_T("%s::%s: COM error in OptionsGroup (0x%08lx)"), pszClass, pszLocation, hr);

	return S_FALSE;
}
