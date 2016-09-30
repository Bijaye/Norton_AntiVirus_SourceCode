// ResultsActionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScanDlgs.h"
#include "ResultsActionDlg.h"


// CResultsActionDlg dialog

IMPLEMENT_DYNAMIC(CResultsActionDlg, CDialog)
CResultsActionDlg::CResultsActionDlg(EAction eAction,
									 CWnd* pParent /*=NULL*/)
  :	CDialog(CResultsActionDlg::IDD, pParent),
	m_eActionRequired(eAction),
	m_eActionChosen(evInvalidAction)
{
	ASSERT(m_eActionRequired >= evFirstValidAction &&
		   m_eActionRequired < evAfterLastRealAction);
}


CResultsActionDlg::~CResultsActionDlg()
{
}


void CResultsActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResultsActionDlg, CDialog)
	ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
END_MESSAGE_MAP()


// CResultsActionDlg message handlers

BOOL CResultsActionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the caption.
	CString strCaption;

	switch (m_eActionRequired)
	{
	case evRemoveRisks:
		strCaption.LoadString(IDS_CAP_REMOVE_RISK);
		break;
	case evReboot:
		strCaption.LoadString(IDS_CAP_REBOOT);
		break;
	case evRemoveRisksAndReboot:
		strCaption.LoadString(IDS_CAP_REMOVE_RISK_AND_REBOOT);
		break;
	}
	SetWindowText(strCaption);
	// Set the message.
	CString strMessage;

	switch (m_eActionRequired)
	{
	case evRemoveRisks:
		strMessage.LoadString(IDS_MSG_REMOVE_RISK);
		break;
	case evReboot:
		strMessage.LoadString(IDS_MSG_REBOOT);
		break;
	case evRemoveRisksAndReboot:
		strMessage.LoadString(IDS_MSG_REMOVE_RISK_AND_REBOOT);
		break;
	}
	GetDlgItem(IDC_MESSAGE)->SetWindowText(strMessage);
	// Set up radio buttons.
	CString strRadio1Text;
	CString strRadio2Text;
	CString strRadio3Text; // Put text here to make this 3rd button visible.

	switch (m_eActionRequired)
	{
	case evRemoveRisks:
		strRadio1Text.LoadString(IDS_RADIO_REMOVE_RISKS);
		strRadio2Text.LoadString(IDS_RADIO_NO_REMOVE_RISKS);
		break;
	case evReboot:
		strRadio1Text.LoadString(IDS_RADIO_REBOOT);
		strRadio2Text.LoadString(IDS_RADIO_NO_REBOOT);
		break;
	case evRemoveRisksAndReboot: // Uses 3 radio buttons.
		strRadio1Text.LoadString(IDS_RADIO_REMOVE_AND_REBOOT);
		strRadio2Text.LoadString(IDS_RADIO_REMOVE_NO_REBOOT);
		strRadio3Text.LoadString(IDS_RADIO_NO_REMOVE_OR_REBOOT);
		break;
	}
	GetDlgItem(IDC_RADIO1)->SetWindowText(strRadio1Text);
	GetDlgItem(IDC_RADIO2)->SetWindowText(strRadio2Text);
	// If we put text in the 3rd button, make it visible.
	if (!strRadio3Text.IsEmpty())
	{
		CWnd *pwndRadio3 = GetDlgItem(IDC_RADIO3);

		pwndRadio3->ShowWindow(SW_SHOW);
		pwndRadio3->SetWindowText(strRadio3Text);
	}
	// Select the 1st radio button by default
	//	   because it corresponds to the required actions.
	ASSERT(GetActionForRadioButton(IDC_RADIO1) == m_eActionRequired);
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


CResultsActionDlg::EAction
	CResultsActionDlg::GetActionForRadioButton(UINT idcButton) const
{
	switch (idcButton)
	{
	case IDC_RADIO1:
		// The first radio button should always correspond
		//	   to all the required actions.
		return m_eActionRequired;
	case IDC_RADIO2:
		switch (m_eActionRequired)
		{
		case evRemoveRisks:
			return evNoAction;
		case evReboot:
			return evNoAction;
		case evRemoveRisksAndReboot:
			return evRemoveRisks;
		}
		break;
	case IDC_RADIO3:
		if (evRemoveRisksAndReboot == m_eActionRequired)
			return evNoAction;
		break;
	}
	// idcButton or m_eActionRequired are invalid.s
	ASSERT(false);
	return evInvalidAction;
}


void CResultsActionDlg::OnOK()
{
	// Set the action chosen.
	m_eActionChosen = GetActionForRadioButton(
						  GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3));

	CDialog::OnOK();
}


void CResultsActionDlg::OnCancel()
{
	// The chosen action is no action.
	m_eActionChosen = evNoAction;

	CDialog::OnCancel();
}


CResultsActionDlg::EAction CResultsActionDlg::GetChosenAction() const
{
	return m_eActionChosen;
}

void CResultsActionDlg::OnBnClickedHelp()
{
	AfxGetApp()->WinHelpInternal(IDD);
}
