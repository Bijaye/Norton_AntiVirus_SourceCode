// ScanWhatToScanPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "ScanWhatToScanPage.h"


// CScanWhatToScanPage dialog
IMPLEMENT_DYNAMIC(CScanWhatToScanPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CScanWhatToScanPage, CPropertyPage)
END_MESSAGE_MAP()


CScanWhatToScanPage::CScanWhatToScanPage()
: CPropertyPage(CScanWhatToScanPage::IDD), scan(NULL), isReadOnly(false)
{
	// Nothing for now
}

CScanWhatToScanPage::~CScanWhatToScanPage()
{
	// Nothing for now
}

void CScanWhatToScanPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK_DESCRIPTION, quickScanDescriptionCtrl);
}


// CScanWhatToScanPage message handlers
void CScanWhatToScanPage::OnOK()
{
	if (!isReadOnly)
	{
		UpdateData(TRUE);
		switch (GetCheckedRadioButton(IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM))
		{
		case IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM:
			scan->SetWhatToScan(CSavConfigureableScan::WhatToScan_UserSpecified);
			break;
		case IDC_SCANS_CONFIGPAGE_WHATTOSCAN_FULL:
			scan->SetWhatToScan(CSavConfigureableScan::WhatToScan_EntireSystem);
			break;
		case IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK:
			scan->SetWhatToScan(CSavConfigureableScan::WhatToScan_QuickScanAreas);
			break;
		}
	}
}

BOOL CScanWhatToScanPage::OnInitDialog()
{
	CString									quickScanDescription;
	CSavConfigureableScan::WhatToScan		scanAreas					= CSavConfigureableScan::WhatToScan_UserSpecified;
	UINT					controlIDs[]			= {IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK,
		IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK_DESCRIPTION,
		IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM,
		IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM_DESCRIPTION,
		IDC_SCANS_CONFIGPAGE_WHATTOSCAN_FULL,
		IDC_SCANS_CONFIGPAGE_WHATTOSCAN_FULL_DESCRIPTION
	};
	int						currControlIDno			= 0;
	CWnd*					currControl				= NULL;

    // Create controls
    CDialog::OnInitDialog();

	quickScanDescription.LoadString(IDS_SCANS_CONFIGPAGE_WHATTOSCAN_QUICKDESCRIPTION);
	quickScanDescriptionCtrl.SetWindowText(quickScanDescription);

	scan->GetWhatToScan(&scanAreas);
	switch (scanAreas)
	{
	case CSavConfigureableScan::WhatToScan_UserSpecified:
		this->CheckRadioButton(IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM);
		break;
	case CSavConfigureableScan::WhatToScan_EntireSystem:
		this->CheckRadioButton(IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_FULL);
		break;
	case CSavConfigureableScan::WhatToScan_QuickScanAreas:
		this->CheckRadioButton(IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_CUSTOM, IDC_SCANS_CONFIGPAGE_WHATTOSCAN_QUICK);
		break;
	}
	UpdateData(FALSE);
	if (isReadOnly)
	{
		for (currControlIDno = 0; currControlIDno < sizeof(controlIDs)/sizeof(controlIDs[0]); currControlIDno++)
		{
			currControl = GetDlgItem(controlIDs[currControlIDno]);
			currControl->EnableWindow(FALSE);
		}
	}

	return TRUE;
}

BOOL CScanWhatToScanPage::OnSetActive()
{
	CPropertySheet* parentSheet = NULL;

	// Only allow back if in add mode AND this is a scheduled scan
	parentSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	if (parentSheet->IsWizard())
	{
		StartType scanStartType = StartType_OnDemand;
		scan->get_StartType(&scanStartType);
		if (scanStartType != StartType_Scheduled)
			parentSheet->SetWizardButtons(PSWIZB_NEXT);
		else
			parentSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}
	else
	{
		parentSheet->SetWizardButtons(PSWIZB_NEXT);
	}
	return TRUE;
}

LRESULT CScanWhatToScanPage::OnWizardNext()
{
	CSavConfigureableScan::WhatToScan		scanAreas					= CSavConfigureableScan::WhatToScan_UserSpecified;

	// Save out the changes so the next dialog in line has what it needs
	OnOK();

	// If this is a quick scan or full scan, no need to show the Files page, just advance to options
	scan->GetWhatToScan(&scanAreas);
	if (scanAreas == CSavConfigureableScan::WhatToScan_UserSpecified)
		return 0;
	else
		return IDD_SCANS_CONFIGPAGE_OPTIONS;

	return 0;
}

void CScanWhatToScanPage::SetScan( CSavConfigureableScan* newScan )
// Sets the scan to configure
{
	scan = newScan;
}

void CScanWhatToScanPage::SetReadOnly( bool newIsReadOnly )
{
	isReadOnly = newIsReadOnly;
}