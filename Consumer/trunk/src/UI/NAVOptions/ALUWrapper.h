////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Wrapper to wrap the damn ielement stuff

#ifndef __NAVOPTIONS_ALUWRAPPER_H__
#define __NAVOPTIONS_ALUWRAPPER_H__

#include "stdafx.h"
#include "uiNISDataElementGuids.h"
#include "uiUpdateInterface.h"
#include "uiNumberDataInterface.h"
#include "uiDateDataInterface.h"
#include "isDataClientLoader.h" // loader, no .cpp needed

class CALUOptions
{
public:
	CALUOptions();
	~CALUOptions();

	HRESULT GetALUState(bool& bOn);
	HRESULT GetALUUserOn(bool &bOn);
	HRESULT SetALUUserOn(bool bOn);
	HRESULT Dirty();
	HRESULT Enable();

private:
	bool m_bInitialized;
	bool m_bOn;
	bool m_bUserWantsOn;

	// IElement stuff
	ui::IProviderPtr m_pProvider;
	ui::IElementPtr m_pElement;

	HRESULT Load();
};

#endif