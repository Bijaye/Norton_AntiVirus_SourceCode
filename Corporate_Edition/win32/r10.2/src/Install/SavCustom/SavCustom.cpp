// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SavCustom.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SavCustom.h"
#include "TransmanClass.h"
#include "appinfo.h"
#include "regkey.h"
#include "dompass.h"
#include "domains.h"
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

/////////////////////////////////////////////////////////////////////////////
// Local function prototypes

// Global Transman.dll pointer
LPTM g_pTrans = NULL;


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
	
	CleanupDomains();

	g_pTrans = NULL;
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function: TMInit
//
// Description: 
// Wrapper function to start transman services during install.
//
////////////////////////////////////////////////////////////////////////////
BOOL __stdcall TMInit ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(ERROR_FILE_NOT_FOUND == StartTransman(g_pTrans,COINIT_APARTMENTTHREADED,NULL,(FARPROC)ReturnPong,TRUE))
		return (FALSE);

	return (TRUE);
	
}

LONG __stdcall TMFindComputer (TCHAR* lpServer, bool bIsNT)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD dwResult = 0;
	CString cstrAppName;
	CString cstrErrMsg;

	if ( NULL != g_pTrans ) 
		{
		TRY
			{
			dwResult = g_pTrans->FindComputer (lpServer, bIsNT);
			}

		CATCH_ALL (e)
			{
			cstrAppName.LoadString(IDS_INSTALL_APP);
			cstrErrMsg.LoadString(IDS_ERROR_SERVER_NOT_FOUND);
			::MessageBox(NULL, cstrErrMsg, cstrAppName, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK);
			}
		END_CATCH_ALL
		}

	return dwResult;
}

LONG __stdcall TMRegisterOCX(LPSTR szInput, LPSTR szName, BOOL bRegister)
{
	HINSTANCE hLibrary;
	FARPROC lpDllEntryPoint;
	CString sRegister, sDLLname = ".\\";
	BOOL dwResult = TRUE;
	TCHAR szCurrDir [MAX_PATH];

    // Save the current dir, and change to the NAV dir so we can load
    // up the DLLs.
    GetCurrentDirectory ( MAX_PATH, szCurrDir );
    SetCurrentDirectory ( szInput );

	if (bRegister) sRegister = DLL_REGISTER;
	else sRegister = DLL_UNREGISTER;
	
	sDLLname += szName;
	
	hLibrary = LoadLibrary(sDLLname);
	if (hLibrary) 
	{
		lpDllEntryPoint = GetProcAddress(hLibrary, sRegister);
		if (lpDllEntryPoint) 
		{
			dwResult = (DWORD)(*lpDllEntryPoint)();
		}
		else 
		{
			dwResult = GetLastError();
		}
		if (!FreeLibrary(hLibrary))
			dwResult = GetLastError();
	}
	else
		dwResult = GetLastError();
    SetCurrentDirectory ( szCurrDir );
	return dwResult;
}

DWORD __stdcall TMFindAllComputers ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   	DWORD dwResult = 0;
	CString cstrAppName;
	CString	cstrErrMsg;
   
   	bool bIntense = false;
   	
   	if ( NULL != g_pTrans )
   		{
   		TRY
   			{
   			dwResult = g_pTrans->FindAllComputers ( bIntense );
   			}
   
   		CATCH_ALL (e)
   			{
   			cstrAppName.LoadString(IDS_INSTALL_APP);
			cstrErrMsg.LoadString(IDS_ERROR_SERVERS_NOT_FOUND);
   			::MessageBox(NULL, cstrErrMsg, cstrAppName, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK);
   			}
   		END_CATCH_ALL
   		}
   
   	return dwResult;
}

void __stdcall TMStartDomains ()
{
	////////////////////////////////////////////////////////
	CString cstrAppName;
	CString	cstrErrMsg;
	if ( NULL != g_pTrans )
   		{
   		TRY
   			{
   			g_pTrans->StartDomain();
   			}
   
   		CATCH_ALL (e)
   			{
			// Unable to build domain list.
   			cstrAppName.LoadString(IDS_INSTALL_APP);
			cstrErrMsg.LoadString(IDS_ERROR_UNABLE_BUILD_DOMAIN_LIST);
   			::MessageBox(NULL, cstrErrMsg, cstrAppName, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK);
   			}
   		END_CATCH_ALL
   		}
   
	///////////////////////////////////////////////////////
}

void __stdcall TMCleanupDomains ()
{
	DWORD dwResult = 0;
	CString cstrAppName;
	CString	cstrErrMsg;
	if ( NULL != g_pTrans )
   		{
   		TRY
   			{
   			g_pTrans->CleanupDomain();
   			}
   
   		CATCH_ALL (e)
   			{
			// Unable to cleanup domain list.
   			cstrAppName.LoadString(IDS_INSTALL_APP);
			cstrErrMsg.LoadString(IDS_ERROR_UNABLE_CLEANUP_DOMAIN_LIST);
   			::MessageBox(NULL, cstrErrMsg, cstrAppName, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK);
   			}
   		END_CATCH_ALL
   		}
   
}

BOOL __stdcall TMVerifyDomain(LPSTR szInput, LPSTR szOutput)
{
	CString sDomain = szInput, sParent;
	BOOL bret = TRUE;
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (AlreadyInDomains(&sDomain, &sParent))
	{
		if (!VerifyServerEngine(sDomain))
		{
			bret = FALSE;
		}
		else strcpy(szOutput, sParent.GetBuffer(256));
	}

	return bret;
}

DWORD __stdcall TMStopTransman ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	DWORD	dwResult = 0;
	CString	cstrAppName;
	CString	cstrErrMsg;

	if ( NULL != g_pTrans )
	{
		TRY
		{
			dwResult = StopTransman ( g_pTrans );
		}

		CATCH_ALL (e)
		{
			cstrAppName.LoadString ( IDS_INSTALL_APP );
			cstrErrMsg.LoadString ( IDS_ERROR_STOP_TRANSMAN );
			::MessageBox ( NULL, cstrErrMsg, cstrAppName, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK );
		}
		END_CATCH_ALL
	}

	return dwResult;
}

DWORD __stdcall TMFreeTransman ()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD	dwResult = 0;
	CString	cstrAppName;
	CString	cstrErrMsg;

	TRY
	{
		dwResult = FreeTransman ();
	}

	CATCH_ALL (e)
	{
		cstrAppName.LoadString ( IDS_INSTALL_APP );
		cstrErrMsg.LoadString ( IDS_ERROR_FREE_TRANSMAN );
		::MessageBox ( NULL, cstrErrMsg, cstrAppName, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK );
	}
	END_CATCH_ALL

	return dwResult;
}

DWORD __stdcall InitDomains()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	StartDomains();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: TMIsServerSecure()
//
// Description: Look at the addresscache reg, load the flag2 key
//	then bitwise-and "&" it against the PF2_SECURE_COMM_ENABLED flag to 
//	determine if the target server supports secure SSL communications and
//	return BOOL TRUE or FALSE.
//
//////////////////////////////////////////////////////////////////////////
// 9/9/04 - Daniel Kowalyshyn function created.
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall TMIsServerSecure (LPSTR pstrPrimaryServerName )
{

	TCHAR szTemp [MAX_PATH+1];
	TCHAR szLog [MAX_PATH+1];
	HKEY  hKey = NULL;
	DWORD dwProductVersionFlag=0;
	BOOL bScsSecureCommsEnabled=FALSE;

	sssnprintf (szLog, sizeof(szLog), "TMIsServerSecure pMultiStr %s", _T(pstrPrimaryServerName));
	OutputDebugString(szLog);

	sssnprintf (szTemp, sizeof(szTemp), "%s\\%s\\%s", _T(szReg_Key_Main), _T(szReg_Key_AddressCache), pstrPrimaryServerName);

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szTemp), 0, KEY_QUERY_VALUE, &hKey))
	{
		DWORD   dwSize = sizeof( dwProductVersionFlag );
		if (ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey,
										_T(szReg_Val_Flags2), 
										NULL, 
										NULL, 
										(LPBYTE)&dwProductVersionFlag, 
										&dwSize ))
		{
			sssnprintf (szLog, sizeof(szLog), "TMIsServerSecure VersionFlag %d", dwProductVersionFlag);
			OutputDebugString(szLog);
		}
		else
		{
			OutputDebugString("TMIsServerSecure regread failed");
		}
		RegCloseKey(hKey);
	}

	if (dwProductVersionFlag & PF2_SECURE_COMM_ENABLED)
	{
		bScsSecureCommsEnabled=TRUE;
		sssnprintf (szLog, sizeof(szLog), "TMIsServerSecure %s is Secure", _T(pstrPrimaryServerName));
		OutputDebugString(szLog);
	}
	else
	{
		bScsSecureCommsEnabled=FALSE;
		sssnprintf (szLog, sizeof(szLog), "TMIsServerSecure %s is NOT Secure", _T(pstrPrimaryServerName));
		OutputDebugString(szLog);
	}

	return ( bScsSecureCommsEnabled );
}



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

//////////////////////////////////////////////////////////////////////////
//
// Function: ServerExistsinAddressCache()
//
// Description: Look at the addresscache reg and if the server exists 
//	then return immedately or wait for a few seconds incase transman is 
//  populating it. 
//	return BOOL TRUE or FALSE.
//
//////////////////////////////////////////////////////////////////////////
// 1/26/05 - Daniel Kowalyshyn function created.
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall ServerExistsinAddressCache ( LPSTR pstrServerName )
{
	TCHAR szTemp [MAX_PATH+1];
	TCHAR szLog [MAX_PATH+1];
	HKEY  hKey = NULL;
	BOOL bServerInCache=FALSE;
	BOOL bTimeOut=FALSE;
	INT dCount=0;
	INT dTimeOutCount=10;

	sssnprintf (szLog, sizeof(szLog), "ServerExistsinAddressCache pMultiStr %s", _T(pstrServerName));
	OutputDebugString(szLog);

	sssnprintf (szTemp, sizeof(szTemp), "%s\\%s\\%s", _T(szReg_Key_Main), _T(szReg_Key_AddressCache), pstrServerName);
	OutputDebugString(szTemp);
	// Loop until parent found in addresscache or timeout reached
	while ( bTimeOut!=TRUE )
	{
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szTemp), 0, KEY_QUERY_VALUE, &hKey))
		{
			OutputDebugString("ServerExistsinAddressCache parent exists");
			bServerInCache=TRUE;
			bTimeOut = TRUE;
		}
		else
		{
			OutputDebugString("ServerExistsinAddressCache regread doesn't exist");
			bServerInCache=FALSE;
		}
		RegCloseKey(hKey);
		// sleep & timer / timeout mechanism
		if ( (dCount < dTimeOutCount) && (bTimeOut != TRUE) )
		{
			OutputDebugString("ServerExistsinAddressCache: Sleep 6 seconds.");
			Sleep( 6000 );
		}
		else
		{
			bTimeOut = TRUE;
		}
		dCount++;
		sssnprintf (szLog, sizeof(szLog), "ServerExistsinAddressCache dCount %d", dCount);
		OutputDebugString(szLog);
	}
	return ( bServerInCache );
}



