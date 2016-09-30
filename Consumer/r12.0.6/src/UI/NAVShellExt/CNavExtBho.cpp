// CNavExtBho.cpp : Implementation of CCNavExtBho
#include "stdafx.h"
#include "CNavExtBho.h"
#include "Exdisp.h"

/////////////////////////////////////////////////////////////////////////////
// CCNavExtBho


STDMETHODIMP CCNavExtBho::SetSite( IUnknown* pUnkSite )
{
	HRESULT hr;
	CComPtr<IWebBrowser2> spIEWbrowser;
	
	if (FAILED(hr = pUnkSite->QueryInterface(IID_IWebBrowser2,(void**)&spIEWbrowser)))
		return hr;
	
	if(spIEWbrowser)
	{
		DWORD dwVal;
		CRegKey Key;

		// Q: Have we been here before for this user?
		if( Key.Create( HKEY_CURRENT_USER, "Software\\Symantec\\Norton AntiVirus" ) != ERROR_SUCCESS )
			return S_OK;

		if( Key.QueryValue( dwVal, _T("ShellExtInit") ) == ERROR_SUCCESS )
			return S_OK;

		// Add the value.
		Key.SetValue( 1, _T("ShellExtInit") );

        return spIEWbrowser->ShowBrowserBar(&CComVariant(_T("{42CDD1BF-3FFB-4238-8AD1-7859DF00B1D6}")), &CComVariant(true), &CComVariant());
    }

	return S_OK;
}
