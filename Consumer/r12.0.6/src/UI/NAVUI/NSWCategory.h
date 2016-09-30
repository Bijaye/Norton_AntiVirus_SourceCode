// NSWCategory.h : Declaration of the CNSWCategory

#ifndef __NSWCATEGORY_H_
#define __NSWCATEGORY_H_

#include "resource.h"       // main symbols
#include "ccWebWnd.h"

typedef ccLib::CStdCollection<IVarBstrCol,
VARIANT,
_bstr_t,
VARIANT,
_variant_t> CVarBstrColEx;

/////////////////////////////////////////////////////////////////////////////
// CNSWCategory

class ATL_NO_VTABLE CNSWCategory : 
	public INSWCategory
   ,public ::ATL::IDispatchImpl<CVarBstrColEx, &IID_IVarBstrCol, &LIBID_NAVUILib>
{
    typedef ::ATL::IDispatchImpl<CVarBstrColEx, &IID_IVarBstrCol, &LIBID_NAVUILib> _COLLECTION_BASE;

public:
    CNSWCategory();
	~CNSWCategory();

// INSWCategory
public:
	STDMETHOD(GetInstance)(/*[out, retval]*/ HINSTANCE* phInstance);
    STDMETHOD(GetTitleID)(/*[out, retval]*/ UINT* pnCategoryTitleID);
	STDMETHOD(CreatePage)(/*[in]*/ HWND hParentWnd, /*[in]*/ DWORD dwParam);
	STDMETHOD(ShowPage)(/*[in]*/ int nCommand);
	STDMETHOD(QueryTerminatePage)(/*[out, retval]*/ BOOL* pbCanTerminate);
	STDMETHOD(TerminatePage)();

protected:
    CComPtr<IccWebWindow> m_pWebPanel;
    IUnknownPtr m_spSafeUnk; // use this to keep a reference to WebPageWnd.
    TCHAR m_szPanelURL[MAX_PATH];
    UINT m_uCategoryTitleID;
    HWND m_hParentWnd;
    HWND m_hControlWnd;
};

#endif //__NSWCATEGORY_H_
