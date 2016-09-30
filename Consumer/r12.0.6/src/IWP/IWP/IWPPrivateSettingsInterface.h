#pragma once

#include "SymInterface.h"
#include "SymNetDriverApi.h"

namespace IWP
{
// Settings interface for internal NAV clients who have "intimate knowledge" of IWPs infrastucture
// (ie: NPFMonitor)
//
class IIWPPrivateSettings : public ISymBase
{
public:
    typedef enum PRIVATE_PROPERTY
    {
        IWPFeatureAvailable = 1000,		// *in/out* - Setting for DRM to use to turn off IWP (don't set this
										//            unless you're DRM)
		IWPLegacyFWInstalled,			// NPFMonitor settings - NOT to be used internally to determine when to
										// un yield.  Instead use the non-exposed ClientRefCount
        IWPFirewallOn,                  // *in/out* - Default is 1. Setting this to 0 will disable all firewall
                                        //            functionality in IWP.
        IWPListenAlerts                 // *in/out* - Do we alert for listen events?
    };

    // Override of IWPSettings functions
    //
    virtual const SYMRESULT GetValue ( const int iProperty /*in*/,
                                       DWORD& dwResult /*out*/) throw() = 0;

    virtual SYMRESULT SetValue ( const int iProperty /*in*/,
                                 const DWORD dwValue /*in*/) throw() = 0;
    virtual SYMRESULT Reload (void) throw() = 0;

    // Saves settings and starts/stops IWP depending on the current settings
    //
    virtual SYMRESULT Save () throw() = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: ResetSymNetDrvSettings()
	// Description	: Resets the SymNetDrv Driver settings
	// Parameters	: pFRP - pointer to FACTORYRESETPARAMS structure that specifies product
	//				  information to pass to the drivers.  when null the function uses NAVs info.
	// Return		: SYM_OK for success
	// Notes		: See ResetIDSSettings()
    virtual SYMRESULT ResetSymNetDrvSettings() throw () = 0;
	virtual SYMRESULT ResetSymNetDrvSettings(PFACTORYRESETPARAMS pFRP) throw() = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: ResetIDSSettings()
	// Description	: Resets the IDS settings
	// Parameters	: lpszProductName - Prodcut name to pass to the ClientIDS,
	//				  ui64ProductVersion - Product version to pass to the ClientIDS,
	//				  when lpszProductName is NULL we use NAV's info
	// Return		: SYM_OK for success
	// Notes		: See ResetSymNetDrvSettings()
    virtual SYMRESULT ResetIDSSettings() throw() = 0;
    virtual SYMRESULT ResetIDSSettings(LPCSTR lpszProductName, const UINT64 ui64ProductVersion) throw() = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: RegisterNISSharedDLLs()
	// Description	: Re-registers NAVs Shared NIS Firewall DLLs.
	// Parameters	: None
	// Return		: SYM_OK for success
	// Notes		: None.
	virtual SYMRESULT RegisterNISSharedDLLs() throw() = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: AddClientRefCount()
	// Description	: Checks to see if the passed in AppID already exists, if not added it and bumps
	//				  the refcount. (Save still needs to be called for this to take effect)
	// Parameters	: lpcszAppID - Application specific ID that you want to add (eg: NIS, NAV)
	// Return		: Resulting RefCount
	// Notes		: See RemoveClientRefCount()
	virtual DWORD AddClientRefCount(LPCSTR lpcszAppID) throw() = 0;
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: RemoveClientRefCount()
	// Description	: Checks to see if the passed in AppID already exists, if so removes it and decs
	//				  the refcount (Save still needs to be called for this to take effect)
	// Parameters	: lpcszAppID - Application specific ID that you want to add (eg: NIS, NAV)
	// Return		: Resulting Refcount
	// Notes		: See AddClientRefCount()
	virtual DWORD RemoveClientRefCount(LPCSTR lpcszAppID) throw() = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: InstallDefaultAles()
	// Description	: Reinstalls ALEs from the default file in the IWP directory.
	// Parameters	: None
	// Return		: SYM_OK on success
	// Notes		: 
	virtual SYMRESULT InstallDefaultAles() throw() = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: InstallDefaultFWRules()
	// Description	: Reinstalls firewall rules from the default file in the IWP directory.
	// Parameters	: None
	// Return		: SYM_OK on success
	// Notes		: 
	virtual SYMRESULT InstallDefaultFWRules() throw() = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Name			: InstallLVPlugin()
	// Description	: adds/removes registration of iwp log viewer plugin
	// Parameters	: binstall - whether to add or remove the entry
	// Return		: SYM_OK on success
	// Notes		: 
	virtual SYMRESULT InstallLVPlugin(const BOOL bInstall) throw() = 0;
};

// {FB2484B5-A254-4e6e-8B94-8EC08A6E51EB}
SYM_DEFINE_INTERFACE_ID(IID_IWPPrivateSettings, 
						0xfb2484b5, 0xa254, 0x4e6e, 0x8b, 0x94, 0x8e, 0xc0, 0x8a, 0x6e, 0x51, 0xeb);
typedef CSymPtr<IIWPPrivateSettings> IIWPPrivateSettingsPtr;
typedef CSymQIPtr<IIWPPrivateSettings, &IID_IWPPrivateSettings> IIWPPrivateSettingsQIPtr;

}; // end namespace IWP
