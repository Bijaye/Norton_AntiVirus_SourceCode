// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SAVClientMoverDlg.h : header file
//

#if !defined(AFX_SAVCLIENTMOVERDLG_H__C999D7B5_F08E_4C69_AE78_5570BA18E9B6__INCLUDED_)
#define AFX_SAVCLIENTMOVERDLG_H__C999D7B5_F08E_4C69_AE78_5570BA18E9B6__INCLUDED_

#include "PasswordCheck.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSAVClientMoverDlg dialog

class CSAVClientMoverDlg : public CDialog
{
// Construction
public:
	HRESULT KeepTheWindowMoving( );
	CPasswordCheck m_PasswordCheck;
	UINT m_nTimer;
	void RefreshViewOfClientLists();
	bool m_bTransmanInitialized;
	CSAVClientMoverDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSAVClientMoverDlg)
	enum { IDD = IDD_SAVCLIENTMOVER_DIALOG };
	CListBox     m_ctlMoveResults;
	CComboBox	m_TargetParentCombo;
	CComboBox	m_SourceParentCombo;
	CListBox	m_TargetClientList;
	CListBox	m_SourceClientList;
	CString	m_csSourceParent;
	CString	m_csTargetParent;
	CString	m_csStatus;
	CString	m_csFreshStatus;
	CString	m_ctlMoveResultStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSAVClientMoverDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSAVClientMoverDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeComboSourceParent();
	afx_msg void OnSelchangeClientListSource();
	afx_msg void OnSelchangeComboTargetParent();
	afx_msg void OnMove();
	afx_msg void OnMoveBack();
	afx_msg void OnSelchangeClientListTarget();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnButtonRefresh();
	afx_msg void OnSelchangeMoveResultsView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void CSAVClientMoverDlg::DoTheMove( CListBox &csClientList, CArray<CString,CString> &csSourceClients, CArray<CString,CString> &csSourceClientsFullName, CArray<CBA_Addr,CBA_Addr> &csSourceClientsAddress, const CString &csSourceParent, const CString &csTargetParent );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVCLIENTMOVERDLG_H__C999D7B5_F08E_4C69_AE78_5570BA18E9B6__INCLUDED_)
