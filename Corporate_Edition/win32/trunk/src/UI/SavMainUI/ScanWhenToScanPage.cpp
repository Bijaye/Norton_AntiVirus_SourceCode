// CScanWhenToScanPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "ScanWhenToScanPage.h"
#include "DialogFont.h"


// CScanWhenToScanPage dialog
IMPLEMENT_DYNAMIC(CScanWhenToScanPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CScanWhenToScanPage, CPropertyPage)
END_MESSAGE_MAP()


CScanWhenToScanPage::CScanWhenToScanPage()
	: CPropertyPage(CScanWhenToScanPage::IDD), scan(NULL), isReadOnly(false)
{
	// Nothing for now
}

CScanWhenToScanPage::~CScanWhenToScanPage()
{
	if (static_cast<HFONT>(titleFont) != NULL)
		titleFont.DeleteObject();
}

void CScanWhenToScanPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_WHENRUN_ONDEMAND, ondemandTitle);
	DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED, scheduledTitle);
	DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP, startupTitle);
	CPropertyPage::DoDataExchange(pDX);
}


// CScanWhenToScanPage message handlers
void CScanWhenToScanPage::OnOK()
{
	bool		showSchedulePage	= false;

	if (!isReadOnly)
	{
		UpdateData(TRUE);
		switch (GetCheckedRadioButton(IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP, IDC_SCANS_CONFIGPAGE_WHENRUN_ONDEMAND))
		{
		case IDC_SCANS_CONFIGPAGE_WHENRUN_ONDEMAND:
			scan->put_StartType(StartType_OnDemand);
			break;
		case IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED:
			scan->put_StartType(StartType_Scheduled);
			showSchedulePage = true;
			break;
		case IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP:
			scan->put_StartType(StartType_Startup);
			break;
		}
	}

	CPropertyPage::OnOK();
}

BOOL CScanWhenToScanPage::OnInitDialog()
{
	StartType				scanStartupType			= StartType_OnDemand;
	UINT					controlIDs[]			= {IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP,
		IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP_DESCRIPTION,
		IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED,
		IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED_DESCRIPTION,
		IDC_SCANS_CONFIGPAGE_WHENRUN_ONDEMAND,
		IDC_SCANS_CONFIGPAGE_WHENRUN_ONDEMAND_DESCRIPTION
	};
	int						currControlIDno			= 0;
	CWnd*					currControl				= NULL;

    // Create controls
    CDialog::OnInitDialog();

	// Set the radio corresponding to this startup type
	scan->get_StartType(&scanStartupType);
	switch (scanStartupType)
	{
	case StartType_OnDemand:
		CheckRadioButton(IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED, IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP, IDC_SCANS_CONFIGPAGE_WHENRUN_ONDEMAND);
		break;
	case StartType_Scheduled:
		CheckRadioButton(IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED, IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP, IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED);
		break;
	case StartType_Startup:
		CheckRadioButton(IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED, IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP, IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP);
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

	// Enlarge the button text
	DialogFont titleFontDF;

	titleFontDF.LoadFromString(IDD_SCANS_CONFIGPAGE_WHENRUN_TITLEFONT);
	titleFontDF.CreateFont(&ondemandTitle, &titleFont);
	ondemandTitle.SetFont(&titleFont);
	scheduledTitle.SetFont(&titleFont);
	startupTitle.SetFont(&titleFont);

    return TRUE;
}

LRESULT CScanWhenToScanPage::OnWizardNext()
{
	DWORD	selectedButton					= IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED;
	LRESULT	returnValNextPageDialogID		= IDD_SCANS_CONFIGPAGE_WHATTOSCAN;

	// Save data in Wizard mode
	OnOK();
	// Skip past the next page, schedule options, unless this is a scheduled scan
	selectedButton = GetCheckedRadioButton(IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED, IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP);
	switch (selectedButton)
	{
	case IDC_SCANS_CONFIGPAGE_WHENRUN_SCHEDULED:
		scan->put_StartType(StartType_Scheduled);
		// Proceed to the next page in the wizard
		returnValNextPageDialogID = 0;
		break;
	case IDC_SCANS_CONFIGPAGE_WHENRUN_STARTUP:
		scan->put_StartType(StartType_Startup);
		break;
	case IDC_SCANS_CONFIGPAGE_WHENRUN_ONDEMAND:
		scan->put_StartType(StartType_OnDemand);
		break;
	}
	return returnValNextPageDialogID;
}

BOOL CScanWhenToScanPage::OnSetActive()
{
	CPropertySheet* parentSheet = NULL;

	parentSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	parentSheet->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}

void CScanWhenToScanPage::SetScan( CSavConfigureableScan* newScan )
// Sets the scan to configure
{
	scan = newScan;
}

void CScanWhenToScanPage::SetReadOnly( bool newIsReadOnly )
{
	isReadOnly = newIsReadOnly;
}
