// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SavCustom.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SavCustom.h"
#include "clientreg.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
 
#include <string>
#include <lmcons.h>
#include <lmshare.h>
#include <atlbase.h>

#define	READ_ACCESS			0x0001
#define	WRITE_ACCESS		0x0002

#define MBUFFER				512

#define	TM_OK				1
#define	TM_FALSE			0

#define REGKEY_CURRVER				_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion")
#define REGKEY_ADMIN_SECURITY		_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AdministratorOnly\\Security")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef NET_API_STATUS (_stdcall*NETADD)(LPSTR,DWORD,LPBYTE,LPDWORD);
typedef NET_API_STATUS (_stdcall*NETDEL)(LPSTR,LPSTR,DWORD);

#define DLL_REGISTER			"DllRegisterServer"
#define DLL_UNREGISTER			"DllUnregisterServer"

void            MSILogMessage( MSIHANDLE, LPCTSTR, BOOL bDoAction = FALSE );

/////////////////////////////////////////////////////////////////////////////
// Local function prototypes



//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CSavCustomApp

BEGIN_MESSAGE_MAP(CSavCustomApp, CWinApp)
	//{{AFX_MSG_MAP(CSavCustomApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSavCustomApp construction

CSavCustomApp::CSavCustomApp()
{
	// add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSavCustomApp object

CSavCustomApp theApp;


BOOL CSavCustomApp::InitInstance()
{
	return TRUE;
}


int CSavCustomApp::ExitInstance() 
{
	
	return CWinApp::ExitInstance();
}

#if 0

//////////////////////////////////////////////////////////////////////////
//
// Function: IsServerPrimary()
//
// Description: Look at the addresscache reg, load the flag key
//	then bitwise-and "&" it against the PF_PRIMARY flag to 
//	determine if the target server is running with a primary server
//	return BOOL TRUE or FALSE.
//
//////////////////////////////////////////////////////////////////////////
// 9/15/04 - Daniel Kowalyshyn function created.
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall IsServerPrimary ( LPSTR pstrPrimaryServerName )
{

	TCHAR szTemp [MAX_PATH+1];
	TCHAR szLog [MAX_PATH+1];
	HKEY  hKey = NULL;
	DWORD dwProductVersionFlag=0;
	BOOL bScsPrimary=FALSE;

	sssnprintf (szLog, sizeof(szLog), "IsServerPrimary pMultiStr %s", _T(pstrPrimaryServerName));
	OutputDebugString(szLog);

	sssnprintf (szTemp, sizeof(szTemp), "%s\\%s\\%s", _T(szReg_Key_Main), _T(szReg_Key_AddressCache), pstrPrimaryServerName);

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szTemp), 0, KEY_QUERY_VALUE, &hKey))
	{
		DWORD   dwSize = sizeof( dwProductVersionFlag );
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey,
										_T(szReg_Val_Flags), 
										NULL, 
										NULL, 
										(LPBYTE)&dwProductVersionFlag, 
										&dwSize ))
		{
			sssnprintf (szLog, sizeof(szLog), "IsServerPrimary VersionFlag %d", dwProductVersionFlag);
			OutputDebugString(szLog);
		}
		else
		{
			OutputDebugString("IsServerPrimary regread failed");
		}
		RegCloseKey(hKey);
	}

	if (dwProductVersionFlag & PF_PRIMARY)
	{
		bScsPrimary=TRUE;
		sssnprintf (szLog, sizeof(szLog), "IsServerPrimary %s is Primamry", _T(pstrPrimaryServerName));
		OutputDebugString(szLog);
	}
	else
	{
		bScsPrimary=FALSE;
		sssnprintf (szLog, sizeof(szLog), "IsServerPrimary %s is NOT Primary", _T(pstrPrimaryServerName));
		OutputDebugString(szLog);
	}

	return ( bScsPrimary );
}
#endif

#if 0
//////////////////////////////////////////////////////////////////////////
//
// Function: ServerHaveParent()
//
// Description: Look at the addresscache reg, and with the servername
//	see if it has a parent server. 
//	return BOOL TRUE or FALSE.
//
//////////////////////////////////////////////////////////////////////////
// 1/13/05 - Daniel Kowalyshyn function created.
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall ServerHaveParent ( LPSTR pstrServerName )
{

	TCHAR szTemp [MAX_PATH+1];
	TCHAR szLog [MAX_PATH+1];
	HKEY  hKey = NULL;
	BOOL bScsHaveParent=FALSE;

	sssnprintf (szLog, sizeof(szLog), "ServerHaveParent pMultiStr %s", _T(pstrServerName));
	OutputDebugString(szLog);

	sssnprintf (szTemp, sizeof(szTemp), "%s\\%s\\%s", _T(szReg_Key_Main), _T(szReg_Key_AddressCache), pstrServerName);
	OutputDebugString(szTemp);

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szTemp), 0, KEY_QUERY_VALUE, &hKey))
	{
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey,
										_T(szReg_Val_Parent), 
										NULL, 
										NULL, 
										NULL, 
										NULL ))
		{
			OutputDebugString("ServerHaveParent parent exists");
			bScsHaveParent=TRUE;
		}
		else
		{
			OutputDebugString("ServerHaveParent regread doesn't exist");
			bScsHaveParent=FALSE;
		}
		RegCloseKey(hKey);
	}
	return ( bScsHaveParent );
}
#endif

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
//////////////////////////////////////////////////////////////////////////
CString GetMSIProperty( MSIHANDLE hInstall, CString strProperty )
{
	CString strRet = "", strTemp;
	char *pCAData = NULL;
	DWORD dwLen = 0;	// This needs to be 0 so that we get ERROR_MORE_DATA error
	UINT nRet = 0;
   
	strProperty.LockBuffer();
   
	try
	{
		nRet = MsiGetProperty( hInstall, strProperty, "", &dwLen );
		if( ERROR_MORE_DATA != nRet )
		{
			strTemp.Format( "Error getting length of %s (nRet = %d)", strProperty, nRet );
			throw( strTemp );
		}

		// Allocate our memory and grab the data, add one for the trailing null
		pCAData = new char[ ++dwLen ];
		if( !pCAData )
			throw( CString("Unable to allocate memory for data buffer") );
		
		nRet = MsiGetProperty( hInstall, strProperty, pCAData, &dwLen );
		if( ERROR_SUCCESS != nRet )
		{
			strTemp.Format( "Unable to obtain the %s (nRet = %d)", strProperty, nRet );
			throw( strTemp );
		}

		// Save the value
		strRet.Format( "%s", pCAData );
		
		// Log the value to the MSI log file
		strTemp.Format( "%s=%s", strProperty, strRet );
		MSILogMessage( hInstall, (LPCTSTR) strTemp );
	}
	catch( CString strErr )
	{
		MSILogMessage( hInstall, (LPCTSTR) strErr );
		strRet = "";
	}

	strProperty.UnlockBuffer();

	if( pCAData )
		delete [] pCAData;

	return strRet;
}
