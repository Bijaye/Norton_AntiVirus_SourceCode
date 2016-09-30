// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Message.cpp : implementation file
//

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "Message.h"
#include "LdvpEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessage dialog


CMessage::CMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CMessage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessage)
	m_bMessageBeep = FALSE;
	m_strMessage = __T("");
	//}}AFX_DATA_INIT
}


void CMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessage)
	DDX_Check(pDX, IDC_MESSAGE_BEEP, m_bMessageBeep);
	DDX_OCText(pDX, IDC_MESSAGE, DISPID(-517), m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessage, CDialog)
	//{{AFX_MSG_MAP(CMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessage message handlers

BOOL CMessage::OnInitDialog() 
{
	DWORD	dwID;
	int		iLock;

	CDialog::OnInitDialog();
	
	//Check locks & create them if needed
	dwID = ( m_bMessageLocked ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;
	iLock = ( m_bMessageLocked ) ? 1 : 0;

	m_MessageLock.Create( WS_VISIBLE | WS_CHILD, IDC_MESSAGE, this, IDC_MESSAGE_LOCK, dwID );
	m_MessageLock.SetLockStyle( 0, LOCK_STYLE_USER );
	m_MessageLock.AddBuddy( IDC_MESSAGE );
	m_MessageLock.Lock( iLock );


	dwID = ( m_bMessageBeepLocked ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;
	iLock = ( m_bMessageBeepLocked ) ? 1 : 0;

	m_MessageBeepLock.Create( WS_VISIBLE | WS_CHILD, IDC_MESSAGE_BEEP, this, IDC_MESSAGE_BEEP_LOCK, dwID );
	m_MessageBeepLock.SetLockStyle( 0, LOCK_STYLE_USER );
	m_MessageBeepLock.AddBuddy( IDC_MESSAGE_BEEP );
	m_MessageBeepLock.Lock( iLock );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
