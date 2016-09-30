// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/***************************************************************************************/
#include "REGISTRY.H"
#include "SymSaferStrings.h"
/***************************************************************************************/

extern char RegDebug = 0;
extern char ProgramDir[255] = "";	   // directory path where vpreg.nlm was run from (should be sys:\system)
extern DWORD RegThreadCount=0;
extern int regNWversion=0;
extern int *_InstallInProgress = NULL;

#ifdef LINUX
extern void* RegThreadID = NULL;
#else
int   RegThreadID = 0;
#include "winThreads.h"
#endif

#ifdef NLM
#include "FineTimeImplementation.c"
#endif

/************************************************************************************/
void UnloadRegistry () {

	int i;

	// ksr - 2/11/02	Unload
	//dprintf (REGISTRY_NAME" Unloading\n");
	RegistryRunning=0;

	if (RegThreadID)
		ResumeThread (RegThreadID);

	ThreadSwitchWithDelay();

	for (i=200 ; RegThreadCount && i ; i--) {
		NTxSleep (100);
	}
	NTxSleep (100);
}
void UnloadRegistryArg (int nSig) {
	(void)nSig;
	UnloadRegistry();
}
/***************************************************************************************/
#ifdef NLM 
/*
	Registered event handler for volume dismount
*/
static void DownServerCallback(LONG nothing1, LONG nothing2)
{
    REF(nothing1);
    REF(nothing2);
	int tgid;

	tgid = SetThreadGroupID(RegTGID);

	RegSaveDatabase();

	SetThreadGroupID(tgid);
}
/**************************************************************************************/

//this section was added to handle CodeWarrior compiler issues when we
//converted from Watcom. It is not needed for Linux
            int __init_environment( void *reserved )
             {
                return 0;
             }

             int __deinit_environment( void *reserved )
             {
                return 0;
             }
#endif
/***************************************************************************************/
int main(int argc,char *argv[]) {

	int i,RegBreak=0;
	DWORD dwRet=0;
//	char *q,*p;
#ifdef MemCheck
	char* certPtr=NULL;
#endif
	DWORD EventHandle = 0;

	RegThreadCount++;

	RegThreadID=NTxGetCurrentThreadId();
	RegTGID=GetThreadGroupID();

#ifdef NLM //termination signal is used for NLM only and not needed on Linux
	// ksr - 2/11/02	Unload
	atexit( UnloadRegistry );
	AtUnload( UnloadRegistry );		// unload command
	EventHandle = RegisterForEvent((LONG)EVENT_DOWN_SERVER, DownServerCallback, (LONG)0);

 
	signal(SIGTERM, UnloadRegistryArg);	// run the unload in My Thread... (4.10 needs this)
#endif

	StrCopy (ProgramDir,NW_SYSTEM_DIR sSLASH REGISTRY_DIR);

#ifdef LINUX	
	mkdir(ProgramDir,S_IRUSR | S_IWUSR);
#else	
	mkdir(ProgramDir);
#endif

	chdir (ProgramDir);

#ifdef MemCheck
	for ( i=0; i<3; i++ )
	{
		certPtr = (char*)malloc(64);
		if(certPtr)
		{
			free(certPtr);
		}

		certPtr = (char*)calloc(1, 64);
		if ( certPtr )
		{
			char *temp = (char*)realloc( certPtr, 128 );
			if ( temp )
			{
				free( temp );
			}
			else
			{
				free( certPtr );
			}
		}
        certPtr = (char*)realloc( NULL, 64 );
        if ( certPtr )
        {
            free( certPtr );
        }

	}

#endif

	for (i=1;i<argc;i++) {

// add commands for backup, restore, repair, dump
		if (stricmp(argv[i],"DEBUG") == 0) {
			gVPRegDebug = 1;
			continue;
		}
		if (stricmp(argv[i],"BREAK") == 0) {
			RegBreak=1;
			continue;
		}
		if (stricmp(argv[i],"BACKUP") == 0) {

		}
		if (stricmp(argv[i],"RESTORE") == 0) {
			continue;
		}
		if (stricmp(argv[i],"REPAIR") == 0) {
			continue;
		}
		if (stricmp(argv[i],"DUMP") == 0) {
			continue;
		}
	}

	if (RegBreak) Breakpoint(RegBreak);

	dprintf (REGISTRY_NAME" loaded\n");

#ifdef NLM // This is used with NLM synchronization with vpstart and is not used on Linux
    _InstallInProgress = (int*)ImportSymbol(GetNLMHandle(), "InstallInProgress"); // gdf 03/19/2003 fix 1-YVE7V
#endif
	dwRet=RegOpenDatabase();

	switch(dwRet) {
	case ERROR_SUCCESS:
		break;
	case ERROR_EMPTY:
		PurgeFile("vpregdp.dat");
		PurgeFile("vpregdp.bak");
		PurgeFile("vpregdp.sav");
		if (RegOpenDatabase() != ERROR_SUCCESS) {
			ConsolePrintf("Database Problem\n");
			goto done;
			}
		ConsolePrintf(REGISTRY_NAME": ERROR - Database Could Not Be Loaded, Creating New Empty DataBase.\r\n");
// FIND ALL DEF FILES AND PROCESS THEM.
		break;

	case ERROR_NO_MEMORY: 
		ConsolePrintf(REGISTRY_NAME": ERROR - Not Enough Memory to Load Program Database.\r\n");
		goto done;
	default:
#ifdef NLM
		if(*_InstallInProgress)  // gdf 03/19/2003 fix 1-YVE7V 				
		{  // gdf 03/19/2003 fix 1-YVE7V				
			ConsolePrintf(REGISTRY_NAME": ERROR - Could Not Open or Restore Program Database, Creating New DataBase.\r\n");   // gdf 03/19/2003 fix 1-YVE7V				            
			break;   // gdf 03/19/2003 fix 1-YVE7V				
		}  // gdf 03/19/2003 fix 1-YVE7V				
		else  // gdf 03/19/2003 fix 1-YVE7V				
#endif
		{  // gdf 03/19/2003 fix 1-YVE7V				
			ConsolePrintf(REGISTRY_NAME": ERROR - Could Not Open or	Restore Program Database.\r\n");  
			goto done;   // gdf 03/19/2003 fix 1-YVE7V				
		} // gdf 03/19/2003 fix 1-YVE7V
	}

#ifdef NLM
	UnimportSymbol(GetNLMHandle(), "InstallInProgress"); // gdf 03/19/2003 fix 1-YVE7V
#endif

#ifdef NLM //will add for Linux after debug work...
		SuspendThread(RegThreadID);
#endif

	RegCloseDatabase(TRUE);
done:
	RegThreadCount--;
#ifdef NLM
    if ( EventHandle )
        UnregisterForEvent(EventHandle);
	UnimportSymbol(GetNLMHandle(), "InstallInProgress"); // gdf 03/19/2003 fix 1-YVE7V
#endif
	// ksr - 2/11/02	Unload
	exit( 0 );
}

/************************************************************************************/
#ifdef NLM
int RegDebugTGID=0;
void DebugScreen(void *nothing) {

	int ScreenHan;

	REF(nothing);

	RegThreadCount++;
	ScreenHan = CreateScreen(REGISTRY_NAME" - Debug",DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);
	if (ScreenHan==EFAILURE || ScreenHan==NULL) {
		RingTheBell();
		RegThreadCount--;
		return;
	}
	SetCurrentScreen(ScreenHan);
	SetAutoScreenDestructionMode(TRUE);
	RegDebugTGID = GetThreadGroupID();

	RenameThread(NTxGetCurrentThreadId(),REGISTRY_NAME" Debug");

	while (RegistryRunning)
		NTxSleep(500);

	RegThreadCount--;

}
#endif

#ifndef LINUX
/************************************************************************************/

VOID Real_dvprintf(const char *format,va_list args) {

	char line[1024];
	int tgid=0;

	if (gVPRegDebug) {
#if defined NLM
		if (RegDebugTGID == 0) {
			BeginThreadGroup(DebugScreen,NULL,8096,NULL);
			NTxSleep(100);
			}
		if (RegDebugTGID) {
			tgid = SetThreadGroupID(RegDebugTGID);

            DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

            DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
            DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
            DWORD dwHours, dwMins, dwSecs;

            SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

            sssnprintf(line,sizeof(line),"%02d:%02d:%02d.%06d[_%ld %ld_]|", dwHours, dwMins, dwSecs, dwMicros, NTxGetCurrentThreadId(), tgid );

			ssvsnprintfappend(line,sizeof(line),format,args);
			NWprintf("%s",line);

			SetThreadGroupID(tgid);
			}
#elif defined LINUX
		ssvsnprintf(line,sizeof(line),format,args);
		printf("%s",line);
#endif
		}
}
VOID Real_dprintf(const char *format,...) {
	va_list args; va_start(args,format); Real_dvprintf(format,args); va_end(args);
}

/***************************************************************************************/
char *_VPstrncpy(char *d,const char *s,int n) {
	char *org=d;
	n--;
	while(n&&*s) {
		*d++=*s++;
		n--;
		}
	*d=0;
	return org;
}		//MLR Fixed
/********************************************************************************/
BOOL StrEqual(char *s1,char *s2) {// returns TRUE if the strings are the same

	int i;

	if (strlen(s1)!=strlen(s2)) return FALSE;

	for (i=0 ; i<strlen(s1) && i<strlen(s2) ; i++)
		if (s1[i]!=s2[i])
			return FALSE;

	return TRUE;
}
/*****************************************************************************************/
#endif
