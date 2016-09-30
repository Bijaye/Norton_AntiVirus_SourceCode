////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "navlaunchconditions.h"

// InstallToolBox Headers
#include "NAVDetection.h"
#include "CloseSYMApps.h"
#include "FileAction.h"
#include "LaunchConditionDlg.h"
#include "Instopts.h"
#include "isLocations.h"

//
// list of files that often get marked for deletion 
// when uninstalling NAV.  we need to check for this.
//
TCHAR* CNAVLaunchConditions::g_szNavFiles[] =
{
	_T("runsched.exe"),
	_T("defloc.dat"),
	_T("msievent.log"),
	_T("advchk.iss"),
	_T("setup.log"),
	_T("navopts.bak"),
	_T("*.gid"),
	_T("resqloc.dat"),
	_T("AVApp.log"),
	_T("AvError.log"),
	_T("AVVirus.log"),
	_T("SYMREDIR.DLL"), 
	_T("NAVSHEXT.DLL"), 
	_T("NAVAPW32.EXE"), 
	_T("APWUTIL.DLL"),
	_T("APWCMD9X.DLL"),
	_T("APWCMDNT.DLL"),
	_T("NAVPROXY.DLL"),
	_T("CCEVTMGR.EXE"),
	_T("NAVEVENT.DLL"),
	_T("CCEVT.DLL"),
	_T("SYMSTORE.DLL"),
	_T("navprod.dll"),
	_T("settings.dat"),
	_T("settings.bak"),
	NULL
};

//
// list of services that often get marked for deletion
// when uninstalling nav we need to check for this
//
TCHAR* CNAVLaunchConditions::g_szServiceNames[] = 
{
	_T("SymEvent"),		//SYMEVENT SERVICE NAME
	_T("navapsvc"),		//NAVAP SERVICE NAME
	_T("NAVAP"),		//NAVAP SYS NAME
	_T("NAVEX15"),		//NAVEX SERVICE NAME
	_T("NAVENG"),		//NAVENG SERVICE NAME
	_T("SYMTDI"),		//SYMTDI SERVICE NAME
	_T("NAVAPEL"),		//NAVCE SERVICE NAME
	NULL
};

CSetupInfo * CNAVLaunchConditions::m_pNAVInfo = NULL;

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckInstallConditions()
//	checks all launch conditions that need to be met before install
//	
//	inputs: none
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckInstallConditions(void)
{	
	pfnLaunchConditon ConditionList[] =
	{
		// check nav specific launch conditionss:
		CLaunchConditions::CheckForServer,
		CNAVLaunchConditions::CheckForNIS,
		CNAVLaunchConditions::CheckForRunningNAV,
		CNAVLaunchConditions::CheckForCorpNAV,
		CNAVLaunchConditions::CheckForServicesMarkedForDeletion,
		CNAVLaunchConditions::CheckForFilesMarkedForDeletion,
		CNAVLaunchConditions::CheckOverInstall,
		NULL
	};

	// check the nav specific launch conditions first
	if(!CLaunchConditions::CheckListOfLaunchConditions(ConditionList))
	{
		return false;
	}

	// now check the generic launch conditions
	return CLaunchConditions::CheckInstallConditions();
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckUnInstallConditions()
//	checks all launch conditions that need to be met before uninstall
//	
//	inputs: none
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckUninstallConditions(void)
{ 
	pfnLaunchConditon ConditionList[] =
	{
		// check nav specific launch conditionss:
		CNAVLaunchConditions::CheckForVistaMigration,
		CNAVLaunchConditions::CheckForRunningNAV,
		NULL
	};
	
	// check the nav specific launch conditions first
	if(!CLaunchConditions::CheckListOfLaunchConditions(ConditionList))
	{
		return false;
	}

	// now check the the generic launch conditions
	return CLaunchConditions::CheckUninstallConditions();
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckForNIS()
//	checks to see if NIS is installed.
//	
//	inputs: dwResID - Resource ID of the string to display to the user on
//			launch condition failure.
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckForNIS(DWORD &dwMsgResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{
	const TCHAR* BlockedProductCodes[]			= { _T("{E16453A0-3AED-11D3-AF47-00600811C705}"),    //NIS 1.0
													_T("{7C1A7F8E-3662-4B0C-A573-C680C043E80E}"),    //NIS 2.0
													_T("{C26B870B-08E6-442A-AAE3-6A250CCFE94E}"),    //NPF 2.0
													_T("{0AE91EA0-83D4-49B9-ADF7-B769F7D091A4}"),    //NIS 3.0
													_T("{CCAE58EF-511C-44D7-81F2-D32876A04EEA}"),    //NPF 3.0
													_T("{71D03DD3-C6D9-4503-A1CC-FBA576F6CFE3}"),    //NIS 4.0
													_T("{1D54BDAC-C362-4849-B52F-814319CF5057}"),    //NPF 4.0
													_T("{72AAF846-3C35-4391-9459-CC7B6EC7E07A}"),    //NIS Pro 4.5
													_T("{AFD2C5B5-BF78-47B6-9569-755448C0D0EE}"),    //NIS 6.0
													_T("{15BFECE8-A100-4861-B92B-1EFF76683C23}"),    //NPF 6.0
													_T("{93DF5BBA-2992-482F-B11D-91027EC8AC7D}"),    //NIS Pro 6.0
													_T("{A93C9E60-29B6-49da-BA21-F70AC6AADE20}"),    //Old NIS product code.
													_T("{5AA2CD16-706F-41f3-87C5-2B5A031F2B3B}"),	 //New NIS product code.
													_T("{B4094407-2F69-44bb-9DD7-9470FBD9F521}"),	 //NIS Essentials product code.
													NULL};
	
	for (unsigned int i = 0; BlockedProductCodes[i]; i++)
	{
		INSTALLSTATE isNISState = MsiQueryProductState(BlockedProductCodes[i]);

		if (isNISState == INSTALLSTATE_DEFAULT || isNISState == INSTALLSTATE_ABSENT)
		{
			dwMsgResID		= IDS_NIS_INSTALLED;
			dwUrlResID		= ADD_REMOVE_CPL;
			dwUrlDescResID	= IDS_ARP_DESC_LC;
			return false;
		}
	}

	//NPF 7.0,8.0 and 9.0 had the same product code. ({3BD0196C-6553-460c-A0C4-90D8AE5D60D2})
	//Also we want to block on 7.0 but not on 8.0 & 9.0
	//Check the version of NPF installed
	INSTALLSTATE isNPFState = MsiQueryProductState(_T("{3BD0196C-6553-460c-A0C4-90D8AE5D60D2}"));	
	if (isNPFState == INSTALLSTATE_DEFAULT || isNPFState == INSTALLSTATE_ABSENT)
	{
		//Check for NISVer.dat under the NPF Install directory
		InstallToolBox::ITB_VERSION itbExistingNPFVer;
		if(GetExistingNPFVersion(&itbExistingNPFVer))
		{
			//If NPF 7.* - Block
			if(7 == itbExistingNPFVer.nMajorHi)
			{
				dwMsgResID		= IDS_NIS_INSTALLED;
				dwUrlResID		= ADD_REMOVE_CPL;
				dwUrlDescResID	= IDS_ARP_DESC_LC;
				return false;
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckForRunningNAV()
//	checks to see if NAV is running.  closes the windows that can be
//	closed, else fails the launch the condition
//	
//	inputs: dwResID - Resource ID of the string to display to the user on
//			launch condition failure.
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckForRunningNAV(DWORD &dwMsgResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{
	if(!InstallToolBox::CloseSYMApps() || InstallToolBox::CheckForRunningSYMApps())
	{
		dwMsgResID = IDS_NAV_RUNNING;
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckForCorpNAV()
//	Checks to see if NAV Corportate version is installed.
//	
//	inputs: dwResID - Resource ID of the string to display to the user on
//			launch condition failure.
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckForCorpNAV(DWORD &dwMsgResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{		
	if(InstallToolBox::IsNAVCorpInstalled())
	{
		dwMsgResID = IDS_NAVCORP_INSTALLED;
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckForServicesMarkedForDeletion()
//	Checks to see if any NAV services are marked for deletion
//	
//	inputs: dwResID - Resource ID of the string to display to the user on
//			launch condition failure.
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckForServicesMarkedForDeletion(DWORD &dwMsgResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{
    // Checks if it's marked for deletion 
    for (int i = 0; g_szServiceNames[i] != NULL; i++)
	{    			
		if (TRUE == InstallToolBox::IsServiceMarkedForDeletetion(g_szServiceNames[i]))
		{
			dwMsgResID = IDS_SERVICES_MARKED;
            return false;
		}
    }    
        
	return true;
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckForFilesMarkedForDeletion()
//	Checks to see if any NAV files are marked for deletion
//	
//	inputs: dwResID - Resource ID of the string to display to the user on
//			launch condition failure.
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckForFilesMarkedForDeletion(DWORD &dwMsgResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{
	// iterate through to see if our NAV files are marked for deletion.
	for(int i=0; g_szNavFiles[i] != NULL; i++)
	{								
		if(InstallToolBox::UnMarkFilesForDeletion(g_szNavFiles[i], FALSE))
		{	
			dwMsgResID = IDS_FILES_MARKED;
			return false;
		}
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::CheckForServer()
//	Checks if the OS a server
//	
//	inputs: dwResID - Resource ID of the string to display to the user on
//			launch condition failure.
//	return value: true if all launch conditions pass, else false			
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::CheckForServer(DWORD &dwMsgResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{
	if(IsServerInstallAllowed())
	{
		return true;
	}
	else
	{
		return CLaunchConditions::CheckForServer(dwMsgResID, dwUrlResID, dwUrlDescResID);
	}
}

///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::ShutdownWndByClass()
//	checks the "secret" key to see if its ok to install on servers
//	
//	inputs: none
//	return value: true if server install allowed, else false
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::IsServerInstallAllowed(void)
{
	CRegKey hKeyForOverride;
	DWORD dwAllowOverride = 0;
	
	if(ERROR_SUCCESS == hKeyForOverride.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"), KEY_READ))
	{
		if(ERROR_SUCCESS == hKeyForOverride.QueryDWORDValue(_T("AllowServer"), dwAllowOverride))
			return true;
	}
	
	return false;
}


///////////////////////////////////////////////////////////////////
//CNAVLaunchConditions::ShutdownWndByClass()
//	Shuts down a window by its class name
//	
//	inputs: lpWndClassName - class name of the window to close
//			uMsg - message to post in order to close the window
//	return value: true if successfull, else false
//
///////////////////////////////////////////////////////////////////
bool CNAVLaunchConditions::ShutdownWndByClass( LPTSTR lpWndClassName, UINT uMsg )
{
	int i = 0;
	HWND hWnd = FindWindow(lpWndClassName, NULL);

    while(hWnd && i < 500)
	{
		if (hWnd) 
		{
			PostMessage(hWnd, uMsg, 0, 0);
		}
		
		Sleep(50);
		hWnd = FindWindow(lpWndClassName, NULL);
		++i;
    }
	
	if( hWnd == NULL )
		return true;
	else	
		return false;
}

bool CNAVLaunchConditions::CheckOverInstall(DWORD &dwMsgResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{
	return m_pNAVInfo->OverInstallLaunchCondition(dwMsgResID, dwUrlResID, dwUrlDescResID);
}

bool CNAVLaunchConditions::GetExistingNPFVersion(InstallToolBox::LP_ITB_VERSION itbExistingVer)
{
	CRegKey rk;
	if(ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, isRegLocations::szInstalledAppsKey))
	{
		CString sBase;
		DWORD dwCount = MAX_PATH;
		BOOL bLoaded = FALSE;
		if(ERROR_SUCCESS == rk.QueryStringValue(isRegLocations::szInstalledAppsValue, sBase.GetBuffer(dwCount), &dwCount) && (0 < dwCount))
			bLoaded = TRUE;
		sBase.ReleaseBuffer();

		dwCount = MAX_PATH;
		if(!bLoaded)
		{
			if(ERROR_SUCCESS == rk.QueryStringValue(isRegLocations::szInstalledAppsValue_Legacy, sBase.GetBuffer(dwCount), &dwCount) && (0 < dwCount))
				bLoaded = TRUE;
			sBase.ReleaseBuffer();
		}


		if(bLoaded)
		{
			CString sFile = sBase;
			sFile +=_T("\\nisver.dat");

			if(InstallToolBox::FileExists(sFile))
			{
				CString sVer;
				GetPrivateProfileString(_T("Versions"), _T("PublicVersion"), NULL, sVer.GetBuffer(MAX_PATH), MAX_PATH, sFile);
				sVer.ReleaseBuffer();

				if(sVer.GetLength() > 0)
				{
					InstallToolBox::GetVersionFromString(sVer , *itbExistingVer);
					CCTRACEW(_T("Installed version = %s"), sVer);
					return true;
				}
			}

			// nisver.dat failed so try nisplug.dll
			sFile = sBase;
			sFile +=_T("\\nisplug.dll");
			if(InstallToolBox::FileExists(sFile))
			{
				InstallToolBox::GetFileVersion(sFile, itbExistingVer);

				CString sVer;
				InstallToolBox::GetStringFromVersion(*itbExistingVer, sVer.GetBuffer(MAX_PATH));
				sVer.ReleaseBuffer();
				CCTRACEW(_T("Installed version = %s"), sVer);

				return true;
			}
		}
	}
	return false;
}

bool CNAVLaunchConditions::CheckForVistaMigration(DWORD& dwResID, DWORD &dwUrlResID, DWORD &dwUrlDescResID)
{
	return true;

	//{
	//	dwResID = IDS_VISTA_MIGRATION;
	//	dwUrlResID = IDS_VISTA_MIGRATION_URL;
	//	return false;
	//}
}
