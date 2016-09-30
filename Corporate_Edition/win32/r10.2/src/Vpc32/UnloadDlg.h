// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_UNLOADDLG_H__3FF6B939_65D3_11D1_81AF_00A0C95C0756__INCLUDED_)
#define AFX_UNLOADDLG_H__3FF6B939_65D3_11D1_81AF_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// UnloadDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnloadDlg dialog

class CUnloadDlg : public CDialog
{
// Construction
public:
	CUnloadDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnloadDlg)
	enum { IDD = IDD_UNLOAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnloadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUnloadDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNLOADDLG_H__3FF6B939_65D3_11D1_81AF_00A0C95C0756__INCLUDED_)
