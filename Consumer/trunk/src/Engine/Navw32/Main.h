////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MAIN_H
#define MAIN_H

#include "NAVW32.h"

bool RunIntegrator();
HWND ShowHelp();

HRESULT PerformPepCall();

bool CanShowUI(bool bShowCED, CNavw32& Navw32);
HMODULE GetAlertDll();
HRESULT CheckVirusDefsAndSubscriptionStatus(HMODULE hModule);

#endif

