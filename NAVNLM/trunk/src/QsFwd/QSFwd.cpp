// Copyright (C) 1999 Symantec Corporation
//***************************************************************************
//
// Description: Main entry point for QsFwd.NLM
//
//***************************************************************************


//#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <nwthread.h>
//bnm updated header in NDK
//#include <advanced.h>
# include <nwadv.h>
//bnm end
#include <ws2nlm.h>
#include <nwservst.h>
#include "qsfwd.h"
//#include <nwconio.h> 
void LoadSymbols();
void UnloadSymbols();
void AtQsFwdUnload(void);
void GetServerMajorVersion();
int main (void)
{
	GetServerMajorVersion();
	LoadSymbols();
	atexit(AtQsFwdUnload);
	ExitThread(TSR_THREAD, 0);   
	return 0;
}

void AtQsFwdUnload(void)
{
	UnloadSymbols();
	return;
}

void* GetFunctionPointer(char* szFunctionName)
{
	return ImportSymbol(GetNLMHandle(), szFunctionName);
}

void LoadSymbols()
{
	sym_socket = NULL;
	sym_inet_addr = NULL;
	sym_gethostbyname = NULL; 
	sym_connect = NULL;
	sym_shutdown = NULL;
	sym_send = NULL;
	sym_recv = NULL;
	sym_closesocket = NULL;
	sym_select = NULL;
	if( majorServerVersion < 4 ) //EA 12/08/199
		return;//EA 12/08/199
	else if( majorServerVersion == 4 )//EA 12/08/199
	{//EA 12/08/199 
		if( ( minorServerVersion != 11 ) && ( minorServerVersion != 2 ) && ( minorServerVersion != 20 ) ) //EA 12/08/199 //winsock 2 support from 4.11 onwards
				return;//EA 12/08/199
	}	//EA 12/08/199
   	sym_startup = (int (*) (WORD, LPWSADATA) )GetFunctionPointer("WSAStartup");
   	sym_cleanup = (int (*) () )GetFunctionPointer("WSACleanup");
    sym_socket = (SOCKET (*) (int, int, int) )GetFunctionPointer("WS2_32_socket");
    sym_inet_addr = (unsigned long (*) (const char FAR *) )GetFunctionPointer("WS2_32_inet_addr");
    sym_gethostbyname = (struct hostent FAR * (*) (const char FAR *) )GetFunctionPointer("WS2_32_gethostbyname");
    sym_connect = (int (*) (SOCKET, const struct sockaddr FAR *, int) )GetFunctionPointer("WSAConnect");
    sym_shutdown = (int (*) (SOCKET, int) )GetFunctionPointer("WS2_32_shutdown");
    sym_send = (int (*) (SOCKET , const char FAR * ,int , int ))GetFunctionPointer("WS2_32_send");   
  	sym_recv = (int (*) (SOCKET , const char FAR * ,int , int ))GetFunctionPointer("WS2_32_recv");  
	sym_closesocket = (int (*) (SOCKET) )GetFunctionPointer("WS2_32_closesocket");
	sym_select = (int (*)(int , fd_set FAR *,   fd_set FAR *, fd_set FAR *, const struct timeval FAR *)) GetFunctionPointer("WS2_32_select");       

}

void UnloadSymbols()
{

	if( majorServerVersion < 4 ) //EA 12/08/199
		return;//EA 12/08/199
	else if( majorServerVersion == 4 )//EA 12/08/199
	{//EA 12/08/199 dont know for 4.2 if minor version is 2 or 20 winsock2 only for 4.11 and 4.2
		if( ( minorServerVersion != 11 ) && ( minorServerVersion != 2 ) && ( minorServerVersion != 20 ) ) //EA 12/08/199 //winsock 2 support from 4.11 onwards
				return;//EA 12/08/199
	}	//EA 12/08/199
   	if(sym_startup != NULL)
   	{
   		UnimportSymbol(GetNLMHandle(), "WSAStartup");
		sym_startup = NULL;
   	}	
   	if(sym_cleanup != NULL)
   	{
   		UnimportSymbol(GetNLMHandle(), "WSACleanup");
		sym_cleanup = NULL;
   	}
	if(sym_socket != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_socket");
		sym_socket = NULL;
	}
	if(sym_inet_addr != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_inet_addr");
		sym_inet_addr = NULL;
	}
	if(sym_gethostbyname != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_gethostbyname");
		sym_gethostbyname = NULL;
	}
	if(sym_connect != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WSAConnect");
		sym_connect = NULL;
	}
	if(sym_shutdown != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_shutdown");
		sym_shutdown = NULL;
	}
	if(sym_send != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_send");
		sym_send = NULL;
	}
	if(sym_recv != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_recv");
		sym_recv = NULL;
	}
	if(sym_closesocket != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_closesocket");
		sym_closesocket = NULL;
	}
	if(sym_select != NULL)
	{
		UnimportSymbol(GetNLMHandle(), "WS2_32_select");
		sym_select = NULL;
	}

}

void GetServerMajorVersion()
{
	int ccode;
	GetOSVersionInfoStructure	buf;

	ccode = SSGetOSVersionInfo((BYTE *)&buf,sizeof(GetOSVersionInfoStructure));
	minorServerVersion = buf.OSMinorVersion;
	if (ccode != 0)
		majorServerVersion = 3;
	else
		majorServerVersion = buf.OSMajorVersion;
}