// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: WProtect32View.cpp 
//  Purpose: CWProtect32View Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "WProtect32.h"

#include "WProtect32Doc.h"
#include "WProtect32View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWProtect32View, CView)

BEGIN_MESSAGE_MAP(CWProtect32View, CView)
	//{{AFX_MSG_MAP(CWProtect32View)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CWProtect32View::CWProtect32View()
{
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CWProtect32View::~CWProtect32View()
{
}

//----------------------------------------------------------------
// PrecreateWindow
//----------------------------------------------------------------
BOOL CWProtect32View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

//----------------------------------------------------------------
// OnDraw
//----------------------------------------------------------------
void CWProtect32View::OnDraw(CDC* pDC)
{
	CWProtect32Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

//----------------------------------------------------------------
// OnPreparePrinting
//----------------------------------------------------------------
BOOL CWProtect32View::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

//----------------------------------------------------------------
// OnBeginPrinting
//----------------------------------------------------------------
void CWProtect32View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

//----------------------------------------------------------------
// OnEndPrinting
//----------------------------------------------------------------
void CWProtect32View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CWProtect32View diagnostics

#ifdef _DEBUG
void CWProtect32View::AssertValid() const
{
	CView::AssertValid();
}

void CWProtect32View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWProtect32Doc* CWProtect32View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWProtect32Doc)));
	return (CWProtect32Doc*)m_pDocument;
}
#endif //_DEBUG
