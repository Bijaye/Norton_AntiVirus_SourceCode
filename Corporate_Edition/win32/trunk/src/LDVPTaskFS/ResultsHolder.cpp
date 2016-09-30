// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ResultsHolder.cpp
//  Purpose: CResultsHolder Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "ResultsHolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CResultsHolder::CResultsHolder()
{
	m_hChildWnd = NULL;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CResultsHolder::~CResultsHolder()
{
}


BEGIN_MESSAGE_MAP(CResultsHolder, CStatic)
	//{{AFX_MSG_MAP(CResultsHolder)
	ON_WM_SIZE()
	ON_MESSAGE(UWM_CLOSING, OnResultsDlgClosing )
	ON_MESSAGE(UWM_ADDCHILD, OnCreateScanDlg )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultsHolder message handlers

//----------------------------------------------------------------
// OnSize
//----------------------------------------------------------------
void CResultsHolder::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	if( m_hChildWnd )
		CWnd::FromHandle( m_hChildWnd )->SetWindowPos( &wndTop, 0, 0, cx, cy, SWP_SHOWWINDOW );
}

//----------------------------------------------------------------
// OnCreateScanDlg
//----------------------------------------------------------------
long CResultsHolder::OnCreateScanDlg( WPARAM wParam, LPARAM )
{
	m_hChildWnd = (HWND)wParam;

	//Now, size the dialog to my size
	if( m_hChildWnd )
	{
		CRect rect;

		GetClientRect( &rect );

		CWnd::FromHandle( m_hChildWnd )->SetWindowPos( &wndTop, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
	}

	return 1;
}

//----------------------------------------------------------------
// OnResultsDlgClosing
//----------------------------------------------------------------
long CResultsHolder::OnResultsDlgClosing( WPARAM, LPARAM )
{
	m_hChildWnd = NULL;

	//Forward this message on to my parent
	GetParent()->SendMessage( UWM_CLOSING, 0, 0 );

	return -1;
}
