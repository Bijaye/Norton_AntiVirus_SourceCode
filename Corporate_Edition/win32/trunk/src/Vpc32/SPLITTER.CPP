// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: splitter.cpp
//  Purpose: CSplitter Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "wprotect32.h"
#include "splitter.h"    

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitter

IMPLEMENT_DYNCREATE(CSplitter, CMDIChildWnd)

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CSplitter::CSplitter()
{
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CSplitter::~CSplitter()
{
}

//----------------------------------------------------------------
// OnCreateClient
//----------------------------------------------------------------
BOOL CSplitter::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,
		CSize(10, 10),
		pContext);
}

BEGIN_MESSAGE_MAP(CSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitter message handlers

//----------------------------------------------------------------
// ReplaceView
//----------------------------------------------------------------
BOOL CSplitter::ReplaceView(int row, int col,CRuntimeClass * pViewClass, SIZE size, BOOL bIgnoreSame /* = TRUE */ )
{
	CCreateContext context;
	BOOL bSetActive;
		    
	CView* pOldView = (CView*) GetPane(row,col);
	//If this isn't the right view, return
	if( bIgnoreSame && (pOldView->IsKindOf(pViewClass))==TRUE )
		return FALSE;
					
	// Get pointer to CDocument object so that it can be used in the creation 
	// process of the new view
	CDocument * pDoc= pOldView->GetDocument();
	CView * pActiveView=GetParentFrame()->GetActiveView();
	if (pActiveView==NULL || pActiveView==pOldView)
		bSetActive=TRUE;
	else
		bSetActive=FALSE;

	// set flag so that document will not be deleted when view is destroyed
	pDoc->m_bAutoDelete=FALSE;    
	// Remove existing view 
	pOldView->DestroyWindow();
	// set flag back to default 
	pDoc->m_bAutoDelete=TRUE;

  // Create new view                      
   context.m_pNewViewClass=pViewClass;
   context.m_pCurrentDoc=pDoc;
   context.m_pNewDocTemplate=NULL;
   context.m_pLastView=NULL;
   context.m_pCurrentFrame=NULL;
   
   CreateView(row,col,pViewClass,size, &context);
   
   CView * pNewView= (CView *)GetPane(row,col);

   pNewView->OnInitialUpdate();
   
   if (bSetActive==TRUE)
      GetParentFrame()->SetActiveView(pNewView);

   RecalcLayout(); 
   GetPane(row,col)->SendMessage(WM_PAINT);
   
   return TRUE;
}


/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
