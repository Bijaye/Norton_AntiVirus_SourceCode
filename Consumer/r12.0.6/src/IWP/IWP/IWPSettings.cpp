#include "StdAfx.h"
#include "IWPSettings.h"
#include "Plugin.h"             // for Agent mutexes
#include "msi.h"				// for managing cc shared and nis shared pieces
#include "SymNetDriverAPI.h"	// for reseting the symnet drivers
#include "IDSManager.h"         // For controlling IDS
#include "build.h"				// for our version information
#include "FWManager.h"          // Helper class for SymNetI calls
#include "StatusPropertyNames.h" // Status key names
#include "FREInteg_h.h"          // For installing ALEs and FW rules
#include "NAVInfo.h"            // For base NAV dir
#include "FREDefines.h"

using namespace IWP;

// Trust
bool CIWPSettings::g_bValidated = false;

// *** Feature settings
//
const WCHAR g_szIWPSettingsKey [] =      L"Internet Worm Protection\\Settings";
const WCHAR g_szIWPUserWantsOn [] =      L"UserWantsOn";        // DWORD - Does the user want IWP on?
const WCHAR g_szIWPFeatureAvailable [] = L"FeatureAvailable";   // DWORD - Does DRM say it's ok to run IWP? 1 = OK
const WCHAR g_szIWPLegacyClient [] =	 L"LegacyClient";       // DWORD - Has NPFMonitor found a legacy client?
const WCHAR g_szIWPRefCount [] =         L"ClientCount";        // DWORD - How many clients have asked us to yield?
const WCHAR g_szIWPFirewallOn [] =       L"FirewallOn";         // DWORD - Do we want IWP to use the firewall?
const WCHAR g_szIWPListenAlerts[] =      L"ListenAlerts";       // DWORD - Do we respond to listen events?

const char g_szIWPSubDir[] = "IWP";
const char g_szIWPALEFileName[] = "ales.xml";
const char g_szIWPFWRulesFileName[] = "defrules.dat";


CIWPSettings::CIWPSettings(void)
{
    m_bLoaded = false;
}

CIWPSettings::~CIWPSettings(void)
{
}

// IIWPSettings interface
const SYMRESULT CIWPSettings::GetValue ( const int iProperty /*in*/,
                                         DWORD& dwResult /*out*/)
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

    ccLib::CSingleLock lock (&m_critSettings, INFINITE, FALSE);

    if ( !m_PersistedSettings.GetSettings() )
        return SYMERR_UNKNOWN;

    switch (iProperty)
    {
    case IWPCanEnable:
        dwResult = (DWORD) isOKToEnable();
        break;
    case IWPUserWantsOn:
        return m_PersistedSettings.GetSettings()->GetDword (g_szIWPUserWantsOn, dwResult );
        break;
    case IWPFeatureAvailable:
        return m_PersistedSettings.GetSettings()->GetDword (g_szIWPFeatureAvailable, dwResult );
        break;
    case IWPState:
        dwResult = getState();
        break;
	case IWPLegacyFWInstalled:
		return m_PersistedSettings.GetSettings()->GetDword (g_szIWPLegacyClient, dwResult );
		break;
    case IWPFirewallOn:
        return m_PersistedSettings.GetSettings()->GetDword (g_szIWPFirewallOn, dwResult);
        break;
    case IWPListenAlerts:
        return m_PersistedSettings.GetSettings()->GetDword (g_szIWPListenAlerts, dwResult);
        break;
	default:
        return SYMERR_INVALIDARG;
        break;
    }

    return SYM_OK;
}

// IIWPSettings interface
SYMRESULT CIWPSettings::SetValue ( const int iProperty /*in*/,
                                   const DWORD dwValue /*on*/)
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

    ccLib::CSingleLock lock (&m_critSettings, INFINITE, FALSE);

    if ( !m_PersistedSettings.GetSettings() )
        return SYMERR_UNKNOWN;

	SYMRESULT symRes = SYMERR_UNKNOWN;
    switch (iProperty)
    {
    case IWPUserWantsOn:
        symRes = m_PersistedSettings.GetSettings()->PutDword (g_szIWPUserWantsOn, dwValue );
        break;
    case IWPFeatureAvailable:
        symRes = m_PersistedSettings.GetSettings()->PutDword (g_szIWPFeatureAvailable, dwValue );
        break;
    case IWPLegacyFWInstalled:
        symRes = m_PersistedSettings.GetSettings()->PutDword (g_szIWPLegacyClient, dwValue );
        break;
    case IWPFirewallOn:
        symRes = m_PersistedSettings.GetSettings()->PutDword (g_szIWPFirewallOn, dwValue );
        break;
    case IWPListenAlerts:
        symRes = m_PersistedSettings.GetSettings()->PutDword (g_szIWPListenAlerts, dwValue );
        break;
	default:
        symRes = SYMERR_INVALIDARG;
        break;
    }

	return symRes;
}

IIWPSettings::IWPSTATE CIWPSettings::getState (void)
{
    DWORD dwValue = 0;

    // Yielding?
    if ( SYM_FAILED(m_PersistedSettings.GetSettings()->GetDword ( g_szIWPRefCount, dwValue )))
        CCTRACEE ( "CIWPSettings::getState - failed getting yield state");

    if ( dwValue )
    {
        CCTRACEI("CIWPSettings::getState: Got ClientCout = %d, Yielding.", dwValue);
        return IWPStateYielding;
    }

    // Make sure we are the only StatusHelper running for this session.
    //
    ccLib::CMutex mutexIWP;

    if ( !mutexIWP.Create ( NULL, TRUE, IWP_MUTEX_SESSIONWIDE, FALSE ))
    {
		CCTRACEE ( "CIWPSettings::getState - Failed to create IWP sessionwide mutex" );
        return IWPStateError; // fatal
    }

	DWORD dwRet = ::WaitForSingleObject(mutexIWP,0);
	if(dwRet != WAIT_OBJECT_0)
	{
        // Agent is running

        // Read the current state from Status
        //
        CSMHelper status;
        if(!status.Load ( AVStatus::szIWPStatusKey ))
        {
            CCTRACEE("CIWPSettings::getState: status.Load != true");
            return IWPStateError;
        }

	    SYMRESULT sr = status.GetSettings()->GetDword (AVStatus::szIWPStatus, dwValue);
        if ( SYM_SUCCEEDED(sr))
        {
            CCTRACEI("CIWPSettings::getState: Got Status = %d", dwValue);
            return (IWPSTATE) dwValue;
        }
	    else
        {
            // If we can't get the Status value then we probably asked before the agent has run
            // for the first time.
            CCTRACEE("CIWPSettings::getState: status.GetSettings()->GetDword = 0x%x", sr);
            return IWPStateDisabled;
        }
	}
    else
    {
        // Agent is not running, the firewall will block everything. We call this an ERROR state
        //
        return IWPStateErrorNoAgent;
    }

}

// IIWPPrivateSettings interface
SYMRESULT CIWPSettings::InstallLVPlugin(BOOL bInstall)
{
	CCTRACEI ( "CIWPSettings::InstallLVPlugin() - Starting with bInstall = %d", bInstall);

    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

	SYMRESULT sResult = SYM_OK;

	try
	{
		CSMHelper settings;
		sResult = settings.Load(L"CommonClient\\LogViewer\\Plugins");
		if(SYM_SUCCEEDED(sResult))
		{
			if(bInstall)
			{
				char szSNlog[MAX_PATH] = {0};
				CNAVInfo NAVInfo;
				_snprintf( szSNlog, MAX_PATH-1, "%s\\%s\\%s", NAVInfo.GetNAVDir(), g_szIWPSubDir, "IWPLog.dll");
				sResult = settings.GetSettings()->PutString("IWPLog", szSNlog);
			}
			else
			{
				sResult = settings.GetSettings()->DeleteValue(L"IWPLog");
			}

			if(SYM_SUCCEEDED(sResult))
			{
				settings.Save(L"CommonClient\\LogViewer\\Plugins");
			}
			else
			{
				CCTRACEE ( "CIWPSettings::InstallLVPlugin() - Unable change settings key");
			}
		}
		else
		{
			CCTRACEE ( "CIWPSettings::InstallLVPlugin() - Unable to load ccSettings Key");
		}
	}
	catch(...)
	{
		CCTRACEE ( "CIWPSettings::InstallLVPlugin() - Unknown error setting LogViewer plugin");
		sResult = SYM_FALSE;
	}

	CCTRACEI ( "CIWPSettings::InstallLVPlugin() - Done");

	return sResult;
}

// IIWPSettings interface
SYMRESULT CIWPSettings::SetYield(const DWORD dwYield, LPCSTR lpcszAppID)
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();
	
    SYMRESULT srRet = SYM_OK;

	if(dwYield)
	{
		// they want to yield, so check if we're already yielding
		DWORD dwStateYielding;
		m_PersistedSettings.GetSettings()->GetDword (g_szIWPRefCount, dwStateYielding );
		CCTRACEI("Checking for Yielding State Returned: %d", dwStateYielding);

		// increment the refcount no matter what
		DWORD dwCount = AddClientRefCount(lpcszAppID);

		if(dwStateYielding)
		{
			// we are already yielding so we're done, just save the new refcount and return
			CCTRACEI("CIWPSettings::SetYield() : Yield request processed. Already yeilding. Refcount = %d", dwCount);
			save();
			return SYM_OK;
		}
		
		InstallLVPlugin(FALSE);
	}
	else
	{
		// they want us unyield, make sure we're not already
		DWORD dwStateYielding;
		m_PersistedSettings.GetSettings()->GetDword (g_szIWPRefCount, dwStateYielding );
		if(!dwStateYielding)
		{
			// we are not in the yielding state so don't do anything
			return SYM_OK;
		}
		
		// we are yielding so we need to check the refcount
		if(RemoveClientRefCount(lpcszAppID) != 0)
		{
			// we can't unyield yet because other firewall product are still using the
			// the driver.  as far as our client is concerned, this is not an error
			// just save the new refcount and return
			save();
			return SYM_OK;
		}
		
		// the refcount says we can unyield, so we have to do some work.
		// if things fail along the way, we should keep on going and add
		// some robustness throughout the agent to try doing these things
		// in the case we can't load up.  the agent should pop up error
		// displays when the time comes.
		if(SYM_FAILED(RegisterNISSharedDLLs()))
		{
			CCTRACEE("CIWPSettings::SetYield() : Failed to register NIS Shared DLLs.");
			srRet = SYM_FALSE;
		}
		
		// install default ALES
		if(SYM_FAILED(InstallDefaultAles()))
		{
			CCTRACEE("CIWPSettings::SetYield() : Failed to install Default ALES.");
			srRet = SYM_FALSE;
		}

		if(SYM_FAILED(ResetSymNetDrvSettings()))
		{			
			CCTRACEE("CIWPSettings::SetYield() : Failed to reset SymNetDrv Driver settings.");
			srRet = SYM_FALSE;
		}
		
		// Install default FW rules. We can install rules even if we never use
        // the firewall. This function handles turning on the firewall for us.
        //
		if(SYM_FAILED(InstallDefaultFWRules()))
		{
			CCTRACEE("CIWPSettings::SetYield() : Failed to install Default FW Rules.");
			srRet = SYM_FALSE;
		}

		// Reset IDS and turn it off.
        //
        CIDSManager IDSManager;

        if(SYM_FAILED(IDSManager.ResetIDSSettings ()))
		{
			CCTRACEE("CIWPSettings::SetYield() : Failed to reset IDS settings.");
			srRet = SYM_FALSE;
		}

		// put the IWP log viewer plugin back
		InstallLVPlugin(TRUE);
	}
	
	save();

	return srRet;
}

// Can IWP be turned on?
//
bool CIWPSettings::isOKToEnable ()
{
    DWORD dwValue = 0;

    // Yielding?
    if ( SYM_FAILED( m_PersistedSettings.GetSettings()->GetDword ( g_szIWPRefCount, dwValue )))
        CCTRACEE ( "CIWPSettings::isOKToEnable() - failed getting yield state");

    if ( dwValue )
        return false;

    // DRM?
    if ( SYM_FAILED(m_PersistedSettings.GetSettings()->GetDword ( g_szIWPFeatureAvailable, dwValue )))
        CCTRACEE ( "CIWPSettings::isOKToEnable() - failed getting feature avail");

    if ( !dwValue )
        return false;

    // All is OK.
    return true;
}

// IIWPSettings interface
SYMRESULT CIWPSettings::Save ()
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

    return save();
}

// Starts/Stops IWP depending on the current settings
//
SYMRESULT CIWPSettings::save (bool bDefaults /*false*/)
{
    // Save settings?
    ccLib::CSingleLock lock (&m_critSettings, INFINITE, FALSE);
    m_PersistedSettings.Save ( g_szIWPSettingsKey );

    // If we are just resetting the settings, don't reset the firewall by accident.
    // This would cause problems in install over/under NPF issues.
    //
    if ( bDefaults )
    {
        // Off by default, set the state
        SetState ( IWP::IIWPSettings::IWPStateDisabled );
    }
    else
        CheckFirewall ();

	// For internal and external clients who care about IWP status (global)
    // See defect 1-2W4D86 - don't use a ccSettings change event.
    //
    sendChangeEvent(SYM_REFRESH_IWP_OPTIONS_EVENT);

    return SYM_OK;
}

// Internal
void CIWPSettings::CheckFirewall ()
{
    // Yielding?
    DWORD dwStateYielding = 0;
    m_PersistedSettings.GetSettings()->GetDword (g_szIWPRefCount, dwStateYielding );

    // If we are yielding there's nothing to do
    //
    if ( 0 != dwStateYielding )
        return;

    // DRM?
    DWORD dwFeatureAvail = 0;
    m_PersistedSettings.GetSettings()->GetDword ( g_szIWPFeatureAvailable, dwFeatureAvail );

    // User?
    DWORD dwUserWantsOn = 0;
    m_PersistedSettings.GetSettings()->GetDword ( g_szIWPUserWantsOn, dwUserWantsOn );

    // Firewall?
    DWORD dwFirewallOn = 1;
    m_PersistedSettings.GetSettings()->GetDword ( g_szIWPFirewallOn, dwFirewallOn );

    // Should we turn the firewall off?
    // If we shouldn't be running and we are the only firewall client, yes!
    // (turning on is handled by the agent - start() function. This way
    // if the agent isn't running we can still stop the firewall.)
    //
    if (!dwFeatureAvail || !dwUserWantsOn) 
    {
        CCTRACEI ( "IWPSettings::CheckFirewall - turning off IDS" );
        

        // Turn off IDS first - defect 1-2K1FXT
        CIDSManager IDSManager;
        IDSManager.Enable (false);

        // Turn the whole feature off
        //
        CCTRACEI ( "IWPSettings::CheckFirewall - turning off IWP" );
        CFWManager firewall;
        firewall.SetFirewallSettings (false);
        return;
    }

    CCTRACEI ( "IWPSettings::CheckFirewall - turning Firewall %d", dwFirewallOn );

    // Turn it on, it might already be on, but that's OK.
    //
    CFWManager firewall;
    if ( SYM_SUCCEEDED (firewall.SetFirewallSettings (dwFirewallOn ? true:false, true, true)))
    {
        // Looks good
        CCTRACEI ( "IWPSettings::CheckFirewall - success" );
    }
    else
    {
        CCTRACEE ( "IWPSettings::CheckFirewall - error enabling" );
        SetState ( IWP::IIWPSettings::IWPStateError );
    }
}

// IIWPPrivateSettings interface
SYMRESULT CIWPSettings::ResetSymNetDrvSettings(const PFACTORYRESETPARAMS pFRP)
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

    CFWManager Firewall;
    return Firewall.ResetSymNetDrvSettings (pFRP);
}

// IIWPPrivateSettings interface
SYMRESULT CIWPSettings::ResetIDSSettings(LPCSTR lpszProductName, const UINT64 ui64ProductVersion)
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

    CIDSManager IDSManager;
    return IDSManager.ResetIDSSettings (lpszProductName, ui64ProductVersion );
}

// IIWPPrivateSettings interface
SYMRESULT CIWPSettings::RegisterNISSharedDLLs()
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

	MsiSetInternalUI(INSTALLUILEVEL_NONE, NULL);
	
	HRESULT hRet = MsiReinstallFeature(_T("{2908F0CB-C1D4-447F-97A2-CFC135C9F8D4}"), //NAV IWP.MSI ProductCode
										_T("NISShared"),			// NIS Shared Firewall components feature
										REINSTALLMODE_FILEMISSING);	// reinstall
	
	if(SUCCEEDED(hRet))
	{
		CCTRACEI(_T("CIWPSettings::RegisterNISSharedDLLs() - Successfully reinstalled NIS Shared FW Components."));
        return SYM_OK;
	}
	else
	{
		CCTRACEE(_T("CIWPSettings::RegisterNISSharedDLLs() - MsiReinstallFeature() == 0x%x"), hRet);
		return SYM_FALSE;
	}
}

// IIWPPrivateSettings interface
DWORD CIWPSettings::AddClientRefCount(LPCSTR lpcszAppID)
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

	DWORD dwClientExists = 0;
	DWORD dwRefCount = 0;
	
	m_PersistedSettings.GetSettings()->GetDword(lpcszAppID, dwClientExists);
    m_PersistedSettings.GetSettings()->GetDword(g_szIWPRefCount, dwRefCount );
	
	CCTRACEI("Checking for \"%s\" Appid, Got %d", lpcszAppID, dwClientExists);
	CCTRACEI("Checking ClientCount, Got %d", dwRefCount);
	
    if(!dwClientExists)
	{
		// if the client doesn't exist then increment the refcount and add the client
		m_PersistedSettings.GetSettings()->PutDword(lpcszAppID, 1);
		m_PersistedSettings.GetSettings()->PutDword(g_szIWPRefCount, ++dwRefCount);
	}

	return dwRefCount;
}

// IIWPPrivateSettings interface
DWORD CIWPSettings::RemoveClientRefCount(LPCSTR lpcszAppID)
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

	DWORD dwClientExists = 0;
	DWORD dwRefCount = 0;
	
	m_PersistedSettings.GetSettings()->GetDword(lpcszAppID, dwClientExists);
    m_PersistedSettings.GetSettings()->GetDword(g_szIWPRefCount, dwRefCount );
	
	
    if(dwClientExists)
	{
		// if the client doesn't exist then deccrement the refcount and delete the client
		m_PersistedSettings.GetSettings()->DeleteValue(lpcszAppID);
		m_PersistedSettings.GetSettings()->PutDword(g_szIWPRefCount, --dwRefCount);
	}

	return dwRefCount;
}

// IIWPSettings interface
SYMRESULT CIWPSettings::Reload ()
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    ccLib::CSingleLock lock (&m_critSettings, INFINITE, FALSE);
    CCTRACEI ("CIWPSettings::Reload - start");

    if ( m_PersistedSettings.Load ( g_szIWPSettingsKey ))
    {
        bool bMissingProps = false;

        // Set defaults
        //
        DWORD dwValue = 0;
        ccSettings::ISettingsPtr pSettings = m_PersistedSettings.GetSettings();

        SYMRESULT result = pSettings->GetDword ( g_szIWPUserWantsOn, dwValue );

        if ( SYM_FAILED (result))
        {
            CCTRACEE ("CIWPSettings::Reload - no UserWantsOn setting - 0x%x", result);
            pSettings->PutDword ( g_szIWPUserWantsOn, 0 );      // Default = off
            bMissingProps = true;
        }

        result = pSettings->GetDword ( g_szIWPFeatureAvailable, dwValue );
        if ( SYM_FAILED (result))
        {
            CCTRACEE ("CIWPSettings::Reload - no FeatureAvailable setting - 0x%x", result);
            pSettings->PutDword ( g_szIWPFeatureAvailable, 1 ); // Default = on (let NAVOptRefresh disable us)
            bMissingProps = true;
        }

        result = pSettings->GetDword ( g_szIWPRefCount, dwValue );
        if ( SYM_FAILED (result))
        {
            CCTRACEE ("CIWPSettings::Reload - no RefCount setting - 0x%x", result);
            pSettings->PutDword ( g_szIWPRefCount, 0 );    // Default = false
            bMissingProps = true;
        }

        result = pSettings->GetDword ( g_szIWPFirewallOn, dwValue );
        if ( SYM_FAILED (result))
        {
            CCTRACEE ("CIWPSettings::Reload - no FirewallOn setting - 0x%x", result);
            pSettings->PutDword ( g_szIWPFirewallOn, 1 );    // Default = ON
            bMissingProps = true;
        }

        result = pSettings->GetDword ( g_szIWPListenAlerts, dwValue );
        if ( SYM_FAILED (result))
        {
            CCTRACEE ("CIWPSettings::Reload - no ListenAlerts setting - 0x%x", result);
            pSettings->PutDword ( g_szIWPListenAlerts, 1 );    // Default = ON
            bMissingProps = true;
        }

        m_bLoaded = true;
        
        if ( bMissingProps )
        {
            CCTRACEE ("CIWPSettings::Reload - saving defaults");
            result = save (true);
            if ( SYM_FAILED (result))
                CCTRACEE ("CIWPSettings::Reload - saving defaults failed - 0x%x", result);
        }
    }
    else
    {
        CCTRACEE ("CIWPSettings::Reload - failed to load");
        return SYMERR_UNKNOWN;
    }

    CCTRACEI ("CIWPSettings::Reload - settings loaded");
    return SYM_OK;
}
// Not made public
//
void CIWPSettings::SetState ( IWP::IIWPSettings::IWPSTATE state )
{
    CSMHelper status;
    // Don't just blow away other values, forward compatible.
    status.Load ( AVStatus::szIWPStatusKey );

    status.GetSettings()->PutDword (AVStatus::szIWPStatus, state);
    if ( status.Save ( AVStatus::szIWPStatusKey ))
        CCTRACEI ("IWPSettings::SetState - saved state %d", state);
    else
        CCTRACEE ("IWPSettings::SetState - error saving state");
    
    // For external clients who care about IWP status (global)
    sendChangeEvent(IWP::SYM_REFRESH_IWP_EVENT);
}

// For external clients who care about IWP status (global)
//
void CIWPSettings::sendChangeEvent (LPCTSTR szEventName)
{
    // For our null dacl
    ATL::CDacl nulldacl;
    ATL::CSecurityDesc secdesc;
    ATL::CSecurityAttributes secatt;
    ccLib::CEvent eventStatus;

    // Set our null dacl
    //
    if ( nulldacl.IsEmpty())
    {
        nulldacl.SetNull();
        secdesc.SetDacl (nulldacl);
        secatt.Set (secdesc); // This throws assertions in debug mode, I know.
    }

    // If I use a EveryoneDACL, or NullDacl, the clients (IWP) get access denied
    // when they try to create the event on their end.
    //
    // Reset the owner
    if ( !eventStatus.Create ( &secatt,
                                TRUE,
                                FALSE,
                                szEventName,
                                TRUE))
    {
        CCTRACEE ("CIWPSettings::sendChangeEvent - unable to create event");
        assert(false);
    }

    eventStatus.PulseEvent ();
}

// IIWPPrivateSettings interface
SYMRESULT CIWPSettings::InstallDefaultAles ()
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

    // Client may not know to init COM
    try
    {
        // FREInteg can't handle MTA COM.
        ccLib::CCoInitialize ((ccLib::CCoInitialize::ThreadModel)ccLib::CCoInitialize::eSTAModel);
                              
        // Call FREInteg with the default ALEs file that is
        // installed to the IWP folder.
        //
        CComPtr<IFREIntegEx> pFREInteg;
        HRESULT hr = E_FAIL;
        _bstr_t bstrAleFile;
        char szAleFile[MAX_PATH] = {0};
        CNAVInfo NAVInfo;
        _snprintf ( szAleFile, MAX_PATH-1, "%s\\%s\\%s", NAVInfo.GetNAVDir(), g_szIWPSubDir, g_szIWPALEFileName);
        bstrAleFile = szAleFile;

        hr = pFREInteg.CoCreateInstance (CLSID_FREIntegEx);
        if ( SUCCEEDED (hr))
        {
            CComQIPtr<IFREProcActivate> pFREProc(pFREInteg);
            if (pFREProc)
                pFREProc->AddProcessor(FRE_ALES);

            hr = pFREInteg->IntegrateXMLFile ( bstrAleFile, VARIANT_TRUE );
            if (FAILED (hr))
            {
                CCTRACEE ("CIWPSettings::InstallDefaultAles - couldn't integrate ALEs - 0x%x", hr);

                if ( hr == E_ACCESSDENIED )
                    return SYMERR_ACCESSDENIED;

                return SYMERR_UNKNOWN;
            }
            else
                return SYM_OK;
        }
        else
            CCTRACEE ("CIWPSettings::InstallDefaultAles - couldn't create FREInteg - 0x%x", hr);
    }
    catch (_com_error& err)
    {
        CCTRACEW ("CIWPSettings::InstallDefaultAles - %s", err.ErrorMessage());
    }
    
    return SYMERR_UNKNOWN;
}

// IIWPPrivateSettings interface
SYMRESULT CIWPSettings::InstallDefaultFWRules ()
{
    //Check trust
    if ( !isItSafe ())
        return SYMERR_ACCESSDENIED;

    if ( !m_bLoaded )
        Reload();

    // Client may not know to init COM
    try
    {
        // FREInteg can't handle MTA COM.
        ccLib::CCoInitialize ((ccLib::CCoInitialize::ThreadModel)ccLib::CCoInitialize::eSTAModel);

        // Call FREInteg with the default FW Rules file that is
        // installed to the IWP folder.
        //
        CComPtr<IFREIntegEx> pFREInteg;
        HRESULT hr = E_FAIL;
        _bstr_t bstrFWRulesFile;
        char szFWRulesFile[MAX_PATH] = {0};
        CNAVInfo NAVInfo;
        _snprintf ( szFWRulesFile, MAX_PATH-1, "%s\\%s\\%s", NAVInfo.GetNAVDir(), g_szIWPSubDir, g_szIWPFWRulesFileName);
        bstrFWRulesFile = szFWRulesFile;

        hr = pFREInteg.CoCreateInstance (CLSID_FREIntegEx);
        if ( SUCCEEDED (hr))
        {
            CComQIPtr<IFREProcActivate> pFREProc(pFREInteg);
            if (pFREProc)
                pFREProc->AddProcessor(FRE_RULES);

            hr = pFREInteg->IntegrateXMLFile ( bstrFWRulesFile, VARIANT_TRUE );
            if (FAILED (hr))
            {
                CCTRACEE ("CIWPSettings::InstallDefaultFWRules - couldn't integrate FW rules - 0x%x", hr);
                if ( hr == E_ACCESSDENIED )
                    return SYMERR_ACCESSDENIED;

                return SYMERR_UNKNOWN;
            }

            DWORD dwFirewallOn = 1;
		    if (SYM_SUCCEEDED (m_PersistedSettings.GetSettings()->GetDword (g_szIWPFirewallOn, dwFirewallOn )))
		        CCTRACEI("CIWPSettings::InstallDefaultFWRules - Checking for FirewallOn Returned: %d", dwFirewallOn);
            else
                CCTRACEE("CIWPSettings::InstallDefaultFWRules - failed to get FirewallOn");

            if ( dwFirewallOn )
            {
                CFWManager firewall;
                return firewall.SetFirewallSettings (true);
            }

            return SYM_OK;
        }
        else
            CCTRACEE ("CIWPSettings::InstallDefaultFWRules - couldn't create FREInteg - 0x%x", hr);
    }
    catch (_com_error& err)
    {
        CCTRACEW ("CIWPSettings::InstallDefaultFWRules - %s", err.ErrorMessage());
    }

    return SYMERR_UNKNOWN;
}

bool CIWPSettings::isItSafe ()
{
	if (!g_bValidated)
	{
        if (GetDisabled())
        {
            g_bValidated = true;
            return true;
        }

		ccVerifyTrust::ccVerifyTrust_IVerifyTrust TrustLoader;
		ccVerifyTrust::IVerifyTrustPtr pVerifyTrust;

		if(SYM_SUCCEEDED(TrustLoader.CreateObject(pVerifyTrust.m_p)) && (pVerifyTrust != NULL))
		{
			if(pVerifyTrust->Create(true) == ccVerifyTrust::eNoError)
			{
				if (pVerifyTrust->VerifyCurrentProcess(ccVerifyTrust::eSymantecSignature) 
					== ccVerifyTrust::eVerifyError)
                {
                    CCTRACEE ("CIWPSettings::isItSafe - not verified");
					return false;
                }
			}
		}

		g_bValidated = true;
	}

	return g_bValidated;
}