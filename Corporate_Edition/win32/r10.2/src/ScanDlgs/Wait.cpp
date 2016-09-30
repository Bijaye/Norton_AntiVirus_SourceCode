// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Wait.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "Wait.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWait dialog


CWait::CWait(CWnd* pParent /*=NULL*/)
	: CDialog(CWait::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWait)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	Create( IDD, NULL );
}


void CWait::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWait)
	DDX_Control(pDX, IDC_WORKING_AVI, m_aviWork);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWait, CDialog)
	//{{AFX_MSG_MAP(CWait)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWait message handlers

BOOL CWait::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_aviWork.Open( IDR_AVI2 );
	m_aviWork.Play( 0, -1, -1 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
