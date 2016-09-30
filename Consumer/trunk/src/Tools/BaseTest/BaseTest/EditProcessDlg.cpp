////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// EditProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseTest.h"
#include "EditProcessDlg.h"
#include <vector>
#include ".\editprocessdlg.h"
#include "AddProcessDlg.h"


// EditProcessDlg dialog

IMPLEMENT_DYNAMIC(EditProcessDlg, CDialog)
EditProcessDlg::EditProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EditProcessDlg::IDD, pParent)
{
}

EditProcessDlg::~EditProcessDlg()
{
}

void EditProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_cListBox);
}


BEGIN_MESSAGE_MAP(EditProcessDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// EditProcessDlg message handlers
void EditProcessDlg::SetProcessName(std::vector<CString> * vProcessName)
{
	m_vProcessName =  vProcessName;
	m_vOldProcessName.clear();
	for (int i = 0; i< m_vProcessName->size(); i++)
		m_vOldProcessName.push_back(m_vProcessName->at(i));
}
BOOL EditProcessDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();
	for (int i = 0; i< m_vProcessName->size(); i++)
		m_cListBox.AddString(m_vProcessName->at(i));
	return TRUE;
}
void EditProcessDlg::OnBnClickedButtonAdd()
{
	AddProcessDlg dlg;
	dlg.SetProcessName( m_vProcessName);
	INT_PTR nResponse = dlg.DoModal();

	m_cListBox.ResetContent();
	for (int i = 0; i< m_vProcessName->size(); i++)
		m_cListBox.AddString(m_vProcessName->at(i));
}

void EditProcessDlg::OnBnClickedButtonRemove()
{
	int nIndex = m_cListBox.GetCurSel();
	if (nIndex != LB_ERR)
		m_cListBox.DeleteString(nIndex);
	UpdateData(TRUE);
}

void EditProcessDlg::OnBnClickedOk()
{
	CString strTemp;
	m_vProcessName->clear();
	for (int i = 0; i< m_cListBox.GetCount(); i++)
	{
		m_cListBox.GetText(i,strTemp);
		m_vProcessName->push_back(strTemp);
	}
	OnOK();
}

void EditProcessDlg::OnBnClickedCancel()
{
	m_vProcessName->clear();
	for (int i = 0; i< m_vOldProcessName.size(); i++)
		m_vProcessName->push_back( m_vOldProcessName.at(i));
	OnCancel();
}
