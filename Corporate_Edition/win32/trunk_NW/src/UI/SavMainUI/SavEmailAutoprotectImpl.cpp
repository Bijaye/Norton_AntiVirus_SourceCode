// CSavEmailAutoprotectImpl.cpp : Implementation of CCSavEmailAutoprotectImpl
#include "stdafx.h"
#include "SavEmailAutoprotectImpl.h"
#include "UiUtil.h"
#include "TrustUtil.h"
#include "SymSaferRegistry.h"
#include <StrSafe.h>


// CSavEmailAutoprotectImpl
HRESULT CSavEmailAutoprotectImpl::FinalConstruct( Config newConfig )
{
    HRESULT                         returnValHR         = E_FAIL;

    // Initialize trust checking
    config = newConfig;
    returnValHR = trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
    return returnValHR;
}

void CSavEmailAutoprotectImpl::FinalRelease( void )
{
    trustVerifier.Shutdown();
}

// IPROTECTION_AUTOPROTECT
HRESULT CSavEmailAutoprotectImpl::get_ID( GUID* id )
{
    // Validate parameters
    if (id == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;
    
    *id = config.componentCLSID;
    return S_OK;
}

HRESULT CSavEmailAutoprotectImpl::get_DisplayName( BSTR* shortName )
{
    // Validate parameters
    if (shortName == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString stringBuffer;
    
    stringBuffer.LoadString(config.displayNameID);
    *shortName = stringBuffer.AllocSysString();
    return S_OK;
}

HRESULT CSavEmailAutoprotectImpl::get_Description( BSTR* userDescription )
{
    CComBSTR userString;

    // Validate parameters
    if (userDescription == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString stringBuffer;
    
    stringBuffer.LoadString(config.descriptionID);
    *userDescription = stringBuffer.AllocSysString();
    return S_OK;
}

HRESULT CSavEmailAutoprotectImpl::get_ProtectionStatus( ProtectionStatus* currStatus )
{
    VARIANT_BOOL        protectionConfig                        = FALSE;
    DWORD               returnValDW                             = ERROR_OUT_OF_PAPER;
    HRESULT             returnValHR                             = E_FAIL;

    // Validate parmaeters
    if (currStatus == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValHR = get_ProtectionConfiguration(&protectionConfig);
    if (protectionConfig)
        *currStatus = ProtectionStatus_On;
    else
        *currStatus = ProtectionStatus_Off;
    return returnValHR;
}

HRESULT CSavEmailAutoprotectImpl::get_ProtectionStatusDescription( BSTR* statusDescription )
{
    ProtectionStatus    currProtectionStatus;
    CComBSTR            protectionStatusDescription;
    HRESULT             returnValHR                     = E_FAIL;

    // Validate parmaeters
    if (statusDescription == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    returnValHR = get_ProtectionStatus(&currProtectionStatus);
    if (SUCCEEDED(returnValHR) && (currProtectionStatus != ProtectionStatus_Error))
    {
        protectionStatusDescription = _T("OK");
    }
    else
    {
        protectionStatusDescription = _T("Unknown error");
    }
    *statusDescription = protectionStatusDescription.Detach();
    returnValHR = S_OK;
    return returnValHR;
}

HRESULT CSavEmailAutoprotectImpl::get_ProtectionStatusReadOnly( VARIANT_BOOL* isStatusReadOnly )
{
    DWORD               returnValDW         = ERROR_SERVICE_NOT_RUNNING;
    HRESULT             returnValHR         = E_FAIL;

    // Validate parmaeter
    if (isStatusReadOnly == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValHR = S_OK;

    // Return result.  We need to always succeed.
    if (SUCCEEDED(returnValHR))
        *isStatusReadOnly = VARIANT_FALSE;
    else
        *isStatusReadOnly = VARIANT_TRUE;
    return S_OK;
}

HRESULT CSavEmailAutoprotectImpl::get_ProtectionConfiguration( VARIANT_BOOL* protectionEnabled )
{
    TCHAR       keyPath[MAX_PATH]   = _T("");
    HKEY        keyHandle           = NULL;
    BOOL        valueData           = FALSE;
    DWORD       valueDataSize       = 0;
    DWORD       returnValDW         = ERROR_OUT_OF_PAPER;

    // Validate parameter
    if (protectionEnabled == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    StringCchPrintf(keyPath, _countof(keyPath), _T(szReg_Key_Main) _T("\\") _T(szReg_Key_Storages) _T("\\%s\\") _T(szReg_Key_Storage_RealTime), config.storageName);
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        valueDataSize = sizeof(valueData);
        returnValDW = SymSaferRegQueryValueEx(keyHandle, _T(szReg_Val_RTSScanOnOff), NULL, NULL, reinterpret_cast<BYTE*>(&valueData), &valueDataSize);
        if (returnValDW == ERROR_SUCCESS)
            *protectionEnabled = valueData;
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }

    return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT CSavEmailAutoprotectImpl::put_ProtectionConfiguration( VARIANT_BOOL newStatus )
{
    TCHAR       keyPath[MAX_PATH]   = _T("");
    HKEY        keyHandle       = NULL;
    BOOL        newStatusBOOL   = FALSE;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // Set the configuration setting
    StringCchPrintf(keyPath, _countof(keyPath), _T(szReg_Key_Main) _T("\\") _T(szReg_Key_Storages) _T("\\%s\\") _T(szReg_Key_Storage_RealTime), config.storageName);
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, NULL, KEY_WRITE, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        newStatusBOOL = newStatus;
        returnValDW = RegSetValueEx(keyHandle, _T(szReg_Val_RTSScanOnOff), NULL, REG_DWORD, reinterpret_cast<BYTE*>(&newStatusBOOL), sizeof(newStatusBOOL));
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
    returnValHR = HRESULT_FROM_WIN32(returnValDW);

    return returnValHR;
}

HRESULT CSavEmailAutoprotectImpl::ShowConfigureUI( CPropertyPage* propertyPage, HWND parentWindowHandle )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;
    if (propertyPage == NULL)
        return E_POINTER;

    // Technically redundant since caller must do this to create their property page, but good practice
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CControlParentSheet         configWizard;
    CWnd                        parentWindow;

    parentWindow.Attach(parentWindowHandle);
    configWizard.SetParent(&parentWindow);
    configWizard.SetCaption(_T("Configure"));
    configWizard.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_PROPTITLE;
    configWizard.AddPage(propertyPage);
    configWizard.DoModal();
    parentWindow.Detach();

    return S_OK;
}

HRESULT CSavEmailAutoprotectImpl::ShowLogUI( CPropertyPage* propertyPage, HWND parentWindowHandle )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CComBSTR displayName;

    displayName.LoadStringW(config.displayNameID);
    
    MessageBox(parentWindowHandle, displayName, _T("Log UI"), MB_OK | MB_ICONINFORMATION);

    return E_NOTIMPL;
}

// ** CLASS STATICS **
bool CSavEmailAutoprotectImpl::IsStorageInstalled( LPCTSTR keyFilename, LPCTSTR storageName )
// Returns S_OK if Exchange protection is installed, S_FALSE if not
{
    std::wstring        savDirectory;
    std::wstring        storageFilename;
    TCHAR               storageKeyPath[MAX_PATH+1]      = _T("");
    TCHAR               valueData[2*MAX_PATH+1]         = _T("");
    DWORD               valueDataSize                   = 0;
    CRegKey             storageKey;
    std::wstring        configuredStorageFilename;
    bool                returnValBool                   = false;
    HRESULT             returnValHR                     = E_FAIL;

    // Does the exchange storage/hook file exist?
    returnValHR = GetSavDirectory(&savDirectory);
    if (SUCCEEDED(returnValHR))
    {
        storageFilename = savDirectory;
        storageFilename += keyFilename;
    }
    returnValBool = FileExists(storageFilename);

    // Is the storage registered with RTVScan?
    if (returnValBool)
    {
        // storageName!!
        StringCchPrintf(storageKeyPath, sizeof(storageKeyPath)/sizeof(storageKeyPath[0]), _T(szReg_Key_Main) _T("\\") _T(szReg_Key_Storages) _T("\\%s"), storageName);
        if (storageKey.Open(HKEY_LOCAL_MACHINE, storageKeyPath, KEY_READ) == ERROR_SUCCESS)
        {
            valueData[0] = NULL;
            valueDataSize = sizeof(valueData)/sizeof(valueData[0]);
            SymSaferRegQueryValueEx(storageKey.m_hKey, _T(DLL_PATH), NULL, NULL, (LPBYTE) &valueData, &valueDataSize);
            configuredStorageFilename = valueData;

            valueData[0] = NULL;
            valueDataSize = sizeof(valueData)/sizeof(valueData[0]);
            SymSaferRegQueryValueEx(storageKey.m_hKey, _T(DLL_NAME), NULL, NULL, (LPBYTE) &valueData, &valueDataSize);
            configuredStorageFilename += valueData;

            storageKey.Close();
            returnValBool = (_tcsicmp(configuredStorageFilename.c_str(), storageFilename.c_str()) == ERROR_SUCCESS);
        }
    }

    return returnValBool;
}