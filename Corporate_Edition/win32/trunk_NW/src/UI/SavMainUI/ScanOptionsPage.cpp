// ScanOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScanOptionsPage.h"
#include "ConfigObj.h"


// CScanOptionsPage dialog
IMPLEMENT_DYNAMIC(CScanOptionsPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CScanOptionsPage, CPropertyPage)
END_MESSAGE_MAP()


CScanOptionsPage::CScanOptionsPage()
: CPropertyPage(CScanOptionsPage::IDD), scan(NULL)
{
	// No code needed
}

CScanOptionsPage::~CScanOptionsPage()
{
    scan = NULL;
}

void CScanOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_OPTIONS_OPTIONSCTRL, optionsCtrl);
}

// CScanOptionsPage message handlers
void CScanOptionsPage::OnOK()
{
	optionsCtrl.Store();
}

BOOL CScanOptionsPage::OnInitDialog()
{
    // Create controls
    CDialog::OnInitDialog();

	optionsCtrl.SetConfigInterface(scan->GetScanConfig());
	optionsCtrl.Load();
	optionsCtrl.ModifyStyleEx(NULL, WS_EX_CONTROLPARENT);

	return TRUE;
}

BOOL CScanOptionsPage::OnSetActive()
{
	// Turn the back button back on
	CPropertySheet* parentSheet = NULL;

	// Disallow back
	parentSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	parentSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return TRUE;
}

LRESULT CScanOptionsPage::OnWizardBack()
{
	CSavConfigureableScan::WhatToScan		scanAreas					= CSavConfigureableScan::WhatToScan_UserSpecified;

	// If this is a quick scan or full scan, no need to show the Files page, just go back to What To Scan
	scan->GetWhatToScan(&scanAreas);
	if (scanAreas == CSavConfigureableScan::WhatToScan_UserSpecified)
		return 0;
	else
		return IDD_SCANS_CONFIGPAGE_WHATTOSCAN;

	return 0;
}

void CScanOptionsPage::SetScan( CSavConfigureableScan* newScan )
{
    scan = newScan;
}