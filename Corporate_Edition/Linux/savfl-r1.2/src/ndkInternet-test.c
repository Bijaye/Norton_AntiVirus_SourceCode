// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Internet Access functions

// ================== ndkInternet-test ==================================================

#include <ndkThreads.h>

#include <ndkInternet.h>

#include "ndkDebug.h"

#include <string.h>

static void startIPaddress2string(void* arg) {
	char from[16] = "192.168.55.18",to[16],*s; struct in_addr address;

	dbgNil("startIPaddress2string()");
	dbgStr("from == \"%s\"",from);
	inet_aton(from,&address);
	
	dbgAwait(1000);
	
	dbgPtr("NWinet_ntoa(to,address) -> %u",s = NWinet_ntoa(to,address));
	dbgStr("to == \"%s\"",to);
	// TODO fix. Not endian nor platform safe:
	wfPtr("startIPaddress2string","NWinet_ntoa()", &to, s);
	wfStr("startIPaddress2string","NWinet_ntoa()", from, to);
		
	dbgKeepAlive();
}

static void ipAddress2string() {
	int nr = 10,i;
	for (i = 0; i < nr; i++) {
		BeginThread(startIPaddress2string, NULL, 0);
	}
	dbgAwaitAlive("ipAddress2string",nr);
}

static void computerInfo() {
	char sHost[HOST_NAME_MAX + 1];
	unsigned int uSize = sizeof(sHost);
	dbgInt("GetComputerName(sHost,&uSize) -> %i",GetComputerName(sHost,&uSize));
	dbgStr("sHost == \"%s\"",sHost);
	dbgInt("uSize == %u",uSize);
	wfLng("computerInfo", "uSize", strlen(sHost), uSize);

	char sMACaddress[256];
	dbgNil("GetMACaddress(sMACaddress)"); GetMACaddress(sMACaddress);
	dbgStr("sMACaddress == \"%s\"",sMACaddress);
}

int main(int argc,char* argv[]) {
	if (dbgInit(argc,argv,"Internet")) {
		dbgNL("ipAddress2string()"); ipAddress2string();
		dbgNL("computerInfo()");     computerInfo();
	}
	return dbgFinal();
}
