// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPBasicPage.cpp : implementation file
//
#include "stdafx.h"
#include "HPPProtectionProviderUI.h"
#include "HPPBasicPage.h"
#include "HPPScanOptionPage.h"
#include "HPPSheet.h"


// CHPPBasicPage dialog

IMPLEMENT_DYNAMIC(CHPPBasicPage, CPropertyPage)

CHPPBasicPage::CHPPBasicPage()
	: CBasePageUI(CHPPBasicPage::IDD)
{
	m_dwProccessScanInterval = 0;
}

CHPPBasicPage::~CHPPBasicPage()
{
}

void CHPPBasicPage::DoDataExchange(CDataExchange* pDX)
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	CBasePageUI::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_ENABLE_HPP_PAGE_RTS, m_ctrlEnableHeurPP);
//	DDX_Check(pDX, IDC_ENABLE_HPP_PAGE_RTS, pSheet->m_bEnableHeurPP);
	DDX_Control(pDX, IDC_HPP_PAGE_RT_STATUS, m_ctrlHeurPPStatusText);
	DDX_Control(pDX, IDC_HPP_PAGE_DETECTIONS, m_ctrlHeurPPDetectionsText);
	DDX_Control(pDX, IDC_HPP_PAGE_NEW_PROC_SCAN_TIME, m_ctrlHeurPPNewPSTText);
	DDX_Control(pDX, IDC_HPP_PAGE_FULL_PROC_SCAN_TIME, m_ctrlHeurPPFPStimeText);
	DDX_Control(pDX, IDC_HPP_PAGE_PROCESS_DETECTED, m_ctrlHeurPPProcessNameText);
	DDX_Control(pDX, IDC_HPP_PAGE_FILENAME, m_ctrlHeurPPFilenameText);
	DDX_Control(pDX, IDC_HPP_PAGE_ACTION_TAKEN, m_ctrlHeurPPActionText);
	DDX_Control(pDX, IDC_HPP_PAGE_DETECTION_TIME, m_ctrlHeurPPActionTimeText);
}


BEGIN_MESSAGE_MAP(CHPPBasicPage, CBasePageUI)
	ON_BN_CLICKED(IDC_ENABLE_HPP_PAGE_RTS, &CHPPBasicPage::OnBnClickedEnableHPPRts)
	ON_STN_CLICKED(IDC_HPP_PAGE_RT_STATUS, &CHPPBasicPage::OnStnClickedHPPRtStatus)
	ON_STN_CLICKED(IDC_HPP_PAGE_DETECTIONS, &CHPPBasicPage::OnStnClickedHPPDetections)
END_MESSAGE_MAP()


// CHPPBasicPage message handlers

////////////////////////////////////////////////////////////////////////
// Function:    CHPPBasicPage::Load
//
// Description: Load the configuration settings form the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPBasicPage::Load()
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	//ScanInterval
	Read(m_dwProccessScanInterval, szReg_Val_SymHPPS_ScanFrequency, 0);

	UpdateData(FALSE);
	OnUpdateUI();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPBasicPage::Store
//
// Description: Writes the configuration settings to the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPBasicPage::Store()
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	UpdateData(TRUE);
}

void CHPPBasicPage::OnBnClickedEnableHPPRts()
{
	// TODO: Add your control notification handler code here
}

void CHPPBasicPage::OnStnClickedHPPRtStatus()
{
	// TODO: Add your control notification handler code here
}

void CHPPBasicPage::OnStnClickedHPPDetections()
{
	// TODO: Add your control notification handler code here
}

BOOL CHPPBasicPage::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CBasePageUI::OnInitDialog();
	Load();

	OnUpdateUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CHPPBasicPage::OnSetActive() 
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	OnUpdateUI();
	return CBasePageUI::OnSetActive();
}

void CHPPBasicPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CBasePageUI::OnCancel();
}

void CHPPBasicPage::OnOK()
{
	UpdateData(TRUE);
	Store();

	CBasePageUI::OnOK();
}

void CHPPBasicPage::OnUpdateUI()
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	CString strWindowText;
	// ATL Loadstring thow catch.  
	
	strWindowText.Format(IDS_NEW_PROC_TIME_FORMAT, m_dwProccessScanInterval);
	m_ctrlHeurPPNewPSTText.SetWindowText(strWindowText);

	for ( int nPage = 0L; nPage < pSheet->GetPageCount(); nPage++ )
	{
		CPropertyPage* pPage = pSheet->GetPage(nPage);
		if ( pPage && pPage->IsKindOf(RUNTIME_CLASS(CHPPScanOptionPage)) )
		{
			CHPPScanOptionPage* pScanPage = reinterpret_cast<CHPPScanOptionPage*>(pPage);
			if ( pScanPage )
			{
				if(pScanPage->IsHPPEnabled())
					strWindowText.LoadString(IDS_ENABLED);
				else
					strWindowText.LoadString(IDS_DISABLED);

				m_ctrlHeurPPStatusText.SetWindowText(strWindowText);
			}
			break;
		}
	}
}
