////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "SymInterface.h"
#include "ccKeyValueCollectionInterface.h"

namespace IWP
{
   
//////////////////////////////////////////////////////////////////////////////////////////
// IIWPSettings -- 
//////////////////////////////////////////////////////////////////////////////////////////
// If IWP is not installed the DLL that implements this interface will not exist.
// Use this fact as a "IsIWPInstalled" logic.
//
class IIWPSettings : public ISymBase
{
public:

	// TODO: Revisit code that used to depend on this and see if we can eliminate
	enum PROPERTY
	{
		IWPCanEnable,           // *DEPRECATED*
								// *out*    - Is IWP allowed to be enabled? Takes "IWPFeatureAvailable"
								//            DRM, yielding, etc. into account. Does not take into
								//            account the IWPUserWantsOn setting!

		IWPUserWantsOn,         // *DEPRECATED*
								// *in/out* - Does the user want IWP? - User setting

		IWPState				// *out*    - Use this to retrieve the yielding state
	};

	typedef enum IWPSTATE  // <100 == error!
	{
		IWPStateError,	            // Couldn't get state - ERROR 

		IWPStateErrorNoAgent,       // *DEPRECATED*
									// The agent isn't running for that session - ERROR (unless starting ccApp)

		IWPStateNotAvailable = 100, // *DEPRECATED*
									// Not supposed to exist (feature not installed) - Not an error!

		IWPStateYielding,           // We are in a yielded state
		
		IWPStateEnabled,            // We are in an unyielded state
		
		IWPStateDisabled            // *DEPRECATED*
									// Feature is OFF
	};

	// Returns NOT_IMPLEMENTED.  Functionality replaced by CFW.
    virtual const SYMRESULT GetValue ( const int iProperty /*in*/,
                                       DWORD& dwResult /*out*/) throw() = 0;

	// Returns NOT_IMPLEMENTED.  Functionality replaced by CFW.
    virtual SYMRESULT SetValue ( const int iProperty /*in*/,
                                 const DWORD dwValue /*in*/) throw() = 0;

	// Returns NOT_IMPLEMENTED.  Functionality replaced by CFW.
    virtual SYMRESULT Save () throw() = 0;

	// Returns NOT_IMPLEMENTED.  Functionality replaced by CFW.
	virtual SYMRESULT Reload () throw() = 0;

	// Call this if you want IWP to you (un)yield for your product.  This will yield/unyield CFW.
	//		lpcszAppID is not used anymore!
	virtual SYMRESULT SetYield(const DWORD dwYield, LPCSTR lpcszAppID) throw() = 0;
};

// {023CDBC6-8636-4938-9768-A81D056ED250}
SYM_DEFINE_INTERFACE_ID(IID_IWPSettings, 0x23cdbc6, 0x8636, 0x4938, 0x97, 0x68, 0xa8, 0x1d, 0x5, 0x6e, 0xd2, 0x50 );
TYPEDEF_SYM_POINTERS(IIWPSettings, IID_IWPSettings)

class IIWPSettings2 : public IIWPSettings
{
public:
	enum eConfigureTasks
	{
		ConfigureIWP_SetPortBlockingFlags	 = 0x1,		// Does not take a KVC value
		ConfigureIWP_SetOEMState			 = 0x2,		// Does not take a KVC value
	};

	// This will take care of setting up the correct OEMState, port block settings, etc.
	//
	// ConfigureTasks specifies which tasks to perform.  These should be values from eConfigureTasks
	// pTaskKVC specifies a matching value for the task if it needs it.  Set the value that matches the key from eConfigureTasks.
	//		pTaskKVC may be NULL.
	//		ie: ConfigureIWP_SetInitialState can take on values from SymNetI::ISymNetSettings::StartupOptions.
	//			[key ConfigureIWP_SetInitialState (0x4)'s value is SymNeti::ISymNetSettings::eStartupBootAutomatic (0x1)]
	virtual SYMRESULT ConfigureIWP(DWORD dwConfigureTasks, const cc::IKeyValueCollection* pTaskKVC) throw() = 0;
};

// {82037B79-57F5-49e5-8E8F-37E1EB38C36B}
SYM_DEFINE_INTERFACE_ID(IID_IWPSettings2, 0x82037b79, 0x57f5, 0x49e5, 0x8e, 0x8f, 0x37, 0xe1, 0xeb, 0x38, 0xc3, 0x6b);
TYPEDEF_SYM_POINTERS(IIWPSettings2, IID_IWPSettings2)

} // end namespace IWP
