#pragma once
#include "SymInterface.h"

namespace IWP
{
    // This Global Windows event gets pulsed when the IWP callback updates the IDS signatures.
    const char SYM_REFRESH_IDS_IWP_SIGS_EVENT[] = "SYM_REFRESH_IDS_IWP_SIGS_EVENT";

    // This Global Windows event gets pulsed when IWP changes it's state
    const char SYM_REFRESH_IWP_EVENT[] = "SYM_REFRESH_IWP_EVENT";

    // This Global Windows event gets pulsed when IWP changes it's options
    const char SYM_REFRESH_IWP_OPTIONS_EVENT[] = "SYM_REFRESH_IWP_OPTIONS_EVENT";

    
//////////////////////////////////////////////////////////////////////////////////////////
// IIWPSettings -- 
//////////////////////////////////////////////////////////////////////////////////////////
//
// NOTE: YOU MUST INITIALIZE COM BEFORE USING THIS INTERFACE
//
// If IWP is not installed the DLL that implements this interface will not exist.
// Use this fact as a "IsIWPInstalled" logic.
//
class IIWPSettings : public ISymBase
{
public:
    enum PROPERTY
    {
        IWPCanEnable,           // *out*    - Is IWP allowed to be enabled? Takes "IWPFeatureAvailable"
                                //            DRM, yielding, etc. into account. Does not take into
                                //            account the IWPUserWantsOn setting!
        IWPUserWantsOn,         // *in/out* - Does the user want IWP? - User setting
        IWPState				// *out*    - Returns the current state of IWP - IWPState
    };

    typedef enum IWPSTATE  // <100 == error!
    {
		IWPStateError,	            // Couldn't get state - ERROR 
        IWPStateErrorNoAgent,       // The agent isn't running for that session - ERROR (unless starting ccApp)
		IWPStateNotAvailable = 100, // Not supposed to exist (feature not installed) - Not an error!
        IWPStateYielding,           // IWP is off because another product is taking precedence
		IWPStateEnabled,            // Feature is ON
		IWPStateDisabled            // Feature is OFF
    };

    virtual const SYMRESULT GetValue ( const int iProperty /*in*/,
                                       DWORD& dwResult /*out*/) throw() = 0;

    virtual SYMRESULT SetValue ( const int iProperty /*in*/,
                                 const DWORD dwValue /*in*/) throw() = 0;

    // Saves settings and starts/stops IWP depending on the current settings
    //
    virtual SYMRESULT Save () throw() = 0;

    // Reloads settings. Use after a change sink event. You don't have to 
    // call this at startup, the settings are already loaded.
    //
    virtual SYMRESULT Reload () throw() = 0;

	// Call this if you want IWP to you (un)yield for your product.  Pass an AppID (eg: NIS, NPF)
	// NOTE: you do not need to call save after making this call
	//
	virtual SYMRESULT SetYield(const DWORD dwYield, LPCSTR lpcszAppID) throw() = 0;
};

// {023CDBC6-8636-4938-9768-A81D056ED250}
SYM_DEFINE_INTERFACE_ID(IID_IWPSettings, 
						0x23cdbc6, 0x8636, 0x4938, 0x97, 0x68, 0xa8, 0x1d, 0x5, 0x6e, 0xd2, 0x50 );
typedef CSymPtr<IIWPSettings> IIWPSettingsPtr;
typedef CSymQIPtr<IIWPSettings, &IID_IWPSettings> IIWPSettingsQIPtr;
} // end namespace IWP
