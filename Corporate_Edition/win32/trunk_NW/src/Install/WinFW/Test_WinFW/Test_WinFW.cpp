// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Test_WinFW.cpp : Defines the entry point for the console application.
//
#define INITIIDS
#include "stdafx.h"
#include "windows.h"
#include "IWinFW.h"
#include "atlbase.h"

int _tmain(int argc, _TCHAR* argv[])
{
	bool bConfig = (MessageBox(NULL,"To configure Windows Firewall for SCS, press YES, to unconfigure Windows Firewall for SCS, press No","Configure Windows Firewall",
		MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON1) == IDYES);
	char szWinFW[MAX_PATH] = {0};
	::GetModuleFileName(NULL,szWinFW,MAX_PATH);
	char* x = strrchr(szWinFW,'\\');
	*(x+1) =0;
	strcat(szWinFW,"WinFW.dll");
	char szRTVScan[MAX_PATH] = {0};
	CRegKey reg;
	if(reg.Open (HKEY_LOCAL_MACHINE,"Software\\Symantec\\InstalledApps",KEY_READ) == ERROR_SUCCESS)
	{
		unsigned long lc = MAX_PATH;
		if(reg.QueryStringValue ("SAVCE",szRTVScan,&lc) != ERROR_SUCCESS)
		{
			MessageBox(NULL,"SAVCE not installed yet","Error",MB_OK);
			return -1;
		}
		strcat(szRTVScan,"rtvscan.exe");


	}
	else
	{
		MessageBox(NULL,"SAVCE not installed yet","Error",MB_OK);
		return -1;
	}

	SYMAPP_LIST apps;
	SYMAPP a[] = { {szRTVScan,"Symantec AntiVirus Service",true,2967,false},{"C:\\WINDOWS\\system32\\CBA\\pds.exe","Intel Ping Discovery Service",false,38293,false}};
	for(int i = 0; i < sizeof(a)/sizeof(a[0]);i++)
		apps.push_back (a[i]);
	if(bConfig )
	{
		if(FAILED(ConfigWinFw4Apps(szWinFW,apps)))
			printf("configure failed");
		else
			printf("configure ok");
	}
	else
	{
		if(FAILED(UnConfigWinFw4Apps(szWinFW,apps)))
			printf("uninstall failed");
		else
			printf("uninstall ok");
	}
	return 0;
}

