////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Symevent.cpp - This file contains the class that verifies Symevent is functioning
//

#include "stdafx.h"
#include "DriverCheck.h"

CDriverCheck::CDriverCheck(LPTSTR szDriverName)
{
	// Save the driver name
	_tcscpy(m_szDriverName, szDriverName);

	// Open the services key for this driver
	TCHAR szServiceKeyName[MAX_PATH] = _T("SYSTEM\\CurrentControlSet\\Services\\");
	_tcscat(szServiceKeyName, szDriverName);
	if( ERROR_SUCCESS != m_ServiceKey.Open(HKEY_LOCAL_MACHINE, szServiceKeyName) )
	{
		g_log.Log(_T("Error: CDriverCheck() - Unable to open the services registry key for this driver: %s"), szServiceKeyName);
	}
}

CDriverCheck::~CDriverCheck()
{
}

/***********************************************************
CCheckSymevent::Verify()

This is responsible for verifying that symevent is
installed and working on the system.

// 1. Make sure the file exists
// 2. Log the version
// 3. Make sure the registry key points to the correct file
***********************************************************/
bool CDriverCheck::Verify()
{
	// First make sure the Service registry key existed.
	if( !m_ServiceKey.m_hKey )
	{
		// Attempt to open it again
		TCHAR szServiceKeyName[MAX_PATH] = _T("SYSTEM\\CurrentControlSet\\Services\\");
		_tcscat(szServiceKeyName, m_szDriverName);
		if( ERROR_SUCCESS != m_ServiceKey.Open(HKEY_LOCAL_MACHINE, szServiceKeyName) )
		{
			g_log.Log(_T("Error: CDriverCheck::Verify() - Unable to open the services registry key for this driver: %s"), szServiceKeyName);
			return false;
		}
	}

	if( !CheckFile() )
	{
		g_log.Log(_T("Error: CDriverCheck::Verify() - The CheckFile function failed for driver: %s"), m_szDriverName);
		return false;
	}

	if( !VerifyStartType() )
	{
		g_log.Log(_T("Error: CDriverCheck::Verify() - The VerifyStartType function failed for driver: %s"), m_szDriverName);
		return false;
	}

	if( !ExtraChecks() )
	{
		g_log.Log(_T("Error: CDriverCheck::Verify() - The ExtraChecks function failed for driver: %s"), m_szDriverName);
		return false;
	}

	g_log.Log(_T("CDriverCheck::Verify() succeeded for driver: %s"), m_szDriverName);
	return true;
}

bool CDriverCheck::CheckFile()
{
	// Get the ImagePath for this driver
	DWORD dwSize = MAX_PATH;
	if( ERROR_SUCCESS != m_ServiceKey.QueryValue(m_szPath, _T("ImagePath"), &dwSize) )
	{
		g_log.Log(_T("Error: CDriverCheck::CheckFile() - The ImagePath key could not be opened for driver: %s"), m_szDriverName);
		return false;
	}

	// Remove the first four characters \??\ from the image path if they exist
	TCHAR *szPath = NULL;
	if( 0 == _tcsncmp(m_szPath,_T("\\??\\"),4 ) )
		szPath = m_szPath+4;
	else
		szPath = m_szPath;


	// Make sure the file exists
	if( -1 == GetFileAttributes(szPath) )
	{
		g_log.Log(_T("Error: CDriverCheck::CheckFile() - The file referenced by the ImagePath key does not exist for driver: %s. The path is: %s"), m_szDriverName, szPath);
		return false;
	}

	// Get the version of this file
	TCHAR szVersion[MAX_PATH] = {0};
	if( !GetFileVersion(szPath, szVersion, MAX_PATH) )
		_tcscpy(szVersion, _T("(No Versioning Available)"));

	g_log.Log(_T("Version %s of the %s driver file exists and is loading from: %s"), szVersion, m_szDriverName, szPath);

	return true;
}

bool CDriverCheck::VerifyStartType()
{
	// Generally Auto (2) and Manual (3) are valid start types
	DWORD dwStart = 0;
	if( ERROR_SUCCESS != m_ServiceKey.QueryValue(dwStart, _T("Start")) )
	{
		g_log.Log(_T("Error: CDriverCheck::VerifyStartType() - The Start key could not be opened for driver: %s"), m_szDriverName);
		return false;
	}

	if( dwStart != 2 && dwStart != 3 )
	{
		g_log.Log(_T("Error: CDriverCheck::VerifyStartType() - The Start mode for driver %s was not set to 2 or 3."), m_szDriverName);
		return false;
	}

	return true;
}

bool CDriverCheck::Install(LPCTSTR cszImagePath, DWORD dwStart, DWORD dwType)
{
	if( !m_ServiceKey.m_hKey )
	{
		// Need to create the key...
		TCHAR szServiceKeyName[MAX_PATH] = _T("SYSTEM\\CurrentControlSet\\Services\\");
		_tcscat(szServiceKeyName, m_szDriverName);
		if( ERROR_SUCCESS != m_ServiceKey.Create(HKEY_LOCAL_MACHINE, szServiceKeyName) )
		{
			g_log.Log(_T("Error: CDriverCheck::Install() - Failed to create the %s key: %s"), m_szDriverName, szServiceKeyName);
			return false;
		}
	}

    if( ERROR_SUCCESS != RegSetValueEx(m_ServiceKey.m_hKey, _T("ImagePath"), 0, REG_EXPAND_SZ, (CONST BYTE*)cszImagePath, _tcslen(cszImagePath)+1) )
    {
        g_log.Log(_T("Error: CDriverCheck::Install() - Failed to set the %s ImagePath key to %s"), m_szDriverName, cszImagePath);
        return false;
    }

    if( ERROR_SUCCESS != m_ServiceKey.SetValue(m_szDriverName ,_T("DisplayName")) )
    {
        g_log.Log(_T("Error: CDriverCheck::Install() - Failed to set the %s DisplayName key"), m_szDriverName);
        return false;
    }

    if( ERROR_SUCCESS != m_ServiceKey.SetValue(dwStart ,_T("Start")) )
    {
        g_log.Log(_T("Error: CDriverCheck::Install() - Failed to set the %s Start key"), m_szDriverName);
        return false;
    }

    if( ERROR_SUCCESS != m_ServiceKey.SetValue(dwType ,_T("Type")) )
    {
        g_log.Log(_T("Error: CDriverCheck::Install() - Failed to set the %s Type key"), m_szDriverName);
        return false;
    }

    if( ERROR_SUCCESS != m_ServiceKey.SetValue(1 ,_T("ErrorControl")) )
    {
        g_log.Log(_T("Error: CDriverCheck::Install() - Failed to set the %s ErrorControl key"), m_szDriverName);
        return false;
    }

    return true;
}