////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// PostInstall.cpp
//
// --Contains exported functions used in MSI custom actions for performing any post-install
// tasks after MSI has copied the files
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "helper.h"
#include "NavCommandLines.h"
#include "InstOptsNames.h"
#include "resource.h"
#include "OSInfo.h"
#include "COEMOptions.h"
#include "DefAlertOptNames.h"
#include "autoupdatedefines.h"
#include <time.h>
#include "NAVDefutilsLoader.h"
#include "build.h"
#include "V2LicensingAuthGuids.h"
#include "exclusionfileloader.h"
#include "isResource.h"
#include "StahlSoft.h"
#include "InstOsHelper.h"

// InstallToolBox headers
#include "installtoolbox.h"
#include "NavDetection.h"
#include "instopts.h"
#include "fileaction.h"
#include "utilities.h"
#include "CustomAction.h"

//cc
#include "ccSplitPath.h"
#include "ccSymInstalledApps.h"

#include "EmailProxyInterface.h"  // For Email options
using namespace InstallToolBox;

HRESULT EnableAppCore()
{
	HRESULT   hr       = S_OK;
	SC_HANDLE hScm     = NULL;
	SC_HANDLE hService = NULL;
	SERVICE_STATUS status = { 0 };
	
	try 
	{
		// open sc manager and service
		if (NULL == (hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT )))
		{
			hr = ::GetLastError();
			CCTRACEE(_T("Failed to open scm Manager (0x%08X)"), hr);
			throw HRESULT_FROM_WIN32( hr );
		}
		if (NULL == (hService = OpenService(hScm, _T("SymAppCore"), 
								SERVICE_START | SERVICE_QUERY_STATUS )))
		{
			hr = ::GetLastError();
			CCTRACEE(_T("Failed to open service (0x%08X)"), hr);
			throw HRESULT_FROM_WIN32( hr );
		}

		// Make sure the service isn't already running
		if (!QueryServiceStatus(hService, &status))
		{
			throw HRESULT_FROM_WIN32( ::GetLastError() );
		}
		// Service is already running, we are done
		if( SERVICE_RUNNING == status.dwCurrentState )
		{
			// Clean up and exit
			::CloseServiceHandle( hService );
			::CloseServiceHandle( hScm );
			return S_OK;
		}

		// attempt to start service
		if (!::StartService(hService, 0, NULL))
		{
			throw HRESULT_FROM_WIN32( ::GetLastError() );	
		}
		if (!QueryServiceStatus(hService, &status))
		{
			throw HRESULT_FROM_WIN32( ::GetLastError() );
		}

		// Wait for the service to start
		DWORD dwWaitTime = 0;
		DWORD dwStartTickCount = GetTickCount();
		DWORD dwOldCheckpoint = status.dwCheckPoint;
		while ( SERVICE_RUNNING != status.dwCurrentState )
		{
			// reduce sleep time given by dwWaitHint
			dwWaitTime = status.dwWaitHint / 10;
			if (dwWaitTime < 1000) 
			{
				dwWaitTime = 1000; 
			}
			else if (10000 < dwWaitTime) 
			{
				dwWaitTime = 10000;
			}

			Sleep(dwWaitTime);

			if (!QueryServiceStatus(hService, &status))
			{
				throw HRESULT_FROM_WIN32( ::GetLastError() );
			}

			if (dwOldCheckpoint < status.dwCheckPoint)
			{
				dwStartTickCount = GetTickCount();
				dwOldCheckpoint = status.dwCheckPoint;
			}
			else
			{
				if(status.dwWaitHint < GetTickCount() - dwStartTickCount)
				{
					break;
				}
			}
		}
	}
	catch (HRESULT& err)
	{
		hr = err;
	}
	catch (_com_error& err)
	{
		hr = err.Error();
	}
	if (hService)
	{
		::CloseServiceHandle(hService);
	}
	if (hScm)
	{
		::CloseServiceHandle(hScm);
	}

	if (hr != S_OK)
	{
		CCTRACEE(_T("EnableAppCore() (0x%08X)"), hr);
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  SetOEMOptions(): Sets the various features on/off depending on the OEM options
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL SetOEMOptions()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetOEMOptions()", "Debug Break", NULL);
#endif

	CCTRACEI(_T("SetOEMOptions() Starting."));

	TCHAR szCurDir[MAX_PATH] = {0};
	TCHAR szNavDir[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szNavDir);
	COEMOptions OEMOptions;
	HKEY hKey = NULL;
	
	//The options library DELAYLOAD on ccTrust will fail unless the working dir is the NAV dir
	//so save the current working dir and restore it later
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)szNavDir, &dwSize);
		GetCurrentDirectory(MAX_PATH, szCurDir);
		SetCurrentDirectory(szNavDir);
	}

	try
	{
		if (!OEMOptions.Initialize())
			throw runtime_error("Unabled to Initialize OEMOptions Class");
		
		CInstoptsDat InstoptsDat;
		
		CNAVInfo NAVInfo;
		TCHAR szNavOptsDat[MAX_PATH] = _T("");
		wsprintf(szNavOptsDat, _T("%s\\NAVOpts.dat"), NAVInfo.GetNAVDir());
		CInstoptsDat NavOptsDat(NULL, szNavOptsDat, TRUE);
		NavOptsDat.TurnOffReadOnly();

		DWORD dwValue = 0;
		InstoptsDat.ReadDWORD(INSTALL_EnableAutoProtect, dwValue, 1);
		OEMOptions.TurnOnAutoProtect(dwValue == 1 ? true : false);

		//check for the uber no-drivers switch
		dwValue = 0;
		InstoptsDat.ReadDWORD(INSTOPTS_NODRIVERS, dwValue, 0);
		if(dwValue == 1)
		{
			// if the vendor used that then disable AP, EMI, EMO, and disable early load
			OEMOptions.TurnOnAutoProtect(false);
            OEMOptions.TurnOnAPEarlyLoad(false);
			OEMOptions.TurnOnEMI(false);
			OEMOptions.TurnOnEMO(false);
			NavOptsDat.WriteDWORD(AUTOPROTECT_DriversSystemStart, 0);
		}
		// if the vendor didn't use the uber no-drivers switch then set each option individually.
		else
		{
            dwValue = 1;
			InstoptsDat.ReadDWORD(INSTALL_EnableDelayLoad, dwValue, 1); // FLIPPED ORIGINAL DEFINITION, 0 = Off, 1 = On
			OEMOptions.TurnOnAPEarlyLoad(dwValue == 0? true : false);   // This is analogous to AUTOPROTECT_DriversSystemStart
			// Write correct (INSTALL_EnableDelayLoad flipped) AUTOPROTECT_DriversSystemStart value
			NavOptsDat.WriteDWORD(AUTOPROTECT_DriversSystemStart, dwValue == 0? 1 : 0);

			dwValue = 1;
			InstoptsDat.ReadDWORD(INSTALL_EnableEmailOut, dwValue, 1);
			OEMOptions.TurnOnEMO(dwValue == 1 ? true : false);

			dwValue = 1;
			InstoptsDat.ReadDWORD(INSTALL_EnableEmailIn, dwValue, 1);
			OEMOptions.TurnOnEMI(dwValue == 1 ? true : false);
		}	
		
		// migrate the ThreatCat options
		DWORD dwFirst, dwLast = 0;
		NavOptsDat.ReadDWORD(THREAT_FirstKnownNonViral, dwFirst, 0);
		NavOptsDat.ReadDWORD(THREAT_LastKnownCategory, dwLast, 0);

		for(DWORD i = dwFirst; i <= dwLast; i++)
		{
			OEMOptions.DefaultThreatCat(i);
		}

		NavOptsDat.Save();
	}
	catch(_com_error &ce)
	{
		CCTRACEE(_T("_com_error exception in SetOEMOptions(). (0x%08X)"), ce.Error());
	}
	catch(exception& ex)
	{
		CCTRACEE(_T("%s"), ex.what());
	}

	//restore the current working dir
	SetCurrentDirectory(szCurDir);

	CCTRACEI(_T("SetOEMOptions() Finished."));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  CleanNAVAPVXDKey():  NAV2003 and later no longer uses the NAVAP.vxd, but on some machines
//		in the field, this driver is still present and being loaded.  We need to remove the key
//		so it does not get loaded and conflict with SAVRT.VXD.  (defect 403520) CODAVIS    
// 
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall CleanNAVAPVXDKey(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug CleanNAVAPVXDKey()", "Debug Break", NULL);
#endif
	
	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("CleanNAVAPVXDKey() Starting."));
	
	HKEY hKey = NULL;
	
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("System\\CurrentControlSet\\Services\\VxD"), 0, KEY_ALL_ACCESS, &hKey))
	{
		if(ERROR_SUCCESS == RegDeleteKey(hKey, "NAVAP"))
		{
			InstallToolBox::CCustomAction::LogMessage(hInstall,
				InstallToolBox::CCustomAction::LogSeverity::logStatus,
				_T("CleanNAVAPVXDKey() Found NAVAP StaticVxD key and deleted it"));
		}
		RegCloseKey(hKey);
	}

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("CleanNAVAPVXDKey() Finished."));
	
	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  SetSilentConfigWiz(): Set ConfigWiz to run Silently
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall SetSilentConfigWiz(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetSilentConfigWiz()", "Debug Break", NULL);
#endif

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("SetSilentConfigWiz() Starting."));

	TCHAR szInstallType[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	HKEY hKey;

	try
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szNAVAppsKey, REG_OPTION_NON_VOLATILE, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if(RegQueryValueEx(hKey, _T("SuiteInstall"), NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
			{
				_tcscpy(szInstallType, _T("1"));			
			}
			
			RegCloseKey(hKey);
		}
		
		if (atoi(szInstallType) == 1)
		{
		
			// Get the path to CfgWiz.dat
			TCHAR szInstallDir[MAX_PATH] = {0};
			TCHAR szCfgWiz[MAX_PATH]= {0};
			dwSize = MAX_PATH;
			HKEY hKey;

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, REG_OPTION_NON_VOLATILE, KEY_READ, &hKey) == ERROR_SUCCESS)
			{		
				if(RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)szInstallDir, &dwSize) != ERROR_SUCCESS)
				{					
					throw runtime_error("Unable to find NAV Install key value.");
				}	
				
				RegCloseKey(hKey);
			}
			else
				throw runtime_error("Unable to find NAV Install key.");
		
			wsprintf(szCfgWiz, "%s\\CfgWiz.dat", szInstallDir);
		
			////////////////// Write to CfgWiz.dat ///////////////////
			CInstoptsDat CfgWizDat(hInstall, szCfgWiz, TRUE);
            
			CfgWizDat.TurnOffReadOnly();
			CfgWizDat.WriteDWORD(CFGWIZ_SilentMode, 1);
			CfgWizDat.Save();
		}
	}

	catch(exception& ex)
	{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logError,
			_T("%s"), ex.what());
	}

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("SetSilentConfigWiz() Finished."));

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  CopyProbe(): copied Probegse.dat to the virusdefs directory
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall CopyProbe(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug CopyProbe()", "Debug Break", NULL);
#endif

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("CopyProbe() Starting."));

	CNAVInfo NAVInfo;

	TCHAR szSourcePath[MAX_PATH] = {0};
	TCHAR szDestPath[MAX_PATH] = {0};
	
	GetTempPath(MAX_PATH, szSourcePath);
	PathAppend(szSourcePath, _T("ProbeGSE.dat"));

	_tcsncat(szDestPath, NAVInfo.GetSymantecCommonDir(), MAX_PATH);
	PathAppend(szDestPath, _T("VirusDefs\\ProbeGSE.dat"));
	
	CopyFile(szSourcePath, szDestPath, FALSE);

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("CopyProbe() Done."));

	return ERROR_SUCCESS;
}

// Modifies registry with Product Name obtained from AVRES.dll
BOOL SetProductName()
{
#ifdef _MSG_BOX
	MessageBox(NULL, "SetProductName()", "Attach", NULL);
#endif
	// Set up path to AVRES.DLL
	ATL::CString sISRES;
	GetModuleFileName(GetModuleHandle(NULL), sISRES.GetBufferSetLength(MAX_PATH), MAX_PATH);
	sISRES.ReleaseBuffer();
	PathRemoveFileSpec(sISRES.GetBuffer());
	sISRES.ReleaseBuffer();
	PathAddBackslash(sISRES.GetBuffer());
	sISRES.ReleaseBuffer();

	sISRES += _T("NAV\\Parent\\NORTON\\App\\ISRES.DLL");

	StahlSoft::CSmartModuleHandle shMod(LoadLibraryEx(sISRES, NULL, LOAD_LIBRARY_AS_DATAFILE) );
	if((HANDLE) shMod != (HANDLE)NULL)
	{
		ATL::CString sProductName;
		sProductName.LoadString(shMod, IDS_BRANDING_PRODUCT_NAME);
#ifdef _MSG_BOX
		MessageBox(NULL, sProductName, "SetProductName", NULL);
#endif
		if(!sProductName.IsEmpty())
		{
			SetRegProductName("ProductName",sProductName);
			return TRUE;
		}
	}
	return FALSE;
}

//Set SSL warning messages to off by default.
// 1-4FWY19
BOOL SetDefaultEmailOptions(void)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetDefaultEmailOptions()", "Debug Break", NULL);
#endif // _DEBUG

	HINSTANCE hccEmlPxy;
	IEmailOptions* pEmailOptions = NULL; 
	ISymFactory* pFactory = NULL;
	pfnGETFACTORY pGetFactory = NULL;
	BOOL bRet = false;
	SYMOBJECT_ID ObjectID;
	ccLib::CString szEmlPxyPath;

	ccSym::CInstalledApps::GetCCDirectory( szEmlPxyPath );
	szEmlPxyPath.Append(_T("\\ccEmlPxy.dll"));

	hccEmlPxy = LoadLibraryEx(szEmlPxyPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if ( hccEmlPxy == NULL)
	{
		return false;
	}

	// Get the options object ID.
	pfnGETEMAILOPTIONSOBJECTID pGetEmailOptionsObjectID =
		(pfnGETEMAILOPTIONSOBJECTID) GetProcAddress(hccEmlPxy, "GetEmailOptionsObjectID");

	if ( pGetEmailOptionsObjectID != NULL && pGetEmailOptionsObjectID(&ObjectID) == SYM_OK)
	{
		// Create the options object using its factory.
		pGetFactory = (pfnGETFACTORY) GetProcAddress(hccEmlPxy, "GetFactory");
		if ( pGetFactory != NULL && pGetFactory(ObjectID, &pFactory) == SYM_OK)
		{
			if ((pFactory->CreateInstance(IID_EmailOptions, (void**) &pEmailOptions) == SYM_OK) && pEmailOptions)
			{
				bRet = true;
			}
		}
	}

	if( bRet )
	{   
		pEmailOptions->SetValue(CCEMLPXY_OUTGOING_SSL_MESSAGE, false);
		pEmailOptions->SetValue(CCEMLPXY_INCOMING_SSL_MESSAGE, false);		
		pEmailOptions->Release();
		pEmailOptions = NULL;
	}
	
	if( pFactory )
	{
		pFactory->Release();
		pFactory = NULL;
	}
	if(hccEmlPxy)
	{
		::FreeLibrary(hccEmlPxy);
		hccEmlPxy = NULL;
	}
	return bRet;
}

// Turn off trust system wide for performance - defect 1-4DB0G8
//
void DisableGlobalTrustPolicy ()
{
    ccVerifyTrust::IVerifyTrustPolicyPtr pTrustPolicy;
    ccVerifyTrust::ccVerifyTrust_IVerifyTrustPolicy TrustPolicyLoader;

    SYMRESULT symRes = TrustPolicyLoader.CreateObject ( &pTrustPolicy );

    if (SYM_FAILED(symRes) ||
        pTrustPolicy == NULL)
    {
        assert(pTrustPolicy);
        return;
    }

    // turn off trust
    pTrustPolicy->SetEnabled (false);
}

//Unregister the NSW One Button Check COM object that talks to NAV
//NAV 2007 cannot interface with NSW 2006's COM object - NAVPreC.dll
void UnRegisterNSW_NAVPreC()
{
	CString sRegSvr32Path;
	//c:\windows\system32\regsve32.exe
	if(ccLib::CSplitPath::GetSystemDirectory(sRegSvr32Path))
	{
		sRegSvr32Path += _T("\\regsvr32.exe");

		//"c:\program files\common files\symantec shared\navprec.dll"
		ccLib::CString sCCDirectory;
		if(ccSym::CInstalledApps::GetCCDirectory(sCCDirectory))
		{
			sCCDirectory.Append(_T("\\navprec.dll\""));
			sCCDirectory.Insert(0,_T("\""));

			CString sParms = _T("/u /s ");
			sParms += sCCDirectory;

			HRESULT hr = InstallToolBox::ExecuteProgram(sRegSvr32Path, sParms,  FALSE);
			if(FAILED(hr))
			{
				CCTRACEE(CCTRCTX _T("ExecuteProgram( %s %s) Failed. HR = 0x%x"),sRegSvr32Path,sParms,hr);
			}
		}
	}
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
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug PostInstall()", "Debug Break", NULL);
#endif // _DEBUG
	
	if(FAILED(EnableAppCore()))
	{
		return ERROR_SUCCESS;
	}

	// we need to call setOEMOptions here because we need to be signed and navsetup.exe is signed
	// whereas msiexec.exe isn't
	CCTRACEI(_T("PostInstall(): SetOEMOptions"));
	SetOEMOptions();

	CCTRACEI(_T("PostInstall(): SetProductName"));
	SetProductName();

	CCTRACEI(_T("PostInstall(): SetDefaultEmailOptions"));
	SetDefaultEmailOptions();

    // Turn off trust system wide for performance - defect 1-4DB0G8
    DisableGlobalTrustPolicy ();

	UnRegisterNSW_NAVPreC();

	CCTRACEI(_T("PostInstall(): Done"));
	return ERROR_SUCCESS;
}
