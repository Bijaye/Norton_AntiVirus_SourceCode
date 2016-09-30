/////////////////////////////////////////////////////////////////////////////////////////////
// UpgradTasks.cpp
//
// --Contains exported functions used in MSI custom actions for performing any upgrade related
// tasks
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "helper.h"
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

#include "Quarantine_const.h"

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
	g_Log.Log("CleanShortCuts() Starting.");

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
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown Exception in CleanShortCuts().");
	}
	
	//restore current working dir
	SetCurrentDirectory(szOriginalWorkDir);

	g_Log.Log("CleanShortCuts() Finished.");

	return ERROR_SUCCESS;
}

//
// helper function for RemoveExistingProducts
//
void PrepareNAVForRemoval(LPCTSTR szUpgradeGUID)
{
	//remove the silent uninstall launch condition
	InstallToolBox::RemoveLineFromCachedTable((LPTSTR)szUpgradeGUID, _T("LaunchCondition"),
		_T("LaunchCondition.Condition='(NOT Installed) OR (UILevel > 2)'"));
	
	//remove the setreboot custom action
	InstallToolBox::RemoveLineFromCachedTable((LPTSTR)szUpgradeGUID, _T("InstallExecuteSequence"),
		_T("InstallExecuteSequence.Action='SetReboot'"));

	//remove the setreboot custom action that comes from symnetdrv
	InstallToolBox::RemoveLineFromCachedTable((LPTSTR)szUpgradeGUID, _T("InstallExecuteSequence"),
		_T("InstallExecuteSequence.Action='SetForcedReboot.6500F9C2_37EA_4F25_A4DE_6211026D9C01'"));
	
	//remove the SetForceReboot custom action that was in Torito
	InstallToolBox::RemoveLineFromCachedTable((LPTSTR)szUpgradeGUID, _T("InstallExecuteSequence"),
		_T("InstallExecuteSequence.Action='SetForceReboot'"));

	//remove the SetRebootPrompt custom action that was in Torito
	InstallToolBox::RemoveLineFromCachedTable((LPTSTR)szUpgradeGUID, _T("InstallExecuteSequence"),
		_T("InstallExecuteSequence.Action='SetRebootPrompt'"));
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Uninstall any previous version of NAV
// 
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL RemoveExistingProducts()
{
	g_Log.Log("VerifyUpgrade() Starting.");

	const TCHAR cszNAVUpgradeCode[] =		_T("{68C33091-72FA-4DB8-A40E-8CEAE4552D73}");
	const TCHAR cszNAVPROUpgradeCode[] =	_T("{CD05B6DD-65EC-43AC-BE00-73394F19B3AA}");
	const TCHAR cszNSWUpgradeCode[]	=		_T("{C7618135-CA07-482B-BB36-F64A848E8A56}");

	const TCHAR cszNISUpgradeCode[]	=		_T("{E363D102-3D7F-11D3-AF49-00600811C705}");
	TCHAR szGuid[MAX_PATH] = {0};
	TCHAR szPath[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	
	PrepareNAVForRemoval(cszNAVUpgradeCode);
	PrepareNAVForRemoval(cszNAVPROUpgradeCode);

	if(MsiEnumRelatedProducts(cszNISUpgradeCode, 0, 0, szGuid) == ERROR_SUCCESS)
	{
		if(MsiQueryFeatureState(szGuid, _T("Norton_AntiVirus")) == INSTALLSTATE_LOCAL)
		{
			// remove the custom action that prevents NIS from being silently uninstalled
			InstallToolBox::RemoveLineFromCachedTable((LPTSTR)cszNISUpgradeCode, _T("InstallExecuteSequence"),
				_T("InstallExecuteSequence.Action='KillUninstall'"));

			// remove the NIS launch condition that thinks its trying to install an old verion
			InstallToolBox::RemoveLineFromCachedTable((LPTSTR)cszNISUpgradeCode, _T("LaunchCondition"),
				_T("LaunchCondition.Condition='(NISPrevVersion<> \"1\") AND (NISSameVersion <> \"1\")'"));
			
			// remove the NIS launch condition that thinks its trying to install an old verion
			InstallToolBox::RemoveLineFromCachedTable((LPTSTR)cszNISUpgradeCode, _T("LaunchCondition"),
				_T("LaunchCondition.Condition='NOT LURunning'"));

			// set the reboot property
			TCHAR szPackage[MAX_PATH*2];
			DWORD dwCount = sizeof(szPackage);
			if(ERROR_SUCCESS == MsiGetProductInfo(szGuid, INSTALLPROPERTY_LOCALPACKAGE, szPackage, &dwCount))
			{				
				MSIHANDLE hInstall = NULL;
				if(ERROR_SUCCESS == MsiOpenDatabase(szPackage, MSIDBOPEN_DIRECT, &hInstall))
				{			
					MSIHANDLE hView;
					if(ERROR_SUCCESS == MsiDatabaseOpenView(hInstall, 
						_T("INSERT INTO `Property` (`Property`.`Property`, `Property`.Value`) VALUES ('REBOOT', 'ReallySupress')"), 
						&hView))
					{
						MsiViewExecute(hView, 0);
                        MsiDatabaseCommit(hInstall);
						MsiCloseHandle(hView);
					}

					MsiCloseHandle(hInstall);
				}
			}
			
            // uninstall the product
			MsiConfigureFeature(szGuid, _T("Norton_AntiVirus"), INSTALLSTATE_ABSENT);

			// remove the property we just added or else a reboot won't happen when the user uninstalls NIS
			InstallToolBox::RemoveLineFromCachedTable((LPTSTR)cszNISUpgradeCode, _T("Property"),
				_T("Property.Property='REBOOT'"));
		}
	}
	
	if(MsiEnumRelatedProducts(cszNSWUpgradeCode, 0, 0, szGuid) == ERROR_SUCCESS)
	{
		if( MsiQueryFeatureState(szGuid, _T("Norton_AntiVirus")) == INSTALLSTATE_LOCAL)
		{	
			// set the reboot property
			TCHAR szPackage[MAX_PATH*2];
			DWORD dwCount = sizeof(szPackage);
			if(ERROR_SUCCESS == MsiGetProductInfo(szGuid, INSTALLPROPERTY_LOCALPACKAGE, szPackage, &dwCount))
			{				
				MSIHANDLE hInstall = NULL;
				if(ERROR_SUCCESS == MsiOpenDatabase(szPackage, MSIDBOPEN_DIRECT, &hInstall))
				{			
					MSIHANDLE hView;
					if(ERROR_SUCCESS == MsiDatabaseOpenView(hInstall, 
						_T("INSERT INTO `Property` (`Property`.`Property`, `Property`.`Value`) VALUES ('REBOOT', 'ReallySupress')"), 
						&hView))
					{
						MsiViewExecute(hView, 0);
                        MsiDatabaseCommit(hInstall);
						MsiCloseHandle(hView);
					}

					MsiCloseHandle(hInstall);
				}
			}

			// uninstall the product
			MsiConfigureFeature(szGuid, _T("Norton_AntiVirus"), INSTALLSTATE_ABSENT);

			// remove the property we just added or else a reboot won't happen when the user uninstalls NSW
			InstallToolBox::RemoveLineFromCachedTable((LPTSTR)cszNISUpgradeCode, _T("Property"),
				_T("Property.Property='REBOOT'"));
		}
	}
	
	if (MsiEnumRelatedProducts(cszNAVPROUpgradeCode, 0, 0, szGuid) == ERROR_SUCCESS)
	{	
		// uninstall the product
		MsiConfigureProductEx(szGuid, 0, INSTALLSTATE_ABSENT, _T("REBOOT = ReallySuppress"));
	}
	
	if(MsiEnumRelatedProducts(cszNAVUpgradeCode, 0, 0, szGuid) == ERROR_SUCCESS)
	{
		// uninstall the product
		MsiConfigureProductEx(szGuid, 0, INSTALLSTATE_ABSENT, _T("REBOOT = ReallySuppress"));
	}

	g_Log.Log("VerifyUpgrade() Finished.");

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
		g_Log.Log("BumpRefCounts() Starting.");

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
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown Exception in BumpRefCounts().");
	}

	g_Log.Log("BumpRefCounts() Finished.");

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
		g_Log.Log("DecRefCounts() Starting.");

		// since symsetup calls our "upgrade" custom action we need to make sure that NAV we only
		// decrement symevent when we have actualy upgraded somthing, look for the OLD NAV key that we set
		ATL::CRegKey InstallAppsKey;
		DWORD dwSize = MAX_PATH;
		TCHAR szOLDNAV[MAX_PATH] = {0};
		UINT ur = InstallAppsKey.Open(HKEY_LOCAL_MACHINE, szSymAppsPath, KEY_READ);
		if(ERROR_SUCCESS == ur)
		{
			ur = InstallAppsKey.QueryStringValue(_T("OLDNAV"), szOLDNAV, &dwSize);
			if(ERROR_SUCCESS != ur)
			{	
				return ERROR_SUCCESS;	//dont decrement b/c we didn't increment
			}
		}
		else
		{
			g_Log.Log("DecRefCounts(): couldn't open %s (0x%08x)", szSymAppsPath, ur);
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
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, &hTmpKey) == ERROR_SUCCESS)
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
					if(0 == _tcsicmp(szOldDir, szNewDir))
					{
						UnMarkFilesForDeletion("S32NAVO.dll");
						UnMarkFilesForDeletion("APWUTIL.DLL");
						UnMarkFilesForDeletion("APWCMD9X.DLL");
						UnMarkFilesForDeletion("APWCMDNT.DLL");
						UnMarkFilesForDeletion("DEFALERT.DLL");
						UnMarkFilesForDeletion("NAVTASKS.DLL");
						UnMarkFilesForDeletion("NAVERROR.DLL");
						UnMarkFilesForDeletion("NAVAPW32.DLL");
						UnMarkFilesForDeletion("NAVPROD.DLL");
						UnMarkFilesForDeletion("DJSMAR00.DLL");
						UnMarkFilesForDeletion("BRANDING.INI");
						UnMarkFilesForDeletion("NAVOPTS.BAK");
						UnMarkFilesForDeletion("NAVOPTS.DLL");
						UnMarkFilesForDeletion("N32EXCLU.DLL");
						UnMarkFilesForDeletion("NAVEMAIL.DLL");
						UnMarkFilesForDeletion("SAVRT32.DLL");
						UnMarkFilesForDeletion("NAVEVENT.DLL");
						UnMarkFilesForDeletion("N32CALL.DLL");
			            UnMarkFilesForDeletion("NAVLUCBK.DLL");
			            UnMarkFilesForDeletion("PATCH32I.DLL");
						UnMarkFilesForDeletion("NAVAPSCR.DLL");
						UnMarkFilesForDeletion("NAVLNCH.DLL");
						UnMarkFilesForDeletion("NAVSTATS.DLL");
						UnMarkFilesForDeletion("NAVUI.DLL");
						UnMarkFilesForDeletion("AVRES.DLL");
						UnMarkFilesForDeletion("NAVLCOM.DLL");
						UnMarkFilesForDeletion("NAVOPTRF.DLL");
						UnMarkFilesForDeletion("STATUSHP.DLL");
						UnMarkFilesForDeletion("CCAVMAIL.DLL");
                        UnMarkFilesForDeletion("NAVShExt.dll");
                        UnMarkFilesForDeletion("NAVAP32.DLL");
                        UnMarkFilesForDeletion("Portal");
                        UnMarkFilesForDeletion("Incoming");

						// virus def files
						UnMarkFilesForDeletion("ECMSVR32.DLL");
						UnMarkFilesForDeletion("NAVENG32.DLL");
						UnMarkFilesForDeletion("NAVEX32A.DLL");
						
                        InstallToolBox::CInstoptsDat Instopts;
                        DWORD dwIWP = 1;
                        Instopts.ReadDWORD(_T("INSTALL:IWP"), dwIWP, 1);
                        if(1 == dwIWP)
                        {
                            UnMarkFilesForDeletion("CCFWSETG.DLL");
                            UnMarkFilesForDeletion("SYMFWAGT.DLL");
                            UnMarkFilesForDeletion("NPFMNTOR.EXE");
						    UnMarkFilesForDeletion("IWP.DLL");
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
		UnMarkFilesForDeletion("NAVShExt.dll");
		UnMarkFilesForDeletion("CCERRDSP.DLL");
		UnMarkFilesForDeletion("CCAPP.EXE");
		UnMarkFilesForDeletion("CCEMLPXY.DLL");
		UnMarkFilesForDeletion("CCEMLPXY.DAT");
		//UnMarkFilesForDeletion("CCTRUST.DLL");
		UnMarkFilesForDeletion("CCVRTRST.DLL");
		UnMarkFilesForDeletion("CCIMSCAN.DLL");
		UnMarkFilesForDeletion("CCEVTMGR.EXE");				
		UnMarkFilesForDeletion("CCWEBWND.DLL");
        UnMarkFilesForDeletion("CCPROSUB.DLL");
        UnMarkFilesForDeletion("CCSETEVT.DLL");
		UnMarkFilesForDeletion("CCL40.DLL");
		//UnMarkFilesForDeletion("CCEVT.DLL");
		//UnMarkFilesForDeletion("CCREGMON.DLL");
		UnMarkFilesForDeletion("LIVETRI.ZIP");
		UnMarkFilesForDeletion("NMAIN.EXE");
		UnMarkFilesForDeletion("DJSLCOM.DLL");
		UnMarkFilesForDeletion("DJSALERT.DLL");
		UnMarkFilesForDeletion("SymLTCOM.dll");
	}
	catch(exception &ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown Exception in DecRefCounts().");
	}

	g_Log.Log("DecRefCounts() Finished.");
		
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


BOOL MoveQuarantineDirectory(ATL::CString sOldQuarPath, ATL::CString sNewQuarPath)
{
	CCTRACEI("MoveQuarantineDirectory(%s, %s) Starting.", sOldQuarPath, sNewQuarPath);

    if(!FileExists(sOldQuarPath) || sNewQuarPath.CompareNoCase(sOldQuarPath) == 0)
        return FALSE;

	PathAppend(sOldQuarPath.GetBuffer(MAX_PATH), _T("*.*"));
	sOldQuarPath.ReleaseBuffer();

	// SHFILEOPSTRUCT Structure documentation for pFrom and pTo:
	// Each file name must be terminated by a single NULL character. 
	// An additional NULL character must be appended to the end of the final name 
	DWORD dwBufSize = sOldQuarPath.GetLength() + 2;
	TCHAR* szOldQuarPath = new TCHAR[dwBufSize];
    ZeroMemory(szOldQuarPath, sizeof(szOldQuarPath));
	_tcscpy(szOldQuarPath, sOldQuarPath);
	szOldQuarPath[dwBufSize - 1] = _T('\0');
	szOldQuarPath[dwBufSize - 2] = _T('\0');
		
	dwBufSize = sNewQuarPath.GetLength() + 2;
	TCHAR* szNewQuarPath = new TCHAR[dwBufSize];
    ZeroMemory(szNewQuarPath, sizeof(szNewQuarPath));
	_tcscpy(szNewQuarPath, sNewQuarPath);
	szNewQuarPath[dwBufSize - 1] = _T('\0');
	szNewQuarPath[dwBufSize - 2] = _T('\0');

	SHFILEOPSTRUCT shFileCopyStruct;
	ZeroMemory(&shFileCopyStruct, sizeof(SHFILEOPSTRUCT));
	shFileCopyStruct.hwnd = NULL;
	shFileCopyStruct.wFunc = FO_MOVE;
	shFileCopyStruct.pFrom = szOldQuarPath;
	shFileCopyStruct.pTo = szNewQuarPath;
	shFileCopyStruct.fFlags = FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
	
	if(0 == SHFileOperation(&shFileCopyStruct))
	{
		CCTRACEI("%s - copied Quarantine files successfully.", __FUNCTION__);
		return TRUE;
	}
	else
	{
		CCTRACEE("%s - failed to copy directory contents.", __FUNCTION__);
		return FALSE;
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  MoveQuarantineFiles() -- 
//		In an upgrade installation over a product installed to a different 
//		directory or when Quarantine files were left behind by a previous 
//		uninstall, we need to move the Quarantine files over to the new 
//		product directory
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL MoveQuarantineFiles()
{
	CCTRACEI("%s - Starting.", __FUNCTION__);

#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug MoveQuarantineFiles()", "Debug Break", NULL);
#endif

	try
	{
		// build the path to the new Quarantine directory
		ATL::CString sAppDataPath;
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, sAppDataPath.GetBuffer(MAX_PATH));
		sAppDataPath.ReleaseBuffer();

		ATL::CString sNewNAVDataPath(sAppDataPath);
		PathAppend(sNewNAVDataPath.GetBuffer(MAX_PATH), SZ_QUARANTINE_DATA_DIRECTORY_NAME);
		sNewNAVDataPath.ReleaseBuffer();
		if(!FileExists(sNewNAVDataPath))
		{
			int iRet = SHCreateDirectoryEx(NULL, sNewNAVDataPath, NULL);
			if(ERROR_SUCCESS == iRet)
			{
				CCTRACEI("%s - Created directory: %s", __FUNCTION__, sNewNAVDataPath);
			}
			else
			{
				CCTRACEE("%s - Did not create directory: %s, iRet=%d", __FUNCTION__, sNewNAVDataPath, iRet);				
			}
		}

		ATL::CString sNewQuarPath(sAppDataPath);
		PathAppend(sNewQuarPath.GetBuffer(MAX_PATH), SZ_QUARANTINE_DATA_DIRECTORY_NAME);
		sNewQuarPath.ReleaseBuffer();

		// look up known previous install locations
		ATL::CString sOldQuarPath;
		ATL::CString sProgramFilesFolder;
		hr = SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, sProgramFilesFolder.GetBuffer(MAX_PATH));
		sProgramFilesFolder.ReleaseBuffer();   
		if(S_OK == hr)
		{
			sOldQuarPath = sProgramFilesFolder;
			PathAppend(sOldQuarPath.GetBuffer(MAX_PATH), SZ_NAV_DEFAULT_DIRECTORY_NAME);
			sOldQuarPath.ReleaseBuffer();
			if(MoveQuarantineDirectory(sOldQuarPath, sNewQuarPath))
				DeleteEmptyParentDirectories(sOldQuarPath, sProgramFilesFolder);
			sOldQuarPath.Empty();

			sOldQuarPath = sProgramFilesFolder;
			PathAppend(sOldQuarPath.GetBuffer(MAX_PATH), SZ_NIS_DEFAULT_DIRECTORY_NAME);
			sOldQuarPath.ReleaseBuffer();
			if(MoveQuarantineDirectory(sOldQuarPath, sNewQuarPath))
				DeleteEmptyParentDirectories(sOldQuarPath, sProgramFilesFolder);
			sOldQuarPath.Empty();

			sOldQuarPath = sProgramFilesFolder;
			PathAppend(sOldQuarPath.GetBuffer(MAX_PATH), SZ_NSW_DEFAULT_DIRECTORY_NAME);
			sOldQuarPath.ReleaseBuffer();
			if(MoveQuarantineDirectory(sOldQuarPath, sNewQuarPath))
				DeleteEmptyParentDirectories(sOldQuarPath, sProgramFilesFolder);
			sOldQuarPath.Empty();
		}

		// Under the InstalledApps key,
		// Look up the old NAV dir (if any)
		// Look up the saved quarantine dir (if any)
		DWORD dwSize = MAX_PATH;
		CRegKey rkInstalledApps;
		LONG lres = rkInstalledApps.Open(HKEY_LOCAL_MACHINE, szSymAppsPath);
		if(ERROR_SUCCESS == lres)
		{
			lres = rkInstalledApps.QueryStringValue(_T("OLDNAV"), sOldQuarPath.GetBuffer(MAX_PATH), &dwSize);
			sOldQuarPath.ReleaseBuffer();
			if(ERROR_SUCCESS == lres)
			{
				PathAppend(sOldQuarPath.GetBuffer(MAX_PATH), SZ_QUARANTINE_DIRECTORY_NAME);
				sOldQuarPath.ReleaseBuffer();
				if(MoveQuarantineDirectory(sOldQuarPath, sNewQuarPath))
					DeleteEmptyParentDirectories(sOldQuarPath, sProgramFilesFolder);
			}
			sOldQuarPath.Empty();
        }
        
		//look in the Quarantine key, PreInstallScanner uses it and previous uninstalls may
        //leave it behind
		dwSize = MAX_PATH;
		CRegKey rkNAV;
        lres = rkNAV.Create(HKEY_LOCAL_MACHINE, SZ_REGKEY_QUARANTINE);
		if(ERROR_SUCCESS == lres)
		{
			lres = rkNAV.QueryStringValue(SZ_REGVALUE_QUARANTINE_PATH, sOldQuarPath.GetBuffer(MAX_PATH), &dwSize);
			sOldQuarPath.ReleaseBuffer();
			if(ERROR_SUCCESS == lres)
			{
				if(MoveQuarantineDirectory(sOldQuarPath, sNewQuarPath))
					DeleteEmptyParentDirectories(sOldQuarPath, sProgramFilesFolder);
			}
            
            // now that Quarantined files have been moved from any possible location
            // set the keys to point to the correct place
            ATL::CString sTempPath = sNewQuarPath;
            ATL::CString sIncomingPath = sNewQuarPath;
            sTempPath.Append(_T("\\Portal"));
            sIncomingPath.Append(_T("\\Incoming"));
            
            CRegKey rkSymantec;
            lres = rkSymantec.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"));
            if(ERROR_SUCCESS == lres)
            {
                rkSymantec.SetStringValue(SZ_REGVALUE_QUARANTINE_PATH_BACKUP, sNewQuarPath);
                rkSymantec.SetStringValue(SZ_REGVALUE_QUARANTINE_TEMP_PATH_BACKUP, sTempPath);
                rkSymantec.SetStringValue(SZ_REGVALUE_QUARANTINE_INCOMING_PATH_BACKUP, sIncomingPath);
            }
		}
	}
	catch(exception &ex)
	{
		CCTRACEE(ex.what());
	}
	catch(...)
	{
		CCTRACEE("%s - Unknown Exception.", __FUNCTION__);
	}

	CCTRACEI("%s - Finished.", __FUNCTION__);
		
	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  SetQuarantinePaths() -- 
//      Previous products might uninstall quarantined files and delete the keys.  Set them
//      now.
/////////////////////////////////////////////////////////////////////////////////////////////
void SetQuarantinePaths()
{
    ATL::CString sQuarPath;
    ATL::CString sTempPath;
    ATL::CString sIncomingPath;

    ATL::CRegKey rkNAV;
    LONG lres = rkNAV.Create(HKEY_LOCAL_MACHINE, SZ_REGKEY_QUARANTINE);
	if(ERROR_SUCCESS == lres)
    {   
        CRegKey rkSymantec;
        lres = rkSymantec.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"));
        if(ERROR_SUCCESS == lres)
        {
            ULONG uSize = MAX_PATH;
            rkSymantec.QueryStringValue(SZ_REGVALUE_QUARANTINE_PATH_BACKUP, sQuarPath.GetBuffer(MAX_PATH), &uSize);
            sQuarPath.ReleaseBuffer();
            rkSymantec.DeleteValue(SZ_REGVALUE_QUARANTINE_PATH_BACKUP);
            
            uSize = MAX_PATH;
            rkSymantec.QueryStringValue(SZ_REGVALUE_QUARANTINE_TEMP_PATH_BACKUP, sTempPath.GetBuffer(MAX_PATH), &uSize);
            sTempPath.ReleaseBuffer();
            rkSymantec.DeleteValue(SZ_REGVALUE_QUARANTINE_TEMP_PATH_BACKUP);
            
            uSize = MAX_PATH;
            rkSymantec.QueryStringValue(SZ_REGVALUE_QUARANTINE_INCOMING_PATH_BACKUP, sIncomingPath.GetBuffer(MAX_PATH), &uSize);
            sIncomingPath.ReleaseBuffer();
            rkSymantec.DeleteValue(SZ_REGVALUE_QUARANTINE_INCOMING_PATH_BACKUP);
        }
        
        rkNAV.SetStringValue(SZ_REGVALUE_QUARANTINE_PATH, sQuarPath);
        rkNAV.SetStringValue(SZ_REGVALUE_QUARANTINE_TEMP_PATH, sTempPath);
        rkNAV.SetStringValue(SZ_REGVALUE_QUARANTINE_INCOMING_PATH, sIncomingPath);
    }
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
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown Exception in StopOldAPService().");
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
	
	g_Log.Log("StopOldAPService() Finished.");
		
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

    // if the location of NAV is going to change we need to move the quaratined files over to new place
	MoveQuarantineFiles();

	InstallToolBox::ITB_VERSION ExistingNAVver = {0};
	CNAVInfo NAVInfo;

	if(InstallToolBox::IsNAVInstalled())
	{
		// store the OLDNAV location because it will be needed to determine whether to
		// move the quarantine files and which files to unmarkfor deletion
		ATL::CRegKey RegKey;
		UINT ur = RegKey.Open(HKEY_LOCAL_MACHINE, szSymAppsPath);
		if(ERROR_SUCCESS == ur)
		{
			ur = RegKey.SetStringValue(_T("OLDNAV"), NAVInfo.GetNAVDir());
			if(ur != ERROR_SUCCESS)
			{
				CCTRACEE(_T("PreUpgrade(): Set InstalledApps\\OLDNAV failed (0x%08x)"), ur);
			}
		}
		else
		{
			CCTRACEE(_T("PreUpgrade(): Open InstalledApps key failed (0x%08x)"), ur);
		}
		
		ATL::CRegKey NAVKey;
		DWORD dwValue = 0;
		ur = NAVKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"), KEY_READ);
		if(ERROR_SUCCESS == ur)
		{
			ur = NAVKey.QueryDWORDValue(_T("DontMigrate"), dwValue);
			// check if we're supposed to migrate options -- migrate by default
			if(ERROR_SUCCESS != ur)
			{
				ATL::CString sPath;
				ATL::CString sOmigrate;

				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				memset(&pi, 0, sizeof(pi));
				memset(&si, 0, sizeof(si));
				si.cb = sizeof(si);

				GetModuleFileName(GetModuleHandle(NULL), sPath.GetBuffer(MAX_PATH), MAX_PATH);
				sPath.ReleaseBuffer();

				sPath = sPath.Left(sPath.ReverseFind(_T('\\')) + 1);
				sOmigrate.Format(_T("%s\\NAV\\OMigrate.exe"), sPath);

				CreateProcess(sOmigrate, NULL, NULL, NULL, FALSE, 0, NULL, sPath, &si, &pi);
			}
			else
			{
				CCTRACEI(_T("PreUpgrade(): DontMigrate key exists"));
			}
		}
		else
		{
			CCTRACEE(_T("PreUpgrade(): Open NAV key failed (0x%08x)"), ur);
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

	g_Log.Log("DropIamApp() Starting.");

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
		g_Log.LogEx(ex.what());
        bRet = FALSE;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in DropIamApp().");
        bRet = FALSE;
	}

	g_Log.Log("DropIamApp() Finished.");

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

	if(InstallToolBox::IsNAVInstalled())
	{
		// remove the previously installed non-symsetup products
        // we can remove ths in 2006 when we on support install over SymSetup
        // products
		RemoveExistingProducts();
	}
    
    SetQuarantinePaths();

    // defect 1-2YJE8L: Some genius told Gateway they could install Scriptblocking
    // after installing NAV w/out Scriptblocking.  This decouples the two
    // so when we remove previos NAV SB is still around.  Now that we have removed
    // NAV, if Scriptblocking is still installed we need to remove it.  This
    // nasty hack can removed in 2007 when we no longer supprt install over NAV2004
    const TCHAR cszSBGuid[] = _T("{D327AFC9-7BAA-473A-8319-6EB7A0D40138}");
    if(MsiQueryFeatureState(cszSBGuid, _T("Complete")) == INSTALLSTATE_LOCAL)
        MsiConfigureFeature(cszSBGuid, _T("Complete"), INSTALLSTATE_ABSENT);

	// request a reboot if we've upgraded, we have to do this here b/c the uninstall nukes it if we do in PreUpgrade()
	ATL::CRegKey InstallAppsKey;
	DWORD dwSize = MAX_PATH;
	TCHAR szOLDNAV[MAX_PATH] = {0};
	InstallAppsKey.Create(HKEY_LOCAL_MACHINE, szSymAppsPath);
	if(ERROR_SUCCESS == InstallAppsKey.QueryStringValue(_T("OLDNAV"), szOLDNAV, &dwSize))
	{
		ATL::CRegKey SymSetupKey;
		SymSetupKey.Create(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\SymSetup"));
		SymSetupKey.SetDWORDValue(_T("RebootNeeded"), 1);
	}

	// alwayas clean the shortcuts, just in case there was a dirty uninstall
	CleanShortCuts();

	return ERROR_SUCCESS;
}
