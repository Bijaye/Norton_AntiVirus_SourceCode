// NSWStatusCategory.h : Declaration of the CNSWStatusCategory

#ifndef __NSWSTATUSCATEGORY_H_
#define __NSWSTATUSCATEGORY_H_

#include "resource.h"       // main symbols
#include "NSWCategory.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWStatusCategory

class ATL_NO_VTABLE CNSWStatusCategory : 
    public CNSWCategory,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNSWStatusCategory, &CLSID_NSWStatusCategory>
{
public:
    CNSWStatusCategory();

DECLARE_REGISTRY_RESOURCEID(IDR_NSWCATEGORY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNSWStatusCategory)
    COM_INTERFACE_ENTRY(INSWCategory)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IVarBstrCol)
END_COM_MAP()


    STDMETHOD(CreatePage)(/*[in]*/ HWND hParentWnd, /*[in]*/ DWORD dwParam);
};

#endif //__NSWSTATUSCATEGORY_H_
