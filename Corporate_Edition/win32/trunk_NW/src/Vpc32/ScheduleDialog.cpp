// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScheduleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ScheduleDialog.h"
#include "ConfigObj.h"
#include "ClientReg.h"
//#include "ScheduleOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScheduleDialog dialog


CScheduleDialog::CScheduleDialog(IConfig *pConfig, CWnd* pParent /*=NULL*/)
	: CDialog(CScheduleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScheduleDialog)
	//}}AFX_DATA_INIT


	if( pConfig )
		(m_ptrConfig = pConfig)->AddRef();
	else
		m_ptrConfig	= NULL;

//	m_strTitle = strTitle;
}

CScheduleDialog::~CScheduleDialog()
{
	if( m_ptrConfig  )
		m_ptrConfig->Release();
}



void CScheduleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScheduleDialog)
	DDX_Control(pDX, IDC_SCHEDULE, m_ctlSchedule);
//	DDX_Control(pDX, IDC_STORAGE_VIEW, m_ctlStorageView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleDialog, CDialog)
	//{{AFX_MSG_MAP(CScheduleDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CScheduleDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CScheduleDialog)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScheduleDialog message handlers

BOOL CScheduleDialog::OnInitDialog() 
{	
    CDialog::OnInitDialog();
	
	m_ctlSchedule.SetScheduleType(1); // 1 = update, 0 = scan
	m_ctlSchedule.Load( m_ptrConfig );
	

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CScheduleDialog::OnOK() 
{
	m_ctlSchedule.Store( m_ptrConfig );

	CDialog::OnOK();
}

