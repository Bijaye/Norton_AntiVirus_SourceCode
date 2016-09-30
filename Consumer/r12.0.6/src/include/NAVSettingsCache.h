// NAVSettingsCache.h -- prevides functionality of the former NAVSettingsHelperEx class
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <stdlib.h>
#include <map>
#include <string>
#include <atlstr.h>
#include <atlpath.h>
#include "cctrace.h"

#include "TraceHR.h"
#include "NAVSettings.h"

#include "simon.h"
#include "NAVBusinessRules.h"
#include "StahlSoft.h"
#include "SyncHelper.h"
#include "StahlSoft_EventHelper.h"
#include "NAVInfo.h"

// This class supports Unicode is a half-way: The value name is still LPCSTR because it is used in this format in many places of NAV code.

typedef std::map< std::basic_string<char>, ccSettings::ISettingsPtr > _NAVSETTINGSMAP;


class CNAVOptSettingsCache
{
private:

    CString m_strMainKeyName;
    DWORD m_dwMaxNameChars, m_dwMaxDataBytes;
	NAVToolbox::CCSettings* m_pCCSetting;
	bool m_bLocalCopy;
    bool m_bInitialized;
    
	_NAVSETTINGSMAP m_SettingsPtrMap;

	SIMON::CSimonPtr<NAV::ICalc> m_spCalc;
	StahlSoft::CSmartModuleHandle m_shBusinessRuleModule;
    CNAVInfo m_NAVInfo;
    BOOL m_bUseBusinessRules;

public:

    /////////////////////////////////////////////////////////////////////////////

	CNAVOptSettingsCache()
	{
		m_pCCSetting = new NAVToolbox::CCSettings();
		m_bLocalCopy = true;

        m_bInitialized = false;

        // Set the max values to 0
        m_dwMaxNameChars = m_dwMaxDataBytes = 0;

		m_bUseBusinessRules = TRUE;
	}

    /////////////////////////////////////////////////////////////////////////////

	CNAVOptSettingsCache(NAVToolbox::CCSettings& ccSetting)
	{
		m_pCCSetting = &ccSetting;
		m_bLocalCopy = false;

        m_bInitialized = false;

        // Set the max values to 0
        m_dwMaxNameChars = m_dwMaxDataBytes = 0;

		m_bUseBusinessRules = TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////

	virtual ~CNAVOptSettingsCache()
	{
		m_spCalc.Release(); // explicitly do this, because m_shBusinessRuleModule may destruct first and take out the object with it. 

	    Cleanup();	

		if (m_bLocalCopy)
        {
            // Need to release all the pointers before deleting the ccsettings class
            // since it will free the DLL
            _NAVSETTINGSMAP::iterator it;
            for( it = m_SettingsPtrMap.begin(); it != m_SettingsPtrMap.end(); ++it )
	        {
		        if ( (*it).second )
                {
                    (*it).second.Release();
                }
            }

			delete m_pCCSetting;
        }
	}

    /////////////////////////////////////////////////////////////////////////////
    // If clients pass in bBusinessRules as FALSE then the business model will not
    // be called to calculate the business rules.
	//
    // Initialize all of the settings objects we need for the
    // settings being loaded. Init() can be called a second time and it will
    // release all of the current settings objects (w/o saving) and create
    // a new group of settings objects. This will refresh the clients list
    // of settings with the most current ones in the settings manager

    BOOL Init(LPCTSTR lpcOptionPath = NULL, BOOL bBusinessRules = TRUE)
	{
		TRACEHR (h);

		try
		{
			if (!m_pCCSetting->Init())
				return FALSE;

			m_bUseBusinessRules = bBusinessRules;

			// The settings will be under Norton AntiVirus\\...
			m_strMainKeyName = _T("Norton AntiVirus\\");

			// Use the file name as the main key
			if(lpcOptionPath == NULL)
			{
				m_strMainKeyName += _T("NAVOPTS.DAT");
			}
			else
			{
				// Remove the path and just use the file name.
				CPath strPath(lpcOptionPath);
				strPath.StripPath();
				m_strMainKeyName += strPath;
			}

			// Open and save an ISettings object for each subkey
			CSymPtr<ccSettings::IEnumSettings> pEnumSettings;

			h %= m_strMainKeyName;

			h << m_pCCSetting->EnumSettings(m_strMainKeyName, &pEnumSettings);

			DWORD dwCount;
			pEnumSettings->GetCount( dwCount );

			h.Verify(dwCount > 0, SYMERR_UNKNOWN, _T("Settings do not have any subkeys"));

			CHAR szBuffer[MAX_PATH];

			for( DWORD i = 0; i < dwCount; i++ )
			{
				DWORD dwSize = MAX_PATH;
				h = pEnumSettings->GetItem( i, szBuffer, dwSize );

				if( SYM_SUCCEEDED(h) )
				{
					// Add the pointer to our map
					//

					// Need the full path to the setting we want
					CString strName = m_strMainKeyName + _T("\\") + szBuffer;

					ccSettings::ISettingsPtr pSettings;
					if( SYM_SUCCEEDED( h = m_pCCSetting->GetSettings(strName, &pSettings) ) )
					{
						// If there is already a settings object saved release it first

						std::basic_string<char> strFullSettingName = GetString(strName);

						if( (m_SettingsPtrMap[strFullSettingName]).m_p != NULL )
							(m_SettingsPtrMap[strFullSettingName]).Release();

						m_SettingsPtrMap[strFullSettingName] = pSettings;

						// Set the max values if necessary
						DWORD dwMaxNameChars, dwMaxDataBytes = 0;
						pSettings->GetMaxSizes(dwMaxNameChars, dwMaxDataBytes);
						if( dwMaxNameChars > m_dwMaxNameChars )
							m_dwMaxNameChars = dwMaxNameChars;
						if( dwMaxDataBytes > m_dwMaxDataBytes )
							m_dwMaxDataBytes = dwMaxDataBytes;
					}
				}
				else
				{
					h /= szBuffer;
				}
			}

			m_bInitialized = true;
		}
		catch (_com_error &e)
		{
			h = e;
			return FALSE;
		}

        return TRUE;
	}

    /////////////////////////////////////////////////////////////////////////////
    // Description:	Returns the maximum number of characters for settings names
    //              and data in the hive.

    void GetMaxNavSettingsSizes(DWORD &dwMaxNameChars, DWORD &dwMaxDataBytes)
    {
        dwMaxNameChars = m_dwMaxNameChars;
        dwMaxDataBytes = m_dwMaxDataBytes;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Description:	clean anything extra up here

	void Cleanup()
    {
        m_strMainKeyName.Empty();

        m_bInitialized = false;

        // Set the max values back to 0
        m_dwMaxNameChars = m_dwMaxDataBytes = 0;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Synchronized save function - the base class will merge and save the dat file
    // then the buisness rules will be calculated on the new option values
    //

    BOOL Save()
	{
		TRACEHR (h);

        BOOL bSave = TRUE;
        BOOL bBusinessRulesSucceeded = TRUE;
        BOOL bRet = TRUE;

        if( !m_bInitialized )
        {
			h /= _T("Settings have not been initialized.");
            return FALSE;
        }

        try
        {
            // If the business rules should be calculated call the object to do
            // the work
            if( m_bUseBusinessRules == TRUE )
            {
                h %= _T("Using the business rules");

                if(m_spCalc == NULL)
			    {
				    // Load the module.
                    TCHAR szBusinessRulesModulePath[_MAX_PATH + 2] = {0};
                    _tcscpy(szBusinessRulesModulePath,m_NAVInfo.GetNAVDir());
			        _tcscat(szBusinessRulesModulePath,_T("\\NavOptRF.dll"));

				    m_shBusinessRuleModule = ::LoadLibrary(szBusinessRulesModulePath);
				    h <<  SIMON::CreateInstanceByDLLInstance(
									    m_shBusinessRuleModule
								    ,NAV::CLSID_CNAVBusinessRules
								    ,NAV::IID_ICalc
								    ,(void**)&m_spCalc);
			    }

                h << m_spCalc->Calc(this);

	           }
        }
        catch( _com_error &e)
        {
			h = e;
            bBusinessRulesSucceeded = FALSE;
        }

        h %= _T("Saving the settings");

        _NAVSETTINGSMAP::iterator it;
        for( it = m_SettingsPtrMap.begin(); it != m_SettingsPtrMap.end(); ++it )
	    {
		    if ( (*it).second )
            {
                if( SYM_FAILED(h = m_pCCSetting->PutSettings( (*it).second )) )
                {
                    h /= (*it).first.c_str();

                    // Return FALSE since not all of the setting were saved properly
                    bRet = FALSE;
                }
            }
	    }

        // If an error occured during the business rules calculation or the actual save return the error
        return bRet && bBusinessRulesSucceeded;
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	GetValue
    //
    // Description:	Read value data of the given name in navopts.dat
    //				Return S_OK if succeeded otherwize E_
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetValue(LPCSTR lpszName, DWORD& dwVal, DWORD dwDefault)
    {
		TRACEHR (h);

		if( !m_bInitialized )
        {
			h /= _T("Settings have not been initialized.");
            return E_FAIL;
        }

        CString strValueName;
        std::basic_string<char> strSettingMapName;

        GetSettingsNames(lpszName, strSettingMapName, strValueName);

        if( !m_SettingsPtrMap[strSettingMapName] || SYM_FAILED(h = m_SettingsPtrMap[strSettingMapName]->GetDword(strValueName, dwVal)) )
        {
            // Use default
            dwVal = dwDefault;
            h = E_FAIL;
        }

		CCTRACEI(_T("%s -> %d"), (LPCTSTR)CString(lpszName), dwVal);
        return h;
    }

    /////////////////////////////////////////////////////////////////////////////

	HRESULT GetValue(LPCSTR lpszName, LPTSTR pszVal, DWORD &dwSize, LPCTSTR szDefault)
    {
		TRACEHR (h);

		if( !m_bInitialized )
        {
			h /= _T("Settings have not been initialized.");
            return E_FAIL;
        }

        CString strValueName;
        std::basic_string<char> strSettingMapName;
        DWORD dwOrigSize = dwSize;

        GetSettingsNames(lpszName, strSettingMapName, strValueName);

        if( !m_SettingsPtrMap[strSettingMapName] || SYM_FAILED(h = m_SettingsPtrMap[strSettingMapName]->GetString(strValueName, pszVal, dwSize)) )
        {
            // Use default
            _tcsncpy(pszVal, szDefault, dwOrigSize);
            h = E_FAIL;
        }

		CCTRACEI(_T("%s -> %s"), (LPCTSTR)CString(lpszName), pszVal);
        return h;
    }

    /////////////////////////////////////////////////////////////////////////////

	HRESULT GetValue(LPCSTR lpszName, LPBYTE pBuf, DWORD &dwSize)
    {
		TRACEHR (h);

		if( !m_bInitialized )
        {
			h /= _T("Settings have not been initialized.");
            return E_FAIL;
        }

        CString strValueName;
        std::basic_string<char> strSettingMapName;

        GetSettingsNames(lpszName, strSettingMapName, strValueName);

        if( !m_SettingsPtrMap[strSettingMapName] || SYM_FAILED(h = m_SettingsPtrMap[strSettingMapName]->GetBinary(strValueName, pBuf, dwSize)) )
        {
            h = E_FAIL;
        }

		CCTRACEI(_T("%s -> [%d]"), (LPCTSTR)CString(lpszName), dwSize);
        return h;
    }
    
    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	SetValue
    //
    // Description:	Set the value data of a value
    //				Return S_OK if succeeded otherwize E_
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT SetValue(LPCSTR lpszName, DWORD dwVal)
    {
		TRACEHR (h);

		if( !m_bInitialized )
        {
			h /= _T("Settings have not been initialized.");
            return E_FAIL;
        }

        CString strValueName;
        std::basic_string<char> strSettingMapName;

        GetSettingsNames(lpszName, strSettingMapName, strValueName);
        
        if( !m_SettingsPtrMap[strSettingMapName] || SYM_FAILED(h = m_SettingsPtrMap[strSettingMapName]->PutDword(strValueName, dwVal)) )
        {
            h = E_FAIL;
        }

		CCTRACEI(_T("%s <- %d"), (LPCTSTR)CString(lpszName), dwVal);
        return h;
    }

    /////////////////////////////////////////////////////////////////////////////

	HRESULT SetValue(LPCSTR lpszName, LPCTSTR pszVal)
    {
		TRACEHR (h);

		if( !m_bInitialized )
        {
			h /= _T("Settings have not been initialized.");
            return E_FAIL;
        }

		CString strValueName;
        std::basic_string<char> strSettingMapName;

        GetSettingsNames(lpszName, strSettingMapName, strValueName);
        
        if( !m_SettingsPtrMap[strSettingMapName] || SYM_FAILED(h = m_SettingsPtrMap[strSettingMapName]->PutString(strValueName, pszVal)) )
        {
            h = E_FAIL;
        }

        // If this increases the max data size then update our internal values
		DWORD dwMaxNameChars, dwMaxDataBytes = 0;
        m_SettingsPtrMap[strSettingMapName]->GetMaxSizes(dwMaxNameChars, dwMaxDataBytes);
		if( dwMaxNameChars > m_dwMaxNameChars )
			m_dwMaxNameChars = dwMaxNameChars;
		if( dwMaxDataBytes > m_dwMaxDataBytes )
			m_dwMaxDataBytes = dwMaxDataBytes;

		CCTRACEI(_T("%s <- %s"), (LPCTSTR)CString(lpszName), pszVal);
        return h;
    }

    /////////////////////////////////////////////////////////////////////////////

	HRESULT SetValue(LPCSTR lpszName, LPBYTE pBuf, DWORD dwSize)
    {
		TRACEHR (h);

		if( !m_bInitialized )
        {
			h /= _T("Settings have not been initialized.");
            return E_FAIL;
        }

        CString strValueName;
        std::basic_string<char> strSettingMapName;

        GetSettingsNames(lpszName, strSettingMapName, strValueName);
        
        if( !m_SettingsPtrMap[strSettingMapName] || SYM_FAILED(h = m_SettingsPtrMap[strSettingMapName]->PutBinary(strValueName, pBuf, dwSize)) )
        {
            CCTRACEE(_T("Unable to set the setting for %s."), (LPCTSTR)strValueName);
            h = E_FAIL;
        }

        // If this increases the max data size then update our internal values
		DWORD dwMaxNameChars, dwMaxDataBytes = 0;
        m_SettingsPtrMap[strSettingMapName]->GetMaxSizes(dwMaxNameChars, dwMaxDataBytes);
		if( dwMaxNameChars > m_dwMaxNameChars )
			m_dwMaxNameChars = dwMaxNameChars;
		if( dwMaxDataBytes > m_dwMaxDataBytes )
			m_dwMaxDataBytes = dwMaxDataBytes;

		CCTRACEI(_T("%s <- [%d]"), (LPCTSTR)CString(lpszName), dwSize);
        return h;
    }

    /////////////////////////////////////////////////////////////////////////////
    // This will allow the options object the ability to enumerate the settings
    // using ccSettings directly
    void GetSettingsMap(_NAVSETTINGSMAP **pMap)
    {
        if( pMap )
            (*pMap) = &m_SettingsPtrMap;
    }
	
	/////////////////////////////////////////////////////////////////////////////
    // This will allow the install to turn off business rule usage for its use
    // of NAVoptions
    void UseBusinessRules(BOOL bUseRules)
    {
		m_bUseBusinessRules = bUseRules;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Restore from an existing settings hive to the current one
    bool Migrate(CNAVOptSettingsCache& other, bool bIgnoreDRMKeys = false)
    {
		TRACEHR (h);

		try
		{
			// First make sure the other settings are initialized
			h.Verify(other.m_bInitialized, E_FAIL, _T("Settings have not been initialized."));

			// If these settings are not initialized then we need to create them
			bool bCreateSettings = false;
			if( !m_bInitialized )
			{
				// If initialization has never been attempted then we must give up since we don't
				// know where to store these values to
				h.Verify(!m_strMainKeyName.IsEmpty(), E_FAIL, _T("Init() has not been called."));

				// We know the key name so create the settings that we'll need during the migration
				bCreateSettings = true;

				// Set the max sizes
				m_dwMaxNameChars = other.m_dwMaxNameChars;
				m_dwMaxDataBytes = other.m_dwMaxDataBytes;
			}

			h %= _T("Walk through the settings values");

			// Walk through the other settings values
			for( _NAVSETTINGSMAP::iterator it = other.m_SettingsPtrMap.begin(); it != other.m_SettingsPtrMap.end(); it++ )
			{
				if( it->first.empty() )
				{
					CCTRACEI(_T("CNAVOptSettings::Migrate() - Encountered an empty settings key."));
					continue;
				}

				if( it->second == NULL )
				{
					CCTRACEI(_T("CNAVOptSettings::Migrate() - Encountered an NULL settings pointer."));
					continue;
				}

				// If we're ignoring the DRM keys then skip it now
				if( bIgnoreDRMKeys && IsDRMSettingKey(it->first.c_str()) )
				{
					CCTRACEI(_T("CNAVOptSettings::Migrate() - Ignoring DRM Key: %s"), it->first.c_str());
					continue;
				}

				if( bCreateSettings )
				{
					// This settings object doesn't exist so create it
					std::basic_string<char> strKeyName = GetString(m_strMainKeyName);
					strKeyName += it->first.substr(it->first.find_last_of("\\"));

					// Make sure the setting doesn't already exist
					if( !m_SettingsPtrMap[strKeyName] )
					{
						h << m_pCCSetting->CreateSettings(strKeyName.c_str(), &m_SettingsPtrMap[strKeyName]);
						m_bInitialized = true;
					}
				}

				CSymPtr<ccSettings::IEnumValues> pEnumValues;
				if( SYM_SUCCEEDED((it->second)->EnumValues(&pEnumValues)) )
				{
					// Get all the data for these values
					DWORD dwCount = 0;
					if( SYM_SUCCEEDED( pEnumValues->GetCount(dwCount) ) )
					{
						// If there are no values move to the next one
						if( dwCount == 0 )
							continue;

						// Allocate a buffer big enough to hold the largest possible setting name
						TCHAR *pszItemName = new TCHAR[other.m_dwMaxNameChars];
						h.Verify(pszItemName != NULL, SYMERR_OUTOFMEMORY);

						// Get all of the values and set them ...
						for(DWORD i=0; i<dwCount && pszItemName; i++)
						{
							DWORD dwSize = other.m_dwMaxNameChars;
							DWORD dwType = 0;
							LPTSTR pszNewVal = NULL;
	                        
							if( SYM_SUCCEEDED(pEnumValues->GetItem(i, pszItemName, dwSize, dwType)) )
							{
								// Get the navopt name
								std::basic_string<char> strNavOptName;
								GetNavoptName(it->first.c_str(), (LPCSTR)pszItemName, strNavOptName);

								if(ccSettings::SYM_SETTING_DWORD == dwType)
								{
									DWORD dwVal = 0;
									if( SUCCEEDED(other.GetValue(strNavOptName.c_str(), dwVal, 0)) )
										SetValue(strNavOptName.c_str(), dwVal);
									else
									{
										CCTRACEE(_T("CNAVOptSettings::Migrate() - Failed to retrieve setting for %s, from %s key"), (LPCTSTR)CString(strNavOptName.c_str()), (LPCTSTR)m_strMainKeyName);
									}
								}
								if(ccSettings::SYM_SETTING_STRING == dwType)
								{
									LPTSTR szVal = new TCHAR[other.m_dwMaxDataBytes];

									if( !szVal )
									{
										CCTRACEE(_T("CNAVOptSettings::Migrate() - memory allocation failed for szVal!"));
										continue;
									}

									dwSize = other.m_dwMaxDataBytes;
									if( SUCCEEDED(other.GetValue(strNavOptName.c_str(), szVal, dwSize, _T(""))) )
										SetValue(strNavOptName.c_str(), szVal);
									else
									{
										CCTRACEE(_T("CNAVOptSettings::Migrate() - Failed to retrieve setting for %s, from %s key"), (LPCTSTR)CString(strNavOptName.c_str()), (LPCTSTR)m_strMainKeyName);
									}

									if(szVal)
										delete [] szVal;
								}
								if(ccSettings::SYM_SETTING_BINARY == dwType)
								{
									LPBYTE pBufVal = new BYTE[other.m_dwMaxDataBytes];

									if( !pBufVal )
									{
										CCTRACEE(_T("CNAVOptSettings::Migrate() - memory allocation failed for pBufVal!"));
										continue;
									}

									dwSize = other.m_dwMaxDataBytes;
									if( SUCCEEDED(other.GetValue(strNavOptName.c_str(), pBufVal, dwSize)) )
										SetValue(strNavOptName.c_str(), pBufVal, dwSize);
									else
									{
										CCTRACEE(_T("CNAVOptSettings::Migrate() - Failed to retrieve setting for %s, from %s key"), (LPCTSTR)CString(strNavOptName.c_str()), (LPCTSTR)other.m_strMainKeyName);
									}

									if(pBufVal)
										delete [] pBufVal;
								}
							}
						}
	                    
						if( pszItemName )
							delete [] pszItemName;
					}
				}
			}

	        
			// Save the migrated values
			if( !Save() )
			{
				CCTRACEE(_T("CNavOptSettings::Migrate() - Failed to save the newly migrated settings."));
				return false;
			}

		}
		catch (_com_error &e)
		{
			h = e;
			m_bInitialized = false;
			return false;
		}

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Takes a ccSetting key and value name. Returns the colon delimited NAVOPTS
    // primitive version of the name
    static void GetNavoptName(LPCSTR pcszCCSettingKey, LPCSTR pcsValueName, std::basic_string<char> &strOptName)
    {
        // Get the primitive name by preceding the item name with the map entry name
        // last part and a colon
        std::basic_string<char> strKey = pcszCCSettingKey;
        std::basic_string<char>::size_type nLastSlash = strKey.find_last_of("\\");
        strOptName = strKey.substr(nLastSlash+1);
        strOptName += ":";
        strOptName += pcsValueName;
    }

private:

    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	GetSettingsNames
    //
    // Description:	Takes an options primitive formated name lpszColonName and
    //              returns the settings map index string and the value name
    //              to be used with ccSettings functions
    //
    /////////////////////////////////////////////////////////////////////////////
    void GetSettingsNames(LPCSTR lpszColonName, std::basic_string<char> &strSettingMap, CString &strValueName)
    {
        CString name = m_strMainKeyName + _T("\\") + lpszColonName;

		int splitIndex = name.Find(_T(':'));
		if (splitIndex > 0)
		{
			strSettingMap = GetString(name.Left(splitIndex));
			strValueName = name.Right(name.GetLength() - splitIndex - 1);
		}
		else
		{
			CCTRACEW(_T("Invalid settings name: %s"), (LPCTSTR)name);
			strSettingMap = GetString(name);
			strValueName = _T("Default");
		}
    }

    /////////////////////////////////////////////////////////////////////////////
    // Helper function to determine if the settings key is a DRM specific key
    bool IsDRMSettingKey( LPCSTR pcszItemName )
    {
        // If the last item name in the key is "DRM" then this is a DRM specific key
        std::basic_string<char> strKey = pcszItemName;
        std::basic_string<char>::size_type nLastSlash = strKey.find_last_of("\\");
        
        if( strKey.substr(nLastSlash+1).compare("DRM") == 0 )
            return true;

        return false;
    }

	inline static LPCSTR GetString(LPCTSTR str)
	{
#ifdef UNICODE
		return (LPCSTR)CStringA(str);
#else
		return str;
#endif
	}
};

