// SymCorpUIScansView.cpp : implementation file
//

#include "stdafx.h"
#include "SymCorpUI.h"
#include "SymCorpUIScansView.h"
#include "enum_iterator.h"
#include "SavMainUI.h"


// CSymCorpUIScansView
BEGIN_MESSAGE_MAP(CSymCorpUIScansView, CViewEx)
    ON_COMMAND(IDC_VIEW_SCANS_QUICK_BUTTON, OnQuickScanBtn)
    ON_COMMAND(IDC_VIEW_SCANS_FULL_BUTTON, OnFullScanBtn)
    ON_COMMAND(IDC_VIEW_SCANS_POLICY_BUTTON, OnPolicyScanBtn)
    ON_COMMAND(IDC_VIEW_SCANS_SCANLIST_CREATE, OnCreateScanBtn)

    ON_NOTIFY(NM_RCLICK, IDC_VIEW_SCANS_SCANLIST, &OnRClickScan)
	ON_NOTIFY(NM_DBLCLK, IDC_VIEW_SCANS_SCANLIST, &OnDoubleClickScan)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSymCorpUIScansView, CViewEx)


CSymCorpUIScansView::CSymCorpUIScansView()
	: CViewEx(CSymCorpUIScansView::IDD, COLOR_WHITE), scans(NULL)
{
    // Nothing needed
}

CSymCorpUIScansView::~CSymCorpUIScansView()
{
    // Release scans
    if (scans != NULL)
    {
        scans->Release();
        scans = NULL;
    }
}

// CSymCorpUIScansView message handlers
void CSymCorpUIScansView::OnDraw( CDC* pDC )
{
    // Nothing for now...
}

void CSymCorpUIScansView::DoDataExchange( CDataExchange* pDX )
{
    DDX_Control(pDX, IDC_VIEW_SCANS_SCANLIST, scanListCtrl);
    DDX_Control(pDX, IDC_VIEW_SCANS_SCANLIST_CREATE, createScanButton);
    // Quick scan block
    DDX_Control(pDX, IDC_VIEW_SCANS_QUICK_TITLE, quickScanTitle);
    DDX_Control(pDX, IDC_VIEW_SCANS_QUICK_GRAPHIC, quickScanGraphic);
    DDX_Control(pDX, IDC_VIEW_SCANS_QUICK_DESCRIPTION, quickScanDescription);
    DDX_Control(pDX, IDC_VIEW_SCANS_QUICK_DURATION, quickScanDurationCaption);
    DDX_Control(pDX, IDC_VIEW_SCANS_QUICK_TIME, quickScanDurationTime);
    DDX_Control(pDX, IDC_VIEW_SCANS_QUICK_BUTTON, quickScanButton);
    // Full scan block
    DDX_Control(pDX, IDC_VIEW_SCANS_FULL_TITLE, fullScanTitle);
    DDX_Control(pDX, IDC_VIEW_SCANS_FULL_GRAPHIC, fullScanGraphic);
    DDX_Control(pDX, IDC_VIEW_SCANS_FULL_DESCRIPTION, fullScanDescription);
    DDX_Control(pDX, IDC_VIEW_SCANS_FULL_DURATION, fullScanDurationCaption);
    DDX_Control(pDX, IDC_VIEW_SCANS_FULL_TIME, fullScanDurationTime);
    DDX_Control(pDX, IDC_VIEW_SCANS_FULL_BUTTON, fullScanButton);
    // Quick scan block
    DDX_Control(pDX, IDC_VIEW_SCANS_POLICY_TITLE, policyScanTitle);
    DDX_Control(pDX, IDC_VIEW_SCANS_POLICY_GRAPHIC, policyScanGraphic);
    DDX_Control(pDX, IDC_VIEW_SCANS_POLICY_DESCRIPTION, policyScanDescription);
    DDX_Control(pDX, IDC_VIEW_SCANS_POLICY_DURATION, policyScanDurationCaption);
    DDX_Control(pDX, IDC_VIEW_SCANS_POLICY_TIME, policyScanDurationTime);
    DDX_Control(pDX, IDC_VIEW_SCANS_POLICY_BUTTON, policyScanButton);

    CViewEx::DoDataExchange(pDX);
}

void CSymCorpUIScansView::OnInitialUpdate()
{
    CViewEx::OnInitialUpdate();
}

void CSymCorpUIScansView::OnUpdate( CView* sender, LPARAM hintParam, CObject* hintObject )
{
    CString         scanListColumnTitleName;
    CString         scanListColumnTitleType;
    CString         scanListColumnTitleLastRun;
    CString         scanListColumnTitleNextRun;
    CString         scanListColumnTitleActions;
    HRESULT         returnValHR                     = E_FAIL;

    // Can't create PTBlockDlg's until we have a window, so don't
    if (!initialUpdateRan)
        return;

    // Quick scan block
    quickScanTitle.fontInfo.LoadFromString(IDS_SCANS_BLOCK_TITLE_FONT);
    //quickScanGraphic.LoadImage();
    quickScanDescription.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DESCRIPTION_FONT);
    quickScanDurationCaption.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DURATION_CAPTION_FONT);
    quickScanDurationTime.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DURATION_TIME_FONT);
    //quickScanButton.LoadImages();
    quickScanButton.defaultFont.LoadFromString(IDS_SCANS_BLOCK_BUTTON_FONT);
    // Full scan block
    fullScanTitle.fontInfo.LoadFromString(IDS_SCANS_BLOCK_TITLE_FONT);
    //fullScanGraphic.LoadImage();
    fullScanDescription.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DESCRIPTION_FONT);
    fullScanDurationCaption.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DURATION_CAPTION_FONT);
    fullScanDurationTime.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DURATION_TIME_FONT);
    //fullScanButton.LoadImages();
    fullScanButton.defaultFont.LoadFromString(IDS_SCANS_BLOCK_BUTTON_FONT);
    // Policy scan block
    policyScanTitle.fontInfo.LoadFromString(IDS_SCANS_BLOCK_TITLE_FONT);
    //policyScanGraphic.LoadImage();
    policyScanDescription.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DESCRIPTION_FONT);
    policyScanDurationCaption.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DURATION_CAPTION_FONT);
    policyScanDurationTime.fontInfo.LoadFromString(IDS_SCANS_BLOCK_DURATION_TIME_FONT);
    //policyScanButton.LoadImages();
    policyScanButton.defaultFont.LoadFromString(IDS_SCANS_BLOCK_BUTTON_FONT);

    // Configure Scan ListView control
    scanListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    // Create columns
    scanListColumnTitleName.LoadString(IDS_SCANS_LISTTITLE_NAME);
    scanListColumnTitleType.LoadString(IDS_SCANS_LISTTITLE_TYPE);
    scanListColumnTitleLastRun.LoadString(IDS_SCANS_LISTTITLE_LASTRUN);
    scanListColumnTitleNextRun.LoadString(IDS_SCANS_LISTTITLE_NEXTRUN);
    scanListColumnTitleActions.LoadString(IDS_SCANS_LISTTITLE_ACTIONS);
    scanListCtrl.InsertColumn(ScanListColumn_Name, scanListColumnTitleName, LVCFMT_LEFT, 175);
    scanListCtrl.InsertColumn(ScanListColumn_Type, scanListColumnTitleType, LVCFMT_LEFT, 100);
    scanListCtrl.InsertColumn(ScanListColumn_LastRun, scanListColumnTitleLastRun, LVCFMT_LEFT, 150);
    scanListCtrl.InsertColumn(ScanListColumn_NextRun, scanListColumnTitleNextRun, LVCFMT_LEFT, 150);
    scanListCtrl.InsertColumn(ScanListColumn_Actions, scanListColumnTitleActions, LVCFMT_LEFT, 120);

    // Get the list of scans
    CSymCorpUIDoc*                                              currDocument                    = NULL;
    ProtectionProviderLib::IEnumProtection_ConfigureableScan*   scanEnumerator                  = NULL;
    ProtectionProviderLib::ProtectionProviderList::iterator     currProtectionProvider;

    // Release existing scans
    if (scans != NULL)
    {
        scans->Release();
        scans = NULL;
    }
    // Get the new scans from the 1st provider we find that supports them
    currDocument = GetDocument();
    if (currDocument != NULL)
    {
        for (currProtectionProvider = currDocument->protectionProviders.begin(); currProtectionProvider != currDocument->protectionProviders.end(); currProtectionProvider++)
        {
            returnValHR = (*currProtectionProvider)->get_ConfigureableScans(&scans);
            if (SUCCEEDED(returnValHR))
                break;
        }
    }

    // Add the scans to the ListView
    scanListCtrl.DeleteAllItems();
    if (scans != NULL)
        scans->get__NewEnum(&scanEnumerator);
    if (scanEnumerator != NULL)
    {
        typedef enum_iterator<IEnumProtection_ConfigureableScan, &IID_IEnumProtection_ConfigureableScan, IProtection_ConfigureableScan*>  ConfigureableScanIterator;
        ConfigureableScanIterator scansBegin(scanEnumerator);
        ConfigureableScanIterator scansEnd;
        ConfigureableScanIterator currScan;

        // Note:  cast required due to problem using enum_iterator with IProtection types in the namespace.
        for(currScan = scansBegin; currScan != scansEnd; currScan++)
            AddScanToListView(reinterpret_cast<ProtectionProviderLib::IProtection_ConfigureableScan*>(*currScan));

        scanEnumerator->Release();
    }
}

HRESULT CSymCorpUIScansView::AddScanToListView( ProtectionProviderLib::IProtection_ConfigureableScan* scanToAdd )
{
	int							newScanItemNo					= 0;
    CComBSTR                    scanName;
    HRESULT                     returnValHR                     = E_FAIL;

    // Validate parameters
    if (scanToAdd == NULL)
        return E_POINTER;

    // Get scan information
    scanToAdd->get_Name(&scanName);
    newScanItemNo = scanListCtrl.InsertItem(LVIF_TEXT | LVIF_PARAM, 100, static_cast<LPCTSTR>(scanName), NULL, NULL, 0, reinterpret_cast<LPARAM>(scanToAdd));
	return UpdateScanInListView(scanToAdd);
}

HRESULT CSymCorpUIScansView::UpdateScanInListView( ProtectionProviderLib::IProtection_ConfigureableScan* scanToUpdate )
// Update the data for the specified scan in the listview
{
    int                         currItemNo						= 0;
	bool						foundScan						= false;
    CComBSTR                    scanName;
    CComBSTR                    scanDescription;
    SYSTEMTIME                  scanLastRunTime                 = {0};
    SYSTEMTIME                  scanNextRunTime                 = {0};
    VARIANT_BOOL                scanReadOnly                    = false;
    VARIANT_BOOL                scanSupportsWindows             = FALSE;
    unsigned int                scanWindowSize                  = 0;
    TCHAR                       dateBuffer[100]                 = _T("");
    TCHAR                       timeBuffer[100]                 = _T("");
    CString                     dateTimeString;
    CString                     nextRunString;
    ProtectionProviderLib::StartType                                    scanStartType           = ProtectionProviderLib::StartType_OnDemand;
    CString                     scanStartTypeString;
    ProtectionProviderLib::IProtection_ConfigureableScan_ScheduledPtr   currScheduledScan;
    HRESULT                     returnValHR                     = E_FAIL;

    // Validate parameters
    if (scanToUpdate == NULL)
        return E_POINTER;

	// Locate the scan in the listview
	for (currItemNo = 0; (currItemNo < scanListCtrl.GetItemCount()) && !foundScan; currItemNo++)
	{
		if (reinterpret_cast<ProtectionProviderLib::IProtection_ConfigureableScan*>(scanListCtrl.GetItemData(currItemNo)) == scanToUpdate)
		{
			foundScan = true;
			break;
		}
	}
	if (!foundScan)
		return S_FALSE;

	// Get scan information
    scanToUpdate->get_Name(&scanName);
    scanToUpdate->get_Description(&scanDescription);
    scanToUpdate->get_LastRunTime(reinterpret_cast<ProtectionProviderLib::_SYSTEMTIME*>(&scanLastRunTime));
    scanToUpdate->get_ReadOnly(&scanReadOnly);
    scanToUpdate->get_StartType(&scanStartType);
    // Get schedule information
    scanNextRunTime = SYSTEMTIME_NULL;
    scanSupportsWindows = FALSE;
    scanWindowSize = 0;
    currScheduledScan = scanToUpdate;
    if (currScheduledScan != NULL)
    {
        currScheduledScan->GetNextRunTime(reinterpret_cast<ProtectionProviderLib::_SYSTEMTIME*>(&scanNextRunTime), &scanSupportsWindows, &scanWindowSize);
        currScheduledScan.Release();
    }

    // Determine display strings
    scanStartTypeString.LoadString(IDS_SCANS_TYPE_ONDEMAND);
    if (scanStartType == StartType_Startup)
        scanStartTypeString.LoadString(IDS_SCANS_TYPE_STARTUP);
    else if (scanStartType == StartType_Scheduled)
        scanStartTypeString.LoadString(IDS_SCANS_TYPE_SCHEDULED);
    if (memcmp(&scanLastRunTime, &SYSTEMTIME_NULL, sizeof(SYSTEMTIME)) != 0)
    {
        GetDateFormat(NULL, DATE_SHORTDATE, &scanLastRunTime, NULL, dateBuffer, sizeof(dateBuffer)/sizeof(dateBuffer[0]));
        GetTimeFormat(NULL, TIME_NOSECONDS, &scanLastRunTime, NULL, timeBuffer, sizeof(timeBuffer)/sizeof(timeBuffer[0]));
        dateTimeString.Format(_T("%s %s"), dateBuffer, timeBuffer);
    }
    else
    {
        dateTimeString = _T("--");
    }
    if (memcmp(&scanNextRunTime, &SYSTEMTIME_NULL, sizeof(SYSTEMTIME)) != 0)
    {
        GetDateFormat(NULL, DATE_SHORTDATE, &scanNextRunTime, NULL, dateBuffer, sizeof(dateBuffer)/sizeof(dateBuffer[0]));
        GetTimeFormat(NULL, TIME_NOSECONDS, &scanNextRunTime, NULL, timeBuffer, sizeof(timeBuffer)/sizeof(timeBuffer[0]));
        nextRunString.Format(_T("%s %s"), dateBuffer, timeBuffer);
    }
    else
    {
        nextRunString = _T("--");
    }

    // Update the scan items
    scanListCtrl.SetItem(currItemNo, ScanListColumn_Name, LVIF_TEXT, (LPCTSTR) scanName, NULL, NULL, NULL, NULL);
	scanListCtrl.SetItem(currItemNo, ScanListColumn_Type, LVIF_TEXT, scanStartTypeString, NULL, NULL, NULL, NULL);
    scanListCtrl.SetItem(currItemNo, ScanListColumn_LastRun, LVIF_TEXT, dateTimeString, NULL, NULL, NULL, NULL);
    scanListCtrl.SetItem(currItemNo, ScanListColumn_NextRun, LVIF_TEXT, nextRunString, NULL, NULL, NULL, NULL);

    return S_OK;
}


void CSymCorpUIScansView::OnQuickScanBtn()
{
    CSymCorpUIDoc*                                              currDocument                = NULL;
    ProtectionProviderLib::ProtectionProviderList::iterator     currProtectionProvider;
    ISavProtectionProvider*                                     savProvider                 = NULL;
    DWORD                                                       savErrorCode                = ERROR_OUT_OF_PAPER;
    CString                                                     errorTitle;
    CString                                                     errorMessage;
    HRESULT                                                     returnValHR                 = E_FAIL;

    // Get the new scans from the 1st provider we find that supports them
    currDocument = GetDocument();
    if (currDocument != NULL)
    {
        for (currProtectionProvider = currDocument->protectionProviders.begin(); currProtectionProvider != currDocument->protectionProviders.end(); currProtectionProvider++)
        {
            (*currProtectionProvider)->QueryInterface(IID_ISavProtectionProvider, (void**) &savProvider);
            if (savProvider != NULL)
            {
                returnValHR = savProvider->RunQuickScan(GetSafeHwnd(), &savErrorCode);
                if (FAILED(returnValHR))
                {
                    CString errorTitle;
                    CString errorDescription;

                    errorTitle.LoadString(IDS_SCAN_SCANERROR_TITLE);
                    errorDescription.FormatMessage(IDS_SCAN_SCANERROR_DESCRIPTION, savErrorCode);
                    MessageBox(errorDescription, errorTitle, MB_OK | MB_ICONERROR);
                }
                savProvider->Release();
                savProvider = NULL;
                break;
            }
        }
    }
}

void CSymCorpUIScansView::OnFullScanBtn()
{
    CSymCorpUIDoc*                                              currDocument                = NULL;
    ProtectionProviderLib::ProtectionProviderList::iterator     currProtectionProvider;
    ISavProtectionProvider*                                     savProvider                 = NULL;
    DWORD                                                       savErrorCode                = ERROR_OUT_OF_PAPER;
    CString                                                     errorTitle;
    CString                                                     errorMessage;
    HRESULT                                                     returnValHR                 = E_FAIL;

    // Get the new scans from the 1st provider we find that supports them
    currDocument = GetDocument();
    if (currDocument != NULL)
    {
        for (currProtectionProvider = currDocument->protectionProviders.begin(); currProtectionProvider != currDocument->protectionProviders.end(); currProtectionProvider++)
        {
            (*currProtectionProvider)->QueryInterface(IID_ISavProtectionProvider, (void**) &savProvider);
            if (savProvider != NULL)
            {
                returnValHR = savProvider->RunFullScan(GetSafeHwnd(), &savErrorCode);
                if (FAILED(returnValHR))
                {
                    CString errorTitle;
                    CString errorDescription;

                    errorTitle.LoadString(IDS_SCAN_SCANERROR_TITLE);
                    errorDescription.FormatMessage(IDS_SCAN_SCANERROR_DESCRIPTION, savErrorCode);
                    MessageBox(errorDescription, errorTitle, MB_OK | MB_ICONERROR);
                }
                savProvider->Release();
                savProvider = NULL;
                break;
            }
        }
    }
}

void CSymCorpUIScansView::OnPolicyScanBtn()
{
    MessageBox(_T("Do policy scan."), _T("Policy Scan"));
}

void CSymCorpUIScansView::OnRClickScan( NMHDR* notifyInfo, LRESULT* result )
{
	LPNMITEMACTIVATE        notifyInfoLV		    = reinterpret_cast<LPNMITEMACTIVATE>(notifyInfo);
    ProtectionProviderLib::IProtection_ConfigureableScan*   thisScan            = NULL;
    CMenu                   scanMenu;
    POINT		            cursorPosition          = {0};
    DWORD                   selectionID             = 0;
    VARIANT_BOOL            isReadOnly              = FALSE;
    GUID                    scanID                  = GUID_NULL;
    CString                 messageTitle;
    CString                 messageDescription;
	CComBSTR				scanName;
    HRESULT                 returnValHR             = E_FAIL;

	if (notifyInfoLV->iItem != -1)
	    thisScan = reinterpret_cast<ProtectionProviderLib::IProtection_ConfigureableScan*>(scanListCtrl.GetItemData(notifyInfoLV->iItem));
	if (thisScan != NULL)
    {
        scanMenu.LoadMenu(IDR_SCAN_OPTIONS);
        ZeroMemory(&cursorPosition, sizeof(cursorPosition));
		GetCursorPos(&cursorPosition);
        selectionID = scanMenu.GetSubMenu(0)->TrackPopupMenu(TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_CENTERALIGN, cursorPosition.x, cursorPosition.y, this);
        switch (selectionID)
        {
        case IDR_SCAN_OPTIONS_SCANNOW:
            thisScan->RunScan(reinterpret_cast<wireHWND>(GetSafeHwnd()));
            break;
        case IDR_SCAN_OPTIONS_EDIT:
            thisScan->ShowConfigureUI(reinterpret_cast<wireHWND>(GetSafeHwnd()));
			UpdateScanInListView(thisScan);
            break;
        case IDR_SCAN_OPTIONS_DELETE:
            thisScan->get_ReadOnly(&isReadOnly);
            if (!isReadOnly)
            {
                messageTitle.LoadString(IDS_SCANS_CONFIRMDELETE_TITLE);
                messageDescription.LoadString(IDS_SCANS_CONFIRMDELETE_MESSAGE);
                if (MessageBox(messageDescription, messageTitle, MB_YESNO | MB_ICONINFORMATION) == IDYES)
                {
                    thisScan->get_ID(&scanID);
                    returnValHR = scans->Delete(scanID);
                    if (SUCCEEDED(returnValHR))
                    {
                        scanListCtrl.DeleteItem(notifyInfoLV->iItem);
                    }
                    else
                    {
                        messageTitle.LoadString(IDS_SCANS_ERROR_DELETING_TITLE);
                        messageDescription.FormatMessage(IDS_SCANS_ERROR_DELETING_MESSAGE, returnValHR);
                        MessageBox(messageDescription, messageTitle, MB_OK | MB_ICONSTOP);
                    }
                }
            }
            else
            {
                messageTitle.LoadString(IDS_SCANS_ERROR_READONLY_TITLE);
                messageDescription.FormatMessage(IDS_SCANS_ERROR_READONLY_MESSAGE, returnValHR);
                MessageBox(messageDescription, messageTitle, MB_OK | MB_ICONSTOP);
            }
            break;
        case IDR_SCAN_OPTIONS_VIEWLOG:
            thisScan->ShowLogUI(reinterpret_cast<wireHWND>(GetSafeHwnd()));
            break;
        }
    }
}

void CSymCorpUIScansView::OnDoubleClickScan( NMHDR* notifyInfo, LRESULT* result )
{
	LPNMITEMACTIVATE        notifyInfoLV		    = reinterpret_cast<LPNMITEMACTIVATE>(notifyInfo);
    ProtectionProviderLib::IProtection_ConfigureableScan*   thisScan            = NULL;

	if (notifyInfoLV->iItem != -1)
	    thisScan = reinterpret_cast<ProtectionProviderLib::IProtection_ConfigureableScan*>(scanListCtrl.GetItemData(notifyInfoLV->iItem));
	if (thisScan != NULL)
	{
        thisScan->ShowConfigureUI(reinterpret_cast<wireHWND>(GetSafeHwnd()));
		UpdateScanInListView(thisScan);
	}
}

void CSymCorpUIScansView::OnCreateScanBtn()
{
    ProtectionProviderLib::IProtection_ConfigureableScan*	newScan = NULL;

    if (scans != NULL)
    {
        scans->ShowAddWizard(reinterpret_cast<wireHWND>(GetSafeHwnd()), &newScan);
        if (newScan != NULL)
            AddScanToListView(newScan);
    }
}