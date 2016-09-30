////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "IWPSettingsInterface.h"
#include "ccDisableVerifyTrustImpl.h"
#include "FWInterface.h"
#include "FWSetupInterface.h"
#include "FWLoaders.h"
#include "SNDHelper.h"

namespace IWP
{


// Implementation of IWPSettingsInterface
//
class CIWPSettings : public ISymBaseImpl< CSymThreadSafeRefCount >,
                     public ccVerifyTrust::CDisableVerifyTrustImpl <ccVerifyTrust::IDisableVerifyTrust>,
                     public IIWPSettings2
{
public:
	SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( IID_IWPSettings, IIWPSettings )
		SYM_INTERFACE_ENTRY( IID_IWPSettings2, IIWPSettings2 )
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

    virtual SYMRESULT Reload ();

	virtual SYMRESULT SetYield(const DWORD dwYield, LPCSTR lpcszAppID);

// IIWPSettings2 methods
	virtual SYMRESULT ConfigureIWP(DWORD dwConfigureTasks, const cc::IKeyValueCollection* pTaskKVC) throw();

protected:
    // Check the trust for the caller
    bool isItSafe (void);

	// Init FWSetup
	bool InitFWSetup();
	bool InitSNDHelper();

	SYMRESULT GetYield(bool& bYield);
	SYMRESULT PerformYield();
	SYMRESULT PerformUnYield();
	SYMRESULT InvokeIArbit();

	SYMRESULT SetPortBlockingFlags();
	SYMRESULT SetOEMState(SymNeti::ISymNetSettings::OEMState state);

    static bool g_bValidated;

	FWSetupLoader			m_FWSetupLoader;
	CSymPtr<IFWSetup>		m_pFWSetup;

	CSymPtr<CSNDHelper>		m_pSNDHelper;

};

}