// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
// MSIEntryPoints.cpp
//
#include "stdafx.h"
#define INITIIDS

#include <windows.h>
#include <Winsock2.h>
#include <MSI.H>

#include "appinfo.h"
#include "IAuth.h"
#include "SymInterface.h"
#include "SymInterfaceLoader.h"
#include "RoleVector.h"
#include "vpcommon.h"
#include "SavCustom.h"
#include "ConnectDlg.h"
#include "VerifySGPass.h"
#include "Domains.h"
#include "password.h"
#include "regkey.h"
#include "SCSSecureLogin.h"
#include "MSIEntryPoints.h"
#include "ScsCommsUtils.h"
#include "TransmanRelatedFuncs.h"
#include "resource.h"
#include "SymSaferRegistry.h"
#include "NewPassDlg.h"

// Added for OEM below
#include "oem.h"
#include "oem_common.h"
#include "oem_constants.h"
#include "regstr.h"
#define OEM_INSTALL_SWITCH	_T("OEM")
#define MAX_SERVER_NAME			48	  // limit imposed by LDVP technology
#define MIN_PASSWORD_LENGTH		6

#define KEY_WOW64_64KEY					(0x0100)
#define DEFAULT_COMMONFILES				_T("Common Files")
#define DEFAULT_PROGRAMFILES			_T("Program Files")
#define DEFAULT_COMMONFILESDIR			DEFAULT_PROGRAMFILESDIR _T("\\") DEFAULT_COMMONFILES
#define DEFAULT_SYMANTEC_SHARED			_T("Symantec Shared")
 
using namespace ScsSecureComms;

const TCHAR g_szPathToScsCommsDll[]						= SYM_SCSCOMMDLL;
const TCHAR g_szPathToTransmanDll[]						= "Transman.dll";

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

// Local function declarations.

DWORD CheckRolesOfLoginCert(ScsSecureComms::IAuth *&pAuth, BOOL *pFullAccess);
BOOL ValidateAdminRoles (MSIHANDLE hInstall,
					     TCHAR* strPrimaryServerName,
					     TCHAR* strUserName,
					     TCHAR* strPassword,
                         ScsSecureComms::RESULT& hResult);

///////////////////////////////////////////////////////////////////////////////
//
//  Function: AddSlash
//
//  Simply adds a slash to the end of a path, if needed.
//
//  08/16/2003 Thomas Brock
//
///////////////////////////////////////////////////////////////////////////////
void __fastcall AddSlash( char* pszPath )
{
	if( pszPath[ strlen( pszPath ) - 1 ] != '\\' )
	{
		strcat( pszPath, "\\" );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function: SetMSIEditControlText
//
//  Send WM_SETTEXT message to the specified edit control on the current
//	MSI UI pane. This will cause MSI to show its updated property value.
//
//  02/24/2005 Sean Kennedy
//
///////////////////////////////////////////////////////////////////////////////
void SetMSIEditControlText( DWORD control )
{
	// Find the MSI UI Window
	HWND msiWindow = FindWindow( "MsiDialogCloseClass", NULL );
	if( NULL != msiWindow )
	{
		DWORD current = 0;
		HWND msiChild = NULL;
		HWND msiLast = NULL;
		do
		{
			// Find any edit controls on the window
			msiChild = FindWindowEx( msiWindow, msiLast, "RichEdit20W", NULL );
			if( msiChild )
			{
				++current;
				if( current == control )
				{
					OutputDebugString( "Sending WM_SETTEXT message" );
					SendMessage( msiChild, WM_SETTEXT, 0L, 0L );
					break;
				}
			}
			msiLast = msiChild;
		} while( NULL != msiChild );
	}
	else
		OutputDebugString( "Unable to find MSI window" );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function: MsiDoServerListDlg
//
//  Wrapper to DoServerListDlg that can be called directly from MSI.
//
//  Since this CustomAction is called from a DoAction control event, it
//  can't use MsiProcessMessage, which means our custom MSILogMessage
//  routines can't be used.
//
//  Replicates functionality originally coded in InstallScript.
//
//  08/03/2003 Thomas Brock
//
///////////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiDoServerListDlg( MSIHANDLE hInstall )
{
	bool	bUnregister			= false;
	DWORD	dwLen				= NULL;
	BOOL	bTMInit				= FALSE;
	BOOL	bTMRegistered		= FALSE;
	LONG	lReturn				= NULL;
	LONG	lFiles				= NULL;
	UINT	nRet				= ERROR_SUCCESS;
	UINT	nError				= ERROR_SUCCESS;
	HMODULE	hMod				= NULL;
	char	szParentServer[MAX_PATH]	= {0};
	char	szFile[MAX_PATH]	= {0};
	char	szCurDir[MAX_PATH]	= {0};
	char	szTempDir[MAX_PATH]	= {0};

	//MessageBox( NULL, "Stop here.", "Debug", MB_OK );

	try
	{
		HWND hParent = GetForegroundWindow();

		CoInitialize( NULL );

		// Have to set the current working directory, so all
		// of the DLL's can find each other.  This allows
		// me to restore it when we are done.
		GetCurrentDirectory( MAX_PATH, szCurDir );

		// Get the temp directory from the MSI database...
		dwLen = MAX_PATH;
		if( ERROR_SUCCESS != ( nError = MsiGetProperty( hInstall, "TempFolder", szTempDir, &dwLen ) ) )
			throw( "Error getting TempFolder property" );
		AddSlash( szTempDir );

		// Since the gang is all here, tell them where here is...
		SetCurrentDirectory( szTempDir );
		// Now we have to see if Transman.dll is already loaded on the system.  If so, we
		// do not want to register our temporary copy.
		bTMRegistered = IsTransmanRegistered();
		hMod = LoadTransman( szTempDir, bTMRegistered );
		if( NULL == hMod )
		{
			OutputDebugString( "Unable to load transman.dll" );
			throw( _T("Unable to load transman.dll") );
		}

		OutputDebugString( "Done with Load" );

		if( !TMInit() )
			throw( _T("TMInit failed") );

		TMStartDomains();
		bTMInit = TRUE;

		// Get the current value of the SERVERNAME property from the MSI database...
		OutputDebugString( "About to get SERVERNAME\r\n" );
		dwLen = MAX_PATH;
		if( ERROR_SUCCESS != ( nError = MsiGetProperty( hInstall, "SERVERNAME", szParentServer, &dwLen ) ) )
			throw( "MsiGetProperty SERVERNAME failed" );

		if( dwLen )
		{
			// Set the value of TEMPSERVERNAME to the current value of SERVERNAME, so if
			// the user selects cancel, we don't change anything.  SERVERNAME will always
			// be assigned the value of TEMPSERVERNAME when this function returns.
			if( ERROR_SUCCESS != ( nError = MsiSetProperty( hInstall, "TEMPSERVERNAME", szParentServer ) ) )
			{
				throw( "MsiSetProperty TEMPSERVERNAME failed" );
			}
		}

		// Bring on the dialog.
		OutputDebugString( "About to DoServerListDlg\r\n" );
		lReturn = DoServerListDlg( hParent, szParentServer );

		// SavCustom assigns IDC_RET_OK to the OK button
		if( lReturn == IDC_RET_OK )
		{
			SetMSIEditControlText( 1 );
			// Set the selected value to TEMPSERVERNAME.
			if( ERROR_SUCCESS != ( nError = MsiSetProperty( hInstall, "TEMPSERVERNAME", szParentServer ) ) )
			{
				throw( "MsiSetProperty TEMPSERVERNAME (2) failed" );
			}
		}
	}
	catch( char *szErr )
	{
		char szError[MAX_PATH] = {0};
		sssnprintf( szError, sizeof(szError), "%s (Error Code: %d)\r\n", szErr, nError );
		OutputDebugString( szError );

		nRet = ERROR_INSTALL_FAILURE;
	}

	// Cleanup code.
	if( bTMInit )
	{
		OutputDebugString( "Cleaning up transman" );
		TMCleanupDomains();
		TMStopTransman();
		TMFreeTransman();
	}
	
	if( hMod )
        UnloadTransman( &hMod, bTMRegistered, (nRet == ERROR_SUCCESS ? szParentServer : NULL) );

	// Set this back to what it was...
	SetCurrentDirectory( szCurDir );

	CoUninitialize();

	return nRet;
}

BOOL IsTransmanRegistered()
{
	BOOL bRet = FALSE;
	HKEY hKey;

	if( ERROR_SUCCESS == RegOpenKey( HKEY_CLASSES_ROOT, "CLSID\\{E381F1E0-910E-11D1-AB1E-00A0C90F8F6F}\\InProcServer32", &hKey ) )
	{
		RegCloseKey( hKey );
		bRet = TRUE;
	}

	return bRet;
}

HMODULE LoadTransman( TCHAR *szTempDir, BOOL bTMRegistered )
{
	// Transman.dll is not here.  We have to register our copy because
	// the underlying code uses the CLSID registry key to find the DLL.
	HMODULE hTransman = NULL;
	FARPROC	fpLoad = NULL;
	DWORD	nError = S_OK;
	char	szFile[MAX_PATH]	= {0};

	try
	{
		// Since we have to dork with Transman.dll, construct a full path to it.
		lstrcpy( szFile, szTempDir );
		lstrcat( szFile, g_pszFiles[4] );

		// Load it.
		hTransman = LoadLibrary( szFile );
		nError = GetLastError();
		if( !hTransman )
			throw( _T("Unable to load transman.dll") );

		if( FALSE == bTMRegistered )
		{
			fpLoad = GetProcAddress( hTransman, "DllRegisterServer" );
			if( !fpLoad )
				throw( _T("Unable to get address for DllRegisterServer in Transman") );

			// Register it.
			if( S_OK != ( nError = (fpLoad)() ) )
				throw( _T("Error occured while registering Transman") );
		}
	}
	catch( TCHAR *szErr )
	{
		CString szTemp;

		szTemp.Format( "%s (Error #%d)", szErr, nError );
		OutputDebugString( szTemp );
		throw( szErr );
	}

	fpLoad = NULL;

	return hTransman;
}

static BOOL DeleteTranmanRegKeys( const TCHAR* strParentServerName )
{
    BOOL bRet = TRUE;

    DWORD dwResult;

    // Preserve the parent server AddressCache entry, if it was passed to us.
    if (strParentServerName != NULL && _tcslen(strParentServerName) > 0)
    {
        TCHAR sName[512];
        DWORD dwSize=sizeof(sName), dwIndex=0;
        FILETIME fTime;
        HKEY regKey;

        if (!(dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache"),0,KEY_ALL_ACCESS,&regKey)))
        {
	        while (RegEnumKeyEx(regKey,dwIndex,sName,&dwSize,NULL,NULL,NULL,&fTime) != ERROR_NO_MORE_ITEMS)
	        {
                if (_tcsicmp(sName, strParentServerName) != 0 )
                {
		            RegKeyDeleteAll(regKey,sName);
                }
                else
                {
                    ++dwIndex;
                }
		        dwSize = sizeof(sName);
	        }
	        RegCloseKey(regKey);
        }
    }
    else
    {
       dwResult = RegKeyDeleteAll(HKEY_LOCAL_MACHINE,      _T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache"));
       dwResult = RegKeyDeleteEmptyKey(HKEY_LOCAL_MACHINE, _T("Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"));
       dwResult = RegKeyDeleteEmptyKey(HKEY_LOCAL_MACHINE, _T("Software\\INTEL\\LANDesk\\VirusProtect6"));
    }

    dwResult = RegKeyDeleteAll(HKEY_LOCAL_MACHINE,       _T("Software\\INTEL\\LANDesk\\AMS2\\NetBiosEnable"));
    dwResult = RegKeyDeleteEmptyKey(HKEY_LOCAL_MACHINE,  _T("Software\\INTEL\\LANDesk\\AMS2"));

    dwResult = RegKeyDeleteEmptyKey(HKEY_LOCAL_MACHINE,  _T("Software\\INTEL\\LANDesk"));
    dwResult = RegKeyDeleteEmptyKey(HKEY_LOCAL_MACHINE,  _T("Software\\INTEL"));

    return bRet;
}


BOOL UnloadTransman( HMODULE *hTransman, BOOL bDontUnreg, const TCHAR* strParentServerName )
{
	BOOL	bRet = FALSE;
	FARPROC	fpUnload = NULL;

	if( FALSE == bDontUnreg )
	{
		fpUnload = GetProcAddress( *hTransman, "DllUnregisterServer" );
		if( fpUnload )
		{
			(fpUnload)();
			bRet = TRUE;
		}

        // If the transman dll needs to be unregistered, 
        // it needs to be cleaned up after too.
        DeleteTranmanRegKeys( strParentServerName );
	}
	else
		bRet = TRUE;

	FreeLibrary( *hTransman );
	hTransman = NULL;

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: ValidateServer();
//		ValidateServer - Used from the Select server UI to check if the
//		savserver is present. 
//
// Parameters:
//		MSIHANDLE
//
// Returns:
//		SERVERVALID property if server was validated
//
// Description:  
//	Validates that the property SERVERNAME points to a server that we 
//  can actually ping. If so then it sets the SERVERVALID property
//
//////////////////////////////////////////////////////////////////////////
// 8/22/03 - SKENNED function created.
// 12/13/04 - DKOWALYSHYN Function modified
//////////////////////////////////////////////////////////////////////////
UINT __stdcall ValidateServer( MSIHANDLE hInstall )
{
	HMODULE hTransman = NULL;
	UINT nRet = ERROR_SUCCESS;
	TCHAR szServerName[ MAX_PATH ] = {0};
	TCHAR szServerGroupName[ MAX_PATH ] = {0};
	TCHAR szTempDir[ MAX_PATH ] = {0};
	TCHAR szTemp[ MAX_PATH ] = {0};
	TCHAR szCurDir[ MAX_PATH ] = {0};
	DWORD dwLen = 0;
	BOOL bTMInit = FALSE;
	BOOL bTMReg = FALSE;
	BOOL bServerParentExist = FALSE;
	BOOL bIsServerPrimary = FALSE;
	int nResult = 0;

	//MessageBox( NULL, "Stop here.", "Debug", MB_OK );

	try
	{
		CoInitialize( NULL );

		if( g_pTrans )
		{
			delete g_pTrans ;
			g_pTrans = NULL;
		}
		// Have to set the current working directory, so all
		// of the DLL's can find each other.  This allows
		// me to restore it when we are done.
		GetCurrentDirectory( MAX_PATH, szCurDir );

		dwLen = sizeof( szTempDir );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, "TempFolder", szTempDir, &dwLen ) )
			throw( _T("Error getting TempFolder property") );
		AddSlash( szTempDir );

		SetCurrentDirectory( szTempDir );
		OutputDebugString( szTempDir );

		bTMReg = IsTransmanRegistered();
		hTransman = LoadTransman( szTempDir, bTMReg );
		if( hTransman == NULL )
			throw( _T("Failed to load transman.dll") );

		if( !TMInit() )
			throw( _T("TMInit failed") );
		// Set flag to let us know we need to stop transman
		bTMInit = TRUE;
		TMStartDomains();
		MSILogMessage( hInstall, _T("ValidateServer:TMStartDomains" ), TRUE );
		MSILogMessage( hInstall, szServerGroupName, TRUE );
	
		CString strTemp;
		strTemp.Format( "handle = %xd", hTransman );

		// Get servername 
		dwLen = sizeof( szServerName );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERNAME"), szServerName, &dwLen ) )
			throw( _T("Error getting SERVERNAME property") );
		MSILogMessage( hInstall, _T("MsiGetProperty: SERVERNAME" ), TRUE );
		MSILogMessage( hInstall, szServerName, TRUE );
		if ( 0 == lstrlen(szServerName) )
		{
				HWND hParent = GetForegroundWindow();
				CString strMessage, strTitle;
				strMessage.LoadString( IDS_ERROR_NO_SERVER_NAME );
				strTitle.LoadString( IDS_ERROR_NO_SERVER_NAME_TITLE );
				MessageBox( hParent, strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
				throw( _T("Error SERVERNAME property blank") );
		}

        // First see if the server was already discovered in the Browse dialog.
        BOOL inAddrCache = ServerExistsinAddressCache(szServerName);
        if ( FALSE == inAddrCache )
        {
            // Server not in address cache yet.  Go find it.
    
		    // Check NT servers first, then Netware
		    OutputDebugString("TMFindComputer");
		    OutputDebugString( szServerName );
		    nResult = TMFindComputer( szServerName, TRUE );
		    if( nResult )
			    nResult = TMFindComputer( szServerName, FALSE );
		    sssnprintf( szTemp, sizeof(szTemp), "TMFindComputer nResult = %d.",nResult);
		    MSILogMessage( hInstall, szTemp );

            // After doing find, see if the computer is in the address cache.
            inAddrCache = ServerExistsinAddressCache(szServerName);
        }
		
		// Check if the ServerExists in the AddressCache, otherwise it'll wait for transman to populate it.
		OutputDebugString("ServerExistsinAddressCache?");
		OutputDebugString( szServerName );
		if ( TRUE == inAddrCache )
			OutputDebugString("ValidateServer: ServerExistsinAddressCache true");			
		else
			OutputDebugString("ValidateServer: ServerExistsinAddressCache true");			

		// Check if server has a parent - aka Managed Secondary Server
		OutputDebugString("IsServerPrimary parent");
		OutputDebugString( szServerName );
		if ( FALSE == ServerHaveParent (szServerName ))
		{
			OutputDebugString("ValidateServer: ServerHaveParent false");
			bServerParentExist = FALSE;
		}
		else
		{
			OutputDebugString("ValidateServer: ServerHaveParent true");
			bServerParentExist = TRUE;
		}
		// Check if we have a primary parent server on this server group
		if( FALSE == IsServerPrimary( szServerName ))
		{
			OutputDebugString("IsServerPrimary FALSE");
			bIsServerPrimary = FALSE;
		}
		else
		{
			OutputDebugString("IsServerPrimary TRUE");
			bIsServerPrimary = TRUE;
		}
		// This condition validates a Server prior to allowing the client to attach to it.
		// ( servername is not blank AND TMFindComputer found the computer ) AND
		// ( either ( Server is a primary server ) OR ( Server is not primary and it has a Parent making it managed))
		if( ((lstrlen(szServerName) > 0) && (0 == nResult)) && ((bIsServerPrimary == TRUE) || ((bIsServerPrimary == FALSE) && (bServerParentExist == TRUE))) )
		{
			MSILogMessage( hInstall, _T("Server is valid, setting SERVERVALID=1"), TRUE );
			if( ERROR_SUCCESS != MsiSetProperty( hInstall, _T("SERVERVALID"), _T("1") ) )
				throw( _T("Error setting SERVERVALID property") );
		}
		else
		{
			MSILogMessage( hInstall, _T("Server cannot be verified, nResult."), TRUE );
			sssnprintf( szTemp, sizeof(szTemp), "nResult = %d.",nResult);
			MSILogMessage( hInstall, szTemp );
			MsiSetProperty( hInstall, _T("SERVERVALID"), NULL );
			// Connection Error Dialogs
			if ( ( FALSE == bIsServerPrimary ) && ( FALSE == bServerParentExist ) && ( 0 == nResult ) )
			{
				HWND hParent = GetForegroundWindow();
				CString strMessage, strTitle;
				strMessage.LoadString( IDS_ERROR_UNMANAGED_SECONDARY );
				strTitle.LoadString( IDS_ERROR_UNMANAGED_SECONDARY_TITLE );
				MessageBox( hParent, strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
			} 
			else // catch any unhandled connection errors
			{
				HWND hParent = GetForegroundWindow();
				CString strMessage, strTitle;
				strMessage.LoadString( IDS_ERROR_SERVERNOTFOUND );
				strTitle.LoadString( IDS_ERROR_SERVER_NOT_FOUND );
				MessageBox( hParent, strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
			}
		}
	}
	catch( TCHAR *szError )
	{
		MSILogMessage( hInstall, szError, TRUE );
	}

	// Clean up
	if( bTMInit )
	{
		TMStopTransman();
		TMFreeTransman();
	}
	
	if( hTransman )
		UnloadTransman( &hTransman, bTMReg, szServerName );

	SetCurrentDirectory( szCurDir );

	CoUninitialize();

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSIValidateAdminLogin();
//
// Parameters:
//		MSIHANDLE
//
// Returns:
//
// Description:  
//	During a secondary server migration from a PRESECURE to a SECURE version,
//	we need to validate the user credentials on the primary server.
//
//////////////////////////////////////////////////////////////////////////
// 1/24/04 - DKOWALYSHYN Function modified
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MSIValidateAdminLogin( MSIHANDLE hInstall )
{
	CString strServerName;
	CString strUserName;
	CString strPassword;
	CString strEncryptedPassword;
	CString strTitle;
	CString strLog;
	TCHAR	szTempDir[MAX_PATH] = {0};
	TCHAR	szCurDir[MAX_PATH] = {0};
	TCHAR	szTemp[ MAX_PATH ] = {0};
	TCHAR	*pszTemp = NULL;
	DWORD	dwLen = 0;
	BOOL	bTMReg = FALSE;
	BOOL	bTMInit = FALSE;

	CoInitialize( NULL );

	try
	{
		GetCurrentDirectory( MAX_PATH, szCurDir );

		HWND hWnd = GetForegroundWindow();
		if( !hWnd )
			hWnd = GetDesktopWindow();

		// - Set MSI Property defaults -
		MsiSetProperty( hInstall, _T("VALIDPASSWORD"), NULL );

		// - SERVERPARENT -
		dwLen = sizeof( szTemp );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERPARENT"), szTemp, &dwLen ) )
			throw( _T("Error getting SERVERPARENT property") );
		strServerName.Format( "%s", szTemp );
		//strLog.Format( "ValSecSerMigLog strServerName:%s", (strServerName));
		//OutputDebugString(strLog);

		// - Get SERVERGROUPPASS -
		dwLen = sizeof( szTemp );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERGROUPPASS"), szTemp, &dwLen ) )
			throw( "Error getting SERVERGROUPPASS property" );
		strPassword.Format( "%s", szTemp );
		//strLog.Format( "ValSecSerMigLog strPassword:%s", (strPassword));
		//OutputDebugString(strLog);

		// - Get SERVERGROUPUSERNAME -
		dwLen = sizeof( szTemp );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERGROUPUSERNAME"), szTemp, &dwLen ) )
			throw( "Error getting SERVERGROUPUSERNAME property" );
		strUserName.Format( "%s", szTemp );
		//strLog.Format( "ValSecSerMigLog strUserName:%s", (strUserName));
		//OutputDebugString(strLog);

		// Stream out the Transman dlls
		dwLen = sizeof( szTempDir );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, "TempFolder", szTempDir, &dwLen ) )
			throw( "Error getting TempFolder property" );
		AddSlash( szTempDir );

		SetCurrentDirectory( szTempDir );

		bTMReg = IsTransmanRegistered();
		OutputDebugString( szTempDir );

		if (LoadTransman() != ERROR_SUCCESS)
			throw( _T("Unable to load transman.dll") );

		if( !TMInit() )
			throw( _T("TMInit failed") );
		TMStartDomains();

		// Set flag to let us know we need to stop transman
		bTMInit = TRUE;

		/*strLog.Format( "ValSecSerMigLog - ValidateAdminRoles strServerName: %s, strUserName:%s, strPassword:%s",
			strServerName,strUserName,strPassword);
		OutputDebugString( strLog );*/

        ScsSecureComms::RESULT res = ScsSecureComms::RTN_FALSE;

		if( FALSE == ValidateAdminRoles ( hInstall, 
				                            strServerName.LockBuffer(), 
				                            strUserName.LockBuffer(), 
				                            strPassword.LockBuffer(),
                                            res ) )
		{
			OutputDebugString("ValidateAdminRoles FALSE");
			strServerName.UnlockBuffer();
			strUserName.UnlockBuffer();
			strPassword.UnlockBuffer();

            if( res == RTNERR_CERTNOTVALID || res == RTNERR_SENDFAILED_CERTREJECTED ||
                res == RTNERR_SENDFAILED_CERTINVALID || res == RTNERR_SENDFAILED_CERTEXPIRED )
            {
    			strLog.LoadString( IDS_ERROR_MIG_TIMEOUTOFSYNC );
            }
            else
            {
    			strLog.LoadString( IDS_ERROR_MIG_FAILEDCREDENTIALS );
            }
			strTitle.LoadString( IDS_ERROR_MIGSRV_PASS_TITLE );
			MessageBox( hWnd, strLog, strTitle, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK); 
			// Change in installer logic, if you cannot auth to the primary, you cannot migrate the secondary server
			MSILogMessage( hInstall, "MSIValidateAdminLogin: SERVERPARENT not set, User selected to continue without a primary server.", TRUE );
			throw( _T("MSIValidateAdminLogin: Primary server failed credentials, blocking secondary migration.") );
		}
		OutputDebugString("ValidateAdminRoles: UnlockBuffer strServerName strUserName strPassword");
		strServerName.UnlockBuffer();
		strUserName.UnlockBuffer();
		strPassword.UnlockBuffer();

		// Encrypt the password
		pszTemp = strEncryptedPassword.GetBuffer( PASS_MAX_CIPHER_TEXT_BYTES );
		MakeEP( pszTemp, PASS_MAX_CIPHER_TEXT_BYTES, PASS_KEY1, PASS_KEY2, strPassword, strPassword.GetLength()+1 );
		strEncryptedPassword.ReleaseBuffer();

//		strLog.Format("NewPass %s, NewEncrypt %s, strEncryptedPassword %s",
//			strPassword, pszTemp, strEncryptedPassword );
//		OutputDebugString ( strLog );

		MsiSetProperty( hInstall, _T("CONSOLEPASSWORD.2D6B2C77_9DB3_4019_A3E4_3F2892186836"), pszTemp );
		MsiSetProperty( hInstall, _T("CONSOLEPASSWORD"), pszTemp );
		MsiSetProperty( hInstall, _T("SERVERGROUPPASS"), strPassword );

		MsiSetProperty( hInstall, _T("VALIDPASSWORD"), _T("1") );
	}

	catch( TCHAR *szError )
	{
		OutputDebugString ( szError );
	}
	// Clean up
	if( bTMInit )
	{
		TMCleanupDomains();
		TMStopTransman();
		TMFreeTransman();
	}

	SetCurrentDirectory( szCurDir );

	CoUninitialize();

	return ERROR_SUCCESS;
}



//////////////////////////////////////////////////////////////////////////
//
// Function: ValidateServerMigration();
//		ValidateServerMigration - Check if the savserver is present. 
//		Ignore return code as it's not important for a silent
//	install, only that transman gets loaded and address cache populated.
//
// Parameters:
//		MSIHANDLE
//
// Returns:
//		SERVERVALID property if server was validated
//
// Description:  
//	Validates that the property SERVERNAME points to a server that we 
//  can actually ping. If so then it sets the SERVERVALID property
//
//////////////////////////////////////////////////////////////////////////
// 1/24/04 - DKOWALYSHYN Function modified
//////////////////////////////////////////////////////////////////////////
UINT __stdcall ValidateServerMigration( MSIHANDLE hInstall )
{
	HMODULE hTransman = NULL;
	UINT nRet = ERROR_SUCCESS;
	TCHAR szServerName[ MAX_PATH ] = {0};
	TCHAR szServerGroupName[ MAX_PATH ] = {0};
	TCHAR szPrimaryServer[ MAX_PATH ] = {0};
	TCHAR szPreSecureFound[ MAX_PATH ] = {0};
	TCHAR szServerParent[ MAX_PATH ] = {0};
	TCHAR szConsolePassword[ PASS_MAX_CIPHER_TEXT_BYTES ] = {0};
	TCHAR szDecryptedPassword[ PASS_MAX_PLAIN_TEXT_BYTES+1 ] = {0};
	TCHAR szTempDir[ MAX_PATH ] = {0};
	TCHAR szTemp[ MAX_PATH ] = {0};
	TCHAR szCurDir[ MAX_PATH ] = {0};
	TCHAR szUILevel[ MAX_PATH ] = {0};
	DWORD dwLen = 0;
	BOOL bTMInit = FALSE;
	BOOL bTMReg = FALSE;
	BOOL bServerParentExist = FALSE;
	BOOL bIsServerPrimary = FALSE;
	int nResult = 0;
	int uiLevel = 5;

	//MessageBox( NULL, "Stop here.", "Debug", MB_OK );
	try
	{
		CoInitialize( NULL );

		if( g_pTrans )
		{
			delete g_pTrans ;
			g_pTrans = NULL;
		}
		// Have to set the current working directory, so all
		// of the DLL's can find each other.  This allows
		// me to restore it when we are done.
		GetCurrentDirectory( MAX_PATH, szCurDir );

		dwLen = sizeof( szTempDir );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, "TempFolder", szTempDir, &dwLen ) )
			throw( _T("Error getting TempFolder property") );
		AddSlash( szTempDir );

		SetCurrentDirectory( szTempDir );
		OutputDebugString( szTempDir );

		bTMReg = IsTransmanRegistered();
		hTransman = LoadTransman( szTempDir, bTMReg );
		if( hTransman == NULL )
			throw( _T("Failed to load transman.dll") );

		if( !TMInit() )
			throw( _T("TMInit failed") );
		// Set flag to let us know we need to stop transman
		bTMInit = TRUE;
		TMStartDomains();
//		MessageBox( NULL, "TMVerifyDomain start.", "Debug", MB_OK );
		MSILogMessage( hInstall, _T("ValidateServer:TMStartDomains" ), TRUE );
		MSILogMessage( hInstall, szServerGroupName, TRUE );

		// Get Server Data 
		// - SERVERGROUPNAME -
		dwLen = sizeof( szServerGroupName );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERGROUPNAME"), szServerGroupName, &dwLen ) )
			throw( _T("Error getting SERVERGROUPNAME property") );
		// - PRIMARYSERVER -
		dwLen = sizeof( szPrimaryServer );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("PRIMARYSERVER"), szPrimaryServer, &dwLen ) )
			throw( _T("Error getting PRIMARYSERVER property") );
		// - SERVERPARENT -
		dwLen = sizeof( szServerParent );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERPARENT"), szServerParent, &dwLen ) )
			throw( _T("Error getting SERVERPARENT property") );
		// - SERVERNAME -
		dwLen = sizeof( szServerName );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERNAME"), szServerName, &dwLen ) )
			throw( _T("Error getting SERVERPARENT property") );
		// - CONSOLEPASSWORD -
		dwLen = sizeof( szConsolePassword );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("CONSOLEPASSWORD"), szConsolePassword, &dwLen ) )
			throw( _T("Error getting CONSOLEPASSWORD property") );
		// - UILevel -
		DWORD dwSize = sizeof( szUILevel );
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, "UILevel", szUILevel, &dwSize ) )
		{
			uiLevel = atoi( szUILevel );
		}
		vpsnprintf(szTemp, sizeof (szTemp), "szUILevel = %s, uiLevel=%d.",szUILevel, uiLevel);
		MSILogMessage( hInstall, szTemp );

		if( lstrlen(szServerName) == 0 ) // Servername isn't known so attempt to get it from Transman
		{
			if( FALSE == TMVerifyDomain( szServerGroupName, szServerName ) )
			{
				throw( _T("Unable to use server, old version") );
			}
		}
		vpsnprintf(szTemp, sizeof (szTemp), "szServerParent = %s.",szServerParent);
		MSILogMessage( hInstall, szTemp );
		vpsnprintf(szTemp, sizeof (szTemp), "szServerGroupName = %s.",szServerGroupName);
		MSILogMessage( hInstall, szTemp );
		vpsnprintf(szTemp, sizeof (szTemp), "szServerName = %s.",szServerName);
		MSILogMessage( hInstall, szTemp );

		// Check if we have a primary parent server on this server group
		if( NULL != _tcsstr(szPrimaryServer,"1" ))
		{
			OutputDebugString("IsServerPrimary TRUE");
			bIsServerPrimary = TRUE;
		}
		else
		{
			OutputDebugString("IsServerPrimary FALSE");
			bIsServerPrimary = FALSE;
            // Only on secondary server See if we can contact the target parent server 
			// Check NT servers first, then Netware
			OutputDebugString("TMFindComputer");
			OutputDebugString( szServerName );
			nResult = TMFindComputer( szServerName, TRUE );
			if( nResult )
				nResult = TMFindComputer( szServerName, FALSE );
			vpsnprintf(szTemp, sizeof (szTemp), "TMFindComputer nResult = %d.",nResult);
			MSILogMessage( hInstall, szTemp );

			// If not primaryserver then Check if server has a parent - aka Managed Secondary Server
			OutputDebugString("ServerHaveParent?");
			OutputDebugString( szServerName );
	
			if ( (0 < lstrlen(szServerParent))) 
			{
				if ( 0 < lstrlen(szServerName) ) // If our servername is blank then lets try to find one
				{
					if (TRUE == ServerExistsinAddressCache (szServerName ))
					{
						OutputDebugString("ServerExistsinAddressCache: Server does exist");
						bServerParentExist = TRUE;
						// We know the SERVERPARENT so set the property so that we can pull the Parent & Guid values from the parent
						if( ERROR_SUCCESS != MsiSetProperty( hInstall, "SERVERPARENT", szServerName ) )
						{
							throw( "MsiSetProperty SERVERPARENT failed" );
						}
						else
						{
							MSILogMessage( hInstall, _T("MsiSetProperty SERVERPARENT set"), TRUE );
						}
					}
					else
					{
						OutputDebugString("ServerExistsinAddressCache: Server does not exist");
						bServerParentExist = FALSE;
					}
				}
				else
				{
					OutputDebugString("ServerExistsinAddressCache: szServerName blank, server can't be located");
					bServerParentExist = FALSE;
				}
			}
			else
			{
				OutputDebugString("ValidateServer: szServerParent is empty.");
				bServerParentExist = FALSE;
			}
		}

		// This condition validates a Server prior to allowing the client to attach to it.
		// ( servername is not blank AND TMFindComputer found the computer ) AND
		// ( either ( Server is a primary server ) OR ( Server is not primary and it has a Parent making it managed))
		if( (bIsServerPrimary == TRUE) || ((0 == nResult) && (bIsServerPrimary == FALSE) && (bServerParentExist == TRUE)))
		{
	        // Also set the decrypted password MSI property.
            UnMakeEP( szConsolePassword, sizeof(szConsolePassword), PASS_KEY1, PASS_KEY2, szDecryptedPassword, sizeof(szDecryptedPassword) );
			MsiSetProperty( hInstall, _T("SERVERGROUPPASS"), szDecryptedPassword );
//			OutputDebugString ( szConsolePassword ); // Encrypted password
//			OutputDebugString ( szDecryptedPassword ); // Decrypted password
			MSILogMessage( hInstall, _T("Server is valid, setting SERVERVALID=1"), TRUE );
			if( ERROR_SUCCESS != MsiSetProperty( hInstall, _T("SERVERVALID"), _T("1") ) )
				throw( _T("Error setting SERVERVALID property") );
		}
		else
		{
			MSILogMessage( hInstall, _T("Server cannot be verified, nResult."), TRUE );
			vpsnprintf(szTemp, sizeof (szTemp), "nResult = %d.",nResult);
			MSILogMessage( hInstall, szTemp );
			MsiSetProperty( hInstall, _T("SERVERVALID"), NULL );
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				// Connection Error Dialogs
				if ( ( FALSE == bIsServerPrimary ) && ( FALSE == bServerParentExist ) )
				{
					if ( 0 == nResult ) 
					{
						HWND hParent = GetForegroundWindow();
						CString strMessage, strTitle;
						strMessage.LoadString( IDS_ERROR_UNMANAGED_SECONDARY_SERVER );
						strTitle.LoadString( IDS_ERROR_UNMANAGED_SECONDARY_SERVER_TITLE );
						MessageBox( hParent, strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
					}
					else if ( ERROR_COMPUTER_NOT_FOUND == nResult )
					{
						HWND hParent = GetForegroundWindow();
						CString strMessage, strTitle;
						strMessage.LoadString( IDS_ERROR_SEC_SRV_MIG );
						strTitle.LoadString( IDS_ERROR_UNMANAGED_SECONDARY_SERVER_TITLE );
						MessageBox( hParent, strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
					}
				}
				else // catch any unhandled connection errors
				{
					HWND hParent = GetForegroundWindow();
					CString strMessage, strTitle;
					strMessage.LoadString( IDS_ERROR_SERVERNOTFOUND );
					strTitle.LoadString( IDS_ERROR_SERVER_NOT_FOUND );
					MessageBox( hParent, strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
				}
			}
		}
	}
	catch( TCHAR *szError )
	{
		MSILogMessage( hInstall, szError, TRUE );
		nRet = ERROR_INSTALL_FAILURE;
	}

	// Clean up
	if( bTMInit )
	{
		TMStopTransman();
		TMFreeTransman();
	}

	if( hTransman )
		UnloadTransman( &hTransman, bTMReg, szServerName );

	SetCurrentDirectory( szCurDir );

	CoUninitialize();

	return nRet;
}



//////////////////////////////////////////////////////////////////////////
//
// Function: ValidateServerGroup()
//
// Parameters:
//		MSIHANDLE
//
// Returns:
//
// Description: This function Validates the ServerGroup data for a 
//	SAV / SCS Install.
//
//////////////////////////////////////////////////////////////////////////
// 8/22/03 - SKENNED function created.
// 8/31/04 - RSUTTON function modified for the SPF45 world
//////////////////////////////////////////////////////////////////////////
UINT __stdcall ValidateServerGroup( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;
	CString strMessage;
	CString strTitle;
	CString strGroupName;
	CString strPassword;
	CString strServerGroupUsername;
	CString strParent;
	CString strBuiltin_Role_Admin = BUILTIN_ROLE_ADMIN;
	TCHAR *pszTemp = NULL;
	TCHAR szTemp[ MAX_PATH ] = {0};
	TCHAR szTempDir[ MAX_PATH ] = {0};
	TCHAR szCurDir[ MAX_PATH ] = {0};
	TCHAR szUILevel[ MAX_PATH ] = {0};
	BOOL bTMInit = FALSE;
	BOOL bTMReg = FALSE;
	DWORD dwLen = 0;
	HWND hWnd;
	int nResult = 0;
	int uiLevel = 5;
	CTime tcurrent,tend;
	HMODULE	hMod = NULL;

	//MessageBox( NULL, "Stop here.", "Debug", MB_OK );
	OutputDebugString ("ValidateServerGroup");

	CoInitialize( NULL );

	try
	{
		// Have to set the current working directory, so all
		// of the DLL's can find each other.  This allows
		// me to restore it when we are done.
		GetCurrentDirectory( MAX_PATH, szCurDir );

		// Set default title for error message boxes
		strTitle.LoadString( IDS_CLIENT_LINK );
		hWnd = GetForegroundWindow();
		if( !hWnd )
			hWnd = GetDesktopWindow();

		// Set default property values
		MsiSetProperty( hInstall, _T("VALIDSERVERGROUP"), NULL );
		MsiSetProperty( hInstall, _T("SERVERGROUPEXISTS"), NULL );
		MsiSetProperty( hInstall, _T("CONSOLEPASSWORD"), NULL );
		MsiSetProperty( hInstall, _T("ENCRYPTSERVERGROUPUSERNAME"), NULL );

		// Get servername 
		dwLen = sizeof( szTemp );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERGROUPNAME"), szTemp, &dwLen ) )
			throw( "Error getting SERVERGROUPNAME property" );
		strGroupName.Format( "%s", szTemp );

		// Get password
		dwLen = sizeof( szTemp );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERGROUPPASS"), szTemp, &dwLen ) )
			throw( "Error getting SERVERGROUPPASS property" );
		strPassword.Format( "%s", szTemp );

		// Get SERVERPARENT
		dwLen = sizeof( szTemp );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERPARENT"), szTemp, &dwLen ) )
			throw( "Error getting SERVERPARENT property" );
		strParent.Format( "%s", szTemp );
		vpsnprintf(szTemp, sizeof (szTemp), "ValidateSRVGRP: SERVERPARENT = strParent = %s.",strParent );
		MSILogMessage( hInstall, szTemp );

		// Get SERVERGROUPUSERNAME
		dwLen = sizeof( szTemp );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("SERVERGROUPUSERNAME"), szTemp, &dwLen ) )
			throw( "Error getting SERVERGROUPUSERNAME property" );
		strServerGroupUsername.Format( "%s", szTemp );

		// - UILevel -
		DWORD dwSize = sizeof( szUILevel );
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, "UILevel", szUILevel, &dwSize ) )
		{
			uiLevel = atoi( szUILevel );
		}
		vpsnprintf(szTemp, sizeof (szTemp), "szUILevel = %s, uiLevel=%d.",szUILevel, uiLevel);
		MSILogMessage( hInstall, szTemp );

		// Quick check for some obvious errors
		// empty ServerGroupname
		if( strGroupName.IsEmpty() ) 
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_ERROR_BLANKGROUP );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Bad server group name") );
		}

		// Servergroupname longer then MAX_SERVER_NAME ( 48 chars )
		if( strGroupName.GetLength() > SYM_MAX_SERVERGROUP_NAME_LEN ) 
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_ERROR_SERVER_NAME_TOO_LONG );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Server group name too long") );
		}

		// JHoa - Forward port - JHoa - Defect 1-3XVE7G - Do not allow first character to be # for server group name
		if( 0 == strGroupName.FindOneOf( "#" ) )
		{
			if( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE
			{
				strMessage.LoadString( IDS_ERROR_GROUPNAME_NOPOUND );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Bad server group name") );
		}

		if( -1 != strGroupName.Find( '\\' ) )
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_ERROR_BADGROUPNAME );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Bad server group name") );
		}

		// empty ServerGroupUsername
		if( strServerGroupUsername.IsEmpty() ) 
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_BLANK_USERNAME );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Bad server group username") );
		}

		// long ServerGroupUsername
		if( strServerGroupUsername.GetLength() > SYM_MAX_USERNAME_LEN) 
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_ERROR_USERNAME_TOO_LONG );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("server group username too long") );
		}

		if( strPassword.IsEmpty() )
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_ERROR_BLANKPASS );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Blank password passed.") );
		}

		if( strPassword.GetLength() < MIN_PASSWORD_LENGTH ) 
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_ERROR_SMALLPASS );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Short password passed.") );
		}

		if( strPassword.GetLength() > SYM_MAX_PASSWORD_LEN ) 
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				strMessage.LoadString( IDS_ERROR_PASSWORD_TOO_LONG );
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
			}
			throw( _T("Password too long.") );
		}

		// Stream out the Transman dlls
		dwLen = sizeof( szTempDir );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, "TempFolder", szTempDir, &dwLen ) )
			throw( "Error getting TempFolder property" );
		AddSlash( szTempDir );

		SetCurrentDirectory( szTempDir );
		bTMReg = IsTransmanRegistered();
		hMod = LoadTransman( szTempDir, bTMReg );
		if( NULL == hMod )
		{
			OutputDebugString( "Unable to load transman.dll" );
			throw( _T("Unable to load transman.dll") );
		}

		if( !TMInit() )
			throw( _T("TMInit failed") );
		TMStartDomains();
		// Set flag to let us know we need to stop transman
		bTMInit = TRUE;
		// This code will put a min time limit of 5 seconds through this action, to fix Transman bug
		// causing dep exception in XPSP2. Can't start & shutdown Transman too fast or else it chokes.
		tcurrent = CTime::GetCurrentTime();
		tend = tcurrent + CTimeSpan( 0, 0, 0, 5 );

		if( TRUE == strParent.IsEmpty() )
		{
			// Validate server group
			memset( szTemp, 0, sizeof( szTemp ) );
			if( FALSE == TMVerifyDomain( strGroupName.LockBuffer(), szTemp ) )
			{
				if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
				{
					strGroupName.UnlockBuffer();
					strMessage.LoadString( IDS_ERROR_OLDVERSION );
					::MessageBox( hWnd, strMessage, strTitle, MB_OK );
				}
				throw( _T("Unable to use server, old version") );
			}
			strGroupName.UnlockBuffer();
			strParent.Format( "%s", szTemp );
		}

		// If we have a parent server then verifiy it's running a SSL (Secure) version, if not then skip
		if( FALSE == strParent.IsEmpty() )
		{
            // Make sure the primary server of this group has been resolved, or the TMIsServerSecure
            // will fail
            CString             parentNameUpper;
            SERVERLOCATION      sl;
            DWORD               returnValDW         = ERROR_OUT_OF_PAPER;
            
            parentNameUpper = strParent;
            parentNameUpper.MakeUpper();
            ZeroMemory(&sl, sizeof(sl));
            strncpy(sl.FindServer, (LPCTSTR) parentNameUpper, NAME_SIZE);
            sl.Flags = SL_NAME_IS_NETWARE;
            returnValDW = g_pTrans->LocateServer(&sl);
            if (returnValDW != ERROR_SUCCESS)
            {
                // If it didn't find it, look for NT
                sl.Flags = SL_NAME_IS_NT;
                returnValDW = g_pTrans->LocateServer(&sl);
            }
            if (returnValDW != ERROR_SUCCESS)
            {
                strMessage.FormatMessage(IDS_ERROR_CANTCONTACTPRIMARY,(LPCTSTR) strParent, (LPCTSTR) strGroupName);
                if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
                    ::MessageBox(hWnd, strMessage, strTitle, MB_OK);
                else
					MSILogMessage(hInstall, _T("Cannot contact primary server for group!"));
                throw( _T("Unable to use server, can't contact primary!"));
            }

			// Check if the server group entered is running a SSL (Secure) SAV version, prompt and cancel if not
            if ( FALSE == TMIsServerSecure ( strParent.LockBuffer() ) )
			{
				OutputDebugString("TMIsServerSecure = FALSE");
				if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
                {
                    strMessage.LoadString( IDS_ERROR_NONSECURE );
					::MessageBox( hWnd, strMessage, strTitle, MB_OK );
                }
				strParent.ReleaseBuffer();
				throw( _T("Unable to use server, pre-ssl version detected") );
			}
			// TMIsServerSecure must be true
			OutputDebugString("TMIsServerSecure = TRUE");
			strParent.ReleaseBuffer();
		}

		// Check if we have a parent server group
		if( FALSE == strParent.IsEmpty() )
		{
			OutputDebugString("ValidateAdminRoles");
			OutputDebugString( strParent );

            ScsSecureComms::RESULT res = ScsSecureComms::RTN_FALSE;

			if( FALSE == ValidateAdminRoles ( hInstall, 
				                              strParent.LockBuffer(), 
				                              strServerGroupUsername.LockBuffer(), 
				                              strPassword.LockBuffer(),
                                              res ) )
			{
				OutputDebugString("ValidateAdminRoles FALSE");
				strParent.UnlockBuffer();
				strServerGroupUsername.UnlockBuffer();
				strPassword.UnlockBuffer();

                if( res == RTNERR_CERTNOTVALID || res == RTNERR_SENDFAILED_CERTREJECTED ||
                    res == RTNERR_SENDFAILED_CERTINVALID || res == RTNERR_SENDFAILED_CERTEXPIRED )
                {
    				strMessage.LoadString( IDS_ERROR_TIMEOUTOFSYNC );
                }
                else
                {
    				strMessage.LoadString( IDS_ERROR_FAILEDCREDENTIALS );
                }

				if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
				{
					::MessageBox( hWnd, strMessage, strTitle, MB_OK );
				}
				throw( _T("Unable to authenticate credentials, cannot continue install.") );
			}
			OutputDebugString("ValidateAdminRoles strParent.UnlockBuffer");
			strParent.UnlockBuffer();
			OutputDebugString("ValidateAdminRoles strServerGroupUsername.UnlockBuffer");
			strServerGroupUsername.UnlockBuffer();
			OutputDebugString("ValidateAdminRoles strPassword.UnlockBuffer");
			strPassword.UnlockBuffer();
			OutputDebugString("ValidateAdminRoles credentials are valid");

			// Check if we have a primary parent server on this server group
			OutputDebugString("IsServerPrimary parent");
			OutputDebugString( strParent );
			if( FALSE == IsServerPrimary( strParent.LockBuffer() ))
			{
				OutputDebugString("IsServerPrimary FALSE");
				strParent.UnlockBuffer();
				if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
				{
					strMessage.LoadString( IDS_ERROR_NOPRIMARY );
					int ret = ::MessageBox( hWnd, strMessage, strTitle, MB_YESNO ); 
					if ( IDYES == ret )
					{	// This is a case with multiple secondary server installs without a primary - SSC would fix it later
						OutputDebugString("IsServerPrimary - User selected to continue without a primary server.");
						MsiSetProperty( hInstall, _T("SERVERPARENT"), NULL );
						MSILogMessage( hInstall, "SERVERPARENT not set,User selected to continue without a primary server.", TRUE );
					}
					else if ( IDNO == ret )
					{
						throw( _T("User selected not to continue without a primary server.") );
					}
				}
				else
				{	// This is a SILENT case with multiple secondary server installs without a primary - SSC would fix it later
					OutputDebugString("IsServerPrimary - User selected to continue without a primary server.");
					MsiSetProperty( hInstall, _T("SERVERPARENT"), NULL );
					MSILogMessage( hInstall, "SERVERPARENT not set,User selected to continue without a primary server.", TRUE );
				}
			}
			else
			{
				OutputDebugString("IsServerPrimary Continue");
				MsiSetProperty( hInstall, _T("SERVERPARENT"), strParent );
				sssnprintf( szTemp, sizeof(szTemp), "SERVERPARENT=%s", strParent );
				MSILogMessage( hInstall, szTemp, TRUE );
			}
		}
		else
		{
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				CVerifySGPass verifyDlg( strPassword, CWnd::FromHandle(hWnd) );
				if( verifyDlg.DoModal() != IDOK)
					throw( _T("User aborted new group creation") );
			}
		}
		if (TRUE == ServerExistsinAddressCache (strParent.LockBuffer() ))
		{
			OutputDebugString("ServerExistsinAddressCache: Server does exist");
		}
		else
		{
			OutputDebugString("ServerExistsinAddressCache: Server does not exist");
		}
		strParent.UnlockBuffer();

		// Encrypt the password
        char szTempPassword[PASS_MAX_CIPHER_TEXT_BYTES] = "";

		MakeEP( szTempPassword, sizeof(szTempPassword), PASS_KEY1, PASS_KEY2, strPassword, strPassword.GetLength()+1 );
		strPassword = szTempPassword;

		// Encrypt the Servergroup Username and return the ENCRYPTSERVERGROUPUSERNAME property
		pszTemp = strServerGroupUsername.GetBuffer( PASS_MAX_CIPHER_TEXT_BYTES );
		MakeEP( pszTemp, PASS_MAX_CIPHER_TEXT_BYTES, PASS_KEY3, PASS_KEY4, strBuiltin_Role_Admin, strBuiltin_Role_Admin.GetLength()+1 );
		strServerGroupUsername.ReleaseBuffer();
		MsiSetProperty( hInstall, _T("ENCRYPTSERVERGROUPUSERNAME"), strServerGroupUsername );
//		wsprintf( szTemp, "ENCRYPTSERVERGROUPUSERNAME=%s", strServerGroupUsername );
//		MSILogMessage( hInstall, szTemp, TRUE );

		// Validate the password
        BOOL bTest = TRUE;
		if( ( strParent.IsEmpty() )  || ( TRUE == bTest ) )
		{
			MsiSetProperty( hInstall, _T("CONSOLEPASSWORD"), strPassword );
			MsiSetProperty( hInstall, _T("VALIDSERVERGROUP"), _T("1") );
			nRet = ERROR_SUCCESS;
		}
		else
		{
			strMessage.LoadString( IDS_ERROR_NOMATCHSERVER );
			if ( uiLevel > 2 ) // No prompts for Silent install, 2 == INSTALLUILEVEL_NONE 
			{
				::MessageBox( hWnd, strMessage, strTitle, MB_OK | MB_ICONERROR );
				MsiSetProperty( hInstall, _T("VALIDSERVERGROUP"), NULL );
				MsiSetProperty( hInstall, _T("CONSOLEPASSWORD"), NULL );
			}
		}

	}
	catch( TCHAR *szError )
	{
		MsiSetProperty( hInstall, _T("VALIDSERVERGROUP"), NULL );
		MSILogMessage( hInstall, szError, TRUE );
		OutputDebugString ( szError );
		if ( uiLevel < 3 ) // In Silent install if we catch any error then fail the installer 
		{
			nRet = ERROR_INSTALL_FAILURE;
		}
	}

	// Clean up
	if( bTMInit )
	{
		while ( tcurrent <= tend ) // this is the delay exist code
		{
			Sleep (100);
			tcurrent = CTime::GetCurrentTime();
		}
		TMCleanupDomains();
		TMStopTransman();
		TMFreeTransman();
	}
	
	SetCurrentDirectory( szCurDir );

	CoUninitialize();

	return nRet;
}


// ---------------------------------------------------------------------------
// Routine:	CheckInstallPath
//
// Purpose:	Make sure install path contains valid information
//
// ---------------------------------------------------------------------------
// 04/11/02 Joe Antonelli Created function
// 10/03/03 Sean Kennedy - Updated function and imported to SCF project
// 12/18/03 Sean Kennedy - Imported to SCS project
// 02/22/04 Dan Kowalyshyn - Changes to block large install paths
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) UINT __stdcall CheckInstallPath( MSIHANDLE hInstall )
{
    UINT	nRet = ERROR_SUCCESS;
    TCHAR	lpszPath[DOUBLE_MAX_PATH];
	TCHAR	szTemp[DOUBLE_MAX_PATH];
    DWORD	dwSize = DOUBLE_MAX_PATH;
    bool	bValidPath = true;
    int     iLen = 0;

	try
	{
		if ( ERROR_SUCCESS != MsiGetProperty( hInstall, "INSTALLDIR", lpszPath, &dwSize ) )
			throw( (UINT)IDS_ERR_NOPATH );
		vpsnprintf(szTemp, sizeof (szTemp), "INSTALLDIR = %s.",lpszPath);
		MSILogMessage( hInstall, szTemp, TRUE );

		// Do not allow NULL path
		if ( !lpszPath )
			throw( (UINT)IDS_ERR_BLANKPATH );

		// path must be greater than 3 characters (path too short)
		iLen = lstrlen(lpszPath);
		if ( iLen <= 3 )
			throw( (UINT)IDS_ERR_PATHTOOSHORT );

		// path must not be greater then MAX_PATH_LENGTH, need room for pki directory & certs 
		if ( iLen >= MAX_PATH_LENGTH )
		{
			vpsnprintf(szTemp, sizeof (szTemp), "INSTALLDIR length = %d.",iLen);
			MSILogMessage( hInstall, szTemp, TRUE );
			throw( (UINT)IDS_ERR_PATHTOOLONG );
		}

		TCHAR szPath[MAX_PATH];
		lstrcpy(szPath, lpszPath);
		if ((szPath[ iLen-1 ] == _T('\\')) && !IsDBCSLeadByte(BYTE(szPath[ iLen-2 ])))
			szPath[ iLen - 1 ] = 0;

		// Ensure no portion of path includes leading spaces.
		// (i.e.  "c:\ program files\  ssc\blah".
		LPCTSTR pszChar = _tcschr(szPath, _T('\\'));

		while (pszChar && *pszChar && (_T(' ') != *CharNext(pszChar)))
		{
			pszChar = CharNext(pszChar);
			pszChar = _tcschr(pszChar, _T('\\'));
		}

		if (pszChar && *pszChar && (_T(' ') == *CharPrev(szPath, pszChar)))
			throw( (UINT)IDS_ERR_NOLEADSPACE );

		// Ensure no portion of the path includes a ';' character
		// i.e. "c:\progra\a;b\SSC"
		pszChar = _tcschr(szPath, _T('\\'));

		while (pszChar && *pszChar &&  (_T(';') != *CharNext(pszChar)))
		{
			pszChar = CharNext(pszChar);
		}

		if (pszChar && *pszChar &&  (_T(';') == *CharNext(pszChar)))
			throw( (UINT)IDS_ERR_NOSEMICOLON );

		// Ensure no portion of the path includes a '&' character
		// i.e. "c:\Joe\SSC&\"
		pszChar = _tcschr(szPath, _T('\\'));
		while (pszChar && *pszChar &&  (_T('&') != *CharNext(pszChar)))
		{
			pszChar = CharNext(pszChar);
		}

		if (pszChar && *pszChar &&  (_T('&') == *CharNext(pszChar)))
			throw( (UINT)IDS_ERR_NOAMPERSAND );
	}
	catch( UINT nErr )
	{
		// Set valid path to false for any exceptions we trap
		bValidPath = false;

		// Get our error strings
		CString strMessage;
		CString strTitle;
		strTitle.LoadString( IDS_PATHTITLE );
		strMessage.LoadString( nErr );

		// Check the UI Level
		TCHAR szUILevel[ MAX_PATH ];
		DWORD dwSize = sizeof( szUILevel );
		if ( ERROR_SUCCESS == MsiGetProperty( hInstall, "UILevel", szUILevel, &dwSize ) )
		{
			int uiLevel = atoi( szUILevel );

			if( uiLevel == 2 )
			{
				// In silent mode there is nothing we can do to fix the path so we must abort
				nRet = ERROR_INSTALL_USEREXIT;
			}
			else
			{
				// If in interactive mode display a message
				HWND hWnd = GetForegroundWindow();
				if( !hWnd )
					hWnd = GetDesktopWindow();
				// format the strMessage to include the MAX_PATH_LENGTH in the messagebox
				if ( nErr == IDS_ERR_PATHTOOLONG )
				{
					strMessage.Format(IDS_ERR_PATHTOOLONG, iLen, MAX_PATH_LENGTH);
				}
				MessageBox( hWnd, strMessage, strTitle, MB_OK );
			}
		}

		// Always log the message
		MSILogMessage( hInstall, strMessage.LockBuffer(), TRUE );
		strMessage.UnlockBuffer();
	}

    if( true == bValidPath )
    {
        MsiSetProperty( hInstall, "ValidInstallDir", "1" );
    }
    else
    {
        MsiSetProperty( hInstall, "ValidInstallDir", "0" );
    }

	return nRet;

}

/////////////////////////////////////////////////////////////////////////
//
// Function: MSILogMessage()
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//		TCHAR [] - Text to log
//		BOOL - TRUE if only OutputDebugString is needed
// Returns:
//		NONE
//
// Description:  
//	This function logs messages to the MSI log file
//
//////////////////////////////////////////////////////////////////////////
// 8/07/01 - SKENNED function created.
//////////////////////////////////////////////////////////////////////////
void MSILogMessage( MSIHANDLE hInstall, TCHAR szString[], BOOL bDoAction )
{
	CString szLogLine( LOG_PREFIX );

	szLogLine += szString;

	if( FALSE == bDoAction )
	{
		PMSIHANDLE hRec = MsiCreateRecord( 1 );

		if( hRec )
		{
			MsiRecordClearData( hRec );
			MsiRecordSetString( hRec, 0, (LPCSTR)szLogLine );
			MsiProcessMessage( hInstall, INSTALLMESSAGE_INFO, hRec );
		}
	}

	OutputDebugString( szLogLine );
}

/////////////////////////////////////////////////////////////////////////
//
// Function: IsLicensedOverUnlicensed
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCESS
//
// Description:  
//	This function checks for installing a licensed version over an unlicensed version
//    - set the LICENSEDOVERUNLICENSED property
//  This is only called if installing a licensed version
//
//////////////////////////////////////////////////////////////////////////
// 6/14/04 - Jim Shock - function created.
//////////////////////////////////////////////////////////////////////////
// void IsLicensedOverUnlicensed( MSIHANDLE hInstall )
extern "C" __declspec(dllexport) UINT __stdcall IsLicensedOverUnlicensed( MSIHANDLE hInstall )
{

	MSILogMessage( hInstall, _T("Checking for Licensed over Unlicensed migration...") );

	// determine if the existing product is licensed or unlicensed
	DWORD dwLicensed = 0;
	HKEY hKey = NULL;
	DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\Licensing", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		SymSaferRegQueryValueEx(hKey, "Enabled", NULL, NULL, LPBYTE(&dwLicensed), &dwSize);
		RegCloseKey(hKey);
	}
	if (dwLicensed == 0)
	{
		MSILogMessage( hInstall, _T("Doing Licensed over Unlicensed migration.") );
		MsiSetProperty( hInstall, _T("LICENSEDOVERUNLICENSED"), _T("1") );
	}
	else
	{
		MSILogMessage( hInstall, _T("Not doing Licensed over Unlicensed migration.") );
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////
//
// Function: IsUnlicensedOverLicensed
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
// Returns:
//		ERROR_SUCCESS
//
// Description:  
//	This function checks for installing an unlicensed version over a licensed version
//    - set the UNLICENSEDOVERLICENSED property
//  This is only called if installing an unlicensed version
//
//////////////////////////////////////////////////////////////////////////
// 6/14/04 - Jim Shock - function created.
//////////////////////////////////////////////////////////////////////////
// void IsUnlicensedOverLicensed( MSIHANDLE hInstall )
extern "C" __declspec(dllexport) UINT __stdcall IsUnlicensedOverLicensed( MSIHANDLE hInstall )
{

	MSILogMessage( hInstall, _T("Checking for Unlicensed over Licensed migration...") );

	// determine if the existing product is licensed or unlicensed
	DWORD dwLicensed = 0;
	HKEY hKey = NULL;
	DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\Licensing", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
	if (dwResult == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(DWORD);
		SymSaferRegQueryValueEx(hKey, "Enabled", NULL, NULL, LPBYTE(&dwLicensed), &dwSize);
		RegCloseKey(hKey);
	}
	if (dwLicensed == 1)
	{
		MSILogMessage( hInstall, _T("Doing Unlicensed over Licensed migration.") );
		MsiSetProperty( hInstall, _T("UNLICENSEDOVERLICENSED"), _T("1") );
	}
	else
	{
		MSILogMessage( hInstall, _T("Not doing Unlicensed over Licensed migration.") );
	}

	return ERROR_SUCCESS;

}


/////////////////////////////////////////////////////////////////////////
//
// Function: ValidateAdminRoles()
//
// Parameters:	HMODULE hTransman - handle to transman module
//				TCHAR* strPrimaryServerName - servergroup name
//				TCHAR* strUserName - Username
//				TCHAR* szPassword - Password
//
// Returns:		BOOL 
//					TRUE - Credentials authenticated
//					FALSE - Invalid credentials 
//
// Description:  This funtion connects to a remote primary server and
//					authenticates a set of user credentials to determine
//					if this user has rights to install & join a new
//					secondary server on to an existing primary server.
//	
//
//////////////////////////////////////////////////////////////////////////
// 11/12/04 - DKOWALYSYN function created.
//////////////////////////////////////////////////////////////////////////
BOOL ValidateAdminRoles ( MSIHANDLE hInstall,
							TCHAR* strPrimaryServerName,
							TCHAR* strUserName,
							TCHAR* szPassword,
                            ScsSecureComms::RESULT& hResult )
{
	typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 IUser,
                                 &SYMOBJECT_User,
                                 &IID_User> CUserLoader;

    CSCSSecureLogin * ptrSecureLogin = NULL;
    SYMRESULT											symres, nError;
	HMODULE transman									= NULL;
	DWORD	transhan									= 0;
	DWORD	ret											= 0;
    TCHAR	szMessString [MAX_PATH]						= {0};
	TCHAR	szTransmanPath [MAX_PATH]					= {0};
	TCHAR	szSCSCommsPath [MAX_PATH]					= {0};
	BOOL	bwAccess									= FALSE;
	BOOL	bwCredentialsValid							= FALSE;
	CAuthServerLoader									objAuthServerLdr;
    CUserLoader											objUserLdr;
	const std::string &strUsername						= strUserName; 
	const std::string &strPassword						= szPassword;
	std::vector <const char *>							ServerNameVector;

    hResult = RTNERR_FAILED;

//	Only uncomment the logline below for testing as it exposes the username / password
//	wsprintf( szMessString, "User: %s, Pass: %s, Dom: %s", strUsername.c_str(), strPassword.c_str(), strPrimaryServerName ); 
//	OutputDebugString( szMessString );
	// Init the loader objects
    symres = LoadScsComms();
	if ( symres != SYM_OK )
	{
		sssnprintf( szMessString, sizeof(szMessString), "LoadScsComms - failed." ); 
		MSILogMessage( hInstall, szMessString );
		goto AuthAdminRoles_Fail;
	}

	try
	{
		TCHAR szCompName[MAX_PATH]						= {0};
		TCHAR szTempPath[MAX_PATH]						= {0};

		DWORD size = sizeof (szCompName);
		GetComputerName (szCompName, &size);
		ptrSecureLogin = new CSCSSecureLogin();
		GetTempPath ( MAX_PATH, szTempPath );
		ptrSecureLogin->Init ( szTempPath, szCompName );
		strcpy ( szTransmanPath, szTempPath );
		strcpy ( szSCSCommsPath, szTempPath );
		strcat ( szTransmanPath, g_szPathToTransmanDll ); // Create transman path to temp dir
		strcat ( szSCSCommsPath, g_szPathToScsCommsDll ); // Create scscomms path to temp dir
	
	}
	catch (std::bad_alloc &)
	{
		sssnprintf( szMessString, sizeof(szMessString), "AuthAdminRoles - bad_alloc catch." ); 
		MSILogMessage( hInstall, szMessString );
		goto AuthAdminRoles_Fail;
	}

	OutputDebugString("objAuthServerLdr.Initialize.");

	sssnprintf( szMessString, sizeof(szMessString), "AuthAdminRoles - szSCSCommsPath = %s.", szSCSCommsPath ); 
	OutputDebugString( szMessString );

	symres = objAuthServerLdr.Initialize( szSCSCommsPath );
    if (SYM_FAILED (symres))
	{
		OutputDebugString("AuthAdminRoles - objAuthServerLdr.Initialize failed.");
		goto AuthAdminRoles_Fail;
	}
	else
	{
		IAuthPtr ptrAuth;

		// Set servername for IAUTH
		ServerNameVector.insert(ServerNameVector.begin(),strPrimaryServerName);

		sssnprintf(szMessString, sizeof(szMessString), "ValidateAdminRoles: logging onto '%s'\n", ServerNameVector.front());
		OutputDebugString(szMessString);

        hResult = ptrSecureLogin->CreateAuthAndLogin(ptrAuth.m_p,
														ServerNameVector,
														ServerNameVector.size(),
														ScsSecureComms::IAuth::NAME_AUTH,
														strUsername, 
														strPassword);

		OutputDebugString("AuthAdminRoles - CheckRolesOfLoginCert.");

		sssnprintf( szMessString, sizeof(szMessString), "hResult = ptrSecureLogin->CreateAuthAndLogin =0x%08X.", hResult ); 
		OutputDebugString( szMessString );

		if ( hResult != ScsSecureComms::RTN_OK )
		{  
			bwCredentialsValid = FALSE;
			OutputDebugString("CreateAuthAndLogin - Failed IAuth user credentials.");
		}
		else
		{
			OutputDebugString("CreateAuthAndLogin - IAuth user credentials succeeded.");
			// Check IAUTH rights
			if ( CheckRolesOfLoginCert( ptrAuth, &bwAccess) != S_OK) 
			{  
				bwCredentialsValid = FALSE;
				OutputDebugString("AuthAdminRoles - Failed to Authenticated user credentials.");
			}
			else
			{
				bwCredentialsValid = TRUE;
				OutputDebugString("AuthAdminRoles - Authenticated user credentials. ");
			}
		}
	}

AuthAdminRoles_Fail:
    if (ptrSecureLogin)
    {
        delete ptrSecureLogin;
    }

	// Unload ScsComms.
	if ( UnLoadScsComms() == SYM_OK )
	{
		sssnprintf( szMessString, sizeof(szMessString), "AuthAdminRoles - UnLoadScsComms() on succeeded." ); 
		OutputDebugString( szMessString );
	}
	else
	{
		LPVOID lpMsgBuf = NULL;
		if( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			                NULL,
			                GetLastError(),
			                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			                (LPTSTR) &lpMsgBuf,
			                0,
			                NULL ) != FALSE )
        {
			sssnprintf( szMessString, sizeof(szMessString), "AuthAdminRoles - UnLoadScsComms() on failed: %s", (LPCTSTR)lpMsgBuf ); 
			OutputDebugString( szMessString );
		    LocalFree( lpMsgBuf );
        }
	}

return ( bwCredentialsValid );
}


////////////////////////////////////////////////////////////////////
//  This function was taken directly from 
//	Norton_System_Center\src\NscTop\TopologyContainer.cpp
//	
//	CTopologyContainer::CheckRolesOfLoginCert()
//
//  Purpose:
//		This function will determine if the user has "admin" rights
//		or "readonly" rights to the domain that he/she just logged into.
//
//  Parameters:
//		ScsSecureComms::IAuth *&pAuth [IN] This the the IAuth pointer.
//		BOOL *pFullAccess [OUT] This indicates whether or not the
//		user has admin access to the domain.
//
//  Return values:
//		S_OK    - Function was successful.
//		E_FAIL  - User has NO rights to the domain (i.e. the user is
//				  not an admin or readonly user
//
////////////////////////////////////////////////////////////////////

DWORD CheckRolesOfLoginCert(ScsSecureComms::IAuth *&pAuth, BOOL *pFullAccess)
{
	// assume the worst; that we can't validate the login...
	DWORD dwRet = E_FAIL;
	*pFullAccess = FALSE;
	TCHAR szMessString[MAX_PATH]					= {0};

	try
	{
		DWORD dwCertCount = pAuth->GetCertCount();

		if (dwCertCount > 0)
		{
			ScsSecureComms::ICertVectorPtr certs;
			ScsSecureComms::RESULT res = ScsSecureComms::RTN_OK;

			if ((res = pAuth->GetCertChain(certs.m_p)) == ScsSecureComms::RTN_OK)
				for (DWORD dwCert = 0; (dwCert < dwCertCount) && (dwRet != S_OK); dwCert++)
				{
					ScsSecureComms::ICertPtr cert;
					ScsSecureComms::IRoleVectorPtr roles;

					cert.Attach( certs->at(dwCert) );

					if( (res = cert->GetRoles(roles.m_p)) == ScsSecureComms::RTN_OK)
					{
						for (unsigned int nRole = 0; (nRole < roles->size()) && (*pFullAccess == FALSE); ++nRole)
						{
							if (stricmp(roles->at(nRole), ScsSecureComms::BUILTIN_ROLE_ADMIN) == 0)
							{
								*pFullAccess = TRUE;
								dwRet = S_OK;
							}
							else if (stricmp(roles->at(nRole), ScsSecureComms::BUILTIN_ROLE_READONLY) == 0)
							{
								*pFullAccess = FALSE;
								dwRet = S_OK;
							}
						}
					}
				}
		}
	}
	catch (...)
	{
		OutputDebugString( "CheckRolesOfLoginCert: catch" );
	}
	sssnprintf( szMessString, sizeof(szMessString), "CheckRolesOfLoginCert: dwRet = %d.", dwRet ); 
	OutputDebugString( szMessString );
	return dwRet;
}



// ---------------------------------------------------------------------------
// Routine:	GetSymInstallDir
//
// Purpose:	Gets the Symantec Antivirus Install Directory
//
// Makes
// ---------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
BOOL GetSymInstallDir (TCHAR *szInstallDir, DWORD dwSize)
{
DWORD lResult = 0;
HKEY hKey = NULL;
DWORD regOptions = KEY_READ;

lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\"), NULL, regOptions, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        lResult = SymSaferRegQueryValueEx(hKey, _T("Home Directory"), NULL, NULL, LPBYTE(szInstallDir), &dwSize);
        RegCloseKey(hKey);
}

return (lResult == ERROR_SUCCESS);

}

// ---------------------------------------------------------------------------
// Routine:	GetProgramFilesDir
//
// Purpose:	Gets the system Program Files Directory. Copied from old Kepler code
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
BOOL GetProgramFilesDir( TCHAR* pszPath, UINT cbSize, bool isWin64 )
//Sets pszPath to the ProgramFiles directory value from the registry, setting the default if needed
//Returns TRUE on success, else FALSE
//Ripped from Phoenix (NSW 2) - originally RegGetProgramFilesDir
{
    HKEY hKey = NULL;
    LONG lResult = 0L;
REGSAM regOptions = NULL;

    // get CurrentVersion location
if( isWin64 )
{
 	regOptions = KEY_READ | KEY_WOW64_64KEY;
}
else
{
 	regOptions = KEY_READ;
}
lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, regOptions, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        DWORD dwSize = cbSize;
        lResult = SymSaferRegQueryValueEx(hKey, _T("ProgramFilesDir"), NULL, NULL, LPBYTE(pszPath), &dwSize);
        RegCloseKey(hKey);
    }

    // manufacture if not good to here
    if (lResult != ERROR_SUCCESS)
    {
        // manufacture program files location
        TCHAR szDirName[ MAX_PATH ] = {0};
        GetWindowsDirectory(szDirName, sizeof(szDirName)); //lint !e534
        _tcscpy(szDirName + 3, DEFAULT_PROGRAMFILES);
 	
        // add key by open/create
        if (CreateDirectory(szDirName, NULL))
        {
            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | regOptions, NULL, &hKey, NULL);
            if (lResult == ERROR_SUCCESS)
            {
                lResult = RegSetValueEx(hKey, _T("ProgramFilesDir"), 0L, REG_SZ, LPBYTE(szDirName), (DWORD(_tcslen(szDirName)) + 1UL) * sizeof(TCHAR));
                RegCloseKey(hKey);
            }
        }
    }

    return (lResult == ERROR_SUCCESS);
}

BOOL GetCommonFilesDir( LPTSTR pszPath, UINT cbSize, bool isWin64 )
//Sets pszPath to the CommonFiles directory value from the registry, setting the default if needed
//Returns TRUE on success, else FALSE
//Ripped from Phoenix (NSW 2) - originally RegGetProgramFilesDir
{
HKEY hKey = NULL;
LONG lResult = 0L;
REGSAM regOptions = NULL;

// get existing Common Files location
if( isWin64 )
{
 	regOptions = KEY_READ | KEY_WOW64_64KEY;
}
else
{
 	regOptions = KEY_READ;
}
lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, regOptions, &hKey);
if (lResult == ERROR_SUCCESS)
{
 	DWORD dwSize = cbSize;
 	lResult = SymSaferRegQueryValueEx(hKey, _T("CommonFilesDir"), NULL, NULL, LPBYTE(pszPath), &dwSize);
 	RegCloseKey(hKey);
}

// manufactur if not good to here
if (lResult != ERROR_SUCCESS)
{
 	TCHAR szDirName[ MAX_PATH ] = {0};
 	if (GetProgramFilesDir(szDirName, sizeof(szDirName), isWin64))
 	{
 		// manufacture program files location
 		_tcscat(szDirName, _T("\\") DEFAULT_COMMONFILES);
 		if (CreateDirectory(szDirName, NULL))
 		{
 	        lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGSTR_PATH_SETUP, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | regOptions, NULL, &hKey, NULL);
 			if (lResult == ERROR_SUCCESS)
 			{
 				lResult = RegSetValueEx(hKey, _T("CommonFilesDir"), 0L, REG_SZ, LPBYTE(szDirName), DWORD((_tcslen(szDirName) + 1) * sizeof(TCHAR)));
 				RegCloseKey(hKey);
 			}
 		}
 	}
}

return (lResult == ERROR_SUCCESS);
}
// ---------------------------------------------------------------------------
// Routine:	GetSymantecSharedDir
//
// Purpose:	Gets the Symantec Shared Directory. Copied from old Kepler code
//
//----------------------------------------------------------------------------
// 09/01/04 Michael Lee --- Function created
BOOL GetSymantecSharedDir( LPTSTR pszPath, UINT cbSize, bool isWin64 )
//Sets pszPath to the Symantec Common directory (Common Files\Symantec Shared)
//Returns TRUE on success, else FALSE
//Ripped from Phoenix (NSW 2) - originally RegGetSymantecSharedDir
{
BOOL fResult = FALSE;
if (GetCommonFilesDir(pszPath, cbSize, isWin64))
{
 	_tcscat(pszPath, _T("\\") DEFAULT_SYMANTEC_SHARED);
 	fResult = TRUE;
}
return fResult;
}
 // ---------------------------------------------------------------------------
 // Routine:	OEMSetOff
 //
 // Purpose:	Custom Action for base build to set the OEM status to off if a 
 //	        previous OEM install existed
 //
 //----------------------------------------------------------------------------
 // 09/23/04 Michael Lee --- Function created
 extern "C" __declspec(dllexport) UINT __stdcall OEMSetOff( MSIHANDLE hInstall )
 {
     UINT	nRet = ERROR_SUCCESS;
     TCHAR	lpszInstallDir[1024];
 	TCHAR	lpszLogMessage[1024];
 
 	try
 	{
 		DWORD dwDebug = 0;
 		HKEY hKey = NULL;
 		DWORD dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Symantec\\OEM", NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL);
 		if (dwResult == ERROR_SUCCESS)
 		{
 			DWORD dwSize = sizeof(DWORD);
 			SymSaferRegQueryValueEx(hKey, "Debug", NULL, NULL, LPBYTE(&dwDebug), &dwSize);
 			RegCloseKey(hKey);
 		}
 
 		if (dwDebug)
 			MessageBox( NULL, "OEMSetOff", "SAVOEM", MB_OK );
 
 		bool	bValidPath = true;
 		int     iLen = 0;
 		int		nError = ERROR_SUCCESS;	
 
 		DWORD dwSize= 1024;
 		_tcscpy(lpszInstallDir, "");
 		// MCLEE - 4/13/2005 --- Use the OEM dir off of shared
	  	if (!GetSymantecSharedDir(lpszInstallDir, dwSize, FALSE))
 		{
 			if (dwDebug)
 				MessageBox( NULL, lpszInstallDir, "INSTALLDIR", MB_OK );
 
 			sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "INSTALLDIR not retrievable");
 			MSILogMessage( hInstall, lpszLogMessage, TRUE );
 			throw( (UINT)IDS_ERR_NOPATH );
 		}
		else
		{
			_tcscat(lpszInstallDir, _T("\\OEM\\"));
 		}

 
 		// Get rid of trailing \\ so it won't mess up formatting below.
 		int i = _tcslen(lpszInstallDir) - sizeof(TCHAR);
 		TCHAR *pStr = lpszInstallDir + i;
 		if ((*pStr) == _T('\\'))
 			lpszInstallDir[i] = '\0';
 
 
 		TCHAR lpszOEMDLL[1024];
 		sssnprintf(lpszOEMDLL, sizeof(lpszOEMDLL), "%s\\%s", lpszInstallDir, OEMDLL);
 
 		// Use short path name. MSI likes this better
 		if (dwDebug)
 			MessageBox( NULL, "Convert SPN", "SAVOEM - SPN",  MB_OK );
 
 		TCHAR lpszOEMDLL_SPN[1024];
 		if (GetShortPathName(lpszOEMDLL, lpszOEMDLL_SPN, 1024))
 		{
 			if (dwDebug)
 				MessageBox( NULL, lpszOEMDLL_SPN, "SAVOEM - SPN",  MB_OK );
 			_tcscpy(lpszOEMDLL, lpszOEMDLL_SPN);
 		}
 
 		if (dwDebug)
 			MessageBox( NULL, lpszOEMDLL, "SAVOEM",  MB_OK );
 
 
 		HMODULE hOEM = LoadLibrary(lpszOEMDLL);
 		if (hOEM)
 		{
 			if (dwDebug)
 				MessageBox( NULL, "Loaded SymOEM.dll", "SAVOEM",  MB_OK );
 
 			
 			// ---------------------  Set OEM Install
 			lpfnSetOEM_Install SetOEM_Install = (lpfnSetOEM_Install) GetProcAddress(hOEM, _T("SetOEM_Install"));
 
 			if (dwDebug)
 			{
 				sssnprintf(lpszLogMessage, sizeof(lpszLogMessage), "SetOEM_Install = 0");
 				MessageBox( NULL, lpszLogMessage, "SAVOEM",  MB_OK );
 			}
 
 			if (SetOEM_Install)
 				SetOEM_Install(FALSE);
 
 
 			// Free the Library.
 			FreeLibrary(hOEM);
 	
 		}
 		else 
 		{
 			if (dwDebug)
 			{
 				TCHAR sError [1024];
 				sssnprintf(sError, sizeof(sError), "Error loading SymOEM.dll = %d", GetLastError());
 				MessageBox( NULL, sError, "SAVOEM",  MB_OK );
 			}
 		}
 	
 	}
 	catch( UINT )
 	{
 	}
 	return nRet;
 }

//////////////////////////////////////////////////////////////////////////
//
// Function: DeleteorMoveFile()
//
// Description: This function does the actual file deleting or movefileex
// with MOVEFILE_DELAY_UNTIL_REBOOT bit set.
//
// Return Values: BOOL TRUE if successful.
//
//////////////////////////////////////////////////////////////////////////
// 12/10/2004 - DKOWALYSHYN  
BOOL DeleteorMoveFile ( MSIHANDLE hInstall, TCHAR* pszFileTarget )
{
	BOOL			bSuccess							= FALSE;
	TCHAR			szBuf[MAX_PATH]						= {0};
	// Set file attribs so we can Deletefile or MoveFileEx them
	if ( !SetFileAttributes ( pszFileTarget, FILE_ATTRIBUTE_NORMAL ))
	{
		sssnprintf(szBuf, sizeof(szBuf), "DeleteorMoveFile - SetFileAttributes failed %s", pszFileTarget); 
		MSILogMessage( hInstall, szBuf );
	}
	else
	{
		sssnprintf(szBuf, sizeof(szBuf), "DeleteorMoveFile - SetFileAttributes succeeded %s", pszFileTarget); 
		MSILogMessage( hInstall, szBuf );
	}
	if( !DeleteFile( pszFileTarget ) )
	{
		sssnprintf(szBuf, sizeof(szBuf), "DeleteorMoveFile - DeleteFile failed %s", pszFileTarget); 
		MSILogMessage( hInstall, szBuf );
		MoveFileEx( pszFileTarget, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
		bSuccess = TRUE;
	}
	else
	{
		sssnprintf(szBuf, sizeof(szBuf), "DeleteorMoveFile - DeleteFile succeeded %s", pszFileTarget); 
		MSILogMessage( hInstall, szBuf );
	}
	return (bSuccess);
 }

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiCleanTempFolder()
//
// Description: This function cleans up the temp folder on an installer
// cancel event.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 12/10/2004 - DKOWALYSHYN  
// 10/06/2006 - DKOWALYSHYN - Added code to remove LUSETUP, VDEFHUB & DIR
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiCleanTempFolder( MSIHANDLE hInstall )
{
	TCHAR			szTARGETDIR[MAX_PATH]				= {0};
	TCHAR			strTargetFile[MAX_PATH]				= {0};
	TCHAR			szFile[MAX_PATH]					= {0};
	TCHAR			szTempFolder[MAX_PATH]				= {0};
	TCHAR			szBuf[MAX_PATH]						= {0};
	DWORD			dwLen;
	DWORD			dwTARGETDIRLen						= sizeof(szTARGETDIR);

	// get the Install temp folder
	dwLen = sizeof( szTempFolder );
	if ( 0 < GetTempPath ((sizeof(szTempFolder)), szTempFolder))
	{
		sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - GetTempPath succeeded %s", szTempFolder); 
		MSILogMessage( hInstall, szBuf );
		for ( int i = 0; i < NUM_TEMP_FOLDER_FILES; i++ )
		{
			lstrcpy( szFile, szTempFolder );
			AddSlash( szFile );
			lstrcat( szFile, pszDestFiles[i] );
			DeleteorMoveFile( hInstall, szFile );
		}
	}
	else
	{
		sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder - GetTempPath Failed %s", szTempFolder); 
		MSILogMessage( hInstall, szBuf );
	}
	// get the PROPERTY_VIRUSDEFFOLDER from the msi engine
	if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T(PROPERTY_VIRUSDEFFOLDER), szTARGETDIR, &dwTARGETDIRLen ))
    {
		vpsnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder_P: PROPERTY_VIRUSDEFFOLDER %s := %s", PROPERTY_VIRUSDEFFOLDER, szTARGETDIR);
		MSILogMessage(hInstall, szBuf);
		for ( int i = 0; i < NUM_PSEUDO_TEMP_FOLDER_FILES; i++ )
		{
			lstrcpy( szFile, szTARGETDIR );
			AddSlash( szFile );
			lstrcat( szFile, pszPseudoFiles[i] );
			DeleteorMoveFile( hInstall, szFile );
		}
		vpsnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder_P: Attempting to RemoveDirectory %s", szTARGETDIR);
		MSILogMessage(hInstall, szBuf);
		RemoveDirectory( szTARGETDIR );
	}
	else
	{
		sssnprintf(szBuf, sizeof(szBuf), "MsiCleanTempFolder_P - GetTempPath Failed %s", szTempFolder); 
		MSILogMessage( hInstall, szBuf );
	}
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiGetServerGroupNamefromServername()
//
// Description: This function looks up the Server Group Name from the
// server name when provided by the browse dialog.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 1/18/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiGetServerGroupNamefromServername( MSIHANDLE hInstall )
{
	DWORD		dwLen = MAX_PATH;
	CString		serverName;
	CString		serverGroupName;
	CString		domainName;
	CString		temp;
	UINT		nError;

	try
	{
		// Get the current value of the SERVERGROUPNAME property from the MSI database and
		// set it to TEMPSERVERGROUP
		if( ERROR_SUCCESS != ( nError = MsiGetProperty( hInstall, "SERVERNAME", serverName.GetBuffer(dwLen), &dwLen ) ) )
			throw( "MsiGetProperty SERVERNAME failed" );
		serverName.ReleaseBuffer();
		dwLen = MAX_PATH;
		if( ERROR_SUCCESS != ( nError = MsiGetProperty( hInstall, "SERVERGROUPNAME", serverGroupName.GetBuffer(dwLen), &dwLen ) ) )
			throw( "MsiGetProperty SERVERGROUPNAME failed" );
		serverGroupName.ReleaseBuffer();

		if( ERROR_SUCCESS != ( nError = MsiSetProperty( hInstall, "TEMPSERVERGROUPNAME", serverGroupName ) ) )
		{
			throw( "MsiSetProperty TEMPSERVERGROUPNAME failed" );
		}

		temp.Format( "SERVERNAME:%s SERVERGROUPNAME:%s", serverName, serverGroupName );
		OutputDebugString ( temp );
		CString addressCache = "SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache\\" + serverName;
		OutputDebugString ( addressCache );
		RegKeyGetValue(HKEY_LOCAL_MACHINE, addressCache, _T("Domain"),
							(LPBYTE)domainName.GetBuffer(MBUFFER), MBUFFER, NULL);
		domainName.ReleaseBuffer();
		temp.Format( "Servername:%s Domain:%s", serverName, domainName );
		OutputDebugString ( temp );

		// Put the domain name into the TEMPSERVERNAME property so that the MSI UI can use it
		if( FALSE == domainName.IsEmpty() )
		{
			if( ERROR_SUCCESS != ( nError = MsiSetProperty( hInstall, "TEMPSERVERGROUPNAME", domainName ) ) )
			{
				throw( "MsiSetProperty TEMPSERVERGROUPNAME failed" );
			}
		}
		else
			OutputDebugString( "Empty domain name" );
		SetMSIEditControlText( 1 );
	}
	catch( char *szErr )
	{
		temp.Format( "%s (Error Code: %d)\r\n", szErr, nError );
		OutputDebugString( temp );

		return ERROR_INSTALL_FAILURE;
	}
	return ERROR_SUCCESS;
}
#include "WinSvc.h"
// ---------------------------------------------------------------------------
 // Routine:	StartCCEvtMgr
 //
 // Purpose:	Custom Action to start CCEvtMgr after installation
 //             since it isn't started in SAVWin64
 //
 //----------------------------------------------------------------------------
 // 03/02/05 Michael Lee --- Function created
 extern "C" __declspec(dllexport) UINT __stdcall StartCCEvtMgr( MSIHANDLE hInstall )
 {
 	try
 	{
		SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
 		if (hSCM)
		{
			SC_HANDLE       hService;
			// ccEvtMgr service name won't change so use it as a constant string (rather than put into a resource file).
			hService = OpenService( hSCM, _T("ccEvtMgr"), SERVICE_START);
			if( hService != NULL )
			{
				StartService( hService, 0, NULL );
				CloseServiceHandle( hService );
			}
			CloseServiceHandle( hSCM );
		}
		
 	}
 	catch( UINT )
 	{
 	}
 	return ERROR_SUCCESS;  // Always return ERROR_SUCCESS since this is NOT a catastrophic error;
 }
 
 //////////////////////////////////////////////////////////////////////////
//
// Function: MsiGetNewServerPassword()
//
// Description: This function determines if you have a small / less secure
//	servergroup password and prompts you to enter a more secure one during
//	migration
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 3/15/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiGetNewServerPassword( MSIHANDLE hInstall )
{
	DWORD		dwLen = MAX_PATH;
	HWND		hWnd;
	TCHAR		szConsolePassword[ PASS_MAX_CIPHER_TEXT_BYTES ] = {0};
	TCHAR		szError[ MAX_PATH ] = {0};
	TCHAR		szMessage[ MAX_PATH ] = {0};
	TCHAR		szUILevel[ MAX_PATH ] = {0};
	TCHAR		szTempPassword[ PASS_MAX_CIPHER_TEXT_BYTES ] = {0};
	TCHAR		szOldUnencryptedPassword[PASS_MAX_PLAIN_TEXT_BYTES +1] = {0};	
	CString		strOldPassword;
	CString		strNewPassword;
	CString		strVerPassword; 
	CString		strServerGroupUsername;
	CString		strServerGroupUsernameEncrypted;
	int			uiLevel = 5;

	try
	{
		// - Set MSI Property defaults -
		MsiSetProperty( hInstall, _T("VALIDPASSWORD"), NULL );
		// - CONSOLEPASSWORD -
		dwLen = sizeof( szConsolePassword );
		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("CONSOLEPASSWORD.2D6B2C77_9DB3_4019_A3E4_3F2892186836"), szConsolePassword, &dwLen ) )
			throw( _T("Error getting CONSOLEPASSWORD.2D6B2C77_9DB3_4019_A3E4_3F2892186836 property") );

//		vpsnprintf(szMessage, sizeof (szMessage), "szConsolePassword = %s",szConsolePassword);
//		OutputDebugString (szMessage);

		UnMakeEP(szConsolePassword, sizeof(szConsolePassword),PASS_KEY1,PASS_KEY2,szOldUnencryptedPassword, sizeof(szOldUnencryptedPassword) );

//		vpsnprintf(szMessage, sizeof (szMessage), "szOldUnencryptedPassword = %s",szOldUnencryptedPassword);
//		OutputDebugString (szMessage);

		if ( strlen ( szOldUnencryptedPassword ) < 6 )
		{
			OutputDebugString( "New Pass lenght >6 chars " );

			hWnd = GetForegroundWindow();
			if( !hWnd )
				hWnd = GetDesktopWindow();

			CNewPassDlg newPassDlg(szOldUnencryptedPassword, CWnd::FromHandle(hWnd));

			if( newPassDlg.DoModal() != IDOK)
				throw( _T("User aborted new group creation") );

			strNewPassword = newPassDlg.szNewPassword;
			strVerPassword = newPassDlg.szVerPassword;

			// Encrypt the password
			MakeEP( szTempPassword, sizeof(szTempPassword), PASS_KEY1, PASS_KEY2, strNewPassword, strNewPassword.GetLength()+1 );
			strNewPassword = szTempPassword;

//			vpsnprintf( szMessage, sizeof (szMessage), "NewPass = %s, NewEncrypt %s", strVerPassword, pszTemp );
//			OutputDebugString ( szMessage );

			MsiSetProperty( hInstall, _T("VALIDPASSWORD"), _T("1") );
			MsiSetProperty( hInstall, _T("CONSOLEPASSWORD.2D6B2C77_9DB3_4019_A3E4_3F2892186836"), szTempPassword );
			MsiSetProperty( hInstall, _T("CONSOLEPASSWORD"), szTempPassword );
			MsiSetProperty( hInstall, _T("SERVERGROUPPASS"), strVerPassword );
		}
		else
		{
			OutputDebugString ( "Valid Password" );
			MsiSetProperty( hInstall, _T("VALIDPASSWORD"), _T("1") );
		}
	}
	catch( TCHAR *szError )
	{
		MsiSetProperty( hInstall, _T("VALIDPASSWORD"), NULL );
		MSILogMessage( hInstall, szError, TRUE );
		OutputDebugString ( szError );
	}
	OutputDebugString ( "ZeroMemory szConsolePassword" );
	SecureZeroMemory(szConsolePassword, sizeof(szConsolePassword));
	OutputDebugString ( "ZeroMemory szOldUnencryptedPassword" );
	SecureZeroMemory(szOldUnencryptedPassword, sizeof(szOldUnencryptedPassword));
	strNewPassword.Empty();
	strVerPassword.Empty();

 	return ERROR_SUCCESS;  
 }

