// TestHarnessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SymCorpUI.h"
#include "TestHarnessDlg.h"
#include "TrustUtil.h"

// CTestHarnessDlg dialog
IMPLEMENT_DYNAMIC(CTestHarnessDlg, CDialog)

BEGIN_MESSAGE_MAP(CTestHarnessDlg, CDialog)
    ON_BN_CLICKED(IDB_CONFIGURE, &CTestHarnessDlg::OnBnClickedSnapinConfigure)
    ON_BN_CLICKED(IDB_VIEWLOGS, &CTestHarnessDlg::OnBnClickedSnapinViewlogs)
    ON_BN_CLICKED(IDB_EXIT, &CTestHarnessDlg::OnBnClickedExit)
    ON_BN_CLICKED(IDB_REFRESH, &CTestHarnessDlg::OnBnClickedRefreshSnapinInfo)
    ON_BN_CLICKED(IDB_TEST1, &CTestHarnessDlg::OnBnClickedTest1)
    ON_BN_CLICKED(IDB_TEST2, &CTestHarnessDlg::OnBnClickedTest2)
    ON_BN_CLICKED(IDB_TEST3, &CTestHarnessDlg::OnBnClickedTest3)
    ON_BN_CLICKED(IDB_TEST4, &CTestHarnessDlg::OnBnClickedTest4)
    ON_NOTIFY(NM_RCLICK, IDLV_AUTOPROTECTS, &CTestHarnessDlg::OnRClickAutoprotects)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDLV_PROTECTIONTECHS2, &CTestHarnessDlg::OnLvnItemActivateProtectionTechnologies)
END_MESSAGE_MAP()

// Constructor-destructor
CTestHarnessDlg::CTestHarnessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestHarnessDlg::IDD, pParent)
{
    // Nothing for now
}

CTestHarnessDlg::~CTestHarnessDlg()
{
    // Nothing for now
}

// Dialog routines
void CTestHarnessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDLV_PROTECTIONTECHS2, protectionTechnologiesCtrl);
	DDX_Control(pDX, IDLV_AUTOPROTECTS, autoprotectsCtrl);
	DDX_Control(pDX, IDCB_SUPPORTSBASICSCAN, supportsBasicScan);
	DDX_Control(pDX, IDCB_SUPPORTSCONFIGSCAN, supportsConfigureableScans);
}

BOOL CTestHarnessDlg::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CDialog::OnInitDialog();

	// Initialize listviews
	CTrustVerifier													trustVerifier;
	ProtectionProviderLib::ProtectionProviderList::iterator			currProtectionProvider;
	ProtectionProviderLib::IProtectionPtr							currProtectionProviderPtr;
	CComBSTR														shortDisplayName;
	TCHAR															errorMessage[2*MAX_PATH+1]	= _T("");
#ifdef _DEBUG
	CLSIDList::size_type											untrustedNoComponents		= 0;
#endif
	HRESULT															returnValHR					= E_FAIL;

	protectionTechnologiesCtrl.InsertColumn(ProtectionTechnologiesColumn_Name, _T("Name"), LVCFMT_LEFT, 200);
	protectionTechnologiesCtrl.InsertColumn(ProtectionTechnologiesColumn_Status, _T("Status"), LVCFMT_LEFT, 75);

	autoprotectsCtrl.InsertColumn(AutoprotectsColumn_Name, _T("Name"), LVCFMT_LEFT, 100);
	autoprotectsCtrl.InsertColumn(AutoprotectsColumn_Status, _T("Status"), LVCFMT_LEFT, 200);

    // Trust verify all ProtectionProvider components
	// NOTE:  In debug builds, it takes 500ms to create a trust loader, but in Release builds it's quite
	// fast.
	returnValHR = trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
	if (SYM_SUCCEEDED(returnValHR))
	{
#pragma message("*** COMCAT adds 0.5 - 2 seconds here, reimplement without it.")
		returnValHR = EnumCategoryComponents(CATID_ProtectionProvider, &protectionTechnologiesIDs);
		if (SUCCEEDED(returnValHR))
		{
	#ifdef _DEBUG
			untrustedNoComponents = protectionTechnologiesIDs.size();
	#endif
            trustVerifier.TrustVerifyComponentList(&protectionTechnologiesIDs);
			trustVerifier.Shutdown();
		}
        else
        {
            wsprintf(errorMessage, _T("Error 0x%08x enumerating protection technologies"), returnValHR);
            MessageBox(errorMessage, _T("Error"));
        }
	}
	else
	{
		wsprintf(errorMessage, _T("Error 0x%08x creating trust verifier."), returnValHR);
		MessageBox(errorMessage, _T("Trust Verifier Error"));
	}

    // Create the protection providers
    if (SUCCEEDED(returnValHR))
    {
#ifdef _DEBUG
        if (protectionTechnologiesIDs.size() != untrustedNoComponents)
        {
            if (protectionTechnologiesIDs.size() == 0)
                MessageBox(_T("All protection providers failed digital signature validation.  Did you remember to turn ccVerifyTrust off?"), _T("Signature Verification Failure"));
            else
                MessageBox(_T("Some protection providers failed digital signature validation.  Did you remember to turn ccVerifyTrust off?"), _T("Signature Verification Failure"));
        }
#endif // _DEBUG
        returnValHR = CreateComponents<ProtectionProviderLib::IProtection_Provider>(protectionTechnologiesIDs, &protectionTechnologies);
    }

    // Add providers to the list
	if (SUCCEEDED(returnValHR))
	{
		for (currProtectionProvider = protectionTechnologies.begin(); currProtectionProvider != protectionTechnologies.end(); currProtectionProvider++)
		{
			currProtectionProviderPtr = *currProtectionProvider;
			if (currProtectionProviderPtr != NULL)
				returnValHR = currProtectionProviderPtr->get_DisplayName(&shortDisplayName);
			else
				returnValHR = E_NOINTERFACE;
			if (SUCCEEDED(returnValHR))
			{
				protectionTechnologiesCtrl.InsertItem(LVIF_TEXT | LVIF_PARAM, 100, (LPCTSTR) shortDisplayName, NULL, NULL, 0, reinterpret_cast<LPARAM>(*currProtectionProvider));
			}
			else
			{
				wsprintf(errorMessage, _T("Error 0x%08x getting short display name from component."), returnValHR);
				MessageBox(errorMessage, _T("Error"));
			}
			shortDisplayName.Empty();
		}

		// Select the first item in the list
		if (protectionTechnologies.size() > 0)
		{
			protectionTechnologiesCtrl.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
			ShowProtectionProviderInfo(*protectionTechnologies.begin());
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestHarnessDlg::PostNcDestroy()
{
	// Release protection technologies
	ProtectionProviderLib::ProtectionProviderList::iterator currProtectionProvider;
	for (currProtectionProvider = protectionTechnologies.begin(); currProtectionProvider != protectionTechnologies.end(); currProtectionProvider++)
		(*currProtectionProvider)->Release();
	protectionTechnologies.clear();

	// Release autoprotects
	ProtectionProviderLib::ProtectionList::iterator currAutoprotect;
	for (currAutoprotect = autoprotects.begin(); currAutoprotect != autoprotects.end(); currAutoprotect++)
		(*currAutoprotect)->Release();
	autoprotects.clear();
}

void CTestHarnessDlg::OnOK()
{
    // Do nothing, instead of closing the dialog
}

void CTestHarnessDlg::OnBnClickedSnapinConfigure()
{
	ProtectionProviderLib::IProtection_Provider*	snapIn	= NULL;
	ProtectionProviderLib::IProtectionPtr			snapInProtectionPtr;

	snapIn = GetSelectedProtectionProvider();
	if (snapIn != NULL)
		snapInProtectionPtr = snapIn;
	if (snapInProtectionPtr != NULL)
		snapInProtectionPtr->ShowConfigureUI((wireHWND) GetSafeHwnd());
}

void CTestHarnessDlg::OnBnClickedSnapinViewlogs()
{
	ProtectionProviderLib::IProtection_Provider*	snapIn	= NULL;
	ProtectionProviderLib::IProtectionPtr			snapInProtectionPtr;

	snapIn = GetSelectedProtectionProvider();
	if (snapIn != NULL)
		snapInProtectionPtr = snapIn;
	if (snapInProtectionPtr != NULL)
		snapInProtectionPtr->ShowLogUI((wireHWND) GetSafeHwnd());
}

void CTestHarnessDlg::OnBnClickedExit()
{
	EndDialog(ERROR_SUCCESS);
}

void CTestHarnessDlg::OnBnClickedRefreshSnapinInfo()
{
	ShowProtectionProviderInfo(GetSelectedProtectionProvider());
}

void CTestHarnessDlg::OnBnClickedTest1()
{
    // TODO: Add your control notification handler code here
}

void CTestHarnessDlg::OnBnClickedTest2()
{
    // TODO: Add your control notification handler code here
}

void CTestHarnessDlg::OnBnClickedTest3()
{
    // TODO: Add your control notification handler code here
}

void CTestHarnessDlg::OnBnClickedTest4()
{
    // TODO: Add your control notification handler code here
}

void CTestHarnessDlg::OnRClickAutoprotects( NMHDR* notifyInfo, LRESULT* result )
{
	LPNMITEMACTIVATE										notifyInfoLV		= reinterpret_cast<LPNMITEMACTIVATE>(notifyInfo);
	ProtectionProviderLib::IProtection*						autoprotect			= NULL;
	CMenu													selectionMenu;

	autoprotect = reinterpret_cast<ProtectionProviderLib::IProtection*>(autoprotectsCtrl.GetItemData(notifyInfoLV->iItem));
	if (autoprotect != NULL)
	{
		VARIANT_BOOL		apEnabled		= false;
		MENUITEMINFO		itemInfo;
		POINT				cursorPosition;
		DWORD				selectionID		= 0;
		HRESULT				returnValHR		= E_FAIL;
		// Windows returns 0 if user clicks off, so don't use 0
		const UINT MenuItem_Enable              = 1;
		const UINT MenuItem_Disable             = 2;
        const UINT MenuItem_ShowConfigure       = 3;
        const UINT MenuItem_ShowLogs            = 4;

		autoprotect->get_ProtectionConfiguration(&apEnabled);

		// Create the menu
        // General item defaults
		selectionMenu.CreatePopupMenu();
		ZeroMemory(&itemInfo, sizeof(itemInfo));
		itemInfo.cbSize			= sizeof(itemInfo);
		itemInfo.fMask			= MIIM_ID | MIIM_CHECKMARKS | MIIM_STRING | MIIM_FTYPE | MIIM_STATE;
		itemInfo.fType			= MFT_STRING;
		itemInfo.fState			= MFS_ENABLED;
		itemInfo.wID			= 0;
		itemInfo.dwTypeData		= NULL;

        // Item:  Enable
		if (apEnabled)
			itemInfo.fState		= MFS_CHECKED | MFS_DISABLED;
		else
			itemInfo.fState		= MFS_UNCHECKED | MFS_ENABLED;
		itemInfo.wID			= MenuItem_Enable;
		itemInfo.dwTypeData		= _T("Enable");
		selectionMenu.InsertMenuItem(itemInfo.wID, &itemInfo, itemInfo.wID);
        // Item:  Disable
		if (!apEnabled)
			itemInfo.fState		= MFS_CHECKED | MFS_DISABLED;
		else
			itemInfo.fState		= MFS_UNCHECKED | MFS_ENABLED;
		itemInfo.wID			= MenuItem_Disable;
		itemInfo.dwTypeData		= _T("Disable");
		selectionMenu.InsertMenuItem(itemInfo.wID, &itemInfo, itemInfo.wID);
        // Item:  Configure
        itemInfo.fState		    = MFS_ENABLED;
		itemInfo.wID			= MenuItem_ShowConfigure;
		itemInfo.dwTypeData		= _T("Configure");
		selectionMenu.InsertMenuItem(itemInfo.wID, &itemInfo, itemInfo.wID);
        // Item:  Logs
        itemInfo.fState		    = MFS_ENABLED;
		itemInfo.wID			= MenuItem_ShowLogs;
		itemInfo.dwTypeData		= _T("Logs");
		selectionMenu.InsertMenuItem(itemInfo.wID, &itemInfo, itemInfo.wID);
        
		// Display the menu
		ZeroMemory(&cursorPosition, sizeof(cursorPosition));
		GetCursorPos(&cursorPosition);
		selectionID = selectionMenu.TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON, cursorPosition.x, cursorPosition.y, this, NULL);
		
		returnValHR = S_OK;
        switch (selectionID)
        {
        case MenuItem_Disable:
			if (apEnabled)
				returnValHR = autoprotect->put_ProtectionConfiguration(FALSE);
            break;
        case MenuItem_Enable:
			if (!apEnabled)
				returnValHR = autoprotect->put_ProtectionConfiguration(TRUE);
            break;
        case MenuItem_ShowLogs:
            autoprotect->ShowLogUI(reinterpret_cast<wireHWND>(GetSafeHwnd()));
            break;
        case MenuItem_ShowConfigure:
            autoprotect->ShowConfigureUI(reinterpret_cast<wireHWND>(GetSafeHwnd()));
            break;
        }

		ShowProtectionProviderInfo(GetSelectedProtectionProvider());
		if (FAILED(returnValHR))
		{
			TCHAR userMessage[2*MAX_PATH] = _T("");
			wsprintf(userMessage, _T("Error 0x%08x reconfiguring autoprotect."), returnValHR);
			MessageBox(userMessage, _T("Error"));
		}
	}

	*result = 0;
}

void CTestHarnessDlg::OnLvnItemActivateProtectionTechnologies( NMHDR* notifyInfo, LRESULT* result )
{
	LPNMITEMACTIVATE notifyInfoLV = reinterpret_cast<LPNMITEMACTIVATE>(notifyInfo);
	
	ShowProtectionProviderInfo(GetSelectedProtectionProvider());

	*result = 0;
}

// Helpers
HRESULT CTestHarnessDlg::ShowProtectionProviderInfo( ProtectionProviderLib::IProtection_Provider* protectionProvider )
{
	ProtectionProviderLib::IProtection_ContainerPtr							autoprotectContainer;
	ProtectionProviderLib::IEnumProtectionPtr								autoprotectEnumerator;
	ProtectionProviderLib::IProtection*										autoprotect;
	ProtectionProviderLib::IProtection_ConfigureableScan_ContainerPtr		configureableScans;
	ProtectionProviderLib::IProtection_DefinitionInfoPtr					defInfo;
	ProtectionProviderLib::IProtection_ScanPtr								simpleScan;
	CComBSTR																autoprotectName;
	ULONG																	noFetched					= 0;
	ProtectionProviderLib::ProtectionStatus									protectionStatus			= ProtectionProviderLib::ProtectionStatus_Error;
	int																		itemNo						= 0;
	TCHAR																	errorMessage[2*MAX_PATH]	= _T("");
	CString																	apStatus;
	HRESULT																	returnValHR					= E_FAIL;

	// *** DML TEMP DEV
	CSimplePerformanceTimer timer;
	CSimplePerformanceTimer apStuff;
	CSimplePerformanceTimer defUtils;
	timer.StartTimer();
	// ***
	// Validate parameters
	if (protectionProvider == NULL)
		return E_POINTER;

	// Release autoprotects
	ProtectionProviderLib::ProtectionList::iterator currAutoprotect;
	for (currAutoprotect = autoprotects.begin(); currAutoprotect != autoprotects.end(); currAutoprotect++)
		(*currAutoprotect)->Release();
	autoprotects.clear();

	// Find autoprotects
	// *** DML TEMP DEV
	apStuff.StartTimer();
	// ***
	autoprotectsCtrl.DeleteAllItems();
	protectionProvider->get_Autoprotects(&autoprotectContainer);
	if (autoprotectContainer != NULL)
	{
		returnValHR = autoprotectContainer->get__NewEnum(&autoprotectEnumerator);
		if (SUCCEEDED(returnValHR))
		{
			while (autoprotectEnumerator->RemoteNext(1, &autoprotect, &noFetched) == S_OK)
			{
				autoprotects.push_back(autoprotect);
				returnValHR = autoprotect->get_DisplayName(&autoprotectName);
				if (SUCCEEDED(returnValHR))
				{
					itemNo = autoprotectsCtrl.InsertItem(LVIF_TEXT | LVIF_PARAM, 100, (LPCTSTR) autoprotectName, NULL, NULL, NULL, reinterpret_cast<LPARAM>(autoprotect));
					returnValHR = autoprotect->get_ProtectionStatus(&protectionStatus);
					if (SUCCEEDED(returnValHR))
					{
						if (protectionStatus == ProtectionProviderLib::ProtectionStatus_On)
							apStatus = _T("On");
						else if (protectionStatus == ProtectionProviderLib::ProtectionStatus_Off)
							apStatus = _T("Off");
						else
							apStatus = _T("Error");
					}
					else
					{
						_bstr_t errorDescription;
						GetErrorDescription(returnValHR, &errorDescription, true);
						wsprintf(errorMessage, _T("Error 0x%08x:  %s"), returnValHR, (LPCTSTR) errorDescription);
						apStatus = errorMessage;
					}
					autoprotectsCtrl.SetItem(itemNo, AutoprotectsColumn_Status, LVIF_TEXT, apStatus, NULL, NULL, NULL, NULL);
				}
			}
		}
	}
	// *** DML TEMP DEV
	apStuff.StopTimer();
	// ***

	simpleScan = protectionProvider;
	if (simpleScan == NULL)
		supportsBasicScan.SetCheck(BST_UNCHECKED);
	else
		supportsBasicScan.SetCheck(BST_CHECKED);

	configureableScans = protectionProvider;
	if (simpleScan == NULL)
		supportsConfigureableScans.SetCheck(BST_UNCHECKED);
	else
		supportsConfigureableScans.SetCheck(BST_CHECKED);

	// *** DML TEMP DEV
	defUtils.StartTimer();
	// ***
	defInfo = protectionProvider;
	if (defInfo != NULL)
	{
		ProtectionProviderLib::_SYSTEMTIME defsDate;
		unsigned int revisionNo = 0;
		defInfo->get_Date(&defsDate);
		defInfo->get_RevisionNo(&revisionNo);
	}
	// *** DML TEMP DEV
	defUtils.StopTimer();
	timer.StopTimer();
	defUtils.OutputElapsedTime(_T("ShowProtectionProviderInfo DefUtils stuff"));
	apStuff.OutputElapsedTime(_T("ShowProtectionProviderInfo AP stuff"));
	timer.OutputElapsedTime(_T("ShowProtectionProviderInfo processing"));
	// ***
	return S_OK;
}

ProtectionProviderLib::IProtection_Provider* CTestHarnessDlg::GetSelectedProtectionProvider( void )
// Returns the currently selected protection provider, or NULL if none selected
{
	POSITION											itemPosition			= NULL;
	int													itemNo					= 0;
	ProtectionProviderLib::IProtection_Provider*		protectionProvider		= NULL;

	itemPosition = protectionTechnologiesCtrl.GetFirstSelectedItemPosition();
	if (itemPosition != NULL)
	{
		itemNo = protectionTechnologiesCtrl.GetNextSelectedItem(itemPosition);
		protectionProvider = reinterpret_cast<ProtectionProviderLib::IProtection_Provider*>(protectionTechnologiesCtrl.GetItemData(itemNo));
	}
	return protectionProvider;
}

ProtectionProviderLib::IProtection* CTestHarnessDlg::GetSelectedAutoprotect( void )
// Returns the currently selected protection provider, or NULL if none selected
{
	POSITION											itemPosition			= NULL;
	int													itemNo					= 0;
	ProtectionProviderLib::IProtection*					autoprotect				= NULL;

	itemPosition = autoprotectsCtrl.GetFirstSelectedItemPosition();
	if (itemPosition != NULL)
	{
		itemNo = autoprotectsCtrl.GetNextSelectedItem(itemPosition);
		autoprotect = reinterpret_cast<ProtectionProviderLib::IProtection*>(protectionTechnologiesCtrl.GetItemData(itemNo));
	}
	return autoprotect;
}

HRESULT CTestHarnessDlg::GetErrorDescription( HRESULT systemMessageID, _bstr_t* errorDescription, bool trimTrailingNewline )
// Sets *errorDescription equal to the description of the system message ID systemMessageID.
// If trimTrailingNewline is TRUE, trims any trailing newline
{
	LPTSTR		messageBuffer			= NULL;
	LPTSTR		lastChar				= NULL;
	DWORD		returnValDW				= FALSE;
	HRESULT		returnValHR				= E_FAIL;

	// Validate parameters
	if (errorDescription == NULL)
		return E_POINTER;

	//Get the message
	returnValDW = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, systemMessageID, NULL, reinterpret_cast<LPTSTR>(&messageBuffer), 1, NULL);
	if (returnValDW != 0)
	{
		if (trimTrailingNewline)
		{
			//Check the last 2 bytes to see if they are 0d0a
			lastChar = CharPrev(messageBuffer, messageBuffer + lstrlen(messageBuffer));
			if (lastChar)
			{
				if ((*lastChar == _T('\r')) || (*lastChar == _T('\n')))
					*lastChar = NULL;
			}
			lastChar = CharPrev(messageBuffer, messageBuffer + lstrlen(messageBuffer));
			if (lastChar)
			{
				if ((*lastChar == _T('\r')) || (*lastChar == _T('\n')))
					*lastChar = NULL;
			}
		}
		try
		{
			*errorDescription = messageBuffer;
			returnValHR = S_OK;
		}
		catch (_com_error&)
		{
			returnValHR = E_OUTOFMEMORY;
		}
		LocalFree(messageBuffer);
		messageBuffer = NULL;
	}
	else
	{
		returnValHR = HRESULT_FROM_WIN32(GetLastError());
	}
	return returnValHR;
}