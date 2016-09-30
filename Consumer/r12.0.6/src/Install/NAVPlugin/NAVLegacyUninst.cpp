// NAVLegacyUninst.cpp: implementation of the CNAVLegacyUninst class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "platform.h"
#include "NAVLegacyUninst.h"
#include "OSInfo.h"
#include "NAVInfo.h"
#include "objbase.h"
#include "NavCommandLines.h"
#include "DefUtils.h"
#include "shlobj.h"
#include "shfolder.h"
#include "SVCCTRL.H"
#include "helper.h"

#ifdef NAVAP_WND_CLASS
    #undef NAVAP_WND_CLASS
#endif

#include "navwcomm.h"
#include "NavApCom.h"

const TCHAR g_cszSymInstalledApps[] = _T("Software\\Symantec\\InstalledApps");
const TCHAR g_cszNAVKey[] = _T("Software\\Symantec\\Norton AntiVirus");
const TCHAR g_cszNAVNTKey[] = _T("SOFTWARE\\Symantec\\Norton AntiVirus NT");
const TCHAR g_cszSBSetup[] = _T("SBSetup.exe");
const TCHAR g_cszSBPath[] = _T("Script Blocking");
const TCHAR g_cszScriptUI[] = _T("ScriptUI.dll");
const TCHAR g_cszNetscapePlugin[] = _T("nsplugin.exe");
const TCHAR g_cszNTaskMgr[] = _T("ntaskmgr.exe");
const TCHAR g_cszAdvProtUninstKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Advanced Tools");
const TCHAR g_cszNAVUninstKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Norton AntiVirus");
const TCHAR g_cszNSWNAVUninstKey[] = _T("Software\\Symantec\\Norton SystemWorks\\uninstall\\Norton AntiVirus");
const TCHAR g_cszLiveRegAppKey[] = _T("Software\\Symantec\\Shared Technology\\LiveReg\\Apps");
const TCHAR g_cszLiveAdvisorAppKey[] = _T("Software\\Symantec\\Shared Technology\\LiveAdvisor\\Apps");
const TCHAR g_cszMSRunKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
const TCHAR g_cszMSRunOnceKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce");
const TCHAR g_cszMSAppPathKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths");
const TCHAR g_cszMSMigrationDLLKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\Migration DLLs");
const TCHAR g_cszMSHelpKey[] = _T("Software\\Microsoft\\Windows\\Help");
const TCHAR g_cszLSUtilsDLL[] = _T("LSUTILS.dll");
const TCHAR g_cszSAPKey[] = _T("Symantec AntiVirus for Palm OS");
const TCHAR g_cszRescueUninstKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Norton Rescue");
const TCHAR g_cszNAVAPVxDKey[] = _T("System\\CurrentControlSet\\Services\\VxD\\NAVAP");
const TCHAR g_cszNAVShellKey[] = _T("CLSID\\{067DF822-EAB6-11cf-B56E-00A0244D5087}");

typedef void (PASCAL *LPFN_STOPPOPROXY)(void);
typedef BOOL (PASCAL *LPFN_ISPROXYRUNNING)(void);
typedef LONG (PASCAL *LPFN_REMOVEHOSTSENTRIES)(void);
typedef void (PASCAL *LPFN_UNCONFIGUREALL)();
typedef void (__stdcall *LPFN_PPopExamUnconfigure)(void);
typedef BOOL (WINAPI* LPFN_N32USERLUNINSTALL)();

const TCHAR* g_cszCOMDlls[] =
{
    _T("AboutPlg.dll"),
    _T("NavAbout.dll"),
    _T("Navlucbk.dll"),
    _T("OfficeAV.dll"),
    _T("Quar32.dll"),
    _T("Scandlvr.dll"),
    _T("SWPlugin.dll"),
    NULL
};

const TCHAR* g_cszNAVAppID9x[] = 
{
	_T("NAV95_50_AP1"),
	_T("NAV95_50_AP2"),
	_T("NAV_50_QUAR"),
	_T("NAV95_50_NAVW"),
	_T("NAV6_Integrator"),
	NULL
};

const TCHAR* g_cszNAVAppIDNT[] =
{
	_T("NAVNT_50_AP1"),
	_T("NAVNT_50_AP2"),
	_T("NAV_50_QUAR"),
	_T("NAVNT_50_NAVW"),
	_T("NAV6_Integrator"),
	NULL
};

const TCHAR* g_cszServices[] = 
{
	_T("NAV Alert"),
	_T("NAV Auto-Protect"),
	_T("Norton Program Scheduler"),
	_T("navap"),
	NULL
};

const TCHAR* g_cszNAVWinFiles[] = 
{
	_T("navustub.exe"),
	_T("nlogos.sys"),
	_T("nlogow.sys"),
	_T("user.nav"),
	_T("system.nav"),
	NULL
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVLegacyUninst::CNAVLegacyUninst()
{
	m_iVersionNum = 0;
	m_bNAVInstalled = FALSE;
	m_bInitialized = FALSE;
	m_bNAVPro = FALSE;
	m_bFORCE = FALSE;

}

CNAVLegacyUninst::~CNAVLegacyUninst()
{
	if (UninstLog != NULL)
		delete UninstLog;
}

BOOL CNAVLegacyUninst::Initialize(LPCTSTR szDirectory)
{
	try
	{
		// Checks the InstalledApps key to see if NAV is installed.

		DWORD dwType;
		DWORD dwSize = sizeof(m_szNAVInstDir);
		LONG lResult;
		HKEY hInstAppKey;
		
		// If no directory is specified, we continue with our uninstall.
		if (szDirectory == NULL)
		{
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszSymInstalledApps, REG_OPTION_NON_VOLATILE, KEY_READ, &hInstAppKey);
			if (lResult == ERROR_SUCCESS)
			{
				// Check for "NAV", "NAV95" and "NAVNT" and find the Norton AntiVirus install directory.	
				lResult = RegQueryValueEx(hInstAppKey, _T("NAV"), NULL, &dwType, (BYTE*)m_szNAVInstDir, &dwSize);

				if (lResult == ERROR_SUCCESS)
				{	
					m_bNAVInstalled = TRUE;			
				}
				else
				{
					dwSize = sizeof(m_szNAVInstDir);
					lResult = RegQueryValueEx(hInstAppKey, _T("NAV95"), NULL, &dwType, (BYTE*)m_szNAVInstDir, &dwSize);

					if (lResult == ERROR_SUCCESS)
					{
						m_bNAVInstalled = TRUE;			
					}
					else
					{
						dwSize = sizeof(m_szNAVInstDir);
						lResult = RegQueryValueEx(hInstAppKey, _T("NAVNT"), NULL, &dwType, (BYTE*)m_szNAVInstDir, &dwSize);

						if (lResult == ERROR_SUCCESS)
						{
							m_bNAVInstalled = TRUE;		
						}
					}
				}

				// Close the regkey.

				RegCloseKey(hInstAppKey);
			}
			else
			{
				// Can't open Symantec InstalledApps key.

				throw runtime_error("Unable to open Symantec InstallApps key.");
			}
			
		}
		
		// This mean we are in Force uninstall mode, so we pass in the directory to uninstall.
		else
		{		
			_tcscpy(m_szNAVInstDir, szDirectory);
			m_bNAVInstalled = TRUE;
			return TRUE;
		}

		// If NAV is found under the InstalledApps, let's check for the version number.
		if (m_bNAVInstalled == TRUE)
		{
			// Open the NAV key.

			COSInfo osInfo;
			TCHAR szNAVKey[MAX_PATH] = {0};
			HKEY hNAVKey;

			if (osInfo.IsWinNT() == TRUE)
			{
				_tcscpy(szNAVKey, g_cszNAVNTKey);
			}
			else
			{
				_tcscpy(szNAVKey, g_cszNAVKey);
			}

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szNAVKey, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hNAVKey) != ERROR_SUCCESS)
			{
				// Can't locate the NAV key.		
				throw runtime_error("Unable to locate Norton AntiVirus key.");
			}


			// Find the CurrentVersion key and read in the version number.

			TCHAR szBuffer[3] = {0};
			dwSize = sizeof(szBuffer);
		
			if (RegQueryValueEx(hNAVKey, _T("CurrentVersion"), NULL, NULL, (BYTE*) szBuffer, &dwSize) == ERROR_SUCCESS)
			{
				m_iVersionNum = szBuffer[0];
			}

			RegCloseKey(hNAVKey);

			// Find out whether we are NAV or NAV Professional

			HKEY hVersionKey;

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVKey, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hVersionKey) == ERROR_SUCCESS)
			{

				TCHAR szNAVType[2] = {0};
				dwSize = sizeof(szNAVType);

				if (RegQueryValueEx(hVersionKey, _T("Type"), NULL, NULL, (BYTE* ) &szNAVType, &dwSize) == ERROR_SUCCESS)
				{
					// If it's NAV Professional, Type should be set to 1

					if (_ttoi(szNAVType) == 1)
					{
						m_bNAVPro = TRUE;
					}
				}
			}					
		}

		// Set the initialization flag to TRUE.

		m_bInitialized = TRUE;
	}
	catch(exception& ex)
	{
		m_iVersionNum = 0;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		// Something bad happened..

		m_bNAVInstalled = FALSE;		
		m_iVersionNum = 0;
		m_bNAVPro = FALSE;
	
		UninstLog->LogEx("Unknown error occurred while initializing legacy uninstall.");
	}
	
	return TRUE;
}

BOOL CNAVLegacyUninst::GetLegacyVersionNum(int* iVersion)
{
	*iVersion = m_iVersionNum;
	
	return TRUE;
}

BOOL CNAVLegacyUninst::ShutdownAPUI()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// If NAV is installed and it's NAV2000 or 2001.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == NAV2000 || m_iVersionNum == NAV2001 || m_bFORCE == TRUE))
		{
			//  Attempts to close the AP window, if it is running.					
			COSInfo osInfo;
			HWND hNavAp;

			if (osInfo.IsWinNT() == true)
			{
				// If it's NT or Win2k (could be XP but I don't think NAV2000 or 2001 supports XP).

				hNavAp = FindWindow(NAVAP_WND_CLASS, NULL);

				if (hNavAp)
				{
					// Found the windows, send a message to close it

					PostMessage(hNavAp, WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
					bReturn = TRUE;
				}
				else
				{
					bReturn = FALSE;
				}
			}
			else
			{
				UINT uNavApWMessage = 0L;

				hNavAp = FindWindow(NAVAP_95_WND_CLASS, NULL);
				if (hNavAp)
				{
					// Found the window, send a message to close it.
					// Get the custom message that NAVW uses to talk to the agent.
				
					uNavApWMessage = RegisterWindowMessage (NAVW_NAVAPW_COMM);

					// Send unload.

					if (uNavApWMessage != NULL)
					{
						PostMessage(hNavAp, uNavApWMessage, NAVW_CMD_UNLOADNAVAPW, 0L);
						bReturn = TRUE;
					}
				}
				else
				{
					bReturn = FALSE;
				}
			}
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while shutting down Auto Protect.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveNAVServices()
{
	BOOL bReturn = TRUE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// If NAV is installed and it's NAV2000 or 2001.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == NAV2000 || m_iVersionNum == NAV2001 || m_bFORCE == TRUE))
		{
			COSInfo osInfo;

			if (osInfo.IsWinNT() == true)
			{
				for (int i = 0; g_cszServices[i] != NULL; i++)
				{
					if (RemoveService(g_cszServices[i]) == FALSE)
					{
						bReturn = FALSE;

						UninstLog->Log("Failed to remove %s service.", g_cszServices[i]);
					}
				}
			}
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Norton AntiVirus services.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveScheduledTasks()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// If NAV is installed and it's NAV2000 or 2001.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == NAV2000 || m_iVersionNum == NAV2001 || m_bFORCE == TRUE))
		{
			// Remove any NAV Scan or NAVLU tasks from the MS Task Scheduler.

			TCHAR szNTaskMgr[MAX_PATH] = {0};

			wsprintf(szNTaskMgr, "%s\\%s", m_szNAVInstDir, g_cszNTaskMgr);

			if (LaunchAndWait(szNTaskMgr, _T("/d")) == FALSE)
			{
				throw runtime_error("Failed to remove scheduled tasks from MS Task Scheduler.");
			}

			bReturn = TRUE;
		}
	}
	catch(exception& ex)
	{
		bReturn = FALSE;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing scheduled tasks.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::ShutdownPoproxy()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// If NAV is installed and it's NAV2000 or 2001.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == 6 || m_iVersionNum == 7 || m_bFORCE == TRUE))
		{
			HINSTANCE hInst;

			// If it's NAV 2000.

			if (m_iVersionNum == 6)
			{
				// Load PopExam.dll and stop Poproxy.

				LPFN_STOPPOPROXY lpfnStopPopProxy;
				LPFN_PPopExamUnconfigure lpfnPopExamUnconfigure;
				TCHAR szDllPath[MAX_PATH] = {0};

				wsprintf(szDllPath, _T("%s\\popexam.dll"), m_szNAVInstDir);

				hInst = LoadLibrary(szDllPath);

				if (hInst != NULL)
				{
					lpfnStopPopProxy = reinterpret_cast<LPFN_STOPPOPROXY>(GetProcAddress(hInst, _T("PX_StopPOProxy")));

					if (lpfnStopPopProxy != NULL)
					{
						lpfnStopPopProxy();
					}

					// unconfigure email clients

					lpfnPopExamUnconfigure = reinterpret_cast<LPFN_PPopExamUnconfigure>(GetProcAddress(hInst, _T("PX_UnconfigureAll")));

					if (lpfnPopExamUnconfigure != NULL)
					{
						lpfnPopExamUnconfigure();
					}

					FreeLibrary(hInst);
				}

				bReturn = TRUE;
			}
			else 
			{
				TCHAR szDllPath[MAX_PATH] = {0};
				TCHAR szCurrentPath[MAX_PATH] = {0};
				LPFN_STOPPOPROXY lpfnStopPopProxy;
				LPFN_ISPROXYRUNNING lpfnIsProxyRunning;
				LPFN_REMOVEHOSTSENTRIES lpfnRemoveHostsEntries;
				LPFN_UNCONFIGUREALL lpfnUnconfigureAll;


				// Since EmailCfg.dll has dependencies on EmailRes.dll, in order for the LoadLibrary 
				// call to succeed, we must change the current path to where EmailCfg is located.

				GetCurrentDirectory(sizeof(szCurrentPath), szCurrentPath);
				SetCurrentDirectory(m_szNAVInstDir);
				wsprintf(szDllPath, _T("%s\\emailcfg.dll"), m_szNAVInstDir);

				hInst = LoadLibrary(szDllPath);

				if (hInst != NULL)
				{
					lpfnIsProxyRunning = reinterpret_cast<LPFN_ISPROXYRUNNING>(GetProcAddress(hInst, _T("IsProxyRunning")));

					// If PopProxy is running, shut it down.

					if (lpfnIsProxyRunning != NULL)
					{
						if (lpfnIsProxyRunning())
						{
							lpfnStopPopProxy = reinterpret_cast<LPFN_STOPPOPROXY>(GetProcAddress(hInst, _T("StopPorxy")));

							if (lpfnStopPopProxy != NULL)
								lpfnStopPopProxy();
						}
					}

					// Unconfigure all Email accounts.

					lpfnUnconfigureAll = reinterpret_cast<LPFN_UNCONFIGUREALL>(GetProcAddress(hInst, _T("UnconfigureAll")));

					if (lpfnUnconfigureAll != NULL)
						lpfnUnconfigureAll();

					// Removed the HOST file entries.

					lpfnRemoveHostsEntries = reinterpret_cast<LPFN_REMOVEHOSTSENTRIES>(GetProcAddress(hInst, _T("RemoveHostsEntries")));

					if (lpfnRemoveHostsEntries != NULL)
						lpfnRemoveHostsEntries();
					
					FreeLibrary(hInst);
				}

				// Restore the current path to what it was when we started.

				SetCurrentDirectory(szCurrentPath);

				bReturn = TRUE;
			}
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while shutting down Email Protection.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveScriptBlocking()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		TCHAR szSBSetup[MAX_PATH] = {0};
		TCHAR szCmdLine[MAX_PATH] = {0};
		CNAVInfo navInfo;

		// Make sure SBSetup.exe exists (SBSetup.exe is the ScriptBlocking installer/uninstaller).

		wsprintf(szSBSetup, "%s\\%s\\%s", navInfo.GetSymantecCommonDir(), g_cszSBPath, g_cszSBSetup);

		if (GetFileAttributes(szSBSetup) == -1)
		{
			TCHAR szErrMsg[MAX_PATH] = {0};

			wsprintf(szErrMsg, "Failed to locate %s", szSBSetup);

			throw runtime_error(szErrMsg);
		}

		// Launch SBSetup.exe with uninstall parameter.

		wsprintf(szCmdLine, "-u \"%s\\%s\" WarChildCallBack", m_szNAVInstDir, g_cszScriptUI);

		if (LaunchAndWait(szSBSetup, szCmdLine) == FALSE)
		{
			throw runtime_error("Failed to remove Script Blocking.");
		}

		bReturn = TRUE;
	}
	catch(exception& ex)
	{
		bReturn = FALSE;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Script Blocking.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::UnregisterWithLU()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// Make sure NAV is installed.
		
		if (m_bNAVInstalled == TRUE)
		{
			HRESULT hResult = CoInitialize(NULL);

			if (SUCCEEDED(hResult))
			{
				CNavCommandLines cmdLines;
				
				if (FAILED(cmdLines.UnregisterCommandLines()))
				{
					bReturn = FALSE;

					UninstLog->Log("Failed to unregistered Norton AntiVirus with LiveUpdate.");
				}

				CoUninitialize();
			}

			bReturn = TRUE;
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while unregistering Norton AntiVirus with LiveUpdate.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::UninstallVirusDefs()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();
		
		if (m_bNAVInstalled == TRUE && (m_iVersionNum == 6 || m_iVersionNum == 7 || m_bFORCE == TRUE))
		{
			CDefUtils DefUtils;
			COSInfo osInfo;

			// Check what OS we're running and use the appropriate App ID list.

			if (osInfo.IsWinNT() == true)
			{
				for (int i = 0; g_cszNAVAppIDNT[i] != NULL; i++)
				{
					if (DefUtils.InitInstallApp(g_cszNAVAppIDNT[i], m_szNAVInstDir))
						DefUtils.StopUsingDefs();
				}
			}
			else
			{
				for (int i = 0; g_cszNAVAppID9x[i] != NULL; i++)
				{
					if (DefUtils.InitInstallApp(g_cszNAVAppID9x[i], m_szNAVInstDir))
						DefUtils.StopUsingDefs();
				}
			}

			bReturn = TRUE;
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while uninstalling virus definitions.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveNAVDXStartUp()
{
	BOOL bReturn = FALSE;

	try
	{
		// Get the path to autoexec.bat.

		TCHAR szAutoexec[MAX_PATH] = {0};
	
		GetWindowsDirectory(szAutoexec, MAX_PATH);
		TCHAR* pStr = _tcsrchr(szAutoexec, _T('\\'));
		if(pStr != NULL)
			*pStr = _T('\0');
	
		_tcscat(szAutoexec, _T("\\autoexec.bat"));
	
		// Call RemoveLine to remove line with navdx.exe.

		if(RemoveLine(szAutoexec, NAVDX_EXE) == FALSE)
		{
			throw runtime_error("Error in Removing navdx.exe from autoexec.bat");
		}

		bReturn = TRUE;
	}
	catch(exception& ex)
	{
		bReturn = FALSE;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing NAVDX from Startup.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveNetscapePlugin()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// Make sure NAV is installed.
		
		if (m_bNAVInstalled == TRUE)
		{
			TCHAR szNSPlugin[MAX_PATH] = {0};
			TCHAR szCmdLine[MAX_PATH] = {0};
			
			// Make sure the Netscape plugin exists.

			wsprintf(szNSPlugin, _T("%s\\%s"), m_szNAVInstDir, g_cszNetscapePlugin);

			if (GetFileAttributes(szNSPlugin) == -1)
			{
				throw runtime_error("Failed to locate NSPlugin.exe (Netscape Plugin).");
			}

			// Launch Netscape plugin with uninstall parameter.

			_tcscat(szCmdLine, "/s /u");

			if (LaunchAndWait(szNSPlugin, szCmdLine) == FALSE)
			{
				throw runtime_error("Failed to remove Netscape plugin.");
			}

			bReturn = TRUE;
		}
	}
	catch(exception& ex)
	{
		bReturn = FALSE;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Netscape plugin.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::UnregisterWithSymEvent()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// Make sure NAV is installed.
		
		if (m_bNAVInstalled == TRUE)
		{
			TCHAR szCmdLine[MAX_PATH] = {0};
			TCHAR szSevInst[MAX_PATH] = {0};
			CNAVInfo navInfo;
			LPCTSTR lpszTmp = NULL;

			wsprintf(szSevInst, _T("%s\\sevinst.exe"), navInfo.GetSymantecCommonDir());

			if (GetFileAttributes(szSevInst) == -1)
			{
				throw runtime_error("Failed to locate SevInst.exe.");
			}

			// Setup the parameter string.

			HKEY hkey;

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Symevent\\NAV" ), 0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
			{
				_tcscpy(szCmdLine, _T("/Q /U NAV"));
			}
			else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Symevent\\NAV95" ), 0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
			{
				_tcscpy(szCmdLine, _T("/Q /U NAV95"));
			}
			else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Symevent\\NAVNT"), 0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS) 
			{
				_tcscpy(szCmdLine, _T("/Q /U NAVNT"));
			}

			if (hkey != NULL)
				RegCloseKey(hkey);

			// If we don't have the parameter, don't execute the command.

			if (!*szCmdLine)
			{
				throw runtime_error("Failed to generate SymEvent uninstall command.");
			}

			// Launch the command to uninstall the SymEvent.
			
			if (LaunchAndWait(szSevInst, szCmdLine) == FALSE)
			{
				throw runtime_error("Failed to unregister with SymEvent.");
			}

			bReturn = TRUE;
		}
	}
	catch(exception& ex)
	{
		bReturn = FALSE;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while unregistering with SymEvent.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::UnregisterCOMObjects()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// Make sure NAV is installed and it's NAV 2000 or 2001.
		
		if (m_bNAVInstalled == TRUE && (m_iVersionNum == NAV2000 || m_iVersionNum == NAV2001 || m_bFORCE == TRUE))
		{
			// Loop and process each file in the list.		
		
			TCHAR szFileName[MAX_PATH] = {0};
		
			for (int i = 0; NULL != g_cszCOMDlls[i]; i++)
			{
				// Assemble the full filename.

				_tcscpy(szFileName, m_szNAVInstDir);
				_tcscat(szFileName, "\\");
				_tcscat(szFileName, g_cszCOMDlls[i]);

				if (UnregisterCOMDll(szFileName, m_szNAVInstDir) == FALSE)
				{
					UninstLog->Log("Failed to unregister %s.", szFileName);
				}
			}

			// Need to unregister a NAV 2000 (version 6.x) specific file (aboutsw.dll).

			if (m_iVersionNum == 6)
			{
				_tcscpy(szFileName, m_szNAVInstDir);
				_tcscat(szFileName, "\\");
				_tcscat(szFileName, _T("aboutsw.dll"));

				if (UnregisterCOMDll(szFileName, m_szNAVInstDir) == FALSE)
				{
					UninstLog->Log("Failed to unregister %s.", szFileName);
				}
			}

			bReturn = TRUE;
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while unregistering COM Dlls.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::UnregisterNMain()
{
	BOOL bReturn = FALSE;

	try
	{
		// Call NMAIN to do its uninstall stuff.

		TCHAR szNMainCmdLine[MAX_PATH] ={0};
		CNAVInfo NavInfo;

		wsprintf(szNMainCmdLine, "%s\\nmain.exe", NavInfo.GetSymantecCommonDir());

		if (GetFileAttributes(szNMainCmdLine) == -1)
		{
			throw runtime_error("Failed to locate NMain.exe.");
		}

		if (LaunchAndWait(szNMainCmdLine, "/unreg") == FALSE)
		{
			throw runtime_error("Failed to unregister NMain.exe");
		}

		bReturn = TRUE;
	}
	catch(exception& ex)
	{
		bReturn = FALSE;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while unregistering Integrator.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveAdvProtection()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are NAV Pro first.

		if (m_bNAVPro == TRUE || m_bFORCE == TRUE)
		{
			HKEY hKey;
			LONG lResult;

			// Try to open the Advance Protection Uninstall key.

			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszAdvProtUninstKey, 0, KEY_QUERY_VALUE, &hKey);
					
			if (lResult == ERROR_SUCCESS)
			{
				// Find the Uninstall Commandline.

				TCHAR szAdvProtCmdLine[MAX_PATH] = {0};
				DWORD dwSize = sizeof(szAdvProtCmdLine);
				
				lResult = RegQueryValueEx(hKey, "QuietUninstallString", NULL, NULL, (LPBYTE)szAdvProtCmdLine, &dwSize );
				
				if (lResult == ERROR_SUCCESS)
				{
					if (LaunchAndWait(NULL, szAdvProtCmdLine) == FALSE)
					{
						bReturn = FALSE;

						UninstLog->Log("Failed to remove Advance Protection.");
					}
					else
					{
						bReturn = TRUE;
					}
				}
				
				RegCloseKey(hKey);
			}
			else
			{
				UninstLog->Log("Cannot located Advance Protection's uninstall key.");
			}
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Advance Protection.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::UnregisterSAP()
{
	BOOL bReturn = FALSE;

	try
	{
		if (m_bInitialized == FALSE)
			Initialize();

		if (m_bNAVPro == TRUE || m_bFORCE == TRUE)
		{
			LONG lResult;
			HKEY hKey;
			TCHAR szCmdLine[MAX_PATH] = {0};
			TCHAR szLSUtilsPath[MAX_PATH] = {0};

			// Get the path to the dll.

			_tcscat(szLSUtilsPath, _T("\""));
			_tcscat(szLSUtilsPath, m_szNAVInstDir);
			_tcscat(szLSUtilsPath, _T("\\"));
			_tcscat(szLSUtilsPath, g_cszLSUtilsDLL);
			_tcscat(szLSUtilsPath, _T("\""));
			GetShortPathName(szLSUtilsPath, szLSUtilsPath, MAX_PATH);
			
			// Check the InstalledApps key for SAP. If SAP is not installed, unregister SAP since 
			// SAP was registered by NAVPRO.
			
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszSymInstalledApps, 0, KEY_QUERY_VALUE, &hKey);
			
			if (lResult == ERROR_SUCCESS)
			{		
				lResult = RegQueryValueEx(hKey, g_cszSAPKey, NULL, NULL, NULL, NULL);

				if (lResult != ERROR_SUCCESS)
				{
					GetSystemDirectory(szCmdLine, MAX_PATH);
    				_tcscat(szCmdLine, _T("\\regsvr32.exe /s /u /n /i:\"4359E9C0-4C19-4881-923C-94DEF01C2F2C\" "));
					_tcscat(szCmdLine, szLSUtilsPath);

					if (LaunchAndWait(NULL, szCmdLine) == FALSE)
					{
						bReturn = FALSE;

						UninstLog->Log("Failed to unregister Symantec AntiVirus for Palm.");
					}
					else
					{
						bReturn = TRUE;
					}
				}
				
				RegCloseKey(hKey);
			}
			else
			{
				UninstLog->Log("Symantec AntiVirus for Palm is not installed.");
			}
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while unregistering Symantec AntiVirus for Palm.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveRescue()
{
	BOOL bReturn = FALSE;

	try
	{
		if (m_bInitialized == FALSE)
			Initialize();

		// Unregister NAVResc.dll (it's Norton AntiVirus' Rescue plugin).

		TCHAR szNAVResc[MAX_PATH] = {0};

		wsprintf(szNAVResc, _T("%s\\NavResc.dll"), m_szNAVInstDir);

		if (GetFileAttributes(szNAVResc) == -1)
		{
			UninstLog->Log("Cannot located Norton AntiVirus' Norton Rescue plugin.");
		}

		if (UnregisterCOMDll(szNAVResc, m_szNAVInstDir) == FALSE)
		{
			UninstLog->Log("Failed to unregister Norton AntiVirus' Norton Rescue plugin.");
		}
		
		// Figure out the Rescue uninstall string.

		HKEY hKey;
		TCHAR szRescueCmdLine[MAX_PATH] = {0};
		DWORD dwSize = sizeof(szRescueCmdLine);
		LONG lResult;

		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszRescueUninstKey, 0, KEY_QUERY_VALUE, &hKey);

		if (lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(hKey, "QuietUninstallString", NULL, NULL, (LPBYTE)szRescueCmdLine, &dwSize);
    
			// Launch Rescue uninstall.

			if (lResult == ERROR_SUCCESS)
			{
				if (LaunchAndWait(NULL, szRescueCmdLine) == FALSE)
				{
					bReturn = FALSE;

					UninstLog->Log("Failed to remove Norton Rescue.");
				}
				else
				{
					bReturn = TRUE;
				}
			}

			RegCloseKey(hKey);
		}
		else
		{
			UninstLog->Log("Cannot located Norton Rescue's uninstall key.");
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Norton Rescue.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveVBoxWrapper()
{
	BOOL bReturn = FALSE;

	try
	{
		if (m_bInitialized == FALSE)
			Initialize();

        // Uninstall our VBox wrapper.

        HINSTANCE hInst;
        LPFN_N32USERLUNINSTALL lpfnUninstall;
		TCHAR szN32Userl[MAX_PATH] = {0};

		wsprintf(szN32Userl, _T("%s\\N32Userl.dll"), m_szNAVInstDir);

        hInst = LoadLibrary(szN32Userl);

        if (hInst != NULL)
		{
            lpfnUninstall = reinterpret_cast<LPFN_N32USERLUNINSTALL>(GetProcAddress(hInst, _T("N32User_UnInstall")));

            if (lpfnUninstall != NULL)
                lpfnUninstall();

            FreeLibrary(hInst);
		}

		bReturn = TRUE;
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing VBox wrapper.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveRegkeys()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// If NAV is installed and it's NAV2000 or 2001.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == NAV2000 || m_iVersionNum == NAV2001 || m_bFORCE == TRUE))
		{
			TCHAR szBuff[MAX_PATH] = {0};

			// Remove the LiveReg & LiveAdvisor Apps registry key(s) so we don't prevent it 
			// from being uninstalled.

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszLiveRegAppKey, _T("NAV")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\Software\\Symantec\\Shared Technology\\LiveReg\\Apps\\NAV");
			}

			if (DeleteRegKeyName( HKEY_LOCAL_MACHINE, g_cszLiveRegAppKey, _T("Norton AntiVirus")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\Software\\Symantec\\Shared Technology\\LiveReg\\Apps\\Norton AntiVirus");
			}

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszLiveAdvisorAppKey, _T("NAV")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\Software\\Symantec\\Shared Technology\\LiveAdvisor\\Apps\\NAV");
			}

			// Remove the Norton Program Scheduler Event Checker.

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszMSRunKey, _T("Norton Program Scheduler Event Checker")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\Norton Program Scheduler Event Checker");
			}

			// Remove installed path values.
			// Remove all NAVxx keys in case where a 9x machine that has NAV installed was upgraded to NT.

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszSymInstalledApps, _T("NAV")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s\\NAV", g_cszSymInstalledApps);
			}

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszSymInstalledApps, _T("NAVNT")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s\\NAVNT", g_cszSymInstalledApps);
			}

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszSymInstalledApps, _T( "NAV95")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s\\NAV95", g_cszSymInstalledApps);
			}

			// Remove NAVAPW32.exe from AppPath key.
			
			wsprintf(szBuff, _T("%s\\navapw32.exe"), g_cszMSAppPathKey);

			if (RegDeleteKeyEx(HKEY_LOCAL_MACHINE, szBuff) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s", szBuff);
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s", szBuff);
			}

			COSInfo osInfo;

			// Remove NAVW32.exe/NAVWNT.exe from AppPath key.
			
			if (osInfo.IsWinNT() == true)
			{
				wsprintf(szBuff, _T("%s\\navwnt.exe"), g_cszMSAppPathKey);
			}
			else
			{
				wsprintf(szBuff, _T("%s\\navw32.exe"), g_cszMSAppPathKey);
			}

			if (RegDeleteKeyEx(HKEY_LOCAL_MACHINE, szBuff) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s", szBuff);
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s", szBuff);
			}

			// Remove ntaskmgr.exe from AppPath key.
			
			wsprintf(szBuff, _T("%s\\ntaskmgr.exe"), g_cszMSAppPathKey);

			if (RegDeleteKeyEx(HKEY_LOCAL_MACHINE, szBuff) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s", szBuff);
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s", szBuff);
			}

			// Remove the HKCR NavScanFile Key.

			if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("NAVScanFile")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKCR\\%s", _T("NAVScanFile"));
			}
			else
			{
				UninstLog->Log("Failed to delete HKCR\\%s", _T("NAVScanFile"));
			}

			// Remove the HKCR .Scan Key.

			if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T(".Scan")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKCR\\%s", _T(".Scan"));
			}
			else
			{
				UninstLog->Log("Failed to delete HKCR\\%s", _T(".Scan"));
			}

			// Remove the HKCR .Scan Key.

			if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("NavLuFile")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKCR\\%s", _T("NavLuFile"));
			}
			else
			{
				UninstLog->Log("Failed to delete HKCR\\%s", _T("NavLuFile"));
			}

			// Remove the HKCR .Scan Key.

			if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("Virfile")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKCR\\%s", _T("Virfile"));
			}
			else
			{
				UninstLog->Log("Failed to delete HKCR\\%s", _T("Virfile"));
			}

			// Remove the HKCR NortonAntiVirus Key.

			if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("NortonAntiVirus")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKCR\\%s", _T("NortonAntiVirus"));
			}
			else
			{
				UninstLog->Log("Failed to delete HKCR\\%s", _T("NortonAntiVirus"));
			}

			// Remove the Nav Shell extension CLSID ID Key.

			if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, g_cszNAVShellKey) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKCR\\%s", g_cszNAVShellKey);
			}
			else
			{
				UninstLog->Log("Failed to delete HKCR\\%s", g_cszNAVShellKey);
			}

			// Remove the HKLM Windows\Migration key.

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszMSMigrationDLLKey, _T("Norton AntiVirus 6.0")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s\\%s", g_cszMSMigrationDLLKey, _T("Norton AntiVirus 6.0"));
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s\\%s", g_cszMSMigrationDLLKey, _T("Norton AntiVirus 6.0"));
			}

			// Remove the NAV Help key.

			if (osInfo.IsWinNT() == true)
			{
				_tcscpy(szBuff, _T("navwnt.hlp"));
			}
			else
			{
				_tcscpy(szBuff, _T("navw32.hlp"));
			}

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszMSHelpKey, szBuff) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s\\%s", g_cszMSHelpKey, szBuff);
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s\\%s", g_cszMSHelpKey, szBuff);
			}

			// Remove the HKLM "Norton AntiVirus" key.

			if (RegDeleteKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVKey) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s", g_cszNAVKey);
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s", g_cszNAVKey);
			}

			// Remove the HKCU "Norton AntiVirus" key.

			if (RegDeleteKeyEx(HKEY_CURRENT_USER, g_cszNAVKey) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKCU\\%s", g_cszNAVKey);
			}
			else
			{
				UninstLog->Log("Failed to delete HKCU\\%s", g_cszNAVKey);
			}

			if (osInfo.IsWinNT() == true) 
			{
				if (RegDeleteKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVNTKey) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKLM\\%s", g_cszNAVNTKey);
				}
				else
				{
					UninstLog->Log("Failed to delete HKLM\\%s", g_cszNAVNTKey);
				}

				if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszMSRunKey, _T("NPS Event Checker")) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKLM\\%s\\NPS Event Checker", g_cszMSRunKey);
				}
				else
				{
					UninstLog->Log("Failed to delete HKLM\\%s\\NPS Event Checker", g_cszMSRunKey);
				}

				if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("*\\Shellex\\ContextMenuHandlers\\NavNT")) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKCR\\*\\Shellex\\ContextMenuHandlers\\NavNT");
				}
				
				if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("Folder\\shellex\\ContextMenuHandlers\\NavNT")) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKCR\\Folder\\shellex\\ContextMenuHandlers\\NavNT");
				}
			}
			else
			{
				if (RegDeleteKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVAPVxDKey) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKLM\\%s", g_cszNAVAPVxDKey);
				}
				else
				{
					UninstLog->Log("Failed to delete HKLM\\%s", g_cszNAVAPVxDKey);
				}

				if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszMSRunKey, _T("Norton Auto-Protect")) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKLM\\%s\\Norton Auto-Protect", g_cszMSRunKey);
				}
				else
				{
					UninstLog->Log("Failed to delete HKLM\\%s\\Norton Auto-Protect", g_cszMSRunKey);
				}

				if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("*\\Shellex\\ContextMenuHandlers\\NortonAntivirus")) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKCR\\*\\Shellex\\ContextMenuHandlers\\NortonAntivirus");
				}

				if (RegDeleteKeyEx(HKEY_CLASSES_ROOT, _T("Folder\\shellex\\ContextMenuHandlers\\NortonAntivirus")) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKCR\\Folder\\shellex\\ContextMenuHandlers\\NortonAntivirus");
				}
			}

			if (DeleteRegKeyName( HKEY_LOCAL_MACHINE, g_cszMSRunKey, _T("NAV DefAlert")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s\\NAV DefAlert", g_cszMSRunKey);
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s\\NAV DefAlert", g_cszMSRunKey);
			}

			if (DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszMSRunKey, _T("Norton eMail Protect")) == ERROR_SUCCESS)
			{
				UninstLog->Log("Deleted HKLM\\%s\\Norton eMail Protect", g_cszMSRunKey);
			}
			else
			{
				UninstLog->Log("Failed to delete HKLM\\%s\\Norton eMail Protect", g_cszMSRunKey);
			}

			// Remove MS Uninstall Key

			HKEY hUninstKey;
			TCHAR szUninstKey[MAX_PATH] = {0};

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVUninstKey, 0, KEY_QUERY_VALUE, &hUninstKey) == ERROR_SUCCESS)
			{
				RegCloseKey(hUninstKey);

				_tcscpy(szUninstKey, g_cszNAVUninstKey);
			}
			else
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszNSWNAVUninstKey, 0, KEY_QUERY_VALUE, &hUninstKey) == ERROR_SUCCESS)
				{
					RegCloseKey(hUninstKey);

					_tcscpy(szUninstKey, g_cszNSWNAVUninstKey);
				}
			}

			// If the uninstall key is not blank, remove it.

			if (*szUninstKey)
			{
				if (RegDeleteKeyEx(HKEY_LOCAL_MACHINE, szUninstKey) == ERROR_SUCCESS)
				{
					UninstLog->Log("Deleted HKLM\\%s", szUninstKey);
				}
				else
				{
					UninstLog->Log("Failed to delete HKLM\\%s", szUninstKey);
				}
			}

			RemoveBackLogKey();

			bReturn = TRUE;
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Norton AntiVirus' registry keys.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveFiles()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// If NAV is installed and it's NAV2000 or 2001.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == 6 || m_iVersionNum == 7 || m_bFORCE == TRUE))
		{
			if (DeleteFilesInDirectory(m_szNAVInstDir) == TRUE)
			{
				bReturn = TRUE;
			}

			// Delete NAV files in Windows directory.

			TCHAR szWindowsDir[MAX_PATH] = {0};
			TCHAR szFileName[MAX_PATH] = {0};
			
			GetWindowsDirectory(szWindowsDir, MAX_PATH);
			
			for (int i = 0; g_cszNAVWinFiles[i] != NULL; i++)
			{
				wsprintf(szFileName, _T("%s\\%s"), szWindowsDir, g_cszNAVWinFiles[i]);

				if (DeleteFile(szFileName) == FALSE)
				{
					FileInUseHandler(szFileName);
				}
			}

			// Delete navapgui.dll on 9x or navap.sys on NT/2K.

			COSInfo osInfo;
			TCHAR szWinSysDir[MAX_PATH] = {0};

			GetSystemDirectory(szWinSysDir, MAX_PATH);

			if (osInfo.IsWinNT() == true)
			{
				wsprintf(szFileName, _T("%s\\drivers\\navap.sys"), szWinSysDir);
			}
			else
			{
				wsprintf(szFileName, _T("%s\\navapgui.dll"), szWinSysDir);
			}

			if (DeleteFile(szFileName) == FALSE)
			{
				FileInUseHandler(szFileName);
			}
		}

		// Delete the tasks file and remove the directory.
		
		RemoveTaskFiles();
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Norton AntiVirus' files.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::RemoveShortcuts()
{
	BOOL bReturn = FALSE;

	try
	{
		// Make sure we are initialized.

		if (m_bInitialized == FALSE)
			Initialize();

		// If NAV is installed and it's NAV2000 or 2001.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == 6 || m_iVersionNum == 7 || m_bFORCE == TRUE))
		{
			// Set the desktop shortcut name according to the version.

			TCHAR szStartMenu[MAX_PATH] = {0};
			TCHAR szNAVStartMenuPath[MAX_PATH] = {0};
			COSInfo osInfo;

            // First try current users group
			HRESULT hRes = SHGetFolderPath(NULL, CSIDL_PROGRAMS, NULL, 0, szStartMenu);

            if (SUCCEEDED(hRes))
            {
                wsprintf(szNAVStartMenuPath, _T("%s\\Norton SystemWorks\\Norton AntiVirus"), szStartMenu);

                if (GetFileAttributes(szNAVStartMenuPath) == -1)
                {
                    if( osInfo.IsWinNT() )
                    {
                        // Try the Common Program Start Menu
                        hRes = SHGetFolderPath(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, szStartMenu);

                        if (SUCCEEDED(hRes))
                        {
                            wsprintf(szNAVStartMenuPath, _T("%s\\Norton SystemWorks\\Norton AntiVirus"), szStartMenu);
                            
                            if (GetFileAttributes(szNAVStartMenuPath) == -1)
                            {
                                // Now try just Norton AntiVirus
                                wsprintf(szNAVStartMenuPath, _T("%s\\Norton AntiVirus"), szStartMenu);
                            }
                        }
                    }
                    else  // 9x Operating System
                    {
                        wsprintf(szNAVStartMenuPath, _T("%s\\Norton AntiVirus"), szStartMenu);
                    }
                }

                if (DeleteFilesInDirectory(szNAVStartMenuPath) == TRUE)
				{
					RemoveDirectory(szNAVStartMenuPath);

					UninstLog->Log("Removed Norton AntiVirus Start Menu folder.");
				}
            }

			// Remove Desktop shortcut

			TCHAR szNAVShortcut[MAX_PATH] = {0};
			TCHAR szNAVShortcutPath[MAX_PATH] = {0};
			TCHAR szDesktopPath[MAX_PATH] = {0};

			if (m_bNAVPro == TRUE)
				_tcscpy(szNAVShortcut, _T("Norton AntiVirus 2001 Professional Edition.lnk"));
			else if (m_iVersionNum == 6)
				_tcscpy(szNAVShortcut, _T("Norton AntiVirus 2000.lnk"));
			else if (m_iVersionNum == 7)
				_tcscpy(szNAVShortcut, _T("Norton AntiVirus 2001.lnk"));

			hRes = SHGetFolderPath(NULL, (osInfo.IsWinNT() ? CSIDL_COMMON_DESKTOPDIRECTORY : CSIDL_DESKTOP), NULL, 0, szDesktopPath);

			if (SUCCEEDED(hRes))
			{
				wsprintf(szNAVShortcutPath, _T("%s\\%s"), szDesktopPath, szNAVShortcut);

				if (DeleteFile(szNAVShortcutPath))
				{
					UninstLog->Log("Removed Norton AntiVirus' desktop shortcut.");
				}
			}

			if (m_bFORCE == TRUE)
			{
				TCHAR szNAVShortcut1[3][MAX_PATH] = {_T("Norton AntiVirus 2001 Professional Edition.lnk"), _T("Norton AntiVirus 2000.lnk"), _T("Norton AntiVirus 2001.lnk")};
				for (int icount=0; icount < 3; icount++)
				{
					wsprintf(szNAVShortcutPath, _T("%s\\%s"), szDesktopPath, szNAVShortcut1[icount]);				
					DeleteFile(szNAVShortcutPath);
				}
			}
		
			TCHAR szStartup[MAX_PATH] = {0};
			TCHAR szNAVStartupPath[MAX_PATH] = {0};

			hRes = SHGetFolderPath(NULL, (osInfo.IsWinNT() ? CSIDL_COMMON_STARTUP : CSIDL_STARTUP), NULL, 0, szStartup);

			if (SUCCEEDED(hRes))
			{
				wsprintf(szNAVStartupPath, _T("%s\\Norton AntiVirus AutoProtect.lnk"), szStartup);

				if (DeleteFile(szNAVStartupPath))
				{
					UninstLog->Log("Removed AutoProtect's startup shortcut.");
				}

			}

			bReturn = TRUE;
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing Norton AntiVirus' shortcuts.");
	}

	return bReturn;
}

BOOL CNAVLegacyUninst::Uninstall(LPCTSTR szDirectory)
{
	BOOL bReturn = FALSE;
	BOOL bForce = FALSE;
	try
	{
		
		UninstLog->Log("Started Uninstalling");		
		if (Initialize(szDirectory) == FALSE)
			throw runtime_error("Failed to Initalize.");
		
		if (szDirectory != NULL)
		{
			m_bFORCE = TRUE;
		}

		// If NAV is installed and it's NAV2000 or 2001, unless the force mode is set to true.

		if (m_bNAVInstalled == TRUE && (m_iVersionNum == 6 || m_iVersionNum == 7) || m_bFORCE == TRUE)
		{			
			if (ShutdownAPUI() == FALSE)
			{
				// Log Error...
			}

			if (RemoveNAVServices() == FALSE)
			{
				// Log Error...
			}

			if (RemoveScheduledTasks() == FALSE)
			{
				// Log Error...
			}

			if (ShutdownPoproxy() == FALSE)
			{
				// Log Error...
			}

			if (RemoveScriptBlocking() == FALSE)
			{
				// Log Error...
			}

			if (UnregisterWithLU() == FALSE)
			{
				// Log Error...
			}

			if (UninstallVirusDefs() == FALSE)
			{
				// Log Error...
			}

			if (RemoveNAVDXStartUp() == FALSE)
			{
				// Log Error...
			}

			if (RemoveNetscapePlugin() == FALSE)
			{
				// Log Error...
			}

			if (UnregisterWithSymEvent() == FALSE)
			{
				// Log Error...
			}

			if (UnregisterCOMObjects() == FALSE)
			{
				// Log Error...
			}

			if (UnregisterNMain() == FALSE)
			{
				// Log Error...
			}

			if (RemoveAdvProtection() == FALSE)
			{
				// Log Error...
			}

			if (UnregisterSAP() == FALSE)
			{
				// Log Error...
			}

			if (RemoveRescue() == FALSE)
			{
				// Log Error...
			}

			if (RemoveVBoxWrapper() == FALSE)
			{
				// Log Error...
			}

			if (RemoveRegkeys() == FALSE)
			{
				// Log Error...
			}

			if (RemoveFiles() == FALSE)
			{
				// Log Error...
			}

			if (RemoveShortcuts() == FALSE)
			{
				// Log Error...
			}

			bReturn = TRUE;
		}
	}
	catch(...)
	{
		bReturn = FALSE;		
		UninstLog->LogEx("Unknown error occurred while uninstalling Norton AntiVirus.");
	}

	UninstLog->Log("Finished Uninstalling");

	return bReturn;
}

BOOL CNAVLegacyUninst::UnregisterCOMDll(TCHAR* pszFileName, TCHAR* pszDirectory)
{
	BOOL bReturn = FALSE;

	try
	{
		if (GetFileAttributes(pszFileName) == -1)
		{
			TCHAR szErrMsg[MAX_PATH] = {0};

			wsprintf(szErrMsg, "Failed to unregister %s, file does not exists", pszFileName);

			throw runtime_error(szErrMsg);
		}

		TCHAR szRegSvr32[MAX_PATH] = {0};
		STARTUPINFO StartupInfo;
		PROCESS_INFORMATION ProcessInfo;
		
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb = sizeof(STARTUPINFO);

		// Create the command to unregister COM object.

		GetSystemDirectory(szRegSvr32, sizeof(szRegSvr32));
		wsprintf(szRegSvr32, _T("%s\\regsvr32.exe /u /s \"%s\""), szRegSvr32, pszFileName);

		if (CreateProcess(NULL, szRegSvr32, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, pszDirectory, &StartupInfo, &ProcessInfo))
		{
			// Wait for regsvr32 to finish, then close the process/thread handles.

			WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
			CloseHandle(ProcessInfo.hThread);
			CloseHandle(ProcessInfo.hProcess);
			bReturn = TRUE;
		}
	}
	catch(exception& ex)
	{
		bReturn = FALSE;

		UninstLog->Log(ex.what());
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while unregistering a COM Dll.");
	}

	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Function:		RemoveService
//
// Description:		Remove the the specified service from the service 
//					control manager database.
//
/////////////////////////////////////////////////////////////////////////////////

BOOL CNAVLegacyUninst::RemoveService(const TCHAR* pszServiceName)
{
	BOOL bReturn = FALSE;
	
	try
	{
		CServiceControl cService;
		DWORD dwStatus = TRUE;
	
		if (ERROR_SUCCESS == cService.Open(pszServiceName))
		{
			if (cService.QueryServiceStatus(&dwStatus))
			{
				// if it is not stopped, try to stop it.

				if (dwStatus != SERVICE_STOPPED)
				{
					if (cService.Stop() != ERROR_SUCCESS)
					{
						UninstLog->Log("Failed to stop %s service.", pszServiceName);
					}
				}
			}

			// don't do error checking here because there are times when delete will
			// fail, but the services are set up correctly 
			
			cService.Delete();

			bReturn = TRUE;
		}
		else    // failed to open it?
		{
			bReturn = FALSE;			
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing a Norton AntiVirus service.");
	}

	return bReturn;
}


/////////////////////////////////////////////////////////////////////////////////
//
// Function:		DeleteRegKeyName
//
// Description:		Remove a specified key name.
//
/////////////////////////////////////////////////////////////////////////////////

LONG CNAVLegacyUninst::DeleteRegKeyName(HKEY hRootKey, LPCTSTR lpszSubKey, LPCTSTR lpszName)
{
	HKEY hRegKey;
	LONG lResult;

	lResult = RegOpenKeyEx(hRootKey, lpszSubKey, 0, KEY_ALL_ACCESS, &hRegKey);

	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegDeleteValue(hRegKey, lpszName);

		RegCloseKey(hRegKey);
	}

	return lResult;
}


/////////////////////////////////////////////////////////////////////////////////
//
// Function:		RemoveBackLogKey
//
// Description:		Check if the value of 
//					HKLM\Software\Microsoft\Windows\CurrentVersion\Run\BACKLOG
//					contains the Nav directory. If yes, remove the key.
//
//					The BACKLOG key exits when the user installed NAV 2001 on
//					a 9x machine and has not rebooted the computer yet.
//
/////////////////////////////////////////////////////////////////////////////////

VOID CNAVLegacyUninst::RemoveBackLogKey()
{
	HKEY hRegKey = NULL;
	DWORD dwDataSize = MAX_PATH;
	TCHAR szValue[MAX_PATH] = {0};
	TCHAR szShortNavDir[MAX_PATH] = {0};
	TCHAR szKeyName[] = _T("BACKLOG");

	// If the nav folder does not exist, exit the function.

	if(!*m_szNAVInstDir)
		return;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszMSRunKey, 0, KEY_ALL_ACCESS, &hRegKey) == ERROR_SUCCESS)
	{
		// Get the value of "BACKLOG" key name.

		if (RegQueryValueEx(hRegKey, szKeyName, NULL, NULL, ( LPBYTE ) szValue, &dwDataSize) == ERROR_SUCCESS)
		{
			// Get short path for Nav directory.

			GetShortPathName(m_szNAVInstDir, szShortNavDir, MAX_PATH);

			// Convert to lower case for comparison.

			LPTSTR lpszBackLogValue = _tcslwr(szValue);
			LPTSTR lpszShortNavDir = _tcslwr(szShortNavDir);
			LPTSTR lpszLongNavDir = _tcslwr(m_szNAVInstDir);

			// Check if BACKLOG contains Nav directory.

			if (_tcsstr(lpszBackLogValue, lpszLongNavDir) || _tcsstr(lpszBackLogValue, lpszShortNavDir))
				DeleteRegKeyName(HKEY_LOCAL_MACHINE, g_cszMSRunKey, szKeyName);
		}

		RegCloseKey(hRegKey);
	}
}

BOOL CNAVLegacyUninst::RemoveTaskFiles()
{
	BOOL bReturn = FALSE;

	try
	{
		TCHAR szTaskDir[MAX_PATH] = {0};

		// If it's NAV2000, delete the task file & folder under the Norton AntiVirus directory.

		if (m_iVersionNum == 6)
		{
			wsprintf(szTaskDir, _T("%s\\Tasks"), m_szNAVInstDir);	
		}

		// If it's NAV2001, delete the task files & folder under the user setting folder.
		// (Typical C:\Documents and Settings\spencer_liang\Application Data\Symantec\
		// Norton AntiVirus\7.0\Tasks).

		if (m_iVersionNum == 7)
		{
			HRESULT hRes = SHGetFolderPath(NULL, CSIDL_APPDATA , NULL, 0, szTaskDir);

			if (SUCCEEDED(hRes))
			{
				wsprintf(szTaskDir, _T("%s\\Symantec\\Norton AntiVirus\\7.0\\Tasks"), szTaskDir);
			}
		}

		if (*szTaskDir)
		{
			DeleteFilesInDirectory(szTaskDir);

			RemoveDirectory(szTaskDir);

			bReturn = TRUE;
		}
	}
	catch(...)
	{
		bReturn = FALSE;

		UninstLog->LogEx("Unknown error occurred while removing task files.");
	}

	return bReturn;
}
