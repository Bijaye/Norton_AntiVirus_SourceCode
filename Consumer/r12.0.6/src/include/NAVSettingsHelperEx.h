// NAVSettingsHelperEx.H
// This class does not cache the settings.
//
// For former NAVSettingsHelperEx implementation see NAVSettingsCache. 
//////////////////////////////////////////////////////////////////////

#pragma once

#include <atlstr.h>
#include <atlpath.h>
#include "cctrace.h"
#include "TraceHR.h"
#include "NAVSettings.h"

class CNAVOptSettingsEx
{
public:
    typedef std::map< CString, ccSettings::ISettingsPtr > _NAVSETTINGSMAP;

private:
	NAVToolbox::CCSettings* m_pCCSetting;
	bool m_bLocalCopy;
	_NAVSETTINGSMAP m_SettingsPtrMap;
    DWORD m_dwMaxNameChars, m_dwMaxDataBytes;
    CString m_strMainKeyName;

public:

	/////////////////////////////////////////////////////////////////////////////

	CNAVOptSettingsEx()
	{
		m_pCCSetting = new NAVToolbox::CCSettings();
		m_bLocalCopy = true;
        m_strMainKeyName = _T("Norton AntiVirus\\NAVOPTS.DAT");
	}

	/////////////////////////////////////////////////////////////////////////////

	CNAVOptSettingsEx(NAVToolbox::CCSettings& ccSetting)
	{
		m_pCCSetting = &ccSetting;
		m_bLocalCopy = false;
        m_strMainKeyName = _T("Norton AntiVirus\\NAVOPTS.DAT");
	}

	/////////////////////////////////////////////////////////////////////////////

	virtual ~CNAVOptSettingsEx()
	{
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
            m_pCCSetting = NULL;
        }
	}

    // The cleanup method will force a release of the local setting copy after calling
    // this method you can no longer make calls into the object, this is here in case
    // you need to free up all the ccSettings nonsense before the constructor is hit
    void Cleanup()
	{
		if (m_bLocalCopy && m_pCCSetting)
        {
            CCTRACEI(_T("CNAVOptSettingsEx::Cleanup() - Freeing the settings object."));
			delete m_pCCSetting;
            m_pCCSetting = NULL;
        }
	}

	/////////////////////////////////////////////////////////////////////////////

    bool Init()
	{
        if( m_pCCSetting == NULL )
        {
            // If we have a local copy that is NULL someone has cleaned up, so
            // re-initialize
            if (m_bLocalCopy)
            {
                CCTRACEI(_T("CNAVOptSettingsEx::Init() - No internal settings object. Will attempt to create a new one."));
                m_pCCSetting = new NAVToolbox::CCSettings();
                if( m_pCCSetting == NULL )
                {
                    CCTRACEE(_T("CNAVOptSettingsEx::Init() - Failed to created new internal settings object"));
                    return false;
                }
            }
            else
            {
                CCTRACEE(_T("CNAVOptSettingsEx::Init() - No internal settings object"));
                return false;
            }
        }

		return m_pCCSetting->Init();
	}

	/////////////////////////////////////////////////////////////////////////////

    // Saves all current settings to ccSettingsManager
    //
    bool Save (void)
    {
        TRACEHR (h);
        bool bRet = true;

        _NAVSETTINGSMAP::iterator it;
        for( it = m_SettingsPtrMap.begin(); it != m_SettingsPtrMap.end(); ++it )
	    {
		    if ( (*it).second )
            {
                if( SYM_FAILED(h = m_pCCSetting->PutSettings( (*it).second )) )
                {
                    h /= (*it).first;

                    // Return false since not all of the setting were saved properly
                    bRet = false;
                }
            }
	    }

        return bRet;
    }

	/////////////////////////////////////////////////////////////////////////////

    HRESULT GetValue(LPCTSTR lpszName, DWORD& dwVal, DWORD dwDefault)
	{
		TRACEHR (h);
		h %= lpszName;

        ccSettings::ISettingsPtr pSettings;
        CString strValueName;

        if ( SYM_SUCCEEDED ( GetSettingsPtr ( lpszName, strValueName, pSettings )))
        {
            if ( SYM_FAILED (pSettings->GetDword(strValueName, dwVal)))
            {
                // Use default
                dwVal = dwDefault;
                h = E_FAIL;
            }
        }
        else
            h = E_FAIL;

		CCTRACEI(_T("%s -> %d"), lpszName, dwVal);
		return h;
	}

	/////////////////////////////////////////////////////////////////////////////

    HRESULT GetValue(LPCTSTR lpszName, LPTSTR pszVal, DWORD dwSize, LPCTSTR szDefault)
    {
		TRACEHR (h);
		h %= lpszName;

        ccSettings::ISettingsPtr pSettings;
        CString strValueName;
        DWORD dwOrigSize = dwSize;

        if ( SYM_SUCCEEDED ( GetSettingsPtr ( lpszName, strValueName, pSettings )))
        {
            if ( SYM_FAILED (pSettings->GetString(strValueName, pszVal, dwSize)))
            {
                // Use default
                _tcsncpy(pszVal, szDefault, dwOrigSize);
                h = E_FAIL;
            }
        }
        else
            h = E_FAIL;

		CCTRACEI(_T("%s -> %s"), lpszName, pszVal);
		return h;
    }

	/////////////////////////////////////////////////////////////////////////////

    HRESULT GetValue(LPCTSTR lpszName, LPBYTE pBuf, DWORD &dwSize)
    {
        TRACEHR (h);
        h %= lpszName;

        ccSettings::ISettingsPtr pSettings;
        CString strValueName;

        if ( SYM_SUCCEEDED ( GetSettingsPtr ( lpszName, strValueName, pSettings )))
        {
            if ( SYM_FAILED (pSettings->GetBinary(strValueName, pBuf, dwSize)))
            {
                h = E_FAIL;
            }
        }
        else
            h = E_FAIL;

        CCTRACEI(_T("%s -> [%d]"), lpszName, dwSize);
		return h;
    }

	/////////////////////////////////////////////////////////////////////////////

    HRESULT SetValue(LPCTSTR lpszName, DWORD dwVal, bool bForceSave = true )
	{
		TRACEHR (h);
		h %= lpszName;

        ccSettings::ISettingsPtr pSettings;
        CString strValueName;

        if ( SYM_SUCCEEDED ( GetSettingsPtr ( lpszName, strValueName, pSettings )))
        {
            if ( SYM_FAILED (pSettings->PutDword(strValueName, dwVal)))
            {
                h = E_FAIL;
            }
            else
            {
                if ( bForceSave )
                {
                    h = m_pCCSetting->PutSettings( pSettings );
                }
            }
        }
        else
            h = E_FAIL;

        CCTRACEI(_T("%s <- %d"), lpszName, dwVal);

		return h;
	}

	/////////////////////////////////////////////////////////////////////////////

    HRESULT SetValue(LPCTSTR lpszName, LPCTSTR pszVal, bool bForceSave = true)
	{
		TRACEHR (h);
		h %= lpszName;

        ccSettings::ISettingsPtr pSettings;
        CString strValueName;

        if ( SYM_SUCCEEDED ( GetSettingsPtr ( lpszName, strValueName, pSettings )))
        {
            if ( SYM_FAILED (pSettings->PutString(strValueName, pszVal)))
            {
                h = E_FAIL;
            }
            else
            {
                if ( bForceSave )
                {
                    h = m_pCCSetting->PutSettings( pSettings );
                }

                // If this increases the max data size then update our internal values
		        DWORD dwMaxNameChars, dwMaxDataBytes = 0;
                pSettings->GetMaxSizes(dwMaxNameChars, dwMaxDataBytes);
		        if( dwMaxNameChars > m_dwMaxNameChars )
			        m_dwMaxNameChars = dwMaxNameChars;
		        if( dwMaxDataBytes > m_dwMaxDataBytes )
			        m_dwMaxDataBytes = dwMaxDataBytes;
            }
        }
        else
            h = E_FAIL;

		CCTRACEI(_T("%s <- %s"), lpszName, pszVal);
		return h;
	}

	/////////////////////////////////////////////////////////////////////////////

    HRESULT SetValue(LPCTSTR lpszName, LPBYTE pBuf, DWORD dwSize, bool bForceSave = true)
	{
		TRACEHR (h);
		h %= lpszName;

        ccSettings::ISettingsPtr pSettings;
        CString strValueName;

        if ( SYM_SUCCEEDED ( GetSettingsPtr ( lpszName, strValueName, pSettings )))
        {
            if ( SYM_FAILED (pSettings->PutBinary(strValueName, pBuf, dwSize)))
            {
                h = E_FAIL;
            }
            else
            {
                if ( bForceSave )
                {
                    h = m_pCCSetting->PutSettings( pSettings );
                }

                // If this increases the max data size then update our internal values
		        DWORD dwMaxNameChars, dwMaxDataBytes = 0;
                pSettings->GetMaxSizes(dwMaxNameChars, dwMaxDataBytes);
		        if( dwMaxNameChars > m_dwMaxNameChars )
			        m_dwMaxNameChars = dwMaxNameChars;
		        if( dwMaxDataBytes > m_dwMaxDataBytes )
			        m_dwMaxDataBytes = dwMaxDataBytes;
            }
        }
        else
            h = E_FAIL;

		CCTRACEI(_T("%s <- [%d]"), lpszName, dwSize);

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

    HRESULT GetString(LPCTSTR lpszName, CString& str)
	{
		TRACEHR (h);
		h %= lpszName;

        ccSettings::ISettingsPtr pSettings;
        CString strValueName;

        if ( SYM_SUCCEEDED ( GetSettingsPtr ( lpszName, strValueName, pSettings )))
        {
		    LPTSTR buf = NULL;
		    try
		    {
			    DWORD dwMaxNameChars;
			    DWORD dwMaxDataBytes;
			    h << pSettings->GetMaxSizes(dwMaxNameChars, dwMaxDataBytes);

                // Pre-allocate a buffer big enough to hold the string with an extra spot for a NULL
                // terminator to be safe
                DWORD dwStrLength = (dwMaxDataBytes*sizeof(TCHAR)) + 1;
                str.Preallocate(dwStrLength+1);
			    pSettings->GetString(strValueName, str.GetBuffer(dwStrLength), dwStrLength);
                str.ReleaseBuffer();
		    }
		    catch (...)
		    {
			    h = E_FAIL;
		    }

		    if (buf != NULL)
			    delete[] buf;
        }
        else
            h = E_FAIL;

		CCTRACEI(_T("%s -> %s"), lpszName, (LPCTSTR)str);
		return h;
	}

	/////////////////////////////////////////////////////////////////////////////

	CString GetString(LPCTSTR lpszName)
	{
		CString str;
		GetString(lpszName, str);
		return str;
	}

	/////////////////////////////////////////////////////////////////////////////

private:

	void GetSettingsNames(  LPCTSTR lpColonValue/*in - e.g. AUTOPROTECT:UserWantsOn */,
                            CString& strSettingMapName/*out - e.g. AUTOPROTECT */,
                            CString& valueName/*out - e.g. UserWantsOn */)
	{
		TRACEHR (h);

		CString colonName(lpColonValue);

		int splitIndex = colonName.Find(_T(':'));
		h.Verify(splitIndex > 0);

		valueName = colonName.Right(colonName.GetLength() - splitIndex - 1);
        strSettingMapName = colonName.Left(splitIndex);
	}

    // Retrieves the settings pointer from the map. If it's not there it loads the data.
    //
    SYMRESULT GetSettingsPtr ( LPCTSTR lpColonValue, CString& strValueName, ccSettings::ISettingsPtr& pSettings )
    {
        SYMRESULT sr = SYMERR_UNKNOWN;

        pSettings = NULL;

        if( m_pCCSetting == NULL )
        {
            CCTRACEE(_T("CNAVOptSettingsEx::GetValue() - No internal settings object"));
            return SYMERR_UNKNOWN;
        }

		TRACEHR (h);
		h %= lpColonValue;

        //std::basic_string<char> strSettingMapName;
        CString strSettingMapName;

        GetSettingsNames(lpColonValue, strSettingMapName, strValueName);

        if ( m_SettingsPtrMap[strSettingMapName] )
        {
            pSettings = m_SettingsPtrMap[strSettingMapName];
            sr = SYM_OK;
        }
        else
        {
            // Can't find it in the map, load it.
            //
            if ( SYM_SUCCEEDED ( LoadSettings ( strSettingMapName )))
            {
                if ( m_SettingsPtrMap[strSettingMapName] )
                {
                    pSettings = m_SettingsPtrMap[strSettingMapName];
                    sr = SYM_OK;
                }
            }
        }

        return sr;
    }

    /////////////////////////////////////////////////////////////////////////////

    // Loads a full key into our map
    //
    SYMRESULT LoadSettings ( CString& strSettingMapName )
    {
		TRACEHR (h);

		// Need the full path to the setting we want
		CString strFullName = m_strMainKeyName + _T("\\") + strSettingMapName;

		ccSettings::ISettingsPtr pSettings;
		if( SYM_SUCCEEDED( h = m_pCCSetting->GetSettings ( strFullName, &pSettings)))
		{
			// If there is already a settings object saved release it first
			if ((m_SettingsPtrMap[strSettingMapName]).m_p != NULL )
				(m_SettingsPtrMap[strSettingMapName]).Release();

		    // Add the pointer to our map
		    //
			m_SettingsPtrMap[strSettingMapName] = pSettings;

			// Set the max values if necessary
			DWORD dwMaxNameChars, dwMaxDataBytes = 0;
			pSettings->GetMaxSizes(dwMaxNameChars, dwMaxDataBytes);
			if( dwMaxNameChars > m_dwMaxNameChars )
				m_dwMaxNameChars = dwMaxNameChars;
			if( dwMaxDataBytes > m_dwMaxDataBytes )
				m_dwMaxDataBytes = dwMaxDataBytes;

		}

        return h;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

#define READ_SETTING_START()                                 \
        {                                                   \
        StahlSoft::HRX hrx;                                 \
        CNAVOptSettingsEx navFeature;                           \
        hrx << (SUCCEEDED(navFeature.Init())?S_OK:E_FAIL);  \
		DWORD dwValue = 0 ;

#define READ_SETTING_bool(_name,_var,_default)                                       \
    _var = _default;                                                                \
    if(SUCCEEDED(navFeature.GetValue(_name,dwValue,static_cast<DWORD>(_default))))  \
        _var = (dwValue != 0);

#define READ_SETTING_DWORD(_name,_var,_default)                                      \
    _var = _default;                                                                \
    if(SUCCEEDED(navFeature.GetValue(_name,dwValue,static_cast<DWORD>(_default))))  \
        _var = dwValue;

#define READ_SETTING_long(_name,_var,_default)                                       \
    _var = _default;                                                                \
    if(SUCCEEDED(navFeature.GetValue(_name,dwValue,static_cast<DWORD>(_default))))  \
        _var = static_cast<long>(dwValue);

#define READ_SETTING_STRING(_name,_var,_size, _default)	\
    navFeature.GetValue(_name,_var,_size,_default);	\

#define READ_SETTING_END }

