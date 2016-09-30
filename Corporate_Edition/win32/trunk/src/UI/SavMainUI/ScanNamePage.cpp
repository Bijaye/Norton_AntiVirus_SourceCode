// ScanNamePage.cpp : implementation file
//

#include "stdafx.h"
#include "ScanNamePage.h"
#include "ConfigObj.h"


// CScanNamePage dialog
IMPLEMENT_DYNAMIC(CScanNamePage, CPropertyPage)

BEGIN_MESSAGE_MAP(CScanNamePage, CPropertyPage)
	ON_EN_CHANGE(IDC_SCANS_CONFIGPAGE_NAME_NAME, ValidateName)
END_MESSAGE_MAP()


CScanNamePage::CScanNamePage()
	: CPropertyPage(CScanNamePage::IDD), scan(NULL)
{
	// No code needed
}

CScanNamePage::~CScanNamePage()
{
    scan = NULL;
}

void CScanNamePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCANS_CONFIGPAGE_NAME_NAME, name);
	DDV_MaxChars(pDX, name, 30);
	DDX_Text(pDX, IDC_SCANS_CONFIGPAGE_NAME_DESCRIPTION, description);
	DDV_MaxChars(pDX, description, 256);
	DDX_Text(pDX, IDD_SCANS_CONFIGPAGE_NAME_WHENRUN, whenRun);
}

// CScanOptionsPage message handlers
void CScanNamePage::OnOK()
{
	// Save data
	UpdateData(TRUE);
	scan->put_Name(CComBSTR(name));
	scan->put_Description(CComBSTR(description));
	CPropertyPage::OnOK();
}

BOOL CScanNamePage::OnWizardFinish()
{
	// Save data when in Wizard mode
	OnOK();
	return CPropertyPage::OnWizardFinish();
}

BOOL CScanNamePage::OnInitDialog()
{
	CComBSTR	scanNameTemp;
	CComBSTR	scanDescriptionTemp;
	StartType	scanStartType			= StartType_OnDemand;

    // Create controls
    CDialog::OnInitDialog();

	scan->get_Name(&scanNameTemp);
	scan->get_Description(&scanDescriptionTemp);
	name = scanNameTemp;
	description = scanDescriptionTemp;
	scan->get_StartType(&scanStartType);
	switch (scanStartType)
	{
	case StartType_OnDemand:
		whenRun.LoadString(IDS_SCANS_CONFIGPAGE_NAME_WHEN_ONDEMAND);
		break;
	case StartType_Scheduled:
		whenRun.LoadString(IDS_SCANS_CONFIGPAGE_NAME_WHEN_SCHEDULED);
		// TODO:  Later determine schedule and display just below this
		// control 
		break;
	case StartType_Startup:
		whenRun.LoadString(IDS_SCANS_CONFIGPAGE_NAME_WHEN_STARTUP);
		break;
	}
	UpdateData(FALSE);
	return TRUE;
}

BOOL CScanNamePage::OnSetActive()
{
	CPropertySheet* parentSheet = NULL;

	parentSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	parentSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return CPropertyPage::OnSetActive();
}

void CScanNamePage::ValidateName()
{
	CPropertySheet*		parentSheet = NULL;
	CString				messageTitle;
	CString				messageText;
	HRESULT				returnValHR = E_FAIL;

	// Initialize
	parentSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	UpdateData(TRUE);

	returnValHR = scan->put_Name(CComBSTR(name));
	if (SUCCEEDED(returnValHR))
		parentSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	else
		parentSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_DISABLEDFINISH);
}

void CScanNamePage::SetScan( CSavConfigureableScan* newScan )
// Set the scan object we're changing
{
	scan = newScan;
}