////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CustomFileDialog.h : header/implementation file
//
#pragma once

#include <afxdlgs.h>

class CCustomFileDialog: public CFileDialog
{
public:
	// ctor/dtor
	CCustomFileDialog(BOOL bOpenFileDialog,
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0);

	~CCustomFileDialog(void);

	// Override methods
	virtual BOOL OnInitDialog();
};
