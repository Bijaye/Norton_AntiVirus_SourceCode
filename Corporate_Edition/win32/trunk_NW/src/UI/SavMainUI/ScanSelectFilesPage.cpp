// ScanSelectFilesPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "ScanSelectFilesPage.h"
#include "ConfigObj.h"


// CScanSelectFilesPage dialog
IMPLEMENT_DYNAMIC(CScanSelectFilesPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CScanSelectFilesPage, CPropertyPage)
END_MESSAGE_MAP()


CScanSelectFilesPage::CScanSelectFilesPage()
	: CPropertyPage(CScanSelectFilesPage::IDD), scan(NULL)
{
    // No code needed
}

CScanSelectFilesPage::~CScanSelectFilesPage()
{
    scan = NULL;
}

void CScanSelectFilesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_FILES_SELECTOR, selectorCtrl);
	DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_FILES_TYPETITLE, typeTitleCtrl);
	DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_FILES_TYPEDESCRIPTION, typeDescriptionCtrl);
}

// CScanSelectFilesPage message handlers
void CScanSelectFilesPage::OnOK()
{
	selectorCtrl.SaveConfig(scan->GetScanConfig(), _T(szReg_Key_Directories), _T(szReg_Key_Files));
}

BOOL CScanSelectFilesPage::OnInitDialog()
{
    IVPStorage2*							scanStorage     = NULL;
	CSavConfigureableScan::WhatToScan		scanAreas		= CSavConfigureableScan::WhatToScan_UserSpecified;
	CString									typeTitle;
	CString									typeDescription;
	HRESULT									returnValHR     = E_FAIL;

    // Create controls
    CDialog::OnInitDialog();

	scan->GetWhatToScan(&scanAreas);
	if ((scanAreas == CSavConfigureableScan::WhatToScan_EntireSystem) || (scanAreas == CSavConfigureableScan::WhatToScan_QuickScanAreas))
	{
		// NOTE:  Selector control is refusing to be hidden no matter what I do.  Workaround was to resize
		// the title and description controls to completely overlap it in this case.
		selectorCtrl.SetActivate(0);
		selectorCtrl.ShowWindow(SW_HIDE);
		typeTitleCtrl.ShowWindow(SW_NORMAL);
		typeDescriptionCtrl.ShowWindow(SW_NORMAL);

		if (scanAreas == CSavConfigureableScan::WhatToScan_EntireSystem)
		{
			typeTitle.LoadString(IDS_SCANS_CONFIGPAGE_FILES_FULL_TITLE);
			typeDescription.LoadString(IDS_SCANS_CONFIGPAGE_FILES_FULL_DESCRIPTION);
		}
		else
		{
			typeTitle.LoadString(IDS_SCANS_CONFIGPAGE_FILES_QUICK_TITLE);
			typeDescription.LoadString(IDS_SCANS_CONFIGPAGE_FILES_QUICK_DESCRIPTION);
		}
		typeDescription = _T("\n\n") + typeDescription;
		typeTitleCtrl.SetWindowText(typeTitle);
		typeDescriptionCtrl.SetWindowText(typeDescription);
	}
	else
	{
		selectorCtrl.ShowWindow(SW_NORMAL);
		typeTitleCtrl.ShowWindow(SW_HIDE);
		typeDescriptionCtrl.ShowWindow(SW_HIDE);

		// NOTE:  WS_EX_CONTROLPARENT appears unneeded.  Further, adding it will cause the ShelSel2 control to
		// become invisible.
		selectorCtrl.SetBShowCDROM(TRUE);
		selectorCtrl.SetBShowMail(FALSE);
		selectorCtrl.SetShowPassword(TRUE);
		// NOTE:  Failure to call SetStorage will result in a GPF in the ShelSel2 control in LoadConfig or SetActivate
		returnValHR = CoCreateLDVPObject(CLSID_CliProxy, IID_IVPStorage2, reinterpret_cast<void**>(&scanStorage));
		if (SUCCEEDED(returnValHR))
		{
			scanStorage->InitRootForScan(NULL, NULL);
			selectorCtrl.SetStorage(scanStorage);
			scanStorage->Release();
			scanStorage = NULL;
		}
		// NOTE:  Failure to call both SetStorage and LoadConfig results in an empty control
		// Load scan settings into control
		selectorCtrl.LoadConfig(scan->GetScanConfig(), _T(szReg_Key_Directories), _T(szReg_Key_Files));
		selectorCtrl.SetActivate(1);
	}

    return TRUE;
}

BOOL CScanSelectFilesPage::OnSetActive()
{
	// Turn the back button back on
	CPropertySheet* parentSheet = NULL;

	// Disallow back
	parentSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	parentSheet->SetWizardButtons(PSWIZB_NEXT);
	return TRUE;
}

void CScanSelectFilesPage::SetScan( CSavConfigureableScan* newScan )
{
    scan = newScan;
}