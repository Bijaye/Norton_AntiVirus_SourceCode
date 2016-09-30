// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//********************************************************************
// $Header:   S:/PRIVMAN/VCS/STATDLG.CPv   1.0   25 Jun 1997 16:07:16   DSACKIN  $
//
//  Description:        Status dialog to display what is going on.
//
//********************************************************************
//  $Log:   S:/PRIVMAN/VCS/STATDLG.CPv  $
// 
//    Rev 1.0   25 Jun 1997 16:07:16   DSACKIN
// Initial revision.
// 
//    Rev 1.0   08 May 1997 10:12:40   pvenkat
// Initial revision.
//********************************************************************

// StatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrivMAN.h"
#include "StatDlg.h"
#include "PManDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg dialog


CStatusDlg::CStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatusDlg)
	m_strMsg = _T("");
	//}}AFX_DATA_INIT
}


void CStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusDlg)
	DDX_Text(pDX, IDC_MSG, m_strMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CStatusDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg message handlers

VOID CStatusDlg::SetMsg (LPTSTR lpszOp, LPTSTR lpszUser, LPTSTR lpszMachine)
{
	CString	strMsg;
	TCHAR	szFormat[MAX_PATH + 1];

//	strMsg.LoadString (IDS_MSG);
    // Get the format
	LoadString (AfxGetResourceHandle (), IDS_MSG, szFormat, sizeof (szFormat));

    // Put the strings
	strMsg.Format (szFormat, lpszOp, lpszUser, lpszMachine);

    // Update the mapped variable.
	m_strMsg = strMsg;

    // Set it on control.
	UpdateData (FALSE);
}

BOOL CStatusDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // Center the dialog.	
	CenterWindow ();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
