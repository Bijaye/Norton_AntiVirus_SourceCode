////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace IWP
{

static const TCHAR g_szFWAlertKeyName[] = _T("FWAlert");
static const TCHAR g_szFWAlertFileName[] = _T("FWAlert.dll");

static const TCHAR g_szCCAppPluginPath[] = _T("CommonClient\\ccApp\\Plugins");

class CIWPSettingsShared
{
public:
	CIWPSettingsShared(void) {}
	~CIWPSettingsShared(void) {}

	// These include the module filename
	static bool GetFWAgentPath(CString& cstrFWAgentPath);
	static bool GetFWAlertPath(CString& cstrFWAlertPath);
};

}