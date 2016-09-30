// NSWFrame.h : Declaration of the CNSWFrame

#ifndef __NSWFRAME_H_
#define __NSWFRAME_H_

#include "resource.h"       // main symbols
//#include "swmisc.h"
//#include <navoptx.h>

#include "NAVLnch.h"
//#include "AppLauncher.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "brandingimpl.h"

#ifdef NSW_PROD_LIST

const CLSID CLSID_SAPProduct =
{0xC41C2D13,0xFAD8,0x418E,{0xB5,0x7E,0xA0,0x36,0xDE,0x54,0xC0,0x5D}};

const CLSID CLSID_SymLTProduct =
{0xF768BE08,0x81D7,0x400D,{0x8B,0xD9,0x8B,0x8F,0x8B,0xB0,0xE9,0x6D}};

const CLSID CLSID_NSCProduct =
{0x93156B26,0x5354,0x474E,{0xBD,0x56,0xC2,0xCE,0xB1,0x90,0xC4,0xF5}};
/*
CLSID const* _pclsProducts[] = 
{
&CLSID_NSWPluginView
};
*/
CLSID const* _pclsProducts[] = 
{
    &CLSID_NSWPluginView,
    //&CLSID_SAPProduct,
	&CLSID_NSCProduct,
	&CLSID_SymLTProduct					// SymLTProduct plugin is an optional plugin; therefore,
										// it must be the last one in the product list.
										// Otherwise, NMain will skip remaining plugin's if SymLTProduct is skipped!!!
};

#endif

/////////////////////////////////////////////////////////////////////////////
// CNSWFrame
class ATL_NO_VTABLE CNSWFrame : 
     public CComObjectRootEx<CComSingleThreadModel>
    ,public CComCoClass<CNSWFrame, &CLSID_NSWFrame>
    ,public INSWFrame
    ,public INSWOEMSupport
    ,public INSWProductReg
    ,public CBrandingImpl
{
public:
	CNSWFrame();
	
	DECLARE_REGISTRY_RESOURCEID(IDR_NSWFRAME)
		
		DECLARE_PROTECT_FINAL_CONSTRUCT()
		
		BEGIN_COM_MAP(CNSWFrame)
		COM_INTERFACE_ENTRY(INSWFrame)
		COM_INTERFACE_ENTRY(INSWOEMSupport)
		COM_INTERFACE_ENTRY(INSWProductReg)
		END_COM_MAP()
		
		HRESULT FinalConstruct();
	// INSWFrame
public:
	STDMETHOD(GetInstance)(/*[out, retval]*/ HINSTANCE* phInstance);
	STDMETHOD(GetTitleNameID)(/*[out, retval]*/ UINT* pnTitleNameID);
	STDMETHOD(GetIconID)(/*[out, retval]*/ UINT* pnIconID);
	STDMETHOD(GetProductBitmapID)(/*[in]*/ SYMSW_BITMAPNO eBitmapNo, /*[out, retval]*/ UINT* pnBitmapID);
	STDMETHOD(DoHelp)(/*[in]*/ HWND hMainWnd);
	STDMETHOD(ResetProductEnum)();
	STDMETHOD(GetNextProduct)(/*[in]*/ BOOL bCreate, /*[out, retval]*/ INSWIntegratorProduct** ppProduct);
	STDMETHOD(ResetTBButtonEnum)();
	STDMETHOD(GetNextTBButton)(/*[out]*/ DWORD* pdwButtonID, /*[out]*/ UINT* pnButtonText, /*[out]*/ UINT* pnBitmapUp);
	STDMETHOD(TBButtonClick)(/*[in]*/ DWORD dwButtonID, /*[in]*/ HWND hMainWnd);
	STDMETHOD(DoAbout)(/*[in]*/ HWND hMainWnd);
	STDMETHOD(SetValue)(/*[in]*/ DWORD dwValueID, /*[in]*/ DWORD dwValue);
	STDMETHOD(GetValue)(/*[in]*/ DWORD dwValueID, /*[out, retval]*/ DWORD* pdwValue);
    STDMETHOD(GetTechSuppURL)(/*[out, retval]*/ LPTSTR szURL);
    STDMETHOD(ShowTechSuppMenu)(/*[out, retval]*/ BOOL* pbShowMenu);
    STDMETHOD(ShowSymantecMenu)(/*[out, retval]*/ BOOL* pbShowMenu);
	STDMETHOD(ShowProductReg)(/*[out, retval]*/ BOOL* pbShowMenu);
	STDMETHOD(DoProductReg)(/*[in]*/ HWND hParentWnd);
private:
	int m_nEnumLoc;
	int m_nEnumMax;
	int m_nTBEnumLoc;
	GUID* m_pgGUIDs;
	
protected:
    CComPtr <IAppLauncher> m_spAppLauncher;
    CComPtr <ISymScriptSafe> m_spSymScriptSafe;
};

#endif //__NSWFRAME_H_
