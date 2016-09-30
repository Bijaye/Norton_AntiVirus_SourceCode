#include "stdafx.h"
#include "navlaunchconditions.h"

// InstallToolBox Headers
#include "NAVDetection.h"
#include "CloseSYMApps.h"
#include "FileAction.h"
#include "LaunchConditionDlg.h"
#include "Instopts.h"

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