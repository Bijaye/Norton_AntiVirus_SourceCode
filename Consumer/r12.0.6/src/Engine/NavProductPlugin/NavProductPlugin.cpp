// NavProductPlugin.cpp: implementation of the CNavProductPlugin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccAppPlugin.h"
#include "NavProductPlugin.h"
#include "navinfo.h"
#include "shellapi.h"
#include "const.h"
#include "resource.h"
#include "AVccModuleId.h"
#include "NAVError.h"
#include "ccEventManagerHelper.h"
#include "ccRegistry.h"
#include "ccosinfo.h"
#include "StahlSoft.h"
#include "..\navprodres\resource.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NavProductPlugin"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

#include "ccResourceLoader.h"
#include "..\NAVProdRes\resource.h"
extern cc::CResourceLoader g_ResLoader;

extern HINSTANCE g_hInstance;
//////////////////////////////////////////////////////////////////////
// Plugin DLL names.  
// TODO: These should probably be constants. 

char* CNavProductPlugin::m_sNAVPluginDllNames[] = 
{
	"navoptrf.dll",
	"navapw32.dll",
	"defalert.dll",
	"ccimscan.dll",
	"statushp.dll", // Status Helper
	NULL
};

char* CNavProductPlugin::m_sNAVOptionalPluginDllNames[] =
{
	"IWP\\iwp.dll",              // Intenet worm protection - might not be installed.
    "HPP32.dll",                 // Home Page Protection - might not be installed.
	NULL
};

char* CNavProductPlugin::m_sCommonPluginDllNames[] = 
{
	NULL
};

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::CNavProductPlugin()

CNavProductPlugin::CNavProductPlugin() 
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::~CNavProductPlugin()

CNavProductPlugin::~CNavProductPlugin()
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::VerifyProduct()

SYMRESULT CNavProductPlugin::VerifyProduct()            
{
	try
	{
		// TODO: Add code to verify the correct installation/configuration
		// of the product.  Note that the verification code needs to be as
		// fast as possible, since this code is called synchronously by 
		// ccApp.


		if(areNAVFilesRemoved())
		{

			// Get the path to Windows system directory.
			TCHAR szWindowSysPath[MAX_PATH] = {0};
			HRESULT hRet = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, szWindowSysPath);
			if (hRet == S_OK)
			{
				// Get the NAV GUID to verify if we are under a suite or as a standalone.
				// If we are a standalone, we will open the NAV uninstall. If we are under
				// a suite, we will launch the Add/Remove wizard, so the user will uninstall
				// through NIS or NSW.
				getNAVGUID();

				// Check if NAV is under a suite.
				int iResult = _tcscmp(m_sNAVGUID, _T("SUITE"));
				if (iResult == 0)
				{			
					// Report failure to start.
					reportFatalError(IDS_START_FAILURE);

					TCHAR szAddRemovePrgm[MAX_PATH] = {0};
					_tcscpy(szAddRemovePrgm, szWindowSysPath);
					_tcscat(szAddRemovePrgm, _T("\\appwiz.cpl"));

					ShellExecute(NULL, "cplopen", szAddRemovePrgm, NULL, NULL, SW_SHOWNORMAL);			
				}
				else
				{
					// Report failure to start.
					reportFatalError(IDS_START_FAILURE_STANDALONE);

					TCHAR szMsiExecPath[MAX_PATH] = {0};
					TCHAR szCommandline[MAX_PATH] = {0};

					_tcscpy(szMsiExecPath, szWindowSysPath);
					_tcscat(szMsiExecPath, _T("\\msiexec.exe"));

					wsprintf(szCommandline, _T("/i%s"), m_sNAVGUID);
					ShellExecute(NULL, NULL, szMsiExecPath, szCommandline, NULL, SW_SHOWNORMAL);
				}
			}

			CCTRACEE("navproduct plug-in returning SYMERR_CCAPP_BAD_INSTALL");
			return SYMERR_CCAPP_BAD_INSTALL;
		}

		// Start the Event Manager Service on NT platforms in case it was set to
		// Manual mode for OEM installations
        if ( !ccEvtMgr::CEventManagerHelper::IsEventManagerActive() )
        {
		    ccLib::COSInfo osInfo;
		    if( osInfo.IsWinNT() )
		    {
			    SC_HANDLE  hSCMgr;     // Handle to the ServiceControl Manager
			    SC_HANDLE  hccEvtMgr;  // Handle to the ccEvtMgr Service

			    hSCMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

			    if (NULL != hSCMgr && NULL != (hccEvtMgr = OpenService(hSCMgr, _T("ccEvtMgr"), SERVICE_ALL_ACCESS)))
			    {
				    // Check if the Event Manager is set to Manual before attempting to start it
				    LPQUERY_SERVICE_CONFIG lpServiceConfig;

				    // Allocate a buffer for the configuration information. 
				    if( lpServiceConfig = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LPTR, 4096) )
				    {
					    DWORD dwBytesNeeded = 0;
					    bool bGotConfig = false;

					    if( QueryServiceConfig(hccEvtMgr, lpServiceConfig, 4096, &dwBytesNeeded) )
						    bGotConfig = true;
					    else if( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
					    {
						    // Need a bigger buffer
						    LocalFree(lpServiceConfig);
						    lpServiceConfig = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LPTR, dwBytesNeeded);

						    if( QueryServiceConfig(hccEvtMgr, lpServiceConfig, dwBytesNeeded, &dwBytesNeeded) )
							    bGotConfig = true;
					    }

					    if( bGotConfig )
					    {
						    // Check if it's Manual Start
						    if( SERVICE_DEMAND_START == lpServiceConfig->dwStartType )
						    {
							    // Start the Service since it's set to manual
							    StartService(hccEvtMgr, 0, NULL);
						    }
					    }

					    // Free the configuration information buffer
					    LocalFree(lpServiceConfig);
				    }

				    CloseServiceHandle(hccEvtMgr);
			    }

			    CloseServiceHandle(hSCMgr);
		    }

		    // Wait for the Event Manager Service to start

		    // Look if the registry has an updated time value
		    static const LPCTSTR szRegKey = _T("Software\\Symantec");
		    static const LPCTSTR szRegValue = _T("NAVStartupDelay");
		    static const long nMinWait = 3 * 1000 * 60;
		    static const long nMaxWait = 10 * 1000 * 60;
		    DWORD dwWait = nMinWait;
		    ccLib::CRegistry reg;

		    if (reg.Open(HKEY_LOCAL_MACHINE, 
			    szRegKey,
			    KEY_READ,
			    FALSE) != FALSE &&
			    reg.GetNumber(szRegValue, dwWait) != FALSE)
		    {
			    CCTRACEI("CNavProductPlugin::VerifyProduct() : NavStartupDelay registry value: %lu.", dwWait);            
		    }

		    if (dwWait < nMinWait)
		    {
			    dwWait = nMinWait;
		    }
		    if (dwWait > nMaxWait)
		    {
			    dwWait = nMaxWait;
		    }

		    const long nTest = 1000;
		    const long nCount = dwWait / nTest;
		    long i = 0;
		    BOOL bOk = FALSE;

		    for (i = 0; i < nCount; i ++)
		    {
			    if (ccEvtMgr::CEventManagerHelper::IsEventManagerActive() == FALSE)
			    {
				    Sleep(nTest);
			    }
			    else
			    {
				    bOk = TRUE;
				    break;
			    }
		    }

		    if (bOk == FALSE)
		    {
			    CCTRACEE("CNavProductPlugin::VerfiyProduct() - bOK is False, returning sym_false");
			    return SYM_FALSE;
		    }
        }

		// Now that we have verified that NAV is installed properly, 
		// build our list of plugins.
		buildPluginList();

		// All is well.
		return SYM_OK;

	}
	catch(...)
	{
		CCTRACEE("nav product plug-in VerfiyProduct() caught an exception");
	}

	return SYMERR_CCAPP_BAD_INSTALL;
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::GetPluginDLLCount()

unsigned int CNavProductPlugin::GetPluginDLLCount()
{
	try
	{
		return m_vPlugins.size();
	}
	catch(...)
	{
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::GetPluginDLLPath()

const char * CNavProductPlugin::GetPluginDLLPath( unsigned int uIndex )
{
	try
	{
		return m_vPlugins.at( uIndex ).c_str();
	}
	catch(...)
	{
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::CNavProductPlugin()

const char * CNavProductPlugin::GetProductName()
{
	return "Norton AntiVirus";
}


//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::buildPluginList()

void CNavProductPlugin::buildPluginList()
{
	//
	// Add NAV plugins 
	//
	int i;
	CNAVInfo navInfo;
	for( i = 0; m_sNAVPluginDllNames[i] != NULL; i++ )
	{
		::std::string sPluginPath = navInfo.GetNAVDir();
		sPluginPath += '\\';
		sPluginPath += m_sNAVPluginDllNames[i];
		m_vPlugins.push_back( sPluginPath );
	}

	// Add optional plugins.
	// If we tell ccApp to load a plug-in that doesn't exist
	// it will display an error. These are optional
	// plug-ins that might not exist depending on the SKU.
	//
	for( i = 0; m_sNAVOptionalPluginDllNames[i] != NULL; i++ )
	{
		::std::string sPluginPath = navInfo.GetNAVDir();
		sPluginPath += '\\';
		sPluginPath += m_sNAVOptionalPluginDllNames[i];

		// Check to see if the file exists    
		if ( ::GetFileAttributes ( sPluginPath.c_str()) != INVALID_FILE_ATTRIBUTES )
			m_vPlugins.push_back( sPluginPath );
	}                

	//
	// Add common client plugins 
	//
	for( i = 0; m_sCommonPluginDllNames[i] != NULL; i++ )
	{
		::std::string sPluginPath = navInfo.GetSymantecCommonDir();
		sPluginPath += '\\';
		sPluginPath += m_sCommonPluginDllNames[i];
		m_vPlugins.push_back( sPluginPath );
	}
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::reportFatalError()
// 
//  This function displays the CEH.
void CNavProductPlugin::reportFatalError(UINT uMessageID)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr<INAVCOMError> spNavError;
		hrx << spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);

		// Populate the error
		hrx << spNavError->put_ModuleID(AV_MODULE_ID_NAV_PRODUCT_PLUGIN);
		hrx << spNavError->put_ErrorID(uMessageID);

		CString sMsg;

		g_ResLoader.LoadString(uMessageID, sMsg);

		CComBSTR bstrErrorMessage(sMsg);
		hrx << spNavError->put_Message(bstrErrorMessage);

		hrx << spNavError->Show(TRUE, FALSE, NULL);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::areNAVFilesRemoved()
// 
//	This function is mainly use to determine if NAV2002 had messup your
//  system, which is determine from a list of removed files and unremoved 
//	files.
bool CNavProductPlugin::areNAVFilesRemoved()
{
	// Get the NAV install path.
	TCHAR szNAVDir[MAX_PATH] = {0};	
	TCHAR* pszTemp = NULL;
	TCHAR szFile[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	int i;

	// Get the full path to checknav.dll
	GetModuleFileName(g_hInstance, szNAVDir, dwSize);

	// Cleanup the path to get such the path to NAV directory.
	pszTemp = _tcsrchr(szNAVDir, '\\');
	if (pszTemp != NULL)
		*pszTemp = _T('\0');

	// Check our list of files too see if they are all removed.
	// If so, it might be SymClean wiping us out, so we need to 
	// check our list of files that are left behind.
	for( i = 0; szNAVRemovedFiles[i] != NULL; i++ )
	{
		wsprintf(szFile, _T("%s\\%s"), szNAVDir, szNAVRemovedFiles[i]);

		// Check if file exists. If it does we know that it's not SymClean
		// removing the files.
		if(GetFileAttributes(szFile) != -1)
		{
			return false;	
		}
	}

	// Check our list of files that are left behind. If it matches we know that
	// SymClean is removing the NAV files.

	for( i = 0; szNAVUnRemovedFiles[i] != NULL; i++ )
	{
		wsprintf(szFile, _T("%s\\%s"), szNAVDir, szNAVUnRemovedFiles[i]);

		if(GetFileAttributes(szFile) == -1)
		{		
			return false;
		}	
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// 	CNavProductPlugin::getNAVGUID()
//
//	This function retrieves the install GUID for NAV in a standalone
//	scenario, as well as a suite scenario. This GUID will be used
//	to call the NAV uninstall.
void CNavProductPlugin::getNAVGUID()
{

	TCHAR szNISGUID[MAX_PATH] = {0};
	TCHAR szNAVGUID[MAX_PATH] = {0};
	TCHAR szNAVPROGUID[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szNAVGUID);	

	*m_sNAVGUID = _T('\0');	

	//check the upgrade state for NIS4
	if(MsiEnumRelatedProducts(cszNISUpgradeCode, 0, 0, szNISGUID) == ERROR_SUCCESS)
	{
		// If we determine that we are suite, we bail out, no need to check other
		// possibility.
		if( MsiQueryFeatureState(szNISGUID, _T("Norton_AntiVirus")) == INSTALLSTATE_LOCAL)
		{			
			_tcscpy(m_sNAVGUID, _T("SUITE"));	
			return;
		}

	}

	//check the upgrade state for NAV
	if(MsiEnumRelatedProducts(cszNAVUpgradeCode, 0, 0, szNAVGUID) == ERROR_SUCCESS)
	{
		//its wrong NAV IS on the system so make it so
		_tcscpy(m_sNAVGUID, szNAVGUID);

	}

	if(MsiEnumRelatedProducts(cszNAVPROUpgradeCode, 0, 0, szNAVPROGUID) == ERROR_SUCCESS)
	{
		//its wrong NAV IS on the system so make it so
		_tcscpy(m_sNAVGUID, szNAVPROGUID);
	}
}
