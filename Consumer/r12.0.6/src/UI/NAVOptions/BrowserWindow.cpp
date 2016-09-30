// BrowserWindow.cpp

#include "StdAfx.h"
#include "Resource.h"
#include "BrowserWindow.h"
#include "ItemIDListPtr.h"
#include "NAVOptionsObj.h"
#include "..\navoptionsres\resource.h"

#ifdef SubclassWindow
#  undef SubclassWindow
#endif

CBrowserWindow::CBrowserWindow(PWCHAR pszPath, EBrowseType eBrowseType) : m_pszPath(pszPath)
                                                                        , m_bIsDirectory(false)
                                                                        , m_eBrowseType(eBrowseType)
{
}

HRESULT CBrowserWindow::Browse(BSTR bstrInPath, BSTR* pbstrOutPath)
{
	USES_CONVERSION;
	HRESULT hr;
	WCHAR szPath[MAX_PATH], szTitle[32];
    WCHAR szWPath[MAX_PATH];

    CCTRACEI(_T("CBrowserWindow::Browse - Starting."));

	::LoadStringW(g_ResModule, Browse_File == m_eBrowseType ? IDS_BrowseFile
	                                                                       : IDS_BrowseDirectory
	                                          , szTitle, SIZEOF(szTitle));

	// Setup the parameters for the common dialog
	BROWSEINFOW bi = { ::GetActiveWindow(), NULL                   // pidlRoot
	                                     , szPath                 // pszDisplayName
										 , szTitle
										 , (Browse_File & m_eBrowseType ? BIF_BROWSEINCLUDEFILES // ulFlags
										                                : 0)
	                                     , browseCallbackProc
										 , reinterpret_cast<LPARAM>(this)
										 , };

	// Get the desktop & global allocator for releasing the PIDLs
	if (FAILED(hr = ::SHGetMalloc(&m_spMalloc))
	 || FAILED(hr = ::SHGetDesktopFolder(&m_spDesktopFolder)))
		return hr;

	// Borwse the disk
	CItemIDListPtr pidlOut(m_spMalloc);
    CCTRACEI(_T("CBrowserWindow::Browse - Calling SHBrowseForFolder."));

	if (pidlOut = ::SHBrowseForFolderW(&bi))
	{
		// Translate the PIDL to BSTR path name
        CCTRACEI(_T("CBrowserWindow::Browse - Received PIDL, translating."));

		if (::SHGetPathFromIDListW(pidlOut, szWPath))
        {
			*pbstrOutPath = ::SysAllocString(W2OLE(szWPath));
        }
		else
        {
            CCTRACEE(_T("CBrowserWindow::Browse - Could not translate PIDL to path string."));
            DWORD err = GetLastError();
            CCTRACEE(_T("CBrowserWindow::Browse - Error code %d."), err);

			return CNAVOptions::Error(IDS_Err_Browse, _T("CBrowserWindow::Browse()"));
        }
	}
	else
    {
        CCTRACEI(_T("CBrowserWindow::Browse - Received no PIDL; returning input string."));
		*pbstrOutPath = ::SysAllocString(bstrInPath);
    }

    CCTRACEW(_T("CBrowserWindow::Browse - Complete, S_OK."));
	return S_OK;
}

void CBrowserWindow::InitDialog(HWND hWnd)
{
	// Find and subclass the tree control
	// Search for the control; don't make any assumptions about the dialog structure
	::EnumChildWindows(hWnd, enumChildProc, reinterpret_cast<LPARAM>(this));

	if (m_pszPath)
		// Select the item sent in the bstrInPath of Browse()
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(m_pszPath));

	CWindow(hWnd).CenterWindow();
}

void CBrowserWindow::SelChanged(HWND hWnd, LPITEMIDLIST pidl)
{
	USES_CONVERSION;
	ULONG dwAttributes;
    CStrRet Name(m_spMalloc);

    if (Browse_Disk == m_eBrowseType  // TODO: Handle disks
	// Get item's path
	 || FAILED(m_spDesktopFolder->GetDisplayNameOf(pidl, SHGDN_NORMAL | SHGDN_FORPARSING,&Name)))
    {
        // Some problem with the file or this is not a drive, directory or a file
        ::SendMessage(hWnd, BFFM_ENABLEOK, 0, 0);
        return;
    }

    // Check the the item's type
    dwAttributes = ::GetFileAttributesW(STRRET_WSTR == Name.uType ? Name.pOleStr
        : STRRET_CSTR == Name.uType ? T2W(Name.cStr)
        : T2W(PTCHAR(pidl)) + Name.uOffset);

	if(0xFFFFFFFFUL == dwAttributes)
	{
		// This is not a drive, directory or a file
		::SendMessage(hWnd, BFFM_ENABLEOK, 0, 0);
		return;
	}

	// Q: is this a directory
	m_bIsDirectory = FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & dwAttributes);

	// Turn on/off the OK button accordingly
	switch(m_eBrowseType)
	{
	case Browse_Directory:
		::SendMessage(hWnd, BFFM_ENABLEOK, 0, m_bIsDirectory);
		break;
	case Browse_File:
		::SendMessage(hWnd, BFFM_ENABLEOK, 0, !m_bIsDirectory);
		break;
	case Browse_File | Browse_Directory:
		::SendMessage(hWnd, BFFM_ENABLEOK, 0, TRUE);
		break;
	case Browse_Disk:  // TODO: Handle disks
		::SendMessage(hWnd, BFFM_ENABLEOK, 0, 0);
		break;
	}
}

LRESULT CBrowserWindow::onDblClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	CWindow OK(::GetDlgItem(GetParent(), IDOK));

	// If the user double clicked on a scannable item simulate an OK click.
	// We take action only on files or directories without children
	// since the default TreeView action is to expand/collepse when
	// the user double click.
	if (OK.IsWindowEnabled() && (!m_bIsDirectory || haveNoChildren()))
		OK.PostMessage(WM_LBUTTONDOWN);
	else
		bHandled = FALSE;

	return 0L;
}

bool CBrowserWindow::haveNoChildren(void)
{
	TVITEM tvi = { TVIF_CHILDREN, TreeView_GetSelection(m_Tree) };

	TreeView_GetItem(m_Tree, &tvi);

	return !tvi.cChildren;
}

BOOL CALLBACK CBrowserWindow::enumChildProc(HWND hWnd, LPARAM lParam)
{
	WCHAR szClassName[128];

	// Q: Is this child the TreeView control
	if (::GetClassNameW(hWnd, szClassName, SIZEOF(szClassName)) && !::wcscmp(szClassName, WC_TREEVIEWW))
	{
		CBrowserWindow* pdlg = reinterpret_cast<CBrowserWindow*>(lParam);

		// Get a reference to the TreeView
		pdlg->m_Tree = hWnd;
		// Subclass the tree control parent
		pdlg->SubclassWindow(pdlg->m_Tree.GetParent());

		return FALSE;
	}

	return TRUE;
}

int CALLBACK CBrowserWindow::browseCallbackProc(HWND hWnd, UINT uMsg
                                                         , LPARAM lParam
                                                         , LPARAM lpData)
{
	switch(uMsg)
	{
		CBrowserWindow* pbp;

	case BFFM_INITIALIZED:
		pbp = reinterpret_cast<CBrowserWindow*>(lpData);
		pbp->InitDialog(hWnd);
		break;
	case BFFM_SELCHANGED:
		pbp = reinterpret_cast<CBrowserWindow*>(lpData);
		pbp->SelChanged(hWnd, reinterpret_cast<LPITEMIDLIST>(lParam));
		break;
	}

	return 0;
}
