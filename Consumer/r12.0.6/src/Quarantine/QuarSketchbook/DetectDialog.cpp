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
// DetectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Sketchbook.h"
#include "DetectDialog.h"
#include ".\detectdialog.h"


// CDetectDialog dialog

IMPLEMENT_DYNAMIC(CDetectDialog, CDialog)
CDetectDialog::CDetectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDetectDialog::IDD, pParent)
	, m_dwVID(0)
{
	_tcscpy(m_szShortPath, _T(""));
	_tcscpy(m_szLongPath, _T(""));
}

CDetectDialog::~CDetectDialog()
{
}

void CDetectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VID, m_dwVID);
	DDX_Control(pDX, IDC_FILE, m_editCtrlFile);
	DDX_Control(pDX, IDC_FILESHORT, m_editCtrlShortFile);
}


BEGIN_MESSAGE_MAP(CDetectDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDetectDialog message handlers

void CDetectDialog::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR);

	if (dlgFile.DoModal() == IDOK)
	{
		if(::PathFileExists(dlgFile.GetPathName()))
		{
			::GetLongPathName(dlgFile.GetPathName(), m_szLongPath, MAX_PATH); 
			::GetShortPathName(dlgFile.GetPathName(), m_szShortPath, MAX_PATH);
		}
		else
		{
			CFile Target;
			Target.Open(dlgFile.GetPathName(), CFile::modeCreate | CFile::modeReadWrite);
			Target.Close();

			::GetLongPathName(dlgFile.GetPathName(), m_szLongPath, MAX_PATH); 
			::GetShortPathName(dlgFile.GetPathName(), m_szShortPath, MAX_PATH);

			::DeleteFile(dlgFile.GetPathName());
		}

		m_editCtrlFile.SetWindowText(m_szLongPath);
		m_editCtrlShortFile.SetWindowText(m_szShortPath);
	}
	}

void CDetectDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	OnOK();
}
