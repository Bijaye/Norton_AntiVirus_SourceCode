// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.

#ifndef _libcnlm_h_
#define _libcnlm_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <library.h>
#include <dlfcn.h>
#include <sys/filio.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <libcclib.h>
#include <string.h>

#include "winBaseTypes.h"

#ifndef GetNLMHandle
#define GetNLMHandle getnlmhandle
#endif

char ScsCommsDir[255] = { '\0' };	   // directory path where scscomms.nlm was run from (including volume)
#define NW_SYSTEM_DIR			"SYS:\\SYSTEM"
#ifndef IMAX_PATH
#define IMAX_PATH PATH_MAX
#endif

//
// SID
//
#ifndef SID_SIZE
	#define SID_SIZE 64
#endif
typedef struct {
	ULONG stuff[SID_SIZE];
	} mSID;
typedef struct {
	DWORD ConnectionID;
	char UserName[sizeof(mSID)-sizeof(DWORD)];
	} *PSID;


#ifndef Sleep
#define Sleep delay
#endif
int delay (unsigned int milliseconds); // from #include <unistd.h>

typedef struct {
	BYTE Version;
	BYTE Command;
	BYTE Product;
	BYTE Reply;
	BYTE ID;
	BYTE Flags;
	BYTE RetryRate;
	BYTE Retrys;
	BYTE ForLaterUse[4];
	DWORD Sig;
	BYTE Data[];
	} HEADER,*PHEADER;

#define ERROR_GENERAL                  0x20000000
#define ERROR_NOT_AUTHORIZED           0x2000007f
#define ERROR_CERT_REJECTED_NO_TRUST   0x20000086
#define ERROR_CERT_REJECTED_BAD_FORMAT 0x20000087
#define ERROR_CERT_REJECTED_EXPIRED    0x20000088

#define COM_ERROR_BAD_ENCRIPTION       0x4000000b
#define COM_ERROR_NO_ENCRIPTION        0x4000000a
#define UNREFERENCED_PARAMETER(x)   (void) x

// Registry access flags -- not implemented on NetWare -- provided for cross-platform friendliness
#define KEY_QUERY_VALUE         0
#define KEY_SET_VALUE           0
#define KEY_CREATE_SUB_KEY      0
#define KEY_ENUMERATE_SUB_KEYS  0
#define KEY_NOTIFY              0
#define KEY_CREATE_LINK         0
#define KEY_WOW64_32KEY         0
#define KEY_WOW64_64KEY         0
#define KEY_WOW64_RES           0
#define KEY_READ                0
#define KEY_WRITE               0
#define KEY_EXECUTE             0
#define KEY_ALL_ACCESS          0

// Types of data values:
#define REG_NONE 				( 0 )   // No value type
#define REG_SZ 					( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ 			( 2 )   // Unicode nul terminated string (with environment variable references)
#define REG_BINARY 				( 3 )   // Free form binary
#define REG_DWORD 				( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN	( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN	( 5 )   // 32-bit number
#define REG_LINK				( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ 			( 7 )   // Multiple Unicode strings
#define REG_NOTIFY_CHANGE_LAST_SET ( 100 )
#define HKEY_LOCAL_MACHINE          (( HKEY ) 0x80000002 )
#define HKEY_CURRENT_USER        (( HKEY ) 0x80000002 )

extern DWORD RegOpenKey(HKEY base,LPCSTR key,PHKEY out);
extern DWORD RegCreateKey(HKEY base,LPCSTR key,PHKEY out);
extern DWORD RegDeleteKey(HKEY base,LPCSTR key);
extern DWORD RegDeleteValue(HKEY base,LPCSTR value);
extern DWORD RegCloseKey(HKEY hkey);
extern DWORD RegSetValueEx(HKEY hkey,LPCSTR value,DWORD reserved,DWORD type,const BYTE* data,DWORD cbdata);
extern DWORD RegQueryValueEx(HKEY hkey,LPCSTR value,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
extern DWORD RegEnumValue(HKEY  hkey,DWORD index,LPSTR value,LPDWORD cbvalue,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);
extern DWORD RegEnumKeyEx(HKEY  hkey,DWORD index,LPSTR key,LPDWORD cbkey,LPDWORD reserved,LPSTR Class,LPDWORD cbClass,PFILETIME time);
extern DWORD RegEnumKey(HKEY  hkey,DWORD index,LPSTR key,DWORD cbkey);
extern DWORD RegFlushKey(HKEY hkey);
extern DWORD RegCloseDatabase(BOOL);
extern DWORD RegOpenDatabase(void);
extern DWORD RegSaveDatabase(void);
extern DWORD RegNotifyChangeKeyValue(HKEY hkey,BOOL fWatchSubTree,DWORD fdwNotifyFilter,HANDLE  hEvent,BOOL  fAsync);
extern DWORD DuplicateHandle(DWORD inProcess,HKEY in,DWORD outProcess,HKEY *out,DWORD access,BOOL inherant,DWORD options);
extern DWORD RegOpenKeyEx(HKEY base,LPCSTR key,DWORD res,DWORD sec,HKEY *out);
extern BOOL RawDumpDatabase(char *,size_t);
extern BOOL FileDumpDatabase(HKEY,char *,size_t);

#ifdef __cplusplus
}
#endif

class ClibContext {
public:
	static void mk();
	static void rm();

	ClibContext();
   ~ClibContext();

private:
	static HANDLE mutex;

   ClibContext(const ClibContext& that);
   ClibContext& operator=(const ClibContext& that);
};

extern "C" void ConsolePrintf( const char *format, ... );
#endif // _libcnlm_h_
