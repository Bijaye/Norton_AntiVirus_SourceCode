// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
#include "classes.h"
#include "SymSaferRegistry.h"

/**********************************************************************************/
CPingParent::CPingParent() {
	StdCreate;
}
/**********************************************************************************/
CPingParent::~CPingParent() {
	StdDestroy;
}
/****************************************************************************************************************/
STDTYPE CPingParent::PingParent(void)
{
	// Create a CBA Addr struct that represents localhost.

	CBA_Addr ms_LocalHostAddr = {0};

	ms_LocalHostAddr.addrSize = sizeof(ms_LocalHostAddr);
	ms_LocalHostAddr.ucProtocol = CBA_PROTOCOL_IP;
	ms_LocalHostAddr.dstAddr.netAddr.ipAddr = 0x0100007f; // localhost: 127 0 0 1

	DWORD port = 0;
	HKEY regKey = 0;
	DWORD dwType, cbData = sizeof(DWORD);

	if ((RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), 0, KEY_READ, &regKey ) == ERROR_SUCCESS ) &&
		(SymSaferRegQueryValueEx( regKey, _T(szReg_Val_AgentIpPort), 0L, &dwType, (LPBYTE)&port, &cbData ) == ERROR_SUCCESS ))
	{
		// Convert to unsigned short, network order.
		((BYTE*)&ms_LocalHostAddr.dstAddr.usPort)[0] = ((BYTE*)&port)[1];
		((BYTE*)&ms_LocalHostAddr.dstAddr.usPort)[1] = ((BYTE*)&port)[0];
	}
	else
	{
		ms_LocalHostAddr.dstAddr.usPort = 0x970b; // 2967 in network order.
	}

	if (regKey != 0)
		RegCloseKey(regKey);

	return SendCOM_REMOVE_PROGRAM(NULL, reinterpret_cast<char*>(&ms_LocalHostAddr), SENDCOM_REMOTE_IS_SERVER|SENDCOM_REMOTE_USES_SECURE_COMM, SERVER_SERVICE_RELOAD_PONG_DATA);
}
/*****************************************************************************/
