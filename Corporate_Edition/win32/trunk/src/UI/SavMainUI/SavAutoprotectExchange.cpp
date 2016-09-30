// SavAutoprotectExchange.cpp : Implementation of CSavAutoprotectExchange
#include "stdafx.h"
#include "SavAutoprotectExchange.h"
#include "ExchangeApPage.h"


// ** STATIC DATA INITIALIZATION
CSavEmailAutoprotectImpl::Config CSavAutoprotectExchange::config = 
{
    CLSID_SavAutoprotectExchange,
    IDS_AP_EXCHANGE_DISPLAYNAME,
    IDS_AP_EXCHANGE_DESCRIPTION,
    _T("\\vpmsece4.dll"),
    _T(szReg_Key_Storage_Exchange)
};
 

// CSavAutoprotectExchange
HRESULT CSavAutoprotectExchange::FinalConstruct()
{
    apImplementation.FinalConstruct(config);
    return S_OK;
}

void CSavAutoprotectExchange::FinalRelease()
{
    apImplementation.FinalRelease();
}

// IPROTECTION_AUTOPROTECT
HRESULT CSavAutoprotectExchange::get_ID( GUID* id )
{
    return apImplementation.get_ID(id);
}

HRESULT CSavAutoprotectExchange::get_DisplayName( BSTR* shortName )
{
    return apImplementation.get_DisplayName(shortName);
}

HRESULT CSavAutoprotectExchange::get_Description( BSTR* userDescription )
{
    return apImplementation.get_Description(userDescription);
}

HRESULT CSavAutoprotectExchange::get_ProtectionStatus( ProtectionStatus* currStatus )
{
    return apImplementation.get_ProtectionStatus(currStatus);
}

HRESULT CSavAutoprotectExchange::get_ProtectionStatusDescription( BSTR* statusDescription )
{
    return apImplementation.get_ProtectionStatusDescription(statusDescription);
}

HRESULT CSavAutoprotectExchange::get_ProtectionStatusReadOnly( VARIANT_BOOL* isStatusReadOnly )
{
    return apImplementation.get_ProtectionStatusReadOnly(isStatusReadOnly);
}

HRESULT CSavAutoprotectExchange::get_ProtectionConfiguration( VARIANT_BOOL* protectionEnabled )
{
    return apImplementation.get_ProtectionConfiguration(protectionEnabled);
}

HRESULT CSavAutoprotectExchange::put_ProtectionConfiguration( VARIANT_BOOL newStatus )
{
    return apImplementation.put_ProtectionConfiguration(newStatus);
}

HRESULT CSavAutoprotectExchange::ShowConfigureUI( HWND parentWindowHandle )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CExchangeApPage propertyPage;

    return apImplementation.ShowConfigureUI(&propertyPage, parentWindowHandle);
}

HRESULT CSavAutoprotectExchange::ShowLogUI( HWND parentWindowHandle )
{
    return apImplementation.ShowLogUI(NULL, parentWindowHandle);
}

// ** CLASS STATICS **
bool CSavAutoprotectExchange::IsStorageInstalled( void )
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
