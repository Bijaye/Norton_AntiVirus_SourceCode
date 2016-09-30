// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "CClientCallback.h"

nsc::NSCRESULT CreateClientAction (
    nsc::IAction **ppActionReturn,
    const DWORD dwNameID,
    const DWORD dwDescID,
    const CClientCallback::e_CallbackCookie eCallbackCookie,
    const GUID& guid_action,
	const GUID& guid_callback);

nsc::NSCRESULT CreateStatus (
    nsc::IConsoleType **ppStatusReturn, 
	nsc::e_SymConsoleType ConsoleType,
    bool bOnOff = false);

nsc::NSCRESULT CreateStatus (
    nsc::IConsoleType **ppStatusReturn, 
	cc::IString*& status_string);

nsc::NSCRESULT CreateHealth (
    nsc::IHealth **ppHealthReturn,
    nsc::e_health e_health,
    DWORD dwNameID,
    DWORD dwDescID,
    CClientCallback::e_CallbackCookie eCallbackCookie,
    const GUID& guidAction,
	const GUID& guidCallback = CLSID_NAV_Client_Callback);

// create health with no action
//  used by other createhealth and for good health
nsc::NSCRESULT CreateHealth (
    nsc::IHealth **ppHealthReturn,
    nsc::e_health e_health);

// return true if admin, false if not admin or cannot determine
bool IsAdmin (const nsc::IContext* context_in);