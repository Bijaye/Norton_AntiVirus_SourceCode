////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// apwUtilStressView.cpp : implementation of the CapwUtilStressView class
//

#include "stdafx.h"
#include "apwUtilStress.h"

#include "apwUtilStressDoc.h"
#include "apwUtilStressView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CapwUtilStressView

IMPLEMENT_DYNCREATE(CapwUtilStressView, CView)

BEGIN_MESSAGE_MAP(CapwUtilStressView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CapwUtilStressView construction/destruction

CapwUtilStressView::CapwUtilStressView()
{
	// TODO: add construction code here

}

CapwUtilStressView::~CapwUtilStressView()
{
}

BOOL CapwUtilStressView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CapwUtilStressView drawing

void CapwUtilStressView::OnDraw(CDC* /*pDC*/)
{
	CapwUtilStressDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CapwUtilStressView printing

BOOL CapwUtilStressView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CapwUtilStressView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CapwUtilStressView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CapwUtilStressView diagnostics

#ifdef _DEBUG
void CapwUtilStressView::AssertValid() const
{
	CView::AssertValid();
}

void CapwUtilStressView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CapwUtilStressDoc* CapwUtilStressView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CapwUtilStressDoc)));
	return (CapwUtilStressDoc*)m_pDocument;
}
#endif //_DEBUG


// CapwUtilStressView message handlers
