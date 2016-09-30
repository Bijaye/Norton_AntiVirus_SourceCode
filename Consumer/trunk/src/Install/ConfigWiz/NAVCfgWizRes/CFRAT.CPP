////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CFRAT.cpp: implementation of the CAuditMode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CFRAT.h"
#include "OsInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFRAT::CFRAT() :
	m_bIsAuditMode(false),
	m_bIsFactoryMode(false),
	m_bIsReseal(false)
{

}

CFRAT::~CFRAT()
{

}


//////////////////////////////////////////////////////////////////////
// Initialize()								[Chad Corriveau 2/22/2002]
//
// --Determines if we are in Factory Mode, Audit Mode, or If
//		Factory Reseal has Occured
//
//////////////////////////////////////////////////////////////////////
void CFRAT::Initialize()
{
	///////////////////////////////////////////////////////////////////
	// For Audit mode detection in Windows 9X(95,98,98SE,ME)
	//		HKLM\Software\Microsoft\Windows\CurrentVersion
	//			Value: AuditInProgress=1(String Value)
	// For Audit mode detection in Windows XP
	//		HKEY_LOCAL_MACHINE\SYSTEM\Setup
	//			value: AuditInProgress (dword: 0x1)
	//			value: FactoryPreInstallInProgress (dword: 0x1)
	// For Factory mode detection in Windows 2K
	//		HKEY_LOCAL_MACHINE\SYSTEM\Setup
	//			value: FactoryPreInstallInProgress (dword: 0x1)
	//
	// For Ractory Reseal Detection in Windos 2K and Xp
	//		HKEY_LOCAL_MACHINE\SYSTEM\Setup
	//			value: CloneTag (Stamped with time of Reseal)
	//
	// For more information contact Chad Corriveau EXT: (310) 8147
	///////////////////////////////////////////////////////////////////

	HKEY hkResult = NULL;
	LONG lReturn = 0;
	OSVERSIONINFO VersionInfo;

	VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
	
	//Getting the operating system version we are running on
	GetVersionEx(&VersionInfo);
	
	COSInfo OSInfo;
	if(OSInfo.GetOSType() == COSInfo::OS_TYPE_WINVISTA)
	{
		lReturn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, WINNT_FRAT_KEY, 0, KEY_READ, &hkResult);

		if(ERROR_SUCCESS == lReturn)
		{
			DWORD dwOobe(-1), dwSsip(-1), dwSize;

			dwSize = sizeof(dwOobe);
			lReturn = RegQueryValueEx(hkResult, WINVISTA_OOBE_VALUE, NULL, NULL, (BYTE*)&dwOobe, &dwSize);
			if(lReturn == ERROR_SUCCESS)
			{
				dwSize = sizeof(dwSsip);
				lReturn = RegQueryValueEx(hkResult, WINVISTA_SSIP_VALUE, NULL, NULL, (BYTE*)&dwSsip, &dwSize);
				if(lReturn == ERROR_SUCCESS)
				{
					RegCloseKey(hkResult);
				}

				m_bIsAuditMode = (dwOobe == 1 && dwSsip == 0) ? true : false;
				m_bIsReseal = (dwOobe == 0 && dwSsip == 0) ? true : false;
			}
		}
	}
	else if (VER_PLATFORM_WIN32_WINDOWS == VersionInfo.dwPlatformId)
	{
		//Then it is Windows 9x platform
		lReturn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, WIN9X_AUDITMODE_KEY, 0, KEY_READ, &hkResult);

		if (ERROR_SUCCESS == lReturn)
		{
			//Checking for the vlaue letting us know if we are in
			//  audit mode or not.
			lReturn = RegQueryValueEx(hkResult, AUDIT_MODE_VALUE, NULL, NULL, NULL, NULL);

			RegCloseKey(hkResult);

			m_bIsAuditMode = (ERROR_SUCCESS == lReturn ? true : false);
		}
	}
	else if (VER_PLATFORM_WIN32_NT == VersionInfo.dwPlatformId)
	{
		//Then it is Windows Nt platform
		lReturn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, WINNT_FRAT_KEY, 0, KEY_READ,  &hkResult);
		
		if (ERROR_SUCCESS == lReturn)
		{
			//On all versions of the NT flavor. (NOTE: NAV2003 does not support NT4 -- only for 2K and XP)
			// Checking for value that lets us know of a factory reseal has occured
			lReturn = RegQueryValueEx(hkResult, FACTORY_RESEAL_VALUE, NULL, NULL, NULL, NULL);
				
			m_bIsReseal = (ERROR_SUCCESS == lReturn ? true : false);
			
			if (5 >= VersionInfo.dwMajorVersion && 1 >= VersionInfo.dwMinorVersion)
			{
				//We are working on an XP or later system here
				// Checking for the value that lets us know we are in Audit Mode
				lReturn = RegQueryValueEx(hkResult, AUDIT_MODE_VALUE, NULL, NULL, NULL, NULL);
				
				m_bIsAuditMode = (ERROR_SUCCESS == lReturn ? true : false);
			}
			else
			{
				//Other older versions of the NT flavor. (NOTE: NAV2003 does not support NT4 -- only for 2K)
				// Checking for value that lets us know of factory mode
				lReturn = RegQueryValueEx(hkResult, FACTORY_MODE_VALUE, NULL, NULL, NULL, NULL);
				
				m_bIsFactoryMode = (ERROR_SUCCESS == lReturn ? true : false);
			}
			RegCloseKey(hkResult);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// IsAuditMode():
//
// --Returns true is the machine is currently in Audit Mode (applies to Win9x & XP)
//
////////////////////////////////////////////////////////////////////////////////
bool CFRAT::IsAuditMode()
{
	if(m_bIsAuditMode)
		CCTRCTXI0 (_T( "CFRAT: Detected AuditMode is on." ));
	return m_bIsAuditMode;
}


////////////////////////////////////////////////////////////////////////////////
// IsFactoryMode():
//
// --Returns true is the machine is currently in Factory Mode (applies to Win2K)
//
////////////////////////////////////////////////////////////////////////////////
bool CFRAT::IsFactoryMode()
{
	if(m_bIsFactoryMode)
		CCTRCTXI0 (_T( "CFRAT: Detected FactoryMode is on." ));
	return m_bIsFactoryMode;
}

////////////////////////////////////////////////////////////////////////////////
// IsReseal():
//
// --Returns true if the machine has been resealed (applies to Win2K and XP)
//
////////////////////////////////////////////////////////////////////////////////
bool CFRAT::IsReseal()
{
	if(m_bIsReseal)
		CCTRCTXI0 (_T( "CFRAT: Detected Reseal." ));
	return m_bIsReseal;
}