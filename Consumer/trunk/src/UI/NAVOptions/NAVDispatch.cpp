////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVDispatch.cpp: Implementation of CNAVDispatch class

#include "StdAfx.h"
#include <algorithm>
#include "NAVDispatch.h"

/////////////////////////////////////////////////////////////
// CProperties
const CProperties::ANONYMOUS_DISPID = 2000;
const WCHAR CProperties::DelimiterChar = L'_';

/////////////////////////////////////////////////////////////
// CProperties::CProperties()
CProperties::CProperties(void): m_MaxID(ANONYMOUS_DISPID)
{}

/////////////////////////////////////////////////////////////
// CProperties::AddProperty()
// Add a property to the name map
// This is internal; the list is automatically created
// as unknown properties are discovered at runtime.
// DispID is filled in when added/found.
HRESULT CProperties::AddProperty(LPCWSTR pwszProperty, DISPID& dispid)
{
	m_IDsOfNames.insert(DISPIDMAP::value_type(pwszProperty, m_MaxID));
	dispid = m_MaxID++;
	return S_OK;
}

HRESULT CProperties::FindProperty(LPCWSTR pwszProperty, DISPID& dispid)
{
	DISPIDMAP::iterator it = m_IDsOfNames.find(pwszProperty);
	if(it == m_IDsOfNames.end())
		return S_FALSE;
	else
	{
		dispid = it->second;
		return S_OK;
	}
}

HRESULT CProperties::Reset() { m_IDsOfNames.clear(); m_MaxID = ANONYMOUS_DISPID; return S_OK; }

/////////////////////////////////////////////////////////
// This function adds a property based on the external
// naming convention (GROUP:name).
HRESULT CProperties::AddNAVProperty(LPCWSTR pwszProperty)
{
	DISPID did;
	if(!pwszProperty)
		return E_POINTER;

	CStringW cswProp = pwszProperty;
	if(cswProp.IsEmpty())
		return E_INVALIDARG;

	cswProp.Replace(L':', DelimiterChar);
	if(ValidProperty(cswProp) != S_OK)
		return E_INVALIDARG;
	if(AddProperty(cswProp, did) != S_OK)
		return E_FAIL;
	
	return S_OK;
}

/////////////////////////////////////////////////////////////
// CProperties::ValidProperty()
// Validity means all options take the form OPTION_name.
// Invalid: OPTIONname, _name, OPTION_, a______
HRESULT CProperties::ValidProperty(LPCWSTR pwszProperty)
{
    CStringW cswProp, cswRight;
    int sLeft;

    if(!pwszProperty || wcslen(pwszProperty) <= 0)
        return E_INVALIDARG;

    cswProp = pwszProperty;
    sLeft = cswProp.Find(DelimiterChar);
    if(sLeft < 0)
        return S_FALSE; // No underscore or starts with underscore. Invalid.
    
    cswRight = cswProp.Right(cswProp.GetLength() - (sLeft+1));
    cswProp = cswProp.Left(sLeft);

    cswProp.Remove(DelimiterChar);
    cswRight.Remove(DelimiterChar);

    if(cswProp.IsEmpty() || cswRight.IsEmpty())
        return S_FALSE; // Cannot start with, end with or be entirely composed of underscores

    return S_OK;
}

/////////////////////////////////////////////////////////////
// CProperties::getIDsOfNames()
HRESULT CProperties::getIDsOfNames(LPOLESTR* rgszNames, UINT cNames, DISPID* rgdispid)
{
	bool bAllIDsFound = true;
	DISPID did;

	// Some of the requested DISPIDs are not in the TypeLib; look for it as a property name
	for (UINT i = 0; i < cNames; i++)
	{
		// Deal only with unresolved DIPSIDs
		if (DISPID_UNKNOWN == rgdispid[i])
		{
			if(FindProperty(rgszNames[i], did) == S_OK)
				rgdispid[i] = did;
			else
			{
				if(ValidProperty(rgszNames[i]) == S_OK &&
					AddProperty(rgszNames[i], did) == S_OK)
				{
					rgdispid[i] = did;
				}
				else
				{
					bAllIDsFound = false;
				}
			}
		}
	}

	if (bAllIDsFound)
		return S_OK;

	if (!ShowScriptErrors())
	{
		for (UINT i = 0; i < cNames; i++)
		{
			rgdispid[i] = ANONYMOUS_DISPID - 1;
		}
	}

    WCHAR szTempError [1000] = {0};

    wsprintf ( szTempError, L"%s %s", L"getIDsOfNames", *rgszNames ); 

	::Error(_T("CProperties"), szTempError, DISP_E_UNKNOWNNAME);

	return E_FAIL;
}

HRESULT CProperties::invoke(DISPID dispidMember, WORD wFlags
                                               , DISPPARAMS* pdispparams
                                               , VARIANT* pvarResult
                                               , EXCEPINFO* pexcepinfo
                                               , UINT* puArgErr)
{
    CStringW cswOption;

	// Get the property name
	DISPIDMAP::iterator nit = find_if(m_IDsOfNames.begin(), m_IDsOfNames.end(), is_dispid(dispidMember));
	if (m_IDsOfNames.end() == nit && (ANONYMOUS_DISPID - 1) != dispidMember)  // should never happen
    {
		return ::Error(_T("CProperties"), _T("invoke"), DISP_E_MEMBERNOTFOUND);
    }

    // Replace underscores with : in order to comply with The Old Way.
    cswOption = nit->first.c_str();
    cswOption.Replace(DelimiterChar, L':');

	switch(wFlags)
	{
	case DISPATCH_PROPERTYGET:
	case DISPATCH_PROPERTYGET | DISPATCH_METHOD:
		// Validate input
		if (!pvarResult)
			return ::Error(_T("CProperties"), _T("invoke"), E_POINTER);

		// Check if the correct numbers of arguments passed
		if (0 != pdispparams->cArgs)
			return ::Error(_T("CProperties"), _T("invoke"), DISP_E_BADPARAMCOUNT);

		// Finally do the work of getting the value
		return Get(cswOption, pvarResult);

	case DISPATCH_PROPERTYPUT:
		return Put(cswOption, pdispparams, puArgErr);
	case DISPATCH_METHOD:  // Get as default value
		// Check if the correct numbers of arguments passed
		if (0 != pdispparams->cArgs)
			return ::Error(_T("CProperties"), _T("invoke"), DISP_E_BADPARAMCOUNT);

		return Default(cswOption, pexcepinfo);
	}

	return S_OK;
}
