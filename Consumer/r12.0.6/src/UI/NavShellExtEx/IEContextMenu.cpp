// IEContextMenu.cpp : Implementation of CIEContextMenu
#include "stdafx.h"
#include "IEContextMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _INIT_COSVERSIONINFO
#include "ssosinfo.h"

StahlSoft::COSVersionInfo g_OSInfo;

// CIEContextMenu

/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
/////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo::InterfaceSupportsErrorInfo()
STDMETHODIMP CIEContextMenu::InterfaceSupportsErrorInfo(REFIID riid)
{
	return InlineIsEqualGUID(IID_IContextMenu, riid)
		|| InlineIsEqualGUID(IID_IShellExtInit, riid) ? S_OK : S_FALSE;
}



// CIEContextMenu::CIEContextMenu()
CIEContextMenu::CIEContextMenu()
#ifdef __Show_Menu_Bitmap__
              : m_hContextMenuBMP(HBITMAP(::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_IECONTEXTMENU)
                                                                                   , IMAGE_BITMAP
                                                                                   , 0, 0
                                                                                   , LR_LOADTRANSPARENT
                                                                                   | LR_LOADMAP3DCOLORS))) // Load menu bitmap.
{
	g_Log.LogT("CIEContextMenu::CIEContextMenu()");

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
// IShellExtInit
/////////////////////////////////////////////////////////////////////////////
// IShellExtInit::Initialize()
HRESULT CIEContextMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hkeyProgID)
{
	g_Log.LogT("CIEContextMenu::Initialize()");
	CRegKey key;
	LONG rc = 0;

	if( m_csProductName.IsEmpty() )
	{
		rc = key.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\Norton AntiVirus"), KEY_READ);
		if(rc == ERROR_SUCCESS)
		{
			ULONG uSize = 128;

			rc = key.QueryStringValue(_T("ProductName"), m_csProductName.GetBufferSetLength(uSize), &uSize);
			if(rc == ERROR_SUCCESS)
			{
				m_csProductName.ReleaseBuffer();
			}
		}

		if(rc != ERROR_SUCCESS)
		{
			m_csProductName.LoadString(_AtlBaseModule.GetResourceInstance(), IDS_NAV);
		}
	}
	if (!lpdobj)
		return E_INVALIDARG;

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
	if( ! m_vItems.size() && !m_vDeniels.size() && !m_vwInvaildPaths.size())
		return S_OK;

	// Insert a separator
	InsertMenu(hMenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL); 

	UINT uMenuFlag = MF_STRING | MF_BYPOSITION;

	CString csMenuItem;
	GetResourceString(IDS_MENU_ITEM,csMenuItem);

	// Insert the menu items
	InsertMenu(hMenu, indexMenu, uMenuFlag, idCmdFirst + ID_SCAN, csMenuItem); 
#ifdef __Show_Menu_Bitmap__
	SetMenuItemBitmaps( hMenu, indexMenu++, MF_BYPOSITION, m_hContextMenuBMP, m_hContextMenuBMP );
#endif __Show_Menu_Bitmap__

	// Must return the greatest menu item identifier plus one. 
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 2 /* One item */);
}

/////////////////////////////////////////////////////////////////////////////
// IContextMenu::InvokeCommand
STDMETHODIMP CIEContextMenu::InvokeCommand( LPCMINVOKECOMMANDINFOEx lpcmi )
{
	// If the high-order word of lpcmi->lpVerb is not NULL, this 
	// function was called by an application and lpVerb is a command 
	// that should be activated. Otherwise, the shell has called this 
	// function, and the low-order word of lpcmi->lpVerb is the 
	// identifier of the menu item that the user selected. 
	if (HIWORD(lpcmi->lpVerb))
		return E_INVALIDARG;

	HRESULT hr = S_OK;
	switch (LOWORD(lpcmi->lpVerb)) 
	{
	case ID_SCAN: // Fire off the scan.
		{
			hr = RunNAVW();
			if(FAILED(hr))
			{
				ShowError(hr);
			}
		}
		break;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// IContextMenu::GetCommandString()
STDMETHODIMP CIEContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType
                                                        , UINT *pwReserved
                                                        , LPSTR pszName
                                                        , UINT cchMax )
{
	if(uType & GCS_HELPTEXT) 
	{
		USES_CONVERSION;
		CString csFormat;
		CString csMsg;

		csFormat.LoadString(_AtlBaseModule.GetResourceInstance(), IDS_STATUSMSG);
		csMsg.Format(csFormat, m_csProductName);

		if(::g_OSInfo.IsWinNT())
			lstrcpynW(reinterpret_cast<WCHAR*>(pszName), T2W(csMsg), csMsg.GetLength());
		else											
			lstrcpynA(pszName, csMsg, csMsg.GetLength());
	}

	return S_OK;
}

HRESULT	CIEContextMenu::_Error(int iId, const IID& guid)
{
	return Error(iId,guid);
}


HRESULT	CIEContextMenu::_Error(CString csError, const IID& guid)
{
	return Error(csError,guid);
}
