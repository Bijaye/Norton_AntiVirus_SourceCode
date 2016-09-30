// ScanSchedulePage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "ScanSchedulePage.h"
#include "SymSaferRegistry.h"


// CScanSchedulePage dialog
IMPLEMENT_DYNAMIC(CScanSchedulePage, CPropertyPage)

BEGIN_MESSAGE_MAP(CScanSchedulePage, CPropertyPage)
END_MESSAGE_MAP()


CScanSchedulePage::CScanSchedulePage()
: CPropertyPage(CScanSchedulePage::IDD), scan(NULL), useResumableScan(false), scheduleControlInitialized(false), scheduleCtrl(NULL), scheduleResumeableScanCtrl(NULL)
{
	HKEY		keyHandle		= NULL;
	DWORD		valueType		= REG_DWORD;
	DWORD		valueData		= 0;
	DWORD		valueDataSize	= 0;
	DWORD		returnValDW		= ERROR_FILE_NOT_FOUND;

	returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_ProductControl), NULL, KEY_READ, &keyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
		valueDataSize = sizeof(valueData);
		returnValDW = SymSaferRegQueryValueEx(keyHandle, _T(szReg_Val_Resumption_Enable), NULL, &valueType, (LPBYTE) &valueData, &valueDataSize);
		if ((returnValDW == ERROR_SUCCESS) && (valueType == REG_DWORD))
			useResumableScan = (valueData != 0);
		RegCloseKey(keyHandle);
		keyHandle = NULL;
	}
	if (useResumableScan)
		scheduleResumeableScanCtrl = new CLDVPSchedule2();
	else
		scheduleCtrl = new CLDVPSchedule();
}

CScanSchedulePage::~CScanSchedulePage()
{
	if (scheduleCtrl)
	{
		delete scheduleCtrl;
		scheduleCtrl = NULL;
	}
	if (scheduleResumeableScanCtrl)
	{
		delete scheduleResumeableScanCtrl;
		scheduleResumeableScanCtrl = NULL;
	}
}

void CScanSchedulePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	if (scheduleControlInitialized)
	{
		if (useResumableScan)
			DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_SCHEDULE_SCHEDULECTRL, *scheduleResumeableScanCtrl);
		else
			DDX_Control(pDX, IDC_SCANS_CONFIGPAGE_SCHEDULE_SCHEDULECTRL, *scheduleCtrl);
	}
}

// CScanSchedulePage message handlers
void CScanSchedulePage::OnOK()
{
	StartType		startupType		= StartType_OnDemand;

	// This page is used on both the edit and add wizards.  On the Add wizard, 
	// the user may not have selected a scheduled scan - if not, don't save schedule data.
	scan->get_StartType(&startupType);
	if (startupType == StartType_Scheduled)
	{
		if (useResumableScan)
			scheduleResumeableScanCtrl->Store(scan->GetScanConfig());
		else
			scheduleCtrl->Store(scan->GetScanConfig());
	}
}

BOOL CScanSchedulePage::OnInitDialog()
{
    CWnd* placeholderCtrl	= NULL;
    CRect placeholderRect;

	// Create controls
    CDialog::OnInitDialog();

	// Create scan config
	placeholderCtrl = GetDlgItem(IDC_SCANS_CONFIGPAGE_SCHEDULE_SCHEDULECTRL);
	if (placeholderCtrl != NULL)
	{
		placeholderCtrl->GetWindowRect(&placeholderRect);
		ScreenToClient(&placeholderRect);
		placeholderCtrl->DestroyWindow();
	}
    if (useResumableScan)
    {
		scheduleResumeableScanCtrl->Create(_T("schedule"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_EX_CONTROLPARENT, placeholderRect, this, IDC_SCANS_CONFIGPAGE_SCHEDULE_SCHEDULECTRL);
        scheduleResumeableScanCtrl->Load(scan->GetScanConfig());
    }
    else
    {
		scheduleCtrl->Create(_T("schedule"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_EX_CONTROLPARENT, placeholderRect, this, IDC_SCANS_CONFIGPAGE_SCHEDULE_SCHEDULECTRL);
        scheduleCtrl->Load(scan->GetScanConfig());
    }
	scheduleControlInitialized = TRUE;

	return TRUE;
}

BOOL CScanSchedulePage::OnSetActive()
{
	// Turn off the back button
	CPropertySheet* parentSheet = NULL;

	// Disallow back
	parentSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	parentSheet->SetWizardButtons(PSWIZB_NEXT);
	return TRUE;
}

void CScanSchedulePage::SetScan( CSavConfigureableScan* newScan )
{
    scan = newScan;
}