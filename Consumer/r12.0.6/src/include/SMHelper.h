#pragma once

#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"

class CSMHelper : public ccSettings::CSettingsManagerHelper
{
public:
    CSMHelper(void);
    virtual ~CSMHelper(void);

    // Waits for Event Manager to be loaded.
    //
    bool IsSettingsManagerActive(bool bWaitForEM);

    // Load () - Loads data from ccSettings into the property bag.
    //
    bool Load (LPCWSTR lpszMainKey);
    
    // Save () - Stores the property bag data into Settings under lpszMainKey.
    //
    bool Save (LPCWSTR lpszMainKey);

    // Delete() - Deletes key and sub keys.
    //
    bool Delete (LPCWSTR lpszMainKey);

    // Use this function to access the loaded settings
    //
    ccSettings::ISettings* GetSettings (void);

protected:
    static const WCHAR m_wcDelimiter;
    bool initSettingsMgr ();

private:
    const static DWORD m_dwLoadWaitMax;
    const static DWORD m_dwLoadWaitInterval;
    ccSettings::ISettingsPtr m_pSettings;
    ccSettings::ISettingsManager3Ptr m_pSettingsManager;
};
