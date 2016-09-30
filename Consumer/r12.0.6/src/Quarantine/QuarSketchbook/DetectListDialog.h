#pragma once
#include "afxwin.h"


// CDetectListDialog dialog

class CDetectListDialog : public CDialog
{
	DECLARE_DYNAMIC(CDetectListDialog)

public:
	CDetectListDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDetectListDialog();

// Dialog Data
	enum { IDD = IDD_DETECTLIST };

	CStringList m_stringListFile;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CEdit m_editCtrlFileList;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	DWORD m_dwVID;
	afx_msg void OnBnClickedAdd();
	CEdit m_editCtrlManual;
};
 