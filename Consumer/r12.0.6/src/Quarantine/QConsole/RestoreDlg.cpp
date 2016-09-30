//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
// 
// RestoreDlg.h - Implementation file for class to handle simple custom dialog for 
// confirming restores.  If there are expanded threat items, adds a checkbox
// to ask the user if they want to exclude the selected items from future 
// scans.
// Fetches appropriate strings for: 
//		multiple vs single items selected
//		viral vs. non-viral view
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconres.h"
#include "RestoreDlg.h"

#include "AVRESBranding.h"
#include "QConsoleDoc.h"

CString CRestoreDlg::m_sAppName;


// CRestoreDlg dialog

IMPLEMENT_DYNAMIC(CRestoreDlg, CDialog)

CRestoreDlg::CRestoreDlg(bool bKnownExpandedThreats, 
						 int iSelectedItems, 
						 CWnd* pParent /*= NULL*/)
						 : CDialog(CRestoreDlg::IDD, pParent)
						 , m_bKnownExpandedThreats(bKnownExpandedThreats)
						 , m_iSelectedItems(iSelectedItems)
						 , m_bExclusionChecked(FALSE)
						 , m_sRestoreDlgText(_T(""))
						 , m_hIcon(NULL)
{
}

CRestoreDlg::~CRestoreDlg()
{
}


void CRestoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RESTOREDLG_TEXT, m_sRestoreDlgText);
	DDX_Check(pDX, IDC_RESTOREDLG_CHECK, m_bExclusionChecked);
}


BOOL CRestoreDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(IsExpandedThreatView())
	{
		if(m_iSelectedItems > 1)
			m_sRestoreDlgText.Format(IDS_MULTIPLE_RESTORE_WARNING_EXPANDED_THREAT, m_iSelectedItems);
		else
			m_sRestoreDlgText.LoadString(IDS_RESTORE_WARNING_EXPANDED_THREAT);		
	}
	else
	{
		if(m_iSelectedItems > 1)
			m_sRestoreDlgText.Format(IDS_MULTIPLE_RESTORE_WARNING_VIRAL, m_iSelectedItems);
		else
			m_sRestoreDlgText.LoadString(IDS_RESTORE_WARNING_VIRAL);
	}

	if(m_bKnownExpandedThreats)
	{
		CString sExclusionCheckText;
		if(m_iSelectedItems > 1)
			sExclusionCheckText.LoadString(IDS_MULTIPLE_RESTORE_EXCLUSION_CHECK);
		else
			sExclusionCheckText.LoadString(IDS_RESTORE_EXCLUSION_CHECK);
		GetDlgItem(IDC_RESTOREDLG_CHECK)->SetWindowText(sExclusionCheckText);

		GetDlgItem(IDC_RESTOREDLG_CHECK)->ShowWindow(TRUE);
	}

	// Icon
	m_hIcon = AfxGetApp()->LoadStandardIcon(MAKEINTRESOURCE(IDI_EXCLAMATION));
	ASSERT(m_hIcon != NULL);
	HICON hIconResult = ((CStatic*)GetDlgItem(IDC_RESTOREDLG_ICON))->SetIcon(m_hIcon);

	// Set title
	if(m_sAppName.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_sAppName = BrandRes.GetString(IDS_QUARANTINE_APP_NAME);
	}

	SetWindowText(m_sAppName);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


bool CRestoreDlg::IsExpandedThreatView()
{
	switch(m_pDoc->GetViewMode())
	{
	case VIEW_MODE_EXPANDED_THREATS:
		return true;
	case VIEW_MODE_VIRAL_THREATS:
	default:
		return false;
	}
}

bool CRestoreDlg::ShouldExcludeSelected()
{
	return (m_bExclusionChecked ? true : false);
}


BEGIN_MESSAGE_MAP(CRestoreDlg, CDialog)
	ON_BN_CLICKED(IDYES, OnBnClickedYes)
	ON_BN_CLICKED(IDNO, OnBnClickedNo)
END_MESSAGE_MAP()


// CRestoreDlg message handlers

void CRestoreDlg::OnBnClickedYes()
{
	if (!UpdateData(TRUE))
	{
		return;
	}
	CDialog::EndDialog(IDYES);
}

void CRestoreDlg::OnBnClickedNo()
{
	CDialog::EndDialog(IDNO);
}
