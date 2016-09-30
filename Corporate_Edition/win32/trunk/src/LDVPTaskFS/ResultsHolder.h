// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ResultsHolder.h
//  Purpose: CResultsHolder Definition file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined(AFX_RESULTSHOLDER_H__60EAEDDB_886A_11D1_81E7_00A0C95C0756__INCLUDED_)
#define AFX_RESULTSHOLDER_H__60EAEDDB_886A_11D1_81E7_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Ldvptaskfs.h"

//----------------------------------------------------------------
//
// CResultsHolder
//
//----------------------------------------------------------------
class CResultsHolder : public CStatic
{
private:
	HWND	m_hChildWnd;

	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM, LPARAM );

// Construction
public:
	CResultsHolder();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultsHolder)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResultsHolder();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResultsHolder)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTSHOLDER_H__60EAEDDB_886A_11D1_81E7_00A0C95C0756__INCLUDED_)
