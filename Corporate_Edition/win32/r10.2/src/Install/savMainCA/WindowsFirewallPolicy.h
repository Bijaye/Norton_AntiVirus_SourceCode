// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
// WindowsFirewallPolicy.h : main header file for the SAVMAINCA DLL
//

#ifndef __WINDOWSFIREWALLPOLICY_H__
#define __WINDOWSFIREWALLPOLICY_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <..\..\..\..\..\..\tools\PLATSDKXPSP2\include\netfw.h>
#define DOUBLE_MAX_PATH			(MAX_PATH * 2)

class CWindowsFirewallPolicy
{
public:
	CWindowsFirewallPolicy();
	virtual ~CWindowsFirewallPolicy();
	int ModifyWindowsFirewallPolicy (MSIHANDLE hInstall, wchar_t* strFileName, wchar_t* strPolicyName, bool bAddingApp);

private:
	void WindowsFirewallCleanup(IN INetFwProfile* fwProfile);
	HRESULT WindowsFirewallInitialize(MSIHANDLE hInstall, OUT INetFwProfile** fwProfile);
	HRESULT WindowsFirewallModifyApp(
			MSIHANDLE hInstall, 
            IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            IN const wchar_t* fwName,
			bool bAddingApp);
	HRESULT WindowsFirewallAppIsEnabled(
			MSIHANDLE hInstall, 
            IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            OUT BOOL* fwAppEnabled);
};

#endif // __WINDOWSFIREWALLPOLICY_H__