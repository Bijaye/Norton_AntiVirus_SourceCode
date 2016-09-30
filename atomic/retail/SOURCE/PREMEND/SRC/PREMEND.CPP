// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/PREMEND/VCS/premend.cpv   1.1   22 Sep 1998 20:26:52   VGLOSTE  $
//
// Description:
//      This file contains the source for the PreMend utility.  This
//		utility allows NAV to be preinstalled.
//
//************************************************************************
// $Log:   S:/PREMEND/VCS/premend.cpv  $
// 
//    Rev 1.1   22 Sep 1998 20:26:52   VGLOSTE
// Changed the first include from windows.h to platform.h for the build
// system.
// 
//    Rev 1.0   22 Sep 1998 03:34:10   VGLOSTE
// Initial revision.
// 
//************************************************************************
#include <platform.h>
#include <tchar.h>
#include <stdio.h>
#include "DefAnnuity.h"
#include "resource.h"

#define DA_VER  0x04080000
#define MAX_KEYSTRING 1024
#define REALLYLONG_STRING 4096
#define BAD_BOOTCOUNT 100
#define PRELOAD_ERROR -1

BOOL WINAPI ReplaceFileOnReboot (LPCTSTR pszExisting, LPCTSTR pszNew);
BOOL MoveKeyValue(LPCTSTR szKeySource, LPCTSTR szValue, LPCTSTR szKeyDest, 
	LPTSTR szBuffer = NULL);
BOOL MyMessageBox(LPCTSTR szMessage, DWORD dwOptions = MB_ICONINFORMATION);
BOOL PromptForOption();

enum COMMAND_TYPE { CT_PRECONFIG = 0, CT_FIRSTBOOT, CT_SECONDBOOT, 
	CT_LATERBOOT, CT_ERROR };
enum E_OSTYPE 
	{ OS_WIN95, OS_WIN98, OS_WINNTWKS, OS_WINNTSRV, OS_WIN32S, OS_ERROR };
enum OPTION { OPT_NONE = 0, OPT_PRE, OPT_SECOND } g_option = OPT_NONE;

// Messages
const TCHAR szErrorPre[] = 
 _T("Norton AntiVirus Preload processing was not successful!.  "
    "NAV is not setup correctly.  The registry may have been modified.");
const TCHAR szGoodPre[] =
 _T("Norton AntiVirus Preload processing was completed successfully!");
const TCHAR szMessageTitle[] = _T("Norton AntiVirus Preinstall Utility");
const TCHAR szErrAP[]	 = 
	_T("Some AP components installed, but not others.  "
	   "Please check configuration.");

// Registry keys
const TCHAR szKeyRun[] = 
	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
const TCHAR szKeyWinVersion[] = 
	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion");
const TCHAR szKeyNTVersion[] = 
	_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
const TCHAR szKeyPhase[] = 
	_T("SOFTWARE\\Symantec\\Norton AntiVirus");
const TCHAR szKeyDefAnn[] = 
	_T("SOFTWARE\\Symantec\\Norton AntiVirus\\DefAnnuity");
const TCHAR szKeyVxD[] =
	_T("System\\CurrentControlSet\\Services\\VxD\\NAVAP");
const TCHAR szNTVerKey[] = 
	_T("System\\CurrentControlSet\\Control\\ProductOptions");

// DefAnnuity value names
const TCHAR szDaFreeLen[]	= _T("FreeLength");
const TCHAR szDaFreeWarn[]	= _T("FreeEndWarning");
const TCHAR szDaSubsLen[]	= _T("SubscriptionLength");
const TCHAR szDaSubsWarn[]	= _T("SubscriptionEndWarning");
const TCHAR szDaVersion[]	= _T("Version");
const TCHAR szDaEnabled[]	= _T("Enabled");

// Misc. strings
const TCHAR szVersion[]	 = _T("1.05");
const TCHAR szNTVerVal[] = _T("Producttype");
const TCHAR szNTWksVal[] = _T("WINNT");
const TCHAR szValName[]  = _T("Norton AV Preload");
const TCHAR szOptName[]	 = _T("Preload Options");
const TCHAR szAPRunVal[] = _T("Norton Auto-Protect");
const TCHAR szAPVXDVal[] = _T("StaticVxD");
const TCHAR szWinIDVal[] = _T("ProductId");
const TCHAR szBatVal[]   = _T("Batch");
const TCHAR szDelCmd[]   = _T("rundll32.exe kernel32.dll,DeleteFileA ");
const TCHAR szNAVDX[]    = _T("NAVDX.EXE");
const TCHAR szAutoexec[] = _T("C:\\autoexec.bat");
const TCHAR szAutoold[]  = _T("C:\\autoexec.old");
const TCHAR szModeR[]	 = _T("r");
const TCHAR szModeW[]	 = _T("w");
const TCHAR szCR[]		 = _T("\r\n");
const TCHAR szAP[]		 = _T("Auto-Protect was ");
const TCHAR szDX[]		 = _T("Scan on startup was ");
const TCHAR szNot[]		 = _T("not ");
const TCHAR szInst[]	 = _T("installed.");
const TCHAR szErrorOS1[] = _T("Invalid OS type: ");
const TCHAR szErrorOS2[] = _T("  Unable to detect OS type!");
const TCHAR szFRestore[] = _T("Autostart components will be restored after the next boot.");
const TCHAR sz2Restore[] = _T("Autostart components will be restored after the second customer boot");
const TCHAR szOS95[]	 = _T("Windows 95");
const TCHAR szOS98[]	 = _T("Windows 98");
const TCHAR szOSNTW[]	 = _T("Windows NT Workstation");
const TCHAR szOSNTS[]	 = _T("Windows NT Server");
const TCHAR szOS32S[]	 = _T("Win32s");
const TCHAR szOSErr[]	 = _T("****ERROR****");
const TCHAR szFmtOS1[]	 = _T(" %2d.%02d Build %d");
const TCHAR szFmtOS2[]	 = _T(" %2d.%02d");
const TCHAR szOSPre[]	 = _T("Operating System: ");
const TCHAR sz4SP[]		 = _T("    ");
const TCHAR szVxDExt[]	 = _T(".vxd");

HINSTANCE g_hInst;

// These are only valid during the preload boot
BOOL g_bAP    = FALSE;
BOOL g_bNAVDX = FALSE;

BOOL g_bSilent = FALSE;
TCHAR szCommand[MAX_KEYSTRING];

/**********************************************************
* FUNCTION: 	GetTitleString
*
* PARAMETERS:	
* RETURNS:		LPCTSTR
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Returns a pointer to a string containing
* 				the title
***********************************************************/
LPCTSTR GetTitleString()
{	static TCHAR szTitle[128];

	_tcscpy(szTitle, szMessageTitle);
	_tcscat(szTitle, sz4SP);
	_tcscat(szTitle, szVersion);
	return szTitle;
}

/**********************************************************
* FUNCTION: 	MyMessageBox
*
* PARAMETERS:	LPCTSTR szMessage
*				DWORD dwOptions
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Creates a message box with the correct
* 				title bar text
***********************************************************/
BOOL MyMessageBox(LPCTSTR szMessage, DWORD dwOptions)
{	LPCTSTR pTitle;

	pTitle = GetTitleString();
	MessageBox(NULL, szMessage, pTitle, MB_OK | dwOptions);
	return TRUE;
}

/**********************************************************
* FUNCTION: 	DoInformInstaller
*
* PARAMETERS:	E_OSTYPE type
*				LPCTSTR szWinVer
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Puts up a message box that contains
* 				information about the preinstall
***********************************************************/
BOOL DoInformInstaller(E_OSTYPE type, LPCTSTR szWinVer)
{	TCHAR szMessage[REALLYLONG_STRING];

	_tcscpy(szMessage, szGoodPre);
	_tcscat(szMessage, szCR);
	_tcscat(szMessage, szOSPre);
	_tcscat(szMessage, szCR);
	_tcscat(szMessage, sz4SP);
	_tcscat(szMessage, szWinVer);
	_tcscat(szMessage, szCR);
	if((type == OS_WIN95) || (type == OS_WIN98))
	{	_tcscat(szMessage, szAP);
		if(!g_bAP)
			_tcscat(szMessage, szNot);
		_tcscat(szMessage, szInst);
		_tcscat(szMessage, szCR);
		_tcscat(szMessage, szDX);
		if(!g_bNAVDX)
			_tcscat(szMessage, szNot);
		_tcscat(szMessage, szInst);
		if(g_bAP || g_bNAVDX)
		{	_tcscat(szMessage, szCR);
			if(g_option == OPT_PRE)
				_tcscat(szMessage, szFRestore);
			if(g_option == OPT_SECOND)
				_tcscat(szMessage, sz2Restore);
		}
	}
	if(!g_bSilent)
		MyMessageBox(szMessage);
	return TRUE;
}

/**********************************************************
* FUNCTION: 	IsPastFirstCustomerBoot
*
* PARAMETERS:	E_OSTYPE type
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Returns TRUE if the COA (Windows install 
* 				key) is in the registry
***********************************************************/
BOOL IsPastFirstCustomerBoot(E_OSTYPE type)
{	TCHAR szData[MAX_KEYSTRING];
	DWORD dwType, dwSize;
	BOOL  bRes = FALSE;
	LONG  lRes;
	HKEY  hKey;

	if((type == OS_WIN95) || (type == OS_WIN98))
		lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyWinVersion, &hKey);
	else
		lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyNTVersion, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	dwSize = MAX_KEYSTRING;
	szData[0] = '\0';
	lRes = RegQueryValueEx(hKey, szWinIDVal, 0, &dwType, (BYTE *)szData, &dwSize);
	if(lRes == ERROR_SUCCESS && _tcslen(szData))
		bRes = TRUE;
	RegCloseKey(hKey);
exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	MoveKeyValue
*
* PARAMETERS:	LPCTSTR szKeySource
*				LPCTSTR szValue
*				LPCTSTR szKeyDest
*				LPTSTR szBuffer
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Moves a string value from one key to another
* 				
***********************************************************/
// Values are assumed to be < MAX_KEYSTRING
BOOL MoveKeyValue(LPCTSTR szKeySource, LPCTSTR szValue, LPCTSTR szKeyDest, 
	LPTSTR szBuffer)
{	TCHAR szData[MAX_KEYSTRING];
	DWORD dwType, dwSize;
	BOOL  bRes = FALSE;
	LONG  lRes;
	HKEY  hKey, hKeyDest;

	if(szBuffer != NULL)
		*szBuffer = '\0';
	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeySource, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	dwSize = MAX_KEYSTRING;
	lRes = RegQueryValueEx(hKey, szValue, 0, &dwType, (BYTE *)szData, &dwSize);
	if(lRes != ERROR_SUCCESS)
		goto key1_exit;
	if(szBuffer != NULL)
		_tcscpy(szBuffer, szData);
	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyDest, &hKeyDest);
	if(lRes != ERROR_SUCCESS)
		goto key1_exit;
	lRes = RegSetValueEx(hKeyDest, szValue, 0, dwType, (CONST BYTE *)szData, 
		dwSize);
	if(lRes != ERROR_SUCCESS)
		goto key2_exit;
	lRes = RegDeleteValue(hKey, szValue);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
key2_exit:
	RegCloseKey(hKeyDest);
key1_exit:
	RegCloseKey(hKey);
exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	MoveDWORDKeyValue
*
* PARAMETERS:	LPCTSTR szKeySource
*				LPCTSTR szValue
*				LPCTSTR szKeyDest
*				DWORD *pData
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Moves a DWORD value from one key to another
* 				
***********************************************************/
BOOL MoveDWORDKeyValue(LPCTSTR szKeySource, LPCTSTR szValue, LPCTSTR szKeyDest, 
	DWORD *pData)
{	DWORD dwData;
	DWORD dwType, dwSize;
	BOOL  bRes = FALSE;
	LONG  lRes;
	HKEY  hKey, hKeyDest;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeySource, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	dwSize = sizeof(DWORD);
	lRes = RegQueryValueEx(hKey, szValue, 0, &dwType, (BYTE *)&dwData, &dwSize);
	if(lRes != ERROR_SUCCESS)
		goto key1_exit;
	if(pData != NULL)
		*pData = dwData;
	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyDest, &hKeyDest);
	if(lRes != ERROR_SUCCESS)
		goto key1_exit;
	lRes = RegSetValueEx(hKeyDest, szValue, 0, dwType, (CONST BYTE *)&dwData, 
		dwSize);
	if(lRes != ERROR_SUCCESS)
		goto key2_exit;
	lRes = RegDeleteValue(hKey, szValue);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
key2_exit:
	RegCloseKey(hKeyDest);
key1_exit:
	RegCloseKey(hKey);
exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	RemoveKeyValue
*
* PARAMETERS:	LPCTSTR szKeySource
*				LPCTSTR szValue
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Removes a value from a key
* 				
***********************************************************/
BOOL RemoveKeyValue(LPCTSTR szKeySource, LPCTSTR szValue)
{	BOOL  bRes = FALSE;
	LONG  lRes;
	HKEY  hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeySource, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	lRes = RegDeleteValue(hKey, szValue);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
	RegCloseKey(hKey);
exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	AddKeyValue
*
* PARAMETERS:	LPCTSTR szKey
*				LPCTSTR szName
*				LPCTSTR szVal
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Adds or replaces a value in a key
* 				
***********************************************************/
BOOL AddKeyValue(LPCTSTR szKey, LPCTSTR szName, LPCTSTR szVal)
{	BOOL bRes = FALSE;
	LONG lRes;
	HKEY hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	lRes = RegSetValueEx(hKey, szName, 0, REG_SZ, (CONST BYTE *)szVal, 
		_tcslen(szVal) + 1);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
	RegCloseKey(hKey);

exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	ReadKeyValue
*
* PARAMETERS:	LPCTSTR szKey
*				LPCTSTR szName
*				LPTSTR szVal
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Reads a key value that contains a string    
* 				
***********************************************************/
BOOL ReadKeyValue(LPCTSTR szKey, LPCTSTR szName, LPTSTR szVal)
{	BOOL bRes = FALSE;
	DWORD dwType, dwSize;
	LONG lRes;
	HKEY hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	dwSize = MAX_KEYSTRING;
	lRes = RegQueryValueEx(hKey, szName, 0, &dwType, (BYTE *)szVal, &dwSize);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
	RegCloseKey(hKey);

exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	ReadDWORDKeyValue
*
* PARAMETERS:	LPCTSTR szKey
*				LPCTSTR szName
*				DWORD dwDefault
* RETURNS:		DWORD
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Reads a key value that contains a DWORD
* 				
***********************************************************/
DWORD ReadDWORDKeyValue(LPCTSTR szKey, LPCTSTR szName, DWORD dwDefault)
{	BOOL bRes = FALSE;
	DWORD dwType, dwSize, dwVal;
	LONG lRes;
	HKEY hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	dwSize = sizeof(DWORD);
	lRes = RegQueryValueEx(hKey, szName, 0, &dwType, (BYTE *)&dwVal, &dwSize);
	if(lRes == ERROR_SUCCESS)
		dwDefault = dwVal;
	RegCloseKey(hKey);

exit:
	return dwDefault;
}

/**********************************************************
* FUNCTION: 	AddRunValue
*
* PARAMETERS:	LPCTSTR szName		value name
*				LPCTSTR szCmdLine	command to be run
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Adds a value to the RUN key causing a 
* 				program to be run on every boot
***********************************************************/
BOOL AddRunValue(LPCTSTR szName, LPCTSTR szCmdLine)
{	BOOL bRes = FALSE;
	LONG lRes;
	HKEY hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyRun, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	lRes = RegSetValueEx(hKey, szName, 0, REG_SZ, (CONST BYTE *)szCmdLine, 
		_tcslen(szCmdLine) + 1);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
	RegCloseKey(hKey);

exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	RemoveRunValue
*
* PARAMETERS:	LPCTSTR szName		value name
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Removes a RUN value, so the program no
* 				longer runs on boots.
***********************************************************/
BOOL RemoveRunValue(LPCTSTR szName)
{	BOOL bRes = FALSE;
	LONG lRes;
	HKEY hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyRun, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	lRes = RegDeleteValue(hKey, szName);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
	RegCloseKey(hKey);

exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	GetWindowsTypeAndVer
*
* PARAMETERS:	int *piVerMajor		receives major OS version
*				int *piVerMinor		receives minor OS version
*				int *piBuild		receives build number
* RETURNS:		E_OSTYPE			returns operating system
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Detects the Operating System and version
* 				
***********************************************************/
E_OSTYPE GetWindowsTypeAndVer(int *piVerMajor, int *piVerMinor, int *piBuild)
{	E_OSTYPE type = OS_ERROR;
	DWORD dwVersion;
	TCHAR szValue[256];

	// Initialize
	*piVerMajor = *piVerMinor = *piBuild = 0;

	// We are  using GetVersion rather then GetVersionEx
	// because earlier versions of Windows NT and Win32s
	// didn't include GetVersionEx:
	dwVersion = GetVersion();

	if (dwVersion < 0x80000000)
	{	if(!ReadKeyValue(szNTVerKey, szNTVerVal, szValue))
			goto exit;
		type = OS_WINNTWKS;
		if(_tcsicmp(szNTWksVal, szValue))
			type = OS_WINNTSRV;
	}
	else if (LOBYTE(LOWORD(dwVersion))<4)
		type = OS_WIN32S;
	else if((HIBYTE(LOWORD(dwVersion))) < 10)
		type = OS_WIN95;
	else
		type = OS_WIN98;
	*piVerMajor = LOBYTE(LOWORD(dwVersion));
	*piVerMinor = HIBYTE(LOWORD(dwVersion));
	switch(type)
	{	case OS_WINNTSRV:
		case OS_WINNTWKS:
			*piBuild = HIWORD(dwVersion);
			break;
		case OS_WIN32S:
			*piBuild = (HIWORD(dwVersion) & ~0x8000);
			break;
	}
exit:
	return type;
}

/**********************************************************
* FUNCTION: 	IncrementBootCount
*
* PARAMETERS:	
* RETURNS:		COMMAND_TYPE
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Increments the boot counter, and returns
* 				what the current phase is
***********************************************************/
COMMAND_TYPE IncrementBootCount()
{	COMMAND_TYPE ct = CT_ERROR;
	DWORD dwBoot, dwType, dwSize;
	LONG  lRes;
	HKEY  hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyPhase, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	dwSize = sizeof(DWORD);
	lRes = RegQueryValueEx(hKey, szValName, 0, &dwType, (BYTE *)&dwBoot, &dwSize);
	if(lRes == ERROR_SUCCESS)
		switch(dwBoot)
		{	case 1:
				ct = CT_FIRSTBOOT;
				dwBoot = 2;
				break;
			case 2:
				ct = CT_SECONDBOOT;
				dwBoot = 3;
				break;
			case 3:
				ct = CT_LATERBOOT;
				dwBoot = 3;
				break;
			default:
				ct = CT_ERROR;
				break;
		}
	else
	{	ct = CT_PRECONFIG;
		dwBoot = 1;
	}
	lRes = RegSetValueEx(hKey, szValName, 0, REG_DWORD, (CONST BYTE *)&dwBoot, 
		sizeof(DWORD));
	RegCloseKey(hKey);
exit:
	return ct;
}

/**********************************************************
* FUNCTION: 	LockoutBootCount
*
* PARAMETERS:	
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Writes a value to the registry that locks
* 				out future running of PreMend
***********************************************************/
BOOL LockoutBootCount()
{	BOOL bRes = FALSE;
	DWORD dwBoot;
	LONG  lRes;
	HKEY  hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeyPhase, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	dwBoot = BAD_BOOTCOUNT;
	lRes = RegSetValueEx(hKey, szValName, 0, REG_DWORD, (CONST BYTE *)&dwBoot, 
		sizeof(DWORD));
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
	RegCloseKey(hKey);
exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	ReviseAutoexecRemove
*
* PARAMETERS:	
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Removes any line from AUTOEXEC.BAT that
* 				contains the string NAVDX, and stores it
*				in the registry
***********************************************************/
BOOL ReviseAutoexecRemove()
{	FILE *fpIn  = NULL;
	FILE *fpOut = NULL;
	BOOL bFound = FALSE;

	DeleteFile(szAutoold);
	if(!MoveFile(szAutoexec, szAutoold))
		goto exit;
	fpIn  = _tfopen(szAutoold, szModeR);
	fpOut = _tfopen(szAutoexec, szModeW);
	if(!fpIn || !fpOut)
		goto aborttest;
	_fgetts(szCommand, sizeof(szCommand), fpIn);
	while(!feof(fpIn))
	{	if(_tcsstr(szCommand, szNAVDX))
		{	AddKeyValue(szKeyPhase, szBatVal, szCommand);
			bFound = TRUE;
		}
		else
			_fputts(szCommand, fpOut);
		_fgetts(szCommand, sizeof(szCommand), fpIn);
	}
aborttest:
	if(fpIn)
	{	fflush(fpIn);
		fclose(fpIn);
	}
	if(fpOut)
	{	fflush(fpOut);
		fclose(fpOut);
	}
	if(!bFound)
	{	DeleteFile(szAutoexec);
		MoveFile(szAutoold, szAutoexec);
	}
	else
		DeleteFile(szAutoold);
exit:
	return bFound;
}

/**********************************************************
* FUNCTION: 	ReviseAutoexecRestore
*
* PARAMETERS:	
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Restores a line to the AUTOEXEC.BAT file
* 				that was previously removed by 
*				ReviseAutoexecRemove()
***********************************************************/
BOOL ReviseAutoexecRestore()
{	FILE *fpIn  = NULL;
	FILE *fpOut = NULL;
	BOOL bFound = FALSE;

	DeleteFile(szAutoold);
	if(!MoveFile(szAutoexec, szAutoold))
		goto exit;
	fpIn  = _tfopen(szAutoold, szModeR);
	fpOut = _tfopen(szAutoexec, szModeW);
	if(!fpIn || !fpOut)
		goto aborttest;
	szCommand[0] = '\0';
	if(!ReadKeyValue(szKeyPhase, szBatVal, szCommand) || !_tcslen(szCommand))
		goto aborttest;
	_fputts(szCommand, fpOut);
	bFound = TRUE;

	_fgetts(szCommand, sizeof(szCommand), fpIn);
	while(!feof(fpIn))
	{	_fputts(szCommand, fpOut);
		_fgetts(szCommand, sizeof(szCommand), fpIn);
	}
aborttest:
	if(fpIn)
	{	fflush(fpIn);
		fclose(fpIn);
	}
	if(fpOut)
	{	fflush(fpOut);
		fclose(fpOut);
	}
	if(!bFound)
	{	DeleteFile(szAutoexec);
		MoveFile(szAutoold, szAutoexec);
	}
	else
	{	DeleteFile(szAutoold);
		RemoveKeyValue(szKeyPhase, szBatVal);
	}
exit:
	return bFound;
}

/**********************************************************
* FUNCTION: 	GetCommandLine
*
* PARAMETERS:	LPTSTR szCmdLine		buffer for output
*				DWORD size				buffer size
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Gets the path and filename (shortened to
* 				8.3) of the current module
***********************************************************/
BOOL GetCommandLine(LPTSTR szCmdLine, DWORD size)
{	BOOL bRes = FALSE;
	TCHAR szTemp[1024];
	DWORD len;

	len = GetModuleFileName(NULL, szTemp, sizeof(szTemp));
	if(!len)
		goto exit;
	len = GetShortPathName(szTemp, szCmdLine, size);
	if(!len || len > size)
		goto exit;
	bRes = TRUE;

exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	ReInitVirusSubscription
*
* PARAMETERS:	
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Reads the current values of the DefAnnuity
* 				regkeys, then reinitializes it.  This makes
*				the base date be today.
***********************************************************/
BOOL ReInitVirusSubscription()
{	DWORD dwDaFreeLen;
	DWORD dwDaFreeWarn;
	DWORD dwDaSubsLen;
	DWORD dwDaSubsWarn;
	DWORD dwDaVersion;
	DWORD dwDaEnabled;

	dwDaFreeLen   = ReadDWORDKeyValue(szKeyPhase, szDaFreeLen,  0);
	dwDaFreeWarn  = ReadDWORDKeyValue(szKeyPhase, szDaFreeWarn, 0);
	dwDaSubsLen   = ReadDWORDKeyValue(szKeyPhase, szDaSubsLen,  0);
	dwDaSubsWarn  = ReadDWORDKeyValue(szKeyPhase, szDaSubsWarn, 0);
	dwDaVersion   = ReadDWORDKeyValue(szKeyPhase, szDaVersion,  DA_VER);
	dwDaEnabled   = ReadDWORDKeyValue(szKeyPhase, szDaEnabled,  1);

	// Delete the virus definitions subscription  information
	RegDeleteKey(HKEY_LOCAL_MACHINE, szKeyDefAnn);

	DefAnnuityInitialize(NULL, dwDaVersion, dwDaEnabled, 1, dwDaFreeLen, 
		dwDaSubsLen, dwDaFreeWarn, dwDaSubsWarn);

	// Get rid of the extra keys we used to temporarily store the 
	// subscription info
	RemoveKeyValue(szKeyPhase, szDaFreeLen);
	RemoveKeyValue(szKeyPhase, szDaFreeWarn);
	RemoveKeyValue(szKeyPhase, szDaSubsLen);
	RemoveKeyValue(szKeyPhase, szDaSubsWarn);
	RemoveKeyValue(szKeyPhase, szDaVersion);
	RemoveKeyValue(szKeyPhase, szDaEnabled);

	return TRUE;
}

/**********************************************************
* FUNCTION: 	PreloadProcess
*
* PARAMETERS:	E_OSTYPE type
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Does the processing for the preload phase.
* 				
***********************************************************/
BOOL PreloadProcess(E_OSTYPE type)
{	BOOL bRes = FALSE;
	BOOL bAP1, bAP2;

	if((type == OS_WIN95) || (type == OS_WIN98))
	{	// Move the AP startup commands
		bAP1 = MoveKeyValue(szKeyRun, szAPRunVal, szKeyPhase);
		bAP2 = MoveKeyValue(szKeyVxD, szAPVXDVal, szKeyPhase);
		g_bAP = (bAP1 && bAP2);
		// No logical XOR in C so we have to do the following
		if(!((bAP1 && bAP2) || !(bAP1 || bAP2)) && !g_bSilent)
			MyMessageBox(szErrAP, MB_ICONEXCLAMATION);

		// Move the NAVDX Autoexec.bat entry to the registry for
		// safekeeping.
		g_bNAVDX = ReviseAutoexecRemove();
	}

	// Move all the DefAnnuity virus definitions subscription values
	MoveDWORDKeyValue(szKeyDefAnn, szDaFreeLen,  szKeyPhase, NULL);
	MoveDWORDKeyValue(szKeyDefAnn, szDaFreeWarn, szKeyPhase, NULL);
	MoveDWORDKeyValue(szKeyDefAnn, szDaSubsLen,  szKeyPhase, NULL);
	MoveDWORDKeyValue(szKeyDefAnn, szDaSubsWarn, szKeyPhase, NULL);
	MoveDWORDKeyValue(szKeyDefAnn, szDaVersion,  szKeyPhase, NULL);
	MoveDWORDKeyValue(szKeyDefAnn, szDaEnabled,  szKeyPhase, NULL);

	// Delete the virus definitions subscription information
	RegDeleteKey(HKEY_LOCAL_MACHINE, szKeyDefAnn);

	// Set things up so that we are run on every boot
	if(!GetCommandLine(szCommand, sizeof(szCommand)))
		goto exit;
	if(g_option == OPT_NONE)	// If it was not set  
	{	g_option = OPT_SECOND;	// set to default
		if((type == OS_WIN95) || (type == OS_WIN98))
			PromptForOption();	// and prompt
	}
	switch(g_option)
	{	case OPT_PRE:
			_tcscat(szCommand, " -F");
			break;
		case OPT_SECOND:
			_tcscat(szCommand, " -2");
			break;
	}
	bRes = AddRunValue(szValName, szCommand);

exit:
	return bRes;
}

/**********************************************************
* FUNCTION: 	FirstbootProcess
*
* PARAMETERS:	E_OSTYPE type
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Performs the processing for the first
* 				boot.
***********************************************************/
BOOL FirstbootProcess(E_OSTYPE type)
{	BOOL bRes = TRUE;
	BOOL bVxD, bAPUI;
	TCHAR szVxDCmd[MAX_KEYSTRING], szAPUICmd[MAX_KEYSTRING];

	if(g_option == OPT_PRE)
	{	if((type == OS_WIN95) || (type == OS_WIN98))
		{	// Restore AP startup commands
			bVxD = MoveKeyValue(szKeyPhase, szAPVXDVal, szKeyVxD, szVxDCmd);
			bAPUI = MoveKeyValue(szKeyPhase, szAPRunVal, szKeyRun, szAPUICmd);

			// Restore NAVDX command to Autoexec.bat
			ReviseAutoexecRestore();
		}
	}

	return bRes;
}

/**********************************************************
* FUNCTION: 	FinalbootProcess
*
* PARAMETERS:	E_OSTYPE type
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Do the processing for the final boot 
* 				
***********************************************************/
BOOL FinalbootProcess(E_OSTYPE type)
{	BOOL bRes = TRUE;
	BOOL bVxD, bAPUI;
	TCHAR szVxDCmd[MAX_KEYSTRING], szAPUICmd[MAX_KEYSTRING];
	UINT uReturn;

	if(g_option == OPT_SECOND)
	{	if((type == OS_WIN95) || (type == OS_WIN98))
		{	// Restore AP startup commands
			bVxD = MoveKeyValue(szKeyPhase, szAPVXDVal, szKeyVxD, szVxDCmd);
			bAPUI = MoveKeyValue(szKeyPhase, szAPRunVal, szKeyRun, szAPUICmd);

			if(bAPUI)
				uReturn = WinExec(szAPUICmd, SW_SHOWNORMAL);

			// Restore NAVDX command to Autoexec.bat
			ReviseAutoexecRestore();
		}
	}

	// Initialize virus subscription
	ReInitVirusSubscription();

	// Remove the command that makes us run on every boot
	RemoveRunValue(szValName);

	// Tell Windows to delete us on the next boot
	if(!GetCommandLine(szCommand, sizeof(szCommand)))
		bRes = FALSE;
	else
		ReplaceFileOnReboot(szCommand, NULL);

	// Lockout any further runs of the program from doing anything
	LockoutBootCount();
	return bRes;
}

/**********************************************************
* FUNCTION: 	ParseCommandLine
*
* PARAMETERS:	LPCSTR lpCmdLine
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Returns TRUE if silent mode is selected,
*				FALSE otherwise.  To select silent mode
*				invoke with "-s" on the command line
*
*				This also sets the option variable.  This
*				controls for Win 9X when the autostart
*				components are restored.  "-2" (the 
*				default) makes the components be restored
*				at the second customer boot.  "-f" 
*				causes the components to be restored at 
*				the first factory boot.  The information 
*				is stored in a global variable: g_option.
* 				
*				Note: this does not use LPCTSTR because
*				the command line is always single-byte.
***********************************************************/
BOOL ParseCommandLine(LPCSTR lpCmdLine)
{	LPCSTR pSwitch;
	BOOL bRes = FALSE;
	BOOL bExit = FALSE;

	pSwitch = strchr(lpCmdLine, '-');
	if(pSwitch == NULL)
		pSwitch = strchr(lpCmdLine, '/');
	if(pSwitch == NULL)
		goto end;
	++pSwitch;
	do
	{	switch(*pSwitch)
		{	case 'S':
			case 's':
				bRes = TRUE;
				break;
			case 'F':
			case 'f':
				g_option = OPT_PRE;
				break;
			case '2':
				g_option = OPT_SECOND;
				break;
			case ' ':
			case '-':
			case '/':
			case '\t':
				break;
			default:
				bExit = TRUE;
		}
		++pSwitch;
	} while (!bExit);
end:
	return bRes;
}

/**********************************************************
* FUNCTION: 	CreateWinVerString
*
* PARAMETERS:	E_OSTYPE type
*				int iVerMajor
*				int iVerMinor
*				int iBuild
* RETURNS:		LPCTSTR
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Returns a pointer to a string that contains
* 				formatted text showing the OS and version
*				information
***********************************************************/
LPCTSTR CreateWinVerString(E_OSTYPE type, int iVerMajor, int iVerMinor, int iBuild)
{	static TCHAR szOut[256];
	TCHAR szFormat[256];

	szOut[0] = szFormat[0] = '\0';
	switch(type)
	{	case OS_WIN95: 
			_tcscpy(szFormat, szOS95);
			break;
		case OS_WIN98: 
			_tcscpy(szFormat, szOS98);
			break;
		case OS_WINNTWKS: 
			_tcscpy(szFormat, szOSNTW);
			break;
		case OS_WINNTSRV: 
			_tcscpy(szFormat, szOSNTS);
			break;
		case OS_WIN32S: 
			_tcscpy(szFormat, szOS32S);
			break;
		case OS_ERROR: 
			_tcscpy(szFormat, szOSErr);
			break;
	}
	switch(type)
	{	case OS_WIN95: 
		case OS_WIN98: 
			_tcscat(szFormat, szFmtOS2);
			wsprintf(szOut, szFormat, iVerMajor, iVerMinor);
			break;
		case OS_WINNTWKS: 
		case OS_WINNTSRV: 
		case OS_WIN32S: 
			_tcscat(szFormat, szFmtOS1);
			wsprintf(szOut, szFormat, iVerMajor, iVerMinor, iBuild);
			break;
	}
	return szOut;
}

/**********************************************************
* FUNCTION: 	CenterWindow
*
* PARAMETERS:	HWND hWnd
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Centers a window on the screen without
* 				resizing it.
***********************************************************/
BOOL CenterWindow(HWND hWnd)
{	RECT rect1;
	BOOL bRes = FALSE;
	int iSWidth, iSHeight;

	if(!GetWindowRect(hWnd, &rect1))
		goto end;
	iSWidth  = GetSystemMetrics(SM_CXFULLSCREEN);
	iSHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	if(iSWidth <= 0  ||  iSHeight <= 0)
		goto end;
	if(SetWindowPos(hWnd, NULL, 
		(iSWidth / 2) - ((rect1.right - rect1.left) / 2),
		(iSHeight / 2) - ((rect1.bottom - rect1.top) / 2),
		0, 0, SWP_NOZORDER | SWP_NOSIZE))
			bRes = TRUE;
end:
	return bRes;
}

/**********************************************************
* FUNCTION: 	DlgProc
*
* PARAMETERS:	HWND hWnd
*				UINT wMsg
*				WPARAM wParam
*				LPARAM lParam
* RETURNS:		int
* AUTHOR:		Vance Gloster
* DESCRIPTION:	DialogProc function for the dialog box that 
* 				asks the user for when to restore the 
*				autostart components.
***********************************************************/
extern "C" 
int APIENTRY DlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wMsg)
	{	case WM_INITDIALOG:
		{	LPCTSTR pTitle;

			pTitle = GetTitleString();
			SetWindowText(hWnd, pTitle);
			if(g_option == OPT_PRE)
				CheckRadioButton(hWnd, IDC_RESTF, IDC_REST2, IDC_RESTF);
			if(g_option == OPT_SECOND)
				CheckRadioButton(hWnd, IDC_RESTF, IDC_REST2, IDC_REST2);
			CenterWindow(hWnd);
			return TRUE;
			break;
		}
		case WM_COMMAND:
			if((LOWORD(wParam) == IDOK) && (HIWORD(wParam) == BN_CLICKED))
			{	UINT uChecked;

				uChecked = IsDlgButtonChecked(hWnd, IDC_RESTF);
				if(uChecked == BST_CHECKED)
					g_option = OPT_PRE;
				else
					g_option = OPT_SECOND;
				EndDialog(hWnd, IDOK);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

/**********************************************************
* FUNCTION: 	PromptForOption
*
* PARAMETERS:	
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Brings up the Dialog Box that asks the 
* 				user for when to restore the autostart 
*				components.
* 				
***********************************************************/
BOOL PromptForOption()
{
	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_RESTORE_TIME), NULL, 
		(DLGPROC) DlgProc);
	return TRUE;
}

void TestFunc()
{
	DebugBreak();
}

/**********************************************************
* FUNCTION: 	WinMain
*
* PARAMETERS:	HINSTANCE hInstance
*				HINSTANCE hPrevInstance
*				LPSTR lpCmdLine
*				int nCmdShow
* RETURNS:		int
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Entry point for the application.
* 				
***********************************************************/
extern "C" 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	COMMAND_TYPE ct = CT_ERROR;
	TCHAR szMessage[512];
	LPCTSTR pWinVer;
	int i1, i2, i3;
	E_OSTYPE type;
	int iRet = 0;

//	TestFunc();
	g_hInst = hInstance;
	type = GetWindowsTypeAndVer(&i1, &i2, &i3);
	pWinVer = CreateWinVerString(type, i1, i2, i3);
	switch(type)
	{	case OS_WINNTWKS: 
		case OS_WINNTSRV: 
			if((i1 >= 4) || ((i1 == 3) && (i2 >= 51)))
				break;
		case OS_WIN32S: 
			_tcscpy(szMessage, szErrorOS1);
			_tcscat(szMessage, pWinVer);
			MyMessageBox(szMessage);
			goto exit;
		case OS_ERROR: 
			_tcscpy(szMessage, pWinVer);
			_tcscat(szMessage, szErrorOS2);
			MyMessageBox(szMessage);
			goto exit;
	}

	g_bSilent = ParseCommandLine(lpCmdLine);
	ct = IncrementBootCount();
	switch(ct)
	{	case CT_PRECONFIG:
			if(!PreloadProcess(type)  && !g_bSilent)
			{	MyMessageBox(szErrorPre, MB_ICONSTOP);
				iRet = PRELOAD_ERROR;
			}
			else
			{	if(!g_bSilent)
					DoInformInstaller(type, pWinVer);
			}
			break;
		case CT_FIRSTBOOT:
			if(g_option == OPT_NONE)	// If it was not set  
				g_option = OPT_SECOND;	// set to default
			FirstbootProcess(type);
			break;
		case CT_SECONDBOOT:
		case CT_LATERBOOT:
			if(g_option == OPT_NONE)	// If it was not set  
				g_option = OPT_SECOND;	// set to default
			if(IsPastFirstCustomerBoot(type))
				FinalbootProcess(type);
			break;
		case CT_ERROR:
		default:
			break;
	}
	goto exit;

exit:
	return iRet;
}

/**********************************************************
* FUNCTION: 	ReplaceFileOnReboot
*
* PARAMETERS:	LPCTSTR pszExisting
*				LPCTSTR pszNew
* RETURNS:		BOOL
* AUTHOR:		Jeffrey Richter
* DESCRIPTION:	Replaces or deletes a file on the next
* 				reboot.  Borrowed from an article in MSJ.
***********************************************************/
BOOL WINAPI ReplaceFileOnReboot (LPCTSTR pszExisting, LPCTSTR pszNew) 
{
   // First, attempt to use the MoveFileEx function.
   BOOL fOk = MoveFileEx(pszExisting, pszNew, MOVEFILE_DELAY_UNTIL_REBOOT);
   if (fOk) 
   	return(fOk);

   // If MoveFileEx failed, we are running on Windows 95 and need to add
   // entries to the WININIT.INI file (an ANSI file).
   // Start a new scope for local variables.   
   {   char szRenameLine[1024];   
   	   int cchRenameLine = wsprintfA(szRenameLine, 
#ifdef UNICODE
      "%ls=%ls\r\n", 
#else
      "%hs=%hs\r\n", 
#endif
      (pszNew == NULL) ? __TEXT("NUL") : pszNew, pszExisting);
      char szRenameSec[] = "[Rename]\r\n";
      int cchRenameSec = sizeof(szRenameSec) - 1;
      HANDLE hfile, hfilemap;
      DWORD dwFileSize, dwRenameLinePos;
      TCHAR szPathnameWinInit[_MAX_PATH];
      // Construct the full pathname of the WININIT.INI file.
      GetWindowsDirectory(szPathnameWinInit, _MAX_PATH);
      lstrcat(szPathnameWinInit, __TEXT("\\WinInit.Ini"));
      // Open/Create the WININIT.INI file.
      hfile = CreateFile(szPathnameWinInit,      
         GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 
         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      if (hfile == INVALID_HANDLE_VALUE)
      	return(fOk);
      // Create a file mapping object that is the current size of 
      // the WININIT.INI file plus the length of the additional string
      // that we're about to insert into it plus the length of the section
      // header (which we might have to add).
      dwFileSize = GetFileSize(hfile, NULL);
      hfilemap = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, 
         dwFileSize + cchRenameLine + cchRenameSec, NULL);
      if (hfilemap != NULL) 
      {
         // Map the WININIT.INI file into memory.  Note: The contents 
         // of WININIT.INI are always ANSI; never Unicode.
         LPSTR pszWinInit = (LPSTR) MapViewOfFile(hfilemap, 
            FILE_MAP_WRITE, 0, 0, 0);
         if (pszWinInit != NULL) 
         {
            // Search for the [Rename] section in the file.
            LPSTR pszRenameSecInFile = strstr(pszWinInit, szRenameSec);
            if (pszRenameSecInFile == NULL) 
            {
               // There is no [Rename] section in the WININIT.INI file.
               // We must add the section too.
               dwFileSize += wsprintfA(&pszWinInit[dwFileSize], "%s",
                                       szRenameSec);
               dwRenameLinePos = dwFileSize;
            } 
            else 
            {
               // We found the [Rename] section, shift all the lines down
               PSTR pszFirstRenameLine = strchr(pszRenameSecInFile, '\n');
               // Shift the contents of the file down to make room for 
               // the newly added line.  The new line is always added 
               // to the top of the list.
               pszFirstRenameLine++;   // 1st char on the next line
               memmove(pszFirstRenameLine + cchRenameLine, pszFirstRenameLine, 
                  pszWinInit + dwFileSize - pszFirstRenameLine);                  
               dwRenameLinePos = pszFirstRenameLine - pszWinInit;
          	}
            // Insert the new line
            memcpy(&pszWinInit[dwRenameLinePos], szRenameLine,cchRenameLine);
            UnmapViewOfFile(pszWinInit);
            // Calculate the true, new size of the file.
            dwFileSize += cchRenameLine;
            // Everything was successful.
            fOk = TRUE;
         }
         CloseHandle(hfilemap);
      }
      // Force the end of the file to be the calculated, new size.
      SetFilePointer(hfile, dwFileSize, NULL, FILE_BEGIN);
      SetEndOfFile(hfile);
      CloseHandle(hfile);
	}
	return(fOk);
}