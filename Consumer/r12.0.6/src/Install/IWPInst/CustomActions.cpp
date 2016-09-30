#include "StdAfx.h"

#include "instopts.h"
#include "instoptsnames.h"
#include "SymNetDriverAPI.H"
#include "build.h"

#define INITIIDS
#include "IWPSettingsInterface.h"
#include "IWPSettingsLoader.h"
#include "IICFManager.h"
#include "NAVICFManagerLoader.h"
#include "ccDisableVerifyTrustImpl.h"

using namespace InstallToolBox;

// global defines
const TCHAR* _REG_LEGACY_INSTALLEDAPPS_VALUES[]		= {_T("Norton Internet Security"), _T("Internet Security"), NULL};


extern "C" __declspec(dllexport) UINT __stdcall CheckForZeroBoot(MSIHANDLE hInstall)
{
	// check if we are in the OEM zero-boot-impact mode
	DWORD dwValue = 0;
	CInstoptsDat InstoptsDat;
	InstoptsDat.ReadDWORD(INSTOPTS_NODRIVERS, dwValue, 0);

	MsiSetProperty(hInstall, _T("ZERO_BOOT_IMPACT"), dwValue ? _T("1") : _T("0"));

	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT __stdcall StartNPFMonitorService(MSIHANDLE hInstall)
{
    SC_HANDLE hNPFMonitor = NULL;
    SC_HANDLE hSCM = NULL;

	try
	{
		// open the scm
		hSCM = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
		if(hSCM)
		{
			// open the npfmonitor service
			hNPFMonitor = OpenService(hSCM, _T("NPFMntor"), SERVICE_START | SERVICE_CHANGE_CONFIG);
			if(hNPFMonitor)
			{
				// configure the service to auto start
				ChangeServiceConfig(hNPFMonitor, SERVICE_NO_CHANGE, SERVICE_AUTO_START,
								SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, _T(""), 
								_T("Norton AntiVirus Firewall Monitor Service"));
					
				// start the service
				StartService(hNPFMonitor, 0, NULL);	
			}
		}
	}
	catch(...)
	{
		CCTRACEE("StartNPFMonitor() : Unknown Error");
	}

    try
    {
        if(hNPFMonitor)
            CloseServiceHandle(hNPFMonitor);

        if(hSCM)
            CloseServiceHandle(hSCM);
    }
    catch(...)
    {
        CCTRACEE("StartNPFMonitor() : Unknown Error while trying to free handles.");
    }	
	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT __stdcall SetRegisterDLLSProperty(MSIHANDLE hInstall)
{
	// during install we only register these DLLs if NIS is not installed
	// if NIS is installed then we cannot hijack these DLLS. instead the NPFMonitor
	// will detect if NIS gets removed and will reregister these DLLS.

	try
	{
		bool bNISInstalled = false;

		// check to see if NIS is installed and if it is we don't register
		ATL::CRegKey rkInstalledApps;
		rkInstalledApps.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"), KEY_READ);
		for(int i = 0; _REG_LEGACY_INSTALLEDAPPS_VALUES[i] != NULL; i++)
		{
			// if the caller cares where its installed to then use the
			// lpszInstallPath and size, otherwise use our own local buffer
			TCHAR szPath[MAX_PATH] = {0};
			DWORD dwSize = MAX_PATH;
			if(rkInstalledApps.QueryStringValue(_REG_LEGACY_INSTALLEDAPPS_VALUES[i], 
												szPath,
												&dwSize) == ERROR_SUCCESS)
			{
				bNISInstalled = true;
			}
		}
		
		// set the shared property depending on whether or not NIS is installed
		MsiSetProperty(hInstall, _T("INSTALL_SHARED_REGKEYS"), bNISInstalled ? _T("0") : _T("1"));
	}
	catch(...)
	{
		CCTRACEE("SetRegisterDLLSProperty() : Unknown Error");
	}
	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT __stdcall SetUnregisterDLLSProperty(MSIHANDLE hInstall)
{
	// during uninstall we will only remove these DLLS if we are not in the yielding state
	// if we are in the yielding state then we need to leave them behind or we will break
	// the current firewall client.
	
	try
	{
      
        bool bYielding = true;
        
        if(SUCCEEDED(CoInitialize(NULL)))
        {

		    // Scope the loader so it goes out of scope after the object ptr and we don't crash
            // The trusted cache loaders don't seem to compile when exposed in an export like this.
            // We are using the older no-cache.
		    IWP_IWPSettingsNoCache IWPSettingsLdr;
		    {
			    IWP::IIWPSettingsPtr spIWPSettings;

			    if(SYM_SUCCEEDED(IWPSettingsLdr.CreateObject(spIWPSettings.m_p)) && spIWPSettings != NULL)
			    {
                    // Make sure ccAlert trusts us, even though our .EXE is not signed
                    if ( ccVerifyTrust::CDisableVerifyTrustClient::DisableVerifyTrust(spIWPSettings))
                    {
                        DWORD dwState = 0;
				        spIWPSettings->GetValue(spIWPSettings->IWPState, dwState);
				        if(spIWPSettings->IWPStateYielding != dwState)
					        bYielding = false;
                    }
			    }
		    }
    	    
            CoUninitialize();	   
        }

        // set the shared property depending on whether or not we are yielding
		MsiSetProperty(hInstall, _T("INSTALL_SHARED_REGKEYS"), bYielding ? _T("0") : _T("1"));

	}
	catch(...)
	{
		CCTRACEE("SetRegisterDLLSProperty() : Unknown Error");
	}
	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT __stdcall RestoreICF(MSIHANDLE hInstall)
{
	// scope the loader up here so it gets
	NAVICFManagerLoader ICFLdr;

	try
	{
		// get our ICFMgr object
		IICFManagerPtr ICFMgr;
		if(SYM_FAILED(ICFLdr.CreateObject(ICFMgr.m_p)))
		{
			CCTRACEE("RestoreICF() : Unable to load ICFMgr.dll");
			return ERROR_SUCCESS;
		}
		
		if(SYM_FAILED(ICFMgr->Init()))
		{
			CCTRACEE("RestoreICF() : Unable to init ICFMgr.dll");
			return	 ERROR_SUCCESS;
		}

		// if it is enabled then disable it
		if(SYM_SUCCEEDED(ICFMgr->RestoreConnections()))
		{
			CCTRACEI("RestoreICF() : Successfully Restored ICF");
		}
		else
		{
			CCTRACEW("RestoreICF() : Unsuccessfully Restore ICF");
		}
	}
	catch(...)
	{
		CCTRACEE("RestoreICF() : Unknown Exception");
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  GetDefaultFWState() - looks for the IWP FW on options
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall GetDefaultFWState(MSIHANDLE hInstall)
{
	try
	{
		CInstoptsDat Instopts;
		DWORD dwValue = 1;
		Instopts.ReadDWORD(INSTOPTS_EnableIWPFirewall, dwValue, 1);
		
		if(0 != dwValue)
		{
			MsiSetProperty(hInstall, _T("IWPFIREWALLON"), _T("1"));
			
			// add a reboot since we'll be turning on the FW
			ATL::CRegKey SymSetupKey;
			SymSetupKey.Create(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\SymSetup"));
			SymSetupKey.SetDWORDValue(_T("RebootNeeded"), 1);
		}
		else
		{
			MsiSetProperty(hInstall, _T("IWPFIREWALLON"), _T("0"));
		}
	}
	catch(...)
	{
		CCTRACEE("(GetDefaultFWState() : Unknown Exception");
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  SetSNState(BOOL bInstall)
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL SetSNState(BOOL bInstall)
{
	HMODULE hSymNeti = NULL;

	try
	{
		// First attempt to load Savrt32.dll from the NAV directory
		TCHAR szSymNetiPath[MAX_PATH+1] = {0};
		GetSystemDirectory(szSymNetiPath, MAX_PATH+1);
		PathAppend(szSymNetiPath, _T("SymNeti.dll"));
		hSymNeti = LoadLibraryEx(szSymNetiPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if( hSymNeti )
		{
			SNSETPRODUCTSTATE SNSetProductState = (SNSETPRODUCTSTATE)::GetProcAddress(hSymNeti,  reinterpret_cast<LPCTSTR>(eSNSetProductState));		
			
			if( SNSetProductState )
			{
				DWORD dwVer = VER_NUM_PRODVERMAJOR;
				dwVer = (dwVer << 16) | VER_NUM_PRODVERMINOR;
			
				if(bInstall)
					SNSetProductState(eCLIENTID_Nav_FW, GS_INSTALLED_CLIENT_VERSION, dwVer);
				else
					SNSetProductState(eCLIENTID_Nav_FW, GS_UNINSTALL_CLIENT_VERSION, dwVer);	
			}
		}
    }
	catch(...)
	{
		CCTRACEE("SetSNStateInstall(): Unknown Error");
	}

    try
    {
        if(hSymNeti)
            FreeLibrary(hSymNeti);
    }
    catch(...)
    {
        CCTRACEE("SetSNStateInstall() : Unknown Error while trying to free handles.");
    }

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// PostInstall():
//	this export is called by SymSetup via the customaction table
//	it is responsible for taking care of anything that needs to be done
//	After the MSI is run
//
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall PostInstall()
{
	SetSNState(TRUE);

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
// PreUinstall():
//	this export is called by SymSetup via the customaction table
//	it is responsible for taking care of anything that needs to be done
//	After the MSI is run
//
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall PreUninstall()
{
	SetSNState(FALSE);

	return ERROR_SUCCESS;
}