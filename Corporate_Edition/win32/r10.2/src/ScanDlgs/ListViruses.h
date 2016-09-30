// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_LISTVIRUSES_H__1A6741AB_A22E_11D1_8211_00A0C95C0756__INCLUDED_)
#define AFX_LISTVIRUSES_H__1A6741AB_A22E_11D1_8211_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ListViruses.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListViruses dialog

class CListViruses : public CDialog
{
// Construction
public:
	CListViruses(CWnd* pParent = NULL);   // standard constructor

	CLSID		m_clsidProvider;
	CString		m_selectedViruses;
	CString		m_strComputerName;
	VIRUSESCB	m_callback;
	void		*m_pContext;

	void OnGetMinMaxInfo( MINMAXINFO* lpMMI );

// Dialog Data
	//{{AFX_DATA(CListViruses)
	enum { IDD = IDD_LIST_VIRUSES };
	CStatic	m_ctlInfo;
	CButton	m_btnOK;
	CButton	m_btnHelp;
	CButton	m_btnCancel;
	CListCtrl	m_ctlList;
	CString	m_strInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListViruses)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CListViruses)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDeleteitemVirusList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int AddVirusItem( VIRUSNAME* pVirusName );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTVIRUSES_H__1A6741AB_A22E_11D1_8211_00A0C95C0756__INCLUDED_)
