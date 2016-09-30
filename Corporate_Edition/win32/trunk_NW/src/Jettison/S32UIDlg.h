// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// S32UIDlg.h : header file
//

#if !defined(AFX_S32UIDLG_H__8905E01D_E911_11D1_A75E_0000E8D3EFD6__INCLUDED_)
#define AFX_S32UIDLG_H__8905E01D_E911_11D1_A75E_0000E8D3EFD6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CS32UIDlg dialog

class CS32UIDlg : public CDialog
{
// Construction
public:
	void RunUninstall();
	CS32UIDlg(CWnd* pParent = NULL);	// standard constructor
  #define Uninstall (WM_USER + 100)  

// Dialog Data
	//{{AFX_DATA(CS32UIDlg)
	enum { IDD = IDD_S32UI_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CS32UIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CS32UIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClickUninstall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_S32UIDLG_H__8905E01D_E911_11D1_A75E_0000E8D3EFD6__INCLUDED_)
