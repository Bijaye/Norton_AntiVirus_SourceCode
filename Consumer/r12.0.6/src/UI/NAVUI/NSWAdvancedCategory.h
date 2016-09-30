
// NSWAdvancedCategory.h : Declaration of the CNSWAdvancedCategory

#ifndef __NSWADVANCEDCATEGORY_H_
#define __NSWADVANCEDCATEGORY_H_

#include "resource.h"       // main symbols
#include "NSWCategory.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWAdvancedCategory

//class CNSWAdvancedCategory : public CNSWCategory
class CNSWAdvancedCategory :
    public CNSWCategory,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNSWAdvancedCategory, &CLSID_NSWAdvancedCategory>
{
public:
	CNSWAdvancedCategory();

DECLARE_REGISTRY_RESOURCEID(IDR_NSWADVANCEDCATEGORY)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNSWAdvancedCategory)
    COM_INTERFACE_ENTRY(INSWCategory)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IVarBstrCol)
END_COM_MAP()

};

#endif //__NSWADVANCEDCATEGORY_H_
