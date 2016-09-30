// NAVOptionGroupObj.h : Declaration of the CNAVOptionGroup

#ifndef __NAVOPTIONGROUP_H_
#define __NAVOPTIONGROUP_H_

#include "Resource.h"       // main symbols
#include "NAVDispatch.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVOptionGroup
class ATL_NO_VTABLE CNAVOptionGroup : public ISupportErrorInfoImpl<&IID_INAVOptionGroup>
                                    , public CNAVDispatchImpl<INAVOptionGroup, &IID_INAVOptionGroup>
                                    , public CComObjectRootEx<CComSingleThreadModel>
                                    , public CComCoClass<CNAVOptionGroup, &CLSID_NAVOptionGroup>
{
	// The name of this group of properties
	CComBSTR m_sbGroupName;
	// Pointer to the parent
	INAVOptions* m_pINAVOptions;

public:
	CNAVOptionGroup();

DECLARE_REGISTRY_RESOURCEID(IDR_NAVOPTIONGROUP)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVOptionGroup)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(INAVOptionGroup)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// CNAVDispatchImpl
	virtual HRESULT Put(LPCWCH pwcName, DISPPARAMS* pdispparams, UINT* puArgErr);
	virtual HRESULT Get(LPCWCH pwcName, VARIANT* pvValue);
	virtual HRESULT Default(LPCWCH pwcName, EXCEPINFO* pexcepinfo);
	HRESULT Error(UINT uiIDS, PTCHAR pszLocation);
	HRESULT Error(PTCHAR pszLocation, HRESULT hr);

// INAVOptionGroup
public:
	// Properties
	HRESULT get__GroupName(/*[out, retval]*/ BSTR *pbstrGroupName);
	HRESULT put__GroupName(/*[in]*/ BSTR bstrGroupName);
	HRESULT get__INAVOptions(/*[out, retval]*/ INAVOptions** ppINAVOptions);
	HRESULT put__INAVOptions(/*[in]*/ INAVOptions* pINAVOptions);
};

#endif //__NAVOPTIONGROUP_H_
