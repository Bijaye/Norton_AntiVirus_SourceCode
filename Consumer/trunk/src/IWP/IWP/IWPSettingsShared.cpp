////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "IWPSettingsShared.h"
#include "FWSettings.h"
#include "ccRegistry.h"
#include "FWModuleNames.h"
#include "FWPathProvider.h"
#include "avPathProvider.h"

namespace IWP
{

bool CIWPSettingsShared::GetFWAgentPath(CString& cstrFWAgentPath)
{
	TCHAR szPath[MAX_PATH] = {0};
	size_t nPathSize = MAX_PATH;
	
	if (! fw::CFWPathProvider::GetPath(szPath, nPathSize))
	{
		CCTRCTXE0(_T("Failed to get CFW Installed Apps path"));
		return false;
	}

	cstrFWAgentPath = szPath;
	cstrFWAgentPath += _T('\\');	// GetPath returns a path w/o the backslash.
	cstrFWAgentPath += fw::sz_FWAgent_dll;

	return true;
}

bool CIWPSettingsShared::GetFWAlertPath(CString& cstrFWAlertPath)
{
	TCHAR szPath[MAX_PATH] = {0};
	size_t nPathSize = MAX_PATH;

	if (! CAVPathProvider::GetPath(szPath, nPathSize))
	{
		CCTRCTXE0(_T("Failed to get NAV Installed Apps path"));
		return false;
	}
	cstrFWAlertPath = szPath;
    cstrFWAlertPath += _T('\\');
	cstrFWAlertPath += g_szFWAlertFileName;

	return true;
}


}