// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SelectFolderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShellSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShellSelDlg dialog


CShellSelDlg::CShellSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShellSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShellSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CShellSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShellSelDlg)
	DDX_Control(pDX, IDC_FOLDERS, m_ShellSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShellSelDlg, CDialog)
	//{{AFX_MSG_MAP(CShellSelDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShellSelDlg message handlers

BOOL CShellSelDlg::OnInitDialog() 
{
	TCHAR	*pItems,
			*pFolders;

	CDialog::OnInitDialog();
	
	//Start by loading the Storage
	m_ShellSel.SetStorage( m_ptrStorage );

	//Now, load the configuration if I need to
	if( m_dwLoadSaveFlag & SHELLSEL_LOAD )
	{
		if( m_strItems.IsEmpty() )
			pItems = NULL;
		else
			pItems = (LPSTR)(LPCTSTR)m_strItems;

		if( m_strFolders.IsEmpty() )
			pFolders = NULL;
		else
			pFolders = (LPSTR)(LPCTSTR)m_strFolders;

		m_ShellSel.LoadConfig( m_ptrConfig, pFolders, pItems );
	}

	//Finally, activate ShellSel
	m_ShellSel.SetActivate( 1 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShellSelDlg::OnOK() 
{
	TCHAR	*pItems,
			*pFolders;

	//If we are to store the configuration, do it now
	if( m_dwLoadSaveFlag & SHELLSEL_SAVE )
	{
		if( m_strItems.IsEmpty() )
			pItems = NULL;
		else
			pItems = (LPSTR)(LPCTSTR)m_strItems;

		if( m_strFolders.IsEmpty() )
			pFolders = NULL;
		else
			pFolders = (LPSTR)(LPCTSTR)m_strFolders;

		m_ShellSel.SaveConfig( m_ptrConfig, pFolders, pItems );
	}
	
	CDialog::OnOK();
}
