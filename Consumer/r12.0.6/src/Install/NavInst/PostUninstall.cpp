/////////////////////////////////////////////////////////////////////////////////////////////
// PostUninstall.cpp
//
// --Contains exported functions used in MSI custom actions for performing any post-uninstall
// tasks after MSI has removed the files
//
/////////////////////////////////////////////////////////////////////////////////////////////


#include "helper.h"
#include "NAVTasks.h"
//#include "SymReg.h"
#include "InstOptsNames.h"

// For RemoveScanFiles function.
#include "NAVTasks.h"							// INAVStatus
#include "NAVTasks_i.c"						// INAVStatus

#include "registry.h"
#include "fileaction.h"

#include "Quarantine_const.h"

using namespace InstallToolBox;

/////////////////////////////////////////////////////////////////////////////////////////////
//  RemoveRemediationData(): Remove remediation data
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall RemoveRemediationData(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RemoveRemediationData()", "Debug Break", NULL);
#endif

    try
    {
        // delete the POSTRBT runonce key entry
        ATL::CRegKey rkRunKey;
        rkRunKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
        rkRunKey.DeleteValue(_T("POSTRBT"));
        rkRunKey.Close();

        // delete the remediateiondata folder
        CNAVInfo NAVInfo;
        ATL::CString sRemediationData = NAVInfo.GetNAVCommonDir();
        sRemediationData += _T("\\RemediationData");
        SHFILEOPSTRUCT fileopstruct;
        ZeroMemory(&fileopstruct, sizeof(SHFILEOPSTRUCT));
        fileopstruct.wFunc = FO_DELETE;
        fileopstruct.pFrom = sRemediationData;
        fileopstruct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
        SHFileOperation(&fileopstruct);
    }
    catch(...)
    {
        CCTRACEE("Uknown Exception caught in RemoveRemediationData()");
    }
    
    return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  RemoveNAVRegistry(): Delete anything left over in the registry
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall RemoveNAVRegistry(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RemoveNAVRegistry()", "Debug Break", NULL);
#endif

	g_Log.Log("RemoveNAVRegistry() Starting.");

	BOOL bResult;

	try
	{
        // before we nuke the NAV key save the quarantine keys, we need to
        // put them and only them back.
        DWORD dwSize = MAX_PATH;
        ATL::CString sQuarPath;
        ATL::CRegKey rkNAV;
        rkNAV.Open(HKEY_LOCAL_MACHINE, SZ_REGKEY_QUARANTINE);
        LONG lRes = rkNAV.QueryStringValue(SZ_REGVALUE_QUARANTINE_PATH, sQuarPath.GetBuffer(MAX_PATH), &dwSize);
        sQuarPath.ReleaseBuffer();

        // Remove the Norton AntiVirus hive from HKLM.
		if (RegRecursiveDeleteKey(HKEY_LOCAL_MACHINE, szNAVAppsKey) != ERROR_SUCCESS)
		{
			g_Log.Log ( "RemoveNAVRegistry: Error in deleting NAV regkeys in HKLM");
		}
        
        if(ERROR_SUCCESS == lRes)
        {
            ATL::CString sTempPath = sQuarPath;
            ATL::CString sIncomingPath = sQuarPath;
            sTempPath.Append(_T("\\Portal"));
            sIncomingPath.Append(_T("\\Incoming"));

            rkNAV.Create(HKEY_LOCAL_MACHINE, SZ_REGKEY_QUARANTINE);
            rkNAV.SetStringValue(SZ_REGVALUE_QUARANTINE_PATH, sQuarPath);
            rkNAV.SetStringValue(SZ_REGVALUE_QUARANTINE_TEMP_PATH, sTempPath);
            rkNAV.SetStringValue(SZ_REGVALUE_QUARANTINE_INCOMING_PATH, sIncomingPath);
        }
		
		// Remove the Norton AntiVirus hive from HKCU.
		if (RegRecursiveDeleteKey(HKEY_CURRENT_USER, szNAVAppsKey) != ERROR_SUCCESS)
		{
			g_Log.Log ( "RemoveNAVRegistry: Error in deleting NAV regkeys in HKCU");
		}
	}

	catch(exception& ex)
	{
		bResult = false;		
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		bResult = false;
		g_Log.LogEx("Unknown exception in RemoveNAVRegistryHelp().");
	}
	
	g_Log.Log("RemoveNAVRegistry() Finished.");

	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Make sure no files are left in the NAV folders
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall RemoveNAVFiles(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RemoveNAVFiles()", "Debug Break", NULL);
#endif

	g_Log.Log("RemoveNAVFiles() Starting.");

	// Remove the NCDTree folder which is used for inoculation.
	TCHAR szNCDTree[MAX_PATH]={0};
	
	// Get the path to the NCDTREE folder.
	GetWindowsDirectory(szNCDTree, MAX_PATH);
	TCHAR* pStr = _tcschr(szNCDTree, _T('\\'));
	if(pStr != NULL)
		*pStr = _T('\0');
	_tcscat(szNCDTree, _T("\\NCDTREE"));
	
	// Delete NCDTree folder and all its files.
	DeleteFilesInDirectory(szNCDTree);
	if(RemoveDirectory(szNCDTree) == 0)
		FileInUseHandler(szNCDTree);
	
	// Delete the MYAUTH.DAT FILE
	TCHAR szMyAuthDat[MAX_PATH]={0};
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES_COMMON, NULL, 0, szMyAuthDat)))
	{			
		_tcscat(szMyAuthDat, _T("\\Symantec Shared\\VirusDefs\\MyAuth.dat"));
		DeleteFile(szMyAuthDat);
	}
			
	// Get the path to the temp folder and remove Symantec files.
	TCHAR szTempFolder[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, szTempFolder);
	RemoveTargetFiles(szTempFolder, g_szNavFiles);
	

	// Get the NAV install path.
	CNAVInfo NAVInfo;
	TCHAR szNAVDir[MAX_PATH] = {0};	
	DWORD dwSize = MAX_PATH;
	_tcscpy(szNAVDir, NAVInfo.GetNAVDir());
	
	RemoveTargetFiles(szNAVDir, g_szNavFiles);
	
	// Remove the NAV folder.
	if(RemoveDirectory(szNAVDir) == 0)
		FileInUseHandler(szNAVDir);

	g_Log.Log("RemoveNAVFiles() Finished.");


	return ERROR_SUCCESS;

}

/////////////////////////////////////////////////////////////////////////////////////////////
//  Removes any Scan Task Files from the system on uninstall
// 
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall RemoveScanFiles(MSIHANDLE hInstall)
{	
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RemoveScanFiles()", "Debug Break", NULL);
#endif

	g_Log.Log("RemoveScanFiles() Starting.");

	HRESULT hResult = S_OK;
	hResult = CoInitialize(NULL);

	INAVScanTasks* pTasks = NULL;
	HRESULT hr = CoCreateInstance( CLSID_NAVScanTasks, NULL, CLSCTX_ALL, IID_INAVScanTasks, (void**) &pTasks );
	if( SUCCEEDED( hr ) )
	{
		ISymScriptSafe* pss = NULL;
		hr = pTasks->QueryInterface( IID_ISymScriptSafe, (void**) &pss );
		if( SUCCEEDED( hr ) )
		{
			hr = pss->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
			if( SUCCEEDED( hr ) )
			{
				pTasks->DeleteAll();
			}
			else
			{
				printf( "Couldn't set security" );
			}

			pss->Release();
		}

		pTasks->Release();
	}
	else
	{
		printf( "Couldn't create status object" );
	}

	if (SUCCEEDED(hResult))
	{
		CoUninitialize();
	}

	g_Log.Log("RemoveScanFiles() Finished.");

	return ERROR_SUCCESS;
}

ATL::CString GetSpecialFolderPath(UINT iFolder)
{
    BOOL bRet = FALSE;
    ATL::CString cszUserApplicationPath;

    // Get the current user's app data folder
    bRet = SHGetSpecialFolderPath(GetDesktopWindow(), cszUserApplicationPath.GetBuffer(MAX_PATH), iFolder, TRUE);
    cszUserApplicationPath.ReleaseBuffer();
    if(!bRet)
        return _T("");

    return cszUserApplicationPath;
}

ATL::CString GetUserAppDataPathFmt()
{
    BOOL bRet;
    ATL::CString cszUserApplicationPath = GetSpecialFolderPath(CSIDL_APPDATA);
    ATL::CString cszUserName;

    if(cszUserApplicationPath.IsEmpty())
        return _T("");

    // Get the current user name
    DWORD dwBufSize = MAX_PATH;
    bRet = GetUserName(cszUserName.GetBuffer(dwBufSize), &dwBufSize);
    cszUserName.ReleaseBuffer();
    if(!bRet)
        return _T("");

    // Replace the username with the string format char
    INT iRet = cszUserApplicationPath.Replace(cszUserName, _T("%s"));
    if(!iRet)
        return _T("");


    return cszUserApplicationPath;
}

ATL::CString GetProfilesPath()
{
    BOOL bRet = FALSE;
    ATL::CString cszUserApplicationPath = GetSpecialFolderPath(CSIDL_APPDATA);
    ATL::CString cszUserName;
    DWORD dwBufSize = MAX_PATH;

    // Get the current user name
    bRet = GetUserName(cszUserName.GetBuffer(dwBufSize), &dwBufSize);
    cszUserName.ReleaseBuffer();
    if(!bRet)
        return _T("");

    // Replace the username with the string format char
    INT iRet = cszUserApplicationPath.Find(cszUserName, 0);

	// -1 is the return value for a failed Find().  If we give this to Truncate it will chunk.
    if(-1 == iRet)
        return _T("");

    cszUserApplicationPath.Truncate(iRet);

    return cszUserApplicationPath;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  Removes any Scan Task Files from the system on uninstall
// 
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall RemoveHPPFiles(MSIHANDLE hInstall)
{	
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RemoveHPPFiles()", "Debug Break", NULL);
#endif

	g_Log.Log("RemoveHPPFiles() Starting.");

	try
	{
		//
		// Need to remove HPP files
		//  CSIDL_APPDATA\Symantec\PendingAlertsQueue.log
		//  CSIDL_COMMON_APPDATA\Symantec\HPPAppActivity.log
		//  CSIDL_COMMON_APPDATA\Symantec\HPPHomePageActivity.log
		//

		ATL::CString cszFileName;
		ATL::CString cszAppData = GetSpecialFolderPath(CSIDL_COMMON_APPDATA);
		::PathAppend(cszAppData.GetBuffer(MAX_PATH), _T("Symantec"));
		cszAppData.ReleaseBuffer();

		//
		// Remove the HPPHomePageActivity log
		::PathCombine(cszFileName.GetBuffer(MAX_PATH), cszAppData, _T("HPPHomePageActivity.log"));
		cszFileName.ReleaseBuffer();
		if(-1 != GetFileAttributes(cszFileName))
		{
			g_Log.Log("RemoveHPPFiles() - removing %s.", cszFileName);

			if(!DeleteFile(cszFileName))
				FileInUseHandler(cszFileName);
		}
		else
		{
			g_Log.Log("RemoveHPPFiles() - HPPHomePageActivity was not found. ref: %s.", cszFileName);
		}

		//
		// Remove the HPPAppActivity log
		::PathCombine(cszFileName.GetBuffer(MAX_PATH), cszAppData, _T("HPPAppActivity.log"));
		cszFileName.ReleaseBuffer();
		if(-1 != GetFileAttributes(cszFileName))
		{
			g_Log.Log("RemoveHPPFiles() - removing %s.", cszFileName);

			if(!DeleteFile(cszFileName))
				FileInUseHandler(cszFileName);
		}
		else
		{
			g_Log.Log("RemoveHPPFiles() - HPPAppActivityLog was not found. ref: %s.", cszFileName);
		}

		// Remove the Symantec dir if empty
		g_Log.Log("RemoveHPPFiles() - removing %s.", cszAppData);
		if(RemoveDirectory(cszAppData) == 0)
			FileInUseHandler(cszAppData);

		// Get the format for the users' app data path
		ATL::CString cszAppDataFmt = GetUserAppDataPathFmt();
		::PathAppend(cszAppDataFmt.GetBuffer(MAX_PATH), _T("Symantec"));
		cszAppDataFmt.ReleaseBuffer();

		// Get the user profiles path
		BOOL bRet = FALSE;
		ATL::CString cszUserProfiles = GetProfilesPath();

		// If this doesn't exist, somethings wrong, bail
		if(-1 == GetFileAttributes(cszUserProfiles))
		{
			// Always return ERROR_SUCCESS
			return ERROR_SUCCESS;
		}
		
		// Init the find data
		WIN32_FIND_DATA wfd;
		ZeroMemory(&wfd, sizeof(WIN32_FIND_DATA));

		// Create the filespec for enumeration
		ATL::CString cszFileSpec;
		::PathCombine(cszFileSpec.GetBuffer(MAX_PATH), cszUserProfiles, _T("*.*"));
		cszFileSpec.ReleaseBuffer();

		// Begin enumerating the user profiles dir
		HANDLE hFind = FindFirstFile(cszFileSpec, &wfd);
		if(INVALID_HANDLE_VALUE == hFind)
		{
			// Always return ERROR_SUCCESS
			return ERROR_SUCCESS;
		}

		// Need to remove the pending alerts queue logs
		// from each individual users app data dir
		BOOL bFindRet = TRUE;
		while(bFindRet)
		{
			// make sure we have a dir that is not a "." or ".."
			if( (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes) && 0 != _tcscmp(_T("."), wfd.cFileName) && 0 != _tcscmp(_T(".."), wfd.cFileName))
			{
				// Build up the path
				ATL::CString cszPendingAlertQueueFileName, cszPendingAlertQueueDirName;
				cszPendingAlertQueueDirName.Format(cszAppDataFmt, wfd.cFileName);
				::PathCombine(cszPendingAlertQueueFileName.GetBuffer(MAX_PATH), cszPendingAlertQueueDirName, _T("PendingAlertsQueue.log"));
				cszPendingAlertQueueFileName.ReleaseBuffer();
				if(-1 != GetFileAttributes(cszPendingAlertQueueFileName))
				{
					g_Log.Log("RemoveHPPFiles() - removing %s.", cszPendingAlertQueueFileName);
					g_Log.Log("RemoveHPPFiles() - removing %s.", cszPendingAlertQueueDirName);

					// jackpot
					if(!DeleteFile(cszPendingAlertQueueFileName))
						FileInUseHandler(cszPendingAlertQueueFileName);

					if(RemoveDirectory(cszPendingAlertQueueDirName) == 0)
						FileInUseHandler(cszPendingAlertQueueDirName);
				}
				else
				{
					g_Log.Log("RemoveHPPFiles() - PendingAlertsQueue was not found. ref: %s.", cszPendingAlertQueueFileName);
				}

			}

			bFindRet = FindNextFile(hFind, &wfd);
		}
		
		FindClose(hFind);
	}
	catch(exception& ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in RemoveHPPFiles().");
	}

	// Always return ERROR_SUCCESS
    return ERROR_SUCCESS;
}