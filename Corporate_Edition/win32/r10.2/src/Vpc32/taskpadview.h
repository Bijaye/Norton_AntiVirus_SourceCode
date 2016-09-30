// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskpadView.h
//  Purpose: CTaskpadView Definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined (TASK_PAD_VIEW_INCLUDED)
#define TASK_PAD_VIEW_INCLUDED

#ifndef TASKPAD
#include "taskpad.h"
#include "WProtect32doc.h"
#endif

class CVPC32Doc;

//----------------------------------------------------------------
//
// CTaskPadView class
//
//----------------------------------------------------------------
class CTaskpadView : public CView
{
private:
	CSubTask	*ptrTask;
	CTaskGroup	*ptrGroup;
	CSubTask	*m_ptrCut;
	CString		m_strCutTaskname;

	void		DeleteCut();

protected: // create from serialization only
	CTaskpadView();
	DECLARE_DYNCREATE(CTaskpadView)

// Attributes
public:
	void FocusToRoot();
	CWProtect32Doc* GetDocument();


	BOOL		CanDeleteSelTask();
	BOOL		CanPasteCurGroup();
	void		DeleteSelTask();
	CTaskpad	m_taskpad;
	void		Cut();
	void		Paste();
	BOOL		CanCut();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskpadView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTaskpadView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CTaskpadView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCut();
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnPaste();
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TaskpadView.cpp
inline CWProtect32Doc* CTaskpadView::GetDocument()
   { return (CWProtect32Doc*)m_pDocument; }
#endif


#endif
