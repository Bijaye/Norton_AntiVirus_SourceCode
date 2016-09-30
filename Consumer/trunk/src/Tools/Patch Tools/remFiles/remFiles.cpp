////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// remFiles.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

//
// Globals
//
CDebugLog g_Log("remFiles.log");

const TCHAR g_cszNAVGUID[] = _T("{3075C5C3-0807-4924-AF8F-FF27052C12AE}");
const TCHAR g_cszNAVPROGUID[] = _T("{68AE158E-38F9-4027-A757-A82B00E45D5C}");
const TCHAR g_cszNAVAppsKey[] = _T("Software\\Symantec\\Norton AntiVirus");

//
// Local Function prototypes
//
bool IsNavPro();
bool AddFileToBeRemoved( MSIHANDLE hDB, LPCTSTR cszFileKey, LPCTSTR cszComponent, LPCTSTR cszFileName, LPCTSTR cszDirectory, LPCTSTR cszInstMode);

//
// Main
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    // Get the location of the local package
    TCHAR szLocalPackage[MAX_PATH] = {0};
    DWORD dwSize = MAX_PATH;

    // Use the correct NAV GUID depending on the product (NAV or NAV Pro)
    LPCTSTR pcszGUID = IsNavPro() ? g_cszNAVPROGUID : g_cszNAVGUID;

    if( ERROR_SUCCESS == MsiGetProductInfo(pcszGUID, INSTALLPROPERTY_LOCALPACKAGE, szLocalPackage, &dwSize) )
    {
        // Open the local package
        MSIHANDLE hNAVdb = NULL;

 	    if( ERROR_SUCCESS == MsiOpenDatabase(szLocalPackage, MSIDBOPEN_DIRECT, &hNAVdb) )
        {
            //
            // Add the alert.dat file to the RemoveFile table of the Local Package
            //
            if( !AddFileToBeRemoved( hNAVdb, _T("alert.dat"), _T("NAVPROXY.DAT.B721A671_09EA_4E4B_95EC_BCC7E7D329F3"),
                                    _T("alert.dat"), _T("Norton_AntiVirus.B721A671_09EA_4E4B_95EC_BCC7E7D329F3"), _T("2")) )
            {
                g_Log.Log("Unable to add file to be removed.");
            }

            // Clean Up
            MsiDatabaseCommit(hNAVdb);
            MsiCloseHandle(hNAVdb);
        }
        else
            g_Log.Log("Unable to open the Local Package: %s.  Bailing Out", szLocalPackage);
    }
    else
        g_Log.Log("Unable to obtain the LocalPackage property using MsiGetProductInfo with the GUID: %s", g_cszNAVGUID);

    

    //
    // Replace the navap.cat and navap.inf files on XP platforms
    //
    OSVERSIONINFO osInfo = {0};
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    ::GetVersionEx(&osInfo);
    
    if(VER_PLATFORM_WIN32_NT == osInfo.dwPlatformId)
    {
        // Get the NAV Directory
        HKEY hSubkey;
	TCHAR szNAVPath[MAX_PATH] = {0};
        TCHAR szNAVAPCat[MAX_PATH] = {0};
        TCHAR szNAVAPInf[MAX_PATH] = {0};
        TCHAR szNAVAPCatNew[MAX_PATH] = {0};
        TCHAR szNAVAPInfNew[MAX_PATH] = {0};
	    DWORD dwSize = sizeof(szNAVPath);

	    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"), 0, KEY_READ, &hSubkey))
	    {
		    if(ERROR_SUCCESS == RegQueryValueEx(hSubkey, _T("NAV"), NULL, NULL, (LPBYTE)szNAVPath, &dwSize))
		    {
                _stprintf(szNAVAPCat, _T("%s\\navap.cat"), szNAVPath);
                _stprintf(szNAVAPInf, _T("%s\\navap.inf"), szNAVPath);

                if( 0 != GetModuleFileName(hInstance, szNAVAPCatNew, MAX_PATH) )
                {
                    TCHAR* pszBkSlash = _tcsrchr(szNAVAPCatNew, _TCHAR('\\'));
                    *pszBkSlash = _TCHAR('\0');
                    _stprintf(szNAVAPInfNew, _T("%s\\navap.inf"), szNAVAPCatNew);
                    _tcscat(szNAVAPCatNew, _T("\\navap.cat"));

                    // See if these bad boys exist
                    if( -1 != GetFileAttributes(szNAVAPCat) )
                    {
                        if( 0 == CopyFile(szNAVAPCatNew, szNAVAPCat, FALSE) )
                            g_Log.Log("Failed to copy navap.cat to the system");
                    }
                    else
                        g_Log.Log("navap.cat does not exist on this system...expected for non-XP systems");
                    if( -1 != GetFileAttributes(szNAVAPInf) )
                    {
                        if( 0 == CopyFile(szNAVAPInfNew, szNAVAPInf, FALSE) )
                            g_Log.Log("Failed to copy navap.inf to the system");
                    }
                    else
                        g_Log.Log("navap.inf does not exist on this system...expected for non-XP systems");
                }
                else
                    g_Log.Log("Unable to get the Module file name for remFiles.exe");
            } // End if RegQueryValueEx()
            else 
                g_Log.Log("Unable to query the NAV installed apps key for the NAV install directory");
            RegCloseKey(hSubkey);
	    } 	// End if RegOpenKeyEx()
        else
            g_Log.Log("Unable to Open the Symantec Installed Apps registry key");
    } // End if NT
                                      
	return 0;
}

// Checks if NAVPRO
bool IsNavPro()
{
	LONG lResult;
	HKEY hKey;
	bool bReturn = false;
	
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVAppsKey, 0, KEY_QUERY_VALUE, &hKey);
	
	if (lResult == ERROR_SUCCESS)
	{
		TCHAR szNAVInstType[2] = {0};
		DWORD dwSize = sizeof(szNAVInstType);
		
		lResult = RegQueryValueEx(hKey, _T("Type"), NULL, NULL, (BYTE* ) &szNAVInstType, &dwSize);
		
		if (lResult == ERROR_SUCCESS)
		{
			// If it's NAV Professional, Type should be set to 1
			if (_ttoi(szNAVInstType) == 1)
			{
				bReturn = true;
			}
		}
		
		RegCloseKey(hKey);
	}
	
	return bReturn;
}

// Adds a file to the RemoveFile table of the database handle passed in
//
// Parameters:
// hDB:             Handle to the MSI Database.
// cszFileKey:      Name of the primary key MSI uses to identify this table entry.
// cszComponent:    Name of an existing component to associate this file with.
// cszFileName:     Name of the file that needs to be deleted.
// cszDirectory:    Name of an existing property that resolves to the full path of
//                  of the directory where this file exists. This property can be
//                  from either the Directory table, AppSearch table, or any other
//                  property that represents a full path.
// cszInstMode:     Must be one of the following values:
//                  1 - Remove only when the associated component is being installed
//                  2 - Remove only when the associated component is being removed
//                  3 - Remove in either of the above cases
//
bool AddFileToBeRemoved( MSIHANDLE hDB, LPCTSTR cszFileKey, LPCTSTR cszComponent, LPCTSTR cszFileName, LPCTSTR cszDirectory, LPCTSTR cszInstMode)
{
    // Verify data input
    if (!hDB || !cszFileKey || !cszComponent || !cszFileName || !cszDirectory || !cszInstMode)
    {
        g_Log.Log("The parameters passed into AddFileToBeRemoved() were invalid!");
        return false;
    }

    // Construct SQL Query string
    TCHAR szQuery[1024] = {0};
    _stprintf(szQuery, _T("INSERT INTO RemoveFile \
                           (RemoveFile.FileKey,RemoveFile.Component_,RemoveFile.FileName,RemoveFile.DirProperty,RemoveFile.InstallMode) \
                           VALUES ('%s','%s','%s','%s','%s')"),
                           cszFileKey, cszComponent, cszFileName, cszDirectory, cszInstMode);

    // Make Query
    MSIHANDLE hRemoveFileView = NULL;
    UINT ret = MsiDatabaseOpenView(hDB,szQuery,&hRemoveFileView);

    if( ERROR_BAD_QUERY_SYNTAX == ret )
    {
        g_Log.Log("The MsiDatabaseOpenView failed due to bad query syntax");
        return false;
    }
    else if( ERROR_SUCCESS != ret )
    {
        g_Log.Log("The MsiDatabaseOpenView failed for a reason other than bad query syntax.");
        return false;
    }
    
    if( ERROR_SUCCESS != MsiViewExecute(hRemoveFileView, 0) )
    {
        g_Log.Log("The attempt to MsiViewExecute() failed.");
        MsiViewClose(hRemoveFileView);
        return false;
    }

    MsiViewClose(hRemoveFileView);
    return true;
}