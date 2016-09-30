// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PreparingScan.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "PreparingScan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreparingScan dialog


CPreparingScan::CPreparingScan(CWnd* pParent /*=NULL*/)
	: CDialog(CPreparingScan::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreparingScan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPreparingScan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreparingScan)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreparingScan, CDialog)
	//{{AFX_MSG_MAP(CPreparingScan)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_MESSAGE( UM_SETFILES, OnUpdateCount )	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreparingScan message handlers

LRESULT CPreparingScan::OnUpdateCount( WPARAM wParam, LPARAM lParam)
{
	WORD	wFiles = LOWORD( lParam ),
			wDirs = HIWORD( lParam );
	CWnd	*ptrFiles = GetDlgItem( IDC_FILES ),
			*ptrDirs = GetDlgItem( IDC_FOLDERS );
	CString strTemp;

	strTemp.Format( "%d", wFiles );
	ptrFiles->SetWindowText( strTemp );

	strTemp.Format( "%d", wDirs );
	ptrDirs->SetWindowText( strTemp );

	return 1;
}
