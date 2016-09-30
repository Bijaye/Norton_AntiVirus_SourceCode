////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// BrowserWindow.h

#ifndef __BrowserWindow_h__
#define __BrowserWindow_h__

#include <atlwin.h>
#include "NAVOptions.h"

class CBrowserWindow : public CWindowImpl<CBrowserWindow>
{
	PWCHAR m_pszPath;
	bool m_bIsDirectory;
	EBrowseType m_eBrowseType;
	CComPtr<IMalloc> m_spMalloc;
	CComPtr<IShellFolder> m_spDesktopFolder;
	CWindow m_Tree;

public:
	CBrowserWindow(PWCHAR pszPath, EBrowseType eBrowseType);

BEGIN_MSG_MAP(CBrowserWindow)
	NOTIFY_CODE_HANDLER(NM_DBLCLK, onDblClick)
END_MSG_MAP()

	HRESULT Browse(BSTR bstrInPath, BSTR* pbstrOutPath);
	void InitDialog(HWND hWnd);
	void SelChanged(HWND hWnd, LPITEMIDLIST pidl);

private:
	LRESULT onDblClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	bool haveNoChildren(void);

	static BOOL CALLBACK enumChildProc(HWND hWnd, LPARAM lParam);
	static int CALLBACK browseCallbackProc(HWND hWnd, UINT uMsg
	                                                , LPARAM lParam
	                                                , LPARAM lpData);
};

#endif  __BrowserWindow_h__
