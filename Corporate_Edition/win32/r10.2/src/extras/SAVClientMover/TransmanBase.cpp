// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// TransmanBase.cpp : Implementation of CNAVClientConnect
#include "stdafx.h"
#include <winsock.h>
#include "TransmanBase.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

/////////////////////////////////////////////////////////////////////////////


	DWORD g_InitTrans;
	HINSTANCE gTransHan = NULL;

// These are global Transman function pointers for the time being, just for simplicity sake.
// I will be moving them to a more proper place.


	tSendCOM_ACT_ON_FILE pfSendCOM_ACT_ON_FILE = NULL;
	tPutVal pfPutVal = NULL;
	tSendCOM_EXEC_PROGRAM pfSendCOM_EXEC_PROGRAM = NULL;
	tMasterFileCopy pfMasterFileCopy = NULL;
	tVerifyAnAddress pfVerifyAnAddress = NULL;
	tSendCOM_CLOSE_FILE pfSendCOM_CLOSE_FILE = NULL;
	tMakeCBAAddrString pfMakeCBAAddrString = NULL;
	tSendCOM_GET_ADDRESS_INFO pfSendCOM_GET_ADDRESS_INFO = NULL;
	tSendCOM_DEL_VALUE pfSendCOM_DEL_VALUE = NULL;
	tSendCOM_START_SCAN pfSendCOM_START_SCAN = NULL;
	tSendCOM_READ_FILE pfSendCOM_READ_FILE = NULL;
	tSendCOM_OPEN_FILE pfSendCOM_OPEN_FILE = NULL;
	tSendCOM_DEL_KEY pfSendCOM_DEL_KEY = NULL;
	tSendCOM_GET_VALUES pfSendCOM_GET_VALUES = NULL;
	tSendCOM_LIST_VALUE_BLOCK pfSendCOM_LIST_VALUE_BLOCK = NULL;
	tSendCOM_DIR_BLOCK pfSendCOM_DIR_BLOCK = NULL;
	tSendCOM_DEL_CONSOLE pfSendCOM_DEL_CONSOLE = NULL;
	tSendCOM_REMOVE_PROGRAM pfSendCOM_REMOVE_PROGRAM = NULL;
	tSendCOM_ADD_CONSOLE pfSendCOM_ADD_CONSOLE = NULL;
	tDeinitTransman pfDeinitTransman = NULL;
	tInitTransman pfInitTransman = NULL;
	tAbortFind pfAbortFind = NULL;
	tFindAllNTServers pfFindAllNTServers = NULL;
	tFindAllNetwareServers pfFindAllNetwareServers = NULL;
	tSendDomainPing pfSendDomainPing = NULL;
	tSendSinglePing pfSendSinglePing = NULL;
	tLocateServer pfLocateServer = NULL;
	tCvtCBA2GRC pfCvtCBA2GRC = NULL;
	tGetData pfGetData = NULL;
	tGetValueFromEnumBlock pfGetValueFromEnumBlock = NULL;
	tGetValueFromBlock pfGetValueFromBlock = NULL;
	tAddValueToBlock pfAddValueToBlock = NULL;
	tGetVal pfGetVal = NULL;
	tGetStr pfGetStr = NULL;
	tSendCOM_LIST_KEY_BLOCK  pfSendCOM_LIST_KEY_BLOCK = NULL;
	tSendCOM_GET_SIG_LIST pfSendCOM_GET_SIG_LIST = NULL;
	tFindAllLocalComputers pfFindAllLocalComputers = NULL;
	tSendCOM_LIST_PATTERNS pfSendCOM_LIST_PATTERNS = NULL;
	tSendCOM_MAKE_ACTIVE_PATTERN pfSendCOM_MAKE_ACTIVE_PATTERN = NULL;
	tSendCOM_SET_VALUES pfSendCOM_SET_VALUES = NULL;

	tResolveAddress pfResolveAddress = NULL;
	tCBASendPing pfSendCBAPing = NULL;
	tSendCOM_READ_GROUP_SETTINGS pfSendCOM_READ_GROUP_SETTINGS = NULL;
	tSendCOM_WRITE_GROUP_SETTINGS pfSendCOM_WRITE_GROUP_SETTINGS = NULL;


//////////////////////////////////////////////////////////////////
//
//	This function was copied from PSCAN.
//	It fills in the global Transman function pointers declared above.
//
//
//////////////////////////////////////////////////////////////////
DWORD LoadTransman(void) 
{
	TCHAR path[512];
	TCHAR key[128];
	HKEY  hkey;
	DWORD size = sizeof(path);
	DWORD ret  = ERROR_GENERAL;

	ssStrnCpy(key,_T("CLSID\\{E381F1E0-910E-11D1-AB1E-00A0C90F8F6F}\\InProcServer32"),sizeof(key));

	//move to global
	//HINSTANCE gTransHan = NULL;

	if (RegOpenKey(HKEY_CLASSES_ROOT,key,&hkey) == ERROR_SUCCESS)
    {
		if (SymSaferRegQueryStringValue(hkey,NULL,path,&size) == ERROR_SUCCESS)
        {
			gTransHan = LoadLibrary(path);
			if (gTransHan)
            {
				if ((pfSendCOM_ACT_ON_FILE = (tSendCOM_ACT_ON_FILE)GetProcAddress(gTransHan,(LPCSTR)117)) == NULL) goto err;
				if ((pfPutVal = (tPutVal)GetProcAddress(gTransHan,(LPCSTR)135)) == NULL) goto err;
				if ((pfSendCOM_EXEC_PROGRAM = (tSendCOM_EXEC_PROGRAM)GetProcAddress(gTransHan,(LPCSTR)158)) == NULL) goto err;
				if ((pfMasterFileCopy = (tMasterFileCopy)GetProcAddress(gTransHan,(LPCSTR)176)) == NULL) goto err;
				if ((pfVerifyAnAddress = (tVerifyAnAddress)GetProcAddress(gTransHan,(LPCSTR)146)) == NULL) goto err;
				if ((pfSendCOM_CLOSE_FILE = (tSendCOM_CLOSE_FILE)GetProcAddress(gTransHan,(LPCSTR)115)) == NULL) goto err;
				if ((pfMakeCBAAddrString = (tMakeCBAAddrString)GetProcAddress(gTransHan,(LPCSTR)128)) == NULL) goto err;
				if ((pfSendCOM_GET_ADDRESS_INFO = (tSendCOM_GET_ADDRESS_INFO)GetProcAddress(gTransHan,(LPCSTR)153)) == NULL) goto err;
				if ((pfSendCOM_DEL_VALUE = (tSendCOM_DEL_VALUE)GetProcAddress(gTransHan,(LPCSTR)156)) == NULL) goto err;
				if ((pfSendCOM_START_SCAN = (tSendCOM_START_SCAN)GetProcAddress(gTransHan,(LPCSTR)118)) == NULL) goto err;
				if ((pfSendCOM_READ_FILE = (tSendCOM_READ_FILE)GetProcAddress(gTransHan,(LPCSTR)113)) == NULL) goto err;
				if ((pfSendCOM_OPEN_FILE = (tSendCOM_OPEN_FILE)GetProcAddress(gTransHan,(LPCSTR)112)) == NULL) goto err;
				if ((pfSendCOM_DEL_KEY = (tSendCOM_DEL_KEY)GetProcAddress(gTransHan,(LPCSTR)110)) == NULL) goto err;
				if ((pfSendCOM_GET_VALUES = (tSendCOM_GET_VALUES)GetProcAddress(gTransHan,(LPCSTR)125)) == NULL) goto err;
				if ((pfSendCOM_LIST_VALUE_BLOCK = (tSendCOM_LIST_VALUE_BLOCK)GetProcAddress(gTransHan,(LPCSTR)109)) == NULL) goto err;
				if ((pfSendCOM_DIR_BLOCK = (tSendCOM_DIR_BLOCK)GetProcAddress(gTransHan,(LPCSTR)107)) == NULL) goto err;
				if ((pfSendCOM_DEL_CONSOLE = (tSendCOM_DEL_CONSOLE)GetProcAddress(gTransHan,(LPCSTR)105)) == NULL) goto err;
				if ((pfSendCOM_REMOVE_PROGRAM = (tSendCOM_REMOVE_PROGRAM)GetProcAddress(gTransHan,(LPCSTR)104)) == NULL) goto err;
				if ((pfSendCOM_ADD_CONSOLE = (tSendCOM_ADD_CONSOLE)GetProcAddress(gTransHan,(LPCSTR)103)) == NULL) goto err;
                if ((pfDeinitTransman = (tDeinitTransman)GetProcAddress(gTransHan,(LPCSTR)140)) == NULL) goto err;
				if ((pfInitTransman = (tInitTransman)GetProcAddress(gTransHan,(LPCSTR)127)) == NULL) goto err;
				if ((pfAbortFind = (tAbortFind)GetProcAddress(gTransHan,(LPCSTR)162)) == NULL) goto err;
				if ((pfFindAllNTServers = (tFindAllNTServers)GetProcAddress(gTransHan,(LPCSTR)172)) == NULL) goto err;
				if ((pfFindAllNetwareServers = (tFindAllNetwareServers)GetProcAddress(gTransHan,(LPCSTR)171)) == NULL) goto err;
				if ((pfSendDomainPing = (tSendDomainPing)GetProcAddress(gTransHan,(LPCSTR)167)) == NULL) goto err;
				if ((pfSendSinglePing = (tSendSinglePing)GetProcAddress(gTransHan,(LPCSTR)166)) == NULL) goto err;
				if ((pfLocateServer = (tLocateServer)GetProcAddress(gTransHan,(LPCSTR)175)) == NULL) goto err;
				if ((pfCvtCBA2GRC = (tCvtCBA2GRC)GetProcAddress(gTransHan,(LPCSTR)161)) == NULL) goto err;
				if ((pfGetData = (tGetData)GetProcAddress(gTransHan,(LPCSTR)136)) == NULL) goto err;
				if ((pfGetValueFromEnumBlock = (tGetValueFromEnumBlock)GetProcAddress(gTransHan,(LPCSTR)148)) == NULL) goto err;
				if ((pfGetValueFromBlock = (tGetValueFromBlock)GetProcAddress(gTransHan,(LPCSTR)139)) == NULL) goto err;
				if ((pfAddValueToBlock = (tAddValueToBlock)GetProcAddress(gTransHan,(LPCSTR)138)) == NULL) goto err;
				if ((pfGetVal = (tGetVal)GetProcAddress(gTransHan,(LPCSTR)129)) == NULL) goto err;
				if ((pfGetStr = (tGetStr)GetProcAddress(gTransHan,(LPCSTR)131)) == NULL) goto err;
				if ((pfSendCOM_LIST_KEY_BLOCK = (tSendCOM_LIST_KEY_BLOCK)GetProcAddress(gTransHan,(LPCSTR)108)) == NULL) goto err;
				if ((pfSendCOM_GET_SIG_LIST = (tSendCOM_GET_SIG_LIST)GetProcAddress(gTransHan,(LPCSTR)157)) == NULL) goto err;
				if ((pfFindAllLocalComputers = (tFindAllLocalComputers)GetProcAddress(gTransHan,(LPCSTR)126)) == NULL) goto err;
				if ((pfSendCOM_LIST_PATTERNS = (tSendCOM_LIST_PATTERNS)GetProcAddress(gTransHan,(LPCSTR)149)) == NULL) goto err;
				if ((pfSendCOM_MAKE_ACTIVE_PATTERN = (tSendCOM_MAKE_ACTIVE_PATTERN)GetProcAddress(gTransHan,(LPCSTR)150)) == NULL) goto err;
				if ((pfSendCOM_SET_VALUES = (tSendCOM_SET_VALUES)GetProcAddress(gTransHan,(LPCSTR)111)) == NULL) goto err;
                
                if ((pfResolveAddress = (tResolveAddress)GetProcAddress(gTransHan,(LPCSTR)200)) == NULL) goto err;
				if ((pfSendCBAPing = (tCBASendPing)GetProcAddress(gTransHan,(LPCSTR)201)) == NULL) goto err;
                // Below added for SSC 4.5 / Navcorp 7.5
                // 4-16-2000 jallee
                if ((pfSendCOM_READ_GROUP_SETTINGS = (tSendCOM_READ_GROUP_SETTINGS)GetProcAddress(gTransHan,(LPCSTR)203)) == NULL) 
                {
                    //DWORD MissingCOM_READ_GROUP_SETTINGS (char *p, DWORD *pdw);
                    //pfSendCOM_READ_GROUP_SETTINGS = MissingCOM_READ_GROUP_SETTINGS;
                }
                if ((pfSendCOM_WRITE_GROUP_SETTINGS = (tSendCOM_WRITE_GROUP_SETTINGS)GetProcAddress(gTransHan,(LPCSTR)204)) == NULL) 
                {
                    //DWORD MissingCOM_WRITE_GROUP_SETTINGS (char *p, DWORD dw);
                    //pfSendCOM_WRITE_GROUP_SETTINGS = MissingCOM_WRITE_GROUP_SETTINGS;
                }
                // Above added for SSC 4.5 / Navcorp 7.5

				ret = ERROR_SUCCESS;
				//CloseHandle(gTransHan);

			}
		}
		RegCloseKey(hkey);
	}
err:
	return ret;
}

DWORD UnLoadTransman(void)
{
    DWORD err=ERROR_SUCCESS;
    if ( gTransHan && !FreeLibrary(gTransHan) )
        err=GetLastError();
    return err;

}

DWORD InitializeTransman()
{
	DWORD dwRetValue = ERROR_SUCCESS;
	CString csAddressCacheKey;
	HKEY hKeyCheck = NULL;

	//Load Transman, if we have not
	if(!g_InitTrans)
	{
		dwRetValue = LoadTransman();
		if( ERROR_SUCCESS == dwRetValue )
		{
			//Initialize Transman
			g_InitTrans = pfInitTransman(NULL,NULL);
			pfCvtCBA2GRC(0);
			if(!g_InitTrans)
			{
				AfxMessageBox((LPCTSTR)"Fail to initialize TRAMSMAN");
				dwRetValue = 1;
				goto exit_InitializeTransman;
			}

		}
		else
		{
			AfxMessageBox((LPCTSTR)"Cannot find TRANSMAN.DLL");
			dwRetValue = 1;
			goto exit_InitializeTransman;
		}
	}

	//Check the local registry if this client entry exists

	//Create string of the regkey we are looking for
	csAddressCacheKey = "SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache";
	
	//Check if ADDRESSCACHE key exist
	if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, csAddressCacheKey, 0, KEY_ALL_ACCESS, &hKeyCheck) )
	{
		dwRetValue = ERROR_SUCCESS;
	}
	else
	{
		//else, attempt to find this key on a server
		AfxMessageBox((LPCTSTR)"Cannot open AddressCache registry key");
	}

exit_InitializeTransman:

	if(hKeyCheck)
		RegCloseKey(hKeyCheck);

	return dwRetValue;
}


DWORD DeinitializeTransman()
{
	DWORD dwRetValue = ERROR_SUCCESS;
	dwRetValue = UnLoadTransman();
	return dwRetValue;
}
