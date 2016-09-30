// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Internet Access functions

#ifndef _ndkInternet_h_
#define _ndkInternet_h_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif
	
char* NWinet_ntoa(char* cp,struct in_addr in);

BOOL GetComputerName(char* sHost,unsigned int* pSize);

void GetMACaddress(char* sMACaddress);

#ifdef __cplusplus
}
#endif
	
#endif // _ndkInternet_h_
