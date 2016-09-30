////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EmailOptions.h"
#include "NAVTrust.h"
#include "NavOptionsConstants.h"
#include "NAVSettingsCache.h"

#define SZ_OEHEUR_DLL                          _T("OEHeur.dll")
#define SZ_OEHEUR_GUID						   _T("{4AA748DA-9D8A-4271-B507-DB5616B93844}")

CEmailOptions::CEmailOptions():
m_bInitialized(false), m_bDirty(false), m_hccEmlPxy(NULL),
m_bEmlTray(false), m_bEmlProgress(false), m_bEmlTimeout(false)
{}

CEmailOptions::~CEmailOptions()
{
	if( m_pEmailOptions != NULL )
		m_pEmailOptions.Release();

    if (m_hccEmlPxy)
        ::FreeLibrary(m_hccEmlPxy);

	if(m_spAVEmail)
		m_spAVEmail.Release();
}

bool CEmailOptions::IsDirty() { return m_bDirty; }

// Call this to initialize Email Options.
// Will create a new cached settings object if we're not operating
// via a borrowed settings object.
// This method MUST be called before this object can be used!
HRESULT CEmailOptions::Initialize()
{
    HRESULT hr;
    ISymFactory* pFactory = NULL;
    pfnGETFACTORY pGetFactory = NULL;
    ccLib::CString szEmlPxyPath;

    if(m_bInitialized)
        return S_FALSE;

    ccSym::CInstalledApps::GetCCDirectory( szEmlPxyPath );
    szEmlPxyPath.Append(_T("\\ccEmlPxy.dll"));

    // Initialize Email Options object
    // Validate the signature
    if( NAVToolbox::IsSymantecSignedImage(szEmlPxyPath) )
    {
        return E_ACCESSDENIED;
    }

    m_hccEmlPxy = LoadLibraryEx(szEmlPxyPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if (m_hccEmlPxy == NULL)
    {
        return E_FAIL;
    }

    // Get the options object ID.
    pfnGETEMAILOPTIONSOBJECTID pGetEmailOptionsObjectID =
        (pfnGETEMAILOPTIONSOBJECTID) GetProcAddress(m_hccEmlPxy, "GetEmailOptionsObjectID");
    SYMOBJECT_ID idObjectId;

	hr = S_OK;

    // Fugly direct-loading code
    if (pGetEmailOptionsObjectID != NULL)
    {
        if (pGetEmailOptionsObjectID(&idObjectId) == SYM_OK)
        {
            // Create the options object using its factory.

            pGetFactory = (pfnGETFACTORY) GetProcAddress(m_hccEmlPxy, "GetFactory");

            if (pGetFactory != NULL)
            {
                if (pGetFactory(idObjectId, &pFactory) == SYM_OK)
                {
                    if (pFactory->CreateInstance(IID_EmailOptions3, (void**) &m_pEmailOptions) != SYM_OK)
                        hr = E_FAIL;
                }
                else
                    hr = E_FAIL;
            }
            else
                hr = E_FAIL;
        }
        else
            hr = E_FAIL;
    }
    else
        hr = E_FAIL;

    // On failure, release our stuff.
    if( FAILED (hr) || !m_pEmailOptions )
    {
        if( pFactory )
		{
            pFactory->Release();
			pFactory = NULL;
		}

        if( m_pEmailOptions )
            m_pEmailOptions.Release();

		if(m_hccEmlPxy)
		{
			::FreeLibrary(m_hccEmlPxy);
			m_hccEmlPxy = NULL;
		}

		return E_FAIL;
    }

    // All is good
    pFactory->Release();

    // Store the current values for each of the email options
    m_pEmailOptions->GetValue(CCEMLPXY_OUTGOING_PROGRESS, m_bEmlProgress);
    m_pEmailOptions->GetValue(CCEMLPXY_TRAY_ANIMATION, m_bEmlTray);
    m_pEmailOptions->GetValue(CCEMLPXY_TIMEOUT_PROTECTION, m_bEmlTimeout);

	// Get the AVComponent stuff
	if(SYM_FAILED(m_AVEmailLoader.CreateObject(&m_spAVEmail)))
	{
		CCTRCTXE0(_T("Error loading IAVEmail object."));
		return E_FAIL;
	}

	if(FAILED(hr = m_spAVEmail->GetOption(AVModule::IAVEmail::eAVEmailOpt_ScanIncoming, m_dwScanIncoming)))
		CCTRCTXW1(_T("Error loading Email Option: Scan Incoming. HR = 0x%08x"), hr);
	if(FAILED(hr = m_spAVEmail->GetOption(AVModule::IAVEmail::eAVEmailOpt_ScanOutgoing, m_dwScanOutgoing)))
		CCTRCTXW1(_T("Error loading Email Option: Scan Outgoing. HR = 0x%08x"), hr);
	if(FAILED(hr = m_spAVEmail->GetOption(AVModule::IAVEmail::eAVEmailOpt_UseOEH, m_dwOEH)))
		CCTRCTXW1(_T("Error loading Email Option: Use OEH. HR = 0x%08x"), hr);

    m_bInitialized = true;
    return S_OK;
}

// Call this method to save any changed options
// If we're using a borrowed settings object, we merely return OK.
// Otherwise, we return OK if the commit saves successfully and we
// properly sent notification.
HRESULT CEmailOptions::Save()
{
	HRESULT hr;
    if(!m_bInitialized || !m_bDirty)
        return S_FALSE;
    else
    {
        // Commit email options
        if(m_pEmailOptions)
        {
            if(!m_pEmailOptions->SetValue(CCEMLPXY_TRAY_ANIMATION,m_bEmlTray) ||
               !m_pEmailOptions->SetValue(CCEMLPXY_OUTGOING_PROGRESS,m_bEmlProgress) ||
               !m_pEmailOptions->SetValue(CCEMLPXY_TIMEOUT_PROTECTION,m_bEmlTimeout)
              )
            {
                return E_FAIL;
            }
        }
        else
        {
            return E_FAIL;
        }
		if(m_spAVEmail)
		{
			TCHAR szOEHDllPath[512];
			size_t nOEHDllPathSize = 512;

			ccSym::CNAVPathProvider::GetPath(szOEHDllPath, nOEHDllPathSize);
			PathAppend(szOEHDllPath, SZ_OEHEUR_DLL);

			if(FAILED(hr = m_spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_ScanIncoming, m_dwScanIncoming)))
				CCTRCTXW1(_T("Error setting IAVEmail Option: Scan Incoming. Hr = 0x%08x"), hr);
			if(FAILED(hr = m_spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_ScanOutgoing, m_dwScanOutgoing)))
				CCTRCTXW1(_T("Error setting IAVEmail Option: Scan Outgoing. Hr = 0x%08x"), hr);
			if(FAILED(hr = m_spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_UseOEH, m_dwOEH)))
				CCTRCTXW1(_T("Error setting IAVEmail Option: Use OEH. Hr = 0x%08x"), hr);
			if(FAILED(hr = m_spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_OEHDLLPath, szOEHDllPath)))
				CCTRCTXW1(_T("Error setting IAVEmail OEH DLL Path. HR = 0x%08x"), hr);
			if(FAILED(hr = m_spAVEmail->SetOption(AVModule::IAVEmail::eAVEmailOpt_OEHSymObjectID, SZ_OEHEUR_GUID)))
				CCTRCTXW1(_T("Error setting IAVEmail OEH SymObjectID. HR = 0x%08x"), hr);

		}
		return S_OK;
    }
}

// Call this to manually notify
HRESULT CEmailOptions::Notify()
{
    if(!m_bInitialized)
        return S_FALSE;
    else
    {
        // Don't need to do anything! IEmailOptions3 interface handles details.
        return S_OK;
    }
}

// Defaults all contained options
HRESULT CEmailOptions::Default()
{
	DWORD dwVal;
	HRESULT hr;
	CNAVOptSettingsCache navOptDefs;

	if(FAILED(hr = Initialize()))
		return hr;

	m_bDirty = true;
	if(!navOptDefs.Init(_T("NAVOPTS.DEF"), FALSE))
	{
		CCTRCTXW0(_T("Error connecting to options defaults."));
		m_bEmlTimeout = m_bEmlProgress = m_bEmlTray = true;
	}
	else
	{
		if(FAILED(hr = navOptDefs.GetValue(NAVPROXY_ShowProgressOut, dwVal, 1)))
		{
			CCTRCTXW1(_T("Error getting Progress default. HR = 0x%08x"), hr);
			dwVal = 1;
		}
		m_bEmlProgress = dwVal ? true:false;

		if(FAILED(hr = navOptDefs.GetValue(NAVPROXY_ShowTrayIcon, dwVal, 1)))
		{
			CCTRCTXW1(_T("Error getting Tray Icon default. HR = 0x%08x"), hr);
			dwVal = 1;
		}
		m_bEmlTray = dwVal ? true:false;

		if(FAILED(hr = navOptDefs.GetValue(NAVPROXY_TimeOutProtection, dwVal, 1)))
		{
			CCTRCTXW1(_T("Error getting Progress default. HR = 0x%08x"), hr);
			dwVal = 1;
		}
		m_bEmlTimeout = dwVal ? true:false;

		if(FAILED(hr = navOptDefs.GetValue(AVEMAIL_ScanIncoming, m_dwScanIncoming, 1)))
		{
			CCTRCTXW1(_T("Error getting AVEMAIL:ScanIncoming. HR = 0x%08x"), hr);
			m_dwScanIncoming = 1;
		}

		if(FAILED(hr = navOptDefs.GetValue(AVEMAIL_ScanOutgoing, m_dwScanOutgoing, 1)))
		{
			CCTRCTXW1(_T("Error getting AVEMAIL:ScanIncoming. HR = 0x%08x"), hr);
			m_dwScanOutgoing = 1;
		}

		if(FAILED(hr = navOptDefs.GetValue(AVEMAIL_OEH, m_dwOEH, 1)))
		{
			CCTRCTXW1(_T("Error getting AVEMAIL:OEH. HR = 0x%08x"), hr);
			m_dwOEH = 1;
		}
	}

    return S_OK;
}

/** Gets and Sets **/

HRESULT CEmailOptions::GetShowProgressOut(DWORD& bShow)
{
    if(!SUCCEEDED(Initialize()))
        return E_FAIL;

    bShow = m_bEmlProgress ? 1:0;
	return S_OK;
}

HRESULT CEmailOptions::SetShowProgressOut(DWORD bShow)
{
    if(!SUCCEEDED(Initialize()))
        return E_FAIL;

    if((bShow?true:false) != m_bEmlProgress)
    {
        m_bDirty = true;
        m_bEmlProgress = bShow ? true:false;
    }

    return S_OK;
}

HRESULT CEmailOptions::GetShowTrayIcon(DWORD& bShow)
{
    if(!SUCCEEDED(Initialize()))
        return E_FAIL;

    bShow = m_bEmlTray ? 1:0;
	return S_OK;
}

HRESULT CEmailOptions::SetShowTrayIcon(DWORD bShow)
{
    if(!SUCCEEDED(Initialize()))
        return E_FAIL;

    if((bShow?true:false) != m_bEmlTray)
    {
        m_bDirty = true;
        m_bEmlTray = bShow ? true:false;
    }

    return S_OK;
}

HRESULT CEmailOptions::GetTimeoutProtection(DWORD& bProtect)
{
    if(!SUCCEEDED(Initialize()))
        return E_FAIL;

    bProtect = m_bEmlTimeout ? 1:0;
	return S_OK;
}

HRESULT CEmailOptions::SetTimeoutProtection(DWORD bProtect)
{
    if(!SUCCEEDED(Initialize()))
        return E_FAIL;

    if((bProtect?true:false) != m_bEmlTimeout)
    {
        m_bDirty = true;
        m_bEmlTimeout = bProtect ? true:false;
    }
    return S_OK;
}

HRESULT CEmailOptions::GetScanIncoming(DWORD& dwScan)
{
	if(!SUCCEEDED(Initialize()))
		return E_FAIL;

	dwScan = m_dwScanIncoming;
	return S_OK;
}

HRESULT CEmailOptions::GetScanOutgoing(DWORD& dwScan)
{
	if(!SUCCEEDED(Initialize()))
		return E_FAIL;

	dwScan = m_dwScanOutgoing;
	return S_OK;
}

HRESULT CEmailOptions::GetScanOEH(DWORD& dwScan)
{
	if(!SUCCEEDED(Initialize()))
		return E_FAIL;

	dwScan = m_dwOEH;
	return S_OK;
}

HRESULT CEmailOptions::SetScanIncoming(const DWORD dwScan)
{
	if(!SUCCEEDED(Initialize()))
		return E_FAIL;

	if(dwScan != m_dwScanIncoming)
	{
		m_bDirty = true;
		m_dwScanIncoming = dwScan;
	}
	return S_OK;
}

HRESULT CEmailOptions::SetScanOutgoing(const DWORD dwScan)
{
	if(!SUCCEEDED(Initialize()))
		return E_FAIL;

	if(dwScan != m_dwScanOutgoing)
	{
		m_bDirty = true;
		m_dwScanOutgoing = dwScan;
	}
	return S_OK;
}

HRESULT CEmailOptions::SetScanOEH(const DWORD dwScan)
{
	if(!SUCCEEDED(Initialize()))
		return E_FAIL;

	if(dwScan != m_dwOEH)
	{
		m_bDirty = true;
		m_dwOEH = dwScan;
	}
	return S_OK;
}

