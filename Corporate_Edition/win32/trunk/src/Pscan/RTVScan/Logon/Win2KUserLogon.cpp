#ifdef WIN32
#ifdef REAL_SERVICE

#define _WIN32_WINNT 0x0501

#include "stdafx.h"
#include "Win2KUserLogon.h"
#include "AccessChecker.h"
#include "AccessToken.h"
#include "ScanStatus.h"
#include "dprintf.h"
#include "VPExceptionHandling.h"
#include "CreateScan.h"
#include "WinUser.h"
#include "WTSSessionPublisher.h"
#include "WTSLoader.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

DWORD NotifyStoragesOfNewUserToken(const char *NewUserName, HANDLE hAccessToken);

extern "C" DWORD gCreateQuickScan;

CWin2KUserLogon::CWin2KUserLogon() :
	m_bIsLoggedOff(true),
	m_dwPid (0),
	m_hAccessToken(NULL),
	m_bStartLogonDetection(false)
{


}

CWin2KUserLogon::~CWin2KUserLogon()
{


}

void CWin2KUserLogon::CheckInitialLogon()
{
	if ((m_bIsLoggedOff) && (m_bStartLogonDetection))
	{
		// Check if vptray.exe is there.
		// Run HandleLogOn code.
		if (FindWindow( VPTRAY_CLASS, NULL))
		{
			dprintfTag0(DEBUGUSERLOGONS, "Win2K --- CheckInitialLogon. Logging in \n");

			HandleUserLogOn();
		}
	}

}
bool CWin2KUserLogon::HandleUserLogOn()
{
	if (!m_bIsLoggedOff)
		return false;

	// 3 areas supported by Sedona
	// -----------------------------
	// 1. ScanListener   ProcessLogon code
	// 2. StorageListener ProcessLogon code
	// 3. SettingsListener ProcessLogon code
	
	ProcessLogon();

	//
	// ScanListener
	//
  	dprintfTag0( DEBUGUSERLOGONS, "Win2K HandleUserLogon: Handle Scan Code\n" );
	CScanStatus* pobjScanStatus = NULL;
    if( ERROR_SUCCESS == GetActiveScan(pobjScanStatus) )
    {
  		dprintfTag0( DEBUGUSERLOGONS, "Win2K HandleUserLogon: Active Scan detected\n" );

        SAVASSERT(pobjScanStatus != NULL);
        if( pobjScanStatus != NULL  && 
            GetVal(pobjScanStatus->hkey,szReg_Value_DisplayStatusDlg,0) &&
            false == CUIManager::GetInstance().ResultsViewExists(pobjScanStatus->ScanID, LOGGER_Manual) )
        {
	  		dprintfTag0( DEBUGUSERLOGONS, "Win2K HandleUserLogon: DisplayResultsView\n" );
            pobjScanStatus->SetSessionID(CWTSLoader::GetInstance().GetActiveConsoleSessionId());
            CUIManager::GetInstance().CreateResultsViewProxy(*pobjScanStatus);
        }

		pobjScanStatus->Release();
    }

	
	//
	// StorageListener
	//
	dprintfTag0( DEBUGUSERLOGONS, "Win2K HandleUserLogon: Handle Storage Code\n" );

	CAccessToken objToken;
	objToken.Attach(m_hAccessToken, TOKEN_WTS);

	_tcscpy(m_szUserName, "");

	//
	// We don't check the return code from ProcessLogon above. We just rely on the
	// m_hAccessToken to be valid
	//
	if (m_hAccessToken)
	{
		const char *szUserName = objToken.GetUserName();
		if (szUserName)
		{
			sssnprintf(m_szUserName, sizeof(m_szUserName), "%s", szUserName);
		}

		dprintfTag1( DEBUGUSERLOGONS, "Win2K HandleUserLogon: Handle Storage Code for UserName = %s\n", m_szUserName );

		if (_tcscmp(m_szUserName, "") != 0)
		{
			ReStartPongEngine(TRUE);
			NotifyStoragesOfNewUserToken(m_szUserName, m_hAccessToken);
			m_bIsLoggedOff = false;
		}
	}

	//
	// SettingsListener
	//

	// QuickScan settings handled in "new" CWin2KUserLogon::ProcessLogon() method

	m_bStartLogonDetection = true;  // This allows one iteration of check from InitPscan and
									// now let processing thread check even if logon didn't succeed.

	return !m_bIsLoggedOff;
}

bool CWin2KUserLogon::HandleUserLogOff()
{
	if (m_bIsLoggedOff)
		return false;

	// 3 areas supported by Sedona
	// -----------------------------
	// 1. ScanListener   ProcessLogon code
	// 2. StorageListener ProcessLogon code
	// 3. SettingsListener ProcessLogon code

	ProcessLogout();

	//
	// ScanListener
	//

	// * No actions needed *

	//
	// StorageListener
	//

	dprintfTag0( DEBUGUSERLOGONS, "Win2K HandleUserLogOff: Handle Storage Code\n" );

	dprintfTag1( DEBUGUSERLOGONS, "Win2K HandleUserOff: Handle Storage Code for UserName = %s\n", m_szUserName );

	if (_tcscmp(m_szUserName, "") != 0)
	{
		ReStartPongEngine(TRUE);
		NotifyStoragesOfNewUserToken(m_szUserName, m_hAccessToken);
		return true;
	}


	//
	// SettingsListener
	//

	// * No actions needed *

	return false;
}

/**********************************************************************************************/
DWORD CWin2KUserLogon::GetSidString(PSID sid, char *out, size_t out_bytes) 
{

	PUCHAR		pNumSubs;
	PDWORD		pdwSubAuthority;
	int			i;

	strcpy(out, "S-1-5");	// We need a string showing SID - revision 1 - NT Authority, then all the subauthorities. // Localization OK
	pNumSubs = GetSidSubAuthorityCount(sid);
	for(i = 0; i < *pNumSubs; i++) {
		pdwSubAuthority = GetSidSubAuthority(sid, i);
		//EA - 02/25/2000
		//make sure we use the unsigned integer format
		//coz when we migrate OS form NT to W2K the GetSidSubAuthority func returns
		//back a sid value which exceeds the range of signed integer
		sssnprintfappend(out, out_bytes, "-%u", *pdwSubAuthority);	// Localization OK
		//EA - 02/25/2000
		}

	return ERROR_SUCCESS;
}

//Checks if the default scan options have been copied for the current user.
//Returns true if already copied and false if not copied yet.
bool CWin2KUserLogon::IsCopiedDefaultScanOptions(PHKEY phCustomTasksKey)
{
    DWORD	dwErr  = NULL,
			dwType = NULL,
			dwData = NULL,
			dwDataSize(sizeof(DWORD));
	char szSubKey[MAX_PATH];
    sssnprintf(szSubKey, sizeof(szSubKey), VpRegBase[HKEY_VP_USER_SCANS&0xff].Key, "");

    dwErr = RegOpenKey(VpRegBase[HKEY_VP_USER_SCANS&0xff].hBase, szSubKey, phCustomTasksKey);
    if( ERROR_SUCCESS != dwErr )
        return false;

    dwErr = SymSaferRegQueryValueEx(*phCustomTasksKey, szReg_Val_Copied_Default_Scan_Options, NULL, &dwType, (BYTE*)&dwData, &dwDataSize);
    if( (ERROR_SUCCESS != dwErr) || ( (REG_DWORD != dwType) && (sizeof(DWORD) != dwDataSize) ) )
        return false;
    else if(0 != dwData)
        return true;

    return false;
}


 /**********************************************************************************************************/
DWORD CWin2KUserLogon::CheckUserKey(PSID sid,DWORD RootID, HKEY *phUserKey)
{
	DWORD index=0;
	char name[256];
	HKEY hkey;
	BOOL f = 0;
	char key[256];

	if (VpRegBase[RootID&0xff].hBase == HKEY_LOCAL_MACHINE)
	{
		return ERROR_SUCCESS;
	}

	if (sid == NULL)
	{
		dprintfTag0( DEBUGUSERLOGONS, "CheckUserKey: Closing user key\n" );
		RegCloseKey(VpRegBase[RootID&0xff].hBase);
		VpRegBase[RootID&0xff].hBase = HKEY_CURRENT_USER;
		return ERROR_SUCCESS;
	}

	GetSidString(sid,key,sizeof(key));

	dprintfTag0( DEBUGUSERLOGONS, "CheckUserKey: Locating the users key\n" );
	if (RegOpenKey(HKEY_USERS,"",&hkey) == ERROR_SUCCESS)
	{
		while (RegEnumKey(hkey,index++,name,256) == 0)
		{
			if (!strcmp(name,key))
			{
				if (VpRegBase[RootID&0xff].hBase != HKEY_CURRENT_USER)
				{
					RegCloseKey(VpRegBase[RootID&0xff].hBase);
					VpRegBase[RootID&0xff].hBase = HKEY_CURRENT_USER;
				}
				if (RegOpenKey(hkey,name,&VpRegBase[RootID&0xff].hBase) == ERROR_SUCCESS)
				{
					dprintfTag0( DEBUGUSERLOGONS, "CheckUserKey: Successful load of client hkey\n" );
					f=1;
					*phUserKey = VpRegBase[RootID&0xff].hBase;
					break;
				}
			}
		}
		RegCloseKey(hkey);
	}

	if (!f)
	{
		dprintfTag0( DEBUGUSERLOGONS, "CheckUserKey: User can NOT run scans because we could not open the key\n" );
	}

	return f?ERROR_SUCCESS:ERROR_NO_USER_REG;
}

//Copies the default scan options in HKLM(created at the time of installation) to HKCU.
HRESULT CWin2KUserLogon::CopyDefaultScanOptions(void)
{
	HKEY hCUCustomTasksKey = NULL,
		 hLMCustomTasksKey = NULL,
		 hCUDefScanOptsKey = NULL;
	char szDefaultScanOptions[MAX_PATH];
		 szDefaultScanOptions[0] = NULL;
	HRESULT hResult = NULL;
	bool bCopiedDefaultScanOptions = false;

	//Check if the default scan options have already been copied.
	dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Checking if Default Scan Options have been copied for the current user.\n" );
	bCopiedDefaultScanOptions = IsCopiedDefaultScanOptions(&hCUCustomTasksKey);

	if(!bCopiedDefaultScanOptions)
		//Copy the default scan options.
	{
		dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Copying Default Scan Options for the current user.\n" );

		//Open the admin default scan options key.
		sssnprintf(szDefaultScanOptions, sizeof(szDefaultScanOptions), VpRegBase[HKEY_VP_ADMIN_CUSTOM_TASKS&0xff].Key, szReg_Key_Scan_Defaults);
		hResult = RegOpenKey(VpRegBase[HKEY_VP_ADMIN_CUSTOM_TASKS&0xff].hBase, szDefaultScanOptions, &hLMCustomTasksKey);
		if( ERROR_SUCCESS == hResult )
		{
			//Create "Default Scan Options" key for the current user.
			vpstrncpy( szDefaultScanOptions, "", sizeof(szDefaultScanOptions)/sizeof(char) );
    		sssnprintf(szDefaultScanOptions, sizeof(szDefaultScanOptions), VpRegBase[HKEY_VP_USER_SCANS&0xff].Key, szReg_Key_Scan_Defaults);

			hResult = RegCreateKey(VpRegBase[HKEY_VP_USER_SCANS&0xff].hBase, szDefaultScanOptions, &hCUDefScanOptsKey);
			if( ERROR_SUCCESS == hResult )
			{
				//Copy the admin default scan options to the current user.
				hResult = RegCopyKeys(hLMCustomTasksKey, hCUDefScanOptsKey);
				if( ERROR_SUCCESS == hResult )
				{
					//Set the szReg_Val_Copied_Default_Scan_Options to 1, close the handles and return.
					if(hCUCustomTasksKey)
					{
						DWORD dwValue = 1;
						hResult = RegSetValueEx(hCUCustomTasksKey, szReg_Val_Copied_Default_Scan_Options, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
						if(ERROR_SUCCESS == hResult)
						{
							dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Default Scan Options have successfully been copied for the current user.\n" );
						}
						else
						{
							dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Error: Could not set the value for CopiedDefaultScanOptions flag.\n" );
						}
					}
					else
					{
						//Might never get here.
						dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Error: Invalid handle for custom tasks reg key of current user. Copy not completed.\n" );
					}
				}
				else
				{
					dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Error: Could not copy the ADMIN Default Scan Options for the current user.\n" );
				}
			}
			else
			{
				dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Error: RegCreateKey of 'Default Scan Options' key for the current user, failed.\n" );
			}
		}
		else
		{
			dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Error: Could not open the ADMIN Default Scan Options reg key.\n" );
		}
	}
	else
	//Default scan options for the current user already copied.
	{
		dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Default Scan Options have already been copied for the current user.\n" );
	}

	if( hLMCustomTasksKey )
	{
		RegCloseKey(hLMCustomTasksKey);
		hLMCustomTasksKey = NULL;
	}

	if( hCUCustomTasksKey )
	{
		RegCloseKey(hCUCustomTasksKey);
		hCUCustomTasksKey = NULL;
	}

	if( hCUDefScanOptsKey )
	{
		RegCloseKey(hCUDefScanOptsKey);
		hCUDefScanOptsKey = NULL;
	}

	return hResult;
}

/***********************************************************************************************************/
DWORD CWin2KUserLogon::ProcessLogon()
{
	static TCHAR szShellName[MAX_PATH] = "";
	static int iSMSRetry = 0;

	DWORD cc = ERROR_SUCCESS;
	DWORD pid = 0;
	HANDLE proc = NULL;
	mSID sid;
	char UserName[NAME_SIZE];
	char tempUser[NAME_SIZE];
	HANDLE hEvent = NULL;
	char* p;
	char* pTemp;


	if( _tcslen( szShellName ) == 0 )
	{
  		// Get the name of the shell
  		cc = GetShellName( szShellName, sizeof(szShellName) );
  		if ( cc != ERROR_SUCCESS )
			{
				dprintfTag0( DEBUGUSERLOGONS, "Win2K ProcessLogon: Unable to Query Shell Name\n");
				memset(szShellName, 0, sizeof(szShellName));

			}

  		dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: Shell name is: %s\n", szShellName );
	}

	//
	// Get the process ID for the guy running the shell
	//
	// dwr - 1-SLNJW merge from 8.1.1
		if( ERROR_SUCCESS == cc )
  		cc = GetUserPIDEx(&pid, szShellName, TRUE);

	//Under some circumstances, the Shell name stored in the registry may not
	//actually be in memory, but an interactive user is logged on.  Use more robust
	//mechanism to see if this is the case.
	if ( cc == ERROR__NOT_FOUND || cc == ERROR_FILE_NOT_FOUND )
  		cc = GetUserPIDEx (&pid, NULL, FALSE);

	//
	// Did we get the ID? And is it different?
	//
	if( cc == ERROR_SUCCESS )
	{
  		if( pid == 0 )
  		{
  			// Shouldn't happen - but we'll make a note of it.

  			dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: Got Invalid Pid: 0x%0x\n", pid );
  		}
  		else if( pid != m_dwPid )
  		{
  			//
  			// Yep!
  			//
  			dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: Got New Pid: 0x%0x\n", pid );

  			m_dwPid = pid;

  			//
  			// Free the handle for the access token if we have one
  			//
  			if ( m_hAccessToken )
  			{
  				CloseHandle( m_hAccessToken );
  				m_hAccessToken = 0;
  			}

  			//
  			// Open a handle for the process
  			//
  			proc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );

  			//
  			// Did we get a handle ok?
  			//
  			if( proc != NULL )
  			{
  				//
  				// Yep, we're good so far.
  				//
  				dprintfTag0( DEBUGUSERLOGONS, "Win2K ProcessLogon: Process open\n" );

  				//
  				// Get an access token for the process
  				//

  				//
  				// And .....
  				//
  				if( OpenProcessToken( proc, TOKEN_ALL_ACCESS & 0xff, &m_hAccessToken ) == FALSE )
  				{
  					dprintfTag1( DEBUGUSERLOGONS, "Win2K Win2K ProcessLogon: Failed to get access token for process [err: %lu].\n", GetLastError() );
  				}
  				else
  				{
  					// We got it!
  					dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: Got Token 0x%08X\n", m_hAccessToken );

  					GetSidFromToken( (PSID)&sid, m_hAccessToken );
  					GetNames( (PSID)&sid, UserName, NULL, NULL );

  					// 1-296YUZ: This is a workaround for problem caused by System Management Server (SMS).
  					// "SMSCliToknLocalAcct" is the account created internally by SMS. The user can never log
  					// in using that account. However, this account is returned as the login account
  					// of the shell process for few seconds after logon when SMS client is installed. This results
  					// in manual scans to fail. To fix this problem, we fail and retry a few times if
  					// the "SMSCliToknLocalAcct"  account is returned as the login account.

  					// Convert username to uppercase before comparision
  					for(p = UserName, pTemp = tempUser; ;++pTemp, ++p)
  					{
  						*pTemp = (char)toupper(*p);
  						if(*p == '\0')
  							break;
  					}

  					// if the account name returned is the SMS account return an error
  					if((strncmp(tempUser, "SMSCLITOKNLOCALACCT", strlen("SMSCLITOKNLOCALACCT")) == 0  ||
  						strncmp(tempUser, "SMSCLITOKNACCT", strlen("SMSCLITOKNACCT")) == 0) &&
  						iSMSRetry < 30)
  					{
  						++iSMSRetry;
  						// set m_dwPid to null so that we don't skip processing if SMS account is returned again
  						m_dwPid = 0;
  						cc = ERROR__NOT_FOUND;
  						dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: Got SMS account: %s\n", UserName );
  					}
  					else /* if( CheckUserName( UserName ) == ERROR_SUCCESS ) */
  					{
  						iSMSRetry = 0;
  						dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: New user is: %s\n", UserName );

						HKEY hUserKey;
  						if (CheckUserKey( (PSID)&sid, HKEY_VP_USER_SCANS, &hUserKey) == ERROR_SUCCESS)
						{
							//create a quickscan
							if( 0 != gCreateQuickScan && GetClientType() != CLIENT_TYPE_SERVER )
							{
								dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: Creating QuickScan with UserKey = %d\n", hUserKey );
								CCreateScan cQuickScan;
								cQuickScan.QuickScan(hUserKey);
							}
						}	  					
  						//Srikanth 3/20/05 Defect 1-3OS87L
  						//Default scan options are created at the ADMIN level(In HKLM), during install.
  						//They have to be copied to the current user hive(In HKCU), for a scan to pick up
  						//these default scan options. This will be done only once, when a new user logs in.
  						//Copy ADMIN Default Scan Options for the current user.
  						CopyDefaultScanOptions();

  						// Notify the WTS session publisher that a new user has logged in.
  						// For Win2K with TS enabled and WinXP and later CWtsSessionPublisher
  						// is not dependent on this call. However, for Win2K without TS 
  						// this is still needed.
  						CWtsSessionPublisher::Instance()->HandleLegacySessionChange(WTS_SESSION_LOGON);
  					}
  				}

  				CloseHandle( proc );

  			}
  		}
  		else
  		{
  			// Nothing has changed.  This will be the case the great majority of the time
  			// on NT4 because we're polling the shell process id.
  		}
	}
		else if( cc == ERROR__NOT_FOUND )
	{
			if ( m_dwPid != 0 )
  		{
  			// Extra logoff detection - GetUserPIDEx will return ERROR__NOT_FOUND
  			// if m_dwPid is not zero, we have not processed a logoff - do it.

  			dprintfTag0( DEBUGUSERLOGONS, "Win2K ProcessLogon: Processing NULL PID condition (logoff).\n" );

  			// This will prompt the PulseWatch thread to handle the logoff.

  			hEvent = OpenEvent (EVENT_MODIFY_STATE, FALSE, NAV_LOGOFF_EVENT);
  			if ( hEvent )
  			{
  				PulseEvent (hEvent);
  				CloseHandle (hEvent);
  			}
  		}
	}
	else
		{
			//
			// Don't know. We had problems getting the process ID
			//
			dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogon: Error %d returned by GetUserPIDEx() in WatchForLogons.\n", cc );
	}

	return( cc );
}

/***********************************************************************************************************/
void CWin2KUserLogon::ProcessLogout()
{
	char UserName[NAME_SIZE];

	// Notify the WTS session publisher that a new user has logged in.
	// For Win2K with TS enabled and WinXP and later CWtsSessionPublisher
	// is not dependent on this call. However, for Win2K without TS 
	// this is still needed.
	CWtsSessionPublisher::Instance()->HandleLegacySessionChange(WTS_SESSION_LOGOFF);

	//if (GetUserName(UserName,&size) == FALSE) // BUG BUG, this ALWAYS returns "SYSTEM" . But that ok here   CC:)
    //{
	//	strcpy(UserName,LS(IDS_USER_NOT_LOGGED_IN));
    //}
	sssnprintf(UserName, sizeof(UserName), "%s", m_szUserName);	

	dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogout for User = %s\n", UserName );

	if (m_hAccessToken)
    {
		dprintfTag1( DEBUGUSERLOGONS, "Win2K ProcessLogout for User = %s\n. Access Token valid", UserName );

		CloseHandle(m_hAccessToken);
		m_hAccessToken = 0;
		//CheckUserName(UserName);
		HKEY hKey = NULL;
		CheckUserKey((PSID)NULL,HKEY_VP_USER_SCANS, &hKey);
	}

	return;
}

#endif
#endif