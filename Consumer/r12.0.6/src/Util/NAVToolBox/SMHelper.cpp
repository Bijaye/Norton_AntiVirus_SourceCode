#include "StdAfx.h"
#include "smhelper.h"

#include <vector>

const DWORD CSMHelper::m_dwLoadWaitMax = 180000;    // 3 min
const DWORD CSMHelper::m_dwLoadWaitInterval = 1000;  // 1 sec
const WCHAR CSMHelper::m_wcDelimiter = L':';

CSMHelper::CSMHelper(void)
{
}

CSMHelper::~CSMHelper(void)
{
}

bool CSMHelper::IsSettingsManagerActive(bool bWaitForEM) 
{
    BOOL bActive = ccSettings::CSettingsManagerHelper::IsSettingsManagerActive();

    // If it's on, or we aren't waiting, return now.
    //
    if ( !bWaitForEM || bActive )
    {
        return bActive;
    }

    ccLib::CEvent eventNothing;
    ccLib::CMessageLock msgLock (TRUE, TRUE);

    for ( DWORD dwWaitLoop = 0; dwWaitLoop < m_dwLoadWaitMax; dwWaitLoop += m_dwLoadWaitInterval )
    {
        eventNothing.Create ( NULL, FALSE, FALSE, NULL, FALSE);
        msgLock.Lock ( eventNothing, m_dwLoadWaitInterval );

        if (ccSettings::CSettingsManagerHelper::IsSettingsManagerActive())
            return true;
    }

    return false;
}

bool CSMHelper::initSettingsMgr ()
{
    // Load ccSettings
    //
    if ( !m_pSettingsManager.m_p )
    {
		if(!IsSettingsManagerActive(true))
			return false;

        ccSettings::ISettingsManagerPtr pSettingsMgr;
        SYMRESULT symRes = Create(pSettingsMgr);
        if (SYM_FAILED(symRes) ||
            pSettingsMgr == NULL)
        {
            CCTRACEE(_T("CSMHelper::initSettingsMgr() : ccSettings.Create() != SYM_OK, 0x%08X\n"), symRes);
            return false;
        }

        ccSettings::ISettingsManager3QIPtr pTempSettingsMgr (pSettingsMgr);

        if ( pTempSettingsMgr )
            m_pSettingsManager = pTempSettingsMgr;
        else
            return false;
    }

    return true;
}

bool CSMHelper::Load (LPCWSTR lpszMainKey)
{
    if ( !initSettingsMgr() )
        return false;

    // Create main key, if it doesn't exist
    //
    SYMRESULT symRes = m_pSettingsManager->CreateSettings(lpszMainKey, &m_pSettings);
    if (SYM_FAILED(symRes))
    {
        CCTRACEE(_T("CSMHelper::Load() : pSettingsManager->GetSettings() != SYM_OK, 0x%08X\n"), symRes);
        return false;
    }

    return true;
}

bool CSMHelper::Save (LPCWSTR lpszMainKey)
{
    if ( !initSettingsMgr() )
        return false;

    // Enum each key/value in the bag and store in Settings
    //
    bool bResult = false;
    SYMRESULT symRes;

    // Create main key, if it's not already open
    //
    if ( !m_pSettings.m_p )
    {
        symRes = m_pSettingsManager->CreateSettings(lpszMainKey, &m_pSettings);
        if (SYM_FAILED(symRes))
        {
            CCTRACEE(_T("CSMHelper::Save() : pSettingsManager->CreateSettings() != SYM_OK, 0x%08X\n"), symRes);
            return false;
        }
    }

    // Save the last settings
    //
    // Commit
    //
    symRes = m_pSettingsManager->PutSettings(m_pSettings, true); // Don't send events!
    if (FAILED(symRes))
    {
        CCTRACEE(_T("CSMHelper::Save() : pSettingsManager->PutSettings() != SYM_OK, 0x%08X\n"), symRes);
        return false;
    }

    return true;
}

bool CSMHelper::Delete (LPCWSTR lpszMainKey)
{
    if ( !initSettingsMgr() )
        return false;

    return ( SYM_SUCCEEDED ( m_pSettingsManager->DeleteSettings (lpszMainKey))); 
}

ccSettings::ISettings* CSMHelper::GetSettings (void)
{
    return m_pSettings;
}