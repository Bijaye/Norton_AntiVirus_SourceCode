// SoftEnforceDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CSoftEnforceDlg dialog
class CSoftEnforceDlg : public CDialog
{
// Construction
public:
	CSoftEnforceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SOFTENFORCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CImageList m_ImageList;

	afx_msg void OnBnClickedButtonLoad();
	CListCtrl m_listView;
	afx_msg void OnBnClickedButtonRun();
	CListCtrl m_listOut;
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonEdit();
};
