// BrowserEvents.cpp : Implemenation of the CBrowserEvents

#include "StdAfx.h"
#include "NAVShellExt.h"
#include "IEToolBandObj.h"

_ATL_FUNC_INFO CBrowserEvents::m_BeforeNavigate2Info = {CC_STDCALL, VT_EMPTY, 7, { VT_DISPATCH
                                                                                 , VT_BSTR
                                                                                 , VT_I4|VT_BYREF
                                                                                 , VT_BSTR|VT_BYREF
                                                                                 , VT_VARIANT|VT_BYREF
                                                                                 , VT_BSTR|VT_BYREF
                                                                                 , VT_BOOL|VT_BYREF} };
_ATL_FUNC_INFO CBrowserEvents::m_NavigateComplete2Info = {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, VT_BSTR } };

CBrowserEvents::CBrowserEvents(CIEToolBand& Outer) : m_Outer(Outer)
                                                   , m_bNavigateState(Navigate_NA)
{
}

void __stdcall CBrowserEvents::onBeforeNavigate2(IDispatch *pIDispatch, VARIANT *pvURL       /* VT_BSTR */
                                                                      , VARIANT *pvFlags     /* VT_I4 */
                                                                      , VARIANT *pvTargetFrameName  /* VT_BSTR */
                                                                      , VARIANT *pvPostData  /* VT_VARIANT */
                                                                      , VARIANT *pvHeaders   /* VT_BSTR */
                                                                      , VARIANT *pvCancel    /* VT_BOOL */)
{
	m_bNavigateState = Navigate_Started;
}

void __stdcall CBrowserEvents::onNavigateComplete2(IDispatch *pIDispatch, VARIANT *pvURL)
{
	m_Outer.canTouchThisSite(pvURL->bstrVal);
	m_bNavigateState = Navigate_Complete;
}

HRESULT CBrowserEvents::Unadvise(void)
{
	HRESULT hr = S_OK;

	if (TEventBeforeNavigate2::m_dwEventCookie != 0xFEFEFEFE)
		hr = TEventBeforeNavigate2::DispEventUnadvise(m_Outer.m_spBrowser);

	if (SUCCEEDED(hr) && TEventNavigateComplete2::m_dwEventCookie != 0xFEFEFEFE)
		hr = TEventNavigateComplete2::DispEventUnadvise(m_Outer.m_spBrowser);

	return hr;
}

HRESULT CBrowserEvents::Advise(void)
{
	// Trap the events
	HRESULT hr = TEventBeforeNavigate2::DispEventAdvise(m_Outer.m_spBrowser);

	if (SUCCEEDED(hr))
		hr = TEventNavigateComplete2::DispEventAdvise(m_Outer.m_spBrowser);

	return hr;
}
