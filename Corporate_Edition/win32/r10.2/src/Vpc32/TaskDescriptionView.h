// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskDescriptionView.h
//  Purpose: CTaskDescriptionView Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#include "TaskPad.h"

//----------------------------------------------------------------
//
// CTaskDescriptionView class
//
//----------------------------------------------------------------
class CTaskDescriptionView : public CScrollView
{

private:
	CTaskpad *m_ptrTaskpad;

protected:
	CTaskDescriptionView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTaskDescriptionView)

// Attributes
public:
	void SetTaskPadPtr( CTaskpad *ptrTaskPad ){ m_ptrTaskpad = ptrTaskPad; };

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskDescriptionView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTaskDescriptionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTaskDescriptionView)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
