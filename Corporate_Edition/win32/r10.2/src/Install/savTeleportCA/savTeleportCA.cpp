// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SavTeleportCA.h"
#include "SymSaferStrings.h"
#include "accesstoken.h"
#include "wtsloader.h"
#include "wtsapi32.h"
#include "SIDUtils.h"
#include "Windows.h"
#include "vpstrutils.h"

//////////////////////////////////////////////////////////////////////////
//
// Function: GetCurrentState()
//
// Parameters:
//		MSIHANDLE hInstall
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function get the current processid, then gets the SessionID from
//	the processid and then sets a public MSI property SESSIONID with the
//	sessionID for use later in deferred mode (system context) by the 
//	LaunchasUser custom action.
//
//////////////////////////////////////////////////////////////////////////
// 6/05/06 - DKowalyshyn
//////////////////////////////////////////////////////////////////////////
UINT _stdcall GetCurrentState ( MSIHANDLE hInstall )
{
	try
	{
		TCHAR szErrorMessage[MAX_PATH*2]			= {0};	
		TCHAR szCmdLine[MAX_PATH]					= {0};
		TCHAR szSessionID[MAX_PATH]					= {0};
		UINT uSetRet								= 0;
		BOOL bResult								= FALSE;
		DWORD dwCurrentSessionId					= 0;
		DWORD dwpSessionID							= 0;
		DWORD dwSessionID							= 0;
		DWORD pdwSessionID							= 0;
		DWORD dwErr									= ERROR_SUCCESS;

		//::MessageBox(NULL,"Check access rights!","Check access rights!",MB_OK);
		dwpSessionID = CWTSLoader::GetInstance().GetActiveConsoleSessionId();

		sssnprintf( szErrorMessage, sizeof(szErrorMessage), "GetCurrentState: ProcessIdToSessionId SessionID is = %d",dwpSessionID );
		MsiLogMessage(hInstall, szErrorMessage);

		// convert sessionID to a string
		sssnprintf( szSessionID, sizeof(szSessionID), "%d",dwpSessionID );
		
		// Set he MSI property SESSIONID with the SessionID
		uSetRet = MsiSetProperty( hInstall, _T(SESSIONID), szSessionID );

		if ( ERROR_SUCCESS == uSetRet )
		{
			sssnprintf( szErrorMessage, sizeof(szErrorMessage), "GetCurrentState: MsiSetProperty - Success");
			MsiLogMessage(hInstall, szErrorMessage);
		}
		else
		{
			sssnprintf( szErrorMessage, sizeof(szErrorMessage), "GetCurrentState: MsiSetProperty - Failed" );
			MsiLogMessage(hInstall, szErrorMessage);
		}
	}
	catch( ... )
    {
		MsiLogMessage( hInstall, "GetCurrentState: Unhandled exception." );
    }

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: LaunchasUser()
//
// Parameters:
//		MSIHANDLE hInstall
//
// Returns:
//		UINT - ERROR_SUCCESS if function completed
//
// Description:  
//	This function get public MSI property SESSIONID and the path to the
//	executable to be launched (in this case VPTray.exe). 
//	Next we call QueryUserToken with the sessionid to create a user token.
//	After that we call DuplicateTokenEx to create a primary user token and
//	finally we call CreateProcessAsUser to launch the target application
//	as a regular little Abby (non - UAC elevated) user.
//
//////////////////////////////////////////////////////////////////////////
// 6/05/06 - DKowalyshyn
//////////////////////////////////////////////////////////////////////////
UINT _stdcall LaunchasUser ( MSIHANDLE hInstall )
{
	try
	{
		HANDLE hPrimaryToken						= NULL;
		TCHAR *multipath							= NULL;
		TCHAR szErrorMessage[MAX_PATH*2]			= {0};	
		TCHAR szCmdLine[MAX_PATH]					= {0};
		TCHAR szSessionID[MAX_PATH]					= {0};
		TCHAR szAppPath[MAX_PATH]					= {0};
		UINT uErr									= 0;
		DWORD dwLen									= 0;
		DWORD dwpSessionID							= 0;
		BOOL bResult								= FALSE;
		STARTUPINFO         si;
		PROCESS_INFORMATION pi;
		CAccessToken atUserToken(CAccessToken::TOKEN_INVALID);

		// Init the startupinfo structure
		memset (&si, 0, sizeof(si));
		si.cb = sizeof(si);
		memset (&pi, 0, sizeof(pi));

		uErr = MsiGetProperty( hInstall, _T("CustomActionData"), "", &dwLen );

		if( uErr != ERROR_MORE_DATA )
		{
			sssnprintf(szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: MsiGetProperty failed - get size. uErr = %d.", uErr);
			throw( _T(szErrorMessage) );
		}
		else
		{
			sssnprintf(szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: CustomActionData uErr %d.", uErr);
			MsiLogMessage(hInstall, szErrorMessage);
			sssnprintf(szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: CustomActionData dwlen %d.", dwLen);
			MsiLogMessage(hInstall, szErrorMessage);
			// We want to pad the CAD string incase the parsing needs a little more space
			dwLen=dwLen+100;  
			// Allocate more room for parsing
			multipath = new TCHAR [ dwLen ];  
			sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: Allocated custom action data + 100 = %d.", dwLen );
			MsiLogMessage(hInstall, szErrorMessage);

			if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("CustomActionData"), multipath, &dwLen ) )
			{
				TCHAR *pstrTemp = strtok( multipath, _T(",") );  // First parameter...
				if( pstrTemp )
				{
					vpstrncpy( szSessionID, pstrTemp, sizeof(szSessionID) );
				}
				pstrTemp = strtok( NULL, _T(",") );
				if( pstrTemp )
				{
					vpstrncpy( szAppPath, pstrTemp, sizeof(szAppPath) );
				}

				// In case any more parameter (e.g. vptray's "-i" parameter to indicate launched by installer)
				pstrTemp = strtok( NULL, _T(",") );
				if( pstrTemp )
				{
					vpstrnappend( szAppPath, " ", sizeof(szAppPath) );
					vpstrnappend( szAppPath, pstrTemp, sizeof(szAppPath) );
				}
			}
			else
			{
				throw( _T("LaunchasUser: MsiGetProperty failed - Read data") );
			}
		}
		vpstrncpy( szCmdLine, szAppPath, sizeof(szCmdLine) );
		dwpSessionID = _tstoi (szSessionID);
		sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: convert %s to %d",szSessionID , dwpSessionID );
		MsiLogMessage(hInstall, szErrorMessage);
		sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: szCmdLine = %s",szCmdLine );
		MsiLogMessage(hInstall, szErrorMessage);
	
		CWTSLoader& oWTSLoader = CWTSLoader::GetInstance();
		// If success, try to obtain an access token.
		if (dwpSessionID != 0xFFFFFFFF)
		{
			oWTSLoader.QueryUserToken(dwpSessionID, atUserToken);
			if( !::DuplicateTokenEx( atUserToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hPrimaryToken)) 
			{
				sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: Failed DuplicateTokenEx()" );
				::CloseHandle( atUserToken );
				throw( _T(szErrorMessage) );
			}
			::CloseHandle( atUserToken );

			if( hPrimaryToken == INVALID_HANDLE_VALUE)
			{
				sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: hPrimaryToken is INVALID_HANDLE_VALUE" );
				::CloseHandle( hPrimaryToken );
				throw( _T(szErrorMessage) );
			}

			sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: hPrimaryToken: 0x%08X(%d)", hPrimaryToken, hPrimaryToken);
			MsiLogMessage(hInstall, szErrorMessage);

			if (hPrimaryToken)
			{
				sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: hPrimaryToken exists" );
				MsiLogMessage(hInstall, szErrorMessage);

				bResult = CreateProcessAsUser(
												hPrimaryToken,		// Client's access token
												NULL,				// File to execute
												szCmdLine,			// Command line
												NULL,				// Pointer to process SECURITY_ATTRIBUTES
												NULL,				// Pointer to thread SECURITY_ATTRIBUTES
												FALSE,				// Handles are not inheritable
												0,					// Creation flags
												NULL,				// Pointer to new environment block
												NULL,				// Name of current directory 
												&si,				// Pointer to STARTUPINFO structure
												&pi					// Receives information about new process
												);
				if (bResult)
				{
					sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: CreateProcessAsUser Success" );
					MsiLogMessage(hInstall, szErrorMessage);
					CloseHandle( pi.hThread );
					CloseHandle( pi.hProcess );
				}
				else
				{
					DWORD dwerr = GetLastError(); 
					sssnprintf(szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: CreateProcessAsUser Failed, error = %d", dwerr);
					MsiLogMessage(hInstall, szErrorMessage);
				}
			}
			else
			{
				sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: atUserToken not created");
				MsiLogMessage(hInstall, szErrorMessage);
			}
			::CloseHandle( hPrimaryToken );
		}
		else
		{
			sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: MsiGetProperty SessionID failed" );
			MsiLogMessage(hInstall, szErrorMessage);
		}

		if (multipath != NULL)
			delete[] multipath;
	}
	catch( TCHAR *szErrorMessage )
	{
		MsiLogMessage(hInstall, szErrorMessage);
	}
	catch (std::bad_alloc &) 
	{
		TCHAR szErrorMessage[MAX_PATH*2]			= {0};	
		sssnprintf( szErrorMessage, sizeof(szErrorMessage), "LaunchasUser: catch (std::bad_alloc)" );
		MsiLogMessage(hInstall, szErrorMessage);
	}
	catch( ... )
    {
		MsiLogMessage( hInstall, "LaunchasUser: Unhandled exception." );
    }

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//	
// Function: MsiLogMessage
//
// Description: MsiLogMessage
//
///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
UINT MsiLogMessage( MSIHANDLE hInstall, LPCTSTR szString )
{
	UINT uiRetVal = ERROR_SUCCESS;

	MSIHANDLE hRec = MsiCreateRecord(1);
	if (hRec)
	{
		TCHAR szLogLine[MAX_PATH*5]; // * buffer overrun fix

		lstrcpy(szLogLine, "SAVTeleportCA: ");
		lstrcat(szLogLine, szString);
//		lstrcat(szLogLine, "\r\n");

		// Use OutputDebugString...
		OutputDebugString(szLogLine);

		// ...as well as the log file.
		MsiRecordClearData(hRec);
		MsiRecordSetString(hRec, 0, szLogLine);
		MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRec);
		MsiCloseHandle(hRec);
	}

	return uiRetVal;
}

