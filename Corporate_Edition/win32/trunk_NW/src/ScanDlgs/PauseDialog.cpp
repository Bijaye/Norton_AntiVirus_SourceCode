// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PauseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "PauseDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPauseDialog dialog


CPauseDialog::CPauseDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPauseDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPauseDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bStopEnabled = FALSE;
}


void CPauseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPauseDialog)
	DDX_Control(pDX, IDC_PAUSE_DELAY_4HOUR_BUTTON, m_ctl4HourSnooze);
	DDX_Control(pDX, IDC_PAUSE_STOP_BUTTON, m_ctlStopButton);
	DDX_Control(pDX, IDC_PAUSE_DELAY_COUNT_TEXT, m_ctlDelayCountText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPauseDialog, CDialog)
	//{{AFX_MSG_MAP(CPauseDialog)
	ON_BN_CLICKED(IDC_PAUSE_CONTINUE_BUTTON, OnPauseContinue)
	ON_BN_CLICKED(IDC_PAUSE_DELAY_1HOUR_BUTTON, OnPauseDelay1Hour)
	ON_BN_CLICKED(IDC_PAUSE_DELAY_4HOUR_BUTTON, OnPauseDelay4Hour)
	ON_BN_CLICKED(IDC_PAUSE_PAUSE_BUTTON, OnPausePause)
	ON_BN_CLICKED(IDC_PAUSE_STOP_BUTTON, OnPauseStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPauseDialog message handlers

BOOL CPauseDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (strDelayCountText.GetLength())
		m_ctlDelayCountText.SetWindowText(strDelayCountText);

	m_ctlStopButton.EnableWindow(m_bStopEnabled);

	m_ctl4HourSnooze.EnableWindow(m_b4HourEnabled);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPauseDialog::OnPauseContinue() 
{
	CDialog::EndDialog(IDC_PAUSE_CONTINUE_BUTTON);
}

void CPauseDialog::OnPauseDelay1Hour() 
{
	CDialog::EndDialog(IDC_PAUSE_DELAY_1HOUR_BUTTON);
}

void CPauseDialog::OnPauseDelay4Hour() 
{
	CDialog::EndDialog(IDC_PAUSE_DELAY_4HOUR_BUTTON);
}

void CPauseDialog::OnPausePause() 
{
	CDialog::EndDialog(IDC_PAUSE_PAUSE_BUTTON);
}

void CPauseDialog::OnPauseStop() 
{
	CDialog::EndDialog(IDC_PAUSE_STOP_BUTTON);
}
