// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////
//
// SAVSecureCA - MSI Custom Action DLL
// Description: To provide custom functionality to allow the installer
//				integration of a PKI secure channel communications system.
//
//	Much of the code is borrowed from ScsCommsAppTest.cpp
///////////////////////////////////////////////////////////////////////////
// 6/24/03 DKowalyshyn
///////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#pragma warning (disable: 4786) // identifier was truncated to '255' characters in the debug information

#define INITIIDS

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <TCHAR.H> 
#include <winsock2.h>
#include <iostream>
#include <process.h>
#include <crtdbg.h>
#include <time.h>
#include <vector>
#include <string>

#include "SAVSecureCA.h"
#include "SymInterface.h"
#include "SymInterfaceLoader.h"
#include "IUser.h"
#include "IAuth.h"
#include "IByteStreamer.h"
#include "IMessageManager.h"
#include "ICertSigningRequest.h"
#include "IByteStreamer.h"
#include "ICertIssuer.h"
#include "ClientReg.h"
#include "vpcommon.h"
#include "CertIssueDefaults.h"
#include "ScsCommsUtils.h"
#include "ParseCAD.h"
#include "SCSCommsDefs.h"
#include "RoleVector.h"
#include "CertVector.h"
#include "SymSaferRegistry.h"

#include "resource.h"

#define DONTEXTERNTRANSMAN
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

#include "TransmanRelatedFuncs.h"
#include "commisc.cpp"

using namespace ScsSecureComms;
using namespace std;

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))

#define GET_SERVER_GROUP_GUID_BYTE_SIZE		33

// Globals
static const char* g_rolesServer[] =
{
	BUILTIN_ROLE_SERVER
};

HINSTANCE g_hinstDLL;

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID )
{
	switch (fdwReason)
	{
		case  DLL_PROCESS_ATTACH:
		{
			g_hinstDLL = (HMODULE) hinstDLL;
			break;
		}

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

		lstrcpy(szLogLine, "SAVSecureCA: ");
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

///////////////////////////////////////////////////////////////////////////
//	
// Function: MSICopyRootCert
//
// Description: Copy Root Certificate
//
///////////////////////////////////////////////////////////////////////////
// 6/24/03 DKowalyshyn
///////////////////////////////////////////////////////////////////////////
UINT _stdcall MSICopyRootCert( MSIHANDLE hInstall)
{
	TCHAR			errorMessage[MAX_PATH*2]			= {""};	// MAX_PATH*2 because the log + path can be larger then MAX_PATH
	TCHAR			filePattern[MAX_PATH+1]				= {""}; 
	TCHAR			fileName[MAX_PATH+1]				= {""};
	TCHAR			strArgSrc[MAX_PATH+1]				= {""};
	TCHAR			strArgDest[MAX_PATH+1]				= {""};
	TCHAR			strRootCertFolder[MAX_PATH+1]		= {""};
	DWORD			dwLen								= 0;

	// Get the size of the CustomActionData data
	if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &dwLen ) )
	{
		sssnprintf(errorMessage, sizeof(errorMessage), "MSICopyRootCert: MsiGetProperty failed");
		MsiLogMessage(hInstall, errorMessage);
	}
	else
	{
        TCHAR			*multipath							= NULL;
  		TCHAR			*pdeststr                           = NULL;
  		INT				commalocation                       = 0;

		sssnprintf(errorMessage, sizeof(errorMessage), "MSICopyRootCert: CustomActionData dwlen %d.", dwLen);
		MsiLogMessage(hInstall, errorMessage);
		try
		{
			multipath = new TCHAR [ dwLen++ ]; 

			if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("CustomActionData"), multipath, &dwLen ) )
			{
				sssnprintf(errorMessage, sizeof(errorMessage), "MSICopyRootCert: CustomActionData %s", multipath);
				MsiLogMessage(hInstall, errorMessage);			

   				pdeststr = strstr( multipath, "," );				// location the comma delimiter
   				commalocation = pdeststr - multipath ;				// calc the comma location in the string
  				_tcsncpy(strArgSrc ,multipath, commalocation);		// parse the first piece of data 
  				_tcscpy(strArgDest ,multipath + commalocation + 1);	// pass the second piece of data

				sssnprintf(errorMessage, sizeof(errorMessage), "MSICopyRootCert: strArgSrc %s.", strArgSrc);
				MsiLogMessage(hInstall, errorMessage);
				sssnprintf(errorMessage, sizeof(errorMessage), "MSICopyRootCert: strArgDest %s.", strArgDest);
				MsiLogMessage(hInstall, errorMessage);
				sssnprintf(errorMessage, sizeof(errorMessage), "MSICopyRootCert: g_hinstDLL %d.", g_hinstDLL);
				MsiLogMessage(hInstall, errorMessage);

				// We have our Sourcedir now build the dest target and launch the PKI copy
				_tcscpy(strRootCertFolder, strArgSrc);
				_tcscat(strRootCertFolder, SYM_SCSCOMM_DIR_CER_ROOT);
				int retcode = CopyDirRecursive ( hInstall, strRootCertFolder, strArgDest );
				sssnprintf( errorMessage, sizeof(errorMessage), "MSICopyRootCert: CopyDirRecursive from %s to %s retcode %d", strRootCertFolder, strArgDest, retcode );
				MsiLogMessage( hInstall, errorMessage );
			}
			else
			{
				MsiLogMessage(hInstall, "MSICopyRootCert - MsiGetProperty Could not get CustomActionData.");
			}
		}
		catch (std::bad_alloc &) {}
		if (multipath != NULL)
			delete[] multipath;
	}
	// Don't fail the install just because we failed this funtion.
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

	sssnprintf(errorMessage, sizeof(errorMessage), "Target file is %s.\n", szFile);
	MsiLogMessage(hInstall, errorMessage);
	hFind = FindFirstFile(szFile, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		sssnprintf(errorMessage, sizeof(errorMessage), "Invalid File Handle. Get Last Error reports %d\n", GetLastError ());
		MsiLogMessage(hInstall, errorMessage);
		return false;
	} 
	else 
	{
		sssnprintf(errorMessage, sizeof(errorMessage), "The first file found is %s\n", FindFileData.cFileName);
		MsiLogMessage(hInstall, errorMessage);
		FindClose(hFind);
		_tcscpy (strPath, FindFileData.cFileName);
	}
	return true;
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
	//Sathish Subramanian 1/31/06. Defect 1-5CNNKK - Buffer overrun in MSIExec.exe when install SCS from long pathname. 
	//We have it 4 times the MAX_PATH to accomodate source, destination and extra comments in a single error string.
	TCHAR				szErrorString[MAX_PATH * 4]		={0};	
	int					nRet						= 0;
	BOOL				bRet						= TRUE;
	WIN32_FIND_DATA		hFindData;
	TCHAR				strDFile[ MAX_PATH ]		={0};
	TCHAR				strSFile[ MAX_PATH ]		={0};
	TCHAR				strNextDirDest[ MAX_PATH ]	={0};
	TCHAR				strNextDirSrc[ MAX_PATH ]	={0};
	TCHAR				strAllFiles[ MAX_PATH ]		={0};

	_tcscpy (strAllFiles, strSource);
	_tcscat (strAllFiles, _T("\\*.*")); // Wildcard copy all files

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
			sssnprintf( szErrorString, sizeof(szErrorString), "CopyFile from %s to %s retcode %d", strSFile, strDFile );
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
// Function: CreateServerPKI()
//
// Parameters:
//		MSIHANDLE hInstall
//
// Returns:
//		Error_Success	
//
// Description:  
//	Create the Standalone secondary server PKI objects
//		ServerCertFile, ServerKeyFile, SGRootCertFile
//
//////////////////////////////////////////////////////////////////////////
//	8/23/04 - Dan Kowlayshyn 
//////////////////////////////////////////////////////////////////////////
UINT CreateServerPKI ( MSIHANDLE hInstall,
                       bool login,
                       bool use_root_auth,
                       HMODULE scscomms,
	                   TCHAR* strPkiBasePath,
                       TCHAR* strPrimaryServerName,
                       TCHAR* strUserName,
                       TCHAR* strPassword )
{
	TCHAR	szName [MAX_PATH]							={0};
	TCHAR	szFQDN [MAX_PATH]							={0};
	TCHAR	szServerGroupGuid [MAX_PATH]				={0};
	TCHAR	szMessString [MAX_PATH*4]					={0};
	TCHAR	strTemp[MAX_PATH*4]							={0};
	DWORD	nError										=S_OK;

	TCHAR	szCertPath [MAX_PATH]						={0};
	TCHAR	szKeyPath [MAX_PATH]						={0};
    TCHAR	szRootCertPath [MAX_PATH]					={0};
	TCHAR	szCSRPath [MAX_PATH]						={0};

	TCHAR	szCertName [MAX_PATH]						={0};
	TCHAR	szKeyName [MAX_PATH]						={0};
    TCHAR	szRootCertName [MAX_PATH]					={0};
	TCHAR	szCSRName [MAX_PATH]						={0};

    // Build base paths.

    _tcsncpy( szCertPath, strPkiBasePath, array_sizeof(szCertPath) );
    AddSlash( szCertPath );
    _tcsncat( szCertPath, SYM_SCSCOMM_DIR_CER_CERTS, array_sizeof(szCertPath) - _tcslen(szCertPath) );
    szCertPath[ array_sizeof(szCertPath) - 1 ] = 0;

    _tcsncpy( szKeyPath, strPkiBasePath, array_sizeof(szKeyPath) );
    AddSlash( szKeyPath );
    _tcsncat( szKeyPath, SYM_SCSCOMM_DIR_CER_PKEY, array_sizeof(szKeyPath) - _tcslen(szKeyPath) );
    szKeyPath[ array_sizeof(szKeyPath) - 1 ] = 0;

    _tcsncpy( szRootCertPath, strPkiBasePath, array_sizeof(szRootCertPath) );
    AddSlash( szRootCertPath );
    _tcsncat( szRootCertPath, SYM_SCSCOMM_DIR_CER_ROOT, array_sizeof(szRootCertPath) - _tcslen(szRootCertPath) );
    szRootCertPath[ array_sizeof(szRootCertPath) - 1 ] = 0;

    _tcsncpy( szCSRPath, strPkiBasePath, array_sizeof(szCSRPath) );
    AddSlash( szCSRPath );
    _tcsncat( szCSRPath, SYM_SCSCOMM_DIR_CER_CSR, array_sizeof(szCSRPath) - _tcslen(szCSRPath) );
    szCSRPath[ array_sizeof(szCSRPath) - 1 ] = 0;


	try
	{
		CAuthServerLoader			objAuthServerLdr;
		CMsgMgrLoader				objMsgMgrLdr;
		CCertLoader					objCertLdr;
		CCertSigningRequestLoader	objCertSigningRequestLdr;
		CCertIssuerLoader			objCertIssuerLdr;

		SYMRESULT					symres = SYM_OK;
        RESULT                      nCommsRes = RTN_OK;

		// Initialize the path for all of our loader objects.
        symres = objCertLdr.Initialize( scscomms );
		if (SYM_FAILED (symres))
		{
			nError=symres;
			throw( _T("ERROR: objCertLdr.Initialize() failed:") );
		}
		symres = objAuthServerLdr.Initialize( scscomms );
		if (SYM_FAILED (symres))
		{
			nError=symres;
			throw( _T("ERROR: objAuthServerLdr.Initialize() failed:") );
		}
		symres = objCertSigningRequestLdr.Initialize( scscomms );
		if (SYM_FAILED (symres))
		{
			nError=symres;
			throw( _T("ERROR: objCertSigningRequestLdr.Initialize() failed:") );
		}
		symres = objCertIssuerLdr.Initialize( scscomms );
		if (SYM_FAILED (symres))
		{
			nError=symres;
			throw( _T("ERROR: objCertIssuerLdr.Initialize() failed:") );
		}

		ICertSigningRequestPtr  ptrCertSigningRequest;
		IKeyPtr                 privateKey;
		ICertIssuerPtr			ptrCertIssuer;
        ICertPtr                ptrCert;
        ICertVectorPtr          ptrCerts;

		// Get a CSR object.

		symres = objCertSigningRequestLdr.CreateObject( &ptrCertSigningRequest );
		if (SYM_FAILED (symres))
		{
			nError=symres;
			throw( _T("ERROR: CreateObject() for ICertSigningRequestPtr failed:") );
		}

		// Create a cert issuer..

		symres = objCertIssuerLdr.CreateObject( &ptrCertIssuer );
		if (SYM_FAILED (symres))
		{
			nError=symres;
			throw( _T("ERROR: CreateObject() for ICertIssuerPtr failed:") );
		}

        int ret = 0;

        // Get the Server's name and FQDN -- we need thois for both Server and Login CA certs.

		if (FALSE == Find_FQDN ( hInstall, szName, szFQDN))
		{
			throw( _T("ERROR: Find_FQDN() failed:") );
		}

		if( login == false )
		{
			// Issue a server cert.

			// Server key pair.

			nCommsRes = ptrCertSigningRequest->CreateKeyPair( SERVER_CERT_KEY_LEN, privateKey.m_p );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: CreateKeyPair() on server csr failed:") );
			}

            // Create the CSR

			nCommsRes = ptrCertSigningRequest->Create( szName,
								                       NULL,
								                       ScsSecureComms::BUILTIN_CERTTYPE_SERVER, 
								                       szFQDN );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: Create() on server csr failed:") );
			}

			// Set issuer params.
			nCommsRes = ptrCertIssuer->SetValidityPeriod( SERVER_CERT_DEFAULT_LIFETIME );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: SetValidityPeriod() on server cert failed") );
			}

			nCommsRes = ptrCertIssuer->SetValidityPreDatePeriod( SERVER_CERT_DEFAULT_PREDATE );

			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: SetValidityPreDatePeriod() on server cert failed") );
			}

			nCommsRes = ptrCertIssuer->SetRoles( g_rolesServer, array_sizeof(g_rolesServer) );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: SetRoles() on server cert failed:") );
			}

			nCommsRes = ptrCertIssuer->SetBasicConstraints( false, 0 );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: SetBasicConstraints() on server cert failed:") );
			}

            _tcsncpy( szCertName, SYM_SCSCOMM_SERVER_CERT,  array_sizeof(szCertName) );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;
            _tcsncat( szCertName, SYM_SCSCOMM_CER_EXT,      array_sizeof(szCertName) - _tcslen(szCertName) - 1 );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;

            _tcsncpy( szKeyName, SYM_SCSCOMM_SERVER_CERT,   array_sizeof(szKeyName) );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;
            _tcsncat( szKeyName, SYM_SCSCOMM_PRIV_KEY_EXT,  array_sizeof(szKeyName) - _tcslen(szKeyName) - 1 );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;

            _tcsncpy( szCSRName, SYM_SCSCOMM_SERVER_CERT,   array_sizeof(szCSRName) );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
            _tcsncat( szCSRName, SYM_SCSCOMM_CERT_SIGNING_REQUEST_EXT, array_sizeof(szCSRName) - _tcslen(szCSRName) - 1 );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
		}
		else
		{
            // Issue a Login CA.

			// Login CA key pair.

			nCommsRes = ptrCertSigningRequest->CreateKeyPair( LOGIN_CA_CERT_KEY_LEN, privateKey.m_p );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: CreateKeyPair() on login ca csr failed:") );
			}

            // Create the CSR

			nCommsRes = ptrCertSigningRequest->Create( szName,
								                       NULL,
								                       ScsSecureComms::BUILTIN_CERTTYPE_LOGINCA, 
								                       NULL );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: Create() on login ca csr failed:") );
			}

			// Set issuer params.
			nCommsRes = ptrCertIssuer->SetValidityPeriod( LOGIN_CA_CERT_DEFAULT_LIFETIME );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: SetValidityPeriod() on login ca cert failed") );
			}

			nCommsRes = ptrCertIssuer->SetValidityPreDatePeriod( LOGIN_CA_CERT_DEFAULT_PREDATE );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: SetValidityPreDatePeriod() on login ca cert failed") );
			}

			nCommsRes = ptrCertIssuer->SetBasicConstraints( true, LOGIN_CA_CERT_DEFAULT_PATH_LEN );
			if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: SetBasicConstraints() on login ca certfailed:") );
			}

            _tcsncpy( szCertName, SYM_SCSCOMM_LOGINCA_CERT, array_sizeof(szCertName) );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;
            _tcsncat( szCertName, SYM_SCSCOMM_CER_EXT,      array_sizeof(szCertName) - _tcslen(szCertName) - 1 );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;

            _tcsncpy( szKeyName, SYM_SCSCOMM_LOGINCA_CERT,  array_sizeof(szKeyName) );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;
            _tcsncat( szKeyName, SYM_SCSCOMM_PRIV_KEY_EXT,  array_sizeof(szKeyName) - _tcslen(szKeyName) - 1 );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;

            _tcsncpy( szCSRName, SYM_SCSCOMM_LOGINCA_CERT,  array_sizeof(szCSRName) );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
            _tcsncat( szCSRName, SYM_SCSCOMM_CERT_SIGNING_REQUEST_EXT, array_sizeof(szCSRName) - _tcslen(szCSRName) - 1 );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
		}

        _tcsncpy( szRootCertName, SYM_SCSCOMM_SERVER_GROUP_CA_CERT, array_sizeof(szRootCertName) );
        szRootCertName[ array_sizeof(szRootCertName) - 1 ] = 0;
        _tcsncat( szRootCertName, SYM_SCSCOMM_CER_EXT, array_sizeof(szRootCertName) - _tcslen(szRootCertName) - 1 );
        szRootCertName[ array_sizeof(szRootCertName) - 1 ] = 0;

		CreatePKITargetPaths( hInstall,
                              strPrimaryServerName,
			                  szCertPath, 
			                  szKeyPath, 
			                  szRootCertPath, 
			                  szCSRPath, 
			                  szName, 
			                  szServerGroupGuid, 
			                  szCertName, 
			                  szKeyName, 
			                  szRootCertName, 
			                  szCSRName );

		sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI- cert path: %s", szCertPath );
		MsiLogMessage( hInstall, szMessString );

		sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI- key path: %s", szKeyPath );
		MsiLogMessage( hInstall, szMessString );

		sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI- root cert path: %s", szRootCertPath );
		MsiLogMessage( hInstall, szMessString );

		sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI- csr path: %s", szCSRPath );
		MsiLogMessage( hInstall, szMessString );

        // Send a request via Transman to the Primary Server to issue us a Server cert and Login CA cert.
        // If we don't have a Primary Server name to use, then we are installing as a stand-alone
        // Server, and we should creates self-signed keys.
		
		if( strPrimaryServerName == NULL || _tcslen( strPrimaryServerName ) == 0 )
        {
		    nCommsRes = ptrCertIssuer->IssueCertSelfSigned( 0, ptrCertSigningRequest, privateKey, ptrCert.m_p );
		    if (SYM_FAILED (nCommsRes))
		    {
			    nError=nCommsRes;
			    throw( _T("ERROR: IssueCertSelfSigned() failed:") );
		    }

            // Match the cert and the key.

		    nCommsRes = ptrCert->SetPrivateKey( privateKey );
		    if (SYM_FAILED(nCommsRes))
		    {
			    nError=nCommsRes;
			    throw( _T("ERROR: SetPrivateKey() after self-signed cert generation failed:") );
		    }

            // Save the CSR (and Admin might need it for reissuing from an Enterprise root cert).

			nCommsRes = ptrCertSigningRequest->Save( szCSRPath );
            if (SYM_FAILED (nCommsRes))
			{
				nError=nCommsRes;
				throw( _T("ERROR: CSR save failed:") );
			}
        }
        else
        {
            IAuthPtr ptrAuth;

            // Get an IAuth object to represent the crenedtials we need for the remote connection.

		    symres = objAuthServerLdr.CreateObject( &ptrAuth );
		    if (SYM_FAILED (symres))
		    {
			    nError=symres;
			    throw( _T("ERROR: CreateObject() for IAuthPtr failed:") );
		    }

            if( use_root_auth == true )
            {
                ptrAuth->SetAuthTypeRemote( IAuth::ROOT_AUTH );
            }
            else
            {
                ptrAuth->SetAuthTypeRemote( IAuth::NAME_AUTH );
            }

            DWORD cc = ERROR_GENERAL;

            if( SendCOM_ISSUE_CERT_FROM_CSR != NULL )
            {
                cc = SendCOM_ISSUE_CERT_FROM_CSR( ptrAuth,
                                                  strPrimaryServerName,
                                                  SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_SECURE_COMM,
                                                  strUserName,
                                                  strPassword,
                                                  (login == false ? IAuth::SERVER_EE_CERT_TYPE : IAuth::LOGIN_CA_CERT_TYPE),
                                                  ptrCertSigningRequest,
                                                  ptrCerts.m_p );
            }
            else
            {
			    throw( _T("ERROR: SendCOM_ISSUE_CERT_FROM_CSR function pointer invalid.") );
            }

            if( cc != ERROR_SUCCESS )
            {
			    nError=cc;
			    throw( _T("ERROR: SendCOM_ISSUE_CERT_FROM_CSR failed:") );
            }

            if( ptrCerts->size() == 0 )
            {
			    nError=0;
			    throw( _T("ERROR: SendCOM_ISSUE_CERT_FROM_CSR failed -- no certs") );
            }
            else
            {
		        sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI- Cert submission suceeded: %lu in the chain", ptrCerts->size() );
		        MsiLogMessage( hInstall, szMessString );
            }

            // Match the cert and the key.

            ptrCert.Attach( ptrCerts->at(0) );

		    nCommsRes = ptrCert->SetPrivateKey( privateKey );
		    if (SYM_FAILED(nCommsRes))
		    {
			    nError=nCommsRes;
			    throw( _T("ERROR: SetPrivateKey() after cert generation failed:") );
		    }

            // Save the root, if we got one.

            if( ptrCerts->size() > 1 )
            {
                ICertPtr ptrRootCert;

                ptrRootCert.Attach( ptrCerts->at(1) );

                nCommsRes = ptrRootCert->Save( szRootCertPath );
                if (SYM_FAILED (nCommsRes))
		        {
                    // Non fatal error.
		            sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI- ERROR: Root cert save failed (Error #%d)", nError );
		            MsiLogMessage( hInstall, szMessString );
		        }
            }
            else
            {
		        MsiLogMessage( hInstall, "CreateServerPKI- No root cert to save." );
            }
        }

        // Save the cert.

        nCommsRes = ptrCert->Save( szCertPath );
        if (SYM_FAILED (nCommsRes))
		{
			nError=nCommsRes;
			throw( _T("ERROR: Cert save failed:") );
		}

        // Save the private key.

        nCommsRes = privateKey->Save( szKeyPath );
        if (SYM_FAILED (nCommsRes))
		{
			nError=nCommsRes;
			throw( _T("ERROR: Private key save failed:") );
		}
	}
	catch( TCHAR *szErr )
	{
		sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI- %s (Error #%d)", szErr, nError );
		MsiLogMessage( hInstall, szMessString );
	}
	
	return nError;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: DeleteServerPKI()
//
// Parameters:
//		MSIHANDLE hInstall
//
// Returns:
//		Error_Success	
//
// Description:  
//	Delete the PKI folder structure, since MSI did not create these files
//		it does not know to delete them
//
//////////////////////////////////////////////////////////////////////////
//	02/14/05 - Sean Kennedy
//////////////////////////////////////////////////////////////////////////
UINT _stdcall DeleteServerPKI ( MSIHANDLE hInstall )
{
	DWORD size = 0;
	TCHAR *basepath = NULL;

	try
	{
		// Get our INSTALLDIR
		if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &size ) )
			throw( _T("MsiGetProperty returned unexepected value when determining CustomActionData size") );

	    basepath = new TCHAR [ size + 1 ];
		if( NULL == basepath )
			throw( _T("Unable to allocate memory to hold CustomActionData") );

		if( ERROR_SUCCESS != MsiGetProperty( hInstall, _T("CustomActionData"), basepath, &size ) )
			throw( _T("Unable to obtain CustomActionData") );

		// Delete the INSTALLDIR\pki folder structure
		tstring pkipath( basepath );
		pkipath.append( "pki" );
		MSIDeleteDir( hInstall, pkipath.c_str(), _T("*.*") );
	}
	catch( TCHAR *err )
	{
		MsiLogMessage( hInstall, err );
	}

	if( basepath )
		delete [] basepath;

	return ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Function: Find_FQDN
//
//  Create and returns the hostname and fully qualified domain name (FQDN).
//
//  08/21/2004 Daniel Kowalyshyn - Function created.
//
///////////////////////////////////////////////////////////////////////////////
BOOL Find_FQDN (MSIHANDLE hInstall, TCHAR* pszName, TCHAR* pszFQDN)
{
	WORD				wVersionRequested;
	WSADATA				wsaData;
	TCHAR				szMessString [MAX_PATH]		={0};
	TCHAR				szName [MAX_PATH]			={0};
	TCHAR				szFQDN [MAX_PATH]			={0};
	PHOSTENT			hostinfo;
	INT					rtn=TRUE;
	wVersionRequested = MAKEWORD( 2, 0 );

	sssnprintf( szMessString, sizeof(szMessString), "before:Find_FQDN  pszName:%s pszFQDNHost:%s szName:%s szFQDN:%s",pszName, pszFQDN, szName, szFQDN);
	MsiLogMessage( hInstall, szMessString );

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		DWORD dwLength = sizeof(szName)/sizeof(szName[0])-1;
		//
		//	Fix 1-3L0TTP: When computer DNS name and NetBIOS name are different,
		//		GRC files are not generated.SSC or client installer can not discover
		//		a newly installed server.
		//		NetBIOS name is at most 15 c long. DNS name can be longer. Currently ScsComms expects
		//		NetBIOS name for naming the certificates and do NAME_AUTH
		//		
		//
		if( GetComputerName(szName,&dwLength) == TRUE )
		{
			if((hostinfo = gethostbyname(szName)) != NULL )
			{
				_tcscpy( szFQDN, hostinfo->h_name );
				sssnprintf( szMessString,sizeof(szMessString),"Host: %s Find_FQDN: %s ",szName, szFQDN);
				MsiLogMessage( hInstall, szMessString );
			}
			else
			{
				rtn=FALSE;
				sssnprintf( szMessString,sizeof(szMessString),"Find_FQDN failed gethostbyname");
				MsiLogMessage( hInstall, szMessString );
			}
		}
		else
		{
			rtn=FALSE;
			sssnprintf( szMessString,sizeof(szMessString),"Find_FQDN failed gethostname");
			MsiLogMessage( hInstall, szMessString );
		}
		WSACleanup( );
	} 
	else
	{
		rtn=FALSE;
		sssnprintf( szMessString,sizeof(szMessString),"Find_FQDN failed gethostname");
		MsiLogMessage( hInstall, szMessString );
	}

	// Make sure we were passed buffers
	if( pszName )
	{
		strcpy( pszName, szName ); 
		char* l = strchr( pszName, '.' ); 
		if( l )
			*l = '\0';
	}
	if( pszFQDN )
	{
		strcpy( pszFQDN, szFQDN );
	}

	sssnprintf( szMessString,sizeof(szMessString),"after:Find_FQDN  pszName:%s pszFQDNHost:%s szName:%s szFQDN:%s",pszName, pszFQDN, szName, szFQDN);
	MsiLogMessage( hInstall, szMessString );

	return rtn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function: AddSlash
//
//  Simply adds a slash to the end of a path, if needed.
//
//  08/16/2003 Thomas Brock - Function created.
//  09/18/2003 Thomas Brock - Added CString version of AddSlash.
//
///////////////////////////////////////////////////////////////////////////////

void __fastcall AddSlash( TCHAR* pszPath )
{
	if( pszPath[ _tcslen( pszPath ) - 1 ] != '\\' )
	{
		_tcscat( pszPath, "\\" );
	}
}


///////////////////////////////////////////////////////////////////////////
//  Purpose:
//     Pull the Server Group GUID out of the registry.
//
//  Parameters:
//    szGUID    [out] The server group guid
//    nNumBytes [in]  Number of bytes available in szDir.
//
//  Return Values:
//    ERROR_SUCCESS; ERROR_REG_FAIL
///////////////////////////////////////////////////////////////////////////
static DWORD GetServerGroupGuid( TCHAR *szGuid, int nNumBytes )
{
    HKEY  hKey = NULL;
    DWORD nRtn = ERROR_REG_FAIL;

    SAVASSERT (szGuid);
    SAVASSERT (nNumBytes > 0);

    // Initialize our [out] params.
    szGuid[0] = '\0';

    // Decrement the nNumBytes byte one to ensure we always have room for the
    // terminating '\0' character.
    --nNumBytes;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), 0, KEY_READ, &hKey))
    {
        unsigned char buf[64];
        DWORD         size = sizeof (buf);

        // if we can retrieve the path value then we are good to go
		if (SymSaferRegQueryValueEx(hKey,_T(szReg_Val_DomainGUID),0,NULL,buf,&size) == ERROR_SUCCESS)
        {
            if (size > 0)
            {
                // Covert the byte array to a string of hex characters.
                for (int xx = 0; static_cast<DWORD>(xx) < size && nNumBytes > 0; ++xx)
                {
                    unsigned char hi         = (unsigned char)((buf[xx] & 0xf0) >> 4);
                    unsigned char lo         = (unsigned char) (buf[xx] & 0x0f);
                    unsigned char nibbles[2] = { hi, lo };

                    for (int cnt = 0; cnt < 2; ++cnt)
                    {
                        if (nNumBytes > 0)
                        {
                            if (nibbles[cnt] < 10)
                                *szGuid = (char)(nibbles[cnt] + '0');
                            else
                                *szGuid = (char)((nibbles[cnt] - 10) + 'a');
                            ++szGuid;

                            --nNumBytes;
                        }
                    }
                }
                *szGuid = _T('\0');
                nRtn = ERROR_SUCCESS;
            }
        }

        RegCloseKey (hKey);
    }

	return nRtn;
}


//////////////////////////////////////////////////////////////////////////
//
// Function: MSIServerPKIStandAlone()
//
// Parameters:
//		MSIHANDLE hInstall
//
// Returns:
//		Error_Success	
//
// Description:  
//	Create the Standalone secondary server PKI objects
//		ServerCertFile, ServerKeyFile, SGRootCertFile
//
//////////////////////////////////////////////////////////////////////////
//	8/23/04 - Dan Kowlayshyn 
//////////////////////////////////////////////////////////////////////////
UINT _stdcall	MSIServerPKIStandAlone	( MSIHANDLE hInstall )
{
	DWORD	nRtrnCode									= ERROR_SUCCESS;
	try
    {
	    TCHAR	szMessString [MAX_PATH*4]					= {0};
        TCHAR   strCommonFilesPath[MAX_PATH+1]              = {0};
        TCHAR   strModuleName[MAX_PATH+1]                   = {0};

        TCHAR   szScsCommsDllName[] = "ScsComms.dll";
        TCHAR   szTransmanDllName[] = "Transman.dll";

        TCHAR   strPkiBaseDir[MAX_PATH+1]                   = {0};
	    TCHAR   strServerGroupName[MAX_PATH+1]				= {0};
        TCHAR   strPrimaryServerName[MAX_PATH+1]            = {0};
        TCHAR   strUserName[MAX_PATH+1]                     = {0};
        TCHAR   strPassword[MAX_PATH+1]                     = {0};
		
		HKEY    hKey;

	    // Get the size of the CustomActionData data first
	    DWORD	dwLen			= 0;
	    TCHAR	*multipath		= NULL;
	    TCHAR	*pdeststr		= NULL;
		UINT	uCSPKIret		= 0;

	    if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &dwLen ) )
	    {
		    sssnprintf(szMessString, sizeof(szMessString), "MSIServerPKIStandAlone: MsiGetProperty failed");
		    MsiLogMessage(hInstall, szMessString);
	    }
	    else
	    {
		    sssnprintf(szMessString, sizeof(szMessString), "MSIServerPKIStandAlone: CustomActionData dwlen %d.", dwLen);
		    MsiLogMessage(hInstall, szMessString);
		    // This is required because we get about 400+ chars from MSI,
		    // we need to to cat the string and it's way bigger then MAX_PATH 
		    dwLen=dwLen+100;  
		    multipath = new TCHAR [ dwLen ];  // Allocate more room for parsing
		    sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- allocated custom action data + 100 = %d.", dwLen );
		    MsiLogMessage(hInstall, szMessString);

		    if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("CustomActionData"), multipath, &dwLen ) )
		    {
			    ParseCAD ( hInstall, 
				           multipath,
                           6,
				           strPkiBaseDir,
                           array_sizeof(strPkiBaseDir),
				           strServerGroupName,
                           array_sizeof(strServerGroupName),
				           strPrimaryServerName,
                           array_sizeof(strPrimaryServerName),
				           strUserName,
                           array_sizeof(strUserName),
				           strPassword,
                           array_sizeof(strPassword),
                           strCommonFilesPath,
                           array_sizeof(strCommonFilesPath) );
		    }
		    else
		    {
			    sssnprintf(szMessString, sizeof(szMessString), "MSIServerPKIStandAlone: MsiGetProperty failed.");
			    MsiLogMessage(hInstall, szMessString);
		    }
	    }

		sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- Common files dir is: %s", strCommonFilesPath ); 
	    MsiLogMessage( hInstall, szMessString );

        HMODULE scscomms = NULL;
        HMODULE transman = NULL;
        DWORD transhan = 0;
        DWORD ret = 0;
        bool use_root_auth = false;

        // Load ScsComms.

        _tcsncpy( strModuleName, strCommonFilesPath, array_sizeof(strModuleName) );
        strModuleName[ array_sizeof(strModuleName) - 1 ] = 0;

        _tcsncat( strModuleName, szScsCommsDllName, array_sizeof(strModuleName) - _tcslen(strModuleName) );
        strModuleName[ array_sizeof(strModuleName) - 1 ] = 0;

		sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- strModuleName is: %s", strModuleName ); 
	    MsiLogMessage( hInstall, szMessString );

	    scscomms = LoadLibrary( strModuleName );
        if( scscomms != NULL )
        {
		    sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- ScsComms loaded successfully from: %s", strModuleName ); 
	        MsiLogMessage( hInstall, szMessString );
        }
        else
        {
		    sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- ScsComms load from %s failed: %lu. Rollback installer.", strModuleName, GetLastError() ); 
    	    MsiLogMessage( hInstall, szMessString );
			nRtrnCode = ERROR_INSTALL_FAILURE;
            goto MSIServerPKIStandAlone_Fail;
        }

        // Load Transman.

        _tcsncpy( strModuleName, strCommonFilesPath, array_sizeof(strModuleName) );
        strModuleName[ array_sizeof(strModuleName) - 1 ] = 0;

        _tcsncat( strModuleName, szTransmanDllName, array_sizeof(strModuleName) - _tcslen(strModuleName) );
        strModuleName[ array_sizeof(strModuleName) - 1 ] = 0;

	    transman = LoadLibrary( strModuleName );
        if( transman != NULL )
        {
		    sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- Transman loaded successfully from: %s", strModuleName ); 
	        MsiLogMessage( hInstall, szMessString );
        }
        else
        {
		    sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- Transman load from %s failed: %lu", strModuleName, GetLastError() ); 
    	    MsiLogMessage( hInstall, szMessString );
            goto MSIServerPKIStandAlone_Fail;
        }

        // Init transman.  This can be called mutliple times per process.

        ret = LoadTransmanFuncs( transman );
        if( ret != 0 )
        {
		    sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- LoadTransmanFuncs failed: %lu", ret ); 
    	    MsiLogMessage( hInstall, szMessString );
        }

        if( InitTransman != NULL )
        {
            transhan = InitTransman( NULL, NULL, NULL );

            if( transhan == 0 )
            {
    	        MsiLogMessage( hInstall, "MSIServerPKIStandAlone- InitTransman failed." );
                goto MSIServerPKIStandAlone_Fail;
            }

            ret = SetTrustedRootsInDir( strPkiBaseDir );

            if( ret == P_NO_PATH )
            {
                // No trusted roots, we will have to do an insecure bootstrap.

                use_root_auth = false;
	            MsiLogMessage( hInstall, "MSIServerPKIStandAlone- SetTrustedRootsInDir found no trusted roots, using name auth" );
            }
            else if( ret != 0 )
            {
		        sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- SetTrustedRootsInDir failed: %lu", ret ); 
    	        MsiLogMessage( hInstall, szMessString );
                goto MSIServerPKIStandAlone_Fail;
            }
        }
        else
        {
	        MsiLogMessage( hInstall, "MSIServerPKIStandAlone- InitTransman function pointer invalid." );
        }

		uCSPKIret = CreateServerPKI	( hInstall,
                          false,	    // generate Server Cert
                          use_root_auth,
                          scscomms,
	                      strPkiBaseDir,
                          strPrimaryServerName,
                          strUserName,
                          strPassword );

		sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI: gen Cert uCSPKIret :0x%08X", uCSPKIret);
		MsiLogMessage( hInstall, szMessString );

		if ( uCSPKIret == ERROR_SUCCESS )
		{
	        MsiLogMessage(hInstall, "MSIServerPKIStandAlone- Server cert done.");
			uCSPKIret = CreateServerPKI	( hInstall,
							true,		// generate Login CA
							use_root_auth,
							scscomms,
							strPkiBaseDir,
							strPrimaryServerName,
							strUserName,
							strPassword );
			sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI: gen Login uCSPKIret :0x%08X", uCSPKIret);
			MsiLogMessage( hInstall, szMessString );

			MsiLogMessage(hInstall, "MSIServerPKIStandAlone- Login CA cert done.");
		}
		else
		{
			// In a failover to standalone install we need to remove the following registries so they don't get migrated
			// This will remove the Parent Key value & set the InheritGroupSettings to zero 
			TCHAR szBuf[MAX_PATH] ={0}; 
			DWORD dwVal = 0;
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), 0, KEY_SET_VALUE, &hKey))
			{
				if( ERROR_SUCCESS != RegSetValueEx( hKey, szReg_Val_Parent, 0,
					REG_EXPAND_SZ, (LPBYTE) szBuf, (DWORD) lstrlen(szBuf)+1))
				{
					MsiLogMessage( hInstall, _T("Unable to blank parent key in registry") );
				}
				if( ERROR_SUCCESS != RegSetValueEx( hKey, szReg_Val_InheritGroupSettings, 0,
					REG_DWORD, (LPBYTE) &dwVal , sizeof(DWORD) ) )
				{
					MsiLogMessage( hInstall, _T("Unable to blank InheritGroupSettings key in registry") );
				}
			}

			uCSPKIret = CreateServerPKI	(	hInstall,
											false,	    // generate Server Cert
											use_root_auth,
											scscomms,
											strPkiBaseDir,
											NULL,
											strUserName,
											strPassword );
			sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI: FAILOVER gen Cert uCSPKIret :0x%08X", uCSPKIret);
			MsiLogMessage( hInstall, szMessString );
			MsiLogMessage(hInstall, "MSIServerPKIStandAlone- Server cert done.");

			uCSPKIret = CreateServerPKI	(	hInstall,
											true,		// generate Login CA
											use_root_auth,
											scscomms,
											strPkiBaseDir,
											NULL,
											strUserName,
											strPassword );
			sssnprintf( szMessString, sizeof(szMessString), "CreateServerPKI: FAILOVER gen Login uCSPKIret :0x%08X", uCSPKIret);
			MsiLogMessage( hInstall, szMessString );
			MsiLogMessage(hInstall, "MSIServerPKIStandAlone- Login CA cert done.");
		}

MSIServerPKIStandAlone_Fail:

        // Unload Transman

        if( transman != NULL )
        {
            DeinitTransman( transhan );

	        if( FALSE == FreeLibrary( scscomms ) )
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
		            sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- FreeLibrary on Transman.dll failed: %s", (LPCTSTR)lpMsgBuf ); 
		            MsiLogMessage( hInstall, szMessString );
		            LocalFree( lpMsgBuf );
                }
	        }
        }

        // Unload ScsComms.

        if( scscomms != NULL )
        {
	        if( FALSE == FreeLibrary( scscomms ) )
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
		            sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone- FreeLibrary on ScsComms.dll failed: %s", (LPCTSTR)lpMsgBuf ); 
		            MsiLogMessage( hInstall, szMessString );
		            LocalFree( lpMsgBuf );
                }
	        }
        }

        if( multipath != NULL )
        {
            delete [] multipath;
        }
    }
    catch( ... )
    {
		MsiLogMessage( hInstall, "MSIServerPKIStandAlone- Unhandled exception." );
    }

	return nRtrnCode;
}



///////////////////////////////////////////////////////////////////////////
//  Name:
//		CreatePKITargetPaths
//  Purpose:
//		Create the PKI outputfile targetpaths.
//
//  Return Values:
//
//  08/24/2004 Daniel Kowalyshyn - Function created.
//   
///////////////////////////////////////////////////////////////////////////
BOOL CreatePKITargetPaths ( MSIHANDLE hInstall, 
                            LPTSTR strPrimaryServerName,
						    LPTSTR strCertFilePath, 
						    LPTSTR strPrivateKeyFilePath, 
						    LPTSTR strRootCertFilePath, 
						    LPTSTR strCSRFilePath, 
    					    LPTSTR szName,
						    LPTSTR szServerGroupGuid,
						    LPTSTR szCert,
						    LPTSTR szKey,
						    LPTSTR szRootCert,
						    LPTSTR szCSR )
{
    TCHAR	szMessString [MAX_PATH+1] = {0};
    TCHAR   szGenericFileName [MAX_PATH+1] = {0};

	AddSlash( strCertFilePath );
	AddSlash( strPrivateKeyFilePath );
	AddSlash( strRootCertFilePath );
	AddSlash( strCSRFilePath );

	// GetServerGroupGuid 

	GetServerGroupGuid ( szServerGroupGuid, GET_SERVER_GROUP_GUID_BYTE_SIZE );

	_tcscpy ( szGenericFileName, szServerGroupGuid );   // copy the server group guid
	_tcscat ( szGenericFileName, "." );                 // add the period
	_tcscat ( szGenericFileName, "0.");                 // zero counter and add the period

    // concatenate the server name

	_tcscat ( strCertFilePath, szName );
	_tcscat ( strPrivateKeyFilePath, szName );
    _tcscat ( strCSRFilePath, szName );

    // add a period

	_tcscat ( strCertFilePath, "." );
	_tcscat ( strPrivateKeyFilePath, "." );
    _tcscat ( strCSRFilePath, "." );

	// concatenate the genericfile name

	_tcscat ( strCertFilePath, szGenericFileName );
	_tcscat ( strPrivateKeyFilePath, szGenericFileName );
	_tcscat ( strRootCertFilePath, szGenericFileName );
	_tcscat ( strCSRFilePath, szGenericFileName );

    // concatenate object names

	_tcscat ( strCertFilePath, szCert );
	_tcscat ( strPrivateKeyFilePath, szKey );
	_tcscat ( strRootCertFilePath, szRootCert );
	_tcscat ( strCSRFilePath, szCSR );

	sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone: strCertFilePath %s.", strCertFilePath );
	MsiLogMessage( hInstall, szMessString );
	sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone: strPrivateKeyFilePath %s.", strPrivateKeyFilePath );
	MsiLogMessage( hInstall, szMessString );
	sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone: strRootCertFilePath %s.", strRootCertFilePath );
	MsiLogMessage( hInstall, szMessString );
	sssnprintf( szMessString, sizeof(szMessString), "MSIServerPKIStandAlone: strCSRFilePath %s.", strCSRFilePath );
	MsiLogMessage( hInstall, szMessString );

	return TRUE;
}

//------------------------------------------------------------------------
// Update the Issuer object with the override information from the registry
// depending on the specific cert type being issued.
//
// Parameters:
//   hScsCommsKey  [in]     An openned VirusProtect6\CurrentVersion\ScsComms key.
//   szUserName    [in]     This function looks up the role associated with
//                          this user and stores that in the certificate.
//   eCertType     [in]     Type of certificate being issued.
//   pvecCertChain [in]     Optional. Certificate chain. The first cert is
//                          the issuing CA.
//   pIssuer       [in/out] Issuer object to update
//------------------------------------------------------------------------
static DWORD UpdateIssuer (MSIHANDLE						  hInstall, 
						   HKEY                               hScsCommsKey,
                           const char						  *szUserName, 
						   ScsSecureComms::IRoleVector        *pRoles,
                           IAuth::CERT_TYPE                   eCertType,
                           std::vector<ICertPtr>              *pvecCertChain,
                           ICertIssuer                        *pIssuer)
{
    RESULT commsres = RTN_OK;
	TCHAR message[ MAX_PATH ] = {0};

    // We must have a valid issuer object.
    SAVASSERT (pIssuer);

    if (pvecCertChain)
    {
        ICertPtr ptrCaCert (pvecCertChain->at (0));
        RESULT commsres = pIssuer->SetIssuingCA (ptrCaCert);
        if (SYM_FAILED (commsres))
        {
			sssnprintf( message, sizeof(message), "Failed setting the issuing CA: 0x%08X", commsres);
			MsiLogMessage( hInstall, message );
            return SecureCommsErrorToVpCommonError (commsres);
        }
    }

    int         nLifetime = SERVER_GROUP_CA_DEFAULT_LIFETIME;
    int         nPreDate  = SERVER_GROUP_CA_DEFAULT_PREDATE;
    int         nPathLen  = 0;
    int         nNumRoles = 0;
    bool        bIsCa     = false;
	bool		bUseInputRoleList = false;
    const char *roles[10] = {0};

    // specified data into the Cert Signing Request.
    switch (eCertType)
    {
        case IAuth::SERVER_GROUP_CA_CERT_TYPE:
            nPathLen  = SERVER_GROUP_CA_DEFAULT_PATH_LEN;
            bIsCa     = true;
            break;

        case IAuth::SERVER_EE_CERT_TYPE:
            roles[nNumRoles++] = BUILTIN_ROLE_SERVER;
            break;

        case IAuth::LOGIN_CA_CERT_TYPE:
            nPathLen  = LOGIN_CA_CERT_DEFAULT_PATH_LEN;
            bIsCa     = true;
            break;

        case IAuth::LOGIN_EE_CERT_TYPE:
			bUseInputRoleList = true;
            break;

        default:
			sssnprintf( message, sizeof(message), "Error issuing new cert. Unknown type: %d", eCertType);
			MsiLogMessage( hInstall, message );
            return ERROR_BAD_PRAMS;
            break;
    }

    // Setup the expiration timing.
    commsres = pIssuer->SetValidityPeriod (nLifetime);
    if (SYM_FAILED (commsres))
    {
		sssnprintf( message, sizeof(message), "Failed setting the validity period: 0x%08X", commsres);
		MsiLogMessage( hInstall, message );
        return SecureCommsErrorToVpCommonError (commsres);
    }

    // Set the pre dating.  This allows an Admin to accommodate time differences in his environment.
    commsres = pIssuer->SetValidityPreDatePeriod (nPreDate);
    if (SYM_FAILED (commsres))
    {
		sssnprintf( message, sizeof(message), "Failed setting the validity pre-date period: 0x%08X", commsres);
		MsiLogMessage( hInstall, message );
        return SecureCommsErrorToVpCommonError (commsres);
    }

	// this is for the debug statement below
	const char * role = NULL;

    // Encode roles, which give privs.
	if (bUseInputRoleList)
	{
		nNumRoles = pRoles ? pRoles->size() : 0;
		if (pRoles && nNumRoles)
			role = pRoles->at(0);
	    commsres = pIssuer->SetRoles(pRoles);
	}
	else
	{
	    commsres = pIssuer->SetRoles(roles, nNumRoles);
		role = roles[0];
	}

    if (SYM_FAILED (commsres))
    {
		sssnprintf( message, sizeof(message), "Failed setting the validity pre-date period: 0x%08X", commsres);
		MsiLogMessage( hInstall, message );
        return SecureCommsErrorToVpCommonError (commsres);
    }

    // Set certificate basic constraints, CA and path length.
    commsres = pIssuer->SetBasicConstraints (bIsCa, nPathLen);
    if (SYM_FAILED (commsres))
    {
		sssnprintf( message, sizeof(message), "Failed setting the basic constraints: 0x%08X", commsres);
		MsiLogMessage( hInstall, message );
        return SecureCommsErrorToVpCommonError (commsres);
    }
    
	sssnprintf( message, sizeof(message),
                "Issue cert params: [user: %s] type: %d, life: %d, predate: %d, ca: %d, pathlen: %d, roles: %d, first role: %s",
                 szUserName != NULL ? szUserName : "<none>",
                 eCertType,
                 nLifetime,
                 nPreDate,
                 bIsCa,
                 nPathLen,
                 nNumRoles,
                 (nNumRoles > 0 ? role : "<none>"));
	MsiLogMessage( hInstall, message );

    return ERROR_SUCCESS;
}

//------------------------------------------------------------------------
// Fill in the correct certificate chain, excluding the about to be issued
// end-entity certificate. Element 0 is the issuing CA.
//
// Parameters:
//   eCertType    [in]  Type of certificate being issued.
//   vecCertChain [out] Cert chain to fill in.
//
// Throws:
//   std::bad_alloc
// Notes:
//	 Taken from IssueCerts.cpp and modified for install
//------------------------------------------------------------------------
static DWORD SetupCaCertChain (MSIHANDLE hInstall,
							   IAuth::CERT_TYPE eCertType, 
                               std::vector<ICertPtr> &vecCertChain,
							   CAData caData )
{
    RESULT commsres = RTN_OK;
	TCHAR message[ MAX_PATH ] = {0};

    vecCertChain.clear();

    // If we are issuing a Login End-Entity cert, add in the Login CA cert
    // into the front of the cert chain.
    if (eCertType == IAuth::LOGIN_EE_CERT_TYPE)
    {
        char szComputerName[IMAX_PATH];
        Find_FQDN(hInstall,szComputerName,NULL);

        ICertPtr ptrCaCert;
		commsres = GetNewestLoginCaCert (caData.PkiBaseDir, szComputerName, NULL,
                                         true /*require private key*/, ptrCaCert.m_p);
        if (commsres == RTNERR_FILE_NOT_FOUND)
        {
            MsiLogMessage( hInstall, _T("Failed to find the required login ca and private key") );
            return ERROR_FILE_NOT_FOUND;
        }
        else if (SYM_FAILED (commsres))
        {
			sssnprintf( message, sizeof(message), "Error loading login ca object: 0x%08X", commsres );
			MsiLogMessage( hInstall, message );
            return SecureCommsErrorToVpCommonError (commsres);
        }

        vecCertChain.push_back (ptrCaCert);
    }

    // We will need the private key for this server group ca when we are issuing
    // certs off it.
    bool bRequireSvrGrpCaPrivateKey = (eCertType == IAuth::SERVER_EE_CERT_TYPE ||
                                       eCertType == IAuth::LOGIN_CA_CERT_TYPE);

    ICertPtr ptrSrvGrpCaCert;
    commsres = GetNewestServerGroupCaCert (caData.PkiBaseDir, bRequireSvrGrpCaPrivateKey, 
										   NULL,
                                           ptrSrvGrpCaCert.m_p);
    
    if (bRequireSvrGrpCaPrivateKey && commsres == RTNERR_FILE_NOT_FOUND)
    {
        MsiLogMessage( hInstall, _T("Error: no required server group ca and private key") );
        return ERROR_FILE_NOT_FOUND;
    }
    else if (commsres == RTNERR_FILE_NOT_FOUND)
    {
        // When the private key is not required, there is another CA actually
        // issuing the cert. The Server Group CA is provide for convenience.
        // Ignore failures for this case.
        MsiLogMessage( hInstall, _T("No server group ca certificate. We must be bootstrapping") );
    }
    else if (SYM_FAILED (commsres))
    {
		sssnprintf( message, sizeof(message), "Error loading server group root cert object: 0x%08X", commsres );
		MsiLogMessage( hInstall, message );
        return SecureCommsErrorToVpCommonError (commsres);
    }
    else // success
        vecCertChain.push_back (ptrSrvGrpCaCert);

    return ERROR_SUCCESS;
}

//------------------------------------------------------------------------
// Get the SCS CommsKey from the registry
//
// Parameters:
//   hInstall		 [in]   Handle to the installer
//   phScsCommsKey   [out]  Pointer to HKEY
// Note: This is code from IssueCerts.cpp, modified for install
//------------------------------------------------------------------------
DWORD GetScsCommsKey (MSIHANDLE hInstall, HKEY *phScsCommsKey)
{
	DWORD err = ERROR_SUCCESS;
    SAVASSERT (phScsCommsKey);

    *phScsCommsKey = NULL;

    // Open up the ..\VirusProtect6\CurrentVersion\ScsComms reg key
    LONG lRet = RegCreateKey (HKEY_LOCAL_MACHINE, _T("Software\\Intel\\LanDesk\\VirusProtect6\\CurrentVersion\\ScsComms"), phScsCommsKey);
    if (lRet != ERROR_SUCCESS)
    {
        MsiLogMessage( hInstall, _T("Failed to create scs comms reg key") );
        err = ERROR_REG_FAIL;
    }

    return err;
}

//------------------------------------------------------------------------
// Get the current serial number based on the certificate type, increment
// it, and return the incremented value.
//
// Parameters:
//   hScsCommsKey       [in]  An openned VirusProtect6\CurrentVersion\ScsComms key.
//   eCertType          [in]  Type of certificate being issued.
//   pnSerialNum        [out] New serial number.
// Note: This is code from IssueCerts.cpp, modified for install
//------------------------------------------------------------------------
DWORD GetAndIncrementSerialLastNum (MSIHANDLE hInstall, 
									HKEY hScsCommsKey,
									IAuth::CERT_TYPE  eCertType,
									DWORD *pnSerialNum)
{
    HKEY hTempKey = NULL;
	TCHAR message[ MAX_PATH] = {0};

    switch (eCertType)
    {
        case IAuth::SERVER_EE_CERT_TYPE:
        case IAuth::LOGIN_CA_CERT_TYPE:
            if( RegCreateKeyEx (hScsCommsKey, szReg_Key_ServerGroupData, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hTempKey, NULL ) != ERROR_SUCCESS )
				return ERROR_FUNCTION_FAILED;
            break;

        case IAuth::LOGIN_EE_CERT_TYPE:
            // Login end-entity certs -- the serial number must go in a LocalData hive
            // under the caller's hive, do that it doesn't get moved around to other servers
            // if we should be involved in a primary server promote/demote operation.
            if( RegCreateKeyEx (hScsCommsKey, szReg_Key_LocalData, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hTempKey, NULL ) == ERROR_SUCCESS )
				return ERROR_FUNCTION_FAILED;
            break;

        default:
			sssnprintf( message, sizeof(message), "Error issuing new cert. Unknown type: %d", eCertType);
			MsiLogMessage( hInstall, message );
            return ERROR_BAD_PRAMS;
            break;
    }

	DWORD size = sizeof( DWORD );
	if( ERROR_SUCCESS != SymSaferRegQueryValueEx( hTempKey, szReg_Val_ServerGroupCaIssueSerialNum, 0, NULL, (LPBYTE)*pnSerialNum, &size ) )
		*pnSerialNum = 0;
    ++(*pnSerialNum);
	if( ERROR_SUCCESS != RegSetValueEx( hTempKey, szReg_Val_ServerGroupCaIssueSerialNum, 0, REG_DWORD, (LPBYTE)pnSerialNum, sizeof( DWORD ) ) )
	{
		MsiLogMessage( hInstall, _T("Unable to update serial number in registry") );
		return ERROR_FUNCTION_FAILED;
	}

	if( hTempKey )
		RegCloseKey( hTempKey );

	sssnprintf( message, sizeof(message), "Increment serial num: %lu", *pnSerialNum);
	MsiLogMessage( hInstall, message );

    return ERROR_SUCCESS;
}

//------------------------------------------------------------------------
// Issue the specified eCertType assuming the szUserName and szPasswd
// credentials allow that functionality.
//
// Parameters:
//   szUserName      [in]  User name to authenticate the operation
//   szPasswd        [in]  That user name's password (clear text).
//   eCertType       [in]  Type of certificate being issued.
//   pCertSigningReq [in]  Certificate Signing Request to process
//   pNewCertChain   [out] Certificate chain. The new cert is at position
//                         0, followed by any CAs, ending with the server
//                         group root.
// Note: This is code from IssueCerts.cpp, modified for install
//------------------------------------------------------------------------
DWORD IssueCert(MSIHANDLE							hInstall,
				const char                          *szUserName,
				ScsSecureComms::IRoleVector         *pRoles,
				ScsSecureComms::IAuth::CERT_TYPE    eCertType,
				ScsSecureComms::ICertSigningRequest *pCertSigningReq,
				ScsSecureComms::ICertVector         *&ivecNewCertChain,
				CAData								caData )
{
    DWORD ret          = ERROR_SUCCESS;
    HKEY  hScsCommsKey = NULL;
	TCHAR message[ MAX_PATH] = {0};

    try
    {
        // We have now authenticated the credentials. Start creating the certificate.

        // Open up the required reg key
        ret = GetScsCommsKey (hInstall, &hScsCommsKey);
        if (ret != ERROR_SUCCESS)
            throw ret;

        ICertIssuerPtr ptrIssuer;
        SYMRESULT symres = g_objCertIssuerLdr.CreateObject (&ptrIssuer);
        if (SYM_FAILED (symres))
        {
			sssnprintf( message, sizeof(message), "Failed creating a cert issuer object: 0x%08X", symres );
            MsiLogMessage( hInstall, message );
            throw (DWORD)ERROR_SECURE_COMMS;
        }

        std::vector<ICertPtr> vecCertChain;

        // Load up the CA (if any) and root certificate. The new certificate
        // is added to the chain later. Position 0 in the vector contains the
        // issuing CA which may or may not also be the root.
        ret = SetupCaCertChain (hInstall, eCertType, vecCertChain, caData);
        if (ret)
            throw ret;
		else
			MsiLogMessage( hInstall, _T("CA cert chain set up") );

        // Update the Issuer with server specified information based on the
        // certificate type.
        ret = UpdateIssuer (hInstall, hScsCommsKey, szUserName, pRoles, eCertType, &vecCertChain, ptrIssuer);
        if (ret)
            throw ret;
		else
			MsiLogMessage( hInstall, _T("Issuer updated") );

        unsigned long nSerialNum = 0;
        ret = GetAndIncrementSerialLastNum (hInstall, hScsCommsKey, eCertType, &nSerialNum);
        if (ret)
            throw ret;
		else
			MsiLogMessage( hInstall, _T("Last serial number updated") );

        ICertPtr ptrNewCert;
        RESULT commsres = ptrIssuer->IssueCert (nSerialNum, pCertSigningReq, ptrNewCert.m_p);
        if (SYM_FAILED (commsres))
        {
			sssnprintf( message, sizeof(message), "Failed creating new cert: 0x%08X", commsres );
            MsiLogMessage( hInstall, message );
            throw (DWORD)SecureCommsErrorToVpCommonError (commsres);
        }
		else
			MsiLogMessage( hInstall, _T("Certificate issued") );

        // Put the new end-entity cert at the front of the cert chain.
        vecCertChain.insert (vecCertChain.begin(), ptrNewCert);

        // Convert our STL cert vector to an ICertVector instance.
        ivecNewCertChain = new CCertVector (vecCertChain);
        ivecNewCertChain->AddRef();
    }
    catch (std::bad_alloc&)
    {
        ret = ERROR_NO_MEM;
    }
    catch (DWORD dwRet)
    {
        ret = dwRet;
    }

    if (hScsCommsKey)
        RegCloseKey (hScsCommsKey);

    return ret;
}

//------------------------------------------------------------------------
// Issue the specified eCertType assuming the szUserName and szPasswd
// credentials allow that functionality.
//
// Parameters:
//	 MSIHandle		 [in]  Handle to the MSI install session
//   szComName       [in]  Computer name
//   szLocalHostName [in]  FQDN 
//   eCertType       [in]  Type of certificate being issued.
//	 caData			 [in]  The custom action data
// Note: This is code from IssueCerts.cpp, modified for install
//------------------------------------------------------------------------
static DWORD IssueLocalChainedCert( MSIHANDLE hInstall, const char *szComName, const char *szLocalHostName, IAuth::CERT_TYPE eCertType, CAData caData )
{
	DWORD err = ERROR_SUCCESS;

	try
	{
		TCHAR message[ MAX_PATH ] = {0};
		ICertSigningRequestPtr ptrSignReq;
		IKeyPtr ptrPrivateKey;
		RESULT commsres = GenerateCertSigningRequest (eCertType, "",
													szLocalHostName, ptrSignReq.m_p,
													ptrPrivateKey.m_p, NULL);
		if (SYM_FAILED (commsres))
		{
			sssnprintf( message, sizeof(message), "%s: Failed gen cert sign request: 0x%08X", szComName, commsres );
			throw( tstring( message ) ); 
		}

		ICertVectorPtr vecCertChain;
		DWORD ret = IssueCert(hInstall, "", NULL, eCertType, ptrSignReq, vecCertChain.m_p, caData);
		if (ret != ERROR_SUCCESS)
			throw( tstring( "Unable to issue certificate") );

		// Associated the private key with the cert.
		ICertPtr ptrNewCert;
		ptrNewCert.Attach (vecCertChain->at (0));
		commsres = ptrNewCert->SetPrivateKey (ptrPrivateKey);
		if (SYM_FAILED (commsres))
		{
			sssnprintf( message, sizeof(message), "%s: Failed set private key on newly issued cert: 0x%08X", szComName, commsres );
			throw( tstring( message ) ); 
		}

		if (eCertType == IAuth::SERVER_EE_CERT_TYPE)
			commsres = SaveServerEeCertChain (caData.PkiBaseDir, szLocalHostName, NULL, vecCertChain);
		else if (eCertType == IAuth::LOGIN_CA_CERT_TYPE)
		{
/* This should not be needed for install
			ResetSerialNum (eCertType);
*/
			commsres = SaveLoginCaCertChain (caData.PkiBaseDir, szLocalHostName, NULL, vecCertChain);
		}
		else
			commsres = RTNERR_INVALIDARG;

		if (SYM_FAILED (commsres))
		{
			sssnprintf( message, sizeof(message), "%s: Failed to save the issued cert chain: 0x%08X", szComName, commsres );
			throw( tstring( message ) ); 
		}
	}
	catch( tstring strErr )
	{
		MsiLogMessage( hInstall, strErr.c_str() );
		err = ERROR_FUNCTION_FAILED;
	}

    return err;
}

static DWORD IssueSelfSignedCert (MSIHANDLE hInstall,
								  const char           *szComName,
                                  const char           *szLocalHostName,
                                  IAuth::CERT_TYPE      eCertType,
                                  ICertSigningRequest **ppNewSignReq,
                                  ICert               *&pNewCert)
{
	TCHAR message[ MAX_PATH ] = {0};
    HKEY  hScsCommsKey = NULL;

    DWORD ret = GetScsCommsKey (hInstall, &hScsCommsKey);
    if (ret != ERROR_SUCCESS)
        return ret;

    try
    {
        ICertSigningRequestPtr ptrSignReq;
        IKeyPtr                ptrPrivateKey;
        RESULT commsres = GenerateCertSigningRequest (eCertType,
                                                      NULL, szLocalHostName, ptrSignReq.m_p,
                                                      ptrPrivateKey.m_p, NULL);
        if (SYM_FAILED (commsres))
        {
			sssnprintf( message, sizeof(message), "%s: generate %d cert signing request failed: 0x%08X\n",szComName, eCertType, commsres);
			MsiLogMessage( hInstall, message );
            throw SecureCommsErrorToVpCommonError (commsres);
        }

        ICertIssuerPtr ptrIssuer;
        SYMRESULT symres = g_objCertIssuerLdr.CreateObject (&ptrIssuer);
        if (SYM_FAILED (symres))
        {
			sssnprintf( message, sizeof(message), "%s: Failed creating a %d cert issuer object: 0x%08X\n",szComName, eCertType, symres);
			MsiLogMessage( hInstall, message );
            throw (DWORD)ERROR_SECURE_COMMS;
        }

        ret = UpdateIssuer (hInstall, hScsCommsKey, "", NULL, eCertType, NULL, ptrIssuer);
        if (ret)
            throw ret;

        // Reset the Server Group CA's or Login CA serial number back to 0.
//        ResetSerialNum (eCertType);

        ICertPtr ptrNewCert;
        commsres = ptrIssuer->IssueCertSelfSigned (0, ptrSignReq, ptrPrivateKey, ptrNewCert.m_p);
        if (SYM_FAILED (commsres))
        {
			sssnprintf( message, sizeof(message), "%s: Failed issuing %d cert: 0x%08X\n",szComName, eCertType, commsres);
			MsiLogMessage( hInstall, message );
            throw (DWORD)SecureCommsErrorToVpCommonError (commsres);
        }

        commsres = ptrNewCert->SetPrivateKey (ptrPrivateKey);
        if (SYM_FAILED (commsres))
        {
			sssnprintf( message, sizeof(message), "%s: Failed setting %d private key: 0x%08X\n",szComName, eCertType, commsres);
			MsiLogMessage( hInstall, message );
            throw (DWORD)SecureCommsErrorToVpCommonError (commsres);
        }

        // Success ... tell the caller the new CA cert.
        pNewCert = ptrNewCert;
        pNewCert->AddRef();

        // If the caller is interested, they can also have the CSR.
        if (ppNewSignReq)
        {
            *ppNewSignReq = ptrSignReq;
            (*ppNewSignReq)->AddRef();
        }
    }
    catch (std::bad_alloc&)
    {
        ret = ERROR_MEMORY;
    }
    catch (DWORD dwRet)
    {
        ret = dwRet;
    }

    RegCloseKey (hScsCommsKey);

    return ret;
}

//------------------------------------------------------------------------
// Create a new Server Group CA certificate and private key; write them
// out to the hard drive.
//------------------------------------------------------------------------
static DWORD IssueAndSaveSelfSignedCert (MSIHANDLE hInstall, 
										 const char *szComName,
                                         const char *szLocalHostName,
                                         IAuth::CERT_TYPE eCertType,
                                         ICert *&pNewCert,
										 CAData caData)
{
    DWORD ret       = ERROR_SUCCESS;
    RESULT commsres = RTN_OK;
	TCHAR message[ MAX_PATH ] = {0};

    ICertSigningRequestPtr ptrSignReq;
    ret = IssueSelfSignedCert (hInstall, szComName, szLocalHostName, eCertType, &ptrSignReq, pNewCert);
    if (ret)
        return ret;

    // Put the self-issued cert into a vector ... sometimes the save function
    // needs a vector of certs to save.
    CCertVector objCertVector;
    commsres = objCertVector.push_back (pNewCert);
    if (SYM_FAILED (commsres))
    {
		sssnprintf( message, sizeof(message), "%s: Failed converting self-issued %d cert to cert vector: 0x%08X",szComName, eCertType, commsres);
		MsiLogMessage( hInstall, message );
        return SecureCommsErrorToVpCommonError (commsres);
    }

    switch (eCertType)
    {
        case IAuth::SERVER_GROUP_CA_CERT_TYPE:
            // Write the new Server Group CA and private key to the hard drive.
			commsres = SaveServerGroupCaCert (caData.PkiBaseDir, NULL, pNewCert);
            if (SYM_FAILED (commsres))
            {
				sssnprintf( message, sizeof(message), "%s: Failed saving new server group CA: 0x%08X", szComName, commsres);
				MsiLogMessage( hInstall, message );
                return SecureCommsErrorToVpCommonError (commsres);
            }

            // We only save CSRs for the Server Group CA certs. This allows
            // administrator to re-issue the server group CA cert as a subordinate
            // CA from an enterprise cert chain.
            commsres = SaveServerGroupCaCsr (caData.PkiBaseDir, NULL, ptrSignReq);
            if (SYM_FAILED (commsres))
            {
				sssnprintf( message, sizeof(message), "%s: Failed saving server group CA cert signing request: 0x%08X", szComName, commsres);
				MsiLogMessage( hInstall, message );
                return SecureCommsErrorToVpCommonError (commsres);
            }
            break;

        case IAuth::SERVER_EE_CERT_TYPE:
            commsres = SaveServerEeCertChain (caData.PkiBaseDir, szLocalHostName, NULL, &objCertVector);
            if (SYM_FAILED (commsres))
            {
				sssnprintf( message, sizeof(message), "%s: Failed saving new server cert: 0x%08X", szComName, commsres);
				MsiLogMessage( hInstall, message );
                return SecureCommsErrorToVpCommonError (commsres);
            }
            break;

        case IAuth::LOGIN_CA_CERT_TYPE:
            commsres = SaveLoginCaCertChain (caData.PkiBaseDir, szLocalHostName, NULL, &objCertVector);
            if (SYM_FAILED (commsres))
            {
				sssnprintf( message, sizeof(message), "%s: Failed saving new login CA cert: 0x%08X", szComName, commsres);
				MsiLogMessage( hInstall, message );
                return SecureCommsErrorToVpCommonError (commsres);
            }
            break;

        default:
			sssnprintf( message, sizeof(message), "%s: Failed saving new cert, unknown cert type.", szComName);
			MsiLogMessage( hInstall, message );
            return ERROR_BAD_PARAM;
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////
//  Name:
//		CreateRootCertificates
//  Purpose:
//		Create the Root Certificates for a Primary Server
//
//  Return Values: 
//
//  11/02/2004 Sean Kennedy - Function created.
//   
//  Note:  This custom action should be run after WriteRegistryValues but
//			before start services.
///////////////////////////////////////////////////////////////////////////
UINT _stdcall CreateRootCertificates( MSIHANDLE hInstall )
{
    DWORD ret = ERROR_SUCCESS;
	TCHAR szComputerName[ MAX_PATH ] = {0};
	TCHAR *szComName = NULL;

    // MessageBox( NULL, "Stop here.", "Debug", MB_OK );

	try
	{
		// Load SCSComms DLL
		if( FAILED( LoadScsComms() ) )
			throw( _T("Unable to load ScsComms.dll") );

		// Get our custom action data
		CAData caData;
		
		if( false == GetCustomActionData( hInstall, &caData ) )
		    throw( tstring("Unable to obtain CustomActionData") );

		Find_FQDN( hInstall, szComputerName, NULL );

		// Issue the new Server Group CA and write it to disk.
        ICertPtr pRootCert;
		ret = IssueAndSaveSelfSignedCert( hInstall, szComName, szComputerName, 
										IAuth::SERVER_GROUP_CA_CERT_TYPE, pRootCert,
										caData );
		if( ret )
			throw( tstring( "Unable to issue root certificate" ) );
		else
			MsiLogMessage( hInstall, _T("Issued root certificate") );

        // Issue the Server End-Entity and write it to disk.
        ret = IssueLocalChainedCert( hInstall, szComName, szComputerName, IAuth::SERVER_EE_CERT_TYPE, caData );
        if( ret )
            throw( tstring( "Unable to issue Server End-Entity certificate") );
		else
			MsiLogMessage( hInstall, _T("Issued Server End-Entity certificate") );

        // Issue the Login CA and write it to disk.
        ret = IssueLocalChainedCert( hInstall, szComName, szComputerName, IAuth::LOGIN_CA_CERT_TYPE, caData );
        if( ret )
            throw( tstring( "Unable to issue Login CA certificate") );
		else
			MsiLogMessage( hInstall, _T("Issued Login CA certificate") );

        pRootCert.Release();  // ScsComms is unloading, release our objects before then.

		UnLoadScsComms();
	}
	catch( tstring err )
	{
		MsiLogMessage( hInstall, err.c_str() );
		UnLoadScsComms();
	}
	catch( TCHAR *err )
	{
		// In this handler we do not need to unload scscomms
		MsiLogMessage( hInstall, err );
	}
	catch( ... )
	{
		MsiLogMessage( hInstall, _T("Unknown exception caught") );
	}

	return ERROR_SUCCESS;
}

bool GetCustomActionData( MSIHANDLE hInstall, CAData *pData )
{
	bool ret = false;

	// Get the size of the CustomActionData data first
    DWORD	dwLen			= 0;
    TCHAR	*multipath		= NULL;
    TCHAR	*pdeststr		= NULL;
    TCHAR	szMessString [MAX_PATH*4]	= {0};

    if( ERROR_MORE_DATA != MsiGetProperty( hInstall, _T("CustomActionData"), "", &dwLen ) )
    {
	    MsiLogMessage(hInstall, "MsiGetProperty failed" );
    }
    else
    {
	    sssnprintf(szMessString, sizeof(szMessString), "CustomActionData dwlen %d.", dwLen);
	    MsiLogMessage(hInstall, szMessString);
	    // This is required because we get about 400+ chars from MSI,
	    // we need to to cat the string and it's way bigger then MAX_PATH 
	    dwLen=dwLen+100;  
	    multipath = new TCHAR [ dwLen ];  // Allocate more room for parsing
	    sssnprintf( szMessString, sizeof(szMessString), "Allocated custom action data + 100 = %d.", dwLen );
	    MsiLogMessage(hInstall, szMessString);
	    if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("CustomActionData"), multipath, &dwLen ) )
	    {
			memset( (void *)pData, 0, sizeof( CAData ) );
		    ParseCAD ( hInstall, 
			           multipath,
                       6,
			           pData->PkiBaseDir,
                       array_sizeof( pData->PkiBaseDir ),
			           pData->ServerGroupName,
                       array_sizeof( pData->ServerGroupName ),
			           pData->PrimaryServerName,
                       array_sizeof( pData->PrimaryServerName ),
			           pData->UserName,
                       array_sizeof( pData->UserName ),
			           pData->Password,
                       array_sizeof( pData->Password ),
                       pData->CommonFilesPath,
                       array_sizeof( pData->CommonFilesPath ) );
			ret = true;
	    }
	    else
	    {
		    sssnprintf(szMessString, sizeof(szMessString), "MsiGetProperty failed.");
		    MsiLogMessage(hInstall, szMessString);
	    }
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSIDeleteDir()
//
// Parameters:
//		MSIHANDLE - Handle to the install instance
//		const char* - folder to remove
//		const char* - pattern of files to delete
//
// Returns:
//		bool - true if folder removed
//
// Description:  
//	Removes a folder including the files inside of it with notification
//
//////////////////////////////////////////////////////////////////////////
// 8/21/03 - SKENNED function created.
// 1/30/04 - DKOWALYSHYN function de-MFC'ed and modified for my needs
// 6/16/04 - DKOWALYSHYN ported over to clean up virusdef directory
//////////////////////////////////////////////////////////////////////////
bool MSIDeleteDir( MSIHANDLE hInstall, const char* folder, const char* pattern )
{
	BOOL bRet = TRUE;
	WIN32_FIND_DATA hFindData;
	PMSIHANDLE hRec = MsiCreateRecord( 3 );

	TCHAR allfiles[MAX_PATH] = {0};
	TCHAR filename[MAX_PATH] = {0};
	TCHAR temp[MAX_PATH] = {0};

	vpstrncpy( allfiles, folder, sizeof(allfiles) );
	vpstrnappendfile( allfiles, pattern, sizeof(allfiles) );

	HANDLE hFind = FindFirstFile( allfiles, &hFindData );
	while( (INVALID_HANDLE_VALUE != hFind) && (bRet) )
	{
		vpstrncpy( filename, folder, sizeof(filename) );
		vpstrnappendfile( filename, hFindData.cFileName, sizeof (filename) );

		if( !(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			if( hRec && g_hinstDLL )
			{
				LoadString( g_hinstDLL, IDS_DELETING, temp, MAX_PATH );
				tstring info( temp );

				info += " ";
				info += filename;
				MsiRecordClearData( hRec );
				MsiRecordSetString( hRec, 2, info.c_str() );
				MsiProcessMessage( hInstall, INSTALLMESSAGE_ACTIONSTART, hRec );
			}

			vpsnprintf( temp, sizeof (temp), "Deleting file %s", filename ); 
			MsiLogMessage( hInstall, temp );
			DeleteFile( filename );
		}
		else
		{
			if( lstrcmp( hFindData.cFileName, "." ) && lstrcmp( hFindData.cFileName, ".." ) )
				MSIDeleteDir(  hInstall, filename, pattern );
		}

		bRet = FindNextFile( hFind, &hFindData );
	}
	
    DWORD dwResult = GetLastError();
    if (dwResult != ERROR_NO_MORE_FILES)
    {
		vpsnprintf( temp, sizeof (temp), "DeleteDir GetLastError returned %u", dwResult); 
		MsiLogMessage( hInstall, temp );
    }

	if( hFind )
		FindClose( hFind );

	vpsnprintf( temp, sizeof (temp), "Deleting folder %s", folder ); 
	MsiLogMessage( hInstall, temp );
	bRet = RemoveDirectory( folder );
	if( hRec && g_hinstDLL )
	{
		LoadString( g_hinstDLL, IDS_REMOVING, temp, MAX_PATH );
		tstring info( temp );

		info += " ";
		info += folder;
		MsiRecordClearData( hRec );
		MsiRecordSetString( hRec, 2, info.c_str() );
		MsiProcessMessage( hInstall, INSTALLMESSAGE_ACTIONSTART, hRec );
	}

	return bRet ? true : false;
}

