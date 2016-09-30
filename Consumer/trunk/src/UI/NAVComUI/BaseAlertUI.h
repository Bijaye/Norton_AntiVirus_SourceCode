////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymInterfaceHelpers.h"
#include "ccAlertInterface.h"
#include "OEHUIInterface.h"
#include <string>
#include "tchar.h"

// Callback sink for the Alert help (Alert Assistant)
//
class CBaseAlertUIHelp :
    public cc::IAlertCallback,
    public ISymBaseImpl<CSymThreadSafeRefCount>  
{
public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_AlertCallback, cc::IAlertCallback)
	SYM_INTERFACE_MAP_END() 

    CBaseAlertUIHelp(void){};
    virtual ~CBaseAlertUIHelp(void){};

	bool Run (HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context);
};

class CBaseAlertUI
{
public:
    CBaseAlertUI(void);
    virtual ~CBaseAlertUI(void);

    int DoModal ();

    OEHACTION GetUserAction();

protected:
    static tstring m_strCCAlertPath;
    static CSymInterfaceDllHelper m_symHelperCCAlert;
	static CString m_strProductName;

    // Alert Help object
    //
    cc::IAlertCallbackPtr m_pHelpLink;
    
    // SmartPtr to CCAlert class
    //
    cc::IAlertPtr m_pUI; 

	// Selected action.
	OEHACTION m_Action;
};
