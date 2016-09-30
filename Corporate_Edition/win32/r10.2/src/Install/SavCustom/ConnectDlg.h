// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_CONNECTDLG_H__9CBFE991_5F8C_11D3_A2B1_00C04F91B157__INCLUDED_)
#define AFX_CONNECTDLG_H__9CBFE991_5F8C_11D3_A2B1_00C04F91B157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog

class CConnectDlg : public CDialog
{
// Construction
public:
	CString m_Selection;
	CConnectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectDlg)
	enum { IDD = IDD_SERVER_LIST };
	CListCtrl	m_listCtrl;
	//}}AFX_DATA

	CImageList g_imgList;
	CBitmap g_bitmap;

    BOOL DrawCutInLine( CDC* pPdc, LPRECT pRect, BOOL bSingle );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnInsertitemServerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFindcomputerButton();
	afx_msg void OnKeydownServerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedServerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRetBack();
	afx_msg void OnRetNext();
	afx_msg void OnRetCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTDLG_H__9CBFE991_5F8C_11D3_A2B1_00C04F91B157__INCLUDED_)
