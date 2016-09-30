// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// We want to make sure these Reg* functions aren't aliased as ipcReg* functions
#ifdef LINUX
#define REGISTRY_CLIENT_IPC_EXPLICIT
#endif

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
	#define ACCESSCHECK R_OK
#else
	#define ACCESSCHECK 0
#endif

#undef LONG
#define LONG long

void DumpSubKey(HKEY hkey);  // gdf CW conversion

#if defined(LINUX)
#define GLOBAL_VAR
#else
#define GLOBAL_VAR extern "C"
#endif

#ifdef NLM
extern char gVPRegDebug     = 0;
#else
GLOBAL_VAR char gVPRegDebug     = 0;
#endif
char RegistryRunning = 1;
GLOBAL_VAR int  RegTGID         = 0;

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
// Port from changelist 1907204 removed Blocked, BlockingThread, NumBlocked, & Times.
// BOOL   Blocked                   = 0;
// void*  BlockingThread            = NULL;
// int    NumBlocked                = 0;
// int    Times                     = 0;
// End port from changelist 1907204 removed Blocked, BlockingThread, NumBlocked, & Times.

BOOL   DataBaseDirty             = FALSE;
#ifndef LINUX
extern "C" BOOL vpRegChanged     = TRUE;
#else
BOOL vpRegChanged                = TRUE;
#endif

// Begin Port from changelist 1907204
extern long registryLockHandle;
extern BOOL registryAvailable;
extern int registryLockOwner;
extern int registryLockCount;
extern int *_InstallInProgress;
// End Port from changelist 1907204

FILE   *fp                       = NULL; // File pointer for the database dump file
char   indent[256]               = "";   // indent buffer for db dump
int    K                         = 0;
int    Ell                       = 0;    // used in db dump for indent(K) and Key path
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
	#define INT_TO_OS_TGID(i) reinterpret_cast<HANDLE>(i)
#else
	#define INT_TO_OS_TGID(i) ((int)(i))
#endif

#ifdef LINUX

#define WAIT_FOR_BLOCK (void)(0)

#define ENTER (void)(0)
#define RETURN(x) return (x)

#define SET_BLOCK (void)(0)
#define WAIT_TILL_REG_CLEAR (void)(0)

#define BLOCK (void)(0)
#define UNBLOCK (void)(0)

#else // LINUX

#define WAIT_FOR_BLOCK	while (Blocked && RegistryRunning) {if (NTxGetCurrentThreadId()== BlockingThread) {VPRegErrorCounters.dwDoubleBlock_TroubleFollows += 1; break;} VPRegErrorCounters.dwWaitForBlock += 1; ThreadSwitchWithDelay(); }

#define ENTER {WAIT_FOR_BLOCK; VPRegErrorCounters.dwEnter += 1; InReg++;}
#define RETURN(x) {InReg--; VPRegErrorCounters.dwReturn += 1; return x;}

#define SET_BLOCK Blocked=TRUE; BlockingThread = NTxGetCurrentThreadId(); if (VPRegErrorCounters.dwLoadRootsBegin != VPRegErrorCounters.dwLoadRootsEnd) VPRegErrorCounters.dwCrashInLoadRoots += 1;
#define WAIT_TILL_REG_CLEAR while ((InReg-NumBlocked)>0 && RegistryRunning) {if (NTxGetCurrentThreadId()== BlockingThread) {VPRegErrorCounters.dwDoubleInReg_TroubleFollows += 1; break;} VPRegErrorCounters.dwWaitTillRegClear += 1; ThreadSwitchWithDelay(); }

// Add something to tell which thread has blocked so we don't get deadlocked waiting for ourself
#define BLOCK {NumBlocked++; WAIT_FOR_BLOCK; SET_BLOCK; WAIT_TILL_REG_CLEAR;  NumBlocked--; SET_BLOCK;}
#define UNBLOCK {Blocked=FALSE; BlockingThread=NULL;}

#endif // LINUX

// Begin port from changelist 1907204 to the Netware trunk.
#define BLOCK Lock(FALSE);
#define UNBLOCK Unlock();
#define ENTER(x) if (FALSE == Lock(FALSE)) return x;
#define RETURN(x) {Unlock(); return x;}

BOOL Lock(int OverrideAvailable)
{
	// Acquire lock
	if ((registryLockHandle != -1) && (registryAvailable || OverrideAvailable))
	{
		if (registryLockOwner != GetThreadID())
		{
			WaitOnLocalSemaphore(registryLockHandle);
			if (!(registryAvailable || OverrideAvailable))
			{
				registryLockOwner = NULL;
				dprintf("Lock: registryAvailable FALSE after WaitOnLocalSemaphore");
				SignalLocalSemaphore(registryLockHandle);
				return FALSE;
			}
    		registryLockOwner = GetThreadID();
		}
		registryLockCount += 1;
	}
	else
	{
		// This state should be impossible
		dprintf("Somebody wants us before we are ready, or after we are closing.  registryLockHandle = %ld   registryAvailable = %ld\n", registryLockHandle, registryAvailable);
		return FALSE;
	}
	return TRUE;
}
BOOL Unlock()
{
	// Release lock
	if ((registryLockHandle != -1) && (registryAvailable))
	{
		registryLockCount -= 1;
		if (registryLockCount == 0)
		{
			registryLockOwner = NULL;
    		SignalLocalSemaphore(registryLockHandle);
		}
		return TRUE;
	}
	dprintf("Unlock failure.  registryLockHandle = %ld   registryAvailable = %ld\n", registryLockHandle, registryAvailable);
	return FALSE;
}

// End port from changelist 1907204 to the Netware trunk.

#define TA(x)	if (!(x)) {RingTheBell(); x;}

static void SignalChange(HKEY hkey);

static void CleanupDelWhenClose();
static void CleanupKey (int i);
static void CleanupValue (int i);
static void CleanupData (int i);
static void CleanupString (int i);

static BOOL DoDump(char *Dir,size_t DirBytes);

void FixupData();

void FixupData();

#ifdef LINUX

// ----- Database file header ----

#define MAGIC_COOKIE "VPRG"
#define HEADER_COOKIE_LEN 4

#define FILE_VERSION 0x00000001
#define HEADER_VERSION_LEN 4

static bool WriteHeader(int fd);

enum RdHdrResult
{
    hdr_success,                // header read OK
    hdr_read_failed,            // failed to read file
    hdr_bad_file_id             // magic number is invalid
};

static RdHdrResult ReadHeader(int fd, unsigned long& version);

#endif // LINUX

static int safeRead(int fd, void* buf, int wanted);
static int safeWrite(int fd, const void* data, int len);


/***************************************************************************************/
void CleanupDelWhenClose()
{
	int i;

	for ( i = 0; i < static_cast<int>(maxkey); i++ ) {
		if(RootKey[i].delwhenclose)
			CleanupKey(i);
		// This if added as part of port of 1907204 to the Netware Trunk.
		if (i % 50 == 49)	
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

    SignalChange(reinterpret_cast<HKEY>(i));

	RootKey[i].used=0;

	CleanupString(RootKey[i].name);
	CleanupValue(RootKey[i].value);

#ifdef LINUX
    if (-1 != RootKey[i].localNotifySemid)
    {
        // Delete the key's local change notify semaphore.
        semctl(RootKey[i].localNotifySemid, 0, IPC_RMID, 0);
        RootKey[i].localNotifySemid = -1;
        RootKey[i].localNotifySemnum = 0;
    }

    if (-1 != RootKey[i].subkeyNotifySemid)
    {
        // Delete the key's subkey change notify semaphore.
        semctl(RootKey[i].subkeyNotifySemid, 0, IPC_RMID, 0);
        RootKey[i].subkeyNotifySemid = -1;
        RootKey[i].subkeyNotifySemnum = 0;
    }
#endif // LINUX

	CleanupKey( reinterpret_cast<int>(RootKey[i].sibling) );
	CleanupKey( reinterpret_cast<int>(RootKey[i].child) );

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
    LONG nRet = safeWrite( handle, buffer, len );

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
// Port from changelist 1907204 to the Netware trunk renamed RegSaveDatabase to _RegSaveDatabase.
DWORD _RegSaveDatabase(void) {

        DWORD i,numStrings=0,dwRet=ERROR_NO_DATABASE;
	WORD len;
	int FileHandle;
	int TGID = -1;


//dprintf ("s\n");

	if (DataBaseDirty == FALSE)
    {
        VPRegErrorCounters.dwRegSaveDatabaseNoSaveNeeded += 1;
		return 0xffffffff;
    }


	if (!RootKey || !RootValue || !RootData || !StringTable)
    {
        VPRegErrorCounters.dwTablesMissingInMemory += 1;

		return (ERROR_NO_MEMORY);
    }

	// Begin port from changelist 1907204 to the Netware Trunk.
    VPRegErrorCounters.dwRegSaveDatabase += 1;

	TGID=SetThreadGroupID (RegTGID);
	if (TGID == -1)
	{
		dprintf("SetThreadGroupID failed.\n");
		goto Done;
	}

	PurgeFile(REGISTRY_LOCATION"VPREGDB.TMP");
	ThreadSwitchWithDelay();
	// End port from changelist 1907204 to the Netware Trunk.

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

#ifdef LINUX
    if(!WriteHeader(FileHandle))
    {
        dprintf("failed to write database file\n");
        goto Done;
    }
#endif // LINUX

	// Begin port from changelist 1907204 to the Netware Trunk.
	if (maxkey > MAXALLOWEDKEYS)
	{
		dprintf("maxkey greater than MAXALLOWEDKEYS.  %U\n", maxkey);
		goto Done;
	}
	// End port from changelist 1907204 to the Netware Trunk.


	i = maxkey*sizeof(KEY);
	if(CheckedWrite(FileHandle,&i,sizeof(i))==-1) goto Done;
	if(CheckedWrite(FileHandle,RootKey,i)==-1) goto Done;

	// Begin port from changelist 1907204 to the Netware Trunk.
	if (maxvalue > MAXALLOWEDVALUES)
	{
		dprintf("maxvalue greater than MAXALLOWEDVALUES.  %U\n", maxvalue);
		goto Done;
	}
	// End port from changelist 1907204 to the Netware Trunk.

	i = maxvalue*sizeof(VALUE);
	if(CheckedWrite(FileHandle,&i,sizeof(i))==-1) goto Done;
	if(CheckedWrite(FileHandle,RootValue,i)==-1) goto Done;

	// Begin port from changelist 1907204 to the Netware Trunk.
	if (maxdata > MAXALLOWEDDATA)
	{
		dprintf("maxdata greater than MAXALLOWEDDATA.  %U\n", maxdata);
		goto Done;
	}
	// End port from changelist 1907204 to the Netware Trunk.

	i = maxdata*sizeof(DATA);
	if(CheckedWrite(FileHandle,&i,sizeof(i))==-1) goto Done;
	if(CheckedWrite(FileHandle,RootData,i)==-1) goto Done;

	// Begin port from changelist 1907204 to the Netware Trunk.
	if (maxstring > MAXALLOWEDSTRINGS)
	{
		dprintf("maxstring is out of range.  %ul\n", maxstring);
		goto Done;
	}
	// End port from changelist 1907204 to the Netware Trunk.

	for (i=0;i<maxstring;i++)
		if (StringTable[i])
			numStrings++;

	// Begin port from changelist 1907204 to the Netware Trunk.
	if (numStrings > MAXALLOWEDSTRINGS)
	{
		dprintf("numStrings greater than MAXALLOWEDSTRINGS.  %U\n", numStrings);
		goto Done;
	}
	// End port from changelist 1907204 to the Netware Trunk.

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

	// Begin port from changelist 1907204 to the Netware Trunk.
	if (!RootKey || !RootValue || !RootData || !StringTable)
	{
		VPRegErrorCounters.dwTablesMissingInMemory += 1;
		dprintf("Root Pointers have gone to NULL during RegSaveDatabase.  Not Saving.\n");
		dprintf("RootKey: %p\tRootValue: %p\tRootData: %p\tStringTable: %p\n", RootKey, RootValue, RootData, StringTable);

		goto Done;
	}

	PurgeFile(REGISTRY_LOCATION"VPREGDB.BAK");
	ThreadSwitchWithDelay();
	rename (REGISTRY_LOCATION"VPREGDB.DAT",REGISTRY_LOCATION"VPREGDB.BAK");
	ThreadSwitchWithDelay();
	rename (REGISTRY_LOCATION"VPREGDB.TMP",REGISTRY_LOCATION"VPREGDB.DAT");
	ThreadSwitchWithDelay();
	// End port from changelist 1907204 to the Netware Trunk.

	DataBaseDirty = FALSE;

	dwRet=ERROR_SUCCESS;

Done:
    if (FileHandle != -1)
    {
        close (FileHandle );        // not leaking handles is a good idea
    }

	// Begin port from changelist 1907204 to the Netware Trunk.
	PurgeFile(REGISTRY_LOCATION"VPREGDB.TMP");
   
	if (TGID != -1)
		SetThreadGroupID (TGID);

	if(dwRet!=ERROR_SUCCESS) 
	{
		VPRegErrorCounters.dwRegSaveDatabaseErrors += 1;
	}

	return(dwRet);
	// End port from changelist 1907204 to the Netware Trunk.

}

/***************************************************************************************/
// RegSaveDatabase add as part port from changelist 1907204 to the Netware Trunk.
DWORD RegSaveDatabase( void )
{
	DWORD	retVal;

	ENTER (ERROR_BADDB);
	retVal = _RegSaveDatabase();
	RETURN(retVal);
}
// End port from changelist 1907204 to the Netware Trunk.


/****************************************************************************************/
static HSTR MakeStr(LPCSTR name) {

	DWORD i;
	LPSTR *NewStringTable;
	int TGID;

// Begin port from changelist 1907204 to the Netware Trunk.
// Note that use of BLOCK; & UNBLOCK; & SetThreadGroupID (TGID); were removed as part of port.

	for (i=1;i<maxstring;i++)
		if (StringTable[i]==NULL) {
			TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));
TA(			StringTable[i] = (char*) malloc(strlen(name)+1));
			SetThreadGroupID(INT_TO_OS_TGID(TGID));

			if (StringTable[i]==NULL) {
				return 0;
			}
			memset (StringTable[i],0,strlen(name)+1);
			strcpy(StringTable[i],name);
			return (HSTR)i;
		}

	TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));
TA(	NewStringTable = (char**) realloc(StringTable,(maxstring+64)*sizeof(LPSTR)));		//<<<<<<<<<< possible sync problem. use malloc?
	if (NewStringTable) {
		StringTable = NewStringTable;
		memset(&StringTable[maxstring],0,64*sizeof(LPSTR));
TA(		StringTable[maxstring] = (char*) malloc(strlen(name)+1));
		SetThreadGroupID(INT_TO_OS_TGID(TGID));
		if (StringTable[maxstring]==NULL) {
			return 0;
		}
		memset (StringTable[i],0,strlen(name)+1);
		strcpy(StringTable[maxstring],name);
		maxstring += 64;
		UNBLOCK;
		return (HSTR)(maxstring-64);
	}
	SetThreadGroupID (TGID);
	return 0;
// End port from changelist 1907204 to the Netware Trunk.

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
        i = safeRead(FromFile,buf,sizeof(buf));
        if (i > 0)  // some bytes read, no error
        {
            nRet=safeWrite(ToFile,buf,i);
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

	dprintf("Old database corrupted.  Restoring backup.\n");

    VPRegErrorCounters.dwRestoreGoodDatabase += 1;

	for (i=0 ; i<0xfff ; i++) {
		sssnprintf( BadDB, sizeof(BadDB), "%sVPREGDB.%03x", REGISTRY_LOCATION, static_cast<int>(i) );
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
// Begin port from changelist 1907204 to the Netware Trunk.
// RestoreBackDatabase added as part of the port.
DWORD RestoreBackDatabase( void )
// only called from LoadRoots so no need to synchronize
{

	int good,copy;
	DWORD ret = 0xffffffff;
	DWORD i;
	char BadDB[16];

	dprintf("Old database corrupted.  Restoring backup. (vpregdb.bak)\n");


	for (i=0 ; i<0xfff ; i++) {
		sprintf (BadDB,"vpregdb.bak.%03x",i);
		ThreadSwitchWithDelay();
		if (access (BadDB,0)) break;
	}

	rename ("VPREGDB.DAT",BadDB);

	copy = open("VPREGDB.TMP",O_TRUNC|O_CREAT|O_BINARY|O_TRUNC|O_WRONLY,S_IREAD|S_IWRITE);
	if (copy != -1) 
	{
		good = open("VPREGDB.BAK",O_BINARY|O_RDONLY,0);
		if (good != -1) 
		{
            ret = CopyDatabase( copy, good );
			close(good);
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
		PurgeFile("VPREGDB.DAT");
		rename("VPREGDB.TMP","VPREGDB.DAT");
	}
	PurgeFile("VPREGDB.TMP");

	return ret;
}

/****************************************************************************************/
DWORD ClearArrays(void) {

	int i;
	int TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));

	// This line added as part of port from changelist 1907204 to the Netware Trunk.
	registryAvailable = FALSE;

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
		for ( i = 0; i < static_cast<int>(maxstring); i++ )
			if (StringTable[i]) {
				free(StringTable[i]);
				StringTable[i] = NULL;
			}
		free(StringTable);
		StringTable=NULL;
	}
	SetThreadGroupID(INT_TO_OS_TGID(TGID));

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

	TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));
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

		SetThreadGroupID( INT_TO_OS_TGID(TGID) );

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

	SetThreadGroupID(INT_TO_OS_TGID(TGID));

	memset(RootKey,0,sizeof(KEY)*maxkey);
	memset(RootValue,0,sizeof(VALUE)*maxvalue);
	memset(RootData,0,sizeof(DATA)*maxdata);
	memset(StringTable,0,sizeof(LPSTR)*maxstring);

	DumpFileNum=0;
	KeyDumpNum=1;

	// Begin port from changelist 1907204 to the Netware Trunk.
	// The port removed the following 3 lines.
	// Removed: BlockingThread=NULL;
	// Removed: NumBlocked=0;
	// Removed: Blocked=0;
	// End port from changelist 1907204 to the Netware Trunk.

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
        {
			RootKey[ROOT_KEY_LOC].used = 1;
#ifdef LINUX
            RootKey[ROOT_KEY_LOC].localNotifySemid = -1;
            RootKey[ROOT_KEY_LOC].subkeyNotifySemid = -1;
#endif
        }
		else	
			return ERROR_NO_MEMORY;
	}

	// This line added as part of port from changelist 1907204 to the Netware Trunk.
	registryAvailable = TRUE;  // It is available just empty.

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

	// Begin port from changelist 1907204 to the Netware Trunk.
	TGID=SetThreadGroupID (RegTGID);

	// On servers with a very heavy startup load, we can get to here before the file system is 
	// ready for us.  This wait loop keeps us from reinitializing the database for no reason.
	i = 60;
	dprintf("VPReg:LoadRoots\n");
	while (_InstallInProgress && !*_InstallInProgress && access("VPREGDB.DAT",0) && i-- > 0)
	{
	    dprintf("access(VPREGDB.DAT) failed. i = %d\n", i);
	    delay(500);
	}

	if( access( REGISTRY_LOCATION"VPREGDB.DAT", ACCESSCHECK ) && access( REGISTRY_LOCATION"VPREGDB.SAV", ACCESSCHECK ) ) 
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
		dprintf("RestoreBackDatabase being called from LoadRoots. VPRegDB.DAT not oppened.\n");
		if (RestoreBackDatabase() == 0) 
		{
			tries++;

			FileHandle = open(REGISTRY_LOCATION"VPREGDB.DAT",O_BINARY|O_RDONLY,0);
			if (FileHandle==-1) 
			{
				dprintf("RestoreGoodDatabase being called from LoadRoots. VPRegDB.DAT not oppened.\n");
				if (RestoreGoodDatabase() == 0) 
				{
					tries++;
					FileHandle = open(REGISTRY_LOCATION"VPREGDB.DAT",O_BINARY|O_RDONLY,0);
				}
			}
		}
		if (FileHandle == -1) 
		{
			dprintf("All opens failed.  Making the registry empty.\n");
			MakeEmpty();
			SetThreadGroupID (TGID);
			VPRegErrorCounters.dwLoadRootsFailed += 1;
			VPRegErrorCounters.dwLoadRootsEnd += 1;
			return ERROR_EMPTY;
		}
	}


#ifdef LINUX
    unsigned long fileVersion = 0;
    switch(ReadHeader(FileHandle, fileVersion))
    {
    case hdr_success:
        if(fileVersion != FILE_VERSION)
        {
            dprintf("Incorrect file version\n");
            goto error;
        }
        break;

    case hdr_read_failed:
        dprintf("Failed to read database file\n");
        goto error;
        break;

    case hdr_bad_file_id:
        dprintf("Unrecognized database file\n");
        goto error;
        break;

    default:
        dprintf("Unknown database error\n");
        goto error;
    }
#endif // LINUX

	dprintf ("Database File Opened\n");
	dprintf ("*********************************************************\n");

	i = safeRead(FileHandle,&dLen,sizeof(dLen));


	if (i != sizeof(dLen))
		goto error;
	dprintf ("Keys:%d\t",dLen/sizeof(KEY));

	if (dLen/sizeof(KEY) > MAXALLOWEDKEYS)
		goto error;

	if (dLen > maxkey*sizeof(KEY)) {
		NewRootKey = (KEY*) realloc(RootKey,dLen);
		if (!NewRootKey) {
			close (FileHandle);
            VPRegErrorCounters.dwLoadRootsFailed += 1;
            VPRegErrorCounters.dwLoadRootsEnd += 1;

			// ksr - Certification, 7/21/2002
			// free memory Initarrays() if the realloc() fails.
			ClearArrays();          
			
			SetThreadGroupID(TGID);
			
			return ERROR_NO_MEMORY;
		}
		RootKey = NewRootKey;
		maxkey = dLen / sizeof(KEY);
	}

	i = safeRead(FileHandle,RootKey,dLen);
	if (i != static_cast<int>(dLen))
		goto error;
	dprintf ("Keys Read\n");
	dprintf ("*********************************************************\n");

	for ( i = 0; i < static_cast<int>(maxkey); i++ )
#ifdef LINUX
    {
        RootKey[i].localNotifySemid = -1;
        RootKey[i].subkeyNotifySemid = -1;
    }
#else
		RootKey[i].sema=0;
#endif

	i = safeRead(FileHandle,&dLen,sizeof(dLen));
	dprintf ("Values:%d\t",dLen/sizeof(VALUE));

	if (i != sizeof(dLen))
		goto error;

	if (dLen/sizeof(VALUE) > MAXALLOWEDVALUES)
		goto error;

	if (dLen > maxvalue*sizeof(VALUE)) {
		NewRootValue = (VALUE*) realloc(RootValue,dLen);
		if (!NewRootValue) {
			close (FileHandle);
			SetThreadGroupID(INT_TO_OS_TGID(TGID));
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

			SetThreadGroupID (TGID);
			return ERROR_NO_MEMORY;
		}
		RootValue = NewRootValue;
		maxvalue = dLen / sizeof(VALUE);
	}

	i = safeRead(FileHandle,RootValue,dLen);

	if (i != static_cast<int>(dLen))
		goto error;
	dprintf ("Values Read\n");
	dprintf ("*********************************************************\n");

	i = safeRead(FileHandle,&dLen,sizeof(dLen));
	dprintf ("Data Items:%d\t",dLen/sizeof(DATA));

	if (i != sizeof(dLen))
		goto error;

	if (dLen/sizeof(DATA) > MAXALLOWEDDATA)
		goto error;

	if (dLen > maxdata*sizeof(DATA)) {
		NewRootData = (DATA*) realloc(RootData,dLen);
		if (!NewRootData) {
			close (FileHandle);
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

			SetThreadGroupID (TGID);
			return ERROR_NO_MEMORY;
		}
		RootData = NewRootData;
		maxdata = dLen / sizeof(DATA);
	}
	i = safeRead(FileHandle,RootData,dLen);
	if (i != static_cast<int>(dLen))
		goto error;
	dprintf ("Data Read\n");
	dprintf ("*********************************************************\n");

	dprintf ("Begin Reading Strings\n");

	i = safeRead(FileHandle,&numStrings,sizeof(numStrings));
	//dprintf ("i:%d  ns:%d\n",i,numStrings);

//	if (i!=sizeof(numStrings)){  // gdf 03/22/2003 Fix 1-YVE7V
	if (i!=sizeof(numStrings)|| (numStrings == 0) ){    // gdf 03/22/2003 Fix 1-YVE7V	
			dprintf ("numStrings Error\n");
			goto error;
		}

	for (j=0;j<numStrings;j++) 
	{
		// ksr - Certification, 7/21/2002
		int jj;  

		i = safeRead(FileHandle,&dLen,sizeof(dLen));
		//dprintf ("%d\t",dLen);
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

				SetThreadGroupID (TGID);
				return ERROR_NO_MEMORY;
			}
			StringTable = NewStringTable;
			memset(&StringTable[maxstring],0,64*sizeof(LPSTR));
			maxstring += 64;
		}
		i = safeRead(FileHandle,&wLen,sizeof(wLen));
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
            VPRegErrorCounters.dwLoadRootsFailed += 1;
            VPRegErrorCounters.dwLoadRootsEnd += 1;
            
            // -------------------------------------
			// ksr - Certification, 7/21/2002
			
			// free StringTable memory 
			for( jj=1; jj < static_cast<int>(dLen); jj++ )       
				if( StringTable[jj] )         
					free( StringTable[jj] );  

			// free Initarrays() if malloc() fails.
			ClearArrays();         
            // -------------------------------------

			SetThreadGroupID (TGID);
			return ERROR_NO_MEMORY;
		}
		i = safeRead(FileHandle,StringTable[dLen],wLen);
		//dprintf ("%s\t",StringTable[dLen]);
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
        {
			RootKey[ROOT_KEY_LOC].used = 1;
#ifdef LINUX
            RootKey[ROOT_KEY_LOC].localNotifySemid = -1;
            RootKey[ROOT_KEY_LOC].subkeyNotifySemid = -1;
#endif
        }
			
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

	registryAvailable = TRUE;

	CleanupDelWhenClose();
	SaveGoodDatabase();
	SetThreadGroupID(INT_TO_OS_TGID(TGID));

    VPRegErrorCounters.dwLoadRootsEnd += 1;

	return ERROR_SUCCESS;

//----------------------------------------
error:
	dprintf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	if (FileHandle != -1)
		close(FileHandle);

	if (tries == 0) {
		dprintf ("Trying again...BAK\n");
		tries++;
		if (RestoreBackDatabase() == 0)
			goto top;
		else
			dprintf("Couldn't restore backup database.\n");
		}

	if (tries == 1) {
		dprintf ("Trying again...SAV\n");
		tries++;
		if (RestoreGoodDatabase() == 0)
			goto top;
		else
			dprintf ("Couldn't restore good DB\n");
		}

	dprintf ("Unable to restore database.  You will have to reinstall.\n");
	MakeEmpty();
	SetThreadGroupID(INT_TO_OS_TGID(TGID));

    VPRegErrorCounters.dwLoadRootsFailed += 1;
    VPRegErrorCounters.dwLoadRootsEnd += 1;

	return ERROR_BAD_DATABASE;

// End of porting from changelist 1907204 to Netware trunk for this function.
}
/****************************************************************************************/
PKEY rMakeKey(LPSTR name, BOOL* realloced) {

	int i;
	PKEY NewRootKey;
	int TGID;

// Begin port from changelist 1907204 to Netware trunk.
// Use of BLOCK; & UNBLOCK; were removed.

	for (i = 1; i < static_cast<int>(maxkey); i++ )
		if (!RootKey[i].used) {
			RootKey[i].name = MakeStr(name);
			if (!RootKey[i].name) {
				return NULL;
			}
			RootKey[i].used = 1;
			RootKey[i].dirty = 1;
#ifdef LINUX
            RootKey[i].localNotifySemid = -1;
            RootKey[i].subkeyNotifySemid = -1;
#endif
			return &RootKey[i];
		}

	TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));
TA(	NewRootKey = (KEY*) realloc(RootKey,(maxkey+32)*sizeof(KEY)));
	SetThreadGroupID(INT_TO_OS_TGID(TGID));
	if (NewRootKey) {
		RootKey = NewRootKey;
		*realloced=TRUE;
		memset(&RootKey[maxkey],0,32*sizeof(KEY));
		RootKey[maxkey].name = MakeStr(name);
		if (!RootKey[maxkey].name) {
			return NULL;
		}
		RootKey[maxkey].used = 1;
		RootKey[maxkey].dirty = 1;
#ifdef LINUX
        RootKey[maxkey].localNotifySemid = -1;
        RootKey[maxkey].subkeyNotifySemid = -1;
#endif
		maxkey += 32;
		return &RootKey[maxkey-32];
	}
	return NULL;
}
/****************************************************************************************/
PKEY GetKey(HKEY hkey) {

	if (!hkey)
		return NULL;

	if (hkey == HKEY_LOCAL_MACHINE) {
		return &RootKey[ROOT_KEY_LOC];
	}

	if (reinterpret_cast<int>(hkey) >= static_cast<int>(maxkey))
		return NULL;

	if (!RootKey[reinterpret_cast<int>(hkey)].used)
		return NULL;

	return &RootKey[reinterpret_cast<int>(hkey)];
}
/****************************************************************************************/
DWORD RegOpenDatabase(void) {

	DWORD retval=ERROR_SUCCESS;

	// Begin port from changelist 1907204 to Netware trunk.
	// This is a form of ENTER()  Thus the matching RETURN....
	if (!Lock(TRUE))
		return(ERROR_REG_FAIL);

CHANGE_REG;
	if (!RootKey || !RootValue || !RootData || !StringTable) {
			retval = LoadRoots();
	}

	// End port from changelist 1907204 to Netware trunk.
	
	RETURN(retval);
}
/****************************************************************************************/
PKEY isKeyValid(HKEY hkey) {

	PKEY k;
	
	// Begin port from changelist 1907204 to Netware trunk.
	errval=ERROR_SUCCESS;
	if (!RootKey || !RootValue || !RootData || !StringTable) {
		if (LoadRoots()) 
		{
			errval=ERROR_NO_MEMORY;
			return NULL;
		}
	}
	k=GetKey(hkey);
	if (!k) errval=ERROR_BAD_KEY;
	return k;
	// End port from changelist 1907204 to Netware trunk.
}

#ifdef LINUX
void NotifySemaphore( int semid, int semnum )
{
    // Only operate on valid semaphores
    if (-1 == semid || 0 > semnum)
    {
        return;
    }
    
    // Release threads waiting on the semaphore by setting the value to zero
    // then re-setting it to one to allow threads to wait again.

    // The value we want to set the semaphore to will be stored here
    semun value;

    // First set the semaphore to 0
    value.val = 0;

    if (-1 == semctl(semid, semnum, SETVAL, value))
    {
        // Failed to zero out the semaphore. We ought to bail out here
        return;
    }

    // Now set the semaphore back to 1
    value.val = 1;

    semctl(semid, semnum, SETVAL, value);
}
#endif

/****************************************************************************************/
void SignalChange(HKEY hkey) {

	PKEY key;

	if ((key=isKeyValid(hkey))==NULL)
		return;

#ifdef LINUX

    // For the key that actually changed, we notify waiters who care about
    // changes to that key alone.
    if (-1 != key->localNotifySemid)
    {
        NotifySemaphore( key->localNotifySemid, key->localNotifySemnum );
    }

    // Then we'll walk up the parent chain and notify waiters who care about
    // changes to subkeys as well.
    while (NULL != key)
    {
        if (-1 != key->subkeyNotifySemid)
        {
            NotifySemaphore( key->subkeyNotifySemid, key->subkeyNotifySemnum );
        }
        
        key = isKeyValid( key->parent );
    }

#else // not LINUX

	// Begin port from changelist 1907204 to Netware trunk.
	if (key->mysema==TRUE) {
		if (key->sema) {
			NTxSemaClose(key->sema);
			key->sema=0;
			}
		key->mysema=FALSE;
		}
	// End port from changelist 1907204 to Netware trunk.

#endif // else not LINUX
  
	CHANGE_REG;
}
/****************************************************************************************/
DWORD _RegOpenKey(HKEY base,LPCSTR okey,HKEY *out,BOOL create) {

	PKEY kbase,cur,first,last;
	LPSTR q;
	LPSTR str;
	HKEY hlast;
	char *key = 0;
	BOOL realloced=FALSE;

// Begin port from Changelist 1907204 to Netware Trunk.  The port replaced used of RETURN with return and removed the one use of ENTER;
CHANGE_REG;

	if ((kbase=isKeyValid(base))==NULL)
		return(errval);


// if the subkey name is null or empty then return the handle that was passed in
	if (!okey || !strlen(okey)) { 
		*out=base;
		return (ERROR_SUCCESS);
	}

    key = strdup( okey );
    if ( !key )
        return(ERROR_NO_MEMORY);

	q = strtok(key,"\\");
	cur = kbase;
	while (q) {
        if ( strlen(q) > 255 ) {
            if ( key )
                free(key);
            return(ERROR_INVALID_PARAMETER);
        }
		last = cur;
		first = cur = GetKey(cur->child);
		while (cur) {
			str = GetStrn(cur->name);
			if (!str) {
                if ( key )
                    free(key);
				return(ERROR_BAD_DATABASE);
            }
			if (!stricmp(q,str))
				break;
			last = cur;
			cur = GetKey(cur->sibling);
		}
		if (!cur) {
			if (!create) {
                if ( key )
                    free(key);
				return(ERROR_NO_PATH);
            }
			hlast = KEYINDEX(last); // we need this in case rMakeKey realloc's RootKey
			cur = rMakeKey(q,&realloced);
			if (!cur) {
                if ( key )
                    free(key);
				return(ERROR_NO_MEMORY);
            }
			if(realloced)
				if ((kbase=isKeyValid(base))==NULL) {
                    if ( key )
                        free(key);
					return(errval);
                }
            SignalChange(base);				// A subkey has changed so we may need to signal
			last=GetKey(hlast);
			if (last) {
				if (first)
                {
					last->sibling = KEYINDEX(cur);
#ifdef LINUX
                    cur->parent = last->parent;
#endif
                }
				else
                {
					last->child = KEYINDEX(cur);
#ifdef LINUX
                    cur->parent = KEYINDEX(last);
#endif
                }
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

    if ( key )
        free(key);

	if (out)
		*out = KEYINDEX(cur);
	cur->open++;
CHANGE_REG;
	return(ERROR_SUCCESS);
// End port from Changelist 1907204 to Netware Trunk.
}
/****************************************************************************************/
DWORD RegOpenKeyEx(HKEY base,LPCSTR lpkey,DWORD res,DWORD sec,HKEY *out) {

	PKEY key;

	REF(res);
	REF(sec);
	
	if ( (lpkey == NULL) || (*lpkey == '\0') ) {

	// Use of ENTER changed in porting changelist 1907204 to Netware Trunk.
	ENTER (ERROR_BADDB);


		if ((key=isKeyValid(base))==NULL)
			RETURN(errval);

		if (out)
			*out = KEYINDEX(key);

		key->open++;

		RETURN(ERROR_SUCCESS);
		}

	
	// A 4th parameter of FALSE was removed for the function RegOpenKey in porting changelist 1907204 to Netware Trunk.
	return RegOpenKey(base,lpkey,out);
}
/****************************************************************************************/
DWORD RegOpenKey(HKEY base,LPCSTR key,HKEY *out) {
// Begin port from changelist 1907204 to Netware Trunk.
	DWORD	retVal;


	if (key == NULL || *key == NULL) {
		if (out)
			*out = base;
		return ERROR_SUCCESS;
		}

	ENTER( NULL );
	retVal = _RegOpenKey(base,key,out,FALSE);
	RETURN(retVal);
// End port from changelist 1907204 to Netware Trunk.
}
/***********************************************************************************************/
DWORD RegCreateKey(HKEY base,LPCSTR key,HKEY *out) {
// Begin port from changelist 1907204 to Netware Trunk.
	DWORD	retVal;

	DIRTY;

	ENTER( NULL );
	retVal = _RegOpenKey(base,key,out,TRUE);
	RETURN(retVal);
// End port from changelist 1907204 to Netware Trunk.
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
// Begin port from changelist 1907204 to Netware Trunk.
	DWORD	retVal;

	DIRTY;

	ENTER( NULL );
	retVal = _RegOpenKey(hKey,lpSubKey,phkResult,TRUE);
	RETURN(retVal);
// End port from changelist 1907204 to Netware Trunk.

}
/****************************************************************************************/
static PVALUE MakeValue(LPCSTR name) {

	int i;
	PVALUE NewRootValue;
	int TGID;

	// Use of BLOCK; & UNBLOCK; removed as part of port from Changelist 1907204 to the Netware Trunk.
	for ( i = 1; i < static_cast<int>(maxvalue); i++ )
		if (!RootValue[i].used) {
			RootValue[i].name = MakeStr(name);
			if (!RootValue[i].name) {
				return NULL;
			}

			RootValue[i].used = 1;
			RootValue[i].dirty = 1;
			return &RootValue[i];
		}
		
	TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));
TA(	NewRootValue = (VALUE*) realloc(RootValue,(maxvalue+32)*sizeof(VALUE)));
	SetThreadGroupID(INT_TO_OS_TGID(TGID));
	if (NewRootValue) {
		RootValue = NewRootValue;
		memset(&RootValue[maxvalue],0,32*sizeof(VALUE));

		RootValue[maxvalue].name = MakeStr(name);
		if (!RootValue[maxvalue].name) {
			return NULL;
		}
		RootValue[maxvalue].used = 1;
		RootValue[maxvalue].dirty = 1;

		maxvalue += 32;
		return &RootValue[maxvalue-32];
	}
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
	PVALUE first;
	PVALUE cur;
	PVALUE last = NULL;
	LPSTR str;
	HVALUE hlast;

	first = cur = GetValue(key->value);

	// TODO check if we need to return when cur is NULL at this point

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

	// Use of BLOCK; & UNBLOCK; removed as part of port from Changelist 1907204 to the Netware Trunk.

	for (i = 1; i < static_cast<int>(maxdata); i++ )
		if (!RootData[i].used) {
			RootData[i].used = 1;
			RootData[i].dirty = 1;
//analyze data type
            switch ( type )
            {
                case REG_DWORD:
					memcpy(RootData[i].data,data,min(static_cast<size_t>(len), sizeof(RootData[i].data)));
                    break;
                case REG_SZ:
		  			if(strlen((const char*) data)<sizeof(RootData[i].data))
		  				memcpy(RootData[i].data,data,strlen((const char*) data));
		  			else
		  				memcpy(RootData[i].data,data,sizeof(RootData[i].data));
                    break;
                case REG_BINARY:
					memcpy(RootData[i].data,data,min(static_cast<size_t>(len), sizeof(RootData[i].data)));
                    break;
                default:
					memcpy(RootData[i].data,data,min(static_cast<size_t>(len), sizeof(RootData[i].data)));
            }

			return &RootData[i];
		}

	TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));
TA(	NewRootData = (DATA*) realloc(RootData,(maxdata+32)*sizeof(DATA)));
	SetThreadGroupID(INT_TO_OS_TGID(TGID));
	if (NewRootData) {
		RootData = NewRootData;
		memset(&RootData[maxdata],0,32*sizeof(DATA));
		RootData[maxdata].used = 1;
		RootData[maxdata].dirty = 1;
		memcpy(RootData[maxdata].data,data,min(static_cast<size_t>(len), sizeof(RootData[maxdata].data)));
		maxdata += 32;
		return &RootData[maxdata-32];
	}

	return NULL;
}

/***************************************************************************************/
int ReadData(HDATA node,BYTE *data,int len) {

	PDATA cur = Get_Data(node);
	int i,LenRead=0;

	if (!cur)
		return 0;

	while (len && cur) {
		i = min(static_cast<size_t>(len), sizeof(cur->data));
		memcpy(data, cur->data, i);
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
		i = min(sizeof(cur->data), static_cast<size_t>(len));
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

// Use of ENTER changed as part of port from changelist 1907204 to Netware Trunk.
ENTER (ERROR_BADDB);
CHANGE_REG;
DIRTY;

    REF(r);

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
    SignalChange(hkey);						// A subkey has changed so we may need to signal
CHANGE_REG;
	RETURN(ERROR_SUCCESS);
}
/***************************************************************************************/
DWORD RegQueryValueEx(HKEY hkey,LPCSTR value,DWORD *r,DWORD *type,BYTE *data,DWORD *cbdata)
{
	PKEY key;
	PVALUE val;

	r=r;

// Use of ENTER changed as part of port from changelist 1907204 to Netware Trunk.
ENTER (ERROR_BADDB);


    DWORD rc = ERROR_SUCCESS;

    key = isKeyValid(hkey);
	if(NULL == key)
        rc = errval;
    else
    {
        if(!value)
            value = "(NAMELESS)";

        val = FindValue(key,value,FALSE);
        if (NULL == val)
            rc = ERROR_NO_PATH;
        else
        {
            if(type)
                *type = val->type;

            if(cbdata)
            {
                if(data)
                {
                    if(*cbdata < val->len)
                        rc = ERROR_MORE_DATA;
                    else
                        ReadData(val->data,data,val->len);
                }
                *cbdata = val->len;
			}
            else if(data)
                rc = ERROR_INVALID_PARAMETER;
		}
	}

	RETURN(rc);
}
/***************************************************************************************/
DWORD RegEnumValue(HKEY hkey, DWORD index, LPSTR value, DWORD *cbvalue,
                   DWORD *r, DWORD *type, BYTE *data, DWORD *cbdata)
{
	PKEY key;
	PVALUE val;
	int i;
	int len;
	LPSTR str;

	REF(r);

// Use of ENTER changed as part of port from changelist 1907204 to Netware Trunk.
ENTER (ERROR_BADDB);
	
    DWORD rc = ERROR_SUCCESS;

    key = isKeyValid(hkey);
	if(NULL == key)
		rc = errval;
    else
    {
        val = GetValue(key->value);
        for ( i = 0; val && i < static_cast<int>(index); i++ )
            val = GetValue(val->value);

        if (!val)
            rc = ERROR_NO_MORE_ITEMS;
        else
        {
            bool done = false;
        
            if (cbvalue)
            {
                str = GetStrn(val->name);
                if (!str)
                {
                    rc = ERROR_BAD_DATABASE;
                    done = true;
                }
                else
                {
                    len = strlen(str)+1;
                    if ( static_cast<int>(*cbvalue) < len)
                    {
                        done = true;
                        if (value)
                            rc = ERROR_MORE_DATA;
                        else
                            *cbvalue = len;
                    }
                    else
                    {
                        *cbvalue = len;

                        if (value)
                            strcpy(value,str);
                    }
                }
            }
            else if(value)
            {
                rc = ERROR_INVALID_PARAMETER;
                done = true;
            }

            if(!done)
            {
                if (type)
                    *type = val->type;

                if (cbdata)
                {
                    if (data)
                    {
                        if (*cbdata < val->len)
                            rc = ERROR_MORE_DATA;
                        else
                            ReadData(val->data,data,val->len);
                    }
                    *cbdata = val->len;
                }
                else if(data)
                    rc = ERROR_INVALID_PARAMETER;
            }
        }
    }
    
	RETURN(rc);
}
/***************************************************************************************/
DWORD RegEnumKey(HKEY  hkey,DWORD index,LPSTR key,DWORD cbkey) {
	return RegEnumKeyEx(hkey,index,key,&cbkey,NULL,NULL,NULL,NULL);
}

/***************************************************************************************/
DWORD RegEnumKeyEx(HKEY hkey, DWORD index, LPSTR pkey, DWORD *cbkey, DWORD *r, LPSTR Class,
	DWORD *cbClass, PFILETIME time) {

	PKEY key;
	int i,len;
	LPSTR str;

	REF(r);
	REF(time);

// Use of ENTER changed as part of port from changelist 1907204 to Netware Trunk.
ENTER (ERROR_BADDB);

    DWORD rc = ERROR_SUCCESS;
 
    key = isKeyValid(hkey);
	if (NULL == key)
        rc = errval;
    else
    {
        key = GetKey(key->child);
        for ( i = 0; key && i < static_cast<int>(index); i++ )
            key = GetKey(key->sibling);

        if (!key)
            rc = ERROR_NO_MORE_ITEMS;
        else
        {
            bool done = false;
        
            if (cbkey)
            {
                str = GetStrn(key->name);
                if (!str)
                {
                    rc = ERROR_BAD_DATABASE;
                    done = true;
                }
                else
                {
                    len = strlen(str)+1;
                    if ( static_cast<int>(*cbkey) < len )
                    {
                        done = true;
                        if (pkey)
                            rc = ERROR_MORE_DATA;
                        else
                            *cbkey = len;
                    }
                    else
                    {
                        *cbkey = len;

                        if (pkey)
                            strcpy(pkey,str);
                    }
                }
            }
            else
            {
                rc = ERROR_INVALID_PARAMETER;
                done = true;
            }
    
            // cbClass can be NULL only if Class is NULL.
            if(!done && !cbClass && Class)
                rc = ERROR_INVALID_PARAMETER;
        }
    }

	RETURN(rc);
}

// Begin port from changelist 1907204 to Netware Trunk.
/***************************************************************************************/
DWORD _RegFlushKey(HKEY hkey) {

	PKEY key;

	if ((key=isKeyValid(hkey))==NULL)
		return (errval);

	if (DataBaseDirty)
		return (_RegSaveDatabase());

	return (ERROR_SUCCESS);
}
/***************************************************************************************/
DWORD RegFlushKey( HKEY hkey )
{
	DWORD	retVal;

	ENTER (ERROR_BADDB);
	retVal = _RegFlushKey(hkey);
	RETURN(retVal);
}

// End port from changelist 1907204 to Netware Trunk.

/***************************************************************************************/
void FreeStr(HSTR str) {

	LPSTR s = GetStrn(str);
	int TGID;

	if (s) {
		if (StringTable[str]) {
			TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));
			free(StringTable[str]);
			SetThreadGroupID(INT_TO_OS_TGID(TGID));
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

// Use of ENTER changed as part of port from changelist 1907204 to Netware Trunk.
ENTER (ERROR_BADDB);
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
        SignalChange(hkey);
#ifdef LINUX
        if (-1 != key->localNotifySemid)
        {
            // Remove the semaphore from the key.
            semctl(key->localNotifySemid, 0, IPC_RMID, 0);
        }

        if (-1 != key->subkeyNotifySemid)
        {
            // Remove the semaphore from the key.
            semctl(key->subkeyNotifySemid, 0, IPC_RMID, 0);
        }

        // no need to set the semid members to -1 here, the whole key is
        // about to be obliterated
#endif // LINUX
		FreeAllValues(key->value);
		FreeStr(key->name);
		memset(key,0,sizeof(KEY));
	}
}
/************************************************************************************/
DWORD RegDeleteKey(HKEY base,LPCSTR skey) {

	PKEY key,cur,last=0;
	LPSTR str;

// Use of ENTER changed as part of port from changelist 1907204 to Netware Trunk.
ENTER (ERROR_BADDB);
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
                SignalChange(base);				// A subkey has changed so we may need to signal
				RETURN(ERROR_SUCCESS);
			}
//			FreeAllValues(cur->value);
			DeleteKey(KEYINDEX(cur));
            SignalChange(base);				// A subkey has changed so we may need to signal
			RETURN(ERROR_SUCCESS);
		}
		last = cur;
		cur = GetKey(cur->sibling);
	}

	RETURN(ERROR_NO_PATH);
}

// Begin port from changelist 1907204 to Netware Trunk.

/***************************************************************************************/
DWORD _RegCloseKey(HKEY hkey) {

	PKEY key;
	DWORD retval=ERROR_SUCCESS;

CHANGE_REG;

	if ((key=isKeyValid(hkey))==NULL)
		return(errval);

	key->open--;

// Removes excessive file writes.
#ifdef FLUSH_ON_CLOSE 

	retval = RegFlushKey(KEYINDEX(key));

#endif // FLUSH_ON_CLOSE 

	if ((key->open<1) && key->delwhenclose)
		DeleteKey(KEYINDEX(key));
	else
    {
        SignalChange(hkey);
#ifdef LINUX
        if(key->open < 1)
        {
            if (-1 != key->localNotifySemid)
            {
                // Delete the key's change notify semaphore.
                semctl(key->localNotifySemid, 0, IPC_RMID, 0);
                key->localNotifySemid = -1;
                key->localNotifySemnum = 0;
            }

            if (-1 != key->subkeyNotifySemid)
            {
                // Delete the key's change notify semaphore.
                semctl(key->subkeyNotifySemid, 0, IPC_RMID, 0);
                key->subkeyNotifySemid = -1;
                key->subkeyNotifySemnum = 0;
            }
        }
#endif // LINUX
    }
            
	return(retval);
}
/***************************************************************************************/
DWORD RegCloseKey( HKEY hkey )
{
	DWORD	retVal;
	ENTER (ERROR_BADDB);
	retVal = _RegCloseKey(hkey);
	RETURN(retVal);
}

// End port from changelist 1907204 to Netware Trunk.


/****************************************************************************************/
void CloseAll(void)
{
	int i;

	if (RootKey)
	{
		for ( i = 0; i < static_cast<int>(maxkey); i++ )
		{
			// Begin port from changelist 1907204 to Netware Trunk.
			if (RootKey[i].used)
			{
				if (RootKey[i].open)
					_RegCloseKey((HKEY)i);
				if (RootKey[i].sema) {
					SignalChange ((HKEY)i);
					RootKey[i].sema=0;
			}
			// End port from changelist 1907204 to Netware Trunk.

		}
	}
}
/****************************************************************************************/
DWORD RegCloseDatabase(BOOL Save) {

	// Begin port from changelist 1907204 to Netware Trunk.

	// This is a form of ENTER()  Thus the matching RETURN....
	if (registryAvailable && !Lock(TRUE))
		return(ERROR_BADDB);

CHANGE_REG;
DIRTY;
	dprintf ("Close database\n");

	if (Save) {
dprintf ("closeall\n");
		CloseAll();
dprintf ("save data\n");
		_RegSaveDatabase();		// Call the unprotected version.  If we are shutting down, don't lock.  If we aren't shutting down then we already locked in this function.
	}

	dprintf ("ClearArrays\n");
	ClearArrays();
	dprintf ("done\n");

	if (!registryAvailable)		// If we are shutting down, don't unlock because we didn't lock see above.
		return ERROR_SUCCESS;

	RETURN(ERROR_SUCCESS);

	// End port from changelist 1907204 to Netware Trunk.

}
/*****************************************************************************************/
DWORD RegNotifyChangeKeyValue(HKEY  hkey,BOOL  fWatchSubTree,DWORD  fdwNotifyFilter, HANDLE  hEvent,BOOL  fAsync) {

// For Linux, then function does nothing.  The Linux registry implementation
// creates the semaphore and saves it on the key in the server without calling
// this method.  All other semaphore operations are handled within this module.
#ifndef LINUX

	PKEY key;

	fWatchSubTree=fWatchSubTree;
	fdwNotifyFilter=fdwNotifyFilter;

// Use of ENTER changed as part of port from changelist 1907204 to Netware Trunk.
ENTER (ERROR_BADDB);

	if ((key=isKeyValid(hkey))==NULL)
		RETURN(errval);

	if (fAsync==FALSE) {
		if (key->sema == 0) {
			key->sema = reinterpret_cast<int>(NTxSemaOpen(0, NULL));
			key->mysema=TRUE;
			}
		// Use of RETURN added as part of port from changelist 1907204 to Netware Trunk.
		RETURN(NTxSemaWait(key->sema));

		}
	else {
		if (key->sema)
			RETURN(ERROR_SEMAPHORE_IN_USE);
		key->sema = reinterpret_cast<int>(hEvent);
		key->mysema=FALSE;
		}
	RETURN(ERROR_SUCCESS);

#else
    return ERROR_SUCCESS;
#endif  // not LINUX
}

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

// Use of ENTER changed as part of port from changelist 1907204 to the Netware Trunk.
ENTER (FALSE);

	TGID = (int)SetThreadGroupID(INT_TO_OS_TGID(RegTGID));

	dprintf ("Performing Database Dump\t");

	sssnprintf (Dir,DirBytes,REG_DUMP,Dir,DumpFileNum);

	fp = fopen (Dir,"w");

	if (!fp) {
		SetThreadGroupID(INT_TO_OS_TGID(TGID));
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
	K = 0;
	StrCopy (kPath[0],"Root");
	Ell = 1;
	fprintf (fp,"Key: /Root\n");
	DumpSubKey(dumpkey);
	fclose (fp);

	SetThreadGroupID(INT_TO_OS_TGID(TGID));
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
					sssnprintf (valstr,sizeof(valstr),"(DWORD):0x%08x = %d", static_cast<int>(*(DWORD*)data), static_cast<int>(*(DWORD*)data));
					break;
				case REG_BINARY:
					sssnprintf (valstr, sizeof(valstr), "(BINARY(%d bytes)):", static_cast<int>(dsz));
					for (i=0 ; i < static_cast<int>(dsz) && NumBytes(valstr) < 256 ; i++) {
						sssnprintf (buf,sizeof(buf),"%02x ",data[i]);
						strcat (valstr,buf);
					}
					if (i < static_cast<int>(dsz))
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
		StrCopy (kPath[Ell], kName);
		fprintf (fp,"\nKey: ");
		for (j = 0; j <= static_cast<DWORD>(Ell); j++) {
			fprintf (fp,"/%s",kPath[j]);
		}
		fprintf (fp,"\n");
		if (RegOpenKey (hkey,kName,&hsub)==ERROR_SUCCESS) {
			Ell++;
			DumpSubKey (hsub);
			Ell--;
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
	if (!fp)
		return static_cast<DWORD>(-1);

	fprintf (fp,"Starting Database Check at %s",ctime(&t));
	fprintf (fp,"Checking Keys\n");
	for (i = 0; i < static_cast<int>(maxkey); i++) {
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
			if (RootKey[i].child == reinterpret_cast<HKEY>(i)) {
				RecursiveChild++;
				fprintf (fp,"\tRecursive Child: %d\n",i);
			}
			if (RootKey[i].sibling == reinterpret_cast<HKEY>(i)) {
				RecursiveSibling++;
				fprintf (fp,"\tRecursive Sibling: %d\n",i);
			}
		}
	}

	fprintf (fp,"----------------------\n");
	fprintf (fp,"Checking Values\n");
	for (i = 0; i < static_cast<int>(maxvalue); i++) {
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
	for (i = 0; i < static_cast<int>(maxdata); i++) {
		if (RootData[i].used) {
			if (RootData[i].link == static_cast<HDATA>(i)) {
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
// This function added as part of port from changelist 1907204 to the Netware Trunk.
DWORD RegQueryInfoKey(
  HKEY hKey,                      // handle to key
  LPTSTR lpClass,                 // class buffer
  LPDWORD lpcClass,               // size of class buffer
  LPDWORD lpReserved,             // reserved
  LPDWORD lpcSubKeys,             // number of subkeys
  LPDWORD lpcMaxSubKeyLen,        // longest subkey name
  LPDWORD lpcMaxClassLen,         // longest class string
  LPDWORD lpcValues,              // number of value entries
  LPDWORD lpcMaxValueNameLen,     // longest value name
  LPDWORD lpcMaxValueLen,         // longest value data
  LPDWORD lpcbSecurityDescriptor, // descriptor length
  PFILETIME lpftLastWriteTime     // last write time
)
{
    PKEY    internalKeyPtr              = NULL;
    int     noSubKeys                   = 0;
    int     noValues                    = 0;
    int     noKeys                      = 0;
    int     maxValueNameLength          = 0;
    int     maxValueDataLength          = 0;
    int     maxSubkeyNameLength         = 0;
    PKEY    currKey                     = NULL;
    PVALUE  currValue                   = NULL;
    LPSTR   nameBuffer                  = NULL;
    int     nameLength                  = 0;
    DWORD   returnVal                   = ERROR_FILE_NOT_FOUND;
    
    // Validate parameters
    if ((lpClass == NULL) && (lpcClass == NULL) && (lpcSubKeys == NULL) && (lpcMaxSubKeyLen == NULL) && (lpcMaxClassLen == NULL) && (lpcValues == NULL) && (lpcMaxValueNameLen == NULL) && (lpcMaxValueLen == NULL) && (lpcbSecurityDescriptor == NULL) && (lpftLastWriteTime == NULL))
        return ERROR_INVALID_PARAMETER;
    
    // Lock registry database
    ENTER (ERROR_BADDB);

    // Validate key handle
    internalKeyPtr = isKeyValid(hKey);
    if (internalKeyPtr == NULL)
        RETURN(errval);
   
    // Obtain the requested information
    returnVal = ERROR_SUCCESS;
    // Do we need to collect subkey information?
    if ((lpcSubKeys != NULL) || (lpcMaxSubKeyLen != NULL))
    {
        // Yes.
        currKey = GetKey(internalKeyPtr->child);
        while( currKey != NULL )
        {
            noKeys++;

            nameBuffer = GetStrn(currKey->name);
            nameLength = 0;
            if (nameBuffer != NULL)
                nameLength = strlen(nameBuffer);
            if (nameLength > maxSubkeyNameLength)
                maxSubkeyNameLength = nameLength;

            currKey = GetKey(currKey->sibling);
        }
    }
    // Do we need to collect value information?
    if (((lpcValues != NULL) || (lpcMaxValueNameLen != NULL) || (lpcMaxValueLen != NULL)) && (returnVal == ERROR_SUCCESS))
    {
        // Yes.
        currValue = GetValue(internalKeyPtr->value);
        while( currValue != NULL )
        {
            noValues++;

            nameBuffer = GetStrn(currValue->name);
            nameLength = 0;
            if (nameBuffer != NULL)
                nameLength = strlen(nameBuffer);
            if (nameLength > maxSubkeyNameLength)
                maxSubkeyNameLength = nameLength;

            if (currValue->len > maxValueDataLength)
                maxValueDataLength = currValue->len;
            currValue = GetValue(currValue->value);
        }
    }
    if (lpcSubKeys != NULL)
        *lpcSubKeys = noKeys;
    if (lpcMaxSubKeyLen != NULL)
        *lpcMaxSubKeyLen = maxSubkeyNameLength;
    if (lpcValues != NULL)
        *lpcValues = noValues;
    if (lpcMaxValueNameLen != NULL)
        *lpcMaxValueNameLen = maxValueNameLength;
    if (lpcMaxValueLen != NULL)
        *lpcMaxValueLen = maxValueDataLength;
    
    // Trivial - the NetWare emulated registry ignores key classes, security descriptors, and does not maintain a last write time
    if ((lpClass != NULL) && (returnVal == ERROR_SUCCESS))
    {
        if (lpcClass != NULL)
        {
            if (*lpcClass > 0)
            {
                *lpcClass = 0;
                lpClass[0] = NULL;
            }
            else
            {
                returnVal = ERROR_MORE_DATA;
            }
        }
        else
        {
            returnVal = ERROR_MORE_DATA;
        }
    }
    if ((lpcClass != NULL) && (returnVal == ERROR_SUCCESS))
        *lpcClass = 0;
    if ((lpcMaxClassLen != NULL) && (returnVal == ERROR_SUCCESS))
        *lpcMaxClassLen = 0;
    if ((lpcbSecurityDescriptor != NULL) && (returnVal == ERROR_SUCCESS))
        *lpcbSecurityDescriptor = NULL;
    if ((lpftLastWriteTime != NULL) && (returnVal == ERROR_SUCCESS))
        (*(DWORD *)lpftLastWriteTime) = NULL;

    // Release lock, return result
    RETURN(returnVal);
}

/****************************************************************************************/
BOOL RegReady(void)
{
	return registryAvailable;
}
  
/****************************************************************************************/
BOOL RawDumpDatabase(char *Dir,size_t DirBytes) {

	BOOL retval;
	int TGID;

	
	// Port from changelist 1907204 to the Netware Trunk.
	// Use of ENTER changed, removed use of BLOCK; & UNBLOCK.
	ENTER(FALSE);

	TGID=SetThreadGroupID (RegTGID);
	//dprintf ("raw dump block\n");
		CheckDatabase(Dir);
		retval=DoDump(Dir,DirBytes);

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

	REF(inProcess);
	REF(outProcess);
	REF(access);
	REF(inherant);
	REF(options);

// Port from 1907204 to the Netware Trunk.
ENTER(FALSE);


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


#ifdef LINUX

/**
 * Write header to database file.
 * 
 * @param fd    [in] File descriptor to write to.
 * 
 * @return true is header successfully written, false if failure.
 */ 
bool WriteHeader(int fd)
{
    unsigned char header[HEADER_COOKIE_LEN + HEADER_VERSION_LEN];
    unsigned char* p = header + HEADER_COOKIE_LEN;

    memcpy(header, MAGIC_COOKIE, HEADER_COOKIE_LEN);

    // Pack bytes into header buffer to write to file.
    int shift = HEADER_VERSION_LEN * 8;
    for(int i = 0; i < HEADER_VERSION_LEN; ++i)
    {
        shift -= 8;
        p[i] = (0x0ff & ((FILE_VERSION) >> shift));
    }

    // Write header to file.
    int wanted = HEADER_COOKIE_LEN + HEADER_VERSION_LEN;
    int sent = safeWrite(fd, header, wanted);
    bool ret = (sent == wanted);
    return ret;
}

/**
 * Read header from database file.
 * 
 * @param fd        [in] File descriptor to read from.
 * @param version   [out] File version value.
 *
 * @return hdr_success if OK, hdr_read_failed if failed to read file,
 *         hdr_bad_file_id if magic value is not correct.
 */ 
RdHdrResult ReadHeader(int fd, unsigned long& version)
{
    unsigned char header[HEADER_COOKIE_LEN + HEADER_VERSION_LEN] = {0};
    RdHdrResult result = hdr_read_failed;
    int wanted = HEADER_COOKIE_LEN + HEADER_VERSION_LEN;

    version = 0;                // set to invalid value

    int got = safeRead(fd, header, wanted);
    if(got == wanted)
    {
        if(0 == memcmp(MAGIC_COOKIE, header, HEADER_COOKIE_LEN))
        {
            unsigned char* p = header + HEADER_COOKIE_LEN;
            int shift = HEADER_VERSION_LEN * 8;

            // Unpack bytes from header buffer read from file.
            for(int i=0; i < HEADER_VERSION_LEN; ++i)
            {
                shift -= 8;
                version |= (p[i] << shift);
            }
            result = hdr_success;
        }
        else
            result = hdr_bad_file_id;
    }

    return result;
}

#endif // LINUX

/**
 * Read all data requested or until EOF.
 *
 * Attempts to read count bytes of data from the descriptor fd into the buffer
 * pointed to by buf.  If count is zero and fd referrs to a regular file, 0 is
 * returned without causing any other effect.  Interruption in blocking I/O
 * (due to signals) and partial reads (fd does not reference normal file) do
 * not affect safeRead().  Reading continues until count bytes have been read,
 * there is no data remaining to be read, or an error occurs.
 *
 * @param fd        [in] File descriptor to read from.
 * @param buf       [in] Buffer to store data in.
 * @param count     [in] Number of bytes to read.
 * 
 * @return If successful, the number of bytes actually read and placed into the
 *         buffer.  Upon reading only end-of-file, zero is returned.  On error,
 *         -1 is returned and errno is set to indicate the error.
 */
int safeRead(int fd, void* buf, int count)
{
#ifdef LINUX

    int nread;

    char* ptr = reinterpret_cast<char*>(buf);
    int nleft = count;
    do
    {
        nread = read(fd, ptr, nleft);
        if(nread > 0)
        {
            nleft -= nread;
            ptr += nread;
        }
        else if( (nread < 0) && (EINTR == errno) )
            nread = 1; // cause re-loop
    } while(nleft > 0 && nread > 0);

    int ret = (nread >= 0 ? (count - nleft) : -1);

#else

    int ret = read(fd, buf, count);

#endif // LINUX

    return ret;
}

/**
 * Write all data requested.
 *
 * Attempts to write count bytes of data to the descriptor fd from the buffer
 * pointed to by buf.  If count is zero and fd referrs to a regular file, 0 is
 * returned without causing any other effect.  Interruption in blocking I/O
 * (due to signals) and partial writes (fd does not reference normal file) do
 * not affect safeWrite().  Writing continues until count bytes have been
 * written, or an error occurs.
 *
 * @param fd        [in] File descriptor to write to.
 * @param data      [in] Buffer containing data to write.
 * @param count     [in] Number of bytes to write.
 * 
 * @return If successful, the number of bytes actually written.  On error, -1
 *         is returned and errno is set to indicate the error.
 */
int safeWrite(int fd, const void* data, int count)
{
#ifdef LINUX

    int nwrote;

    const char* ptr = reinterpret_cast<const char*>(data);
    int nleft = count;
    do
    {
        nwrote = write(fd, ptr, nleft);
        if(nwrote > 0)
        {
            nleft -= nwrote;
            ptr += nwrote;
        }
        else if( (nwrote < 0) && (EINTR == errno) )
            nwrote = 1; // cause re-loop
    } while(nleft > 0 && nwrote > 0);

    int ret = (nwrote >= 0 ? (count - nleft) : -1);

#else

    int ret = write(fd, data, count);

#endif // LINUX

    return ret;
}

