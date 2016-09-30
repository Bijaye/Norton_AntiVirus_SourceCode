// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// StubSetup.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "StubSetup.h"

#include "Utility.h"
#include "Cverrsrc.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStubSetupApp

BEGIN_MESSAGE_MAP(CStubSetupApp, CWinApp)
	//{{AFX_MSG_MAP(CStubSetupApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStubSetupApp construction

CStubSetupApp::CStubSetupApp()
{ 
	// Init all MSI function addresses to NULL
	m_MsiEnableLog			= NULL;
	m_MsiOpenPackage		= NULL;
	m_MsiCloseHandle		= NULL;
	m_MsiSetInternalUI		= NULL;
	m_MsiGetFileVersion		= NULL;
	m_MsiGetProductProperty	= NULL;

	// Get the OS Version information
	m_bWinNT = FALSE;
	m_bWin95 = FALSE;
	m_bWin9x = FALSE;

	OSVERSIONINFO   osInfo;

	memset( &osInfo, 0, sizeof( OSVERSIONINFO ) );
	osInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

	GetVersionEx( &osInfo );

	if ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
		m_bWinNT = TRUE;
	else
	{
		m_bWin95 = ( (osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
			( (osInfo.dwMajorVersion == 4) && (osInfo.dwMinorVersion == 0) ) );
		m_bWin9x = (osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CStubSetupApp object

CStubSetupApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CStubSetupApp initialization

CStubSetupApp::~CStubSetupApp()
{

	if(NULL != m_hMSILib)
	{
		// Set all function addresses to NULL since we are unloading library
		m_MsiEnableLog			= NULL;
		m_MsiOpenPackage		= NULL;
		m_MsiCloseHandle		= NULL;
		m_MsiSetInternalUI		= NULL;
		m_MsiGetFileVersion		= NULL;
		m_MsiGetProductProperty	= NULL;

		FreeLibrary(m_hMSILib);
	}
}

BOOL CStubSetupApp::InitInstance()
{
	GetSystemDirectory(m_strWinSysDir.GetBuffer(_MAX_PATH), _MAX_PATH);
	m_strWinSysDir.ReleaseBuffer();

	CString strModule;
	LPSTR lpStrModule = strModule.GetBuffer(MAX_PATH);
	int nLen = GetModuleFileName(AfxGetInstanceHandle(), lpStrModule, MAX_PATH);	
	GetShortPathName(lpStrModule, lpStrModule, nLen);
	strModule.ReleaseBuffer();

	int nPos= strModule.ReverseFind('\\');

	if(nPos > 0)
	{
		m_strModuleDir = strModule.Left(nPos);
		m_strSupportDir = m_strModuleDir;
		m_strSupportDir += "\\Support";
	}

	// Install MSI if need be and set pointers to MSI functions
	InitMSI();
	ResolveMSIFunctions();

	if(m_bWin95)
	{
		CString strTgt(m_strWinSysDir);
		CString strSrc(m_strSupportDir);

		BuildPathName(strTgt, CString("OLEAUT32.DLL"));
		BuildPathName(strSrc, CString("OADIST.EXE"));

		if(FileExists(strSrc))
		{
			BOOL bInstallOA = TRUE;

			if(FileExists(strTgt))
			{
				CVersionResource verTgt ( strTgt );

				verTgt.InitializeData();

				DWORD dwFileVerMS = MAKELONG(20, 2);
				DWORD dwFileVerLS = MAKELONG(0, 4054);

				DWORD dwRet = verTgt.CompareFileVer(dwFileVerMS, dwFileVerLS);

				if(dwRet != CVER_COMPARE_OLDER)
					bInstallOA = FALSE;
			}

			if(bInstallOA)
			{
				strSrc += " /Q";
				LaunchAppAndWait(strSrc);
			}
		}

		strTgt = m_strWinSysDir;
		BuildPathName(strTgt, CString("MSVCRT.DLL"));
		strSrc = m_strSupportDir;
		BuildPathName(strSrc, CString("MSVCRT.DLL"));

		if(!FileExists(strTgt))
		{
			CopyFile(strSrc, strTgt, TRUE);
		}
	}

	LPTSTR lpCmdLine = GetCommandLine();

	CString strBuf(lpCmdLine);
	CString strArgs("");

	if(strBuf.GetAt(0) == '\"')
	{
		int nQPos = strBuf.Find("\"", 1);

		if(nQPos > 0)
		{
			int nSPos = strBuf.Find(" ", nQPos);
			
			if(nSPos > 0 && strBuf.GetLength() > (nSPos + 1))
			{
				strArgs = strBuf.Right(strBuf.GetLength() - nSPos - 1);
			}
		}
	}
	else
	{
		int nSPos = strBuf.Find(" ", 0);

		if(nSPos > 0 && strBuf.GetLength() > (nSPos + 1))
		{
			strArgs = strBuf.Right(strBuf.GetLength() - nSPos - 1);
		}
	}

	// Check to see if we only need to do an upgrade and not a full install
	// But only upgrade previous MSI versions of the install (7.2 or greater)
	DWORD dwCVer = GetCurrentVersion();
	DWORD dwPVer = 0;
	HKEY  hMKey = NULL;
	CString MediaHiveKey;

	if( dwCVer >= 720 )
	{
		DWORD dwClientType = GetClientType();
		if( dwClientType == CLIENTTYPE_CONNECTED || dwClientType == CLIENTTYPE_STANDALONE )
		{
			CString VolumeName, RootPathName = m_strModuleDir.Left(3);
			
			if( m_bWin9x )
			{
				hMKey = HKEY_CURRENT_USER;
				MediaHiveKey = MEDIA_KEY_HIVE_9X;
			}
			else
			{
				hMKey = HKEY_CLASSES_ROOT;
				MediaHiveKey = MEDIA_KEY_HIVE_NT;
			}

			// Only change vulome label if where installing from the cdrom.
			if( ( GetDriveInfo(VolumeName, RootPathName) == DRIVE_CDROM ) && GetMediaKeyFromEncryptProductCode(hMKey, MediaHiveKey) )
			{
				
				HKEY  hKey = NULL;
				// Search registry for navcorp source media

				// Save the hive before we change the media value.
				if(RegCopyValue( hMKey, MediaHiveKey, HKEY_LOCAL_MACHINE, TEMPORARY_HIVE ) == TRUE)
				{
					
					LONG Status = RegOpenKeyEx( hMKey, MediaHiveKey, NULL, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE, &hKey);

					if( Status == ERROR_SUCCESS ) 
					{
						// Enumarate through all the value from Installer\\Products\\74BE21DBFDBD3D11EBAE000ACC725290\\SourceList\\Media
						// look for the string "NAVCORP7##" or "Disk1" which is the name of navcorp CD vulome label
						DWORD dwSubKeyIndex = 0;
						do
						{
							DWORD dwType;
							TCHAR tszDataBuffer[REGSTR_MAX_VALUE_LENGTH];
							TCHAR tszStringValueBuffer[REGSTR_MAX_VALUE_LENGTH];
							DWORD dwDataBuffer = REGSTR_MAX_VALUE_LENGTH;
							DWORD dwStringValueBuffer;

							dwStringValueBuffer = REGSTR_MAX_VALUE_LENGTH;
							 
							Status = RegEnumValue ( hKey, dwSubKeyIndex, tszStringValueBuffer, &dwStringValueBuffer,
											NULL, &dwType, LPBYTE(tszDataBuffer), &dwDataBuffer );
							// increment to the next string value
							++dwSubKeyIndex;
							// we are looking for the name "1"
							if ( Status == ERROR_SUCCESS && (strlen(tszStringValueBuffer) == 1) && ( tszStringValueBuffer[0] == '1' ) )
							{
								CString DataBuffer = tszDataBuffer;
								CString NavcorpMediaName = VolumeName + DataBuffer.Right(DataBuffer.GetLength() - DataBuffer.ReverseFind(';'));
								// Just fail if the value can't be changed
								RegSetValueEx( hKey, tszStringValueBuffer, 0, dwType, (CONST BYTE *)(NavcorpMediaName.GetBuffer(NavcorpMediaName.GetLength())),
											NavcorpMediaName.GetLength() );
								CString MediaPackage = m_strModuleDir.Right(m_strModuleDir.GetLength() - 2) + "\\";
								// Fix for XP not resolving the cd's path
								RegSetValueEx( hKey, MEDIAPACKAGE, 0, dwType, (CONST BYTE *)(MediaPackage.GetBuffer(MediaPackage.GetLength())),
											MediaPackage.GetLength() );

								// Just quit when we find the string
								break;
							}

						} while ( Status == ERROR_SUCCESS );

						RegCloseKey(hKey);
					}
				} 
				else
				{
					// Any value that was successfully created
					RegDeleteValue( HKEY_LOCAL_MACHINE, TEMPORARY_HIVE );
					RegDeleteKey ( HKEY_LOCAL_MACHINE, TEMPORARY_HIVE );
				}
			}
		}
		// Before we append reinstall paramters make sure we are not uninstalling
		// and that we do not overwrite any reinstall parameters give by user
		CString szCmdLine = ::GetCommandLine();
		if( (-1 == szCmdLine.Find("/x")) && (-1 == szCmdLine.Find("/f")) )
		{
			// Read the version from the MSI package
			dwPVer = GetPackageVersion();
			if( dwPVer >= dwCVer )
					// Add the m_strModuleDir as a property so we can determine working 
					// directory inside install. First check to see if the /v option
					// is being used, if so append to it, otherwise add it for our new
					// property
					if( -1 == strArgs.Find("/v") )
						strArgs += " /v\"UPGRADEDIR=" + m_strModuleDir + "\" /fva";
					else
					{
						// Take off closing "
						CString szTemp = strArgs.Left(strArgs.GetLength() - 1);
						strArgs = szTemp + " UPGRADEDIR=" + m_strModuleDir + "\" /fva";
					}
		}
	}

	CString strCmdLine, szTemp;
	
	strCmdLine.Format("\"%s\\_Setup.Exe\" %s", m_strModuleDir, strArgs);

	// Debug messagebox
//	szTemp.Format("%s\nCurrent Ver = %d, Package Ver = %d",strCmdLine,dwCVer,dwPVer);
//	::MessageBox(NULL,szTemp,"Symantec Antivirus Corporate Edition",MB_OK);

#if 0
	CString strBuf2(lpCmdLine);

	strBuf2.MakeUpper();
	int nExePos = strBuf2.Find("SETUP.EXE");

	CString strCmdLine;

	strCmdLine = strBuf.Left(nExePos);
	strCmdLine += "_Setup.Exe";
	strCmdLine += strBuf.Right(strBuf.GetLength() - nExePos - CString("Setup.Exe").GetLength());
#endif
//	int nRet = AfxMessageBox(strCmdLine, MB_YESNO);

//	if(nRet==IDYES) 
	// see if nav return an error
	LaunchAppAndWait(strCmdLine);

	return FALSE;
}

BOOL CStubSetupApp::LaunchAppAndWait(CString& strCmdLine)
{
	ZeroMemory(&m_SInfo,sizeof(STARTUPINFO));
	m_SInfo.cb = sizeof(STARTUPINFO);

//	GetStartupInfo(&m_SInfo);
//	m_SInfo.lpTitle = (LPTSTR)(LPCTSTR)m_strTitle;
//	m_SInfo.dwFlags =  STARTF_USESHOWWINDOW;
//	m_SInfo.wShowWindow = SW_SHOWNOACTIVATE;

    int nSafeCmdLineLength = _tcslen((LPCTSTR)strCmdLine) + 3;
	TCHAR *safeCmdLine= new TCHAR[nSafeCmdLineLength];

	if (! _tcschr( strCmdLine, _T('\"')))
		sssnprintf( safeCmdLine, nSafeCmdLineLength, "\"%s\"", (LPCTSTR)strCmdLine );
	else
		_tcscpy( safeCmdLine, (LPCTSTR)strCmdLine );

	BOOL bRet = ::CreateProcess(NULL,
		safeCmdLine,
		NULL, NULL, FALSE, 0, NULL, NULL, &m_SInfo, &m_PInfo);
	// CREATE_NEW_CONSOLE

	delete [] safeCmdLine;

	WaitForSingleObject((HANDLE)m_PInfo.hProcess, INFINITE);

	CloseHandle(m_pInfo.hProcess);
	CloseHandle(m_pInfo.hThread);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: GetDriveInfo(CString& VolumeName, CString& RootPathName)
//
// Description:
//  Get Volume name and drive type
//
// Input:
//  required only RootPathName 
//
// Returns:
//  Drive Type and volume name
// Value Meaning 
//
// DRIVE_UNKNOWN - The drive type cannot be determined. 
// DRIVE_NO_ROOT_DIR - The root path is invalid. For example, no volume is mounted at the path. 
// DRIVE_REMOVABLE - The disk can be removed from the drive. 
// DRIVE_FIXED - The disk cannot be removed from the drive. 
// DRIVE_REMOTE - The drive is a remote (network) drive. 
// DRIVE_CDROM - The drive is a CD-ROM drive. 
// DRIVE_RAMDISK - The drive is a RAM disk. 
//
//////////////////////////////////////////////////////////////////////////
// 08/21/01  Henry Jalandoni -- function created
//////////////////////////////////////////////////////////////////////////

UINT CStubSetupApp::GetDriveInfo(CString& VolumeName, CString& RootPathName)
{

	char szVolumeName[128];
	if(GetVolumeInformation(RootPathName,szVolumeName,sizeof(szVolumeName) - 1,NULL,NULL,NULL,NULL,0) == 0)
	{
		return DRIVE_UNKNOWN;
	}
	VolumeName = szVolumeName;
	return GetDriveType(RootPathName);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    InitMSI()
//
// Description:
//  Check to see if MSI is installed, if not run the appropriate installer
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 01/09/01  Sean Kennedy -- function created
//////////////////////////////////////////////////////////////////////////
void CStubSetupApp::InitMSI(void)
{
	// Check if MSI is installed, if not run the MSI installation
	HKEY	hMSIKey;
	CString	szKeyName;
	szKeyName.LoadString(IDS_DETECTMSIKEY);
	LONG	lSuccess = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
								(LPCSTR)szKeyName,
								0,
								KEY_READ,
								&hMSIKey);
	if(ERROR_SUCCESS != lSuccess)
	{
		// Failure means MSI is not installed so we need to install it
		CString szMSIName;

		// Set the filename for either the WinNT or 9x version
		if(TRUE == m_bWinNT)
			szMSIName = "instmsiw.exe";
		else
			szMSIName = "instmsia.exe";

		// Check to see if this is a request for a silent install if so
		// pass this request to the MSI installer
//		CString szCmdLine = ::GetCommandLine(); <-- This needs to go back in
//		if( -1 == szCmdLine.Find("/s") )		<-- when we can determine how to keep MSI from rebooting
		szMSIName += " /q";

		LaunchAppAndWait(szMSIName);
	}
	else
		RegCloseKey(hMSIKey);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    ResolveMSIFunctions()
//
// Description:
//  Load the MSI.DLL library and set the member function pointers
//  This function should only be called after the installation of MSI on
//  those platforms that do not currently have it
//
// Input:
//  Nothing.
//
// Returns:
//  Always BOOL, TRUE if successful
//
//////////////////////////////////////////////////////////////////////////
// 01/09/01  Sean Kennedy -- function created
//////////////////////////////////////////////////////////////////////////
BOOL CStubSetupApp::ResolveMSIFunctions(void)
{
	BOOL bSuccessful = FALSE;

	// Before we can use any MSI functions we need to load the DLL and
	// resolve the function addresses
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\MSI.DLL"));
	m_hMSILib = LoadLibrary(szPath);
	if(NULL != m_hMSILib)
	{
		m_MsiCloseHandle = (UINT(WINAPI *)(MSIHANDLE))GetProcAddress(m_hMSILib,"MsiCloseHandle");
		m_MsiOpenPackage = (UINT(WINAPI *)(LPCTSTR,MSIHANDLE*))GetProcAddress(m_hMSILib,"MsiOpenPackageA");
		m_MsiGetProductProperty = (UINT(WINAPI *)(MSIHANDLE,LPCTSTR,LPTSTR,DWORD*))GetProcAddress(m_hMSILib,"MsiGetProductPropertyA");
		m_MsiGetFileVersion = (UINT(WINAPI *)(LPCSTR,LPCSTR,DWORD *,LPCSTR,DWORD *))GetProcAddress(m_hMSILib,"MsiGetFileVersionA");
		m_MsiEnableLog = (UINT(WINAPI *)(DWORD,LPCTSTR,DWORD))GetProcAddress(m_hMSILib,"MsiEnableLogA");
		m_MsiSetInternalUI = (INSTALLUILEVEL(WINAPI *)(INSTALLUILEVEL,HWND *))GetProcAddress(m_hMSILib,"MsiSetInternalUI");
		if( (NULL != m_MsiOpenPackage) &&
			(NULL != m_MsiCloseHandle) &&
			(NULL != m_MsiSetInternalUI) &&
			(NULL != m_MsiGetFileVersion) &&
			(NULL != m_MsiEnableLog) &&
			(NULL != m_MsiGetProductProperty) )
			bSuccessful = TRUE;
	}

	return bSuccessful;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    GetCurrentVersion()
//
// Description:
//  Get the current version (if any) of NAV installed on the machine
//
// Input:
//  Nothing.
//
// Returns:
//  Always DWORD, the version
//
//////////////////////////////////////////////////////////////////////////
// 01/09/01  Sean Kennedy -- function created
//////////////////////////////////////////////////////////////////////////
DWORD CStubSetupApp::GetCurrentVersion(void)
{
	DWORD	dwVersion	= 0;
	DWORD	dwBuild		= 0;
	HKEY	hVersionKey;

	CString	szKeyName;
	szKeyName.LoadString(IDS_CURRENTVERSIONKEY);

	// Open the key containing version and read the value
	LONG lSuccess = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
								(LPCSTR)szKeyName,
								0,
								KEY_READ,
								&hVersionKey);
	if(ERROR_SUCCESS == lSuccess)
	{	
		DWORD	dwSize = MAX_PATH;
		DWORD	dwType;
		LPSTR	pBuffer;
		CString	szValue;

		// Read the "ProductVersion" value from the registry
		pBuffer = szValue.GetBuffer(dwSize);
		lSuccess = SymSaferRegQueryValueEx(	hVersionKey,
									"ProductVersion", 
									NULL,
									&dwType,
									(LPBYTE)pBuffer,
									&dwSize);
		
		// If successful parse out the version and build numbers
		if(ERROR_SUCCESS == lSuccess)
		{
			dwVersion = LOWORD( *( reinterpret_cast<int *>(pBuffer) ) );
			dwBuild   = HIWORD( *( reinterpret_cast<int *>(pBuffer) ) );
		}
		
		szValue.ReleaseBuffer();

		RegCloseKey(hVersionKey);
	}

	return dwVersion;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    GetPackageVersion()
//
// Description:
//  Get the version of the MSI file from the current directory
//	Must call ResolveMSIFunctions() before calling this to set the address
//	of the MSI function we will be using
//
// Input:
//  Nothing.
//
// Returns:
//  Always DWORD, the version
//
//////////////////////////////////////////////////////////////////////////
// 01/09/01  Sean Kennedy -- function created
//////////////////////////////////////////////////////////////////////////
DWORD CStubSetupApp::GetPackageVersion(void)
{
	MSIHANDLE hMSI;
	INSTALLUILEVEL PrevUILevel;
	BOOL	bSilentMode = FALSE;
	DWORD	dwVersion = 0;
	DWORD	dwSize = MAX_PATH;
	CString	szFilename;
	CString	szPackage;
	CString	szVersion;
	CString szTemp;

	// Make sure we have resolved the addresses of the functions we need
	if( (NULL != m_MsiOpenPackage) &&
		(NULL != m_MsiCloseHandle) &&
		(NULL != m_MsiSetInternalUI) &&
		(NULL != m_MsiGetProductProperty) )
	{
		// Check to see if we are running in silent mode and adjust UI
		CString szCmdLine = ::GetCommandLine();
		if( -1 != szCmdLine.Find("/s") )
			bSilentMode = TRUE;
		if(TRUE == bSilentMode)
			PrevUILevel = m_MsiSetInternalUI(INSTALLUILEVEL_NONE, NULL);

		// Open the MSI package so we can query it
		szPackage.LoadString(IDS_MSIFILENAME);
		szFilename.Format("%s\\%s", m_strModuleDir, szPackage);
		UINT uiSuccess = m_MsiOpenPackage((LPCTSTR)szFilename, &hMSI);
		
		// If successful extract and parse the version info
		if(ERROR_SUCCESS == uiSuccess)
		{
			// LPSTR pBuffer = szVersion.GetBuffer(MAX_PATH);
			m_MsiGetProductProperty(hMSI, "ProductVersion", szVersion.GetBuffer(MAX_PATH), &dwSize);
			m_MsiCloseHandle(hMSI);
			szVersion.ReleaseBuffer();

			// Parse the major and minor numbers from the string
			DWORD	dwCount = 0;
			UINT	iStrLen = szVersion.GetLength();
			for(UINT i=0;(i<iStrLen) && (dwCount<3); i++)
			{
				if('.' == szVersion.GetAt(i))
					++dwCount;
				else
					szTemp += szVersion.GetAt(i);
			}

			dwVersion = atoi(szTemp);
			// Adjust for incomplete version numbers ie 7.5 instead of 7.50
			if(dwVersion < 10)
				dwVersion *= 100;
			if(dwVersion < 100)
				dwVersion *= 10;	
		}

		// Reset old UI level if we were running silently
		if(TRUE == bSilentMode)
			m_MsiSetInternalUI(PrevUILevel, NULL);
	}

	return dwVersion;
}


/////////////////////////////////////////////////////////////////////////////
// CStubSetupApp::RegDeleteValue()
BOOL CStubSetupApp::RegDeleteValue( HKEY hMKey, LPCTSTR lpszKey)
{
    DWORD dwTempBuffer;
    TCHAR tszTempBuffer[REGSTR_MAX_VALUE_LENGTH + 1];
    HKEY hKey = NULL;
    LONG Status = ERROR_SUCCESS;
	DWORD dwSubKeyIndex = 0;

    Status = RegOpenKeyEx( hMKey, lpszKey, NULL, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE, &hKey);
    if ( Status == ERROR_SUCCESS )
    {
        // Delete all the values underneath the registry key 
        do
        {
            DWORD dwType;
            TCHAR tszDataBuffer[REGSTR_MAX_VALUE_LENGTH];
            DWORD dwDataBuffer = REGSTR_MAX_VALUE_LENGTH;

            dwTempBuffer = REGSTR_MAX_VALUE_LENGTH;
            Status = RegEnumValue ( hKey, dwSubKeyIndex, tszTempBuffer, &dwTempBuffer,
                            NULL, &dwType, LPBYTE(tszDataBuffer), &dwDataBuffer );
            if ( Status == ERROR_SUCCESS )
            {

                Status = RegDeleteValue( hKey, tszTempBuffer );
                if ( Status != ERROR_SUCCESS )
                {
                    RegCloseKey(hKey);
                    return FALSE;
                }
            }else if ( Status == ERROR_NO_MORE_ITEMS )
            {
                RegCloseKey(hKey);
                return TRUE;
            }
			++dwSubKeyIndex;

        } while ( Status == ERROR_SUCCESS );
		RegCloseKey(hKey);
    }
    return ( Status == ERROR_SUCCESS );

}

/////////////////////////////////////////////////////////////////////////////
// CStubSetupApp::RegCopyValue()
BOOL CStubSetupApp::RegCopyValue( HKEY hMDKey, LPCTSTR lpszSourceKey, HKEY hMSKey, LPCTSTR lpszDestinationKey )
{
    HKEY  hSKey = NULL;
    HKEY  hDKey = NULL;
    DWORD dwSubKeyIndex = 0, dwStringValueBuffer;
    TCHAR tszStringValueBuffer[REGSTR_MAX_VALUE_LENGTH + 1 ];
    LONG lSStatus = ERROR_SUCCESS;
    LONG lDStatus = ERROR_SUCCESS;
    DWORD dwDisposition = 0;

    lDStatus = RegCreateKeyEx ( hMSKey, lpszDestinationKey,
                    NULL, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hDKey, &dwDisposition );
    // Open key for access
    lSStatus = RegOpenKeyEx( hMDKey, lpszSourceKey, NULL, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE, &hSKey);
    if ( lSStatus == ERROR_SUCCESS && lDStatus == ERROR_SUCCESS )
    {
        // Copy the values 
        do
        {
            DWORD dwType;
            TCHAR tszDataBuffer[REGSTR_MAX_VALUE_LENGTH];
            DWORD dwDataBuffer = REGSTR_MAX_VALUE_LENGTH;
			

            dwStringValueBuffer = REGSTR_MAX_VALUE_LENGTH;
            // Enumarate through all the value while capying
            lSStatus = RegEnumValue ( hSKey, dwSubKeyIndex, tszStringValueBuffer, &dwStringValueBuffer,
                            NULL, &dwType, LPBYTE(tszDataBuffer), &dwDataBuffer );
            if ( lSStatus == ERROR_SUCCESS )
            {
                // set the value for this new subkey
                lDStatus = RegSetValueEx( hDKey, tszStringValueBuffer, 0, dwType, LPBYTE(tszDataBuffer), dwDataBuffer );
                if ( lDStatus != ERROR_SUCCESS )
                {
                    RegCloseKey(hSKey);
                    RegCloseKey(hDKey);
                    return FALSE;
                }
            } 
			else if ( lSStatus == ERROR_NO_MORE_ITEMS )
            {
                RegCloseKey(hSKey);
                RegCloseKey(hDKey);
                return TRUE;
            }

            ++dwSubKeyIndex;
        } while ( lSStatus == ERROR_SUCCESS );

        RegCloseKey(hSKey);
        RegCloseKey(hDKey);
    }

    return ( lSStatus == ERROR_SUCCESS && lDStatus == ERROR_SUCCESS );

}

BOOL CStubSetupApp::GetMediaKeyFromEncryptProductCode(HKEY hMKey, CString& MediaHiveKey)
{
    DWORD dwTempBuffer;
    TCHAR tszTempBuffer[REGSTR_MAX_VALUE_LENGTH + 1];
    HKEY hKey = NULL;
	HKEY hSKey = NULL;
    LONG Status = ERROR_SUCCESS;
	DWORD dwKeyIndex = 0;

    Status = RegOpenKeyEx( hMKey, MediaHiveKey, NULL, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE, &hKey);
    if ( Status == ERROR_SUCCESS )
    {
        do
        {
            dwTempBuffer = REGSTR_MAX_VALUE_LENGTH;
            Status = RegEnumKeyEx ( hKey, dwKeyIndex, tszTempBuffer, &dwTempBuffer,
                            NULL, NULL, NULL, NULL );
            if ( Status == ERROR_SUCCESS )
            {
				CString TempMediaHiveKey = MediaHiveKey;
				TempMediaHiveKey += tszTempBuffer;
				Status = RegOpenKeyEx( hMKey, TempMediaHiveKey, NULL, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE, &hSKey);
				if ( Status == ERROR_SUCCESS )
				{
					DWORD dwType;
					TCHAR tszDataBuffer[REGSTR_MAX_VALUE_LENGTH];
					DWORD dwDataBuffer = REGSTR_MAX_VALUE_LENGTH;
					CString ProductName = PRODUCTNAME;
					CString ProductNameString = PRODUCTNAMESTRING;					

					dwTempBuffer = REGSTR_MAX_VALUE_LENGTH;
					LONG ValueStatus = SymSaferRegQueryValueEx ( hSKey, ProductNameString, 0, 
							&dwType, LPBYTE(tszDataBuffer), &dwDataBuffer );
					if ( ValueStatus == ERROR_SUCCESS && ( ProductName == tszDataBuffer ))
					{
						MediaHiveKey = TempMediaHiveKey + MEDIA_KEY_STRING_AFTER_PRODUCTCODE;
						RegCloseKey(hKey);
						RegCloseKey(hSKey);
						return TRUE;
					}

					RegCloseKey(hSKey);
				}

            }

			++dwKeyIndex;

        } while ( Status == ERROR_SUCCESS && Status != ERROR_NO_MORE_ITEMS );
		RegCloseKey(hKey);
    }
    return FALSE;

}

DWORD CStubSetupApp::GetClientType(void)
{
	DWORD	dwVersion	= 0;
	DWORD	dwBuild		= 0;
	HKEY	hVersionKey;

	CString	szKeyName;
	szKeyName.LoadString(IDS_CURRENTVERSIONKEY);
	// Open the key containing version and read the value
	LONG lSuccess = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCSTR)szKeyName, 0, KEY_READ,
								&hVersionKey);
	if(ERROR_SUCCESS == lSuccess)
	{	
		DWORD	dwSize = MAX_PATH;
		DWORD	dwType;
		LPSTR	pBuffer;
		CString	szValue;

		// Read the "ProductVersion" value from the registry
		pBuffer = szValue.GetBuffer(dwSize);
		lSuccess = SymSaferRegQueryValueEx(	hVersionKey,
									_T("ClientType"), 
									NULL,
									&dwType,
									(LPBYTE)pBuffer,
									&dwSize);
		
		// If successful parse out the version and build numbers
		if(ERROR_SUCCESS == lSuccess)
		{
			dwVersion = (DWORD) *pBuffer;
		}
		
		szValue.ReleaseBuffer();

		RegCloseKey(hVersionKey);
	}

	return dwVersion;
}
