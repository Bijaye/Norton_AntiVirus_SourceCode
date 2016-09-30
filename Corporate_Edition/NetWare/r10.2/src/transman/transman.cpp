// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#define DEF_REG
#include "transman.h"
#include "string.h"
#include "winThreads.h"
#include "ICertSigningRequest.h"
#include "pscan.h"
#include "nwfinfo.h"
#include "SymSaferStrings.h"


DWORD scanThreadIDArray[MAX_THREADS];
extern "C" DWORD scanThreadIDArray[MAX_THREADS];

typedef struct _THREADS {
	struct _THREADS *Next;
	void (*fun)(void *);
	void *p;
	DWORD Id;
	HANDLE han;
	char* name;  //IB modified to char* because name may be longer than 64 bytes long
} THREADS;

typedef struct {
	char *Name;
} THREADDATA;

typedef struct {
	void *block;
	char loc[30];
	long size;
} ttt;


int inited=0;
int TransManMainThread=0;
extern "C" int TransManMainThreadGroup=0;
BOOL CopyFile(LPCTSTR ExistingFile,LPCTSTR NewFile,BOOL FailIfExists);

int RegFunctionsLoaded = FALSE;
int NWversion=0;
int NWsubVersion=0;
int threadIDIndex=0;
extern "C" int threadIDIndex;

// extern "C" DWORD CvtCBA2GRC(DWORD t);
#define VT ValidateTransman(); //CvtCBA2GRC((time(NULL)/60)^MASTER_SEED); 


/************************************** Prototypes *************************************/

DWORD TransManCopyFile( char *FileServerName, char *Localpath, char *Remotepath, int ToFrom);


void  DeinitTransman(void);
inline void DeinitTransmanArg(int sig)
{
	(void) sig; 	
	DeinitTransman();
}
extern "C" DWORD InitCrit(void);
extern "C" void TransCloseKeys(void);

DWORD LoadRegistryFunctions(void);
DWORD UnloadRegistryFunctions(void);
DWORD StartNTSService(void);
DWORD rtvBeginThread(void (*fun)(void*),void *p,char *name);
void ValidateTransman(void);
void _st(void *nothing);

/***************************************************************************************/
void main (int argc, char *argv[])
{

	TransManMainThread=GetThreadID();
	TransManMainThreadGroup=GetThreadGroupID();
	
	signal( SIGTERM,  DeinitTransmanArg);    

    VT   // initialize
			
	SuspendThread(TransManMainThread);
	exit(0);
}

/*************************************************************************************/
extern "C" int UnloadCheck(void)
{

	return 0;
}

/*************************************************************************************/
void InitTransman(void)
{

	LoadRegistryFunctions();

	InitCrit();   // Open important keys
//	StartNTS(NULL);
//	StartNTSService();	

	inited++;
}
/**************************************************************************************/
void DeinitTransman(void)
{
//	StopNTS();
	TransCloseKeys();
	UnloadRegistryFunctions();  

	inited--;

	if (TransManMainThread) {
		ResumeThread(TransManMainThread);
	}

//	return 0;
}

/************************************************************************************/
void ValidateTransman(void)
{

	if (inited == 0)
		InitTransman();
	
}
/*********************************************************************************************************************/
DWORD LoadRegistryFunctions(void)
{

	if ( (RegOpenKey = (tRegOpenKey)ImportSymbol(GetNLMHandle(),"vpRegOpenKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegCreateKey = (tRegCreateKey)ImportSymbol(GetNLMHandle(),"vpRegCreateKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegDeleteKey = (tRegDeleteKey)ImportSymbol(GetNLMHandle(),"vpRegDeleteKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegDeleteValue = (tRegDeleteValue)ImportSymbol(GetNLMHandle(),"vpRegDeleteValue")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegCloseKey = (tRegCloseKey)ImportSymbol(GetNLMHandle(),"vpRegCloseKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegSetValueEx = (tRegSetValueEx)ImportSymbol(GetNLMHandle(),"vpRegSetValueEx")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegQueryValueEx = (tRegQueryValueEx)ImportSymbol(GetNLMHandle(),"vpRegQueryValueEx")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegEnumValue = (tRegEnumValue)ImportSymbol(GetNLMHandle(),"vpRegEnumValue")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegEnumKeyEx = (tRegEnumKeyEx)ImportSymbol(GetNLMHandle(),"vpRegEnumKeyEx")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegEnumKey = (tRegEnumKey)ImportSymbol(GetNLMHandle(),"vpRegEnumKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegFlushKey = (tRegFlushKey)ImportSymbol(GetNLMHandle(),"vpRegFlushKey")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegNotifyChangeKeyValue = (tRegNotifyChangeKeyValue)ImportSymbol(GetNLMHandle(),"vpRegNotifyChangeKeyValue")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (DuplicateHandle = (tDuplicateHandle)ImportSymbol(GetNLMHandle(),"vpDuplicateHandle")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegOpenKeyEx = (tRegOpenKeyEx)ImportSymbol(GetNLMHandle(),"vpRegOpenKeyEx")) == NULL ) return IDS_ERR_OPEN_REGDB;

	if ( (RegCloseDatabase = (tRegCloseDatabase)ImportSymbol(GetNLMHandle(),"RegCloseDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegOpenDatabase = (tRegOpenDatabase)ImportSymbol(GetNLMHandle(),"RegOpenDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RegSaveDatabase = (tRegSaveDatabase)ImportSymbol(GetNLMHandle(),"RegSaveDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (RawDumpDatabase = (tRawDumpDatabase)ImportSymbol(GetNLMHandle(),"RawDumpDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (DumpFileNum = (tDumpFileNum)ImportSymbol(GetNLMHandle(),"DumpFileNum")) == NULL ) return IDS_ERR_OPEN_REGDB;
	if ( (FileDumpDatabase = (tFileDumpDatabase)ImportSymbol(GetNLMHandle(),"FileDumpDatabase")) == NULL ) return IDS_ERR_OPEN_REGDB;

	RegFunctionsLoaded=TRUE;

	return ERROR_SUCCESS;
}
/**********************************************************************************************/
DWORD UnloadRegistryFunctions(void)
{

	if ( !RegFunctionsLoaded )
		return ERROR_SUCCESS;

	UnimportSymbol(GetNLMHandle(),"vpRegOpenKey");
	UnimportSymbol(GetNLMHandle(),"vpRegCreateKey");
	UnimportSymbol(GetNLMHandle(),"vpRegDeleteKey");
	UnimportSymbol(GetNLMHandle(),"vpRegDeleteValue");
	UnimportSymbol(GetNLMHandle(),"vpRegCloseKey");
	UnimportSymbol(GetNLMHandle(),"vpRegSetValueEx");
	UnimportSymbol(GetNLMHandle(),"vpRegQueryValueEx");
	UnimportSymbol(GetNLMHandle(),"vpRegEnumValue");
	UnimportSymbol(GetNLMHandle(),"vpRegEnumKeyEx");
	UnimportSymbol(GetNLMHandle(),"vpRegEnumKey");
	UnimportSymbol(GetNLMHandle(),"vpRegFlushKey");
	UnimportSymbol(GetNLMHandle(),"vpRegNotifyChangeKeyValue");
	UnimportSymbol(GetNLMHandle(),"vpDuplicateHandle");
	UnimportSymbol(GetNLMHandle(),"vpRegOpenKeyEx");

	UnimportSymbol(GetNLMHandle(),"RegCloseDatabase");
	UnimportSymbol(GetNLMHandle(),"RegOpenDatabase");
	UnimportSymbol(GetNLMHandle(),"RegSaveDatabase");
	UnimportSymbol(GetNLMHandle(),"RawDumpDatabase");
	UnimportSymbol(GetNLMHandle(),"DumpFileNum");
	UnimportSymbol(GetNLMHandle(),"FileDumpDatabase");
	RegOpenKey = NULL;              RegCreateKey = NULL;    RegDeleteKey = NULL;            RegDeleteValue = NULL;  
	RegCloseKey = NULL;             RegSetValueEx = NULL;   RegQueryValueEx = NULL;         RegEnumValue = NULL;    
	RegEnumKeyEx = NULL;    RegEnumKey = NULL;              RegFlushKey = NULL;                     RegNotifyChangeKeyValue = NULL;
	DuplicateHandle = NULL; RegOpenKeyEx = NULL;    RegCloseDatabase = NULL;        RegOpenDatabase = NULL; 
	RegSaveDatabase = NULL; RawDumpDatabase = NULL; DumpFileNum = NULL;                     FileDumpDatabase = NULL;

	RegFunctionsLoaded = FALSE;

	return ERROR_SUCCESS;
}
/**********************************************************************************************/
DWORD StartNTSService(void)
{

	HANDLE han;
	char libpath[IMAX_PATH];

	sssnprintf(libpath,sizeof(libpath),NW_SYSTEM_DIR"\\%s","PDS.NLM");
	han = _LoadLibrary(libpath,TRUE);

	if ( han == NULL )
		return P_NO_VIRUS_ENGINE;

	return 0;
}
/***********************************************************************************************/
DWORD rtvBeginThread(void (*fun)(void*),void *p,char *name)
{
	DWORD ret;
    int size = sizeof(THREADS); 
	THREADS *t;
	
	void *s = malloc(size);;

	t = (THREADS*)s;
	
	if (t)
	{
		t->fun = fun;
		t->p = p;

		//copy name over
		t->name = (char*)malloc(strlen(name) + 1);
		memset(t->name, 0, strlen(name) + 1);
		strncpy(t->name,name,strlen(name) + 1);

		//begin thread
		ret = NTxBeginThread(_st,(void*)t);
//			dprintfTag2( DEBUGTHREADS, "THRD: Thread %s created %08X\n", t->name, ret );
	}
	else
	{
//			dprintfTag1( DEBUGTHREADS, "THRD: Thread creation %s failed because no memory\n", name );
		ret = MINUS_ONE;
	}
	return ret;
}
/************************************************************************************/
BOOL GetSid(PSID pSid)
{

	pSid->ConnectionID = 0;
	StrCopy(pSid->UserName,"NetWare Server");

	return TRUE;
}

/****************************************************************************/
extern "C" void StartBox(PEVENTBLOCK eb)
{
	if ( eb->Special )
		_printf("%s%c\r\n\r\n",(char *)eb->Special,_GetVal(eb->hKey,"Beep",1)?'\a':' ');
}



/*********************************************************************/
BOOL CopyFile(LPCTSTR ExistingFile,LPCTSTR NewFile,BOOL FailIfExists)
{

	int sHan,dHan;
	int ret=FALSE;

	sHan = open(ExistingFile,O_BINARY|O_RDONLY,0);
	if ( sHan != INVALID_HANDLE_VALUE )
	{
		dHan = open(NewFile,O_WRONLY|O_TRUNC|O_BINARY|(FailIfExists?O_EXCL:0)|O_CREAT,S_IREAD|S_IWRITE);
		if ( dHan != INVALID_HANDLE_VALUE )
		{
			LONG size = lseek(sHan,0,SEEK_END);
			LONG out=0;
			lseek(sHan,0,SEEK_SET);
			ret = FileServerFileCopy(sHan,dHan,0,0,size,&out);
			if ( !ret )
				ret = TRUE;
			close(dHan);
		}
		close(sHan);
	}

	return(BOOL)ret;
}

/************************************************************************************************/
void _st(void *nothing)
{


	LONG    origTaskID,     newTaskID;
	THREADS *t = (THREADS *)nothing;
	void *p = t->p;
	void (*fun)(void *) = t->fun;

	char tName[64];
	strcpy(tName,t->name);

	RenameThread(GetThreadID(),t->name);

//all scan threads start with this label. All threads start here, we only want the scan threads.
//there will be ten of them.

	if(!strncmp("RTV Checker",t->name,11))
	{
		scanThreadIDArray[threadIDIndex]=GetThreadID();
		threadIDIndex++;
		ConsolePrintf("threadIDIndex= %d \r", threadIDIndex);
	}

	free(t);

	ThreadsInUse++;
//		dprintf ("Thread %s starting\n",tName);
	newTaskID = AllocateBlockOfTasks(1);
	origTaskID = SetCurrentTask(newTaskID);

#ifdef ThreadCheck
	ThreadStarting(tName,newTaskID);
#endif // ThreadCheck

	fun(p);

#ifdef ThreadCheck
	ThreadEnding(newTaskID);
#endif // ThreadCheck

	SetCurrentTask(origTaskID);
	ReturnBlockOfTasks(newTaskID, 1);
//		dprintf ("Thread %s terminating\n",tName);
	ThreadsInUse--;
}
/************************************************************************************************/

