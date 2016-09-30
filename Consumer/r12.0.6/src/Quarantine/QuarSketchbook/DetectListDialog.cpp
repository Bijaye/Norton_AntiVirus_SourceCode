// DetectListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Sketchbook.h"
#include "DetectListDialog.h"
#include ".\detectlistdialog.h"


// CDetectListDialog dialog

IMPLEMENT_DYNAMIC(CDetectListDialog, CDialog)
CDetectListDialog::CDetectListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDetectListDialog::IDD, pParent)
	, m_dwVID(0)
{
}

CDetectListDialog::~CDetectListDialog()
{
}

void CDetectListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELIST, m_editCtrlFileList);
	DDX_Text(pDX, IDC_VID, m_dwVID);
	DDX_Control(pDX, IDC_ADDSTRING, m_editCtrlManual);
}


BEGIN_MESSAGE_MAP(CDetectListDialog, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
END_MESSAGE_MAP()


// CDetectListDialog message handlers



void CDetectListDialog::OnBnClickedBrowse()
{
	// TODO: Add your control notification handler code here

	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR);

	if (dlgFile.DoModal() == IDOK)
	{
		TCHAR szLongPath[MAX_PATH];
		TCHAR szShortPath[MAX_PATH];

		if(::PathFileExists(dlgFile.GetPathName()))
		{
			::GetLongPathName(dlgFile.GetPathName(), szLongPath, MAX_PATH); 
			::GetShortPathName(dlgFile.GetPathName(), szShortPath, MAX_PATH);
		}
		else
		{
			CFile Target;
			Target.Open(dlgFile.GetPathName(), CFile::modeCreate | CFile::modeReadWrite);
			Target.Close();

			::GetLongPathName(dlgFile.GetPathName(), szLongPath, MAX_PATH); 
			::GetShortPathName(dlgFile.GetPathName(), szShortPath, MAX_PATH);

			::DeleteFile(dlgFile.GetPathName());
		}

		m_stringListFile.AddTail(szLongPath);
		m_stringListFile.AddTail(szShortPath);

		CString strText;
		m_editCtrlFileList.GetWindowText(strText);
		strText += szLongPath;
		strText += _T("; ");
		strText += szShortPath;
		strText += _T("; ");
		m_editCtrlFileList.SetWindowText(strText);
	}
}

void CDetectListDialog::OnBnClickedAdd()
{
	// TODO: Add your control notification handler code here

	CString strText;
	m_editCtrlManual.GetWindowText(strText);
	if(strText != _T(""))
	{
		m_stringListFile.AddTail(strText);
		m_editCtrlManual.SetWindowText(_T(""));

		CString strWin;
		m_editCtrlFileList.GetWindowText(strWin);
		strWin += strText;
		strWin += _T("; ");
		m_editCtrlFileList.SetWindowText(strWin);
	}
}
