// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SavRoam.cpp : Defines the entry point for the console application.
//

//#define INITIIDS

#define _WINSOCKAPI_

#include "stdafx.h"
#include "resource.h"
#include "tchar.h"
#include "windows.h"
#include "stdio.h"
#include "direct.h"
#include "shfolder.h"
#include "atlconv.h"

#define INCL_WINSOCK_API_TYPEDEFS 1

#include "winsock2.h"
#include "wininet.h"
#include "process.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "finetime.h"

#ifdef __cplusplus
}
#endif

#include "SAVRoamM.h"       // message definitions

#include <io.h>
#include <errno.h>

// NOTE: don't change this, since that will whack all the resolutions of
// my registry values! They all assume a millisecond counter.
#define FINERESOLUTION MILLISECONDS
#include "FineTimeImplementation.c"

#define MAX_SERVER_NAME	48		// limit imposed by LDVP technology

#define DONTEXTERNTRANSMAN 1
#include "transman.h"
#include "ClientReg.h"

#define SAVROAM_DEFINE 1		// define the globals in this file
#include "savroam.h"
#include "roam.h"

#include "commisc.cpp"

//RBo.Begin
#include "ScsCommsUtils.h"
using namespace ScsSecureComms;
//RBo.End

static BOOL g_bWin2000plus = FALSE;

static HINSTANCE g_hKernel32 = NULL;
static HINSTANCE g_hAdvapi32 = NULL;

// Win2000+ specific functions

typedef SC_HANDLE (WINAPI* LPFNOPENSCMANAGER)(LPCTSTR lpMachineName, LPCTSTR lpDatabaseName, DWORD dwDesiredAccess);
typedef SC_HANDLE (WINAPI* LPFNCREATESERVICE)(SC_HANDLE hSCManager, LPCTSTR lpServiceName, LPCTSTR lpDisplayName, DWORD dwDesiredAccess, DWORD dwServiceType, DWORD dwStartType, DWORD dwErrorControl, LPCTSTR lpBinaryPathName, LPCTSTR lpLoadOrderGroup, LPDWORD lpdwTagId, LPCTSTR lpDependencies, LPCTSTR lpServiceStartName, LPCTSTR lpPassword);
typedef BOOL (WINAPI* LPFNDELETESERVICE)(SC_HANDLE hService);
typedef BOOL (WINAPI* LPFNQUERYSERVICESTATUS)(SC_HANDLE hService, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNSTARTSERVICE)(SC_HANDLE hService, DWORD dwNumServiceArgs, LPCTSTR *lpServiceArgVectors);
typedef BOOL (WINAPI* LPFNSTARTSERVICECTRLDISPATCHER)(LPSERVICE_TABLE_ENTRY lpServiceStartTable);
typedef SERVICE_STATUS_HANDLE (WINAPI* LPFNREGISTERSERVICECTRLHANDLER)(LPCTSTR lpServiceName, LPHANDLER_FUNCTION lpHandlerProc);
typedef BOOL (WINAPI* LPFNCONTROLSERVICE)(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNSETSERVICESTATUS)(SERVICE_STATUS_HANDLE hServiceStatus, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNCLOSESERVICEHANDLE)(SC_HANDLE hSCObject);

// dwr, 1-228798, set Description field for Service Manager
typedef BOOL (WINAPI* LPFNCHANGESERVICECONFIG2)(SC_HANDLE hService, DWORD dwInfoLevel, LPVOID lpInfo);
 
static LPFNOPENSCMANAGER lpfnOpenSCManager = NULL;
static LPFNCREATESERVICE lpfnCreateService = NULL;
static LPFNDELETESERVICE lpfnDeleteService = NULL;
static LPFNQUERYSERVICESTATUS lpfnQueryServiceStatus = NULL;
static LPFNSTARTSERVICE lpfnStartService = NULL;
static LPFNSTARTSERVICECTRLDISPATCHER lpfnStartServiceCtrlDispatcher = NULL;
static LPFNREGISTERSERVICECTRLHANDLER lpfnRegisterServiceCtrlHandler = NULL;
static LPFNCONTROLSERVICE lpfnControlService = NULL;
static LPFNSETSERVICESTATUS lpfnSetServiceStatus = NULL;
static LPFNCLOSESERVICEHANDLE lpfnCloseServiceHandle = NULL;

// dwr, 1-228798, set Description field for Service Manager
static LPFNCHANGESERVICECONFIG2 lpfnCreateServiceConfig2 = NULL;

static DWORD s_dwTransmanHandle = 0;
static ScsSecureComms::IAuthPtr *s_pAnonymousAuth = NULL;
static HMODULE s_hmScsComms = NULL;

static BOOL s_bRunningAsService = FALSE;
static BOOL s_bVerbose = FALSE;
static BOOL s_bDebug = FALSE;

#define MAX_SHUTDOWN_WAIT 30
DWORD gdwShutdownWait = 0;

void OutputLine( char *szLine, FILE *fFile, BOOL bDetail );
void OutputDebugLine( char *szLine, FILE *fFile, BOOL bDetail );
void GetLastErrorString( LPTSTR *lplpMsgBuf );

static DWORD InitRegKeys( )
{
    DWORD cc = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
    if ( cc != ERROR_SUCCESS )
        return ERROR_NO_KEY;

    RegOpenKey(hMainKey, "ProductControl", &hProductControlKey);

    return 0;
}

void DeInitRegKeys( )
{
    RegCloseKey( hMainKey );
	RegCloseKey( hProductControlKey );
}


//------------------------------------------------------------------------
//  GetSendInfoCallback
//
// Call back to get information about the requested computer. Either the
// addr or the szComputerName will contain the computer referencing data.
//
// Parameters:
//   client_data       [in]     This is the parameter specified as client_data
//                              to the RegisterGetSendInfoCallback().
//   addr              [in/out] Optional. If szComputerName is empty string, the callback
//                              uses this in order to find the specific remote
//                              computer SendCOM needs more info about. If
//                              szComputerName is a non-empty string, this gets
//                              filled in with the address info found for the
//                              specified szComputerName.
//   szComputerName    [in/out] Optional. If this is a non-empty string, the callback 
//                              uses this when looking up the matching computer.
//                              If this is an empty string, addr will be used
//                              when looking up the matching computer and this
//                              receives the matching computer name as a string.
//   nComputerNameBytes   [in]  Total number of bytes in szComputerName buffer.
//   ppAuth               [out] Optional. NULL specification indicates that
//                              the caller not interestd in getting an IAuth
//                              back. When non-NULL, this routine will update
//                              this pointer to refer to the IAuth to use when
//                              communicating to this addr or szComputerName
//                              specified computer. Caller must call (*ppAuth)->
//                              Release() in order to release its reference when
//                              done. A *ppAuth == NULL output must be handled 
//                              correctly by the caller on function return.
//   pbSecureCommsEnabled [out] Optional. If non-NULL, SendCOM use a secure
//                              communication channel (SSL) when true and the
//                              legacy insecure communication channel when false.
//   pbUseMyHint     [out]      Required.  Set to true if the caller should
//                              use the value in *pbSecureCommsEnabled as
//                              a hint; otherwise no hint may be provided.
//
// Returns:
//   ERROR_SUCCESS            on success
//   ERROR_COMPUTER_NOT_FOUND when a computer associated with addr or
//                            szComputerName could not be found.
//   ERROR_MISSING_IAUTH      Computer was found, but there is no IAuth
//                            associated with it. This must only be returned
//                            when pAuth is non-NULL.
//   ERROR_*                  Some other error occurred.
//------------------------------------------------------------------------

DWORD GetSendInfoCallback(void						*client_data,
						  CBA_Addr					*addr,
                          char						*szComputerName,
                          unsigned long				nComputerNameBytes,
                          ScsSecureComms::IAuth		**ppAuth,
                          bool						*pbSecureCommsEnabled,
						  bool						*pbUseMyHint)
{
	*pbSecureCommsEnabled = true;
	*pbUseMyHint = true;

	BOOL bSetAuth = FALSE;
 
 	//OutputLine( "In GetSendInfoCallback\n", NULL, TRUE );
 
	if (ppAuth)
	{
		*ppAuth = NULL;

		if (s_pAnonymousAuth == NULL)
			s_pAnonymousAuth = new ScsSecureComms::IAuthPtr;

		if (*s_pAnonymousAuth == NULL)
		{
			SYMRESULT symres = g_objAuthLoginLdr.CreateObject (*s_pAnonymousAuth);
			if (SYM_FAILED (symres))
			{
				return ERROR_SECURE_COMMS;
			}

 			//
 			// NMCI needs insecure communication to bootstrap from client to server
 			//
 			DWORD lResult = 0;
 			DWORD dwRoamBootstrapUnAuth = 0;
 			DWORD dwRoamBootstrapUnAuthSize = sizeof(DWORD);
 			DWORD dwType = REG_DWORD;
 			lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("RoamBootstrap"), NULL, &dwType, (unsigned char *)&dwRoamBootstrapUnAuth, &dwRoamBootstrapUnAuthSize );
             if ( ERROR_SUCCESS == lResult )
             {
 				if (dwRoamBootstrapUnAuth)
 				{
 					//OutputLine( "Doing RoamBootStrap comm\n", NULL, TRUE );
 					SYMRESULT commsres = (*s_pAnonymousAuth)->SetAuthTypeRemote(IAuth::NAME_AUTH);
 					if (SYM_FAILED (commsres))
 					{
 						return ERROR_SECURE_COMMS;
 					}
 					
 					//OutputLine( "Success Set RoamBootStrap comm\n", NULL, TRUE );
 
 					bSetAuth = TRUE;
 				}
 			}
 
 			if (!bSetAuth)
   			{
 				//OutputLine( "Doing normal comm\n", NULL, TRUE );
 
 				SYMRESULT commsres = (*s_pAnonymousAuth)->SetAuthTypeRemote(IAuth::ROOT_AUTH);
 				if (SYM_FAILED (commsres))
 				{
 					return ERROR_SECURE_COMMS;
 				}
 
 				//OutputLine( "Success Set normal comm\n", NULL, TRUE );
 				bSetAuth = TRUE;
   			}
		}

		(*s_pAnonymousAuth).AddRef();
		*ppAuth = *s_pAnonymousAuth;
	}
	 
	if (!bSetAuth)
 	{
 		//OutputLine("Auth empty\n", NULL, TRUE);
 		if (*s_pAnonymousAuth)
 		{
 			//OutputLine("Anoynomous Auth NOT\n", NULL, TRUE);
 
 			DWORD lResult = 0;
 			DWORD dwRoamBootstrapUnAuth = 0;
 			DWORD dwRoamBootstrapUnAuthSize = sizeof(DWORD);
 			DWORD dwType = REG_DWORD;
 			lResult = SymSaferRegQueryValueEx( hProductControlKey, _T("RoamBootstrap"), NULL, &dwType, (unsigned char *)&dwRoamBootstrapUnAuth, &dwRoamBootstrapUnAuthSize );
             if ( ERROR_SUCCESS == lResult )
             {
 				if (dwRoamBootstrapUnAuth)
 				{
 					//OutputLine( "Doing RoamBootStrap comm\n", NULL, TRUE );
 					SYMRESULT commsres = (*s_pAnonymousAuth)->SetAuthTypeRemote(IAuth::NAME_AUTH);
 					if (SYM_FAILED (commsres))
 					{
 						return ERROR_SECURE_COMMS;
 					}
 					
 					//OutputLine( "Success Set RoamBootStrap comm\n", NULL, TRUE );
 
 				}
 			}
 		}
 	}
 
 	//OutputLine( "Out GetSendInfoCallback\n", NULL, TRUE );

	return ERROR_SUCCESS;
}

DWORD StartTransman()
{
	DWORD err = -1;
	if (err=LoadTransman() != ERROR_SUCCESS)
		return err;
	s_dwTransmanHandle = InitTransman(NULL,NULL,NULL);
	return 0;
}


DWORD StopTransman()
{
	DWORD err=0;
	if (s_dwTransmanHandle)
		err = DeinitTransman(s_dwTransmanHandle);
	s_dwTransmanHandle = 0;
	return err;
}

static DWORD DeinitCommunications()
{
	UnLoadScsComms();
	StopTransman();
	CoUninitialize();
    return 0;
}

static DWORD InitCommunications()
{
	TCHAR szAppPath[MAX_PATH] = {0};
	DWORD dwResult = ERROR_GENERAL;

	OutputDebugString("SavRoam: initializing communications\n");

	// Set trusted roots dir.
    GetHomeDir(szAppPath, sizeof(szAppPath));

	OutputDebugString("SavRoam: initializing COM\n");
	if (CoInitializeEx(NULL, COINIT_MULTITHREADED) != S_OK)
		return ERROR_GENERAL;

	OutputDebugString("SavRoam: starting Transman\n");
	dwResult = StartTransman();
    
	if (ERROR_SUCCESS == dwResult)
    {
		USES_CONVERSION;

		TCHAR szOutput[MAX_PATH+100];
		sssnprintf(szOutput, sizeof(szOutput), "SavRoam: loading certs from: %s\n", szAppPath);
		OutputDebugString(szOutput);

		// Load our trusted root certificates.
		SetTrustedRootsInDir(T2A(szAppPath));

		// Load ScsComms.
		OutputDebugString("SavRoam: loading ScsComms\n");
		dwResult = LoadScsComms();
		if (dwResult != SYM_OK)
		{
			DeinitCommunications();
			OutputDebugString("SavRoam: communication initialization FAILED!\n");
			return ERROR_SECURE_COMMS;
		}

		// Register a callback for Transman to call when it needs credentials.
		RegisterGetSendInfoCallback(GetSendInfoCallback, NULL);
	}
	else
	{
		DeinitCommunications();
		OutputDebugString("SavRoam: Transman initialization FAILED!\n");
		return ERROR_SECURE_COMMS;
	}

	OutputDebugString("SavRoam: communication initialization SUCCEEDED!\n");
    return ERROR_SUCCESS;
}

// begin here

void DbgOut( char *szString )
{
    char szLine[1024];
    sssnprintf( szLine, sizeof(szLine), _T("%d - %s\n"), GetFineLinearTime( ), szString );
    OutputDebugString( szLine );
}


void OutputLine( char *szLine, FILE *fFile, BOOL bDetail )
{
    if ( s_bVerbose  || ! bDetail || s_bRunningAsService )
    {
        if ( s_bRunningAsService )
        {
            char szOutput[1024];
            sssnprintf( szOutput, sizeof(szOutput), _T("%s"), szLine );
            DbgOut( szOutput );
        }
        else
        {
            _tprintf( _T("%s"), szLine );
        }

        if ( fFile )
        {
            _ftprintf( fFile, _T("%s"), szLine );
        }
    }
}

void OutputDebugLine( char *szLine, FILE *fFile, BOOL bDetail )
{
    (void) szLine;
    (void) fFile;
    (void) bDetail;

#ifdef _DEBUG

    OutputLine( szLine, fFile, bDetail );

    FILE *fLog = fopen( "savroam.log", "a+t" );
    if (fLog)
    {
        _ftprintf( fLog, _T("%d %s"), GetFineLinearTime( ), szLine );

        fclose( fLog );
    }

#endif

}

int DisplayLastErrorMessageBox( LPTSTR szError )
{
    LPSTR   lpszError;
    TCHAR   szBuiltErrorMsg[1024];

    GetLastErrorString( &lpszError );
    if ( lpszError )
    {
        sssnprintf( szBuiltErrorMsg, sizeof (szBuiltErrorMsg), szError, lpszError, GetLastError( ) );
        LocalFree( lpszError );
    }
    else
    {
		LoadString(NULL, IDS_NO_ERROR, szBuffer, MAX_BUFFER);
        sssnprintf( szBuiltErrorMsg, sizeof (szBuiltErrorMsg), szError, szBuffer, GetLastError( ) );
    }

	LoadString(NULL, IDS_ERROR, szBuffer, MAX_BUFFER);
    MessageBox (NULL, szBuiltErrorMsg, szBuffer, MB_OK);

    return FALSE;
}

int DisplayLastError( LPTSTR szError )
{
    LPSTR   lpszError;
    TCHAR   szBuiltErrorMsg[MAX_BUFFER];
	TCHAR	szReason[MAX_BUFFER];

    GetLastErrorString( &lpszError );
    if ( lpszError )
    {
		LoadString(NULL, IDS_ERROR_REASON, szReason, MAX_BUFFER);
        sssnprintf( szBuiltErrorMsg, sizeof (szBuiltErrorMsg), szReason, szError, lpszError, GetLastError( ) );
        LocalFree( lpszError );
    }
    else
    {
		LoadString(NULL, IDS_NO_ERROR2, szReason, MAX_BUFFER);
        sssnprintf( szBuiltErrorMsg, sizeof (szBuiltErrorMsg), szReason, szError, GetLastError( ) );
    }

    _tprintf(szBuiltErrorMsg);

    return FALSE;
}

//Added an extra parameter to decide if we should or not display the windows error
//this is so that when we get an error from our functions we do not want use
//getlasterror to display microsoft error messages
// void ReportLastError( char *szMsg, FILE *fFile, BOOL bGetLastError = TRUE);

void ReportLastError( char *szMsg, FILE *fFile, BOOL bGetLastError )
{
    LPVOID lpMsgBuf;
    char szLine[1024];
	char szError[1024];

    
	if(bGetLastError)
	{
		FormatMessage(
                 FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL,
                 GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 (LPTSTR) &lpMsgBuf,
                 0,
                 NULL );
		LoadString(NULL, IDS_ERROR, szError, MAX_BUFFER);
		sssnprintf( szLine, sizeof(szLine), _T("%s - %s (%x) %s\n"), szMsg, szError, GetLastError( ), (LPCTSTR)lpMsgBuf );
	}
	else
		sssnprintf( szLine, sizeof(szLine), _T("%s \n"), szMsg);
    OutputLine( szLine, fFile, FALSE );

    LocalFree( lpMsgBuf );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: DirectoryExists
//
// Description  : See if the specified directory exists
//
// Parameters   : LPTSTR lpDir - the directory to find
//
// Return Values: TRUE if the dirctory exists
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL DirectoryExists(LPTSTR lpDir)
{
    BOOL            bDirExists = FALSE;
    HANDLE          hDir = NULL;
    WIN32_FIND_DATA finddata = {0};


    hDir = FindFirstFile( lpDir, &finddata );

    if ( hDir && hDir != INVALID_HANDLE_VALUE )
    {
        bDirExists = TRUE;
        FindClose( hDir );
    }

    return bDirExists;
}

typedef HRESULT (WINAPI *PFNSHGetFoldPathA)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
#define SHARED_VERSION_DATA_DIRECTORY   _T("7.5")

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CreateUserDataPath
//
// Description  : If the path returned by SHGetFolderPath(CSIDL_LOCAL_APPDATA)
//                fails to get the directory for the current user, then we build
//                the path here.
//                
//                The buffer must contain the string returned by SHGetFolderPath.
//
// Return Values: 
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
static BOOL CreateUserDataPath(LPTSTR lpUserDataDir, DWORD nNumUserDataDirBytes)
{
	char	szCurrentUserName[256];
    TCHAR   szTemp[MAX_PATH] = {0};
    TCHAR   szSubDir[MAX_PATH] = {0};
    LPTSTR  lpTemp1 = NULL;
    LPTSTR  lpTemp2 = NULL;
    BOOL    bRet = S_FALSE;

    // Do we have a valid buffer?
    if ( !lpUserDataDir )
    {
        return bRet;
    }

    // Does the string contain "Default User?
    lpTemp1 = _tcsstr( lpUserDataDir, _T("Default User") );

    if ( !lpTemp1 )
    {
        return bRet;
    }

    DWORD dwUserNameSize = sizeof(szCurrentUserName);

    GetUserName( szCurrentUserName, &dwUserNameSize );

    // See if we have the name of the current user
    if ( !_tcslen(szCurrentUserName) )
    {
        return bRet;
    }

    // Copy the string to our buffer
    vpstrncpy( szTemp, lpUserDataDir, sizeof (szTemp));

    // Point to "Default User" in the temp buffer
    lpTemp1 = _tcsstr( szTemp, _T("Default User") );

    // Find the rest of the string
    lpTemp2 = _tcschr( lpTemp1, _T('\\') );

    // Save off the rest of the string for later
    vpstrncpy( szSubDir, lpTemp2, sizeof (szSubDir) );

    // Terminate the string right there
    *lpTemp1 = '\0';

    // Append the name of the current user
    vpstrnappend (szTemp, szCurrentUserName, sizeof (szTemp));

    // Copy the rest of the callers buffer to 
    // our temp buffer.
    vpstrnappend (szTemp, szSubDir, sizeof (szTemp));

    // Does the directory exist?
    if ( DirectoryExists(szTemp) )
    {
        // Yep. Copy it to the callers buffer.
        vpstrncpy( lpUserDataDir, szTemp, nNumUserDataDirBytes );

        bRet = S_OK;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetHomeDir
//
// Description  : Get the data for the HomeDir reg key. The normal getval()
//                method may npt always be available.
//
// Parameters:    [out] - LPTSTR lpHomeDir - buffer to receive the value 
//                               of the key
//                [in]  - DWORD dwHomeSize - size of the above buffer
//  
// Return Values: Standard error codes.
//
///////////////////////////////////////////////////////////////////////////////
// 3/19/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LRESULT GetHomeDir(LPTSTR lpHomeDir, DWORD dwHomeSize)
{
    DWORD           dwSize = dwHomeSize;
    LRESULT         lRet = ERROR_SUCCESS;
    HKEY            hKey = NULL;

    lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szReg_Key_Main, 0, KEY_READ, &hKey );

    if ( ERROR_SUCCESS == lRet )
    {
        lRet = SymSaferRegQueryValueEx(hKey, szReg_Val_HomeDir, NULL, 
                               NULL, (LPBYTE)lpHomeDir, &dwSize);

        RegCloseKey( hKey );
    }

    return lRet;
}

//////////////////////////////////////////////////////////////////
//
// Function:    GetAppDataDirectory
//
// Description: Get SAV's data directory.
//
// Params:      [in]     - DWORD  dwFlags - See pscan.h
//              [in/out] - LPTSTR lpDataDir
//              [in]     - DWORD  nDataDirBytes
//
// Returns:     Standard error codes.
// 
//////////////////////////////////////////////////////////////////
// 03/03/2000   TCASHIN Function Created
// 08/08/2005   DAVEVW Modified to be more string safe
//////////////////////////////////////////////////////////////////
DWORD GetAppDataDirectory(DWORD dwFlags, LPTSTR lpDataDir, DWORD nDataDirBytes)
{
    DWORD   dwError = ERROR_SUCCESS;

    auto PFNSHGetFoldPathA pfnSHGetFolderPathA = NULL;

    auto TCHAR          szAppData[MAX_PATH] = {0};
    auto TCHAR          szSystemDir[MAX_PATH] = {0};
    auto TCHAR          szSHFolderDLL[MAX_PATH] ={0};
    auto HINSTANCE      hFolderDLL = NULL;
    auto HRESULT        hr = S_OK;
    auto int            len = 0;

    // A little error checking...
    if ( (dwFlags != SAV_COMMON_APP_DATA  &&
          dwFlags != SAV_USER_APP_DATA)   ||
         lpDataDir == NULL )
    {
        return ERROR_INVALID_PARAMETER;
    }

    // Get the system directory
    if ( GetSystemDirectory(szSystemDir, sizeof(szSystemDir)) )
    {
        // Append the DLL name
        sssnprintf ( szSHFolderDLL, sizeof (szSHFolderDLL), _T("%s\\shfolder.dll"), szSystemDir );

        // Load it.
        hFolderDLL = LoadLibrary( szSHFolderDLL );

        if ( hFolderDLL )
        {
            // Get the function
            pfnSHGetFolderPathA = (PFNSHGetFoldPathA)GetProcAddress( hFolderDLL, _T("SHGetFolderPathA") );

            if ( pfnSHGetFolderPathA )
            {
                // Call it.
                if ( dwFlags == SAV_COMMON_APP_DATA )
                {
                    // Per machine, all users, non-roaming, set up at install
                    hr = pfnSHGetFolderPathA( NULL, CSIDL_COMMON_APPDATA, NULL, 0, szAppData);
                }
                else
                {
					// Win2K+, Per machine, specific user, non-roaming, create it here
					hr = pfnSHGetFolderPathA( NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szAppData);
                }

                if ( SUCCEEDED(hr) )
                {
                    // Did we get a path for the "Default User"
                    if ( _tcsstr(szAppData, _T("Default User")) )
                    {
                        // Yes, but we don't write there. Go create the one we need
                        CreateUserDataPath( szAppData, sizeof (szAppData) );
                    }

                    // Make the string for the SAV data directory
                    len = sssnprintf( lpDataDir, nDataDirBytes, "%s\\%s\\%s",szAppData, 
                                    SYMANTEC_COMMON_DATA_DIR, 
                                    SHARED_VERSION_DATA_DIRECTORY );
                }
                else
                {
                    char szLine[512];

					LoadString(NULL, IDS_FUNCTION_FAILED_WITH_HRESULT, szBuffer, MAX_BUFFER);
					sssnprintf(szLine, sizeof (szLine), szBuffer, _T("GetAppDataDirectory: SHGetFolderPath()"));
                    OutputLine( szLine, NULL, FALSE );
                    dwError = ERROR_NO_PATH1;
                }
            }

            FreeLibrary( hFolderDLL );
        }
        else
            dwError = GetLastError();
    }
    else
        dwError = GetLastError();

    return dwError;
}

//============================================================================
DWORD _InstallDriver(IN SC_HANDLE SchSCManager,IN LPSTR DriverName,IN LPSTR ServiceExe,LPSTR Display,DWORD type,DWORD start,char *group,char *dep)
{
    SC_HANDLE  schService;
    DWORD      err;

    schService=lpfnCreateService(SchSCManager,          // SCManager database
                             DriverName,            // name of service
                             Display,               // name to display
                             SERVICE_ALL_ACCESS,    // desired access
                             type,                  // service type
                             start,                 // SERVICE_DEMAND_START,      // start type
                             SERVICE_ERROR_NORMAL,  // error control type
                             ServiceExe,            // service's binary path name
                             group,                 // load order group
                             NULL,                  // tagID
                             dep,                   // dependencies
                             NULL,                  // ServiceStartName
                             NULL);                 // password

    if ( schService==NULL )
    {
        err=GetLastError();
        _tprintf(_T("Error in Install Driver %u\n"),err);
        return err;
    }
    else
    {
        // dwr, 1-228798, set Description field for Service Manager
        if (NULL != lpfnCreateServiceConfig2)
        {
            SERVICE_DESCRIPTION sd;
            LoadString(NULL, IDS_SCM_DESCRIPTION, szBuffer, MAX_BUFFER);
            sd.lpDescription=szBuffer;
            lpfnCreateServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sd);
        }
    }

    lpfnCloseServiceHandle(schService);
    return 0;
}
/******************************************************************************************/
DWORD InstallDriver(IN SC_HANDLE SchSCManager,IN LPSTR DriverName, IN LPSTR ServiceExe)
{

    return _InstallDriver(SchSCManager,
                          DriverName,ServiceExe,DriverName,
                          SERVICE_KERNEL_DRIVER,
                          SERVICE_DEMAND_START,
                          "extended base",
                          NULL);
}

/******************************************************************************************/
DWORD InstallService(IN SC_HANDLE SchSCManager, IN LPSTR DriverName, IN LPSTR ServiceExe)
{

    return _InstallDriver(SchSCManager,
                          DriverName,ServiceExe,DriverName,
                          SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                          SERVICE_AUTO_START,
                          "",
                          NULL);
}

//============================================================================
DWORD RemoveDriver(IN SC_HANDLE SchSCManager,IN LPCTSTR DriverName)
{
    SC_HANDLE  schService;
    BOOL       ret;

    schService=OpenService(SchSCManager,DriverName,SERVICE_ALL_ACCESS);
    if ( schService==NULL )
        return P_NO_OPEN_SERV;

    ret=lpfnDeleteService(schService);
    if ( !ret )
    {
        ret = GetLastError();
    }
    else
        ret = 0;

    lpfnCloseServiceHandle(schService);
    return ret;
}

void LogEvent(WORD wType, DWORD dwID,
              const char* pszS1,
              const char* pszS2,
              const char* pszS3 )
{
    if ( !s_bDebug )
    {
        const char* ps[3];
        ps[0] = pszS1;
        ps[1] = pszS2;
        ps[2] = pszS3;

        int iStr = 0;
        for ( int i=0; i<3; i++ )
        {
            if ( ps[i] != NULL ) iStr++;
        }

        HANDLE  hEventSource;

        hEventSource = RegisterEventSource(NULL, TEXT(SZSERVICENAME)); // source name

        if ( hEventSource )
        {
            ReportEvent(hEventSource,
                        wType,
                        0,
                        dwID,
                        NULL, // sid
                        iStr,
                        0,
                        ps,
                        NULL);
        }
    }
}


void GetLastErrorString( LPTSTR *lplpMsgBuf )
{
    LPVOID lpMsgBuf = NULL;

    FormatMessage
		(
                 FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                 FORMAT_MESSAGE_FROM_SYSTEM | 
                 FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL,
                 GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                 (LPTSTR) &lpMsgBuf,
                 0,
                 NULL );

    char *szMsgBuf = (char *)lpMsgBuf;
    int nLen = strlen(szMsgBuf);

    // nuke any trailing newline

    if ( nLen > 0 )
        if ( szMsgBuf[nLen-1] == '\n' ||
             szMsgBuf[nLen-1] == '\r' )
        {
            szMsgBuf[nLen-1] = 0;
        }

    nLen = strlen(szMsgBuf);

    // nuke any trailing newline - there are often both cr and lf

    if ( nLen > 0 )
        if ( szMsgBuf[nLen-1] == '\n' ||
             szMsgBuf[nLen-1] == '\r' )
        {
            szMsgBuf[nLen-1] = 0;
        }

    *lplpMsgBuf = szMsgBuf;

}

static int MyGetLastErrorString( LPTSTR szError, DWORD nNumErrorBytes)
{
    LPSTR   lpszError;
    TCHAR   szBuiltErrorMsg[1024];

    GetLastErrorString( &lpszError );
    if ( lpszError )
    {
        vpstrncpy( szError, lpszError, nNumErrorBytes );
        LocalFree( lpszError );
    }
    else
    {
		LoadString(NULL, IDS_NO_ERROR_STRING, szBuffer, MAX_BUFFER);
        sssnprintf( szBuiltErrorMsg, sizeof (szBuiltErrorMsg), szBuffer, GetLastError( ) );
        vpstrncpy( szError, szBuiltErrorMsg, nNumErrorBytes );
    }

    return FALSE;
}

//
//  FUNCTION: AddToMessageLog(LPTSTR lpszMsg)
//
//  PURPOSE: Allows any thread to log a generic error message
//
VOID AddToMessageLog(LPTSTR lpszMsg)
{
    TCHAR   szMsg[1024];

    if ( !s_bDebug )
    {
        TCHAR szErrorExpanded[1024];
		DWORD dwErr;

        MyGetLastErrorString( szErrorExpanded, sizeof (szErrorExpanded) );
        dwErr = GetLastError();

        // Use event logging to log the error.

        sssnprintf(szMsg, sizeof (szMsg), TEXT("%s (0x%x)"), szErrorExpanded, dwErr);

        LogEvent( EVENTLOG_ERROR_TYPE, EVMSG_GENERAL_ERROR, lpszMsg, szMsg, NULL );
    }
}




SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
TCHAR                   szErr[256];

// internal function prototypes
VOID WINAPI service_ctrl(DWORD dwCtrlCode);
VOID WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);
VOID CmdInstallService( char *szInstallPath, char *szServiceName, char *szExeName );
VOID CmdRemoveService( char *szServiceName );
VOID CmdDebugService(int argc, char **argv);
BOOL WINAPI ControlHandler ( DWORD dwCtrlType );
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize );

//
//  FUNCTION: ReportStatusToSCMgr()
//
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Manager
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
//
//  COMMENTS:
//
BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
                         DWORD dwWin32ExitCode,
                         DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;
    BOOL fResult = TRUE;

    if ( !s_bDebug ) // when debugging we don't report to the SCM
    {
        if ( dwCurrentState == SERVICE_START_PENDING )
            ssStatus.dwControlsAccepted = 0;
        else
            ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

        ssStatus.dwCurrentState = dwCurrentState;
        ssStatus.dwWin32ExitCode = dwWin32ExitCode;
        ssStatus.dwWaitHint = dwWaitHint;

        if ( ( dwCurrentState == SERVICE_RUNNING ) ||
             ( dwCurrentState == SERVICE_STOPPED ) )
            ssStatus.dwCheckPoint = 0;
        else
            ssStatus.dwCheckPoint = dwCheckPoint++;


        // Report the status of the service to the service control manager.
        //
        if ( !(fResult = lpfnSetServiceStatus( sshStatusHandle, &ssStatus)) )
        {
            AddToMessageLog(TEXT("SetServiceStatus"));
        }
    }
    return fResult;
}

BOOL KeepSCMHappyOnStart( int nWaitHint )
{
    DbgOut( "KeepSCMHappyOnStart" );
    return ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, nWaitHint );
}


BOOL KeepSCMHappyOnStop( int nWaitHint )
{
    DbgOut( "KeepSCMHappyOnStop" );
    return ReportStatusToSCMgr( SERVICE_STOP_PENDING, NO_ERROR, nWaitHint );
}


BOOL TellSCMGoodToGo( )
{
    DbgOut( "TellSCMGoodToGo" );
    return ReportStatusToSCMgr( SERVICE_RUNNING, NO_ERROR, 0 );
}


BOOL TellSCMAllDoneNow( DWORD dwErr )
{
    DbgOut( "TellSCMAllDoneNow" );
    return ReportStatusToSCMgr( SERVICE_STOPPED, dwErr, 0 );
}


//    If a ServiceStop procedure is going to
//    take longer than 3 seconds to execute,
//    it should spawn a thread to execute the
//    stop code, and return.  Otherwise, the
//    ServiceControlManager will believe that
//    the service has stopped responding.

VOID ServiceStop()
{
    DbgOut( "ServiceStop" );
    StopRoamingThread( );
}

//  PURPOSE: Actual code of the service
//           that does the work.
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//

VOID ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv)
{
    //
    // Service initialization
    //

    if ( ! KeepSCMHappyOnStart( 3000 ) ) goto cleanup;

    // report the status to the service control manager.
    //
    if ( ! TellSCMGoodToGo( ) )
        goto cleanup;

    // End of initialization

    // DebugBreak( );               // this if for debugging this when running as service

    if ( InitCommunications() )
    {
        goto cleanup;
    }

    // Service is now running, perform work until shutdown
    // this code is also run by rtvscan

    RoamingThread(NULL);
    DbgOut( "Service completing" );

cleanup:

	KeepSCMHappyOnStop(12000);
	DeinitCommunications();
    DbgOut( "DeInitCom complete" );
	KeepSCMHappyOnStop( 3000 );
}


//
//  FUNCTION: service_main
//
//  PURPOSE: To perform actual initialization of the service
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This routine performs the service initialization and then calls
//    the user defined ServiceStart() routine to perform majority
//    of the work.
//
void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
    // register our service control handler:
    //
    sshStatusHandle = lpfnRegisterServiceCtrlHandler( TEXT(SZSERVICENAME), service_ctrl);

    if ( !sshStatusHandle )
        goto cleanup;

    // SERVICE_STATUS members that don't change in example
    //
    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS ;
    ssStatus.dwServiceSpecificExitCode = 0;

    if ( ! KeepSCMHappyOnStart( 3000 ) ) goto cleanup;
	s_bRunningAsService = TRUE;	
    ServiceStart( dwArgc, lpszArgv );
	s_bRunningAsService = FALSE;
    cleanup:

    // try to report the stopped status to the service control manager.
    //
    if ( sshStatusHandle )
        TellSCMAllDoneNow( dwErr );

    return;
}



//
//  FUNCTION: service_ctrl
//
//  PURPOSE: This function is called by the SCM whenever
//           ControlService() is called on this service.
//
//  PARAMETERS:
//    dwCtrlCode - type of control requested
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
VOID WINAPI service_ctrl(DWORD dwCtrlCode)
{
    char szLine[1024];
    sssnprintf( szLine, sizeof (szLine), "ServiceControl %d", dwCtrlCode );
    DbgOut( szLine );

    // Handle the requested control code.
    //
    switch ( dwCtrlCode )
    {
        // Stop the service.
        //
        // SERVICE_STOP_PENDING should be reported before
        // setting the Stop Event - hServerStopEvent - in
        // ServiceStop().  This avoids a race condition
        // which may result in a 1053 - The Service did not respond...
        // error.
        case SERVICE_CONTROL_STOP:
            ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
            ServiceStop();
            return;

            // Update the service status.
            //
        case SERVICE_CONTROL_INTERROGATE:
            break;

            // invalid control code
            //
        default:
            break;
    }

    ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}



//  The following code handles service installation and removal

void CmdInstallService( char *szInstallPath, char *szServiceName, char *szExeName )
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    TCHAR szPath[512];
    TCHAR szNewPath[1024];
    TCHAR szLocalServiceName[1024];

    TCHAR szShortPath[1024];

    if ( GetModuleFileName( NULL, szPath, 512 ) == 0 )
    {
		LoadString(NULL, IDS_ERROR_INSTALLING, szBuffer, MAX_BUFFER);
		_tprintf(szBuffer, TEXT(SZSERVICEDISPLAYNAME), GetLastErrorText(szErr, 256));
        return;
    }

    // set the service name, either default or real

    if ( strlen(szServiceName) > 0 )
        vpstrncpy( szLocalServiceName, szServiceName, sizeof (szLocalServiceName) );
    else
        vpstrncpy( szLocalServiceName, TEXT(SZSERVICEDISPLAYNAME), sizeof (szLocalServiceName) );

    // if they are installing to a new place, copy my executable there

    if ( strlen(szInstallPath) > 0 )
    {
        vpstrncpy( szNewPath, szInstallPath, sizeof (szNewPath) );

        // ensure a trailing back slash

        int nLen = strlen(szNewPath);
        if ( szNewPath[nLen-1] != '\\' )
            vpstrnappend ( szNewPath, "\\", sizeof (szNewPath) );

        // add on the executable name

        if ( strlen(szExeName) > 0 )
            vpstrnappend ( szNewPath, szExeName , sizeof (szNewPath) );
        else
        {
            char *szMyExeName = strrchr( szPath, '\\' );
            if ( szMyExeName )
                vpstrnappend ( szNewPath, CharNext( szMyExeName ), sizeof (szNewPath) );
            else
            {
                // something is broken here
#ifdef ADMIN_ONLY
                vpstrnappend ( szNewPath, "RoamAdmn.exe", sizeof (szNewPath) );
#else
                vpstrnappend ( szNewPath, "SAVRoam.exe", sizeof (szNewPath) );
#endif
            }
        }

        BOOL bCopyResult = CopyFile( szPath, szNewPath, FALSE );
        if ( bCopyResult )
        {
            // worked - set the installation location as the new path

            vpstrncpy( szPath, szNewPath, sizeof (szPath) );
        }
		else //if copy failed bail out of the program
		{

			LoadString(NULL, IDS_ERROR_INSTALLING, szBuffer, MAX_BUFFER);
			_tprintf(szBuffer, TEXT(SZSERVICEDISPLAYNAME), GetLastErrorText(szErr, 256));
			return;
		}
    }

    // convert to short name just to be safe - this used to fail sometimes

    DWORD dwLen = GetShortPathName( szPath, szShortPath, sizeof(szShortPath)/sizeof(TCHAR) );
    vpstrncpy( szPath, szShortPath, sizeof (szPath) );

	schSCManager = lpfnOpenSCManager(
								NULL,                   // machine (NULL == local)
								NULL,                   // database (NULL == default)
								SC_MANAGER_ALL_ACCESS   // access required
								);
	if ( schSCManager )
	{
	    schService = lpfnCreateService(
								schSCManager,               // SCManager database
								szLocalServiceName,         // name of service
								szLocalServiceName,         // name to display
								SERVICE_ALL_ACCESS,         // desired access
								SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS ,  // service type
								SERVICE_AUTO_START,         // start type
								SERVICE_ERROR_NORMAL,       // error control type
								szPath,                     // service's binary
								NULL,                       // no load ordering group
								NULL,                       // no tag identifier
								TEXT(SZDEPENDENCIES),       // dependencies
								NULL,                       // LocalSystem account
								NULL);                      // no password

		if ( schService )
		{
			// dwr, 1-228798, set Description field for Service Manager
			if (NULL != lpfnCreateServiceConfig2)
			{
				SERVICE_DESCRIPTION sd;
				LoadString(NULL, IDS_SCM_DESCRIPTION, szBuffer, MAX_BUFFER);
				sd.lpDescription=szBuffer;
				lpfnCreateServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sd);
			}

			LoadString(NULL, IDS_INSTALLED, szBuffer, MAX_BUFFER);
			_tprintf(szBuffer, szLocalServiceName );

	        // and start it up, since it won't start itself

			if(lpfnStartService( schService, 0, NULL))
			{
				LoadString(NULL, IDS_SERVICE_STARTED, szBuffer, MAX_BUFFER);
				_tprintf(szBuffer, szLocalServiceName);
			}
			else
			{
				LoadString(NULL, IDS_UNABLE_TO_START, szBuffer, MAX_BUFFER);
				_tprintf(szBuffer, szLocalServiceName);
			}
	        lpfnCloseServiceHandle(schService);
	    }
		else
		{

			LoadString(NULL, IDS_FUNCTION_FAILED_TEXT, szBuffer, MAX_BUFFER);
			_tprintf(szBuffer, _T("CreateService"), GetLastErrorText(szErr, 256));
		}

	    // make registry entries to support logging messages
		// Add the source name as a subkey under the Application
		// key in the EventLog service portion of the registry.

	    char szKey[256];
		HKEY hKey = NULL;
		vpstrncpy    (szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\", sizeof (szKey));
		vpstrnappend (szKey, szLocalServiceName, sizeof (szKey));
		if ( RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS )
	        return;

	    // Add the Event ID message-file name to the 'EventMessageFile' subkey.
		RegSetValueEx(hKey,
					_T("EventMessageFile"),
					0,
					REG_EXPAND_SZ, 
					(CONST BYTE*)szPath,
					strlen(szPath) + 1);     

	    // Set the supported types flags.
		DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
		RegSetValueEx(hKey,
					_T("TypesSupported"),
					0,
					REG_DWORD,
					(CONST BYTE*)&dwData,
					sizeof(DWORD));
		RegCloseKey(hKey);

	    //LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, szLocalServiceName);

		lpfnCloseServiceHandle(schSCManager);
	}
	else
	{

		LoadString(NULL, IDS_FUNCTION_FAILED_TEXT, szBuffer, MAX_BUFFER);
		_tprintf(szBuffer, _T("OpenSCManager"), GetLastErrorText(szErr,256));
	}
}

void CmdRemoveService( char *szServiceName )
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    TCHAR szLocalServiceName[1024];


    // set the service name, either default or real

    if ( strlen(szServiceName) > 0 )
        vpstrncpy( szLocalServiceName, szServiceName, sizeof (szLocalServiceName) );
    else
        vpstrncpy( szLocalServiceName, TEXT(SZSERVICEDISPLAYNAME), sizeof (szLocalServiceName) );

	schSCManager = lpfnOpenSCManager(
								NULL,                   // machine (NULL == local)
								NULL,                   // database (NULL == default)
								SC_MANAGER_ALL_ACCESS   // access required
								);
	if ( schSCManager )
	{
	    schService = OpenService(schSCManager, szLocalServiceName, SERVICE_ALL_ACCESS);

	    if ( schService )
		{
	        // try to stop the service
			if ( lpfnControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
			{
				LoadString(NULL, IDS_STOPPING_SERVICE, szBuffer, MAX_BUFFER);
				_tprintf(szBuffer, szLocalServiceName);
				Sleep( 1000 );

	            while ( lpfnQueryServiceStatus( schService, &ssStatus ) )
				{
	                if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
					{
	                    _tprintf(TEXT("."));
						Sleep( 1000 );
					}
					else
	                    break;
				}

	            if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
				{
					LoadString(NULL, IDS_SERVICE_STOPPED, szBuffer, MAX_BUFFER);
					_tprintf(szBuffer, szLocalServiceName );
				}
				else
				{
					LoadString(NULL, IDS_ERROR_STOPPING_SERVICE, szBuffer, MAX_BUFFER);
					_tprintf( szBuffer, szLocalServiceName );
				}
	        }

	        // now remove the service
			if ( lpfnDeleteService(schService) )
			{
				LoadString(NULL, IDS_SERVICE_REMOVED, szBuffer, MAX_BUFFER);
				_tprintf(szBuffer, szLocalServiceName );
			}
			else
			{
				LoadString(NULL, IDS_FUNCTION_FAILED_TEXT, szBuffer, MAX_BUFFER);
				_tprintf(szBuffer, _T("DeleteService"), GetLastErrorText(szErr,256));
			}

	        lpfnCloseServiceHandle(schService);

	        // nuke registry entries to support logging messages
			char szKey[256];
			HKEY hKey = NULL;
			vpstrncpy    (szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\", sizeof (szKey));
			vpstrnappend (szKey, szLocalServiceName, sizeof (szKey));
			if ( RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS )
			{
	            lpfnCloseServiceHandle(schSCManager);
				return;
			}

	        // Add the Event ID message-file name to the 'EventMessageFile' subkey.
			RegDeleteValue(hKey, _T("EventMessageFile") );
			RegDeleteValue(hKey, _T("TypesSupported") );

			RegCloseKey(hKey);

			// now delete the key itself

			vpstrncpy(szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\", sizeof (szKey));
			if ( RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS )
			{
	            lpfnCloseServiceHandle(schSCManager);
				return;
			}

	        RegDeleteKey( hKey, szLocalServiceName );
			RegCloseKey( hKey );


	    }
		else
		{
			LoadString(NULL, IDS_FUNCTION_FAILED_TEXT, szBuffer, MAX_BUFFER);
			_tprintf(szBuffer, _T("OpenService"), GetLastErrorText(szErr,256));
		}

		lpfnCloseServiceHandle(schSCManager);
	}
	else
	{
		LoadString(NULL, IDS_FUNCTION_FAILED_TEXT, szBuffer, MAX_BUFFER);
		_tprintf(szBuffer, _T("OpenSCManager"), GetLastErrorText(szErr,256));
	}
}

//  The following code is for running the service as a console app
//
//  FUNCTION: CmdDebugService(int argc, char ** argv)
//
//  PURPOSE: Runs the service as a console application
//
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments

void CmdDebugService(int argc, char ** argv)
{
    DWORD dwArgc;
    LPTSTR *lpszArgv;


#ifdef UNICODE
    lpszArgv = CommandLineToArgvW(GetCommandLineW(), &(dwArgc) );
#else
    dwArgc   = (DWORD) argc;
    lpszArgv = argv;
#endif

	LoadString(NULL, IDS_DEBUGGING_SERVICE, szBuffer, MAX_BUFFER);
    _tprintf(szBuffer, TEXT(SZSERVICEDISPLAYNAME));

    BOOL bResult = SetConsoleCtrlHandler( ControlHandler, TRUE );

	ServiceStart( dwArgc, lpszArgv );
}


//
//  FUNCTION: ControlHandler ( DWORD dwCtrlType )
//
//  PURPOSE: Handled console control events
//
//  PARAMETERS:
//    dwCtrlType - type of control event
//
//  RETURN VALUE:
//    True - handled
//    False - unhandled
//
//  COMMENTS:
//
BOOL WINAPI ControlHandler ( DWORD dwCtrlType )
{

    switch ( dwCtrlType )
    {
        case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
        case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			LoadString(NULL, IDS_STOPPING_SERVICE, szBuffer, MAX_BUFFER);
            _tprintf(szBuffer, TEXT(SZSERVICEDISPLAYNAME));
				ServiceStop();
            return TRUE;
            break;

    }
    return FALSE;
}


//
//  FUNCTION: CheckIfPassedStringIsANumber
//
//  PURPOSE: To check if passed string is a a valid 
//			 number or not i.e if it has digits or not
//
//  PARAMETERS:
//    szArg - the string argument to determine if it has valid digits or not

//
//  RETURN VALUE:
//    If the passed string has even one element in it that is not a digit it will pass back FALSE
//	  else for success it will pass TRUE	

BOOL CheckIfPassedStringIsANumber(char* szArg)
{
	BOOL nValidateStringAsNumber = TRUE;
	if( (szArg == NULL) || (strlen(szArg) <= 0) )
	{
		//cannot check hence return failure
		return FALSE;
	}
	for( unsigned int i=0; i<strlen(szArg); i++ )
	{
		if( isdigit((int)szArg[i]) == 0)
		{
			//not a digit break and go 
			nValidateStringAsNumber = FALSE;
			break;
		}
	}
	return nValidateStringAsNumber;
}

//  FUNCTION: DeinitWindowsLibraries
//
//  PURPOSE: decrement reference count on any librariesload any libraries specific to Windows version
//
//  RETURN VALUE:  BOOL for success/failure
//    
//
BOOL DeinitWindowsLibraries()
{
	BOOL bFreedKernel32 = TRUE;
	BOOL bFreedAdvapi32 = TRUE;

	if(g_hKernel32)
		bFreedKernel32 = FreeLibrary(g_hKernel32);
	if(g_hAdvapi32)
		bFreedAdvapi32 = FreeLibrary(g_hAdvapi32);

	return (bFreedAdvapi32 && bFreedKernel32);
}

//
//  FUNCTION: InitNTLibraries
//
//  PURPOSE: load libraries and save entrypoints to 
//			 NT specific functions.
//
//  RETURN VALUE:  BOOL for success in obtaining all entrypoints
//    
//  COMMENTS:  LoadLibrary/GetProcAddress permits linking one
//             app for all versions.
//
BOOL InitNTLibraries()
{
	BOOL bKernel32LoadError = FALSE;
	BOOL bAdvapi32LoadError = FALSE;

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\advapi32.dll"));
	g_hAdvapi32 = LoadLibrary(szPath);
	if(!g_hAdvapi32) 
	{
		LoadString(NULL, IDS_ERROR_LOADING_LIBRARY, szBuffer, MAX_BUFFER);
		sssnprintf(szBuffer2, sizeof (szBuffer2), szBuffer, _T("advapi32"));
		OutputLine(szBuffer2, NULL, FALSE);
		return FALSE;
	}

	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\Kernel32.dll"));
    g_hKernel32 = LoadLibrary(szPath);
    if( g_hKernel32 == NULL ) 
	{
		FreeLibrary(g_hAdvapi32);
		LoadString(NULL, IDS_ERROR_LOADING_LIBRARY, szBuffer, MAX_BUFFER);
		sssnprintf(szBuffer2, sizeof (szBuffer2), szBuffer, _T("Kernel32"));
		OutputLine(szBuffer2, NULL, FALSE);
		return FALSE;
	}

#ifdef _UNICODE
	lpfnOpenSCManager = (LPFNOPENSCMANAGER)GetProcAddress(g_hAdvapi32, _T("OpenSCManagerW"));
	lpfnCreateService = (LPFNCREATESERVICE)GetProcAddress(g_hAdvapi32, _T("CreateServiceW"));
	lpfnStartService = (LPFNSTARTSERVICE)GetProcAddress(g_hAdvapi32, _T("StartServiceW));
	lpfnStartServiceCtrlDispatcher = (LPFNSTARTSERVICECTRLDISPATCHER)GetProcAddress(g_hAdvapi32, _T("StartServiceCtrlDispatcherW"));
	lpfnRegisterServiceCtrlHandler = (LPFNREGISTERSERVICECTRLHANDLER)(GetProceAddress(g_hAdvapi32, _T("RegisterServiceCtrlHandlerW"));

	// dwr, 1-228798, set Description field for Service Manager
	lpfnCreateServiceConfig2 = (LPFNCHANGESERVICECONFIG2)GetProcAddress(g_hAdvapi32, _T("ChangeServiceConfig2W"));
#else
	lpfnOpenSCManager = (LPFNOPENSCMANAGER)GetProcAddress(g_hAdvapi32, _T("OpenSCManagerA"));
	lpfnCreateService = (LPFNCREATESERVICE)GetProcAddress(g_hAdvapi32, _T("CreateServiceA"));
	lpfnStartService = (LPFNSTARTSERVICE)GetProcAddress(g_hAdvapi32, _T("StartServiceA"));
	lpfnStartServiceCtrlDispatcher = (LPFNSTARTSERVICECTRLDISPATCHER)GetProcAddress(g_hAdvapi32, _T("StartServiceCtrlDispatcherA"));
	lpfnRegisterServiceCtrlHandler = (LPFNREGISTERSERVICECTRLHANDLER)GetProcAddress(g_hAdvapi32, _T("RegisterServiceCtrlHandlerA"));

	// dwr, 1-228798, set Description field for Service Manager
	lpfnCreateServiceConfig2 = (LPFNCHANGESERVICECONFIG2)GetProcAddress(g_hAdvapi32, _T("ChangeServiceConfig2A"));
#endif
	lpfnDeleteService = (LPFNDELETESERVICE)GetProcAddress(g_hAdvapi32, _T("DeleteService"));
	lpfnQueryServiceStatus = (LPFNQUERYSERVICESTATUS)GetProcAddress(g_hAdvapi32, _T("QueryServiceStatus"));
	lpfnControlService = (LPFNCONTROLSERVICE)GetProcAddress(g_hAdvapi32, _T("ControlService"));
	lpfnSetServiceStatus = (LPFNSETSERVICESTATUS)GetProcAddress(g_hAdvapi32, _T("SetServiceStatus"));
	lpfnCloseServiceHandle = (LPFNCLOSESERVICEHANDLE)GetProcAddress(g_hAdvapi32, _T("CloseServiceHandle"));

	if(!lpfnOpenSCManager || 
	   !lpfnCreateService || 
	   !lpfnDeleteService || 
	   !lpfnQueryServiceStatus || 
	   !lpfnStartService || 
	   !lpfnStartServiceCtrlDispatcher ||
	   !lpfnRegisterServiceCtrlHandler ||
	   !lpfnControlService || 
	   !lpfnSetServiceStatus || 
	   !lpfnCloseServiceHandle)
		bAdvapi32LoadError = TRUE;

	if(bAdvapi32LoadError || bKernel32LoadError)
	{
		FreeLibrary(g_hKernel32);
		FreeLibrary(g_hAdvapi32);

		if(bKernel32LoadError)
		{
			LoadString(NULL, IDS_ERROR_LIBRARY_FUNCTIONS, szBuffer, MAX_BUFFER);
			sssnprintf(szBuffer2, sizeof (szBuffer2), szBuffer, _T("Kernel32"));
			OutputLine(szBuffer2, NULL, FALSE);
		}

		if(bAdvapi32LoadError)
		{
			LoadString(NULL, IDS_ERROR_LIBRARY_FUNCTIONS, szBuffer, MAX_BUFFER);
			sssnprintf(szBuffer2, sizeof (szBuffer2), szBuffer, _T("advapi32"));
			OutputLine(szBuffer2, NULL, FALSE);
		}

		return FALSE;
	}

	return TRUE;
}

//
//  FUNCTION: CheckWindowsVersion
//
//  PURPOSE: determine Windows platform
//
//  RETURN VALUE:  BOOL for success in determining platform
//    
//  COMMENTS:  Sets global variables g_bWin2000plus
//
BOOL CheckWindowsVersion()
{
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

    GetVersionEx( &ver );

	if(ver.dwMajorVersion >= 5)
		g_bWin2000plus = TRUE;

	if(g_bWin2000plus)
		return TRUE;

	LoadString(NULL, IDS_ERROR_WINDOWS_VERSION, szBuffer, MAX_BUFFER);
	OutputLine(szBuffer, NULL, FALSE);
    return FALSE;
}

//
//  FUNCTION: InitWindowsLibraries
//
//  PURPOSE: load any libraries specific to Windows version
//
//  RETURN VALUE:  BOOL for success/failure
//    
//
BOOL InitWindowsLibraries()
{
	BOOL bInitialized = FALSE;

	if(!CheckWindowsVersion())
		return FALSE;

	bInitialized = InitNTLibraries();

	return bInitialized;
}


BOOL RunAsService()
{
    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main},
        { NULL, NULL}
    };

    lpfnStartServiceCtrlDispatcher(dispatchTable);
    return TRUE;
}

//
//  FUNCTION: main
//
//  PURPOSE: entrypoint for service
//
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    main() either performs the command line task, or
//    call StartServiceCtrlDispatcher to register the
//    main service thread.  When the this call returns,
//    the service has stopped, so exit.
//
int main(int argc, char* argv[])
{
    DWORD   dwTransmanHandle = 0;
    DWORD   dwRet = 1;          // this is the application exit code, 0 is good

	BOOL bRunningAsService = ( argc == 1 );

	if( !InitWindowsLibraries() )
	{
        // this library is required
		dwRet = -1;
        goto AllDone;
	}

	dwRet = LoadTransman( );
	if( dwRet != ERROR_SUCCESS ){
	dwRet = -1;
		goto AllDone;
	}

    InitRegKeys( );

    if ( ! RoamingInit( bRunningAsService, FALSE, &dwRet ) )
    {
        goto AllDone;
    }

    if ( argc > 1 && ! stricmp( argv[1], "/install" ) )
    {
        char szInstallPath[1024] = "";
        char szServiceName[1024] = "";
        char szExeName[1024] = "";

        char szLine[1024];

        if ( argc > 2 )
            vpstrncpy( szInstallPath, argv[2], sizeof (szInstallPath));

        if ( argc > 3 )
            vpstrncpy( szServiceName, argv[3], sizeof (szServiceName) );

        if ( argc > 4 )
        {
            vpstrncpy( szExeName, argv[4], sizeof (szExeName) );
			//check if an extension is there if no extension then append exe
			//if extension then check if exe else bail out
		   char path_buffer[_MAX_PATH];
		   char drive[_MAX_DRIVE];
		   char dir[_MAX_DIR];
		   char fname[_MAX_FNAME];
		   char ext[_MAX_EXT];
		   vpstrncpy(path_buffer,szExeName, sizeof (path_buffer));	
		   _tsplitpath( path_buffer, drive, dir, fname, ext );
		   vpstrncpy(szExeName, fname, sizeof (szExeName));
		   
		   //check if is exe
		   if(_tcslen(ext) > 0)
		   {
			   if(_tcsicmp(ext, ".EXE"))
			   {
					//not an exe invalid extension

					LoadString(NULL, IDS_INVALID_EXTENSION, szBuffer2, MAX_BUFFER);
					sssnprintf(szBuffer, sizeof (szBuffer), szBuffer2, "EXE");
					LoadString(NULL, IDS_ERROR_INSTALLING, szBuffer2, MAX_BUFFER);
					_tprintf(szBuffer2, TEXT(SZSERVICEDISPLAYNAME), szBuffer);
					dwRet = -1;
					goto AllDone;
			   }
			   
		   }
			   
			//no extension append exe to the szExeName
			vpstrnappend (szExeName, _T(".EXE"), sizeof (szExeName));
        }

        if ( strlen(szInstallPath) > 0 )
        {
			LoadString(NULL, IDS_INSTALLING_TO_PATH, szBuffer, MAX_BUFFER);
            sssnprintf( szLine, sizeof (szLine), szBuffer, _T(SZSERVICEDISPLAYNAME), szInstallPath );
            OutputLine( szLine, NULL, FALSE );
        }

        if ( strlen(szServiceName) > 0 )
        {
			LoadString(NULL, IDS_INSTALLING_AS_SERVICE, szBuffer, MAX_BUFFER);
            sssnprintf( szLine, sizeof (szLine), szBuffer, _T(SZSERVICEDISPLAYNAME), szServiceName );
            OutputLine( szLine, NULL, FALSE );
        }

        if ( strlen(szExeName) > 0 )
        {
			LoadString(NULL, IDS_INSTALLING_AS_EXE, szBuffer, MAX_BUFFER);
            sssnprintf( szLine, sizeof (szLine), szBuffer, _T(SZSERVICEDISPLAYNAME), szExeName );
            OutputLine( szLine, NULL, FALSE );
        }

        CmdInstallService( szInstallPath, szServiceName, szExeName );
        dwRet = 0;
        goto AllDone;
    }

    if ( argc > 1 && ! stricmp( argv[1], "/remove" ) )
    {
        char szServiceName[1024] = "";
        char szLine[1024];

        if ( argc > 2 )
            vpstrncpy( szServiceName, argv[2], sizeof (szServiceName) );

        if ( strlen(szServiceName) > 0 )
        {
			LoadString(NULL, IDS_REMOVING_AS_SERVICE, szBuffer, MAX_BUFFER);
            sssnprintf( szLine, sizeof (szLine), szBuffer, _T(SZSERVICEDISPLAYNAME), szServiceName );
            OutputLine( szLine, NULL, FALSE );
        }

        CmdRemoveService( szServiceName );

        dwRet = 0;
        goto AllDone;
    }

    // import server list

    if ( argc > 2 && ! stricmp( argv[1], "/import" ) )
    {
        if ( ! DialUpNetworkPresent( FALSE ) )          goto AllDone;
        if ( InitCommunications() )  goto AllDone;

        s_bVerbose = TRUE;

        ImportServerList( argv[2] );

        DeinitCommunications();

        dwRet = 0;

        goto AllDone;

    }

    // export server list

    if ( argc > 1 && ! stricmp( argv[1], "/export") )
    {
        if ( ! DialUpNetworkPresent( FALSE ) )          goto AllDone;
        if ( InitCommunications() )  goto AllDone;

        // either report just to the screen or create an import file as well

		if(argc > 2) 
			ExportServerListToFile( argv[2] ); 
        else
        {
            s_bVerbose = TRUE;

            ReportServerList( NULL, "Parent" );        // screen only
        }

        DeinitCommunications();

        dwRet = 0;
        goto AllDone;
    }

    if ( argc > 1 && ! stricmp( argv[1], "/debug" ) )
    {
        s_bDebug = TRUE;
        CmdDebugService( argc, argv );

        dwRet = 0;
        goto AllDone;
    }

    // machine shutdown

    if ( argc > 1 && ! stricmp( argv[1], "/shutdown") )
    {
        if ( ! DialUpNetworkPresent( FALSE ) )          goto AllDone;
        if ( InitCommunications() )  goto AllDone;

        MachineShutdown( );

        DeinitCommunications();

        dwRet = 0;
        goto AllDone;
    }

    if ( argc > 1 && ! stricmp( argv[1], "/nearest") )
    {
        s_bVerbose = TRUE;

        if ( ! DialUpNetworkPresent( FALSE ) )          goto AllDone;
        if ( InitCommunications() )  goto AllDone;

        FindNearestParent( INTERACTIVE );

        DeinitCommunications();

        dwRet = 0;
        goto AllDone;
    }

    if ( argc > 3 && ! stricmp( argv[1], "/time_network") )
    {
        s_bVerbose = TRUE;

        if ( ! DialUpNetworkPresent( FALSE ) )          goto AllDone;
        if ( InitCommunications() )  goto AllDone;

        if( (argv[4] != NULL) && (argv[3] != NULL) && (argv[2] != NULL) )
		{
			//we have the correct no of parameters
			//now check if the arg2 and arg3 are valid numbers
			int nValidateSuccess = 1;
			int arg2 = -1;
			int arg3 =-1;
			char* arg = NULL;
			
			for(int j = 0; j < 2; j++)
			{
				arg=argv[j+2];
				if(CheckIfPassedStringIsANumber(arg) != TRUE)
				{
					//we could not validate break out of this for loop
					nValidateSuccess = -1;
					break;
				}
			}
			if(nValidateSuccess == -1)
			{
				DeinitCommunications();
				goto DisplayHelp;
			}
			else
			{
				TimeNetwork( argv[4], argv[2], argv[3] );
			}
		}
		else
		{
			DeinitCommunications();
			goto DisplayHelp;
		}

        DeinitCommunications();

        dwRet = 0;
        goto AllDone;
    }

    if ( argc > 1 && ! stricmp( argv[1], "/check_parent") )
    {
        s_bVerbose = TRUE;

        if ( ! DialUpNetworkPresent( FALSE ) )          goto AllDone;
        if ( InitCommunications() )  goto AllDone;

        CheckAndResetParent( INTERACTIVE );

        DeinitCommunications();

        dwRet = 0;
        goto AllDone;
    }

    if ( argc > 1 && ! stricmp( argv[1], "/force") )
    {
        s_bVerbose = TRUE;

        if ( ! DialUpNetworkPresent( FALSE ) ) goto AllDone;
        if ( InitCommunications() ) goto AllDone;

        ForceNearestMachineParent( argv[2] );

        DeinitCommunications();

        dwRet = 0;
        goto AllDone;
    }

	if( bRunningAsService )
	{
		// otherwise I assume that this is a real service, and start accordingly

        RunAsService();
		dwRet = 0;
		goto AllDone;
	}

DisplayHelp:
	// Unrecognizable arguments - display help
#ifdef ADMIN_ONLY
	LoadString(NULL, IDS_ROAMADMN_HELP, szBuffer, MAX_BUFFER);
#else
	LoadString(NULL, IDS_SAVROAM_HELP, szBuffer, MAX_BUFFER);
#endif
	sssnprintf(szBuffer2, sizeof (szBuffer2), szBuffer,
		_T("import"),
		_T("export"),
		_T("install"), 
		_T("remove"),
		_T("nearest"),
		_T("time_network"), 
		_T("check_parent"),
		_T("shutdown"));

	OutputLine(szBuffer2, NULL, FALSE);

	dwRet = 0;

AllDone:    

	DeinitWindowsLibraries( );

    RoamingDeinit( &dwRet );

    DeInitRegKeys( );

    return dwRet;
}
