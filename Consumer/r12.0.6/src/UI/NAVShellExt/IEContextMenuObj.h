// IEContextMenuObj.h : Declaration of the CIEContextMenu

#ifndef __IECONTEXTMENU_H_
#define __IECONTEXTMENU_H_

#include "Resource.h"       // main symbols
#include "SelectionList.h"

/////////////////////////////////////////////////////////////////////////////
// CIEContextMenu
class ATL_NO_VTABLE CIEContextMenu : public IContextMenu
                                   , public IShellExtInit
                                   , public ISupportErrorInfo
                                   , public CComObjectRootEx<CComSingleThreadModel>
                                   , public CSelectionList<CIEContextMenu, &CLSID_IEContextMenu, &IID_IContextMenu>
{
	// Context menu IDs
	enum COMMAND_IDS {
		ID_SCAN = 0	
	};
#ifdef __Show_Menu_Bitmap__
	// Menu item bitmap.
	HBITMAP	m_hContextMenuBMP;
#endif __Show_Menu_Bitmap__

public:
	CIEContextMenu();
#ifdef __Show_Menu_Bitmap__
	virtual ~CIEContextMenu();
#endif __Show_Menu_Bitmap__

DECLARE_REGISTRY_RESOURCEID(IDR_IECONTEXTMENU)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CIEContextMenu)
	COM_INTERFACE_ENTRY(IContextMenu)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IContextMenu

	STDMETHOD(QueryContextMenu)(HMENU hMenu, UINT indexMenu
	                                       , UINT idCmdFirst
	                                       , UINT idCmdLast
	                                       , UINT uFlags);
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpcmi);
	STDMETHOD(GetCommandString)(UINT idCmd, UINT uType
	                                      , UINT *pwReserved
	                                      , LPSTR pszName
	                                      , UINT  cchMax);

// IShellExtInit
	STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hkeyProgID);

private:
	CString m_csProductName;
};

#endif //__IECONTEXTMENU_H_
