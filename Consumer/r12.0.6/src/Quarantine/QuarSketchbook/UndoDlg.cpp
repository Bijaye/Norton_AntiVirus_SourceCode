// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// UndoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sketchbook.h"
#include "UndoDlg.h"
#include ".\undodlg.h"


// CUndoDlg dialog

IMPLEMENT_DYNAMIC(CUndoDlg, CDialog)
CUndoDlg::CUndoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUndoDlg::IDD, pParent)
{
	m_type = Save;
}

CUndoDlg::~CUndoDlg()
{
}

void CUndoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUndoDlg, CDialog)
	ON_BN_CLICKED(IDC_UNDOSAVE, OnBnClickedUndosave)
	ON_BN_CLICKED(IDC_UNDOSAVETO, OnBnClickedUndosaveto)
	ON_BN_CLICKED(IDC_UNDOSAVEAS, OnBnClickedUndosaveas)
END_MESSAGE_MAP()


// CUndoDlg message handlers



void CUndoDlg::OnBnClickedUndosave()
{
	// TODO: Add your control notification handler code here
	m_type = Save;
	OnOK();
}

void CUndoDlg::OnBnClickedUndosaveto()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO info;

	ZeroMemory(&info, sizeof(BROWSEINFO));
	info.lpszTitle = _T("Select the folder which you want to restore the filen");
	info.ulFlags = BIF_NONEWFOLDERBUTTON | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN;

	LPITEMIDLIST item = SHBrowseForFolder(&info);
	if(item != NULL)
	{
		TCHAR szPath[MAX_PATH];
		if(SHGetPathFromIDList(item, szPath))
		{
			strDir = szPath;
			m_type = SaveTo;
			OnOK();
		}
	}
}

void CUndoDlg::OnBnClickedUndosaveas()
{
	// TODO: Add your control notification handler code here
	CFileDialog Dlg(FALSE);

	if(IDOK == Dlg.DoModal())
	{
		m_type = SaveAs;
		strDir = Dlg.GetPathName();
		strDir = strDir.Left(strDir.ReverseFind(_T('\\')) + 1);
		strFile = Dlg.GetFileName();
		OnOK();
	}
}
