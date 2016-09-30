////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// UpgradTasks.cpp
//
// --Contains exported functions used in MSI custom actions for performing any upgrade related
// tasks
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "helper.h"
#include "InstOptsNames.h"
#include "OptNames.h"
#include "OSInfo.h"

#include "NAVOptHelperEx.h"

// CC headers
#include "ccLibStaticLink.h"
#include "ccSymCommonClientInfo.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("navinst"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// installtoolbox headers
#include "unmark.h"
#include "fileaction.h"
#include "NAVDetection.h"
#include "utilities.h"
#include "instopts.h"
#include "shutdown.h"
#include "CustomAction.h"

using namespace InstallToolBox;


/////////////////////////////////////////////////////////////////////	////////////////////////
// CleanShortCuts() : deletes any .lnk files containining "Norton AntiVirus" (not localized)
//	from the all users and current user startmenu and desktop
//	
//	--needed b/c of of Win2K migration moves the startmenu shortcuts and our uninstall
//	  doesn't remove them on install over case.
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CleanShortCuts()
{
	CCTRACEI(_T("CleanShortCuts() Starting."));

	TCHAR szOriginalWorkDir[MAX_PATH] = {0};
	
	try
	{
		//get current working dir and save it
		GetCurrentDirectory(MAX_PATH, szOriginalWorkDir);
		
		//set up the deletion structure and the values that don't change
		SHFILEOPSTRUCT shDeleteLinksOp;
		shDeleteLinksOp.hwnd = NULL;
		shDeleteLinksOp.wFunc = FO_DELETE;
		shDeleteLinksOp.pFrom = _T("*Norton AntiVirus*.lnk");
		shDeleteLinksOp.pTo = NULL;
		shDeleteLinksOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_FILESONLY;
		
		TCHAR szCurrentPath[MAX_PATH] = {0};

		//find the all users desktop path and delete and NAV links in it
		if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, 0, szCurrentPath)))
		{
			SetCurrentDirectory(szCurrentPath);
			SHFileOperation(&shDeleteLinksOp);
		}
		
		//find the current users desktoppath and delete any NAV links in it
		if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, szCurrentPath)))
		{
			SetCurrentDirectory(szCurrentPath);
			SHFileOperation(&shDeleteLinksOp);
		}
		
		//find the all users start menu and delete any NAV links in it
		if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, szCurrentPath)))
		{
			_tcscat(szCurrentPath, _T("\\Norton AntiVirus"));
			SetCurrentDirectory(szCurrentPath);
			SHFileOperation(&shDeleteLinksOp);
		}

		//find the current users start menu and delete any NAV links in it
		if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAMS, NULL, 0, szCurrentPath)))
		{
			_tcscat(szCurrentPath, _T("\\Norton AntiVirus"));
			SetCurrentDirectory(szCurrentPath);
			SHFileOperation(&shDeleteLinksOp);
		}
	}
	catch(exception &ex)
	{
		CCTRACEE(CA2CT(ex.what()));
	}
	
	//restore current working dir
	SetCurrentDirectory(szOriginalWorkDir);

	CCTRACEI(_T("CleanShortCuts() Finished."));

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  Bump the ref counts of various drivers to prevent them from being marked for deletion
// 
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL BumpRefCounts()
{
	try
	{
		CCTRACEI(_T("BumpRefCounts() Starting."));

		//
		//Need to "bump" up Symevent ref counts.. just install a dummy symevent id
		//
		TCHAR szPathName[MAX_PATH] = {0};
		TCHAR szLongPathName[MAX_PATH] = {0};
		DWORD dwSize = sizeof(szPathName);
		
		GetModuleFileName(NULL, szLongPathName, MAX_PATH);
		TCHAR *pBackSlash = _tcsrchr(szLongPathName, _T('\\'));
		*pBackSlash = _T('\0');
		
		//instead use create process and wait for it to finish
		_tcscat(szLongPathName, _T("\\Support\\SEVINST\\sevinst.exe"));
		GetShortPathName(szLongPathName, szPathName, MAX_PATH);
		_tcscat(szPathName, _T(" /Q -consumer UPGRADE"));
		
		ExecuteProgram(_T(""), szPathName, TRUE);
	}
	catch(exception &ex)
	{
		CCTRACEE(CA2CT(ex.what()));
	}

	CCTRACEI(_T("BumpRefCounts() Finished."));

	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//  Bring the Ref counts back down to normal
//	 (NOTE: it is important that this action gets called BEFORE InstallServices action, because
//		it unmarks some services for deletion and if this doesn't happen prior to attempting to
//		re-install them, then the SCM calls will fail)
// 
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall DecRefCounts(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug DecRefCounts()", "Debug Break", NULL);
#endif
	
	try
	{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logStatus,
			_T("DecRefCounts() Starting."));

		// since symsetup calls our "upgrade" custom action we need to make sure that NAV we only
		// decrement symevent when we have actually upgraded something, look for the OLD NAV key that we set
		ATL::CRegKey TempRegKeys;
		DWORD dwSize = MAX_PATH;
		TCHAR szOLDNAV[MAX_PATH] = {0};
		UINT ur = TempRegKeys.Open(HKEY_LOCAL_MACHINE, szTempRegKeys, KEY_READ);
		if(ERROR_SUCCESS == ur)
		{
			ur = TempRegKeys.QueryStringValue(_T("OLDNAV"), szOLDNAV, &dwSize);
			if(ERROR_SUCCESS != ur)
			{	
				return ERROR_SUCCESS;	//dont decrement b/c we didn't increment
			}
		}
		else
		{
			InstallToolBox::CCustomAction::LogMessage(hInstall,
				InstallToolBox::CCustomAction::LogSeverity::logAlways,
				_T("DecRefCounts(): couldn't open %s (0x%08x)"), szTempRegKeys, ur);
			return ERROR_SUCCESS;
		}
	
		//
		//Need to restore the symevent back to normal refcounts. uninstall the dummy key
		//
		TCHAR szPathName[MAX_PATH] = {0};
		TCHAR szLongPathName[MAX_PATH] = {0};
		dwSize = sizeof(szPathName);
		
		//
		//sevinst is "smart" and double checks that there is an installedapps key for the product, if
		//there isn't then it will just remove the id without decrementing the refcounts.. so lets create it -- per Leo Cohen
		//
		HKEY hInstalledAppsKey = NULL;
		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, 0, KEY_ALL_ACCESS, &hInstalledAppsKey))
		{
			const TCHAR szTempValue[] = _T("TempDir");
			RegSetValueEx(hInstalledAppsKey, _T("UPGRADE"), 0, REG_SZ, (BYTE*)szTempValue, sizeof(szTempValue));
		}

		//
		//Now we can call sevinst to uninstall the dummy ID
		//
		CNAVInfo NAVInfo;
		_tcscpy ( szLongPathName, NAVInfo.GetSymantecCommonDir());
		
		wsprintf(szLongPathName, _T("%s\\sevinst.exe"), NAVInfo.GetSymantecCommonDir());
		GetShortPathName(szLongPathName, szPathName, MAX_PATH);
		_tcscat(szPathName, _T(" /Q /U -consumer UPGRADE"));
		
		ExecuteProgram(_T(""), szPathName, TRUE);

		//
		//Now that we have decremented the refcounts, remove our dummy Installed apps key
		//
		if(hInstalledAppsKey)
		{
			RegDeleteValue(hInstalledAppsKey, _T("UPGRADE"));
			RegCloseKey(hInstalledAppsKey);
		}

			
		//
		//Some files get marked for deletion in the Winit.ini file on Win9x.  We need to remove these from 
		//the list of files to delete. The reason they get marked for deltion is b/c they are in use when 
		//we uninstall the previous product.  If the previous product is some version of NAV9.0 (eg. NAVPRO over NAV)
		//then most of the files are the same version and they get marked for deletion.
		//
		TCHAR szNewDir[MAX_PATH] = {0};
		TCHAR szOldDir[MAX_PATH] = {0};
		HKEY hTmpKey = NULL;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szTempRegKeys, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, &hTmpKey) == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(szOldDir);
			if(ERROR_SUCCESS == RegQueryValueEx(hTmpKey, _T("OLDNAV"), 0, NULL, (BYTE*)szOldDir, &dwSize))
			{
				dwSize = sizeof(szNewDir);
				if(ERROR_SUCCESS == RegQueryValueEx(hTmpKey, _T("SSINSTALLDIR"), 0, NULL, (BYTE*)szNewDir, &dwSize))
				{	
					//
					//we only want to unmark the following files for deletion if the installation directory is the same
					//as it was for the previously installed version, otherwise the file should get deleted
					//

					//
					// NIS is able to completely remove their directory on overinstalls, even with files in use.
					// If the directory has been deleted, then it's possible for the SFN path to point to the new 
					// files.  In this case it's safest to unmark the files.
					// Defect 1-58Q57F
					//
					DWORD dwOldDirAttributes = ::GetFileAttributes(szOldDir);
					if(0 == _tcsicmp(szOldDir, szNewDir) ||
						FILE_ATTRIBUTE_DIRECTORY != dwOldDirAttributes)
					{
						UnMarkFilesForDeletion(_T("S32NAVO.dll"));
						UnMarkFilesForDeletion(_T("APWUTIL.DLL"));
						UnMarkFilesForDeletion(_T("APWCMD9X.DLL"));
						UnMarkFilesForDeletion(_T("APWCMDNT.DLL"));
						UnMarkFilesForDeletion(_T("DEFALERT.DLL"));
						UnMarkFilesForDeletion(_T("NAVERROR.DLL"));
						UnMarkFilesForDeletion(_T("NAVAPW32.DLL"));
						UnMarkFilesForDeletion(_T("NAVPROD.DLL"));
						UnMarkFilesForDeletion(_T("BRANDING.INI"));
						UnMarkFilesForDeletion(_T("NAVOPTS.BAK"));
						UnMarkFilesForDeletion(_T("NAVOPTS.DLL"));
						UnMarkFilesForDeletion(_T("N32EXCLU.DLL"));
						UnMarkFilesForDeletion(_T("NAVEMAIL.DLL"));
						UnMarkFilesForDeletion(_T("SAVRT32.DLL"));
						UnMarkFilesForDeletion(_T("NAVEVENT.DLL"));
						UnMarkFilesForDeletion(_T("N32CALL.DLL"));
			            UnMarkFilesForDeletion(_T("NAVLUCBK.DLL"));
			            UnMarkFilesForDeletion(_T("PATCH32I.DLL"));
						UnMarkFilesForDeletion(_T("NAVAPSCR.DLL"));
						UnMarkFilesForDeletion(_T("NAVLNCH.DLL"));
						UnMarkFilesForDeletion(_T("NAVSTATS.DLL"));
						UnMarkFilesForDeletion(_T("NAVUI.DLL"));
						UnMarkFilesForDeletion(_T("AVRES.DLL"));
						UnMarkFilesForDeletion(_T("NAVLCOM.DLL"));
						UnMarkFilesForDeletion(_T("NAVOPTRF.DLL"));
						UnMarkFilesForDeletion(_T("STATUSHP.DLL"));
						UnMarkFilesForDeletion(_T("CCAVMAIL.DLL"));
                        UnMarkFilesForDeletion(_T("NAVShExt.dll"));
                        UnMarkFilesForDeletion(_T("NAVAP32.DLL"));
                        UnMarkFilesForDeletion(_T("Portal"));
                        UnMarkFilesForDeletion(_T("Incoming"));

						// virus def files
						UnMarkFilesForDeletion(_T("ECMSVR32.DLL"));
						UnMarkFilesForDeletion(_T("NAVENG32.DLL"));
						UnMarkFilesForDeletion(_T("NAVEX32A.DLL"));
						
                        InstallToolBox::CInstoptsDat Instopts;
                        DWORD dwIWP = 1;
                        Instopts.ReadDWORD(_T("INSTALL:IWP"), dwIWP, 1);
                        if(1 == dwIWP)
                        {
                            UnMarkFilesForDeletion(_T("CCFWSETG.DLL"));
                            UnMarkFilesForDeletion(_T("SYMFWAGT.DLL"));
                            UnMarkFilesForDeletion(_T("NPFMNTOR.EXE"));
						    UnMarkFilesForDeletion(_T("IWP.DLL"));
                        }
					}
				}
			}
			
			RegCloseKey(hTmpKey);
		}
		
		//
		//these files go to the same place no matter where the product is installed to and we
		//should always unmark them.
		//
		UnMarkFilesForDeletion(_T("NAVShExt.dll"));
		UnMarkFilesForDeletion(_T("CCERRDSP.DLL"));
		UnMarkFilesForDeletion(_T("CCAPP.EXE"));
		UnMarkFilesForDeletion(_T("CCEMLPXY.DLL"));
		UnMarkFilesForDeletion(_T("CCEMLPXY.DAT"));
		//UnMarkFilesForDeletion(_T("CCTRUST.DLL"));
		UnMarkFilesForDeletion(_T("CCVRTRST.DLL"));
		UnMarkFilesForDeletion(_T("CCIMSCAN.DLL"));
		UnMarkFilesForDeletion(_T("CCEVTMGR.EXE"));				
		UnMarkFilesForDeletion(_T("CCWEBWND.DLL"));
        UnMarkFilesForDeletion(_T("CCPROSUB.DLL"));
        UnMarkFilesForDeletion(_T("CCSETEVT.DLL"));
		UnMarkFilesForDeletion(_T("CCL40.DLL"));
		//UnMarkFilesForDeletion(_T("CCEVT.DLL"));
		//UnMarkFilesForDeletion(_T("CCREGMON.DLL"));
		UnMarkFilesForDeletion(_T("LIVETRI.ZIP"));
		UnMarkFilesForDeletion(_T("NMAIN.EXE"));
		UnMarkFilesForDeletion(_T("DJSLCOM.DLL"));
		UnMarkFilesForDeletion(_T("DJSALERT.DLL"));
		UnMarkFilesForDeletion(_T("SymLTCOM.dll"));
		UnMarkFilesForDeletion(_T("settings.dat"));
		UnMarkFilesForDeletion(_T("settings.bak"));
	}
	catch(exception &ex)
	{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logError,
			CA2CT(ex.what()));
	}

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("DecRefCounts() Finished."));
		
	return ERROR_SUCCESS;
}


BOOL DeleteEmptyParentDirectories(ATL::CString sOldQuarPath, ATL::CString sProgramFilesFolder)
{
	BOOL bRemovedAny = FALSE;

	int iLen = sProgramFilesFolder.GetLength();
	if(_T('\\') == sProgramFilesFolder[iLen - 1])
	{
		sProgramFilesFolder.Left(iLen - 1);
	}

	while(!sOldQuarPath.IsEmpty() && (0 != sOldQuarPath.CompareNoCase(sProgramFilesFolder)))
	{
		if(!FileExists(sOldQuarPath))
		{
			// Directory doesn't exist, go up one directory
			int iFindBackslash = sOldQuarPath.ReverseFind(_T('\\'));
			if(-1 == iFindBackslash) // No more parent paths to go up.
			{
				break;
			}
			sOldQuarPath = sOldQuarPath.Left(iFindBackslash);
			continue;
		}

		// Remove directory if empty
		BOOL bRemoved = RemoveDirectory(sOldQuarPath);
		DWORD dwLastError = GetLastError();
		bRemovedAny = bRemovedAny || bRemoved;
		if(bRemoved)
		{
			CCTRACEI(_T("%s - Removed empty parent directory: %s"), __FUNCTION__, sOldQuarPath);
		}
		else
		{
			return bRemovedAny;
		}
	}

	return bRemovedAny;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  Stop the AP service before trying to uninstall the previous NAV.  A bug in NAV2002
//	uninstaller causes the service to be deleted before it is completely stopped (the call
//	to stop it is not syncronous).  We need to shut it down ourselves to make sure this 
//	doesn't happen.
// 
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL StopOldAPService()
{
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schAPService = NULL;

	try
	{
		COSInfo OSInfo;
		
		//
		//only do this on WindowsNT platforms
		//
		if(!OSInfo.IsWinNT())
		{
			return ERROR_SUCCESS;
		}

		//
		//get a handle to the scm
		//
		schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
		
		if (!schSCManager)
		{
			throw runtime_error("Could not open Service Control Manager.");
		}
		
		//
		//get a handle to AP service
		//
		schAPService = OpenService(schSCManager, _T("NAVAPSVC") /* 2002 Service Name */,
			SERVICE_STOP);

		if(!schAPService)
		{
			throw runtime_error("Could not open AutoProtect Service.");
		}
		
		//
		//stop the service
		//
		SERVICE_STATUS xAPStatus = {0};
		ControlService(schAPService, SERVICE_CONTROL_STOP, &xAPStatus);
		
		//
		//wait around until the service is stopped, give up after 10 secs
		//
		int iCtr = 0;
		QueryServiceStatus(schAPService, &xAPStatus);
		while( ( xAPStatus.dwCurrentState != SERVICE_STOPPED ) &&
			   ( iCtr++ < 10 ) )
		{
			Sleep( 1000 );
			QueryServiceStatus( schAPService, &xAPStatus );
		}

		if ( xAPStatus.dwCurrentState != SERVICE_STOPPED )
			throw runtime_error("Could not stop AutoProtect Service.");
	}	
	catch(exception &ex)
	{
		CCTRACEE(CA2CT(ex.what()));
	}

	//
	//clean up
	//
	if(schSCManager)
	{
		CloseServiceHandle(schSCManager);
	}

	if(schAPService)
	{
		CloseServiceHandle(schAPService);
	}
	
	CCTRACEI(_T("StopOldAPService() Finished."));
		
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
//PreUpgrade(): Export called by SymSetup to upgrade NAV
//	--this function gets called by symsetup prior to removing the previously installed
//		symsetup products. we prepare for the uninstall in here
//	--function assumes that Upgrade() will get called by symsetup after this
///////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall PreUpgrade()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug PreUpgrade()", "Debug Break", NULL);
#endif

	CCTRACEI(_T("PreUpgrade() Starting."));

	InstallToolBox::ITB_VERSION ExistingNAVver = {0};
	CNAVInfo NAVInfo;

	if(InstallToolBox::IsNAVInstalled())
	{
		// store the OLDNAV location because it will be needed to determine whether to
		// move the quarantine files and which files to unmarkfor deletion
		ATL::CRegKey RegKey;
		UINT ur = RegKey.Open(HKEY_LOCAL_MACHINE, szTempRegKeys);

		// This key may not exist yet; so if we couldn't open it, attempt
		// to create it.
		if(ERROR_SUCCESS != ur)
			ur = RegKey.Create(HKEY_LOCAL_MACHINE, szTempRegKeys);

		if(ERROR_SUCCESS == ur)
		{
			ur = RegKey.SetStringValue(_T("OLDNAV"), NAVInfo.GetNAVDir());
			if(ur != ERROR_SUCCESS)
			{
				CCTRACEE(_T("PreUpgrade(): Set %s\\OLDNAV failed (0x%08x)"), szTempRegKeys, ur);
			}
		}
		else
		{
			CCTRACEE(_T("PreUpgrade(): Open/Create %s key failed (0x%08x)"), szTempRegKeys, ur);
		}
		
		// bump the symevent refcount so the drivers dont get deleted
		BumpRefCounts();
		
		// sometimes the AP service won't stop during the uninstall, so we'll stop it here
		StopOldAPService();
        
        InstallToolBox::ShutdownWndByClass(_T("ccAppWindow"), WM_CLOSE);
	}

	CCTRACEI(_T("PreUpgrade() Finished."));

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  DropIamApp(): Fixes an issue with NIS2003 blocking our digital signature checks
//	and causing a hang.  If one of our apps tries to check digital signatures before ccApp
//	is running then it won't prompt to allow access and it causes our app hang..
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL DropIamApp()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug DropIamApp()", "Debug Break", NULL);
#endif

	CCTRACEI(_T("DropIamApp() Starting."));

    BOOL bRet = TRUE;

	try
    {
		// First find the NIS directory
        ccLib::CString sNisDir;
        DWORD dwSize = MAX_PATH;
        ccSym::CCommonClientInfo::GetNISDirectory(sNisDir);

        if( InstallToolBox::FileExists(sNisDir) )
		{
			// figure out which version of NIS is installed
			TCHAR szNisVerDat[MAX_PATH] = {0};
			TCHAR szNisVersion[MAX_PATH] = {0};

			wsprintf(szNisVerDat, _T("%s\\nisver.dat"), sNisDir);
			GetPrivateProfileString(_T("Versions"), _T("PublicVersion"), _T(""), szNisVersion, MAX_PATH, szNisVerDat);
			if(0 != _tcscmp(szNisVersion, _T("")))
			{
				InstallToolBox::ITB_VERSION nisVer;
				InstallToolBox::GetVersionFromString(szNisVersion, nisVer);
				if(nisVer.nMajorHi == 6 && nisVer.nMinorHi < 2)
				{
					// Get the IamApp.exe file path
					// located in the NAV folder
					TCHAR szIamFile[MAX_PATH] = {0};
					::GetModuleFileName(NULL, szIamFile, MAX_PATH);
					TCHAR* pszBackSlash = _tcsrchr(szIamFile, _TCHAR('\\'));
					if( pszBackSlash )
						*pszBackSlash = NULL;
					_tcscat(szIamFile, _T("\\NAV\\Iamapp.dll"));

					if( InstallToolBox::FileExists(szIamFile) )
					{
						TCHAR szTmpFile[MAX_PATH] = {0};
						TCHAR szTargetFile[MAX_PATH] = {0};
						
						wsprintf(szTmpFile, _T("%s\\Iamapp2.dll"), sNisDir);
						wsprintf(szTargetFile, _T("%s\\Iamapp.dll"), sNisDir);
						
						// we have to copy to the system first b/c MoveFileEx requires that the src file not be read-only
						::CopyFile(szIamFile, szTmpFile, TRUE);
						::SetFileAttributes(szTmpFile, FILE_ATTRIBUTE_NORMAL);

						// mark the file for update on the next reboot.
						::MoveFileEx(szTmpFile, szTargetFile, MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING);
					}
				}
			}
		}

	}
	catch(exception& ex)
	{
		CCTRACEE(CA2CT(ex.what()));
        bRet = FALSE;
	}

	CCTRACEI(_T("DropIamApp() Finished."));

	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////
//Upgrade(): Export called by SymSetup to upgrade NAV
//	--this function gets called by symsetup after it has removed the symsetup installed
//		products.  we uninstall non-symsetup products here and clean up afterwards.
//	--function assumes that symsetup called preupgrade() already.
///////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall Upgrade()
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug Upgrade()", "Debug Break", NULL);
#endif

	InstallToolBox::ITB_VERSION ExistingNAVver = {0};

	// alwayas clean the shortcuts, just in case there was a dirty uninstall
	CleanShortCuts();

	return ERROR_SUCCESS;
}
