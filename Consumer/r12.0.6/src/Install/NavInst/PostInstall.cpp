/////////////////////////////////////////////////////////////////////////////////////////////
// PreInstall.cpp
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
#include "NAVTasks.h"
#include "COEMOptions.h"
#include "DefAlertOptNames.h"
#include "autoupdatedefines.h"
#include "Qmigrate.h"
#include "APOptNames.h"
#include <time.h>
#include "NAVDefutilsLoader.h"
#include "AutoProtectWrapper.h"
#include "build.h"
#include "V2LicensingAuthGuids.h"
#include "exclusionfileloader.h"
#include "AVRES.h"
#include "StahlSoft.h"
#include "InstOsHelper.h"

// InstallToolBox headers
#include "NavDetection.h"
#include "instopts.h"
#include "fileaction.h"
#include "utilities.h"

#include "Quarantine_const.h"
#include "EmailProxyInterface.h"  // For Email options


using namespace InstallToolBox;


/////////////////////////////////////////////////////////////////////////////////////////////
//  SetOEMOptions(): Sets the various features on/off depending on the OEM options
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL SetOEMOptions()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetOEMOptions()", "Debug Break", NULL);
#endif

	g_Log.Log("SetOEMOptions() Starting.");

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
		
		//check for the uber no-drivers switch
		DWORD dwValue = 0;
		InstoptsDat.ReadDWORD(INSTOPTS_NODRIVERS, dwValue, 0);
		if(dwValue == 1)
		{
			// if the vendor used that then disable AP, EMI, EMO, and set SAVRT to manual
			OEMOptions.SetSAVRTPELManual();
            OEMOptions.TurnOnDelayLoad(false);
			OEMOptions.TurnOnAP(false);
			OEMOptions.TurnOnEMI(false);
			OEMOptions.TurnOnEMO(false);
			OEMOptions.SetCCServicesStates(false, false);
		}
		// if the vendor didn't use the uber no-drivers switch then set each option individually.
		else
		{
			// turn on CC services
			OEMOptions.SetCCServicesStates(true, true);

            // Need to set the delay load mode before enabling AP since 
            // the AP scriptable configure call is dependent on this value being
            // set
            dwValue = 1;
			InstoptsDat.ReadDWORD(INSTALL_EnableDelayLoad, dwValue, 0);
			OEMOptions.TurnOnDelayLoad(dwValue == 1? true : false);

			dwValue = 1;
			InstoptsDat.ReadDWORD(INSTALL_EnableAutoProtect, dwValue, 1);
			OEMOptions.TurnOnAP(dwValue == 1 ? true : false);

			dwValue = 1;
			InstoptsDat.ReadDWORD(INSTALL_EnableEmailOut, dwValue, 1);
			OEMOptions.TurnOnEMO(dwValue == 1 ? true : false);

			dwValue = 1;
			InstoptsDat.ReadDWORD(INSTALL_EnableEmailIn, dwValue, 1);
			OEMOptions.TurnOnEMI(dwValue == 1 ? true : false);
		}	
		
		// migrate the ThreatCat options
		DWORD dwFirst, dwLast = 0;
		InstoptsDat.ReadDWORD(INSTALL_FirstKnownNonViral, dwFirst, 0);
		InstoptsDat.ReadDWORD(INSTALL_LastKnownCategory, dwLast, 0);

		for(DWORD i = dwFirst; i <= dwLast; i++)
		{
			dwValue = 1;
			TCHAR szOptName[100] = {0};
			wsprintf(szOptName, "%s%d", INSTALL_EnableThreatBaseName, i);
			InstoptsDat.ReadDWORD(szOptName, dwValue, 1);
			OEMOptions.TurnOnThreatCat(i, dwValue == 1 ? true : false);
		}

		dwValue = 1;
		InstoptsDat.ReadDWORD(INSTALL_EnableMSNScanning, dwValue, 1);
		OEMOptions.TurnOnMSN(dwValue == 1 ? true : false);
	}
	catch(exception& ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in SetOEMOptions()"); 
	}

	//restore the current working dir
	SetCurrentDirectory(szCurDir);

	g_Log.Log("SetOEMOptions() Finished.");

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
	
	g_Log.Log("CleanNAVAPVXDKey() Starting.");
	
	HKEY hKey = NULL;
	
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("System\\CurrentControlSet\\Services\\VxD"), 0, KEY_ALL_ACCESS, &hKey))
	{
		if(ERROR_SUCCESS == RegDeleteKey(hKey, "NAVAP"))
			g_Log.Log("CleanNAVAPVXDKey() Found NAVAP StaticVxD key and deleted it");
		RegCloseKey(hKey);
	}

	g_Log.Log("CleanNAVAPVXDKey() Finished.");
	
	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//  SetInstallTime(): Record InstallTime to alert.dat
// 
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall SetInstallTime(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetInstallTime()", "Debug Break", NULL);
#endif

	g_Log.Log("SetInstallTime() Starting.");
	
	HNAVOPTS32 hDefAlert = NULL;

	try
	{
		// Get full path of alert.dat

		TCHAR szAlertFile[MAX_PATH] = {0};
		CNAVInfo NAVInfo;

		wsprintf(szAlertFile, _T("%s\\alert.dat"), NAVInfo.GetNAVCommonDir());

		NAVOPTS32_STATUS status = NAVOPTS32_OK;
		status = NavOpts32_Allocate(&hDefAlert);

		if(status != NAVOPTS32_OK)
			throw runtime_error("SetInstallTime: Unable to allocate NavOpts32.");

		#ifdef _UNICODE
			status = NavOpts32_LoadU(szAlertFile, hDefAlert, TRUE);
		#else
			status = NavOpts32_Load(szAlertFile, hDefAlert, TRUE);
		#endif

		if(status != NAVOPTS32_OK)
			throw runtime_error("SetInstallTime: Unable to load alert.dat.");

		long iTime = 0;
		time(&iTime);

		DWORD dwValue = static_cast<DWORD>(iTime);
		NavOpts32_SetDwordValue(hDefAlert, ALERT_InstallTime, dwValue);

		#ifdef _UNICODE
			NavOpts32_SaveU(szAlertFile, hDefAlert);			
		#else
			NavOpts32_Save(szAlertFile, hDefAlert);			
		#endif
	}
	catch(exception& ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("SetInstallTime: Unknown exception thrown");
	}

	try
	{
		if(hDefAlert)
			NavOpts32_Free(hDefAlert);
	}
	catch(...)
	{
		g_Log.LogEx("SetInstallTime: Free NavOpts failed.");
	}
	
	g_Log.Log("SetInstallTime() Finished.");

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

	g_Log.Log("SetSilentConfigWiz() Starting.");

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
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in SetSilentConfigWiz().");
	}
	
	g_Log.Log("SetSilentConfigWiz() Finished.");

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

	g_Log.Log("CopyProbe() Starting.");

	try
	{
		CNAVInfo NAVInfo;

		TCHAR szSourcePath[MAX_PATH] = {0};
		TCHAR szDestPath[MAX_PATH] = {0};
		
		GetTempPath(MAX_PATH, szSourcePath);
		PathAppend(szSourcePath, _T("ProbeGSE.dat"));

		_tcsncat(szDestPath, NAVInfo.GetSymantecCommonDir(), MAX_PATH);
		PathAppend(szDestPath, _T("VirusDefs\\ProbeGSE.dat"));
		
		CopyFile(szSourcePath, szDestPath, FALSE);
	}
	catch(...)
	{
		g_Log.LogEx("CopyProbe(): Unknown Error");
	}
	
	g_Log.Log("CopyProbe() Done.");

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  SetSAVRTValues(): copied Probegse.dat to the virusdefs directory
//
/////////////////////////////////////////////////////////////////////////////////////////////
bool SetSAVRTValues()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetSAVRTValues()", "Debug Break", NULL);
#endif

	g_Log.Log("SetSAVRTDefsPath() Starting.");
	
	DefUtilsLoader HawkingsLoader;
	try
	{
		CAutoProtectOptions SavrtOptions;
		if( SAVRT_OPTS_OK != SavrtOptions.Load())
			return false;

		IDefUtilsPtr pDefUtils;
				
		// load the Defutils object, get the defs dir, and set it
		if (SYM_SUCCEEDED(HawkingsLoader.CreateObject(pDefUtils.m_p)) && pDefUtils != NULL)
		{
			TCHAR szSAVRTDat[MAX_PATH] = {0};
			TCHAR szDefsDir[MAX_PATH] = {0};
			if(pDefUtils->InitWindowsApp("NAVUI") && pDefUtils->GetCurrentDefs(szDefsDir, MAX_PATH))
			{
				SavrtOptions.SetStringValue(AP_VirusDefinitionsPath, szDefsDir);
			}
		}

		// Look up the quarantine incoming dir and set it in the dat
		HKEY hkKey = NULL;
		HRESULT hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SZ_REGKEY_QUARANTINE, 0 , KEY_READ, &hkKey);
		if(SUCCEEDED(hr))
		{
			TCHAR szQuarIncomingPath[MAX_PATH] = {0};
			DWORD dwBuffLen = MAX_PATH;
			hr = RegQueryValueEx(hkKey, SZ_REGVALUE_QUARANTINE_INCOMING_PATH, NULL, NULL, (LPBYTE)szQuarIncomingPath, &dwBuffLen);
			if(ERROR_SUCCESS == hr)
			{
				SavrtOptions.SetStringValue(QUAR_IncomingPath, szQuarIncomingPath);
			}

			RegCloseKey(hkKey);
		}

		// find the HIDETray icon option in instopts.dat and set that one
		DWORD dwHideTrayIcon = 0;
		CInstoptsDat InstoptsDat;
		InstoptsDat.ReadDWORD(INSTOPTS_HideNAVTrayIcon, dwHideTrayIcon, 0);
		SavrtOptions.SetDwordValue(TSR_HideIcon, dwHideTrayIcon);
		
		// Grab NoThreatCat from instopts.dat and set RespondToThreats accordingly
		DWORD dwNoThreatCat = 0;
		if(InstoptsDat.ReadDWORD(INSTALL_NoThreatCat, dwNoThreatCat, 0))
		{
			if(1 == dwNoThreatCat)
			{
				// non-viral scanning is disabled
				SavrtOptions.SetDwordValue(AP_szNAVAPCFGdwRespondToThreats, SAVRT_THREAT_MODE_VIRAL_HEURISTIC);
			}
			else
			{
				SavrtOptions.SetDwordValue(AP_szNAVAPCFGdwRespondToThreats, SAVRT_THREAT_MODE_VIRAL_HEURISTIC
																		| SAVRT_THREAT_MODE_NONVIRAL);
			}
		}


		SavrtOptions.Save();


		// Copy SAVRT.DAT to SAVRT.DEF, the backup/default copy of the file
		CNAVInfo NAVInfo;

		TCHAR szSAVRTDAT[MAX_PATH] = {0};
		_tcscpy(szSAVRTDAT, NAVInfo.GetNAVDir());
		PathAppend(szSAVRTDAT, _T("SAVRT.DAT"));

		TCHAR szSAVRT_Default[MAX_PATH] = {0};
		_tcscpy(szSAVRT_Default, NAVInfo.GetNAVDir());
		PathAppend(szSAVRT_Default, _T("SAVRT.DEF"));

		if(FALSE == CopyFile(szSAVRTDAT, szSAVRT_Default, FALSE))
		{
			g_Log.Log(_T("%s - Failed to copy SAVRT.DAT to SAVRT.DEF. GetLastError=%d"), __FUNCTION__, GetLastError());
		}
	}
	catch(...)
	{
		g_Log.LogEx("SetSAVRTDefsPath(): Unknown Error");
	}
	
	g_Log.Log("SetSAVRTDefsPath() Done.");

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// QMigrate():
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL QMigrate()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug QMigrate()", "Debug Break", NULL);
#endif

	g_Log.Log("QMigrate() Starting.");

	CQMigrate QMigrate;
	HKEY hkKey = NULL;

	HRESULT hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SZ_REGKEY_QUARANTINE, 0 , KEY_READ, &hkKey);
	if(FAILED(hr))
		return ERROR_SUCCESS;

	TCHAR szQuarPath[MAX_PATH] = {0};
	unsigned long lBuffLen = MAX_PATH;
	hr = RegQueryValueEx(hkKey, SZ_REGVALUE_QUARANTINE_PATH, NULL, NULL, (unsigned char*)szQuarPath, &lBuffLen); 
	if(hr != ERROR_SUCCESS)
		return ERROR_SUCCESS;

	RegCloseKey(hkKey);

	// We need to set our current directory to the NAV directory so that all of our calls
	// will succeed since it needs various DLLs.
	TCHAR szCurDir[MAX_PATH] = {0};
	TCHAR szNAVPath[MAX_PATH] = {0};
	DWORD dwSize=MAX_PATH;
	CNAVInfo NavInfo;

	GetCurrentDirectory(dwSize, szCurDir);
	_tcscpy(szNAVPath, NavInfo.GetNAVDir());
	if (szNAVPath[0] != NULL)
		SetCurrentDirectory(szNAVPath);
	else
		return ERROR_SUCCESS;

	// Disable AP for this process
	QMigrate.EnableAutoProtect(false);
	
	QMigrate.Initialize(szQuarPath);
	QMigrate.UpdateOldFormatFiles();

	// Re-enable AP for this process
	QMigrate.EnableAutoProtect(true);

	// Set the current directory back to what it was before.
	SetCurrentDirectory(szCurDir);

	return ERROR_SUCCESS;

	g_Log.Log("QMigrate() Finished.");

	return ERROR_SUCCESS;
}

bool SetExclusions()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetExclusions()", "Debug Break", NULL);
#endif
    
    if(FAILED(CoInitialize(NULL)))
        return false;

    NavExclusions::CExclusionFileLoader FileLoader;
    CNAVInfo NAVInfo;
    TCHAR szPath[MAX_PATH] = {0};
    wsprintf(szPath, _T("%s\\%s"), NAVInfo.GetNAVDir(), EXCLUSIONS_DEFAULTS_FILENAME_TCHAR);
    
    FileLoader.initialize(szPath);
    FileLoader.LoadAndFilter(NavExclusions::IExclusion::ExclusionState::NOT_EXCLUDED,
                                NavExclusions::IExclusion::ExclusionType::INVALID);
    FileLoader.uninitialize();
    
    CoUninitialize();

    return true;
}


// Modifies registry with Product Name obtained from AVRES.dll
BOOL SetProductName()
{
#ifdef _MSG_BOX
	MessageBox(NULL, "SetProductName()", "Attach", NULL);
#endif
	// Set up path to AVRES.DLL
	ATL::CString sAVRES;
	GetModuleFileName(GetModuleHandle(NULL), sAVRES.GetBufferSetLength(MAX_PATH), MAX_PATH);
	sAVRES.ReleaseBuffer();
	PathRemoveFileSpec(sAVRES.GetBuffer());
	sAVRES.ReleaseBuffer();
	PathAddBackslash(sAVRES.GetBuffer());
	sAVRES.ReleaseBuffer();

	sAVRES += _T("NAV\\External\\Norton\\AVRES.DLL");

	StahlSoft::CSmartModuleHandle shMod(LoadLibraryEx(sAVRES, NULL, LOAD_LIBRARY_AS_DATAFILE) );
	if((HANDLE) shMod != (HANDLE)NULL)
	{
		ATL::CString sProductName;
		sProductName.LoadString(shMod, IDS_AV_PRODUCT_NAME);
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

	ATL::CRegKey InstallAppsKey;
	DWORD dwSize = MAX_PATH;
	TCHAR szOLDNAV[MAX_PATH] = {0};
	UINT ur = InstallAppsKey.Open(HKEY_LOCAL_MACHINE, szSymAppsPath);
	if(ERROR_SUCCESS == ur)
	{
		ur = InstallAppsKey.QueryStringValue(_T("OLDNAV"), szOLDNAV, &dwSize);
		if(ERROR_SUCCESS == ur)
		{
			// migrate the format of the quarantine files
			QMigrate();

			ATL::CRegKey NAVKey;
			DWORD dwValue = 0;
			ur = NAVKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"));
			if(ERROR_SUCCESS == ur)
			{
				// if we're supposed to migrate options
				ur = NAVKey.QueryDWORDValue(_T("DontMigrate"), dwValue);
				if(ERROR_SUCCESS != ur)
				{
					ATL::CString sPath;
					ATL::CString sOmigrate;

					GetModuleFileName(GetModuleHandle(NULL), sPath.GetBuffer(MAX_PATH), MAX_PATH);
					sPath.ReleaseBuffer();

					sPath = sPath.Left(sPath.ReverseFind(_T('\\')) + 1);
					sOmigrate.Format(_T("\"%s\\NAV\\OMigrate.exe\""), sPath);
                    
					ur = InstallToolBox::ExecuteProgram(sOmigrate, _T(""), 1);

                    if(ERROR_SUCCESS != ur)
					{
						CCTRACEE(_T("PostInstall(): Error executing %s (0x%08x)"), sOmigrate, ur);
					}
				}
				else
				{
					CCTRACEI(_T("PostInstall(): DontMigrate key exists"));
				}
			}
			else
			{
				CCTRACEE(_T("PostInstall(): Open NAV key failed (0x%08x)"), ur);
			}

			InstallAppsKey.DeleteValue(_T("OLDNAV"));
		}
	}
	else
	{
		CCTRACEE("PostInstall(): couldn't open %s (0x%08x)", szSymAppsPath, ur);
	}
		
	// we need to call setOEMOptions here because we need to be signed and navsetup.exe is signed
	// whereas msiexec.exe isn't
	CCTRACEI(_T("PostInstall(): SetOEMOptions"));
	SetOEMOptions();

    // install the default exclusions to ccsettings.
	CCTRACEI(_T("PostInstall(): SetExclusions"));
	SetExclusions();

	CCTRACEI(_T("PostInstall(): SetProductName"));
	SetProductName();

	// we need to call SetSAVRTValues here because we need to be signed to load savrtdll
	// Call this after SetOEMOptions so we pick up all modified options 
	// when we make a copy of SAVRT.DAT to create the default file.
	CCTRACEI(_T("PostInstall(): SetSAVRTValues"));
	SetSAVRTValues();

	CCTRACEI(_T("PostInstall(): SetDefaultEmailOptions"));
	SetDefaultEmailOptions();

    // Turn off trust system wide for performance - defect 1-4DB0G8
    DisableGlobalTrustPolicy ();

	CCTRACEI(_T("PostInstall(): Done"));
	return ERROR_SUCCESS;
}