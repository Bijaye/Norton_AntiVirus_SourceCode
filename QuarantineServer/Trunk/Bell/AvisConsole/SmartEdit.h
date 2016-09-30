/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_SMARTEDIT_H__B13C40E2_B677_421D_9B50_8D46E8298874__INCLUDED_)
#define AFX_SMARTEDIT_H__B13C40E2_B677_421D_9B50_8D46E8298874__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SmartEdit.h : header file
//
// above the range for normal EN_ messages
#define PEN_ILLEGALCHAR     0x8000
#define WM_EDITCLASSMSG    WM_APP + 100

#include "acstatelistctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CSmartEdit window

class CSmartEdit : public CEdit
{
// Construction
public:
	CSmartEdit();

// Attributes
public:
	BOOL m_bEscapeKey;
	BOOL m_bReturnKey;
	CEventObject *m_pEventObject;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartEdit)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	virtual void OnBadInput();

// Implementation
public:
	virtual ~CSmartEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSmartEdit)
	afx_msg void OnKillfocus();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMARTEDIT_H__B13C40E2_B677_421D_9B50_8D46E8298874__INCLUDED_)
