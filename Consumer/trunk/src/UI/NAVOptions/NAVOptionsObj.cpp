////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptionsObj.cpp : Implementation of CNAVOptions
#include "StdAfx.h"

#include "StahlSoft.h"

#ifndef _INIT_COSVERSIONINFO
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#endif

#include "Simon.h"
#include "NavOptionRefreshHelperInterface.h"

#include <algorithm>
#include "NAVOptions.h"
#include "ccWebWnd.h"
#include "NAVOptionsObj.h"
#include "ccModuleID.h"
#include "ccOSInfo.h"
#include "AVccModuleID.h"
#include "NAVTrust.h"
#include "globalevent.h"
#include "globalevents.h"
#include "ISVersion.h"
#include "isErrorResource.h"
#include "isErrorIDs.h"

#include "build.h" // For version number

#include "defutilsinterface.h"  // Defutils syminterface object
#include "NAVDefutilsLoader.h"

#include "NAVVer.h"
#include "SymHelp.h"
#include "..\navoptionsres\resource.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions
CNAVOptions::CNAVOptions(void)
           : m_bIsDirty(false)
           , m_bCanAccessRegKey(false)
           , m_OfficePlugin(this)
           , m_bIsPasswordDirty(false)
           , m_bExclusionsLoaded(false)
           , m_bExclusionsDirty(false)
           , m_bNavDefsInitialized(false)
           , m_MyHwnd(NULL)
           , m_dwMaxName(0)
           , m_dwMaxData(0)
           , m_bSymProtectEnable(false)
           , m_bSymProtectDirty(false)
           , m_pSymProtect(NULL)
           , m_eInitialPageID(ShowPageID_REAL_TIME_PROTECTION)
		   , m_bAPLoaded(false)
		   , m_bEmailIncomingEnabledAtStart(false)
		   , m_bEmailOutgoingEnabledAtStart(false)
           , m_bPasswordSupported(false)
           , m_bWin64(false)
           , m_bVista(false)
{

    // Regular options - send events! (true value for arg 2)
    m_pNavOpts = new CNAVOptSettingsCache(m_ccSettings,true);   
    // Default options - do not modify/don't send events 
    m_pNavDefs = new CNAVOptSettingsCache(m_ccSettings);

	// Initialize other properties array
	// Note: This must be updateded as properties
	//       are added or removed!
	m_Properties[0] = &m_OfficePlugin;

	CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

    // Check NAVError module for Symantec Signature...
    //
    if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
                                              NULL,
                                              CLSCTX_INPROC_SERVER)))
    {		
        m_spError = NULL;
    }

	m_csProductName = CISVersion::GetProductName();

	m_csTitle.LoadString(g_ResModule, IDS_Title);

    // Create a global mutex to synchronize loading, saving, and setting defaults
    // the options object
    TCHAR szMutexName[MAX_PATH] = _T("CNAVOptionsObj_{72ED3226-4C58-4290-93D4-7F15AE02203F}");
    m_shNavOptsObjMutex = StahlSoft::CreateMutexEx(NULL,FALSE,szMutexName,TRUE);
    m_NavOptsObjLockMutex.Attach(m_shNavOptsObjMutex);

    _tcscpy ( m_szCurrentWorkingDir, _T(""));

	if(ccLib::COSInfo::IsWinVista(true))
	{

		// Disable Password protection
		// because Vista is already protected by UAC.
		m_bPasswordSupported = false;
		CCTRCTXI0(L"Disable password protection for Vista");
	}
	else
	{
		// Enable Password protection for XP
		m_bPasswordSupported = true;
		CCTRCTXI0(L"Enable password protection for Vista");
	}
}

CNAVOptions::~CNAVOptions(void)
{
	// Free resources

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

	if(FAILED(UnloadAPExclusions()))
		CCTRCTXE0(_T("Error unloading AP exclusions!"));

	if(m_spSnoozeDlg)
		m_spSnoozeDlg.Release();

	if(m_APOptsMap)
		m_APOptsMap.Release();

	if(m_spAP)
		m_spAP.Release();

    if(m_spExclusionMgr)
    {
        CCTRACEI(_T("CNAVOptions::dtor - Releasing exclusion manager."));
        m_spExclusionMgr->uninitialize(false);
        m_spExclusionMgr.Release();

    }
    CCTRACEI(_T("CNAVOptions::dtor - Releasing exclusion vectors."));
    m_vFSExclusions.clear();
    m_vAnomalyExclusions.clear();

	if(m_spSkin)
		m_spSkin.Release();
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

	return Load();
}

void CNAVOptions::FinalRelease(void)
{
	CComObjectRootEx<CComSingleThreadModel>::FinalRelease();
}

/////////////////////////////////////////////////////////////////////////////
// CNAVOptions::CNAVDispatchImpl
HRESULT CNAVOptions::Put(LPCWCH pwcName, DISPPARAMS* pdispparams, UINT* puArgErr)
{
	long lType;
	HRESULT hr;
	CComVariant vArg;

	if(!pwcName || !puArgErr)
		return E_INVALIDARG;

	if(1 != pdispparams->cArgs)
		return DISP_E_BADPARAMCOUNT;


	CComBSTR bstrOption(pwcName);
	if(FAILED(hr = this->get__TypeOf(pwcName, &lType)))
		return hr;

	// Acquire variant parameter (i.e. the script variant with the value to set)
	if(FAILED(hr = ::DispGetParam(pdispparams, DISPID_PROPERTYPUT, 
						lType == ccSettings::SYM_SETTING_DWORD ? VT_UI4:VT_BSTR, 
						&vArg, puArgErr)))
	{
		CCTRCTXE1(_T("Error getting parameter from IDispatch params. HR = 0x%08x"), hr);
		return hr;
	}

	return this->Put(bstrOption, vArg);
}

HRESULT CNAVOptions::Get(LPCWCH pwcName, VARIANT* pvValue)
{
	CComBSTR bstrOption(pwcName);
	// Use empty variant for default
	return this->Get(bstrOption, CComVariant(), pvValue);
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

	// Initialize the defaults the first time
	if (!m_bNavDefsInitialized)
	{
		TCHAR szNAVDefs[MAX_PATH];

		// Path to the default options file
		::_tcscat(::_tcscpy(szNAVDefs, g_NAVInfo.GetNAVDir()), _T("\\NAVOPTS.DEF"));
		// Load the list w/values from the file
        if( !m_pNavDefs->Init(szNAVDefs, FALSE) )
		{
		    MakeError(IDS_Err_DefaultsFile, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
            return E_FAIL;
        }

        m_bNavDefsInitialized = true;
	}

    DWORD dwMaxDataSize = 0;
    DWORD dwMaxNameSize = 0;
    m_pNavDefs->GetMaxNavSettingsSizes(dwMaxNameSize, dwMaxDataSize);
    
	if (FAILED(hr = get__TypeOf(pwcName, &eType)))
		return hr;

	//option name for checking if its an e-mail option
	CStringW wcsOptName, wcsOptGroup;
    std::vector<WCHAR> szValue;
    szValue.reserve (dwMaxDataSize);

    std::vector<WCHAR> szValueCurrent;
    szValue.reserve (m_dwMaxData);

	if(FAILED(hr = SplitOptionName(pwcName, wcsOptName, wcsOptGroup)))
	{
		CCTRCTXE1(_T("SplitOptionName failed! HR = 0x%08x"), hr);
		return hr;
	}

    // Normalize the default
	switch(eType)
	{
	case ccSettings::SYM_SETTING_STRING:
		// Get the stored value
		if (!forceError (IDS_Err_Default) &&
            SUCCEEDED(m_pNavDefs->GetValue(pwcName, &szValue[0], dwMaxDataSize, _T("")))
           )
		{
			//Get current setting and check to see if it is the same.
			if (FAILED(m_pNavOpts->GetValue(pwcName, &szValueCurrent[0], m_dwMaxData, _T("")))
				|| lstrcmpi(&szValue[0], &szValueCurrent[0]) != 0)
			{
				//Couldn't get setting or setting is different, save it
				if (forceError (IDS_Err_Put) ||
                    FAILED(m_pNavOpts->SetValue(pwcName, &szValue[0])) )
				{
					//Error setting option, return error.
					MakeError( IDS_Err_Put, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );
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
			MakeError( IDS_Err_Default, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );
			return E_FAIL;
		}

		return S_OK;

	case ccSettings::SYM_SETTING_DWORD:
		if( wcsOptGroup.Compare(_T("NAVPROXY")) != 0) // Not a special-case option; pass-through to ccSettings
		{
			// Get the stored value
			if (!forceError (IDS_Err_Default) &&
                SUCCEEDED(m_pNavDefs->GetValue(pwcName,dwValue,0)))
            {
				if( wcsOptGroup.Compare(_T("AUTOPROTECT")) == 0 )
				{
					wcsOptName = _T("AP_Opt_") + wcsOptName; // Hack warning! Prepend "AP_Opt_".
					if(FAILED(hr = m_APOptsMap->SetValue(wcsOptName, dwValue)))
					{
						CCTRCTXE3(_T("Error defaulting AUTOPROTECT:%s to %d. HR = 0x%08x"), wcsOptName, dwValue, hr);
						return hr;
					}
				}
				else
				{	// Non-special-case setting - Use ccSettings
					//Get current setting and check to see if it is the same.
					if (FAILED(m_pNavOpts->GetValue(pwcName,dwValueCurrent,0))
						|| dwValue != dwValueCurrent)
					{
						//Couldn't get setting or setting is different, save it
						if (forceError (IDS_Err_Put) ||
							FAILED(m_pNavOpts->SetValue(pwcName, dwValue)))
						{
							//Error setting option, return error.
							MakeError( IDS_Err_Put, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );
							return E_FAIL;
						}
						else
						{
						//Setting successfully changed, set the dirty flag.
						m_bIsDirty = true;
						}
					}
				}
			}
			else
			{
				//Error getting value from default file, return error.
				MakeError( IDS_Err_Default, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );
				return E_FAIL;
			}
        }
		else
		{   // Email options must be defaulted by the SetEmailOptionsDefaults() call.
			// This is here for compatibility only.
			if(FAILED(m_EmailOptions.Default()))
				CCTRCTXW0(_T("Error setting email options to defaults."));
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
    ccLib::COSInfo osInfo;
    SYMRESULT sr;

    m_bVista = osInfo.IsWinVista(true);
    m_bWin64 = osInfo.IsWin64();

    CCTRACEI(_T("CNAVOptions::Load() called"));
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
	StahlSoft::CSmartLock smLock(&m_NavOptsObjLockMutex);
	//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

	bool bWarnedDefaults = false; // Only want to warn about defaults once per load

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
        CCTRACEE(_T("CNAVOptions::Load() Failed to load the option settings, attempting to restore defaults then load."));

        // First attempt to restore from the backup hive. If this succeeds we don't need to warn the
        // user because this is the last set of options they saved
		/* -- This does not make any sense since NAV options moved to ccSettings:
		 * -- if NAVOPTS.DAT is corrupted, NAVOPTS.BAK will be corrupted is well.

        CNAVOptSettingsCache NavOptBak;
        if( !NavOptBak.Init("NAVOPTS.BAK", FALSE) || !m_pNavOpts->Migrate(NavOptBak) )
		*/
        {

            // Attempt to restore from the defaults hive. From here on out the user needs to be warned
            // 
            if (!m_bNavDefsInitialized)
	        {
		        // Use the default options file name in order to initialize
                // the default ccSettings NAV hive
                if( !m_pNavDefs->Init(_T("NAVOPTS.DEF"), FALSE) )
		        {
                    CCTRACEE(_T("CNAVOptions::Load() - Failed to initialize the navopts.def ccSetting hive."));
                }
                else
                    m_bNavDefsInitialized = true;
	        }

            if( !m_bNavDefsInitialized || !m_pNavOpts->Migrate(*m_pNavDefs) )
            {
                CCTRCTXE0(_T("CNAVOptions::Load() - Failed to restore from the default settings hive."));
            }
            else
                bWarnedDefaults = true;
        }

        //
        // Now we have restored and loaded the options
        //

        // Warn about the defaults if we had to use the default file
        if( bWarnedDefaults )
        {
            CString csFormat, csMsg;

            csFormat.LoadString(IDS_Wrn_Default);
            csMsg = csFormat;
            csMsg.Format(csFormat, m_csProductName);

            // Warn the user that the defaults were restored.
            ::MessageBox(::GetActiveWindow(), csMsg, m_csTitle, MB_OK | MB_ICONEXCLAMATION);
        }

        // Notify everyone that the options may have changed.
		notify();
    }

    // Save the maximum value for setting names and data
    m_pNavOpts->GetMaxNavSettingsSizes(m_dwMaxName, m_dwMaxData);

	// Try to initialize snooze
	if(SYM_FAILED(sr = m_SnoozeDlgLoader.CreateObject(&m_spSnoozeDlg)))
	{
		CCTRCTXE1(_T("Error initializing Snooze. SR = 0x%08x"), sr);
		return E_FAIL;
	}

    // Remove all knowledge of options types
    m_OptionsTypes.clear();
	_NAVSETTINGSMAP *pMap = NULL;
	m_pNavOpts->GetSettingsMap(&pMap);
	if(!pMap || !EnumerateSettings(pMap))
	{
		m_NavOptsObjLockMutex.Unlock();
		MakeError(IDS_Err_Load, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		m_spError->LogAndDisplay(0);
		CCTRCTXE0(_T("Error while enumerating the NAV Options settings type map."));
		return E_FAIL;
	}
    
	// Force the custom properties to re-load
	for (int i = 0; i < SIZEOF(m_Properties); i++)
		m_Properties[i]->Init();

    //
    // Add the email options to the options names list
    //
    pair<OPTYPEMAP::iterator, bool> pr;

    // Add the Tray Animation as "NAVPROXY:ShowTrayIcon" so the script doesn't have to change
    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(L"NAVPROXY:ShowTrayIcon", ccSettings::SYM_SETTING_DWORD));
	_ASSERT(pr.second);  // Validate that such value didn't exist
	
    // Add the Outgoing Progress as "NAVPROXY:ShowProgressOut" so the script doesn't have to change
    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(L"NAVPROXY:ShowProgressOut", ccSettings::SYM_SETTING_DWORD));
	_ASSERT(pr.second);  // Validate that such value didn't exist

    // Add the TimeOutProtection as "NAVPROXY:TimeOutProtection" so the script doesn't have to change
    pr = m_OptionsTypes.insert(OPTYPEMAP::value_type(L"NAVPROXY:TimeOutProtection", ccSettings::SYM_SETTING_DWORD));
	_ASSERT(pr.second);  // Validate that such value didn't exist

	// Load AP
	if(FAILED(this->LoadAP()))
	{
		CCTRCTXE0(_T("Error loading AP."));
		return E_FAIL;
	}

	// Load Email options for snoozing
	DWORD dwVal;
	if(FAILED(m_EmailOptions.Initialize()))
	{
		CCTRCTXE0(_T("Error loading Email Options."));
	}

	if(FAILED(m_EmailOptions.GetScanIncoming(dwVal)))
		CCTRCTXW0(_T("Error getting IAVEmail:ScanIncoming."));
	m_bEmailIncomingEnabledAtStart = dwVal ? true:false;
	if(FAILED(m_EmailOptions.GetScanOutgoing(dwVal)))
		CCTRCTXW0(_T("Error getting IAVEmail:ScanOutgoing."));
	m_bEmailOutgoingEnabledAtStart = dwVal ? true:false;

	return S_OK;
}

STDMETHODIMP CNAVOptions::Save(void)
{
	CCTRACEI(_T("CNAVOptions::Save()"));

    // Make sure that it is used by an authorized Symantec application
	if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
	StahlSoft::CSmartLock smLock(&m_NavOptsObjLockMutex);
	//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

	HRESULT hr;
	bool bOptionsIsDirty = m_bIsDirty;
	BOOL bExclusionsIsDirty = FALSE;

	if(FAILED(hr = ProcessSnoozeables()))
	{
		CCTRCTXE1(_T("ProcessSnoozeables failed. HR = 0x%08x"), hr);
	}

	// Flush the regular navopts.dat options if dirty
	if (bOptionsIsDirty)
	{
		// Save the options that are stored in the options file.
        if( !m_pNavOpts->Save() )
        {
            CCTRACEE(_T("CNAVOptions::Save() Failed to save the option settings, returning error."));
            MakeError(IDS_Err_Save, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
            m_NavOptsObjLockMutex.Unlock();
	        return E_FAIL;
        }
	}

    // Save any dirty custom property options
    for (int i = 0; i < SIZEOF(m_Properties); i++)
	{
        if (m_Properties[i]->IsDirty())
        {
			if (FAILED(hr = m_Properties[i]->Save()))
            {
                CCTRACEE(_T("CNAVOptions::Save() Failed to save custom property %d, moving to next property."), i);
				continue;
            }

            // Set the dirty flag
            bOptionsIsDirty = true;
        }
	}

	// Save AP
	if(m_bAPLoaded)
	{
		// Commit AP enabled setting and AP options
		if(m_bAPEnabled != m_bAPEnabledAtStart)
		{
			if(FAILED(hr = m_spAP->EnableAP(m_bAPEnabled)))
				CCTRCTXE2(_T("Error setting AP to %d! HR = 0x%08x"), m_bAPEnabled, hr);
		}
		else
			CCTRCTXI0(_T("Skipping AP enable - user has not changed AP state."));

		if(FAILED(hr = m_spAP->SetOptions(m_APOptsMap)))
			CCTRCTXE1(_T("Error committing AP options map. HR = 0x%08x"), hr);
		if(m_spAPExclusions)
		{
			// Commit AP Extensions
			if(FAILED(hr = this->SaveExtensions()))
				CCTRCTXW1(_T("Error while sending AP extensions changes to AP. HR = 0x%08x"), hr);

			if(FAILED(hr = m_spAPExclusions->Dirty()))
				CCTRCTXE1(_T("Error checking whether AP Exclusions are dirty. HR = 0x%08x"), hr);
			if(hr == S_OK)
			{
				if(FAILED(hr = m_spAPExclusions->Commit()))
					CCTRCTXE1(_T("Error commiting AP exclusions. HR = 0x%08x"), hr);
			}
			else
				CCTRCTXI0(_T("Skipping AP Exclusions - not dirty."));
		}
	}

	// Save ALU state
	if(m_ALUWrapper.Dirty() == S_OK)
	{
		if(FAILED(hr = m_ALUWrapper.Enable()))
		{
			CCTRCTXE1(_T("Failed to enable ALU. HR = 0x%08x"), hr);
			MakeError(IDS_Err_ALUEnable, hr, IDS_NAVOPTS_ERR_ALUEnable);
			m_spError->LogAndDisplay(0);
		}
	}

    // Save the advanced Email options
    if( FAILED(hr = m_EmailOptions.Save()) || forceError( IDS_Err_Set_EmailOption ) )
    {
        CCTRCTXE1(_T("Failed to save Email Options. HR=0x%08x"),hr);
        MakeError(IDS_Err_Set_EmailOption,hr,IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        m_spError->LogAndDisplay(0);
    }

    // Save the threat exclusions
    if( m_bExclusionsDirty )
    {
        if( forceError(IDS_ERR_SAVE_THREAT_EXCLUSIONS) || 
            m_spExclusionMgr->saveState() != AVExclusions::Success)
        {
            CCTRACEE(_T("Failed to save exclusions"));
            MakeError(IDS_ERR_SAVE_THREAT_EXCLUSIONS, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
            m_spError->LogAndDisplay(0);
        }
        else
        {
            m_bExclusionsDirty = false;
            bExclusionsIsDirty = TRUE;
        }
    }

    // Save the IWP settings
	if(m_IWPOptions.IsDirty())
	{
		if( FAILED(hr = m_IWPOptions.Save()) )
		{
			CCTRCTXE1(_T("Error while saving IWP Options. HR = 0x%08x"), hr);
		}
	}

    // Save the IM Options
    if( FAILED(hr = m_IMOptions.Save()) )
    {
        CCTRCTXE1(_T("Error while saving IM Options. HR = 0x%08x"), hr);
    }

	// Save the Threat Category options
	if(FAILED(hr = m_ThreatCatOptions.Save()))
		CCTRCTXE1(_T("Error while saving Threat Category options. HR = 0x%08x"), hr);

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
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::Get(/*[in]*/ BSTR bstrOption, /*[in]*/ VARIANT vDefault, /*[out, retval]*/ VARIANT *pvValue)
{
	USES_CONVERSION;
	HRESULT hr = E_FAIL;
	long eType = 0;
	CStringW wcsOption;

	if (forceError (ERR_INVALID_ARG) ||
		!bstrOption || !pvValue)
	{
		MakeError(ERR_INVALID_ARG, E_INVALIDARG, IDS_ISSHARED_ERROR_INTERNAL);
		return E_INVALIDARG;
	}

	::VariantInit(pvValue);
	wcsOption = bstrOption;

	//option name for checking if it's a special-case option
	CStringW wcsOptName, wcsGroupName;
	ATL::CStringW strValue, strDefault;

	if(FAILED(hr = SplitOptionName(wcsOption, wcsOptName, wcsGroupName)))
	{
		CCTRCTXE1(_T("SplitOptionName failed! HR = 0x%08x"), hr);
		return hr;
	}

	if(wcsGroupName.Compare(_T("AUTOPROTECT")) == 0)
	{
		// Prepend "AP_Opt_" because AP options are in that format.
		wcsOptName = _T("AP_Opt_") + wcsOptName; // Hack warning!

		// Load AP if necessary
		if(!m_bAPLoaded && FAILED(hr = this->LoadAP()))
		{
			CCTRCTXE1(_T("Failed to load AP. Assuming 0. HR = 0x%08x"), hr);
			pvValue->ulVal = 0;
			return S_OK;
		}
		else
		{
			// Get AP value from map.
			DWORD dwValue = 0;
			if(FAILED(hr = m_APOptsMap->GetValue(wcsOptName, dwValue)))
			{
				CCTRCTXW1(_T("Error getting AP option. HR = 0x%08x"), hr);
				//If cannot get value from AVModule, do not fail, try CCSettings
			}
			else
			{
				pvValue->vt = VT_UI4;
				pvValue->ulVal = dwValue;
				return S_OK;
			}
		}
	}

	if (FAILED(hr = get__TypeOf(wcsOption, &eType)))
	{
		CCTRCTXE1(_T("Error in get__TypeOf, hr = 0x%08x"),hr);
		return hr;
	}



	switch(eType)
	{
		{
	case ccSettings::SYM_SETTING_STRING:
		// Coerce default to string type
		if(FAILED(hr = ::VariantChangeType(&vDefault, &vDefault, 0U, VT_BSTR)))
			return hr;

		if( ::SysStringLen(vDefault.bstrVal) > m_dwMaxData )
			return E_POINTER;

		strDefault = vDefault.bstrVal;

		// Get the stored value
		if(FAILED(m_pNavOpts->GetValue(wcsOption
			, strValue.GetBuffer (m_dwMaxData)
			, m_dwMaxData
			, strDefault)) )
		{
			strValue.ReleaseBuffer();

			// Insert non existing values into the options file
			if (SUCCEEDED(m_pNavOpts->SetValue(wcsOption, strValue)))
			{
				CCTRACEI(_T("CNAVOptions::Get() - Failed to get option %s, added it with value %s"), wcsOption, strValue);
				m_bIsDirty = true;
				// Update the max sizes since they may have increased with the new Set
				m_pNavOpts->GetMaxNavSettingsSizes(m_dwMaxName, m_dwMaxData);
			}
			else
			{
				MakeError(IDS_Err_Get, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
				return E_FAIL;
			}
		}

		// Convert the value to a VARIANT
		CComVariant v (strValue);
		v.Detach (pvValue); // copy to pvValue
		return S_OK;
		}// end SYM_SETTING_STRING

	case ccSettings::SYM_SETTING_DWORD:
		{
			DWORD bVal;
			// Check if it's an Email option
			if( wcsGroupName.Compare(_T("NAVPROXY")) == 0 )
			{
				if( forceError( IDS_Err_Get_EmailOption ) )
				{
					MakeError(IDS_Err_Get_EmailOption,E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
					return E_FAIL;
				}

				if( wcsOptName.Compare(A2W(CCEMLPXY_TRAY_ANIMATION)) == 0 )
					hr = m_EmailOptions.GetShowTrayIcon(bVal);
				else if( wcsOptName.Compare(A2W(CCEMLPXY_OUTGOING_PROGRESS)) == 0 )
					hr = m_EmailOptions.GetShowProgressOut(bVal);
				else if( wcsOptName.Compare(A2W(CCEMLPXY_TIMEOUT_PROTECTION)) == 0 )
					hr = m_EmailOptions.GetTimeoutProtection(bVal);
				else // Unknown email option name...
					hr = E_FAIL;

				if(FAILED(hr))
				{
					MakeError(IDS_Err_Get_EmailOption,E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
					return hr;
				}

				pvValue->ulVal = bVal;
			}
			else
			{
				// Coerce default value to DWORD
				if(FAILED(hr = ::VariantChangeType(&vDefault, &vDefault, 0U, VT_UI4)))
					return hr;

				if ( forceError ( IDS_Err_Get ))
				{
					MakeError(IDS_Err_Get, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
					return E_FAIL;
				}

				// Get the stored value
				if( FAILED(m_pNavOpts->GetValue(wcsOption, pvValue->ulVal, vDefault.ulVal)) )
				{
					// Insert non existing values into the options file
					if (SUCCEEDED(m_pNavOpts->SetValue(wcsOption, pvValue->ulVal)))
					{
						CCTRACEI(_T("CNAVOptions::Get() - Failed to get option %s, added it with value %d"), wcsOption, pvValue->ulVal);
						m_bIsDirty = true;
					}
					else
					{
						MakeError(IDS_Err_Get, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
						return E_FAIL;
					}
				}
			}

			pvValue->vt = VT_UI4;
			return S_OK;
		}

	default:  // Unknown option data type
		CCTRCTXE0(_T("Terminating - Unknown data type"));
		return DISP_E_TYPEMISMATCH;
	}        

	return hr;
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

    CComVariant vTempValue (vValue);

	//option name for checking if its an e-mail option
	CStringW wcsOptName, wcsGroupName;
    ATL::CStringW strValue;

	if(FAILED(hr = SplitOptionName(bstrOption, wcsOptName, wcsGroupName)))
	{
		CCTRCTXE1(_T("Error calling SplitOptionName. HR = 0x%08x"), hr);
		return hr;
	}

	switch(eType)
	{
	case ccSettings::SYM_SETTING_STRING:  // String
		// Coerce from any other type to a BSTR (as needed)
		if (FAILED(hr = ::VariantChangeType(&vValue, &vValue, 0U, VT_BSTR)))
			return hr;

        if ( forceError (IDS_Err_Get))
        {
			MakeError(IDS_Err_Get, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
			return E_FAIL;
        }

		// Compare to the old value to see if anything changed
		hrGet = m_pNavOpts->GetValue(OLE2W(bstrOption), strValue.GetBuffer(m_dwMaxData), m_dwMaxData, L"");
        strValue.ReleaseBuffer();

		if (strValue != vTempValue || FAILED(hrGet))
		{
			m_bIsDirty = true;
			if ( forceError (IDS_Err_Put) ||
                    FAILED(m_pNavOpts->SetValue(OLE2W(bstrOption), OLE2W(vTempValue.bstrVal))) )
			{
                MakeError(IDS_Err_Put, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
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
        if( wcsGroupName.Compare(_T("NAVPROXY")) == 0 )
        {
            bool bVal = false;

			// Get the value from the options page
            if( 0 == vValue.ulVal )
                bVal = false;
            else
                bVal = true;

			// Compare to the old value to see if it changed
			if( wcsOptName.Compare(A2W(CCEMLPXY_TRAY_ANIMATION)) == 0 )
                hr = m_EmailOptions.SetShowTrayIcon(bVal);
			else if( wcsOptName.Compare(A2W(CCEMLPXY_OUTGOING_PROGRESS)) == 0 )
				hr = m_EmailOptions.SetShowProgressOut(bVal);
			else if( wcsOptName.Compare(A2W(CCEMLPXY_TIMEOUT_PROTECTION)) == 0 )
                hr = m_EmailOptions.SetTimeoutProtection(bVal);
			else // Unknown email option name...
                hr = E_FAIL;

            if(FAILED(hr))
            {
                MakeError(IDS_Err_Set_EmailOption,E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
                return hr;
            }
        }
		else if( wcsGroupName.Compare(_T("AUTOPROTECT")) == 0 )
		{
			wcsOptName = _T("AP_Opt_") + wcsOptName; // Hack warning! Prepend "AP_Opt_".

			if(FAILED(hr = m_APOptsMap->SetValue(wcsOptName, vValue.ulVal)))
			{
				CCTRCTXE3(_T("Failed to set AUTOPROTECT:%s to %d. HR = 0x%08x"), wcsOptName, vValue.ulVal, hr);
				return hr;
			}
		}
        else // Not an email option
        {
			if (forceError ( IDS_Err_Get ))
            {
			    MakeError(IDS_Err_Get, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
				return E_FAIL;
            }

            // Compare to the old value to see if anything changed
            hrGet = m_pNavOpts->GetValue(OLE2W(bstrOption), dwOldValue, dwOldValue);

		    // If this is a new or changed value, set it
			if (dwOldValue != vValue.ulVal || FAILED(hrGet))
			{
				m_bIsDirty = true;
				if ( FAILED(m_pNavOpts->SetValue(OLE2W(bstrOption), vValue.ulVal)) )
				{
                    MakeError(IDS_Err_Put, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
				    return E_FAIL;
                }
			}
        }
		break;

	default:  // Unknown option data type
		return DISP_E_TYPEMISMATCH;
	}
	return S_OK;
}

STDMETHODIMP CNAVOptions::Default(void)
{
    CCTRACEI(_T("CNAVOptions::Default() - Begin Defaults"));
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
	StahlSoft::CSmartLock smLock(&m_NavOptsObjLockMutex);
	//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

	HRESULT hr;

    CCTRACEI(_T("CNAVOptions::Default() - Calling is it safe"));

	// Make sure that it is used by an authorized Symantec application
	if ( forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    CCTRACEI(_T("CNAVOptions::Default() - It is safe!"));

    //
	// Restore default options
    //

    // Check and store Delay Load setting if it's locked.
    // We don't want to revert AP Delay Load if we've locked it!
    DWORD dwDelayLoadLock = 0;
    DWORD dwSystemStart = 0;
    if(!SUCCEEDED(m_pNavOpts->GetValue(NAVAP_DelayLoadLock, dwDelayLoadLock,0)))
    {
        CCTRCTXW0(_T("Could not read Delay Load Lock setting. Assuming unlocked."));
    }
    else if(dwDelayLoadLock)
    {
        // Store off Delay Load setting, defaulting to off
        if(!SUCCEEDED(m_pNavOpts->GetValue(AUTOPROTECT_DriversSystemStart, dwSystemStart, 1)))
        {
            CCTRCTXW0(_T("Could not read Delay Load setting. Assuming 0."));
        }
    }
    CCTRCTXI1(_T("DelayLoadLock set to %d"), dwDelayLoadLock);
    CCTRCTXI1(_T("SystemStart set to %d"),dwSystemStart);

    // First attempt to restore from the default hive, if that fails
    // restore from the file

    // Initialize the defaults if they have not been initialized yet
	if (!m_bNavDefsInitialized)
	{
        CCTRACEI(_T("CNAVOptions::Default() - Going to initialize the defaults settings hive"));
		// Use the default options file name in order to initialize
        // the default ccSettings NAV hive
        if( !m_pNavDefs->Init(_T("NAVOPTS.DEF"), FALSE) )
		{
            CCTRACEE(_T("CNAVOptions::Default() - Failed to initialize the navopts.def ccSetting hive. Using the file to restore defaults."));
        }
        else
            m_bNavDefsInitialized = true;
        CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: initializing defaults settings hive"));
	}

    CCTRACEI(_T("CNAVOptions::Default() - Migrating from the defaults settings hive to the regular options hive"));
    // Send in a true as the second parameter to migrate so the DRM keys are not restored
    if( !m_bNavDefsInitialized || !m_pNavOpts->Migrate(*m_pNavDefs, true) )
    {
        CCTRCTXE0(_T("CNAVOptions::Default() - Could not restore from the default ccSettings hive."));
    }
    CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: Migrating from the defaults settings hive to the regular options hive"));

	// Reset each of the custom properties to default
	for (int i = 0; i < SIZEOF(m_Properties); i++)
	{
        CCTRACEI(_T("CNAVOptions::Default() - Reseting custom property %d"), i);
		if (FAILED(hr = m_Properties[i]->Default())
		 || FAILED(hr = m_Properties[i]->Save()))
        {
            CCTRACEE(_T("CNAVOptions::Default() - Failed to restore defaults for custom property: %d"), i);
        }

		m_Properties[i]->Init();
        CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: Reseting custom property %d"), i);
	}

	// Restore AP Defaults
	CCTRCTXI0(_T("Setting AutoProtect Defaults."));
	if(FAILED(hr = this->SetAPDefaults()))
	{
		CCTRCTXW1(_T("Error calling SetAPDefaults. HR = 0x%08x"), hr);
	}

    // Restore AP Delay Load if necessary
    if(dwDelayLoadLock)
    {
        CCTRCTXI0(_T("Lock detected. Restoring System Start state."));
        // Restore System Start setting
		if(FAILED(hr = m_APOptsMap->SetValue(AVModule::AutoProtect::APOPTION_SYSTEMSTART_DRIVERS, dwSystemStart)))
		{
			CCTRCTXE1(_T("Error setting SystemStart in AP options map. HR = 0x%08x"), hr);
		}

        // Restore lock
        if(!SUCCEEDED(m_pNavOpts->SetValue(NAVAP_DelayLoadLock, dwDelayLoadLock)))
        {
            CCTRCTXW1(_T("Error while setting DelayLoadLock to %d"), dwDelayLoadLock);
        }

        if(!m_pNavOpts->Save())
        {
            CCTRCTXW0(_T("Unknown error while saving DelayLoad settings changes."));
        }
    }

	CCTRCTXI0(_T("Restoring AutoProtect Exclusions defaults."));
	if(FAILED(hr = this->SetAPExclusionDefaults()))
	{
		CCTRCTXE1(_T("Error setting AP exclusion defaults. HR = 0x%08x"), hr);
	}
	CCTRCTXI0(_T("COMPLETED: Restoring AutoProtect Exclusions defaults."));

	// Commit AP enabled setting and AP options
	if(FAILED(hr = m_spAP->EnableAP(m_bAPEnabled)))
		CCTRCTXE2(_T("Error setting AP to %d! HR = 0x%08x"), m_bAPEnabled, hr);
	if(FAILED(hr = m_spAP->SetOptions(m_APOptsMap)))
		CCTRCTXE1(_T("Error committing AP options map. HR = 0x%08x"), hr);

	// Commit AP Extensions
	if(FAILED(hr = this->SaveExtensions()))
		CCTRCTXW1(_T("Error while sending AP extensions changes to AP. HR = 0x%08x"), hr);
	if(FAILED(hr = m_spAPExclusions->Commit()))
		CCTRCTXE1(_T("Error commiting AP exclusions. HR = 0x%08x"), hr);
	CCTRCTXI0(_T("COMPLETED: Restoring AutoProtect defaults"));

    CCTRACEI(_T("CNAVOptions::Default() - Restoring CCemailPxy defaults"));
    // Restore default Advanced Email options
    if(FAILED(m_EmailOptions.Default()))
    {
        CCTRCTXW0(_T("Error while defaulting Email Options."));
    }
    else if(FAILED(m_EmailOptions.Save()))
    {
        CCTRCTXW0(_T("Error while saving Email Options."));
    }

    CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: Restoring CCemailPxy defaults"));

    CCTRACEI(_T("CNAVOptions::Default() - Restoring threat exclusions defaults"));
    // Initialize exclusions, if needed
    if(!this->m_spExclusionMgr)
    {
        this->LoadExclusions(true);
    }

	// Get path of default exclusions file
	ccLib::CString defaultExcluFile;
	if( !ccSym::CInstalledApps::GetInstAppsDirectory(_T("AntiVirus"), defaultExcluFile) )
	{
		CCTRACEE(_T("CNAVOptions::Default() - Error, could not find CAV InstalledApps Director"));
		MakeError(IDS_ERR_SET_THREAT_DEFAULTS, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        m_spError->LogAndDisplay(0);
	}
	ccLib::CStringConvert::AppendPath(defaultExcluFile, EXCLUSIONS_DEFAULTS_FILE);

	if(this->m_spExclusionMgr)
    {
        // Unfiltered reset.
        if(m_spExclusionMgr->loadFromFile(
						&defaultExcluFile,
						(AVExclusions::IExclusion::ExclusionState)0,
						(AVExclusions::IExclusion::ExclusionType)0) 
					!= AVExclusions::Success)
        {
            CCTRACEE(_T("CNAVOptions::Default() - Error while loading exclusion defaults."));
            MakeError(IDS_ERR_SET_THREAT_DEFAULTS, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
            m_spError->LogAndDisplay(0);
        }

        // Forcing a save. It appears that everything else in here forces a save
        // to its state, so we have to as well.
        if(m_spExclusionMgr->saveState() != AVExclusions::Success)
        {
            CCTRACEE(_T("CNAVOptions::Default() - Error while saving exclusion state."));
            MakeError(IDS_ERR_SET_THREAT_DEFAULTS, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
            m_spError->LogAndDisplay(0);
        }
    }
    CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: Restoring threat exclusions defaults"));

    CCTRACEI(_T("CNAVOptions::Default() - Restoring SymProtect default"));
    // Restore the symprotect state to on
    if( /* SYMPROTECT64BITSUPPORT */ !m_bWin64 && InitSymProtect() )   
    {   // Remove the !m_bWin64 bit once we have 64bit SPBBC
        if( !m_pSymProtect->SetComponentEnabled(true) )
        {
            CCTRACEE(_T("CNAVOptions::Default() - Failed to enable symprotect"));
        }
    }
    else
        CCTRACEE(_T("CNAVOptions::Default() - Failed to initialize symprotect or skipping for Win64")); // SYMPROTECT64BITSUPPORT - fix this trace once we have 64 bit SPBCC
    CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: Restoring SymProtect default"));

    CCTRACEI(_T("CNAVOptions::Default() - Restoring IWP defualt"));
    // Restore the IWP state to on
    if( FAILED(hr = m_IWPOptions.Default()) )
    {
        CCTRCTXE1(_T("Error calling IWPOptions::Default(). HR = 0x%08x"), hr);
    }
	else if( FAILED(hr = m_IWPOptions.Save()) )
	{
		CCTRCTXE1(_T("Error saving IWP Defaults. HR = 0x%08x"), hr);
	}
    CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: Restoring IWP default"));

    CCTRCTXI0(_T("Restoring Threat Category Exclusion defaults"));
    if(FAILED(hr = m_ThreatCatOptions.Default()))
    {
        CCTRCTXE1(_T("Error calling ThreatCatOptions::Default(). HR = 0x%08x"), hr);
    }
    CCTRCTXI0(_T("Completed restoring Threat Category Exclusion defaults"));

    // Restore the IM Options to Default
    CCTRCTXI0(_T("Restoring IM Options to default"));
    if(FAILED(hr = m_IMOptions.Default()))
    {
        CCTRCTXE1(_T("Error calling IMOptions::Default. HR = 0x%08x"), hr);
    }
    CCTRCTXI0(_T("Completed restoring IM Options to default."));

	// Restore ALU options to default
	CCTRCTXI0(_T("Restoring ALU options to default"));
	if(FAILED(hr = this->DefaultLiveUpdate()))
	{
		CCTRCTXE1(_T("Error calling DefaultLiveUpdate(). Hr = 0x%08x"), hr);
	}
	if(FAILED(hr = m_ALUWrapper.Enable()))
	{
		CCTRCTXE1(_T("Error saving ALU state. HR = 0x%08x"), hr);
	}
	CCTRCTXI0(_T("Completed restoring ALU options to default."));

	// Tell everyone that the options changes so they have a chance to reload the file
    m_NavOptsObjLockMutex.Unlock();

    CCTRACEI(_T("CNAVOptions::Default() - notifying..."));
	notify();
    CCTRACEI(_T("CNAVOptions::Default() - COMPLETED: notifying..."));

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

    // Advanced email options?
    if( m_EmailOptions.IsDirty() )
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
	if(m_bPasswordSupported)
	{
		if( m_bIsPasswordDirty )
		{
			*pbIsDirty = VARIANT_TRUE;
			return S_OK;
		}
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::put_IsDirty( BOOL bIsDirty)
{
    // Make sure the caller is Symantec signed
    if ( forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
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
    MakeError(IDS_Err_ExclusionsInit, E_NOTIMPL, IDS_ISSHARED_ERROR_INTERNAL);
	return E_NOTIMPL;
}

STDMETHODIMP CNAVOptions::get_IsIMInstalled(/*[in]*/EIMType IMType, /*[out, retval]*/BOOL *pVal)
{
    HRESULT hr;
    bool bInstalled;

    // Intialize return value to not installed
    *pVal = VARIANT_FALSE;

    // Convert EIMType enumeration to IMTYPE enumeration
    ISShared::IIMConfig::IMTYPE imtype;
    switch( IMType )
    {
    case IMTYPE_all:
        imtype = ISShared::IIMConfig::IMTYPE_ALL;
        break;
    case IMTYPE_msn:
        imtype = ISShared::IIMConfig::IMTYPE_MSN;
        break;
    case IMTYPE_aol:
        imtype = ISShared::IIMConfig::IMTYPE_AOL;
        break;
    case IMTYPE_yahoo:
        imtype = ISShared::IIMConfig::IMTYPE_YAHOO;
        break;
    default:
        // Invalid IM Type passed in
        // Populate CNAVError object before returning
        return E_INVALIDARG;
    }

    hr = m_IMOptions.IsInstalled(imtype, bInstalled);
    if(FAILED(hr))
    {
        CCTRCTXE2(_T("Error calling IMOptions::IsInstalled, imtype=%d. HR=0x%08x"),(DWORD)imtype,hr);
        *pVal = VARIANT_FALSE;
    }
    else
        *pVal = bInstalled ? VARIANT_TRUE:VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::ConfigureIM(/*[in]*/ EIMType IMType)
{
    HRESULT hr;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Convert EIMType enumeration to IMTYPE enumeration
    ISShared::IIMConfig::IMTYPE imtype;
    switch( IMType )
    {
    case IMTYPE_all:
        imtype = ISShared::IIMConfig::IMTYPE_ALL;
        break;
    case IMTYPE_msn:
        imtype = ISShared::IIMConfig::IMTYPE_MSN;
        break;
    case IMTYPE_aol:
        imtype = ISShared::IIMConfig::IMTYPE_AOL;
        break;
    case IMTYPE_yahoo:
        imtype = ISShared::IIMConfig::IMTYPE_YAHOO;
        break;
    default:
        // Invalid IM Type passed in
        // Populate CNAVError object before returning
        return E_INVALIDARG;
    }

    hr = m_IMOptions.Configure(imtype);
    if(FAILED(hr))
    {
        CCTRCTXE1(_T("Error calling IMOptions::Configure. HR = 0x%08x"), hr);
    }

    return hr;
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
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if( m_bPasswordSupported && m_bIsPasswordDirty )
        *pVal = VARIANT_TRUE;
    else
        *pVal = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::put_IsPasswordDirty(BOOL newVal)
{
	if(m_bPasswordSupported)
	{
		if( newVal )
			m_bIsPasswordDirty = true;
		else
			m_bIsPasswordDirty = false;
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_OEMVendor(BSTR *pVal)
{
	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
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
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
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
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if( NULL == m_MyHwnd )
    {
        m_MyHwnd = ::FindWindow(ccWebWindow_ClassName, m_csTitle);
    }

    *pVal = (long)m_MyHwnd;

    return S_OK;
}
 
STDMETHODIMP CNAVOptions::get_IWPIsInstalled(BOOL *pVal)
{
    HRESULT hr;
    bool bVal = false;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if( FAILED(hr = m_IWPOptions.IsIWPInstalled(bVal)) )
    {
        CCTRCTXW1(_T("Error calling IsIWPInstalled. Assuming false. HR = 0x%08x"), hr);
        bVal = false;
    }

    *pVal = bVal ? VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CNAVOptions::get_IWPState(long *pVal)
{
    HRESULT hr;
    LONGLONG llVal;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if( FAILED(hr = m_IWPOptions.GetIWPState(llVal)) )
    {
        CCTRACEE (_T("GetIWPState failed. HR = 0x%08x"), hr);
        MakeError (IDS_ERR_GETIWPSTATE, hr, IDS_ISSHARED_ERROR_INTERNAL);
        *pVal = -1;
    }
    else
        *pVal = (LONG)llVal;

    return hr;
}

STDMETHODIMP CNAVOptions::get_IWPCanEnable(BOOL *pEnable)
{
    HRESULT hr;
    bool bEnable = false;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pEnable )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *pEnable = VARIANT_FALSE;

	if( FAILED(hr = m_IWPOptions.CanChangeIWPState(bEnable)) )
	{
		CCTRCTXE1(_T("Error calling CanChangeIWPState. HR = 0x%08x"), hr);
		MakeError (IDS_ERR_IWPCANENABLE, hr, IDS_ISSHARED_ERROR_INTERNAL);
	}

    *pEnable = bEnable ? VARIANT_TRUE:VARIANT_FALSE;
    return hr;
}

STDMETHODIMP CNAVOptions::get_IWPFirewallOn(BOOL *pOn)
{
    HRESULT hr;
    bool bOn = false;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pOn )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *pOn = VARIANT_TRUE;

    if( FAILED(hr = m_IWPOptions.IsIWPOn(bOn)) )
    {
        CCTRCTXE1(_T("IsIWPOn failed. HR = 0x%08x"), hr);
        MakeError (IDS_ERR_IWPCANENABLE, hr, IDS_ISSHARED_ERROR_INTERNAL);
    }

    *pOn = bOn ? VARIANT_TRUE:VARIANT_FALSE;
    return hr;
}

STDMETHODIMP CNAVOptions::get_IWPUserWantsToTrust(BOOL *pVal)
{
    HRESULT hr;
    bool bOn;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if( FAILED(hr = m_IWPOptions.GetIWPUserTrust(bOn)) )
    {
        CCTRCTXE1(_T("GetIWPUserTrust failed. HR = 0x%08x"), hr);
        MakeError (IDS_ERR_GETUSERWANTSTOTRUST, hr, IDS_ISSHARED_ERROR_INTERNAL);
    }
	else
		*pVal = (bOn ? TRUE : FALSE);

    return hr;
}

STDMETHODIMP CNAVOptions::put_IWPUserWantsToTrust(BOOL bUserWantsToTrust)
{
    HRESULT hr;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( FAILED(hr = m_IWPOptions.SetIWPUserTrust((bUserWantsToTrust==TRUE)?true:false)) )
    {
        CCTRCTXE1(_T("SetIWPUserTrust failed. HR = 0x%08x"), hr);
        MakeError (IDS_ERR_PUTUSERWANTSTOTRUST, hr, IDS_ISSHARED_ERROR_INTERNAL);
    }

    return hr;
}

STDMETHODIMP CNAVOptions::get_IWPUserWantsOn(BOOL *pVal)
{
    HRESULT hr;
    bool bOn;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if( FAILED(hr = m_IWPOptions.GetIWPUserEnabled(bOn)) )
    {
        CCTRCTXE1(_T("GetIWPUserEnabled failed. HR = 0x%08x"), hr);
        MakeError (IDS_ERR_GETUSERWANTSON, hr, IDS_ISSHARED_ERROR_INTERNAL);
    }
	else
		*pVal = (bOn ? TRUE : FALSE);

    return hr;
}

STDMETHODIMP CNAVOptions::put_IWPUserWantsOn(BOOL bUserWantsOn)
{
    HRESULT hr;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( FAILED(hr = m_IWPOptions.SetIWPUserEnabled((bUserWantsOn==TRUE)?true:false)) )
    {
        CCTRCTXE1(_T("SetIWPUserEnabled failed. HR = 0x%08x"), hr);
        MakeError (IDS_ERR_PUTUSERWANTSON, hr, IDS_ISSHARED_ERROR_INTERNAL);
    }

    return hr;
}

bool CNAVOptions::InitSymProtect()
{
    if(m_bWin64) /* SYMPROTECT64BITSUPPORT */
    {   // Remove this block once we have 64 bit SPBBC
        m_bSymProtectEnable = false;
        return false;
    }

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
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Make sure pointer is valid
    if( forceError ( ERR_INVALID_POINTER ) || NULL == bEnable )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    if(m_bWin64) /* SYMPROTECT64BITSUPPORT */
    {   // Remove this block once we have 64 bit SPBBC
        *bEnable = VARIANT_FALSE;
        return S_OK;
    }

    // SymProtect initialized?
    if( !InitSymProtect() )
    {
        CCTRACEE(_T("CNAVOptions::get_SymProtectEnabled() - Failed to initialize SymProtect."));
        MakeError (IDS_ERR_INIT_SYMPROTECT, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    *bEnable = (m_bSymProtectEnable ? VARIANT_TRUE : VARIANT_FALSE);

    return S_OK;
}

STDMETHODIMP CNAVOptions::put_SymProtectEnabled(BOOL bEnable)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if(m_bWin64) /* SYMPROTECT64BITSUPPORT */
    {   // Remove this block once we have 64 bit SPBBC
        m_bSymProtectEnable = false;
        m_bSymProtectDirty = false;
        return S_OK;
    }

    // SymProtect initialized?
    if( !InitSymProtect() )
    {
        CCTRACEE(_T("CNAVOptions::get_SymProtectEnabled() - Failed to initialize SymProtect."));
        MakeError (IDS_ERR_INIT_SYMPROTECT, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    // Set our internal enable to the state the user wants, we'll
    // do the actual work on the save when we see the dirty flag is set
    m_bSymProtectDirty = true;
    m_bSymProtectEnable = (bEnable ? true : false);

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
            CCTRACEI (_T("CNAVOptions::SetWorkingDirIWP - %s"), szDir);
            return S_OK;
        }
        else
        {
            CCTRACEE (_T("CNAVOptions::SetWorkingDirIWP - failed to set working IWP dir - %s"), szDir);
            return S_FALSE;
        }
    }

    // Reset old working dir
    //
    if ( '\0' != m_szCurrentWorkingDir[0] && SetCurrentDirectory (m_szCurrentWorkingDir))
    {
        CCTRACEI (_T("CNAVOptions::SetWorkingDirIWP - reset -  %s"), m_szCurrentWorkingDir);
        return S_OK;
    }
    else
    {
        CCTRACEE (_T("CNAVOptions::SetWorkingDirIWP - failed to reset working dir - %s"), m_szCurrentWorkingDir);
        return S_FALSE;
    }
}

STDMETHODIMP CNAVOptions::get_ThreatCategoryEnabled(unsigned long ulCategory,  BOOL* bEnabled)
{
    HRESULT hr;
    bool bExcluded;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Make sure pointer is valid
    if( forceError ( ERR_INVALID_POINTER ) || NULL == bEnabled )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    hr = m_ThreatCatOptions.GetThreatCatExcluded(ulCategory, bExcluded);
    if(FAILED(hr))
    {
        CCTRCTXE1(_T("Error calling GetThreatCatExcluded. HR = 0x%08x"), hr);
        *bEnabled = VARIANT_FALSE;
    }
    else
        *bEnabled = bExcluded ? VARIANT_TRUE:VARIANT_FALSE;

    return hr;
}

STDMETHODIMP CNAVOptions::put_ThreatCategoryEnabled(unsigned long ulCategory,  BOOL bEnabled)
{
    HRESULT hr;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }
    
    hr = m_ThreatCatOptions.SetThreatCatExcluded(ulCategory, bEnabled?true:false);
    if(FAILED(hr))
        CCTRCTXE1(_T("Error calling SetThreatCatExcluded. HR = 0x%08x"), hr);

    return hr;
}

STDMETHODIMP CNAVOptions::DefaultThreatCategoryEnabled(unsigned long ulCategory,  BOOL* bEnabled)
{
    HRESULT hr;
    bool bExcluded;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Make sure pointer is valid
    if( forceError ( ERR_INVALID_POINTER ) || NULL == bEnabled )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    hr = m_ThreatCatOptions.DefaultThreatCatExcluded(ulCategory, bExcluded);
    if(FAILED(hr))
    {
        CCTRCTXE1(_T("Error calling DefaultThreatCatExcluded. HR = 0x%08x"), hr);
        *bEnabled = VARIANT_FALSE;
    }
    else
        *bEnabled = bExcluded ? VARIANT_TRUE:VARIANT_FALSE;

    return hr;
}

STDMETHODIMP CNAVOptions::get_IMEnabled(unsigned long ulImType,  BOOL *pEnabled)
{
    HRESULT hr;
    DWORD dwState;

    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Make sure pointer is valid
    if( forceError ( ERR_INVALID_POINTER ) || NULL == pEnabled )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    hr = m_IMOptions.GetState((ISShared::IIMConfig::IMTYPE)ulImType, dwState);
    if(FAILED(hr))
    {
        CCTRCTXE1(_T("Error calling IMOptions::GetState. HR = 0x%08x"), hr);
        *pEnabled = VARIANT_FALSE;
    }
    else
        *pEnabled = (dwState)?VARIANT_TRUE:VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CNAVOptions::put_IMEnabled(unsigned long ulImType,  BOOL bEnable)
{
    HRESULT hr;
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    hr = m_IMOptions.SetState((ISShared::IIMConfig::IMTYPE)ulImType, (DWORD)bEnable);
    if(FAILED(hr))
    {
        CCTRCTXE1(_T("Error calling IMOptions::SetState. HR = 0x%08x"), hr);
        return hr;
    }

    return S_OK;
}

/* Invokes a modal IWP dialog.
 * 0 = IWP Exclusions
 * 1 = Application Rule List
 * 2 = IWP General Firewall Rules
 * 3 = AutoBlock
 */
STDMETHODIMP CNAVOptions::InvokeIWPDialog(unsigned long ulDialog)
{
    // Make sure the caller is Symantec signed
    if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    switch(ulDialog)
    {
    case 0:
        return m_IWPOptions.LaunchExclusionsUI(m_MyHwnd);
    case 1:
        return m_IWPOptions.LaunchAppRulesUI(m_MyHwnd);
    case 2:
		return m_IWPOptions.LaunchFWRulesUI(m_MyHwnd);
    case 3:
		return m_IWPOptions.LaunchAutoBlockUI(m_MyHwnd);
    default:
		break;
    }

	return E_FAIL;
}

STDMETHODIMP CNAVOptions::DefaultIMEnabled()
{
	HRESULT hr;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	hr = m_IMOptions.Default();

	if(FAILED(hr))
		CCTRCTXE1(_T("Error calling CIMOptions::Default. HR = 0x%08x"), hr);

	return hr;
}

STDMETHODIMP CNAVOptions::get_EmailScanIncoming(BOOL* bEnabled)
{
	HRESULT hr;
	DWORD dwVal;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	// Make sure pointer is valid
	if( forceError ( ERR_INVALID_POINTER ) || NULL == bEnabled )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	hr = m_EmailOptions.GetScanIncoming(dwVal);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error calling CEmailOptions::GetScanIncoming. HR = 0x%08x"), hr);
		*bEnabled = VARIANT_FALSE;
	}
	else
		*bEnabled = (dwVal)?VARIANT_TRUE:VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_EmailScanOutgoing(BOOL* bEnabled)
{
	HRESULT hr;
	DWORD dwVal;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	// Make sure pointer is valid
	if( forceError ( ERR_INVALID_POINTER ) || NULL == bEnabled )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	hr = m_EmailOptions.GetScanOutgoing(dwVal);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error calling CEmailOptions::GetScanOutgoing. HR = 0x%08x"), hr);
		*bEnabled = VARIANT_FALSE;
	}
	else
		*bEnabled = (dwVal)?VARIANT_TRUE:VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_EmailScanOEH(BOOL* bEnabled)
{
	HRESULT hr;
	DWORD dwVal;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	// Make sure pointer is valid
	if( forceError ( ERR_INVALID_POINTER ) || NULL == bEnabled )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	hr = m_EmailOptions.GetScanOEH(dwVal);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error calling CEmailOptions::GetScanOEH. HR = 0x%08x"), hr);
		*bEnabled = VARIANT_FALSE;
	}
	else
		*bEnabled = (dwVal)?VARIANT_TRUE:VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVOptions::put_EmailScanIncoming(BOOL bEnabled)
{
	HRESULT hr;
	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	hr = m_EmailOptions.SetScanIncoming((DWORD)bEnabled);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error calling CEmailOptions::SetScanIncoming. HR = 0x%08x"), hr);
		return hr;
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::put_EmailScanOEH(BOOL bEnabled)
{
	HRESULT hr;
	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	hr = m_EmailOptions.SetScanOEH((DWORD)bEnabled);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error calling CEmailOptions::SetScanOEH. HR = 0x%08x"), hr);
		return hr;
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::put_EmailScanOutgoing(BOOL bEnabled)
{
	HRESULT hr;
	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	hr = m_EmailOptions.SetScanOutgoing((DWORD)bEnabled);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Error calling CEmailOptions::SetScanOutgoing. HR = 0x%08x"), hr);
		return hr;
	}

	return S_OK;
}

STDMETHODIMP CNAVOptions::SetEmailPageDefaults()
{
	HRESULT hr;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	hr = m_EmailOptions.Default();

	if(FAILED(hr))
		CCTRCTXE1(_T("Error calling CEmailOptions::Default. HR = 0x%08x"), hr);

	return hr;
}

STDMETHODIMP CNAVOptions::SetIWPDefaults()
{
	HRESULT hr;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	hr = m_IWPOptions.Default();

	if(FAILED(hr))
		CCTRCTXE1(_T("Error calling CIWPOptions::Default. HR = 0x%08x"), hr);

	return hr;
}

STDMETHODIMP CNAVOptions::get_PasswordSupported(BOOL *pVal)
{
	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	if( m_bPasswordSupported )
		*pVal = TRUE;
	else
		*pVal = FALSE;

	return S_OK;
}
