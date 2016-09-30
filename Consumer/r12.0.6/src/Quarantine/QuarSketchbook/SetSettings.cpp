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
// SetSettings.cpp : implementation file
//

#include "stdafx.h"
#include "sketchbook.h"
#include "SetSettings.h"


// SetSettings dialog

IMPLEMENT_DYNAMIC(SetSettings, CDialog)
SetSettings::SetSettings(CWnd* pParent /*=NULL*/)
	: CDialog(SetSettings::IDD, pParent)
	, m_bScanDll(FALSE)
	, m_dwDomain(0)
	, m_dwRepair(0)
{
}

SetSettings::~SetSettings()
{
}

void SetSettings::DoDataExchange(CDataExchange* pDX)
{
	BOOL bScanDll = m_bScanDll ? TRUE : FALSE;

	CDialog::DoDataExchange(pDX);
	DDX_Text (pDX, IDC_DEPTH, m_dwSettingsDepth);
	DDX_Check(pDX, IDC_SCANDLL, bScanDll);
	DDX_CBIndex(pDX, IDC_DOMAIN, m_dwDomain);
	DDX_CBIndex(pDX, IDC_REPAIR, m_dwRepair);

	m_bScanDll = bScanDll ? true : false;
}


BEGIN_MESSAGE_MAP(SetSettings, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RESETFILTER, OnBnClickedResetfilter)
	ON_BN_CLICKED(IDC_FILTER, OnBnClickedFilter)
	ON_BN_CLICKED(IDC_OUTFILTER, OnBnClickedOutfilter)
	ON_BN_CLICKED(IDC_RESETOUTFILTER, OnBnClickedResetoutfilter)
END_MESSAGE_MAP()


// SetSettings message handlers

void SetSettings::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void SetSettings::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void SetSettings::OnBnClickedFilter()
{
	// TODO: Add your control notification handler code here

	ccEraser::IFilterQIPtr pFilter = m_pInputFilter;

	SetFilterDlg m_filterDlg(this);

	if(pFilter)
	{
		pFilter->InDetectionFilter(ccEraser::BatchDetectionActionType, m_filterDlg.m_chkBatch);
		pFilter->InDetectionFilter(ccEraser::COMDetectionActionType, m_filterDlg.m_chkCOM);
		pFilter->InDetectionFilter(ccEraser::FileDetectionActionType, m_filterDlg.m_chkFile);
		pFilter->InDetectionFilter(ccEraser::INIDetectionActionType, m_filterDlg.m_chkINI);
		pFilter->InDetectionFilter(ccEraser::ProcessDetectionActionType, m_filterDlg.m_chkProcess);
		pFilter->InDetectionFilter(ccEraser::RegistryDetectionActionType, m_filterDlg.m_chkRegistry);
		pFilter->InDetectionFilter(ccEraser::ServiceDetectionActionType, m_filterDlg.m_chkService);
		pFilter->InDetectionFilter(ccEraser::StartupDetectionActionType, m_filterDlg.m_chkStartup);

		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Viral, m_filterDlg.m_chkViral);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Malicious, m_filterDlg.m_chkMalicious);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::ReservedMalicious, m_filterDlg.m_chkReservedMalicious);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Heuristic, m_filterDlg.m_chkHeuristic);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::SecurityRisk, m_filterDlg.m_chkSecurityRisk);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Hacktool, m_filterDlg.m_chkHacktool);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::SpyWare, m_filterDlg.m_chkSpyWare);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Trackware, m_filterDlg.m_chkTrackware);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Dialer, m_filterDlg.m_chkDialer);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::RemoteAccess, m_filterDlg.m_chkRemoteAccess);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Adware, m_filterDlg.m_chkAdware);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Joke, m_filterDlg.m_chkJoke);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::ClientCompliancy, m_filterDlg.m_chkClientCompliancy);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::GenericLoadPoint, m_filterDlg.m_chkGenericLoadpoint);
	}

	if(IDOK == m_filterDlg.DoModal())
	{
		CSketchbookDlg * Dlg = (CSketchbookDlg *) GetParent();
		ccEraser::eResult res = Dlg->m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, (void **) &pFilter);
		m_pDlg->setResultWindow(res, _T("Dlg->m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, (void **) &pFilter)"));
		if(Failed(res))
		{
			return;
		}

		m_filterDlg.m_chkBatch && pFilter->AddDetectionType(ccEraser::BatchDetectionActionType);
		m_filterDlg.m_chkCOM && pFilter->AddDetectionType(ccEraser::COMDetectionActionType);
		m_filterDlg.m_chkFile && pFilter->AddDetectionType(ccEraser::FileDetectionActionType);
		m_filterDlg.m_chkINI && pFilter->AddDetectionType(ccEraser::INIDetectionActionType);
		m_filterDlg.m_chkProcess && pFilter->AddDetectionType(ccEraser::ProcessDetectionActionType);
		m_filterDlg.m_chkRegistry && pFilter->AddDetectionType(ccEraser::RegistryDetectionActionType);
		m_filterDlg.m_chkService && pFilter->AddDetectionType(ccEraser::ServiceDetectionActionType);
		m_filterDlg.m_chkStartup && pFilter->AddDetectionType(ccEraser::StartupDetectionActionType);

		m_filterDlg.m_chkViral && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Viral);
		m_filterDlg.m_chkMalicious && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Malicious);
		m_filterDlg.m_chkReservedMalicious && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::ReservedMalicious);
		m_filterDlg.m_chkHeuristic && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Heuristic);
		m_filterDlg.m_chkSecurityRisk && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::SecurityRisk);
		m_filterDlg.m_chkHacktool && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Hacktool);
		m_filterDlg.m_chkSpyWare && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::SpyWare);
		m_filterDlg.m_chkTrackware && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Trackware);
		m_filterDlg.m_chkDialer && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Dialer);
		m_filterDlg.m_chkRemoteAccess && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::RemoteAccess);
		m_filterDlg.m_chkAdware && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Adware);
		m_filterDlg.m_chkJoke && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Joke);
		m_filterDlg.m_chkClientCompliancy && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::ClientCompliancy);
		m_filterDlg.m_chkGenericLoadpoint && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

		!m_filterDlg.m_chkBatch && pFilter->RemoveDetectionType(ccEraser::BatchDetectionActionType);
		!m_filterDlg.m_chkCOM && pFilter->RemoveDetectionType(ccEraser::COMDetectionActionType);
		!m_filterDlg.m_chkFile && pFilter->RemoveDetectionType(ccEraser::FileDetectionActionType);
		!m_filterDlg.m_chkINI && pFilter->RemoveDetectionType(ccEraser::INIDetectionActionType);
		!m_filterDlg.m_chkProcess && pFilter->RemoveDetectionType(ccEraser::ProcessDetectionActionType);
		!m_filterDlg.m_chkRegistry && pFilter->RemoveDetectionType(ccEraser::RegistryDetectionActionType);
		!m_filterDlg.m_chkService && pFilter->RemoveDetectionType(ccEraser::ServiceDetectionActionType);
		!m_filterDlg.m_chkStartup && pFilter->RemoveDetectionType(ccEraser::StartupDetectionActionType);

		!m_filterDlg.m_chkViral && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Viral);
		!m_filterDlg.m_chkMalicious && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Malicious);
		!m_filterDlg.m_chkReservedMalicious && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::ReservedMalicious);
		!m_filterDlg.m_chkHeuristic && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Heuristic);
		!m_filterDlg.m_chkSecurityRisk && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::SecurityRisk);
		!m_filterDlg.m_chkHacktool && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Hacktool);
		!m_filterDlg.m_chkSpyWare && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::SpyWare);
		!m_filterDlg.m_chkTrackware && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Trackware);
		!m_filterDlg.m_chkDialer && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Dialer);
		!m_filterDlg.m_chkRemoteAccess && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::RemoteAccess);
		!m_filterDlg.m_chkAdware && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Adware);
		!m_filterDlg.m_chkJoke && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Joke);
		!m_filterDlg.m_chkClientCompliancy && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::ClientCompliancy);
		!m_filterDlg.m_chkGenericLoadpoint && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

		m_pInputFilter = pFilter;
	}
}

void SetSettings::OnBnClickedResetfilter()
{
	// TODO: Add your control notification handler code here
	ccEraser::IFilterQIPtr pFilter = m_pInputFilter;

	if(pFilter == NULL)
	{
		return;
	}

	ccEraser::eResult res = pFilter->ClearDetectionFilter();
	m_pDlg->setResultWindow(res, _T("pFilter->ClearDetectionFilter();"));
	if(Failed(res))
	{
		return;
	}

	res = pFilter->ClearAnomalyFilter();
	m_pDlg->setResultWindow(res, _T("res = pFilter->ClearAnomalyFilter()"));
	if(Failed(res))
	{
		return;
	}

	m_pInputFilter = pFilter;
}

void SetSettings::OnBnClickedOutfilter()
{
	// TODO: Add your control notification handler code here

	ccEraser::IFilterQIPtr pFilter = m_pOutputFilter;

	SetFilterDlg m_filterDlg(this);
	m_filterDlg.m_bOutFilter = TRUE;

	if(pFilter)
	{
		pFilter->InDetectionFilter(ccEraser::BatchDetectionActionType, m_filterDlg.m_chkBatch);
		pFilter->InDetectionFilter(ccEraser::COMDetectionActionType, m_filterDlg.m_chkCOM);
		pFilter->InDetectionFilter(ccEraser::FileDetectionActionType, m_filterDlg.m_chkFile);
		pFilter->InDetectionFilter(ccEraser::INIDetectionActionType, m_filterDlg.m_chkINI);
		pFilter->InDetectionFilter(ccEraser::ProcessDetectionActionType, m_filterDlg.m_chkProcess);
		pFilter->InDetectionFilter(ccEraser::RegistryDetectionActionType, m_filterDlg.m_chkRegistry);
		pFilter->InDetectionFilter(ccEraser::ServiceDetectionActionType, m_filterDlg.m_chkService);
		pFilter->InDetectionFilter(ccEraser::StartupDetectionActionType, m_filterDlg.m_chkStartup);

		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Viral, m_filterDlg.m_chkViral);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Malicious, m_filterDlg.m_chkMalicious);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::ReservedMalicious, m_filterDlg.m_chkReservedMalicious);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Heuristic, m_filterDlg.m_chkHeuristic);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::SecurityRisk, m_filterDlg.m_chkSecurityRisk);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Hacktool, m_filterDlg.m_chkHacktool);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::SpyWare, m_filterDlg.m_chkSpyWare);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Trackware, m_filterDlg.m_chkTrackware);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Dialer, m_filterDlg.m_chkDialer);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::RemoteAccess, m_filterDlg.m_chkRemoteAccess);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Adware, m_filterDlg.m_chkAdware);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::Joke, m_filterDlg.m_chkJoke);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::ClientCompliancy, m_filterDlg.m_chkClientCompliancy);
		pFilter->InAnomalyFilter(ccEraser::IAnomaly::GenericLoadPoint, m_filterDlg.m_chkGenericLoadpoint);
	}

	if(IDOK == m_filterDlg.DoModal())
	{
		CSketchbookDlg * Dlg = (CSketchbookDlg *) GetParent();
		ccEraser::eResult res = Dlg->m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, (void **) &pFilter);
		m_pDlg->setResultWindow(res, _T("Dlg->m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, (void **) &pFilter)"));
		if(Failed(res))
		{
			return;
		}

		m_filterDlg.m_chkViral && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Viral);
		m_filterDlg.m_chkMalicious && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Malicious);
		m_filterDlg.m_chkReservedMalicious && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::ReservedMalicious);
		m_filterDlg.m_chkHeuristic && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Heuristic);
		m_filterDlg.m_chkSecurityRisk && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::SecurityRisk);
		m_filterDlg.m_chkHacktool && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Hacktool);
		m_filterDlg.m_chkSpyWare && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::SpyWare);
		m_filterDlg.m_chkTrackware && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Trackware);
		m_filterDlg.m_chkDialer && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Dialer);
		m_filterDlg.m_chkRemoteAccess && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::RemoteAccess);
		m_filterDlg.m_chkAdware && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Adware);
		m_filterDlg.m_chkJoke && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Joke);
		m_filterDlg.m_chkClientCompliancy && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::ClientCompliancy);
		m_filterDlg.m_chkGenericLoadpoint && pFilter->AddAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

		!m_filterDlg.m_chkViral && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Viral);
		!m_filterDlg.m_chkMalicious && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Malicious);
		!m_filterDlg.m_chkReservedMalicious && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::ReservedMalicious);
		!m_filterDlg.m_chkHeuristic && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Heuristic);
		!m_filterDlg.m_chkSecurityRisk && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::SecurityRisk);
		!m_filterDlg.m_chkHacktool && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Hacktool);
		!m_filterDlg.m_chkSpyWare && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::SpyWare);
		!m_filterDlg.m_chkTrackware && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Trackware);
		!m_filterDlg.m_chkDialer && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Dialer);
		!m_filterDlg.m_chkRemoteAccess && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::RemoteAccess);
		!m_filterDlg.m_chkAdware && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Adware);
		!m_filterDlg.m_chkJoke && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::Joke);
		!m_filterDlg.m_chkClientCompliancy && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::ClientCompliancy);
		!m_filterDlg.m_chkGenericLoadpoint && pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

		m_pOutputFilter = pFilter;
	}
}

void SetSettings::OnBnClickedResetoutfilter()
{
	// TODO: Add your control notification handler code here

	ccEraser::IFilterQIPtr pFilter = m_pOutputFilter;

	if(pFilter == NULL)
	{
		return;
	}

	ccEraser::eResult res = pFilter->ClearDetectionFilter();
	m_pDlg->setResultWindow(res, _T("pFilter->ClearDetectionFilter();"));
	if(Failed(res))
	{
		return;
	}

	res = pFilter->ClearAnomalyFilter();
	m_pDlg->setResultWindow(res, _T("res = pFilter->ClearAnomalyFilter()"));
	if(Failed(res))
	{
		return;
	}

	m_pOutputFilter = pFilter;
}