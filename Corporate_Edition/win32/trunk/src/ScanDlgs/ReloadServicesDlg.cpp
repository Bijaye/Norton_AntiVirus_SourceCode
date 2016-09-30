// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ReloadServicesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "ReloadServicesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReloadServicesDlg dialog


CReloadServicesDlg::CReloadServicesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReloadServicesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReloadServicesDlg)
	m_text = _T("");
	//}}AFX_DATA_INIT
}


void CReloadServicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReloadServicesDlg)
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReloadServicesDlg, CDialog)
	//{{AFX_MSG_MAP(CReloadServicesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReloadServicesDlg message handlers
