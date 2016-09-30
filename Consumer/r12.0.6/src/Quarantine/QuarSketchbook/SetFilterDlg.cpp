// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// SetFilterDlg.cpp : implementation file
//

#include "stdafx.h"

#include "sketchbook.h"
#include "SetFilterDlg.h"

// SetFilterDlg dialog

IMPLEMENT_DYNAMIC(SetFilterDlg, CDialog)
SetFilterDlg::SetFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetFilterDlg::IDD, pParent)
{
	m_chkFile = false;
	m_chkRegistry = false;
	m_chkProcess = false;
	m_chkService = false;
	m_chkBatch = false;
	m_chkINI = false;
	m_chkStartup = false;
	m_chkCOM = false;
	
	m_chkViral = false;
	m_chkSpyWare = false;
	m_chkGenericLoadpoint = false;
	m_chkClientCompliancy = false;
	m_chkReservedMalicious = false;
	m_chkHeuristic = false;
	m_chkSecurityRisk = false;
	m_chkHacktool = false;
	m_chkTrackware = false;
	m_chkDialer = false;
	m_chkRemoteAccess = false;
	m_chkAdware = false;
	m_chkJoke = false;
	m_chkMalicious = false;

	m_bOutFilter = FALSE;
}

SetFilterDlg::~SetFilterDlg()
{
}

BOOL SetFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_bOutFilter)
	{
		CWnd * wndCtrl;
		wndCtrl = GetDlgItem(IDC_FILE);
		wndCtrl->EnableWindow(FALSE);
		wndCtrl = GetDlgItem(IDC_REGISTRY);
		wndCtrl->EnableWindow(FALSE);
		wndCtrl = GetDlgItem(IDC_PROCESS);
		wndCtrl->EnableWindow(FALSE);
		wndCtrl = GetDlgItem(IDC_BATCH);
		wndCtrl->EnableWindow(FALSE);
		wndCtrl = GetDlgItem(IDC_COM);
		wndCtrl->EnableWindow(FALSE);
		wndCtrl = GetDlgItem(IDC_INI);
		wndCtrl->EnableWindow(FALSE);
		wndCtrl = GetDlgItem(IDC_SERVICE);
		wndCtrl->EnableWindow(FALSE);
		wndCtrl = GetDlgItem(IDC_STARTUP);
		wndCtrl->EnableWindow(FALSE);
	}

	return TRUE;
}

void SetFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	BOOL chkFile = m_chkFile ? TRUE : FALSE;
	BOOL chkRegistry = m_chkRegistry ? TRUE : FALSE;
	BOOL chkProcess = m_chkProcess ? TRUE : FALSE;
	BOOL chkService = m_chkService ? TRUE : FALSE;
	BOOL chkBatch = m_chkBatch ? TRUE : FALSE;
	BOOL chkINI = m_chkINI ? TRUE : FALSE;
	BOOL chkStartup = m_chkStartup ? TRUE : FALSE;
	BOOL chkCOM = m_chkCOM ? TRUE : FALSE;

	BOOL chkViral = m_chkViral ? TRUE : FALSE;
	BOOL chkMalicious = m_chkMalicious ? TRUE : FALSE;
	BOOL chkReservedMalicious = m_chkReservedMalicious ? TRUE : FALSE;
	BOOL chkHeuristic = m_chkHeuristic ? TRUE : FALSE;
	BOOL chkSecurityRisk = m_chkSecurityRisk ? TRUE : FALSE;
	BOOL chkHacktool = m_chkHacktool ? TRUE : FALSE;
	BOOL chkSpyWare = m_chkSpyWare ? TRUE : FALSE;
	BOOL chkTrackware = m_chkTrackware ? TRUE : FALSE;
	BOOL chkDialer = m_chkDialer ? TRUE : FALSE;
	BOOL chkRemoteAccess = m_chkRemoteAccess ? TRUE : FALSE;
	BOOL chkAdware = m_chkAdware ? TRUE : FALSE;
	BOOL chkJoke = m_chkJoke ? TRUE : FALSE;
	BOOL chkGenericLoadpoint = m_chkGenericLoadpoint ? TRUE : FALSE;
	BOOL chkClientCompliancy = m_chkClientCompliancy ? TRUE : FALSE;

	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_FILE, chkFile);
	DDX_Check(pDX, IDC_REGISTRY, chkRegistry);
	DDX_Check(pDX, IDC_PROCESS, chkProcess);
	DDX_Check(pDX, IDC_SERVICE, chkService);
	DDX_Check(pDX, IDC_BATCH, chkBatch);
	DDX_Check(pDX, IDC_INI, chkINI);
	DDX_Check(pDX, IDC_STARTUP, chkStartup);
	DDX_Check(pDX, IDC_COM, chkCOM);

	DDX_Check(pDX, IDC_VIRAL, chkViral);
	DDX_Check(pDX, IDC_MALICIOUS, chkMalicious);
	DDX_Check(pDX, IDC_RESERVEMALICIOUS, chkReservedMalicious);
	DDX_Check(pDX, IDC_HEURISTIC, chkHeuristic);
	DDX_Check(pDX, IDC_SECURITYRISK, chkSecurityRisk);
	DDX_Check(pDX, IDC_HACKTOOL, chkHacktool);
	DDX_Check(pDX, IDC_SPYWARE, chkSpyWare);
	DDX_Check(pDX, IDC_TRACKWARE, chkTrackware);
	DDX_Check(pDX, IDC_DIALER, chkDialer);
	DDX_Check(pDX, IDC_REMOTEACCESS, chkRemoteAccess);
	DDX_Check(pDX, IDC_ADWARE, chkAdware);
	DDX_Check(pDX, IDC_JOKE, chkJoke);
	DDX_Check(pDX, IDC_GENERICLOAD, chkGenericLoadpoint);
	DDX_Check(pDX, IDC_CLIENTCOM, chkClientCompliancy);

	m_chkFile = chkFile ? true : false;
	m_chkRegistry = chkRegistry ? true : false;
	m_chkProcess = chkProcess ? true : false;
	m_chkService = chkService ? true : false;
	m_chkBatch = chkBatch ? true : false;
	m_chkINI = chkINI ? true : false;
	m_chkStartup = chkStartup ? true : false;
	m_chkCOM = chkCOM ? true : false;

	m_chkViral = chkViral ? true : false;
	m_chkMalicious = chkMalicious ? true : false;
	m_chkReservedMalicious = chkReservedMalicious ? true : false;
	m_chkHeuristic = chkHeuristic ? true : false;
	m_chkSecurityRisk = chkSecurityRisk ? true : false;
	m_chkHacktool = chkHacktool ? true : false;
	m_chkSpyWare = chkSpyWare ? true : false;
	m_chkTrackware = chkTrackware ? true : false;
	m_chkDialer = chkDialer ? true : false;
	m_chkRemoteAccess = chkRemoteAccess ? true : false;
	m_chkAdware = chkAdware ? true : false;
	m_chkJoke = chkJoke ? true : false;
	m_chkGenericLoadpoint = chkGenericLoadpoint ? true : false;
	m_chkClientCompliancy = chkClientCompliancy ? true : false;
}


BEGIN_MESSAGE_MAP(SetFilterDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// SetFilterDlg message handlers

void SetFilterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void SetFilterDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
