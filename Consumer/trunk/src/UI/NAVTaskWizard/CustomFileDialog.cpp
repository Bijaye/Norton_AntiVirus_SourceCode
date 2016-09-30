////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CustomFileDialog.cpp : implementation file
//
#include "stdafx.h"
#include "CustomFileDialog.h"
#include "..\\NavTaskWizardRes\\resource.h"

CCustomFileDialog::CCustomFileDialog(BOOL bOpenFileDialog,
									LPCTSTR lpszDefExt,
									LPCTSTR lpszFileName,
									DWORD dwFlags,
									LPCTSTR lpszFilter,
									CWnd* pParentWnd,
									DWORD dwSize):
CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, dwSize)
{
}

CCustomFileDialog::~CCustomFileDialog(void)
{
}

BOOL CCustomFileDialog::OnInitDialog()
{
	CCTRCTXI0(L"Enter");
	
	CFileDialog::OnInitDialog();

	// Set OK button text to "Scan"
	CStringW sText;
	sText.LoadString(GetResInst(), IDS_SCAN);
	GetParent()->SetDlgItemText(IDOK, sText.GetString());

	sText.LoadString(GetResInst(), IDS_CANCEL);
	GetParent()->SetDlgItemText(IDCANCEL, sText.GetString());
	CCTRCTXI0(L"Exit");

	return TRUE;
}
