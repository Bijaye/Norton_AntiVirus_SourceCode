// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK Internet Access functions

// ================== ndkInternet =======================================================

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "ndkDefinitions.h"
#include "ndkInternet.h"

#include "Lock.hpp"

// --------------------- IP Address -----------------------------------------------------

// Converts an Internet address in in_addr format into an ASCII string representing the
// address in dotted notation. If `NWinet_ntoa' is successful, it returns a pointer to a
// character array containing the ASCII string representing the Internet address in
// dotted notation.
//
// The function is thread safe as the user has to allocate his own string, as opposed to
// `inet_ntoa', that relies on a shared statically allocated string. Therefore, we have
// mutexed all invocations.

static pthread_mutex_t NWinet_ntoaMutex = PTHREAD_MUTEX_INITIALIZER;

char* NWinet_ntoa(char* cp,struct in_addr in) {
	Lock lock(&NWinet_ntoaMutex); strcpy(cp,inet_ntoa(in)); return cp;
}

// --------------------- Computer Network Information -----------------------------------

// Get the computer name

BOOL GetComputerName(char* sHost,unsigned int* pSize) {
	// TODO: do something to catch bad sizes. Some callers passing uninitialized size.
	int r = gethostname(sHost, *pSize);
	if ( r == 0 )
	{
		// If the name is too long, it *might* not be null-terminated
		sHost[(*pSize) - 1] = 0;
		*pSize = strlen(sHost);
	}
	else
	{
		sHost[0] = 0;
		*pSize = 0;
	}
	return ! r;
}

// Get MAC address   
	
void GetMACaddress(char* sMACaddress) {
	FILE* file; if (! (file = fopen("/proc/net/arp","r"))) {
		sMACaddress[0] = '\0'; return;
	}
   fscanf(file,"%*[ a-zA-Z]");						// skip over header

	char sNetworkInterface[256];
	do {
      int r = fscanf(file,"%*s %*s %*s %s %*s %s",sMACaddress,sNetworkInterface);
      if (r == EOF) break;  if (r < 2) continue;
   } while (strcmp(sNetworkInterface,"eth0"));

   fclose(file);
}
