//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// ExportDlg.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/exportdlg.cpv  $
// 
//    Rev 1.1   21 May 1998 11:29:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.0   14 Apr 1998 00:41:54   DBuches
// Initial revision.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "ExportDlg.h"
#include "qconsoledoc.h"
#include "qconhlp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

CExportDlg::CExportDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	//{{AFX_DATA_INIT(CExportDlg)
	m_iExport = 0;
	m_iFormat = 0;
    m_iViewMode = 0;
	//}}AFX_DATA_INIT

    // Set title
    m_sTitle.LoadString(IDS_EXPORT_FILE_TITLE);
    m_ofn.lpstrTitle = m_sTitle.GetBuffer(0);

    // Set up extension template.
    m_ofn.Flags |= OFN_ENABLETEMPLATE;
    m_ofn.hInstance = AfxFindResourceHandle(MAKEINTRESOURCE(IDD_EXPORT_DIALOG), RT_DIALOG);
    m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_EXPORT_DIALOG);

    // Set bogus control ID.
    m_iCtrlID = -1;
}


void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_iExport);
	DDX_Radio(pDX, IDC_RADIO3, m_iFormat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportDlg, CFileDialog)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_WM_DESTROY()
    ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers

//////////////////////////////////////////////////////////////////////////////
// Handles WM_INITDIALOG
// 4/13/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
BOOL CExportDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	// Set correct button text for This view radio button
    CWnd *p = GetDlgItem(IDC_RADIO2);

    // Get window text and append the view mode name
    CString sWindowText;
    CString sViewModeName;

    switch(m_iViewMode)
	{
		case VIEW_MODE_EXPANDED_THREATS:
			sViewModeName.LoadString(IDS_TREE_ROOT_EXPANDED_THREATS);
			break;
		case VIEW_MODE_VIRAL_THREATS:
			sViewModeName.LoadString(IDS_TREE_ROOT_VIRAL_THREATS);
			break;
	}

    sWindowText.Format(_T("(%s)"), sViewModeName);
    GetDlgItem(IDC_NAME_STATIC)->SetWindowText(sWindowText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//////////////////////////////////////////////////////////////////////////////
// Handles OK button
// 4/13/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnOK()
{
}


//////////////////////////////////////////////////////////////////////////////
// 4/13/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnDestroy()
{
	UpdateData();

    CFileDialog::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////////
// Handles context menu click
// 5/21/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // WJORDAN 7-30-03: Removing context sensitive help
}


//////////////////////////////////////////////////////////////////////////////
// Handles F1 and ? button help
// 5/21/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
BOOL CExportDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// WJORDAN 7-30-03: Removing context sensitive help
    return TRUE;
}
