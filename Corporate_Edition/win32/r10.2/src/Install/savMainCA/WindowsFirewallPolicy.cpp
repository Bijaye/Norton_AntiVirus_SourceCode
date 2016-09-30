// WindowsFirewallPolicy.cpp : Defines the initialization routines for the DLL.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2006 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "WindowsFirewallPolicy.h"
#include "savMainCA.h"

CWindowsFirewallPolicy::CWindowsFirewallPolicy()
{
}
CWindowsFirewallPolicy::~CWindowsFirewallPolicy()
{
}

int CWindowsFirewallPolicy::ModifyWindowsFirewallPolicy (MSIHANDLE hInstall, wchar_t* strFileName, wchar_t* strPolicyName, bool bAddPolicy)  
{
	TCHAR	szMessString[DOUBLE_MAX_PATH]	= {0};
	HRESULT hr = S_OK;
	HRESULT comInit = E_FAIL;
	INetFwProfile* fwProfile = NULL;

	// Initialize COM.
	comInit = CoInitializeEx(
			0,
			COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
			);

	// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
	// initialized with a different mode. Since we don't care what the mode is,
	// we'll just use the existing mode.
	if (comInit != RPC_E_CHANGED_MODE)
	{
		hr = comInit;
		if (FAILED(hr))
		{
			sssnprintf(szMessString, sizeof(szMessString), "CoInitializeEx failed: 0x%08lx\n", hr);
			MSILogMessage(hInstall, szMessString);
			goto error;
		}
	}

    // Retrieve the firewall profile currently in effect.
    hr = WindowsFirewallInitialize(hInstall, &fwProfile);
    if (FAILED(hr))
    {
		sssnprintf(szMessString, sizeof(szMessString), "WindowsFirewallInitialize failed: 0x%08lx\n", hr);
		MSILogMessage(hInstall, szMessString);
        goto error;
    }
    // Add Application to the authorized application collection.
	    hr = WindowsFirewallModifyApp(
			hInstall,
            fwProfile,
            strFileName,
            strPolicyName,
			bAddPolicy);
    if (FAILED(hr))
    {
		sssnprintf(szMessString, sizeof(szMessString), "WindowsFirewallAddApp failed: 0x%08lx\n", hr);
		MSILogMessage(hInstall, szMessString);
        goto error;
    }
error:

    // Release the firewall profile.
    WindowsFirewallCleanup(fwProfile);

    // Uninitialize COM.
    if (SUCCEEDED(comInit))
    {
        CoUninitialize();
    }
    return 0;
}

HRESULT CWindowsFirewallPolicy::WindowsFirewallInitialize(MSIHANDLE hInstall, OUT INetFwProfile** fwProfile)
{
	TCHAR	szMessString[DOUBLE_MAX_PATH]	= {0};
	HRESULT hr = S_OK;
    INetFwMgr* fwMgr = NULL;
    INetFwPolicy* fwPolicy = NULL;

    _ASSERT(fwProfile != NULL);

    *fwProfile = NULL;

    // Create an instance of the firewall settings manager.
    hr = CoCreateInstance(
            __uuidof(NetFwMgr),
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwMgr),
            (void**)&fwMgr
            );
    if (FAILED(hr))
    {
        printf("CoCreateInstance failed: 0x%08lx\n", hr);
        goto error;
    }

    // Retrieve the local firewall policy.
    hr = fwMgr->get_LocalPolicy(&fwPolicy);
    if (FAILED(hr))
    {
		sssnprintf(szMessString, sizeof(szMessString), "get_LocalPolicy failed: 0x%08lx\n", hr);
		MSILogMessage(hInstall, szMessString);
        goto error;
    }

    // Retrieve the firewall profile currently in effect.
    hr = fwPolicy->get_CurrentProfile(fwProfile);
    if (FAILED(hr))
    {
		sssnprintf(szMessString, sizeof(szMessString), "get_CurrentProfile failed: 0x%08lx\n", hr);
		MSILogMessage(hInstall, szMessString);
        goto error;
    }

error:

    // Release the local firewall policy.
    if (fwPolicy != NULL)
    {
        fwPolicy->Release();
    }

    // Release the firewall settings manager.
    if (fwMgr != NULL)
    {
        fwMgr->Release();
    }

    return hr;
}


HRESULT CWindowsFirewallPolicy::WindowsFirewallModifyApp(
            MSIHANDLE hInstall, 
			IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            IN const wchar_t* fwName,
			bool bAddingApp)
{
 	TCHAR	szMessString[DOUBLE_MAX_PATH]	= {0};
	HRESULT hr = S_OK;
    BOOL fwAppEnabled;
    BSTR fwBstrName = NULL;
    BSTR fwBstrProcessImageFileName = NULL;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwName != NULL);

    // First check to see if the application is already authorized.
    hr = WindowsFirewallAppIsEnabled(
			hInstall,
            fwProfile,
            fwProcessImageFileName,
            &fwAppEnabled
            );
    if (FAILED(hr))
    {
		sssnprintf(szMessString, sizeof(szMessString), "WindowsFirewallAppIsEnabled failed: 0x%08lx\n", hr);
		MSILogMessage(hInstall, szMessString);
        goto error;
    }
    // Only add the application if it isn't already authorized or only remove if it is authorized
    if (((!fwAppEnabled)&&(true == bAddingApp)) || ((fwAppEnabled)&&(false == bAddingApp)))
    {
        // Retrieve the authorized application collection.
        hr = fwProfile->get_AuthorizedApplications(&fwApps);
        if (FAILED(hr))
        {
			sssnprintf(szMessString, sizeof(szMessString), "get_AuthorizedApplications failed: 0x%08lx\n", hr);
			MSILogMessage(hInstall, szMessString);
            goto error;
        }

        // Create an instance of an authorized application.
        hr = CoCreateInstance(
                __uuidof(NetFwAuthorizedApplication),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwAuthorizedApplication),
                (void**)&fwApp
                );
        if (FAILED(hr))
        {
			sssnprintf(szMessString, sizeof(szMessString), "CoCreateInstance failed: 0x%08lx\n", hr);
			MSILogMessage(hInstall, szMessString);
            goto error;
        }

        // Allocate a BSTR for the process image file name.
        fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
        if (SysStringLen(fwBstrProcessImageFileName) == 0)
        {
            hr = E_OUTOFMEMORY;
			sssnprintf(szMessString, sizeof(szMessString), "SysAllocString failed: 0x%08lx\n", hr);
			MSILogMessage(hInstall, szMessString);
            goto error;
        }
		if ( true == bAddingApp )
		{
			// Set the process image file name.
			hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
			if (FAILED(hr))
			{
				sssnprintf(szMessString, sizeof(szMessString), "put_ProcessImageFileName failed: 0x%08lx\n", hr);
				MSILogMessage(hInstall, szMessString);
				goto error;
			}

			// Allocate a BSTR for the application friendly name.
			fwBstrName = SysAllocString(fwName);
			if (SysStringLen(fwBstrName) == 0)
			{
				hr = E_OUTOFMEMORY;
				sssnprintf(szMessString, sizeof(szMessString), "SysAllocString failed: 0x%08lx\n", hr);
				MSILogMessage(hInstall, szMessString);
				goto error;
			}

			// Set the application friendly name.
			hr = fwApp->put_Name(fwBstrName);
			if (FAILED(hr))
			{
				sssnprintf(szMessString, sizeof(szMessString), "put_Name failed: 0x%08lx\n", hr);
				MSILogMessage(hInstall, szMessString);
				goto error;
			}

			// Add the application to the collection.
			hr = fwApps->Add(fwApp);
			if (FAILED(hr))
			{
				sssnprintf(szMessString, sizeof(szMessString), "Add failed: 0x%08lx\n", hr);
				MSILogMessage(hInstall, szMessString);
				goto error;
			}
			sssnprintf(szMessString, sizeof(szMessString), "Authorized application %lS is now enabled in the firewall.\n",
				fwProcessImageFileName);
			MSILogMessage(hInstall, szMessString);
		}
		else
		{
			// Add the application to the collection.
			hr = fwApps->Remove(fwBstrProcessImageFileName);
			if (FAILED(hr))
			{
				sssnprintf(szMessString, sizeof(szMessString), "Remove failed: 0x%08lx\n", hr);
				MSILogMessage(hInstall, szMessString);
				goto error;
			}
			sssnprintf(szMessString, sizeof(szMessString), "Application %lS has now been removed from the firewall.\n",
				fwProcessImageFileName);
			MSILogMessage(hInstall, szMessString);
		}

    }

error:

    // Free the BSTRs.
    SysFreeString(fwBstrName);
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL)
    {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL)
    {
        fwApps->Release();
    }

    return hr;
}
void CWindowsFirewallPolicy::WindowsFirewallCleanup(IN INetFwProfile* fwProfile)
{
    // Release the firewall profile.
    if (fwProfile != NULL)
    {
        fwProfile->Release();
    }
}

HRESULT CWindowsFirewallPolicy::WindowsFirewallAppIsEnabled(
			MSIHANDLE hInstall, 
            IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            OUT BOOL* fwAppEnabled
            )
{
	TCHAR	szMessString[DOUBLE_MAX_PATH]	= {0};
	HRESULT hr = S_OK;
    BSTR fwBstrProcessImageFileName = NULL;
    VARIANT_BOOL fwEnabled;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwAppEnabled != NULL);

    *fwAppEnabled = FALSE;

    // Retrieve the authorized application collection.
    hr = fwProfile->get_AuthorizedApplications(&fwApps);
    if (FAILED(hr))
    {
		sssnprintf(szMessString, sizeof(szMessString), "get_AuthorizedApplications failed: 0x%08lx\n", hr);
		MSILogMessage(hInstall, szMessString);
        goto error;
    }

    // Allocate a BSTR for the process image file name.
    fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
    if (SysStringLen(fwBstrProcessImageFileName) == 0)
    {
        hr = E_OUTOFMEMORY;
		sssnprintf(szMessString, sizeof(szMessString), "SysAllocString failed: 0x%08lx\n", hr);
		MSILogMessage(hInstall, szMessString);
        goto error;
    }

    // Attempt to retrieve the authorized application.
    hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
    if (SUCCEEDED(hr))
    {
        // Find out if the authorized application is enabled.
        hr = fwApp->get_Enabled(&fwEnabled);
        if (FAILED(hr))
        {
			sssnprintf(szMessString, sizeof(szMessString), "get_Enabled failed: 0x%08lx\n", hr);
			MSILogMessage(hInstall, szMessString);
            goto error;
        }

        if (fwEnabled != VARIANT_FALSE)
        {
            // The authorized application is enabled.
            *fwAppEnabled = TRUE;
			sssnprintf(szMessString, sizeof(szMessString), "Authorized application %lS is enabled in the firewall.\n",
                fwProcessImageFileName);
			MSILogMessage(hInstall, szMessString);
        }
        else
        {
			sssnprintf(szMessString, sizeof(szMessString), "Authorized application %lS is disabled in the firewall.\n",
                fwProcessImageFileName);
			MSILogMessage(hInstall, szMessString);
        }
    }
    else
    {
        // The authorized application was not in the collection.
        hr = S_OK;
		sssnprintf(szMessString, sizeof(szMessString),"Authorized application %lS is disabled in the firewall.\n",
            fwProcessImageFileName);
		MSILogMessage(hInstall, szMessString);
    }

error:

    // Free the BSTR.
    SysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL)
    {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL)
    {
        fwApps->Release();
    }

    return hr;
}
