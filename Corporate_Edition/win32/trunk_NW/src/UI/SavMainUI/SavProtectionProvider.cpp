// SavProtectionProvider.cpp : Implementation of CSavProtectionProvider

#include "stdafx.h"
#include "SavProtectionProvider.h"
#include "msi.h"
#include "msiquery.h"
#include "TrustUtil.h"
#include "VPExceptionHandling.h"
#include "SavIdefutilsLoader.h"
#include "DefutilsInterface.h"
#include "navver.h"
#include "FutzerificDlg.h"
#include "FilesystemApPage.h"
#include "ExchangeApPage.h"
#include "NotesApPage.h"
#include "InternetMailApPage.h"
#include "GeneralPage.h"
#include "UiUtil.h"
#include "SavAutoprotectFilesystem.h"
#include "SavAutoprotectExchange.h"
#include "SavAutoprotectInternetEmail.h"
#include "SavAutoprotectNotes.h"
#include "ConfigureableScanCollection.h"
#include "ConfigObj.h"

// CSavProtectionProvider
STDMETHODIMP CSavProtectionProvider::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ISavProtectionProvider
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

// CONSTRUCTOR-DESTRUCTOR
CSavProtectionProvider::CSavProtectionProvider()
{
    // Nothing for now (should probably go in FinalConstruct instead)
}

HRESULT CSavProtectionProvider::FinalConstruct()
{
    HRESULT                         returnValHR         = E_FAIL;

    // Initialize trust checking
    returnValHR = trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
    if (FAILED(returnValHR))
        return returnValHR;

    // Create the autoprotects
    returnValHR = CSavAutoprotectFilesystem::CreateInstance(&apFilesystem);
    if (SUCCEEDED(returnValHR))
    {
        try
        {
            autoprotects.push_back(apFilesystem);
        }
        catch (std::bad_alloc&)
        {
            returnValHR = E_OUTOFMEMORY;
        }
    }
    if (CSavAutoprotectInternetEmail::IsStorageInstalled())
    {
        returnValHR = CSavAutoprotectInternetEmail::CreateInstance(&apInternetEmail);
        if (SUCCEEDED(returnValHR))
        {
            try
            {
                autoprotects.push_back(apInternetEmail);
            }
            catch (std::bad_alloc&)
            {
                returnValHR = E_OUTOFMEMORY;
            }
        }
    }
    if (CSavAutoprotectExchange::IsStorageInstalled())
    {
        returnValHR = CSavAutoprotectExchange::CreateInstance(&apExchange);
        if (SUCCEEDED(returnValHR))
        {
            try
            {
                autoprotects.push_back(apExchange);
            }
            catch (std::bad_alloc&)
            {
                returnValHR = E_OUTOFMEMORY;
            }
        }
    }
    if (CSavAutoprotectNotes::IsStorageInstalled())
    {
        returnValHR = CSavAutoprotectNotes::CreateInstance(&apNotes);
        if (SUCCEEDED(returnValHR))
        {
            try
            {
                autoprotects.push_back(apNotes);
            }
            catch (std::bad_alloc&)
            {
                returnValHR = E_OUTOFMEMORY;
            }
        }
    }

    // Always succeed to expose as much functionality as possible
    return S_OK;
}

void CSavProtectionProvider::FinalRelease() 
{
    autoprotects.clear();
    apFilesystem.Release();
    apInternetEmail.Release();
    apNotes.Release();
    apExchange.Release();
    trustVerifier.Shutdown();
}

// IPROTECTION_CONTAINER
HRESULT CSavProtectionProvider::get_ProtectionCount( long* noItems )
{
    // Validate parameters
    if (noItems == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *noItems = static_cast<long>(autoprotects.size());
    return S_OK;
}

HRESULT CSavProtectionProvider::get_Item( GUID itemID, IProtection** autoprotect )
{
    ProtectionList::iterator    currAutoprotect;
    GUID                        currAPID            = GUID_NULL;
    HRESULT                     returnValHR         = E_FAIL;

    // Validate parameters
    if (autoprotect == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValHR = E_INVALIDARG;
    for (currAutoprotect = autoprotects.begin(); currAutoprotect != autoprotects.end(); currAutoprotect++)
    {
        if (SUCCEEDED((*currAutoprotect)->get_ID(&currAPID)))
        {
            if (currAPID == itemID)
            {
                *autoprotect = *currAutoprotect;
                (*autoprotect)->AddRef();
                returnValHR = S_OK;
            }
        }
    }

    return returnValHR;
}

HRESULT CSavProtectionProvider::get__NewEnum( IEnumProtection** enumerator )
{
    // NOTE:  atlhost.h
    //typedef CComObject<CComEnum<IEnumUnknown, &__uuidof(IEnumUnknown), IUnknown*, _CopyInterface<IUnknown> > > enumunk;
    typedef CComEnumOnSTL< IEnumProtection, &IID_IEnumProtection, IProtection*, CopyInterfaceFixed<IProtection>, ProtectionList > CComEnumProtectionOnList;
    CComObject<CComEnumProtectionOnList>*           actualEnumerator        = NULL;

    HRESULT                                         returnValHR             = E_FAIL;

    // Validate parameters
    if (enumerator == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValHR = CComObject<CComEnumProtectionOnList>::CreateInstance(&actualEnumerator);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = actualEnumerator->Init(NULL, autoprotects);
        if (SUCCEEDED(returnValHR))
            returnValHR = actualEnumerator->QueryInterface(IID_IEnumProtection, (void**) enumerator);
    }
    return returnValHR;
}

// ISavProtectionProvider
HRESULT CSavProtectionProvider::RunQuickScan( HWND parentWindow, DWORD* savErrorCode )
{
    GUID            tempScanID              = GUID_NULL;
    CComBSTR        tempScanIDstring;
    CString         tempScanFullInternalName;
    _bstr_t         stringConverter;
	IScanConfig*    configObject            = NULL;
    IScan*          savScan                 = NULL;
    BOOL            scanMemory              = FALSE;
    BOOL            scanLoadpoints          = FALSE;
    BOOL            scanEraserDefs          = FALSE;
    CString         scanTitle;
	DWORD		    returnValDW             = 0;
    HRESULT         returnValHR             = E_FAIL;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    returnValHR = CoCreateLDVPObject(CLSID_CliProxy, IID_IScanConfig, (void**)&configObject);
	if( SUCCEEDED(returnValHR))
	{
        scanTitle.LoadString(IDS_SCAN_QUICKSCAN_TITLE);
        // Create a temporary config object
        // Use a new name each time.  If this is NOT done, then if this scan is restarted during the 10-second cleanup
        // interval (see below), RTVScan will mistakenly believe a scan is already in progress and every subsequent scan
        // attempt will be queued until RTVScan is restarted.
        CoCreateGuid(&tempScanID);
        tempScanIDstring = tempScanID;
        tempScanFullInternalName.FormatMessage(_T("Temporary Quick Scan Options %s"), (LPCTSTR) tempScanIDstring);
        stringConverter = tempScanFullInternalName;
        configObject->Open(NULL, HKEY_VP_USER_SCANS, (char*) stringConverter);
        // Read in the defaults
        stringConverter = _T(szReg_Key_Scan_Defaults);
  		configObject->CopyFromName(HKEY_VP_USER_SCANS, (char*) stringConverter);
        // Mark to delete when closed.  Despite this, it takes 10 seconds to get cleaned up.
        configObject->ChangeCloseMode(TRUE);

    	CConfigObj config(configObject);
        scanMemory = config.GetOption(_T(szReg_Val_ScanProcesses), REG_DEFAULT_ScanProcesses_QuickScan);
        scanLoadpoints = config.GetOption(_T(szReg_Val_ScanLoadpoints), REG_DEFAULT_ScanLoadpoints_QuickScan);
        scanEraserDefs = config.GetOption(_T(szReg_Val_ScanERASERDefs), REG_DEFAULT_ScanERASERDefs_QuickScan);
        config.SetOption(_T(szReg_Val_ScanProcesses),  REG_DEFAULT_ScanProcesses_QuickScan);
        config.SetOption(_T(szReg_Val_ScanLoadpoints), REG_DEFAULT_ScanLoadpoints_QuickScan);
        if (scanEraserDefs)
            config.SetOption(_T(szReg_Val_ScanERASERDefs), REG_DEFAULT_ScanERASERDefs_QuickScan);
        config.SetOption(_T(szReg_Val_ScanAllDrives),  FALSE);
        config.SetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_QUICK);
		config.SetOption(_T(szReg_Value_DisplayStatusDlg), TRUE);
	    config.SetOption(_T(szReg_Value_ScanTitle), scanTitle);
	    config.SetOption(_T(szReg_Value_Logger), (DWORD) LOGGER_Manual);
        config.SetOption(_T(szReg_Value_ScanStatus), (DWORD) S_NEVER_RUN );

        returnValHR = configObject->CreateByIID(IID_IScan, (void**)&savScan);
		if( SUCCEEDED(returnValHR))
		{
            returnValHR = savScan->Open(NULL, configObject);
			if (SUCCEEDED(returnValHR))
            {
                returnValDW = savScan->StartScan(TRUE);
                if (returnValDW == ERROR_SUCCESS)
                {
                    returnValHR = S_OK;
                }
                else
                {
                    if (savErrorCode != NULL)
                        *savErrorCode = returnValDW;
                }
            }
			savScan->Release();
			savScan = NULL;
		}
        configObject->Release();
        configObject = NULL;
    }

    return returnValHR;
}

HRESULT CSavProtectionProvider::RunFullScan( HWND parentWindow, DWORD* savErrorCode )
{
    GUID            tempScanID              = GUID_NULL;
    CComBSTR        tempScanIDstring;
    CString         tempScanFullInternalName;
    _bstr_t         stringConverter;
	IScanConfig*    configObject            = NULL;
    IScan*          savScan                 = NULL;
    BOOL            scanMemory              = FALSE;
    BOOL            scanLoadpoints          = FALSE;
    BOOL            scanEraserDefs          = FALSE;
    CString         scanTitle;
	DWORD		    returnValDW             = 0;
    HRESULT         returnValHR             = E_FAIL;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    returnValHR = CoCreateLDVPObject(CLSID_CliProxy, IID_IScanConfig, (void**)&configObject);
	if( SUCCEEDED(returnValHR))
	{
        scanTitle.LoadString(IDS_SCAN_FULLSCAN_TITLE);
        // Create a temporary config object
        // Create a temporary config object
        // Use a new name each time.  If this is NOT done, then if this scan is restarted during the 10-second cleanup
        // interval (see below), RTVScan will mistakenly believe a scan is already in progress and every subsequent scan
        // attempt will be queued until RTVScan is restarted.
        CoCreateGuid(&tempScanID);
        tempScanIDstring = tempScanID;
        tempScanFullInternalName.FormatMessage(_T("Temporary Quick Scan Options %s"), (LPCTSTR) tempScanIDstring);
        stringConverter = tempScanFullInternalName;
        configObject->Open(NULL, HKEY_VP_USER_SCANS, (char*) stringConverter);
        // Read in the defaults
        stringConverter = _T(szReg_Key_Scan_Defaults);
  		configObject->CopyFromName(HKEY_VP_USER_SCANS, (char*) stringConverter);
        // Mark to delete when closed
        configObject->ChangeCloseMode(TRUE);

    	CConfigObj config(configObject);
        scanMemory = config.GetOption(_T(szReg_Val_ScanProcesses), REG_DEFAULT_ScanProcesses_QuickScan);
        scanLoadpoints = config.GetOption(_T(szReg_Val_ScanLoadpoints), REG_DEFAULT_ScanLoadpoints_QuickScan);
        scanEraserDefs = config.GetOption(_T(szReg_Val_ScanERASERDefs), REG_DEFAULT_ScanERASERDefs_QuickScan);
        config.SetOption(_T(szReg_Val_ScanProcesses),  REG_DEFAULT_ScanProcesses_QuickScan);
        config.SetOption(_T(szReg_Val_ScanLoadpoints), REG_DEFAULT_ScanLoadpoints_QuickScan);
        if (scanEraserDefs)
            config.SetOption(_T(szReg_Val_ScanERASERDefs), REG_DEFAULT_ScanERASERDefs_QuickScan);
        config.SetOption(_T(szReg_Val_ScanAllDrives),  TRUE);
        config.SetOption(_T(szReg_Val_SelectedScanType), SCAN_TYPE_FULL);
		config.SetOption(_T(szReg_Value_DisplayStatusDlg), TRUE);
	    config.SetOption(_T(szReg_Value_ScanTitle), scanTitle);
	    config.SetOption(_T(szReg_Value_Logger), (DWORD) LOGGER_Manual);
        config.SetOption(_T(szReg_Value_ScanStatus), (DWORD) S_NEVER_RUN );

        returnValHR = configObject->CreateByIID(IID_IScan, (void**)&savScan);
		if( SUCCEEDED(returnValHR))
		{
            returnValHR = savScan->Open(NULL, configObject);
			if (SUCCEEDED(returnValHR))
            {
                returnValDW = savScan->StartScan(TRUE);
                if (returnValDW == ERROR_SUCCESS)
                {
                    returnValHR = S_OK;
                }
                else
                {
                    if (savErrorCode != NULL)
                        *savErrorCode = returnValDW;
                }
            }
			savScan->Release();
			savScan = NULL;
		}
        configObject->Release();
        configObject = NULL;
    }

    return returnValHR;
}

// IPROTECTION_DEFINITIONINFO
HRESULT CSavProtectionProvider::get_Date( SYSTEMTIME* defsDate )
{
    CDefUtilsLoaderHelper   objDefUtilsLdr;
    IDefUtils4Ptr           ptrDefUtils;
    DWORD                   defsRevision        = 0;
    HRESULT                 returnValHR         = E_FAIL;

    // Validate parameters
    if (defsDate == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValHR = objDefUtilsLdr.CreateObject(ptrDefUtils);
    if (SYM_SUCCEEDED(returnValHR))
    {
        if (ptrDefUtils->InitWindowsApp ("NAVCORP_70"))
        {
            if (ptrDefUtils->GetCurrentDefsDate(&(defsDate->wYear), &(defsDate->wMonth), &(defsDate->wDay), &defsRevision))
            {
                // Determine a SYSTEMTIME-compatible day of week, and clear all other fields.
                try
                {
                    CTime defsDateCT(defsDate->wYear, defsDate->wMonth, defsDate->wDay);
                    int x = defsDateCT.GetDayOfWeek();
                }
                catch(...)
                {
                    // Only DOW is bad, rest is fine, just ignore this error
                }
                returnValHR = S_OK;
            }
        }
    }

    return returnValHR;
}

HRESULT CSavProtectionProvider::get_RevisionNo( unsigned int* revisionNo )
{
    CDefUtilsLoaderHelper   objDefUtilsLdr;
    IDefUtils4Ptr           ptrDefUtils;
    WORD                    defsYear            = 0;
    WORD                    defsMonth           = 0;
    WORD                    defsDay             = 0;
    DWORD                   defsRev             = 0;
    HRESULT                 returnValHR         = E_FAIL;

    // Validate parameters
    if (revisionNo == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValHR = objDefUtilsLdr.CreateObject(ptrDefUtils);
    if (SYM_SUCCEEDED(returnValHR))
    {
        if (ptrDefUtils->InitWindowsApp ("NAVCORP_70"))
        {
            if (ptrDefUtils->GetCurrentDefsDate(&defsYear, &defsMonth, &defsDay, &defsRev))
            {
                *revisionNo = defsRev;
                returnValHR = S_OK;
            }
        }
    }

    return returnValHR;
}

HRESULT CSavProtectionProvider::get_ShortDescription( BSTR* shortDescription )
{
    // Validate parameters
    if (shortDescription == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CComBSTR nameBuffer;

    nameBuffer.LoadString(IDS_SAVPROVIDER_DEFSDESCRIPTION);
    *shortDescription = nameBuffer.Detach();
    return S_OK;
}

// IPROTECTION_PROVIDER
HRESULT CSavProtectionProvider::get_ID( GUID* id )
{
    // Validate parameter
    if (id == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *id = CLSID_SavProtectionProvider;
    return S_OK;
}

HRESULT CSavProtectionProvider::get_SplashGraphic( HGDIOBJ* bitmapHandle )
{
    // Validate parameter
    if (bitmapHandle == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *bitmapHandle = ::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_SPLASHGRAPHIC));
    if (*bitmapHandle != NULL)
        return S_OK;
    else
        return E_FAIL;
}

HRESULT CSavProtectionProvider::get_Installed( VARIANT_BOOL* isInstalled )
{
    StringList  productCodes;
    HRESULT     returnValHR     = E_FAIL;
    
    // Validate parameter
    if (isInstalled == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // We never fail - it's either there or isn't.
    returnValHR = S_OK;
    // Locate SAV or SCS.  Match version, but give a little leeway.
    GetProductsWithUpgradeCode(_T(MSI_UPGRADECODE_SCS), 11, 0, 11, 1, &productCodes);
    GetProductsWithUpgradeCode(_T(MSI_UPGRADECODE_SAV), 11, 0, 11, 1, &productCodes);
    GetProductsWithUpgradeCode(_T(MSI_UPGRADECODE_SAV64AMD), 11, 0, 11, 1, &productCodes);
    
    if (productCodes.size() == 0)
        *isInstalled = VARIANT_TRUE;
    else
        *isInstalled = VARIANT_FALSE;
    return returnValHR;
}

HRESULT CSavProtectionProvider::get_Autoprotects( IProtection_Container** autoprotects )
{
    // Validate parameters
    if (autoprotects == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    return QueryInterface(IID_IProtection_Container, reinterpret_cast<void**>(autoprotects));
}

HRESULT CSavProtectionProvider::get_PrimaryAutoprotect( IProtection** primaryAutoprotect )
{
    // Validate parameters
    if (primaryAutoprotect == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    *primaryAutoprotect = apFilesystem;
    (*primaryAutoprotect)->AddRef();
    return S_OK;
}

HRESULT CSavProtectionProvider::get_ConfigureableScans( IProtection_ConfigureableScan_Container** scans )
{
    HRESULT                                         returnValHR             = E_FAIL;

    // Validate parameters
    if (scans == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    returnValHR = CConfigureableScanCollection::CreateInstance(scans);
    if (SUCCEEDED(returnValHR))
    {
        CConfigureableScanCollection* scansCollectionPtr = NULL;
        scansCollectionPtr = dynamic_cast<CConfigureableScanCollection*>(*scans);
        if (scans != NULL)
        {
            returnValHR = S_OK;
            scansCollectionPtr->AddCurrentUserScans();
            scansCollectionPtr->AddAdminScans();
        }
    }

    return returnValHR;
}

HRESULT CSavProtectionProvider::get_AdministrativeExceptions( IExceptionItem_Container** administrativeExceptions )
{
    return E_NOTIMPL;
}

HRESULT CSavProtectionProvider::get_LocalExceptions( IExceptionItem_Container** localExceptions )
{
    return E_NOTIMPL;
}

HRESULT CSavProtectionProvider::put_LocalExceptions( IExceptionItem_Container* newLocalExceptions )
{
    return E_NOTIMPL;
}

// IPROTECTION
HRESULT CSavProtectionProvider::get_DisplayName( BSTR* shortDisplayName )
{
    CComBSTR stringBuffer;

    // Validate parameters
    if (shortDisplayName == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    stringBuffer.LoadString(IDS_SAVPROVIDER_NAME);
    *shortDisplayName = stringBuffer.Detach();
    return S_OK;
}

HRESULT CSavProtectionProvider::get_Description( BSTR* userDescription )
{
    CComBSTR stringBuffer;

    // Validate parameter
    if (userDescription == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    stringBuffer.LoadString(IDS_SAVPROVIDER_DESCRIPTION);
    *userDescription = stringBuffer.Detach();
    return S_OK;
}

HRESULT CSavProtectionProvider::get_ProtectionStatus( ProtectionStatus* currStatus )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

#pragma message("*** CSavProtectionProvider::get_ProtectionStatus")
    *currStatus = ProtectionStatus_On;
    return E_NOTIMPL;
}

HRESULT CSavProtectionProvider::get_ProtectionStatusDescription( BSTR* statusDescription )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    MessageBox(NULL, _T("Not yet implemented"), _T("CSavProtectionProvider::get_ProtectionStatusDescription"), MB_OK | MB_ICONINFORMATION);
    return E_NOTIMPL;
}

HRESULT CSavProtectionProvider::get_ProtectionStatusReadOnly( VARIANT_BOOL* isReadOnly )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    MessageBox(NULL, _T("Not yet implemented"), _T("CSavProtectionProvider::get_ProtectionStatusReadOnly"), MB_OK | MB_ICONINFORMATION);
    return E_NOTIMPL;
}

HRESULT CSavProtectionProvider::get_ProtectionConfiguration( VARIANT_BOOL* protectionEnabled )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    MessageBox(NULL, _T("Not yet implemented"), _T("CSavProtectionProvider::get_ProtectionConfiguration"), MB_OK | MB_ICONINFORMATION);
    return E_NOTIMPL;
}

HRESULT CSavProtectionProvider::put_ProtectionConfiguration( VARIANT_BOOL newStatus )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    MessageBox(NULL, _T("Not yet implemented"), _T("CSavProtectionProvider::put_ProtectionConfiguration"), MB_OK | MB_ICONINFORMATION);
    return E_NOTIMPL;
}

HRESULT CSavProtectionProvider::ShowConfigureUI( HWND parentWindowHandle )
{
    TCHAR stringBuffer[100] = _T("");
    DWORD returnValDWB      = 0;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CControlParentSheet         configWizard;
    CFilesystemApPage           filesystemAP;
    CExchangeApPage             exchangeAP;
    CNotesApPage                notesAP;
    CInternetMailApPage         imailPage;
    CGeneralPage                generalPage;
    CWnd                        parentWindow;

    parentWindow.Attach(parentWindowHandle);
    configWizard.SetParent(&parentWindow);
    configWizard.SetCaption(_T("Configure AntiVirus"));
    configWizard.m_psh.dwFlags |= PSH_NOAPPLYNOW;
    configWizard.AddPage(&generalPage);
    configWizard.AddPage(&filesystemAP);
    configWizard.AddPage(&imailPage);
    configWizard.AddPage(&exchangeAP);
    configWizard.AddPage(&notesAP);
    configWizard.DoModal();
    parentWindow.Detach();

    return S_FALSE;
}

HRESULT CSavProtectionProvider::ShowLogUI( HWND parentWindowHandle )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    MessageBox(parentWindowHandle, _T("Coming soon to a client near you"), _T("Log UI"), MB_OK | MB_ICONINFORMATION);
    return S_OK;
}
