// AddProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseTest.h"
#include "AddProcessDlg.h"
#include ".\addprocessdlg.h"
#include <vector>


// AddProcessDlg dialog

IMPLEMENT_DYNAMIC(AddProcessDlg, CDialog)
AddProcessDlg::AddProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddProcessDlg::IDD, pParent)
	, m_strProcessName(_T(""))
{
}

AddProcessDlg::~AddProcessDlg()
{
}

void AddProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PNAME, m_strProcessName);
}


BEGIN_MESSAGE_MAP(AddProcessDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()



BOOL AddProcessDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();
	//for (int i = 0; i< m_vProcessName->size(); i++)
		//m_cListBox.AddString(m_vProcessName->at(i));
	return TRUE;
}
void AddProcessDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_vProcessName->push_back(m_strProcessName);
	OnOK();
}
void AddProcessDlg::SetProcessName(std::vector<CString> * vProcessName)
{
	m_vProcessName =  vProcessName;
}