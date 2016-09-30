// NAVOptionsObj.cpp : Implementation of CNAVOptions
#include "StdAfx.h"

#include "StahlSoft.h"

#ifndef _INIT_COSVERSIONINFO
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#endif

#define SIMON_INITGUID
#include "Simon.h"
#include "NavOptionRefreshHelperInterface.h"

#include <algorithm>
#include "NAVOptions.h"
#include "ccWebWnd.h"
#include "NAVOptionsObj.h"
#include "NAVOptionGroupObj.h"
#include "IMScan.h"             // Instant Messenger Functions
#include "NProtectOptions.h"    // For NAV Pro NProtect Options panels
#include "ccModuleID.h"
#include "AVccModuleID.h"
#include "NAVTrust.h"
#include "globalevent.h"
#include "globalevents.h"
#include "AVRESBranding.h"
#include "NAVErrorResource.h"

#include "build.h" // For version number

#include "defutilsinterface.h"  // Defutils syminterface object
#include "NAVDefutilsLoader.h"

// Ensure that we define and initialize the strings defined in NavAppIDs.h
#define NAVAPPIDS_BUILDING
#include "NavAppIds.h"

#include "IWPPrivateSettingsInterface.h"   // For IWP FirewallOn setting

#include "fwui_i.c"
#include "FWPropertyBag.h"
#include "NAVVer.h"
#include "SymHelp.h"
#include "..\navoptionsres\resource.h"

using namespace std;

// The following "functor" is used in order to insert properties into a property group
struct AddOptions
{
	LPCWCH pn;
	CNAVOptionGroup* po;
	AddOptions(LPCWCH pwcName, CNAVOptionGroup* pNAVOptionGroup) : pn(pwcName), po(pNAVOptionGroup) {}
	void operator()(OPTYPEMAP::value_type opt)
	{
		int i = opt.first.find(L":");
		// Add a property by the name xxx to group YYY if the name is "YYY:xxx"
		_ASSERT(-1 < i);
		if (opt.first.substr(0, i) == pn)
			po->AddProperty(opt.first.substr(i+1).c_str());
	}
};

//
// Performs a sort on the space delimited extension list
void SortExtList(LPTSTR pszExtList, LPTSTR pszSortedList, DWORD dwSortedListBytes)
{
    // Validate the input
    if( !pszExtList || !(*pszExtList) || !pszSortedList )
        return;

    // Create our own buffer to parse the extension list
    TCHAR* pszList = new TCHAR[_tcslen(pszExtList)+1];

    if( !pszList )
        return;

    _tcscpy(pszList, pszExtList);

    // Clear the output buffer
    ZeroMemory(pszSortedList, dwSortedListBytes);

    // Perform a sort using a vector of strings
    vector<string> vExtList;

    // Add all items in the list to the vector
    TCHAR* pszCurExt = NULL;
    pszCurExt = _tcstok(pszList, _T(" "));
    while( NULL != pszCurExt )
    {
        vExtList.push_back(pszCurExt);
        pszCurExt = _tcstok(NULL, _T(" "));
    }

    // Sort the vector
    sort(vExtList.begin(), vExtList.end());

    // Put it into the sorted list
    vector<string>::iterator Iter;
    bool bFirst = true;
    for( Iter = vExtList.begin(); Iter != vExtList.end(); Iter++ )
    {
        // Don't put a space in front of the first item
        if( bFirst )
            bFirst = false;
        else
            _tcsncat(pszSortedList, _T(" "), dwSortedListBytes);

        _tcsncat(pszSortedList, Iter->c_str(), dwSortedListBytes);
    }

    if( pszList )
        delete [] pszList;
}
/////////////////////////////////////////////////////////////////////////////
// CNAVOptions
CNAVOptions::CNAVOptions(void)
           : m_bIsDirty(false)
           , m_bCanAccessRegKey(false)
           , m_LiveUpdate(this)
           , m_LiveUpdateMode(this)
           , m_OfficePlugin(this)
           , m_StartupScan(this)
           , m_hIMScanDll(NULL)
           , m_hAPExclusions(NULL)
           , m_pEmailOptions(NULL)
           , m_hccEmlPxy(NULL)
           , m_hSavrRTDll(NULL)
           , APExclusionOpen(NULL)
           , APExclusionClose(NULL)
           , APExclusionGetNext(NULL)
           , APExclusionModifyAdd(NULL)
           , APExclusionModifyRemove(NULL)
           , APExclusionModifyEdit(NULL)
           , APExclusionSetDefault(NULL)
           , APExclusionIsDirty(NULL)
           , APExclusionGoToListHead(NULL)
           , m_bIsPasswordDirty(false)
		   , m_bAdvEmlOptsDirty(false)
		   , m_bEmlProgress(false)
		   , m_bEmlTray(false)
		   , m_bEmlTimeout(false)
		   , m_bALURunning(FALSE)
		   , m_hALUMutex(NULL)
           , m_bExclusionsLoaded(false)
           , m_bExclusionsDirty(false)
           , m_bExclusionsAreViral(false)
           , m_bNavDefsInitialized(false)
           , m_MyHwnd(NULL)
           , m_dwMaxName(0)
           , m_dwMaxData(0)
           , m_pIWPSettings(NULL)
           , m_bIWPDirty(false)
           , m_bSymProtectEnable(false)
           , m_bSymProtectDirty(false)
           , m_pSymProtect(NULL)
           , pFWUIPropBag(NULL)
           , m_pSNMgr(NULL)
           , m_eInitialPageID(ShowPageID_AUTOPROTECT)
           , m_bIMScanDirty(false)
           , m_bEmailDirty(false)
{
    m_pNavOpts = new CNAVOptSettingsCache(m_ccSettings);    // Regular options
    m_pNavDefs = new CNAVOptSettingsCache(m_ccSettings);    // Default options

	// Initialize other properties array
	// Note: This must be updateded as properties
	//       are added or removed!
	m_Properties[0] = &m_LiveUpdate;
	m_Properties[1] = &m_LiveUpdateMode;
	m_Properties[2] = &m_OfficePlugin;
	m_Properties[3] = &m_StartupScan;

	CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

    // Check NAVError module for Symantec Signature...
    //
    if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
                                              NULL,
                                              CLSCTX_INPROC_SERVER)))
    {		
        m_spError = NULL;
    }

	CBrandingRes BrandRes;
	m_csProductName = BrandRes.ProductName();

	CString csFormat;
	csFormat.LoadString(g_ResModule, IDS_Title);
	m_csTitle.Format(csFormat, m_csProductName);

    // Create a global mutex to synchronize loading, saving, and setting defaults
    // the options object
    TCHAR szMutexName[MAX_PATH] = _T("CNAVOptionsObj_{72ED3226-4C58-4290-93D4-7F15AE02203F}");
    m_shNavOptsObjMutex = StahlSoft::CreateMutexEx(NULL,FALSE,szMutexName,TRUE);
    m_NavOptsObjLockMutex.Attach(m_shNavOptsObjMutex);

    _tcscpy ( m_szCurrentWorkingDir, "");
}

CNAVOptions::~CNAVOptions(void)
{
	// Free resources
    if (m_hIMScanDll)
        ::FreeLibrary(m_hIMScanDll);
    if (m_hAPExclusions)
    {
        if( APExclusionClose )
        {
            // Have the AP Exclusions handle still...Close without committing
            APExclusionClose( &m_hAPExclusions, FALSE );
            m_hAPExclusions = NULL;
        }
    }
    if (m_pEmailOptions)
        m_pEmailOptions->Release();
    if (m_hccEmlPxy)
        ::FreeLibrary(m_hccEmlPxy);
    if (m_hSavrRTDll)
        ::FreeLibrary(m_hSavrRTDll);
	if( m_hALUMutex )
		CloseHandle(m_hALUMutex);

    // Release the IWP Settings object before the loader class which has the
    // dll loaded is freed
    if( m_pIWPSettings )
        m_pIWPSettings.Release();

    if( pFWUIPropBag )
    {
        pFWUIPropBag->Release();
        pFWUIPropBag = NULL;
    }

    // Delete the symprotect guy
    if( m_pSymProtect )
    {
        delete m_pSymProtect;
        m_pSymProtect = NULL;
    }

	if (m_pNavOpts != NULL)
	    delete m_pNavOpts;

	if (m_pNavDefs != NULL)
	    delete m_pNavDefs;

    if(m_spExclusionMgr)
    {
        CCTRACEI(_T("CNAVOptions::dtor - Releasing exclusion manager."));
        m_spExclusionMgr->uninitialize(false);
        m_spExclusionMgr.Release();

    }
    CCTRACEI(_T("CNAVOptions::dtor - Releasing exclusion vectors."));
    m_vFSExclusions.clear();
    m_vAnomalyExclusions.clear();
}

HRESULT CNAVOptions::FinalConstruct(void)
{
	CRegKey rkeyNAV;
	DWORD dwBogusValue = 0UL;

	// We also have to check for write access to a reg key because
	// we set the SmartScan and automatic LiveUpdate settings there.
	// Note : Under Win 2000 we found that you can request write access
	//        on a reg key hive but still not be able to change values of
	//        keys located inside it. Hence, the double check below.
	m_bCanAccessRegKey = ERROR_SUCCESS == rkeyNAV.Create(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"),  _T("KEY_WRITE"))
	                  && ERROR_SUCCESS == rkeyNAV.SetValue(_T("OptionsTest"), REG_DWORD, &dwBogusValue, sizeof(dwBogusValue));

	if (m_bCanAccessRegKey)
		rkeyNAV.DeleteValue(_T("OptionsTest"));

	{  // Opening for the semaphore block.
	CSemaphore sm;

	// Q: Does the semaphore exists?
	switch(sm.Open())
	{
		HWND hAppWnd;

	case false:  // This is an instance of the options that is used
	             // by someone other then an options dialog since
	             // only place that this semaphore is created is in Show().
		break;

	case true:   // Options dialog exists or coming into existance.
		// Q: Is an instance of Options dialog running?
		// If the dialog window is not there yet it may indicate that this is
		// an instance of options that is being used by some other program
		// i.e. Config Wizard, Main UI or the boot strap (NAVStub.exe) instance
		//      of options used for calling Show().
		if (hAppWnd = ::FindWindow(ccWebWindow_ClassName, m_csTitle))
			sm.Release();  // Only when we are sure that dialog exists
			               // we can allow the semaphore to be released.
		break;
	}  // end of switch(...)
	}  // Ending of the semaphore block.

	return Load();
}

void CNAVOptions::FinalRelease(void)
{
	// Release all options groups
	for (OPTGRPMAP::iterator it = m_INAVOptionGroups.begin(); m_INAVOptionGroups.end() != it; it++)
	{
		it->second.Release();
	}
	m_INAVOptionGroups.clear();

	CComObjectRootEx<CComSingleThreadModel>::FinalRelease();
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions::CNAVDispatchImpl
HRESULT CNAVOptions::Put(LPCWCH pwcName, DISPPARAMS* pdispparams, UINT* puArgErr)
{
	return DISP_E_MEMBERNOTFOUND; // Read-only
}

HRESULT CNAVOptions::Get(LPCWCH pwcName, VARIANT* pvValue)
{
	HRESULT hr;
	CComBSTR sbName = pwcName;
	CComPtr<INAVOptionGroup>& spINAVOptionGroup = m_INAVOptionGroups[pwcName];

	// Q: Is the automation client is using a reference to an existing object?
	if (!spINAVOptionGroup)
	{
		CComObject<CNAVOptionGroup>* pNAVOptionGroup = NULL;

		// Create a properties group object to bundle the properties
        // First check for a digital signature on the COM object
		if( forceError ( ERR_SECURITY_FAILED ) ||
			NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVOptionGroup) )
        {
            // Invalid signature
			MakeError (ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_ACCESSDENIED;
        }
		if (FAILED(hr = CComObject<CNAVOptionGroup>::CreateInstance(&pNAVOptionGroup)))
			return hr;

		pNAVOptionGroup->AddRef();

		// Give it it's name & a back reference
		if (SUCCEEDED(hr = pNAVOptionGroup->put__GroupName(sbName))
		 && SUCCEEDED(hr = pNAVOptionGroup->put__INAVOptions(this)))
		// Cache it in the map
			hr = pNAVOptionGroup->QueryInterface(&spINAVOptionGroup);

		pNAVOptionGroup->Release();

		if (FAILED(hr))
			return hr;

		// Store group names in the map for GetIDsOfNames()
		for_each(m_OptionsTypes.begin(), m_OptionsTypes.end(), AddOptions(pwcName, pNAVOptionGroup));
	}

	// Pagckage it into a VARIANT for sending it back to the calling client
	pvValue->pdispVal = NULL;
	if (SUCCEEDED(hr = spINAVOptionGroup.QueryInterface(&pvValue->pdispVal)))
		pvValue->vt = VT_DISPATCH;

	return hr;
}

HRESULT CNAVOptions::StdDefault(ITypeInfo* pTInfo, DISPID dispidMember)
{
	HRESULT hr;
	unsigned int i;
	CComBSTR sbName;

	// Find if this is a standard property
	if (FAILED(hr = pTInfo->GetNames(dispidMember, &sbName, 1, &i)))
		return hr;

	// Find which property to set to the default value
	for (i = 0; i < SIZEOF(m_Properties); i++)
	{
		if (!::wcscmp(m_Properties[i]->Name(), sbName))
			return m_Properties[i]->Default();
	}

	return DISP_E_MEMBERNOTFOUND;
}

HRESULT CNAVOptions::Default(LPCWCH pwcName, EXCEPINFO* pexcepinfo)
{
	USES_CONVERSION;
	HRESULT hr;
	long eType;
	DWORD dwValue;
	DWORD dwValueCurrent;

    // For AP options ask for the default value from savrt
    // Get the group name to see if it's an AP option group
    wstring wstrGroupName = pwcName;
    wstrGroupName = wstrGroupName.substr(0, wstrGroupName.find_first_of(L":"));
    if( IsAPOption(wstrGroupName.c_str()) )
    {
        if( forceError ( IDS_Err_SetAPDefault ) ||
            SAVRT_OPTS_OK != m_SavrtOptions.SetDefaultValue( W2A(pwcName) ) )
        {
            MakeError(IDS_Err_SetAPDefault, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
        return S_OK;
    }

	// Initialize the defaults the first time
	if (!m_bNavDefsInitialized)
	{
		TCHAR szNAVDefs[MAX_PATH];

		// Path to the default options file
		::_tcscat(::_tcscpy(szNAVDefs, g_NAVInfo.GetNAVDir()), _T("\\NAVOPTS.DEF"));
		// Load the list w/values from the file
        if( !m_pNavDefs->Init(szNAVDefs, FALSE) )
		{
		    MakeError(IDS_Err_DefaultsFile, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }

        m_bNavDefsInitialized = true;
	}

    DWORD dwMaxDataSize = 0;
    DWORD dwMaxNameSize = 0;
    m_pNavDefs->GetMaxNavSettingsSizes(dwMaxNameSize, dwMaxDataSize);
    CBuffer szValue(dwMaxDataSize);
    CBuffer szValueCurrent(m_dwMaxData);

	if (FAILED(hr = get__TypeOf(pwcName, &eType)))
		return hr;

	//option name for checking if its an e-mail option
	CComBSTR bstrOptName;

	// Normalize the default
	switch(eType)
	{
	case ccSettings::SYM_SETTING_STRING:
		// Get the stored value
		if (!forceError (IDS_Err_Default) &&
            SUCCEEDED(m_pNavDefs->GetValue(W2A(pwcName), szValue, dwMaxDataSize, ""))
           )
		{
			//Get current setting and check to see if it is the same.
			if (FAILED(m_pNavOpts->GetValue(W2A(pwcName), szValueCurrent, m_dwMaxData, ""))
				|| lstrcmpi(szValue, szValueCurrent) != 0)
			{
				//Couldn't get setting or setting is different, save it
				if (forceError (IDS_Err_Put) ||
                    FAILED(m_pNavOpts->SetValue(W2A(pwcName), szValue)) )
				{
					//Error setting option, return error.
					MakeError( IDS_Err_Put, E_FAIL, IDS_NAVERROR_INTERNAL );
					return E_FAIL;
				}
				else
				{
					//Setting successfully changed, set the dirty flag and
                    //update the max sizes
					m_bIsDirty = true;
                    m_pNavOpts->GetMaxNavSettingsSizes(m_dwMaxName, m_dwMaxData);
				}
			}
		}
		else
		{
			//Error getting value from default file, return error.
			MakeError( IDS_Err_Default, E_FAIL, IDS_NAVERROR_INTERNAL );
			return E_FAIL;
		}

		return S_OK;

	case ccSettings::SYM_SETTING_DWORD:
        // Handle Email options differently
        if( IsEmailOption( _bstr_t(pwcName), &bstrOptName ) )
        {
            m_bEmailDirty = true;

            // Compare to the default value of true for the advanced email
			// option to the current value of the email option and change
			// it if necessary
			if( 0 == _tcscmp(CCEMLPXY_TRAY_ANIMATION, OLE2T(bstrOptName)) )
            {
				if( true != m_bEmlTray )
                {
                    m_bEmlTray = m_bAdvEmlOptsDirty = true;
                }
            }
			else if( 0 == _tcscmp(CCEMLPXY_OUTGOING_PROGRESS, OLE2T(bstrOptName)) )
            {
				if( true != m_bEmlProgress )
                {
                    m_bEmlProgress = m_bAdvEmlOptsDirty = true;
                }
            }
			else if( 0 == _tcscmp(CCEMLPXY_TIMEOUT_PROTECTION, OLE2T(bstrOptName)) )
            {
                if( true != m_bEmlTimeout )
                {
                    m_bEmlTimeout = m_bAdvEmlOptsDirty = true;
                }
            }
        }
        else // Not an email option
		{
              // check for navemail options
              if (!wcscmp(wstrGroupName.c_str(),L"NAVEMAIL"))
              {
                m_bEmailDirty = true;
              }

              // check for im options
              if (!wcsncmp(wstrGroupName.c_str(),L"IMSCAN",wcslen(L"IMSCAN")))
              {
                 m_bIMScanDirty = true;
              }

              // Get the stored value
              if (!forceError (IDS_Err_Default) &&
                  SUCCEEDED(m_pNavDefs->GetValue(W2A(pwcName),dwValue,0)))
              {
                 //Get current setting and check to see if it is the same.
                 if (FAILED(m_pNavOpts->GetValue(W2A(pwcName),dwValueCurrent,0))
                     || dwValue != dwValueCurrent)
                 {
                    //Couldn't get setting or setting is different, save it
                    if (forceError (IDS_Err_Put) ||
                        FAILED(m_pNavOpts->SetValue(W2A(pwcName), dwValue)))
                    {
			              //Error setting option, return error.
			              MakeError( IDS_Err_Put, E_FAIL, IDS_NAVERROR_INTERNAL );
                          return E_FAIL;
                    }
                    else
                    {
                       //Setting successfully changed, set the dirty flag.
                       m_bIsDirty = true;
                    }
                 }
              }
              else
              {
                 //Error getting value from default file, return error.
                 MakeError( IDS_Err_Default, E_FAIL, IDS_NAVERROR_INTERNAL );
                 return E_FAIL;
              }
        }
     	return S_OK;

	default:  // Unknown option data type
		return DISP_E_TYPEMISMATCH;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions::INAVOptions
/////////////////////////////////////////////////////////////////////////////
// Methods
STDMETHODIMP CNAVOptions::Load(void)
{
	
    CCTRACEI("CNAVOptions::Load() called");
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
	StahlSoft::CSmartLock smLock(&m_NavOptsObjLockMutex);
	//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

	HRESULT hr;
	bool bWarnedDefaults = false; // Only want to warn about defaults once per load

    // Initialize Savrt32.dll and it's functions, the initialize funciton
    // will make the error information if it fails, we need to display it since
    // the load is probably not coming from script
    if( FAILED(hr = initializeSAVRTDLL()) )
    {
        CCTRACEE("CNAVOptions::Load() Failed to initialize the savrt32.dll. Returning Error");
        m_NavOptsObjLockMutex.Unlock();
        m_spError->LogAndDisplay(0);
        return hr;
    }

	// If the file is unloadable make 2 attempts to fix it.
	// 1. Restore the last known good version of the file.
	// 2. Restore default options file.
	if (forceError (IDS_Wrn_LoadFailed))
    {
        m_NavOptsObjLockMutex.Unlock();
        MakeOptionsError(IDS_Wrn_LoadFailed, E_FAIL);
        m_spError->LogAndDisplay(0);
		return E_FAIL;
    }

    // Load the NAV settings
    if( !m_pNavOpts->Init() )
    {
        // Failed to load the option settings
        CCTRACEE("CNAVOptions::Load() Failed to load the option settings, attempting to restore defaults then load.");

        // First attempt to restore from the backup hive. If this succeeds we don't need to warn the
        // user because this is the last set of options they saved
		/* -- This does not make any sense since NAV options moved to ccSettings:
		 * -- if NAVOPTS.DAT is corrupted, NAVOPTS.BAK will be corrupted is well.

        CNAVOptSettingsCache NavOptBak;
        if( !NavOptBak.Init("NAVOPTS.BAK", FALSE) || !m_pNavOpts->Migrate(NavOptBak) )
		*/
        {
            // CCTRACEE("CNAVOptions::Load() - Failed to restore from the backup setings hive. Attempting to load from the default hive.");

            // Attempt to restore from the defaults hive. From here on out the user needs to be warned
            // 
            if (!m_bNavDefsInitialized)
	        {
		        // Use the default options file name in order to initialize
                // the default ccSettings NAV hive
                if( !m_pNavDefs->Init(_T("NAVOPTS.DEF"), FALSE) )
		        {
                    CCTRACEE("CNAVOptions::Load() - Failed to initialize the navopts.def ccSetting hive.");
                }
                else
                    m_bNavDefsInitialized = true;
	        }

            if( !m_bNavDefsInitialized || !m_pNavOpts->Migrate(*m_pNavDefs) )
            {
                CCTRACEE("CNAVOptions::Load() - Failed to restore from the default settings hive. Using navopts.dat file.");
                // Last resort is to restore from the navpopts.dat file
                // to ccSettings
                if( S_OK != restore(_T("\\NAVOPTS.DAT")) || !m_pNavOpts->Init() )
                {
                    // We're in trouble - no file to load options from
                    m_NavOptsObjLockMutex.Unlock();
                    CCTRACEE("CNAVOptions::Load() Failed to load the option settings after restoring defaults.");
                    MakeOptionsError(IDS_Wrn_LoadFailed, E_FAIL);
                    m_spError->LogAndDisplay(0);
			        return E_FAIL;
                }
                else
                    bWarnedDefaults = true;
            }
            else
                bWarnedDefaults = true;
        }

        //
        // Now we have restored and loaded the options
        //

        // Warn about the defaults if we had to use the default file
        if( bWarnedDefaults )
            warning(IDS_Wrn_Default);

        // Restore the licensing state
        if( FAILED(restoreLicensingState()) )
        {
            // Just log the failure
            CCTRACEE("CNAVOptions::Load() - Failed to restore the licensing state when setting all defaults");
        }

        // Notify everyone that the options may have changed.
		notify();
    }

    // Save the maximum value for setting names and data
    m_pNavOpts->GetMaxNavSettingsSizes(m_dwMaxName, m_dwMaxData);

    try
    {
        // Load the AP options from SavRT32.dll
		if( forceError (IDS_Err_LoadAPOptions) )
		{
            m_NavOptsObjLockMutex.Unlock();
			MakeError(IDS_Err_LoadAPOptions, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
			m_spError->LogAndDisplay(0);
			return E_FAIL;
		}

        SAVRT_OPTS_STATUS savrtLoadRet = m_SavrtOptions.Load();
        if( SAVRT_OPTS_OK != savrtLoadRet )
        {
            CCTRACEE("Error: LoadAPOptions() returned %d, attempting to load the defaults", savrtLoadRet);
			// If the original file couldn't be loaded attempt to load the
			// defaults
			if( SAVRT_OPTS_OK != (savrtLoadRet = m_SavrtOptions.RestoreDefaultsAndLoad()) )
			{
                m_NavOptsObjLockMutex.Unlock();
                CCTRACEE("Error: RestoreAPDefaultsAndLoad() returned %d, erroring out", savrtLoadRet);
				MakeError(IDS_Err_LoadAPOptions, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
				m_spError->LogAndDisplay(0);
				return E_FAIL;
			}
			else
			{
                // If the hide AP icon option is not supposed to be displayed then NIS is overtaking
                // the AP icon so reset this value to off regardless of the default value
                DWORD dwHide = 0;
                m_pNavOpts->GetValue(NAVOPTION_HideAPIconOption, dwHide, 0);

                if( dwHide )
                {
                    CCTRACEI(_T("CNAVOptions::Load() - The hide AP icon option is set...setting the TSR:HideIcon value to 1 in savrt.dat after restoring defaults"));
                    if( SAVRT_OPTS_OK != m_SavrtOptions.SetDwordValue(TSR_HideIcon, 1) )
                        CCTRACEE(_T("CNAVOptions::Load() - Failed to set the TSR:HideIcon setting to 1"));
                    else
                    {
                        if( SAVRT_OPTS_OK != m_SavrtOptions.Save() )
                            CCTRACEE("CNAVOptions::Load() - Attempting to restore the AP defaults. Failed to save TSR:HideIcon to savrt.dat");
                    }
                }
                
                //
                // Restore the virus definitions path
                //

                // Need to get the AP defs path from defutils
                char pszDir[MAX_PATH] = {0};
                DefUtilsLoader DefUtilsLoader;

                { // Scope the object
                    IDefUtilsPtr pIDefUtils;
                    if( SYM_SUCCEEDED(DefUtilsLoader.CreateObject(pIDefUtils.m_p)) )
                    {
                        if( g_OSInfo.IsWinNT() )  // NT
                        {
                            if( pIDefUtils->InitWindowsApp(g_szNavAppId1ApNt) )
                                pIDefUtils->GetCurrentDefs(pszDir, MAX_PATH);
                            else
                                CCTRACEE("Attempting to restore the AP defaults. Failed to InitWindowsApp for AP ID");
                        }
                        else // 9x
                        {
                            if( pIDefUtils->InitWindowsApp(g_szNavAppId1Ap95) )
                                pIDefUtils->GetCurrentDefs(pszDir, MAX_PATH);
                            else
                                CCTRACEE("Attempting to restore the AP defaults. Failed to InitWindowsApp for AP ID");
                        }
                    }
                    else
                    {
                        CCTRACEE("Attempting to restore the AP defaults. Failed to create the defutils object");
                    }
                }// end defutils scope

                if( SAVRT_OPTS_OK != m_SavrtOptions.SetStringValue(AP_VirusDefinitionsPath, pszDir) )
                    CCTRACEE("Attempting to restore the AP defaults. Failed to set the defs directory to savrt.dat");
                else
                {
                    if( SAVRT_OPTS_OK != m_SavrtOptions.Save() )
                        CCTRACEE("Attempting to restore the AP defaults. Failed to save the defs directory to savrt.dat");
                }

				// Restored the defaults for AP, warn the user if
				// we haven't already
				if( !bWarnedDefaults )
					warning(IDS_Wrn_Default);
			}
        }
        else
        {
            CCTRACEI("Loaded AP/SAVRT options successfully.");
        }
    }
    catch(...)
    {
        // AP options load failed
        //
        m_NavOptsObjLockMutex.Unlock();
        CCTRACEE("Caught an exception during the AP Loading, erroring out");
        MakeError(IDS_Err_LoadAPOptions, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    m_spError->LogAndDisplay(0);
        return E_FAIL;
    }

    USES_CONVERSION;

	Reset(); // Clear the property map
	m_OptionsTypes.clear();

    _NAVSETTINGSMAP* pMap = NULL;
    m_pNavOpts->GetSettingsMap(&pMap);
    if( pMap && EnumerateSettings(pMap) )
    {}
    else
    {
        // Can't get the options info so we can't really do anything
        m_NavOptsObjLockMutex.Unlock();
        MakeError(IDS_Err_Load, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        m_spError->LogAndDisplay(0);
        CCTRACEE("CNAVOptions::Load() Failed to get the options map for enumeration.");
	    return E_FAIL;
    }

    // Get the AP options info
	unsigned int uCount = 0, uAPCount = 0;
    PSAVRT_OPTS_VALUE_INFO pAPInfo = NULL, pAPCurrent = NULL;
    pair<OPTYPEMAP::iterator, bool> pr;
    
    SAVRT_OPTS_STATUS optsstatus = m_SavrtOptions.GetValuesInfo( &pAPInfo, &uAPCount );

    if( !forceError (IDS_Err_LoadAPOptions) &&
        SAVRT_OPTS_OK == optsstatus )
    {
        // Walk the AP options info structure
        for( unsigned int j = 0; j < uAPCount; j++ )
        {
            WCHAR wcGroup[SAVRT_OPTS_MAX_ATTRIBUTE_NAME];
            pAPCurrent = &pAPInfo[j];
            
            // Save the type of this option so we can coerce VARIANTs
            pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(A2W(pAPCurrent->szValue), pAPCurrent->iType));
            _ASSERT(pr.second);  // Validate that such value didn't exist
            
            ::wcscpy(wcGroup, pr.first->first.c_str());
            // Q: Does this group already exist?
            if (::wcstok(wcGroup, L":") && !PropertyExists(wcGroup))
            {
                // We need to add the group
                AddAPProperty(wcGroup);
            }
        }

        // Free the AP Options info
        if( pAPInfo )
            m_SavrtOptions.FreeValuesInfo( pAPInfo );
    }
    else
    {
        CCTRACEE ("CNAVOptions::Load() - error calling GetValuesInfo - did it get corrupted? - %d", optsstatus);
    }

	// Force the custom properties to re-load
	for (int i = 0; i < SIZEOF(m_Properties); i++)
		m_Properties[i]->Init();

    //
    // Add the email options to the options names list
    //

    // First add the NAVPROXY group if it isn't already there
    // Q: Does this group already exist?
	if ( !PropertyExists(L"NAVPROXY"))
		// We need to add the group
		AddProperty(L"NAVPROXY");

    // Add the Tray Animation as "NAVPROXY:ShowTrayIcon" so the script doesn't have to change
    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(A2W(_T("NAVPROXY:ShowTrayIcon")), ccSettings::SYM_SETTING_DWORD));
	_ASSERT(pr.second);  // Validate that such value didn't exist
	
    // Add the Outgoing Progress as "NAVPROXY:ShowProgressOut" so the script doesn't have to change
    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(A2W(_T("NAVPROXY:ShowProgressOut")), ccSettings::SYM_SETTING_DWORD));
	_ASSERT(pr.second);  // Validate that such value didn't exist

    // Add the TimeOutProtection as "NAVPROXY:TimeOutProtection" so the script doesn't have to change
    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(A2W(_T("NAVPROXY:TimeOutProtection")), ccSettings::SYM_SETTING_DWORD));
	_ASSERT(pr.second);  // Validate that such value didn't exist

    // Load IWP if it isn't loaded yet
    //
    if(!m_pIWPSettings)
    {
        m_bIWPDirty = false;
        if( SYM_FAILED(IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(),&m_pIWPSettings)) )
        {
            // No IWP installed
            CCTRACEI ("CNAVOptions::Load - No IWP installed");
            m_pIWPSettings = NULL;
        }
    }

	return S_OK;
}

STDMETHODIMP CNAVOptions::Save(void)
{
	CCTRACEI("CNAVOptions::Save()");

    // Make sure that it is used by an authorized Symantec application
	if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
	StahlSoft::CSmartLock smLock(&m_NavOptsObjLockMutex);
	//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

	HRESULT hr;
	bool bOptionsIsDirty = m_bIsDirty;
	BOOL bExclusionsIsDirty = FALSE;

	// Flush the regular navopts.dat options if dirty
	if (bOptionsIsDirty)
	{
		// Save the options that are stored in the options file.
        if( !m_pNavOpts->Save() )
        {
            CCTRACEE("CNAVOptions::Save() Failed to save the option settings, returning error.");
            MakeError(IDS_Err_Save, E_FAIL, IDS_NAVERROR_INTERNAL);
            m_NavOptsObjLockMutex.Unlock();
	        return E_FAIL;
        }

		/* -- This does not make any sense since NAV options moved to ccSettings:
		 * -- if NAVOPTS.DAT is corrupted, NAVOPTS.BAK will be corrupted is well.
        // Create a backup of the settings
        CNAVOptSettingsCache NavOptBak;
        NavOptBak.Init("NAVOPTS.BAK", FALSE);
        if( !NavOptBak.Migrate(m_NavOpts) )
        {
            CCTRACEE("CNAVOptions::Save() - Failed to create a backup of the options setings.");
        }
		*/
	}

    // Save any dirty custom property options
    for (int i = 0; i < SIZEOF(m_Properties); i++)
	{
        if (m_Properties[i]->IsDirty())
        {
			if (FAILED(hr = m_Properties[i]->Save()))
            {
                CCTRACEE("CNAVOptions::Save() Failed to save custom property %d, moving to next property.", i);
				continue;
            }

            // Set the dirty flag
            bOptionsIsDirty = true;
        }
	}

    // Save the AP options if dirty
    bool bAPOptionsChanged = false;
    try
    {
        if( SAVRT_OPTS_OK == m_SavrtOptions.AreOptionsDirty() )
        {
            // Dirty, need to save them
            if ( forceError (IDS_ERR_SAVE_AP))
                throw FALSE;

            SAVRT_OPTS_STATUS apSaveStatus = m_SavrtOptions.Save();

            if( SAVRT_OPTS_OK != apSaveStatus && SAVRT_OPTS_FALSE != apSaveStatus )
            {
                CCTRACEE("Failed to save the AP options.");
                m_NavOptsObjLockMutex.Unlock();
                MakeError(IDS_ERR_SAVE_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
	            m_spError->LogAndDisplay(0);
            }

            // Set the dirty flag so the notification events occur
            bOptionsIsDirty = true;
            bAPOptionsChanged = true;
        }
    }
    catch(...)
    {
        CCTRACEE("Failed to save the AP options.");
        m_NavOptsObjLockMutex.Unlock();
        MakeError(IDS_ERR_SAVE_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
	    m_spError->LogAndDisplay(0);
    }

    // Close the AP exclusions
    if( m_hAPExclusions )
    {
        BOOL bCommit = FALSE;
        if( APExclusionIsDirty )
            bCommit = APExclusionIsDirty(m_hAPExclusions);

        if( APExclusionClose )
        {
            APExclusionClose(&m_hAPExclusions, bCommit);
            m_hAPExclusions = NULL;
        }

		bExclusionsIsDirty = bCommit || bExclusionsIsDirty;
    }

    // Save the advanced Email options
    if( m_bAdvEmlOptsDirty && m_pEmailOptions )
    {
        if( forceError ( IDS_Err_Set_EmailOption ) ||
            !m_pEmailOptions->SetValue(CCEMLPXY_TRAY_ANIMATION, m_bEmlTray) ||
            !m_pEmailOptions->SetValue(CCEMLPXY_OUTGOING_PROGRESS, m_bEmlProgress) ||
            !m_pEmailOptions->SetValue(CCEMLPXY_TIMEOUT_PROTECTION, m_bEmlTimeout) )
        {
            CCTRACEE("Failed to save the advanced email options");
            MakeError(IDS_Err_Set_EmailOption,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            m_spError->LogAndDisplay(0);
        }
        else
        {
            // Set the dirty flag
            bOptionsIsDirty = true;
        }
    }

    // Save the threat exclusions
    if( m_bExclusionsDirty )
    {
        if( forceError(IDS_ERR_SAVE_THREAT_EXCLUSIONS) || 
            m_spExclusionMgr->saveState() != NavExclusions::Success)
        {
            CCTRACEE("Failed to save exclusions");
            MakeError(IDS_ERR_SAVE_THREAT_EXCLUSIONS, E_FAIL, IDS_NAVERROR_INTERNAL);
            m_spError->LogAndDisplay(0);
        }
        else
        {
            m_bExclusionsDirty = false;
            bExclusionsIsDirty = TRUE;
        }
    }

    // Save the IWP settings
    if ( m_bIWPDirty && m_pIWPSettings )
    {
        if ( SYM_FAILED (m_pIWPSettings->Save ()))
        {
            CCTRACEE (_T("CNAVOptions::Save - IWP settings failed"));
            // Set the dirty flag so we notify
            bOptionsIsDirty = true;
        }
        else // reset the dirty bit on a successful save
            m_bIWPDirty = false;
    }

    // Configure symprotect if necessary
    if( m_pSymProtect && m_bSymProtectDirty )
    {
        // First get the current state
        bool bCurState = false;
        bool bError = false;
        CCTRACEI(_T("CNAVOptions::Save() - Attempting to configure SymProtect."));

        if( !m_pSymProtect->GetStatus(bCurState, bError) )
        {
            CCTRACEE(_T("CNAVOptions::Save() - Failed to get the status for SymProtect...going to attempt to change the state anyway."));
        }

        // User wants symprotect enabled
        if( m_bSymProtectEnable )
        {
            CCTRACEI(_T("CNAVOptions::Save() - Attemping to enable SymProtect. CurState = %d"), (int)bCurState);

            // Always make this call, even if GetStatus() returns ENABLED
            if( !m_pSymProtect->SetComponentEnabled(true) )
                CCTRACEE (_T("CNAVOptions::Save() - Failed to enable SymProtect"));
            else
            {
                CCTRACEI(_T("CNAVOptions::Save() - SymProtect enabled."));
                bOptionsIsDirty = true;
            }
        }
        // User wants symprotect disabled
        else
        {
            CCTRACEI(_T("CNAVOptions::Save() - Attemping to disable SymProtect. CurState = %d"), (int)bCurState);

            // Always make this call, even if GetStatus() returns DISABLED
            if( !m_pSymProtect->SetComponentEnabled(false) )
                CCTRACEE (_T("CNAVOptions::Save() - Failed to disable SymProtect."));
            else
            {
                CCTRACEI(_T("CNAVOptions::Save() - SymProtect disabled."));
                bOptionsIsDirty = true;
            }
        }

        m_bSymProtectDirty = false;
    }

	if (bOptionsIsDirty || bExclusionsIsDirty)
	{
		notify();

        // Prevents spamming AP changes. AP needs to reload drivers in 
        // many cases so we should wait until it's ready before returning.
        // This is so cheesy...
        //
        if (bAPOptionsChanged)
        {
            CCTRACEI ("CNAVOptions::Save() - waiting for AP to finish");

            CGlobalEvent eventAPDone;
            if( eventAPDone.Create( SYM_REFRESH_AP_STATUS_EVENT ))
            {
                if ( WAIT_TIMEOUT == ::WaitForSingleObject ( eventAPDone, 5*1000 )) // 5 secs tops
                    CCTRACEW ("CNAVOptions::Save() - timed out waiting for AP");
                else
                    CCTRACEI ("CNAVOptions::Save() - AP done");
            }
            else
                CCTRACEE ( "CNAVOptions::Save() - waiting for AP, failed to create the AP event" );
            
        }

		//return hr;
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::Get(/*[in]*/ BSTR bstrOption, /*[in]*/ VARIANT vDefault, /*[out, retval]*/ VARIANT *pvValue)
{
	USES_CONVERSION;
	HRESULT hr;
	long eType;

    if (forceError (ERR_INVALID_ARG) ||
        !bstrOption || !pvValue)
    {
        MakeError(ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
	    return E_INVALIDARG;
    }

	::VariantInit(pvValue);

	if (FAILED(hr = get__TypeOf(bstrOption, &eType)))
		return hr;

    // Need to decide if we're reading from AP options or
    // the regular NavOptions

    // Get the group name to see if it's an AP option group
    wstring wstrGroupName = bstrOption;
    wstrGroupName = wstrGroupName.substr(0, wstrGroupName.find_first_of(L":"));

	//option name for checking if its an e-mail option
	CComBSTR bstrOptName;

    if( !IsAPProperty( wstrGroupName.c_str() ) )
    {
	    CBuffer sbDefault(m_dwMaxData)
	            , sbValue(m_dwMaxData);

        // Normalize the default
	    if ( forceError (IDS_Err_Get))
        {
			MakeError(IDS_Err_Get, E_FAIL, IDS_NAVERROR_INTERNAL);
			return E_FAIL;
        }


        switch(eType)
	    {
	    case ccSettings::SYM_SETTING_STRING:
		    // Coerce from any other type to a BSTR (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vDefault, &vDefault, 0U, VT_BSTR)))
			    return hr;

            if ( ::SysStringLen ( vDefault.bstrVal ) > m_dwMaxData )
                return E_POINTER;

            sbDefault.Convert(vDefault.bstrVal);
			
			// Get the stored value
			if(FAILED(m_pNavOpts->GetValue(OLE2T(bstrOption)
									  , sbValue
									  , m_dwMaxData
									  , sbDefault)) )
			{
                // Insert non existing values into the options file
			    if (SUCCEEDED(m_pNavOpts->SetValue(OLE2T(bstrOption), sbValue)))
                {
                    CCTRACEI(_T("CNAVOptions::Get() - Failed to get option %s, added it with value %s"), OLE2T(bstrOption), sbValue);
                    m_bIsDirty = true;
                    // Update the max sizes since they may have increased with the new Set
                    m_pNavOpts->GetMaxNavSettingsSizes(m_dwMaxName, m_dwMaxData);
                }
                else
                {
				    MakeError(IDS_Err_Get, E_FAIL, IDS_NAVERROR_INTERNAL);
				    return E_FAIL;
                }
			}
		    
			// Convert the value to a VARIANT
		    return sbValue.Convert(pvValue);

	    case ccSettings::SYM_SETTING_DWORD:
		    // Coerce from any other type to a long (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vDefault, &vDefault, 0U, VT_UI4)))
			    return hr;

            // Check if it's an Email option
            if( IsEmailOption( bstrOption, &bstrOptName ) )
            {
                if( FAILED(hr = initializeCCEmailPxy()) )
                    // If this fails the initialize function will have already filled in the NAVError object
                    return hr;

                bool bVal = false;

				if( forceError( IDS_Err_Get_EmailOption ) )
				{
					MakeError(IDS_Err_Get_EmailOption,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
                    return E_FAIL;
				}

                if( 0 == _tcscmp(CCEMLPXY_TRAY_ANIMATION, OLE2T(bstrOptName)) )
                    bVal = m_bEmlTray;
				else if( 0 == _tcscmp(CCEMLPXY_OUTGOING_PROGRESS, OLE2T(bstrOptName)) )
					bVal = m_bEmlProgress;
				else if( 0 == _tcscmp(CCEMLPXY_TIMEOUT_PROTECTION, OLE2T(bstrOptName)) )
                    bVal = m_bEmlTimeout;
				else // Unknown email option name...
				{
                    MakeError(IDS_Err_Get_EmailOption,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
                    return E_FAIL;
				}

                if( bVal )
                    pvValue->ulVal = 1;
                else
                    pvValue->ulVal = 0;
            }
            else
            {
		        if ( forceError ( IDS_Err_Get ))
                {
                    MakeError(IDS_Err_Get, E_FAIL, IDS_NAVERROR_INTERNAL);
				    return E_FAIL;
                }

                // Get the stored value
		        if(FAILED(m_pNavOpts->GetValue(OLE2T(bstrOption)
		                                    , pvValue->ulVal
		                                    , vDefault.ulVal)) )
		        {
			        // Insert non existing values into the options file
			        if (SUCCEEDED(m_pNavOpts->SetValue(OLE2T(bstrOption), pvValue->ulVal)))
                    {
                        CCTRACEI(_T("CNAVOptions::Get() - Failed to get option %s, added it with value %d"), OLE2T(bstrOption), pvValue->ulVal);
                        m_bIsDirty = true;
                    }
                    else
                    {
		                MakeError(IDS_Err_Get, E_FAIL, IDS_NAVERROR_INTERNAL);
				        return E_FAIL;
                    }
                }
            }

		    pvValue->vt = VT_UI4;
		    return S_OK;

	    default:  // Unknown option data type
		    return DISP_E_TYPEMISMATCH;
	    }
        
    }
    else
    {
        // Getting AP option
        SAVRT_OPTS_STATUS or;
        CBuffer sbDefault(SAVRT_OPTS_MAX_STRING_LENGTH)
	          , sbValue(SAVRT_OPTS_MAX_STRING_LENGTH);

        // Normalize the default
	    switch(eType)
	    {
	    case SAVRT_OPTS_DATA_STRING:
		    // Coerce from any other type to a BSTR (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vDefault, &vDefault, 0U, VT_BSTR)))
			    return hr;

            if ( ::SysStringLen ( vDefault.bstrVal ) > SAVRT_OPTS_MAX_STRING_LENGTH )
                return E_POINTER;

            sbDefault.Convert(vDefault.bstrVal);

		    // Get the stored value
            if (forceError (IDS_ERR_PUT_AP))
            {
                MakeError(IDS_ERR_PUT_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }
		    
            switch(or = m_SavrtOptions.GetStringValue(OLE2T(bstrOption)
                                                        , sbValue
		                                                , SAVRT_OPTS_MAX_STRING_LENGTH
		                                                , OLE2T(vDefault.bstrVal)))
		    {
		    case SAVRT_OPTS_VALUE_NOT_FOUND:
			    // Insert non existing values into the options file
			    m_bIsDirty = true;
			    if (forceError (IDS_ERR_PUT_AP) ||
                    SAVRT_OPTS_OK != (or = m_SavrtOptions.SetStringValue(OLE2T(bstrOption), sbValue)))
                {
                    MakeError(IDS_ERR_PUT_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				    return E_FAIL;
                }
		    case SAVRT_OPTS_OK:
			    break;
		    default:
                MakeError(IDS_ERR_GET_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
		    }

		    // Convert the value to a VARIANT
		    return sbValue.Convert(pvValue);

	    case SAVRT_OPTS_DATA_DWORD:
		    // Coerce from any other type to a long (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vDefault, &vDefault, 0U, VT_UI4)))
			    return hr;

            if ( forceError (IDS_ERR_GET_AP) )
            {
                MakeError(IDS_ERR_GET_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }
        
		    // Get the stored value
		    switch(or = m_SavrtOptions.GetDwordValue(OLE2T(bstrOption)
		                                            , &pvValue->ulVal
		                                            , vDefault.ulVal))
		    {
		    case SAVRT_OPTS_VALUE_NOT_FOUND:
			    // Insert non existing values into the options file
			    m_bIsDirty = true;
			    if (forceError (IDS_ERR_PUT_AP) ||
                    SAVRT_OPTS_OK != (or = m_SavrtOptions.SetDwordValue(OLE2T(bstrOption), pvValue->ulVal)))
                {
                    MakeError(IDS_ERR_PUT_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				    return E_FAIL;
                }
		    case SAVRT_OPTS_OK:
			    break;
		    default:
                MakeError(IDS_ERR_GET_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }

		    pvValue->vt = VT_UI4;
		    return S_OK;

        case SAVRT_OPTS_DATA_BINARY:
		    if (forceError (IDS_ERR_GET_AP))
            {
                MakeError(IDS_ERR_GET_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }

            // Get the stored value
            pvValue->pbVal = new BYTE[SAVRT_OPTS_MAX_BINARY_LENGTH];
            if( pvValue->pbVal == NULL )
            {
                CCTRACEE(_T("CNAVOptions::Get() - Failed to allocate memory to get the AP binary option %s"), OLE2T(bstrOption));
                MakeError(IDS_ERR_GET_AP, E_OUTOFMEMORY, IDS_NAVERROR_INTERNAL);
                return E_OUTOFMEMORY;
            }

		    switch(or = m_SavrtOptions.GetBinaryValue(OLE2T(bstrOption)
		                                            , pvValue->pbVal
		                                            , SAVRT_OPTS_MAX_BINARY_LENGTH))
		    {
		    case SAVRT_OPTS_VALUE_NOT_FOUND:
			    // Insert non existing values into the options file if a binary default was supplied
                if( vDefault.pbVal )
                {
			        m_bIsDirty = true;
			        if (forceError (IDS_ERR_PUT_AP) ||
                        SAVRT_OPTS_OK != (or = m_SavrtOptions.SetBinaryValue(OLE2T(bstrOption), vDefault.pbVal, SAVRT_OPTS_MAX_BINARY_LENGTH)))
                    {
                        MakeError(IDS_ERR_PUT_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				        return E_FAIL;
                    }
                }
                else
                {
                    if( pvValue->pbVal )
                    {
                        delete [] pvValue->pbVal;
                        pvValue->pbVal = NULL;
                    }
                    CCTRACEE(_T("CNAVOptions::Get() - Failed to get AP binary option %s and no binary default supplied to set."), OLE2T(bstrOption));
                }
		    case SAVRT_OPTS_OK:
			    break;
		    default:
                if( pvValue->pbVal )
                {
                    delete [] pvValue->pbVal;
                    pvValue->pbVal = NULL;
                }
                MakeError(IDS_ERR_GET_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }

            
            pvValue->vt = VT_BYREF|VT_UI1;
		    return S_OK;

	    default:  // Unknown option data type
		    return DISP_E_TYPEMISMATCH;
	    }
    }
}

STDMETHODIMP CNAVOptions::Put(/*[in]*/ BSTR bstrOption, /*[in]*/ VARIANT vValue)
{
	USES_CONVERSION;
	long eType;
	HRESULT hr;
    HRESULT hrGet = S_OK;
	DWORD dwOldValue = 0UL;

	if (!bstrOption)
		return Error(_T("Put()"), E_INVALIDARG);


	if (FAILED(hr = get__TypeOf(bstrOption, &eType)))
		return hr;

	//option name for checking if its an e-mail option
	CComBSTR bstrOptName;

    //
    // Determine if we are putting an AP Option or a NAV option
    //

    // Get the group name to see if it's an AP option group
    wstring wstrGroupName = bstrOption;
    wstrGroupName = wstrGroupName.substr(0, wstrGroupName.find_first_of(L":"));

    if( !IsAPProperty( wstrGroupName.c_str() ) )
    {
        CBuffer sbBuffer, sbValue(m_dwMaxData);
        
        // Need to sort the list retrieved from file if the option being set
        // is the extension list
        CBuffer szSortedList(m_dwMaxData);

	    switch(eType)
	    {
	    case ccSettings::SYM_SETTING_STRING:  // String
		    // Coerce from any other type to a BSTR (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vValue, &vValue, 0U, VT_BSTR)))
			    return hr;

            if ( forceError (IDS_Err_Get))
            {
				MakeError(IDS_Err_Get, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }

			// Compare to the old value to see if anything changed
			hrGet = m_pNavOpts->GetValue(OLE2T(bstrOption), sbValue, m_dwMaxData, "");

            // Have to sort the list stored in the options because the one passed from the
            // script is sorted
            if( 0 == _tcscmp(OLE2T(bstrOption), SCANNER_Ext) )
            {
                SortExtList(sbValue, szSortedList, m_dwMaxData);
                sbValue = (char *)szSortedList;
            }

			if (FAILED(hr = sbBuffer.Convert(vValue)))
				return hr;

			if (sbValue != sbBuffer || FAILED(hrGet))
			{
				m_bIsDirty = true;
				if ( forceError (IDS_Err_Put) ||
                     FAILED(m_pNavOpts->SetValue(OLE2T(bstrOption), sbBuffer)) )
				{
                    MakeError(IDS_Err_Put, E_FAIL, IDS_NAVERROR_INTERNAL);
				    return E_FAIL;
                }

                // Update the max sizes since they may have increased with the new Put
                m_pNavOpts->GetMaxNavSettingsSizes(m_dwMaxName, m_dwMaxData);
			}

            break;

	    case ccSettings::SYM_SETTING_DWORD:
		    // Coerce from any other type to a long (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vValue, &vValue, 0U, VT_UI4)))
			    return hr;

            // Find out if it's an email option which is handling differently
            if( IsEmailOption( bstrOption, &bstrOptName ) )
            {
                bool bVal = false;
                m_bEmailDirty = true; // Doesn't compare values, just a general value

				// Need to ensure the CCEmail Proxy initialize has been complete since this
				// reads the initial values for the Advanced Email options
                if( FAILED(hr = initializeCCEmailPxy()) )
                    // If this fails the initialize function will have already filled in the NAVError object
                    return hr;

				// Get the value from the options page
                if( 0 == vValue.ulVal )
                    bVal = false;
                else
                    bVal = true;

				// Compare to the old value to see if it changed
				if( 0 == _tcscmp(CCEMLPXY_TRAY_ANIMATION, OLE2T(bstrOptName)) )
                {
					if( bVal != m_bEmlTray )
                    {
                        m_bEmlTray = bVal;
                        m_bAdvEmlOptsDirty = true;
                    }
                }
				else if( 0 == _tcscmp(CCEMLPXY_OUTGOING_PROGRESS, OLE2T(bstrOptName)) )
                {
					if( bVal != m_bEmlProgress )
                    {
                        m_bEmlProgress = bVal;
                        m_bAdvEmlOptsDirty = true;
                    }
                }
				else if( 0 == _tcscmp(CCEMLPXY_TIMEOUT_PROTECTION, OLE2T(bstrOptName)) )
                {
                    if( bVal != m_bEmlTimeout )
                    {
                        m_bEmlTimeout = bVal;
                        m_bAdvEmlOptsDirty = true;
                    }
                }
				else // Unknown email option name...
				{
                    MakeError(IDS_Err_Set_EmailOption,E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
                    return E_FAIL;
				}
            }
            else // Not an email option
            {
                // check for navemail options
                if (!wcscmp(wstrGroupName.c_str(),L"NAVEMAIL"))
                {
                    m_bEmailDirty = true;
                }

                // check for im options
                if (!wcscmp(wstrGroupName.c_str(),L"IMSCAN"))
                {
                    m_bIMScanDirty = true;
                }

			    if (forceError ( IDS_Err_Get ))
                {
			        MakeError(IDS_Err_Get, E_FAIL, IDS_NAVERROR_INTERNAL);
				    return E_FAIL;
                }

                // Compare to the old value to see if anything changed
                hrGet = m_pNavOpts->GetValue(OLE2T(bstrOption), dwOldValue, dwOldValue);

		        // If this is a new or changed value, set it
			    if (dwOldValue != vValue.ulVal || FAILED(hrGet))
			    {
				    m_bIsDirty = true;
				    if ( FAILED(m_pNavOpts->SetValue(OLE2T(bstrOption), vValue.ulVal)) )
					{
                        MakeError(IDS_Err_Put, E_FAIL, IDS_NAVERROR_INTERNAL);
				        return E_FAIL;
                    }
			    }
            }
		    break;

	    default:  // Unknown option data type
		    return DISP_E_TYPEMISMATCH;
	    }
    }
    else // Put the AP Option into the AP option file using savrt32.dll
    {
        SAVRT_OPTS_STATUS or;
        CBuffer sbBuffer, sbValue(SAVRT_OPTS_MAX_STRING_LENGTH);

        // Check the force error case
        if ( forceError (IDS_ERR_PUT_AP) )
        {
            MakeError(IDS_ERR_PUT_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
			return E_FAIL;
        }

	    switch(eType)
	    {
	    case SAVRT_OPTS_DATA_STRING:  // String
		    // Coerce from any other type to a BSTR (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vValue, &vValue, 0U, VT_BSTR)))
			    return hr;

            if ( ::SysStringLen ( vValue.bstrVal ) > SAVRT_OPTS_MAX_STRING_LENGTH )
                return E_POINTER;

		    // Compare to the old value to see if anything changed
            switch(or = m_SavrtOptions.GetStringValue( OLE2T(bstrOption)
		                                                , sbValue
		                                                , SAVRT_OPTS_MAX_STRING_LENGTH
		                                                , ""))
		    {
		    case SAVRT_OPTS_OK:
		    case SAVRT_OPTS_VALUE_NOT_FOUND:
			    if (FAILED(hr = sbBuffer.Convert(vValue)))
				    return hr;

			    if (sbValue != sbBuffer || SAVRT_OPTS_VALUE_NOT_FOUND == or )
			    {
				    if ( SAVRT_OPTS_OK != (or = m_SavrtOptions.SetStringValue(OLE2T(bstrOption), sbBuffer)))
                    {
					    MakeError(IDS_ERR_PUT_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				        return E_FAIL;
                    }
			    }
			    break;
		    default:
			    MakeError(IDS_ERR_GET_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }
		    break;

	    case SAVRT_OPTS_DATA_DWORD:
		    // Coerce from any other type to a long (as needed)
		    if (FAILED(hr = ::VariantChangeType(&vValue, &vValue, 0U, VT_UI4)))
			    return hr;
            
            // Compare to the old value to see if anything changed
		    switch(or = m_SavrtOptions.GetDwordValue( W2A(bstrOption)
		                                             , &dwOldValue
		                                             , dwOldValue))
		    {
		    case SAVRT_OPTS_OK:
		    case SAVRT_OPTS_VALUE_NOT_FOUND:
			    if (dwOldValue != vValue.ulVal || SAVRT_OPTS_VALUE_NOT_FOUND == or )
			    {
				    if (SAVRT_OPTS_OK != (or = m_SavrtOptions.SetDwordValue(OLE2T(bstrOption), vValue.ulVal)))
                    {
                        MakeError(IDS_ERR_PUT_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				        return E_FAIL;
                    }
			    }
			    break;
		    default:
			    MakeError(IDS_ERR_GET_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
            }
		    break;

        case SAVRT_OPTS_DATA_BINARY:
            // The Put() function does not take in a buffer size so we don't know how much binary
            // data to write out to AP. Therefore we won't even try. The only binary options currently
            // in AP are the extension lists which we have other exported functions to handle
            CCTRACEE(_T("CNAVOptions::Put() - You cannot use CNAVOptions::Put() to set the binary AP option %s"), OLE2T(bstrOption));
            return S_FALSE;
            break;

	    default:  // Unknown option data type
		    return DISP_E_TYPEMISMATCH;
	    }
    }

	return S_OK;
}

STDMETHODIMP CNAVOptions::Default(void)
{
    CCTRACEI("CNAVOptions::Default() - Begin Defaults");
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
	StahlSoft::CSmartLock smLock(&m_NavOptsObjLockMutex);
	//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

	HRESULT hr;

    CCTRACEI("CNAVOptions::Default() - Calling is it safe");

	// Make sure that it is used by an authorized Symantec application
	if ( forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
         !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    CCTRACEI("CNAVOptions::Default() - It is safe!");

    //
	// Restore default options
    //

    // First attempt to restore from the default hive, if that fails
    // restore from the file

    // Initialize the defaults if they have not been initialized yet
	if (!m_bNavDefsInitialized)
	{
        CCTRACEI("CNAVOptions::Default() - Going to initialize the defaults settings hive");
		// Use the default options file name in order to initialize
        // the default ccSettings NAV hive
        if( !m_pNavDefs->Init(_T("NAVOPTS.DEF"), FALSE) )
		{
            CCTRACEE("CNAVOptions::Default() - Failed to initialize the navopts.def ccSetting hive. Using the file to restore defaults.");
        }
        else
            m_bNavDefsInitialized = true;
        CCTRACEI("CNAVOptions::Default() - COMPLETED: initializing defaults settings hive");
	}

    CCTRACEI("CNAVOptions::Default() - Migrating from the defaults settings hive to the regular options hive");
    // Send in a true as the second parameter to migrate so the DRM keys are not restored
    if( !m_bNavDefsInitialized || !m_pNavOpts->Migrate(*m_pNavDefs, true) )
    {
        CCTRACEE("CNAVOptions::Default() - Could not restore from the default ccSettings hive. Using navopts.dat file.");
	    if (FAILED(hr = restore(_T("\\NAVOPTS.DAT"))))
        {
            CCTRACEE("CNAVOptions::Default() - Could not restore the default settings from the navopts.dat file.");
		    m_spError->LogAndDisplay(0);
        }
        else
        {   // Fix for defect 1-4EXNVQ. Need to pulse IM scanner change event.
            m_bIMScanDirty = true;
        }
    }
    else
    {   // Defect 1-4EXNVQ. Need to pulse IM scanner change event.
        m_bIMScanDirty = true;
    }
    CCTRACEI("CNAVOptions::Default() - COMPLETED: Migrating from the defaults settings hive to the regular options hive");

	// Reset each of the custom properties to default
	for (int i = 0; i < SIZEOF(m_Properties); i++)
	{
        CCTRACEI("CNAVOptions::Default() - Reseting custom property %d", i);
		if (FAILED(hr = m_Properties[i]->Default())
		 || FAILED(hr = m_Properties[i]->Save()))
        {
            CCTRACEE("CNAVOptions::Default() - Failed to restore defaults for custom property: %d", i);
        }

		m_Properties[i]->Init();
        CCTRACEI("CNAVOptions::Default() - COMPLETED: Reseting custom property %d", i);
	}

    // Restore the Auto-Protect Option Defaults
    CCTRACEI("CNAVOptions::Default() - Restoring AP defaults");
    SAVRT_OPTS_STATUS apSaveStatus = m_SavrtOptions.DefaultAll();
    if( !forceError (IDS_Err_SetAPDefault) &&
        SAVRT_OPTS_OK == apSaveStatus )
    {
        // If the hide AP icon option is not supposed to be displayed then NIS is overtaking
        // the AP icon so reset this value to off regardless of the default value
        DWORD dwHide = 0;
        m_pNavOpts->GetValue(NAVOPTION_HideAPIconOption, dwHide, 0);

        if( dwHide )
        {
            CCTRACEI(_T("CNAVOptions::Default() - The hide AP icon option is set...setting the TSR:HideIcon value to 1 in savrt.dat after restoring defaults"));
            if( SAVRT_OPTS_OK != m_SavrtOptions.SetDwordValue(TSR_HideIcon, 1) )
                CCTRACEE(_T("CNAVOptions::Default() - Failed to set the TSR:HideIcon setting to 1"));
        }

        if( forceError (IDS_ERR_SAVE_AP) ||
            SAVRT_OPTS_OK != m_SavrtOptions.Save() )
        {
            MakeError(IDS_ERR_SAVE_AP, E_FAIL, IDS_NAVERROR_INTERNAL);
            m_spError->LogAndDisplay(0);
        }
    }
    else
    {
        MakeError(IDS_Err_SetAPDefault, E_FAIL, IDS_NAVERROR_INTERNAL);
        m_spError->LogAndDisplay(0);
    }
    CCTRACEI("CNAVOptions::Default() - COMPLETED: Restoring AP defaults");

    CCTRACEI("CNAVOptions::Default() - Restoring AP exclusions defaults");
	// Restore default Auto-Protect Exclusions
    if( FAILED( hr = initializeAPExclusions() ) )
    {
        m_spError->LogAndDisplay(0);
    }

    if( APExclusionSetDefault )
    {
        if( forceError (IDS_Err_Set_AP_Default_Exclusions) ||
            SAVRT_OPTS_OK != APExclusionSetDefault(m_hAPExclusions) )
        {
            MakeError (IDS_Err_Set_AP_Default_Exclusions, E_FAIL, IDS_NAVERROR_INTERNAL);
		    m_spError->LogAndDisplay(0);
        }

		// Now Save them
		if( m_hAPExclusions )
		{
			BOOL bCommit = FALSE;
			if( APExclusionIsDirty )
				bCommit = APExclusionIsDirty(m_hAPExclusions);

			if( APExclusionClose )
			{
				APExclusionClose(&m_hAPExclusions, bCommit);
				m_hAPExclusions = NULL;
			}
		}
    }
    CCTRACEI("CNAVOptions::Default() - COMPLETED: Restoring AP exclusions defaults");

    CCTRACEI("CNAVOptions::Default() - Restoring CCemailPxy defaults");
    // Restore default Advanced Email options
    if( FAILED(hr = initializeCCEmailPxy()) )
    {
        // If this fails the initialize function will have already filled in the NAVError object
        m_spError->LogAndDisplay(0);
    }

    if(m_pEmailOptions)
    {
        m_pEmailOptions->Install();
        m_bEmailDirty = true;
    }

    CCTRACEI("CNAVOptions::Default() - COMPLETED: Restoring CCemailPxy defaults");

    CCTRACEI("CNAVOptions::Default() - Restoring threat exclusions defaults");
    // Initialize exclusions, if needed
    if(!this->m_spExclusionMgr)
    {
        m_bExclusionsAreViral = true;   // Faster average-case load time.
        this->LoadExclusions(true);
    }

    if(this->m_spExclusionMgr)
    {
        // Unfiltered reset.
        if(m_spExclusionMgr->resetToDefaults(
                    (NavExclusions::IExclusion::ExclusionState)0,(
                    NavExclusions::IExclusion::ExclusionType)0) != NavExclusions::Success)
        {
            CCTRACEE(_T("CNAVOptions::Default() - Error while loading exclusion defaults."));
            MakeError(IDS_ERR_SET_THREAT_DEFAULTS, E_FAIL, IDS_NAVERROR_INTERNAL);
            m_spError->LogAndDisplay(0);
        }

        // Forcing a save. It appears that everything else in here forces a save
        // to its state, so we have to as well.
        if(m_spExclusionMgr->saveState() != NavExclusions::Success)
        {
            CCTRACEE(_T("CNAVOptions::Default() - Error while saving exclusion state."));
            MakeError(IDS_ERR_SET_THREAT_DEFAULTS, E_FAIL, IDS_NAVERROR_INTERNAL);
            m_spError->LogAndDisplay(0);
        }
    }

    CCTRACEI("CNAVOptions::Default() - COMPLETED: Restoring threat exclusions defaults");

    CCTRACEI("CNAVOptions::Default() - Restoring SymProtect default");
    // Restore the symprotect state to on
    if( InitSymProtect() )
    {
        if( !m_pSymProtect->SetComponentEnabled(true) )
        {
            CCTRACEE("CNAVOptions::Default() - Failed to enable symprotect");
        }
    }
    else
        CCTRACEE("CNAVOptions::Default() - Failed to initialize symprotect");
    CCTRACEI("CNAVOptions::Default() - COMPLETED: Restoring SymProtect default");

    CCTRACEI("CNAVOptions::Default() - Restoring IWP defualt");
    // Restore the IWP state to on
    if( m_pIWPSettings )
    {
        DWORD dwCurState = IWP::IIWPSettings::IWPStateError;
        m_pIWPSettings->GetValue (IWP::IIWPSettings::IWPState, dwCurState);

        if( dwCurState != IWP::IIWPSettings::IWPStateEnabled && // Not currently enabled
            dwCurState != IWP::IIWPSettings::IWPStateYielding)  // Not in the yielding state
        {
            if ( SYM_FAILED (m_pIWPSettings->SetValue (IWP::IIWPSettings::IWPUserWantsOn, 1)))
                CCTRACEE("CNAVOptions::Default() - Could not turn on IWP");
            if( SYM_FAILED (m_pIWPSettings->Save()) )
                CCTRACEE("CNAVOptions::Default() - IWP Save() failed");
        }
    }
    CCTRACEI("CNAVOptions::Default() - COMPLETED: Restoring IWP defualt");

	// Tell everyone that the options changes so they have a chance to reload the file
    m_NavOptsObjLockMutex.Unlock();

    CCTRACEI("CNAVOptions::Default() - notifying...");
	notify();
    CCTRACEI("CNAVOptions::Default() - COMPLETED: notifying...");

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Properties
STDMETHODIMP CNAVOptions::get_Version(/*[out, retval]*/ unsigned long *pulVersion)
{
	if (!pulVersion)
		return Error(_T("get_Version()"), E_POINTER);

    *pulVersion = VER_NUM_PRODVERMAJOR;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_IsDirty(/*[out, retval]*/ BOOL *pbIsDirty)
{
	if (!pbIsDirty)
		return Error(_T("get_IsDirty()"), E_POINTER);

    //
    // Need to check all dirty flags to see if some of the options are dirty
    //

    *pbIsDirty = VARIANT_FALSE;

    // navopts.dat file?
    if( m_bIsDirty )
    {
	    *pbIsDirty = VARIANT_TRUE;
        return S_OK;
    }

    // Custom properties?
    for (int i = 0; i < SIZEOF(m_Properties); i++)
	{
        if (m_Properties[i]->IsDirty())
        {
			*pbIsDirty = VARIANT_TRUE;
            return S_OK;
        }
	}

    // AP Options?
    if( SAVRT_OPTS_OK == m_SavrtOptions.AreOptionsDirty() )
    {
        *pbIsDirty = VARIANT_TRUE;
        return S_OK;
    }

    // AP Exclusions?
    BOOL bAPExclDirty = FALSE;
    if( m_hAPExclusions && APExclusionIsDirty && (bAPExclDirty = APExclusionIsDirty(m_hAPExclusions)) )
    {
        *pbIsDirty = VARIANT_TRUE;
        return S_OK;
    }

    // Advanced email options?
    if( m_bAdvEmlOptsDirty )
    {
        *pbIsDirty = VARIANT_TRUE;
        return S_OK;
    }

    // Threat Exclusions?
    if( m_bExclusionsDirty )
    {
        *pbIsDirty = VARIANT_TRUE;
        return S_OK;
    }

    // Password
    if( m_bIsPasswordDirty )
    {
        *pbIsDirty = VARIANT_TRUE;
        return S_OK;
    }

	return S_OK;
}

STDMETHODIMP CNAVOptions::put_IsDirty( BOOL bIsDirty)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( bIsDirty )
	    m_bIsDirty = true;
    else
        m_bIsDirty = false;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_EXCLUSIONS(/*[out, retval]*/ IDispatch **ppINAVExclusions)
{
    // This method is no longer supported as of NAV 2006
    MakeError(IDS_Err_ExclusionsInit, E_NOTIMPL, IDS_NAVERROR_INTERNAL);
	return E_NOTIMPL;
}

STDMETHODIMP CNAVOptions::ModifyRecycleBin()
{
    DisplayNProtectOptions(::GetForegroundWindow (), 0);

	return S_OK;
}

STDMETHODIMP CNAVOptions::ModifyNPRB()
{
	DisplayNProtectOptions(::GetForegroundWindow (), 1);

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_IsNAVPro(BOOL *pVal)
{
	*pVal = FALSE;
	const TCHAR szProductKey[] = _T("Software\\Symantec\\Norton AntiVirus");

	CRegKey Key;
	LONG lResult = Key.Open(HKEY_LOCAL_MACHINE, szProductKey, KEY_READ);
	
	if (lResult == ERROR_SUCCESS)
	{
		TCHAR szType[2];
		DWORD dwSize = sizeof(szType);
		lResult = Key.QueryValue(_T("Type"), NULL, szType, &dwSize);
		if ((lResult == ERROR_SUCCESS) && (_ttoi(szType) == 1))
			*pVal = TRUE;

		Key.Close();
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_IsIMInstalled(/*[in]*/EIMType IMType, /*[out, retval]*/BOOL *pVal)
{
    // Intialize return value to not installed
    *pVal = FALSE;

    // Convert EIMType enumeration to IMTYPE enumeration
    IMTYPE imtype;
    switch( IMType )
    {
    case IMTYPE_all:
        imtype = IMTYPE_ALL;
        break;
    case IMTYPE_msn:
        imtype = IMTYPE_MSN;
        break;
    case IMTYPE_aol:
        imtype = IMTYPE_AOL;
        break;
    case IMTYPE_yahoo:
        imtype = IMTYPE_YAHOO;
        break;
    default:
        // Invalid IM Type passed in
        // Populate CNAVError object before returning
        return E_INVALIDARG;
    }

    // Load ccIMScan.dll if necessary
    if( !m_hIMScanDll )
    {
        // Get the full path to the IM scanner
        TCHAR szCCImscanPath[MAX_PATH + 16];
		::_tcscpy(szCCImscanPath, g_NAVInfo.GetNAVDir());
		::_tcscat(szCCImscanPath, _T("\\ccIMScan.dll"));

        // Verify the signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szCCImscanPath) )
        {
            MakeError(ERR_LOAD_CCIMSCAN, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_ACCESSDENIED;
        }

        if ( forceError ( ERR_LOAD_CCIMSCAN ) ||
             NULL == (m_hIMScanDll = LoadLibraryEx(szCCImscanPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)) )
        {           
			MakeError (ERR_LOAD_CCIMSCAN, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }
    }

    // Get the IMScanInstalled function
    pfnISIMCLIENTINSTALLED IsIMScannerInstalled = reinterpret_cast<pfnISIMCLIENTINSTALLED>(::GetProcAddress(m_hIMScanDll, _T("IsIMClientInstalled")));
    
    if (forceError ( ERR_GET_IMSCANINSTALLED ) ||
        NULL == IsIMScannerInstalled)
    {        
		MakeError (ERR_GET_IMSCANINSTALLED, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( IsIMScannerInstalled(imtype) )
        *pVal = TRUE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::ConfigureIM(/*[in]*/ EIMType IMType)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Convert EIMType enumeration to IMTYPE enumeration
    IMTYPE imtype;
    switch( IMType )
    {
    case IMTYPE_all:
        imtype = IMTYPE_ALL;
        break;
    case IMTYPE_msn:
        imtype = IMTYPE_MSN;
        break;
    case IMTYPE_aol:
        imtype = IMTYPE_AOL;
        break;
    case IMTYPE_yahoo:
        imtype = IMTYPE_YAHOO;
        break;
    default:
        // Invalid IM Type passed in
        // Populate CNAVError object before returning
        return E_INVALIDARG;
    }

	// Load ccIMScan.dll if necessary
    if( !m_hIMScanDll )
    {
        // Get the full path to the IM scanner
        TCHAR szCCImscanPath[MAX_PATH + 16];
		::_tcscpy(szCCImscanPath, g_NAVInfo.GetNAVDir());
		::_tcscat(szCCImscanPath, _T("\\ccIMScan.dll"));

        // Verify the signature
        if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(szCCImscanPath) )
        {
            MakeError(ERR_LOAD_CCIMSCAN, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_ACCESSDENIED;
        }

        if (forceError ( ERR_LOAD_CCIMSCAN ) ||
            NULL == (m_hIMScanDll = LoadLibraryEx(szCCImscanPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)) )
        {           
			MakeError (ERR_LOAD_CCIMSCAN, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }
    }

    // Get the IMScanInstalled function
    pfnCONFIGUREIM ConfigureIM = reinterpret_cast<pfnCONFIGUREIM>(::GetProcAddress(m_hIMScanDll, _T("ConfigureIM")));
    
    if (forceError ( ERR_GET_CONFIGUREIM ) ||
        NULL == ConfigureIM)
    {
        // Populate CNAVError object
		MakeError (ERR_GET_CONFIGUREIM, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    m_bIMScanDirty = true;
    HRESULT hr = ConfigureIM(imtype);

    // notify status helper that IM settings have changed
    ccLib::CEvent eventIMScan;
    if ( eventIMScan.Create ( NULL, TRUE, FALSE, SYM_REFRESH_IMSCANNER_STATUS_EVENT, TRUE ))
        eventIMScan.PulseEvent();

    return hr;
}

// CNAVOptions::EnableALU(BOOL bEnable, BOOL bProduct)
//
// Turns ALU ON or OFF for either the NAV registered product or
// the two registered MicroDefs. The first parameter bEnable is
// TRUE to turn ALU ON, or FALSE to turn ALU OFF. The second
// parameter bProduct is TRUE if the NAV product is being enabled/
// disabled or FALSE if the MicroDefs are being enabled/disabled
STDMETHODIMP CNAVOptions::EnableALU(BOOL bEnable, BOOL bProduct)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    HRESULT hr = S_OK;
    // Init the ProductReg LU object
	if (forceError ( IDS_ERR_INIT_PRODREG ) ||
        FAILED(hr = InitProductReg()))
    {
        // Unable to initialize the ProductReg object
        MakeError(IDS_ERR_INIT_PRODREG,hr, IDS_NAVERROR_NDSCHEDULER);
		return hr;
    }
	// Handle the case where we can't get the ProductReg object because
	// ALU is running as a system proccess on NT (LU bug)
	else if( S_FALSE == hr )
	{
		// Just ignore the attempt to change it
		return S_FALSE;
	}

    BSTR bstrMonikers[3] = {NULL,NULL,NULL};

    if( bProduct )
    {
        // Get the moniker for the product. Since the Def command lines also have the
        // string "NAV" in them we need to use NAVNT or NAV95
        TCHAR szProdSubStr[10] = {0};
        if( g_OSInfo.IsWinNT() )
            _tcscpy(szProdSubStr, _T("NAVNT"));
        else
            _tcscpy(szProdSubStr, _T("NAV95"));

        if( forceError ( ERR_GET_NAV_MONIKER ) ||
            !GetLUMonikerFromProductNameSubString(szProdSubStr, bstrMonikers) )
        {
            // Error getting monikers
			GetProductReg().Release();
			MakeError (ERR_GET_NAV_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }
    }
    else
    {
        // Get the monikers for the virus defs
        if( forceError ( ERR_GET_DEFS_MONIKER ) ||
            !GetLUMonikerFromProductNameSubString(_T("MicroDefs"), bstrMonikers) )
        {
            // Error getting monikers
			GetProductReg().Release();
			MakeError (ERR_GET_DEFS_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }
    }

    // Make sure the moniker exists for the substring entered
    if( forceError ( ERR_NOT_EXIST_MONIKER ) ||
        NULL == bstrMonikers[0] )
    {
        // Moniker does not exist for specified substring
		GetProductReg().Release();
		MakeError (ERR_NOT_EXIST_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( bEnable )
    {
        // Enable ALU for the necessary monikers
        for( int i=0; NULL != bstrMonikers[i]; i++)
        {
            if( forceError ( ERR_ENABLE_ALU ) ||
                FAILED (GetProductReg()->SetProperty(bstrMonikers[i], 
									               _bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									               _bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) )) )
            { 
				MakeError (ERR_ENABLE_ALU, E_FAIL, IDS_NAVERROR_INTERNAL);
				GetProductReg().Release();
                return E_FAIL;
            }

			// If setting the product make sure ALU is in noisy mode
			if( bProduct)
			{
				GetProductReg()->SetProperty(bstrMonikers[i],
                                             _bstr_t( AU_REG_UPDATE_TYPE ),
                                             _bstr_t( AU_REG_PATCH_NOISY) );
			}

            // Done with current BSTR so set it free
            SysFreeString(bstrMonikers[i]);
            bstrMonikers[i] = NULL;
        }
    }
    else
    {
        // Disable ALU for the necessary monikers
        for( int i=0; NULL != bstrMonikers[i]; i++)
        {
            if( forceError ( ERR_DISABLE_ALU ) ||
                FAILED( GetProductReg()->SetProperty(bstrMonikers[i], 
									              _bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									              _bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) )))
            {
                MakeError (ERR_DISABLE_ALU, E_FAIL, IDS_NAVERROR_INTERNAL);
				GetProductReg().Release();
                return E_FAIL;
            }
            // Done with current BSTR so set it free
            SysFreeString(bstrMonikers[i]);
            bstrMonikers[i] = NULL;
        }
    }
	GetProductReg().Release();
	return S_OK;
}

// Returns true if ALU is on for the registry CommandLine(s) containing a ProductName
// that has a substring in it equal to the bstrProductNameSubString passed in.
// Possible return values:
// ALUonResult_ERROR        = -1 // Error getting ALU mode
// ALUonResult_OFF		    = 0	 // ALU is off
// ALUonResult_ON	        = 1  // ALU is on
STDMETHODIMP CNAVOptions::get_IsAluOn(/*[in]*/BSTR bstrProductNameSubString, /*[out, retval]*/EALUonResult *result)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Initialize out variable
    if( forceError ( ERR_INVALID_POINTER ) ||
        NULL == result )
    {        
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
		return E_POINTER;
    }
    *result = ALUonResult_ERROR;

    HRESULT hr = S_OK;

    // Init the ProductReg LU object
	if (forceError ( IDS_ERR_INIT_PRODREG ) ||
        FAILED(hr = InitProductReg()))
    {
        // Unable to initialize the ProductReg object
        MakeError(IDS_ERR_INIT_PRODREG,hr, IDS_NAVERROR_NDSCHEDULER);
		return hr;
    }

	if( S_FALSE == hr )
	{
		// Can't get the product reg object cause ALU is running on NT - LU bug!
		// Just return because the page will be disabled in this case anyway
		return S_FALSE;
	}

    // Get the Moniker to check the ALU status of
    BSTR bstrMonikers[3] = {NULL,NULL,NULL};

    // Wrap the raw bstr in a bstr object
    _bstr_t bstrSubStr = bstrProductNameSubString;

    // Get the moniker for the product
    if( forceError ( ERR_GET_MONIKER ) ||
        !GetLUMonikerFromProductNameSubString(bstrSubStr, bstrMonikers) )
    {
        // Error while getting moniker
		GetProductReg().Release();
		MakeError (ERR_GET_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( forceError ( ERR_NOT_EXIST_MONIKER ) ||
        NULL == bstrMonikers[0] )
    {
        // The command line did not exist for input substring
		GetProductReg().Release();
        MakeError (ERR_NOT_EXIST_MONIKER, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
		return E_FAIL;
    }

    VARIANT vALUon;
    VariantInit(&vALUon);

    // Find out if ALU is ON or OFF for the specified commandline
    if( SUCCEEDED ( hr = (GetProductReg()->GetProperty(bstrMonikers[0], 
									       _bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									       &vALUon ))) )
    {
        if( vALUon.vt & VT_BSTR && 
			_bstr_t(AU_REG_USE_AU_FOR_UPDATES_ON) == _bstr_t(vALUon.bstrVal) )
            *result = ALUonResult_ON;
		else
			*result = ALUonResult_OFF;
    }
	// WJORDAN 6-26-02: else = Could not get the ALU property for the given Moniker,
	// this happens if our commandlines are no longer registered - Defect# 389199.
	// Attmepting to fix it by re-registering the commandlines we have in the registry
	// with ALU
	else
	{
		if( SUCCEEDED( hr = registerCommandLines() ) )
        {
            // Try again after re-registering with LU
            if( SUCCEEDED ( hr = (GetProductReg()->GetProperty(bstrMonikers[0], 
									               _bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
									               &vALUon ))) )
            {
                if( vALUon.vt & VT_BSTR && 
					_bstr_t(AU_REG_USE_AU_FOR_UPDATES_ON) == _bstr_t(vALUon.bstrVal) )
                    *result = ALUonResult_ON;
				else
					*result = ALUonResult_OFF;
            }
        }
	}

    // Free the monikers
    for( int i=0; NULL != bstrMonikers[i]; i++ )
    {
        SysFreeString(bstrMonikers[i]);
    }

    // Clear the Variant
    VariantClear(&vALUon);

    if ( forceError (IDS_ERR_GETTING_ALU_MODE))
        hr = E_FAIL;

    if( FAILED(hr) )
    {
		GetProductReg().Release();
        MakeError(IDS_ERR_GETTING_ALU_MODE, hr, IDS_NAVERROR_INTERNAL);
	    return hr;
    }
	GetProductReg().Release();
    return S_OK;
}

STDMETHODIMP CNAVOptions::get_NAVError(INAVCOMError* *pVal)
{
	if ( !m_spError )
        return E_FAIL;

    *pVal = m_spError;
    (*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}

void CNAVOptions::MakeError(long lErrorID, long lHResult, long lNAVErrorResID)
{
    if ( !m_spError )
        return;
    m_spError->put_ModuleID ( AV_MODULE_ID_OPTIONS_UI );
	m_spError->put_ErrorID ( lErrorID );
	m_spError->put_ErrorResourceID ( lNAVErrorResID );
    m_spError->put_HResult ( lHResult );
	_Module.SetResourceInstance(_Module.GetModuleInstance());
}

void CNAVOptions::MakeOptionsError(long lErrorID, long lHResult)
{
	if ( !m_spError )
		return;

	CString csFormat;
	CString csMessage;

	// Load error message
	_Module.SetResourceInstance(g_ResModule);
	csFormat.LoadString(lErrorID);
	csMessage = csFormat;

	// Format error message with product name
	switch(lErrorID)
	{
	case IDS_Wrn_LoadFailed:
		csMessage.Format(csFormat, m_csProductName, m_csProductName);
		break;

	default:
		break;
	}
	
	CComBSTR bstrErrorMessage(csMessage);
	m_spError->put_Message ( bstrErrorMessage );
	m_spError->put_ModuleID ( AV_MODULE_ID_OPTIONS_UI );
	m_spError->put_ErrorID ( lErrorID );
	m_spError->put_HResult ( lHResult );
	_Module.SetResourceInstance(_Module.GetModuleInstance());
}

bool CNAVOptions::forceError(long lErrorID)
{
    if ( !m_spError )
        return false;

    long lTempErrorID = 0;
    long lTempModuleID = 0;

    m_spError->get_ForcedModuleID ( &lTempModuleID );
    m_spError->get_ForcedErrorID ( &lTempErrorID );

    if ( lTempModuleID == AV_MODULE_ID_OPTIONS_UI &&
         lTempErrorID == lErrorID )
         return true;
    else
        return false;
}

STDMETHODIMP CNAVOptions::get_IsPasswordDirty(BOOL *pVal)
{
	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    if( m_bIsPasswordDirty )
        *pVal = VARIANT_TRUE;
    else
        *pVal = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::put_IsPasswordDirty(BOOL newVal)
{
	if( newVal )
	    m_bIsPasswordDirty = true;
    else
        m_bIsPasswordDirty = false;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_ALUrunning(BOOL *pVal)
{
	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

	*pVal = m_bALURunning;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_OEMVendor(BSTR *pVal)
{
	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

	// Get the OEMVendor string from version.dat
    TCHAR szVersionDatPath [MAX_PATH] = {0};
    TCHAR szVendorName[MAX_PATH] = {0};

    _tcscpy(szVersionDatPath, g_NAVInfo.GetNAVDir());
    _tcscat(szVersionDatPath, _T("\\version.dat"));

    GetPrivateProfileString(_T("Versions"), _T("OEMVendor"), _T(""),
		szVendorName, sizeof(szVendorName), szVersionDatPath);

    USES_CONVERSION;
    *pVal = ::SysAllocString(T2OLE(szVendorName));

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_IsHighContrastMode(BOOL *pVal)
{
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Initialize to false
    *pVal = VARIANT_FALSE;

    HIGHCONTRAST highContrast;
    ZeroMemory(&highContrast, sizeof(HIGHCONTRAST));
    highContrast.cbSize = sizeof(HIGHCONTRAST);

    if( SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &highContrast, 0) )
    {
        if (highContrast.dwFlags & HCF_HIGHCONTRASTON)
        {
            *pVal = VARIANT_TRUE;
        }
    }

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_HWND(long *pVal)
{
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    if( NULL == m_MyHwnd )
    {
        m_MyHwnd = ::FindWindow(ccWebWindow_ClassName, m_csTitle);
    }

    *pVal = (long)m_MyHwnd;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_APCanSetSystemStart(BOOL *pVal)
{
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // This function is no longer available from SAVRT32.dll so
    // we just say yes

    *pVal = VARIANT_TRUE;

    return S_OK;
}
 
STDMETHODIMP CNAVOptions::get_IWPIsInstalled(BOOL *pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = m_pIWPSettings ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_IWPState(long *pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = IWP::IIWPSettings::IWPStateError;

    if (m_pIWPSettings)
    {
        if ( SYM_FAILED (m_pIWPSettings->GetValue (IWP::IIWPSettings::IWPState, (DWORD&)*pVal)))
        {
            CCTRACEE ("CNAVOptions::get_IWPState - GetValue failed");
            MakeError (IDS_ERR_GETIWPSTATE, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }

    CCTRACEI("CNAVOptions::get_IWPState - returning state = %d", *pVal);

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_IWPCanEnable(BOOL *pEnable)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pEnable )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pEnable = VARIANT_FALSE;
    DWORD dwReturn = 0;

	if (g_OSInfo.IsAdminProcess())
	{
		if (m_pIWPSettings)
		{
			if ( SYM_FAILED (m_pIWPSettings->GetValue (IWP::IIWPSettings::IWPCanEnable, dwReturn)))
			{
				CCTRACEE ("CNAVOptions::get_IWPCanEnable - GetValue failed");
				MakeError (IDS_ERR_IWPCANENABLE, E_FAIL, IDS_NAVERROR_INTERNAL);
				return E_FAIL;
			}
			if (dwReturn)
			    *pEnable = VARIANT_TRUE;
		}
	}

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_IWPFirewallOn(BOOL *pOn)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pOn )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pOn = VARIANT_TRUE;
    DWORD dwReturn = 0;

    if (m_pIWPSettings)
    {
        if ( SYM_FAILED (m_pIWPSettings->GetValue (IWP::IIWPPrivateSettings::IWPFirewallOn, dwReturn)))
        {
            CCTRACEE ("CNAVOptions::get_IWPFirewallOn - GetValue failed");
            MakeError (IDS_ERR_IWPCANENABLE, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }

    *pOn = dwReturn ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_IWPUserWantsOn(BOOL *pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = VARIANT_FALSE;
    DWORD dwReturn = 0;

    if (m_pIWPSettings)
    {
        if ( SYM_FAILED (m_pIWPSettings->GetValue (IWP::IIWPSettings::IWPUserWantsOn, dwReturn)))
        {
            CCTRACEE ("CNAVOptions::get_IWPIWPUserWantsOn - GetValue failed");
            MakeError (IDS_ERR_GETUSERWANTSON, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }

    *pVal = dwReturn ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CNAVOptions::put_IWPUserWantsOn(BOOL bUserWantsOn)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if (m_pIWPSettings)
    {
        DWORD dwSetValue = 0;
        if( bUserWantsOn )
            dwSetValue = 1;

        if ( SYM_FAILED (m_pIWPSettings->SetValue (IWP::IIWPSettings::IWPUserWantsOn, dwSetValue)))
        {
            CCTRACEE ("CNAVOptions::put_IWPIWPUserWantsOn - SetValue failed");
            MakeError (IDS_ERR_PUTUSERWANTSON, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
        else
            m_bIWPDirty = true;
    }

    return S_OK;
}

bool CNAVOptions::InitSymProtect()
{
    // Initialize SymProtect if not already done
    if( m_pSymProtect == NULL )
    {
        // Set to off since we don't know
        m_bSymProtectEnable = false;

        // Allocate the object
        m_pSymProtect = new CSymProtectControlHelper;

        if( m_pSymProtect == NULL )
        {
            CCTRACEE(_T("CNAVOptions::InitSymProtect() - Failed to allocate the symprotect control helper"));
            return false;
        }

        if( !m_pSymProtect->Create() )
        {
            CCTRACEE(_T("CNAVOptions::InitSymProtect() - Failed to create() the symprotect control helper"));
            delete m_pSymProtect;
            m_pSymProtect = NULL;
            return false;
        }

        // Get the initial state of SymProtect
        bool bCurState = false;
        bool bError = false;
        int nTries = 0;
        if(m_pSymProtect->GetStatus(bCurState, bError))
        {
            if( bCurState == true )
            {
                m_bSymProtectEnable = true;
            }
        }
    }

    return true;
}

STDMETHODIMP CNAVOptions::get_SymProtectEnabled(BOOL *bEnable)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Make sure pointer is valid
    if( forceError ( ERR_INVALID_POINTER ) || NULL == bEnable )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // SymProtect initialized?
    if( !InitSymProtect() )
    {
        CCTRACEE(_T("CNAVOptions::get_SymProtectEnabled() - Failed to initialize SymProtect."));
        MakeError (IDS_ERR_INIT_SYMPROTECT, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    *bEnable = (m_bSymProtectEnable ? VARIANT_TRUE : VARIANT_FALSE);

    return S_OK;
}

STDMETHODIMP CNAVOptions::put_SymProtectEnabled(BOOL bEnable)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // SymProtect initialized?
    if( !InitSymProtect() )
    {
        CCTRACEE(_T("CNAVOptions::get_SymProtectEnabled() - Failed to initialize SymProtect."));
        MakeError (IDS_ERR_INIT_SYMPROTECT, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Set our internal enable to the state the user wants, we'll
    // do the actual work on the save when we see the dirty flag is set
    m_bSymProtectDirty = true;
    m_bSymProtectEnable = (bEnable ? true : false);

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AutoBlockEnabled(BOOL *pVal)
{
	if(!IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    
    if( m_pSNMgr == NULL )
    {
        // Attempting to gracefully error out when SymNeti.dll blows up on us
        // instead of crashing navoptions
        try
        {
            m_pSNMgr = new CSNManager;
        }
        catch(...)
        {
            CCTRACEE(_T("CNAVOptions::get_AutoBlockEnabled() - Exception attempt to create symneti manager class."));
        }

        if( m_pSNMgr == NULL )
        {
            MakeError(IDS_ERR_LOAD_SYMNETMGR, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }

    HRESULT hr = S_OK;
    bool bOn = false;
    if( FAILED(hr = m_pSNMgr->GetAutoBlockEnabled(bOn)) )
    {
        MakeError(IDS_ERR_GET_AUTOBLOCKENABLED, hr, IDS_NAVERROR_INTERNAL);
        return hr;
    }

    *pVal = bOn ? VARIANT_TRUE : VARIANT_FALSE;
    
	return S_OK;
}

STDMETHODIMP CNAVOptions::put_AutoBlockEnabled(BOOL newVal)
{
	if(!IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( m_pSNMgr == NULL )
    {
        // Attempting to gracefully error out when SymNeti.dll blows up on us
        // instead of crashing navoptions
        try
        {
            m_pSNMgr = new CSNManager;
        }
        catch(...)
        {
            CCTRACEE(_T("CNAVOptions::put_AutoBlockEnabled() - Exception attempt to create symneti manager class."));
        }

        if( m_pSNMgr == NULL )
        {
            MakeError(IDS_ERR_LOAD_SYMNETMGR, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }

    HRESULT hr = S_OK;
    if( FAILED(hr = m_pSNMgr->SetAutoBlockEnabled(newVal ? true : false)) )
    {
        MakeError(IDS_ERR_PUT_AUTOBLOCKENABLED, hr, IDS_NAVERROR_INTERNAL);
        return hr;
    }
    
	return S_OK;
}

STDMETHODIMP CNAVOptions::SetWorkingDirIWP(BOOL bIWPDir)
{
    if ( bIWPDir )
    {
        // Save current directory 
        //
        if ( '\0' == m_szCurrentWorkingDir[0] )
            GetCurrentDirectory ( MAX_PATH, m_szCurrentWorkingDir );

        // Get the NAV directory
        TCHAR szDir[MAX_PATH+1] = {0};
        _tcsncpy(szDir, g_NAVInfo.GetNAVDir(), MAX_PATH);

        // Append IWP if setting to the IWP directory
        _tcsncat(szDir, _T("\\IWP"), MAX_PATH - _tcslen(g_NAVInfo.GetNAVDir()));

        if( SetCurrentDirectory(szDir) )
        {
            CCTRACEI ("CNAVOptions::SetWorkingDirIWP - %s", szDir);
            return S_OK;
        }
        else
        {
            CCTRACEE ("CNAVOptions::SetWorkingDirIWP - failed to set working IWP dir - %s", szDir);
            return S_FALSE;
        }
    }

    // Reset old working dir
    //
    if ( '\0' != m_szCurrentWorkingDir[0] && SetCurrentDirectory (m_szCurrentWorkingDir))
    {
        CCTRACEI ("CNAVOptions::SetWorkingDirIWP - reset -  %s", m_szCurrentWorkingDir);
        return S_OK;
    }
    else
    {
        CCTRACEE ("CNAVOptions::SetWorkingDirIWP - failed to reset working dir - %s", m_szCurrentWorkingDir);
        return S_FALSE;
    }
}

STDMETHODIMP CNAVOptions::get_FWUIPropBag(VARIANT *pvValue)
{
    if(!IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Only create the property bag once
    if( pFWUIPropBag == NULL )
    {
        try
        {
            HRESULT hr = CoCreateInstance(CLSID_VarBstrCol, NULL, CLSCTX_INPROC_SERVER, IID_IVarBstrCol, (void**) &pFWUIPropBag);
            if( FAILED(hr) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to create the firewall property bag. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vProductDisplayName, _variant_t(m_csProductName)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag product display name. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vProductType, _variant_t(FWUI_PRODUCT_NAV)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag product type. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vProductVersion, _variant_t(NAV_RES_FILEVERSIONSTRING)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag product version. hr = 0x%X"), hr);
                throw hr;
            }
	        
            TCHAR szPath[MAX_PATH] = {0};
            _stprintf(szPath, _T("res://%s\\Navopts.dll/201"), g_NAVInfo.GetNAVDir());
            if( FAILED( hr = pFWUIPropBag->put_Item(vProductIconPath, _variant_t(szPath)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag product icon path. hr = 0x%X"), hr);
                throw hr;
            }
        	
        	if( FAILED( hr = pFWUIPropBag->put_Item(vProductHelpPath, _variant_t(m_Help.GetHelpPath())) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag product help path. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vRulesHelpID, _variant_t(NAV_IWP_customize_general_rules)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag RulesHelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vProgramRulesHelpID, _variant_t(NAV_IWP_customize_app_rules)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag ProgramRulesHelpID. hr = 0x%X"), hr);
                throw hr;
            }

			if( FAILED( hr = pFWUIPropBag->put_Item(vTrojanRulesHelpID, _variant_t(NAV_IWP_customize_trojan_rule)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag TrojanRulesHelpID. hr = 0x%X"), hr);
                throw hr;
            }

			if( FAILED( hr = pFWUIPropBag->put_Item(vRuleEditActionHelpID, _variant_t(IWP_IDH_RULE_ACTION)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

			if( FAILED( hr = pFWUIPropBag->put_Item(vRuleEditDirectionHelpID, _variant_t(IWP_IDH_RULE_CONNECTIONS)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vRuleEditIPHelpID, _variant_t(IWP_IDH_RULE_COMPUTERS)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vRuleEditPortsHelpID, _variant_t(FW_IDH_RULE_COMMUNICATIONS)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vRuleEditLocationsHelpID, _variant_t(FW_IDH_NIS_ADD_RULE_LOCATION)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vRuleEditLoggingHelpID, _variant_t(IWP_IDH_RULE_TRACKING)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vRuleEditTitleHelpID, _variant_t(IWP_IDH_RULE_DESCRIPTION)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vAdaptersHelpID, _variant_t(FW_IDH_NETWORK_ADAPTERS)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vICMPHelpID, _variant_t(FW_IDH_SPECIFY_ICMP_COMMANDS)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vIPExclusionHelpID, _variant_t(FW_IDH_Exclusions_TellMeMore_button)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vSignatureHelpID, _variant_t(FW_IDH_IDS_signature_exclusions)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag HelpID. hr = 0x%X"), hr);
                throw hr;
            }

            //////////////////// NAV Specific //////////////////////////
	        /// NAV should fill these properties.

            if( FAILED( hr = pFWUIPropBag->put_Item(vDisplayLocationPage, _variant_t(VARIANT_FALSE)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag display location page. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vDisplayMonitorOption, _variant_t(VARIANT_FALSE)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag display monitor option. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vDisplaySecurityAlertOption, _variant_t(VARIANT_FALSE)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag display security alert option. hr = 0x%X"), hr);
                throw hr;
            }

            if( FAILED( hr = pFWUIPropBag->put_Item(vDisplayAdaptersOption, _variant_t(VARIANT_FALSE)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag display adapters option. hr = 0x%X"), hr);
                throw hr;
            }
        	
        	if( FAILED( hr = pFWUIPropBag->put_Item(vDisplayConnectionsPage, _variant_t(VARIANT_FALSE)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag display connections page option. hr = 0x%X"), hr);
                throw hr;
            }

        	if( FAILED( hr = pFWUIPropBag->put_Item(vDisplayLightTrojanRules, _variant_t(VARIANT_TRUE)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag display light trojan rules. hr = 0x%X"), hr);
                throw hr;
            }

            // Can't use "Firewall" for the NAV IWP feature
            CAtlString strIWP;
			_Module.SetResourceInstance(g_ResModule);
            strIWP.LoadString(IDS_IWP_FEATURE_NAME);
			_Module.SetResourceInstance(_Module.GetModuleInstance());
            if( FAILED( hr = pFWUIPropBag->put_Item(vFirewallTitleName, _variant_t(strIWP)) ) )
            {
                CCTRACEE(_T("CNAVOptions::get_FWUIPropBag() - Failed to set the firewall property bag display firewall title name. hr = 0x%X"), hr);
                throw hr;
            }
        }
        catch(_com_error &e)
        {
            if( pFWUIPropBag != NULL )
            {
                pFWUIPropBag->Release();
                pFWUIPropBag = NULL;
            }

            // Need to MakeError here
            MakeError(IDS_ERR_FILL_FWUI_PROPERTYBAG, e.Error(), IDS_NAVERROR_INTERNAL);
            return e.Error();
        }
    }

    *pvValue = variant_t(pFWUIPropBag).Detach();

    return S_OK;
}

STDMETHODIMP CNAVOptions::SetUseBusinessRules(BOOL bUseBusinessRules)
{
    if(!IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	m_pNavOpts->UseBusinessRules(bUseBusinessRules);

	return S_OK;
}
