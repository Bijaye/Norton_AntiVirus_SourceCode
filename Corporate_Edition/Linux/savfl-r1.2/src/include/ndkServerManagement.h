// Copyright 2004 Symantec Corporation. All rights reserved.

#ifndef _ndkServerManagement_h_
#define _ndkServerManagement_h_

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

void StopServerCheck (void* han);
BOOL StartServerCheck(void* handle);
void StartUIScanList(void);

/*ServerManagement*/
//GetFileServerDescriptionStrings -   Returns the name of the company that distributed this copy of NetWare. not used on Linux
//GetFileServerName
void GetFileServerName (   
   WORD   fileServerID,   
   char   *fileServerName);  

//GetServerInformation - for netware servers only - not used in Linux
//SSGetOSVersionInfo- for netware servers only - not used in Linux

void StartConsoleScreen();

#ifdef __cplusplus
}
#endif
  
#endif // _ndkServerManagement_h_
