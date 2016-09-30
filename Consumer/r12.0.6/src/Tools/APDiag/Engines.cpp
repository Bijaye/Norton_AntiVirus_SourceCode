// Engines.cpp - This file contains the class that verifies the virus engines are functioning
//

#include "stdafx.h"
#include "Engines.h"
#include "DefUtils.h"

// General Functions used by the Engines classes
//

bool RebuildUsage(LPTSTR pszNewestDefsDir)
{
    // Get the defs dir and the Section name
    TCHAR szDefsDir[MAX_PATH] = {0};
    TCHAR szSectionName[50] = {0};
    _tcscpy(szDefsDir, pszNewestDefsDir);
    TCHAR* pszBckSlsh = _tcsrchr(szDefsDir, _TCHAR('\\'));
    _tcscpy(szSectionName, pszBckSlsh+1);
    if( pszBckSlsh )
        *pszBckSlsh = NULL;

    // Get the path to usage.dat
    TCHAR szUsage[MAX_PATH] = {0};
    TCHAR szUsageBak[MAX_PATH] = {0};
    _tcscpy(szUsage, szDefsDir);
    _tcscpy(szUsageBak, szDefsDir);
    _tcscat(szUsage, _T("\\usage.dat"));
    _tcscat(szUsageBak, _T("\\usage.bak"));

    // backup the old usage.dat just in case to usage.bak
    if( !CopyFile(szUsage, szUsageBak, FALSE) )
        g_log.Log(_T("Error: RebuildUsage() - Failed to backup usage.dat from path %s"), szUsage);
    
    // Open the usage.dat and truncate existing contents then close it
    HANDLE hUsage = CreateFile(szUsage, 
                               GENERIC_ALL,
                               FILE_SHARE_WRITE | FILE_SHARE_READ,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if( INVALID_HANDLE_VALUE != hUsage )
    {
        g_log.Log(_T("RebuildUsage() - Created the usage.dat file using path %s"), szUsage);
        CloseHandle(hUsage);
    }
    else
    {
        DWORD dwErr = GetLastError();
        g_log.Log(_T("Error: RebuildUsage() - Failed to create the usage.dat file using path %s. Error code: %u"), szUsage, dwErr);
        return false;
    }

    // Create a section containing the folder name of the NewestDefsDir and all
    // of the apps referenced by the SharedDefs registry key
    HKEY hSharedDefs = NULL;
    if( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\SharedDefs"), 0, KEY_QUERY_VALUE, &hSharedDefs) )
    {
        TCHAR szData[2048] = {0}, szCurValue[MAX_PATH] = {0};
        DWORD dwCurValSize = MAX_PATH, dwCurType = 0, dwDataPos = 0;
        for(int i=0; ERROR_SUCCESS == RegEnumValue(hSharedDefs, i, szCurValue, &dwCurValSize, NULL, &dwCurType, NULL, NULL); i++, dwCurValSize = MAX_PATH)
        {
            // Copy the current value name to the data buffer with an =1 at the end
            _tcscat(szCurValue, _T("=1"));
            dwCurValSize += 3;
            memcpy(szData+dwDataPos, szCurValue, dwCurValSize);
            dwDataPos += dwCurValSize;
        }

        WritePrivateProfileSection(szSectionName, szData, szUsage);
        RegCloseKey(hSharedDefs);
    }
    else
    {
        // Could not open the SharedDefs registry key
        g_log.Log(_T("Error: RebuildUsage() - Failed to open the SharedDefs registry key."));
        return false;
    }

    return true;
}

bool GetCurrentVirusDefsFolder(LPTSTR pszFolder, DWORD dwBufSize)
{
	CDefUtils* defUtils = NULL;
	defUtils = new CDefUtils;

	if( !defUtils )
	{
		g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - Unable to allocate a CDefUtils object."));
		return false;
	}

	if( !defUtils->InitWindowsApp(_T("NAVNT_50_AP1")) )
	{
		g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - InitWindowsApp failed for NAVNT_50_AP1"));

		// Make sure the AVENGEDEFS key exists
		CRegKey InstApps;
		if( ERROR_SUCCESS != InstApps.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\InstalledApps")) )
		{
			g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - The installed apps key could not be opened!"));
			delete defUtils;
			return false;
		}

        DWORD dwSize = dwBufSize;
		if( ERROR_SUCCESS == InstApps.QueryValue(pszFolder, _T("AVENGEDEFS"), &dwSize) )
		{
			g_log.Log(_T("The AVENGEDEFS installed apps key exists and is currently pointing to %s"), pszFolder);

			if( -1 == GetFileAttributes(pszFolder) )
			{
                g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - The virus defs folder %s does not exist."), pszFolder);
                delete defUtils;
				return false;
			}
		}

        // Create the AVENGEDEFS key
        if( !g_CCInfo.GetCCFolder(pszFolder) )
        {
            g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - Unable to get the Symantec Shared directory"));
            delete defUtils;
			return false;
        }

        _tcscat(pszFolder, _T("\\VirusDefs"));
        GetShortPathName(pszFolder, pszFolder, dwBufSize);
		if( ERROR_SUCCESS != InstApps.SetValue(pszFolder, _T("AVENGEDEFS")) )
        {
            g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - Failed to set the AVENGEDEFS installed apps key"));
            delete defUtils;
			return false;
        }

		// Close this key
		InstApps.Close();

		// Get a new instance of CDefUtils
		delete defUtils;
		defUtils = NULL;
		defUtils = new CDefUtils;

		if( !defUtils )
		{
			g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - Unable to allocate a second CDefUtils object."));
			return false;
		}

        g_log.Log(_T("Set the AVENGEDEFS installed apps key to %s"),pszFolder);

        // Try again
        if( !defUtils->InitWindowsApp(_T("NAVNT_50_AP1")) )
        {
            g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - InitWindowsApp still failed."));
            delete defUtils;
			return false;
        }

		// Created the key so the service will need to be restarted in order to use defs
		g_bRestartNavapsvc = true;
	}

    // Get the defs in use by AP
    if( !defUtils->GetCurrentDefs(pszFolder, dwBufSize) )
    {
        g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - GetCurrentDefs failed. Attempting to rebuild usage.dat"));

        // Rebuild the usage.dat file
        TCHAR szNewestDefs[MAX_PATH] = {0};
        WORD wYear, wMonth, wDay;
        DWORD dwRev;
        if( defUtils->GetNewestDefsDate(&wYear, &wMonth, &wDay, &dwRev) )
        {
            // Build the path to the newest defs directory and use it to Rebuild usage.dat
            TCHAR szCCFolder[MAX_PATH] = {0};
            g_CCInfo.GetCCFolder(szCCFolder);
            _stprintf(szNewestDefs, _T("%s\\VirusDefs\\%04i%02i%02i.%03u"), szCCFolder, wYear, wMonth, wDay, dwRev);

            if( RebuildUsage(szNewestDefs) )
            {
                g_log.Log(_T("Rebuilt the usage.dat file using directory %s, now attempting to UseNewestDefs and then GetCurrentDefs for AP."), szNewestDefs);
                // It will be necessary to restart the service now
                g_bRestartNavapsvc = true;
            }
            else
            {
                g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - RebuildUsage failed for the newest directory %s"), szNewestDefs);
            }
        }
        else
        {
            g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - GetNewestDefsDate failed."));

            // Possibly check for and re-create definfo.dat???
        }

		// Attempt to use the newest defs
		bool bChanged = false;
		if( defUtils->UseNewestDefs(&bChanged) )
		{
			if( bChanged )
				g_log.Log(_T("GetCurrentVirusDefsFolder() - Updated to use newest defs."));
			else
				g_log.Log(_T("GetCurrentVirusDefsFolder() - Already using newest defs."));

			// Try to get the CurrentDefs now
			if( !defUtils->GetCurrentDefs(pszFolder, dwBufSize) )
			{
				g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - GetCurrentDefs failed after updating to newest defs. GetCurrent returned: %s"), pszFolder);
				delete defUtils;
				return false;
			}
			else
			{
				g_log.Log(_T("GetCurrentVirusDefsFolder() - Now using defs: %s"), pszFolder);
				delete defUtils;
				return true;
			}
		}

		g_log.Log(_T("Error: GetCurrentVirusDefsFolder() - UseNewestDefs failed."));
        delete defUtils;
		return false;
    }
	
	delete defUtils;
	return true;
}

// Naveng checking class implementation
//
CCheckNaveng::CCheckNaveng() : CDriverCheck(_T("NAVENG"))
{
}

bool CCheckNaveng::Install()
{
	// Get the current path to naveng
	TCHAR szNavengPath[MAX_PATH] = {0};
	GetCurrentVirusDefsFolder(szNavengPath, MAX_PATH);
	_tcscat(szNavengPath,_T("\\naveng.sys"));
	
	// Make sure that the file exists
	if( -1 == GetFileAttributes(szNavengPath) )
	{
		g_log.Log (_T("Error: CCheckNaveng::Install - The Naveng.sys file %s does not exist"), szNavengPath);
		return false;
	}

	TCHAR szImagePath[MAX_PATH] = _T("\\??\\");

    _tcscat(szImagePath, szNavengPath);

    CDriverCheck::Install(szImagePath, 3, 1);

	return true;
}

bool CCheckNaveng::ExtraChecks()
{
	// Make sure the AVENGEDEFS key and hawkings is working
	TCHAR szPath[MAX_PATH] = {0};
	GetCurrentVirusDefsFolder(szPath,MAX_PATH);

	return true;
}

bool CCheckNaveng::VerifyStartType()
{
	// Naveng should be set to start manually - 3
	DWORD dwStart = 0;
	if( ERROR_SUCCESS != m_ServiceKey.QueryValue(dwStart, _T("Start")) )
	{
		g_log.Log(_T("Error - CCheckNaveng::VerifyStartType() - The Start key could not be opened for Naveng"));
		return false;
	}

	if( dwStart != 3 )
	{
		g_log.Log(_T("Error - CCheckNaveng::VerifyStartType() - The Start mode for Naveng is not 3 (it is %i). Fixing the start mode"),dwStart);
		if( ERROR_SUCCESS != m_ServiceKey.SetValue(3, _T("Start")) )
		{
			g_log.Log(_T("Error - CCheckNaveng::VerifyStartType() - The Start mode for Naveng could not be set to 3."));
			return false;
		}
		g_log.Log(_T("CCheckNaveng::VerifyStartType() - The Start mode for Naveng was changed to 3."));
	}

	return true;
}


// Navex15 checking class implementation
//
CCheckNavex::CCheckNavex() : CDriverCheck(_T("NAVEX15"))
{
}

bool CCheckNavex::Install()
{
    // Get the current path to naveng
	TCHAR szNavexPath[MAX_PATH] = {0};
	GetCurrentVirusDefsFolder(szNavexPath, MAX_PATH);
	_tcscat(szNavexPath,_T("\\navex15.sys"));

	// Make sure that the file exists
	if( -1 == GetFileAttributes(szNavexPath) )
	{
		g_log.Log (_T("Error: CCheckNavex::Install - The Navex15.sys file %s does not exist"), szNavexPath);
		return false;
	}

	TCHAR szImagePath[MAX_PATH] = _T("\\??\\");
    _tcscat(szImagePath, szNavexPath);

    CDriverCheck::Install(szImagePath, 3, 1);

	return true;
}

bool CCheckNavex::ExtraChecks()
{
	return true;
}

bool CCheckNavex::VerifyStartType()
{
	// Navex should be set to start manually - 3
	DWORD dwStart = 0;
	if( ERROR_SUCCESS != m_ServiceKey.QueryValue(dwStart, _T("Start")) )
	{
		g_log.Log(_T("Error - CCheckNavex::VerifyStartType() - The Start key could not be opened for Navex"));
		return false;
	}

	if( dwStart != 3 )
	{
		g_log.Log(_T("Error - CCheckNavex::VerifyStartType() - The Start mode for Navex is not 3 (it is %i). Fixing the start mode"),dwStart);
		if( ERROR_SUCCESS != m_ServiceKey.SetValue(3, _T("Start")) )
		{
			g_log.Log(_T("Error - CCheckNavex::VerifyStartType() - The Start mode for Navex could not be set to 3."));
			return false;
		}
		g_log.Log(_T("CCheckNavex::VerifyStartType() - The Start mode for Navex was changed to 3."));
	}

	return true;
}