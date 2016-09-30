// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SAVServerCA.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <new>
#include <Shlobj.h>
#include <TCHAR.H>
#include <lm.h> 
#include <stdio.h>
#include "SAVServerCA.h"
#include "password.h"
#include "ClientReg.h"
#include "ParseCAD.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "VPExceptionHandling.h"

#define	READ_ACCESS			0x0001
#define	WRITE_ACCESS		0x0002

typedef NET_API_STATUS (_stdcall*NETADD)(LPSTR,DWORD,LPBYTE,LPDWORD);
typedef NET_API_STATUS (_stdcall*NETDEL)(LPSTR,LPSTR,DWORD);
typedef NET_API_STATUS (_stdcall*NETCHECK)(LPWSTR,LPWSTR,LPDWORD);

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))

BOOL WINAPI DllMain(HANDLE /*hInst*/, ULONG ul_reason_for_call, LPVOID /*lpReserved*/)
{
    switch (ul_reason_for_call)
        {
        case  DLL_PROCESS_ATTACH:
	        break;

        case  DLL_THREAD_ATTACH:
            break;

        case  DLL_THREAD_DETACH:
            break;

        case  DLL_PROCESS_DETACH:
            break;

        default:
            break;
        }

    return  (TRUE);

}  // end of "DllMain"

//////////////////////////////////////////////////////////////////////////
//
// Function: executeCommand()
//
// Parameters:
//		CString - Command to be executed, with all parameters.
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function will execute a command as if from the DOS prompt.
//
//////////////////////////////////////////////////////////////////////////
// 11/13/03 - JMEADE  created function to fix 1-1Z3TSG.
//////////////////////////////////////////////////////////////////////////
BOOL executeCommand( TCHAR *commandLine, MSIHANDLE hInstall )
{
	BOOL				bRet				= FALSE;
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	// Set up the process info
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);

	// Execute the command.
	//
	MsiLogMessage( hInstall, "Executing command: " );
	MsiLogMessage( hInstall, commandLine );

    try
    {
        TCHAR *safeCmdLine= new TCHAR[ _tcslen(commandLine) + 3];

        if (! _tcschr( commandLine, _T('\"')))
            wsprintf( safeCmdLine, "\"%s\"", commandLine );
        else
            _tcscpy( safeCmdLine, commandLine );

        if( CreateProcess(  NULL, safeCmdLine, NULL,NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi ) )
        {
            MsiLogMessage( hInstall, "Waiting for uninstall to complete ..." );
            WaitForSingleObject( pi.hProcess, INFINITE );
            MsiLogMessage( hInstall, "Uninstall completed." );
            CloseHandle( pi.hThread );
            CloseHandle( pi.hProcess );
            bRet = TRUE;
        }
        else
        {
            DWORD lastError = 0;
            TCHAR errorCode[MAX_PATH] = {0};

            lastError = GetLastError();
            vpsnprintf( errorCode, sizeof(errorCode), "CreateProcess() failed with last error %d, command line <%s>.", lastError, commandLine );
            MsiLogMessage(hInstall, errorCode);
        }

        delete [] safeCmdLine;
    }
    VP_CATCH_MEMORYEXCEPTIONS(;)

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSIUnInstallAMSServerLegacy()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:
//	This function will uninstall a legacy AMS Server.
//  Prior to SAV 8.0 the AMS Server installation was performed
//  using InstallSheild, and therefore the MSI installation
//  can only perform the uninstall using custom actions.
//
//////////////////////////////////////////////////////////////////////////
// 11/13/03 - JMEADE  created function to fix 1-1Z3TSG.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MSIUnInstallAMSServerLegacy( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;

	MsiLogMessage( hInstall, "Check for legacy AMS server" );

	// Look for the registry key used exclusively by the legacy AMS Server installation.
	//
	HKEY hKey = NULL;
	TCHAR *amsServer = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\AMS Server");
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE, amsServer, 0, KEY_READ, &hKey ) )
	{
		MsiLogMessage( hInstall, "Opened regkey" );
		// A legacy version of AMS Server was found.
		// Get the uninstall string.
		//
		TCHAR   buffer[ MAX_PATH ] = {0};
		DWORD   dwSize = sizeof( buffer );

        if ( ERROR_SUCCESS == SymSaferRegQueryStringValue( hKey, _T("UninstallString"), buffer, &dwSize ) )
		{
			MsiLogMessage( hInstall, "Found UninstallString value" );
			TCHAR msg[ MAX_PATH ] = {0};
            vpsnprintf( msg, sizeof(msg), _T("SymSaferRegQueryStringValue( 'UninstallString' ) returned buffer='%s'"), buffer );
			MsiLogMessage(hInstall, msg );

			// test code find the substring and add the silent uninstall -a for old InstallShield AMS
			TCHAR	*pdest;
			TCHAR   szCommandLine[ MAX_PATH ] = {0};
			TCHAR   szErrorFlags[ MAX_PATH ] = {0};
			const TCHAR   string[] = _T("IsUninst.exe");
            const TCHAR   switches[] = _T(" -a -m ");
			int inLocation = 0; // Location in string that matches 

			pdest = _tcsstr ( buffer, string );

            // Simplistic check to verify expected command present and string is short enough to add switches.
            if ( ( pdest != NULL ) && 
                 ( (_tcslen(buffer) + _tcslen(switches) + 2) < (sizeof(szCommandLine)/sizeof(*szCommandLine) ) ) )
            {
			    inLocation = pdest - buffer;

                // Quote and copy uninstall app name
                _tcscpy(szCommandLine, _T("\""));
                _tcsncat(szCommandLine, buffer, inLocation + _tcslen(string) ); // copy the string up to IsUninst.exe
                _tcscat(szCommandLine, _T("\""));

			    _tcscat(szCommandLine, switches);  // add a -a "silent" & -s suppress command to the uninstall cmd line
			    _tcscat(szCommandLine, &buffer[ inLocation + _tcslen(string) ]); // put the rest of the string back together

                vpsnprintf( szErrorFlags, sizeof(szErrorFlags), _T("MSIUnInstallAMSServerLegacy szCommandLine %s"), szCommandLine);
			    MsiLogMessage (hInstall, szErrorFlags );

			    // Execute the uninstall string.
			    // If we want a failed AMS uninstall to interrupt the new install, then
			    // translate the return value from executeCommand().
			    //
			    executeCommand( szCommandLine, hInstall );
            }
            else
            {
                MsiLogMessage( hInstall, "Unrecognized UninstallString for legacy AMS server." );
            }
		}
		else
		{
			MsiLogMessage( hInstall, "Did not find UninstallString for legacy AMS server." );
		}

		// Cleanup.
		//
		RegCloseKey( hKey );
	}
	else
	{
		MsiLogMessage( hInstall, "Did not find Uninstall key for legacy AMS server." );
	}

	return nRet;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSIFixUNCPATH
//
// Description: We can't create a proper UNC path directly from MSI
//	so this function patches up the string path
//
///////////////////////////////////////////////////////////////////////////
// 9/4/03 DKowalyshyn
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSIFixUNCPATH( MSIHANDLE hInstall)
{

	HKEY hKey;
	TCHAR szUNCPathDir[256] = {0};
	TCHAR szErrorString[MAX_LOG_PATH] = {0};
	TCHAR *pdest;
	int  pos=0;
	int lRegReturn;
	DWORD dwBufLen;
	DWORD dwRet = 0;

	// load the unc path created from the installer in the registry
	lRegReturn = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		SYMANTEC_UNCPATH, // Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion
		0, KEY_ALL_ACCESS, &hKey );

	dwBufLen = sizeof(szUNCPathDir);
	lRegReturn = SymSaferRegQueryValueEx( hKey, "UNCPath", NULL, NULL, (LPBYTE) szUNCPathDir, &dwBufLen);
	// szUNCPathDir should look something like "\\pdr-dan2kvm\c:\program files\symantec antivrus\"
	sssnprintf(szErrorString, sizeof(szErrorString), "MSIFixUNCPATH UNCPath original");
	MsiLogMessage (hInstall, szUNCPathDir);
	
	pdest = strstr( szUNCPathDir, ":" );
	//	find the ':' and it's location
	pos = pdest - szUNCPathDir;

	sssnprintf(szErrorString, sizeof(szErrorString), "MSIFixUNCPATH UNCPath changed");
	MsiLogMessage (hInstall, szUNCPathDir);
	if (pos >0) // if found then change and write it out
	{
		//Change to a proper unc share like "\\pdr-dan2kvm\c$\program files\symantec antivrus\"
		szUNCPathDir[pos]='$';
		if( ERROR_SUCCESS ==  RegSetValueEx (hKey,
			"UNCPath",
			0,
			REG_SZ,
			(LPBYTE)&szUNCPathDir,			// pointer to the value data
			sizeof(szUNCPathDir) ))		// length of value data 
		{
			dwRet = ERROR_SUCCESS;
			MsiLogMessage (hInstall, "MSIFixUNCPATH szUNCPathDir reg update worked properly");
		}
		else
		{
			dwRet =1;
		}
	}
	else
	{
		MsiLogMessage (hInstall, "MSIFixUNCPATH failed");
	}
	RegCloseKey( hKey );

	sssnprintf(szErrorString, sizeof(szErrorString), "MSIFixUNCPATH returncode %li ",dwRet);
	MsiLogMessage (hInstall, szErrorString);

	return (dwRet);
}

///////////////////////////////////////////////////////////////////////////
//	
// Function: MSICreateSAVServerShares
//
// Description: Create the Shares for SavServer
//
///////////////////////////////////////////////////////////////////////////
// 8/28/03 DKowalyshyn
// 2/11/04 John Lenz - Removed VPALERT$ share as per defect 1-2CBNJ0
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSICreateSAVServerShares( MSIHANDLE hInstall )
{
	HKEY hKey;
	char szInstallDir[MAX_PATH] = {0};
	DWORD dwBufLen;
	DWORD dwAccess;
	DWORD dwRet = 0;
	INT len;
	int lRegReturn;
	TCHAR szErrorString[MAX_LOG_PATH] = {0};
	TCHAR szPathString[DOUBLE_MAX_PATH] = {0};
	TCHAR szVPLOGONPathString[DOUBLE_MAX_PATH] = {0};
	TCHAR szVPHOMEPathString[DOUBLE_MAX_PATH] = {0};

	// First, remove any existing shares...
	MSIRemoveSAVServerShares( hInstall );
	MsiLogMessage (hInstall, _T("Removing any existing VP shares."));

	// load the installed path from the registry, should be set by the AMS mergemodule
	RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		SYMANTEC_INSTALLDIR, // Software\\Symantec\\Symantec Antivirus\\Install\\7.50,
		0, KEY_QUERY_VALUE, &hKey );

	dwBufLen = sizeof(szInstallDir);
	
	lRegReturn = SymSaferRegQueryValueEx( hKey, "InstallDir", NULL, NULL, (LPBYTE) szInstallDir, &dwBufLen);
	RegCloseKey( hKey );

	sssnprintf(szErrorString, sizeof(szErrorString), "SymSaferRegQueryValueEx returncode %li ",lRegReturn);
	MsiLogMessage (hInstall, szErrorString);

	MsiLogMessage (hInstall, "szInstallDir regpath");
	MsiLogMessage (hInstall, szInstallDir );
	if ( lstrcmpi(szInstallDir,"")&& ( ERROR_SUCCESS == lRegReturn)) // Catch trash coming back from SymSaferRegQueryValueEx
	{
		// if path exists then use it
		_tcscpy (szPathString,szInstallDir);
		MsiLogMessage (hInstall, "szInstallDir exists");
		MsiLogMessage (hInstall, szPathString );
	}
	else
	{
		// if path doesn't exist then attempt the default.
		_tcscpy (szPathString, SYMANTEC_SHARE_PATH_BACKUP);
		MsiLogMessage (hInstall, "szInstallDir doesn't exists, using default");
		MsiLogMessage (hInstall, szPathString );
	}
	MsiLogMessage (hInstall, "szInstallDir ");
	MsiLogMessage (hInstall, szInstallDir );

	// VPHOME
	_tcscpy (szVPHOMEPathString,szInstallDir);	
	/* Check length */
	len = strlen(szVPHOMEPathString);
	/* Remove trailing backslash */
	if (len && (szVPHOMEPathString[len-1] == '\\')) szVPHOMEPathString[--len] = '\0';
	MsiLogMessage (hInstall, "szVPHOMEPathString path");
	MsiLogMessage (hInstall, szVPHOMEPathString );
	
	//VPLOGON
	_tcscpy (szVPLOGONPathString,szInstallDir);
	/* Check length */
	len = strlen(szVPLOGONPathString);
	/* Remove trailing backslash */
	if (len && (szVPLOGONPathString[len-1] == '\\')) szVPLOGONPathString[--len] = '\0';
	_tcscat (szVPLOGONPathString,"\\logon");
	MsiLogMessage (hInstall, "szVPLOGONPathString path");
	MsiLogMessage (hInstall, szVPLOGONPathString );

	// Add share for our home directory
	dwAccess = READ_ACCESS;
	// need to point to real cli-inst directory
	dwRet = AddNTShare( "VPHOME", szVPHOMEPathString, "Symantec AntiVirus", dwAccess, hInstall );
	sssnprintf(szErrorString, sizeof(szErrorString), "AddNTShare VPHOME error returncode %li ",lRegReturn);
	MsiLogMessage (hInstall, szErrorString );

	dwAccess = READ_ACCESS;
	dwRet = AddNTShare( "VPLOGON", szVPLOGONPathString, "Symantec AntiVirus", dwAccess, hInstall );
	sssnprintf(szErrorString, sizeof(szErrorString), "AddNTShare VPLOGON error returncode %li ",lRegReturn);
	MsiLogMessage (hInstall, szErrorString );

	return ERROR_SUCCESS;
}



/////////////////////////////////////////////////////////////////////////////
//
// Function: AddNTShare()
//
// Description: This function will add a network share on NT.  This code
// was lifted from Intel's Beluga code and modified for Iliad.
//
/////////////////////////////////////////////////////////////////////////////

extern "C" DllExport DWORD __stdcall AddNTShare(LPTSTR szShareName,
									  LPTSTR szSharePath,
									  LPTSTR szShareRemark,
									  DWORD dwAccess,
									  MSIHANDLE hInstall)
{

	MsiLogMessage (hInstall, "AddNTShare values");
	MsiLogMessage (hInstall, "szShareName");
	MsiLogMessage (hInstall, szShareName );
	MsiLogMessage (hInstall, "szSharePath");
	MsiLogMessage (hInstall, szSharePath );
	MsiLogMessage (hInstall, "szShareRemark");
	MsiLogMessage (hInstall, szShareRemark );


	HINSTANCE hLib;
	DWORD dwRet = 0;

	NETADD pNetAddProc;
	SHARE_INFO_502 si;

	LPWSTR wName=0;
	LPWSTR wPath=0;
	LPWSTR wDesc=0;
	DWORD dwRights=0;

	SID_IDENTIFIER_AUTHORITY siaWorldSidA = SECURITY_WORLD_SID_AUTHORITY;
	PSECURITY_DESCRIPTOR pSD = NULL;
	PSID psidWorld = NULL;
	PACL pACLNew = NULL;
	DWORD dwSidW1SubA,cbACL = LBUFFER;

	memset(&si,0,sizeof(SHARE_INFO_502));
	try
	{
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\netapi32.dll"));
		hLib = LoadLibrary(szPath);
		if (!hLib)
			throw(0);

		// Reference Procedure Addresses
		pNetAddProc = (NETADD)GetProcAddress(hLib,"NetShareAdd");

		if (!pNetAddProc)
			throw(0);

		// Add or Delete Share

		// Set Share Name
		if (ConvertToWide(szShareName,wName))
			throw(0);

		// Set Share Path
		if (ConvertToWide(szSharePath,wPath))
			throw(0);

		switch( dwAccess )
		{
		case READ_ACCESS:
			dwRights = FILE_GENERIC_READ | FILE_GENERIC_EXECUTE;
			break;
		case WRITE_ACCESS:
			dwRights = FILE_ALL_ACCESS;
			break;
		case READ_ACCESS | WRITE_ACCESS:
			dwRights = FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | FILE_ALL_ACCESS;
			break;
		default:
			break;
		}


		// Set Share Remark
		if (ConvertToWide(szShareRemark,wDesc))
			throw(0);

		// Set SID Structure
		dwSidW1SubA = GetSidLengthRequired(1);
		psidWorld = (PSID)LocalAlloc(LPTR, dwSidW1SubA);
		if (!psidWorld)
			throw(0);

		if (!InitializeSid(psidWorld, &siaWorldSidA, 1))
			throw(0);

		*(GetSidSubAuthority(psidWorld, 0)) = SECURITY_WORLD_RID;

		pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);  // defined in WINNT.H
		if (!pSD)
			throw(0);

		// Initialize Security Descriptor
		if (!InitializeSecurityDescriptor(pSD,SECURITY_DESCRIPTOR_REVISION))
			throw(0);

		pACLNew = (PACL) LocalAlloc(LPTR, cbACL);
		if (!pACLNew)
			throw(0);

		// Initialize a New ACL.
		if (!InitializeAcl(pACLNew, cbACL, ACL_REVISION2))
			throw(0);

		if (!AddAccessAllowedAce(pACLNew,ACL_REVISION2,dwRights,psidWorld))
			throw(0);

		if (!SetSecurityDescriptorDacl(pSD,TRUE,pACLNew,FALSE))
			throw(0);

		si.shi502_netname=(LPTSTR)wName;
		si.shi502_remark=(LPTSTR)wDesc;
		si.shi502_max_uses= (DWORD)-1;
		si.shi502_path=(LPTSTR)wPath;
		si.shi502_security_descriptor=pSD;

		// Call NetAddShare Function
		(*pNetAddProc)(NULL,502,(LPBYTE)&si,NULL);
		SHChangeNotify(SHCNE_NETSHARE,0,0,0);
	}
	catch (DWORD dwError)
	{
		if (!dwError)
			dwRet = GetLastError();
		else
			dwRet = dwError;
	}

	// Cleanup Dynamic Memory Allocation
	if (pSD) LocalFree((HLOCAL) pSD);
	if (pACLNew) LocalFree((HLOCAL) pACLNew);
	if (psidWorld) LocalFree((HLOCAL) psidWorld);

	if (wName) delete[] wName;
	if (wPath) delete[] wPath;
	if (wDesc) delete[] wDesc;

	if (hLib) FreeLibrary(hLib);

	return dwRet;
}



///////////////////////////////////////////////////////////////////////////
//
// Function: ConvertToWide
//
// Description: Convert string to Unicode
//
///////////////////////////////////////////////////////////////////////////
// 7/7/99 MHOTTA Function lifted from Beluga source.
///////////////////////////////////////////////////////////////////////////
DWORD ConvertToWide(LPCSTR pMultiStr, LPWSTR &pWideStr)
{
	int size;
	//
	//	***delete string allocated by the caller is bad 
	//
	if (pWideStr) 
		delete[] pWideStr;


	size = MultiByteToWideChar(CP_ACP,0,pMultiStr, -1,pWideStr,0);
	//
	//	*** allocate a buffer to return to the caller is  bad  2
	//
	try
	{
		pWideStr = new wchar_t[size];
	}
	catch(std::bad_alloc&)
	{
		return E_OUTOFMEMORY;
	}

	if(!MultiByteToWideChar(CP_ACP,0,pMultiStr, -1,pWideStr,size))
		return GetLastError();

	return 0;
}

///////////////////////////////////////////////////////////////////////////
//	MSI Interface point for  MSICreateSAVServerShares
//
///////////////////////////////////////////////////////////////////////////
// 8/24/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSIRemoveSAVServerShares( MSIHANDLE hInstall)
{
	DWORD dwRet = 0;
	TCHAR szErrorString[MAX_LOG_PATH] = {0};;

	dwRet = RemoveServerShares( hInstall );
	sssnprintf(szErrorString, sizeof(szErrorString), "RemoveServerShares %li ",dwRet);
	MsiLogMessage (hInstall, szErrorString );

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: RemoveServerShares
//
// Description: This function will determine if the current machine is
// sharing predefined folders, and if so, they will get removed.  The
// following shares will be searched:
//
// <TARGETDIR>			"VPHOME"
// <TARGETDIR>\LOGON	"VPLOGON"
// <TARGETDIR>\ALERT	"VPALERT$"
//
///////////////////////////////////////////////////////////////////////////
// 7/7/99 MHOTTA Function Created.
///////////////////////////////////////////////////////////////////////////

extern "C" DllExport DWORD __stdcall RemoveServerShares(MSIHANDLE hInstall)
{
	HINSTANCE hInst;
	NETCHECK pNetCheckProc;
	LPWSTR wTarget;
	TCHAR szTarget[SYM_MAX_PATH] = {0};;
	DWORD dwType = 0, dwRtn = 0;
	wTarget = 0;

	HKEY hKey;
	char szInstallDir[MAX_PATH] = {0};;
	DWORD dwBufLen;
	DWORD dwRet = 0;
	INT len;
	int lRegReturn;
	TCHAR szErrorString[MAX_LOG_PATH] = {0};;
	TCHAR szPathString[DOUBLE_MAX_PATH] = {0};;
	// load the installed path from the registry but this might not be the best answer,
	// we might want to pull the installdir from customactiondata. DJK 8/26/2003
	RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		SYMANTEC_INSTALLDIR, // Software\\Symantec\\Symantec Antivirus\\Install\\7.50,
		0, KEY_QUERY_VALUE, &hKey );

	dwBufLen = sizeof(szInstallDir);
	
	lRegReturn = SymSaferRegQueryValueEx( hKey, "InstallDir", NULL, NULL, (LPBYTE) szInstallDir, &dwBufLen);
	RegCloseKey( hKey );

	sssnprintf(szErrorString, sizeof(szErrorString), "SymSaferRegQueryValueEx returncode %li ",lRegReturn);
	MsiLogMessage (hInstall, szErrorString);

	MsiLogMessage (hInstall, "szInstallDir regpath");
	MsiLogMessage (hInstall, szInstallDir );
	if ( lstrcmpi(szInstallDir,"")&& ( ERROR_SUCCESS == lRegReturn)) // Catch trash coming back from SymSaferRegQueryValueEx
	{
		// if path exists then use it
		_tcscpy (szPathString,szInstallDir);
		MsiLogMessage (hInstall, "szInstallDir exists");
		MsiLogMessage (hInstall, szPathString );
	}
	else
	{
		// if path doesn't exist then attempt the default.
		_tcscpy (szPathString, SYMANTEC_SHARE_PATH_BACKUP);
		MsiLogMessage (hInstall, "szInstallDir doesn't exists, using default");
		MsiLogMessage (hInstall, szPathString );
	}

		/* Check length */
	len = strlen(szPathString);
	/* Remove trailing backslash */
	if (len && (szPathString[len-1] == '\\')) szPathString[--len] = '\0';

	MsiLogMessage (hInstall, "szPathString ");
	MsiLogMessage (hInstall, szPathString );
	lstrcpy( szTarget, szPathString );

	try
	{
		// Get a pointer to the NetShareCheck() function within netapi32.dll
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\netapi32.dll"));
		hInst = LoadLibrary( szPath );

		if( !hInst )
			throw(0);

		pNetCheckProc = (NETCHECK)GetProcAddress( hInst, "NetShareCheck" );

		// first, check and remove the ~\$vphome share.
		CharUpper( szTarget );

		if( ConvertToWide( szTarget, wTarget ) )
			throw(0);

		if( NERR_Success == (*pNetCheckProc)(NULL, wTarget, &dwType) )
			RemoveNTShare( "VPHOME" );
		MsiLogMessage (hInstall, "RemoveNTShare VPHOME, NERR_Success");
		MsiLogMessage (hInstall, NERR_Success );
		// reset the target so we can check and remove the ~\$vphome\logon share.
		wTarget = 0;
		lstrcpy( szTarget, szPathString /*g_szTarget*/ );
		lstrcat( szTarget, "logon" );
		CharUpper( szTarget );

		if( ConvertToWide( szTarget, wTarget ) )
			throw(0);

		if( NERR_Success == (*pNetCheckProc)(NULL, wTarget, &dwType) )
			RemoveNTShare( "VPLOGON" );
		MsiLogMessage (hInstall, "RemoveNTShare VPLOGON, NERR_Success");
		MsiLogMessage (hInstall, NERR_Success );
		// reset the target so we can check and remove the ~\vphome\alert$ share.
		wTarget = 0;
		lstrcpy( szTarget, szPathString /*g_szTarget*/ );
		lstrcat( szTarget, "alert" );
		CharUpper( szTarget );
		
		if( ConvertToWide( szTarget, wTarget ) )
			throw(0);

		if( NERR_Success == (*pNetCheckProc)(NULL, wTarget, &dwType) )
			RemoveNTShare( "VPALERT$" );
		MsiLogMessage (hInstall, "RemoveNTShare VPALERT$, NERR_Success");
		MsiLogMessage (hInstall, NERR_Success );

	}
	catch( DWORD dwError )
	{
		if( !dwError )
			dwRtn = GetLastError();
		else
			dwRtn = dwError;
	}

	sssnprintf(szErrorString, sizeof(szErrorString), "RemoveServerShares error code - dwRtn %li ",dwRet);
	MsiLogMessage (hInstall, szErrorString );

	return( dwRtn );

}

///////////////////////////////////////////////////////////////////////////
//
// Function: RemoveNTShare
//
// Description: Will remove the specified NT share.
//
// IN: Name of the NT share
//
///////////////////////////////////////////////////////////////////////////
// 7/7/99 MHOTTA Function Created.
///////////////////////////////////////////////////////////////////////////

DWORD RemoveNTShare( LPTSTR szShareName )
{
	HINSTANCE hLib;
	DWORD dwRet = 0;
	NETDEL pNetDelProc;
	LPWSTR wName=0;

	try
	{
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\netapi32.dll"));
		hLib = LoadLibrary(szPath);
		if (!hLib)
			throw(0);

		// Reference Procedure Addresses
		pNetDelProc = (NETDEL)GetProcAddress(hLib,"NetShareDel");

		if (!pNetDelProc)
			throw(0);

		if (ConvertToWide(szShareName,wName))
			throw(0);
			
		// Call NetShareDel Function
		(*pNetDelProc)(NULL,(LPTSTR)wName,0);
		// Notify the shell that share information has changed.
		SHChangeNotify(SHCNE_NETUNSHARE,0,0,0);
	}
	catch (DWORD dwError)
	{
		if (!dwError)
			dwRet = GetLastError();
		else
			dwRet = dwError;
	}

	if (wName) delete[] wName;
	if (hLib) FreeLibrary(hLib);

	return dwRet;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSIUpdateRemoteStatus
//
// Description: Will write to a registry to keep Rollout updated 
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/25/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSIUpdateRemoteStatus( MSIHANDLE hInstall)
{
	DWORD cchBuff = 256;		// size of computer or user name 
	BOOL bRet = 0;				// returncode
	TCHAR szRegString[DOUBLE_MAX_PATH] = {0};
	LPTSTR lpszSystemInfo;		// pointer to system information string 
	TCHAR tchBuffer2[DOUBLE_MAX_PATH] = {0};  // buffer for concatenated string 
	TCHAR szCompName[DOUBLE_MAX_PATH] = {0};


	lpszSystemInfo = tchBuffer2; 

	bRet = GetComputerName(szCompName, &cchBuff); 
	lstrcpy (szRegString, szCompName);
	lstrcat (szRegString, "_status");

	MsiLogMessage (hInstall, "MSIUpdateRemoteStatus szRegString" );
	MsiLogMessage (hInstall, szRegString );

	WriteRemoteStatusReg (szRegString,STS_INITIAL_MESSAGE);

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_INITIAL_MESSAGE
//
// Description: Will to registry for MSISTS_INITIAL_MESSAGE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_INITIAL_MESSAGE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 0 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_INITIAL_MESSAGE | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_INITIAL_MESSAGE dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_STARTED_MESSAGE
//
// Description: Will to registry for MSISTS_STARTED_MESSAGE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_STARTED_MESSAGE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 10 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_STARTED_MESSAGE | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_STARTED_MESSAGE dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_COPYING_FILES
//
// Description: Will to registry for MSISTS_COPYING_FILES status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_COPYING_FILES( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 30 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_COPYING_FILES | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_COPYING_FILES dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_UPDATING_REGISTRY
//
// Description: Will to registry for MSISTS_UPDATING_REGISTRY status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_UPDATING_REGISTRY( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 50 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_UPDATING_REGISTRY | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_UPDATING_REGISTRY dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_ADDING_SHARES
//
// Description: Will to registry for MSISTS_ADDING_SHARES status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_ADDING_SHARES( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 60 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_ADDING_SHARES | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_ADDING_SHARES dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_STARTING_SERVICE
//
// Description: Will to registry for MSISTS_STARTING_SERVICE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_STARTING_SERVICE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 70 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_STARTING_SERVICE | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_STARTING_SERVICE dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_UPDATING_AMS2
//
// Description: Will to registry for MSISTS_UPDATING_AMS2 status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 10/02/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_UPDATING_AMS2( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 80 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = STS_UPDATING_AMS2 | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_UPDATING_AMS2 dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_FINISHED_AMS2
//
// Description: Will to registry for MSISTS_FINISHED_AMS2 status
//
///////////////////////////////////////////////////////////////////////////
// 10/06/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_FINISHED_AMS2( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent=0;
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	dwErrorPercent = 100 << PERCENT_SHIFT;  // percentage done and bitshift
	dwErrorFlags = dwErrorFlags = FINISH_TYPE | STS_REBOOT_NECESSARY | dwErrorPercent;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_FINISHED_AMS2 dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall ,dwErrorFlags);
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_FINISHED_MESSAGE
//
// Description: Will to registry for MSISTS_FINISHED_MESSAGE status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_FINISHED_MESSAGE( MSIHANDLE hInstall)
{
	DWORD dwErrorPercent                    = 0;
	DWORD dwErrorFlags                      = 0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH]     = {0};
    TCHAR szRebootPropertyValue[MAX_PATH]   = {""};
    DWORD dwBufferLength                    = 0;
    DWORD returnValDW                       = ERROR_OUT_OF_PAPER;
    bool  rebootRequired                    = false;

	dwErrorPercent = 100 << PERCENT_SHIFT;  // percentage done and bitshift
    // Currently, SymEvent is always updated for SymProtect and thus a reboot is always required for
    // SP to be active (AP is OK until then, however).  However, for PM reasons, on server deployment we install
    // with REBOOT=ReallySuppress, meaning Setup32 does NOT report that a reboot is needed...because
    // we've (server install) been told to ignore it, but it is needed.
    // Resolution is for server installs, see if REBOOT=ReallySuppress - if it is, assume a reboot is needed
    // (safe as it always is at the moment) and set the STS_REBOOT_NECESSARY flag for the Setup32 deployment box.
    // REBOOT must be passed via CustomActionData because this CA is system context
    dwBufferLength = sizeof(szRebootPropertyValue)/sizeof(szRebootPropertyValue[0]);
    returnValDW = MsiGetProperty(hInstall, _T("CustomActionData"), szRebootPropertyValue, &dwBufferLength);
    if (_tcsicmp(szRebootPropertyValue, _T("reallysuppress")) == 0)
    {
        rebootRequired = true;
        MsiLogMessage(hInstall, "SAVServerCA:  Server deployment reporting override - finished with reboot required.");
    }
	// Normal method - Check MSIRUNMODE_REBOOTATEND to see if we need to set the STS_REBOOT_NECESSARY flag
	if ( TRUE == MsiGetMode(
		hInstall,
		MSIRUNMODE_REBOOTATEND))
    {
        MsiLogMessage(hInstall, "SAVServerCA:  Server deployment detected finished with reboot required.");
        rebootRequired = true;
    }
    else
    {
        MsiLogMessage(hInstall, "SAVServerCA:  Server deployment detected finished, no reboot required.");
    }

    if (rebootRequired)
	{
		dwErrorFlags = FINISH_TYPE | STS_REBOOT_NECESSARY | dwErrorPercent;
		sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_REBOOT_NECESSARY dwErrorFlags %li ",dwErrorFlags);
		MsiLogMessage (hInstall, szErrorFlags );
		CreateRemoteReg (hInstall ,dwErrorFlags);
	}
	else
	{
		dwErrorFlags = FINISH_TYPE | STS_FINISHED_MESSAGE | dwErrorPercent;
		sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_FINISHED_MESSAGE dwErrorFlags %li ",dwErrorFlags);
		MsiLogMessage (hInstall, szErrorFlags );
		CreateRemoteReg (hInstall ,dwErrorFlags);
	}
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSI_ERROR_ROLLBACK
//
// Description: Will to registry for MSI_ERROR_ROLLBACK status
// On a failure and MSI Rollback this function will be called and update
// the failure back to rollout. 
//
///////////////////////////////////////////////////////////////////////////
// 8/27/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSI_ERROR_ROLLBACK( MSIHANDLE hInstall)
{
	
	if ( TRUE == MsiGetMode(
		hInstall,
		MSIRUNMODE_ROLLBACK))
	{
		DWORD dwErrorFlags=0;
		TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
		//set the error bit to 1 and indicate that we're done, but have an error.
		dwErrorFlags = FINISH_TYPE | STS_FINISHED_MESSAGE | 0x00000001;
		sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSI_ERROR_ROLLBACK dwErrorFlags %li ",dwErrorFlags);
		MsiLogMessage (hInstall, szErrorFlags );
		CreateRemoteReg (hInstall, dwErrorFlags);
	}
	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSISTS_ERROR_OLDSSC
//
// Description: If we detect a Sever migration over while an old SSC exist
// we'll try to fail and post this message back to Rollout. 
//
///////////////////////////////////////////////////////////////////////////
// 3/01/2005 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSISTS_ERROR_OLDSSC( MSIHANDLE hInstall)
{
	DWORD dwErrorFlags=0;
	TCHAR szErrorFlags[DOUBLE_MAX_PATH] = {0};
	//set the error bit to 1 and indicate that we're done, but have an error.
	dwErrorFlags = FINISH_TYPE | STS_OLDSSC_MIG_ERROR | 0x00000001;
	sssnprintf(szErrorFlags, sizeof(szErrorFlags), "MSISTS_ERROR_OLDSSC dwErrorFlags %li ",dwErrorFlags);
	MsiLogMessage (hInstall, szErrorFlags );
	CreateRemoteReg (hInstall, dwErrorFlags);
	// Setup the RemoteUpgrade\Error for AVServer to read
	SetOldSSCMessage();

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: SetOldSSCMessage
//
// Description: Set a special flag so that AVServer rollout and detect 
//	this error and return a useful code. 
//
///////////////////////////////////////////////////////////////////////////
// 3/19/2005 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
void SetOldSSCMessage()
{
	HKEY hKey;
	TCHAR szBuf[MAX_PATH] ={0}; 
	TCHAR szRegKey_RU[MAX_PATH] ={0}; 
	TCHAR szCompName[MAX_PATH] = {0};
	TCHAR szRegString[MAX_PATH] = {0};
	DWORD cchBuff = MAX_PATH;		// size of computer or user name 

	GetComputerName(szCompName, &cchBuff); 
	lstrcpy (szRegString, szCompName);
	lstrcat (szRegString, "_Error");

	vpsnprintf( szRegKey_RU, sizeof(szRegKey_RU), "%s\\%s\\%s", szReg_Key_Main,szReg_Val_RemoteUpgrade,szReg_Val_Errors);

	DWORD dwVal = STS_OLDSSC_MIG_ERROR;

	if( ERROR_SUCCESS ==  RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,                   
		_T(szRegKey_RU),              
		0,								
		_T(szRegString),				
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		NULL))
	{
		RegSetValueEx( hKey, szRegString, 0, REG_DWORD, (LPBYTE) &dwVal , sizeof(DWORD) );
	}
}

///////////////////////////////////////////////////////////////////////////
//
// Function: CreateRemoteReg
//
// Description: Create the specific registry information to be written
//
// Calling: WriteRemoteStatusReg
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT CreateRemoteReg (MSIHANDLE hInstall, DWORD dwErrorFlags)
{
	DWORD cchBuff = 256;		// size of computer or user name 
	BOOL bRet = 0;				// returncode
	TCHAR szRegString[DOUBLE_MAX_PATH] = {0};
	LPTSTR lpszSystemInfo;		// pointer to system information string 
	TCHAR tchBuffer2[DOUBLE_MAX_PATH] = {0};  // buffer for concatenated string 
	TCHAR szCompName[DOUBLE_MAX_PATH] = {0};
	lpszSystemInfo = tchBuffer2; 

	bRet = GetComputerName(szCompName, &cchBuff); 
	lstrcpy (szRegString, szCompName);
	lstrcat (szRegString, "_status");

	MsiLogMessage (hInstall, "CreateRemoteReg szRegString" );
	MsiLogMessage (hInstall, szRegString );

	WriteRemoteStatusReg (szRegString,dwErrorFlags);

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: WriteRemoteStatusReg
//
// Description: Writing out registry information for Rollout status
//
// 
//
///////////////////////////////////////////////////////////////////////////
// 8/26/2003 DKOWALYSHYN Function Created.
///////////////////////////////////////////////////////////////////////////
UINT WriteRemoteStatusReg (LPTSTR szStatusString, DWORD dwStatus)
{
	DWORD dwRet = 0;
	HKEY hKey;
	TCHAR szClassName[] = {0};

//	wsprintf(szStatusStringVal, "%li/0",dwStatus);

	if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  HKEY_LOCAL_MACHINE,                   // handle to open key
	  REMOTE_UPGRADE_PATH_KEY,              // subkey name
	  0,									// reserved
	  szClassName,									// class string
	  REG_OPTION_NON_VOLATILE,              // special options
	  KEY_ALL_ACCESS,                       // desired security access
	  NULL,									// inheritance
	  &hKey,								// key handle 
	  NULL									// disposition value buffer
	  )){
		if( ERROR_SUCCESS ==  RegSetValueEx (hKey,
			szStatusString,
			0,
			REG_DWORD,
			(LPBYTE)&dwStatus,			// pointer to the value data
			sizeof(dwStatus) ))		// length of value data 
		{
			dwRet = ERROR_SUCCESS;
		}
		else
		{
			dwRet =1;
		}
	}
	RegCloseKey( hKey );
	return dwRet;
}

///////////////////////////////////////////////////////////////////////////
//
// Function: ProcessSettingsINI
//
// Description: Set internal properties based on settings.ini file
//
// Notes: This function should only be excuted in immediate mode
//
///////////////////////////////////////////////////////////////////////////
// 8/29/2003 SKENNED Function Created.
///////////////////////////////////////////////////////////////////////////
UINT __stdcall ProcessSettingsINI( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	TCHAR szFilename[ MAX_PATH ] = {0};
	TCHAR szData[ __max( MAX_PATH,PASS_MAX_CIPHER_TEXT_BYTES)  ] = {0};  // make sure the buffer is big enough
	TCHAR szTemp[ MAX_PATH ] = {0};
    TCHAR szDecryptedPassword[ PASS_MAX_PLAIN_TEXT_BYTES+1 ] = {0};

	try
	{
		// Get the sourcedir from SourceDir property
		lstrcpy( szFilename, _T("SourceDir") );

		if( ERROR_SUCCESS != GetMSIProperty( hInstall, szFilename ) )
			throw( _T("Error getting SourceDir") );
		lstrcat( szFilename, _T("settings.ini") );
		MsiLogMessage( hInstall, szFilename );

		// Verify the file exists
		HANDLE hFile = CreateFile(  szFilename,
									GENERIC_READ,
									NULL,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL );
		if( INVALID_HANDLE_VALUE == hFile )
			throw( _T("settings.ini file does not exist") );
		CloseHandle( hFile );

		// Read the values we need and set the appropriate properties
		GetPrivateProfileString( _T("General"), _T("DomainName"), _T("none"), szData, array_sizeof(szData), szFilename );
		if( _tcsicmp( szData, _T("none") ) )
		{
			MsiSetProperty( hInstall, _T("SERVERGROUPNAME"), szData );
			sssnprintf( szTemp, sizeof(szTemp), "Set SERVERGROUPNAME=%s", szData );
			MsiLogMessage( hInstall, szTemp );
		}
		sssnprintf( szTemp, sizeof(szTemp), "Retrieved %s", szData );
		MsiLogMessage( hInstall, szTemp );

        // Password in the settings file is encrypted.

		GetPrivateProfileString( _T("General"), _T("DomainPassword"), _T("none"), szData, array_sizeof(szData), szFilename );
		if( _tcsicmp( szData, _T("none") ) )
		{
			MsiSetProperty( hInstall, _T("CONSOLEPASSWORD"), szData );
			// We need to overwrite the migrated value so we take the settings.ini over the previous
			MsiSetProperty( hInstall, _T("CONSOLEPASSWORD.2D6B2C77_9DB3_4019_A3E4_3F2892186836"), szData );
			
#ifdef _DEBUG
            // This puts password data in the MSI log ... use only for debugging.
            sssnprintf( szTemp, sizeof(szTemp), "Set CONSOLEPASSWORD=%s", szData );
            MsiLogMessage( hInstall, szTemp );
#endif
            // Also set the decrypted password MSI property.
            UnMakeEP( szData, sizeof(szData), PASS_KEY1, PASS_KEY2, szDecryptedPassword , sizeof(szDecryptedPassword ) );
			MsiSetProperty( hInstall, _T("SERVERGROUPPASS"), szDecryptedPassword );

#ifdef _DEBUG
            // This puts password data in the MSI log ... use only for debugging.
			sssnprintf( szTemp, sizeof(szTemp), "Set SERVERGROUPPASS=%s", szDecryptedPassword );
			MsiLogMessage( hInstall, szTemp );
#endif
		}
		sssnprintf( szTemp, sizeof(szTemp), "Retrieved encrypted CONSOLEPASSWORD" );
		MsiLogMessage( hInstall, szTemp );

		GetPrivateProfileString( _T("General"), _T("Parent"), _T("none"), szData, array_sizeof(szData), szFilename );
		if( _tcsicmp( szData, _T("none") ) )
		{
			MsiSetProperty( hInstall, _T("SERVERPARENT"), szData );
			sssnprintf( szTemp, sizeof(szTemp), "Set SERVERPARENT=%s", szData );
			MsiLogMessage( hInstall, szTemp );
		}
		sssnprintf( szTemp, sizeof(szTemp), "Retrieved %s", szData );
		MsiLogMessage( hInstall, szTemp );

		GetPrivateProfileString( _T("General"), _T("InstallPath"), _T("none"), szData, array_sizeof(szData), szFilename );
		if( _tcsicmp( szData, _T("none") ) )
		{
			MsiSetProperty( hInstall, _T("INSTALLDIR"), szData );
			sssnprintf( szTemp, sizeof(szTemp), "Set INSTALLPATH=%s", szData );
			MsiLogMessage( hInstall, szTemp );
		}
		sssnprintf( szTemp, sizeof(szTemp), "Retrieved %s", szData );
		MsiLogMessage( hInstall, szTemp );
	
		// Grab the Domainusername and hand it off to the installer.
		GetPrivateProfileString( _T("General"), _T("DomainUserName"), _T("none"), szData, array_sizeof(szData), szFilename );
		if( _tcsicmp( szData, _T("none") ) )
		{
			MsiSetProperty( hInstall, _T("SERVERGROUPUSERNAME"), szData );
			sssnprintf( szTemp, sizeof(szTemp), "Set SERVERGROUPUSERNAME=%s", szData );
			MsiLogMessage( hInstall, szTemp );
		}
		sssnprintf( szTemp, sizeof(szTemp), "Retrieved %s", szData );
		MsiLogMessage( hInstall, szTemp );
	}
	catch( TCHAR *szErr )
	{
		MsiLogMessage( hInstall, szErr );
	}

    SecureZeroMemory( szData, sizeof(szData) );
    SecureZeroMemory( szTemp, sizeof(szTemp) );
    SecureZeroMemory( szDecryptedPassword, sizeof(szDecryptedPassword) );

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: UpdateLoginDirFile()
//
// Description: The purpose of this function is to update the
// "vplogon.bat" file for NAVCORP.  This was necessary b/c Install
// Shield's EzBatchxxx functions could not handle extended characters in
// the path properly.
//
// Return Values: 0 if successful.
//                Error value returned by GetLastError() otherwise.
//
//////////////////////////////////////////////////////////////////////////
// 7/27/99 - MHOTTA function created.
// 01/07/2004 - DKOWALYSHYN - resurrected from ZIM, needed for Server 
//////////////////////////////////////////////////////////////////////////
UINT _stdcall UpdateLoginDirFile( MSIHANDLE hInstall )
{

	BOOL bRet = FALSE;
	DWORD cchBuff = 256;		// size of computer or user name 
	LPTSTR lpszSystemInfo;		// pointer to system information string 
	TCHAR tchBuffer2[MAX_PATH] ={0};  // buffer for concatenated string 
	TCHAR szCompName[MAX_PATH] ={0};
	TCHAR szRegString[MAX_PATH] ={0};
	TCHAR szTarget[MAX_PATH] ={0};
	TCHAR szInfo[MAX_PATH] ={0};
	TCHAR szInstallDir[MAX_PATH] ={0};
	HKEY hKey;

	lpszSystemInfo = tchBuffer2; 

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(INSTALLED_APPS_KEY), 0, KEY_QUERY_VALUE, &hKey))
	{
		DWORD   dwSize = sizeof( szInstallDir );
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, _T(SYMANTEC_INSTALL_PATH), NULL, NULL, (LPBYTE)szInstallDir, &dwSize))
		{
			MsiLogMessage (hInstall, "UpdateLoginDirFile Registry read" );
			// get the computername and format the install path & create the FILESERVER info key
			//create the LANG info key e.g. "@SET FILE_SERVER=MySavServerName"
			bRet = GetComputerName(szCompName, &cchBuff);
			_tcscpy (szRegString,"@SET FILE_SERVER=");
			_tcscat (szRegString, szCompName);
			MsiLogMessage (hInstall, "UpdateLoginDirFile szRegString" );
			MsiLogMessage (hInstall, szRegString );

			// create the full VPLOGON.BAT 	e.g. "C:\Program Files\Symantec\logon\vplogon.bat"	
			_tcscat (szInstallDir,VPLOGON_BAT_PATH);
			_tcscpy (szTarget, szInstallDir);
			_tcscpy (szInfo, szRegString);
			MsiLogMessage (hInstall, "UpdateLoginDirFile szTarget" );
			MsiLogMessage (hInstall, szTarget );
			MsiLogMessage (hInstall, "UpdateLoginDirFile szInfo" );
			MsiLogMessage (hInstall, szInfo );
			_tcscat( szInfo, _T("\r\n") );
			DoUpdateLoginDirFile (szTarget, szInfo);

			//create the LANG info key e.g. "@SET LANG=ENU"
			_tcscpy (szInfo,"@SET LANG=");
			_tcscat (szInfo, INTEL_INSTALL_LANGUAGE_CODE);
			MsiLogMessage (hInstall, "UpdateLoginDirFile szTarget" );
			MsiLogMessage (hInstall, szTarget );
			MsiLogMessage (hInstall, "UpdateLoginDirFile szInfo" );
			MsiLogMessage (hInstall, szInfo );
			_tcscat( szInfo, _T("\r\n") );
			DoUpdateLoginDirFile (szTarget, szInfo);
		}
		else
		{
			MsiLogMessage (hInstall, "SymSaferRegQueryValueEx failed" );
		}
		RegCloseKey(hKey);
	}
	else
	{
		MsiLogMessage (hInstall, "RegOpenKeyEx failed" );
	}

	return( ERROR_SUCCESS );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DoUpdateLoginDirFile()
//
// Description: The purpose of this function is to update the
// "vplogon.bat" file for NAVCORP.  This was necessary b/c Install
// Shield's EzBatchxxx functions could not handle extended characters in
// the path properly.
//
// Return Values: 0 if successful.
//                Error value returned by GetLastError() otherwise.
//
//////////////////////////////////////////////////////////////////////////
// 7/27/99 - MHOTTA function created.
// 01/06/2004 - DKOWALYSHYN - resurrected from ZIM, needed for Server 
//////////////////////////////////////////////////////////////////////////
BOOL DoUpdateLoginDirFile( TCHAR* szTarget, TCHAR* szInfo )
{
	HANDLE hFile;
	HANDLE hFileMap;
	DWORD dwFileSize;
	DWORD dwInfoSize;
	DWORD dwRtn = 0;
	TCHAR* lpOffSet;
		// First get a handle to the file.
	hFile = CreateFile( szTarget,
						GENERIC_READ | GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
						NULL );

	if( hFile )
	{
		// Get the size of the file and the size of the string to add.
		dwFileSize = GetFileSize( hFile, NULL );
		dwInfoSize = _tcslen( szInfo );

		// Map the file to memory using the new calculated size to
		// account for the increase when its added.
		hFileMap = CreateFileMapping( hFile,
										NULL,
										PAGE_READWRITE,
										0,
										dwFileSize + dwInfoSize,
										NULL );

		if( NULL != hFileMap )
		{
			// lpOffSet will point to the first TCHAR of the file.
			lpOffSet = (TCHAR*)MapViewOfFile( hFileMap, FILE_MAP_WRITE, 0, 0, 0 );

			if( lpOffSet )
			{
				int nInfoSize = _tcslen( szInfo );

				// Now shift the whole file over to make room for the new string.
				memmove( &lpOffSet[nInfoSize], lpOffSet, lstrlen(lpOffSet) );
				// Copy the new string to the beginning of the file.
				memcpy( &lpOffSet[0], szInfo, nInfoSize );
			}

			UnmapViewOfFile( lpOffSet );
			CloseHandle( hFileMap );
		}
		else
		{
			dwRtn = GetLastError();
		}

		CloseHandle( hFile );
	}
	else
	{
		dwRtn = GetLastError();
	}

	return( ERROR_SUCCESS );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: StreamAMSCustdll()
//
// Description: The purpose of this function is to stream out a newer
// file version of Navcust for migration over 7.5,7.6 versions of AMS server
// they had a bug and on silent uninstall they would popup a dialog box.
//
// Return Values: 0 if successful.
//                Error value returned by GetLastError() otherwise.
//
//////////////////////////////////////////////////////////////////////////
// 05/01/03 Sean Kennedy - Updated for enterprise edition (StreamNewFRE)
// 01/06/2004 - DKOWALYSHYN - Lifted from SCF and modified for AMS Server 
//////////////////////////////////////////////////////////////////////////
UINT _stdcall StreamAMSCustdll( MSIHANDLE hInstall )
{
	BOOL			bRet						= TRUE;
	UINT			nError						= ERROR_SUCCESS;
	UINT			nReturnVal					= 0;
	HANDLE			hFile						= NULL;
	MSIHANDLE		hDatabase					= NULL;
	PMSIHANDLE		hView						= NULL;
	PMSIHANDLE		hRecord						= NULL;
	TCHAR           szViewSQL[90]				= {"SELECT * from Binary WHERE Name='amscust.dll.DLL.F4413C06_9100_4CCA_959E_AAC27B587C0D'"};
	DWORD			dwReturnVal					= 0;
	DWORD			dwFileAttrs					= INVALID_FILE_ATTRIBUTES;
	const TCHAR		szOldFileName[MAX_PATH+1]	= _T("amscust.dll");
	TCHAR			szOldFilePath[MAX_PATH+1]	= {0};
	TCHAR			szTempPath[MAX_PATH+1]		= {0};
	TCHAR			szTempFilePath[MAX_PATH+1]	= {0};
	TCHAR			szTemp[MAX_PATH]			= {0};
	char			*pBuffer					= NULL;
	HKEY			hKey						= NULL;
	bool			bCreatedTempFile			= false;
	bool			bRemovedTempFile			= false;

	if( !hInstall )
	{
		return ERROR_SUCCESS;
	}

	try
	{
		// Set up the MSI database for extracting file
		if( NULL == (hDatabase = MsiGetActiveDatabase( hInstall )) )
			throw( _T("Error getting active database") );

		if( ERROR_SUCCESS != (nError = MsiDatabaseOpenView( hDatabase, szViewSQL, &hView )) )
			throw( _T("Error opening database view") );

		if( ERROR_SUCCESS != (nError = MsiViewExecute( hView, NULL )) )
			throw( _T("Error executing view") );

		if( ERROR_SUCCESS != (nError = MsiViewFetch( hView, &hRecord )) )
			throw( _T("Error fetching view") );

		// Get the size of data in the record
		DWORD dwBuffSize;
		if( ERROR_SUCCESS != (nError = MsiRecordReadStream( hRecord, 2, NULL, &dwBuffSize )) )
			throw( _T("Error determining streamed data size") );

		vpsnprintf( szTemp, sizeof(szTemp), "Reading %d bytes from stream", dwBuffSize);
		MsiLogMessage( hInstall, szTemp );

		pBuffer = new char[ dwBuffSize ];
		if( NULL == pBuffer )
			throw( _T("Error allocating memory") );

		// Copy data into buffer
		if( ERROR_SUCCESS != (nError = MsiRecordReadStream( hRecord, 2, pBuffer, &dwBuffSize )) )
			throw( _T("Error reading stream") );

		// Generate a temporary location and file name for the streamed binary.
		dwReturnVal = GetTempPath(MAX_PATH+1, szTempPath);
		if( !dwReturnVal || (dwReturnVal > MAX_PATH+1) )
		{
			throw( _T("Error creating the temporary path."));
		}

		nReturnVal = GetTempFileName(szTempPath, _T("DLL"), 0, szTempFilePath);
		if( nReturnVal == 0 )
		{
			throw( _T("Error creating the temporary file."));
		}

		// Create the new file and stream data to it
		hFile = CreateFile( szTempFilePath,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);

		if( INVALID_HANDLE_VALUE == hFile )
		{
			nError = ::GetLastError();
			vpsnprintf( szTemp, sizeof(szTemp), "Error creating %s", szTempFilePath );
			throw( szTemp );
		}
		// Write out data to the file
		DWORD dwBytesWritten = 0;
		if( !WriteFile(	hFile,
			pBuffer,
			dwBuffSize,
			&dwBytesWritten,
			NULL ) )
		{
			nError = ::GetLastError();
			vpsnprintf( szTemp, sizeof(szTemp), "Error writing %s", szTempFilePath );
			throw( szTemp );
		}

		if( hFile )
		{
			bCreatedTempFile = true;

			// File opened asynchronous write - closing it so we can copy it.
			CloseHandle( hFile );
			hFile = NULL;
		}

		vpsnprintf( szTemp, sizeof(szTemp), "Wrote %s (%d bytes)", szTempFilePath, dwBytesWritten );
		MsiLogMessage( hInstall, szTemp );

		// Locate the Legacy AMS CA amscust.dll library path from the "AMS Server" key.
		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE, AMS_INSTALLED_APPS_KEY, 0, KEY_READ, &hKey ) )
		{
			MsiLogMessage( hInstall, "Opened regkey" );
			// A legacy version of AMS Server was found.
			// Now lets get the install path
			TCHAR   buffer[ MAX_PATH ]	= {0};
			TCHAR   buffer2[ MAX_PATH ] = {0};
			DWORD	dwType;
			DWORD   dwSize				= sizeof( buffer );

			if ( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey, AMS_INSTALLED_REG_PATH, NULL, &dwType, (LPBYTE)buffer2, &dwSize) )
			{
				int count2=0;
				int length=_tcslen(buffer2); // get the install path length
				for (int count=0;count < length;count++)
				{
					buffer[count2] = buffer2[count];
					if ( buffer2[count] == char(92))  // match a single '\' from reg key
					{
						count2++;
						buffer[count2] = char(92); // if found '\' make it add a second '\'
					}
					count2++;
				}
				vpsnprintf( szOldFilePath, sizeof(szOldFilePath), "%s\\%s", buffer, szOldFileName); // append the filename to the string
				MsiLogMessage( hInstall, "Legacy AMS Path" );
				MsiLogMessage( hInstall, szOldFilePath );

				dwFileAttrs = GetFileAttributes(szOldFilePath);
				if( (dwFileAttrs != INVALID_FILE_ATTRIBUTES) &&
					((dwFileAttrs & FILE_ATTRIBUTE_READONLY) ||
					(dwFileAttrs & FILE_ATTRIBUTE_HIDDEN)) )
				{
					dwFileAttrs &= ~FILE_ATTRIBUTE_READONLY;
					dwFileAttrs &= ~FILE_ATTRIBUTE_HIDDEN;
					if( !SetFileAttributes(szOldFilePath, dwFileAttrs) )
					{
						throw(_T("Error changing the file attributes on amscust.dll."));
					}
				}

				if( !CopyFile(szTempFilePath, szOldFilePath, FALSE) )
				{
					throw( _T("Error replacing amscust.dll."));
				}
			}
		}

		if( DeleteFile(szTempFilePath) ) // Clean up the temp file.
		{
			bRemovedTempFile = true;
		}
		else
		{
			MsiLogMessage( hInstall, "Unable the purge the temporary file." );
		}
	}
	catch( char *szErr )
	{
		TCHAR szTemp[MAX_PATH]	= {0};
		vpsnprintf( szTemp, sizeof(szTemp), "%s (Error Code: %d)", szErr, nError );
		MsiLogMessage( hInstall, szTemp );

		bRet = FALSE;
	}
	catch(std::bad_alloc &)
	{
		bRet = FALSE;
	}

	// Clean up

	if( hFile )
	{
		// Second chance cleanup from an exception.
		CloseHandle( hFile );
		hFile = NULL;
	}

	if( bCreatedTempFile && !bRemovedTempFile )
	{
		// Second chance cleanup from an exception.
		// Check to make sure that the temp file was created (by us) and cleaned up normally.

		dwFileAttrs = INVALID_FILE_ATTRIBUTES;
		dwFileAttrs = GetFileAttributes(szTempFilePath);
		if( dwFileAttrs != INVALID_FILE_ATTRIBUTES )
		{
			// The file was not cleaned up.

			// The first delete may have failed or an exception was thrown.
			if( DeleteFile(szTempFilePath) )
			{
				bRemovedTempFile = true;
			}
			else
			{
				MsiLogMessage( hInstall, "Unable the purge the temporary file." );
			}
		}
	}

	if( hDatabase )
	{
		MsiCloseHandle( hDatabase );
		hDatabase = NULL;
	}

	if( pBuffer )
	{
		delete [] pBuffer;
		pBuffer = NULL;
	}

	return( ERROR_SUCCESS );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: EncryptConsolePass()
//
// Description: The purpose of this function is to encrypt the console 
// password so that it can be written to the regisrty in the server install
// HKLM\Software\INTEL\LANDesk\VirusProtect6\CurrentVersion
// RegSZ: ConsolePassword:
//
// Return Values: 0 if successful.
//                Error value returned by GetLastError() otherwise.
//
//////////////////////////////////////////////////////////////////////////
// 01/22/2004 - DKOWALYSHYN - Similar to ValidateServerGroup in Savcustom 
//////////////////////////////////////////////////////////////////////////

UINT __stdcall EncryptConsolePass( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	TCHAR szPasswordEnc[ PASS_MAX_CIPHER_TEXT_BYTES ] = {0};
	TCHAR szPasswordClr[ PASS_MAX_PLAIN_TEXT_BYTES+1 ] = {0};
	DWORD dwLen = 0;

	//MessageBox( NULL, "Stop here.", "Debug", MB_OK );

	try
	{
		// Get password
		dwLen = sizeof( szPasswordClr );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERGROUPPASS"), szPasswordClr, &dwLen ) )
			throw( "Error getting SERVERGROUPPASS property" );
//	Uncomment only if you need to see the unencrypted password, leave this commented out so we don't log
//		MsiLogMessage( hInstall, "ServerGroupPass before encrypting" );
//		MsiLogMessage( hInstall, szPasswordClr );

		// Validate the password
		if( 0 == lstrlen( szPasswordClr ) )
		{
			// If SERVERGROUPPASS is not defined, default it...
			_tcscpy( szPasswordClr, _T(DEFAULT_DOMAIN_PASSWORD) );
		}

		// Encrypt the password
		MakeEP( szPasswordEnc, sizeof(szPasswordEnc), PASS_KEY1, PASS_KEY2, szPasswordClr , sizeof(szPasswordClr ) );

		MsiLogMessage( hInstall, "ServerGroupPass after encrypting" );
		MsiLogMessage( hInstall, szPasswordEnc );

		MsiSetProperty( hInstall, _T("CONSOLEPASSWORD"), szPasswordEnc );
		MsiLogMessage( hInstall, "Setting CONSOLEPASSWORD after encrypting" );
		nRet = ERROR_SUCCESS;
	}
	catch( TCHAR *szError )
	{
		nRet = ERROR_INVALID_ACCESS;
		MsiLogMessage( hInstall, szError );
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSIRemoveVDB_Dirs()
//
// Description: The purpose of this function is to remove the vdb & xdb
// directories on a SavServer at uninstall time.
//
// Return Values: ERROR_SUCCESS.
//
//////////////////////////////////////////////////////////////////////////
// 01/29/2004 - DKOWALYSHYN 
//////////////////////////////////////////////////////////////////////////

UINT _stdcall MSIRemoveVDB_Dirs( MSIHANDLE hInstall )
{
	HKEY hKey = NULL;
    TCHAR szInstallDir[MAX_PATH] = {0};
	TCHAR szLogMsgBuf[DOUBLE_MAX_PATH] = {0}; 
	DWORD dwBufLen = 0;
	UINT nRet = ERROR_SUCCESS;

	nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
	    SYMANTEC_INSTALLDIR, // Software\\Symantec\\Symantec Antivirus\\Install\\7.50,
	    0, KEY_QUERY_VALUE, &hKey );
	if (nRet == ERROR_SUCCESS)
	{
	    dwBufLen = sizeof(szInstallDir);
	    nRet = SymSaferRegQueryValueEx(hKey, _T(SYMANTEC_INSTALL_DIR), NULL, NULL, (LPBYTE)szInstallDir, &dwBufLen);
	    if (nRet == ERROR_SUCCESS)
	    {
		    MsiLogMessage (hInstall, "szInstallDir regpath");
		    MsiLogMessage (hInstall, szInstallDir );
			if ( lstrcmpi(szInstallDir,"")) // Catch trash coming back from SymSaferRegQueryValueEx
		    {
			    // non-empty path
                ssStrnAppend(szInstallDir,I2_LDVP_VDB_PATH, sizeof(szInstallDir)); // Append the I2_LDVP_VDB_PATH

                MsiLogMessage (hInstall, "VDB Dir is ");
			    MsiLogMessage (hInstall, szInstallDir );
            }
		    else
		    {
			    MsiLogMessage (hInstall, "InstallDir not set.");
			    nRet=ERROR_INVALID_DATA;
		    }
	    }
        else
        {
            MsiLogMessage( hInstall, "Failed to retrieve reg value: " SYMANTEC_INSTALL_DIR);
        }

	    RegCloseKey( hKey );
    }
    else
    {
        MsiLogMessage( hInstall, "Failed to open HKLM reg key: " SYMANTEC_INSTALLDIR);
    }

	if (nRet == ERROR_SUCCESS)
	{
	    // Call DeleteDir to remove the virusdefs
	    sssnprintf( szLogMsgBuf, sizeof(szLogMsgBuf), "Deleting directory %s", szInstallDir);
	    MsiLogMessage( hInstall, szLogMsgBuf );

		// Never let a small path through (e.g. "c:\\") as it could delete an entire drive.
		if ((_tcslen(szInstallDir)) > 4)  
		{
			DeleteDir( hInstall, szInstallDir, "*.*");
		}
		else 
		{
			MsiLogMessage (hInstall, "Illegal path, skipping dir delete");
		}
	}

	return ERROR_SUCCESS;   // Don't fail the uninstall/migration because of this.
}


//////////////////////////////////////////////////////////////////////////
//
// Function: DeleteDir()
//
// Parameters:
//		CString - Folder to delete
//		CString - optional file pattern of files inside
//
// Returns:
//		BOOL - TRUE if folder removed
//
// Description:  
//	Removes a folder including the files inside of it
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
// 1/30/04 - DKOWALYSHYN function de-MFC'ed and modified for my needs
//////////////////////////////////////////////////////////////////////////
BOOL DeleteDir( MSIHANDLE hInstall, TCHAR* strFolder, TCHAR* strPattern )
{
	BOOL bRet = TRUE;
	WIN32_FIND_DATA hFindData;
	TCHAR strAllFiles[ MAX_PATH] = {0};
	TCHAR strSFile[ MAX_PATH] = {0};
	DWORD		dwFileAttrs					= INVALID_FILE_ATTRIBUTES;
	dwFileAttrs &= ~FILE_ATTRIBUTE_READONLY;
	dwFileAttrs &= ~FILE_ATTRIBUTE_HIDDEN;
					
	_tcscpy (strAllFiles,strFolder);
	_tcscat (strAllFiles,"\\");
	_tcscat (strAllFiles,strPattern);
	MsiLogMessage( hInstall, "DeleteDir folder" );
	MsiLogMessage( hInstall, strFolder);
	MsiLogMessage( hInstall, "DeleteDir pattern" );
	MsiLogMessage( hInstall, strPattern);

	HANDLE hFind = FindFirstFile( strAllFiles, &hFindData );
	while( (INVALID_HANDLE_VALUE != hFind) && (bRet) )
	{
		_tcscpy (strSFile,strFolder);
		_tcscat (strSFile,"\\");
		_tcscat (strSFile,hFindData.cFileName);
		if( !(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			MsiLogMessage( hInstall, "DeleteDir file" );
			MsiLogMessage( hInstall, strSFile);

			SetFileAttributes(strSFile, dwFileAttrs);
					
			DeleteFile( strSFile );
		}
		else
		{

			if( lstrcmp( hFindData.cFileName, "." ) && lstrcmp( hFindData.cFileName, ".." ) )
				DeleteDir(  hInstall, strSFile, strPattern );
		}

		bRet = FindNextFile( hFind, &hFindData );
	}
	
    DWORD dwResult = GetLastError();
    if (dwResult == ERROR_NO_MORE_FILES)
    {
        OutputDebugString("No more files to delete.");
    }
    else
    {
			TCHAR szBuf[MAX_PATH] = {0}; 
		    _tprintf(szBuf, "Delete dir GetLastError returned %u\n", dwResult); 
			MsiLogMessage( hInstall, szBuf );
    }
	if( hFind )
		FindClose( hFind );
	bRet = RemoveDirectory( strFolder );

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//
// Function: GetMSIProperty()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//		CString - Name of property to read
//
// Returns:
//		CString - Product code of the MSI
//
// Description:  
//	This function reads the CustomActionData property and returns it
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
// 8/25/03 - DKOWALYSHYN changed.
//////////////////////////////////////////////////////////////////////////
DWORD GetMSIProperty( MSIHANDLE hInstall, TCHAR szProperty[] )
{
	TCHAR szErrorString[MAX_LOG_PATH]={0};

	char *pCAData = NULL;
	DWORD dwLen = 0;	// This needs to be 0 so that we get ERROR_MORE_DATA error
	UINT nRet = ERROR_SUCCESS;
	UINT nErr = ERROR_SUCCESS;

	try
	{
		nErr = MsiGetProperty( hInstall, szProperty, "", &dwLen );
		if( ERROR_MORE_DATA != nErr )
		{
			sssnprintf(szErrorString, sizeof(szErrorString), "Error getting length of %s (nErr = %d)", szProperty, nErr);
			throw( szErrorString );
		}

		// Allocate our memory and grab the data, add one for the trailing null
		pCAData = new char[ ++dwLen * sizeof( TCHAR ) ];
		if( !pCAData )
		{
			sssnprintf(szErrorString, sizeof(szErrorString), "Unable to allocate memory for data buffer %s", szErrorString);
			throw( szErrorString );
		}

		nErr = MsiGetProperty( hInstall, szProperty, pCAData, &dwLen );
		if( ERROR_SUCCESS != nErr )
		{
			sssnprintf( szErrorString, sizeof(szErrorString), "Unable to allocate memory for data buffer %s (nErr = %d)", szProperty, nErr );
			throw( szErrorString );
		}

		// Log the value to the MSI log file
		sssnprintf( szErrorString, sizeof(szErrorString), "%s=%s", szProperty, pCAData );
		MsiLogMessage( hInstall, szErrorString );
		_tcscpy( szProperty, pCAData );
	}
	catch( TCHAR *szErr )
	{
		nRet = ERROR_FUNCTION_FAILED;
		MsiLogMessage( hInstall, szErr);
	}
	catch(std::bad_alloc&)
	{
		nRet = ERROR_FUNCTION_FAILED;
	}

	if( pCAData )
		delete [] pCAData;

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSIBackupServerGroupsDir()
//
// Description: The purpose of this function is too backup the 
// SavServer groups directory structure.
//
// Return Values: ERROR_SUCCESS.
//
//////////////////////////////////////////////////////////////////////////
// 02/12/2004 - DKOWALYSHYN 
//////////////////////////////////////////////////////////////////////////

UINT _stdcall MSIBackupServerGroupsDir( MSIHANDLE hInstall )
{
	HKEY hKey=0;
	int retcode =0;
	TCHAR szErrorString[MAX_LOG_PATH]={0};
	TCHAR strSource[MAX_PATH]={0};
	TCHAR strDest[MAX_PATH]={0};
	TCHAR szInstallDir[MAX_PATH]={0};

	// load the installed path from the registry
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(INSTALLED_APPS_KEY), 0, KEY_QUERY_VALUE, &hKey))
	{
		DWORD   dwSize = sizeof( szInstallDir );
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, _T(SYMANTEC_INSTALL_PATH), NULL, NULL, (LPBYTE)szInstallDir, &dwSize))
		{
			MsiLogMessage (hInstall, "SYMANTEC_INSTALL_PATH szInstallDir regpath");
			MsiLogMessage (hInstall, szInstallDir );
		}
		else if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, _T(NAV_INSTALL_PATH), NULL, NULL, (LPBYTE)szInstallDir, &dwSize))
		{
			_tcscat (szInstallDir, _T("\\")); // fix the problem that 8.1 doesn't have trailing slash while 9 does
			MsiLogMessage (hInstall, "NAV_INSTALL_PATH szInstallDir regpath");
			MsiLogMessage (hInstall, szInstallDir );
		}

		if ( _tcslen(szInstallDir) > 4 )
		{
			_tcscpy (strSource,szInstallDir);
			_tcscat (strSource, _T("groups"));

			GetTempPath( MAX_PATH, strDest );
			_tcscat (strDest, _T("groups"));

			retcode = CopyDirRecursive ( hInstall, strSource, strDest );
			sssnprintf( szErrorString, sizeof(szErrorString), "MSIBackupServerGroupsDir: CopyDirRecursive from %s to %s retcode %d", strSource, strDest, retcode );
			MsiLogMessage( hInstall, szErrorString );
		}
		RegCloseKey (hKey);
	}
	else
	{
		MsiLogMessage( hInstall, _T("Unable open INSTALLED_APPS_KEY") );
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSIBackupServerGroupsDir()
//
// Description: The purpose of this function is too backup the 
// SavServer groups directory structure.
//
// Return Values: ERROR_SUCCESS.
//
//////////////////////////////////////////////////////////////////////////
// 02/12/2004 - DKOWALYSHYN 
//////////////////////////////////////////////////////////////////////////

UINT _stdcall MSIRestoreServerGroupsDir( MSIHANDLE hInstall )
{
	HKEY hKey=0;
	int retcode =0;
	TCHAR szErrorString[MAX_LOG_PATH]={0};
	TCHAR strSource[MAX_PATH]={0};
	TCHAR strDest[MAX_PATH]={0};
	TCHAR szInstallDir[MAX_PATH]={0};

	// load the installed path from the registry
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(INSTALLED_APPS_KEY), 0, KEY_QUERY_VALUE, &hKey))
	{
		DWORD   dwSize = sizeof( szInstallDir );
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, _T(SYMANTEC_INSTALL_PATH), NULL, NULL, (LPBYTE)szInstallDir, &dwSize))
		{
			MsiLogMessage (hInstall, "SYMANTEC_INSTALL_PATH szInstallDir regpath");
			MsiLogMessage (hInstall, szInstallDir );
			if ( _tcslen(szInstallDir) > 4 )
			{
				_tcscpy (strDest,szInstallDir);
				_tcscat (strDest, _T("groups"));

				GetTempPath( MAX_PATH, strSource );
				_tcscat (strSource, _T("groups"));

				retcode = CopyDirRecursive ( hInstall, strSource, strDest );
				sssnprintf( szErrorString, sizeof(szErrorString), "MSIRestoreServerGroupsDir: CopyDirRecursive from %s to %s retcode %d", strSource, strDest, retcode );
				MsiLogMessage( hInstall, szErrorString );

				sssnprintf( szErrorString, sizeof(szErrorString), "MSIRestoreServerGroupsDir: DeleteDir temp backup dir %s", strSource );
				MsiLogMessage( hInstall, szErrorString );
				DeleteDir( hInstall, strSource, "*.*");
			}
		}
		else
		{
			MsiLogMessage( hInstall, _T("Unable to open SYMANTEC_INSTALL_PATH") );
		}

		RegCloseKey (hKey);
	}
	else
	{
		MsiLogMessage( hInstall, _T("Unable to open INSTALLED_APPS_KEY") );
	}

	return ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////
//
// Function: CopyDirRecursive()
//
// Parameters:
//		TCHAR** - Source folder
//		CString - Destination folder
//
// Returns:
//		int - number of files copied, -1 if error	
//
// Description:  
//	Copies all files from source to destination
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
// 2/11/03 - Dan Kowlayshyn Took out the MFC and made it work recursively
//	for nested directories.
//////////////////////////////////////////////////////////////////////////

int CopyDirRecursive( MSIHANDLE hInstall, TCHAR* strSource, TCHAR* strDest )
{
	TCHAR szErrorString[MAX_LOG_PATH]={0};
	int nRet = 0;
	BOOL bRet = TRUE;
	WIN32_FIND_DATA hFindData;
	TCHAR strDFile[ MAX_PATH ] ={0};
	TCHAR strSFile[ MAX_PATH ] ={0};
	TCHAR strNextDirDest[ MAX_PATH ] ={0};
	TCHAR strNextDirSrc[ MAX_PATH ] ={0};
	TCHAR strAllFiles[ MAX_PATH ] ={0};

	_tcscpy (strAllFiles, strSource);
	_tcscat (strAllFiles, _T("\\*.*"));

	HANDLE hFind = FindFirstFile( strAllFiles, &hFindData );
	while( (INVALID_HANDLE_VALUE != hFind) && (bRet) )
	{
		if( !(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			_tcscpy ( strDFile,strDest);
			_tcscat ( strDFile,_T("\\"));
			_tcscat ( strDFile,hFindData.cFileName);
			_tcscpy ( strSFile,strSource);
			_tcscat ( strSFile,_T("\\"));
			_tcscat ( strSFile,hFindData.cFileName);
			if( CopyFile( strSFile, strDFile, FALSE ) )
				++nRet;
			sssnprintf( szErrorString, sizeof(szErrorString), "CopyFile from %s to %s - Retcode: %d", strSFile, strDFile, GetLastError() );
			MsiLogMessage( hInstall, szErrorString );
		}
		else
		{
			if ( 2>=(_tcslen(hFindData.cFileName)))
			{
				CreateDirectory( strDest, NULL );
				sssnprintf( szErrorString, sizeof(szErrorString), "CreateDirectory %s", strDest );
				MsiLogMessage( hInstall, szErrorString );
			}
			else
			{
				_tcscpy (strNextDirDest, strDest);
				_tcscat (strNextDirDest,_T("\\"));
				_tcscat (strNextDirDest,hFindData.cFileName);
				CreateDirectory( strNextDirDest, NULL );
				sssnprintf( szErrorString, sizeof(szErrorString), "CreateDirectory %s", strNextDirDest );
				MsiLogMessage( hInstall, szErrorString );
				_tcscpy (strNextDirSrc, strSource);
				_tcscat (strNextDirSrc,_T("\\"));
				_tcscat (strNextDirSrc,hFindData.cFileName);
				// subdirectory detected so recursively call copydir
				sssnprintf( szErrorString, sizeof(szErrorString), "Call CopyDirRecursive %s %s", strNextDirSrc, strNextDirDest);
				MsiLogMessage( hInstall, szErrorString );
				CopyDirRecursive( hInstall, strNextDirSrc, strNextDirDest ); 
			}
		}
		bRet = FindNextFile( hFind, &hFindData );
	}
	if( hFind )
		FindClose( hFind );
	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: LogMessage()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//		TCHAR [] - Text to log
//
// Returns:
//		NONE
//
// Description:  
//	This function logs messages to the MSI log file
//
//////////////////////////////////////////////////////////////////////////
// 8/07/01 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
UINT MsiLogMessage( MSIHANDLE hInstall, TCHAR *szString )
{
	UINT	uiRetVal = ERROR_SUCCESS;

	MSIHANDLE hRec = MsiCreateRecord( 1 );
	if( hRec )
	{
	    TCHAR	szLogLine[MAX_LOG_PATH] = {0};

		lstrcpy( szLogLine, "SAVServerCA: " );
		lstrcat( szLogLine, szString );

		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 0, szLogLine );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
		MsiCloseHandle( hRec );
	}

	return uiRetVal;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: FixupAMSMigration()
//
// Description: This function removes AMS specific entries from the
// PendingFileRenameOperations registry key so that AMS file in use
// during migration do not get deleted.
//
// IsAMSFile is a helper function.
//
// Return Values: ERROR_SUCCESS always.
//
//////////////////////////////////////////////////////////////////////////
// 01/11/2004 - Thomas Brock - Function created
//////////////////////////////////////////////////////////////////////////

#define NUM_FILES 42

BOOL IsAMSFile( char * pstr )
{
	char * pszFiles[NUM_FILES] = {	"AMS.DLL",
									"AMS.DL_",
									"AMSLIB.DLL",
									"AMSLIB.DL_",
									"AMSUI.DLL",
									"AMSUI.DL_",
									"BCSTHCFG.DLL",
									"BCSTHCFG.DL_",
									"CBA.DLL",
									"CBA.DL_",
									"CBAXFR.DLL",
									"CBAXFR.DL_",
									"ENUAMS.LRC",
									"ENUAMS.LR_",
									"ENUAMSUI.LRC",
									"ENUAMSUI.LR_",
									"ITMLHCFG.DLL",
									"ITMLHCFG.DL_",
									"LOC32VC0.DLL",
									"LOC32VC0.DL_",
									"MSBXHCFG.DLL",
									"MSBXHCFG.DL_",
									"MSGSYS.DLL",
									"MSGSYS.DL_",
									"MSGSYS.EXE",
									"MSGSYS.EX_",
									"NLMXHCFG.DLL",
									"NLMXHCFG.DL_",
									"NTELHCFG.DLL",
									"NTELHCFG.DL_",
									"NTS.DLL",
									"NTS.DL_",
									"NTSU2T.DLL",
									"NTSU2T.DL_",
									"PAGEHCFG.DLL",
									"PAGEHCFG.DL_",
									"PDS.DLL",
									"PDS.DL_",
									"PRGXHCFG.DLL",
									"PRGXHCFG.DL_",
									"SNMPHCFG.DLL",
									"SNMPHCFG.DL_"};

	char strTemp[MAX_PATH];

	strcpy( strTemp, pstr );
	CharUpperBuff( strTemp, lstrlen(strTemp) );

	for( int i = 0; i < NUM_FILES; i++ )
	{
		if( strstr( pstr, pszFiles[i] ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

UINT _stdcall FixupAMSMigration( MSIHANDLE /*hInstall*/ )
{
	HKEY hKey;
	LONG lRetval;
	BOOL bOutput;
	DWORD dwType, dwSize, dwPosIn, dwPosOut, dwPosStr;
	char * pBuffIn = NULL;
	char * pBuffOut = NULL;
	char strTemp1[MAX_PATH];
	char strTemp2[MAX_PATH];

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager"), NULL, KEY_ALL_ACCESS, &hKey ) )
	{
		dwSize = 0;
		lRetval = SymSaferRegQueryValueEx( hKey, _T("PendingFileRenameOperations"), NULL, &dwType, NULL, &dwSize );
		if( ( !lRetval ) && dwSize )
		{
			try
			{
				pBuffIn = new char[ dwSize ];
				pBuffOut = new char[ dwSize ];
			}
			catch (std::bad_alloc &) {}
			if( pBuffIn && pBuffOut )
			{
				pBuffIn = new char[ dwSize ];
				pBuffOut = new char[ dwSize ];

				if( pBuffIn && pBuffOut )
				{
					lRetval = SymSaferRegQueryValueEx( hKey, _T("PendingFileRenameOperations"), NULL, &dwType, (LPBYTE) pBuffIn, &dwSize );
					if( ( ERROR_SUCCESS == lRetval ) && ( dwType == REG_MULTI_SZ ) )
					{
						dwPosIn = 0;
						dwPosOut = 0;
						while( pBuffIn[ dwPosIn ] )
						{
							bOutput = FALSE;

							// Get the first string of the pair...
							dwPosStr = 0;
							do
							{
								strTemp1[ dwPosStr++ ] = pBuffIn[ dwPosIn++ ];
							}
							while( strTemp1[ dwPosStr - 1 ] );

							// Get the second string of the pair...
							dwPosStr = 0;
							do
							{
								strTemp2[ dwPosStr++ ] = pBuffIn[ dwPosIn++ ];
							}
							while( strTemp2[ dwPosStr - 1 ] );

							// If the second string is empty, this is a delete...
							if( ! strTemp2[0] )
							{
								// This is a delete...
								if( ! IsAMSFile( strTemp1 ) )
								{
									bOutput = TRUE;
								}
							}
							else
							{
								// This is a rename...
								bOutput = TRUE;
							}

							if( bOutput )
							{
								// Write the first string of the pair...
								dwPosStr = 0;
								do
								{
									pBuffOut[ dwPosOut++ ] = strTemp1[ dwPosStr++ ];
								}
								while( pBuffOut[ dwPosOut - 1 ] );

								// Write the second string of the pair...
								dwPosStr = 0;
								do
								{
									pBuffOut[ dwPosOut++ ] = strTemp2[ dwPosStr++ ];
								}
								while( pBuffOut[ dwPosOut - 1 ] );
							}
						}

						// Did we write anything to the output buffer?
						if( dwPosOut )
						{
							// Add final null...
							pBuffOut[ dwPosOut++ ] = NULL;
							RegSetValueEx( hKey, _T("PendingFileRenameOperations"), NULL, REG_MULTI_SZ, (CONST BYTE *) pBuffOut, dwPosOut );
						}
						else
						{
							RegDeleteValue( hKey, _T("PendingFileRenameOperations") );
						}
					}
				}
			}
			if( pBuffIn ) delete[] pBuffIn;
			if( pBuffOut ) delete[] pBuffOut;
		}
	}

	return ERROR_SUCCESS;//?
}

// Use this function to debug this DLL using Rundll32.exe
void CALLBACK DebugEntryPoint(
  HWND /*hwnd*/,        // handle to owner window
  HINSTANCE /*hinst*/,  // instance handle for the DLL
  LPTSTR /*lpCmdLine*/, // string the DLL will parse
  int /*nCmdShow*/      // show state
)
{
	FixupAMSMigration( NULL );
}

///////////////////////////////////////////////////////////////////////////
//
// Function: MSICreateDomainGuid
//
// Description: This function is called to create a domain guid in a 
// server install to identify the domain.
//
// * NOTE - The string representation of the UUID that gets MSI logged looks
//	totally different from what actually is written to the reg. You need to
//  us UuidFromString to convert the reg to something that resembles a GUID.
//
///////////////////////////////////////////////////////////////////////////
// 8/19/04 Dan Kowalyshyn
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSICreateDomainGuid ( MSIHANDLE hInstall )
{
    DWORD               nRtn = ERROR_SUCCESS;
	TCHAR				szLogMessage[DOUBLE_MAX_PATH] = {0};
	GUID			    Guid;
	TCHAR		        *pszGuid;
	HKEY				hKey;
	BOOL				bFailOver = FALSE;
    TCHAR               strPrimaryServerName [MAX_PATH+1] = {0};

    // Get our custom action data.

	DWORD	dwLen			= 0;
	TCHAR	*multipath		= NULL;

	// Get the size first

	if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &dwLen ) )
	{
		sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid: MsiGetProperty failed" );
		MsiLogMessage(hInstall, szLogMessage);
        return ERROR_INSTALL_FAILURE;
    }
	else
	{
		sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid: CustomActionData length = %lu.", dwLen);
		MsiLogMessage(hInstall, szLogMessage);

        if( dwLen > 0 )
        {
            try
            {
		        multipath = new TCHAR [ dwLen ];  // Allocate more room for parsing
            }
            catch( std::bad_alloc& )
            {
		        MsiLogMessage(hInstall, "MSICreateDomainGuid: memory allocation for CAD failed" );
                return ERROR_INSTALL_FAILURE;
            }

		    if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("CustomActionData"), multipath, &dwLen ) )
		    {
			    ParseCAD ( hInstall, 
				           multipath,
                           1,
				           strPrimaryServerName,
                           array_sizeof(strPrimaryServerName) );
		    }
		    else
		    {
			    MsiLogMessage(hInstall, "MSIServerPKIStandAlone: MsiGetProperty failed.");
                return ERROR_INSTALL_FAILURE;
		    }

            if (multipath != NULL)
                delete [] multipath;
        }
        else
        {
            strPrimaryServerName[0] = 0;
        }
	}

    // If we are joining an existing Server Group, then we must use that DomainGUID.
    // If we are going it alone, then we have to generate a new DomainGUID.

    if (_tcslen(strPrimaryServerName) > 0)
    {
        TCHAR szTemp [MAX_PATH+1] = {0};
        HKEY  hKey = NULL;

        sssnprintf (szTemp, sizeof(szTemp), "%s\\%s\\%s", _T(szReg_Key_Main), _T(szReg_Key_AddressCache), strPrimaryServerName);

        if( (nRtn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szTemp, 0, KEY_READ, &hKey)) == ERROR_SUCCESS )
        {
            DWORD size = sizeof (Guid);

		    if( (nRtn = SymSaferRegQueryValueEx(hKey,_T(szReg_Val_DomainGUID),0,NULL,reinterpret_cast<BYTE*>(&Guid),&size)) != ERROR_SUCCESS )
            {
		        sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid: Failed to get GUID from %s AddressCache: %lu.", strPrimaryServerName, nRtn );
		        MsiLogMessage( hInstall, szLogMessage );
				bFailOver = TRUE; // Primary may be unreachable, attempt Failover standalone install
            }
            else
            {
		        sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid: Got GUID from %s AddressCache entry.", strPrimaryServerName );
		        MsiLogMessage( hInstall, szLogMessage );
            }

            RegCloseKey (hKey);
        }
        else
        {
		    sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid: Failed to open %s AddressCache entry: %lu.", strPrimaryServerName, nRtn );
		    MsiLogMessage( hInstall, szLogMessage );
			bFailOver = TRUE; // Primary may be unreachable, attempt Failover standalone install
        }
    }
	// Either no Primary Server found or Domainguid regread failed so Failover to standalone install
    if ( (_tcslen(strPrimaryServerName) == 0) || (TRUE == bFailOver) )
    {
	    UuidCreate( &Guid );	// Create Guid
		strPrimaryServerName[0] = 0;
		nRtn = ERROR_SUCCESS;
	    sssnprintf( szLogMessage, sizeof(szLogMessage), "Either no Primary Server found or Domainguid regread failed so Failover to standalone install" );
	    MsiLogMessage( hInstall, szLogMessage );
    }

    // Write the GUID we got to our DomainGUID reg value.

    if( nRtn == ERROR_SUCCESS )
    {
        // Get a string representation for logging.

	    if (RPC_S_OK != UuidToString( &Guid, reinterpret_cast<unsigned char**>(&pszGuid) ))
	    {
		    sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid: UuidToString failed: %lu.", GetLastError() );
		    MsiLogMessage( hInstall, szLogMessage );
            pszGuid = NULL;
	    }
        else
        {
		    sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid: UuidToString succeeded: %s.", pszGuid );
		    MsiLogMessage( hInstall, szLogMessage );
        }

	    if( (nRtn = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		                          _T(szReg_Key_Main),	// szReg_Key_Main reg - "Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion"
		                          0,
		                          KEY_WRITE,				
		                          &hKey )) == ERROR_SUCCESS )
	    {

		    if( (nRtn = RegSetValueEx( hKey,			            // set reg key value with UUID
			                           szReg_Val_DomainGUID,	    // DomainGUID reg key - "DomainGUID"
			                           0,
			                           REG_BINARY,				    // REG_BINARY type
			                           (LPBYTE)&Guid,			    // pointer to the Guid data
			                           sizeof(Guid) )) == ERROR_SUCCESS )			// length of Guid data 
		    {
                sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid reg set DomainGUID success :%s ", (pszGuid == NULL ? "" : pszGuid) );
			    MsiLogMessage ( hInstall, szLogMessage );
		    }
		    else
		    {
			    sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid reg set DomainGUID failed: %s: %lu", (pszGuid == NULL ? "" : pszGuid), nRtn );
			    MsiLogMessage  (hInstall, szLogMessage );
		    }
	    }
	    else
	    {
		    sssnprintf( szLogMessage, sizeof(szLogMessage), "MSICreateDomainGuid RegOpenKeyEx (%s) failed: %lu", szReg_Key_Main, nRtn );
		    MsiLogMessage( hInstall, szLogMessage );
	    }

        if (pszGuid != NULL)
            RpcStringFree( reinterpret_cast<unsigned char**>(&pszGuid) );
    }

    return (nRtn == ERROR_SUCCESS ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE);
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SAVServerCleanup
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS reguardless of anything that happens
//		in the function.  This function should only be called when
//		uninstalling, and we would not want to rollback the uninstall
//		if a quarantine file could not be deleted.
//
// Description:
//		Deletes orphaned directories under the install folder
//	    1-3OX10V has these directories and files orphaned after uninstall:
//
//[Directory] CLT-INST
//[Directory] I2_LDVP.VDB
//[Directory] Logon (shared directory).
//grc.dat
//grcgrp.dat
//////////////////////////////////////////////////////////////////////////
// 03/03/05 - Michael C. Lee - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SAVServerCleanup( MSIHANDLE hInstall )
{
	DWORD dwResult = 0;
	HKEY hKey = NULL;
	DWORD dwRegOptions = KEY_READ | KEY_SET_VALUE;
	TCHAR szInstallDir[1024];
	TCHAR szInstallDirWithoutSlash[1024];
	DWORD dwSize = 1024;

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = SymSaferRegQueryValueEx(hKey, _T("SAVHomeDirectory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegDeleteValue( hKey, _T("SAVHomeDirectory") );
		RegCloseKey(hKey);
	
		if (dwResult == ERROR_SUCCESS)
		{
			_tcscpy(szInstallDirWithoutSlash, szInstallDir); 
			szInstallDirWithoutSlash[_tcslen(szInstallDirWithoutSlash) - 1] = '\0'; // get rid of \\ 
			DeleteDir( hInstall, szInstallDirWithoutSlash, _T("*.*") );
		}
	}
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: TempSaveSAVHomeDirectory
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS reguardless of anything that happens
//		in the function.  This function should only be called when
//		uninstalling, and we would not want to rollback the uninstall
//		if a quarantine file could not be deleted.
//
// Description:
//	    Saves Home Directory during uninstall before registry is blown away
//
//////////////////////////////////////////////////////////////////////////
// 03/12/05 - Michael C. Lee - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall TempSaveSAVHomeDirectory( MSIHANDLE hInstall )
{
	DWORD dwResult = 0;
	HKEY hKey = NULL;
	DWORD dwRegOptions = KEY_READ;
	TCHAR szInstallDir[1024];
	DWORD dwSize = 1024;

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = SymSaferRegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegCloseKey(hKey);
		if (dwResult != ERROR_SUCCESS)
		{
			dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\DomainData"), NULL, dwRegOptions, &hKey);
			if (dwResult == ERROR_SUCCESS)
			{
				dwResult = SymSaferRegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
				RegCloseKey(hKey);
			}
		}
		if (dwResult == ERROR_SUCCESS)
		{
			DWORD dwRet = 0;
			HKEY hKey;
			TCHAR szClassName[] = {0};
			DWORD dwStatus = 0;

			if( ERROR_SUCCESS ==  RegCreateKeyEx(
	  					HKEY_LOCAL_MACHINE,                   // handle to open key
	  					_T("Software\\Symantec"), 	      // subkey name
	  					0,									// reserved
	  					szClassName,									// class string
	  					REG_OPTION_NON_VOLATILE,              // special options
	  					KEY_ALL_ACCESS,                       // desired security access
	  					NULL,									// inheritance
	  					&hKey,								// key handle 
	  					NULL									// disposition value buffer
	  					))
			{
				RegSetValueEx (hKey,
						_T("SAVHomeDirectory"),
						0,
						REG_SZ,
						(LPBYTE)szInstallDir,			// pointer to the value data
						_tcslen(szInstallDir) + 1);		// length of value data 
				
				RegCloseKey( hKey );
	
			}
		}
				
	}
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiClearAddressCache()
//
// Description: This function completely wipes out any exist addresscache
//	and should only be run at the end of the install. This fixes a minor
//	problem where SSC could read the only AC if you hadn't rediscovered.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 3/20/2005 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiClearAddressCache( MSIHANDLE hInstall )
{
    DWORD dwResult;
	MsiLogMessage( hInstall, "MsiClearAddressCache - deleting AddressCache" );
	dwResult = RegKeyDeleteAll(HKEY_LOCAL_MACHINE, _T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache"));
	// 1-576IMZ - SSC: Cannot unlock the server group if reinstall SCS server in the same box as the SSC
	// The Accounts hive and specifically the (Default) value "admin" is messing witht he RTVSCAN password upconvert function
	// We need to wipe out this hive at uninstall time, but not migration.
	MsiLogMessage( hInstall, "MsiClearAddressCache - deleting Accounts" );
	dwResult = RegKeyDeleteAll(HKEY_LOCAL_MACHINE, _T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\Accounts"));

	return ERROR_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
//
// Function: RegKeyDeleteAll()
//
// Description: This function was lifted out of savcustom but it should
// really be in a shared library somewhere.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 3/20/2005 - DKOWALYSHYN - lifted, didn't write it  
//////////////////////////////////////////////////////////////////////////
DWORD RegKeyDeleteAll(HKEY hKey, LPCSTR sKeyName)
{
	DWORD dwSize=MBUFFER, dwIndex=0,rtnVal;
	FILETIME fTime;
	TCHAR sName[MBUFFER];
	HKEY regKey;

	if (!(rtnVal = RegOpenKeyEx(hKey,sKeyName,0,KEY_ALL_ACCESS,&regKey)))
	{
		while (RegEnumKeyEx(regKey,dwIndex,sName,&dwSize,NULL,NULL,NULL,&fTime) != ERROR_NO_MORE_ITEMS)
		{
			rtnVal = RegKeyDeleteAll(regKey,sName);
			dwSize = MBUFFER;
		}
		RegCloseKey(regKey);
	}
	if (!rtnVal)
		rtnVal = RegDeleteKey(hKey,sKeyName);
	return rtnVal;
}


/////////////////////////////////////////////////////////////////////////
//
// Function: ModifyVPLOGONBAT
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCESS
//
// Description:  
//	This function handles defect 1-3KG5D9 --- SAV Server complaining of ResolveSoruce
//
//  This is only called if Patching and in SAVServer mode (and not removal).
//
//////////////////////////////////////////////////////////////////////////
// 04/07/2005 Michael C. Lee - function created.
// 06/04/2005 Brian Aljian - ported from Kepler to Almagest
// 06/16/2005 Brian Aljian - Moved from savMainCA to savServerCA
// 07/11/2005 Jimmy Hoa - ported from Almagest, defect 1-46EY0D
//////////////////////////////////////////////////////////////////////////
UINT _stdcall ModifyVPLOGONBAT( MSIHANDLE hInstall )
{
	MsiLogMessage( hInstall, _T("Modify VPLogon.bat") );

	DWORD dwResult = 0;
	HKEY hKey = NULL;
	DWORD dwRegOptions = KEY_READ;
	TCHAR szVPLOGONFile[1024];
	TCHAR lpszLogMessage[1024];
	DWORD dwSize = 1024;

	MsiLogMessage( hInstall, "ModifyVPLOGONBAT");

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion"), NULL, dwRegOptions, &hKey);
    if (dwResult == ERROR_SUCCESS)
    {
		MsiLogMessage( hInstall, "Read top level registry");

		dwResult = RegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szVPLOGONFile), &dwSize);
		if (dwResult == ERROR_SUCCESS)
		{
			MsiLogMessage( hInstall, "Home Directory");

			_tcscat(szVPLOGONFile, _T("Logon\\VPLOGON.BAT"));

			MsiLogMessage( hInstall, szVPLOGONFile);

			FILETIME ft;
			SYSTEMTIME st;
			GetLocalTime(&st);
			if (SystemTimeToFileTime(&st, &ft))
			{
				MsiLogMessage( hInstall, "Succeeded SystemTimeToFileTime");

				HFILE hFile = HFILE_ERROR;
				OFSTRUCT ofs;

				hFile =OpenFile(szVPLOGONFile,&ofs,OF_READWRITE);

				if ((hFile) && (hFile != HFILE_ERROR))
				{
					if (SetFileTime((HANDLE) hFile, NULL, NULL, &ft))
					{
						MsiLogMessage( hInstall, "Succeeded SetFileTime");
					}
					else
					{
						sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "SetFileTime failed. Error = %d", GetLastError());
						MsiLogMessage( hInstall, lpszLogMessage);
					}

					_lclose(hFile);
				}
				else
				{
					sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "OpenFile failed. Error = %d", GetLastError());
					MsiLogMessage( hInstall, lpszLogMessage);
				}
			}
			else
			{
				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "SystemTimeToFileTime failed. Error = %d", GetLastError());
				MsiLogMessage( hInstall,lpszLogMessage);

			}
		}
		RegCloseKey(hKey);
	}

	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////
//
// Function: MoveVersionedFiles
//
// Parameters:
//		BOOL -	TRUE - Move from AMS dir to temp
//				FALSE - Move from temp back to AMS Dir, rollback
// Returns:
//		ERROR_SUCCES 
//
// Description:  
//	Defect 1-4WBL8G - AMS component not upgrading during migration
//	- Versioned files are not allowing unversioned files to replace them
//		removing files before migration
//
//////////////////////////////////////////////////////////////////////////
// 10/20/2005 Jimmy Hoa - function created.
//////////////////////////////////////////////////////////////////////////
#define AMS_ROLLBACK		"_amsrb"
#define NUM_VERSION_FILES	10

UINT MoveVersionedFiles( BOOL bRemove )
{
	TCHAR *pszFiles[NUM_VERSION_FILES] = {	"CACONFIG.EX_",
											"CASVC.EX_",
											"CBACHAT.EX_",
											"HNDLRSVC.EX_",
											"IAO.EX_",
											"MSGSYS.EX_",
											"ODBCAD32.EX_",
											"PDS.EX_",
											"SNMPAT.EX_",
											"XFR.EX_" };
	TCHAR szPathString[260] = {};
	TCHAR szExisting[260]	= {};
	TCHAR szDest[260]		= {};
	TCHAR szTempPath[260]	= {};
	HKEY hKey = 0;
	BOOL bAmsInstalled = FALSE;
	LONG lReturn = 0;
	DWORD dwType = REG_SZ;;
	DWORD dwSize = 0;

	dwSize = sizeof(szPathString);
	// Check if the registry value Software\Symantec\InstallPath exists.
	// If so, AMS is installed, SAV 9.x or later
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, INSTALLED_APPS_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS )
	{
		lReturn = RegQueryValueEx( hKey, TEXT("AMS_INST_PATH"), 0, &dwType, (LPBYTE)szPathString, &dwSize );
		if( lReturn == ERROR_SUCCESS && dwSize )
			bAmsInstalled = TRUE;			 
		
		RegCloseKey(hKey);
	}
	else if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, AMS_INSTALLED_APPS_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS )
	{
		// AMS installed and is a legacy SAV 8.xx or earlier
		lReturn = RegQueryValueEx( hKey, TEXT("InstallPath"), 0, &dwType, (LPBYTE)szPathString, &dwSize );
		if( lReturn == ERROR_SUCCESS && dwSize )
		{
			// Path only has partial path to AMS files
			vpstrnappend( szPathString, "\\AMS2\\WINNT", sizeof(szPathString) );
			bAmsInstalled = TRUE;
		}

		RegCloseKey(hKey);
	}

	// check if AMS installed
	if( bAmsInstalled )
	{
		// move to temp to save just in case of rollback, %temp%\AMS_ROLLBACK
        GetTempPath( sizeof(szTempPath), szTempPath );
		vpsnprintf( szDest, sizeof(szDest), "%s%s", szTempPath, AMS_ROLLBACK );
		CreateDirectory( szDest, NULL );

		for( int i = 0; i < NUM_VERSION_FILES; i++ )
		{
			// Existing file path
			vpsnprintf( szExisting, sizeof(szExisting), "%s\\%s", szPathString, pszFiles[i] );
			// Destination temp file path
			vpsnprintf( szDest, sizeof(szDest), "%s%s\\%s", szTempPath, AMS_ROLLBACK, pszFiles[i] );
			
			if( bRemove )
				// Move files to temp directory
				MoveFileEx(szExisting, szDest, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
			else
				// Rollback move files from temp directory back
				MoveFileEx(szDest, szExisting, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		}
	}
	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////
//
// Function: MSIRemoveAmsVerFiles
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCES 
//
// Description:  
//	Defect 1-4WBL8G - AMS component not upgrading during migration
//	- Moves versioned files from AMS directory to temp. 
//
//////////////////////////////////////////////////////////////////////////
// 10/20/2005 Jimmy Hoa - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MSIRemoveAmsVerFiles( MSIHANDLE hInstall )
{
	return MoveVersionedFiles( TRUE );
}

/////////////////////////////////////////////////////////////////////////
//
// Function: MSIRemoveAmsVerFilesRB (Rollback)
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCES 
//
// Description:  
//	Defect 1-4WBL8G - AMS component not upgrading during migration
//	- Moves versioned files from temp directory to AMS dir on rollback
//
//////////////////////////////////////////////////////////////////////////
// 10/20/2005 Jimmy Hoa - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MSIRemoveAmsVerFilesRB( MSIHANDLE hInstall )
{
	return MoveVersionedFiles( FALSE );
}

/////////////////////////////////////////////////////////////////////////
//
// Function: MSIRemoveAmsTempDir
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCES 
//
// Description:  
//	Defect 1-4WBL8G - AMS component not upgrading during migration
//	- Deletes temp directory containing versioned files moved earlier in
//		MSIRemoveAmsVerFiles
//
//////////////////////////////////////////////////////////////////////////
// 10/20/2005 Jimmy Hoa - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MSIRemoveAmsTempDir( MSIHANDLE hInstall )
{
	TCHAR szAMSTempPath[260] = {};
	TCHAR szTempPath[260] = {};
	TCHAR szFilePath[260] = {};
	WIN32_FIND_DATA findData = {};
    
	GetTempPath( sizeof(szTempPath), szTempPath );
	vpsnprintf( szAMSTempPath, sizeof(szAMSTempPath), "%s%s\\*", szTempPath, AMS_ROLLBACK );

	HANDLE hFile = FindFirstFile( szAMSTempPath, &findData );

	if( hFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			vpsnprintf( szAMSTempPath, sizeof(szAMSTempPath), "%s%s\\%s", szTempPath, AMS_ROLLBACK, findData.cFileName );
			SetFileAttributes( szAMSTempPath, FILE_ATTRIBUTE_NORMAL );
            DeleteFile( szAMSTempPath );
		}
		while( FindNextFile( hFile, &findData ) );

		CloseHandle( hFile );
	}

	vpsnprintf( szAMSTempPath, sizeof(szAMSTempPath), "%s%s", szTempPath, AMS_ROLLBACK );
	RemoveDirectory( szAMSTempPath );

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: Findit()
//
// Description: This function is supposed to be used to hunt (findfile)for 
// a given file pattern in a given folder.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 06/16/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
BOOL Findit (MSIHANDLE hInstall, LPCTSTR szFile, TCHAR* strPath)
{
	TCHAR						errorMessage[MAX_PATH+1]= {""};	
	WIN32_FIND_DATA				FindFileData;
	HANDLE						hFind;

	sssnprintf(errorMessage, sizeof(errorMessage), "Target file is %s.", szFile);
	MsiLogMessage(hInstall, errorMessage);
	hFind = FindFirstFile(szFile, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		sssnprintf(errorMessage, sizeof(errorMessage), "Invalid File Handle. Get Last Error reports %d.", GetLastError ());
		MsiLogMessage(hInstall, errorMessage);
		return false;
	} 
	else 
	{
		sssnprintf(errorMessage, sizeof(errorMessage), "The first file found is %s.", FindFileData.cFileName);
		MsiLogMessage(hInstall, errorMessage);
		FindClose(hFind);
		_tcscpy (strPath, FindFileData.cFileName);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DetectSCSInstallers
//
// Description: This function is looks for SCS Client installer packages
//	on the Server installer source and sets properties when it finds them.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 06/16/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT _stdcall MSIDetectSCSInstallers( MSIHANDLE hInstall )
{
	TCHAR strTargetDir		[MAX_PATH] = {0};
	TCHAR strErrorMessage	[MAX_PATH+1]= {""};	
	TCHAR strRetSourcePath	[MAX_PATH] = {0};
	TCHAR strSourceDir		[MAX_PATH] = {0};
	DWORD dwLen = MAX_PATH;

	if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("SOURCEDIR"), strSourceDir, &dwLen ) )
	{
		sssnprintf(strErrorMessage, sizeof(strErrorMessage), "DetectSCSInstallers SOURCEDIR = %s", strSourceDir);
		MsiLogMessage(hInstall, strErrorMessage);

		BOOL bRet = false;
		
		for ( int nCount = 0 ; nCount < INSTALLER_SRC_PACKAGES_COUNT ; nCount ++ )
		{
			bRet = false;
			sssnprintf(strTargetDir, sizeof(strTargetDir), "%s%s", strSourceDir, pszInstallerSrcPackages[nCount] );
			bRet = Findit ( hInstall, strTargetDir, strRetSourcePath );
			if ( TRUE == bRet )
			{
				// Set property
				sssnprintf(strErrorMessage, sizeof(strErrorMessage), "DetectSCSInstallers pszInstallerSrcPackages = %s FOUND.", pszInstallerSrcPackages[nCount] );
				MsiLogMessage(hInstall, strErrorMessage);
				if( ERROR_SUCCESS == MsiSetProperty( hInstall, _T(pszSrcPropertiesDectected[nCount]), strTargetDir ))
				{
					sssnprintf(strErrorMessage, sizeof(strErrorMessage), "DetectSCSInstallers Setting PROPERTY %s = %s SUCCESS.", pszSrcPropertiesDectected[nCount], strTargetDir);
					MsiLogMessage(hInstall, strErrorMessage);
				}
				else
				{
					sssnprintf(strErrorMessage, sizeof(strErrorMessage), "DetectSCSInstallers Setting PROPERTY %s FAILED.", pszSrcPropertiesDectected[nCount]);
					MsiLogMessage(hInstall, strErrorMessage);
				}
			}
			else
			{
				// don't set, not found
				sssnprintf(strErrorMessage, sizeof(strErrorMessage), "DetectSCSInstallers pszInstallerSrcPackages = %s NOT FOUND.", pszInstallerSrcPackages);
				MsiLogMessage(hInstall, strErrorMessage);
			}
		}
	}
	else
	{
		sssnprintf(strErrorMessage, sizeof(strErrorMessage), "DetectSCSInstallers failed to get a SOURCEDIR SRV\\Client staging will fail.");
		MsiLogMessage(hInstall, strErrorMessage);
	}

	return ERROR_SUCCESS;
}