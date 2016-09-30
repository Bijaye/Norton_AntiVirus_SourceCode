#include "stdafx.h"

#include "LiveSubscribe.h"
#include "CommonStructs.h"
#include "SessionFile.h"
#include "helper.h"
#include "FileAction.h"
#include "utilities.h"
#include "shlwapi.h"
#include "InstOptsNames.h"
#include "PathValidation.h"

// for CPDOEM functionality
#include "hrx.h"
#include "SmartModuleHandle.h"
#define SIMON_INITGUID
#include <simon.h>
#include "CPDOEM.h"

using namespace InstallToolBox;

extern "C" __declspec(dllexport) UINT __stdcall GetCipherInfo(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug GetCipherInfo()", "Debug Break", NULL);
#endif

	try
	{
		HRESULT hr = S_OK;
		StahlSoft::CSmartModuleHandle shModule;
		SIMON::CSimonPtr<IOEMInfoAll> spInfo;
		StahlSoft::HRX hrx;
		TCHAR szSourceDir[MAX_PATH] = {0};
		TCHAR szVendorID[32 + 2] = {0};
		TCHAR szVendorTag[32 + 2] = {0};
		DWORD dwSize = 0;
		DWORD dwValue = 0;
		CInstoptsDat Instopts;

		//run the following ciper code if we are OEM(1), CTO(2) or PPO(3)
		Instopts.ReadDWORD(INSTOPTS_OEM, dwValue, 0);
		if ( 0 != dwValue )
		{
			// make sure the both the VendorID and VendorTag exists
			Instopts.ReadString(INSTOPTS_CipherID, szVendorID, sizeof(szVendorID), _T("-1"));
			Instopts.ReadString(INSTOPTS_CipherTag, szVendorTag, sizeof(szVendorID), _T("-1"));

			if ( (0 != (_tcsicmp(szVendorID, _T("-1")))) && (0 != (_tcsicmp(szVendorTag, _T("-1")))) )
			{
				// get the path to the OEM dll (current MSI location)
				dwSize = sizeof(szSourceDir);
				MsiGetProperty(hInstall, _T("SourceDir"), szSourceDir, &dwSize);
				
				// HP doesnt know how to set path environment variables
				// so we need to carry their dll for them and set the current
				// working directory to where it is located.  only do this
				// if we are able to get the original directory
				TCHAR szCurrentDir[MAX_PATH] = {0};
				bool bGotDir = false;
				bGotDir = (0 != GetCurrentDirectory(MAX_PATH, szCurrentDir));
				g_Log.Log("GetCipherInfo: Getting Current directory: %s , and save it for restoration later", szCurrentDir);
				
				if(bGotDir)
				{
					g_Log.Log("GetCipherInfo: Setting Current directory to source dir: %s, in order to load hpqnt.dll", szSourceDir);
					SetCurrentDirectory(szSourceDir);
				}
				
				PathAppend(szSourceDir, _T("CPDOEM.DLL"));
				g_Log.Log("Location of CPDOEM.dll: %s", szSourceDir);

				// load the OEM dll
				shModule = ::LoadLibrary(szSourceDir);
				g_Log.Log("LoadLibrary(cpdoem.dll)");
				hrx << (((HANDLE)shModule == NULL) ? E_FAIL : S_OK);
				g_Log.Log("GetCipherInfo: SimonCreateInstanceByDLLInstance(...)");

				// get the Service Tag from the OEM dll
				hrx << SIMON::CreateInstanceByDLLInstance(shModule, CLSID_COEMInfoAll, IID_IOEMInfoAll, (void**)&spInfo);
				g_Log.Log("GetOEMVendorTag(...)");
				hrx << spInfo->GetOEMVendorTag(szVendorID, szVendorTag, sizeof(szVendorTag));
				g_Log.Log(("szVendorID=%s szVendorTag=%s"), szVendorID, szVendorTag);

				// restore the current working dir to what it was
				if(bGotDir)
				{
					g_Log.Log("GetCipherInfo: Setting Current directory back to %s", szCurrentDir);
					SetCurrentDirectory(szCurrentDir);
				}


				Instopts.TurnOffReadOnly();
				Instopts.WriteString(INSTOPTS_CipherTag, szVendorTag);
				if (!Instopts.Save())
					g_Log.Log("GetCipherInfo: Failed to save Instopts.dat");
			}
			else
			{
				g_Log.Log("GetCipherInfo: Invalide INSTOPTS_CipherID/INSTOPTS_CipherTag");
			}
		}
		else
		{
			g_Log.Log("GetCipherInfo: we don't need Ciper functionality in this layout");
		}
	}
	catch (...)
	{
		g_Log.LogEx("GetCipherInfo: Unknown exception.");
	}

	return ERROR_SUCCESS;

}