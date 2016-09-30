// NAVOptionsObj.cpp : Implementation of CNAVOptions
#include "StdAfx.h"
#include "GlobalEvent.h"
#include "GlobalEvents.h"

#include "EmailProxyInterface.h"  // For Email options
#include "NAVOptionsObj.h"
#include "Navtrust.h"

#include "NAVOptMigrate.h"
#include "..\navoptionsres\resource.h"
#include "NAVErrorResource.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions::CBuffer: a cleanup utility class
CNAVOptions::CSemaphore::CSemaphore(void) : m_hSemaphore(NULL)
{
	// Compose the semaphore name according to the OS
	if(g_OSInfo.IsTerminalServicesInstalled())
		::_tcscpy(m_szSemaphore, _T("Global\\"));
	else
		*m_szSemaphore = _T('\0');

	::_tcscat(m_szSemaphore, _T("Symantec.Norton.AntiVirus.Options"));
}

CNAVOptions::CSemaphore::~CSemaphore(void)
{
	if (m_hSemaphore)
		::CloseHandle(m_hSemaphore);
}

HRESULT CNAVOptions::CSemaphore::Create(void)
{
	// Create a new semaphore to be used by the options dialog.
	if (NULL == (m_hSemaphore = ::CreateSemaphore(NULL, 0, 1, m_szSemaphore)))
		// Q: Access is denied by another session?
		return ERROR_ACCESS_DENIED == ::GetLastError()
		    && g_OSInfo.IsTerminalServicesInstalled() ? S_EXIST
		                                              : CNAVOptions::Error(IDS_Err_Semaphore, _T("CSemaphore::Create()"));

	return ERROR_ALREADY_EXISTS == ::GetLastError() ? S_FALSE : S_OK;
}

bool CNAVOptions::CSemaphore::Open(void)
{
	return NULL != (m_hSemaphore = ::OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, m_szSemaphore));
}

DWORD CNAVOptions::CSemaphore::Accuire(void)
{
	// Wait 5 minutes to be safe before giving up on the dialog
	return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hSemaphore, 30000);
}

void CNAVOptions::CSemaphore::Release(void)
{
	(void)::ReleaseSemaphore(m_hSemaphore, 1, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions::CBuffer: a cleanup utility class
// Convert the string Unicode to ANSI.
HRESULT CNAVOptions::CBuffer::Convert(const VARIANT& vValue)
{
	int ccStr = ::SysStringLen(vValue.bstrVal)
	  , cchBuf;

	if (!ccStr)  // nil
	{
		_ASSERT(!m_pszBuffer);
		if (NULL == (m_pszBuffer = new char[1]))
			return CNAVOptions::Error(_T("CBuffer::Convert()"), E_OUTOFMEMORY);
		m_pszBuffer[0] = '\0';
		return S_OK;
	}

	// First lets find how much buffer space we need.
	if (0 == (cchBuf = ::WideCharToMultiByte(CP_ACP, 0              // performance and mapping flags
	                                               , vValue.bstrVal // wide-character string
	                                               , ccStr          // number of chars in string
	                                               , NULL, 0, NULL, NULL)))
	{
		DWORD dwError = ::GetLastError();
		return CNAVOptions::Error(IDS_Err_Conversion, _T("CBuffer::Convert()"));
	}

	// Get a properly sized buffer
	_ASSERT(!m_pszBuffer);
	if (NULL == (m_pszBuffer = new char[cchBuf + 1]))
		return CNAVOptions::Error(_T("CBuffer::Convert()"), E_OUTOFMEMORY);

	// Do the actual conversion
	if (cchBuf != ::WideCharToMultiByte(CP_ACP, 0              // performance and mapping flags
	                                          , vValue.bstrVal // wide-character string
	                                          , ccStr          // number of chars in string
	                                          , m_pszBuffer    // buffer for new string
	                                          , cchBuf         // size of buffer
	                                          , NULL, NULL))
		return CNAVOptions::Error(IDS_Err_Conversion, _T("CBuffer::Convert()"));
	m_pszBuffer[cchBuf] = '\0';

	return S_OK;
}

// Convert the ANSI string to Unicode.
HRESULT CNAVOptions::CBuffer::Convert(VARIANT *pvValue)
{
	HRESULT hr;

	// Convert to BSTR
	if (FAILED(hr = Convert(pvValue->bstrVal)))
		return hr;

	// Send the result back
	(*pvValue).vt = VT_BSTR;

	return S_OK;
}

// Assign the TCHAR string to the buffer
char* CNAVOptions::CBuffer::operator =(LPCSTR pcszOther)
{
    // Make sure the existing buffer is big enough, the length
    // of this buffer should be the exact size of the buffer since
    // this is how it is allocated.
    if( !m_pszBuffer || strlen(m_pszBuffer) < strlen(pcszOther) )
    {
        if( m_pszBuffer )
            delete [] m_pszBuffer;
        m_pszBuffer = new char[strlen(pcszOther)+1];
    }

    if( !m_pszBuffer )
        return NULL;

    // Copy the data to the buffer
    return strcpy(m_pszBuffer, pcszOther); 
}

// Convert the ANSI string to Unicode.
HRESULT CNAVOptions::CBuffer::Convert(BSTR& bstrValue)
{
	int ccWC = 0;

	// First figure the size of the BSTR needed
	if (::strlen(m_pszBuffer)
	 && 0 >= (ccWC = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_pszBuffer, -1, NULL, 0) - 1))
		return CNAVOptions::Error(IDS_Err_Conversion, _T("CBuffer::Convert()"));

	CComBSTR sbstrValue(ccWC);

	// Now do the actual conversion
	if (ccWC != ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED
	                                        , m_pszBuffer
	                                        , ::strlen(m_pszBuffer)
	                                        , sbstrValue, ccWC))
		return CNAVOptions::Error(IDS_Err_Conversion, _T("CBuffer::Convert()"));

	// Send the result back
	bstrValue = sbstrValue.Detach();

	return S_OK;
}

bool CNAVOptions::CBuffer::operator==(const CBuffer& rhs) const
{
	return rhs.m_pszBuffer && m_pszBuffer && 0 == ::strcmp(rhs.m_pszBuffer, m_pszBuffer);
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions
HRESULT CNAVOptions::exception(UINT iID, EXCEPINFO* pexcepinfo)
{
	CComBSTR sbSource, sbDesciption;
	_Module.SetResourceInstance(g_ResModule);
	sbSource.LoadString(IDS_NAVOptions);
	sbDesciption.LoadString(iID);

	// Generate a standard IDispatch exception
	::memset(pexcepinfo, 0, sizeof(EXCEPINFO));
	pexcepinfo->scode           = E_FAIL;
	pexcepinfo->bstrSource      = sbSource.Detach();
	pexcepinfo->bstrDescription = sbDesciption.Detach();
	_Module.SetResourceInstance(_Module.GetModuleInstance());
	return DISP_E_EXCEPTION;
}

HRESULT CNAVOptions::get__TypeOf(LPCWCH bstrOption, long *plType)
{
	if (forceError (ERR_INVALID_POINTER) ||
        !bstrOption || !plType)
    {
	    MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

	// Check if the DISPID exists
	OPTYPEMAP::iterator it = m_OptionsTypes.find(bstrOption);
	if (forceError (IDS_ERR_INVALID_DISPID) || m_OptionsTypes.end() == it)
    {
        MakeError(IDS_ERR_INVALID_DISPID,DISP_E_MEMBERNOTFOUND, IDS_NAVERROR_INTERNAL);
		return DISP_E_MEMBERNOTFOUND;
    }

	*plType = it->second;

	return S_OK;
}

HRESULT CNAVOptions::Default(const char* pcszDefault, DWORD& dwDefault)
{
	if (forceError (IDS_Err_Default) ||
        FAILED(m_pNavOpts->GetValue(pcszDefault, dwDefault, 0)) )
	{
		MakeError(IDS_Err_Default, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

	return S_OK;
}

void CNAVOptions::notify(void)
{
	CCTRACEI("NAVOptions changed notification");

	// Tell everyone that the options changes so they have a chance to reload the file
	CGlobalEvent ge;
#ifndef _UNICODE
	if (ge.Create(SYM_OPTIONS_CHANGED_EVENT))
#else  _UNICODE
	if (ge.CreateW(SYM_OPTIONS_CHANGED_EVENT))
#endif  _UNICODE
		::PulseEvent(ge);


    // Signal that the IMScanner state has changed.
    //
    if ( m_bIMScanDirty )
    {
        ccLib::CEvent eventIMScan;
        if ( eventIMScan.Create ( NULL, TRUE, FALSE, SYM_REFRESH_IMSCANNER_STATUS_EVENT, TRUE ))
            eventIMScan.PulseEvent();

		try
        {
			SendSettingsEvent(IMScanSettingsKeyName);
		}        
		catch(...)
        {
            CCTRACEE ("CNAVOptions::notify - caught exception sending IMScan event");
        }
	}

    if ( m_bEmailDirty )
    {
        try
        {
            // Email proxy still listens for ccSettingsManager events, but we no longer send them.
            // Give them an event when email settings change
			SendSettingsEvent(NavEmailSettingsKeyName);
        }
        catch(...)
        {
            CCTRACEE ("CNAVOptions::notify - caught exception sending email event");
        }

        ccLib::CEvent eventEmailScan;
        if ( eventEmailScan.Create ( NULL, TRUE, FALSE, SYM_REFRESH_NAVPROXY_STATUS_EVENT, TRUE ))
            eventEmailScan.PulseEvent();
    }

    m_bEmailDirty = false;
    m_bIMScanDirty = false;
	m_bIsDirty = false;  // No longer dirty
    m_bAdvEmlOptsDirty = false;
    m_bIsPasswordDirty = false;
}

void CNAVOptions::SendSettingsEvent(PCTSTR pszKey)
{
	ccSettings::CSettingsManagerHelper ccSettingsHelper;
	ccSettings::ISettingsManagerPtr pSettingsManager;
	if ( SYM_SUCCEEDED (ccSettingsHelper.Create(pSettingsManager)))
	{
		ccSettings::ISettingsPtr pSettings;
		pSettingsManager->GetSettings (pszKey, &pSettings);
		if( pSettings )
		{
			pSettingsManager->PutSettings(pSettings); // Send Events.
		}
		else
		{
			CCTRACEE ("CNAVOptions::SendSettingsEvent - Error sending event. Key not found %s",pszKey);
		}
	}
}

PTCHAR CNAVOptions::url(void)
{
	static TCHAR szURL[MAX_PATH + 6 + 16] = _T("res://");

	if (!szURL[6])  // Initialize the first time
	{
		// Compose the URL
		::GetModuleFileName(g_ResModule, szURL + 6, MAX_PATH);
		::_tcscat(szURL, _T("/options.htm"));
	}

	return szURL;
}

HRESULT CNAVOptions::restore(PCTSTR pszFile)
{
    if (forceError (IDS_Err_Defaults))
    {
        MakeError(IDS_Err_Defaults, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Restore settings hives from the options primitive file name
    // passed in...
	TCHAR szDefaults[MAX_PATH];

	// Path for the file to restore
	::_tcscat(::_tcscpy(szDefaults, g_NAVInfo.GetNAVDir()), pszFile);

	// Q: Does this file exists?
	if (0xFFFFFFFF == ::GetFileAttributes(szDefaults))
		return E_FAIL;

	// Overwrite current options from the default file
    CNAVOptMigrate migrate;
    if( !migrate.MigrateNAVOptFile(szDefaults, true) )
    {
        MakeError(IDS_Err_Defaults, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

	return S_OK;
}

HRESULT CNAVOptions::initializeSAVRTDLL(void)
{
    // This will load the SavRT32.dll and get all the function pointers
    if( !m_hSavrRTDll )
    {
        // First attempt to load Savrt32.dll from the NAV directory
        TCHAR szSavrtPath[MAX_PATH] = {0};
        _tcscpy(szSavrtPath, g_NAVInfo.GetNAVDir());
        _tcscat(szSavrtPath, _T("\\SavRT32.dll"));

        // Verify the signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szSavrtPath) )
        {
            MakeError(IDS_ERR_LOAD_SAVRT, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_ACCESSDENIED;
        }

        m_hSavrRTDll = LoadLibraryEx(szSavrtPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

        if( forceError (IDS_ERR_LOAD_SAVRT) ||
            NULL == m_hSavrRTDll )
        {
            MakeError(IDS_ERR_LOAD_SAVRT, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }

        // Now get the function pointers
       
        APExclusionOpen = reinterpret_cast<pfn_SAVRT_EXCLUSION_OPEN>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_OPEN)));
        APExclusionClose = reinterpret_cast<pfn_SAVRT_EXCLUSION_CLOSE>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_CLOSE)));
        APExclusionGetNext = reinterpret_cast<pfn_SAVRT_EXCLUSION_GETNEXT>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_GETNEXT)));
        APExclusionModifyAdd = reinterpret_cast<pfn_SAVRT_EXCLUSION_MODIFY_ADD>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_MODIFY_ADD)));
        APExclusionModifyRemove = reinterpret_cast<pfn_SAVRT_EXCLUSION_MODIFY_REMOVE>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_MODIFY_REMOVE)));
        APExclusionModifyEdit = reinterpret_cast<pfn_SAVRT_EXCLUSION_MODIFY_EDIT>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_MODIFY_EDIT)));
        APExclusionSetDefault = reinterpret_cast<pfn_SAVRT_EXCLUSION_SET_DEFAULT>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_SET_DEFAULT)));
        APExclusionIsDirty = reinterpret_cast<pfn_SAVRT_EXCLUSION_IS_DIRTY>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_IS_DIRTY)));
        APExclusionGoToListHead = reinterpret_cast<pfn_SAVRT_EXCLUSION_GO_TO_LIST_HEAD>(::GetProcAddress(m_hSavrRTDll, reinterpret_cast<LPCTSTR>(SAVRT_EXCLUSION_GO_TO_LIST_HEAD)));


        if( forceError (IDS_ERR_SAVRT_FUNCTION) ||
            !APExclusionOpen || !APExclusionClose || !APExclusionGetNext || !APExclusionGoToListHead
            || !APExclusionModifyAdd || !APExclusionModifyRemove || !APExclusionModifyEdit || !APExclusionSetDefault || !APExclusionIsDirty )
        {
            MakeError(IDS_ERR_SAVRT_FUNCTION, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CNAVOptions::initializeCCEmailPxy(void)
{
    if( !m_hccEmlPxy )
    {
        // Initialize
        HRESULT hr = S_OK;
        SYMOBJECT_ID ObjectID;
        m_pEmailOptions = NULL;
		ISymFactory* pFactory = NULL;
        pfnGETFACTORY pGetFactory = NULL;

        ccLib::CString szEmlPxyPath;

        ccSym::CInstalledApps::GetCCDirectory( szEmlPxyPath );
        szEmlPxyPath.Append(_T("\\ccEmlPxy.dll"));

        // Validate the signature
        if( forceError ( IDS_ERR_INVALIDSIG_CCEMLPXY ) ||
			NAVToolbox::IsSymantecSignedImage(szEmlPxyPath) )
        {
            MakeError(IDS_ERR_INVALIDSIG_CCEMLPXY, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
		    return E_FAIL;
        }

        m_hccEmlPxy = LoadLibraryEx(szEmlPxyPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	    if (forceError ( IDS_Err_LoadccEmlPxy ) || m_hccEmlPxy == NULL)
        {
            MakeError(IDS_Err_LoadccEmlPxy, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
		    return E_FAIL;
        }

	    // Get the options object ID.

	    pfnGETEMAILOPTIONSOBJECTID pGetEmailOptionsObjectID =
		    (pfnGETEMAILOPTIONSOBJECTID) GetProcAddress(m_hccEmlPxy, "GetEmailOptionsObjectID");

	    if (pGetEmailOptionsObjectID != NULL)
		{
            if (pGetEmailOptionsObjectID(&ObjectID) == SYM_OK)
            {
		        // Create the options object using its factory.

	            pGetFactory = (pfnGETFACTORY) GetProcAddress(m_hccEmlPxy, "GetFactory");

	            if (pGetFactory != NULL)
		        {
	                if (pGetFactory(ObjectID, &pFactory) == SYM_OK)
		            {
		                if (pFactory->CreateInstance(IID_EmailOptions, (void**) &m_pEmailOptions) == SYM_OK)
		                {
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
        }
        else
            hr = E_FAIL;

        if( forceError ( IDS_Err_ccEmlPxy ) || FAILED (hr) || !m_pEmailOptions )
        {
            if ( !forceError ( IDS_Err_ccEmlPxy ))
            {
                if(m_hccEmlPxy)
                {
                    ::FreeLibrary(m_hccEmlPxy);
                    m_hccEmlPxy = NULL;
                }
                if( pFactory )
                {
                    pFactory->Release();
                    pFactory = NULL;
                }
                if( m_pEmailOptions )
                {
                    m_pEmailOptions->Release();
                    m_pEmailOptions = NULL;
                }
            }

            MakeError(IDS_Err_ccEmlPxy, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }

        // All is good
        pFactory->Release();

		// Store the current values for each of the email options
		m_pEmailOptions->GetValue(CCEMLPXY_OUTGOING_PROGRESS, m_bEmlProgress);
		m_pEmailOptions->GetValue(CCEMLPXY_TRAY_ANIMATION, m_bEmlTray);
		m_pEmailOptions->GetValue(CCEMLPXY_TIMEOUT_PROTECTION, m_bEmlTimeout);
    }

    return S_OK;
}

void CNAVOptions::warning(UINT uiIDS)
{
	CString csFormat;
	CString csMsg;

	csFormat.LoadString(uiIDS);
	csMsg = csFormat;

	switch(uiIDS)
	{
	case IDS_Wrn_Default:
		csMsg.Format(csFormat, m_csProductName);
		break;

	default:
		break;
	}

	// Warn the user that the defaults were restored.
	::MessageBox(::GetActiveWindow(), csMsg, m_csTitle, MB_OK | MB_ICONEXCLAMATION);
}

HRESULT CNAVOptions::Error(UINT uiIDS, PTCHAR pszLocation, bool bReturnFalse/*= true*/)
{
	HRESULT hr;

    CCTRACEE(_T("CNAVOptions::Error() - Received error ID = %d. Location =%s"), uiIDS, pszLocation);

	if (ShowScriptErrors())
		return CComCoClass<CNAVOptions, &CLSID_NAVOptions>::Error(uiIDS, IID_INAVOptions);
	else
		return FAILED(hr = ::Error(_T("CNAVOptions"), uiIDS, pszLocation)) || bReturnFalse ? hr : S_OK;
}

HRESULT CNAVOptions::Error(PTCHAR pszLocation, HRESULT hr, bool bReturnFalse/*= true*/)
{
    CCTRACEE(_T("CNAVOptions::Error() - Received error HRESULT = 0x%X. Location =%s"), hr, pszLocation);
	return FAILED(hr = ::Error(_T("CNAVOptions"), pszLocation, hr)) || bReturnFalse ? hr : S_OK;
}
