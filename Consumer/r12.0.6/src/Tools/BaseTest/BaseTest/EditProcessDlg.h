#pragma once
#include <vector>
#include "afxwin.h"

// EditProcessDlg dialog

class EditProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(EditProcessDlg)

public:
	EditProcessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~EditProcessDlg();

// Dialog Data
	enum { IDD = IDD_EDIT_DIALOG };

	void SetProcessName(std::vector <CString> * vProcessName);

	std::vector<CString> * m_vProcessName;
	std::vector<CString> m_vOldProcessName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cListBox;
	afx_msg void OnBnClickedButtonAdd();

	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
