// NSWScanCategory.h : Declaration of the CNSWScanCategory

#ifndef __NSWSCANCATEGORY_H_
#define __NSWSCANCATEGORY_H_
#include "resource.h"       // main symbols
#include "NSWCategory.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWScanCategory

//class CNSWScanCategory : public CNSWCategory

class ATL_NO_VTABLE CNSWScanCategory : 
    public CNSWCategory,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNSWScanCategory, &CLSID_NSWScanCategory>
{
public:
	CNSWScanCategory();

DECLARE_REGISTRY_RESOURCEID(IDR_NSWCATEGORY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNSWScanCategory)
    COM_INTERFACE_ENTRY(INSWCategory)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IVarBstrCol)
END_COM_MAP()

};

#endif //__NSWSCANCATEGORY_H_
