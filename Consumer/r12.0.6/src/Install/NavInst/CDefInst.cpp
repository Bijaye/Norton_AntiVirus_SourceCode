/////////////////////////////////////////////////////////////////////////////////////////////
// CDefinst.cpp: contains definstaller class implementation
//
// COLLIN DAVIS
/////////////////////////////////////////////////////////////////////////////////////////////

#include "CDefinst.h"
#include "helper.h"
#include "NAVDefutilsLoader.h"
#include "FileAction.h"

#include "unmark.h"

//
//Constructor: initialize private data values
//
CDefinst::CDefinst()
{
	SharedDefs = _T("Software\\Symantec\\SharedDefs");
	cstrLatestDefsPath = _T("Latest");
	cstrTestFile = _T("virscan1.dat");	
}


/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PUBLIC FUNCTION: Inst												COLLIN DAVIS
//
// MODULE USAGE: Used on install only by DefInst.msm
//
// DESCRIPTION:  Installs Definitions onto client machine.  Reads
//				 Definstaller.exe's "command line" switches from
//				 from the properties table of the install and 
//				 registers the application ID's with DefUtils.	
//  
/////////////////////////////////////////////////////////////////////////////////////////////
UINT CDefinst::Inst(MSIHANDLE hInstall)
{
	MSIDefUtilsLoader DefUtilsLoader;
	
	// scoping the loader to make sure that the IDefUtilsPtr goes out of scope before the
	// the loader does
	{
		g_hDefsInstall = hInstall;
		IDefUtilsPtr pDefUtils;
		g_hDefsInstall = hInstall;

		// load the Defutils object
		if (SYM_FAILED(DefUtilsLoader.CreateObject(pDefUtils.m_p)) || pDefUtils == NULL)
			return -1;

		pDefUtils->SetDoAuthenticationCheck(true);
		//
		// Parse the Properties tables to look for DefaultPath & APIDs
		//
		if (!ParseProperties(hInstall))
		{
			return -1;
		}
        
        // figure out where to copy defs from in the layout
        TCHAR szHubPath[MAX_PATH*2] = {0};
		TCHAR* pszTemp = NULL;
        DWORD dwSize = sizeof(szHubPath) / sizeof(szHubPath[0]);

        // find where the MSI is being installed from
		if (MsiGetSourcePath(hInstall, _T("SOURCEDIR"), szHubPath, &dwSize) != ERROR_SUCCESS)
		{
		   return -1;
		}
		
        // go up one level from the source directory and append VirusDef
        PathAppend(szHubPath, _T("..\\VirusDef"));


        // PROBLEM:  Hawkings doesn't differentiate bewteen eraser enabled defs
        // and non-eraser enabled defs.  so if a customer has a defs on the machine
        // that are newer than the defs in our layout it won't install them... even
        // if they dont have eraser.  so we need to force a backrev of defs in this
        // case to switch to eraser enabled defs.
        //

        // figure out where the current defs are if they are installed on
        // the machine
        TCHAR szCurDefsFolder[MAX_PATH*2] = {0};
        bool bEraserExists = true; // default to true, so we use the standard install case
                                   // when no defs are installed

        // install our temporary APPID. we'll remove this below
        pDefUtils->InitWindowsApp(_T("NAV_RETAIL_INSTALLER"));
        
        // call UseNewestDefs to find the current installed latest defs (there may not be any)
        if(pDefUtils->UseNewestDefs(szCurDefsFolder, sizeof(szCurDefsFolder)))
        {
            // check the current defs set for ccEraser.dll
            TCHAR szEraserDll[MAX_PATH*2] = {0};
            _tcscpy(szEraserDll, szCurDefsFolder);
            PathAppend(szEraserDll, _T("ccEraser.dll"));
            if(!InstallToolBox::FileExists(szEraserDll))
                bEraserExists = false;
        }
        
        // stop using defs so our ID doesn't get left over.
        pDefUtils->StopUsingDefs();

        // Call DefUtils::InitInstallApp() on all the application(s) we are 
		// installing the virus definitions for. 
		//
		m_iInstApps = m_InstApps.begin();
	

		for (; m_iInstApps != m_InstApps.end(); ++m_iInstApps)
    	{
	    	pDefUtils->InitInstallApp((*m_iInstApps).szAppID, (*m_iInstApps).szAppPath);
		}
	
		//
		// Call DefUtils::PreDefUpdate to get a path to copy our virus 
		// definitions to.
		//
		TCHAR szTempPath[MAX_PATH*2] = {0};
        pDefUtils->PreDefUpdate(szTempPath, sizeof(szTempPath)/sizeof(TCHAR), DU_INSTALL);

		//
		// Copy the virus definitions.
		//
		if (CopyDefs(szHubPath, szTempPath))
		{
            //
		    // Call DefUtils::PostDefUpdate after we copied the virus definitions
		    // to the specified location.
	        //  
            if(bEraserExists)
            {
                // if eraser is already there then just call normal PostDefUpdate
                // we dont want to force a backrev if they already have eraser.
                pDefUtils->PostDefUpdate();
            }
            else
            {
                // when eraser is not present we need to call postdefupdate with
                // the new Always install flag that was added to the DefUtils5 interface
                IDefUtils5QIPtr pDefUtils5(pDefUtils);
                if(pDefUtils5 != NULL)
                    pDefUtils5->PostDefUpdate(true);
            }
		}

		// Check if the "Latest" directory exists in the VirusDef folder
		TCHAR szLatestDefPath[MAX_PATH*2] = {0};
		TCHAR szLatestDefTestFile[MAX_PATH*2] = {0};

		wsprintf(szLatestDefPath, _T("%s\\%s"), szHubPath, cstrLatestDefsPath);
		wsprintf(szLatestDefTestFile, _T("%s\\%s"), szLatestDefPath, cstrTestFile);

		DWORD dwReturn = GetFileAttributes(szLatestDefTestFile);

		// If the Latest directory exists with files, install the latest defs
		if (dwReturn != -1)
		{
			pDefUtils->PreDefUpdate(szTempPath, sizeof(szTempPath)/sizeof(TCHAR), DU_AV_UPDATER);
			
			if (!CopyDefs(szLatestDefPath, szTempPath))
			{
				return -1;
			}

			pDefUtils->PostDefUpdate();
		}

		//
		// Go through our application list and call DefUtils::InitWindowsApp and
		// DefUtils::UseNewestDefs.
		//
		m_iInstApps = m_InstApps.begin();
		

		for (; m_iInstApps != m_InstApps.end(); ++m_iInstApps)
		{
			pDefUtils->InitWindowsApp((*m_iInstApps).szAppID);
				
			bool bChanged;
			TCHAR szDir[MAX_PATH] = {0};
			pDefUtils->UseNewestDefs(szDir, sizeof(szDir), &bChanged);
		}        

		return S_OK;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PUBLIC FUNCTION: UnInst												COLLIN DAVIS
//
// MODULE USAGE: Used on UnInstall only by DefInst.msm
//
// DESCRIPTION:  Unregisters IDs installed for definitions
//				 on client machine by the installer.  The
//				 properties table of the install lists these.
//				 If Definitions are longer being used, DefUtils
//				 will automatically delete the files.  
//
/////////////////////////////////////////////////////////////////////////////////////////////
UINT CDefinst::UnInst(MSIHANDLE hInstall)
{
	DefUtilsLoader DefUtilsLoader;

	// scoping the loader to make sure that the IDefUtilsPtr goes out of scope before the
	// the loader does
    {
		g_hDefsInstall = hInstall;
		IDefUtilsPtr pDefUtils;

		// load the Defutils object
		if (SYM_FAILED(DefUtilsLoader.CreateObject(pDefUtils.m_p)) ||pDefUtils == NULL)
			return -1;
		
		//add the app ID's to m_InstApps
		if (!ParseProperties(hInstall))
		{
			return -1;
		}	

		//initialize the iterator
		m_iInstApps = m_InstApps.begin();
		
		//loop through application id list and unregister each id from defutils.
		for (; m_iInstApps != m_InstApps.end(); ++m_iInstApps)
		{
			pDefUtils->InitInstallApp((*m_iInstApps).szAppID);
			
			pDefUtils->StopUsingDefs();
		}

		return S_OK;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// 								
// PRIVATE FUNCTION: ParseProperties										COLLIN DAVIS						
// 
// Description:	Parses the properties table of the DefInst.msm  
//				to get the parameters needed to install Definitions
//
// hInstall:	Handle to the Windows Installer
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDefinst::ParseProperties(MSIHANDLE hInstall)
{

	TCHAR szOption[MAX_PATH*2] = {0};
	DWORD dwSize = sizeof(szOption);
	TCHAR szDefaultAppPath[MAX_PATH*2] = {0};
	TCHAR szIDField[MAX_PATH] = {0};
	TCHAR szGUID[MAX_PATH] = {0};
	int iCount = 0;

	wsprintf(szGUID, "31723B88_F23E_4307_B18A_2463C204A61D");
	
	//
	// Look for DefaultPath
	//
	if (MsiGetTargetPath(hInstall, _T("INSTALLDIR"), szOption, &dwSize) == ERROR_SUCCESS)
	{
		_tcscpy(szDefaultAppPath, szOption);

		int iLength = _tcslen(szDefaultAppPath);

		// if the last character of the path is a '\', remove it.
		if (szDefaultAppPath[iLength - 1] == '\\')
			szDefaultAppPath[iLength - 1] = '\0';
	}
	else
	{
		// If DefaultPath does not exists in the command line parameter,
		// Quit.
		return FALSE;
	}

	//
	// Need to parse the rest of the properties to look for Application
	// IDs. Then insert the Application IDs and DefaultPath into our
	// list.
	//
	
	//get the number of ID's from the NUMIDS property in the MSI
	wsprintf(szIDField, "NUMIDS.%s", szGUID);
	
	if (MsiGetProperty(hInstall, szIDField, szOption, &dwSize) == ERROR_SUCCESS)
	{
		iCount = atoi(szOption);
	}
	else
	{
		return FALSE;
	}
	
	//
	// Loop through each property and insert the 
	// appid into the m_InstApps list
	//
	OSVERSIONINFO localosvi = {0};
	localosvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&localosvi);
	for (int i = 1; i <= iCount; i++ )
	{
		ZeroMemory(szOption, MAX_PATH);
		
		//create the property name by inserting the current count in between "ID" and the GUID
		wsprintf(szIDField, "ID%d.%s", i, szGUID);
		
		dwSize=sizeof(szOption);

		//get the value of the current ID
		if (MsiGetProperty(hInstall, szIDField, szOption, &dwSize) == ERROR_SUCCESS)
		{	
			//we need to do a quick check for NT/9x Specific AP IDs here.  and then only insert the
			//apropritate ones into the list
			if(localosvi.dwPlatformId == VER_PLATFORM_WIN32_NT)  //if this an NT machine, make sure its not a NAV95 Key
			{
			  if((strcmp(szOption, "NAV95_50_AP1") != 0) && (strcmp(szOption, "NAV95_50_AP2") != 0))
				InsertInstAppInfo(szOption, szDefaultAppPath);
			}
			else												 //if its not an NT machine, make sure its not a NAVNT Key
			 if(strcmp(szOption, "NAVNT_50_AP1") != 0)
				InsertInstAppInfo(szOption, szDefaultAppPath);
		}
		else
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PRIVATE FUNCTION InsertInstAppInfo()
// 
// Description:	InsertAppInfo will insert the AppID and AppPath to the
//				g_InstApp List.
//
// szAppID -	Application ID
// szAppPath -	Application path
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDefinst::InsertInstAppInfo(TCHAR* szAppID, TCHAR* szAppPath)
{
	DefAppInfo AppInfo;

	_tcscpy(AppInfo.szAppID, szAppID);
	
	if (szAppPath == NULL)
		_tcscpy(AppInfo.szAppPath, _T("\0"));
	else
		_tcscpy(AppInfo.szAppPath, szAppPath);

	m_InstApps.push_back(AppInfo);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// 
// PRIVATE FUNCTION: CopyDefs()
// 
// Description:	CopyDefs will copy the list of virus definition files
//				to the specified directory.
//
// szSrcPath  - Path of the virus definition we are installing
//
// szTempPath -	This is path where the virus definition will be copied
//				to. This is the path returned by calling
//				DefUtils::PreDefUpdate();
//
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDefinst::CopyDefs(TCHAR* szSrcPath, TCHAR *szTempPath)
{
	BOOL bReturn = TRUE;
	DWORD dwErrorCode = 0;
	TCHAR szCurrentDir[MAX_PATH*2] = {0};
	WIN32_FIND_DATA FindData;

	wsprintf(szCurrentDir, _T("%s\\%s"), szSrcPath, _T("*.*"));

	HANDLE hFile = FindFirstFile(szCurrentDir, &FindData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	TCHAR szExistFileName[MAX_PATH*2] = {0};
	TCHAR szNewFileName[MAX_PATH*2] = {0};

	//
	// Copy all the files in the VirusDef directory (except DefInst.exe)
	//
	do
	{
		if (_tcscmp(FindData.cFileName, _T(".")) == 0 || _tcscmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		wsprintf(szExistFileName, "%s\\%s", szSrcPath, FindData.cFileName);
		wsprintf(szNewFileName, "%s\\%s", szTempPath, FindData.cFileName);

		if (CopyFile(szExistFileName, szNewFileName, FALSE) == 0)
		{
			bReturn = FALSE;
			dwErrorCode = GetLastError();
		}

        //
        // Make sure this file hasn't already been marked for delete
        //  Should make sure that the file we're unmarking is in
        //  the actual directory we're concerned with, but this
        //  function doesn't currently support that operation.
        //
        InstallToolBox::UnMarkFilesForDeletion(FindData.cFileName);
		
		if (SetFileAttributes(szNewFileName, FILE_ATTRIBUTE_NORMAL) == 0)
		{
			dwErrorCode = GetLastError();
		}
	} while (FindNextFile(hFile, &FindData));

	FindClose(hFile);
	
	return bReturn;
}
