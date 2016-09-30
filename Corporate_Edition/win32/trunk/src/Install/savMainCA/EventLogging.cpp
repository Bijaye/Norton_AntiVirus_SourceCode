// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// EventLogging.cpp: Custom actions used to log install/uninstall events.
//

#include "stdafx.h"

#define DONTEXTERNTRANSMAN
#define INITIIDS

#include "vpcommon.h"
// #include "transman.h"
#include "sidutils.h"
#include "commisc.cpp"
#include "ClientReg.h"

#include "syminterface.h"
#include "SymInterfaceLoader.h"
#include "SavrtModuleInterface.h"

#include "savMainCA.h"
#include "cregent.h"

// using namespace ScsSecureComms;

// forward declarations
static void LogClientInstallAV(MSIHANDLE hInstall);
// static void LogClientInstallFW(MSIHANDLE hInstall);
static void LogClientUninstall(MSIHANDLE hInstall);
static void LogClientUninstallRollback(MSIHANDLE hInstall);
static DWORD GlobalLog(MSIHANDLE hInstall, EVENTBLOCK* log_record);

#if 0
//////////////////////////////////////////////////////////
// Function: InstallingSCF()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//		bool - Boolean to control testing for install or uninstall
//
// Returns:
//		bool - True if Installing/Uninstalling SCF, False otherwise
//
// Description:
//		Function will use the install database handle to query the
//		feature state for SCFMain.  For installs, this function checks
//		to see if SCFMain is being installed.  For uninstalls, this
//		function checks that SCFMain is currently installed, and is
//		being removed.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/04 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
bool InstallingSCF( MSIHANDLE hInstall, bool bInstall )
{
	bool bRetval = false;
	INSTALLSTATE iInstalled, iAction;

	if( ERROR_SUCCESS == MsiGetFeatureState( hInstall, _T("SCFMain"), &iInstalled, &iAction ) )
	{
		if( bInstall )
		{
			if( INSTALLSTATE_LOCAL == iAction )
			{
				MSILogMessage( hInstall, _T("SCFMain is being installed.") );
				bRetval = true;
			}
		}
		else
		{
			if( ( INSTALLSTATE_LOCAL == iInstalled ) && ( INSTALLSTATE_ABSENT == iAction ) )
			{
				MSILogMessage( hInstall, _T("SCFMain is being uninstalled.") );
				bRetval = true;
			}
		}
	}
	else
	{
		MSILogMessage( hInstall, _T("Could not retrieve feature state for SCFMain.") );
	}

	return bRetval;
}
#endif

//////////////////////////////////////////////////////////
// Function: LogInstallEvent()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		ERROR_SUCCESS Always.
//
// Description:
//		This is an immediate mode custom action.
//
//		This custom action is used to create an intall event
//		for SAV, and optionally one for SCF.  These events
//		indicate that an install has completed successfully.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/04 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall LogInstallEvent( MSIHANDLE hInstall )
{
	MSILogMessage( hInstall, _T("Begin Logging Install Event.") );

	LogClientInstallAV(hInstall);

#if 0
	if ( InstallingSCF( hInstall, true ) )
		LogClientInstallFW(hInstall);
#endif

	MSILogMessage( hInstall, _T("End Logging Install Event.") );
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////
// Function: LogUninstallEvent()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		ERROR_SUCCESS Always.
//
// Description:
//		This is an immediate mode custom action.
//
//		This custom action is used to create an unintall event
//		for SAV, and optionally one for SCF.  These events
//		indicate that an uninstall is about to begin.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/04 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall LogUninstallEvent( MSIHANDLE hInstall )
{
	MSILogMessage( hInstall, _T("Begin Logging Uninstall Event.") );

	LogClientUninstall(hInstall);

	MSILogMessage( hInstall, _T("End Logging Uninstall Event.") );
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////
// Function: LogUninstallEvent_RB()
//
// Parameters:
//		MSIHANDLE - Handle to the current install
//
// Returns:
//		ERROR_SUCCESS Always.
//
// Description:
//		This is an immediate mode rollback custom action.
//
//		This custom action is the rollback action for
//		LogUninstallEvent.  This custom action is
//		used to create an unintall failure event
//		for SAV, and optionally one for SCF.  These events
//		indicate that an uninstall has been halted.  The
//		absence of these events indicate uninstall success.
//
//////////////////////////////////////////////////////////////////////////
// 11/11/04 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT _stdcall LogUninstallEvent_RB( MSIHANDLE hInstall )
{
	MSILogMessage( hInstall, _T("Begin Logging Uninstall Rollback Event.") );

	LogClientUninstallRollback(hInstall);

	MSILogMessage( hInstall, _T("End Logging Uninstall Rollback Event.") );
	return ERROR_SUCCESS;
}


// *************************************************************************
//
// Name:        GlobalLog
//
// Description: Initialize the LDVP COM Objects and write a record to the log.
//
// Parameters:  EVENTBLOCK* log_record - Log record to send to the SAV log.
//
// Returns:     None.
//
// *************************************************************************
// 8/20/2002 - RSUTTON : Function created.
// *************************************************************************
static DWORD GlobalLog(MSIHANDLE /*hInstall*/, EVENTBLOCK* log_record  )
{
    GUID clsid_cliproxy      = _CLSID_CliProxy;
    GUID iid_ivirusprotect   = _IID_IVirusProtect;
    GUID iid_iutil2          = _IID_IUtil2;
    GUID iid_iservicecontrol = _IID_IServiceControl;

    IVirusProtect   *pRoot           = NULL;
    IVirusProtect   *pCliScanVP      = NULL;
    IServiceControl *pServiceControl = NULL;
    IUtil2          *pUtil2          = NULL;

    HRESULT         hr;
	HRESULT			hr_cominit;

    hr_cominit = CoInitialize( NULL );

    if( SUCCEEDED( hr_cominit ) )
    {
        hr = CoCreateInstance( clsid_cliproxy, NULL, CLSCTX_INPROC_SERVER, iid_ivirusprotect, reinterpret_cast<void **>( &pRoot ) );

        if ( SUCCEEDED( hr ) )
        {
            // Ensure service is running before using CliProxy. Otherwise, use CliScan.

            hr = pRoot->CreateByIID( iid_iservicecontrol, reinterpret_cast<void **>( &pServiceControl ) );

            if ( SUCCEEDED( hr ) )
            {
                if ( ERROR_SERVICE_NOT_RUNNING != pServiceControl->GetStatus() )
                {
                    hr = pRoot->CreateByIID( iid_iutil2, reinterpret_cast<void **>( &pUtil2 ) );
                }

                pServiceControl->Release();
            }

#if 0
            if ( NULL == pUtil2 )
            {
                // Couldn't use CliProxy, switch to CliScan.

                hr = CoCreateInstance( clsid_cliscan, NULL, CLSCTX_INPROC_SERVER, iid_ivirusprotect, reinterpret_cast<void **>( &pCliScanVP ) );

                if ( SUCCEEDED( hr ) )
                {
                    hr = pCliScanVP->CreateByIID( iid_iutil2, reinterpret_cast<void **>( &pUtil2 ) );

                    pCliScanVP->Release();
                }
            }
#endif

            if ( NULL != pUtil2 )
            {
                // Call LogExternalEvent to send our event to the SAV log.

				pUtil2->LogExternalEvent( 0, (PEXTERNALEVENTBLOCK)log_record );
                pUtil2->Release();
				Sleep(7000);
            }

            pRoot->Release();
        }

        CoUninitialize();
    }

    return ( 0 );
}

//////////////////////////////////////////////////////////
// Function: LogClientInstallAV()
//
// Parameters:
//
// Returns:
//
// Description:
//		Creates a new log event using GlobalLog() indicating
//		that an AV client intsallation.
//
//
//////////////////////////////////////////////////////////////////////////
// 10/26/04 - R Bonchek - function created.
//////////////////////////////////////////////////////////////////////////
void LogClientInstallAV(MSIHANDLE hInstall)
{
	EVENTBLOCK log;
	char line[256];

	memset(&log,0,sizeof(log));

	CString str;
	str.LoadString( IDS_AV_INSTALL );

	vpsnprintf(line,sizeof(line), str);
	
	log.Description = line;
	log.logger = LOGGER_System;
	log.hKey[0] = 0;
	log.Category = GL_CAT_SUMMARY;
	log.Event =  GL_EVENT_CLIENT_INSTALL_AV;
	GlobalLog(hInstall, &log);
}

#if 0
//////////////////////////////////////////////////////////
// Function: LogClientInstallFW()
//
// Parameters:
//
// Returns:
//
// Description:
//		Creates a new log event using GlobalLog() indicating
//		that an FW client intsallation.
//
//
//////////////////////////////////////////////////////////////////////////
// 10/26/04 - R Bonchek - function created.
//////////////////////////////////////////////////////////////////////////
void LogClientInstallFW(MSIHANDLE hInstall)
{
	EVENTBLOCK log;
	char line[256];

	memset(&log,0,sizeof(log));

	CString str;
	str.LoadString( IDS_FW_INSTALL );

	vpsnprintf(line, sizeof (line), str);
	
	log.Description = line;
	log.logger = LOGGER_System;
	log.hKey[0] = 0;
	log.Category = GL_CAT_SUMMARY;
	log.Event =  GL_EVENT_CLIENT_INSTALL_FW;
	GlobalLog(hInstall, &log);
}
#endif

#if 0
//////////////////////////////////////////////////////////
// Function: GetLocalCBAIP()
//
// Parameters: 
//		pointer to char buffer and 
//		size of buffer
//
// Returns: bool
//		true = successful
//		false = failure
//
// Description:
//		Looks up local ip address, requires Winsock
//		Stores in buffer if large enough
//
//////////////////////////////////////////////////////////////////////////
// 6/17/05 - D Van Wagner - function created.
//////////////////////////////////////////////////////////////////////////
#define CBA_IP_PREFIX "(IP)-"
bool GetLocalCBAIP(char *szIP, size_t nSize)
{
	WSAData wsaData;
	char szHostname[80];
	struct hostent *phe=0;
	struct in_addr addr;
	bool bRet = false;

	if (nSize > 0)
		*szIP = 0;

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) == 0)
	{
		if (gethostname(szHostname, sizeof(szHostname)) != SOCKET_ERROR)
		{
			phe = gethostbyname(szHostname);
			if (phe != 0)
			{
				memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
				char* szPrefix=CBA_IP_PREFIX;
				char* pIP=inet_ntoa(addr);
				if (pIP!=0 && (strlen(pIP)+strlen(szPrefix)+1)<=nSize)
				{
					strcpy(szIP, szPrefix);
					strcat(szIP, pIP);
					bRet = true;
				}
			}
		}
		WSACleanup();
	}

	return bRet;
}
#endif

//////////////////////////////////////////////////////////
// Function: LogClientInstallUninstall()
//
// Parameters:
//
// Returns:
//
// Description:
//
//		First attempts to send a log event indicating the client
//		has been uninstalled directly to the paretn via SendCOM.
//
//		The idea is to bypass any queue of events that may be waiting
//		to be forwarded to the parent in order to ensure this event
//		goes up.
//
//		Failing SendCOM, it creates the event using GlobalLog().
//
//
//////////////////////////////////////////////////////////////////////////
// 10/26/04 - R Bonchek - function created.
//////////////////////////////////////////////////////////////////////////
void LogClientUninstall(MSIHANDLE hInstall)
{
    bool bLogged = false;

	EVENTBLOCK	eb;
	TCHAR		eb_line[256];

#if 0
	char		ParentComputerName[NAME_SIZE] = {0};  
    DWORD		dwParentComputerName = NAME_SIZE;
    TCHAR		szCommonDir[MAX_PATH] = {0};
    TCHAR		szPathToScsCommsDll[MAX_PATH] = {0};
	bool		bTransmanLoaded = false;
	DWORD		hTransman = 0;
	mSID		sid = {0};
	DWORD		ret = ERROR_SUCCESS;
	ScsSecureComms::IAuthPtr ptrAuth;
	typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                IAuth,
                                &SYMOBJECT_AuthLogin,
                                &IID_Auth> CAuthLoginLoader;
	CAuthLoginLoader s_objAuthLoginLdr;
	TCHAR szUserName[NAME_SIZE] = {0};
    TCHAR szDomainName[NAME_SIZE] = {0};
	char szLocalIP[21]; // longest ex: (IP)-###.###.###.###
#endif

	// Prepare the event block.

	memset(&eb,0,sizeof(eb));

	{
		CString str;
		str.LoadString( IDS_UNINSTALL );
		vpsnprintf(eb_line, sizeof (eb_line), _T("%s"), (const TCHAR*)str);
	}
	
	eb.Description = eb_line;
	eb.logger = LOGGER_System;
	eb.hKey[0] = 0;
	eb.Category = GL_CAT_SUMMARY;
	eb.Event =  GL_EVENT_CLIENT_UNINSTALL;

#if 0
	// Siebel Defect: 1-42COB4
	// IP address is normally set by local rtvscan
	// but we must set it here because sending directly to parent server
	GetLocalCBAIP(szLocalIP, sizeof(szLocalIP));
	eb.lpCBAAddress = szLocalIP;

	// Siebel Defect: 1-3INH0Z
	// Retrieve a real SID and then try to manafacture a false SID from it.
	// We use a false SID because the call to avoid a  call to LookupAccountSid()
	// in GetNames() which is called by GenerateLogLine() in DoCOM_FORWARD_LOG(),
	// which would fail.

	if ( GetSidFromToken( &sid, NULL ) )
	{
		GetNames( &sid, szUserName, szDomainName, NULL );
		MakeFalseSid( &sid, szUserName, szDomainName );
	}

	eb.pSid = &sid;

	//Fix 1-3TWF6V - Client Uninstall Log still gets forwarded to parent even if you disable it.
	//Verify whether the Log Forwarding is enabled for GL_EVENT_CLIENT_UNINSTALL
	//if enabled then forward the log event,otherwise write to global log

	// TODO:  Call CheckFwdEventKeys() instead.
	{	
		DWORD dwUnInstallLogForward = 1;	//Forward By default
		DWORD dwType, dwSz = sizeof(DWORD);
		CString strKey = _T("");
		CString strVal = _T("");
		strKey.Format("%s\\%s\\%s\\%s", szReg_Key_Main, szReg_Key_Common_Key, szReg_Key_ForwardEvents,szReg_Val_ForwardSAVProduct0);
		strVal.Format("%lu", GL_EVENT_CLIENT_UNINSTALL);

		CRegistryEntry reg( HKEY_LOCAL_MACHINE, strKey, strVal );

		if ( reg.GetValue(&dwType, (LPBYTE)&dwUnInstallLogForward, &dwSz) != ERROR_SUCCESS )
			goto sendcom_failed;

		if(dwType != REG_DWORD)
			goto sendcom_failed;

		//Log Forwarding is disabled for GL_EVENT_CLIENT_UNINSTALL
		if(0 == dwUnInstallLogForward)
			goto sendcom_failed;
	}

	// Load SCS Comms

	if ( SHGetFolderPath( NULL, CSIDL_PROGRAM_FILES_COMMON, NULL, 0, szCommonDir) != ERROR_SUCCESS )
		goto sendcom_failed;

	vpsnprintf ( szPathToScsCommsDll, sizeof (szPathToScsCommsDll), _T("%s\\symantec shared\\ssc\\scscomms.dll"), szCommonDir );

	// Get the parent computer name.

	{
		CRegistryEntry reg( HKEY_LOCAL_MACHINE, "Software\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion", "Parent" );

		if ( reg.GetStringValue(ParentComputerName, &dwParentComputerName) != ERROR_SUCCESS )
			goto sendcom_failed;
	}

	// Load Transman

	if ( LoadTransman() != ERROR_SUCCESS )
		goto sendcom_failed;
	else
		bTransmanLoaded = true;

	hTransman = InitTransman(NULL, NULL, NULL);

    if (hTransman == 0)
		goto sendcom_failed;

	// Set trusted roots dir.

	{
		TCHAR  szSAVDir[MAX_PATH] = {0};
		DWORD  dwSAVDir=MAX_PATH;
		CRegistryEntry reg(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec\\InstalledApps", "SAVCE" );
			if ( reg.GetStringValue(szSAVDir, &dwSAVDir) != ERROR_SUCCESS )
				goto sendcom_failed;

		if ( SetTrustedRootsInDir(szSAVDir) != ERROR_SUCCESS)
			goto sendcom_failed;
	}

	// Initialize auth object.

	if ( SYM_FAILED(s_objAuthLoginLdr.Initialize(szPathToScsCommsDll)) )
		goto sendcom_failed;

    if ( SYM_FAILED(s_objAuthLoginLdr.CreateObject(ptrAuth)) )
		goto sendcom_failed;

	// Attempt to forward the log entry directly to the parent.

	if ( SendCOM_FORWARD_LOG( ptrAuth, ParentComputerName, SENDCOM_REMOTE_USES_SECURE_COMM|SENDCOM_REMOTE_IS_SERVER, &eb ) == ERROR_SUCCESS )
        bLogged = true;

sendcom_failed:

    // The IAuthPtr must be release before the ScsComms dll is unloaded (as part of the transman tear down)
    ptrAuth.Release();

    // Clean up transman.

    if ( hTransman )
        DeinitTransman( hTransman );

	if ( bTransmanLoaded )
		(DWORD) UnLoadTransman();
#endif

	// Communicating directly with parent failed so fall back
	// to logging locally and hope it gets forwarded up.
    if (!bLogged)
    {
	    MSILogMessage( hInstall, _T("SendCOM_FORWARD_LOG failed, falling back to GlobalLog.") );

	    eb.pSid = NULL;

	    GlobalLog( hInstall, &eb );
    }
}

//////////////////////////////////////////////////////////
// Function: LogClientInstallUninstall()
//
// Parameters:
//
// Returns:
//
// Description:
//		Creates a new log event using GlobalLog() indicating
//		that a client has uninstall has been rolled-back
//		before completing.
//
//
//////////////////////////////////////////////////////////////////////////
// 10/26/04 - R Bonchek - function created.
//////////////////////////////////////////////////////////////////////////
void LogClientUninstallRollback(MSIHANDLE hInstall)
{
	EVENTBLOCK log;
	char line[256];

	memset(&log,0,sizeof(log));

	CString str;
	str.LoadString( IDS_UNINSTALL_RB );

	vpsnprintf(line, sizeof (line), str);
	
	log.Description = line;
	log.logger = LOGGER_System;
	log.hKey[0] = 0;
	log.Category = GL_CAT_SUMMARY;
	log.Event =  GL_EVENT_CLIENT_UNINSTALL_ROLLBACK;
	GlobalLog(hInstall, &log);
}
