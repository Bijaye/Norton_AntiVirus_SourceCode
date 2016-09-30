
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// RestoreFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RestoreFileDialog.h"
#include "resource.h" // for IDS_ error strings

/////////////////////////////////////////////////////////////////////////////
// CRestoreFileDialog
//
IMPLEMENT_DYNAMIC(CRestoreFileDialog, CFileDialog)
//
// Constructor
//
CRestoreFileDialog::CRestoreFileDialog(BOOL bOpenFileDialog, 
									   LPCTSTR lpszDefExt, 
									   LPCTSTR lpszFileName, 
									   DWORD dwFlags, 
									   LPCTSTR lpszFilter, 
									   CWnd* pParentWnd) :
					CFileDialog(FALSE, 
								lpszDefExt, 
						        lpszFileName, 
								dwFlags, 
								lpszFilter, 
								pParentWnd)
{
    // Set title
    m_sTitle.LoadString( IDS_RESTORE_FILE_TITLE );
    m_ofn.lpstrTitle = m_sTitle.GetBuffer(0);
}
//
// Destructor
//
CRestoreFileDialog::~CRestoreFileDialog()
{
}

BEGIN_MESSAGE_MAP(CRestoreFileDialog, CFileDialog)
END_MESSAGE_MAP()
//
// Initialization of the Restore File Dialog Box
//
BOOL CRestoreFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	//
	// Change the "Save" text to "Restore"
	//
	SetControlText(IDOK, _T("Restore"));
	//
	// Set the initial directory to C:\
	//
	TCHAR chFolder[255] = "C:\\";
	m_ofn.lpstrInitialDir = chFolder;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
