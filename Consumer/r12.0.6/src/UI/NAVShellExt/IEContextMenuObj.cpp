// IEContextMenuObj.cpp : Implementation of CIEContextMenu
#include "StdAfx.h"
#include "NAVShellExt.h"
#include "IEContextMenuObj.h"
#include "cltLicenseConstants.h"
#include "SSOsinfo.h"
#include "AVRESBranding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIEContextMenu
/////////////////////////////////////////////////////////////////////////////
// CIEContextMenu::CIEContextMenu()
CIEContextMenu::CIEContextMenu()
#ifdef __Show_Menu_Bitmap__
              : m_hContextMenuBMP(HBITMAP(::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_IECONTEXTMENU)
                                                                                   , IMAGE_BITMAP
                                                                                   , 0, 0
                                                                                   , LR_LOADTRANSPARENT
                                                                                   | LR_LOADMAP3DCOLORS))) // Load menu bitmap.
{
	// Fit the bmp to the menu
	m_hContextMenuBMP = HBITMAP(::CopyImage(m_hContextMenuBMP, IMAGE_BITMAP                        // type of image to copy
	                                                         , ::GetSystemMetrics(SM_CXMENUCHECK)  // desired width of new image
	                                                         , ::GetSystemMetrics(SM_CYMENUCHECK)  // desired height of new image
	                                                         , LR_COPYDELETEORG | LR_CREATEDIBSECTION));
}

/////////////////////////////////////////////////////////////////////////////
// CIEContextMenu::~CIEContextMenu()
CIEContextMenu::~CIEContextMenu()
{
	if(m_hContextMenuBMP)
	{
		::DeleteObject(m_hContextMenuBMP);
		m_hContextMenuBMP = NULL;
	}
}
#else  __Show_Menu_Bitmap__
{
}
#endif __Show_Menu_Bitmap__

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo::InterfaceSupportsErrorInfo()
STDMETHODIMP CIEContextMenu::InterfaceSupportsErrorInfo(REFIID riid)
{
	return InlineIsEqualGUID(IID_IContextMenu, riid)
		|| InlineIsEqualGUID(IID_IShellExtInit, riid) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IShellExtInit
/////////////////////////////////////////////////////////////////////////////
// IShellExtInit::Initialize()
HRESULT CIEContextMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hkeyProgID)
{
	if (!lpdobj)
		return E_INVALIDARG;

	CBrandingRes BrandRes;
	m_csProductName = BrandRes.ProductName();

	// Save the file path
	return GetSelectionList(lpdobj);
}

/////////////////////////////////////////////////////////////////////////////
// IContextMenu
/////////////////////////////////////////////////////////////////////////////
// IContextMenu::QueryContextMenu
STDMETHODIMP CIEContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu
                                                         , UINT idCmdFirst
                                                         , UINT idCmdLast
                                                         , UINT uFlags)
{
	// We only want to add menu items if we have items to scan
	if( m_vItems.size() == 0 )
		return S_OK;

	// Insert a separator
	InsertMenu(hMenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL); 

	UINT uMenuFlag = MF_STRING | MF_BYPOSITION;


	// Insert the menu items
	CString csMenuItem, csFormat;
	csFormat.LoadString(_Module.GetResourceInstance(), IDS_MENU_ITEM);
	csMenuItem.Format(csFormat, m_csProductName);

	InsertMenu(hMenu, indexMenu, uMenuFlag, idCmdFirst + ID_SCAN, csMenuItem); 
#ifdef __Show_Menu_Bitmap__
	SetMenuItemBitmaps( hMenu, indexMenu++, MF_BYPOSITION, m_hContextMenuBMP, m_hContextMenuBMP );
#endif __Show_Menu_Bitmap__

	// Must return the greatest menu item identifier plus one. 
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 2 /* One item */);
}

/////////////////////////////////////////////////////////////////////////////
// IContextMenu::InvokeCommand
STDMETHODIMP CIEContextMenu::InvokeCommand( LPCMINVOKECOMMANDINFO lpcmi )
{
	// If the high-order word of lpcmi->lpVerb is not NULL, this 
	// function was called by an application and lpVerb is a command 
	// that should be activated. Otherwise, the shell has called this 
	// function, and the low-order word of lpcmi->lpVerb is the 
	// identifier of the menu item that the user selected. 
	if (HIWORD(lpcmi->lpVerb))
		return E_INVALIDARG;

	switch (LOWORD(lpcmi->lpVerb)) 
	{
	case ID_SCAN: // Fire off the scan.
		return RunNAVW();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IContextMenu::GetCommandString()
STDMETHODIMP CIEContextMenu::GetCommandString(UINT idCmd, UINT uType
                                                        , UINT *pwReserved
                                                        , LPSTR pszName
                                                        , UINT cchMax )
{
	if(uType & GCS_HELPTEXT) 
	{
		USES_CONVERSION;
		TCHAR szMsg[256];
		int nLen = min(SIZEOF(szMsg), cchMax);

		::LoadString(_Module.GetResourceInstance(), IDS_STATUSMSG, szMsg, SIZEOF(szMsg));

		if(::g_OSInfo.IsWinNT())
			lstrcpynW(reinterpret_cast<WCHAR*>(pszName), T2W(szMsg), nLen);
		else
			lstrcpynA(pszName, CT2A(szMsg, CP_ACP), nLen);
	}

	return S_OK;
}
