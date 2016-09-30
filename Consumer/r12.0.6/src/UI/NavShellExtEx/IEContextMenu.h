// IEContextMenu.h : Declaration of the CIEContextMenu

#pragma once
#include "resource.h"       // main symbols

#include "NavShellExtEx.h"
#include "SelectionList.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error Single-threaded COM objects are not properly supported on Windows CE platforms that do not include full DCOM support.  Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support them anyway. You must also change the threading model in your rgs file from 'Apartment'/'Single' to 'Free'.
#endif



// CIEContextMenu

class ATL_NO_VTABLE CIEContextMenu : public IContextMenuEx
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
	COM_INTERFACE_ENTRY(IContextMenuEx)
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
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFOEx lpcmi);
    STDMETHOD(GetCommandString)(UINT_PTR,UINT,UINT *,LPSTR,UINT);

// IShellExtInit
	STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hkeyProgID);

	HRESULT	_Error(int iId, const IID& guid);
	HRESULT	_Error(CString csError, const IID& guid);

};
OBJECT_ENTRY_AUTO(__uuidof(IEContextMenu), CIEContextMenu)