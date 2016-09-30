// NAVDispatch.h

#ifndef __NAVDispatch_h__
#define __NAVDispatch_h__

#include <set>
#include <map>
#include <string>
#include <tchar.h>
#include "NAVOptions.h"

// This is a helper interfaces to implement the mechanics of the named properties
class CProperties
{
    typedef ::std::map<::std::basic_string<WCHAR>, DISPID> DISPIDMAP;
    typedef ::std::set<::std::basic_string<WCHAR> > WSTRINGSET;

	typedef struct is_dispid
	{
		const DISPID did;
		is_dispid(DISPID _did) : did(_did) {}
		bool operator()(DISPIDMAP::value_type x) { return did == x.second; }
	} is_dispid;

	DISPIDMAP m_IDsOfNames;

    // Creating another list of the AP option group names so I can
    // determine which ones are AP specific. This will be a subset
    // of the m_IDsOfNames list
    WSTRINGSET m_APGroupNames;

public:
	CProperties(void);
/*
 *	Implement the methods below in the derived classes to do the
 *	actual work of setting and retrieving the values
 */
	virtual HRESULT Put(LPCWCH pwcName, DISPPARAMS* pdispparams, UINT* puArgErr) = 0;
	virtual HRESULT Get(LPCWCH pwcName, VARIANT* pvValue) = 0;
	virtual HRESULT Default(LPCWCH pwcName, EXCEPINFO* pexcepinfo) = 0;
	virtual HRESULT StdDefault(ITypeInfo* pTInfo, DISPID dispidMember) { return E_FAIL; }
    bool IsAPProperty(LPCWCH pwcPropertyName);

protected:
	static const DISPID_FIRST;
	DISPID    m_iMaxDISPID;

	void Reset(void);
	void AddProperty(LPCWCH pwcPropertyName);
    void AddAPProperty(LPCWCH pwcPropertyName);
	bool PropertyExists(LPCWCH pwcPropertyName);
	HRESULT getIDsOfNames(LPOLESTR* rgszNames, UINT cNames, DISPID* rgdispid);
	HRESULT invoke(DISPID dispidMember, WORD wFlags
	                                  , DISPPARAMS* pdispparams
	                                  , VARIANT* pvarResult
	                                  , EXCEPINFO* pexcepinfo
	                                  , UINT* puArgErr);
	friend struct AddOptions;  // need access to AddProperty()
};


///////////////////////////////////////////////////////
// Derive from this class every COM object that need to implement
// dynamic named properties. The object will have to implement
// a specilized Put() & Get() methods to handle the setting & getting
// of the values for the properties.
template<class T, IID const* piid>
class ATL_NO_VTABLE CNAVDispatchImpl : public IDispatchImpl<T, piid>
                                     , public CProperties
{
	typedef IDispatchImpl<T, piid> ATLIDispatchImpl;

public:
/*
 *	Implement the methods below in the derived classes to do the
 *	actual work of setting and retrieving the values
 */
	virtual HRESULT Put(LPCWCH pwcName, DISPPARAMS* pdispparams, UINT* puArgErr) = 0;
	virtual HRESULT Get(LPCWCH pwcName, VARIANT* pvValue) = 0;

// IDispatch
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames
	                                    , UINT cNames
	                                    , LCID lcid
	                                    , DISPID* rgdispid)
	{
		HRESULT hr;

		// Try first to get the DISPIDs that exist in the TypeLib
		if (DISP_E_UNKNOWNNAME == (hr = ATLIDispatchImpl::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid)))
			hr = getIDsOfNames(rgszNames, cNames, rgdispid);

		return hr;
	}
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid
	                                     , LCID lcid
	                                     , WORD wFlags
	                                     , DISPPARAMS* pdispparams
	                                     , VARIANT* pvarResult
	                                     , EXCEPINFO* pexcepinfo
	                                     , UINT* puArgErr)
	{
		HRESULT hr;

		if ((DISPATCH_PROPERTYGET == (DISPATCH_PROPERTYGET & wFlags) // Get as property
		  || DISPATCH_PROPERTYPUT == wFlags                          // Put as prperty
		  || DISPATCH_METHOD      == wFlags)                         // Get as default value
		 && dispidMember >= DISPID_FIRST - 1
		 && dispidMember <= m_iMaxDISPID)
		{
			if ((DISPID_FIRST - 1) == dispidMember)
				switch(wFlags)
				{
				case DISPATCH_PROPERTYGET:
				case DISPATCH_PROPERTYGET | DISPATCH_METHOD:
						pvarResult->vt = VT_DISPATCH;
						return QueryInterface(IID_IDispatch, (void**)&pvarResult->pdispVal);

				case DISPATCH_PROPERTYPUT:
				case DISPATCH_METHOD:  // Get as default value
					return S_FALSE;
				}

			return invoke(dispidMember, wFlags
	                                  , pdispparams
	                                  , pvarResult
	                                  , pexcepinfo
	                                  , puArgErr);
		}

		//
		// When we want to set a property back to default value the script
		// is calling the prperty as thou it was a method using ()
		// (i.e. NAVOptions.Property();) this will result in a call to Invoke()
		// flaged as DISPATCH_METHOD. This in turn will produce DISP_E_MEMBERNOTFOUND
		// error.
		// We are intecpting those errors to validate that this is not an attempt by the script
		// to reset the default value.
		//
		if (DISP_E_MEMBERNOTFOUND == (hr = ATLIDispatchImpl::Invoke(dispidMember, riid
		                                                                        , lcid
		                                                                        , wFlags
		                                                                        , pdispparams
		                                                                        , pvarResult
		                                                                        , pexcepinfo, puArgErr)))
		{
			HRESULT _hr;
			CComPtr<ITypeInfo> spTInfo;

			// Check if this is a call to one of the properties pretending it's a method
			if (SUCCEEDED(_hr = GetTypeInfo(0, lcid, &spTInfo))
			 && SUCCEEDED(_hr = StdDefault(spTInfo, dispidMember)))
				hr = _hr;
		}

		return hr;
	}
};

#endif  __NAVDispatch_h__
