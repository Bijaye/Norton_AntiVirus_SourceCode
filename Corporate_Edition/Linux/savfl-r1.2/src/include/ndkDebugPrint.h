/* nwDebug.h - Header file for nwDebug.c */

#ifndef _ndkDebugPrint_h_
#define _ndkDebugPrint_h_

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

void SysLogLine(char* line);

char *_VPstrncpy(char *d,const char *s,int n);

BOOL StrEqual(char *s1, char *s2);
	
#ifdef __cplusplus
}
#endif
	
#endif // _ndkDebugPrint_h_
