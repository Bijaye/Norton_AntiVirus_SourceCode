// NavExtBho.cpp : Implementation of CNavExtBho

#include "stdafx.h"
#include "NavExtBho.h"
#include "Exdisp.h"

/////////////////////////////////////////////////////////////////////////////
// CCNavExtBho


STDMETHODIMP CNavExtBho::SetSite( IUnknown* pUnkSite )
{
	g_Log.LogT("CNavExtBho::SetSite()");

	HRESULT hr;
	CComPtr<IWebBrowser2> spIEWbrowser;
	
	if (FAILED(hr = pUnkSite->QueryInterface(IID_IWebBrowser2,(void**)&spIEWbrowser)))
		return hr;
	
	if(spIEWbrowser)
	{
		DWORD dwVal;
		CRegKey Key;

		// Q: Have we been here before for this user?
		// Verify for x64	@TBD
		if( Key.Create( HKEY_CURRENT_USER, "Software\\Symantec\\Norton AntiVirus" ) != ERROR_SUCCESS )
			return S_OK;

		if( Key.QueryValue( dwVal, _T("ShellExtInit") ) == ERROR_SUCCESS )
			return S_OK;

		// Add the value.
		Key.SetValue( 1, _T("ShellExtInit") );

        return spIEWbrowser->ShowBrowserBar(&CComVariant(_T("{C4069E3A-68F1-403E-B40E-20066696354B}")), &CComVariant(true), &CComVariant());
    }

	return S_OK;
}

// CNavExtBho

