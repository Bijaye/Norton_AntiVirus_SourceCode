#pragma once
#include "IWPPrivateSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "ccDisableVerifyTrustImpl.h"
#include "SMHelper.h"

extern const WCHAR g_szIWPSettingsKey[];

// Implementation of IWPSettingsInterface
//
class CIWPSettings : public ISymBaseImpl< CSymThreadSafeRefCount >,
                     public ccVerifyTrust::CDisableVerifyTrustImpl <ccVerifyTrust::IDisableVerifyTrust>,
                     public IWP::IIWPSettings,                 
					 public IWP::IIWPPrivateSettings
{
public:
	SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( IWP::IID_IWPSettings, IWP::IIWPSettings )
		SYM_INTERFACE_ENTRY( IWP::IID_IWPPrivateSettings, IWP::IIWPPrivateSettings )
        SYM_INTERFACE_ENTRY( ccVerifyTrust::IID_DisableVerifyTrust, ccVerifyTrust::IDisableVerifyTrust)
	SYM_INTERFACE_MAP_END()

    CIWPSettings(void);
    virtual ~CIWPSettings(void);

// IIWPSettings methods
    virtual const SYMRESULT GetValue ( const int iProperty /*in*/,
                                       DWORD& dwResult /*out*/);

    virtual SYMRESULT SetValue ( const int iProperty /*in*/,
                                 const DWORD dwValue /*in*/);

    virtual SYMRESULT Save ();

    // Reloads the settings - settings are loaded during construction so this is
    // only needs to be called if the client wants a refresh.
    virtual SYMRESULT Reload ();

	virtual SYMRESULT SetYield(const DWORD dwYield, LPCSTR lpcszAppID);

// IIWPPriveSettings methods
    // Defaults
    virtual SYMRESULT ResetSymNetDrvSettings()
    {
        return ResetSymNetDrvSettings (NULL);
    }	
	virtual SYMRESULT ResetSymNetDrvSettings(PFACTORYRESETPARAMS pFRP);	//when NULL we set it for NAV's current version

    // Defaults
    virtual SYMRESULT ResetIDSSettings()
    {
        return ResetIDSSettings (NULL, 0);
    }
    virtual SYMRESULT ResetIDSSettings(LPCSTR lpszProductName,			//when NULL we set it for NAV's current version
									   const UINT64 ui64ProductVersion);			//ignored when lpszProductName = NULL

	virtual SYMRESULT RegisterNISSharedDLLs();

	virtual DWORD AddClientRefCount(LPCSTR lpcszAppID);
	virtual DWORD RemoveClientRefCount(LPCSTR lpcszAppID);
    virtual SYMRESULT InstallDefaultAles ();
    virtual SYMRESULT InstallDefaultFWRules ();
	virtual SYMRESULT InstallLVPlugin (const BOOL bInstall);

// Special internal settings
    void SetState ( IWP::IIWPSettings::IWPSTATE state );
    void CIWPSettings::CheckFirewall (void);

protected:
    // Save the settings and reset the firewall if needed
    //
    SYMRESULT save (bool bDefaults = false);

    // Helper function for sending change events - 1-4GXYXI
    void sendChangeEvent(LPCTSTR szEventName);

    // Settings bag for persisted values
    //
    CSMHelper m_PersistedSettings;

    // General critical section for accessing settings
    ccLib::CCriticalSection m_critSettings;

    IWP::IIWPSettings::IWPSTATE m_state;

    IWP::IIWPSettings::IWPSTATE getState (void);
    bool isOKToEnable (void);

    // Check the trust for the caller
    static bool g_bValidated;
    bool isItSafe (void);
    
    // Have we loaded settings?
    bool m_bLoaded; 
};
