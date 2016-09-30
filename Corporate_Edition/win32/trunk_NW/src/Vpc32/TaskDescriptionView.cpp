// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: TaskDescriptionView.cpp 
//  Purpose: CTaskDescriptionView Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#include "stdafx.h"
#include "wprotect32.h"
#include "TaskDescriptionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTaskDescriptionView, CScrollView)

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CTaskDescriptionView::CTaskDescriptionView()
{
	m_ptrTaskpad = NULL;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CTaskDescriptionView::~CTaskDescriptionView()
{
	SetScrollSizes( MM_TEXT, CSize(0, 0));
}


BEGIN_MESSAGE_MAP(CTaskDescriptionView, CScrollView)
	//{{AFX_MSG_MAP(CTaskDescriptionView)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnDraw
//----------------------------------------------------------------
void CTaskDescriptionView::OnDraw(CDC* pDC)
{
	CBrush		brushBackground;
	RECT		rect;

	CDocument* pDoc = GetDocument();

	GetClientRect( &rect );
	
	//Set the bottom to the larger of rect.bottom and ptrTask->m_ySize
	if( m_ptrTaskpad )
	{
		if( rect.bottom < m_ptrTaskpad->m_ySize )
		{
			rect.bottom = m_ptrTaskpad->m_ySize;
		}
	}

	//Paint the background grey
	brushBackground.CreateSolidBrush( (COLORREF)GetSysColor( COLOR_BTNFACE ) );
	pDC->FillRect( &rect, &brushBackground );
	
	brushBackground.DeleteObject();

	//Call the DrawTaskWindow method of the Taskpad class
	if( m_ptrTaskpad )
	{
		m_ptrTaskpad->DrawTaskWindow( pDC );

		SetScrollSizes( MM_TEXT, CSize(0, m_ptrTaskpad->m_ySize));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTaskDescriptionView diagnostics

#ifdef _DEBUG
void CTaskDescriptionView::AssertValid() const
{
	// Intentionaly, calling CView::AssertValid rather than CScrollView.
	// When removing a view the m_nMapMode may not be set to a valid mode
	// and cause a benign (but annoying) assert.
	CView::AssertValid();
}

void CTaskDescriptionView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTaskDescriptionView message handlers

//----------------------------------------------------------------
// OnInitialUpdate
//----------------------------------------------------------------
void CTaskDescriptionView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	

	SetScrollSizes( MM_TEXT, CSize(0, 0));

}

//----------------------------------------------------------------
// OnUpdate
//----------------------------------------------------------------
void CTaskDescriptionView::OnUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	SetScrollSizes( MM_TEXT, CSize(0, m_ptrTaskpad->m_ySize));
}


void CTaskDescriptionView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//Try to select the task that was clicked on
	m_ptrTaskpad->PostMessage( UWM_SELECTBYPOINT, (WPARAM)MAKEWPARAM( point.x, point.y ), 0 );
	
	CScrollView::OnLButtonUp(nFlags, point);
}

