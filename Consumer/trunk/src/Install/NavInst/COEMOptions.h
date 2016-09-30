////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "NavOptions.h"
#include "OptNames.h"
//#include "SrtOptionNames.h" // update for AP 10.0
#include "atlbase.h"
#include <winreg.h>
#include "Shellapi.h"
#include "NAVInfo.h"
#include "OSInfo.h"

// IElement stuff for AP
#include "ISDataClientLoader.h"
#include "uiNISDataElementGuids.h"
#include "uiProviderInterface.h"
#include "uiElementInterface.h"
#include "uiNumberDataInterface.h"

// custom interfaces
#include "isDataNoUIInterface.h"

class COEMOptions
{
public:
	//public functions
	COEMOptions();
	~COEMOptions();
	
	bool Initialize();
	bool TurnOnThreatCat(int iIndex, bool bYes_No);
	bool DefaultThreatCat(int iIndex);
	bool TurnOnAPEarlyLoad(bool bYes_No);
    bool SetAPValue(const wchar_t* lpszOptionName, DWORD dwValue );
	bool TurnOnEMI(bool bYes_No);
	bool TurnOnEMO(bool bYes_No);
	bool TurnOnAutoProtect(bool bEnable);

	bool SetCCServicesStates(BOOL bAutoStart, BOOL bStartNow);

private:
	//member vars
	bool RebootNeeded();
	bool m_bAllOK;
	INAVOptions* pOptionsObject;
    ui::IProviderPtr m_pProvider;

	HRESULT m_hrCOM;
};