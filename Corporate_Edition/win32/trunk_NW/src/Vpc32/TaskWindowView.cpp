// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// TaskWindowView.cpp : implementation file
//

#include "stdafx.h"
#include "wprotect32.h"
#include "TaskWindowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskWindowView

IMPLEMENT_DYNCREATE(CTaskWindowView, CView)

CTaskWindowView::CTaskWindowView()
{
}

CTaskWindowView::~CTaskWindowView()
{
}


BEGIN_MESSAGE_MAP(CTaskWindowView, CView)
	//{{AFX_MSG_MAP(CTaskWindowView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskWindowView drawing

void CTaskWindowView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CTaskWindowView diagnostics

#ifdef _DEBUG
void CTaskWindowView::AssertValid() const
{
	CView::AssertValid();
}

void CTaskWindowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTaskWindowView message handlers
