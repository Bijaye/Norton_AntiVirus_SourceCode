////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* NAV Firewall (IWP) Options Object                                    */
/************************************************************************/

#ifndef __NAVOPTIONS_IWPOPTIONS_H__
#define __NAVOPTIONS_IWPOPTIONS_H__

#include "stdafx.h"
#include "uiNISDataElementGuids.h"
#include "uiUpdateInterface.h"
#include "uiNumberDataInterface.h"
#include "uiDateDataInterface.h"
#include "isDataClientLoader.h" // loader, no .cpp needed
#include "SNDHelper.h"

class CIWPOptions
{
public:
    CIWPOptions();
    ~CIWPOptions();

    // CORE OPTIONS INTERFACE BEGIN
    HRESULT Initialize();
    HRESULT Save();
    HRESULT Default();
    HRESULT Notify();
    bool IsDirty();
    // CORE OPTIONS INTERFACE END

    // IWP Business Methods
    HRESULT IsIWPInstalled(bool& bInstalled);
    HRESULT IsIWPOn(bool &bOn);
    HRESULT CanChangeIWPState(bool &bAllowed);
    HRESULT GetIWPState(LONGLONG& llState);
    // End IWP Business Methods

    // IWP Interface Methods
    HRESULT LaunchExclusionsUI(HWND hWndParent);
    HRESULT LaunchAppRulesUI(HWND hWndParent);
    HRESULT LaunchFWRulesUI(HWND hWndParent);
    HRESULT LaunchAutoBlockUI(HWND hWndParent);
    // End IWP Interface Methods

    // IWP Gets/Sets
	HRESULT GetIWPUserTrust(bool& bEnable);
    HRESULT SetIWPUserTrust(const bool& bEnable);
	HRESULT GetIWPUserEnabled(bool& bEnable);
    HRESULT SetIWPUserEnabled(const bool& bEnable);
    // End Gets/Sets
private:
    bool m_bInitialized;

    LONGLONG m_llState;
	LONGLONG m_llUserState;

    // IElement stuff
    ui::IProviderPtr m_pProvider;
    ui::IElementPtr m_pIWPElement;

	// snd helper with shared root-db
	CSymPtr<CSNDHelper> m_sndhelper;

	bool m_bTrustLocalLans;
	bool m_bTrustLocalLansOrg;
};

#endif