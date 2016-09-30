////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// OSInfo.cpp: implementation of the COSInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OSInfo.h"
#include "wtsapi32.h"
#include "tchar.h"

// Initialize static data.
int COSInfo::m_iOSType = COSInfo::OS_TYPE_UNKNOWN;
bool COSInfo::m_bInitialized = false;
bool COSInfo::m_bIsTerminalServicesInstalled = false;
DWORD COSInfo::m_dwSessionID = 0;


//////////////////////////////////////////////////////////////////////
// COSInfo::COSInfo

COSInfo::COSInfo()
{
	// Make sure data gathering only happens once.
	if( m_bInitialized )
		return;

	m_bInitialized = true;

	// Get windows version info.
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &vi );
	
	switch( vi.dwPlatformId )
	{
	case VER_PLATFORM_WIN32_WINDOWS: 
		// Check for Windows Millenium
        //
        if ( vi.dwMajorVersion == 4 && vi.dwMinorVersion >= 90 )
            m_iOSType = OS_TYPE_WINME;
        else
            m_iOSType = OS_TYPE_WIN9x; 
		break;

	case VER_PLATFORM_WIN32_NT:
        if ( vi.dwMajorVersion == 6 )
        {
            m_iOSType = OS_TYPE_WINVISTA;
            break;
        }
        else
        {
		    if ( vi.dwMajorVersion == 5 && vi.dwMinorVersion >= 1 )
                m_iOSType = OS_TYPE_WINXP;
            else
                m_iOSType = OS_TYPE_WINNT; 
		    break;
        }
	default: 
		m_iOSType = OS_TYPE_UNKNOWN; 
		break;
	}

	// Get terminal services flag.
	m_bIsTerminalServicesInstalled = isTerminalServicesInstalled();
}

/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::GetOSType
//
// Get the specific OS type.
//
// @return  <table cellspacing=1>	
//          <tr><td>OS_TYPE_WIN9x</TD>
//	        <tr><td>OS_TYPE_WINNT</TD>
//	        <tr><td>OS_TYPE_WINXP</TD>
//	        <tr><td>OS_TYPE_SOLARIS</TD>
//	        <tr><td>OS_TYPE_LINUX</TD>
//          <tr><td>OS_TYPE_WINME</TD>
//	        <tr><td>OS_TYPE_UNKNOWN</TD>
//          </table>
	
int COSInfo::GetOSType() 
{ 
    return m_iOSType; 
}
	
/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::IsWinNT
//
// Windows NT based OS?
//
// @return  <table cellspacing=1>	
//          <tr><td><i>true</i></td>
//              <td>if OS is Windows NT kernel based (WinNT, Windows 2000, Whistler (WinXP) )</td>
//          </tr>
//          <tr><td><i>false</i></td>
//              <td>if the OS is not NT kernel based.</td>
//          </tr>
//          </table>

bool COSInfo::IsWinNT() 
{
    return (m_iOSType == OS_TYPE_WINNT || 
            m_iOSType == OS_TYPE_WINXP ||
            m_iOSType == OS_TYPE_WINVISTA); 
}

/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::IsDBCS
//
// Is this a DBCS system?
//
// @return  <table cellspacing=1>	
//          <tr><td><i>true</i></td>
//              <td>if OS is DBCS (Japanese, Chinese, Korean, etc.)</td>
//          </tr>
//          <tr><td><i>false</i></td>
//              <td>if the OS is not DBCS.</td>
//          </tr>
//          </table>

bool COSInfo::IsDBCS () 
{
    return (::GetSystemMetrics ( SM_DBCSENABLED ) == TRUE ); 
}

/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::IsWinME
//
// Is this a Windows ME (Millenium) system?
//
// @return  <table cellspacing=1>	
//          <tr><td><i>true</i></td>
//              <td>if OS is Windows ME</td>
//          </tr>
//          <tr><td><i>false</i></td>
//              <td>if the OS is not ME</td>
//          </tr>
//          </table>

bool COSInfo::IsWinME()
{
    return ( m_iOSType == OS_TYPE_WINME ); 
}

/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::IsTerminalServicesInstalled
//
// Does the system have Windows Terminal Services installed?
//
// @return  <table cellspacing=1>	
//          <tr><td><i>true</i></td>
//              <td>Windows Terminal Services are installed.</td>
//          </tr>
//          <tr><td><i>false</i></td>
//              <td>Windows Terminal Services are not installed.</td>
//          </tr>
//          </table>

bool COSInfo::IsTerminalServicesInstalled()
{
    return m_bIsTerminalServicesInstalled; 
}

/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::IsWin32
//
// Win32 based OS
//
// @return  <table cellspacing=1>	
//          <tr><td><i>true</i></td>
//              <td>if OS is Win32 based (NT, 9x, ME, etc.)</td>
//          </tr>
//          <tr><td><i>false</i></td>
//              <td>if OS is not. (UNIX, etc.)</td>
//          </tr>
//          </table>

bool COSInfo::IsWin32()
{
    return ( m_iOSType == OS_TYPE_WINNT ||
             m_iOSType == OS_TYPE_WINXP ||
             m_iOSType == OS_TYPE_WIN9x ||
             m_iOSType == OS_TYPE_WINME ||
             m_iOSType == OS_TYPE_WINVISTA);
}

/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::IsAdminProcess
//
// Does the process have Admin rights?
//
// @return  <table cellspacing=1>	
//          <tr><td><i>true</i></td>
//              <td>User is an administrator (9x returns true always)</td>
//          </tr>
//          <tr><td><i>false</i></td>
//              <td>User is not an admin.</td>
//          </tr>
//          </table>

bool COSInfo::IsAdminProcess()
{
	// On 9x systems, everyone is an admin!
	if( IsWinNT() == false )
		return true;
    
	// open the current process token

    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return false;

    // get the token's group information

    BYTE RawGroupList[4096];
    DWORD dwSize = sizeof(RawGroupList);
    TOKEN_GROUPS& GroupList = *((TOKEN_GROUPS *) RawGroupList);

    if (!GetTokenInformation(hToken, TokenGroups, &GroupList, dwSize, &dwSize))
    {
        CloseHandle(hToken);
        return false;
    }

    // create a SID for the Administrators group to compare to

    PSID pAdminSid;
    SID_IDENTIFIER_AUTHORITY Auth = SECURITY_NT_AUTHORITY;

    if (!AllocateAndInitializeSid(&Auth, 2, SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminSid))
    {
        CloseHandle(hToken);
        return false;
    }

    // loop through groups in token and compare

    bool bFound = false;

    for(DWORD dwIndex = 0; dwIndex < GroupList.GroupCount; dwIndex++)
    {
        if (EqualSid(pAdminSid, GroupList.Groups[dwIndex].Sid))
        {
            bFound = true;
            break;
        }
    }

    // clean-up

    FreeSid(pAdminSid);
    CloseHandle(hToken);

    return bFound;
}

////////////////////////////////////////////////////////////
// ValidateProductSuite function
//
// Terminal Services detection code for systems running
// Windows NT 4.0 and earlier.
//
////////////////////////////////////////////////////////////

BOOL COSInfo::validateProductSuite (LPSTR lpszSuiteToValidate) 
{
	BOOL fValidated = FALSE;
	LONG lResult;
	HKEY hKey = NULL;
	DWORD dwType = 0;
	DWORD dwSize = 0;
	LPSTR lpszProductSuites = NULL;
	LPSTR lpszSuite;
	
	// Open the ProductOptions key.
	
	lResult = RegOpenKeyA(
		HKEY_LOCAL_MACHINE,
		"System\\CurrentControlSet\\Control\\ProductOptions",
		&hKey
		);
	if (lResult != ERROR_SUCCESS)
		goto exit;
	
	// Determine required size of ProductSuite buffer.
	
	lResult = RegQueryValueExA( hKey, "ProductSuite", NULL, &dwType, 
		NULL, &dwSize );
	if (lResult != ERROR_SUCCESS || !dwSize)
		goto exit;
	
	// Allocate buffer.
	
	lpszProductSuites = (LPSTR) LocalAlloc( LPTR, dwSize );
	if (!lpszProductSuites)
		goto exit;
	
	// Retrieve array of product suite strings.
	
	lResult = RegQueryValueExA( hKey, "ProductSuite", NULL, &dwType,
		(LPBYTE) lpszProductSuites, &dwSize );
	if (lResult != ERROR_SUCCESS || dwType != REG_MULTI_SZ)
		goto exit;
	
	// Search for suite name in array of strings.
	
	lpszSuite = lpszProductSuites;
	while (*lpszSuite) 
	{
		if (lstrcmpA( lpszSuite, lpszSuiteToValidate ) == 0) 
		{
			fValidated = TRUE;
			break;
		}
		lpszSuite += (lstrlenA( lpszSuite ) + 1);
	}
	
exit:
	if (lpszProductSuites)
		LocalFree( lpszProductSuites );
	
	if (hKey)
		RegCloseKey( hKey );
	
	return fValidated;
}

//////////////////////////////////////////////////////////////////////
// isTerminalServicesInstalled

bool COSInfo::isTerminalServicesInstalled()
{
	BOOL    bResult = FALSE;
	DWORD   dwVersion;
	OSVERSIONINFOEXA osVersion;
	DWORDLONG dwlCondition = 0;
	HMODULE hmodK32 = NULL;
	HMODULE hmodNtDll = NULL;
	typedef ULONGLONG (WINAPI *PFnVerSetCondition) (ULONGLONG, ULONG, UCHAR);
	typedef BOOL (WINAPI *PFnVerifyVersionA) (POSVERSIONINFOEXA, DWORD, DWORDLONG);
	PFnVerSetCondition pfnVerSetCondition;
	PFnVerifyVersionA pfnVerifyVersionA;
	
	dwVersion = GetVersion();
	
	// Are we running Windows NT?
	
	if (!(dwVersion & 0x80000000)) 
	{
		// Is it Windows 2000 or greater?
		
		if (LOBYTE(LOWORD(dwVersion)) > 4) 
		{
			// Is it WindowsXP ( Rev 5.1 or greater ) ?  If so, 
			// terminal services are present.
			
			if( HIBYTE( LOWORD( dwVersion ) ) > 0 )
				return true;
			
			// In Windows 2000, use the VerifyVersionInfo and 
			// VerSetConditionMask functions. Don't static link because 
			// it won't load on earlier systems.
			
			hmodNtDll = GetModuleHandleA( "ntdll.dll" );
			if (hmodNtDll) 
			{
				pfnVerSetCondition = (PFnVerSetCondition) GetProcAddress( 
					hmodNtDll, "VerSetConditionMask");
				if (pfnVerSetCondition != NULL) 
				{
					dwlCondition = (*pfnVerSetCondition) (dwlCondition, 
						VER_SUITENAME, VER_AND);
					
					// Get a VerifyVersionInfo pointer.
					
					hmodK32 = GetModuleHandleA( "KERNEL32.DLL" );
					if (hmodK32 != NULL) 
					{
						pfnVerifyVersionA = (PFnVerifyVersionA) GetProcAddress(
							hmodK32, "VerifyVersionInfoA") ;
						if (pfnVerifyVersionA != NULL) 
						{
							ZeroMemory(&osVersion, sizeof(OSVERSIONINFOEXA));
							osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
							osVersion.wSuiteMask = VER_SUITE_TERMINAL;
							bResult = (*pfnVerifyVersionA) (&osVersion,
								VER_SUITENAME, dwlCondition);
						}
					}
				}
			}
		}
		else  // This is Windows NT 4.0 or earlier.
			
			bResult = validateProductSuite( "Terminal Server" );
	}
	
	return bResult == TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// @function COSInfo::IsWinXP
//
// Is this a Windows XP (Whistler) system?
//
// @return  <table cellspacing=1>	
//          <tr><td><i>true</i></td>
//              <td>if OS is Windows XP</td>
//          </tr>
//          <tr><td><i>false</i></td>
//              <td>if the OS is not XP</td>
//          </tr>
//          </table>

bool COSInfo::IsWinXP()
{
    return ( m_iOSType == OS_TYPE_WINXP ); 
}

bool COSInfo::IsWinVista()
{
    return ( m_iOSType == OS_TYPE_WINVISTA );
}

DWORD COSInfo::GetSessionID ()
{
	// Try to get pointer to ProcessIDtoSessionID function
	// exported from kernel32.dll.  This only works in 
	// terminal services aware environments.
	typedef BOOL (WINAPI *pfnProcessIDtoSessionID)( DWORD, DWORD* );
	pfnProcessIDtoSessionID pfn;
	HMODULE hMod = GetModuleHandle( _T("kernel32") );
	if( hMod )
	{
		pfn = ( pfnProcessIDtoSessionID ) GetProcAddress( hMod, "ProcessIdToSessionId" );
		if( pfn )
		{
			// Call ProcessIDtoSessionID() routine.
			pfn( GetCurrentProcessId(), &m_dwSessionID );
		}
	}

    return m_dwSessionID;
}

//-------------------------------------------------------------------------
bool COSInfo::IsGuest()
{
	// On 9x systems, everyone is an admin!
	if( IsWinNT() == false )
		return false;

	// open the current process token

	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return false;

	// get the token's group information

	BYTE RawGroupList[4096];
	DWORD dwSize = sizeof(RawGroupList);
	TOKEN_GROUPS& GroupList = *((TOKEN_GROUPS *) RawGroupList);

	if (!GetTokenInformation(hToken, TokenGroups, &GroupList, dwSize, &dwSize))
	{
		CloseHandle(hToken);
		return false;
	}

	// create a SID for the Guests group to compare to

	PSID pGuestSid;
	SID_IDENTIFIER_AUTHORITY Auth = SECURITY_NT_AUTHORITY;

	if (!AllocateAndInitializeSid(&Auth, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_GUESTS, 0, 0, 0, 0, 0, 0, &pGuestSid))
	{
		CloseHandle(hToken);
		return false;
	}

	// loop through groups in token and compare

	bool bFound = false;

	for(DWORD dwIndex = 0; dwIndex < GroupList.GroupCount; dwIndex++)
	{
		if (EqualSid(pGuestSid, GroupList.Groups[dwIndex].Sid))
		{
			bFound = true;
			break;
		}
	}

	// clean-up

	FreeSid(pGuestSid);
	CloseHandle(hToken);

	return bFound;
}

