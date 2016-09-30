////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// IEContextMenu.cpp : Implementation of CIEContextMenu
#include "stdafx.h"
#include "..\\NavShellExtExRes\\resource.h"
#include "IEContextMenu.h"
#include "ISVersion.h"
#include "ccOSInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
	CCTRACEI(_T("CIEContextMenu::Initialize()"));

	if( m_csProductName.IsEmpty())
	{
		m_csProductName = CISVersion::GetProductName();
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
	if( ! m_vItems.size() && !m_vDenials.size() && !m_vwInvaildPaths.size())
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
	CCTRCTXI0(L"Enter");

	HRESULT hr = S_OK;
	bool bLaunchScannner = false;

	// If the high-order word of lpcmi->lpVerb is not NULL, this 
	// function was called by an application and lpVerb is a command 
	// that should be activated. Otherwise, the shell has called this 
	// function, and the low-order word of lpcmi->lpVerb is the 
	// identifier of the menu item that the user selected. 
	if (HIWORD(lpcmi->lpVerb))
	{
		if(ccLib::COSInfo::IsWinVista(true))
		{
			// On Vista, we experience a strange behavior:
			// 1. For a file, lpVerb is an offset identifier of scan command. 
			// 2. For a shortcut, lpVerb is a description of the scan command. 
			//	  (Set in CIEContextMenu::GetCommandString())
			//
			// On XP, it is always an offset identifier of the scan command.
			// This seems like a bug in Vista. While waiting for
			// Vista service pack, we will apply a workaround by 
			// checking to see if lpVerb is description of scan command.

			CString sVerb(lpcmi->lpVerb);
			CCTRCTXI1(L"lpcmi->lpVerb: %s", sVerb.GetString());

			if(m_sCmdString.IsEmpty())
			{
				m_sCmdString.LoadString(_AtlBaseModule.GetResourceInstance(), IDS_STATUSMSG);
			}

			if(0 == m_sCmdString.CompareNoCase(sVerb.GetString()))
			{
				bLaunchScannner = true;
			}
		}
		else
		{
			// XP
			return E_INVALIDARG;
		}
	}
	else
	{
		CCTRCTXI1(L"LOWORD(lpcmi->lpVerb): %d", LOWORD(lpcmi->lpVerb));

		switch (LOWORD(lpcmi->lpVerb)) 
		{
		case ID_SCAN: 
			{
				bLaunchScannner = true;
			}
			break;
		}
	}


	// Fire off the scan
	if(bLaunchScannner)
	{
		hr = RunNAVW();
		if(FAILED(hr))
		{
			ShowError(hr);
		}
	}

	CCTRCTXI0(L"Exit");
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
		if(pszName)
		{
			// Get the text that will be displayed in the Status bar
			// Per MSDN, the text length should be 40 characters or less.
			if(m_sCmdString.IsEmpty())
			{
				m_sCmdString.LoadString(_AtlBaseModule.GetResourceInstance(), IDS_STATUSMSG);
			}

			if(uType & GCS_UNICODE)
			{
				// We need to cast pszName to a Unicode string, and then use the
				// Unicode string copy API.
				lstrcpynW(reinterpret_cast<LPWSTR>(pszName), m_sCmdString.GetString(), cchMax);
			}
			else
			{
				USES_CONVERSION;

				// Use the ANSI string copy API to return the help string.
				lstrcpynA(pszName, W2CA(m_sCmdString.GetString()), cchMax);
			}
		}
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
