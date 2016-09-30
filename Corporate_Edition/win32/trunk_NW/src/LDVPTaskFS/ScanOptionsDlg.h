// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//{{AFX_INCLUDES()
#include "filesystemui.h"
//}}AFX_INCLUDES
#if !defined(AFX_SCANOPTIONSDLG_H__2E76B2B1_C603_11D1_826C_00A0C95C0756__INCLUDED_)
#define AFX_SCANOPTIONSDLG_H__2E76B2B1_C603_11D1_826C_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScanOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScanOptionsDlg dialog

#include "Resource.h"

class CScanOptionsDlg : public CDialog
{
private:
	IConfig	*m_ptrConfig;

// Construction
public:
	CScanOptionsDlg(IConfig *ptrConfig = NULL, CWnd* pParent = NULL);   // standard constructor
	~CScanOptionsDlg();

// Dialog Data
	//{{AFX_DATA(CScanOptionsDlg)
	enum { IDD = IDD_SCAN_OPTIONS };
	CFileSystemUI	m_ctlUI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanOptionsDlg)
	public:
	virtual void WinHelpInternal(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
public:
	// modal processing
	virtual INT_PTR DoModal();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScanOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMakeDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANOPTIONSDLG_H__2E76B2B1_C603_11D1_826C_00A0C95C0756__INCLUDED_)
