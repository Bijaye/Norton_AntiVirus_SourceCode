// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#ifdef LINUX
	#include <sys/types.h>
	#include <unistd.h>

    #include "ndkDefinitions.h"
	#include "ndkFunctions.h"
#endif

#ifdef NLM
	#include <process.h>
	#include <io.h>
	#include <conio.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#if ! (defined NLM || defined LINUX)
	#include <share.h>  //  gdf CW conversion
#endif

#include "NLM.H"

#include "nwreg.h"
#include "vpcommon.h"

#include "nts.h"
#undef HANDLE
#include "symnts.h"
#undef HANDLE

#include "slash.h"

#ifdef LINUX

// semaphore includes
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
	HSTR name;
	char used;
	char open;
	char delwhenclose;
	char dirty;
	HVALUE value;	
	HKEY sibling;
	HKEY child;
#ifndef LINUX
	DWORD sema;
	BOOL mysema;
#else
    // we don't want to change the NetWare registry file format, so this parent
    // key is only for linux, even though the information is not Linux specific
    HKEY parent;
    // semid and number for notification on this key only
    int localNotifySemid;
    int localNotifySemnum;
    // semid and number for notification for this key and subkeys
    int subkeyNotifySemid;
    int subkeyNotifySemnum;
#endif
} KEY,FAR *PKEY;

typedef struct {
	HSTR name;
	HVALUE value;
	char used;
	char dirty;
	WORD x;
	DWORD type;
	DWORD len;
	HDATA data;
} VALUE,FAR *PVALUE;

typedef struct {
	HDATA link;
	char used;
	char dirty;
	WORD x;
	BYTE  data[32];
} DATA,FAR *PDATA;


#define ERROR_FILE_NOT_FOUND		2L		//
//#define ERROR_ACCESS_DENIED			5L		//
#define ERROR_OUTOFMEMORY			14L		//
#define ERROR_SEM_IS_SET			102L	//
#define ERROR_BADDB					1009L	//
#define ERROR_BADKEY				1010L	//
#define ERROR_CANTWRITE				1013L	//
#define ERROR_REGISTRY_CORRUPT		1015L	//
#define ERROR_REGISTRY_IO_FAILED	1016L	//

//Flags for RegNotifyChangeKeyValue fdwNotifyFilter
//#define REG_NOTIFY_CHANGE_NAME		(0x00000001L) // Create or delete (child)
//#define REG_NOTIFY_CHANGE_ATTRIBUTES	(0x00000002L)
//#define REG_NOTIFY_CHANGE_LAST_SET		(0x00000004L) // time stamp
//#define REG_NOTIFY_CHANGE_SECURITY	(0x00000008L)

#define VPstrncpy _VPstrncpy
char *_VPstrncpy(char *d,const char *s,int n);

int MySetFileInfo(const char *file,const struct stat *stbuf);

 DWORD RegOpenKey(HKEY base,LPCSTR key,PHKEY out);
 DWORD RegCreateKey(HKEY base,LPCSTR key,PHKEY out);
 DWORD RegCreateKeyEx(HKEY    hKey,
                     LPCTSTR lpSubKey,
                     DWORD   Reserved,
                     LPTSTR  lpClass,
                     DWORD   dwOptions,
                     DWORD   samDesired,
                     DWORD  *lpSecurityAttributes,
                     PHKEY   phkResult,
                     DWORD  *lpdwDisposition);
 DWORD RegDeleteKey(HKEY base,LPCSTR key);
 DWORD RegDeleteValue(HKEY base,LPCSTR value);
 DWORD RegCloseKey(HKEY hkey);
 DWORD RegSetValueEx(HKEY hkey,LPCSTR value,DWORD reserved,DWORD type,const BYTE* data,DWORD cbdata);
 DWORD RegQueryValueEx(HKEY hkey,LPCSTR value,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
 DWORD RegEnumValue(HKEY  hkey,DWORD index,LPSTR value,LPDWORD cbvalue,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
 DWORD RegEnumKeyEx(HKEY  hkey,DWORD index,LPSTR key,LPDWORD cbkey,LPDWORD reserved,LPSTR Class,LPDWORD cbClass,PFILETIME time);
 DWORD RegEnumKey(HKEY  hkey,DWORD index,LPSTR key,DWORD cbkey);
 DWORD RegFlushKey(HKEY hkey);
 DWORD RegCloseDatabase(BOOL);
 DWORD RegOpenDatabase(void);
 DWORD RegSaveDatabase(void);
 DWORD RegNotifyChangeKeyValue(HKEY hkey,BOOL fWatchSubTree,DWORD fdwNotifyFilter,HANDLE  hEvent,BOOL  fAsync);
 DWORD DuplicateHandle(DWORD inProcess,HKEY in,DWORD outProcess,HKEY *out,DWORD access,BOOL inherant,DWORD options);
DWORD RegOpenKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY *phkResult);
BOOL RawDumpDatabase(char *,size_t);
BOOL FileDumpDatabase(HKEY,char *,size_t);

 #define dprintf 	if (gVPRegDebug) Real_dprintf

 //REGEDIT.C
 void EditRegistry(void *);
 void regCloseConsoleScreen();
 void StopRegistryEditor();

 //REG.C
 extern int DumpFileNum; 			// used to serialize db dumps.  Reset to 0 on NLM load
 extern BOOL RegChanged;
 extern int RegEditThreadGroup;

 extern char gVPRegDebug;
 extern char ProgramDir[255];	   // directory path where rtvscan.nlm was run from (including volume)
 extern char RegistryRunning;
 extern DWORD RegThreadCount;
 extern BOOL ExitingNormally;
 extern int regNWversion;
 extern char **RegStringTable;
 extern int RegTGID;
 extern char StartRegistryEditor;
 extern BOOL RegChanged;

 void UnloadRegistry (void);
 void ReleaseTheHounds (void);

#define RAW_REG_DUMP "%s" sSLASH "vp_Raw%02d.txt"
#define REG_DUMP "%s" sSLASH "vp_Reg%02d.txt"
#define REG_CHECK "%s" sSLASH "vp_Chk%02d.txt"

 #define ROOT_KEY_LOC		1

 #define KEYINDEX(x)	(HKEY)(x-RootKey)
 #define VALUEINDEX(x)	(HVALUE)(x-RootValue)
 #define DATAINDEX(x)	(HDATA)(x-RootData)

 #define MAXALLOWEDKEYS			2000
 #define MAXALLOWEDVALUES		20000
 #define MAXALLOWEDDATA			40000
 #define MAXALLOWEDSTRINGS		30000
 #define MAXALLOWEDSTRINGSIZE	1024
 #define KEY_ALL_ACCESS 		0

 #define CURR parentKey[openKey]		// the currently open key

 #define ERROR_VAL_NAME		9
 #define ERROR_BAD_TYPE		10
 #define ERROR_CLOSE_ROOT	11

// the following are used in RegCommandMenu
 #define NUM_ADD_KEY		1
 #define NUM_ADD_VALUE		2
 #define NUM_DELETE_KEY		3
 #define NUM_DELETE_VALUE	4
 #define NUM_OPEN_SUBKEY	5
 #define NUM_CLOSE_KEY		6
 #define NUM_SHOW_DATA		7
 #define NUM_EDIT_DATA		8
 #define NUM_QUIT_PROG		9

 #define CMD_ESCAPE			255

 #define LEFT		2				// The left text margin
 #define RIGHT		79				// The right margin
 #define MIDDLE		40				// The middle of the screen

 #define KEYPATHY	1				// row where the key path is printed
 #define SUBKEYY	2				// row where subkeys and values begin printing
 #define COMY		20				// row where commands begin listing
 #define STATY		23				// row where status is printed
 #define PROMPTY	23				// row where question prompts are printed

 #define KEYPATH_XY	LEFT,KEYPATHY		// location where key path is printed
 #define SUBKEY_XY 	LEFT,SUBKEYY		// Location where subkeys begin printing
 #define VALUE_XY 	MIDDLE+1,SUBKEYY	// location where values begin printing
 #define COM_XY		LEFT,COMY			// location where commands start printing
 #define STAT_XY	LEFT,STATY			// location where status is printed
 #define PROMPT_XY 	LEFT,PROMPTY		// location where question prompts are printed
 #define PROMPT_YX	PROMPTY,LEFT		// location of prompt in row/column order

 #define KEYLEN		128				// Max length of Key names
 #define VALLEN		128				// Max length of Value names
 #define DATALEN	256				// Max length of value data
 #define KEYPATHLEN	32				// Max depth of key path

 typedef struct ValueStruct {
	char name[VALLEN];
	DWORD type;
 } Value;

 void ClearStrings (void);			// Zeros out the subKeys and values arrays
 void PrintSubkeys (void);
 void PrintValues (void);

VOID Real_dprintf(const char* format,...);

#ifdef __cplusplus
}
#endif

#endif //_REG_H_
