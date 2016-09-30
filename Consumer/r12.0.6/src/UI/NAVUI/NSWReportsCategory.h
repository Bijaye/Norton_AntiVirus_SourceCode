// NSWReportsCategory.h : Declaration of the CNSWReportsCategory

#ifndef __NSWREPORTSCATEGORY_H_
#define __NSWREPORTSCATEGORY_H_
#include "resource.h"       // main symbols
#include "NSWCategory.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWReportsCategory

//class CNSWReportsCategory : public CNSWCategory

class ATL_NO_VTABLE CNSWReportsCategory : 
    public CNSWCategory,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNSWReportsCategory, &CLSID_NSWReportsCategory>
{
public:
	CNSWReportsCategory();
    
DECLARE_REGISTRY_RESOURCEID(IDR_NSWCATEGORY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNSWReportsCategory)
    COM_INTERFACE_ENTRY(INSWCategory)
    COM_INTERFACE_ENTRY(IVarBstrCol)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

};

#endif //__NSWREPORTSCATEGORY_H_
