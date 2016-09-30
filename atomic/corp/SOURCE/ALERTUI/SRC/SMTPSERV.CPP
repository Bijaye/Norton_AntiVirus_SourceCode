/////////////////////////////////////////////////////////////////////
// SMTPServ.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/SMTPSERV.CPv   1.1   25 Apr 1997 11:03:36   JBRENNA  $
//
// Description:
//
//  This file contains the dialog that allows a user to configure the
//  SMTP server.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/SMTPSERV.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:03:36   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:22   RFULLER
// Initial revision
// 
//    Rev 1.1   25 Nov 1996 15:46:32   SEDWARD
// Major overhaul (more to come).
// 
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "SMTPServ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMTPServerDlg dialog


CSMTPServerDlg::CSMTPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSMTPServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSMTPServerDlg)
	m_smtpServerStr = _T("");
	//}}AFX_DATA_INIT
}


void CSMTPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSMTPServerDlg)
	DDX_Text(pDX, IDC_SMTP_SERVER, m_smtpServerStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSMTPServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSMTPServerDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMTPServerDlg message handlers



// ===== OnInitDialog =====================================================
//
//  Input:  nothing
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CSMTPServerDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

    m_smtpServerStr = gAlertOptions.GetSMTPPostOffice();
    UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
