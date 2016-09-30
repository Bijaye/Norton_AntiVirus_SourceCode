// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// exchange_helper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <msi.h>
#include "clientreg.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

//---------------------------------------------------------------------------------
//
//	Routine: MsiOutlookSetupExtension 
//	Purpose: To make sure that the Outlook Setup Extensions are set properly
//
//---------------------------------------------------------------------------------
// 07/26/02 Joe Antonelli Created.
//---------------------------------------------------------------------------------
extern "C" __declspec(dllexport) UINT __stdcall MsiOutlookSetupExtension(MSIHANDLE hInstall)
{
	HKEY hExchange = NULL;

	if( RegOpenKey(HKEY_LOCAL_MACHINE, szReg_Key_Outlook_Extensions, &hExchange) != ERROR_SUCCESS ||
		hExchange == NULL ){
		return GetLastError();
	}

	RegSetValueEx(hExchange, szReg_Val_Outlook_Extension_Setup,0,REG_SZ, (unsigned char*)szReg_Val_Outlook_Reset_String, strlen(szReg_Val_Outlook_Reset_String) );

	RegCloseKey(hExchange);

	return ERROR_SUCCESS;
}

//---------------------------------------------------------------------------------
//
//	Routine: OldEntryCleanup 
//	Purpose: To clean up any entries left over from previous versions
//
//---------------------------------------------------------------------------------
// 06/21/04 Jim Shock - function creates
//---------------------------------------------------------------------------------
extern "C" __declspec(dllexport) UINT __stdcall OldEntryCleanup(MSIHANDLE hInstall)
{
	HKEY hExchange = NULL;

	if( RegOpenKey(HKEY_LOCAL_MACHINE, szReg_Key_Outlook_Extensions, &hExchange) != ERROR_SUCCESS ||
		hExchange == NULL ){
		// return SUCCESS in all cases (may not be an exchange key)
		return ERROR_SUCCESS;
	}

	// bug fix (1-2HE2JD) - remove any SAV plugin values that may be left from previous versions
	//    different scenarios of install / upgrade / remove can leave entries behind
	RegDeleteValue(hExchange, "LDVP");
	RegDeleteValue(hExchange, "SavCorp801");
	RegDeleteValue(hExchange, "SavCorp810");
	RegDeleteValue(hExchange, "SavCorp90");
	RegDeleteValue(hExchange, "Symantec AntiVirus");	// sav 10.0

	RegCloseKey(hExchange);

	return ERROR_SUCCESS;
}
