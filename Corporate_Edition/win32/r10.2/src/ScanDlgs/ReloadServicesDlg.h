// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_RELOADSERVICESDLG_H__C05D1E71_DEA5_11D1_B796_00A0C99C7131__INCLUDED_)
#define AFX_RELOADSERVICESDLG_H__C05D1E71_DEA5_11D1_B796_00A0C99C7131__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ReloadServicesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReloadServicesDlg dialog

class CReloadServicesDlg : public CDialog
{
// Construction
public:
	CReloadServicesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReloadServicesDlg)
	enum { IDD = IDD_RELOAD_SERVICES };
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReloadServicesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReloadServicesDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELOADSERVICESDLG_H__C05D1E71_DEA5_11D1_B796_00A0C99C7131__INCLUDED_)
