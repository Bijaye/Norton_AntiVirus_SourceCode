// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ConfigHistory.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "ConfigHistory.h"
#include "ClientReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define szLogFrequency "LogFrequency"

/////////////////////////////////////////////////////////////////////////////
// CConfigHistory dialog


CConfigHistory::CConfigHistory(CWnd* pParent /*=NULL*/, bool bShowApplyAll /*=false*/, bool bReadOnly /*=false*/)
: CDialog(CConfigHistory::IDD, pParent), m_pConfig(NULL), m_bShowApplyAll(bShowApplyAll), m_bReadOnly(bReadOnly)
{
	//{{AFX_DATA_INIT(CConfigHistory)
	//}}AFX_DATA_INIT

	m_bManualChange = FALSE;
	m_bApplyToClients = true; // always start with true!
	m_pConfig2=NULL;
}


void CConfigHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigHistory)
	DDX_Control(pDX, IDC_HO_APPLYTOCLIENTS, m_ctlApplyToClients);
	DDX_Control(pDX, IDC_SPIN2, m_ctlSpin);
	DDX_Control(pDX, IDC_VALUE, m_ctlEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigHistory, CDialog)
	//{{AFX_MSG_MAP(CConfigHistory)
	ON_EN_KILLFOCUS(IDC_VALUE, OnKillfocusValue)
	ON_BN_CLICKED(CONFIG_HISTORY_HELP, OnHistoryHelp)
	ON_BN_CLICKED(IDC_HO_APPLYTOCLIENTS, OnApplyToClients)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigHistory message handlers

/*void CConfigHistory::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CWnd *pWin = GetDlgItem(IDC_VALUE);
	CString sText;

	// the the text to the new position
	if (pNMUpDown->iDelta != pNMUpDown->iPos)
	{
		sText.Format("%d",pNMUpDown->iPos + pNMUpDown->iDelta);
		pWin->SetWindowText(sText);
	}

}
*/

BOOL CConfigHistory::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox* pType = (CComboBox*)GetDlgItem(IDC_TYPE);
//	CEdit *pNum = (CEdit*)GetDlgItem(IDC_VALUE);
//	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2);
	HRESULT hr;

	//Add extra initialization here

	// set the spin control
//	pSpin->SetBuddy(pNum);

	// populte the frequency type
	CString sText;
	sText.LoadString(IDS_LOG_DAYS);
	pType->AddString(sText);
	sText.LoadString(IDS_LOG_MONTHS);
	pType->AddString(sText);
	sText.LoadString(IDS_LOG_YEARS);
	pType->AddString(sText);
	

	// set the frequencly value from the registry
	long val=30;
	long ifreq=0;
	double dval=0;
	
	if( m_pGenConfig )
		hr = m_pGenConfig->CollectData();
	if (m_pConfig2) 
		hr = m_pConfig2->CollectData();

	hr = m_pConfig->GetIntOption(szReg_Val_LogRollOverDays,&val,30);
	hr = m_pConfig->GetIntOption(szLogFrequency,&ifreq,0);

	// convert frequency to days
	if (ifreq==1)
	{
		dval = ((double)val/30.4375); // convert months
		
	}
	// convert frequncy to years
	else if (ifreq==2)
	{
		dval = ((double)val/365.25);  // convert years
	}
	// frequency is alreay in days 
	else
		dval = val;

	pType->SetCurSel(ifreq);

	// round the number
	if (dval - long(dval) > 0.5) 
		val = (long)dval+1;
	else
		val = (long)dval;
		
	
	sText.Format("%d",val);
//	pNum->LimitText(4);
//	pNum->SetWindowText(sText);
//	pSpin->SetPos(val);
//	pSpin->SetRange(1,9999);
	m_ctlEdit.LimitText(4);
	m_ctlEdit.SetWindowText(sText);
	m_ctlSpin.SetRange(1,9999);
	m_ctlSpin.SetPos(val);


	if (m_bShowApplyAll)
		{
		m_bApplyToClients = true;
		m_ctlApplyToClients.SetCheck(1);
		m_ctlApplyToClients.EnableWindow(!m_bReadOnly);
		}
	else
		{
		m_bApplyToClients = true;
		m_ctlApplyToClients.SetCheck(1);
		m_ctlApplyToClients.EnableWindow(FALSE);
		m_ctlApplyToClients.ShowWindow(SW_HIDE);
		}

	m_ctlSpin.EnableWindow(!m_bReadOnly);
	m_ctlEdit.EnableWindow(!m_bReadOnly);
	m_ctlSpin.EnableWindow(!m_bReadOnly);
	pType->EnableWindow(!m_bReadOnly);
    GetDlgItem(IDOK)->EnableWindow(!m_bReadOnly);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*
void CConfigHistory::OnUpdateValue() 
{
	CString sText;

	// update spin control position if user manually changes text
	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2);
	CWnd *pWin = GetDlgItem(IDC_VALUE);
	pWin->GetWindowText(sText);
	pSpin->SetPos(atol(sText));
}
*/

void CConfigHistory::OnOK() 
{
	if (!m_bReadOnly)
		{
		//get frequency type
		CComboBox* pType = (CComboBox*)GetDlgItem(IDC_TYPE);
		// get frequency value
		CWnd *pNum = (CEdit*)GetDlgItem(IDC_VALUE);
		CString sText;
		
		pNum->GetWindowText(sText);
		DWORD dwDays = atol(sText);
		DWORD ifreq= pType->GetCurSel();

		// convert frequency to days
		if (ifreq==1)
			dwDays = (DWORD)((double)dwDays*30.4375); // convert months
		else if (ifreq==2)
			dwDays = (DWORD)((double)dwDays*365.25);  // convert years
		//else frequency is already in day no need to convert


		// save out to option
		m_pConfig->SetOption(szReg_Val_LogRollOverDays,REG_DWORD,(BYTE*)&dwDays,sizeof(DWORD));
		m_pConfig->SetOption(szLogFrequency,REG_DWORD,(BYTE*)&ifreq,sizeof(DWORD));

		if (m_pConfig2 && m_bApplyToClients) 
		{
			m_pConfig2->SetOption(szReg_Val_LogRollOverDays,REG_DWORD,(BYTE*)&dwDays,sizeof(DWORD));
			m_pConfig2->SetOption(szLogFrequency,REG_DWORD,(BYTE*)&ifreq,sizeof(DWORD));
		}

		if( m_pGenConfig )
			m_pGenConfig->WriteData(FALSE);
		if (m_pConfig2 && m_bApplyToClients)
			m_pConfig2->WriteData(FALSE);
		}
	
	CDialog::OnOK();
}

void CConfigHistory::OnKillfocusValue() 
{
	m_ctlSpin.SetPos(LOWORD( m_ctlSpin.GetPos() ));
}

void CConfigHistory::OnHistoryHelp() 
{
	AfxGetApp()->WinHelpInternal( IDD );
}

void CConfigHistory::OnApplyToClients() 
{
	m_bApplyToClients = m_ctlApplyToClients.GetCheck() ? true : false;
}
