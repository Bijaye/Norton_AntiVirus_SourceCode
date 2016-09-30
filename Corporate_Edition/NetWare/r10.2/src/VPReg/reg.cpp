// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "REGISTRY.H"
#include "SymSaferStrings.h"

#ifndef LINUX
	#include "nlm/nwfinfo.h"
   #include "winThreads.h"
#endif

#define RAW_REG_DUMP "%s" sSLASH "vp_Raw%02d.txt"
#define REG_DUMP "%s" sSLASH "vp_Reg%02d.txt"
#define REG_CHECK "%s" sSLASH "vp_Chk%02d.txt"

#ifdef LINUX
	#include <RegistrySemaphores.h>
	#define ACCESSCHECK R_OK
#else
	#define ACCESSCHECK 0
#endif

#undef LONG
#define LONG long

#ifdef LINUX	
	#undef  NTxSemaOpen
	#define NTxSemaOpen(     lValue,x)        ipcOpenSemaphore(   0,lValue)
	#undef  NTxSemaClose
	#define NTxSemaClose(    hSemaphore)      ipcCloseSemaphore(  hSemaphore)
	#undef  NTxSemaExamine
	#define NTxSemaExamine(  hSemaphore)      ipcExamineSemaphore(hSemaphore)
	#undef  NTxSemaSignal
	#define NTxSemaSignal(   hSemaphore)      ipcReleaseSemaphore(hSemaphore)
	#undef  NTxSemaWait
	#define NTxSemaWait(     hSemaphore)      ipcWaitForSemaphore(hSemaphore,INFINITE)
	#undef  NTxSemaTimedWait
	#define NTxSemaTimedWait(hSemaphore,msec) ipcWaitForSemaphore(hSemaphore,msec)
#endif

void DumpSubKey(HKEY hkey);  // gdf CW conversion

extern char gVPRegDebug     = 0;
char RegistryRunning = 1;
extern "C" int  RegTGID         = 0;

PKEY   RootKey                   = NULL;
PVALUE RootValue                 = NULL;
PDATA  RootData                  = NULL;
LPSTR  *StringTable              = NULL;
DWORD  maxkey                    = 0;
DWORD  maxvalue                  = 0;
DWORD  maxdata                   = 0;
DWORD  maxstring                 = 0;

DWORD  errval                    = ERROR_SUCCESS;

int    DumpFileNum               = 0;
int    KeyDumpNum                = 0;
int    InReg                     = 0;
BOOL   Blocked                   = 0;
void*  BlockingThread            = NULL;
int    NumBlocked                = 0;
int    Times                     = 0;
BOOL   DataBaseDirty             = FALSE;
extern "C" BOOL vpRegChanged     = TRUE;

FILE   *fp                       = NULL; // File pointer for the database dump file
char   indent[256]               = "";   // indent buffer for db dump
int    K                         = 0;
int    L                         = 0;    // used in db dump for indent(K) and Key path
// length(L) 
char   kPath[KEYPATHLEN][KEYLEN] = {0};  // Array for open key path names

#ifdef NLM
extern "C" struct {
#else
struct {
#endif
	DWORD dwVersion;
	
	DWORD dwSaveGoodDatabase;
	DWORD dwRestoreGoodDatabase;
	DWORD dwRegSaveDatabaseNoSaveNeeded;
	DWORD dwRegSaveDatabase;
	
	DWORD dwTablesMissingInMemory;
	
	DWORD dwLastReadErrno;
	DWORD dwLastReadNetWareErrno;
	DWORD dwReadErrors;
	
	DWORD dwLastCopyWriteErrno;
	DWORD dwLastCopyWriteNetWareErrno;
	DWORD dwCopyWriteErrors;
	DWORD dwCopyWriteDiskFullErrors;
	
	DWORD dwLastWriteErrno;
	DWORD dwLastWriteNetWareErrno;
	DWORD dwWriteErrors;
	DWORD dwWriteDiskFullErrors;
	
	DWORD dwLastOpenTmpErrno;
	DWORD dwLastOpenTmpNetWareErrno;
	DWORD dwOpenTmpErrors;
	
	DWORD dwLastOpenDatErrno;
	DWORD dwLastOpenDatNetWareErrno;
	DWORD dwOpenDatErrors;
	
	DWORD dwLastOpenSavErrno;
	DWORD dwLastOpenSavNetWareErrno;
	DWORD dwOpenSavErrors;
	
	DWORD dwWaitForBlock;
	DWORD dwWaitTillRegClear;
	DWORD dwEnter;
	DWORD dwReturn;
	
	DWORD dwLoadRootsBegin;
	DWORD dwLoadRootsEnd;
	DWORD dwLoadRootsFailed;
	
	DWORD dwInitArrays;
	
	DWORD dwRegSaveDatabaseErrors;
	
	// these are all serious internal errors - we should never see these!
	// these should all be last, next to the baadfood in dwLastCounter
	
	// as it turns out, we get the last 2 all the time, but they are still not
	// a good error. what happens is that the second one clears the lock before
	// the first lock expects to be cleared. This needs more investigation - they
	// should probably be changed to a counted lock.
	
	DWORD dwCrashInLoadRoots;
	DWORD dwDoubleBlock_TroubleFollows;
	DWORD dwDoubleInReg_TroubleFollows;
	
	DWORD dwLastCounter;          // this is just a marker for dumps - it contains
	                              // 0xbaadf00d 
	
} VPRegErrorCounters = { 1, 0 }; // update the version number (the first number) if
                                 // you add counters! 

#define CHANGE_REG vpRegChanged=TRUE
#define DIRTY DataBaseDirty=TRUE

#ifdef LINUX
	#define INT_CAST
	#define VOID_STAR_CAST
#else
	#define INT_CAST       (int)
	#define VOID_STAR_CAST (void*)
#endif

#define WAIT_FOR_BLOCK	while (Blocked && RegistryRunning) {if (NTxGetCurrentThreadId()== INT_CAST BlockingThread) {VPRegErrorCounters.dwDoubleBlock_TroubleFollows += 1; break;} VPRegErrorCounters.dwWaitForBlock += 1; ThreadSwitchWithDelay(); }

#define ENTER {WAIT_FOR_BLOCK; VPRegErrorCounters.dwEnter += 1; InReg++;}
#define RETURN(x) {InReg--; VPRegErrorCounters.dwReturn += 1; return x;}

#define SET_BLOCK Blocked=TRUE; BlockingThread= VOID_STAR_CAST NTxGetCurrentThreadId(); if (VPRegErrorCounters.dwLoadRootsBegin != VPRegErrorCounters.dwLoadRootsEnd) VPRegErrorCounters.dwCrashInLoadRoots += 1;
#define WAIT_TILL_REG_CLEAR while ((InReg-NumBlocked)>0 && RegistryRunning) {if (NTxGetCurrentThreadId()== INT_CAST BlockingThread) {VPRegErrorCounters.dwDoubleInReg_TroubleFollows += 1; break;} VPRegErrorCounters.dwWaitTillRegClear += 1; ThreadSwitchWithDelay(); }

// Add something to tell which thread has blocked so we don't get deadlocked waiting for ourself
#define BLOCK {NumBlocked++; WAIT_FOR_BLOCK; SET_BLOCK; WAIT_TILL_REG_CLEAR;  NumBlocked--; SET_BLOCK;}
#define UNBLOCK {Blocked=FALSE; BlockingThread=NULL;}

#define TA(x)	if (!(x)) {RingTheBell(); x;}

static void SignalChange(HKEY hkey);

static void CleanupDelWhenClose();
static void CleanupKey (int i);
static void CleanupValue (int i);
static void CleanupData (int i);
static void CleanupString (int i);

static BOOL DoDump(char *Dir,size_t DirBytes);

void FixupData();

/***************************************************************************************/
void CleanupDelWhenClose()
{
	int i;

	for (i=0 ; i<maxkey ; i++) {
		if(RootKey[i].delwhenclose)
			CleanupKey(i);
		ThreadSwitchWithDelay();
	}
}
/***************************************************************************************/
void CleanupKey (int i)
{
	if(!i) return;
	if(!RootKey[i].used)
		return;

	if(!RootKey[i].delwhenclose)
		return;

	if (RootKey[i].sema) {
		SignalChange(i);
	}
	RootKey[i].used=0;

	CleanupString(RootKey[i].name);
	CleanupValue(RootKey[i].value);
	CleanupKey(RootKey[i].sibling);
	CleanupKey(RootKey[i].child);

	memset(&RootKey[i],0,sizeof(KEY));
}
/****************************************************************************************/
void CleanupValue (int i)
{
	if(!i) return;
	if(!RootValue[i].used)
		return;

	RootValue[i].used=0;

	CleanupString(RootValue[i].name);
	CleanupValue(RootValue[i].value);
	CleanupData(RootValue[i].data);

	memset(&RootValue[i],0,sizeof(VALUE));
}
/****************************************************************************************/
void CleanupData (int i)
{
	if(!i) return;
	if(!RootData[i].used)
		return;

	RootData[i].used=0;

	CleanupData(RootData[i].link);

	memset(&RootData[i],0,sizeof(DATA));
}
/****************************************************************************************/
void CleanupString (int i)
{
	if(!i) return;
	if(!StringTable[i]) return;

	free(StringTable[i]);
	StringTable[i] = NULL;
}

// the only error returned is -1, internally I count actual errors and save the last one
// otherwise I return the number of bytes written

LONG CheckedWrite( int handle, void *buffer, LONG len)
{
    LONG nRet = write( handle, buffer, len );

    if (nRet == -1)
    {
        VPRegErrorCounters.dwWriteErrors += 1;
        VPRegErrorCounters.dwLastWriteErrno = errno;
        VPRegErrorCounters.dwLastWriteNetWareErrno = NetWareErrno;
    }
    else if (nRet < len)
    {
        VPRegErrorCounters.dwWriteDiskFullErrors += 1;
        nRet = -1;          // flag error
    }

    return nRet;
}


/***************************************************************************************/
DWORD RegSaveDatabase(void) {

        DWORD i,numStrings=0,dwRet=ERROR_NO_DATABASE;
	WORD len;
	int FileHandle;
	int TGID;

//dprintf ("s\n");

	if (DataBaseDirty == FALSE)
    {
        VPRegErrorCounters.dwRegSaveDatabaseNoSaveNeeded += 1;
		return 0xffffffff;
    }

    VPRegErrorCounters.dwRegSaveDatabase += 1;

    	dprintf ("save database block\n");

ENTER;

	if (!RootKey || !RootValue || !RootData || !StringTable)
    {
        VPRegErrorCounters.dwTablesMissingInMemory += 1;

		RETURN (ERROR_NO_MEMORY);
    }

BLOCK;
	TGID=SetThreadGroupID (RegTGID);
	PurgeFile( REGISTRY_LOCATION"VPREGDB.BAK" );
	rename( REGISTRY_LOCATION"VPREGDB.DAT", REGISTRY_LOCATION"VPREGDB.BAK" );
	FileHandle = open( REGISTRY_LOCATION"VPREGDB.TMP", O_CREAT|O_BINARY|O_TRUNC|O_RDWR, S_IREAD|S_IWRITE );

	if (FileHandle==-1) 
    {
        VPRegErrorCounters.dwOpenTmpErrors += 1;
        VPRegErrorCounters.dwLastOpenTmpErrno = errno;
        VPRegErrorCounters.dwLastOpenTmpNetWareErrno = NetWareErrno;

		goto Done;
    }

	ThreadSwitch();

	CleanupDelWhenClose();

	for (i=0;i<maxkey;i++)
		RootKey[i].dirty = 0;
	for (i=0;i<maxvalue;i++)
		RootValue[i].dirty = 0;
	for (i=1;i<maxdata;i++)
		RootData[i].dirty = 0;

	lseek(FileHandle,0,SEEK_SET);

//	dprintf ("save database block\n");

	i = maxkey*sizeof(KEY);
	if(CheckedWrite(FileHandle,&i,sizeof(i))==-1) goto Done;
	if(CheckedWrite(FileHandle,RootKey,i)==-1) goto Done;

	i = maxvalue*sizeof(VALUE);
	if(CheckedWrite(FileHandle,&i,sizeof(i))==-1) goto Done;
	if(CheckedWrite(FileHandle,RootValue,i)==-1) goto Done;

	i = maxdata*sizeof(DATA);
	if(CheckedWrite(FileHandle,&i,sizeof(i))==-1) goto Done;
	if(CheckedWrite(FileHandle,RootData,i)==-1) goto Done;

	for (i=0;i<maxstring;i++)
		if (StringTable[i])
			numStrings++;
	if(CheckedWrite(FileHandle,&numStrings,sizeof(numStrings))==-1) goto Done;

	for (i=0;i<maxstring;i++) {
		ThreadSwitch();
		if (StringTable[i]) {
			if(CheckedWrite(FileHandle,&i,sizeof(i))==-1) goto Done;
			len = strlen(StringTable[i])+1;
			if(CheckedWrite(FileHandle,&len,sizeof(len))==-1) goto Done;
			if(CheckedWrite(FileHandle,StringTable[i],len)==-1) goto Done;
		}
	}

	close (FileHandle);
        FileHandle = -1;
	
	rename( REGISTRY_LOCATION"VPREGDB.TMP", REGISTRY_LOCATION"VPREGDB.DAT" );
	PurgeFile(REGISTRY_LOCATION"VPREGDB.TMP" );

	DataBaseDirty = FALSE;

	dwRet=ERROR_SUCCESS;

Done:
    if (FileHandle != -1)
    {
        close (FileHandle );        // not leaking handles is a good idea
    }

	SetThreadGroupID (TGID);

	if(dwRet!=ERROR_SUCCESS) 
        {
            VPRegErrorCounters.dwRegSaveDatabaseErrors += 1;
	    rename( REGISTRY_LOCATION"VPREGDB.BAK", REGISTRY_LOCATION"VPREGDB.DAT" );
        }

UNBLOCK;
	RETURN(dwRet);
}


/****************************************************************************************/
static HSTR MakeStr(LPCSTR name) {

	DWORD i;
	LPSTR *NewStringTable;
	int TGID;

//dprintf ("make str block\n");
BLOCK;
	for (i=1;i<maxstring;i++)
		if (StringTable[i]==NULL) {
			TGID=SetThreadGroupID (RegTGID);
TA(			StringTable[i] = (char*) malloc(strlen(name)+1));
			SetThreadGroupID (TGID);

			if (StringTable[i]==NULL) {
				UNBLOCK;
				return 0;
			}
			memset (StringTable[i],0,strlen(name)+1);
			strcpy(StringTable[i],name);
			UNBLOCK;
			return (HSTR)i;
		}

	TGID=SetThreadGroupID (RegTGID);
TA(	NewStringTable = (char**) realloc(StringTable,(maxstring+64)*sizeof(LPSTR)));		//<<<<<<<<<< possible sync problem. use malloc?
	SetThreadGroupID (TGID);
	if (NewStringTable) {
		StringTable = NewStringTable;
		memset(&StringTable[maxstring],0,64*sizeof(LPSTR));
		TGID=SetThreadGroupID (RegTGID);
TA(		StringTable[maxstring] = (char*) malloc(strlen(name)+1));
		SetThreadGroupID (TGID);
		if (StringTable[maxstring]==NULL) {
			UNBLOCK;
			return 0;
		}
		memset (StringTable[i],0,strlen(name)+1);
		strcpy(StringTable[maxstring],name);
		maxstring += 64;
		UNBLOCK;
		return (HSTR)(maxstring-64);
	}
UNBLOCK;
	return 0;
}
/****************************************************************************************/
LPSTR GetStrn(HSTR str) {

	if (!str || str >= maxstring)
		return NULL;

	if (!StringTable[str])
		return NULL;

	return StringTable[str];
}

// copy a file from one handle to another, checking and counting errors

// returns ERROR_SUCESS or -1

DWORD CopyDatabase( int ToFile, int FromFile )
{
	BYTE    buf[1000];
    int     nRet = -1;
	int     i;

    do 
    {
        i = read(FromFile,buf,sizeof(buf));
        if (i > 0)  // some bytes read, no error
        {
            nRet=write(ToFile,buf,i);
            if (nRet == -1)
            {
                VPRegErrorCounters.dwWriteErrors += 1;
                VPRegErrorCounters.dwLastWriteErrno = errno;
                VPRegErrorCounters.dwLastWriteNetWareErrno = NetWareErrno;
            }
            else if (nRet < i)
            {
                VPRegErrorCounters.dwWriteDiskFullErrors += 1;
                nRet = -1;          // flag error so I stop
            }
        }
        else if (i == -1)   // error on read
        {
            VPRegErrorCounters.dwReadErrors += 1;
            VPRegErrorCounters.dwLastReadErrno = errno;
            VPRegErrorCounters.dwLastReadNetWareErrno = NetWareErrno;

            nRet = i;
        }
    } while (nRet != -1 && !eof(FromFile));
    
    if(nRet != -1)
    {
        nRet = ERROR_SUCCESS;
    }

    return (DWORD) nRet;
}



/****************************************************************************************/
DWORD SaveGoodDatabase(void) { // only called from LoadRoots so no need to synchronize

	int good,copy;
	DWORD ret = 0xffffffff;

    VPRegErrorCounters.dwSaveGoodDatabase += 1;

	PurgeFile( REGISTRY_LOCATION"VPREGDB.TMP" );
	copy = open( REGISTRY_LOCATION"VPREGDB.TMP", O_TRUNC|O_CREAT|O_BINARY|O_TRUNC|O_WRONLY, S_IREAD|S_IWRITE );
	if (copy != -1) 
	{
	    good = open( REGISTRY_LOCATION"VPREGDB.DAT", O_BINARY|O_RDONLY,0 );
	    if (good != -1) 
	    {
                ret = CopyDatabase( copy, good );
		close(good);
	    }
        else
        {
            VPRegErrorCounters.dwOpenDatErrors += 1;
            VPRegErrorCounters.dwLastOpenDatErrno = errno;
            VPRegErrorCounters.dwLastOpenDatNetWareErrno = NetWareErrno;
        }
		close(copy);
	}
    else
    {
        VPRegErrorCounters.dwOpenTmpErrors += 1;
        VPRegErrorCounters.dwLastOpenTmpErrno = errno;
        VPRegErrorCounters.dwLastOpenTmpNetWareErrno = NetWareErrno;
    }

	if(ret==ERROR_SUCCESS) 
	{
		PurgeFile( REGISTRY_LOCATION"VPREGDB.SAV" );
		rename( REGISTRY_LOCATION"VPREGDB.TMP", REGISTRY_LOCATION"VPREGDB.SAV" );
	}
	PurgeFile( REGISTRY_LOCATION"VPREGDB.TMP" );

	return ret;
}


/****************************************************************************************/
DWORD RestoreGoodDatabase(void) { // only called from LoadRoots so no need to synchronize

	int good,copy;
	DWORD ret = 0xffffffff;
	DWORD i;
	char BadDB[16];
//	int TGID;

	dprintf("Old database corrupted.  Restoring backup.\n");

    VPRegErrorCounters.dwRestoreGoodDatabase += 1;

	for (i=0 ; i<0xfff ; i++) {
		sssnprintf( BadDB, sizeof(BadDB), "%sVPREGDB.%03x", REGISTRY_LOCATION, i );
		ThreadSwitchWithDelay();
		if (access (BadDB,0)) break;
	}

	PurgeFile( REGISTRY_LOCATION"VPREGDB.BAD" );
	rename( REGISTRY_LOCATION"VPREGDB.DAT", BadDB );

	copy = open( REGISTRY_LOCATION"VPREGDB.TMP", O_TRUNC|O_CREAT|O_BINARY|O_TRUNC|O_WRONLY, S_IREAD|S_IWRITE );
	if (copy != -1) 
	{
		good = open( REGISTRY_LOCATION"VPREGDB.SAV", O_BINARY|O_RDONLY, 0 );
		if (good != -1) 
		{
            ret = CopyDatabase( copy, good );
			close(good);
		}
        else
        {
            VPRegErrorCounters.dwOpenSavErrors += 1;
            VPRegErrorCounters.dwLastOpenSavErrno = errno;
            VPRegErrorCounters.dwLastOpenSavNetWareErrno = NetWareErrno;
        }
		close(copy);
	}
    else
    {
        VPRegErrorCounters.dwOpenTmpErrors += 1;
        VPRegErrorCounters.dwLastOpenTmpErrno = errno;
        VPRegErrorCounters.dwLastOpenTmpNetWareErrno = NetWareErrno;
    }

	if(ret==ERROR_SUCCESS)
	{
		PurgeFile( REGISTRY_LOCATION"VPREGDB.DAT" );
		rename( REGISTRY_LOCATION"VPREGDB.TMP", REGISTRY_LOCATION"VPREGDB.DAT" );
	}
	PurgeFile( REGISTRY_LOCATION"VPREGDB.TMP" );

	return ret;
}
/****************************************************************************************/
DWORD ClearArrays(void) {

	int i;
	int TGID=SetThreadGroupID (RegTGID);

	if (RootKey) {
		free(RootKey);
		RootKey=NULL;
	}

	if (RootValue) {
		free(RootValue);
		RootValue=NULL;
	}

	if (RootData) {
		free(RootData);
		RootData=NULL;
	}

	if (StringTable) {
		for (i=0;i<maxstring;i++)
			if (StringTable[i]) {
				free(StringTable[i]);
				StringTable[i] = NULL;
			}
		free(StringTable);
		StringTable=NULL;
	}
	SetThreadGroupID (TGID);

	maxkey=0;
	maxvalue=0;
	maxdata=0;
	maxstring=0;

	return 1;
}
/*********************************************************************************/
DWORD InitArrays(void) { // only called from LoadRoots so no need to synchronize

	int TGID;

    VPRegErrorCounters.dwInitArrays += 1;

	ClearArrays();

	maxkey=32;
	maxvalue=32;
	maxdata=32;
	maxstring=64;

	TGID=SetThreadGroupID (RegTGID);
	RootKey = (KEY*) malloc(sizeof(KEY)*maxkey);

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
	
	if( !RootKey )
		return ERROR_NO_MEMORY;	
		
	// --------------------------------------

	RootValue = (VALUE*) malloc(sizeof(VALUE)*maxvalue);

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
	
	if( !RootValue )   
	{  
		if( RootKey )   
		{   
			free( RootKey );  
			RootKey = NULL;  
		}  
		return ERROR_NO_MEMORY;
	}  
	
	// --------------------------------------

	RootData = (DATA*) malloc(sizeof(DATA)*maxdata);

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
	
	if( !RootData )   
	{  
		if( RootValue )   
		{   
			free( RootValue );  
			RootValue = NULL;  
		} 
		 
		if( RootKey )   
		{   
			free( RootKey );  
			RootKey = NULL;  
		}  
		return ERROR_NO_MEMORY;
	}  
	
	// --------------------------------------
	
	StringTable = (char**) malloc(sizeof(LPSTR)*maxstring);


	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
	
	if( !StringTable )   
	{  
		if( RootData )   
		{   
			free( RootData );  
			RootData = NULL;  
		} 
		 
		if( RootValue )   
		{   
			free( RootValue );  
			RootValue = NULL;  
		} 
		 
		if( RootKey )   
		{   
			free( RootKey );  
			RootKey = NULL;  
		}  

		SetThreadGroupID( TGID );

		return ERROR_NO_MEMORY;
	}  
	
	// --------------------------------------

	// -----------------------------
	//  ksr - Certification, 7/21/2002
	/*
	if (!RootKey || !RootValue || !RootData || !StringTable) {

		if (RootKey) {
			free(RootKey);
			RootKey=NULL;
		}

		if (RootValue) {
			free(RootValue);
			RootValue=NULL;
		}
		if (RootData) {
			free(RootData);
			RootData=NULL;
		}

		if (StringTable) {
			free(StringTable);
			StringTable=NULL;
		}
		SetThreadGroupID (TGID);
		return 1;
	}
	*/
	// -----------------------------

	SetThreadGroupID (TGID);

	memset(RootKey,0,sizeof(KEY)*maxkey);
	memset(RootValue,0,sizeof(VALUE)*maxvalue);
	memset(RootData,0,sizeof(DATA)*maxdata);
	memset(StringTable,0,sizeof(LPSTR)*maxstring);

	DumpFileNum=0;
	KeyDumpNum=1;
	BlockingThread=NULL;
	NumBlocked=0;
	Blocked=0;

//	dprintf ("Root Arrays Initialized\n");

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
	return ERROR_SUCCESS;	
}

/****************************************************************************************/
	
// --------------------------------------
// ksr - NetWare Certification, 8/30/2002

int MakeEmpty(void) 
{
	memset(RootKey,0,sizeof(KEY)*maxkey);
	memset(RootValue,0,sizeof(VALUE)*maxvalue);
	memset(RootData,0,sizeof(DATA)*maxdata);
	memset(StringTable,0,sizeof(LPSTR)*maxstring);

	if (!RootKey[ROOT_KEY_LOC].used) 
	{
		RootKey[ROOT_KEY_LOC].name = MakeStr("LOCAL MACHINE");
		if (RootKey[ROOT_KEY_LOC].name)
			RootKey[ROOT_KEY_LOC].used = 1;
		else	
			return ERROR_NO_MEMORY;
	}
	return ERROR_SUCCESS;	
}

// --------------------------------------

/****************************************************************************************/
DWORD LoadRoots(void) { // synchronized in RegOpenDatabase and IsKeyValid

	// -----------------------
	// ksr - Certification, 7/21/2002
	
	//	PVALUE NewRootValue; 
	//	PKEY NewRootKey;
	//	PDATA NewRootData;
	//	LPSTR *NewStringTable;
	
	PVALUE NewRootValue = 0;  
	PKEY NewRootKey = 0;       
	PDATA NewRootData = 0; 
	LPSTR *NewStringTable = 0;  
	    
	// -----------------------

	int i,j,numStrings;
	DWORD dLen;
	WORD wLen;
	char tries=0;
	int FileHandle=-1;
	int TGID;

    VPRegErrorCounters.dwLoadRootsBegin += 1;
    VPRegErrorCounters.dwLastCounter = 0xbaadf00d;

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002

	if ( InitArrays() != ERROR_SUCCESS )
		
	// --------------------------------------
    {
        VPRegErrorCounters.dwLoadRootsFailed += 1;
        VPRegErrorCounters.dwLoadRootsEnd += 1;
		return ERROR_NO_MEMORY;
    }

	TGID=SetThreadGroupID (RegTGID);


	if( access( REGISTRY_LOCATION"VPREGDB.DAT", ACCESSCHECK ) ) 
	{
#ifdef NLM
	  MakeEmpty();
#endif
	  SetThreadGroupID (TGID);
	  VPRegErrorCounters.dwLoadRootsFailed += 1;
	  VPRegErrorCounters.dwLoadRootsEnd += 1;
	  return ERROR_EMPTY;
	}

top:
	FileHandle = open( REGISTRY_LOCATION"VPREGDB.DAT" ,O_BINARY|O_RDONLY, 0 );
	if( FileHandle == -1 ) 
	{
	  if( RestoreGoodDatabase() == 0 ) 
	  {
	    FileHandle = open( REGISTRY_LOCATION"VPREGDB.DAT", O_BINARY|O_RDONLY , 0 );
	    if (FileHandle==-1) 
	    {
	      SetThreadGroupID (TGID);
	      MakeEmpty();
	      VPRegErrorCounters.dwLoadRootsFailed += 1;
	      VPRegErrorCounters.dwLoadRootsEnd += 1;
	      return ERROR_EMPTY;
	    }
	  }
	  else
	  {
	    MakeEmpty();
	    SetThreadGroupID (TGID);
            VPRegErrorCounters.dwLoadRootsFailed += 1;
            VPRegErrorCounters.dwLoadRootsEnd += 1;
	    return ERROR_EMPTY;
	  }
	}

	dprintf ("Database File Opened\n");
	dprintf ("*********************************************************\n");

	i = read(FileHandle,&dLen,sizeof(dLen));


	if (i != sizeof(dLen))
		goto error;
	dprintf ("Keys:%d\t",dLen/sizeof(KEY));

	if (dLen/sizeof(KEY) > MAXALLOWEDKEYS)
		goto error;

	if (dLen > maxkey*sizeof(KEY)) {
		NewRootKey = (KEY*) realloc(RootKey,dLen);
		if (!NewRootKey) {
			close (FileHandle);
			SetThreadGroupID (TGID);
            VPRegErrorCounters.dwLoadRootsFailed += 1;
            VPRegErrorCounters.dwLoadRootsEnd += 1;

			// ksr - Certification, 7/21/2002
			// free memory Initarrays() if the realloc() fails.
			ClearArrays();          
			
			return ERROR_NO_MEMORY;
		}
		RootKey = NewRootKey;
		maxkey = dLen / sizeof(KEY);
	}

	i = read(FileHandle,RootKey,dLen);
	if (i != dLen)
		goto error;
	dprintf ("Keys Read\n");
	dprintf ("*********************************************************\n");

	for (i=0;i<maxkey;i++)
		RootKey[i].sema=0;

	i = read(FileHandle,&dLen,sizeof(dLen));
	dprintf ("Values:%d\t",dLen/sizeof(VALUE));

	if (i != sizeof(dLen))
		goto error;

	if (dLen/sizeof(VALUE) > MAXALLOWEDVALUES)
		goto error;

	if (dLen > maxvalue*sizeof(VALUE)) {
		NewRootValue = (VALUE*) realloc(RootValue,dLen);
		if (!NewRootValue) {
			close (FileHandle);
			SetThreadGroupID (TGID);
            VPRegErrorCounters.dwLoadRootsFailed += 1;
            VPRegErrorCounters.dwLoadRootsEnd += 1;
            
			// -------------------------------
			// ksr - Certification, 7/21/2002
			// free memory Initarrays() 
			free( RootValue );           
			free( RootData );            
			free( StringTable );          
			
			// free NewRootKey if it was allocated.
			if( NewRootKey )  
			{  
				free( NewRootKey );   
			}   
			// -------------------------------

			return ERROR_NO_MEMORY;
		}
		RootValue = NewRootValue;
		maxvalue = dLen / sizeof(VALUE);
	}

	i = read(FileHandle,RootValue,dLen);

	if (i != dLen)
		goto error;
	dprintf ("Values Read\n");
	dprintf ("*********************************************************\n");

	i = read(FileHandle,&dLen,sizeof(dLen));
	dprintf ("Data Items:%d\t",dLen/sizeof(DATA));

	if (i != sizeof(dLen))
		goto error;

	if (dLen/sizeof(DATA) > MAXALLOWEDDATA)
		goto error;

	if (dLen > maxdata*sizeof(DATA)) {
		NewRootData = (DATA*) realloc(RootData,dLen);
		if (!NewRootData) {
			close (FileHandle);
			SetThreadGroupID (TGID);
            VPRegErrorCounters.dwLoadRootsFailed += 1;
            VPRegErrorCounters.dwLoadRootsEnd += 1;
            
			// -------------------------------------
			// ksr - Certification, 7/21/2002
			// free Initarrays() 
  			free( RootData );            
			free( StringTable );          

 			// free NewRootKey if it was allocated.
			if( NewRootKey )   
			{   
				free( NewRootKey ); 
			}   

			// free NewRootValue if it was allocated.
			if( NewRootValue )   
			{   
				free( NewRootValue );   
			}   
			// --------------------------------------
          
			return ERROR_NO_MEMORY;
		}
		RootData = NewRootData;
		maxdata = dLen / sizeof(DATA);
	}
	i = read(FileHandle,RootData,dLen);
	if (i != dLen)
		goto error;
	dprintf ("Data Read\n");
	dprintf ("*********************************************************\n");

	dprintf ("Begin Reading Strings\n");

	i = read(FileHandle,&numStrings,sizeof(numStrings));
	dprintf ("i:%d  ns:%d\n",i,numStrings);

//	if (i!=sizeof(numStrings)){  // gdf 03/22/2003 Fix 1-YVE7V
	if (i!=sizeof(numStrings)|| (numStrings == 0) ){    // gdf 03/22/2003 Fix 1-YVE7V	
			dprintf ("numStrings Error\n");
			goto error;
		}

	for (j=0;j<numStrings;j++) 
	{
		// ksr - Certification, 7/21/2002
		int jj;  

		i = read(FileHandle,&dLen,sizeof(dLen));
		dprintf ("%d\t",dLen);
		if (eof(FileHandle)) {
			dprintf ("eof error\n");
			goto error;
		}
		if (i != sizeof(dLen)) {
			dprintf ("dLen Size error\n");
			goto error;
		}

		if (dLen > MAXALLOWEDSTRINGS){
			dprintf ("MaxString Error\n");
			goto error;
		}

		while (dLen >= maxstring) 
		{
			// ksr - Certification, 7/21/2002
			int Curallocstrings = dLen;
			    
			NewStringTable = (char**) realloc(StringTable,(maxstring+64)*sizeof(LPSTR));
			if (!NewStringTable) {
				dprintf ("Memory Error\n");
				close (FileHandle);
				SetThreadGroupID (TGID);
                VPRegErrorCounters.dwLoadRootsFailed += 1;
                VPRegErrorCounters.dwLoadRootsEnd += 1;
            	
            	// -------------------------------------
				// ksr - Certification, 7/21/2002
				
				// free StringTable 
				for( jj=1; jj < Curallocstrings; jj++ )       
					if (StringTable[jj] )         
						free( StringTable[jj] );  
				
                free( StringTable ); 
 				// free NewRootKey if allocated.
				if( NewRootKey )  
				{   
					free( NewRootKey );   
				}   

				// free NewRootValue if allocated.
				if( NewRootValue )   
				{  
					free(NewRootValue);   
				}   

				// free NewRootData if allocated.
				if( NewRootData )   
				{  
					free(NewRootData);   
				}  
            	// -------------------------------------

				return ERROR_NO_MEMORY;
			}
			StringTable = NewStringTable;
			memset(&StringTable[maxstring],0,64*sizeof(LPSTR));
			maxstring += 64;
		}
		i = read(FileHandle,&wLen,sizeof(wLen));
		if (i != sizeof(wLen)){
			dprintf ("wLen Size error\n");
			goto error;
		}

		if (wLen > MAXALLOWEDSTRINGSIZE){
			dprintf ("String Size error\n");
			goto error;
		}

		StringTable[dLen] = (char*) malloc(wLen);
		if (!StringTable[dLen]) {
			dprintf ("Memory Error\n");
			close (FileHandle);
			SetThreadGroupID (TGID);
            VPRegErrorCounters.dwLoadRootsFailed += 1;
            VPRegErrorCounters.dwLoadRootsEnd += 1;
            
            // -------------------------------------
			// ksr - Certification, 7/21/2002
			
			// free StringTable memory 
			for( jj=1; jj < dLen; jj++ )       
				if( StringTable[jj] )         
					free( StringTable[jj] );  

			// free Initarrays() if malloc() fails.
			ClearArrays();         
            // -------------------------------------

			return ERROR_NO_MEMORY;
		}
		i = read(FileHandle,StringTable[dLen],wLen);
		dprintf ("%s\t",StringTable[dLen]);
		if (i != wLen){
			dprintf ("wLen2 Size error\n");
			goto error;
		}
	}

	dprintf ("\nStrings Read:%d\n",j);
	dprintf ("*********************************************************\n");
	if (!RootKey[ROOT_KEY_LOC].used) {
		RootKey[ROOT_KEY_LOC].name = MakeStr("LOCAL MACHINE");
		if (RootKey[ROOT_KEY_LOC].name)
			RootKey[ROOT_KEY_LOC].used = 1;
			
		// --------------------------------------
		// ksr - NetWare Certification, 8/30/2002
		else   
		{  
			goto error;		
		}  
	
		// --------------------------------------
	}

    // Scan for and correct any non-terminated REG_SZ data items.
    // (Performed after string table is loaded so that debugging can be meaningful)
    FixupData();

	close (FileHandle);
	dprintf ("Done Loading Database\n");

	CleanupDelWhenClose();
	SaveGoodDatabase();
	SetThreadGroupID (TGID);

    VPRegErrorCounters.dwLoadRootsEnd += 1;

	return ERROR_SUCCESS;

//----------------------------------------
error:
	dprintf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	if (FileHandle != -1)
		close(FileHandle);

	if (tries == 0) {
		dprintf ("Trying again...\n");
		tries++;
		if (RestoreGoodDatabase() == 0)
			goto top;
		else
			dprintf ("Couldn't restore good DB\n");
		}

	dprintf ("Unable to restore database.  You will have to reinstall.\n");
	MakeEmpty();
	SetThreadGroupID (TGID);

    VPRegErrorCounters.dwLoadRootsFailed += 1;
    VPRegErrorCounters.dwLoadRootsEnd += 1;

	return ERROR_BAD_DATABASE;
}
/****************************************************************************************/
PKEY rMakeKey(LPSTR name, BOOL* realloced) {

	int i;
	PKEY NewRootKey;
	int TGID;

//dprintf ("Make Key block %d %d\n",InReg,NumBlocked);
	BLOCK;
	for (i=1;i<maxkey;i++)
		if (!RootKey[i].used) {
			RootKey[i].name = MakeStr(name);
			if (!RootKey[i].name) {
				UNBLOCK;
				return NULL;
			}
			RootKey[i].used = 1;
			RootKey[i].dirty = 1;
			UNBLOCK;
			return &RootKey[i];
		}

	TGID=SetThreadGroupID (RegTGID);
TA(	NewRootKey = (KEY*) realloc(RootKey,(maxkey+32)*sizeof(KEY)));
	SetThreadGroupID (TGID);
	if (NewRootKey) {
		RootKey = NewRootKey;
		*realloced=TRUE;
		memset(&RootKey[maxkey],0,32*sizeof(KEY));
		RootKey[maxkey].name = MakeStr(name);
		if (!RootKey[maxkey].name) {
			UNBLOCK;
			return NULL;
		}
		RootKey[maxkey].used = 1;
		RootKey[maxkey].dirty = 1;
		maxkey += 32;
		UNBLOCK;
		return &RootKey[maxkey-32];
	}
	UNBLOCK;
	return NULL;
}
/****************************************************************************************/
PKEY GetKey(HKEY hkey) {

	if (!hkey)
		return NULL;

	if (hkey == HKEY_LOCAL_MACHINE) {
		return &RootKey[ROOT_KEY_LOC];
	}

	if (hkey >= maxkey)
		return NULL;

	if (!RootKey[hkey].used)
		return NULL;

	return &RootKey[hkey];
}
/****************************************************************************************/
DWORD RegOpenDatabase(void) {

	DWORD retval=ERROR_SUCCESS;

ENTER;
CHANGE_REG;
	if (!RootKey || !RootValue || !RootData || !StringTable) {
//dprintf ("open database block\n");
		BLOCK;
			retval = LoadRoots();
		UNBLOCK;
	}
	
	RETURN(retval);
}
/****************************************************************************************/
PKEY isKeyValid(HKEY hkey) {

	PKEY k;
	
	errval=ERROR_SUCCESS;
	if (!RootKey || !RootValue || !RootData || !StringTable) {
		BOOL retval;
//dprintf ("is KeyValid block\n");
		BLOCK;
			retval = LoadRoots();
		UNBLOCK;
		if (retval) {
			errval=ERROR_NO_MEMORY;
			return NULL;
		}
	}
//	if (!RootKey || !RootValue || !RootData) {
//		return NULL;
//	}
	k=GetKey(hkey);
	if (!k) errval=ERROR_BAD_KEY;
	return k;
}
/****************************************************************************************/
void SignalChange(HKEY hkey) {

	PKEY key;

	if ((key=isKeyValid(hkey))==NULL)
		return;

  	if (key->sema==0)
  		return;
  
#ifndef LINUX
	while(NTxSemaExamine(key->sema) < 0)
#endif		
		NTxSemaSignal(key->sema);

#ifdef NLM	
	if (key->mysema==TRUE) {
		if (key->sema)
			NTxSemaClose(key->sema);
		key->mysema=FALSE;
	}
	key->sema=0;
#endif	

	CHANGE_REG;
}
/****************************************************************************************/
DWORD _RegOpenKey(HKEY base,LPCSTR okey,HKEY *out,BOOL create) {

	PKEY kbase,cur,first,last;
	LPSTR q;
	LPSTR str;
	HKEY hlast;
	char key[256];
	BOOL realloced=FALSE;

ENTER;
CHANGE_REG;

	if ((kbase=isKeyValid(base))==NULL)
		RETURN(errval);

// if the subkey name is null or empty then return the handle that was passed in
	if (!okey || !strlen(okey)) { 
		*out=base;
		RETURN (ERROR_SUCCESS);
	}

	VPstrncpy(key,okey,sizeof(key));

	q = strtok(key,"\\");
	cur = kbase;
	while (q) {
		last = cur;
		first = cur = GetKey(cur->child);
		while (cur) {
			str = GetStrn(cur->name);
			if (!str)
				RETURN(ERROR_BAD_DATABASE);
			if (!stricmp(q,str))
				break;
			last = cur;
			cur = GetKey(cur->sibling);
		}
		if (!cur) {
			if (!create)
				RETURN(ERROR_NO_PATH);
			hlast = KEYINDEX(last); // we need this in case rMakeKey realloc's RootKey
			cur = rMakeKey(q,&realloced);
			if (!cur)
				RETURN(ERROR_NO_MEMORY);
			if(realloced)
				if ((kbase=isKeyValid(base))==NULL)
					RETURN(errval);
			if (kbase->sema)
				SignalChange(base);				// A subkey has changed so we may need to signal
			last=GetKey(hlast);
			if (last) {
				if (first)
					last->sibling = KEYINDEX(cur);
				else
					last->child = KEYINDEX(cur);
			}
		}

		q = strtok(NULL,"\\");
		
		// ---------------------------------------
		// ksr - NetWare Certification, 10/21/2002   
		// Time slice 		
#ifdef NLM			
		ThreadSwitchWithDelay();	
#endif
		// ----------------------------------------
	
	}

	if (out)
		*out = KEYINDEX(cur);
	cur->open++;
CHANGE_REG;
	RETURN(ERROR_SUCCESS);
}
/****************************************************************************************/
DWORD RegOpenKeyEx(HKEY base,LPCSTR lpkey,DWORD res,DWORD sec,HKEY *out) {

	PKEY key;

	res=res;
	sec=sec;
	
	if (lpkey == NULL || *lpkey == NULL) {

	ENTER;

		if ((key=isKeyValid(base))==NULL)
			RETURN(errval);

		if (out)
			*out = KEYINDEX(key);

		key->open++;

		RETURN(ERROR_SUCCESS);
		}

	return _RegOpenKey(base,lpkey,out,FALSE);
}
/****************************************************************************************/
DWORD RegOpenKey(HKEY base,LPCSTR key,HKEY *out) {
	if (key == NULL || *key == NULL) {
		if (out)
			*out = base;
		return ERROR_SUCCESS;
		}
	return _RegOpenKey(base,key,out,FALSE);
}
/***********************************************************************************************/
DWORD RegCreateKey(HKEY base,LPCSTR key,HKEY *out) {
	DIRTY;
	return _RegOpenKey(base,key,out,TRUE);
}
/****************************************************************************************/
DWORD RegCreateKeyEx(HKEY    hKey,
                     LPCTSTR lpSubKey,
                     DWORD   Reserved,
                     LPTSTR  lpClass,
                     DWORD   dwOptions,
                     DWORD   samDesired,
                     DWORD  *lpSecurityAttributes,
                     HKEY   *phkResult,
                     DWORD  *lpdwDisposition)
{
	DIRTY;
	return _RegOpenKey(hKey,lpSubKey,phkResult,TRUE);
}
/****************************************************************************************/
static PVALUE MakeValue(LPCSTR name) {

	int i;
	PVALUE NewRootValue;
	int TGID;
//dprintf ("make value block\n");
	BLOCK;
	for (i=1;i<maxvalue;i++)
		if (!RootValue[i].used) {
			RootValue[i].name = MakeStr(name);
			if (!RootValue[i].name) {
				UNBLOCK;
				return NULL;
			}

			RootValue[i].used = 1;
			RootValue[i].dirty = 1;
			UNBLOCK;
			return &RootValue[i];
		}
		
	TGID=SetThreadGroupID (RegTGID);
TA(	NewRootValue = (VALUE*) realloc(RootValue,(maxvalue+32)*sizeof(VALUE)));
	SetThreadGroupID (TGID);
	if (NewRootValue) {
		RootValue = NewRootValue;
		memset(&RootValue[maxvalue],0,32*sizeof(VALUE));

		RootValue[maxvalue].name = MakeStr(name);
		if (!RootValue[maxvalue].name) {
			UNBLOCK;
			return NULL;
		}
		RootValue[maxvalue].used = 1;
		RootValue[maxvalue].dirty = 1;

		maxvalue += 32;
		UNBLOCK;
		return &RootValue[maxvalue-32];
	}
	UNBLOCK;
	return NULL;
}
/***************************************************************************************/
PVALUE GetValue(HVALUE val) {

	if (!val)
		return NULL;

	if (val >= maxvalue)
		return NULL;

	if (!RootValue[val].used)
		return NULL;

	return &RootValue[val];
}
/***************************************************************************************/
static PVALUE FindValue(PKEY key,LPCSTR value,BOOL create) {

	PVALUE cur,first,last;
	LPSTR str;
	HVALUE hlast;

	first = cur = GetValue(key->value);

	while (cur) {
		str = GetStrn(cur->name);
		if (!str)
			return NULL;
		if (!stricmp(value,str))
			break;
		last = cur;
		cur = GetValue(cur->value);
	}


	if (!cur) {
		if (!create)
			return NULL;

		hlast = VALUEINDEX(last);  // We need this in case MakeValue realloc's RootValue
		cur = MakeValue(value);
		if (!cur)
			return NULL;

		if (first) {
			last = GetValue(hlast);
			if (last)
				last->value = VALUEINDEX(cur);
		}
		else
			key->value = VALUEINDEX(cur);
	}
	return cur;
}

/***************************************************************************************/
PDATA Get_Data(HDATA data) {

	if (!data)
		return NULL;

	if (data >= maxdata)
		return NULL;

	if (!RootData[data].used)
		return NULL;

	return &RootData[data];
}

/***************************************************************************************/
void FreeData(HDATA data) {

	PDATA cur,last;

	cur = Get_Data(data);
	while (cur) {
		last = cur;
		cur = Get_Data(cur->link);
		last->used=0;
		memset(last,0,sizeof(DATA));
	}
}

/***************************************************************************************/
PDATA MakeData(const BYTE *data,DWORD type, int len) {

	int i;
	PDATA NewRootData;
	int TGID;

//dprintf ("Make Data block\n");
	BLOCK;
	for (i=1;i<maxdata;i++)
		if (!RootData[i].used) {
			RootData[i].used = 1;
			RootData[i].dirty = 1;
//analyze data type
            switch ( type )
            {
                case REG_DWORD:
					memcpy(RootData[i].data,data,min(len, sizeof(RootData[i].data)));
                    break;
                case REG_SZ:
		  			if(strlen((const char*) data)<sizeof(RootData[i].data))
		  				memcpy(RootData[i].data,data,strlen((const char*) data));
		  			else
		  				memcpy(RootData[i].data,data,sizeof(RootData[i].data));
                    break;
                case REG_BINARY:
					memcpy(RootData[i].data,data,min(len, sizeof(RootData[i].data)));
                    break;
                default:
					memcpy(RootData[i].data,data,min(len, sizeof(RootData[i].data)));
            }

			UNBLOCK;
			return &RootData[i];
		}

	TGID=SetThreadGroupID (RegTGID);
TA(	NewRootData = (DATA*) realloc(RootData,(maxdata+32)*sizeof(DATA)));
	SetThreadGroupID (TGID);
	if (NewRootData) {
		RootData = NewRootData;
		memset(&RootData[maxdata],0,32*sizeof(DATA));
		RootData[maxdata].used = 1;
		RootData[maxdata].dirty = 1;
		memcpy(RootData[maxdata].data,data,min(len, sizeof(RootData[maxdata].data)));
		maxdata += 32;
		UNBLOCK;
		return &RootData[maxdata-32];
	}

	UNBLOCK;
	return NULL;
}

/***************************************************************************************/
int ReadData(HDATA node,BYTE *data,int len) {

	PDATA cur = Get_Data(node);
	int i,LenRead=0;

	if (!cur)
		return 0;

	while (len&&cur) {
		memcpy(data,cur->data,i=min(len,sizeof(cur->data)));
		data += i;
		len -= i;
		LenRead += i;
		cur = Get_Data(cur->link);
	}
	return LenRead;
}

/***************************************************************************************/
HDATA WriteData(const BYTE *data,int len,DWORD type) {

	HDATA hfirst=0,hlast=0;
	PDATA cur,last=0;
	int i;

	while (len) {
		cur = MakeData(data,type, len);
		if (!cur)
			return 0;
		if (hlast)
			last = Get_Data(hlast);
		if (!hfirst)
			hlast = hfirst = DATAINDEX(cur);
		else if (last)
			hlast = last->link = DATAINDEX(cur);
		i=min(sizeof(cur->data),len);
		data += i;
		len  -= i;
	}

	return hfirst;
}

/***************************************************************************************/
void FixupData()
{
    for ( int valNum = 0; valNum < static_cast<int>(maxvalue); valNum++ ) {
        if ( RootValue[valNum].used && RootValue[valNum].type == REG_SZ ) {
            DWORD len = RootValue[valNum].len;
            size_t left = len;
            PDATA cur = Get_Data(RootValue[valNum].data);
            while ( cur && left ) {
                size_t used = min(left, sizeof(cur->data));
                if ( used == left ) {
                    // in the last DATA item
                    if ( cur->data[left - 1] ) {
                        // The string is non-terminated
                        if ( left < sizeof(cur->data) ) {
                            cur->data[left] = 0;
                            cur->dirty = 1;
                        } else {
                            BYTE tmp[1] = {0};
                            PDATA ender = MakeData(tmp, REG_SZ, 1);
                            if ( ender ) {
                                cur->link = DATAINDEX(ender);
                            } else {
                                // TODO handle error
                            }
                        }
                        DIRTY;
                        RootValue[valNum].len++;
                        RootValue[valNum].dirty = 1;
                    }
                }

                left -= used;
                cur = Get_Data(cur->link);
            }
        }
    }
}

/***************************************************************************************/
DWORD RegSetValueEx(HKEY hkey,LPCSTR value,DWORD r,DWORD type,const BYTE *data,DWORD cbdata) {

	PKEY key;
	PVALUE val;

ENTER;
CHANGE_REG;
DIRTY;

	r=r;

	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	if (!value)
		value = "(NAMELESS)";

	val = FindValue(key,value,TRUE);
	if (val == NULL)
		RETURN(ERROR_NO_MEMORY);

	val->type = type;
	FreeData(val->data);
    DWORD lenUsed = cbdata;
	if (data && cbdata) {
        const BYTE* dataUsed = data;
        BYTE* tmp = 0;
        if ( type == REG_SZ && data[cbdata - 1] != 0 ) {
            // Time to correct for a string without a trailing null terminator
            if ( data[cbdata] == 0 ) {
                lenUsed = cbdata + 1;
            } else {
                // additional correction, just to be safe
                tmp = (BYTE*)malloc(cbdata + 1);
                memcpy( tmp, data, cbdata );
                tmp[cbdata] = 0;
                lenUsed = cbdata + 1;
                dataUsed = tmp;
            }
        }
     val->data = WriteData(dataUsed, lenUsed, type);
     if ( tmp ) {
         free(tmp);
         tmp = 0;
     }
		if (val->data == 0)
			RETURN(ERROR_NO_MEMORY);
	}
    // TODO check potential problem if cbdata > 0 but data is null
    val->len = lenUsed;
	if (key->sema)
		SignalChange(hkey);						// A subkey has changed so we may need to signal
CHANGE_REG;
	RETURN(ERROR_SUCCESS);
}
/***************************************************************************************/
DWORD RegQueryValueEx(HKEY hkey,LPCSTR value,DWORD *r,DWORD *type,BYTE *data,DWORD *cbdata) {

	PKEY key;
	PVALUE val;

	r=r;

ENTER;

	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	if (!value)
		value = "(NAMELESS)";

	val = FindValue(key,value,FALSE);
	if (val == NULL)
		RETURN(ERROR_NO_PATH);

	if (type)
		*type = val->type;

	if (cbdata) {
		if (*cbdata < val->len) {
			if (data) {
				RETURN(ERROR_NO_ROOM);
			}
			else {
				*cbdata = val->len;
				RETURN(ERROR_SUCCESS);
			}
		}
		*cbdata = val->len;

		if (data)
			ReadData(val->data,data,val->len);
	}
	RETURN(ERROR_SUCCESS);
}
/***************************************************************************************/
DWORD RegEnumValue(HKEY hkey,DWORD index,LPSTR value,DWORD *cbvalue,DWORD *r,DWORD *type,
	BYTE *data,DWORD *cbdata) {

	PKEY key;
	PVALUE val;
	int i;
	int len;
	LPSTR str;

	r=r;

	ENTER;
	
	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	val = GetValue(key->value);
	for (i=0;val&&i<index;i++)
		val = GetValue(val->value);

	if (!val)
		RETURN(ERROR_NO_MORE_ITEMS);

	if (cbvalue) {
		str = GetStrn(val->name);
		if (!str)
			RETURN(ERROR_BAD_DATABASE);
		len = strlen(str)+1;
		if (*cbvalue < len) {
			if (value) {
				RETURN(ERROR_NO_ROOM);
			}
			else {
				*cbvalue = len;
				RETURN(ERROR_SUCCESS);
			}
		}
		*cbvalue = len;

		if (value)
			strcpy(value,str);
	}

	if (type)
		*type = val->type;

	if (cbdata) {
		if (*cbdata < val->len) {
			if (data) {
				RETURN(ERROR_NO_ROOM);
			}
			else {
				*cbdata = val->len;
				RETURN(ERROR_SUCCESS);
			}
		}
		*cbdata = val->len;

		if (data)
			ReadData(val->data,data,val->len);
	}
	RETURN(ERROR_SUCCESS);
}
/***************************************************************************************/
DWORD RegEnumKey(HKEY  hkey,DWORD index,LPSTR key,DWORD cbkey) {
	return RegEnumKeyEx(hkey,index,key,&cbkey,NULL,NULL,NULL,NULL);
}

/***************************************************************************************/
DWORD RegEnumKeyEx(HKEY hkey,DWORD index,LPSTR pkey,DWORD *cbkey,DWORD *r,LPSTR Class,
	DWORD *cbClass,void *time) {

	PKEY key;
	int i,len;
	LPSTR str;

	r=r;
	Class=Class;
	cbClass=cbClass;
	time=time;

ENTER;

	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	key = GetKey(key->child);
	for (i=0;key&&i<index;i++)
		key = GetKey(key->sibling);

	if (!key)
		RETURN(ERROR_NO_MORE_ITEMS);

	if (cbkey) {
		str = GetStrn(key->name);
		if (!str)
			RETURN(ERROR_BAD_DATABASE);
		len = strlen(str)+1;
		if (*cbkey < len) {
			if (pkey) {
				RETURN(ERROR_NO_ROOM);
			}
			else {
				*cbkey = len;
				RETURN(ERROR_SUCCESS);
			}
		}
		*cbkey = len;

		if (pkey)
			strcpy(pkey,str);
	}

	RETURN(ERROR_SUCCESS);
}
/***************************************************************************************/
DWORD RegFlushKey(HKEY hkey) {

	PKEY key;

	ENTER;

	if ((key=isKeyValid(hkey))==NULL)
		RETURN (errval);

	if (DataBaseDirty)
		RETURN (RegSaveDatabase());

	RETURN (ERROR_SUCCESS);
}
/***************************************************************************************/
void FreeStr(HSTR str) {

	LPSTR s = GetStrn(str);
	int TGID;

	if (s) {
		if (StringTable[str]) {
			TGID=SetThreadGroupID (RegTGID);
			free(StringTable[str]);
			SetThreadGroupID (TGID);
		}
		StringTable[str] = NULL;
	}
}
/***************************************************************************************/
void FreeValue(HVALUE value) {

	PVALUE cur = GetValue(value);

	if (cur) {
		FreeData(cur->data);
		FreeStr(cur->name);
		memset(cur,0,sizeof(VALUE));
	}
}
/***************************************************************************************/
void FreeAllValues(HVALUE value) {

	PVALUE cur = GetValue(value);
	HVALUE next;

	while (cur) {
		next = cur->value;
		FreeValue(VALUEINDEX(cur));
		cur = GetValue(next);
	}
}
/***************************************************************************************/
DWORD RegDeleteValue(HKEY hkey,LPCSTR value) {

	PKEY key;
	PVALUE val,last=0;
	LPSTR str;

ENTER;
CHANGE_REG;
DIRTY;

	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	val = GetValue(key->value);
	while (val) {
		str = GetStrn(val->name);
		if (!str)
			RETURN(ERROR_BAD_DATABASE);
		if (!strcmp(value,str)) {
			if (last)
				last->value = val->value;
			else
				key->value = val->value;
			FreeValue(VALUEINDEX(val));
			if (key->sema)
				SignalChange(hkey);					// A subkey has changed so we may need to signal
			RETURN(ERROR_SUCCESS);
		}
		last = val;
		val = GetValue(val->value);
	}

	RETURN(ERROR_NO_PATH);
}
/************************************************************************************/
void DeleteKey(HKEY hkey) {

	PKEY key = GetKey(hkey);

	if (key) {
		if (key->sema)
			SignalChange(hkey);
		FreeAllValues(key->value);
		FreeStr(key->name);
		memset(key,0,sizeof(KEY));
	}
}
/************************************************************************************/
DWORD RegDeleteKey(HKEY base,LPCSTR skey) {

	PKEY key,cur,last=0;
	LPSTR str;

ENTER;
CHANGE_REG;
DIRTY;


	if ((key=isKeyValid(base))==NULL)
		RETURN(errval);

	cur = GetKey(key->child);
	while(cur) {
		str = GetStrn(cur->name);
		if (!str)
			RETURN(ERROR_BAD_DATABASE);
		if (!stricmp(skey,str)) {
			if (cur->child) {
				RETURN(ERROR_KEY_HAS_CHILDREN);
			}
			if (last)
				last->sibling = cur->sibling;
			else
				key->child = cur->sibling; // we're deleting the first child of the key
			if (cur->open) {
				cur->delwhenclose = 1;
				if (key->sema)
					SignalChange(base);				// A subkey has changed so we may need to signal
				RETURN(ERROR_SUCCESS);
			}
//			FreeAllValues(cur->value);
			DeleteKey(KEYINDEX(cur));
			if (key->sema)
				SignalChange(base);				// A subkey has changed so we may need to signal
			RETURN(ERROR_SUCCESS);
		}
		last = cur;
		cur = GetKey(cur->sibling);
	}

	RETURN(ERROR_NO_PATH);
}
/***************************************************************************************/
DWORD RegCloseKey(HKEY hkey) {

	PKEY key;
	DWORD retval=ERROR_SUCCESS;

ENTER;
CHANGE_REG;

	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	key->open--;
#ifdef LINUX
	retval = RegFlushKey(KEYINDEX(key));   //Removing Tiffer Jenkins  Defect:1-21D0K0   Excessive File Writes on NetWare
#endif
	if ((key->open<1) && key->delwhenclose)
		DeleteKey(KEYINDEX(key));
	else
		if (key->sema)
			SignalChange(hkey);
	RETURN(retval);
}
/****************************************************************************************/
void CloseAll(void) {

	int i;

	if (RootKey) {
		for (i=0;i<maxkey;i++) {
			if (RootKey[i].used) {
				if (RootKey[i].open)
					RegCloseKey((HKEY)i);
				if (RootKey[i].sema) {
					SignalChange ((HKEY)i);
					RootKey[i].sema=0;
				}
			}
		}
	}
}
/****************************************************************************************/
DWORD RegCloseDatabase(BOOL Save) {
//dprintf ("enter %d %u\n",InReg,BlockingThread);
ENTER;
CHANGE_REG;
DIRTY;
	dprintf ("Close database\n");
BLOCK;
InReg--;

	if (Save) {
dprintf ("closeall\n");
		CloseAll();
dprintf ("save data\n");
		RegSaveDatabase();
	}
dprintf ("ClearArrays\n");
	ClearArrays();
dprintf ("done\n");
InReg++;
UNBLOCK;
RETURN(ERROR_SUCCESS);
}
/*****************************************************************************************/
DWORD RegNotifyChangeKeyValue(HKEY  hkey,BOOL  fWatchSubTree,DWORD  fdwNotifyFilter,
	HANDLE  hEvent,BOOL  fAsync) {

	PKEY key;

	fWatchSubTree=fWatchSubTree;
	fdwNotifyFilter=fdwNotifyFilter;

ENTER;

	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	if (fAsync==FALSE) {
		if (key->sema == 0) {
			key->sema=NTxSemaOpen(0,NULL);
			key->mysema=TRUE;
			}
		NTxSemaWait(key->sema);
		}
	else {
		if (key->sema)
			RETURN(ERROR_SEMAPHORE_IN_USE);
		key->sema = hEvent;
		key->mysema=FALSE;
		}

	RETURN(ERROR_SUCCESS);
}
#ifdef LINUX
void RegOpenSemaphore(DWORD hkey,int iSemaphoreNumber,long lValue) {
	PKEY key = isKeyValid(hkey); if (! key) return;
	key->sema = ipcOpenSemaphore(iSemaphoreNumber,lValue);
}
void RegAttachSemaphore(DWORD hkey,int iSemaphoreNumber) {
	PKEY key = isKeyValid(hkey); if (! key) return;
	key->sema = ipcAttachSemaphore(iSemaphoreNumber);
}
void RegCloseSemaphore(DWORD hkey) {
	PKEY key = isKeyValid(hkey); if (! (key && key->sema)) return;
	ipcCloseSemaphore(key->sema); key->sema = 0;
}
#endif

/****************************************************************************************/
/*void KeyDump(int Trunc,int FileNum) {

	char FileName[20];
	int k;
	FILE *fp;
	time_t t = time(NULL);

//dprintf ("Key Dump Block\n");
	BLOCK;
	sssnprintf (FileName,sizeof(FileName),sSLASH "v%02d_keys.txt",FileNum);

	if (!Trunc) {
		fp = fopen (FileName,"wt");
		fprintf (fp,"%s: Formated Raw Dump of RootKey\n\n",FileName);
		KeyDumpNum++;
	}
	else
		fp = fopen (FileName,"at");

	if (!fp) {
		dprintf ("Error Opening File %s\t",FileName);
		UNBLOCK;
		return;
	}

	fprintf (fp,"Dump Num: %02d  %s",Trunc,ctime(&t));
	fprintf (fp,"Keys: %d   Values: %d   Data: %d   Strings: %d\n",
		maxkey,maxvalue,maxdata,maxstring);

	fprintf (fp,"Key\tName\t\t\tValue\tSibling\tChild\tDel\n");

	for (k=0 ; k<maxkey ; k++)
		if (RootKey[k].used)
			fprintf (fp,"%d:\t%3d-%-19.19s\t%d\t%d\t%d\n",k,RootKey[k].name,
				StringTable[RootKey[k].name],RootKey[k].value,RootKey[k].sibling,
				RootKey[k].child,RootKey[k].delwhenclose);

	fprintf (fp,"\n\n");

	fflush (fp);
	fclose (fp);

	UNBLOCK;
}*/
/************************************************************************************/
BOOL FileDumpDatabase(HKEY dumpkey,char* Dir,size_t DirBytes) {

//	char FileName[IMAX_PATH];
	int TGID;

	ENTER;

	TGID=SetThreadGroupID (RegTGID);

	dprintf ("Performing Database Dump\t");

	sssnprintf (Dir,DirBytes,REG_DUMP,Dir,DumpFileNum);

	fp = fopen (Dir,"w");

	if (!fp) {
		SetThreadGroupID (TGID);
		RETURN(FALSE);
		}

	fprintf (fp,"maxkey=%d\n",maxkey);
	fprintf (fp,"maxvalue=%d\n",maxvalue);
	fprintf (fp,"maxdata=%d\n",maxdata);
	fprintf (fp,"maxstring=%d\n",maxstring);

	fprintf (fp,"DumpFileNum=%d\n",DumpFileNum);
	fprintf (fp,"KeyDumpNum=%d\n",KeyDumpNum);

	fprintf (fp,"MAXALLOWEDKEYS=%d\n",MAXALLOWEDKEYS);
	fprintf (fp,"MAXALLOWEDVALUES=%d\n",MAXALLOWEDVALUES);
	fprintf (fp,"MAXALLOWEDDATA=%d\n",MAXALLOWEDDATA);
	fprintf (fp,"MAXALLOWEDSTRINGS=%d\n",MAXALLOWEDSTRINGS);
	fprintf (fp,"MAXALLOWEDSTRINGSIZE=%d\n",MAXALLOWEDSTRINGSIZE);

	fprintf (fp,"KEYLEN=%d\n",KEYLEN);
	fprintf (fp,"VALLEN=%d\n",VALLEN);
	fprintf (fp,"DATALEN=%d\n",DATALEN);
//	fprintf (fp,"nSUBKEYS=%d\n",nSUBKEYS);
//	fprintf (fp,"nVALUES=%d\n",nVALUES);
	fprintf (fp,"KEYPATHLEN=%d\n",KEYPATHLEN);


	memset (indent,0,256);
	K=0;
	StrCopy (kPath[0],"Root");
	L=1;
	fprintf (fp,"Key: /Root\n");
	DumpSubKey(dumpkey);
	fclose (fp);

	SetThreadGroupID (TGID);
	RETURN(TRUE);
}
/************************************************************************************/
void DumpValues(HKEY hkey) {

	DWORD v=0,sz=0,dsz=0,type=0;
	int i=0;
	char name[VALLEN],data[DATALEN],buf[256],valstr[DATALEN+VALLEN];

	memset (name,0,VALLEN);
	memset (data,0,DATALEN);
	memset (buf,0,256);
	memset (valstr,0,DATALEN+VALLEN);
	
	fprintf (fp,"%sValues:\n",indent);
	for (dsz=DATALEN,sz=VALLEN,v=0 ; (RegEnumValue(hkey,v,name,&sz,NULL,&type,reinterpret_cast<unsigned char*>(data),&dsz)==ERROR_SUCCESS) ; v++,dsz=DATALEN,sz=VALLEN) {
		if (dsz) { 									// the value has data associated with it
			data[dsz]=0;
			switch(type) {
				case REG_SZ:
					sssnprintf (valstr,sizeof(valstr),"(STRING):\"%s\"",data);
					break;
				case REG_DWORD:
					sssnprintf (valstr,sizeof(valstr),"(DWORD):0x%08x = %d",*(DWORD*)data,*(DWORD*)data);
					break;
				case REG_BINARY:
					sssnprintf (valstr,sizeof(valstr),"(BINARY(%d bytes)):",dsz);
					for (i=0 ; i<dsz && NumBytes(valstr)<256 ; i++) {
						sssnprintf (buf,sizeof(buf),"%02x ",data[i]);
						strcat (valstr,buf);
					}
					if (i<dsz)
						strcat (valstr,"...");
					break;
				default:
					sssnprintf (valstr,sizeof(valstr),"ERROR! Invalid Type\n");
					break;
			}
		}
		else StrCopy (valstr,"-EMPTY-");
		fprintf (fp,"%s    %s %s\n",indent,name,valstr);
	}
	if (!v)
		fprintf (fp,"%s    NONE\n",indent);
}
/************************************************************************************/
void DumpSubKey(HKEY hkey) {

	DWORD k=0,j=0;
	HKEY hsub=0;
	char kName[KEYLEN];

	memset (kName,0,KEYLEN);

	strcat (indent,"    ");
	K=NumBytes(indent);

	DumpValues(hkey);
	for (k=0 ; (RegEnumKey(hkey,k,kName,KEYLEN)==ERROR_SUCCESS) ; k++) {
		StrCopy (kPath[L],kName);
		fprintf (fp,"\nKey: ");
		for (j=0 ; j<=L ; j++) {
			fprintf (fp,"/%s",kPath[j]);
		}
		fprintf (fp,"\n");
		if (RegOpenKey (hkey,kName,&hsub)==ERROR_SUCCESS) {
			L++;
			DumpSubKey (hsub);
			L--;
			RegCloseKey (hsub);
		}
		else
			fprintf (fp,"%sERROR! Could not Open Key\n",indent);
	}
	K-=4;
	indent[K] = 0;
}
/***************************************************************************************/
DWORD CheckDatabase(char *Dir) {

	int i;
	FILE *fp;
	time_t t = time (NULL);
	int
		UnDel=0,
		NullString=0,
		KeyName=0,
		ValueName=0,
		ValueData=0,
		RecursiveChild=0,
		RecursiveSibling=0,
		RecursiveData=0;
	char CheckFile[16];

	sssnprintf (CheckFile,sizeof(CheckFile),REG_CHECK,Dir,DumpFileNum);
	fp = fopen (CheckFile,"wt");
	if (!fp) return -1;

	fprintf (fp,"Starting Database Check at %s",ctime(&t));
	fprintf (fp,"Checking Keys\n");
	for (i=0 ; i<maxkey ; i++) {
		ThreadSwitch();
		if (RootKey[i].used) {
			if(RootKey[i].delwhenclose) {
				UnDel++;
			}
			if (!RootKey[i].name) {
				KeyName++;
				fprintf (fp,"\tKey Name Error: %d\n",i);
			}
			else if (!StringTable[RootKey[i].name]) {
				NullString++;
				fprintf (fp,"\tNull String: %d\n",RootKey[i].name);
			}
			if (RootKey[i].child == i) {
				RecursiveChild++;
				fprintf (fp,"\tRecursive Child: %d\n",i);
			}
			if (RootKey[i].sibling == i) {
				RecursiveSibling++;
				fprintf (fp,"\tRecursive Sibling: %d\n",i);
			}
		}
	}

	fprintf (fp,"----------------------\n");
	fprintf (fp,"Checking Values\n");
	for (i=0 ; i<maxvalue ; i++) {
		if (RootValue[i].used) {
			if (!RootValue[i].name) {
				ValueName++;
				fprintf (fp,"\tValue Name Error: %d\n",i);
			}
			else if (!StringTable[RootValue[i].name]) {
				NullString++;
				fprintf (fp,"\tNull String: %d\n",RootValue[i].name);
			}
			if (!RootValue[i].data) {
				ValueData++;
				fprintf (fp,"\tValue Data Error: %d\n",i);
			}
		}
	}

	fprintf (fp,"----------------------\n");
	fprintf (fp,"Checking Data\n");
	for (i=0 ; i<maxdata ; i++) {
		if (RootData[i].used) {
			if (RootData[i].link == i) {
				RecursiveData++;
				fprintf (fp,"\tRecursive Data: %d\n",i);
			}
		}
	}
	fprintf (fp,"----------------------\n");
	fprintf (fp,"Database Check Completed\n");
	fprintf (fp,"======== ===== =========\n");
	fprintf (fp,"Keys with Delete When Close set: %d\n",UnDel);	
	fprintf (fp,"Null Strings: %d\n",NullString);
	fprintf (fp,"Key Name Errors: %d\n",KeyName);
	fprintf (fp,"Value Name Errors: %d\n",ValueName);
	fprintf (fp,"Value Data Errors: %d\n",ValueData);
	fprintf (fp,"Recursive Child Keys: %d\n",RecursiveChild);
	fprintf (fp,"Recursive Sibling Keys: %d\n",RecursiveSibling);
	fprintf (fp,"Recursive Data values: %d\n\n",RecursiveData);
	fclose (fp);

	return 0;
}
/****************************************************************************************/
BOOL RawDumpDatabase(char *Dir,size_t DirBytes) {

	BOOL retval;
	int TGID;

	ENTER;

	TGID=SetThreadGroupID (RegTGID);
	//dprintf ("raw dump block\n");
	BLOCK;
		CheckDatabase(Dir);
		retval=DoDump(Dir,DirBytes);
	UNBLOCK;

	SetThreadGroupID (TGID);
	RETURN(retval);
}
/****************************************************************************************/
BOOL DoDump(char *Dir,size_t DirBytes){
	DWORD k,v,d,s,i;
	FILE *fp;
	char valstr[512],buf[128];
//	char FileName[20];
	time_t t=time(NULL);

//dprintf ("Dumping Database  ");
	DumpFileNum++;

	sssnprintf (Dir,DirBytes,RAW_REG_DUMP,Dir,DumpFileNum);

	fp = fopen (Dir,"wt");

	if (!fp) {
//dprintf ("Couldn't Open File %s\n",FileName);
		return FALSE;
	}

//dprintf ("File open %s  ",FileName);

	fprintf (fp,"Formated Raw Dump of Database\n%s\n",ctime(&t));
	fprintf (fp,"Keys: %d   Values: %d   Data: %d   Strings: %d\n\n",
		maxkey,maxvalue,maxdata,maxstring);

dprintf ("\nPrinting Keys: ");
	fprintf (fp,"RootKey:\n");
	fprintf (fp,"Key\tName\t\t\tValue\tSibling\tChild\tDel\n");

	for (k=0 ; k<maxkey ; k++)
		if (RootKey[k].used) {
			dprintf ("%d(",k);
			dprintf ("%3d-",RootKey[k].name);
			if (RootKey[k].name < maxstring)
				dprintf ("%-19.19s,",StringTable[RootKey[k].name]);
			dprintf ("%d,",RootKey[k].value);
			dprintf ("%d,",RootKey[k].sibling);
			dprintf ("%d,",RootKey[k].child);
			dprintf ("%d)\n",RootKey[k].delwhenclose);
			fprintf (fp,"%d:\t%3d-%-19.19s\t%d\t%d\t%d\t%d\n",
				k,RootKey[k].name,RootKey[k].name < maxstring?StringTable[RootKey[k].name]:"*****",RootKey[k].value,
				RootKey[k].sibling,RootKey[k].child,RootKey[k].delwhenclose);
		}

dprintf ("\nPrinting Values: ");
	fprintf (fp,"\n\nRootValue:\n");
	fprintf (fp,"Val\tName\t\t\tValue\tType\tLength\tData\n");
	for (v=0 ; v<maxvalue ; v++)
		if (RootValue[v].used) {
			dprintf ("%d",v);
			fprintf (fp,"%d:\t%3d-%-19.19s\t%d\t",
				v,RootValue[v].name,RootValue[v].name<maxstring?StringTable[RootValue[v].name]:"****",RootValue[v].value);
			switch (RootValue[v].type) {
				case REG_DWORD:
	dprintf ("d ");
					fprintf (fp,"DWORD\t%d\t%2d-0x%08x\n",RootValue[v].len,RootValue[v].data,
						*(DWORD*)RootData[RootValue[v].data].data);
					break;

				case REG_SZ:
	dprintf ("s ");
					fprintf (fp,"STRING\t%d\t%2d-\"%s\"\n",RootValue[v].len,RootValue[v].data,
						RootData[RootValue[v].data].data);
						break;

				case REG_BINARY:
				case REG_NONE:
	dprintf ("b ");
					fprintf (fp,"BINARY\t%d\t%2d-",RootValue[v].len,RootValue[v].data);

					for (i=0 ; i<RootValue[v].len && strlen(valstr)<20 ; i++) {
						sssnprintf (buf,sizeof(buf),"%02x ",RootData[RootValue[v].data].data[i]);
						strcat (valstr,buf);
					}

					if (i<RootValue[v].len)
						strcat (valstr,"...");

					fprintf (fp,"%s\n",valstr);
					break;

				default:
	dprintf ("u ");
					fprintf (fp,"UNKNOWN\t%d\t%2d\n",RootValue[v].len,RootValue[v].data);
					break;
			}
		}

dprintf ("\nPrint Data: ");
	fprintf (fp,"\n\nRootData:\n");
	fprintf (fp,"data\tlink\tdata\n");
	for (d=0 ; d<maxdata ; d++)
		if (RootData[d].used) {
			dprintf ("%d ",d);
			fprintf (fp,"%d:\t%d\t\"%s\"\n",d,RootData[d].link,RootData[d].data);
		}

dprintf ("\nPrint Strings: ");
	fprintf (fp,"\n\nStringTable:\n");
	fprintf (fp,"Str\tString\n");
	for (s=0 ; s<maxstring ; s++)
		if (StringTable[s]) {
			dprintf ("%d ",s);
			fprintf (fp,"%d:\t\"%s\"\n",s,StringTable[s]);
		}

	fflush (fp);
	fclose (fp);
	flushall();
dprintf ("\nDone\n");
	return TRUE;
}
/****************************************************************************************/

/****************************************************************************************/
DWORD DuplicateHandle(DWORD inProcess,HKEY in,DWORD outProcess,HKEY *out,DWORD access,BOOL inherant,DWORD options) {

	PKEY key;

	inProcess=inProcess;
	outProcess=outProcess;
	access=access;
	inherant=inherant;
	options=options;

ENTER;

	if ((key=isKeyValid(in))==NULL)
		RETURN(errval);

	if (out)
		*out = KEYINDEX(key);

	key->open++;

	RETURN(ERROR_SUCCESS);
}

/*********************************************************************/
int PurgeFile (const char *file) {
#ifdef NLM
	SetPurgeFlag( file );
#endif
	return unlink( file );
}
#ifdef NLM
/*********************************************************************/
void SetPurgeFlag (const char *file) {

	struct stat Stats;

	if( stat( file, &Stats ) !=0 )
		return;

	if( Stats.st_attr & _A_IMMPURG ) // purge flag is already set
		return;

	Stats.st_attr |= _A_IMMPURG;	// and set them to read/write
	MySetFileInfo( file, &Stats );		// and write to the file
}
/*************************************************************************************/
int MySetFileInfo(const char *file,const struct stat *stbuf) {
	struct DOSDateTime {
		struct _DOSDate DD;
		struct _DOSTime DT;
	} DC,DLU,DLA,DLa;

	// convert calendar time to DOS style date and time
	_ConvertTimeToDOS(stbuf->st_ctime,&DC.DD,&DC.DT);       // create date and time
	_ConvertTimeToDOS(stbuf->st_atime,&DLA.DD,&DLA.DT);     // Last access date and time
	_ConvertTimeToDOS(stbuf->st_btime,&DLa.DD,&DLa.DT);     // Last archive date and time
	_ConvertTimeToDOS(stbuf->st_mtime,&DLU.DD,&DLU.DT);     // Last update/modify date and time

	return SetFileInfo(file,6 /* Hidden and System */,stbuf->st_attr,(char *)&DC,
						(char *)&DLA.DD,(char *)&DLU,(char *)&DLa,stbuf->st_uid);
}
/*********************************************************************/
#endif
