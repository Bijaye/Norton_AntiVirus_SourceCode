// NAVDispatch.cpp: Implementation of CNAVDispatch class

#include "StdAfx.h"
#include <algorithm>
#include "NAVDispatch.h"

/////////////////////////////////////////////////////////////
// CProperties
const CProperties::DISPID_FIRST = 2000;

/////////////////////////////////////////////////////////////
// CProperties::CProperties()
CProperties::CProperties(void) : m_iMaxDISPID(DISPID_FIRST)
{
}

/////////////////////////////////////////////////////////////
// CProperties::Reset()
void CProperties::Reset(void)
{
	m_IDsOfNames.clear();
    m_APGroupNames.clear();
}

/////////////////////////////////////////////////////////////
// CProperties::AddProperty()
void CProperties::AddProperty(LPCWCH pwcPropertyName)
{
	m_IDsOfNames.insert(DISPIDMAP::value_type(pwcPropertyName, m_iMaxDISPID++));
}

/////////////////////////////////////////////////////////////
// CProperties::AddAPProperty()
void CProperties::AddAPProperty(LPCWCH pwcPropertyName)
{
	m_IDsOfNames.insert(DISPIDMAP::value_type(pwcPropertyName, m_iMaxDISPID++));

    // Also save the AP group names separately for internal knowledge
    // so we know which options should come from SAVRT32.dll and not
    // from navopts.dat.  If there are overlapping group names then we'll
    // get into some trouble with this implementation.  Crossing my fingers
    // for now...
    m_APGroupNames.insert(WSTRINGSET::value_type(pwcPropertyName));
}

bool CProperties::PropertyExists(LPCWCH pwcPropertyName)
{
	return m_IDsOfNames.end() != m_IDsOfNames.find(pwcPropertyName);
}

bool CProperties::IsAPProperty(LPCWCH pwcPropertyName)
{
    return m_APGroupNames.end() != m_APGroupNames.find(pwcPropertyName);
}
/////////////////////////////////////////////////////////////
// CProperties::getIDsOfNames()
HRESULT CProperties::getIDsOfNames(LPOLESTR* rgszNames, UINT cNames, DISPID* rgdispid)
{
	bool bAllIDsFound = true;

	// Some of the requested DISPIDs are not in the TypeLib; look for it as a property name
	for (int i = 0; i < cNames; i++)
	{
		// Deal only with unresolved DIPSIDs
		if (DISPID_UNKNOWN == rgdispid[i])
		{
			if (PropertyExists(rgszNames[i]))
				rgdispid[i] = m_IDsOfNames[rgszNames[i]];
			else
				bAllIDsFound = false;
		}
	}

	if (bAllIDsFound)
		return S_OK;

	if (!ShowScriptErrors())
	{
		for (int i = 0; i < cNames; i++)
		{
			rgdispid[i] = DISPID_FIRST - 1;
		}
	}

    char szTempNames [1000] = {0};
    char szTempError [1000] = {0};

    wcstombs ( szTempNames, *rgszNames, wcslen ( *rgszNames )); 
    sprintf ( szTempError, "%s %s", "getIDsOfNames", szTempNames ); 

	::Error(_T("CProperties"), szTempError, DISP_E_UNKNOWNNAME);

	return E_FAIL;
}

HRESULT CProperties::invoke(DISPID dispidMember, WORD wFlags
                                               , DISPPARAMS* pdispparams
                                               , VARIANT* pvarResult
                                               , EXCEPINFO* pexcepinfo
                                               , UINT* puArgErr)
{
	// Get the property name
	DISPIDMAP::iterator nit = find_if(m_IDsOfNames.begin(), m_IDsOfNames.end(), is_dispid(dispidMember));
	if (m_IDsOfNames.end() == nit && (DISPID_FIRST - 1) != dispidMember)  // should never happen
		return ::Error(_T("CProperties"), _T("invoke"), DISP_E_MEMBERNOTFOUND);

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
		return Get(nit->first.c_str(), pvarResult);

	case DISPATCH_PROPERTYPUT:
		return Put(nit->first.c_str(), pdispparams, puArgErr);
	case DISPATCH_METHOD:  // Get as default value
		// Check if the correct numbers of arguments passed
		if (0 != pdispparams->cArgs)
			return ::Error(_T("CProperties"), _T("invoke"), DISP_E_BADPARAMCOUNT);

		return Default(nit->first.c_str(), pexcepinfo);
	}

	return S_OK;
}
