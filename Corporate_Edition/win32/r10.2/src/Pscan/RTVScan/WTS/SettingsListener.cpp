// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation, All rights reserved.
// CSettingsListener.h

#define _WIN32_WINNT 0x0501
#include "SettingsListener.h"
#include <Wtsapi32.h>

#include "SymSaferRegistry.h"
#include "pscan.h"
#include "savassert.h"
#include "sidutils.h"
#include "createscan.h"
#include "wtssessionpublisher.h"

#define SID_STRING_LENGTH 256
extern "C" DWORD gCreateQuickScan;

CSettingsListener CSettingsListener::g_SettingsListenerInstance;

CSettingsListener& CSettingsListener::GetInstance()
{
    CGuard objGuard(g_SettingsListenerInstance.m_objLock);
    if( false == g_SettingsListenerInstance.m_bInitialized )
    {
        CWtsSessionPublisher::Instance()->UpdateListener(g_SettingsListenerInstance);
        g_SettingsListenerInstance.m_bInitialized = true;
    }
    return g_SettingsListenerInstance;
}

CSettingsListener::CSettingsListener(void):m_bInitialized(false)
{
}

CSettingsListener::~CSettingsListener(void)
{
}

void CSettingsListener::ProcessLogon(const CWtsSessionEvent& objEvent)
{
    HKEY hUserKey = NULL;
    if( ERROR_SUCCESS == GetUserKey(objEvent, hUserKey) )
    {
        //create a quickscan
        if( 0 != gCreateQuickScan && GetClientType() != CLIENT_TYPE_SERVER )
        {
            dprintfTag0( DEBUGUSERLOGONS, "ProcessLogon: Creating QuickScan\n" );
            CCreateScan cQuickScan;
            cQuickScan.QuickScan(hUserKey);
        }

        CopyDefaultScanOptions(hUserKey);

        CloseHandle(hUserKey);
    }
}

void CSettingsListener::SessionChange(const CWtsSessionEvent& objEvent)
{
	switch (objEvent.GetEventType())
	{
		case WTS_SESSION_LOGON:
			ProcessLogon(objEvent);
			break;
        case WTS_REMOTE_CONNECT:
		case WTS_SESSION_LOGOFF:
		case WTS_REMOTE_DISCONNECT:
        case WTS_CONSOLE_CONNECT:
		case WTS_CONSOLE_DISCONNECT:
		default:
			// Ignore other types of events that we aren't interested in but log these.
			dprintfTag2(DEBUGSCAN, _T("Settings Listner: Ignoring session change event: %u for session ID: %u\n"), objEvent.GetEventType(), objEvent.GetSessionId());
			break;
	}
}

DWORD CSettingsListener::GetUserKey(const CWtsSessionEvent& objEvent, HKEY& hUserKey)
{
	if ( NULL == objEvent.GetPSID() )
	{
        SAVASSERT(FALSE);
		return ERROR_INVALID_PARAMETER;
	}
    
    dprintfTag0( DEBUGUSERLOGONS, "CSettingsListener::GetUserKey: Locating the users key\n" );
    
    char szStringSid[SID_STRING_LENGTH];
    HKEY hTempKey = NULL;
    DWORD dwRet = ERROR_NO_USER_REG;
    size_t cbSize =  sizeof(szStringSid);
	if( true == objEvent.GetTextSID(szStringSid, &cbSize) &&
        RegOpenKey(HKEY_USERS,"",&hTempKey) == ERROR_SUCCESS )
	{
        char szUserKeyName[SID_STRING_LENGTH];
        int index = 0;
		while (RegEnumKey(hTempKey,index++,szUserKeyName,sizeof(szUserKeyName)) == 0)
		{
			if (!strcmp(szUserKeyName,szStringSid))
			{
				if (RegOpenKey(hTempKey,szStringSid,&hUserKey) == ERROR_SUCCESS)
				{
                    dwRet = ERROR_SUCCESS;
					dprintfTag1( DEBUGUSERLOGONS, "CSettingsListener::GetUserKey:: Successful load of client hkey%s\n", szStringSid);
					break;
				}
			}
		}
		RegCloseKey(hTempKey);
	}

	if( ERROR_SUCCESS != dwRet )
	{
		dprintfTag0( DEBUGUSERLOGONS, "GetUserKey: User can NOT run scans because we could not open the key\n" );
	}

	return dwRet;
}


DWORD CSettingsListener::CopyDefaultScanOptions(HKEY hUserKey)
{
	HKEY hCUCustomTasksKey = NULL,
		 hLMCustomTasksKey = NULL,
		 hCUDefScanOptsKey = NULL;
	char szDefaultScanOptions[MAX_PATH];
		 szDefaultScanOptions[0] = NULL;
	DWORD dwRes = ERROR_SUCCESS;
	bool bCopiedDefaultScanOptions = false;

	//Check if the default scan options have already been copied.
	dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Checking if Default Scan Options have been copied for the current user.\n" );
	bCopiedDefaultScanOptions = IsCopiedDefaultScanOptions(hUserKey, &hCUCustomTasksKey);

	if(!bCopiedDefaultScanOptions)
		//Copy the default scan options.
	{
		dprintfTag0( DEBUGUSERLOGONS, "CopyDefaultScanOptions: Copying Default Scan Options for the current user.\n" );

		//Open the admin default scan options key.
		sssnprintf(szDefaultScanOptions, sizeof(szDefaultScanOptions), VpRegBase[HKEY_VP_ADMIN_CUSTOM_TASKS&0xff].Key, szReg_Key_Scan_Defaults);
		dwRes = RegOpenKey(VpRegBase[HKEY_VP_ADMIN_CUSTOM_TASKS&0xff].hBase, szDefaultScanOptions, &hLMCustomTasksKey);
		if( ERROR_SUCCESS == dwRes )
		{
			//Create "Default Scan Options" key for the current user.
			vpstrncpy( szDefaultScanOptions, "", sizeof(szDefaultScanOptions)/sizeof(char) );
    		sssnprintf(szDefaultScanOptions, sizeof(szDefaultScanOptions), REGHEADER"\\Custom Tasks\\%s", szReg_Key_Scan_Defaults);

			dwRes = RegCreateKey(hUserKey, szDefaultScanOptions, &hCUDefScanOptsKey);
			if( ERROR_SUCCESS == dwRes )
			{
				//Copy the admin default scan options to the current user.
				dwRes = RegCopyKeys(hLMCustomTasksKey, hCUDefScanOptsKey);
				if( ERROR_SUCCESS == dwRes )
				{
					//Set the szReg_Val_Copied_Default_Scan_Options to 1, close the handles and return.
					if(hCUCustomTasksKey)
					{
						DWORD dwValue = 1;
						dwRes = RegSetValueEx(hCUCustomTasksKey, szReg_Val_Copied_Default_Scan_Options, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
						if(ERROR_SUCCESS == dwRes)
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

	return dwRes;
}

//Checks if the default scan options have been copied for the current user.
//Returns true if already copied and false if not copied yet.
bool CSettingsListener::IsCopiedDefaultScanOptions(HKEY hUserKey, PHKEY phCustomTasksKey)
{
    DWORD	dwErr  = NULL,
			dwType = NULL,
			dwData = NULL,
			dwDataSize(sizeof(DWORD));
	char szSubKey[MAX_PATH];
    sssnprintf(szSubKey, sizeof(szSubKey), REGHEADER"\\Custom Tasks\\%s", "");

    dwErr = RegOpenKey(hUserKey, szSubKey, phCustomTasksKey);
    if( ERROR_SUCCESS != dwErr )
        return false;

    dwErr = SymSaferRegQueryValueEx(*phCustomTasksKey, szReg_Val_Copied_Default_Scan_Options, NULL, &dwType, (BYTE*)&dwData, &dwDataSize);
    if( (ERROR_SUCCESS != dwErr) || ( (REG_DWORD != dwType) && (sizeof(DWORD) != dwDataSize) ) )
        return false;
    else if(0 != dwData)
        return true;

    return false;
}