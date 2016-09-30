// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_WAIT_H__E51DB8A0_783F_11D4_80C9_00C04F05837B__INCLUDED_)
#define AFX_WAIT_H__E51DB8A0_783F_11D4_80C9_00C04F05837B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Wait.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWait dialog

class CWait : public CDialog
{
// Construction
public:
	CWait(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWait)
	enum { IDD = IDD_WAIT };
	CAnimateCtrl	m_aviWork;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWait)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWait)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAIT_H__E51DB8A0_783F_11D4_80C9_00C04F05837B__INCLUDED_)
