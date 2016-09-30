// DefsManager.cpp: implementation of the CDefsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "const.h"
#include "DefsManager.h"
#include "serviceutil.h"
#include "GlobalEvents.h"
#include "GlobalEvent.h"
#include "NAVDefutilsLoader.h"

using namespace std;

const TCHAR AVENGEDEFS_VAL[] = _T("AVENGEDEFS");
const TCHAR SYMANTEC_INSTALLED_APPS_KEY[] = _T("SOFTWARE\\Symantec\\InstalledApps");

/////////////////////////////////////////////////////////////////////////////
// CDefsManager::CDefsManager()

CDefsManager::CDefsManager()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDefsManager::~CDefsManager()

CDefsManager::~CDefsManager()
{
	// Intentionally empty
}

/////////////////////////////////////////////////////////////////////////////
// CDefsManager::SwitchToLatestDefs()

void CDefsManager::SwitchToLatestDefs( LPCSTR szAppID,
                                       bool bUseDefs /*true*/, 
                                       const char* szTestDefs/*NULL*/,
                                       bool bAuthenticate /* true */)
{
    ccLib::CSingleLock lock( &m_csDefUtils, INFINITE, FALSE );
    bool bNewer = true;
	WORD wYear = 0, wMonth = 0, wDay = 0;
	DWORD dwRev = 0;

    // Try to use the newest defs.  If this fails, it's probably because
	// there is a new set of defs on the machine that has not been integrated
	// yet, but the current process doesn't have rights to the hawkings tree.
	// In this case, just use the current defs.
    char szDefsDir[MAX_PATH] = {0};

    DefUtilsLoader defUtilsLoader;
    IDefUtilsPtr pIDefUtils;

    SYMRESULT result = defUtilsLoader.CreateObject(pIDefUtils.m_p);
    if( SYM_FAILED(result) )
    {
        CCTRACEE ("CDefsManager::SwitchToLatestDefs() - failed to create DefUtils object 0x%x. Exiting...", result);
        return;
    }

    CCTRACEI ("CDefsManager::Run() - IDefUtils created");

    if (!pIDefUtils->InitWindowsApp( szAppID ))
    {
        CCTRACEE ("CDefsManager::SwitchToLatestDefs() - failed InitWindowsApp, exiting...");
        return;
    }

    // Make sure we are using the latest def set
    //
	if ( szTestDefs[0] )
    {
        CCTRACEI ("CDefsManager::SwitchToLatestDefs - using test defs %s", szTestDefs);
        
        // DefUtils interface doesn't want a const char*...
        char szDefs[MAX_PATH] = {0};
        strncpy ( szDefs, szTestDefs, MAX_PATH);
        if ( !pIDefUtils->UseNewestDefs( szDefs, MAX_PATH, &bNewer ))
        {
            CCTRACEE ("CDefsManager::SwitchToLatestDefs () - UseNewestDefs failed, exiting");
            return;
        }
    }
    else
    {
        // Enable authentication
        if( bUseDefs && bAuthenticate && !pIDefUtils->SetDoAuthenticationCheck(true) )
        {
            CCTRACEE ("CDefsManager::SwitchToLatestDefs() - Failed to enable definition authentication.");
        }

        CCTRACEI ("CDefsManager::SwitchToLatestDefs() - Starting def authentication %d", bUseDefs );
        bool bUseNewestDefsSucceeded = pIDefUtils->UseNewestDefs( &bNewer );
        CCTRACEI ("CDefsManager::SwitchToLatestDefs() - End def authentication %d", bUseDefs );

        DURESULT duResult = pIDefUtils->GetLastResult();

        // If this is not the APP ID we care about then ignore the failure
        if ( bUseDefs && !bUseNewestDefsSucceeded )
	    {
            if( duResult == DU_E_AUTH_FAILED_NO_RECOVERY )
            {
                // Disable AP if there are no authentic definitions
                CCTRACEE(_T("CDefsManager::SwitchToLatestDefs() - Def authentication failed sending the alert"));

                // If the bad defs flag was not already set then we need to set it and notify everyone
                // that the defs have changed
                if( _Module.m_bDefsBad == false )
                {
                    _Module.m_bDefsBad = true;

                    CGlobalEvent eventDefs;
                    eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
                    if ( eventDefs )
                        ::PulseEvent ( eventDefs );

                    // Alert the user and exit so we don't update the defs paths
                    _Module.defAuthenticationFailure(AUTHENTICATION_FAILED);
                    return;
                }
            }
            else
            {
                CCTRACEE(_T("CDefsManager::SwitchToLatestDefs() - UseNewestDefs() failed with error %d. Checking current defs"), duResult);

                // UseNewestDefs failed for some reason other than authentication, see if the defs we are currently
                // using are good
                if( pIDefUtils->GetCurrentDefs( szDefsDir, MAX_PATH ) )
                {
                    bool bTrusted = false;
                    if ( !pIDefUtils->IsAuthenticDefs(szDefsDir, &bTrusted) || !bTrusted )
                    {
                        CCTRACEE(_T("CDefsManager::SwitchToLatestDefs() - Current definitons are not authentic."));
                        // The current defs are not authentic
                        if( _Module.m_bDefsBad == false )
                        {
                            _Module.m_bDefsBad = true;

                            CGlobalEvent eventDefs;
                            eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
                            if ( eventDefs )
                                ::PulseEvent ( eventDefs );

                            // Alert the user and exit so we don't update the defs paths
                            _Module.defAuthenticationFailure(AUTHENTICATION_FAILED);
                        }
                        return;
                    }
                    else if( _Module.m_bDefsBad == true )
                    {
                        CCTRACEI(_T("CDefsManager::SwitchToLatestDefs() - Use newest defs failed but the current definitions are authentic so we'll take 'em."));
                        _Module.m_bDefsBad = false;
                    }
                }
                else
                {
                    // Couldn't get new defs or current defs so set to bad if we haven't already
                    CCTRACEE(_T("CDefsManager::SwitchToLatestDefs() - Used newest defs failed and we also failed to get current definitons."));
                    if( _Module.m_bDefsBad == false )
                    {
                        _Module.m_bDefsBad = true;

                        CGlobalEvent eventDefs;
                        eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
                        if ( eventDefs )
                            ::PulseEvent ( eventDefs );

                        // Alert the user
                        _Module.defAuthenticationFailure(AUTHENTICATION_FAILED);
                    }
                    return;
                }
            }
        }
        else if( bUseDefs )
        {
            // Check the last error code for the UseNewestDefs call in case defutils
            // reverted back to an old def set
            if( duResult == DU_S_AUTH_FAILED_USING_OLD )
            {
                CCTRACEE(_T("CDefsManager::SwitchToLatestDefs() - Def authentication reverted definitions sending the alert"));

                // Alert the user
                _Module.defAuthenticationFailure(AUTHENTICATION_REVERTED);
            }

            // The defs are good and this is the APP id we care about
            CCTRACEI(_T("CDefsManager::SwitchToLatestDefs() - Def authentication succeeded"));

            // If the defs were bad alert any def clients that they have changed
            if( _Module.m_bDefsBad )
            {
                CCTRACEI(_T("CDefsManager::SwitchToLatestDefs() - Def authentication succeeded. Defs were previously bad so will alert def clients."));
                _Module.m_bDefsBad = false;

                CGlobalEvent eventDefs;
                eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
                if ( eventDefs )
                    ::PulseEvent ( eventDefs );
            }
        }
    } // end UseNewestDefs

    // Only check authentication and dates if we are going to use these defs.
    //
    if ( bUseDefs )
    {
        // Get the defs directory we are using
        //
        if ( szTestDefs[0] )
            strncpy ( szDefsDir, szTestDefs, MAX_PATH );
        else
        {
            if( !pIDefUtils->GetCurrentDefs( szDefsDir, MAX_PATH ) )
            {
                DURESULT duRes = pIDefUtils->GetLastResult();
                CCTRACEE(_T("CDefsManager::SwitchToLatestDefs() - Failed to get the current defs directory. Error = %d"), duRes);
                return;
            }
        }

        CCTRACEI ("CDefsManager::SwitchToLatestDefs () - Newer defs?: %d", bNewer);
        CCTRACEI ("CDefsManager::SwitchToLatestDefs () - Current defs directory: %s", szDefsDir);

        // Get the date of the defs we are using.
        if (!pIDefUtils->GetCurrentDefsDate( &wYear, &wMonth, &wDay, &dwRev ) ||
            0 == wYear )
        {
            CCTRACEE ("CDefsManager::SwitchToLatestDefs - failure getting current def date, exiting");
            return;
        }

        // Remember this value so we can put it in the activity log
        //
        // YYYYMMDDRRRR = 12 chars
        //
        TCHAR szDefsRevision [13] = {0};    
        wsprintf ( szDefsRevision, "%04u%02u%02u%04u", wYear, wMonth, wDay, dwRev );
        _Module.Config().SetDefsRevision ( szDefsRevision );

        // Update the defs path in savrt
        if( !_Module.Config().UpdateSavrtDefsPath(szDefsDir) )
        {
            CCTRACEE(_T("Error updating the virus definitions path in SAVRT"));
        }

        std::string strDefsDir;
        strDefsDir = szDefsDir;
        updateDefsServicePaths( strDefsDir );

        CCTRACEI ("CDefsManager::SwitchToLatestDefs () - DefsDate: %d/%d/%d.%d", wMonth,wDay,wYear, dwRev);
    } // end bUseDefs
}

/////////////////////////////////////////////////////////////////////////////
// CDefsManager::UpdateDefsServicePaths()

void CDefsManager::updateDefsServicePaths( std::string& sDefsDir )
{
    CCTRACEI(_T("CDefsManager::updateDefsServicePaths() - Updating with path %s"), sDefsDir.c_str());
	std::string sFullPath;

	// Build full path to NAVEX.
	sFullPath = sDefsDir;
	sFullPath += _T('\\');
	sFullPath += NAVAP_NAVEX_SYS;

	// Change NAVEX device path.  Ignore error result
	if( !CServiceUtil::ChangeServicePath( NAVEX_SERVICE_NAME, sFullPath.c_str() ) )
        CCTRACEE(_T("CDefsManager::updateDefsServicePaths() - Failed to update the navex service registry path to %s"), sFullPath.c_str());
	
	// Build full path to NAVENG.
	sFullPath = sDefsDir;
	sFullPath += _T('\\');
	sFullPath += NAVAP_NAVENG_SYS;

	// Change NAVENG device path.  Ignore error result
	if( !CServiceUtil::ChangeServicePath( NAVENG_SERVICE_NAME, sFullPath.c_str() ) )
        CCTRACEE(_T("CDefsManager::updateDefsServicePaths() - Failed to update the naveng service registry path to %s"), sFullPath.c_str());
}

bool CDefsManager::GetRootDefsDirectory (LPTSTR pszDefsDir, int nMaxCount)
{
    TCHAR szDir[_MAX_PATH] = {0};
    DWORD dwType = 0;
    DWORD dwSize = (DWORD) nMaxCount;
    HKEY  hKey = NULL;
    
    // open "InstalledApps" key
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYMANTEC_INSTALLED_APPS_KEY, 0, KEY_READ, &hKey))
    {
        // check for "AVENGEDEFS" value
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, AVENGEDEFS_VAL, 0, &dwType, (LPBYTE)szDir, &dwSize))
        {
            if ((dwType == REG_SZ) && _tcslen(szDir))
            {
                // convert pathname to short name, if possible
                TCHAR szShort[_MAX_PATH];
                if (GetShortPathName(szDir, szShort, _MAX_PATH))
                    _tcsncpy(szDir, szShort, nMaxCount-1);
                    
                _tcscpy(pszDefsDir, szDir);
                
                RegCloseKey(hKey);
                return true;
            }
        }

        RegCloseKey(hKey);
    }

    return false;
}
