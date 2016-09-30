// NSWPluginView.h : Declaration of the CNSWPluginView

#ifndef __NSWPLUGINVIEW_H_
#define __NSWPLUGINVIEW_H_

#include "resource.h"       // main symbols
#include "brandingimpl.h"
#include "HelpMenu.h"

#ifdef NSW_CAT_LIST

// Categories for NAV Standard Edition

CLSID const* _pclsCategories[] =
{
	&CLSID_NSWStatusCategory,
		&CLSID_NSWScanCategory,
		&CLSID_NSWReportsCategory,
		//	&CLSID_NSWSchedulingCategory
};

// Categories for NAV Professional Edition

CLSID const* _pclsCategoriesPro[] = 
{
	&CLSID_NSWStatusCategory,
		&CLSID_NSWScanCategory,
		&CLSID_NSWReportsCategory,
		//	&CLSID_NSWSchedulingCategory,
		&CLSID_NSWAdvancedCategory
};

#endif

typedef ccLib::CStdCollection<IVarBstrCol,
VARIANT,
_bstr_t,
VARIANT,
_variant_t> CVarBstrColEx;


/////////////////////////////////////////////////////////////////////////////
// CNSWPluginView
class ATL_NO_VTABLE CNSWPluginView : 
	public CComObjectRootEx<CComSingleThreadModel>
	,public CComCoClass<CNSWPluginView, &CLSID_NSWPluginView>
	,public INSWIntegratorProduct
	,public CBrandingImpl
	,public INSWHelpMenuEx
	,public ::ATL::IDispatchImpl<CVarBstrColEx, &IID_IVarBstrCol, &LIBID_NAVUILib>
{
	typedef ::ATL::IDispatchImpl<CVarBstrColEx, &IID_IVarBstrCol, &LIBID_NAVUILib> _COLLECTION_BASE;
public:
	CNSWPluginView();
	virtual ~CNSWPluginView();

	DECLARE_REGISTRY_RESOURCEID(IDR_NSWPLUGINVIEW)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CNSWPluginView)
		COM_INTERFACE_ENTRY(INSWIntegratorProduct)
		COM_INTERFACE_ENTRY(INSWHelpMenuEx)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IVarBstrCol)
	END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	// INSWHelpMenuEx
public:
	STDMETHOD (ResetMenuItemEnum)();
	STDMETHOD (GetNextMenuItem)( WORD *pdwItemID,
		SYMSW_MENUITEMTYPE *pdwItemType,
		UINT *pnItemText);
	STDMETHOD (DoMenuItem)( WORD dwItemID,
		HWND hMainWnd); 

	// INSWPluginView
public:
	STDMETHOD(GetInstance)(/*[out, retval]*/ HINSTANCE* phInstance);
	STDMETHOD(GetProductNameID)(/*[out, retval]*/ UINT* pnProductNameID);
	STDMETHOD(GetProductBitmapID)(/*[in]*/ SYMSW_BITMAPNO eBitmapNo, /*[out, retval]*/ UINT* pnProductBitmapID);
	STDMETHOD(GetProductOrder)(/*[out, retval]*/ UINT* pnOrderLoc);
	STDMETHOD(HasOptions)(/*[out, retval]*/ BOOL* pbHasOptions);
	STDMETHOD(DoOptions)(/*[in]*/ HWND hParentWnd);
	STDMETHOD(HasHelp)(/*[out, retval]*/ BOOL* pbHasHelp);
	STDMETHOD(DoHelp)(/*[in]*/ int nIndex, /*[in]*/ HWND hParentWnd);
	STDMETHOD(ResetCategoryEnum)();
	STDMETHOD(GetNextCategory)(/*[in]*/ BOOL bCreate, /*[out, retval]*/ INSWCategory** ppCategory);
	STDMETHOD(SetValue)(/*[in]*/ DWORD dwValueID, /*[in]*/ DWORD dwValue);
	STDMETHOD(GetValue)(/*[in]*/ DWORD dwValueID, /*[out, retval]*/ DWORD* pdwValue);


	// INSWPluginView
	STDMETHOD(put_Item)( 
		/* [in] */ VARIANT vKey,
		/* [in] */ VARIANT newVal);


private:
	int m_nEnumLoc;
	BOOL m_bProEdition;
	BOOL m_bStandAloneMode;
	SIMON::CSimonPtr<INAVOptionRefresh> m_spOptionRefresh;

protected:
	StahlSoft::CSmartHandle m_shMutex;
	CHelpMenu m_HelpMenu;
    bool getPre2004Suite();

};

#endif //__NSWPLUGINVIEW_H_
