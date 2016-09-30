// UnRegCmd.cpp : Defines the entry point for the application.
//
// If this executable is run with a -i commandline parameter it will
// remove the MicroDefs 1.0 commandlines from the LU product catalog
// and update the registry commandlines with the MicroDefs 2.0 data.
// 
// WJORDAN 3-12-2002: Fix for Defect# 381370. Now also setting access
// rights for alert.dat to give Everyone full access to the file.
//
// If it is not run with a -i commandline it will just copy itself to
// the NAV directory and create a run key entry to run on the next reboot
// with the -i parameter

#include "stdafx.h"

#import "PRODUCTREGCOM.TLB"
#include "LuCOM.h"

// local function prototypes
BOOL DeleteFileInUse(TCHAR* szFileName);


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    // Get the full path to this executable
    // since we will need it no matter which path we take
    TCHAR szFullPath[MAX_PATH];
    GetModuleFileName(hInstance, szFullPath, MAX_PATH);

    // Check if we are installing MicroDefs 2.0 or just
    // setting ourselves up to be run at the next reboot
    if( 0 == _tcscmp(lpCmdLine, _T("-i")) )
    {
        // WJORDAN 4-8-2002: Adding registration of Qserver.exe

        // We are installing MicroDefs 2.0 now
        //
        // First remove the current MicroDefs 1.0 commandlines and the Group
        // to make way for the new 2.0 stuff
        //
        try 
        {
            if( SUCCEEDED(CoInitialize(NULL)) )
            {
                PRODUCTREGCOMLib::IluProductRegPtr ptrProductReg = NULL;
                PRODUCTREGCOMLib::IluGroupPtr ptrGroup = NULL;

                // Remove the old MicroDefs Product groups from LiveUpdate
                if( SUCCEEDED(ptrProductReg.CreateInstance(__uuidof( luProductReg ),NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER))) )
                {
                    try
                    {
                        ptrProductReg->DeleteProduct(_bstr_t("{BFC05484-BAB7-11d2-9C30-00C04FB59D98}"));
                    }
                    catch(...)
                    {
                    }
                    try
                    {
                        ptrProductReg->DeleteProduct(_bstr_t("{BFC05483-BAB7-11d2-9C30-00C04FB59D98}"));
                    }
                    catch(...)
                    {
                    }
                    ptrProductReg = NULL;
                }

                // Remove the group
                if( SUCCEEDED(ptrGroup.CreateInstance( __uuidof( luGroup ), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER))) )
                {
                    try
                    {
                        ptrGroup->DeleteGroup( _bstr_t("{E5C8D100-B06E-11d2-9C30-00C04FB59D98}") );
                    }
                    catch(...)
                    {
                    }
                    ptrGroup = NULL;
                }

                CoUninitialize();
            }
        }
        catch(...)
        {
            // Swallow all errors
        }

        //
        // Now add the proper commandlines for MicroDefs 2.0 so the LU callback
        // will register them the next time LU is run
        //
        CRegKey cmdLineKey;
    
        // Modify CmdLine1
        if( ERROR_SUCCESS == cmdLineKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus\\LiveUpdate\\CmdLines\\CmdLine1")) )
        {
            cmdLineKey.SetValue(_T("SymAllLanguages"), _T("ProductLanguage"));
            cmdLineKey.SetValue(_T("{ACB5A41F-D2C5-436e-82B5-7446C0E64EBA}"), _T("ProductMoniker"));
            cmdLineKey.SetValue(_T("Avenge 1.5 MicroDefs2"), _T("ProductName"));
            cmdLineKey.SetValue(_T("MicroDefsB.Full"), _T("ProductVersion"));

            cmdLineKey.Close();
        }

        // Modify CmdLine4
        if( ERROR_SUCCESS == cmdLineKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus\\LiveUpdate\\CmdLines\\CmdLine4")) )
        {
            cmdLineKey.SetValue(_T("SymAllLanguages"), _T("ProductLanguage"));
            cmdLineKey.SetValue(_T("{C7D827FA-85C8-4b26-8FE6-AA6431BD765F}"), _T("ProductMoniker"));
            cmdLineKey.SetValue(_T("Avenge 1.5 MicroDefs2"), _T("ProductName"));
            cmdLineKey.SetValue(_T("MicroDefsB.CurDefs"), _T("ProductVersion"));

            cmdLineKey.Close();
        }

        //
        // Set the Security priveleges on the All Users\Application Data\Symantec\Norton AntiVirus\alert.dat so
        // everyone can access it on NT platforms
        //
        OSVERSIONINFO osInfo = {0};
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);
        ::GetVersionEx(&osInfo);
    
        if(VER_PLATFORM_WIN32_NT == osInfo.dwPlatformId)
        {
            HKEY hKey = NULL;
            TCHAR szAlertDat[MAX_PATH] = {0};
            DWORD dwSize = MAX_PATH;

            if( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                              _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
                                              0,
                                              KEY_QUERY_VALUE,
                                              &hKey) )
            {
                if( ERROR_SUCCESS == RegQueryValueEx(hKey, _T("Common AppData"), NULL, NULL, (LPBYTE)&szAlertDat, &dwSize) )
                {
                    _tcscat(szAlertDat, _T("\\Symantec\\Norton AntiVirus\\alert.dat"));

                    SECURITY_DESCRIPTOR sd = {0};
                    if ( ::InitializeSecurityDescriptor ( &sd,SECURITY_DESCRIPTOR_REVISION) )
                    {
                        // set NULL DACL on the SD
                        if ( ::SetSecurityDescriptorDacl (&sd, TRUE, ( PACL) NULL, FALSE) )
                        {
                            ::SetFileSecurity(szAlertDat,DACL_SECURITY_INFORMATION,&sd);
                        }
                    }
                }
                RegCloseKey(hKey);
            }
        }

        // Register Qserver.exe
        CNAVInfo navInfo;
        TCHAR szQserver[MAX_PATH] = {0};

        // Get full path to qserver.exe
        _tcscpy(szQserver, navInfo.GetNAVDir());
        _tcscat(szQserver, _T("\\QServer.exe"));

        ShellExecute(NULL, NULL, szQserver, _T("/regserver"), navInfo.GetNAVDir(), SW_SHOWNORMAL);

        //
        // Set myself to be deleted on reboot
        //
        DeleteFileInUse( szFullPath );
    }
    else
    {
        // This means we were not launched with the -i commandline parameter
        // Just copy ourself to the NAV directory and place us in the run key
        // with a -i parameter

        // First get the NAV directory
        // Get the NAV install path
        TCHAR szNAVPath[MAX_PATH] = {0};
        DWORD dwSize = MAX_PATH;
        HKEY hInstAppsKey = NULL;

        if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_ALL_ACCESS, &hInstAppsKey))
        {
            if(ERROR_SUCCESS == RegQueryValueEx(hInstAppsKey, _T("NAV"), NULL, NULL, (LPBYTE)szNAVPath, &dwSize))
            {
                // Remove any trailing backslashes from the NAV directory
                int len = _tcslen(szNAVPath);
                if( szNAVPath[len-1] == _TCHAR('\\') )
                {
                    szNAVPath[len-1] = _TCHAR('\0');
                }

                // Get the file name for this executable
                // leaving the trailing backslash, so that we can
                // append it to the NAV directory where we will copy it
                TCHAR* szFileNameWithBackSlash = _tcsrchr(szFullPath, _TCHAR('\\'));

                // Create the full path to the destination file
                _tcscat(szNAVPath,szFileNameWithBackSlash);

                // Copy the file
                CopyFile(szFullPath, szNAVPath, FALSE);

                // Append a '-i' to the newly copied path
                // this will be the commandline parameter so the next
                // time it runs it will set up MicroDefs 2.0
                _tcscat(szNAVPath, _T(" -i"));

                // Now add it to the runonce key
                HKEY hRunKey = NULL;
                if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"), 0, KEY_ALL_ACCESS, &hRunKey))
                {
                    RegSetValueEx(hRunKey, _T("UnRegCmd"), 0, REG_SZ, (LPBYTE)szNAVPath, _tcslen(szNAVPath)+1 );
                    RegCloseKey(hRunKey);
                }
            }
            RegCloseKey(hInstAppsKey);
        }
    }

	return 0;
}

BOOL DeleteFileInUse(TCHAR* szFileName)
{
    BOOL bRet = FALSE;
	FILE* fpFile;
    TCHAR szValue[MAX_PATH] = "SSB";
	TCHAR szFile[MAX_PATH] = {0};
	TCHAR szWinInit[MAX_PATH] = {0};
	TCHAR szLine[MAX_PATH] = {0};
    OSVERSIONINFO osInfo;

	if (DeleteFile(szFileName) == 0)
	{
		// Get the short file name
		GetShortPathName(szFileName, szFile, MAX_PATH);

		// determine which platform we're running on (Win95 or NT)
		osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (FALSE == GetVersionEx(&osInfo))
		    return (bRet);

		if (VER_PLATFORM_WIN32_NT == osInfo.dwPlatformId)
		{
		    MoveFileEx(szFile, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		    bRet = TRUE;
		}
		else if (VER_PLATFORM_WIN32_WINDOWS == osInfo.dwPlatformId)
		{
		    GetWindowsDirectory(szWinInit, MAX_PATH);
			_tcscat(szWinInit, _T("\\wininit.ini"));

		    WritePrivateProfileString("Rename", szValue, szFile, szWinInit);
		    WritePrivateProfileString(NULL, NULL, NULL, szWinInit);
		    fpFile = fopen(szWinInit, "rt");

		    if (fpFile != NULL)
		    {
		        fpos_t fpos;

		        do
		        {
		            fgetpos(fpFile, &fpos);
		            fgets(szLine, MAX_PATH, fpFile);

		            if (_tcsncmp(szLine, szValue, 3) == 0)
		            {
		                fflush(fpFile);
		                fclose(fpFile);

		                fpFile = fopen(szWinInit, "r+t");

		                if (fpFile != NULL)
		                {
		                    fsetpos(fpFile, &fpos);
		                    fprintf(fpFile, "NUL=%s\n", szFile);
		                    fflush(fpFile);
		                    fclose(fpFile);
		                    fpFile = NULL;
		                }
		                break;
		            }
		        }
		        while(!feof(fpFile));

			    if (fpFile != NULL)
			        fclose(fpFile);

			    bRet = TRUE;
		    }
		}
	}

    return (bRet);
}