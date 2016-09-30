// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Debug Printouts for Unit and Acceptance Test 

#ifndef _ndkDebug_h_
#define _ndkDebug_h_

#include <ndkThreads.h>

extern BOOL dbgVerbose;
extern int	dbgResult;

BOOL dbgInit(int argc,char* argv[],char* name);
int  dbgFinal();

void dbgNL( char* format);
void dbgNil(char* format);
void dbgChr(char* format,char		 value);
void dbgWch(char* format,wchar_t	 value);
void dbgInt(char* format,int		 value);
void dbgLng(char* format,long		 value);
void dbgStr(char* format,char*	 value);
void dbgWcs(char* format,wchar_t* value);
void dbgPtr(char* format,void*	 value);

void wfChr(char* sCase,char* sDescription,char		 cExpected,char		cActual);
void wfWch(char* sCase,char* sDescription,wchar_t	wcExpected,wchar_t  wcActual);
void wfInt(char* sCase,char* sDescription,int		 iExpected,int			iActual);
void wfLng(char* sCase,char* sDescription,long		 lExpected,long		lActual);
void wfStr(char* sCase,char* sDescription,char*		 sExpected,char*		sActual);
void wfWcs(char* sCase,char* sDescription,wchar_t* wsExpected,wchar_t* wsActual);
void wfPtr(char* sCase,char* sDescription,
			  void* uExpected, void* uActual);

void dbgWait();
void dbgRelease();
void dbgKeepAlive();
void dbgAwaitAlive(char* sCase,int iNumber);
void dbgAwait(unsigned long msec);

#endif // _ndkDebug_h_
