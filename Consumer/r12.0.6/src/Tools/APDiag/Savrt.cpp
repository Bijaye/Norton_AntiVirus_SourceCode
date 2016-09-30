// Savrt.cpp - This file contains the classes that verify Savrt, Savrtpel, and navapsvc
//

#include "stdafx.h"
#include "Savrt.h"

// Savrt checking class implementation
//
CCheckSavrt::CCheckSavrt() : CDriverCheck(_T("SAVRT"))
{
}

bool CCheckSavrt::Install()
{
    TCHAR szSavrtDir[MAX_PATH] = {0};
    TCHAR szDriversDir[MAX_PATH] = {0};
    GetSystemDirectory(szDriversDir,MAX_PATH);
    _tcscat(szDriversDir,_T("\\Drivers"));
    _stprintf(szSavrtDir, _T("\\??\\%s\\savrt.sys"), szDriversDir);

    CDriverCheck::Install(szSavrtDir, 3, 1);

    return true;
}

bool CCheckSavrt::ExtraChecks()
{
    // Make sure NAVAP.SYS is not still around and loading
	CRegKey NavapKey;
	if(	ERROR_SUCCESS == NavapKey.Open(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\NAVAP")) )
	{
		g_log.Log(_T("Error: The NAVAP key still exists."));

		// See if the file exists too
		TCHAR szNavap[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		if( ERROR_SUCCESS == NavapKey.QueryValue(szNavap, _T("ImagePath"), &dwSize) )
		{
			// Remove the first four characters \??\ from the image path if they exist
			TCHAR *szPath = NULL;
			if( 0 == _tcsncmp(szNavap,_T("\\??\\"),4 ) )
				szPath = szNavap+4;
			else
				szPath = szNavap;

			if( -1 != GetFileAttributes(szPath) )
			{
				if( DeleteFile(szPath) )
					g_log.Log(_T("Error: The NAVAP file also exists: %s. The file was removed successfully."), szPath);
				else
					g_log.Log(_T("Error: The NAVAP file also exists: %s. The file could not be removed."), szPath);
				
			}
		}

		// Delete the NAVAP registry key
		NavapKey.Close();
		if(	ERROR_SUCCESS == NavapKey.Open(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services")) )
		{
			if( ERROR_SUCCESS == NavapKey.RecurseDeleteKey(_T("NAVAP")) )
				g_log.Log(_T("Successfully deleted the NAVAP key."));
		}
		
	}
    return true;
}

bool CCheckSavrt::VerifyStartType()
{
    // SAVRT should be set to start manually - 3
	DWORD dwStart = 0;
	if( ERROR_SUCCESS != m_ServiceKey.QueryValue(dwStart, _T("Start")) )
	{
		g_log.Log(_T("Error - CCheckSavrt::VerifyStartType() - The Start key could not be opened for Savrt"));
		return false;
	}

	if( dwStart != 3 )
	{
		g_log.Log(_T("Error - CCheckSavrt::VerifyStartType() - The Start mode for Savrt is not 3 (it is %i). Fixing the start mode"),dwStart);
		if( ERROR_SUCCESS != m_ServiceKey.SetValue(3, _T("Start")) )
		{
			g_log.Log(_T("Error - CCheckSavrt::VerifyStartType() - The Start mode for Savrt could not be set to 3."));
			return false;
		}
		g_log.Log(_T("CCheckSavrt::VerifyStartType() - The Start mode for Savrt was changed to 3."));
	}

    return true;
}


// SavrtPel checking class implementation
//
CCheckSavrtPel::CCheckSavrtPel() : CDriverCheck(_T("SAVRTPEL"))
{
}

bool CCheckSavrtPel::Install()
{
    TCHAR szSavrtPelDir[MAX_PATH] = {0};
    TCHAR szDriversDir[MAX_PATH] = {0};
    GetSystemDirectory(szDriversDir,MAX_PATH);
    _tcscat(szDriversDir,_T("\\Drivers"));
    _stprintf(szSavrtPelDir, _T("\\??\\%s\\savrtpel.sys"), szDriversDir);

    CDriverCheck::Install(szSavrtPelDir, 3, 1);

    return true;
}

bool CCheckSavrtPel::ExtraChecks()
{
    // Make sure NAVAPEL.SYS is not still around and loading
	// Make sure NAVAP.SYS is not still around and loading
	CRegKey NavapKey;
	if(	ERROR_SUCCESS == NavapKey.Open(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\NAVAPEL")) )
	{
		g_log.Log(_T("Error: The NAVAPEL key still exists."));

		// See if the file exists too
		TCHAR szNavap[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		if( ERROR_SUCCESS == NavapKey.QueryValue(szNavap, _T("ImagePath"), &dwSize) )
		{
			// Remove the first four characters \??\ from the image path if they exist
			TCHAR *szPath = NULL;
			if( 0 == _tcsncmp(szNavap,_T("\\??\\"),4 ) )
				szPath = szNavap+4;
			else
				szPath = szNavap;

			if( -1 != GetFileAttributes(szPath) )
			{
				if( DeleteFile(szPath) )
					g_log.Log(_T("Error: The NAVAPEL file also exists: %s. The file was removed successfully."), szPath);
				else
					g_log.Log(_T("Error: The NAVAPEL file also exists: %s. The file could not be removed."), szPath);
				
			}
		}

		// Delete the NAVAP registry key
		NavapKey.Close();
		if(	ERROR_SUCCESS == NavapKey.Open(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services")) )
		{
			if( ERROR_SUCCESS == NavapKey.RecurseDeleteKey(_T("NAVAPEL")) )
				g_log.Log(_T("Successfully deleted the NAVAPEL key."));
		}
		
	}

    return true;
}

bool CCheckSavrtPel::VerifyStartType()
{
    return true;
}
