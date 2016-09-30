// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PreparingScan.h : header file
//

#define UM_SETFILES WM_USER + 234

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CPreparingScan dialog

class CPreparingScan : public CDialog
{
// Construction
public:
	CPreparingScan(CWnd* pParent = NULL);   // standard constructor

	LRESULT OnUpdateCount( WPARAM, LPARAM );

// Dialog Data
	//{{AFX_DATA(CPreparingScan)
	enum { IDD = IDD_PREPARING_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreparingScan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreparingScan)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
