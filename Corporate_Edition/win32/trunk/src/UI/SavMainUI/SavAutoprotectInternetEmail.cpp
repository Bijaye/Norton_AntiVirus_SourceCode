// SavAutoprotectInternetEmail.cpp : Implementation of CSavAutoprotectInternetEmail
#include "stdafx.h"
#include "SavAutoprotectInternetEmail.h"
#include "InternetMailApPage.h"


// ** STATIC DATA INITIALIZATION
CSavEmailAutoprotectImpl::Config CSavAutoprotectInternetEmail::config = 
{
    CLSID_SavAutoprotectExchange,
    IDS_AP_IMAIL_DISPLAYNAME,
    IDS_AP_IMAIL_DESCRIPTION,
    _T("\\imail.dll"),
    _T(szReg_Key_Storage_InternetMail)
};
 

// CSavAutoprotectInternetEmail
HRESULT CSavAutoprotectInternetEmail::FinalConstruct()
{
    apImplementation.FinalConstruct(config);
    return S_OK;
}

void CSavAutoprotectInternetEmail::FinalRelease()
{
    apImplementation.FinalRelease();
}

// IPROTECTION_AUTOPROTECT
HRESULT CSavAutoprotectInternetEmail::get_ID( GUID* id )
{
    return apImplementation.get_ID(id);
}

HRESULT CSavAutoprotectInternetEmail::get_DisplayName( BSTR* shortName )
{
    return apImplementation.get_DisplayName(shortName);
}

HRESULT CSavAutoprotectInternetEmail::get_Description( BSTR* userDescription )
{
    return apImplementation.get_Description(userDescription);
}

HRESULT CSavAutoprotectInternetEmail::get_ProtectionStatus( ProtectionStatus* currStatus )
{
    return apImplementation.get_ProtectionStatus(currStatus);
}

HRESULT CSavAutoprotectInternetEmail::get_ProtectionStatusDescription( BSTR* statusDescription )
{
    return apImplementation.get_ProtectionStatusDescription(statusDescription);
}

HRESULT CSavAutoprotectInternetEmail::get_ProtectionStatusReadOnly( VARIANT_BOOL* isStatusReadOnly )
{
    return apImplementation.get_ProtectionStatusReadOnly(isStatusReadOnly);
}

HRESULT CSavAutoprotectInternetEmail::get_ProtectionConfiguration( VARIANT_BOOL* protectionEnabled )
{
    return apImplementation.get_ProtectionConfiguration(protectionEnabled);
}

HRESULT CSavAutoprotectInternetEmail::put_ProtectionConfiguration( VARIANT_BOOL newStatus )
{
    return apImplementation.put_ProtectionConfiguration(newStatus);
}

HRESULT CSavAutoprotectInternetEmail::ShowConfigureUI( HWND parentWindowHandle )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CInternetMailApPage propertyPage;

    return apImplementation.ShowConfigureUI(&propertyPage, parentWindowHandle);
}

HRESULT CSavAutoprotectInternetEmail::ShowLogUI( HWND parentWindowHandle )
{
    return apImplementation.ShowLogUI(NULL, parentWindowHandle);
}

// ** CLASS STATICS **
bool CSavAutoprotectInternetEmail::IsStorageInstalled( void )
// Returns S_OK if Exchange protection is installed, S_FALSE if not
{
    return CSavEmailAutoprotectImpl::IsStorageInstalled(config.storageFilename, config.storageName);

    /*
    // Is the exchange hook registered with exchange?
    if (returnValBool)
    {
//            StringCchPrintf(extensionEntry, sizeof(extensionEntry)/sizeof(extensionEntry[0]), _T("4.0;[%s];1;00000011111"), storageFilename.c_str());
        if (exchangeKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Exchange\\Client\\Extensions"), KEY_READ) == ERROR_SUCCESS)
        {
            valueData[0] = NULL;
            exchangeKey.QueryStringValue(_T("Symantec AntiVirus 10.1"), valueData, sizeof(valueData)/sizeof(valueData[0]));
            exchangeKey.Close();
        }

        // Just check the filename in the entry
        firstSeparatorPos = _tcschr(valueData, _T(';'))
        secondSeparatorPos = _tcschr(firstSeparator, _T(';'))
        if ((firstSeparatorPos != NULL) && (secondSeparatorPos != NULL))
        {
            *firstSeparatorPos = NULL;
            *secondSeparatorPos = NULL;
            if (_tcsicmp(CharNext(firstSeparatorPos), storageFilename) == 0);
                returnValHR = S_OK;
        }
    }
    return returnValBool;
*/
}
