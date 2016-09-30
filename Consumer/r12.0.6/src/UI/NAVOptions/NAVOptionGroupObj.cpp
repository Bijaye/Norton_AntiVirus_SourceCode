// NAVOptionGroupObj.cpp : Implementation of CNAVOptionGroup
#include "stdafx.h"
#include "NAVOptions.h"
#include "NAVOptionsObj.h"
#include "NAVOptionGroupObj.h"
#include "..\navoptionsres\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVOptionGroup
/////////////////////////////////////////////////////////////////////////////
// CNAVOptionGroup::CNAVOptionGroup
CNAVOptionGroup::CNAVOptionGroup() : m_pINAVOptions(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptionGroup::CNAVDispatchImpl
HRESULT CNAVOptionGroup::Put(LPCWCH pwcName, DISPPARAMS* pdispparams, UINT* puArgErr)
{
	HRESULT hr;
	long eTypeOf;
	CComVariant vArg;

	// Validate that the object is used correctly
	if (!m_pINAVOptions)
		return Error(IDS_Err_InvalidContext, _T("Put()"));

	// Validate input
	if (!puArgErr)
		return Error(_T("Put()"), E_POINTER);

	// Check if the correct numbers of arguments passed
	if (1 != pdispparams->cArgs)
		return DISP_E_BADPARAMCOUNT;

	// Compose the property name
	CComBSTR sbName(m_sbGroupName);
	sbName += L":";
	sbName += pwcName;

	CNAVOptions* pNAVOptions = NULL;

	// QI for IID_NULL will provid a this pointer to the C++ of the object
	if (FAILED(hr = m_pINAVOptions->QueryInterface(IID_NULL, (void**)&pNAVOptions)))
		return hr;

	// Parse the value from the input params & coerce it if needed
	if (FAILED(hr = pNAVOptions->get__TypeOf(sbName, &eTypeOf))
	 || FAILED(hr = ::DispGetParam(pdispparams, DISPID_PROPERTYPUT                                 // position
                                              , ccSettings::SYM_SETTING_DWORD == eTypeOf ? VT_UI4 : VT_BSTR // coersion
	                                          , &vArg                                              // coerced value
	                                          , puArgErr)))                                        // index of erroneous argument
		return hr;

	return m_pINAVOptions->Put(sbName, vArg);
}

HRESULT CNAVOptionGroup::Get(LPCWCH pwcName, VARIANT* pvValue)
{
	::VariantInit(pvValue);

	// Validate that the object is used correctly
	if (!m_pINAVOptions)
		return Error(IDS_Err_InvalidContext, _T("Get()"));

	// Compose the property name
	CComBSTR sbName(m_sbGroupName);
	sbName += L":";
	sbName += pwcName;

	return m_pINAVOptions->Get(sbName, CComVariant(/*dummy*/), pvValue);
}

HRESULT CNAVOptionGroup::Default(LPCWCH pwcName, EXCEPINFO* pexcepinfo)
{
	// Validate that the object is used correctly
	if (!m_pINAVOptions)
		return Error(IDS_Err_InvalidContext, _T("Default()"));

	// Compose the property name
	CComBSTR sbName(m_sbGroupName);
	sbName += L":";
	sbName += pwcName;

	CNAVOptions* pNAVOptions = static_cast<CComObject<CNAVOptions>*>(m_pINAVOptions);

	return pNAVOptions->Default(sbName, pexcepinfo);
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptionGroup::Properties
/////////////////////////////////////////////////////////////////////////////
// GroupName
HRESULT CNAVOptionGroup::get__GroupName(/*[out, retval]*/ BSTR *pbstrGroupName)
{
	if (!pbstrGroupName)
		return Error(_T("get__GroupName()"), E_POINTER);

	*pbstrGroupName = m_sbGroupName.Copy();

	return S_OK;
}

HRESULT CNAVOptionGroup::put__GroupName(/*[in]*/ BSTR bstrGroupName)
{
	if (!bstrGroupName || !*bstrGroupName)
		return Error(_T("put__GroupName()"), E_INVALIDARG);

	m_sbGroupName = bstrGroupName;

	return S_OK;
}

// INAVOptions
HRESULT CNAVOptionGroup::get__INAVOptions(/*[out, retval]*/ INAVOptions** ppINAVOptions)
{
	if (!ppINAVOptions)
		return Error(_T("get__INAVOptions()"), E_POINTER);

	*ppINAVOptions = m_pINAVOptions;

	return S_OK;
}

HRESULT CNAVOptionGroup::put__INAVOptions(/*[in]*/ INAVOptions* pINAVOptions)
{
	if (!pINAVOptions)
		return Error(_T("put__INAVOptions()"), E_INVALIDARG);

	m_pINAVOptions = pINAVOptions;

	return S_OK;
}

HRESULT CNAVOptionGroup::Error(UINT uiIDS, PTCHAR pszLocation)
{
	if (ShowScriptErrors())
		return CComCoClass<CNAVOptionGroup, &CLSID_NAVOptionGroup>::Error(uiIDS, IID_INAVOptionGroup);
	else
		return ::Error(_T("CNAVOptionGroup"), uiIDS, pszLocation);
}

HRESULT CNAVOptionGroup::Error(PTCHAR pszLocation, HRESULT hr)
{
	return ::Error(_T("CNAVOptionGroup"), pszLocation, hr);
}
