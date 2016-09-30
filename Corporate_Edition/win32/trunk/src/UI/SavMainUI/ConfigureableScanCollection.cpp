// ConfigureableScanCollection.cpp : Implementation of CConfigureableScanCollection

#include "stdafx.h"
#include "ConfigureableScanCollection.h"
#include "SavConfigureableScan.h"
#include "ScanSelectFilesPage.h"
#include "UiUtil.h"
#include "ScanWhenToScanPage.h"
#include "ScanWhatToScanPage.h"
#include "ScanSelectFilesPage.h"
#include "ScanOptionsPage.h"
#include "ScanSchedulePage.h"
#include "ScanNamePage.h"
#include "ConfigObj.h"


// IProtection_ConfigureableScan_Container Methods not implemented by ATL
// Deletes a scan from the collection
HRESULT CConfigureableScanCollection::Delete( GUID id )
{
    IProtection_ConfigureableScan*      targetScan          = NULL;
    CSavConfigureableScan*              targetScanActual    = NULL;
    HRESULT                             returnValHR         = E_FAIL;

    returnValHR = get_Item(id, &targetScan);
    if (SUCCEEDED(returnValHR))
    {
        targetScanActual = (CSavConfigureableScan*) targetScan;
        returnValHR = targetScanActual->Delete();
    }
    return returnValHR;
}

HRESULT CConfigureableScanCollection::get_Item( GUID id, IProtection_ConfigureableScan** scan )
{
    ConfigureableScanList::iterator     currScan;
    GUID                                currScanID      = GUID_NULL;
    HRESULT                             returnValHR     = E_FAIL;

    // Validate parameters
    if (scan == NULL)
        return E_POINTER;
    if (id == GUID_NULL)
        return E_INVALIDARG;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // Find the scan
    returnValHR = S_FALSE;
    for (currScan = m_coll.begin(); currScan != m_coll.end(); currScan++)
    {
        if (SUCCEEDED((*currScan)->get_ID(&currScanID)))
        {
            if (currScanID == id)
            {
                *scan = *currScan;
                (*scan)->AddRef();
                returnValHR = S_OK;
                break;
            }
        }
    }
    return returnValHR;
}

HRESULT CConfigureableScanCollection::get__NewEnum( IEnumProtection_ConfigureableScan** enumerator )
{
    typedef std::list<IProtection_ConfigureableScan*> ConfigureableScanList;
    typedef CComEnumOnSTL< IEnumProtection_ConfigureableScan, &IID_IEnumProtection_ConfigureableScan, IProtection_ConfigureableScan*, CopyInterfaceFixed<IProtection_ConfigureableScan>, ConfigureableScanList > CComEnumConfigureableScanOnList;
    CComObject<CComEnumConfigureableScanOnList>*    actualEnumerator    = NULL;
    HRESULT                                         returnValHR         = E_FAIL;

    // Validate parameter
    if (enumerator == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValHR = CComObject<CComEnumConfigureableScanOnList>::CreateInstance(&actualEnumerator);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = actualEnumerator->Init(NULL, m_coll);
        if (SUCCEEDED(returnValHR))
            returnValHR = actualEnumerator->QueryInterface(IID_IEnumProtection_ConfigureableScan, (void**) enumerator);
    }
    return returnValHR;
}

HRESULT CConfigureableScanCollection::ShowAddWizard( HWND parentWindowHandle, IProtection_ConfigureableScan** newScan )
// Returns S_OK on success, S_FALSE if no user cancelled, else the error code of the failure
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // Technically redundant since caller must do this to create their property page, but good practice
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IProtection_ConfigureableScan*		newScanActual			= NULL;
    CSavConfigureableScan*				newScanInternal         = NULL;
    CControlParentSheet                 configWizard;
	CScanWhenToScanPage					whenToScanPage;
	CScanWhatToScanPage					whatToScanPage;
    CScanSelectFilesPage                filesPage;
	CScanOptionsPage					optionsPage;
	CScanSchedulePage					schedulePage;
	CScanNamePage						namePage;
    CWnd                                parentWindow;
    IScanConfig*                        scanConfig				= NULL;
	CString								wizardTitle;
    INT_PTR                             returnValIP				= IDCANCEL;
	DWORD								scanType				= SCAN_TYPE_CUSTOM;
    HRESULT                             returnValHR				= E_FAIL;

	// Create a new temporary scan object and initialize it as a new scan
    returnValHR = CSavConfigureableScan::CreateInstance(&newScanActual);
    if (SUCCEEDED(returnValHR))
    {
        newScanInternal = static_cast<CSavConfigureableScan*>(newScanActual);
		returnValHR = newScanInternal->CreateNew();
		if (SUCCEEDED(returnValHR))
		    scanConfig = newScanInternal->GetScanConfig();
		else
			returnValHR = E_FAIL;
    }

	// Create wizard and pages
	if (scanConfig != NULL)
    {
		CConfigObj			config(scanConfig);

		scanConfig->AddRef();
		whenToScanPage.SetScan(newScanInternal);
		whatToScanPage.SetScan(newScanInternal);
		schedulePage.SetScan(newScanInternal);
		scanType = config.GetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_CUSTOM);
		optionsPage.SetScan(newScanInternal);
		namePage.SetScan(newScanInternal);
		filesPage.SetScan(newScanInternal);

		// Setup wizard
		parentWindow.Attach(parentWindowHandle);
        configWizard.SetParent(&parentWindow);
		wizardTitle = _T("Create new scan");
        configWizard.SetCaption(wizardTitle);
        configWizard.m_psh.dwFlags |= PSH_NOAPPLYNOW;
		configWizard.AddPage(&whenToScanPage);
		configWizard.AddPage(&schedulePage);
		configWizard.AddPage(&whatToScanPage);
		configWizard.AddPage(&filesPage);
		configWizard.AddPage(&optionsPage);
		configWizard.AddPage(&namePage);
		configWizard.SetWizardMode();
		configWizard.m_psh.dwFlags |= PSH_HASHELP | PSH_NOAPPLYNOW;
        returnValIP = configWizard.DoModal();
        parentWindow.Detach();

		if (returnValIP == ID_WIZFINISH)
		{
			try
			{
				// Re-read scan data
				newScanInternal->ReadFromRegistry(NULL, NULL, false);
				m_coll.push_back(newScanActual);
				*newScan = newScanActual;
				returnValHR = S_OK;
			}
			catch (std::bad_alloc&)
			{
				returnValHR = E_OUTOFMEMORY;
				newScanInternal->Delete();
				newScanActual->Release();
				newScanActual = NULL;
				newScanInternal = NULL;
			}
		}
		else
		{
			newScanInternal->Delete();
			returnValHR = S_FALSE;
		}
    }

	if (returnValHR != S_OK)
	{
		if (newScanActual != NULL)
		{
			newScanActual->Release();
			newScanActual = NULL;
			newScanInternal = NULL;
		}
	}
    return returnValHR;
}

// Private APIs
HRESULT CConfigureableScanCollection::AddCurrentUserScans( void )
{
    return AddScansFromKey(HKEY_CURRENT_USER, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_CustomTasks), false);
}

HRESULT CConfigureableScanCollection::AddAdminScans( void )
{
    return AddScansFromKey(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_LocalScans), true);
}

HRESULT CConfigureableScanCollection::AddScansFromKey( HKEY hiveHandle, LPCTSTR keyPath, bool isReadOnly )
{
    HKEY                            scansListKeyHandle      = NULL;
    TCHAR                           currScanKeyName[50]     = _T("");
    DWORD                           currScanNo              = 0;
    IProtection_ConfigureableScan*  newScan             = NULL;
    CSavConfigureableScan*          newScanInternal     = NULL;
    DWORD                           returnValDW             = ERROR_OUT_OF_PAPER;
    HRESULT                         returnValHR             = E_FAIL;

    // Datastore format for user scans:
    // <SAV Main HCU key>\Custom Tasks
    //      <Scan GUID>
    //          <All Scan options EXCEPT description>
    //          SelectedScanType = Full | Quick | Custom | COH
    //      TaskPadScripted
    //          <Scan User Readable Name>
    //              (Default) = <Scan GUID>
    //              ActiveX ID = HARDCODED VALUE
    //              Description = <Scan Description>
    // RTVScan determines Scheduled by this test:  (Schedule\MinOfDay == NO_SCHEDULED_START_TIME) && (!Scheduled\RandomizeDayEnabled && !Schedule\RandomizeWeekEnabled && !Schedule\RandomizeMonthEnabled))
    //      See misc.cpp's GetTimeValues for loading time values
    //
    // If this is a StartupScan, it is also listed here:
    //      TaskPadStartup
    //          <Scan Name>
    //              (Default) = <Scan GUID>
    //              ActiveX ID = HARDCODED VALUE
    //              Description = <Scan Description>
    // If this is a ScheduledScan, it is also listed here:
    //      TaskPadScheduled
    //          <Scan Name>
    //              (Default) = <Scan GUID>
    //              ActiveX ID = HARDCODED VALUE
    //              Description = <Scan Description>
    //
    // Datastore format for admin scheduled scans:
    // <SAV main HLM key>\LocalScans
    //      <Scan GUID>
    //          <All Scan options...>
    //          Schedule
    //              RTVScan determines Scheduled by this test:  (Schedule\MinOfDay == NO_SCHEDULED_START_TIME) && (!Scheduled\RandomizeDayEnabled && !Schedule\RandomizeWeekEnabled && !Schedule\RandomizeMonthEnabled))
    //              See misc.cpp's GetTimeValues for loading time values
    //              Note that Type != 0 is also used in the UI for this
    // Note: SSC has no description UI for admin scheduled scans, only names

    returnValDW = RegOpenKeyEx(hiveHandle, keyPath, NULL, KEY_READ, &scansListKeyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = RegEnumKey(scansListKeyHandle, currScanNo, currScanKeyName, sizeof(currScanKeyName)/sizeof(currScanKeyName[0]));
        while (returnValDW == ERROR_SUCCESS)
        {
            returnValHR = CSavConfigureableScan::CreateInstance(&newScan);
            if (SUCCEEDED(returnValHR))
            {
                newScanInternal = static_cast<CSavConfigureableScan*>(newScan);
                returnValHR = newScanInternal->ReadFromRegistry(scansListKeyHandle, currScanKeyName, isReadOnly);
                if (SUCCEEDED(returnValHR))
                    m_coll.push_back(newScan);
                else
                    newScan->Release();
                newScan = NULL;
                newScanInternal = NULL;
            }
            currScanNo += 1;
            returnValDW = RegEnumKey(scansListKeyHandle, currScanNo, currScanKeyName, sizeof(currScanKeyName)/sizeof(currScanKeyName[0]));
        }

        RegCloseKey(scansListKeyHandle);
        scansListKeyHandle = NULL;
    }
    return HRESULT_FROM_WIN32(returnValDW);
}

// Constructor-destructor
CConfigureableScanCollection::CConfigureableScanCollection()
{
    // Nothing for now
}

HRESULT CConfigureableScanCollection::FinalConstruct()
{
    // Initialize trust checking
    return trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
}

void CConfigureableScanCollection::FinalRelease()
{
    ConfigureableScanList::iterator currScan;
    IProtection_ConfigureableScan* currScanPtr = NULL;

    // Free the scans
    for (currScan = m_coll.begin(); currScan != m_coll.end(); currScan++)
    {
        currScanPtr = *currScan;
		if (currScanPtr != NULL)
		{
	        currScanPtr->Release();
		    currScanPtr = NULL;
		}
    }
    m_coll.clear();
    trustVerifier.Shutdown();
}
